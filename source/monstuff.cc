/*
 *  File:       monstuff.cc
 *  Summary:    Misc monster related functions.
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 *      <5>     31 July 2000   GDL     More Manticore fixes.
 *      <4>     29 July 2000   JDJ     Fixed a bunch of places in mons_pickup where MSLOT_WEAPON
 *                                     was being erroneously used.
 *      <3>     25 July 2000   GDL     Fixed Manticores
 *      <2>     11/23/99       LRH     Upgraded monster AI
 *      <1>     -/--/--        LRH     Created
 */

#include "AppHdr.h"
#include "monstuff.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#ifdef DOS
#include <conio.h>
#endif

#include "externs.h"

#include "beam.h"
#include "debug.h"
#include "fight.h"
#include "itemname.h"
#include "misc.h"
#include "monplace.h"
#include "monspeak.h"
#include "mon-util.h"
#include "mstuff2.h"
#include "player.h"
#include "spells2.h"
#include "stuff.h"
#include "view.h"

static bool handle_special_ability(struct monsters *monster, bolt & beem);
static bool mons_pickup(struct monsters *monster);
static void flag_ench(struct monsters *monster, int p);
static void handle_behavior(struct monsters *monster);
static void mons_in_cloud(struct monsters *monster);
static void monster_move(struct monsters *monster);
static bool plant_spit(struct monsters *monster, struct bolt &pbolt);
static int map_wand_to_mspell(int wand_type);

char mmov_x, mmov_y;

bool curse_an_item(char which, char power)
{
    /* use which later, if I want to curse weapon/gloves whatever
       which, for now: 0 = non-mummy, 1 = mummy (potions as well)
       don't change mitm.special of !odecay */

    /* I think this is wrong??? */
    FixedVector < char, ENDOFPACK > possib;
    char cu = power;

    cu = 0;
    char cu1 = 0;
    char cu2 = 0;
    char cu3 = 0;

    for (cu = 0; cu < ENDOFPACK; cu++)
    {
        possib[cu] = 100;
    }

    for (cu = 0; cu < ENDOFPACK; cu++)
    {
        if (you.inv_quantity[cu] == 0)
            continue;

        cu1 = you.inv_class[cu];

        if (cu1 == OBJ_WEAPONS || cu1 == OBJ_ARMOUR)
        {
            if (you.inv_dam [cu] % 30 >= 25)
                continue; //no randarts
        }

        if (cu1 == OBJ_JEWELLERY)
        {
            if (you.inv_dam[cu] == 200)
                continue;       // no randarts
        }

        if (cu1 == OBJ_WEAPONS || cu1 == OBJ_ARMOUR || cu1 == OBJ_JEWELLERY)
        {
            if (you.inv_plus[cu] >= 130)
                continue;
            possib[cu2] = cu;
            cu2++;
        }

        if (cu1 == OBJ_POTIONS && which == 1)
        {
            if (you.inv_type[cu] == POT_DECAY)
                continue;
            possib[cu2] = cu;
            cu2++;
        }
    }

    if (cu2 == 0)
        return 0;

    do
    {
        cu3 = random2(cu2);
    }
    while (possib[cu3] == 100);


    if (you.inv_class[possib[cu3]] == OBJ_POTIONS)
    {
        /* don't change you.inv_dam (just for fun) */
        you.inv_type[possib[cu3]] = POT_DECAY;
        return 1;
        /* problem: changes large piles of potions */
    }

    you.inv_plus[possib[cu3]] += 100;

    return 1;
}

// who wrote this? for shame! {dlb} --
// I can't be bothered writing an intelligent function,
// so I'll make it ugly:
void monster_blink(struct monsters *monster)
{
    FixedVector < int, 2 > passed;      // for random_near_space f(x) {dlb}

    // XXX: this picks a place based on the player not the monster --bwr
    if (!random_near_space(passed))
        return;

    mgrd[monster->x][monster->y] = NON_MONSTER;

    monster->x = passed[0];
    monster->y = passed[1];

    mgrd[monster->x][monster->y] = monster_index(monster);
}                               // end monster_blink()

// least_two will try and get a space more than one square away
// from the player
bool random_near_space(FixedVector < int, 2 > &passed, bool least_two)
{
    passed[0] = 0;
    passed[1] = 0;

    int blx = passed[0];
    int bly = passed[1];
    int tries = 0;

    do
    {
        blx = you.x_pos - 6 + random2(14);
        bly = you.y_pos - 6 + random2(14);

        tries++;

        if (tries > 149)
            return false;
    }
    while (!see_grid(blx, bly)
           || grd[blx][bly] < DNGN_SHALLOW_WATER
           || mgrd[blx][bly] != NON_MONSTER
           || (you.x_pos == blx && you.y_pos == bly)
           || (least_two && distance(you.x_pos, you.y_pos, blx, bly) <= 2));

    passed[0] = blx;
    passed[1] = bly;

    return true;
}                               // end random_near_space()

static bool habitat_okay( struct monsters *monster, int targ )
{
    bool ret = false;
    int habitat = monster_habitat( monster->type );

    if (mons_flies( monster->type ))
    {
        // flying monsters don't care
        ret = true;
    }
    else if (habitat == DNGN_FLOOR && targ >= DNGN_FLOOR)
    {
        // FLOOR habitat monster going to a non-bad place
        ret = true;
    }
    else if (habitat == DNGN_DEEP_WATER
        && (targ == DNGN_DEEP_WATER || targ == DNGN_SHALLOW_WATER))
    {
        // Water monster to water
        ret = true;
    }
    else if (habitat == DNGN_LAVA && targ == DNGN_LAVA)
    {
        // Lava monster to lava
        ret = true;
    }

    return (ret);
}

// This doesn't really swap places, it just sets the monster's
// position equal to the player (the player has to be moved afterwards).
// It also has a slight problem with the fact the if the player is
// levitating over an inhospitable habitat for the monster the monster
// will be put in a place it normally couldn't go (this could be a
// feature because it prevents insta-killing).  In order to prevent
// that little problem, we go looking for a square for the monster
// to "scatter" to instead... and if we can't find one the monster
// just refuses to be swapped (not a bug, this is intentionally
// avoiding the insta-kill).  Another option is to look a bit
// wider for a vaild square (either by a last attempt blink, or
// by looking at a wider radius)...  insta-killing should be a
// last resort in this function (especially since Tome, Dig, and
// Summoning can be used to set up death traps).  If worse comes
// to worse, at least consider making the Swap spell not work
// when the player is over lava or water (if the player want's to
// swap pets to their death, we can let that go). -- bwr
bool swap_places(struct monsters *monster)
{
    bool swap = false;

    int loc_x = you.x_pos;
    int loc_y = you.y_pos;

    swap = habitat_okay( monster, grd[loc_x][loc_y] );

    // chose an appropiate habitat square at random around the target.
    if (!swap)
    {
        int num_found = 0;
        int temp_x, temp_y;

        for (int x = -1; x <= 1; x++)
        {
            temp_x = you.x_pos + x;
            if (temp_x < 0 || temp_x >= GXM)
                continue;

            for (int y = -1; y <= 1; y++)
            {
                if (x == 0 && y == 0)
                    continue;

                temp_y = you.y_pos + y;
                if (temp_y < 0 || temp_y >= GYM)
                    continue;

                if (mgrd[temp_x][temp_y] == NON_MONSTER
                    && habitat_okay( monster, grd[temp_x][temp_y] ))
                {
                    // Found an appropiate space... check if we
                    // switch the current choice to this one.
                    num_found++;
                    if (one_chance_in(num_found))
                    {
                        loc_x = temp_x;
                        loc_y = temp_y;
                    }
                }
            }
        }

        if (num_found)
            swap = true;
    }

    if (swap)
    {
        mpr("You swap places.");

        mgrd[monster->x][monster->y] = NON_MONSTER;

        monster->x = loc_x;
        monster->y = loc_y;

        mgrd[monster->x][monster->y] = monster_index(monster);
    }
    else
    {
        // Might not be ideal, but it's better that insta-killing
        // the monster... maybe try for a short blinki instead? -- bwr
        simple_monster_message( monster, " resists." );
    }

    return (swap);
}                               // end swap_places()

void print_wounds(struct monsters *monster)
{
    // prevents segfault -- cannot use info[] here {dlb}
    char str_wound[200];
    int  dam_level = MDAM_OKAY;

    if (monster->type == -1)
        return;

    if (monster->hit_points == monster->max_hit_points
        || monster->hit_points < 1)
    {
        return;
    }

    if (monster_descriptor(monster->type, MDSC_NOMSG_WOUNDS))
        return;

    strcpy(str_wound, " is ");

    if (monster->hit_points <= monster->max_hit_points / 6)
    {
        strcat(str_wound, "almost ");
        strcat(str_wound, wounded_damaged(monster->type) ? "destroyed"
                                                         : "dead");
        dam_level = MDAM_ALMOST_DEAD;
    }
    else
    {
        if (monster->hit_points <= monster->max_hit_points / 6)
        {
            strcat(str_wound, "horribly ");
            dam_level = MDAM_HORRIBLY_DAMAGED;
        }
        else if (monster->hit_points <= monster->max_hit_points / 3)
        {
            strcat(str_wound, "heavily " );
            dam_level = MDAM_HEAVILY_DAMAGED;
        }
        else if (monster->hit_points <= 3 * (monster-> max_hit_points / 4))
        {
            strcat(str_wound, "moderately ");
            dam_level = MDAM_MODERATELY_DAMAGED;
        }
        else
        {
            strcat(str_wound, "lightly ");
            dam_level = MDAM_LIGHTLY_DAMAGED;
        }

        strcat(str_wound, wounded_damaged(monster->type) ? "damaged"
                                                         : "wounded");
    }

    strcat(str_wound, ".");
    simple_monster_message(monster, str_wound, MSGCH_MONSTER_DAMAGE, dam_level);
}                               // end print_wounds()

// (true == 'damaged') [constructs, undead, etc.]
// and (false == 'wounded') [living creatures, etc.] {dlb}
bool wounded_damaged(int wound_class)
{
    // this schema needs to be abstracted into real categories {dlb}:
    if (mons_holiness(wound_class) == MH_UNDEAD)
        return true;

    if (wound_class >= MONS_CLAY_GOLEM && wound_class <= MONS_TOENAIL_GOLEM)
        return true;

    if (wound_class == MONS_FUNGUS || wound_class == MONS_ABOMINATION_LARGE
        || wound_class == MONS_PLANT || wound_class == MONS_FIRE_VORTEX
        || wound_class == MONS_ABOMINATION_SMALL
        || wound_class == MONS_DANCING_WEAPON
        || wound_class == MONS_SPATIAL_VORTEX)
    {
        return true;
    }

    return false;
}                               // end wounded_damaged()

//---------------------------------------------------------------
//
// handle_behavior
//
// Uses the monster's behavior flag to adjust the square the
// monster will move towards.
//
//---------------------------------------------------------------
static void handle_behavior(struct monsters *monster)
{
    // single calculation permissible {dlb}
    bool monsterNearby = mons_near(monster);

    switch (monster->behavior)
    {
    case BEH_SLEEP:
        monster->target_x = monster->x;
        monster->target_y = monster->y;
        break;

    case BEH_CHASING_I:
    case BEH_CHASING_II:
        if ((monsterNearby
            && (!you.invis
                || mons_see_invis(monster->type)
                || (grd[you.x_pos][you.y_pos] == DNGN_SHALLOW_WATER
                    && !you.levitation)))
            || (!monsterNearby
                && monster->target_x == monster->x
                && monster->target_y == monster->y))
        {
            monster->target_x = you.x_pos;
            monster->target_y = you.y_pos;
        }
        break;

    // this should be made more rigorous {dlb}
    case BEH_WANDER:
        if ((monster->target_x == monster->x
             && monster->target_y == monster->y) || one_chance_in(20))
        {
            monster->target_x = 10 + random2(GXM - 10);
            monster->target_y = 10 + random2(GYM - 10);
        }
        break;

    case BEH_FLEE:
        if (monsterNearby && (!you.invis || mons_see_invis(monster->type)))
        {
            monster->target_x = you.x_pos;
            monster->target_y = you.y_pos;
        }

        if (monster->hit_points > (monster->max_hit_points / 2))
            monster->behavior = BEH_CHASING_I;
            // change the '= 1' to whatever monster used to be doing
        break;

    case BEH_FIGHT:             // attacking other monster
        if (monster->monster_foe == MHITNOT)
        {
            monster->behavior = BEH_WANDER;     // ?
        }
        else
        {
            monster->target_x = menv[monster->monster_foe].x;
            monster->target_y = menv[monster->monster_foe].y;
        }
        break;

    //To be nice (although not strictly accurate) they should follow you
    // even when they can't see you
    case BEH_ENSLAVED:
        if (you.pet_target != MHITNOT)
        {
            if (monster->monster_foe == MHITNOT)
                monster->monster_foe = you.pet_target;
        }

        // monsters do not attack themselves {dlb}
        if (monster->monster_foe == monster_index(monster))
        {
            monster->monster_foe = MHITNOT;
        }
        else if ((monster->monster_foe != MHITNOT
                    && monster->monster_foe != MHITYOU)
                && menv[monster->monster_foe].behavior == BEH_ENSLAVED)
                // enslaved monsters do not attack other enslaved monsters {dlb}
        {
            monster->monster_foe = MHITNOT;
        }

        // this needs to be fixed for MHITYOU or segfaults {dlb}
        if (monster->monster_foe != MHITNOT
            && ((monsterNearby && mons_near(&menv[monster->monster_foe]))
                || distance(monster->x, menv[monster->monster_foe].x,
                            monster->y, menv[monster->monster_foe].y) == 1))
        {
            monster->target_x = menv[monster->monster_foe].x;
            monster->target_y = menv[monster->monster_foe].y;
        }
        else if (monsterNearby
                 && (monster->target_x == monster->x
                     && monster->target_y == monster->y) || one_chance_in(3))
        {
            // see BEH_WANDER case for my commentary {dlb}:
            if (one_chance_in(7))
            {
                monster->target_x = 10 + random2(GXM - 10);
                monster->target_y = 10 + random2(GYM - 10);

            }
            else
            {
                monster->target_x = you.x_pos;
                monster->target_y = you.y_pos;
            }
        }

        if (!monsterNearby)
        {
            monster->target_x = you.x_pos;
            monster->target_y = you.y_pos;
        }
        break;
    }
}                               // end handle_behavior()

//---------------------------------------------------------------
//
// flag_ench
//
// Handle the result of a spell cast upon a monster. Returns
// true if the monster died.
//
//---------------------------------------------------------------
static void flag_ench(struct monsters *monster, int p)
{
    monster->enchantment[p] = ENCH_NONE;

    if (monster->enchantment[0] == ENCH_NONE
        && monster->enchantment[1] == ENCH_NONE
        && monster->enchantment[2] == ENCH_NONE)
    {
        monster->enchantment1 = 0;
    }
}                               // end flag_ench()

// note that this function *completely* blocks messaging for monsters
// distant or invisible to the player ... look elsewhere for a function
// permitting output of "It" messages for the invisible {dlb}
// utilizes both str_pass[] and info[] so be careful calling it {dlb}:
bool simple_monster_message(struct monsters *monster, const char *event,
                            int channel, int param)
{
    if (mons_near(monster)
        && (monster->enchantment[2] != ENCH_INVIS || player_see_invis()))
    {
        if (monster->type != MONS_DANCING_WEAPON)
        {
            strcpy(info, ptr_monam(monster, 0));
        }
        else
        {
            it_name(monster->inv[MSLOT_WEAPON], 0, str_pass);
            strcpy(info, str_pass);
        }

        strcat(info, event);
        mpr(info, channel, param);
        return true;
    }

    return false;
}                               // end simple_monster_message()

static bool handle_enchantment(struct monsters *monster)
{
    bool died = false;

    if (monster->enchantment1)
    {
        for (int p = 0; p < 3 && !died; p++)
        {
            switch (monster->enchantment[p])
            {
            case ENCH_SLOW:
                if (random2(250) <= 10 + monster->hit_dice)
                {
                    if (monster->speed >= 100)
                        monster->speed = 100 + ((monster->speed - 100) * 2);
                    else
                        monster->speed *= 2;

                    flag_ench(monster, p);
                    simple_monster_message(monster, " seems to speed up.");
                }
                break;

            case ENCH_HASTE:
                if (one_chance_in(20))
                {
                    if (monster->speed >= 100)
                        monster->speed = ((monster->speed - 100) / 2) + 100;
                    else
                        monster->speed /= 2;

                    flag_ench(monster, p);
                    simple_monster_message(monster, " seems to slow down.");
                }
                break;

            case ENCH_FEAR:
                monster->behavior = BEH_FLEE;

                if (random2(150) <= 10 + monster->hit_dice)
                {
                    // reset to monster's original behaviour
                    monster->behavior = BEH_CHASING_I;
                    simple_monster_message(monster,
                                           " seems to regain its courage.");
                    flag_ench(monster, p);
                }
                break;

            case ENCH_CONFUSION:
                monster->behavior = BEH_CONFUSED;
                if (random2(120) < monster->hit_dice + 5)
                {
                    if (monster->type != MONS_BUTTERFLY
                        && monster->type != MONS_FIRE_VORTEX
                        && monster->type != MONS_SPATIAL_VORTEX
                        && monster->type != MONS_BALL_LIGHTNING
                        && monster->type != MONS_VAPOUR)
                    {
                        monster->behavior = BEH_CHASING_I;
                        simple_monster_message(monster,
                                               " seems less confused.");
                        flag_ench(monster, p);
                    }
                }
                break;

            case ENCH_INVIS:
                if ((monster_habitat(monster->type) != DNGN_FLOOR
                        && monster->number == 0)
                    || (monster->type == MONS_AIR_ELEMENTAL
                        && one_chance_in(3)) || one_chance_in(20))
                {
                    // invisible monsters stay invisible
                    if (!mons_flag(monster->type, M_INVIS))
                    {
                        if (monster_habitat(monster->type) == DNGN_FLOOR
                            || monster->number != 1)
                        {
                            flag_ench(monster, p);
                            // Can't use simple_message, because we want
                            //  'a thing appears' (not 'the')
                            // Note: this message not printed if the player
                            //  could already see the monster.
                            if (mons_near(monster)
                                && (!player_see_invis()
                                    || monster_habitat(monster->type)
                                                            != DNGN_FLOOR))
                            {
                                strcpy(info, ptr_monam(monster, 2));
                                strcat(info, " appears!");
                                mpr(info);
                            }
                        }
                    }
                }
                break;

            case ENCH_POISON_I:
            case ENCH_POISON_II:
            case ENCH_POISON_III:
            case ENCH_POISON_IV:
            case ENCH_YOUR_POISON_I:
            case ENCH_YOUR_POISON_II:
            case ENCH_YOUR_POISON_III:
            case ENCH_YOUR_POISON_IV:
                {
                    int poisonval = monster->enchantment[p] % 50;

                    if (coinflip())
                        hurt_monster(monster, 1 + random2(poisonval - 3));

                    if (mons_res_poison(monster->type) < 0)
                    {
                        hurt_monster(monster, 1 + random2(poisonval - 5)
                                                + random2(poisonval - 5));
                    }

                    if (monster->hit_points < 1)
                    {
                        monster_die(monster,
                                    ((monster->enchantment[p] < ENCH_POISON_I)
                                                ? KILL_YOU : KILL_MISC), 0);
                        died = true;
                    }

                    if (one_chance_in(5))
                    {
                        monster->enchantment[p]--;      //= 0;

                        if (monster->enchantment[p] % 50 == 6)
                            flag_ench(monster, p);
                    }
                }
                break;

            case ENCH_YOUR_ROT_I:
                if (one_chance_in(4))
                    flag_ench(monster, p);
                else if (monster->hit_points > 1 && one_chance_in(3))
                    hurt_monster(monster, 1);
                break;

            //jmf: FIXME: if (undead) make_small_rot_cloud();
            case ENCH_YOUR_ROT_II:
            case ENCH_YOUR_ROT_III:
            case ENCH_YOUR_ROT_IV:
                if (monster->hit_points > 1 && one_chance_in(3))
                    hurt_monster(monster, 1);
                if (one_chance_in(4))
                    monster->enchantment[p]--;
                break;

            case ENCH_BACKLIGHT_I:
                if (one_chance_in(3))
                    flag_ench(monster, p);
                break;

            case ENCH_BACKLIGHT_II:
            case ENCH_BACKLIGHT_III:
            case ENCH_BACKLIGHT_IV:
                if (one_chance_in(3))
                    monster->enchantment[p]--;
                break;

            // assumption: mons_res_fire has already been checked
            case ENCH_STICKY_FLAME_I:
            case ENCH_STICKY_FLAME_II:
            case ENCH_STICKY_FLAME_III:
            case ENCH_STICKY_FLAME_IV:
            case ENCH_YOUR_STICKY_FLAME_I:
            case ENCH_YOUR_STICKY_FLAME_II:
            case ENCH_YOUR_STICKY_FLAME_III:
            case ENCH_YOUR_STICKY_FLAME_IV:
                hurt_monster(monster, ((1 + random2avg(7, 2)) * 10)
                                                            / monster->speed);

                if (mons_res_fire(monster->type) == -1)
                {
                    hurt_monster(monster, ((1 + random2avg(9, 2)) * 10)
                                                            / monster->speed);
                }

                simple_monster_message(monster, " burns!");

                if (monster->hit_points < 1)
                {
                    monster_die(monster,
                            ((monster->enchantment[p] < ENCH_STICKY_FLAME_I)
                                        ? KILL_YOU : KILL_MISC), 0);
                    died = true;
                }

                if (coinflip())
                {
                    monster->enchantment[p]--;  //= 0;

                    if (monster->enchantment[p] == ENCH_STICKY_FLAME_I - 1
                        || monster->enchantment[p] == ENCH_YOUR_STICKY_FLAME_I - 1)
                    {
                        flag_ench(monster, p);
                    }
                }
                break;

            // 19 is taken by summoning:
            // If these are changed, must also change abjuration
            case ENCH_ABJ_I:
            case ENCH_ABJ_II:
            case ENCH_ABJ_III:
            case ENCH_ABJ_IV:
            case ENCH_ABJ_V:
            case ENCH_ABJ_VI:
            case ENCH_FRIEND_ABJ_I:
            case ENCH_FRIEND_ABJ_II:
            case ENCH_FRIEND_ABJ_III:
            case ENCH_FRIEND_ABJ_IV:
            case ENCH_FRIEND_ABJ_V:
            case ENCH_FRIEND_ABJ_VI:
                if (one_chance_in(10))
                    monster->enchantment[p]--;

                if (monster->enchantment[p] == ENCH_ABJ_I - 1
                    || monster->enchantment[p] == ENCH_FRIEND_ABJ_I - 1)
                {
                    monster_die(monster, KILL_RESET, 0);
                    died = true;
                }
                break;

            case ENCH_CHARM:
                if (monster->behavior != BEH_ENSLAVED
                    || random2(500) <= 10 + monster->hit_dice)
                {
                    if (monster->behavior == BEH_ENSLAVED)
                        simple_monster_message(monster,
                            " is no longer your friend.");
                    // reset to monster's original behaviour
                    monster->behavior = BEH_CHASING_I;
                    flag_ench(monster, p);
                }
                break;

            case ENCH_GLOWING_SHAPESHIFTER:     // this ench never runs out
                if (monster->type == MONS_GLOWING_SHAPESHIFTER
                    || one_chance_in(4))
                {
                    monster_polymorph(monster, RANDOM_MONSTER, 0);
                }
                break;

            case ENCH_SHAPESHIFTER:     // this ench never runs out
                if (monster->type == MONS_SHAPESHIFTER || one_chance_in(15))
                    monster_polymorph(monster, RANDOM_MONSTER, 0);
                break;

            case ENCH_TP_I:
                monster_teleport(monster, true);
                flag_ench(monster, p);
                break;

            case ENCH_TP_II:
            case ENCH_TP_III:
            case ENCH_TP_IV:
                monster->enchantment[p]--;
                break;

            case ENCH_SLEEP_WARY:
                if (one_chance_in(20))
                    flag_ench(monster, p);
                break;
            }
        }
    }

    return died;
}                               // end handle_enchantment()

//---------------------------------------------------------------
//
// handle_movement
//
// Move the monster close to its target square.
//
//---------------------------------------------------------------
static void handle_movement(struct monsters *monster)
{
    // move the monster:
    if (monster->behavior == BEH_FLEE)
    {
        mmov_x = ((monster->target_x > monster->x) ? -1 :
                  (monster->target_x < monster->x) ? 1 : 0);

        mmov_y = ((monster->target_y > monster->y) ? -1 :
                  (monster->target_y < monster->y) ? 1 : 0);
    }
    else
    {
        mmov_x = ((monster->target_x > monster->x) ? 1 :
                  (monster->target_x < monster->x) ? -1 : 0);

        mmov_y = ((monster->target_y > monster->y) ? 1 :
                  (monster->target_y < monster->y) ? -1 : 0);
    }

    // bounds check: don't let fleeing monsters try to run
    // off the map
    if (monster->target_x + mmov_x < 0 || monster->target_x + mmov_x >= GXM)
        mmov_x = 0;
    if (monster->target_y + mmov_y < 0 || monster->target_y + mmov_y >= GYM)
        mmov_y = 0;

    // see if we attacked another monster
    int mnt = mgrd[monster->x + mmov_x][monster->y + mmov_y];

    if (mnt != NON_MONSTER && mnt != monster_index(monster))
    {

        if (monster->behavior == BEH_ENSLAVED
            && menv[mnt].behavior != BEH_ENSLAVED)
        {
            monster->monster_foe = mnt;
        }

        if (monster->behavior != BEH_ENSLAVED
            && menv[mnt].behavior == BEH_ENSLAVED)
        {
            monster->monster_foe = mnt;
        }
    }
}                               // end handle_movement()

//---------------------------------------------------------------
//
// handle_nearby_ability
//
// Gives monsters a chance to use a special ability when they're
// next to the player.
//
//---------------------------------------------------------------
static void handle_nearby_ability(struct monsters *monster)
{

    if (mons_near(monster) && monster->behavior != BEH_SLEEP)
    {
        if (mons_flag(monster->type, M_SPEAKS) && one_chance_in(21))
            mons_speaks(monster);

        switch (monster->type)
        {
        case MONS_SPATIAL_VORTEX:
        case MONS_KILLER_KLOWN:
            // I think it is used for colouring {dlb}
            monster->number = random_colour();
            break;

        case MONS_GIANT_EYEBALL:
            if (coinflip())
            {
                simple_monster_message(monster, " stares at you.");

                if (you.paralysis < 10)
                    you.paralysis += 2 + random2(3);
            }
            break;

        case MONS_EYE_OF_DRAINING:
            if (coinflip())
            {
                simple_monster_message(monster, " stares at you.");

                dec_mp(2 + random2avg(13, 3));

                heal_monster(monster, 5, true); // heh heh {dlb}
            }
            break;

        case MONS_LAVA_WORM:
        case MONS_LAVA_FISH:
        case MONS_LAVA_SNAKE:
//      "another lava thing" has no stats! (GDL)
//        case MONS_ANOTHER_LAVA_THING:
        case MONS_BIG_FISH:
        case MONS_GIANT_GOLDFISH:
        case MONS_ELECTRICAL_EEL:
        case MONS_JELLYFISH:
        case MONS_WATER_ELEMENTAL:
        case MONS_SWAMP_WORM:
            if (grd[monster->x][monster->y] == DNGN_SHALLOW_WATER
                || grd[monster->x][monster->y] == DNGN_BLUE_FOUNTAIN)
            {
                monster->number = 0;
                monster->enchantment[2] = ENCH_NONE;
            }
            else if (monster->number == 0
                     && grd[monster->x][monster->y] != DNGN_SHALLOW_WATER
                     && grd[monster->x][monster->y] != DNGN_BLUE_FOUNTAIN)
            {                   // visible
                if (one_chance_in(5))
                {
                    monster->number = 1;
                    monster->enchantment[2] = ENCH_INVIS;
                    monster->enchantment1 = 1;
                }

            }
            else if ((monster->x != you.x_pos || monster->y != you.y_pos)
                     && one_chance_in(10))
            {
                monster->number = 0;
            }
            break;

        case MONS_AIR_ELEMENTAL:
            if (one_chance_in(5))
            {
                monster->enchantment[2] = ENCH_INVIS;
                monster->enchantment1 = 1;
            }
            break;

        case MONS_PANDEMONIUM_DEMON:
            if (ghost.values[13])
                monster->number = random_colour();
            break;
        }
    }
}                               // end handle_nearby_ability()

//---------------------------------------------------------------
//
// handle_special_ability
//
// $$$ not sure what to say here...
//
//---------------------------------------------------------------
static bool handle_special_ability(struct monsters *monster, bolt & beem)
{
    bool used = false;

    FixedArray < unsigned int, 19, 19 > show;

//    losight(show, grd, you.x_pos, you.y_pos);

    switch (monster->type)
    {
    case MONS_LAVA_SNAKE:
        if (monster->behavior == BEH_CONFUSED)
            break;

        if (you.invis && !mons_see_invis(monster->type))
            break;

        if (monster->number == 1 || monster->enchantment[2] == ENCH_INVIS
            || coinflip())
        {
            break;
        }

        // setup tracer
        strcpy(beem.beam_name, "glob of lava");
        beem.range = 4;
        beem.rangeMax = 13;
        beem.damage = 108;
        beem.colour = RED;
        beem.type = SYM_ZAP;
        beem.flavour = BEAM_LAVA;
        beem.hit = 20;
        beem.beam_source = monster_index(monster);
        beem.thrower = KILL_MON;

        // fire tracer
        fire_tracer(monster, beem);

        // good idea?
        if (mons_should_fire(beem))
        {
            simple_monster_message(monster, " spits lava!");
            beam(beem);
            used = true;
        }
        break;

    case MONS_ELECTRICAL_EEL:
        if (monster->behavior == BEH_CONFUSED)
            break;

        if (you.invis && !mons_see_invis(monster->type))
            break;

        if (monster->number == 1 || monster->enchantment[2] == ENCH_INVIS)
            break;

        // setup tracer
        strcpy(beem.beam_name, "bolt of electricity");
        beem.damage = 105;
        beem.colour = LIGHTCYAN;
        beem.type = SYM_ZAP;
        beem.flavour = BEAM_ELECTRICITY;
        beem.hit = 150;
        beem.beam_source = monster_index(monster);
        beem.thrower = KILL_MON;
        beem.range = 4;
        beem.rangeMax = 13;
        beem.isBeam = true;

        // fire tracer
        fire_tracer(monster, beem);

        // good idea?
        if (mons_should_fire(beem))
        {
            simple_monster_message(monster, " shoots out a bolt of electricity!");
            beam(beem);
            used = true;
        }
        break;

    case MONS_OKLOB_PLANT:
        if (monster->behavior == BEH_CONFUSED)
            break;

        if (you.invis && !mons_see_invis(monster->type))
            break;

        if (one_chance_in(3))
            used = plant_spit(monster, beem);

        break;

    case MONS_PIT_FIEND:
        if (one_chance_in(3))
            break;
        // deliberate fall through
    case MONS_FIEND:
        if (monster->behavior == BEH_CONFUSED)
            break;

        if (one_chance_in(4))
        {
            int spell_cast;

            switch (random2(4))
            {
            case 0:
                spell_cast = MS_TORMENT;
                /* beem should be irrelevant here */
                mons_cast(monster, beem, spell_cast);
                used = true;
                break;
            case 1:
            case 2:
            case 3:
                spell_cast = MS_HELLFIRE;
                setup_mons_cast(monster, beem, spell_cast);

                // fire tracer
                fire_tracer(monster, beem);

                // good idea?
                if (mons_should_fire(beem))
                {
                    simple_monster_message(monster, " makes a gesture!");
                    mons_cast(monster, beem, spell_cast);
                    used = true;
                }
                break;
            }

            mmov_x = 0;
            mmov_y = 0;
        }
        break;

    case MONS_IMP:
    case MONS_PHANTOM:
    case MONS_INSUBSTANTIAL_WISP:
    case MONS_BLINK_FROG:
        if (one_chance_in(7))
        {
            simple_monster_message(monster, " blinks.");
            monster_blink(monster);
        }
        break;

    case MONS_MANTICORE:
        if (you.invis && !mons_see_invis(monster->type))
            break;

        if (monster->behavior == BEH_CONFUSED)
            break;

        if (!mons_near(monster))
            break;

        // the fewer spikes the manticore has left,  the less
        // likely it will use them.
        if (random2(16) >= monster->number)
            break;

        // do the throwing right here,  since the beam is so
        // easy to set up and doesn't involve inventory.

        // set up the beam
        strcpy(beem.beam_name, "volley of spikes");
        beem.range = 9;
        beem.rangeMax = 9;
        beem.hit = 14;
        beem.damage = 10;
        beem.beam_source = monster_index(monster);
        beem.type = SYM_MISSILE;
        beem.colour = LIGHTGREY;
        beem.flavour = BEAM_MISSILE;
        beem.thrower = KILL_MON;

        // fire tracer
        fire_tracer(monster, beem);

        // good idea?
        if (mons_should_fire(beem))
        {
            simple_monster_message(monster, " flicks its tail!");
            beam(beem);
            used = true;
            // decrement # of volleys left
            monster->number -= 1;
        }
        break;

    // dragon breath weapon:
    case MONS_DRAGON:
    case MONS_HELL_HOUND:
    case MONS_ICE_DRAGON:
    case MONS_LINDWORM:
    case MONS_FIREDRAKE:
    case MONS_XTAHUA:
        if (you.invis && !mons_see_invis(monster->type))
            break;

        if (monster->behavior == BEH_CONFUSED)
            break;

        if ((monster->type != MONS_HELL_HOUND && random2(13) < 3)
            || one_chance_in(10))
        {
            setup_dragon(monster, beem);

            // fire tracer
            fire_tracer(monster, beem);

            // good idea?
            if (mons_should_fire(beem))
            {
                simple_monster_message(monster, " breathes.");
                beam(beem);
                mmov_x = 0;
                used = true;
            }
        }
        break;
    }

    return used;
}                               // end handle_special_ability()

//---------------------------------------------------------------
//
// handle_potion
//
// Give the monster a chance to quaff a potion. Returns true if
// the monster imbibed.
//
//---------------------------------------------------------------
static bool handle_potion(struct monsters *monster, bolt & beem)
{

    // yes, there is a logic to this ordering {dlb}:
    if (monster->behavior == BEH_SLEEP)
        return false;
    else if (monster->inv[MSLOT_POTION] == NON_ITEM)
        return false;
    else if (!one_chance_in(3))
        return false;
    else
    {
        bool imbibed = false;

        switch (mitm.sub_type[monster->inv[MSLOT_POTION]])
        {
        case POT_HEALING:
        case POT_HEAL_WOUNDS:
            if (monster->hit_points <= monster->max_hit_points / 2)
            {
                simple_monster_message(monster, " drinks a potion.");

                if (heal_monster(monster, 5 + random2(7), false))
                    simple_monster_message(monster, " is healed!");

                if (mitm.sub_type[monster->inv[MSLOT_POTION]]
                                                    == POT_HEAL_WOUNDS)
                {
                    heal_monster(monster, 10 + random2avg(28, 3), false);
                }

                imbibed = true;
            }
            break;

        case POT_SPEED:
            // notice that these are the same odd colours used in
            // mons_ench_f2() {dlb}
            beem.colour = BLUE;
            // intentional fall through
        case POT_INVISIBILITY:
            if (mitm.sub_type[monster->inv[MSLOT_POTION]] == POT_INVISIBILITY)
                beem.colour = MAGENTA;

            // why only drink these if not near player? {dlb}
            if (!mons_near(monster))
            {
                simple_monster_message(monster, " drinks a potion.");

                mons_ench_f2(monster, beem);

                imbibed = true;
            }
            break;
        }

        if (imbibed)
        {
            mitm.quantity[monster->inv[MSLOT_POTION]]--;

            if (mitm.quantity[monster->inv[MSLOT_POTION]] < 1)
                destroy_item(monster->inv[MSLOT_POTION]);
        }

        return imbibed;
    }
}                               // end handle_potion()

static bool handle_reaching(struct monsters *monster)
{
    bool ret = false;

    if (monster->inv[MSLOT_WEAPON] != NON_ITEM
        && mitm.base_type[monster->inv[MSLOT_WEAPON]] == OBJ_WEAPONS
        && mitm.special[monster->inv[MSLOT_WEAPON]] < NWPN_SINGING_SWORD
        && mitm.special[monster->inv[MSLOT_WEAPON]] % 30 == SPWPN_REACHING)
    {
        int dx = abs(monster->x - you.x_pos);
        int dy = abs(monster->y - you.y_pos);

        if ((dx == 2 && dy <= 2) || (dy == 2 && dx <= 2))
        {
            ret = true;
            monster_attack(monster_index(monster));
        }
    }

    return ret;
}                               // end handle_reaching()

//---------------------------------------------------------------
//
// handle_scroll
//
// Give the monster a chance to read a scroll. Returns true if
// the monster read something.
//
//---------------------------------------------------------------
static bool handle_scroll(struct monsters *monster)
{
    // yes, there is a logic to this ordering {dlb}:
    if (monster->behavior == BEH_CONFUSED || monster->behavior == BEH_SLEEP)
        return false;
    else if (monster->inv[MSLOT_SCROLL] == NON_ITEM)
        return false;
    else if (!one_chance_in(3))
        return false;
    else
    {
        bool read = false;

        // notice how few cases are actually accounted for here {dlb}:
        switch (mitm.sub_type[monster->inv[MSLOT_SCROLL]])
        {
        case SCR_TELEPORTATION:
            if (monster->enchantment[0] != ENCH_TP_I
                && monster->enchantment[1] != ENCH_TP_I
                && monster->enchantment[2] != ENCH_TP_I)
            {
                if (monster->behavior == BEH_FLEE)
                {
                    simple_monster_message(monster, " reads a scroll.");
                    monster_teleport(monster, false);
                    read = true;
                }
            }
            break;

        case SCR_BLINKING:
            if (monster->behavior == BEH_FLEE)
            {
                if (mons_near(monster))
                {
                    simple_monster_message(monster, " reads a scroll.");
                    simple_monster_message(monster, " blinks!");
                    monster_blink(monster);
                    read = true;
                }
            }
            break;

        case SCR_SUMMONING:
            if (mons_near(monster))
            {
                simple_monster_message(monster, " reads a scroll.");
                create_monster(MONS_ABOMINATION_SMALL, 21, monster->behavior,
                           monster->x, monster->y, monster->monster_foe, 250);
                read = true;
            }
            break;
        }

        if (read)
        {
            mitm.quantity[monster->inv[MSLOT_SCROLL]]--;

            if (mitm.quantity[monster->inv[MSLOT_SCROLL]] < 1)
                destroy_item(monster->inv[MSLOT_SCROLL]);
        }

        return read;
    }
}                               // end handle_scroll()

//---------------------------------------------------------------
//
// handle_wand
//
// Give the monster a chance to zap a wand. Returns true if the
// monster zapped.
//
//---------------------------------------------------------------
static bool handle_wand(struct monsters *monster, bolt & beem)
{
    // yes, there is a logic to this ordering {dlb}:
    if (monster->behavior == BEH_SLEEP)
        return false;
    else if (monster->behavior == BEH_CHASING_II && !mons_near(monster))
        return false;
    else if ((monster->behavior == BEH_FIGHT
                || monster->behavior == BEH_ENSLAVED)
            && monster->monster_foe == MHITNOT)
    {
        return false;
    }
    else if (monster->inv[MSLOT_WAND] == NON_ITEM
             || mitm.pluses[monster->inv[MSLOT_WAND]] <= 0)
    {
        return false;
    }
    else if (coinflip())
        return false;
    else
    {
        bool niceWand = false;
        bool zap = true;

        switch (mitm.sub_type[monster->inv[MSLOT_WAND]])
        {
            // these have been deemed "to tricky" at this time {dlb}:
        case WAND_POLYMORPH_OTHER:
        case WAND_ENSLAVEMENT:
        case WAND_DIGGING:
        case WAND_RANDOM_EFFECTS:
            return false;
            break;

        // these are wands that monsters will aim at themselves {dlb}:
        case WAND_HASTING:
            if (monster->enchantment[0] != ENCH_HASTE
                && monster->enchantment[1] != ENCH_HASTE
                && monster->enchantment[2] == ENCH_HASTE)
            {
                beem.target_x = monster->x;
                beem.target_y = monster->y;

                niceWand = true;
            }
            break;

        case WAND_HEALING:
            if (monster->hit_points <= monster->max_hit_points / 2)
            {
                beem.target_x = monster->x;
                beem.target_y = monster->y;

                niceWand = true;
            }
            break;

        case WAND_INVISIBILITY:
            if (monster->enchantment[2] != ENCH_INVIS)
            {
                beem.target_x = monster->x;
                beem.target_y = monster->y;
                niceWand = true;
            }
            break;

        case WAND_TELEPORTATION:
            if (monster->hit_points <= monster->max_hit_points / 2)
            {
                if (monster->enchantment[0] != ENCH_TP_I
                    && monster->enchantment[1] != ENCH_TP_I
                    && monster->enchantment[2] != ENCH_TP_I)
                {
                    beem.target_x = monster->x;
                    beem.target_y = monster->y;

                    niceWand = true;
                }
            }
            break;
        }            // end "switch mitm.sub_type[monster->inv[MSLOT_WAND]]"

        // map wand type to monster spell type
        int mzap = map_wand_to_mspell(mitm.sub_type[monster->inv[MSLOT_WAND]]);
        if (mzap == 0)
            return false;

        // set up the beam
        int power = 25;
        struct SBeam theBeam = mons_spells(mzap, power);

        strcpy(beem.beam_name, theBeam.name.c_str());
        beem.beam_source = monster_index(monster);
        beem.colour = theBeam.colour;
        beem.range = theBeam.range;
        beem.rangeMax = theBeam.rangeMax;
        beem.damage = theBeam.damage;
        beem.ench_power = beem.damage;
        beem.hit = theBeam.hit;
        beem.type = theBeam.type;
        beem.flavour = theBeam.flavour;
        beem.thrower = theBeam.thrown;
        beem.isBeam = theBeam.isBeam;

        // fire tracer, if necessary
        if (!niceWand)
            fire_tracer(monster, beem);

        // good idea?
        zap = mons_should_fire(beem);
        if (niceWand || zap)
        {
            if (!simple_monster_message(monster, " zaps a wand."))
            {
                if (!silenced(you.x_pos, you.y_pos))
                    mpr("You hear a zap.");
            }

            // charge expenditure {dlb}
            mitm.pluses[monster->inv[MSLOT_WAND]]--;
            beam(beem);
            zap = true;
        }

        return zap;
    }
}                               // end handle_wand()

//---------------------------------------------------------------
//
// handle_spell
//
// Give the monster a chance to cast a spell. Returns true if
// a spell was cast.
//
//---------------------------------------------------------------
static bool handle_spell(struct monsters *monster, bolt & beem)
{
    int temp_rand = 0;          // probability determination {dlb}
    bool monsterNearby = mons_near(monster);
    bool finalAnswer = false;   // as in: "Is that your...?" {dlb}

    // yes, there is a logic to this ordering {dlb}:
    if (monster->behavior == BEH_SLEEP
        || !mons_flag(monster->type, M_SPELLCASTER))
    {
        return false;
    }

    if ((mons_flag(monster->type, M_ACTUAL_SPELLS)
            || mons_flag(monster->type, M_PRIEST))
        && (monster->enchantment[1] == ENCH_GLOWING_SHAPESHIFTER
            || monster->enchantment[1] == ENCH_SHAPESHIFTER))
    {
        return false;           //jmf: shapeshiftes don't get spells, just
                                //     physical powers.
    }
    else if (monster->behavior == BEH_CONFUSED && monster->type != MONS_VAPOUR)
        return false;
    else if ((monster->behavior == BEH_FIGHT
                 || monster->behavior == BEH_ENSLAVED)
             && monster->monster_foe == MHITNOT)
    {
        return false;
    }
    else if (monster->type == MONS_PANDEMONIUM_DEMON && ghost.values[9] == 0)
        return false;
    else if (random2(200) > 50 + monster->hit_dice)
        return false;
    else
    {
        int spell_cast = MS_NO_SPELL;
        int hspell_pass[6] = { MS_NO_SPELL, MS_NO_SPELL, MS_NO_SPELL,
            MS_NO_SPELL, MS_NO_SPELL, MS_NO_SPELL };

        int msecc = ((monster->type == MONS_HELLION)           ? MST_BURNING_DEVIL :
                     (monster->type == MONS_PANDEMONIUM_DEMON) ? MST_GHOST
                                                            : monster->number);

        mons_spell_list(msecc, hspell_pass);

        // forces the casting of dig when player not visible - this is EVIL!
        if (!monsterNearby)
        {
            if (hspell_pass[4] == MS_DIG
                && monster->behavior == BEH_CHASING_I)
            {
                spell_cast = MS_DIG;
                finalAnswer = true;
            }
            else if (monster->monster_foe == MHITYOU)
                return false;
        }

        if (!finalAnswer)
        {
            // should monster not have selected dig by now, it never will:
            if (hspell_pass[4] == MS_DIG)
                hspell_pass[4] = MS_NO_SPELL;

            if (monster->behavior == BEH_FLEE)
            {
                spell_cast = (one_chance_in(5) ? MS_NO_SPELL : hspell_pass[5]);
            }
            else
            {
                // up to four tries to pick a spell.
                for (int loopy = 0; loopy < 4; loopy ++)
                {
                    // setup spell
                    spell_cast = hspell_pass[random2(5)];
                    if (spell_cast == MS_NO_SPELL)
                        continue;

                    // setup the spell
                    setup_mons_cast(monster, beem, spell_cast);

                    // do we need a tracer?
                    if (ms_always_fire(spell_cast))
                        break;

                    // fire tracer
                    fire_tracer(monster, beem);

                    // good idea?
                    if (mons_should_fire(beem))
                        break;
                    spell_cast = MS_NO_SPELL;

                    // ok, maybe we'll cast a defensive spell
                    if (coinflip())
                        spell_cast = hspell_pass[2];
                }
            }
        }

        // should the monster *still* not have a spell, well, too bad {dlb}:
        if (spell_cast == MS_NO_SPELL)
            return false;

        // can't see anything to animate:
        if (spell_cast == MS_ANIMATE_DEAD
            && (!monsterNearby || !animate_dead(100, monster->behavior,
                                                monster->monster_foe, 0)))
        {
            return false;
        }

        if (monsterNearby)      // handle monsters within range of player
        {
            switch (monster->type)
            {
            default:
                if (silenced(monster->x, monster->y))
                    return false;

                if (mons_flag(monster->type, M_PRIEST))
                {
                    simple_monster_message(monster, " utters an invocation.",
                                           MSGCH_MONSTER_SPELL);
                }
                else
                {
                    simple_monster_message(monster, " casts a spell.",
                                           MSGCH_MONSTER_SPELL);
                }
                break;

            case MONS_STEAM_DRAGON:
            case MONS_MOTTLED_DRAGON:
            case MONS_STORM_DRAGON:
            case MONS_GOLDEN_DRAGON:
            case MONS_SHADOW_DRAGON:
            case MONS_SWAMP_DRAGON:
            case MONS_SWAMP_DRAKE:
            case MONS_HELL_HOG:
            case MONS_SERPENT_OF_HELL:
            case MONS_QUICKSILVER_DRAGON:
            case MONS_IRON_DRAGON:
                if (!simple_monster_message(monster, " breathes.",
                                            MSGCH_MONSTER_SPELL))
                {
                    if (!silenced(monster->x, monster->y)
                        && !silenced(you.x_pos, you.y_pos))
                    {
                        mpr("You hear a roar.", MSGCH_MONSTER_SPELL);
                    }
                }
                break;

            case MONS_GERYON:
                if (silenced(monster->x, monster->y))
                    return false;

                simple_monster_message(monster, " winds a great silver horn.",
                                       MSGCH_MONSTER_SPELL);
                break;

            case MONS_VAPOUR:
                monster->enchantment[2] = ENCH_INVIS;
                break;

            case MONS_BRAIN_WORM:
                break;

            case MONS_GREAT_ORB_OF_EYES:
            case MONS_SHINING_EYE:
            case MONS_EYE_OF_DEVASTATION:
                simple_monster_message(monster, " gazes.", MSGCH_MONSTER_SPELL);
                break;

            case MONS_GIANT_ORANGE_BRAIN:
                simple_monster_message(monster, " pulsates.",
                                       MSGCH_MONSTER_SPELL);
                break;

            case MONS_NAGA:
            case MONS_NAGA_WARRIOR:
                simple_monster_message(monster, " spits poison.",
                                       MSGCH_MONSTER_SPELL);
                break;

            case MONS_EFREET:
            case MONS_HELLION:
            case MONS_ROTTING_DEVIL:
            case MONS_TORMENTOR:
            case MONS_REAPER:
            case MONS_SOUL_EATER:
            case MONS_HAIRY_DEVIL:
            case MONS_ICE_DEVIL:
            case MONS_BLUE_DEVIL:
            case MONS_BEAST:
            case MONS_IRON_DEVIL:
            case MONS_WHITE_IMP:
            case MONS_LEMURE:
            case MONS_UFETUBUS:
            case MONS_MANES:
            case MONS_MIDGE:
            case MONS_NEQOXEC:
            case MONS_ORANGE_DEMON:
            case MONS_HELLWING:
            case MONS_SMOKE_DEMON:
            case MONS_YNOXINUL:
            case MONS_EXECUTIONER:
            case MONS_GREEN_DEATH:
            case MONS_BLUE_DEATH:
            case MONS_BALRUG:
            case MONS_CACODEMON:
                simple_monster_message(monster, " gestures.",
                                       MSGCH_MONSTER_SPELL);
                break;
            }
        }
        else                    // handle far-away monsters
        {
            if (monster->type == MONS_GERYON
                && !silenced(you.x_pos, you.y_pos))
            {
                mpr("You hear a weird and mournful sound.");
            }
        }

        // FINALLY! determine primary spell effects {dlb}:
        if (spell_cast == MS_BLINK && monsterNearby)
            // why only cast blink if nearby? {dlb}
        {
            simple_monster_message(monster, " blinks!");
            monster_blink(monster);
        }
        else
        {
            mons_cast(monster, beem, spell_cast);
            mmov_x = 0;
        }
    } // end "if mons_flag(monster->type, M_SPELLCASTER) ...

    return true;
}                               // end handle_spell()

//---------------------------------------------------------------
//
// handle_throw
//
// Give the monster a chance to throw something. Returns true if
// the monster hurled.
//
//---------------------------------------------------------------
static bool handle_throw(struct monsters *monster, bolt & beem)
{
    // yes, there is a logic to this ordering {dlb}:
    if (monster->behavior == BEH_CONFUSED || monster->behavior == BEH_SLEEP)
        return false;

    if ((monster->behavior == BEH_FIGHT
                || monster->behavior == BEH_ENSLAVED)
            && monster->monster_foe == MHITNOT)
        return false;

    if (!mons_itemuse(monster->type))
        return false;

    if (monster->inv[MSLOT_MISSILE] == NON_ITEM
             || mitm.quantity[monster->inv[MSLOT_MISSILE]] <= 0)
        return false;

    // don't allow offscreen throwing.. for now.
    if (monster->monster_foe == MHITYOU && !mons_near(monster))
        return false;

    if (monster->type == MONS_TWO_HEADED_OGRE) // poor 2-headed ogres {dlb}
        return false;

    // recent addition {GDL} - monsters won't throw if they can do melee.
    // wastes valuable ammo,  and most monsters are better at melee anyway.
    int dx = beem.target_x - monster->x;
    int dy = beem.target_y - monster->y;
    if (abs(dx) < 2 && abs(dy) < 2)
        return false;

    if (one_chance_in(5))
        return false;

    // new (GDL) - don't throw idiotic stuff.  It's a waste of time.
    int mon_item = monster->inv[MSLOT_MISSILE];
    int wepClass = mitm.base_type[mon_item];
    int wepType = mitm.sub_type[mon_item];

    int weapon = monster->inv[MSLOT_WEAPON];
    int lnchClass = weapon != NON_ITEM ? mitm.base_type[weapon] : -1;
    int lnchType = weapon != NON_ITEM ? mitm.sub_type[weapon] : 0;
    bool thrown = false;
    bool launched = false;
    throw_type(lnchClass, lnchType, wepClass, wepType, launched, thrown);
    if (!(launched || thrown))
        return false;

    // ok, we'll try it.
    setup_generic_throw(monster, beem);

    // fire tracer
    fire_tracer(monster, beem);

    // good idea?
    if (mons_should_fire(beem))
    {
        item_name(mitm.pluses2[mon_item], mitm.base_type[mon_item],
                  mitm.sub_type[mon_item], mitm.special[mon_item],
                  mitm.pluses[mon_item], 1, mitm.id[mon_item], 6, str_pass);
        strcpy(beem.beam_name, str_pass);

        return mons_throw(monster, beem, mon_item);
    }
    else
        return false;
}                               // end handle_throw()

//---------------------------------------------------------------
//
// monster
//
// This is the routine that controls monster AI.
//
//---------------------------------------------------------------
void monster(void)
{
    bool bat = false;
    bool brkk = false;
    int monc = 0;

    struct bolt beem;

    FixedArray < unsigned int, 19, 19 > show;

//    losight(show, grd, you.x_pos, you.y_pos);

    for (int i = 0; i < MAX_MONSTERS; i++)
    {
        struct monsters *monster = &menv[i];

        if (monster->type != -1)
        {
            monc++;

            if (monster->hit_points > monster->max_hit_points)
                monster->hit_points = monster->max_hit_points;

            if ((monster->type == MONS_GIANT_SPORE
                    || monster->type == MONS_BALL_LIGHTNING)
                && monster->hit_points < 1)
            {
                // detach monster from the grid first, so it
                // doesn't get hit by its own explosion (GDL)
                mgrd[monster->x][monster->y] = NON_MONSTER;

                spore_goes_pop(monster);
                monster_cleanup(monster);

                continue;
            }

            monster->speed_increment += (monster->speed * you.time_taken / 10);

            if (you.slow > 0)
            {
                monster->speed_increment +=
                                    (monster->speed * you.time_taken / 10);
            }

            while (monster->speed_increment >= 80)
            {                   // The continues & breaks are WRT this.
                if (monster->type != -1 && monster->hit_points < 1)
                    monster_die(monster, KILL_MISC, 0);

                monster->speed_increment -= 10;

                if (env.cgrid[monster->x][monster->y] != EMPTY_CLOUD)
                {
                    if (monster_habitat(monster->type) != DNGN_FLOOR
                        && monster->number == 1)
                    {
                        break;
                    }

                    if (monster->type == -1)
                        break;  // problem with vortices

                    mons_in_cloud(monster);

                    if (monster->type == -1)
                    {
                        monster->speed_increment = 1;
                        break;
                    }
                }

                if (you.invis && !mons_see_invis(monster->type)
                    && monster->target_x == monster->x
                    && monster->target_y == monster->y)
                {
                    monster->behavior = BEH_WANDER;
                }

                if (monster->type == MONS_GLOWING_SHAPESHIFTER)
                    monster->enchantment[1] = ENCH_GLOWING_SHAPESHIFTER;

                // otherwise there are potential problems with summonings
                if (monster->type == MONS_SHAPESHIFTER)
                    monster->enchantment[1] = ENCH_SHAPESHIFTER;

                handle_behavior(monster);

                if (handle_enchantment(monster))
                    goto endbat;

                // regenerate:
                if (monster_descriptor(monster->type, MDSC_REGENERATES)
                    || one_chance_in(25))
                {
                    heal_monster(monster, 1, false);
                }

                if (monster->speed >= 100)
                    continue;

                if (monster->type == MONS_ZOMBIE_SMALL
                    || monster->type == MONS_ZOMBIE_LARGE
                    || monster->type == MONS_SIMULACRUM_SMALL
                    || monster->type == MONS_SIMULACRUM_LARGE
                    || monster->type == MONS_SKELETON_SMALL
                    || monster->type == MONS_SKELETON_LARGE)
                {
                    monster->max_hit_points = monster->hit_points;
                }

                if (igrd[monster->x][monster->y] != NON_ITEM
                    && (mons_itemuse(monster->type) == 3
                        || monster->type == MONS_JELLY
                        || monster->type == MONS_NECROPHAGE
                        || monster->type == MONS_GHOUL))
                {
                    if (mons_pickup(monster))
                        continue;
                }

                handle_movement(monster);

                brkk = false;

                if (monster->behavior == BEH_CONFUSED)
                {
                    mmov_x = random2(3) - 1;
                    mmov_y = random2(3) - 1;

                    // bounds check: don't let confused monsters try to run
                    // off the map
                    if (monster->target_x + mmov_x < 0 || monster->target_x + mmov_x >= GXM)
                        mmov_x = 0;
                    if (monster->target_y + mmov_y < 0 || monster->target_y + mmov_y >= GYM)
                        mmov_y = 0;

                    if (mgrd[monster->x + mmov_x][monster->y + mmov_y]
                                                            != NON_MONSTER
                        && (mmov_x != 0 || mmov_y != 0))
                    {
                        if (monsters_fight(i,
                                mgrd[monster->x + mmov_x][monster->y + mmov_y]))
                        {
                            mmov_x = 0;
                            mmov_y = 0;
                            brkk = true;
                            break;
                        }
                    }
                }

                if (brkk)
                    continue;

                handle_nearby_ability(monster);

                if (((monster->behavior == BEH_CHASING_I
                            || monster->behavior == BEH_FLEE)
                        && mons_near(monster))
                    || ((monster->behavior == BEH_FIGHT
                             || monster->behavior == BEH_ENSLAVED)
                         && monster->monster_foe != MHITNOT))
                {
                    if ((monster->behavior == BEH_CHASING_I
                            || monster->behavior == BEH_FLEE)
                        && mons_near(monster))
                    {
                        you.pet_target = i;
                    }

                    if ((monster->behavior == BEH_FIGHT
                            || monster->behavior == BEH_ENSLAVED)
                        && (monster->monster_foe == MHITNOT
                            || monster->monster_foe == MHITYOU))
                    {
                        goto end_switch;
                    }

                    // VERY IMPORTANT BIT HERE -- set up the beam target.
                    // normally set to monster foe (either player or another
                    // monster),  will be set to acting monster x,y if monster
                    // has no foe.  Which is fine - there's enough checks so
                    // that monsters shouldn't be blowing themselves to bits.

                    if (monster->behavior == BEH_FIGHT
                        || monster->behavior == BEH_ENSLAVED)
                    {
                        if (monster->monster_foe == MHITNOT)
                        {
                            beem.target_x = monster->x;  // aim at self
                            beem.target_y = monster->y;
                        }
                        else
                        {
                            beem.target_x = menv[monster->monster_foe].x;
                            beem.target_y = menv[monster->monster_foe].y;
                        }
                    }
                    else
                    {
                        beem.target_x = you.x_pos;
                        beem.target_y = you.y_pos;
                    }

                    // this seems to prevent monsters from hitting you from
                    // offscreen.
                    if (monster->monster_foe == MHITYOU && !mons_near(monster)
                        && monster->type != MONS_CACODEMON)
                    {
                        goto end_switch;
                    }

                    if (handle_special_ability(monster, beem))
                        continue;

                    if (handle_potion(monster, beem))
                        continue;

                    if (handle_scroll(monster))
                        continue;

                    if (handle_wand(monster, beem))
                        continue;

                    if (handle_spell(monster, beem))
                        continue;

                    if (handle_reaching(monster))
                        continue;

                  end_switch:
                    if (handle_throw(monster, beem))
                        continue;
                }

              end_throw:
                if ((monster->behavior == BEH_ENSLAVED
                        || monster->behavior == BEH_FIGHT)
                    && (mmov_x != 0 || mmov_y != 0)
                    && mgrd[monster->x + mmov_x][monster->y + mmov_y]
                                                            != NON_MONSTER)
                {
                    if (monsters_fight(i,
                            mgrd[monster->x + mmov_x][monster->y + mmov_y]))
                    {
                        if (monster->type == MONS_GIANT_BAT
                            || monster->type == MONS_UNSEEN_HORROR
                            || monster->type == MONS_GIANT_BLOWFLY)
                        {
                            monster->speed_increment -= monster->speed;
                        }

                        mmov_x = 0;
                        mmov_y = 0;
                        brkk = true;
                        break;
                    }
                }

                if (brkk)
                    continue;

                if (monster->x + mmov_x == you.x_pos
                    && monster->y + mmov_y == you.y_pos)
                {
                    if (monster->type == MONS_GIANT_BAT
                        || monster->type == MONS_UNSEEN_HORROR
                        || monster->type == MONS_GIANT_BLOWFLY)
                    {
                        if (!bat)
                            monster_attack(i);

                        if (monster->behavior != BEH_ENSLAVED)
                        {
                            monster->behavior = BEH_WANDER;
                            bat = true;
                        }
                    }
                    else
                        monster_attack(i);

                    if ((monster->type == MONS_GIANT_SPORE
                            || monster->type == MONS_BALL_LIGHTNING)
                        && monster->hit_points < 1)
                    {

                        // detach monster from the grid first, so it
                        // doesn't get hit by its own explosion (GDL)
                        mgrd[monster->x][monster->y] = NON_MONSTER;

                        spore_goes_pop(monster);
                        monster_cleanup(monster);
                        continue;
                    }

                    mmov_x = 0;
                    mmov_y = 0;
                    continue;   //break;
                }

                if (monster->type == -1 || monster->type == MONS_OKLOB_PLANT
                    || monster->type == MONS_CURSE_SKULL
                    || (monster->type >= MONS_CURSE_TOE
                        && monster->type <= MONS_POTION_MIMIC))
                {
                    continue;
                }

                monster_move(monster);

                if (monster->behavior == BEH_CHASING_I)
                {
                    if (mons_near(monster)
                        && (!you.invis || mons_see_invis(monster->type)))
                    {
                        monster->target_x = you.x_pos;
                        monster->target_y = you.y_pos;
                    }
                }

                // Tame monsters can't become afraid, because fear
                // overwrites tameness
                if (monster->hit_points <= (monster->max_hit_points / 4 - 1)
                    && mons_intel(monster->type) > I_ANIMAL
                    && monster->behavior != BEH_ENSLAVED)
                {
                    monster->behavior = BEH_FLEE;
                }
            }                   // end while

          endbat:
            bat = false;

            if (monster->behavior == BEH_FIGHT && !one_chance_in(3))
                monster->behavior = BEH_CHASING_I;      // I think?
        }                       // end of if (mons_class != -1)
    }                           // end of for loop
}                               // end monster()

//---------------------------------------------------------------
//
// mons_pickup
//
// Returns 0 if monster doesn't spend any time pickup up
//
//---------------------------------------------------------------
static bool mons_pickup(struct monsters *monster)
{
    // single calculation permissible {dlb}
    bool monsterNearby = mons_near(monster);

    if (monster->type == MONS_JELLY
        || monster->type == MONS_BROWN_OOZE
        || monster->type == MONS_ACID_BLOB
        || monster->type == MONS_ROYAL_JELLY)
    {
        int hps_gained;
        unsigned int quant_eated = 1 + random2(10); // value range: [1,10] {dlb}

        if (quant_eated > mitm.quantity[igrd[monster->x][monster->y]])
            quant_eated = mitm.quantity[igrd[monster->x][monster->y]];

        if (mitm.base_type[igrd[monster->x][monster->y]] == OBJ_WEAPONS
            && mitm.special[igrd[monster->x][monster->y]] >= NWPN_SINGING_SWORD)
        {
            return false;       /* unique items */
        }

        if (mitm.base_type[igrd[monster->x][monster->y]] == OBJ_MISSILES
            && (mitm.sub_type[igrd[monster->x][monster->y]] == MI_STONE
               || mitm.sub_type[igrd[monster->x][monster->y]] == MI_LARGE_ROCK))
        {
            /* shouldn't eat stone things.
               - but what about stone wands & rings? */
            return false;
        }

        if (mitm.base_type[igrd[monster->x][monster->y]] != OBJ_GOLD)
        {
            hps_gained = quant_eated
                            * mass(mitm.base_type[igrd[monster->x][monster->y]],
                                   mitm.sub_type[igrd[monster->x][monster->y]]);
            hps_gained /= 20;
        }
        else
            hps_gained = 1;  // seems a bit of a trifle not to scale this {dlb}

        if (hps_gained < 1)
            hps_gained = 1;
        else if (hps_gained > 50)
            hps_gained = 50;

        mitm.quantity[igrd[monster->x][monster->y]] -= quant_eated;

        if (mitm.quantity[igrd[monster->x][monster->y]] < 1)
        {
            igrd[monster->x][monster->y] =
                                    mitm.link[igrd[monster->x][monster->y]];
        }

        monster->hit_points += hps_gained;

        // note here, that this makes jellies "grow" {dlb}:
        if (monster->hit_points > monster->max_hit_points)
            monster->max_hit_points = monster->hit_points;

        if (!silenced(you.x_pos, you.y_pos)
            && !silenced(monster->x, monster->y))
        {
            strcpy(info, "You hear a");
            if (!monsterNearby)
                strcat(info, " distant");
            strcat(info, " slurping noise.");
            mpr(info);
        }

        if (monster->hit_points > 50)
        {
            if (!jelly_divide(monster))
            {
                // and here is where the jelly might divide {dlb}
                monster->hit_points = 50;
            }
        }

        return false;
    }                           // end "if jellies"

    switch (mitm.base_type[igrd[monster->x][monster->y]])
    {
    case OBJ_WEAPONS:
        if (monster->inv[MSLOT_WEAPON] != NON_ITEM)
            return false;
        if (mitm.special[igrd[monster->x][monster->y]] >= NWPN_SINGING_SWORD)
            return false;
        if (mitm.special[igrd[monster->x][monster->y]] % 30 >=
            SPWPN_RANDART_I)
            return false;

        // wimpy monsters (Kob, gob) shouldn't pick up halberds etc
        // of course, this also block knives {dlb}:
        if ((mons_charclass(monster->type) == MONS_KOBOLD
                || mons_charclass(monster->type) == MONS_GOBLIN)
            && property(mitm.base_type[igrd[monster->x][monster->y]],
                    mitm.sub_type[igrd[monster->x][monster->y]], PWPN_HIT) <= 0)
        {
            return false;
        }

        // Nobody picks up giant clubs:
        if (mitm.sub_type[igrd[monster->x][monster->y]] == WPN_GIANT_CLUB
            || mitm.sub_type[igrd[monster->x][monster->y]]
                                                    == WPN_GIANT_SPIKED_CLUB)
        {
            return false;
        }

        monster->inv[MSLOT_WEAPON] = igrd[monster->x][monster->y];
        igrd[monster->x][monster->y] = mitm.link[igrd[monster->x][monster->y]];
        mitm.link[monster->inv[MSLOT_WEAPON]] = NON_ITEM;

        if (mitm.special[monster->inv[MSLOT_WEAPON]] % 30 == SPWPN_PROTECTION)
            monster->armor_class += 3;

        if (monsterNearby)
        {
            strcpy(info, ptr_monam(monster, 0));
            strcat(info, " picks up ");
            it_name(monster->inv[MSLOT_WEAPON], 3, str_pass);
            strcat(info, str_pass);
            strcat(info, ".");
            mpr(info);
        }
        break;

    case OBJ_MISSILES:
        /* Removed check for item_plus2 - probably irrelevant */
        if (monster->inv[MSLOT_MISSILE] != NON_ITEM
            && mitm.sub_type[monster->inv[MSLOT_MISSILE]]
                                == mitm.sub_type[igrd[monster->x][monster->y]]
            && mitm.pluses[monster->inv[MSLOT_MISSILE]]
                                == mitm.pluses[igrd[monster->x][monster->y]]
            && mitm.special[monster->inv[MSLOT_MISSILE]]
                                == mitm.special[igrd[monster->x][monster->y]])
        {
            if (monsterNearby)
            {
                strcpy(info, ptr_monam(monster, 0));
                strcat(info, " picks up ");
                it_name(igrd[monster->x][monster->y], 3, str_pass);
                strcat(info, str_pass);
                strcat(info, ".");
                mpr(info);
            }

            mitm.quantity[monster->inv[MSLOT_MISSILE]] +=
                                mitm.quantity[igrd[monster->x][monster->y]];

            mitm.quantity[igrd[monster->x][monster->y]] = 0;

            igrd[monster->x][monster->y] =
                                mitm.link[igrd[monster->x][monster->y]];

            mitm.link[monster->inv[MSLOT_MISSILE]] = NON_ITEM;
            break;
        }

        // nobody bothers to pick up rocks if they don't already have some:
        if (mitm.sub_type[igrd[monster->x][monster->y]] == MI_LARGE_ROCK)
            return false;

        // nobody stoops to pick up just one more of something they
        // already have {dlb}
        if (monster->inv[MSLOT_MISSILE] != NON_ITEM
            || mitm.quantity[igrd[monster->x][monster->y]] == 1)
        {
            return false;
        }

        // monsters with powerful melee attacks don't bother
        if (mondamage(monster->type, 0) > 5)
            return false;

        monster->inv[MSLOT_MISSILE] = igrd[monster->x][monster->y];
        igrd[monster->x][monster->y] = mitm.link[igrd[monster->x][monster->y]];
        mitm.link[monster->inv[MSLOT_MISSILE]] = NON_ITEM;

        if (monsterNearby)
        {
            strcpy(info, ptr_monam(monster, 0));
            strcat(info, " picks up ");
            it_name(monster->inv[MSLOT_MISSILE], 3, str_pass);
            strcat(info, str_pass);
            strcat(info, ".");
            mpr(info);
        }
        break;

    case OBJ_WANDS:
        if (monster->inv[MSLOT_WAND] != NON_ITEM)
            return false;

        monster->inv[MSLOT_WAND] = igrd[monster->x][monster->y];
        igrd[monster->x][monster->y] = mitm.link[igrd[monster->x][monster->y]];
        mitm.link[monster->inv[MSLOT_WAND]] = NON_ITEM;

        if (monsterNearby)
        {
            strcpy(info, ptr_monam(monster, 0));
            strcat(info, " picks up ");
            it_name(monster->inv[MSLOT_WAND], 3, str_pass);
            strcat(info, str_pass);
            strcat(info, ".");
            mpr(info);
        }
        break;

    case OBJ_SCROLLS:
        if (monster->inv[MSLOT_SCROLL] != NON_ITEM)
            return false;

        monster->inv[MSLOT_SCROLL] = igrd[monster->x][monster->y];
        igrd[monster->x][monster->y] = mitm.link[igrd[monster->x][monster->y]];
        mitm.link[monster->inv[MSLOT_SCROLL]] = NON_ITEM;

        if (monsterNearby)
        {
            strcpy(info, ptr_monam(monster, 0));
            strcat(info, " picks up ");
            it_name(monster->inv[MSLOT_SCROLL], 3, str_pass);
            strcat(info, str_pass);
            strcat(info, ".");
            mpr(info);
        }
        break;

    case OBJ_POTIONS:
        if (monster->inv[MSLOT_POTION] != NON_ITEM)
            return false;

        monster->inv[MSLOT_POTION] = igrd[monster->x][monster->y];
        igrd[monster->x][monster->y] = mitm.link[igrd[monster->x][monster->y]];
        mitm.link[monster->inv[MSLOT_POTION]] = NON_ITEM;

        if (monsterNearby)
        {
            strcpy(info, ptr_monam(monster, 0));
            strcat(info, " picks up ");
            it_name(monster->inv[MSLOT_POTION], 3, str_pass);
            strcat(info, str_pass);
            strcat(info, ".");
            mpr(info);
        }
        break;

    case OBJ_CORPSES:
        if (monster->type != MONS_NECROPHAGE && monster->type != MONS_GHOUL)
            return false;

        monster->hit_points +=
            1 + random2(mons_weight(mitm.pluses[igrd[monster->x][monster->y]]))
                                                                        / 100;

        // limited growth factor here -- should 77 really be the cap? {dlb}:
        if (monster->hit_points > 77)
            monster->hit_points = 77;

        if (monster->hit_points > monster->max_hit_points)
            monster->max_hit_points = monster->hit_points;

        if (monsterNearby)
        {
            strcpy(info, ptr_monam(monster, 0));
            strcat(info, " eats ");
            it_name(igrd[monster->x][monster->y], 1, str_pass);
            strcat(info, str_pass);
            strcat(info, ".");
            mpr(info);
        }

        destroy_item(igrd[monster->x][monster->y]);
        break;

    default:
        return false;
    }

    //if (monster->speed_increment > 25)
    //  monster->speed_increment -= monster->speed;
    return true;
}                               // end mons_pickup()

static void monster_move(struct monsters *monster)
{
    bool which_first = coinflip();

    FixedArray < bool, 3, 3 > good_move;
    int count_x, count_y, vacated_x, vacated_y;
    int okmove = DNGN_SHALLOW_WATER;

    // effectively slows down monster movement across water.
    // Fire elementals can't cross at all.
    if (monster->type == MONS_FIRE_ELEMENTAL || one_chance_in(3))
        okmove = DNGN_WATER_STUCK;

    if (mons_flies(monster->type) > 0
        || monster_habitat(monster->type) != DNGN_FLOOR)
    {
        okmove = MINMOVE;
    }

    for (count_x = 0; count_x < 3; count_x++)
    {
        for (count_y = 0; count_y < 3; count_y++)
        {
            good_move[count_x][count_y] = true;
            const int targ_x = monster->x + count_x - 1;
            const int targ_y = monster->y + count_y - 1;

            // bounds check - don't consider moving out of grid!
            if (targ_x < 0 || targ_x >= GXM ||
                targ_y < 0 || targ_y >= GYM)
            {
                good_move[count_x][count_y] = false;
                continue;
            }

            if (monster->type == MONS_BORING_BEETLE)
            {
                if (grd[ targ_x ][ targ_y ] == DNGN_ROCK_WALL
                    && targ_x > 7 && targ_x < (GXM - 8)
                    && targ_y > 7 && targ_y < (GYM - 8))
                {
                    goto tunnel;
                }
            }

            if (grd[ targ_x ][ targ_y ] < okmove)
            {
                good_move[count_x][count_y] = false;
                continue;
            }

          tunnel:
            if (mgrd[ targ_x ][ targ_y ] != NON_MONSTER)
            {
                const int monst = mgrd[ targ_x ][ targ_y ];

                if (!(menv[ monst ].behavior == BEH_ENSLAVED
                     || monster->behavior == BEH_ENSLAVED
                     && menv[ monst ].behavior != monster->behavior))
                {
                    good_move[count_x][count_y] = false;
                    continue;
                }
            }

            const int targ_cloud = env.cgrid[ targ_x ][ targ_y ];
            const int curr_cloud = env.cgrid[ monster->x ][ monster->y ];

            if (targ_cloud != EMPTY_CLOUD)
            {
                if (curr_cloud != EMPTY_CLOUD
                    && env.cloud_type[targ_cloud] == env.cloud_type[curr_cloud])
                {
                    continue;
                }

                if (mons_intel(monster->type) < I_ANIMAL)
                    continue;

                switch (env.cloud_type[ targ_cloud ])
                {
                case CLOUD_FIRE:
                case CLOUD_FIRE_MON:
                    if (mons_res_fire(monster->type) > 0
                        || (monster->inv[MSLOT_ARMOUR] != NON_ITEM
                            && mitm.special[monster->inv[MSLOT_ARMOUR]] % 30
                                                    == SPARM_FIRE_RESISTANCE))
                    {
                        continue;
                    }

                    if (monster->hit_points >= 15 + random2avg(46, 5))
                        continue;
                    break;

                case CLOUD_STINK:
                case CLOUD_STINK_MON:
                    if (mons_res_poison(monster->type) > 0)
                        continue;
                    if (1 + random2(5) < monster->hit_dice)
                        continue;
                    if (monster->hit_points >= random2avg(19, 2))
                        continue;
                    break;

                case CLOUD_COLD:
                case CLOUD_COLD_MON:
                    if (mons_res_cold(monster->type) > 0
                        || (monster->inv[MSLOT_ARMOUR] != NON_ITEM
                            && mitm.special[monster->inv[MSLOT_ARMOUR]] % 30
                                                    == SPARM_COLD_RESISTANCE))
                    {
                        continue;
                    }

                    if (monster->hit_points >= 15 + random2avg(46, 5))
                        continue;
                    break;

                case CLOUD_POISON:
                case CLOUD_POISON_MON:
                    if (mons_res_poison(monster->type) > 0)
                        continue;
                    if (monster->hit_points >= random2avg(37, 4))
                        continue;
                    break;

                // why is this classed as a harmful cloud? {dlb}
                case CLOUD_GREY_SMOKE:
                case CLOUD_GREY_SMOKE_MON:
                    if (mons_res_fire(monster->type) > 0
                        || (monster->inv[MSLOT_ARMOUR] != NON_ITEM
                            && mitm.special[monster->inv[MSLOT_ARMOUR]] % 30
                                                    == SPARM_FIRE_RESISTANCE))
                    {
                        continue;
                    }

                    if (monster->hit_points >= random2avg(19, 2))
                        continue;
                    break;

                default:
                    continue;   // harmless clouds
                }

                good_move[count_x][count_y] = false;
            }
        }
    }

    // this is a cheat, as all monsters know of secret doors
    // even dumb ones like jellies <explicitly included below> {dlb}
    if (grd[monster->x + mmov_x][monster->y + mmov_y] == DNGN_CLOSED_DOOR
        || grd[monster->x + mmov_x][monster->y + mmov_y] == DNGN_SECRET_DOOR)
    {
        if (monster->type == MONS_ZOMBIE_SMALL
            || monster->type == MONS_ZOMBIE_LARGE
            || monster->type == MONS_SIMULACRUM_SMALL
            || monster->type == MONS_SIMULACRUM_LARGE
            || monster->type == MONS_SKELETON_SMALL
            || monster->type == MONS_SKELETON_LARGE
            || monster->type == MONS_SPECTRAL_THING)
        {
            if (mons_itemuse(monster->type) > 0)
                // this is most certainly wrong {dlb}
            {
                grd[monster->x + mmov_x][monster->y + mmov_y] = DNGN_OPEN_DOOR;
                return;
            }
        }
        else if (mons_itemuse(monster->type) > 0)
        {
            grd[monster->x + mmov_x][monster->y + mmov_y] = DNGN_OPEN_DOOR;
            return;
        }

        // this should also be tweaked to include open doors, too
        if (monster->type == MONS_JELLY
            || monster->type == MONS_BROWN_OOZE
            || monster->type == MONS_ACID_BLOB
            || monster->type == MONS_ROYAL_JELLY)
        {
            grd[monster->x + mmov_x][monster->y + mmov_y] = DNGN_FLOOR;

            if (!silenced(you.x_pos, you.y_pos)
                && !silenced(monster->x, monster->y))
            {
                strcpy(info, "You hear a");
                if (!mons_near(monster))
                    strcat(info, " distant");
                strcat(info, " slurping noise.");
                mpr(info);
            }

            monster->hit_points += 5;

            // note here, that this makes jellies "grow" {dlb}:
            if (monster->hit_points > monster->max_hit_points)
                monster->max_hit_points = monster->hit_points;

            if (monster->hit_points > 50)
            {
                // and here is where the jelly might divide {dlb}
                if (!jelly_divide(monster))
                    monster->hit_points = 50;
            }
        }
    }

    /*  equivalent of your move() for monsters: */
    if (mmov_x != 0 && mmov_y != 0)
    {
        // Now, we want to make the monster move in a straight line unless an
        //  oblique line is faster (often, either is optimum above a certain
        //  distance from the target). But should be a little random

        // seems a bit redundant of the above {dlb}
        if (mmov_x != 0 && mmov_y != 0
            && abs(monster->y - monster->target_y)
                            != abs(monster->x - monster->target_x)
            && !one_chance_in(4))
        {
            if (abs(monster->target_x - monster->x)
                            > abs(monster->target_y - monster->y))
            {
                if (good_move[mmov_x + 1][1])
                    mmov_y = 0;
            }
            else if (good_move[1][mmov_y + 1])
                mmov_x = 0;
        }

        if (!(good_move[mmov_x + 1][mmov_y + 1]))
        {
            // now why on earth would this all have to be repeated? {dlb}
            // what about jellies down here? {dlb}

            /* some monsters opening doors */
            if (mons_itemuse(monster->type) > 0
                && (grd[monster->x + mmov_x][monster->y + mmov_y]
                                                    == DNGN_CLOSED_DOOR
                    || grd[monster->x + mmov_x][monster->y + mmov_y]
                                                    == DNGN_SECRET_DOOR))
            {
                if (monster->type == MONS_ZOMBIE_SMALL
                    || monster->type == MONS_ZOMBIE_LARGE
                    || monster->type == MONS_SIMULACRUM_SMALL
                    || monster->type == MONS_SIMULACRUM_LARGE
                    || monster->type == MONS_SKELETON_SMALL
                    || monster->type == MONS_SKELETON_LARGE
                    || monster->type == MONS_SPECTRAL_THING)
                {
                    // see above for comments {dlb}
                    if (mons_itemuse(monster->type) > 0)
                    {
                        grd[monster->x + mmov_x][monster->y + mmov_y]
                                                        = DNGN_OPEN_DOOR;
                        return;
                    }
                }
                else
                {
                    grd[monster->x + mmov_x][monster->y + mmov_y]
                                                        = DNGN_OPEN_DOOR;
                    return;
                }
            }

            if (which_first)
            {
                if (good_move[mmov_x + 1][1])
                    mmov_y = 0;
                else if (good_move[1][mmov_y + 1])
                    mmov_x = 0;
            }
            else
            {
                if (good_move[1][mmov_y + 1])
                    mmov_x = 0;
                else if (good_move[mmov_x + 1][1])
                    mmov_y = 0;
            }

            if (mmov_x != 0 && mmov_y != 0)
            {
                mmov_x = random2(3) - 1;
                mmov_y = random2(3) - 1;
            }
        }
    }
    else /* end of: if (mmov_x !=0 etc */
    {
        if (mmov_x != 0 ^ mmov_y != 0)  /* ^ = XOR, || = OR(inc) */
        {
            if (mmov_x != 0 && !(good_move[mmov_x + 1][1]))
            {
                if (coinflip())
                {
                    if (good_move[1][0])
                    {
                        mmov_y = -1;
                        mmov_x = 0;
                    }
                    else
                    {
                        if (good_move[1][2])
                        {
                            mmov_y = 1;
                            mmov_x = 0;
                        }
                    }
                }
                else
                {
                    if (good_move[1][2])
                    {
                        mmov_y = 1;
                        mmov_x = 0;
                    }
                    else
                    {
                        if (good_move[1][0])
                        {
                            mmov_y = -1;
                            mmov_x = 0;
                        }
                    }
                }
            }
            else if (mmov_y != 0 && !(good_move[1][mmov_y + 1]))
            {
                if (coinflip())
                {
                    if (good_move[0][1])
                    {
                        mmov_x = -1;
                        mmov_y = 0;
                    }
                    else if (good_move[2][1])
                    {
                        mmov_x = 1;
                        mmov_y = 0;
                    }
                }
                else
                {
                    if (good_move[2][1])
                    {
                        mmov_x = 1;
                        mmov_y = 0;
                    }
                    else if (good_move[0][1])
                    {
                        mmov_x = -1;
                        mmov_y = 0;
                    }
                }
            }

            if (mmov_x != 0 && mmov_y != 0)
            {
                if (good_move[mmov_x + 1][mmov_y + 1])
                {
                    mmov_y = random2(3) - 1;
                    mmov_x = random2(3) - 1;
                }
            }
        }                   // end of if (mmov_x != 0 ^ etc for the 2nd time
    }                       // end of else above

    /* maybe: (makes worms stupid, though) */
    if (monster->type == MONS_BORING_BEETLE && mmov_x != 0 && mmov_y != 0)
    {
        if (coinflip())
            mmov_x = 0;
        else
            mmov_y = 0;

        if (grd[monster->x + mmov_x][monster->y + mmov_y] == DNGN_STONE_WALL
            && monster->x + mmov_x > 6 && monster->x + mmov_x < (GXM - 7)
            && monster->y + mmov_y > 6 && monster->y + mmov_y < (GYM - 7))
        {
            grd[monster->x + mmov_x][monster->y + mmov_y] = DNGN_FLOOR;

            if (!silenced(you.x_pos, you.y_pos))
                mpr("You hear a grinding noise.");
        }
    }

    unsigned char grik = grd[monster->x + mmov_x][monster->y + mmov_y];

    mgrd[monster->x][monster->y] = NON_MONSTER;

    if (grik >= okmove
        && mgrd[monster->x + mmov_x][monster->y + mmov_y] == NON_MONSTER)
    {
        if (monster->x + mmov_x == you.x_pos
            && monster->y + mmov_y == you.y_pos)
        {
            monster_attack(monster_index(monster));
            mmov_x = 0;
            mmov_y = 0;
        }

        vacated_x = monster->x;
        vacated_y = monster->y;

        if (monster_habitat(monster->type) == DNGN_LAVA && grik != DNGN_LAVA)
        {
            mmov_x = 0;
            mmov_y = 0;
        }

        if (monster_habitat(monster->type) == DNGN_DEEP_WATER
            && grik != DNGN_DEEP_WATER
            && grik != DNGN_SHALLOW_WATER && grik != DNGN_BLUE_FOUNTAIN)
        {
            mmov_x = 0;
            mmov_y = 0;
        }

        if (monster->type == MONS_WANDERING_MUSHROOM
            && see_grid(monster->x + mmov_x, monster->y + mmov_y))
        {
            mmov_x = 0;
            mmov_y = 0;
        }

        if (monster->type == MONS_EFREET
            || monster->type == MONS_FIRE_ELEMENTAL)
        {
            place_cloud(CLOUD_FIRE_MON, monster->x, monster->y, 2 + random2(4));
        }

        /* this appears to be the real one, ie where the movement occurs: */
        monster->x += mmov_x;
        monster->y += mmov_y;
    }

    /* need to put in something so that monster picks up multiple
       items (eg ammunition) identical to those it's carrying. */
    mgrd[monster->x][monster->y] = monster_index(monster);

    // monsters stepping on traps:
    if (grd[monster->x][monster->y] >= DNGN_TRAP_MECHANICAL
        && grd[monster->x][monster->y] <= DNGN_UNDISCOVERED_TRAP
        && (mmov_x != 0 || mmov_y != 0))
    {
        mons_trap(monster);
    }
}                               // end monster_move()


static bool plant_spit(struct monsters *monster, struct bolt &pbolt)
{
    bool didSpit = false;

    char spit_string[100];

    // setup plant spit
    strcpy(pbolt.beam_name, "plant spit");
    pbolt.type = SYM_ZAP;
    pbolt.range = 9;
    pbolt.rangeMax = 9;
    pbolt.colour = YELLOW;
    pbolt.flavour = BEAM_ACID;
    pbolt.beam_source = monster_index(monster);
    pbolt.damage = 107;
    pbolt.hit = 20 + (3 * monster->hit_dice);
    pbolt.thrower = KILL_MON_MISSILE;

    // fire tracer
    fire_tracer(monster, pbolt);

    if (mons_should_fire(pbolt))
    {
        strcpy(spit_string, " spits");
        if (pbolt.target_x == you.x_pos && pbolt.target_y == you.y_pos)
            strcat(spit_string, " at you");

        strcat(spit_string, ".");
        simple_monster_message(monster, spit_string);

        beam(pbolt);
        didSpit = true;
    }

    return didSpit;
}                               // end plant_spit()

static void mons_in_cloud(struct monsters *monster)
{
    int wc = env.cgrid[monster->x][monster->y];
    int hurted = 0;
    struct bolt beam;

    if (mons_category(monster->type) == MC_MIMIC)
    {
        mimic_alert(monster);
        return;
    }

    switch (env.cloud_type[wc])
    {
    case CLOUD_DEBUGGING:
        cprintf("Fatal error: monster steps on nonexistant cloud!");
        exit(0);
        return;

    case CLOUD_FIRE:
    case CLOUD_FIRE_MON:
        if (monster->type == MONS_FIRE_VORTEX
            || monster->type == MONS_EFREET
            || monster->type == MONS_FIRE_ELEMENTAL)
        {
            return;
        }

        simple_monster_message(monster, " is engulfed in flame!");

        if (mons_res_fire(monster->type) > 0)
            return;

        hurted += ((random2avg(16, 3) + 6) * 10) / monster->speed;

        if (mons_res_fire(monster->type) < 0
            && !(monster->inv[MSLOT_ARMOUR] != NON_ITEM
                 && mitm.special[monster->inv[MSLOT_ARMOUR]] % 30
                                                 == SPARM_FIRE_RESISTANCE))
        {
            hurted += (random2(15) * 10) / monster->speed;
        }

        // remember that the above is in addition to the other you.damage.
        hurted -= random2(1 + monster->armor_class);

        if (monster->inv[MSLOT_ARMOUR] != NON_ITEM
            && mitm.special[monster->inv[MSLOT_ARMOUR]] % 30
                                                == SPARM_FIRE_RESISTANCE)
        {
            hurted /= 3;
        }
        break;                  // to damage routine at end {dlb}

    case CLOUD_STINK:
    case CLOUD_STINK_MON:
        simple_monster_message(monster, " is engulfed in noxious gasses!");

        if (mons_res_poison(monster->type) > 0)
            return;

        beam.colour = RED;

        if (1 + random2(27) >= monster->hit_dice)
            mons_ench_f2(monster, beam);

        hurted += (random2(3) * 10) / monster->speed;
        break;                  // to damage routine at end {dlb}

    case CLOUD_COLD:
    case CLOUD_COLD_MON:
        simple_monster_message(monster, " is engulfed in freezing vapours!");

        if (mons_res_cold(monster->type) > 0)
            return;

        hurted += ((6 + random2avg(16, 3)) * 10) / monster->speed;

        if (mons_res_cold(monster->type) < 0
            && !(monster->inv[MSLOT_ARMOUR] != NON_ITEM
                 && mitm.special[monster->inv[MSLOT_ARMOUR]] % 30
                                             == SPARM_COLD_RESISTANCE))
        {
            hurted += (random2(15) * 10) / monster->speed;
        }

        // remember that the above is in addition to the other damage.
        hurted -= random2(1 + monster->armor_class);

        if (monster->inv[MSLOT_ARMOUR] != NON_ITEM
            && mitm.special[monster->inv[MSLOT_ARMOUR]] % 30
                                            == SPARM_COLD_RESISTANCE)
        {
            hurted /= 3;
        }
        break;                  // to damage routine at end {dlb}

    // what of armour of poison resistance here? {dlb}
    case CLOUD_POISON:
    case CLOUD_POISON_MON:
        simple_monster_message(monster, " is engulfed in a cloud of poison!");

        if (mons_res_poison(monster->type) > 0)
            return;

        poison_monster(monster, (env.cloud_type[wc] == CLOUD_POISON));

        hurted += (random2(8) * 10) / monster->speed;

        if (mons_res_poison(monster->type) < 0)
            hurted += (random2(4) * 10) / monster->speed;
        break;                  // to damage routine at end {dlb}

    case CLOUD_STEAM:
    case CLOUD_STEAM_MON:
        // couldn't be bothered coding for armour of res fire

        // what of whether it is wearing steam dragon armour? {dlb}
        if (monster->type == MONS_STEAM_DRAGON)
            return;

        simple_monster_message(monster, " is engulfed in steam!");

        if (mons_res_fire(monster->type) > 0
            || (monster->inv[MSLOT_ARMOUR] != NON_ITEM
                && mitm.special[monster->inv[MSLOT_ARMOUR]] % 30
                                                == SPARM_FIRE_RESISTANCE))
        {
            return;
        }

        hurted += (random2(6) * 10) / monster->speed;

        if (mons_res_fire(monster->type) < 0
            && !(monster->inv[MSLOT_ARMOUR] != NON_ITEM
                 && mitm.special[monster->inv[MSLOT_ARMOUR]] % 30
                                             == SPARM_FIRE_RESISTANCE))
        {
            hurted += (random2(6) * 10) / monster->speed;
        }

        hurted -= random2(1 + monster->armor_class);
        break;                  // to damage routine at end {dlb}

    case CLOUD_MIASMA:
    case CLOUD_MIASMA_MON:
        simple_monster_message(monster, " is engulfed in a dark miasma!");

        if (mons_holiness(monster->type) != MH_NATURAL)
            return;

        poison_monster(monster, (env.cloud_type[wc] == CLOUD_MIASMA));

        if (monster->max_hit_points > 4 && coinflip())
            monster->max_hit_points--;

        beam.colour = BLACK;

        if (one_chance_in(3))
            mons_ench_f2(monster, beam);

        hurted += (10 * random2avg(12, 3)) / monster->speed;    // 3
        break;              // to damage routine at end {dlb}

    default:                // 'harmless' clouds -- colored smoke, etc {dlb}.
        return;
    }

    if (hurted < 0)
        hurted = 0;
    else if (hurted > 0)
    {
        hurt_monster(monster, hurted);

        if (monster->hit_points < 1)
        {
            switch (env.cloud_type[wc])
            {
            case CLOUD_FIRE_MON:
            case CLOUD_STINK_MON:
            case CLOUD_COLD_MON:
            case CLOUD_POISON_MON:
            case CLOUD_STEAM_MON:
            case CLOUD_MIASMA_MON:
                monster_die(monster, KILL_MISC, 0);
                break;
            default:
                monster_die(monster, KILL_YOU, 0);
            }

            switch (env.cloud_type[wc])
            {
            case CLOUD_FIRE:
            case CLOUD_FIRE_MON:
            case CLOUD_COLD:
            case CLOUD_COLD_MON:
            case CLOUD_STEAM:
            case CLOUD_STEAM_MON:
                monster->speed_increment = 1;
            }
        }
    }
}                               // end mons_in_cloud()

unsigned char monster_habitat(int which_class)
{
    switch (which_class)
    {
    case MONS_BIG_FISH:
    case MONS_GIANT_GOLDFISH:
    case MONS_ELECTRICAL_EEL:
    case MONS_JELLYFISH:
    case MONS_WATER_ELEMENTAL:
    case MONS_SWAMP_WORM:
        return DNGN_DEEP_WATER; // no shallow water (only) monsters? {dlb}
        // must remain DEEP_WATER for now, else breaks code {dlb}

    case MONS_LAVA_WORM:
    case MONS_LAVA_FISH:
    case MONS_LAVA_SNAKE:
// "another lava thing" has no stats! (GDL)
//    case MONS_ANOTHER_LAVA_THING:
        return DNGN_LAVA;

    default:
        return DNGN_FLOOR;      // closest match to terra firma {dlb}
    }
}                               // end monster_habitat()

bool monster_descriptor(int which_class, unsigned char which_descriptor)
{
    switch (which_descriptor)
    {
    case MDSC_LEAVES_HIDE:
        switch (which_class)
        {
        case MONS_DRAGON:
        case MONS_TROLL:
        case MONS_ICE_DRAGON:
        case MONS_STEAM_DRAGON:
        case MONS_MOTTLED_DRAGON:
        case MONS_STORM_DRAGON:
        case MONS_GOLDEN_DRAGON:
        case MONS_SWAMP_DRAGON:
            return true;
        default:
            return false;
        }
        break;

    case MDSC_REGENERATES:
        switch (which_class)
        {
        case MONS_CACODEMON:
        case MONS_DEEP_TROLL:
        case MONS_HELLWING:
        case MONS_IMP:
        case MONS_IRON_TROLL:
        case MONS_LEMURE:
        case MONS_ROCK_TROLL:
        case MONS_SLIME_CREATURE:
        case MONS_SNORG:
        case MONS_TROLL:
            return true;
        default:
            return false;
        }
        break;

    case MDSC_NOMSG_WOUNDS:
        switch (which_class)
        {
        case MONS_RAKSHASA:
        case MONS_RAKSHASA_FAKE:
        case MONS_SKELETON_LARGE:
        case MONS_SKELETON_SMALL:
        case MONS_ZOMBIE_LARGE:
        case MONS_ZOMBIE_SMALL:
        case MONS_SIMULACRUM_SMALL:
        case MONS_SIMULACRUM_LARGE:
            return true;
        default:
            return false;
        }
        break;

    default:
        return false;
    }
}                               // end monster_descriptor()

bool message_current_target(void)
{
    if (!(you.prev_targ == MHITNOT || you.prev_targ == MHITYOU))
    {
        struct monsters *montarget = &menv[you.prev_targ];

        if (mons_near(montarget)
            && (montarget->enchantment[2] != ENCH_INVIS
                || player_see_invis()))
        {
            strcpy(info, "You are currently targetting ");
            strcat(info, ptr_monam(montarget, 1));
            strcat(info, " (p to target).");
            mpr(info);

            return true;        // early return {dlb}
        }
        else
            mpr("You have no current target.");
    }

    return false;
}                               // end message_current_target()

// aaah, the simple joys of pointer arithmetic! {dlb}:
int monster_index(struct monsters *monster)
{
    return (monster - menv.buffer());
}                               // end monster_index()

bool hurt_monster(struct monsters * victim, int damage_dealt)
{
    bool just_a_scratch = true;

    if (damage_dealt > 0)
    {
        just_a_scratch = false;
        victim->hit_points -= damage_dealt;
    }

    return (!just_a_scratch);
}                               // end hurt_monster()

bool heal_monster(struct monsters * patient, int health_boost,
                  bool permit_growth)
{
    if (health_boost < 1)
        return false;
    else if (!permit_growth && patient->hit_points == patient->max_hit_points)
        return false;
    else
    {
        if ((patient->hit_points += health_boost) > patient->max_hit_points)
        {
            if (permit_growth)
                patient->max_hit_points++;

            patient->hit_points = patient->max_hit_points;
        }
    }

    return true;
}                               // end heal_monster()

static int map_wand_to_mspell(int wand_type)
{
    int mzap = 0;

    switch (wand_type)
    {
        case WAND_FLAME:
            mzap = MS_FLAME;
            break;
        case WAND_FROST:
            mzap = MS_FROST;
            break;
        case WAND_SLOWING:
            mzap = MS_SLOW;
            break;
        case WAND_HASTING:
            mzap = MS_HASTE;
            break;
        case WAND_MAGIC_DARTS:
            mzap = MS_MMISSILE;
            break;
        case WAND_HEALING:
            mzap = MS_HEAL;
            break;
        case WAND_PARALYSIS:
            mzap = MS_PARALYSIS;
            break;
        case WAND_FIRE:
            mzap = MS_FIRE_BOLT;
            break;
        case WAND_COLD:
            mzap = MS_COLD_BOLT;
            break;
        case WAND_CONFUSION:
            mzap = MS_CONFUSE;
            break;
        case WAND_INVISIBILITY:
            mzap = MS_INVIS;
            break;
        case WAND_TELEPORTATION:
            mzap = MS_TELEPORT_OTHER;
            break;
        case WAND_LIGHTNING:
            mzap = MS_LIGHTNING_BOLT;
            break;
        case WAND_DRAINING:
            mzap = MS_NEGATIVE_BOLT;
            break;
        default:
            mzap = 0;
            break;
    }

    return mzap;
}

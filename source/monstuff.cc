/*
 *  File:       monstuff.cc
 *  Summary:    Misc monster related functions.
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 *      <8>      7 Aug  2001   MV      Inteligent monsters now pick up gold
 *      <7>     26 Mar  2001   GDL     Fixed monster reaching
 *      <6>     13 Mar  2001   GDL     Rewrite of monster AI
 *      <5>     31 July 2000   GDL     More Manticore fixes.
 *      <4>     29 July 2000   JDJ     Fixed a bunch of places in handle_pickup where MSLOT_WEAPON
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
#include "cloud.h"
#include "debug.h"
#include "fight.h"
#include "itemname.h"
#include "items.h"
#include "misc.h"
#include "monplace.h"
#include "monspeak.h"
#include "mon-util.h"
#include "mstuff2.h"
#include "player.h"
#include "randart.h"
#include "spells2.h"
#include "spells4.h"
#include "stuff.h"
#include "view.h"

static bool handle_special_ability(struct monsters *monster, bolt & beem);
static bool handle_pickup(struct monsters *monster);
static void handle_behaviour(struct monsters *monster);
static void mons_in_cloud(struct monsters *monster);
static void monster_move(struct monsters *monster);
static bool plant_spit(struct monsters *monster, struct bolt &pbolt);
static int map_wand_to_mspell(int wand_type);

char mmov_x, mmov_y;

static int compass_x[8] = { -1, 0, 1, 1, 1, 0, -1, -1 };
static int compass_y[8] = { -1, -1, -1, 0, 1, 1, 1, 0 };

#define FAR_AWAY    1000000         // used in monster_move()

bool curse_an_item( char which, char power )
{
    /* use which later, if I want to curse weapon/gloves whatever
       which, for now: 0 = non-mummy, 1 = mummy (potions as well)
       don't change mitm.special of !odecay */

    int count = 0;
    int item  = ENDOFPACK;

    for (int i = 0; i < ENDOFPACK; i++)
    {
        if (!is_valid_item( you.inv[i] ))
            continue;

        if (you.inv[i].base_type == OBJ_WEAPONS
            || you.inv[i].base_type == OBJ_ARMOUR
            || you.inv[i].base_type == OBJ_JEWELLERY
            || you.inv[i].base_type == OBJ_POTIONS)
        {
            if (item_cursed( you.inv[i] ))
                continue;

            if (you.inv[i].base_type == OBJ_POTIONS
                && (which != 1 || you.inv[i].sub_type == POT_DECAY))
            {
                continue;
            }

            // item is valid for cursing, so we'll give it a chance
            count++;
            if (one_chance_in( count ))
                item = i;
        }
    }

    // any item to curse?
    if (item == ENDOFPACK)
        return (false);

    // curse item:

    /* problem: changes large piles of potions */
    /* don't change you.inv_special (just for fun) */
    if (you.inv[item].base_type == OBJ_POTIONS)
        you.inv[item].sub_type = POT_DECAY;
    else
        do_curse_item( you.inv[item] );

    return (true);
}

void monster_blink(struct monsters *monster)
{
    int nx, ny;

    if (!random_near_space(monster->x, monster->y, nx, ny,
        false, false))
        return;

    mgrd[monster->x][monster->y] = NON_MONSTER;

    monster->x = nx;
    monster->y = ny;

    mgrd[nx][ny] = monster_index(monster);
}                               // end monster_blink()

// allow_adjacent:  allow target to be adjacent to origin
// restrict_LOS:    restict target to be within PLAYER line of sight
bool random_near_space(int ox, int oy, int &tx, int &ty, bool allow_adjacent,
    bool restrict_LOS)
{
    int tries = 0;

    do
    {
        tx = ox - 6 + random2(14);
        ty = oy - 6 + random2(14);

        // origin is not 'near'
        if (tx == ox && ty == oy)
            continue;

        tries++;

        if (tries > 149)
            break;
    }
    while ((!see_grid(tx, ty) && restrict_LOS)
           || grd[tx][ty] < DNGN_SHALLOW_WATER
           || mgrd[tx][ty] != NON_MONSTER
           || (!allow_adjacent && distance(ox, oy, tx, ty) <= 2));

    return (tries < 150);
}                               // end random_near_space()

static bool habitat_okay( struct monsters *monster, int targ )
{
    bool ret = false;
    int habitat = monster_habitat( monster->type );

    if (mons_flies( monster ))
    {
        // flying monsters don't care
        ret = true;
    }
    else if (monster->type == MONS_WATER_ELEMENTAL && targ >= DNGN_DEEP_WATER)
    {
        // water elementals can crawl out over the land
        ret = true;
    }
    else if (habitat == DNGN_FLOOR
            && (targ >= DNGN_FLOOR || targ == DNGN_SHALLOW_WATER))
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
    bool swap;

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
    int  dam_level;

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
        return (true);

    if (wound_class >= MONS_CLAY_GOLEM && wound_class <= MONS_TOENAIL_GOLEM)
        return (true);

    if (wound_class == MONS_FUNGUS || wound_class == MONS_ABOMINATION_LARGE
        || wound_class == MONS_PLANT || wound_class == MONS_FIRE_VORTEX
        || wound_class == MONS_ABOMINATION_SMALL
        || wound_class == MONS_DANCING_WEAPON
        || wound_class == MONS_SPATIAL_VORTEX)
    {
        return (true);
    }

    return (false);
}                               // end wounded_damaged()

//---------------------------------------------------------------
//
// behaviour_event
//
// 1. Change any of: monster state, foe, and attitude
// 2. Call handle_behaviour to re-evaluate AI state and target x,y
//
//---------------------------------------------------------------
void behaviour_event(struct monsters *mon, int event, int param)
{
    bool isSmart = (mons_intel(mon->type) > I_ANIMAL);
    bool isFriendly = mons_friendly(mon);
    bool sourceFriendly = false;
    bool setTarget = false;
    bool breakCharm = false;

    if (param == MHITYOU)
        sourceFriendly = true;
    else if (param != MHITNOT)
        sourceFriendly = mons_friendly(&menv[param]);

    switch(event)
    {
        case ME_EVAL:
            break;
        case ME_DISTURB:
            // assumes disturbed by noise.. just wake up by
            // setting to wander.
            if (mon->behaviour == BEH_SLEEP)
                mon->behaviour = BEH_WANDER;
            break;
        case ME_WHACK:
        case ME_ANNOY:
            // will turn monster against <param1>,  unless they
            // are BOTH friendly and stupid.   Hitting someone
            // over the head, of course, always triggers this code.
            if (isFriendly != sourceFriendly || isSmart
                || event == ME_WHACK)
            {
                mon->foe = param;
                if (mon->behaviour != BEH_CORNERED)
                    mon->behaviour = BEH_SEEK;
                if (param == MHITYOU)
                {
                    mon->attitude = ATT_HOSTILE;
                    breakCharm = true;
                }
            }
            if (event == ME_WHACK)
            {
                // now set target x,y so that monster can whack
                // back (once) at an invisible foe
                setTarget = true;
            }
            break;
        case ME_ALERT:
            // will alert monster to <param1> and turn them
            // against them,  unless they have a current foe.
            // it won't turn friends hostile either.
            if (mon->behaviour != BEH_CORNERED)
                mon->behaviour = BEH_SEEK;
            if (mon->foe == MHITNOT)
                mon->foe = param;
            break;
        case ME_SCARE:
            mon->foe = param;
            mon->behaviour = BEH_FLEE;
            // assume monsters know where to run from, even
            // if player is invisible.
            setTarget = true;
            break;
        case ME_CORNERED:
            // just set behaviour.. foe doesn't change.
            if (mon->behaviour != BEH_CORNERED && !mons_has_ench(mon,ENCH_FEAR))
                simple_monster_message(mon, " turns to fight!");
            mon->behaviour = BEH_CORNERED;
            break;
        default:
            break;
    }

    if (setTarget)
    {
        if (param == MHITYOU)
        {
            mon->target_x = you.x_pos;
            mon->target_y = you.y_pos;
            mon->attitude = ATT_HOSTILE;
        }
        else if (param != MHITNOT)
        {
            mon->target_x = menv[param].x;
            mon->target_y = menv[param].y;
        }
    }

    // now, break charms if appropriate
    if (breakCharm)
    {
        mons_del_ench(mon, ENCH_CHARM);
    }

    // do any resultant foe or state changes
    handle_behaviour(mon);
}

//---------------------------------------------------------------
//
// handle_behaviour
//
// 1. Evalutates current AI state
// 2. Sets monster targetx,y based on current foe
//
//---------------------------------------------------------------
static void handle_behaviour(struct monsters *mon)
{
    bool changed = true;
    bool isFriendly = mons_friendly(mon);
    bool proxPlayer = mons_near(mon);
    bool proxFoe;
    bool isHurt = (mon->hit_points <= mon->max_hit_points / 4 - 1);
    bool isHealthy = (mon->hit_points > mon->max_hit_points / 2);
    bool isSmart = (mons_intel(mon->type) > I_ANIMAL);
    bool isScared = mons_has_ench(mon, ENCH_FEAR);

    // immobility logic stolen from later on in handle_monster().. argh!  --gdl
    bool isMobile = !(mon->type == MONS_OKLOB_PLANT
                        || mon->type == MONS_CURSE_SKULL
                        || (mon->type >= MONS_CURSE_TOE
                            && mon->type <= MONS_POTION_MIMIC));


    // check for confusion -- early out.
    if (mons_has_ench(mon, ENCH_CONFUSION))
    {
        mon->target_x = 10 + random2(GXM - 10);
        mon->target_y = 10 + random2(GYM - 10);
        return;
    }

    // validate current target exists
    if (mon->foe != MHITNOT && mon->foe != MHITYOU)
    {
        if (menv[mon->foe].type == -1)
            mon->foe = MHITNOT;
    }

    // change proxPlayer depending on invisibility and standing
    // in shallow water
    if (proxPlayer && you.invis)
    {
        if (!(mons_see_invis(mon)
            || (grd[you.x_pos][you.y_pos] == DNGN_SHALLOW_WATER
                && !you.levitation)))
        {
            proxPlayer = false;
        }

        // must be able to see each other
        if (!see_grid(mon->x, mon->y))
            proxPlayer = false;

        // now, the corollary to that is that sometimes, if a
        // player is right next to a monster, they will 'see'
        if (distance(you.x_pos, you.y_pos, mon->x, mon->y) < 2
            && one_chance_in(3))
        {
            proxPlayer = true;
        }
    }

    // set friendly target, if they don't already have one
    if (isFriendly && (mon->foe == MHITNOT || mon->foe == MHITYOU))
    {
        if (you.pet_target != MHITNOT)
            mon->foe = you.pet_target;
    }

    // monsters do not attack themselves {dlb}
    if (mon->foe == monster_index(mon))
    {
        mon->foe = MHITNOT;
    }

    // friendly monsters do not attack other friendly monsters
    if (!(mon->foe == MHITNOT || mon->foe == MHITYOU))
    {
        if (isFriendly && mons_friendly(&menv[mon->foe]))
            mon->foe = MHITNOT;
    }

    // unfriendly monsters fighting other monsters will usually
    // target the player,  if they're healthy
    if (!isFriendly && mon->foe != MHITYOU && mon->foe != MHITNOT
        && proxPlayer && !one_chance_in(3) && isHealthy)
    {
        mon->foe = MHITYOU;
    }

    // validate target again
    if (mon->foe != MHITNOT && mon->foe != MHITYOU)
    {
        if (menv[mon->foe].type == -1)
            mon->foe = MHITNOT;
    }

    while(changed)
    {
        int foe_x = you.x_pos;
        int foe_y = you.y_pos;

        // evaluate these each time; they may change
        if (mon->foe == MHITNOT)
            proxFoe = false;
        else
        {
            if (mon->foe == MHITYOU)
            {
                foe_x = you.x_pos;
                foe_y = you.y_pos;
                proxFoe = proxPlayer;   // take invis into account
            }
            else
            {
                proxFoe = mons_near(mon, mon->foe);

                if (!mons_monster_visible( mon, &menv[mon->foe] ))
                    proxFoe = false;

                // XXX monsters will rely on player LOS -- GDL
                if (!see_grid(menv[mon->foe].x, menv[mon->foe].y))
                    proxFoe = false;

                foe_x = menv[mon->foe].x;
                foe_y = menv[mon->foe].y;
            }
        }

        // track changes to state; attitude never changes here.
        unsigned int new_beh = mon->behaviour;
        unsigned int new_foe = mon->foe;

        // take care of monster state changes
        switch(mon->behaviour)
        {
            case BEH_SLEEP:
                // default sleep state
                mon->target_x = mon->x;
                mon->target_y = mon->y;
                new_foe = MHITNOT;

                break;
            case BEH_SEEK:
                // no foe? then wander.
                if (mon->foe == MHITNOT)
                {
                    new_beh = BEH_WANDER;
                    break;
                }

                // foe gone out of LOS?
                if (!proxFoe)
                {
                    if (isFriendly)
                    {
                        new_foe = MHITYOU;
                        mon->target_x = foe_x;
                        mon->target_y = foe_y;
                        break;
                    }
                    if (mon->foe_memory > 0 && mon->foe != MHITNOT)
                    {
                        // if we've arrived at our target x,y
                        // do a stealth check.  If the foe
                        // fails,  monster will then start
                        // tracking foe's CURRENT position,
                        // but only for a few moves (smell and
                        // intuition only go so far)

                        if (mon->x == mon->target_x &&
                            mon->y == mon->target_y)
                        {
                            if (mon->foe == MHITYOU)
                            {
                                if (check_awaken(monster_index(mon)))
                                {
                                    mon->target_x = you.x_pos;
                                    mon->target_y = you.y_pos;
                                }
                                else
                                    mon->foe_memory = 1;
                            }
                            else
                            {
                                if (coinflip())     // XXX: cheesy!
                                {
                                    mon->target_x = menv[mon->foe].x;
                                    mon->target_y = menv[mon->foe].y;
                                }
                                else
                                    mon->foe_memory = 1;
                            }
                        }

                        // either keep chasing, or start
                        // wandering.
                        if (mon->foe_memory < 2)
                        {
                            mon->foe_memory = 0;
                            new_beh = BEH_WANDER;
                        }
                        break;
                    }

                    // hack: smarter monsters will
                    // tend to persue the player longer.
                    int memory;
                    switch(mons_intel(monster_index(mon)))
                    {
                        case I_HIGH:
                            memory = 100+random2(200);
                            break;
                        case I_NORMAL:
                            memory = 20+random2(20);
                            break;
                        case I_ANIMAL:
                            memory = 6+random2(10);
                            break;
                        case I_INSECT:
                            memory = 4+random2(5);
                            break;
                        default:
                            memory = 3;
                            break;
                    }
                    mon->foe_memory = memory;
                    break;      // from case
                }

                // monster can see foe: continue 'tracking'
                // by updating target x,y
                if (mon->foe == MHITYOU)
                {
                    // sometimes,  your friends will wander a bit.
                    if (isFriendly && one_chance_in(8))
                    {
                        mon->target_x = 10 + random2(GXM - 10);
                        mon->target_y = 10 + random2(GYM - 10);

                    }
                    else
                    {
                        mon->target_x = you.x_pos;
                        mon->target_y = you.y_pos;
                    }
                }
                else
                {
                    mon->target_x = menv[mon->foe].x;
                    mon->target_y = menv[mon->foe].y;
                }

                if (isHurt && isSmart && isMobile)
                    new_beh = BEH_FLEE;

                break;
            case BEH_WANDER:
                // is our foe in LOS?
                if (proxFoe)
                {
                    new_beh = BEH_SEEK;
                    break;
                }

                // default wander behaviour
                mon->target_x = 10 + random2(GXM - 10);
                mon->target_y = 10 + random2(GYM - 10);

                // during their wanderings,  monsters will
                // eventually relax their guard (stupid
                // ones will do so faster,  smart monsters
                // have longer memories
                if (!proxFoe && mon->foe != MHITNOT)
                {
                    if (one_chance_in(isSmart?60:20))
                        new_foe = MHITNOT;
                }
                break;

            case BEH_FLEE:
                // check for healed
                if (isHealthy && !isScared)
                    new_beh = BEH_SEEK;
                // smart monsters flee until they can
                // flee no more...  possible to get a
                // 'CORNERED' event,  at which point
                // we can jump back to WANDER if the foe
                // isn't present.

                if (proxFoe)
                {
                    // try to flee _from_ the correct position
                    mon->target_x = foe_x;
                    mon->target_y = foe_y;
                }

                break;
            case BEH_CORNERED:
                // foe gone out of LOS?
                if (!proxFoe)
                {
                    if (isFriendly || proxPlayer)
                        new_foe = MHITYOU;
                    else
                        new_beh = BEH_WANDER;
                }

                break;
            default:
                return;     // uh oh
        }

        changed = (new_beh != mon->behaviour || new_foe != mon->foe);
        mon->behaviour = new_beh;

        if (mon->foe != new_foe)
            mon->foe_memory = 0;

        mon->foe = new_foe;
    }
}                               // end handle_behaviour()

// note that this function *completely* blocks messaging for monsters
// distant or invisible to the player ... look elsewhere for a function
// permitting output of "It" messages for the invisible {dlb}
// INtentionally avoids info and str_pass now. -- bwr
bool simple_monster_message(struct monsters *monster, const char *event,
                            int channel, int param)
{
    char buff[200];

    if (mons_near( monster )
        && (channel == MSGCH_MONSTER_SPELL || player_monster_visible(monster)))
    {
        snprintf( buff, sizeof(buff), "%s%s",
                  ptr_monam(monster, DESC_CAP_THE), event );

        mpr( buff, channel, param );
        return (true);
    }

    return (false);
}                               // end simple_monster_message()

static bool handle_enchantment(struct monsters *monster)
{
    const int habitat = monster_habitat( monster->type );
    bool died = false;
    int grid;

    for (int p = 0; p < NUM_MON_ENCHANTS && !died; p++)
    {
        switch (monster->enchantment[p])
        {
        case ENCH_SLOW:
            if (random2(250) <= monster->hit_dice + 10)
                mons_del_ench(monster, ENCH_SLOW);
            break;

        case ENCH_HASTE:
            if (one_chance_in(40))
                mons_del_ench(monster, ENCH_HASTE);
            break;

        case ENCH_FEAR:
            if (random2(150) <= monster->hit_dice + 5)
                mons_del_ench(monster, ENCH_FEAR);
            break;

        case ENCH_CONFUSION:
            if (random2(120) < monster->hit_dice + 5)
            {
                // don't delete perma-confusion
                if (!mons_flag(monster->type, M_CONFUSED))
                    mons_del_ench(monster, ENCH_CONFUSION);
            }
            break;

        case ENCH_INVIS:
            if (one_chance_in(40))
            {
                // don't delete perma-invis
                if (!mons_flag( monster->type, M_INVIS ))
                    mons_del_ench(monster, ENCH_INVIS);
            }
            break;

        case ENCH_SUBMERGED:
            grid = grd[monster->x][monster->y];

            if (monster->type == MONS_AIR_ELEMENTAL)
            {
                heal_monster( monster, 1, one_chance_in(5) );

                if (one_chance_in(5))
                    mons_del_ench( monster, ENCH_SUBMERGED );
            }
            else if ((habitat != DNGN_FLOOR && habitat != grid)
                    || one_chance_in(40))
            {
                mons_del_ench( monster, ENCH_SUBMERGED );
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
                int poisonval = monster->enchantment[p] - ENCH_POISON_I;
                if (poisonval < 0 || poisonval > 3)
                    poisonval = monster->enchantment[p] - ENCH_YOUR_POISON_I;

                if (coinflip())
                    hurt_monster(monster, 1 + random2(poisonval + 3));

                // don't believe this is possible. -- bwr
                if (mons_res_poison(monster) < 0)
                {
                    hurt_monster(monster, 1 + random2(poisonval + 1)
                                            + random2(poisonval + 1));
                }

                if (monster->hit_points < 1)
                {
                    monster_die(monster,
                                ((monster->enchantment[p] < ENCH_POISON_I)
                                            ? KILL_YOU : KILL_MISC), 0);
                    died = true;
                }

                // chance to get over poison
                if (one_chance_in(8))
                {
                    if (monster->enchantment[p] == ENCH_POISON_I
                        || monster->enchantment[p] == ENCH_YOUR_POISON_I)
                    {
                        mons_del_ench(monster, ENCH_POISON_I);
                        mons_del_ench(monster, ENCH_YOUR_POISON_I);
                    }
                    else
                        monster->enchantment[p]--;
                }
            }
            break;

        case ENCH_YOUR_ROT_I:
            if (one_chance_in(4))
                mons_del_ench(monster, ENCH_YOUR_ROT_I);
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
                mons_del_ench(monster, ENCH_BACKLIGHT_I);
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

            if (mons_res_fire(monster) < 0)
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
                if (monster->enchantment[p] == ENCH_STICKY_FLAME_I
                    || monster->enchantment[p] == ENCH_YOUR_STICKY_FLAME_I)
                {
                    mons_del_ench(monster, ENCH_STICKY_FLAME_I);
                    mons_del_ench(monster, ENCH_YOUR_STICKY_FLAME_I);
                }
                else
                    monster->enchantment[p] --;
            }
            break;

        case ENCH_SHORT_LIVED:
            // this should only be used for ball lightning
            if (one_chance_in(10))
               monster->hit_points = -1;
            break;

        // 19 is taken by summoning:
        // If these are changed, must also change abjuration
        case ENCH_ABJ_I:
        case ENCH_ABJ_II:
        case ENCH_ABJ_III:
        case ENCH_ABJ_IV:
            if (one_chance_in(10))
                monster->enchantment[p]--;

            if (monster->enchantment[p] < ENCH_ABJ_I)
            {
                monster_die(monster, KILL_RESET, 0);
                died = true;
            }
            break;

        case ENCH_ABJ_V:
            if (one_chance_in(25))
                monster->enchantment[p] = ENCH_ABJ_IV;
            break;

        case ENCH_ABJ_VI:
            if (one_chance_in(50))
                monster->enchantment[p] = ENCH_ABJ_V;
            break;

        case ENCH_CHARM:
            if (random2(500) <= monster->hit_dice + 10)
                mons_del_ench(monster, ENCH_CHARM);
            break;

        case ENCH_GLOWING_SHAPESHIFTER:     // this ench never runs out
            if (monster->type == MONS_GLOWING_SHAPESHIFTER || one_chance_in(4))
                monster_polymorph(monster, RANDOM_MONSTER, 0);
            break;

        case ENCH_SHAPESHIFTER:     // this ench never runs out
            if (monster->type == MONS_SHAPESHIFTER || one_chance_in(15))
                monster_polymorph(monster, RANDOM_MONSTER, 0);
            break;

        case ENCH_TP_I:
            mons_del_ench(monster, ENCH_TP_I);
            monster_teleport(monster, true);
            break;

        case ENCH_TP_II:
        case ENCH_TP_III:
        case ENCH_TP_IV:
            monster->enchantment[p]--;
            break;

        case ENCH_SLEEP_WARY:
            if (one_chance_in(20))
                mons_del_ench(monster, ENCH_SLEEP_WARY);
            break;
        }
    }

    return died;
}                               // end handle_enchantment()

//---------------------------------------------------------------
//
// handle_movement
//
// Move the monster closer to its target square.
//
//---------------------------------------------------------------
static void handle_movement(struct monsters *monster)
{
    // some calculations
    int dx = monster->target_x - monster->x;
    int dy = monster->target_y - monster->y;

    // move the monster:
    mmov_x = (dx>0) ? 1 : ((dx<0) ? -1 : 0);
    mmov_y = (dy>0) ? 1 : ((dy<0) ? -1 : 0);

    if (monster->behaviour == BEH_FLEE)
    {
        mmov_x *= -1;
        mmov_y *= -1;
    }

    // bounds check: don't let fleeing monsters try to run
    // off the map
    if (monster->target_x + mmov_x < 0 || monster->target_x + mmov_x >= GXM)
        mmov_x = 0;

    if (monster->target_y + mmov_y < 0 || monster->target_y + mmov_y >= GYM)
        mmov_y = 0;

    // now quit if we're can't move
    if (mmov_x == 0 && mmov_y == 0)
        return;

    // reproduced here is some semi-legacy code that makes monsters
    // move somewhat randomly along oblique paths.  It is an exceedingly
    // good idea,  given crawl's unique line of sight properties.
    if (abs(dx) > abs(dy))
    {
        // sometimes we'll just move parallel the x axis
        if (coinflip())
            mmov_y = 0;
    }

    if (abs(dy) > abs(dx))
    {
        // sometimes we'll just move parallel the y axis
        if (coinflip())
            mmov_x = 0;
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

    if (mons_near(monster) && monster->behaviour != BEH_SLEEP)
    {
        if (mons_flag(monster->type, M_SPEAKS) && one_chance_in(21)
            && monster->behaviour != BEH_WANDER)
            mons_speaks(monster);

        switch (monster->type)
        {
        case MONS_SPATIAL_VORTEX:
        case MONS_KILLER_KLOWN:
            // I think it is used for colouring {dlb}
            monster->number = random_colour();
            break;

        case MONS_GIANT_EYEBALL:
            if (coinflip() && !mons_friendly(monster)
                && monster->behaviour != BEH_WANDER)
            {
                simple_monster_message(monster, " stares at you.");

                if (you.paralysis < 10)
                    you.paralysis += 2 + random2(3);
            }
            break;

        case MONS_EYE_OF_DRAINING:
            if (coinflip() && !mons_friendly(monster)
                && monster->behaviour != BEH_WANDER)
            {
                simple_monster_message(monster, " stares at you.");

                dec_mp(2 + random2avg(13, 3));

                heal_monster(monster, 5, true); // heh heh {dlb}
            }
            break;

        case MONS_LAVA_WORM:
        case MONS_LAVA_FISH:
        case MONS_LAVA_SNAKE:
        case MONS_SALAMANDER:
        case MONS_BIG_FISH:
        case MONS_GIANT_GOLDFISH:
        case MONS_ELECTRICAL_EEL:
        case MONS_JELLYFISH:
        case MONS_WATER_ELEMENTAL:
        case MONS_SWAMP_WORM:
            if (mons_has_ench( monster, ENCH_SUBMERGED))
            {
                if (grd[monster->x][monster->y] == DNGN_SHALLOW_WATER
                    || grd[monster->x][monster->y] == DNGN_BLUE_FOUNTAIN)
                {
                    mons_del_ench( monster, ENCH_SUBMERGED );
                }
                else if ((monster->x != you.x_pos || monster->y != you.y_pos)
                         && one_chance_in(10))
                {
                    mons_del_ench( monster, ENCH_SUBMERGED );
                }
            }
            else if ((grd[monster->x][monster->y] != DNGN_SHALLOW_WATER
                         && grd[monster->x][monster->y] != DNGN_BLUE_FOUNTAIN)
                     && (one_chance_in(5)))
            {
                    mons_add_ench( monster, ENCH_SUBMERGED );
            }
            break;

        case MONS_AIR_ELEMENTAL:
            if (one_chance_in(5))
                mons_add_ench( monster, ENCH_SUBMERGED );
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
    case MONS_BALL_LIGHTNING:
        if (monster->attitude == ATT_HOSTILE
            && distance( you.x_pos, you.y_pos, monster->x, monster->y ) <= 5)
        {
            monster->hit_points = -1;
            used = true;
            break;
        }

        for (int i = 0; i < MAX_MONSTERS; i++)
        {
            struct monsters *targ = &menv[i];

            if (targ->type == -1 || targ->type == NON_MONSTER)
                continue;

            if (distance( monster->x, monster->y, targ->x, targ->y ) >= 5)
                continue;

            if (monster->attitude == targ->attitude)
                continue;

            // faking LOS by checking the neighbouring square
            int dx = targ->x - monster->x;
            if (dx)
                dx /= dx;

            int dy = targ->y - monster->y;
            if (dy)
                dy /= dy;

            const int tx = monster->x + dx;
            const int ty = monster->y + dy;

            if (tx < 0 || tx > GXM || ty < 0 || ty > GYM)
                continue;

            if (grd[tx][ty] > DNGN_LAST_SOLID_TILE)
            {
                monster->hit_points = -1;
                used = true;
                break;
            }
        }
        break;

    case MONS_LAVA_SNAKE:
        if (mons_has_ench(monster, ENCH_CONFUSION))
            break;

        if (you.invis && !mons_see_invis(monster))
            break;

        if (mons_has_ench( monster, ENCH_SUBMERGED ) || coinflip())
        {
            break;
        }

        // setup tracer
        strcpy(beem.beam_name, "glob of lava");
        beem.range = 4;
        beem.rangeMax = 13;
        beem.damage = dice_def( 3, 8 );
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
        if (mons_has_ench(monster, ENCH_CONFUSION))
            break;

        if (you.invis && !mons_see_invis(monster))
            break;

        if (mons_has_ench( monster, ENCH_SUBMERGED ))
            break;

        // setup tracer
        strcpy(beem.beam_name, "bolt of electricity");
        beem.damage = dice_def( 3, 5 );
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
        if (mons_has_ench(monster, ENCH_CONFUSION))
            break;

        if (you.invis && !mons_see_invis(monster))
            break;

        if (one_chance_in(3))
            used = plant_spit(monster, beem);

        break;

    case MONS_PIT_FIEND:
        if (one_chance_in(3))
            break;
        // deliberate fall through
    case MONS_FIEND:
        if (mons_has_ench(monster, ENCH_CONFUSION))
            break;

        // friendly fiends won't use torment,  preferring hellfire
        // (right now there is no way a monster can predict how
        // badly they'll damage the player with torment) -- GDL
        if (one_chance_in(4))
        {
            int spell_cast;

            switch (random2(4))
            {
            case 0:
                if (!mons_friendly(monster))
                {
                    spell_cast = MS_TORMENT;
                    mons_cast(monster, beem, spell_cast);
                    used = true;
                    break;
                }
                // deliberate fallthrough -- see above
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
                    simple_monster_message( monster, " makes a gesture!",
                                            MSGCH_MONSTER_SPELL );

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
        if (you.invis && !mons_see_invis(monster))
            break;

        if (mons_has_ench(monster, ENCH_CONFUSION))
            break;

        if (!mons_near(monster))
            break;

        // the fewer spikes the manticore has left,  the less
        // likely it will use them.
        if (random2(16) >= static_cast<int>(monster->number))
            break;

        // do the throwing right here,  since the beam is so
        // easy to set up and doesn't involve inventory.

        // set up the beam
        strcpy(beem.beam_name, "volley of spikes");
        beem.range = 9;
        beem.rangeMax = 9;
        beem.hit = 14;
        beem.damage = dice_def( 1, 10 );
        beem.beam_source = monster_index(monster);
        beem.type = SYM_MISSILE;
        beem.colour = LIGHTGREY;
        beem.flavour = BEAM_MISSILE;
        beem.thrower = KILL_MON;
        beem.isBeam = false;

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
        if (you.invis && !mons_see_invis(monster))
            break;

        if (mons_has_ench(monster, ENCH_CONFUSION))
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

    return (used);
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
    if (monster->behaviour == BEH_SLEEP)
        return (false);
    else if (monster->inv[MSLOT_POTION] == NON_ITEM)
        return (false);
    else if (!one_chance_in(3))
        return (false);
    else
    {
        bool imbibed = false;

        switch (mitm[monster->inv[MSLOT_POTION]].sub_type)
        {
        case POT_HEALING:
        case POT_HEAL_WOUNDS:
            if (monster->hit_points <= monster->max_hit_points / 2
                && mons_holiness(monster->type) != MH_UNDEAD)
            {
                simple_monster_message(monster, " drinks a potion.");

                if (heal_monster(monster, 5 + random2(7), false))
                    simple_monster_message(monster, " is healed!");

                if (mitm[monster->inv[MSLOT_POTION]].sub_type
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
            if (mitm[monster->inv[MSLOT_POTION]].sub_type == POT_INVISIBILITY)
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
            if (dec_mitm_item_quantity( monster->inv[MSLOT_POTION], 1 ))
                monster->inv[MSLOT_POTION] = NON_ITEM;
        }

        return (imbibed);
    }
}                               // end handle_potion()

static bool handle_reaching(struct monsters *monster)
{
    bool       ret = false;
    const int  wpn = monster->inv[MSLOT_WEAPON];

    if (mons_aligned(monster_index(monster), monster->foe))
        return (false);

    if (wpn != NON_ITEM && get_weapon_brand( mitm[wpn] ) == SPWPN_REACHING )
    {
        if (monster->foe == MHITYOU)
        {
            // this check isn't redundant -- player may be invisible.
            if (monster->target_x == you.x_pos && monster->target_y == you.y_pos)
            {
                int dx = abs(monster->x - you.x_pos);
                int dy = abs(monster->y - you.y_pos);

                if ((dx == 2 && dy <= 2) || (dy == 2 && dx <= 2))
                {
                    ret = true;
                    monster_attack(monster_index(monster));
                }
            }
        }
        else if (monster->foe != MHITNOT)
        {
            // same comments as to invisibility as above.
            if (monster->target_x == menv[monster->foe].x
                && monster->target_y == menv[monster->foe].y)
            {
                int dx = abs(monster->x - menv[monster->foe].x);
                int dy = abs(monster->y - menv[monster->foe].y);
                if ((dx == 2 && dy <= 2) || (dy == 2 && dx <= 2))
                {
                    ret = true;
                    monsters_fight(monster_index(monster),
                        monster->foe);
                }
            }
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
    if (mons_has_ench(monster, ENCH_CONFUSION) || monster->behaviour == BEH_SLEEP)
        return (false);
    else if (monster->inv[MSLOT_SCROLL] == NON_ITEM)
        return (false);
    else if (!one_chance_in(3))
        return (false);
    else
    {
        bool read = false;

        // notice how few cases are actually accounted for here {dlb}:
        switch (mitm[monster->inv[MSLOT_SCROLL]].sub_type)
        {
        case SCR_TELEPORTATION:
            if (!mons_has_ench(monster, ENCH_TP_I))
            {
                if (monster->behaviour == BEH_FLEE)
                {
                    simple_monster_message(monster, " reads a scroll.");
                    monster_teleport(monster, false);
                    read = true;
                }
            }
            break;

        case SCR_BLINKING:
            if (monster->behaviour == BEH_FLEE)
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
                create_monster( MONS_ABOMINATION_SMALL, ENCH_ABJ_II,
                                SAME_ATTITUDE(monster), monster->x, monster->y,
                                monster->foe, 250 );
                read = true;
            }
            break;
        }

        if (read)
        {
            if (dec_mitm_item_quantity( monster->inv[MSLOT_SCROLL], 1 ))
                monster->inv[MSLOT_SCROLL] = NON_ITEM;
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
    if (monster->behaviour == BEH_SLEEP)
        return (false);
    else if (!mons_near(monster))
        return (false);
    else if (monster->inv[MSLOT_WAND] == NON_ITEM
             || mitm[monster->inv[MSLOT_WAND]].plus <= 0)
    {
        return (false);
    }
    else if (coinflip())
        return (false);
    else
    {
        bool niceWand = false;
        bool zap;

        switch (mitm[monster->inv[MSLOT_WAND]].sub_type)
        {
            // these have been deemed "to tricky" at this time {dlb}:
        case WAND_POLYMORPH_OTHER:
        case WAND_ENSLAVEMENT:
        case WAND_DIGGING:
        case WAND_RANDOM_EFFECTS:
            return (false);

        // these are wands that monsters will aim at themselves {dlb}:
        case WAND_HASTING:
            if (!mons_has_ench(monster, ENCH_HASTE))
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
            if (!mons_has_ench( monster, ENCH_INVIS )
                && !mons_has_ench( monster, ENCH_SUBMERGED ))
            {
                beem.target_x = monster->x;
                beem.target_y = monster->y;
                niceWand = true;
            }
            break;

        case WAND_TELEPORTATION:
            if (monster->hit_points <= monster->max_hit_points / 2)
            {
                if (!mons_has_ench(monster, ENCH_TP_I))
                {
                    beem.target_x = monster->x;
                    beem.target_y = monster->y;

                    niceWand = true;
                }
            }
            break;
        }            // end "switch mitm[monster->inv[MSLOT_WAND]].sub_type"

        // map wand type to monster spell type
        int mzap = map_wand_to_mspell(mitm[monster->inv[MSLOT_WAND]].sub_type);
        if (mzap == 0)
            return (false);

        // set up the beam
        int power = 25;
        struct SBeam theBeam = mons_spells(mzap, power);

        strcpy(beem.beam_name, theBeam.name.c_str());
        beem.beam_source = monster_index(monster);
        beem.colour = theBeam.colour;
        beem.range = theBeam.range;
        beem.rangeMax = theBeam.rangeMax;
        beem.damage = theBeam.damage;
        beem.ench_power = theBeam.ench_power;
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
            mitm[monster->inv[MSLOT_WAND]].plus--;
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
    bool monsterNearby = mons_near(monster);
    bool finalAnswer = false;   // as in: "Is that your...?" {dlb}

    // yes, there is a logic to this ordering {dlb}:
    if (monster->behaviour == BEH_SLEEP
        || !mons_flag(monster->type, M_SPELLCASTER))
    {
        return (false);
    }

    if ((mons_flag(monster->type, M_ACTUAL_SPELLS)
            || mons_flag(monster->type, M_PRIEST))
        && (mons_has_ench(monster, ENCH_GLOWING_SHAPESHIFTER, ENCH_SHAPESHIFTER)))
    {
        return (false);           //jmf: shapeshiftes don't get spells, just
                                  //     physical powers.
    }
    else if (mons_has_ench(monster, ENCH_CONFUSION)
            && !mons_flag(monster->type, M_CONFUSED))
    {
        return (false);
    }
    else if (monster->type == MONS_PANDEMONIUM_DEMON && ghost.values[9] == 0)
    {
        return (false);
    }
    else if (random2(200) > monster->hit_dice + 50
            || (monster->type == MONS_BALL_LIGHTNING && coinflip()))
    {
        return (false);
    }
    else
    {
        int spell_cast = MS_NO_SPELL;
        int hspell_pass[6] = { MS_NO_SPELL, MS_NO_SPELL, MS_NO_SPELL,
                               MS_NO_SPELL, MS_NO_SPELL, MS_NO_SPELL };

        int msecc = ((monster->type == MONS_HELLION)           ? MST_BURNING_DEVIL :
                     (monster->type == MONS_PANDEMONIUM_DEMON) ? MST_GHOST
                                                            : monster->number);

        mons_spell_list( msecc, hspell_pass );

        // forces the casting of dig when player not visible - this is EVIL!
        if (!monsterNearby)
        {
            if (hspell_pass[4] == MS_DIG && monster->behaviour == BEH_SEEK)
            {
                spell_cast = MS_DIG;
                finalAnswer = true;
            }
            else if (hspell_pass[2] == MS_HEAL
                        && monster->hit_points < monster->max_hit_points)
            {
                // The player's out of sight!
                // Quick, let's take a turn to heal ourselves. -- bwr
                spell_cast = MS_HEAL;
                finalAnswer = true;
            }
            else if (monster->behaviour == BEH_FLEE)
            {
                // Since the player isn't around, we'll extend the monster's
                // normal fleeing choices to include the self-enchant slot.
                if (ms_useful_fleeing_out_of_sight(monster,hspell_pass[5]))
                {
                    spell_cast = hspell_pass[5];
                    finalAnswer = true;
                }
                else if (ms_useful_fleeing_out_of_sight(monster,hspell_pass[2]))
                {
                    spell_cast = hspell_pass[2];
                    finalAnswer = true;
                }
            }
            else if (monster->foe == MHITYOU)
            {
                return (false);
            }
        }

        // Promote the casting of useful spells for low-HP monsters.
        if (!finalAnswer
            && monster->hit_points < monster->max_hit_points / 6
            && one_chance_in(4))
        {
            // Note: There should always be at least some chance we don't
            // get here... even if the monster is on it's last HP.  That
            // way we don't have to worry about monsters infinitely casting
            // Healing on themselves (ie orc priests).
            if (monster->behaviour == BEH_FLEE
                && ms_low_hitpoint_cast( monster, hspell_pass[5] ))
            {
                spell_cast = hspell_pass[5];
                finalAnswer = true;
            }
            else if (ms_low_hitpoint_cast( monster, hspell_pass[2] ))
            {
                spell_cast = hspell_pass[2];
                finalAnswer = true;
            }
        }

        if (!finalAnswer)
        {
            // should monster not have selected dig by now, it never will:
            if (hspell_pass[4] == MS_DIG)
                hspell_pass[4] = MS_NO_SPELL;

            // remove healing/invis/haste if we don't need them
            int num_no_spell = 0;

            for (int i = 0; i < 6; i++)
            {
                if (hspell_pass[i] == MS_NO_SPELL)
                    num_no_spell++;
                else if (ms_waste_of_time( monster, hspell_pass[i] ))
                {
                    hspell_pass[i] = MS_NO_SPELL;
                    num_no_spell++;
                }
            }

            // If no useful spells... cast no spell.
            if (num_no_spell == 6)
                return (false);

            // up to four tries to pick a spell.
            for (int loopy = 0; loopy < 4; loopy ++)
            {
                bool spellOK = false;

                // setup spell - fleeing monsters will always try to
                // choose their emergency spell.
                if (monster->behaviour == BEH_FLEE)
                {
                    spell_cast = (one_chance_in(5) ? MS_NO_SPELL
                                                   : hspell_pass[5]);
                }
                else
                {
                    // Randomly picking one of the non-emergency spells:
                    spell_cast = hspell_pass[random2(5)];
                }

                if (spell_cast == MS_NO_SPELL)
                    continue;

                // setup the spell
                setup_mons_cast(monster, beem, spell_cast);

                // beam-type spells requiring tracers
                if (ms_requires_tracer(spell_cast))
                {
                    fire_tracer(monster, beem);
                    // good idea?
                    if (mons_should_fire(beem))
                        spellOK = true;
                }
                else
                {
                    // all direct-effect/summoning/self-enchantments/etc
                    spellOK = true;
                    if (ms_direct_nasty(spell_cast)
                        && mons_aligned(monster_index(monster), monster->foe))
                    {
                        spellOK = false;
                    }
                }

                // if not okay, then maybe we'll cast a defensive spell
                if (!spellOK)
                    spell_cast = (coinflip() ? hspell_pass[2] : MS_NO_SPELL);

                if (spell_cast != MS_NO_SPELL)
                    break;
            }
        }

        // should the monster *still* not have a spell, well, too bad {dlb}:
        if (spell_cast == MS_NO_SPELL)
            return (false);

        // Try to animate dead: if nothing rises, pretend we didn't cast it
        if (spell_cast == MS_ANIMATE_DEAD
            && !animate_dead( 100, SAME_ATTITUDE(monster), monster->foe, 0 ))
        {
            return (false);
        }

        if (monsterNearby)      // handle monsters within range of player
        {
            if (monster->type == MONS_GERYON)
            {
                if (silenced(monster->x, monster->y))
                    return (false);

                simple_monster_message( monster, " winds a great silver horn.",
                                        MSGCH_MONSTER_SPELL );
            }
            else if (mons_is_demon( monster->type ))
            {
                simple_monster_message( monster, " gestures.",
                                        MSGCH_MONSTER_SPELL );
            }
            else
            {
                switch (monster->type)
                {
                default:
                    if (silenced(monster->x, monster->y))
                        return (false);

                    if (mons_flag(monster->type, M_PRIEST))
                    {
                        switch (random2(3))
                        {
                        case 0:
                            simple_monster_message( monster,
                                                    " prays.",
                                                    MSGCH_MONSTER_SPELL );
                            break;
                        case 1:
                            simple_monster_message( monster,
                                                    " mumbles some strange prayers.",
                                                    MSGCH_MONSTER_SPELL );
                            break;
                        case 2:
                        default:
                            simple_monster_message( monster,
                                                    " utters an invocation.",
                                                    MSGCH_MONSTER_SPELL );
                            break;
                        }
                    }
                    else
                    {
                        switch (random2(3))
                        {
                        case 0:
                            // XXX: could be better, chosen to match the
                            // ones in monspeak.cc... has the problem
                            // that it doesn't suggest a vocal component. -- bwr
                            simple_monster_message( monster,
                                                    " gestures wildly.",
                                                    MSGCH_MONSTER_SPELL );
                            break;
                        case 1:
                            simple_monster_message( monster,
                                                    " mumbles some strange words.",
                                                    MSGCH_MONSTER_SPELL );
                            break;
                        case 2:
                        default:
                            simple_monster_message( monster,
                                                    " casts a spell.",
                                                    MSGCH_MONSTER_SPELL );
                            break;
                        }
                    }
                    break;

                case MONS_BALL_LIGHTNING:
                    monster->hit_points = -1;
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

                case MONS_VAPOUR:
                    mons_add_ench( monster, ENCH_SUBMERGED );
                    break;

                case MONS_BRAIN_WORM:
                case MONS_ELECTRIC_GOLEM:
                    // These don't show any signs that they're casting a spell.
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
                }
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

    return (true);
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
    if (mons_has_ench(monster, ENCH_CONFUSION) || monster->behaviour == BEH_SLEEP)
        return (false);

    if (mons_itemuse(monster->type) == MONUSE_NOTHING)
        return (false);

    const int mon_item = monster->inv[MSLOT_MISSILE];
    if (mon_item == NON_ITEM || !is_valid_item( mitm[mon_item] ))
        return (false);

    // don't allow offscreen throwing.. for now.
    if (monster->foe == MHITYOU && !mons_near(monster))
        return (false);

    // poor 2-headed ogres {dlb}
    if (monster->type == MONS_TWO_HEADED_OGRE || monster->type == MONS_ETTIN)
        return (false);

    // recent addition {GDL} - monsters won't throw if they can do melee.
    // wastes valuable ammo,  and most monsters are better at melee anyway.
    if (adjacent( beem.target_x, beem.target_y, monster->x, monster->y ))
        return (false);

    if (one_chance_in(5))
        return (false);

    // new (GDL) - don't throw idiotic stuff.  It's a waste of time.
    int wepClass = mitm[mon_item].base_type;
    int wepType = mitm[mon_item].sub_type;

    int weapon = monster->inv[MSLOT_WEAPON];

    int lnchClass = (weapon != NON_ITEM) ? mitm[weapon].base_type : -1;
    int lnchType  = (weapon != NON_ITEM) ? mitm[weapon].sub_type  :  0;

    bool thrown = false;
    bool launched = false;

    throw_type( lnchClass, lnchType, wepClass, wepType, launched, thrown );

    if (!launched && !thrown)
        return (false);

    // ok, we'll try it.
    setup_generic_throw( monster, beem );

    // fire tracer
    fire_tracer( monster, beem );

    // good idea?
    if (mons_should_fire( beem ))
    {
        beem.beam_name[0] = '\0';
        return (mons_throw( monster, beem, mon_item ));
    }

    return (false);
}                               // end handle_throw()

//---------------------------------------------------------------
//
// handle_monsters
//
// This is the routine that controls monster AI.
//
//---------------------------------------------------------------
void handle_monsters(void)
{
    bool        brkk = false;
    struct bolt beem;
    int         i;

    FixedArray < unsigned int, 19, 19 > show;

//    losight(show, grd, you.x_pos, you.y_pos);

    for (i = 0; i < MAX_MONSTERS; i++)
    {
        struct monsters *monster = &menv[i];

        if (monster->type != -1)
        {
            if (monster->hit_points > monster->max_hit_points)
                monster->hit_points = monster->max_hit_points;

            // monster just summoned, skip
            if (monster->flags & MF_JUST_SUMMONED)
            {
                monster->flags &= ~MF_JUST_SUMMONED;
                continue;
            }

            monster->speed_increment += (monster->speed * you.time_taken) / 10;

            if (you.slow > 0)
            {
                monster->speed_increment += (monster->speed * you.time_taken) / 10;
            }

            while (monster->speed_increment >= 80)
            {                   // The continues & breaks are WRT this.
                if (monster->type != -1 && monster->hit_points < 1)
                    break;

                monster->speed_increment -= 10;

                if (env.cgrid[monster->x][monster->y] != EMPTY_CLOUD)
                {
                    if (mons_has_ench( monster, ENCH_SUBMERGED ))
                        break;

                    if (monster->type == -1)
                        break;  // problem with vortices

                    mons_in_cloud(monster);

                    if (monster->type == -1)
                    {
                        monster->speed_increment = 1;
                        break;
                    }
                }

                if (monster->type == MONS_GLOWING_SHAPESHIFTER)
                    mons_add_ench(monster, ENCH_GLOWING_SHAPESHIFTER);

                // otherwise there are potential problems with summonings
                if (monster->type == MONS_SHAPESHIFTER)
                    mons_add_ench(monster, ENCH_SHAPESHIFTER);

                // memory is decremented here for a reason -- we only want it
                // decrementing once per monster "move"
                if (monster->foe_memory > 0)
                    monster->foe_memory --;

                handle_behaviour(monster);

                if (handle_enchantment(monster))
                    continue;

                // submerging monsters will hide from clouds
                const int habitat = monster_habitat( monster->type );
                if (habitat != DNGN_FLOOR
                    && habitat == grd[monster->x][monster->y]
                    && env.cgrid[monster->x][monster->y] != EMPTY_CLOUD)
                {
                    mons_add_ench( monster, ENCH_SUBMERGED );
                }

                // regenerate:
                if (monster_descriptor(monster->type, MDSC_REGENERATES)

                    || (monster->type == MONS_FIRE_ELEMENTAL
                        && (grd[monster->x][monster->y] == DNGN_LAVA
                            || env.cgrid[monster->x][monster->y] == CLOUD_FIRE
                            || env.cgrid[monster->x][monster->y] == CLOUD_FIRE_MON))

                    || (monster->type == MONS_WATER_ELEMENTAL
                        && (grd[monster->x][monster->y] == DNGN_SHALLOW_WATER
                            || grd[monster->x][monster->y] == DNGN_DEEP_WATER))

                    || (monster->type == MONS_AIR_ELEMENTAL
                        && env.cgrid[monster->x][monster->y] == EMPTY_CLOUD
                        && one_chance_in(3))

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
                    && (mons_itemuse(monster->type) == MONUSE_WEAPONS_ARMOUR
                        || monster->type == MONS_JELLY
                        || monster->type == MONS_NECROPHAGE
                        || monster->type == MONS_GHOUL))
                {
                    if (handle_pickup(monster))
                        continue;
                }

                // calculates mmov_x, mmov_y based on monster target.
                handle_movement(monster);

                brkk = false;

                if (mons_has_ench( monster, ENCH_CONFUSION )
                    || (monster->type == MONS_AIR_ELEMENTAL
                        && mons_has_ench( monster, ENCH_SUBMERGED )))
                {
                    mmov_x = random2(3) - 1;
                    mmov_y = random2(3) - 1;

                    // bounds check: don't let confused monsters try to run
                    // off the map
                    if (monster->target_x + mmov_x < 0
                            || monster->target_x + mmov_x >= GXM)
                    {
                        mmov_x = 0;
                    }

                    if (monster->target_y + mmov_y < 0
                            || monster->target_y + mmov_y >= GYM)
                    {
                        mmov_y = 0;
                    }

                    if (mgrd[monster->x + mmov_x][monster->y + mmov_y] != NON_MONSTER
                        && (mmov_x != 0 || mmov_y != 0))
                    {
                        mmov_x = 0;
                        mmov_y = 0;

                        if (monsters_fight(i, mgrd[monster->x + mmov_x][monster->y + mmov_y]))
                        {
                            brkk = true;
                        }
                    }
                }

                if (brkk)
                    continue;

                // shapeshifters don't get special abilities
                if (!mons_has_ench(monster, ENCH_GLOWING_SHAPESHIFTER,
                                            ENCH_SHAPESHIFTER))
                {
                    handle_nearby_ability(monster);
                }

                beem.target_x = monster->target_x;
                beem.target_y = monster->target_y;

                if (!(monster->behaviour == BEH_SLEEP
                    || monster->behaviour == BEH_WANDER))
                {
                    // prevents unfriendlies from nuking you from offscreen.
                    // How nice!
                    if (mons_friendly(monster) || mons_near(monster))
                    {
                        // shapeshifters don't get special abilities
                        if (!mons_has_ench(monster, ENCH_GLOWING_SHAPESHIFTER,
                                                    ENCH_SHAPESHIFTER))
                        {
                            if (handle_special_ability(monster, beem))
                                continue;
                        }

                        if (handle_potion(monster, beem))
                            continue;

                        if (handle_scroll(monster))
                            continue;

                        if (handle_wand(monster, beem))
                            continue;

                        // shapeshifters don't get spells
                        if (!mons_has_ench(monster, ENCH_GLOWING_SHAPESHIFTER,
                            ENCH_SHAPESHIFTER))
                        {
                            if (handle_spell(monster, beem))
                                continue;
                        }

                        if (handle_reaching(monster))
                            continue;
                    }

                    if (handle_throw(monster, beem))
                        continue;
                }

                // see if we move into (and fight) an unfriendly monster
                int targmon = mgrd[monster->x + mmov_x][monster->y + mmov_y];
                if (targmon != i && targmon != NON_MONSTER
                    && !mons_aligned(i, targmon))
                {
                    // figure out if they fight
                    if (monsters_fight(i, targmon))
                    {
                        if (testbits(monster->flags, MF_BATTY))
                        {
                            monster->behaviour = BEH_WANDER;
                            monster->foe = MHITNOT;
                            monster->speed_increment -= monster->speed;
                        }

                        mmov_x = 0;
                        mmov_y = 0;
                        brkk = true;
                    }
                }

                if (brkk)
                    continue;

                if (monster->x + mmov_x == you.x_pos
                    && monster->y + mmov_y == you.y_pos)
                {
                    bool isFriendly = mons_friendly(monster);
                    bool attacked = false;
                    if (!isFriendly)
                    {
                        monster_attack(i);
                        attacked = true;

                        if (testbits(monster->flags, MF_BATTY))
                        {
                            monster->behaviour = BEH_WANDER;
                            monster->foe = MHITNOT;
                        }
                    }

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

                    if (attacked)
                    {
                        mmov_x = 0;
                        mmov_y = 0;
                        continue;   //break;
                    }
                }

                if (monster->type == -1 || monster->type == MONS_OKLOB_PLANT
                    || monster->type == MONS_CURSE_SKULL
                    || (monster->type >= MONS_CURSE_TOE
                        && monster->type <= MONS_POTION_MIMIC))
                {
                    continue;
                }

                monster_move(monster);

                // reevaluate behaviour, since the monster's
                // surroundings have changed (it may have moved,
                // or died for that matter.  Don't bother for
                // dead monsters.  :)
                if (monster->type != -1)
                    handle_behaviour(monster);

            }                   // end while

            if (monster->type != -1 && monster->hit_points < 1)
            {
                if (monster->type == MONS_GIANT_SPORE
                    || monster->type == MONS_BALL_LIGHTNING)
                {
                    // detach monster from the grid first, so it
                    // doesn't get hit by its own explosion (GDL)
                    mgrd[monster->x][monster->y] = NON_MONSTER;

                    spore_goes_pop( monster );
                    monster_cleanup( monster );
                    continue;
                }
                else
                {
                    monster_die( monster, KILL_MISC, 0 );
                }
            }
        }                       // end of if (mons_class != -1)
    }                           // end of for loop

    // Clear any summoning flags so that lower indiced
    // monsters get their actions in the next round.
    for (i = 0; i < MAX_MONSTERS; i++)
    {
        menv[i].flags &= ~MF_JUST_SUMMONED;
    }
}                               // end handle_monster()


//---------------------------------------------------------------
//
// handle_pickup
//
// Returns false if monster doesn't spend any time pickup up
//
//---------------------------------------------------------------
static bool handle_pickup(struct monsters *monster)
{
    // single calculation permissible {dlb}
    bool monsterNearby = mons_near(monster);
    int  item = NON_ITEM;

    if (monster->type == MONS_JELLY
        || monster->type == MONS_BROWN_OOZE
        || monster->type == MONS_ACID_BLOB
        || monster->type == MONS_ROYAL_JELLY)
    {
        int hps_gained;
        int quant_eated = 1 + random2(10); // value range: [1,10] {dlb}

        if (quant_eated > mitm[igrd[monster->x][monster->y]].quantity)
            quant_eated = mitm[igrd[monster->x][monster->y]].quantity;

        if (mitm[igrd[monster->x][monster->y]].base_type == OBJ_WEAPONS
            && is_fixed_artefact( mitm[igrd[monster->x][monster->y]] ))
        {
            return (false);       /* unique items */
        }

        if (mitm[igrd[monster->x][monster->y]].base_type == OBJ_MISSILES
            && (mitm[igrd[monster->x][monster->y]].sub_type == MI_STONE
               || mitm[igrd[monster->x][monster->y]].sub_type == MI_LARGE_ROCK))
        {
            /* shouldn't eat stone things.
               - but what about stone wands & rings? */
            return (false);
        }

        if (mitm[igrd[monster->x][monster->y]].base_type != OBJ_GOLD)
        {
            hps_gained = quant_eated * mass_item( mitm[igrd[monster->x][monster->y]] );
            hps_gained /= 20;
        }
        else
        {
            // shouldn't be much trouble to digest a huge pile of gold!
            quant_eated = mitm[igrd[monster->x][monster->y]].quantity;
            if (quant_eated > 500)
                quant_eated = 500 + random2avg(quant_eated - 500,2);
            hps_gained = quant_eated / 10;
        }

        if (hps_gained < 1)
            hps_gained = 1;
        else if (hps_gained > 50)
            hps_gained = 50;

        dec_mitm_item_quantity( igrd[monster->x][monster->y], quant_eated );

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

        return (false);
    }                           // end "if jellies"

    // Note: Monsters only look at top of stacks.
    item = igrd[monster->x][monster->y];

    switch (mitm[item].base_type)
    {
    case OBJ_WEAPONS:
        if (monster->inv[MSLOT_WEAPON] != NON_ITEM)
            return (false);

        if (is_fixed_artefact( mitm[item] ))
            return (false);

        if (is_random_artefact( mitm[item] ))
            return (false);

        // wimpy monsters (Kob, gob) shouldn't pick up halberds etc
        // of course, this also block knives {dlb}:
        if ((mons_charclass(monster->type) == MONS_KOBOLD
                || mons_charclass(monster->type) == MONS_GOBLIN)
            && property( mitm[item], PWPN_HIT ) <= 0)
        {
            return (false);
        }

        // Nobody picks up giant clubs:
        if (mitm[item].sub_type == WPN_GIANT_CLUB
            || mitm[item].sub_type == WPN_GIANT_SPIKED_CLUB)
        {
            return (false);
        }

        monster->inv[MSLOT_WEAPON] = item;

        if (get_weapon_brand(mitm[monster->inv[MSLOT_WEAPON]]) == SPWPN_PROTECTION)
        {
            monster->armour_class += 3;
        }

        if (monsterNearby)
        {
            strcpy(info, ptr_monam(monster, DESC_CAP_THE));
            strcat(info, " picks up ");
            it_name(monster->inv[MSLOT_WEAPON], DESC_NOCAP_A, str_pass);
            strcat(info, str_pass);
            strcat(info, ".");
            mpr(info);
        }
        break;

    case OBJ_MISSILES:
        // fleeing monsters never stop to pick up ammo
        if (monster->behaviour == BEH_FLEE)
            return (false);

        // don't pick up if we're in combat, and there isn't
        // much there
        if (mitm[item].quantity < 5 && monster->behaviour != BEH_WANDER)
        {
            return (false);
        }

        if (monster->inv[MSLOT_MISSILE] != NON_ITEM
            && mitm[monster->inv[MSLOT_MISSILE]].sub_type == mitm[item].sub_type
            && mitm[monster->inv[MSLOT_MISSILE]].plus == mitm[item].plus
            && mitm[monster->inv[MSLOT_MISSILE]].special == mitm[item].special)
        {
            if (monsterNearby)
            {
                strcpy(info, ptr_monam(monster, DESC_CAP_THE));
                strcat(info, " picks up ");
                it_name(item, DESC_NOCAP_A, str_pass);
                strcat(info, str_pass);
                strcat(info, ".");
                mpr(info);
            }

            inc_mitm_item_quantity( monster->inv[MSLOT_MISSILE],
                                    mitm[item].quantity );

            dec_mitm_item_quantity( item, mitm[item].quantity );
            return (true);
        }

        // nobody bothers to pick up rocks if they don't already have some:
        if (mitm[item].sub_type == MI_LARGE_ROCK)
            return (false);

        // monsters with powerful melee attacks don't bother
        if (mons_damage(monster->type, 0) > 5)
            return (false);

        monster->inv[MSLOT_MISSILE] = item;

        if (monsterNearby)
        {
            strcpy(info, ptr_monam(monster, DESC_CAP_THE));
            strcat(info, " picks up ");
            it_name(monster->inv[MSLOT_MISSILE], DESC_NOCAP_A, str_pass);
            strcat(info, str_pass);
            strcat(info, ".");
            mpr(info);
        }
        break;

    case OBJ_WANDS:
        if (monster->inv[MSLOT_WAND] != NON_ITEM)
            return (false);

        monster->inv[MSLOT_WAND] = item;

        if (monsterNearby)
        {
            strcpy(info, ptr_monam(monster, DESC_CAP_THE));
            strcat(info, " picks up ");
            it_name(monster->inv[MSLOT_WAND], DESC_NOCAP_A, str_pass);
            strcat(info, str_pass);
            strcat(info, ".");
            mpr(info);
        }
        break;

    case OBJ_SCROLLS:
        if (monster->inv[MSLOT_SCROLL] != NON_ITEM)
            return (false);

        monster->inv[MSLOT_SCROLL] = item;

        if (monsterNearby)
        {
            strcpy(info, ptr_monam(monster, DESC_CAP_THE));
            strcat(info, " picks up ");
            it_name(monster->inv[MSLOT_SCROLL], DESC_NOCAP_A, str_pass);
            strcat(info, str_pass);
            strcat(info, ".");
            mpr(info);
        }
        break;

    case OBJ_POTIONS:
        if (monster->inv[MSLOT_POTION] != NON_ITEM)
            return (false);

        monster->inv[MSLOT_POTION] = item;

        if (monsterNearby)
        {
            strcpy(info, ptr_monam(monster, DESC_CAP_THE));
            strcat(info, " picks up ");
            it_name(monster->inv[MSLOT_POTION], DESC_NOCAP_A, str_pass);
            strcat(info, str_pass);
            strcat(info, ".");
            mpr(info);
        }
        break;

    case OBJ_CORPSES:
        if (monster->type != MONS_NECROPHAGE && monster->type != MONS_GHOUL)
            return (false);

        monster->hit_points += 1 + random2(mons_weight(mitm[item].plus)) / 100;

        // limited growth factor here -- should 77 really be the cap? {dlb}:
        if (monster->hit_points > 100)
            monster->hit_points = 100;

        if (monster->hit_points > monster->max_hit_points)
            monster->max_hit_points = monster->hit_points;

        if (monsterNearby)
        {
            strcpy(info, ptr_monam(monster, DESC_CAP_THE));
            strcat(info, " eats ");
            it_name(item, DESC_NOCAP_THE, str_pass);
            strcat(info, str_pass);
            strcat(info, ".");
            mpr(info);
        }

        destroy_item( item );
        return (true);

    case OBJ_GOLD: //mv - monsters now pick up gold (19 May 2001)
        if (monsterNearby)
        {

            strcpy(info, monam( monster->number, monster->type,
                                player_monster_visible( monster ),
                                DESC_CAP_THE ));

            strcat(info, " picks up some gold.");
            mpr(info);
        }

        if (monster->inv[MSLOT_GOLD] != NON_ITEM)
        {
            // transfer gold to monster's object, destroy ground object
            inc_mitm_item_quantity( monster->inv[MSLOT_GOLD],
                                    mitm[item].quantity );

            destroy_item( item );
            return (true);
        }
        else
        {
            monster->inv[MSLOT_GOLD] = item;
        }
        break;

    default:
        return (false);
    }

    // Item has been picked-up, move to monster inventory.
    mitm[item].x = 0;
    mitm[item].y = 0;

    // Monster's only take the top item of stacks, so relink the
    // top item, and unlink the item.
    igrd[monster->x][monster->y] = mitm[item].link;
    mitm[item].link = NON_ITEM;

    if (monster->speed_increment > 25)
        monster->speed_increment -= monster->speed;

    return (true);
}                               // end handle_pickup()

static void monster_move(struct monsters *monster)
{
    FixedArray < bool, 3, 3 > good_move;
    int count_x, count_y;
    int okmove = DNGN_SHALLOW_WATER;

    const int habitat = monster_habitat( monster->type );

    // let's not even bother with this if mmov_x and mmov_y are zero.
    if (mmov_x == 0 && mmov_y == 0)
        return;

    // effectively slows down monster movement across water.
    // Fire elementals can't cross at all.
    if (monster->type == MONS_FIRE_ELEMENTAL || one_chance_in(5))
        okmove = DNGN_WATER_STUCK;

    if (mons_flies(monster) > 0 || habitat != DNGN_FLOOR)
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
            int target_grid = grd[targ_x][targ_y];

            const int targ_cloud = env.cgrid[ targ_x ][ targ_y ];
            const int curr_cloud = env.cgrid[ monster->x ][ monster->y ];

            // bounds check - don't consider moving out of grid!
            if (targ_x < 0 || targ_x >= GXM || targ_y < 0 || targ_y >= GYM)
            {
                good_move[count_x][count_y] = false;
                continue;
            }

            if (monster->type == MONS_BORING_BEETLE)
            {
                if (target_grid == DNGN_ROCK_WALL)
                {
                    // don't burrow out of bounds
                    if (!(targ_x > 7 && targ_x < (GXM - 8)
                    && targ_y > 7 && targ_y < (GYM - 8)))
                    {
                        good_move[count_x][count_y] = false;
                        continue;
                    }

                    // don't burrow at an angle (legacy behaviour)
                    if (count_x != 1 && count_y != 1)
                    {
                        good_move[count_x][count_y] = false;
                        continue;
                    }
                }
            }

            if (grd[ targ_x ][ targ_y ] < okmove)
            {
                good_move[count_x][count_y] = false;
                continue;
            }

            if (!habitat_okay( monster, target_grid ))
            {
                good_move[count_x][count_y] = false;
                continue;
            }

            if (monster->type == MONS_WANDERING_MUSHROOM
                && see_grid(targ_x, targ_y))
            {
                good_move[count_x][count_y] = false;
                continue;
            }

            if (monster->type == MONS_WATER_ELEMENTAL
                && (target_grid == DNGN_LAVA
                    || targ_cloud == CLOUD_FIRE
                    || targ_cloud == CLOUD_FIRE_MON))
            {
                good_move[count_x][count_y] = false;
                continue;
            }

            if (monster->type == MONS_FIRE_ELEMENTAL
                && (target_grid == DNGN_DEEP_WATER
                    || target_grid == DNGN_SHALLOW_WATER
                    || target_grid == DNGN_BLUE_FOUNTAIN))
            {
                good_move[count_x][count_y] = false;
                continue;
            }

            // smacking the player is always a good move if
            // we're hostile (even if we're heading somewhere
            // else)

            // smacking another monster is good,  if the monsters
            // are aligned differently
            if (mgrd[targ_x][targ_y] != NON_MONSTER)
            {
                if (mons_aligned(monster_index(monster), mgrd[targ_x][targ_y]))
                {
                    good_move[count_x][count_y] = false;
                    continue;
                }
            }

            // wandering through a trap is OK if we're pretty healthy, or
            // really stupid.
            if (trap_at_xy(targ_x,targ_y) >= 0
                && mons_intel(monster->type) != I_PLANT)
            {
                if (monster->hit_points < monster->max_hit_points / 2)
                {
                    good_move[count_x][count_y] = false;
                    continue;
                }
            }

            if (targ_cloud != EMPTY_CLOUD)
            {
                if (curr_cloud != EMPTY_CLOUD
                    && env.cloud[targ_cloud].type == env.cloud[curr_cloud].type)
                {
                    continue;
                }

                switch (env.cloud[ targ_cloud ].type)
                {
                case CLOUD_FIRE:
                case CLOUD_FIRE_MON:
                    if (mons_res_fire(monster) > 0)
                        continue;

                    if (monster->hit_points >= 15 + random2avg(46, 5))
                        continue;
                    break;

                case CLOUD_STINK:
                case CLOUD_STINK_MON:
                    if (mons_res_poison(monster) > 0)
                        continue;
                    if (1 + random2(5) < monster->hit_dice)
                        continue;
                    if (monster->hit_points >= random2avg(19, 2))
                        continue;
                    break;

                case CLOUD_COLD:
                case CLOUD_COLD_MON:
                    if (mons_res_cold(monster) > 0)
                        continue;

                    if (monster->hit_points >= 15 + random2avg(46, 5))
                        continue;
                    break;

                case CLOUD_POISON:
                case CLOUD_POISON_MON:
                    if (mons_res_poison(monster) > 0)
                        continue;

                    if (monster->hit_points >= random2avg(37, 4))
                        continue;
                    break;

                // this isn't harmful,  but dumb critters might think so.
                case CLOUD_GREY_SMOKE:
                case CLOUD_GREY_SMOKE_MON:
                    if (mons_intel(monster->type) > I_ANIMAL || coinflip())
                        continue;

                    if (mons_res_fire(monster) > 0)
                        continue;

                    if (monster->hit_points >= random2avg(19, 2))
                        continue;
                    break;

                default:
                    continue;   // harmless clouds
                }

                // if we get here, the cloud is potentially harmful.
                // exceedingly dumb creatures will still wander in.
                if (mons_intel(monster->type) != I_PLANT)
                    good_move[count_x][count_y] = false;
            }
        }
    } // now we know where we _can_ move.

    // normal/smart monsters know about secret doors (they _live_ in the
    // dungeon!)
    if (grd[monster->x + mmov_x][monster->y + mmov_y] == DNGN_CLOSED_DOOR
        || (grd[monster->x + mmov_x][monster->y + mmov_y] == DNGN_SECRET_DOOR
            && (mons_intel(monster_index(monster)) == I_HIGH
                || mons_intel(monster_index(monster)) == I_NORMAL)))
    {
        if (monster->type == MONS_ZOMBIE_SMALL
            || monster->type == MONS_ZOMBIE_LARGE
            || monster->type == MONS_SIMULACRUM_SMALL
            || monster->type == MONS_SIMULACRUM_LARGE
            || monster->type == MONS_SKELETON_SMALL
            || monster->type == MONS_SKELETON_LARGE
            || monster->type == MONS_SPECTRAL_THING)
        {
            // for zombies, monster type is kept in mon->number
            if (mons_itemuse(monster->number) > MONUSE_OPEN_DOORS)
            {
                grd[monster->x + mmov_x][monster->y + mmov_y] = DNGN_OPEN_DOOR;
                return;
            }
        }
        else if (mons_itemuse(monster->type) > MONUSE_OPEN_DOORS)
        {
            grd[monster->x + mmov_x][monster->y + mmov_y] = DNGN_OPEN_DOOR;
            return;
        }
    } // endif - secret/closed doors

    // jellies eat doors.  Yum!
    if ((grd[monster->x + mmov_x][monster->y + mmov_y] == DNGN_CLOSED_DOOR
        || grd[monster->x + mmov_x][monster->y + mmov_y] == DNGN_OPEN_DOOR)
        && (monster->type == MONS_JELLY
            || monster->type == MONS_BROWN_OOZE
            || monster->type == MONS_ACID_BLOB
            || monster->type == MONS_ROYAL_JELLY))
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
    } // done door-eating jellies

    // take care of beetle burrowing
    if (monster->type == MONS_BORING_BEETLE)
    {
        if (grd[monster->x + mmov_x][monster->y + mmov_y] == DNGN_ROCK_WALL
            && good_move[mmov_x + 1][mmov_y + 1] == true)
        {
            grd[monster->x + mmov_x][monster->y + mmov_y] = DNGN_FLOOR;

            if (!silenced(you.x_pos, you.y_pos))
                mpr("You hear a grinding noise.");

            // I find it too powerful that these things can burrow and
            // move in the same turn,  but I'll leave it for now.  -- GDL
        }
    }

    // now,  if a monster can't move in its intended direction,  try
    // either side.  If they're both good,  move in whichever dir
    // gets it closer(farther for fleeing monsters) to its target.
    // If neither does,  do nothing.
    if (good_move[mmov_x + 1][mmov_y + 1] == false)
    {
        int current_distance = distance(monster->x, monster->y,
            monster->target_x, monster->target_y);

        int dir = -1;
        int i, mod, newdir;

        for(i=0; i<8; i++)
        {
            if (compass_x[i] == mmov_x && compass_y[i] == mmov_y)
            {
                dir = i;
                break;
            }
        }

        if (dir < 0)
            goto forget_it;

        int dist[2];

        // first 1 away,  then 2 (3 is silly)
        for (int j=1; j<3; j++)
        {
            int sdir, inc;

            if (coinflip())
            {
                sdir = -j;
                inc = 2*j;
            }
            else
            {
                sdir = j;
                inc = -2*j;
            }

            // try both directions
            for(mod = sdir, i=0; i<2; mod += inc, i++)
            {
                newdir = (dir + 8 + mod) % 8;
                if (good_move[compass_x[newdir] + 1][compass_y[newdir] + 1])
                {
                    dist[i] = distance( monster->x + compass_x[newdir],
                                        monster->y + compass_y[newdir],
                                        monster->target_x, monster->target_y );
                }
                else
                {
                    dist[i] = (monster->behaviour == BEH_FLEE) ? (-FAR_AWAY)
                                                               : FAR_AWAY;
                }
            }

            // now choose
            if (dist[0] == dist[1] && abs(dist[0]) == FAR_AWAY)
                continue;

            // which one was better? -- depends on FLEEING or not
            if (monster->behaviour == BEH_FLEE)
            {
                if (dist[0] >= dist[1] && dist[0] > current_distance)
                {
                    mmov_x = compass_x[((dir+8)+sdir)%8];
                    mmov_y = compass_y[((dir+8)+sdir)%8];
                    break;
                }
                if (dist[1] >= dist[0] && dist[1] > current_distance)
                {
                    mmov_x = compass_x[((dir+8)-sdir)%8];
                    mmov_y = compass_y[((dir+8)-sdir)%8];
                    break;
                }
            }
            else
            {
                if (dist[0] <= dist[1] && dist[0] < current_distance)
                {
                    mmov_x = compass_x[((dir+8)+sdir)%8];
                    mmov_y = compass_y[((dir+8)+sdir)%8];
                    break;
                }
                if (dist[1] <= dist[0] && dist[1] < current_distance)
                {
                    mmov_x = compass_x[((dir+8)-sdir)%8];
                    mmov_y = compass_y[((dir+8)-sdir)%8];
                    break;
                }
            }
        }
    } // end - try to find good alternate move

forget_it:

    // ------------------------------------------------------------------
    // if we haven't found a good move by this point, we're not going to.
    // ------------------------------------------------------------------

    mgrd[monster->x][monster->y] = NON_MONSTER;

    if (good_move[mmov_x + 1][mmov_y + 1] && !(mmov_x == 0 && mmov_y == 0))
    {
        // check for attacking player
        if (monster->x + mmov_x == you.x_pos
            && monster->y + mmov_y == you.y_pos)
        {
            monster_attack(monster_index(monster));
            mmov_x = 0;
            mmov_y = 0;
        }

        // check for attacking another monster
        int targmon = mgrd[monster->x + mmov_x][monster->y + mmov_y];
        if (targmon != NON_MONSTER)
        {
            monsters_fight(monster_index(monster), targmon);
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
    else
    {
        // fleeing monsters that can't move will panic and possibly
        // turn to face their attacker
        if (monster->behaviour == BEH_FLEE)
            behaviour_event(monster, ME_CORNERED);
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
    pbolt.damage = dice_def( 3, 7 );
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

    return (didSpit);
}                               // end plant_spit()

static void mons_in_cloud(struct monsters *monster)
{
    int wc = env.cgrid[monster->x][monster->y];
    int hurted = 0;
    struct bolt beam;

    if (mons_charclass(monster->type) == MONS_GOLD_MIMIC)
    {
        mimic_alert(monster);
        return;
    }

    switch (env.cloud[wc].type)
    {
    case CLOUD_DEBUGGING:
        cprintf("Fatal error: monster steps on nonexistent cloud!");
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

        if (mons_res_fire(monster) > 0)
            return;

        hurted += ((random2avg(16, 3) + 6) * 10) / monster->speed;

        if (mons_res_fire(monster) < 0)
            hurted += (random2(15) * 10) / monster->speed;

        // remember that the above is in addition to the other you.damage.
        hurted -= random2(1 + monster->armour_class);
        break;                  // to damage routine at end {dlb}

    case CLOUD_STINK:
    case CLOUD_STINK_MON:
        simple_monster_message(monster, " is engulfed in noxious gasses!");

        if (mons_res_poison(monster) > 0)
            return;

        beam.colour = RED;

        if (1 + random2(27) >= monster->hit_dice)
            mons_ench_f2(monster, beam);

        hurted += (random2(3) * 10) / monster->speed;
        break;                  // to damage routine at end {dlb}

    case CLOUD_COLD:
    case CLOUD_COLD_MON:
        simple_monster_message(monster, " is engulfed in freezing vapours!");

        if (mons_res_cold(monster) > 0)
            return;

        hurted += ((6 + random2avg(16, 3)) * 10) / monster->speed;

        if (mons_res_cold(monster) < 0)
            hurted += (random2(15) * 10) / monster->speed;

        // remember that the above is in addition to the other damage.
        hurted -= random2(1 + monster->armour_class);
        break;                  // to damage routine at end {dlb}

    // what of armour of poison resistance here? {dlb}
    case CLOUD_POISON:
    case CLOUD_POISON_MON:
        simple_monster_message(monster, " is engulfed in a cloud of poison!");

        if (mons_res_poison(monster) > 0)
            return;

        poison_monster(monster, (env.cloud[wc].type == CLOUD_POISON));

        hurted += (random2(8) * 10) / monster->speed;

        if (mons_res_poison(monster) < 0)
            hurted += (random2(4) * 10) / monster->speed;
        break;                  // to damage routine at end {dlb}

    case CLOUD_STEAM:
    case CLOUD_STEAM_MON:
        // couldn't be bothered coding for armour of res fire

        // what of whether it is wearing steam dragon armour? {dlb}
        if (monster->type == MONS_STEAM_DRAGON)
            return;

        simple_monster_message(monster, " is engulfed in steam!");

        if (mons_res_fire(monster) > 0)
            return;

        hurted += (random2(6) * 10) / monster->speed;

        if (mons_res_fire(monster) < 0)
            hurted += (random2(6) * 10) / monster->speed;

        hurted -= random2(1 + monster->armour_class);
        break;                  // to damage routine at end {dlb}

    case CLOUD_MIASMA:
    case CLOUD_MIASMA_MON:
        simple_monster_message(monster, " is engulfed in a dark miasma!");

        if (mons_holiness(monster->type) != MH_NATURAL)
            return;

        poison_monster(monster, (env.cloud[wc].type == CLOUD_MIASMA));

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
            switch (env.cloud[wc].type)
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

            switch (env.cloud[wc].type)
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
    case MONS_SWAMP_WORM:
    case MONS_WATER_ELEMENTAL:
        return (DNGN_DEEP_WATER); // no shallow water (only) monsters? {dlb}
        // must remain DEEP_WATER for now, else breaks code {dlb}

    case MONS_LAVA_WORM:
    case MONS_LAVA_FISH:
    case MONS_LAVA_SNAKE:
    case MONS_SALAMANDER:
        return (DNGN_LAVA);

    default:
        return (DNGN_FLOOR);      // closest match to terra firma {dlb}
    }
}                               // end monster_habitat()

bool monster_descriptor(int which_class, unsigned char which_descriptor)
{
    if (which_descriptor == MDSC_LEAVES_HIDE)
    {
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
            return (true);
        default:
            return (false);
        }
    }

    if (which_descriptor == MDSC_REGENERATES)
    {
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
            return (true);
        default:
            return (false);
        }
    }

    if (which_descriptor == MDSC_NOMSG_WOUNDS)
    {
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
            return (true);
        default:
            return (false);
        }
    }
    return (false);
}                               // end monster_descriptor()

bool message_current_target(void)
{
    if (!(you.prev_targ == MHITNOT || you.prev_targ == MHITYOU))
    {
        struct monsters *montarget = &menv[you.prev_targ];

        if (mons_near(montarget) && player_monster_visible( montarget ))
        {
            strcpy(info, "You are currently targeting ");
            strcat(info, ptr_monam(montarget, DESC_NOCAP_THE));
            strcat(info, " (p to target).");
            mpr(info);

            return (true);        // early return {dlb}
        }
        else
            mpr("You have no current target.");
    }

    return (false);
}                               // end message_current_target()

// aaah, the simple joys of pointer arithmetic! {dlb}:
unsigned int monster_index(struct monsters *monster)
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
        return (false);
    else if (!permit_growth && patient->hit_points == patient->max_hit_points)
        return (false);
    else
    {
        if ((patient->hit_points += health_boost) > patient->max_hit_points)
        {
            if (permit_growth)
                patient->max_hit_points++;

            patient->hit_points = patient->max_hit_points;
        }
    }

    return (true);
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

    return (mzap);
}

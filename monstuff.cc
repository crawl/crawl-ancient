/*
 *  File:       monstuff.cc
 *  Summary:    Misc monster related functions.
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 *               <2>     11/23/99       LRH             Upgraded monster AI
 *               <1>     -/--/--        LRH     Created
 */

#include "AppHdr.h"
#include "monstuff.h"

#include <stdlib.h>
#include <string.h>

#ifdef DOS
#include <conio.h>
#endif

#include "externs.h"
#include "enum.h"

#include "beam.h"
#include "debug.h"
#include "fight.h"
#include "itemname.h"
#include "misc.h"
#include "monplace.h"
#include "mstruct.h"
#include "mstuff2.h"
#include "ouch.h"
#include "player.h"
#include "spells2.h"
#include "stuff.h"
#include "view.h"

char mons_pickup(int i);
void plant_spit(int i, struct bolt beam[1]);
void monster_move(int i);
void mons_in_cloud(int i);
char mons_speaks(int i);
void simple_monster_message(int i, monsters& monster, const char *event);

char mmov_x, mmov_y;

char curse_an_item(char which, char power)
{
/* use which later, if I want to curse weapon/gloves whatever
   which, for now: 0 = non-mummy, 1 = mummy (potions as well)
   don't change mitm.special of !odecay */

/* I think this is wrong??? */

    char possib[52];
    char cu = power;

    cu = 0;
    char cu1 = 0;
    char cu2 = 0;
    char cu3 = 0;


    for (cu = 0; cu < 52; cu++)
    {
        possib[cu] = 100;
    }

    for (cu = 0; cu < 52; cu++)
    {
        if (you.inv_quantity[cu] == 0)
            continue;

        cu1 = you.inv_class[cu];

/*  if (cu1 == OBJ_WEAPONS || cu1 == OBJ_ARMOUR)
   {
   if (you.inv_dam [cu] % 30 >= 25) continue; / * no randarts * /
   } */

        if (cu1 == OBJ_JEWELLERY)
        {
            if (you.inv_dam[cu] == 200)
                continue;       /* no randarts */
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

        you.inv_type[possib[cu3]] = POT_DECAY;  /* don't change you.inv_dam (just for fun) */
        return 1;
        /* problem: changes large piles of potions */
    }

    you.inv_plus[possib[cu3]] += 100;

    return 1;

}


void monster_blink(int mn)
{

/* I can't be bothered writing an intelligent function, so I'll make it ugly: */

     if (menv[mn].type == MONS_TUNNELING_WORM || menv[mn].type == MONS_WORM_TAIL)
          return;                 /* Do not let this happen! */

     int passed[2];

     if (random_near_space(passed) == 0)
          return;

     mgrd[menv[mn].x][menv[mn].y] = MNG;

     menv[mn].x = passed[0];
     menv[mn].y = passed[1];

     mgrd[menv[mn].x][menv[mn].y] = mn;

}

char random_near_space(int passed[2])
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
        if (tries >= 150)
            return 0;
    }
    while (see_grid(blx, bly) == 0 || grd[blx][bly] < 65 || mgrd[blx][bly] != MNG || (you.x_pos == blx && you.y_pos == bly));

    passed[0] = blx;
    passed[1] = bly;

    return 1;
}


bool monster_regenerates(int monster_type)
{
    if (monster_type == MONS_IMP
                    || monster_type == MONS_TROLL
                    || monster_type == MONS_SLIME_CREATURE
                    || monster_type == MONS_ROCK_TROLL
                    || monster_type == MONS_IRON_TROLL
                    || monster_type == MONS_CACODEMON
                    || monster_type == MONS_LEMURE
                    || monster_type == MONS_HELLWING
                    || monster_type == MONS_ANITA
                    || monster_type == MONS_DEEP_TROLL)
    {
        return( true );
    }
    else
    {
        return( false );
    }
}


int distance(int x1, int x2, int y1, int y2)
{
// not accurate (at all), but its main purpose is to see whether you're next to something or not.
    int xdisp = x1 - x2;

    if (xdisp < 0)
        xdisp *= -1;

    int ydisp = y1 - y2;

    if (ydisp < 0)
        ydisp *= -1;

    if (xdisp == 1 && ydisp == 1)
        ydisp = 0;
//itoa(xdisp + ydisp, st_prn, 10);
    //strcpy(info, st_prn);
    //mpr(info);

    return xdisp + ydisp;
}



void swap_places(int swap)
{

    strcpy(info, "You swap places.");
    mpr(info);

//grd [mons_x [swap]] [mons_y [swap]] -= 50;
    mgrd[menv[swap].x][menv[swap].y] = MNG;

    menv[swap].x = you.x_pos;
    menv[swap].y = you.y_pos;

//grd [mons_x [swap]] [mons_y [swap]] += 50;
    mgrd[menv[swap].x][menv[swap].y] = swap;

}                               // end of swap_places



void print_wounds(int wounded)
{

    if (menv[wounded].type == -1)
        return;

    if (menv[wounded].type == MONS_SMALL_ZOMBIE || menv[wounded].type == MONS_BIG_ZOMBIE || menv[wounded].type == MONS_SMALL_SKELETON || menv[wounded].type == MONS_LARGE_SKELETON || menv[wounded].type == MONS_RAKSHASA || menv[wounded].type == MONS_FAKE_RAKSHASA)
        return;

    if (menv[wounded].enchantment[2] == 6 && player_see_invis() == 0)
        return;

    if (!mons_near(wounded))
        return;

    if (menv[wounded].hit_points == menv[wounded].max_hit_points || menv[wounded].hit_points <= 0)
        return;

    strcpy(info, monam(menv[wounded].number, menv[wounded].type, menv[wounded].enchantment[2], 0));
    strcat(info, " is");

    if (menv[wounded].hit_points <= menv[wounded].max_hit_points / 6)
    {
        strcat(info, " almost");
        if (wounded_damaged(menv[wounded].type) == 0)
            strcat(info, " dead.");
        else
            strcat(info, " destroyed.");
        mpr(info);
        return;
    }

    if (menv[wounded].hit_points <= menv[wounded].max_hit_points / 4)
    {
        strcat(info, " horribly ");
        goto wounded_print;
    }

    if (menv[wounded].hit_points <= menv[wounded].max_hit_points / 3)
    {
        strcat(info, " heavily ");
        goto wounded_print;
    }

/*if (menv[wounded].hit_points <= menv[wounded].max_hit_points / 2)
   {
   strcat(info, " somewhat ");
   goto wounded_print;
   } */

    if (menv[wounded].hit_points <= (menv[wounded].max_hit_points / 4) * 3)
    {
        strcat(info, " moderately ");
        goto wounded_print;
    }

    strcat(info, " lightly ");

wounded_print:
    if (wounded_damaged(menv[wounded].type) == 0)
        strcat(info, "wounded.");
    else
        strcat(info, "damaged.");
    mpr(info);
}


int wounded_damaged(int wound_class)
{
    if (mons_holiness(wound_class) == MH_UNDEAD)
        return 1;
    if (wound_class >= MONS_CLAY_GOLEM && wound_class <= MONS_TOENAIL_GOLEM)
        return 1;
    if (wound_class == MONS_FUNGUS || wound_class == MONS_SMALL_ABOMINATION || wound_class == MONS_PLANT || wound_class == MONS_FIRE_VORTEX || wound_class == MONS_LARGE_ABOMINATION || wound_class == MONS_DANCING_WEAPON || wound_class == MONS_SPATIAL_VORTEX)
        return 1;               // 'damaged'

    return 0;                   // 'wounded'

}


// $$$$$$$$$ start of changes

//---------------------------------------------------------------
//
// handle_behavior
//
// Uses the monster's behavior flag to adjust the square the
// monster will move towards.
//
//---------------------------------------------------------------
static void handle_behavior(int i)
{
    ASSERT(i >= 0);
    ASSERT(i < MNST);

    monsters & monster = env.mons[i];

    switch (monster.behavior)
    {
    case BEH_SLEEP:
        monster.target_x = monster.x;
        monster.target_y = monster.y;
        break;

    case BEH_CHASING_II:
    case BEH_CHASING_I:
        if ((mons_near(i) && (you.invis == 0 || mons_see_invis(monster.type) != 0 || (grd[you.x_pos][you.y_pos] == 65 && you.levitation == 0))) || (!mons_near(i) && monster.target_x == monster.x && monster.target_y == monster.y))
        {
            monster.target_x = you.x_pos;
            monster.target_y = you.y_pos;
        }
        break;

    case BEH_WANDER:
        if ((monster.target_x == monster.x && monster.target_y == monster.y) || random2(20) == 0)
        {
            monster.target_x = random2(80);
            monster.target_y = random2(70);
        }
        break;

    case BEH_FLEE:
        if (mons_near(i) && (you.invis == 0 || mons_see_invis(monster.type) > 0))
        {
            monster.target_x = you.x_pos;
            monster.target_y = you.y_pos;
        }

        if (monster.hit_points > (monster.max_hit_points / 2))
            monster.behavior = BEH_CHASING_I;   // change the '= 1' to whatever monster used to be doing

        break;

    case BEH_FIGHT:             // attacking other monster

        if (monster.monster_foe == MHITNOT)
        {
            monster.behavior = BEH_WANDER;      // ?

        }
        else
        {
            monster.target_x = menv[monster.monster_foe].x;
            monster.target_y = menv[monster.monster_foe].y;
        }
        break;

    case BEH_ENSLAVED:          //To be nice (although not strictly accurate) they should follow you even when they can't see you.

        if (you.pet_target != MHITNOT)
        {
            if (monster.monster_foe == MHITNOT)
            {
                monster.monster_foe = you.pet_target;
            }
        }

        if (monster.monster_foe == i || menv[monster.monster_foe].behavior == BEH_ENSLAVED)
            monster.monster_foe = MHITNOT;      // I don't know why I have to do that.

        if (monster.monster_foe != MHITNOT && ((mons_near(i) && mons_near(monster.monster_foe)) || distance(monster.x, menv[monster.monster_foe].x, monster.y, menv[monster.monster_foe].y) == 1))
        {
            monster.target_x = menv[monster.monster_foe].x;
            monster.target_y = menv[monster.monster_foe].y;

        }
        else if (mons_near(i) && (monster.target_x == monster.x && monster.target_y == monster.y) || random2(3) == 0)
        {
            if (random2(7) == 0)
            {
                monster.target_x = random2(80);
                monster.target_y = random2(70);

            }
            else
            {
                monster.target_x = you.x_pos;
                monster.target_y = you.y_pos;
            }
        }

        if (!mons_near(i))
        {
            monster.target_x = you.x_pos;
            monster.target_y = you.y_pos;
        }
        break;
    }
}


//---------------------------------------------------------------
//
// handle_enchantment
//
// Handle the result of a spell cast upon a monster. Returns
// true if the monster died.
//
//---------------------------------------------------------------

static void flag_ench(monsters& monster, int p)
{
     monster.enchantment[p] = ENCH_NONE;
     if (monster.enchantment[0] == ENCH_NONE && monster.enchantment[1] == ENCH_NONE && monster.enchantment[2] == ENCH_NONE)
          monster.enchantment1 = 0;
}

void simple_monster_message(int i, monsters& monster, const char *event)
{
    if (mons_near(i) && (monster.enchantment[2] != 6 || player_see_invis() != 0))
    {
         strcpy(info, monam(monster.number, monster.type, monster.enchantment[2], 0));
         strcat(info, event);
         mpr(info);
    }
}

#define POISONVAL(p) ((p) % 50)

static bool handle_enchantment(int i)
{
    ASSERT(i >= 0);
    ASSERT(i < MNST);

    bool died = false;

    monsters & monster = env.mons[i];

    if (monster.enchantment1 != 0)
    {
        for (int p = 0; p < 3 && !died; p++)
        {
            switch (monster.enchantment[p])
            {
            case ENCH_SLOW:

                if (random2(250) <= monster.hit_dice + 10)
                {
                    if (monster.speed >= 100)
                    {
                        monster.speed = ((monster.speed - 100) * 2) + 100;
                    }
                    else
                        monster.speed *= 2;
                    flag_ench(monster,p);
                    simple_monster_message(i, monster, " seems to speed up.");
                }
                break;

            case ENCH_HASTE:             // haste

                if (random2(20) == 0)
                {
                    if (monster.speed >= 100)
                    {
                        monster.speed = ((monster.speed - 100) / 2) + 100;
                    }
                    else
                        monster.speed /= 2;
                    flag_ench(monster,p);
                    simple_monster_message(i, monster, " seems to slow down.");
                }
                break;

            case ENCH_FEAR:             // fear

                monster.behavior = BEH_FLEE;
                if (random2(150) <= monster.hit_dice + 10)
                {
                    monster.behavior = BEH_CHASING_I;   // reset to monster's original behaviour
                    simple_monster_message(i, monster, " seems to regain its courage.");
                    flag_ench(monster,p);
                }
                break;

            case ENCH_CONFUSION:             // confusion

                monster.behavior = BEH_CONFUSED;
                if (random2(80) < monster.hit_dice + 10)
                {
                    if (monster.type != MONS_BUTTERFLY && monster.type != MONS_FIRE_VORTEX && monster.type != MONS_SPATIAL_VORTEX && monster.type != MONS_VAPOUR)
                    {
                        monster.behavior = BEH_CHASING_I;
                        simple_monster_message(i, monster, " seems less confused.");
                        flag_ench(monster,p);
                    }
                }
                break;

            case ENCH_INVIS:             // invisibility

                if (random2(20) == 0 || (monster.type >= MLAVA0 && monster.number == 0) || (monster.type == 125 && random2(3) == 0))
                {
                    if (!mons_flag(monster.type, M_INVIS))
                    {           // invisible monsters stay invisible

                        if (monster.type < MLAVA0 || monster.number != 1)
                        {
                           flag_ench(monster,p);
                           // Can't use simple_message, because we want
                           //  'a thing appears' (not 'the')
                           // Note: this message not printed if the player
                           //  could already see the monster.
                           if (mons_near(i) && (player_see_invis() == 0 || monster.type >= MLAVA0))
                           {
                                strcpy(info, monam(monster.number, monster.type, monster.enchantment[2], 2));
                                strcat(info, " appears!");
                                mpr(info);
                           }
                        }
                    }
                }
                break;

            case ENCH_POISON_I:            // poison
            case ENCH_POISON_II:            // worse poison
            case ENCH_POISON_III:            // worser poison
            case ENCH_POISON_IV:            // worsest poison
            case ENCH_YOUR_POISON_I:             // your poison
            case ENCH_YOUR_POISON_II:             // your worse poison
            case ENCH_YOUR_POISON_III:             // your worser poison
            case ENCH_YOUR_POISON_IV:            // your worsest poison
              {
              int poisonval=POISONVAL(monster.enchantment[p]);
                if (random2(2) == 0)
                    monster.hit_points -= random2(poisonval - 3) + 1;

                if (mons_res_poison(monster.type) < 0)
                    monster.hit_points -= random2(poisonval - 5) + random2(poisonval - 5) + 1;

                if (monster.hit_points <= 0)
                {
                    if (monster.enchantment[p] < ENCH_POISON_I)
                        monster_die(i, KILL_YOU, 0);   // ie you

                    else
                        monster_die(i, KILL_MISC, 0);
                    died = true;
                }

                if (random2(5) == 0)
                {               // ((30 - mons_HD [i]) <= 0) // LOOK AT THIS!!!

                    monster.enchantment[p]--;   //= 0;

                    if (POISONVAL(monster.enchantment[p]) == 6)
                    {
                         flag_ench(monster,p);
                    }
                }
              }
              break;

              /* assumption: mons_res_fire has already been checked */
            case ENCH_STICKY_FLAME_I:
            case ENCH_STICKY_FLAME_II:
            case ENCH_STICKY_FLAME_III:
            case ENCH_STICKY_FLAME_IV:
            case ENCH_YOUR_STICKY_FLAME_I:
            case ENCH_YOUR_STICKY_FLAME_II:
            case ENCH_YOUR_STICKY_FLAME_III:
            case ENCH_YOUR_STICKY_FLAME_IV:

                monster.hit_points -= ((random2(4) + random2(4) + 1) * 10) / monster.speed;
                if (mons_res_fire(monster.type) == -1)
                    monster.hit_points -= ((random2(5) + random2(5) + 1) * 10) / monster.speed;

                if (mons_near(i))
                {
                    strcpy(info, monam(monster.number, monster.type, monster.enchantment[2], 0));
                    strcat(info, " burns!");
                    mpr(info);
                }

                if (monster.hit_points <= 0)
                {
                    if (monster.enchantment[p] < ENCH_STICKY_FLAME_I)
                        monster_die(i, KILL_YOU, 0);   // ie you

                    else
                        monster_die(i, KILL_MISC, 0);
                    died = true;
                }

                if (random2(2) == 0)
                {
                    monster.enchantment[p]--;   //= 0;

                    if (monster.enchantment[p] == ENCH_STICKY_FLAME_I-1 ||
                        monster.enchantment[p] == ENCH_YOUR_STICKY_FLAME_I-1)
                    {
                         flag_ench(monster,p);
                    }
                }
                break;

                /* 19 is taken by summoning:
                   If these are changed, must also change abjuration */
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
                if (random2(10) == 0)
                    monster.enchantment[p]--;

                if (monster.enchantment[p] == ENCH_ABJ_I-1 || monster.enchantment[p] == ENCH_FRIEND_ABJ_I-1)
                {
                    monster_die(i, KILL_RESET, 0);
                    died = true;
                }
                break;

            case ENCH_CHARM:            // charm monster

                if (monster.behavior != BEH_ENSLAVED || random2(500) <= monster.hit_dice + 10)
                {
                    monster.behavior = BEH_CHASING_I;   // reset to monster's original behaviour

                    flag_ench(monster,p);
                    simple_monster_message(i, monster, " is no longer your friend.");
                }
                break;

            case ENCH_GLOWING_SHAPESHIFTER:            // monster is a glowing shapeshifter. This ench never runs out.

                if (random2(4) == 0 || monster.type == MONS_GLOWING_SHAPESHIFTER)
                    monster_polymorph(i, MONS_PROGRAM_BUG, 0);
                break;

            case ENCH_SHAPESHIFTER:            // monster is a shapeshifter. This ench never runs out.

                if (random2(15) == 0 || monster.type == MONS_SHAPESHIFTER)
                    monster_polymorph(i, MONS_PROGRAM_BUG, 0);
                break;

            case ENCH_TP_I:
                monster_teleport(i, 1);
                flag_ench(monster,p);
                break;

            case ENCH_TP_II:
            case ENCH_TP_III:
            case ENCH_TP_IV:
                monster.enchantment[p]--;
                break;
            }
        }
    }

    return died;
}


//---------------------------------------------------------------
//
// handle_movement
//
// Move the monster close to its target square.
//
//---------------------------------------------------------------
static void handle_movement(int i)
{
    ASSERT(i >= 0);
    ASSERT(i < MNST);

    monsters & monster = env.mons[i];

    // move the monster
    if (monster.behavior == BEH_FLEE)
    {
        if (monster.target_x < monster.x)
            mmov_x = 1;
        else
            mmov_x = 0;

        if (monster.target_x >
            monster.x)
            mmov_x = -1;

        if (monster.target_y < monster.y)
            mmov_y = 1;
        else
            mmov_y = 0;

        if (monster.target_y > monster.y)
            mmov_y = -1;

    }
    else
    {
        if (monster.target_x < monster.x)
            mmov_x = -1;
        else
            mmov_x = 0;

        if (monster.target_x > monster.x)
            mmov_x = 1;

        if (monster.target_y < monster.y)
            mmov_y = -1;
        else
            mmov_y = 0;

        if (monster.target_y > monster.y)
            mmov_y = 1;
    }

    // see if we attacked another monster
    if (mgrd[monster.x + mmov_x][monster.y + mmov_y] != MNG && mgrd[monster.x + mmov_x][monster.y + mmov_y] != i)
    {
        int mnt = mgrd[monster.x + mmov_x][monster.y + mmov_y];

        if (monster.behavior == BEH_ENSLAVED && menv[mnt].behavior != BEH_ENSLAVED)
            monster.monster_foe = mnt;

        if (monster.behavior != BEH_ENSLAVED && menv[mnt].behavior == BEH_ENSLAVED)
            monster.monster_foe = mnt;
    }
}


//---------------------------------------------------------------
//
// handle_nearby_ability
//
// Gives monsters a chance to use a special ability when they're
// next to the player.
//
//---------------------------------------------------------------
static void handle_nearby_ability(int i)
{
    ASSERT(i >= 0);
    ASSERT(i < MNST);

    monsters & monster = env.mons[i];

    if (mons_near(i) && monster.behavior != BEH_SLEEP)
    {
        if (mons_flag(monster.type, M_SPEAKS) && random2(5) == 0)
            mons_speaks(i);

        switch (monster.type)
        {
        case MONS_SPATIAL_VORTEX:
        case MONS_KILLER_KLOWN:
            monster.number = random2(15) + 1;
            break;

        case MONS_GIANT_EYEBALL:
            if (random2(2) == 0 && (monster.enchantment[2] != 6 || player_see_invis() != 0))
            {
                mpr("The giant eyeball stares at you.");

                if (you.paralysis < 10)
                    you.paralysis += 2 + random2(3);
            }
            break;

        case MONS_EYE_OF_DRAINING:
            if (random2(2) == 0 && (monster.enchantment[2] != ENCH_INVIS || player_see_invis() != 0))
            {
                mpr("The eye of draining stares at you.");
                you.magic_points -= random2(5) + random2(5) + random2(5) + 2;

                if (you.magic_points < 0)
                    you.magic_points = 1;

                you.redraw_magic_points = 1;
                monster.hit_points += 5;

                if (monster.hit_points >= monster.max_hit_points)
                    monster.hit_points = monster.max_hit_points;
            }
            break;

        case MLAVA0:
        case MLAVA1:
        case MLAVA2:
        case MLAVA3:
        case MLAVA4:
        case MWATER0:
        case MWATER1:
        case MWATER2:
        case MWATER3:
        case MWATER4:
        case MWATER5:
            if (grd[monster.x][monster.y] == 65 || grd[monster.x][monster.y] == 200)
            {
                monster.number = 0;
                monster.enchantment[2] = 0;

            }
            else if (monster.number == 0 && grd[monster.x][monster.y] != 65 && grd[monster.x][monster.y] != 200)
            {                   // visible

                if (random2(5) == 0)
                {
                    monster.number = 1;
                    monster.enchantment[2] = ENCH_INVIS;
                    monster.enchantment1 = 1;
                }

            }
            else if (random2(10) == 0 && (monster.x != you.x_pos || monster.y != you.y_pos))
            {
                monster.number = 0;
            }
            break;

        case MONS_AIR_ELEMENTAL:
            if (random2(5) == 0)
            {
                monster.enchantment[2] = ENCH_INVIS;
                monster.enchantment1 = 1;
            }
            break;

        case MONS_PANDEMONIUM_DEMON:    // random demon

            if (ghost.values[13] == 1)
                monster.number = random2(15) + 1;
            break;
        }
    }
}


//---------------------------------------------------------------
//
// handle_special_ability
//
// $$$ not sure what to say here...
//
//---------------------------------------------------------------
static bool handle_special_ability(int i, bolt & beem)
{
    ASSERT(i >= 0);
    ASSERT(i < MNST);

    bool used = false;

    monsters & monster = env.mons[i];

    unsigned int show[19][19];

    losight(show, grd, you.x_pos, you.y_pos);

    switch (monster.type)
    {
    case MLAVA2:
        if (monster.behavior == BEH_CONFUSED)
            break;

        if (you.invis != 0 && mons_see_invis(monster.type) == 0)
            break;

        if (random2(2) == 0 || monster.number == 1 || monster.enchantment[2] == 6)
            break;

        // viewwindow was here.
        if (show[monster.x - you.x_pos + 6][monster.y - you.y_pos + 6] != 0)
        {
            beem.move_x = 0;
            beem.move_y = 0;

            beem.move_x = beem.trac_targ_x - monster.x;
            beem.move_y = beem.trac_targ_y - monster.y;

            if (beem.move_x != 0 || beem.move_y != 0)
            {
                viewwindow(1, false);
                beem.target_x = beem.trac_targ_x;
                beem.target_y = beem.trac_targ_y;
                beem.aim_down = 1;
                beem.range = random2(10) + 5;
                beem.source_x = monster.x;
                beem.source_y = monster.y;
                strcpy(beem.beam_name, "glob of lava");
                beem.damage = 108;
                beem.colour = RED;
                beem.type = '#';        // hash #

                beem.flavour = 20;      // lava

                beem.hit = 20;
                strcpy(info, monam(monster.number, monster.type, monster.enchantment[2], 0));
                strcat(info, " spits lava!");
                beem.beam_source = i;
                mpr(info);
                beem.thing_thrown = 4;
                missile(&beem, 0);
            }
        }
        break;

    case MWATER2:
        if (monster.behavior == BEH_CONFUSED)
            break;

        if (you.invis != 0 && mons_see_invis(monster.type) == 0)
            break;

        if (monster.number == 1 || monster.enchantment[2] == 6)
            break;

        if (show[monster.x - you.x_pos + 6][monster.y - you.y_pos + 6] != 0)
        {
            beem.move_x = 0;
            beem.move_y = 0;
            beem.move_x = beem.trac_targ_x - monster.x;
            beem.move_y = beem.trac_targ_y - monster.y;

            if (beem.move_x != 0 || beem.move_y != 0)
            {
                beem.target_x = beem.trac_targ_x;
                beem.target_y = beem.trac_targ_y;
                beem.aim_down = 1;
                viewwindow(1, false);
                beem.range = random2(10) + 5;
                beem.source_x = monster.x;
                beem.source_y = monster.y;
                strcpy(beem.beam_name, "bolt of electricity");
                beem.damage = 105;
                beem.colour = LIGHTCYAN;
                beem.type = 35; // hash #

                beem.flavour = 5;       // elec

                beem.hit = 150;
                beem.beam_source = i;
                strcpy(info, monam(monster.number, monster.type, monster.enchantment[2], 0));   //(gmon_name [mons_class [i]]);

                strcat(info, " shoots out a bolt of electricity!");
                mpr(info);
                beem.thing_thrown = 2;
                beam(&beem);
            }
        }
        break;

    case MONS_OKLOB_PLANT:
        if (monster.behavior == BEH_CONFUSED)
            break;

        if (you.invis != 0 && mons_see_invis(monster.type) == 0)
            break;

        if (random2(3) == 0)    // o plant

            plant_spit(i, &beem);
        break;

    case MONS_PIT_FIEND:
        if (random2(3) == 0)
            break;
        // fall thru

    case MONS_FIEND:
        if (monster.behavior == BEH_CONFUSED)
            break;

        if (random2(4) == 0)
        {
            int spell_cast;

            switch (random2(4))
            {
            case 0:
                spell_cast = 48;
                mons_cast(i, &beem, spell_cast);        /* beem should be irrelevant here */
                break;

            case 1:
            case 2:
            case 3:
                tracer_f(i, &beem);
                if (monster.behavior == BEH_ENSLAVED && (beem.tracer == 1 || beem.tracer == 2))
                    break;

                if (beem.tracer != 0)
                {               /* || (beem.trac_targ != MHITYOU && beem.trac_hit_mons != 0) */
                    if (monster.enchantment[2] != 6)
                    {
                        strcpy(info, monam(monster.number, monster.type, monster.enchantment[2], 0));
                        strcat(info, " makes a gesture!");
                        mpr(info);
                    }

                    spell_cast = 49;
                    mons_cast(i, &beem, spell_cast);
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
        if (random2(7) == 0)
        {
            if (mons_near(i))
            {
                strcpy(info, monam(monster.number, monster.type, monster.enchantment[2], 0));
                strcat(info, " blinks.");
                mpr(info);
            }

            monster_blink(i);
        }
        break;

    case MONS_MANTICORE:
        if (you.invis != 0 && mons_see_invis(monster.type) == 0)
            break;

        if (monster.behavior == BEH_CONFUSED)
            break;

        if (monster.inv[1] != 501)
        {
            int hand_used = monster.inv[1];

            beem.range = 6;
            item_name(mitm.pluses2[hand_used], mitm.base_type[hand_used], mitm.sub_type[hand_used], mitm.special[hand_used], mitm.pluses[hand_used], 1, mitm.id[hand_used], 6, str_pass);
            strcpy(beem.beam_name, str_pass);
            mons_throw(i, &beem, hand_used);
            used = true;        // ????

        }
        break;

        // dragon breath weapon:
    case MONS_DRAGON:
    case MONS_HELL_HOUND:
    case MONS_ICE_DRAGON:
    case MONS_LINDWORM:
    case MONS_FIREDRAKE:
    case MONS_XTAHUA:
        if (monster.behavior == BEH_CONFUSED)
            break;

        if (you.invis != 0 && mons_see_invis(monster.type) == 0)
            break;

        if ((monster.type != MONS_HELL_HOUND && random2(13) < 3) || random2(10) == 0)
        {
            if (monster.behavior == BEH_FIGHT || monster.behavior == BEH_ENSLAVED)
            {
                beem.trac_targ = monster.monster_foe;
                tracer_f(i, &beem);

                if (beem.tracer == 0)
                    break;

                if (monster.behavior == 7 && (beem.tracer == 1 || beem.tracer == 2))
                    break;

                if (monster.behavior == 7 && beem.tracer == 4)
                    break;

                if (beem.tracer == 3 && beem.tracer_mons == 0 && monster.behavior != BEH_ENSLAVED)
                    break;
            }

            dragon(i, &beem);
            mmov_x = 0;
            used = true;        // $$$ why don't more of these cases set this?

        }
        break;
    }

    return used;
}


//---------------------------------------------------------------
//
// handle_potion
//
// Give the monster a chance to quaff a potion. Returns true if
// the monster imbibed.
//
//---------------------------------------------------------------
static bool handle_potion(int i, bolt & beem)
{
    ASSERT(i >= 0);
    ASSERT(i < MNST);

    bool imbibed = false;

    monsters & monster = env.mons[i];

    if (monster.inv[4] != 501 && random2(3) == 0 && monster.behavior != BEH_SLEEP)
    {
        switch (mitm.sub_type[monster.inv[4]])
        {
        case 0:
        case 1:
            if (monster.hit_points <= monster.max_hit_points / 2)
            {
                if (mons_near(i))
                {
                    strcpy(info, monam(monster.number, monster.type, monster.enchantment[2], 0));
                    strcat(info, " drinks a potion.");
                    mpr(info);
                    strcpy(info, monam(monster.number, monster.type, monster.enchantment[2], 0));
                    strcat(info, " is healed!");
                    mpr(info);
                }

                if (mitm.sub_type[monster.inv[4]] == 1)
                    monster.hit_points += random2(10) + random2(10) + random2(10) + 10;

                monster.hit_points += random2(7) + 5;
                if (monster.hit_points > monster.max_hit_points)
                    monster.hit_points = monster.max_hit_points;

                imbibed = true;
            }
            break;

        case 2:
            beem.colour = 1;
            // fall thru?

        case 12:
            if (mitm.sub_type[monster.inv[4]] == 12)
                beem.colour = 5;

            if (!mons_near(i))
            {
                strcpy(info, monam(monster.number, monster.type, monster.enchantment[2], 0));
                strcat(info, " drinks a potion.");
                mpr(info);

                int func_pass[10];

                mons_ench_f2(i, 1, func_pass, &beem);

                imbibed = true;
            }
            break;
        }

        if (imbibed)
        {
            mitm.quantity[monster.inv[4]]--;
            if (mitm.quantity[monster.inv[4]] <= 0)
                destroy_item(monster.inv[4]);
        }
    }

    return imbibed;
}


static bool handle_reaching(int i)
{
    bool ret = false;

    monsters & monster = env.mons[i];

    if (monster.inv[0] != 501 && mitm.base_type[monster.inv[0]] == OBJ_WEAPONS
                && mitm.special[monster.inv[0]] < 180
                && mitm.special[monster.inv[0]] % 30 == SPWPN_REACHING)
    {
        int dx = abs( monster.x - you.x_pos );
        int dy = abs( monster.y - you.y_pos );

        if ((dx == 2 && dy <= 2) || (dy == 2 && dx <= 2))
        {
            ret = true;
            monster_attack(i);
        }
    }

    return ret;
}

//---------------------------------------------------------------
//
// handle_scroll
//
// Give the monster a chance to read a scrol. Returns true if
// the monster read something.
//
//---------------------------------------------------------------
static bool handle_scroll(int i)
{
    ASSERT(i >= 0);
    ASSERT(i < MNST);

    bool read = false;

    monsters & monster = env.mons[i];

    if (monster.inv[6] != 501 && random2(3) == 0 && monster.behavior != BEH_CONFUSED && monster.behavior != BEH_SLEEP)
    {
        switch (mitm.sub_type[monster.inv[6]])
        {
        case 1:
            if (monster.enchantment[0] != 40 && monster.enchantment[1] != 40 && monster.enchantment[2] != 40)
            {
                if (monster.behavior == BEH_FLEE)
                {
                    if (mons_near(i))
                    {
                        strcpy(info, monam(monster.number, monster.type, monster.enchantment[2], 0));
                        strcat(info, " reads a scroll.");
                        mpr(info);
                    }

                    monster_teleport(i, 0);
                    read = true;
                }
            }
            break;

        case 14:
            if (monster.behavior == BEH_FLEE)
            {
                if (mons_near(i))
                {
                    strcpy(info, monam(monster.number, monster.type, monster.enchantment[2], 0));
                    strcat(info, " reads a scroll.");
                    mpr(info);

                    strcpy(info, monam(monster.number, monster.type, monster.enchantment[2], 0));
                    strcat(info, " blinks!");
                    mpr(info);

                    monster_blink(i);
                    read = true;
                }
            }
            break;

        case 6:
            if (mons_near(i))
            {
                strcpy(info, monam(monster.number, monster.type, monster.enchantment[2], 0));
                strcat(info, " reads a scroll.");
                mpr(info);

                create_monster(MONS_LARGE_ABOMINATION, 21, monster.behavior, monster.x, monster.y, monster.monster_foe, 250);
                read = true;
            }
            break;
        }

        if (read)
        {
            mitm.quantity[monster.inv[6]]--;
            if (mitm.quantity[monster.inv[6]] <= 0)
                destroy_item(monster.inv[6]);
        }
    }

    return read;
}


//---------------------------------------------------------------
//
// handle_wand
//
// Give the monster a chance to zap a wand. Returns true if the
// monster zapped.
//
//---------------------------------------------------------------
static bool handle_wand(int i, bolt & beem)
{
    ASSERT(i >= 0);
    ASSERT(i < MNST);

    bool zap = false;

    monsters & monster = env.mons[i];

    if (monster.inv[5] != 501 && random2(2) == 0 && monster.behavior != BEH_SLEEP)
    {
        if (mitm.pluses[monster.inv[5]] > 0)
        {
            switch (mitm.sub_type[monster.inv[5]])
            {
            case WAND_HASTING:
                if (monster.enchantment[0] != 2 && monster.enchantment[1] != 2 && monster.enchantment[2] == 2)
                {
                    beem.target_x = monster.x;
                    beem.target_y = monster.y;

                    zap = true;
                }
                break;

            case WAND_HEALING:
                if (monster.hit_points <= monster.max_hit_points / 2)
                {
                    beem.target_x = monster.x;
                    beem.target_y = monster.y;

                    zap = true;
                }
                break;

            case WAND_INVISIBILITY:
                if (monster.enchantment[2] != 6)
                {
                    beem.target_x = monster.x;
                    beem.target_y = monster.y;

                    zap = true;
                }
                break;

            case WAND_FIREBALL: // too risky

            case WAND_DIGGING:  // I must do this sometime

                break;

            case WAND_TELEPORTATION:
                if (monster.hit_points <= monster.max_hit_points / 2)
                {
                    if (monster.enchantment[0] != 40 && monster.enchantment[1] != 40 && monster.enchantment[2] != 40)
                    {
                        beem.target_x = monster.x;
                        beem.target_y = monster.y;

                        zap = true;
                    }
                }
                break;

            default:
                zap = true;

                if ((monster.behavior == BEH_CHASING_II || monster.behavior == BEH_SLEEP) && !mons_near(i))
                {
                    zap = false;

                }
                else if ((monster.behavior == BEH_FIGHT || monster.behavior == BEH_ENSLAVED) && monster.monster_foe == MHITNOT)
                {
                    zap = false;

                }
                else
                {
                    tracer_f(i, &beem);
                    if (beem.tracer == 0)
                        zap = false;

                    if (monster.behavior == BEH_ENSLAVED && (beem.tracer == 1 || beem.tracer == 2))
                        zap = false;

                    if (monster.behavior == BEH_ENSLAVED && beem.tracer_mons == 4)
                        zap = false;

                    if (beem.tracer == 3 && beem.tracer_mons == 0 && monster.behavior != BEH_ENSLAVED)
                        zap = false;
                }
            }

            if (zap)
            {
                int mzap = 0;

                switch (mitm.sub_type[monster.inv[5]])
                {
                case WAND_FLAME:
                    mzap = 1;
                    break;
                case WAND_FROST:
                    mzap = 2;
                    break;
                case WAND_SLOWING:
                    mzap = 4;
                    break;
                case WAND_HASTING:
                    mzap = 5;
                    break;
                case WAND_MAGIC_DARTS:
                    mzap = 0;
                    break;
                case WAND_HEALING:
                    mzap = 13;
                    break;
                case WAND_PARALYSIS:
                    mzap = 3;
                    break;
                case WAND_FIRE:
                    mzap = 8;
                    break;
                case WAND_COLD:
                    mzap = 9;
                    break;
                case WAND_CONFUSION:
                    mzap = 6;
                    break;
                case WAND_INVISIBILITY:
                    mzap = 11;
                    break;
                case WAND_DIGGING:
                    zap = false;
                case WAND_FIREBALL:
                    zap = false;
//                              case 11:                                        mzap = 11; break; // digging
                    //                              case 12:                                        mzap = 11; break; // fireball
                case WAND_TELEPORTATION:
                    mzap = 15;
                    break;
                case WAND_LIGHTNING:
                    mzap = 10;
                    break;
                case WAND_POLYMORPH_OTHER:
                    zap = false;
                case WAND_ENSLAVEMENT:
                    zap = false;
//                              case 15:                                        mzap = 10; break; // lightning
                    //                              case 16:                                        mzap = 10; break; // lightning
                case WAND_DRAINING:
                    mzap = 19;
                    break;
                case WAND_RANDOM_EFFECTS:
                    zap = false;
//                              case 18: random effects
                }

                if (zap)
                {
                    viewwindow(1, false);

                    beem.move_x = beem.trac_targ_x - monster.x;
                    beem.move_y = beem.trac_targ_y - monster.y;

                    beem.target_x = beem.trac_targ_x;
                    beem.target_y = beem.trac_targ_y;

                    beem.source_x = monster.x;
                    beem.source_y = monster.y;

                    int power = 25;

                    SBeam theBeam = mons_spells(mzap, power);

                    beem.colour = theBeam.colour;
                    beem.range = theBeam.range;
                    beem.damage = theBeam.damage;
                    beem.hit = theBeam.hit;
                    beem.type = theBeam.type;
                    beem.flavour = theBeam.flavour;
                    beem.thing_thrown = theBeam.thrown;

                    strcpy(beem.beam_name, theBeam.name.c_str());

                    beem.ench_power = 20;       // I think

                    beem.beam_source = i;
                    beem.ench_power = beem.damage;      // !!!

                    if (mzap == 5 || mzap == 13 || mzap == 11 || mzap == 15)
                    {
                        beem.move_x = 0;
                        beem.move_y = 0;

                    }
                    else if (beem.move_x == 0 && beem.move_y == 0)
                        zap = false;

                    if (zap)
                    {
                        if (mons_near(i) != 0)
                        {
                            strcpy(info, monam(monster.number, monster.type, monster.enchantment[2], 0));
                            strcat(info, " zaps a wand.");
                        }
                        else
                            strcpy(info, "You hear a zap.");
                        mpr(info);

                        mitm.pluses[monster.inv[5]]--;

                        if (theBeam.isBeam)
                            beam(&beem);
                        else
                            missile(&beem, 0);
                    }
                    /* remember: the monster may well be dead here (eg if it zapped a /o lightning) */
                }
            }
        }

// tracer = 0 = run out of range
        // tracer = 1 = hits you in range
        // tracer_mons = 0 = hits no monsters
        //       "     = 1 = hits monster before you (possibly also after)
        //       "     = 2 = hits monster after but not before
        //       "     = 3 = hits tamed monster(s) but no others

// remember to use the charge!
    }

    return zap;
}


//---------------------------------------------------------------
//
// handle_spell
//
// Give the monster a chance to cast a spell. Returns true if
// a spell was cast.
//
//---------------------------------------------------------------
static bool handle_spell(int i, bolt &beem)
{
    ASSERT(i >= 0);
    ASSERT(i < MNST);

    bool cast = false;

    monsters &monster = env.mons[i];

    if (mons_flag(monster.type, M_SPELLCASTER) && monster.behavior != BEH_SLEEP)
    {
        int spell_cast = 100;

        if (monster.type == MONS_HELLION)
        {
            switch (random2(3))
            {
            case 0:
                monster.number = RED;
                break;
            case 1:
                monster.number = LIGHTRED;
                break;
            case 2:
                monster.number = YELLOW;
                break;
            }
        }

        if (monster.behavior == BEH_CONFUSED && monster.type != MONS_VAPOUR)
            goto bail;          // vapours cast spells even if confused

        if (you.invis != 0 && mons_see_invis(monster.type) == 0)
            goto bail;

        if (random2(200) > 50 + menv[i].hit_dice)
            goto bail;

        if ((monster.behavior == BEH_FIGHT || monster.behavior == BEH_ENSLAVED) && monster.monster_foe == MHITNOT)
            goto bail;

        int msecc = monster.number;

        if (monster.type == MONS_HELLION)
            msecc = 30;         // burning devil

        if (monster.type == MONS_PANDEMONIUM_DEMON)
        {
            msecc = 119;
            if (ghost.values[9] == 0)
                goto bail;      // random demon

        }

        // tracer = 0 = run out of range
        // tracer = 1 = hits you in range
        // tracer_mons = 0 = hits no monsters
        //       "     = 1 = hits monster before you (possibly also after)
        //       "     = 2 = hits monster after but not before
        //       "     = 3 = hits tamed monster(s) but no others

        int func_pass_2[6];

        func_pass_2[0] = 100;
        func_pass_2[1] = 100;
        func_pass_2[2] = 100;
        func_pass_2[3] = 100;
        func_pass_2[4] = 100;
        func_pass_2[5] = 100;

        mons_spell_list(msecc, func_pass_2);

        if (func_pass_2[4] == 18 && !mons_near(i) && monster.behavior == BEH_CHASING_I)
        {
            spell_cast = 18;    /* Casts dig even when player not visible.
                                                 * This is EVIL! */

            goto casted;
        }

        if (beem.trac_targ == MHITYOU && !mons_near(i))
            goto bail;

        /*
           Used by monsters in "planning" which spell to cast. Fires off a "tracer"
           which tells the monster what it'll hit if it breathes/casts etc.

           tracer = 0 = run out of range or hits a wall
           tracer = 1 = hits you in range
           tracer = 2 = hits friendly monster
           tracer = 3 = hits hostile monster

           tracer_mons = 1 = hits monster specified in trac_targ

           tracer_hit_mons now holds value of mons_see_invis of attacking
           monster. This is used in missile().
           If it's 0, won't register invis monsters or you

           Note: only the missile() function is used for tracers. The tracer code
           in beam() is obsolete and useless.
         */

        tracer_f(i, &beem);

        if (beem.tracer == 0)
            goto bail;

        if (beem.tracer == 3 && beem.tracer_mons == 0 && monster.behavior != BEH_ENSLAVED)
        {
         spell_cast = func_pass_2 [2];
         goto casted;
        } /* Is a monster in between, so cast a direct spell (smiting etc)
                   * or a self-enchantment */

        if (monster.behavior == BEH_ENSLAVED && (beem.tracer == 1 || beem.tracer == 2))
                goto bail;
            /* Although it would be nice if friendly monsters also cast
             * smiting etc like this, too tricky right now. */


        if (func_pass_2 [4] == 18)
            func_pass_2 [4] = 100; /* Had your chance to cast dig. */

        if (monster.behavior != BEH_FLEE)
        {
            if ((monster.behavior != BEH_ENSLAVED && (beem.tracer == 1 || beem.tracer == 2 || beem.tracer_mons == 1)) || (monster.behavior == BEH_ENSLAVED && beem.tracer == 3))
            {
                spell_cast = func_pass_2[random2(5)];
                if (spell_cast == 100)
                    spell_cast = func_pass_2[random2(5)];

                if (spell_cast == 100)
                    spell_cast = func_pass_2[random2(5)];

                goto casted;
            }

            if (monster.behavior == BEH_ENSLAVED && beem.tracer_mons == 0)
            {
                spell_cast = func_pass_2[2];
                goto casted;
            }

            if (random2(2) == 0)
                goto bail;

            spell_cast = func_pass_2[2];
            goto casted;

        }
        else
        {
            if (random2(5) == 0)
                goto bail;

            spell_cast = func_pass_2[5];
        }

casted:
        if (spell_cast == 100)
            goto bail;

        if (spell_cast == MS_ANIMATE_DEAD
                && (!mons_near(i)
                || !animate_dead(100, menv[i].behavior, menv[i].monster_foe, 0)))
                goto bail; // can't see anything to animate

        if (mons_near(i))
        {
            strcpy(info, monam(monster.number, monster.type, monster.enchantment[2], 0));
            if (monster.type != MONS_BRAIN_WORM)
            {
                if (monster.type == MONS_STEAM_DRAGON || monster.type == MONS_MOTTLED_DRAGON || monster.type == MONS_STORM_DRAGON || monster.type == MONS_GOLDEN_DRAGON || monster.type == MONS_SHADOW_DRAGON || monster.type == MONS_SWAMP_DRAGON || monster.type == MONS_SWAMP_DRAKE || monster.type == MONS_HELL_HOG || monster.type == MONS_SERPENT_OF_HELL || monster.type == MONS_QUICKSILVER_DRAGON || monster.type == MONS_IRON_DRAGON)
                {
                    if (monster.enchantment[2] != 6)
                    {
                        strcat(info, " breathes.");
                        mpr(info);
                    } else mpr("You hear a roar.");

                }
                else if (monster.type == MONS_GREAT_ORB_OF_EYES || monster.type == MONS_SHINING_EYE || monster.type == MONS_EYE_OF_DEVASTATION)
                {
                    if (monster.enchantment[2] != 6)
                    {
                        strcat(info, " gazes.");
                        mpr(info);
                    }

                }
                else if (monster.type == MONS_VAPOUR)
                {
                    monster.enchantment[2] = 6;

                }
                else if (monster.type == MONS_GIANT_ORANGE_BRAIN)
                {
                    if (monster.enchantment[2] != 6)
                    {
                        strcat(info, " pulsates.");
                        mpr(info);
                    }

                }
                else if (monster.type == MONS_GERYON)
                {
                    strcat(info, " winds a great silver horn.");
                    mpr(info);

                }
                else if (monster.type == MONS_NAGA || monster.type == MONS_NAGA_WARRIOR)
                {
                    strcat(info, " spits poison.");
                    mpr(info);

                }
                else if ((monster.type >= MONS_HELLION && monster.type <= 90) || monster.type == MONS_EFREET || (monster.type >= MONS_WHITE_IMP && monster.type <= MONS_CACODEMON))
                {
                    strcat(info, " gestures.");
                    mpr(info);

                }
                else if (monster.type == MONS_DORGI || monster.type == MONS_GUARDIAN_ROBOT)
                {
                    strcat(info, " fires!");
                    mpr(info);

                }
                else if (monster.type == MONS_SWORD)
                {
                    strcat(info, " burns!");
                    mpr(info);

                }
                else if (strstr(monam(monster.number, monster.type, 0, 0), "priest") != NULL)
                {
                    strcat(info, " utters an invocation.");
                    mpr(info);

                }
                else
                {
                    strcat(info, " casts a spell.");
                    mpr(info);
                }
            }

        }
        else
        {                       // handle far-away monsters

            if (monster.type == MONS_GERYON)
            {
                strcpy(info, "You hear a weird and mournful sound.");
                mpr(info);
            }
        }

        if (spell_cast == 16 && mons_near(i))
        {
            strcpy(info, monam(monster.number, monster.type, monster.enchantment[2], 0));
            strcat(info, " blinks!");
            mpr(info);
            monster_blink(i);

        }
        else
        {
            beem.damage = 8 * monster.hit_dice;         // really???

            beem.beam_source = i;
            mons_cast(i, &beem, spell_cast);

            mmov_x = 0;
            beem.trac_hit_tamed = 0;
        }

        cast = true;
    }

bail:
    return cast;
}


//---------------------------------------------------------------
//
// handle_throw
//
// Give the monster a chance to throw something. Returns true if
// the monster hurled.
//
//---------------------------------------------------------------
static bool handle_throw(int i, bolt & beem)
{
    ASSERT(i >= 0);
    ASSERT(i < MNST);

    bool thrown = false;

    monsters & monster = env.mons[i];

    if (gmon_use[monster.type] > 0 && monster.inv[1] != 501 && monster.type != MONS_TWO_HEADED_OGRE && (you.invis == 0 || mons_see_invis(monster.type) != 0) && monster.behavior != BEH_CONFUSED && monster.behavior != BEH_SLEEP)
    {
        if ((monster.behavior == BEH_FIGHT || monster.behavior == BEH_ENSLAVED) && monster.monster_foe == MHITNOT)
            goto bail;

        int hand_used = monster.inv[1];

        if (random2(10) < 8)
        {
            if (mitm.quantity[hand_used] > 0)
            {
                if (beem.trac_targ == MHITYOU && beem.tracer_mons == 1)
                    goto bail;

                if (beem.trac_targ == MHITYOU && !mons_near(i))
                    goto bail;

                tracer_f(i, &beem);
                if (beem.tracer == 0)
                    goto bail;

                if (monster.behavior == BEH_ENSLAVED && (beem.tracer == 1 || beem.tracer == 2))
                    goto bail;

                if (beem.tracer == 3 && beem.tracer_mons == 0 && monster.behavior != BEH_ENSLAVED)
                    goto bail;

                if (monster.behavior == BEH_ENSLAVED && beem.tracer == 1 && beem.tracer_mons != 1)
                    goto bail;

                if (monster.behavior == BEH_ENSLAVED && beem.tracer_mons == 4)
                    goto bail;

                if (beem.tracer != 0 || (beem.trac_targ != MHITYOU && beem.trac_hit_mons != 0))
                {               // doesn't need to worry about you.haste

                    beem.range = 6;
                    item_name(mitm.pluses2[hand_used], mitm.base_type[hand_used], mitm.sub_type[hand_used], mitm.special[hand_used], mitm.pluses[hand_used], 1, mitm.id[hand_used], 6, str_pass);
                    strcpy(beem.beam_name, str_pass);

                    mons_throw(i, &beem, hand_used);
                    thrown = true;
                }
            }
        }
    }

bail:
    return thrown;
}


//---------------------------------------------------------------
//
// monster
//
// This is the routine that controls monster AI.
//
//---------------------------------------------------------------
void monster()
{
    char bat = 0;
    int monc = 0;
    char brkk = 0;
    int j;

    struct bolt beem[1];

    unsigned int show[19][19];

    losight(show, grd, you.x_pos, you.y_pos);

    int no_mons = 0;

    for (int i = 0; i < MNST; i++)
    {
        monsters & monster = env.mons[i];

        if (monster.type != -1)
            no_mons++;

        if (monster.type != -1 && monster.type != MONS_WORM_TAIL)
        {
            monc++;

            if (monster.hit_points > monster.max_hit_points)
                monster.hit_points = monster.max_hit_points;

            if (monster.type == MONS_GIANT_SPORE && monster.hit_points < 1)
            {
                for (j = 0; j < 3; j++)
                {
                    monster.enchantment[j] = 0;
                }

                monster.enchantment1 = 0;
                no_mons--;
                monster.type = -1;
                mgrd[monster.x][monster.y] = MNG;
                spore_goes_pop(i);      // has to be after the above, so that spore isn't killed twice.

                continue;
            }

                monster.speed_increment += (monster.speed * you.time_taken / 10);

            if (you.slow > 0)
                monster.speed_increment += (monster.speed * you.time_taken / 10);

            while (monster.speed_increment >= 80)
            {                   // The continues & breaks are WRT this.

                if (monster.type != -1 && monster.hit_points <= 0)
                {
                    monster_die(i, KILL_MISC, 0);

                    if (monster.type != MONS_GIANT_SPORE)
                        continue;
                }

                monster.speed_increment -= 10;

                if (env.cgrid[monster.x][monster.y] != CNG)
                {
                    if (monster.type >= MLAVA0 && monster.number == 1)
                        break;

                    if (monster.type == -1)
                        break;  // problem with vortices

                    mons_in_cloud(i);

                    if (monster.type == -1)
                    {
                        monster.speed_increment = 1;
                        break;
                    }
                }

                if (you.invis != 0 && mons_see_invis(monster.type) != 1 && monster.target_x == monster.x && monster.target_y == monster.y)
                    monster.behavior = BEH_WANDER;

                if (monster.type == MONS_GLOWING_SHAPESHIFTER)
                    monster.enchantment[1] = 38;

                if (monster.type == MONS_SHAPESHIFTER)
                    monster.enchantment[1] = 39;        // otherwise there are potential problems with summonings

                handle_behavior(i);

                bool died = handle_enchantment(i);

                if (died)
                    goto endbat;

                // regenerate
                if (monster.hit_points < monster.max_hit_points
                        && (random2(25) == 0
                            || monster_regenerates( monster.type )))
                {
                    monster.hit_points++;
                }

                if (monster.speed >= 100)
                    continue;

                if (monster.type == MONS_SMALL_ZOMBIE || monster.type == MONS_BIG_ZOMBIE || monster.type == MONS_SMALL_SKELETON || monster.type == MONS_LARGE_SKELETON)
                    monster.max_hit_points = monster.hit_points;

                if (igrd[monster.x][monster.y] != 501 && (gmon_use[monster.type] == 3 || monster.type == MONS_JELLY || monster.type == MONS_NECROPHAGE || monster.type == MONS_GHOUL))
                {
                    if (mons_pickup(i)) continue;
                }

                handle_movement(i);

                brkk = 0;

                if (monster.behavior == BEH_CONFUSED)
                {
                    mmov_x = random2(3) - 1;
                    mmov_y = random2(3) - 1;

                    if (mgrd[monster.x + mmov_x][monster.y + mmov_y] != MNG && (mmov_x != 0 || mmov_y != 0))
                    {
                        if (monsters_fight(i, mgrd[monster.x + mmov_x][monster.y + mmov_y]) == 1)
                        {
                            mmov_x = 0;
                            mmov_y = 0;
                            brkk = 1;
                            break;
                        }
                    }
                }

                if (brkk == 1)
                    continue;

                handle_nearby_ability(i);

                if (((monster.behavior == BEH_CHASING_I || monster.behavior == BEH_FLEE) && mons_near(i)) || ((monster.behavior == BEH_FIGHT || monster.behavior == BEH_ENSLAVED) && menv[i].monster_foe != MHITNOT))
                {
                    if ((monster.behavior == BEH_CHASING_I || monster.behavior == BEH_FLEE) && mons_near(i) == 1)
                        you.pet_target = i;

                    if ((monster.behavior == BEH_FIGHT || monster.behavior == BEH_ENSLAVED) && (monster.monster_foe == MHITNOT || monster.monster_foe == MHITYOU))
                        goto end_switch;

                    if (monster.behavior == BEH_FIGHT || monster.behavior == BEH_ENSLAVED)
                    {
                        beem[0].trac_targ = monster.monster_foe;

                        if (monster.monster_foe == MHITNOT)
                        {
                            beem[0].trac_targ_x = monster.x;
                            beem[0].trac_targ_y = monster.y;

                        }
                        else
                        {
                            beem[0].trac_targ_x = menv[monster.monster_foe].x;
                            beem[0].trac_targ_y = menv[monster.monster_foe].y;
                        }

                    }
                    else
                    {
                        beem[0].trac_targ = MHITYOU;
                        beem[0].trac_targ_x = you.x_pos;
                        beem[0].trac_targ_y = you.y_pos;
                    }

                    if (beem[0].trac_targ == MHITYOU && !mons_near(i) && monster.type != MONS_CACODEMON)
                        goto end_switch;

                    if (handle_special_ability(i, beem[0]))
                        continue;

                    if (handle_potion(i, beem[0]))
                        continue;


                    if (handle_scroll(i))
                        continue;


                    if (handle_wand(i, beem[0]))
                    {
                        continue;
                    }
                    else if (monster.behavior == BEH_ENSLAVED
                                            && beem[0].tracer_mons == 4)
                    {
                        goto end_throw;     // $$$ is this right?
                    }

                    if (handle_spell(i, beem[0]))
                        continue;

                    if (handle_reaching(i))
                        continue;

end_switch:
                    if (handle_throw(i, beem[0]))
                        continue;
                }

              end_throw:
                if ((monster.behavior == BEH_ENSLAVED || monster.behavior == BEH_FIGHT) && (mmov_x != 0 || mmov_y != 0) && mgrd[monster.x + mmov_x][monster.y + mmov_y] != MNG)
                {
                    if (monsters_fight(i, mgrd[monster.x + mmov_x][monster.y + mmov_y]) == 1)
                    {
                        if (monster.type == MONS_GIANT_BAT || monster.type == MONS_UNSEEN_HORROR || monster.type == MONS_GIANT_BLOWFLY)
                            monster.speed_increment -= monster.speed;

                        mmov_x = 0;
                        mmov_y = 0;
                        brkk = 1;
                        break;
                    }
                }

                if (brkk == 1)
                    continue;

                if (monster.x + mmov_x == you.x_pos && monster.y + mmov_y == you.y_pos)
                {
                    mmov_x = monster.inv[0];

                    if ((monster.type == MONS_GIANT_BAT || monster.type == MONS_UNSEEN_HORROR || monster.type == MONS_GIANT_BLOWFLY) && bat == 0)
                        monster_attack(i);

                    if ((monster.type == MONS_GIANT_BAT || monster.type == MONS_UNSEEN_HORROR || monster.type == MONS_GIANT_BLOWFLY) && monster.behavior != BEH_ENSLAVED)
                    {
                        monster.behavior = BEH_WANDER;
                        bat = 1;
                    }

                    if (monster.type != MONS_GIANT_BAT && monster.type != MONS_UNSEEN_HORROR && monster.type != MONS_GIANT_BLOWFLY)
                        monster_attack(i);

                    if (monster.type == MONS_GIANT_SPORE && monster.hit_points < 1)
                    {
                        for (j = 0; j < 3; j++)
                            monster.enchantment[j] = 0;

                        monster.enchantment1 = 0;
                        no_mons--;
                        monster.type = -1;
                        mgrd[monster.x][monster.y] = MNG;
                        spore_goes_pop(i);      /* has to be after the above, so that spore isn't killed twice. */
                        continue;
                    }

                    mmov_x = 0;
                    mmov_y = 0;
                    continue;   //break;

                }

                if (monster.type == -1 || monster.type == MONS_OKLOB_PLANT || menv[i].type == MONS_CURSE_SKULL || (menv[i].type >= MONS_CURSE_TOE && menv[i].type <= MONS_POTION_MIMIC))
                    continue;

                // o plant
                monster_move(i);

                if (monster.behavior == BEH_CHASING_I)
                {
                    if (mons_near(i) && (you.invis == 0 || mons_see_invis(monster.type) != 0))
                    {
                        monster.target_x = you.x_pos;
                        monster.target_y = you.y_pos;
                    }
                }

                // Tame monsters can't become afraid, because fear overwrites tameness
                if (monster.hit_points <= (monster.max_hit_points / 4 - 1) && mons_intel(monster.type) > 1 && monster.behavior != BEH_ENSLAVED)
                    monster.behavior = BEH_FLEE;
            }                   // end while

          endbat:
            bat = 0;

            if (monster.behavior == BEH_FIGHT && random2(3) != 0)
                monster.behavior = BEH_CHASING_I;       // I think?

        }                       // end of if (mons_class != -1)

    }                           // end of for loop

}                               // end of void monster()


//---------------------------------------------------------------
//
// mons_pickup
//
// Returns 0 if monster doesn't spend any time pickup up
//
//---------------------------------------------------------------
char mons_pickup(int i)
{

    if (menv[i].type == MONS_JELLY || menv[i].type == MONS_BROWN_OOZE || menv[i].type == MONS_ACID_BLOB || menv[i].type == MONS_ROYAL_JELLY)  /* Jelly! */
    {

        int hps_gained;
        unsigned int quant_eated;

        quant_eated = random2(10) + 1;

        if (quant_eated > mitm.quantity[igrd[menv[i].x][menv[i].y]])
            quant_eated = mitm.quantity[igrd[menv[i].x][menv[i].y]];

        if (mitm.base_type[igrd[menv[i].x][menv[i].y]] == OBJ_WEAPONS && mitm.special[igrd[menv[i].x][menv[i].y]] > 180)
            return 0;             /* unique items */

        if (mitm.base_type[igrd[menv[i].x][menv[i].y]] == OBJ_MISSILES && (mitm.sub_type[igrd[menv[i].x][menv[i].y]] == MI_STONE || mitm.sub_type[igrd[menv[i].x][menv[i].y]] == MI_LARGE_ROCK))
        {
            /* shouldn't eat stone things.
               - but what about stone wands & rings? */
            return 0;
        }

        if (mitm.base_type[igrd[menv[i].x][menv[i].y]] < 15)
        {
            hps_gained = quant_eated * mass(mitm.base_type[igrd[menv[i].x][menv[i].y]], mitm.sub_type[igrd[menv[i].x][menv[i].y]]);
            hps_gained /= 20;
        }
        else
        {
            hps_gained = 1;
        }

        if (hps_gained < 1)
            hps_gained = 1;
        if (hps_gained > 50)
            hps_gained = 50;


        mitm.quantity[igrd[menv[i].x][menv[i].y]] -= quant_eated;
        if (mitm.quantity[igrd[menv[i].x][menv[i].y]] <= 0)
            igrd[menv[i].x][menv[i].y] = mitm.link[igrd[menv[i].x][menv[i].y]];


        menv[i].hit_points += hps_gained;
        if (menv[i].hit_points > menv[i].max_hit_points)
            menv[i].max_hit_points = menv[i].hit_points;
        /* adjust for mass of object!!! */

        if (mons_near(i))
        {
            strcpy(info, "You hear a slurping noise.");
            mpr(info);
        }
        else
        {
            strcpy(info, "You hear a distant slurping noise.");
            mpr(info);
        }


        if (menv[i].hit_points >= 50)
        {
            if (jelly_divide(i) == 0)
                menv[i].hit_points = 50;
        }

        return 0;

    }


    switch (mitm.base_type[igrd[menv[i].x][menv[i].y]])
    {
    case OBJ_WEAPONS:
        if (menv[i].inv[0] != 501)
            return 0;
        if (mitm.special[igrd[menv[i].x][menv[i].y]] > 180)
            return 0;
        if (mitm.special[igrd[menv[i].x][menv[i].y]] % 30 >= 25)
            return 0;
        if ((mons_charclass(menv[i].type) == MONS_KOBOLD || mons_charclass(menv[i].type) == MONS_GOBLIN) && property(mitm.base_type[igrd[menv[i].x][menv[i].y]], mitm.sub_type[igrd[menv[i].x][menv[i].y]], 1) <= 0)
            return 0;
        // wimpy monsters (Kob, gob) shouldn't pick up halberds etc
        if (mitm.sub_type[igrd[menv[i].x][menv[i].y]] == WPN_GIANT_CLUB || mitm.sub_type[igrd[menv[i].x][menv[i].y]] == WPN_GIANT_SPIKED_CLUB)
            return 0;
        // Nobody picks up giant clubs
        menv[i].inv[0] = igrd[menv[i].x][menv[i].y];
        igrd[menv[i].x][menv[i].y] = mitm.link[igrd[menv[i].x][menv[i].y]];
        mitm.link[menv[i].inv[0]] = 501;
        if (mitm.special[menv[i].inv[0]] % 30 == SPWPN_PROTECTION)
            menv[i].armor_class += 3;
        strcpy(info, monam(menv[i].number, menv[i].type, menv[i].enchantment[2], 0));
        strcat(info, " picks up ");
        it_name(menv[i].inv[0], 3, str_pass);
        strcat(info, str_pass);
        strcat(info, ".");
        if (mons_near(i))
            mpr(info);
/*      mmov_x = 0; mmov_y = 0; */
        break;

    case OBJ_MISSILES:
        if (menv[i].inv[1] != 501 && mitm.sub_type[menv[i].inv[1]] == mitm.sub_type[igrd[menv[i].x][menv[i].y]] && mitm.pluses[menv[i].inv[1]] == mitm.pluses[igrd[menv[i].x][menv[i].y]] && mitm.special[menv[i].inv[1]] == mitm.special[igrd[menv[i].x][menv[i].y]])
            /* Removed check for item_plus2 - probably irrelevant */
        {
            strcpy(info, monam(menv[i].number, menv[i].type, menv[i].enchantment[2], 0));
            strcat(info, " picks up ");
            it_name(igrd[menv[i].x][menv[i].y], 3, str_pass);
            strcat(info, str_pass);
            strcat(info, ".");
            if (mons_near(i))
                mpr(info);
            mitm.quantity[menv[i].inv[1]] += mitm.quantity[igrd[menv[i].x][menv[i].y]];
            mitm.quantity[igrd[menv[i].x][menv[i].y]] = 0;
            igrd[menv[i].x][menv[i].y] = mitm.link[igrd[menv[i].x][menv[i].y]];
            mitm.link[menv[i].inv[0]] = 501;
            break;
        }
        if (mitm.sub_type[igrd[menv[i].x][menv[i].y]] == MI_LARGE_ROCK)
            return 0;
        // Nobody bothers to pick up rocks if they don't already have some.
        if (menv[i].inv[0] != 501 || mitm.quantity[igrd[menv[i].x][menv[i].y]] == 1)
            return 0;
        if (mondamage(menv[i].type, 0) >= 6)
            return 0; // monsters with powerful melee attacks don't bother
        menv[i].inv[1] = igrd[menv[i].x][menv[i].y];
        igrd[menv[i].x][menv[i].y] = mitm.link[igrd[menv[i].x][menv[i].y]];
        mitm.link[menv[i].inv[0]] = 501;
        strcpy(info, monam(menv[i].number, menv[i].type, menv[i].enchantment[2], 0));
        strcat(info, " picks up ");
        it_name(menv[i].inv[1], 3, str_pass);
        strcat(info, str_pass);
        strcat(info, ".");
        if (mons_near(i))
            mpr(info);
/*      mmov_x = 0; mmov_y = 0; */
        break;

    case OBJ_WANDS:
        if (menv[i].inv[5] != 501)
            return 0;
        menv[i].inv[5] = igrd[menv[i].x][menv[i].y];
        igrd[menv[i].x][menv[i].y] = mitm.link[igrd[menv[i].x][menv[i].y]];
        mitm.link[menv[i].inv[0]] = 501;
        strcpy(info, monam(menv[i].number, menv[i].type, menv[i].enchantment[2], 0));
        strcat(info, " picks up ");
        it_name(menv[i].inv[5], 3, str_pass);
        strcat(info, str_pass);
        strcat(info, ".");
        if (mons_near(i))
            mpr(info);
/*      mmov_x = 0; mmov_y = 0; */
        break;

    case OBJ_SCROLLS:
        if (menv[i].inv[6] != 501)
            return 0;
        menv[i].inv[6] = igrd[menv[i].x][menv[i].y];
        igrd[menv[i].x][menv[i].y] = mitm.link[igrd[menv[i].x][menv[i].y]];
        mitm.link[menv[i].inv[0]] = 501;
        strcpy(info, monam(menv[i].number, menv[i].type, menv[i].enchantment[2], 0));
        strcat(info, " picks up ");
        it_name(menv[i].inv[6], 3, str_pass);
        strcat(info, str_pass);
        strcat(info, ".");
        if (mons_near(i))
            mpr(info);
/*      mmov_x = 0; mmov_y = 0; */
        break;

    case OBJ_POTIONS:
        if (menv[i].inv[4] != 501)
            return 0;
        menv[i].inv[4] = igrd[menv[i].x][menv[i].y];
        igrd[menv[i].x][menv[i].y] = mitm.link[igrd[menv[i].x][menv[i].y]];
        mitm.link[menv[i].inv[0]] = 501;
        strcpy(info, monam(menv[i].number, menv[i].type, menv[i].enchantment[2], 0));
        strcat(info, " picks up ");
        it_name(menv[i].inv[4], 3, str_pass);
        strcat(info, str_pass);
        strcat(info, ".");
        if (mons_near(i))
            mpr(info);
/*      mmov_x = 0; mmov_y = 0; */
        break;



    case OBJ_CORPSES:
        if (menv[i].type != MONS_NECROPHAGE && menv[i].type != MONS_GHOUL)
            return 0;
        menv[i].hit_points += random2(mons_weight(mitm.pluses[igrd[menv[i].x][menv[i].y]])) / 100 + 1;
        if (menv[i].hit_points > 77)
            menv[i].hit_points = 77;
        if (menv[i].hit_points > menv[i].max_hit_points)
            menv[i].max_hit_points = menv[i].hit_points;
        strcpy(info, monam(menv[i].number, menv[i].type, menv[i].enchantment[2], 0));
        strcat(info, " eats ");
        it_name(igrd[menv[i].x][menv[i].y], 1, str_pass);
        strcat(info, str_pass);
        strcat(info, ".");
        if (mons_near(i))
            mpr(info);
        destroy_item(igrd[menv[i].x][menv[i].y]);
        break;

        default: return 0;

    }

/*    if (menv[i].speed_increment > 25)
        menv[i].speed_increment -= menv[i].speed;*/
        return 1;

}




void monster_move(int i)
{
    char which_first = random2(2);
    char good_move[3][3];
    int count_x, count_y, vacated_x, vacated_y;

//mmov_x = 0;
    //mmov_y = 0;


    int okmove = DNGN_SHALLOW_WATER;

    if (random2(3) == 0 || menv[i].type == MONS_FIRE_ELEMENTAL)
        okmove = DNGN_WATER_STUCK;            /* effectively slows down monster movement across water. Fire elementals can't cross */
    if (mons_flies(menv[i].type) > 0 || menv[i].type >= MLAVA0)
        okmove = MINMOVE;

    for (count_x = 0; count_x < 3; count_x++)
    {
        for (count_y = 0; count_y < 3; count_y++)
        {
            good_move[count_x][count_y] = 1;
            if (menv[i].type == MONS_TUNNELING_WORM || menv[i].type == MONS_BORING_BEETLE)
            {
                if (grd[menv[i].x + count_x - 1][menv[i].y + count_y - 1] == DNGN_ROCK_WALL && menv[i].x + count_x - 1 > 7 && menv[i].x + count_x - 1 < 72 && menv[i].y + count_y - 1 > 7 && menv[i].y + count_y - 1 < 62)
                    goto tunnel;
            }
            if (grd[menv[i].x + count_x - 1][menv[i].y + count_y - 1] < okmove)
            {
                good_move[count_x][count_y] = 0;
                continue;
            }
          tunnel:if (mgrd[menv[i].x + count_x - 1][menv[i].y + count_y - 1] != MNG)
            {
                if (!(menv[mgrd[menv[i].x + count_x - 1][menv[i].y + count_y - 1]].behavior == BEH_ENSLAVED || menv[i].behavior == BEH_ENSLAVED && menv[mgrd[menv[i].x + count_x - 1][menv[i].y + count_y - 1]].behavior != menv[i].behavior))
                {
                    good_move[count_x][count_y] = 0;
                    continue;
                }
            }
            if (env.cgrid[menv[i].x + count_x - 1][menv[i].y + count_y - 1] != CNG)
            {
                if (env.cgrid[menv[i].x][menv[i].y] != CNG && env.cloud_type[env.cgrid[menv[i].x + count_x - 1][menv[i].y + count_y - 1]] == env.cloud_type[env.cgrid[menv[i].x][menv[i].y]])
                    continue;
                if (mons_intel(menv[i].type) <= 1)
                    continue;
                switch (env.cloud_type[env.cgrid[menv[i].x + count_x - 1][menv[i].y + count_y - 1]])
                {
                case 1:
                    if (mons_res_fire(menv[i].type) > 0 || (menv[i].inv[2] != 501 && mitm.special[menv[i].inv[2]] % 30 == 2))
                        continue;
                    if (menv[i].hit_points >= 15 + random2(10) + random2(10) + random2(10) + random2(10) + random2(10))
                        continue;
                    break;

                case 2:
                    if (mons_res_poison(menv[i].type) > 0)
                        continue;
                    if (random2(5) + 1 < menv[i].hit_dice)
                        continue;
                    if (menv[i].hit_points >= random2(10) + random2(10))
                        continue;
                    break;

                case 3:
                    if (mons_res_cold(menv[i].type) > 0 || (menv[i].inv[2] != 501 && mitm.special[menv[i].inv[2]] % 30 == 3))
                        continue;
                    if (menv[i].hit_points >= 15 + random2(10) + random2(10) + random2(10) + random2(10) + random2(10))
                        continue;
                    break;

                case 4:
                    if (mons_res_poison(menv[i].type) > 0)
                        continue;
                    if (menv[i].hit_points >= random2(10) + random2(10) + random2(10) + random2(10))
                        continue;
                    break;

                case 5:
                    if (mons_res_fire(menv[i].type) > 0 || (menv[i].inv[2] != 501 && mitm.special[menv[i].inv[2]] % 30 == 2))
                        continue;
                    if (menv[i].hit_points >= random2(10) + random2(10))
                        continue;
                    break;

                default:
                    continue;   /* harmless clouds */
                }
                good_move[count_x][count_y] = 0;
            }
        }
    }

/* some monsters opening doors: change the gmon_use == 1 to gmon_use > 0 maybe? */
    if (grd[menv[i].x + mmov_x][menv[i].y + mmov_y] == DNGN_CLOSED_DOOR || grd[menv[i].x + mmov_x][menv[i].y + mmov_y] == DNGN_SECRET_DOOR)
    {
        if (menv[i].type == MONS_SMALL_ZOMBIE || menv[i].type == MONS_BIG_ZOMBIE || menv[i].type == MONS_SMALL_SKELETON || menv[i].type == MONS_LARGE_SKELETON || menv[i].type == MONS_SPECTRAL_THING)
        {
            if (gmon_use[menv[i].number] > 0)
            {
                grd[menv[i].x + mmov_x][menv[i].y + mmov_y] = DNGN_OPEN_DOOR;
                return;
            }
        }
        else if (gmon_use[menv[i].type] > 0)
        {
            grd[menv[i].x + mmov_x][menv[i].y + mmov_y] = DNGN_OPEN_DOOR;
            return;
        }

        if (menv[i].type == MONS_JELLY)
        {
            grd[menv[i].x + mmov_x][menv[i].y + mmov_y] = DNGN_FLOOR;
            strcpy(info, "You hear a slurping sound.");
            mpr(info);
            menv[i].hit_points += 5;
            if (menv[i].hit_points > menv[i].max_hit_points)
                menv[i].max_hit_points = menv[i].hit_points;
        }
    }


/*  equivalent of your move() for monsters: */
    if (mmov_x != 0 && mmov_y != 0)
    {

        // Now, we want to make the monster move in a straight line unless an
        //  oblique line is faster (often, either is optimum above a certain
        //  distance from the target). But should be a little random
        if (random2(4) != 0 && mmov_x != 0 && mmov_y != 0 && abs(menv[i].y - menv[i].target_y) != abs(menv[i].x - menv[i].target_x))
        {
            if (abs(menv[i].target_x - menv[i].x) > abs(menv[i].target_y - menv[i].y))
            {
                if (good_move[mmov_x + 1][1] == 1)
                        mmov_y = 0;
            }
                else if (good_move[1][mmov_y + 1] == 1)
                        mmov_x = 0;
        }

        if (good_move[mmov_x + 1][mmov_y + 1] == 0)
        {

            /* some monsters opening doors */
            if (gmon_use[menv[i].type] > 0 && (grd[menv[i].x + mmov_x][menv[i].y + mmov_y] == DNGN_CLOSED_DOOR || grd[menv[i].x + mmov_x][menv[i].y + mmov_y] == DNGN_SECRET_DOOR))
            {
                if (menv[i].type == MONS_SMALL_ZOMBIE || menv[i].type == MONS_BIG_ZOMBIE || menv[i].type == MONS_SMALL_SKELETON || menv[i].type == MONS_LARGE_SKELETON || menv[i].type == MONS_SPECTRAL_THING)
                {
                    if (gmon_use[menv[i].number] > 0)
                    {
                        grd[menv[i].x + mmov_x][menv[i].y + mmov_y] = DNGN_OPEN_DOOR;
                        return;
                    }
                }
                else
                {
                    grd[menv[i].x + mmov_x][menv[i].y + mmov_y] = DNGN_OPEN_DOOR;
                    return;
                }
            }

            if (which_first == 1)
            {

                if (good_move[mmov_x + 1][1] == 1)

                {
                    mmov_y = 0;

                }
                else if (good_move[1][mmov_y + 1] == 1)
                {
                    mmov_x = 0;
                }

            }                   /* end if which_first */
            else
            {

                if (good_move[1][mmov_y + 1] == 1)
                {
                    mmov_x = 0;
                }
                else if (good_move[mmov_x + 1][1] == 1)
                {
                    mmov_y = 0;
                }
            }                   /* end if which_first */


            if (mmov_x != 0 && mmov_y != 0)
            {
                mmov_x = random2(3) - 1;
                mmov_y = random2(3) - 1;
            }

        }
    }
    else
        /* end of: if (mmov_x !=0 etc */
    {

        if (mmov_x != 0 ^ mmov_y != 0)  /* ^ = XOR, || = OR(inc) */
        {
            if (mmov_x != 0 && (good_move[mmov_x + 1][1] == 0))
            {

                if (random2(2) == 0)
                {

                    if (good_move[1][0] == 1)
                    {
                        mmov_y = -1;
                        mmov_x = 0;
                    }
                    else
                    {
                        if (good_move[1][2] == 1)
                        {
                            mmov_y = 1;
                            mmov_x = 0;
                        }
                    }
                }
                else
                {
                    if (good_move[1][2] == 1)
                    {
                        mmov_y = 1;
                        mmov_x = 0;
                    }
                    else
                    {
                        if (good_move[1][0] == 1)
                        {
                            mmov_y = -1;
                            mmov_x = 0;
                        }
                    }
                }               /* end if random2(2) else */

            }
            else if (mmov_y != 0 && (good_move[1][mmov_y + 1] == 0))
            {

                if (random2(2) == 0)
                {

                    if (good_move[0][1] == 1)
                    {
                        mmov_x = -1;
                        mmov_y = 0;
                    }
                    else if (good_move[2][1] == 1)
                    {
                        mmov_x = 1;
                        mmov_y = 0;
                    }
                }
                else
                {


                    if (good_move[2][1] == 1)
                    {
                        mmov_x = 1;
                        mmov_y = 0;
                    }
                    else if (good_move[0][1] == 1)
                    {
                        mmov_x = -1;
                        mmov_y = 0;
                    }

                }               /* end random2(2) else */

            }


            if (mmov_x != 0 && mmov_y != 0)
            {
                if (good_move[mmov_x + 1][mmov_y + 1] == 1)
                {

                    {
                        mmov_y = random2(3) - 1;
                        mmov_x = random2(3) - 1;
                    }
                }
            }

        }                       // end of if (mmov_x != 0 ^ etc for the 2nd time

    }                           // end of else above


/* maybe: (makes worms stupid, though) */
    if ((menv[i].type == MONS_TUNNELING_WORM || menv[i].type == MONS_BORING_BEETLE) && mmov_x != 0 && mmov_y != 0)
    {
        if (random2(2) == 0)
            mmov_x = 0;
        else
            mmov_y = 0;
        if (grd[menv[i].x + mmov_x][menv[i].y + mmov_y] == DNGN_STONE_WALL && menv[i].x + mmov_x > 6 && menv[i].x + mmov_x < 73 && menv[i].y + mmov_y > 6 && menv[i].y + mmov_y < 63)
        {
            grd[menv[i].x + mmov_x][menv[i].y + mmov_y] = DNGN_FLOOR;
            strcpy(info, "You hear a grinding noise.");
            mpr(info);
        }
    }

    unsigned char grik = grd[menv[i].x + mmov_x][menv[i].y + mmov_y];

    mgrd[menv[i].x][menv[i].y] = MNG;

    if (grik >= okmove && mgrd[menv[i].x + mmov_x][menv[i].y + mmov_y] == MNG)
    {

        if (menv[i].x + mmov_x == you.x_pos && menv[i].y + mmov_y == you.y_pos)
        {
            monster_attack(i);
            mmov_x = 0;
            mmov_y = 0;
        }
        vacated_x = menv[i].x;
        vacated_y = menv[i].y;

        if (menv[i].type >= MLAVA0 && menv[i].type < MWATER0 && grik != DNGN_LAVA)
        {
            mmov_x = 0;
            mmov_y = 0;
        }
        if (menv[i].type >= MWATER0 && grik != DNGN_DEEP_WATER && grik != DNGN_SHALLOW_WATER && grik != DNGN_BLUE_FOUNTAIN)
        {
            mmov_x = 0;
            mmov_y = 0;
        }

        if (menv[i].type == MONS_WANDERING_MUSHROOM && see_grid(menv[i].x + mmov_x, menv[i].y + mmov_y) == 1)
        {
            mmov_x = 0;
            mmov_y = 0;
        }

        if (menv[i].type == MONS_EFREET || menv[i].type == MONS_FIRE_ELEMENTAL)
            place_cloud(101, menv[i].x, menv[i].y, 2 + random2(4));

        /* this appears to be the real one, ie where the movement occurs: */
        menv[i].x += mmov_x;
        menv[i].y += mmov_y;

        if (menv[i].type == MONS_TUNNELING_WORM && mgrd[vacated_x][vacated_y] == MNG && (mmov_x != 0 || mmov_y != 0))   //(mmov_x != 0 || mmov_y != 0)) // worm

        {
            char vac_x_2 = vacated_x;   //menv [i].x;

            char vac_y_2 = vacated_y;   //menv [i].y;

            mgrd[menv[i].x][menv[i].y] = i;

            i++;

            if (menv[i].type != MONS_WORM_TAIL)
            {
                goto out_of_worm;
            }

            while (menv[i].type == MONS_WORM_TAIL)
            {
                mgrd[menv[i].x][menv[i].y] = MNG;

                vacated_x = menv[i].x;
                vacated_y = menv[i].y;

                menv[i].x = vac_x_2;
                menv[i].y = vac_y_2;

                vac_x_2 = vacated_x;
                vac_y_2 = vacated_y;

                mgrd[menv[i].x][menv[i].y] = i;         //+= 50;

                i++;
            }

          out_of_worm:i--;

            return;             /* hopefully will do next monster... */

        }


    }
    else
    {

    }

/* need to put in something so that monster picks up multiple items (eg ammunition) identical to those it's carrying. */

    mgrd[menv[i].x][menv[i].y] = i;

/* monsters stepping on traps: */
    if (grd[menv[i].x][menv[i].y] >= DNGN_TRAP_I && grd[menv[i].x][menv[i].y] <= DNGN_UNDISCOVERED_TRAP && (mmov_x != 0 || mmov_y != 0))
    {
//if (mons_flies(menv [i].type) == 0)
        mons_trap(i);
    }                           /* end of if monster steps on trap */


}                               /* end of monster_move() */




void plant_spit(int i, struct bolt beam[1])
{
    if (!mons_near(i))
        return;

    strcpy(info, monam(menv[i].number, menv[i].type, menv[i].enchantment[2], 0));
    strcat(info, " spits at you.");
    mpr(info);

    beam[0].move_x = beam[0].trac_targ_x - menv[i].x;
    beam[0].move_y = beam[0].trac_targ_y - menv[i].y;
    strcpy(beam[0].beam_name, "plant spit");

    beam[0].type = '#';
    beam[0].source_x = menv[i].x;
    beam[0].source_y = menv[i].y;
    beam[0].range = 10;
    beam[0].colour = YELLOW;
    beam[0].flavour = BEAM_CLOUD;
    beam[0].beam_source = i;

    beam[0].damage = 107;

    beam[0].hit = 20 + menv[i].hit_dice * 3;

    beam[0].thing_thrown = 4;

    missile(beam, 0);
}








void mons_in_cloud(int i)
{

    int wc = env.cgrid[menv[i].x][menv[i].y];
    int hurted = 0;
    struct bolt beam[1];

    if (menv[i].type >= MONS_GOLD_MIMIC && menv[i].type <= MONS_POTION_MIMIC)
    {
        mimic_alert(i);
        return;
    }

    switch (env.cloud_type[wc] % 100)
    {
    case 99:
        cprintf("Fatal error: monster steps on nonexistant cloud!");
        exit(0);
        break;

    case 1:                     // fire

        if (menv[i].type == MONS_FIRE_VORTEX || menv[i].type == MONS_EFREET || menv[i].type == MONS_FIRE_ELEMENTAL)
            break;              // fire vortex, efreet, and fire elemental

        strcpy(info, monam(menv[i].number, menv[i].type, menv[i].enchantment[2], 0));   //gmon_name [menv [i].type]);

        strcat(info, " is engulfed in flame!");
        if (mons_near(i) && menv[i].type != MONS_EFREET)
            mpr(info);          // efreet

        if (mons_res_fire(menv[i].type) > 0)
            break;
        hurted += ((random2(8) + random2(5) + random2(5) + 6) * 10) / menv[i].speed;
        if (mons_res_fire(menv[i].type) < 0 && !(menv[i].inv[2] != 501 && mitm.special[menv[i].inv[2]] % 30 == 2))
            hurted += (random2(15) * 10) / menv[i].speed;
        // remember that the above is in addition to the other you.damage.
        hurted -= random2(menv[i].armor_class + 1);
        if (menv[i].inv[2] != 501 && mitm.special[menv[i].inv[2]] % 30 == 2)
        {
            hurted /= 3;
        }
        if (hurted <= 0)
            hurted = 0;
        menv[i].hit_points -= hurted;
        if (menv[i].hit_points < 1)
        {
            if (env.cloud_type[wc] >= 100)
                monster_die(i, 5, 0);
            else
                monster_die(i, 1, 0);
            menv[i].speed_increment = 1;
        }
        break;

    case 2:                     // stinking cloud

        strcpy(info, monam(menv[i].number, menv[i].type, menv[i].enchantment[2], 0));
        strcat(info, " is engulfed in noxious gasses!");
        if (mons_near(i))
            mpr(info);

        if (mons_res_poison(menv[i].type) > 0)
            return;

        int func_pass[10];

        beam[0].colour = 4;
        if (random2(27) + 1 >= menv[i].hit_dice)
            mons_ench_f2(i, mons_near(i), func_pass, beam);

        hurted += (random2(3) * 10) / menv[i].speed;
//   hurted -= random2(menv [i].armor_class + 1);
        if (hurted <= 0)
            hurted = 0;
        menv[i].hit_points -= hurted;
        if (menv[i].hit_points < 1)
        {
            if (env.cloud_type[wc] >= 100)
                monster_die(i, 5, 0);
            else
                monster_die(i, 1, 0);

        }
        break;




    case 3:                     // cold

        strcpy(info, monam(menv[i].number, menv[i].type, menv[i].enchantment[2], 0));   //gmon_name [mons_class [i]]);

        strcat(info, " is engulfed in freezing vapours!");
        if (mons_near(i))
            mpr(info);
        if (mons_res_cold(menv[i].type) > 0)
            break;
        hurted += ((random2(8) + random2(5) + random2(5) + 6) * 10) / menv[i].speed;
        if (mons_res_cold(menv[i].type) < 0 && !(menv[i].inv[2] != 501 && mitm.special[menv[i].inv[2]] % 30 == 3))
            hurted += (random2(15) * 10) / menv[i].speed;
        // remember that the above is in addition to the other damage.
        hurted -= random2(menv[i].armor_class + 1);
        if (menv[i].inv[2] != 501 && mitm.special[menv[i].inv[2]] % 30 == 3)
        {
            hurted /= 3;
        }
        if (hurted <= 0)
            hurted = 0;
        menv[i].hit_points -= hurted;
        if (menv[i].hit_points < 1)
        {
            if (env.cloud_type[wc] >= 100)
                monster_die(i, 5, 0);
            else
                monster_die(i, 1, 0);
            menv[i].speed_increment = 1;
        }
        break;

    case 4:                     // you.poison cloud

        strcpy(info, monam(menv[i].number, menv[i].type, menv[i].enchantment[2], 0));
        strcat(info, " is engulfed in a cloud of poison!");
        if (mons_near(i))
            mpr(info);
        if (mons_res_poison(menv[i].type) > 0)
            return;
        if (env.cloud_type[wc] >= 100)
            poison_monster(i, 1);       // something else

        else
            poison_monster(i, 0);

        hurted += (random2(8) * 10) / menv[i].speed;
        if (mons_res_poison(menv[i].type) < 0)
            hurted += (random2(4) * 10) / menv[i].speed;
        if (hurted <= 0)
            hurted = 0;
        menv[i].hit_points -= hurted;
        if (menv[i].hit_points < 1)
        {
            if (env.cloud_type[wc] >= 100)
                monster_die(i, 5, 0);
            else
                monster_die(i, 1, 0);
        }
        break;

// 5,6,7 are harmless smoke

    case 8:                     // steam - I couldn't be bothered doing this for armour of res fire

        if (menv[i].type == MONS_STEAM_DRAGON)
            break;
        strcpy(info, monam(menv[i].number, menv[i].type, menv[i].enchantment[2], 0));   //gmon_name [mons_class [i]]);

        strcat(info, " is engulfed in steam!");
        if (mons_near(i) && menv[i].type != MONS_EFREET)
            mpr(info);          // efreet

        if (mons_res_fire(menv[i].type) > 0 || (menv[i].inv[2] != 501 && mitm.special[menv[i].inv[2]] % 30 == 2))
            break;
        hurted += (random2(6) * 10) / menv[i].speed;
        if (mons_res_fire(menv[i].type) < 0 && !(menv[i].inv[2] != 501 && mitm.special[menv[i].inv[2]] % 30 == 2))
            hurted += (random2(6) * 10) / menv[i].speed;
        hurted -= random2(menv[i].armor_class + 1);
        if (hurted <= 0)
            hurted = 0;
        menv[i].hit_points -= hurted;
        if (menv[i].hit_points < 1)
        {
            if (env.cloud_type[wc] >= 100)
                monster_die(i, 5, 0);
            else
                monster_die(i, 1, 0);
            menv[i].speed_increment = 1;
        }
        break;


    case 9:                     // dark miasma

        strcpy(info, monam(menv[i].number, menv[i].type, menv[i].enchantment[2], 0));
        strcat(info, " is engulfed in a dark miasma.");
        if (mons_near(i))
            mpr(info);
        if (mons_holiness(menv[i].type) > 0)
            return;
        if (env.cloud_type[wc] >= 100)
            poison_monster(i, 1);       // something else

        else
            poison_monster(i, 0);

        hurted += ((random2(5) + random2(5) + random2(4)) * 10) / menv[i].speed;        // 3

        if (hurted <= 0)
            hurted = 0;
        menv[i].hit_points -= hurted;
        if (menv[i].max_hit_points > 4)
        {
            menv[i].max_hit_points -= random2(2);
        }
        beam[0].colour = 0;
        if (random2(3) == 0)
            mons_ench_f2(i, mons_near(i), func_pass, beam);

        if (menv[i].hit_points < 1)
        {
            if (env.cloud_type[wc] >= 100)
                monster_die(i, 5, 0);
            else
                monster_die(i, 1, 0);
        }
        break;

    }


}


char mons_speaks(int i)
{

    if (menv[i].behavior == BEH_FLEE)
        return 0;

    strcpy(info, monam(menv[i].number, menv[i].type, menv[i].enchantment[2], 0));

    switch (menv[i].type)
    {
    case MONS_DORGI:            // dorgi

        switch (random2(10))
        {
        case 0:
            strcat(info, " screams \"HALT!\"");
            break;
        case 1:
            strcat(info, " screams \"HALT! HALT RIGHT NOW!\"");
            break;
        case 2:
            strcat(info, " screams \"HALT, OR YOU WILL BE ELIMINATED!\"");
            break;
        case 3:
            strcat(info, " screams \"HALT OR I SHOOT!\"");
            break;
        case 4:
            strcat(info, " screams \"DROP YOUR WEAPONS! UP AGAINST THE WALL!\"");
            break;
        case 5:
            strcat(info, " screams \"PREPARE TO DIE!\"");
            break;
        case 6:
            strcat(info, " screams \"SUBMIT OR DIE!\"");
            break;
        case 7:
            strcat(info, " screams \"YOU ARE VIOLATING AREA SECURITY!\"");
            break;
        case 8:
            strcat(info, " screams \"SUBMIT TO THERAPY OR DIE!\"");
            break;
        case 9:
            strcat(info, " screams \"HALT, TRESPASSER!\"");
            break;
        }
        mpr(info);
        break;

    case MONS_KILLER_KLOWN:     // Killer Klown

        switch (random2(10))
        {
        case 0:
            strcat(info, " giggles crazily.");
            break;
        case 1:
            strcat(info, " laughs merrily.");
            break;
        case 2:
            strcat(info, " beckons to you.");
            break;
        case 3:
            strcat(info, " does a flip.");
            break;
        case 4:
            strcat(info, " does a somersault.");
            break;
        case 5:
            strcat(info, " smiles at you.");
            break;
        case 6:
            strcat(info, " grins with merry abandon.");
            break;
        case 7:
            strcat(info, " howls with blood-lust!");
            break;
        case 8:
            strcat(info, " pokes out its tongue.");
            break;
        case 9:
            strcat(info, " says \"Come and play with me!\"");
            break;
        }
        mpr(info);
        break;

/* default:
   switch(random2(10))
   {
   case 0: strcat(info, " laughs at you."); break;
   case 1: strcat(info, " salutes you derisively."); break;
   case 2: strcat(info, " beckons to you."); break;
   case 3: strcat(info, " smiles in anticipation of an easy kill."); break;
   case 4: strcat(info, " giggles maniacally."); break;
   case 5: strcat(info, " ."); break;
   case 6: strcat(info, " ."); break;
   case 7: strcat(info, " ."); break;
   case 8: strcat(info, " ."); break;
   case 9: strcat(info, " ."); break;
   }
   mpr(info);
   break; */

    }

    return 1;

}

/*
 *  File:       monsplace.cc
 *  Summary:    Functions used when placing monsters in the dungeon.
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 *               <1>     -/--/--        LRH             Created
 */

#include "AppHdr.h"
#include "monplace.h"

#include "externs.h"

#include "dungeon.h"
#include "monstuff.h"
#include "stuff.h"

int mons_place(int typed, bool is_summoning, int px, int py, char behaviour,
               int hitting, unsigned char plus_seventy, int lev_mons)
{
    FixedVector < int, 2 > passed;
    int nomons = 0;
    int temp_rand = 0;          // probabilty determination {dlb}
    bool permit_bands = true;

    passed[0] = 0;
    passed[1] = 0;

    for (int il = 0; il < MAX_MONSTERS; il++)
    {
        if (menv[il].type != -1)
            nomons++;
    }

    if (typed == WANDERING_MONSTER)
    {
        if (nomons > 150)
            return -1;

        typed = RANDOM_MONSTER;
    }

    // all monsters have been assigned? {dlb}
    if (nomons > MAX_MONSTERS - 2)
        return -1;

    if (you.char_direction == DIR_ASCENDING && typed == RANDOM_MONSTER
                                    && you.level_type == LEVEL_DUNGEON)
    {
        temp_rand = random2(276);

        typed = ((temp_rand > 184) ? MONS_WHITE_IMP + random2(15) :  // 33.33%
                 (temp_rand > 104) ? MONS_HELLION + random2(10) :    // 28.99%
                 (temp_rand > 78)  ? MONS_HELL_HOUND :               //  9.06%
                 (temp_rand > 54)  ? MONS_ABOMINATION_LARGE :        //  8.70%
                 (temp_rand > 33)  ? MONS_ABOMINATION_SMALL :        //  7.61%
                 (temp_rand > 13)  ? MONS_RED_DEVIL                  //  7.25%
                                   : MONS_PIT_FIEND);                //  5.07%
    }

    if (typed != RANDOM_MONSTER && lev_mons != 52)
        permit_bands = false;

    int rval = place_monster( plus_seventy, typed, is_summoning, px, py,
                                behaviour, hitting, permit_bands, lev_mons,
                                passed );

    return ((rval > 0) ? passed[0] : -1);
}                               // end mons_place()


int create_monster(int cls, int dur, int beha, int cr_x, int cr_y,
                   int hitting, int zsec)
{
    int summd = 0;
    int dem_beha = beha;
    FixedVector < char, 2 > empty;
    struct monsters *creation = 0;      // NULL {dlb}

    // see the problem? {dlb}
    unsigned char spcw =
            monster_habitat((cls == RANDOM_MONSTER) ? MONS_PROGRAM_BUG : cls);

    empty[0] = 0;
    empty[1] = 0;

    // This is for the summon greater demons spell,
    // where monsters are hostile but charmed:
    if (beha == BEH_CHASING_II)
        beha = BEH_ENSLAVED;

    // determine whether creating a monster is successful (summd != -1) {dlb}:
    if (!empty_surrounds(cr_x, cr_y, spcw, true, empty))
        summd = -1;
    else
        summd = mons_place( cls, true, empty[0], empty[1], beha, hitting,
                                zsec, you.your_level );

    // then handle the outcome {dlb}:
    if (summd == -1)
    {
        if (see_grid(cr_x, cr_y))
            mpr("You see a puff of smoke.");
    }
    else
    {
        creation = &menv[summd];

        // This is for the summon greater demons spell,
        // where monsters are hostile but charmed:
        if (dem_beha == BEH_CHASING_II)
            creation->enchantment[0] = ENCH_CHARM;

        if (dur)
        {
            // some monsters (eg butterflies) use enchantment[0] for confusion
            creation->enchantment[1] = dur;
            creation->enchantment1 = 1;

            if (beha == BEH_ENSLAVED)
                creation->enchantment[1] += ENCH_FRIEND_ABJ_I - ENCH_ABJ_I;
        }
        else
        {
            if (beha == BEH_ENSLAVED)
                creation->enchantment[1] = ENCH_CREATED_FRIENDLY;
        }

        if (creation->type == MONS_RAKSHASA_FAKE && !one_chance_in(3))
        {
            creation->enchantment[2] = ENCH_INVIS;
            creation->enchantment1 = 1;
        }
    }

    // the return value is either -1 (failure of some sort)
    // or the index of the monster placed (if I read things right) {dlb}
    return summd;
}                               // end create_monster()


bool empty_surrounds(int emx, int emy, unsigned char spc_wanted,
                     bool allow_centre, FixedVector < char, 2 > &empty)
{
    bool success = false;

    int count_x = 0, count_y = 0;

    char minx = -1;
    char maxx = 3;
    char miny = -1;
    char maxy = 2;
    char xinc = 1;
    char yinc = 1;

    if (coinflip())
    {
        minx = 1;
        maxx = -3;
        xinc = -1;
    }

    if (coinflip())
    {
        miny = 1;
        maxy = -2;
        yinc = -1;
    }

    for (count_x = minx; count_x != maxx; count_x += xinc)
    {
        for (count_y = miny; count_y != maxy; count_y += yinc)
        {
            if (!allow_centre && count_x == 0 && count_y == 0)
                continue;

            if (emx + count_x == you.x_pos && emy + count_y == you.y_pos)
                continue;

            if (mgrd[emx + count_x][emy + count_y] != NON_MONSTER)
                continue;

            if (grd[emx + count_x][emy + count_y] == spc_wanted)
            {
                success = true;
                break;          // out of inner loop {dlb}
            }

            // second chance - those seeking ground can stand anywhere {dlb}:
            if (spc_wanted == DNGN_FLOOR
                && grd[emx + count_x][emy + count_y] >= DNGN_SHALLOW_WATER)
            {
                success = true;
                break;          // out of inner loop {dlb}
            }
        }                       // end "for count_y"

        if (success)
            break;              // out of outer loop {dlb}
    }                           // end "for count_x"

    if (success)
    {
        empty[0] = emx + count_x;
        empty[1] = emy + count_y;
    }

    return (success);
}                               // end empty_surrounds()

int summon_any_demon(char demon_class)
{
    int summoned = MONS_PROGRAM_BUG;    // error trapping {dlb}
    int temp_rand = 0;          // probability determination {dlb}

    switch (demon_class)
    {
    case DEMON_LESSER:
        temp_rand = random2(60);
        summoned = ((temp_rand > 49) ? MONS_IMP :        // 10 in 60
                    (temp_rand > 40) ? MONS_WHITE_IMP :  //  9 in 60
                    (temp_rand > 31) ? MONS_LEMURE :     //  9 in 60
                    (temp_rand > 22) ? MONS_UFETUBUS :   //  9 in 60
                    (temp_rand > 13) ? MONS_MANES :      //  9 in 60
                    (temp_rand > 4)  ? MONS_MIDGE        //  9 in 60
                                     : MONS_SHADOW_IMP); //  5 in 60
        break;

    case DEMON_COMMON:
        temp_rand = random2(3948);
        summoned = ((temp_rand > 3367) ? MONS_NEQOXEC :         // 14.69%
                    (temp_rand > 2787) ? MONS_ORANGE_DEMON :    // 14.69%
                    (temp_rand > 2207) ? MONS_HELLWING :        // 14.69%
                    (temp_rand > 1627) ? MONS_SMOKE_DEMON :     // 14.69%
                    (temp_rand > 1047) ? MONS_YNOXINUL :        // 14.69%
                    (temp_rand > 889)  ? MONS_RED_DEVIL :       //  4.00%
                    (temp_rand > 810)  ? MONS_HELLION :         //  2.00%
                    (temp_rand > 731)  ? MONS_ROTTING_DEVIL :   //  2.00%
                    (temp_rand > 652)  ? MONS_TORMENTOR :       //  2.00%
                    (temp_rand > 573)  ? MONS_REAPER :          //  2.00%
                    (temp_rand > 494)  ? MONS_SOUL_EATER :      //  2.00%
                    (temp_rand > 415)  ? MONS_HAIRY_DEVIL :     //  2.00%
                    (temp_rand > 336)  ? MONS_ICE_DEVIL :       //  2.00%
                    (temp_rand > 257)  ? MONS_BLUE_DEVIL :      //  2.00%
                    (temp_rand > 178)  ? MONS_BEAST :           //  2.00%
                    (temp_rand > 99)   ? MONS_IRON_DEVIL :      //  2.00%
                    (temp_rand > 49)   ? MONS_SUN_DEMON         //  1.26%
                                       : MONS_SHADOW_IMP);      //  1.26%
        break;

    case DEMON_GREATER:
        temp_rand = random2(1000);
        summoned = ((temp_rand > 868) ? MONS_CACODEMON :        // 13.1%
                    (temp_rand > 737) ? MONS_BALRUG :           // 13.1%
                    (temp_rand > 606) ? MONS_BLUE_DEATH :       // 13.1%
                    (temp_rand > 475) ? MONS_GREEN_DEATH :      // 13.1%
                    (temp_rand > 344) ? MONS_EXECUTIONER :      // 13.1%
                    (temp_rand > 244) ? MONS_FIEND :            // 10.0%
                    (temp_rand > 154) ? MONS_ICE_FIEND :        //  9.0%
                    (temp_rand > 73)  ? MONS_SHADOW_FIEND       //  8.1%
                                      : MONS_PIT_FIEND);        //  7.4%
        break;

    default:
        summoned = MONS_GIANT_ANT;      // this was the original behavior {dlb}
        break;
    }

    return summoned;
}                               // end summon_any_demon()

/*
 *  File:       abyss.cc
 *  Summary:    Misc functions (most of which don't appear to be related to priests).
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 *     <2>     10/11/99       BCR     Added Daniel's crash patch
 *     <1>     -/--/--        LRH     Created
 */

#include "AppHdr.h"
#include "abyss.h"

#include <stdlib.h>
#include "dungeon.h"
#include "levels.h"
#include "message.h"

#include "externs.h"

#include "stuff.h"

void generate_area(unsigned char gx1, unsigned char gy1, unsigned char gx2, unsigned char gy2);
char area_shift(void);




void priest_spells(int func_pass[10], char religious)
{

    switch (religious)
    {
    case GOD_ZIN:
        func_pass[1] = SPELL_LESSER_HEALING;
        func_pass[2] = SPELL_REPEL_UNDEAD;
        func_pass[3] = SPELL_HEAL_OTHER;
        func_pass[4] = SPELL_PURIFICATION;
        func_pass[5] = SPELL_GREATER_HEALING;
        func_pass[6] = SPELL_SMITING;
        func_pass[7] = SPELL_HOLY_WORD;
        func_pass[8] = SPELL_REMOVE_CURSE;
        func_pass[9] = SPELL_GUARDIAN;
        break;
/*
   case 2: // The Shining One
   func_pass [1] = 46; // repel undead
   func_pass [2] = 38; // lesser healing
   func_pass [3] = 65; // heal other
   func_pass [4] = 41; // purification
   func_pass [5] = 92; // abjuration
   func_pass [6] = 88; // thunderbolt
   func_pass [7] = 90; // shining light
   func_pass [8] = 91; // summon something
   func_pass [9] = 89; // flame of cleansing
   break;

   case 3: // Elyvilon - Healing
   func_pass [1] = 38; // lesser healing
   func_pass [2] = 65; // heal other
   func_pass [3] = 41; // purification
   func_pass [4] = 93; // restore abilities
   func_pass [5] = 39; // greater healing
   func_pass [6] = 93; // another healing spell
   func_pass [7] = 94; // something else
   func_pass [8] = -1; //
   func_pass [9] = -1; //
   break;
*/
    }

}

/*
   Spells to be added: (+ renamed!)
   holy berserker
   86 Summon guardian
   87 Pestilence
   88 Thunderbolt
   89 flame of cleansing?
   90 shining light
   91 summon something
   92 abjuration
   93 another healing spell
   94 something else healing
*/



void generate_area(unsigned char gx1, unsigned char gy1, unsigned char gx2, unsigned char gy2)
{

    int i, j;

    for (i = 0; i < 80; i++)
    {
        for (j = 0; j < 70; j++)
        {
            env.map[i][j] = 0;
            mgrd[i][j] = MNG;
        }
    }

    int x1, x2, y1, y2;

    int items_placed = 0;

    int thickness = random2(70) + 30;

    int what_was_there;
    int thing_created;

    //int it_no2 = 0;
    int rooms_done = 0;
    int rooms_to_do = 0;

    unsigned char replaced[5];
    unsigned char base_type = DNGN_ROCK_WALL;

    if ( one_chance_in(4) )
        base_type = DNGN_STONE_WALL;
    if ( one_chance_in(15) )
        base_type = DNGN_METAL_WALL;
    if ( one_chance_in(20) )
        base_type = DNGN_LAVA;
    if ( one_chance_in(25) )
        base_type = DNGN_DEEP_WATER;
    if ( one_chance_in(1000) )
        base_type = DNGN_CLOSED_DOOR;

    for (i = 0; i < 5; i++)
    {
        replaced[i] = base_type;
        if ( one_chance_in(5) )
            replaced[i] = DNGN_ROCK_WALL;
        if ( one_chance_in(10) )
            replaced[i] = DNGN_STONE_WALL;
        if ( one_chance_in(15) )
            replaced[i] = DNGN_METAL_WALL;
        if ( one_chance_in(20) )
            replaced[i] = DNGN_LAVA;
        if ( one_chance_in(25) )
            replaced[i] = DNGN_DEEP_WATER;
        if ( one_chance_in(15) )
            replaced[i] = DNGN_SHALLOW_WATER;
        if ( one_chance_in(1000) )
            replaced[i] = DNGN_CLOSED_DOOR;
    }


    if ( one_chance_in(3) )
    {
        rooms_to_do = 1 + random2(10);
        do
        {
            x1 = 10 + random2(60);
            y1 = 10 + random2(50);
            x2 = x1 + 1 + random2(10);
            y2 = y1 + 1 + random2(10);
            if ( one_chance_in(100) )
                goto out_of_rooms;
            for (i = x1; i < x2; i++)
            {
                for (j = y1; j < y2; j++)
                {
                    if (grd[i][j] != 30)
                        goto continued;
                }
            }
            for (i = x1; i < x2; i++)
            {
                for (j = y1; j < y2; j++)
                {
                    grd[i][j] = DNGN_FLOOR;
                }
            }
          continued:
            rooms_done++;
            if (rooms_done >= rooms_to_do)
                break;
        }
        while (items_placed == 0);      // always true

    }


out_of_rooms:
    for (i = gx1; i < gx2 + 1; i++)
    {
        for (j = gy1; j < gy2 + 1; j++)
        {
            if (grd[i][j] == 30 && random2(100) <= thickness)
            {
                grd[i][j] = DNGN_FLOOR;
                if ( items_placed < 150 && one_chance_in(200) )  // % 200

                {
                    if ( one_chance_in(500) )
                    {
/*         do
   {
   k = 4 + random2(16);
   if ( one_chance_in(100) ) goto creation;
   } while (you.unique_items [k + 3] == 1); */

                        thing_created = items(1, OBJ_MISCELLANY, MISC_RUNE_OF_ZOT, 1, 51, 250);

                        goto created;
                    }


                    thing_created = items(1, 250, 250, 1, 51, 250);

                  created:what_was_there = igrd[i][j];
                    mitm.link[thing_created] = what_was_there;
                    igrd[i][j] = thing_created;
                    items_placed++;
                }
            }
        }
    }

    for (i = gx1; i < gx2 + 1; i++)
    {
        for (j = gy1; j < gy2 + 1; j++)
        {
            if (grd[i][j] == 30)
                grd[i][j] = replaced[random2(5)];
            if ( one_chance_in(7500) )
                grd[i][j] = DNGN_EXIT_ABYSS; /* gate out of abyss */
            if ( one_chance_in(10000) )  /* altar */
            {
                do
                {
                    grd[i][j] = DNGN_ALTAR_ZIN + random2(12);
                }
                while (grd[i][j] == DNGN_ALTAR_YREDELEMNUL || grd[i][j] == DNGN_ALTAR_VEHUMET || grd[i][j] == DNGN_ALTAR_NEMELEX_XOBEH);
            }
        }
    }

}



char area_shift(void)
{
/*unsigned char grd [80] [70];
   struct player you [1]; */

    int i, j;
    int k, l;
    unsigned char cloud_no2 = 0;


    for (i = 0; i < MNST; i++)
    {
        if (menv[i].type == -1)
            continue;
        if (menv[i].x < you.x_pos - 10 || menv[i].x >= you.x_pos + 11 || menv[i].y < you.y_pos - 10 || menv[i].y >= you.y_pos + 11)
        {
            menv[i].type = -1;
            for (j = 0; j < 8; j++)
            {
                mitm.quantity[menv[i].inv[j]] = 0;
                mitm.link[menv[i].inv[j]] = ING;
                menv[i].inv[j] = ING;
            }
            for (j = 0; j < MNST; j++)
            {
                if (menv[j].monster_foe == i)
                    menv[j].monster_foe = MHITNOT;
            }

        }
        else
        {
            menv[i].target_x = menv[i].x;       // this will make it find a new target
            menv[i].target_y = menv[i].y;
        }
    }


    for (i = 5; i < 75; i++)
    {
        for (j = 5; j < 65; j++)
        {
            if (i > you.x_pos - 10 && i < you.x_pos + 11 && j > you.y_pos - 10 && j < you.y_pos + 11)
                continue;
            grd[i][j] = 30;
            mgrd[i][j] = MNG;
            if (igrd[i][j] != ING)
            {
                k = igrd[i][j];
                igrd[i][j] = ING;
                if (k < 0 || k > ING)
                    k = ING;
                while (k != ING)
                {
                    mitm.quantity[k] = 0;
                    l = mitm.link[k];
                    mitm.link[k] = ING;
                    if (mitm.base_type[k] == OBJ_ORBS && mitm.sub_type[k] >= 4 && mitm.sub_type[k] <= 19)
                    {
                        you.unique_items[mitm.sub_type[k] + 3] = 2;     /* This means lost in the abyss, and can be found again only there. */
                    }
                    if (mitm.base_type[k] == OBJ_WEAPONS && mitm.special[k] >= 181)
                    {
                        if (mitm.special[k] - 181 <= 6)
                            you.unique_items[mitm.special[k] - 181] = 1;
                        else
                            you.unique_items[24 + mitm.special[k] - 181 - 7] = 1;
                    }
                    k = l;
                }
            }

        }
    }



    for (i = you.x_pos - 10; i < you.x_pos + 11; i++)
    {
        if ( i < 0 || i >= GXM )
          continue;
        for (j = you.y_pos - 10; j < you.y_pos + 11; j++)
        {
            if ( j < 0 || j >= GYM )
              continue;
            grd[45 + i - you.x_pos][35 + j - you.y_pos] = grd[i][j];
            igrd[45 + i - you.x_pos][35 + j - you.y_pos] = igrd[i][j];
            igrd[i][j] = ING;
            mgrd[45 + i - you.x_pos][35 + j - you.y_pos] = mgrd[i][j];
            if (mgrd[i][j] != MNG)
            {
                mgrd[45 + i - you.x_pos][35 + j - you.y_pos] = mgrd[i][j];
                menv[mgrd[45 + i - you.x_pos][35 + j - you.y_pos]].x = 45 + i - you.x_pos;
                menv[mgrd[45 + i - you.x_pos][35 + j - you.y_pos]].y = 35 + j - you.y_pos;
                mgrd[i][j] = MNG;
            }
            else
                mgrd[45 + i - you.x_pos][35 + j - you.y_pos] = MNG;

            env.cgrid[45 + i - you.x_pos][35 + j - you.y_pos] = env.cgrid[i][j];
            if (env.cgrid[i][j] != CNG)
            {
                env.cgrid[45 + i - you.x_pos][35 + j - you.y_pos] = env.cgrid[i][j];
                env.cloud_x[env.cgrid[45 + i - you.x_pos][35 + j - you.y_pos]] = 45 + i - you.x_pos;
                env.cloud_y[env.cgrid[45 + i - you.x_pos][35 + j - you.y_pos]] = 35 + j - you.y_pos;
                env.cgrid[i][j] = CNG;
            }
            else
                env.cgrid[45 + i - you.x_pos][35 + j - you.y_pos] = CNG;
        }
    }


    for (i = 0; i < CLOUDS; i++)
    {
        if (env.cloud_type[i] == CLOUD_NONE)
            continue;
        if (env.cloud_x[i] < 35 || env.cloud_x[i] > 55 || env.cloud_y[i] < 25 || env.cloud_y[i] > 45)
        {
            env.cloud_type[i] = CLOUD_NONE;
            env.cloud_decay[i] = 0;
        }
        else
            cloud_no2++;
    }


    you.x_pos = 45;
    you.y_pos = 35;

    generate_area(5, 5, 75, 65);

    int passed[2];

    passed[0] = 0;
    passed[1] = 0;

    int mcount = 0;

    for (mcount = 0; mcount < 15; mcount++)
    {

        mcount +=               /*place_monster(
                                   400, // this shouldn't do anything?
                                   250, // used for menv_sec for zombies
                                   250, //250, // typed,
                                   0, //0, //type_place,
                                   1, //1, //px,
                                   1, //1, //py,
                                   1, //0, //behaviour,
                                   MHITNOT, //MHITNOT, //hitting,
                                   1,//1,//1,//allow_bands
                                   gmon_use,
                                   grd,
                                   passed,
                                   you.x_pos,
                                   you.y_pos,
                                   mgrd,
                                   menv,
                                   51
                                   ); */

            place_monster(
                             250,       //unsigned char plus_seventy,
                              250, 0, 1, 1, 1,
                             MHITNOT,
                             1,
                             51, passed);


    }

    return cloud_no2;

}




// must set cloud_no to zero when calling this function
void abyss_teleport(void)
{

    int i, j, k;

    init_pandemonium();         /* changes colours */
    unsigned char pcol = 0;

    pcol = mcolour[env.mons_alloc[9]];
    if (pcol == 0)
        pcol = LIGHTGREY;
    env.floor_colour = pcol;
    pcol = mcolour[env.mons_alloc[8]];
    if (pcol == 0)
        pcol = LIGHTGREY;
    env.rock_colour = pcol;

    for (i = 10; i < 71; i++)
    {
        for (j = 10; j < 61; j++)
        {
            grd[i][j] = 30;
            igrd[i][j] = ING;
            mgrd[i][j] = MNG;
            env.cgrid[i][j] = CNG;
        }
    }

    for (k = 0; k < ITEMS; k++)
    {
        if (mitm.quantity[k] >= 1)
        {
            if (mitm.base_type[k] == OBJ_ORBS)
            {
                you.unique_items[mitm.sub_type[k] + 3] = 2;
            }
            if (mitm.base_type[k] == OBJ_WEAPONS && mitm.special[k] >= 181)
            {
                if (mitm.special[k] - 181 <= 6)
                    you.unique_items[mitm.special[k] - 181] = 1;
                else
                    you.unique_items[24 + mitm.special[k] - 181 - 7] = 1;
//      you.unique_items [mitm.special [k] - 181] = 2;
            }
        }
        mitm.quantity[k] = 0;
        mitm.link[k] = ING;
    }

    for (i = 0; i < MNST; i++)
    {
        menv[i].type = -1;
    }

    for (i = 0; i < CLOUDS; i++)
    {
        env.cloud_type[i] = CLOUD_NONE;
        env.cloud_decay[i] = 0;
        env.cloud_x[i] = 0;
        env.cloud_y[i] = 0;
    }

    env.cloud_no = 0;           // I know this doesn't do anything, of course

    you.x_pos = 45;
    you.y_pos = 35;

    generate_area(10, 10, 70, 60);

    grd[you.x_pos][you.y_pos] = DNGN_FLOOR;

}

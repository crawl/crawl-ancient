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
#include "enum.h"

void generate_area(unsigned char gx1, unsigned char gy1, unsigned char gx2, unsigned char gy2);
char area_shift(void);



/*
   char *god_name(char which_god)
   {

   switch(which_god)
   {
   case 1: return "Zin";
   case 2: return "The Shining One";
   case 3: return "Elyvilon";
   case 4: return "Nelemex Xobeh";
   case 5: return "Jurubetut";
   case 6: return "Vuhemeti";
   case 7: return "Okawaru";
   case 8: return "Kikubaaqudgha";
   case 9: return "Sif Muna";
   case 10: return "Lugafu the Hairy";
   }

   return "The God of Software Bugs";
   }
 */

void priest_spells(int func_pass[10], char religious)
{

    switch (religious)
    {
    case 1:                     // Zin

        func_pass[1] = 38;      // lesser healing

        func_pass[2] = 46;      // repel undead

        func_pass[3] = 65;      // heal other

        func_pass[4] = 41;      // purification

        func_pass[5] = 39;      // greater healing

        func_pass[6] = 45;      // smiting

        func_pass[7] = 47;      // holy word

        func_pass[8] = 4;       // pestilence  - remove curse?

        func_pass[9] = 86;      // summon guardian

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


   case 0: return 6; // identify
   case 1: return 5; // teleportation
   case 2: return 6; // cause fear
   case 3: return 1; // noise
   case 4: return 5; // remove curse
   case 5: return 1; // magic missile
   case 6: return 5; // fireball
   case 13: return 3; // conjure flame
   case 14: return 4; // dig
   case 15: return 5; // firebolt
   case 16: return 5; // bolt of freezing cold
   case 17: return 6; // bolt of lightning
   case 20: return 6; // polymorph other
   case 21: return 3;
   case 22: return 4;
   case 23: return 4;
   case 24: return 3;
   case 25: return 4; // invisibility
   case 26: return 2; // throw flame
   case 27: return 2; // throw frost
   case 28: return 4; // blink
   case 29: return 7; // freezing cloud
   case 30: return 3; // stinking cloud
   case 31: return 8; // ring of flames
   case 32: return 2; //  restore strength:
   case 33: return 2; //          int
   case 34: return 2; //          dex
   case 35: return 5; // venom bolt
   case 36: return 4; // toxic radiance - uses lots of food?
   case 37: return 5; // teleport other
   case 38: return 2; // lesser healing
   case 39: return 6; // greater healing
   case 40: return 3; // cure poison
   case 41: return 5; // purification
   case 42: return 8; // death's door
   case 43: return 3; // selective amnesia
   case 44: return 7; // mass confusion
   case 45: return 4; // smiting
   case 46: return 3; // repel undead
   case 47: return 7; // holy word
   case 48: return 3; // detect curse
   case 49: return 1; // summon small mammal
   case 50: return 4; // Abjuration
   case 51: return 4; // summon scorpions
   case 52: return 3; // levitation
   case 53: return 6; // bolt of draining
   case 54: return 8; // splinters
   case 55: return 4; // innacuracy
   case 56: return 6; // poisonous cloud
   case 57: return 9; // fire storm
   case 58: return 2; // detect traps
   case 59: return 2; // random blink
   case 60: return 5; // Isk's blast
   case 61: return 5; // swarm
   case 62: return 8; // Summon Thing
   case 63: return 4; // Enslavement
   case 64: return 4; // Magic Mapping
   case 65: return 3; // heal other
   case 66: return 7; // Animate dead
   case 67: return 1; // Pain
   case 68: return 4; // Extension
   case 69: return 6; // Control Undead
   case 70: return 3; // animate skeleton
   case 71: return 4; // vampiric draining
   case 72: return 8; // summon greater undead
   case 73: return 2; // detect items
   case 74: return 6; // reviv
   case 75: return 1; // burn
   case 76: return 1; // freeze
   case 77: return 6; // Summon elemental
   case 78: return 5; // refrigeration
   case 79: return 4; // Sticky flame
   case 80: return 5; // Ice beast
   case 81: return 4; // Ozocubu's Armour
   case 82: return 2; // imp
   case 83: return 2; // deflect missiles
   case 84: return 3; // berserker
   case 85: return 4; // dispel undead


   / *
   Gods:
   1:Zin - old priest/P
   2The Shining One
   3Lugafu the Hairy
   4Elyvilon - Healing
   5Nelemex Xobeh - Trickster
   6Jurubetut - Fire
   7Vuhimeti - Ice
   8Okawaru - war, destruction, chaos etc
   9Kikubaaqudgha - Death
   10Sif Muna - magic
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

    int thickness = random() % 70 + 30;

    int what_was_there;
    int thing_created;

//int it_no2 = 0;
    int rooms_done = 0;
    int rooms_to_do = 0;

    unsigned char replaced[5];
    unsigned char base_type = 1;

    if (random() % 4 == 0)
        base_type = 2;
    if (random() % 15 == 0)
        base_type = 4;
    if (random() % 20 == 0)
        base_type = 61;
    if (random() % 25 == 0)
        base_type = 62;
    if (random() % 1000 == 0)
        base_type = 3;


    for (i = 0; i < 5; i++)
    {
        replaced[i] = base_type;
        if (random() % 5 == 0)
            replaced[i] = 1;
        if (random() % 10 == 0)
            replaced[i] = 2;
        if (random() % 15 == 0)
            replaced[i] = 4;
        if (random() % 20 == 0)
            replaced[i] = 61;
        if (random() % 25 == 0)
            replaced[i] = 62;
        if (random() % 15 == 0)
            replaced[i] = 65;
        if (random() % 1000 == 0)
            replaced[i] = 3;
    }


    if (random() % 3 == 0)
    {
        rooms_to_do = 1 + random() % 10;
        do
        {
            x1 = 10 + random() % 60;
            y1 = 10 + random() % 50;
            x2 = x1 + 1 + random() % 10;
            y2 = y1 + 1 + random() % 10;
            if (random() % 100 == 0)
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
                    grd[i][j] = 67;
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
            if (grd[i][j] == 30 && random() % 100 <= thickness)
            {
                grd[i][j] = 67;
                if (random() % 200 == 0 && items_placed < 150)  // % 200

                {
                    if (random() % 500 == 0)
                    {
/*         do
   {
   k = 4 + random() % 16;
   if (random() % 100 == 0) goto creation;
   } while (you.unique_items [k + 3] == 1); */

                        thing_created = items(1, 13, 14, 1, 51, 250);

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
                grd[i][j] = replaced[random() % 5];
            if (random() % 7500 == 0)
                grd[i][j] = 97; /* gate out of abyss */
            if (random() % 10000 == 0)  /* altar */
            {
                do
                {
                    grd[i][j] = 180 + random() % 12;
                }
                while (grd[i][j] == 183 || grd[i][j] == 185 || grd[i][j] == 190);
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
                mitm.link[menv[i].inv[j]] = 501;
                menv[i].inv[j] = 501;
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
            if (igrd[i][j] != 501)
            {
                k = igrd[i][j];
                igrd[i][j] = 501;
                if (k < 0 || k > 501)
                    k = 501;
                while (k != 501)
                {
                    mitm.quantity[k] = 0;
                    l = mitm.link[k];
                    mitm.link[k] = 501;
                    if (mitm.base_type[k] == 12 && mitm.sub_type[k] >= 4 && mitm.sub_type[k] <= 19)
                    {
                        you.unique_items[mitm.sub_type[k] + 3] = 2;     /* This means lost in the abyss, and can be found again only there. */
                    }
                    if (mitm.base_type[k] == 0 && mitm.special[k] >= 181)
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
            igrd[i][j] = 501;
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
        if (env.cloud_type[i] == 0)
            continue;
        if (env.cloud_x[i] < 35 || env.cloud_x[i] > 55 || env.cloud_y[i] < 25 || env.cloud_y[i] > 45)
        {
            env.cloud_type[i] = 0;
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
            igrd[i][j] = 501;
            mgrd[i][j] = MNG;
            env.cgrid[i][j] = CNG;
        }
    }

    for (k = 0; k < ITEMS; k++)
    {
        if (mitm.quantity[k] >= 1)
        {
            if (mitm.base_type[k] == 12)
            {
                you.unique_items[mitm.sub_type[k] + 3] = 2;
            }
            if (mitm.base_type[k] == 0 && mitm.special[k] >= 181)
            {
                if (mitm.special[k] - 181 <= 6)
                    you.unique_items[mitm.special[k] - 181] = 1;
                else
                    you.unique_items[24 + mitm.special[k] - 181 - 7] = 1;
//      you.unique_items [mitm.special [k] - 181] = 2;
            }
        }
        mitm.quantity[k] = 0;
        mitm.link[k] = 501;
    }

    for (i = 0; i < MNST; i++)
    {
        menv[i].type = -1;
    }

    for (i = 0; i < CLOUDS; i++)
    {
        env.cloud_type[i] = 0;
        env.cloud_decay[i] = 0;
        env.cloud_x[i] = 0;
        env.cloud_y[i] = 0;
    }

    env.cloud_no = 0;           // I know this doesn't do anything, of course


    you.x_pos = 45;
    you.y_pos = 35;

    generate_area(10, 10, 70, 60);

    grd[you.x_pos][you.y_pos] = 67;


}

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

#include "externs.h"

#include "dungeon.h"
#include "lev-pand.h"
#include "stuff.h"


static void generate_area(unsigned char gx1, unsigned char gy1, unsigned char gx2, unsigned char gy2);




static void generate_area(unsigned char gx1, unsigned char gy1, unsigned char gx2, unsigned char gy2)
{

    unsigned char i, j;
    unsigned char x1, x2, y1, y2;
    unsigned char items_placed = 0;
    int thickness = random2(70) + 30;
    int what_was_there;
    int thing_created;
    unsigned int rooms_done = 0;
    unsigned int rooms_to_do = 0;

    int temp_rand;    // probability determination {dlb}

    FixedVector<unsigned char, 5> replaced;


    for (i = 0; i < GXM; i++)
      for (j = 0; j < GYM; j++)
        {
            env.map[i][j] = 0;
            mgrd[i][j] = NON_MONSTER;
        }

    for (i = 0; i < 5; i++)
    {
        temp_rand = random2(10000);

        replaced[i] = ( (temp_rand > 4926) ? DNGN_ROCK_WALL :       // 50.73% chance {dlb}
                        (temp_rand > 2918) ? DNGN_STONE_WALL :      // 20.08% chance {dlb}
                        (temp_rand > 2004) ? DNGN_METAL_WALL :      //  9.14% chance {dlb}
                        (temp_rand > 1282) ? DNGN_LAVA :            //  7.22% chance {dlb}
                        (temp_rand >  616) ? DNGN_SHALLOW_WATER :   //  6.66% chance {dlb}
                        (temp_rand >   15) ? DNGN_DEEP_WATER        //  6.01% chance {dlb}
                                           : DNGN_CLOSED_DOOR );    //  0.16% chance {dlb}
    }


    if ( one_chance_in(3) )
      {
        rooms_to_do = 1 + random2(10);

        do
        {
            x1 = 10 + random2(GXM - 20);
            y1 = 10 + random2(GYM - 20);
            x2 = x1 + 1 + random2(10);
            y2 = y1 + 1 + random2(10);

            if ( one_chance_in(100) )
              goto out_of_rooms;

            for (i = x1; i < x2; i++)      // that is, [10,(GXM-1)]  {dlb}
              for (j = y1; j < y2; j++)    // that is, [10,(GYM-1)]  {dlb}
                 if ( grd[i][j] != 30 )
                   goto continued;

            for (i = x1; i < x2; i++)      // that is, [10,(GXM-1)]  {dlb}
              for (j = y1; j < y2; j++)    // that is, [10,(GYM-1)]  {dlb}
                 grd[i][j] = DNGN_FLOOR;

continued:
            rooms_done++;

            if ( rooms_done >= rooms_to_do )
              break;
        }
        while ( 1 );

    }

out_of_rooms:
    for (i = gx1; i < gx2 + 1; i++)
      for (j = gy1; j < gy2 + 1; j++)
      {
          if ( grd[i][j] == 30 && random2(100) <= thickness )
          {

              grd[i][j] = DNGN_FLOOR;

              if ( items_placed < 150 && one_chance_in(200) )
              {
                  if ( one_chance_in(500) )
                    thing_created = items(1, OBJ_MISCELLANY, MISC_RUNE_OF_ZOT, 1, 51, 250);
                  else
                    thing_created = items(1, OBJ_RANDOM, OBJ_RANDOM, 1, 51, 250);

                  what_was_there = igrd[i][j];
                  mitm.link[thing_created] = what_was_there;
                  igrd[i][j] = thing_created;
                  items_placed++;
              }
          }
      }


    for (i = gx1; i < gx2 + 1; i++)
      for (j = gy1; j < gy2 + 1; j++)
      {
          if ( grd[i][j] == 30 )    // what the h*** is 30 ??? {dlb}
            grd[i][j] = replaced[random2(5)];
          if ( one_chance_in(7500) )
            grd[i][j] = DNGN_EXIT_ABYSS;
          if ( one_chance_in(10000) )
            {
              do
              {
                  grd[i][j] = DNGN_ALTAR_ZIN + random2(12);
              }
              while (grd[i][j] == DNGN_ALTAR_YREDELEMNUL || grd[i][j] == DNGN_ALTAR_VEHUMET || grd[i][j] == DNGN_ALTAR_NEMELEX_XOBEH);
            }
      }

}




char area_shift( void )
{

    int i, j, k, l;
    unsigned char cloud_no2 = 0;


    for (i = 0; i < MAX_MONSTERS; i++)
    {
        if ( menv[i].type == -1 )
          continue;
        if ( menv[i].x < you.x_pos - 10
              || menv[i].x >= you.x_pos + 11
              || menv[i].y < you.y_pos - 10
              || menv[i].y >= you.y_pos + 11 )
        {
            menv[i].type = -1;

            for (j = 0; j < NUM_MONSTER_SLOTS; j++)
            {
                if (menv[i].inv[j] != NON_ITEM)
                {
                    mitm.quantity[menv[i].inv[j]] = 0;
                    mitm.link[menv[i].inv[j]] = NON_ITEM;
                    menv[i].inv[j] = NON_ITEM;
                }
            }

            for (j = 0; j < MAX_MONSTERS; j++)
            {
                if ( menv[j].monster_foe == i )
                  menv[j].monster_foe = MHITNOT;
            }

        }
        else
        {
          menv[i].target_x = menv[i].x; // this will make it find a new target
          menv[i].target_y = menv[i].y;
        }
    }


    for (i = 5; i < (GXM - 5); i++)
      for (j = 5; j < (GYM - 5); j++)
      {
          if ( i > you.x_pos - 10
              && i < you.x_pos + 11
              && j > you.y_pos - 10
              && j < you.y_pos + 11 )
            continue;

          grd[i][j] = 30;
          mgrd[i][j] = NON_MONSTER;

          if ( igrd[i][j] != NON_ITEM )
          {
              k = igrd[i][j];

              igrd[i][j] = NON_ITEM;

              if (k < 0 || k > NON_ITEM)
                k = NON_ITEM;

              while (k != NON_ITEM)
              {
                  mitm.quantity[k] = 0;
                  l = mitm.link[k];
                  mitm.link[k] = NON_ITEM;

                  if ( mitm.base_type[k] == OBJ_ORBS && mitm.sub_type[k] >= 4 && mitm.sub_type[k] <= 19 )
                    you.unique_items[mitm.sub_type[k] + 3] = 2;     /* This means lost in the abyss, and can be found again only there. */

                  if ( mitm.base_type[k] == OBJ_WEAPONS && mitm.special[k] >= NWPN_SINGING_SWORD )
                  {
                      if ( mitm.special[k] - NWPN_SINGING_SWORD <= 6 )
                        you.unique_items[mitm.special[k] - NWPN_SINGING_SWORD] = 1;
                      else
                        you.unique_items[24 + mitm.special[k] - NWPN_SINGING_SWORD - 7] = 1;
                  }

                  k = l;
              }
          }

      }



    for (i = you.x_pos - 10; i < you.x_pos + 11; i++)
    {
        if (i < 0 || i >= GXM)
            continue;
        for (j = you.y_pos - 10; j < you.y_pos + 11; j++)
        {
            if (j < 0 || j >= GYM)
                continue;
            grd[45 + i - you.x_pos][35 + j - you.y_pos] = grd[i][j];
            igrd[45 + i - you.x_pos][35 + j - you.y_pos] = igrd[i][j];
            igrd[i][j] = NON_ITEM;
            mgrd[45 + i - you.x_pos][35 + j - you.y_pos] = mgrd[i][j];
            if (mgrd[i][j] != NON_MONSTER)
            {
                mgrd[45 + i - you.x_pos][35 + j - you.y_pos] = mgrd[i][j];
                menv[mgrd[45 + i - you.x_pos][35 + j - you.y_pos]].x = 45 + i - you.x_pos;
                menv[mgrd[45 + i - you.x_pos][35 + j - you.y_pos]].y = 35 + j - you.y_pos;
                mgrd[i][j] = NON_MONSTER;
            }
            else
                mgrd[45 + i - you.x_pos][35 + j - you.y_pos] = NON_MONSTER;

            env.cgrid[45 + i - you.x_pos][35 + j - you.y_pos] = env.cgrid[i][j];

            if (env.cgrid[i][j] != EMPTY_CLOUD)
            {
                env.cgrid[45 + i - you.x_pos][35 + j - you.y_pos] = env.cgrid[i][j];
                env.cloud_x[env.cgrid[45 + i - you.x_pos][35 + j - you.y_pos]] = 45 + i - you.x_pos;
                env.cloud_y[env.cgrid[45 + i - you.x_pos][35 + j - you.y_pos]] = 35 + j - you.y_pos;
                env.cgrid[i][j] = EMPTY_CLOUD;
            }
            else
                env.cgrid[45 + i - you.x_pos][35 + j - you.y_pos] = EMPTY_CLOUD;
        }
    }


    for (i = 0; i < MAX_CLOUDS; i++)
    {
        if ( env.cloud_type[i] == CLOUD_NONE )
          continue;

        if ( env.cloud_x[i] < 35
              || env.cloud_x[i] > 55
              || env.cloud_y[i] < 25
              || env.cloud_y[i] > 45 )
        {
            env.cloud_type[i] = CLOUD_NONE;
            env.cloud_decay[i] = 0;
        }
        else
          cloud_no2++;
    }

    you.x_pos = 45;
    you.y_pos = 35;

    generate_area(5, 5, (GXM - 5), (GYM - 5));

    FixedVector<int, 2> passed;
    passed[0] = passed[1] = 0;

    for (int mcount = 0; mcount < 15; mcount++)
      mcount += place_monster(250, RANDOM_MONSTER, false, 1, 1, BEH_CHASING_I, MHITNOT, true, 51, passed);

    return cloud_no2;

}




// must set cloud_no to zero when calling this function
void abyss_teleport( void )
{

    int i, j, k;

    init_pandemonium();         /* changes colours */

    env.floor_colour = ( (mcolour[env.mons_alloc[9]] == BLACK) ? LIGHTGREY : mcolour[env.mons_alloc[9]] );

    env.rock_colour = ( (mcolour[env.mons_alloc[8]] == BLACK) ? LIGHTGREY : mcolour[env.mons_alloc[8]] );

    for (i = 10; i < (GXM - 9); i++)      // shouldn't it be (GXM - 10) instead of 71 ??? {dlb}
      for (j = 10; j < (GYM - 9); j++)    // shouldn't it be (GYM - 10) instead of 61 ??? {dlb}
      {
          grd[i][j] = 30;    // what the h*** is this? {dlb}
          igrd[i][j] = NON_ITEM;
          mgrd[i][j] = NON_MONSTER;
          env.cgrid[i][j] = EMPTY_CLOUD;
      }

    for (k = 0; k < MAX_ITEMS; k++)
    {
        if ( mitm.quantity[k] > 0 )
        {
            if ( mitm.base_type[k] == OBJ_ORBS )
              you.unique_items[mitm.sub_type[k] + 3] = 2;

            if ( mitm.base_type[k] == OBJ_WEAPONS && mitm.special[k] >= NWPN_SINGING_SWORD )
            {
                if ( mitm.special[k] - NWPN_SINGING_SWORD <= 6 )
                  you.unique_items[mitm.special[k] - NWPN_SINGING_SWORD] = 1;
                else
                  you.unique_items[24 + mitm.special[k] - NWPN_SINGING_SWORD - 7] = 1;
            }
        }
        mitm.quantity[k] = 0;
        mitm.link[k] = NON_ITEM;
    }

    for (i = 0; i < MAX_MONSTERS; i++)
      menv[i].type = -1;

    for (i = 0; i < MAX_CLOUDS; i++)
    {
        env.cloud_type[i] = CLOUD_NONE;
        env.cloud_decay[i] = 0;
        env.cloud_x[i] = 0;
        env.cloud_y[i] = 0;
    }

    env.cloud_no = 0;           // I know this doesn't do anything, of course

    you.x_pos = 45;
    you.y_pos = 35;

    generate_area(10, 10, (GXM - 10), (GYM - 10));

    grd[you.x_pos][you.y_pos] = DNGN_FLOOR;

}

/*
 *  file:       abyss.cc
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

#include "globals.h"
#include "externs.h"

#include "cloud.h"
#include "monplace.h"
#include "dungeon.h"
#include "items.h"
#include "mon-util.h"
#include "monstuff.h"
#include "lev-pand.h"
#include "randart.h"
#include "stuff.h"
#include "view.h"

// used for initial entry to abyss as generate_abyss assumes a pre-seeded map
void init_abyss(void)
{
    int i, j;                   // loop variables
    int temp_rand;              // probability determination {dlb}

    for (i = X_BOUND_1; i <= X_BOUND_2; i++)
    {
        for (j = Y_BOUND_1; j <= Y_BOUND_2; j++)
        {
            temp_rand = random2(4000);

            grd[i][j] = ((temp_rand > 999) ? DNGN_FLOOR :       // 75.0%
                         (temp_rand > 400) ? DNGN_ROCK_WALL :   // 15.0%
                         (temp_rand > 100) ? DNGN_STONE_WALL :  //  7.5%
                         (temp_rand >   0) ? DNGN_METAL_WALL    //  2.5%
                                           : DNGN_CLOSED_DOOR); // 1 in 4000
        }
    }

    grd[ X_ABYSS_CENTER ][ Y_ABYSS_CENTER ] = DNGN_FLOOR;

    for (i = 0; i < 40; i++)
    {
        mons_place( RANDOM_MONSTER, BEH_HOSTILE, MHITNOT, false, 1, 1,
                    LEVEL_ABYSS, PROX_ANYWHERE );
    }
}                               // end init_abyss()

static bool check_unseen_area( int x1, int y1, int x2, int y2 )
{
    int i, j;

    for (i = x1; i <= x2; i++)
    {
        for (j = y1; j <= y2; j++)
        {
            if (grd[i][j] != DNGN_UNSEEN)
                return (false);
        }
    }

    return (true);
}

static void generate_abyss()
{
    int i, j, k;
    int x1, x2, y1, y2;
    int items_placed = 0;
    int thickness = random2(70) + 30;
    int thing_created;
    int temp_rand;              // probability determination {dlb}

    FixedVector< unsigned char, 5 > replaced;

    // nuke map
    clear_envmap();

    // generate level composition vector
    for (i = 0; i < 5; i++)
    {
        temp_rand = random2(10000);

        replaced[i] = ((temp_rand < 5000) ? DNGN_ROCK_WALL :     // 50%
                       (temp_rand < 7000) ? DNGN_STONE_WALL :    // 20%
                       (temp_rand < 8000) ? DNGN_METAL_WALL :    // 10%
                       (temp_rand < 8700) ? DNGN_LAVA :          //  7%
                       (temp_rand < 9360) ? DNGN_SHALLOW_WATER : //  6.6%
                       (temp_rand < 9980) ? DNGN_DEEP_WATER      //  6.2%
                                          : DNGN_CLOSED_DOOR);   //  0.2%
    }

    if (one_chance_in(3))
    {
        const int rooms = roll_dice(1, 10);

        for (k = 0; k < rooms; k++)
        {
            x1 = 0;
            y1 = 0;
            x2 = roll_dice(1,10);
            y2 = roll_dice(1,10);

            random_place_rectangle( x1, y1, x2, y2 );

            if (check_unseen_area( x1, y1, x2, y2 ))
            {
                // make room if it's not in LoS
                for (i = x1; i <= x2; i++)
                {
                    for (j = y1; j <= y2; j++)
                    {
                        grd[i][j] = DNGN_FLOOR;
                    }
                }
            }
        }
    }

    for (i = X_BOUND_1; i <= X_BOUND_2; i++)
    {
        for (j = Y_BOUND_1; j <= Y_BOUND_2; j++)
        {
            if (grd[i][j] != DNGN_UNSEEN)
                continue;

            if (random2(100) > thickness)
            {
                grd[i][j] = replaced[ random2(5) ];
            }
            else if (grid_distance(i, j, you.x_pos, you.y_pos) > 12
                    && one_chance_in(7500))
            {
                grd[i][j] = DNGN_EXIT_ABYSS;
            }
            else if (one_chance_in(10000))
            {
                do
                {
                    grd[i][j] = DNGN_ALTAR_ZIN + random2(12);
                }
                while (grd[i][j] == DNGN_ALTAR_YREDELEMNUL
                       || grd[i][j] == DNGN_ALTAR_VEHUMET
                       || grd[i][j] == DNGN_ALTAR_NEMELEX_XOBEH);
            }
            else
            {
                grd[i][j] = DNGN_FLOOR;

                if (items_placed < 150 && one_chance_in(400))
                {
                    if (one_chance_in(500))
                    {
                        thing_created = make_item( 1, OBJ_MISCELLANY,
                                                   MISC_RUNE_OF_ZOT, true,
                                                   ABYSS_DEPTH, RUNE_ABYSSAL );
                    }
                    else
                    {
                        thing_created = make_item( 1, OBJ_RANDOM, OBJ_RANDOM,
                                                   true, ABYSS_DEPTH, 250 );
                    }

                    move_item_to_grid( &thing_created, i, j );

                    if (thing_created != NON_ITEM)
                        items_placed++;
                }
            }
        }
    }
}

static void delete_monster( int id )
{
    ASSERT( menv[id].type != -1 );

    // invalidate monster and remove from grid
    menv[id].type = -1;
    mgrd[ menv[id].x ][ menv[id].y ] = NON_MONSTER;

    // destroy the monster's equipment
    for (int i = 0; i < NUM_MONSTER_SLOTS; i++)
    {
        if (menv[id].inv[i] != NON_ITEM)
        {
            destroy_item( menv[id].inv[i] );
            menv[id].inv[i] = NON_ITEM;
        }
    }
}

void area_shift(void)
/*******************/
{
    // Since overlap isn't handled and is bad, we assert against it now.
    ASSERT( X_ABYSS_1 + ABYSS_SHIFT_RADIUS < X_ABYSS_CENTER - ABYSS_SHIFT_RADIUS );
    ASSERT( X_ABYSS_2 - ABYSS_SHIFT_RADIUS > X_ABYSS_CENTER + ABYSS_SHIFT_RADIUS );
    ASSERT( Y_ABYSS_1 + ABYSS_SHIFT_RADIUS < Y_ABYSS_CENTER - ABYSS_SHIFT_RADIUS );
    ASSERT( Y_ABYSS_2 - ABYSS_SHIFT_RADIUS > Y_ABYSS_CENTER + ABYSS_SHIFT_RADIUS );
    ASSERT( ABYSS_SHIFT_RADIUS >= MAX_VISION_RANGE );

#if DEBUG_DIAGNOSTICS
    mpr( MSGCH_DIAGNOSTICS, "Shifting..." );
#endif

    int i, j;

    // erase non-nearby terrain
    for (i = X_BOUND_1; i <= X_BOUND_2; i++)
    {
        for (j = Y_BOUND_1; j <= Y_BOUND_2; j++)
        {
            // don't modify terrain by player
            if (grid_distance(i, j, you.x_pos, you.y_pos) <= ABYSS_SHIFT_RADIUS)
                continue;

            // nuke terrain otherwise
            grd[i][j] = DNGN_UNSEEN;

            // nuke items
            destroy_item_stack( i, j );

            // nuke cloud
            if (env.cgrid[i][j] != EMPTY_CLOUD)
                delete_cloud( env.cgrid[i][j] );

            // nuke monster
            if (mgrd[i][j] != NON_MONSTER)
                delete_monster( mgrd[i][j] );
        }
    }

    // shift nearby monsters, items, clouds, and terrain to center area
    for (i = -ABYSS_SHIFT_RADIUS; i <= ABYSS_SHIFT_RADIUS; i++)
    {
        for (j = -ABYSS_SHIFT_RADIUS; j <= ABYSS_SHIFT_RADIUS; j++)
        {
            const int x_old = you.x_pos + i;
            const int y_old = you.y_pos + j;
            const int x_new = X_ABYSS_CENTER + i;
            const int y_new = Y_ABYSS_CENTER + j;

            if (!in_bounds( x_old, y_old ))
                continue;

            // move terrain
            grd[x_new][y_new] = grd[x_old][y_old];

            // move item
            move_item_stack_to_grid( x_old, y_old, x_new, y_new );

            // move monster
            if (mgrd[x_old][y_old] != NON_MONSTER)
                move_monster_to_grid( mgrd[x_old][y_old], x_new, y_new );

            // move cloud
            if (env.cgrid[x_old][y_old] != EMPTY_CLOUD)
                move_cloud( env.cgrid[x_old][y_old], x_new, y_new );
        }
    }

    // center the player and generate a new surrounding abyss
    you.x_pos = X_ABYSS_CENTER;
    you.y_pos = Y_ABYSS_CENTER;

    generate_abyss();

    for (i = 0; i < 20; i++)
    {
        mons_place( RANDOM_MONSTER, BEH_HOSTILE, MHITNOT, false, 1, 1,
                    LEVEL_ABYSS, PROX_AWAY_FROM_PLAYER );
    }
}


void abyss_teleport( bool new_area )
/**********************************/
{
#if DEBUG_DIAGNOSTICS
    mpr( MSGCH_DIAGNOSTICS, "Abyss teleport" );
#endif

    int x, y, i, j, k;

    if (!new_area)
    {
        // try to find a good spot within the shift zone:
        for (i = 0; i < 100; i++)
        {
            x = X_ABYSS_1 + random2( X_ABYSS_WIDTH );
            y = Y_ABYSS_1 + random2( Y_ABYSS_WIDTH );

            if ((grd[x][y] == DNGN_FLOOR
                    || grd[x][y] == DNGN_SHALLOW_WATER)
                && mgrd[x][y] == NON_MONSTER
                && env.cgrid[x][y] == EMPTY_CLOUD)
            {
                break;
            }
        }

        if (i < 100)
        {
            you.x_pos = x;
            you.y_pos = y;
            return;
        }
    }

    // teleport to a new area of the abyss:

    init_pandemonium();                         /* changes colours */

    env.floor_colour = mons_class_colour( env.mons_alloc[9] );
    env.rock_colour  = mons_class_colour( env.mons_alloc[8] );

    // Orbs and fixed artefacts are marked as "lost in the abyss"
    for (k = 0; k < MAX_ITEMS; k++)
    {
        if (is_valid_item( mitm[k] ))
        {
            if (mitm[k].base_type == OBJ_ORBS)
            {
                set_unique_item_status( OBJ_ORBS, mitm[k].sub_type,
                                        UNIQ_LOST_IN_ABYSS );
            }
            else if (is_fixed_artefact( mitm[k] ))
            {
                set_unique_item_status( OBJ_WEAPONS, mitm[k].special,
                                        UNIQ_LOST_IN_ABYSS );
            }

            destroy_item( k );
        }
    }

    for (i = 0; i < MAX_MONSTERS; i++)
    {
        if (menv[i].type != -1)
            delete_monster(i);
    }

    for (i = 0; i < MAX_CLOUDS; i++)
        delete_cloud(i);

    for (i = X_BOUND_1; i <= X_BOUND_2; i++)
    {
        for (j = Y_BOUND_1; j <= Y_BOUND_2; j++)
        {
            grd[i][j] = DNGN_UNSEEN;    // so generate_abyss will pick it up
            igrd[i][j] = NON_ITEM;
            mgrd[i][j] = NON_MONSTER;
            env.cgrid[i][j] = EMPTY_CLOUD;
        }
    }

    ASSERT( env.cloud_no == 0 );

    you.x_pos = X_ABYSS_CENTER;
    you.y_pos = Y_ABYSS_CENTER;

    generate_abyss();

    grd[you.x_pos][you.y_pos] = DNGN_FLOOR;

    for (i = 0; i < 40; i++)
    {
        mons_place( RANDOM_MONSTER, BEH_HOSTILE, MHITNOT, false, 1, 1,
                    LEVEL_ABYSS, (one_chance_in(10) ? PROX_CLOSE_TO_PLAYER
                                                    : PROX_ANYWHERE) );
    }
}

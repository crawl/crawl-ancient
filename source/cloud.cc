/*
 *  File:       cloud.cc
 *  Summary:    Functions related to clouds.
 *  Written by: Brent Ross
 *
 *  Creating a cloud module so all the cloud stuff can be isolated.
 *
 *  Change History (most recent first):
 *
 *   <1>    Oct 1/2001   BWR    Created
 */

#include "AppHdr.h"
#include "globals.h"
#include "externs.h"

#include "cloud.h"
#include "stuff.h"
#include "ouch.h"
#include "misc.h"
#include "monstuff.h"
#include "mon-util.h"

void delete_cloud( int cloud )
{
    if (env.cloud[ cloud ].type != CLOUD_NONE)
    {
        const int cloud_x = env.cloud[ cloud ].x;
        const int cloud_y = env.cloud[ cloud ].y;

        env.cloud[ cloud ].type = CLOUD_NONE;
        env.cloud[ cloud ].decay = 0;
        env.cloud[ cloud ].x = 0;
        env.cloud[ cloud ].y = 0;
        env.cgrid[ cloud_x ][ cloud_y ] = EMPTY_CLOUD;
        env.cloud_no--;
    }
}

// The current use of this function is for shifting in the abyss, so
// that clouds get moved along with the rest of the map.
void move_cloud( int cloud, int new_x, int new_y )
{
    if (cloud != EMPTY_CLOUD)
    {
        const int old_x = env.cloud[ cloud ].x;
        const int old_y = env.cloud[ cloud ].y;

        env.cgrid[ new_x ][ new_y ] = cloud;
        env.cloud[ cloud ].x = new_x;
        env.cloud[ cloud ].y = new_y;
        env.cgrid[ old_x ][ old_y ] = EMPTY_CLOUD;
    }
}

bool is_cloud( int x, int y )
{
    return (env.cgrid[x][y] != EMPTY_CLOUD);
}

int get_cloud_type( int x, int y )
{
    if (env.cgrid[x][y] == EMPTY_CLOUD)
        return (CLOUD_NONE);

    return (env.cloud[ env.cgrid[x][y] ].type);
}

static bool is_weak_cloud( int cloud_id )
{
    return ((env.cloud[ cloud_id ].type >= CLOUD_GREY_SMOKE
                && env.cloud[ cloud_id ].type <= CLOUD_STEAM)
            || env.cloud[ cloud_id ].type == CLOUD_STINK
            || env.cloud[ cloud_id ].type == CLOUD_BLACK_SMOKE
            || env.cloud[ cloud_id ].decay <= 20);     // soon gone
}

//  Places a cloud with the given stats. May delete old clouds to make way
//  if there are too many (MAX_CLOUDS == 30) on level. Will overwrite an old
//  cloud under some circumstances.
void place_cloud( int cl_type, int ctarget_x, int ctarget_y, int cl_range,
                  bool affect_grid )
{
    int cl_new = -1;

    const int target_cgrid = env.cgrid[ctarget_x][ctarget_y];

    if (target_cgrid != EMPTY_CLOUD)
    {
        // check to see if we can replace the current cloud (or abort)
        if (!is_weak_cloud( target_cgrid ))
            return;  // too strong!  abort!
        else
        {
            // delete it and take it's slot
            delete_cloud( target_cgrid );
            cl_new = target_cgrid;
        }
    }
    else if (env.cloud_no < MAX_CLOUDS)
    {
        // find slot for cloud
        for (int ci = 0; ci < MAX_CLOUDS; ci++)
        {
            if (env.cloud[ci].type == CLOUD_NONE)
            {
                cl_new = ci;
                break;
            }
        }
    }
    else // too many clouds
    {
        // default to random in case there's no low quality clouds
        int cl_del = random2( MAX_CLOUDS );

        for (int ci = 0; ci < MAX_CLOUDS; ci++)
        {
            if (is_weak_cloud( ci ))
            {
                cl_del = ci;
                break;
            }
        }

        // delete it and take it's slot
        delete_cloud( cl_del );
        cl_new = cl_del;
    }

    // create new cloud
    if (cl_new != -1)
    {
        env.cloud[ cl_new ].type = cl_type;
        env.cloud[ cl_new ].decay = 10 * cl_range;
        env.cloud[ cl_new ].x = ctarget_x;
        env.cloud[ cl_new ].y = ctarget_y;
        env.cgrid[ ctarget_x ][ ctarget_y ] = cl_new;
        env.cloud_no++;

        if (affect_grid)
        {
            if (you.x_pos == ctarget_x && you.y_pos == ctarget_y)
                in_a_cloud();

            const int mid = mgrd[ctarget_x][ctarget_y];
            if (mid != NON_MONSTER && !mons_is_submerged( &menv[mid] ))
                mons_in_cloud( &menv[mid] );

            switch (cl_type)
            {
            case CLOUD_FIRE:
            case CLOUD_FIRE_MON:
                expose_floor_to_element( BEAM_FIRE, ctarget_x, ctarget_y );
                break;

            case CLOUD_COLD:
            case CLOUD_COLD_MON:
                expose_floor_to_element( BEAM_COLD, ctarget_x, ctarget_y );
                break;

            default:
                break;
            }
        }
    }
}                               // end place_cloud();

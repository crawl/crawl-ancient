/*
 *  File:       travel.cc
 *  Summary:    Travel stuff
 *  Written by: Darshan Shaligram
 *
 *  Known issues:
 *   Hardcoded dungeon features all over the place - this thing is a devil to
 *   refactor.
 */

#include "AppHdr.h"
#include "files.h"
#include "FixAry.h"
#include "misc.h"
#include "mon-util.h"
#include "player.h"
#include "stash.h"
#include "stuff.h"
#include "travel.h"
#include "view.h"
#include <stdarg.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#ifdef DOS
#include <dos.h>
#endif

#define TC_MAJOR_VERSION (static_cast<unsigned char>(4))
#define TC_MINOR_VERSION (static_cast<unsigned char>(3))

static char trans_travel_dest[30];
TravelCache travel_cache;

// Tracks the distance between the target location on the target level and the
// stairs on the level.
static std::vector<stair_info> curr_stairs;

// This is where we last tried to take a stair during interlevel travel.
// Note that last_stair.depth should be set to -1 before initiating interlevel
// travel.
static level_id last_stair;

// Where travel wants to get to.
static level_pos travel_target;

// Array of points on the map, each value being the distance the character
// would have to travel to get there. Negative distances imply that the point
// is a) a trap or hostile terrain or b) only reachable by crossing a trap or
// hostile terrain.
short point_distance[GXM][GYM];

static FixedArray< unsigned char, GXM + 1, GYM + 1>  mapshadow;

// XXX: Reduce this reduplication.
// Clockwise, around the compass from north (same order as enum RUN_DIR)
// Copied from acr.cc
static const struct coord_def Compass[8] =
{
    {  0, -1 }, {  1, -1 }, {  1,  0 }, {  1,  1 },
    {  0,  1 }, { -1,  1 }, { -1,  0 }, { -1, -1 },
};

#define TRAVERSABLE  1
#define IMPASSABLE   0
#define FORBIDDEN   -1

// Map of terrain types that are traversable.
static signed char traversable_terrain[256];

static int trans_negotiate_stairs();
static int find_transtravel_square(const level_pos &pos, bool verbose = true);

inline bool is_player_mapped( int x, int y )
{
    return is_terrain_seen(x, y);
}

// Returns true if the dungeon feature supplied is an altar.
inline bool is_altar( int grid )
{
    return (grid >= DNGN_ALTAR_ZIN && grid <= DNGN_ALTAR_ELYVILON);
}

inline bool is_altar( const coord_def &c )
{
    return is_altar( grd[c.x][c.y] );
}

inline bool is_player_altar( int grid )
{
    // An ugly hack, but that's what religion.cc does.
    return (you.religion != GOD_NO_GOD
            && grid == DNGN_ALTAR_ZIN - 1 + you.religion);
}

inline bool is_player_altar( const coord_def &c )
{
    return (is_player_altar( grd[c.x][c.y] ));
}

// Copies player-mapped status of each dungeon square to a shadow map.
static void shadow_map()
{
    for (int y = 1; y <= GYM; ++y)
    {
        for (int x = 1; x <= GXM; ++x)
        {
            mapshadow[x][y] = is_player_mapped(x, y);
            if (x == you.x_pos && y == you.y_pos)
                mapshadow[x][y] = 1;
        }
    }
}

// Returns true if there is a known trap at (x,y). Returns false for non-trap
// squares as also for undiscovered traps.
//
static bool is_trap( int x, int y )
{
    const unsigned char grid = grd[x][y];

    return (grid == DNGN_TRAP_MECHANICAL || grid == DNGN_TRAP_MAGICAL
              || grid == DNGN_TRAP_III);
}

/*
 * Returns true if the character can cross this dungeon feature.
 */
inline bool is_traversable(int grid)
{
    return (traversable_terrain[grid] == TRAVERSABLE);
}

/*
 * Returns true if the square at (x,y) is a dungeon feature the character
 * can't (under normal circumstances) safely cross.
 *
 * Note: is_reseedable can return true for dungeon features that is_traversable
 *       also returns true for. This is okay, because is_traversable always
 *       takes precedence over is_reseedable. is_reseedable is used only to
 *       decide which squares to reseed from when flood-filling outwards to
 *       colour the level map. It does not affect pathing of actual
 *       travel/explore.
 */
static bool is_reseedable( int x, int y )
{
    const int grid = grd[x][y];

    // Not too well named, but "destroys items" is used for unsafe squares
    // for the player as well.
    return (grid == DNGN_DEEP_WATER || grid == DNGN_SHALLOW_WATER
            || grid == DNGN_LAVA || is_trap(x, y));
}

/*
 * Returns true if the square at (x,y) is okay to travel over. If ignore_hostile
 * is true, returns true even for dungeon features the character can normally
 * not cross safely (deep water, lava, traps).
 */
static bool is_travel_ok( int x, int y, bool ignore_hostile )
{
    if (!is_terrain_known(x, y))
        return (false);

    const int grid = grd[x][y];
    // XXX: Special-case secret doors?

    const int mon = mgrd[x][y];
    if (mon != NON_MONSTER)
    {
        // Kludge warning: navigating around zero-exp beasties uses knowledge
        //                 that the player may not have (the player may not
        //                 know that there's a plant at any given (x,y), but we
        //                 know, because we're looking directly at the grid).
        //                 Arguably the utility of this feature is greater than
        //                 the information we're giving the player for free.
        // Navigate around plants and fungi. Yet another tasty hack.
        if (player_monster_visible( &menv[mon] )
            && mons_appears_harmless( menv[mon].type ))
        {
            extern short point_distance[GXM][GYM];

            // We have to set the point_distance array if the level map is
            // to be properly coloured. The caller isn't going to do it because
            // we say this square is inaccessible, so in a horrible hack, we
            // do it ourselves. Ecch.
            point_distance[x][y] = (ignore_hostile) ? -42 : 42;
            return (false);
        }
    }

    // If 'ignore_hostile' is true, we're ignoring hazards that can be
    // navigated over if the player is willing to take damage, or levitate.
    if (ignore_hostile && is_reseedable(x, y))
        return (true);

    return (is_traversable(grid));
}

// Returns true if the location at (x,y) is monster-free and contains no clouds.
static bool is_safe(int x, int y)
{
    const int mon = mgrd[x][y];
    if (mon != NON_MONSTER)
    {
        // If this is an invisible critter, say we're safe to get here, but
        // turn off travel - the result should be that the player bashes into
        // the monster and stops travelling right there. Same treatment applies
        // to mimics.
        if (!player_monster_visible(&menv[mon])
            || mons_is_mimic( menv[mon].type ))
        {
            you.running = 0;
            return (true);
        }

        // Stop before wasting energy on plants and fungi. Note that this
        // new definition includes oklob plants, which are far from harmless.
        if (mons_appears_harmless( menv[mon].type ))
            return (false);

        // If this is any *other* monster, it'll be visible and
        // a) Friendly, in which case we'll displace it, no problem.
        // b) Unfriendly, in which case we're in deep trouble, since travel
        //    should have been aborted already by the checks in view.cc.
    }

    const char cloud = env.cgrid[x][y];
    if (cloud == EMPTY_CLOUD)
        return (true);

    // We can also safely run through smoke.
    const char cloud_type = env.cloud[ cloud ].type;

    return (cloud_type == CLOUD_GREY_SMOKE
            || cloud_type == CLOUD_GREY_SMOKE_MON
            || cloud_type == CLOUD_BLUE_SMOKE
            || cloud_type == CLOUD_BLUE_SMOKE_MON
            || cloud_type == CLOUD_PURP_SMOKE
            || cloud_type == CLOUD_PURP_SMOKE_MON
            || cloud_type == CLOUD_BLACK_SMOKE
            || cloud_type == CLOUD_BLACK_SMOKE_MON);
}

static bool player_is_permalevitating()
{
    return (you.levitation > 1
            && ((you.species == SP_KENKU && you.xp_level >= 15)
                 || player_equip_ego_type( EQ_BOOTS, SPARM_LEVITATION )));
}

static void set_pass_feature(unsigned char grid, signed char pass)
{
    if (traversable_terrain[static_cast<unsigned int>(grid)] != FORBIDDEN)
        traversable_terrain[static_cast<unsigned int>(grid)] = pass;
}

/*
 * Sets traversable terrain based on the character's role and whether or not he
 * has permanent levitation
 */
static void init_terrain_check()
{
    // Merfolk get deep water.
    const signed char water = (you.species == SP_MERFOLK) ? TRAVERSABLE
                                                          : IMPASSABLE;

    // If the player has overridden deep water already, we'll respect that.
    set_pass_feature( DNGN_DEEP_WATER, water );

    // Permanently levitating players can cross most hostile terrain.
    const signed char trav = (player_is_permalevitating()) ? TRAVERSABLE
                                                           : IMPASSABLE;

    if (you.species != SP_MERFOLK)
        set_pass_feature( DNGN_DEEP_WATER, trav );

    set_pass_feature( DNGN_LAVA, trav );
    set_pass_feature( DNGN_TRAP_MECHANICAL, trav );
}

/*
 * Sets up travel-related stuff.
 */
void initialise_travel()
{
    // Need a better way to do this. :-(
    traversable_terrain[DNGN_FLOOR] =
    traversable_terrain[DNGN_ENTER_HELL] =
    traversable_terrain[DNGN_OPEN_DOOR] =
    traversable_terrain[DNGN_UNDISCOVERED_TRAP] =
    traversable_terrain[DNGN_ENTER_SHOP] =
    traversable_terrain[DNGN_ENTER_LABYRINTH] =
    traversable_terrain[DNGN_STONE_STAIRS_DOWN_I] =
    traversable_terrain[DNGN_STONE_STAIRS_DOWN_II] =
    traversable_terrain[DNGN_STONE_STAIRS_DOWN_III] =
    traversable_terrain[DNGN_ROCK_STAIRS_DOWN] =
    traversable_terrain[DNGN_STONE_STAIRS_UP_I] =
    traversable_terrain[DNGN_STONE_STAIRS_UP_II] =
    traversable_terrain[DNGN_STONE_STAIRS_UP_III] =
    traversable_terrain[DNGN_ROCK_STAIRS_UP] =
    traversable_terrain[DNGN_ENTER_DIS] =
    traversable_terrain[DNGN_ENTER_GEHENNA] =
    traversable_terrain[DNGN_ENTER_COCYTUS] =
    traversable_terrain[DNGN_ENTER_TARTARUS] =
    traversable_terrain[DNGN_ENTER_ABYSS] =
    traversable_terrain[DNGN_EXIT_ABYSS] =
    traversable_terrain[DNGN_STONE_ARCH] =
    traversable_terrain[DNGN_ENTER_PANDEMONIUM] =
    traversable_terrain[DNGN_EXIT_PANDEMONIUM] =
    traversable_terrain[DNGN_TRANSIT_PANDEMONIUM] =
    traversable_terrain[DNGN_ENTER_ORCISH_MINES] =
    traversable_terrain[DNGN_ENTER_HIVE] =
    traversable_terrain[DNGN_ENTER_LAIR] =
    traversable_terrain[DNGN_ENTER_SLIME_PITS] =
    traversable_terrain[DNGN_ENTER_VAULTS] =
    traversable_terrain[DNGN_ENTER_CRYPT] =
    traversable_terrain[DNGN_ENTER_HALL_OF_BLADES] =
    traversable_terrain[DNGN_ENTER_ZOT] =
    traversable_terrain[DNGN_ENTER_TEMPLE] =
    traversable_terrain[DNGN_ENTER_SNAKE_PIT] =
    traversable_terrain[DNGN_ENTER_ELVEN_HALLS] =
    traversable_terrain[DNGN_ENTER_TOMB] =
    traversable_terrain[DNGN_ENTER_SWAMP] =
    traversable_terrain[DNGN_RETURN_FROM_ORCISH_MINES] =
    traversable_terrain[DNGN_RETURN_FROM_HIVE] =
    traversable_terrain[DNGN_RETURN_FROM_LAIR] =
    traversable_terrain[DNGN_RETURN_FROM_SLIME_PITS] =
    traversable_terrain[DNGN_RETURN_FROM_VAULTS] =
    traversable_terrain[DNGN_RETURN_FROM_CRYPT] =
    traversable_terrain[DNGN_RETURN_FROM_HALL_OF_BLADES] =
    traversable_terrain[DNGN_RETURN_FROM_ZOT] =
    traversable_terrain[DNGN_RETURN_FROM_TEMPLE] =
    traversable_terrain[DNGN_RETURN_FROM_SNAKE_PIT] =
    traversable_terrain[DNGN_RETURN_FROM_ELVEN_HALLS] =
    traversable_terrain[DNGN_RETURN_FROM_TOMB] =
    traversable_terrain[DNGN_RETURN_FROM_SWAMP] =
    traversable_terrain[DNGN_ALTAR_ZIN] =
    traversable_terrain[DNGN_ALTAR_SHINING_ONE] =
    traversable_terrain[DNGN_ALTAR_KIKUBAAQUDGHA] =
    traversable_terrain[DNGN_ALTAR_YREDELEMNUL] =
    traversable_terrain[DNGN_ALTAR_XOM] =
    traversable_terrain[DNGN_ALTAR_VEHUMET] =
    traversable_terrain[DNGN_ALTAR_OKAWARU] =
    traversable_terrain[DNGN_ALTAR_MAKHLEB] =
    traversable_terrain[DNGN_ALTAR_SIF_MUNA] =
    traversable_terrain[DNGN_ALTAR_TROG] =
    traversable_terrain[DNGN_ALTAR_NEMELEX_XOBEH] =
    traversable_terrain[DNGN_ALTAR_ELYVILON] =
    traversable_terrain[DNGN_BLUE_FOUNTAIN] =
    traversable_terrain[DNGN_DRY_FOUNTAIN_I] =
    traversable_terrain[DNGN_SPARKLING_FOUNTAIN] =
    traversable_terrain[DNGN_DRY_FOUNTAIN_II] =
    traversable_terrain[DNGN_DRY_FOUNTAIN_III] =
    traversable_terrain[DNGN_DRY_FOUNTAIN_IV] =
    traversable_terrain[DNGN_DRY_FOUNTAIN_V] =
    traversable_terrain[DNGN_DRY_FOUNTAIN_VI] =
    traversable_terrain[DNGN_DRY_FOUNTAIN_VII] =
    traversable_terrain[DNGN_DRY_FOUNTAIN_VIII] =
    traversable_terrain[DNGN_PERMADRY_FOUNTAIN] =
    traversable_terrain[DNGN_CLOSED_DOOR] =
    traversable_terrain[DNGN_SHALLOW_WATER] =
                            TRAVERSABLE;
}

/*
 * Given a dungeon feature description, returns the feature number. This is a
 * crude hack and currently recognises only (deep/shallow) water.
 *
 * Returns -1 if the feature named is not recognised, else returns the feature
 * number (guaranteed to be 0-255).
 */
int get_feature_type( const std::string &feature )
{
    if (feature.find("deep water") != std::string::npos)
        return (DNGN_DEEP_WATER);

    if (feature.find("shallow water") != std::string::npos)
        return (DNGN_SHALLOW_WATER);

    return (-1);
}

/*
 * Given a feature description, prevents travel to locations of that feature
 * type.
 */
void prevent_travel_to( const std::string &feature )
{
    const int feature_type = get_feature_type(feature);

    if (feature_type != -1)
        traversable_terrain[feature_type] = FORBIDDEN;
}

/*
 * Returns true if the given dungeon feature can be considered a stair.
 */
inline bool is_stair(unsigned char gridc)
{
    switch (static_cast<short>(gridc))
    {
    case DNGN_ENTER_HELL:
    case DNGN_STONE_STAIRS_DOWN_I:
    case DNGN_STONE_STAIRS_DOWN_II:
    case DNGN_STONE_STAIRS_DOWN_III:
    case DNGN_ROCK_STAIRS_DOWN:
    case DNGN_STONE_STAIRS_UP_I:
    case DNGN_STONE_STAIRS_UP_II:
    case DNGN_STONE_STAIRS_UP_III:
    case DNGN_ROCK_STAIRS_UP:
    case DNGN_ENTER_DIS:
    case DNGN_ENTER_GEHENNA:
    case DNGN_ENTER_COCYTUS:
    case DNGN_ENTER_TARTARUS:
    case DNGN_ENTER_ORCISH_MINES:
    case DNGN_ENTER_HIVE:
    case DNGN_ENTER_LAIR:
    case DNGN_ENTER_SLIME_PITS:
    case DNGN_ENTER_VAULTS:
    case DNGN_ENTER_CRYPT:
    case DNGN_ENTER_HALL_OF_BLADES:
    case DNGN_ENTER_ZOT:
    case DNGN_ENTER_TEMPLE:
    case DNGN_ENTER_SNAKE_PIT:
    case DNGN_ENTER_ELVEN_HALLS:
    case DNGN_ENTER_TOMB:
    case DNGN_ENTER_SWAMP:
    case DNGN_RETURN_FROM_ORCISH_MINES:
    case DNGN_RETURN_FROM_HIVE:
    case DNGN_RETURN_FROM_LAIR:
    case DNGN_RETURN_FROM_SLIME_PITS:
    case DNGN_RETURN_FROM_VAULTS:
    case DNGN_RETURN_FROM_CRYPT:
    case DNGN_RETURN_FROM_HALL_OF_BLADES:
    case DNGN_RETURN_FROM_ZOT:
    case DNGN_RETURN_FROM_TEMPLE:
    case DNGN_RETURN_FROM_SNAKE_PIT:
    case DNGN_RETURN_FROM_ELVEN_HALLS:
    case DNGN_RETURN_FROM_TOMB:
    case DNGN_RETURN_FROM_SWAMP:
        return (true);
    default:
        break;
    }

    return (false);
}

/*
 * Given a square that has just become visible during explore, returns true
 * if the player might consider the square worth stopping explore for.
 */
static bool is_interesting_square(int x, int y)
{
    if (igrd[x][y] != NON_ITEM)
        return (true);

    // Shops and altars outside the Ecumenical Temple are interesting.
    const unsigned char grid = grd[x][y];
    return (grid == DNGN_ENTER_SHOP
            || is_stair(grid)
            || (is_altar(grid)
                && you.where_are_you != BRANCH_ECUMENICAL_TEMPLE));
}

/*
 * Top-level travel control (called from input() in acr.cc).
 *
 * travel() is responsible for making the individual moves that constitute
 * (interlevel) travel and explore and deciding when travel and explore
 * end.
 *
 * Don't call travel() if you.running >= 0.
 */
void travel( int *keyin, char *move_x, char *move_y )
{
    *keyin = 128;

    // Abort travel/explore if you're confused or a key was pressed.
    if (kbhit() || you.conf)
    {
        you.running = 0;
        *keyin      = 0;

        if (Options.travel_delay == -1)
            redraw_screen();
        return;
    }

    if (Options.explore_item_stop && you.running == RMODE_EXPLORE)
    {
        // Scan through the shadow map, compare it with the actual map, and if
        // there are any squares of the shadow map that have just been
        // discovered and contain an item, or have an interesting dungeon
        // feature, stop exploring.
        for (int y = 1; y <= GYM; ++y)
        {
            for (int x = 1; x <= GXM; ++x)
            {
                if (!mapshadow[x][y]
                    && is_player_mapped(x, y)
                    && is_interesting_square(x, y))
                {
                    you.running = 0;
                    y = GYM;
                    break;
                }
            }
        }
        shadow_map();
    }

    if (you.running == RMODE_EXPLORE)
    {
        // Exploring
        you.run_x = 0;
        find_travel_pos(you.x_pos, you.y_pos, NULL, NULL);

        if (!you.run_x)         // No place to go. :-(
            you.running = 0;
    }

    if (you.running == RMODE_INTERLEVEL && !you.run_x)
    {
        // Interlevel travel. Since you.run_x is zero, we've either just
        // initiated travel, or we've just climbed or descended a staircase,
        // and we need to figure out where to travel to next.
        //
        // No place to go. :-(
        if (!find_transtravel_square(travel_target) || !you.run_x)
            you.running = 0;
    }

    if (you.running < 0)
    {
        // Remember what run-mode we were in so that we can resume explore/
        // interlevel travel correctly.
        const int runmode = you.running;

        // Get the next step to make. If the travel command can't find a route,
        // we turn off travel (find_travel_pos does that automatically).
        find_travel_pos(you.x_pos, you.y_pos, move_x, move_y);

        if (!*move_x && !*move_y)
        {
            // If we've reached the square we were traveling towards, travel
            // should stop if this is simple travel. If we're exploring, we
            // should continue doing so (explore has its own end condition
            // upstairs); if we're traveling between levels and we've reached
            // our travel target, we're on a staircase and should take it.
            if (you.x_pos == you.run_x && you.y_pos == you.run_y)
            {
                if (runmode == RMODE_EXPLORE)
                    you.running = RMODE_EXPLORE;       // Turn explore back on
                else if (runmode == RMODE_INTERLEVEL
                        && (travel_target.pos.x != you.x_pos
                            || travel_target.pos.y != you.y_pos
                            || travel_target.id != level_id::get_current_level_id()))
                {
                    // For interlevel travel, we'll want to take the stairs unless
                    // the interlevel travel specified a destination square and
                    // we've reached that destination square.
                    if (last_stair.depth != -1
                        && last_stair == level_id::get_current_level_id())
                    {
                        // We're trying to take the same stairs again. Baaad.
                        you.running = 0;
                        return;
                    }

                    you.running = RMODE_INTERLEVEL;
                    *keyin = trans_negotiate_stairs();

                    // If, for some reason, we fail to use the stairs, we
                    // need to make sure we don't go into an infinite loop
                    // trying to take it again and again. We'll check
                    // last_stair before attempting to take stairs again.
                    last_stair = level_id::get_current_level_id();

                    // This is important, else we'll probably stop traveling
                    // the moment we clear the stairs. That's because the
                    // (run_x, run_y) destination will no longer be valid on
                    // the new level. Setting run_x to zero forces us to
                    // recalculate our travel target next turn (see previous
                    // if block).
                    you.run_x = you.run_y = 0;
                }
            }
            else
            {
                you.running = 0;
            }
        }
        else if (Options.travel_delay > 0)
            delay(Options.travel_delay);
    }

    if (!you.running && Options.travel_delay == -1)
        redraw_screen();
}

/*
 * The travel algorithm is based on the NetHack travel code written by Warwick
 * Allison - used with his permission.
 */
void find_travel_pos( int youx, int youy,
                      char *move_x, char *move_y,
                      std::vector<coord_def>* features )
{
    init_terrain_check();

    int start_x = you.run_x, start_y = you.run_y;
    int dest_x  = youx, dest_y  = youy;
    bool floodout = false;

    // Normally we start from the destination and floodfill outwards, looking
    // for the character's current position. If we're merely trying to populate
    // the point_distance array (or exploring), we'll want to start from the
    // character's current position and fill outwards
    if (!move_x || !move_y)
    {
        start_x = youx;
        start_y = youy;

        dest_x = dest_y = -1;

        floodout = true;
    }

    // Abort run if we're trying to go someplace evil
    if (dest_x != -1
        && !is_travel_ok(start_x, start_y, false)
        && !is_trap(start_x, start_y))
    {
        you.running = 0;
        return ;
    }

    // Abort run if we're going nowhere.
    if (start_x == dest_x && start_y == dest_y)
    {
        you.running = 0;
        return ;
    }

    // How many points are we currently considering? We start off with just one
    // point, and spread outwards like a flood-filler.
    int points                  = 1;

    // How many points we'll consider next iteration.
    int next_iter_points        = 0;

    // How far we've traveled from (start_x, start_y), in moves (a diagonal move
    // is no longer than an orthogonal move).
    int traveled_distance       = 1;

    // Which index of the circumference array are we currently looking at?
    int circ_index              = 0;

    // The circumference points of the floodfilled area, for this iteration
    // and the next (this iteration's points being circ_index amd the next one's
    // being !circ_index).
    static FixedVector<coord_def, GXM * GYM> circumference[2];

    // Coordinates of all discovered traps. If we're exploring instead of
    // travelling, we'll reseed from these points after we've explored the map
    std::vector<coord_def> trap_seeds;

    // When set to true, the travel code ignores features, traps and hostile
    // terrain, and simply tries to map contiguous floorspace. Will only be set
    // to true if we're exploring, instead of travelling.
    bool ignore_hostile = false;

    // Set the seed point
    circumference[circ_index][0].x = start_x;
    circumference[circ_index][0].y = start_y;

    // Zap out previous distances array
    // XXX: Hope this works for everybody and the same problem as the
    // info buffer doesn't crop up where some compilers don't do sizeof
    // on globals correctly (but this is native to this file so hopefully
    // it's okay).
    memset( point_distance, 0, sizeof(point_distance) );

    for ( ; points > 0; ++traveled_distance, circ_index = !circ_index,
                        points = next_iter_points, next_iter_points = 0)
    {
        for (int i = 0; i < points; ++i)
        {
            const int x = circumference[circ_index][i].x;
            const int y = circumference[circ_index][i].y;

            // (x,y) is a known (explored) location - we never put unknown
            // points in the circumference vector, so we don't need to examine
            // the map array, just the grid array.
            unsigned char feature = grd[x][y];

            // If this is a feature that'll take time to travel past, we
            // simulate that extra turn by taking this feature next turn,
            // thereby artificially increasing traveled_distance.
            //
            // Note: I don't know how slow walking through shallow water and
            //       opening closed doors is - right now it's considered to have
            //       the cost of two normal moves.
            if ((feature == DNGN_SHALLOW_WATER || feature == DNGN_CLOSED_DOOR)
                    && point_distance[x][y] > traveled_distance - 2)
            {
                circumference[!circ_index][next_iter_points].x = x;
                circumference[!circ_index][next_iter_points].y = y;
                next_iter_points++;
                continue;
            }

            // For each point, we look at all surrounding points. Take them
            // orthogonals first so that the travel path doesn't zigzag all over
            // the map. Note the (dir = 1) is intentional assignment.
            for (int dir = 0; dir < 8; (dir += 2) == 8 && (dir = 1))
            {
                int dx = x + Compass[dir].x, dy = y + Compass[dir].y;

                if (dx <= 0 || dx >= GXM || dy <= 0 || dy >= GYM)
                    continue;

                if (floodout && you.running == RMODE_EXPLORE
                    && !is_player_mapped(dx, dy))
                {
                    // Setting run_x and run_y here is evil - this function
                    // should ideally not modify game state in any way.
                    you.run_x = x;
                    you.run_y = y;

                    return;
                }

                if ((dx != dest_x || dy != dest_y)
                    && !is_travel_ok(dx, dy, ignore_hostile))
                {
                    // This point is not okay to travel on, but if this is a
                    // trap, we'll want to put it on the feature vector anyway.
                    if (is_reseedable(dx, dy)
                        && !point_distance[dx][dy]
                        && (dx != start_x || dy != start_y))
                    {
                        if (features)
                        {
                            coord_def c = { dx, dy };
                            if (is_trap(dx, dy))
                                features->push_back(c);
                            trap_seeds.push_back(c);
                        }

                        // Appropriate mystic number. Nobody else should check
                        // this number, since this square is unsafe for travel.
                        point_distance[dx][dy] = -42;
                    }
                    continue;
                }

                if (dx == dest_x && dy == dest_y)
                {
                    // Hallelujah, we're home!
                    if (is_safe(x, y) && move_x && move_y)
                    {
                        *move_x = sgn(x - dest_x);
                        *move_y = sgn(y - dest_y);
                    }
                    return;
                }
                else if (!point_distance[dx][dy])
                {
                    // This point is going to be on the agenda for the next
                    // iteration
                    circumference[!circ_index][next_iter_points].x = dx;
                    circumference[!circ_index][next_iter_points].y = dy;
                    next_iter_points++;

                    point_distance[dx][dy] = traveled_distance;

                    // Negative distances here so that show_map can colour
                    // the map differently for these squares.
                    if (ignore_hostile)
                        point_distance[dx][dy] = -point_distance[dx][dy];

                    const unsigned char new_feature = grd[dx][dy];
                    if (features && !ignore_hostile
                            && new_feature != DNGN_FLOOR
                            && new_feature != DNGN_SHALLOW_WATER
                            && (dx != start_x || dy != start_y))
                    {
                        coord_def c = { dx, dy };
                        features->push_back(c);
                    }
                }
            } // for (dir = 0; dir < 8 ...
        } // for (i = 0; i < points ...

        if (!next_iter_points && features && !move_x && !ignore_hostile
            && trap_seeds.size())
        {
            // Reseed here
            for (unsigned int i = 0; i < trap_seeds.size(); ++i)
                circumference[!circ_index][i] = trap_seeds[i];

            next_iter_points = trap_seeds.size();
            ignore_hostile = true;
        }
    } // for ( ; points > 0 ...
}

// Mappings of which branches spring from which other branches, essential to
// walk backwards up the tree. Yes, this is a horrible abuse of coord_def.
static coord_def branch_backout[] =
{
    { BRANCH_SWAMP, BRANCH_LAIR },
    { BRANCH_SLIME_PITS, BRANCH_LAIR },
    { BRANCH_SNAKE_PIT, BRANCH_LAIR },

    { BRANCH_HALL_OF_BLADES, BRANCH_VAULTS },
    { BRANCH_CRYPT, BRANCH_VAULTS },

    { BRANCH_TOMB, BRANCH_CRYPT },

    { BRANCH_ELVEN_HALLS, BRANCH_ORCISH_MINES },

    { BRANCH_ORCISH_MINES, BRANCH_MAIN_DUNGEON },
    { BRANCH_HIVE, BRANCH_MAIN_DUNGEON },
    { BRANCH_LAIR, BRANCH_MAIN_DUNGEON },
    { BRANCH_VAULTS, BRANCH_MAIN_DUNGEON },
    { BRANCH_HALL_OF_ZOT, BRANCH_MAIN_DUNGEON },
    { BRANCH_ECUMENICAL_TEMPLE, BRANCH_MAIN_DUNGEON },
};

/*
 * Given a branch id, returns the parent branch. If the branch id is not found,
 * returns BRANCH_MAIN_DUNGEON.
 */
unsigned char find_parent_branch( unsigned char br )
{
    for (unsigned int i = 0;
            i < sizeof(branch_backout) / sizeof(branch_backout[0]);
            i++)
    {
        if (branch_backout[i].x == br)
            return (static_cast<unsigned char>( branch_backout[i].y ));
    }

    return (0);
}

void find_parent_branch( unsigned char br, int depth,
                         unsigned char *pb, int *pd )
{
    UNUSED( depth );

    int lev = env.stair_level[br];

    if (lev <= 0)
    {
        *pb = 0;
        *pd = 0;        // Check depth before using *pb.
        return;
    }

    *pb = find_parent_branch(br);
    *pd = subdungeon_depth(*pb, lev);
}

// Appends the passed in branch/depth to the given vector, then attempts to
// repeat the operation with the parent branch of the given branch.
//
// As an example of what it does, assume this dungeon structure
//   Stairs to lair on D:11
//   Stairs to snake pit on lair:5
//
// If level 3 of the snake pit is the level we want to track back from,
// we'd call trackback(vec, BRANCH_SNAKE_PIT, 3), and the resulting vector will
// look like:
// { BRANCH_SNAKE_PIT, 3 }, { BRANCH_LAIR, 5 }, { BRANCH_MAIN_DUNGEON, 11 }
// (Assuming, of course, that the vector started out empty.)
//
void trackback(std::vector<level_id> &vec, unsigned char branch, int subdepth)
{
    if (subdepth < 1 || subdepth > MAX_LEVELS)
        return;

    level_id lid( branch, subdepth );
    vec.push_back(lid);

    if (branch != BRANCH_MAIN_DUNGEON)
    {
        unsigned char pb;
        int pd;

        find_parent_branch( branch, subdepth, &pb, &pd );
        if (pd)
            trackback( vec, pb, pd );
    }
}

void track_intersect(std::vector<level_id> &cur,
                     std::vector<level_id> &targ,
                     level_id *cx)
{
    cx->branch = 0;
    cx->depth  = -1;

    int us = cur.size() - 1;
    int them = targ.size() - 1;

    for ( ; us >= 0 && them >= 0; us--, them--)
    {
        if (cur[us].branch != targ[them].branch)
            break;
    }

    us++, them++;

    if (us < static_cast<int>( cur.size() )
        && them < static_cast<int>( targ.size() )
        && us >= 0
        && them >= 0)
    {
        *cx = targ[them];
    }
}

/*
 * Returns the number of stairs the player would need to take to go from
 * the 'first' level to the 'second' level. If there's no obvious route between
 * 'first' and 'second', returns -1. If first == second, returns 0.
 */
static int level_distance( level_id first, level_id second )
{
    if (first == second)
        return (0);

    std::vector<level_id> fv, sv;

    // If in the same branch, easy.
    if (first.branch == second.branch)
        return (abs(first.depth - second.depth));

    // Figure out the dungeon structure between the two levels.
    trackback( fv, first.branch, first.depth );
    trackback( sv, second.branch, second.depth );

    level_id intersect;
    track_intersect( fv, sv, &intersect );

    if (intersect.depth == -1)          // No common ground?
        return (-1);

    int distance = 0;

    // If the common branch is not the same as the current branch, we'll
    // have to walk up the branch tree until we get to the common branch.
    while (first.branch != intersect.branch)
    {
        distance += first.depth;

        find_parent_branch( first.branch, first.depth,
                            &first.branch, &first.depth );
        if (!first.depth)
            return (-1);
    }

    // Now first.branch == intersect.branch
    distance += abs(first.depth - intersect.depth);

    bool ignore_end = true;
    for (int i = sv.size() - 1; i >= 0; --i)
    {
        if (ignore_end)
        {
            if (sv[i].branch == intersect.branch)
                ignore_end = false;
            continue;
        }
        distance += sv[i].depth;
    }

    return (distance);
}

static struct {
    const char *branch_name, *full_name;
    char hotkey;
} Branches [] = {
    { "Dungeon",        "the Main Dungeon", 'D' },
    { "Dis",            "Dis",              'I' },
    { "Gehenna",        "Gehenna",          'W' },
    { "Hell",           "Hell",             'U' },
    { "Cocytus",        "Cocytus",          'X' },
    { "Tartarus",       "Tartarus",         'Y' },
    { "Inferno",        "",                 'R' },
    { "The Pit",        "",                 '0' },
    { "------------",   "",                 '-' },
    { "------------",   "",                 '-' },
    { "Orcish Mines",   "the Orcish Mines", 'O' },
    { "Hive",           "the Hive",         'H' },
    { "Lair",           "the Lair",         'L' },
    { "Slime Pits",     "the Slime Pits",   'M' },
    { "Vaults",         "the Vaults",       'V' },
    { "Crypt",          "the Crypt",        'C' },
    { "Hall of Blades", "the Hall of Blades", 'B' },
    { "Zot",            "the Realm of Zot", 'Z' },
    { "Temple",         "the Ecumenical Temple",  'T' },
    { "Snake Pit",      "the Snake Pit",    'P' },
    { "Elven Halls",    "the Elven Halls",  'E' },
    { "Tomb",           "the Tomb",         'G' },
    { "Swamp",          "the Swamp",        'S' }
};

static struct {
    const char *abbr;
    unsigned char branch;
} Branch_Abbrvs[] = {
    { "D",      BRANCH_MAIN_DUNGEON },
    { "Dis",    BRANCH_DIS },
    { "Geh",     BRANCH_GEHENNA },
    { "Hell",   BRANCH_VESTIBULE_OF_HELL },
    { "Coc",    BRANCH_COCYTUS },
    { "Tar",    BRANCH_TARTARUS },
    { "inf",    BRANCH_INFERNO },
    { "pit",    BRANCH_THE_PIT },
    { "Orc",    BRANCH_ORCISH_MINES },
    { "Hive",   BRANCH_HIVE },
    { "Lair",   BRANCH_LAIR },
    { "Slime",    BRANCH_SLIME_PITS },
    { "Vault",  BRANCH_VAULTS },
    { "Crypt",  BRANCH_CRYPT },
    { "Blades", BRANCH_HALL_OF_BLADES },
    { "Zot",    BRANCH_HALL_OF_ZOT },
    { "Temple", BRANCH_ECUMENICAL_TEMPLE },
    { "Snake",  BRANCH_SNAKE_PIT },
    { "Elf",    BRANCH_ELVEN_HALLS },
    { "Tomb",   BRANCH_TOMB },
    { "Swamp",  BRANCH_SWAMP },
};

void set_trans_travel_dest(char *buffer, int maxlen, const level_pos &target)
{
    if (!buffer)
        return;

    const char *branch = NULL;
    for (unsigned int i = 0; i < sizeof(Branch_Abbrvs) / sizeof(Branch_Abbrvs[0]);
            ++i)
    {
        if (Branch_Abbrvs[i].branch == target.id.branch)
        {
            branch = Branch_Abbrvs[i].abbr;
            break;
        }
    }

    if (!branch)
        return;

    const unsigned char branch_id = target.id.branch;
    // Show level+depth information and tack on an @(x,y) if the player
    // wants to go to a specific square on the target level. We don't use
    // actual coordinates since that will give away level information we
    // don't want the player to have.
    if (branch_id != BRANCH_ECUMENICAL_TEMPLE
            && branch_id != BRANCH_HALL_OF_BLADES
            && branch_id != BRANCH_VESTIBULE_OF_HELL)
    {
        snprintf( buffer, maxlen, "%s:%d%s", branch, target.id.depth,
                  (target.pos.x != -1) ? " @ (x,y)" : "" );
    }
    else
    {
        snprintf( buffer, maxlen, "%s%s", branch,
                  (target.pos.x != -1) ? " @ (x,y)" : "" );
    }
}

// Returns the level on the given branch that's closest to the player's
// current location.
static int get_nearest_level_depth( unsigned char branch )
{
    int depth = 1;

    // Hell needs special treatment, because we can't walk up
    // Hell and its branches to the main dungeon.
    if (branch == BRANCH_MAIN_DUNGEON
        && (player_in_branch( BRANCH_VESTIBULE_OF_HELL )
            || player_in_branch( BRANCH_COCYTUS )
            || player_in_branch( BRANCH_TARTARUS )
            || player_in_branch( BRANCH_DIS )
            || player_in_branch( BRANCH_GEHENNA )))
    {
        return (you.hell_exit + 1);
    }

    level_id id = level_id::get_current_level_id();

    do
    {
        find_parent_branch( id.branch, id.depth, &id.branch, &id.depth );
        if (id.depth && id.branch == branch)
        {
            depth = id.depth;
            break;
        }
    }
    while (id.depth);

    return (depth);
}

// Returns true if the player character knows of the existence of the given
// branch (which would make the branch a valid target for interlevel travel).
static bool is_known_branch( unsigned char branch )
{
    // The main dungeon is always known.
    if (branch == BRANCH_MAIN_DUNGEON)
        return (true);

    // If we're in the branch, it darn well is known.
    if (you.where_are_you == branch)
        return (true);

    // If the overmap knows the stairs to this branch, we know the branch.
    unsigned char par;
    int pdep;

    find_parent_branch( branch, 1, &par, &pdep );
    if (pdep)
        return (true);

    // Do a brute force search in the travel cache for this branch.
    return (travel_cache.is_known_branch(branch));

    // Doing this to check for the reachability of a branch is slow enough to
    // be noticeable.

    // Ask interlevel travel if it knows how to go there. If it knows how to
    // get (partway) there, return true.

    // level_pos pos;
    // pos.id.branch = branch;
    // pos.id.depth  = 1;

    // return find_transtravel_square(pos, false) != 0;
}

/*
 * Returns a list of the branches that the player knows the location of the
 * stairs to, in the same order as overmap.cc lists them.
 */
static std::vector<unsigned char> get_known_branches()
{
    // Lifted from overmap.cc. XXX: Move this to a better place?
    const unsigned char list_order[] = {
        BRANCH_MAIN_DUNGEON,
        BRANCH_ECUMENICAL_TEMPLE,
        BRANCH_ORCISH_MINES, BRANCH_ELVEN_HALLS,
        BRANCH_LAIR, BRANCH_SWAMP, BRANCH_SLIME_PITS, BRANCH_SNAKE_PIT,
        BRANCH_HIVE,
        BRANCH_VAULTS, BRANCH_HALL_OF_BLADES, BRANCH_CRYPT, BRANCH_TOMB,
        BRANCH_VESTIBULE_OF_HELL,
        BRANCH_DIS, BRANCH_GEHENNA, BRANCH_COCYTUS, BRANCH_TARTARUS,
        BRANCH_HALL_OF_ZOT
    };

    std::vector<unsigned char> my_branches;
    for (unsigned int i = 0; i < sizeof(list_order) / sizeof(*list_order); ++i)
    {
        if (is_known_branch( list_order[i] ))
            my_branches.push_back( list_order[i] );
    }

    return (my_branches);
}

static int prompt_travel_branch()
{
    unsigned char branch = BRANCH_MAIN_DUNGEON;     // Default
    std::vector<unsigned char> br = get_known_branches();

    // Don't kill the prompt even if the only branch we know is the main dungeon
    // This keeps things consistent for the player.
    if (br.size() < 1)
        return (branch);

    bool waypoint_list = false;
    int waycount = travel_cache.get_waypoint_count();

    for (;;)
    {
        char buf[100];
        if (waypoint_list)
            travel_cache.list_waypoints();
        else
        {
            int linec = 0;
            string line;

            for (int i = 0, count = br.size(); i < count; ++i, ++linec)
            {
                if (linec == 4)
                {
                    linec = 0;
                    mpr(line.c_str());
                    line = "";
                }

                snprintf(buf, sizeof(buf), "(%c) %-14s ", Branches[br[i]].hotkey,
                         Branches[br[i]].branch_name);
                line += buf;
            }

            if (line.length())
                mpr(line.c_str());
        }

        char shortcuts[100];
        *shortcuts = 0;
        if (*trans_travel_dest || waycount || waypoint_list)
        {
            strncpy(shortcuts, "(", sizeof(shortcuts));
            if (waypoint_list)
                strncat(shortcuts, "[*] lists branches", sizeof(shortcuts));
            else if (waycount)
                strncat(shortcuts, "[*] lists waypoints", sizeof(shortcuts));

            if (*trans_travel_dest)
            {
                char travel_dest[60];
                snprintf( travel_dest, sizeof(travel_dest), "[Enter] for %s",
                          trans_travel_dest );

                if (waypoint_list || waycount)
                    strncat( shortcuts, ", ", sizeof(shortcuts));

                strncat( shortcuts, travel_dest, sizeof(shortcuts));
            }

            strncat( shortcuts, ") ", sizeof(shortcuts) );
        }

        mpr( MSGCH_PROMPT, "Where do you want to go? %s", shortcuts );

        int keyin = get_ch();
        if (keyin == ESCAPE)
            return (MAX_BRANCHES);

        // XXX: Ick!
        if (keyin == '\n' || keyin == '\r')
            return (MAX_BRANCHES + 1);

        for (int i = 0, count = br.size(); i < count; ++i)
        {
            if (toupper(keyin) == Branches[br[i]].hotkey)
                return (br[i]);
        }

        if (keyin == '*' && (waypoint_list || waycount))
        {
            waypoint_list = !waypoint_list;
            mesclr();
            continue;
        }

        // We're cheating here by checking for waypoints.
        if (keyin >= '0' && keyin <= '9')
            return (-1 - (keyin - '0'));

        return (MAX_BRANCHES);
    }
}

static int prompt_travel_depth(unsigned char branch)
{
    // Handle one-level branches by not prompting.
    if (branch == BRANCH_ECUMENICAL_TEMPLE
        || branch == BRANCH_VESTIBULE_OF_HELL
        || branch == BRANCH_HALL_OF_BLADES)
    {
        return (1);
    }

    char buf[100];
    int depth = get_nearest_level_depth(branch);

    mesclr();
    mpr( MSGCH_PROMPT, "What level of %s do you want to go to? [default %d] ",
         Branches[branch].full_name, depth );

    if (!cancelable_get_line( buf, sizeof( buf )))
        return (0);

    if (*buf)
        depth = atoi(buf);

    return (depth);
}

static level_pos prompt_translevel_target()
{
    level_pos target;
    int branch = prompt_travel_branch();

    if (branch < 0)
    {
        travel_cache.travel_to_waypoint( -branch - 1 );
        return (target);
    }

    if (branch == MAX_BRANCHES)
        return (target);

    // If user chose to repeat last travel, return that.
    if (branch > MAX_BRANCHES)
        return (travel_target);

    target.id.branch = branch;

    // User's chosen a branch, so now we ask for a level.
    target.id.depth = prompt_travel_depth( target.id.branch );

    if (target.id.depth < 1 || target.id.depth >= MAX_LEVELS)
        target.id.depth = -1;

    if (target.id.depth > -1)
    {
        set_trans_travel_dest( trans_travel_dest, sizeof( trans_travel_dest ),
                               target );
    }

    return (target);
}

void start_translevel_travel(bool prompt_for_destination)
{
    if (!check_naked_ok())
        return;

    // Update information for this level. We need it even for the prompts, so
    // we can't wait to confirm that the user chose to initiate travel.
    travel_cache.get_level_info( level_id::get_current_level_id() ).update();

    if (prompt_for_destination)
    {
        // prompt_translevel_target may actually initiate travel directly if
        // the user chose a waypoint instead of a branch + depth. As far as
        // we're concerned, if the target depth is unset, we need to take no
        // further action.
        level_pos target = prompt_translevel_target();
        if (target.id.depth == -1)
            return;

        travel_target = target;
    }

    if (level_id::get_current_level_id() == travel_target.id
        && (travel_target.pos.x == -1
            || (travel_target.pos.x == you.x_pos
                && travel_target.pos.y == you.y_pos)))
    {
        mpr("You're already here!");
        return;
    }

    if (travel_target.id.depth > -1)
    {
        you.running = RMODE_INTERLEVEL;
        you.run_x = you.run_y = 0;
        last_stair.depth = -1;
    }
}

int trans_negotiate_stairs()
{
    const int key = ((grd[you.x_pos][you.y_pos] < DNGN_STONE_STAIRS_UP_I
                    || grd[you.x_pos][you.y_pos] > DNGN_ROCK_STAIRS_UP)
                    && (grd[you.x_pos][you.y_pos] < DNGN_RETURN_FROM_ORCISH_MINES
                    || grd[you.x_pos][you.y_pos] > DNGN_RETURN_FROM_SWAMP))
                    ?  '>' : '<';

    return (key);
}

int subdungeon_depth(unsigned char branch, int depth)
{
    int curr_subdungeon_level = depth + 1;

    // maybe last part better expresssed as <= PIT {dlb}
    if (branch >= BRANCH_DIS && branch <= BRANCH_THE_PIT)
        curr_subdungeon_level = depth - 26;

    /* Remember, must add this to the death_string in ouch */
    if (branch >= BRANCH_ORCISH_MINES && branch <= BRANCH_SWAMP)
        curr_subdungeon_level = depth - you.branch_stairs[branch - 10];

    return (curr_subdungeon_level);
}

static int target_distance_from( const coord_def &pos )
{
    for (int i = 0, count = curr_stairs.size(); i < count; ++i)
    {
        if (curr_stairs[i].position == pos)
            return (curr_stairs[i].distance);
    }

    return (-1);
}

/*
 * Sets best_stair to the coordinates of the best stair on the player's current
 * level to take to get to the 'target' level. Should be called with 'distance'
 * set to 0, 'stair' set to (you.x_pos, you.y_pos) and 'best_distance' set to
 * -1. 'cur' should be the player's current level.
 *
 * If best_stair remains unchanged when this function returns, there is no
 * travel-safe path between the player's current level and the target level OR
 * the player's current level *is* the target level.
 *
 * This function relies on the point_distance array being correctly populated
 * with a floodout call to find_travel_pos starting from the player's location.
 */
static int find_transtravel_stair( const level_id &cur,
                                   const level_pos &target,
                                   int distance,
                                   const coord_def &stair,
                                   level_id &closest_level,
                                   int &best_level_distance,
                                   coord_def &best_stair )
{
    int local_distance = -1;
    level_id player_level = level_id::get_current_level_id();

    // Have we reached the target level?
    if (cur == target.id)
    {
        // If there's no target position on the target level, or we're on the
        // target, we're home.
        if (target.pos.x == -1 || target.pos == stair)
            return (distance);

        // If there *is* a target position, we need to work out our distance
        // from it.
        int deltadist = target_distance_from(stair);

        if (deltadist == -1 && cur == player_level)
        {
            // Okay, we don't seem to have a distance available to us, which
            // means we're either (a) not standing on stairs or (b) whoever
            // initiated interlevel travel didn't call
            // populate_stair_distances.  Assuming we're not on stairs, that
            // situation can arise only if interlevel travel has been triggered
            // for a location on the same level. If that's the case, we can get
            // the distance off the point_distance matrix.
            deltadist = point_distance[target.pos.x][target.pos.y];
            if (!deltadist
                && (stair.x != target.pos.x || stair.y != target.pos.y))
            {
                deltadist = -1;
            }
        }

        if (deltadist != -1)
        {
            local_distance = distance + deltadist;

            // See if this is a degenerate case of interlevel travel:
            // A degenerate case of interlevel travel decays to normal travel;
            // we identify this by checking if:
            // a. The current level is the target level.
            // b. The target square is reachable from the 'current' square.
            // c. The current square is where the player is.
            //
            // Note that even if this *is* degenerate, interlevel travel may
            // still be able to find a shorter route, since it can consider
            // routes that leave and reenter the current level.
            if (player_level == target.id && stair.x == you.x_pos
                    && stair.y == you.y_pos)
            {
                best_stair = target.pos;
            }

            // The local_distance is already set, but there may actually be
            // stairs we can take that'll get us to the target faster than the
            // direct route, so we also try the stairs.
        }
    }

    LevelInfo &li = travel_cache.get_level_info(cur);
    std::vector<stair_info> &stairs = li.get_stairs();

    // this_stair being NULL is perfectly acceptable, since we start with
    // coords as the player coords, and the player need not be standing on
    // stairs.
    stair_info *this_stair = li.get_stair(stair);

    if (!this_stair && cur != player_level)
    {
        // Whoops, there's no stair in the travel cache for the current
        // position, and we're not on the player's current level (i.e., there
        // certainly *should* be a stair here). Since we can't proceed in any
        // reasonable way, bail out.
        return (local_distance);
    }

    for (int i = 0, count = stairs.size(); i < count; ++i)
    {
        stair_info &si = stairs[i];

        int deltadist = li.distance_between( this_stair, &si );
        if (!this_stair)
        {
            deltadist = point_distance[si.position.x][si.position.y];
            if (!deltadist
                && (you.x_pos != si.position.x || you.y_pos != si.position.y))
            {
                deltadist = -1;
            }
        }

        // deltadist == 0 is legal (if this_stair is NULL), since the player
        // may be standing on the stairs. If two stairs are disconnected,
        // deltadist has to be negative.
        if (deltadist < 0)
            continue;

        int dist2stair = distance + deltadist;
        if (si.distance == -1 || si.distance > dist2stair)
        {
            si.distance = dist2stair;

            dist2stair += Options.travel_stair_cost;
            ++dist2stair;   // Account for the cost of taking the stairs

            // Already too expensive? Short-circuit.
            if (local_distance != -1 && dist2stair >= local_distance)
                continue;

            const level_pos &dest = si.destination;

            // We can only short-circuit the stair-following process if we
            // have no exact target location. If there *is* an exact target
            // location, we can't follow stairs for which we have incomplete
            // information.
            if (target.pos.x == -1 && dest.id == target.id)
            {
                if (local_distance == -1 || local_distance > dist2stair)
                {
                    local_distance = dist2stair;
                    if (cur == player_level && you.x_pos == stair.x
                        && you.y_pos == stair.y)
                    {
                        best_stair = si.position;
                    }
                }
                continue;
            }

            if (dest.id.depth > -1) // We have a valid level descriptor.
            {
                int dist = level_distance(dest.id, target.id);

                if (dist != -1
                    && (dist < best_level_distance
                        || best_level_distance == -1))
                {
                    best_level_distance = dist;
                    closest_level       = dest.id;
                }
            }

            // If we don't know where these stairs go, we can't take them.
            if (!dest.is_valid())
                continue;

            // We need to get the stairs at the new location and set the
            // distance on them as well.
            LevelInfo &lo = travel_cache.get_level_info(dest.id);
            stair_info *so = lo.get_stair(dest.pos);

            if (so)
            {
                if (so->distance == -1 || so->distance > dist2stair)
                    so->distance = dist2stair;
                else
                    continue;   // We've already been here.
            }

            // Okay, take these stairs and keep going.
            const int newdist = find_transtravel_stair(dest.id, target,
                                    dist2stair, dest.pos, closest_level,
                                    best_level_distance, best_stair);

            if (newdist != -1 &&
                    (local_distance == -1 || local_distance > newdist))
            {
                local_distance = newdist;
                if (cur == player_level && you.x_pos == stair.x
                    && you.y_pos == stair.y)
                {
                    best_stair = si.position;
                }
            }
        }
    }

    return (local_distance);
}

static void populate_stair_distances( const level_pos &target )
{
    // Populate point_distance.
    find_travel_pos( target.pos.x, target.pos.y, NULL, NULL, NULL );

    level_id current = level_id::get_current_level_id();
    LevelInfo &li = travel_cache.get_level_info(current);
    const std::vector<stair_info> &stairs = li.get_stairs();

    curr_stairs.clear();
    for (int i = 0, count = stairs.size(); i < count; ++i)
    {
        stair_info si = stairs[i];
        si.distance = point_distance[si.position.x][si.position.y];

        if (!si.distance && target.pos != si.position)
            si.distance = -1;

        if (si.distance < -1)
            si.distance = -1;

        curr_stairs.push_back(si);
    }
}

static int find_transtravel_square( const level_pos &target, bool verbose )
{
    level_id current = level_id::get_current_level_id();

    coord_def best_stair = { -1, -1 };
    coord_def cur_stair  = { you.x_pos, you.y_pos };
    level_id closest_level;
    int best_level_distance = -1;
    travel_cache.reset_distances();

    find_travel_pos(you.x_pos, you.y_pos, NULL, NULL, NULL);

    find_transtravel_stair(current, target,
                           0, cur_stair, closest_level,
                           best_level_distance, best_stair);

    if (best_stair.x != -1 && best_stair.y != -1)
    {
        you.run_x = best_stair.x;
        you.run_y = best_stair.y;
        return 1;
    }
    else if (best_level_distance != -1 && closest_level != current
            && target.pos.x == -1)
    {
        int current_dist = level_distance(current, target.id);
        level_pos newlev;
        newlev.id = closest_level;
        if (current_dist == -1 || best_level_distance < current_dist)
            return (find_transtravel_square(newlev, verbose));
    }

    if (verbose && target.id != current)
        mpr("Sorry, I don't know how to get there.");

    return (0);
}

void start_travel( int x, int y )
{
    // Redundant target?
    if ((x == you.x_pos && y == you.y_pos) || !check_naked_ok())
        return;

    // Start running
    you.running = RMODE_TRAVEL;
    you.run_x   = x;
    you.run_y   = y;

    // Check whether we can get to the square.
    find_travel_pos( you.x_pos, you.y_pos, NULL, NULL, NULL );

    if (point_distance[x][y] == 0
        && (x != you.x_pos || you.run_y != you.y_pos)
        && is_travel_ok( x, y, false ))
    {
        // We'll need interlevel travel to get here.
        travel_target.id = level_id::get_current_level_id();
        travel_target.pos.x = x;
        travel_target.pos.y = y;

        you.running = RMODE_INTERLEVEL;
        you.run_x = you.run_y = 0;
        last_stair.depth = -1;

        // We need the distance of the target from the various stairs around.
        populate_stair_distances( travel_target );

        set_trans_travel_dest( trans_travel_dest, sizeof( trans_travel_dest ),
                               travel_target );
    }
}

void start_explore()
{
    if (!check_naked_ok())
        return;

    you.running   = RMODE_EXPLORE;

    if (Options.explore_item_stop) // Clone shadow array off map
        shadow_map();
}

/*
 * Given a feature vector, arranges the features in the order that the player
 * is most likely to be interested in. Currently, the only thing it does is to
 * put altars of the player's religion at the front of the list.
 */
void arrange_features( std::vector<coord_def> &features )
{
    for (int i = 0, count = features.size(); i < count; ++i)
    {
        if (is_player_altar( features[i] ))
        {
            int place = i;
            // Shuffle this altar as far up the list as possible.
            for (int j = place - 1; j >= 0; --j)
            {
                if (is_altar(features[j]))
                {
                    if (is_player_altar(features[j]))
                        break;

                    coord_def temp = features[j];
                    features[j] = features[place];
                    features[place] = temp;

                    place = j;
                }
            }
        }
    }
}

//////////////////////////////////////////////////////////////////////////
// Interlevel travel classes

static void writeCoord(FILE *file, const coord_def &pos)
{
    writeShort(file, pos.x);
    writeShort(file, pos.y);
}

static void readCoord(FILE *file, coord_def &pos)
{
    pos.x = readShort(file);
    pos.y = readShort(file);
}

level_id level_id::get_current_level_id()
{
    level_id id;
    id.branch = you.where_are_you;
    id.depth  = subdungeon_depth( you.where_are_you, you.depth );

    return (id);
}

level_id level_id::get_next_level_id( const coord_def &pos )
{
    short gridc = grd[pos.x][pos.y];
    level_id id = get_current_level_id();

    switch (gridc)
    {
    case DNGN_STONE_STAIRS_DOWN_I:   case DNGN_STONE_STAIRS_DOWN_II:
    case DNGN_STONE_STAIRS_DOWN_III: case DNGN_ROCK_STAIRS_DOWN:
        id.depth++;
        break;
    case DNGN_STONE_STAIRS_UP_I:     case DNGN_STONE_STAIRS_UP_II:
    case DNGN_STONE_STAIRS_UP_III:   case DNGN_ROCK_STAIRS_UP:
        id.depth--;
        break;
    case DNGN_ENTER_HELL:
        id.branch = BRANCH_VESTIBULE_OF_HELL;
        id.depth  = 1;
        break;
    case DNGN_ENTER_DIS:
        id.branch = BRANCH_DIS;
        id.depth  = 1;
        break;
    case DNGN_ENTER_GEHENNA:
        id.branch = BRANCH_GEHENNA;
        id.depth  = 1;
        break;
    case DNGN_ENTER_COCYTUS:
        id.branch = BRANCH_COCYTUS;
        id.depth  = 1;
        break;
    case DNGN_ENTER_TARTARUS:
        id.branch = BRANCH_TARTARUS;
        id.depth  = 1;
        break;
    case DNGN_ENTER_ORCISH_MINES:
        id.branch = BRANCH_ORCISH_MINES;
        id.depth  = 1;
        break;
    case DNGN_ENTER_HIVE:
        id.branch = BRANCH_HIVE;
        id.depth  = 1;
        break;
    case DNGN_ENTER_LAIR:
        id.branch = BRANCH_LAIR;
        id.depth  = 1;
        break;
    case DNGN_ENTER_SLIME_PITS:
        id.branch = BRANCH_SLIME_PITS;
        id.depth  = 1;
        break;
    case DNGN_ENTER_VAULTS:
        id.branch = BRANCH_VAULTS;
        id.depth  = 1;
        break;
    case DNGN_ENTER_CRYPT:
        id.branch = BRANCH_CRYPT;
        id.depth  = 1;
        break;
    case DNGN_ENTER_HALL_OF_BLADES:
        id.branch = BRANCH_HALL_OF_BLADES;
        id.depth  = 1;
        break;
    case DNGN_ENTER_ZOT:
        id.branch = BRANCH_HALL_OF_ZOT;
        id.depth  = 1;
        break;
    case DNGN_ENTER_TEMPLE:
        id.branch = BRANCH_ECUMENICAL_TEMPLE;
        id.depth  = 1;
        break;
    case DNGN_ENTER_SNAKE_PIT:
        id.branch = BRANCH_SNAKE_PIT;
        id.depth  = 1;
        break;
    case DNGN_ENTER_ELVEN_HALLS:
        id.branch = BRANCH_ELVEN_HALLS;
        id.depth  = 1;
        break;
    case DNGN_ENTER_TOMB:
        id.branch = BRANCH_TOMB;
        id.depth  = 1;
        break;
    case DNGN_ENTER_SWAMP:
        id.branch = BRANCH_SWAMP;
        id.depth  = 1;
        break;

    case DNGN_RETURN_FROM_ORCISH_MINES:     case DNGN_RETURN_FROM_HIVE:
    case DNGN_RETURN_FROM_LAIR:             case DNGN_RETURN_FROM_SLIME_PITS:
    case DNGN_RETURN_FROM_VAULTS:           case DNGN_RETURN_FROM_CRYPT:
    case DNGN_RETURN_FROM_HALL_OF_BLADES:   case DNGN_RETURN_FROM_ZOT:
    case DNGN_RETURN_FROM_TEMPLE:           case DNGN_RETURN_FROM_SNAKE_PIT:
    case DNGN_RETURN_FROM_ELVEN_HALLS:      case DNGN_RETURN_FROM_TOMB:
    case DNGN_RETURN_FROM_SWAMP:
        find_parent_branch( id.branch, id.depth, &id.branch, &id.depth );
        if (!id.depth)
        {
            id.branch = find_parent_branch( you.where_are_you );
            id.depth  = -1;
        }
        break;
    }

    return (id);
}

void level_id::save(FILE *file) const
{
    writeByte(file, branch);
    writeShort(file, depth);
}

void level_id::load(FILE *file)
{
    branch = readByte(file);
    depth  = readShort(file);
}

void level_pos::save(FILE *file) const
{
    id.save(file);
    writeCoord(file, pos);
}

void level_pos::load(FILE *file)
{
    id.load(file);
    readCoord(file, pos);
}

void stair_info::save(FILE *file) const
{
    writeCoord(file, position);
    destination.save(file);
    writeByte(file, guessed_pos? 1 : 0);
}

void stair_info::load(FILE *file)
{
    readCoord(file, position);
    destination.load(file);
    guessed_pos = readByte(file) != 0;
}

LevelInfo::LevelInfo(const LevelInfo &other)
{
    stairs = other.stairs;
    int sz = stairs.size() * stairs.size();
    stair_distances = new short [ sz ];
    if (other.stair_distances)
        memcpy( stair_distances, other.stair_distances, sz * sizeof(int) );
}

const LevelInfo &LevelInfo::operator = ( const LevelInfo &other )
{
    if (&other == this)
        return (*this);

    stairs = other.stairs;
    int sz = stairs.size() * stairs.size();
    delete [] stair_distances;

    stair_distances = new short [ sz ];
    if (other.stair_distances)
        memcpy( stair_distances, other.stair_distances, sz * sizeof(short) );

    return (*this);
}

LevelInfo::~LevelInfo()
{
    delete [] stair_distances;
}

void LevelInfo::update()
{
    // We need to update all stair information and distances on this level.

    // First, we get all known stairs
    std::vector<coord_def> stair_positions;
    get_stairs( stair_positions );

    // Make sure our stair list is correct.
    correct_stair_list( stair_positions );

    update_stair_distances();
}

void LevelInfo::update_stair_distances()
{
    // Now we update distances for all the stairs, relative to all other
    // stairs.
    for (int s = 0, end = stairs.size(); s < end; ++s)
    {
        // For each stair, we need to ask travel to populate the distance
        // array.
        find_travel_pos( stairs[s].position.x, stairs[s].position.y,
                          NULL, NULL, NULL );

        for (int other = 0; other < end; ++other)
        {
            const int ox = stairs[other].position.x;
            const int oy = stairs[other].position.y;

            int dist = point_distance[ox][oy];

            // Note dist == 0 is illegal because we can't have two stairs on
            // the same square.
            if (dist <= 0)
                dist = -1;

            stair_distances[ s * stairs.size() + other ] = dist;
            stair_distances[ other * stairs.size() + s ] = dist;
        }
    }
}

void LevelInfo::update_stair( int x, int y, const level_pos &p, bool guess )
{
    stair_info *si = get_stair(x, y);

    // What 'guess' signifies: whenever you take a stair from A to B, the travel
    // code knows that the stair reliably takes you from A->B. In that case,
    // update_stair() is called with guess == false.
    //
    // Unfortunately, Crawl doesn't guarantee that A->B implies B->A, but the
    // travel code has to assume that anyway (because that's what the player
    // will expect), and call update_stair() again with guess == true.
    //
    // The idea of using 'guess' is that we'll update the stair's destination
    // with a guess only if we know that the currently set destination is
    // itself a guess.
    //
    if (si && (si->guessed_pos || !guess))
    {
        si->destination = p;
        si->guessed_pos = guess;
    }
}

stair_info *LevelInfo::get_stair(int x, int y)
{
    coord_def c = { x, y };
    return (get_stair(c));
}

stair_info *LevelInfo::get_stair( const coord_def &pos )
{
    const int index = get_stair_index(pos);

    return ((index != -1) ? &stairs[index] : NULL);
}

int LevelInfo::get_stair_index(const coord_def &pos) const
{
    for (int i = stairs.size() - 1; i >= 0; --i)
    {
        if (stairs[i].position == pos)
            return (i);
    }
    return (-1);
}

void LevelInfo::add_waypoint(const coord_def &pos)
{
    if (pos.x < 0 || pos.y < 0)
        return;

    // First, make sure we don't already have this position in our stair list.
    for (int i = 0, sz = stairs.size(); i < sz; ++i)
    {
        if (stairs[i].position == pos)
            return;
    }

    stair_info si;
    si.position = pos;
    si.destination.id.depth = -2;   // Magic number for waypoints.

    stairs.push_back(si);

    delete [] stair_distances;
    stair_distances = new short [ stairs.size() * stairs.size() ];

    update_stair_distances();
}

void LevelInfo::remove_waypoint(const coord_def &pos)
{
    for (std::vector<stair_info>::iterator i = stairs.begin();
            i != stairs.end(); ++i)
    {
        if (i->position == pos && i->destination.id.depth == -2)
        {
            stairs.erase(i);
            break;
        }
    }

    delete [] stair_distances;
    stair_distances = new short [ stairs.size() * stairs.size() ];

    update_stair_distances();
}

void LevelInfo::correct_stair_list(const std::vector<coord_def> &s)
{
    // If we have a waypoint on this level, we'll always delete stair_distances
    delete [] stair_distances;
    stair_distances = NULL;

    // First we kill any stairs in 'stairs' that aren't there in 's'.
    for (std::vector<stair_info>::iterator i = stairs.begin();
            i != stairs.end(); ++i)
    {

        // Waypoints are not stairs, so we skip them.
        if (i->destination.id.depth == -2)
            continue;

        bool found = false;
        for (int j = s.size() - 1; j >= 0; --j)
        {
            if (s[j] == i->position)
            {
                found = true;
                break;
            }
        }

        if (!found)
            stairs.erase(i--);
    }

    // For each stair in 's', make sure we have a corresponding stair
    // in 'stairs'.
    for (int i = 0, sz = s.size(); i < sz; ++i)
    {
        bool found = false;
        for (int j = stairs.size() - 1; j >= 0; --j)
        {
            if (s[i] == stairs[j].position)
            {
                found = true;
                break;
            }
        }

        if (!found)
        {
            stair_info si;
            si.position = s[i];
            si.destination.id = level_id::get_next_level_id(s[i]);
            // We don't know where on the next level these stairs go to, but
            // that can't be helped. That information will have to be filled
            // in whenever the player takes these stairs.
            stairs.push_back(si);
        }
    }

    stair_distances = new short [ stairs.size() * stairs.size() ];
}

int LevelInfo::distance_between( const stair_info *s1, const stair_info *s2 )
                    const
{
    if (!s1 || !s2)
        return (0);

    if (s1 == s2)
        return (0);

    const int i1 = get_stair_index(s1->position);
    const int i2 = get_stair_index(s2->position);

    if (i1 == -1 || i2 == -1)
        return (0);

    return (stair_distances[ i1 * stairs.size() + i2 ]);
}

void LevelInfo::get_stairs(std::vector<coord_def> &st)
{
    // These are env map coords, not grid coordinates.
    for (int y = 1; y < GYM; ++y)
    {
        for (int x = 1; x < GXM; ++x)
        {
            const unsigned char grid = grd[x][y];
            if (is_terrain_known(x, y) && is_stair(grid))
            {
                // Convert to grid coords, because that's what we use
                // everywhere else.
                coord_def stair = { x, y };
                st.push_back(stair);
            }
        }
    }
}

void LevelInfo::reset_distances()
{
    for (int i = 0, count = stairs.size(); i < count; ++i)
        stairs[i].reset_distance();
}

bool LevelInfo::is_known_branch(unsigned char branch) const
{
    for (int i = 0, count = stairs.size(); i < count; ++i)
    {
        if (stairs[i].destination.id.branch == branch)
            return (true);
    }

    return (false);
}

void LevelInfo::travel_to_waypoint(const coord_def &pos)
{
    stair_info *target = get_stair(pos);
    if (!target)
        return;

    curr_stairs.clear();
    for (int i = 0, sz = stairs.size(); i < sz; ++i)
    {
        if (stairs[i].destination.id.depth == -2)
            continue;

        stair_info si = stairs[i];
        si.distance   = distance_between( target, &stairs[i] );

        curr_stairs.push_back(si);
    }

    start_translevel_travel( false );
}

void LevelInfo::save(FILE *file) const
{
    int stair_count = stairs.size();

    // How many stairs do we know of?
    writeShort( file, stair_count );
    for (int i = 0; i < stair_count; ++i)
        stairs[i].save(file);

    if (stair_count)
    {
        // XXX Assert stair_distances != NULL?
        // Save stair distances as short ints.
        for (int i = stair_count * stair_count - 1; i >= 0; --i)
            writeShort( file, stair_distances[i] );
    }
}

void LevelInfo::load(FILE *file)
{
    stairs.clear();
    int stair_count = readShort(file);
    for (int i = 0; i < stair_count; ++i)
    {
        stair_info si;
        si.load(file);
        stairs.push_back(si);
    }

    if (stair_count)
    {
        delete [] stair_distances;
        stair_distances = new short [ stair_count * stair_count ];
        for (int i = stair_count * stair_count - 1; i >= 0; --i)
            stair_distances[i] = readShort( file );
    }
}

void TravelCache::travel_to_waypoint(int num)
{
    if (num < 0 || num >= TRAVEL_WAYPOINT_COUNT)
        return;

    if (waypoints[num].id.depth == -1)
        return;

    travel_target = waypoints[num];
    set_trans_travel_dest( trans_travel_dest, sizeof( trans_travel_dest ),
                           travel_target );

    LevelInfo &li = get_level_info( travel_target.id );
    li.travel_to_waypoint( travel_target.pos );
}

void TravelCache::list_waypoints() const
{
    string line;
    char dest[30];
    char choice[50];
    int count = 0;

    for (int i = 0; i < TRAVEL_WAYPOINT_COUNT; ++i)
    {
        if (waypoints[i].id.depth == -1)
            continue;

        set_trans_travel_dest( dest, sizeof( dest ), waypoints[i] );

        // All waypoints will have @ (x,y), remove that.
        char *at = strchr( dest, '@' );
        if (at)
            *--at = 0;

        snprintf( choice, sizeof( choice ), "(%d) %-8s", i, dest );
        line += choice;
        if (!(++count % 5))
        {
            mpr(line.c_str());
            line = "";
        }
    }

    if (line.length())
        mpr(line.c_str());
}

void TravelCache::add_waypoint()
{
    if (get_waypoint_count())
    {
        mpr("Existing waypoints");
        list_waypoints();
    }

    mpr( MSGCH_PROMPT, "Assign waypoint to what number? (0-9) " );
    int keyin = get_ch();

    if (keyin < '0' || keyin > '9')
        return;

    int waynum = keyin - '0';

    if (waypoints[waynum].is_valid())
    {
        bool unique_waypoint = true;
        for (int i = 0; i < TRAVEL_WAYPOINT_COUNT; ++i)
        {
            if (i == waynum)
                continue;

            if (waypoints[waynum] == waypoints[i])
            {
                unique_waypoint = false;
                break;
            }
        }

        if (unique_waypoint)
        {
            LevelInfo &li = get_level_info(waypoints[waynum].id);
            li.remove_waypoint(waypoints[waynum].pos);
        }
    }

    coord_def pos = { you.x_pos, you.y_pos };
    const level_id &lid = level_id::get_current_level_id();

    LevelInfo &li = get_level_info(lid);
    li.add_waypoint(pos);

    waypoints[waynum].id  = lid;
    waypoints[waynum].pos = pos;
}

int TravelCache::get_waypoint_count() const
{
    int count = 0;
    for (int i = 0; i < TRAVEL_WAYPOINT_COUNT; ++i)
    {
        if (waypoints[i].is_valid())
            count++;
    }

    return (count);
}

void TravelCache::reset_distances()
{
    std::map<level_id, LevelInfo, level_id::less_than>::iterator i =
                    levels.begin();

    for ( ; i != levels.end(); ++i)
        i->second.reset_distances();
}

bool TravelCache::is_known_branch( unsigned char branch ) const
{
    std::map<level_id, LevelInfo, level_id::less_than>::const_iterator i =
                    levels.begin();

    for ( ; i != levels.end(); ++i)
    {
        if (i->second.is_known_branch(branch))
            return (true);
    }

    return (false);
}

void TravelCache::save( FILE *file ) const
{
    // Travel cache version information
    writeByte( file, TC_MAJOR_VERSION );
    writeByte( file, TC_MINOR_VERSION );

    // How many levels do we have?
    writeShort( file, levels.size() );

    // Save all the levels we have
    std::map<level_id, LevelInfo, level_id::less_than>::const_iterator i =
                levels.begin();

    for ( ; i != levels.end(); ++i)
    {
        i->first.save(file);
        i->second.save(file);
    }

    for (int wp = 0; wp < TRAVEL_WAYPOINT_COUNT; ++wp)
        waypoints[wp].save(file);
}

void TravelCache::load(FILE *file)
{
    levels.clear();

    // Check version. If not compatible, we just ignore the file altogether.
    unsigned char major = readByte(file),
                  minor = readByte(file);

    if (major != TC_MAJOR_VERSION || minor != TC_MINOR_VERSION)
        return ;

    int level_count = readShort(file);
    for (int i = 0; i < level_count; ++i)
    {
        level_id id;
        id.load(file);

        LevelInfo linfo;
        linfo.load(file);

        levels[id] = linfo;
    }

    for (int wp = 0; wp < TRAVEL_WAYPOINT_COUNT; ++wp)
        waypoints[wp].load(file);
}

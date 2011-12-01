/*
 *  File:       misc.cc
 *  Summary:    Misc functions.
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 *   <3>   11/14/99      cdl    evade with random40(ev) vice random2(ev)
 *   <2>    5/20/99      BWR    Multi-user support, new berserk code.
 *   <1>    -/--/--      LRH    Created
 */


#include "AppHdr.h"
#include "misc.h"

#include <string.h>
#if !(defined(__IBMCPP__) || defined(__BCPLUSPLUS__))
#include <unistd.h>
#endif

#ifdef __MINGW32__
#include <io.h>
#endif

#include <stdlib.h>
#include <stdio.h>

#ifdef DOS
#include <conio.h>
#endif

#include <stdarg.h>

#include "globals.h"
#include "externs.h"

#include "cloud.h"
#include "delay.h"
#include "dungeon.h"
#include "effects.h"
#include "fight.h"
#include "files.h"
#include "food.h"
#include "it_use2.h"
#include "items.h"
#include "itemprop.h"
#include "lev-pand.h"
#include "macro.h"
#include "monplace.h"
#include "mon-util.h"
#include "monstuff.h"
#include "ouch.h"
#include "player.h"
#include "shopping.h"
#include "skills.h"
#include "skills2.h"
#include "spells3.h"
#include "spl-cast.h"
#include "stuff.h"
#include "transfor.h"
#include "travel.h"
#include "view.h"


static bool scramble(void);

static void dart_trap( bool trap_known, int trap_id, struct bolt &pbolt,
                       bool poison );

// Clockwise, around the compass from north (same order as enum RUN_DIR)
static const struct coord_def Compass[8] =
{
    {  0, -1 }, {  1, -1 }, {  1,  0 }, {  1,  1 },
    {  0,  1 }, { -1,  1 }, { -1,  0 }, { -1, -1 },
};

run_dir_type convert_to_compass( int dx, int dy )
{
    int ret = RDIR_NONDIR;

    for (int i = 0; i < 8; i++)
    {
        if (Compass[i].x == dx && Compass[i].y == dy)
        {
            ret = i;
            break;
        }
    }

    return (static_cast< run_dir_type >( ret ));
}

// Set up the front facing array for detecting terrain based stops
static void set_run_check( int index, int dir )
{
    you.run_check[index].dx = Compass[dir].x;
    you.run_check[index].dy = Compass[dir].y;

    const int targ_x = you.x_pos + Compass[dir].x;
    const int targ_y = you.y_pos + Compass[dir].y;

    you.run_check[index].grid = grid_base_type( targ_x, targ_y );
}

bool check_naked_ok()
{
    if (!you.naked_okay
        && you.equip[EQ_BODY_ARMOUR] == -1
        && transform_can_equip_type( EQ_BODY_ARMOUR ))
    {
        mpr( MSGCH_WARN, "You are currently %s around %s!",
              (player_is_levitating()) ? "flying" :
              (player_is_swimming())   ? "swimming"
                                       : "running",
              (one_chance_in(10)) ? "naked" : "without body armour" );

        if (!yesno( "Are you sure you want to do this?" ))
            return (false);

        you.naked_okay = true;
    }

    return (true);
}

// Set up the running variables for the current run.
void start_running( run_dir_type dir, run_mode_type mode )
{
    if (dir == RDIR_REST)
    {
        you.run_x = 0;
        you.run_y = 0;
        you.running = mode;
        return;
    }

    ASSERT( dir >= 0 && dir <= 7 );

    if (!check_naked_ok())
        return;

    you.run_x = Compass[dir].x;
    you.run_y = Compass[dir].y;
    you.running = mode;

    // Get the compass point to the left/right of intended travel:
    const int left  = (dir - 1 < 0) ? 7 : (dir - 1);
    const int right = (dir + 1 > 7) ? 0 : (dir + 1);

    // Record the direction and starting tile type for later reference:
    set_run_check( RCHECK_LEFT, left );
    set_run_check( RCHECK_FRONT, dir );
    set_run_check( RCHECK_RIGHT, right );
}

void stop_running( void )
{
    you.running = RMODE_NOT_RUNNING;
}

bool is_resting( void )
{
    return (you.running && !you.run_x && !you.run_y);
}

// Returns true if one of the front three grids has changed.
bool check_stop_running( void )
{
    if (is_cloud( you.x_pos + you.run_x, you.y_pos + you.run_y ))
        return (true);

    if (mgrd[you.x_pos + you.run_x][you.y_pos + you.run_y] != NON_MONSTER)
        return (true);

    for (int i = 0; i < 3; i++)
    {
        const int targ_x = you.x_pos + you.run_check[i].dx;
        const int targ_y = you.y_pos + you.run_check[i].dy;
        const int targ_grid = grid_base_type( targ_x, targ_y );

        if (you.run_check[i].grid != targ_grid)
            return (true);
    }

    return (false);
}

static armour_type get_hide_type( int mons_class )
{
    armour_type  hide = ARM_NONE;

    switch (mons_class)
    {
    case MONS_DRAGON:
        hide = ARM_DRAGON_HIDE;
        break;
    case MONS_TROLL:
        hide = ARM_TROLL_HIDE;
        break;
    case MONS_ICE_DRAGON:
        hide = ARM_ICE_DRAGON_HIDE;
        break;
    case MONS_STEAM_DRAGON:
        hide = ARM_STEAM_DRAGON_HIDE;
        break;
    case MONS_MOTTLED_DRAGON:
        hide = ARM_MOTTLED_DRAGON_HIDE;
        break;
    case MONS_STORM_DRAGON:
        hide = ARM_STORM_DRAGON_HIDE;
        break;
    case MONS_GOLDEN_DRAGON:
        hide = ARM_GOLD_DRAGON_HIDE;
        break;
    case MONS_SWAMP_DRAGON:
        hide = ARM_SWAMP_DRAGON_HIDE;
        break;
    default:
        break;
    }

    return (hide);
}

void turn_corpse_into_chunks( item_def &corpse )
{
    const int mons_class = corpse.plus;
    const int max_chunks = mons_weight( mons_class ) / 150;

    ASSERT( corpse.base_type == OBJ_CORPSES );

    corpse.base_type = OBJ_FOOD;
    corpse.sub_type = FOOD_CHUNK;
    corpse.plus2 = 0;
    corpse.quantity = 1 + random2( max_chunks );

    corpse.quantity = stepdown_value( corpse.quantity, 4, 2, 10, 12 );

    // seems to me that this should come about only
    // after the corpse has been butchered ... {dlb}

    armour_type  hide = get_hide_type( mons_class );
    if (hide != ARM_NONE && one_chance_in(3))
    {
        int o = get_item_slot( 100 + random2(200) );
        if (o == NON_ITEM)
            return;

        mitm[o].quantity = 1;
        mitm[o].base_type = OBJ_ARMOUR;
        mitm[o].sub_type = hide;
        mitm[o].colour = corpse.colour;   // always matches the input corpse

        mitm[o].plus = 0;
        mitm[o].plus2 = 0;
        mitm[o].special = 0;
        mitm[o].flags = 0;

        move_item_to_grid( &o, corpse.x, corpse.y );
    }
}                               // end place_chunks()

bool grid_is_wall( unsigned char grid )
{
    return (grid == DNGN_ROCK_WALL
            || grid == DNGN_STONE_WALL
            || grid == DNGN_METAL_WALL
            || grid == DNGN_GREEN_CRYSTAL_WALL
            || grid == DNGN_WAX_WALL
            || grid == DNGN_PERMAROCK_WALL);
}

bool grid_is_opaque( unsigned char grid )
{
    return (grid < MINSEE && grid != DNGN_ORCISH_IDOL);
}

bool grid_is_solid( unsigned char grid )
{
    return (grid < MINMOVE);
}

bool grid_is_water( unsigned char grid )
{
    return (grid == DNGN_SHALLOW_WATER || grid == DNGN_DEEP_WATER);
}

bool grid_destroys_items( unsigned char grid )
{
    return (grid == DNGN_LAVA || grid == DNGN_DEEP_WATER);
}

// returns 0 is grid is not an altar, else it returns the GOD_* type
god_type grid_altar_god( unsigned char grid )
{
    if (grid >= DNGN_ALTAR_ZIN && grid <= DNGN_ALTAR_ELYVILON)
        return (static_cast<god_type>( grid - DNGN_ALTAR_ZIN + 1 ));

    return (GOD_NO_GOD);
}

bool grid_is_branch_stairs( unsigned char grid )
{
    return ((grid >= DNGN_ENTER_ORCISH_MINES && grid <= DNGN_ENTER_RESERVED_4)
            || (grid >= DNGN_ENTER_DIS && grid <= DNGN_ENTER_TARTARUS));
}


unsigned char grid_secret_door_appearance( int gx, int gy )
{
    unsigned char ret = DNGN_FLOOR;

    for (int dx = -1; dx <= 1; dx++)
    {
        for (int dy = -1; dy <= 1; dy++)
        {
            // only considering orthagonal grids
            if ((abs(dx) + abs(dy)) % 2 == 0)
                continue;

            const int targ = grd[gx + dx][gy + dy];

            if (!grid_is_wall( targ ))
                continue;

            if (ret == DNGN_FLOOR)
                ret = targ;
            else if (ret != targ)
                ret = ((ret < targ) ? ret : targ);
        }
    }

    return ((ret == DNGN_FLOOR) ? static_cast<unsigned char>( DNGN_ROCK_WALL )
                                : ret);
}

// This function creates "equivalence classes" so that undiscovered
// traps and secret doors aren't run stopping points.
unsigned char grid_base_type( int x, int y )
{
    const unsigned char grid = grd[x][y];

    // Don't stop for undiscovered traps:
    if (grid == DNGN_UNDISCOVERED_TRAP)
        return (DNGN_FLOOR);

    // Or secret doors (which currently always look like rock walls):
    if (grid == DNGN_SECRET_DOOR)
        return (grid_secret_door_appearance( x, y ));

    return (grid);
}

// A constructor for bolt to help guarantee that we start clean (this has
// caused way too many bugs).  Putting it here since there's no good place to
// put it, and it doesn't do anything other than initialize it's members.
//
// TODO: Eventually it'd be nice to have a proper factory for these things
// (extended from setup_mons_cast() and zapping() which act as limited ones).
bolt::bolt() : range(0), rangeMax(0), type(SYM_ZAP), colour(BLACK),
               flavour(BEAM_MAGIC), source_x(0), source_y(0), damage(0,0),
               ench_power(0), hit(0), target_x(0), target_y(0),
               thrower(KILL_MISC), ex_size(0), beam_source(MHITNOT), name(),
               is_beam(false), is_explosion(false), is_big_cloud(false),
               is_enchant(false), is_energy(false),
               is_launched(false), is_thrown(false), target_first(false),
               aux_source(NULL), obvious_effect(false),
               fr_count(0), foe_count(0), fr_power(0), foe_power(0),
               is_tracer(false), aimed_at_feet(false), msg_generated(false),
               in_explosion_phase(false), smart_monster(false),
               can_see_invis(false), is_friendly(false), foe_ratio(0)
{ }

static int calc_search_skill( void )
{
    int ret = 5 + 2 * skill_bump( SK_TRAPS_DOORS )
                + 5 * you.mutation[MUT_ACUTE_VISION];

    if (you.mutation[MUT_BLURRY_VISION] > 0)
        ret = ret / (1 + you.mutation[MUT_BLURRY_VISION]);

    return (ret);
}

// returns true only if we discovered something in front of us while running.
void search_around(void)
{
    int i;
    bool whoa_nelly = false; // used to rein in a running player
    int found_doors = 0;
    int found_traps = 0;
    int prac = 0;

    // Never if doing something else... this prevents a slight asymetry
    // where using autopickup was giving free searches in comparison to
    // not using autopickup.  -- bwr
    if (you_are_delayed() || !player_sound_mind_and_body())
        return;

    const int skill = calc_search_skill();

    for (int x = -1; x <= 1; x++)
    {
        for (int y = -1; y <= 1; y++)
        {
            const int srx = you.x_pos + x;
            const int sry = you.y_pos + y;

            // don't exclude own square; may be levitating over trap
            if (!in_bounds( srx, sry ))
                continue;

            if (grd[srx][sry] == DNGN_SECRET_DOOR && random2(100) < skill)
            {
                grd[srx][sry] = DNGN_CLOSED_DOOR;

                found_doors++;
                prac += (coinflip() ? 2 : 3);
            }

            if (grd[srx][sry] == DNGN_UNDISCOVERED_TRAP && random2(100) < skill)
            {
                i = trap_at_xy( srx, sry );

                if (i != -1)
                    grd[srx][sry] = trap_category( env.trap[i].type );

                // Must read running vars before calling mpr() as messages
                // always stop running.
                if (you.running && x == you.run_x && y == you.run_y)
                    whoa_nelly = true;

                found_traps++;
                prac += (coinflip() ? 2 : 3);
            }
        }
    }

    if (found_doors)
    {
        mpr( "You found %s secret door%s!",
              (found_doors > 1) ? "some" : "a",
              (found_doors > 1) ? "s" : "" );

    }

    if (found_traps)
    {
        mpr( ((whoa_nelly) ? MSGCH_WARN : MSGCH_PLAIN),
                "You found %s trap%s!",
                  (found_doors > 1) ? "some" : "a",
                  (found_doors > 1) ? "s" : "" );

        // Try to stop a player who's running from just running onto a
        // trap they've just discovered in front of themselves.
        if (whoa_nelly)
            more();
    }

    if (prac)
        exercise( SK_TRAPS_DOORS, prac );

    return;
}                               // end search_around()

int adjust_time_for_speed( int time_taken, speed_type speed )
{
    ASSERT( time_taken > 0 );

    int ret = time_taken;

    // currently limiting the effect to +/- one action per round.
    if (speed == SPEED_SLOWED)
    {
        if (time_taken >= 5)
            ret *= 2;
        else
        {
            double actions = 10.0 / static_cast< double >( time_taken );
            actions -= 1.0;
            ret = conv_double_rand_round( 10.0 / actions );
        }
    }
    else if (speed == SPEED_HASTED)
    {
        if (time_taken >= 10)
            ret = div_rand_round( time_taken, 2 );
        else
        {
            double actions = 10.0 / static_cast< double >( time_taken );
            actions += 1.0;
            ret = conv_double_rand_round( 10.0 / actions );
        }
    }

    return (ret);
}

int adjust_energy_for_speed( int energy, speed_type speed )
{
    // since energy is the inverse of time ticks:
    if (speed == SPEED_SLOWED)
        speed = SPEED_HASTED;
    else if (speed == SPEED_HASTED)
        speed = SPEED_SLOWED;

    return (adjust_time_for_speed( energy, speed ));
}

void in_a_cloud(void)
{
    const int cid = env.cgrid[ you.x_pos ][ you.y_pos ];
    const int ctype = get_cloud_type( you.x_pos, you.y_pos );
    int hurted = 0;
    int resist;

    // any cloud disrupts the condensation shield:
    if (you.duration[DUR_CONDENSATION_SHIELD] > 0)
    {
        mpr(MSGCH_DURATION,"Your icy shield dissipates!" );
        you.duration[DUR_CONDENSATION_SHIELD] = 0;
        set_redraw_status( REDRAW_ARMOUR_CLASS );
    }

    switch (ctype)
    {
    case CLOUD_FIRE:
    case CLOUD_FIRE_MON:
        if (you.fire_shield)
            return;

        mpr("You are engulfed in roaring flames!");

        resist = player_res_fire();

        if (resist <= 0)
        {
            hurted += (roll_dice(3,7) + 10) * you.time_taken;

            if (resist < 0)
                hurted += (roll_dice(2,7) + 1) * you.time_taken;

            hurted /= 10;
        }
        else
        {
            canned_msg(MSG_YOU_RESIST);

            hurted += ((roll_dice(3,7) + 10) * you.time_taken) / 10;
            hurted /= (1 + resist * resist);
        }

        hurted -= random2( player_armour_class() );

        if (hurted > 0)
            ouch( hurted, cid, KILLED_BY_CLOUD, "flame" );

        expose_player_to_element( BEAM_FIRE, 10 );
        break;

    case CLOUD_COLD:
    case CLOUD_COLD_MON:
        mpr("You are engulfed in freezing vapours!");

        resist = player_res_cold();

        if (resist <= 0)
        {
            hurted += (roll_dice(3,7) + 10) * you.time_taken;

            if (resist < 0)
                hurted += (roll_dice(2,7) + 1) * you.time_taken;

            hurted /= 10;
        }
        else
        {
            canned_msg(MSG_YOU_RESIST);
            hurted += ((roll_dice(3,7) + 10) * you.time_taken) / 10;
            hurted /= (1 + resist * resist);
        }

        hurted -= random2( player_armour_class() );

        if (hurted > 0)
            ouch( hurted, cid, KILLED_BY_CLOUD, "freezing vapour" );

        expose_player_to_element( BEAM_COLD, 10 );
        break;

    case CLOUD_STINK:
    case CLOUD_STINK_MON:
        // If you don't have to breathe, unaffected
        mpr("You are engulfed in noxious fumes!");
        if (player_res_poison() > 0)  // weak poison
            break;

        hurted += (random2(3) * you.time_taken) / 10;
        if (hurted < 1)
            hurted = 0;
        else
            ouch( (hurted * you.time_taken) / 10, cid, KILLED_BY_CLOUD,
                    "noxious fumes" );

        if (1 + random2(27) >= you.xp_level)
        {
            mpr("You choke on the stench!");
            confuse_player( (coinflip() ? 3 : 2) );
        }
        break;

    case CLOUD_POISON:
    case CLOUD_POISON_MON:
        // If you don't have to breathe, unaffected
        mpr("You are engulfed in poison gas!");

        resist = player_res_poison();
        if (resist < 3)
        {
            ouch( (random2(20 - resist * 5) * you.time_taken) / 10, cid,
                    KILLED_BY_CLOUD, "poison gas" );

            poison_player( roll_dice( 1, 3 - resist ) );
        }
        break;

    case CLOUD_GREY_SMOKE:
    case CLOUD_BLUE_SMOKE:
    case CLOUD_PURP_SMOKE:
    case CLOUD_BLACK_SMOKE:
    case CLOUD_GREY_SMOKE_MON:
    case CLOUD_BLUE_SMOKE_MON:
    case CLOUD_PURP_SMOKE_MON:
    case CLOUD_BLACK_SMOKE_MON:
        mpr("You are engulfed in a cloud of smoke!");
        break;

    case CLOUD_STEAM:
    case CLOUD_STEAM_MON:
        mpr("You are engulfed in a cloud of scalding steam!");

        if ((you.species == SP_PALE_DRACONIAN && you.xp_level >= 7)
            || player_equip( EQ_BODY_ARMOUR, ARM_STEAM_DRAGON_ARMOUR ))
        {
            mpr("It doesn't seem to affect you.");
            return;
        }

        hurted += (random2(6) * you.time_taken) / 10;
        if (hurted < 0 || player_res_fire() > 0)
            hurted = 0;

        ouch( (hurted * you.time_taken) / 10, cid, KILLED_BY_CLOUD, "poison gas" );
        break;

    case CLOUD_MIASMA:
    case CLOUD_MIASMA_MON:
        mpr("You are engulfed in a dark miasma.");

        if (player_prot_life() > random2(3))
            return;

        if (player_res_poison() <= 0)
            poison_player(1);

        if (you.hp_max > 4 && player_prot_life() <= 0 && coinflip())
            rot_hp(1);

        hurted += (roll_dice(3,4) * you.time_taken) / 10;

        if (hurted > 0)
            ouch( hurted, cid, KILLED_BY_CLOUD, "foul pestilence" );

        if (one_chance_in( 3 + 2 * player_prot_life() ))
            potion_effect( POT_SLOWING, 5 );
        break;
    }

    return;
}                               // end in_a_cloud()

static void warn_teleport_control_zone( void )
{
    if (allow_control_teleport( true ))
        return;

    if (you.skills[SK_TRANSLOCATIONS] > 0)
    {
        mpr( MSGCH_WARN, "You sense a powerful magical force %s.",
              player_has_spell( SPELL_CONTROL_TELEPORT )
                    ? "preventing teleport control" : "warping space" );
    }

    if (you.equip[EQ_LEFT_RING] != -1
        && you.inv[you.equip[EQ_LEFT_RING]].sub_type == RING_TELEPORT_CONTROL)
    {
        mpr( MSGCH_WARN, "Your left ring glows red." );
    }

    if (you.equip[EQ_RIGHT_RING] != -1
        && you.inv[you.equip[EQ_RIGHT_RING]].sub_type == RING_TELEPORT_CONTROL)
    {
        mpr( MSGCH_WARN, "Your right ring glows red." );
    }
}

void up_stairs( unsigned char stair_used, bool real_stairs )
{
    char old_where = you.where_are_you;
    bool was_a_labyrinth = false;

    if (stair_used == DNGN_ENTER_SHOP)
    {
        shop();
        return;
    }

    // probably still need this check here (teleportation) -- bwr
    if ((stair_used < DNGN_STONE_STAIRS_UP_I
            || stair_used > DNGN_ROCK_STAIRS_UP)
        && (stair_used < DNGN_RETURN_FROM_ORCISH_MINES || stair_used >= 150)
        && stair_used != DNGN_EXIT_HELL)
    {
        mpr("You can't go up here.");
        return;
    }

    // the up stairs in hell are destinations only
    if (player_in_hell()
        && stair_used >= DNGN_STONE_STAIRS_UP_I
        && stair_used <= DNGN_ROCK_STAIRS_UP)
    {
        mpr("A mysterious force prevents you from ascending the staircase.");
        return;
    }

    // Since the overloaded message set turn_is_over, I'm assuming that
    // the overloaded character makes an attempt... so we're doing this
    // check before that one. -- bwr
    if (real_stairs
        && !player_is_levitating()
        && you.conf
        && (stair_used >= DNGN_STONE_STAIRS_UP_I
            && stair_used <= DNGN_ROCK_STAIRS_UP)
        && random2(you.dex) < 4)
    {
        mpr("In your confused state, you trip and fall back down the stairs.");

        ouch( roll_dice( 3 + you.burden_state, 5 ), 0,
              KILLED_BY_FALLING_DOWN_STAIRS );

        you.turn_is_over = true;
        return;
    }

    if (real_stairs && you.burden_state == BS_OVERLOADED)
    {
        mpr("You are carrying too much to climb upwards.");
        you.turn_is_over = true;
        return;
    }

    if (you.depth == 0
        && !yesno("Are you sure you want to leave the Dungeon?", false))
    {
        mpr("Alright, then stay!");
        return;
    }

    int old_level  = you.depth;

    // Zero out last travel coords
    you.run_x = you.run_y = 0;

    // Interlevel travel data:
    const bool collect_travel_data = (you.level_type != LEVEL_LABYRINTH
                                        && you.level_type != LEVEL_ABYSS
                                        && you.level_type != LEVEL_PANDEMONIUM);

    level_id  old_level_id    = level_id::get_current_level_id();
    LevelInfo &old_level_info = travel_cache.get_level_info(old_level_id);
    int stair_x = you.x_pos, stair_y = you.y_pos;

    if (collect_travel_data)
        old_level_info.update();

    // Make sure we return to our main dungeon level... labyrinth entrances
    // in the abyss or pandemonium a bit trouble (well the labyrinth does
    // provide a way out of those places, its really not that bad I suppose)
    if (you.level_type == LEVEL_LABYRINTH)
    {
        you.level_type = LEVEL_DUNGEON;
        was_a_labyrinth = true;
    }

    you.depth--;

    int i = 0;

    if (you.depth < 0)
    {
        mpr("You have escaped!");

        for (i = 0; i < ENDOFPACK; i++)
        {
            if (is_valid_item( you.inv[i] )
                && you.inv[i].base_type == OBJ_ORBS)
            {
                ouch( INSTANT_DEATH, 0, KILLED_BY_WINNING );
            }
        }

        ouch( INSTANT_DEATH, 0, KILLED_BY_LEAVING );
    }

    mpr("Entering...");
    you.prev_targ = MHITNOT;
    you.pet_target = MHITNOT;

    if (player_in_branch( BRANCH_VESTIBULE_OF_HELL ))
    {
        mpr("Thank you for visiting Hell. Please come again soon.");
        you.where_are_you = BRANCH_MAIN_DUNGEON;
        you.depth = you.hell_exit;
        stair_used = DNGN_STONE_STAIRS_UP_I;
    }

    switch (stair_used)
    {
    case DNGN_EXIT_HELL:
        mpr("Welcome back to the Vestibule!");
        you.where_are_you = BRANCH_VESTIBULE_OF_HELL;
        you.depth = 27;
        break;
    case DNGN_RETURN_FROM_ORCISH_MINES:
    case DNGN_RETURN_FROM_HIVE:
    case DNGN_RETURN_FROM_LAIR:
    case DNGN_RETURN_FROM_VAULTS:
    case DNGN_RETURN_FROM_TEMPLE:
    case DNGN_RETURN_FROM_ZOT:
        mpr("Welcome back to the Dungeon!");
        you.where_are_you = BRANCH_MAIN_DUNGEON;
        break;
    case DNGN_RETURN_FROM_SLIME_PITS:
    case DNGN_RETURN_FROM_SNAKE_PIT:
    case DNGN_RETURN_FROM_SWAMP:
        mpr("Welcome back to the Lair of Beasts!");
        you.where_are_you = BRANCH_LAIR;
        break;
    case DNGN_RETURN_FROM_CRYPT:
    case DNGN_RETURN_FROM_HALL_OF_BLADES:
        mpr("Welcome back to the Vaults!");
        you.where_are_you = BRANCH_VAULTS;
        break;
    case DNGN_RETURN_FROM_TOMB:
        mpr("Welcome back to the Crypt!");
        you.where_are_you = BRANCH_CRYPT;
        break;
    case DNGN_RETURN_FROM_ELVEN_HALLS:
        mpr("Welcome back to the Orcish Mines!");
        you.where_are_you = BRANCH_ORCISH_MINES;
        break;
    }

    if (!player_is_levitating())
        mpr("You climb upwards.");
    else
    {
        if (you.duration[DUR_CONTROLLED_FLIGHT])
            mpr("You fly upwards.");
        else
            mpr("You float upwards... And bob straight up to the ceiling!");
    }

    unsigned char stair_taken = stair_used;

    load(stair_taken, LOAD_ENTER_LEVEL, was_a_labyrinth, old_level, old_where);

    you.turn_is_over = true;

    save_game( false );

    enter_new_level();

    viewwindow( true, true );

    warn_teleport_control_zone();

    if (collect_travel_data)
    {
        // Update stair information for the stairs we just ascended, and the
        // down stairs we're currently on.
        level_id  new_level_id    = level_id::get_current_level_id();

        if (you.level_type != LEVEL_PANDEMONIUM
            && you.level_type != LEVEL_ABYSS
            && you.level_type != LEVEL_LABYRINTH)
        {
            LevelInfo &new_level_info = travel_cache.get_level_info(new_level_id);
            new_level_info.update();

            // First we update the old level's stair.
            level_pos lp;
            lp.id  = new_level_id;
            lp.pos.x = you.x_pos;
            lp.pos.y = you.y_pos;

            bool guess = false;
            // Ugly hack warning:
            // The stairs in the Vestibule of Hell exhibit special behaviour:
            // they always lead back to the dungeon level that the player
            // entered the Vestibule from. This means that we need to pretend
            // we don't know where the upstairs from the Vestibule go each time
            // we take it. If we don't, interlevel travel may try to use portals
            // to Hell as shortcuts between dungeon levels, which won't work,
            // and will confuse the dickens out of the player (well, it confused
            // the dickens out of me when it happened).
            if (new_level_id.branch == BRANCH_MAIN_DUNGEON
                && old_level_id.branch == BRANCH_VESTIBULE_OF_HELL)
            {
                lp.id.depth = -1;
                lp.pos.x = lp.pos.y = -1;
                guess = true;
            }

            old_level_info.update_stair( stair_x, stair_y, lp, guess );

            // We *guess* that going up a staircase lands us on a downstair,
            // and that we can descend that downstair and get back to where we
            // came from. This assumption is guaranteed false when climbing out
            // of one of the branches of Hell.
            if (new_level_id.branch != BRANCH_VESTIBULE_OF_HELL)
            {
                // Set the new level's stair, assuming arbitrarily that going
                // downstairs will land you on the same upstairs you took to
                // begin with (not necessarily true).
                lp.id = old_level_id;
                lp.pos.x = stair_x;
                lp.pos.y = stair_y;
                new_level_info.update_stair( you.x_pos, you.y_pos, lp, true );
            }
        }
    }
}                               // end up_stairs()

void down_stairs( unsigned char stair_used, int old_level, bool real_stairs )
{
    int i;
    char old_level_type = you.level_type;
    bool was_a_labyrinth = false;

    //int old_level = you.depth;
    // bool leave_abyss_pan = false;
    char old_where = you.where_are_you;

    // probably still need this check here (teleportation) -- bwr
    if ((stair_used < DNGN_ENTER_LABYRINTH
            || stair_used > DNGN_ROCK_STAIRS_DOWN)
        && stair_used != DNGN_ENTER_HELL
        && ((stair_used < DNGN_ENTER_DIS
                || stair_used > DNGN_TRANSIT_PANDEMONIUM)
            && stair_used != DNGN_STONE_ARCH)
        && !(stair_used >= DNGN_ENTER_ORCISH_MINES
            && stair_used < DNGN_RETURN_FROM_ORCISH_MINES))
    {
        mpr( "You can't go down here!" );
        return;
    }

    // the stairs in the vestibule are destinations only
    if (player_in_branch( BRANCH_VESTIBULE_OF_HELL )
        && stair_used >= DNGN_STONE_STAIRS_DOWN_I
        && stair_used <= DNGN_ROCK_STAIRS_DOWN)
    {
        mpr("A mysterious force prevents you from descending the staircase.");
        return;
    }

    if (stair_used == DNGN_STONE_ARCH)
    {
        mpr("You can't go down here!");
        return;
    }

    if (real_stairs && !player_access_floor())
    {
        mpr("You're floating high up above the floor!");
        return;
    }

    if (stair_used == DNGN_ENTER_ZOT)
    {
        int num_runes = 0;

        for (i = 0; i < ENDOFPACK; i++)
        {
            if (is_valid_item( you.inv[i] )
                && you.inv[i].base_type == OBJ_MISCELLANY
                && you.inv[i].sub_type == MISC_RUNE_OF_ZOT)
            {
                num_runes += you.inv[i].quantity;
            }
        }

        if (num_runes < NUMBER_OF_RUNES_NEEDED)
        {
            mpr( "You need %s Rune%s to enter this place.",
                  number_string( NUMBER_OF_RUNES_NEEDED ),
                  (NUMBER_OF_RUNES_NEEDED != 1) ? "s" : "" );
            return;
        }
    }

    // Zero out last travel coords
    you.run_x = you.run_y = 0;

    // Interlevel travel data:
    const bool collect_travel_data = (you.level_type != LEVEL_LABYRINTH
                                        && you.level_type != LEVEL_ABYSS
                                        && you.level_type != LEVEL_PANDEMONIUM);

    level_id  old_level_id    = level_id::get_current_level_id();
    LevelInfo &old_level_info = travel_cache.get_level_info(old_level_id);
    int stair_x = you.x_pos, stair_y = you.y_pos;

    if (collect_travel_data)
        old_level_info.update();

    if (you.level_type == LEVEL_PANDEMONIUM
        && stair_used == DNGN_TRANSIT_PANDEMONIUM)
    {
        was_a_labyrinth = true;
    }
    else
    {
        if (you.level_type != LEVEL_DUNGEON)
            was_a_labyrinth = true;

        you.level_type = LEVEL_DUNGEON;
    }

    mpr("Entering...");
    you.prev_targ = MHITNOT;
    you.pet_target = MHITNOT;

    if (stair_used == DNGN_ENTER_HELL)
    {
        you.where_are_you = BRANCH_VESTIBULE_OF_HELL;
        you.hell_exit = you.depth;

        mpr("Welcome to Hell!");
        mpr("Please enjoy your stay.");
        you.depth = 26;    // = 59;
    }

    if ((stair_used >= DNGN_ENTER_DIS
            && stair_used <= DNGN_ENTER_TARTARUS)
        || (stair_used >= DNGN_ENTER_ORCISH_MINES
            && stair_used < DNGN_RETURN_FROM_ORCISH_MINES))
    {
        // no idea why such a huge switch and not 100-grd[][]
        // planning ahead for re-organizaing grd[][] values - 13jan2000 {dlb}
        strcpy( info, "Welcome to " );
        switch (stair_used)
        {
        case DNGN_ENTER_DIS:
            strcat(info, "the Iron City of Dis!");
            you.where_are_you = BRANCH_DIS;
            you.depth = 26;
            break;
        case DNGN_ENTER_GEHENNA:
            strcat(info, "Gehenna!");
            you.where_are_you = BRANCH_GEHENNA;
            you.depth = 26;
            break;
        case DNGN_ENTER_COCYTUS:
            strcat(info, "Cocytus!");
            you.where_are_you = BRANCH_COCYTUS;
            you.depth = 26;
            break;
        case DNGN_ENTER_TARTARUS:
            strcat(info, "Tartarus!");
            you.where_are_you = BRANCH_TARTARUS;
            you.depth = 26;
            break;
        case DNGN_ENTER_ORCISH_MINES:
            strcat(info, "the Orcish Mines!");
            you.where_are_you = BRANCH_ORCISH_MINES;
            break;
        case DNGN_ENTER_HIVE:
            strcpy(info, "You hear a buzzing sound coming from all directions.");
            you.where_are_you = BRANCH_HIVE;
            break;
        case DNGN_ENTER_LAIR:
            strcat(info, "the Lair of Beasts!");
            you.where_are_you = BRANCH_LAIR;
            break;
        case DNGN_ENTER_SLIME_PITS:
            strcat(info, "the Pits of Slime!");
            you.where_are_you = BRANCH_SLIME_PITS;
            break;
        case DNGN_ENTER_VAULTS:
            strcat(info, "the Vaults!");
            you.where_are_you = BRANCH_VAULTS;
            break;
        case DNGN_ENTER_CRYPT:
            strcat(info, "the Crypt!");
            you.where_are_you = BRANCH_CRYPT;
            break;
        case DNGN_ENTER_HALL_OF_BLADES:
            strcat(info, "the Hall of Blades!");
            you.where_are_you = BRANCH_HALL_OF_BLADES;
            break;
        case DNGN_ENTER_ZOT:
            strcat(info, "the Hall of Zot!");
            you.where_are_you = BRANCH_HALL_OF_ZOT;
            break;
        case DNGN_ENTER_TEMPLE:
            strcat(info, "the Ecumenical Temple!");
            you.where_are_you = BRANCH_ECUMENICAL_TEMPLE;
            break;
        case DNGN_ENTER_SNAKE_PIT:
            strcat(info, "the Snake Pit!");
            you.where_are_you = BRANCH_SNAKE_PIT;
            break;
        case DNGN_ENTER_ELVEN_HALLS:
            strcat(info, "the Elven Halls!");
            you.where_are_you = BRANCH_ELVEN_HALLS;
            break;
        case DNGN_ENTER_TOMB:
            strcat(info, "the Tomb!");
            you.where_are_you = BRANCH_TOMB;
            break;
        case DNGN_ENTER_SWAMP:
            strcat(info, "the Swamp!");
            you.where_are_you = BRANCH_SWAMP;
            break;
        }

        mpr(info);
    }
    else if (stair_used == DNGN_ENTER_LABYRINTH)
    {
        you.level_type = LEVEL_LABYRINTH;
        grd[you.x_pos][you.y_pos] = DNGN_FLOOR;
    }
    else if (stair_used == DNGN_ENTER_ABYSS)
    {
        you.level_type = LEVEL_ABYSS;
    }
    else if (stair_used == DNGN_ENTER_PANDEMONIUM)
    {
        you.level_type = LEVEL_PANDEMONIUM;
    }

    if (you.level_type == LEVEL_LABYRINTH
        || you.level_type == LEVEL_ABYSS
        || you.level_type == LEVEL_PANDEMONIUM)
    {
        char glorpstr[kFileNameSize];
        char del_file[kFileNameSize];
        int sysg;

#ifdef SAVE_DIR_PATH
        snprintf( glorpstr, sizeof(glorpstr),
                  SAVE_DIR_PATH "%s-%d", you.your_name,
                  static_cast<int>( getuid() ) );
#else
        strncpy(glorpstr, you.your_name, kFileNameLen);

        // glorpstr [strlen(glorpstr)] = 0;
        // This is broken. Length is not valid yet! We have to check if we got
        // a trailing NULL; if not, write one:
        /* is name 6 chars or more? */
        if (strlen(you.your_name) > kFileNameLen - 1)
            glorpstr[kFileNameLen] = '\0';
#endif

        strcpy(del_file, glorpstr);
        strcat(del_file, ".lab");

#ifdef DOS
        strupr(del_file);
#endif
        sysg = unlink(del_file);

#if DEBUG_DIAGNOSTICS
        strcpy( info, "Deleting: " );
        strcat( info, del_file );
        mpr( MSGCH_DIAGNOSTICS, info );
#endif
    }

    if (stair_used == DNGN_EXIT_ABYSS || stair_used == DNGN_EXIT_PANDEMONIUM)
    {
        // leave_abyss_pan = true;
        mpr("You pass through the gate.");
        more();
    }

    if (!player_is_levitating()
        && you.conf
        && (stair_used >= DNGN_STONE_STAIRS_DOWN_I
            && stair_used <= DNGN_ROCK_STAIRS_DOWN)
        && random2(you.dex) < 4)
    {
        mpr("In your confused state, you trip and fall down the stairs.");

        // Nastier than when climbing stairs, but you'll aways get to
        // your destination, -- bwr
        ouch( roll_dice( 6 + you.burden_state, 10 ), 0,
              KILLED_BY_FALLING_DOWN_STAIRS );
    }

    if (you.level_type == LEVEL_DUNGEON)
        you.depth++;

    int stair_taken = stair_used;

    if (you.level_type == LEVEL_LABYRINTH || you.level_type == LEVEL_ABYSS)
        stair_taken = DNGN_FLOOR;

    if (you.level_type == LEVEL_PANDEMONIUM)
        stair_taken = DNGN_TRANSIT_PANDEMONIUM;

    switch (you.level_type)
    {
    case LEVEL_LABYRINTH:
        if (one_chance_in(10))
            mpr("You enter a maze of twisty passages, all alike.");
        else
            mpr("You enter a dark and forbidding labyrinth.");
        break;

    case LEVEL_ABYSS:
        mpr("You enter the Abyss!");
        mpr("To return, you must find a gate leading back.");
        break;

    case LEVEL_PANDEMONIUM:
        if (old_level_type == LEVEL_PANDEMONIUM)
            mpr("You pass into a different region of Pandemonium.");
        else
        {
            mpr("You enter the halls of Pandemonium!");
            mpr("To return, you must find a gate leading back.");
        }
        break;

    default:
        mpr("You climb downwards.");
        break;
    }

    load(stair_taken, LOAD_ENTER_LEVEL, was_a_labyrinth, old_level, old_where);

    unsigned char pc = 0;
    unsigned char pt = roll_zdice(3,10);

    switch (you.level_type)
    {
    case LEVEL_LABYRINTH:
        if (old_level_type == LEVEL_DUNGEON)
            you.depth++;
        break;

    case LEVEL_ABYSS:
        grd[you.x_pos][you.y_pos] = DNGN_FLOOR;

        if (old_level_type == LEVEL_DUNGEON
            || old_level_type == LEVEL_LABYRINTH)
        {
            you.depth--;   // Linley-suggested addition 17jan2000 {dlb}
        }

        init_pandemonium();     /* colours only */

        if (player_in_hell())
        {
            you.where_are_you = BRANCH_MAIN_DUNGEON;
            you.depth = you.hell_exit - 1;
        }
        break;

    case LEVEL_PANDEMONIUM:
        if (old_level_type == LEVEL_PANDEMONIUM)
        {
            init_pandemonium();
            for (pc = 0; pc < pt; pc++)
                pandemonium_mons();
        }
        else
        {
            if (old_level_type == LEVEL_DUNGEON
                || old_level_type == LEVEL_LABYRINTH)
            {
                you.depth--;   // Linley-suggested addition 17jan2000 {dlb}
            }

            init_pandemonium();

            for (pc = 0; pc < pt; pc++)
                pandemonium_mons();

            if (player_in_hell())
            {
                you.where_are_you = BRANCH_MAIN_DUNGEON;
                you.hell_exit = 26;
                you.depth = 26;
            }
        }
        break;

    default:
        break;
    }

    you.turn_is_over = true;

    save_game( false );

    enter_new_level();

    viewwindow( true, true );

    warn_teleport_control_zone();

    if (collect_travel_data)
    {
        // Update stair information for the stairs we just descended, and the
        // upstairs we're currently on.
        level_id  new_level_id    = level_id::get_current_level_id();

        if (you.level_type != LEVEL_PANDEMONIUM
            && you.level_type != LEVEL_ABYSS
            && you.level_type != LEVEL_LABYRINTH)
        {
            LevelInfo &new_level_info = travel_cache.get_level_info(new_level_id);
            new_level_info.update();

            // First we update the old level's stair.
            level_pos lp;
            lp.id  = new_level_id;
            lp.pos.x = you.x_pos;
            lp.pos.y = you.y_pos;

            old_level_info.update_stair(stair_x, stair_y, lp);

            // Then the new level's stair, assuming arbitrarily that going
            // upstairs will land you on the same downstairs you took to begin
            // with (not necessarily true).
            lp.id = old_level_id;
            lp.pos.x = stair_x;
            lp.pos.y = stair_y;
            new_level_info.update_stair(you.x_pos, you.y_pos, lp, true);
        }
    }
}                               // end down_stairs()

void enter_new_level( void )
{
    int curr_subdungeon_level = you.depth + 1;

    textcolor( LIGHTGREY );

    // maybe last part better expresssed as <= PIT {dlb}
    if (player_in_hell() || player_in_branch( BRANCH_VESTIBULE_OF_HELL ))
        curr_subdungeon_level = you.depth - 26;

    /* Remember, must add this to the death_string in ouch */
    if (you.where_are_you >= BRANCH_ORCISH_MINES
        && you.where_are_you <= BRANCH_SWAMP)
    {
        curr_subdungeon_level = you.depth
                                    - you.branch_stairs[you.where_are_you - 10];
    }

    gotoxy(46, 12);

#if DEBUG_DIAGNOSTICS
    cprintf( "(%d) ", you.depth + 1 );
#endif

    env.floor_colour = LIGHTGREY;
    env.rock_colour  = BROWN;

    if (you.level_type == LEVEL_PANDEMONIUM)
    {
        cprintf("- Pandemonium            ");

        env.floor_colour = mons_element_colour( env.mons_alloc[9] );
        env.rock_colour  = mons_element_colour( env.mons_alloc[8] );

        if (env.floor_colour == BLACK)
            env.floor_colour = LIGHTGREY;

        if (env.rock_colour == BLACK)
            env.rock_colour = BROWN;
    }
    else if (you.level_type == LEVEL_ABYSS)
    {
        cprintf("- The Abyss               ");

        env.floor_colour = mons_element_colour( env.mons_alloc[9] );
        env.rock_colour  = mons_element_colour( env.mons_alloc[8] );

        if (env.floor_colour == BLACK)
            env.floor_colour = LIGHTGREY;

        if (env.rock_colour == BLACK)
            env.rock_colour = BROWN;
    }
    else if (you.level_type == LEVEL_LABYRINTH)
    {
        cprintf("- a Labyrinth           ");
    }
    else
    {
        // level_type == LEVEL_DUNGEON
        if (!player_in_branch( BRANCH_VESTIBULE_OF_HELL ))
            cprintf( "%d", curr_subdungeon_level );

        switch (you.where_are_you)
        {
        case BRANCH_MAIN_DUNGEON:
            cprintf(" of the Dungeon           ");
            break;
        case BRANCH_DIS:
            env.floor_colour = CYAN;
            env.rock_colour  = CYAN;
            cprintf(" of Dis                   ");
            break;
        case BRANCH_GEHENNA:
            env.floor_colour = DARKGREY;
            env.rock_colour  = RED;
            cprintf(" of Gehenna               ");
            break;
        case BRANCH_VESTIBULE_OF_HELL:
            env.floor_colour = LIGHTGREY;
            env.rock_colour  = LIGHTGREY;
            cprintf("- the Vestibule of Hell            ");
            break;
        case BRANCH_COCYTUS:
            env.floor_colour = LIGHTBLUE;
            env.rock_colour  = LIGHTCYAN;
            cprintf(" of Cocytus                   ");
            break;
        case BRANCH_TARTARUS:
            env.floor_colour = DARKGREY;
            env.rock_colour  = DARKGREY;
            cprintf(" of Tartarus                ");
            break;
        case BRANCH_INFERNO:
            env.floor_colour = LIGHTRED;
            env.rock_colour  = RED;
            cprintf(" of the Inferno               ");
            break;
        case BRANCH_THE_PIT:
            env.floor_colour = RED;
            env.rock_colour  = DARKGREY;
            cprintf(" of the Pit              ");
            break;
        case BRANCH_ORCISH_MINES:
            env.floor_colour = BROWN;
            env.rock_colour  = BROWN;
            cprintf(" of the Orcish Mines          ");
            break;
        case BRANCH_HIVE:
            env.floor_colour = YELLOW;
            env.rock_colour  = BROWN;
            cprintf(" of the Hive                  ");
            break;
        case BRANCH_LAIR:
            env.floor_colour = GREEN;
            env.rock_colour  = BROWN;
            cprintf(" of the Lair                  ");
            break;
        case BRANCH_SLIME_PITS:
            env.floor_colour = GREEN;
            env.rock_colour  = LIGHTGREEN;
            cprintf(" of the Slime Pits            ");
            break;
        case BRANCH_VAULTS:
            env.floor_colour = LIGHTGREY;
            env.rock_colour  = BROWN;
            cprintf(" of the Vaults                ");
            break;
        case BRANCH_CRYPT:
            env.floor_colour = LIGHTGREY;
            env.rock_colour  = LIGHTGREY;
            cprintf(" of the Crypt                 ");
            break;
        case BRANCH_HALL_OF_BLADES:
            env.floor_colour = LIGHTGREY;
            env.rock_colour  = LIGHTGREY;
            cprintf(" of the Hall of Blades        ");
            break;

        case BRANCH_HALL_OF_ZOT:
            if (you.depth - you.branch_stairs[7] <= 1)
            {
                env.floor_colour = LIGHTGREY;
                env.rock_colour  = LIGHTGREY;
            }
            else
            {
                switch (you.depth - you.branch_stairs[7])
                {
                case 2:
                    env.floor_colour = BLUE;
                    env.rock_colour  = LIGHTGREY;
                    break;
                case 3:
                    env.floor_colour = LIGHTBLUE;
                    env.rock_colour  = BLUE;
                    break;
                case 4:
                    env.floor_colour = MAGENTA;
                    env.rock_colour  = LIGHTBLUE;
                    break;
                case 5:
                    env.floor_colour = LIGHTMAGENTA;
                    env.rock_colour  = MAGENTA;
                    break;
                }
            }
            cprintf(" of the Realm of Zot          ");
            break;

        case BRANCH_ECUMENICAL_TEMPLE:
            env.floor_colour = LIGHTGREY;
            env.rock_colour  = LIGHTGREY;
            cprintf(" of the Temple                ");
            break;
        case BRANCH_SNAKE_PIT:
            env.floor_colour = LIGHTGREEN;
            env.rock_colour  = YELLOW;
            cprintf(" of the Snake Pit             ");
            break;
        case BRANCH_ELVEN_HALLS:
            env.floor_colour = DARKGREY;
            env.rock_colour  = LIGHTGREY;
            cprintf(" of the Elven Halls           ");
            break;
        case BRANCH_TOMB:
            env.floor_colour = YELLOW;
            env.rock_colour  = LIGHTGREY;
            cprintf(" of the Tomb                  ");
            break;
        case BRANCH_SWAMP:
            env.floor_colour = BROWN;
            env.rock_colour  = BROWN;
            cprintf(" of the Swamp                 ");
            break;
        }
    }                           // end else
}                               // end enter_new_level()

void handle_player_shield_blocks( int damage, int exer )
{
    // no blocking count or exercise if no shield was used
    if (you.equip[EQ_SHIELD] != -1)
    {
        you.shield_blocks++;

        if (exer)
            exercise( SK_SHIELDS, exer );
    }
    else
    {
        const int reduction = 1 + damage / 5;

#if DEBUG_DIAGNOSTICS
        mpr( MSGCH_DIAGNOSTICS, "shield reduction = %d", reduction );
#endif

        if (you.duration[ DUR_CONDENSATION_SHIELD ] > reduction)
            you.duration[ DUR_CONDENSATION_SHIELD ] -= reduction;
        else
        {
            mpr( MSGCH_DURATION, "Your icy shield dissipates!" );
            you.duration[ DUR_CONDENSATION_SHIELD ] = 0;
            set_redraw_status( REDRAW_ARMOUR_CLASS );
        }
    }
}

void handle_mon_shield_blocks( struct monsters *mon )
{
    mon->shield_blocks++;
}

// Note: chance now collapses all the one_chance_in(X) calls that used
// to be made by applying that change of probablity directly to the mass
// chack.
void exercise_armour( int exer, int chance )
{
    if (you.equip[EQ_BODY_ARMOUR] != -1)
    {
        const int mass = item_mass( you.inv[ you.equip[EQ_BODY_ARMOUR] ] );

        if (!player_light_armour() && random2( 1000 * chance ) <= mass)
            exercise( SK_ARMOUR, exer );
    }
}

void exercise_dodging( int exer, int chance )
{
    if (player_light_armour() && one_chance_in( chance ))
        exercise( SK_DODGING, exer );
}


static void dart_trap( bool trap_known, int trap_id, struct bolt &pbolt,
                       bool poison )
{
    ASSERT( env.trap[trap_id].type != TRAP_UNASSIGNED );

    int damage_taken = 0;

    const int base = (trap_known ? 10 : 60) + (you.conf ? 40 : 0);
    const int level = (you.level_type == LEVEL_DUNGEON) ? you.depth : 36;

    const int avoid = you.dex * (you.skills[SK_TRAPS_DOORS] + 1);
    const int diff = base + level / 2;

#if DEBUG_DIAGNOSTICS
    mpr( MSGCH_DIAGNOSTICS, "avoid trap: %d; trap difficulty: %d", avoid, diff );
#endif

    // first check if we can avoid setting it off:
    if (random2(avoid) >= diff)
    {
        mpr( "You avoid triggering a%s trap.", pbolt.name );

        // only give out skill for discovery of trap + avoidance.
        if (!trap_known)
            exercise( SK_TRAPS_DOORS, 2 );

        return;
    }

    // second: check if trap is armed
    if (!env.trap[trap_id].ammo)
    {
        noisy( SL_TRAP_CLICK, you.x_pos, you.y_pos,
               "You hear something go click." );

        // re-hide the trap
        if (!trap_known)
            grd[you.x_pos][you.y_pos] = DNGN_UNDISCOVERED_TRAP;

        return;
    }

    // finally, trap fires!
    env.trap[trap_id].ammo--;

    snprintf( info, INFO_SIZE, "A%s shoots out and ", pbolt.name );

    int hit = 10 + base + 3 * level;

    if (you.duration[DUR_DEFLECT_MISSILES])
        hit = 0;
    else if (player_repel_missiles())
        hit /= 2;

    const int shield_class = player_shield_class();
    const int con_block = base / 3 + level / 2 + your_shield_blocks_penalty();

    // Like in beam.cc, we do dodge before block here so the player can
    // apply deflection before they have to use a shield_block.
    if (!test_hit( hit, player_evasion() ))
    {
        strcat( info, "misses you." );
        mpr(info);

        if (!you.duration[DUR_DEFLECT_MISSILES])
            exercise_dodging( 1, 3 );
    }
    else if (player_can_block() && test_block( shield_class, con_block ))
    {
        strcat( info, "hits your shield." );
        mpr(info);

        handle_player_shield_blocks( pbolt.damage.num * pbolt.damage.size,
                                     one_chance_in(3) ? 1 : 0 );
    }
    else
    {
        const int ac = player_armour_class();

        strcat( info, "hits you!" );
        mpr(info);

        if (poison && random2(ac) < 4)
            poison_player( 1 + random2(3) );

        damage_taken = roll_dice( pbolt.damage );
        damage_taken -= random2( ac + 1 );

        if (damage_taken > 0)
            ouch( damage_taken, 0, KILLED_BY_TRAP, pbolt.name );

        exercise_armour( 1, 2 );
    }

    if (coinflip())
        drop_trap_item( trap_id, you.x_pos, you.y_pos, 1 );
}                               // end dart_trap()

static void blade_trap( bool trap_known, int trap_id )
{
    // These are based on the ones in dart_trap()
    const int base = (trap_known ? 10 : 60) + (you.conf ? 40 : 0);
    const int level = (you.level_type == LEVEL_DUNGEON) ? you.depth : 36;

    const int avoid = you.dex * (you.skills[SK_TRAPS_DOORS] + 1);
    const int diff = base + level / 2;

    const int hit = 20 + base + 5 * level;

#if DEBUG_DIAGNOSTICS
    mpr( MSGCH_DIAGNOSTICS, "avoid trap: %d; trap difficulty: %d", avoid, diff );
#endif

    // order is important here:
    if (random2(avoid) >= diff)
    {
        mpr( "You avoid triggering a blade trap." );

        // only give out XP for discovery of trap + avoidance.
        if (!trap_known)
            exercise( SK_TRAPS_DOORS, 1 );
    }
    else if (one_chance_in(50))                    // triggered, check for jam
    {
        noisy( SL_TRAP_JAM, you.x_pos, you.y_pos,
               (trap_known) ? "The blade trap jams!" : "You hear a screech!" );

        remove_trap( trap_id );
    }
    else if (!test_hit( hit, player_evasion() ))   // swinging, check for hit
    {
        mpr( "A huge blade swings just past you!" );
        exercise_dodging( 1, 3 );
    }
    else
    {
        mpr( "A huge blade swings out and slices into you!" );

        ouch( 2 * level + roll_zdice(2,15) - random2(player_armour_class()+1),
                0, KILLED_BY_TRAP, " blade" );

        exercise_armour( 1, 2 );
    }
}

void handle_traps( char trt, int id, bool trap_known, bool disarm )
{
    UNUSED( disarm );

    struct bolt beam;
    const int level = (you.level_type == LEVEL_DUNGEON) ? you.depth : 36;

    switch (trt)
    {
    case TRAP_DART:
        strcpy( beam.name, " dart" );
        beam.damage = dice_def( 1, 4 + level / 2 );
        dart_trap( trap_known, id, beam, false );
        break;

    case TRAP_NEEDLE:
        strcpy( beam.name, " needle" );
        beam.damage = dice_def( 1, 0 );
        dart_trap( trap_known, id, beam, true );
        break;

    case TRAP_ARROW:
        strcpy( beam.name, "n arrow" );
        beam.damage = dice_def( 1, 7 + (level * 3) / 2 );
        dart_trap( trap_known, id, beam, false );
        break;

    case TRAP_BOLT:
        strcpy( beam.name, " bolt" );
        beam.damage = dice_def( 1, 15 + level );
        dart_trap( trap_known, id, beam, false );
        break;

    case TRAP_SPEAR:
        strcpy( beam.name, " spear" );
        beam.damage = dice_def( 3, 5 + level / 3 );
        dart_trap( trap_known, id, beam, false );
        break;

    case TRAP_AXE:
        strcpy( beam.name, "n axe" );
        beam.damage = dice_def( 3, 7 + level / 3 );
        dart_trap( trap_known, id, beam, false );
        break;

    case TRAP_TELEPORT:
        mpr("You enter a teleport trap!");

        if (scan_randarts( RAP_PREVENT_TELEPORTATION ))
            mpr("You feel a weird sense of stasis.");
        else
            you_teleport2( true );
        break;

    case TRAP_AMNESIA:
        canned_msg( MSG_DISORIENTED );
        if (!player_mental_clarity())
            forget_map( roll_dice(2,50) );
        break;

    case TRAP_BLADE:
        blade_trap( trap_known, id );
        break;

    case TRAP_ZOT:
    default:
        mpr( (trap_known) ? "You enter the Zot trap."
                          : "Oh no! You have blundered into a Zot trap!" );

        noisy( SL_TRAP_ZOT, you.x_pos, you.y_pos, "ZOT!" );

        miscast_effect( SPTYP_RANDOM, random2(30) + level, 75 + random2(100), 3,
                        "a Zot trap" );
        break;
    }
}                               // end handle_traps()

void disarm_trap( struct dist &disa )
{
    if (you.berserker)
    {
        canned_msg( MSG_TOO_BERSERK );
        return;
    }

    if (you.conf)
    {
        canned_msg( MSG_TOO_CONFUSED );
        return;
    }

    const int trap_x = you.x_pos + disa.dx;
    const int trap_y = you.y_pos + disa.dy;

    const int id = trap_at_xy( trap_x, trap_y );

    if (id == -1)
    {
        mpr("Error - couldn't find that trap.");
        return;
    }

    const int type = env.trap[id].type;
    const int category = trap_category( type );

    if (category == DNGN_TRAP_MAGICAL)
    {
        mpr("This trap cannot be disarmed.");
        return;
    }

    ASSERT( category == DNGN_TRAP_MECHANICAL );

    const int  skill = (you.dex * (you.skills[SK_TRAPS_DOORS] + 1)) / 10 + 1;
    const int  level = (you.level_type == LEVEL_DUNGEON) ? you.depth : 36;
    const bool armed = (type != TRAP_BLADE && env.trap[id].ammo > 0);
    const int  diff  = (10 + level / 2) / (armed ? 1 : 3);

    you.turn_is_over = true;

#if DEBUG_DIAGNOSTICS
    mpr( MSGCH_DIAGNOSTICS, "disarm skill: %d; trap difficulty: %d", skill, diff );

#endif

    if (random2(skill) <= random2(diff))
    {
        mpr("You failed to disarm the trap.");
        handle_traps( type, id, true, true );

        if (armed)
        {
            // Low skill/dex can result in some ammo getting damaged...
            // This allows us to increase the usefulness of traps and doors
            // skill for extracting ammo from traps without having to subject
            // the player to an even larger number of failures to accomplish
            // the same effect.
            const int perc = stat_mult( you.dex,
                                        10 * skill_bump(SK_TRAPS_DOORS),
                                        15, 0 );

            const int roll = random2(100);

            if (roll >= perc)
            {
                if (type == TRAP_SPEAR || type == TRAP_AXE)  // less ammo
                    env.trap[id].ammo--;
                else
                    env.trap[id].ammo -= 1 + (roll - perc + 5) / 10;

#if DEBUG_DIAGNOSTICS
                mpr( MSGCH_DIAGNOSTICS, "ammo remaining: %d", env.trap[id].ammo );
#endif

                if (env.trap[id].ammo < 0)
                    env.trap[id].ammo = 0;
            }
        }
    }
    else
    {
        mpr("You have disarmed the trap.");

        int exer = 5 + random2( 2 + level / 5 );        // was 5 + random2(5)

        if (type != TRAP_BLADE)
        {
            // Ammo traps:
            if (!armed)     // unarmed traps are easier, so less exercise
                exer /= 2;
            else
                drop_trap_item( id, trap_x, trap_y, env.trap[id].ammo, false );
        }

        exercise( SK_TRAPS_DOORS, exer );

        // finally clear trap
        remove_trap( id );
    }
}                               // end disarm_trap()

void manage_clouds(void)
{
    // amount which cloud dissipates - must be unsigned! {dlb}
    int dissipate = 0;

    for (unsigned char cc = 0; cc < MAX_CLOUDS; cc++)
    {
        const int ctype = env.cloud[cc].type;

        if (ctype == CLOUD_NONE)   // no cloud -> next iteration
            continue;

        const int terr = grd[ env.cloud[cc].x ][ env.cloud[cc].y ];

        dissipate = you.time_taken;

        // water -> flaming clouds:
        // lava -> freezing clouds:
        if (ctype == CLOUD_FIRE || ctype == CLOUD_FIRE_MON)
        {
            expose_floor_to_element( BEAM_FIRE, env.cloud[cc].x, env.cloud[cc].y );
            if (terr == DNGN_DEEP_WATER)
                dissipate *= 4;
        }
        else if (ctype == CLOUD_COLD || ctype == CLOUD_COLD_MON)
        {
            expose_floor_to_element( BEAM_COLD, env.cloud[cc].x, env.cloud[cc].y );
            if (terr == DNGN_LAVA)
                dissipate *= 4;
        }

        // check for total dissipatation and handle accordingly:
        if (dissipate >= env.cloud[cc].decay)
            delete_cloud( cc );
        else
            env.cloud[cc].decay -= dissipate;
    }

    return;
}                               // end manage_clouds()

void weird_writing(char stringy[40])
{
    int temp_rand = 0;          // for probability determinations {dlb}

    temp_rand = random2(15);

    // you'll see why later on {dlb}
    strcpy(stringy, (temp_rand == 0) ? "writhing" :
                    (temp_rand == 1) ? "bold" :
                    (temp_rand == 2) ? "faint" :
                    (temp_rand == 3) ? "spidery" :
                    (temp_rand == 4) ? "blocky" :
                    (temp_rand == 5) ? "angular" :
                    (temp_rand == 6) ? "shimmering" :
                    (temp_rand == 7) ? "glowing" : "");

    if (temp_rand < 8)
        strcat(stringy, " ");   // see above for reasoning {dlb}

    temp_rand = random2(14);

    strcat(stringy, (temp_rand ==  0) ? "yellow" :
                    (temp_rand ==  1) ? "brown" :
                    (temp_rand ==  2) ? "black" :
                    (temp_rand ==  3) ? "purple" :
                    (temp_rand ==  4) ? "orange" :
                    (temp_rand ==  5) ? "lime-green" :
                    (temp_rand ==  6) ? "blue" :
                    (temp_rand ==  7) ? "grey" :
                    (temp_rand ==  8) ? "silver" :
                    (temp_rand ==  9) ? "gold" :
                    (temp_rand == 10) ? "umber" :
                    (temp_rand == 11) ? "charcoal" :
                    (temp_rand == 12) ? "pastel" :
                    (temp_rand == 13) ? "mauve"
                                      : "colourless");

    strcat(stringy, " ");

    temp_rand = random2(14);

    strcat(stringy, (temp_rand == 0) ? "writing" :
                    (temp_rand == 1) ? "scrawl" :
                    (temp_rand == 2) ? "sigils" :
                    (temp_rand == 3) ? "runes" :
                    (temp_rand == 4) ? "hieroglyphics" :
                    (temp_rand == 5) ? "scrawl" :
                    (temp_rand == 6) ? "print-out" :
                    (temp_rand == 7) ? "binary code" :
                    (temp_rand == 8) ? "glyphs" :
                    (temp_rand == 9) ? "symbols"
                                     : "text");

    return;
}                               // end weird_writing()

// must be a better name than 'place' for the first parameter {dlb}
// returns true if we manage to scramble free.
bool fall_into_a_pool( int entry_x, int entry_y, bool allow_shift,
                       unsigned char terrain )
{
    bool escape = false;
    FixedVector< char, 2 > empty;

    strcpy(info, "You fall into the ");

    strcat(info, (terrain == DNGN_LAVA)       ? "lava" :
                 (terrain == DNGN_DEEP_WATER) ? "water"
                                              : "programming rift");

    strcat(info, "!");
    mpr(info);

    more();
    mesclr();

    if (terrain == DNGN_LAVA)
    {
        const int resist = player_res_fire();

        if (resist <= 0)
        {
            mpr( "The lava burns you to a cinder!" );
            ouch( INSTANT_DEATH, 0, KILLED_BY_LAVA );
        }
        else
        {
            // should boost # of bangs per damage in the future {dlb}
            mpr( "The lava burns you!" );
            ouch( (10 + roll_dice(2,50)) / resist, 0, KILLED_BY_LAVA );
        }

        expose_player_to_element( BEAM_LAVA, 14 );
    }

    // a distinction between stepping and falling from you.levitation
    // prevents stepping into a thin stream of lava to get to the other side.
    if (scramble())
    {
        if (allow_shift)
        {
            if (empty_surrounds( you.x_pos, you.y_pos, DNGN_FLOOR, 1, false,
                                 false, empty ))
            {
                escape = true;
            }
            else
            {
                escape = false;
            }
        }
        else
        {
            // back out the way we came in, if possible
            if (grid_distance( you.x_pos, you.y_pos, entry_x, entry_y ) == 1
                && (entry_x != empty[0] || entry_y != empty[1]))
            {
                escape = true;
                empty[0] = entry_x;
                empty[1] = entry_y;
            }
            else  // zero or two or more squares away, with no way back
            {
                escape = false;
            }
        }
    }
    else
    {
        mpr("You try to escape, but your burden drags you down!");
    }

    if (escape && move_player_to_grid( empty[0], empty[1], false, false, true ))
    {
        mpr("You manage to scramble free!");

        if (terrain == DNGN_LAVA)
            expose_player_to_element( BEAM_LAVA, 14 );

        return (true);
    }

    mpr("You drown...");

    if (terrain == DNGN_LAVA)
        ouch( INSTANT_DEATH, 0, KILLED_BY_LAVA );
    else if (terrain == DNGN_DEEP_WATER)
        ouch( INSTANT_DEATH, 0, KILLED_BY_WATER );

    return (false);
}                               // end fall_into_a_pool()

bool scramble(void)
{
    int max_carry = carrying_capacity();

    if ((max_carry / 2) + random2(max_carry / 2) <= you.burden)
        return false;
    else
        return true;
}                               // end scramble()

void weird_colours(unsigned char coll, char wc[30])
{
    unsigned char coll_div16 = coll / 16; // conceivable max is then 16 {dlb}

    // Must start with a consonant!
    strcpy(wc, (coll_div16 == 0 || coll_div16 ==  7) ? "brilliant" :
               (coll_div16 == 1 || coll_div16 ==  8) ? "pale" :
               (coll_div16 == 2 || coll_div16 ==  9) ? "mottled" :
               (coll_div16 == 3 || coll_div16 == 10) ? "shimmering" :
               (coll_div16 == 4 || coll_div16 == 11) ? "bright" :
               (coll_div16 == 5 || coll_div16 == 12) ? "dark" :
               (coll_div16 == 6 || coll_div16 == 13) ? "shining"
                                                     : "faint");

    strcat(wc, " ");

    while (coll > 17)
        coll -= 10;

    strcat(wc, (coll ==  0) ? "red" :
               (coll ==  1) ? "purple" :
               (coll ==  2) ? "green" :
               (coll ==  3) ? "orange" :
               (coll ==  4) ? "magenta" :
               (coll ==  5) ? "black" :
               (coll ==  6) ? "grey" :
               (coll ==  7) ? "silver" :
               (coll ==  8) ? "gold" :
               (coll ==  9) ? "pink" :
               (coll == 10) ? "yellow" :
               (coll == 11) ? "white" :
               (coll == 12) ? "brown" :
               (coll == 13) ? "aubergine" :
               (coll == 14) ? "ochre" :
               (coll == 15) ? "leaf green" :
               (coll == 16) ? "mauve" :
               (coll == 17) ? "azure"
                            : "colourless");

    return;
}                               // end weird_colours()

void inc_berserk( int inc )
{
    ASSERT( inc > 0 );

    // checks and handling for starting a new berserk:
    if (!you.berserker)
    {
        // raise strength if not already boosted
        if (!you.might)
            modify_stat( STAT_STRENGTH, 5, true );

        set_redraw_status( REDRAW_STRENGTH );        // always redraw

        calc_hp_max( (you.hp * 15) / 10 );

        mpr("A red film seems to cover your vision as you go berserk!");
    }

    you.berserker += inc;

    // every time we gain rage, our penalty counter should be reset.
    if (you.berserk_penalty != NO_BERSERK_PENALTY)
        you.berserk_penalty = 0;
}

void dec_berserk( int dec )
{
    ASSERT( dec > 0 );

    you.berserker -= dec;
    if (you.berserker < 1)
        you.berserker = 1;      // as the main loop handles going down
}

bool go_berserk( bool intentional )
{
    if (you.is_undead)
    {
        if (intentional)
            mpr("You cannot raise a blood rage in your lifeless body.");
        // or else you won't notice -- no message here
        return (false);
    }

    if (you.berserker)
    {
        if (intentional)
            mpr("You're already berserk!");
        // or else you won't notice -- no message here.
        return (false);
    }

    if (you.exhausted)
    {
        if (intentional) // or else they won't notice -- no message here
            mpr("You're too exhausted to go berserk.");

        return (false);
    }

    inc_berserk( 20 + roll_zdice(2,10) );
    you.flash_colour = RED;

    return (true);
}                               // end go_berserk()

// returns true if ammo is recoverable
// Note: The skill usage here isn't too realistic in the sense that any ammo
// that gets lost because this function fails never has a chance to be retested
// at a higher skill level later.  This, however, is nice and simple.
bool fired_ammo_recoverable( const item_def &ammo )
{
    // These are based off the fact that sum(p) = 1 / (1 - p) (which is the
    // multiple of #shots we get after we've run through the stack an infinite
    // number of times).  However, we want to reverse that so that we can
    // easily interpolate on the multiple (to get good increases with lower
    // skill levels easier), not the percentage chance of recovering ammo...
    // thus we use p = (y - 1) / y, with y being the multiple (x10 in the code
    // below),

    // default breakage rate:
    int chance = 200;                   // 20x          (95%)

    switch (ammo.base_type)
    {
    default:
        break;

    case OBJ_WEAPONS:
    case OBJ_ARMOUR:
    case OBJ_CORPSES:
    case OBJ_GOLD:
    case OBJ_ORBS:
        chance = 1000;
        break;

    case OBJ_POTIONS:
        chance = 20;                    // 2x           (50%)
        break;

    case OBJ_MISCELLANY:
        switch (ammo.sub_type)
        {
        case MISC_RUNE_OF_ZOT:
        case MISC_PORTABLE_ALTAR_OF_NEMELEX:
            chance = 1000;
            break;

        default:
            chance = 20;                // 2x           (50%)
            break;
        }
        break;

    case OBJ_MISSILES:
        {
            // Using Throwing skill as the fletching/ammo preserving skill,
            // since we want this to be transferable.  -- bwr
            const int skill = skill_bump( SK_RANGED_COMBAT );

            switch (ammo.sub_type)
            {
            case MI_DART:
                chance = 20 + skill / 2;    // 2x - 3.5x    (50% - 71.4%)
                break;

            case MI_BOLT:
            case MI_ARROW:
                chance = 15 + skill;        // 1.5x - 4.5x  (33.3% - 77.8%)
                break;

            case MI_NEEDLE:
                chance = 20;                // 2x flat      (50%)
                break;

            case MI_STONE:
            case MI_LARGE_ROCK:
            default:
                chance = 30;                // 3x flat      (66.7%)
                break;
            }
        }
        break;
    }

    return (chance >= 1000 || random2( chance ) >= 10);
}

void drop_trap_item( int trap_id, int x, int y, short quant, bool attrition )
{
    ASSERT( env.trap[trap_id].type != TRAP_UNASSIGNED );

    item_def  item;

    item.base_type = OBJ_MISSILES;
    item.sub_type = MI_DART;
    item.plus = 0;
    item.plus2 = 0;
    item.flags = 0;
    item.special = 0;
    item.quantity = quant;
    item.colour = LIGHTCYAN;
    item.slot = -1;

    switch (env.trap[trap_id].type)
    {
    case TRAP_NEEDLE:
        item.base_type = OBJ_MISSILES;
        item.sub_type = MI_NEEDLE;
        set_item_ego_type( item, OBJ_MISSILES, SPMSL_POISONED );
        item.colour = WHITE;
        break;

    case TRAP_DART:
        item.base_type = OBJ_MISSILES;
        item.sub_type = MI_DART;
        set_item_ego_type( item, OBJ_MISSILES, SPMSL_NORMAL );
        break;

    case TRAP_ARROW:
        item.base_type = OBJ_MISSILES;
        item.sub_type = MI_ARROW;
        set_item_ego_type( item, OBJ_MISSILES, SPMSL_NORMAL );
        break;

    case TRAP_BOLT:
        item.base_type = OBJ_MISSILES;
        item.sub_type = MI_BOLT;
        set_item_ego_type( item, OBJ_MISSILES, SPMSL_NORMAL );
        break;

    case TRAP_SPEAR:
        item.base_type = OBJ_WEAPONS;
        item.sub_type = WPN_SPEAR;
        item.quantity = 1;      // doesn't stack, multiple handled below
        set_item_ego_type( item, OBJ_WEAPONS, SPWPN_NORMAL );
        break;

    case TRAP_AXE:
        item.base_type = OBJ_WEAPONS;
        item.sub_type = WPN_HAND_AXE;
        item.quantity = 1;      // doesn't stack, multiple handled below
        set_item_ego_type( item, OBJ_WEAPONS, SPWPN_NORMAL );
        break;

    default:
        // magic and blade traps
        return;
    }

    // Additional attrition applied here as walking through traps should never
    // be a viable system for retrieving ammo from them (ie keep disarm as the
    // correct option).
    if (!attrition
        || (coinflip() && fired_ammo_recoverable( item )))
    {
        if (env.trap[trap_id].type == TRAP_SPEAR
            || env.trap[trap_id].type == TRAP_AXE)
        {
            for (int i = 0; i < quant; i++)
                copy_item_to_grid( item, x, y );
        }
        else
        {
            copy_item_to_grid( item, x, y );
        }
    }
}                               // end drop_trap_item()

// returns appropriate trap symbol for a given trap type {dlb}
int trap_category( int type )
{
    switch (type)
    {
    case TRAP_TELEPORT:
    case TRAP_AMNESIA:
    case TRAP_ZOT:
        return (DNGN_TRAP_MAGICAL);

    case TRAP_DART:
    case TRAP_ARROW:
    case TRAP_SPEAR:
    case TRAP_AXE:
    case TRAP_BLADE:
    case TRAP_BOLT:
    case TRAP_NEEDLE:
    default:                    // what *would* be the default? {dlb}
        return (DNGN_TRAP_MECHANICAL);
    }
}                               // end trap_category()

// returns index of the trap for a given (x,y) coordinate pair {dlb}
int trap_at_xy( int which_x, int which_y )
{

    for (int which_trap = 0; which_trap < MAX_TRAPS; which_trap++)
    {
        if (env.trap[which_trap].type == TRAP_UNASSIGNED)
            continue;

        if (env.trap[which_trap].x == which_x
            && env.trap[which_trap].y == which_y)
        {
            return (which_trap);
        }
    }

    // no idea how well this will be handled elsewhere: {dlb}
    return (-1);
}                               // end trap_at_xy()

void remove_trap( int trap_id )
{
    ASSERT( env.trap[trap_id].type != TRAP_UNASSIGNED );

    grd[ env.trap[trap_id].x ][ env.trap[trap_id].y ] = DNGN_FLOOR;
    env.trap[trap_id].type = TRAP_UNASSIGNED;
    env.trap[trap_id].x = 0;
    env.trap[trap_id].y = 0;
    env.trap[trap_id].ammo = 0;
}

void fudge_ghost_stats( int &str, int &dex )
{
    // We're fudgins stats so that unarmed combat gets based off
    // of the ghost's species, not the player's stats... exact
    // stats are not required anyways, all that matters is whether
    // dex >= str. -- bwr
    dex = 10;
    str = 10;

    switch (env.ghost.values[GVAL_CLASS])
    {
    case JOB_GLADIATOR:
    case JOB_THIEF:
    case JOB_ASSASSIN:
    case JOB_STALKER:
    case JOB_MONK:
    case JOB_TRANSMUTER:
        dex += 5;
        break;

    case JOB_FIGHTER:
    case JOB_BERSERKER:
    case JOB_PALADIN:
    case JOB_PRIEST:
        str += 5;
        break;

    default:
        break;
    }

    switch (env.ghost.values[GVAL_SPECIES])
    {
    case SP_HILL_DWARF:
    case SP_MOUNTAIN_DWARF:
    case SP_TROLL:
    case SP_OGRE:
    case SP_OGRE_MAGE:
    case SP_MINOTAUR:
    case SP_HILL_ORC:
    case SP_CENTAUR:
    case SP_NAGA:
    case SP_MUMMY:
    case SP_GHOUL:
        str += 5;
        break;

    case SP_HUMAN:
    case SP_DEMIGOD:
    case SP_DEMONSPAWN:
        break;

    default:
        str -= 5;
        break;
    }
}

const char *const damage_punctuation( int damage )
{
    if (damage < HIT_WEAK)
        return (".");
    else if (damage < HIT_MED)
        return ("!");
    else if (damage < HIT_STRONG)
        return ("!!");

    return ("!!!");
}

int conv_char_to_obj_class( char sym )
{
    const std::string obj_syms = ")([/%.?=!.+\\0}&$";

    // Make the amulet symbol equiv to ring, etc -- bwross
    switch (sym)
    {
    case '"': sym = '=';  break; // also represents jewellery
    case '|': sym = '\\'; break; // also represents staves
    case ':': sym = '+';  break; // also represents books

    default:
        break;
    }

    const unsigned int ret = obj_syms.find_first_of( sym );

    return ((ret == std::string::npos) ? static_cast<int>(OBJ_UNASSIGNED) : ret);
}

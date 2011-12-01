/*
 *  File:       player.cc
 *  Summary:    Player related functions.
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 * <6> 7/30/99 BWR   Added player_spell_levels()
 * <5> 7/13/99 BWR   Added player_res_electricity()
 *                   and player_hunger_rate()
 * <4> 6/22/99 BWR   Racial adjustments to stealth and Armour.
 * <3> 5/20/99 BWR   Fixed problems with random stat increases, added kobold
 *                   stat increase.  increased EV recovery for Armour.
 * <2> 5/08/99 LRH   display_char_status correctly handles magic_contamination.
 * <1> -/--/-- LRH   Created
 */

#include "AppHdr.h"
#include "player.h"

#ifdef DOS
#include <conio.h>
#endif

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <ctype.h>

#include "globals.h"
#include "externs.h"

#include "cloud.h"
#include "delay.h"
#include "effects.h"
#include "fight.h"
#include "itemname.h"
#include "items.h"
#include "itemprop.h"
#include "macro.h"
#include "misc.h"
#include "mon-util.h"
#include "mutation.h"
#include "output.h"
#include "ouch.h"
#include "randart.h"
#include "religion.h"
#include "skills.h"
#include "skills2.h"
#include "spl-util.h"
#include "spells4.h"
#include "stuff.h"
#include "transfor.h"
#include "view.h"


/*
   you.duration []: //jmf: obsolete, see enum.h instead
   0 - liquid flames
   1 - icy armour
   2 - repel missiles
   3 - prayer
   4 - regeneration
   5 - vorpal blade
   6 - fire brand
   7 - ice brand
   8 - lethal infusion
   9 - swiftness
   10 - insulation
   11 - stonemail
   12 - controlled flight
   13 - teleport
   14 - control teleport
   15 - poison weapon
   16 - resist poison
   17 - breathe something
   18 - transformation (duration)
   19 - death channel
   20 - deflect missiles
 */

/* attributes
   0 - resist lightning
   1 - spec_air
   2 - spec_earth
   3 - control teleport
   4 - walk slowly (eg naga)
   5 - transformation (form)
   6 - Nemelex card gift countdown
   7 - Nemelex has given you a card table
   8 - How many demonic powers a dspawn has
 */

/* armour list
   0 - wielded
   1 - cloak
   2 - helmet
   3 - gloves
   4 - boots
   5 - shield
   6 - body armour
   7 - ring 0
   8 - ring 1
   9 - amulet
 */

/* Contains functions which return various player state vars,
   and other stuff related to the player. */

int species_exp_mod(char species);
void ability_increase(void);

// Use this function whenever the player enters (or lands and thus re-enters)
// a grid.
//
// stepped - normal walking moves
// allow_shift - allowed to scramble in any direction out of lava/water
// force - ignore safety checks, move must happen (traps, lava/water).
bool move_player_to_grid( int x, int y, bool stepped, bool allow_shift,
                          bool force )
{
    ASSERT( in_bounds( x, y ) );

    int id;

    // assuming that entering the same square means coming from above (levitate)
    const bool from_above = (you.x_pos == x && you.y_pos == y);

    const int old_grid = (from_above) ? static_cast<int>(DNGN_FLOOR)
                                      : grd[you.x_pos][you.y_pos];

    const int new_grid = grd[x][y];

    // really must be clear
    ASSERT( !grid_is_solid( new_grid ) );

    // if (grid_is_solid( new_grid ))
    //     return (false);

    // better not be an unsubmerged monster either:
    ASSERT( mgrd[x][y] == NON_MONSTER
            || mons_is_submerged( &menv[ mgrd[x][y] ] ));

    // if (mgrd[x][y] != NON_MONSTER && !mons_is_submerged( &menv[ mgrd[x][y] ] ))
    //     return (false);

    // if we're walking along, give a chance to avoid trap
    if (stepped
        && !force
        && new_grid == DNGN_UNDISCOVERED_TRAP)
    {
        const int skill = 4 + you.skills[SK_TRAPS_DOORS]
                            + you.mutation[MUT_ACUTE_VISION]
                            - 2 * you.mutation[MUT_BLURRY_VISION];

        if (random2( skill ) > 6)
        {
            mpr( MSGCH_WARN,
                  "Wait a moment, %s!  Do you really want to step there?",
                  you.your_name );

            more();

            exercise( SK_TRAPS_DOORS, 3 );

            you.turn_is_over = false;

            id = trap_at_xy( x, y );
            if (id != -1)
                grd[x][y] = trap_category( env.trap[id].type );

            return (false);
        }
    }

    // only consider terrain if player is not levitating
    if (!player_is_levitating())
    {
        // XXX: at some point we're going to need to fix the swimming
        // code to handle burder states.
        if (new_grid == DNGN_LAVA
            || (new_grid == DNGN_DEEP_WATER && you.species != SP_MERFOLK))
        {
            // lava and dangerous deep water (ie not merfolk)
            int entry_x = (stepped) ? you.x_pos : x;
            int entry_y = (stepped) ? you.y_pos : y;

            if (stepped && !force && !you.conf)
            {
                bool okay = yesno( "Do you really want to step there?", false );

                if (!okay)
                {
                    canned_msg(MSG_OK);
                    return (false);
                }
            }

            // have to move now so fall_into_a_pool will work
            you.x_pos = x;
            you.y_pos = y;

            viewwindow( true, false );

            // if true, we were shifted and so we're done.
            if (fall_into_a_pool( entry_x, entry_y, allow_shift, new_grid ))
                return (true);
        }
        else if (new_grid == DNGN_SHALLOW_WATER || new_grid == DNGN_DEEP_WATER)
        {
            // safer water effects
            if (you.species == SP_MERFOLK)
            {
                if (old_grid != DNGN_SHALLOW_WATER
                    && old_grid != DNGN_DEEP_WATER)
                {
                    if (stepped)
                        mpr("Your legs become a tail as you enter the water.");
                    else
                        mpr("Your legs become a tail as you dive into the water.");

                    merfolk_start_swimming();
                }
            }
            else
            {
                ASSERT( new_grid != DNGN_DEEP_WATER );

                if (!stepped)
                    noisy( SL_SPLASH, you.x_pos, you.y_pos, "Splash!" );

                you.time_taken *= 13 + random2(8);
                you.time_taken /= 10;

                if (old_grid != DNGN_SHALLOW_WATER)
                {
                    mpr( "You %s the shallow water.",
                          (stepped ? "enter" : "fall into") );

                    mpr("Moving in this stuff is going to be slow.");

                    if (you.invis)
                        mpr( "... and don't expect to remain undetected." );
                }
            }
        }
    }

    // move the player to location
    you.x_pos = x;
    you.y_pos = y;

    viewwindow( true, false );

    // Other Effects:
    // clouds -- do we need this? (always seems to double up with acr.cc call)
    // if (is_cloud( you.x_pos, you.y_pos ))
    //     in_a_cloud();

    // icy shield goes down over lava
    if (new_grid == DNGN_LAVA)
        expose_player_to_element( BEAM_LAVA );

    // traps go off:
    if (new_grid >= DNGN_TRAP_MECHANICAL && new_grid <= DNGN_UNDISCOVERED_TRAP)
    {
        id = trap_at_xy( you.x_pos, you.y_pos );

        if (id != -1)
        {
            bool trap_known = true;

            if (new_grid == DNGN_UNDISCOVERED_TRAP)
            {
                trap_known = false;

                const int type = trap_category( env.trap[id].type );

                grd[you.x_pos][you.y_pos] = type;
                set_envmap_char( you.x_pos, you.y_pos, get_sightmap_char(type) );
            }

            // not easy to blink onto a trap without setting it off:
            if (!stepped)
                trap_known = false;

            if (!player_is_levitating()
                || trap_category( env.trap[id].type ) != DNGN_TRAP_MECHANICAL)
            {
                handle_traps( env.trap[id].type, id, trap_known, false );
            }
        }
    }

    return (true);
}

bool player_in_mappable_area( void )
{
    return (you.level_type != LEVEL_LABYRINTH && you.level_type != LEVEL_ABYSS);
}

bool player_in_branch( int branch )
{
    return (you.level_type == LEVEL_DUNGEON && you.where_are_you == branch);
}

bool player_in_hell( void )
{
    return (you.level_type == LEVEL_DUNGEON
            && (you.where_are_you >= BRANCH_DIS
                && you.where_are_you <= BRANCH_THE_PIT)
            && you.where_are_you != BRANCH_VESTIBULE_OF_HELL);
}

bool player_in_water(void)
{
    return (!player_is_levitating()
            && (grd[you.x_pos][you.y_pos] == DNGN_DEEP_WATER
                || grd[you.x_pos][you.y_pos] == DNGN_SHALLOW_WATER));
}

bool player_is_swimming(void)
{
    return (you.species == SP_MERFOLK && player_in_water());
}

bool player_under_penance(void)
{
    return ((you.religion != GOD_NO_GOD) ? you.penance[you.religion] : false);
}

bool player_genus( int which_genus, int species )
{
    if (species == SP_UNKNOWN)
        species = you.species;

    switch (species)
    {
    case SP_RED_DRACONIAN:
    case SP_WHITE_DRACONIAN:
    case SP_GREEN_DRACONIAN:
    case SP_YELLOW_DRACONIAN:
    case SP_GREY_DRACONIAN:
    case SP_BLACK_DRACONIAN:
    case SP_PURPLE_DRACONIAN:
    case SP_MOTTLED_DRACONIAN:
    case SP_PALE_DRACONIAN:
    case SP_UNK0_DRACONIAN:
    case SP_UNK1_DRACONIAN:
    case SP_BASE_DRACONIAN:
        return (which_genus == GENPC_DRACONIAN);

    case SP_ELF:
    case SP_HIGH_ELF:
    case SP_GREY_ELF:
    case SP_DEEP_ELF:
    case SP_SLUDGE_ELF:
        return (which_genus == GENPC_ELVEN);

    case SP_HILL_DWARF:
    case SP_MOUNTAIN_DWARF:
        return (which_genus == GENPC_DWARVEN);

    default:
        break;
    }

    return (false);
}                               // end player_genus()

// returns true when player is in control of their actions:
bool player_sound_mind_and_body( void )
{
    return (!you.berserker && !you.conf && !you.duration[DUR_STUN]
            && !you.paralysis);
}

// Player is capable of countering the long melee ev bonus (halfted/whips).
bool player_long_melee( void )
{
    const size_type  body = player_size( PSIZE_BODY );
    const int        wpn  = get_inv_wielded();

    if (wpn != -1 && weapon_ev_bonus( you.inv[wpn], 0, body, you.dex ) > 0)
        return (true);

    // XXX: add case for non-weapon based long melee countering (ie giant size
    // or giant clubs (too awkward to use to keep opponents at bay, but capable
    // of hitting a target at the long distance?)).

    return (false);
}

// Returns the player's skill with wpn at keeping opponents at bay.
// Note: this doesn't check that the weapon is capable of getting the bonus!
int player_long_skill( const item_def &wpn )
{
    ASSERT( wpn.base_type == OBJ_WEAPONS || wpn.base_type == OBJ_STAVES );

    int ret = (skill_bump( weapon_skill(wpn) ) * 2
                + skill_bump( SK_DODGING )) / 3;

    return (ret);
}

// return player's bonus melee EV for long weapons
int player_weapon_ev_bonus( struct monsters *mon )
{
    ASSERT( mon );

    int ret = 0;

    // hafted weapons and whips now give EV bonus in melee
    const int wpn = get_inv_wielded();

    if (wpn != -1                               // only from wpns
        && !mons_long_melee( mon )              // monster doesn't have long
        && adjacent( you.x_pos, you.y_pos, mon->x, mon->y ) // adj
        && player_monster_visible( mon )        // can see opponent
        && player_sound_mind_and_body())        // in control of actions
    {
        const int        skill = player_long_skill( you.inv[wpn] );
        const size_type  body  = player_size( PSIZE_BODY );

        ret = weapon_ev_bonus( you.inv[wpn], skill, body, you.dex );
    }

    return (ret);
}

// Looks in equipment "slot" to see if there is an equiped "sub_type".
// Returns number of matches (in the case of rings, both are checked)
int player_equip( int slot, int sub_type, bool req_id )
{
    int ret = 0;
    int item;

    switch (slot)
    {
    case EQ_WEAPON:
        // Weapons must be wielded.
        item = get_inv_wielded();

        if (item != -1
            && you.inv[item].base_type == OBJ_WEAPONS
            && you.inv[item].sub_type == sub_type
            && (!req_id || item_ident(you.inv[item], ISFLAG_KNOW_TYPE)))
        {
            ret++;
        }
        break;

    case EQ_STAFF:
        // Magical stave don't need to be melee capable to count here.
        item = get_inv_hand_tool();

        if (item != -1
            && you.inv[item].base_type == OBJ_STAVES
            && you.inv[item].sub_type == sub_type
            && (!req_id ||
                item_ident(you.inv[you.equip[EQ_LEFT_RING]], ISFLAG_KNOW_TYPE)))
        {
            ret++;
        }
        break;

    case EQ_RINGS:
        if (you.equip[EQ_LEFT_RING] != -1
            && you.inv[you.equip[EQ_LEFT_RING]].sub_type == sub_type
            && (!req_id ||
               item_ident(you.inv[you.equip[EQ_LEFT_RING]], ISFLAG_KNOW_TYPE)))
        {
            ret++;
        }

        if (you.equip[EQ_RIGHT_RING] != -1
            && you.inv[you.equip[EQ_RIGHT_RING]].sub_type == sub_type
            && (!req_id ||
               item_ident(you.inv[you.equip[EQ_RIGHT_RING]], ISFLAG_KNOW_TYPE)))
        {
            ret++;
        }
        break;

    case EQ_RINGS_PLUS:
        if (you.equip[EQ_LEFT_RING] != -1
            && you.inv[you.equip[EQ_LEFT_RING]].sub_type == sub_type
            && (!req_id
                || item_ident( you.inv[you.equip[EQ_LEFT_RING]], ISFLAG_KNOW_PLUSES )))
        {
            ret += you.inv[you.equip[EQ_LEFT_RING]].plus;
        }

        if (you.equip[EQ_RIGHT_RING] != -1
            && you.inv[you.equip[EQ_RIGHT_RING]].sub_type == sub_type
            && (!req_id
                || item_ident( you.inv[you.equip[EQ_RIGHT_RING]], ISFLAG_KNOW_PLUSES )))
        {
            ret += you.inv[you.equip[EQ_RIGHT_RING]].plus;
        }
        break;

    case EQ_RINGS_PLUS2:
        if (you.equip[EQ_LEFT_RING] != -1
            && you.inv[you.equip[EQ_LEFT_RING]].sub_type == sub_type
            && (!req_id
                || item_ident( you.inv[you.equip[EQ_LEFT_RING]], ISFLAG_KNOW_PLUSES )))
        {
            ret += you.inv[you.equip[EQ_LEFT_RING]].plus2;
        }

        if (you.equip[EQ_RIGHT_RING] != -1
            && you.inv[you.equip[EQ_RIGHT_RING]].sub_type == sub_type
            && (!req_id
                || item_ident( you.inv[you.equip[EQ_RIGHT_RING]], ISFLAG_KNOW_PLUSES )))
        {
            ret += you.inv[you.equip[EQ_RIGHT_RING]].plus2;
        }
        break;

    case EQ_ALL_ARMOUR:
        // doesn't make much sense here... be specific. -- bwr
        break;

    default:
        if (you.equip[slot] != -1
            && you.inv[you.equip[slot]].sub_type == sub_type
            && (!req_id ||
                item_ident(you.inv[you.equip[slot]], ISFLAG_KNOW_TYPE)))
        {
            ret++;
        }
        break;
    }

    return (ret);
}


// Looks in equipment "slot" to see if equiped item has "special" ego-type
// Returns number of matches (jewellery returns zero -- no ego type).
int player_equip_ego_type( int slot, int special, bool req_id )
{
    int ret = 0;
    int wpn;

    switch (slot)
    {
    case EQ_WEAPON:
        // This actually checks against the "branding", so it will catch
        // randart brands, but not fixed artefacts.  -- bwr

        // Most weapon ego types require wielding to do anything (as they
        // add flavoured damage), protection is a "tool" type brand, so we
        // let it work when usable (maybe allow it at just "in hands"?)
        if (special == SPWPN_PROTECTION)
            wpn = get_inv_hand_tool();
        else
            wpn = get_inv_wielded();

        if (wpn != -1
            && you.inv[wpn].base_type == OBJ_WEAPONS
            && get_weapon_brand( you.inv[wpn] ) == special
            && (!req_id || item_ident(you.inv[wpn], ISFLAG_KNOW_TYPE)))
        {
            ret++;
        }
        break;

    case EQ_LEFT_RING:
    case EQ_RIGHT_RING:
    case EQ_AMULET:
    case EQ_STAFF:
    case EQ_RINGS:
    case EQ_RINGS_PLUS:
    case EQ_RINGS_PLUS2:
        // no ego types for these slots
        break;

    case EQ_ALL_ARMOUR:
        // Check all armour slots:
        for (int i = EQ_CLOAK; i <= EQ_BODY_ARMOUR; i++)
        {
            if (you.equip[i] != -1
                && get_armour_ego_type( you.inv[you.equip[i]] ) == special
                && (!req_id || item_ident(you.inv[you.equip[i]], ISFLAG_KNOW_TYPE)))
            {
                ret++;
            }
        }
        break;

    default:
        // Check a specific armour slot for an ego type:
        if (you.equip[slot] != -1
            && get_armour_ego_type( you.inv[you.equip[slot]] ) == special
            && (!req_id || item_ident(you.inv[you.equip[slot]], ISFLAG_KNOW_TYPE)))
        {
            ret++;
        }
        break;
    }

    return (ret);
}

static int player_barehand_damage_type( void )
{
    if (player_is_shapechanged(true))
        return (transform_hand_type());

    if (you.species == SP_TROLL || you.species == SP_GHOUL)
        return (DAM_SLICE);

    if (you.mutation[MUT_CLAWS])
        return (DAM_SLICE);

    return (DAM_BLUDGEON);
}

// like above but the claws need to be large enough
bool player_barehand_butcher( void )
{
    if (player_is_shapechanged( true ))
        return (transform_hand_type() == DAM_SLICE);

    return (you.species == SP_TROLL
            || you.species == SP_GHOUL
            || you.mutation[MUT_CLAWS] >= 2);
}

bool player_can_cut_meat( void )
{
    const int wpn = get_inv_hand_tool();
    const int gloves = you.equip[EQ_GLOVES];

    return ((wpn != -1 && can_cut_meat( you.inv[wpn] ))
            || (gloves == -1 && player_barehand_butcher())
            || player_size() >= SIZE_GIANT);
}

int player_damage_type( bool extra_unarmed )
{
    const int wpn = get_inv_wielded();
    const int gloves = you.equip[EQ_GLOVES];

    if (!extra_unarmed && wpn != -1)
        return (get_damage_type( you.inv[wpn] ));
    else if (gloves == -1)
        return (player_barehand_damage_type());

    return (DAM_BLUDGEON); // no weapon, covered hands
}

// returns brand of player's melee damage, ignore weapon if unarmed is true
int player_damage_brand( bool extra_unarmed )
{
    int ret = SPWPN_NORMAL;
    const int wpn = get_inv_wielded();

    if (!extra_unarmed && wpn != -1)
        ret = get_weapon_brand( you.inv[wpn] );
    else if (you.equip[EQ_GLOVES] == -1)
    {
        if (you.confusing_touch)
            ret = SPWPN_CONFUSING_TOUCH;
        else if (you.mutation[MUT_DRAIN_LIFE] == 3)
            ret = SPWPN_VAMPIRICISM;
        else if (you.mutation[MUT_DRAIN_LIFE] == 2)
            ret = SPWPN_DRAINING;
        else
            ret = transform_hand_brand();
    }

    return (ret);
}

// returns true if the player has a usable weapon in hand.
bool player_weapon_wielded( void )
{
    const int wpn = you.equip[EQ_WEAPON];

    if (wpn == -1)
        return (false);

    if (you.inv[wpn].base_type != OBJ_WEAPONS
        && you.inv[wpn].base_type != OBJ_STAVES)
    {
        return (false);
    }

    // should never have a bad "shape" weapon in hand
    ASSERT( check_weapon_shape( you.inv[wpn], false ) );

    if (!check_weapon_wieldable_size( you.inv[wpn], player_size() ))
        return (false);

    return (true);
}

// returns true if the player has a usable tool in hand.
bool player_tool_usable( void )
{
    return (get_inv_hand_tool() != -1);
}

// returns true if the player has a non-melee-weapon object in hand
bool player_is_bashing( void )
{
    const int  wpn = get_inv_in_hand();

    return (wpn != -1
            && (is_range_weapon( you.inv[wpn] )
                || !player_weapon_wielded()));
}

skill_type player_weapon_skill( void )
{
    if (get_inv_in_hand() == -1)
        return (SK_UNARMED_COMBAT);

    return (weapon_skill( you.inv[ you.equip[EQ_WEAPON] ] ));
}

int player_teleport(bool req_type_id)
{
    int tp = 0;

    /* rings */
    tp += 8 * player_equip( EQ_RINGS, RING_TELEPORTATION, req_type_id );

    /* mutations */
    tp += you.mutation[MUT_TELEPORT] * 3;

    /* randart weapons only */
    tp += scan_randarts( RAP_CAUSE_TELEPORTATION, req_type_id );

    return (tp);
}                               // end player_teleport()

int player_teleport_control( bool req_id )
{
    int tc = 0;

    tc += 3 * (you.duration[ DUR_CONTROL_TELEPORT ] > 0);
    tc += 3 * player_equip( EQ_RINGS, RING_TELEPORT_CONTROL, req_id );

    tc += you.mutation[ MUT_TELEPORT_CONTROL ];

    return ((tc > 3) ? 3 : tc);
}

int player_magic_regen(void)
{
    int mr = 7;

    if (you.mutation[MUT_CHANNEL_HELL] == 3)
        mr += (you.max_magic_points * 2) / 3;
    else
        mr += you.max_magic_points / 2;

    return (mr);
}

int player_regen(void)
{
    int rr = you.hp_max / 3;

    if (rr > 20)
        rr = 20 + ((rr - 20) / 2);

    // undead or dematerialized heal more slowly
    if (you.is_undead || you.attribute[ATTR_TRANSFORMATION] == TRAN_AIR)
        rr /= 2;
    else
    {
        // Only normal living bodies get benefits from regeneration.
        /* rings */
        rr += 40 * player_equip( EQ_RINGS, RING_REGENERATION );

        /* spell */
        if (you.duration[DUR_REGENERATION])
            rr += 100;

        /* troll or troll leather -- trolls can't get both */
        if (you.species == SP_TROLL && !transform_changed_physiology())
            rr += 40;
        else if (player_equip( EQ_BODY_ARMOUR, ARM_TROLL_LEATHER_ARMOUR ))
            rr += 30;

        /* fast heal mutation */
        rr += 20 * you.mutation[MUT_REGENERATION];

        // scale mutation
        rr += 20 * (you.mutation[MUT_GREY_SCALES] == 3);
    }

    // because 1 and 2 are far worse, we limit this to at least 3
    return ((rr < 3) ? 3 : rr);
}

int player_hunger_rate(void)
{
    int hunger = 3;

    if (you.is_undead == US_UNDEAD)
        return (0);

    // extra food requirements for large bodies or the very small
    const int size = player_size( PSIZE_BODY );
    if (size > SIZE_MEDIUM)
        hunger += 2 * (size - SIZE_MEDIUM) - 1;
    else if (size < SIZE_SMALL)
        hunger--;

    // food requirements by physiology
    if (transform_changed_physiology())
        hunger += transform_metabolism();
    else
    {
        switch (you.species)
        {
        case SP_HALFLING:
            hunger--;
            break;

        case SP_DEMIGOD:
            hunger++;
            break;

        case SP_TROLL:
            hunger += 5;
            break;
        }
    }

    if (you.duration[DUR_REGENERATION] > 0)
        hunger += 5;

    // moved invis/haste here from acr.cc
    if (you.invis > 0)
        hunger += 5;

    if (you.haste > 0)
        hunger += 5;

    // mutations
    hunger += you.mutation[ MUT_REGENERATION ];
    hunger += you.mutation[ MUT_FAST_METABOLISM ];
    hunger -= you.mutation[ MUT_SLOW_METABOLISM ];

    // rings
    hunger += 2 * player_equip( EQ_RINGS, RING_REGENERATION );
    hunger += 4 * player_equip( EQ_RINGS, RING_HUNGER );
    hunger -= 2 * player_equip( EQ_RINGS, RING_SUSTENANCE );

    // weapon ego types
    hunger += 6 * player_equip_ego_type( EQ_WEAPON, SPWPN_VAMPIRICISM );
    hunger += 9 * player_equip_ego_type( EQ_WEAPON, SPWPN_VAMPIRES_TOOTH );

    // troll leather armour
    // hunger += player_equip( EQ_BODY_ARMOUR, ARM_TROLL_LEATHER_ARMOUR );

    // boots of running
    hunger += player_equip_ego_type( EQ_BOOTS, SPARM_RUNNING );

    // randarts
    hunger += scan_randarts( RAP_METABOLISM );

    // burden
    hunger += you.burden_state;

    if (hunger < 1)
        hunger = (random2( 4 - hunger ) < 3); // 3/4, 3/5, 3/6, 3/7, ...

    return (hunger);
}

int player_total_spell_levels(void)
{
    int sl = (you.xp_level - 1) + (you.skills[SK_SPELLCASTING] * 2);

    return ((sl > 99) ? 99 : sl);
}

int player_spell_levels(void)
{
    int sl = player_total_spell_levels();

    bool fireball = false;
    bool delayed_fireball = false;

    for (int i = 0; i < 25; i++)
    {
        if (you.spells[i] == SPELL_FIREBALL)
            fireball = true;
        else if (you.spells[i] == SPELL_DELAYED_FIREBALL)
            delayed_fireball = true;

        if (you.spells[i] != SPELL_NO_SPELL)
            sl -= spell_level(you.spells[i]);
    }

    // Fireball is free for characters with delayed fireball
    if (fireball && delayed_fireball)
        sl += spell_level( SPELL_FIREBALL );

    // Note: This can happen because of level drain.  Maybe we should
    // force random spells out when that happens. -- bwr
    if (sl < 0)
        sl = 0;

    return (sl);
}

int player_res_magic(void)
{
    int rm = 0;

    // base by species and level:
    switch (you.species)
    {
    default:
        rm = you.xp_level * 3;
        break;

    case SP_ELF:
    case SP_GREY_ELF:
    case SP_HIGH_ELF:
    case SP_SLUDGE_ELF:
    case SP_HILL_DWARF:
    case SP_MOUNTAIN_DWARF:
    case SP_NAGA:
        rm = you.xp_level * 4;
        break;

    case SP_DEEP_ELF:
    case SP_GNOME:
    case SP_PURPLE_DRACONIAN:
        rm = you.xp_level * 5;
        break;

    case SP_SPRIGGAN:
        rm = you.xp_level * 6;
        break;
    }

    // enchantment/transmigration bonus based on current form
    const int form = transform_res_magic();

    if (form < 0)
        rm += form * you.xp_level; // effectively reduces species die size

    // Skill/intel bonuses are separate and independant of berserk.
    // Note: form bonus == 0 for the cases where transmig doesn't apply
    // so we apply enchantments instead, else apply positive transmig
    // skill modifier... negative modifiers are handled above and can
    // combine with berserk as they aren't intel based.
    if (you.berserker)
    {
        // Berserkers are particularly well focused, which limits the
        // effectiveness of hostile enchantments against them...
        rm += 15;

        // ... faithful Trogites more so.
        if (you.religion == GOD_TROG && !player_under_penance())
        {
            rm += 15;

            if (you.duration[DUR_PRAYER])
                rm += you.piety / 5;
        }
    }
    else if (form == 0)
        rm += stat_mult( you.intel, 2 * you.skills[SK_ENCHANTMENTS] );
    else if (form > 0)
        rm += stat_mult( you.intel, form * you.skills[SK_TRANSMIGRATION] );

    // staff
    rm += 30 * player_equip( EQ_STAFF, STAFF_ENCHANTMENT );

    // armour
    rm += 30 * player_equip_ego_type( EQ_ALL_ARMOUR, SPARM_MAGIC_RESISTANCE );

    // rings of magic resistance
    rm += 40 * player_equip( EQ_RINGS, RING_PROTECTION_FROM_MAGIC );

    // randarts
    rm += scan_randarts( RAP_MAGIC );

    // Mutations
    rm += 20 * you.mutation[MUT_MAGIC_RESISTANCE];

    if (!transform_changed_physiology())
        rm += 20 * (you.mutation[MUT_PURPLE_SCALES] == 3);

    return ((rm > 0) ? rm : 0);
}

int player_res_fire( bool req_type_id )
{
    int rf = 0;

    /* rings of fire resistance/fire */
    rf += player_equip( EQ_RINGS, RING_PROTECTION_FROM_FIRE, req_type_id );
    rf += player_equip( EQ_RINGS, RING_FIRE, req_type_id );

    /* rings of ice */
    rf -= player_equip( EQ_RINGS, RING_ICE, req_type_id );

    /* Staves */
    rf += player_equip( EQ_STAFF, STAFF_FIRE, req_type_id );

    // body armour:
    rf += 2 * player_equip( EQ_BODY_ARMOUR, ARM_DRAGON_ARMOUR );
    rf += player_equip( EQ_BODY_ARMOUR, ARM_GOLD_DRAGON_ARMOUR );
    rf -= player_equip( EQ_BODY_ARMOUR, ARM_ICE_DRAGON_ARMOUR );

    // ego armours
    rf += player_equip_ego_type( EQ_ALL_ARMOUR, SPARM_FIRE_RESISTANCE, req_type_id );
    rf += player_equip_ego_type( EQ_ALL_ARMOUR, SPARM_RESISTANCE, req_type_id );

    // randart weapons:
    rf += scan_randarts( RAP_FIRE, req_type_id );

    // transform/species:
    if (transform_changed_physiology())
        rf += transform_res_fire();
    else
    {
        if (you.species == SP_MUMMY)
            rf--;
        else if (you.species == SP_RED_DRACONIAN && you.xp_level >= 14)
            rf += ((you.xp_level - 2) / 12);
        else if ((you.species == SP_MOTTLED_DRACONIAN
                    || you.species == SP_PALE_DRACONIAN)
                && you.xp_level >= 18)
        {
            rf++;
        }

        // mutations:
        rf += you.mutation[MUT_HEAT_RESISTANCE];
        rf -= (you.mutation[MUT_COLD_RESISTANCE] > 1);
        rf += (you.mutation[MUT_RED_SCALES] == 3);
    }

    if (rf < -3)
        rf = -3;
    else if (rf > 3)
        rf = 3;

    return (rf);
}

int player_res_cold( bool req_type_id )
{
    int rc = 0;

    /* rings of fire resistance/fire */
    rc += player_equip( EQ_RINGS, RING_PROTECTION_FROM_COLD, req_type_id );
    rc += player_equip( EQ_RINGS, RING_ICE, req_type_id );

    /* rings of ice */
    rc -= player_equip( EQ_RINGS, RING_FIRE, req_type_id );

    /* Staves */
    rc += player_equip( EQ_STAFF, STAFF_COLD, req_type_id );

    // body armour:
    rc += 2 * player_equip( EQ_BODY_ARMOUR, ARM_ICE_DRAGON_ARMOUR );
    rc += player_equip( EQ_BODY_ARMOUR, ARM_GOLD_DRAGON_ARMOUR );
    rc -= player_equip( EQ_BODY_ARMOUR, ARM_DRAGON_ARMOUR );

    // ego armours
    rc += player_equip_ego_type( EQ_ALL_ARMOUR, SPARM_COLD_RESISTANCE, req_type_id );
    rc += player_equip_ego_type( EQ_ALL_ARMOUR, SPARM_RESISTANCE, req_type_id );

    // randart weapons:
    rc += scan_randarts( RAP_COLD, req_type_id );

    // transform/species:
    if (transform_changed_physiology())
        rc += transform_res_cold();
    else
    {
        if (you.is_undead)
            rc++;
        else if (you.species == SP_WHITE_DRACONIAN && you.xp_level >= 14)
            rc += ((you.xp_level - 2) / 12);

        // mutations:
        rc += you.mutation[MUT_COLD_RESISTANCE];
        rc -= (you.mutation[MUT_HEAT_RESISTANCE] > 1);
        rc += (you.mutation[MUT_WHITE_SCALES] == 3);
    }

    if (rc < -3)
        rc = -3;
    else if (rc > 3)
        rc = 3;

    return (rc);
}

int player_res_acid( void )
{
    return (!transform_changed_physiology()
            && ((you.species == SP_YELLOW_DRACONIAN && you.xp_level >= 7)
                || you.mutation[MUT_YELLOW_SCALES] == 3));
}

int player_res_electricity( bool req_type_id )
{
    int re = 0;

    if (you.duration[DUR_INSULATION])
        re++;

    // staff
    re += player_equip( EQ_STAFF, STAFF_AIR, req_type_id );

    // body armour:
    re += player_equip( EQ_BODY_ARMOUR, ARM_STORM_DRAGON_ARMOUR );

    // randart weapons:
    re += scan_randarts( RAP_ELECTRICITY, req_type_id );

    // species:
    if (transform_changed_physiology())
        re += transform_res_elec();
    else
    {
        if (you.species == SP_BLACK_DRACONIAN && you.xp_level >= 16)
            re++;

        // mutations:
        if (you.mutation[MUT_SHOCK_RESISTANCE])
            re++;

        if (you.mutation[MUT_BLUE_SCALES] == 3)
            re++;
    }

    if (you.attribute[ATTR_DIVINE_LIGHTNING_PROTECTION] > 0)
        re = 3;
    else if (re > 1)
        re = 1;

    return (re);
}                               // end player_res_electricity()

// funny that no races are susceptible to poisons {dlb}
int player_res_poison( bool req_type_id )
{
    int rp = 0;

    /* rings of poison resistance */
    rp += 3 * player_equip( EQ_RINGS, RING_POISON_RESISTANCE, req_type_id );

    /* Staves */
    rp += player_equip( EQ_STAFF, STAFF_POISON, req_type_id );

    /* the staff of Olgreb: */
    const int tool = get_inv_hand_tool();

    if (tool != -1
        && you.inv[tool].base_type == OBJ_WEAPONS
        && you.inv[tool].special == SPWPN_STAFF_OF_OLGREB)
    {
        rp += 3;
    }

    // ego armour:
    rp += player_equip_ego_type( EQ_ALL_ARMOUR, SPARM_POISON_RESISTANCE, req_type_id );

    // body armour:
    rp += player_equip( EQ_BODY_ARMOUR, ARM_GOLD_DRAGON_ARMOUR );
    rp += 3 * player_equip( EQ_BODY_ARMOUR, ARM_SWAMP_DRAGON_ARMOUR );

    // spells:
    if (you.duration[DUR_RESIST_POISON] > 0)
        rp += 3;

    // randart weapons:
    rp += scan_randarts( RAP_POISON, req_type_id );

    // species:
    if (transform_changed_physiology())
        rp += transform_res_pois();
    else
    {
        if (you.is_undead)
            rp += 3;
        else if (you.species == SP_NAGA)
            rp++;
        else if (you.species == SP_GREEN_DRACONIAN && you.xp_level >= 7)
            rp += (you.xp_level / 7);

        // mutations:
        rp += you.mutation[MUT_POISON_RESISTANCE];
        rp += (you.mutation[MUT_GREEN_SCALES] == 3);
    }

    if (rp > 3)
        rp = 3;

    return (rp);
}                               // end player_res_poison()

int player_spec_death(void)
{
    int sd = 0;

    /* Staves */
    sd += player_equip( EQ_STAFF, STAFF_DEATH );

    // body armour:
    if (player_equip_ego_type( EQ_BODY_ARMOUR, SPARM_ARCHMAGI ))
        sd++;

    // species:
    if (you.species == SP_MUMMY)
    {
        if (you.xp_level >= 13)
            sd++;
        if (you.xp_level >= 26)
            sd++;
    }

    // transformations:
    if (you.attribute[ATTR_TRANSFORMATION] == TRAN_LICH)
        sd++;

    return (sd);
}

int player_spec_holy(void)
{
    //if ( you.char_class == JOB_PRIEST || you.char_class == JOB_PALADIN )
    //  return 1;
    return (0);
}

int player_spec_fire(void)
{
    int sf = 0;

    // staves:
    sf += player_equip( EQ_STAFF, STAFF_FIRE );
    sf -= player_equip( EQ_STAFF, STAFF_COLD );

    // rings of fire:
    sf += player_equip( EQ_RINGS, RING_FIRE );
    sf -= player_equip( EQ_RINGS, RING_ICE );

    return (sf);
}

int player_spec_cold(void)
{
    int sc = 0;

    // staves:
    sc += player_equip( EQ_STAFF, STAFF_COLD );
    sc -= player_equip( EQ_STAFF, STAFF_FIRE );

    // rings of ice:
    sc += player_equip( EQ_RINGS, RING_ICE );
    sc -= player_equip( EQ_RINGS, RING_FIRE );

    return (sc);
}

int player_spec_earth(void)
{
    int se = 0;

    /* Staves */
    se += player_equip( EQ_STAFF, STAFF_EARTH );
    se -= player_equip( EQ_STAFF, STAFF_AIR );

    return (se);
}

int player_spec_air(void)
{
    int sa = 0;

    /* Staves */
    sa += player_equip( EQ_STAFF, STAFF_AIR );
    sa -= player_equip( EQ_STAFF, STAFF_EARTH );

    return (sa);
}

int player_spec_conj(void)
{
    int sc = 0;

    /* Staves */
    sc += player_equip( EQ_STAFF, STAFF_CONJURATION );

    // armour of the Archmagi
    if (player_equip_ego_type( EQ_BODY_ARMOUR, SPARM_ARCHMAGI ))
        sc++;

    return (sc);
}

int player_spec_ench(void)
{
    int se = 0;

    /* Staves */
    se += player_equip( EQ_STAFF, STAFF_ENCHANTMENT );

    // armour of the Archmagi
    if (player_equip_ego_type( EQ_BODY_ARMOUR, SPARM_ARCHMAGI ))
        se++;

    return (se);
}

int player_spec_summ(void)
{
    int ss = 0;

    /* Staves */
    ss += player_equip( EQ_STAFF, STAFF_SUMMONING );

    // armour of the Archmagi
    if (player_equip_ego_type( EQ_BODY_ARMOUR, SPARM_ARCHMAGI ))
        ss++;

    return (ss);
}

int player_spec_poison(void)
{
    int sp = 0;

    /* Staves */
    sp += player_equip( EQ_STAFF, STAFF_POISON );

    const int tool = get_inv_hand_tool();

    if (tool != -1
        && you.inv[tool].base_type == OBJ_WEAPONS
        && you.inv[tool].special == SPWPN_STAFF_OF_OLGREB)
    {
        sp++;
    }

    return (sp);
}

int player_energy(void)
{
    int pe = 0;

    // Staves
    pe += player_equip( EQ_STAFF, STAFF_ENERGY );

    return (pe);
}

int player_prot_life( bool req_type_id )
{
    int pl = 0;

    // rings:
    pl += player_equip( EQ_RINGS, RING_LIFE_PROTECTION, req_type_id );

    // armour:
    pl += player_equip_ego_type( EQ_ALL_ARMOUR, SPARM_POSITIVE_ENERGY );

    if (transform_changed_physiology())
        pl += transform_prot_life();
    else
    {
        if (you.is_undead)
            pl += 3;

        // demonic power
        pl += you.mutation[MUT_NEGATIVE_ENERGY_RESISTANCE];
        pl += (you.mutation[MUT_BLUE_SCALES] == 3);
    }

    // randarts
    pl += scan_randarts( RAP_NEGATIVE_ENERGY, req_type_id );

    if (pl > 3)
        pl = 3;

    return (pl);
}

// New player movement speed system... allows for a bit more that
// "player runs fast" and "player walks slow" in that the speed is
// actually calculated (allowing for centaurs to get a bonus from
// swiftness and other such things).  Levels of the mutation now
// also have meaning (before they all just meant fast).  Most of
// this isn't as fast as it used to be (6 for having anything), but
// even a slight speed advantage is very good... and we certainly don't
// want to go past 6 (see below). -- bwr
int player_movement_speed(void)
{
    int mv = 10;

    if (player_is_levitating())
    {
        // air movement:
        if (you.species == SP_KENKU)
        {
            // Always at home in the air, regardless of anything-else.
            if (you.burden_state == BS_UNENCUMBERED)
            {
                mv = (you.xp_level >= 25) ? 8 :
                     (you.xp_level >= 20) ? 9 : 10;
            }
        }
        else if (!player_is_shapechanged()
                || you.attribute[ATTR_TRANSFORMATION] != TRAN_DRAGON)
        {
            // - large and big races don't do so well in the air
            // - leviation is a slower, bobbing movement
            const size_type  size = player_size( PSIZE_BODY );
            const bool       fly  = player_control_flight();

            if (size > SIZE_MEDIUM)
                mv = (fly ?  9 : 11) + 2 * (size - SIZE_MEDIUM);
            else
                mv = (fly ? 10 : 12);
        }

        // Spells... swiftness is more effective off the ground.
        if (you.duration[DUR_SWIFTNESS])
            mv -= 3;
    }
    else if (player_in_water())
    {
        // water movement:
        if (player_is_swimming() && you.burden_state == BS_UNENCUMBERED)
            mv = 6;
        else
        {
            // Note: Non-swimmers also have an uncertain penalty that's
            // applied elsewhere... so 10 isn't as good as on land.

            // we're using profile here so that naga's can snake through water
            const size_type  size = player_size( PSIZE_PROFILE );

            if (size > SIZE_MEDIUM)
                mv = 11 + 2 * (size - SIZE_MEDIUM);
            else
                mv = 10;
        }
    }
    else
    {
        // ground movement:
        /* race and transformations */
        if (!player_is_shapechanged())
        {
            // Centaurs and spriggans are only fast in their regular
            // shape (ie untransformed, blade hands, lich form)
            if (you.species == SP_SPRIGGAN)
                mv = 6;
            else if (you.species == SP_CENTAUR || you.mutation[MUT_HOOVES] == 3)
                mv = 8;
        }
        else
        {
            // Note: statue form has a 1.5x time penalty on all actions,
            // so it doesn't need to be here.
            if (you.attribute[ATTR_TRANSFORMATION] == TRAN_SPIDER)
                mv = 8;
        }

        /* armour */
        if (player_equip_ego_type( EQ_BOOTS, SPARM_RUNNING ))
            mv -= 2;

        /* Mutations: -2, -3, -4 */
        if (you.mutation[MUT_FAST] > 0)
            mv -= (you.mutation[MUT_FAST] + 1);

        // Spells:
        if (you.duration[DUR_SWIFTNESS])
            mv -= 2;

        // Nagas move slowly over the ground:
        if (you.species == SP_NAGA && !player_is_shapechanged())
        {
            mv = (mv * 15) / 10;
            if (mv < 10)
                mv = 10;
        }
    }

    // Pnderousness should be handled like burden:
    if (player_equip_ego_type( EQ_BODY_ARMOUR, SPARM_PONDEROUSNESS ))
        mv += 2;

    // Burden: lev/fly already increases carrying which reduces this
    if (you.burden_state == BS_ENCUMBERED)
        mv += 2;
    else if (you.burden_state == BS_OVERLOADED)
        mv += 5;

    // We'll use the old value of six as a minimum, with haste this could
    // end up as a speed of three, which is about as fast as we want
    // the player to be able to go (since 3 is 3.33x as fast and 2 is 5x,
    // which is a bit of a jump, and a bit too fast) -- bwr
    if (mv < 6)
        mv = 6;

    return (mv);
}

speed_type player_net_speed_effect( void )
{
    int  ret = SPEED_NORMAL;

    if (you.slow || you.exhausted)
        ret -= 1;

    if (you.haste || you.berserker)
        ret += 1;

    return (static_cast< speed_type >( ret ));
}

// This function differs from movement rate in that it's used to set the
// initial time_taken value for the turn.  Everything else (movement,
// spellcasting, combat) applies a ratio to this value.
int player_base_time_taken( void )
{
    int ret = 10;

    if (player_is_shapechanged( false ))
    {
        if (you.attribute[ATTR_TRANSFORMATION] == TRAN_STATUE)
            ret = (ret * 15) / 10;
    }
    else
    {
        const size_type size = player_size( PSIZE_BODY );
        if (size > SIZE_BIG)
            ret = (ret * (9 + 3 * (size - SIZE_BIG))) / 10;
    }

    return (ret);
}

static unsigned long player_isflag_race_type( void )
{
    unsigned long ret = 0;

    if (player_genus( GENPC_DWARVEN ))
        ret = static_cast<unsigned long>( ISFLAG_DWARVEN );
    else if (player_genus( GENPC_ELVEN ))
        ret = static_cast<unsigned long>( ISFLAG_ELVEN );
    else if (you.species == SP_HILL_ORC)
        ret = static_cast<unsigned long>( ISFLAG_ORCISH );

    return (ret);
}

int player_armour_class(void)
{
    int ac = 0;
    int i;                      // loop variable

    // get the armour race value that corresponds to the character's race:
    const unsigned long racial_type = player_isflag_race_type();

    for (i = EQ_CLOAK; i <= EQ_BODY_ARMOUR; i++)
    {
        const int item = you.equip[i];

        if (item == -1 || i == EQ_SHIELD)
            continue;

        int ac_value = property( you.inv[item], PARM_AC );

        // The very large races have a harder time covering themselves
        // with body armour and so take a penalty like the deformed...
        // nagas and centaurs take an extra penalty because their extended
        // bodies leave parts of them exposed.
        if (i == EQ_BODY_ARMOUR && ac_value > 2)
        {
            const int odd_pen = (player_size( PSIZE_BODY ) > SIZE_LARGE)
                                    + (you.species == SP_NAGA
                                        || you.species == SP_CENTAUR)
                                    + you.mutation[MUT_DEFORMED];

            if (odd_pen > 0)
            {
                ac_value -= (ac_value * odd_pen) / (odd_pen + 2);

                if (ac_value < 1)
                    ac_value = 1;
            }
        }

        const unsigned long armour_race = get_equip_race( you.inv[item] );
        int racial_bonus = 0;  // additional levels of armour skill

        // Dwarven armour is universally good -- bwr
        if (armour_race == ISFLAG_DWARVEN)
            racial_bonus += 2;

        if (racial_type && armour_race == racial_type)
        {
            // Elven armour is light, but still gives one level
            // to elves.  Orcish and Dwarven armour are worth +2
            // to the correct species, plus the plus that anyone
            // gets with dwarven armour. -- bwr

            if (racial_type == ISFLAG_ELVEN)
                racial_bonus++;
            else
                racial_bonus += 2;
        }

        ac += ac_value * (15 + skill_bump(SK_ARMOUR) + racial_bonus) / 15;
        ac += you.inv[item].plus;
    }

    ac += player_equip( EQ_RINGS_PLUS, RING_PROTECTION );

    if (player_equip( EQ_STAFF, STAFF_EARTH ))
        ac += 5;

    if (player_equip_ego_type( EQ_WEAPON, SPWPN_PROTECTION ))
        ac += 5;

    // shield and cloaks:
    ac += 3 * player_equip_ego_type( EQ_ALL_ARMOUR, SPARM_PROTECTION );

    ac += scan_randarts( RAP_AC );

    if (you.duration[DUR_ICY_ARMOUR])
        ac += 4 + you.skills[SK_ICE_MAGIC] / 3;         // max 13

    if (you.duration[DUR_STONEMAIL])
        ac += 5 + you.skills[SK_EARTH_MAGIC] / 2;       // max 18

    if (you.duration[DUR_STONESKIN])
        ac += 2 + (you.skills[SK_EARTH_MAGIC] + 3) / 5; // max 8

    if (!transform_changed_physiology( true ))
    {
        // Being a lich doesn't preclude the benefits of hide/scales -- bwr
        //
        // Note: Even though necromutation is a high level spell, it does
        // allow the character full armour (so the bonus is low). -- bwr
        if (you.attribute[ATTR_TRANSFORMATION] == TRAN_LICH)
            ac += (3 + you.skills[SK_NECROMANCY] / 6);          // max 7

        switch (you.species)
        {
        case SP_NAGA:
            ac += 1 + you.xp_level / 4
                    + (you.xp_level == 27);             // max 8
            break;

        case SP_OGRE:
        case SP_CENTAUR:
        case SP_TROLL:
            ac += 2;
            break;

        case SP_GIANT:
            ac += 5;
            break;

        default:
            if (player_genus( GENPC_DRACONIAN ))
            {
                if (you.xp_level < 7)
                    ac += 2;
                else if (you.species == SP_GREY_DRACONIAN)
                    ac += 1 + (you.xp_level - 4) / 2;       // max 12
                else
                {
                    ac += 1 + you.xp_level / 4
                            + (you.xp_level == 27);         // max 8
                }
            }
            break;
        }

        ac += get_scale_property( PARM_AC );
    }
    else
    {
        // transformations:
        switch (you.attribute[ATTR_TRANSFORMATION])
        {
        case TRAN_NONE:
        case TRAN_BLADE_HANDS:
        case TRAN_LICH:
            // these three get hides/scales and are handled above
            break;

        case TRAN_SPIDER: // low level (small bonus), also gets EV
            ac += (2 + you.skills[SK_POISON_MAGIC] / 6);        // max 6
            break;

        case TRAN_ICE_BEAST:
            ac += (5 + (you.skills[SK_ICE_MAGIC] + 1) / 4);     // max 12

            if (you.duration[DUR_ICY_ARMOUR])
                ac += (1 + you.skills[SK_ICE_MAGIC] / 4);       // max +7
            break;

        case TRAN_DRAGON:
            ac += (7 + you.skills[SK_FIRE_MAGIC] / 3);          // max 16
            break;

        case TRAN_STATUE: // main ability is armour (high bonus)
            ac += (16 + (you.skills[SK_EARTH_MAGIC] + 1) / 2);  // max 30

            if (you.duration[DUR_STONESKIN] || you.duration[DUR_STONEMAIL])
                ac += (1 + you.skills[SK_EARTH_MAGIC] / 4);     // max +7
            break;

        case TRAN_SERPENT_OF_HELL:
            ac += (10 + you.skills[SK_FIRE_MAGIC] / 3);         // max 19
            break;

        case TRAN_AIR:    // air - scales & species ought to be irrelevent
            ac = (you.skills[SK_AIR_MAGIC] * 3) / 2;            // max 40
            break;

        default:
            break;
        }
    }

    return (ac);
}

bool is_light_armour( const item_def &item )
{
    ASSERT( item.base_type == OBJ_ARMOUR );

    if (get_equip_race( item ) == ISFLAG_ELVEN)
        return (true);

    return (base_armour_is_light( item ));
}

bool player_light_armour(void)
{
    if (you.equip[EQ_BODY_ARMOUR] == -1)
        return true;

    return (is_light_armour( you.inv[you.equip[EQ_BODY_ARMOUR]] ));
}                               // end player_light_armour()

//
// This function is used to tell if the charcter's shape has changed from
// their standard appearance (statues, blade hands, and statue form do
// not radically change the characters shape).  If check_hands is true
// then we're only interested in whether the character has usable hands.
//
bool player_is_shapechanged( bool check_hands )
{
    if (transform_size() == SIZE_CHARACTER)
    {
        if (check_hands)
            return (you.attribute[ATTR_TRANSFORMATION] == TRAN_BLADE_HANDS);

        return (false);
    }

    return (true);
}

// Returns true if PC's currently has the same physiology as "species".
bool check_species_physiology( species_type species )
{
    if (transform_changed_physiology())
        return (false);

    return (you.species == species);
}

// psize defaults to PSIZE_TORSO, which checks the part of the body
// that wears armour and wields weapons (which is different for some hybrids).
// base defaults to "false", meaning consider our current size, not our
// natural one.
size_type player_size( int psize, bool base )
{
    size_type  ret = (base) ? SIZE_CHARACTER : transform_size( psize );

    if (ret == SIZE_CHARACTER)
    {
        // transformation has size of character's species:
        switch (you.species)
        {
        case SP_GIANT:
            ret = SIZE_GIANT;
            break;

        case SP_OGRE:
        case SP_OGRE_MAGE:
        case SP_TROLL:
            ret = SIZE_LARGE;
            break;

        case SP_NAGA:
            // Most of their body is on the ground giving them a low profile.
            if (psize == PSIZE_TORSO || psize == PSIZE_PROFILE)
                ret = SIZE_MEDIUM;
            else
                ret = SIZE_BIG;
            break;

        case SP_CENTAUR:
            ret = (psize == PSIZE_TORSO) ? SIZE_MEDIUM : SIZE_BIG;
            break;

        case SP_SPRIGGAN:
            ret = SIZE_LITTLE;
            break;

        case SP_HALFLING:
        case SP_GNOME:
        case SP_KOBOLD:
            ret = SIZE_SMALL;
            break;

        default:
            ret = SIZE_MEDIUM;
            break;
        }
    }

    return (ret);
}

int player_evasion(void)
{
    const size_type  size  = player_size( PSIZE_BODY );
    const size_type  torso = player_size( PSIZE_TORSO );

    const int size_factor = SIZE_MEDIUM - size;
    int ev = 10 + 2 * size_factor;

    // Repulsion fields and size are all that matters when paralysed.
    if (you.paralysis)
    {
        ev = 2 + size_factor;

        if (you.mutation[MUT_REPULSION_FIELD] > 0)
            ev += (you.mutation[MUT_REPULSION_FIELD] * 2) - 1;

        return ((ev < 1) ? 1 : ev);
    }

    // Calculate the base bonus here, but it may be reduced by heavy
    // armour below.
    int dodge_bonus = (skill_bump(SK_DODGING) * you.dex + 10)
                                                / (20 - size_factor);

    // Limit on bonus from dodging:
    const int max_bonus = (you.skills[SK_DODGING] * (10 + size_factor)) / 9;
    if (dodge_bonus > max_bonus)
        dodge_bonus = max_bonus;

    // Some lesser armours have small penalties now (shields, barding)
    for (int i = EQ_CLOAK; i < EQ_BODY_ARMOUR; i++)
    {
        if (you.equip[i] == -1)
            continue;

        int pen = property( you.inv[ you.equip[i] ], PARM_EVASION );

        // reducing penalty of larger shields for larger characters
        if (i == EQ_SHIELD && torso > SIZE_MEDIUM)
            pen += (torso - SIZE_MEDIUM);

        if (pen < 0)
            ev += pen;
    }

    // handle main body armour penalty
    if (you.equip[EQ_BODY_ARMOUR] != -1)
    {
        // XXX: magnify arm_penalty for weak characters?
        // Remember: arm_penalty and ev_change are negative.
        int arm_penalty = property( you.inv[ you.equip[EQ_BODY_ARMOUR] ],
                                          PARM_EVASION );

        // The very large races take a penalty to AC for not being able
        // to fully cover, and in compensation we give back some freedom
        // of movement here.  Likewise, the very small are extra encumbered
        // by armour (which partially counteracts their size bonus above).
        if (size < SIZE_SMALL || size > SIZE_LARGE)
        {
            arm_penalty -= ((size - SIZE_MEDIUM) * arm_penalty) / 4;

            if (arm_penalty > 0)
                arm_penalty = 0;
        }

        int ev_change = arm_penalty;
        ev_change += (you.skills[SK_ARMOUR] * you.str) / 60;

        if (ev_change > arm_penalty / 2)
            ev_change = arm_penalty / 2;

        ev += ev_change;

        // This reduces dodging ability in heavy armour.
        if (!player_light_armour())
            dodge_bonus += (arm_penalty * 30 + 15) / you.str;
    }

    if (dodge_bonus > 0)                // always a bonus
        ev += dodge_bonus;

    if (you.duration[DUR_FORESCRY])
        ev += 5;

    if (you.duration[DUR_STONEMAIL])
        ev -= 2;

    ev += player_equip( EQ_RINGS_PLUS, RING_EVASION );
    ev += scan_randarts( RAP_EVASION );
    ev += get_scale_property( PARM_EVASION );

    if (player_equip_ego_type( EQ_BODY_ARMOUR, SPARM_PONDEROUSNESS ))
        ev -= 2;

    if (you.mutation[MUT_REPULSION_FIELD] > 0)
        ev += (you.mutation[MUT_REPULSION_FIELD] * 2) - 1;

    // transformation penalties/bonuses not covered by size alone:
    switch (you.attribute[ATTR_TRANSFORMATION])
    {
    case TRAN_STATUE:
        ev -= 5;                // stiff
        break;

    case TRAN_AIR:
        ev += 20;               // vapourous
        break;

    default:
        break;
    }

    return (ev);
}                               // end player_evasion()

// XXX: Not a good way to do this, but okay given that there's only one
// rare use for this information (shuffle card).  There's always
// the problem that there might be something missing here (this needs
// proper tracking)... at the very least this function suggests that
// there's a lot of work that needs doing here. -- bwr
int player_base_stat( int stat )
{
    int net  = 0;       // net value (ie max_*)
    int mut  = 0;       // value from mutations
    int ring = 0;       // value from rings
    int ego  = 0;       // value from equipment (gloves, rings, hats)
    int art  = 0;       // value from random artefacts
    int tran = 0;       // value from transformation
    int misc = 0;       // others

    switch (stat)
    {
    case STAT_STRENGTH:
        net = you.max_str;

        mut = you.mutation[MUT_STRONG] - you.mutation[MUT_WEAK]
                + you.mutation[MUT_STRONG_STIFF]
                - you.mutation[MUT_FLEXIBLE_WEAK];

        ring = player_equip( EQ_RINGS_PLUS, RING_STRENGTH );
        ego = 3 * player_equip_ego_type( EQ_ALL_ARMOUR, SPARM_STRENGTH );
        art = scan_randarts( RAP_STRENGTH );
        tran = transform_stat_bonus( STAT_STRENGTH );
        misc = 5 * (you.might > 0 || you.berserker > 0);
        break;

    case STAT_INTELLIGENCE:
        net = you.max_intel;
        mut = you.mutation[MUT_CLEVER] - you.mutation[MUT_DOPEY];
        ring = player_equip( EQ_RINGS_PLUS, RING_INTELLIGENCE );
        ego = 3 * player_equip_ego_type( EQ_ALL_ARMOUR, SPARM_INTELLIGENCE );
        art = scan_randarts( RAP_INTELLIGENCE );
        tran = transform_stat_bonus( STAT_INTELLIGENCE );
        break;

    case STAT_DEXTERITY:
        net = you.max_dex;

        mut = you.mutation[MUT_AGILE] - you.mutation[MUT_CLUMSY]
                - you.mutation[MUT_STRONG_STIFF]
                + you.mutation[MUT_FLEXIBLE_WEAK];

        ring = player_equip( EQ_RINGS_PLUS, RING_DEXTERITY );
        ego = 3 * player_equip_ego_type( EQ_ALL_ARMOUR, SPARM_DEXTERITY );
        art = scan_randarts( RAP_DEXTERITY );
        tran = transform_stat_bonus( STAT_DEXTERITY );
        break;
    }

    return (net - mut - ring - ego - art - tran - misc);
}

int player_magical_power( void )
{
    int ret = 0;

    ret += 13 * player_equip( EQ_STAFF, STAFF_POWER );
    ret +=  9 * player_equip( EQ_RINGS, RING_MAGICAL_POWER );

    return (ret);
}

int player_mag_abil( void )
{
    int ma = 0;

    ma += player_equip( EQ_RINGS, RING_WIZARDRY );

    /* Staves */
    ma += player_equip( EQ_STAFF, STAFF_WIZARDRY );

    /* armour of the Archmagi (checks body armour only) */
    ma += player_equip_ego_type( EQ_BODY_ARMOUR, SPARM_ARCHMAGI );

    return (ma);
}                               // end player_mag_abil()

int player_shield_class(void)   //jmf: changes for new spell
{
    int sh = 0;
    const int shield = you.equip[EQ_SHIELD];

    if (shield == -1)
    {
        if (!you.fire_shield && you.duration[DUR_CONDENSATION_SHIELD])
            sh = 2 + (skill_bump( SK_ICE_MAGIC ) / 5);  // max 8
    }
    else
    {
        const int base_value = property( you.inv[shield], PARM_AC );

        // modify by how much of our body we can cover:
        sh = base_value + (SIZE_MEDIUM - player_size( PSIZE_BODY ));

        // apply skill (stat applies to repeated use)
        if (you.skills[SK_SHIELDS])
            sh = (sh * (skill_bump( SK_SHIELDS ) + 10)) / 10;

        // apply plus
        sh += you.inv[shield].plus;
    }

    return (sh);
}                               // end player_shield_class()

bool player_can_block( struct monsters *mon )
{
    // mon is NULL for ranged bolt attacks
    const bool can_see_attack = ((mon) ? player_monster_visible(mon) : true);

    if (you.duration[DUR_CONDENSATION_SHIELD])  // doesn't require anything
        return (true);

    if (you.equip[EQ_SHIELD] != -1              // using a shield
        && player_sound_mind_and_body()         // in control of our actions
        && !you_are_delayed()                   // not busy
        && can_see_attack                       // can see the attack
        && player_shield_class() > 0)           // has an effective shield
    {
        return (true);
    }

    return (false);
}

bool player_repel_missiles( void )
{
    return (you.duration[DUR_REPEL_MISSILES] > 0
            || you.mutation[MUT_REPULSION_FIELD] == 3);
}

int player_see_invis( bool req_type_id )
{
    int si = 0;

    si += player_equip( EQ_RINGS, RING_SEE_INVISIBLE, req_type_id );

    /* armour: (checks head armour only) */
    si += player_equip_ego_type( EQ_HELMET, SPARM_SEE_INVISIBLE );

    /* Nagas & Spriggans have good eyesight */
    if (you.species == SP_NAGA || you.species == SP_SPRIGGAN)
        si++;

    if (you.mutation[MUT_ACUTE_VISION] == 3)
        si++;

    //jmf: added see_invisible spell
    if (you.duration[DUR_SEE_INVISIBLE] > 0)
        si++;

    /* randart wpns */
    si += scan_randarts( RAP_EYESIGHT, req_type_id );

    return (si);
}

bool player_can_smell( void )
{
    return (you.species != SP_MUMMY);
}

// This does NOT do line of sight!  It checks the monster's visibility
// with repect to the players perception, but doesn't do walls or range...
// to find if the square the monster is in is visible see mons_near().
bool player_monster_visible( struct monsters *mon )
{
    if (you.mutation[MUT_ACUTE_VISION] >= 2
        && grid_distance( mon->x, mon->y, you.x_pos, you.y_pos ) <= 1)
    {
        return (true);
    }

    if (mons_is_submerged( mon )
        || (mons_is_invisible( mon ) && !player_see_invis()))
    {
        return (false);
    }

    return (true);
}

int player_sust_abil( bool req_type_id )
{
    int sa = 0;

    sa += player_equip( EQ_RINGS, RING_SUSTAIN_ABILITIES, req_type_id );

    return (sa);
}                               // end player_sust_abil()

// Note that size isn't used here because str is doing that well enough.
// If we want a size factor, we could measure carrying volume and use
// item_size... but this function is simply mass vs mass.
int carrying_capacity( void )
{
    // Originally: you.str * 200 + (you.levitation ? 2000 : 1000)
    // return (you.str * 100 + (player_is_levitating() ? 4500 : 3500));
    return (you.str * 160 + (player_is_levitating() ? 3500 : 2200));
}

int burden_change(void)
{
    char old_burdenstate = you.burden_state;

    you.burden = 0;

    int max_carried = carrying_capacity();

    if (you.duration[DUR_STONEMAIL])
        you.burden += 800;

    for (int bu = 0; bu < ENDOFPACK; bu++)
    {
        if (you.inv[bu].quantity < 1)
            continue;

        if (you.inv[bu].base_type == OBJ_CORPSES)
        {
            if (you.inv[bu].sub_type == CORPSE_BODY)
                you.burden += mons_weight(you.inv[bu].plus);
            else if (you.inv[bu].sub_type == CORPSE_SKELETON)
                you.burden += mons_weight(you.inv[bu].plus) / 2;
        }
        else
        {
            you.burden += item_mass( you.inv[bu] ) * you.inv[bu].quantity;
        }
    }

    you.burden_state = BS_UNENCUMBERED;

    // changed the burdened levels to match the change to max_carried
    // changed again to 85% and 95% (nicer than 5/6 and 11/12 for display)
    if (you.burden < (max_carried * 85) / 100)
    // (you.burden < max_carried - 1000)
    {
        you.burden_state = BS_UNENCUMBERED;

        // this message may have to change, just testing {dlb}
        if (old_burdenstate != you.burden_state)
            mpr("Your possessions no longer seem quite so burdensome.");
    }
    else if (you.burden < (max_carried * 95) / 100)
    // (you.burden < max_carried - 500)
    {
        you.burden_state = BS_ENCUMBERED;

        if (old_burdenstate != you.burden_state)
            mpr("You are being weighed down by all of your possessions.");
    }
    else
    {
        you.burden_state = BS_OVERLOADED;

        if (old_burdenstate != you.burden_state)
            mpr("You are being crushed by all of your possessions.");
    }

    // Stop travel if we get burdened (as from potions of might/levitation
    // wearing off).
    if (you.burden_state > old_burdenstate)
        interrupt_activity( AI_BURDEN_CHANGE );

    return (you.burden);
}                               // end burden_change()

bool you_resist_magic( int power )
{
    const int resist = player_res_magic();

    // power = stepdown_value( power, 30, 40, 100, 120 );

    const int target = 100 + resist - power;
    const int roll = random2(101) + random2(101);

#if DEBUG_DIAGNOSTICS
    int num;

    // based on enumerating the two triangles of the roll distribution
    if (target <= 0)
        num = 0;
    else if (target <= 100)
        num = (target * (target + 1)) / 2;
    else if (target >= 201)
        num = 10201;
    else
    {
        const int side = 201 - target;
        num = 10201 - (side * (side + 1)) / 2;
    }

    mpr( MSGCH_DIAGNOSTICS,
          "power: %d, MR: %d, target: %d (%0.2f%%), roll: %d",
          power, resist, target, static_cast<float>(num) / 102.01, roll );
#endif

    return (roll < target);
}

void forget_map( int chance_forgotten )
{
    int xcount, ycount = 0;

    // must forget within the map boundary (one larger than the players)
    for (xcount = X_BOUND_1; xcount <= X_BOUND_2; xcount++)
    {
        for (ycount = Y_BOUND_1; ycount <= Y_BOUND_2; ycount++)
        {
            if (random2(100) < chance_forgotten)
            {
                if (trap_at_xy( xcount, ycount ) != -1)
                    grd[xcount][ycount] = DNGN_UNDISCOVERED_TRAP;

                clear_envmap_grid( xcount, ycount );
            }
        }
    }
}                               // end forget_map()

void gain_exp( unsigned int exp_gained )
{

    if (player_equip_ego_type( EQ_BODY_ARMOUR, SPARM_ARCHMAGI )
        && !one_chance_in(20))
    {
        return;
    }

#if DEBUG_DIAGNOSTICS
    mpr( MSGCH_DIAGNOSTICS, "gain_exp: %d", exp_gained );
#endif

    if (you.xp + exp_gained > 8999999)
        you.xp = 8999999;
    else
        you.xp += exp_gained;

    if (you.exp_available + exp_gained > 20000)
        you.exp_available = 20000;
    else
        you.exp_available += exp_gained;

    level_change();
}                               // end gain_exp()

static bool every_nth_level( int n )
{
    // the n == 4 case makes # of stat gains 5, 7, 9 instead of 5, 6, 9
    return ((you.xp_level % n) == 0
            || (n == 4 && you.xp_level == 27));
}

void level_change(void)
{
    // necessary for the time being, as level_change() is called
    // directly sometimes {dlb}
    set_redraw_status( REDRAW_EXPERIENCE );

    while (you.xp_level < 27
            && you.xp > exp_needed(you.xp_level + 2))
    {
        int hp_adjust = 0;
        int mp_adjust = 0;

        you.xp_level++;

        if (you.xp_level <= you.max_xp_level)
        {
            mpr( MSGCH_INTRINSIC_GAIN, "Welcome back to level %d!", you.xp_level );
            more();

            // Gain back the hp and mp we lose in lose_level().  -- bwr
            inc_hp( 4, true );
            inc_mp( 1, true );
        }
        else  // character has gained a new level
        {
            you.max_xp_level = you.xp_level;

            // Using species instead of class here now because it
            // makes more sense... classes mean nothing during
            // the game (especially the religious ones if you choose
            // to leave), whereas experience level is actually applied
            // to various species abilities like breath weapons.  Species
            // determines the experience required for this anyways.
            mpr( MSGCH_INTRINSIC_GAIN, "You are now a level %d %s!",
                  you.xp_level, species_name( you.species ) );
            more();

            int hd = 0;

            if (you.xp_level > 21)
                hd = (coinflip() ? 3 : 2);
            else if (you.xp_level > 12)
                hd = 2 + random2(3);
            else
                hd = 4 + random2(3);      // up from 3 + random2(4) -- bwr

            inc_hp( hd, true );
            inc_mp( 1, true );

            if (every_nth_level(3))
                ability_increase();

            switch (you.species)
            {
            case SP_HUMAN:
                if (every_nth_level(4))
                    modify_stat( STAT_RANDOM, 1, false );
                break;

            case SP_ELF:
                if (every_nth_level(3))
                    mp_adjust++;
                else
                    hp_adjust--;

                if (every_nth_level(4))
                    modify_stat( random_stat(0, 50, 50), 1, false );
                break;

            case SP_HIGH_ELF:
                //jmf: Glamour ability
                if (you.xp_level == 15)
                    mpr( MSGCH_INTRINSIC_GAIN, "You feel charming!" );

                if (!every_nth_level(3))
                    hp_adjust--;

                if (every_nth_level(2))
                    mp_adjust++;

                if (every_nth_level(3))
                    modify_stat( random_stat(0, 70, 30), 1, false );
                break;

            case SP_GREY_ELF:
                //jmf: Glamour ability
                if (you.xp_level == 5)
                    mpr( MSGCH_INTRINSIC_GAIN, "You feel charming!" );

                if (you.xp_level < 14)
                    hp_adjust--;

                if (!every_nth_level(3))
                    mp_adjust++;

                if (every_nth_level(4))
                    modify_stat( random_stat(0, 60, 40), 1, false );
                break;

            case SP_DEEP_ELF:
                if (you.xp_level < 17)
                    hp_adjust--;

                if (every_nth_level(2))
                    hp_adjust--;

                if (!every_nth_level(4))
                    mp_adjust++;

                if (every_nth_level(4))
                    modify_stat( random_stat(0, 80, 20), 1, false );
                break;

            case SP_SLUDGE_ELF:
                if (every_nth_level(3))
                    mp_adjust++;
                else
                    hp_adjust--;

                if (every_nth_level(3))
                    modify_stat( random_stat(20, 40, 40), 1, false );
                break;

            case SP_HILL_DWARF:
                if (!every_nth_level(3))
                    hp_adjust++;

                if (every_nth_level(2))
                    mp_adjust--;

                if (every_nth_level(4))
                    modify_stat( random_stat(80, 0, 20), 1, false );
                break;

            case SP_MOUNTAIN_DWARF:
                if (every_nth_level(2))
                    hp_adjust++;

                if (every_nth_level(3))
                    mp_adjust--;

                if (every_nth_level(4))
                    modify_stat( random_stat(70, 10, 20), 1, false );
                break;

            case SP_HALFLING:
                if (you.xp_level < 17)
                    hp_adjust--;

                if (every_nth_level(2))
                    hp_adjust--;

                if (every_nth_level(4))
                    modify_stat( random_stat(0, 20, 80), 1, false );
                break;

            case SP_KOBOLD:
                if (you.xp_level < 17)
                    hp_adjust--;

                if (every_nth_level(3))
                    hp_adjust--;

                if (every_nth_level(5))
                    modify_stat( random_stat(30, 0, 70), 1, false );
                break;

            case SP_HILL_ORC:
                if (every_nth_level(2))
                    hp_adjust++;

                if (every_nth_level(3))
                    mp_adjust--;

                if (every_nth_level(5))
                    modify_stat( random_stat(70, 0, 30), 1, false );
                break;

            case SP_MUMMY:
                if (you.xp_level == 13 || you.xp_level == 26)
                {
                    mpr( MSGCH_INTRINSIC_GAIN,
                         "You feel more in touch with the powers of death." );
                }

                if (you.xp_level == 13)  // level 13 for now -- bwr
                {
                    mpr( MSGCH_INTRINSIC_GAIN,
                         "You can now infuse your body with magic to restore decomposition." );
                }
                break;

            case SP_NAGA:
                if (every_nth_level(2))
                    hp_adjust++;

                if (every_nth_level(5))
                    modify_stat( random_stat(40, 40, 20), 1, false );

                if (every_nth_level(4))
                {
                    mpr(MSGCH_INTRINSIC_GAIN,"Your skin feels tougher." );
                    set_redraw_status( REDRAW_ARMOUR_CLASS );
                }
                break;

            case SP_GNOME:
                if (you.xp_level < 13)
                    hp_adjust--;

                if (every_nth_level(3))
                    hp_adjust--;

                if (every_nth_level(4))
                    modify_stat( random_stat(0, 50, 50), 1, false );
                break;

            case SP_OGRE:
            case SP_TROLL:
                hp_adjust++;

                if (every_nth_level(2))
                    hp_adjust++;

                if (!every_nth_level(3))
                    mp_adjust--;

                if (every_nth_level(3))
                    modify_stat( random_stat(90, 0, 10), 1, false );
                break;

            case SP_GIANT:
                hp_adjust += 2;

                if (!every_nth_level(3))
                    mp_adjust--;

                if (every_nth_level(3))
                    modify_stat( STAT_STRENGTH, 1, false );
                break;

            case SP_OGRE_MAGE:
                hp_adjust++;

                if (every_nth_level(4))
                    modify_stat( random_stat(50, 50, 0), 1, false );
                break;

            case SP_RED_DRACONIAN:
            case SP_WHITE_DRACONIAN:
            case SP_GREEN_DRACONIAN:
            case SP_YELLOW_DRACONIAN:
            case SP_BLACK_DRACONIAN:
            case SP_PURPLE_DRACONIAN:
            case SP_MOTTLED_DRACONIAN:
            case SP_PALE_DRACONIAN:
            case SP_UNK0_DRACONIAN:
            case SP_UNK1_DRACONIAN:
            case SP_BASE_DRACONIAN:
            /* Grey is later */
                if (you.xp_level == 7)
                {
                    switch (you.species)
                    {
                    case SP_RED_DRACONIAN:
                        mpr(MSGCH_INTRINSIC_GAIN,"Your scales start taking on a fiery red colour." );
                        break;
                    case SP_WHITE_DRACONIAN:
                        mpr(MSGCH_INTRINSIC_GAIN,"Your scales start taking on an icy white colour." );
                        break;
                    case SP_GREEN_DRACONIAN:
                        mpr(MSGCH_INTRINSIC_GAIN,"Your scales start taking on a green colour." );
                        break;

                    case SP_YELLOW_DRACONIAN:
                        mpr(MSGCH_INTRINSIC_GAIN,"Your scales start taking on a golden yellow colour." );
                        break;
                    case SP_BLACK_DRACONIAN:
                        mpr(MSGCH_INTRINSIC_GAIN,"Your scales start turning black." );
                        break;
                    case SP_PURPLE_DRACONIAN:
                        mpr(MSGCH_INTRINSIC_GAIN,"Your scales start taking on a rich purple colour." );
                        break;
                    case SP_MOTTLED_DRACONIAN:
                        mpr(MSGCH_INTRINSIC_GAIN,"Your scales start taking on a weird mottled pattern." );
                        break;
                    case SP_PALE_DRACONIAN:
                        mpr(MSGCH_INTRINSIC_GAIN,"Your scales start fading to a pale grey colour." );
                        break;
                    case SP_UNK0_DRACONIAN:
                    case SP_UNK1_DRACONIAN:
                    case SP_BASE_DRACONIAN:
                        break;
                    }

                    recalc_drac_skill_points();
                    more();
                    redraw_screen();
                }

                if (you.species == SP_GREEN_DRACONIAN
                    && you.xp_level % 7 == 0)
                {
                    mpr( MSGCH_INTRINSIC_GAIN, "You feel %s to poison.",
                          (you.xp_level ==  7) ? "resistant" :
                          (you.xp_level == 14) ? "more resistant"
                                                       : "immune" );
                }

                if ((you.species == SP_RED_DRACONIAN
                        || you.species == SP_WHITE_DRACONIAN)
                    && (you.xp_level - 2) % 12 == 0)
                {
                    mpr( MSGCH_INTRINSIC_GAIN, "You feel %sresistant to %s.",
                          (you.xp_level == 14) ? "" : "more ",
                          (you.species == SP_RED_DRACONIAN) ? "fire"
                                                            : "cold" );
                }

                if ((you.species == SP_MOTTLED_DRACONIAN
                        || you.species == SP_PALE_DRACONIAN)
                    && you.xp_level == 18)
                {
                    mpr( MSGCH_INTRINSIC_GAIN, "You feel resistant to fire." );
                }

                if (you.species == SP_BLACK_DRACONIAN
                    && you.xp_level == 16)
                {
                    mpr(MSGCH_INTRINSIC_GAIN,"You feel resistant to electrical energy." );
                }

                if (every_nth_level(3))
                    hp_adjust++;

                if (every_nth_level(4))
                {
                    if (you.xp_level > 7 && you.species == SP_PURPLE_DRACONIAN)
                        modify_stat( random_stat( 0, 80, 20), 1, false );
                    else
                        modify_stat( random_stat(60, 20, 20), 1, false );
                }

                if (you.xp_level >= 7 && every_nth_level(4))
                {
                    mpr(MSGCH_INTRINSIC_GAIN,"Your scales feel tougher." );
                    set_redraw_status( REDRAW_ARMOUR_CLASS );
                }
                break;

            case SP_GREY_DRACONIAN:
                if (you.xp_level == 7)
                {
                    mpr(MSGCH_INTRINSIC_GAIN,"Your scales start turning grey." );
                    recalc_drac_skill_points();
                    more();
                    redraw_screen();
                }

                if (every_nth_level(3))
                {
                    hp_adjust++;
                    if (you.xp_level > 7)
                        hp_adjust++;
                }

                // First two gains 1 level late to hide the colour
                if (you.xp_level == 4)
                    modify_stat( random_stat(60, 20, 20), 1, false );
                else if ((you.xp_level > 7 && every_nth_level(3))
                        || you.xp_level == 7)
                {
                    modify_stat( random_stat(40, 20, 40), 1, false );
                }

                if (you.xp_level >= 7 && every_nth_level(2))
                {
                    mpr(MSGCH_INTRINSIC_GAIN,"Your scales feel tougher." );
                    set_redraw_status( REDRAW_ARMOUR_CLASS );
                }
                break;

            case SP_CENTAUR:
                if (every_nth_level(2))
                    hp_adjust++;

                if (every_nth_level(3))
                    mp_adjust--;

                if (every_nth_level(5))
                    modify_stat( random_stat(75, 25, 0), 1, false );
                break;

            case SP_DEMIGOD:
                if (every_nth_level(2))
                    hp_adjust++;

                if (!every_nth_level(3))
                    mp_adjust++;

                if (every_nth_level(3))
                    modify_stat( STAT_RANDOM, 1, false );
                break;

            case SP_SPRIGGAN:
                if (you.xp_level < 17)
                    hp_adjust--;

                if (!every_nth_level(3))
                    hp_adjust--;

                mp_adjust++;

                if (every_nth_level(5))
                    modify_stat( random_stat(0, 50, 50), 1, false );
                break;

            case SP_MINOTAUR:
                if (every_nth_level(2))
                    hp_adjust++;

                if (every_nth_level(2))
                    mp_adjust--;

                if (every_nth_level(4))
                    modify_stat( random_stat(80, 0, 20), 1, false );
                break;

            case SP_DEMONSPAWN:
                if (every_nth_level(4))
                    modify_stat( STAT_RANDOM, 1, false );

                if (every_nth_level(3))
                    demonspawn();
                break;

            case SP_GHOUL:
                if (every_nth_level(2))
                {
                    hp_adjust--;
                    mp_adjust--;
                }

                if (every_nth_level(5))
                    modify_stat( STAT_STRENGTH, 1, false );
                break;

            case SP_KENKU:
                if (you.xp_level < 17)
                    hp_adjust--;

                if (every_nth_level(3))
                    hp_adjust--;

                if (every_nth_level(4))
                    modify_stat( STAT_RANDOM, 1, false );

                if (you.xp_level == 5)
                    mpr(MSGCH_INTRINSIC_GAIN,"You have gained the ability to fly." );
                else if (you.xp_level == 15)
                    mpr( MSGCH_INTRINSIC_GAIN,"You can now fly continuously." );
                else if (you.xp_level == 20)
                    mpr( MSGCH_INTRINSIC_GAIN,"You can now fly faster." );
                else if (you.xp_level == 25)
                    mpr( MSGCH_INTRINSIC_GAIN,"You can now fly faster yet." );
                break;

            case SP_MERFOLK:
                if (every_nth_level(3))
                    mp_adjust--;
                else
                    hp_adjust++;

                if (every_nth_level(4))
                    modify_stat( STAT_RANDOM, 1, false );
                break;
            }
        }

        // add hp and mp adjustments - GDL
        inc_max_hp( hp_adjust );
        inc_max_mp( mp_adjust );

        if (you.magic_points < 0)
            you.magic_points = 0;

        calc_hp_max();
        calc_mp_max();

        if (you.religion == GOD_XOM)
            Xom_acts(true, you.xp_level, true);
    }

    set_redraw_status( REDRAW_SKILL );
}                               // end level_change()

// here's a question for you: does the ordering of mods make a difference?
// (yes) -- are these things in the right order of application to stealth?
// - 12mar2000 {dlb}
int player_stealth(void)
{
    if (you.special_wield == SPWLD_SHADOW       // evil presence
        || you.berserker                        // not concerned with stealth
        || you.duration[DUR_SILENCE])           // large negative noise
    {
        return (0);
    }

    int ret = 15;

    const size_type  size = player_size( PSIZE_PROFILE );

    int fact = 10 + 2 * (SIZE_MEDIUM - size);

    if (!transform_changed_physiology( true )
        && you.mutation[MUT_PATTERNED_SCALES] > 1)
    {
        // camouflage increases skill factor
        fact += (you.mutation[MUT_PATTERNED_SCALES] * 5) / 3;
    }

    // apply skill:
    ret += fact * skill_bump(SK_STEALTH);

    // apply dexterity:
    ret = (ret * (you.dex + 5)) / 20;

    const int arm   = you.equip[EQ_BODY_ARMOUR];
    const int cloak = you.equip[EQ_CLOAK];
    const int boots = you.equip[EQ_BOOTS];

    // strength reduces penalty from heavy armour
    if (arm != -1 && !player_light_armour())
        ret -= (item_mass( you.inv[arm] ) * 20) / (you.str + 5);

    // check specific capabilities for land, sea, and air:
    if (player_is_levitating())
        ret += 20;
    else if (player_in_water())
    {
        // Merfolk can sneak up on monsters underwater -- bwr
        if (you.species == SP_MERFOLK)
            ret += 50;
        else
            ret -= 50;       // splashy-splashy
    }
    else if (boots != -1)   // boots only apply when walking on ground
    {
        if (get_armour_ego_type( you.inv[boots] ) == SPARM_STEALTH)
            ret += 50;

        if (get_equip_race( you.inv[boots] ) == ISFLAG_ELVEN)
            ret += 20;
    }

    if (cloak != -1 && get_equip_race( you.inv[cloak] ) == ISFLAG_ELVEN)
        ret += 20;

    // 10-80, avg 45
    ret += scan_randarts( RAP_STEALTH );

    // magic glow -- not counting when invisible to be nice
    if (!you.invis && you.magic_contamination > 5)
        ret -= 5 * (you.magic_contamination - 5);

    // factors which reduce our stealth to a fraction:
    int div_pen = 0;
    if (you.burden_state == BS_ENCUMBERED)
        div_pen = 2;
    else if (you.burden_state == BS_OVERLOADED)
        div_pen = 5;

    if (you.conf)
        div_pen += 3;

    if (div_pen)
        ret = (ret / div_pen) - 5 * div_pen;

    if (ret < 0)
        ret = 0;

    return (ret);
}                               // end player_stealth()

void ability_increase(void)
{
    char keyin;

    mpr(MSGCH_INTRINSIC_GAIN,"Your experience leads to an increase in your attributes!" );

    more();
    mesclr();

    mpr(MSGCH_PROMPT,"Increase (S)trength, (I)ntelligence, or (D)exterity? " );

    for (;;)
    {
        keyin = getch();
        if (keyin == 0)
        {
            getch();
            continue;
        }

        switch (keyin)
        {
        case 's':
        case 'S':
            modify_stat( STAT_STRENGTH, 1, false );
            return;

        case 'i':
        case 'I':
            modify_stat( STAT_INTELLIGENCE, 1, false );
            return;

        case 'd':
        case 'D':
            modify_stat( STAT_DEXTERITY, 1, false );
            return;
        }
    }
}                               // end ability_increase()

void display_char_status(void)
{
    if (you.is_undead)
        mpr( "You are undead." );
    else if (you.deaths_door)
        mpr( "You are standing in death's doorway." );
    else
        mpr( "You are alive." );

    switch (you.attribute[ATTR_TRANSFORMATION])
    {
    case TRAN_SPIDER:
        mpr( "You are in spider-form." );
        break;
    case TRAN_BLADE_HANDS:
        mpr( "You have blades for hands." );
        break;
    case TRAN_STATUE:
        mpr( "You are a statue." );
        break;
    case TRAN_ICE_BEAST:
        mpr( "You are an ice creature." );
        break;
    case TRAN_DRAGON:
        mpr( "You are in dragon-form." );
        break;
    case TRAN_LICH:
        mpr( "You are in lich-form." );
        break;
    case TRAN_SERPENT_OF_HELL:
        mpr( "You are a huge demonic serpent." );
        break;
    case TRAN_AIR:
        mpr( "You are a cloud of diffuse gas." );
        break;
    }

    if (you.duration[DUR_LIQUID_FLAMES])
        mpr( "You are covered in liquid flames." );

    if (you.duration[DUR_ICY_ARMOUR])
        mpr( "You are protected by an icy ehield." );

    // Note: repel and deflect get printed out separately here because
    // there is nothing preventing both counters from running at the
    // same time.  They are non-cumulative in effect, however.
    if (you.duration[DUR_REPEL_MISSILES])
        mpr( "You are protected from missiles." );

    if (you.duration[DUR_DEFLECT_MISSILES])
        mpr( "You deflect missiles." );

    if (you.duration[DUR_PRAYER])
        mpr( "You are praying." );

    if (you.duration[DUR_REGENERATION])
        mpr( "You are regenerating." );

    if (you.duration[DUR_REGENERATION])
        mpr( "You are regenerating." );

    if (you.duration[DUR_INSULATION])
        mpr( "You are insulated." );

    if (you.duration[DUR_STONEMAIL])
        mpr( "You are covered in scales of stone." );

    if (you.duration[DUR_TELEPORT])
        mpr( "You are about to teleport." );

    if (you.duration[DUR_CONTROL_TELEPORT])
        mpr( "You can control teleportation." );

    if (you.duration[DUR_RESIST_POISON])
        mpr( "You are temporarily resistant to poison." );

    if (you.duration[DUR_BREATH_WEAPON])
        mpr( "You are short of breath." );

    if (you.duration[DUR_DEATH_CHANNEL])
        mpr( "You are channeling the dead." );

    if (you.duration[DUR_FORESCRY])
        mpr( "You are forewarned." );

    if (you.duration[DUR_SEE_INVISIBLE])
        mpr( "You can temporarily see invisible." );

    if (you.duration[DUR_WEAPON_BRAND])
        mpr( "Your weapon's brand is temporary." );

    if (you.duration[DUR_SILENCE])
        mpr( "You radiate silence." );

    if (you.duration[DUR_STONESKIN])
        mpr( "Your skin is tough as stone." );

    if (you.duration[DUR_REPEL_UNDEAD])
        mpr( "You have a holy aura protecting you from undead." );

    if (you.duration[DUR_CUT])
        mpr( "You are bleeding." );

    if (you.duration[DUR_STUN])
        mpr( "You are stunned." );

    if (you.conf)
        mpr( "You are confused." );

    if (you.paralysis)
        mpr( "You are paralysed." );

    if (you.invis)
        mpr( "You are invisible." );

    if (you.exhausted)
        mpr( "You are %s.", player_is_exhausted() ?  "exhausted" : "fatigued" );

    if (you.might || you.berserker)
        mpr( "You are mighty." );

    if (you.berserker)
        mpr( "You are possessed by a berserker rage." );

    if (you.poison)
    {
        mpr( "You are %s poisoned.",
                  (you.poison > 10) ? "extremely" :
                  (you.poison > 5)  ? "very" :
                  (you.poison > 3)  ? "quite"
                                    : "mildly" );
    }

    if (you.disease)
    {
        mpr( "You are %sdiseased.",
                  (you.disease > 120) ? "badly " :
                  (you.disease >  40) ? ""
                                      : "mildly " );
    }

    if (you.rotting || you.species == SP_GHOUL)
    {
        // I apologize in advance for the horrendous ugliness about to
        // transpire.  Avert your eyes!
        mpr( "Your flesh is rotting%s",
                  (you.rotting > 15) ? " before your eyes!":
                  (you.rotting > 8)  ? " away quickly.":
                  (you.rotting > 4)  ? " badly."
             : ((you.species == SP_GHOUL && you.rotting > 0)
                        ? " faster than usual." : ".") );
    }

    contaminate_player( 0, true );

    if (you.confusing_touch)
    {
        mpr( "Your hands are glowing %s red.",
                (you.confusing_touch > 20) ? "bright" : "a soft" );
    }

    if (you.sure_blade)
    {
        mpr( "You have a %sbond with your blade.",
                  (you.sure_blade > 15) ? "strong " :
                  (you.sure_blade >  5) ? ""
                                        : "weak " );
    }

    const bool water  = player_in_water();
    const bool swim   = player_is_swimming();

    const bool lev    = player_is_levitating();
    const bool fly    = (lev && player_control_flight());
    const bool swift  = (you.duration[DUR_SWIFTNESS] > 0);

    // This message is pretty meaningless and awkward, so I'm cutting it. -- bwr
    // if (!fly && you.duration[DUR_CONTROLLED_FLIGHT])
    //    mpr( "You can control your flight." );

    const speed_type  speed = player_net_speed_effect();

    int move_cost = (player_movement_speed() * player_base_time_taken()) / 10;
    move_cost = adjust_time_for_speed( move_cost, speed );

    if (water && !swim)
        move_cost = (move_cost * 165) / 100; // based on move-into penalty

    if (speed == SPEED_SLOWED)
        mpr( "Your actions are slowed." );
    else if (speed == SPEED_HASTED)
        mpr( "Your actions are hasted." );
    else if (you.slow || you.exhausted)
    {
        // speed == SPEED_NORMAL, therefore must be fast too
        ASSERT( you.haste || you.berserker );
        mpr( "You are under both slowing and hasting effects." );
    }

    mpr( "Your %s speed is %s%s%s.",
              // order is important for these:
              (swim)    ? "swimming" :
              (water)   ? "wading" :
              (fly)     ? "flying" :
              (lev)     ? "levitating"
                        : "movement",

              (water && !swim)  ? "uncertain and " :
              (!water && swift) ? "aided by the wind" : "",

              (!water && swift) ? ((move_cost >= 10) ? ", but still "
                                                     :  " and ")
                                : "",

              (move_cost <   8) ? "very quick" :
              (move_cost <  10) ? "quick" :
              (move_cost == 10) ? "average" :
              (move_cost <  13) ? "slow"
                                : "very slow" );

    // XXX: should these be here, or under the less crowded 'C' command?

    // unarmed or melee armed characters evaluate their fighting ability:
    if (player_is_bashing())
        mpr( MSGCH_WARN, "You are not currently prepared for melee." );
    else
    {
        // Note: the character doesn't notice inaccuracy effects here,
        // and this doesn't take into account any special cases with
        // monsters or terrain.
        const int wpn = get_inv_wielded();
        const bool half = ((wpn != -1) ? check_hand_and_half_style() : false);
        const int to_hit = your_melee_to_hit( NULL, 0, -1, half, true );

        // Messages based largely on percentage chance of missing the
        // average EV 10 humanoid, and very agile EV 30 (pretty much
        // max EV for monsters currently).
        //
        // "awkward"    - need lucky hit (less than EV)
        // "difficult"  - worse than 2 in 3
        // "hard"       - worse than fair chance
        snprintf( info, INFO_SIZE,
                  "%s in your current equipment.",
                  (to_hit <   1) ? "You are completely incapable of fighting" :
                  (to_hit <   5) ? "Hitting even clumsy monsters is extremely awkward" :
                  (to_hit <  10) ? "Hitting average monsters is awkward" :
                  (to_hit <  15) ? "Hitting average monsters is difficult" :
                  (to_hit <  20) ? "Hitting average monsters is hard" :
                  (to_hit <  30) ? "Very agile monsters are a bit awkward to hit" :
                  (to_hit <  45) ? "Very agile monsters are a bit difficult to hit" :
                  (to_hit <  60) ? "Very agile monsters are a bit hard to hit" :
                  (to_hit < 100) ? "You feel comfortable with your ability to fight"
                                 : "You feel confident with your ability to fight" );

#if DEBUG_DIAGNOSTICS
        char str_pass[INFO_SIZE];
        snprintf( str_pass, INFO_SIZE, " (%d)", to_hit );
        strncat( info, str_pass, INFO_SIZE );
#endif

        mpr( ((to_hit < 10) ? MSGCH_WARN : MSGCH_PLAIN), info );

        if (wpn != -1
            && is_double_ended( you.inv[wpn] )
            && !check_using_both_ends())
        {
            mpr(  MSGCH_WARN, "You cannot use both ends of your weapon." );
        }
    }

    // character evaluates their ability to sneak around:
    const int stealth = player_stealth();

    // XXX: made these values up, probably could be better.
    snprintf( info, INFO_SIZE, "You feel %sstealthy.",
                (stealth <  10) ? "extremely un" :
                (stealth <  20) ? "very un" :
                (stealth <  30) ? "un" :
                (stealth <  50) ? "fairly " :
                (stealth <  80) ? "" :
                (stealth < 120) ? "quite " :
                (stealth < 160) ? "very " :
                (stealth < 200) ? "extremely "
                                : "incredibly " );

#if DEBUG_DIAGNOSTICS
        char str_pass[INFO_SIZE];
        snprintf( str_pass, INFO_SIZE, " (%d)", stealth );
        strncat( info, str_pass, INFO_SIZE );
#endif

    mpr( info );
}                               // end display_char_status()

// Note that this function only has the one static buffer, so if you
// want to use the results, you'll want to make a copy.
char *species_name( int  speci, int level, bool genus, bool adj, bool cap )
// defaults:                        -1          false       false     true
{
    static char species_buff[80];

    if (level == -1)
        level = you.max_xp_level;

    if (player_genus( GENPC_DRACONIAN, speci ))
    {
        if (adj || genus)  // adj doesn't care about exact species
            strcpy( species_buff, "Draconian" );
        else
        {
            // No longer have problems with ghosts here -- Sharp Aug2002
            if (level < 7)
                strcpy( species_buff, "Draconian" );
            else
            {
                switch (speci)
                {
                case SP_RED_DRACONIAN:
                    strcpy( species_buff, "Red Draconian" );
                    break;
                case SP_WHITE_DRACONIAN:
                    strcpy( species_buff, "White Draconian" );
                    break;
                case SP_GREEN_DRACONIAN:
                    strcpy( species_buff, "Green Draconian" );
                    break;
                case SP_YELLOW_DRACONIAN:
                    strcpy( species_buff, "Yellow Draconian" );
                    break;
                case SP_GREY_DRACONIAN:
                    strcpy( species_buff, "Grey Draconian" );
                    break;
                case SP_BLACK_DRACONIAN:
                    strcpy( species_buff, "Black Draconian" );
                    break;
                case SP_PURPLE_DRACONIAN:
                    strcpy( species_buff, "Purple Draconian" );
                    break;
                case SP_MOTTLED_DRACONIAN:
                    strcpy( species_buff, "Mottled Draconian" );
                    break;
                case SP_PALE_DRACONIAN:
                    strcpy( species_buff, "Pale Draconian" );
                    break;
                case SP_UNK0_DRACONIAN:
                case SP_UNK1_DRACONIAN:
                case SP_BASE_DRACONIAN:
                default:
                    strcpy( species_buff, "Draconian" );
                    break;
                }
            }
        }
    }
    else if (player_genus( GENPC_ELVEN, speci ))
    {
        if (adj)  // doesn't care about species/genus
            strcpy( species_buff, "Elven" );
        else if (genus)
            strcpy( species_buff, "Elf" );
        else
        {
            switch (speci)
            {
            case SP_ELF:
            default:
                strcpy( species_buff, "Elf" );
                break;
            case SP_HIGH_ELF:
                strcpy( species_buff, "High Elf" );
                break;
            case SP_GREY_ELF:
                strcpy( species_buff, "Grey Elf" );
                break;
            case SP_DEEP_ELF:
                strcpy( species_buff, "Deep Elf" );
                break;
            case SP_SLUDGE_ELF:
                strcpy( species_buff, "Sludge Elf" );
                break;
            }
        }
    }
    else if (player_genus( GENPC_DWARVEN, speci ))
    {
        if (adj)  // doesn't care about species/genus
            strcpy( species_buff, "Dwarven" );
        else if (genus)
            strcpy( species_buff, "Dwarf" );
        else
        {
            switch (speci)
            {
            case SP_HILL_DWARF:
                strcpy( species_buff, "Hill Dwarf" );
                break;
            case SP_MOUNTAIN_DWARF:
                strcpy( species_buff, "Mountain Dwarf" );
                break;
            default:
                strcpy( species_buff, "Dwarf" );
                break;
            }
        }
    }
    else
    {
        switch (speci)
        {
        case SP_HUMAN:
            strcpy( species_buff, "Human" );
            break;
        case SP_HALFLING:
            strcpy( species_buff, "Halfling" );
            break;
        case SP_HILL_ORC:
            strcpy( species_buff, (adj) ? "Orcish" : (genus) ? "Orc"
                                                             : "Hill Orc" );
            break;
        case SP_KOBOLD:
            strcpy( species_buff, "Kobold" );
            break;
        case SP_MUMMY:
            strcpy( species_buff, "Mummy" );
            break;
        case SP_NAGA:
            strcpy( species_buff, "Naga" );
            break;
        case SP_GNOME:
            strcpy( species_buff, (adj) ? "Gnomish" : "Gnome" );
            break;
        case SP_OGRE:
            strcpy( species_buff, (adj) ? "Ogreish" : "Ogre" );
            break;
        case SP_TROLL:
            strcpy( species_buff, (adj) ? "Trollish" : "Troll" );
            break;
        case SP_OGRE_MAGE:
            // We've previously declared that these are radically
            // different from Ogres... so we're not going to
            // refer to them as Ogres.  -- bwr
            strcpy( species_buff, "Ogre-Mage" );
            break;
        case SP_CENTAUR:
            strcpy( species_buff, "Centaur" );
            break;
        case SP_DEMIGOD:
            strcpy( species_buff, (adj) ? "Divine" : "Demigod" );
            break;
        case SP_SPRIGGAN:
            strcpy( species_buff, "Spriggan" );
            break;
        case SP_MINOTAUR:
            strcpy( species_buff, "Minotaur" );
            break;
        case SP_DEMONSPAWN:
            strcpy( species_buff, (adj) ? "Demonic" : "Demonspawn" );
            break;
        case SP_GHOUL:
            strcpy( species_buff, (adj) ? "Ghoulish" : "Ghoul" );
            break;
        case SP_KENKU:
            strcpy( species_buff, "Kenku" );
            break;
        case SP_MERFOLK:
            strcpy( species_buff, (adj) ? "Merfolkian" : "Merfolk" );
            break;
        case SP_GIANT:
            strcpy( species_buff, "Giant" );
            break;
        default:
            strcpy( species_buff, (adj) ? "Yakish" : "Yak" );
            break;
        }
    }

    if (!cap)
        strlwr( species_buff );

    return (species_buff);
}                               // end species_name()

bool player_res_corrosion( bool req_id )
{
    return (player_equip( EQ_AMULET, AMU_RESIST_CORROSION, req_id )
            || player_equip_ego_type( EQ_CLOAK, SPARM_PRESERVATION, req_id ));
}

bool player_item_conserve( bool req_id )
{
    return (player_equip( EQ_AMULET, AMU_CONSERVATION, req_id )
            || player_equip_ego_type( EQ_CLOAK, SPARM_PRESERVATION, req_id ));
}

int player_mental_clarity( bool req_id )
{
    const int ret = 3 * player_equip( EQ_AMULET, AMU_CLARITY, req_id )
                        + you.mutation[ MUT_CLARITY ];

    return ((ret > 3) ? 3 : ret);
}

bool player_is_levitating(void)
{
    return (you.attribute[ATTR_TRANSFORMATION] == TRAN_DRAGON || you.levitation);
}

bool player_control_flight( bool req_id )
{
    return (you.duration[DUR_CONTROLLED_FLIGHT]
            || you.attribute[ATTR_TRANSFORMATION] == TRAN_DRAGON
            || player_genus(GENPC_DRACONIAN)
            || player_equip( EQ_AMULET, AMU_CONTROLLED_FLIGHT, req_id ));
}

bool player_has_warding( bool req_id )
{
    return (player_equip( EQ_AMULET, AMU_WARDING, req_id )
             || (you.religion == GOD_VEHUMET
                 && you.duration[DUR_PRAYER]
                 && !player_under_penance()
                 && you.piety >= 75));
}

bool player_access_floor(void)
{
    return (!player_is_levitating() || player_control_flight());
}

bool player_has_spell( int spell )
{
    for (int i = 0; i < 25; i++)
    {
        if (you.spells[i] == spell)
            return (true);
    }

    return (false);
}

int species_exp_mod(char species)
{
    int ret = 10;

    if (player_genus(GENPC_DRACONIAN))
        ret = 14;
    else if (player_genus(GENPC_DWARVEN))
        ret = 13;
    else
    {
        switch (species)
        {
        default:
        case SP_HUMAN:
        case SP_HALFLING:
        case SP_HILL_ORC:
        case SP_KOBOLD:
            ret = 10;
            break;
        case SP_GNOME:
            ret = 11;
            break;
        case SP_ELF:
        case SP_SLUDGE_ELF:
        case SP_NAGA:
        case SP_MERFOLK:
            ret = 12;
            break;
        case SP_SPRIGGAN:
        case SP_KENKU:
            ret = 13;
            break;
        case SP_GREY_ELF:
        case SP_DEEP_ELF:
        case SP_OGRE:
        case SP_CENTAUR:
        case SP_MINOTAUR:
        case SP_DEMONSPAWN:
            ret = 14;
            break;
        case SP_HIGH_ELF:
        case SP_GHOUL:
        case SP_MUMMY:
        case SP_TROLL:
        case SP_OGRE_MAGE:
        case SP_GIANT:
            ret = 15;
            break;
        case SP_DEMIGOD:
            ret = 16;
            break;
        }
    }

    return (ret);
}                               // end species_exp_mod()

unsigned long exp_needed(int lev)
{
    lev--;

    unsigned long level = 0;

#if 0
    case  1: level = 1;
    case  2: level = 10;
    case  3: level = 35;
    case  4: level = 70;
    case  5: level = 120;
    case  6: level = 250;
    case  7: level = 510;
    case  8: level = 900;
    case  9: level = 1700;
    case 10: level = 3500;
    case 11: level = 8000;
    case 12: level = 20000;

    default:                    //return 14000 * (lev - 11);
        level = 20000 * (lev - 11) + ((lev - 11) * (lev - 11) * (lev - 11)) * 130;
        break;
#endif

    // This is a better behaved function than the above.  The above looks
    // really ugly when you consider the second derivative, its not smooth
    // and has a horrible bump at level 12 followed by comparitively easy
    // teen levels.  This tries to sort out those issues.
    //
    // Basic plan:
    // Section 1: levels  1- 5, second derivative goes 10-10-20-30.
    // Section 2: levels  6-13, second derivative is exponential/doubling.
    // Section 3: levels 14-27, second derivative is constant at 6000.
    //
    // Section three is constant so we end up with high levels at about
    // their old values (level 27 at 850k), without delta2 ever decreasing.
    // The values that are considerably different (ie level 13 is now 29000,
    // down from 41040 are because the second derivative goes from 9040 to
    // 1430 at that point in the original, and then slowly builds back
    // up again).  This function smoothes out the old level 10-15 area
    // considerably.

    // Here's a table:
    //
    // level      xp      delta   delta2
    // =====   =======    =====   ======
    //   1           0        0       0
    //   2          10       10      10
    //   3          30       20      10
    //   4          70       40      20
    //   5         140       70      30
    //   6         270      130      60
    //   7         520      250     120
    //   8        1010      490     240
    //   9        1980      970     480
    //  10        3910     1930     960
    //  11        7760     3850    1920
    //  12       15450     7690    3840
    //  13       29000    13550    5860
    //  14       48500    19500    5950
    //  15       74000    25500    6000
    //  16      105500    31500    6000
    //  17      143000    37500    6000
    //  18      186500    43500    6000
    //  19      236000    49500    6000
    //  20      291500    55500    6000
    //  21      353000    61500    6000
    //  22      420500    67500    6000
    //  23      494000    73500    6000
    //  24      573500    79500    6000
    //  25      659000    85500    6000
    //  26      750500    91500    6000
    //  27      848000    97500    6000


    switch (lev)
    {
    case 1:
        level = 1;
        break;
    case 2:
        level = 10;
        break;
    case 3:
        level = 30;
        break;
    case 4:
        level = 70;
        break;

    default:
        if (lev < 13)
        {
            lev -= 4;
            level = 10 + 10 * lev
                       + 30 * (static_cast<int>(pow( 2.0, lev + 1 )));
        }
        else
        {
            lev -= 12;
            level = 15500 + 10500 * lev + 3000 * lev * lev;
        }
        break;
    }

    return ((level - 1) * species_exp_mod(you.species) / 10);
}                               // end exp_needed()

// returns bonuses from rings of slaying, etc.
int slaying_bonus( int which_affected, bool req_id )
{
    int ret = 0;

    if (which_affected == PWPN_HIT)
    {
        ret += player_equip( EQ_RINGS_PLUS, RING_SLAYING, req_id );
        ret += scan_randarts( RAP_ACCURACY, req_id );
    }
    else if (which_affected == PWPN_DAMAGE)
    {
        ret += player_equip( EQ_RINGS_PLUS2, RING_SLAYING, req_id );
        ret += scan_randarts( RAP_DAMAGE, req_id );
    }

    return (ret);
}                               // end slaying_bonus()

/* Checks each equip slot for a randart, and adds up all of those with
   a given property. Slow if any randarts are worn, so avoid where possible. */
int scan_randarts( randart_prop_type which_property, bool req_id )
{
    int i = 0;
    int retval = 0;

    ASSERT( which_property != RAP_BRAND );  // doesn't make sense to sum

    // special handling for weapons which must be usable as a tool to work
    const int wpn = get_inv_hand_tool();

    if (wpn != -1
        && is_random_artefact( you.inv[wpn] )
        && (!req_id
            || item_ident( you.inv[wpn], ISFLAG_KNOW_PROPERTIES )))
    {
        retval += you.inv[wpn].ra_props[which_property];
    }

    // armour and jewellery
    for (i = EQ_CLOAK; i < NUM_EQUIP; i++)
    {
        const int eq = you.equip[i];

        if (eq != -1
            && is_random_artefact( you.inv[eq] )
            && (!req_id
                || item_ident( you.inv[wpn], ISFLAG_KNOW_PROPERTIES )))
        {
            retval += you.inv[eq].ra_props[which_property];
        }
    }

    return (retval);
}                               // end scan_randarts()

void dec_hp(int hp_loss, bool fatal)
{
    if (hp_loss < 1)
        return;

    you.hp -= hp_loss;

    if (!fatal && you.hp < 1)
        you.hp = 1;

    set_redraw_status( REDRAW_HIT_POINTS );

    return;
}                               // end dec_hp()

void dec_mp(int mp_loss)
{
    if (mp_loss < 1)
        return;

    you.magic_points -= mp_loss;

    if (you.magic_points < 0)
        you.magic_points = 0;

    set_redraw_status( REDRAW_MAGIC_POINTS );

    return;
}                               // end dec_mp()

bool enough_hp(int minimum, bool suppress_msg)
{
    // We want to at least keep 1 HP -- bwr
    if (you.hp < minimum + 1)
    {
        if (!suppress_msg)
            mpr("You haven't enough vitality at the moment.");

        return false;
    }

    return true;
}                               // end enough_hp()

bool enough_mp(int minimum, bool suppress_msg)
{
    if (you.magic_points < minimum)
    {
        if (!suppress_msg)
            mpr("You haven't enough magic at the moment.");

        return false;
    }

    return true;
}                               // end enough_mp()

// Note that "max_too" refers to the base potential, the actual
// resulting max value is subject to penalties, bonuses, and scalings.
void inc_mp(int mp_gain, bool max_too)
{
    if (mp_gain < 1)
        return;

    you.magic_points += mp_gain;

    if (max_too)
        inc_max_mp( mp_gain );

    if (you.magic_points > you.max_magic_points)
        you.magic_points = you.max_magic_points;

    set_redraw_status( REDRAW_MAGIC_POINTS );

    return;
}                               // end inc_mp()

// Note that "max_too" refers to the base potential, the actual
// resulting max value is subject to penalties, bonuses, and scalings.
void inc_hp(int hp_gain, bool max_too)
{
    if (hp_gain < 1)
        return;

    you.hp += hp_gain;

    if (max_too)
        inc_max_hp( hp_gain );

    if (you.hp > you.hp_max)
        you.hp = you.hp_max;

    set_redraw_status( REDRAW_HIT_POINTS );
}                               // end inc_hp()

void rot_hp( int hp_loss )
{
    you.base_hp -= hp_loss;
    calc_hp_max();
}

void unrot_hp( int hp_recovered )
{
    if (hp_recovered >= 5000 - you.base_hp)
        you.base_hp = 5000;
    else
        you.base_hp += hp_recovered;

    calc_hp_max();
}

int player_rotted( void )
{
    return (5000 - you.base_hp);
}

void rot_mp( int mp_loss )
{
    you.base_magic_points -= mp_loss;
    calc_mp_max();
}

void inc_max_hp( int hp_gain )
{
    you.base_hp2 += hp_gain;
    calc_hp_max();
}

void dec_max_hp( int hp_loss )
{
    you.base_hp2 -= hp_loss;
    calc_hp_max();
}

void inc_max_mp( int mp_gain )
{
    you.base_magic_points2 += mp_gain;
    calc_mp_max();
}

void dec_max_mp( int mp_loss )
{
    you.base_magic_points2 -= mp_loss;
    calc_mp_max();
}


// Note that "max_too" refers to the base potential, the actual
// resulting max value is subject to penalties, bonuses, and scalings.
void set_hp(int new_amount, bool max_too)
{
    if (you.hp != new_amount)
        you.hp = new_amount;

    if (max_too && you.hp_max != new_amount)
    {
        you.base_hp2 = 5000 + new_amount;
        calc_hp_max();
    }

    if (you.hp > you.hp_max)
        you.hp = you.hp_max;

    // must remain outside conditional, given code usage {dlb}
    set_redraw_status( REDRAW_HIT_POINTS );

    return;
}                               // end set_hp()

// Note that "max_too" refers to the base potential, the actual
// resulting max value is subject to penalties, bonuses, and scalings.
void set_mp(int new_amount, bool max_too)
{
    if (you.magic_points != new_amount)
        you.magic_points = new_amount;

    if (max_too && you.max_magic_points != new_amount)
    {
        // note that this gets scaled down for values > 18
        you.base_magic_points2 = 5000 + new_amount;
    }

    if (you.magic_points > you.max_magic_points)
        you.magic_points = you.max_magic_points;

    // must remain outside conditional, given code usage {dlb}
    set_redraw_status( REDRAW_MAGIC_POINTS );

    return;
}                               // end set_mp()


static const char * Species_Abbrev_List[ NUM_SPECIES ] =
    { "XX", "Hu", "El", "HE", "GE", "DE", "SE", "HD", "MD", "Ha",
      "HO", "Ko", "Mu", "Na", "Gn", "Og", "Tr", "OM", "Dr", "Dr",
      "Dr", "Dr", "Dr", "Dr", "Dr", "Dr", "Dr", "Dr", "Dr", "Dr",
      "Ce", "DG", "Sp", "Mi", "DS", "Gh", "Ke", "Mf", "Gt" };

int get_species_index_by_abbrev( const char *abbrev )
{
    int i;
    for (i = SP_HUMAN; i < NUM_SPECIES; i++)
    {
        if (tolower( abbrev[0] ) == tolower( Species_Abbrev_List[i][0] )
            && tolower( abbrev[1] ) == tolower( Species_Abbrev_List[i][1] ))
        {
            break;
        }
    }

    return ((i < NUM_SPECIES) ? i : -1);
}

int get_species_index_by_name( const char *name )
{
    int i;
    int sp = -1;

    char *ptr;
    char lowered_buff[80];

    strncpy( lowered_buff, name, sizeof( lowered_buff ) );
    strlwr( lowered_buff );

    for (i = SP_HUMAN; i < NUM_SPECIES; i++)
    {
        char *lowered_species = species_name( i, 0, false, false, false );
        ptr = strstr( lowered_species, lowered_buff );
        if (ptr != NULL)
        {
            sp = i;
            if (ptr == lowered_species)  // prefix takes preference
                break;
        }
    }

    return (sp);
}

const char *get_species_abbrev( int which_species )
{
    ASSERT( which_species > 0 && which_species < NUM_SPECIES );

    return (Species_Abbrev_List[ which_species ]);
}


static const char * Class_Abbrev_List[ NUM_JOBS ] =
    { "Fi", "Wz", "Pr", "Th", "Gl", "Ne", "Pa", "As", "Be", "Hu",
      "Cj", "En", "FE", "IE", "Su", "AE", "EE", "Cr", "DK", "VM",
      "CK", "Tm", "He", "XX", "Re", "St", "Mo", "Wr", "Wn" };

static const char * Class_Name_List[ NUM_JOBS ] =
    { "Fighter", "Wizard", "Priest", "Thief", "Gladiator", "Necromancer",
      "Paladin", "Assassin", "Berserker", "Hunter", "Conjurer", "Enchanter",
      "Fire Elementalist", "Ice Elementalist", "Summoner", "Air Elementalist",
      "Earth Elementalist", "Crusader", "Death Knight", "Venom Mage",
      "Chaos Knight", "Transmuter", "Healer", "Quitter", "Reaver", "Stalker",
      "Monk", "Warper", "Wanderer" };

int get_class_index_by_abbrev( const char *abbrev )
{
    int i;

    for (i = 0; i < NUM_JOBS; i++)
    {
        if (i == JOB_QUITTER)
            continue;

        if (tolower( abbrev[0] ) == tolower( Class_Abbrev_List[i][0] )
            && tolower( abbrev[1] ) == tolower( Class_Abbrev_List[i][1] ))
        {
            break;
        }
    }

    return ((i < NUM_JOBS) ? i : -1);
}

const char *get_class_abbrev( int which_job )
{
    ASSERT( which_job < NUM_JOBS && which_job != JOB_QUITTER );

    return (Class_Abbrev_List[ which_job ]);
}

int get_class_index_by_name( const char *name )
{
    int i;
    int cl = -1;

    char *ptr;
    char lowered_buff[80];
    char lowered_class[80];

    strncpy( lowered_buff, name, sizeof( lowered_buff ) );
    strlwr( lowered_buff );

    for (i = 0; i < NUM_JOBS; i++)
    {
        if (i == JOB_QUITTER)
            continue;

        strncpy( lowered_class, Class_Name_List[i], sizeof( lowered_class ) );
        strlwr( lowered_class );

        ptr = strstr( lowered_class, lowered_buff );
        if (ptr != NULL)
        {
            cl = i;
            if (ptr == lowered_class)  // prefix takes preference
                break;
        }
    }

    return (cl);
}

const char *get_class_name( int which_job )
{
    ASSERT( which_job < NUM_JOBS && which_job != JOB_QUITTER );

    return (Class_Name_List[ which_job ]);
}

/* ******************************************************************

// this function is solely called by a commented out portion of
// player::level_change() and is a bit outta whack with the
// current codebase - probably should be struck as well 19may2000 {dlb}

void priest_spells( int priest_pass[10], char religious )
{

    switch ( religious )
    {
      case GOD_ZIN:
        priest_pass[1] = SPELL_LESSER_HEALING;
        priest_pass[2] = SPELL_REPEL_UNDEAD;
        priest_pass[3] = SPELL_HEAL_OTHER;
        priest_pass[4] = SPELL_PURIFICATION;
        priest_pass[5] = SPELL_GREATER_HEALING;
        priest_pass[6] = SPELL_SMITING;
        priest_pass[7] = SPELL_HOLY_WORD;
        priest_pass[8] = SPELL_REMOVE_CURSE;
        priest_pass[9] = SPELL_GUARDIAN;
        break;

     case GOD_SHINING_ONE:
       priest_pass[1] = SPELL_REPEL_UNDEAD;
       priest_pass[2] = SPELL_LESSER_HEALING;
       priest_pass[3] = SPELL_HEAL_OTHER;
       priest_pass[4] = SPELL_PURIFICATION;
       priest_pass[5] = SPELL_ABJURATION_II;
       priest_pass[6] = SPELL_THUNDERBOLT;
       priest_pass[7] = SPELL_SHINING_LIGHT;
       priest_pass[8] = SPELL_SUMMON_DAEVA;
       priest_pass[9] = SPELL_FLAME_OF_CLEANSING;
       break;

     case GOD_ELYVILON:
       priest_pass[1] = SPELL_LESSER_HEALING;
       priest_pass[2] = SPELL_HEAL_OTHER;
       priest_pass[3] = SPELL_PURIFICATION;
       priest_pass[4] = 93; // restore abilities
       priest_pass[5] = SPELL_GREATER_HEALING;
       priest_pass[6] = 94; // another healing spell
       priest_pass[7] = 95; // something else
       priest_pass[8] = -1; //
       priest_pass[9] = -1; //
       break;
    }

}

// Spells to be added: (+ renamed!)
//   holy berserker
//   87 Pestilence
//   93 Restore Abilities
//   94 something else healing
//   95 something else

****************************************************************** */

void contaminate_player( int change, bool statusOnly )
{
    // get current contamination level
    int old_level;
    int new_level;


    old_level = (you.magic_contamination >= 60) ? (you.magic_contamination / 20 + 2) :
                (you.magic_contamination >= 40) ? 4 :
                (you.magic_contamination >= 25) ? 3 :
                (you.magic_contamination >= 15) ? 2 :
                (you.magic_contamination >= 5)  ? 1 : 0;

    // make the change
    if (change + you.magic_contamination < 0)
        you.magic_contamination = 0;
    else
    {
        if (change + you.magic_contamination > 250)
            you.magic_contamination = 250;
        else
            you.magic_contamination += change;
    }

#if DEBUG_DIAGNOSTICS
    if (change > 0 || (change < 0 && you.magic_contamination))
    {
        mpr( MSGCH_DIAGNOSTICS, "contamination: change=%d; radiation=%d",
              change, you.magic_contamination );
    }
#endif

    // figure out new level
    new_level = (you.magic_contamination >= 60) ? (you.magic_contamination / 20 + 2) :
                (you.magic_contamination >= 40) ? 4 :
                (you.magic_contamination >= 25) ? 3 :
                (you.magic_contamination >= 15) ? 2 :
                (you.magic_contamination >=  5) ? 1 : 0;

    if (statusOnly)
    {
        if (new_level > 0)
        {
            if (new_level > 3)
            {
                strcpy(info, (new_level == 4) ?
                    "Your entire body has taken on an eerie glow!" :
                    "You are engulfed in a nimbus of crackling magics!");
            }
            else
            {
                snprintf( info, INFO_SIZE, "You are %s with residual magics%c",
                    (new_level == 3) ? "practically glowing" :
                    (new_level == 2) ? "heavily infused"
                                     : "contaminated",
                    (new_level == 3) ? '!' : '.');
            }

            mpr(info);
        }
        return;
    }

    if (new_level == old_level)
        return;

    mpr( ((change > 0) ? MSGCH_WARN : MSGCH_RECOVERY),
            "You feel %s contaminated with magical energies.",
             (change < 0) ? "less" : "more" );
}

void poison_player( int amount, bool force )
{
    if ((!force && player_res_poison() >= 3) || amount <= 0)
        return;

    const int old_value = you.poison;
    you.poison += amount;

    if (you.poison > 40)
        you.poison = 40;

    // XXX: which message channel for this message?
    if (you.poison > old_value)
        mpr( "You are %spoisoned.", (old_value > 0) ? "more " : "" );
}

void reduce_poison_player( int amount )
{
    if (you.poison == 0 || amount <= 0)
        return;

    you.poison -= amount;

    if (you.poison <= 0)
    {
        you.poison = 0;
        mpr( MSGCH_RECOVERY, "You feel better." );
    }
    else
    {
        mpr( MSGCH_RECOVERY, "You feel a little better." );
    }
}

void paralyse_player( int amount, bool quiet )
{
    ASSERT( amount > 0 );

    // Note: this can happen from balls of fixation:
    if (you.paralysis > 12)
        return;

    if (!quiet)
    {
        if (you.paralysis > 0)
            mpr( MSGCH_WARN, "You feel your muscles getting stiffer!" );
        else
            mpr( MSGCH_WARN, "You suddenly lose the ability to move!" );
    }

    you.paralysis += amount;

    if (you.paralysis > 12)
        you.paralysis = 12;
}

void confuse_player( int amount, bool resistable )
{
    if (amount <= 0)
        return;

    if (resistable && player_mental_clarity() > random2(3))
    {
        mpr( "You feel momentarily confused." );
        return;
    }

    const int old_value = you.conf;
    you.conf += amount;

    if (you.conf > 40)
        you.conf = 40;

    // XXX: which message channel for this message?
    if (you.conf > old_value)
        mpr( "You are %sconfused.", (old_value > 0) ? "more " : "" );
}

void reduce_confuse_player( int amount )
{
    if (you.conf == 0 || amount <= 0)
        return;

    you.conf -= amount;

    if (you.conf <= 0)
    {
        you.conf = 0;
        mpr( "You feel less confused." );
    }
}

void stun_player( int amount )
{
    if (amount <= 0)
        return;

    if (player_mental_clarity() > random2(3))
    {
        mpr( "You feel momentarily dizzy." );
        return;
    }

    if (you.duration[DUR_STUN] < 20)
    {
        mpr( "You are %sstunned.", (you.duration[DUR_STUN]) ? "more " : "" );
        you.duration[DUR_STUN] += amount;
    }
    else if (you.conf < 20)
    {
        confuse_player( amount );
    }
    else
    {
        mpr( MSGCH_WARN, "You lose consciousness!" );
        paralyse_player( roll_dice(1,3), true );
    }
}

void cut_player( int amount )
{
    if (amount <= 0)
        return;

    if (you.is_undead)
        rot_hp( roll_dice(1,5) );
    else if (you.duration[DUR_CUT] < 40)
    {
        if (!you.duration[DUR_CUT])
            mpr( "You're bleeding!" );
        else
            mpr( "Your wounds get worse!" );

        you.duration[DUR_CUT] += amount;
    }
}

void slow_player( int amount )
{
    if (amount <= 0)
        return;

    if (player_equip( EQ_AMULET, AMU_RESIST_SLOW ))
        mpr("You feel momentarily lethargic.");
    else if (you.slow >= 100)
        mpr( "You already are as slow as you could be." );
    else
    {
        if (you.slow == 0)
            mpr( "You feel yourself slow down." );
        else
            mpr( "You feel as though you will be slow longer." );

        you.slow += amount;

        if (you.slow > 100)
            you.slow = 100;
    }
}

void dec_slow_player( void )
{
    if (you.slow > 1)
    {
        you.slow--;

        // BCR - Amulet of resist slow affects slow counter
        if (player_equip( EQ_AMULET, AMU_RESIST_SLOW ))
        {
            you.slow -= 2;
            if (you.slow < 1)
                you.slow = 1;
        }
    }
    else if (you.slow == 1)
    {
        you.slow = 0;
        mpr(MSGCH_DURATION,"You feel yourself speed up." );
    }
}

void haste_player( int amount )
{
    const bool amu_eff = player_equip( EQ_AMULET, AMU_RESIST_SLOW );

    if (amount <= 0)
        return;

    if (amu_eff)
        mpr( "Your amulet glows brightly." );

    if (you.haste == 0)
        mpr( "You feel yourself speed up." );
    else if (you.haste > 80 + 20 * amu_eff)
        mpr( "You already have as much speed as you can handle." );
    else
    {
        mpr( "You feel as though your hastened speed will last longer." );
        contaminate_player(1);
    }

    you.haste += amount;

    if (you.haste > 80 + 20 * amu_eff)
        you.haste = 80 + 20 * amu_eff;

    did_god_conduct( DID_STIMULANTS, 4 + random2(4) );
}

void dec_haste_player( void )
{
    if (you.haste > 1)
    {
        // BCR - Amulet of resist slow affects haste counter
        if (!player_equip( EQ_AMULET, AMU_RESIST_SLOW ) || coinflip())
            you.haste--;

        if (you.haste == 6)
        {
            mpr( MSGCH_DURATION, "Your extra speed is starting to run out." );
            if (coinflip())
                you.haste--;
        }
    }
    else if (you.haste == 1)
    {
        you.haste = 0;

        if (!you.berserker)
            mpr( MSGCH_DURATION, "You feel yourself slow down." );
    }
}

void disease_player( int amount )
{
    if (you.is_undead || amount <= 0)
        return;

    mpr( "You feel ill." );

    const int tmp = you.disease + amount;
    you.disease = (tmp > 210) ? 210 : tmp;
}

void dec_disease_player( void )
{
    if (you.disease > 0)
    {
        you.disease--;

        if (you.disease > 5
            && (you.species == SP_KOBOLD
                || you.duration[ DUR_REGENERATION ]
                || you.mutation[ MUT_REGENERATION ] == 3))
        {
            you.disease -= 2;
        }

        if (!you.disease)
            mpr( MSGCH_RECOVERY,"You feel your health improve." );
    }
}

void rot_player( int amount )
{
    if (amount <= 0)
        return;

    if (you.rotting < 40)
    {
        // Either this, or the actual rotting message should probably
        // be changed so that they're easier to tell apart. -- bwr
        mpr( MSGCH_WARN, "You feel your flesh %s away!",
              (you.rotting) ? "rotting" : "start to rot" );

        you.rotting += amount;

        if (you.rotting > 50)
            you.rotting = 50;
    }
}

// XXX: Need to incorperate fatigue/exhaustion a bit more... for example,
// it could be good as an alternate balance factor for powerul spells.
// It could also be used to affect combat readiness, spellcasting, and
// other abilities as appropriate.
void fatigue_player( int amount )
{
    if (amount <= 0)
        return;

    const int old = you.exhausted;

    if (you.exhausted < 100)
    {
        you.exhausted += amount;

        mpr( "You feel %s%s.",
                (old) ? "more " : "",
                (player_is_exhausted()) ?  "exhausted" : "fatigued" );
    }
}

bool player_is_fatigued( void )
{
    return (you.exhausted > 0);
}

bool player_is_exhausted( void )
{
    return (you.exhausted >= EXHAUSTION_THRESHOLD);
}

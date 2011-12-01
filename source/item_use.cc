/*
 *  File:       item_use.cc
 *  Summary:    Functions for making use of inventory items.
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 *   <8>     28July2000  GDL    Revised player throwing
 *   <7>     11/23/99    LRH    Horned characters can wear hats/caps
 *   <6>     7/13/99     BWR    Lowered learning rates for
 *                              throwing skills, and other
 *                              balance tweaks
 *   <5>     5/28/99     JDJ    Changed wear_armour to allow Spriggans to
 *                              wear bucklers.
 *   <4>     5/26/99     JDJ    body armour can be removed and worn if an
 *                              uncursed cloak is being worn.
 *                              Removed lots of unnessary mpr string copying.
 *                              Added missing ponderous message.
 *   <3>     5/20/99     BWR    Fixed staff of air bug, output of trial
 *                              identified items, a few you.wield_changes so
 *                              that the weapon gets updated.
 *   <2>     5/08/99     JDJ    Added armour_prompt.
 *   <1>     -/--/--     LRH    Created
 */

#include "AppHdr.h"
#include "item_use.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "globals.h"
#include "externs.h"

#include "beam.h"
#include "debug.h"
#include "delay.h"
#include "describe.h"
#include "direct.h"
#include "effects.h"
#include "fight.h"
#include "food.h"
#include "invent.h"
#include "it_use2.h"
#include "it_use3.h"
#include "items.h"
#include "itemname.h"
#include "itemprop.h"
#include "misc.h"
#include "monplace.h"
#include "monstuff.h"
#include "mstuff2.h"
#include "mon-util.h"
#include "ouch.h"
#include "player.h"
#include "randart.h"
#include "religion.h"
#include "skills.h"
#include "skills2.h"
#include "spells1.h"
#include "spells2.h"
#include "spells3.h"
#include "spl-book.h"
#include "spl-cast.h"
#include "stuff.h"
#include "transfor.h"
#include "view.h"

static bool drink_fountain( void );
static void throw_it( struct bolt &pbolt, int ammo_id );
static bool enchant_armour( void );

// Rather messy - we've gathered all the can't-wield logic from wield_weapon()
// here. Inventory slot checks (are we already wielding this, are we wearing
// this object) are *not* performed.
bool can_wield( const item_def &weapon )
{
    if (you.berserker)
        return (false);

    if (you.attribute[ATTR_TRANSFORMATION] != TRAN_NONE
        && !transform_can_equip_type( EQ_WEAPON ))
    {
        return (false);
    }

    const int old_tool = get_inv_hand_tool();
    if (old_tool != -1 && item_cursed( you.inv[old_tool] ))
        return (false);

    const size_type size = player_size();
    if ((weapon.base_type == OBJ_WEAPONS || weapon.base_type == OBJ_STAVES)
        && hands_reqd( weapon, size ) == HANDS_TWO
        && you.equip[EQ_SHIELD] != -1)
    {
        return (false);
    }

    if (!check_weapon_shape( weapon, true, true ))
        return (false);

    // We can wield this weapon. Phew!
    return (true);
}

void wield_weapon( bool auto_wield, int slot )
{
    int item_slot = 0;
    char str_pass[ ITEMNAME_SIZE ];

    if (inv_count() < 1)
    {
        canned_msg(MSG_NOTHING_CARRIED);
        return;
    }

    if (you.berserker)
    {
        canned_msg(MSG_TOO_BERSERK);
        return;
    }

    // Redundant with check_weapon_shape() but avoids going through prompt
    // here, and is needed there for untransform checks.
    if (you.attribute[ATTR_TRANSFORMATION] != TRAN_NONE)
    {
        if (!transform_can_equip_type( EQ_WEAPON ))
        {
            mpr("You can't wield anything in your present form.");
            return;
        }
    }

    // tools can be sticky cursed to hand (wielded assumed to be tool)
    const int old_tool = get_inv_hand_tool();

    if (old_tool != -1 && item_cursed( you.inv[old_tool] ))
    {
        mpr("You can't unwield your weapon to draw a new one!");
        return;
    }

    if (you.sure_blade)
    {
        mpr("The bond with your blade fades away.");
        you.sure_blade = 0;
    }

    if (auto_wield)
    {
        if (slot != -1)
            item_slot = slot;
        else if (you.equip[EQ_WEAPON] == 0)  // ie. weapon is currently 'a'
            item_slot = 1;
        else
            item_slot = 0;
    }

    const bool force_unwield = (you.inv[item_slot].base_type != OBJ_WEAPONS
                               && you.inv[item_slot].base_type != OBJ_MISSILES
                               && you.inv[item_slot].base_type != OBJ_STAVES);

    // Prompt if not using the auto swap command,
    // or if the swap slot is empty.
    if (!auto_wield || !is_valid_item(you.inv[item_slot]) || force_unwield)
    {
        if (!auto_wield)
        {
            item_slot = prompt_invent_item( "Wield which item (- for none)?",
                                            OBJ_WEAPONS, true, true, true, '-' );
        }
        else
            item_slot = PROMPT_GOT_SPECIAL;

        if (item_slot == PROMPT_ABORT)
        {
            canned_msg( MSG_OK );
            return;
        }
        else if (item_slot == PROMPT_GOT_SPECIAL)  // '-' or bare hands
        {
            if (you.equip[EQ_WEAPON] == -1)
                mpr( "You are already empty-handed." );
            else
            {
                you.turn_is_over = true;
                you.time_taken *= 3;
                you.time_taken /= 10;

                unwield_item( true );
                mpr( "You are now empty handed." );
            }
            return;
        }
    }

    if (item_slot == you.equip[EQ_WEAPON])
    {
        mpr( "You are already %s that!",
              player_weapon_wielded() ? "wielding" : "holding" );
        return;
    }

    for (int i = EQ_CLOAK; i <= EQ_AMULET; i++)
    {
        if (item_slot == you.equip[i])
        {
            mpr("You are wearing that object!");
            return;
        }
    }

    if (you.inv[item_slot].base_type == OBJ_WEAPONS
        || you.inv[item_slot].base_type == OBJ_STAVES)
    {
        // All weapons are now placable "in-hand"... after all,
        // we're currently allowing the PC to lug around corpses.
        // The question nowdays: Is the weapon just "in-hand" or
        // is it actually useful as a weapon?
        //
        // if (!check_weapon_size( you.inv[item_slot], player_size() ))
        // {
        //     mpr( "That weapon is too large for you to wield." );
        //     return;
        // }
        const size_type size = player_size();

        if (hands_reqd( you.inv[item_slot], size ) == HANDS_TWO
            && you.equip[EQ_SHIELD] != -1)
        {
            mpr( "You need to use both hands to %s that.",
                  (!fit_weapon_wieldable_size( you.inv[item_slot], size ))
                      ? "wield" : "lift" );
            return;
        }

        if (!check_weapon_shape( you.inv[item_slot], false ))
            return;

        if (you.equip[EQ_WEAPON] != -1)
        {
            you.turn_is_over = true;   // doing work by unwielding item
            unwield_item( true );
        }
    }

    // finally put the item in hand
    you.equip[EQ_WEAPON] = item_slot;

    // any oddness on wielding taken care of here
    wield_effects( true );

    in_name( item_slot, DESC_INVENTORY_EQUIP, str_pass );
    mpr( str_pass );

    // warn player if weapon unfit for melee
    wield_warning( true );

    // time calculations
    you.time_taken *= 5;
    you.time_taken /= 10;

    set_redraw_status( REDRAW_WIELD );
    you.turn_is_over = true;
}

// provide a function for handling initial wielding of 'special'
// weapons,  or those whose function is annoying to reproduce in
// other places *cough* auto-butchering *cough*    {gdl}
void wield_effects( bool showMsgs )
{
    const int tool = get_inv_hand_tool();

    // first misc tool effects:
    if (tool != -1 && you.inv[tool].base_type == OBJ_MISCELLANY)
    {
        if (you.inv[tool].sub_type == MISC_LANTERN_OF_SHADOWS)
        {
            if (showMsgs)
                mpr("The area is filled with flickering shadows.");

            you.special_wield = SPWLD_SHADOW;
        }
    }

    // now for real weapon/stave wield effects
    const int wpn = get_inv_wielded();

    // no effects here if not wielded
    if (wpn == -1)
        return;

    if (you.inv[wpn].base_type == OBJ_STAVES)
    {
        if (you.inv[wpn].sub_type == STAFF_POWER)
        {
            // inc_max_mp(13);
            calc_mp_max();
            set_ident_flags( you.inv[wpn], ISFLAG_EQ_WEAPON_MASK );
        }

        set_ident_flags( you.inv[wpn], ISFLAG_KNOW_CURSE );
        if (item_cursed( you.inv[wpn] ))
            mpr("It sticks to your hand!");
    }

    if (you.inv[wpn].base_type == OBJ_WEAPONS)
    {
        int brand = SPWPN_NORMAL;

        if (is_demonic( you.inv[wpn] )
            && (you.religion == GOD_ZIN
                || you.religion == GOD_SHINING_ONE
                || you.religion == GOD_ELYVILON))
        {
            if (showMsgs)
                mpr("You really shouldn't be using a nasty item like this.");
        }

        set_ident_flags( you.inv[wpn], ISFLAG_EQ_WEAPON_MASK );

        if (is_random_artefact( you.inv[wpn] ))
        {
            brand = you.inv[wpn].ra_props[RAP_BRAND];
            use_randart( wpn );
        }
        else
        {
            brand = you.inv[wpn].special;
        }

        if (brand != SPWPN_NORMAL)
        {
            // message first
            if (showMsgs)
            {
                switch (brand)
                {
                case SPWPN_SWORD_OF_CEREBOV:
                case SPWPN_FLAMING:
                    if (is_range_weapon( you.inv[wpn] ))
                        mpr("It glows red for a moment.");
                    else
                        mpr("It bursts into flame!");
                    break;

                case SPWPN_FREEZING:
                    if (is_range_weapon( you.inv[wpn] ))
                        mpr("It is covered in frost.");
                    else
                        mpr("It glows with a cold blue light!");
                    break;

                case SPWPN_HOLY_WRATH:
                    mpr("It softly glows with a divine radiance!");
                    break;

                case SPWPN_ELECTROCUTION:
                    mpr("You hear the crackle of electricity.");
                    break;

                case SPWPN_ORC_SLAYING:
                    mpr((you.species == SP_HILL_ORC)
                            ? "You feel a sudden desire to commit suicide."
                            : "You feel a sudden desire to kill orcs!");
                    break;

                case SPWPN_VENOM:
                    mpr("It begins to drip with poison!");
                    break;

                case SPWPN_PROTECTION:
                    mpr("You feel protected!");
                    break;

                case SPWPN_DRAINING:
                    mpr("You sense an unholy aura.");
                    break;

                case SPWPN_SPEED:
                    mpr("Your hands tingle!");
                    break;

                case SPWPN_VAMPIRICISM:
                    if (!you.is_undead)
                        mpr("You feel a strange hunger.");
                    else
                        mpr("You feel strangely empty.");
                    break;

                case SPWPN_DISRUPTION:
                    mpr("You sense a holy aura.");
                    break;

                case SPWPN_PAIN:
                    mpr("A searing pain shoots up your arm!");
                    break;

                case SPWPN_SINGING_SWORD:
                    mpr("The Singing Sword hums in delight!");
                    break;

                case SPWPN_WRATH_OF_TROG:
                    mpr("You feel bloodthirsty!");
                    break;

                case SPWPN_SCYTHE_OF_CURSES:
                    mpr("A shiver runs down your spine.");
                    break;

                case SPWPN_GLAIVE_OF_PRUNE:
                    mpr("You feel pruney.");
                    break;

                case SPWPN_SCEPTRE_OF_TORMENT:
                    mpr("A terribly searing pain shoots up your arm!");
                    break;

                case SPWPN_SWORD_OF_ZONGULDROK:
                    mpr("You sense an extremely unholy aura.");
                    break;

                case SPWPN_SWORD_OF_POWER:
                    mpr("You sense an aura of extreme power.");
                    break;

                case SPWPN_STAFF_OF_OLGREB:
                    if (player_can_smell())
                        mpr("You smell chlorine.");
                    else
                        mpr("The staff glows green.");
                    break;

                case SPWPN_VAMPIRES_TOOTH:
                    // mummies cannot smell, and do not hunger {dlb}
                    if (!you.is_undead)
                        mpr("You feel a strange hunger, and smell blood on the air...");
                    else
                        mpr("You feel strangely empty.");
                    break;

                default:
                    break;
                }
            }

            // effect second
            switch (brand)
            {
            case SPWPN_PROTECTION:
                set_redraw_status( REDRAW_ARMOUR_CLASS );
                break;

            case SPWPN_DISTORTION:
                miscast_effect( SPTYP_TRANSLOCATION, 9, 90, 100, "a distortion effect" );
                break;

            case SPWPN_SINGING_SWORD:
                you.special_wield = SPWLD_SING;
                break;

            case SPWPN_WRATH_OF_TROG:
                you.special_wield = SPWLD_TROG;
                break;

            case SPWPN_SCYTHE_OF_CURSES:
                you.special_wield = SPWLD_CURSE;
                if (one_chance_in(5))
                    do_curse_item( you.inv[wpn] );
                break;

            case SPWPN_MACE_OF_VARIABILITY:
                you.special_wield = SPWLD_VARIABLE;
                break;

            case SPWPN_GLAIVE_OF_PRUNE:
                you.special_wield = SPWLD_NONE;
                break;

            case SPWPN_SCEPTRE_OF_TORMENT:
                you.special_wield = SPWLD_TORMENT;
                break;

            case SPWPN_SWORD_OF_ZONGULDROK:
                you.special_wield = SPWLD_ZONGULDROK;
                break;

            case SPWPN_SWORD_OF_POWER:
                you.special_wield = SPWLD_POWER;
                break;

            case SPWPN_STAFF_OF_OLGREB:
                // josh declares mummies cannot smell {dlb}
                you.special_wield = SPWLD_OLGREB;
                break;

            case SPWPN_STAFF_OF_WUCAD_MU:
                miscast_effect( SPTYP_DIVINATION, 9, 90, 100, "the Staff of Wucad Mu" );
                you.special_wield = SPWLD_WUCAD_MU;
                break;
            }
        }

        set_redraw_status( REDRAW_EVASION );

        if (item_cursed( you.inv[wpn] ))
            mpr("It sticks to your hand!");
    }
}                               // end wield_weapon()


// nows warns player if current weapon is not particularly useful -- bwr
void wield_warning( bool newWeapon )
{
    const int wpn = get_inv_in_hand();

    if (wpn == -1)
        return;

    // non-weapons don't give warnings here:
    if (you.inv[wpn].base_type != OBJ_WEAPONS
        && you.inv[wpn].base_type != OBJ_STAVES)
    {
        return;
    }

    const size_type  size  = player_size();
    const int        unfit = fit_weapon_wieldable_size( you.inv[wpn], size );

    if (unfit)
    {
        mpr( MSGCH_WARN,
              "%s weapon is too %s for you to wield properly%s!",
              (newWeapon) ? "This" : "Your",
              (unfit < 0) ? "small" : "large",
              (is_tool( you.inv[wpn] )
                   && check_weapon_tool_size( you.inv[wpn], size ))
                ? " (but is usable as a tool)" : "" );
    }

    const int weight_pen = heavy_weapon_penalty();

    if (weight_pen > 0)
    {
        mpr( MSGCH_WARN, "%s weapon is %stoo heavy for you to wield properly!",
                (newWeapon) ? "This" : "Your",
                (weight_pen <= 400)  ? "" :
                (weight_pen <= 1000) ? "much "
                                     : "far " );
    }
}

//---------------------------------------------------------------
//
// armour_prompt
//
// Prompt the user for some armour. Returns true if the user picked
// something legit.
//
//---------------------------------------------------------------
bool armour_prompt( const std::string & mesg, int *index )
{
    ASSERT(index != NULL);

    bool  succeeded = false;
    int   slot;

    if (inv_count() < 1)
        canned_msg(MSG_NOTHING_CARRIED);
    else if (you.berserker)
        canned_msg(MSG_TOO_BERSERK);
    else
    {
        slot = prompt_invent_item( mesg.c_str(), OBJ_ARMOUR );

        if (slot != PROMPT_ABORT)
        {
            *index = slot;
            succeeded = true;
        }
    }

    return (succeeded);
}                               // end armour_prompt()

// NOTE: This sort of peaking at the delay queue details doesn't really
// belong out here (should probably be hidden in delay.cc).
static bool cloak_is_being_removed( void )
{
    if (current_delay_action() != DELAY_ARMOUR_OFF)
        return (false);

    if (you.delay_queue.front().parm1 != you.equip[ EQ_CLOAK ])
        return (false);

    return (true);
}

//---------------------------------------------------------------
//
// wear_armour
//
//---------------------------------------------------------------
void wear_armour(void)
{
    int armour_wear_2;

    if (!armour_prompt("Wear which item?", &armour_wear_2))
        return;

    do_wear_armour( armour_wear_2, false );
}

bool do_wear_armour( int arm, bool quiet )
{
    if (!is_valid_item( you.inv[arm] ))
    {
        if (!quiet)
           mpr("You don't have any such object.");

        return (false);
    }

    if (you.inv[arm].base_type != OBJ_ARMOUR)
    {
        mpr( "That's not even wearable!" );
        return (false);
    }

    for (int loopy = EQ_CLOAK; loopy <= EQ_BODY_ARMOUR; loopy++)
    {
        if (arm == you.equip[loopy])
        {
            if (!quiet)
               mpr("You are already wearing that!");

            return (false);
        }
    }

    const int           slot = get_armour_slot( you.inv[arm] );
    const size_type     size = player_size();
    const int           in_hand = get_inv_in_hand();

    if (slot == EQ_SHIELD
        && in_hand != -1
        && hands_reqd( you.inv[in_hand], size ) == HANDS_TWO)
    {
        if (!quiet)
           mpr("You'd need three hands to do that!");

        return (false);
    }

    const int  fit = fit_armour_size( you.inv[arm], size );

    if (fit != 0)
    {
        if (!quiet)
        {
            mpr( "That %s is too %s for you.",
                  (slot == EQ_SHIELD) ? "shield" : "armour",
                  (fit < 0) ? "small" : "large" );
        }

        return (false);
    }

    if (!check_armour_shape( you.inv[arm], false ))
        return (false);

    if (arm == you.equip[EQ_WEAPON])
    {
        you.turn_is_over = true;   // doing work by unwielding item
        unwield_item();
    }

    bool removedCloak = false;
    int  cloak = -1;

    if (slot == EQ_BODY_ARMOUR
        && (you.equip[EQ_CLOAK] != -1 && !cloak_is_being_removed()))
    {
        if (!item_cursed( you.inv[you.equip[EQ_CLOAK]] ))
        {
            cloak = you.equip[EQ_CLOAK];
            if (!takeoff_armour( you.equip[EQ_CLOAK] ))
                return (false);

            removedCloak = true;
        }
        else
        {
            if (!quiet)
               mpr("Your cloak prevents you from wearing the armour.");

            return (false);
        }
    }

    if (you.equip[slot] != -1)
    {
        if (!takeoff_armour( you.equip[slot] ))
            return (false);
    }

    you.turn_is_over = true;

    int delay = -property( you.inv[arm], PARM_EVASION );

    // non-body armour (but not barding) is faster to change:
    if (slot != EQ_BODY_ARMOUR && slot != EQ_BOOTS)
        delay /= 2;

    start_delay( DELAY_ARMOUR_ON, delay, arm );

    if (removedCloak)
        start_delay( DELAY_ARMOUR_ON, 0, cloak );

    return (true);
}                               // end do_wear_armour()

bool takeoff_armour( int arm )
{
    if (you.inv[arm].base_type != OBJ_ARMOUR)
    {
        mpr( "That's not even wearable!" );
        return (false);
    }

    if (item_cursed( you.inv[arm] ))
    {
        for (int loopy = EQ_CLOAK; loopy <= EQ_BODY_ARMOUR; loopy++)
        {
            if (arm == you.equip[loopy])
            {
                in_name(arm, DESC_CAP_YOUR, info);
                strcat(info, " is stuck to your body!");
                mpr(info);
                return (false);
            }
        }
    }

    bool removedCloak = false;
    int cloak = -1;

    const int slot = get_armour_slot( you.inv[arm] );

    if (slot == EQ_BODY_ARMOUR)
    {
        if (you.equip[EQ_CLOAK] != -1 && !cloak_is_being_removed())
        {
            if (!item_cursed( you.inv[you.equip[EQ_CLOAK]] ))
            {
                cloak = you.equip[ EQ_CLOAK ];
                if (!takeoff_armour(you.equip[EQ_CLOAK]))
                    return (false);

                removedCloak = true;
            }
            else
            {
                mpr("Your cloak prevents you from removing the armour.");
                return (false);
            }
        }
    }

    if (arm != you.equip[slot])
    {
        mpr("You aren't wearing that!");
        return (false);
    }

    you.turn_is_over = true;

    int delay = -property( you.inv[arm], PARM_EVASION );

    // non-body armour (but not barding) is faster to change:
    if (slot != EQ_BODY_ARMOUR && slot != EQ_BOOTS)
        delay /= 2;

    start_delay( DELAY_ARMOUR_OFF, delay, arm );

    if (removedCloak)
        start_delay( DELAY_ARMOUR_ON, 0, cloak );

    return (true);
}                               // end takeoff_armour()

// Return index of first missile of (base_type, sub_type) or ENDOFPACK
static int find_missile( int base_type, int sub_type )
{
    int i;
    const size_type size = player_size();

    for (i = Options.fire_items_start; i < ENDOFPACK; i++)
    {
        // skip invalid objects
        if (!is_valid_item( you.inv[i] ))
            continue;

        // don't throw wielded melee weapons
        if (base_type == OBJ_WEAPONS && you.equip[EQ_WEAPON] == i)
            continue;

        // skip items that aren't of an appropriate size (ie fit != 0)
        if (fit_item_throwable_size( you.inv[i], size ))
            continue;

        // consider melee weapons that can also be thrown
        if (you.inv[i].base_type == base_type
            && you.inv[i].sub_type == sub_type)
        {
            break;
        }
    }

    return (i);
}

// Note: This is a simple implementation, not an efficient one. -- bwr
//
// Returns item index or ENDOFPACK if no item found for auto-firing
int get_fire_item_index( void )
{
    int index = ENDOFPACK;
    const int wpn = get_inv_wielded(); // must be wielded for launcher to work

    for (int i = 0; i < NUM_FIRE_TYPES; i++)
    {
        // look for next type on list... if found index is set != ENDOFPACK
        switch (Options.fire_order[i])
        {
        case FIRE_LAUNCHER:
            // check if we have ammo for a wielded launcher:
            if (wpn != -1
                && you.inv[wpn].base_type == OBJ_WEAPONS
                && is_range_weapon( you.inv[wpn] ))
            {
                index = find_missile( OBJ_MISSILES,
                                      fires_ammo_type( you.inv[wpn] ) );
            }
            break;

        case FIRE_DART:
            index = find_missile( OBJ_MISSILES, MI_DART );
            break;

        case FIRE_STONE:
            index = find_missile( OBJ_MISSILES, MI_STONE );
            break;

        case FIRE_ROCK:
            index = find_missile( OBJ_MISSILES, MI_LARGE_ROCK );
            break;

        case FIRE_DAGGER:
            index = find_missile( OBJ_WEAPONS, WPN_DAGGER );
            break;

        case FIRE_SPEAR:
            index = find_missile( OBJ_WEAPONS, WPN_SPEAR );
            break;

        case FIRE_HAND_AXE:
            index = find_missile( OBJ_WEAPONS, WPN_HAND_AXE );
            break;

        case FIRE_CLUB:
            index = find_missile( OBJ_WEAPONS, WPN_CLUB );
            break;

        case FIRE_NONE:
        default:
            break;
        }

        // if successful break
        if (index != ENDOFPACK)
            break;
    }

    // either item was found or is still ENDOFPACK for no item
    return (index);
}

void shoot_thing( void )
{
    struct bolt beam;  // passed in by reference, but never used here
    char str_pass[ ITEMNAME_SIZE ];

    if (you.berserker)
    {
        canned_msg(MSG_TOO_BERSERK);
        return;
    }
    else if (player_is_shapechanged( true ))
    {
        canned_msg(MSG_NOT_IN_PRESENT_FORM);
        return;
    }

    const int index = get_fire_item_index();

    if (index == ENDOFPACK)
    {
        mpr("No suitable missiles.");
        return;
    }

    in_name( index, DESC_INVENTORY_EQUIP, str_pass );
    mpr( "Firing: %s", str_pass );

    throw_it( beam, index );
}                               // end shoot_thing()

void throw_anything( void )
{
    struct bolt beam;
    int throw_slot;

    if (you.berserker)
    {
        canned_msg(MSG_TOO_BERSERK);
        return;
    }
    else if (player_is_shapechanged( true ))
    {
        canned_msg(MSG_NOT_IN_PRESENT_FORM);
        return;
    }
    else if (inv_count() < 1)
    {
        canned_msg(MSG_NOTHING_CARRIED);
        return;
    }

    throw_slot = prompt_invent_item( "Throw which item?", OBJ_MISSILES );
    if (throw_slot == PROMPT_ABORT)
    {
        canned_msg( MSG_OK );
        return;
    }

    if (throw_slot == you.equip[EQ_WEAPON])
    {
        // must be at least a tool to stick
        const int tool = get_inv_hand_tool();

        if (tool != -1 && item_cursed( you.inv[tool] ))
        {
            mpr("That thing is stuck to your hand!");
            return;
        }
    }
    else // check if it's worn armour
    {
        for (int loopy = EQ_CLOAK; loopy <= EQ_AMULET; loopy++)
        {
            if (throw_slot == you.equip[loopy])
            {
                mpr("You are wearing that object!");
                return;
            }
        }
    }

    throw_it( beam, throw_slot );
}                               // end throw_anything()

static int get_item_thrown_symbol( item_def &ammo )
{
    int ret = SYM_MISSILE;

    switch (ammo.base_type)
    {
    case OBJ_WEAPONS:    ret = SYM_WEAPON;  break;
    case OBJ_MISSILES:   ret = SYM_MISSILE; break;
    case OBJ_ARMOUR:     ret = SYM_ARMOUR;  break;
    case OBJ_WANDS:      ret = SYM_STICK;   break;
    case OBJ_FOOD:       ret = SYM_CHUNK;   break;
    case OBJ_UNKNOWN_I:  ret = SYM_BURST;   break;
    case OBJ_SCROLLS:    ret = SYM_SCROLL;  break;
    case OBJ_JEWELLERY:  ret = SYM_TRINKET; break;
    case OBJ_POTIONS:    ret = SYM_FLASK;   break;
    case OBJ_UNKNOWN_II: ret = SYM_ZAP;     break;
    case OBJ_BOOKS:      ret = SYM_OBJECT;  break;
        // this does not seem right, but value was 11 {dlb}
        // notice how the .type does not match the class -- hmmm... {dlb}
    case OBJ_STAVES:     ret = SYM_CHUNK;   break;
    }

    return (ret);
}


// throw_it - currently handles player throwing only.  Monster
// throwing is handled in mstuff2:mons_throw()
static void throw_it( struct bolt &pbolt, int ammo_id )
{
    struct dist thr;

    char str_pass[ ITEMNAME_SIZE ];

    mpr( MSGCH_PROMPT, STD_DIRECTION_PROMPT );
    message_current_target();

    direction( thr, DIR_NONE, TARG_ENEMY );

    if (!thr.isValid)
    {
        if (thr.isCancel)
            canned_msg(MSG_OK);

        return;
    }

    // Must unwield before we make a copy in order to remove things
    // like temporary branding. -- bwr
    if (ammo_id == you.equip[EQ_WEAPON] && you.inv[ammo_id].quantity == 1)
        unwield_item();

    // Making a copy of the item: changed only for venom launchers
    item_def ammo = you.inv[ammo_id];
    ammo.quantity = 1;

    // Now start filling in the pbolt struct:
    pbolt.source_x = you.x_pos;
    pbolt.source_y = you.y_pos;

    if (you.conf)
    {
        pbolt.target_x = you.x_pos + random2(13) - 6;
        pbolt.target_y = you.y_pos + random2(13) - 6;
    }
    else
    {
        pbolt.target_x = thr.tx;
        pbolt.target_y = thr.ty;
        pbolt.target_first = thr.dir;
    }

    pbolt.type = get_item_thrown_symbol( ammo );
    pbolt.colour = ammo.colour;

    item_name( ammo, DESC_PLAIN, str_pass );
    strncpy( pbolt.name, str_pass, BEAMNAME_SIZE );

    // ensure we're firing a 'missile'-type beam
    pbolt.flavour = BEAM_MISSILE;
    pbolt.is_beam = false;
    pbolt.is_enchant = false;
    pbolt.is_tracer = false;

    pbolt.thrower = KILL_YOU_MISSILE;
    pbolt.aux_source = NULL;
    pbolt.is_launched = false;     // default, set to true later if launched
    pbolt.is_thrown = false;       // default, set to true later if thrown

    const launch_retval launched = is_launched( MHITYOU, ammo, true );
    const size_type     size     = player_size();

    // Calculations for to-hit, damage, speed:
    int hit_dex_factor = you.dex;   // pluses that benefit from dex
    int hit_plus = 0;               // non-dex magical pluses and such
    int hit_skill_factor = 0;       // factor from skill
    int dex_weight = 10;            // amount of skill that's dex related

    int base_dam = 0;               // base damage done
    int dam_plus = 0;               // magical pluses and situational factors
    int dam_bonus = -50;            // as a percent, applied to base (not plus)
    bool no_damage = false;         // weapon does 0 damage (ie blowgun)

    int speed = 100;

    if (launched == LRET_LAUNCHED)
    {
        const int bow = get_inv_wielded();

        // We better have a launcher if we're launching:
        ASSERT( bow != -1 );

        // Until we have spear launchers or bows as a separate OBJ class:
        ASSERT( you.inv[bow].base_type == OBJ_WEAPONS );
        ASSERT( ammo.base_type == OBJ_MISSILES );

        const int  shoot_skill  = range_skill( you.inv[bow] );
        const bool two_handed   = (you.equip[EQ_SHIELD] == -1);
        const int  str_weight   = weapon_str_weight( you.inv[bow] );
        const int  str_factor   = property( you.inv[bow], PWPN_DAMAGE );
        const int  bow_brand    = get_weapon_brand( you.inv[bow] );
        const int  ammo_brand   = get_ammo_brand( ammo );
        const bool poisoned     = (ammo_brand == SPMSL_POISONED
                                    || ammo_brand == SPMSL_POISONED_UNUSED);

        const int  effective_str = (you.str <= 15) ? you.str
                                                   : 15 + (you.str - 15) / 2;

        // mark the fact that this is launched:
        pbolt.is_launched = true;

        pbolt.range = 8;

        hit_plus = property( you.inv[bow], PWPN_HIT )
                    + slaying_bonus( PWPN_HIT )
                    + you.inv[bow].plus + ammo.plus;

        hit_skill_factor = 4 * skill_bump( SK_RANGED_COMBAT )
                            + 2 * you.skills[ shoot_skill ];

        dex_weight = 10 - str_weight;

        base_dam = property( ammo, PWPN_DAMAGE );
        dam_plus = slaying_bonus( PWPN_DAMAGE )
                   + you.inv[bow].plus2 + ammo.plus;  // ammo only has 1 plus

        dam_bonus += 3 * you.skills[SK_RANGED_COMBAT]
                    + 7 * you.skills[shoot_skill]
                    + str_factor * effective_str;

        // check for matches;  dwarven,elven,orcish
        if (get_equip_race( you.inv[bow] ) != ISFLAG_NO_RACE)
        {
            if (get_equip_race( you.inv[bow] ) == get_equip_race( ammo ))
            {
                hit_dex_factor += 1 * TOHIT_PLUS_MULTIPLE;
                dam_plus += 1;

                // elves with elven bows
                if (get_equip_race( you.inv[bow] ) == ISFLAG_ELVEN &&
                        player_genus(GENPC_ELVEN))
                {
                    hit_dex_factor += 1 * TOHIT_PLUS_MULTIPLE;
                }
            }
        }

        int speed_base = 10 * property( you.inv[bow], PWPN_SPEED );
        int speed_min = 50;
        int speed_stat = str_weight * you.str + (10 - str_weight) * you.dex;

        // This matches the original distribution:
        int prac = one_chance_in(4) ? (coinflip() ? 1 : 4)
                                    : (coinflip() ? 2 : 3);

        switch (you.inv[bow].sub_type)
        {
        case WPN_SLING:
            // Slings are easier to learn because they're not really all
            // that good, and its harder to get ammo anyways.
            prac += (coinflip() ? 1 : 2);
            hit_dex_factor += you.dex - 10;
            dam_bonus += 2 * you.dex;
            break;

        case WPN_BLOWGUN:
            no_damage = true;
            hit_dex_factor += you.dex - 10;
            break;

        case WPN_BOW:
        case WPN_LONGBOW:
            speed_min = 40;
            hit_skill_factor += you.skills[ shoot_skill ];
            break;

        case WPN_CROSSBOW:
            speed_min = 60;
            break;

        case WPN_HAND_CROSSBOW:
            break;
        }

        // Apply time penalty for loading launcher without both hands free
        if (!two_handed)
        {
            // Since we're assuming range weapons are either
            // HANDS_HALF or HANDS_TWO:
            ASSERT( hands_reqd( you.inv[bow], size ) == HANDS_HALF );

            speed_base = (speed_base * 3 + 1) / 2;
            speed_min = (speed_min * 3 + 1) / 2;
        }

        speed = speed_base - (you.skills[ shoot_skill ] * speed_stat) / 50;

        if (speed < speed_min)
            speed = speed_min;

        // special cases for flame, frost, poison, etc.
        // check for venom brand (usually only available for blowguns)
        if (bow_brand == SPWPN_VENOM && ammo_brand == SPMSL_NORMAL)
        {
            // poison brand the ammo
            set_item_ego_type( ammo, OBJ_MISSILES, SPMSL_POISONED );
            item_name( ammo, DESC_PLAIN, str_pass );
            strcpy( pbolt.name, str_pass );
        }

        // Note that bow_brand is known since the bow is equiped.
        if ((bow_brand == SPWPN_FLAMING || ammo_brand == SPMSL_FLAME)
            && ammo_brand != SPMSL_ICE && bow_brand != SPWPN_FREEZING)
        {
            // Remember that this consumes the ammo, thus the bonus
            // needs to be big enough (is it, though?)
            dam_bonus += 300;
            pbolt.flavour = BEAM_FIRE;
            strcpy(pbolt.name, "bolt of ");

            if (poisoned)
                strcat(pbolt.name, "poison ");

            strcat(pbolt.name, "flame");
            pbolt.colour = RED;
            pbolt.type = SYM_BOLT;
            pbolt.thrower = KILL_YOU_MISSILE;
            pbolt.aux_source = NULL;

            // ammo known if we can't attribute it to the bow
            if (bow_brand != SPWPN_FLAMING)
            {
                set_ident_flags( ammo, ISFLAG_KNOW_TYPE );
                set_ident_flags( you.inv[ammo_id], ISFLAG_KNOW_TYPE );
            }
        }

        if ((bow_brand == SPWPN_FREEZING || ammo_brand == SPMSL_ICE)
            && ammo_brand != SPMSL_FLAME && bow_brand != SPWPN_FLAMING)
        {
            // Remember that this consumes the ammo, thus the bonus
            // needs to be big enough (is it, though?)
            dam_bonus += 300;
            pbolt.flavour = BEAM_COLD;
            strcpy(pbolt.name, "bolt of ");

            if (poisoned)
                strcat(pbolt.name, "poison ");

            strcat(pbolt.name, "frost");
            pbolt.colour = WHITE;
            pbolt.type = SYM_BOLT;
            pbolt.thrower = KILL_YOU_MISSILE;
            pbolt.aux_source = NULL;

            // ammo known if we can't attribute it to the bow
            if (bow_brand != SPWPN_FREEZING)
            {
                set_ident_flags( ammo, ISFLAG_KNOW_TYPE );
                set_ident_flags( you.inv[ammo_id], ISFLAG_KNOW_TYPE );
            }
        }

        // ammo known if it cancels the effect of the bow
        if ((bow_brand == SPWPN_FLAMING && ammo_brand == SPMSL_ICE)
            || (bow_brand == SPWPN_FREEZING && ammo_brand == SPMSL_FLAME))
        {
            set_ident_flags( ammo, ISFLAG_KNOW_TYPE );
            set_ident_flags( you.inv[ammo_id], ISFLAG_KNOW_TYPE );
        }

        // XXX: small because we don't have a mechanism for checking
        // to see it the target can resist it, and we simply don't
        // want vorpal bows to be the nonthinking Correct (tm) choice...
        // maybe we should just remove them?
        if (bow_brand == SPWPN_VORPAL)
            dam_bonus += 50;

        // XXX: Of course, maybe these should go too... they are always going
        // to be no brain choices.
        if (bow_brand == SPWPN_SPEED)
            speed = (speed * 2) / 3;

        /* the chief advantage here is the extra damage this does
         * against susceptible creatures */

        /* Note: weapons & ammo of eg fire are not cumulative
         * ammo of fire and weapons of frost don't work together,
         * and vice versa */

        // practice the launcher skill:
        exercise( shoot_skill, prac );

        // maybe learn a little general skill too:
        if (coinflip())
            exercise( SK_RANGED_COMBAT, 1 );

        // ID check
        if (!item_ident( you.inv[bow], ISFLAG_KNOW_PLUSES )
            && random2(100) < you.skills[ shoot_skill ])
        {
            ASSERT( player_weapon_wielded() );
            set_ident_flags( you.inv[bow], ISFLAG_KNOW_PLUSES );

            strcpy(info, "You are wielding ");
            in_name( bow, DESC_NOCAP_A, str_pass );
            strcat( info, str_pass );
            strcat( info, "." );
            mpr(info);

            more();
            set_redraw_status( REDRAW_WIELD );
        }
    }
    else if (launched == LRET_THROWN)
    {
        pbolt.range = 6;
        int speed_min = 50;

        pbolt.is_thrown = true;

        if (ammo.base_type == OBJ_MISSILES)
        {
            base_dam = property( ammo, PWPN_DAMAGE );

            hit_skill_factor += 5 * skill_bump( SK_RANGED_COMBAT );

            hit_plus += slaying_bonus( PWPN_HIT ) + ammo.plus;
            dam_plus += slaying_bonus( PWPN_DAMAGE ) + ammo.plus;

            dam_bonus += 3 * you.skills[ SK_RANGED_COMBAT ];

            if (ammo.sub_type == MI_LARGE_ROCK)
            {
                dex_weight = 2;
                speed_min = 100;
                pbolt.range = you.str / 5 + random2(3);

                // size -> leverage -> distance
                if (size < SIZE_GIANT)
                    pbolt.range /= (SIZE_GIANT - size + 1);

                if (pbolt.range > 8)
                    pbolt.range = 8;
                else if (pbolt.range < 1)
                    pbolt.range = 1;
            }
            else if (ammo.sub_type == MI_DART)
            {
                pbolt.range = 8;
                hit_dex_factor += you.dex - 10;
                hit_skill_factor += 2 * skill_bump( SK_DARTS );

                dam_bonus += 5 * you.skills[ SK_DARTS ] + 2 * you.dex;

                speed = 100 - (you.skills[ SK_DARTS ] * you.dex) / 5;

                exercise( SK_DARTS, roll_dice(1,3) );
            }
        }
        else
        {
            ASSERT( ammo.base_type == OBJ_WEAPONS );

            base_dam = property( ammo, PWPN_DAMAGE );
            dex_weight = weapon_dex_weight( ammo );

            hit_skill_factor += 5 * skill_bump( SK_RANGED_COMBAT );
            hit_dex_factor += you.dex - 10;
            hit_plus += property( ammo, PWPN_HIT ) - 3
                        + slaying_bonus( PWPN_HIT )
                        + ammo.plus;

            dam_bonus += 8 * you.skills[ SK_RANGED_COMBAT ]
                            + ((10 + dex_weight) * you.dex) / 5;

            dam_plus += slaying_bonus( PWPN_DAMAGE ) + ammo.plus2;

            const int speed_stat = dex_weight * you.dex
                                    + (10 - dex_weight) * you.str;

            speed = 100 - (you.skills[ SK_RANGED_COMBAT ] * speed_stat) / 50;

            speed_min = 85 - 5 * dex_weight;
            if (speed_min < 40)
                speed_min = 40;
        }

        if (speed < speed_min)
            speed = speed_min;

        // exercise skill
        if (coinflip())
            exercise(SK_RANGED_COMBAT, 1);
    }
    else // throwing an unbalanced item
    {
        const int mass = item_mass( ammo );
        const int fit  = fit_item_throwable_size( ammo, size );

        // to hit and damage for generic objects
        hit_skill_factor = 2 * skill_bump( SK_RANGED_COMBAT );
        hit_dex_factor += you.str - mass / 10;

        if (ammo.base_type != OBJ_WEAPONS)
            base_dam = mass / 100;
        else
        {
            // special: might be throwing generic weapon;
            // use base wep. damage, w/ penalty
            base_dam = property( ammo, PWPN_DAMAGE ) - 4;
            if (base_dam < 2)
                base_dam = 2;
        }

        speed = ((8 + random2(5)) * mass) / you.str;

        if (speed < 100)
            speed = 100;
        else if (speed > 250)
            speed = 250;

        // calculate range (1..4)
        if (speed > 150)
            pbolt.range = 1;
        else if (speed > 100)
            pbolt.range = 2;
        else
            pbolt.range = 3;

        if (speed < 200)
            pbolt.range += (random2( speed + 100 ) < 100);

        if (fit > 0)
            pbolt.range -= (fit + 1);
        else if (fit < 0)
            pbolt.range += (random2(3) - 1);

        if (pbolt.range < 1)
            pbolt.range = 1;

        if (one_chance_in(20))
            exercise( SK_RANGED_COMBAT, 1 );
    }

    // set max range equal to range for this
    pbolt.rangeMax = pbolt.range;

    // Calculate to-hit:
    // XXX: apply heavy equipment penalty, any other factors?
    // (1) skill, with weighted portion scaled to DEX
    const int dex_skill = (hit_skill_factor * dex_weight) / 10;

    hit_dex_factor += dex_skill;
    pbolt.hit += hit_skill_factor - dex_skill;

    // (2) apply dex magnified factors
    if (hit_dex_factor <= 0)
        pbolt.hit += hit_dex_factor;
    else if (!you.conf)
        pbolt.hit += stat_mult( you.dex, hit_dex_factor );

    // (3) apply pluses
    pbolt.hit += TOHIT_PLUS_MULTIPLE * hit_plus;

    // Calculate damage:
    pbolt.damage.num = 1;

    if (no_damage)                          // ie needles
        pbolt.damage.size = 0;
    else
    {
        pbolt.damage.size = base_dam
                            + dam_plus
                            + div_rand_round( base_dam * dam_bonus, 100 );
    }

    // Applying speed to time_taken:
    // never faster than 4 per round.
    if (speed < 25)
        speed = 25;

    you.time_taken = div_rand_round( speed * you.time_taken, 100 );

    // create message
    mpr( "You %s a%s %s.",
          (launched == LRET_LAUNCHED) ? "shoot" : "throw",
          is_vowel( pbolt.name[0] ) ? "n" : "", pbolt.name );

#if DEBUG_DIAGNOSTICS
    mpr( MSGCH_DIAGNOSTICS, "hit:%d (pl:%d, sk:%d, dex:%d); dam:%dd%d (pl:%d, bon:%d); spd:%d",
          pbolt.hit, hit_plus, hit_skill_factor, hit_dex_factor,
          pbolt.damage.num, pbolt.damage.size, dam_plus, dam_bonus,
          speed );
#endif

    // mark this item as thrown if it's a missile, so that we'll pick it up
    // when we walk over it.
    ammo.flags |= ISFLAG_THROWN;

    // using copy, since the launched item might be differect (venom blowgun)
    fire_beam( pbolt, &ammo );
    dec_inv_item_quantity( ammo_id, 1 );

    // Making all ranged combat silent to give it a bit of a boost. -- bwr
    //
    // throwing and blowguns are silent
    // if (launched && lnchType != WPN_BLOWGUN)
    //     noisy( SL_BOW, you.x_pos, you.y_pos );

    // but any monster nearby can see that something has been thrown:
    alert_nearby_monsters();

    you.turn_is_over = true;
}                               // end throw_it()

void puton_ring(void)
{
    char str_pass[ITEMNAME_SIZE];
    int item_slot;

    if (inv_count() < 1)
    {
        canned_msg(MSG_NOTHING_CARRIED);
        return;
    }

    if (you.berserker)
    {
        canned_msg(MSG_TOO_BERSERK);
        return;
    }

    item_slot = prompt_invent_item( "Put on which piece of jewellery?",
                                    OBJ_JEWELLERY );

    if (item_slot == PROMPT_ABORT)
    {
        canned_msg( MSG_OK );
        return;
    }

    if (you.inv[item_slot].base_type != OBJ_JEWELLERY)
    {
        mpr("You can only put on jewellery.");
        return;
    }

    if (!check_jewellery_size( you.inv[item_slot], player_size() ))
    {
        // currently only the very large cannot wear rings
        mpr("It's too small for you to put on!");
        return;
    }

    if (item_slot == you.equip[EQ_LEFT_RING]
        || item_slot == you.equip[EQ_RIGHT_RING]
        || item_slot == you.equip[EQ_AMULET])
    {
        mpr("You've already put that on!");
        return;
    }

    if (item_slot == you.equip[EQ_WEAPON])
    {
        you.turn_is_over = true;           // doing work by unwielding item
        unwield_item();
        // redraw_screen();
    }

    const bool is_amulet = jewellery_is_amulet( you.inv[item_slot] );

    if (!is_amulet)     // ie it's a ring
    {
        // Blade hands: cannot put on or remove a ring, but rings
        // aren't removed on transformation (they get stuck in the
        // blade somehow).
        if (you.attribute[ATTR_TRANSFORMATION] == TRAN_BLADE_HANDS)
        {
            mpr("You can't put that on your blade hands!");
            return;
        }

        if (you.equip[EQ_GLOVES] != -1
            && item_cursed( you.inv[you.equip[EQ_GLOVES]] ))
        {
            mpr("You can't take your gloves off to put on a ring!");
            return;
        }

        if (you.inv[item_slot].base_type == OBJ_JEWELLERY
            && you.equip[EQ_LEFT_RING] != -1
            && you.equip[EQ_RIGHT_RING] != -1)
        {
            // and you are trying to wear body you.equip.
            mpr("You've already put a ring on each hand.");
            return;
        }
    }
    else if (you.equip[EQ_AMULET] != -1)
    {
        strcpy(info, "You are already wearing an amulet.");

        if (one_chance_in(20))
            strcat(info, " And I must say it looks quite fetching.");

        mpr(info);
        return;
    }

    int hand_used = 0;

    if (you.equip[EQ_LEFT_RING] != -1)
        hand_used = 1;

    if (you.equip[EQ_RIGHT_RING] != -1)
        hand_used = 0;

    if (is_amulet)
        hand_used = 2;
    else if (you.equip[EQ_LEFT_RING] == -1 && you.equip[EQ_RIGHT_RING] == -1)
    {
        mpr(MSGCH_PROMPT,"Put on which hand (l or r)?" );

        int keyin = get_ch();

        if (keyin == 'l')
            hand_used = 0;
        else if (keyin == 'r')
            hand_used = 1;
        else if (keyin == ESCAPE)
            return;
        else
        {
            mpr("You don't have such a hand!");
            return;
        }
    }

    you.equip[ EQ_LEFT_RING + hand_used ] = item_slot;

    int ident = ID_TRIED_TYPE;

    switch (you.inv[item_slot].sub_type)
    {
    case RING_FIRE:
    case RING_HUNGER:
    case RING_ICE:
    case RING_LIFE_PROTECTION:
    case RING_POISON_RESISTANCE:
    case RING_PROTECTION_FROM_COLD:
    case RING_PROTECTION_FROM_FIRE:
    case RING_PROTECTION_FROM_MAGIC:
    case RING_SUSTAIN_ABILITIES:
    case RING_SUSTENANCE:
    case RING_SLAYING:
    case RING_SEE_INVISIBLE:
    case RING_TELEPORTATION:
    case RING_WIZARDRY:
    case RING_REGENERATION:
        break;

    case RING_PROTECTION:
        set_redraw_status( REDRAW_ARMOUR_CLASS );
        if (you.inv[item_slot].plus != 0)
            ident = ID_KNOWN_TYPE;
        break;

    case RING_INVISIBILITY:
        if (!you.invis)
        {
            mpr("You become transparent for a moment.");
            ident = ID_KNOWN_TYPE;
        }
        break;

    case RING_EVASION:
        set_redraw_status( REDRAW_EVASION );
        if (you.inv[item_slot].plus != 0)
            ident = ID_KNOWN_TYPE;
        break;

    case RING_STRENGTH:
        modify_stat( STAT_STRENGTH, you.inv[item_slot].plus, true );
        if (you.inv[item_slot].plus != 0)
            ident = ID_KNOWN_TYPE;
        break;

    case RING_DEXTERITY:
        modify_stat( STAT_DEXTERITY, you.inv[item_slot].plus, true );
        if (you.inv[item_slot].plus != 0)
            ident = ID_KNOWN_TYPE;
        break;

    case RING_INTELLIGENCE:
        modify_stat( STAT_INTELLIGENCE, you.inv[item_slot].plus, true );
        if (you.inv[item_slot].plus != 0)
            ident = ID_KNOWN_TYPE;
        break;

    case RING_MAGICAL_POWER:
        calc_mp_max();
        ident = ID_KNOWN_TYPE;
        break;

    case RING_LEVITATION:
        mpr("You feel buoyant.");
        ident = ID_KNOWN_TYPE;
        break;

    case RING_TELEPORT_CONTROL:
        // XXX: is this safe or should we make it a function -- bwr
        if (!allow_control_teleport( true ))
            mpr( "The ring glows red." );
        break;

    case AMU_RAGE:
        mpr("You feel a brief urge to hack something to bits.");
        ident = ID_KNOWN_TYPE;
        break;
    }

    you.turn_is_over = true;

    // Artefacts have completely different appearance than base types
    // so we don't allow them to make the base types known
    if (is_random_artefact( you.inv[item_slot] ))
        use_randart(item_slot);
    else
    {
        set_ident_type( you.inv[item_slot].base_type,
                        you.inv[item_slot].sub_type, ident );
    }

    if (ident == ID_KNOWN_TYPE)
        set_ident_flags( you.inv[item_slot], ISFLAG_EQ_JEWELLERY_MASK );

    if (item_cursed( you.inv[item_slot] ))
    {
        mpr( "Oops, that %s feels deathly cold.", (is_amulet) ? "amulet"
                                                              : "ring" );
    }

    // cursed or not, we know that since we've put the ring on
    set_ident_flags( you.inv[item_slot], ISFLAG_KNOW_CURSE );

    in_name( item_slot, DESC_INVENTORY_EQUIP, str_pass );
    mpr( str_pass );
}                               // end puton_ring()

void unwear_ring( int ring )
{
    switch (you.inv[ring].sub_type)
    {
    case RING_FIRE:
    case RING_HUNGER:
    case RING_ICE:
    case RING_LIFE_PROTECTION:
    case RING_POISON_RESISTANCE:
    case RING_PROTECTION_FROM_COLD:
    case RING_PROTECTION_FROM_FIRE:
    case RING_PROTECTION_FROM_MAGIC:
    case RING_REGENERATION:
    case RING_SEE_INVISIBLE:
    case RING_SLAYING:
    case RING_SUSTAIN_ABILITIES:
    case RING_SUSTENANCE:
    case RING_TELEPORTATION:
    case RING_WIZARDRY:
    case RING_MAGICAL_POWER:
    case RING_TELEPORT_CONTROL:
    default:
        break;

    case RING_PROTECTION:
        set_redraw_status( REDRAW_ARMOUR_CLASS );
        break;

    case RING_EVASION:
        set_redraw_status( REDRAW_EVASION );
        break;

    case RING_STRENGTH:
        modify_stat( STAT_STRENGTH, -you.inv[ring].plus, true );
        break;

    case RING_DEXTERITY:
        modify_stat( STAT_DEXTERITY, -you.inv[ring].plus, true );
        break;

    case RING_INTELLIGENCE:
        modify_stat( STAT_INTELLIGENCE, -you.inv[ring].plus, true );
        break;

    case RING_INVISIBILITY:
        // removing this ring effectively cancels all invisibility {dlb}
        if (you.invis)
            you.invis = 1;
        break;

    case RING_LEVITATION:
        // removing this ring now removes all levitation, unless the
        // PC is wearing levitation boots (which never time out).
        if (you.levitation
            && !player_equip_ego_type( EQ_BOOTS, SPARM_LEVITATION ))
        {
            you.levitation = 1;
        }
        break;
    }

    if (is_random_artefact( you.inv[ring] ))
        unuse_randart( ring );
}

void remove_ring( void )
{
    char str_pass[ ITEMNAME_SIZE ];

    if (you.equip[EQ_LEFT_RING] == -1
        && you.equip[EQ_RIGHT_RING] == -1
        && you.equip[EQ_AMULET] == -1)
    {
        mpr( "You aren't wearing any rings or amulets." );
        return;
    }

    if (you.berserker)
    {
        canned_msg(MSG_TOO_BERSERK);
        return;
    }

    if (you.equip[EQ_GLOVES] != -1
        && item_cursed( you.inv[you.equip[EQ_GLOVES]] )
        && you.equip[EQ_AMULET] == -1)
    {
        mpr( "You can't take your gloves off to remove your rings!" );
        return;
    }

    int equipn = prompt_invent_item( "Remove which piece of jewellery?",
                                     OBJ_JEWELLERY );

    if (equipn == PROMPT_ABORT)
    {
        canned_msg( MSG_OK );
        return;
    }

    if (you.inv[equipn].base_type != OBJ_JEWELLERY)
    {
        mpr( "That isn't a piece of jewellery." );
        return;
    }

    int hand_used = -1;
    for (int i = EQ_LEFT_RING; i <= EQ_AMULET; i++)
    {
        if (equipn == you.equip[i])
        {
            hand_used = i;
            break;
        }
    }

    if (hand_used == -1)
    {
        mpr( "You aren't wearing that." );
        return;
    }

    if (hand_used != EQ_AMULET)
    {
        if (you.attribute[ATTR_TRANSFORMATION] == TRAN_BLADE_HANDS)
        {
            mpr( "That ring is firmly locked into your blade hand!" );
            return;
        }

        if (you.equip[EQ_GLOVES] != -1
            && item_cursed( you.inv[you.equip[EQ_GLOVES]] ))
        {
            mpr( "You can't take your gloves off to remove your rings!" );
            return;
        }
    }

    const int ring = you.equip[hand_used];

    if (ring == -1)
    {
        mpr( "I don't think you really meant that." );
        return;
    }

    if (item_cursed( you.inv[ring] ))
    {
        mpr( "It's stuck to you!" );
        set_ident_flags( you.inv[ring], ISFLAG_KNOW_CURSE );
        return;
    }

    mpr( "You remove %s.", in_name( you.equip[hand_used],
            DESC_NOCAP_YOUR, str_pass ) );

    unwear_ring( ring );
    you.equip[hand_used] = -1;
    calc_mp_max();  // must occur after ring is removed -- bwr

    you.turn_is_over = true;
}                               // end remove_ring()

void zap_wand( void )
{
    struct bolt beam;
    struct dist wand;
    int item_slot;
    char str_pass[ITEMNAME_SIZE];

    // Unless the character knows the type of the wand, the targeting
    // system will default to cycling through all monsters. -- bwr
    int targ_mode = TARG_ANY;

    beam.obvious_effect = false;

    if (inv_count() < 1)
    {
        canned_msg(MSG_NOTHING_CARRIED);
        return;
    }

    if (you.berserker)
    {
        canned_msg(MSG_TOO_BERSERK);
        return;
    }

    item_slot = prompt_invent_item( "Zap which item?", OBJ_WANDS );
    if (item_slot == PROMPT_ABORT)
    {
        canned_msg( MSG_OK );
        return;
    }

    if (you.inv[item_slot].base_type != OBJ_WANDS
        || you.inv[item_slot].plus < 1)
    {
        canned_msg( MSG_NOTHING_HAPPENS );
        you.turn_is_over = true;
        return;
    }

    if (item_ident( you.inv[item_slot], ISFLAG_KNOW_TYPE ))
    {
        if (you.inv[item_slot].sub_type == WAND_HASTING
            || you.inv[item_slot].sub_type == WAND_HEALING
            || you.inv[item_slot].sub_type == WAND_INVISIBILITY)
        {
            targ_mode = TARG_FRIEND;
        }
        else
        {
            targ_mode = TARG_ENEMY;
        }
    }

    mpr( MSGCH_PROMPT, STD_DIRECTION_PROMPT );
    message_current_target();

    direction( wand, DIR_NONE, targ_mode );

    if (!wand.isValid)
    {
        if (wand.isCancel)
            canned_msg(MSG_OK);
        return;
    }

    if (you.conf)
    {
        wand.tx = you.x_pos + random2(13) - 6;
        wand.ty = you.y_pos + random2(13) - 6;
    }

    // blargh! blech! this is just begging to be a problem ...
    // not to mention work-around after work-around as wands are
    // added, removed, or altered {dlb}:
    char type_zapped = you.inv[item_slot].sub_type;

    if (type_zapped == WAND_ENSLAVEMENT)
        type_zapped = ZAP_ENSLAVEMENT;

    if (type_zapped == WAND_DRAINING)
        type_zapped = ZAP_NEGATIVE_ENERGY;

    if (type_zapped == WAND_DISINTEGRATION)
        type_zapped = ZAP_DISINTEGRATION;

    if (type_zapped == WAND_RANDOM_EFFECTS)
    {
        type_zapped = random2(16);
        if (one_chance_in(20))
            type_zapped = ZAP_NEGATIVE_ENERGY;
        if (one_chance_in(17))
            type_zapped = ZAP_ENSLAVEMENT;
    }

    beam.source_x = you.x_pos;
    beam.source_y = you.y_pos;
    beam.target_x = wand.tx;
    beam.target_y = wand.ty;

    zapping( type_zapped, 30 + roll_dice(2, you.skills[SK_EVOCATIONS]), beam );

    if (beam.obvious_effect == 1 || you.inv[item_slot].sub_type == WAND_FIREBALL)
    {
        if (get_ident_type( you.inv[item_slot].base_type,
                            you.inv[item_slot].sub_type ) != ID_KNOWN_TYPE)
        {
            set_ident_type( you.inv[item_slot].base_type,
                            you.inv[item_slot].sub_type, ID_KNOWN_TYPE );

            in_name(item_slot, DESC_INVENTORY_EQUIP, str_pass);
            mpr( str_pass );

            // update if wielding
            if (you.equip[EQ_WEAPON] == item_slot)
                set_redraw_status( REDRAW_WIELD );
        }
    }
    else
    {
        set_ident_type( you.inv[item_slot].base_type,
                        you.inv[item_slot].sub_type, ID_TRIED_TYPE );
    }

    // wand can be at zero at this point if it was curse/drained
    if (you.inv[item_slot].plus > 0)
    {
        you.inv[item_slot].plus--;

        if (get_ident_type( you.inv[item_slot].base_type,
                            you.inv[item_slot].sub_type ) == ID_KNOWN_TYPE
            && (item_ident( you.inv[item_slot], ISFLAG_KNOW_PLUSES )
                || you.skills[SK_EVOCATIONS] > 5 + random2(15)))
        {
            if (!item_ident( you.inv[item_slot], ISFLAG_KNOW_PLUSES ))
            {
                mpr("Your skill with magical items lets you calculate the power of this device...");
            }

            mpr( "This wand has %d charge%s left.",
                 you.inv[item_slot].plus,
                 (you.inv[item_slot].plus == 1) ? "" : "s" );

            set_ident_flags( you.inv[item_slot], ISFLAG_KNOW_PLUSES );

            // update if wielding
            if (you.equip[EQ_WEAPON] == item_slot)
                set_redraw_status( REDRAW_WIELD );
        }
    }
    else
    {
        // this is possible with mummy death curse
        mpr( "The wand crumbles to dust." );
        dec_inv_item_quantity( item_slot, 1 );
    }

    exercise( SK_EVOCATIONS, 1 );
    alert_nearby_monsters();

    you.turn_is_over = true;
}                               // end zap_wand()

void drink(void)
{
    int item_slot;

    if (you.is_undead == US_UNDEAD)
    {
        mpr("You can't drink.");
        return;
    }

    if (grd[you.x_pos][you.y_pos] == DNGN_BLUE_FOUNTAIN
        || grd[you.x_pos][you.y_pos] == DNGN_SPARKLING_FOUNTAIN)
    {
        if (drink_fountain())
            return;
    }

    if (inv_count() < 1)
    {
        canned_msg(MSG_NOTHING_CARRIED);
        return;
    }

    if (you.berserker)
    {
        canned_msg(MSG_TOO_BERSERK);
        return;
    }

    item_slot = prompt_invent_item( "Drink which item?", OBJ_POTIONS );
    if (item_slot == PROMPT_ABORT)
    {
        canned_msg( MSG_OK );
        return;
    }

    if (you.inv[item_slot].base_type != OBJ_POTIONS)
    {
        mpr("You can't drink that!");
        return;
    }

    if (potion_effect( you.inv[item_slot].sub_type, 40 ))
    {
        set_ident_flags( you.inv[item_slot], ISFLAG_IDENT_MASK );

        set_ident_type( you.inv[item_slot].base_type,
                        you.inv[item_slot].sub_type, ID_KNOWN_TYPE );
    }
    else
    {
        set_ident_type( you.inv[item_slot].base_type,
                        you.inv[item_slot].sub_type, ID_TRIED_TYPE );
    }

    dec_inv_item_quantity( item_slot, 1 );
    you.turn_is_over = true;

    lessen_hunger(40, true);
}                               // end drink()

static bool drink_fountain(void)
{
    bool gone_dry = false;
    int temp_rand;              // for probability determinations {dlb}
    int fountain_effect = POT_WATER;    // for fountain effects {dlb}

    switch (grd[you.x_pos][you.y_pos])
    {
    case DNGN_BLUE_FOUNTAIN:
        if (!yesno("Drink from the fountain?"))
            return false;

        mpr("You drink the pure, clear water.");
        break;

    case DNGN_SPARKLING_FOUNTAIN:
        if (!yesno("Drink from the sparkling fountain?"))
            return false;

        mpr("You drink the sparkling water.");
        break;
    }

    if (grd[you.x_pos][you.y_pos] == DNGN_SPARKLING_FOUNTAIN)
    {
        temp_rand = random2(4500);

        fountain_effect = ((temp_rand > 2399) ? POT_WATER :     // 46.7%
                           (temp_rand > 2183) ? POT_DECAY :     //  4.8%
                           (temp_rand > 2003) ? POT_MUTATION :  //  4.0%
                           (temp_rand > 1823) ? POT_HEALING :   //  4.0%
                           (temp_rand > 1643) ? POT_HEAL_WOUNDS :// 4.0%
                           (temp_rand > 1463) ? POT_SPEED :     //  4.0%
                           (temp_rand > 1283) ? POT_MIGHT :     //  4.0%
                           (temp_rand > 1139) ? POT_DEGENERATION ://3.2%
                           (temp_rand > 1019) ? POT_LEVITATION ://  2.7%
                           (temp_rand > 899) ? POT_POISON :     //  2.7%
                           (temp_rand > 779) ? POT_SLOWING :    //  2.7%
                           (temp_rand > 659) ? POT_PARALYSIS :  //  2.7%
                           (temp_rand > 539) ? POT_CONFUSION :  //  2.7%
                           (temp_rand > 419) ? POT_INVISIBILITY :// 2.7%
                           (temp_rand > 329) ? POT_MAGIC :      //  2.0%
                           (temp_rand > 239) ? POT_RESTORE_ABILITIES ://  2.0%
                           (temp_rand > 149) ? POT_STRONG_POISON ://2.0%
                           (temp_rand > 59) ? POT_BERSERK_RAGE :  //2.0%
                           (temp_rand > 39) ? POT_GAIN_STRENGTH : //0.4%
                           (temp_rand > 19) ? POT_GAIN_DEXTERITY  //0.4%
                                            : POT_GAIN_INTELLIGENCE);//0.4%
    }

    potion_effect(fountain_effect, 100);

    switch (grd[you.x_pos][you.y_pos])
    {
    case DNGN_BLUE_FOUNTAIN:
        if (one_chance_in(20))
            gone_dry = true;
        break;

    case DNGN_SPARKLING_FOUNTAIN:
        if (one_chance_in(10))
        {
            gone_dry = true;
            break;
        }
        else
        {
            temp_rand = random2(50);

            // you won't know it (yet)
            if (temp_rand > 40) // 18% probability
                grd[you.x_pos][you.y_pos] = DNGN_BLUE_FOUNTAIN;
        }
        break;
    }

    if (gone_dry)
    {
        mpr("The fountain dries up!");
        if (grd[you.x_pos][you.y_pos] == DNGN_BLUE_FOUNTAIN)
            grd[you.x_pos][you.y_pos] = DNGN_DRY_FOUNTAIN_I;
        else if (grd[you.x_pos][you.y_pos] == DNGN_SPARKLING_FOUNTAIN)
            grd[you.x_pos][you.y_pos] = DNGN_DRY_FOUNTAIN_II;
    }

    you.turn_is_over = true;
    return true;
}                               // end drink_fountain()

static bool affix_weapon_enchantment( void )
{
    const int wpn = get_inv_wielded();
    bool success = true;
    char str_pass[ ITEMNAME_SIZE ];

    struct bolt beam;

    if (wpn == -1 || !you.duration[DUR_WEAPON_BRAND])
        return (false);

    in_name( wpn, DESC_CAP_YOUR, str_pass );
    strcpy( info, str_pass );

    switch (get_weapon_brand( you.inv[wpn] ))
    {
    case SPWPN_VORPAL:
        if (get_vorpal_type( you.inv[wpn] ) == DVORP_CRUSHING)
            strcat(info, "'s heaviness feels very stable.");
        else
            strcat(info, "'s sharpness seems more permanent.");

        mpr(info);
        break;

    case SPWPN_FLAMING:
        strcat(info," is engulfed in an explosion of flames!");
        mpr(info);

        beam.type = SYM_BURST;
        beam.damage = dice_def( 3, 10 );
        beam.flavour = 2;
        beam.target_x = you.x_pos;
        beam.target_y = you.y_pos;
        strcpy(beam.name, "fiery explosion");
        beam.colour = RED;
        beam.thrower = KILL_YOU;
        beam.aux_source = "a fiery explosion";
        beam.ex_size = 2;
        beam.is_tracer = false;

        explosion(beam);
        break;

    case SPWPN_FREEZING:
        strcat(info," glows brilliantly blue for a moment.");
        mpr(info);
        cast_refrigeration(60);
        break;

    case SPWPN_DRAINING:
        strcat(info," thirsts for the lives of mortals!");
        mpr(info);
        drain_exp();
        break;

    case SPWPN_VENOM:
        strcat(info, " seems more permanently poisoned.");
        mpr(info);
        cast_toxic_radiance();
        break;

    case SPWPN_DISTORTION:
        strcat(info, " twongs alarmingly.");
        mpr(info);
        miscast_effect(SPTYP_TRANSLOCATION, 9, 90, 100, "a distortion effect");
        break;

    default:
        success = false;
        break;
    }

    if (success)
        you.duration[DUR_WEAPON_BRAND] = 0;

    return (success);
}

bool enchant_weapon( int which_stat, bool quiet )
{
    // Yep, enchant weapon effects require wielding even for the side effect.
    // Tools will require remove curse.
    const int wpn = get_inv_wielded();
    bool affected = true;
    int enchant_level;
    char str_pass[ ITEMNAME_SIZE ];

    if (wpn == -1
        || (you.inv[wpn].base_type != OBJ_WEAPONS
            && you.inv[wpn].base_type != OBJ_MISSILES
            && you.inv[wpn].base_type != OBJ_STAVES))   // uncurse only
    {
        if (!quiet)
            canned_msg( MSG_NOTHING_HAPPENS );

        return (false);
    }

    set_redraw_status( REDRAW_WIELD );

    if (you.inv[wpn].base_type != OBJ_STAVES)
    {
        // missiles only have one stat
        if (you.inv[wpn].base_type == OBJ_MISSILES)
            which_stat = ENCHANT_TO_HIT;

        if (which_stat == ENCHANT_TO_HIT)
            enchant_level = you.inv[wpn].plus;
        else
            enchant_level = you.inv[wpn].plus2;

        // artefacts can't be enchanted, but scrolls still remove curses
        if (you.inv[wpn].base_type == OBJ_WEAPONS
            && (is_fixed_artefact( you.inv[wpn] )
                || is_random_artefact( you.inv[wpn] )))
        {
            affected = false;
        }

        if (enchant_level >= MAX_WPN_ENCHANT / 2
            && random2( MAX_WPN_ENCHANT + 1 ) <= enchant_level)
        {
            affected = false;
        }
    }

    // if it isn't affected by the enchantment, it will still
    // be uncursed:
    if (!affected || you.inv[wpn].base_type == OBJ_STAVES)
    {
        if (item_cursed( you.inv[wpn] ))
        {
            if (!quiet)
            {
                in_name( wpn, DESC_CAP_YOUR, str_pass );
                strcpy( info, str_pass );
                strcat( info, " glows silver for a moment." );
                mpr(info);
            }

            do_uncurse_item( you.inv[wpn] );

            return (true);
        }
        else
        {
            if (!quiet)
                canned_msg( MSG_NOTHING_HAPPENS );

            return (false);
        }
    }

    // vVvVv    This is *here* (as opposed to lower down) for a reason!
    in_name( wpn, DESC_CAP_YOUR, str_pass );
    strcpy( info, str_pass );

    do_uncurse_item( you.inv[wpn] );

    if (you.inv[ wpn ].base_type == OBJ_WEAPONS)
    {
        if (which_stat == ENCHANT_TO_DAM)
        {
            you.inv[ wpn ].plus2++;

            if (!quiet)
            {
                strcat(info, " glows red for a moment.");
                mpr(info);
            }
        }
        else if (which_stat == ENCHANT_TO_HIT)
        {
            you.inv[ wpn ].plus++;

            if (!quiet)
            {
                strcat(info, " glows green for a moment.");
                mpr(info);
            }
        }
    }
    else if (you.inv[ wpn ].base_type == OBJ_MISSILES)
    {
        strcat( info, (you.inv[ wpn ].quantity > 1) ? " glow"
                                                    : " glows" );

        strcat(info, " red for a moment.");

        you.inv[ wpn ].plus++;
    }

    return (true);
}

static bool enchant_armour( void )
{
    // NOTE: It is assumed that armour which changes in this way does
    // not change into a form of armour with a different evasion modifier.
    char str_pass[ ITEMNAME_SIZE ];
    int nthing = you.equip[EQ_BODY_ARMOUR];

    if (nthing != -1 && armour_is_hide( you.inv[nthing] ))
    {
        in_name( you.equip[EQ_BODY_ARMOUR], DESC_CAP_YOUR, str_pass );
        strcpy(info, str_pass);
        strcat(info, " glows purple and changes!");
        mpr(info);

        set_redraw_status( REDRAW_ARMOUR_CLASS );

        hide2armour( you.inv[nthing] );
        return (true);
    }

    // pick random piece of armour
    int count = 0;
    int affected_slot = EQ_WEAPON;

    for (int i = EQ_CLOAK; i <= EQ_BODY_ARMOUR; i++)
    {
        if (you.equip[i] != -1)
        {
            count++;
            if (one_chance_in( count ))
                affected_slot = i;
        }
    }

    // no armour == no enchantment
    if (affected_slot == EQ_WEAPON)
    {
        canned_msg( MSG_NOTHING_HAPPENS );
        return (false);
    }

    bool affected = true;
    item_def &item = you.inv[you.equip[ affected_slot ]];

    const int max_plus = armour_max_enchant( item );

    if (is_random_artefact( item )
        || item.plus >= max_plus    // any armour
        || (max_plus > 4            // ie armour that can support larger pluses
            && item.plus >= max_plus / 2
            && random2( max_plus + 1 ) <= item.plus))
    {
        affected = false;
    }

    // even if not affected, it may be uncursed.
    if (!affected)
    {
        if (item_cursed( item ))
        {
            item_name( item, DESC_CAP_YOUR, str_pass );
            strcpy( info, str_pass );
            strcat( info, " glows silver for a moment." );
            mpr(info);

            do_uncurse_item( item );
            return (true);
        }
        else
        {
            canned_msg( MSG_NOTHING_HAPPENS );
            return (false);
        }
    }

    // vVvVv    This is *here* for a reason!
    item_name( item, DESC_CAP_YOUR, str_pass );
    strcpy( info, str_pass );
    strcat( info, " glows green for a moment." );
    mpr( info );

    item.plus++;

    do_uncurse_item( item );
    set_redraw_status( REDRAW_ARMOUR_CLASS );
    return (true);
}

static void handle_read_book( int item_slot )
{
    int spell, spell_index, nthing;

    if (you.inv[item_slot].sub_type == BOOK_DESTRUCTION)
    {
        if (silenced(you.x_pos, you.y_pos))
        {
            mpr("This book does not work if you cannot read it aloud!");
            return;
        }

        tome_of_power(item_slot);
        return;
    }
    else if (you.inv[item_slot].sub_type == BOOK_MANUAL)
    {
        skill_manual(item_slot);
        return;
    }

    // else Spellbook
    spell = read_book( you.inv[item_slot], RBOOK_READ_SPELL );

    for (;;)
    {
        if (spell < 'a' || spell > 'h')     //jmf: was 'g', but 8=h
            break;

        spell_index = letter_to_index( spell );

        nthing = which_spell_in_book(you.inv[item_slot].sub_type, spell_index);
        if (nthing == SPELL_NO_SPELL)
            break;

        spell = describe_spell( nthing );
    }

    redraw_screen();

    mesclr( true );
    return;
}

void read_scroll(void)
{
    int affected = 0;
    int i;
    int count;
    int nthing;
    int brand;
    struct bolt beam;
    char str_pass[ ITEMNAME_SIZE ];

    // added: scroll effects are never tracers.
    beam.is_tracer = false;

    if (you.berserker)
    {
        canned_msg(MSG_TOO_BERSERK);
        return;
    }

    if (inv_count() < 1)
    {
        canned_msg(MSG_NOTHING_CARRIED);
        return;
    }

    int item_slot = prompt_invent_item( "Read which item?", OBJ_SCROLLS );
    if (item_slot == PROMPT_ABORT)
    {
        canned_msg( MSG_OK );
        return;
    }

    if (you.inv[item_slot].base_type != OBJ_BOOKS
        && you.inv[item_slot].base_type != OBJ_SCROLLS)
    {
        mpr("You can't read that!");
        return;
    }

    // here we try to read a book {dlb}:
    if (you.inv[item_slot].base_type == OBJ_BOOKS)
    {
        handle_read_book( item_slot );
        return;
    }

    if (silenced(you.x_pos, you.y_pos))
    {
        mpr("Magic scrolls do not work when you're silenced!");
        return;
    }

    // ok - now we FINALLY get to read a scroll !!! {dlb}
    you.turn_is_over = true;

    // imperfect vision prevents players from reading actual content {dlb}:
    if (you.mutation[MUT_BLURRY_VISION] > 0
        && random2(5) < you.mutation[MUT_BLURRY_VISION])
    {
        mpr( (you.mutation[MUT_BLURRY_VISION] == 3 && one_chance_in(3))
                        ? "This scroll appears to be blank."
                        : "The writing blurs in front of your eyes." );
        return;
    }

    // decrement and handle inventory if any scroll other than paper {dlb}:
    const int scroll_type = you.inv[item_slot].sub_type;
    if (scroll_type != SCR_PAPER)
    {
        mpr("As you read the scroll, it crumbles to dust.");
        // Actual removal of scroll done afterwards. -- bwr
    }

    // scrolls of paper are also exempted from this handling {dlb}:
    if (scroll_type != SCR_PAPER)
    {
        if (you.conf)
        {
            random_uselessness( item_slot );
            return;
        }

        if (!you.skills[SK_SPELLCASTING])
            exercise( SK_SPELLCASTING, (coinflip() ? 2 : 1) );
    }

    bool id_the_scroll = true;  // to prevent unnecessary repetition

    // it is the exception, not the rule, that
    // the scroll will not be identified {dlb}:
    switch (scroll_type)
    {
    case SCR_PAPER:
        // remember paper scrolls handled as special case above, too:
        mpr("This scroll appears to be blank.");
        break;

    case SCR_RANDOM_USELESSNESS:
        random_uselessness( item_slot );
        id_the_scroll = false;
        break;

    case SCR_BLINKING:
        blink();
        break;

    case SCR_TELEPORTATION:
        you_teleport();
        break;

    case SCR_REMOVE_CURSE:
        if (!remove_curse(false))
            id_the_scroll = false;
        break;

    case SCR_DETECT_CURSE:
        if (!detect_curse(false))
            id_the_scroll = false;
        break;

    case SCR_ACQUIREMENT:
        acquirement(OBJ_RANDOM);
        break;

    case SCR_FEAR:
        if (!mass_enchantment( ENCH_FEAR, 1000, MHITYOU,
                   (get_ident_type( OBJ_SCROLLS, SCR_FEAR ) == ID_KNOWN_TYPE)))
        {
            id_the_scroll = false;
        }
        break;

    case SCR_NOISE:
        if (!noisy( SL_NOISE_SCROLL, you.x_pos, you.y_pos,
                    "You hear a loud clanging noise!"))
        {
            id_the_scroll = false;
        }
        break;

    case SCR_SUMMONING:
        if (create_monster( MONS_ABOMINATION_SMALL, BEH_FRIENDLY, 6 ) != -1)
            mpr("A horrible Thing appears!");
        break;

    case SCR_FORGETFULNESS:
        canned_msg( MSG_DISORIENTED );
        if (!player_mental_clarity())
            forget_map( 50 + random2(50) );
        break;

    case SCR_MAGIC_MAPPING:
        if (you.level_type == LEVEL_LABYRINTH
            || you.level_type == LEVEL_ABYSS)
        {
            canned_msg( MSG_DISORIENTED );
            id_the_scroll = false;
        }
        else
        {
            mpr("You feel aware of your surroundings.");
            magic_mapping( 50, 90 + random2(10) );
        }
        break;

    case SCR_TORMENT:
        torment( you.x_pos, you.y_pos );
        did_god_conduct( DID_NECROMANCY, 5 );
        break;

    case SCR_IMMOLATION:
        mpr("The scroll explodes in your hands!");

        beam.type = SYM_BURST;
        beam.damage = dice_def( 3, 10 );
        beam.flavour = BEAM_FIRE;
        beam.is_explosion = true;
        beam.target_x = you.x_pos;
        beam.target_y = you.y_pos;
        strcpy(beam.name, "fiery explosion");
        beam.colour = RED;
        // your explosion, (not someone else's explosion)
        beam.thrower = KILL_YOU;
        beam.aux_source = "reading a scroll of immolation";
        beam.ex_size = 2;

        explosion(beam);
        break;

    case SCR_IDENTIFY:
        set_ident_flags( you.inv[item_slot], ISFLAG_IDENT_MASK );

        // important {dlb}
        set_ident_type( OBJ_SCROLLS, SCR_IDENTIFY, ID_KNOWN_TYPE );

        identify(-1);
        set_redraw_status( REDRAW_WIELD );
        break;

    case SCR_CURSE_WEAPON:
        // These work on tools for the sake of evil.
        nthing = get_inv_hand_tool();

        if (nthing == -1
            || (you.inv[nthing].base_type != OBJ_WEAPONS
                && you.inv[nthing].base_type != OBJ_STAVES)
            || item_cursed( you.inv[nthing] ))
        {
            canned_msg( MSG_NOTHING_HAPPENS );
            id_the_scroll = false;
        }
        else
        {
            in_name( nthing, DESC_CAP_YOUR, str_pass );
            strcpy(info, str_pass);
            strcat(info, " glows black for a moment.");
            mpr(info);

            do_curse_item( you.inv[nthing] );
            set_redraw_status( REDRAW_WIELD );
        }
        break;

    case SCR_ENCHANT_WEAPON_I:
        id_the_scroll = enchant_weapon( ENCHANT_TO_HIT );
        break;

    case SCR_ENCHANT_WEAPON_II:
        id_the_scroll = enchant_weapon( ENCHANT_TO_DAM );
        break;

    case SCR_ENCHANT_WEAPON_III:
        nthing = get_inv_wielded();

        if (nthing != -1
            && (you.inv[nthing].base_type == OBJ_WEAPONS
                || you.inv[nthing].base_type == OBJ_MISSILES
                || you.inv[nthing].base_type == OBJ_STAVES))
        {
            if (!affix_weapon_enchantment())
            {
                bool work = false;
                bool uncurse = false;

                // we want the original name for messages:
                in_name( nthing, DESC_CAP_YOUR, str_pass );

                if (item_cursed( you.inv[nthing] ))
                {
                    uncurse = true;
                    do_uncurse_item( you.inv[nthing] );
                }

                work = enchant_weapon( ENCHANT_TO_HIT, true );

                if (coinflip())
                    work = enchant_weapon( ENCHANT_TO_HIT, true ) || work;

                work = enchant_weapon( ENCHANT_TO_DAM, true ) || work;

                if (coinflip())
                    work = enchant_weapon( ENCHANT_TO_DAM, true ) || work;

                if (work || uncurse)
                {
                    mpr( "%s glows bright %s for a while.",
                          str_pass, (work) ? "yellow" : "silver" );
                }
                else
                {
                    canned_msg( MSG_NOTHING_HAPPENS );
                    id_the_scroll = false;
                }
            }
        }
        else
        {
            canned_msg( MSG_NOTHING_HAPPENS );
            id_the_scroll = false;
        }
        break;

    case SCR_VORPALISE_WEAPON:
        nthing = get_inv_wielded();

        if (nthing == -1
            || you.inv[nthing].base_type != OBJ_WEAPONS
            || (you.inv[nthing].base_type == OBJ_WEAPONS
                && (is_fixed_artefact( you.inv[nthing] )
                    || is_unrandom_artefact( you.inv[nthing] )
                    || you.inv[nthing].sub_type == WPN_BLOWGUN // no dam base
                    || (is_random_artefact( you.inv[nthing] )
                        && !one_chance_in(3)))))
        {
            canned_msg( MSG_NOTHING_HAPPENS );
            id_the_scroll = false;
            break;
        }

        in_name( nthing, DESC_CAP_YOUR, str_pass );
        strcpy( info, str_pass );
        strcat( info, " emits a brilliant flash of light!" );
        mpr( info );

        alert_nearby_monsters();
        set_redraw_status( REDRAW_WIELD );

        brand = get_weapon_brand( you.inv[nthing] );

        // order is important here:
        if (is_random_artefact( you.inv[nthing] ))
        {
            unuse_randart( nthing );
            unmake_item_randart( you.inv[nthing] );
            set_item_ego_type( you.inv[nthing], OBJ_WEAPONS, SPWPN_NORMAL );
            you.inv[nthing].plus = 0;
            you.inv[nthing].plus2 = 0;
        }
        else if (brand == SPWPN_VORPAL)
        {
            if (you.duration[DUR_WEAPON_BRAND])
                affix_weapon_enchantment();
            else
            {
                enchant_weapon( ENCHANT_TO_HIT, true );
                enchant_weapon( ENCHANT_TO_DAM, true );
            }
        }
        else if (brand != SPWPN_NORMAL)
        {
            set_item_ego_type( you.inv[nthing], OBJ_WEAPONS, SPWPN_NORMAL );
            you.inv[nthing].plus = 0;
            you.inv[nthing].plus2 = 0;
        }
        else
        {
            set_item_ego_type( you.inv[nthing], OBJ_WEAPONS, SPWPN_VORPAL );
        }
        break;

    case SCR_RECHARGING:
        nthing = get_inv_wielded(); // only used for elec weapons

        if (nthing != -1
            && !is_random_artefact( you.inv[nthing] )
            && !is_fixed_artefact( you.inv[nthing] )
            && get_weapon_brand( you.inv[nthing] ) == SPWPN_ELECTROCUTION)
        {
            id_the_scroll = !enchant_weapon( ENCHANT_TO_DAM );
            break;
        }

        if (!recharge_wand())
        {
            canned_msg( MSG_NOTHING_HAPPENS );
            id_the_scroll = false;
        }
        break;

    case SCR_ENCHANT_ARMOUR:
        id_the_scroll = enchant_armour();
        break;

    case SCR_CURSE_ARMOUR:
        // make sure there's something to curse first
        count = 0;
        affected = EQ_WEAPON;
        for (i = EQ_CLOAK; i <= EQ_BODY_ARMOUR; i++)
        {
            if (you.equip[i] != -1 && !item_cursed( you.inv[you.equip[i]] ))
            {
                count++;
                if (one_chance_in( count ))
                    affected = i;
            }
        }

        if (affected == EQ_WEAPON)
        {
            canned_msg( MSG_NOTHING_HAPPENS );
            id_the_scroll = false;
            break;
        }

        // make the name _before_ we curse it
        in_name( you.equip[affected], DESC_CAP_YOUR, str_pass );
        do_curse_item( you.inv[you.equip[affected]] );

        strcpy(info, str_pass);
        strcat(info, " glows black for a moment.");
        mpr(info);
        break;
    }                           // end switch

    bool last = false;

    // finally, destroy and identify the scroll
    if (scroll_type != SCR_PAPER)
        last = dec_inv_item_quantity( item_slot, 1 );

    if (scroll_type == SCR_RANDOM_USELESSNESS)
    {
        // property of a ?oRU is that a similar one has been tried
        if (!last)
            set_ident_flags( you.inv[item_slot], ISFLAG_KNOW_PROPERTIES );
    }
    else
    {
        set_ident_type( OBJ_SCROLLS, scroll_type,
                        (id_the_scroll) ? ID_KNOWN_TYPE : ID_TRIED_TYPE );
    }

}                               // end read_scroll()

void original_name(void)
{
    int item_slot = prompt_invent_item( "Examine which item?", -1 );
    if (item_slot == PROMPT_ABORT)
    {
        canned_msg( MSG_OK );
        return;
    }

    describe_item( you.inv[item_slot] );
    redraw_screen();
}                               // end original_name()

void use_randart( int it )
{
    ASSERT( it != -1 && is_random_artefact( you.inv[it] ) );

    if (you.inv[it].ra_props[RAP_AC])
        set_redraw_status( REDRAW_ARMOUR_CLASS );

    if (you.inv[it].ra_props[RAP_EVASION])
        set_redraw_status( REDRAW_EVASION );

    // modify ability scores
    modify_stat( STAT_STRENGTH,     you.inv[it].ra_props[RAP_STRENGTH],     true );
    modify_stat( STAT_INTELLIGENCE, you.inv[it].ra_props[RAP_INTELLIGENCE], true );
    modify_stat( STAT_DEXTERITY,    you.inv[it].ra_props[RAP_DEXTERITY],    true );

    if (you.inv[it].ra_props[RAP_NOISES])
        you.special_wield = you.inv[it].ra_props[RAP_NOISES];
}

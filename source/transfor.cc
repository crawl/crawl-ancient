/*
 *  File:       transfor.cc
 *  Summary:    Misc function related to player transformations.
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 * <2>  5/26/99  JDJ  transform() and untransform() set you.wield_change so
 *                    the weapon line updates.
 * <1> -/--/--   LRH  Created
 */

#include "AppHdr.h"
#include "transfor.h"

#include <stdio.h>
#include <string.h>

#include "globals.h"
#include "externs.h"

#include "effects.h"
#include "item_use.h"
#include "it_use2.h"
#include "itemname.h"
#include "itemprop.h"
#include "items.h"
#include "misc.h"
#include "player.h"
#include "skills2.h"
#include "stuff.h"

// XXX: add resistances and other properties from player.cc and abl-show.cc?
struct transform_def
{
    char                        sym;
    int                         colour;
    int                         base_dur;
    int                         max_dur;
    bool                        allow_extend;
    bool                        allow_untrans;

    int                         hp_mult;
    int                         str_plus;
    int                         int_plus;
    int                         dex_plus;
    size_type                   size;   // SIZE_CHARACTER marks non-shapchanges

    int                         hand_type;
    brand_type                  hand_brand;
    int                         hand_damage;
    int                         hand_to_hit;
    int                         hand_speed; // note: statue, dragons = x1.5

    int                         res_fire;
    int                         res_cold;
    int                         res_elec;
    int                         res_pois;
    int                         prot_life;
    int                         res_magic; // 0: apply ench, neg: reduce innate
    int                         metabolism;
    int                         spell_penalty;

    bool                        rem_cursed;
    unsigned int                rem_stuff;
};

// NOTE: as implied about a size of SIZE_CHARACTER is currently
// used to mark transformations that don't cause a major change
// to appearance... proper shapchanges all go to a specific size
// regardless of the characters original size (so a Spriggan
// spiderform and an Ogre-Mage one are the same size).  Consult
// player_is_shapechanged() is you want to change this.
static transform_def Trans[ NUM_TRANSFORMATIONS ] =
{
    //  chr  colour     dur  max extend untran
    //  hp   str int dex size
    //  hand_type        brand        dam hit spd
    //  fire cold elec pois life MR meta spl
    //  removes: cursed, types

    { // No tranfromation:
        '@', LIGHTGREY,  0,   0, false, false,
        10,   0,  0,  0, SIZE_CHARACTER,
        DAM_BLUDGEON, SPWPN_NORMAL,    3,  3, 10,
        0,  0,  0,  0,  0,  0,  0,  0,
        false, RM_NONE
    },

    { // Spider form:
        's', BROWN,     10,  30,  true,  true,
        10,   0,  0,  5, SIZE_TINY,
        DAM_PIERCE,   SPWPN_VENOM,     6,  3, 10,
        0,  0,  0,  0,  0, -2,  0, 15,
        false, RM_ALL
    },

    { // Blade hands:
        '@', LIGHTGREY, 10,  30,  true,  true,
        10,   0,  0,  0, SIZE_CHARACTER,
        DAM_SLICE,    SPWPN_NORMAL,    5,  6, 12,
        0,  0,  0,  0,  0,  0,  0, 25,
        false, RMFLAG(EQ_WEAPON) | RMFLAG(EQ_GLOVES) | RMFLAG(EQ_SHIELD)
    },

    { // Statue form:
        '8', LIGHTGREY, 20,  50,  true,  true,
        15,   3,  0, -3, SIZE_CHARACTER,
        DAM_BLUDGEON, SPWPN_NORMAL,   12, -3, 10,
        0,  0,  1,  3,  1,  3, -3,  0,
        true, RM_ALL_BUT( RMFLAG(EQ_WEAPON) | RMFLAG(EQ_CLOAK) | RMFLAG(EQ_HELMET) )
    },

    { // Ice form:
        'I', WHITE,     30,  75,  true,  true,
        12,   0,  0,  0, SIZE_LARGE,
        DAM_PIERCE,   SPWPN_FREEZING, 12,  5, 12,
        -1,  3,  0,  3,  0,  2,  0,  0,
        true, RM_ALL_BUT( RMFLAG(EQ_CLOAK) )
    },

    { // Dragon form:
        'D', GREEN,     20,  90,  true,  true,
        16,  10,  0,  0, SIZE_HUGE,
        DAM_SLICE,    SPWPN_NORMAL,   18, -5, 16,
        2, -1,  0,  2,  0,  3,  3,  0,
        true, RM_ALL | RM_RINGS
    },

    { // Necromutation/Lich form
      // Also: undead giving no food/potions, torment res, mutation res
      // Also: necromancy booster
        'L', LIGHTGREY, 20,  90, false, false,
        10,   0,  0,  0, SIZE_CHARACTER,
        DAM_BLUDGEON, SPWPN_DRAINING,  3,  3, 10,
        0,  0,  0,  3,  3,  4,  0,  0,
        true, RM_NONE
    },

    { // Serpent of Hell (unused):
        'S', RED,       20,  90,  true,  true,
        17,  13,  0,  0, SIZE_HUGE,
        DAM_SLICE,    SPWPN_NORMAL,   20, -8, 16,
        2,  0,  0,  2,  2,  5,  3,  0,
        true, RM_ALL | RM_RINGS
    },

    { // Airwalk (unused):
        '#', DARKGREY,  20,  90, false, false,
        10,   0,    0,  8, SIZE_MEDIUM,
        DAM_BASH,     SPWPN_NORMAL, -100, -100, 20,
        -2, -2,  1,  3,  0,  5,  5,  0,
        true, RM_EVERYTHING
    },
};

static void drop_everything(void);

static bool must_remove( unsigned int rem_stuff, int eq_slot )
{
    return (rem_stuff & RMFLAG(eq_slot));
}

static void set_remove( unsigned int &rem_stuff, int eq_slot )
{
    rem_stuff |= RMFLAG(eq_slot);
}

bool remove_equipment( unsigned int rem )
{
    char str_pass[ ITEMNAME_SIZE ];

    // if we're removing body armour, the cloak will come off as well -- bwr
    if (must_remove( rem, EQ_BODY_ARMOUR ) && you.equip[EQ_BODY_ARMOUR] != -1)
        set_remove( rem, EQ_CLOAK );

    // if we're removing gloves, the weapon will come off as well -- bwr
    if (must_remove( rem, EQ_GLOVES ) && you.equip[EQ_GLOVES] != -1)
        set_remove( rem, EQ_WEAPON );

    if (must_remove( rem, EQ_WEAPON ) && you.equip[EQ_WEAPON] != -1)
        unwield_item();

    for (int i = EQ_CLOAK; i < NUM_EQUIP; i++)
    {
        if (!must_remove( rem, i ) || you.equip[i] == -1)
            continue;

        mpr( "%s falls %s.",
              in_name( you.equip[i], DESC_CAP_YOUR, str_pass ),
              (i < EQ_LEFT_RING) ? "away" : "off" );

        if (i < EQ_LEFT_RING)
            unwear_armour( you.equip[i] );
        else
            unwear_ring( you.equip[i] );

        you.equip[i] = -1;
    }

    if (rem == RM_EVERYTHING)
        drop_everything();

    return (true);
}                               // end remove_equipment()

// Returns true if any piece of equipment that has to be removed is cursed.
// Useful for keeping low level transformations from being too useful.
static bool check_for_cursed_equipment( unsigned int rem )
{
    for (int i = EQ_WEAPON; i < NUM_EQUIP; i++)
    {
        if (!must_remove( rem, i ) || you.equip[i] == -1)
            continue;

        if ((i != EQ_WEAPON || player_tool_usable())
            && item_cursed( you.inv[you.equip[i]] ))
        {
            mpr( "Your cursed equipment won't allow you to complete the transformation." );

            return (true);
        }
    }

    return (false);
}                               // end check_for_cursed_equipment()

bool transform( int pow, int form )
{
    if (you.species == SP_MERFOLK && player_in_water()
        && form != TRAN_DRAGON)
    {
        // This might by overkill, but it's okay because obviously
        // whatever magical ability that let's them walk on land is
        // removed when they're in water (in this case, their natural
        // form is completely over-riding any other... goes well with
        // the forced transform when entering water)... but merfolk can
        // transform into dragons, because dragons fly. -- bwr
        mpr("You cannot transform out of your normal form while in water.");
        return (false);
    }

    // This must occur before the untransform() and the is_undead check.
    if (you.attribute[ATTR_TRANSFORMATION] == form)
    {
        if (Trans[form].allow_extend
            && you.duration[DUR_TRANSFORMATION] < Trans[form].max_dur)
        {
            mpr( "You extend your transformation's duration." );
            you.duration[DUR_TRANSFORMATION] += random2(pow);

            if (you.duration[DUR_TRANSFORMATION] > Trans[form].max_dur)
                you.duration[DUR_TRANSFORMATION] = Trans[form].max_dur;

            return (true);
        }
        else
        {
            mpr( "You cannot extend your transformation any further!" );
            return (false);
        }
    }

    if (you.attribute[ATTR_TRANSFORMATION] != TRAN_NONE)
    {
        if (Trans[form].allow_untrans)
        {
            untransform();
            contaminate_player(1);
        }
        else
        {
            mpr( "You are stuck in your current form for now." );
            return (false);
        }
    }

    if (you.is_undead)
    {
        mpr("Your unliving flesh cannot be transformed in this way.");
        return (false);
    }

    //jmf: silently discard this enchantment
    you.duration[DUR_STONESKIN] = 0;

    set_redraw_status( REDRAW_EVASION );
    set_redraw_status( REDRAW_ARMOUR_CLASS );
    set_redraw_status( REDRAW_WIELD );

    if (!Trans[form].rem_cursed
        && check_for_cursed_equipment( Trans[form].rem_stuff ))
    {
        return (false);
    }

    switch (form)
    {
    case TRAN_SPIDER:           // also AC +3, ev +3, fast_run
        mpr("You turn into a venomous arachnid creature.");
        break;

    case TRAN_ICE_BEAST:  // also AC +3, cold +3, fire -1, pois +1
        mpr( "You turn into a creature of crystalline ice." );

        if (you.duration[DUR_ICY_ARMOUR])
            mpr( "Your new body merges with your icy armour." );
        break;

    case TRAN_BLADE_HANDS:
        mpr("Your hands turn into razor-sharp scythe blades.");
        break;

    case TRAN_STATUE: // also AC +20, ev -5, elec +1, pois +1, neg +1, slow
        if (you.species == SP_GNOME && coinflip())
            mpr( "Look, a garden gnome.  How cute!" );
        else if (player_genus(GENPC_DWARVEN) && one_chance_in(10))
            mpr( "You inwardly fear your resemblance to a lawn ornament." );
        else
            mpr( "You turn into a living statue of rough stone." );

        if (you.duration[DUR_STONEMAIL] || you.duration[DUR_STONESKIN])
            mpr( "Your new body merges with your stone armour." );
        break;

    case TRAN_DRAGON:  // also AC +10, ev -3, cold -1, fire +2, pois +1, flight
        if (you.species == SP_MERFOLK && player_in_water())
            mpr("You fly out of the water as you turn into a fearsome dragon!");
        else
            mpr("You turn into a fearsome dragon!");
        break;

    case TRAN_LICH:
        // also AC +3, cold +1, neg +3, pois +1, is_undead, res magic +50,
        // spec_death +1, and drain attack (if empty-handed)
        if (you.deaths_door)
        {
            mpr( "The transformation conflicts with an enchantment already in effect." );

            return (false);
        }

        mpr("Your body is suffused with negative energy!");

        // undead cannot regenerate -- bwr
        if (you.duration[DUR_REGENERATION])
        {
            mpr( MSGCH_DURATION, "You stop regenerating." );
            you.duration[DUR_REGENERATION] = 0;
        }

        // silently removed since undead automatically resist poison -- bwr
        you.duration[DUR_RESIST_POISON] = 0;

        you.is_undead = US_UNDEAD;
        you.hunger_state = HS_SATIATED;  // no hunger effects while transformed
        break;

    case TRAN_AIR:
        // also AC 20, ev +20, regen/2, no hunger, fire -2, cold -2, air +2,
        // pois +1, spec_earth -1
        mpr( "You feel diffuse..." );
        break;

    case TRAN_SERPENT_OF_HELL:
        // also AC +10, ev -5, fire +2, pois +1, life +2, slow
        mpr( "You transform into a huge demonic serpent!" );
        break;
    }

    remove_equipment( Trans[form].rem_stuff );
    you.attribute[ATTR_TRANSFORMATION] = form;

    you.duration[DUR_TRANSFORMATION] = Trans[form].base_dur + random2(pow)
                                                            + random2(pow);

    if (you.duration[DUR_TRANSFORMATION] > Trans[form].max_dur)
        you.duration[DUR_TRANSFORMATION] = Trans[form].max_dur;

    if (Trans[form].str_plus)
        modify_stat( STAT_STRENGTH, Trans[form].str_plus, true );

    if (Trans[form].int_plus)
        modify_stat( STAT_INTELLIGENCE, Trans[form].int_plus, true );

    if (Trans[form].dex_plus)
        modify_stat( STAT_DEXTERITY, Trans[form].dex_plus, true );

    if (Trans[form].hp_mult != 10)
        calc_hp_max( (you.hp * transform_hp_factor()) / 10 );

    you.symbol = Trans[form].sym;
    you.colour = Trans[form].colour;

    return (true);
}                               // end transform()

void untransform( void )
{
    set_redraw_status( REDRAW_EVASION );
    set_redraw_status( REDRAW_ARMOUR_CLASS );
    set_redraw_status( REDRAW_WIELD );

    you.symbol = Trans[TRAN_NONE].sym;
    you.colour = Trans[TRAN_NONE].colour;

    // must be unset first or else infinite loops might result -- bwr
    const int old_form = you.attribute[ATTR_TRANSFORMATION];
    you.attribute[ATTR_TRANSFORMATION] = TRAN_NONE;
    you.duration[DUR_TRANSFORMATION] = 0;

    switch (old_form)
    {
    case TRAN_SPIDER:
        mpr( MSGCH_DURATION, "Your transformation has ended." );
        break;

    case TRAN_BLADE_HANDS:
        mpr( MSGCH_DURATION, "Your hands revert to their normal proportions." );
        break;

    case TRAN_STATUE:
        mpr( MSGCH_DURATION, "You revert to your normal fleshy form." );

        // Note: if the core goes down, the combined effect soon disappears,
        // but the reverse isn't true. -- bwr
        if (you.duration[DUR_STONEMAIL])
            you.duration[DUR_STONEMAIL] = 1;

        if (you.duration[DUR_STONESKIN])
            you.duration[DUR_STONESKIN] = 1;
        break;

    case TRAN_ICE_BEAST:
        mpr( MSGCH_DURATION, "You warm up again." );

        // Note: if the core goes down, the combined effect soon disappears,
        // but the reverse isn't true. -- bwr
        if (you.duration[DUR_ICY_ARMOUR])
            you.duration[DUR_ICY_ARMOUR] = 1;
        break;

    case TRAN_DRAGON:
        mpr( MSGCH_DURATION, "Your transformation has ended." );

        // re-check terrain now that be may no longer be flying.
        move_player_to_grid( you.x_pos, you.y_pos, false, true, true );
        break;

    case TRAN_LICH:
        mpr( MSGCH_DURATION, "You feel yourself come back to life." );
        you.is_undead = US_ALIVE;
        break;

    case TRAN_AIR:
        mpr( MSGCH_DURATION, "Your body solidifies." );
        break;

    case TRAN_SERPENT_OF_HELL:
        mpr( MSGCH_DURATION, "Your transformation has ended." );
        break;
    }

    if (Trans[old_form].str_plus)
        modify_stat( STAT_STRENGTH, -Trans[old_form].str_plus, true );

    if (Trans[old_form].int_plus)
        modify_stat( STAT_INTELLIGENCE, -Trans[old_form].int_plus, true );

    if (Trans[old_form].dex_plus)
        modify_stat( STAT_DEXTERITY, -Trans[old_form].dex_plus, true );

    unsigned int     rem_stuff = 0;
    const size_type  size = player_size();

    // weapon needs to be removed?
    if (you.equip[EQ_WEAPON] != -1)
    {
        const item_def &wpn = you.inv[ you.equip[EQ_WEAPON] ];

        // Currently, all weapons are capable of being put "in-hand"
        // so we're not using check_weapon_size().
        if ((wpn.base_type == OBJ_WEAPONS || wpn.base_type == OBJ_STAVES)
            && !check_weapon_shape(wpn, true))
        {
            set_remove( rem_stuff, EQ_WEAPON );
        }
    }

    // armour needs to be removed?
    for (int i = EQ_CLOAK; i < NUM_EQUIP; i++)
    {
        if (you.equip[i] == -1)
            continue;

        const item_def &arm = you.inv[ you.equip[i] ];

        if (i < EQ_LEFT_RING)
        {
            // armour checks
            if (check_armour_size( arm, size )
                && check_armour_shape( arm, true ))
            {
                continue;
            }
        }
        else
        {
            // jewellery checks
            if (check_jewellery_size( arm, size ))
                continue;
        }

        set_remove( rem_stuff, i );
    }

    if (rem_stuff)
        remove_equipment( rem_stuff );

    calc_hp_max();
}                               // end untransform()

void merfolk_start_swimming(void)
{
    unsigned int rem_stuff = 0;

    if (you.attribute[ATTR_TRANSFORMATION] != TRAN_NONE)
        untransform();

    // bye, bye feet...
    if (you.equip[EQ_BOOTS] != -1)
        set_remove( rem_stuff, EQ_BOOTS );

    // Perhaps a bit to easy for the player, but we allow merfolk
    // to slide out of heavy body armour freely when entering water,
    // rather than handling emcumbered swimming. -- bwr
    if (!player_light_armour())
    {
        // Can't slide out of just the body armour, cloak comes off -- bwr
        if (you.equip[EQ_CLOAK])
            set_remove( rem_stuff, EQ_CLOAK );

        set_remove( rem_stuff, EQ_BODY_ARMOUR );
    }

    remove_equipment( rem_stuff );
}


// raw comparison of an item, must use check_armour_shape for full version
bool transform_can_equip_type( int eq_slot )
{
    const int form = you.attribute[ATTR_TRANSFORMATION];

    return (!must_remove( Trans[form].rem_stuff, eq_slot ));
}

int transform_hp_factor( void )
{
    return (Trans[ you.attribute[ATTR_TRANSFORMATION] ].hp_mult);
}

int transform_stat_bonus( int stat )
{
    switch (stat)
    {
    case STAT_STRENGTH:
        return (Trans[ you.attribute[ATTR_TRANSFORMATION] ].str_plus);

    case STAT_INTELLIGENCE:
        return (Trans[ you.attribute[ATTR_TRANSFORMATION] ].int_plus);

    case STAT_DEXTERITY:
        return (Trans[ you.attribute[ATTR_TRANSFORMATION] ].dex_plus);

    default:
        break;
    }

    return (0);
}

size_type transform_size( int psize )
{
    UNUSED( psize );

    return (Trans[ you.attribute[ATTR_TRANSFORMATION] ].size);
}

brand_type transform_hand_brand( void )
{
    return (Trans[ you.attribute[ATTR_TRANSFORMATION] ].hand_brand);
}

int transform_hand_damage( void )
{
    int dam = Trans[ you.attribute[ATTR_TRANSFORMATION] ].hand_damage;

    if (!player_is_shapechanged( true ) && you.equip[EQ_GLOVES] == -1)
    {
        if (you.species == SP_TROLL || you.species == SP_GHOUL)
            dam++;

        dam += you.mutation[MUT_CLAWS];
    }

    if (you.attribute[ATTR_TRANSFORMATION] == TRAN_BLADE_HANDS)
        dam += div_rand_round( you.dex,  4 );

    return (dam);
}

int transform_hand_to_hit( void )
{
    return (Trans[ you.attribute[ATTR_TRANSFORMATION] ].hand_to_hit);
}

int transform_hand_speed( void )
{
    return (Trans[ you.attribute[ATTR_TRANSFORMATION] ].hand_speed);
}

int transform_hand_type( void )
{
    return (Trans[ you.attribute[ATTR_TRANSFORMATION] ].hand_type);
}

// Used to mark transformations which override species/mutation intrinsics.
// If phys_scales is true then we're checking to see if the form keeps
// the physical (AC/EV) properties from scales... the special intrinsic
// features (resistances, etc) are lost in those forms however.
bool transform_changed_physiology( bool phys_scales )
{
    return (you.attribute[ATTR_TRANSFORMATION] != TRAN_NONE
            && you.attribute[ATTR_TRANSFORMATION] != TRAN_BLADE_HANDS
            && (!phys_scales
                || (you.attribute[ATTR_TRANSFORMATION] != TRAN_LICH
                    && you.attribute[ATTR_TRANSFORMATION] != TRAN_STATUE)));
}

int transform_res_fire( void )
{
    return (Trans[ you.attribute[ATTR_TRANSFORMATION] ].res_fire);
}

int transform_res_cold( void )
{
    return (Trans[ you.attribute[ATTR_TRANSFORMATION] ].res_cold);
}

int transform_res_elec( void )
{
    return (Trans[ you.attribute[ATTR_TRANSFORMATION] ].res_elec);
}

int transform_res_pois( void )
{
    return (Trans[ you.attribute[ATTR_TRANSFORMATION] ].res_pois);
}

int transform_prot_life( void )
{
    return (Trans[ you.attribute[ATTR_TRANSFORMATION] ].prot_life);
}

int transform_res_magic( void )
{
    return (Trans[ you.attribute[ATTR_TRANSFORMATION] ].res_magic);
}

int transform_metabolism( void )
{
    return (Trans[ you.attribute[ATTR_TRANSFORMATION] ].metabolism);
}

int transform_spell_penalty( void )
{
    return (Trans[ you.attribute[ATTR_TRANSFORMATION] ].spell_penalty);
}

bool transform_can_untransform( void )
{
    return (Trans[ you.attribute[ATTR_TRANSFORMATION] ].allow_untrans);
}

// This isn't really tested... but it might work better than it used
// to now that rings can be stripped off by transformtations.
void drop_everything(void)
{
    int i = 0;

    if (inv_count() < 1)
        return;

    mpr( "You find yourself unable to carry your possessions!" );

    for (i = 0; i < ENDOFPACK; i++)
    {
        if (is_valid_item( you.inv[i] ))
        {
            copy_item_to_grid( you.inv[i], you.x_pos, you.y_pos );
            you.inv[i].quantity = 0;
        }
    }

    return;
}                               // end drop_everything()

/*
 *  File:       it_use2.cc
 *  Summary:    Functions for using wands, potions, and weapon/armour removal.4\3
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 *      26jun2000   jmf   added ZAP_MAGMA
 *  <4> 19mar2000   jmf   Added ZAP_BACKLIGHT and ZAP_SLEEP
 *  <3>     10/1/99         BCR             Changed messages for speed and
 *                                          made amulet resist slow up speed
 *  <2>     5/20/99         BWR             Fixed bug with RAP_METABOLISM
 *                                          and RAP_NOISES artefacts/
 *  <1>     -/--/--         LRH             Created
 */

#include "AppHdr.h"
#include "it_use2.h"

#include <stdio.h>
#include <string.h>

#include "globals.h"
#include "externs.h"

#include "beam.h"
#include "effects.h"
#include "food.h"
#include "itemname.h"
#include "itemprop.h"
#include "misc.h"
#include "mutation.h"
#include "player.h"
#include "randart.h"
#include "religion.h"
#include "skills2.h"
#include "spells2.h"
#include "spl-cast.h"
#include "stuff.h"
#include "view.h"

// From an actual potion, pow == 40 -- bwr
bool potion_effect( char pot_eff, int pow )
{
    bool effect = true;  // current behaviour is all potions id on quaffing

    int new_value = 0;
    int i;

    // Note that actual potions are only pow == 40, 150 is for spell usage.
    if (pow > 150)
        pow = 150;

    switch (pot_eff)
    {
    case POT_HEALING:
        mpr("You feel better.");

        new_value = pow / 6;
        if (new_value < 5)
            new_value = 5;

        inc_hp( new_value + roll_dice(2, new_value), false );

        // only fix rot when healed to full
        if (you.hp == you.hp_max)
        {
            unrot_hp(1);
            set_hp(you.hp_max, false);
        }

        you.poison = 0;
        you.rotting = 0;
        you.disease = 0;
        you.conf = 0;
        you.duration[ DUR_STUN ] = 0;
        break;

    case POT_HEAL_WOUNDS:
        mpr("You feel much better.");

        new_value = pow / 3;
        if (new_value < 10)
            new_value = 10;

        inc_hp( new_value + roll_dice( 2, new_value ), false );

        // only fix rot when healed to full
        if (you.hp == you.hp_max)
        {
            unrot_hp( roll_dice(2,3) );
            set_hp(you.hp_max, false);
        }

        you.duration[ DUR_CUT ] = 0;
        break;

    case POT_SPEED:
        haste_player( 40 + random2(pow) );
        break;

    case POT_MIGHT:
        {
            const bool were_mighty = (you.might > 0 || you.berserker > 0);

            if (!were_mighty)
                mpr( "You feel very mighty all of a sudden." );
            else
            {
                mpr( "You still feel pretty mighty." );
                contaminate_player(1);
            }

            // conceivable max gain of +184 {dlb}
            you.might += 35 + random2(pow);

            if (!were_mighty)
                modify_stat(STAT_STRENGTH, 5, true);

            // files.cc permits values up to 215, but ... {dlb}
            if (you.might > 80)
                you.might = 80;

            did_god_conduct( DID_STIMULANTS, 4 + random2(4) );
        }
        break;

    case POT_GAIN_STRENGTH:
        mutate( MUT_STRONG, true );
        break;

    case POT_GAIN_DEXTERITY:
        mutate( MUT_AGILE, true );
        break;

    case POT_GAIN_INTELLIGENCE:
        mutate( MUT_CLEVER, true );
        break;

    case POT_LEVITATION:
        if (player_is_levitating())
            mpr( "You feel more buoyant." );
        else
        {
            mpr( "You gently float upwards %s.",
                  (grd[you.x_pos][you.y_pos] == DNGN_SHALLOW_WATER
                   || grd[you.x_pos][you.y_pos] == DNGN_DEEP_WATER)
                              ? "out of the water" : "off the floor" );
        }

        you.levitation += 25 + random2(pow);

        if (you.levitation > 100)
            you.levitation = 100;

        burden_change();
        break;

    case POT_STRONG_POISON:
        if (player_res_poison() >= 2)
            pot_eff = POT_POISON;
        // fall-through
    case POT_POISON:
        if (player_res_poison() >= 3)
        {
            mpr( "You feel %s nauseous.",
                    (pot_eff == POT_POISON) ? "slightly" : "quite" );
        }
        else
        {
            mpr( "That liquid tasted %s nasty...",
                    (pot_eff == POT_POISON) ? "very" : "extremely" );

            poison_player( ((pot_eff == POT_POISON) ? roll_dice(2,5)
                                                    : 5 + roll_dice(2,10)) );
        }
        break;

    case POT_SLOWING:
        slow_player( 10 + random2(pow) );
        break;

    case POT_PARALYSIS:
        paralyse_player( 2 + random2(pow) / 20 );
        break;

    case POT_CONFUSION:
        confuse_player( 3 + random2(pow) / 5 );
        break;

    case POT_INVISIBILITY:
        mpr( (!you.invis) ? "You fade into invisibility!"
                          : "You feel safely hidden away." );

        // now multiple invisiblity casts aren't as good -- bwr
        if (!you.invis)
            you.invis = 15 + random2(pow);
        else
            you.invis += random2(pow);

        if (you.invis > 100)
            you.invis = 100;
        break;

    // carnivore check here? {dlb}
    case POT_PORRIDGE:          // oatmeal - always gluggy white/grey?
        mpr("That potion was really gluggy!");
        lessen_hunger(6000, true);
        break;

    case POT_DEGENERATION:
        mpr("There was something very wrong with that liquid!");
        lose_stat( STAT_RANDOM, 1 + roll_zdice(2,3) );
        break;

    // Don't generate randomly - should be rare and interesting
    case POT_DECAY:
        rot_player( 10 + random2(10) );
        break;

    case POT_WATER:
        mpr("This tastes like water.");
        // we should really separate thirst from hunger {dlb}
        // Thirst would just be annoying for the player, the
        // 20 points here doesn't represesent real food anyways -- bwr
        lessen_hunger(20, true);
        break;

    case POT_EXPERIENCE:
        if (you.xp_level < 27)
        {
            mpr("You feel more experienced!");

            you.xp = 1 + exp_needed( 2 + you.xp_level );
            level_change();
        }
        else
            mpr("A flood of memories washes over you.");
        break;                  // I'll let this slip past robe of archmagi

    case POT_MAGIC:
        mpr( "You feel magical!" );
        new_value = 3 + roll_dice(2,10);

        // increase intrinsic MP points
        if (you.magic_points + new_value > you.max_magic_points)
        {
            new_value = (you.max_magic_points - you.magic_points)
                + (you.magic_points + new_value - you.max_magic_points) / 4 + 1;
        }

        inc_mp( new_value, true );
        break;

    case POT_RESTORE_ABILITIES:
        // messaging taken care of within function {dlb}
        // not quite true... if no stat's are restore = no message, and
        // that's just confusing when trying out random potions (this one
        // still auto-identifies so we know what the effect is, but it
        // shouldn't require bringing up the descovery screen to do that -- bwr
        if (restore_stat( STAT_ALL ) == false)
            mpr( "You feel refreshed." );
        break;

    case POT_BERSERK_RAGE:
        go_berserk(true);
        break;

    case POT_CURE_MUTATION:
        mpr("It has a very clean taste.");
        for (i = 0; i < 7; i++)
        {
            if (random2(10) > i)
                delete_mutation( PICK_RANDOM_MUTATION );
        }
        break;

    case POT_MUTATION:
        mpr("You feel extremely strange.");

        for (i = 0; i < 3; i++)
            mutate( PICK_RANDOM_MUTATION, false, false );
        break;
    }

    return (effect);
}                               // end potion_effect()

void unwield_item( bool no_empty_hand_msg )
{
    const int wpn = you.equip[EQ_WEAPON];

    if (wpn == -1)      // already empty handed
        return;

    you.special_wield = SPWLD_NONE;
    set_redraw_status( REDRAW_WIELD );

    if (you.inv[wpn].base_type == OBJ_WEAPONS)
    {
        if (is_fixed_artefact( you.inv[wpn] ))
        {
            switch (you.inv[wpn].special)
            {
            case SPWPN_SINGING_SWORD:
                mpr("The Singing Sword sighs.");
                break;
            case SPWPN_WRATH_OF_TROG:
                mpr("You feel less violent.");
                break;
            case SPWPN_SWORD_OF_POWER:
            case SPWPN_STAFF_OF_OLGREB:
                you.inv[wpn].plus = 0;
                you.inv[wpn].plus2 = 0;
                break;
            case SPWPN_STAFF_OF_WUCAD_MU:
                you.inv[wpn].plus = 0;
                you.inv[wpn].plus2 = 0;
                miscast_effect( SPTYP_DIVINATION, 9, 90, 100, "the Staff of Wucad Mu" );
                break;
            default:
                break;
            }

            return;
        }

        if (is_random_artefact( you.inv[wpn] ))
            unuse_randart(wpn);

        const int brand = get_weapon_brand( you.inv[wpn] );

        if (brand != SPWPN_NORMAL)
        {
            char str_pass[ITEMNAME_SIZE];
            in_name(wpn, DESC_CAP_YOUR, str_pass);
            strcpy(info, str_pass);

            switch (brand)
            {
            case SPWPN_SWORD_OF_CEREBOV:
            case SPWPN_FLAMING:
                strcat(info, " stops flaming.");
                mpr(info);
                break;

            case SPWPN_FREEZING:
            case SPWPN_HOLY_WRATH:
                strcat(info, " stops glowing.");
                mpr(info);
                break;

            case SPWPN_ELECTROCUTION:
                strcat(info, " stops crackling.");
                mpr(info);
                break;

            case SPWPN_VENOM:
                strcat(info, " stops dripping with poison.");
                mpr(info);
                break;

            case SPWPN_PROTECTION:
                mpr("You feel less protected.");
                set_redraw_status( REDRAW_ARMOUR_CLASS );
                break;

            case SPWPN_VAMPIRICISM:
                mpr("You feel the strange hunger wane.");
                break;

            case SPWPN_DISTORTION:
                miscast_effect( SPTYP_TRANSLOCATION, 9, 90, 100, "a distortion effect" );
                break;
            }                   // end switch


            if (you.duration[DUR_WEAPON_BRAND])
            {
                you.duration[DUR_WEAPON_BRAND] = 0;
                set_item_ego_type( you.inv[wpn], OBJ_WEAPONS, SPWPN_NORMAL );
                mpr("The weapon's brand evaporates.");
            }
        }                       // end if
    }

    you.equip[EQ_WEAPON] = -1;
    set_redraw_status( REDRAW_WIELD );

    if (!no_empty_hand_msg)
        canned_msg( MSG_EMPTY_HANDED );

    // Must have staff out of hand before calling calc_mp_max()
    if (player_equip( EQ_STAFF, STAFF_POWER ))
        calc_mp_max();

    return;
}                               // end unwield_item()

// This does *not* call ev_mod!
void unwear_armour(char unw)
{
    set_redraw_status( REDRAW_ARMOUR_CLASS );
    set_redraw_status( REDRAW_EVASION );

    switch (get_armour_ego_type( you.inv[unw] ))
    {
    case SPARM_RUNNING:
        mpr("You feel rather sluggish.");
        break;

    case SPARM_FIRE_RESISTANCE:
        mpr("\"Was it this warm in here before?\"");
        break;

    case SPARM_COLD_RESISTANCE:
        mpr("You catch a bit of a chill.");
        break;

    case SPARM_POISON_RESISTANCE:
        if (player_res_poison() <= 0)
            mpr("You feel less healthy.");
        break;

    case SPARM_SEE_INVISIBLE:
        if (!player_see_invis())
            mpr("You feel less perceptive.");
        break;

    case SPARM_DARKNESS:        // I do not understand this {dlb}
        if (you.invis)
            you.invis = 1;
        break;

    case SPARM_STRENGTH:
        modify_stat( STAT_STRENGTH, -3, false );
        break;

    case SPARM_DEXTERITY:
        modify_stat( STAT_DEXTERITY, -3, false );
        break;

    case SPARM_INTELLIGENCE:
        modify_stat( STAT_INTELLIGENCE, -3, false );
        break;

    case SPARM_PONDEROUSNESS:
        mpr("That put a bit of spring back into your step.");
        // you.speed -= 2;
        break;

    case SPARM_LEVITATION:
        //you.levitation++;
        if (you.levitation)
            you.levitation = 1;
        break;

    case SPARM_MAGIC_RESISTANCE:
        mpr("You feel less resistant to magic.");
        break;

    case SPARM_PROTECTION:
        mpr("You feel less protected.");
        break;

    case SPARM_STEALTH:
        mpr("You feel less stealthy.");
        break;

    case SPARM_RESISTANCE:
        mpr("You feel hot and cold all over.");
        break;

    case SPARM_POSITIVE_ENERGY:
        mpr("You feel vulnerable.");
        break;

    case SPARM_ARCHMAGI:
        mpr("You feel strangely numb.");
        break;
    }

    if (is_random_artefact( you.inv[unw] ))
        unuse_randart(unw);

    return;
}                               // end unwear_armour()

void unuse_randart(unsigned char unw)
{
    ASSERT( is_random_artefact( you.inv[unw] ) );

    if (you.inv[unw].ra_props[RAP_AC])
        set_redraw_status( REDRAW_ARMOUR_CLASS );

    if (you.inv[unw].ra_props[RAP_EVASION])
        set_redraw_status( REDRAW_EVASION );

    // modify ability scores
    modify_stat( STAT_STRENGTH, -you.inv[unw].ra_props[RAP_STRENGTH], true );
    modify_stat( STAT_INTELLIGENCE, -you.inv[unw].ra_props[RAP_INTELLIGENCE], true );
    modify_stat( STAT_DEXTERITY, -you.inv[unw].ra_props[RAP_DEXTERITY], true );

    if (you.inv[unw].ra_props[RAP_NOISES])
        you.special_wield = SPWLD_NONE;
}                               // end unuse_randart()

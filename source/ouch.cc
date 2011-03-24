/*
 *  File:       ouch.cc
 *  Summary:    Functions used when Bad Things happen to the player.
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 *      <8>      7/30/00        JDJ     Fixed end_game so that it works with filenames longer than 6 characters.
 *      <7>      19 June 2000   GDL     Changed handle to FILE *
 *      <6>      11/23/99       LRH     Fixed file purging for DOS?
 *      <5>      9/29/99        BCR     Fixed highscore so that it
 *                                      doesn't take so long.  Also
 *                                      added some whitespace to the scores.
 *                                      Fixed problem with uniques and 'a'.
 *      <4>      6/13/99        BWR     applied a mix of DML and my tmp
 *                                      file purging improvements.
 *      <3>      5/26/99        JDJ     highscore() will print more scores on
 *                                      larger windows.
 *      <2>      5/21/99        BWR     Added SCORE_FILE_ENTRIES, so
 *                                      that more top scores can be
 *                                      saved.
 *      <1>      -/--/--        LRH     Created
 */

#include "AppHdr.h"
#include "ouch.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>

#ifdef DOS
#include <conio.h>
#include <file.h>
#endif

#ifdef LINUX
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#endif

#ifdef USE_EMX
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#endif

#ifdef OS9
#include <stat.h>
#else
#include <sys/stat.h>
#endif

#ifdef __MINGW32__
#include <io.h>
#endif

#include "globals.h"
#include "externs.h"

#include "chardump.h"
#include "delay.h"
#include "files.h"
#include "hiscores.h"
#include "invent.h"
#include "itemname.h"
#include "itemprop.h"
#include "items.h"
#include "macro.h"
#include "mon-util.h"
#include "player.h"
#include "randart.h"
#include "religion.h"
#include "shopping.h"
#include "skills2.h"
#include "spells4.h"
#include "stuff.h"
#include "view.h"


void end_game( struct scorefile_entry &se );

/* NOTE: DOES NOT check for hellfire!!! */
// When warn_only is true we don't bother giving resist messages, we only
// give the more interesting susceptible messages.
//
// Note: sometimes damage gets passed in here magnified 10x, so it's important
// that everything here be applied as a ratio for now.
int check_your_resists( int hurted, beam_type flavour, bool warn_only )
{
    int   resist;

#if DEBUG_DIAGNOSTICS
    mpr( MSGCH_DIAGNOSTICS, "checking resistance: flavour=%d", flavour );
#endif

    // currently this only does condensation shield checks... still does
    // because we're calling it without any strength.
    expose_player_to_element( flavour );

    // duplicating old behaviour of these being "fire", but tagging and
    // handling the special cases here now.
    if (flavour == BEAM_NAPALM)
    {
        if ((you.species == SP_MOTTLED_DRACONIAN && you.xp_level >= 7)
            || player_equip( EQ_BODY_ARMOUR, ARM_MOTTLED_DRAGON_ARMOUR ))
        {
            if (!warn_only)
                canned_msg( MSG_YOU_RESIST );

            return (0);
        }

        flavour = BEAM_FIRE;
    }
    else if (flavour == BEAM_STEAM)
    {
        if ((you.species == SP_PALE_DRACONIAN && you.xp_level >= 7)
            || player_equip( EQ_BODY_ARMOUR, ARM_STEAM_DRAGON_ARMOUR ))
        {
            if (!warn_only)
                canned_msg( MSG_YOU_RESIST );

            return (0);
        }

        flavour = BEAM_FIRE;
    }

    switch (flavour)
    {
    case BEAM_FIRE:
        resist = player_res_fire();

        if (resist > 0)
        {
            if (!warn_only)
                canned_msg(MSG_YOU_RESIST);

            hurted /= (1 + resist);
        }
        else if (resist < 0)
        {
            mpr("It burns terribly!");
            hurted = (hurted * (10 - 5 * resist)) / 10;
        }
        break;

    case BEAM_HELLFIRE:
        resist = player_res_fire();

        if (resist >= 3)
        {
            if (!warn_only)
                canned_msg(MSG_YOU_RESIST);

            hurted = (hurted * 2) / 3;
        }
        else if (resist <= 0)
        {
            mpr("It burns terribly!");
            hurted = (hurted * (15 - 5 * resist)) / 10;
        }
        break;

    case BEAM_COLD:
        resist = player_res_cold();

        if (resist > 0)
        {
            if (!warn_only)
                canned_msg(MSG_YOU_RESIST);

            hurted /= (1 + resist);
        }
        else if (resist < 0)
        {
            mpr("You feel a terrible chill!");
            hurted = (hurted * (10 - 5 * resist)) / 10;
        }
        break;

    case BEAM_HELLFROST:
        resist = player_res_cold();

        if (resist >= 3)
        {
            if (!warn_only)
                canned_msg(MSG_YOU_RESIST);

            hurted = (hurted * 2) / 3;
        }
        else if (resist <= 0)
        {
            mpr("You feel a terrible chill!");
            hurted = (hurted * (15 - 5 * resist)) / 10;
        }
        break;

    case BEAM_ELECTRICITY:
        resist = player_res_electricity();

        if (resist >= 3)
        {
            if (!warn_only)
                canned_msg(MSG_YOU_RESIST);

            hurted = 0;
        }
        else if (resist > 0)
        {
            if (!warn_only)
                canned_msg(MSG_YOU_RESIST);

            hurted /= 3;
        }
        break;


    case BEAM_POISON:
        resist = player_res_poison();

        poison_player( coinflip() ? 2 : 1 );

        if (resist > 0)
        {
            if (!warn_only)
                canned_msg(MSG_YOU_RESIST);

            hurted /= (resist + 1);
        }
        break;

    case BEAM_POISON_ARROW:
        if (!you.is_undead)
            poison_player( 6 + random2(3), true );

        if (player_res_poison() > 0)
        {
            if (!warn_only)
                mpr("You partially resist.");

            hurted /= 2;
        }
        break;

    case BEAM_NEG:
        resist = player_prot_life();

        // Note: messages handled by drain_exp()
        if (resist > 0)
            hurted -= (hurted * resist) / 3;

        drain_exp();
        break;

    case BEAM_ICE:
        resist = player_res_cold();

        if (resist > 0)
        {
            if (!warn_only)
                mpr("You partially resist.");

            hurted /= 2;
        }
        else if (resist < 0)
        {
            mpr("You feel a painful chill!");
            hurted = (hurted * (11 - 2 * resist)) / 10;
        }
        break;

    case BEAM_LAVA:
        resist = player_res_fire();

        if (resist > 1)
        {
            if (!warn_only)
                mpr("You partially resist.");

            hurted /= resist;
        }
        else if (resist < 0)
        {
            mpr("It burns terribly!");
            hurted = (hurted * (10 - 5 * resist)) / 10;
        }
        break;

    case BEAM_ACID:
        if (player_res_acid())
        {
            if (!warn_only)
                canned_msg( MSG_YOU_RESIST );

            hurted /= 3;
        }
        break;

    case BEAM_MIASMA:
        if (player_prot_life() > random2(3))
        {
            if (!warn_only)
                canned_msg( MSG_YOU_RESIST );

            hurted = 0;
        }
        break;

    case BEAM_HOLY:
        if (!you.is_undead && you.species != SP_DEMONSPAWN)
        {
            if (!warn_only)
                canned_msg( MSG_YOU_RESIST );

            hurted = 0;
        }
        break;

    default:
        break;
    }                           /* end switch */

    return (hurted);
}                               // end check_your_resists()

void splash_with_acid( int acid_strength )
{
    /* affects equip only?
       assume that a message has already been sent, also that damage has
       already been done
     */
    int  splc = 0;
    int  dam = 0;

    const bool wearing_cloak = (you.equip[EQ_CLOAK] != -1);

    mpr( "You are splashed with acid!" );

    for (splc = EQ_WEAPON; splc <= EQ_BODY_ARMOUR; splc++)
    {
        if (you.equip[splc] == -1)
        {
            if (!wearing_cloak || coinflip())
                dam += roll_dice( 1, acid_strength );

            continue;
        }

        ASSERT( you.equip[splc] != -1 );

        if (random2(20) <= acid_strength)
            item_corrode( you.inv[ you.equip[splc] ] );
    }

    dam = check_your_resists( dam, BEAM_ACID );
    ouch( dam, 0, KILLED_BY_ACID );
}                               // end splash_with_acid()

void weapon_acid( char acid_strength )
{
    int hand_thing = get_inv_wielded();

    // empty handed or not wielding a weapon goes to gloves...
    if (hand_thing == -1 || you.inv[hand_thing].base_type != OBJ_WEAPONS)
        hand_thing = you.equip[EQ_GLOVES];

    // ... or to bare hands if we don't have gloves
    if (hand_thing == -1)
    {
        mpr( "Your %s burn!", your_hand(true) );

        int dam = check_your_resists( roll_dice(1, acid_strength), BEAM_ACID );
        ouch( dam, 0, KILLED_BY_ACID );
    }
    else if (random2(20) <= acid_strength)
    {
        item_corrode( you.inv[hand_thing] );
    }
}                               // end weapon_acid()

void item_corrode( item_def &item )
{
    bool it_resists = false;    // code simplifier {dlb}
    bool suppress_msg = false;  // code simplifier {dlb}

    if (item.base_type != OBJ_WEAPONS && item.base_type != OBJ_ARMOUR)
        return;

    const int how_rusty = ((item.base_type == OBJ_WEAPONS) ? -item.plus2
                                                           : -item.plus);

    const int max = (item.base_type == OBJ_ARMOUR) ? property( item, PARM_AC )
                                                   : 5;
    if (is_random_artefact( item  )
        || is_unrandom_artefact( item )
        || is_fixed_artefact( item )
        || how_rusty >= max)
    {
        it_resists = true;
        suppress_msg = true;
    }
    else if ((get_equip_race( item ) == ISFLAG_DWARVEN && !one_chance_in(5))
            || (player_res_corrosion() && !one_chance_in(10))
            || one_chance_in(5))
    {
        it_resists = true;
    }

    // handle message output and item damage {dlb}:
    if (!suppress_msg)
    {
        char str_pass[ ITEMNAME_SIZE ];
        item_name( item, DESC_CAP_YOUR, str_pass );
        strcpy( info, str_pass );
        strcat( info, (it_resists) ? " resists." : " is eaten away!" );

        mpr( info );
    }

    if (!it_resists)
    {
        if (item.base_type == OBJ_WEAPONS)
            item.plus2--;
        else
            item.plus--;

        if (item.slot == you.equip[EQ_WEAPON])
            set_redraw_status( REDRAW_WIELD );
        else
            set_redraw_status( REDRAW_ARMOUR_CLASS );
    }

    return;
}                               // end item_corrode()

// Helper function for the expose functions below.
// This currently works because elements only target a single type each.
static int get_target_class( beam_type flavour )
{
    int target_class = OBJ_UNASSIGNED;

    switch (flavour)
    {
    case BEAM_FIRE:
    case BEAM_LAVA:
    case BEAM_NAPALM:
    case BEAM_HELLFIRE:
        target_class = OBJ_SCROLLS;
        break;

    case BEAM_COLD:
    case BEAM_FRAG:
    case BEAM_HELLFROST:
        target_class = OBJ_POTIONS;
        break;

    case BEAM_SPORE:
        target_class = OBJ_FOOD;
        break;

    default:
        break;
    }

    return (target_class);
}

// XXX: These expose functions could use being reworked into a real system...
// the usage and implementation is currently very hacky.
// Handles the destruction of inventory items from the elements.
static void expose_invent_to_element( beam_type flavour, int strength )
{
    int i, j;
    int num_dest = 0;

    const int target_class = get_target_class( flavour );
    if (target_class == OBJ_UNASSIGNED)
        return;

    // Currently we test against each stack (and item in the stack)
    // independantly at strength%... perhaps we don't want that either
    // because it makes the system very fair and removes the protection
    // factor of junk (which might be more desirable for game play).
    for (i = 0; i < ENDOFPACK; i++)
    {
        if (!is_valid_item( you.inv[i] ))
            continue;

        if (is_valid_item( you.inv[i] )
            && (you.inv[i].base_type == target_class
                || (target_class == OBJ_FOOD
                    && you.inv[i].base_type == OBJ_CORPSES)))
        {
            if (player_item_conserve() && !one_chance_in(10))
                continue;

            for (j = 0; j < you.inv[i].quantity; j++)
            {
                if (random2(100) < strength)
                {
                    num_dest++;

                    if (i == you.equip[EQ_WEAPON])
                        set_redraw_status( REDRAW_WIELD );

                    if (dec_inv_item_quantity( i, 1 ))
                        break;
                }
            }
        }
    }

    if (num_dest > 0)
    {
        switch (target_class)
        {
        case OBJ_SCROLLS:
            snprintf( info, INFO_SIZE, "%s you are carrying %s fire!",
                      (num_dest > 1) ? "Some of the scrolls" : "A scroll",
                      (num_dest > 1) ? "catch" : "catches" );
            break;

        case OBJ_POTIONS:
            snprintf( info, INFO_SIZE, "%s you are carrying %s and %s!",
                      (num_dest > 1) ? "Some of the potions" : "A potion",
                      (num_dest > 1) ? "freeze" : "freezes",
                      (num_dest > 1) ? "shatter" : "shatters" );
            break;

        case OBJ_FOOD:
            snprintf( info, INFO_SIZE, "Some of your food is covered with spores!" );
            break;

        default:
            snprintf( info, INFO_SIZE, "%s you are carrying %s destroyed!",
                      (num_dest > 1) ? "Some items" : "An item",
                      (num_dest > 1) ? "were" : "was" );
            break;
        }

        mpr( info );    // XXX: should this be in a channel?
    }
}


// Handle side-effects for exposure to element other than damage.
// This function exists because some code calculates its own damage
// instead of using check_resists and we want to isolate all the special
// code they keep having to do... namely condensation shield checks,
// you really can't expect this function to even be called for much else.
//
// This function now calls expose_invent_to_element if strength > 0.
//
// XXX: this function is far from perfect and a work in progress.
void expose_player_to_element( beam_type flavour, int strength )
{
    // Note that BEAM_TELEPORT is sent here when the player
    // blinks or teleports.
    if (flavour == BEAM_FIRE || flavour == BEAM_LAVA
        || flavour == BEAM_HELLFIRE || flavour == BEAM_FRAG
        || flavour == BEAM_TELEPORT || flavour == BEAM_NAPALM
        || flavour == BEAM_STEAM)
    {
        if (you.duration[DUR_CONDENSATION_SHIELD] > 0)
        {
            mpr( MSGCH_DURATION, "Your icy shield dissipates!" );
            you.duration[DUR_CONDENSATION_SHIELD] = 0;
            set_redraw_status( REDRAW_ARMOUR_CLASS );
        }
    }

    if (strength)
        expose_invent_to_element( flavour, strength );

}


// Handles the destruction of floor items from the elements
void expose_floor_to_element( beam_type flavour, int x, int y )
{
    const int target_class = get_target_class( flavour );
    if (target_class == OBJ_UNASSIGNED)
        return;

    int smoke = 0;
    int shatter = 0;
    int next = NON_ITEM;

    for (int obj = igrd[x][y]; obj != NON_ITEM; obj = next)
    {
        next = mitm[obj].link;  // in case it's not there later

        if (mitm[obj].base_type == target_class
            || (target_class == OBJ_FOOD && mitm[obj].base_type == OBJ_CORPSES))
        {
            destroy_item( obj );
            if (target_class == OBJ_SCROLLS && see_grid(x,y))
                smoke++;
            else if (target_class == OBJ_POTIONS && player_can_hear(x,y))
                shatter++;
        }
    }

    if (shatter)
        mpr( MSGCH_SOUND, "You hear glass shatter." );

    if (smoke)
    {
        mpr( "You see %spuff%s of smoke.",
                (smoke > 1) ? "some " : "a ",
                (smoke > 1) ? "s" : "" );
    }
}


void lose_level(void)
{
    // because you.xp is unsigned long, if it's going to be -ve
    // must die straightaway.
    if (you.xp_level == 1)
        ouch( INSTANT_DEATH, 0, KILLED_BY_DRAINING );

    you.xp = exp_needed( you.xp_level + 1 ) - 1;
    you.xp_level--;

    mpr( MSGCH_WARN, "You are now a level %d %s!",
          you.xp_level, species_name( you.species ) );

    // Constant value to avoid grape jelly trick... see level_change() for
    // where these HPs and MPs are given back.  -- bwr
    ouch( 4, 0, KILLED_BY_DRAINING );
    dec_max_hp(4);

    dec_mp(1);
    dec_max_mp(1);

    calc_hp_max();
    calc_mp_max();

    set_redraw_status( REDRAW_EXPERIENCE );
}                               // end lose_level()

void drain_exp(void)
{
    int protection = player_prot_life();

    if (you.duration[DUR_PRAYER]
        && (you.religion == GOD_ZIN || you.religion == GOD_SHINING_ONE)
        && random2(150) < you.piety)
    {
        simple_god_message(" protects your life force!");
        return;
    }

    if (protection >= 3 || you.is_undead)
    {
        mpr( "You are unaffected." );
        return;
    }

    if (you.xp == 0)
        ouch( INSTANT_DEATH, 0, KILLED_BY_DRAINING );

    if (you.xp_level == 1)
    {
        you.xp = 0;
        you.exp_available = 0;
        mpr("You feel like you were born yesterday.");
        return;
    }

    const unsigned long base = 25 * (exp_needed(you.xp_level + 2)
                                      - exp_needed(you.xp_level + 1));

    unsigned long exp_drained = (base + random2(base)) / 100;

    if (protection > 0)
    {
        mpr("You partially resist.");
        exp_drained -= (protection * exp_drained) / 3;
    }

    if (exp_drained > 0)
    {
        mpr("You feel drained.");
        you.xp -= exp_drained;
        you.exp_available -= exp_drained;

        if (you.exp_available < 0)
            you.exp_available = 0;

#if DEBUG_DIAGNOSTICS
        mpr( MSGCH_DIAGNOSTICS, "exp_drain: %ld", exp_drained );
#endif

        set_redraw_status( REDRAW_EXPERIENCE );

        if (you.xp < exp_needed(you.xp_level + 1))
            lose_level();
    }
}                               // end drain_exp()

// death_source should be set to zero for non-monsters {dlb}
void ouch( int dam, int death_source, char death_type, const char *aux )
{
    int d = 0;

    interrupt_activity( AI_HP_LOSS );

    if (you.deaths_door
        && death_type != KILLED_BY_LAVA
        && death_type != KILLED_BY_WATER
        && death_type != KILLED_BY_PETRIFICATION)
    {
        return;
    }

#if DEBUG_DIAGNOSTICS
    // assumed bug for high damage amounts
    if (dam > 300
        && death_type != KILLED_BY_LAVA
        && death_type != KILLED_BY_WATER
        && death_type != KILLED_BY_PETRIFICATION)
    {
        mpr( MSGCH_WARN, "Potential bug: Unexpectedly high damage = %d", dam );
        return;
    }
#endif

    if (you_are_delayed())
        stop_delay();

    if (dam != INSTANT_DEATH)    // that is, a "death" caused by hp loss {dlb}
    {
        if (dam <= 0)
            return;

        switch (you.religion)
        {
        case GOD_XOM:
            if (random2(you.hp_max) > you.hp
                && dam > random2(you.hp)
                && one_chance_in(5))
            {
                simple_god_message( " protects you from harm!" );
                return;
            }
            break;

        case GOD_ZIN:
        case GOD_SHINING_ONE:
        case GOD_ELYVILON:
        case GOD_OKAWARU:
        case GOD_YREDELEMNUL:
            if (dam >= you.hp
                && you.duration[DUR_PRAYER] > 0
                && random2(you.piety) >= 30)
            {
                simple_god_message( " protects you from harm!" );
                lose_piety( 3 + random2(3) );
                return;
            }
            break;
        }

        // Damage applied here:
        dec_hp( dam, true );

        // Even if we have low HP messages off, we'll still give a
        // big hit warning (in this case, a hit for half our HPs) -- bwr
        if (dam > 0 && you.hp_max <= dam * 2)
            mpr( MSGCH_DANGER, "Ouch!  That really hurt!" );

        if (you.hp > 0 && Options.hp_warning
            && you.hp <= (you.hp_max * Options.hp_warning) / 100)
        {
            mpr( MSGCH_DANGER, "* * * LOW HITPOINT WARNING * * *" );
        }

        if (you.hp > 0)
            return;
    }

#ifdef WIZARD
    if (death_type != KILLED_BY_QUITTING
        && death_type != KILLED_BY_WINNING
        && death_type != KILLED_BY_LEAVING)
    {
        if (you.wizard)
        {
#ifdef USE_OPTIONAL_WIZARD_DEATH

#if DEBUG_DIAGNOSTICS
            mpr( MSGCH_DIAGNOSTICS, "Damage: %d; Hit points: %d", dam, you.hp );
#endif

            if (!yesno("Die?", false))
            {
                set_hp(you.hp_max, false);
                return;
            }
#else  // !def USE_OPTIONAL_WIZARD_DEATH
            mpr("Since you're a debugger, I'll let you live.");
            mpr("Be more careful next time, okay?");

            set_hp(you.hp_max, false);
            return;
#endif  // USE_OPTIONAL_WIZARD_DEATH
        }
    }
#endif  // WIZARD

    //okay, so you're dead:

    // do points first.
    long points = (you.xp * 7) / 10;

    // these now handled by giving player the value of their inventory
    char temp_id[NUM_IDTYPE][MAX_SUBTYPES];

    init_id_array( temp_id, ID_KNOWN_TYPE );

    // CONSTRUCT SCOREFILE ENTRY
    struct scorefile_entry se;

    // Score file entry version:
    //
    // 4.0      - original versioned entry
    // 4.1      - added real_time and num_turn fields
    // 4.2      - stats and god info

    se.version = 4;
    se.release = 2;

    strncpy( se.name, you.your_name, kNameLen );
    se.name[ kNameLen - 1 ] = '\0';
#ifdef MULTIUSER
    se.uid = static_cast<int>( getuid() );
#else
    se.uid = 0;
#endif

    FixedVector< int, NUM_RUNE_TYPES >  rune_array;

    se.num_runes = 0;
    se.num_diff_runes = 0;

    for (int i = 0; i < NUM_RUNE_TYPES; i++)
        rune_array[i] = 0;

    // Calculate value of pack and runes when character leaves dungeon
    if (death_type == KILLED_BY_LEAVING || death_type == KILLED_BY_WINNING)
    {
        long money = you.gold;

        for (d = 0; d < ENDOFPACK; d++)
        {
            if (is_valid_item( you.inv[d] ))
            {
                money += item_value( you.inv[d], temp_id, true );

                if (you.inv[d].base_type == OBJ_MISCELLANY
                    && you.inv[d].sub_type == MISC_RUNE_OF_ZOT)
                {
                    if (rune_array[ you.inv[d].plus ] == 0)
                        se.num_diff_runes++;

                    se.num_runes += you.inv[d].quantity;
                    rune_array[ you.inv[d].plus ] += you.inv[d].quantity;
                }
                else if (you.inv[d].base_type == OBJ_ORBS
                         && you.inv[d].special > 0) // only if set
                {
                    // Bonus for low total_skill_points when Orb was picked up.
                    se.skill_bonus_level = you.inv[d].special;
                    points += calc_skill_bonus( se.skill_bonus_level );
                }
            }
        }

        points += money * ((death_type == KILLED_BY_WINNING) ? 5 : 1);

        // Bonus for exploring different areas, not for collecting a
        // huge stack of demonic runes in Pandemonium (gold value
        // is enough for those). -- bwr
        if (se.num_diff_runes >= 3)
            points += ((se.num_diff_runes + 2) * (se.num_diff_runes + 2) * 1000);
    }

    // Players will have a hard time getting 1/10 of this (see XP cap):
    if (points > 99999999)
        points = 99999999;

    se.points = points;
    se.race = you.species;
    se.cls = you.char_class;

    // strcpy(se.race_class_name, "");
    se.race_class_name[0] = '\0';

    se.lvl = you.xp_level;
    se.best_skill = best_skill( SK_FIGHTING, NUM_SKILLS - 1 );
    se.best_skill_lvl = you.skills[ se.best_skill ];
    se.death_type = death_type;

    // for death by monster

    // Set the default aux data value...
    // If aux is passed in (ie for a trap), we'll default to that.
    if (!aux)
        se.auxkilldata[0] = '\0';
    else
    {
        strncpy( se.auxkilldata, aux, ITEMNAME_SIZE );
        se.auxkilldata[ ITEMNAME_SIZE - 1 ] = '\0';
    }

    if ((death_type == KILLED_BY_MONSTER || death_type == KILLED_BY_BEAM)
        && death_source >= 0 && death_source < MAX_MONSTERS)
    {
        struct monsters *monster = &menv[death_source];

        if (monster->type > 0 || monster->type <= NUM_MONSTERS)
        {
            se.death_source = monster->type;
            se.mon_num = monster->number;

            // Previously the weapon was only used for dancing weapons,
            // but now we pass it in as a string through the scorefile
            // entry to be appended in hiscores_format_single in long or
            // medium scorefile formats.
            // It still isn't used in monam for anything but flying weapons
            // though
            if (death_type == KILLED_BY_MONSTER
                && monster->inv[MSLOT_WEAPON] != NON_ITEM)
            {
#if HISCORE_WEAPON_DETAIL
                set_ident_flags( mitm[monster->inv[MSLOT_WEAPON]],
                                 ISFLAG_IDENT_MASK );
#else
                // changing this to ignore the pluses to keep it short
                unset_ident_flags( mitm[monster->inv[MSLOT_WEAPON]],
                                   ISFLAG_IDENT_MASK );

                set_ident_flags( mitm[monster->inv[MSLOT_WEAPON]],
                                 ISFLAG_KNOW_TYPE );

                // clear "runed" description text to make shorter yet
                set_equip_desc( mitm[monster->inv[MSLOT_WEAPON]], 0 );
#endif

                // Setting this is redundant for dancing weapons, however
                // we do care about the above indentification. -- bwr
                if (monster->type != MONS_DANCING_WEAPON)
                {
                    it_name( monster->inv[MSLOT_WEAPON], DESC_NOCAP_A, info );
                    strncpy( se.auxkilldata, info, ITEMNAME_SIZE );
                    se.auxkilldata[ ITEMNAME_SIZE - 1 ] = '\0';
                }
            }

            strcpy( info, monam( monster->number, monster->type, true,
                                 DESC_NOCAP_A, monster->inv[MSLOT_WEAPON] ) );

            strncpy( se.death_source_name, info, 40 );
            se.death_source_name[39] = '\0';
        }
    }
    else
    {
        se.death_source = 0;
        se.mon_num = 0;
        se.death_source_name[0] = '\0';
    }

    se.damage = dam;
    se.final_hp = you.hp;
    se.final_max_hp = you.hp_max;
    se.final_max_max_hp = you.hp_max + player_rotted();
    se.str = you.str;
    se.intel = you.intel;
    se.dex = you.dex;

    se.god = you.religion;
    if (you.religion != GOD_NO_GOD)
    {
        se.piety = you.piety;
        se.penance = you.penance[you.religion];
    }

    // main dungeon: level is simply level
    se.dlvl = you.depth + 1;
    switch (you.where_are_you)
    {
        case BRANCH_ORCISH_MINES:
        case BRANCH_HIVE:
        case BRANCH_LAIR:
        case BRANCH_SLIME_PITS:
        case BRANCH_VAULTS:
        case BRANCH_CRYPT:
        case BRANCH_HALL_OF_BLADES:
        case BRANCH_HALL_OF_ZOT:
        case BRANCH_ECUMENICAL_TEMPLE:
        case BRANCH_SNAKE_PIT:
        case BRANCH_ELVEN_HALLS:
        case BRANCH_TOMB:
        case BRANCH_SWAMP:
            se.dlvl = you.depth - you.branch_stairs[you.where_are_you - 10];
            break;

        case BRANCH_DIS:
        case BRANCH_GEHENNA:
        case BRANCH_VESTIBULE_OF_HELL:
        case BRANCH_COCYTUS:
        case BRANCH_TARTARUS:
        case BRANCH_INFERNO:
        case BRANCH_THE_PIT:
            se.dlvl = you.depth - 26;
            break;
    }

    se.branch = you.where_are_you;      // no adjustments necessary.
    se.level_type = you.level_type;     // pandemonium, labyrinth, dungeon..

    se.birth_time = you.birth_time;     // start time of game
    se.death_time = time( NULL );       // end time of game

    if (you.real_time != -1)
        se.real_time = you.real_time + (se.death_time - you.start_time);
    else
        se.real_time = -1;

    se.num_turns = you.num_turns;

#ifdef WIZARD
    se.wiz_mode = (you.wizard ? true : false);
#else
    se.wiz_mode = false;
#endif

#ifdef SCORE_WIZARD_CHARACTERS
    // add this highscore to the score file.
    hiscores_new_entry(se);
#else

    // only add non-wizards to the score file.
    // never generate bones files of wizard characters -- bwr
    if (!you.wizard)
    {
        hiscores_new_entry(se);

        if (death_type != KILLED_BY_LEAVING && death_type != KILLED_BY_WINNING)
            save_ghost();
    }

#endif

    end_game(se);
}

static std::string morgue_name(time_t when_crawl_got_even)
{
#ifdef SHORT_FILE_NAMES
    return "morgue";
#else  // !SHORT_FILE_NAMES
    std::string name = "morgue-" + std::string(you.your_name);

    if (tm *loc = localtime(&when_crawl_got_even))
    {
        char buf[25];
        snprintf(buf, sizeof buf, "-%04d%02d%02d-%02d%02d%02d",
                 loc->tm_year + 1900,
                 loc->tm_mon + 1,
                 loc->tm_mday,
                 loc->tm_hour,
                 loc->tm_min,
                 loc->tm_sec);
        name += buf;
    }
    return (name);
#endif // SHORT_FILE_NAMES
}

void end_game( struct scorefile_entry &se )
{
    int i;
    char del_file[300];         // massive overkill!
    bool dead = true;

    if (se.death_type == KILLED_BY_LEAVING ||
        se.death_type == KILLED_BY_WINNING)
    {
        dead = false;
    }

    // clean all levels that we think we have ever visited
    for (int level = 0; level < MAX_LEVELS; level++)
    {
        for (int dungeon = 0; dungeon < MAX_BRANCHES; dungeon++)
        {
            if (env.level_files[level][dungeon])
            {
                make_filename( info, you.your_name, level, dungeon,
                               false, false );
                unlink(info);
            }
        }
    }

    // temp level, if any
    make_filename( info, you.your_name, 0, 0, true, false );
    unlink(info);

    // create base file name
#ifdef SAVE_DIR_PATH
    snprintf( info, INFO_SIZE, SAVE_DIR_PATH "%s-%d", you.your_name,
                 static_cast<int>( getuid() ) );
#else
    strncpy(info, you.your_name, kFileNameLen);
    info[kFileNameLen] = '\0';
#endif

    // this is to catch the game package if it still exists.
#ifdef PACKAGE_SUFFIX
    strcpy(del_file, info);
    strcat(del_file, "." PACKAGE_SUFFIX);
    unlink(del_file);
#endif

    // last, but not least, delete player .sav file
    strcpy(del_file, info);

    char st_file[300];
    char kl_file[300];
    char tc_file[300];

    strcpy(st_file, del_file);
    strcpy(kl_file, st_file);
    strcpy(tc_file, st_file);

    strcat(del_file, ".sav");
    unlink(del_file);

    // Delete record of stashes
    strcat(st_file, ".st");
    unlink(st_file);

    strcat(kl_file, ".kil");
    unlink(kl_file);

    strcat(tc_file, ".tc");
    unlink(tc_file);

    // death message
    if (dead)
    {
        mpr("You die...");      // insert player name here? {dlb}
        viewwindow(true, false);// don't do this for leaving/winning characters
    }
    more();

    for (i = 0; i < ENDOFPACK; i++)
        set_ident_flags( you.inv[i], ISFLAG_IDENT_MASK );

    for (i = 0; i < ENDOFPACK; i++)
    {
        if (you.inv[i].base_type != 0)
        {
            set_ident_type( you.inv[i].base_type,
                            you.inv[i].sub_type, ID_KNOWN_TYPE );
        }
    }

    // XXX: store paralysis and other effects in scorefile?
    you.paralysis = 0;  // remove effect on evasion

    show_invent( -1, !dead );
    clrscr();

    if (!dump_char( morgue_name(se.death_time), !dead ))
        mpr("Char dump unsuccessful! Sorry about that.");
#if DEBUG_DIAGNOSTICS
    //jmf: switched logic and moved "success" message to debug-only
    else
        mpr("Char dump successful! (morgue.txt).");
#endif // DEBUG

    more();

    clrscr();
#ifdef DOS_TERM
    window(1, 1, 80, 25);
#endif

    clrscr();
    cprintf( "Goodbye, " );
    cprintf( you.your_name );
    cprintf( "." );
    cprintf( EOL EOL "    " ); // Space padding where # would go in list format

    char scorebuff[ HIGHSCORE_SIZE ];

    const int lines = hiscores_format_single_long( scorebuff, se, true );

    // truncate
    scorebuff[ HIGHSCORE_SIZE - 1 ] = '\0';
    cprintf( scorebuff );

    cprintf( EOL "Best Crawlers -" EOL );

    // "- 5" gives us an extra line in case the description wraps on a line.
    hiscores_print_list( get_number_of_lines() - lines - 5 );

    // just to pause, actual value returned does not matter {dlb}
    get_ch();
    end(0);
}

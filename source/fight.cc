/*
 *  File:       fight.cc
 *  Summary:    Functions used during combat.
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 *  <11>   07-jul-2000   JDJ    Fixed some of the code in you_attack so it doesn't
 *                              index past the end of arrays for unarmed attacks.
 *  <10>   03-mar-2000   bwr    changes for new spells, no stave magic
 *                              skill practising
 *   <9>   11/23/99      LRH    Now you don't get xp/piety for killing
 *                              monsters who were created friendly
 *   <8>   11/14/99      cdl    evade with random40(ev) vice random2(ev)
 *   <7>   10/ 8/99      BCR    Large races get a smaller
 *                                    penalty for large shields
 *   <6>    9/09/99      BWR    Code for 1-1/2 hand weapons
 *   <5>    8/08/99      BWR    Reduced power of EV/shields
 *   <4>    6/22/99      BWR    Changes to stabbing code, made
 *                              most gods not care about the
 *                              deathes of summoned monsters
 *   <3>    5/21/99      BWR    Upped learning of armour skill
 *                                    in combat slightly.
 *   <2>    5/12/99      BWR    Fixed a bug where burdened
 *                                    barehanded attacks where free
 *   <1>    -/--/--      LRH    Created
 */

#include "AppHdr.h"
#include "fight.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

#ifdef DOS
#include <conio.h>
#endif

#include "globals.h"
#include "externs.h"

#include "beam.h"
#include "cloud.h"
#include "debug.h"
#include "delay.h"
#include "effects.h"
#include "fight.h"
#include "food.h"
#include "it_use2.h"
#include "items.h"
#include "itemname.h"
#include "itemprop.h"
#include "macro.h"
#include "misc.h"
#include "monplace.h"
#include "mon-pick.h"
#include "monstuff.h"
#include "mon-util.h"
#include "mstuff2.h"
#include "mutation.h"
#include "ouch.h"
#include "player.h"
#include "randart.h"
#include "religion.h"
#include "skills.h"
#include "skills2.h"
#include "spells1.h"
#include "spells3.h"
#include "spells4.h"
#include "spl-cast.h"
#include "stuff.h"
#include "transfor.h"
#include "view.h"

struct extra_attack_def
{
    char  verb[20];
    int   to_hit;
    int   damage;
    int   speed;
    int   brand;

    int   index;    // internal use only
};

// static void stab_message(struct monsters *defender, int stab_bonus);

static void get_attack_description(int damage, char noise[80], char noise2[80]);

static int player_melee_weighted_stat( void );

/*
 **************************************************
 *                                                *
 *             BEGIN PUBLIC FUNCTIONS             *
 *                                                *
 **************************************************
*/


// Use this function when we care about success margins... if will return
// the difference in margin.
bool test_hit( int to_hit, int ev, int &margin )
{
    int   roll = -1;

    margin = AUTOMATIC_HIT;

    if (to_hit >= AUTOMATIC_HIT)
        return (true);
    else if (random2(1000) < 10 * MIN_HIT_MISS_PERCENTAGE)
        margin = (coinflip() ? 1 : -1) * AUTOMATIC_HIT;
    else
    {
        roll = random2( to_hit + 1 );
        margin = (roll - ev);
    }

#if DEBUG_DIAGNOSTICS
    char buff[80];

    float miss;

    if (to_hit < ev)
        miss = 100.0 - static_cast<float>( MIN_HIT_MISS_PERCENTAGE ) / 2.0;
    else
    {
        miss = static_cast<float>( MIN_HIT_MISS_PERCENTAGE ) / 2.0
              + static_cast<float>( (100 - MIN_HIT_MISS_PERCENTAGE) * ev )
                  / static_cast<float>( to_hit );
    }

    snprintf( buff, sizeof(buff),
              "to hit: %d; ev: %d; miss: %0.2f%%; roll: %d; result: %s%s (%d)",
              to_hit, ev, miss, roll, (margin >= 0) ? "hit" : "miss",
              (roll == -1) ? "!!!" : "", margin );

    mpr( MSGCH_DIAGNOSTICS, buff );
#endif

    return (margin >= 0);
}

// Use this function when success margin is irrelevant.
bool test_hit( int to_hit, int ev )
{
    int margin;  // passes, but ignored
    return (test_hit( to_hit, ev, margin ));
}

bool test_block( int shield, int attack )
{
    // We want to add at least one to shield so there's a difference
    // between SH 0 and SH 1.  We do it here so the diagnostic gives
    // the correct percent. "attack" is a calculated value that shouldn't
    // have this problem.
    shield += 1;
    const int shield_roll = random2( shield );
    const int attack_roll = random2( attack );

#if DEBUG_DIAGNOSTICS
    float per;
    char buff[80];

    if (shield <= attack)
    {
        const int succ = (shield * (shield + 1)) / 2;  // big triangle
        per = static_cast<float>(succ) / static_cast<float>(shield * attack);
    }
    else
    {
        const int fail = (attack * (attack - 1)) / 2;  // small triangle
        per = 1.0 - static_cast<float>(fail) / static_cast<float>(shield * attack);
    }

    snprintf( buff, sizeof(buff),
              "to-block: %d (%d); con-block: %d (%d); block: %0.2f%%; result: %s",
              shield, shield_roll, attack, attack_roll, 100.0 * per,
              (shield_roll >= attack_roll) ? "blocked" : "not" );

    mpr( MSGCH_DIAGNOSTICS, buff );
#endif

    return (shield_roll >= attack_roll);
}

int your_shield_blocks_penalty( void )
{
    // const bool ur_armed = (you.equip[EQ_WEAPON] != -1);
    const int shield = you.equip[EQ_SHIELD];

    if (!you.shield_blocks)
        return (0);

    // int penalty = MULTIBLOCK_FACTOR * (ur_armed ? 10 : 20);
    int penalty = MULTIBLOCK_FACTOR * 10;

    if (shield != -1)
    {
        const int pen = -5 * property( you.inv[shield], PARM_EVASION );
        const int stat = (you.inv[shield].sub_type == ARM_BUCKLER) ? you.dex
                                                                   : you.str;

        penalty = stat_div( stat, penalty, 10 + pen );
        penalty = stat_div( skill_bump( SK_SHIELDS ), penalty );
    }

    int mult = you.shield_blocks - random2( skill_bump(SK_SHIELDS) ) / 5;
    penalty *= ((mult < 1) ? 1 : mult);

    return (div_rand_round( penalty, 10 ));
}

// returns x10 penalty to hit
int heavy_weapon_penalty( void )
{
    const int wpn = get_inv_in_hand();

    if (wpn == -1)
        return (0);

    int heavy_weapon = 0;

    if (you.inv[wpn].base_type == OBJ_WEAPONS
        || you.inv[wpn].base_type == OBJ_STAVES)
    {
        // ignoring weight for 1-1/2 style check since that's done here
        const bool half = check_hand_and_half_style( true );
        const int  req  = weapon_str_required( you.inv[wpn], half );
        const int  diff = req - you.str + 1;

        if (diff > 1)
            heavy_weapon = diff * diff * 100;
    }
    else
    {
        heavy_weapon = item_mass( you.inv[wpn] ) / 5;
    }

    return (heavy_weapon);
}

// returns x10 penalty to hit
int heavy_shield_penalty( void )
{
    const int shield = you.equip[EQ_SHIELD];

    int heavy_shield = 0;

    if (shield != -1)
    {
        // penalty from evasion modifier (reduced by skill):
        const int pen = -20 * property( you.inv[shield], PARM_EVASION );

        heavy_shield = pen - skill_bump( SK_SHIELDS );

        if (heavy_shield < 0)
            heavy_shield = 0;
        else
            heavy_shield = stat_div( you.str, heavy_shield );

        // penalty for insufficient strength
        const int req = armour_str_required( you.inv[shield] );
        const int diff = req - you.str + 1;

        if (diff > 1)
            heavy_shield += diff * diff * 50;
    }

    return (heavy_shield);
}

// returns x10 penalty to hit
int heavy_armour_penalty( void )
{
    const int arm = you.equip[EQ_BODY_ARMOUR];

    int heavy_armour = 0;

    if (arm != -1)
    {
        // penalty from evasion modifier (reduced by skill):
        const int ev_pen = -10 * property( you.inv[arm], PARM_EVASION );

        heavy_armour = stat_div( skill_bump( SK_ARMOUR ), ev_pen );

        if (heavy_armour < ev_pen / 2)
            heavy_armour = ev_pen / 2;

        // penalty for insufficient strength
        const int req = armour_str_required( you.inv[arm] );
        const int diff = req - you.str + 1;

        if (diff > 1)
            heavy_armour += diff * diff * 100;
    }

    return (heavy_armour);
}

// returns x10 penalty to hit
int heavy_equip_penalty( void )
{
    const int heavy_weapon = heavy_weapon_penalty();
    const int heavy_shield = heavy_shield_penalty();
    const int heavy_armour = heavy_armour_penalty();

    const int heavy_equip = heavy_weapon + heavy_shield + heavy_armour;

#if DEBUG_DIAGNOSTICS
    mpr( MSGCH_DIAGNOSTICS,
          "heavy = %0.1f: wpn = %0.1f, shd = %0.1f, arm = %0.1f",
          static_cast<float>(heavy_equip) / 10.0,
          static_cast<float>(heavy_weapon) / 10.0,
          static_cast<float>(heavy_shield) / 10.0,
          static_cast<float>(heavy_armour) / 10.0 );
#endif

    return (heavy_equip);
}

// Calculate the player's to-hit... max == -1 for the primary strike.
int your_melee_to_hit( struct monsters *targ, int extra_hit, int max,
                       bool hand_and_half, bool ignore_hidden )
{
    const int  wpn = get_inv_wielded();
    const bool extra_unarmed = (max >= 0);

    const bool unarmed = (extra_unarmed || wpn == -1);
    const bool bashing = player_is_bashing();
    const bool armed   = (!unarmed && !bashing);
    const bool monk    = check_monk_style();

    const int  wpn_skill  = player_weapon_skill();
    const bool inaccuracy = (!ignore_hidden && player_equip( EQ_AMULET, AMU_INACCURACY ));
    const int  dex_weight = (armed) ? weapon_dex_weight( you.inv[wpn] ) : 10;

    // Basic formula is: HIT = DEX * dex-factors + plus-factors
    int to_hit = 0;

    // The dexterity factor (multiplied by dex, inaccuracy cancels)
    int dex_factor = you.dex;   // this gives a O(dex^2) term

    // Non-dex related pluses to add
    int plus = slaying_bonus( PWPN_HIT, ignore_hidden ) - 5 * inaccuracy;

    // We apply dex only to a dex weighted portion of the skill... This
    // is so that strong/low dex characters can use the big heavy weapons
    // we're expecting them to use.
    int skill_factor = 4 * skill_bump( SK_FIGHTING );

    if (wpn_skill != SK_FIGHTING && !bashing)
        skill_factor += 2 * you.skills[ wpn_skill ];

    const int dex_skill = (skill_factor * dex_weight) / 10;

    dex_factor += dex_skill;
    to_hit += skill_factor - dex_skill;

    if (unarmed)
    {
        if (!extra_unarmed)        // these only apply to the first strike
        {
            // see transfor.cc for hand "weapon" stats (including non-shifted)
            plus += transform_hand_to_hit();

            if (you.confusing_touch)
                dex_factor += 1 * TOHIT_PLUS_MULTIPLE;
        }

        // skill applied a second time for monk style fighters
        if (monk)
            dex_factor += 3 + skill_bump( SK_UNARMED_COMBAT ) / 3;
    }
    else if (armed)  // usable thing in hand (!bashing)
    {
        if (hand_and_half)
            plus += 1;

        if (you.inv[wpn].base_type == OBJ_WEAPONS)
        {
            // elven weapons and sure blades are influenced by dex:
            if (get_equip_race( you.inv[wpn] ) == ISFLAG_ELVEN
                && player_genus( GENPC_ELVEN ))
            {
                dex_factor += 1 * TOHIT_PLUS_MULTIPLE;
            }

            if (wpn_skill == SK_SHORT_BLADES && you.sure_blade)
                dex_factor += 2 * TOHIT_PLUS_MULTIPLE;

            // but weapon base accuracy and plus to-hit are not:
            plus += property( you.inv[wpn], PWPN_HIT );

            if (!ignore_hidden || item_ident(you.inv[wpn], ISFLAG_KNOW_PLUSES))
                plus += you.inv[wpn].plus;

        }
        else if (you.inv[wpn].base_type == OBJ_STAVES)
        {
            plus += property( you.inv[wpn], PWPN_HIT );
        }
    }

    // All effects pertaining to a particular target go here:
    if (targ)
    {
        switch (player_damage_brand( extra_unarmed ))
        {
        case SPWPN_ORC_SLAYING:
            if (mons_species( targ->type ) == MONS_ORC)
                plus += 3;
            break;

        case SPWPN_DISRUPTION:
            if (mons_holiness( targ ) == MH_UNDEAD)
                plus += 3;
            break;

        case SPWPN_HOLY_WRATH:
            if (mons_is_unholy( targ ))
                plus += 2;
            break;

        default:
            break;
        }

        if (mons_has_ench( targ, ENCH_BACKLIGHT ))
            plus += 2;

        if (!player_monster_visible( targ ))
            plus -= 5;
    }

    if (you.invis && !player_see_invis())      // can't see own hands
        plus -= 2;

    if (you.conf)
        plus -= 5;

    if (you.duration[DUR_STUN])
        plus -= 5;

    if (you.hunger_state == HS_STARVING)
        plus -= 1;

    if (you.burden_state == BS_ENCUMBERED)
        plus -= 2;
    else if (you.burden_state == BS_OVERLOADED)
        plus -= 5;

    // Now to calculate to-ht:
    // Apply dex based factors (don't make high dex make a minus worse)
    if (dex_factor <= 0)
        to_hit += dex_factor;
    else if (!inaccuracy)
        to_hit += stat_mult( you.dex, dex_factor );

    // Apply other pluses:
    to_hit += TOHIT_PLUS_MULTIPLE * plus;

    // Penalty from equipment: no longer random since this function needs
    // to return consistant values (it's used for qualitative skill message).
    to_hit -= (heavy_equip_penalty() + 5) / 10;

    // other unusual conditions (ie water attacks, multiple attack penalty)
    to_hit += extra_hit;

    // extra actions become more difficult...
    if (extra_unarmed)
    {
        // limited by previous to_hit
        if (to_hit > max)
            to_hit = max;

        // dex and monk style play reduce penalty
        to_hit -= (3 * TOHIT_PLUS_MULTIPLE * (monk ? 10 : 25)) / you.dex;
    }

    return ((to_hit > 0) ? to_hit : 0);
}

static int your_melee_damage( int base_dam, bool hand_and_half, bool water_attack )
{
    const bool extra_unarmed = (base_dam > 0);
    const int  wpn = get_inv_wielded();

    const bool bashing = player_is_bashing();
    const bool unarmed = ((extra_unarmed || wpn == -1) && !bashing);
    const bool armed   = (!unarmed && !bashing);
    const bool monk    = check_monk_style();

    const bool half = check_hand_and_half_style();
    const int  req  = (armed) ? weapon_str_required( you.inv[wpn], half ) : 0;
    const int  diff = req - you.str;

    const int  wpn_skill = player_weapon_skill();
    const int  skill = (wpn_skill == SK_FIGHTING) ? 0 : you.skills[wpn_skill];

    // const int  melee_brand = player_damage_brand( extra_unarmed );

    if (base_dam < 0 || bashing)
        base_dam = 2;

    int plus = slaying_bonus( PWPN_DAMAGE );

    if (!extra_unarmed)  // because that's how these used to be.
    {
        if (water_attack)
            plus += 10;

        if (you.might > 1 || you.berserker > 1)
            plus += 10;

        if (you.hunger_state == HS_STARVING)
            plus -= 5;
    }

    if (unarmed)
    {
        if (!extra_unarmed)
        {
            if (you.confusing_touch)
                base_dam = 0;
            else
                base_dam = transform_hand_damage();
        }

        if (you.mutation[MUT_DRAIN_LIFE] == 1)
            base_dam += 3;

        if (monk)
        {
            base_dam += 2;
            plus += 1 + you.skills[SK_UNARMED_COMBAT] / 3;
        }
    }
    else if (armed)
    {
        base_dam = property( you.inv[wpn], PWPN_DAMAGE );

        if (you.species == SP_HILL_ORC
            && get_equip_race( you.inv[wpn] ) == ISFLAG_ORCISH)
        {
            base_dam += (coinflip() ? 0 : 1);
        }
        else if (player_genus( GENPC_DWARVEN )
            && get_equip_race( you.inv[wpn] ) == ISFLAG_DWARVEN)
        {
            base_dam += 1;
        }

        if (hand_and_half)
            base_dam += 1;

        // weak penalty:
        if (diff > 0)
            base_dam = 1 + (base_dam + 1) / (1 + diff * diff);

        if (you.inv[wpn].base_type == OBJ_WEAPONS)
            plus += you.inv[wpn].plus2;
    }

    int skill_bonus = -50;

    if (!bashing && diff <= 0)
    {
        // removing the +5 shift for now
        skill_bonus = base_dam;

        // XXX: apply impact_mass here as part of the STR factor
        const int str_bonus = (you.str <= 25) ? 4 * (you.str - 10)
                                              : 60 + 2 * (you.str - 25);

        skill_bonus *= (2 * you.skills[SK_FIGHTING] + 8 * skill + str_bonus);
    }

    const int damage = base_dam + plus + div_rand_round( skill_bonus, 100 );

#if DEBUG_DIAGNOSTICS
    mpr( MSGCH_DIAGNOSTICS,
            "damage %d = base (%d) + plus (%d) + skill (%0.3f)",
            damage, base_dam, plus, static_cast<float>(skill_bonus) / 100.0 );
#endif

    return ((damage > 1) ? damage : 1);
}

// returns time taken x100
int your_melee_speed( bool hand_and_half, bool speed_brand, bool ignore_blocks ) {
    UNUSED( hand_and_half );
    UNUSED( ignore_blocks );

    const int  wpn = get_inv_wielded();

    const bool bashing = player_is_bashing();
    const bool unarmed = (wpn == -1);
    const bool monk    = check_monk_style();
    // const bool bearing_shield = (you.equip[EQ_SHIELD] != -1);

    const bool armed      = (!unarmed && !bashing);
    const int  speed_stat = (player_melee_weighted_stat() + 5); // stat * 10

    const int  wpn_skill = player_weapon_skill();
    const int  skill     = (wpn_skill == SK_FIGHTING) ? 0
                                                      : you.skills[wpn_skill];

    const bool half = check_hand_and_half_style();
    const int  req  = (armed) ? weapon_str_required( you.inv[wpn], half ) : 0;
    const int  diff = req - you.str;

    int base = 10;              // time * 1
    int factor = 1000;          // percent * 10
    int speed = 10000;          // time * 1000
    int min_speed = 5000;       // time * 1000

    if (unarmed)
    {
        base = transform_hand_speed();
        min_speed = base * 400;
        factor = (skill * speed_stat) / ((monk) ? 7 : 9);
    }
    else if (armed)
    {
        ASSERT( you.inv[wpn].base_type == OBJ_WEAPONS
                || you.inv[wpn].base_type == OBJ_STAVES );

        base = property( you.inv[wpn], PWPN_SPEED );

        // Double ended weapons are only fast for those who are the
        // correct size and using both hands:
        if (is_double_ended( you.inv[wpn] ) && !check_using_both_ends())
            base = double_wpn_awkward_speed( you.inv[wpn] );

        // Note: speed_stat is somewhat easier to have high than
        // specifically dex or str, since the player can choose the
        // stat by choosing weapons.
        if (wpn_skill == SK_SHORT_BLADES)
        {
            factor = (skill * speed_stat) / 9;
            min_speed = base * 400;
        }
        else
        {
            factor = (skill * speed_stat) / 11;
            min_speed = base * 500;
        }

        if (diff > 0)
            factor -= 100 * diff * diff;
    }
    else  // bashing
    {
        factor = -stat_div( you.str, item_mass( you.inv[wpn] ) );
        min_speed = 10000;
    }

    // calculate base speed:
    speed = base * (1000 - factor);

    if (speed < min_speed)
        speed = min_speed;

    // apply speed weapon effect
    if (speed_brand)
        speed = (speed * 2) / 3;

    // safty checks to keep weapon speeds reasonable
    if (speed < 2500)
        speed = 2500;
    else if (speed > 30000)
        speed = 30000;

#if DEBUG_DIAGNOSTICS
    mpr( MSGCH_DIAGNOSTICS, "melee stat: %d; factor: %0.1f%%; speed: %.3f",
              speed_stat,
              static_cast<float>(factor) / 10.0,
              static_cast<float>(speed) / 1000.0 );
#endif

    return (speed);
}

// Note: it's up to the calling code to decide if/which weapon is suitable...
// for example, this can affect extra unarmed strikes while using a weapon.
bool check_monk_style( void )
{
    return (you.equip[EQ_SHIELD] == -1
            && you.burden_state == BS_UNENCUMBERED
            && player_light_armour()
            && !player_is_bashing()
            && !player_is_shapechanged( false )
            && you.dex >= 15);
}

bool check_hand_and_half_style( bool ignore_weight )
{
    const int wpn = get_inv_wielded();

    if (wpn != -1)
    {
        return (you.equip[EQ_SHIELD] == -1
                && you.burden_state == BS_UNENCUMBERED
                && hands_reqd( you.inv[wpn], player_size() ) == HANDS_HALF
                && !is_double_ended( you.inv[wpn] ) // handled elsewhere
                && !item_cursed( you.inv[wpn] )
                && !player_is_bashing()
                && (ignore_weight || !heavy_weapon_penalty()));
    }

    return (false);
}

bool check_using_both_ends( void )
{
    const int wpn = get_inv_wielded();

    if (wpn == -1)
        return (false);

    ASSERT( is_double_ended( you.inv[wpn] ) );

    return (you.equip[EQ_SHIELD] == -1
            && hands_reqd( you.inv[wpn], player_size() ) == HANDS_HALF);
}

static void vampiric_heal( int att_id, int def_id, int base_dam,
                           int fixed_special = SPWPN_NORMAL )
{
    int amount = 0;

    if (fixed_special == SPWPN_VAMPIRES_TOOTH)
        amount = roll_dice( 1, base_dam );
    else
        amount = roll_dice( 1, base_dam / 2 );

    if (att_id == MHITYOU)
    {
        if (you.hp < you.hp_max && amount > 0)
        {
            mpr( "You feel better." );
            inc_hp( amount, false );
            did_god_conduct( DID_NECROMANCY, 2 );
            // This is probably a bit much... healing should be good enough.
            // if (you.hunger_state != HS_ENGORGED)
            //      lessen_hunger( 10 + roll_zdice(2,15), true );
        }
    }
    else
    {
        if (heal_monster( &menv[att_id], amount, true ))
        {
            if (def_id == MHITYOU)
                mon_msg( &menv[att_id], " draws strength from your injuries." );
            else
                mon_msg( &menv[att_id], " is healed." );
        }
    }
}

// Returns the DAM type of the critical is successful, and DAM_BASH if not
// XXX: right now we're not using extra_unarmed (as that might be too good)...
// however, it might be nice if eventually we could give tailslaps and
// the martial punch stun criticals (I don't really see horns as being
// too capable of the pierce critical though... maybe cut for goring?).
static int check_player_crit_hit( struct monsters *const mon, bool extra_unarmed )
{
    const int  stat = player_melee_weighted_stat();
    const int  brand = player_damage_brand( extra_unarmed );

    const int  mon_id = monster_index( mon );
    const int  dam_type = player_damage_type( extra_unarmed );
    const int  chosen_type = being_choose_damage_type( mon_id, dam_type );
    const bool helpless = mons_class_flag( mon->type, M_NO_EXP_GAIN );

    int chance = you.skills[SK_STABBING];

    if (helpless || chosen_type == DAM_BASH || !player_monster_visible(mon))
        return (DAM_BASH);
    else if (mon->behaviour == BEH_SLEEP)
    {
        if (player_weapon_skill() == SK_SHORT_BLADES)
            return (DAM_PIERCE);

        return (chosen_type);
    }

    if (mon->behaviour == BEH_FLEE || mons_has_ench( mon, ENCH_CONFUSION ))
        chance += 5;
    else if (mon->foe != MHITYOU && !mons_is_batty( mon ))
        chance += 2;

    if (brand == SPWPN_VORPAL)
        chance += 10;

    if (being_res_damage_type( mon_id, chosen_type ) < 0)
        chance += 10;

    chance = stat_mult( stat, chance );

    if (chance > 50)
        chance = 50;

    return ((random2(1000) < chance) ? chosen_type : DAM_BASH);
}

// returns the DAM type of the critical is successful, and DAM_BASH if not
static int check_mons_crit_hit( int att_id, int def_id, int dam_type )
{
    const int  chosen_type = being_choose_damage_type( def_id, dam_type );
    const bool fighter = mons_class_flag( menv[att_id].type, M_FIGHTER );

    const int  chance = menv[att_id].hit_dice * ((fighter) ? 2 : 1)
                        + 10 * fighter;

    return ((random2(1000) < chance) ? chosen_type : DAM_BASH);
}

static bool do_ego_damage( int base, int melee_brand, int dam_type,
                           int att_id, int def_id, int fixed_special )
{
    ASSERT( att_id != MHITNOT && def_id != MHITNOT );
    ASSERT( att_id != def_id );

    bool wounded = false;

    int res = 0;
    int specdam = 0;
    const char *msg = 0;

    switch (melee_brand)
    {
    case SPWPN_NORMAL:
    case SPWPN_SPEED:           // done elsewhere
    default:
        break;

    case SPWPN_FLAMING:
        res = being_res_fire( def_id );
        if (fixed_special == SPWPN_SWORD_OF_CEREBOV)
        {
            if (res < 3 && res > 0)
                res = 0;
            else
                res--;
        }

        if (res <= 0)
            specdam = roll_dice( 1 - res, base );

        if (specdam)
            msg = "burn";
        break;

    case SPWPN_FREEZING:
        res = being_res_cold( def_id );

        if (res <= 0)
            specdam = roll_dice( 1 - res, base );

        if (specdam)
            msg = "freeze";
        break;

    case SPWPN_HOLY_WRATH:
        if (def_id != MHITYOU)
        {
            // XXX: there should be a case in here for holy monsters?
            // see elsewhere in fight.cc {dlb}
            const mon_holy_type holy = mons_holiness( &menv[def_id] );

            if (holy == MH_UNDEAD)
                specdam = roll_dice( 1, base );
            else if (holy == MH_DEMONIC)
                specdam = roll_dice( 1, (base * 15) / 10 );

            if (specdam)
                mon_msg( &menv[def_id], " convulses in agony!" );
        }
        else
        {
            if (you.is_undead)
                specdam = roll_dice( 1, base );
            else if (you.species == SP_DEMONSPAWN)
                specdam = roll_dice( 1, (base * 15) / 10 );

            if (specdam)
                mpr( "The wound is extremely painful!" );
        }
        break;

    case SPWPN_ELECTROCUTION:
        if (being_res_elec( def_id ) > 0)
            break;

        if (one_chance_in(3))
        {
            mpr( "There is a sudden explosion of sparks!" );
            specdam = roll_dice( 3, 10 );
        }
        break;

    case SPWPN_ORC_SLAYING:
        if (def_id != MHITYOU)
        {
            if (mons_species( menv[def_id].type ) == MONS_ORC)
            {
                specdam = roll_dice( 2, base );
                mon_msg( &menv[def_id], " screams in agony!" );
            }
        }
        else
        {
            if (you.species == SP_HILL_ORC && !player_is_shapechanged())
            {
                specdam = roll_dice( 2, base );
                mpr( "The wound is extremely painful!" );
            }
        }
        break;

    case SPWPN_VENOM:
        if (!one_chance_in(3))
        {
            if (def_id != MHITYOU)
                poison_monster( &menv[def_id], true ); // does resist and msg
            else
            {
                if (menv[att_id].type == MONS_DANCING_WEAPON)
                    mon_msg( &menv[att_id], " is poisoned!" );
                else
                    mon_msg( &menv[att_id], "'s weapon is poisoned!" );

                if (player_res_poison() < 3)
                    poison_player(2);
            }
        }
        break;

    case SPWPN_DRAINING:
        if (being_res_negative_energy( def_id ) > 0 || one_chance_in(3))
            break;

        if (def_id != MHITYOU)
            drain_monster( &menv[def_id] );
        else
            drain_exp();

        if (att_id == MHITYOU)
            did_god_conduct( DID_NECROMANCY, 2 );
        break;

    case SPWPN_VORPAL:
        res = being_res_damage_type( def_id, dam_type );

        if (res == 0)
            specdam = roll_dice( 1, base / 2 );
        else if (res < 0)
            specdam = roll_dice( 1, base );
        break;

    case SPWPN_VAMPIRICISM:
        res = being_res_negative_energy( def_id );
        if (res > random2(3) || base < 1 || !one_chance_in(3))
            break;

        vampiric_heal( att_id, def_id, base, fixed_special );
        break;

    case SPWPN_DISRUPTION:
        if (!one_chance_in(3))
        {
            if ((def_id != MHITYOU
                    && mons_holiness( &menv[def_id] ) == MH_UNDEAD)
                || you.is_undead)
            {
                specdam += roll_dice( 3, base );

                if (def_id != MHITYOU)
                    mon_msg( &menv[def_id], " shudders!" );
                else
                    mpr( "You are blasted by holy energy!" );
            }
        }
        break;

    case SPWPN_PAIN:
        if (being_res_negative_energy( def_id ) < random2(4))
        {
            if (att_id != MHITYOU)
                specdam = roll_dice( 1, 5 + menv[att_id].hit_dice );
            else
            {
                specdam = roll_dice( 1, 1 + you.skills[SK_NECROMANCY] );
                did_god_conduct( DID_NECROMANCY, 4 );
            }

            if (def_id != MHITYOU)
                mon_msg( &menv[def_id], " convulses in agony!" );
            else
                mpr( "The wound is extremely painful!" );
        }
        break;

    case SPWPN_DISTORTION:
        if (one_chance_in(5))
            specdam = distort_being( def_id, att_id );
        break;

    case SPWPN_CONFUSING_TOUCH:
        if (def_id == MHITYOU)
            confuse_player( 3 + random2(10) );  // but shouldn't happen (yet)
        else
        {
            if (mons_holiness( &menv[def_id] ) == MH_NATURAL)
            {
                // simple HD save:
                if (random2(30) < menv[def_id].hit_dice)
                {
                    // partial discharge hands
                    you.confusing_touch -= roll_dice(3, 5);

                    if (you.confusing_touch < 1)
                        you.confusing_touch = 1;
                }
                else
                {
                    // declaring these just to pass to the enchant function
                    struct bolt beam_temp;
                    beam_temp.flavour = BEAM_CONFUSION;

                    // This skips the MR check:
                    mons_ench_f2( &menv[def_id], beam_temp );

                    // discharge hands
                    you.confusing_touch = 1;
                }
            }
        }
        break;
    }                       /* end switch */

    if (specdam > 0)
    {
        // Print a message for those who still need one.
        if (msg)
        {
            const char *const punct = damage_punctuation( specdam );

            if (att_id == MHITYOU)
            {
                mpr( "You %s %s%s",
                     msg, ptr_monam( &menv[def_id], DESC_NOCAP_THE ), punct );
            }
            else if (def_id == MHITYOU)
            {
                mpr( "%s %ss you%s",
                     ptr_monam( &menv[att_id], DESC_CAP_THE ), msg, punct );
            }
            else
            {
                // Note that ptr_monam uses a single static buffer...
                // thus we have to assemble things in parts.
                snprintf( info, INFO_SIZE, "%s %ss ",
                          ptr_monam( &menv[att_id], DESC_CAP_THE ), msg );
                strncat( info, ptr_monam( &menv[def_id], DESC_NOCAP_THE ), INFO_SIZE );
                strncat( info, punct, INFO_SIZE );

                mpr( info );
            }
        }

#if DEBUG_DIAGNOSTICS
        mpr( MSGCH_DIAGNOSTICS, "brand: %d; base: %d; damage: %d",
             melee_brand, base, specdam );
#endif

        if (def_id != MHITYOU)
        {
            if (hurt_monster( &menv[def_id], specdam ))
                wounded = true;
        }
        else
        {
            ouch( specdam, att_id, KILLED_BY_MONSTER );
            wounded = true;

            if (you.religion == GOD_XOM
                && you.hp <= you.hp_max / 3
                && one_chance_in(10))
            {
                Xom_acts( true, you.xp_level, false );
            }
        }
    }

    return (wounded);
}

static void init_extra_attacks( struct extra_attack_def &ex_att )
{
    ex_att.verb[0] = '\0';
    ex_att.to_hit  = 0;
    ex_att.damage  = 0;
    ex_att.speed   = 3;
    ex_att.brand   = SPWPN_NORMAL;
    ex_att.index   = 0;
}

static bool get_extra_attack( struct extra_attack_def &ex_att )
{
    const int   wpn = get_inv_wielded();
    const bool  bearing_shield = (you.equip[EQ_SHIELD] != -1);
    const bool  ur_armed = (wpn != -1);

    // XXX: add in different moves for brawling vs martial arts
    // const bool brawling = (random2( you.str ) > random2( you.dex ));

    const size_type        size = player_size();

    hands_reqd_type  hands = HANDS_ONE;

    if (ur_armed && !weapon_skill( you.inv[wpn] ) == SK_STAVES)
        hands = hands_reqd( you.inv[wpn], size );

    const bool merfish = (you.species == SP_MERFOLK && player_is_swimming());

    ex_att.verb[0] = '\0';
    ex_att.to_hit  = 0;
    ex_att.damage  = 0;
    ex_att.speed   = 3;
    ex_att.brand   = SPWPN_NORMAL;

    int unarmed_attack = UNAT_HEADBUTT;

    if (you.species != SP_NAGA && !merfish
        && (you.mutation[MUT_HOOVES] || coinflip()))
    {
        unarmed_attack = UNAT_KICK;
    }

    if ((you.attribute[ATTR_TRANSFORMATION] == TRAN_DRAGON
            || player_genus(GENPC_DRACONIAN)
            || merfish
            || you.mutation[MUT_STINGER])
        && one_chance_in(3))
    {
        unarmed_attack = UNAT_TAILSLAP;
    }

    if (random2(60) < skill_bump(SK_UNARMED_COMBAT))
        unarmed_attack = UNAT_PUNCH;

    // XXX: Currently doing this hack with index to get back the
    // old behaviour of only getting at most one of each.
    for (; ex_att.index < 4; ex_att.index++)
    {
        switch (ex_att.index)
        {
        case 0:
            if (unarmed_attack != UNAT_KICK
                && ((you.species != SP_CENTAUR && !you.mutation[MUT_HOOVES])
                    || coinflip()))
            {
                continue;
            }

            if (you.attribute[ATTR_TRANSFORMATION] == TRAN_SERPENT_OF_HELL
                || you.attribute[ATTR_TRANSFORMATION] == TRAN_ICE_BEAST
                || you.attribute[ATTR_TRANSFORMATION] == TRAN_DRAGON
                || you.attribute[ATTR_TRANSFORMATION] == TRAN_SPIDER)
            {
                continue;
            }

            strcpy( ex_att.verb, "kick" );
            ex_att.damage = (you.mutation[MUT_HOOVES] >= 2
                                || you.species == SP_CENTAUR) ? 8 :
                            (you.mutation[MUT_HOOVES] == 1)   ? 6
                                                              : 4;
            break;

        case 1:
            if (unarmed_attack != UNAT_HEADBUTT
                && ((you.species != SP_MINOTAUR
                        && you.species != SP_KENKU
                        && !you.mutation[MUT_HORNS])
                    || !one_chance_in(3)))
            {
                continue;
            }

            if (you.attribute[ATTR_TRANSFORMATION] == TRAN_SERPENT_OF_HELL
                || you.attribute[ATTR_TRANSFORMATION] == TRAN_SPIDER
                || you.attribute[ATTR_TRANSFORMATION] == TRAN_ICE_BEAST
                || you.attribute[ATTR_TRANSFORMATION] == TRAN_DRAGON)
            {
                continue;
            }

            strcpy( ex_att.verb, (you.species == SP_KENKU) ? "peck"
                                                           : "headbutt" );
            ex_att.damage = 3;

            if (you.species == SP_MINOTAUR)
                ex_att.damage += 5;
            else if (you.mutation[MUT_HORNS])
                ex_att.damage += 2 * you.mutation[MUT_HORNS] - 1;
            else
            {
                const int helm = you.equip[EQ_HELMET];
                if (helm != -1 && you.inv[helm].sub_type == ARM_HELMET)
                {
                    ex_att.damage++;

                    const int desc = get_helmet_desc( you.inv[helm] );
                    if (desc == THELM_DESC_SPIKED || desc == THELM_DESC_HORNED)
                        ex_att.damage += 2;
                }
            }
            break;

        case 2:             /* draconians */
            if (unarmed_attack != UNAT_TAILSLAP
                && ((!player_genus(GENPC_DRACONIAN)
                        && (!(you.species == SP_MERFOLK && player_is_swimming()))
                        && !you.mutation[ MUT_STINGER ])
                    || (!one_chance_in(4))))
            {
                continue;
            }

            if (you.attribute[ATTR_TRANSFORMATION] == TRAN_SPIDER
                || you.attribute[ATTR_TRANSFORMATION] == TRAN_ICE_BEAST)
            {
                continue;
            }

            strcpy( ex_att.verb, "tail-slap" );
            ex_att.damage = 5;
            ex_att.speed = 3;

            if (you.mutation[MUT_STINGER] > 0)
            {
                ex_att.damage += 2 * (you.mutation[MUT_STINGER] - 1);
                ex_att.brand  = SPWPN_VENOM;
            }

            /* grey dracs have spiny tails, or something */
            // maybe add this to player messaging {dlb}
            if (you.species == SP_GREY_DRACONIAN && you.xp_level >= 7)
                ex_att.damage += 5;
            break;

        case 3:
            if (unarmed_attack != UNAT_PUNCH
                || bearing_shield
                || (ur_armed && hands == HANDS_TWO)
                || coinflip())
            {
                continue;
            }

            if (you.attribute[ATTR_TRANSFORMATION] == TRAN_SERPENT_OF_HELL
                || you.attribute[ATTR_TRANSFORMATION] == TRAN_SPIDER
                || you.attribute[ATTR_TRANSFORMATION] == TRAN_ICE_BEAST
                || you.attribute[ATTR_TRANSFORMATION] == TRAN_DRAGON)
            {
                continue;
            }

            if (you.attribute[ATTR_TRANSFORMATION] == TRAN_BLADE_HANDS)
            {
                strcpy( ex_att.verb, "slash" );
                ex_att.damage = 7;
            }
            else
            {
                strcpy( ex_att.verb, "punch" );
                ex_att.damage = 3;

                if (you.attribute[ATTR_TRANSFORMATION] == TRAN_STATUE)
                    ex_att.damage += 2;
            }

            /* applied twice */
            ex_att.damage += you.skills[SK_UNARMED_COMBAT] / 3;
            break;

        default:
            break;
        }
    }

    return (ex_att.damage > 0);
}

void you_attack( int def_id, bool unarmed_attacks )
{
    struct monsters *const defender = &menv[def_id];

    int to_hit, damage;
    int temp_rand;              // for probability determination {dlb}
    int damage_done = 0;
    bool wounded = false;

    const int wpn = get_inv_wielded();
    const bool ur_armed = (wpn != -1);   // compacts code a bit {dlb}
    // const bool bearing_shield = (you.equip[EQ_SHIELD] != -1);
    const bool helpless = mons_class_flag( defender->type, M_NO_EXP_GAIN );

    const int dam_type    = player_damage_type();
    const int melee_brand = player_damage_brand();

    // used for special fixed artefact cases:
    const int fixed_special = (ur_armed && is_fixed_artefact( you.inv[wpn] ))
                                        ? you.inv[wpn].special
                                        : static_cast<int>( SPWPN_NORMAL );

    // in order to check if things have changed:
    const int def_x = defender->x;
    const int def_y = defender->y;

    char damage_noise[80], damage_noise2[80];

    char str_pass[ITEMNAME_SIZE];

#if DEBUG_DIAGNOSTICS
    char st_prn[20];
#endif

    if (you.religion == GOD_SHINING_ONE
        && (defender->behaviour == BEH_SLEEP
            || defender->behaviour == BEH_FLEE
            || mons_has_ench( defender, ENCH_CONFUSION ))
        && !mons_is_unholy( defender ))
    {
        if (yesno("Are you sure you want to attack that helpless creature?"))
            did_god_conduct( DID_STABBING, 4 );
        else
            return;
    }

    // fumbling in shallow water <early return>:
    if (player_in_water() && !player_is_swimming() && random2(you.dex) < 4)
    {
        mpr("Unstable footing causes you to fumble your attack.");
        return;
    }

    // calculate heavy armour penalties:
    const int         heavy_equip = div_rand_round( heavy_equip_penalty(), 10 );
    const skill_type  wpn_skill = player_weapon_skill();
    // const hands_reqd_type  hands = (ur_armed ? hands_reqd( you.inv[wpn], player_size() )
    //                                          : HANDS_ONE);

    // Calculate the following flags in advance
    // to know what player does this combat turn:
    bool extra_attacks = false;
    bool hand_and_half = false;
    bool water_attack = false;

    if (you.burden_state == BS_UNENCUMBERED
        && unarmed_attacks
        && random2(20) < you.skills[SK_UNARMED_COMBAT]
        && random2(1 + heavy_equip) < 2)
    {
        extra_attacks = true;
    }

    // if we're not getting potential unarmed attacks, and not wearing a
    // shield, and have a suitable uncursed weapon we get the bonus.
    if (unarmed_attacks && !extra_attacks && check_hand_and_half_style())
        hand_and_half = true;

    if (player_is_swimming()
        // monster not a water creature
        && monster_habitat( defender->type ) != DNGN_DEEP_WATER
        && !mons_class_flag( defender->type, M_AMPHIBIOUS )
        // monster on water grid
        && (grd[defender->x][defender->y] == DNGN_SHALLOW_WATER
            || grd[defender->x][defender->y] == DNGN_DEEP_WATER)
        // monster not flying
        && !mons_flies( defender ))
    {
        water_attack = true;
    }

    // Now handle melee weapon attacking effects (berserk & Wucad Mu):
    if (ur_armed)
    {
        if (you.special_wield == SPWLD_WUCAD_MU && one_chance_in( you.intel ))
        {
            miscast_effect( SPTYP_DIVINATION, random2(9), random2(70), 100,
                            "the Staff of Wucad Mu" );
        }

        // if (get_weapon_brand( you.inv[wpn] ) == SPWPN_DISTORTION
        //     && one_chance_in(100))
        // {
        //     miscast_effect( SPTYP_TRANSLOCATION, random2(9), random2(70), 100,
        //                     "a distortion effect" );
        // }

        if ((you.inv[wpn].base_type == OBJ_WEAPONS
                && is_random_artefact( you.inv[wpn] )
                && !one_chance_in( 1 + you.inv[wpn].ra_props[RAP_ANGRY] ))

            || (you.mutation[MUT_BERSERK]
                && random2(100) < (you.mutation[MUT_BERSERK] * 10) - 5)

            || (you.special_wield == SPWLD_TROG && coinflip()))
        {
            go_berserk(false);
        }
    }

    // calculate our to-hit
    int extra_hit = 5 * water_attack;

    to_hit = your_melee_to_hit( defender, extra_hit, -1, hand_and_half, false );

    // now speed:
    const int speed = your_melee_speed( hand_and_half,
                                        (melee_brand == SPWPN_SPEED), false );

    // Now we're going to commit ourselves:

    // Sure we're tossing away a lot of granularity here, but a little
    // random spread on time_taken for melee is appropriate.
    you.time_taken = div_rand_round( you.time_taken * speed, 10000 );

    if (you.time_taken < 1)
        you.time_taken = 1;

    // energy expenditure in terms of food:
    make_hungry( 3, true );

    if (mons_friendly( defender ))
        did_god_conduct( DID_ATTACK_FRIEND, 5 );

    if (mons_holiness( defender ) == MH_HOLY)
        did_god_conduct( DID_ATTACK_HOLY, defender->hit_dice );

    if (you.pet_target == MHITNOT)
        you.pet_target = def_id;

    const bool  can_block = mons_can_block( defender );
    const int   shield_class = mons_shield_class( defender );

    int con_block = you.dex
                + (2 * skill_bump( SK_SHIELDS ) + skill_bump( SK_FIGHTING )) / 2
                + mons_blocks_penalty( defender );

    // calculate monster's melee EV:
    const bool easy = (defender->behaviour == BEH_SLEEP
                        || mons_has_ench( defender, ENCH_PARALYSIS )
                        || mons_has_ench( defender, ENCH_PETRIFY ));

    const int at_bay = mons_weapon_ev_bonus( defender );
    const int def_ev = defender->evasion + at_bay;
    int margin = 0;

    if (can_block && test_block( shield_class, con_block ))
    {
        mon_msg( defender, " blocks your attack." );
        handle_mon_shield_blocks( defender );
    }
    else if ((easy && !one_chance_in(20))
            || test_hit( to_hit, def_ev, margin ))
    {
        damage = your_melee_damage( -1, hand_and_half, water_attack );
        damage_done = roll_dice( 1, damage );

#if DEBUG_DIAGNOSTICS
        const int roll_damage = damage_done;
#endif

        if (ur_armed && you.inv[wpn].base_type == OBJ_WEAPONS)
        {
            if (!is_range_weapon( you.inv[wpn] )
                && !item_ident( you.inv[wpn], ISFLAG_KNOW_PLUSES )
                && wpn_skill != SK_FIGHTING
                && player_weapon_wielded()
                && random2(100) < you.skills[wpn_skill])
            {
                set_ident_flags( you.inv[wpn], ISFLAG_KNOW_PLUSES );
                set_redraw_status( REDRAW_WIELD );

                in_name( wpn, DESC_NOCAP_A, str_pass );
                mpr( "You are wielding %s.", str_pass );
                more();
            }
        }

        // Keeping this around in this fashion (ie that attacks on sleeping
        // monsters count as stealthy).
        if (defender->behaviour != BEH_SLEEP)
        {
            // ok.. if you didn't backstab, you wake up the neighborhood.
            // I can live with that.
            alert_nearby_monsters();
        }

        const int  crit_type = check_player_crit_hit( defender, true );

        if (crit_type != DAM_BASH)
        {
            // XXX: need to work on this... might want the old style of
            // different amounts for different situations of stabbing
            // (sleeping vs fleeing vs confuses vs whatever).   Might also
            // want use the weapon base damage in here instead of that
            // damage_done.
            int factor = 5;

            if (being_res_damage_type( def_id, crit_type ) < 0)
                factor += 5;

            if (crit_type == DAM_PIERCE && wpn_skill == SK_SHORT_BLADES)
            {
                factor += you.dex / 3;

                if (ur_armed
                    && you.inv[wpn].base_type == OBJ_WEAPONS
                    && you.inv[wpn].sub_type == WPN_DAGGER)
                {
                    factor += skill_bump( SK_STABBING );
                }
            }

            damage_done += random2( damage_done * factor ) / 10;
        }

        // XXX: perhaps we want a way to handle partial armour piercing
        // like we used to do (ie a skill/stat reduction against AC)...
        // but for now we'll just make some attacks go straight through,
        // and the others count as "energy" (so they'll hit a "soft" spot).
        if (crit_type != DAM_PIERCE || !one_chance_in(3))
        {
            damage_done = apply_mons_armour( damage_done, defender,
                                             (crit_type == DAM_PIERCE) );
        }

        if (damage_done < 0)
            damage_done = 0;

#if DEBUG_DIAGNOSTICS
        mpr( MSGCH_DIAGNOSTICS, "damage: %d; rolled: %d; post AC: %d",
             damage, roll_damage, damage_done );
#endif

        // Describe attack:
        get_attack_description( damage_done, damage_noise, damage_noise2 );

        snprintf( info, INFO_SIZE, "You %s %s%s",
                    damage_noise,
                    ptr_monam( defender, DESC_NOCAP_THE ),
                    damage_noise2 );

        // apply damage and exercise skills
        if (hurt_monster( defender, damage_done ))
        {
            wounded = true;

#if DEBUG_DIAGNOSTICS
            strcat( info, " for " );
            /* note: doesn't take account of special weapons etc */
            itoa( damage_done, st_prn, 10 );
            strcat( info, st_prn );
#endif
            strncat( info, damage_punctuation( damage_done ), INFO_SIZE );
            mpr(info);

            if (ur_armed && wpn_skill != SK_FIGHTING)
            {
                if (!helpless || you.skills[wpn_skill] < 2)
                    exercise( wpn_skill, 1 );
            }
            else
            {
                if (!helpless || you.skills[SK_UNARMED_COMBAT] < 2)
                    exercise( SK_UNARMED_COMBAT, 1 );
            }

            if ((!helpless || you.skills[SK_FIGHTING] < 2)
                && one_chance_in(3))
            {
                exercise( SK_FIGHTING, 1 );
            }

            if (crit_type != DAM_BASH)
            {
                switch (crit_type)
                {
                case DAM_BLUDGEON:
                    mons_add_ench( defender, ENCH_STUN, MHITYOU );
                    mpr( "%s is stunned!",
                         ptr_monam( defender, DESC_CAP_THE ) );
                    break;

                case DAM_SLICE:
                    mons_add_ench( defender, ENCH_CUT, MHITYOU );
                    mpr( "%s is wounded!",
                         ptr_monam( defender, DESC_CAP_THE ) );
                    break;

                case DAM_PIERCE:
                    // armour piercing applied above
                    mpr( "You stab %s in a vulnerable spot!",
                         ptr_monam( defender, DESC_NOCAP_THE ) );
                    break;

                default:
                    break;
                }

                exercise( SK_STABBING, 2 + random2(3) );
            }

            // check for kill
            if (defender->hit_points < 1)
            {
                if (ur_armed && melee_brand == SPWPN_VAMPIRICISM)
                    vampiric_heal( MHITYOU, def_id, damage_done, fixed_special );

                monster_die( defender, KILL_YOU, 0 );
                return;
            }
        }
        else // no damage was done
        {
            if (player_monster_visible( defender ))
                strcat( info, ", but do no damage." );
            else
                strcat( info, "." );

            mpr(info);
        }

        // splash acid
        if (defender->type == MONS_JELLY
            || defender->type == MONS_BROWN_OOZE
            || defender->type == MONS_ACID_BLOB
            || defender->type == MONS_ROYAL_JELLY)
        {
            weapon_acid(5);  // also does gloves and hands
        }

        // conducts
        if (ur_armed)
        {
            if (you.special_wield == SPWLD_TORMENT)
            {
                torment( you.x_pos, you.y_pos );
                did_god_conduct( DID_NECROMANCY, 5 );
            }
            else if (you.special_wield == SPWLD_ZONGULDROK
                    || you.special_wield == SPWLD_CURSE)
            {
                did_god_conduct( DID_NECROMANCY, 3 );
            }

            if (is_demonic( you.inv[wpn] ))
                did_god_conduct( DID_UNHOLY, 1 );
        }

        // always upset monster regardless of damage
        behaviour_event( defender, ME_WHACK, MHITYOU );

        bool skip_special = false;

        if (defender->type == MONS_HYDRA && damage_done > 0)
        {
            if (dam_type == DAM_SLICE
                && (melee_brand == SPWPN_VORPAL
                    || (damage_done >= 4 && coinflip())))
            {
                defender->number--;

                temp_rand = random2(4);
                const char *const verb = (temp_rand == 0) ? "slice" :
                                         (temp_rand == 1) ? "lop" :
                                         (temp_rand == 2) ? "chop"
                                                          : "hack";

                if (defender->number < 1)
                {
                    mpr( "You %s %s's last head off!",
                          verb, ptr_monam( defender, DESC_NOCAP_THE ) );

                    defender->hit_points = -1;
                }
                else
                {
                    mpr( "You %s one of %s's heads off!",
                          verb, ptr_monam(defender, DESC_NOCAP_THE) );

                    if (melee_brand == SPWPN_FLAMING)
                        mpr( "The flame cauterises the wound!" );
                    else if (defender->number < 19)
                    {
                        mon_msg( defender, " grows two more!" );
                        defender->number += 2;
                        // XXX: actually cancel attack fully?
                        heal_monster( defender, 8 + random2(8), true );
                    }
                }

                // if the hydra looses a head:
                // - it's dead if it has none remaining (HP set to -1)
                // - flame used to cauterise doesn't do extra damage
                // - ego weapons do their additional damage to the
                //   hydra's decapitated head, so it's ignored.
                //
                // ... and so we skip the special damage.
                skip_special = true;
            }
        }

        // handle special brand damage
        if (!skip_special && damage_done > 0)
        {
            // XXX: use damage_done here, or the base damage of the weapon?
            // - if base damage then we'll probably want to add a small
            //   constant to the damage "multiples" in do_ego_damage.
            do_ego_damage( damage_done, melee_brand, dam_type,
                           MHITYOU, def_id, fixed_special );
        }
    }
    else // failed block and failed to hit
    {
        // upset only non-sleeping monsters if we missed
        if (defender->behaviour != BEH_SLEEP)
            behaviour_event( defender, ME_WHACK, MHITYOU );

        // XXX: return message about heavy equipment causing miss here?

        if (-margin <= at_bay)
        {
            extra_attacks = false;

            snprintf( info, INFO_SIZE, "%s holds you at bay",
                                       ptr_monam( defender, DESC_CAP_THE ) );

            if (defender->inv[MSLOT_WEAPON] != NON_ITEM
                && player_monster_visible( defender ))
            {
                it_name( defender->inv[MSLOT_WEAPON], DESC_NOCAP_A, str_pass );

                strncat( info, " with ", INFO_SIZE );
                strncat( info, str_pass, INFO_SIZE );
            }

            strncat( info, ".", INFO_SIZE );

            mpr( info );
        }
        else
        {
            mon_msg( defender, "You miss %s.", false );
        }
    }

    if (defender->hit_points < 1 || defender->hit_dice < 1)
    {
        monster_die( defender, KILL_YOU, 0 );
        return;
    }

    // handle extra unarmed attacks (must check if monster still there!)
    if (extra_attacks && to_hit > 0
        && def_x == defender->x && def_y == defender->y)
    {
        const int max_speed = player_base_time_taken();
        struct extra_attack_def  attack;

        init_extra_attacks( attack );

        while (get_extra_attack( attack ))
        {
            // Calculate extra attacks to-hit (heavy armour doubly applied)
            // -- attack.to_hit is NOT added to the running total
            extra_hit -= heavy_equip;
            to_hit = your_melee_to_hit( defender, extra_hit + attack.to_hit,
                                        to_hit, false, false );

#if DEBUG_DIAGNOSTICS
            mpr( MSGCH_DIAGNOSTICS, "extra hit: %d; to-hit: %d",
                  extra_hit, to_hit );
#endif

            // If to_hit has becomes difficult, then no extra attack.
            // Note: This is actually a bonus to the player, as extra
            //       attacks cost food.  -- bwr
            const int min_to_hit = (def_ev * 3) / 2;
            if (to_hit < min_to_hit || you.time_taken > max_speed)
                break;

            alert_nearby_monsters();
            make_hungry( 2, true );

            you.time_taken += attack.speed;

            con_block = you.dex
                    + (2 * you.skills[SK_SHIELDS] + you.skills[SK_FIGHTING]) / 3
                    + mons_blocks_penalty( defender );

            if (can_block && test_block( shield_class, con_block ))
            {
                mon_msg( defender, " blocks your attack.", false );
                handle_mon_shield_blocks( defender );
            }
            else if (test_hit( to_hit, def_ev ))
            {
                damage = your_melee_damage( attack.damage, false, false );
                damage_done = roll_dice( 1, damage );

#if DEBUG_DIAGNOSTICS
                const int roll_damage = damage_done;
#endif

                damage_done = apply_mons_armour( damage_done, defender );

#if DEBUG_DIAGNOSTICS
                mpr( MSGCH_DIAGNOSTICS,
                    "damage: %d, rolled: %d, post AC: %d",
                     damage, roll_damage, damage_done );
#endif

                if (damage_done < 1)
                    damage_done = 0;

                if (hurt_monster( defender, damage_done ))
                {
                    wounded = true;

                    if ((!helpless || you.skills[SK_FIGHTING] < 2)
                        && one_chance_in(5))
                    {
                        exercise( SK_FIGHTING, 1 );
                    }

                    if (!helpless || you.skills[SK_UNARMED_COMBAT] < 2)
                        exercise( SK_UNARMED_COMBAT, 1 );

                    snprintf( info, INFO_SIZE, "You %s %s",
                              attack.verb, ptr_monam(defender, DESC_NOCAP_THE) );

#if DEBUG_DIAGNOSTICS
                    strcat( info, " for " );
                    itoa( damage_done, st_prn, 10 );
                    strcat( info, st_prn );
#endif

                    strcat( info, damage_punctuation( damage_done ) );
                    mpr(info);

                    // only one brand to do:
                    if (attack.brand == SPWPN_VENOM && coinflip())
                        poison_monster( defender, true );
                }
                else // no damage was done
                {
                    snprintf( info, INFO_SIZE, "You %s %s", attack.verb,
                              ptr_monam(defender, DESC_NOCAP_THE) );

                    if (player_monster_visible( defender ))
                        strcat( info, ", but do no damage." );
                    else
                        strcat( info, "." );

                    mpr(info);
                }

                if (defender->hit_points < 1)
                {
                    monster_die( defender, KILL_YOU, 0 );
                    return;
                }
            }
            else
            {
                mpr( "Your %s misses %s.",
                      attack.verb, ptr_monam(defender, DESC_NOCAP_THE) );
            }
        }
    }

    if (wounded)
        print_wounds( defender );

#if DEBUG_DIAGNOSTICS
    mpr( MSGCH_DIAGNOSTICS, "time_taken: %d", you.time_taken );
#endif

    return;
}                               // end you_attack()

// XXX: still needs work
int apply_player_armour( int damage_size, int damage_taken, bool energy )
{
    const int ac = player_armour_class();

    if (ac > 0)
    {
        int damage_reduction = random2(ac + 1);

        if (!energy && !player_light_armour())
        {
            const int arm = you.equip[EQ_BODY_ARMOUR];
            const int body_arm_ac = (arm == -1) ? 0 : property( you.inv[arm], PARM_AC );

            int percent = 2 * (you.skills[SK_ARMOUR] + body_arm_ac);

            if (percent > 50)
                percent = 50;

            int min = 1 + (damage_size * percent) / 100;

            if (min > ac / 2)
                min = ac / 2;

            if (damage_reduction < min)
                damage_reduction = min;
        }

        damage_taken -= damage_reduction;
    }

    return (damage_taken);
}

// XXX: this needs work and comparison with the player code.
static bool apply_attack_flavour_monster( struct monsters *const def,
                                          struct monsters *const att,
                                          const mon_attack_def &attack,
                                          int &damage, bool sees )
{
    int specdam = 0;

    switch (attack.flavour)
    {
    case AF_POISON:
    case AF_POISON_MEDIUM:
    case AF_POISON_STR:
    case AF_POISON_NASTY:
    case AF_POISON_STRONG:
        if (mons_res_poison( def ) < 1
            && (attack.flavour == AF_POISON_NASTY
                || one_chance_in( 15 + 5 * (attack.flavour == AF_POISON) )
                || (damage > 2
                    && one_chance_in( (attack.flavour == AF_POISON) ? 4 : 3 ))))
        {
            poison_monster( def, false, 1 + (attack.flavour == AF_POISON_STRONG) );
        }
        break;

    case AF_ACID:
        if (att->type == MONS_SPINY_WORM)
            poison_monster( def, false );

        if (damage > 0 && !mons_res_acid( def ))
        {
            if (sees)
                mon_msg( def, " is splashed with acid." );

            specdam = roll_dice(2, 3);
        }
        break;

    case AF_ROT:
    case AF_DISEASE:
        if (mons_res_negative_energy( def ))
            break;

        if (one_chance_in(20)
            || (damage > 2 && one_chance_in(3)))
        {
            def->max_hit_points -= 1 + random2(3);

            if (sees)
                mon_msg( def, " looks unhealthy." );

            if (def->hit_points > def->max_hit_points)
                def->hit_points = def->max_hit_points;

            if (attack.flavour == AF_ROT)
                mons_inc_ench_levels(def, ENCH_ROT, MHITNOT, 1);
        }
        break;

    case AF_FIRE:
        if (att->type == MONS_FIRE_VORTEX)
            att->hit_points = -10;

        if (mons_res_fire(def) == 0)
            specdam = 15 + random2(15);
        else if (mons_res_fire(def) < 0)
            specdam = 20 + random2(25);

        if (specdam && sees)
            mon_msg(def, " is engulfed in flame!");
        break;

    case AF_PARALYSE:
        if (att->type == MONS_RED_WASP)
            poison_monster( def, false );

        if (mons_holiness( def ) == MH_NATURAL
            && !mons_res_poison( def )
            && !mons_has_ench( def, ENCH_PARALYSIS )
            && (one_chance_in(20)
                || (damage > 2 && one_chance_in(4))))
        {
            if (mons_add_ench(def, ENCH_PARALYSIS, monster_index(att)) && sees)
                mon_msg( def, " suddenly stops moving!" );
        }
        break;

    case AF_CONFUSE:
        if (attack.type == AT_SPORE)
        {
            if (mons_res_poison( def ))
                break;
            else
            {
                att->hit_dice--;

                if (sees)
                    mon_msg( att, " releases a cloud of spores." );
            }
        }

        if (att->hit_dice > 3
            && (one_chance_in(10)
                || (damage > 2 && one_chance_in(3))))
        {
            if (mons_add_ench(def, ENCH_CONFUSION, monster_index(att)) && sees)
                mon_msg( def, " appears confused." );
        }
        break;

    case AF_DRAIN_DEX:
    case AF_DRAIN_STR:
    case AF_DRAIN_XP:
        if (mons_res_negative_energy( def ))
            break;

        if (att->type == MONS_SPECTRAL_THING && coinflip())
            break;

        if (one_chance_in(30) || (damage > 5 && coinflip()))
        {
            drain_monster( def, !sees );

            if (def->hit_points < 1 || def->hit_dice < 1)
            {
                monster_die( def, KILL_MON, monster_index(att) );
                return (true);
            }
        }
        break;

    case AF_COLD:
        if (mons_res_cold(def) == 0)
            specdam = att->hit_dice + random2( 2 * att->hit_dice );
        else if (mons_res_cold(def) < 0)
            specdam = att->hit_dice + random2( 3 * att->hit_dice );

        if (specdam && sees)
            mon_msg( def, " is frozen!" );
        break;

    case AF_ELEC:
        if (!mons_flies(def) && !mons_res_elec(def))
            specdam = att->hit_dice + random2( 2 * att->hit_dice );

        if (specdam && sees)
            mon_msg( def, " is shocked!" );
        break;

    case AF_VAMPIRIC:
        if (mons_res_negative_energy(def) > random2(3))
            break;

        // heh heh {dlb}
        if (heal_monster( att, random2( damage ), true ) && sees)
            mon_msg( att, " is healed." );
        break;

    case AF_BLINK:
        if (one_chance_in(3))
            monster_blink( att, false, !sees );
        break;

    default:
        break;
    }

#ifdef DEBUG_DIAGNOSTICS
    if (specdam)
        mpr( MSGCH_DIAGNOSTICS, "flavour damage = %d", specdam );
#endif

    damage += specdam;

    return (false);
}

static void apply_attack_flavour_player( struct monsters *const att,
                                         const mon_attack_def &attack,
                                         int &damage )
{
    int res = 0;
    int extra = 0;

    switch (attack.flavour)
    {
    case AF_POISON:
    case AF_POISON_NASTY:
    case AF_POISON_MEDIUM:
    case AF_POISON_STRONG:
        // If this looks messy it's because we're duplicating what
        // used to be 6 different pieces of poisoning code, with
        // exceptions.
        res = player_res_poison();

        if (res >= 3
            || (res >= 2 && attack.flavour == AF_POISON_MEDIUM)
            || (res >= 1 && attack.flavour == AF_POISON))
        {
            break;
        }

        if (attack.flavour == AF_POISON_NASTY
            || one_chance_in( 15 + 5 * (attack.flavour == AF_POISON) )
            || (damage > 2
                && one_chance_in( (attack.flavour == AF_POISON) ? 4 : 3 )))
        {
            if (attack.type == AT_BITE || attack.type == AT_STING)
            {
                snprintf( info, INFO_SIZE, "'s %s was poisonous!",
                          (attack.type == AT_BITE) ? "bite" : "sting" );

                mon_msg( att, info );
            }
            else
            {
                strcpy( info, ptr_monam( att, DESC_CAP_THE ) );
                strcat( info, " poisons you!" );
                mpr( info );
            }

            int amount = 1;

            if (attack.flavour == AF_POISON_NASTY)
                amount++;
            else if (attack.flavour == AF_POISON_MEDIUM)
                amount += random2(3);
            else if (attack.flavour == AF_POISON_STRONG)
                amount += roll_dice(2,5);

            poison_player( amount );
        }
        break;

    case AF_POISON_STR:
        res = player_res_poison();
        if (res < 3)
        {
            poison_player( roll_dice(1,3) );

            if (res < 2)
                lose_stat( STAT_STRENGTH, 1 );
        }
        break;

    case AF_ROT:
        if (you.is_undead)
            break;

        // both sides call random2() - looking familiar by now {dlb}
        if (one_chance_in(20) || (damage > 2 && one_chance_in(3)))
        {
            rot_player( 2 + random2(3) );

            if (damage > 5)
                rot_hp(1);
        }

        if (one_chance_in(4))
            disease_player( 50 + random2(100) );
        break;

    case AF_DISEASE:
        if (!one_chance_in(3))
            disease_player( 50 + random2(100) );
        break;

    case AF_FIRE:
        if (att->type == MONS_FIRE_VORTEX)
            att->hit_points = -10;

        strcpy( info, "You are engulfed in flames" );

        extra = 15 + random2(15);

        res = player_res_fire();
        if (res > 0)
            extra /= (1 + res * res);
        else if (res < 0)
            extra += 8 + random2(8);

        strcat( info, damage_punctuation( extra ) );
        mpr( info );

        expose_player_to_element( BEAM_FIRE, 2 );
        break;

    case AF_CONFUSE:
        if (attack.type == AT_SPORE)
        {
            if (player_res_poison() >= 3)
                break;
            else
            {
                att->hit_dice--;
                mpr( "You are engulfed in a cloud of spores!" );
            }
        }

        if (att->hit_dice > 3
            && (one_chance_in(10)
                || (damage > 2 && one_chance_in(3))))
        {
            confuse_player( 1 + random2( 3 + att->hit_dice ) );
        }
        break;

    case AF_DRAIN_XP:
        if (att->type == MONS_SPECTRAL_THING && coinflip())
            break;

        if (one_chance_in(30) || (damage > 5 && coinflip()))
            drain_exp();
        break;


    case AF_PARALYSE:
        // NOTE: This currently assumes wasp stings, therefore poison resists!
        res = player_res_poison();

        if (att->type == MONS_RED_WASP)
        {
            if (res < 3)
                poison_player( (coinflip() ? 2 : 1) );
        }

        // Rasied the damage threshold to 4 and reduces that
        // frequency to 1/3 instead of 2/3.  Wasps are less
        // of an instadeath for the non-poison-resistant, but
        // still potentially a really big threat. --bwr
        if (res < 2)
        {
            if (one_chance_in( (res < 1 && damage > 4) ? 3 : 20))
                paralyse_player( roll_dice(1, 3) );
            else if ((res == 0 || damage > 4) && coinflip())
                slow_player( roll_dice(1, 3) );
        }
        break;

    case AF_ACID:
        if (att->type == MONS_SPINY_WORM)
        {
            if (player_res_poison() < 3)
                poison_player( 2 + random2(4) );
        }

        splash_with_acid(3);
        break;

    case AF_COLD:
        extra = att->hit_dice + random2(att->hit_dice * 2);

        res = player_res_cold();
        if (res > 0)
            extra /= (1 + res * res);
        else if (res < 0)
            extra += att->hit_dice + random2( 2 * att->hit_dice );

        if (extra > 4)
        {
            strcpy( info, ptr_monam( att, DESC_CAP_THE ) );
            if (extra < 10)
                strcat( info, " chills you." );
            else
                strcat( info, " freezes you!" );

            if (extra > 19)
                strcat( info, "!" );

            mpr(info);
        }

        expose_player_to_element( BEAM_COLD, 2 );
        break;

    case AF_ELEC:
        if (player_res_electricity() <= 0)
        {
            extra = att->hit_dice + random2( 2 * att->hit_dice );
            mon_msg( att, " shocks you!" );
        }
        break;

    case AF_VAMPIRIC:
        if (you.is_undead || player_prot_life() > random2(3))
            break;

        if (heal_monster( att, damage / 2 + random2( damage / 2 ), true ))
        {
            mon_msg( att, " draws strength from your injuries!" );

            // restoring old HP drain -- bwr
            if ((damage > 6 && one_chance_in(3)) || one_chance_in(20))
            {
                mpr( MSGCH_WARN, "You feel less resilient." );
                dec_max_hp( coinflip() ? 2 : 1 );
            }
        }
        break;

    case AF_DRAIN_STR:
        if (damage > 0 && player_prot_life() < random2(4))
            lose_stat( STAT_STRENGTH, 1 );
        break;

    case AF_DRAIN_DEX:
        if (damage > 0 && player_res_poison() < random2(4))
            lose_stat( STAT_DEXTERITY, 1 );
        break;

    case AF_HUNGER:
        if (you.is_undead == US_UNDEAD)
            break;

        if (one_chance_in(20) || (damage > 0 && coinflip()))
            make_hungry( 400, false );
        break;

    case AF_BLINK:
        if (one_chance_in(3))
            monster_blink( att, false );
        break;

    case AF_MUTATE:
    case AF_BAD_MUTATE:
        if (one_chance_in(3))
        {
            if (attack.flavour == AF_BAD_MUTATE || !one_chance_in(5))
                give_bad_mutation();
            else
                mutate( PICK_RANDOM_MUTATION );
        }
        break;


    default:
        break;
    }                   // end of switch for special attacks.

    damage += extra;

#ifdef DEBUG_DIAGNOSTICS
    if (extra)
        mpr( MSGCH_DIAGNOSTICS, "flavour damage = %d", extra );
#endif

    return;
}

static inline bool apply_attack_flavour_being( int def_id, int att_id,
                                               const mon_attack_def &attack,
                                               int &damage, bool sees )
{
    ASSERT( def_id != MHITNOT );

    bool ret = false;

    if (def_id == MHITYOU)
        apply_attack_flavour_player( &menv[att_id], attack, damage );
    else
    {
        // returns if monster dead, stop function:
        ret = apply_attack_flavour_monster( &menv[def_id], &menv[att_id],
                                            attack, damage, sees );
    }

    return (ret);
}

bool monster_attack( int att_id, int def_id )
{
    struct monsters *const att = &menv[att_id];
    struct monsters *const def = (def_id != MHITYOU) ? &menv[def_id] : 0;

    char def_str[ ITEMNAME_SIZE ];
    strncpy( def_str, ptr_being_name(def_id, DESC_NOCAP_THE), sizeof(def_str) );

    int damage_taken = 0;
    // bool hit = false;

    // Factors for blocking
    const bool can_block    = being_can_block( def_id, att_id );
    const int  shield_class = being_shield_class( def_id );
    const bool can_see      = being_visible( att_id, def_id );
    const bool is_friendly  = beings_aligned( att_id, def_id );
    const int  att_habitat  = monster_habitat( att->type );

    // so we can check if the player moved
    // a copy of defender (x,y)
    const int old_x = (!def) ? you.x_pos : def->x;
    const int old_y = (!def) ? you.y_pos : def->y;

    // references to the current defender (x,y)
    const int &ref_x = (!def) ? you.x_pos : def->x;
    const int &ref_y = (!def) ? you.y_pos : def->y;

    // Player can see this fight (ie gets messages)...
    // of course, if the PC is the target (!def), it's true.
    const bool sees = (!def) ? true : (mons_near(att) && mons_near(def));

    int heads = 0;                 // for hydras {dlb}
    int hand_used = MSLOT_WEAPON;
    char str_pass[ ITEMNAME_SIZE ];

    // don't attack ourselves
    if (att_id == def_id)
        return (false);

    // don't attack if submerged, or opponent submerged
    if (mons_is_submerged( att ) || (def && mons_is_submerged( def )))
        return (false);

    if (is_friendly && can_see && !mons_is_confused( att ))
        return (false);

    // monster doesn't have a physical attack
    if (!mons_damage( att, 0 ))
        return (false);

    if (att->type == MONS_GIANT_SPORE || att->type == MONS_BALL_LIGHTNING)
    {
        att->hit_points = -1;
        return (false);
    }

    // tell hostile monsters that they bumped into someone
    if (!is_friendly)
        behaviour_event( att, ME_ALERT, def_id );

    if (!def)
    {
        // This should happen after the mons_friendly check so we're
        // only disturbed by hostiles. -- bwr
        if (you_are_delayed())
            stop_delay();

        // if a friend wants to help,  they can attack <att_id>
        if (you.pet_target == MHITNOT)
            you.pet_target = att_id;

        if (you.duration[DUR_REPEL_UNDEAD]
            && mons_holiness( att ) == MH_UNDEAD
            && !check_mons_resist_magic( att, you.piety ))
        {
            mon_msg( att, " tries to attack you, but is repelled." );
            return (true);
        }

        if (player_has_warding())
        {
            // should be scaled {dlb}
            if (mons_is_summoned( att ) && coinflip())
            {
                mon_msg( att, " tries to attack you, but flinches away." );
                return (true);
            }
        }
    }

    // Calculate water attacks:
    // (1) attacker is non-native to water
    if (grd[att->x][att->y] == DNGN_SHALLOW_WATER
        && !mons_flies( att )
        && !mons_class_flag( att->type, M_AMPHIBIOUS )
        && att_habitat == DNGN_FLOOR
        && one_chance_in(4))
    {
        mon_msg( att, " splashes around in the water.", false );
        return (true);
    }

    // (2) attacker is native and attacking non-native defender
    bool water_attack = false;

    if (att_habitat == DNGN_DEEP_WATER)
    {
        if (!def)
            water_attack = (player_in_water() && !player_is_swimming());
        else
        {
            water_attack = (grd[def->x][def->y] == DNGN_SHALLOW_WATER
                            && !mons_flies(def)
                            && !mons_class_flag( def->type, M_AMPHIBIOUS )
                            && monster_habitat( def->type ) == DNGN_FLOOR);
        }

        if (water_attack)
            mon_msg( att, " uses the watery terrain to its advantage." );
    }

    if (att->type == MONS_HYDRA)
        heads = att->number;

    const int attack_cost = mons_attack_cost(att);

    // Adjust time taken if monster used weapon (Note that 10 energy
    // has automatically been taken off already).
    att->energy -= (attack_cost - 10);

#if DEBUG_DIAGNOSTICS
    mpr( MSGCH_DIAGNOSTICS, "attack cost: %d", attack_cost );
#endif

    bool done = false;

    for (int runthru = 0; runthru < 4 && !done; runthru++)
    {
        // if monster moves (ie distortion) our attacks stop
        if (old_x != ref_x || old_y != ref_y)
            break;

        if (att->type == MONS_HYDRA)
        {
            if (heads < 1)
                break;

            runthru = 0;
            heads--;
        }
        else if (att->type == MONS_TWO_HEADED_OGRE || att->type == MONS_ETTIN)
        {
            // The much overloaded second hand slot.
            if (runthru == 1)
                hand_used = MSLOT_MISSILE;
        }

        bool blocked = false;

        struct mon_attack_def  attack;
        get_mons_attack( att, runthru, attack );

        if (attack.type == AT_NONE)
            break;

        const int wpn = att->inv[hand_used];
        const bool using_weapon = (attack.type == AT_HIT
                                    && wpn != NON_ITEM
                                    && mitm[wpn].base_type == OBJ_WEAPONS
                                    && !is_range_weapon( mitm[wpn] ));

        // Penalty from multiple blocks in this round:
        const int def_block_pen = being_blocks_penalty( def_id );


        // Factors against blocking
        // using base evasion as "dex" and HD as "skill"
        const int con_block = mons_class_evasion( att->type )
                                + att->hit_dice
                                + def_block_pen;

        const int hd_mult = (mons_class_flag( att->type, M_FIGHTER )) ? 10 : 6;
        int mons_to_hit = 20 + hd_mult * att->hit_dice;

        const int dam_type = mons_damage_type( att, attack, hand_used );
        const brand_type wpn_brand = mons_weapon_brand( att, hand_used );

        const int fixed_special = (using_weapon && is_fixed_artefact(mitm[wpn]))
                                        ? you.inv[wpn].special
                                        : static_cast<int>( SPWPN_NORMAL );


        if (water_attack)
            mons_to_hit += 5;

        if (using_weapon)
        {
            mons_to_hit += mitm[wpn].plus;
            mons_to_hit += property( mitm[wpn], PWPN_HIT );
        }

        if (!can_see)
            mons_to_hit -= 5;

        if (mons_has_ench( att, ENCH_STUN ))
            mons_to_hit -= 10;

        if (!def && you_are_delayed())
            mons_to_hit += 5;

        // calculate player's melee EV (appling weapon bonus):
        const int at_bay = being_weapon_ev_bonus( def_id, att_id );
        const int def_ev = at_bay + being_evasion( def_id );

        int margin = 0;

        if (can_block && test_block( shield_class, con_block ))
        {
            blocked = true;
            // hit = false;

            if (!def)
            {
                mpr( "You block %s's attack.", ptr_monam( att, DESC_NOCAP_THE ) );

                // XXX: acid attack on shield for corroding melee attacks?
                int dam = attack.damage;

                if (using_weapon)
                    dam += property( mitm[wpn], PWPN_DAMAGE );

                handle_player_shield_blocks( dam, coinflip() ? 1 : 2 );
            }
            else
            {
                handle_mon_shield_blocks( def );

                if (sees)
                {
                    snprintf( info, INFO_SIZE, " blocks %s's attack.",
                                               ptr_monam( att, DESC_NOCAP_THE ) );

                    mon_msg( def, info );
                }
            }
        }
        else if (test_hit( mons_to_hit, def_ev, margin ))
        {
            bool dodge = false;
            int damage_size = 0;

            if (using_weapon)
            {
                damage_size = property( mitm[wpn], PWPN_DAMAGE );
                damage_taken = random2( damage_size );

                if (get_equip_race( mitm[wpn] ) == ISFLAG_ORCISH
                    && mons_species( att->type ) == MONS_ORC
                    && coinflip())
                {
                    damage_taken++;
                }

                if (mitm[wpn].plus2 >= 0)
                    damage_taken += random2( 1 + mitm[wpn].plus2 );
                else
                    damage_taken -= random2( 1 + abs(mitm[wpn].plus2) );

                damage_taken -= 1 + random2(3);
            }

            damage_size += attack.damage;
            damage_taken += roll_dice( 1, attack.damage );

            if (water_attack)
                damage_taken += roll_dice( 2, attack.damage );

            const int crit_type = check_mons_crit_hit( att_id, def_id, dam_type );
            if (crit_type != DAM_BASH)
            {
                int factor = 5 + 5 * (being_res_damage_type( def_id, crit_type ) < 0);
                if (mons_class_flag( att->type, M_FIGHTER ))
                    factor += 5 + att->hit_dice;

                damage_taken += random2( damage_taken * factor ) / 10;
            }

            if (crit_type != DAM_PIERCE || !one_chance_in(3))
            {
                damage_taken = apply_being_armour( def_id, damage_size, damage_taken,
                                                  (crit_type == DAM_PIERCE) );
            }

            if (!def)
            {
                const int ev = player_evasion();
                const int ev_roll = random2(ev);

#if DEBUG_DIAGNOSTICS
                mpr( MSGCH_DIAGNOSTICS, "margin: %d, ev_roll: %d, dodge:%s",
                     margin, ev_roll, (ev_roll > margin) ? "true" : "false" );
#endif

                if (ev_roll > margin && random2(100) < you.skills[SK_DODGING])
                {
                    const int dam_reduction = random2(ev);

#if DEBUG_DIAGNOSTICS
                    mpr( MSGCH_DIAGNOSTICS, "dodging reduces damage %d", dam_reduction );
#endif
                    damage_taken -= dam_reduction;
                    dodge = true;
                }

                exercise_dodging( 1, 4 );
            }

            // output message if player sees the combat:
            if (sees)
            {
                const bool vis = player_monster_visible( att );

                if (damage_taken <= 0 && (vis || !def))
                {

                    snprintf( info, INFO_SIZE, " %s %s, %s.",
                              get_attack_verb( attack.type ),
                              def_str,
                              (dodge) ? "but you dodged the blow"
                                      : "but doesn't do any damage" );

                    mon_msg( att, info );
                }
                else
                {
                    snprintf( info, INFO_SIZE, "%s %s %s",
                              ptr_monam( att, DESC_CAP_THE ),
                              get_attack_verb( attack.type ),
                              def_str );

                    if (using_weapon
                        && vis
                        && att->type != MONS_DANCING_WEAPON)
                    {
                        strncat( info, " with ", INFO_SIZE );
                        it_name( wpn, DESC_NOCAP_A, str_pass );
                        strncat( info, str_pass, INFO_SIZE );
                    }

#if DEBUG_DIAGNOSTICS
                    char st_prn[20];

                    strncat( info, " for ", INFO_SIZE );
                    // note: doesn't take account of special weapons etc
                    itoa( damage_taken, st_prn, 10 );
                    strncat( info, st_prn, INFO_SIZE );
#endif

                    if (vis || !def)
                        strncat( info, damage_punctuation( damage_taken ), INFO_SIZE );
                    else
                        strncat( info, ".", INFO_SIZE );

                    mpr( info );

                    // because the message might be quite long:
                    if (dodge)
                        mpr( "You managed to dodge part of the blow!" );
                }
            }

            if (damage_taken > 0)
            {
                if (attack.type == AT_HIT && attack.flavour == AF_PLAIN)
                {
                    // XXX: use damage_done  or the base damage of the weapon?
                    // - if base damage then we'll probably want to add a small
                    //   constant to the damage "multiples" in do_ego_damage.
                    do_ego_damage( damage_taken, wpn_brand, dam_type,
                                   att_id, def_id, fixed_special );
                }
                else if (attack.flavour != AF_PLAIN)
                {
                    // This should be after the standard attack message for
                    // proper message ordering.
                    // Returns if monster dead, so we can stop attacking.
                    if (apply_attack_flavour_being( def_id, att_id, attack,
                                                    damage_taken, sees ))
                    {
                        return (true);
                    }
                }

                // do the damage
                if (def)
                {
                    if (crit_type != DAM_BASH)
                    {
                        switch (crit_type)
                        {
                        case DAM_BLUDGEON:
                            mons_add_ench( def, ENCH_STUN, MHITYOU );
                            mpr( "%s is stunned!",
                                 ptr_monam( def, DESC_CAP_THE ) );
                            break;

                        case DAM_SLICE:
                            mons_add_ench( def, ENCH_CUT, MHITYOU );
                            mpr( "%s is wounded!",
                                 ptr_monam( def, DESC_CAP_THE ) );
                            break;

                        case DAM_PIERCE:
                            // armour piercing applied above
                            mpr( "%s was stabbed in a vulnerable spot!",
                                 ptr_monam( def, DESC_NOCAP_THE ) );
                            break;

                        default:
                            break;
                        }
                    }

                    if (hurt_monster_to_kill( def, damage_taken, KILL_MON, att_id ))
                        return (true);
                }
                else
                {
                    if (crit_type != DAM_BASH)
                    {
                        switch (crit_type)
                        {
                        case DAM_BLUDGEON:
                            stun_player( roll_dice(2,5) );
                            break;

                        case DAM_SLICE:
                            cut_player( roll_dice(1,4) );
                            break;

                        case DAM_PIERCE:
                            // armour piercing applied above
                            mpr( "%s stabs you in a vulnerable spot!",
                                 ptr_monam( att, DESC_CAP_THE ) );
                            break;

                        default:
                            break;
                        }
                    }

                    exercise_armour( (coinflip() ? 1 : 2), 2 );

                    if (damage_taken)
                    {
                        ouch( damage_taken, att_id, KILLED_BY_MONSTER );

                        if (you.religion == GOD_XOM
                            && you.hp <= you.hp_max / 3
                            && one_chance_in(10))
                        {
                            Xom_acts( true, you.xp_level, false );
                        }
                    }
                }
            }
        }
        else  // miss
        {
            if (!def)
                exercise_dodging( (coinflip() ? 1 : 2), 2 );

            if (-margin <= at_bay)
            {
                done = true;
                if (sees)
                {
                    snprintf( info, INFO_SIZE, "%s hold%s %s at bay.",
                              def_str, (!def) ? "" : "s",
                              ptr_monam( att, DESC_NOCAP_THE ) );

                    // capitalise:
                    info[0] = toupper( info[0] );
                    mpr( info );
                }
            }
            else // pure miss without the at_bay EV bonus
            {
                if (sees)
                    mpr( "%s misses %s.", ptr_monam( att, DESC_CAP_THE ), def_str );
            }
        }
    }                           // end of for runthru

    return (true);
}                               // end monster_attack()



/*
 **************************************************
 *                                                *
 *              END PUBLIC FUNCTIONS              *
 *                                                *
 **************************************************
*/


// Added by DML 6/10/99.
static void get_attack_description(int damage, char noise[80], char noise2[80])
{
    const int wpn = get_inv_wielded();
    int weap_type = WPN_UNKNOWN;

    if (wpn == -1)
        weap_type = WPN_UNARMED;
    else if (player_weapon_wielded())
    {
        if (you.inv[wpn].base_type == OBJ_WEAPONS)
            weap_type = you.inv[wpn].sub_type;
        else if (you.inv[wpn].base_type == OBJ_STAVES)
            weap_type = WPN_STAFF;
    }

    noise[0] = '\0';
    noise2[0] = '\0';

    if (damage < HIT_WEAK)
    {
        // All weak hits look the same, except for when the player
        // has a non-weapon in hand.  -- bwr
        if (weap_type == WPN_UNKNOWN)
            strcpy( noise, "clumsily bash" );
        else
            strcpy( noise, "hit" );
    }
    else
    {
        switch (weap_type)
        {
        default:
            strcpy( noise, "hit" );
            break;

        case WPN_KNIFE:
        case WPN_DAGGER:
        case WPN_SHORT_SWORD:
        case WPN_TRIDENT:
        case WPN_DEMON_TRIDENT:
        case WPN_SPEAR:
            if (damage < HIT_MED)
                strcpy( noise, "puncture" );
            else if (damage < HIT_STRONG)
                strcpy( noise, "impale" );
            else
            {
                strcpy( noise, "spit" );
                strcpy( noise2, " like a pig" );
            }
            break;

        case WPN_BOW:
        case WPN_LONGBOW:
        case WPN_CROSSBOW:
        case WPN_HAND_CROSSBOW:
            if (damage < HIT_STRONG)
                strcpy( noise, "puncture" );
            else
                strcpy( noise, "skewer" );
            break;

        case WPN_LONG_SWORD:
        case WPN_GREAT_SWORD:
        case WPN_SCIMITAR:
        case WPN_FALCHION:
        case WPN_HALBERD:
        case WPN_GLAIVE:
        case WPN_HAND_AXE:
        case WPN_WAR_AXE:
        case WPN_BROAD_AXE:
        case WPN_BATTLEAXE:
        case WPN_SCYTHE:
        case WPN_QUICK_BLADE:
        case WPN_KATANA:
        case WPN_LAJATANG:
        case WPN_EXECUTIONERS_AXE:
        case WPN_DOUBLE_SWORD:
        case WPN_TRIPLE_SWORD:
        case WPN_SABRE:
        case WPN_DEMON_BLADE:
        case WPN_BLESSED_BLADE:
            if (damage < HIT_MED)
                strcpy( noise, "slash" );
            else if (damage < HIT_STRONG)
                strcpy( noise, "slice" );
            else
            {
                strcpy( noise, "open" );
                strcpy( noise2, " like a pillowcase" );
            }
            break;

        case WPN_SLING:
        case WPN_CLUB:
        case WPN_MACE:
        case WPN_FLAIL:
        case WPN_GREAT_MACE:
        case WPN_QUARTERSTAFF:
        case WPN_STAFF:
        case WPN_GIANT_CLUB:
        case WPN_HAMMER:
        case WPN_ANCUS:
        case WPN_MORNINGSTAR:       /*for now, just a bludgeoning weapon */
        case WPN_SPIKED_FLAIL:      /*for now, just a bludgeoning weapon */
        case WPN_DIRE_FLAIL:        /*for now, just a bludgeoning weapon */
        case WPN_EVENINGSTAR:
        case WPN_GIANT_SPIKED_CLUB:
            if (damage < HIT_MED)
                strcpy( noise, "sock" );
            else if (damage < HIT_STRONG)
                strcpy( noise, "bludgeon" );
            else
            {
                strcpy( noise, "crush" );
                strcpy( noise2, " like a grape" );
            }
            break;

        case WPN_WHIP:
        case WPN_DEMON_WHIP:
            if (damage < HIT_MED)
                strcpy( noise, "whack" );
            else
                strcpy( noise, "thrash" );
            break;

        case WPN_UNARMED:
            // take transformations into account, if no weapon is weilded
            switch (you.attribute[ATTR_TRANSFORMATION])
            {
            case TRAN_NONE:
            default:
                if (you.species == SP_TROLL || you.mutation[MUT_CLAWS])
                {
                    if (damage < HIT_MED)
                        strcpy( noise, "claw" );
                    else if (damage < HIT_STRONG)
                        strcpy( noise, "mangle" );
                    else
                        strcpy( noise, "eviscerate" );
                }
                else
                {
                    if (damage < HIT_MED)
                        strcpy( noise, "punch" );
                    else
                        strcpy( noise, "pummel" );
                }
                break;

            case TRAN_SPIDER:
                if (damage < HIT_STRONG)
                    strcpy( noise, "bite" );
                else
                    strcpy( noise, "maul" );
                break;

            case TRAN_BLADE_HANDS:
                if (damage < HIT_MED)
                    strcpy( noise, "slash" );
                else if (damage < HIT_STRONG)
                    strcpy( noise, "slice" );
                else
                {
                    strcpy( noise, "open" );
                    strcpy( noise2, " like a pillowcase" );
                }
                break;

            case TRAN_ICE_BEAST:
            case TRAN_STATUE:
            case TRAN_LICH:
                if (damage < HIT_MED)
                    strcpy( noise, "punch" );
                else
                    strcpy( noise, "pummel" );
                break;

            case TRAN_DRAGON:
            case TRAN_SERPENT_OF_HELL:
                if (damage < HIT_MED)
                    strcpy( noise, "claw" );
                else if (damage < HIT_STRONG)
                    strcpy( noise, "bite" );
                else
                    strcpy( noise, "maul" );
                break;

            case TRAN_AIR:
                strcpy( noise, "buffet" );
                break;
            }
            break;
        }
    }
}                               // end weapon_type_modify()

// Returns a value from 0 to 10 representing the weight of strength to
// dexterity for the players currently wielded weapon.
static int player_weapon_str_weight( void )
{
    const int wpn = get_inv_wielded();

    if (wpn == -1)
        return (0); // ie unarmed gives str weight 0

    return (weapon_str_weight( you.inv[wpn] ));
}

static int player_melee_weighted_stat( void )
{
    const int  delta = you.str - you.dex;

    // this == (10 - w) * dex + w * str
    return (10 * you.dex + player_weapon_str_weight() * delta);
}

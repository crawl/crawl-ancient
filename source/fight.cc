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

#ifdef DOS
#include <conio.h>
#endif

#include "externs.h"

#include "beam.h"
#include "debug.h"
#include "effects.h"
#include "food.h"
#include "it_use2.h"
#include "itemname.h"
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
#include "spells.h"
#include "spells1.h"
#include "spells3.h"
#include "spells4.h"
#include "stuff.h"
#include "view.h"
#include "wpn-misc.h"

#ifdef MACROS
#include "macro.h"
#endif

#define HIT_WEAK 7
#define HIT_MED 18
#define HIT_STRONG 36
// ... was 5, 12, 21
// how these are used will be replaced by a function in a second ... :P {dlb}

extern bool wield_change;       // defined in output.cc

static int weapon_type_modify(int weap, char *st_prn, char *noise2, int damage);
static void monster_drop_ething(struct monsters *monster);
static void place_monster_corpse(struct monsters *monster);

int weapon_str_weight( int wpn_class, int wpn_type );

static inline int player_weapon_str_weight( void );
static inline int player_weapon_dex_weight( void );

static inline int calc_stat_to_hit_base( void );
static inline int calc_stat_to_dam_base( void );

/*
 **************************************************
 *                                                *
 *             BEGIN PUBLIC FUNCTIONS             *
 *                                                *
 **************************************************
*/

// This function returns the "extra" stats the player gets because of
// choice of weapon... it's used only for giving warnings when a player
// weilds a less than ideal weapon.
int effective_stat_bonus( int wepType )
{
#ifdef USE_NEW_COMBAT_STATS
    int str_weight;
    if (wepType == -1)
        str_weight = player_weapon_str_weight();
    else
        str_weight = weapon_str_weight( OBJ_WEAPONS, wepType );

    return ((you.strength - you.dex) * (str_weight - 5) / 10);
#else
    return (0);
#endif
}

void you_attack(int monster_attacked, bool unarmed_attacks)
{
    struct monsters *defender = &menv[monster_attacked];

    int your_to_hit;
    int damage_done = 0;
    bool hit = false;
    unsigned char stab_bonus = 0;       // this is never negative {dlb}
    int temp_rand;              // for probability determination {dlb}

    const int weapon = you.equip[ EQ_WEAPON ];
    const bool ur_armed = (weapon != -1);   // compacts code a bit {dlb}
    const bool bearing_shield = (you.equip[EQ_SHIELD] != -1);

    bool water_attack = false;

    char str_pass[80], damage_noise[20], damage_noise2[20];
    int special_brand = SPWPN_NORMAL;

    int heavy_armour = 0;

    // heavy armour modifiers for shield borne
    if (bearing_shield)
    {
        switch (you.inv_type[you.equip[EQ_SHIELD]])
        {
        case ARM_SHIELD:
            if (you.skills[SK_SHIELDS] < random2(7))
                heavy_armour++;
            break;
        case ARM_LARGE_SHIELD:
            if ((you.species >= SP_OGRE && you.species <= SP_OGRE_MAGE)
                || player_genus(GENPC_DRACONIAN))
            {
                if (you.skills[SK_SHIELDS] < random2(13))
                    heavy_armour++;     // was potentially "+= 3" {dlb}
            }
            else
            {
                for (unsigned char i = 0; i < 3; i++)
                {
                    if (you.skills[SK_SHIELDS] < random2(13))
                        heavy_armour += random2(3);
                }
            }
            break;
        default:
            break;
        }
    }

    // heavy armour modifiers for PARM_EVASION
    if (you.equip[EQ_BODY_ARMOUR] != -1)
    {
        const int ev_pen = property( OBJ_ARMOUR,
                                     you.inv_type[you.equip[EQ_BODY_ARMOUR]],
                                     PARM_EVASION );

        if (ev_pen < 0 && random2(you.skills[SK_ARMOUR]) < abs( ev_pen ))
            heavy_armour += random2( abs(ev_pen) );
    }

    // ??? what is the reasoning behind this ??? {dlb}
    if (!ur_armed)
        heavy_armour *= (coinflip()? 3 : 2);

    // Calculate the following two flags in advance
    // to know what player does this combat turn:
    bool can_do_unarmed_combat = false;

    if (you.burden_state == BS_UNENCUMBERED
        && random2(20) < you.skills[SK_UNARMED_COMBAT]
        && random2(1 + heavy_armour) < 2)
    {
        can_do_unarmed_combat = true;
    }

    // if we're not getting potential unarmed attacks, and not wearing a
    // shield, and have a suitable uncursed weapon we get the bonus.
    bool use_hand_and_a_half_bonus = false;

    int wpn_skill = SK_UNARMED_COMBAT;
    int hands_reqd = HANDS_ONE_HANDED;  // XXX: is this okay for unarmed? - bwr

    if (weapon != -1)
    {
        wpn_skill = weapon_skill( you.inv_class[weapon], you.inv_type[weapon] );
        hands_reqd = hands_reqd_for_weapon(you.inv_class[weapon],
                                                        you.inv_type[weapon]);
    }

    if (unarmed_attacks
        && !can_do_unarmed_combat
        && !bearing_shield && ur_armed
        && you.inv_plus[ weapon ] < 130      // that is, uncursed
        && hands_reqd == HANDS_ONE_OR_TWO_HANDED)
    {
        // currently: +1 dam, +1 hit, -1 spd (loosly)
        use_hand_and_a_half_bonus = true;
    }

/*
 **************************************************************************
 *                                                                        *
 *  IMPORTANT: When altering damage routines, must also change in ouch.cc *
 *  for saving of player ghosts.                                          *
 *                                                                        *
 **************************************************************************
 */
    bool helpless = mons_flag(defender->type, M_NO_EXP_GAIN);

    if (!mons_friendly(defender))
        you.pet_target = monster_attacked;
    else
        naughty(NAUGHTY_ATTACK_FRIEND, 5);

    // fumbling in shallow water <early return>:
    if (player_in_water() && you.species != SP_MERFOLK)
    {
        if (random2(you.dex) < 4 || one_chance_in(5))
        {
            mpr("Unstable footing causes you to fumble your attack.");
            return;
        }
    }

    // wet merfolk
    if (you.species == SP_MERFOLK && player_in_water()
        // monster not a water creature
        && monster_habitat(defender->type) != DNGN_DEEP_WATER
        // monster in water
        && (grd[defender->x][defender->y] == DNGN_SHALLOW_WATER
            || grd[defender->x][defender->y] == DNGN_DEEP_WATER)
        // monster not flying
        && (!mons_flies(defender->type)))
    {
        water_attack = true;
    }

    // preliminary to_hit modifications:
    your_to_hit = 15 + (calc_stat_to_hit_base() / 2);

    if (water_attack)
        your_to_hit += 5;

    if (wearing_amulet(AMU_INACCURACY))
        your_to_hit -= 5;

    your_to_hit += random2(1 + you.skills[SK_FIGHTING]);

    if (ur_armed)
    {
        if (wpn_skill)
            your_to_hit += random2(you.skills[wpn_skill] + 1);
    }
    else                        // ...you must be unarmed
    {
        your_to_hit += (you.species == SP_TROLL
                        || you.species == SP_GHOUL) ? 4 : 2;
        your_to_hit += random2(1 + you.skills[SK_UNARMED_COMBAT]);
    }

    // energy expenditure in terms of food:
    if (you.is_undead != US_UNDEAD)
        make_hungry(3, true);

    if (ur_armed
        && you.inv_class[ weapon ] == OBJ_WEAPONS
        && (you.inv_dam[ weapon ] % 30 == SPWPN_SPEED
            || (you.inv_dam[ weapon ] % 30 >= SPWPN_RANDART_I
                && inv_randart_wpn_properties(weapon, 0, RAP_BRAND) == SPWPN_SPEED)))
    {
        you.time_taken *= 5;
        you.time_taken /= 10;
    }

    if (ur_armed
        && you.inv_class[ weapon ] == OBJ_WEAPONS
        && you.inv_dam[ weapon ] % 30 >= SPWPN_RANDART_I)
    {
        if (inv_randart_wpn_properties( weapon, 0, RAP_ANGRY) >= 1)
        {
            if (random2(1 + inv_randart_wpn_properties( weapon, 0, RAP_ANGRY )))
                go_berserk(false);
        }
    }

    switch (you.special_wield)
    {
    case SPWLD_TROG:
        if (coinflip())
            go_berserk(false);
        break;
    case SPWLD_WUCAD_MU:
        if (one_chance_in(9))
            miscast_effect(SPTYP_DIVINATION, random2(9), random2(70), 100);
        break;
    default:
        break;
    }

    if (you.mutation[MUT_BERSERK])
    {
        if (random2(100) < (you.mutation[MUT_BERSERK] * 10) - 5)
            go_berserk(false);
    }

    if (ur_armed)
    {
        if (you.inv_class[ weapon ] == OBJ_WEAPONS)
        {
            int ghoggl = you.inv_plus[ weapon ] - 50;

            if (you.inv_plus[ weapon ] > 130)
                ghoggl -= 100;

            // there was some stupid conditional here that applied this
            // "if (ghoggl >= 0)" and "else" that .. which is all
            // cases (d'oh!) {dlb}
            your_to_hit += ghoggl;

            your_to_hit += property( OBJ_WEAPONS, you.inv_type[ weapon ],
                                     PWPN_HIT );

            if (you.inv_dam[ weapon ] / 30 == DWPN_ELVEN
                && player_genus(GENPC_ELVEN))
            {
                your_to_hit += (coinflip()? 2 : 1);
            }
        }

        /* magical staff */
        if (you.inv_class[ weapon ] == OBJ_STAVES)
            your_to_hit += property(OBJ_WEAPONS, WPN_QUARTERSTAFF, PWPN_HIT);
    }

    your_to_hit += slaying_bonus(PWPN_HIT);     // see: player.cc

    if (you.hunger_state == HS_STARVING)
        your_to_hit -= 3;

    your_to_hit -= heavy_armour;

    // why does this come here and not later? {dlb}
    // apparently to give the following pluses more significance -- bwr
    your_to_hit = random2(your_to_hit);

    if (use_hand_and_a_half_bonus)
        your_to_hit += random2(3);

    if (ur_armed && wpn_skill == SK_SHORT_BLADES && you.sure_blade)
        your_to_hit += 5 + random2limit( you.sure_blade, 10 );

    int damage = 1;

    if (!ur_armed)              // empty-handed
    {
        damage = 3;

        if (you.confusing_touch)
        {
            // just trying to touch is easier that trying to damage
            special_brand = SPWPN_CONFUSE;
            your_to_hit += random2(you.dex);
            // no base hand damage while using this spell
            damage = 0;
        }

        if (you.mutation[MUT_DRAIN_LIFE])
            special_brand = SPWPN_DRAINING;

        if (you.attribute[ATTR_TRANSFORMATION] != TRAN_NONE)
        {
            switch (you.attribute[ATTR_TRANSFORMATION])
            {
            case TRAN_SPIDER:
                damage = 9;
                special_brand = SPWPN_VENOM;
                your_to_hit += random2(10);
                break;
            case TRAN_BLADE_HANDS:
                damage = 12 + (you.strength / 4) + (you.dex / 4);
                your_to_hit += random2(12);
                break;
            case TRAN_STATUE:
                damage = 12 + you.strength;
                your_to_hit += random2(9);
                break;
            case TRAN_ICE_BEAST:
                damage = 10;
                special_brand = SPWPN_FREEZING;
                your_to_hit += random2(10);
                break;
            case TRAN_DRAGON:
                damage = 20 + you.strength;
                your_to_hit += random2(10);
                break;
            case TRAN_LICH:
                damage = 5;
                special_brand = SPWPN_DRAINING;
                your_to_hit += random2(10);
                break;
            }
        }
        else
        {
            if (you.species == SP_TROLL)
                damage += 5;
            else if (you.species == SP_GHOUL)
                damage += 2;
        }

        damage += you.skills[SK_UNARMED_COMBAT];
    }
    else
    {
        if (you.inv_class[ weapon ] == OBJ_WEAPONS)
        {
            damage = property( you.inv_class[ weapon ],
                               you.inv_type[ weapon ], PWPN_DAMAGE );
        }
        else if (you.inv_class[ weapon ] == OBJ_STAVES)
            damage = property(OBJ_WEAPONS, WPN_QUARTERSTAFF, PWPN_DAMAGE);
    }

    // Yes, this isn't the *2 damage that monsters get, but since
    // monster hps and player hps are different (as are the sizes
    // of the attacks) it just wouldn't be fair to give merfolk
    // that gross a potential... still they do get something for
    // making use of the water.  -- bwr
    if (water_attack)
        damage += random2avg(10,2);

    //jmf: check for backlight enchantment
    if (mons_has_ench(defender, ENCH_BACKLIGHT_I, ENCH_BACKLIGHT_IV))
       your_to_hit += random2(10);

    int weapon_speed2 = 10;

    if (ur_armed)
    {
        if (you.inv_class[ weapon ] == OBJ_WEAPONS
            || you.inv_class[ weapon ] == OBJ_STAVES)
        {
            if (you.inv_class[ weapon ] == OBJ_STAVES)
            {
                weapon_speed2 = property( OBJ_WEAPONS, WPN_QUARTERSTAFF,
                                          PWPN_SPEED);
            }
            else
            {
                weapon_speed2 = property( you.inv_class[ weapon ],
                                          you.inv_type[ weapon ], PWPN_SPEED );
            }

            weapon_speed2 -= you.skills[ wpn_skill ] / 2;

            int min_speed = 10;

            if (you.inv_class[ weapon ] == OBJ_STAVES)
            {
                min_speed = property(OBJ_WEAPONS, WPN_QUARTERSTAFF,
                                                            PWPN_SPEED) / 2;
            }
            else
            {
                min_speed = property(you.inv_class[ weapon ],
                                      you.inv_type[ weapon ], PWPN_SPEED) / 2;
            }

            // Using both hands can get a weapon up to speed 7
            if ((hands_reqd == HANDS_TWO_HANDED || use_hand_and_a_half_bonus)
                && min_speed > 7)
            {
                min_speed = 7;
            }

            if (weapon_speed2 < min_speed)
                weapon_speed2 = min_speed;

            if (bearing_shield)
            {
                switch (you.inv_type[you.equip[EQ_SHIELD]])
                {
                case ARM_SHIELD:
                    weapon_speed2++;
                    break;
                case ARM_LARGE_SHIELD:
                    weapon_speed2 += 2;
                    break;
                }
            }
        }
    }
    else
    {
        if (you.burden_state == BS_UNENCUMBERED
            && one_chance_in(heavy_armour + 1))
        {
            weapon_speed2 = 10 - you.skills[SK_UNARMED_COMBAT] / 3;

            if (weapon_speed2 < 4)
                weapon_speed2 = 4;
        }
    }

    // Hand and a half bonus only helps speed up to a point, any more
    // than speed 10 must come from skill and the weapon
    if (use_hand_and_a_half_bonus && weapon_speed2 > 10)
        weapon_speed2--;

    // Never allow anything faster than 3 to get through... three+ attacks
    // per round is enough... 5 is just silly. -- bwr
    if (weapon_speed2 < 3)
        weapon_speed2 = 3;

    you.time_taken *= weapon_speed2;
    you.time_taken /= 10;

    if (you.time_taken < 1)
        you.time_taken = 1;

#ifdef WIZARD
    sprintf(info, "Time taken: %d", you.time_taken);
    mpr(info);
#endif

    if (defender->speed_increment <= 40
        || ((defender->behavior == BEH_FLEE
            || mons_has_ench(defender, ENCH_CONFUSION))
                && random2(200) <= you.skills[SK_STABBING] + you.dex)
        || defender->behavior == BEH_SLEEP)
    {
        switch (defender->behavior)
        {
        case BEH_SLEEP:
            // monster is a very easy target
            // note: lower stab_bonuses are better, because they're used
            //       as part of the divisors.
            stab_bonus = 1;
            break;

        case BEH_FLEE:
            // monster has limited control of his actions
            stab_bonus = 2;
            break;

        default:
            // monster is in control of his actions
            stab_bonus = 3;
        }

        // can't do this in the case statement
        if (mons_has_ench(defender, ENCH_CONFUSION))
            stab_bonus = 2;

        simple_monster_message(defender, " fails to defend itself.");

        exercise(SK_STABBING, 1 + random2avg(5, 4));

        if (mons_holiness(defender->type) == MH_NATURAL
            || mons_holiness(defender->type) == MH_HOLY)
        {
            naughty(NAUGHTY_STABBING, 4);
        }
    }
    else
        // ok.. if you didn't backstab, you wake up the neighborhood.
        // I can live with that.
        alert();

    // now, whether or not you 'hit' the monster in question,  you've
    // pissed them off
    behavior_event(defender, ME_WHACK, MHITYOU);

    if ((your_to_hit >= defender->evasion || one_chance_in(15))
        || ((defender->speed_increment <= 60
             || defender->behavior == BEH_SLEEP)
            && !one_chance_in(10 + you.skills[SK_STABBING])))
    {
        hit = true;
        int dammod = 78;

        const int dam_stat_val = calc_stat_to_dam_base();

        if (dam_stat_val > 11)
            dammod += (random2(dam_stat_val - 11) * 2);
        else if (dam_stat_val < 9)
            dammod -= (random2(9 - dam_stat_val) * 3);

        damage *= dammod;       //random2(you.strength);
        damage /= 78;


        damage_done = random2(damage);

        if (ur_armed && (you.inv_class[ weapon ] == OBJ_WEAPONS
                            || you.inv_class[ weapon ] == OBJ_STAVES))
        {
            damage_done *= 25 + (random2( you.skills[ wpn_skill ] + 1 ));
            damage_done /= 25;
        }

        damage_done *= 30 + (random2(you.skills[SK_FIGHTING] + 1));
        damage_done /= 30;

        if (you.might > 1)
            damage_done += 1 + random2(10);

        if (you.hunger_state == HS_STARVING)
            damage_done -= random2(5);

        if (ur_armed && you.inv_class[ weapon ] == OBJ_WEAPONS)
        {
            int hoggl = you.inv_plus2[ weapon ] - 50;

            damage_done += (hoggl > -1) ? (random2(1 + hoggl)) : (hoggl);

            // removed 2-handed weapons from here... their "bonus" is
            // already included in the damage stat for the weapon -- bwr
            if (use_hand_and_a_half_bonus)
                damage_done += random2(3);

            if (you.inv_dam[ weapon ] / 30 == DWPN_DWARVEN
                && player_genus(GENPC_DWARVEN))
            {
                damage_done += random2(3);
            }

            if (you.inv_dam[ weapon ] / 30 == DWPN_ORCISH
                && you.species == SP_HILL_ORC && coinflip())
            {
                damage_done++;
            }

            if (you.inv_ident[ weapon ] < 3
                && random2(100) < you.skills[ wpn_skill ])
            {
                you.inv_ident[ weapon ] = 3;
                strcpy(info, "You are wielding ");
                in_name( weapon , 3, str_pass);
                strcat(info, str_pass);
                strcat(info, ".");
                mpr(info);
                more();
                wield_change = true;
            }
        }

        //  else (damage_done -= random2(5));
        damage_done += slaying_bonus(PWPN_DAMAGE);

        if (stab_bonus)
        {
            // lets make sure we have some damage to work with...
            if (damage_done < 1)
                damage_done = 1;

            if (defender->behavior == BEH_SLEEP)
            {
                // Sleeping moster wakes up when stabbed but may be groggy
                if (random2(200) <= you.skills[SK_STABBING] + you.dex)
                {
                    int stun = random2(you.dex + 1);

                    if (defender->speed_increment > stun)
                        defender->speed_increment -= stun;
                    else
                        defender->speed_increment = 0;
                }
            }

            switch (wpn_skill)
            {
            case SK_SHORT_BLADES:
                if (you.inv_type[ weapon ] == WPN_DAGGER)
                {
                    int bonus = you.dex / 3;

                    if (bonus > you.skills[SK_STABBING] + 1)
                        bonus = you.skills[SK_STABBING] + 1;

                    damage_done += bonus;
                }
                // fall through
            case SK_LONG_SWORDS:
                damage_done *= 10 + you.skills[SK_STABBING] /
                    (stab_bonus + (wpn_skill == SK_SHORT_BLADES ? 0 : 1));
                damage_done /= 10;
                // fall through
            default:
                damage_done *= 12 + you.skills[SK_STABBING] / stab_bonus;
                damage_done /= 12;
            }

            // when stabbing we can get by some of the armour
            if (defender->armor_class > 0)
            {
                int ac = defender->armor_class
                            - random2( you.skills[SK_STABBING] / stab_bonus );

                if (ac > 0)
                    damage_done -= random2(1 + ac);
            }
        }
        else
        {
            // apply AC normally
            if (defender->armor_class > 0)
                damage_done -= random2(1 + defender->armor_class);
        }

        damage_done = weapon_type_modify( weapon , damage_noise,
                                                   damage_noise2, damage_done);

        if (damage_done < 0)
            damage_done = 0;

        if (hurt_monster(defender, damage_done))
        {
            if (ur_armed && wpn_skill)
            {
                if (!helpless || you.skills[ wpn_skill ] < 2)
                    exercise( wpn_skill, 1 );
            }
            else
            {
                if (!helpless || you.skills[SK_UNARMED_COMBAT] < 2)
                    exercise(SK_UNARMED_COMBAT, 1);
            }

            if ((!helpless || you.skills[SK_FIGHTING] < 2)
                && one_chance_in(3))
            {
                exercise(SK_FIGHTING, 1);
            }
        }

        if (defender->hit_points < 1)
        {
#ifdef WIZARD
            itoa(damage_done, st_prn, 10);
            strcpy(info, "Hit for ");
            /* note: doesn't take account of special weapons etc */
            strcat(info, st_prn);
            strcat(info, ".");
            mpr(info);
#endif
            if (ur_armed
                && you.inv_class[ weapon ] == OBJ_WEAPONS
                && you.inv_dam[ weapon ] < NWPN_SINGING_SWORD
                && you.inv_dam[ weapon ] % 30 == SPWPN_VAMPIRICISM)
            {
                if ((mons_holiness(defender->type) == MH_NATURAL
                        || mons_holiness(defender->type) == MH_HOLY)
                    && damage_done > 0 && you.hp < you.hp_max
                    && !one_chance_in(5))
                {
                    mpr("You feel better.");

                    // more than if not killed
                    inc_hp(1 + random2(damage_done), false);

                    if (you.hunger_state != HS_ENGORGED)
                        lessen_hunger(30 + random2avg(59, 2), true);

                    naughty(NAUGHTY_NECROMANCY, 2);
                }
            }

            monster_die(defender, KILL_YOU, 0);

            if (defender->type == MONS_GIANT_SPORE)
            {
                sprintf(info, "You %s the giant spore.",
                    damage_noise);
                mpr(info);
            }
            else if (defender->type == MONS_BALL_LIGHTNING)
            {
                sprintf(info, "You %s the ball lightning.",
                    damage_noise);
                mpr(info);
            }
            return;
        }

        if (damage_done < 1 && (!mons_has_ench(defender,ENCH_INVIS)
            || player_see_invis()))
        {
            hit = true;

            sprintf(info, "You %s ", damage_noise);
            strcat(info, ptr_monam(defender, 1));
            strcat(info, ", but do no damage.");
            mpr(info);
        }
    }
    else
    {
        hit = false;

        if ((your_to_hit + heavy_armour / 2) >= defender->evasion)
            strcpy(info, "Your armour prevents you from hitting ");
        else
            strcpy(info, "You miss ");

        strcat(info, ptr_monam(defender, 1));
        strcat(info, ".");
        mpr(info);
    }

    if (hit && damage_done > 0
        || (hit && damage_done < 1 && mons_has_ench(defender,ENCH_INVIS)))
    {
        strcpy(info, "You ");
        strcat(info, damage_noise);
        strcat(info, " ");
        strcat(info, ptr_monam(defender, 1));
        strcat(info, damage_noise2);

#ifdef WIZARD
        itoa(damage_done, st_prn, 10);
        strcat(info, " for ");
        /* note: doesn't take account of special weapons etc */
        strcat(info, st_prn);
#endif
        if (damage_done < HIT_WEAK)
            strcat(info, ".");
        else if (damage_done < HIT_MED)
            strcat(info, "!");
        else if (damage_done < HIT_STRONG)
            strcat(info, "!!");
        else
            strcat(info, "!!!");

        mpr(info);

        if (mons_holiness(defender->type) == MH_HOLY)
            done_good(GOOD_KILLED_ANGEL_I, 1);

        if (you.special_wield == SPWLD_TORMENT)
        {
            torment(you.x_pos, you.y_pos);
            naughty(NAUGHTY_UNHOLY, 5);
        }

        if (you.special_wield == SPWLD_ZONGULDROK
            || you.special_wield == SPWLD_CURSE)
        {
            naughty(NAUGHTY_NECROMANCY, 3);
        }
    }

    if (defender->type == MONS_JELLY
        || defender->type == MONS_BROWN_OOZE
        || defender->type == MONS_ACID_BLOB
        || defender->type == MONS_ROYAL_JELLY)
    {
        weapon_acid(5);
    }

    int specdam = 0;

    /* remember, damage_done is still useful! */

    int weap_dam = SPWPN_NORMAL;

    if (hit && !ur_armed && special_brand != SPWPN_NORMAL)
    {
        /* No electrocution - this references inv_plus2 */
        weap_dam = special_brand;
        goto dam_thing;
    }

    if (hit && ur_armed && you.inv_class[ weapon ] == OBJ_WEAPONS)
    {
        if (is_demonic(you.inv_type[ weapon ]))
            naughty(NAUGHTY_UNHOLY, 1);

        weap_dam = you.inv_dam[ weapon ] % 30;

        if (you.inv_dam[ weapon ] >= NWPN_SINGING_SWORD)
            weap_dam = SPWPN_NORMAL;

        if (you.inv_dam[ weapon ] % 30 >= SPWPN_RANDART_I)
            weap_dam = inv_randart_wpn_properties( weapon, 0, RAP_BRAND );

        switch (you.inv_dam[ weapon ])
        {
        case NWPN_SWORD_OF_CEREBOV:
            weap_dam = SPWPN_FLAMING;
            break;
        case NWPN_STAFF_OF_OLGREB:
            weap_dam = SPWPN_VENOM;
            break;
        case NWPN_VAMPIRES_TOOTH:
            weap_dam = SPWPN_VAMPIRICISM;
            break;
        }
    }

    // jmf: BEGIN STAFF HACK
    // How much bonus damage will a staff of <foo> do?
    // FIXME: make these not macros. inline functions?
    // actually, it will all be pulled out and replaced by functions -- {dlb}
#define STAFF_DAMAGE(SKILL) ((3 + random2(you.skills[ (SKILL) ] ) \
                                + random2(you.skills[ (SKILL) ] ) \
                                + random2(you.skills[ (SKILL) ] )) / 6 )
#define STAFF_COST 2
#define CAT_MONSTER_INFO(MONSTER) strcat(info, \
                                  ptr_monam(&(menv[(MONSTER)]), 1) )

#define CAT_PUNCTUATION(DAMAGE) if ((DAMAGE) < 3) strcat(info, "."); \
                                if ((DAMAGE) >= 3) strcat(info, "!"); \
                                if ((DAMAGE) >= 10) strcat(info, "!"); \
                                if ((DAMAGE) >  20) strcat(info, "!");


    if (ur_armed && hit && you.inv_class[ weapon ] == OBJ_STAVES)
    {
        specdam = 0;
        if (you.magic_points >= STAFF_COST)
        {
            switch (you.inv_type[ weapon ])
            {
            case STAFF_AIR:
                if (damage_done + you.skills[SK_AIR_MAGIC] > random2(30))
                {
                    if (mons_res_elec(defender->type))
                        break;

                    strcpy(info, ptr_monam(defender, 0));
                    strcat(info, " is jolted");
                    specdam = STAFF_DAMAGE(SK_AIR_MAGIC);
                    CAT_PUNCTUATION(specdam);
                    mpr(info);
                }
                break;

            case STAFF_COLD:    // FIXME: I don't think I used these right ...
                if (mons_res_cold(defender->type) > 0)
                    break;

                specdam = STAFF_DAMAGE(SK_ICE_MAGIC);

                if (defender->inv[MSLOT_ARMOUR] != NON_ITEM
                    && mitm.special[defender->inv[MSLOT_ARMOUR]] % 30
                                                    == SPARM_COLD_RESISTANCE)
                {
                    specdam /= 3;
                }

                if (mons_res_cold(defender->type) < 0
                    && (defender->inv[MSLOT_ARMOUR] == NON_ITEM
                        || mitm.special[defender->inv[MSLOT_ARMOUR]] % 30
                                                    != SPARM_COLD_RESISTANCE))
                {
                    specdam += STAFF_DAMAGE(SK_ICE_MAGIC);
                }

                if (specdam)
                {
                    strcpy(info, "You freeze ");
                    CAT_MONSTER_INFO(monster_attacked);
                    CAT_PUNCTUATION(specdam);
                    mpr(info);
                }
                break;

            case STAFF_EARTH:
                if (mons_flies(defender->type))
                    break;      //jmf: lame, but someone ought to resist

                specdam = STAFF_DAMAGE(SK_EARTH_MAGIC);
                strcpy(info, "You crush ");
                CAT_MONSTER_INFO(monster_attacked);
                CAT_PUNCTUATION(specdam);
                mpr(info);
                break;

            case STAFF_FIRE:
                if (mons_res_fire(defender->type) > 0)
                    break;

                specdam = STAFF_DAMAGE(SK_FIRE_MAGIC);

                if (defender->inv[MSLOT_ARMOUR] != NON_ITEM
                    && mitm.special[defender->inv[MSLOT_ARMOUR]] % 30
                                                    == SPARM_FIRE_RESISTANCE)
                {
                    specdam = (random2(damage_done) / 2 + 1) / 3;
                }

                if (mons_res_fire(defender->type) < 0
                    && (defender->inv[MSLOT_ARMOUR] == NON_ITEM
                        || mitm.special[defender->inv[MSLOT_ARMOUR]] % 30
                                                    != SPARM_FIRE_RESISTANCE))
                {
                    specdam += STAFF_DAMAGE(SK_FIRE_MAGIC);
                }

                if (specdam)
                {
                    strcpy(info, "You burn ");
                    CAT_MONSTER_INFO(monster_attacked);
                    CAT_PUNCTUATION(specdam);
                    mpr(info);
                }
                break;

            case STAFF_POISON:
                // cap chance at 30% -- let staff of Olgreb shine
                temp_rand = damage_done + you.skills[SK_POISON_MAGIC];
                if (temp_rand > 30)
                    temp_rand = 30;

                if (random2(100) < temp_rand)
                    poison_monster(defender, true);
                break;

            case STAFF_DEATH:
                // FIXME: bring into line with other staff functions
                if ((mons_holiness(defender->type) == MH_NATURAL
                        || mons_holiness(defender->type) == MH_HOLY)
                    && random2(8) <= you.skills[SK_NECROMANCY])
                {
                    strcpy(info, ptr_monam(defender, 0));
                    strcat(info, " convulses in agony");
                    specdam = STAFF_DAMAGE(SK_NECROMANCY);
                    CAT_PUNCTUATION(specdam);
                    mpr(info);
                    naughty(NAUGHTY_NECROMANCY, 4);
                }
                break;

            default:
                mpr("You're wielding some staff I've never heard of! (fight.cc)");
            case STAFF_SMITING:
            case STAFF_POWER:
            case STAFF_SUMMONING_I:
            case STAFF_SUMMONING_II:
            case STAFF_DESTRUCTION_I:
            case STAFF_DESTRUCTION_II:
            case STAFF_DESTRUCTION_III:
            case STAFF_DESTRUCTION_IV:
            case STAFF_WARDING:
            case STAFF_DISCOVERY:
            case STAFF_DEMONOLOGY:
            case STAFF_CHANNELING:
            case STAFF_CONJURATION:
            case STAFF_ENCHANTMENT:
            case STAFF_ENERGY:
            case STAFF_WIZARDRY:
                break;
            } // end switch
        }

        if (specdam > 0)
        {
            dec_mp(STAFF_COST);

            if (you.inv_ident[ weapon ] < 3)
            {
                you.inv_ident[ weapon ] = 3;
                strcpy(info, "You are wielding ");
                in_name( weapon , 3, str_pass);
                strcat(info, str_pass);
                strcat(info, ".");
                mpr(info);
                more();
                wield_change = true;
            }
            goto post_spec_damage;
        }
    }

#undef STAFF_DAMAGE
#undef STAFF_COST
#undef CAT_MONSTER_INFO
#undef CAT_PUNCTUATION
    // END STAFF HACK

    if (ur_armed && hit && you.inv_class[ weapon ] == OBJ_WEAPONS)
    {
      dam_thing:
        switch (weap_dam)
        {
        case SPWPN_NORMAL:
            break;

        case SPWPN_FLAMING:
            specdam = 0;

            if (!mons_res_fire(defender->type))
            {
                specdam = random2(damage_done) / 2 + 1;
                if (defender->inv[MSLOT_ARMOUR] != NON_ITEM
                    && mitm.special[defender->inv[MSLOT_ARMOUR]] % 30
                                                    == SPARM_FIRE_RESISTANCE)
                {
                    specdam = (random2(damage_done) / 2 + 1) / 3;
                }
            }

            if (mons_res_fire(defender->type) == -1
                && (defender->inv[MSLOT_ARMOUR] == NON_ITEM
                    || mitm.special[defender->inv[MSLOT_ARMOUR]] % 30
                                                    != SPARM_FIRE_RESISTANCE))
            {
                specdam = random2(damage_done) + 1;
            }

            if (specdam)
            {
                strcpy(info, "You burn ");
                strcat(info, ptr_monam(defender, 1));

                if (specdam < 3)
                    strcat(info, ".");
                else if (specdam < 7)
                    strcat(info, "!");
                else
                    strcat(info, "!!");

                mpr(info);
            }
            break;

        case SPWPN_FREEZING:
            specdam = 0;
            if (!mons_res_cold(defender->type))
            {
                specdam = 1 + random2(damage_done) / 2;
                if (defender->inv[MSLOT_ARMOUR] != NON_ITEM
                    && mitm.special[defender->inv[MSLOT_ARMOUR]] % 30
                                                    == SPARM_COLD_RESISTANCE)
                {
                    specdam = (1 + random2(damage_done) / 2) / 3;
                }
            }

            if (mons_res_cold(defender->type) == -1
                && (defender->inv[MSLOT_ARMOUR] == NON_ITEM
                    || mitm.special[defender->inv[MSLOT_ARMOUR]] % 30
                                                    != SPARM_COLD_RESISTANCE))
            {
                specdam = 1 + random2(damage_done);
            }

            if (specdam)
            {
                strcpy(info, "You freeze ");
                strcat(info, ptr_monam(defender, 1));

                if (specdam < 3)
                    strcat(info, ".");
                else if (specdam < 7)
                    strcat(info, "!");
                else
                    strcat(info, "!!");

                mpr(info);
            }
            break;

        case SPWPN_HOLY_WRATH:
            specdam = 0;
            switch (mons_holiness(defender->type))
            {
            // there should be a case in here for holy monsters,
            // see elsewhere in fight.cc {dlb}
            case MH_UNDEAD:
                specdam += 1 + random2(damage_done);
                break;

            case MH_DEMONIC:
                specdam += 1 + (random2(damage_done * 15) / 10);
                break;
            }
            break;


        case SPWPN_ELECTROCUTION:
            specdam = 0;

            if (mons_flies(defender->type))
                break;
            else if (mons_res_elec(defender->type))
                break;
            else if (one_chance_in(3))
            {
                mpr("There is a sudden explosion of sparks!");
                specdam += random2avg(28, 3);
            }
            break;

        case SPWPN_ORC_SLAYING:
            if (mons_charclass(defender->type) == MONS_ORC)
                hurt_monster(defender, 1 + random2(damage_done));
            break;

        case SPWPN_VENOM:
            if (!one_chance_in(4))
                poison_monster(defender, true);
            break;

        case SPWPN_DRAINING:
            if ((mons_holiness(defender->type) == MH_UNDEAD
                    || mons_holiness(defender->type) == MH_DEMONIC)
                || one_chance_in(3))
            {
                break;
            }

            strcpy(info, "You drain ");
            strcat(info, ptr_monam(defender, 1));
            strcat(info, "!");
            mpr(info);

            if (one_chance_in(5))
                defender->hit_dice--;

            defender->max_hit_points -= 2 + random2(3);
            defender->hit_points -= 2 + random2(3);

            if (defender->hit_points >= defender->max_hit_points)
                defender->hit_points = defender->max_hit_points;

            if (defender->hit_dice < 1)
                defender->hit_points = 0;
            specdam = 1 + (random2(damage_done) / 2);
            naughty(NAUGHTY_NECROMANCY, 2);
            break;

            /* 9 = speed - done before */

        case SPWPN_VORPAL:
            specdam = 1 + random2(damage_done) / 2;
            break;

        case SPWPN_VAMPIRICISM:
            specdam = 0;        // NB: does no extra damage

                 // should really also prevent draining eg golemsholiness(defender->type) == MH_UNDEAD
            if (mons_holiness(defender->type) == MH_UNDEAD
                || mons_holiness(defender->type) == MH_DEMONIC)
            {
                break;
            }
            else if (damage_done < 1)
                break;
            else if (you.hp == you.hp_max || one_chance_in(5))
                break;

            mpr("You feel better.");

            if (ur_armed
                && you.inv_dam[ weapon ] == NWPN_VAMPIRES_TOOTH)
            {
                inc_hp(damage_done, false);
            }
            else
            {
                // thus is probably more valuable on larger weapons?
                inc_hp(1 + random2(damage_done), false);
            }

            if (you.hunger_state != HS_ENGORGED)
                lessen_hunger(random2avg(59, 2), true);

            naughty(NAUGHTY_NECROMANCY, 2);
            break;

        case SPWPN_DISRUPTION:
            specdam = 0;
            if (mons_holiness(defender->type) == MH_UNDEAD && !one_chance_in(3))
            {
                simple_monster_message(defender, " shudders.");
                specdam += random2avg((1 + (damage_done * 3)), 3);
            }
            break;

        case SPWPN_PAIN:
            specdam = 0;
            if ((mons_holiness(defender->type) == MH_NATURAL
                    || mons_holiness(defender->type) == MH_HOLY)
                && random2(8) <= you.skills[SK_NECROMANCY])
            {
                simple_monster_message(defender, " convulses in agony.");
                specdam += random2(1 + (you.skills[SK_NECROMANCY] * 2));
            }
            naughty(NAUGHTY_NECROMANCY, 4);
            break;

        case SPWPN_DISTORTION:
            //jmf: blink frogs *like* distortion
            // I think could be amended to let blink frogs "grow" like
            // jellies do {dlb}
            if (defender->type == MONS_BLINK_FROG)
            {
                if (one_chance_in(5))
                {
                    simple_monster_message( defender,
                        " basks in the translocular energy." );
                    heal_monster(defender, 1 + random2avg(7, 2), true); // heh heh
                }
                break;
            }

            if (one_chance_in(3))
            {
                strcpy(info, "Space bends around ");
                strcat(info, ptr_monam(defender, 1));
                strcat(info, ".");
                mpr(info);
                specdam += 1 + random2avg(7, 2);
                break;
            }

            if (one_chance_in(3))
            {
                strcpy(info, "Space warps horribly around ");
                strcat(info, ptr_monam(defender, 1));
                strcat(info, "!");
                mpr(info);
                specdam += 3 + random2avg(24, 2);
                break;
            }

            if (one_chance_in(3))
            {
                monster_blink(defender);
                break;
            }

            if (coinflip())
            {
                monster_teleport(defender, coinflip());
                break;
            }

            if (coinflip())
            {
                monster_die(defender, KILL_RESET, 0);
                return;
            }
            break;

        case SPWPN_CONFUSE:
            {
                // declaring these just to pass to the enchant function
                struct bolt beam_temp;

                // ick -- colour tells beam function (confusion == RED)
                beam_temp.colour = RED;

                mons_ench_f2(defender, beam_temp);

                you.confusing_touch -= random2(20);

                if (you.confusing_touch < 1)
                    you.confusing_touch = 1;
            }
            break;
        }                       /* end switch */
    }

  post_spec_damage:             //jmf: added

    if (mons_holiness(defender->type) == MH_HOLY)
        naughty(NAUGHTY_ATTACK_HOLY, defender->hit_dice);

    if (defender->type == MONS_HYDRA)
    {
        const int dam_type = damage_type(you.inv_class[ weapon ],
                                            you.inv_type[ weapon ]);
        if (ur_armed && dam_type == DVORP_SLICING || dam_type == DVORP_CHOPPING)
        {
            if ((damage_done < 4 || coinflip())
                && (you.inv_class[ weapon ] != OBJ_WEAPONS
                    || you.inv_dam[ weapon ] != WPN_BATTLEAXE))
            {
                goto mons_dies;
            }

            strcpy(info, "You ");
            temp_rand = random2(4);
            strcat(info, (temp_rand == 0) ? "slice" :
                         (temp_rand == 1) ? "lop" :
                         (temp_rand == 2) ? "chop"
                                          : "hack");
            strcat(info, " one of ");
            strcat(info, ptr_monam(defender, 1));
            strcat(info, "'s heads off.");
            mpr(info);

            defender->number--;
            if (defender->number < 1)
            {
                defender->hit_points = -1;
                goto mons_dies;
            }

            if (you.inv_class[ weapon ] == OBJ_WEAPONS
                && you.inv_dam[ weapon ] % 30 == SPWPN_FLAMING)
            {
                goto mons_dies; // cauterised
            }

            if (defender->number >= 18)
                goto mons_dies;

            simple_monster_message(defender, " grows two more!");

            defender->number += 2;
        }
    }

    /* remember, the hydra function sometimes skips straight to mons_dies */
    hurt_monster(defender, specdam);

  mons_dies:
    if (defender->hit_points < 1)
    {
        monster_die(defender, KILL_YOU, 0);
        return;
    }

    if (unarmed_attacks)
    {
        char attack_name[20] = "";
        int sc_dam = 0;

        int unarmed_attack = UNAT_NO_ATTACK;

        if (can_do_unarmed_combat)
        {
            unarmed_attack = (coinflip() ? UNAT_HEADBUTT : UNAT_KICK);

            if (you.attribute[ATTR_TRANSFORMATION] == TRAN_DRAGON
                    || player_genus(GENPC_DRACONIAN)
                    || (you.species == SP_MERFOLK && player_in_water())
                && one_chance_in(3))
            {
                unarmed_attack = UNAT_TAILSLAP;
            }

            if (coinflip())
                unarmed_attack = UNAT_PUNCH;
        }

        for (unsigned char scount = 0; scount < 4; scount++)
        {
            switch (scount)
            {
            case 0:
                if (unarmed_attack != UNAT_KICK)        //jmf: hooves mutation
                {
                    if ((you.species != SP_CENTAUR && !you.mutation[MUT_HOOVES])
                        || coinflip())
                    {
                        continue;
                    }
                }

                if (you.attribute[ATTR_TRANSFORMATION] == TRAN_SERPENT_OF_HELL
                    || you.attribute[ATTR_TRANSFORMATION] == TRAN_ICE_BEAST
                    || you.attribute[ATTR_TRANSFORMATION] == TRAN_DRAGON
                    || you.attribute[ATTR_TRANSFORMATION] == TRAN_SPIDER)
                {
                    continue;
                }

                strcpy(attack_name, "kick");
                sc_dam = ((you.mutation[MUT_HOOVES]
                            || you.species == SP_CENTAUR) ? 10 : 5);
                break;

            case 1:
                if (unarmed_attack != UNAT_HEADBUTT)
                {
                    if ((you.species != SP_MINOTAUR
                            && (!you.mutation[MUT_HORNS]
                                && you.species != SP_KENKU))
                        || !one_chance_in(3))
                    {
                        continue;
                    }
                }

                if (you.attribute[ATTR_TRANSFORMATION] == TRAN_SERPENT_OF_HELL
                    || you.attribute[ATTR_TRANSFORMATION] == TRAN_SPIDER
                    || you.attribute[ATTR_TRANSFORMATION] == TRAN_ICE_BEAST
                    || you.attribute[ATTR_TRANSFORMATION] == TRAN_DRAGON)
                {
                    continue;
                }

                strcpy(attack_name, (you.species == SP_KENKU) ? "peck"
                                                              : "headbutt");

                sc_dam = 5 + you.mutation[MUT_HORNS] * 3;

                if (you.species == SP_MINOTAUR)
                    sc_dam += 5;

                if (you.equip[EQ_HELMET] != -1
                    && you.inv_plus2[you.equip[EQ_HELMET]] <= 1)
                {
                    sc_dam += 2;

                    if (you.inv_dam[you.equip[EQ_HELMET]] / 30 == DHELM_SPIKED
                        || you.inv_dam[you.equip[EQ_HELMET]] / 30
                                                                == DHELM_HORNED)
                    {
                        sc_dam += 3;
                    }
                }
                break;

            case 2:             /* draconians */
                if (unarmed_attack != UNAT_TAILSLAP)
                {
                    // not draconian and not wet merfolk
                    if ((!player_genus(GENPC_DRACONIAN)
                        && (!(you.species == SP_MERFOLK && player_in_water())))
                        || (!one_chance_in(4)))

                    {
                        continue;
                    }
                }

                if (you.attribute[ATTR_TRANSFORMATION] == TRAN_SPIDER
                    || you.attribute[ATTR_TRANSFORMATION] == TRAN_ICE_BEAST)
                {
                    continue;
                }

                strcpy(attack_name, "tail-slap");
                sc_dam = 6;

                /* grey dracs have spiny tails, or something */
                // maybe add this to player messaging {dlb}
                if (you.species == SP_GREY_DRACONIAN
                    && you.experience_level > 5)
                {
                    sc_dam = 12;
                }
                break;

            case 3:
                if (unarmed_attack != UNAT_PUNCH)
                    continue;

                if (you.attribute[ATTR_TRANSFORMATION] == TRAN_SERPENT_OF_HELL
                    || you.attribute[ATTR_TRANSFORMATION] == TRAN_SPIDER
                    || you.attribute[ATTR_TRANSFORMATION] == TRAN_ICE_BEAST
                    || you.attribute[ATTR_TRANSFORMATION] == TRAN_DRAGON)
                {
                    continue;
                }

                /* no punching with a shield or 2-handed wpn, except staves */
                if (bearing_shield || coinflip()
                    || (ur_armed && hands_reqd == HANDS_TWO_HANDED))
                {
                    continue;
                }

                strcpy(attack_name, "punch");

                /* applied twice */
                sc_dam = 5 + you.skills[SK_UNARMED_COMBAT] / 3;

                if (you.attribute[ATTR_TRANSFORMATION] == TRAN_BLADE_HANDS)
                {
                    strcpy(attack_name, "slash");
                    sc_dam += 6;
                }
                break;

                /* To add more, add to while part of loop below as well */
            default:
                continue;

            }

            your_to_hit = 13 + you.dex / 2 + you.skills[SK_UNARMED_COMBAT] / 2
                                           + you.skills[SK_FIGHTING] / 5;

            if (wearing_amulet(AMU_INACCURACY))
                your_to_hit -= 5;

            if (you.is_undead != US_UNDEAD)
                make_hungry(2, true);

            if (you.hunger_state == HS_STARVING)
                your_to_hit -= 3;

            your_to_hit += slaying_bonus(PWPN_HIT);
            your_to_hit = random2(your_to_hit);

            damage = sc_dam;    //4 + you.experience_level / 3;

            alert();

            if (your_to_hit >= defender->evasion || one_chance_in(15))
            {
                bool hit = true;
                int dammod = 10;

                const int dam_stat_val = calc_stat_to_dam_base();

                if (dam_stat_val > 11)
                    dammod += random2(dam_stat_val - 11) / 3;
                if (dam_stat_val < 9)
                    dammod -= random2(9 - dam_stat_val) / 2;

                damage *= dammod;
                damage /= 10;

                damage_done = (int) random2(damage);

                damage_done *= 40 + (random2(you.skills[SK_FIGHTING] + 1));
                damage_done /= 40;

                damage_done *= 25 + (random2(you.skills[SK_UNARMED_COMBAT]+1));
                damage_done /= 25;

                if (you.might > 1)
                    damage_done += 1 + random2(10);

                if (you.hunger_state == HS_STARVING)
                    damage_done -= random2(5);

                damage_done += slaying_bonus(PWPN_DAMAGE);

                damage_done -= random2(1 + defender->armor_class);

                if (damage_done < 1)
                    damage_done = 0;
                else
                    hurt_monster(defender, damage_done);

                if (damage_done > 0)
                {
                    if ((!helpless || you.skills[SK_FIGHTING] < 2)
                        && one_chance_in(5))
                    {
                        exercise(SK_FIGHTING, 1);
                    }
                }

                if (defender->hit_points < 1)
                {
#ifdef WIZARD
                    itoa(damage_done, st_prn, 10);
                    strcpy(info, "Kick for ");
                    strcat(info, st_prn);
                    strcat(info, ".");
                    mpr(info);
#endif
                    monster_die(defender, KILL_YOU, 0);

                    if (defender->type == MONS_GIANT_SPORE)
                    {
                        strcpy(info, "You ");
                        strcat(info, attack_name);
                        strcat(info, "the giant spore.");
                        mpr(info);
                    }
                    else if (defender->type == MONS_BALL_LIGHTNING)
                    {
                        strcpy(info, "You ");
                        strcat(info, attack_name);
                        strcat(info, "the ball lightning.");
                        mpr(info);
                    }
                    return;
                }

                if (damage_done < 1
                    && (!mons_has_ench(defender,ENCH_INVIS)
                        || player_see_invis()))
                {
                    strcpy(info, "You ");
                    strcat(info, attack_name);
                    strcat(info, " ");
                    strcat(info, ptr_monam(defender, 1));
                    strcat(info, ", but do no damage.");
                    mpr(info);
                    hit = true;
                }
            }
            else
            {
                strcpy(info, "Your ");
                strcat(info, attack_name);
                strcat(info, " misses ");
                strcat(info, ptr_monam(defender, 1));
                strcat(info, ".");
                mpr(info);
                damage_done = -99;
            }

            if (damage_done > 0
                || (damage_done < 1 && mons_has_ench(defender,ENCH_INVIS)
                    && damage_done != -99))
            {
                strcpy(info, "You ");
                strcat(info, attack_name);
                strcat(info, " ");
                strcat(info, ptr_monam(defender, 1));

#ifdef WIZARD
                itoa(damage_done, st_prn, 10);
                strcat(info, " for ");
                strcat(info, st_prn);
#endif

                if (damage_done < HIT_WEAK)
                    strcat(info, ".");
                else if (damage_done < HIT_MED)
                    strcat(info, "!");
                else if (damage_done < HIT_STRONG)
                    strcat(info, "!!");
                else
                    strcat(info, "!!!");

                mpr(info);

                if (mons_holiness(defender->type) == MH_HOLY)
                    done_good(GOOD_KILLED_ANGEL_I, 1);

                hit = true;
            }

            if (!helpless || you.skills[SK_UNARMED_COMBAT] < 2)
                exercise(SK_UNARMED_COMBAT, 1);
        }
    }

    if (hit)
        print_wounds(defender);

    return;
}                               // end you_attack()

void monster_attack(int monster_attacking)
{
    struct monsters *attacker = &menv[monster_attacking];

    int damage_taken = 0;
    bool hit = false;
    bool blocked = false;

    // being attacked by a water creature while standing in water?
    bool water_attack = false;

    bool bearing_shield = (you.equip[EQ_SHIELD] != -1);

    int mmov_x = 0;
    char str_pass[80];
    int specdam = 0;
    char heads = 0;             // for hydras {dlb}
    int hand_used = 0;
    int extraDamage = 0;            // from special mon. attacks (burn, freeze, etc)
    int resistValue = 0;           // player resist value (varies)
    int wepSpd;                    // monster weapon speed

    if (attacker->type == MONS_HYDRA)
        heads = attacker->number;

    if (mons_friendly(attacker))
        return;

    if (attacker->type == MONS_GIANT_SPORE
        || attacker->type == MONS_BALL_LIGHTNING)
    {
        attacker->hit_points = -1;
        return;
    }

    // if a friend wants to help,  they can attack <monster_attacking>
    you.pet_target = monster_attacking;

    if (monster_habitat(attacker->type) != DNGN_FLOOR && attacker->number == 1)
        return;

    if (wearing_amulet(AMU_WARDING)
        || (you.religion == GOD_VEHUMET && you.duration[DUR_PRAYER]
            && (!player_under_penance() && you.piety >= 75)))
    {
        if (mons_has_ench(attacker, ENCH_ABJ_I, ENCH_ABJ_VI))
        {
            // should be scaled {dlb}
            if (coinflip())
            {
                simple_monster_message(attacker,
                                   " tries to attack you, but flinches away.");
                return;
            }
        }
    }

    if (grd[attacker->x][attacker->y] == DNGN_SHALLOW_WATER
        && !mons_flies(attacker->type)
        && monster_habitat(attacker->type) == DNGN_FLOOR && one_chance_in(4))
    {
        simple_monster_message(attacker, " splashes around in the water.");
        return;
    }

    if (player_in_water() && you.species != SP_MERFOLK
        && monster_habitat(attacker->type) == DNGN_DEEP_WATER)
    {
        water_attack = true;
        simple_monster_message(attacker,
                               " uses the watery terrain to its advantage.");
    }

    char runthru;

    for (runthru = 0; runthru < 4; runthru++)
    {
        blocked = false;
        wepSpd = 0;             // 0 = didn't attack w/ weapon

        if (attacker->type == MONS_HYDRA)
        {
            if (heads < 1)
                break;
            runthru = 0;
            heads--;
        }

        char mdam = mondamage(attacker->type, runthru);

        if (attacker->type == MONS_ZOMBIE_SMALL
            || attacker->type == MONS_ZOMBIE_LARGE
            || attacker->type == MONS_SKELETON_SMALL
            || attacker->type == MONS_SKELETON_LARGE
            || attacker->type == MONS_SIMULACRUM_SMALL
            || attacker->type == MONS_SIMULACRUM_LARGE
            || attacker->type == MONS_SPECTRAL_THING)
        {
            mdam = mondamage(attacker->number, runthru);

            // these are cumulative, of course: {dlb}
            if (mdam > 1)
                mdam--;
            if (mdam > 4)
                mdam--;
            if (mdam > 11)
                mdam--;
            if (mdam > 14)
                mdam--;
        }

        if (mdam == 0)
            break;

        if (attacker->type == MONS_TWO_HEADED_OGRE
            && runthru == 1 && attacker->inv[MSLOT_MISSILE] != NON_ITEM)
            hand_used = 1;

        damage_taken = 0;

        int mons_to_hit = 16 + attacker->hit_dice; //* attacker->hit_dice;//* 3

        if (water_attack)
            mons_to_hit += 5;

        if (attacker->inv[hand_used] != NON_ITEM)
        {
            it_name(attacker->inv[hand_used], 0, str_pass);

            if (mitm.base_type[attacker->inv[hand_used]] != 0
                || strstr(str_pass, "questionable item") != NULL)
            {
                destroy_item(attacker->inv[hand_used]);
                attacker->inv[hand_used] = NON_ITEM;
            }
        }

        if (attacker->inv[hand_used] != NON_ITEM)
        {
            if (mitm.pluses[attacker->inv[hand_used]] - 50 > 130)
                mons_to_hit += mitm.pluses[attacker->inv[hand_used]] - 50 - 100;
            else
                mons_to_hit += mitm.pluses[attacker->inv[hand_used]] - 50;

            mons_to_hit += property(OBJ_WEAPONS,
                                     mitm.sub_type[attacker->inv[MSLOT_WEAPON]],
                                     PWPN_HIT);

            wepSpd = property(mitm.base_type[attacker->inv[hand_used]],
                mitm.sub_type[attacker->inv[hand_used]], PWPN_SPEED);
        }

        // Factors against blocking
        const int con_block = 15 + attacker->hit_dice
                               + (5 * you.shield_blocks * you.shield_blocks);

        // Factors for blocking
        const int pro_block = player_shield_class() + (random2(you.dex) / 5);

        if (player_shield_class() > 0 && !you.paralysis && !you.conf
            && random2(con_block) <= random2(pro_block))
        {
            you.shield_blocks++;

            strcpy(info, "You block ");

            int mmov_x = attacker->inv[hand_used];

            if (attacker->type == MONS_DANCING_WEAPON)
            {
                char str_pass[80];

                it_name(mmov_x, 0, str_pass);
                strcpy(info, str_pass);
            }
            else
            {
                strcat(info, ptr_monam(attacker, 1));
            }

            strcat(info, "'s attack.");
            mpr(info);

            blocked = true;
            hit = false;

            if (bearing_shield && one_chance_in(4))
                exercise(SK_SHIELDS, 1);
        }
        else if (player_light_armour() && one_chance_in(3))
        {
            exercise(SK_DODGING, 1);
        }

        const int player_dodge = random2limit(player_evasion(), 40)
                                                + random2(you.dex) / 3 - 2;
        if (!blocked
            && (random2(mons_to_hit) >= player_dodge || one_chance_in(15)))
        {
            hit = true;

            int damage_size = 0;

            if (attacker->inv[hand_used] != NON_ITEM
                && mitm.base_type[attacker->inv[hand_used]] == OBJ_WEAPONS
                && (mitm.sub_type[attacker->inv[hand_used]] < WPN_SLING
                    || mitm.sub_type[attacker->inv[hand_used]] > WPN_CROSSBOW))
            {
                damage_size =
                        property(mitm.base_type[attacker->inv[hand_used]],
                                 mitm.sub_type[attacker->inv[hand_used]],
                                 PWPN_DAMAGE);

                damage_taken = random2(damage_size);

                if (mitm.special[attacker->inv[hand_used]] / 30 == DWPN_ORCISH
                    && mons_charclass(attacker->type) == MONS_ORC
                    && coinflip())
                {
                    damage_taken++;
                }

                if (mitm.pluses2[attacker->inv[hand_used]] > 130)
                {
                    if (mitm.pluses2[attacker->inv[hand_used]] - 150 >= 0)
                    {
                        /* + or 0 to-dam */
                        damage_taken +=
                                random2(mitm.pluses2[attacker->inv[hand_used]]
                                                            - 150 + 1);
                    }
                    else
                    {
                        /* - to-dam */
                        damage_taken -=
                            random2(abs(mitm.pluses2[attacker->inv[hand_used]]
                                                            - 150 + 1));
                    }
                }
                else
                {
                    if (mitm.pluses2[attacker->inv[hand_used]] - 50 >= 0)
                    {
                        /* + or 0 to-dam */
                        damage_taken +=
                            random2(mitm.pluses2[attacker->inv[hand_used]]
                                                            - 50 + 1);
                    }
                    else
                    {
                        /* - to-dam */
                        damage_taken -=
                            random2(abs(mitm.pluses2[attacker->inv[hand_used]]
                                                            - 50 + 1));
                    }
                }

                damage_taken -= 1 + random2(3); //1;
            }

            damage_size += mdam;
            damage_taken += 1 + random2(mdam);

            if (water_attack)
                damage_taken *= 2;

            int ac = player_AC();

            if (ac > 0)
            {
                int damage_reduction = random2(ac + 1);

                if (!player_light_armour())
                {
                    const int body_arm_ac = property(OBJ_ARMOUR,
                                        you.inv_type[you.equip[EQ_BODY_ARMOUR]],
                                        PARM_AC);

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

            if (damage_taken < 1)
                damage_taken = 0;

        }
        else if (!blocked)
        {
            hit = false;
            simple_monster_message(attacker, " misses you.");
        }

        if (damage_taken < 1 && hit && !blocked)
        {
            simple_monster_message(attacker,
                                    " hits you but doesn't do any damage.");
        }

        if (damage_taken > 0)
        {
            hit = true;

            mmov_x = attacker->inv[hand_used];

            if (attacker->type == MONS_DANCING_WEAPON)
            {
                it_name(mmov_x, 0, str_pass);
                strcpy(info, str_pass);
            }
            else
            {
                strcpy(info, ptr_monam(attacker, 0));
            }

            strcat(info, " hits you");

#ifdef WIZARD
            itoa(mmov_x, st_prn, 10);
            strcat(info, " it:");
            // note: doesn't take account of special weapons etc
            strcat(info, st_prn);
#endif

            if (attacker->type != MONS_DANCING_WEAPON && mmov_x != NON_ITEM
                && mitm.base_type[mmov_x] == OBJ_WEAPONS
                && !launches_things(mitm.sub_type[mmov_x]))
            {
                strcat(info, " with ");
                it_name(mmov_x, 3, str_pass);   // was 7
                strcat(info, str_pass);
            }

            strcat(info, "!");

            mpr( info, MSGCH_PLAIN );

            if (hit)
            {
                if (you.equip[EQ_BODY_ARMOUR] != -1)
                {
                    const int body_arm_mass = mass(OBJ_ARMOUR,
                                    you.inv_type[you.equip[EQ_BODY_ARMOUR]]);

                    if (!player_light_armour() && coinflip()
                        && random2(1000) <= body_arm_mass)
                    {
                        // raised from 1 {bwross}
                        exercise(SK_ARMOUR, (coinflip() ? 2 : 1));
                    }
                }
            }

            /* special attacks: */
            int mclas = attacker->type;

            if (mclas == MONS_KILLER_KLOWN)
            {
                switch (random2(6))
                {
                case 0:
                    // comment and enum do not match {dlb}
                    mclas = MONS_SNAKE; // scorp
                    break;
                case 1:
                    mclas = MONS_NECROPHAGE;
                    break;
                case 2:
                    mclas = MONS_WRAITH;
                    break;
                case 3:
                    mclas = MONS_FIRE_ELEMENTAL;
                    break;
                case 4:
                    mclas = MONS_ICE_BEAST;
                    break;
                case 5:
                    mclas = MONS_PHANTOM;
                    break;
                }
            }

            switch (mclas)
            {
            case MONS_GIANT_ANT:
            case MONS_WOLF_SPIDER:
            case MONS_REDBACK:
                if (player_res_poison())
                    break;

                if (one_chance_in(20)
                    || (damage_taken > 3 && one_chance_in(4)))
                {
                    strcpy(info, ptr_monam(attacker, 0));
                    strcat(info, " stings you!");
                    mpr(info);
                    you.poison++;

                    if (attacker->type == MONS_REDBACK)
                        you.poison += random2avg(9, 2) + 3;
                }
                break;

            case MONS_KILLER_BEE:
            case MONS_BUMBLEBEE:
                if (!player_res_poison()
                    && (one_chance_in(20)
                        || (damage_taken > 2 && one_chance_in(3))))

                {
                    strcpy(info, ptr_monam(attacker, 0));
                    strcat(info, " stings you!");
                    mpr(info);
                    you.poison++;

                    if (attacker->type == MONS_BUMBLEBEE)
                        you.poison += random2(3);
                }
                break;

            case MONS_ROTTING_DEVIL:
            case MONS_NECROPHAGE:
            case MONS_GHOUL:
            case MONS_DEATH_OOZE:
                if (you.is_undead)
                    break;

                // both sides call random2() - looking familiar by now {dlb}
                if (one_chance_in(20) || (damage_taken > 2 && one_chance_in(3)))
                {
                    mpr("You feel your flesh start to rot away!");
                    you.rotting += random2(3) + 1;
                }

                if (one_chance_in(4))
                {
                    mpr("You feel ill.");

                    if (you.disease > 100)
                        you.disease = 210;
                    else
                        you.disease += 50 + random2(100);
                }
                break;

            case MONS_GIANT_MOSQUITO:
                if (!one_chance_in(3))
                    mpr("You feel ill.");

                if (you.disease > 100)
                    you.disease = 210;
                else
                    you.disease += 50 + random2(100);
                break;

            case MONS_FIRE_VORTEX:
                attacker->hit_points = -10;
                // fall through -- intentional? {dlb}
            case MONS_FIRE_ELEMENTAL:
            case MONS_BALRUG:
            case MONS_SUN_DEMON:
                strcpy(info, "You are engulfed in flames");

                resistValue = player_res_fire() - 100;
                extraDamage = 15 + random2(15);
                if (resistValue > 0)
                {
                    extraDamage /= (1 + resistValue * resistValue);
                }
                else
                {
                    if (resistValue < 0)
                        extraDamage += 8 + random2(8);
                }

                strcat(info, (extraDamage < 10) ? "." :
                             (extraDamage < 25) ? "!" :
                             "!!");

                mpr(info);
                damage_taken += extraDamage;
                scrolls_burn(1, OBJ_SCROLLS);
                break;

            case MONS_SMALL_SNAKE:
            case MONS_SNAKE:
            case MONS_GIANT_MITE:
            case MONS_GOLD_MIMIC:
            case MONS_WEAPON_MIMIC:
            case MONS_ARMOUR_MIMIC:
            case MONS_SCROLL_MIMIC:
            case MONS_POTION_MIMIC:
                if (!player_res_poison()
                    && (one_chance_in(15)
                        || (damage_taken > 2 && one_chance_in(4))))
                {
                    strcpy(info, ptr_monam(attacker, 0));
                    strcat(info, " poisons you!");
                    mpr(info);

                    you.poison++;
                }
                break;

            case MONS_QUEEN_BEE:
            case MONS_GIANT_CENTIPEDE:
            case MONS_SOLDIER_ANT:
            case MONS_QUEEN_ANT:
                if (!player_res_poison())
                {
                    strcpy(info, ptr_monam(attacker, 0));
                    strcat(info, " stings you!");
                    mpr(info);
                    you.poison += 2;
                }
                break;

            case MONS_SCORPION:
            case MONS_BROWN_SNAKE:
            case MONS_BLACK_SNAKE:
            case MONS_YELLOW_SNAKE:
                if (!player_res_poison()
                    && (one_chance_in(20)
                        || (damage_taken > 2 && one_chance_in(4))))
                        // ^^^yep, this should be a function^^^ {dlb}
                {
                    strcpy(info, ptr_monam(attacker, 0));
                    strcat(info, " poisons you!");
                    mpr(info);
                    you.poison++;
                }
                break;

            case MONS_SHADOW_DRAGON:
            case MONS_SPECTRAL_THING:
                if (coinflip())
                    break;
                // fall-through {dlb}
            case MONS_WIGHT:    // less likely because wights do less damage
            case MONS_WRAITH:

            // enum does not match comment 14jan2000 {dlb}
            case MONS_SOUL_EATER:       // shadow devil

            // enum does not match comment 14jan2000 {dlb}
            case MONS_SPECTRAL_WARRIOR: // spectre

            case MONS_SHADOW_FIEND:
            case MONS_ORANGE_RAT:
            case MONS_SHADOW_WRAITH:
            case MONS_ANCIENT_LICH:
                if (one_chance_in(30) || (damage_taken > 5 && coinflip()))
                    drain_exp();
                break;

            case MONS_RED_WASP:
                if (!player_res_poison())
                    you.poison += (coinflip()? 2 : 1);
                // intentional fall-through {dlb}
            case MONS_YELLOW_WASP:
                strcpy(info, ptr_monam(attacker, 0));
                strcat(info, " stings you.");
                mpr(info);

                if (!player_res_poison()
                    && (one_chance_in(20)
                        || (damage_taken > 2 && !one_chance_in(3))))
                        // maybe I should flip back the other way? {dlb}
                {
                    if (you.paralysis > 0)
                        mpr("You still can't move!");
                    else
                        mpr("You suddenly lose the ability to move!");

                    you.paralysis += 1 + random2(3);
                }
                break;

            case MONS_SPINY_WORM:
                if (!player_res_poison())
                {
                    strcpy(info, ptr_monam(attacker, 0));
                    strcat(info, " stings you!");
                    mpr(info);

                    you.poison += 2 + random2(4);
                }
                // intentional fall-through {dlb}
            case MONS_BROWN_OOZE:
            case MONS_ACID_BLOB:
            case MONS_ROYAL_JELLY:
            case MONS_JELLY:
                mpr("You are splashed with acid!");
                splash_with_acid(3);
                break;

            case MONS_ICE_DEVIL:
            case MONS_ICE_BEAST:
            case MONS_FREEZING_WRAITH:
            case MONS_ICE_FIEND:
            case MONS_WHITE_IMP:
            case MONS_ANTAEUS:
            case MONS_AZURE_JELLY:
                extraDamage = attacker->hit_dice +
                    random2(attacker->hit_dice * 2);
                resistValue = player_res_cold() - 100;

                if (resistValue > 0)
                {
                    extraDamage /= (1 + resistValue * resistValue);
                }
                if (resistValue < 0)
                {
                    extraDamage += attacker->hit_dice +
                        random2(attacker->hit_dice * 2);
                }

                if (extraDamage > 4)
                {
                    strcpy(info, ptr_monam(attacker, 0));
                    if (extraDamage < 10)
                        strcat(info, " chills you.");
                    else
                        strcat(info, " freezes you!");
                    if (extraDamage > 19)
                        strcat(info, "!");
                    mpr(info);
                }

                damage_taken += extraDamage;

                scrolls_burn(1, OBJ_POTIONS);
                break;

            case MONS_ELECTRIC_GOLEM:
                if (!player_res_electricity())
                {
                    damage_taken += attacker->hit_dice
                                            + random2(attacker->hit_dice * 2);

                    strcpy(info, ptr_monam(attacker, 0));
                    strcat(info, " shocks you!");
                    mpr(info);
                }
                break;

            case MONS_VAMPIRE:
                if (you.is_undead)
                    break;

/* ******************************************************************
                if ( damage_taken > 6 && one_chance_in(3) || !one_chance_in(20))
                {
                    mpr("You feel less resilient.");
                    you.hp_max -= ( coinflip() ? 2 : 1 );
                    deflate_hp(you.hp_max, false);
                    heal_monster(attacker, 5 + random2(8), true);
                }
****************************************************************** */

                // heh heh {dlb}
                // oh, this is mean!  {gdl}   :)
                if (heal_monster(attacker, random2(damage_taken), true))
                    simple_monster_message(attacker, " draws strength from your injuries!");

                break;

            case MONS_SHADOW:
                if (player_prot_life() <= random2(3)
                    && (one_chance_in(20)
                        || (damage_taken > 0 && one_chance_in(3))))
                {
                    lose_stat(STAT_STRENGTH, 1);
                }
                break;

            case MONS_HUNGRY_GHOST:
                if (you.is_undead == US_UNDEAD)
                    break;

                if (one_chance_in(20) || (damage_taken > 0 && coinflip()))
                    make_hungry(400, false);
                break;

            case MONS_GUARDIAN_NAGA:
                break;

            case MONS_PHANTOM:
            case MONS_INSUBSTANTIAL_WISP:
            case MONS_BLINK_FROG:
            case MONS_MIDGE:
                if (one_chance_in(3))
                {
                    simple_monster_message(attacker, " blinks.");
                    monster_blink(attacker);
                }
                break;

            case MONS_JELLYFISH:
            case MONS_ORANGE_DEMON:
                // if ( !one_chance_in(3) ) break;
                if (player_res_poison())
                    break;

                if (attacker->type == MONS_ORANGE_DEMON
                    && (!one_chance_in(4) || runthru != 1))
                {
                    break;
                }

                strcpy(info, ptr_monam(attacker, 0));
                strcat(info, " stings you!");
                mpr(info);
                you.poison++;
                lose_stat(STAT_STRENGTH, 1);
                break;

            case MONS_PULSATING_LUMP:
                if (one_chance_in(3))
                    mutate(100);
                break;
            }                   // end of switch for special attacks.
            /* use brek for level drain, maybe with beam variables,
               because so many creatures use it. */
        }

        /* special weapons */
        if (hit
            && (attacker->inv[hand_used] != NON_ITEM
                || ((attacker->type == MONS_PLAYER_GHOST
                        || attacker->type == MONS_PANDEMONIUM_DEMON)
                    && ghost.values[8] != 0)))
        {
            unsigned char itdam;

            if (attacker->type == MONS_PLAYER_GHOST
                || attacker->type == MONS_PANDEMONIUM_DEMON)
            {
                itdam = ghost.values[8];
            }
            else
                itdam = mitm.special[attacker->inv[hand_used]];

            specdam = 0;

            if (itdam == NWPN_SWORD_OF_CEREBOV)
                goto flaming;

            if (attacker->type == MONS_PLAYER_GHOST
                || attacker->type == MONS_PANDEMONIUM_DEMON
                || mitm.special[attacker->inv[hand_used]] < NWPN_SINGING_SWORD)
            {
                switch (itdam % 30)
                {
                case SPWPN_NORMAL:
                    break;

                case SPWPN_FLAMING:
                  flaming:
                    specdam = 0;

                    if (player_res_fire() > 100)
                    {
                        damage_taken += (random2(damage_taken) / 2 + 1) /
                                            (1 + (player_res_fire() - 100)
                                                * (player_res_fire() - 100));
                    }

                    if (player_res_fire() <= 100)
                        specdam = random2(damage_taken) / 2 + 1;

                    if (player_res_fire() < 100)
                        specdam += random2(damage_taken) / 2 + 1;

                    if (specdam)
                    {
                        simple_monster_message(attacker, " burns you.");
/* **********************

commented out for now
                        if (specdam < 3)
                          strcat(info, ".");
                        if (specdam >= 3 && specdam < 7)
                          strcat(info, "!");
                        if (specdam >= 7)
                          strcat(info, "!!");

*********************** */
                    }
                    break;

                case SPWPN_FREEZING:
                    specdam = 0;

                    if (player_res_cold() <= 100)
                        specdam = random2(damage_taken) / 2 + 1;

                    if (player_res_cold() < 100)
                        specdam += random2(damage_taken) / 2 + 1;

                    if (player_res_cold() > 100)
                    {
                        damage_taken += (random2(damage_taken) / 2 + 1) /
                                            (1 + (player_res_cold() - 100)
                                                * (player_res_cold() - 100));
                    }

                    if (specdam)
                    {
                        simple_monster_message(attacker, " freezes you.");

/* **********************

commented out for now
                        if (specdam < 3)
                          strcat(info, ".");
                        if (specdam >= 3 && specdam < 7)
                          strcat(info, "!");
                        if (specdam >= 7)
                          strcat(info, "!!");

*********************** */
                    }
                    break;


                case SPWPN_HOLY_WRATH:
                    if (attacker->type == MONS_PLAYER_GHOST)
                        break;  // ghosts can't wield holy wrath

                    if (you.is_undead)
                    {
                        specdam = random2(damage_taken);

                        if (specdam)
                        {
                            strcpy(info, "The wound is extremely painful");

                            if (specdam < 3)
                                strcat(info, ".");
                            else if (specdam < 7)
                                strcat(info, "!");
                            else
                                strcat(info, "!!");

                            mpr(info);
                        }
                    }
                    break;

                case SPWPN_ELECTROCUTION:
                    // This runs the risk of making levitation into a
                    // cheap version of the Resist spell (with added
                    // bonuses), so it shouldn't be used anywhere else,
                    // and should possibly be removed from this case as
                    // well. -- bwr
                    if (you.levitation) // you're not grounded
                        break;

                    if (player_res_electricity())   // resist lightning
                        break;
                    //if (menv [monster_attacking].type == MONS_PLAYER_GHOST) break;
                    specdam = 0;

                    if (mitm.pluses2[attacker->inv[hand_used]] <= 50
                        || mitm.pluses2[attacker->inv[hand_used]] > 130
                        && mitm.pluses2[attacker->inv[hand_used]] <= 150)
                    {
                        break;
                    }

                    if (one_chance_in(3))
                    {
                        mpr("You are electrocuted!");
                        specdam += 10 + random2(15);
                    }
                    break;

                case SPWPN_ORC_SLAYING:
                    if (you.species == SP_HILL_ORC)
                    {
                        specdam = random2(damage_taken);

                        if (specdam)
                        {
                            strcpy(info, "The wound is extremely painful");

                            if (specdam < 3)
                                strcat(info, ".");
                            else if (specdam < 7)
                                strcat(info, "!");
                            else
                                strcat(info, "!!");

                            mpr(info);
                        }
                    }
                    break;

                case SPWPN_VENOM:
                    if (!player_res_poison() && one_chance_in(3))
                    {
                        simple_monster_message(attacker,
                                (attacker->type == MONS_DANCING_WEAPON)
                                ? " is poisoned!" : "'s weapon is poisoned!");

                        you.poison += 2;
                    }
                    break;

                case SPWPN_PROTECTION:
                    break;

                case SPWPN_DRAINING:
                    drain_exp();
                    specdam = random2(damage_taken) / (2 + player_prot_life()) + 1;
                    break;

                case SPWPN_SPEED:
                    wepSpd = wepSpd / 2 + 1;
                    break;

                case SPWPN_VORPAL:
                    specdam = 1 + (random2(damage_taken) / 2);
                    break;

                case SPWPN_VAMPIRICISM:
                    specdam = 0;        // note does no extra damage

                    if (you.is_undead)
                        break;

                    if (one_chance_in(5))
                        break;

                    // heh heh {dlb}
                    if (heal_monster(attacker, 1 + random2(damage_taken), true))
                        simple_monster_message(attacker, " draws strength from your injuries!");
                    break;

                case SPWPN_DISRUPTION:
                    if (attacker->type == MONS_PLAYER_GHOST)
                        break;

                    if (you.is_undead)
                    {
                        specdam = random2(damage_taken) + random2(damage_taken)
                                + random2(damage_taken) + random2(damage_taken);

                        if (specdam)
                        {
                            strcpy(info, "You are blasted by holy energy");

                            if (specdam < 7)
                                strcat(info, ".");
                            else if (specdam < 15)
                                strcat(info, "!");
                            else
                                strcat(info, "!!");

                            mpr(info);
                        }
                    }
                    break;


                case SPWPN_DISTORTION:
                    //if ( !one_chance_in(3) ) break;

                    if (one_chance_in(3))
                    {
                        mpr("Your body is twisted painfully.");
                        specdam += 1 + random2avg(7, 2);
                        break;
                    }

                    if (one_chance_in(3))
                    {
                        mpr("Your body is terribly warped!");
                        specdam += 3 + random2avg(24, 2);
                        break;
                    }

                    if (one_chance_in(3))
                    {
                        random_blink(true);
                        break;
                    }

                    if (coinflip())
                    {
                        you_teleport();
                        break;
                    }

                    if (coinflip())
                    {
                        you_teleport2(true);
                        break;
                    }

                    if (coinflip())
                    {
                        banished(DNGN_ENTER_ABYSS);
                        break;
                    }
                    break;
                }               // end of switch
            }
        }                       // end of special weapons

        damage_taken += specdam;

        if (damage_taken > 0)
        {
            ouch(damage_taken, monster_attacking, KILLED_BY_MONSTER);

            if (you.religion == GOD_XOM && you.hp <= you.hp_max / 3
                && one_chance_in(10))
            {
                Xom_acts(true, you.experience_level, false);
            }
        }

        // adjust time taken if monster used weapon
        if (wepSpd > 0)
        {
            // only get one third penalty/bonus for second weapons.
            if (runthru > 0)
                wepSpd = (20 + wepSpd) / 3;
            attacker->speed_increment -= (wepSpd - 10) / 2;
        }
    }                           // end of for runthru

    return;
}                               // end monster_attack()

bool monsters_fight(int monster_attacking, int monster_attacked)
{
    struct monsters *attacker = &menv[monster_attacking];
    struct monsters *defender = &menv[monster_attacked];

    int weapon = -1;            // monster weapon, if any
    int damage_taken = 0;
    bool hit = false;
    int mmov_x = 0;
    char str_pass[80];
    bool water_attack = false;
    int specdam = 0;
    int hand_used = 0;
    bool sees = false;
    int wepSpd;               // 0 == didn't use actual weapon

    if (attacker->type == MONS_GIANT_SPORE
        || attacker->type == MONS_BALL_LIGHTNING)
    {
        attacker->hit_points = -1;
        return false;
    }

    if (monster_habitat(attacker->type) != DNGN_FLOOR && attacker->number == 1
        && monster_habitat(defender->type) == DNGN_FLOOR)
    {
        return false;
    }

    if (grd[attacker->x][attacker->y] == DNGN_SHALLOW_WATER
        && !mons_flies(attacker->type)
        && monster_habitat(attacker->type) == DNGN_FLOOR && one_chance_in(4))
    {
        mpr("You hear a splashing noise.");
        return true;
    }

    if (grd[defender->x][defender->y] == DNGN_SHALLOW_WATER
        && !mons_flies(defender->type)
        && monster_habitat(attacker->type) == DNGN_DEEP_WATER)
    {
        water_attack = true;
    }

    if (mons_near(attacker) && mons_near(defender))
        sees = true;

    // now disturb defender, regardless
    behavior_event(defender, ME_WHACK, monster_attacking);

    char runthru;

    for (runthru = 0; runthru < 4; runthru++)
    {
        char mdam = mondamage(attacker->type, runthru);
        wepSpd = 0;

        if (attacker->type == MONS_ZOMBIE_SMALL
            || attacker->type == MONS_ZOMBIE_LARGE
            || attacker->type == MONS_SKELETON_SMALL
            || attacker->type == MONS_SKELETON_LARGE
            || attacker->type == MONS_SIMULACRUM_SMALL
            || attacker->type == MONS_SIMULACRUM_LARGE
            || attacker->type == MONS_SPECTRAL_THING)
            // what do these things have in common? {dlb}
        {
            mdam = mondamage(attacker->number, runthru);
            // cumulative reductions - series of if-conditions
            // is necessary: {dlb}
            if (mdam > 1)
                mdam--;
            if (mdam > 2)
                mdam--;
            if (mdam > 5)
                mdam--;
            if (mdam > 9)
                mdam--;         // was: "-= 2" {dlb}
        }

        if (mdam == 0)
            break;

        if (attacker->type == MONS_TWO_HEADED_OGRE
            && runthru == 1 && attacker->inv[MSLOT_MISSILE] != NON_ITEM)
        {
            hand_used = 1;
        }

        damage_taken = 0;

        int mons_to_hit = 20 + attacker->hit_dice;
                                // * menv [monster_attacking].hit_dice; // * 3

        if (water_attack)
            mons_to_hit += 5;

        weapon = attacker->inv[hand_used];

        if (weapon != NON_ITEM)
        {
            if (mitm.pluses[weapon] - 50 > 130)
                mons_to_hit += mitm.pluses[weapon] - 150;
            else
                mons_to_hit += mitm.pluses[weapon] - 50;

            mons_to_hit += 3 * property(OBJ_WEAPONS, mitm.sub_type[weapon],
                PWPN_HIT);

            wepSpd = property(mitm.base_type[ weapon ], mitm.sub_type[ weapon ],
                PWPN_SPEED);
        }

        mons_to_hit = random2(mons_to_hit);

        if (mons_to_hit >= defender->evasion
            || ((defender->speed_increment <= 60
                 || defender->behavior == BEH_SLEEP) && !one_chance_in(20)))
        {
            hit = true;

            if (attacker->inv[hand_used] != NON_ITEM
                && mitm.base_type[attacker->inv[hand_used]] == OBJ_WEAPONS
                && !launches_things(mitm.sub_type[attacker->inv[hand_used]]))
            {
                damage_taken =
                    random2(property(
                                mitm.base_type[attacker->inv[hand_used]],
                                mitm.sub_type[attacker->inv[hand_used]],
                                PWPN_DAMAGE));

                if (mitm.special[attacker->inv[hand_used]] / 30 == DWPN_ORCISH
                    && mons_charclass(attacker->type) == MONS_ORC
                    && coinflip())
                {
                    damage_taken++;
                }

                //if (mitm.pluses[mons_inv[i][0]] > 80) damage_taken -= 100;
                //  damage_taken += mitm.pluses[mons_inv[i][0]];
                if (mitm.pluses2[attacker->inv[hand_used]] > 130)
                {
                    if (mitm.pluses2[attacker->inv[hand_used]] - 150 >= 0)
                    {
                        /* + or 0 to-dam */
                        damage_taken +=
                                random2(mitm.pluses2[attacker->inv[hand_used]]
                                                        - 150 + 1);
                    }
                    else
                    {
                        /* - to-dam */
                        damage_taken -=
                            random2(abs(mitm.pluses2[attacker->inv[hand_used]]
                                                        - 150 + 1));
                    }
                }
                else
                {
                    if (mitm.pluses2[attacker->inv[hand_used]] - 50 >= 0)
                    {
                        /* + or 0 to-dam */
                        damage_taken +=
                            random2(mitm.pluses2[attacker->inv[hand_used]]
                                                        - 50 + 1);
                    }
                    else
                    {
                        /* - to-dam */
                        damage_taken -=
                            random2(abs(mitm.pluses2[attacker->inv[hand_used]]
                                                        - 50 + 1));
                    }
                }
                damage_taken -= 1 + random2(3); //1;
            }

            damage_taken += 1 + random2(mdam);

            if (water_attack)
                damage_taken *= 2;

            damage_taken -= random2(1 + defender->armor_class);

            if (damage_taken < 1)
                damage_taken = 0;
        }
        else
        {
            hit = false;

            int mmov_x = attacker->inv[hand_used];

            char str_pass[80];

            if (sees)
            {
                if (attacker->type == MONS_DANCING_WEAPON)
                {
                    it_name(mmov_x, 0, str_pass);
                    strcpy(info, str_pass);
                }
                else
                {
                    strcpy(info, ptr_monam(attacker, 0));
                }

                strcat(info, " misses ");
                strcat(info, ptr_monam(defender, 1));
                strcat(info, ".");
                mpr(info);
            }
        }

        if (damage_taken < 1 && hit)
        {
            int mmov_x = attacker->inv[hand_used];

            char str_pass[80];

            if (sees)
            {
                if (attacker->type == MONS_DANCING_WEAPON)
                {
                    it_name(mmov_x, 0, str_pass);
                    strcpy(info, str_pass);
                }
                else
                {
                    strcpy(info, ptr_monam(attacker, 0));
                }

                strcat(info, " hits ");
                strcat(info, ptr_monam(defender, 1));

#ifdef WIZARD
                itoa(damage_taken, st_prn, 10);
                strcat(info, " for ");
                // note: doesn't take account of special weapons etc
                strcat(info, st_prn);
#endif
                strcat(info, ".");      // but doesn't do any you.damage.");
                mpr(info);
            }
        }

        if (hit)                //(int) damage_taken > 0)
        {
            int mmov_x = attacker->inv[hand_used];

            if (sees)
            {
                if (attacker->type == MONS_DANCING_WEAPON)
                {
                    char str_pass[80];

                    it_name(mmov_x, 0, str_pass);
                    strcpy(info, str_pass);
                }
                else
                {
                    strcpy(info, ptr_monam(attacker, 0));
                }

                strcat(info, " hits ");
                strcat(info, ptr_monam(defender, 1));

                if (attacker->type != MONS_DANCING_WEAPON
                    && attacker->inv[hand_used] != NON_ITEM
                    && mitm.base_type[attacker->inv[hand_used]] == OBJ_WEAPONS
                    && !launches_things(mitm.sub_type[attacker->inv[hand_used]]))
                {
                    strcat(info, " with ");
                    it_name(mmov_x, 3, str_pass);       // was 7
                    strcat(info, str_pass);
                }

                strcat(info, "! ");
                mpr(info);
            }

            // special attacks:
            switch (attacker->type)
            {
            // enum does not match comment 14jan2000 {dlb}
            case MONS_CENTAUR:  // cockatrice
            case MONS_JELLY:
            case MONS_GUARDIAN_NAGA:
                break;

            case MONS_GIANT_ANT:
            case MONS_WOLF_SPIDER:
            case MONS_REDBACK:
            case MONS_SPINY_WORM:
            case MONS_JELLYFISH:
            case MONS_ORANGE_DEMON:
                if (attacker->type == MONS_SPINY_WORM || one_chance_in(20)
                    || (damage_taken > 3 && one_chance_in(4)))
                {
                    if (sees)
                    {
                        strcpy(info, ptr_monam(attacker, 0));
                        strcat(info, " stings ");
                        strcat(info, ptr_monam(defender, 1));
                        strcat(info, ".");
                        mpr(info);
                    }
                    poison_monster(defender, false);
                }
                break;

            case MONS_KILLER_BEE:
            case MONS_BUMBLEBEE:
                if (one_chance_in(20)
                    || (damage_taken > 2 && one_chance_in(3)))
                {
                    if (sees)
                    {
                        strcpy(info, ptr_monam(attacker, 0));
                        strcat(info, " stings ");
                        strcpy(info, ptr_monam(defender, 1));
                        strcat(info, ".");
                        mpr(info);
                    }
                    poison_monster(defender, false);
                }
                break;

            case MONS_NECROPHAGE:
            case MONS_ROTTING_DEVIL:
            case MONS_GHOUL:
            case MONS_DEATH_OOZE:
                if (mons_holiness(defender->type) == MH_UNDEAD
                    || mons_holiness(defender->type) == MH_DEMONIC)
                    break;

                if (one_chance_in(20)
                    || (damage_taken > 2 && one_chance_in(3)))
                {
                    defender->max_hit_points -= 1 + random2(3);

                    if (defender->hit_points > defender->max_hit_points)
                        defender->hit_points = defender->max_hit_points;
                }
                break;

            case MONS_FIRE_VORTEX:
                attacker->hit_points = -10;
                // deliberate fall-through
            case MONS_FIRE_ELEMENTAL:
            case MONS_BALRUG:
            case MONS_SUN_DEMON:
                specdam = 0;
                if (!mons_res_fire(defender->type))
                {
                    specdam = 15 + random2(15);

                    if (defender->inv[MSLOT_ARMOUR] != NON_ITEM
                        && mitm.special[defender->inv[MSLOT_ARMOUR]] % 30
                                                == SPARM_FIRE_RESISTANCE)
                    {
                        specdam /= 3;
                    }
                }

                if (mons_res_fire(defender->type) == -1
                    || (defender->inv[MSLOT_ARMOUR] == NON_ITEM
                        || mitm.special[defender->inv[MSLOT_ARMOUR]] % 30
                                                != SPARM_FIRE_RESISTANCE))
                {
                    specdam = random2(25) + 20;
                }

                if (specdam)
                    simple_monster_message(defender, " is engulfed in flame!");

                damage_taken += specdam;
                break;

            case MONS_QUEEN_BEE:
            case MONS_GIANT_CENTIPEDE:
            case MONS_SOLDIER_ANT:
            case MONS_QUEEN_ANT:
                //if ((damage_taken > 2 && one_chance_in(3) ) || one_chance_in(20) )
                //{
                if (sees)
                {
                    strcpy(info, ptr_monam(attacker, 0));
                    strcat(info, " stings ");
                    strcat(info, ptr_monam(defender, 1));
                    strcat(info, ".");
                    mpr(info);
                }
                poison_monster(defender, false);
                //}
                break;

            // enum does not match comment 14jan2000 {dlb}
            case MONS_SCORPION: // snake
            case MONS_BROWN_SNAKE:
            case MONS_BLACK_SNAKE:
            case MONS_YELLOW_SNAKE:
            case MONS_GOLD_MIMIC:
            case MONS_WEAPON_MIMIC:
            case MONS_ARMOUR_MIMIC:
            case MONS_SCROLL_MIMIC:
            case MONS_POTION_MIMIC:
                if (one_chance_in(20)
                    || (damage_taken > 2 && one_chance_in(4)))
                {
                    simple_monster_message(defender, " is poisoned");
                    poison_monster(defender, false);
                }
                break;

            case MONS_SHADOW_DRAGON:
            case MONS_SPECTRAL_THING:
                if (coinflip())
                    break;
                // intentional fall-through
            case MONS_WIGHT:
            case MONS_WRAITH:
            case MONS_SOUL_EATER:
            case MONS_SHADOW_FIEND:
            case MONS_SPECTRAL_WARRIOR:
            case MONS_ORANGE_RAT:
            case MONS_ANCIENT_LICH:
                if (mons_holiness(defender->type) == MH_UNDEAD
                    || mons_holiness(defender->type) == MH_DEMONIC)
                {
                    break;
                }

                if (one_chance_in(30) || (damage_taken > 5 && coinflip()))
                {
                    simple_monster_message(defender, " is drained.");

                    if (one_chance_in(5))
                        defender->hit_dice--;

                    defender->max_hit_points -= 2 + random2(3);
                    defender->hit_points -= 2 + random2(3);

                    if (defender->hit_points >= defender->max_hit_points)
                        defender->hit_points = defender->max_hit_points;

                    if (defender->hit_points < 1 || defender->hit_dice < 1)
                    {
                        monster_die(defender, KILL_MON, monster_attacking);
                        return true;
                    }
                }
                break;

            // enum does not match comment 14jan2000 {dlb}
            case MONS_WORM:     // giant wasp
                break;

            case MONS_ICE_DEVIL:
            case MONS_ICE_BEAST:
            case MONS_FREEZING_WRAITH:
            case MONS_ICE_FIEND:
            case MONS_WHITE_IMP:
            case MONS_AZURE_JELLY:
            case MONS_ANTAEUS:
                specdam = 0;
                if (!mons_res_cold(defender->type))
                {
                    specdam = attacker->hit_dice
                                        + random2(attacker->hit_dice * 2);

                    if (defender->inv[MSLOT_ARMOUR] != NON_ITEM
                        && mitm.special[defender->inv[MSLOT_ARMOUR]] % 30
                                                    == SPARM_COLD_RESISTANCE)
                    {
                        specdam = (random2(attacker->hit_dice * 2)
                                                + (attacker->hit_dice) / 3);
                    }
                }

                if (mons_res_cold(defender->type) == -1
                    && (defender->inv[MSLOT_ARMOUR] == NON_ITEM
                        || mitm.special[defender->inv[MSLOT_ARMOUR]] % 30
                                                    != SPARM_COLD_RESISTANCE))
                {
                    specdam = random2(attacker->hit_dice * 3)
                                                + (attacker->hit_dice * 2);
                }

                if (specdam)
                {
                    if (sees)
                    {
                        strcpy(info, ptr_monam(attacker, 0));
                        strcat(info, " freezes ");
                        strcat(info, ptr_monam(defender, 1));
                        strcat(info, ".");
                        mpr(info);
                    }
                }
                damage_taken += specdam;
                break;

            case MONS_ELECTRIC_GOLEM:
                if (!mons_flies(defender->type
                    && !mons_res_elec(defender->type)))
                {
                    specdam = attacker->hit_dice
                                        + random2(attacker->hit_dice * 2);
                }

                if (specdam)
                {
                    if (sees)
                    {
                        strcpy(info, ptr_monam(attacker, 0));
                        strcat(info, " shocks ");
                        strcat(info, ptr_monam(defender, 1));
                        strcat(info, ".");
                        mpr(info);
                    }
                }
                damage_taken += specdam;
                break;

            case MONS_VAMPIRE:
                if (mons_holiness(defender->type) == MH_UNDEAD
                    || mons_holiness(defender->type) == MH_DEMONIC)
                {
                    break;
                }

                // heh heh {dlb}
                if (heal_monster(attacker, random2(damage_taken), true))
                    simple_monster_message(attacker, " is healed.");

                break;
            }
        }

        // special weapons:
        if (hit
            && (attacker->inv[hand_used] != NON_ITEM
                || ((attacker->type == MONS_PLAYER_GHOST
                        || attacker->type == MONS_PANDEMONIUM_DEMON)
                     && ghost.values[8] != 0)))
        {
            unsigned char itdam;

            if (attacker->type == MONS_PLAYER_GHOST
                || attacker->type == MONS_PANDEMONIUM_DEMON)
            {
                itdam = ghost.values[8];
            }
            else
                itdam = mitm.special[attacker->inv[hand_used]];

            specdam = 0;

            if (itdam == NWPN_SWORD_OF_CEREBOV)
                goto flaming;

            if (attacker->type == MONS_PLAYER_GHOST
                || attacker->type == MONS_PANDEMONIUM_DEMON
                || mitm.special[attacker->inv[hand_used]] < NWPN_SINGING_SWORD)
            {
                switch (itdam % 30)
                {
                case SPWPN_NORMAL:
                    break;

                case SPWPN_FLAMING:
                  flaming:specdam = 0;
                    if (!mons_res_fire(defender->type))
                    {
                        specdam = 1 + (random2(damage_taken));
                        if (defender->inv[MSLOT_ARMOUR] != NON_ITEM
                            && mitm.special[defender->inv[MSLOT_ARMOUR]] % 30
                                                    == SPARM_FIRE_RESISTANCE)
                        {
                            specdam /= 3;
                        }
                    }

                    if (mons_res_fire(defender->type) == -1
                        && (defender->inv[MSLOT_ARMOUR] == NON_ITEM
                            || mitm.special[defender->inv[MSLOT_ARMOUR]] % 30
                                                    != SPARM_FIRE_RESISTANCE))
                    {
                        specdam = 1 + random2(damage_taken);
                    }

                    if (specdam)
                    {
                        if (sees)
                        {
                            int mmov_x = attacker->inv[hand_used];

                            char str_pass[80];

                            if (attacker->type == MONS_DANCING_WEAPON)
                            {
                                it_name(mmov_x, 0, str_pass);
                                strcpy(info, str_pass);
                            }
                            else
                            {
                                strcpy(info, ptr_monam(attacker, 0));
                            }

                            strcat(info, " burns ");
                            strcat(info, ptr_monam(defender, 1 ));

                            if (specdam < 3)
                                strcat(info, ".");
                            else if (specdam < 7)
                                strcat(info, "!");
                            else
                                strcat(info, "!!");

                            mpr(info);
                        }
                    }
                    break;

                case SPWPN_FREEZING:
                    specdam = 0;

                    if (!mons_res_cold(defender->type))
                    {
                        specdam = 1 + (random2(damage_taken));
                        if (defender->inv[MSLOT_ARMOUR] != NON_ITEM
                            && mitm.special[defender->inv[MSLOT_ARMOUR]] % 30
                                                    == SPARM_COLD_RESISTANCE)
                        {
                            specdam = (1 + (random2(damage_taken) / 2)) / 3;
                        }
                    }

                    if (mons_res_cold(defender->type) == -1
                        && (defender->inv[MSLOT_ARMOUR] == NON_ITEM
                            || mitm.special[defender->inv[MSLOT_ARMOUR]] % 30
                                                    != SPARM_COLD_RESISTANCE))
                    {
                        specdam = 1 + random2(damage_taken);
                    }

                    if (specdam)
                    {
                        if (sees)
                        {
                            mmov_x = attacker->inv[hand_used];

                            if (attacker->type == MONS_DANCING_WEAPON)
                            {
                                it_name(mmov_x, 0, str_pass);
                                strcpy(info, str_pass);
                            }
                            else
                            {
                                strcpy(info, ptr_monam(attacker, 0));
                            }

                            strcat(info, " freezes ");
                            strcat(info, ptr_monam(defender, 1));

                            if (specdam < 3)
                                strcat(info, ".");
                            else if (specdam < 7)
                                strcat(info, "!");
                            else
                                strcat(info, "!!");

                            mpr(info);
                        }
                    }
                    break;

                case SPWPN_HOLY_WRATH:
                    if (attacker->type == MONS_PLAYER_GHOST)
                        break;
                    specdam = 0;
                    switch (mons_holiness(defender->type))
                    {
                    case MH_HOLY:
                        // I would think that it would do zero damage {dlb}
                        damage_taken -= 5 + random2(5);
                        break;

                    case MH_UNDEAD:
                        specdam += 1 + random2(damage_taken);
                        break;

                    case MH_DEMONIC:
                        specdam += 1 + (random2(damage_taken) * 15) / 10;
                        break;
                    }
                    break;

                case SPWPN_ELECTROCUTION:
                    //if ( attacker->type == MONS_PLAYER_GHOST ) break;
                    if (mons_flies(defender->type)
                        || mons_res_elec(defender->type))
                        break;

                    specdam = 0;

                    if (mitm.pluses2[attacker->inv[hand_used]] <= 50
                        || mitm.pluses2[attacker->inv[hand_used]] > 130
                        && mitm.pluses2[attacker->inv[hand_used]] <= 150)
                    {
                        break;
                    }

                    if (one_chance_in(3))
                    {
                        if (sees)
                            mpr("There is a sudden explosion of sparks!");

                        specdam += 10 + random2(15);
                        //mitm.pluses2[attacker->inv[hand_used]] --;
                    }
                    break;

                case SPWPN_ORC_SLAYING:
                    if (mons_charclass(defender->type) == MONS_ORC)
                        hurt_monster(defender, 1 + random2(damage_taken));
                    break;

                case SPWPN_VENOM:
                    if (one_chance_in(3))
                        poison_monster(defender, true);
                    break;

                    //case 7: // protection

                case SPWPN_DRAINING:
                    if ((mons_holiness(defender->type) == MH_UNDEAD
                            || mons_holiness(defender->type) == MH_DEMONIC)
                        && (one_chance_in(30)
                            || (damage_taken > 5 && coinflip())))
                    {
                        simple_monster_message(defender, " is drained");

                        if (one_chance_in(5))
                            defender->hit_dice--;

                        defender->max_hit_points -= 2 + random2(3);
                        defender->hit_points -= 2 + random2(3);

                        if (defender->hit_points >= defender->max_hit_points)
                            defender->hit_points = defender->max_hit_points;

                        if (defender->hit_points < 1
                            || defender->hit_dice < 1)
                        {
                            monster_die(defender, KILL_MON, monster_attacking);
                            return true;
                        }
                        specdam = 1 + (random2(damage_taken) / 2);
                    }
                    break;

                case SPWPN_SPEED:
                    wepSpd = wepSpd / 2 + 1;
                    break;

                case SPWPN_VORPAL:
                    specdam += 1 + (random2(damage_taken) / 2);
                    break;

                case SPWPN_VAMPIRICISM:
                    specdam = 0;        // note does no extra damage

                    if (mons_holiness(defender->type) == MH_UNDEAD
                        || mons_holiness(defender->type) == MH_DEMONIC)
                    {
                        break;
                    }

                    if (one_chance_in(5))
                        break;

                    // heh heh {dlb}
                    if (heal_monster(attacker, 1 + random2(damage_taken), true))
                        simple_monster_message(attacker, " is healed.");
                    break;

                case SPWPN_DISRUPTION:
                    if (attacker->type == MONS_PLAYER_GHOST)
                        break;

                    specdam = 0;

                    if (mons_holiness(defender->type) == MH_UNDEAD
                        && !one_chance_in(3))
                    {
                        simple_monster_message(defender, " shudders.");
                        specdam += random2avg(1 + (3 * damage_taken), 3);
                    }
                    break;


                case SPWPN_DISTORTION:
                    if (one_chance_in(3))
                    {
                        if (mons_near(defender)
                            && (!mons_has_ench(defender,ENCH_INVIS)
                                || player_see_invis()))
                        {
                            strcpy(info, "Space bends around ");
                            strcat(info, ptr_monam(defender, 1));
                            strcat(info, ".");
                            mpr(info);
                        }

                        specdam += 1 + random2avg(7, 2);
                        break;
                    }
                    if (one_chance_in(3))
                    {
                        if (mons_near(defender)
                            && (!mons_has_ench(defender,ENCH_INVIS)
                                || player_see_invis()))
                        {
                            strcpy(info, "Space warps horribly around ");
                            strcat(info, ptr_monam(defender, 1));
                            strcat(info, "!");
                            mpr(info);
                        }

                        specdam += 3 + random2avg(24, 2);
                        break;
                    }

                    if (one_chance_in(3))
                    {
                        monster_blink(defender);
                        break;
                    }

                    if (coinflip())
                    {
                        monster_teleport(defender, coinflip());
                        break;
                    }

                    if (coinflip())
                    {
                        monster_die(defender, KILL_RESET, monster_attacking);
                        break;
                    }
                    break;
                }
            }
        }                       // end of if special weapon

        damage_taken += specdam;

        if (damage_taken > 0)
        {
            hurt_monster(defender, damage_taken);

            if (defender->hit_points < 1)
            {
                monster_die(defender, KILL_MON, monster_attacking);
                return true;
            }
        }

        // speed adjustment for weapon using monsters
        if (wepSpd > 0)
        {
            // only get one third penalty/bonus for second weapons.
            if (runthru > 0)
                wepSpd = (20 + wepSpd) / 3;
            attacker->speed_increment -= (wepSpd - 10) / 2;
        }
    }                           // end of for runthru

    return true;
}                               // end monsters_fight()

void monster_die(struct monsters *monster, char killer, int i)
{

    int dmi;                    // dead monster's inventory
    int monster_killed = monster_index(monster);

    // From time to time Trog gives you a little bonus
    if (killer == KILL_YOU
        && you.religion == GOD_TROG
        && you.berserker
        && (!player_under_penance() && you.piety > random2(1000)))
    {
        int bonus = 3 + random2(3);

        you.berserker += bonus;
        you.might += bonus;
        you.haste += bonus;
        mpr("You feel the power of Trog in you as your rage grows.",
            MSGCH_GOD, GOD_TROG);
    }

    if (you.prev_targ == monster_killed)
        you.prev_targ = MHITNOT;

    if (monster->type == MONS_GIANT_SPORE
        || monster->type == MONS_BALL_LIGHTNING)
    {
        if (monster->hit_points < 1 && monster->hit_points > -15)
            return;
    }
    else if (monster->type == MONS_FIRE_VORTEX
             || monster->type == MONS_SPATIAL_VORTEX)
    {
        simple_monster_message(monster, " dissipates!", MSGCH_MONSTER_DAMAGE,
                               MDAM_DEAD);

        if (YOU_KILL(killer)
            && !testbits(monster->flags, MF_CREATED_FRIENDLY))
        {
            gain_exp(exper_value( monster->type, monster->hit_dice,
                                  monster->max_hit_points ));
        }

        if (monster->type == MONS_FIRE_VORTEX)
            place_cloud(CLOUD_FIRE_MON, monster->x, monster->y, 2 + random2(4));
    }
    else if (monster->type == MONS_DANCING_WEAPON)
    {
        simple_monster_message(monster, " falls from the air.",
                               MSGCH_MONSTER_DAMAGE, MDAM_DEAD);

        if (YOU_KILL(killer)
            && !testbits(monster->flags, MF_CREATED_FRIENDLY))
        {
            gain_exp(exper_value( monster->type, monster->hit_dice,
                                  monster->max_hit_points ));
        }
    }
    else
    {
        switch (killer)
        {
        case KILL_YOU:          /* You kill in combat. */
        case KILL_YOU_MISSILE:  /* You kill by missile or beam. */
            strcpy(info, "You ");
            strcat(info, (wounded_damaged(monster->type)) ? "destroy" : "kill");
            strcat(info, " ");
            strcat(info, ptr_monam(monster, 1));
            strcat(info, "!");
            mpr(info, MSGCH_MONSTER_DAMAGE, MDAM_DEAD);

            if (!testbits(monster->flags, MF_CREATED_FRIENDLY))
            {
                gain_exp(exper_value( monster->type, monster->hit_dice,
                                      monster->max_hit_points ));
            }
            else
                mpr("That felt strangely unrewarding.");

            // Xom doesn't care who you killed:
            if (you.religion == GOD_XOM
                && random2(70) <= 10 + monster->hit_dice)
            {
                Xom_acts(true, 1 + random2(monster->hit_dice), false);
            }

            // Trying to prevent summoning abuse here, so we're trying to
            // prevent summoned creatures from being being done_good kills,
            // Only affects monsters friendly when created.
            if (!testbits(monster->flags, MF_CREATED_FRIENDLY))
            {
                if (you.duration[DUR_PRAYER])
                {
                    if (mons_holiness(monster->type) == MH_NATURAL)
                        done_good(GOOD_KILLED_LIVING, monster->hit_dice);

                    if (mons_holiness(monster->type) == MH_UNDEAD)
                        done_good(GOOD_KILLED_UNDEAD, monster->hit_dice);

                    if (mons_holiness(monster->type) == MH_DEMONIC)
                        done_good(GOOD_KILLED_DEMON, monster->hit_dice);

                    if (mons_holiness(monster->type) == MH_HOLY)
                        done_good(GOOD_KILLED_ANGEL_II, monster->hit_dice);

                    //jmf: Trog hates wizards
                    if (mons_flag(monster_killed, M_ACTUAL_SPELLS))
                        done_good(GOOD_KILLED_WIZARD, monster->hit_dice);

                    //jmf: maybe someone hates priests?
                    if (mons_flag(monster_killed, M_PRIEST))
                        done_good(GOOD_KILLED_PRIEST, monster->hit_dice);
                }
                else if (mons_holiness(monster->type) == MH_HOLY)
                {
                    done_good(GOOD_KILLED_ANGEL_I, monster->hit_dice);
                }
            }

            if (you.mutation[MUT_DEATH_STRENGTH]
                || (you.religion == GOD_MAKHLEB && you.duration[DUR_PRAYER]
                    && (!player_under_penance() && random2(you.piety) >= 30)))
            {
                if (you.hp < you.hp_max)
                {
                    mpr("You feel a little better.");
                    inc_hp(monster->hit_dice + random2(monster->hit_dice),
                           false);
                }
            }

            if ((you.religion == GOD_MAKHLEB || you.religion == GOD_VEHUMET)
                && you.duration[DUR_PRAYER]
                && (!player_under_penance() && random2(you.piety) >= 30))
            {
                if (you.magic_points < you.max_magic_points)
                {
                    mpr("You feel your power returning.");
                    inc_mp(1 + random2(random2(monster->hit_dice)), false);
                }
            }

            if (you.duration[DUR_DEATH_CHANNEL]
                && mons_holiness(monster->type) == MH_NATURAL
                && mons_weight(mons_charclass(monster->type)))
            {
                if (create_monster( MONS_SPECTRAL_THING, 0, BEH_FRIENDLY,
                                    monster->x, monster->y, you.pet_target,
                                    mons_charclass(monster->type)) != -1)
                {
                    mpr("A strange glowing mist starts to gather...");
                }
            }
            break;

        case KILL_MON:          /* Monster kills in combat */
        case KILL_MON_MISSILE:  /* Monster kills by missile or beam */
            simple_monster_message(monster, " dies!", MSGCH_MONSTER_DAMAGE,
                                   MDAM_DEAD);

            // no piety loss if god gifts killed by other monsters
            if (mons_friendly(monster) && !testbits(monster->flags, MF_GOD_GIFT))
                naughty(NAUGHTY_FRIEND_DIES, 1 + (monster->hit_dice / 2));

            if ((i >= 0 && i < 200) && mons_friendly(&menv[i]))
            {
                // Trying to prevent summoning abuse here, so we're trying to
                // prevent summoned creatures from being being done_good kills.
                // Only affects creatures which were friendly when summoned.
                if (!testbits(monster->flags, MF_CREATED_FRIENDLY))
                {
                    gain_exp(exper_value( monster->type, monster->hit_dice,
                                          monster->max_hit_points ) / 2 + 1);

                    if (mons_holiness(menv[i].type) == MH_UNDEAD)
                    {
                        if (mons_holiness(monster->type) == MH_NATURAL)
                        {
                            done_good( GOOD_SLAVES_KILL_LIVING,
                                       monster->hit_dice );
                        }
                        else
                            done_good(GOOD_SERVANTS_KILL, monster->hit_dice);
                    }
                    else
                    {
                        done_good(GOOD_SERVANTS_KILL, monster->hit_dice);

                        if (you.religion == GOD_VEHUMET
                            && (!player_under_penance()
                                && random2(you.piety) >= 30))
                        {
                            /* Vehumet - only for non-undead servants (coding
                               convenience, no real reason except that Vehumet
                               prefers demons) */
                            if (you.magic_points < you.max_magic_points)
                            {
                                mpr("You feel your power returning.");
                                inc_mp(1 + random2(random2(monster->hit_dice)),
                                       false);
                            }
                        }
                    }
                }
            }
            break;

        /* Monster killed by trap/inanimate thing/itself/poison not from you */
        case KILL_MISC:
            simple_monster_message(monster, " dies!", MSGCH_MONSTER_DAMAGE,
                                   MDAM_DEAD);
            break;

        case KILL_RESET:
        /* Monster doesn't die, just goes back to wherever it came from
           This must only be called by monsters running out of time (or
           abjuration), because it uses the beam variables! Or does it??? */
            if (monster->type != MONS_SIMULACRUM_SMALL
                && monster->type != MONS_SIMULACRUM_LARGE)
            {
                simple_monster_message( monster,
                                        " disappears in a puff of smoke!" );

                place_cloud(CLOUD_GREY_SMOKE_MON + random2(3), monster->x,
                            monster->y, 1 + random2(3));
            }
            else
            {
                simple_monster_message( monster,
                                        " vaporizes!" );

                place_cloud(CLOUD_COLD_MON, monster->x, monster->y,
                            1 + random2(3));
            }

            for (dmi = MSLOT_GOLD; dmi >= MSLOT_WEAPON; dmi--)
            {                   /* takes whatever it's carrying back home */
                if (monster->inv[dmi] != NON_ITEM)
                    destroy_item(monster->inv[dmi]);

                monster->inv[dmi] = NON_ITEM;
            }
            break;
        }
    }

    if (monster->type == MONS_MUMMY)
    {
        if (YOU_KILL(killer))
        {
            if (curse_an_item(1, 0))
                mpr("You feel nervous for a moment...", MSGCH_MONSTER_SPELL);
        }
    }
    else if (monster->type == MONS_GUARDIAN_MUMMY
             || monster->type == MONS_GREATER_MUMMY
             || monster->type == MONS_MUMMY_PRIEST)
    {
        if (YOU_KILL(killer))
        {
            mpr("You feel extremely nervous for a moment...",
                MSGCH_MONSTER_SPELL);

            miscast_effect(SPTYP_NECROMANCY,
                           3 + (monster->type == MONS_GREATER_MUMMY) * 8
                             + (monster->type == MONS_MUMMY_PRIEST) * 5,
                           random2avg(88, 3), 100);
        }
    }

    if (killer != KILL_RESET)
    {
        if (mons_has_ench(monster, ENCH_ABJ_I, ENCH_ABJ_VI))
        {
            if (mons_weight(mons_charclass(monster->type)))
            {
                if (monster->type == MONS_SIMULACRUM_SMALL
                    || monster->type == MONS_SIMULACRUM_LARGE)
                {
                    simple_monster_message( monster, " vaporizes!" );

                    place_cloud(CLOUD_COLD_MON, monster->x, monster->y,
                                1 + random2(3));
                }
                else
                {
                    simple_monster_message(monster,
                                "'s corpse disappears in a puff of smoke!");
                    place_cloud(CLOUD_GREY_SMOKE_MON + random2(3),
                                monster->x, monster->y, 1 + random2(3));
                }
            }
        }
        else
        {
            // have to add case for disintegration effect here? {dlb}
            place_monster_corpse(monster);
        }
    }

    monster_drop_ething(monster);
    monster_cleanup(monster);
}                                                   // end monster_die

void monster_cleanup(struct monsters *monster)
{
    int monster_killed = monster_index(monster);
    int dmi = 0;

    for (unsigned char j = 0; j < NUM_MON_ENCHANTS; j++)
        monster->enchantment[j] = ENCH_NONE;

    monster->flags = 0;
    monster->type = -1;
    monster->hit_points = 0;
    monster->max_hit_points = 0;
    monster->hit_dice = 0;
    monster->armor_class = 0;
    monster->evasion = 0;
    monster->speed_increment = 0;

    mgrd[monster->x][monster->y] = NON_MONSTER;

    for (dmi = MSLOT_GOLD; dmi >= MSLOT_WEAPON; dmi--)
    {
        monster->inv[dmi] = NON_ITEM;
    }

    for (dmi = 0; dmi < MAX_MONSTERS; dmi++)
    {
        if (menv[dmi].foe == monster_killed)
            menv[dmi].foe = MHITNOT;
    }

    if (you.pet_target == monster_killed)
        you.pet_target = MHITNOT;

}                               // end monster_cleanup()

bool jelly_divide(struct monsters * parent)
{
    int jex = 0, jey = 0;       // loop variables {dlb}
    bool foundSpot = false;     // to rid code of hideous goto {dlb}
    struct monsters *child = 0; // NULL - value determined with loop {dlb}

    if (parent->type != MONS_JELLY || parent->hit_points == 1)
        return false;

    // first, find a suitable spot for the child {dlb}:
    for (jex = -1; jex < 3; jex++)
    {
        // loop moves beyond those tiles contiguous to parent {dlb}:
        if (jex > 1)
            return false;

        for (jey = -1; jey < 2; jey++)
        {
            // 10-50 for now - must take clouds into account:
            if (mgrd[parent->x + jex][parent->y + jey] == NON_MONSTER
                && grd[parent->x + jex][parent->y + jey] >
                DNGN_LAST_SOLID_TILE && (parent->x + jex != you.x_pos
                                         || parent->y + jey != you.y_pos))
            {
                foundSpot = true;
                break;
            }
        }

        if (foundSpot)
            break;
    }                           /* end of for jex */

    int k = 0;                  // must remain outside loop that follows {dlb}

    // now that we have a spot, find a monster slot {dlb}:
    for (k = 0; k < MAX_MONSTERS; k++)
    {
        child = &menv[k];

        if (child->type == -1)
            break;
        else if (k == MAX_MONSTERS - 1)
            return false;
    }

    // handle impact of split on parent {dlb}:
    parent->max_hit_points /= 2;

    if (parent->hit_points > parent->max_hit_points)
        parent->hit_points = parent->max_hit_points;

    // create child {dlb}:
    // this is terribly partial and really requires
    // more thought as to generation ... {dlb}
    child->type = parent->type;
    child->hit_dice = 4;
    child->hit_points = parent->hit_points;
    child->max_hit_points = child->hit_points;
    child->armor_class = 0;
    child->evasion = 2;
    child->speed = 5;
    child->speed_increment = 70 + random2(5);
    child->behavior = parent->behavior; /* Look at this! */
    child->foe = parent->foe;
    child->attitude = parent->attitude;

    child->x = parent->x + jex;
    child->y = parent->y + jey;

    mgrd[child->x][child->y] = k;

    if (!simple_monster_message(parent, " splits in two!"))
    {
        if (!silenced(parent->x, parent->y) || !silenced(child->x, child->y))
            mpr("You hear a squelching noise.");
    }

    return true;
}                               // end jelly_divde()

// if you're invis and throw/zap whatever, alerts menv to your position
void alert(void)
{
    struct monsters *monster = 0;       // NULL {dlb}

    for (int it = 0; it < MAX_MONSTERS; it++)
    {
        monster = &menv[it];

        if (monster->type != -1 && mons_near(monster))
            behavior_event(monster, ME_ALERT, MHITYOU);
    }
}                               // end alert()

static bool valid_morph( struct monsters *monster, int new_mclass )
{
    unsigned char current_tile = grd[monster->x][monster->y];

    /* various inappropriate polymorph targets */
    if ( mons_rarity(new_mclass) == 0
          || mons_flag(new_mclass, M_NO_EXP_GAIN)
          || new_mclass == monster->type         // no "in-place" poly {dlb}
          || new_mclass == MONS_SHAPESHIFTER
          || new_mclass == MONS_GLOWING_SHAPESHIFTER
          || new_mclass == MONS_ZOMBIE_SMALL
          || new_mclass == MONS_ZOMBIE_LARGE
          || new_mclass == MONS_SPECTRAL_THING
          || new_mclass == MONS_SKELETON_SMALL
          || new_mclass == MONS_SKELETON_LARGE
          || new_mclass == MONS_SIMULACRUM_SMALL
          || new_mclass == MONS_SIMULACRUM_LARGE
          || new_mclass == MONS_DANCING_WEAPON )
    {
        return false;
    }

    /* Not fair to instakill a monster like this --
     order of evaluation of inner conditional important */
    if ( current_tile == DNGN_LAVA || current_tile == DNGN_DEEP_WATER )
    {
        if ( !mons_flies(new_mclass)
            || monster_habitat(new_mclass) != current_tile )
        {
            return false;
        }
    }

    return true;
}        // end valid_morph()

// note that power is (as of yet) unused within this function -
// may be worthy of consideration of later implementation, though,
// so I'll still let the parameter exist for the time being {dlb}
bool monster_polymorph( struct monsters *monster, int targetc, int power )
{
    char str_polymon[200] = "";      // cannot use info[] here {dlb}
    bool player_messaged = false;

    if ( targetc == RANDOM_MONSTER )
    {
        do
        {
            targetc = random2(NUM_MONSTERS);     // was: random2(400) {dlb}
        }
        while ( !valid_morph(monster, targetc) );
    }

    // messaging: {dlb}

    if (mons_has_ench(monster, ENCH_GLOWING_SHAPESHIFTER, ENCH_SHAPESHIFTER))
    {
        strcat(str_polymon, " changes into ");
    }
    else if (targetc == MONS_PULSATING_LUMP)
        strcat(str_polymon, " degenerates into ");
    else
        strcat(str_polymon, " evaporates and reforms as " );

    bool invis = mons_flag(targetc, M_INVIS) ||
        mons_has_ench(monster, ENCH_INVIS);
    strcat(str_polymon, monam(250, targetc, (invis)?ENCH_INVIS:ENCH_NONE, 3));

    if ( targetc == MONS_PULSATING_LUMP )
        strcat(str_polymon, " of flesh");

    strcat(str_polymon, "!");

    player_messaged = simple_monster_message(monster, str_polymon);

    // the actual polymorphing:
    int old_hp = monster->hit_points;
    int old_hp_max = monster->max_hit_points;

    /* deal with mons_sec */
    monster->type = targetc;
    monster->number = 250;

    int ench = mons_has_ench(monster, ENCH_ABJ_I, ENCH_ABJ_VI);
    int ench2 = mons_has_ench(monster, ENCH_GLOWING_SHAPESHIFTER, ENCH_SHAPESHIFTER);

    for (unsigned char unenc = 0; unenc < 5; unenc++)
        monster->enchantment[unenc] = ENCH_NONE;

    mons_add_ench(monster, ench);
    mons_add_ench(monster, ench2);

    if ( mons_flag(monster->type, M_INVIS) )
        mons_add_ench(monster, ENCH_INVIS);

    define_monster(monster_index(monster));

    monster->hit_points = monster->max_hit_points
                                * ((old_hp * 100) / old_hp_max) / 100
                                + random2(monster->max_hit_points);

    if ( monster->hit_points > monster->max_hit_points )
        monster->hit_points = monster->max_hit_points;

    monster->speed_increment = 70 + random2(5);

    /* make sure that eg manticores don't drop their darts */
    monster_drop_ething(monster);

    return ( player_messaged );
}          // end monster_polymorph()


/*
 **************************************************
 *                                                *
 *              END PUBLIC FUNCTIONS              *
 *                                                *
 **************************************************
*/


// Added by DML 6/10/99.
// For now, always returns damage: that is, it never modifies values,
// just adds 'color'.
static int weapon_type_modify(int weapnum, char *noise, char *noise2,
                              int damage)
{
    int weap_type = WPN_UNKNOWN;

    if (damage >= HIT_WEAK)
    {
        if (weapnum == -1)
            weap_type = WPN_UNARMED;
        else
        {
            if (you.inv_class[weapnum] == OBJ_STAVES)
                weap_type = WPN_QUARTERSTAFF;
            else if (you.inv_class[weapnum] == OBJ_WEAPONS)
                weap_type = you.inv_type[weapnum];
        }
    }

    strcpy(noise2, "");

    // take transformations into account, if no weapon is weilded
    if (weap_type == WPN_UNKNOWN && you.attribute[ATTR_TRANSFORMATION] != TRAN_NONE)
    {
        switch (you.attribute[ATTR_TRANSFORMATION])
        {
            case TRAN_SPIDER:
                if (damage < HIT_STRONG)
                    strcpy(noise, "bite");
                else
                    strcpy(noise, "maul");
                break;
            case TRAN_BLADE_HANDS:
                if (damage < HIT_MED)
                    strcpy(noise, "slash");
                else if (damage < HIT_STRONG)
                    strcpy(noise, "slice");
                break;
            case TRAN_ICE_BEAST:
            case TRAN_STATUE:
            case TRAN_LICH:
                if (damage < HIT_MED)
                    strcpy(noise, "punch");
                else
                    strcpy(noise, "pummel");
                break;
            case TRAN_DRAGON:
            case TRAN_SERPENT_OF_HELL:
                if (damage < HIT_MED)
                    strcpy(noise, "claw");
                else if (damage < HIT_STRONG)
                    strcpy(noise, "bite");
                else
                    strcpy(noise, "maul");
                break;
            case TRAN_AIR:
                strcpy(noise, "buffet");
                break;
        } // transformations

        return damage;
    }

    switch (weap_type)
    {
    case WPN_KNIFE:
    case WPN_DAGGER:
    case WPN_SHORT_SWORD:
    case WPN_TRIDENT:
    case WPN_DEMON_TRIDENT:
    case WPN_SPEAR:
        if (damage < HIT_MED)
            strcpy(noise, "puncture");
        else if (damage < HIT_STRONG)
            strcpy(noise, "impale");
        else
        {
            strcpy(noise, "spit");
            strcpy(noise2, " like a pig");
        }
        return damage;

    case WPN_BOW:
    case WPN_CROSSBOW:
    case WPN_HAND_CROSSBOW:
        if (damage < HIT_STRONG)
            strcpy(noise, "puncture");
        else
            strcpy(noise, "skewer");
        return damage;

    case WPN_LONG_SWORD:
    case WPN_GREAT_SWORD:
    case WPN_SCIMITAR:
    case WPN_HALBERD:
    case WPN_GLAIVE:
    case WPN_HAND_AXE:
    case WPN_AXE:
    case WPN_BROAD_AXE:
    case WPN_BATTLEAXE:
    case WPN_SCYTHE:
    case WPN_QUICK_BLADE:
    case WPN_KATANA:
    case WPN_EXECUTIONERS_AXE:
    case WPN_DOUBLE_SWORD:
    case WPN_TRIPLE_SWORD:
    case WPN_SABRE:
    case WPN_DEMON_BLADE:
        if (damage < HIT_MED)
            strcpy(noise, "slash");
        else if (damage < HIT_STRONG)
            strcpy(noise, "slice");
        else
        {
            strcpy(noise, "open");
            strcpy(noise2, " like a pillowcase");
        }
        return damage;

    case WPN_SLING:
    case WPN_CLUB:
    case WPN_MACE:
    case WPN_FLAIL:
    case WPN_GREAT_MACE:
    case WPN_GREAT_FLAIL:
    case WPN_QUARTERSTAFF:
    case WPN_GIANT_CLUB:
    case WPN_HAMMER:
    case WPN_ANCUS:
    case WPN_MORNINGSTAR:       /*for now, just a bludgeoning weapon */
    case WPN_SPIKED_FLAIL:      /*for now, just a bludgeoning weapon */
    case WPN_EVENINGSTAR:
    case WPN_GIANT_SPIKED_CLUB:
        if (damage < HIT_MED)
            strcpy(noise, "sock");
        else if (damage < HIT_STRONG)
            strcpy(noise, "bludgeon");
        else
        {
            strcpy(noise, "crush");
            strcpy(noise2, " like a grape");
        }
        return damage;

    case WPN_WHIP:
    case WPN_DEMON_WHIP:
        if (damage < HIT_MED)
            strcpy(noise, "whack");
        else
            strcpy(noise, "thrash");
        return damage;

    case WPN_UNARMED:
        if (you.species == SP_TROLL || you.mutation[MUT_CLAWS])
        {
            if (damage < HIT_MED)
                strcpy(noise, "claw");
            else if (damage < HIT_STRONG)
                strcpy(noise, "mangle");
            else
                strcpy(noise, "eviscerate");
        }
        else
        {
            if (damage < HIT_MED)
                strcpy(noise, "punch");
            else
                strcpy(noise, "pummel");
        }
        return damage;

    case WPN_UNKNOWN:
    default:
        strcpy(noise, "hit");
        return damage;
    }
}                               // end weapon_type_modify()

static void monster_drop_ething(struct monsters *monster)
{
    /* drop weapons & missiles last (ie on top) so others pick up */
    int loopy;                  // loop variable {dlb}
    bool splashes = false;

    if (grd[monster->x][monster->y] == DNGN_LAVA
        || grd[monster->x][monster->y] == DNGN_DEEP_WATER)
    {
        for (loopy = 0; loopy < NUM_MONSTER_SLOTS; loopy++)
        {
            if (monster->inv[loopy] != NON_ITEM)
            {
                destroy_item(monster->inv[loopy]);
                splashes = true;
            }
        }

        // I don't think things dropped into lava "splash" {dlb}
        if (splashes)
            mpr("You hear a splashing sound.");

        return;
    }

    for (loopy = MSLOT_GOLD; loopy >= MSLOT_WEAPON; loopy--)
    {
        if (monster->inv[loopy] != NON_ITEM)
        {
            mitm.link[monster->inv[loopy]] = NON_ITEM;

            if (igrd[monster->x][monster->y] == NON_ITEM)
            {
                igrd[monster->x][monster->y] = monster->inv[loopy];
            }
            else
            {
                int huj = igrd[monster->x][monster->y];

                igrd[monster->x][monster->y] = monster->inv[loopy];
                mitm.link[monster->inv[loopy]] = huj;
            }
        }

        monster->inv[loopy] = NON_ITEM;
    }
}                               // end monster_drop_ething()

static void place_monster_corpse(struct monsters *monster)
{
    int corpse_class = mons_charclass(monster->type);

    if (mons_has_ench(monster, ENCH_SHAPESHIFTER))
        corpse_class = MONS_SHAPESHIFTER;
    else if (mons_has_ench(monster, ENCH_GLOWING_SHAPESHIFTER))
        corpse_class = MONS_GLOWING_SHAPESHIFTER;

    if (!mons_weight(corpse_class)
        || grd[monster->x][monster->y] == DNGN_LAVA
        || grd[monster->x][monster->y] == DNGN_DEEP_WATER || coinflip())
    {
        return;
    }

    int o;

    for (o = 0; o < MAX_ITEMS; o++)
    {
        // seems to me that this should be translated to a probability
        // calculation outside the loop (why keep calling random2() with
        // each iteration?) {dlb}
        if (o >= 200 + random2(150))
            return;

        if (mitm.quantity[o] == 0)
        {
            mitm.id[o] = 0;
            mitm.base_type[o] = OBJ_CORPSES;
            mitm.pluses[o] = corpse_class;
            mitm.pluses2[o] = 0;
            mitm.sub_type[o] = CORPSE_BODY;
            mitm.special[o] = 210;
            mitm.colour[o] = mons_colour(corpse_class);
            if (mitm.colour[o] == BLACK)
                mitm.colour[o] = monster->number;
            mitm.quantity[o] = 1;
            mitm.link[o] = NON_ITEM;
            break;
        }
    }

    if (igrd[monster->x][monster->y] == NON_ITEM)
        igrd[monster->x][monster->y] = o;
    else
    {
        int hug = igrd[monster->x][monster->y];

        igrd[monster->x][monster->y] = o;
        mitm.link[o] = hug;
    }

    return;
}                               // end place_monster_corpse()

// Returns a value between 0 and 10 representing the weight given to str
int weapon_str_weight( int wpn_class, int wpn_type )
{
    int  ret;

    const int  wpn_skill  = weapon_skill( wpn_class, wpn_type );
    const int  hands_reqd = hands_reqd_for_weapon( wpn_class, wpn_type );

    // These are low values, because we'll be adding some bonus to the
    // larger weapons later.  Remember also that 1-1/2-hand weapons get
    // a bonus in player_weapon_str_weight() as well (can't be done
    // here because this function is used for cases where the weapon
    // isn't being used by the player).

    // Reasonings:
    // - Short Blades are the best for the dexterous... although they
    //   are very limited in damage potential
    // - Long Swords are better for the dexterous, the two-handed
    //   swords are a 50/50 split; bastard swords are in between.
    // - Staves: didn't want to punish the mages who want to use
    //   these... made it a 50/50 split after the 2-hnd bonus
    // - Polearms: Spears and tridents are the only ones that can
    //   be used one handed and are poking weapons, which requires
    //   more agility than strength.  The other ones also require a
    //   fair amount of agility so they end up at 50/50 (most can poke
    //   as well as slash... although slashing is not their strong
    //   point).
    // - Axes are weighted and edged and so require mostly strength,
    //   but not as much as Maces and Flails, which are typically
    //   blunt and spiked weapons.
    switch (wpn_skill)
    {
    case SK_SHORT_BLADES:     ret = 2; break;
    case SK_LONG_SWORDS:      ret = 3; break;
    case SK_STAVES:           ret = 3; break;   // == 5 after 2-hand bonus
    case SK_POLEARMS:         ret = 3; break;   // most are +2 for 2-hands
    case SK_AXES:             ret = 6; break;
    case SK_MACES_FLAILS:     ret = 7; break;
    default:                  ret = 5; break;
    }

    // whips are special cased (because they are not much like maces)
    if (wpn_type == WPN_WHIP || wpn_type == WPN_DEMON_WHIP)
        ret = 2;
    else if (wpn_type == WPN_QUICK_BLADE) // high dex is very good for these
        ret = 1;

    if (hands_reqd == HANDS_TWO_HANDED)
        ret += 2;

    // most weapons are capped at 8
    if (ret > 8)
    {
        // these weapons are huge, so strength plays a larger role
        if (wpn_type == WPN_GIANT_CLUB || wpn_type == WPN_GIANT_SPIKED_CLUB)
            ret = 9;
        else
            ret = 8;
    }

    return (ret);
}

// Returns a value from 0 to 10 representing the weight of strength to
// dexterity for the players currently wielded weapon.
static inline int player_weapon_str_weight( void )
{
    const int weapon = you.equip[ EQ_WEAPON ];

    // unarmed, weighted slightly towards dex -- would have been more,
    // but then we'd be punishing Trolls and Ghouls who are strong and
    // get special unarmed bonuses.
    if (weapon == -1)
        return (4);

    int ret = weapon_str_weight( you.inv_class[weapon], you.inv_type[weapon] );

    const bool shield     = (you.equip[EQ_SHIELD] != -1);
    const int  hands_reqd = hands_reqd_for_weapon( you.inv_class[weapon],
                                                   you.inv_type[weapon] );

    if (hands_reqd == HANDS_ONE_OR_TWO_HANDED && !shield)
        ret += 1;

    return (ret);
}

// weapon_dex_weight() + weapon_str_weight == 10 so we only need define
// one of these.
static inline int player_weapon_dex_weight( void )
{
    return (10 - player_weapon_str_weight());
}

static inline int calc_stat_to_hit_base( void )
{
#ifdef USE_NEW_COMBAT_STATS

    // towards_str_avg is a variable, whose sign points towards strength,
    // and the magnitude is half the difference (thus, when added directly
    // to you.dex it gives the average of the two.
    const signed int towards_str_avg = (you.strength - you.dex) / 2;

    // dex is modified by strength towards the average, by the
    // weighted amount weapon_str_weight() / 10.
    return (you.dex + towards_str_avg * player_weapon_str_weight() / 10);

#else
    return (you.dex);
#endif
}


static inline int calc_stat_to_dam_base( void )
{
#ifdef USE_NEW_COMBAT_STATS

    const signed int towards_dex_avg = (you.dex - you.strength) / 2;
    return (you.strength + towards_dex_avg * player_weapon_dex_weight() / 10);

#else
    return (you.strength);
#endif
}

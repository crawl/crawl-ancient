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


extern bool wield_change;    // defined in output.cc


static int weapon_type_modify(int weap, char *st_prn, char *noise2, int damage);
static void monster_drop_ething(struct monsters *monster);
static void place_monster_corpse(struct monsters *monster);




/*
 **************************************************
 *                                                *
 *             BEGIN PUBLIC FUNCTIONS             *
 *                                                *
 **************************************************
*/


void you_attack( int monster_attacked, bool unarmed_attacks )
{

    struct monsters *defender = &menv[monster_attacked];

    int your_to_hit;
    int damage_done = 0;
    bool hit = false;
    unsigned char stab_bonus = 0;                           // this is never negative {dlb}
    int temp_rand;                                          // for probability determination {dlb}
    bool ur_armed = (you.equip[EQ_WEAPON] != -1);           // compacts code a bit {dlb}
    bool bearing_shield = (you.equip[EQ_SHIELD] != -1);     // compacts code a bit {dlb}

    char str_pass[80], damage_noise[20], damage_noise2[20];
    int special_brand = SPWPN_NORMAL;

    int heavy_armour = 0;

// heavy armour modifiers for shield borne
    if ( bearing_shield )
      {
        switch ( you.inv_type[you.equip[EQ_SHIELD]] )
        {
          case ARM_SHIELD:
            if ( you.skills[SK_SHIELDS] < random2(7) )
              heavy_armour++;
            break;
          case ARM_LARGE_SHIELD:
            if ( ( you.species >= SP_OGRE && you.species <= SP_OGRE_MAGE ) || player_genus(GENPC_DRACONIAN) )
              {
                if ( you.skills[SK_SHIELDS] < random2(13) )
                  heavy_armour++;    // was potentially "+= 3" {dlb}
              }
            else
              for (unsigned char i = 0; i < 3; i++)
                if ( you.skills[SK_SHIELDS] < random2(13) )
                  heavy_armour += random2(3);
            break;
          default:
            break;
        }
      }

// heavy armour modifiers for PARM_EVASION
    if (you.equip[EQ_BODY_ARMOUR] != -1)
      {
        if ( property(OBJ_ARMOUR, you.inv_type[you.equip[EQ_BODY_ARMOUR]], PARM_EVASION) < 0
            && random2(you.skills[SK_ARMOUR]) < abs(property(OBJ_ARMOUR, you.inv_type[you.equip[EQ_BODY_ARMOUR]], PARM_EVASION)) )
          heavy_armour += random2(abs(property(OBJ_ARMOUR, you.inv_type[you.equip[EQ_BODY_ARMOUR]], PARM_EVASION)));
      }

// ??? what is the reasoning behind this ??? {dlb}
    if ( !ur_armed )
      heavy_armour *= ( coinflip() ? 3 : 2 );

// Calculate the following two flags in advance
// to know what player does this combat turn:
    bool can_do_unarmed_combat = false;

    if ( you.burden_state == BS_UNENCUMBERED
        && random2(20) < you.skills[SK_UNARMED_COMBAT]
        && random2(1 + heavy_armour) < 2 )
      can_do_unarmed_combat = true;

// if we're not getting potential unarmed attacks, and not wearing a
// shield, and have a suitable uncursed weapon we get the bonus.
    bool use_hand_and_a_half_bonus = false;

    if ( unarmed_attacks
        && !can_do_unarmed_combat
        && !bearing_shield
        && ur_armed
        && you.inv_plus[you.equip[EQ_WEAPON]] < 130     // that is, uncursed
        && hands_reqd_for_weapon(you.inv_class[you.equip[EQ_WEAPON]], you.inv_type[you.equip[EQ_WEAPON]]) == HANDS_ONE_OR_TWO_HANDED )
      use_hand_and_a_half_bonus = true;    // currently: +1 dam, +1 hit, -1 spd

/*
 **************************************************************************
 *                                                                        *
 *  IMPORTANT: When altering damage routines, must also change in ouch.cc *
 *  for saving of player ghosts.                                          *
 *                                                                        *
 **************************************************************************
 */

    bool helpless = mons_flag(defender->type, M_NO_EXP_GAIN);

    if (defender->behavior != BEH_ENSLAVED)
      you.pet_target = monster_attacked;
    if (defender->behavior == BEH_ENSLAVED)
      {

        defender->behavior = BEH_CHASING_I;
        naughty(NAUGHTY_ATTACK_FRIEND, 5);

/* ************************************************************************
// jmf seems to have added this,
// but done_good() has no handle on it {dlb}

        if ( ( defender->enchantment[1] < ENCH_FRIEND_ABJ_I || defender->enchantment[1] > ENCH_FRIEND_ABJ_VI )
            && defender->enchantment[1] != ENCH_CREATED_FRIENDLY )
          done_good(GOOD_ATTACKED_FRIEND, 5);
************************************************************************ */

      }

// fumbling in shallow water <early return>:
    if ( grd[you.x_pos][you.y_pos] == DNGN_SHALLOW_WATER && !you.levitation )
    {
        if ( random2(you.dex) < 4 || one_chance_in(5) )
        {
            mpr("Unstable footing causes you to fumble your attack.");
            return;
        }
    }

// preliminary to_hit modifications:
    your_to_hit = 15 + (you.dex / 2);    // + (you.strength / 2);

    if ( wearing_amulet(AMU_INACCURACY) )
      your_to_hit -= 5;

    your_to_hit += random2(1 + you.skills[SK_FIGHTING]);

    if ( ur_armed )
      {
        if ( weapon_skill(you.inv_class[you.equip[EQ_WEAPON]],you.inv_type[you.equip[EQ_WEAPON]]) )
          your_to_hit += random2(you.skills[weapon_skill(you.inv_class[you.equip[EQ_WEAPON]], you.inv_type[you.equip[EQ_WEAPON]])] + 1);
      }
    else    // ...you must be unarmed
      {
        your_to_hit += (you.species == SP_TROLL || you.species == SP_GHOUL) ? 4 : 2;
        your_to_hit += random2(1 + you.skills[SK_UNARMED_COMBAT]);
      }

// energy expenditure in terms of food:
    if ( you.is_undead != US_UNDEAD )
      make_hungry(3, true);

    if ( ur_armed
        && you.inv_class[you.equip[EQ_WEAPON]] == OBJ_WEAPONS
        && (you.inv_dam[you.equip[EQ_WEAPON]] % 30 == SPWPN_SPEED
            || (you.inv_dam[you.equip[EQ_WEAPON]] % 30 >= SPWPN_RANDART_I
              && randart_wpn_properties(you.inv_class[you.equip[EQ_WEAPON]], you.inv_type[you.equip[EQ_WEAPON]], you.inv_dam[you.equip[EQ_WEAPON]], you.inv_plus[you.equip[EQ_WEAPON]], you.inv_plus2[you.equip[EQ_WEAPON]], 0, RAP_BRAND) == SPWPN_SPEED)))
    {
        you.time_taken *= 5;
        you.time_taken /= 10;
    }

    if ( ur_armed
        && you.inv_class[you.equip[EQ_WEAPON]] == OBJ_WEAPONS
        && you.inv_dam[you.equip[EQ_WEAPON]] % 30 >= SPWPN_RANDART_I)
    {
        if (randart_wpn_properties(you.inv_class[you.equip[EQ_WEAPON]], you.inv_type[you.equip[EQ_WEAPON]], you.inv_dam[you.equip[EQ_WEAPON]], you.inv_plus[you.equip[EQ_WEAPON]], you.inv_plus2[you.equip[EQ_WEAPON]], 0, RAP_ANGRY) >= 1)
        {
            if (random2(1 + randart_wpn_properties(you.inv_class[you.equip[EQ_WEAPON]], you.inv_type[you.equip[EQ_WEAPON]], you.inv_dam[you.equip[EQ_WEAPON]], you.inv_plus[you.equip[EQ_WEAPON]], you.inv_plus2[you.equip[EQ_WEAPON]], 0, RAP_ANGRY)))
              go_berserk();
        }
    }

    switch ( you.special_wield )
    {
      case SPWLD_TROG:
        if ( coinflip() )
          go_berserk();
        break;
      case SPWLD_WUCAD_MU:
        if ( one_chance_in(9) )
          miscast_effect(SPTYP_DIVINATION, random2(9), random2(70), 100);
        break;
      default:
        break;
    }


    if ( you.mutation[MUT_BERSERK] )
    {
        if ( random2(100) < (you.mutation[MUT_BERSERK] * 10) - 5 )
          go_berserk();
    }

    if ( ur_armed )
    {
        if (you.inv_class[you.equip[EQ_WEAPON]] == OBJ_WEAPONS)
        {
            int ghoggl = you.inv_plus[you.equip[EQ_WEAPON]] - 50;

            if (you.inv_plus[you.equip[EQ_WEAPON]] > 130)
              ghoggl -= 100;

            your_to_hit += ghoggl;    // there was some stupid conditional here that applied this "if (ghoggl >= 0)" and "else" that .. which is all cases (d'oh!) {dlb}

            your_to_hit += property(OBJ_WEAPONS, you.inv_type[you.equip[EQ_WEAPON]], PWPN_HIT);

            if ( you.inv_dam[you.equip[EQ_WEAPON]] / 30 == DWPN_ELVEN
                && player_genus(GENPC_ELVEN) )
              your_to_hit += ( coinflip() ? 2 : 1 );
        }
        if ( you.inv_class[you.equip[EQ_WEAPON]] == OBJ_STAVES )
          your_to_hit += property(OBJ_WEAPONS, WPN_QUARTERSTAFF, PWPN_HIT);   /* magical staff */
    }

    your_to_hit += slaying_bonus(PWPN_HIT);    // see: player.cc

    if ( you.hunger_state == HS_STARVING )
      your_to_hit -= 3;

    your_to_hit -= heavy_armour;

    your_to_hit = random2(your_to_hit);     // why does this come here and not later? {dlb}

    if ( use_hand_and_a_half_bonus )
      your_to_hit += random2(3);

    if ( ur_armed
        && weapon_skill(you.inv_class[you.equip[EQ_WEAPON]], you.inv_type[you.equip[EQ_WEAPON]]) == SK_SHORT_BLADES
        && you.sure_blade )
      your_to_hit += 5 + random2limit(you.sure_blade, 25);

    int damage = 1;

    if ( !ur_armed )    // empty-handed
      {
        damage = 3;

        if ( you.confusing_touch )
          {
            special_brand = SPWPN_CONFUSE;     // just trying to touch is easier that trying to damage
            your_to_hit += random2(you.dex);
            damage = 1;
          }

        if ( you.mutation[MUT_DRAIN_LIFE] )
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
            if ( you.species == SP_TROLL )
              damage += 5;
            else if ( you.species == SP_GHOUL )
              damage += 2;
        }

        damage += you.skills[SK_UNARMED_COMBAT];
      }
    else
      {
        if ( you.inv_class[you.equip[EQ_WEAPON]] == OBJ_WEAPONS )
          damage = property(you.inv_class[you.equip[EQ_WEAPON]], you.inv_type[you.equip[EQ_WEAPON]], PWPN_DAMAGE);
        else if ( you.inv_class[you.equip[EQ_WEAPON]] == OBJ_STAVES )
          damage = property(OBJ_WEAPONS, WPN_QUARTERSTAFF, PWPN_DAMAGE);
      }

    int weapon_speed2 = 10;

    if ( ur_armed )
      {
        if (you.inv_class[you.equip[EQ_WEAPON]] == OBJ_WEAPONS || you.inv_class[you.equip[EQ_WEAPON]] == OBJ_STAVES)
          {
            if (you.inv_class[you.equip[EQ_WEAPON]] == OBJ_STAVES)
              weapon_speed2 = property(OBJ_WEAPONS, WPN_QUARTERSTAFF, PWPN_SPEED);
            else
              weapon_speed2 = property(you.inv_class[you.equip[EQ_WEAPON]], you.inv_type[you.equip[EQ_WEAPON]], PWPN_SPEED);

            weapon_speed2 -= you.skills[weapon_skill(you.inv_class[you.equip[EQ_WEAPON]], you.inv_type[you.equip[EQ_WEAPON]])] / 2;

            int min_speed = 10;

            if ( you.inv_class[you.equip[EQ_WEAPON]] == OBJ_STAVES )
              min_speed = property(OBJ_WEAPONS, WPN_QUARTERSTAFF, PWPN_SPEED) / 2;
            else
              min_speed = property(you.inv_class[you.equip[EQ_WEAPON]], you.inv_type[you.equip[EQ_WEAPON]], PWPN_SPEED) / 2;

// Using both hands can get a weapon up to speed 7
            if ( ( hands_reqd_for_weapon(you.inv_class[you.equip[EQ_WEAPON]], you.inv_type[you.equip[EQ_WEAPON]] ) == HANDS_TWO_HANDED
                   || use_hand_and_a_half_bonus )
                && min_speed > 7 )
              {
                min_speed = 7;
              }

            if (weapon_speed2 < min_speed)
              weapon_speed2 = min_speed;

            if ( bearing_shield )
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
        if ( you.burden_state == BS_UNENCUMBERED && one_chance_in(heavy_armour + 1) )
          {
            weapon_speed2 = 10 - you.skills[SK_UNARMED_COMBAT] / 3;

            if (weapon_speed2 < 4)
              weapon_speed2 = 4;
          }
      }

// Hand and a half bonus only helps speed up to a point, any more
// than speed 10 must come from skill and the weapon
    if ( use_hand_and_a_half_bonus && weapon_speed2 > 10 )
      weapon_speed2--;

    ASSERT(weapon_speed2 > 0);
    you.time_taken *= weapon_speed2;
    you.time_taken /= 10;

    if ( you.time_taken < 1 )
      you.time_taken = 1;

#ifdef WIZARD
    sprintf(info, "Time taken: %d", you.time_taken);
    mpr(info);
#endif

    if ( defender->speed_increment <= 40
        || ((defender->behavior == BEH_FLEE
    //              || defender->behavior == BEH_WANDER
             || defender->behavior == BEH_CONFUSED)
            && random2(200) <= you.skills[SK_STABBING] + you.dex)

        || defender->behavior == BEH_SLEEP)
    {
        switch ( defender->behavior )
        {
          case BEH_SLEEP:
            // monster is a very easy target
            // note: lower stab_bonuses are better, because they're used
            //       as part of the divisors.
            stab_bonus = 1;
            break;

          case BEH_FLEE:
          case BEH_CONFUSED:
            // monster has limited control of his actions
            stab_bonus = 2;
            break;

          default:
            // monster is in control of his actions
            stab_bonus = 3;
        }

        simple_monster_message(defender, " fails to defend itself.");

        exercise(SK_STABBING, 1 + random2avg(5,4));

        if ( mons_holiness(defender->type) == MH_NATURAL || mons_holiness(defender->type) == MH_HOLY )
          naughty(NAUGHTY_STABBING, 4);
    }
    else
      alert();


    //jmf: check for backlight enchantment
    if ( defender->enchantment1 )
      for (unsigned char index = 0; index < 3; index++)
        if ( defender->enchantment[index] == ENCH_BACKLIGHT_I
              || defender->enchantment[index] == ENCH_BACKLIGHT_II
              || defender->enchantment[index] == ENCH_BACKLIGHT_III
              || defender->enchantment[index] == ENCH_BACKLIGHT_IV )
          your_to_hit += 15; // FIXME: is this fair? lame?

    if ((your_to_hit >= defender->evasion || one_chance_in(15))
        || ((defender->speed_increment <= 60
             || defender->behavior == BEH_SLEEP)
            && !one_chance_in(10 + you.skills[SK_STABBING])))
    {
        hit = true;
        int dammod = 78;

        if ( you.strength > 11 )
          dammod += (random2(you.strength - 11) << 1);
        else if ( you.strength < 9 )
          dammod -= (random2(9 - you.strength) * 3);

        damage *= dammod;       //random2(you.strength);

        damage /= 78;

        damage_done = random2(damage);

        if ( ur_armed && (you.inv_class[you.equip[EQ_WEAPON]] == OBJ_WEAPONS || you.inv_class[you.equip[EQ_WEAPON]] == OBJ_STAVES))
        {
            damage_done *= 25 + (random2(you.skills[weapon_skill(you.inv_class[you.equip[EQ_WEAPON]], you.inv_type[you.equip[EQ_WEAPON]])] + 1));
            damage_done /= 25;
        }

        damage_done *= 30 + (random2(you.skills[SK_FIGHTING] + 1));
        damage_done /= 30;

        if ( you.might > 1 )
          damage_done += 1 + random2(10);

        if ( you.hunger_state == HS_STARVING )
          damage_done -= random2(5);

        if ( ur_armed && you.inv_class[you.equip[EQ_WEAPON]] == OBJ_WEAPONS )
        {
            int hoggl = you.inv_plus2[you.equip[EQ_WEAPON]] - 50;

            damage_done += (hoggl > -1) ? (random2(1 + hoggl)) : (hoggl);

            if ( use_hand_and_a_half_bonus
                  || hands_reqd_for_weapon(you.inv_class[you.equip[EQ_WEAPON]], you.inv_type[you.equip[EQ_WEAPON]]) == HANDS_TWO_HANDED )
              damage_done += random2(3);

            if ( you.inv_dam[you.equip[EQ_WEAPON]] / 30 == DWPN_DWARVEN
                && player_genus(GENPC_DWARVEN) )
              damage_done += random2(3);

            if ( you.inv_dam[you.equip[EQ_WEAPON]] / 30 == DWPN_ORCISH
                && you.species == SP_HILL_ORC && coinflip() )
              damage_done++;

            if (you.inv_ident[you.equip[EQ_WEAPON]] < 3 && random2(100) < you.skills[weapon_skill(you.inv_class[you.equip[EQ_WEAPON]], you.inv_type[you.equip[EQ_WEAPON]])])
            {
                you.inv_ident[you.equip[EQ_WEAPON]] = 3;
                strcpy(info, "You are wielding ");
                in_name(you.equip[EQ_WEAPON], 3, str_pass);
                strcat(info, str_pass);
                strcat(info, ".");
                mpr(info);
                more();
                wield_change = true;
            }
        }
//  else (damage_done -= random2(5));

        damage_done += slaying_bonus(PWPN_DAMAGE);

        if ( stab_bonus )
        {
            int skill_type = weapon_skill(you.inv_class[you.equip[EQ_WEAPON]], you.inv_type[you.equip[EQ_WEAPON]]);

            if (defender->behavior == BEH_SLEEP)
            {
                // Sleeping moster wakes up when stabbed but may be groggy
                defender->behavior = BEH_CHASING_I;

                if (random2(200) <= you.skills[SK_STABBING] + you.dex)
                {
                    int stun = random2(you.dex + 1);

                    if ( defender->speed_increment > stun )
                      defender->speed_increment -= stun;
                    else
                      defender->speed_increment = 0;
                }
            }

            switch (skill_type)
            {
              case SK_SHORT_BLADES:
                if ( you.inv_type[you.equip[EQ_WEAPON]] == WPN_DAGGER )
                {
                    int bonus = you.dex / 3;

                    if ( bonus > you.skills[SK_STABBING] + 1 )
                      bonus = you.skills[SK_STABBING] + 1;

                    damage_done += bonus;
                }
              // fall through
              case SK_LONG_SWORDS:
                damage_done *= 10 + you.skills[SK_STABBING] / (stab_bonus + (skill_type == SK_SHORT_BLADES ? 0 : 1));
                damage_done /= 10;
              // fall through
              default:
                damage_done *= 12 + you.skills[SK_STABBING] / stab_bonus;
                damage_done /= 12;
            }
        }

        if ( defender->armor_class > 0 )
          damage_done -= random2(1 + defender->armor_class);

        damage_done = weapon_type_modify(you.equip[EQ_WEAPON], damage_noise, damage_noise2, damage_done);

        if ( damage_done < 0 )
          damage_done = 0;

        if ( hurt_monster(defender, damage_done) )
        {
            int skill = 0;

            if ( ur_armed
                && (skill = weapon_skill(you.inv_class[you.equip[EQ_WEAPON]], you.inv_type[you.equip[EQ_WEAPON]])) )
            {
                if ( !helpless || you.skills[skill] < 2 )
                  exercise(skill, 1);
            }
            else
            {
                if ( !helpless || you.skills[SK_UNARMED_COMBAT] < 2 )
                  exercise(SK_UNARMED_COMBAT, 1);
            }

            if ((!helpless || you.skills[SK_FIGHTING] < 2) && one_chance_in(3))
            {
                exercise(SK_FIGHTING, 1);
            }
        }

        if ( defender->hit_points < 1 )
        {
            /* thing_thrown = 1; */
#ifdef WIZARD
            itoa(damage_done, st_prn, 10);
            strcpy(info, "Hit for ");
            strcat(info, st_prn);       /* note: doesn't take account of special weapons etc */
            strcat(info, ".");
            mpr(info);
#endif
            if ( ur_armed
                && you.inv_class[you.equip[EQ_WEAPON]] == OBJ_WEAPONS
                && you.inv_dam[you.equip[EQ_WEAPON]] < NWPN_SINGING_SWORD
                && you.inv_dam[you.equip[EQ_WEAPON]] % 30 == SPWPN_VAMPIRICISM )
            {
                if ( ( mons_holiness(defender->type) == MH_NATURAL || mons_holiness(defender->type) == MH_HOLY )
                    && damage_done > 0
                    && you.hp < you.hp_max
                    && !one_chance_in(5) )
                {
                    mpr("You feel better.");

                    inc_hp(1 + random2(damage_done), false);    // more than if not killed

                    if ( you.hunger_state != HS_ENGORGED )
                      lessen_hunger(30 + random2avg(59,2), true);

                    naughty(NAUGHTY_NECROMANCY, 2);
                }
            }

            monster_die(defender, KILL_YOU, 0);

            if ( defender->type == MONS_GIANT_SPORE )
              mpr("You hit the giant spore.");
            return;
        }

        if ( damage_done < 1 && ( defender->enchantment[2] != ENCH_INVIS || player_see_invis() ) )
        {
            hit = true;

            strcpy(info, "You hit ");
            strcat(info, monam(defender->number, defender->type, defender->enchantment[2], 1));
            strcat(info, ", but do no damage.");
            mpr(info);
        }
    }
    else
    {
        hit = false;

        if ( (your_to_hit + heavy_armour / 2) >= defender->evasion )
          strcpy(info, "Your armour prevents you from hitting ");
        else
          strcpy(info, "You miss ");

        strcat(info, monam(defender->number, defender->type, defender->enchantment[2], 1));
        strcat(info, ".");
        mpr(info);
    }

    if ( hit && damage_done > 0 || ( hit && damage_done < 1 && defender->enchantment[2] == ENCH_INVIS))
    {
        strcpy(info, "You ");
        strcat(info, damage_noise);
        strcat(info, " ");
        strcat(info, monam(defender->number, defender->type, defender->enchantment[2], 1));
        strcat(info, damage_noise2);
#ifdef WIZARD
        itoa(damage_done, st_prn, 10);
        strcat(info, " for ");
        strcat(info, st_prn);   /* note: doesn't take account of special weapons etc */
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

        if ( mons_holiness(defender->type) == MH_HOLY )
          done_good(GOOD_KILLED_ANGEL_I, 1);

        if ( you.special_wield == SPWLD_TORMENT )
        {
            torment(you.x_pos, you.y_pos);
            naughty(NAUGHTY_UNHOLY, 5);
        }
        if ( you.special_wield == SPWLD_ZONGULDROK || you.special_wield == SPWLD_CURSE )
          naughty(NAUGHTY_NECROMANCY, 3);
    }

    if ( defender->type == MONS_JELLY
         || defender->type == MONS_BROWN_OOZE
         || defender->type == MONS_ACID_BLOB
         || defender->type == MONS_ROYAL_JELLY )
      weapon_acid(5);

    int specdam = 0;

/* remember, damage_done is still useful! */

    int weap_dam = 0;

    if ( hit
        && !ur_armed
        && special_brand != SPWPN_NORMAL )
    {
        /* No electrocution - this references inv_plus2 */
        weap_dam = special_brand;
        goto dam_thing;
    }

    if ( hit
        && ur_armed
        && you.inv_class[you.equip[EQ_WEAPON]] == OBJ_WEAPONS )
    {
        if ( is_demonic(you.inv_type[you.equip[EQ_WEAPON]]) )
          naughty(NAUGHTY_UNHOLY, 1);

        weap_dam = you.inv_dam[you.equip[EQ_WEAPON]] % 30;

        if ( you.inv_dam[you.equip[EQ_WEAPON]] >= NWPN_SINGING_SWORD )
          weap_dam = 0;

        if ( you.inv_dam[you.equip[EQ_WEAPON]] % 30 >= SPWPN_RANDART_I )
          weap_dam = randart_wpn_properties(you.inv_class[you.equip[EQ_WEAPON]], you.inv_type[you.equip[EQ_WEAPON]], you.inv_dam[you.equip[EQ_WEAPON]], you.inv_plus[you.equip[EQ_WEAPON]], you.inv_plus2[you.equip[EQ_WEAPON]], 0, RAP_BRAND);

        switch (you.inv_dam[you.equip[EQ_WEAPON]])
        {
          case NWPN_SWORD_OF_CEREBOV:
            weap_dam = 1;
            break;
          case NWPN_STAFF_OF_OLGREB:
            weap_dam = 6;
            break;
          case NWPN_VAMPIRES_TOOTH:
            weap_dam = 13;
            break;
        }
    }

    // jmf: BEGIN STAFF HACK
    // How much bonus damage will a staff of <foo> do?
    // FIXME: make these not macros. inline functions?
    // actually, it will all be pulled out and replaced by functions -- {dlb}
#define STAFF_DAMAGE(SKILL) ((3 + random2(you.skills[ SKILL ] ) \
                                + random2(you.skills[ SKILL ] ) \
                                + random2(you.skills[ SKILL ] )) / 6 )
#define STAFF_COST 2
#define CAT_MONSTER_INFO(MONSTER) strcat(info, \
                                  monam(menv[MONSTER].number, \
                                        menv[MONSTER].type, \
                                        menv[MONSTER].enchantment[2], 1) )

#define CAT_PUNCTUATION(DAMAGE) if (DAMAGE < 3) strcat(info, "."); \
                                if (DAMAGE >= 3) strcat(info, "!"); \
                                if (DAMAGE >= 10) strcat(info, "!"); \
                                if (DAMAGE >  20) strcat(info, "!");


    if (ur_armed && you.inv_class[you.equip[EQ_WEAPON]] == OBJ_STAVES)
    {
        specdam = 0;
        if (you.magic_points >= STAFF_COST)
            switch (you.inv_type[you.equip[EQ_WEAPON]])
            {
            case STAFF_AIR:
                if (damage_done + you.skills[SK_AIR_MAGIC] > random2(30))
                {
                    if (mons_res_elec(defender->type))
                        break;

                    CAT_MONSTER_INFO(monster_attacked);
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

                if ( defender->inv[MSLOT_ARMOUR] != NON_ITEM && mitm.special[defender->inv[MSLOT_ARMOUR]] % 30 == SPARM_COLD_RESISTANCE )
                {
                    specdam /= 3;
                }

                if ( mons_res_cold(defender->type) < 0
                    && ( defender->inv[MSLOT_ARMOUR] == NON_ITEM || mitm.special[defender->inv[MSLOT_ARMOUR]] % 30 != SPARM_COLD_RESISTANCE))
                {
                    specdam += STAFF_DAMAGE(SK_ICE_MAGIC);
                }

                if ( specdam )
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

                if ( defender->inv[MSLOT_ARMOUR] != NON_ITEM && mitm.special[defender->inv[MSLOT_ARMOUR]] % 30 == SPARM_FIRE_RESISTANCE )
                {
                    specdam = (random2(damage_done) / 2 + 1) / 3;
                }

                if ( mons_res_fire(defender->type) < 0
                    && (defender->inv[MSLOT_ARMOUR] == NON_ITEM || mitm.special[defender->inv[MSLOT_ARMOUR]] % 30 != SPARM_FIRE_RESISTANCE))
                {
                    specdam += STAFF_DAMAGE(SK_FIRE_MAGIC);
                }

                if ( specdam )
                {
                    strcpy(info, "You burn ");
                    CAT_MONSTER_INFO(monster_attacked);
                    CAT_PUNCTUATION(specdam);
                    mpr(info);
                }
                break;

            case STAFF_POISON:
                if ( damage_done + you.skills[SK_POISON_MAGIC] > random2(30) )
                  poison_monster(defender, true);
                break;

            case STAFF_DEATH:
                // FIXME: bring into line with other staff functions
                if ( ( mons_holiness(defender->type) == MH_NATURAL || mons_holiness(defender->type) == MH_HOLY )
                    && random2(8) <= you.skills[SK_NECROMANCY])
                {
                    strcpy(info, monam(defender->number, defender->type, defender->enchantment[2], 0));
                    strcat(info, " convulses in agony");
                    specdam = STAFF_DAMAGE(SK_NECROMANCY);
                    CAT_PUNCTUATION(specdam);
                    mpr(info);
                    naughty(NAUGHTY_NECROMANCY, 4);
                }
                break;

            default:
              mpr("You're wielding some staff I've never heard of! (fight.cc)");
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
            }

        if (specdam > 0)
        {
            dec_mp(STAFF_COST);

            if (you.inv_ident[you.equip[EQ_WEAPON]] < 3)
            {
                you.inv_ident[you.equip[EQ_WEAPON]] = 3;
                strcpy(info, "You are wielding ");
                in_name(you.equip[EQ_WEAPON], 3, str_pass);
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


    if ( ur_armed && hit
        && you.inv_class[you.equip[EQ_WEAPON]] == OBJ_WEAPONS )
    {
dam_thing:
        switch (weap_dam)
        {
        case SPWPN_NORMAL:
            break;

        case SPWPN_FLAMING:
            specdam = 0;

            if ( !mons_res_fire(defender->type) )
            {
                specdam = random2(damage_done) / 2 + 1;
                if ( defender->inv[MSLOT_ARMOUR] != NON_ITEM && mitm.special[defender->inv[MSLOT_ARMOUR]] % 30 == SPARM_FIRE_RESISTANCE )
                {
                    specdam = (random2(damage_done) / 2 + 1) / 3;
                }
            }

            if ( mons_res_fire(defender->type) == -1
                && (defender->inv[MSLOT_ARMOUR] == NON_ITEM || mitm.special[defender->inv[MSLOT_ARMOUR]] % 30 != SPARM_FIRE_RESISTANCE))
            {
                specdam = random2(damage_done) + 1;
            }

            if ( specdam )
            {
                strcpy(info, "You burn ");
                strcat(info, monam(defender->number, defender->type, defender->enchantment[2], 1));

                if ( specdam < 3 )
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
            if ( !mons_res_cold(defender->type) )
            {
                specdam = 1 + random2(damage_done) / 2;
                if (defender->inv[MSLOT_ARMOUR] != NON_ITEM && mitm.special[defender->inv[MSLOT_ARMOUR]] % 30 == SPARM_COLD_RESISTANCE)
                {
                    specdam = (1 + random2(damage_done) / 2) / 3;
                }
            }

            if ( mons_res_cold(defender->type) == -1
                && (defender->inv[MSLOT_ARMOUR] == NON_ITEM || mitm.special[defender->inv[MSLOT_ARMOUR]] % 30 != SPARM_COLD_RESISTANCE))
            {
                specdam = 1 + random2(damage_done);
            }

            if ( specdam )
            {
                strcpy(info, "You freeze ");
                strcat(info, monam(defender->number, defender->type, defender->enchantment[2], 1));

                if ( specdam < 3 )
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
// there should be a case in here for holy monsters, see elsewhere in fight.cc {dlb}
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

            if ( mons_flies(defender->type) )
              break;
            else if ( mons_res_elec(defender->type) )
              break;
            else if ( one_chance_in(3) )
            {
                mpr("There is a sudden explosion of sparks!");
                specdam += random2avg(28,3);
            }
            break;

        case SPWPN_ORC_SLAYING:
            if ( mons_charclass(defender->type) == MONS_ORC )
              hurt_monster(defender, 1 + random2(damage_done));
            break;

        case SPWPN_VENOM:
            if (!one_chance_in(4))
                poison_monster(defender, true);
            break;

        case SPWPN_DRAINING:
            if ( ( mons_holiness(defender->type) == MH_UNDEAD || mons_holiness(defender->type) == MH_DEMONIC )
                || one_chance_in(3))
                break;

            strcpy(info, "You drain ");
            strcat(info, monam(defender->number, defender->type, defender->enchantment[2], 1));
            strcat(info, "!");
            mpr(info);

            if ( one_chance_in(5) )
              defender->hit_dice--;

            defender->max_hit_points -= 2 + random2(3);
            defender->hit_points -= 2 + random2(3);

            if ( defender->hit_points >= defender->max_hit_points )
              defender->hit_points = defender->max_hit_points;

            if ( defender->hit_dice < 1 )
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

            if ( mons_holiness(defender->type) == MH_UNDEAD
                  || mons_holiness(defender->type) == MH_DEMONIC )
              break;          // should really also prevent draining eg golems
            else if ( damage_done < 1 )
              break;
            else if ( you.hp == you.hp_max || one_chance_in(5) )
              break;

            mpr("You feel better.");

            if ( ur_armed
                && you.inv_dam[you.equip[EQ_WEAPON]] == NWPN_VAMPIRES_TOOTH )
              inc_hp(damage_done, false);
            else                   // thus is probably more valuable on larger weapons?
              inc_hp(1 + random2(damage_done), false);

            if ( you.hunger_state != HS_ENGORGED )
              lessen_hunger(random2avg(59,2), true);

            naughty(NAUGHTY_NECROMANCY, 2);
            break;

        case SPWPN_DISRUPTION:
            specdam = 0;
            if (mons_holiness(defender->type) == MH_UNDEAD
                && !one_chance_in(3))
            {
                simple_monster_message(defender, " shudders.");

                specdam += random2avg((1 + (damage_done * 3)),3);
            }
            break;

        case SPWPN_PAIN:
            specdam = 0;
            if ( ( mons_holiness(defender->type) == MH_NATURAL || mons_holiness(defender->type) == MH_HOLY )
                && random2(8) <= you.skills[SK_NECROMANCY])
            {
                simple_monster_message(defender, " convulses in agony.");
                specdam += random2(1 + (you.skills[SK_NECROMANCY] << 1));
            }
            naughty(NAUGHTY_NECROMANCY, 4);
            break;

        case SPWPN_DISTORTION:
        //jmf: blink frogs *like* distortion
        // I think could be amended to let blink frogs "grow" like jellies do {dlb}
            if ( defender->type == MONS_BLINK_FROG && one_chance_in(5) )
            {
                simple_monster_message(defender, " basks in the translocular energy.");

                heal_monster(defender, 1 + random2avg(7,2), true);    // heh heh {dlb}
                break;
            }

            if (one_chance_in(3))
            {
                strcpy(info, "Space bends around ");
                strcat(info, monam(defender->number, defender->type, defender->enchantment[2], 1));
                strcat(info, ".");
                mpr(info);
                specdam += 1 + random2avg(7,2);
                break;
            }

            if (one_chance_in(3))
            {
                strcpy(info, "Space warps horribly around ");
                strcat(info, monam(defender->number, defender->type, defender->enchantment[2], 1));
                strcat(info, "!");
                mpr(info);
                specdam += 3 + random2avg(24,2);
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
          {  // declaring these just to pass to the enchant function
            struct bolt beam_temp;

            // ick -- colour tells beam function (confusion == RED)
            beam_temp.colour = RED;

            mons_ench_f2(defender, &beam_temp);

            you.confusing_touch -= random2(20);

            if ( you.confusing_touch < 1 )
              you.confusing_touch = 1;
          }
          break;
        }                       /* end switch */
    }

post_spec_damage:             //jmf: added

    if ( mons_holiness(defender->type) == MH_HOLY )
      naughty(NAUGHTY_ATTACK_HOLY, defender->hit_dice);

    if ( defender->type == MONS_HYDRA )
    {
        if ( ur_armed
            && (damage_type(you.inv_class[you.equip[EQ_WEAPON]], you.inv_type[you.equip[EQ_WEAPON]]) == DVORP_SLICING
                || damage_type(you.inv_class[you.equip[EQ_WEAPON]], you.inv_type[you.equip[EQ_WEAPON]]) == DVORP_CHOPPING))
        {
            if ((damage_done < 4 || coinflip())
                && (you.inv_class[you.equip[EQ_WEAPON]] != OBJ_WEAPONS
                    || you.inv_dam[you.equip[EQ_WEAPON]] != WPN_BATTLEAXE))
            {
                goto mons_dies;
            }

            strcpy(info, "You ");
            temp_rand = random2(4);
            strcat(info, (temp_rand == 0) ? "slice" :
                         (temp_rand == 1) ? "lop" :
                         (temp_rand == 2) ? "chop"
                                          : "hack" );
            strcat(info, " one of ");
            strcat(info, monam(defender->number, defender->type, defender->enchantment[2], 1));
            strcat(info, "'s heads off.");
            mpr(info);

            defender->number--;

            if ( defender->number < 1 )
            {
                defender->hit_points = -1;
                goto mons_dies;
            }

            if ( you.inv_class[you.equip[EQ_WEAPON]] == OBJ_WEAPONS
                && you.inv_dam[you.equip[EQ_WEAPON]] % 30 == SPWPN_FLAMING )
            {
                goto mons_dies; // cauterised
            }

            if ( defender->number >= 18 )
              goto mons_dies;

            simple_monster_message(defender, " grows two more!");

            defender->number += 2;
        }
    }
/* remember, the hydra function sometimes skips straight to mons_dies */

    hurt_monster(defender, specdam);

mons_dies:
    if ( defender->hit_points < 1 )
    {
        monster_die(defender, KILL_YOU, 0);
        return;
    }

    if ( you.invis && defender->behavior == BEH_SLEEP )
      defender->behavior = BEH_CHASING_I;

    if ( you.invis && coinflip() )
    {
        defender->target_x = you.x_pos;
        defender->target_y = you.y_pos;
        defender->behavior = BEH_CHASING_I;
    }

    if ( unarmed_attacks )
    {
        char attack_name[20] = "";
        int sc_dam = 0;

        int unarmed_attack = UNAT_NO_ATTACK;

        if ( can_do_unarmed_combat )
        {
            unarmed_attack = ( coinflip() ? UNAT_HEADBUTT : UNAT_KICK );

            if ( ( you.attribute[ATTR_TRANSFORMATION] == TRAN_DRAGON || player_genus(GENPC_DRACONIAN) )
                 && one_chance_in(3) )
            {
                unarmed_attack = UNAT_TAILSLAP;
            }

            if ( coinflip() )
                unarmed_attack = UNAT_PUNCH;
        }

        for (unsigned char scount = 0; scount < 4; scount++)
        {
            switch (scount)
            {
              case 0:
                if (unarmed_attack != UNAT_KICK) //jmf: hooves mutation
                  if ((you.species != SP_CENTAUR && !you.mutation[MUT_HOOVES])
                        || coinflip())
                    continue;
                if (you.attribute[ATTR_TRANSFORMATION] == TRAN_SERPENT_OF_HELL
                     || you.attribute[ATTR_TRANSFORMATION] == TRAN_ICE_BEAST
                     || you.attribute[ATTR_TRANSFORMATION] == TRAN_DRAGON
                     || you.attribute[ATTR_TRANSFORMATION] == TRAN_SPIDER)
                  continue;

                strcpy(attack_name, "kick");
                sc_dam = ( (you.mutation[MUT_HOOVES]  || you.species == SP_CENTAUR ) ? 10 : 5 );
                break;

              case 1:
                if (unarmed_attack != UNAT_HEADBUTT)
                  if ((you.species != SP_MINOTAUR
                       && ( !you.mutation[MUT_HORNS]
                           && you.species != SP_KENKU ) )
                       || !one_chance_in(3))
                    continue;

                if (you.attribute[ATTR_TRANSFORMATION] == TRAN_SERPENT_OF_HELL
                    || you.attribute[ATTR_TRANSFORMATION] == TRAN_SPIDER
                    || you.attribute[ATTR_TRANSFORMATION] == TRAN_ICE_BEAST
                    || you.attribute[ATTR_TRANSFORMATION] == TRAN_DRAGON)
                  continue;

                strcpy(attack_name, (you.species == SP_KENKU) ? "peck" : "headbutt");

                sc_dam = 5 + you.mutation[MUT_HORNS] * 3;

                if (you.species == SP_MINOTAUR)
                  sc_dam += 5;

                if (you.equip[EQ_HELMET] != -1
                    && you.inv_plus2[you.equip[EQ_HELMET]] <= 1)
                {
                    sc_dam += 2;

                    if (you.inv_dam[you.equip[EQ_HELMET]] / 30 == DHELM_SPIKED
                        || you.inv_dam[you.equip[EQ_HELMET]] / 30 == DHELM_HORNED)
                      sc_dam += 3;
                }
                break;

            case 2:      /* draconians */
                if ( unarmed_attack != UNAT_TAILSLAP )
                  if ( !player_genus(GENPC_DRACONIAN) || !one_chance_in(4))
                    continue;

                if (you.attribute[ATTR_TRANSFORMATION] == TRAN_SPIDER
                    || you.attribute[ATTR_TRANSFORMATION] == TRAN_ICE_BEAST)
                  continue;

                strcpy(attack_name, "tail-slap");

                sc_dam = 6;

                /* grey dracs have spiny tails, or something */    // maybe add this to player messaging {dlb}
                if ( you.species == SP_GREY_DRACONIAN && you.experience_level > 5 )
                  sc_dam = 12;
                break;

            case 3:
                if (unarmed_attack != UNAT_PUNCH)
                  continue;

                if (you.attribute[ATTR_TRANSFORMATION] == TRAN_SERPENT_OF_HELL
                    || you.attribute[ATTR_TRANSFORMATION] == TRAN_SPIDER
                    || you.attribute[ATTR_TRANSFORMATION] == TRAN_ICE_BEAST
                    || you.attribute[ATTR_TRANSFORMATION] == TRAN_DRAGON)
                  continue;

                /* no punching with a shield or 2-handed wpn, except staves */
                if ( bearing_shield
                      || coinflip()
                      || ( ur_armed
                          && hands_reqd_for_weapon(you.inv_class[you.equip[EQ_WEAPON]], you.inv_type[you.equip[EQ_WEAPON]]) == HANDS_TWO_HANDED) )
                  continue;

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

            your_to_hit = 13 + you.dex / 2 + you.skills[SK_UNARMED_COMBAT] / 2 + you.skills[SK_FIGHTING] / 5;

            if ( wearing_amulet(AMU_INACCURACY) )
              your_to_hit -= 5;

            if ( you.is_undead != US_UNDEAD )
              make_hungry(2, true);

            if ( you.hunger_state == HS_STARVING )
              your_to_hit -= 3;

            your_to_hit += slaying_bonus(PWPN_HIT);
            your_to_hit = random2(your_to_hit);

            damage = sc_dam;    //4 + you.experience_level / 3;

            alert();

            if (your_to_hit >= defender->evasion || one_chance_in(15))
            {
                bool hit = true;
                int dammod = 10;

                if (you.strength > 11)
                    dammod += random2(you.strength - 11) / 3;
                if (you.strength < 9)
                    dammod -= random2(9 - you.strength) / 2;

                damage *= dammod;
                damage /= 10;

                damage_done = (int) random2(damage);

                damage_done *= 40 + (random2(you.skills[SK_FIGHTING] + 1));
                damage_done /= 40;
                damage_done *= 25 + (random2(you.skills[SK_UNARMED_COMBAT] + 1));
                damage_done /= 25;

                if ( you.might > 1 )
                  damage_done += 1 + random2(10);

                if ( you.hunger_state == HS_STARVING )
                  damage_done -= random2(5);

                damage_done += slaying_bonus(PWPN_DAMAGE);

                damage_done -= random2(1 + defender->armor_class);

                if (damage_done < 1)
                  damage_done = 0;
                else
                  hurt_monster(defender, damage_done);

                if (damage_done > 0 )
                  if ((!helpless || you.skills[SK_FIGHTING] < 2)
                        && one_chance_in(5))
                    exercise(SK_FIGHTING, 1);

                if ( defender->hit_points < 1 )
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
                    return;
                }

                if ( damage_done < 1
                    && ( defender->enchantment[2] != ENCH_INVIS || player_see_invis() ) )
                {
                    strcpy(info, "You ");
                    strcat(info, attack_name);
                    strcat(info, " ");
                    strcat(info, monam(defender->number, defender->type, defender->enchantment[2], 1));
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
                strcat(info, monam(defender->number, defender->type, defender->enchantment[2], 1));
                strcat(info, ".");
                mpr(info);
                damage_done = -99;
            }

            if ( damage_done > 0
                  || ( damage_done < 1
                      && defender->enchantment[2] == ENCH_INVIS
                      && damage_done != -99 ) )
            {
                strcpy(info, "You ");
                strcat(info, attack_name);
                strcat(info, " ");
                strcat(info, monam(defender->number, defender->type, defender->enchantment[2], 1));

#ifdef WIZARD
                itoa(damage_done, st_prn, 10);
                strcat(info, " for ");
                strcat(info, st_prn);
#endif

                if ( damage_done < HIT_WEAK )
                  strcat(info, ".");
                else if ( damage_done < HIT_MED )
                  strcat(info, "!");
                else if ( damage_done < HIT_STRONG )
                  strcat(info, "!!");
                else
                  strcat(info, "!!!");

                mpr(info);

                if ( mons_holiness(defender->type) == MH_HOLY )
                  done_good(GOOD_KILLED_ANGEL_I, 1);

                hit = true;
            }

            if ( !helpless || you.skills[SK_UNARMED_COMBAT] < 2 )
              exercise(SK_UNARMED_COMBAT, 1);
        }
    }

    if ( hit )
      print_wounds(defender);

    return;

}          // end you_attack()




void monster_attack( int monster_attacking )
{

    struct monsters *attacker = &menv[monster_attacking];

    int damage_taken = 0;
    bool hit = false;
    bool blocked = false;
    bool water_attack = false;    // being attacked by a water creature while standing in water?
    bool bearing_shield = (you.equip[EQ_SHIELD] != -1);     // compacts code a bit {dlb}
    int mmov_x = 0;
    char str_pass[80];
    int specdam = 0;
    char heads = 0;               // for hydras {dlb}
    int hand_used = 0;

    if ( attacker->type == MONS_HYDRA )
      heads = attacker->number;

    if ( attacker->type == MONS_GIANT_SPORE )
    {
        attacker->hit_points = -1;
        return;
    }

    you.pet_target = monster_attacking;    // ??? {dlb}

    if (attacker->behavior == BEH_ENSLAVED)
        return;

    if ( monster_habitat(attacker->type) != DNGN_FLOOR
        && attacker->number == 1 )
      return;

    if ( wearing_amulet(AMU_WARDING)
          || ( you.religion == GOD_VEHUMET && you.duration[DUR_PRAYER]
              && ( !player_under_penance() && you.piety >= 75 ) ) )
    {
        if ( (attacker->enchantment[1] >= ENCH_ABJ_I && attacker->enchantment[1] <= ENCH_ABJ_VI)
             || (attacker->enchantment[1] >= ENCH_FRIEND_ABJ_I && attacker->enchantment[1] <= ENCH_FRIEND_ABJ_VI) )

          if ( coinflip() )    // should be scaled {dlb}
          {
              simple_monster_message(attacker, " tries to attack you, but flinches away.");
              return;
          }
    }

    if ( grd[attacker->x][attacker->y] == DNGN_SHALLOW_WATER
        && !mons_flies(attacker->type)
        && monster_habitat(attacker->type) == DNGN_FLOOR
        && one_chance_in(4) )
    {
        simple_monster_message(attacker, " splashes around in the water.");
        return;
    }

    if ( grd[you.x_pos][you.y_pos] == DNGN_SHALLOW_WATER && !you.levitation && monster_habitat(attacker->type) == DNGN_DEEP_WATER )
    {
        water_attack = true;
        simple_monster_message(attacker, " uses the watery terrain to its advantage.");
    }

    char runthru = 0;

    for (runthru = 0; runthru < 4; runthru++)
    {

        blocked = false;

        if (attacker->type == MONS_HYDRA)
        {
            if ( heads < 1 )
              break;
            runthru = 0;
            heads--;
        }

        char mdam = mondamage(attacker->type, runthru);

        if ( attacker->type == MONS_ZOMBIE_SMALL
              || attacker->type == MONS_ZOMBIE_LARGE
              || attacker->type == MONS_SKELETON_SMALL
              || attacker->type == MONS_SKELETON_LARGE
              || attacker->type == MONS_SPECTRAL_THING )
        {
            mdam = mondamage(attacker->number, runthru);
// these are cumulative, of course: {dlb}
            if ( mdam > 1 )
              mdam--;
            if ( mdam > 4 )
              mdam--;
            if ( mdam > 11 )
              mdam--;
            if ( mdam > 14 )
              mdam--;
        }

        if ( mdam == 0 )
          break;

        if ( attacker->type == MONS_TWO_HEADED_OGRE
            && runthru == 1 && attacker->inv[MSLOT_MISSILE] != NON_ITEM )
          hand_used = 1;

        damage_taken = 0;

        int mons_to_hit = 16 + attacker->hit_dice;        // * attacker->hit_dice; // * 3

        if ( water_attack )
          mons_to_hit += 5;

        if ( attacker->inv[hand_used] != NON_ITEM )
        {
            it_name(attacker->inv[hand_used], 0, str_pass);

            if (mitm.base_type[attacker->inv[hand_used]] != 0 || strstr(str_pass, "questionable item") != NULL)
            {
                destroy_item(attacker->inv[hand_used]);
                attacker->inv[hand_used] = NON_ITEM;
            }
        }

        if (attacker->inv[hand_used] != NON_ITEM)
        {
            if (mitm.pluses[attacker->inv[hand_used]] - 50 > 130)
            {
                mons_to_hit += mitm.pluses[attacker->inv[hand_used]] - 50 - 100;
            }
            else
            {
                mons_to_hit += mitm.pluses[attacker->inv[hand_used]] - 50;
            }

            mons_to_hit += property(OBJ_WEAPONS, mitm.sub_type[attacker->inv[MSLOT_WEAPON]], PWPN_HIT);

            if ( attacker->speed_increment >= 50 )
              attacker->speed_increment -= (property(mitm.base_type[attacker->inv[MSLOT_WEAPON]], mitm.sub_type[attacker->inv[MSLOT_WEAPON]], PWPN_SPEED) - 10);
        }

        if (attacker->type == MONS_KILLER_BEE || attacker->type == MONS_WORM)
        {
            /* maybe this will work better: */
            if ( attacker->speed_increment >= 20 )
              attacker->speed_increment -= 10;
        }

        if ( player_shield_class() > 0 && !you.paralysis && !you.conf
            && random2(15 + attacker->hit_dice + (5 * you.shield_blocks))
                 <= random2(player_shield_class()) + (random2(you.dex) / 5) - 1)
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
              strcat(info, monam(attacker->number, attacker->type, attacker->enchantment[2], 1));

            strcat(info, "'s attack.");
            mpr(info);

            blocked = true;
            hit = false;

            if ( bearing_shield && one_chance_in(4) )
              exercise(SK_SHIELDS, 1);
        }
        else if ( player_light_armour() && one_chance_in(3) )
          exercise(SK_DODGING, 1);

        if ( !blocked
            && (random2(mons_to_hit) >= random2limit(player_evasion(), 40) + random2(you.dex) / 3 - 2
                || one_chance_in(15)))
        {
            hit = true;

            int damage_size = 0;

            if ( attacker->inv[hand_used] != NON_ITEM && mitm.base_type[attacker->inv[hand_used]] == OBJ_WEAPONS && (mitm.sub_type[attacker->inv[hand_used]] < WPN_SLING || mitm.sub_type[attacker->inv[hand_used]] > WPN_CROSSBOW))
            {

                damage_size = property(mitm.base_type[attacker->inv[MSLOT_WEAPON]], mitm.sub_type[attacker->inv[MSLOT_WEAPON]], PWPN_DAMAGE);
                damage_taken = random2(damage_size);

                if ( mitm.special[attacker->inv[hand_used]] / 30 == DWPN_ORCISH
                    && mons_charclass(attacker->type) == MONS_ORC && coinflip() )
                  damage_taken++;

                if ( mitm.pluses2[attacker->inv[hand_used]] > 130 )
                {
                    if (mitm.pluses2[attacker->inv[hand_used]] - 150 >= 0)        /* + or 0 to-dam */
                      damage_taken += random2(mitm.pluses2[attacker->inv[hand_used]] - 150 + 1);
                    else
                      damage_taken -= random2(abs(mitm.pluses2[attacker->inv[hand_used]] - 150 + 1));   /* - to-dam */
                }
                else
                {
                    if (mitm.pluses2[attacker->inv[hand_used]] - 50 >= 0)         /* + or 0 to-dam */
                      damage_taken += random2(mitm.pluses2[attacker->inv[hand_used]] - 50 + 1);
                    else
                      damage_taken -= random2(abs(mitm.pluses2[attacker->inv[hand_used]] - 50 + 1));    /* - to-dam */
                }

                damage_taken -= 1 + random2(3);         //1;
            }

            damage_size += mdam;
            damage_taken += 1 + random2(mdam);

            if ( water_attack )
             damage_taken <<= 1;

            int ac = player_AC();

            if ( ac > 0 )
            {
                int damage_reduction = random2(1 + ac);

                if ( !player_light_armour() )
                {
                    int percent = you.skills[SK_ARMOUR] + property(OBJ_ARMOUR, you.inv_type[you.equip[EQ_BODY_ARMOUR]], PARM_AC);

                    if ( percent > 50 )
                      percent = 50;

                    int min = 1 + ( (damage_size * percent) / 100 );

                    if ( min > ac / 2 )
                      min = ac / 2;

                    if ( damage_reduction < min )
                      damage_reduction = min;
                }

                damage_taken -= damage_reduction;
            }

            if ( damage_taken < 1 )
              damage_taken = 0;

        }
        else if ( !blocked )
        {
            hit = false;
            simple_monster_message(attacker, " misses you.");
        }

        if ( damage_taken < 1 && hit && !blocked )
          simple_monster_message(attacker, " hits you but doesn't do any damage.");

        if ( damage_taken > 0 )
        {
            hit = true;

            mmov_x = attacker->inv[hand_used];

            if (attacker->type == MONS_DANCING_WEAPON)
            {
                it_name(mmov_x, 0, str_pass);
                strcpy(info, str_pass);
            }
            else
              strcpy(info, monam(attacker->number, attacker->type, attacker->enchantment[2], 0));

            strcat(info, " hits you");

#ifdef WIZARD
            itoa(mmov_x, st_prn, 10);
            strcat(info, " it:");
            strcat(info, st_prn);       // note: doesn't take account of special weapons etc
#endif

            if ( attacker->type != MONS_DANCING_WEAPON && mmov_x != NON_ITEM && mitm.base_type[mmov_x] == OBJ_WEAPONS && !launches_things(mitm.sub_type[mmov_x]) )
            {
                strcat(info, " with ");
                it_name(mmov_x, 3, str_pass);    // was 7
                strcat(info, str_pass);
            }

            strcat(info, "!");
            mpr(info);

            if ( hit )
            {
                if ( you.equip[EQ_BODY_ARMOUR] != -1 )
                    if ( !player_light_armour() && coinflip()
                        && random2(1000) <= mass(OBJ_ARMOUR, you.inv_type[you.equip[EQ_BODY_ARMOUR]]) )
                      exercise(SK_ARMOUR, (coinflip() ? 2 : 1));     // raised from 1 {bwross}

            }
/* special attacks: */

            int mclas = attacker->type;

            if ( mclas == MONS_KILLER_KLOWN )
            {
                switch (random2(6))
                {
                  case 0:
                    mclas = MONS_SNAKE;      // scorp     // comment and enum do not match {dlb}
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
                if ( player_res_poison() )
                  break;
                if ( one_chance_in(20) || ( damage_taken > 3 && one_chance_in(4) ) )

                {
                    strcpy(info, monam(attacker->number, attacker->type, attacker->enchantment[2], 0));
                    strcat(info, " stings you!");
                    mpr(info);
                    you.poison++;
                    if ( attacker->type == MONS_REDBACK )
                      you.poison += random2avg(9,2) + 3;
                }
                break;

            case MONS_KILLER_BEE:
            case MONS_BUMBLEBEE:
                if ( !player_res_poison() && ( one_chance_in(20) || ( damage_taken > 2 && one_chance_in(3) ) ) )

                {
                    strcpy(info, monam(attacker->number, attacker->type, attacker->enchantment[2], 0));
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
                if ( you.is_undead )
                  break;
                if (one_chance_in(20) || (damage_taken > 2 && one_chance_in(3)))       // both sides call random2() - looking familiar by now {dlb}

                {
                    mpr("You feel your flesh start to rot away!");
                    you.rotting += random2(3) + 1;
                }
                if (one_chance_in(4))
                {
                    mpr("You feel ill.");

                    if ( you.disease > 100 )
                      you.disease = 210;
                    else
                      you.disease += 50 + random2(100);
                }
                break;

            case MONS_GIANT_MOSQUITO:
                if ( !one_chance_in(3) )
                  mpr("You feel ill.");

                if ( you.disease > 100 )
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
                mpr("You are engulfed in flame!");

                if (player_res_fire() > 100)
                  damage_taken += (15 + random2(15)) / 2 + ((player_res_fire() - 100) * (player_res_fire() - 100));

                if (player_res_fire() <= 100)
                  damage_taken += 15 + random2(15);

                if (player_res_fire() < 100)
                  damage_taken += 8 + random2(8);

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
                if ( !player_res_poison() && (one_chance_in(15) || (damage_taken > 2 && one_chance_in(4))))         // beginning to think this conditional should become a function {dlb}
                {
                    strcpy(info, monam(attacker->number, attacker->type, attacker->enchantment[2], 0));
                    strcat(info, " poisons you!");
                    mpr(info);

                    you.poison++;
                }
                break;

            case MONS_QUEEN_BEE:
            case MONS_GIANT_CENTIPEDE:
            case MONS_SOLDIER_ANT:
            case MONS_QUEEN_ANT:
                if ( !player_res_poison() )
                {
                    strcpy(info, monam(attacker->number, attacker->type, attacker->enchantment[2], 0));
                    strcat(info, " stings you!");
                    mpr(info);
                    you.poison += 2;
                }
                break;

            case MONS_SCORPION:
            case MONS_BROWN_SNAKE:
            case MONS_BLACK_SNAKE:
            case MONS_YELLOW_SNAKE:

                if ( !player_res_poison() && (one_chance_in(20) || (damage_taken > 2 && one_chance_in(4))))         // yep, this should be a function {dlb}
                {
                    strcpy(info, monam(attacker->number, attacker->type, attacker->enchantment[2], 0));
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
            case MONS_SOUL_EATER:       // shadow devil     // enum does not match comment 14jan2000 {dlb}
            case MONS_SHADOW_FIEND:
            case MONS_SPECTRAL_WARRIOR: // spectre          // enum does not match comment 14jan2000 {dlb}
            case MONS_ORANGE_RAT:
            case MONS_SHADOW_WRAITH:
            case MONS_ANCIENT_LICH:
                if ( one_chance_in(30) || ( damage_taken > 5 && coinflip() ) )
                  drain_exp();
                break;

            case MONS_RED_WASP:
                if ( !player_res_poison() )
                  you.poison += ( coinflip() ? 2 : 1 );
            // intentional fall-through {dlb}
            case MONS_YELLOW_WASP:
                strcpy(info, monam(attacker->number, attacker->type, attacker->enchantment[2], 0));
                strcat(info, " stings you.");
                mpr(info);

                if ( !player_res_poison() && (one_chance_in(20) || (damage_taken > 2 && !one_chance_in(3))))        // maybe I should flip back the other way? {dlb}
                {
                    if ( you.paralysis > 0 )
                      mpr("You still can't move!");
                    else
                      mpr("You suddenly lose the ability to move!");

                    you.paralysis += 1 + random2(3);
                }
                break;

            case MONS_SPINY_WORM:
                if ( !player_res_poison() )
                {
                    strcpy(info, monam(attacker->number, attacker->type, attacker->enchantment[2], 0));
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
                if (player_res_cold() <= 100)
                {
                    damage_taken += attacker->hit_dice + random2(attacker->hit_dice << 1);
                    strcpy(info, monam(attacker->number, attacker->type, attacker->enchantment[2], 0));
                    strcat(info, " freezes you!");
                    mpr(info);
                }
                else
                {
                    damage_taken += attacker->hit_dice + random2(attacker->hit_dice << 1);
                    damage_taken /= (2 + (player_res_cold() - 100) * (player_res_cold() - 100));
                    strcpy(info, monam(attacker->number, attacker->type, attacker->enchantment[2], 0));
                    strcat(info, " chills you.");
                    mpr(info);
                }

                if (player_res_cold() < 100)
                    damage_taken += attacker->hit_dice + random2(attacker->hit_dice);

                scrolls_burn(1, OBJ_POTIONS);
                break;

            case MONS_VAMPIRE:
                if ( you.is_undead )
                  break;

/* ******************************************************************
                if ( damage_taken > 6 && one_chance_in(3) || !one_chance_in(20) )
                {
                    mpr("You feel less resilient.");
                    you.hp_max -= ( coinflip() ? 2 : 1 );
                    deflate_hp(you.hp_max, false);
                    heal_monster(attacker, 5 + random2(8), true);
                }
****************************************************************** */

                strcpy(info, monam(attacker->number, attacker->type, attacker->enchantment[2], 0));
                strcat(info, " draws strength from you!");
                mpr(info);

                simple_monster_message(attacker, " and is healed!");

                heal_monster(attacker, random2(damage_taken), true);    // heh heh {dlb}

                break;

            case MONS_SHADOW:
                if ( player_prot_life() <= random2(3)
                    && ( one_chance_in(20) || ( damage_taken > 0 && one_chance_in(3) ) ) )
                {
                    lose_stat(STAT_STRENGTH, 1);
                }
                break;

            case MONS_HUNGRY_GHOST:
                if ( you.is_undead == US_UNDEAD )
                  break;
                if ( one_chance_in(20) || ( damage_taken > 0 && coinflip() ) )
                  make_hungry(400, false);
                break;

            case MONS_GUARDIAN_NAGA:
                break;

            case MONS_PHANTOM:
            case MONS_INSUBSTANTIAL_WISP:
            case MONS_BLINK_FROG:
            case MONS_MIDGE:
                if ( one_chance_in(3) )
                {
                    simple_monster_message(attacker, " blinks.");
                    monster_blink(attacker);
                }
                break;

            case MONS_JELLYFISH:
            case MONS_ORANGE_DEMON:
// if ( !one_chance_in(3) ) break;
                if ( player_res_poison() )
                  break;
                if (attacker->type == MONS_ORANGE_DEMON && (!one_chance_in(4) || runthru != 1))
                  break;
                strcpy(info, monam(attacker->number, attacker->type, attacker->enchantment[2], 0));
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

/* use brek for level drain, maybe with beam variables, because so many creatures use it. */
        }

/* special weapons */
        if ( hit && (attacker->inv[hand_used] != NON_ITEM || ((attacker->type == MONS_PLAYER_GHOST || attacker->type == MONS_PANDEMONIUM_DEMON) && ghost.values[8] != 0)))
        {
            unsigned char itdam;

            if ( attacker->type == MONS_PLAYER_GHOST
                  || attacker->type == MONS_PANDEMONIUM_DEMON )
              itdam = ghost.values[8];
            else
              itdam = mitm.special[attacker->inv[hand_used]];

            specdam = 0;

            if ( itdam == NWPN_SWORD_OF_CEREBOV )
              goto flaming;

            if ( attacker->type == MONS_PLAYER_GHOST
                  || attacker->type == MONS_PANDEMONIUM_DEMON
                  || mitm.special[attacker->inv[hand_used]] < NWPN_SINGING_SWORD )
                switch ( itdam % 30 )
                {
                  case SPWPN_NORMAL:
                    break;

                  case SPWPN_FLAMING:
flaming:
                    specdam = 0;
                    if (player_res_fire() > 100)
                      damage_taken += (random2(damage_taken) / 2 + 1) / 2 + ((player_res_fire() - 100) * (player_res_fire() - 100));
                    if (player_res_fire() <= 100)
                      specdam = random2(damage_taken) / 2 + 1;
                    if (player_res_fire() < 100)
                      specdam += random2(damage_taken) / 2 + 1;

                    if ( specdam )
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
                      damage_taken += (random2(damage_taken) / 2 + 1) / 2 + ((player_res_cold() - 100) * (player_res_cold() - 100));

                    if ( specdam )
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
                    if ( attacker->type == MONS_PLAYER_GHOST )
                      break;  // ghosts can't wield holy wrath
                    if ( you.is_undead )
                    {
                        specdam = random2(damage_taken);

                        if ( specdam )
                        {
                            strcpy(info, "The wound is extremely painful");

                            if ( specdam < 3 )
                              strcat(info, ".");
                            else if ( specdam < 7 )
                              strcat(info, "!");
                            else
                              strcat(info, "!!");

                            mpr(info);
                        }
                    }
                    break;

                case SPWPN_ELECTROCUTION:
                    if ( you.levitation )   // you're not grounded
                      break;

                    if ( player_res_electricity() > 0 )  // resist lightning
                      break;
                    //if (menv [monster_attacking].type == MONS_PLAYER_GHOST) break;

                    specdam = 0;

                    if (mitm.pluses2[attacker->inv[hand_used]] <= 50 || mitm.pluses2[attacker->inv[hand_used]] > 130 && mitm.pluses2[attacker->inv[hand_used]] <= 150)
                      break;

                    if (one_chance_in(3))
                    {
                        mpr("You are electrocuted!");
                        specdam += 10 + random2(15);
                    }
                    break;

                case SPWPN_ORC_SLAYING:
                    if ( you.species == SP_HILL_ORC )
                    {
                        specdam = random2(damage_taken);

                        if ( specdam )
                        {
                            strcpy(info, "The wound is extremely painful");

                            if (specdam < 3)
                              strcat(info, ".");
                            else if (specdam < 7 )
                              strcat(info, "!");
                            else
                              strcat(info, "!!");

                            mpr(info);
                        }
                    }
                    break;

                case SPWPN_VENOM:
                    if ( !player_res_poison() && one_chance_in(3) )
                    {
                        simple_monster_message(attacker, (attacker->type == MONS_DANCING_WEAPON) ? " is poisoned!"
                                                                                                 : "'s weapon is poisoned!" );

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
                    attacker->speed_increment -= attacker->speed / 2;
                    break;

                case SPWPN_VORPAL:
                    specdam = 1 + (random2(damage_taken) / 2);
                    break;

                case SPWPN_VAMPIRICISM:
                    specdam = 0;        // note does no extra damage

                    if (you.is_undead)
                      break;
                    if (attacker->hit_points == attacker->max_hit_points || one_chance_in(5))
                      break;

                    heal_monster(attacker, 1 + random2(damage_taken), true);    // heh heh {dlb}

                    simple_monster_message(attacker, " is healed.");
                    break;

                case SPWPN_DISRUPTION:
                    if (attacker->type == MONS_PLAYER_GHOST)
                        break;
                    if (you.is_undead)
                    {
                        specdam = random2(damage_taken) + random2(damage_taken) + random2(damage_taken) + random2(damage_taken);

                        if ( specdam )
                        {
                            strcpy(info, "You are blasted by holy energy");

                            if ( specdam < 7 )
                              strcat(info, ".");
                            else if ( specdam < 15 )
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
                        specdam += 1 + random2avg(7,2);
                        break;
                    }
                    if (one_chance_in(3))
                    {
                        mpr("Your body is terribly warped!");
                        specdam += 3 + random2avg(24,2);
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

        }                       // end of special weapons

        damage_taken += specdam;

        if ( damage_taken > 0 )
          {
            ouch(damage_taken, monster_attacking, KILLED_BY_MONSTER);

            if ( you.religion == GOD_XOM && you.hp <= you.hp_max / 3 && one_chance_in(10) )
              Xom_acts(true, you.experience_level, false);
          }
    }                           // end of for runthru


    return;

}          // end monster_attack()




bool monsters_fight( int monster_attacking, int monster_attacked )
{

    struct monsters *attacker = &menv[monster_attacking];
    struct monsters *defender = &menv[monster_attacked];

    int damage_taken = 0;
    bool hit = false;
    int mmov_x = 0;
    char str_pass[80];
    bool water_attack = false;
    int specdam = 0;
    int hand_used = 0;
    bool sees = false;

    if ( attacker->type == MONS_GIANT_SPORE )
    {
        attacker->hit_points = -1;
        return false;
    }

    if ( monster_habitat(attacker->type) != DNGN_FLOOR && attacker->number == 1 && monster_habitat(defender->type) == DNGN_FLOOR )
      return false;

    if ( attacker->behavior == BEH_ENSLAVED )
    {
        if ( defender->behavior == BEH_ENSLAVED )
          return false;
        attacker->monster_foe = monster_attacked;
    }

    if ( defender->behavior == BEH_FIGHT )    // is this correct? see nested conditional! {dlb}
    {
        if ( defender->behavior != BEH_ENSLAVED && monster_attacked != attacker->monster_foe )
            return false;
    }

    if ( grd[attacker->x][attacker->y] == DNGN_SHALLOW_WATER
        && !mons_flies(attacker->type)
        && monster_habitat(attacker->type) == DNGN_FLOOR
        && one_chance_in(4) )
    {
        mpr("You hear a splashing noise.");
        return true;
    }

    if ( grd[defender->x][defender->y] == DNGN_SHALLOW_WATER && !mons_flies(defender->type) && monster_habitat(attacker->type) == DNGN_DEEP_WATER )
      water_attack = true;

    if ( mons_near(attacker) && mons_near(defender) )
      sees = true;

    char runthru;

    for (runthru = 0; runthru < 4; runthru++)
    {

        char mdam = mondamage(attacker->type, runthru);

        if ( attacker->type == MONS_ZOMBIE_SMALL
              || attacker->type == MONS_ZOMBIE_LARGE
              || attacker->type == MONS_SKELETON_SMALL
              || attacker->type == MONS_SKELETON_LARGE
              || attacker->type == MONS_SPECTRAL_THING ) // what do these things have in common? {dlb}
        {
            mdam = mondamage(attacker->number, runthru);
// cumulative reductions - series of if-conditions is necessary: {dlb}
            if ( mdam > 1 )
              mdam--;
            if ( mdam > 2 )
              mdam--;
            if ( mdam > 5 )
              mdam--;
            if ( mdam > 9 )
              mdam--;         // was: "-= 2" {dlb}
        }

        if (mdam == 0)
          break;

        if ( attacker->type == MONS_TWO_HEADED_OGRE
            && runthru == 1
            && attacker->inv[MSLOT_MISSILE] != NON_ITEM )
          hand_used = 1;

        damage_taken = 0;

        int mons_to_hit = 20 + attacker->hit_dice;        // * menv [monster_attacking].hit_dice; // * 3

        if ( water_attack )
          mons_to_hit += 5;

        if ( attacker->inv[hand_used] != NON_ITEM )
        {
            if ( mitm.pluses[attacker->inv[hand_used]] - 50 > 130 )
              mons_to_hit += mitm.pluses[attacker->inv[hand_used]] - 50 - 100;
            else
              mons_to_hit += mitm.pluses[attacker->inv[hand_used]] - 50;

            mons_to_hit += 3 * property(OBJ_WEAPONS, mitm.sub_type[attacker->inv[MSLOT_WEAPON]], PWPN_HIT);

            attacker->speed_increment -= ((property(you.inv_class[you.equip[EQ_WEAPON]], you.inv_type[you.equip[EQ_WEAPON]], PWPN_SPEED) - 10) / 2);      // / 10);
        }

        mons_to_hit = random2(mons_to_hit);

        if ( attacker->type == MONS_KILLER_BEE || attacker->type == MONS_WORM )
          attacker->speed_increment -= 10;      //--

        if (mons_to_hit >= defender->evasion
            || ((defender->speed_increment <= 60
                 || defender->behavior == BEH_SLEEP)
                && !one_chance_in(20)))
        {
            hit = true;

            if ( attacker->inv[hand_used] != NON_ITEM && mitm.base_type[attacker->inv[hand_used]] == OBJ_WEAPONS && !launches_things(mitm.sub_type[attacker->inv[hand_used]]) )
            {

                damage_taken = random2(property(mitm.base_type[attacker->inv[MSLOT_WEAPON]], mitm.sub_type[attacker->inv[MSLOT_WEAPON]], PWPN_DAMAGE));

                if ( mitm.special[attacker->inv[hand_used]] / 30 == DWPN_ORCISH
                    && mons_charclass(attacker->type) == MONS_ORC && coinflip() )
                  damage_taken++;

                //if (mitm.pluses[mons_inv[i][0]] > 80) damage_taken -= 100;
                //  damage_taken += mitm.pluses[mons_inv[i][0]];

                if ( mitm.pluses2[attacker->inv[hand_used]] > 130 )
                {
                    if ( mitm.pluses2[attacker->inv[hand_used]] - 150 >= 0 )        /* + or 0 to-dam */
                      damage_taken += random2(mitm.pluses2[attacker->inv[hand_used]] - 150 + 1);
                    else
                      damage_taken -= random2(abs(mitm.pluses2[attacker->inv[hand_used]] - 150 + 1));   /* - to-dam */
                }
                else
                {
                    if ( mitm.pluses2[attacker->inv[hand_used]] - 50 >= 0 )         /* + or 0 to-dam */
                      damage_taken += random2(mitm.pluses2[attacker->inv[hand_used]] - 50 + 1);
                    else
                      damage_taken -= random2(abs(mitm.pluses2[attacker->inv[hand_used]] - 50 + 1));    /* - to-dam */
                }

                damage_taken -= 1 + random2(3);         //1;
            }

            damage_taken += 1 + random2(mdam);

            if ( water_attack )
              damage_taken <<= 1;

            damage_taken -= random2(1 + defender->armor_class);

            if ( damage_taken < 1 )
              damage_taken = 0;
        }
        else
        {
            hit = false;

            int mmov_x = attacker->inv[hand_used];

            char str_pass[80];

            if ( sees )
            {
                if (attacker->type == MONS_DANCING_WEAPON)
                {
                    it_name(mmov_x, 0, str_pass);
                    strcpy(info, str_pass);
                }
                else
                  strcpy(info, monam(attacker->number, attacker->type, attacker->enchantment[2], 0));

                strcat(info, " misses ");
                strcat(info, monam(defender->number, defender->type, defender->enchantment[2], 1));
                strcat(info, ".");
                mpr(info);
            }
        }

        if ( damage_taken < 1 && hit )
        {
            int mmov_x = attacker->inv[hand_used];

            char str_pass[80];

            if ( sees )
            {
                if (attacker->type == MONS_DANCING_WEAPON)
                {
                    it_name(mmov_x, 0, str_pass);
                    strcpy(info, str_pass);
                }
                else
                    strcpy(info, monam(attacker->number, attacker->type, attacker->enchantment[2], 0));

                strcat(info, " hits ");
                strcat(info, monam(defender->number, defender->type, defender->enchantment[2], 1));
#ifdef WIZARD
                itoa(damage_taken, st_prn, 10);
                strcat(info, " for ");
                strcat(info, st_prn);       // note: doesn't take account of special weapons etc
#endif
                strcat(info, ".");  // but doesn't do any you.damage.");
                mpr(info);
            }
        }

        if ( hit )           //(int) damage_taken > 0)
        {
            int mmov_x = attacker->inv[hand_used];

            if ( sees )
            {
                if (attacker->type == MONS_DANCING_WEAPON)
                {
                    char str_pass[80];

                    it_name(mmov_x, 0, str_pass);
                    strcpy(info, str_pass);
                }
                else
                  strcpy(info, monam(attacker->number, attacker->type, attacker->enchantment[2], 0));

                strcat(info, " hits ");
                strcat(info, monam(defender->number, defender->type, defender->enchantment[2], 1));

                if ( attacker->type != MONS_DANCING_WEAPON && attacker->inv[hand_used] != NON_ITEM && mitm.base_type[attacker->inv[hand_used]] == OBJ_WEAPONS && !launches_things(mitm.sub_type[attacker->inv[hand_used]]) )
                {
                    strcat(info, " with ");
                    it_name(mmov_x, 3, str_pass);    // was 7
                    strcat(info, str_pass);
                }

                strcat(info, "! ");
                mpr(info);
            }

// special attacks:
            switch (attacker->type)
            {
            case MONS_CENTAUR:  // cockatrice             // enum does not match comment 14jan2000 {dlb}
            case MONS_JELLY:
            case MONS_GUARDIAN_NAGA:
                break;

            case MONS_GIANT_ANT:
            case MONS_WOLF_SPIDER:
            case MONS_REDBACK:
            case MONS_SPINY_WORM:
            case MONS_JELLYFISH:
            case MONS_ORANGE_DEMON:
                if (attacker->type == MONS_SPINY_WORM || one_chance_in(20) || (damage_taken > 3 && one_chance_in(4)))
                {
                    if ( sees )
                    {
                        strcpy(info, monam(attacker->number, attacker->type, attacker->enchantment[2], 0));
                        strcat(info, " stings ");
                        strcat(info, monam(defender->number, defender->type, defender->enchantment[2], 1));
                        strcat(info, ".");
                        mpr(info);
                    }
                    poison_monster(defender, false);
                }
                break;

            case MONS_KILLER_BEE:
            case MONS_BUMBLEBEE:
                if (one_chance_in(20) || (damage_taken > 2 && one_chance_in(3)))
                {
                    if ( sees )
                    {
                        strcpy(info, monam(attacker->number, attacker->type, attacker->enchantment[2], 0));
                        strcat(info, " stings ");
                        strcat(info, monam(defender->number, defender->type, defender->enchantment[2], 1));
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
                if ( mons_holiness(defender->type) == MH_UNDEAD || mons_holiness(defender->type) == MH_DEMONIC )
                  break;
                if ( one_chance_in(20) || ( damage_taken > 2 && one_chance_in(3) ) )
                {
                    defender->max_hit_points -= 1 + random2(3);

                    if ( defender->hit_points > defender->max_hit_points )
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
                if ( !mons_res_fire(defender->type) )
                {
                    specdam = 15 + random2(15);

                    if (defender->inv[MSLOT_ARMOUR] != NON_ITEM && mitm.special[defender->inv[MSLOT_ARMOUR]] % 30 == SPARM_FIRE_RESISTANCE)
                        specdam /= 3;
                }

                if ( mons_res_fire(defender->type) == -1 || ( defender->inv[MSLOT_ARMOUR] == NON_ITEM || mitm.special[defender->inv[MSLOT_ARMOUR]] % 30 != SPARM_FIRE_RESISTANCE ) )
                  specdam = random2(25) + 20;

                if ( specdam )
                  simple_monster_message(defender, " is engulfed in flame!");

                damage_taken += specdam;
                break;

            case MONS_QUEEN_BEE:
            case MONS_GIANT_CENTIPEDE:
            case MONS_SOLDIER_ANT:
            case MONS_QUEEN_ANT:
                //if ((damage_taken > 2 && one_chance_in(3) ) || one_chance_in(20) )
                //{
                if ( sees )
                {
                    strcpy(info, monam(attacker->number, attacker->type, attacker->enchantment[2], 0));
                    strcat(info, " stings ");
                    strcat(info, monam(defender->number, defender->type, defender->enchantment[2], 1));
                    strcat(info, ".");
                    mpr(info);
                }
                poison_monster(defender, false);
                //}
                break;

            case MONS_SCORPION: // snake     // enum does not match comment 14jan2000 {dlb}
            case MONS_BROWN_SNAKE:
            case MONS_BLACK_SNAKE:
            case MONS_YELLOW_SNAKE:
            case MONS_GOLD_MIMIC:
            case MONS_WEAPON_MIMIC:
            case MONS_ARMOUR_MIMIC:
            case MONS_SCROLL_MIMIC:
            case MONS_POTION_MIMIC:
                if (one_chance_in(20) || (damage_taken > 2 && one_chance_in(4)))
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
                if ( mons_holiness(defender->type) == MH_UNDEAD || mons_holiness(defender->type) == MH_DEMONIC )
                  break;
                if ( one_chance_in(30) || ( damage_taken > 5 && coinflip() ) )
                {
                    simple_monster_message(defender, " is drained.");

                    if ( one_chance_in(5) )
                      defender->hit_dice--;

                    defender->max_hit_points -= 2 + random2(3);
                    defender->hit_points -= 2 + random2(3);

                    if ( defender->hit_points >= defender->max_hit_points )
                      defender->hit_points = defender->max_hit_points;

                    if ( defender->hit_points < 1 || defender->hit_dice < 1 )
                    {
                        monster_die(defender, KILL_MON, monster_attacking);
                        return true;
                    }
                }
                break;

            case MONS_WORM:     // giant wasp     // enum does not match comment 14jan2000 {dlb}
                break;

            case MONS_ICE_DEVIL:
            case MONS_ICE_BEAST:
            case MONS_FREEZING_WRAITH:
            case MONS_ICE_FIEND:
            case MONS_WHITE_IMP:
            case MONS_AZURE_JELLY:
            case MONS_ANTAEUS:
                specdam = 0;
                if ( !mons_res_cold(defender->type) )
                {
                    specdam = attacker->hit_dice + random2(attacker->hit_dice << 1);
                    if (defender->inv[MSLOT_ARMOUR] != NON_ITEM && mitm.special[defender->inv[MSLOT_ARMOUR]] % 30 == SPARM_COLD_RESISTANCE )
                      specdam = (random2(attacker->hit_dice << 1) + (attacker->hit_dice) / 3);
                }

                if (mons_res_cold(defender->type) == -1 && (defender->inv[MSLOT_ARMOUR] == NON_ITEM || mitm.special[defender->inv[MSLOT_ARMOUR]] % 30 != SPARM_COLD_RESISTANCE))
                  specdam = random2(attacker->hit_dice * 3) + (attacker->hit_dice << 1);

                if ( specdam )
                {
                    if ( sees )
                    {
                        strcpy(info, monam(attacker->number, attacker->type, attacker->enchantment[2], 0));
                        strcat(info, " freezes ");
                        strcat(info, monam(defender->number, defender->type, defender->enchantment[2], 1));
                        strcat(info, ".");
                        mpr(info);
                    }
                }
                damage_taken += specdam;
                break;

            case MONS_VAMPIRE:
                if ( mons_holiness(defender->type) == MH_UNDEAD || mons_holiness(defender->type) == MH_DEMONIC )
                  break;

                simple_monster_message(attacker, " is healed.");

                heal_monster(attacker, random2(damage_taken), true);    // heh heh {dlb}
                break;
            }
        }

// special weapons:
        if ( hit && (attacker->inv[hand_used] != NON_ITEM || ((attacker->type == MONS_PLAYER_GHOST || attacker->type == MONS_PANDEMONIUM_DEMON) && ghost.values[8] != 0)))
        {
            unsigned char itdam;

            if ( attacker->type == MONS_PLAYER_GHOST || attacker->type == MONS_PANDEMONIUM_DEMON )
              itdam = ghost.values[8];
            else
              itdam = mitm.special[attacker->inv[hand_used]];

            specdam = 0;

            if ( itdam == NWPN_SWORD_OF_CEREBOV )
              goto flaming;

            if ( attacker->type == MONS_PLAYER_GHOST || attacker->type == MONS_PANDEMONIUM_DEMON || mitm.special[attacker->inv[hand_used]] < NWPN_SINGING_SWORD )
                switch (itdam % 30)
                {
                  case SPWPN_NORMAL:
                    break;

                case SPWPN_FLAMING:
                  flaming:specdam = 0;
                    if ( !mons_res_fire(defender->type) )
                    {
                        specdam = 1 + (random2(damage_taken));
                        if (defender->inv[MSLOT_ARMOUR] != NON_ITEM && mitm.special[defender->inv[MSLOT_ARMOUR]] % 30 == SPARM_FIRE_RESISTANCE)
                            specdam /= 3;
                    }
                    if (mons_res_fire(defender->type) == -1 && (defender->inv[MSLOT_ARMOUR] == NON_ITEM || mitm.special[defender->inv[MSLOT_ARMOUR]] % 30 != SPARM_FIRE_RESISTANCE))
                    {
                        specdam = 1 + random2(damage_taken);
                    }
                    if ( specdam )
                    {
                        if ( sees )
                        {
                            int mmov_x = attacker->inv[hand_used];

                            char str_pass[80];

                            if (attacker->type == MONS_DANCING_WEAPON)
                            {
                                it_name(mmov_x, 0, str_pass);
                                strcpy(info, str_pass);
                            }
                            else
                              strcpy(info, monam(attacker->number, attacker->type, attacker->enchantment[2], 0));

                            strcat(info, " burns ");
                            strcat(info, monam(defender->number, defender->type, defender->enchantment[2], 1));

                            if ( specdam < 3 )
                              strcat(info, ".");
                            else if ( specdam < 7 )
                              strcat(info, "!");
                            else
                              strcat(info, "!!");

                            mpr(info);
                        }
                    }
                    break;

                case SPWPN_FREEZING:
                    specdam = 0;
                    if ( !mons_res_cold(defender->type) )
                    {
                        specdam = 1 + (random2(damage_taken));
                        if (defender->inv[MSLOT_ARMOUR] != NON_ITEM && mitm.special[defender->inv[MSLOT_ARMOUR]] % 30 == SPARM_COLD_RESISTANCE)
                            specdam = (1 + (random2(damage_taken) / 2)) / 3;
                    }
                    if (mons_res_cold(defender->type) == -1 && (defender->inv[MSLOT_ARMOUR] == NON_ITEM || mitm.special[defender->inv[MSLOT_ARMOUR]] % 30 != SPARM_COLD_RESISTANCE))
                    {
                        specdam = 1 + random2(damage_taken);
                    }
                    if ( specdam )
                    {
                        if ( sees )
                        {
                            mmov_x = attacker->inv[hand_used];

                            if (attacker->type == MONS_DANCING_WEAPON)
                            {
                                it_name(mmov_x, 0, str_pass);
                                strcpy(info, str_pass);
                            }
                            else
                                strcpy(info, monam(attacker->number, attacker->type, attacker->enchantment[2], 0));

                            strcat(info, " freezes ");
                            strcat(info, monam(defender->number, defender->type, defender->enchantment[2], 1));

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
                        damage_taken -= 5 + random2(5);     // I would think that it would do zero damage {dlb}
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

                    if ( mons_flies(defender->type) || mons_res_elec(defender->type) )
                      break;

                    specdam = 0;

                    if (mitm.pluses2[attacker->inv[hand_used]] <= 50 || mitm.pluses2[attacker->inv[hand_used]] > 130 && mitm.pluses2[attacker->inv[hand_used]] <= 150)
                        break;

                    if ( one_chance_in(3) )
                    {
                        if ( sees )
                          mpr("There is a sudden explosion of sparks!");

                        specdam += 10 + random2(15);
                        //mitm.pluses2[attacker->inv[hand_used]] --;
                    }
                    break;

                case SPWPN_ORC_SLAYING:
                    if ( mons_charclass(defender->type) == MONS_ORC )
                      hurt_monster(defender, 1 + random2(damage_taken));
                    break;

                case SPWPN_VENOM:
                    if ( one_chance_in(3) )
                      poison_monster(defender, true);
                    break;

                //case 7: // protection

                case SPWPN_DRAINING:
                    if ( ( mons_holiness(defender->type) == MH_UNDEAD || mons_holiness(defender->type) == MH_DEMONIC ) && (one_chance_in(30) || (damage_taken > 5 && coinflip())))
                    {
                        simple_monster_message(defender, " is drained");

                        if ( one_chance_in(5) )
                          defender->hit_dice--;

                        defender->max_hit_points -= 2 + random2(3);
                        defender->hit_points -= 2 + random2(3);

                        if ( defender->hit_points >= defender->max_hit_points )
                          defender->hit_points = defender->max_hit_points;

                        if (defender->hit_points < 1  || defender->hit_dice < 1)
                        {
                            monster_die(defender, KILL_MON, monster_attacking);
                            return true;
                        }
                        specdam = 1 + (random2(damage_taken) / 2);
                    }
                    break;

                case SPWPN_SPEED:
                    attacker->speed_increment -= attacker->speed / 2;
                    break;

                case SPWPN_VORPAL:
                    specdam += 1 + (random2(damage_taken) / 2);
                    break;

                case SPWPN_VAMPIRICISM:
                    specdam = 0;        // note does no extra damage

                    if ( mons_holiness(defender->type) == MH_UNDEAD || mons_holiness(defender->type) == MH_DEMONIC )
                      break;
                    if (attacker->hit_points == attacker->max_hit_points || one_chance_in(5))
                      break;

                    heal_monster(attacker, 1 + random2(damage_taken), true);    // heh heh {dlb}

                    simple_monster_message(attacker, " is healed.");
                    break;

                case SPWPN_DISRUPTION:
                    if ( attacker->type == MONS_PLAYER_GHOST )
                      break;

                    specdam = 0;

                    if ( mons_holiness(defender->type) == MH_UNDEAD && !one_chance_in(3) )
                    {
                        simple_monster_message(defender, " shudders.");
                        specdam += random2avg(1 + (3 * damage_taken),3);
                    }
                    break;


                case SPWPN_DISTORTION:
                    if (one_chance_in(3))
                    {
                        if ( mons_near(defender) && ( defender->enchantment[2] != ENCH_INVIS || player_see_invis() ) )
                        {
                            strcpy(info, "Space bends around ");
                            strcat(info, monam(defender->number, defender->type, defender->enchantment[2], 1));
                            strcat(info, ".");
                            mpr(info);
                        }

                        specdam += 1 + random2avg(7,2);
                        break;
                    }
                    if (one_chance_in(3))
                    {
                        if ( mons_near(defender) && ( defender->enchantment[2] != ENCH_INVIS || player_see_invis() ) )
                        {
                            strcpy(info, "Space warps horribly around ");
                            strcat(info, monam(defender->number, defender->type, defender->enchantment[2], 1));
                            strcat(info, "!");
                            mpr(info);
                        }

                        specdam += 3 + random2avg(24,2);
                        break;
                    }
                    if ( one_chance_in(3) )
                    {
                        monster_blink(defender);
                        break;
                    }
                    if ( coinflip() )
                    {
                        monster_teleport(defender, coinflip());
                        break;
                    }
                    if ( coinflip() )
                    {
                        monster_die(defender, KILL_RESET, monster_attacking);
                        break;
                    }
                    break;

                }
        }                       // end of if special weapon

        damage_taken += specdam;

        if ( damage_taken > 0 )
        {
            hurt_monster(defender, damage_taken);

            if (defender->hit_points < 1)
            {
                monster_die(defender, KILL_MON, monster_attacking);
                return true;
            }
        }
    }                           // end of for runthru

    if (defender->behavior == BEH_ENSLAVED)
    {
        defender->monster_foe = monster_attacking;
        attacker->monster_foe = monster_attacked;
    }

    if ( attacker->behavior != BEH_CONFUSED )       // && attacker->behavior != 7) // the latter is so that mons_beh[attacker] doesn't get reset to 6
    {
        if ( defender->behavior <= BEH_WANDER
            && distance(you.x_pos, you.y_pos, defender->x, defender->y) > 2 )
        {
            defender->behavior = BEH_FIGHT;
            defender->monster_foe = monster_attacking;     // This'll do for now.
        }
    }

    return true;

}          // end monsters_fight()




void monster_die( struct monsters *monster, char killer, int i )
{

    int dmi;                                           // dead monster's inventory
    int monster_killed = monster_index(monster);

// From time to time Trog gives you a little bonus
    if ( killer == KILL_YOU
        && you.religion == GOD_TROG
        && you.berserker
        && ( !player_under_penance() && you.piety > random2(1000) ) )
    {
        int bonus = 3 + random2(3);

        you.berserker += bonus;
        you.might += bonus;
        you.haste += bonus;
        mpr("You feel the power of Trog in you as your rage grows.");
    }

    if ( you.prev_targ == monster_killed )
      you.prev_targ = MHITNOT;

    if ( monster->type == MONS_GIANT_SPORE )
    {
        if ( monster->hit_points < 1 && monster->hit_points > -15 )
          return;
    }
    else if ( monster->type == MONS_FIRE_VORTEX || monster->type == MONS_SPATIAL_VORTEX )
    {
        simple_monster_message(monster, " dissipates!");

        if ( YOU_KILL(killer)
            && ( monster->enchantment[1] < ENCH_FRIEND_ABJ_I
                  || monster->enchantment[1] > ENCH_FRIEND_ABJ_VI )
            && monster->enchantment[1] != ENCH_CREATED_FRIENDLY )
          gain_exp(exper_value(monster->type, monster->hit_dice, monster->max_hit_points));

        if ( monster->type == MONS_FIRE_VORTEX )
          place_cloud(CLOUD_FIRE_MON, monster->x, monster->y, 2 + random2(4));
    }
    else if ( monster->type == MONS_DANCING_WEAPON )
    {
        simple_monster_message(monster, " falls from the air.");

        if ( YOU_KILL(killer)
            && ( monster->enchantment[1] < ENCH_FRIEND_ABJ_I
                  || monster->enchantment[1] > ENCH_FRIEND_ABJ_VI )
            && monster->enchantment[1] != ENCH_CREATED_FRIENDLY )
          gain_exp(exper_value(monster->type, monster->hit_dice, monster->max_hit_points));
    }
    else
    {
        switch ( killer )
        {
            case KILL_YOU:              /* You kill in combat. */
            case KILL_YOU_MISSILE:      /* You kill by missile or beam. */
              strcpy(info, "You ");
              strcat(info, (wounded_damaged(monster->type)) ? "destroy" : "kill");
              strcat(info, " ");
              strcat(info, monam(monster->number, monster->type, monster->enchantment[2], 1));
              strcat(info, "!");
              mpr(info);

              if ( ( monster->enchantment[1] < ENCH_FRIEND_ABJ_I || monster->enchantment[1] > ENCH_FRIEND_ABJ_VI )
                  && monster->enchantment[1] != ENCH_CREATED_FRIENDLY )
                gain_exp(exper_value(monster->type, monster->hit_dice, monster->max_hit_points));
              else
                mpr("That felt strangely unrewarding.");

          // Xom doesn't care who you killed:
              if ( you.religion == GOD_XOM && random2(70) <= 10 + monster->hit_dice )
                Xom_acts(true, 1 + random2(monster->hit_dice), false);

        // Trying to prevent summoning abuse here, so we're trying to
        // prevent summoned creatures from being being done_good kills,
        // Only affects monsters friendly when created.
              if ( ( monster->enchantment[1] < ENCH_FRIEND_ABJ_I || monster->enchantment[1] > ENCH_FRIEND_ABJ_VI )
                  && monster->enchantment[1] != ENCH_CREATED_FRIENDLY )
              {
                  if ( you.duration[DUR_PRAYER] )
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
                  else if ( mons_holiness(monster->type) == MH_HOLY )
                  {
                      done_good(GOOD_KILLED_ANGEL_I, monster->hit_dice);
                  }
              }

              if (you.mutation[MUT_DEATH_STRENGTH] || (you.religion == GOD_MAKHLEB && you.duration[DUR_PRAYER] && (!player_under_penance() && random2(you.piety) >= 30)))
              {
                  if ( you.hp < you.hp_max )
                  {
                      mpr("You feel a little better.");
                      inc_hp(monster->hit_dice + random2(monster->hit_dice), false);
                  }
              }

              if ( ( you.religion == GOD_MAKHLEB || you.religion == GOD_VEHUMET )
                  && you.duration[DUR_PRAYER] && ( !player_under_penance() && random2(you.piety) >= 30 ) )
              {
                  if (you.magic_points < you.max_magic_points)
                  {
                      mpr("You feel your power returning.");
                      inc_mp(1 + random2(random2(monster->hit_dice)), false);
                  }
              }

              if ( you.duration[DUR_DEATH_CHANNEL] && mons_holiness(monster->type) == MH_NATURAL && mons_weight(mons_charclass(monster->type)) )
              {
                  if ( create_monster(MONS_SPECTRAL_THING, 0, BEH_ENSLAVED, monster->x, monster->y, you.pet_target, mons_charclass(monster->type)) != -1 )
                    mpr("A strange glowing mist starts to gather...");
              }
              break;

            case KILL_MON:              /* Monster kills in combat */
            case KILL_MON_MISSILE:      /* Monster kills by missile or beam */
              simple_monster_message(monster, " dies!");

              if ( monster->behavior == BEH_ENSLAVED )
                naughty(NAUGHTY_FRIEND_DIES, 1 + (monster->hit_dice / 2));

              if ( (i >= 0 && i < 200) && menv[i].behavior == BEH_ENSLAVED )
              {
              // Trying to prevent summoning abuse here, so we're trying to
              // prevent summoned creatures from being being done_good kills.
              // Only affects creatures which were friendly when summoned.
                  if ( ( monster->enchantment[1] < ENCH_FRIEND_ABJ_I || monster->enchantment[1] > ENCH_FRIEND_ABJ_VI )
                      && monster->enchantment[1] != ENCH_CREATED_FRIENDLY )
                  {
                      gain_exp(exper_value(monster->type, monster->hit_dice, monster->max_hit_points) / 2 + 1);

                      if (mons_holiness(menv[i].type) == MH_UNDEAD)
                      {
                          if (mons_holiness(monster->type) == MH_NATURAL)
                            done_good(GOOD_SLAVES_KILL_LIVING, monster->hit_dice);
                          else
                            done_good(GOOD_SERVANTS_KILL, monster->hit_dice);
                      }
                      else
                      {
                          done_good(GOOD_SERVANTS_KILL, monster->hit_dice);

                          if ( you.religion == GOD_VEHUMET && (!player_under_penance() && random2(you.piety) >= 20) )
                          {
                            /* Vehumet - only for non-undead servants (coding
                               convenience, no real reason except that Vehumet
                               prefers demons) */
                              if ( you.magic_points < you.max_magic_points )
                              {
                                  mpr("You feel your power returning.");
                                  inc_mp(1 + random2(random2(monster->hit_dice)), false);
                              }
                          }
                      }
                  }
              }
              break;

            case KILL_MISC:             /* Monster killed by trap/inanimate thing/itself/poison not from you */
              simple_monster_message(monster, " dies!");
              break;

            case KILL_RESET:            /* Monster doesn't die, just goes back to wherever it came from
                                           This must only be called by monsters running out of time (or abjuration),
                                           because it uses the beam variables! Or does it??? */
              simple_monster_message(monster, " disappears in a puff of smoke!");
              place_cloud(CLOUD_GREY_SMOKE_MON + random2(3), monster->x, monster->y, 1 + random2(3));

              for (dmi = MSLOT_GOLD; dmi >= MSLOT_WEAPON; dmi--)
              {                       /* takes whatever it's carrying back home */
                if ( monster->inv[dmi] != NON_ITEM )
                  destroy_item(monster->inv[dmi]);

                monster->inv[dmi] = NON_ITEM;
              }
              break;
        }
    }

    if ( monster->type == MONS_MUMMY )
    {
        if ( YOU_KILL(killer) )
          if ( curse_an_item(1, 0) )
            mpr("You feel nervous for a moment...");
    }
    else if ( monster->type == MONS_GUARDIAN_MUMMY
               || monster->type == MONS_GREATER_MUMMY
               || monster->type == MONS_MUMMY_PRIEST )
    {
        if ( YOU_KILL(killer) )
        {
            mpr("You feel extremely nervous for a moment...");
            miscast_effect(SPTYP_NECROMANCY, 3 + (monster->type == MONS_GREATER_MUMMY) * 8 + (monster->type == MONS_MUMMY_PRIEST) * 5, random2avg(88,3), 100);
        }
    }

    if ( killer != KILL_RESET )
    {
        if ( ( monster->enchantment[1] >= ENCH_ABJ_I && monster->enchantment[1] <= ENCH_ABJ_VI )
            || ( monster->enchantment[1] >= ENCH_FRIEND_ABJ_I && monster->enchantment[1] <= ENCH_FRIEND_ABJ_VI ) )
        {
            if ( mons_weight(mons_charclass(monster->type)) )
            {
                simple_monster_message(monster, "'s corpse disappears in a puff of smoke!");
                place_cloud(CLOUD_GREY_SMOKE_MON + random2(3), monster->x, monster->y, 1 + random2(3));
            }
        }
        else
          place_monster_corpse(monster);    // have to add case for disintegration effect here? {dlb}
    }

    monster_drop_ething(monster);

    for (unsigned char j = 0; j < 3; j++)
      monster->enchantment[j] = ENCH_NONE;

    monster->enchantment1 = 0;
    monster->type = -1;
    monster->hit_points = 0;
    monster->max_hit_points = 0;
    monster->hit_dice = 0;
    monster->armor_class = 0;
    monster->evasion = 0;

    mgrd[monster->x][monster->y] = NON_MONSTER;

    for (dmi = MSLOT_GOLD; dmi >= MSLOT_WEAPON; dmi--)
      monster->inv[dmi] = NON_ITEM;

    viewwindow(1, false);

    for (dmi = 0; dmi < MAX_MONSTERS; dmi++)
      if ( menv[dmi].monster_foe == monster_killed )
        menv[dmi].monster_foe = MHITNOT;

    if ( you.pet_target == monster_killed )
      you.pet_target = MHITNOT;

    return;

}          // end monster_die()




bool jelly_divide( struct monsters *parent )
{

    int jex = 0, jey = 0;                   // loop variables {dlb}
    bool foundSpot = false;                 // to rid code of hideous goto {dlb}
    struct monsters *child = 0;             // NULL - value determined with loop {dlb}

    if ( parent->type != MONS_JELLY || parent->hit_points == 1 )
      return false;

// first, find a suitable spot for the child {dlb}:
    for (jex = -1; jex < 3; jex++)
    {
    // loop moves beyond those tiles contiguous to parent {dlb}:
        if ( jex > 1 )
          return false;

        for (jey = -1; jey < 2; jey++)
        {
        // 10-50 for now - must take clouds into account:
            if ( mgrd[parent->x + jex][parent->y + jey] == NON_MONSTER
                && grd[parent->x + jex][parent->y + jey] > DNGN_LAST_SOLID_TILE
                && ( parent->x + jex != you.x_pos || parent->y + jey != you.y_pos ) )
            {
                foundSpot = true;
                break;
            }
        }

        if ( foundSpot )
          break;
    }                           /* end of for jex */

    int k = 0;    // must remain outside loop that follows {dlb}

// now that we have a spot, find a monster slot {dlb}:
    for (k = 0; k < MAX_MONSTERS; k++)
    {
        child = &menv[k];

        if ( child->type == -1 )
          break;
        else if ( k == MAX_MONSTERS - 1 )
          return false;
    }

// handle impact of split on parent {dlb}:
    parent->max_hit_points /= 2;

    if ( parent->hit_points > parent->max_hit_points )
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
    child->behavior = parent->behavior;      /* Look at this! */

    child->x = parent->x + jex;
    child->y = parent->y + jey;

    mgrd[child->x][child->y] = k;

    if ( !simple_monster_message(parent, " splits in two!") )
      if ( !silenced(parent->x, parent->y) || !silenced(child->x, child->y) )
        mpr("You hear a squelching noise.");

    return true;

}          // end jelly_divde()




// if you're invis and throw/zap whatever, alerts menv to your position
void alert( void )
{

    struct monsters *monster = 0;    // NULL {dlb}

    for (int it = 0; it < MAX_MONSTERS; it++)
    {
        monster = &menv[it];

        if ( monster->type != -1 && mons_near(monster) )
          if ( monster->behavior == BEH_CHASING_I || monster->behavior == BEH_WANDER || monster->behavior == BEH_SLEEP )
          {
              monster->behavior = BEH_CHASING_I;

              if ( !you.invis || mons_see_invis(monster->type) )
              {
                  monster->target_x = you.x_pos;
                  monster->target_y = you.y_pos;
              }
          }
    }

}          // end alert()




// note that power is (as of yet) unused within this function -
// may be worthy of consideration of later implementation, though,
// so I'll still let the parameter exist for the time being {dlb}
bool monster_polymorph( struct monsters *monster, int targetc, int power )
{
  char str_polymon[200]; // prevents segfaults -- cannot use info[] here {dlb}

  if ( targetc == RANDOM_MONSTER )
    {
    loopy:
      do
        {
          targetc = random2(NUM_MONSTERS);     // was: random2(400) {dlb}
        }
        while ( mons_rarity(targetc) == 0 // not the best possible check routine? {dlb}
                 || targetc == monster->type         // added this {dlb}
                 || targetc == MONS_SHAPESHIFTER
                 || targetc == MONS_GLOWING_SHAPESHIFTER
                 || targetc == MONS_ZOMBIE_SMALL
                 || targetc == MONS_ZOMBIE_LARGE
                 || targetc == MONS_SPECTRAL_THING
                 || targetc == MONS_SKELETON_SMALL
                 || targetc == MONS_SKELETON_LARGE
                 || targetc == MONS_DANCING_WEAPON
                 || mons_flag(targetc, M_NO_EXP_GAIN) );

        if ( grd[monster->x][monster->y] == DNGN_LAVA
             || grd[monster->x][monster->y] == DNGN_DEEP_WATER )
          if ( !mons_flies(targetc) )
            goto loopy;     /* Not fair to instakill a monster like this (actually, I can't be bothered implementing it) */
        /* Too long to put in the loop thing */
    }

    int old_class = monster->type;
    int old_hp = monster->hit_points;
    int old_hp_max = monster->max_hit_points;
    unsigned char old_sec = monster->number;

    /* deal with mons_sec */

    monster->type = targetc;
    monster->number = 250;

    for (unsigned char unenc = 0; unenc < 3; unenc++)
    {
      if ( monster->enchantment[unenc] >= ENCH_ABJ_I
          && monster->enchantment[unenc] <= ENCH_ABJ_VI )
        continue; // Summoned creatures are still going to disappear eventually
        if ( monster->enchantment[unenc] != ENCH_SHAPESHIFTER
             && monster->enchantment[unenc] != ENCH_GLOWING_SHAPESHIFTER )
          monster->enchantment[unenc] = 0; /* shapeshifters stay as such */
    }

    if ( mons_flag(monster->type, M_INVIS) )
    {
        monster->enchantment1 = 1;
        monster->enchantment[2] = ENCH_INVIS;
    }

    if ( monster->enchantment[0] == ENCH_NONE
        && monster->enchantment[1] == ENCH_NONE
        && monster->enchantment[2] == ENCH_NONE )
      monster->enchantment1 = 0;

    define_monster(monster_index(monster), menv);

    monster->hit_points = monster->max_hit_points * ((old_hp * 100) / old_hp_max) / 100 + random2(monster->max_hit_points);

    if ( monster->hit_points > monster->max_hit_points )
      monster->hit_points = monster->max_hit_points;

    monster->speed_increment = 70 + random2(5);

/* make sure that eg manticores don't drop their darts */

    monster_drop_ething(monster);

// messaging: {dlb}
    strcpy(str_polymon, monam(old_sec, old_class, monster->enchantment[2], 0));

    strcat(str_polymon, (monster->enchantment[1] == ENCH_SHAPESHIFTER
                          || monster->enchantment[1] == ENCH_GLOWING_SHAPESHIFTER) ? " changes into " :
                        (targetc == MONS_PULSATING_LUMP)                           ? " degenerates into "
                                                                                   : " evaporates and reforms as " );

    strcat(str_polymon, monam(monster->number, monster->type, monster->enchantment[2], 3));

    if ( targetc == MONS_PULSATING_LUMP )
      strcat(str_polymon, " of flesh");

    strcat(str_polymon, "!");

    return ( simple_monster_message(monster, str_polymon) );

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
static int weapon_type_modify( int weapnum, char *noise, char *noise2, int damage )
{

    int weap_type = WPN_UNKNOWN;

    if (damage >= HIT_WEAK)
    {
        if ( weapnum == -1 )
          weap_type = WPN_UNARMED;
        else
        {
            if ( you.inv_class[weapnum] == OBJ_STAVES )
              weap_type = WPN_QUARTERSTAFF;
            else if ( you.inv_class[weapnum] == OBJ_WEAPONS )
              weap_type = you.inv_type[weapnum];
        }
    }

    strcpy(noise2, "");

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
            strcpy(noise, "slice");
        else if (damage < HIT_STRONG)
            strcpy(noise, "slash");
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
            return damage;
        }
        else
        {
            if (damage < HIT_MED)
                strcpy(noise, "punch");
            else
                strcpy(noise, "pummel");
            return damage;
        }
    case WPN_UNKNOWN:
    default:
        strcpy(noise, "hit");
        return damage;
    }

}          // end weapon_type_modify()




static void monster_drop_ething( struct monsters *monster )
{
/* drop weapons & missiles last (ie on top) so others pick up */

    int loopy;                // loop variable {dlb}
    bool splashes = false;

    if ( grd[monster->x][monster->y] == DNGN_LAVA
          || grd[monster->x][monster->y] == DNGN_DEEP_WATER )
    {
        for (loopy = 0; loopy < NUM_MONSTER_SLOTS; loopy++)
        {
            if ( monster->inv[loopy] != NON_ITEM )
            {
                destroy_item(monster->inv[loopy]);
                splashes = true;
            }
        }

        if ( splashes )
          mpr("You hear a splashing sound.");    // I don't think things dropped into lava "splash" {dlb}

        return;
    }

    for (loopy = MSLOT_GOLD; loopy >= MSLOT_WEAPON; loopy--)
    {
        if ( monster->inv[loopy] != NON_ITEM )
        {
            mitm.link[monster->inv[loopy]] = NON_ITEM;

            if ( igrd[monster->x][monster->y] == NON_ITEM )
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

}          // end monster_drop_ething()




static void place_monster_corpse( struct monsters *monster )
{

    int corpse_class = ( (monster->enchantment[1] == ENCH_SHAPESHIFTER)         ? MONS_SHAPESHIFTER :
                         (monster->enchantment[1] == ENCH_GLOWING_SHAPESHIFTER) ? MONS_GLOWING_SHAPESHIFTER
                                                                                : mons_charclass(monster->type) );

    if ( !mons_weight(corpse_class)
          || grd[monster->x][monster->y] == DNGN_LAVA
          || grd[monster->x][monster->y] == DNGN_DEEP_WATER
          || coinflip() )
      return;

    int o;

    for (o = 0; o < MAX_ITEMS; o++)
    {
        if ( o >= 200 + random2(150) )    // seems to me that this should be translated to a probability calculation outside the loop (why keep calling random2() with each iteration?) {dlb}
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
            if ( mitm.colour[o] == BLACK )
              mitm.colour[o] = monster->number;
            mitm.quantity[o] = 1;
            mitm.link[o] = NON_ITEM;
            break;
        }
    }

    if ( igrd[monster->x][monster->y] == NON_ITEM )
      igrd[monster->x][monster->y] = o;
    else
    {
        int hug = igrd[monster->x][monster->y];

        igrd[monster->x][monster->y] = o;
        mitm.link[o] = hug;
    }

    return;

}          // end place_monster_corpse()

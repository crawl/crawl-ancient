/*
 *  File:       fight.cc
 *  Summary:    Functions used during combat.
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 *      <6>      9/09/99        BWR             Code for 1-1/2 hand weapons
 *      <5>      8/08/99        BWR             Reduced power of EV/shields
 *      <4>      6/22/99        BWR             Changes to stabbing code, made
 *                                              most gods not care about the
 *                                              deathes of summoned monsters
 *      <3>      5/21/99        BWR             Upped learning of armour skill
 *                                              in combat slightly.
 *      <2>      5/12/99        BWR             Fixed a bug where burdened
 *                                              barehanded attacks where free
 *
 *      <1>      -/--/--        LRH             Created
 */

#include "AppHdr.h"
#include "fight.h"

#ifdef DOS
#include <conio.h>
#endif /* DOS */

#include <string.h>
#include <stdlib.h>

#include "externs.h"
#include "enum.h"

#include "beam.h"
#include "debug.h"
#include "dungeon.h"
#include "effects.h"
#include "it_use2.h"
#include "it_use3.h"
#include "item_use.h"
#include "itemname.h"
#include "items.h"
#include "message.h"
#include "misc.h"
#include "monplace.h"
#include "monstuff.h"
#include "mons_lev.h"
#include "mstruct.h"
#include "mstuff2.h"
#include "mutation.h"
#include "ouch.h"
#include "output.h"
#include "player.h"
#include "abyss.h"
#include "randart.h"
#include "religion.h"
#include "shopping.h"
#include "skills.h"
#include "skills2.h"
#include "spell.h"
#include "spells.h"
#include "spells0.h"
#include "spells1.h"
#include "spells2.h"
#include "spells3.h"
#include "stuff.h"
#include "view.h"

#ifdef MACROS
  #include "macro.h"
#endif

#define HIT_WEAK 7
#define HIT_MED 18
#define HIT_STRONG 36
// ... was 5, 12, 21

int jelly_divide(int jel);
void place_monster_corpse(unsigned char mcr);
extern char wield_change;       /* defined in output.cc */
int weapon_type_modify(int weap, char *st_prn, char *noise2, int damage);


void you_attack(int monster_attacked, bool unarmed_attacks )
{
    int your_to_hit;
    int damage_done = 0;
    char hit = 0;
    char stab_bonus = 0;

//int mmov_x = 0;
    char str_pass[80],damage_noise[20],damage_noise2[20];
    int special_brand = 0;

    int heavy_armour = 0;

    if (you.equip[EQ_SHIELD] != -1)
    {
        if (you.inv_type[you.equip[EQ_SHIELD]] == ARM_SHIELD && you.skills[SK_SHIELDS] < random2(7))
            heavy_armour++;
        if (you.inv_type[you.equip[EQ_SHIELD]] == ARM_LARGE_SHIELD && you.skills[SK_SHIELDS] < random2(13))
            heavy_armour++;
        if (you.inv_type[you.equip[EQ_SHIELD]] == ARM_LARGE_SHIELD && you.skills[SK_SHIELDS] < random2(13))
            heavy_armour++;
        if (you.inv_type[you.equip[EQ_SHIELD]] == ARM_LARGE_SHIELD && you.skills[SK_SHIELDS] < random2(13))
            heavy_armour += random2(4);
    }
    if (you.equip[EQ_BODY_ARMOUR] != -1)
    {
        if (property(OBJ_ARMOUR, you.inv_type[you.equip[EQ_BODY_ARMOUR]], PARM_EVASION) < 0 && random2(you.skills[SK_ARMOUR]) < abs(property(OBJ_ARMOUR, you.inv_type[you.equip[EQ_BODY_ARMOUR]], PARM_EVASION)))
            heavy_armour += random2(abs(property(OBJ_ARMOUR, you.inv_type[you.equip[EQ_BODY_ARMOUR]], PARM_EVASION)));
    }

    if (you.equip[EQ_WEAPON] == -1)
        heavy_armour *= 2 + random2(2);

    // We calculate these up here so that we know about what we'll be doing
    // this combat turn.
    bool can_do_unarmed_combat = false;
    bool use_hand_and_a_half_bonus = false;

    if (you.burden_state == 0
                    && random2(20) < you.skills[SK_UNARMED_COMBAT]
                    && random2(heavy_armour + 1) <= 1)
    {
        can_do_unarmed_combat = true;
    }

    // if we're not getting potential unarmed attacks, and not wearing a
    // shield, and have a suitable uncursed weapon we get the bonus.
    if (unarmed_attacks && !can_do_unarmed_combat
            && you.equip[EQ_SHIELD] == -1
            && you.equip[EQ_WEAPON] != -1
            && you.inv_plus[you.equip[EQ_WEAPON]] < 130    // is uncursed
            && hands_required_for_weapon( you.inv_class[you.equip[EQ_WEAPON]],
                                you.inv_type[you.equip[EQ_WEAPON]] )
                                            == HANDS_ONE_OR_TWO_HANDED)
    {
        // this is currently a +1 dam, +1 to hit, -1 speed bonus
        use_hand_and_a_half_bonus = true;
    }

/*
   IMPORTANT: If damage routines are changed, must also change in ouch.cc
   for saving of player ghosts.

 */
    bool helpless = mons_flag(menv[monster_attacked].type, M_NO_EXP_GAIN);

    if (menv[monster_attacked].behavior != BEH_ENSLAVED)
        you.pet_target = monster_attacked;
    if (menv[monster_attacked].behavior == BEH_ENSLAVED)
    {
        menv[monster_attacked].behavior = BEH_CHASING_I;
        naughty(NAUGHTY_ATTACK_FRIEND, 5);
    }
    if (grd[you.x_pos][you.y_pos] == 65 && you.levitation == 0)
    {
        if (random2(you.dex) <= 3 || random2(5) == 0)
        {
            mpr("Unstable footing causes you to fumble your attack.");
            return;
        }
    }
    your_to_hit = 15 + you.dex / 2;     // + (0.5 * you.strength);// + (you.f_abil / 10); // / 100

    if (wearing_amulet(AMU_INACCURACY) == 1)
        your_to_hit -= 5;

    your_to_hit += random2(you.skills[SK_FIGHTING] + 1);

    if (you.equip[EQ_WEAPON] != -1)
    {

        if (weapon_skill(you.inv_class[you.equip[EQ_WEAPON]], you.inv_type[you.equip[EQ_WEAPON]]) != 0)
            your_to_hit += random2(you.skills[weapon_skill(you.inv_class[you.equip[EQ_WEAPON]], you.inv_type[you.equip[EQ_WEAPON]])] + 1);
    }
    else
    {                           /* Unarmed... */
        if (you.species == SP_TROLL || you.species == SP_GHOUL)
            your_to_hit += 2;
        your_to_hit += 2 + random2(you.skills[SK_UNARMED_COMBAT] + 1);
    }



    if (you.is_undead != 2)
        you.hunger -= 3;

    if (you.equip[EQ_WEAPON] != -1
                && you.inv_class[you.equip[EQ_WEAPON]] == OBJ_WEAPONS
                && (you.inv_dam[you.equip[EQ_WEAPON]] % 30 == SPWPN_SPEED
                || (you.inv_dam[you.equip[EQ_WEAPON]] % 30 >= SPWPN_RANDART_I
                && randart_wpn_properties(you.inv_class[you.equip[EQ_WEAPON]],
                                    you.inv_type[you.equip[EQ_WEAPON]],
                                    you.inv_dam[you.equip[EQ_WEAPON]],
                                    you.inv_plus[you.equip[EQ_WEAPON]],
                                    you.inv_plus2[you.equip[EQ_WEAPON]],
                                                0, RAP_BRAND) == SPWPN_SPEED)))
    {
        you.time_taken *= 5;
        you.time_taken /= 10;
    }                           /* weapon of speed */

    if (you.equip[EQ_WEAPON] != -1
                && you.inv_class[you.equip[EQ_WEAPON]] == OBJ_WEAPONS
                && you.inv_dam[you.equip[EQ_WEAPON]] % 30 >= SPWPN_RANDART_I)
    {
        if (randart_wpn_properties(you.inv_class[you.equip[EQ_WEAPON]], you.inv_type[you.equip[EQ_WEAPON]], you.inv_dam[you.equip[EQ_WEAPON]], you.inv_plus[you.equip[EQ_WEAPON]], you.inv_plus2[you.equip[EQ_WEAPON]], 0, RAP_ANGRY) >= 1)
        {
            if (random2(1 + randart_wpn_properties(you.inv_class[you.equip[EQ_WEAPON]], you.inv_type[you.equip[EQ_WEAPON]], you.inv_dam[you.equip[EQ_WEAPON]], you.inv_plus[you.equip[EQ_WEAPON]], you.inv_plus2[you.equip[EQ_WEAPON]], 0, RAP_ANGRY)))
                go_berserk();
        }
    }

    if (you.special_wield != SPWLD_NONE)
    {

        switch (you.special_wield)
        {
        case SPWLD_TROG:
            if (random2(2) == 0)
                go_berserk();
            break;

        case SPWLD_WUCAD_MU:
            if (random2(9) == 0)
                miscast_effect(19, random2(9), random2(70), 100);
            break;

        }
    }
    if (you.mutation[MUT_BERSERK] != 0)
    {
        if (random2(100) < (you.mutation[MUT_BERSERK] * 10) - 5)
            go_berserk();
    }
    if (you.equip[EQ_WEAPON] != -1)
    {
        if (you.inv_class[you.equip[EQ_WEAPON]] == OBJ_WEAPONS)
        {
            int ghoggl = you.inv_plus[you.equip[EQ_WEAPON]] - 50;

            if (you.inv_plus[you.equip[EQ_WEAPON]] > 130)
                ghoggl -= 100;

            if (ghoggl >= 0)
                your_to_hit += ghoggl;
            else
                your_to_hit += ghoggl;

            your_to_hit += property(OBJ_WEAPONS, you.inv_type[you.equip[EQ_WEAPON]], PWPN_HIT);

            if (you.inv_dam[you.equip[EQ_WEAPON]] / 30 == DWPN_ELVEN && you.species >= SP_ELF && you.species <= SP_SLUDGE_ELF)
            {
                your_to_hit += 1 + random2(2);
            }
        }
        if (you.inv_class[you.equip[EQ_WEAPON]] == OBJ_STAVES)
        {
            your_to_hit += property(OBJ_WEAPONS, WPN_QUARTERSTAFF, PWPN_HIT);   /* magical staff */
        }
    }
    your_to_hit += slaying_bonus(0);    /* in player.cc */

    if (you.hunger_state == 1)
    {
        your_to_hit -= 3;
    }


    your_to_hit -= heavy_armour;

    your_to_hit = random2(your_to_hit);

    if (use_hand_and_a_half_bonus)
        your_to_hit += random2(3);

    int damage = 1;


    if (you.equip[EQ_WEAPON] == -1)
    {                           /* empty hands */
        damage = 3;
        if (you.mutation[MUT_DRAIN_LIFE] != 0)
            special_brand = SPWPN_DRAINING;     /* demonic power */

        if (you.attribute[ATTR_TRANSFORMATION] != TRAN_NONE)
        {
            switch (you.attribute[ATTR_TRANSFORMATION])
            {
            case TRAN_SPIDER:
                damage = 9;
                special_brand = 6;
                your_to_hit += random2(10);
                break;          /* spider */
            case TRAN_BLADE_HANDS:
                damage = 12 + (you.strength / 4) + (you.dex / 4);
                your_to_hit += random2(12);
                break;          /* blade */
            case TRAN_STATUE:
                damage = 12 + you.strength;
                your_to_hit += random2(9);
                break;          /* statue */
            case TRAN_ICE_BEAST:
                damage = 10;
                special_brand = 2;
                your_to_hit += random2(10);
                break;          /* ice */
            case TRAN_DRAGON:
                damage = 20 + you.strength;
                your_to_hit += random2(10);
                break;          /* dragon */
            case TRAN_LICH:
                damage = 5;
                special_brand = SPWPN_DRAINING;
                your_to_hit += random2(10);
                break;          /* necromutation */
            }
        }
        else
        {
            if (you.species == SP_TROLL)
            {                   /* troll */
                damage += 5;
            }
            if (you.species == SP_GHOUL)
            {                   /* ghoul */
                damage += 2;
            }
        }
        damage += you.skills[SK_UNARMED_COMBAT];
    }

    if (you.equip[EQ_WEAPON] != -1)
    {
        if (you.inv_class[you.equip[EQ_WEAPON]] == OBJ_WEAPONS)
        {
            damage = property(you.inv_class[you.equip[EQ_WEAPON]], you.inv_type[you.equip[EQ_WEAPON]], PWPN_DAMAGE);
        }
        if (you.inv_class[you.equip[EQ_WEAPON]] == OBJ_STAVES)
        {
            damage = property(OBJ_WEAPONS, WPN_QUARTERSTAFF, PWPN_DAMAGE);
        }
    }

    int weapon_speed2 = 10;

    if (you.equip[EQ_WEAPON] != -1)
    {
        if (you.inv_class[you.equip[EQ_WEAPON]] == OBJ_WEAPONS || you.inv_class[you.equip[EQ_WEAPON]] == OBJ_STAVES)
        {
            if (you.inv_class[you.equip[EQ_WEAPON]] == OBJ_STAVES)
                weapon_speed2 = property(OBJ_WEAPONS, WPN_QUARTERSTAFF, PWPN_SPEED);
            else
                weapon_speed2 = property(you.inv_class[you.equip[EQ_WEAPON]], you.inv_type[you.equip[EQ_WEAPON]], PWPN_SPEED);

            weapon_speed2 -= you.skills[weapon_skill(you.inv_class[you.equip[EQ_WEAPON]], you.inv_type[you.equip[EQ_WEAPON]])] / 2;

            int min_speed = 10;

            if (you.inv_class[you.equip[EQ_WEAPON]] == OBJ_STAVES)
            {
                min_speed = property(OBJ_WEAPONS, WPN_QUARTERSTAFF,
                                                        PWPN_SPEED) / 2;
            }
            else
            {
                min_speed = property(you.inv_class[you.equip[EQ_WEAPON]],
                    you.inv_type[you.equip[EQ_WEAPON]], PWPN_SPEED) / 2;
            }

            // Using both hands can get a weapon up to speed 7
            if ((hands_required_for_weapon(
                        you.inv_class[you.equip[EQ_WEAPON]],
                        you.inv_type[you.equip[EQ_WEAPON]]) == HANDS_TWO_HANDED
                    || use_hand_and_a_half_bonus)
                && min_speed > 7)
            {
                min_speed = 7;
            }


            if (weapon_speed2 < min_speed)
                weapon_speed2 = min_speed;

            if (you.equip[EQ_SHIELD] != -1)
            {
                switch (you.inv_type[you.equip[EQ_SHIELD]])
                {
                case ARM_SHIELD:
                    weapon_speed2 += 1;
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
        if (you.burden_state == 0 && random2(heavy_armour + 1) == 0)
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

    ASSERT(weapon_speed2 > 0);
    you.time_taken *= weapon_speed2;
    you.time_taken /= 10;

    if (you.time_taken <= 0)
        you.time_taken = 1;

#ifdef WIZARD
    sprintf(info, "Time taken: %d", you.time_taken );
    mpr(info);
#endif


    if (menv[monster_attacked].speed_increment <= 40

                || ((menv[monster_attacked].behavior == BEH_FLEE
        //              || menv[monster_attacked].behavior == BEH_WANDER
                        || menv[monster_attacked].behavior == BEH_CONFUSED)
                    && random2(200) <= you.skills[SK_STABBING] + you.dex)

                || menv[monster_attacked].behavior == BEH_SLEEP)
    {
        switch (menv[monster_attacked].behavior) {
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

        strcpy(info, monam(menv[monster_attacked].number,
                        menv[monster_attacked].type,
                        menv[monster_attacked].enchantment[2], 0));
        strcat(info, " fails to defend itself.");
        mpr(info);

        exercise(SK_STABBING, 1 + random2(2) + random2(2)
                                                + random2(2) + random2(2));

        if (mons_holiness(menv[monster_attacked].type) != MH_UNDEAD
                && mons_holiness(menv[monster_attacked].type) != MH_DEMONIC)
            naughty(NAUGHTY_STABBING, 4);  // Servants of TSO must fight fair
    }
    else
    {
        alert();
    }


    if ((your_to_hit >= menv[monster_attacked].evasion || random2(15) == 0) || ((menv[monster_attacked].speed_increment <= 60 || menv[monster_attacked].behavior == BEH_SLEEP) && random2(10 + you.skills[SK_STABBING]) != 0))
    {
        hit = 1;
        int dammod = 78;

        if (you.strength > 11)
            dammod += (random2(you.strength - 11) * 2);
        else if (you.strength < 9)
            dammod -= (random2(9 - you.strength) * 3);

        damage *= dammod;       //random2(you.strength);

        damage /= 78;

        damage_done = (int) random2(damage);


        if (you.equip[EQ_WEAPON] != -1 && you.inv_class[you.equip[EQ_WEAPON]] == OBJ_WEAPONS || you.inv_class[you.equip[EQ_WEAPON]] == OBJ_STAVES)
        {
            damage_done *= 25 + (random2(you.skills[weapon_skill(you.inv_class[you.equip[EQ_WEAPON]], you.inv_type[you.equip[EQ_WEAPON]])] + 1));
            damage_done /= 25;
        }
        damage_done *= 30 + (random2(you.skills[SK_FIGHTING] + 1));
        damage_done /= 30;

        if (you.might > 1)
            damage_done += random2(10) + 1;
//        if (you.berserker > 1) damage_done += random2(10) + 1; /* remember these two are cumulative (not any more) */
        if (you.hunger_state == 1)
            damage_done -= random2(5);
        if (you.equip[EQ_WEAPON] != -1 && you.inv_class[you.equip[EQ_WEAPON]] == OBJ_WEAPONS)
        {
            int hoggl = you.inv_plus2[you.equip[EQ_WEAPON]] - 50;

            if (hoggl >= 0)
            {
                damage_done += random2(hoggl + 1);
            }
            else
                damage_done += hoggl;

            if (use_hand_and_a_half_bonus
                    || hands_required_for_weapon(
                        you.inv_class[you.equip[EQ_WEAPON]],
                        you.inv_type[you.equip[EQ_WEAPON]]) == HANDS_TWO_HANDED)
            {
                damage_done += random2(3);
            }

            if (you.inv_dam[you.equip[EQ_WEAPON]] / 30 == DWPN_DWARVEN
                        && you.species >= SP_HILL_DWARF
                                && you.species <= SP_MOUNTAIN_DWARF)
            {
                damage_done += random2(3);
            }

            if (you.inv_dam[you.equip[EQ_WEAPON]] / 30 == DWPN_ORCISH
                        && you.species == SP_HILL_ORC)
            {
                damage_done += random2(2);
            }

            if (you.inv_ident[you.equip[EQ_WEAPON]] < 3 && random2(100) < you.skills[weapon_skill(you.inv_class[you.equip[EQ_WEAPON]], you.inv_type[you.equip[EQ_WEAPON]])])
            {
                you.inv_ident[you.equip[EQ_WEAPON]] = 3;
                strcpy(info, "You are wielding ");
                in_name(you.equip[EQ_WEAPON], 3, str_pass);
                strcat(info, str_pass);
                strcat(info, ".");
                mpr(info);
                more();
                wield_change = 1;
            }
        }
//  else (damage_done -= random2(5));

        damage_done += slaying_bonus(1);

        if (stab_bonus > 0)
        {
            int skill_type = weapon_skill(you.inv_class[you.equip[EQ_WEAPON]],
                                          you.inv_type[you.equip[EQ_WEAPON]]);

            if (menv[monster_attacked].behavior == BEH_SLEEP)
            {
                // Sleeping moster wakes up when stabbed but may be groggy
                menv[monster_attacked].behavior = BEH_CHASING_I;

                if (random2(200) <= you.skills[SK_STABBING] + you.dex)
                {
                    int stun = random2( you.dex + 1 );
                    if (menv[monster_attacked].speed_increment > stun)
                        menv[monster_attacked].speed_increment -= stun;
                    else
                        menv[monster_attacked].speed_increment = 0;
                }
            }

            switch (skill_type) {
            case SK_SHORT_BLADES:
                if (you.inv_type[you.equip[EQ_WEAPON]] == WPN_DAGGER)
                {
                    int bonus = you.dex / 3;
                    if (bonus > you.skills[SK_STABBING] + 1)
                        bonus = you.skills[SK_STABBING] + 1;

                    damage_done += bonus;
                }
                // fall through

            case SK_LONG_SWORDS:
                damage_done *= 10 + you.skills[SK_STABBING] /
                        (stab_bonus + (skill_type == SK_SHORT_BLADES ? 0 : 1));
                damage_done /= 10;
                // fall through

            default:
                damage_done *= 12 + you.skills[SK_STABBING] / stab_bonus;
                damage_done /= 12;
            }
        }

        if (menv[monster_attacked].armor_class > 0) {
            damage_done -= random2(menv[monster_attacked].armor_class + 1);
        }

        damage_done = weapon_type_modify(you.equip[EQ_WEAPON],damage_noise,
                                                 damage_noise2, damage_done);
        if (damage_done < 0)
            damage_done = 0;
        menv[monster_attacked].hit_points -= damage_done;

        if (damage_done >= 1)
        {
            int  skill = 0;
            if (you.equip[EQ_WEAPON] != -1
                && (skill = weapon_skill(you.inv_class[you.equip[EQ_WEAPON]],
                                    you.inv_type[you.equip[EQ_WEAPON]])) != 0)
            {
                if (!helpless || you.skills[skill] < 2)
                {
                    exercise( skill, 1 );
                }
            }
            else
            {
                if (!helpless || you.skills[SK_UNARMED_COMBAT] < 2)
                {
                    exercise(SK_UNARMED_COMBAT, 1);
                }
            }

            if (random2(3) == 0 && (!helpless || you.skills[SK_FIGHTING] < 2))
            {
                exercise(SK_FIGHTING, 1);
            }
        }
        if (menv[monster_attacked].hit_points <= 0)
        {
            /* thing_thrown = 1; */
#ifdef WIZARD
            itoa(damage_done, st_prn, 10);
            strcpy(info, "Hit for ");
            strcat(info, st_prn);       /* note: doesn't take account of special weapons etc */
            strcat(info, ".");
            mpr(info);
#endif
            if (you.equip[EQ_WEAPON] != -1 && you.inv_class[you.equip[EQ_WEAPON]] == OBJ_WEAPONS && you.inv_dam[you.equip[EQ_WEAPON]] < 180 && you.inv_dam[you.equip[EQ_WEAPON]] % 30 == SPWPN_VAMPIRICISM)
            {
                if (mons_holiness(menv[monster_attacked].type) <= MH_NORMAL && damage_done >= 1 && random2(5) != 0 && you.hp < you.hp_max)
                {
                    you.hp += random2(damage_done) + 1;         // more than if not killed

                    you.redraw_hit_points = 1;
                    if (you.hp > you.hp_max)
                        you.hp = you.hp_max;
                    if (you.hunger <= 11000 && you.is_undead < 2)
                        you.hunger += random2(30) + random2(30) + 30;
                    mpr("You feel better.");
                    naughty(NAUGHTY_NECROMANCY, 2);
                }
            }
            monster_die(monster_attacked, 1, 0);
            if (menv[monster_attacked].type == MONS_GIANT_SPORE)
            {
                strcpy(info, "You hit the giant spore.");
                mpr(info);
            }
            return;
        }
        if (damage_done < 1 && menv[monster_attacked].enchantment[2] != 6)
        {
            hit = 1;
            strcpy(info, "You hit ");
            strcat(info, monam(menv[monster_attacked].number, menv[monster_attacked].type, menv[monster_attacked].enchantment[2], 1));
            strcat(info, ", but do no damage.");
            mpr(info);
        }
    }
    else
    {
        hit = 0;
        if (your_to_hit + heavy_armour / 2 >= menv[monster_attacked].evasion)
        {
            strcpy(info, "Your armour prevents you from hitting ");
        }
        else
            strcpy(info, "You miss ");
        strcat(info, monam(menv[monster_attacked].number, menv[monster_attacked].type, menv[monster_attacked].enchantment[2], 1));
        strcat(info, ".");
        mpr(info);
    }

    if (hit == 1 && damage_done >= 1 || (hit == 1 && damage_done < 1 && menv[monster_attacked].enchantment[2] == 6))
    {
        strcpy(info, "You ");
        strcat(info,damage_noise);
        strcat(info," ");
        strcat(info, monam(menv[monster_attacked].number, menv[monster_attacked].type, menv[monster_attacked].enchantment[2], 1));
        strcat(info,damage_noise2);
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

        if (mons_holiness(menv[monster_attacked].type) == -1)
            done_good(4, 1);

        if (you.special_wield == SPWLD_TORMENT)
        {
            torment();
            naughty(NAUGHTY_UNHOLY, 5);
        }
        if (you.special_wield == SPWLD_ZONGULDROK || you.special_wield == SPWLD_CURSE)
            naughty(NAUGHTY_NECROMANCY, 3);
    }
    if (menv[monster_attacked].type == MONS_JELLY || menv[monster_attacked].type == MONS_OOZE || menv[monster_attacked].type == MONS_ACID_BLOB || menv[monster_attacked].type == MONS_ROYAL_JELLY)
        weapon_acid(5);

    int specdam = 0;

/* remember, damage_done is still useful! */

    int weap_dam = 0;


    if (hit == 1 && special_brand != 0 && you.equip[EQ_WEAPON] == -1)
    {
        /* No electrocution - this references inv_plus2 */
        weap_dam = special_brand;
        goto dam_thing;
    }

    if (you.equip[EQ_WEAPON] != -1
            && you.inv_class[you.equip[EQ_WEAPON]] == OBJ_WEAPONS && hit == 1)
    {
        if (you.inv_type[you.equip[EQ_WEAPON]] == WPN_DEMON_BLADE
                    || you.inv_type[you.equip[EQ_WEAPON]] == WPN_DEMON_WHIP
                    || you.inv_type[you.equip[EQ_WEAPON]] == WPN_DEMON_TRIDENT)
            naughty(NAUGHTY_UNHOLY, 1);

        weap_dam = you.inv_dam[you.equip[EQ_WEAPON]] % 30;
        if (you.inv_dam[you.equip[EQ_WEAPON]] >= 180)
            weap_dam = 0;

        if (you.inv_dam[you.equip[EQ_WEAPON]] % 30 >= SPWPN_RANDART_I)
            weap_dam = randart_wpn_properties(
                                    you.inv_class[you.equip[EQ_WEAPON]],
                                    you.inv_type[you.equip[EQ_WEAPON]],
                                    you.inv_dam[you.equip[EQ_WEAPON]],
                                    you.inv_plus[you.equip[EQ_WEAPON]],
                                    you.inv_plus2[you.equip[EQ_WEAPON]],
                                                    0, RAP_BRAND);

        switch (you.inv_dam[you.equip[EQ_WEAPON]])
        {
        case NWPN_SWORD_OF_CEREBOV:               /* sword of cerebov */
            weap_dam = 1;
            break;

        case NWPN_STAFF_OF_OLGREB:               /* staff of Olgreb */
            weap_dam = 6;
            break;

        case NWPN_VAMPIRE_S_TOOTH:               /* vampire's tooth */
            weap_dam = 13;
            break;
        }
    }

    if (you.equip[EQ_WEAPON] != -1
            && you.inv_class[you.equip[EQ_WEAPON]] == OBJ_WEAPONS && hit == 1)
            // && you.inv_dam [you.equip [0]] <= 180)
    {
dam_thing:
        switch (weap_dam)
        {
        case SPWPN_NORMAL:      /* nothing */
            break;

        case SPWPN_FLAMING:     /* flaming */
            specdam = 0;

            if (mons_res_fire(menv[monster_attacked].type) == 0)
            {
                specdam = random2(damage_done) / 2 + 1;
                if (menv[monster_attacked].inv[2] != 501
                    && mitm.special[menv[monster_attacked].inv[2]] % 30 == 2)
                {
                    specdam = (random2(damage_done) / 2 + 1) / 3;
                }
            }

            if (mons_res_fire(menv[monster_attacked].type) == -1
                    && (menv[monster_attacked].inv[2] == 501
                    || mitm.special[menv[monster_attacked].inv[2]] % 30 != 2))
            {
                specdam = random2(damage_done) + 1;
            }

            if (specdam != 0)
            {
                strcpy(info, "You burn ");
                strcat(info, monam(menv[monster_attacked].number,
                                    menv[monster_attacked].type,
                                    menv[monster_attacked].enchantment[2], 1));

                if (specdam < 3)
                    strcat(info, ".");

                if (specdam >= 3 && specdam < 7)
                    strcat(info, "!");

                if (specdam >= 7)
                    strcat(info, "!!");

                mpr(info);
            }
            break;

        case SPWPN_FREEZING:    /* freezing */
            specdam = 0;
            if (mons_res_cold(menv[monster_attacked].type) == 0)
            {
                specdam = random2(damage_done) / 2 + 1;
                if (menv[monster_attacked].inv[2] != 501
                    && mitm.special[menv[monster_attacked].inv[2]] % 30 == 3)
                {
                    specdam = (random2(damage_done) / 2 + 1) / 3;
                }
            }

            if (mons_res_cold(menv[monster_attacked].type) == -1
                    && (menv[monster_attacked].inv[2] == 501
                    || mitm.special[menv[monster_attacked].inv[2]] % 30 != 3))
            {
                specdam = random2(damage_done) + 1;
            }

            if (specdam != 0)
            {
                strcpy(info, "You freeze ");
                strcat(info, monam(menv[monster_attacked].number,
                                    menv[monster_attacked].type,
                                    menv[monster_attacked].enchantment[2], 1));

                if (specdam < 3)
                    strcat(info, ".");

                if (specdam >= 3)
                    strcat(info, "!");

                if (specdam >= 7)
                    strcat(info, "!!");

                mpr(info);
            }
            break;

        case SPWPN_HOLY_WRATH:  /* holy wrath */
            specdam = 0;
            switch (mons_holiness(menv[monster_attacked].type))
            {
            case 1:
                specdam += random2(damage_done) + 1;
                break;

            case 2:
                specdam += (random2(damage_done * 15) / 10) + 1;
                break;
            }
            break;


        case SPWPN_ELECTROCUTION:       /* electrocution */
            specdam = 0;

            if (mons_flies(menv[monster_attacked].type))
                break;

            if (mons_res_elec(menv[monster_attacked].type) != 0)
                break;

            if (random2(3) == 0)
            {
                strcpy(info, "There is a sudden explosion of sparks!");
                mpr(info);
                specdam += random2(10) + random2(10) + random2(10);
            }
            break;

        case SPWPN_ORC_SLAYING: /* orc slaying */
            if (mons_charclass(menv[monster_attacked].type) == MONS_ORC)
            {
                menv[monster_attacked].hit_points -= 1 + random2(damage_done);
            }
            break;

        case SPWPN_VENOM:       /* venom */
            if (random2(4) != 0)
                poison_monster(monster_attacked, 0);
            break;

        case SPWPN_DRAINING:
            if (mons_holiness(menv[monster_attacked].type) > MH_NORMAL
                                                            || random2(3) == 0)
                break;

            strcpy(info, "You drain ");
            strcat(info, monam(menv[monster_attacked].number,
                                    menv[monster_attacked].type,
                                    menv[monster_attacked].enchantment[2], 1));
            strcat(info, "!");
            mpr(info);

            if (random2(5) == 0)
                menv[monster_attacked].hit_dice--;

            menv[monster_attacked].max_hit_points -= 2 + random2(3);
            menv[monster_attacked].hit_points -= 2 + random2(3);
            if (menv[monster_attacked].hit_points
                                    >= menv[monster_attacked].max_hit_points)
            {
                menv[monster_attacked].hit_points
                                    = menv[monster_attacked].max_hit_points;
            }

            if (menv[monster_attacked].hit_dice <= 0)
                menv[monster_attacked].hit_points = 0;
            specdam = random2(damage_done) / 2 + 1;
            naughty(NAUGHTY_NECROMANCY, 2);
            break;

            /* 9 = speed - done before */

        case SPWPN_VORPAL:      /* slicing etc */
            specdam = random2(damage_done) / 2 + 1;
            if (menv[monster_attacked].type == MONS_WORM_TAIL
                && (damage_type(0, you.inv_type[you.equip[EQ_WEAPON]]) == 1
                || damage_type(0, you.inv_type[you.equip[EQ_WEAPON]]) == 3))
            {
                strcpy(info, "You slice the worm tail!");
                mpr(info);
                menv[monster_attacked].hit_points -= 20 + random2(20);
            }
            break;

        case SPWPN_VAMPIRICISM: /* vampiric */
            specdam = 0;        /* note does no extra damage */
            if (mons_holiness(menv[monster_attacked].type) > MH_NORMAL)
                break;     /* should really also prevent draining eg golems */

            if (damage_done < 1)
                break;

            if (random2(5) == 0 || you.hp == you.hp_max)
                break;

            if (you.equip[EQ_WEAPON] != -1
                                && you.inv_dam[you.equip[EQ_WEAPON]] == 194)
            {
                you.hp += damage_done;
            }
            else
            {
                /* thus is probably more valuable on larger weapons? */
                you.hp += random2(damage_done) + 1;
            }

            you.redraw_hit_points = 1;

            if (you.hp > you.hp_max)
                you.hp = you.hp_max;

            if (you.hunger <= 11000 && you.is_undead < 2)
                you.hunger += random2(30) + random2(30);

            mpr("You feel better.");

            naughty(NAUGHTY_NECROMANCY, 2);
            break;

        case SPWPN_DISRUPTION:  /* mace of disruption */
            specdam = 0;
            if (mons_holiness(menv[monster_attacked].type) == MH_UNDEAD
                                                            && random2(3) != 0)
            {
                strcpy(info, monam(menv[monster_attacked].number,
                                    menv[monster_attacked].type,
                                    menv[monster_attacked].enchantment[2], 0));

                strcat(info, " shudders.");
                mpr(info);

                specdam += random2(damage_done + 1);
                specdam += random2(damage_done + 1);
                specdam += random2(damage_done + 1);
            }
            break;

        case SPWPN_PAIN:        /* pain */
            specdam = 0;
            if (mons_holiness(menv[monster_attacked].type) <= MH_NORMAL
                                    && random2(8) <= you.skills[SK_NECROMANCY])
            {
                strcpy(info, monam(menv[monster_attacked].number,
                                    menv[monster_attacked].type,
                                    menv[monster_attacked].enchantment[2], 0));
                strcat(info, " convulses in agony.");
                mpr(info);
                specdam += random2((you.skills[SK_NECROMANCY] * 2) + 1);
            }
            naughty(NAUGHTY_NECROMANCY, 4);
            break;

        case SPWPN_DISTORTION:  /* distortion */
            if (random2(3) == 0)
            {
                strcpy(info, "Space bends around ");
                strcat(info, monam(menv[monster_attacked].number,
                                    menv[monster_attacked].type,
                                    menv[monster_attacked].enchantment[2], 1));
                strcat(info, ".");
                mpr(info);
                specdam += random2(5) + random2(3) + 1;
                break;
            }

            if (random2(3) == 0)
            {
                strcpy(info, "Space warps horribly around ");
                strcat(info, monam(menv[monster_attacked].number,
                                    menv[monster_attacked].type,
                                    menv[monster_attacked].enchantment[2], 1));
                strcat(info, "!");
                mpr(info);
                specdam += random2(12) + random2(13) + 3;
                break;
            }

            if (random2(3) == 0)
            {
                monster_blink(monster_attacked);
                break;
            }

            if (random2(2) == 0)
            {
                monster_teleport(monster_attacked, 0);
                break;
            }

            if (random2(2) == 0)
            {
                monster_teleport(monster_attacked, 1);
                break;
            }

            if (random2(2) == 0)
            {
                monster_die(monster_attacked, KILL_RESET, 0);
                return;
            }
            break;
        }                       /* end switch */
    }

    if (mons_holiness(menv[monster_attacked].type) < MH_NORMAL)
        naughty(NAUGHTY_ATTACK_HOLY, menv[monster_attacked].hit_dice);

    if (menv[monster_attacked].type == MONS_HYDRA)      // hydra
    {
        if (you.equip[EQ_WEAPON] != -1
                && (damage_type(you.inv_class[you.equip[EQ_WEAPON]],
                        you.inv_type[you.equip[EQ_WEAPON]]) == 1
                || damage_type(you.inv_class[you.equip[EQ_WEAPON]],
                        you.inv_type[you.equip[EQ_WEAPON]]) == 3))
        {
            if ((random2(2) != 0 || damage_done < 4)
                        && (you.inv_class[you.equip[EQ_WEAPON]] != OBJ_WEAPONS
                        || you.inv_dam[you.equip[EQ_WEAPON]] != WPN_BATTLEAXE))
            {
                goto mons_dies;
            }

            strcpy(info, "You ");
            switch (random2(4))
            {
            case 0:
                strcat(info, "slice");
                break;
            case 1:
                strcat(info, "lop");
                break;
            case 2:
                strcat(info, "chop");
                break;
            case 3:
                strcat(info, "hack");
                break;
            }

            strcat(info, " one of ");
            strcat(info, monam(menv[monster_attacked].number,
                                    menv[monster_attacked].type,
                                    menv[monster_attacked].enchantment[2], 1));
            strcat(info, "'s heads off.");
            mpr(info);

            menv[monster_attacked].number--;
            if (menv[monster_attacked].number <= 0)
            {
                menv[monster_attacked].hit_points = -1;
                goto mons_dies;
            }

            if (you.inv_class[you.equip[EQ_WEAPON]] == OBJ_WEAPONS
                    && you.inv_dam[you.equip[EQ_WEAPON]] % 30 == SPWPN_FLAMING)
            {
                goto mons_dies; // cauterised
            }

            if (menv[monster_attacked].number >= 18)
                goto mons_dies;
            strcpy(info, "It grows two more!");
            mpr(info);
            menv[monster_attacked].number += 2;
        }
    }
/* remember, the hydra function sometimes skips straight to mons_dies */

    menv[monster_attacked].hit_points -= specdam;

mons_dies:
    if (menv[monster_attacked].hit_points <= 0)
    {
        monster_die(monster_attacked, 1, 0);
        return;
    }

    if (you.invis != 0 && menv[monster_attacked].behavior == BEH_SLEEP)
        menv[monster_attacked].behavior = BEH_CHASING_I;

    if (you.invis != 0 && random2(2) == 0)
    {
        menv[monster_attacked].target_x = you.x_pos;
        menv[monster_attacked].target_y = you.y_pos;
        menv[monster_attacked].behavior = BEH_CHASING_I;
    }

    if (unarmed_attacks)
    {
        char attack_name[20] = "";
        int sc_dam = 0;

        int unarmed_attack = UNAT_NO_ATTACK;

        if (can_do_unarmed_combat)
        {
            unarmed_attack = random2(2) + 1;

            if ((you.attribute[ATTR_TRANSFORMATION] == TRAN_DRAGON
                    || (you.species >= SP_WHITE_DRACONIAN
                                && you.species <= SP_UNK2_DRACONIAN))
                    && random2(3) == 0)
            {
                unarmed_attack = UNAT_TAILSLAP;
            }

            if (random2(2) == 0)
                unarmed_attack = UNAT_PUNCH;
        }

        for (int scount = 0; scount < 4; scount++)
        {
            switch (scount)
            {
            case 0:
                if (unarmed_attack != UNAT_KICK)
                    if (you.species != SP_CENTAUR || random2(2) == 0)
                        continue;
                if (you.attribute[ATTR_TRANSFORMATION] == TRAN_SPIDER
                        || you.attribute[ATTR_TRANSFORMATION] == TRAN_ICE_BEAST
                        || you.attribute[ATTR_TRANSFORMATION] == TRAN_DRAGON)
                {
                    continue;
                }

                strcpy(attack_name, "kick");
                sc_dam = 5 + (you.species == SP_CENTAUR) * 5;
                break;

            case 1:
                if (unarmed_attack != UNAT_HEADBUTT)
                {
                    if ((you.species != SP_MINOTAUR
                            && (you.mutation[MUT_HORNS] == 0
                                    && you.species != SP_KENKU))
                            || random2(3) != 0)
                    {
                        continue;
                    }
                }

                if (you.attribute[ATTR_TRANSFORMATION] == TRAN_SPIDER
                        || you.attribute[ATTR_TRANSFORMATION] == TRAN_ICE_BEAST
                        || you.attribute[ATTR_TRANSFORMATION] == TRAN_DRAGON)
                {
                    continue;
                }

                if (you.species == SP_KENKU)
                    strcpy(attack_name, "peck");
                else
                    strcpy(attack_name, "head-butt");

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
                }             /* horned/spiked helmets/helms do extra damage */
                break;

            case 2:         /* draconians */
                if (unarmed_attack != UNAT_TAILSLAP)
                {
                    if (you.species < SP_RED_DRACONIAN
                        || you.species > SP_UNK2_DRACONIAN || random2(4) != 0)
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
                if (you.species == SP_GREY_DRACONIAN
                                            && you.experience_level >= 6)
                {
                    sc_dam = 12;
                }
                break;

            case 3:
                if (unarmed_attack != UNAT_PUNCH)
                    continue;

                if (you.attribute[ATTR_TRANSFORMATION] == TRAN_SPIDER
                        || you.attribute[ATTR_TRANSFORMATION] == TRAN_ICE_BEAST
                        || you.attribute[ATTR_TRANSFORMATION] == TRAN_DRAGON)
                {
                    continue;
                }

                /* no punching with a shield or 2-handed wpn, except staves */
                if (random2(2) == 0 || you.equip[EQ_SHIELD] != -1
                    || (you.equip[EQ_WEAPON] != -1
                        && hands_required_for_weapon(
                                you.inv_class[you.equip[EQ_WEAPON]],
                                you.inv_type[you.equip[EQ_WEAPON]] )
                                                        == HANDS_TWO_HANDED))
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

            if (wearing_amulet(AMU_INACCURACY) == 1)
                your_to_hit -= 5;

            if (you.is_undead != 2)
                you.hunger -= 2;

            if (you.hunger_state == 1)
                your_to_hit -= 3;

            your_to_hit += slaying_bonus(0);
            your_to_hit = random2(your_to_hit);

            damage = sc_dam;        //4 + you.experience_level / 3;

            alert();

            if ((your_to_hit >= menv[monster_attacked].evasion
                                                        || random2(15) == 0))
            {
                int hit = 1;
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

                if (you.might > 1)
                    damage_done += random2(10) + 1;
                if (you.hunger_state == 1)
                    damage_done -= random2(5);

                damage_done += slaying_bonus(1);

                damage_done -= random2(menv[monster_attacked].armor_class + 1);

                if (damage_done < 0)
                    damage_done = 0;
                menv[monster_attacked].hit_points -= damage_done;

                if (damage_done >= 1)
                {
                    if (random2(5) == 0
                                && (!helpless || you.skills[SK_FIGHTING] < 2))
                    {
                        exercise(SK_FIGHTING, 1);
                    }
                }

                if (menv[monster_attacked].hit_points <= 0)
                {
    #ifdef WIZARD
                    itoa(damage_done, st_prn, 10);
                    strcpy(info, "Kick for ");
                    strcat(info, st_prn);
                    strcat(info, ".");
                    mpr(info);
    #endif
                    monster_die(monster_attacked, 1, 0);
                    if (menv[monster_attacked].type == MONS_GIANT_SPORE)
                    {
                        strcpy(info, "You ");
                        strcat(info, attack_name);
                        strcat(info, "the giant spore.");
                        mpr(info);
                    }
                    return;
                }

                if (damage_done < 1
                                && menv[monster_attacked].enchantment[2] != 6)
                {
                    strcpy(info, "You ");
                    strcat(info, attack_name);
                    strcat(info, " ");
                    strcat(info, monam(menv[monster_attacked].number,
                                    menv[monster_attacked].type,
                                    menv[monster_attacked].enchantment[2], 1));
                    strcat(info, ", but do no damage.");
                    mpr(info);
                    hit = 1;
                }
            }
            else
            {
                strcpy(info, "Your ");
                strcat(info, attack_name);
                strcat(info, " misses ");
                strcat(info, monam(menv[monster_attacked].number,
                                    menv[monster_attacked].type,
                                    menv[monster_attacked].enchantment[2], 1));
                strcat(info, ".");
                mpr(info);
                damage_done = -99;
            }

            if (damage_done >= 1
                        || (damage_done < 1
                                && menv[monster_attacked].enchantment[2] == 6
                                && damage_done != -99))
            {
                strcpy(info, "You ");
                strcat(info, attack_name);
                strcat(info, " ");
                strcat(info, monam(menv[monster_attacked].number,
                                    menv[monster_attacked].type,
                                    menv[monster_attacked].enchantment[2], 1));

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

                if (mons_holiness(menv[monster_attacked].type) == -1)
                    done_good(4, 1);
                hit = 1;
            }

            if (!helpless || you.skills[SK_UNARMED_COMBAT] < 2)
            {
                exercise(SK_UNARMED_COMBAT, 1);
            }
        }
    }

    if (hit != 0)
    {
        print_wounds(monster_attacked);
    }
    return;
}



int weapon_type_modify(int weapnum, char *noise, char *noise2, int damage)
{
     /* Added by DML 6/10/99.
        For now, always returns damage: that is, it never modifies values,
        just adds 'color'*/
     int weap_type=WPN_UNKNOWN;
     if(damage>=HIT_WEAK)
     {
          if(weapnum == -1)
               weap_type=WPN_UNARMED;
          else {
               if(you.inv_class[weapnum]==OBJ_STAVES)
                    weap_type=WPN_QUARTERSTAFF;
               else if(you.inv_class[weapnum] == OBJ_WEAPONS)
                    weap_type=you.inv_type[weapnum];
          }
     }

         strcpy(noise2, "");

     switch(weap_type)
     {
     case WPN_KNIFE:
     case WPN_DAGGER:
     case WPN_SHORT_SWORD:
     case WPN_TRIDENT:
     case WPN_DEMON_TRIDENT:
     case WPN_SPEAR:
          if(damage<HIT_MED)
               strcpy(noise,"puncture");
          else if(damage<HIT_STRONG)
               strcpy(noise,"impale");
          else
          {
            strcpy(noise,"spit");
            strcpy(noise2, " like a pig");
          }
          return damage;
     case WPN_BOW:
     case WPN_CROSSBOW:
     case WPN_HAND_CROSSBOW:
          if(damage<HIT_STRONG)
               strcpy(noise,"puncture");
          else
               strcpy(noise,"skewer");
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
          if(damage<HIT_MED)
               strcpy(noise,"slice");
          else if(damage<HIT_STRONG)
               strcpy(noise,"slash");
          else
          {
            strcpy(noise,"open");
            strcpy(noise2," like a pillowcase");
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
     case WPN_MORNINGSTAR:  /*for now, just a bludgeoning weapon*/
     case WPN_SPIKED_FLAIL:  /*for now, just a bludgeoning weapon*/
     case WPN_EVENINGSTAR:
     case WPN_GIANT_SPIKED_CLUB:
          if(damage<HIT_MED)
               strcpy(noise,"sock");
          else if(damage<HIT_STRONG)
               strcpy(noise,"bludgeon");
          else
          {
            strcpy(noise,"crush");
            strcpy(noise2," like a grape");
          }
          return damage;
     case WPN_WHIP:
     case WPN_DEMON_WHIP:
          if(damage<HIT_MED)
               strcpy(noise,"whack");
          else strcpy(noise,"thrash");
          return damage;
     case WPN_UNARMED:
          if(you.species==SP_TROLL)
          {
               if(damage<HIT_MED)
                    strcpy(noise,"claw");
               else if(damage<HIT_STRONG)
                    strcpy(noise,"mangle");
               else strcpy(noise,"eviscerate");
               return damage;
          } else {
               if(damage<HIT_MED)
                    strcpy(noise,"punch");
               else strcpy(noise,"pummel");
               return damage;
          }
     case WPN_UNKNOWN:
     default:
          strcpy(noise,"hit");
          return damage;
     }
}


void monster_attack(int monster_attacking)
{
    int damage_taken = 0;
    char blocked = 0;
    char hit = 0;
    int mmov_x = 0;
    char str_pass[80];
    char water_attack = 0;      /* Is the player being attacked by a water creature while standing in water? */
    int specdam = 0;

    char heads = 0;

    if (menv[monster_attacking].type == MONS_HYDRA)
        heads = menv[monster_attacking].number;

    int hand_used = 0;

    if (menv[monster_attacking].type == MONS_GIANT_SPORE)
    {
        menv[monster_attacking].hit_points = -1;
        return;
    }
    {
        you.pet_target = monster_attacking;
    }

    if (menv[monster_attacking].behavior == BEH_ENSLAVED)
        return;

    if (menv[monster_attacking].type >= MLAVA0 && menv[monster_attacking].number == 1)
        return;

    if (wearing_amulet(AMU_WARDING) == 1 || (you.religion == GOD_VEHUMET && you.duration[DUR_PRAYER] != 0 && you.piety >= 75))
    {
        if (menv[monster_attacking].enchantment[1] >= 20 && menv[monster_attacking].enchantment[1] <= 25)
            if (random2(2) == 0)
            {
                strcpy(info, monam(menv[monster_attacking].number, menv[monster_attacking].type, menv[monster_attacking].enchantment[2], 0));
                strcat(info, " tries to attack you, but flinches away.");
                mpr(info);
                return;
            }
    }
    if (grd[menv[monster_attacking].x][menv[monster_attacking].y] == 65 && mons_flies(menv[monster_attacking].type) == 0 && menv[monster_attacking].type < MLAVA0)
    {
        if (random2(4) == 0)
        {
            strcpy(info, monam(menv[monster_attacking].number, menv[monster_attacking].type, menv[monster_attacking].enchantment[2], 0));
            strcat(info, " splashes around in the water.");
            mpr(info);
            return;
        }
    }
    if (grd[you.x_pos][you.y_pos] == 65 && you.levitation == 0 && menv[monster_attacking].type >= MWATER0)
    {
        water_attack = 1;
        strcpy(info, monam(menv[monster_attacking].number, menv[monster_attacking].type, menv[monster_attacking].enchantment[2], 0));
        strcat(info, " uses the watery terrain to its advantage.");
        mpr(info);
    }
    char runthru = 0;

    for (runthru = 0; runthru < 4; runthru++)
    {

        blocked = 0;

        if (menv[monster_attacking].type == MONS_HYDRA)
        {
            if (heads <= 0)
                break;
            runthru = 0;
            heads--;
        }
        char mdam = mondamage(menv[monster_attacking].type, runthru);

        if (menv[monster_attacking].type == MONS_SMALL_ZOMBIE || menv[monster_attacking].type == MONS_BIG_ZOMBIE || menv[monster_attacking].type == MONS_SMALL_SKELETON || menv[monster_attacking].type == MONS_LARGE_SKELETON || menv[monster_attacking].type == MONS_SPECTRAL_THING)
        {
            mdam = mondamage(menv[monster_attacking].number, runthru);
            if (mdam > 1 && mdam < 6)
            {
                mdam--;
            }
            if (mdam > 5 && mdam < 14)
            {
                mdam -= 2;
            }
            if (mdam > 13 && mdam < 18)
            {
                mdam -= 3;
            }
            if (mdam > 17)
            {
                mdam -= 4;
            }
        }
        if (mdam == 0)
            break;

        if (menv[monster_attacking].type == MONS_TWO_HEADED_OGRE && runthru == 1 && menv[monster_attacking].inv[1] != 501)
        {
            hand_used = 1;
        }
        damage_taken = 0;


        int mons_to_hit = 20 + menv[monster_attacking].hit_dice;        // * menv [monster_attacking].hit_dice; // * 3  //you.strength + (0.5 * you.you.max_dex) + (you.f_abil);

        mons_to_hit += water_attack * 5;

        if (menv[monster_attacking].inv[hand_used] != 501)
        {

            item_name(mitm.pluses2[menv[monster_attacking].inv[hand_used]], mitm.base_type[menv[monster_attacking].inv[hand_used]], mitm.sub_type[menv[monster_attacking].inv[hand_used]], mitm.special[menv[monster_attacking].inv[hand_used]], mitm.pluses[menv[monster_attacking].inv[hand_used]], mitm.quantity[menv[monster_attacking].inv[hand_used]], mitm.id[menv[monster_attacking].inv[hand_used]], 0, str_pass);

            if (mitm.base_type[menv[monster_attacking].inv[hand_used]] != 0 || strstr(str_pass, "questionable item") != NULL)
            {
                destroy_item(menv[monster_attacking].inv[hand_used]);
                menv[monster_attacking].inv[hand_used] = 501;
            }
        }
        if (menv[monster_attacking].inv[hand_used] != 501)
        {
            if (mitm.pluses[menv[monster_attacking].inv[hand_used]] - 50 > 130)
            {
                mons_to_hit += mitm.pluses[menv[monster_attacking].inv[hand_used]] - 50 - 100;
            }
            else
            {
                mons_to_hit += mitm.pluses[menv[monster_attacking].inv[hand_used]] - 50;
            }

            mons_to_hit += property(OBJ_WEAPONS, mitm.sub_type[menv[monster_attacking].inv[0]], PWPN_HIT);

            if (menv[monster_attacking].speed_increment >= 50)
                menv[monster_attacking].speed_increment -= (property(mitm.base_type[menv[monster_attacking].inv[0]], mitm.sub_type[menv[monster_attacking].inv[0]], PWPN_SPEED) - 10);
        }

        if (menv[monster_attacking].type == MONS_KILLER_BEE || menv[monster_attacking].type == MONS_WORM)
        {
            /* maybe this will work better: */
            if (menv[monster_attacking].speed_increment >= 20)
                menv[monster_attacking].speed_increment -= 10;
        }

        if (player_shield_class() > 0 && you.paralysis == 0 && you.conf == 0
                && random2(menv[monster_attacking].hit_dice + 15
                                    + 5 * you.shield_blocks)
                                            <= random2(player_shield_class()))
        {
            you.shield_blocks++;
            strcpy(info, "You block ");
            int mmov_x = menv[monster_attacking].inv[hand_used];

            if (menv[monster_attacking].type == MONS_DANCING_WEAPON)
            {
                char str_pass[80];

                item_name(mitm.pluses2[mmov_x], mitm.base_type[mmov_x], mitm.sub_type[mmov_x], mitm.special[mmov_x], mitm.pluses[mmov_x], mitm.quantity[mmov_x], mitm.id[mmov_x], 0, str_pass);
                strcpy(info, str_pass);
            }
            else
                strcat(info, monam(menv[monster_attacking].number, menv[monster_attacking].type, menv[monster_attacking].enchantment[2], 1));   //gmon_name [menv [monster_attacking].type]);

            strcat(info, "'s attack.");
            mpr(info);
            blocked = 1;
            hit = 0;
            if (you.equip[EQ_SHIELD] != -1 && random2(4) == 0)
                exercise(SK_SHIELDS, 1);
            /* continue; */
        }
        else if (player_light_armour() && random2(3) == 0)
            exercise(SK_DODGING, 1);


        if (blocked == 0
                && (random2(mons_to_hit)
                        >= random2(player_evasion()) + random2(you.dex) / 3 - 2
                    || random2(15) == 0))
        {
            hit = 1;

            int damage_size = 0;

            if (menv[monster_attacking].inv[hand_used] != 501 && mitm.base_type[menv[monster_attacking].inv[hand_used]] == OBJ_WEAPONS && (mitm.sub_type[menv[monster_attacking].inv[hand_used]] < WPN_SLING || mitm.sub_type[menv[monster_attacking].inv[hand_used]] > WPN_CROSSBOW))
            {

                damage_size = property(mitm.base_type[menv[monster_attacking].inv[0]], mitm.sub_type[menv[monster_attacking].inv[0]], PWPN_DAMAGE);
                damage_taken = random2(damage_size);

                if (mitm.special[menv[monster_attacking].inv[hand_used]] / 30 == DWPN_ORCISH && mons_charclass(menv[monster_attacking].type) == MONS_ORC)
                {
                    damage_taken += random2(2);
                }

                if (mitm.pluses2[menv[monster_attacking].inv[hand_used]] > 130)
                {
                    if (mitm.pluses2[menv[monster_attacking].inv[hand_used]] - 150 >= 0)        /* + or 0 to-dam */
                        damage_taken += random2(mitm.pluses2[menv[monster_attacking].inv[hand_used]] - 150 + 1);
                    else
                        damage_taken -= random2(abs(mitm.pluses2[menv[monster_attacking].inv[hand_used]] - 150 + 1));   /* - to-dam */
                }
                else
                {
                    if (mitm.pluses2[menv[monster_attacking].inv[hand_used]] - 50 >= 0)         /* + or 0 to-dam */
                        damage_taken += random2(mitm.pluses2[menv[monster_attacking].inv[hand_used]] - 50 + 1);
                    else
                        damage_taken -= random2(abs(mitm.pluses2[menv[monster_attacking].inv[hand_used]] - 50 + 1));    /* - to-dam */
                }
                damage_taken -= random2(3) + 1;         //1;

            }

            damage_size += mdam;
            damage_taken += random2(mdam) + 1;

            damage_taken *= water_attack + 1;

            int ac = player_AC();

            if (ac > 0)
            {
                int damage_reduction = random2(ac + 1);

                if (!player_light_armour())
                {
                    int percent = you.skills[SK_ARMOUR];

                    if (you.equip[EQ_BODY_ARMOUR] != -1)
                    {
                        percent += property( OBJ_ARMOUR,
                                    you.inv_type[you.equip[EQ_BODY_ARMOUR]],
                                             PARM_AC );
                    }

                    if (percent > 50)
                        percent = 50;

                    int min;

                    min = (damage_size * percent) / 100 + 1;

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
        /* end of if mons_to_hit >= player_evasion() */
        else if (blocked == 0)
        {
            hit = 0;

            int mmov_x = menv[monster_attacking].inv[hand_used];
            char str_pass[80];

            if (menv[monster_attacking].type == MONS_DANCING_WEAPON)
            {
                item_name(mitm.pluses2[mmov_x], mitm.base_type[mmov_x], mitm.sub_type[mmov_x], mitm.special[mmov_x], mitm.pluses[mmov_x], mitm.quantity[mmov_x], mitm.id[mmov_x], 0, str_pass);
                strcpy(info, str_pass);
            }
            else
                strcpy(info, monam(menv[monster_attacking].number, menv[monster_attacking].type, menv[monster_attacking].enchantment[2], 0));   //gmon_name [menv [monster_attacking].type]);

            strcat(info, " misses you.");
            mpr(info);
        }
        if (damage_taken < 1 && hit == 1 && blocked == 0)
        {
            int mmov_x = menv[monster_attacking].inv[hand_used];
            char str_pass[80];

            if (menv[monster_attacking].type == MONS_DANCING_WEAPON)
            {
                item_name(mitm.pluses2[mmov_x], mitm.base_type[mmov_x], mitm.sub_type[mmov_x], mitm.special[mmov_x], mitm.pluses[mmov_x], mitm.quantity[mmov_x], mitm.id[mmov_x], 0, str_pass);
                strcpy(info, str_pass);
            }
            else
                strcpy(info, monam(menv[monster_attacking].number, menv[monster_attacking].type, menv[monster_attacking].enchantment[2], 0));   //gmon_name [menv [monster_attacking].type]);

            strcat(info, " hits you but doesn't do any damage.");
            mpr(info);
        }
        if ((int) damage_taken >= 1)
        {
            hit = 1;
            mmov_x = menv[monster_attacking].inv[hand_used];

            mmov_x = menv[monster_attacking].inv[hand_used];

            if (menv[monster_attacking].type == MONS_DANCING_WEAPON)
            {
                item_name(mitm.pluses2[mmov_x], mitm.base_type[mmov_x], mitm.sub_type[mmov_x], mitm.special[mmov_x], mitm.pluses[mmov_x], mitm.quantity[mmov_x], mitm.id[mmov_x], 0, str_pass);
                strcpy(info, str_pass);
            }
            else
                strcpy(info, monam(menv[monster_attacking].number, menv[monster_attacking].type, menv[monster_attacking].enchantment[2], 0));
            strcat(info, " hits you");

#ifdef WIZARD
            itoa(mmov_x, st_prn, 10);
            strcat(info, " it:");
            strcat(info, st_prn);       // note: doesn't take account of special weapons etc
#endif


//        if (menv [monster_attacking].type != 144 && menv [monster_attacking].inv [hand_used] != 501 && mitm.base_type [menv [monster_attacking].inv [hand_used]] == 0 && mitm.sub_type [menv [monster_attacking].inv [hand_used]] < 13 || mitm.sub_type [menv [monster_attacking].inv [hand_used]] > 16)
            if (menv[monster_attacking].type != MONS_DANCING_WEAPON && mmov_x != 501 && mitm.base_type[mmov_x] == 0 && (mitm.sub_type[mmov_x] < 13 || mitm.sub_type[mmov_x] > 16))
            {
                strcat(info, " with ");

                item_name(mitm.pluses2[mmov_x], mitm.base_type[mmov_x], mitm.sub_type[mmov_x], mitm.special[mmov_x], mitm.pluses[mmov_x], mitm.quantity[mmov_x], mitm.id[mmov_x], 3, str_pass);         // was 7

                strcat(info, str_pass);
                strcat(info, "!");

            }
            else
            {
                strcat(info, "!");
            }

            mpr(info);

            if (hit == 1)
            {
                if (you.equip[EQ_BODY_ARMOUR] != -1)
                    if (you.inv_type[you.equip[EQ_BODY_ARMOUR]] > ARM_LEATHER_ARMOUR && !(you.inv_type[you.equip[EQ_BODY_ARMOUR]] >= ARM_STEAM_DRAGON_HIDE && you.inv_type[you.equip[EQ_BODY_ARMOUR]] <= ARM_MOTTLED_DRAGON_ARMOUR) && random2(1000) <= mass(OBJ_ARMOUR, you.inv_type[you.equip[EQ_BODY_ARMOUR]]) && random2(2) == 0)
                        // raised from 1 -- bwross
                        exercise(SK_ARMOUR, 1 + random2(2));
            }
/* special attacks: */

            int mclas = menv[monster_attacking].type;

            if (mclas == MONS_KILLER_KLOWN)
            {                   /* Killer Klown */
                switch (random2(6))
                {
                case 0:
                    mclas = MONS_SNAKE;
                    break;      /* scorp */
                case 1:
                    mclas = MONS_NECROPHAGE;
                    break;      /* necrophage */
                case 2:
                    mclas = MONS_WRAITH;
                    break;      /* wraith */
                case 3:
                    mclas = MONS_FIRE_ELEMENTAL;
                    break;      /* fire elem */
                case 4:
                    mclas = MONS_ICE_BEAST;
                    break;      /* ice beast */
                case 5:
                    mclas = MONS_PHANTOM;
                    break;      /* phantom */
                }
            }
            switch (mclas)
            {
            case MONS_GIANT_ANT:        /* giant ant */
            case MONS_WOLF_SPIDER:      /* wolf spider */
            case MONS_REDBACK:  /* redback */
                if (player_res_poison() != 0)
                    break;
                if ((damage_taken >= 4 && random2(4) == 0) || random2(20) == 0)
                {
                    strcpy(info, monam(menv[monster_attacking].number, menv[monster_attacking].type, menv[monster_attacking].enchantment[2], 0));
                    strcat(info, " stings you!");
                    mpr(info);
                    you.poison++;
                    if (menv[monster_attacking].type == MONS_REDBACK)
                        you.poison += 3 + random2(5) + random2(5);
                }
                break;

            case MONS_KILLER_BEE:       /* killer bee */
            case MONS_BUMBLEBEE:        /* bumblebee */
                if (player_res_poison() == 0 && ((damage_taken >= 3 && random2(3) == 0) || random2(20) == 0))
                {
                    strcpy(info, monam(menv[monster_attacking].number, menv[monster_attacking].type, menv[monster_attacking].enchantment[2], 0));
                    strcat(info, " stings you!");
                    mpr(info);
                    you.poison += 1;
                    if (menv[monster_attacking].type == MONS_BUMBLEBEE)
                        you.poison += random2(3);
                }
                break;

            case MONS_ROTTING_DEVIL:    /* rotting devil */
            case MONS_NECROPHAGE:       /* necrophage */
            case MONS_GHOUL:    /* ghoul */
            case MONS_DEATH_OOZE:       // death ooze

                if (you.is_undead != 0)
                    break;
                if ((damage_taken >= 3 && random2(3) == 0) || random2(20) == 0)
                {
                    strcpy(info, "You feel your flesh start to rot away!");
                    mpr(info);
                    you.rotting += random2(3) + 1;
                }
                if (random2(4) == 0)
                {
                    strcpy(info, "You feel ill.");
                    mpr(info);
                    if (you.disease > 100)
                        you.disease = 210;
                    else
                        you.disease += 50 + random2(100);
                }
                break;

            case MONS_GIANT_MOSQUITO:   // mosquito

                if (random2(3) != 0)
                    strcpy(info, "You feel ill.");
                mpr(info);
                if (you.disease > 100)
                    you.disease = 210;
                else
                    you.disease += 50 + random2(100);
                break;

            case MONS_FIRE_VORTEX:      /* fire vortex */
                menv[monster_attacking].hit_points = -10;
            case MONS_FIRE_ELEMENTAL:   /* fire elemental */
            case MONS_BALRUG:   /* balrug */
            case MONS_SUN_DEMON:        /* sun demon */
                strcpy(info, "You are engulfed in flame!");
                mpr(info);
                if (player_res_fire() > 100)
                {
                    damage_taken += (15 + random2(15)) / 2 + ((player_res_fire() - 100) * (player_res_fire() - 100));
                }
                if (player_res_fire() <= 100)
                {
                    damage_taken += 15 + random2(15);
                }
                if (player_res_fire() < 100)
                {
                    damage_taken += 8 + random2(8);
                }
                scrolls_burn(1, 6);
                break;


            case MONS_SNAKE:    /* scorpion */
            case MONS_GIANT_MITE:       /* giant mite */
            case MONS_GOLD_MIMIC:
            case MONS_WEAPON_MIMIC:
            case MONS_ARMOUR_MIMIC:
            case MONS_SCROLL_MIMIC:
            case MONS_POTION_MIMIC:     /* Mimics */
                /* case MONS_MIDGE: less demon */
                if (player_res_poison() == 0 && ((damage_taken >= 3 && random2(4) == 0) || random2(15) == 0))
                {
                    strcpy(info, monam(menv[monster_attacking].number, menv[monster_attacking].type, menv[monster_attacking].enchantment[2], 0));
                    strcat(info, " poisons you!");
                    mpr(info);
                    you.poison++;
                }
                break;



            case MONS_QUEEN_BEE:        /* Queen bee */
            case MONS_GIANT_CENTIPEDE:          /* giant centipede */
            case MONS_SOLDIER_ANT:      /* soldier ant */
            case MONS_QUEEN_ANT:        /* Queen ant */
                if (player_res_poison() == 0)
                {
                    strcpy(info, monam(menv[monster_attacking].number, menv[monster_attacking].type, menv[monster_attacking].enchantment[2], 0));
                    strcat(info, " stings you!");
                    mpr(info);
                    you.poison += 2;
                }
                break;


            case MONS_SCORPION: // snake

            case MONS_BROWN_SNAKE:      // br snake

            case MONS_BLACK_SNAKE:      // black snake

            case MONS_YELLOW_SNAKE:     // yellow snake

                if (player_res_poison() == 0 && ((damage_taken >= 3 && random2(4) == 0) || random2(20) == 0))
                {
                    strcpy(info, monam(menv[monster_attacking].number, menv[monster_attacking].type, menv[monster_attacking].enchantment[2], 0));
                    strcat(info, " poisons you!");
                    mpr(info);
                    you.poison++;
                }
                break;


            case MONS_SHADOW_DRAGON:    // shadow dragon
            case MONS_SPECTRAL_THING:   /* spectral thing */
                if (random2(2) == 0)
                    break;

            case MONS_WIGHT:    // less likely because wights do less damage
            case MONS_WRAITH:   // wraith
            case MONS_SOUL_EATER:       // shadow devil
            case MONS_SHADOW_FIEND:     // shad fiend
            case MONS_SPECTRAL_WARRIOR: // spectre
            case MONS_ORANGE_RAT:       // orange rat
            case MONS_SHADOW_WRAITH:    // shadow wraith
            case MONS_ANCIENT_LICH:     // ancient lich
                if ((damage_taken >= 6 && random2(2) == 0) || random2(30) == 0)
                {
                    drain_exp();
                }
                break;

            case MONS_RED_WASP: // red wasp

                if (player_res_poison() == 0)
                {
                    you.poison += 1 + random2(2);
                }               // no break is intentional

            case MONS_YELLOW_WASP:      // yellow wasp

                strcpy(info, monam(menv[monster_attacking].number, menv[monster_attacking].type, menv[monster_attacking].enchantment[2], 0));
                strcat(info, " stings you.");
                mpr(info);
                if (player_res_poison() == 0 && ((damage_taken >= 3 && random2(3) != 0) || random2(20) == 0))
                {
                    if (you.paralysis > 0)
                    {
                        strcpy(info, "You still can't move!");
                    }
                    else
                        strcpy(info, "You suddenly lose the ability to move!");
                    mpr(info);
                    you.paralysis += random2(3) + 1;
                }
                break;

            case MONS_SPINY_WORM:       // spiny worm

                if (player_res_poison() == 0)
                {
                    strcpy(info, monam(menv[monster_attacking].number, menv[monster_attacking].type, menv[monster_attacking].enchantment[2], 0));
                    strcat(info, " stings you!");
                    mpr(info);
                    you.poison += 2 + random2(4);
                }               // no break is intentional

            case MONS_OOZE:     // ooze

            case MONS_ACID_BLOB:        // acid blob

            case MONS_ROYAL_JELLY:      // royal jelly

            case MONS_JELLY:    // Jelly

                strcpy(info, "You are splashed with acid!");
                mpr(info);
                splash_with_acid(3);
                break;

            case MONS_ICE_DEVIL:        // ice devil

            case MONS_ICE_BEAST:        // Ice beast

            case MONS_FREEZING_WRAITH:          // blue wraith

            case MONS_ICE_FIEND:        // I Fiend

            case MONS_WHITE_IMP:        // lesser demon

            case MONS_ANTAEUS:  // Antaeus

            case MONS_AZURE_JELLY:      // azure jelly

                if (player_res_cold() <= 100)
                {
                    damage_taken += menv[monster_attacking].hit_dice + random2(menv[monster_attacking].hit_dice * 2);
                    strcpy(info, monam(menv[monster_attacking].number, menv[monster_attacking].type, menv[monster_attacking].enchantment[2], 0));
                    strcat(info, " freezes you!");
                    mpr(info);
                }
                else
                {
                    damage_taken += menv[monster_attacking].hit_dice + random2(menv[monster_attacking].hit_dice * 2);
                    damage_taken /= (2 + (player_res_cold() - 100) * (player_res_cold() - 100));
                    strcpy(info, monam(menv[monster_attacking].number, menv[monster_attacking].type, menv[monster_attacking].enchantment[2], 0));
                    strcat(info, " chills you.");
                    mpr(info);
                }

                if (player_res_cold() < 100)
                    damage_taken += menv[monster_attacking].hit_dice + random2(menv[monster_attacking].hit_dice);

                scrolls_burn(1, 8);
                break;

            case MONS_VAMPIRE:  // Vampire

                if (you.is_undead != 0)
                    break;
/*      if (damage_taken >= 7 && random2(3) == 0 || random2(20) == 0)
   {
   strcpy(info, "You feel less resilient.");
   mpr(info);
   you.hp_max -= random2(2) + 1;
   you.redraw_hit_points = 1;
   if (you.hp >= you.hp_max) you.hp = you.hp_max;
   menv [monster_attacking].hit_points += 5 + random2(8);
   if (menv [monster_attacking].hit_points > menv [monster_attacking].max_hit_points) menv [monster_attacking].hit_points = menv [monster_attacking].max_hit_points;
   } */
                strcpy(info, monam(menv[monster_attacking].number, menv[monster_attacking].type, menv[monster_attacking].enchantment[2], 0));
                strcat(info, " is healed.");
                mpr(info);
                menv[monster_attacking].hit_points += random2(damage_taken);
                if (menv[monster_attacking].hit_points > menv[monster_attacking].max_hit_points)
                    menv[monster_attacking].hit_points = menv[monster_attacking].max_hit_points;
                break;

            case MONS_SHADOW:   // shadow
                if (player_prot_life() <= random2(3)
                        && ((damage_taken >= 1 && random2(3) == 0)
                                                    || random2(20) == 0)
                        && you.strength > 3 && player_sust_abil() == 0)
                {
                    strcpy(info, "You feel weaker.");
                    mpr(info);
                    you.strength--;
                    you.redraw_strength = 1;
                }
                break;

            case MONS_HUNGRY_GHOST:     // hungry ghost

                if (you.is_undead == 2)
                    break;
                if (damage_taken >= 1 && random2(2) == 0 || random2(20) == 0)
                {
                    strcpy(info, "You feel hungry.");
                    mpr(info);
                    you.hunger -= 400;
                    //redraw_strength = 1;
                }
                break;


            case MONS_GUARDIAN_NAGA:    // Naga

                break;

            case MONS_PHANTOM:  // phantom

            case MONS_INSUBSTANTIAL_WISP:       // wisp

            case MONS_BLINK_FROG:       // blink frog

            case MONS_MIDGE:    // less demon

                if (random2(3) == 0)
                {
                    strcpy(info, monam(menv[monster_attacking].number, menv[monster_attacking].type, menv[monster_attacking].enchantment[2], 0));
                    strcat(info, " blinks.");
                    mpr(info);
                    monster_blink(monster_attacking);
                }
                break;

            case MWATER3:       // jellyfish

            case MONS_ORANGE_DEMON:
// if (random2(3) != 0) break;
                if (player_res_poison() != 0)
                    break;
                if (menv[monster_attacking].type == MONS_ORANGE_DEMON && (random2(4) != 0 || runthru != 1))
                    break;
                strcpy(info, monam(menv[monster_attacking].number, menv[monster_attacking].type, menv[monster_attacking].enchantment[2], 0));
                strcat(info, " stings you!");
                mpr(info);
                you.poison++;
                if (player_sust_abil() == 0 && you.strength > 3)
                {
                    strcpy(info, "You feel weaker.");
                    mpr(info);
                    you.redraw_strength = 1;
                    you.strength--;
                }
                break;

            case MONS_PULSATING_LUMP:   // pulsating lump

                if (random2(3) == 0)
                    mutate(100);
                break;


            }                   // end of switch for special attacks.

            /* use brek for level drain, maybe with beam variables, because so many creatures use it. */
        }

/* special weapons */
        if (hit == 1 && (menv[monster_attacking].inv[hand_used] != 501 || ((menv[monster_attacking].type == MONS_PLAYER_GHOST || menv[monster_attacking].type == MONS_PANDEMONIUM_DEMON) && ghost.values[8] != 0)))
        {
            unsigned char itdam;

            if (menv[monster_attacking].type == MONS_PLAYER_GHOST || menv[monster_attacking].type == MONS_PANDEMONIUM_DEMON)
            {
                itdam = ghost.values[8];
            }
            else
            {
                itdam = mitm.special[menv[monster_attacking].inv[hand_used]];
            }

            specdam = 0;

            if (itdam == 188)
                goto flaming;   // sword of Okawaru

            if (menv[monster_attacking].type == MONS_PLAYER_GHOST || menv[monster_attacking].type == MONS_PANDEMONIUM_DEMON || mitm.special[menv[monster_attacking].inv[hand_used]] < 180)
                switch (itdam % 30)
                {
                case SPWPN_NORMAL:      // nothing

                    break;

                case SPWPN_FLAMING:     // flaming

                  flaming:specdam = 0;
                    if (player_res_fire() > 100)
                    {
                        damage_taken += (random2(damage_taken) / 2 + 1) / 2 + ((player_res_fire() - 100) * (player_res_fire() - 100));
                    }
                    if (player_res_fire() <= 100)
                    {
                        specdam = random2(damage_taken) / 2 + 1;
                    }
                    if (player_res_fire() < 100)
                    {
                        specdam += random2(damage_taken) / 2 + 1;
                    }
                    if (specdam != 0)
                    {
                        mmov_x = menv[monster_attacking].inv[hand_used];

                        if (menv[monster_attacking].type == MONS_DANCING_WEAPON)
                        {
                            item_name(mitm.pluses2[mmov_x], mitm.base_type[mmov_x], mitm.sub_type[mmov_x], mitm.special[mmov_x], mitm.pluses[mmov_x], mitm.quantity[mmov_x], mitm.id[mmov_x], 0, str_pass);
                            strcpy(info, str_pass);
                        }
                        else
                            strcpy(info, monam(menv[monster_attacking].number, menv[monster_attacking].type, menv[monster_attacking].enchantment[2], 0));
                        strcat(info, " burns you");
                        if (specdam < 3)
                            strcat(info, ".");
                        if (specdam >= 3 && specdam < 7)
                            strcat(info, "!");
                        if (specdam >= 7)
                            strcat(info, "!!");
                        mpr(info);
                    }
                    break;

                case SPWPN_FREEZING:    // freezing

                    specdam = 0;
                    if (player_res_cold() <= 100)
                    {
                        specdam = random2(damage_taken) / 2 + 1;
                    }
                    if (player_res_cold() < 100)
                    {
                        specdam += random2(damage_taken) / 2 + 1;
                    }
                    if (player_res_cold() > 100)
                    {
                        damage_taken += (random2(damage_taken) / 2 + 1) / 2 + ((player_res_cold() - 100) * (player_res_cold() - 100));
                    }
                    if (specdam != 0)
                    {
                        mmov_x = menv[monster_attacking].inv[hand_used];

                        if (menv[monster_attacking].type == MONS_DANCING_WEAPON)
                        {
                            item_name(mitm.pluses2[mmov_x], mitm.base_type[mmov_x], mitm.sub_type[mmov_x], mitm.special[mmov_x], mitm.pluses[mmov_x], mitm.quantity[mmov_x], mitm.id[mmov_x], 0, str_pass);
                            strcpy(info, str_pass);
                        }
                        else
                            strcpy(info, monam(menv[monster_attacking].number, menv[monster_attacking].type, menv[monster_attacking].enchantment[2], 0));
                        strcat(info, " freezes you");
                        if (specdam < 3)
                            strcat(info, ".");
                        if (specdam >= 3 && specdam < 7)
                            strcat(info, "!");
                        if (specdam >= 7)
                            strcat(info, "!!");
                        mpr(info);
                    }
                    break;


                case SPWPN_HOLY_WRATH:          // holy wrath

                    if (menv[monster_attacking].type == MONS_PLAYER_GHOST)
                        break;  /* ghosts can't wield holy wrath */
                    if (you.is_undead != 0)
                    {
                        specdam = random2(damage_taken);
                        if (specdam != 0)
                        {
                            strcpy(info, "The wound is extremely painful");
                            if (specdam < 3)
                                strcat(info, ".");
                            if (specdam >= 3 && specdam < 7)
                                strcat(info, "!");
                            if (specdam >= 7)
                                strcat(info, "!!");
                            mpr(info);
                        }
                    }
                    break;

                case SPWPN_ELECTROCUTION:       // electrocution

                    if (you.levitation != 0)
                        break;  // you're not grounded

                    if (player_res_electricity() > 0)
                        break;  // resist lightning
                    //   if (menv [monster_attacking].type == 400) break;

                    specdam = 0;

                    if (mitm.pluses2[menv[monster_attacking].inv[hand_used]] <= 50 || mitm.pluses2[menv[monster_attacking].inv[hand_used]] > 130 && mitm.pluses2[menv[monster_attacking].inv[hand_used]] <= 150)
                        break;
                    if (random2(3) == 0)
                    {
                        strcpy(info, "You are electrocuted!");
                        mpr(info);
                        specdam += 10 + random2(15);
//    mitm.pluses2 [menv [monster_attacking].inv [hand_used]] --;
                    }
                    break;

                case SPWPN_ORC_SLAYING: // orc slaying

                    if (you.species == SP_HILL_ORC)
                    {
                        specdam = random2(damage_taken);
                        if (specdam != 0)
                        {
                            strcpy(info, "The wound is extremely painful");
                            if (specdam < 3)
                                strcat(info, ".");
                            if (specdam >= 3 && specdam < 7)
                                strcat(info, "!");
                            if (specdam >= 7)
                                strcat(info, "!!");
                            mpr(info);
                        }
                    }
                    break;

                case SPWPN_VENOM:       // venom

                    if (random2(3) == 0 && player_res_poison() == 0)
                    {
                        mmov_x = menv[monster_attacking].inv[hand_used];

                        if (menv[monster_attacking].type == MONS_DANCING_WEAPON)
                        {
                            item_name(mitm.pluses2[mmov_x], mitm.base_type[mmov_x], mitm.sub_type[mmov_x], mitm.special[mmov_x], mitm.pluses[mmov_x], mitm.quantity[mmov_x], mitm.id[mmov_x], 0, str_pass);
                            strcpy(info, str_pass);
                        }
                        else
                            strcpy(info, monam(menv[monster_attacking].number, menv[monster_attacking].type, menv[monster_attacking].enchantment[2], 0));
                        if (menv[monster_attacking].type != MONS_DANCING_WEAPON)
                            strcat(info, "'s weapon is poisoned!");
                        else
                            strcat(info, " is poisoned!");

                        you.poison += 2;
                    }
                    break;

                case SPWPN_PROTECTION:          // protection
                    break;

                case SPWPN_DRAINING:
                    drain_exp();
                    specdam = random2(damage_taken)
                                            / (2 + player_prot_life()) + 1;
                    break;

                case SPWPN_SPEED:       // you.speed

                    menv[monster_attacking].speed_increment -= menv[monster_attacking].speed / 2;
                    break;

                case SPWPN_VORPAL:      // slicing etc

                    specdam = random2(damage_taken) / 2 + 1;
                    break;

                case SPWPN_VAMPIRICISM: // vampiric

                    specdam = 0;        // note does no extra damage

                    if (you.is_undead != 0)
                        break;
                    if (random2(5) == 0 || menv[monster_attacking].hit_points == menv[monster_attacking].max_hit_points)
                        break;
                    menv[monster_attacking].hit_points += random2(damage_taken) + 1;    // thus is probably more valuable on larger weapons?

                    if (menv[monster_attacking].hit_points > menv[monster_attacking].max_hit_points)
                        menv[monster_attacking].hit_points = menv[monster_attacking].max_hit_points;
//   if (you.hunger <= 11000) you.hunger += random() % 30;
                    if (menv[monster_attacking].enchantment[2] != 6 || player_see_invis() != 0)
                    {
                        if (menv[monster_attacking].type == MONS_DANCING_WEAPON)
                        {
                            item_name(mitm.pluses2[mmov_x], mitm.base_type[mmov_x], mitm.sub_type[mmov_x], mitm.special[mmov_x], mitm.pluses[mmov_x], mitm.quantity[mmov_x], mitm.id[mmov_x], 0, str_pass);
                            strcpy(info, str_pass);
                        }
                        else
                            strcpy(info, monam(menv[monster_attacking].number, menv[monster_attacking].type, menv[monster_attacking].enchantment[2], 0));
                        strcat(info, " is healed.");
                        mpr(info);
                    }
                    break;

                case SPWPN_DISRUPTION:          // disruption

                    if (menv[monster_attacking].type == MONS_PLAYER_GHOST)
                        break;
                    if (you.is_undead != 0)
                    {
                        specdam = random2(damage_taken) + random2(damage_taken) + random2(damage_taken) + random2(damage_taken);
                        if (specdam != 0)
                        {
                            strcpy(info, "You are blasted by holy energy");
                            if (specdam < 7)
                                strcat(info, ".");
                            if (specdam >= 7 && specdam < 15)
                                strcat(info, "!");
                            if (specdam >= 15)
                                strcat(info, "!!");
                            mpr(info);
                        }
                    }
                    break;


                case SPWPN_DISTORTION:          // distortion
                    //   if (random2(3) != 0) break;

                    if (random2(3) == 0)
                    {
                        strcpy(info, "Your body is twisted painfully.");
                        mpr(info);
                        specdam += random2(5) + random2(3) + 1;
                        break;
                    }
                    if (random2(3) == 0)
                    {
                        strcpy(info, "Your body is terribly warped!");
                        mpr(info);
                        specdam += random2(12) + random2(13) + 3;
                        break;
                    }
                    if (random2(3) == 0)
                    {
                        random_blink();
                        break;
                    }
                    if (random2(2) == 0)
                    {
                        you_teleport();
                        break;
                    }
                    if (random2(2) == 0)
                    {
                        you_teleport2(1);
                        break;
                    }
                    if (random2(2) == 0)
                    {
                        banished(96);
                        break;
                    }
                    break;


                }               // end of switch

        }                       // end of special weapons

        damage_taken += specdam;

        if (damage_taken > 0 && damage_taken < 150)
        {                       /* ultra-high damages are assumed buggy - I wish there was a more elegant way to fix this (there probably is) */
            ouch(damage_taken, monster_attacking, 0);
            you.redraw_hit_points = 1;
            if (you.religion == GOD_XOM && you.hp <= you.hp_max / 3 && random2(10) == 0)
                Xom_acts(1, you.experience_level, 0);

        }
    }                           // end of for runthru


    return;
}                               // end of void monster_attack(int monster_attacking)










char monsters_fight(int monster_attacking, int monster_attacked)
{
    int damage_taken = 0;
    char hit = 0;
    int mmov_x = 0;
    char str_pass[80];
    char water_attack = 0;
    int specdam = 0;

    int hand_used = 0;

    int sees = 0;

    if (menv[monster_attacking].type == MONS_GIANT_SPORE)
    {
        menv[monster_attacking].hit_points = -1;
        return 0;
    }
    if (menv[monster_attacking].type >= MLAVA0 && menv[monster_attacking].number == 1 && menv[monster_attacked].type < MLAVA0)
        return 0;

    if (menv[monster_attacking].behavior == BEH_ENSLAVED)
    {
        if (menv[monster_attacked].behavior == BEH_ENSLAVED)
            return 0;
        menv[monster_attacking].monster_foe = monster_attacked;
    }
    if (menv[monster_attacked].behavior == BEH_FIGHT)
    {
        if (menv[monster_attacked].behavior != BEH_ENSLAVED && monster_attacked != menv[monster_attacking].monster_foe)
            return 0;
    }
    if (grd[menv[monster_attacking].x][menv[monster_attacking].y] == 65 && mons_flies(menv[monster_attacking].type) == 0 && menv[monster_attacking].type < MLAVA0)
    {
        if (random2(4) == 0)
        {
            mpr("You hear a splashing noise.");
            return 1;
        }
    }
    if (grd[menv[monster_attacked].x][menv[monster_attacked].y] == 65 && mons_flies(menv[monster_attacked].type) == 0 && menv[monster_attacking].type >= MWATER0)
        water_attack = 1;

    if (mons_near(monster_attacking) && mons_near(monster_attacked))
        sees = 1;

    char runthru;

    for (runthru = 0; runthru < 4; runthru++)
    {

        char mdam = mondamage(menv[monster_attacking].type, runthru);

        if (menv[monster_attacking].type == MONS_SMALL_ZOMBIE || menv[monster_attacking].type == MONS_BIG_ZOMBIE || menv[monster_attacking].type == MONS_SMALL_SKELETON || menv[monster_attacking].type == MONS_LARGE_SKELETON || menv[monster_attacking].type == MONS_SPECTRAL_THING)
        {
            mdam = mondamage(menv[monster_attacking].number, runthru);
            if (mdam > 1 && mdam < 4)
            {
                mdam--;
            }
            if (mdam > 3 && mdam < 8)
            {
                mdam -= 2;
            }
            if (mdam > 7 && mdam < 13)
            {
                mdam -= 3;
            }
            if (mdam > 12)
            {
                mdam -= 5;
            }
        }
        if (mdam == 0)
            break;

        if (menv[monster_attacking].type == MONS_TWO_HEADED_OGRE && runthru == 1 && menv[monster_attacking].inv[1] != 501)
        {
            hand_used = 1;
        }
        damage_taken = 0;


        int mons_to_hit = 20 + menv[monster_attacking].hit_dice;        // * menv [monster_attacking].hit_dice; // * 3  //you.strength + (0.5 * you.you.max_dex) + (you.f_abil);

        mons_to_hit += 5 * water_attack;

        if (menv[monster_attacking].inv[hand_used] != 501)
        {
            if (mitm.pluses[menv[monster_attacking].inv[hand_used]] - 50 > 130)
            {
                mons_to_hit += mitm.pluses[menv[monster_attacking].inv[hand_used]] - 50 - 100;  //random2(mitm.pluses [menv [monster_attacking].inv [hand_used]] - 50 - 100 + 1);

            }
            else
            {
                mons_to_hit += mitm.pluses[menv[monster_attacking].inv[hand_used]] - 50;        //random2(mitm.pluses [menv [monster_attacking].inv [hand_used]] - 50 + 1); /// 10;

            }

            mons_to_hit += 3 * property(OBJ_WEAPONS, mitm.sub_type[menv[monster_attacking].inv[0]], PWPN_HIT);

            menv[monster_attacking].speed_increment -= ((property(you.inv_class[you.equip[EQ_WEAPON]], you.inv_type[you.equip[EQ_WEAPON]], PWPN_SPEED) - 10) / 2);      // / 10);

        }
        mons_to_hit = random2(mons_to_hit);


        if (menv[monster_attacking].type == MONS_KILLER_BEE || menv[monster_attacking].type == MONS_WORM)
        {
            menv[monster_attacking].speed_increment -= 10;      //--

        }
        if (mons_to_hit >= menv[monster_attacked].evasion || ((menv[monster_attacked].speed_increment <= 60 || menv[monster_attacked].behavior == BEH_SLEEP) && random2(20) != 0))
        {
            hit = 1;


            if (menv[monster_attacking].inv[hand_used] != 501 && mitm.base_type[menv[monster_attacking].inv[hand_used]] == OBJ_WEAPONS && (mitm.sub_type[menv[monster_attacking].inv[hand_used]] < WPN_SLING || mitm.sub_type[menv[monster_attacking].inv[hand_used]] > WPN_HAND_CROSSBOW))
            {

                damage_taken = random2(property(mitm.base_type[menv[monster_attacking].inv[0]], mitm.sub_type[menv[monster_attacking].inv[0]], PWPN_DAMAGE));

                if (mitm.special[menv[monster_attacking].inv[hand_used]] / 30 == DWPN_ORCISH && mons_charclass(menv[monster_attacking].type) == MONS_ORC)
                {
                    damage_taken += random2(2);
                }
//if (mitm.pluses [mons_inv [i] [0]] > 80) damage_taken -= 100;
                //damage_taken += mitm.pluses [mons_inv [i] [0]];
                if (mitm.pluses2[menv[monster_attacking].inv[hand_used]] > 130)
                {
                    if (mitm.pluses2[menv[monster_attacking].inv[hand_used]] - 150 >= 0)        /* + or 0 to-dam */
                        damage_taken += random2(mitm.pluses2[menv[monster_attacking].inv[hand_used]] - 150 + 1);
                    else
                        damage_taken -= random2(abs(mitm.pluses2[menv[monster_attacking].inv[hand_used]] - 150 + 1));   /* - to-dam */
                }
                else
                {
                    if (mitm.pluses2[menv[monster_attacking].inv[hand_used]] - 50 >= 0)         /* + or 0 to-dam */
                        damage_taken += random2(mitm.pluses2[menv[monster_attacking].inv[hand_used]] - 50 + 1);
                    else
                        damage_taken -= random2(abs(mitm.pluses2[menv[monster_attacking].inv[hand_used]] - 50 + 1));    /* - to-dam */
                }
/*        if (mitm.pluses2 [menv [monster_attacking].inv [hand_used]] > 130)
   {
   damage_taken += random2(mitm.pluses2 [menv [monster_attacking].inv [hand_used]] - 150 + 1);
   } else
   {
   damage_taken += random2(mitm.pluses2 [menv [monster_attacking].inv [hand_used]] - 50 + 1);// % 10;
   } */
                damage_taken -= random2(3) + 1;         //1;

            }
            damage_taken += random2(mdam) + 1;

            damage_taken *= water_attack + 1;

//gmon_att [menv [monster_attacking].type] [runthru]) + 1;
            //damage_taken /= (random2 (player_AC()) + 1); // / 3
            //damage_taken *= 2;

//if (menv [monster_attacking].inv [0] != 501)
            //{
            //      damage_taken /= ((random2 (player_AC()) / property [mitm.base_type [mons_inv [i] [0]]] [mitm.sub_type [mons_inv [i] [0]]] [2]) + 1);
            //} else
            //      damage_taken /= (random2 (player_AC()) + 1);

            damage_taken -= random2(menv[monster_attacked].armor_class + 1);

            if (damage_taken < 1)
                damage_taken = 0;
//you.hp -= (int) damage_taken;
            //ouch(damage_taken, monster_attacking, 0);

//you.redraw_hit_points = 1;

        }                       // end of if mons_to_hit >= player_evasion()
        else
        {
            hit = 0;
//      strcpy(info, "The ");
            //} else
            //strcat(info, "The ");

            int mmov_x = menv[monster_attacking].inv[hand_used];

            char str_pass[80];

            if (menv[monster_attacking].type == MONS_DANCING_WEAPON)
            {
                item_name(mitm.pluses2[mmov_x], mitm.base_type[mmov_x], mitm.sub_type[mmov_x], mitm.special[mmov_x], mitm.pluses[mmov_x], mitm.quantity[mmov_x], mitm.id[mmov_x], 0, str_pass);
                strcpy(info, str_pass);
            }
            else
                strcpy(info, monam(menv[monster_attacking].number, menv[monster_attacking].type, menv[monster_attacking].enchantment[2], 0));   //gmon_name [menv [monster_attacking].type]);

            strcat(info, " misses ");
            strcat(info, monam(menv[monster_attacked].number, menv[monster_attacked].type, menv[monster_attacked].enchantment[2], 1));
            strcat(info, ".");
            if (sees == 1)
                mpr(info);
        }


        if (damage_taken < 1 && hit == 1)
        {
//if (78 - strlen(info) < strlen(gmon_name [menv [monster_attacking].type]) + 17)
            //      {
            //mpr(info);
            //      strcpy(info, "The ");
            //} else
            //strcat(info, "The ");


            int mmov_x = menv[monster_attacking].inv[hand_used];
            char str_pass[80];

            if (menv[monster_attacking].type == MONS_DANCING_WEAPON)
            {
                item_name(mitm.pluses2[mmov_x], mitm.base_type[mmov_x], mitm.sub_type[mmov_x], mitm.special[mmov_x], mitm.pluses[mmov_x], mitm.quantity[mmov_x], mitm.id[mmov_x], 0, str_pass);
                strcpy(info, str_pass);
            }
            else
                strcpy(info, monam(menv[monster_attacking].number, menv[monster_attacking].type, menv[monster_attacking].enchantment[2], 0));   //gmon_name [menv [monster_attacking].type]);

            strcat(info, " hits ");
            strcat(info, monam(menv[monster_attacked].number, menv[monster_attacked].type, menv[monster_attacked].enchantment[2], 1));
#ifdef WIZARD
            itoa(damage_taken, st_prn, 10);
            strcat(info, " for ");
            strcat(info, st_prn);       // note: doesn't take account of special weapons etc
#endif
            strcat(info, ".");  // but doesn't do any you.damage.");

            if (sees == 1)
                mpr(info);
        }
        if (hit == 1)           //(int) damage_taken >= 1)

        {
//if (78 - strlen(info) < strlen(gmon_name [menv [monster_attacking].type]) + 15)
            //      {
            //      mpr(info);
            //      strcpy(info, "The ");
            //      } else
            //      strcpy(info, "The ");

            int mmov_x = menv[monster_attacking].inv[hand_used];

            if (menv[monster_attacking].type == MONS_DANCING_WEAPON)
            {
                char str_pass[80];

                item_name(mitm.pluses2[mmov_x], mitm.base_type[mmov_x], mitm.sub_type[mmov_x], mitm.special[mmov_x], mitm.pluses[mmov_x], mitm.quantity[mmov_x], mitm.id[mmov_x], 0, str_pass);
                strcpy(info, str_pass);
            }
            else
                strcpy(info, monam(menv[monster_attacking].number, menv[monster_attacking].type, menv[monster_attacking].enchantment[2], 0));
            strcat(info, " hits ");
            strcat(info, monam(menv[monster_attacked].number, menv[monster_attacked].type, menv[monster_attacked].enchantment[2], 1));


/*        strcat(info, " for ");
   itoa(damage_taken, st_prn, 10);
   strcat(info, st_prn);
 */


//      itoa(

            if (menv[monster_attacking].type != MONS_DANCING_WEAPON && menv[monster_attacking].inv[hand_used] != 501 && mitm.base_type[menv[monster_attacking].inv[hand_used]] == OBJ_WEAPONS && (mitm.sub_type[menv[monster_attacking].inv[hand_used]] < WPN_SLING || mitm.sub_type[menv[monster_attacking].inv[hand_used]] > WPN_HAND_CROSSBOW))
            {
                strcat(info, " with ");

                item_name(mitm.pluses2[mmov_x], mitm.base_type[mmov_x], mitm.sub_type[mmov_x], mitm.special[mmov_x], mitm.pluses[mmov_x], mitm.quantity[mmov_x], mitm.id[mmov_x], 3, str_pass);         // was 7

                strcat(info, str_pass);
            }
            strcat(info, "! "); // put something for '.', '!', '!!' etc depending on what % of you.damage you took.

            if (sees == 1)
                mpr(info);

            // special attacks:

            /* brek = 0; */



            switch (menv[monster_attacking].type)
            {
            case MONS_GIANT_ANT:        // giant ant

            case MONS_WOLF_SPIDER:
            case MONS_REDBACK:
            case MONS_SPINY_WORM:       // spiny worm

            case MWATER3:       // jellyfish

            case MONS_ORANGE_DEMON:     // demon

                if ((damage_taken >= 4 && random2(4) == 0) || random2(20) == 0 || menv[monster_attacking].type == MONS_SPINY_WORM)
                {
                    strcpy(info, monam(menv[monster_attacking].number, menv[monster_attacking].type, menv[monster_attacking].enchantment[2], 0));
                    strcat(info, " stings ");
                    strcat(info, monam(menv[monster_attacked].number, menv[monster_attacked].type, menv[monster_attacked].enchantment[2], 1));
                    strcat(info, ".");
                    if (sees == 1)
                        mpr(info);
                    //        you.poison++;
                    poison_monster(monster_attacked, 1);
                }
                break;

            case MONS_CENTAUR:  // cockatrice.

                break;

            case MONS_KILLER_BEE:       // killer bee

            case MONS_BUMBLEBEE:
                if (((damage_taken >= 3 && random2(3) == 0) || random2(20) == 0))
                {
                    strcpy(info, monam(menv[monster_attacking].number, menv[monster_attacking].type, menv[monster_attacking].enchantment[2], 0));
                    strcat(info, " stings ");
                    strcat(info, monam(menv[monster_attacked].number, menv[monster_attacked].type, menv[monster_attacked].enchantment[2], 1));
                    strcat(info, ".");
                    if (sees == 1)
                        mpr(info);
                    // you.poison += 1;
                    poison_monster(monster_attacked, 1);
                }
                break;

            case MONS_NECROPHAGE:       // necrophage

            case MONS_ROTTING_DEVIL:    /* rotting devil */
            case MONS_GHOUL:    /* ghoul */
            case MONS_DEATH_OOZE:       // death ooze

                if (mons_holiness(menv[monster_attacked].type) >= MH_UNDEAD)
                    break;
                if ((damage_taken >= 3 && random2(3) == 0) || random2(20) == 0)
                {
                    //strcpy(info, "You feel your flesh start to rot away!");
                    //mpr(info);
                    //you.poison += random2(3) + 1;
                    menv[monster_attacked].max_hit_points -= random2(3) + 1;
                    if (menv[monster_attacked].hit_points > menv[monster_attacked].max_hit_points)
                        menv[monster_attacked].hit_points = menv[monster_attacked].max_hit_points;
                }
                break;

            case MONS_FIRE_VORTEX:      // fire_vortex

                menv[monster_attacking].hit_points = -10;
            case MONS_FIRE_ELEMENTAL:   // fire elem

            case MONS_BALRUG:   // balrug

            case MONS_SUN_DEMON:        // sun demon

                specdam = 0;
                if (mons_res_fire(menv[monster_attacked].type) == 0)
                {
                    specdam = random2(15) + 15;
                    if (menv[monster_attacked].inv[2] != 501 && mitm.special[menv[monster_attacked].inv[2]] % 30 == 2)
                        specdam /= 3;
                }
                if (mons_res_fire(menv[monster_attacked].type) == -1 || (menv[monster_attacked].inv[2] == 501 || mitm.special[menv[monster_attacked].inv[2]] % 30 != 2))
                {
                    specdam = random2(25) + 20;
                }
                if (specdam != 0)
                {
                    if (sees == 1)
                    {
                        strcpy(info, monam(menv[monster_attacked].number, menv[monster_attacked].type, menv[monster_attacked].enchantment[2], 0));
                        strcat(info, " is engulfed in flame!");
                        //if (sees == 1)
                        mpr(info);
                    }
                }
                damage_taken += specdam;
                break;

            case MONS_QUEEN_BEE:        // queen bee

            case MONS_GIANT_CENTIPEDE:          // giant centipede

            case MONS_SOLDIER_ANT:      // soldier ant

            case MONS_QUEEN_ANT:        /* Queen ant */
//      if ((damage_taken >= 3 && random2(3) == 0) || random2(20) == 0)
                //      {
                strcpy(info, monam(menv[monster_attacking].number, menv[monster_attacking].type, menv[monster_attacking].enchantment[2], 0));
                strcat(info, " stings ");
                strcat(info, monam(menv[monster_attacked].number, menv[monster_attacked].type, menv[monster_attacked].enchantment[2], 1));
                strcat(info, ".");
                if (sees == 1)
                    mpr(info);
                poison_monster(monster_attacked, 1);
//      you.poison += 2;
                //      }
                break;


            case MONS_SCORPION: // snake

            case MONS_BROWN_SNAKE:      // br snake

            case MONS_BLACK_SNAKE:      // black snake

            case MONS_YELLOW_SNAKE:     // yellow snake

            case MONS_GOLD_MIMIC:
            case MONS_WEAPON_MIMIC:
            case MONS_ARMOUR_MIMIC:
            case MONS_SCROLL_MIMIC:
            case MONS_POTION_MIMIC:     /* Mimics */
                if ((damage_taken >= 3 && random2(4) == 0) || random2(20) == 0)
                {
                    strcpy(info, monam(menv[monster_attacked].number, menv[monster_attacked].type, menv[monster_attacked].enchantment[2], 0));
                    strcat(info, " is poisoned.");
                    if (sees == 1)
                        mpr(info);
                    poison_monster(monster_attacked, 1);
                }
                break;

            case MONS_SHADOW_DRAGON:    // shadow dragon

            case MONS_SPECTRAL_THING:   /* spectral thing */
                if (random2(2) == 0)
                    break;
            case MONS_WIGHT:    // wight

            case MONS_WRAITH:   // wraith

            case MONS_SOUL_EATER:       // soul eater

            case MONS_SHADOW_FIEND:     // sh F

            case MONS_SPECTRAL_WARRIOR: // spectre

            case MONS_ORANGE_RAT:       // orange rat

            case MONS_ANCIENT_LICH:     // ancient lich

                if (mons_holiness(menv[monster_attacked].type) >= MH_UNDEAD)
                    break;
                if ((damage_taken >= 6 && random2(2) == 0) || random2(30) == 0)
                {
                    strcpy(info, monam(menv[monster_attacked].number, menv[monster_attacked].type, menv[monster_attacked].enchantment[2], 0));
                    strcat(info, " is drained.");
                    if (sees == 1)
                        mpr(info);
                    if (random2(5) == 0)
                        menv[monster_attacked].hit_dice--;
                    menv[monster_attacked].max_hit_points -= 2 + random2(3);
                    menv[monster_attacked].hit_points -= 2 + random2(3);
                    if (menv[monster_attacked].hit_points >= menv[monster_attacked].max_hit_points)
                        menv[monster_attacked].hit_points = menv[monster_attacked].max_hit_points;
                    if (menv[monster_attacked].hit_points <= 0 || menv[monster_attacked].hit_dice <= 0)
                    {
                        monster_die(monster_attacked, 2, monster_attacking);
                        return 1;
                    }
                    //brek = 1;
                }
                break;

            case MONS_WORM:     // giant wasp
                /*      if ((damage_taken >= 3 && random2(3) == 0) || random2(20) == 0)
                   {
                   if (you.paralysis > 0)
                   {
                   strcpy(info, "You still can't move!");
                   } else strcpy(info, "You suddenly lose the ability to move!");
                   mpr(info);
                   you.paralysis += random2(3) + 1;
                   } */
                break;

            case MONS_JELLY:    // Jelly
                //      strcpy(info, "You are splashed with acid!");
                //      mpr(info);
                //      splash_with_acid(10);

                break;




            case MONS_ICE_DEVIL:        // ice devil

            case MONS_ICE_BEAST:        // Ice beast

            case MONS_FREEZING_WRAITH:          // blue wraith

            case MONS_ICE_FIEND:        // I Fiend

            case MONS_WHITE_IMP:        // lesser demon

            case MONS_AZURE_JELLY:      // azure jelly

            case MONS_ANTAEUS:  // Antaeus

                specdam = 0;
                if (mons_res_cold(menv[monster_attacked].type) == 0)
                {
                    specdam = random2(menv[monster_attacking].hit_dice * 2) + menv[monster_attacking].hit_dice;
                    if (menv[monster_attacked].inv[2] != 501 && mitm.special[menv[monster_attacked].inv[2]] % 30 == 3)
                        specdam = (random2(menv[monster_attacking].hit_dice * 2) + menv[monster_attacking].hit_dice) / 3;
                }
                if (mons_res_cold(menv[monster_attacked].type) == -1 && (menv[monster_attacked].inv[2] == 501 || mitm.special[menv[monster_attacked].inv[2]] % 30 != 3))
                {
                    specdam = random2(menv[monster_attacking].hit_dice * 3) + menv[monster_attacking].hit_dice * 2;
                }
                if (specdam != 0)
                {
                    if (sees == 1)
                    {
                        strcpy(info, monam(menv[monster_attacking].number, menv[monster_attacking].type, menv[monster_attacking].enchantment[2], 0));
                        strcat(info, " freezes ");
                        strcat(info, monam(menv[monster_attacked].number, menv[monster_attacked].type, menv[monster_attacked].enchantment[2], 1));
                        strcat(info, ".");
                        if (sees == 1)
                            mpr(info);
                    }
                }
                damage_taken += specdam;
                break;

            case MONS_VAMPIRE:  // Vampire
                /* if (damage_taken >= 7 && random2(2) == 0 || random2(10) == 0)
                   menv [monster_attacked].max_hit_points -= 3;
                   if (menv [monster_attacked].hit_points >= menv [monster_attacked].max_hit_points) menv [monster_attacked].hit_points = menv [monster_attacked].max_hit_points;
                   menv [monster_attacking].hit_points += random2(8);
                   if (menv [monster_attacking].hit_points > menv [monster_attacking].max_hit_points) menv [monster_attacking].hit_points = menv [monster_attacking].max_hit_points; */
                if (mons_holiness(menv[monster_attacked].type) >= MH_UNDEAD)
                    break;
                strcpy(info, monam(menv[monster_attacking].number, menv[monster_attacking].type, menv[monster_attacking].enchantment[2], 0));
                strcat(info, " is healed.");
                if (mons_near(monster_attacking))
                    mpr(info);
                menv[monster_attacking].hit_points += random2(damage_taken);
                if (menv[monster_attacking].hit_points > menv[monster_attacking].max_hit_points)
                    menv[monster_attacking].hit_points = menv[monster_attacking].max_hit_points;
                break;




            case MONS_GUARDIAN_NAGA:    // Naga

                break;


            }                   // end of switch for special attacks.


        }
/* special weapons */
        if (hit == 1 && (menv[monster_attacking].inv[hand_used] != 501 || ((menv[monster_attacking].type == MONS_PLAYER_GHOST || menv[monster_attacking].type == MONS_PANDEMONIUM_DEMON) && ghost.values[8] != 0)))
        {
            unsigned char itdam;

            if (menv[monster_attacking].type == MONS_PLAYER_GHOST || menv[monster_attacking].type == MONS_PANDEMONIUM_DEMON)
            {
                itdam = ghost.values[8];
            }
            else
            {
                itdam = mitm.special[menv[monster_attacking].inv[hand_used]];
            }

            specdam = 0;

            if (itdam == 188)
                goto flaming;   // sword of Okawaru

            if (menv[monster_attacking].type == MONS_PLAYER_GHOST || menv[monster_attacking].type == MONS_PANDEMONIUM_DEMON || mitm.special[menv[monster_attacking].inv[hand_used]] < 180)
                switch (itdam % 30)
                {

                case SPWPN_NORMAL:      // nothing

                    break;

                case SPWPN_FLAMING:     // flaming

                  flaming:specdam = 0;
                    if (mons_res_fire(menv[monster_attacked].type) == 0)
                    {
                        specdam = random2(damage_taken) / 2 + 1;
                        if (menv[monster_attacked].inv[2] != 501 && mitm.special[menv[monster_attacked].inv[2]] % 30 == 2)
                            specdam /= 3;
                    }
                    if (mons_res_fire(menv[monster_attacked].type) == -1 && (menv[monster_attacked].inv[2] == 501 || mitm.special[menv[monster_attacked].inv[2]] % 30 != 2))
                    {
                        specdam = random2(damage_taken) + 1;
                    }
                    if (specdam != 0)
                    {
                        if (sees == 1)
                        {
                            int mmov_x = menv[monster_attacking].inv[hand_used];
                            char str_pass[80];

                            if (menv[monster_attacking].type == MONS_DANCING_WEAPON)
                            {
                                item_name(mitm.pluses2[mmov_x], mitm.base_type[mmov_x], mitm.sub_type[mmov_x], mitm.special[mmov_x], mitm.pluses[mmov_x], mitm.quantity[mmov_x], mitm.id[mmov_x], 0, str_pass);
                                strcpy(info, str_pass);
                            }
                            else
                                strcpy(info, monam(menv[monster_attacking].number, menv[monster_attacking].type, menv[monster_attacking].enchantment[2], 0));
                            strcat(info, " burns ");
                            strcat(info, monam(menv[monster_attacked].number, menv[monster_attacked].type, menv[monster_attacked].enchantment[2], 1));
                            if (specdam < 3)
                                strcat(info, ".");
                            if (specdam >= 3 && specdam < 7)
                                strcat(info, "!");
                            if (specdam >= 7)
                                strcat(info, "!!");
                            //    strcat(info, " is burned.");
                            mpr(info);
                        }
                    }
                    break;

                case SPWPN_FREEZING:    // freezing

                    specdam = 0;
                    if (mons_res_cold(menv[monster_attacked].type) == 0)
                    {
                        specdam = random2(damage_taken) / 2 + 1;
                        if (menv[monster_attacked].inv[2] != 501 && mitm.special[menv[monster_attacked].inv[2]] % 30 == 3)
                            specdam = (random2(damage_taken) / 2 + 1) / 3;
                    }
                    if (mons_res_cold(menv[monster_attacked].type) == -1 && (menv[monster_attacked].inv[2] == 501 || mitm.special[menv[monster_attacked].inv[2]] % 30 != 3))
                    {
                        specdam = random2(damage_taken) + 1;
                    }
                    if (specdam != 0)
                    {
                        if (sees == 1)
                        {
                            mmov_x = menv[monster_attacking].inv[hand_used];

                            if (menv[monster_attacking].type == MONS_DANCING_WEAPON)
                            {
                                item_name(mitm.pluses2[mmov_x], mitm.base_type[mmov_x], mitm.sub_type[mmov_x], mitm.special[mmov_x], mitm.pluses[mmov_x], mitm.quantity[mmov_x], mitm.id[mmov_x], 0, str_pass);
                                strcpy(info, str_pass);
                            }
                            else
                                strcpy(info, monam(menv[monster_attacking].number, menv[monster_attacking].type, menv[monster_attacking].enchantment[2], 0));
                            strcat(info, " freezes ");
                            strcat(info, monam(menv[monster_attacked].number, menv[monster_attacked].type, menv[monster_attacked].enchantment[2], 1));
                            if (specdam < 3)
                                strcat(info, ".");
                            if (specdam >= 3 && specdam < 7)
                                strcat(info, "!");
                            if (specdam >= 7)
                                strcat(info, "!!");
                            //    strcat(info, " is burned.");
                            mpr(info);
                        }
                    }
                    break;

                case SPWPN_HOLY_WRATH:          // holy wrath

                    if (menv[monster_attacking].type == MONS_PLAYER_GHOST)
                        break;
                    specdam = 0;
                    switch (mons_holiness(menv[monster_attacked].type))
                    {
                    case -1:
//    strcpy(info, "
                        damage_taken -= 5 + random2(5);
                        break;

                    case 1:
                        specdam += random2(damage_taken) + 1;
                        break;

                    case 2:
                        specdam += (random2(damage_taken) * 15) / 10 + 1;       // does * 1.5 do anything?

                        break;
                    }
                    break;

                case SPWPN_ELECTROCUTION:       // electrocution
                    //   if (you.levitation != 0) break; // you're not grounded
                    //   if (menv [monster_attacking].type == 400) break;

                    if (mons_flies(menv[monster_attacked].type))
                        break;
                    if (mons_res_elec(menv[monster_attacked].type) != 0)
                        break;
                    specdam = 0;

                    if (mitm.pluses2[menv[monster_attacking].inv[hand_used]] <= 50 || mitm.pluses2[menv[monster_attacking].inv[hand_used]] > 130 && mitm.pluses2[menv[monster_attacking].inv[hand_used]] <= 150)
                        break;
                    if (random2(3) == 0)
                    {
                        strcpy(info, "There is a sudden explosion of sparks!");
                        if (sees == 1)
                            mpr(info);
                        specdam += 10 + random2(15);
//    mitm.pluses2 [menv [monster_attacking].inv [hand_used]] --;
                    }
                    break;

                case SPWPN_ORC_SLAYING: // orc slaying

                    if (mons_charclass(menv[monster_attacked].type) == MONS_ORC)
//   if (menv [monster_attacked].type == 14 || menv [monster_attacked].type == 52 || menv [monster_attacked].type == 54 || menv [monster_attacked].type == 55 || menv [monster_attacked].type == 103)
                    {
                        menv[monster_attacked].hit_points -= 1 + random2(damage_taken);
                    }
                    break;

                case SPWPN_VENOM:       // venom

                    if (random2(3) == 0)
                        poison_monster(monster_attacked, 0);
                    break;

//   case 7: // protection
                case SPWPN_DRAINING:
                    if (mons_holiness(menv[monster_attacked].type) > MH_NORMAL && ((damage_taken >= 6 && random2(2) == 0) || random2(30) == 0))
                    {
                        strcpy(info, monam(menv[monster_attacked].number, menv[monster_attacked].type, menv[monster_attacked].enchantment[2], 0));
                        strcat(info, " is drained.");
                        if (sees == 1)
                            mpr(info);
                        if (random2(5) == 0)
                            menv[monster_attacked].hit_dice--;
                        menv[monster_attacked].max_hit_points -= 2 + random2(3);
                        menv[monster_attacked].hit_points -= 2 + random2(3);
                        if (menv[monster_attacked].hit_points >= menv[monster_attacked].max_hit_points)
                            menv[monster_attacked].hit_points = menv[monster_attacked].max_hit_points;
                        if (menv[monster_attacked].hit_points <= 0 || menv[monster_attacked].hit_dice <= 0)
                        {
                            monster_die(monster_attacked, 2, monster_attacking);
                            return 1;
                        }
                        specdam = random2(damage_taken) / 2 + 1;
                    }
                    break;

                case SPWPN_SPEED:       // you.speed

                    menv[monster_attacking].speed_increment -= menv[monster_attacking].speed / 2;
                    break;

                case SPWPN_VORPAL:      // slicing etc

                    specdam += (random2(damage_taken) / 2) + 1;
                    break;

                case SPWPN_VAMPIRICISM: // vampiric

                    specdam = 0;        // note does no extra damage

                    if (mons_holiness(menv[monster_attacked].type) > MH_NORMAL)
                        break;
                    if (random2(5) == 0 || menv[monster_attacking].hit_points == menv[monster_attacking].max_hit_points)
                        break;
                    menv[monster_attacking].hit_points += random2(damage_taken) + 1;    // thus is probably more valuable on larger weapons?

                    if (menv[monster_attacking].hit_points > menv[monster_attacking].max_hit_points)
                        menv[monster_attacking].hit_points = menv[monster_attacking].max_hit_points;
//   if (you.hunger <= 11000) you.hunger += random() % 30;
                    if (mons_near(monster_attacking) && (menv[monster_attacking].enchantment[2] != 6 || player_see_invis() != 0))
                    {
                        if (menv[monster_attacking].type == MONS_DANCING_WEAPON)
                        {
                            item_name(mitm.pluses2[mmov_x], mitm.base_type[mmov_x], mitm.sub_type[mmov_x], mitm.special[mmov_x], mitm.pluses[mmov_x], mitm.quantity[mmov_x], mitm.id[mmov_x], 0, str_pass);
                            strcpy(info, str_pass);
                        }
                        else
                            strcpy(info, monam(menv[monster_attacking].number, menv[monster_attacking].type, menv[monster_attacking].enchantment[2], 0));
                        strcat(info, " is healed.");
                        mpr(info);
                    }
                    break;

                case SPWPN_DISRUPTION:          // mace of disruption

                    if (menv[monster_attacking].type == MONS_PLAYER_GHOST)
                        break;
                    specdam = 0;
                    if (mons_holiness(menv[monster_attacked].type) == MH_UNDEAD && random2(3) != 0)
                    {
                        strcpy(info, monam(menv[monster_attacked].number, menv[monster_attacked].type, menv[monster_attacked].enchantment[2], 0));
                        strcat(info, " shudders.");
                        if (mons_near(monster_attacked))
                            mpr(info);
                        specdam += random2(damage_taken + 1);
                        specdam += random2(damage_taken + 1);
                        specdam += random2(damage_taken + 1);
/*    if (menv [monster_attacked].enchantment [1] >= 20 && menv [monster_attacked].enchantment [1] <= 25)
   menv [monster_attacked].enchantment [1] -= 1;
   if (menv [monster_attacked].enchantment [1] == 19) menv [monster_attacked].enchantment [1] == 20; */
                    }
                    break;


                case SPWPN_DISTORTION:          // distortion
                    //   if (random2(3) != 0) break;

                    if (random2(3) == 0)
                    {
                        strcpy(info, "Space bends around ");
                        strcat(info, monam(menv[monster_attacked].number, menv[monster_attacked].type, menv[monster_attacked].enchantment[2], 1));
                        strcat(info, ".");
                        if (mons_near(monster_attacked))
                            mpr(info);
                        specdam += random2(5) + random2(3) + 1;
                        break;
                    }
                    if (random2(3) == 0)
                    {
                        strcpy(info, "Space warps horribly around ");
                        strcat(info, monam(menv[monster_attacked].number, menv[monster_attacked].type, menv[monster_attacked].enchantment[2], 1));
                        strcat(info, "!");
                        if (mons_near(monster_attacked))
                            mpr(info);
                        specdam += random2(12) + random2(13) + 3;
                        break;
                    }
                    if (random2(3) == 0)
                    {
                        monster_blink(monster_attacked);
                        break;
                    }
                    if (random2(2) == 0)
                    {
                        monster_teleport(monster_attacked, 0);
                        break;
                    }
                    if (random2(2) == 0)
                    {
                        monster_teleport(monster_attacked, 1);
                        break;
                    }
                    if (random2(2) == 0)
                    {
                        monster_die(monster_attacked, 6, monster_attacking);
                        break;
                    }
                    break;



                }
        }                       // end of if special weapon

        damage_taken += specdam;

        if (damage_taken > 0)
        {
            //ouch(damage_taken, monster_attacking, 0);
            //you.redraw_hit_points = 1;
            menv[monster_attacked].hit_points -= damage_taken;
            if (menv[monster_attacked].hit_points <= 0)
            {
                monster_die(monster_attacked, 2, monster_attacking);
                return 1;
            }
        }
    }                           // end of for runthru

    if (menv[monster_attacked].behavior == BEH_ENSLAVED)
    {
//   menv [monster_attacked].behavior = 6;
        menv[monster_attacked].monster_foe = monster_attacking;
        menv[monster_attacking].monster_foe = monster_attacked;
//  }
    }
    if (menv[monster_attacking].behavior != BEH_CONFUSED)       // && menv [monster_attacking].behavior != 7) // the latter is so that mons_beh [attacker] doesn't get reset to 6

    {
        if (menv[monster_attacked].behavior <= BEH_WANDER && distance(you.x_pos, menv[monster_attacked].x, you.y_pos, menv[monster_attacked].y) > 1)
        {
            menv[monster_attacked].behavior = BEH_FIGHT;
            menv[monster_attacked].monster_foe = monster_attacking;     // This'll do for now.

        }
    }
    return 1;
}                               // end of void monsters_fight(int monster_attacking, ...)



void monster_die(int monster_killed, char killer, int i)
{
    int dmi;                    /* dead monster's inventory */

    if (you.prev_targ == monster_killed)
        you.prev_targ = MHITNOT;

    if (menv[monster_killed].type == MONS_GIANT_SPORE)
    {
        if (menv[monster_killed].hit_points < 1 && menv[monster_killed].hit_points > -15)
        {
            return;
        }
    }
    if (menv[monster_killed].type == MONS_FIRE_VORTEX || menv[monster_killed].type == MONS_SPATIAL_VORTEX)
    {
        strcpy(info, monam(menv[monster_killed].number, menv[monster_killed].type, menv[monster_killed].enchantment[2], 0));
        strcat(info, " dissipates!");   // <- spelling?

        if (mons_near(monster_killed))
            mpr(info);
        if (YOU_KILL(killer))
        {
            gain_exp(exper_value(menv[monster_killed].type, menv[monster_killed].hit_dice, menv[monster_killed].max_hit_points));
        }
        if (menv[monster_killed].type == MONS_FIRE_VORTEX)
            place_cloud(101, menv[monster_killed].x, menv[monster_killed].y, 2 + random2(4));
        goto out_of_switch;
    }                           /* end fire vortex */
    if (menv[monster_killed].type == MONS_DANCING_WEAPON)
    {
        strcpy(info, monam(menv[monster_killed].number, menv[monster_killed].type, menv[monster_killed].enchantment[2], 0));
        strcat(info, " falls from the air.");
        if (mons_near(monster_killed))
            mpr(info);
        if (YOU_KILL(killer))
        {
            gain_exp(exper_value(menv[monster_killed].type, menv[monster_killed].hit_dice, menv[monster_killed].max_hit_points));
        }
        goto out_of_switch;
    }                           /* end dancing weapon */
    switch (killer)
    {
    case KILL_YOU:                     /* You kill in combat. */
    case KILL_YOU_MISSILE:             /* You kill by missile or beam. */
        strcpy(info, "You kill ");
        if (wounded_damaged(menv[monster_killed].type) == 1)
            strcpy(info, "You destroy ");
        strcat(info, monam(menv[monster_killed].number, menv[monster_killed].type, menv[monster_killed].enchantment[2], 1));    //gmon_name [menv [monster_killed].type]);

        strcat(info, "!");
        mpr(info);
        gain_exp(exper_value(menv[monster_killed].type, menv[monster_killed].hit_dice, menv[monster_killed].max_hit_points));
        if (you.religion == GOD_XOM && random2(70) <= 10 + menv[monster_killed].hit_dice)
            Xom_acts(1, random2(menv[monster_killed].hit_dice) + 1, 0);

        // Trying to prevent summoning abuse here, so we're trying to
        // prevent summoned creatures from being being done_good kills,
        // this will also mean that opponenet summoned monsters will
        // also not count.
        if ((menv[monster_killed].enchantment[1] < 20
                            || menv[monster_killed].enchantment[1] > 25)

                // these gods can't be used for summoning abuse,
                // and for the most part have to be passed through
                // because they might be doing something naughty.
                    || you.religion == GOD_ELYVILON
                    || you.religion == GOD_SHINING_ONE
                    || you.religion == GOD_ZIN)
        {
            if (you.duration[DUR_PRAYER] > 0)
            {
                if (mons_holiness(menv[monster_killed].type) == MH_NORMAL)
                    done_good(1, menv[monster_killed].hit_dice);
                if (mons_holiness(menv[monster_killed].type) == MH_UNDEAD)
                    done_good(2, menv[monster_killed].hit_dice);
                if (mons_holiness(menv[monster_killed].type) == MH_DEMONIC)
                    done_good(3, menv[monster_killed].hit_dice);
                if (mons_holiness(menv[monster_killed].type) == -1)
                    done_good(5, menv[monster_killed].hit_dice);
            }
            else if (mons_holiness(menv[monster_killed].type) == -1)
            {
                done_good(4, menv[monster_killed].hit_dice);
            }
        }

        if ((you.religion == GOD_MAKHLEB && you.duration[DUR_PRAYER] != 0 && random2(you.piety) >= 30) || you.mutation[MUT_DEATH_STRENGTH] != 0)
        {                       /* Makhleb */
            if (you.hp < you.hp_max)
            {
                mpr("You feel a little better.");
                you.hp += menv[monster_killed].hit_dice + random2(menv[monster_killed].hit_dice);
                if (you.hp > you.hp_max)
                    you.hp = you.hp_max;
                you.redraw_hit_points = 1;
            }
        }
        if ((you.religion == GOD_MAKHLEB || you.religion == GOD_VEHUMET) && you.duration[DUR_PRAYER] != 0 && random2(you.piety) >= 30)
        {                       /* Makhleb or Vehumet */
            if (you.magic_points < you.max_magic_points)
            {
                mpr("You feel your power returning.");
                you.magic_points += random2(random2(menv[monster_killed].hit_dice)) + 1;
                if (you.magic_points > you.max_magic_points)
                    you.magic_points = you.max_magic_points;
                you.redraw_magic_points = 1;
            }
        }
        if (you.duration[DUR_DEATH_CHANNEL] != 0 && mons_holiness(menv[monster_killed].type) == MH_NORMAL && mons_weight(mons_charclass(menv[monster_killed].type)) != 0)
        {
            if (create_monster(MONS_SPECTRAL_THING, 0, BEH_ENSLAVED, menv[monster_killed].x, menv[monster_killed].y, you.pet_target, mons_charclass(menv[monster_killed].type)) != -1)
                mpr("A strange glowing mist starts to gather...");
        }                       /* Death Channel - creates a spectral version of the monster */
        break;

    case KILL_MON:                     /* Monster kills in combat */
    case KILL_MON_MISSILE:                     /* Monster kills by missile or beam */
        if (mons_near(monster_killed))
        {
            strcpy(info, monam(menv[monster_killed].number, menv[monster_killed].type, menv[monster_killed].enchantment[2], 0));        //gmon_name [menv [monster_killed].type]);

            strcat(info, " dies!");
            mpr(info);
        }

        if (menv[monster_killed].behavior == BEH_ENSLAVED)
        {
            naughty( NAUGHTY_FRIEND_DIES,
                                (menv[monster_killed].hit_dice / 2) + 1 );
        }

        if ((i >= 0 && i < 200) && menv[i].behavior == BEH_ENSLAVED)
        {
            gain_exp(exper_value(menv[monster_killed].type, menv[monster_killed].hit_dice, menv[monster_killed].max_hit_points) / 2 + 1);

            // Trying to prevent summoning abuse here, so we're trying to
            // prevent summoned creatures from being being done_good kills,
            // this will also mean that opponenet summoned monsters will
            // also not count.
            if ((menv[monster_killed].enchantment[1] < 20
                            || menv[monster_killed].enchantment[1] > 25)

                // these gods can't be used for summoning abuse,
                // and for the most part have to be passed through
                // because they might be doing something naughty.
                    || you.religion == GOD_ELYVILON
                    || you.religion == GOD_SHINING_ONE
                    || you.religion == GOD_ZIN)
            {
                if (mons_holiness(menv[i].type) == MH_UNDEAD)
                {
                    if (mons_holiness(menv[monster_killed].type) == MH_NORMAL)
                        done_good(9, menv[monster_killed].hit_dice);
                    else
                        done_good(10, menv[monster_killed].hit_dice);
                }
                else
                {
                    done_good(10, menv[monster_killed].hit_dice);
                    if (you.religion == GOD_VEHUMET && random2(you.piety) >= 20)
                    {
                        /* Vehumet - only for non-undead servants (coding
                        convenience, no real reason except that Vehumet
                        prefers demons) */
                        if (you.magic_points < you.max_magic_points)
                        {
                            mpr("You feel your power returning.");
                            you.magic_points += random2(random2(menv[monster_killed].hit_dice)) + 1;
                            if (you.magic_points > you.max_magic_points)
                                you.magic_points = you.max_magic_points;
                            you.redraw_magic_points = 1;
                        }
                    }
                }
            }
        }
        break;

    case KILL_MISC:                     /* Monster killed by trap/inanimate thing/itself/poison not from you */
        if (mons_near(monster_killed))
        {
            strcpy(info, monam(menv[monster_killed].number, menv[monster_killed].type, menv[monster_killed].enchantment[2], 0));        //gmon_name [mons_class [i]]);

            strcat(info, " dies!");
            mpr(info);
        }
        break;

    case KILL_RESET:                     /* Monster doesn't die, just goes back to wherever it came from
                                   This must only be called by monsters running out of time (or abjuration),
                                   because it uses the beam variables! Or does it??? */
        if (mons_near(monster_killed))
        {
            strcpy(info, monam(menv[monster_killed].number, menv[monster_killed].type, menv[monster_killed].enchantment[2], 0));        //gmon_name [mons_class [i]]);

            strcat(info, " disappears in a puff of smoke!");
            mpr(info);
            place_cloud(random2(3) + 105, menv[monster_killed].x, menv[monster_killed].y, 1 + random2(3));
        }

        for (dmi = 7; dmi >= 0; dmi--)
        {                       /* takes whatever it's carrying back home */
            if (menv[monster_killed].inv[dmi] != 501)
            {
                destroy_item(menv[monster_killed].inv[dmi]);
            }
            menv[monster_killed].inv[dmi] = 501;
        }
        break;
    }                           /* end of switch */


out_of_switch:
    if (menv[monster_killed].type == MONS_MUMMY)
    {
        /* mummy! */
        if (YOU_KILL(killer))
        {
            if (curse_an_item(1, 0) == 1)
            {
                strcpy(info, "You feel nervous for a moment...");
                mpr(info);
            }
        }
    }

    if (menv[monster_killed].type == MONS_GUARDIAN_MUMMY || menv[monster_killed].type == MONS_GREATER_MUMMY || menv[monster_killed].type == MONS_MUMMY_PRIEST)
    {                           /* other mummies */
        if (YOU_KILL(killer))
        {
            mpr("You feel extremely nervous for a moment...");
            miscast_effect(16, 3 + (menv[monster_killed].type == MONS_GREATER_MUMMY) * 8 + (menv[monster_killed].type == MONS_MUMMY_PRIEST) * 5, random2(30) + random2(30) + random2(30), 100);
        }
    }

    if (menv[monster_killed].type == MONS_WORM_TAIL)
    {
        dmi = monster_killed;
        while (menv[dmi].type != MONS_TUNNELING_WORM)
        {
            dmi--;
            if ((menv[dmi].type != MONS_WORM_TAIL && menv[dmi].type != MONS_TUNNELING_WORM) || dmi < 0)
            {
                strcpy(info, "Error: Worm head not found!");
                mpr(info);
                strcpy(info, "Oh well. Carry on.");
                mpr(info);
                goto out_of_worm;
            }
        }

        if (menv[dmi].hit_dice >= 3)
        {
            menv[dmi].hit_dice -= 2;
            menv[dmi].max_hit_points -= 8 + random2(8);
            menv[dmi].hit_points -= random2(8) + 8;
            if (menv[dmi].max_hit_points <= 0)
                menv[dmi].max_hit_points = 1;
            if (menv[dmi].hit_points > menv[dmi].max_hit_points)
                menv[dmi].hit_points = menv[dmi].max_hit_points;
            if (menv[dmi].hit_points <= 0)
                menv[dmi].hit_points = 1;
        }
    }

out_of_worm:
    if (menv[monster_killed].type == MONS_TUNNELING_WORM || menv[monster_killed].type == MONS_WORM_TAIL)
    {
        dmi = monster_killed;

        while (menv[dmi + 1].type == MONS_WORM_TAIL)
        {
            dmi++;
            menv[dmi].type = -1;
            mgrd[menv[dmi].x][menv[dmi].y] = MNG;
        }

    }

    if (killer != KILL_RESET)
    {
        if (menv[monster_killed].enchantment[1] >= 20 && menv[monster_killed].enchantment[1] <= 25)
        {
            if (mons_near(monster_killed) && mons_weight(mons_charclass(menv[monster_killed].type)) != 0)
            {
                strcpy(info, monam(menv[monster_killed].number, menv[monster_killed].type, menv[monster_killed].enchantment[2], 0));
                strcat(info, "'s corpse disappears in a puff of smoke!");
                mpr(info);
                place_cloud(random2(3) + 105, menv[monster_killed].x, menv[monster_killed].y, 1 + random2(3));
            }
        }
        else
            place_monster_corpse(monster_killed);
    }

    monster_drop_ething(monster_killed);


    int j;

    for (j = 0; j < 3; j++)
    {
        menv[monster_killed].enchantment[j] = 0;
    }

    menv[monster_killed].enchantment1 = 0;

    /*no_mons --; */
    menv[monster_killed].type = -1;
    menv[monster_killed].hit_points = 0;
    menv[monster_killed].max_hit_points = 0;
    menv[monster_killed].hit_dice = 0;
    menv[monster_killed].armor_class = 0;
    menv[monster_killed].evasion = 0;

    mgrd[menv[monster_killed].x][menv[monster_killed].y] = MNG;

    for (dmi = 7; dmi >= 0; dmi--)
    {
        menv[monster_killed].inv[dmi] = 501;
    }


    /*brek = 1; */
    viewwindow(1, false);

    for (dmi = 0; dmi < MNST; dmi++)
    {
        if (menv[dmi].monster_foe == monster_killed)
            menv[dmi].monster_foe = MHITNOT;
    }

    if (you.pet_target == monster_killed)
        you.pet_target = MHITNOT;

    return;
}



int jelly_divide(int jel)
{

    int k = 0;

    if (menv[jel].type != MONS_JELLY)
        return 0;

    if (menv[jel].hit_points == 1)
        return 0;

    int jex = 0;
    int jey = 0;

    for (jex = -1; jex < 3; jex++)
    {

        if (jex == 2)
            return 0;

        for (jey = -1; jey < 2; jey++)
        {
            if (mgrd[menv[jel].x + jex][menv[jel].y + jey] == MNG && mgrd[menv[jel].x + jex][menv[jel].y + jey] > 10 && (menv[jel].x + jex != you.x_pos || menv[jel].y + jey != you.y_pos))
                /* 10-50 for now - must take clouds into account */
                goto jel2;
        }                       /* end of for jey */

    }                           /* end of for jex */

  jel2:for (k = 0; k < MNST; k++)
    {
        if (k == MNST - 1)
            return 0;

        if (menv[k].type == -1)
            break;
    }

    menv[jel].max_hit_points /= 2;
    if (menv[jel].hit_points > menv[jel].max_hit_points)
        menv[jel].hit_points = menv[jel].max_hit_points;

    menv[k].type = MONS_JELLY;
    menv[k].hit_dice = 4;
    menv[k].hit_points = menv[jel].hit_points;
    menv[k].max_hit_points = menv[k].hit_points;
    menv[k].armor_class = 0;
    menv[k].evasion = 2;
    menv[k].speed = 5;
    menv[k].speed_increment = 70 + random2(5);

    menv[k].x = menv[jel].x + jex;
    menv[k].y = menv[jel].y + jey;

    mgrd[menv[k].x][menv[k].y] = k;

/*         no_mons ++;
   mn_no ++; */

    menv[k].behavior = menv[jel].behavior;      /* Look at this! */

    if (mons_near(jel))
    {
        strcpy(info, "The jelly splits in two!");
        mpr(info);
    }
    else
    {
        strcpy(info, "You hear a squelching noise in the distance.");
        mpr(info);
    }

    return 1;

}



void alert(void)
{
/* if you're invis and throw/zap whatever, alerts menv to your position */

    int it;

    for (it = 0; it < MNST; it++)
    {

        if (menv[it].type == -1)
            continue;

        if (mons_near(it))
        {
            if (menv[it].behavior == BEH_CHASING_I || menv[it].behavior == BEH_WANDER || menv[it].behavior == BEH_SLEEP)
            {
                menv[it].behavior = BEH_CHASING_I;
                if (you.invis == 0 || mons_see_invis(menv[it].type) == 1)
                {
                    menv[it].target_x = you.x_pos;
                    menv[it].target_y = you.y_pos;
                }
            }
        }
    }                           /* end of for it */

}                               /* end of void alert() */




void monster_polymorph(unsigned char monsc, unsigned char targetc, int power)
{

    if (targetc == 250)
    {
      loopy:
        do
        {
            targetc = random2(400);
        }
        while (mons_rarity(targetc) == 0
                || targetc == MONS_SHAPESHIFTER
                || targetc == MONS_GLOWING_SHAPESHIFTER
                || targetc == MONS_SMALL_ZOMBIE
                || targetc == MONS_BIG_ZOMBIE
                || targetc == MONS_SPECTRAL_THING
                || targetc == MONS_SMALL_SKELETON
                || targetc == MONS_LARGE_SKELETON
                || targetc == MONS_DANCING_WEAPON
                || mons_flag( targetc, M_NO_EXP_GAIN ));

        // no shapeshifters or zombies/skeletons/spectr or
        // plants/fungii/other no exp stuff

        if (grd[menv[monsc].x][menv[monsc].y] == 61 || grd[menv[monsc].x][menv[monsc].y] == 62)
            if (mons_flies(targetc) == 0)
                goto loopy;     /* Not fair to instakill a monster like this (actually, I can't be bothered implementing it) */
        /* Too long to put in the loop thing */
    }
/* if (power != -1) // automatic success */

    unsigned char old_class = menv[monsc].type;
    unsigned char old_hp = menv[monsc].hit_points;
    unsigned char old_hp_max = menv[monsc].max_hit_points;
    unsigned char old_sec = menv[monsc].number;

/* deal with mons_sec */


    menv[monsc].type = targetc;


    menv[monsc].number = 250;

    for (int unenc = 0; unenc < 3; unenc++)
    {
        if (menv[monsc].enchantment[unenc] >= 20 && menv[monsc].enchantment[unenc] <= 25)
            continue;           /* Summoned creatures are still going to disappear eventually */
        if (menv[monsc].enchantment[unenc] != 39 && menv[monsc].enchantment[unenc] != 38)
            menv[monsc].enchantment[unenc] = 0;         /* shapeshifters stay as such */
    }

    if (mons_flag(menv[monsc].type, M_INVIS))
    {
        menv[monsc].enchantment1 = 1;
        menv[monsc].enchantment[2] = 6;
    }
    if (menv[monsc].enchantment[0] == 0 && menv[monsc].enchantment[1] == 0 && menv[monsc].enchantment[2] == 0)
        menv[monsc].enchantment1 = 0;

    define_monster(monsc, menv);

    menv[monsc].hit_points = menv[monsc].max_hit_points * ((old_hp * 100) / old_hp_max) / 100 + random2(menv[monsc].max_hit_points);

    if (menv[monsc].hit_points > menv[monsc].max_hit_points)
        menv[monsc].hit_points = menv[monsc].max_hit_points;

    menv[monsc].speed_increment = 70 + random2(5);

/* make sure that eg manticores don't drop their darts */

    monster_drop_ething(monsc);

    strcpy(info, monam(old_sec, old_class, menv[monsc].enchantment[2], 0));
    if (menv[monsc].enchantment[1] == 39 || menv[monsc].enchantment[1] == 38)
        strcat(info, " changes into ");
    else if (targetc == 131)
        strcat(info, " degenerates into ");
    else
        strcat(info, " evaporates, and reforms as ");

    strcat(info, monam(menv[monsc].number, menv[monsc].type, menv[monsc].enchantment[2], 3));
    if (targetc == 131)
        strcat(info, " of flesh");
    strcat(info, "!");
    if (mons_near(monsc))
        mpr(info);

}                               /* end of monster_polymorph */


void monster_drop_ething(int monster_killed)
{
/* use monster_killed for convenience
   drop weapons & missiles last (ie on top) so others pick up */

    int ygy;
    char splashes = 0;

    if (grd[menv[monster_killed].x][menv[monster_killed].y] == 61 || grd[menv[monster_killed].x][menv[monster_killed].y] == 62)
    {
        for (ygy = 0; ygy < 8; ygy++)
        {
            if (menv[monster_killed].inv[ygy] != 501)
            {
                destroy_item(menv[monster_killed].inv[ygy]);
                splashes++;
            }
        }
        if (splashes > 0)
        {
            strcpy(info, "You hear a splashing sound.");
            mpr(info);
        }
        return;
    }
    for (ygy = 7; ygy >= 0; ygy--)
    {
        if (menv[monster_killed].inv[ygy] != 501)
        {
            mitm.link[menv[monster_killed].inv[ygy]] = 501;
            if (igrd[menv[monster_killed].x][menv[monster_killed].y] == 501)
                /* this bit is wrong, BTW:
                   if (mons_inv [monster_killed] [j] != 501) */
            {
                igrd[menv[monster_killed].x][menv[monster_killed].y] = menv[monster_killed].inv[ygy];
            }
            else
            {
                int huj = igrd[menv[monster_killed].x][menv[monster_killed].y];

                igrd[menv[monster_killed].x][menv[monster_killed].y] = menv[monster_killed].inv[ygy];
                mitm.link[menv[monster_killed].inv[ygy]] = huj;
            }
        }
        menv[monster_killed].inv[ygy] = 501;
    }



}                               /* end of void monster_drop_ething */



void place_monster_corpse(unsigned char mcr)
{
    int corpse_class = mons_charclass(menv[mcr].type);

    if (menv[mcr].enchantment[1] == 39)
        corpse_class = MONS_SHAPESHIFTER;       /* shapeshifter */
    if (menv[mcr].enchantment[1] == 38)
        corpse_class = MONS_GLOWING_SHAPESHIFTER;       /* shapeshifter */

    if (mons_weight(corpse_class) == 0 || random2(2) == 0)
        return;
    if (grd[menv[mcr].x][menv[mcr].y] == 61 || grd[menv[mcr].x][menv[mcr].y] == 62)
        return;

    int o;

    for (o = 0; o < ITEMS; o++)
    {
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
            mitm.colour[o] = mcolour[corpse_class];
            if (mcolour[corpse_class] == BLACK)
                mitm.colour[o] = menv[mcr].number;
            mitm.quantity[o] = 1;
            mitm.link[o] = 501;
            break;
        }
    }                           // end of o loop

    if (igrd[menv[mcr].x][menv[mcr].y] == 501)
    {
        igrd[menv[mcr].x][menv[mcr].y] = o;
    }
    else
    {
        int hug = igrd[menv[mcr].x][menv[mcr].y];

        igrd[menv[mcr].x][menv[mcr].y] = o;
        mitm.link[o] = hug;
    }

    return;

}

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

#include "externs.h"

#include "itemname.h"
#include "misc.h"
#include "mon-util.h"
#include "mutation.h"
#include "output.h"
#include "randart.h"
#include "religion.h"
#include "skills2.h"
#include "spl-util.h"
#include "stuff.h"
#include "view.h"

#ifdef MACROS
#include "macro.h"
#endif

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

//void priest_spells(int priest_pass[10], char religious);    // see actual function for reasoning here {dlb}

bool player_in_water(void)
{
    return (!you.levitation
            && (grd[you.x_pos][you.y_pos] == DNGN_DEEP_WATER
                || grd[you.x_pos][you.y_pos] == DNGN_SHALLOW_WATER));
}

bool player_under_penance(void)
{
    if (you.religion != GOD_NO_GOD)
        return (you.penance[you.religion]);
    else
        return (false);
}

bool player_genus(unsigned char which_genus, unsigned char species)
{
    if (species == SP_UNKNOWN)
        species = you.species;

    switch (species)
    {
    case SP_RED_DRACONIAN:
    case SP_WHITE_DRACONIAN:
    case SP_GREEN_DRACONIAN:
    case SP_GOLDEN_DRACONIAN:
    case SP_GREY_DRACONIAN:
    case SP_BLACK_DRACONIAN:
    case SP_PURPLE_DRACONIAN:
    case SP_MOTTLED_DRACONIAN:
    case SP_PALE_DRACONIAN:
    case SP_UNK0_DRACONIAN:
    case SP_UNK1_DRACONIAN:
    case SP_UNK2_DRACONIAN:
        if (which_genus == GENPC_DRACONIAN)
            return true;
        else
            return false;
        break;

    case SP_ELF:
    case SP_HIGH_ELF:
    case SP_GREY_ELF:
    case SP_DEEP_ELF:
    case SP_SLUDGE_ELF:
        if (which_genus == GENPC_ELVEN)
            return true;
        else
            return false;
        break;

    case SP_HILL_DWARF:
    case SP_MOUNTAIN_DWARF:
        if (which_genus == GENPC_DWARVEN)
            return true;
        else
            return false;
        break;

    default:
        return false;
        break;
    }
}                               // end player_genus()

int player_teleport(void)
{
    int tp = 0;

    /* rings */
    if (you.equip[EQ_LEFT_RING] != -1
        && you.inv_type[you.equip[EQ_LEFT_RING]] == RING_TELEPORTATION)
    {
        tp += 8;
    }

    if (you.equip[EQ_RIGHT_RING] != -1
        && you.inv_type[you.equip[EQ_RIGHT_RING]] == RING_TELEPORTATION)
    {
        tp += 8;
    }

    /* mutations */
    tp += you.mutation[MUT_TELEPORT] * 3;

    /* randart weapons only */
    if (you.equip[EQ_WEAPON] != -1
        && you.inv_class[you.equip[EQ_WEAPON]] == OBJ_WEAPONS
        && you.inv_dam[you.equip[EQ_WEAPON]] % 30 >= 25)
    {
        tp += scan_randarts(RAP_CAUSE_TELEPORTATION);
    }

    return tp;
}                               // end player_teleport()

int player_regen(void)
{
    int rr = you.hp_max / 3;

    if (rr > 20)
        rr = 20 + ((rr - 20) / 2);

    /* rings */
    if (you.equip[EQ_LEFT_RING] != -1
        && you.inv_type[you.equip[EQ_LEFT_RING]] == RING_REGENERATION)
    {
        rr += 40;
    }

    if (you.equip[EQ_RIGHT_RING] != -1
        && you.inv_type[you.equip[EQ_RIGHT_RING]] == RING_REGENERATION)
    {
        rr += 40;
    }

    /* spell */
    if (you.duration[DUR_REGENERATION])
        rr += 100;

    /* troll or troll leather */
    if (you.species == SP_TROLL)
        rr += 40;

    /* trolls gain no benefit from troll leather */
    else if (you.equip[EQ_BODY_ARMOUR] != -1
         && you.inv_type[you.equip[EQ_BODY_ARMOUR]] == ARM_TROLL_LEATHER_ARMOUR)
    {
        rr += 30;
    }

    /* fast heal mutation */
    rr += you.mutation[MUT_REGENERATION] * 20;

    /* ghouls heal slowly */
    // dematerialized people heal slowly
    // dematerialized ghouls shouldn't heal any more slowly -- bwr
    if ((you.species == SP_GHOUL
            && (you.attribute[ATTR_TRANSFORMATION] == TRAN_NONE
                || you.attribute[ATTR_TRANSFORMATION] == TRAN_BLADE_HANDS))
        || you.attribute[ATTR_TRANSFORMATION] == TRAN_AIR)
    {
        rr /= 2;
    }

    return rr;
}

int player_hunger_rate(void)
{
    int hunger = 3;

    // jmf: hunger isn't fair while you can't eat
    // Actually, it is since you can detransform any time you like -- bwr
    if (you.attribute[ATTR_TRANSFORMATION] == TRAN_AIR)
        return 0;

    switch (you.species)
    {
    case SP_HALFLING:
    case SP_SPRIGGAN:
        hunger--;
        break;

    case SP_OGRE:
    case SP_OGRE_MAGE:
    case SP_DEMIGOD:
        hunger++;
        break;

    case SP_CENTAUR:
        hunger += 2;
        break;

    case SP_TROLL:
        hunger += 6;
        break;
    }

    if (you.duration[DUR_REGENERATION] > 0)
        hunger += 4;

    hunger += you.mutation[MUT_FAST_METABOLISM];

    if (you.mutation[MUT_SLOW_METABOLISM] > 2)
        hunger -= 2;
    else if (you.mutation[MUT_SLOW_METABOLISM] > 0)
        hunger--;

    for (int i = EQ_LEFT_RING; i <= EQ_RIGHT_RING; i++)
    {
        if (you.equip[i] != -1)
        {
            switch (you.inv_type[you.equip[i]])
            {
            case RING_REGENERATION:
                hunger += 2;
                break;

            case RING_HUNGER:
                hunger += 4;
                break;

            case RING_SUSTENANCE:
                hunger -= 2;
                break;
            }
        }
    }

    if (you.equip[EQ_WEAPON] != -1
        && you.inv_class[you.equip[EQ_WEAPON]] == OBJ_WEAPONS)
    {
        if (you.inv_dam[you.equip[EQ_WEAPON]] < NWPN_SINGING_SWORD)
        {
            if (you.inv_dam[you.equip[EQ_WEAPON]] % 30 == SPWPN_VAMPIRICISM)
                hunger += 6;
        }
        else if (you.inv_dam[you.equip[EQ_WEAPON]] == NWPN_VAMPIRES_TOOTH)
        {
            hunger += 9;
        }
    }

    if (you.equip[EQ_BODY_ARMOUR] != -1
         && you.inv_type[you.equip[EQ_BODY_ARMOUR]] == ARM_TROLL_LEATHER_ARMOUR)
    {
        hunger += 1;
    }

    hunger += scan_randarts(RAP_METABOLISM);

    return hunger;
}

int player_spell_levels(void)
{
    int sl = (you.experience_level - 1) + (you.skills[SK_SPELLCASTING] * 2);

    if (sl > 99)
        sl = 99;

    for (int i = 0; i < 25; i++)
    {
        if (you.spells[i] != SPELL_NO_SPELL)
            sl -= spell_difficulty(you.spells[i]);
    }

    if (sl < 0)
        sl = 0;

    return (sl);
}

static int player_res_magic(void)
{
    int rm = 0;

    switch (you.species)
    {
    default:
        rm = you.experience_level * 3;
        break;
    case SP_HIGH_ELF:
    case SP_GREY_ELF:
    case SP_ELF:
    case SP_SLUDGE_ELF:
    case SP_HILL_DWARF:
    case SP_MOUNTAIN_DWARF:
        rm = you.experience_level * 4;
        break;
    case SP_NAGA:
        rm = you.experience_level * 5;
        break;
    case SP_PURPLE_DRACONIAN:
    case SP_GNOME:
    case SP_DEEP_ELF:
        rm = you.experience_level * 6;
        break;
    case SP_SPRIGGAN:
        rm = you.experience_level * 7;
        break;
    }

    /* armour: (checks cloak & body armour only) */
    if (you.equip[EQ_CLOAK] != -1
        && you.inv_dam[you.equip[EQ_CLOAK]] % 30 == SPARM_MAGIC_RESISTANCE)
    {
        rm += 30;
    }
    if (you.equip[EQ_BODY_ARMOUR] != -1
        && you.inv_dam[you.equip[EQ_BODY_ARMOUR]] % 30
                                                 == SPARM_MAGIC_RESISTANCE)
    {
        rm += 30;
    }

    /* rings of magic resistance */
    if (you.equip[EQ_LEFT_RING] != -1
        && you.inv_type[you.equip[EQ_LEFT_RING]] == RING_PROTECTION_FROM_MAGIC)
    {
        rm += 40;
    }
    if (you.equip[EQ_RIGHT_RING] != -1
        && you.inv_type[you.equip[EQ_RIGHT_RING]] == RING_PROTECTION_FROM_MAGIC)
    {
        rm += 40;
    }

    /* randarts */
    rm += scan_randarts(RAP_MAGIC);
    /* Enchantment skill */
    rm += you.skills[SK_ENCHANTMENTS] * 2;
    /* Mutations */
    rm += you.mutation[MUT_MAGIC_RESISTANCE] * 30;

    /* transformations */
    switch (you.attribute[ATTR_TRANSFORMATION])
    {
    case TRAN_LICH:
        rm += 50;
        break;
    default:
        break;
    }

    return rm;
}

int player_res_fire(void)
{
    int rf = 100;

    /* rings of fire resistance/fire */
    if (you.equip[ EQ_LEFT_RING ] != -1
        && (you.inv_type[you.equip[EQ_LEFT_RING]] == RING_PROTECTION_FROM_FIRE
            || you.inv_type[you.equip[EQ_LEFT_RING]] == RING_FIRE))
    {
        rf++;
    }

    if (you.equip[EQ_RIGHT_RING] != -1
        && (you.inv_type[you.equip[EQ_RIGHT_RING]] == RING_PROTECTION_FROM_FIRE
            || you.inv_type[you.equip[EQ_RIGHT_RING]] == RING_FIRE))
    {
        rf++;
    }

    /* rings of ice */
    if (you.equip[EQ_LEFT_RING] != -1
        && you.inv_type[you.equip[EQ_LEFT_RING]] == RING_ICE)
    {
        rf--;
    }

    if (you.equip[EQ_RIGHT_RING] != -1
        && you.inv_type[you.equip[EQ_RIGHT_RING]] == RING_ICE)
    {
        rf--;
    }

    /* Staves */
    if (you.equip[EQ_WEAPON] != -1
        && you.inv_class[you.equip[EQ_WEAPON]] == OBJ_STAVES
        && you.inv_type[you.equip[EQ_WEAPON]] == STAFF_FIRE)
    {
        rf++;
    }

    // body armour:
    if (you.equip[EQ_BODY_ARMOUR] != -1)
    {
        if (you.inv_dam[you.equip[EQ_BODY_ARMOUR]] % 30 == SPARM_FIRE_RESISTANCE
            || you.inv_dam[you.equip[EQ_BODY_ARMOUR]] % 30 == SPARM_RESISTANCE)
        {
            rf++;
        }

        switch (you.inv_type[you.equip[EQ_BODY_ARMOUR]])
        {
        case ARM_DRAGON_ARMOUR:
            rf += 2;
            break;
        case ARM_GOLD_DRAGON_ARMOUR:
            rf++;
            break;
        case ARM_ICE_DRAGON_ARMOUR:
            rf--;
            break;
        }
    }

    // randart weapons:
    rf += scan_randarts(RAP_FIRE);

    // species:
    if (you.species == SP_MUMMY)
        rf--;
    else if (you.species == SP_RED_DRACONIAN && you.experience_level > 17)
        rf++;

    // mutations:
    rf += you.mutation[MUT_HEAT_RESISTANCE];

    // transformations:
    switch (you.attribute[ATTR_TRANSFORMATION])
    {
    case TRAN_ICE_BEAST:
        rf--;
        break;
    case TRAN_DRAGON:
        rf += 2;
        break;
    case TRAN_SERPENT_OF_HELL:
        rf += 2;
        break;
    case TRAN_AIR:
        rf -= 2;
        break;
    }

    return rf;
}

int player_res_cold(void)
{
    int rc = 100;

    /* rings of cold resistance/ice */
    if (you.equip[EQ_LEFT_RING] != -1
        && (you.inv_type[you.equip[EQ_LEFT_RING]] == RING_PROTECTION_FROM_COLD
            || you.inv_type[you.equip[EQ_LEFT_RING]] == RING_ICE))
    {
        rc++;
    }

    if (you.equip[EQ_RIGHT_RING] != -1
        && (you.inv_type[you.equip[EQ_RIGHT_RING]] ==
            RING_PROTECTION_FROM_COLD
            || you.inv_type[you.equip[EQ_RIGHT_RING]] == RING_ICE))
    {
        rc++;
    }

    /* rings of fire */
    if (you.equip[EQ_LEFT_RING] != -1
        && you.inv_type[you.equip[EQ_LEFT_RING]] == RING_FIRE)
    {
        rc--;
    }

    if (you.equip[EQ_RIGHT_RING] != -1
        && you.inv_type[you.equip[EQ_RIGHT_RING]] == RING_FIRE)
    {
        rc--;
    }

    /* Staves */
    if (you.equip[EQ_WEAPON] != -1
        && you.inv_class[you.equip[EQ_WEAPON]] == OBJ_STAVES
        && you.inv_type[you.equip[EQ_WEAPON]] == STAFF_COLD)
    {
        rc++;
    }

    // body armour:
    if (you.equip[EQ_BODY_ARMOUR] != -1)
    {
        if (you.inv_dam[you.equip[EQ_BODY_ARMOUR]] % 30 == SPARM_COLD_RESISTANCE
            || you.inv_dam[you.equip[EQ_BODY_ARMOUR]] % 30 == SPARM_RESISTANCE)
        {
            rc++;
        }

        switch (you.inv_type[you.equip[EQ_BODY_ARMOUR]])
        {
        case ARM_ICE_DRAGON_ARMOUR:
            rc += 2;
            break;
        case ARM_GOLD_DRAGON_ARMOUR:
            rc++;
            break;
        case ARM_DRAGON_ARMOUR:
            rc--;
            break;
        }
    }

    // randart weapons:
    rc += scan_randarts(RAP_COLD);

    // species:
    if (you.species == SP_MUMMY || you.species == SP_GHOUL)
        rc++;
    else if (you.species == SP_WHITE_DRACONIAN && you.experience_level > 17)
        rc++;

    // mutations:
    rc += you.mutation[MUT_COLD_RESISTANCE];

    // transformations:
    switch (you.attribute[ATTR_TRANSFORMATION])
    {
    case TRAN_ICE_BEAST:
        rc += 3;
        break;
    case TRAN_DRAGON:
        rc--;
        break;
    case TRAN_LICH:
        rc++;
        break;
    case TRAN_AIR:
        rc -= 2;
        break;
    }

    return rc;
}

int player_res_electricity(void)
{
    int re = 0;

    if (you.duration[DUR_INSULATION])
        re++;

    if (you.attribute[ATTR_DIVINE_LIGHTNING_PROTECTION])
        re++;

    if (you.equip[EQ_WEAPON] != -1
        && you.inv_class[you.equip[EQ_WEAPON]] == OBJ_STAVES
        && you.inv_type[you.equip[EQ_WEAPON]] == STAFF_AIR)
    {
        re++;
    }

    // body armour:
    if (you.equip[EQ_BODY_ARMOUR] != -1
        && you.inv_type[you.equip[EQ_BODY_ARMOUR]] == ARM_STORM_DRAGON_ARMOUR)
    {
        re++;
    }

    // randart weapons:
    re += scan_randarts(RAP_ELECTRICITY);

    // species:
    if (you.species == SP_BLACK_DRACONIAN && you.experience_level > 17)
        re++;

    // mutations:
    if (you.mutation[MUT_SHOCK_RESISTANCE])
        re++;

    // transformations:
    switch (you.attribute[ATTR_TRANSFORMATION])
    {
    case TRAN_AIR:
        re += 2;  // meaningless, only one level actually counts -- bwr
        break;
    default:
        break;
    }

    return re;
}                               // end player_res_electricity()

// funny that no races are susceptible to poisons {dlb}
int player_res_poison(void)
{
    int rp = 0;

    /* rings of poison resistance */
    if (you.equip[EQ_LEFT_RING] != -1
        && you.inv_type[you.equip[EQ_LEFT_RING]] == RING_POISON_RESISTANCE)
    {
        rp++;
    }

    if (you.equip[EQ_RIGHT_RING] != -1
        && you.inv_type[you.equip[EQ_RIGHT_RING]] == RING_POISON_RESISTANCE)
    {
        rp++;
    }

    /* Staves */
    if (you.equip[EQ_WEAPON] != -1
        && you.inv_class[you.equip[EQ_WEAPON]] == OBJ_STAVES
        && you.inv_type[you.equip[EQ_WEAPON]] == STAFF_POISON)
    {
        rp++;
    }

    /* the staff of Olgreb: */
    if (you.equip[EQ_WEAPON] != -1
        && you.inv_class[you.equip[EQ_WEAPON]] == OBJ_WEAPONS
        && you.inv_dam[you.equip[EQ_WEAPON]] == NWPN_STAFF_OF_OLGREB)
    {
        rp++;
    }

    // cloaks:
    if (you.equip[EQ_CLOAK] != -1
        && you.inv_dam[you.equip[EQ_CLOAK]] % 30 == SPARM_POISON_RESISTANCE)
    {
        rp++;
    }

    // body armour:
    if (you.equip[EQ_BODY_ARMOUR] != -1)
    {
        if (you.inv_dam[you.equip[EQ_BODY_ARMOUR]] % 30
                                                == SPARM_POISON_RESISTANCE)
        {
            rp++;
        }

        if (you.inv_type[you.equip[EQ_BODY_ARMOUR]] == ARM_GOLD_DRAGON_ARMOUR)
            rp++;
        else if (you.inv_type[you.equip[EQ_BODY_ARMOUR]]
                                                == ARM_SWAMP_DRAGON_ARMOUR)
        {
            rp++;
        }
    }

    // spells:
    if (you.duration[DUR_RESIST_POISON] > 0)
        rp++;

    // randart weapons:
    rp += scan_randarts(RAP_POISON);

    // species:
    if (you.species == SP_MUMMY || you.species == SP_NAGA
        || you.species == SP_GHOUL
        || (you.species == SP_GREEN_DRACONIAN && you.experience_level > 6))
    {
        rp++;
    }

    // mutations:
    rp += you.mutation[MUT_POISON_RESISTANCE];

    // transformations:
    switch (you.attribute[ATTR_TRANSFORMATION])
    {
    case TRAN_LICH:
    case TRAN_DRAGON:           //jmf: monster dragons are immune to poison
    case TRAN_SERPENT_OF_HELL:
    case TRAN_AIR:
        rp++;
        break;
    }

    return rp;
}                               // end player_res_poison()

unsigned char player_spec_death(void)
{
    int sd = 0;

    /* Staves */
    if (you.equip[EQ_WEAPON] != -1
        && you.inv_class[you.equip[EQ_WEAPON]] == OBJ_STAVES
        && you.inv_type[you.equip[EQ_WEAPON]] == STAFF_DEATH)
    {
        sd++;
    }

    // body armour:
    if (you.equip[EQ_BODY_ARMOUR] != -1
        && you.inv_dam[you.equip[EQ_BODY_ARMOUR]] % 30 == SPARM_ARCHMAGI)
    {
        sd++;
    }

    // species:
    if (you.species == SP_MUMMY)
    {
        if (you.experience_level > 12)
        {
            sd++;

            if (you.experience_level > 25)
                sd++;
        }
    }

// transformations:
    switch (you.attribute[ATTR_TRANSFORMATION])
    {
    case TRAN_LICH:
        sd++;
        break;
    }

    return sd;
}

unsigned char player_spec_holy(void)
{
    //if ( you.char_class == JOB_PRIEST || you.char_class == JOB_PALADIN )
    //  return 1;
    return 0;
}

unsigned char player_spec_fire(void)
{
    int sf = 0;

    // staves:
    if (you.equip[EQ_WEAPON] != -1
        && you.inv_class[you.equip[EQ_WEAPON]] == OBJ_STAVES
        && you.inv_type[you.equip[EQ_WEAPON]] == STAFF_FIRE)
    {
        sf++;
    }

    // rings of fire:
    if (you.equip[EQ_LEFT_RING] != -1
        && you.inv_type[you.equip[EQ_LEFT_RING]] == RING_FIRE)
    {
        sf++;
    }

    if (you.equip[EQ_RIGHT_RING] != -1
        && you.inv_type[you.equip[EQ_RIGHT_RING]] == RING_FIRE)
    {
        sf++;
    }

    return sf;
}

unsigned char player_spec_cold(void)
{
    int sc = 0;

    // staves:
    if (you.equip[EQ_WEAPON] != -1
        && you.inv_class[you.equip[EQ_WEAPON]] == OBJ_STAVES
        && you.inv_type[you.equip[EQ_WEAPON]] == STAFF_COLD)
    {
        sc++;
    }

    // rings of ice:
    if (you.equip[EQ_LEFT_RING] != -1
        && you.inv_type[you.equip[EQ_LEFT_RING]] == RING_ICE)
    {
        sc++;
    }

    if (you.equip[EQ_RIGHT_RING] != -1
        && you.inv_type[you.equip[EQ_RIGHT_RING]] == RING_ICE)
    {
        sc++;
    }

    return sc;
}

unsigned char player_spec_earth(void)
{
    int se = 0;

    /* Staves */
    if (you.equip[EQ_WEAPON] != -1
        && you.inv_class[you.equip[EQ_WEAPON]] == OBJ_STAVES
        && you.inv_type[you.equip[EQ_WEAPON]] == STAFF_EARTH)
    {
        se++;
    }

    if (you.attribute[ATTR_TRANSFORMATION] == TRAN_AIR)
        se--;

    return se;
}

unsigned char player_spec_air(void)
{
    int sa = 0;

    /* Staves */
    if (you.equip[EQ_WEAPON] != -1
        && you.inv_class[you.equip[EQ_WEAPON]] == OBJ_STAVES
        && you.inv_type[you.equip[EQ_WEAPON]] == STAFF_AIR)
    {
        sa++;
    }

    //jmf: this was too good
    //if (you.attribute[ATTR_TRANSFORMATION] == TRAN_AIR)
    //  sa++;
    return sa;
}

unsigned char player_spec_conj(void)
{
    int sc = 0;

    /* Staves */
    if (you.equip[EQ_WEAPON] != -1
        && you.inv_class[you.equip[EQ_WEAPON]] == OBJ_STAVES
        && you.inv_type[you.equip[EQ_WEAPON]] == STAFF_CONJURATION)
    {
        sc++;
    }

    /* armour of the Archmagi (checks body armour only) */
    if (you.equip[EQ_BODY_ARMOUR] != -1
        && you.inv_dam[you.equip[EQ_BODY_ARMOUR]] % 30 == SPARM_ARCHMAGI)
    {
        sc++;
    }

    return sc;
}

unsigned char player_spec_ench(void)
{
    int se = 0;

    /* Staves */
    if (you.equip[EQ_WEAPON] != -1
        && you.inv_class[you.equip[EQ_WEAPON]] == OBJ_STAVES
        && you.inv_type[you.equip[EQ_WEAPON]] == STAFF_ENCHANTMENT)
    {
        se++;
    }

    /* armour of the Archmagi (checks body armour only) */
    if (you.equip[EQ_BODY_ARMOUR] != -1
        && you.inv_dam[you.equip[EQ_BODY_ARMOUR]] % 30 == SPARM_ARCHMAGI)
    {
        se++;
    }

    return se;
}

unsigned char player_spec_summ(void)
{
    int ss = 0;

    /* Staves */
    if (you.equip[EQ_WEAPON] != -1
        && you.inv_class[you.equip[EQ_WEAPON]] == OBJ_STAVES
        && you.inv_type[you.equip[EQ_WEAPON]] == STAFF_SUMMONING_I)
    {
        ss++;
    }

    /* armour of the Archmagi (checks body armour only) */
    if (you.equip[EQ_BODY_ARMOUR] != -1
        && you.inv_dam[you.equip[EQ_BODY_ARMOUR]] % 30 == SPARM_ARCHMAGI)
    {
        ss++;
    }

    return ss;
}

unsigned char player_spec_poison(void)
{
    int sp = 0;

    // Staves
    if (you.equip[EQ_WEAPON] != -1
        && you.inv_class[you.equip[EQ_WEAPON]] == OBJ_STAVES
        && you.inv_type[you.equip[EQ_WEAPON]] == STAFF_POISON)
    {
        sp++;
    }

    // the staff of Olgreb
    if (you.equip[EQ_WEAPON] != -1
        && you.inv_class[you.equip[EQ_WEAPON]] == OBJ_WEAPONS
        && you.inv_dam[you.equip[EQ_WEAPON]] == NWPN_STAFF_OF_OLGREB)
    {
        sp++;
    }

    return sp;
}

unsigned char player_energy(void)
{
    unsigned char pe = 0;

    // Staves
    if (you.equip[EQ_WEAPON] != -1
        && you.inv_class[you.equip[EQ_WEAPON]] == OBJ_STAVES
        && you.inv_type[you.equip[EQ_WEAPON]] == STAFF_ENERGY)
    {
        pe++;
    }

    return pe;
}

int player_prot_life(void)
{
    int pl = 0;

    if (you.equip[EQ_LEFT_RING] != -1
        && you.inv_type[you.equip[EQ_LEFT_RING]] == RING_LIFE_PROTECTION)
    {
        pl++;
    }

    if (you.equip[EQ_RIGHT_RING] != -1
        && you.inv_type[you.equip[EQ_RIGHT_RING]] == RING_LIFE_PROTECTION)
    {
        pl++;
    }

    // armour: (checks body armour only)
    if (you.equip[EQ_BODY_ARMOUR] != -1
        && you.inv_dam[you.equip[EQ_BODY_ARMOUR]] % 30 == SPARM_POSITIVE_ENERGY)
    {
        pl++;
    }

    if (you.is_undead)
        pl += 3;

    switch (you.attribute[ATTR_TRANSFORMATION])
    {
    case TRAN_LICH:
        pl += 3;
        break;
    case TRAN_SERPENT_OF_HELL:
        pl += 2;
        break;
    default:
        break;
    }

    // randart wpns
    pl += scan_randarts(RAP_NEGATIVE_ENERGY);

    // demonic power
    pl += you.mutation[MUT_NEGATIVE_ENERGY_RESISTANCE];

    return pl;
}

#if 0
unsigned char player_fast_run(void)
{
    int fr = 0;

    /* armour: (checks boots only) */
    if (you.equip[EQ_BOOTS] != -1
        && you.inv_dam[you.equip[EQ_BOOTS]] % 30 == SPARM_RUNNING)
    {
        fr++;
    }

    if (you.duration[DUR_SWIFTNESS])
        fr++;

    /* Mutations */
    if (you.mutation[MUT_FAST] > 0)
        fr++;

    /* transformations */
    if (!player_is_shapechanged())
    {
        // Centaurs and spriggans are only fast in their regular
        // shape (ie untransformed, blade hands, lich form)
        if (you.species == SP_CENTAUR || you.species == SP_SPRIGGAN)
            fr++;
    }
    else if (you.attribute[ATTR_TRANSFORMATION] == TRAN_SPIDER)
        fr++;

    // swimming merfolk are fast
    if (you.species == SP_MERFOLK && player_in_water())
        fr++;

    return fr;
}
#endif

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

    if (you.species == SP_MERFOLK && player_in_water())
    {
        // This is swimming... so it doesn't make sense to really
        // apply the other things (the mutation is "cover ground",
        // swiftness is an air spell, can't wear boots, can't be
        // transformed).
        mv = 6;
    }
    else
    {
        /* transformations */
        if (!player_is_shapechanged())
        {
            // Centaurs and spriggans are only fast in their regular
            // shape (ie untransformed, blade hands, lich form)
            if (you.species == SP_CENTAUR)
                mv = 8;
            else if (you.species == SP_SPRIGGAN)
                mv = 6;
        }
        else if (you.attribute[ATTR_TRANSFORMATION] == TRAN_SPIDER)
            mv = 8;

        /* armour: (checks boots only) */
        if (you.equip[EQ_BOOTS] != -1
            && you.inv_dam[you.equip[EQ_BOOTS]] % 30 == SPARM_RUNNING)
        {
            mv -= 2;
        }

        // Now when it says "you feel a little slower" you really are:
        if (you.duration[DUR_SWIFTNESS] > 6)
            mv -= 2;
        else if (you.duration[DUR_SWIFTNESS] > 0)
            mv--;

        /* Mutations: -2, -3, -4 */
        if (you.mutation[MUT_FAST] > 0)
            mv -= (you.mutation[MUT_FAST] + 1);
    }

    // We'll use the old value of six as a minimum, with haste this could
    // end up with a speed of three, which is about as fast as we want
    // the player to be able to go (since 3 is 3.33x as fast and 2 is 5x
    // which is a bit of a jump, and a bit too fast) -- bwr
    if (mv < 6)
        mv = 6;

    // Only one movement slowing thing so far... and we'll apply it in
    // the traditional fashion (ie a mutiplier not an addition).
    if (you.species == SP_NAGA && !player_is_shapechanged())
    {
        mv *= 14;
        mv /= 10;
    }

    return (mv);
}

// This function differs from the above in that it's used to set the
// initial time_taken value for the turn.  Everything else (movement,
// spellcasting, combat) applies a ratio to this value.
int player_speed(void)
{
    int ps = 10;

    if (you.haste)
        ps /= 2;

    switch (you.attribute[ATTR_TRANSFORMATION])
    {
    case TRAN_STATUE:
        ps *= 15;
        ps /= 10;
        break;
    case TRAN_SERPENT_OF_HELL:
        ps *= 12;
        ps /= 10;
        break;
    default:
        break;
    }

    return ps;
}

int player_AC(void)
{
    int AC = 0;
    int i;                      // loop variable
    const int racial_type = ((player_genus(GENPC_DWARVEN)) ? DARM_DWARVEN :
                             (player_genus(GENPC_ELVEN))   ? DARM_ELVEN :
                             (you.species == SP_HILL_ORC)  ? DARM_ORCISH
                                                           : DARM_PLAIN);

    for (i = EQ_CLOAK; i < EQ_LEFT_RING; i++)
    {
        if (you.equip[i] == -1 || i == EQ_SHIELD)
            continue;

        if (you.inv_plus[you.equip[i]] > 130)
            AC += you.inv_plus[you.equip[i]] - 150;
        else
            AC += you.inv_plus[you.equip[i]] - 50;

        // do we mean to exclude caps here? {dlb}
        // I guess yes -- they're not real armour so there's no base AC -- bwr
        if (i == EQ_HELMET && you.inv_plus2[you.equip[i]] >= 2)
            continue;

        if (i == EQ_BOOTS && you.inv_plus2[you.equip[i]] != 0)
            AC += 3;            /* barding */

        int racial_bonus = 0;
        const int armour_race = (int) (you.inv_dam[you.equip[i]]);

        // Dwarven armour is universally good -- bwr
        if (armour_race == DARM_DWARVEN)
            racial_bonus += 2;

        if (racial_type != DARM_PLAIN && armour_race == racial_type)
        {
            // Elven armour is too flexible, but still gives one
            // level to elves.  Orcish and Dwarven armour are worth +2
            // to the correct species, plus the plus that anyone
            // gets with dwarven armour. -- bwr

            if (racial_type == DARM_ELVEN)
                racial_bonus++;
            else
                racial_bonus += 2;
        }

        AC += property(OBJ_ARMOUR, you.inv_type[you.equip[i]], PARM_AC)
                            * (15 + you.skills[SK_ARMOUR] + racial_bonus) / 15;

        /* Nagas/Centaurs/the deformed don't fit into body armour very well */
        if ((you.species == SP_NAGA || you.species == SP_CENTAUR
             || you.mutation[MUT_DEFORMED] > 0) && i == EQ_BODY_ARMOUR)
        {
            AC -= property(OBJ_ARMOUR, you.inv_type[you.equip[i]], PARM_AC) / 2;
        }
    }

    if (you.equip[EQ_RIGHT_RING] != -1
        && you.inv_type[you.equip[EQ_RIGHT_RING]] == RING_PROTECTION)
    {
        if (you.inv_plus[you.equip[EQ_RIGHT_RING]] > 130)
            AC -= 100;

        AC += you.inv_plus[you.equip[EQ_RIGHT_RING]];
        AC -= 50;
    }

    if (you.equip[EQ_LEFT_RING] != -1
        && you.inv_type[you.equip[EQ_LEFT_RING]] == RING_PROTECTION)
    {
        if (you.inv_plus[you.equip[EQ_LEFT_RING]] > 130)
            AC -= 100;

        AC += you.inv_plus[you.equip[EQ_LEFT_RING]];
        AC -= 50;
    }

    if (you.equip[EQ_WEAPON] != -1
        && you.inv_class[you.equip[EQ_WEAPON]] == OBJ_WEAPONS
        && you.inv_dam[you.equip[EQ_WEAPON]] % 30 == SPWPN_PROTECTION)
    {
        AC += 5;
    }

    if (you.equip[EQ_SHIELD] != -1
        && you.inv_dam[you.equip[EQ_SHIELD]] % 30 == SPARM_PROTECTION)
    {
        AC += 3;
    }

    AC += scan_randarts(RAP_AC);

#ifdef USE_HARDER_AC_RULES
    if (you.attribute[ATTR_TRANSFORMATION] == TRAN_NONE
        || you.attribute[ATTR_TRANSFORMATION] == TRAN_BLADE_HANDS)
    {
#endif
        //jmf: only give:
        if (player_genus(GENPC_DRACONIAN))
        {
            if (you.experience_level < 8)
                AC += 2;
            else if (you.species == SP_GREY_DRACONIAN)
                AC += 1 + (you.experience_level - 4) / 2;
            else
                AC += 1 + (you.experience_level / 4);
        }
        else
        {
            switch (you.species)
            {
            case SP_NAGA:
                AC += you.experience_level / 3;
                break;
            case SP_OGRE:
                AC++;
                break;
            case SP_TROLL:
            case SP_CENTAUR:
                AC += 3;
                break;
            default:
                break;
            }
        }

#ifdef USE_HARDER_AC_RULES
    }
#endif

    if (you.duration[DUR_ICY_ARMOUR])
        AC += 4 + you.skills[SK_ICE_MAGIC] / 3;

    if (you.duration[DUR_STONEMAIL])
        AC += 5 + you.skills[SK_EARTH_MAGIC] / 2;

#ifdef USE_HARDER_AC_RULES
    if (you.attribute[ATTR_TRANSFORMATION] == TRAN_NONE
        || you.attribute[ATTR_TRANSFORMATION] == TRAN_BLADE_HANDS)
    {
#endif
        // Stoneskin doesn't work when transformed (unlike Icy Armour
        // and Stonemail) so it goes in here with the scales.
        if (you.duration[DUR_STONESKIN] > 0)
            AC += 1 + you.skills[SK_EARTH_MAGIC] / 5;

        // Scales -- some evil uses of the fact that boolean "true" == 1...
        // I'll spell things out with some comments -- bwr

        // mutations:
        // these give: +1, +2, +3
        AC += you.mutation[MUT_TOUGH_SKIN];
        AC += you.mutation[MUT_GREY_SCALES];
        AC += you.mutation[MUT_SPECKLED_SCALES];
        AC += you.mutation[MUT_IRIDESCENT_SCALES];
        AC += you.mutation[MUT_PATTERNED_SCALES];
        AC += you.mutation[MUT_BLUE_SCALES];

        // these gives: +1, +3, +5
        if (you.mutation[MUT_GREEN_SCALES] > 0)
            AC += (you.mutation[MUT_GREEN_SCALES] * 2) - 1;
        if (you.mutation[MUT_NACREOUS_SCALES] > 0)
            AC += (you.mutation[MUT_NACREOUS_SCALES] * 2) - 1;
        if (you.mutation[MUT_BLACK2_SCALES] > 0)
            AC += (you.mutation[MUT_BLACK2_SCALES] * 2) - 1;
        if (you.mutation[MUT_WHITE_SCALES] > 0)
            AC += (you.mutation[MUT_WHITE_SCALES] * 2) - 1;

        // these give: +2, +4, +6
        AC += you.mutation[MUT_GREY2_SCALES] * 2;
        AC += you.mutation[MUT_YELLOW_SCALES] * 2;
        AC += you.mutation[MUT_PURPLE_SCALES] * 2;

        // black gives: +3, +6, +9
        AC += you.mutation[MUT_BLACK_SCALES] * 3;

        // boney plates give: +2, +3, +4
        if (you.mutation[MUT_BONEY_PLATES] > 0)
            AC += you.mutation[MUT_BONEY_PLATES] + 1;

        // red gives +1, +2, +4
        AC += you.mutation[MUT_RED_SCALES]
                            + (you.mutation[MUT_RED_SCALES] == 3);

        // indigo gives: +2, +3, +5
        if (you.mutation[MUT_INDIGO_SCALES] > 0)
        {
            AC += 1 + you.mutation[MUT_INDIGO_SCALES]
                            + (you.mutation[MUT_INDIGO_SCALES] == 3);
        }

        // brown gives: +2, +4, +5
        AC += (you.mutation[MUT_BROWN_SCALES] * 2)
                            - (you.mutation[MUT_METALLIC_SCALES] == 3);

        // orange gives: +1, +3, +4
        AC += you.mutation[MUT_ORANGE_SCALES]
                            + (you.mutation[MUT_ORANGE_SCALES] > 1);

        // knobbly red gives: +2, +5, +7
        AC += (you.mutation[MUT_RED2_SCALES] * 2)
                            + (you.mutation[MUT_RED2_SCALES] > 1);

        // metallic gives +3, +7, +10
        AC += you.mutation[MUT_METALLIC_SCALES] * 3
                            + (you.mutation[MUT_METALLIC_SCALES] > 1);

#ifdef USE_HARDER_AC_RULES
    }
    else
    {
#endif
        // transformations:
        switch (you.attribute[ATTR_TRANSFORMATION])
        {
        case TRAN_NONE:
        case TRAN_BLADE_HANDS:
            break;
        case TRAN_SPIDER:
        case TRAN_ICE_BEAST:
            AC += 2;
            break;
        case TRAN_LICH:
            AC += 3;
            break;
        case TRAN_DRAGON:
            AC += 7;
            break;
        case TRAN_STATUE:
            AC += 20;
            break;
        case TRAN_SERPENT_OF_HELL:
            AC += 10;
            break;
        case TRAN_AIR:
            AC = 20;            // air - scales & species ought to be irrelevent
            break;
        default:
            break;
        }

#ifdef USE_HARDER_AC_RULES
    }
#endif

    return AC;
}

bool is_light_armour( int inv_type, int inv_dam )
{
    if (inv_dam / 30 == DARM_ELVEN)
        return (true);

    switch (inv_type)
    {
    case ARM_ROBE:
    case ARM_ANIMAL_SKIN:
    case ARM_LEATHER_ARMOUR:
    case ARM_STEAM_DRAGON_HIDE:
    case ARM_STEAM_DRAGON_ARMOUR:
    case ARM_MOTTLED_DRAGON_HIDE:
    case ARM_MOTTLED_DRAGON_ARMOUR:
        //case ARM_TROLL_HIDE: //jmf: these are knobbly and stiff
        //case ARM_TROLL_LEATHER_ARMOUR:
        return true;

    default:
        return false;
    }
}

bool player_light_armour(void)
{
    if (you.equip[EQ_BODY_ARMOUR] == -1)
        return true;

    return (is_light_armour( you.inv_type[you.equip[EQ_BODY_ARMOUR]],
                             you.inv_dam[you.equip[EQ_BODY_ARMOUR]] ));
}                               // end player_light_armour()

//
// This function returns true if the player has a radically different
// shape... minor changes like blade hands don't count, also note
// that lich transformation doesn't change the character's shape
// (so we end up with Naga-lichs, Spiggan-lichs, Minotaur-lichs)
// it just makes the character undead (with the benefits that implies). --bwr
//
bool player_is_shapechanged(void)
{
    if (you.attribute[ATTR_TRANSFORMATION] == TRAN_NONE
        || you.attribute[ATTR_TRANSFORMATION] == TRAN_BLADE_HANDS
        || you.attribute[ATTR_TRANSFORMATION] == TRAN_LICH)
    {
        return (false);
    }

    return (true);
}

int player_evasion(void)
{
    int ev = 10;

    int armour_ev_penalty;

    if (you.equip[EQ_BODY_ARMOUR] == -1)
        armour_ev_penalty = 0;
    else
    {
        armour_ev_penalty = property( OBJ_ARMOUR,
                                    you.inv_type[you.equip[EQ_BODY_ARMOUR]],
                                    PARM_EVASION);
    }

    // We return 1 here to give the player some chance of not being hit
    if (you.paralysis)
        return (1 + you.mutation[MUT_REPULSION_FIELD]);  //jmf: added mutation

    if (you.species == SP_CENTAUR)
        ev -= 3;

    if (you.equip[EQ_BODY_ARMOUR] != -1)
    {
        int ev_change = 0;

        ev_change = armour_ev_penalty;
        ev_change += you.skills[SK_ARMOUR] / 2; // lowered from / 3 {bwross}

        if (ev_change > armour_ev_penalty / 3)
            ev_change = armour_ev_penalty / 3;

        ev += ev_change;        /* remember that it's negative */
    }

    if (you.equip[EQ_RIGHT_RING] != -1
        && you.inv_type[you.equip[EQ_RIGHT_RING]] == RING_EVASION)
    {
        if (you.inv_plus[you.equip[EQ_RIGHT_RING]] > 130)
            ev -= 100;

        ev += you.inv_plus[you.equip[EQ_RIGHT_RING]];
        ev -= 50;
    }

    if (you.equip[EQ_LEFT_RING] != -1
        && you.inv_type[you.equip[EQ_LEFT_RING]] == RING_EVASION)
    {
        if (you.inv_plus[you.equip[EQ_LEFT_RING]] > 130)
            ev -= 100;

        ev += you.inv_plus[you.equip[EQ_LEFT_RING]];
        ev -= 50;
    }

    if (you.duration[DUR_STONEMAIL])
        ev -= 2;
    if (you.duration[DUR_FORESCRY])
        ev += 8;                //jmf: is this a reasonable value?

    int emod = 0;

    if (!player_light_armour())
    {
        // meaning that the armour evasion modifier is often effectively
        // applied twice, but not if you're wearing elven armour
        emod += (armour_ev_penalty * 14) / 10;
    }

    emod += you.skills[SK_DODGING] / 2; // too generous?

    if (emod > 0)
        ev += emod;

    if (you.mutation[MUT_REPULSION_FIELD] > 0)
        ev += (you.mutation[MUT_REPULSION_FIELD] * 2) - 1;

    switch (you.attribute[ATTR_TRANSFORMATION])
    {
    // case TRAN_SPIDER:  // used to be +3, now it's -3? +3 makes more sense
                          // but spider form is already good enough -- bwr
    case TRAN_DRAGON:
        ev -= 3;
        break;
    case TRAN_STATUE:
    case TRAN_SERPENT_OF_HELL:
        ev -= 5;
        break;
    case TRAN_AIR:
        ev += 20;
        break;
    default:
        break;
    }

    ev += scan_randarts(RAP_EVASION);

    return ev;
}                               // end player_evasion()

int player_mag_abil(bool is_weighted)
{
    int ma = 0;

    if (you.equip[EQ_LEFT_RING] != -1
        && you.inv_type[you.equip[EQ_LEFT_RING]] == RING_WIZARDRY)
    {
        ma += 3;
    }

    if (you.equip[EQ_RIGHT_RING] != -1
        && you.inv_type[you.equip[EQ_RIGHT_RING]] == RING_WIZARDRY)
    {
        ma += 3;
    }

    /* Staves */
    if (you.equip[EQ_WEAPON] != -1
        && you.inv_class[you.equip[EQ_WEAPON]] == OBJ_STAVES
        && you.inv_type[you.equip[EQ_WEAPON]] == STAFF_WIZARDRY)
    {
        ma += 4;
    }

    /* armour of the Archmagi (checks body armour only) */
    if (you.equip[EQ_BODY_ARMOUR] != -1
        && you.inv_dam[you.equip[EQ_BODY_ARMOUR]] % 30 == SPARM_ARCHMAGI)
    {
        ma += 2;
    }

    return ((is_weighted) ? ((ma * you.intel) / 10) : ma);
}                               // end player_mag_abil()

int player_shield_class(void)   //jmf: changes for new spell
{
    int base_shield = 0;

    if (you.equip[EQ_SHIELD] == -1)
    {
        if (!you.shock_shield && you.duration[DUR_CONDENSATION_SHIELD])
            base_shield += 2 + (you.skills[SK_ICE_MAGIC] / 6);
        else
            return 0;
    }
    else
    {
        switch (you.inv_type[you.equip[EQ_SHIELD]])
        {
        case ARM_BUCKLER:
            base_shield += 3;
            break;
        case ARM_LARGE_SHIELD:
            base_shield += 7;
            break;
        case ARM_SHIELD:
            base_shield += 5;
            break;
        }

        base_shield *= 10 + (you.skills[SK_SHIELDS] / 2);
        base_shield /= 10;

        if (you.inv_plus[you.equip[EQ_SHIELD]] > 130)
            base_shield += you.inv_plus[you.equip[EQ_SHIELD]] - 150;
        else
            base_shield += you.inv_plus[you.equip[EQ_SHIELD]] - 50;
    }

    return base_shield;
}                               // end player_shield_class()

unsigned char player_see_invis(void)
{
    unsigned char si = 0;

    if (you.equip[EQ_LEFT_RING] != -1
        && you.inv_type[you.equip[EQ_LEFT_RING]] == RING_SEE_INVISIBLE)
    {
        si++;
    }

    if (you.equip[EQ_RIGHT_RING] != -1
        && you.inv_type[you.equip[EQ_RIGHT_RING]] == RING_SEE_INVISIBLE)
    {
        si++;
    }

    /* armour: (checks head armour only) */
    if (you.equip[EQ_HELMET] != -1
        && you.inv_dam[you.equip[EQ_HELMET]] % 30 == SPARM_SEE_INVISIBLE)
    {
        si++;
    }

    /* Nagas & Spriggans have good eyesight */
    if (you.species == SP_NAGA || you.species == SP_SPRIGGAN)
        si++;

    if (you.mutation[MUT_ACUTE_VISION] > 0)
        si += you.mutation[MUT_ACUTE_VISION];

    //jmf: added see_invisible spell
    if (you.duration[DUR_SEE_INVISIBLE] > 0)
        si++;

    /* randart wpns */
    int artefacts = scan_randarts(RAP_EYESIGHT);

    if (artefacts > 0)
        si += artefacts;

    return si;
}

unsigned char player_sust_abil(void)
{
    unsigned char sa = 0;

    if (you.equip[EQ_LEFT_RING] != -1
        && you.inv_type[you.equip[EQ_LEFT_RING]] == RING_SUSTAIN_ABILITIES)
    {
        sa++;
    }

    if (you.equip[EQ_RIGHT_RING] != -1
        && you.inv_type[you.equip[EQ_RIGHT_RING]] == RING_SUSTAIN_ABILITIES)
    {
        sa++;
    }

    if (you.species == SP_MUMMY) // to balance lack of means to restore stats
        sa++;

    return sa;
}                               // end player_sust_abil()

int carrying_capacity(void)
{
    // Originally: 1000 + you.strength * 200 + ( you.levitation ? 1000 : 0 )
    return (3500 + (you.strength * 100) + (you.levitation ? 1000 : 0));
}

int burden_change(void)
{
    char old_burdenstate = you.burden_state;

    you.burden = 0;

    int max_carried = carrying_capacity();

    if (you.duration[DUR_STONEMAIL])
        you.burden += 800;

    for (unsigned char bu = 0; bu < ENDOFPACK; bu++)
    {
        if (you.inv_quantity[bu] < 1)
            continue;

        if (you.inv_class[bu] == OBJ_CORPSES)
        {
            if (you.inv_type[bu] == CORPSE_BODY)
                you.burden += mons_weight(you.inv_plus[bu]);
            else if (you.inv_type[bu] == CORPSE_SKELETON)
                you.burden += mons_weight(you.inv_plus[bu]) / 2;
        }
        else
        {
            you.burden += mass(you.inv_class[bu], you.inv_type[bu])
                                                    * you.inv_quantity[bu];
        }
    }

    you.burden_state = BS_UNENCUMBERED;
    you.redraw_burden = 1;

    // changed the burdened levels to match the change to max_carried
    if (you.burden < (max_carried * 5) / 6)
    // (you.burden < max_carried - 1000)
    {
        you.burden_state = BS_UNENCUMBERED;

        // this message may have to change, just testing {dlb}
        if (old_burdenstate != you.burden_state)
            mpr("You possessions no longer seem quite so burdensome.");
    }
    else if (you.burden < (max_carried * 11) / 12)
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

    return you.burden;
}                               // end burden_change()

bool you_resist_magic(int power)
{
    int ench_power = power;

    // nested if's rather than stacked 'em uglier than before but slightly
    // more efficient 16jan2000 {dlb}
    if (ench_power > 40)
    {
        ench_power = ((ench_power - 40) / 2) + 40;
        if (ench_power > 70)
        {
            ench_power = ((ench_power - 70) / 2) + 70;
            if (ench_power > 90)
            {
                ench_power = ((ench_power - 90) / 2) + 90;
                if (ench_power > 120)
                    ench_power = 120;
            }
        }
    }     // NB: same steppings as in beam::check_mons_magres() {dlb}

    int mrchance = 100 + player_res_magic();

    mrchance -= ench_power;

    int mrch2 = random2(100) + random2(101);

#ifdef WIZARD
    strcpy(info, "Pow:");
    itoa(ench_power, st_prn, 10);
    strcat(info, st_prn);
    strcat(info, ", mrs: ");
    itoa(player_res_magic(), st_prn, 10);
    strcat(info, st_prn);
    strcat(info, ", mrchance:");
    itoa(mrchance, st_prn, 10);
    strcat(info, st_prn);
    strcat(info, ", mrch2:");
    itoa(mrch2, st_prn, 10);
    strcat(info, st_prn);
    mpr(info);
#endif

    if (mrch2 < mrchance)
        return true;            // ie saved successfully

    return false;
/* if (random2(power) / 3 + random2(power) / 3 + random2(power) / 3 >= player_res_magic()) return 0;
   return 1; */
}

void forget_map(unsigned char chance_forgotten)
{
    unsigned char xcount, ycount = 0;

    for (xcount = 0; xcount < GXM; xcount++)
    {
        for (ycount = 0; ycount < GYM; ycount++)
        {
            if (random2(100) < chance_forgotten)
                env.map[xcount][ycount] = 0;
        }
    }
}                               // end forget_map()

void gain_exp(unsigned int exp_gained)
{

    if (you.equip[EQ_BODY_ARMOUR] != -1
        && you.inv_dam[you.equip[EQ_BODY_ARMOUR]] % 30 == SPARM_ARCHMAGI
        && !one_chance_in(20))
    {
        return;
    }

    if (you.experience + exp_gained > 8999999)
        you.experience = 8999999;
    else
        you.experience += exp_gained;

    if (you.exp_available + exp_gained > 20000)
        you.exp_available = 20000;
    else
        you.exp_available += exp_gained;

    level_change();
}                               // end gain_exp()

// Look at this !!!!    // ok,ok. I am. {dlb}
void level_change(void)
{
    // why does this need to be ten characters long, exactly? {dlb}
    char temp_quant[10];

    // necessary for the time being, as level_change() is called
    // directly sometimes {dlb}
    you.redraw_experience = 1;

    while (you.experience_level < 27
        && you.experience > exp_needed(you.experience_level + 2))
    {
        you.experience_level++;

        strcpy(info, "You are now a level ");
        itoa(you.experience_level, temp_quant, 10);
        strcat(info, temp_quant);
        strcat(info, " ");
        strcat(info, you.class_name);
        strcat(info, "!");
        mpr(info, MSGCH_INTRINSIC_GAIN);
        more();

/* ******************************************************************

// this appears to have been functionality for handing out "priestly"
// spells to paladins and priests -- no idea when it was first commented
// out, but the code is a little behind the times, the idea behind it
// seems not to be part of Crawl anymore, and it should probably be
// struck out of the codebase entirely. 19may2000 {dlb}

    scrloc = 0;

    int priest_pass[10];

    switch( you.char_class )
    {
      case 2: // priest
        if ( you.experience_level > you.max_level && !(you.experience_level % 3) )
        {
            if ( you.spell_no > 20 )
              break;

            prtry2 = you.experience_level / 3;

            if ( you.experience_level > 12 )
              prtry2 = (you.experience_level - 12) / 2 + 4;

            priest_spells(priest_pass, 1);

            if ( prtry2 > 7 || priest_pass[prtry2] == -1 )
              goto get_out;

            prtry = priest_pass[prtry2];

            for (prtry3 = 0; prtry3 < 25; prtry3++)
              if ( you.spells[prtry3] == SPELL_NO_SPELL )
              {
                  you.spells[prtry3] = prtry;
                  you.spell_no++;
                  you.spell_levels -= spell_difficulty(prtry);
                  break;
              }

            mpr("You have been granted a spell!");
        }
        break;

      case 6: // paladin
        if ( you.experience_level > you.max_level && !(you.experience_level % 4) )
        {
            if ( you.spell_no > 20 )
              break;

            prtry2 = you.experience_level / 4;

            if ( you.experience_level > 16 )
              prtry2 = (you.experience_level - 16) / 2 + 4;

            switch( prtry2 )
            {
              case 1:
                prtry = 46;    // repel undead
                break;
              case 2:
                prtry = 38;    // lesser healing
                break;
              case 3:
                prtry = 65;    // heal other
                break;
              case 4:
                prtry = 45;    // smiting
                break;
              case 5:
                prtry = 39;    // greater healing
                break;
              case 6:
                prtry = 41;    // purification
                break;
              case 7:
                prtry = 47;    // holy word
                break;
              default:
                goto get_out;
            }

            for (prtry3 = 0; prtry3 < 25; prtry3++)
              if ( you.spells [prtry3] == SPELL_NO_SPELL )
              {
                  you.spells[prtry3] = prtry;
                  you.spell_no++;
                  you.spell_levels -= spell_difficulty(prtry);
                  break;
              }

            mpr("You have been granted a spell!");
        }
        break;

      default:
get_out:
        break;
    }

****************************************************************** */

        int brek = 0;

        if (you.experience_level > 21)
            brek = (coinflip() ? 3 : 2);
        else if (you.experience_level > 12)
            brek = 3 + random2(3);      // up from 2 + rand(3) -- bwr
        else
            brek = 4 + random2(4);      // up from 3 + rand(4) -- bwr

        you.hp += brek;
        you.base_hp2 += brek;

        you.magic_points++;
        you.base_magic_points2++;

        char hp_adjust = 0;
        char mp_adjust = 0;

        if (you.experience_level > you.max_level)
        {
            if (!(you.experience_level % 3))
                ability_increase();

            switch (you.species)
            {
            case SP_HUMAN:
                if (!(you.experience_level % 5))
                    modify_stat(STAT_RANDOM, 1, false);
                break;

            case SP_ELF:
                if (you.experience_level % 3)
                    hp_adjust--;
                else
                    mp_adjust++;

                if (!(you.experience_level % 4))
                {
                    modify_stat( (coinflip() ? STAT_INTELLIGENCE
                                                : STAT_DEXTERITY), 1, false );
                }
                break;

            case SP_HIGH_ELF:
                if (you.experience_level % 3)
                    hp_adjust--;

                if (!(you.experience_level % 2))
                    mp_adjust++;

                if (!(you.experience_level % 3))
                {
                    modify_stat( (coinflip() ? STAT_INTELLIGENCE
                                                : STAT_DEXTERITY), 1, false );
                }
                break;

            case SP_GREY_ELF:
#ifdef USE_ELVISH_GLAMOUR_ABILITY
                if (you.experience_level == 5)
                {
                    //jmf: got Glamour ability
                    mpr("You feel charming!", MSGCH_INTRINSIC_GAIN);
                    mp_adjust++;
                }
#endif
                if (you.experience_level < 14)
                    hp_adjust--;

                if (you.experience_level % 3)
                    mp_adjust++;

                if (!(you.experience_level % 4))
                {
                    modify_stat( (coinflip() ? STAT_INTELLIGENCE
                                                : STAT_DEXTERITY), 1, false );
                }

                break;

            case SP_DEEP_ELF:
                if (you.experience_level < 17)
                    hp_adjust--;
                if (!(you.experience_level % 3))
                    hp_adjust--;

                mp_adjust++;

                if (!(you.experience_level % 4))
                    modify_stat(STAT_INTELLIGENCE, 1, false);
                break;

            case SP_SLUDGE_ELF:
                if (you.experience_level % 3)
                    hp_adjust--;
                else
                    mp_adjust++;

                if (!(you.experience_level % 4))
                {
                    modify_stat( (coinflip() ? STAT_INTELLIGENCE
                                                : STAT_DEXTERITY), 1, false );
                }
                break;

            case SP_HILL_DWARF:
                // lowered because of HD raise -- bwr
                // if (you.experience_level < 14)
                //     hp_adjust++;

                if (you.experience_level % 3)
                    hp_adjust++;

                if (!(you.experience_level % 2))
                    mp_adjust--;

                if (!(you.experience_level % 4))
                    modify_stat(STAT_STRENGTH, 1, false);
                break;

            case SP_MOUNTAIN_DWARF:
                // lowered because of HD raise -- bwr
                // if (you.experience_level < 14)
                //     hp_adjust++;

                if (!(you.experience_level % 2))
                    hp_adjust++;

                if (!(you.experience_level % 3))
                    mp_adjust--;

                if (!(you.experience_level % 4))
                    modify_stat(STAT_STRENGTH, 1, false);
                break;

            case SP_HALFLING:
                if (!(you.experience_level % 5))
                    modify_stat(STAT_DEXTERITY, 1, false);

                if (you.experience_level < 17)
                    hp_adjust--;

                if (!(you.experience_level % 2))
                    hp_adjust--;
                break;

            case SP_KOBOLD:
                if (!(you.experience_level % 5))
                {
                    modify_stat( (coinflip() ? STAT_STRENGTH
                                                : STAT_DEXTERITY), 1, false );
                }

                if (you.experience_level < 17)
                    hp_adjust--;

                if (!(you.experience_level % 2))
                    hp_adjust--;
                break;

            case SP_HILL_ORC:
                // lower because of HD raise -- bwr
                // if (you.experience_level < 17)
                //     hp_adjust++;

                if (!(you.experience_level % 2))
                    hp_adjust++;

                if (!(you.experience_level % 3))
                    mp_adjust--;

                if (!(you.experience_level % 5))
                    modify_stat(STAT_STRENGTH, 1, false);
                break;

            case SP_MUMMY:
                if (you.experience_level == 13 || you.experience_level == 26)
                    mpr("You feel more in touch with the powers of death.",
                        MSGCH_INTRINSIC_GAIN);
                break;

            case SP_NAGA:
                // lower because of HD raise -- bwr
                // if (you.experience_level < 14)
                //     hp_adjust++;

                hp_adjust++;

                if (!(you.experience_level % 4))
                    modify_stat(STAT_RANDOM, 1, false);

                if (!(you.experience_level % 3))
                {
                    mpr("Your skin feels tougher.", MSGCH_INTRINSIC_GAIN);
                    you.redraw_armor_class = 1;
                }
                break;

            case SP_GNOME:
                if (you.experience_level < 13)
                    hp_adjust--;

                if (!(you.experience_level % 3))
                    hp_adjust--;

                if (!(you.experience_level % 4))
                {
                    modify_stat( (coinflip() ? STAT_INTELLIGENCE
                                                : STAT_DEXTERITY), 1, false );
                }
                break;

            case SP_OGRE:
            case SP_TROLL:
                hp_adjust++;

                // lowered because of HD raise -- bwr
                // if (you.experience_level < 14)
                //     hp_adjust++;

                if (!(you.experience_level % 2))
                    hp_adjust++;

                if (you.experience_level % 3)
                    mp_adjust--;

                if (!(you.experience_level % 3))
                    modify_stat(STAT_STRENGTH, 1, false);
                break;

            case SP_OGRE_MAGE:
                hp_adjust++;

                // lowered because of HD raise -- bwr
                // if (you.experience_level < 14)
                //     hp_adjust++;

                if (!(you.experience_level % 5))
                {
                    modify_stat( (coinflip() ? STAT_INTELLIGENCE
                                                : STAT_STRENGTH), 1, false );
                }
                break;

            case SP_RED_DRACONIAN:
            case SP_WHITE_DRACONIAN:
            case SP_GREEN_DRACONIAN:
            case SP_GOLDEN_DRACONIAN:
/* Grey is later */
            case SP_BLACK_DRACONIAN:
            case SP_PURPLE_DRACONIAN:
            case SP_MOTTLED_DRACONIAN:
            case SP_PALE_DRACONIAN:
            case SP_UNK0_DRACONIAN:
            case SP_UNK1_DRACONIAN:
            case SP_UNK2_DRACONIAN:
                if (you.experience_level == 7)
                {
                    switch (you.species)
                    {
                    case SP_RED_DRACONIAN:
                        mpr("Your scales start taking on a fiery red colour.",
                            MSGCH_INTRINSIC_GAIN);
                        break;
                    case SP_WHITE_DRACONIAN:
                        mpr("Your scales start taking on an icy white colour.",
                            MSGCH_INTRINSIC_GAIN);
                        break;
                    case SP_GREEN_DRACONIAN:
                        mpr("Your scales start taking on a green colour.",
                            MSGCH_INTRINSIC_GAIN);
                        mpr("You feel resistant to poison.", MSGCH_INTRINSIC_GAIN);
                        break;

                    case SP_GOLDEN_DRACONIAN:
                        mpr("Your scales start taking on a golden yellow colour.", MSGCH_INTRINSIC_GAIN);
                        break;
                    case SP_BLACK_DRACONIAN:
                        mpr("Your scales start turning black.", MSGCH_INTRINSIC_GAIN);
                        break;
                    case SP_PURPLE_DRACONIAN:
                        mpr("Your scales start taking on a rich purple colour.", MSGCH_INTRINSIC_GAIN);
                        break;
                    case SP_MOTTLED_DRACONIAN:
                        mpr("Your scales start taking on a weird mottled pattern.", MSGCH_INTRINSIC_GAIN);
                        break;
                    case SP_PALE_DRACONIAN:
                        mpr("Your scales start fading to a pale grey colour.", MSGCH_INTRINSIC_GAIN);
                        break;
                    case SP_UNK0_DRACONIAN:
                    case SP_UNK1_DRACONIAN:
                    case SP_UNK2_DRACONIAN:
                        mpr("");
                        break;
                    }
                    more();
                    redraw_screen();
                }

                if (you.experience_level == 18)
                {
                    switch (you.species)
                    {
                    case SP_RED_DRACONIAN:
                        mpr("You feel resistant to fire.", MSGCH_INTRINSIC_GAIN);
                        break;
                    case SP_WHITE_DRACONIAN:
                        mpr("You feel resistant to cold.", MSGCH_INTRINSIC_GAIN);
                        break;
                    case SP_BLACK_DRACONIAN:
                        mpr("You feel resistant to electrical energy.",
                            MSGCH_INTRINSIC_GAIN);
                        break;
                    }
                }

                if (!(you.experience_level % 3))
                    hp_adjust++;

                if (you.experience_level > 7 && !(you.experience_level % 4))
                {
                    mpr("Your scales feel tougher.", MSGCH_INTRINSIC_GAIN);
                    you.redraw_armor_class = 1;
                    modify_stat(STAT_RANDOM, 1, false);
                }
                break;

            case SP_GREY_DRACONIAN:
                if (you.experience_level == 7)
                {
                    mpr("Your scales start turning grey.", MSGCH_INTRINSIC_GAIN);
                    more();
                    redraw_screen();
                }

                if (!(you.experience_level % 3))
                {
                    hp_adjust++;
                    if (you.experience_level > 7)
                        hp_adjust++;
                }

                if (you.experience_level > 7 && !(you.experience_level % 2))
                {
                    mpr("Your scales feel tougher.", MSGCH_INTRINSIC_GAIN);
                    you.redraw_armor_class = 1;
                }

                if ((you.experience_level > 7 && !(you.experience_level % 3))
                    || you.experience_level == 4 || you.experience_level == 7)
                {
                    modify_stat(STAT_RANDOM, 1, false);
                }
                break;

            case SP_CENTAUR:
                if (!(you.experience_level % 4))
                {
                    modify_stat( (coinflip() ? STAT_DEXTERITY
                                                : STAT_STRENGTH), 1, false );
                }

                // lowered because of HD raise -- bwr
                // if (you.experience_level < 17)
                //     hp_adjust++;

                if (!(you.experience_level % 2))
                    hp_adjust++;

                if (!(you.experience_level % 3))
                    mp_adjust--;
                break;

            case SP_DEMIGOD:
                if (!(you.experience_level % 3))
                    modify_stat(STAT_RANDOM, 1, false);

                // lowered because of HD raise -- bwr
                // if (you.experience_level < 17)
                //    hp_adjust++;

                if (!(you.experience_level % 2))
                    hp_adjust++;

                if (you.experience_level % 3)
                    mp_adjust++;
                break;

            case SP_SPRIGGAN:
                if (you.experience_level < 17)
                    hp_adjust--;

                if (you.experience_level % 3)
                    hp_adjust--;

                mp_adjust++;

                if (!(you.experience_level % 5))
                {
                    modify_stat( (coinflip() ? STAT_INTELLIGENCE
                                                : STAT_DEXTERITY), 1, false );
                }
                break;

            case SP_MINOTAUR:
                // lowered because of HD raise -- bwr
                // if (you.experience_level < 17)
                //     hp_adjust++;

                if (!(you.experience_level % 2))
                    hp_adjust++;

                if (!(you.experience_level % 2))
                    mp_adjust--;

                if (!(you.experience_level % 4))
                {
                    modify_stat( (coinflip() ? STAT_DEXTERITY
                                                : STAT_STRENGTH), 1, false );
                }
                break;

            case SP_DEMONSPAWN:
                if (you.attribute[ATTR_NUM_DEMONIC_POWERS] == 0
                    && (you.experience_level == 4
                        || (you.experience_level < 4 && one_chance_in(3))))
                {
                    demonspawn();
                }

                if (you.attribute[ATTR_NUM_DEMONIC_POWERS] == 1
                    && you.experience_level > 4
                    && (you.experience_level == 9
                        || (you.experience_level < 9 && one_chance_in(3))))
                {
                    demonspawn();
                }

                if (you.attribute[ATTR_NUM_DEMONIC_POWERS] == 2
                    && you.experience_level > 9
                    && (you.experience_level == 14
                        || (you.experience_level < 14 && one_chance_in(3))))
                {
                    demonspawn();
                }

                if (you.attribute[ATTR_NUM_DEMONIC_POWERS] == 3
                    && you.experience_level > 14
                    && (you.experience_level == 19
                        || (you.experience_level < 19 && one_chance_in(3))))
                {
                    demonspawn();
                }

                if (you.attribute[ATTR_NUM_DEMONIC_POWERS] == 4
                    && you.experience_level > 19
                    && (you.experience_level == 24
                        || (you.experience_level < 24 && one_chance_in(3))))
                {
                    demonspawn();
                }

                if (you.attribute[ATTR_NUM_DEMONIC_POWERS] == 5
                    && you.experience_level == 27)
                {
                    demonspawn();
                }

/*if (you.attribute [ATTR_NUM_DEMONIC_POWERS] == 6 && (you.experience_level == 8 || (you.experience_level < 8 && one_chance_in(3) ) )
   demonspawn(); */
                if (!(you.experience_level % 4))
                    modify_stat(STAT_RANDOM, 1, false);
                break;

            case SP_GHOUL:
                // lowered because of HD raise -- bwr
                // if (you.experience_level < 17)
                //     hp_adjust++;

                if (!(you.experience_level % 2))
                    hp_adjust++;

                if (!(you.experience_level % 3))
                    mp_adjust--;

                if (!(you.experience_level % 5))
                    modify_stat(STAT_STRENGTH, 1, false);
                break;

            case SP_KENKU:
                if (you.experience_level < 17)
                    hp_adjust--;

                if (!(you.experience_level % 3))
                    hp_adjust--;

                if (!(you.experience_level % 4))
                    modify_stat(STAT_RANDOM, 1, false);

                if (you.experience_level == 5)
                    mpr("You have gained the ability to fly.", MSGCH_INTRINSIC_GAIN);
                else if (you.experience_level == 15)
                    mpr("You can now fly continuously.", MSGCH_INTRINSIC_GAIN);
                break;

            case SP_MERFOLK:
                if (you.experience_level % 3)
                    hp_adjust++;

                if (!(you.experience_level % 5))
                    modify_stat(STAT_RANDOM, 1, false);
                break;
            }
        }

        // add hp and mp adjustments - GDL
        you.hp_max += hp_adjust;
        you.base_hp2 += hp_adjust;
        you.base_magic_points2 += mp_adjust;

        deflate_hp(you.hp_max, false);

        if (you.magic_points > you.max_magic_points)
            you.magic_points = you.max_magic_points;
        if (you.magic_points < 0)
            you.magic_points = 0;

        calc_hp();
        calc_mp();

        if (you.experience_level > you.max_level)
            you.max_level = you.experience_level;

        if (you.religion == GOD_XOM)
            Xom_acts(true, you.experience_level, true);
    }
}                               // end level_change()

// here's a question for you: does the ordering of mods make a difference?
// (yes) -- are these things in the right order of application to stealth?
// - 12mar2000 {dlb}
int check_stealth(void)
{
    if (you.special_wield == SPWLD_SHADOW)
        return (0);

    int stealth = you.dex * 3;

    if (you.skills[SK_STEALTH])
    {
        if (player_genus(GENPC_DRACONIAN))
            stealth += (you.skills[SK_STEALTH] * 12);
        else
        {
            switch (you.species)
            {
            case SP_TROLL:
            case SP_OGRE:
            case SP_OGRE_MAGE:
            case SP_CENTAUR:
                stealth += (you.skills[SK_STEALTH] * 9);
                break;
            case SP_MINOTAUR:
                stealth += (you.skills[SK_STEALTH] * 12);
                break;
            case SP_GNOME:
            case SP_HALFLING:
            case SP_KOBOLD:
            case SP_SPRIGGAN:
            case SP_NAGA:       // not small but very good at stealth
                stealth += (you.skills[SK_STEALTH] * 18);
                break;
            default:
                stealth += (you.skills[SK_STEALTH] * 15);
                break;
            }
        }
    }

    if (you.burden_state == BS_ENCUMBERED)
        stealth /= 2;
    else if (you.burden_state == BS_OVERLOADED)
        stealth /= 5;

    if (you.equip[EQ_BODY_ARMOUR] != -1 && !player_light_armour())
    {
        stealth -= mass( OBJ_ARMOUR, you.inv_type[you.equip[EQ_BODY_ARMOUR]] )
                                                                        / 10;
    }

    if (you.equip[EQ_CLOAK] != -1
        && you.inv_dam[you.equip[EQ_CLOAK]] / 30 == DARM_ELVEN)
    {
        stealth += 20;
    }

    if (you.equip[EQ_BOOTS] != -1)
    {
        if (you.inv_dam[you.equip[EQ_BOOTS]] % 30 == SPARM_STEALTH)
            stealth += 50;

        if (you.inv_dam[you.equip[EQ_BOOTS]] / 30 == DARM_ELVEN)
            stealth += 20;
    }

    if (you.levitation)
        stealth += 10;
    else if (grd[you.x_pos][you.y_pos] == DNGN_SHALLOW_WATER)
    {
        // Merfolk can sneak up on monsters underwater -- bwr
        if (you.species == SP_MERFOLK)
            stealth += 50;
        else
            stealth /= 2;
    }

    // Radiating silence is the negative compliment of shouting all the
    // time... a sudden change from background noise to no noise is going
    // to clue anything in to the fact that something is very wrong...
    // a personal silence spell would naturally be different, but this
    // silence radiates for a distance and prevents monster spellcasting,
    // which pretty much gives away the stealth game.
    if (you.duration[DUR_SILENCE])
        stealth -= 50;

    if (stealth < 0)
        stealth = 0;

    return stealth;
}                               // end check_stealth()

void ability_increase(void)
{
    unsigned char keyin;

    mpr("Your experience leads to an increase in your attributes!",
        MSGCH_INTRINSIC_GAIN);

    more();
    mesclr();

    mpr("Increase Strength, Intelligence, or Dexterity? ", MSGCH_PROMPT);

  get_key:
    keyin = getch();
    if (keyin == 0)
    {
        getch();
        goto get_key;
    }

    switch (keyin)
    {
    case 's':
    case 'S':
        modify_stat(STAT_STRENGTH, 1, false);
        return;

    case 'i':
    case 'I':
        modify_stat(STAT_INTELLIGENCE, 1, false);
        return;

    case 'd':
    case 'D':
        modify_stat(STAT_DEXTERITY, 1, false);
        return;
    }

    goto get_key;
/* this is an infinite loop because it is reasonable to assume that you're not going to want to leave it prematurely. */
}                               // end ability_increase()

void display_char_status(void)
{
    if (you.is_undead)
        mpr("You are undead.");
    else
        mpr("You are alive.");

    switch (you.attribute[ATTR_TRANSFORMATION])
    {
    case TRAN_SPIDER:
        mpr("You are in spider-form.");
        break;
    case TRAN_BLADE_HANDS:
        mpr("You have blades for hands.");
        break;
    case TRAN_STATUE:
        mpr("You are a statue.");
        break;
    case TRAN_ICE_BEAST:
        mpr("You are an ice creature.");
        break;
    case TRAN_DRAGON:
        mpr("You are in dragon-form.");
        break;
    case TRAN_LICH:
        mpr("You are in lich-form.");
        break;
    case TRAN_SERPENT_OF_HELL:
        mpr("You are a huge demonic serpent.");
        break;
    case TRAN_AIR:
        mpr("You are a cloud of diffuse gas.");
        break;
    }

    if (you.duration[DUR_LIQUID_FLAMES])
        mpr("You are covered in liquid flames.");

    if (you.duration[DUR_ICY_ARMOUR])
        mpr("You are protected by an icy shield.");

    if (you.duration[DUR_REPEL_MISSILES])
        mpr("You are protected from missiles.");

    if (you.duration[DUR_DEFLECT_MISSILES])
        mpr("You deflect missiles.");

    if (you.duration[DUR_PRAYER])
        mpr("You are praying.");

    if (you.duration[DUR_REGENERATION])
        mpr("You are regenerating.");

    if (you.duration[DUR_SWIFTNESS])
        mpr("You can move swiftly.");

    if (you.duration[DUR_INSULATION])
        mpr("You are insulated.");

    if (you.duration[DUR_STONEMAIL])
        mpr("You are covered in scales of stone.");

    if (you.duration[DUR_CONTROLLED_FLIGHT])
        mpr("You can control your flight.");

    if (you.duration[DUR_TELEPORT])
        mpr("You are about to teleport.");

    if (you.duration[DUR_CONTROL_TELEPORT])
        mpr("You can control teleportation.");

    if (you.duration[DUR_DEATH_CHANNEL])
        mpr("You are channeling the dead.");

    if (you.duration[DUR_FORESCRY])     //jmf: added 19mar2000
        mpr("You are forewarned.");

    if (you.duration[DUR_SILENCE])      //jmf: added 27mar2000
        mpr("You radiate silence.");

    if (you.duration[DUR_INFECTED_SHUGGOTH_SEED])       //jmf: added 19mar2000
        mpr("You are infected with a Shuggoth parasite.");

    if (you.duration[DUR_STONESKIN])
        mpr("Your skin is tough as stone.");

    if (you.invis)
        mpr("You are invisible.");

    if (you.conf)
        mpr("You are confused.");

    if (you.paralysis)
        mpr("You are paralysed.");

    if (you.exhausted)
        mpr("You are exhausted.");

    if (you.slow)
        mpr("You are moving very slowly.");

    if (you.haste)
        mpr("You are moving very quickly.");

    if (you.might)
        mpr("You are mighty.");

    if (you.berserker)
        mpr("You are possessed by a berserker rage.");

    if (you.levitation)
        mpr("You are hovering above the floor.");

    if (you.poison)
    {
        strcpy(info, "You are ");
        strcat(info, (you.poison > 10) ? "extremely" :
                     (you.poison > 5)  ? "very" :
                     (you.poison > 3)  ? "quite"
                                       : "mildly");
        strcat(info, " poisoned.");
        mpr(info);
    }

    if (you.deaths_door)
        mpr("You are standing in death's doorway.");

    if (you.disease)
    {
        strcpy(info, "You are ");
        strcat(info, (you.disease > 120) ? "badly " :
                     (you.disease >  40) ? ""
                                         : "mildly ");
        strcat(info, "diseased.");
        mpr(info);
    }

    if (you.magic_contamination > 5)
    {
        strcpy(info, "You are ");
        strcat(info, (you.magic_contamination > 25) ? "almost glowing" :
                     (you.magic_contamination > 15) ? "infused"
                                                    : "mildly infused");
        strcat(info, " with magical energy.");
        mpr(info);
    }

    if (you.confusing_touch)
    {
        strcpy(info, "Your hands are glowing ");
        strcat(info, (you.confusing_touch > 40) ? "an extremely bright" :
                     (you.confusing_touch > 20) ? "bright"
                                                : "a soft");
        strcat(info, " red.");
        mpr(info);
    }

    if (you.sure_blade)
    {
        strcpy(info, "You have a ");
        strcat(info, (you.sure_blade > 15) ? "strong " :
                     (you.sure_blade >  5) ? ""
                                           : "weak ");
        strcat(info, "bond with your blade.");
        mpr(info);
    }
}                               // end display_char_status()

void redraw_skill(const char your_name[kNameLen], const char class_name[40])
{
    char print_it[80];
    char print_it2[42];

    int i = 0;
    bool spaces = false;

    strcpy(print_it, your_name);
    strcat(print_it, " the ");
    strcat(print_it, class_name);

    strncpy(print_it2, print_it, 39);

    for (i = 0; i < 40; i++)
    {
        if (print_it2[i] == 0)
            spaces = true;
        if (spaces)
            print_it2[i] = ' ';
    }

    print_it2[39] = 0;

    textcolor(LIGHTGREY);
#ifdef DOS_TERM
    window(1, 1, 80, 25);
#endif
    gotoxy(40, 1);
    textcolor(LIGHTGREY);
    cprintf(print_it2);
}                               // end redraw_skill()

char *species_name(unsigned char speci)
{
    // Causes minor problems with ghosts, but nevermind
    // still the case? - 12mar200 {dlb}
    if (player_genus(GENPC_DRACONIAN) && you.experience_level < 7)
        return "Draconian";

    switch (speci)
    {
    case SP_HUMAN:
        return "Human";
    case SP_ELF:
        return "Elf";
    case SP_HIGH_ELF:
        return "High Elf";
    case SP_GREY_ELF:
        return "Grey Elf";
    case SP_DEEP_ELF:
        return "Deep Elf";
    case SP_SLUDGE_ELF:
        return "Sludge Elf";
    case SP_HILL_DWARF:
        return "Hill Dwarf";
    case SP_MOUNTAIN_DWARF:
        return "Mountain Dwarf";
    case SP_HALFLING:
        return "Halfling";
    case SP_HILL_ORC:
        return "Hill Orc";
    case SP_KOBOLD:
        return "Kobold";
    case SP_MUMMY:
        return "Mummy";
    case SP_NAGA:
        return "Naga";
    case SP_GNOME:
        return "Gnome";
    case SP_OGRE:
        return "Ogre";
    case SP_TROLL:
        return "Troll";
    case SP_OGRE_MAGE:
        return "Ogre-Mage";
    case SP_RED_DRACONIAN:
        return "Red Draconian";
    case SP_WHITE_DRACONIAN:
        return "White Draconian";
    case SP_GREEN_DRACONIAN:
        return "Green Draconian";
    case SP_GOLDEN_DRACONIAN:
        return "Yellow Draconian";
    case SP_GREY_DRACONIAN:
        return "Grey Draconian";
    case SP_BLACK_DRACONIAN:
        return "Black Draconian";
    case SP_PURPLE_DRACONIAN:
        return "Purple Draconian";
    case SP_MOTTLED_DRACONIAN:
        return "Mottled Draconian";
    case SP_PALE_DRACONIAN:
        return "Pale Draconian";
    case SP_UNK0_DRACONIAN:
    case SP_UNK1_DRACONIAN:
    case SP_UNK2_DRACONIAN:
        return "Draconian";
    case SP_CENTAUR:
        return "Centaur";
    case SP_DEMIGOD:
        return "Demigod";
    case SP_SPRIGGAN:
        return "Spriggan";
    case SP_MINOTAUR:
        return "Minotaur";
    case SP_DEMONSPAWN:
        return "Demonspawn";
    case SP_GHOUL:
        return "Ghoul";
    case SP_KENKU:
        return "Kenku";
    case SP_MERFOLK:
        return "Merfolk";
    default:
        return "Yak";
    }
}                               // end species_name()

bool wearing_amulet(char amulet)
{
    if (amulet == AMU_CONTROLLED_FLIGHT
        && (you.duration[DUR_CONTROLLED_FLIGHT]
            || player_genus(GENPC_DRACONIAN)
            || you.attribute[ATTR_TRANSFORMATION] == TRAN_DRAGON))
    {
        return true;
    }

    if (amulet == AMU_CLARITY && you.mutation[MUT_CLARITY])
        return true;

    if (amulet == AMU_RESIST_CORROSION || amulet == AMU_CONSERVATION)
    {
        if (you.equip[EQ_CLOAK] != -1
            && you.inv_dam[you.equip[EQ_CLOAK]] % 30 == SPARM_PRESERVATION)
        {
            // this is hackish {dlb}
            return true;
        }
    }

    if (you.equip[EQ_AMULET] == -1)
        return false;

    if (you.inv_type[you.equip[EQ_AMULET]] == amulet)
        return true;

    return false;
}                               // end wearing_amulet()

int species_exp_mod(char species)
{

    if (player_genus(GENPC_DRACONIAN))
        return 14;
    else if (player_genus(GENPC_DWARVEN))
        return 13;
    {
        switch (species)
        {
        case SP_HUMAN:
        case SP_HALFLING:
        case SP_HILL_ORC:
        case SP_KOBOLD:
            return 10;
        case SP_GNOME:
            return 11;
        case SP_ELF:
        case SP_SLUDGE_ELF:
        case SP_NAGA:
        case SP_GHOUL:
        case SP_MERFOLK:
            return 12;
        case SP_SPRIGGAN:
        case SP_KENKU:
            return 13;
        case SP_GREY_ELF:
        case SP_DEEP_ELF:
        case SP_OGRE:
        case SP_CENTAUR:
        case SP_MINOTAUR:
        case SP_DEMONSPAWN:
            return 14;
        case SP_HIGH_ELF:
        case SP_MUMMY:
        case SP_TROLL:
        case SP_OGRE_MAGE:
            return 15;
        case SP_DEMIGOD:
            return 16;
        default:
            return 0;
        }
    }
}                               // end species_exp_mod()

unsigned long exp_needed(int lev)
{
    lev--;

    unsigned long level = 0;

    switch (lev)
    {
    case 1:
        level = 1;
        break;
    case 2:
        level = 10;
        break;
    case 3:
        level = 35;
        break;
    case 4:
        level = 70;
        break;
    case 5:
        level = 120;
        break;
    case 6:
        level = 250;
        break;
    case 7:
        level = 510;
        break;
    case 8:
        level = 900;
        break;
    case 9:
        level = 1700;
        break;
    case 10:
        level = 3500;
        break;
    case 11:
        level = 8000;
        break;
    case 12:
        level = 20000;
        break;

    default:                    //return 14000 * (lev - 11);
        level = 20000 * (lev - 11) + ((lev - 11) * (lev - 11) * (lev - 11)) * 130;
        break;
    }

    return (level - 1) * species_exp_mod(you.species) / 10;
}                               // end exp_needed()

// returns bonuses from rings of slaying, etc.
int slaying_bonus(char which_affected)
{
    int to_hit = 0;
    int to_dam = 0;
    int i = 0;

    for (i = EQ_LEFT_RING; i < EQ_AMULET; i++)
    {
        if (you.equip[i] == -1 || you.inv_type[you.equip[i]] != RING_SLAYING)
            continue;

        int ghoggl = you.inv_plus[you.equip[i]] - 50;

        if (you.inv_plus[you.equip[i]] > 130)
            ghoggl -= 100;

        to_hit += ghoggl;
        to_dam += you.inv_plus2[you.equip[i]] - 50;
    }

    if (which_affected == PWPN_HIT)
    {
        to_hit += scan_randarts(RAP_ACCURACY);
        return to_hit;
    }

    if (which_affected == PWPN_DAMAGE)
    {
        to_dam += scan_randarts(RAP_DAMAGE);
        return to_dam;
    }

    return 0;
}                               // end slaying_bonus()

/* Checks each equip slot for a randart, and adds up all of those with
   a given property. Slow if any randarts are worn, so avoid where possible. */
int scan_randarts(char which_property)
{
    int i = 0;
    int retval = 0;

    for (i = EQ_WEAPON; i < EQ_LEFT_RING; i++)
    {
        if (you.equip[i] == -1)
            continue;
        if (i == EQ_WEAPON && you.inv_class[you.equip[i]] != OBJ_WEAPONS)
            continue;
        if (you.inv_dam[you.equip[i]] % 30 < 25)
            continue;           /* change for rings/amulets */

        retval += inv_randart_wpn_properties(you.equip[i], 0, which_property);
    }

    for (i = EQ_LEFT_RING; i < NUM_EQUIP; i++)  /* rings + amulets */
    {
        if (you.equip[i] == -1)
            continue;
        if (you.inv_dam[you.equip[i]] != 200
            && you.inv_dam[you.equip[i]] != 201)
            continue;

        retval += inv_randart_wpn_properties(you.equip[i], 0, which_property);
    }

    return retval;
}                               // end scan_randarts()

void modify_stat(unsigned char which_stat, char amount, bool suppress_msg)
{
    char *ptr_stat = NULL;
    char *ptr_stat_max = NULL;
    char *ptr_redraw = NULL;

    // sanity - is non-zero amount?
    if (amount == 0)
        return;

    if (!suppress_msg)
        strcpy(info, "You feel ");

    if (which_stat == STAT_RANDOM)
        which_stat = random2(NUM_STATS);

    switch (which_stat)
    {
    case STAT_STRENGTH:
        ptr_stat = &you.strength;
        ptr_stat_max = &you.max_strength;
        ptr_redraw = &you.redraw_strength;
        if (!suppress_msg)
            strcat(info, (amount > 0)?"stronger":"weaker");
        break;

    case STAT_DEXTERITY:
        ptr_stat = &you.dex;
        ptr_stat_max = &you.max_dex;
        ptr_redraw = &you.redraw_dexterity;
        if (!suppress_msg)
            strcat(info, (amount > 0)?"agile":"clumsy");
        break;

    case STAT_INTELLIGENCE:
        ptr_stat = &you.intel;
        ptr_stat_max = &you.max_intel;
        ptr_redraw = &you.redraw_intelligence;
        if (!suppress_msg)
            strcat(info, (amount > 0)?"clever":"stupid");
        break;
    }

    if (!suppress_msg)
    {
        strcat(info, ".");
        mpr(info, (amount > 0)?MSGCH_INTRINSIC_GAIN:MSGCH_WARN);
    }

    *ptr_stat += amount;
    *ptr_stat_max += amount;
    *ptr_redraw = 1;

    if (ptr_stat == &you.strength)
        burden_change();

    return;
}                               // end modify_stat()

void dec_hp(int hp_loss, bool fatal)
{
    if (hp_loss < 1)
        return;

    you.hp -= hp_loss;

    if (!fatal && you.hp < 1)
        you.hp = 1;

    you.redraw_hit_points = 1;

    return;
}                               // end dec_hp()

void dec_mp(int mp_loss)
{
    if (mp_loss < 1)
        return;

    you.magic_points -= mp_loss;

    if (you.magic_points < 0)
        you.magic_points = 0;

    you.redraw_magic_points = 1;

    return;
}                               // end dec_mp()

bool enough_hp(int minimum, bool suppress_msg)
{
    if (you.hp < minimum)
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

void inc_mp(int mp_gain, bool max_too)
{
    if (mp_gain < 1)
        return;

    you.magic_points += mp_gain;

    if (max_too)
        you.max_magic_points += mp_gain;
    else if (you.magic_points > you.max_magic_points)
        you.magic_points = you.max_magic_points;

    you.redraw_magic_points = 1;

    return;
}                               // end inc_mp()

void inc_hp(int hp_gain, bool max_too)
{
    if (hp_gain < 1)
        return;

    you.hp += hp_gain;

    if (max_too)
        you.hp_max += hp_gain;
    else if (you.hp > you.hp_max)
        you.hp = you.hp_max;

    you.redraw_hit_points = 1;

    return;
}                               // end inc_hp()

// use of floor: false = hp max, true = hp min {dlb}
void deflate_hp(int new_level, bool floor)
{
    if (floor && you.hp < new_level)
        you.hp = new_level;
    else if (!floor && you.hp > new_level)
        you.hp = new_level;

    // must remain outside conditional, given code usage {dlb}
    you.redraw_hit_points = 1;

    return;
}                               // end deflate_hp()

void set_hp(int new_amount, bool max_too)
{
    if (you.hp != new_amount)
        you.hp = new_amount;

    if (max_too && you.hp_max != new_amount)
        you.hp_max = new_amount;

    // must remain outside conditional, given code usage {dlb}
    you.redraw_hit_points = 1;

    return;
}                               // end set_hp()

void set_mp(int new_amount, bool max_too)
{

    if (you.magic_points != new_amount)
        you.magic_points = new_amount;

    if (max_too && you.max_magic_points != new_amount)
        you.max_magic_points = new_amount;

    // must remain outside conditional, given code usage {dlb}
    you.redraw_magic_points = 1;

    return;
}                               // end set_mp()

char *job_title(int which_job)
{
    switch (which_job)
    {
    case JOB_FIGHTER:
        return "Fighter";
    case JOB_WIZARD:
        return "Wizard";
    case JOB_PRIEST:
        return "Priest";
    case JOB_THIEF:
        return "Thief";
    case JOB_GLADIATOR:
        return "Gladiator";
    case JOB_NECROMANCER:
        return "Necromancer";
    case JOB_PALADIN:
        return "Paladin";
    case JOB_ASSASSIN:
        return "Assassin";
    case JOB_BERSERKER:
        return "Berserker";
    case JOB_HUNTER:
        return "Hunter";
    case JOB_CONJURER:
        return "Conjurer";
    case JOB_ENCHANTER:
        return "Enchanter";
    case JOB_FIRE_ELEMENTALIST:
        return "Fire Elementalist";
    case JOB_ICE_ELEMENTALIST:
        return "Ice Elementalist";
    case JOB_SUMMONER:
        return "Summoner";
    case JOB_AIR_ELEMENTALIST:
        return "Air Elementalist";
    case JOB_EARTH_ELEMENTALIST:
        return "Earth Elementalist";
    case JOB_CRUSADER:
        return "Crusader";
    case JOB_DEATH_KNIGHT:
        return "Death Knight";
    case JOB_VENOM_MAGE:
        return "Venom Mage";
    case JOB_CHAOS_KNIGHT:
        return "Knight of Chaos";
    case JOB_TRANSMUTER:
        return "Transmuter";
    case JOB_HEALER:
        return "Healer";
    case JOB_REAVER:
        return "Reaver";
    case JOB_STALKER:
        return "Stalker";
    case JOB_MONK:
        return "Monk";
    case JOB_WARPER:
        return "Warper";
    case JOB_WANDERER:
        return "Wanderer";
    default:
        return "bug hunter";
    }
}                               // end job_title()

// Try to keep all species and class abbreviations different, as
// it will make things a bit easier (ie. avoid HuTr (Human-Troll?)) -- bwr
char *species_abbrev(unsigned char which_species)
{
    if (player_genus(GENPC_DRACONIAN)) // was this further differentiated? {dlb}
        return "Dr";

    switch (which_species)
    {
    case SP_CENTAUR:            return "Ce";
    case SP_DEEP_ELF:           return "DE";
    case SP_DEMIGOD:            return "DG";
    case SP_DEMONSPAWN:         return "DS";
    case SP_ELF:                return "El";
    case SP_GHOUL:              return "Gh";
    case SP_GNOME:              return "Gn";
    case SP_HALFLING:           return "Ha";
    case SP_HILL_DWARF:         return "HD";
    case SP_HIGH_ELF:           return "HE";
    case SP_HILL_ORC:           return "HO";
    case SP_HUMAN:              return "Hu";
    case SP_KENKU:              return "Ke";
    case SP_KOBOLD:             return "Ko";
    case SP_GREY_ELF:           return "GE";
    case SP_MOUNTAIN_DWARF:     return "MD";
    case SP_MERFOLK:            return "Mf";
    case SP_MINOTAUR:           return "Mi";
    case SP_MUMMY:              return "Mu";
    case SP_NAGA:               return "Na";
    case SP_OGRE:               return "Og";
    case SP_OGRE_MAGE:          return "OM";
    case SP_SLUDGE_ELF:         return "SE";
    case SP_SPRIGGAN:           return "Sp";
    case SP_TROLL:              return "Tr";
    default:                    return "Yk";
    }
}                               // end species_abbrev()

char *class_abbrev( unsigned char which_class )
{
    switch (which_class)
    {
    case JOB_AIR_ELEMENTALIST:    return "AI";
    case JOB_ASSASSIN:            return "As";
    case JOB_BERSERKER:           return "Be";
    case JOB_CONJURER:            return "Cj";
    case JOB_CHAOS_KNIGHT:        return "CK";
    case JOB_CRUSADER:            return "Cr";
    case JOB_DEATH_KNIGHT:        return "DK";
    case JOB_EARTH_ELEMENTALIST:  return "EE";
    case JOB_ENCHANTER:           return "En";
    case JOB_FIRE_ELEMENTALIST:   return "FE";
    case JOB_FIGHTER:             return "Fi";
    case JOB_GLADIATOR:           return "Gl";
    case JOB_HEALER:              return "He";
    case JOB_HUNTER:              return "Hn";
    case JOB_ICE_ELEMENTALIST:    return "IE";
    case JOB_MONK:                return "Mo";
    case JOB_NECROMANCER:         return "Ne";
    case JOB_PALADIN:             return "Pa";
    case JOB_PRIEST:              return "Pr";
    case JOB_REAVER:              return "Re";
    case JOB_STALKER:             return "St";
    case JOB_SUMMONER:            return "Su";
    case JOB_THIEF:               return "Th";
    case JOB_TRANSMUTER:          return "Tm";
    case JOB_VENOM_MAGE:          return "VM";
    case JOB_WANDERER:            return "Wn";
    case JOB_WARPER:              return "Wr";
    case JOB_WIZARD:              return "Wz";
    default:                      return "XX";
    }
}                               // end class_abbrev()

bool player_descriptor(unsigned char which_descriptor, unsigned char species)
{
    if (species == SP_UNKNOWN)
        species = you.species;

    switch (which_descriptor)
    {
    case PDSC_UNDEAD:
        if (species == SP_GHOUL || species == SP_MUMMY)
            return true;
        break;

    default:
        return false;
    }

    return false;
}                               // end player_descriptor()

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

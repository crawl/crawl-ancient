/*
 *  File:       player.cc
 *  Summary:    Player related functions.
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 *      <6>      7/30/99        BWR             Added player_spell_levels()
 *      <5>      7/13/99        BWR             Added player_res_electricity()
 *                                              and player_hunger_rate()
 *      <4>      6/22/99        BWR             Racial adjustments to stealth
 *                                              and Armour.
 *      <3>      5/20/99        BWR             Fixed problems with random stat
 *                                              increases, added kobold stat
 *                                              increase.  increased EV
 *                                              recovery for Armour.
 *      <2>      5/08/99        LRH             display_char_status correctly handles magic_contamination.
 *      <1>      -/--/--        LRH             Created
 */

#include "AppHdr.h"
#include "player.h"

#ifdef DOS
#include <conio.h>
#endif

#include <string.h>
#include <stdlib.h>

#include "externs.h"
#include "enum.h"

#include "itemname.h"
#include "misc.h"
#include "mstruct.h"
#include "mutation.h"
#include "output.h"
#include "player.h"
#include "abyss.h"
#include "randart.h"
#include "religion.h"
#include "skills2.h"
#include "spells0.h"
#include "stuff.h"
#include "view.h"

#ifdef MACROS
  #include "macro.h"
#endif

/*
   you.duration []:
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
void increase_stats(char which_stat);
int scan_randarts(char which_property);


int player_teleport(void)
{
    int tp = 0;

/* rings */
    if (you.equip[EQ_LEFT_RING] != -1 && you.inv_type[you.equip[EQ_LEFT_RING]] == RING_TELEPORTATION)
        tp += 8;
    if (you.equip[EQ_RIGHT_RING] != -1 && you.inv_type[you.equip[EQ_RIGHT_RING]] == RING_TELEPORTATION)
        tp += 8;

/* mutations */
    tp += you.mutation[MUT_TELEPORT] * 3;

/* randart weapons only */
    if (you.equip[EQ_WEAPON] != -1 && you.inv_class[you.equip[EQ_WEAPON]] == OBJ_WEAPONS && you.inv_dam[you.equip[EQ_WEAPON]] % 30 >= 25)
        tp += scan_randarts(RAP_CAUSE_TELEPORTATION);

    return tp;
}

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
    if (you.duration[DUR_REGENERATION] != 0)
    {
        rr += 100;
    }

    /* troll leather */
    if (you.species == SP_TROLL)
    {
        rr += 40;
    }
    else if (you.equip[EQ_BODY_ARMOUR] != -1
        && you.inv_type[you.equip[EQ_BODY_ARMOUR]] == ARM_TROLL_LEATHER_ARMOUR)
    {
        /* trolls gain no benefit from troll leather */
        rr += 30;
    }


    /* fast heal mutation */
    rr += you.mutation[MUT_REGENERATION] * 20;

    /* ghouls heal slowly */
    if (you.species == SP_GHOUL)
        rr /= 2;

    return rr;
}


int player_hunger_rate(void)
{
    int hunger = 3;

    switch (you.species)
    {
    case SP_HALFLING:
    case SP_SPRIGGAN:
        hunger -= 1;
        break;

    case SP_OGRE:
    case SP_OGRE_MAGE:
    case SP_DEMIGOD:
        hunger += 1;
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
        hunger -= 1;

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
        if (you.inv_dam[you.equip[EQ_WEAPON]] < 180)
        {
            if (you.inv_dam[you.equip[EQ_WEAPON]] % 30 == SPWPN_VAMPIRICISM)
                hunger += 6;
        }
        else if (you.inv_dam[you.equip[EQ_WEAPON]] == NWPN_VAMPIRE_S_TOOTH)
        {
            hunger += 9;
        }
    }

    hunger += scan_randarts( RAP_METABOLISM );

    return hunger;
}


int player_spell_levels(void)
{
    int sl = (you.experience_level - 1) + you.skills[SK_SPELLCASTING] * 2;

    if (sl > 99)
        sl = 99;

    for (int i = 0; i < 25; i++)
    {
        if (you.spells[i] != 210)
        {
            sl -= spell_value( you.spells[i] );
        }
    }

    if (sl < 0)
        sl = 0;

    return (sl);
}


static int player_res_magic(void)
{
    int rm = you.experience_level * 3;

    switch (you.species)
    {
    case SP_HIGH_ELF:
    case SP_GREY_ELF:
    case SP_ELF:
    case SP_SLUDGE_ELF:
    case SP_HILL_DWARF:
    case SP_MOUNTAIN_DWARF:
        rm += you.experience_level;
        break;

    case SP_NAGA:
        rm += you.experience_level * 2;
        break;

    case SP_PURPLE_DRACONIAN:
    case SP_GNOME:
    case SP_DEEP_ELF:
        rm += you.experience_level * 3;
        break;

    case SP_SPRIGGAN:
        rm += you.experience_level * 4;
        break;
    }

/* armour: (checks cloak & body armour only) */
    if (you.equip[EQ_CLOAK] != -1 && you.inv_dam[you.equip[EQ_CLOAK]] % 30 == SPARM_MAGIC_RESISTANCE)
        rm += 30;
    if (you.equip[EQ_BODY_ARMOUR] != -1 && you.inv_dam[you.equip[EQ_BODY_ARMOUR]] % 30 == SPARM_MAGIC_RESISTANCE)
        rm += 30;
/* rings of magic resistance */
    if (you.equip[EQ_LEFT_RING] != -1 && you.inv_type[you.equip[EQ_LEFT_RING]] == RING_PROTECTION_FROM_MAGIC)
        rm += 40;
    if (you.equip[EQ_RIGHT_RING] != -1 && you.inv_type[you.equip[EQ_RIGHT_RING]] == RING_PROTECTION_FROM_MAGIC)
        rm += 40;
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
        break;                  /* Lich */
    }
    return rm;
}
/*        case 18: strcat(glog , "dragon scale mail"); break;
   case 19: strcat(glog , "troll leather armour"); break;
   case 20: strcat(glog , "ice dragon hide"); break;
   case 21: strcat(glog , "ice dragon scale mail"); break;

   case 22: strcat(glog , "steam dragon hide"); break;
   case 23: strcat(glog , "steam dragon scale mail"); break;
   case 24: strcat(glog , "mottled dragon hide"); break;
   case 25: strcat(glog , "mottled dragon scale mail"); break;
   case 26: strcat(glog , "storm dragon hide"); break;
   case 27: strcat(glog , "storm dragon scale mail"); break;
   case 28: strcat(glog , "gold dragon hide"); break;
   case 29: strcat(glog , "gold dragon scale mail"); break;

   case 1: strcat(glog, " of running"); break;
   case 2: strcat(glog, " of fire resistance"); break;
   case 3: strcat(glog, " of cold resistance"); break;
   case 4: strcat(glog, " of poison resistance"); break;
   case 5: strcat(glog, " of see invisible"); break;
   case 6: strcat(glog, " of darkness"); break;
   case 7: strcat(glog, " of strength"); break;
   case 8: strcat(glog, " of dexterity"); break;
   case 9: strcat(glog, " of intelligence"); break;
   case 10: strcat(glog, " of ponderousness"); break;
   case 11: strcat(glog, " of levitation"); break;
   case 12: strcat(glog, " of magic resistance"); break;
   case 13: strcat(glog, " of protection"); break;
   case 14: strcat(glog, " of stealth"); break;
   case 15: strcat(glog, " of resistance"); break;
   case 16: strcat(glog, " of positive energy"); break;
   case 17: strcat(glog, " of the Archmagi"); break;
   case 18: strcat(glog, " of preservation"); break;

   case 0: strcat(glog , "ring of regeneration"); break;
   case 1: strcat(glog , "ring of protection"); break;
   case 2: strcat(glog , "ring of protection from fire"); break;
   case 3: strcat(glog , "ring of poison resistance"); break;
   case 4: strcat(glog , "ring of protection from cold"); break;
   case 5: strcat(glog , "ring of strength"); break;
   case 6: strcat(glog , "ring of slaying"); break;
   case 7: strcat(glog , "ring of see invisible"); break;
   case 8: strcat(glog , "ring of invisibility"); break;
   case 9: strcat(glog , "ring of hunger"); break;
   case 10: strcat(glog , "ring of teleportation"); break;
   case 11: strcat(glog , "ring of evasion"); break;
   case 12: strcat(glog , "ring of sustain abilities"); break;
   case 13: strcat(glog , "ring of sustenance"); break;
   case 14: strcat(glog , "ring of dexterity"); break;
   case 15: strcat(glog , "ring of intelligence"); break;
   case 16: strcat(glog , "ring of wizardry"); break;
   case 17: strcat(glog , "ring of magical power"); break;
   case 18: strcat(glog , "ring of levitation"); break;
   case 19: strcat(glog , "ring of life protection"); break;
   case 20: strcat(glog , "ring of protection from magic"); break;
   case 21: strcat(glog , "ring of fire"); break;
   case 22: strcat(glog , "ring of ice"); break;
   case 23: strcat(glog , "ring of teleport control"); break;
   case 35: strcat(glog , "amulet of rage"); break;
   case 36: strcat(glog , "amulet of maintain speed"); break; // not foolproof
   case 37: strcat(glog , "amulet of clarity"); break; // not foolproof
   case 38: strcat(glog , "amulet of warding"); break;
   case 39: strcat(glog , "amulet of resist corrosion"); break;
   case 40: strcat(glog , "amulet of the gourmand"); break;
   case 41: strcat(glog , "amulet of conservation"); break;
   case 42: strcat(glog , "amulet of controlled flight"); break;
   case 43: strcat(glog , "amulet of inaccuracy"); break;
   }
 */


int player_res_fire(void)
{
    int rf = 100;

/* rings of fire resistance/fire */
    if (you.equip[EQ_LEFT_RING] != -1 && (you.inv_type[you.equip[EQ_LEFT_RING]] == RING_PROTECTION_FROM_FIRE || you.inv_type[you.equip[EQ_LEFT_RING]] == RING_FIRE))
        rf++;
    if (you.equip[EQ_RIGHT_RING] != -1 && (you.inv_type[you.equip[EQ_RIGHT_RING]] == RING_PROTECTION_FROM_FIRE || you.inv_type[you.equip[EQ_RIGHT_RING]] == RING_FIRE))
        rf++;
/* rings of ice */
    if (you.equip[EQ_LEFT_RING] != -1 && you.inv_type[you.equip[EQ_LEFT_RING]] == RING_ICE)
        rf--;
    if (you.equip[EQ_RIGHT_RING] != -1 && you.inv_type[you.equip[EQ_RIGHT_RING]] == RING_ICE)
        rf--;
/* Staves */
    if (you.equip[EQ_WEAPON] != -1 && you.inv_class[you.equip[EQ_WEAPON]] == OBJ_STAVES && you.inv_type[you.equip[EQ_WEAPON]] == STAFF_FIRE)
        rf++;
/* armour: (checks body armour only) */
    if (you.equip[EQ_BODY_ARMOUR] != -1 && (you.inv_dam[you.equip[EQ_BODY_ARMOUR]] % 30 == SPARM_FIRE_RESISTANCE || you.inv_dam[you.equip[EQ_BODY_ARMOUR]] % 30 == SPARM_RESISTANCE))
        rf++;
/* DSMails */
    if (you.equip[EQ_BODY_ARMOUR] != -1 && you.inv_type[you.equip[EQ_BODY_ARMOUR]] == ARM_DRAGON_ARMOUR)
        rf += 2;
    if (you.equip[EQ_BODY_ARMOUR] != -1 && you.inv_type[you.equip[EQ_BODY_ARMOUR]] == ARM_GOLD_DRAGON_ARMOUR)
        rf++;
    if (you.equip[EQ_BODY_ARMOUR] != -1 && you.inv_type[you.equip[EQ_BODY_ARMOUR]] == ARM_ICE_DRAGON_ARMOUR)
        rf--;
/* randart wpns */
    rf += scan_randarts(RAP_FIRE);
/* Mummies are highly flammable */
    if (you.species == SP_MUMMY)
        rf--;
/* Mutations */
    rf += you.mutation[MUT_HEAT_RESISTANCE];
/* Red drac */
    if (you.species == SP_RED_DRACONIAN && you.experience_level >= 18)
        rf++;
/* transformations */
    switch (you.attribute[ATTR_TRANSFORMATION])
    {
    case TRAN_ICE_BEAST:
        rf -= 1;
        break;                  /* Ice beast */
    case TRAN_DRAGON:
        rf += 2;
        break;                  /* Dragon */
    }
    return rf;
}

int player_res_cold(void)
{
    int rc = 100;

/* rings of cold resistance/ice */
    if (you.equip[EQ_LEFT_RING] != -1 && (you.inv_type[you.equip[EQ_LEFT_RING]] == RING_PROTECTION_FROM_COLD || you.inv_type[you.equip[EQ_LEFT_RING]] == RING_ICE))
        rc++;
    if (you.equip[EQ_RIGHT_RING] != -1 && (you.inv_type[you.equip[EQ_RIGHT_RING]] == RING_PROTECTION_FROM_COLD || you.inv_type[you.equip[EQ_RIGHT_RING]] == RING_ICE))
        rc++;
/* rings of fire */
    if (you.equip[EQ_LEFT_RING] != -1 && you.inv_type[you.equip[EQ_LEFT_RING]] == RING_FIRE)
        rc--;
    if (you.equip[EQ_RIGHT_RING] != -1 && you.inv_type[you.equip[EQ_RIGHT_RING]] == RING_FIRE)
        rc--;
/* Staves */
    if (you.equip[EQ_WEAPON] != -1 && you.inv_class[you.equip[EQ_WEAPON]] == OBJ_STAVES && you.inv_type[you.equip[EQ_WEAPON]] == STAFF_COLD)
        rc++;
/* armour: (checks body armour only) */
    if (you.equip[EQ_BODY_ARMOUR] != -1 && (you.inv_dam[you.equip[EQ_BODY_ARMOUR]] % 30 == SPARM_COLD_RESISTANCE || you.inv_dam[you.equip[EQ_BODY_ARMOUR]] % 30 == SPARM_RESISTANCE))
        rc++;
/* DSMails */
    if (you.equip[EQ_BODY_ARMOUR] != -1 && you.inv_type[you.equip[EQ_BODY_ARMOUR]] == ARM_ICE_DRAGON_ARMOUR)
        rc += 2;
    if (you.equip[EQ_BODY_ARMOUR] != -1 && you.inv_type[you.equip[EQ_BODY_ARMOUR]] == ARM_GOLD_DRAGON_ARMOUR)
        rc++;
    if (you.equip[EQ_BODY_ARMOUR] != -1 && you.inv_type[you.equip[EQ_BODY_ARMOUR]] == ARM_DRAGON_ARMOUR)
        rc--;
/* Mummies & ghouls are cold resistant */
    if (you.species == SP_MUMMY || you.species == SP_GHOUL)
        rc++;
/* randart wpns */
    rc += scan_randarts(RAP_COLD);
/* White drac */
    if (you.species == SP_WHITE_DRACONIAN && you.experience_level >= 18)
        rc++;
/* Mutations */
    rc += you.mutation[MUT_COLD_RESISTANCE];
/* transformations */
    switch (you.attribute[ATTR_TRANSFORMATION])
    {
    case TRAN_ICE_BEAST:
        rc += 3;
        break;                  /* Ice beast */
    case TRAN_DRAGON:
        rc--;
        break;                  /* Dragon */
    case TRAN_LICH:
        rc++;
        break;                  /* Lich */
    }
    return rc;
}

int player_res_electricity(void)
{
    int re = 0;

    if (you.duration[DUR_INSULATION] > 0)
        re += 1;

    if (you.attribute[ATTR_DIVINE_LIGHTNING_PROTECTION] > 0)
        re += 1;

    if (you.equip[EQ_WEAPON] != -1
                && you.inv_class[you.equip[EQ_WEAPON]] == OBJ_STAVES
                && you.inv_type[you.equip[EQ_WEAPON]] == STAFF_AIR)
        re += 1;

    if (you.equip[EQ_BODY_ARMOUR] != -1 &&
            you.inv_type[you.equip[EQ_BODY_ARMOUR]] == ARM_STORM_DRAGON_ARMOUR)
        re += 1;

    if (you.mutation[MUT_SHOCK_RESISTANCE] > 0)
        re += 1;

    if (you.species == SP_BLACK_DRACONIAN && you.experience_level >= 18)
        re += 1;

    re += scan_randarts( RAP_ELECTRICITY );

    return re;
}

int player_res_poison(void)
{
    int rp = 0;

/* rings of poison resistance */
    if (you.equip[EQ_LEFT_RING] != -1 && you.inv_type[you.equip[EQ_LEFT_RING]] == RING_POISON_RESISTANCE)
        rp++;
    if (you.equip[EQ_RIGHT_RING] != -1 && you.inv_type[you.equip[EQ_RIGHT_RING]] == RING_POISON_RESISTANCE)
        rp++;
/* Staves */
    if (you.equip[EQ_WEAPON] != -1 && you.inv_class[you.equip[EQ_WEAPON]] == OBJ_STAVES && you.inv_type[you.equip[EQ_WEAPON]] == STAFF_POISON)
        rp++;
/* the staff of Olgreb: */
    if (you.equip[EQ_WEAPON] != -1 && you.inv_class[you.equip[EQ_WEAPON]] == OBJ_WEAPONS && you.inv_dam[you.equip[EQ_WEAPON]] == NWPN_STAFF_OF_OLGREB)
        rp++;
/* armour: (checks body armour only) */
    if (you.equip[EQ_BODY_ARMOUR] != -1 && you.inv_dam[you.equip[EQ_BODY_ARMOUR]] % 30 == SPARM_POISON_RESISTANCE)
        rp++;
/* DSMails */
    if (you.equip[EQ_BODY_ARMOUR] != -1 && you.inv_type[you.equip[EQ_BODY_ARMOUR]] == ARM_GOLD_DRAGON_ARMOUR)
        rp++;                   /* Gold */
    if (you.equip[EQ_BODY_ARMOUR] != -1 && you.inv_type[you.equip[EQ_BODY_ARMOUR]] == ARM_SWAMP_DRAGON_ARMOUR)
        rp++;                   /* Swamp */
/* Mummies, Nagas and ghouls are poison resistant */
    if (you.species == SP_MUMMY || you.species == SP_NAGA || you.species == SP_GHOUL)
        rp++;
/* spell of resist poison */
    if (you.duration[DUR_RESIST_POISON] > 0)
        rp++;
/* randarts */
    rp += scan_randarts(RAP_POISON);
/* Mutations */
    rp += you.mutation[MUT_POISON_RESISTANCE];
/* Green drac */
    if (you.species == SP_GREEN_DRACONIAN && you.experience_level >= 7)
        rp++;
/* transformations */
    switch (you.attribute[ATTR_TRANSFORMATION])
    {
    case TRAN_LICH:
        rp++;
        break;                  /* Lich */
    }
    return rp;
}

int player_spec_death(void)
{
    int sd = 0;

/* Staves */
    if (you.equip[EQ_WEAPON] != -1 && you.inv_class[you.equip[EQ_WEAPON]] == OBJ_STAVES && you.inv_type[you.equip[EQ_WEAPON]] == STAFF_DEATH)
        sd++;
/* Mummies */
    if (you.species == SP_MUMMY && you.experience_level > 12)
        sd++;
    if (you.species == SP_MUMMY && you.experience_level > 26)
        sd++;
/* armour of the Archmagi (checks body armour only) */
    if (you.equip[EQ_BODY_ARMOUR] != -1 && you.inv_dam[you.equip[EQ_BODY_ARMOUR]] % 30 == SPARM_ARCHMAGI)
        sd++;
/* transformations */
    switch (you.attribute[ATTR_TRANSFORMATION])
    {
    case TRAN_LICH:
        sd++;
        break;                  /* Lich */
    }
    return sd;
}

int player_spec_holy(void)
{
    return 0;
/* if (you.char_class == JOB_PRIEST || you.char_class == JOB_PALADIN) return 1;
   return 0; */
}

int player_spec_fire(void)
{
    int sf = 0;

/* Staves */
    if (you.equip[EQ_WEAPON] != -1 && you.inv_class[you.equip[EQ_WEAPON]] == OBJ_STAVES && you.inv_type[you.equip[EQ_WEAPON]] == STAFF_FIRE)
        sf++;
/* rings of fire */
    if (you.equip[EQ_LEFT_RING] != -1 && you.inv_type[you.equip[EQ_LEFT_RING]] == RING_FIRE)
        sf++;
    if (you.equip[EQ_RIGHT_RING] != -1 && you.inv_type[you.equip[EQ_RIGHT_RING]] == RING_FIRE)
        sf++;

    return sf;
}

int player_spec_cold(void)
{
    int sc = 0;

/* Staves */
    if (you.equip[EQ_WEAPON] != -1 && you.inv_class[you.equip[EQ_WEAPON]] == OBJ_STAVES && you.inv_type[you.equip[EQ_WEAPON]] == STAFF_COLD)
        sc++;
/* rings of fire */
    if (you.equip[EQ_LEFT_RING] != -1 && you.inv_type[you.equip[EQ_LEFT_RING]] == RING_ICE)
        sc++;
    if (you.equip[EQ_RIGHT_RING] != -1 && you.inv_type[you.equip[EQ_RIGHT_RING]] == RING_ICE)
        sc++;

    return sc;
}

int player_spec_earth(void)
{
    int se = 0;

/* Staves */
    if (you.equip[EQ_WEAPON] != -1 && you.inv_class[you.equip[EQ_WEAPON]] == OBJ_STAVES && you.inv_type[you.equip[EQ_WEAPON]] == STAFF_EARTH)
        se++;

    return se;
}

int player_spec_air(void)
{
    int sa = 0;

/* Staves */
    if (you.equip[EQ_WEAPON] != -1 && you.inv_class[you.equip[EQ_WEAPON]] == OBJ_STAVES && you.inv_type[you.equip[EQ_WEAPON]] == STAFF_AIR)
        sa++;

    return sa;
}

int player_spec_conj(void)
{
    int sc = 0;

/* Staves */
    if (you.equip[EQ_WEAPON] != -1 && you.inv_class[you.equip[EQ_WEAPON]] == OBJ_STAVES && you.inv_type[you.equip[EQ_WEAPON]] == STAFF_CONJURATION)
        sc++;
/* armour of the Archmagi (checks body armour only) */
    if (you.equip[EQ_BODY_ARMOUR] != -1 && you.inv_dam[you.equip[EQ_BODY_ARMOUR]] % 30 == SPARM_ARCHMAGI)
        sc++;
    return sc;
}

int player_spec_ench(void)
{
    int se = 0;

/* Staves */
    if (you.equip[EQ_WEAPON] != -1 && you.inv_class[you.equip[EQ_WEAPON]] == OBJ_STAVES && you.inv_type[you.equip[EQ_WEAPON]] == STAFF_ENCHANTMENT)
        se++;
/* armour of the Archmagi (checks body armour only) */
    if (you.equip[EQ_BODY_ARMOUR] != -1 && you.inv_dam[you.equip[EQ_BODY_ARMOUR]] % 30 == SPARM_ARCHMAGI)
        se++;
    return se;
}

int player_spec_summ(void)
{
    int ss = 0;

/* Staves */
    if (you.equip[EQ_WEAPON] != -1 && you.inv_class[you.equip[EQ_WEAPON]] == OBJ_STAVES && you.inv_type[you.equip[EQ_WEAPON]] == STAFF_SUMMONING_I)
        ss++;
/* armour of the Archmagi (checks body armour only) */
    if (you.equip[EQ_BODY_ARMOUR] != -1 && you.inv_dam[you.equip[EQ_BODY_ARMOUR]] % 30 == SPARM_ARCHMAGI)
        ss++;
    return ss;
}

int player_spec_poison(void)
{
    int sp = 0;

/* Staves */
    if (you.equip[EQ_WEAPON] != -1 && you.inv_class[you.equip[EQ_WEAPON]] == OBJ_STAVES && you.inv_type[you.equip[EQ_WEAPON]] == STAFF_POISON)
        sp++;
/* the staff of Olgreb: */
    if (you.equip[EQ_WEAPON] != -1 && you.inv_class[you.equip[EQ_WEAPON]] == OBJ_WEAPONS && you.inv_dam[you.equip[EQ_WEAPON]] == NWPN_STAFF_OF_OLGREB)
        sp++;

    return sp;
}



int player_energy(void)
{
    int pe = 0;

/* Staves */
    if (you.equip[EQ_WEAPON] != -1 && you.inv_class[you.equip[EQ_WEAPON]] == OBJ_STAVES && you.inv_type[you.equip[EQ_WEAPON]] == STAFF_ENERGY)
        pe++;
    return pe;
}

int player_prot_life(void)
{
    int pl = 0;

    /* rings of life resistance */
    if (you.equip[EQ_LEFT_RING] != -1 && you.inv_type[you.equip[EQ_LEFT_RING]] == RING_LIFE_PROTECTION)
        pl++;

    if (you.equip[EQ_RIGHT_RING] != -1 && you.inv_type[you.equip[EQ_RIGHT_RING]] == RING_LIFE_PROTECTION)
        pl++;

    /* armour: (checks body armour only) */
    if (you.equip[EQ_BODY_ARMOUR] != -1 && you.inv_dam[you.equip[EQ_BODY_ARMOUR]] % 30 == SPARM_POSITIVE_ENERGY)
        pl++;

    /* Mummies are undead, as are ghouls */
    if (you.species == SP_MUMMY || you.species == SP_GHOUL)
        pl += 3;

    /* randart wpns */
    pl += scan_randarts(RAP_NEGATIVE_ENERGY);

    /* demonic power */
    pl += you.mutation[MUT_NEGATIVE_ENERGY_RESISTANCE];

    /* transformations */
    switch (you.attribute[ATTR_TRANSFORMATION])
    {
    case TRAN_LICH:
        pl += 3;
        break;                  /* Lich */
    }

    return pl;
}

int player_fast_run(void)
{
    int fr = 0;

/* armour: (checks boots only) */
    if (you.equip[EQ_BOOTS] != -1 && you.inv_dam[you.equip[EQ_BOOTS]] % 30 == SPARM_RUNNING)
        fr++;
    if (you.duration[DUR_SWIFTNESS] != 0)
        fr++;
/* Mutations */
    if (you.mutation[MUT_FAST] > 0)
        fr++;
/* Centaurs & Spriggans */
    if (you.species == SP_CENTAUR || you.species == SP_SPRIGGAN)
        fr++;
/* transformations */
    switch (you.attribute[ATTR_TRANSFORMATION])
    {
    case TRAN_NONE:
        break;
    case TRAN_SPIDER:
        fr++;
        break;                  /* spider */
    }
    return fr;
}

int player_speed(void)
{
        int ps = 10;
    if (you.haste != 0) ps /= 2;

    if (you.attribute[ATTR_TRANSFORMATION] == TRAN_STATUE)
    {
        ps *= 15;
        ps /= 10;
    }
    return ps;
}

int player_AC(void)
{
    int AC = 0;
    int i;
    int racial_type = 0;

    switch (you.species) {
    case SP_ELF:
    case SP_HIGH_ELF:
    case SP_GREY_ELF:
    case SP_DEEP_ELF:
    case SP_SLUDGE_ELF:
        racial_type = DARM_ELVEN;
        break;

    case SP_HILL_DWARF:
    case SP_MOUNTAIN_DWARF:
        racial_type = DARM_DWARVEN;
        break;

    case SP_HILL_ORC:
        racial_type = DARM_ORCISH;
        break;
    }

    for (i = EQ_CLOAK; i < EQ_LEFT_RING; i++)
    {
        if (you.equip[i] == -1)
            continue;

        if (i == EQ_SHIELD)
            continue;

        if (you.inv_plus[you.equip[i]] > 130)
            AC += you.inv_plus[you.equip[i]] - 150;
        else
            AC += you.inv_plus[you.equip[i]] - 50;

        if (i == EQ_HELMET && you.inv_plus2[you.equip[i]] >= 2)
            continue;

        if (i == EQ_BOOTS && you.inv_plus2[you.equip[i]] != 0)
            AC += 3;            /* barding */


        int racial_bonus = 0;

        if (racial_type != 0
                    && (int) (you.inv_dam[you.equip[i]] / 30) == racial_type)
        {
            if (you.species == SP_MOUNTAIN_DWARF
                                        || you.species == SP_HILL_DWARF)
                racial_bonus = 3;
            else
                racial_bonus = 1;
        }

        AC += property(2, you.inv_type[you.equip[i]], 0) *
                            (15 + you.skills[SK_ARMOUR] + racial_bonus) / 15;

        /* Nagas/Centaurs/the deformed don't fit into body armour very well */
        if ((you.species == SP_NAGA || you.species == SP_CENTAUR
                    || you.mutation[MUT_DEFORMED] > 0) && i == EQ_BODY_ARMOUR)
        {
            AC -= property(2, you.inv_type[you.equip[i]], 0) / 2;
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

    if (you.species == SP_NAGA)
        AC += you.experience_level / 3;         /* naga */

    /* grey dracs have especially tough armour */
    if (you.species == SP_GREY_DRACONIAN && you.experience_level > 7)
    {
        AC += (you.experience_level - 4) / 2 - 1;
    }
    else if (you.species >= SP_RED_DRACONIAN
                                && you.species <= SP_UNK2_DRACONIAN
                                            && you.experience_level > 7)
    {
        AC += you.experience_level / 4 - 1;
    }

    if (you.species >= SP_RED_DRACONIAN && you.species <= SP_UNK2_DRACONIAN)
        AC += 2;


    if (you.species == SP_OGRE)
        AC++;                   /* ogre */

    if (you.species == SP_TROLL)
        AC += 3;                /* troll */

    if (you.species == SP_CENTAUR)
        AC += 3;                /* centaur */

    if (you.duration[DUR_ICY_ARMOUR] > 0)
        AC += 4 + you.skills[SK_ICE_MAGIC] / 3;         // ice armour

    if (you.duration[DUR_STONEMAIL] > 0)
        AC += 5 + you.skills[SK_EARTH_MAGIC] / 2;       // stonemail

    /* mutations */
    AC += you.mutation[MUT_TOUGH_SKIN];

    if (you.mutation[MUT_GREEN_SCALES] > 0)
        AC += you.mutation[MUT_GREEN_SCALES] * 2 - 1;

    AC += you.mutation[MUT_BLACK_SCALES] * 3;

    AC += you.mutation[MUT_GREY_SCALES];

    if (you.mutation[MUT_BONEY_PLATES] > 0)
        AC += you.mutation[MUT_BONEY_PLATES] + 1;

    AC += you.mutation [MUT_RED_SCALES] + (you.mutation [MUT_RED_SCALES] == 3);

    if (you.mutation[MUT_NACREOUS_SCALES] > 0)
        AC += you.mutation[MUT_NACREOUS_SCALES] * 2 - 1;

    AC += you.mutation[MUT_GREY2_SCALES] * 2;

    AC += you.mutation[MUT_METALLIC_SCALES] * 3 + (you.mutation [MUT_METALLIC_SCALES] > 1);

    if (you.mutation[MUT_BLACK2_SCALES] > 0)
        AC += you.mutation[MUT_BLACK2_SCALES] * 2 - 1;

    if (you.mutation[MUT_WHITE_SCALES] > 0)
        AC += you.mutation[MUT_WHITE_SCALES] * 2 - 1;

    AC += you.mutation[MUT_YELLOW_SCALES] * 2;

    if (you.mutation[MUT_BROWN_SCALES] > 0)
        AC += you.mutation[MUT_BROWN_SCALES] * 2 - (you.mutation [MUT_METALLIC_SCALES] == 3);

    AC += you.mutation[MUT_BLUE_SCALES];

    AC += you.mutation[MUT_PURPLE_SCALES] * 2;

    AC += you.mutation[MUT_SPECKLED_SCALES];

    AC += you.mutation [MUT_ORANGE_SCALES] + (you.mutation [MUT_ORANGE_SCALES] >= 2);

    if (you.mutation[MUT_INDIGO_SCALES] > 0)
        AC += you.mutation[MUT_INDIGO_SCALES] + 1 + (you.mutation [MUT_INDIGO_SCALES] == 3);

    AC += you.mutation [MUT_RED2_SCALES] * 2 + (you.mutation [MUT_RED2_SCALES] >= 2);

    AC += you.mutation[MUT_IRIDESCENT_SCALES];

    AC += you.mutation[MUT_PATTERNED_SCALES];


    /* transformations */
    switch (you.attribute[ATTR_TRANSFORMATION])
    {
    case TRAN_NONE:
        break;
    case TRAN_SPIDER:
        AC += 2;
        break;                  /* spider */
    case TRAN_STATUE:
        AC += 20;
        break;                  /* statue */
    case TRAN_ICE_BEAST:
        AC += 2;
        break;                  /* Ice beast */
    case TRAN_DRAGON:
        AC += 7;
        break;                  /* Dragon */
    case TRAN_LICH:
        AC += 3;
        break;                  /* Lich */
    }

    return AC;
}

bool player_light_armour(void)
{
    if (you.equip[EQ_BODY_ARMOUR] == -1
                || you.inv_dam[you.equip[EQ_BODY_ARMOUR]] / 30 == DARM_ELVEN)
        return true;

    switch (you.inv_type[you.equip[EQ_BODY_ARMOUR]])
    {
    case ARM_ROBE:
    case ARM_LEATHER_ARMOUR:
    case ARM_STEAM_DRAGON_HIDE:
    case ARM_STEAM_DRAGON_ARMOUR:
    case ARM_MOTTLED_DRAGON_HIDE:
    case ARM_MOTTLED_DRAGON_ARMOUR:
        return true;

    default:
        return false;
    }
}

int player_evasion(void)
{
    int ev = 10;

    if (you.species == SP_CENTAUR)
        ev = 7;

    if (you.equip[EQ_BODY_ARMOUR] != -1)
    {
        int ev_change = 0;

        ev_change = property(2, you.inv_type[you.equip[EQ_BODY_ARMOUR]], 1);
        ev_change += you.skills[SK_ARMOUR] / 2;  // lowered from / 3 -- bwross

        if (ev_change
                > property(2, you.inv_type[you.equip[EQ_BODY_ARMOUR]], 1) / 3)
        {
            ev_change
                = property(2, you.inv_type[you.equip[EQ_BODY_ARMOUR]], 1) / 3;
        }

        ev += ev_change;            /* remember that it's negative */
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

    if (you.duration[DUR_STONEMAIL] > 0)
        ev -= 2;                // stonemail

    int emod = 0;

    if (!player_light_armour())
    {
        // meaning that the armour evasion modifier is often effectively
        // applied twice, but not if you're wearing elven armour
        emod += (property(2, you.inv_type[you.equip[EQ_BODY_ARMOUR]], 1)
                                                                * 14) / 10;
    }

    emod += you.skills[SK_DODGING] / 2;         // is this too generous?

    if (emod > 0)
        ev += emod;

    /* repulsion field */
    if (you.mutation[MUT_REPULSION_FIELD] > 0)
        ev += you.mutation[MUT_REPULSION_FIELD] * 2 - 1;

    /* transformations */
    switch (you.attribute[ATTR_TRANSFORMATION])
    {
    case TRAN_NONE:
        break;
    case TRAN_SPIDER:
        ev += 3;
        break;                  /* spider */
    case TRAN_STATUE:
        ev -= 5;
        break;                  /* statue */
    case TRAN_DRAGON:
        ev -= 3;
        break;                  /* Dragon */
    }

    ev += scan_randarts(RAP_EVASION);

    return ev;
}

int player_mag_abil(void)
{
    int ma = 0;

/* rings of wizardry */
    if (you.equip[EQ_LEFT_RING] != -1 && you.inv_type[you.equip[EQ_LEFT_RING]] == RING_WIZARDRY)
        ma += 3;
    if (you.equip[EQ_RIGHT_RING] != -1 && you.inv_type[you.equip[EQ_RIGHT_RING]] == RING_WIZARDRY)
        ma += 3;
/* Staves */
    if (you.equip[EQ_WEAPON] != -1 && you.inv_class[you.equip[EQ_WEAPON]] == OBJ_STAVES && you.inv_type[you.equip[EQ_WEAPON]] == STAFF_WIZARDRY)
        ma += 4;
/* armour of the Archmagi (checks body armour only) */
    if (you.equip[EQ_BODY_ARMOUR] != -1 && you.inv_dam[you.equip[EQ_BODY_ARMOUR]] % 30 == SPARM_ARCHMAGI)
        ma += 2;
    return ma;
}

int player_shield_class(void)
{

    int base_shield = 0;

    if (you.equip[EQ_SHIELD] == -1)
        return 0;

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

    base_shield *= 10 + you.skills[SK_SHIELDS] / 2;
    base_shield /= 10;

    if (you.inv_plus[you.equip[EQ_SHIELD]] > 130)
        base_shield += you.inv_plus[you.equip[EQ_SHIELD]] - 150;
    else
        base_shield += you.inv_plus[you.equip[EQ_SHIELD]] - 50;

    return base_shield;

}

int player_see_invis(void)
{
    int si = 0;

    /* rings of see invis */
    if (you.equip[EQ_LEFT_RING] != -1
                && you.inv_type[you.equip[EQ_LEFT_RING]] == RING_SEE_INVISIBLE)
        si++;

    if (you.equip[EQ_RIGHT_RING] != -1
                && you.inv_type[you.equip[EQ_RIGHT_RING]] == RING_SEE_INVISIBLE)
        si++;

    /* armour: (checks head armour only) */
    if (you.equip[EQ_HELMET] != -1
            && you.inv_dam[you.equip[EQ_HELMET]] % 30 == SPARM_SEE_INVISIBLE)
        si++;

    /* Nagas & Spriggans have good eyesight */
    if (you.species == SP_NAGA || you.species == SP_SPRIGGAN)
        si++;

    if (you.mutation[MUT_ACUTE_VISION] > 0)
        si += you.mutation[MUT_ACUTE_VISION];

    /* randart wpns */
    int artefacts = scan_randarts(RAP_EYESIGHT);

    if (artefacts > 0)
        si += artefacts;

    return si;
}

int player_sust_abil(void)
{
    int sa = 0;

/* rings of sust abil */
    if (you.equip[EQ_LEFT_RING] != -1 && you.inv_type[you.equip[EQ_LEFT_RING]] == SPWPN_FROST)
        sa++;
    if (you.equip[EQ_RIGHT_RING] != -1 && you.inv_type[you.equip[EQ_RIGHT_RING]] == RING_SUSTAIN_ABILITIES)
        sa++;
/* Mummies have no way to restore stats, so only fair: */
    if (you.species == SP_MUMMY)
        sa++;
    return sa;
}

int carrying_capacity(void)
{
    // Originally: 1000 + you.strength * 200 + (you.levitation != 0) * 1000
    return( 3500 + you.strength * 100 + (you.levitation != 0) * 1000 );
}

int burden_change()
{
    char old_burden = you.burden_state;

    you.burden = 0;

    char bu = 0;

    int max_carried = carrying_capacity();

    if (you.duration[DUR_STONEMAIL] != 0)
        you.burden += 800;

    for (bu = 0; bu < 52; bu++)
    {
        if (you.inv_quantity[bu] < 1)
            continue;

        if (you.inv_class[bu] == 14)
        {
            if (you.inv_type[bu] == 0)
                you.burden += mons_weight(you.inv_plus[bu]);
            if (you.inv_type[bu] == 1)
                you.burden += mons_weight(you.inv_plus[bu]) / 2;
        }
        else
        {
            you.burden += mass(you.inv_class[bu], you.inv_type[bu])
                                                    * you.inv_quantity[bu];
        }
    }

    you.burden_state = 0;
    you.redraw_burden = 1;

    // changed the burdened levels to do with the change to max_carried.

    // if (you.burden < max_carried - 1000)
    if (you.burden < (max_carried * 5) / 6)
    {
        you.burden_state = 0;   /* unencumbered */
        return you.burden;
    }

    // if (you.burden < max_carried - 500)
    if (you.burden < (max_carried * 11) / 12)
    {
        you.burden_state = 2;   /* encumbered */
        if (old_burden != you.burden_state)
            mpr("You are being weighed down by all of your possessions.");

        return you.burden;
    }

    you.burden_state = 5;
    if (old_burden != you.burden_state)
        mpr("You are being crushed by all of your possessions.");

    return you.burden;
}


char you_resist_magic(int power)
{
    int ench_power = power;

    if (ench_power > 40)
        ench_power = ((ench_power - 40) / 2) + 40;
    if (ench_power > 70)
        ench_power = ((ench_power - 70) / 2) + 70;
    if (ench_power > 90)
        ench_power = ((ench_power - 90) / 2) + 90;

    if (ench_power > 120)
        ench_power = 120;
/*
   itoa(power, st_prn, 10);
   strcpy(info, st_prn);
   mpr(info);
   itoa(you.res_magic, st_prn, 10);
   strcpy(info, st_prn);
   mpr(info); */


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
        return 1;               // ie saved successfully

    return 0;



/* if (random2(power) / 3 + random2(power) / 3 + random2(power) / 3 >= player_res_magic()) return 0;
   return 1; */
}



void forget_map(char chance_forgotten)
{
    char xcount = 0;
    char ycount = 0;

    for (xcount = 0; xcount < 80; xcount++)
    {
        for (ycount = 0; ycount < 70; ycount++)
        {
            if (random2(100) < chance_forgotten)
                env.map[xcount][ycount] = 0;
        }
    }


}

void how_hungered(int hunge)
{
    if (hunge <= 1)
        return;

    if (hunge <= 100)
    {
        strcpy(info, "You feel slightly");
        goto full_or_hungry;
    }
    if (hunge <= 350)
    {
        strcpy(info, "You feel somewhat");
        goto full_or_hungry;
    }
    if (hunge <= 800)
    {
        strcpy(info, "You feel a quite a bit");
        goto full_or_hungry;
    }
    strcpy(info, "You feel a lot");

full_or_hungry:
    if (you.hunger_state >= 4)
    {
        strcat(info, " less full.");
        mpr(info);
        return;
    }
    strcat(info, " more hungry.");
    mpr(info);
    return;

}


void gain_exp(unsigned int exp_gained)
{

    if (you.equip[EQ_BODY_ARMOUR] != -1)
    {
        if (you.inv_dam[you.equip[EQ_BODY_ARMOUR]] % 30 == SPARM_ARCHMAGI)
            return;             // robe of archmagi

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
}




void level_change(void)         // Look at this !!!!
 {

#ifdef CLASSES
/*
   case 0: // fighter
   case 1: // wizard
   case 2: // priest
   case 3: // thief
   case 4: // Gladiator
   case 5: // necromancer
   case 6: // paladin
   case 7: // Assassin
   case 8: // Barbarian
   case 9: // ranger
   case 10: // Conjurer
   case 11: // Enchanter
   case 12: // Fire Wizard
   case 13: // Ice Wizard
   case 14: // Summoner
 */

// total about 250
    int lev_skill[19][8][3] =
    {
        {                       // Fighter
            {0, 0, 80},
            {1, 7, 70},
            {8, 12, 50},
            {13, 17, 50},
            {0, 0, 0},
            {0, 0, 0},
            {0, 0, 0},
            {0, 0, 0}
        },
        {                       // Wizard
            {1, 7, 20},
            {14, 15, 50},
            {8, 12, 20},
            {26, 49, 90},
            {25, 25, 70},
            {0, 0, 0},
            {0, 0, 0},
            {0, 0, 0}
        },
        {                       // Priest - only gets 230 points
            {0, 0, 50},
            {1, 7, 40},
            {13, 13, 30},
            {17, 17, 30},
            {25, 25, 50},
            {32, 32, 30},       // Div
             {0, 0, 0},
            {0, 0, 0}
        },
        {                       // Thief
            {0, 0, 40},
            {1, 7, 50},
            {14, 16, 80},
            {18, 18, 30},
            {25, 25, 20},
            {8, 12, 30},
            {0, 0, 0},
            {0, 0, 0}
        },
        {                       // Glad
            {0, 0, 80},
            {1, 7, 90},
            {13, 17, 80},
            {0, 0, 0},
            {0, 0, 0},
            {0, 0, 0},
            {0, 0, 0},
            {0, 0, 0}
        },
        {                       // Necromancer
            {1, 7, 20},
            {14, 15, 40},
            {8, 12, 20},
            {26, 49, 40},
            {25, 25, 40},
            {29, 29, 90},
            {0, 0, 0},
            {0, 0, 0}
        },
        {                       // Paladin - only gets 240 points
            {0, 0, 60},
            {1, 7, 60},
            {13, 13, 45},
            {17, 17, 45},
            {25, 25, 30},
            {0, 0, 0},
            {0, 0, 0},
            {0, 0, 0}
        },
        {                       // Assassin
            {0, 0, 60},
            {1, 7, 80},
            {14, 16, 60},
            {0, 0, 0},
            {25, 25, 20},
            {8, 12, 30},
            {0, 0, 0},
            {0, 0, 0}
        },
        {                       // Berserker
            {0, 0, 80},
            {1, 7, 40},
            {8, 12, 60},
            {14, 15, 50},
            {17, 17, 20},
            {0, 0, 0},
            {0, 0, 0},
            {0, 0, 0}
        },
        {                       // ranger
            {0, 0, 80},
            {1, 7, 40},
            {8, 12, 100},
            {14, 16, 50},
            {25, 25, 30},
            {0, 0, 0},
            {0, 0, 0},
            {0, 0, 0}
        },
        {                       // Conjurer
            {1, 7, 20},
            {14, 15, 40},
            {8, 12, 20},
            {26, 49, 40},
            {25, 25, 40},
            {26, 26, 90},
            {0, 0, 0},
            {0, 0, 0}
        },
        {                       // Enchanter
            {1, 7, 20},
            {14, 15, 40},
            {8, 12, 20},
            {26, 49, 30},
            {25, 25, 50},
            {27, 27, 90},
            {0, 0, 0},
            {0, 0, 0}
        },
        {                       // Fire Wizard
            {1, 7, 20},
            {14, 15, 40},
            {8, 12, 20},
            {26, 49, 50},
            {25, 25, 40},
            {33, 33, 80},
            {0, 0, 0},
            {0, 0, 0}
        },
        {                       // Ice wizard
            {1, 7, 20},
            {14, 15, 40},
            {8, 12, 20},
            {26, 49, 50},
            {25, 25, 40},
            {34, 34, 80},
            {0, 0, 0},
            {0, 0, 0}
        },
        {                       // Summoner
            {1, 7, 20},
            {14, 15, 40},
            {8, 12, 20},
            {26, 49, 40},
            {25, 25, 40},
            {28, 28, 90},
            {0, 0, 0},
            {0, 0, 0}
        },
        {                       // Air
            {1, 7, 20},
            {14, 15, 40},
            {8, 12, 20},
            {26, 49, 50},
            {25, 25, 40},
            {35, 35, 80},
            {0, 0, 0},
            {0, 0, 0}
        },
        {                       // Earth
            {1, 7, 20},
            {14, 15, 40},
            {8, 12, 20},
            {26, 49, 50},
            {25, 25, 40},
            {35, 35, 80},
            {0, 0, 0},
            {0, 0, 0}
        },
        {                       // Crusader
            {0, 0, 60},
            {1, 7, 60},
            {13, 17, 50},
            {0, 0, 0},
            {25, 25, 40},
            {8, 12, 20},
            {26, 49, 20},
            {0, 0, 0}
        },
        {                       // Death knight
            {0, 0, 60},
            {1, 7, 60},
            {13, 17, 50},
            {0, 0, 0},
            {25, 25, 30},
            {29, 29, 50},
            {0, 0, 0},
            {0, 0, 0}
        }
    };

#endif

    char temp_quant[10];

//while (you.experience > (20 * ((you.experience_level * 0.6) * (you.experience_level * 0.7)) + 10) && you.experience_level < 27)
    while (you.experience > exp_needed(you.experience_level + 2, you.species) && you.experience_level < 27)     //(20 * you.experience_level * you.experience_level)) // && you.experience_level < 27)
        //while(you.experience > exp_needed(you.experience_level))

    {
        you.experience_level++;
        strcpy(info, "You are now a level ");
        itoa(you.experience_level, temp_quant, 10);
        strcat(info, temp_quant);
        strcat(info, " ");
        strcat(info, you.class_name);
        strcat(info, "!");
        mpr(info);
        more();
/* scrloc = 0;
   switch(you.char_class)
   {
   case 0: // fighter
   break;
   case 1: // wizard
   case 5: // necromancer
   case 10: // Conjurer
   case 11: // Enchanter
   case 12: // Fire Wizard
   case 13: // Ice Wizard
   case 14: // Summoner
   break;

   case 3: // thief
   break;

   case 4: // Gladiator
   break;

   case 2: // priest
   if (you.experience_level > you.max_level && you.experience_level % 3 == 0)
   {
   if (you.spell_no >= 21)
   {
   break;
   }
   prtry2 = you.experience_level / 3;
   if (you.experience_level > 12) prtry2 = (you.experience_level - 12) / 2 + 4;
   priest_spells(func_pass, 1);
   if (prtry2 >= 8 || func_pass [prtry2] == -1) goto get_out;
   prtry = func_pass [prtry2];
   for (prtry3 = 0; prtry3 < 25; prtry3 ++)
   {
   if (you.spells [prtry3] == 210)
   {
   you.spells [prtry3] = prtry;
   you.spell_no ++;
   you.spell_levels -= spell_value(prtry);
   break;
   }
   }
   strcpy(info, "You have been granted a spell!");
   mpr(info);
   }
   break;


   case 6: // paladin
   if (you.experience_level > you.max_level && you.experience_level % 4 == 0)
   {
   if (you.spell_no >= 21)
   {
   break; //goto infuse2;
   }
   prtry2 = you.experience_level / 4;
   if (you.experience_level > 16) prtry2 = (you.experience_level - 16) / 2 + 4;
   switch(prtry2)
   {
   case 1: prtry = 46; break; // repel undead
   case 2: prtry = 38; break; // lesser healing
   case 3: prtry = 65; break; // heal other
   case 4: prtry = 45; break; // smiting
   case 5: prtry = 39; break; // greater healing
   case 6: prtry = 41; break; // purification
   case 7: prtry = 47; break; // holy word
   default: goto get_out;
   }
   for (prtry3 = 0; prtry3 < 25; prtry3 ++)
   {
   if (you.spells [prtry3] == 210)
   {
   you.spells [prtry3] = prtry;
   you.spell_no ++;
   you.spell_levels -= spell_value(prtry);
   break;
   }
   }
   strcpy(info, "You have been granted a spell!");
   mpr(info);
   }
   get_out : break;


   case 7: // Assassin
   break;

   case 8: // Barbarian
   break;

   case 9: // Ranger
   break;

   // etcetera

   } */


#ifdef CLASSES
        if (you.experience_level > you.max_level)
        {
            int sklev1 = 0;
            int sklev2 = 0;
            int ski = 0;
            int unapplied_points = 0;

            sklev1 = skill_exp_needed(you.experience_level + 2) - skill_exp_needed(you.experience_level + 1);


            for (ski = 0; ski < 8; ski++)
            {
                if (lev_skill[you.char_class][ski][2] == 0)
                    continue;
                unapplied_points += add_skill(you, lev_skill[you.char_class][ski][0], lev_skill[you.char_class][ski][1], sklev1 * lev_skill[you.char_class][ski][2] / 100);
            }

            add_skill(you, 0, 49, unapplied_points);

            for (ski = SK_FIGHTING; ski < 50; ski++)
            {
                exercise2(ski, 0, 1);
            }

        }
#endif

// you.res_magic += 3;

        int brek;

        brek = random2(4) + 3;
        if (you.experience_level > 12)
            brek = random2(3) + 2;
        if (you.experience_level > 21)
            brek = random2(2) + 2;
        you.hp += brek;
        you.base_hp2 += brek;

        you.magic_points += 1;
        you.base_magic_points2++;

        // if (you.spell_levels < 99)
        //    you.spell_levels++;

        if (you.experience_level > you.max_level && you.experience_level % 3 == 0)
        {
            ability_increase();
        }

        if (you.experience_level > you.max_level)
        {
            switch (you.species)
            {
            case SP_HUMAN:      // human

                if (you.experience_level % 5 == 0)
                    increase_stats(random() % 3);
                break;
            case SP_ELF:        // elf

                if (you.experience_level % 3 != 0)
                {
                    you.hp_max--;
                    you.base_hp2--;
                }
                if (you.experience_level % 3 == 0)
                {
                    you.base_magic_points2++;
                }
                if (you.experience_level % 4 == 0)
                    increase_stats(1 + random() % 2);
                break;
            case SP_HIGH_ELF:   // high elf

                if (you.experience_level % 3 != 0)
                {
                    you.hp_max--;
                    you.base_hp2--;
                }
                if (you.experience_level % 2 == 0)
                {
                    you.base_magic_points2++;
                }
//you.res_magic ++;
                if (you.experience_level % 3 == 0)
                    increase_stats(1 + random() % 2);
                break;
            case SP_GREY_ELF:   // grey elf

                if (you.experience_level <= 13)
                {
                    you.hp_max--;
                    you.base_hp2--;
                }
                if (you.experience_level % 3 != 0)
                {
                    you.base_magic_points2++;
                }
//you.res_magic ++;
                if (you.experience_level % 4 == 0)
                    increase_stats(1 + random() % 2);
                break;
            case SP_DEEP_ELF:   // deep elf

                if (you.experience_level <= 16)
                {
                    you.hp_max--;
                    you.base_hp2--;
                }
                if (you.experience_level % 3 == 0)
                {
                    you.hp_max--;
                    you.base_hp2--;
                }
                you.base_magic_points2++;
                if (you.experience_level % 4 == 0)
                    increase_stats(2);
                break;
            case SP_SLUDGE_ELF: // sludge elf

                if (you.experience_level % 3 != 0)
                {
                    you.hp_max--;
                    you.base_hp2--;
                }
                if (you.experience_level % 3 == 0)
                {
                    you.base_magic_points2++;
                }
                if (you.experience_level % 4 == 0)
                    increase_stats(1 + random() % 2);
                break;
            case SP_HILL_DWARF: // hill dwarf

                if (you.experience_level <= 13)
                {
                    you.hp_max++;
                    you.base_hp2++;
                }
                if (you.experience_level % 3 != 0)
                {
                    you.hp_max++;
                    you.base_hp2++;
                }
                if (you.experience_level % 2 == 0)
                {
                    you.base_magic_points2--;
                }
                if (you.experience_level % 4 == 0)
                    increase_stats(0);
                break;
            case SP_MOUNTAIN_DWARF:     // mountain dwarf

                if (you.experience_level <= 13)
                {
                    you.hp_max++;
                    you.base_hp2++;
                }
                if (you.experience_level % 2 == 0)
                {
                    you.hp_max++;
                    you.base_hp2++;
                }
                if (you.experience_level % 3 == 0)
                {
                    you.base_magic_points2--;
                }
                if (you.experience_level % 4 == 0)
                    increase_stats(0);
                break;

            case SP_KOBOLD:     // kobold

            case SP_HALFLING:   // halfling

                if (you.experience_level % 5 == 0)
                {
                    increase_stats(you.species == SP_HALFLING ? 1 : random2(2));
                }

                if (you.experience_level <= 16)
                {
                    you.hp_max--;
                    you.base_hp2--;
                }

                if (you.experience_level % 2 == 0)
                {
                    you.base_hp2--;
                }
                break;

            case SP_HILL_ORC:   // hill orc

                if (you.experience_level <= 16)
                {
                    you.hp_max++;
                    you.base_hp2++;
                }
                if (you.experience_level % 2 == 0)
                {
                    you.base_hp2++;
                    you.hp_max++;
                }
                if (you.experience_level % 3 == 0)
                {
                    you.base_magic_points2--;
                }
                if (you.experience_level % 5 == 0)
                    increase_stats(0);
                break;
// 11 (kobold) is with halfling
            case SP_MUMMY:      // Mummy

                if (you.experience_level == 13 || you.experience_level == 23)
                {
/* you.spec_death ++; */
                    strcpy(info, "You feel more in touch with the powers of death.");
                    mpr(info);
                }
                break;
            case SP_NAGA:       // Naga

                if (you.experience_level <= 13)
                {
                    you.hp_max++;
                    you.base_hp2++;
                }
                you.base_hp2++;
                you.hp_max++;
//you.res_magic += 2;
                if (you.experience_level % 4 == 0)
                    increase_stats(random2(3));
                if (you.experience_level % 3 == 0)
                {
                    strcpy(info, "Your skin feels tougher.");
                    mpr(info);
/* player_AC(you) ++; */
                    you.redraw_armor_class = 1;
                }
                break;
            case SP_GNOME:      // Gnome

                if (you.experience_level <= 12)
                {
                    you.hp_max--;
                    you.base_hp2--;
                }
                if (you.experience_level % 3 == 0)
                {
                    you.hp_max--;
                    you.base_hp2--;
                }
//you.res_magic += 3;
                if (you.experience_level % 4 == 0)
                    increase_stats(1 + random2(2));
                break;
            case SP_OGRE:       // ogre

            case SP_TROLL:      // troll

                you.hp_max++;
                you.base_hp2++;
                if (you.experience_level <= 13)
                {
                    you.hp_max++;
                    you.base_hp2++;
                }
                if (you.experience_level % 2 == 0)
                {
                    you.base_hp2++;
                    you.hp_max++;
                }
                if (you.experience_level % 3 != 0)
                {
                    you.base_magic_points2--;
                }
                if (you.experience_level % 3 == 0)
                    increase_stats(0);
                break;
            case SP_OGRE_MAGE:  // ogre-mage

                you.hp_max++;
                you.base_hp2++;
                if (you.experience_level <= 13)
                {
                    you.hp_max++;
                    you.base_hp2++;
                }
                if (you.experience_level % 5 == 0)
                    increase_stats(random2(2) * 2);
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
                        mpr("Your scales start taking on a fiery red colour.");
                        break;
                    case SP_WHITE_DRACONIAN:
                        mpr("Your scales start taking on an icy white colour.");
                        break;
                    case SP_GREEN_DRACONIAN:
                        mpr("Your scales start taking on a green colour.");
                        mpr("You feel resistant to poison.");
                        break;
                    case SP_GOLDEN_DRACONIAN:
                        mpr("Your scales start taking on a golden yellow colour.");
                        break;

                    case SP_BLACK_DRACONIAN:
                        mpr("Your scales start turning black.");
                        break;
                    case SP_PURPLE_DRACONIAN:
                        mpr("Your scales start taking on a rich purple colour.");
                        break;
                    case SP_MOTTLED_DRACONIAN:
                        mpr("Your scales start taking on a weird mottled pattern.");
                        break;
                    case SP_PALE_DRACONIAN:
                        mpr("Your scales start fading to a pale grey colour.");
                        break;
                    case SP_UNK0_DRACONIAN:
                        mpr("");
                        break;
                    case SP_UNK1_DRACONIAN:
                        mpr("");
                        break;
                    case SP_UNK2_DRACONIAN:
                        mpr("");
                        break;
                    }
                    more();

                    char title[40];

                    strcpy(title, skill_title(best_skill(0, 50, 99),
                                    you.skills[best_skill(0, 50, 99)],
                                    you.char_class, you.experience_level));

                    draw_border(BROWN, you.your_name, title, you.species);

                    you.redraw_hit_points = 1;
                    you.redraw_magic_points = 1;
                    you.redraw_strength = 1;
                    you.redraw_intelligence = 1;
                    you.redraw_dexterity = 1;
                    you.redraw_armor_class = 1;
                    you.redraw_evasion = 1;
                    you.redraw_gold = 1;
                    you.redraw_experience = 1;
                    you.redraw_hunger = 1;
                    you.redraw_burden = 1;

                    print_stats();
                    new_level();
                }

                if (you.experience_level == 18)
                {
                    switch (you.species)
                    {
                    case SP_RED_DRACONIAN:
                        mpr("You feel resistant to fire.");
                        break;
                    case SP_WHITE_DRACONIAN:
                        mpr("You feel resistant to cold.");
                        break;
                    case SP_BLACK_DRACONIAN:
                        mpr("You feel resistant to electrical energy.");
                        break;
                    }
                }

                if (you.experience_level % 3 == 0)
                {
                    you.hp_max += 1;
                    you.base_hp2 += 1;
                }

                if (you.experience_level > 7 && you.experience_level % 4 == 0)
                {
                    mpr("Your scales feel tougher.");
                    you.redraw_armor_class = 1;
                    increase_stats(random2(3));
                }
                break;

            case SP_GREY_DRACONIAN:     /* grey drac */
                if (you.experience_level == 7)
                {
                    mpr("Your scales start turning grey.");
                    more();

                    char title[40];

                    strcpy(title, skill_title(best_skill(0, 50, 99),
                                    you.skills[best_skill(0, 50, 99)],
                                    you.char_class, you.experience_level));

                    draw_border(BROWN, you.your_name, title, you.species);

                    you.redraw_hit_points = 1;
                    you.redraw_magic_points = 1;
                    you.redraw_strength = 1;
                    you.redraw_intelligence = 1;
                    you.redraw_dexterity = 1;
                    you.redraw_armor_class = 1;
                    you.redraw_evasion = 1;
                    you.redraw_gold = 1;
                    you.redraw_experience = 1;
                    you.redraw_hunger = 1;
                    you.redraw_burden = 1;

                    print_stats();
                    new_level();
                }
                if (you.experience_level % 3 == 0)
                {
                    you.hp_max += 1;
                    you.base_hp2 += 1;
                    if (you.experience_level > 7)
                    {
                        you.hp_max += 1;
                        you.base_hp2 += 1;
                    }
                }
                if (you.experience_level > 7 && you.experience_level % 2 == 0)
                {
                    mpr("Your scales feel tougher.");
                    you.redraw_armor_class = 1;
                }

                if ((you.experience_level > 7 && you.experience_level % 3 == 0)
                                    || you.experience_level == 4
                                    || you.experience_level == 7)
                    increase_stats(random2(3));
                break;

            case SP_CENTAUR:    // centaur

                if (you.experience_level % 4 == 0)
                    increase_stats(random2(2));  /* str or dex */
                if (you.experience_level <= 16)
                {
                    you.hp_max++;
                    you.base_hp2++;
                }
                if (you.experience_level % 2 == 0)
                {
                    you.base_hp2++;
                    you.hp_max++;
                }
                if (you.experience_level % 3 == 0)
                {
                    you.base_magic_points2--;
                }
                break;

            case SP_DEMIGOD:    // demigod

                if (you.experience_level % 3 == 0)
                    increase_stats(random2(3));
                if (you.experience_level <= 16)
                {
                    you.hp_max++;
                    you.base_hp2++;
                }
                if (you.experience_level % 2 == 0)
                {
                    you.base_hp2++;
                    you.hp_max++;
                }
                if (you.experience_level % 3 != 0)
                    you.base_magic_points2++;
                break;

            case SP_SPRIGGAN:   // spriggan

                if (you.experience_level <= 16)
                {
                    you.hp_max--;
                    you.base_hp2--;
                }
                if (you.experience_level % 3 != 0)
                {
                    you.hp_max--;
                    you.base_hp2--;
                }
                you.base_magic_points2++;
                if (you.experience_level % 5 == 0)
                    increase_stats(1 + random2(2));
                break;
            case SP_MINOTAUR:   // Minotaur

                if (you.experience_level <= 16)
                {
                    you.hp_max++;
                    you.base_hp2++;
                }
                if (you.experience_level % 2 == 0)
                {
                    you.base_hp2++;
                    you.hp_max++;
                }
                if (you.experience_level % 2 == 0)
                {
                    you.base_magic_points2--;
                }
                if (you.experience_level % 4 == 0)
                    increase_stats(random2(2));  /* str or dex */
                break;
            case SP_DEMONSPAWN: // demonspawn

                if (you.attribute[ATTR_NUM_DEMONIC_POWERS] == 0 && (you.experience_level == 4 || (you.experience_level < 4 && random2(3) == 0)))
                    demonspawn();
                if (you.attribute[ATTR_NUM_DEMONIC_POWERS] == 1 && you.experience_level > 4 && (you.experience_level == 9 || (you.experience_level < 9 && random2(3) == 0)))
                    demonspawn();
                if (you.attribute[ATTR_NUM_DEMONIC_POWERS] == 2 && you.experience_level > 9 && (you.experience_level == 14 || (you.experience_level < 14 && random2(3) == 0)))
                    demonspawn();
                if (you.attribute[ATTR_NUM_DEMONIC_POWERS] == 3 && you.experience_level > 14 && (you.experience_level == 19 || (you.experience_level < 19 && random2(3) == 0)))
                    demonspawn();
                if (you.attribute[ATTR_NUM_DEMONIC_POWERS] == 4 && you.experience_level > 19 && (you.experience_level == 24 || (you.experience_level < 24 && random2(3) == 0)))
                    demonspawn();
                if (you.attribute[ATTR_NUM_DEMONIC_POWERS] == 5 && you.experience_level == 27)
                    demonspawn();
/*if (you.attribute [ATTR_NUM_DEMONIC_POWERS] == 6 && (you.experience_level == 8 || (you.experience_level < 8 && random2(3) == 0))
   demonspawn(); */
                if (you.experience_level % 4 == 0)
                    increase_stats(random2(3));
                break;
            case SP_GHOUL:      // Ghoul

                if (you.experience_level <= 16)
                {
                    you.hp_max++;
                    you.base_hp2++;
                }
                if (you.experience_level % 2 == 0)
                {
                    you.base_hp2++;
                    you.hp_max++;
                }
                if (you.experience_level % 3 == 0)
                {
                    you.base_magic_points2--;
                }
                if (you.experience_level % 5 == 0)
                    increase_stats(STAT_STRENGTH);
                break;
            case SP_KENKU:      // Kenku

                if (you.experience_level <= 16)
                {
                    you.hp_max--;
                    you.base_hp2--;
                }
                if (you.experience_level % 3 == 0)
                {
                    you.hp_max--;
                    you.base_hp2--;
                }
                if (you.experience_level % 4 == 0)
                    increase_stats(random2(3));
                if (you.experience_level == 5)
                    mpr("You have gained the ability to fly.");
                if (you.experience_level == 15)
                    mpr("You can now fly continuously.");
                break;

            }
        }


        if (you.hp > you.hp_max)
            you.hp = you.hp_max;
        if (you.magic_points > you.max_magic_points)
            you.magic_points = you.max_magic_points;
        if (you.magic_points <= 0)
            you.magic_points = 0;

        calc_hp();
        calc_ep();

        if (you.experience_level > you.max_level)
            you.max_level = you.experience_level;



/*
   if (you.experience_level < 5) you.rate_regen += 2;
   if (you.experience_level > 4 && you.experience_level <= 10 && you.experience_level % 2 == 0) you.rate_regen += 2;
   if (you.experience_level > 10 && you.experience_level % 2 == 0) you.rate_regen ++;
 */

        you.redraw_hit_points = 1;
        you.redraw_magic_points = 1;

        if (you.religion == GOD_XOM)
            Xom_acts(1, you.experience_level, 1);


    }

    you.redraw_experience = 1;

}


void increase_stats(char which_stat)
{

    switch (which_stat)
    {
    case STAT_STRENGTH:
        you.strength++;
        you.max_strength++;
        you.redraw_strength = 1;
        strcpy(info, "You feel stronger.");
        mpr(info);
        break;
    case STAT_DEXTERITY:
        you.dex++;
        you.max_dex++;
        you.redraw_dexterity = 1;
        strcpy(info, "You feel agile.");
        mpr(info);
        break;
    case STAT_INTELLIGENCE:
        you.intel++;
        you.max_intel++;
        you.redraw_intelligence = 1;
        strcpy(info, "You feel clever.");
        mpr(info);
        break;
    }

}

int check_stealth(void)
{
    int stealth = you.dex * 3;

    switch (you.species)
    {
        case SP_TROLL:
        case SP_OGRE:
        case SP_OGRE_MAGE:
        case SP_CENTAUR:
            stealth += (you.skills[SK_STEALTH] * 9);
            break;

        case SP_MINOTAUR:
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
            stealth += (you.skills[SK_STEALTH] * 12);
            break;

        case SP_GNOME:
        case SP_HALFLING:
        case SP_KOBOLD:
        case SP_SPRIGGAN:
        case SP_NAGA:           // not small but very good at stealth
            stealth += (you.skills[SK_STEALTH] * 18);
            break;

        default:
            stealth += (you.skills[SK_STEALTH] * 15);
            break;
    }

    if (you.burden_state == 2)
        stealth /= 2;
    if (you.burden_state == 5)
        stealth /= 5;

    if (you.equip[EQ_BODY_ARMOUR] != -1 && !player_light_armour())
    {
        stealth -=
                mass(OBJ_ARMOUR, you.inv_type[you.equip[EQ_BODY_ARMOUR]]) / 10;
    }

    if (you.equip[EQ_BOOTS] != -1)
    {
        if (you.inv_dam[you.equip[EQ_BOOTS]] % 30 == SPARM_STEALTH)
            stealth += 50;      // boots of stealth

        if (you.inv_dam[you.equip[EQ_BOOTS]] / 30 == 4)
            stealth += 20;      // elven
        /* boots */
    }


    if (you.equip[EQ_CLOAK] != -1)
    {
        if (you.inv_dam[you.equip[EQ_CLOAK]] / 30 == 4)
            stealth += 20;      // elven
        /* cloaks */
    }

    if (you.levitation != 0)
        stealth += 10;
    else if (grd[you.x_pos][you.y_pos] == DNGN_SHALLOW_WATER)
        stealth /= 2;           /* Walking through water */

    if (you.special_wield == SPWLD_SHADOW)
        stealth = 0;            // shadow lantern

    if (stealth <= 0)
        stealth = 0;

    return stealth;
}


void ability_increase()
{
    unsigned char keyin;

    strcpy(info, "Your experience leads to an increase in your attributes!");
    mpr(info);
    more();
    mesclr();
    strcpy(info, "Increase Strength, Intelligence, or Dexterity? ");
    mpr(info);

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
        increase_stats(STAT_STRENGTH);
        return;

    case 'i':
    case 'I':
        increase_stats(STAT_INTELLIGENCE);
        return;

    case 'd':
    case 'D':
        increase_stats(STAT_DEXTERITY);
        return;

    }

    goto get_key;
/* this is an infinite loop because it is reasonable to assume that you're not going to want to leave it prematurely. */

}


void display_char_status()
{
    if (you.is_undead == 0)
        mpr("You are alive.");
    else
        mpr("You are undead.");

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
    }

    if (you.duration[DUR_LIQUID_FLAMES] != 0)
        mpr("You are covered in liquid flames.");

    if (you.duration[DUR_ICY_ARMOUR] != 0)
        mpr("You are protected by an icy shield.");

    if (you.duration[DUR_REPEL_MISSILES] != 0)
        mpr("You are protected from missiles.");

    if (you.duration[DUR_DEFLECT_MISSILES] != 0)
        mpr("You deflect missiles.");

    if (you.duration[DUR_PRAYER] != 0)
        mpr("You are praying.");        // not yet implemented

    if (you.duration[DUR_REGENERATION] != 0)
        mpr("You are regenerating.");

    if (you.duration[DUR_SWIFTNESS] != 0)
        mpr("You can move swiftly.");

    if (you.duration[DUR_INSULATION] != 0)
        mpr("You are insulated.");

    if (you.duration[DUR_STONEMAIL] != 0)
        mpr("You are covered in scales of stone.");

    if (you.duration[DUR_CONTROLLED_FLIGHT] != 0)
        mpr("You can control your flight.");

    if (you.duration[DUR_TELEPORT] != 0)
        mpr("You are about to teleport.");

    if (you.duration[DUR_CONTROL_TELEPORT] != 0)
        mpr("You can control teleportation.");

    if (you.duration[DUR_DEATH_CHANNEL] != 0)
        mpr("You are channeling the dead.");

    if (you.invis != 0)
        mpr("You are invisible.");

    if (you.conf != 0)
        mpr("You are confused.");

    if (you.paralysis != 0)
        mpr("You are paralysed.");

    if (you.exhausted != 0)
        mpr("You are exhausted.");

    if (you.slow != 0)
        mpr("You are moving very slowly.");

    if (you.haste != 0)
        mpr("You are moving very quickly.");

    if (you.might != 0)
        mpr("You are mighty.");

    if (you.berserker != 0)
        mpr("You are possessed by a berserker rage.");

    if (you.levitation != 0)
        mpr("You are hovering above the floor.");

    if (you.poison > 10)
        mpr("You are extremely poisoned.");
    else if (you.poison > 5)
        mpr("You are very poisoned.");
    else if (you.poison >= 2)
        mpr("You are quite poisoned.");
    else if (you.poison != 0)
        mpr("You are poisoned.");

    if (you.deaths_door != 0)
        mpr("You are standing in death's doorway.");

    if (you.disease != 0)
    {
        if (you.disease > 120)
            mpr("You are badly diseased.");
        else if (you.disease > 40)
            mpr("You are diseased.");
        else
            mpr("You are mildly diseased.");
    }

    if (you.magic_contamination > 5)
    {
        if (you.magic_contamination > 25)
            mpr("You are almost glowing with magical energy.");
        else if (you.magic_contamination > 15)
            mpr("You are infused with magical energy.");
        else
            mpr("You are mildly infused with magical energy.");
    }
}


void redraw_skill(char your_name[kNameLen], char class_name[40])
{
    char print_it[80];
    char print_it2[42];

    int i = 0;
    char spaces = 0;

    strcpy(print_it, your_name);
    strcat(print_it, " the ");
    strcat(print_it, class_name);

    strncpy(print_it2, print_it, 39);

    for (i = 0; i < 40; i++)
    {
        if (print_it2[i] == 0)
            spaces = 1;
        if (spaces == 1)
            print_it2[i] = ' ';
    }

    print_it2[39] = 0;

    textcolor(LIGHTGREY);
#ifdef DOS_TERM
    window(1, 1, 80, 25);
#endif
    gotoxy(40, 1);
    textcolor(7);
    cprintf(print_it2);
}


char *species_name(char speci)
{

    if (you.species >= SP_RED_DRACONIAN && you.species <= SP_UNK2_DRACONIAN && you.experience_level < 7)
        return "Draconian";     /* Causes minor problems with ghosts, but nevermind */

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
        return "Red Draconian"; /* Fire */
    case SP_WHITE_DRACONIAN:
        return "White Draconian";       /* Cold */
    case SP_GREEN_DRACONIAN:
        return "Green Draconian";       /* Poison */
    case SP_GOLDEN_DRACONIAN:
        return "Yellow Draconian";      /* Acid */
    case SP_GREY_DRACONIAN:
        return "Grey Draconian";        /* Nothing */
    case SP_BLACK_DRACONIAN:
        return "Black Draconian";       /* Elec */
    case SP_PURPLE_DRACONIAN:
        return "Purple Draconian";      /* Energy */

    case SP_MOTTLED_DRACONIAN:
        return "Mottled Draconian";     /*  */
    case SP_PALE_DRACONIAN:
        return "Pale Draconian";        /*  */
    case SP_UNK0_DRACONIAN:
        return "Draconian";     /*  */
    case SP_UNK1_DRACONIAN:
        return "Draconian";     /*  */
    case SP_UNK2_DRACONIAN:
        return "Draconian";     /*  */
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

    }

    return "Yak";
}


char wearing_amulet(char amulet)
{
    if (amulet == AMU_CONTROLLED_FLIGHT &&
        (you.duration[DUR_CONTROLLED_FLIGHT] != 0 ||
    (you.species >= SP_RED_DRACONIAN && you.species <= SP_UNK2_DRACONIAN) ||
         you.attribute[ATTR_TRANSFORMATION] == TRAN_DRAGON))
        return 1;               // controlled flight - duration [10] : flight spell in operation

    if (amulet == AMU_CLARITY && you.mutation[MUT_CLARITY] != 0)
        return 1;

    if (amulet == AMU_RESIST_CORROSION || amulet == AMU_CONSERVATION)
        if (you.equip[EQ_CLOAK] != -1 && you.inv_dam[you.equip[EQ_CLOAK]] % 30 == SPARM_PRESERVATION)
            return 1;

    if (you.equip[EQ_AMULET] == -1)
        return 0;

    if (you.inv_type[you.equip[EQ_AMULET]] == amulet)
        return 1;

    return 0;
}


int species_exp_mod(char species)
{
    switch (species)
    {
    case SP_HUMAN:
        return 10;              // human

    case SP_ELF:
        return 12;              // elf

    case SP_HIGH_ELF:
        return 15;              // h elf

    case SP_GREY_ELF:
        return 14;              // g elf

    case SP_DEEP_ELF:
        return 14;              // d e

    case SP_SLUDGE_ELF:
        return 12;              // s e

    case SP_HILL_DWARF:
        return 13;              // h dw

    case SP_MOUNTAIN_DWARF:
        return 13;              // m dw

    case SP_HALFLING:
        return 10;              // 1/2ling

    case SP_HILL_ORC:
        return 10;              // ho

    case SP_KOBOLD:
        return 10;              // kobold

    case SP_MUMMY:
        return 15;              // mummy

    case SP_NAGA:
        return 12;              // naga

    case SP_GNOME:
        return 11;              // Gnome

    case SP_OGRE:
        return 14;              // ogre

    case SP_TROLL:
        return 15;              // troll

    case SP_OGRE_MAGE:
        return 15;              // ogre mage

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
        return 14;              // draconian

    case SP_CENTAUR:
        return 14;              // centaur

    case SP_DEMIGOD:
        return 16;              // demigod

    case SP_SPRIGGAN:
        return 13;              // Spriggan

    case SP_MINOTAUR:
        return 14;              // Minotaur

    case SP_DEMONSPAWN:
        return 14;              // Demonspawn

    case SP_GHOUL:
        return 12;              // Ghoul

    case SP_KENKU:
        return 13;              // Kenku

    default:
        return 0;
    }

}

unsigned long exp_needed(int lev, char species)
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
/*        case 1: return 1;
   case 2: return 10;
   case 3: return 20;
   case 4: return 45;
   case 5: return 90;
   case 6: return 200;
   case 7: return 410;
   case 8: return 800;
   case 9: return 1400;
   case 10: return 3000;
   case 11: return 7000;
   case 12: return 14000; */
//      case 13: return 20480;
        //      case 14: return 40960;
        //      case 15: return 81920;
        //      case 16: return 163840;
        //      case 17: return 327760;
    default:                    //return 14000 * (lev - 11);

        level = 20000 * (lev - 11) + ((lev - 11) * (lev - 11) * (lev - 11)) * 130;
        break;
    }

    return (level - 1) * species_exp_mod(species) / 10;

}



int slaying_bonus(char which_affected)  /* returns bonuses from rings of slaying etc */
{
    int to_hit = 0;
    int to_dam = 0;
    int i = 0;

    for (i = EQ_LEFT_RING; i < EQ_AMULET; i++)
    {
        if (you.equip[i] == -1)
            continue;
        if (you.inv_type[you.equip[i]] != RING_SLAYING)
            continue;

        int ghoggl = you.inv_plus[you.equip[i]] - 50;

        if (you.inv_plus[you.equip[i]] > 130)
            ghoggl -= 100;
        to_hit += ghoggl;
        to_dam += you.inv_plus2[you.equip[i]] - 50;
    }

    if (which_affected == 0)
    {
        to_hit += scan_randarts(RAP_ACCURACY);
        return to_hit;
    }

    if (which_affected == 1)
    {
        to_dam += scan_randarts(RAP_DAMAGE);
        return to_dam;
    }

    return 0;
}


/* Checks each equip slot for a randart, and adds up all of those with
   a given property. Slow if any randarts are worn, so avoid where possible. */
int scan_randarts(char which_property)
{

    int i = 0;
    int retval = 0;

//for (i = 0; i < 10; i ++)
    for (i = EQ_WEAPON; i < EQ_LEFT_RING; i++)
    {
        if (you.equip[i] == -1)
            continue;
        if (i == EQ_WEAPON && you.inv_class[you.equip[i]] != OBJ_WEAPONS)
            continue;
        if (you.inv_dam[you.equip[i]] % 30 < 25)
            continue;           /* change for rings/amulets */

        retval += randart_wpn_properties(you.inv_class[you.equip[i]], you.inv_type[you.equip[i]], you.inv_dam[you.equip[i]], you.inv_plus[you.equip[i]], you.inv_plus2[you.equip[i]], 0, which_property);

    }

    for (i = EQ_LEFT_RING; i < 10; i++)         /* rings + amulets */
    {
        if (you.equip[i] == -1)
            continue;
        if (you.inv_dam[you.equip[i]] != 200 && you.inv_dam[you.equip[i]] != 201)
            continue;
        retval += randart_wpn_properties(you.inv_class[you.equip[i]], you.inv_type[you.equip[i]], you.inv_dam[you.equip[i]], you.inv_plus[you.equip[i]], you.inv_plus2[you.equip[i]], 0, which_property);
    }

    return retval;

}

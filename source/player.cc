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
#include <stdio.h>

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
#include "spells4.h"
#include "stuff.h"
#include "view.h"
#include "wpn-misc.h"

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

bool player_is_swimming(void)
{
    return (player_in_water() && you.species == SP_MERFOLK);
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
            return (true);
        else
            return (false);
        break;

    case SP_ELF:
    case SP_HIGH_ELF:
    case SP_GREY_ELF:
    case SP_DEEP_ELF:
    case SP_SLUDGE_ELF:
        if (which_genus == GENPC_ELVEN)
            return (true);
        else
            return (false);
        break;

    case SP_HILL_DWARF:
    case SP_MOUNTAIN_DWARF:
        if (which_genus == GENPC_DWARVEN)
            return (true);
        else
            return (false);
        break;

    default:
        return (false);
        break;
    }
}                               // end player_genus()

// Looks in equipment "slot" to see if there is an equiped "sub_type".
// Returns number of matches (in the case of rings, both are checked)
int player_equip( int slot, int sub_type )
{
    int ret = 0;

    switch (slot)
    {
    case EQ_WEAPON:
        // Hands can have more than just weapons.
        if (you.equip[EQ_WEAPON] != -1
            && you.inv[you.equip[EQ_WEAPON]].base_type == OBJ_WEAPONS
            && you.inv[you.equip[EQ_WEAPON]].sub_type == sub_type)
        {
            ret++;
        }
        break;

    case EQ_STAFF:
        // Like above, but must be magical stave.
        if (you.equip[EQ_WEAPON] != -1
            && you.inv[you.equip[EQ_WEAPON]].base_type == OBJ_STAVES
            && you.inv[you.equip[EQ_WEAPON]].sub_type == sub_type)
        {
            ret++;
        }
        break;

    case EQ_RINGS:
        if (you.equip[EQ_LEFT_RING] != -1
            && you.inv[you.equip[EQ_LEFT_RING]].sub_type == sub_type)
        {
            ret++;
        }

        if (you.equip[EQ_RIGHT_RING] != -1
            && you.inv[you.equip[EQ_RIGHT_RING]].sub_type == sub_type)
        {
            ret++;
        }
        break;

    case EQ_RINGS_PLUS:
        if (you.equip[EQ_LEFT_RING] != -1
            && you.inv[you.equip[EQ_LEFT_RING]].sub_type == sub_type)
        {
            ret += you.inv[you.equip[EQ_LEFT_RING]].plus;
        }

        if (you.equip[EQ_RIGHT_RING] != -1
            && you.inv[you.equip[EQ_RIGHT_RING]].sub_type == sub_type)
        {
            ret += you.inv[you.equip[EQ_RIGHT_RING]].plus;
        }
        break;

    case EQ_RINGS_PLUS2:
        if (you.equip[EQ_LEFT_RING] != -1
            && you.inv[you.equip[EQ_LEFT_RING]].sub_type == sub_type)
        {
            ret += you.inv[you.equip[EQ_LEFT_RING]].plus2;
        }

        if (you.equip[EQ_RIGHT_RING] != -1
            && you.inv[you.equip[EQ_RIGHT_RING]].sub_type == sub_type)
        {
            ret += you.inv[you.equip[EQ_RIGHT_RING]].plus2;
        }
        break;

    case EQ_ALL_ARMOUR:
        // doesn't make much sense here... be specific. -- bwr
        break;

    default:
        if (you.equip[slot] != -1
            && you.inv[you.equip[slot]].sub_type == sub_type)
        {
            ret++;
        }
        break;
    }

    return (ret);
}


// Looks in equipment "slot" to see if equiped item has "special" ego-type
// Returns number of matches (jewellery returns zero -- no ego type).
int player_equip_special( int slot, int special )
{
    int ret = 0;

    switch (slot)
    {
    case EQ_WEAPON:
        // Hands can have more than just weapons.
        if (you.equip[EQ_WEAPON] != -1
            && you.inv[you.equip[EQ_WEAPON]].base_type == OBJ_WEAPONS
            && you.inv[you.equip[EQ_WEAPON]].special == special)
        {
            ret++;
        }
        break;

    case EQ_LEFT_RING:
    case EQ_RIGHT_RING:
    case EQ_AMULET:
    case EQ_STAFF:
    case EQ_RINGS:
    case EQ_RINGS_PLUS:
    case EQ_RINGS_PLUS2:
        // no ego types for these slots
        break;

    case EQ_ALL_ARMOUR:
        // Check all armour slots:
        for (int i = EQ_CLOAK; i <= EQ_BODY_ARMOUR; i++)
        {
            if (you.equip[i] != -1
                && you.inv[you.equip[i]].special == special)
            {
                ret++;
            }
        }
        break;

    default:
        // Check a specific armour slot for an ego type:
        if (you.equip[slot] != -1
            && you.inv[you.equip[slot]].special == special)
        {
            ret++;
        }
        break;
    }

    return (ret);
}

int player_damage_type( void )
{
    const int wpn = you.equip[ EQ_WEAPON ];

    if (wpn != -1)
    {
        return (damage_type( you.inv[wpn].base_type, you.inv[wpn].sub_type ));
    }
    else if (you.attribute[ATTR_TRANSFORMATION] == TRAN_BLADE_HANDS
            || you.attribute[ATTR_TRANSFORMATION] == TRAN_DRAGON
            || you.mutation[MUT_CLAWS]
            || you.species == SP_TROLL
            || you.species == SP_GHOUL)
    {
        return (DVORP_SLICING);
    }

    return (DVORP_CRUSHING);
}

int player_teleport(void)
{
    int tp = 0;

    /* rings */
    tp += 8 * player_equip( EQ_RINGS, RING_TELEPORTATION );

    /* mutations */
    tp += you.mutation[MUT_TELEPORT] * 3;

    /* randart weapons only */
    if (you.equip[EQ_WEAPON] != -1
        && you.inv[you.equip[EQ_WEAPON]].base_type == OBJ_WEAPONS
        && is_random_artefact( you.inv[you.equip[EQ_WEAPON]] ))
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
    rr += 40 * player_equip( EQ_RINGS, RING_REGENERATION );

    /* spell */
    if (you.duration[DUR_REGENERATION])
        rr += 100;

    /* troll or troll leather -- trolls can't get both */
    if (you.species == SP_TROLL)
        rr += 40;
    else if (player_equip( EQ_BODY_ARMOUR, ARM_TROLL_LEATHER_ARMOUR ))
        rr += 30;

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

    // moved here from acr.cc... maintaining the >= 40 behaviour
    if (you.hunger >= 40)
    {
        if (you.invis > 0)
            hunger += 5;

        // berserk has its own food penalty -- excluding berserk haste
        if (you.haste > 0 && !you.berserker)
            hunger += 5;
    }

    hunger += you.mutation[MUT_FAST_METABOLISM];

    if (you.mutation[MUT_SLOW_METABOLISM] > 2)
        hunger -= 2;
    else if (you.mutation[MUT_SLOW_METABOLISM] > 0)
        hunger--;

    // rings
    hunger += 2 * player_equip( EQ_RINGS, RING_REGENERATION );
    hunger += 4 * player_equip( EQ_RINGS, RING_HUNGER );
    hunger -= 2 * player_equip( EQ_RINGS, RING_SUSTENANCE );

    // weapon ego types
    hunger += 6 * player_equip_special( EQ_WEAPON, SPWPN_VAMPIRICISM );
    hunger += 9 * player_equip_special( EQ_WEAPON, SPWPN_VAMPIRES_TOOTH );

    // troll leather armour
    hunger += player_equip( EQ_BODY_ARMOUR, ARM_TROLL_LEATHER_ARMOUR );

    // randarts
    hunger += scan_randarts(RAP_METABOLISM);

    // burden
    hunger += you.burden_state;

    if (hunger < 1)
        hunger = 1;

    return (hunger);
}

int player_spell_levels(void)
{
    int sl = (you.experience_level - 1) + (you.skills[SK_SPELLCASTING] * 2);

    bool fireball = false;
    bool delayed_fireball = false;

    if (sl > 99)
        sl = 99;

    for (int i = 0; i < 25; i++)
    {
        if (you.spells[i] == SPELL_FIREBALL)
            fireball = true;
        else if (you.spells[i] == SPELL_DELAYED_FIREBALL)
            delayed_fireball = true;

        if (you.spells[i] != SPELL_NO_SPELL)
            sl -= spell_difficulty(you.spells[i]);
    }

    // Fireball is free for characters with delayed fireball
    if (fireball && delayed_fireball)
        sl += spell_difficulty( SPELL_FIREBALL );

    // Note: This can happen because of level drain.  Maybe we should
    // force random spells out when that happens. -- bwr
    if (sl < 0)
        sl = 0;

    return (sl);
}

int player_res_magic(void)
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

    /* armour  */
    rm += 30 * player_equip_special( EQ_ALL_ARMOUR, SPARM_MAGIC_RESISTANCE );

    /* rings of magic resistance */
    rm += 40 * player_equip( EQ_RINGS, RING_PROTECTION_FROM_MAGIC );

    /* randarts */
    rm += scan_randarts(RAP_MAGIC);

    /* Enchantment skill */
    rm += you.skills[SK_ENCHANTMENTS];

    /* Mutations */
    rm += you.mutation[MUT_MAGIC_RESISTANCE] * 30;

    /* transformations */
    if (you.attribute[ATTR_TRANSFORMATION] == TRAN_LICH)
        rm += 50;

    return rm;
}

int player_res_fire(void)
{
    int rf = 100;

    /* rings of fire resistance/fire */
    rf += player_equip( EQ_RINGS, RING_PROTECTION_FROM_FIRE );
    rf += player_equip( EQ_RINGS, RING_FIRE );

    /* rings of ice */
    rf -= player_equip( EQ_RINGS, RING_ICE );

    /* Staves */
    rf += player_equip( EQ_STAFF, STAFF_FIRE );

    // body armour:
    rf += 2 * player_equip( EQ_BODY_ARMOUR, ARM_DRAGON_ARMOUR );
    rf += player_equip( EQ_BODY_ARMOUR, ARM_GOLD_DRAGON_ARMOUR );
    rf -= player_equip( EQ_BODY_ARMOUR, ARM_ICE_DRAGON_ARMOUR );

    // ego armours
    rf += player_equip_special( EQ_ALL_ARMOUR, SPARM_FIRE_RESISTANCE );
    rf += player_equip_special( EQ_ALL_ARMOUR, SPARM_RESISTANCE );

    // randart weapons:
    rf += scan_randarts(RAP_FIRE);

    // species:
    if (you.species == SP_MUMMY)
        rf--;
    else if (you.species == SP_RED_DRACONIAN && you.experience_level > 17)
        rf++;

    // mutations:
    rf += you.mutation[MUT_HEAT_RESISTANCE];

    if (you.fire_shield)
        rf += 2;

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

    /* rings of fire resistance/fire */
    rc += player_equip( EQ_RINGS, RING_PROTECTION_FROM_COLD );
    rc += player_equip( EQ_RINGS, RING_ICE );

    /* rings of ice */
    rc -= player_equip( EQ_RINGS, RING_FIRE );

    /* Staves */
    rc += player_equip( EQ_STAFF, STAFF_COLD );

    // body armour:
    rc += 2 * player_equip( EQ_BODY_ARMOUR, ARM_ICE_DRAGON_ARMOUR );
    rc += player_equip( EQ_BODY_ARMOUR, ARM_GOLD_DRAGON_ARMOUR );
    rc -= player_equip( EQ_BODY_ARMOUR, ARM_DRAGON_ARMOUR );

    // ego armours
    rc += player_equip_special( EQ_ALL_ARMOUR, SPARM_COLD_RESISTANCE );
    rc += player_equip_special( EQ_ALL_ARMOUR, SPARM_RESISTANCE );

    // randart weapons:
    rc += scan_randarts(RAP_COLD);

    // species:
    if (you.species == SP_MUMMY || you.species == SP_GHOUL)
        rc++;
    else if (you.species == SP_WHITE_DRACONIAN && you.experience_level > 17)
        rc++;

    // mutations:
    rc += you.mutation[MUT_COLD_RESISTANCE];

    if (you.fire_shield)
        rc -= 2;

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

    // staff
    re += player_equip( EQ_STAFF, STAFF_AIR );

    // body armour:
    re += player_equip( EQ_BODY_ARMOUR, ARM_STORM_DRAGON_ARMOUR );

    // randart weapons:
    re += scan_randarts(RAP_ELECTRICITY);

    // species:
    if (you.species == SP_BLACK_DRACONIAN && you.experience_level > 17)
        re++;

    // mutations:
    if (you.mutation[MUT_SHOCK_RESISTANCE])
        re++;

    // transformations:
    if (you.attribute[ATTR_TRANSFORMATION] == TRAN_AIR)
        re += 2;  // mutliple levels currently meaningless

    return re;
}                               // end player_res_electricity()

// funny that no races are susceptible to poisons {dlb}
int player_res_poison(void)
{
    int rp = 0;

    /* rings of poison resistance */
    rp += player_equip( EQ_RINGS, RING_POISON_RESISTANCE );

    /* Staves */
    rp += player_equip( EQ_STAFF, STAFF_POISON );

    /* the staff of Olgreb: */
    rp += player_equip_special( EQ_WEAPON, SPWPN_STAFF_OF_OLGREB );

    // ego armour:
    rp += player_equip_special( EQ_ALL_ARMOUR, SPARM_POISON_RESISTANCE );

    // body armour:
    rp += player_equip( EQ_BODY_ARMOUR, ARM_GOLD_DRAGON_ARMOUR );
    rp += player_equip( EQ_BODY_ARMOUR, ARM_SWAMP_DRAGON_ARMOUR );

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
    sd += player_equip( EQ_STAFF, STAFF_DEATH );

    // body armour:
    if (player_equip_special( EQ_BODY_ARMOUR, SPARM_ARCHMAGI ))
        sd++;

    // species:
    if (you.species == SP_MUMMY)
    {
        if (you.experience_level > 12)
            sd++;
        if (you.experience_level > 25)
            sd++;
    }

    // transformations:
    if (you.attribute[ATTR_TRANSFORMATION] == TRAN_LICH)
        sd++;

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
    sf += player_equip( EQ_STAFF, STAFF_FIRE );

    // rings of fire:
    sf += player_equip( EQ_RINGS, RING_FIRE );

    if (you.fire_shield)
        sf++;

    return sf;
}

unsigned char player_spec_cold(void)
{
    int sc = 0;

    // staves:
    sc += player_equip( EQ_STAFF, STAFF_COLD );

    // rings of ice:
    sc += player_equip( EQ_RINGS, RING_ICE );

    return sc;
}

unsigned char player_spec_earth(void)
{
    int se = 0;

    /* Staves */
    se += player_equip( EQ_STAFF, STAFF_EARTH );

    if (you.attribute[ATTR_TRANSFORMATION] == TRAN_AIR)
        se--;

    return se;
}

unsigned char player_spec_air(void)
{
    int sa = 0;

    /* Staves */
    sa += player_equip( EQ_STAFF, STAFF_AIR );

    //jmf: this was too good
    //if (you.attribute[ATTR_TRANSFORMATION] == TRAN_AIR)
    //  sa++;
    return sa;
}

unsigned char player_spec_conj(void)
{
    int sc = 0;

    /* Staves */
    sc += player_equip( EQ_STAFF, STAFF_CONJURATION );

    // armour of the Archmagi
    if (player_equip_special( EQ_BODY_ARMOUR, SPARM_ARCHMAGI ))
        sc++;

    return sc;
}

unsigned char player_spec_ench(void)
{
    int se = 0;

    /* Staves */
    se += player_equip( EQ_STAFF, STAFF_ENCHANTMENT );

    // armour of the Archmagi
    if (player_equip_special( EQ_BODY_ARMOUR, SPARM_ARCHMAGI ))
        se++;

    return se;
}

unsigned char player_spec_summ(void)
{
    int ss = 0;

    /* Staves */
    ss += player_equip( EQ_STAFF, STAFF_SUMMONING );

    // armour of the Archmagi
    if (player_equip_special( EQ_BODY_ARMOUR, SPARM_ARCHMAGI ))
        ss++;

    return ss;
}

unsigned char player_spec_poison(void)
{
    int sp = 0;

    /* Staves */
    sp += player_equip( EQ_STAFF, STAFF_POISON );

    // armour of the Archmagi
    if (player_equip_special( EQ_WEAPON, SPWPN_STAFF_OF_OLGREB ))
        sp++;

    return sp;
}

unsigned char player_energy(void)
{
    unsigned char pe = 0;

    // Staves
    pe += player_equip( EQ_STAFF, STAFF_ENERGY );

    return pe;
}

int player_prot_life(void)
{
    int pl = 0;

    // rings
    pl += player_equip( EQ_RINGS, RING_LIFE_PROTECTION );

    // armour: (checks body armour only)
    pl += player_equip_special( EQ_ALL_ARMOUR, SPARM_POSITIVE_ENERGY );

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

    if (you.species == SP_MERFOLK && player_is_swimming())
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

        /* armour */
        if (player_equip_special( EQ_BOOTS, SPARM_RUNNING ))
            mv -= 2;

        if (player_equip_special( EQ_BODY_ARMOUR, SPARM_PONDEROUSNESS ))
            mv += 2;

        // Swiftness is an Air spell, it doesn't work so well in water -- bwr
        if (!player_in_water())
        {
            // Now when it says "you feel a little slower" you really are:
            if (you.duration[DUR_SWIFTNESS] > 6)
                mv -= 2;
            else if (you.duration[DUR_SWIFTNESS] > 0)
                mv--;
        }

        /* Mutations: -2, -3, -4 */
        if (you.mutation[MUT_FAST] > 0)
            mv -= (you.mutation[MUT_FAST] + 1);

        // Burden
        if (you.burden_state == BS_ENCUMBERED)
            mv += 1;
        else if (you.burden_state == BS_OVERLOADED)
            mv += 3;
    }

    // We'll use the old value of six as a minimum, with haste this could
    // end up with a speed of three, which is about as fast as we want
    // the player to be able to go (since 3 is 3.33x as fast and 2 is 5x
    // which is a bit of a jump, and a bit too fast) -- bwr
    if (mv < 6)
        mv = 6;

    // Nagas move slowly:
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

    // get the armour race value that corresponds to the character's race:
    const unsigned long racial_type
                            = ((player_genus(GENPC_DWARVEN)) ? ISFLAG_DWARVEN :
                               (player_genus(GENPC_ELVEN))   ? ISFLAG_ELVEN :
                               (you.species == SP_HILL_ORC)  ? ISFLAG_ORCISH
                                                             : 0);

    for (i = EQ_CLOAK; i <= EQ_BODY_ARMOUR; i++)
    {
        const int item = you.equip[i];

        if (item == -1 || i == EQ_SHIELD)
            continue;

        AC += you.inv[ item ].plus;

        // Note that helms and boots have a sub-sub classing system
        // which uses "plus2"... since not all members have the same
        // AC value, we use special cases. -- bwr
        if (i == EQ_HELMET
            && (cmp_helmet_type( you.inv[ item ], THELM_CAP )
                || cmp_helmet_type( you.inv[ item ], THELM_WIZARD_HAT )
                || cmp_helmet_type( you.inv[ item ], THELM_SPECIAL )))
        {
            continue;
        }

        if (i == EQ_BOOTS
            && (you.inv[ item ].plus2 == TBOOT_NAGA_BARDING
                || you.inv[ item ].plus2 == TBOOT_CENTAUR_BARDING))
        {
            AC += 3;
            continue;
        }

        int racial_bonus = 0;  // additional levels of armour skill
        const unsigned long armour_race = get_equip_race( you.inv[ item ] );
        const int ac_value = property( you.inv[ item ], PARM_AC );

        // Dwarven armour is universally good -- bwr
        if (armour_race == ISFLAG_DWARVEN)
            racial_bonus += 2;

        if (racial_type && armour_race == racial_type)
        {
            // Elven armour is too flexible, but still gives one
            // level to elves.  Orcish and Dwarven armour are worth +2
            // to the correct species, plus the plus that anyone
            // gets with dwarven armour. -- bwr

            if (racial_type == ISFLAG_ELVEN)
                racial_bonus++;
            else
                racial_bonus += 2;
        }

        AC += ac_value * (15 + you.skills[SK_ARMOUR] + racial_bonus) / 15;

        /* Nagas/Centaurs/the deformed don't fit into body armour very well */
        if ((you.species == SP_NAGA || you.species == SP_CENTAUR
             || you.mutation[MUT_DEFORMED] > 0) && i == EQ_BODY_ARMOUR)
        {
            AC -= ac_value / 2;
        }
    }

    AC += player_equip( EQ_RINGS_PLUS, RING_PROTECTION );

    if (player_equip_special( EQ_WEAPON, SPWPN_PROTECTION ))
        AC += 5;

    if (player_equip_special( EQ_SHIELD, SPARM_PROTECTION ))
        AC += 3;

    AC += scan_randarts(RAP_AC);

    if (you.duration[DUR_ICY_ARMOUR])
        AC += 4 + you.skills[SK_ICE_MAGIC] / 3;

    if (you.duration[DUR_STONEMAIL])
        AC += 5 + you.skills[SK_EARTH_MAGIC] / 2;

    if (you.duration[DUR_STONESKIN] > 0)
        AC += 1 + you.skills[SK_EARTH_MAGIC] / 5;

    if (you.attribute[ATTR_TRANSFORMATION] == TRAN_NONE
        || you.attribute[ATTR_TRANSFORMATION] == TRAN_BLADE_HANDS)
    {
        //jmf: only give:
        if (player_genus(GENPC_DRACONIAN))
        {
            if (you.experience_level < 8)
                AC += 2;
            else if (you.species == SP_GREY_DRACONIAN)
                AC += 1 + (you.experience_level - 4) / 2;       // max 12
            else
                AC += 1 + (you.experience_level / 4);           // max 7
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
    }
    else
    {
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
    }

    return AC;
}

bool is_light_armour( const item_def &item )
{
    if (cmp_equip_race( item, ISFLAG_ELVEN ))
        return (true);

    switch (item.sub_type)
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

    return (is_light_armour( you.inv[you.equip[EQ_BODY_ARMOUR]] ));
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
        armour_ev_penalty = property( you.inv[you.equip[EQ_BODY_ARMOUR]],
                                      PARM_EVASION );
    }

    // We return 2 here to give the player some chance of not being hit,
    // repulsion fields still work while paralysed
    if (you.paralysis)
    {
        if (you.mutation[MUT_REPULSION_FIELD])
            return (1 + you.mutation[MUT_REPULSION_FIELD] * 2);
        else
            return (2);
    }

    if (you.species == SP_CENTAUR)
        ev -= 3;

    if (you.equip[EQ_BODY_ARMOUR] != -1)
    {
        int ev_change = 0;

        ev_change = armour_ev_penalty;
        ev_change += you.skills[SK_ARMOUR] / 3;

        if (ev_change > armour_ev_penalty / 3)
            ev_change = armour_ev_penalty / 3;

        ev += ev_change;        /* remember that it's negative */
    }

    ev += player_equip( EQ_RINGS_PLUS, RING_EVASION );

    if (player_equip_special( EQ_BODY_ARMOUR, SPARM_PONDEROUSNESS ))
        ev -= 2;

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

    emod += you.skills[SK_DODGING] / 2;

    if (emod > 0)
        ev += emod;

    if (you.mutation[MUT_REPULSION_FIELD] > 0)
        ev += (you.mutation[MUT_REPULSION_FIELD] * 2) - 1;

    switch (you.attribute[ATTR_TRANSFORMATION])
    {
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

    ma += 3 * player_equip( EQ_RINGS, RING_WIZARDRY );

    /* Staves */
    ma += 4 * player_equip( EQ_STAFF, STAFF_WIZARDRY );

    /* armour of the Archmagi (checks body armour only) */
    ma += 2 * player_equip_special( EQ_BODY_ARMOUR, SPARM_ARCHMAGI );

    return ((is_weighted) ? ((ma * you.intel) / 10) : ma);
}                               // end player_mag_abil()

int player_shield_class(void)   //jmf: changes for new spell
{
    int base_shield = 0;
    const int shield = you.equip[EQ_SHIELD];

    if (shield == -1)
    {
        if (!you.fire_shield && you.duration[DUR_CONDENSATION_SHIELD])
            base_shield += 2 + (you.skills[SK_ICE_MAGIC] / 6);  // max 6
        else
            return 0;
    }
    else
    {
        switch (you.inv[ shield ].sub_type)
        {
        case ARM_BUCKLER:
            base_shield += 3;   // +3/20 per skill level    max 6
            break;
        case ARM_SHIELD:
            base_shield += 5;   // +5/20 per skill level    max 11
            break;
        case ARM_LARGE_SHIELD:
            base_shield += 7;   // +7/20 per skill level    max 16
            break;
        }

        // bonus applied only to base, see above for effect:
        base_shield *= 10 + (you.skills[SK_SHIELDS] / 2);
        base_shield /= 10;

        base_shield += you.inv[ shield ].plus;
    }

    return base_shield;
}                               // end player_shield_class()

unsigned char player_see_invis(void)
{
    unsigned char si = 0;

    si += player_equip( EQ_RINGS, RING_SEE_INVISIBLE );

    /* armour: (checks head armour only) */
    si += player_equip_special( EQ_HELMET, SPARM_SEE_INVISIBLE );

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

// This does NOT do line of sight!  It checks the monster's visibility
// with repect to the players perception, but doesn't do walls or range...
// to find if the square the monster is in is visible see mons_near().
bool player_monster_visible( struct monsters *mon )
{
    if (mons_has_ench( mon, ENCH_SUBMERGED )
        || (mons_has_ench( mon, ENCH_INVIS ) && !player_see_invis()))
    {
        return (false);
    }

    return (true);
}

unsigned char player_sust_abil(void)
{
    unsigned char sa = 0;

    sa += player_equip( EQ_RINGS, RING_SUSTAIN_ABILITIES );

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
        if (you.inv[bu].quantity < 1)
            continue;

        if (you.inv[bu].base_type == OBJ_CORPSES)
        {
            if (you.inv[bu].sub_type == CORPSE_BODY)
                you.burden += mons_weight(you.inv[bu].plus);
            else if (you.inv[bu].sub_type == CORPSE_SKELETON)
                you.burden += mons_weight(you.inv[bu].plus) / 2;
        }
        else
        {
            you.burden += mass_item( you.inv[bu] ) * you.inv[bu].quantity;
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
            mpr("Your possessions no longer seem quite so burdensome.");
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
    int ench_power = stepdown_value( power, 30, 40, 100, 120 );

    int mrchance = 100 + player_res_magic();

    mrchance -= ench_power;

    int mrch2 = random2(100) + random2(101);

#if DEBUG_DIAGNOSTICS
    snprintf( info, INFO_SIZE, "Power: %d, player's MR: %d, target: %d, roll: %d",
             ench_power, player_res_magic(), mrchance, mrch2 );

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

void gain_exp( unsigned int exp_gained )
{

    if (player_equip_special( EQ_BODY_ARMOUR, SPARM_ARCHMAGI )
        && !one_chance_in(20))
    {
        return;
    }

#if DEBUG_DIAGNOSTICS
    snprintf( info, INFO_SIZE, "gain_exp: %d", exp_gained );
    mpr( info );
#endif

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

void level_change(void)
{
    // necessary for the time being, as level_change() is called
    // directly sometimes {dlb}
    you.redraw_experience = 1;

    while (you.experience_level < 27
        && you.experience > exp_needed(you.experience_level + 2))
    {
        you.experience_level++;

        snprintf( info, INFO_SIZE, "You are now a level %d %s!",
                 you.experience_level, you.class_name );

        mpr(info, MSGCH_INTRINSIC_GAIN);
        more();

        int brek = 0;

        if (you.experience_level > 21)
            brek = (coinflip() ? 3 : 2);
        else if (you.experience_level > 12)
            brek = 3 + random2(3);      // up from 2 + rand(3) -- bwr
        else
            brek = 4 + random2(4);      // up from 3 + rand(4) -- bwr

        inc_hp( brek, true );
        inc_mp( 1, true );

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
                if (you.experience_level == 15)
                {
                    //jmf: got Glamour ability
                    mpr("You feel charming!", MSGCH_INTRINSIC_GAIN);
                }

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
                if (you.experience_level == 5)
                {
                    //jmf: got Glamour ability
                    mpr("You feel charming!", MSGCH_INTRINSIC_GAIN);
                    mp_adjust++;
                }

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
                    you.redraw_armour_class = 1;
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
                    you.redraw_armour_class = 1;
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
                    you.redraw_armour_class = 1;
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
        inc_max_hp( hp_adjust );
        inc_max_mp( mp_adjust );

        deflate_hp(you.hp_max, false);

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
        stealth -= (mass_item( you.inv[you.equip[EQ_BODY_ARMOUR]] ) / 10);
    }

    if (you.equip[EQ_CLOAK] != -1
        && cmp_equip_race( you.inv[you.equip[EQ_CLOAK]], ISFLAG_ELVEN ))
    {
        stealth += 20;
    }

    if (you.equip[EQ_BOOTS] != -1)
    {
        if (you.inv[you.equip[EQ_BOOTS]].special == SPARM_STEALTH)
            stealth += 50;

        if (cmp_equip_race( you.inv[you.equip[EQ_BOOTS]], ISFLAG_ELVEN ))
            stealth += 20;
    }

    if (you.levitation)
        stealth += 10;
    else if (player_in_water())
    {
        // Merfolk can sneak up on monsters underwater -- bwr
        if (you.species == SP_MERFOLK)
            stealth += 50;
        else
            stealth /= 2;       // splashy-splashy
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

    mpr("Increase (S)trength, (I)ntelligence, or (D)exterity? ", MSGCH_PROMPT);

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

    if (you.duration[DUR_BREATH_WEAPON])
        mpr("You are short of breath.");

    if (you.duration[DUR_REPEL_UNDEAD])
        mpr("You have a holy aura protecting you from undead.");

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
        mpr("You are infected with a shuggoth parasite.");

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

    if (you.rotting || you.species == SP_GHOUL)
    {
        // I apologize in advance for the horrendous ugliness about to
        // transpire.  Avert your eyes!
        strcpy(info, "Your flesh is rotting");
        strcat(info, (you.rotting > 15) ? " before your eyes!":
                     (you.rotting > 8)  ? " away quickly.":
                     (you.rotting > 4)  ? " badly."
                                        :
                     ((you.species == SP_GHOUL && you.rotting > 0)
                        ?" faster than usual.":"."));
        mpr(info);
    }

    contaminate_player( 0, true );

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
        // this is hackish {dlb}
        if (player_equip_special( EQ_CLOAK, SPARM_PRESERVATION ))
            return true;
    }

    if (you.equip[EQ_AMULET] == -1)
        return false;

    if (you.inv[you.equip[EQ_AMULET]].sub_type == amulet)
        return true;

    return false;
}                               // end wearing_amulet()

bool player_has_spell( int spell )
{
    for (int i = 0; i < 25; i++)
    {
        if (you.spells[i] == spell)
            return (true);
    }

    return (false);
}

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
    int ret = 0;

    if (which_affected == PWPN_HIT)
    {
        ret += player_equip( EQ_RINGS_PLUS, RING_SLAYING );
        ret += scan_randarts(RAP_ACCURACY);
    }
    else if (which_affected == PWPN_DAMAGE)
    {
        ret += player_equip( EQ_RINGS_PLUS2, RING_SLAYING );
        ret += scan_randarts(RAP_DAMAGE);
    }

    return (ret);
}                               // end slaying_bonus()

/* Checks each equip slot for a randart, and adds up all of those with
   a given property. Slow if any randarts are worn, so avoid where possible. */
int scan_randarts(char which_property)
{
    int i = 0;
    int retval = 0;

    for (i = EQ_WEAPON; i < NUM_EQUIP; i++)
    {
        const int eq = you.equip[i];

        if (eq == -1)
            continue;

        // only weapons give their effects when in our hands
        if (i == EQ_WEAPON && you.inv[ eq ].base_type != OBJ_WEAPONS)
            continue;

        if (!is_random_artefact( you.inv[ eq ] ))
            continue;

        retval += randart_wpn_property( you.inv[ eq ], which_property );
    }

    return (retval);
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
            strcat(info, (amount > 0) ? "stronger." : "weaker.");
        break;

    case STAT_DEXTERITY:
        ptr_stat = &you.dex;
        ptr_stat_max = &you.max_dex;
        ptr_redraw = &you.redraw_dexterity;
        if (!suppress_msg)
            strcat(info, (amount > 0) ? "agile." : "clumsy.");
        break;

    case STAT_INTELLIGENCE:
        ptr_stat = &you.intel;
        ptr_stat_max = &you.max_intel;
        ptr_redraw = &you.redraw_intelligence;
        if (!suppress_msg)
            strcat(info, (amount > 0) ? "clever." : "stupid.");
        break;
    }

    if (!suppress_msg)
        mpr( info, (amount > 0) ? MSGCH_INTRINSIC_GAIN : MSGCH_WARN );

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
    if (you.hp < minimum + 1)
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

// Note that "max_too" refers to the base potential, the actual
// resulting max value is subject to penalties, bonuses, and scalings.
void inc_mp(int mp_gain, bool max_too)
{
    if (mp_gain < 1)
        return;

    you.magic_points += mp_gain;

    if (max_too)
        inc_max_mp( mp_gain );

    if (you.magic_points > you.max_magic_points)
        you.magic_points = you.max_magic_points;

    you.redraw_magic_points = 1;

    return;
}                               // end inc_mp()

// Note that "max_too" refers to the base potential, the actual
// resulting max value is subject to penalties, bonuses, and scalings.
void inc_hp(int hp_gain, bool max_too)
{
    if (hp_gain < 1)
        return;

    you.hp += hp_gain;

    if (max_too)
        inc_max_hp( hp_gain );

    if (you.hp > you.hp_max)
        you.hp = you.hp_max;

    you.redraw_hit_points = 1;
}                               // end inc_hp()

void rot_hp( int hp_loss )
{
    you.base_hp -= hp_loss;
    calc_hp();

    you.redraw_hit_points = 1;
}

void unrot_hp( int hp_recovered )
{
    if (hp_recovered >= 5000 - you.base_hp)
        you.base_hp = 5000;
    else
        you.base_hp += hp_recovered;

    calc_hp();

    you.redraw_hit_points = 1;
}

int player_rotted( void )
{
    return (5000 - you.base_hp);
}

void inc_max_hp( int hp_gain )
{
    you.base_hp2 += hp_gain;
    calc_hp();

    you.redraw_hit_points = 1;
}

void dec_max_hp( int hp_loss )
{
    you.base_hp2 -= hp_loss;
    calc_hp();

    you.redraw_hit_points = 1;
}

void inc_max_mp( int mp_gain )
{
    you.base_magic_points2 += mp_gain;
    calc_mp();

    you.redraw_magic_points = 1;
}

void dec_max_mp( int mp_loss )
{
    you.base_magic_points2 -= mp_loss;
    calc_mp();

    you.redraw_magic_points = 1;
}

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

// Note that "max_too" refers to the base potential, the actual
// resulting max value is subject to penalties, bonuses, and scalings.
void set_hp(int new_amount, bool max_too)
{
    if (you.hp != new_amount)
        you.hp = new_amount;

    if (max_too && you.hp_max != new_amount)
    {
        you.base_hp2 = 5000 + new_amount;
        calc_hp();
    }

    if (you.hp > you.hp_max)
        you.hp = you.hp_max;

    // must remain outside conditional, given code usage {dlb}
    you.redraw_hit_points = 1;

    return;
}                               // end set_hp()

// Note that "max_too" refers to the base potential, the actual
// resulting max value is subject to penalties, bonuses, and scalings.
void set_mp(int new_amount, bool max_too)
{
    if (you.magic_points != new_amount)
        you.magic_points = new_amount;

    if (max_too && you.max_magic_points != new_amount)
    {
        // note that this gets scaled down for values > 18
        you.base_magic_points2 = 5000 + new_amount;
        calc_mp();
    }

    if (you.magic_points > you.max_magic_points)
        you.magic_points = you.max_magic_points;

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
        return "Chaos Knight";
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
    // was this further differentiated? {dlb}
    if (player_genus(GENPC_DRACONIAN, which_species))
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
    default:                    return "XX";
    }
}                               // end species_abbrev()

char *class_abbrev( unsigned char which_class )
{
    switch (which_class)
    {
    case JOB_AIR_ELEMENTALIST:    return "AE";
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
void contaminate_player(int change, bool statusOnly)
{
    // get current contamination level
    int old_level;
    int new_level;


#if DEBUG_DIAGNOSTICS
    if (change > 0 || (change < 0 && you.magic_contamination))
    {
        snprintf( info, INFO_SIZE, "change: %d  radiation: %d",
                 change, change + you.magic_contamination );

        mpr( info );
    }
#endif

    old_level = (you.magic_contamination > 60)?(you.magic_contamination / 20 + 2) :
                (you.magic_contamination > 40)?4 :
                (you.magic_contamination > 25)?3 :
                (you.magic_contamination > 15)?2 :
                (you.magic_contamination > 5)?1  : 0;

    // make the change
    if (change + you.magic_contamination < 0)
        you.magic_contamination = 0;
    else
    {
        if (change + you.magic_contamination > 250)
            you.magic_contamination = 250;
        else
            you.magic_contamination += change;
    }

    // figure out new level
    new_level = (you.magic_contamination > 60)?(you.magic_contamination / 20 + 2) :
                (you.magic_contamination > 40)?4 :
                (you.magic_contamination > 25)?3 :
                (you.magic_contamination > 15)?2 :
                (you.magic_contamination > 5)?1  : 0;

    if (statusOnly)
    {
        if (new_level > 0)
        {
            if (new_level > 3)
            {
                strcpy(info, (new_level == 4) ?
                    "Your entire body has taken on an eerie glow!" :
                    "You are engulfed in a nimbus of crackling magics!");
            }
            else
            {
                snprintf( info, INFO_SIZE, "You are %s with residual magics%c",
                    (new_level == 3) ? "practically glowing" :
                    (new_level == 2) ? "heavily infused"
                                     : "contaminated",
                    (new_level == 3) ? '!' : '.');
            }

            mpr(info);
        }
        return;
    }

    if (new_level == old_level)
        return;

    snprintf( info, INFO_SIZE, "You feel %s contaminated with magical energies.",
        (change < 0)?"less":"more");
    mpr(info, change>0?MSGCH_WARN:MSGCH_RECOVERY);
}

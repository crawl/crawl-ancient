/*
 *  File:       unrand.cc
 *  Summary:    Definitions for unrandom artifacts.
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *               <3>     7 Aug 2001      MV             Added many new items
 *               <2>     5/09/99        JDJ             Cekugob no longer has fire and cold
 *                                                                      resistances.
 *               <1>     -/--/--        LRH             Created
 */
#ifndef UNRAND_H
#define UNRAND_H

#include "defines.h"

/*
   List of "unrandom" artefacts. Not the same as "fixed" artefacts, which are
   completely hardcoded (eg Singing Sword, Wrath of Trog).
   note: the order of the list doesn't matter
   Because the list numbering starts at 1, the last entry is the highest value
   which can be given to NO_UNRANDARTS (eg if the list consists of randarts no
   1, 2 or 3, NO_UNRANDARTS must be set to 3 or lower, but probably not to 0).
   Setting it higher could cause nasty problems.

   Okay, so the steps to adding a new unrandart go as follows:
   1) - Fill in a new entry below, using the following guidelines:
   true name: The name which is displayed when the item is id'd
   un-id'd name: obvious
   class: weapon, armour etc
   type: long sword, plate mail etc. Jewellery unrandarts have the powers of
   their base types in addition to anything else.
   plus: For weapons, plus to-hit. For armour, plus. For jewellery, irrelevant.
   But add 100 to make the item stickycursed. Note that the values for
   wpns and armr are +50.
   plus2: For wpns, plus to-dam. Curses are irrelevant here. Mostly unused
   for armr and totally for rings. Armour: if boots, plus2 == 1 means
   naga barding; 2 means centaur barding. If headgear, 1 means helmet,
   2 means helm, 3 means cap, 4 means wizard's hat.
   colour: Obvious. Don't use BLACK, use DARKGREY instead.

   * Note * any exact combination of class, type, plus & plus2 must be unique,
   so (for example) you can't have two +5, +5 long swords in the list. Curses
   don't count as distinguishing factors.

   brand: Weapons only. Have a look in enum.h for a list, and look in fight.cc
   and describe.cc for the effects.
   Range of possible values: see enum.h

   +/- to AC, ev, str, int, dex - These are pretty obvious. Be careful - a player
   with a negative str, int or dex dies instantly, so avoid high penalties
   to these stats.
   Range: any, but be careful.

   res fire, res cold: Resists. Can be above 1; multiple sources of fire or cold
   resist *are* cumulative. Can also be -1 (but probably not -2 or below)
   for susceptibility.
   Range: -1 to about 5, after which you'll become almost immune.
   res elec: Resist electricity. Unlike fire and cold, resist electricity
   reduces electrical damage to 0. This makes multiple resists irrelevant.
   Also is no susceptibility, so don't use -1.
   Range: 0 or 1.
   res poison: same as res electricity.
   Range: 0 or 1.
   life prot: Stops energy draining and negative energy attacks. Not cumulative,
   and no susceptibility here either.
   Range: 0 or 1.
   res magic: This is cumulative, but no susceptibility. To be meaningful, should
   be set to about 30 - 50.
   Range: 0 to MAXINT probably, but about 100 is a realistic ceiling.

   see invis: Lets you see invisible things, but not submerged water beasts.
   Range: 0 or 1.
   turn invis: Gives you the ability to turn invisible using the 'a' menu.
   levitate, blink, go berserk, sense surroundings: like turn invis.
   Ranges for all these: 0 or 1.

   make noise: Irritate nearby creatures and disrupts rest. Weapons only.
   Range: 1 - 4, for different types of noises (see special_wielded() in
   it_use3.cc); 0 for none.
   no spells: Prevents any spellcasting (but not scrolls or wands etc)
   Range: 0 or 1.
   teleport: Every now and then randomly teleports you. *Really* annoying.
   Weapons only.
   Range: 0 to about 15 (higher means more teleporting).
   no teleport: Prevents the player from teleporting, except rarely when they're
   forced to (eg banished to the Abyss).
   Range: 0 or 1.

   force berserk: Every time you attack, you go berserk. Weapons only.
   Range: 0 or 1.
   speed metabolism: Makes you consume food faster. No effect on mummies.
   Range: 0 to about 4. 4 would be horrible; 1 is annoying but tolerable.
   mutate: makes you mutate, sometimes after a long delay. No effect on some
   races (espec undead).
   Range: 0 to about 4.
   +/- to-hit/to-dam: Obvious. Affects both melee and missile. Should be left
   at 0 for weapons, which get +s normally.
   cursed: 0 or 1.  Sets the item's initial curse status.

   Some currently unused properties follow, then:

   First string: is appended to the unrandart's 'V' description when id'd.

   Second string: replaces the thing at the start of a 'V' description.
   If empty, uses the description of the unrandart's base type. Note: the
   base type of a piece of unrandart jewellery is relevant to its function, so
   don't obscure it unnecessarily.

   Then another unused string.

   2) - Add one to the #define NO_UNRANDARTS line in randart.h

   3) - Maybe increase the probability of an unrandart of the appropriate
   type being created; look in dungeon.cc for this (search for "unrand").
   Forget this step if you don't understand it; it's of very little importance.

   Done! Now recompile and wait years for it to turn up.

   Note: changing NO_UNRANDARTS probably makes savefiles incompatible.

 */

/* This is a dummy, but still counts for NO_UNRANDARTS */
/* 1 */
{
    "Dum", "",
/* class, type, plus (to-hit), plus2 (depends on class), colour */
        250, 250, 250, 250, 0,
/* Properties, all approx thirty of them: */
    {
/* brand, +/- to AC, +/- to ev, +/- to str, +/- to int, +/- to dex */
        0, 0, 0, 0, 0, 0,
/* res fire, res cold, res elec, res poison, life protection, res magic */
        0, 0, 0, 0, 0, 0,
/* see invis, turn invis, levitate, blink, teleport at will, go berserk */
        0, 0, 0, 0, 0, 0,
/* sense surroundings, make noise, no spells, teleport, no teleprt */
        0, 0, 0, 0, 0,
/* force berserk, speed metabolism, mutate, +/- to hit, +/- to dam (not weapons) */
        0, 0, 0, 0, 0,
/* cursed, unused */
        0, 0
    }
    ,
/* Special description appended to the 'V' description */
        "",
/* Base description of item */
        "",
/* Unused string */
        ""
}
,


/* 2 */
{                               /* true name: */
    "long sword \"Bloodbane\"", /* un-id'd name */ "blackened long sword",
/* class, type, plus (to-hit), plus2 (depends on class), colour */
        OBJ_WEAPONS, WPN_LONG_SWORD, +7, +8, DARKGREY,
/* Properties, all approx thirty of them: */
    {
/* brand, +/- to AC, +/- to ev, +/- to str, +/- to int, +/- to dex */
        SPWPN_VORPAL, 0, 0, 0, 0, 0,
/* res fire, res cold, res elec, res poison, life protection, res magic */
        0, 0, 0, 0, 0, 0,
/* see invis, turn invis, levitate, blink, teleport at will, go berserk */
        0, 0, 0, 0, 0, 1,
/* sense surroundings, make noise, no spells, teleport, no teleprt */
        0, 0, 0, 0, 0,
/* force berserk, speed metabolism, mutate, +/- to hit, +/- to dam (not weapons) */
        1, 0, 0, 0, 0,
/* some as yet unused properties */
        0, 0
    }
    ,
/* 3 strings for describing the item in the 'V' display. */
        "",
        "",
        ""
}
,

/* 3 */
{                               /* true name: */
    "ring of Shadows", /* un-id'd name */ "black ring",
/* class, type, plus (to-hit), plus2 (depends on class), colour */
        OBJ_JEWELLERY, RING_INVISIBILITY, 0, 0, DARKGREY,
/* Properties, all approx thirty of them: */
    {
/* brand, +/- to AC, +/- to ev, +/- to str, +/- to int, +/- to dex */
        0, 0, 4, 0, 0, 0,
/* res fire, res cold, res elec, res poison, life protection, res magic */
        0, 0, 0, 0, 1, 0,
/* see invis, turn invis, levitate, blink, teleport at will, go berserk */
        1, 0, 0, 0, 0, 0,
/* sense surroundings, make noise, no spells, teleport, no teleprt */
        0, 0, 0, 0, 0,
/* force berserk, speed metabolism, mutate, +/- to hit, +/- to dam (not weapons) */
        0, 0, 0, -3, 0,
/* some as yet unused properties */
        0, 0
    }
    ,
/* 3 strings for describing the item in the 'V' display. */
        "",
        "",
        ""
}
,

/* 4 */
{
    "long sword of Flaming Death", "smoking long sword",
        OBJ_WEAPONS, WPN_LONG_SWORD, +6, +2, RED,
    {
        SPWPN_FLAMING, 0, 0, 0, 0, 0,
        2, -1, 0, 2, 0, 2,
        0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0,
        0, 0, 0, 0, 0,
        0, 0
    }
    ,
        "",
        "",
        ""
}
,

/* 5 */
{
    "shield of Ignorance", "dull large shield",
        OBJ_ARMOUR, ARM_LARGE_SHIELD, +5, 0, BROWN,
    {
        0, 2, 2, 0, -6, 0,
        0, 0, 0, 0, 1, 0,
        0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0,
        0, 0, 0, 0, 0,
        1, 0
    }
    ,
        "",
        "",
        ""
}
,


/* 6 */
{
    "Holy Armour of Zin", "glowing golden plate mail",
        OBJ_ARMOUR, ARM_PLATE_MAIL, +6, 0, YELLOW,
    {
        0, 0, 0, 3, 0, 0,
        0, 0, 0, 0, 1, 50,
        0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0,
        0, 0, 0, 0, 0,
        0, 0
    }
    ,
        "",
        "A suit of mail and large plates of golden metal.",
        ""
}
,

/* 7 */
{
    "robe of Augmentation", "silk robe",
        OBJ_ARMOUR, ARM_ROBE, +0, 0, LIGHTRED,
    {
        0, 0, 0, 2, 2, 2,
        0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0,
        0, 0, 0, 0, 0,
        0, 0
    }
    ,
        "",
        "A robe made of the finest silk.",
        ""
}
,

/* 8 */
{
    "mace of Brilliance", "brightly glowing mace",
        OBJ_WEAPONS, WPN_MACE, +5, +5, WHITE,
    {
        SPWPN_HOLY_WRATH, 3, 0, 0, 0, 0,
        0, 0, 0, 0, 1, 0,
        1, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0,
        0, 0, 0, 0, 0,
        0, 0
    }
    ,
        "",
        "",
        ""
}
,

/* 9 */
{
    "cloak of the Thief", "tattered cloak",
        OBJ_ARMOUR, ARM_CLOAK, +1, 0, DARKGREY,
    {
        0, 0, 2, 0, 0, 2,
        0, 0, 0, 0, 0, 0,
        1, 1, 0, 0, 0, 0,
        0, 0, 0, 0, 0,
        0, 0, 0, 0, -3,
        0, 0
    }
    ,
        "It allows its wearer to excell in the arts of thievery.",
        "",
        ""
}
,




/* 10 */
{
    "buckler \"Bullseye\"", "round buckler",
        OBJ_ARMOUR, ARM_BUCKLER, +10, 0, RED,
    {
        0, 0, -3, 0, 0, 0,
        0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0,
        0, 0, 0, 0, 0,
        0, 0
    }
    ,
        "",
        "",
        ""
}
,

/* 11 */
{
    "crown of Dyrovepreva", "jewelled bronze crown",
        OBJ_ARMOUR, ARM_HELMET, +3, THELM_SPECIAL, BROWN,
    {
        0, 0, 0, 0, 2, 0,
        0, 0, 1, 0, 0, 0,
        1, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0,
        0, 1, 0, 0, 0,
        0, 0
    }
    ,
        "",
        "A large crown of dull bronze, set with a dazzling array of gemstones.",
        ""
}
,


/* 12 */
{
    "demon blade \"Leech\"", "runed demon blade",
        OBJ_WEAPONS, WPN_DEMON_BLADE, +13, +4, MAGENTA,
    {
        SPWPN_VAMPIRICISM, -1, -1, -1, -1, -1,
        0, 0, 0, 0, 1, 0,
        0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0,
        0, 0, 0, 0, 0,
        1, 0
    }
    ,
        "",
        "",
        ""
}
,

/* 13 */
{
    "amulet of Cekugob", "crystal amulet",
        OBJ_JEWELLERY, AMU_WARDING, +0, 0, LIGHTGREY,
    {
        0, 1, 1, 0, 0, 0,
        0, 0, 1, 1, 1, 0,
        0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 1,
        0, 2, 0, 0, 0,
        0, 0
    }
    ,
        "",
        "",
        ""
}
,


/* 14 */
{
    "robe of misfortune", "fabulously ornate robe",
        OBJ_ARMOUR, ARM_ROBE, -5, 0, MAGENTA,
    {
        0, 0, -4, -2, -2, -2,
        0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0,
        0, 0, 1, 1, 0,
        0, 0, 5, 0, 0,
        1, 0
    }
    ,
        "",
        "A splendid flowing robe of fur and silk.",
        ""
}

#ifdef USE_NEW_UNRANDS
,
/* 15 */
{
    "dagger of Chilly Death", "saphire dagger",
        OBJ_WEAPONS, WPN_DAGGER, +2, +5, LIGHTBLUE,
    {
        SPWPN_FREEZING, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0,
        0, 0, 0, 0, 0,
        0, 0
    }
    ,
        "",
        "A dagger made of one huge piece of saphire.",
        ""
}
,
/* 16 */
{
    "amulet of Four Winds", "jade amulet",
        OBJ_JEWELLERY, AMU_CLARITY, +0, 0, LIGHTGREEN,
    {
        0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 1, 1,
        0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0,
        0, 0, 0, 0, 0,
        0, 0
    }
    ,
        "",
        "",
        ""
}
,

/* 17 */
{
    "dagger \"Morg\"", "rusty dagger",
        OBJ_WEAPONS, WPN_DAGGER, -1, +4, LIGHTRED,
    {
        SPWPN_PAIN, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0,
        0, 0, 0, 0, 0,
        0, 0
    }
    ,
        "Many years ago it was property of powerful mage called "
        "Boris. He got lost in the Dungeon while seeking some kind of orb. ",
        "An ugly rusty dagger. ",
        ""
}
,

/* 18 */
{
    "scythe \"Finisher\"", "blackened scythe",
        OBJ_WEAPONS, WPN_SCYTHE, +3, +5, DARKGRAY,
    {
        0, 0, 0, 3, 0, 0,
        0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0,
        0, 0, 0, 0, 0,
        1, 0
    }
    ,
        "",
        "A long and sharp scythe, specially modified for combat purposes.",
        ""
}
,

/* 19 */
{
    "sling \"Punk\"", "blue sling",
        OBJ_WEAPONS, WPN_SLING, +3, +4, LIGHTBLUE,
    {
        SPWPN_FROST, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0,
        0, 0, 0, 0, 0,
        0, 0
    }
    ,
        "",
        "A sling made of weird blue leather.",
        ""
}
,
/* 20 */
{
    "Krishna's bow", "golden bow",
        OBJ_WEAPONS, WPN_BOW, +8, +8, YELLOW,
    {
        0, 0, 0, 0, 0, 3,
        0, 0, 0, 0, 0, 0,
        1, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0,
        0, 0, 0, 0, 0,
        0, 0
    }
    ,
        "It once belonged to one foreign god. It works best with some kind "
        "of special arrows which are not generally available.",
        "A wonderful golden bow. ",
        ""
}
,
/* 21 */
{
    "cloak of Flash", "vibrating cloak",
        OBJ_ARMOUR, ARM_CLOAK, +2, 0, RED,
    {
        0, 0, 4, 0, 0, 0,
        0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 1, 0,
        0, 0, 0, 0, 0,
        0, 0, 0, 0, 0,
        0, 0
    }
    ,
        "",
        "A vibrating cloak.",
        ""
}
,
/* 22 */
{
    "giant club \"Skullcrusher\"", "brutal giant club",
        OBJ_WEAPONS, WPN_GIANT_CLUB, +0, +5, BROWN,
    {
        0, 0, 0, 5, 0, 0,
        0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0,
        0, 0, 0, 0, 0,
        0, 0
    }
    ,
        "",
        "",
        ""
}
,
/* 23 */
{
    "boots of Assassin", "soft boots",
        OBJ_ARMOUR, ARM_BOOTS, +2, 0, BROWN,
    {
        0, 0, 0, 0, 0, 3,
        0, 0, 0, 0, 0, 0,
        0, 1, 0, 0, 0, 0,
        0, 0, 0, 0, 0,
        0, 0, 0, 0, 0,
        0, 0
    }
    ,
        "These boots were specially designed by Assasin's Guild.",
        "A soft boots.",
        ""
}
,
/* 24 */
{
    "glaive of the Guard", "polished glaive",
        OBJ_WEAPONS, WPN_GLAIVE, +5, +8, LIGHTCYAN,
    {
        SPWPN_PROTECTION, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 1,
        0, 0, 0, 0, 0,
        0, 0, 0, 0, 0,
        0, 0
     }
    ,
        "This weapon once belonged to Gar Dogh the guard of king's treasures. "
        "According to legend he was lost somewhere in the Dungeon.",
        "",
        ""
}
,
/* 25 */
{
    "sword of Jihad", "crystal sword",
        OBJ_WEAPONS, WPN_LONG_SWORD, +4, +4, WHITE,
    {
        SPWPN_HOLY_WRATH, 0, 3, 0, 0, 0,
        0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0,
        1, 0, 0, 0, 0,
        0, 0
    }
    ,
        "This sword was The Shining One's gift to one of his paladins." ,
        "A long sword made of one huge piece of crystal.",
        ""
}
,
/* 26 */
{
    "Lear's chain mail", "golden chain mail",
        OBJ_ARMOUR, ARM_CHAIN_MAIL, -1, 0, YELLOW,
    {
        0, 0, 0, 0, 0, -3,
        0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0,
        0, 0, 1, 0, 0,
        0, 0, 0, 0, 0,
        1, 0
    }
    ,
        "",
        "A chain mail made of pure gold.",
        ""
}
,
/* 27 */
{
    "skin of Zhor", "smelly skin",
        OBJ_ARMOUR, ARM_ANIMAL_SKIN, +3, 0, BROWN,
    {
        0, 0, 0, 0, 0, 0,
        0, 1, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0,
        0, 0, 0, 0, 0,
        0, 0
    }
    ,
        "",
        "A skin of some strange animal.",
        ""
}
,
/* 28 */
{
    "crossbow \"Fiery Devil\"", "flaming crossbow",
        OBJ_WEAPONS, WPN_CROSSBOW, +4, +0, LIGHTRED,
    {
        SPWPN_FLAMING, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0,
        0, 0, 0, 0, 0,
        0, 0
    }
    ,
        "",
        "A flaming crossbow.",
        ""
}
,
/* 29 */
{
    "salamander's hide armour", "red leather armour",
        OBJ_ARMOUR, ARM_LEATHER_ARMOUR, +2, 0, RED,
    {
        0, 0, 0, 0, 0, 0,
        2, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0,
        0, 0, 0, 0, 0,
        0, 0
    }
    ,
        "",
        "A leather armour made of salamander's skin.",
        ""
}
,
/* 30 */
{
    "gauntlets of War", "thick gauntlets",
        OBJ_ARMOUR, ARM_GLOVES, +3, 0, BROWN,
    {
        0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0,
        0, 0, 0, 3, 3,
        0, 0
    }
    ,
        "",
        "",
        ""
}
,
/* 31 */
{
    "sword of Doom Knight", "adamantine great sword",
        OBJ_WEAPONS, WPN_GREAT_SWORD, +4, +4, BLUE,
    {
        SPWPN_PAIN, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 1,
        0, 0, 0, 0, 0, 0,
        0, 0, 1, 0, 0,
        0, 0, 0, 0, 0,
        0, 0
    }
    ,
        "",
        "An adamantine great sword.",
        ""
}
,
/* 32 */
{
    "shield of resistance", "bronze shield",
        OBJ_ARMOUR, ARM_SHIELD, +3, 0, LIGHTRED,
    {
        0, 0, 0, 0, 0, 0,
        1, 1, 0, 0, 0, 40,
        0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0,
        0, 0, 0, 0, 0,
        0, 0
    }
    ,
        "",
        "A bronze shield.",
        ""
}
,
/* 33 */
{
    "robe of folly", "foolish robe",
        OBJ_ARMOUR, ARM_ROBE, -1, 0, LIGHTGRAY,
    {
        0, 0, 0, 0, -5, 0,
        0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0,
        0, 0, 1, 0, 0,
        0, 0, 0, 0, 0,
        1, 0
    }
    ,
        "",
        "A dull gray robe.",
        ""
}
,
/* 34 */
{
    "necklace of Bloodlust", "blood-stained necklace",
        OBJ_JEWELLERY, AMU_RAGE, +0, 0, RED,
    {
        0, 0, 0, 2, -2, 0,
        0, 0, 0, 0, 0, 20,
        0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0,
        1, 0, 0, 0, 3,
        1, 0
    }
    ,
        "",
        "",
        ""
}
,
/* 35 */
{
    "\"Eos\"", "encrusted morningstar",
        OBJ_WEAPONS, WPN_MORNINGSTAR, +5, +5, LIGHTCYAN,
    {
        0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 1,
        0, 0, 0, 0, 0,
        0, 0
    }
    ,
        "",
        "",
        ""
}
,
/* 36 */
{
    "ring of Shaolin", "jade ring",
        OBJ_JEWELLERY, RING_EVASION, +8, 0, LIGHTGREEN,
    {
        0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0,
        0, 0, 0, 0, 0,
        0, 0
    }
    ,
        "",
        "",
        ""
}
,
/* 37 */
{
    "ring of robustness", "steel ring",
        OBJ_JEWELLERY, RING_PROTECTION, +8, 0, LIGHTGRAY,
    {
        0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0,
        0, 0, 0, 0, 0,
        0, 0
    }
    ,
        "",
        "",
        ""
}
,
/* 38 */
{
    "Edison's patent armour", "weird-looking armour",
        OBJ_ARMOUR, ARM_PLATE_MAIL, +0, 0, LIGHTGREEN,
    {
        0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0,
        0, 0, 1, 0, 1,
        0, 0, 0, 0, 0,
        1, 0
    }
    ,
        "",
        "A weird-looking armour.",
        ""
}
,
/* 39 */
{
    "spear of Voo-Doo", "ebony spear",
        OBJ_WEAPONS, WPN_SPEAR, +2, +4, DARKGRAY,
    {
        SPWPN_VAMPIRICISM, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 1, 0,
        0, 0, 0, 0, 0, 0,
        0, 3, 0, 0, 0,
        0, 0, 0, 0, 0,
        0, 0
    }
    ,
        "It's really dark and malign artifact and no wise man would even touch it.",
        "",
        ""
}
,
/* 40 */
{
    "trident of Octopus's king", "mangy trident",
        OBJ_WEAPONS, WPN_TRIDENT, +4, +4, CYAN,
    {
        SPWPN_VENOM, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0,
        0, 0, 0, 0, 0,
        0, 0
    }
    ,
        "This trident was stolen many years ago from Octopus's garden "
        "by one really unimportant and already dead man. But beware of "
        "Octopus's king's wrath!",
        "",
        ""
}
,
/* 41 */
{
    "mask of dragon", "blue mask",
        OBJ_ARMOUR, ARM_HELMET, +0, THELM_SPECIAL, BLUE,
    {
        0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0,
        0, 0, 0, 2, 2,
        0, 0
    }
    ,
        "",
        "A blue mask.",
        ""
}
,
/* 42 */
{
    "mithril axe \"Arga\"", "mithril axe",
        OBJ_WEAPONS, WPN_WAR_AXE, +3, +3, WHITE,
    {
        SPWPN_SPEED, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0,
        0, 0, 0, 0, 0,
        0, 0
    }
    ,
        "",
        "A beautiful mithril axe, probably lost by some dwarven hero.",
        ""
}
,
/* 43 */
{
    "elemental staff", "black staff",
        OBJ_WEAPONS, WPN_QUARTERSTAFF, +3, +1, DARKGRAY,
    {
        SPWPN_PROTECTION, 0, 0, 0, 0, 0,
        1, 1, 0, 0, 0, 60,
        0, 0, 0, 0, 0, 0,
        0, 1, 0, 0, 0,
        0, 0, 0, 0, 0,
        0, 0
    }
    ,
        "This powerful staff used to belong to leader of"
        " the Guild of Five Elements.",
        "A black glyphic staff.",
        ""
}
,
/* 44 */
{
    "hand crossbow \"Sniper\"", "black crossbow",
        OBJ_WEAPONS, WPN_HAND_CROSSBOW, +8, +0, DARKGRAY,
    {
        0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0,
        1, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0,
        0, 0, 0, 0, 0,
        0, 0
    }
    ,
        "",
        "A hand crossbow made of some black material.",
        ""
}
,
/* 45 */
{
    "bow \"Erchidel\"", "metal bow",
        OBJ_WEAPONS, WPN_BOW, +5, +3, CYAN,
    {
        0, 0, 0, 3, 0, 0,
        0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0,
        0, 0, 0, 0, 0,
        0, 0
    }
    ,
        "",
        "A metal bow.",
        ""
}
,
/* 46 */
{
    "robe of Night", "black robe",
        OBJ_ARMOUR, ARM_ROBE, +3, 0, DARKGRAY,
    {
        0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 20,
        1, 1, 0, 0, 0, 0,
        0, 0, 0, 0, 0,
        0, 0, 0, 0, 0,
        0, 0
    }
    ,
        "According to legend this robe was gift of Ratri the Goddess of the Night "
        "to one her follower.",
        "A long black robe made of strange flossy material.",
        ""
}
,
/* 47 */
{
    "plutonium sword", "glowing long sword",
        OBJ_WEAPONS, WPN_LONG_SWORD, +5, +7, LIGHTGREEN,
    {
        0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0,
        0, 0, 6, 0, 0,
        1, 0
    }
    ,
        "",
        "A long sword made of weird glowing metal.",
        ""
}
,
/* 48 */
{
    "mace \"Undeadhunter\"", "steel mace",
        OBJ_WEAPONS, WPN_MACE, +4, +6, LIGHTGRAY,
    {
        SPWPN_DISRUPTION, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0,
        0, 0, 0, 0, 0,
        0, 0
    }
    ,
        "",
        "",
        ""
}
,
/* 49 */
{
    "armour of the Dragon King", "shiny dragon armour",
        OBJ_ARMOUR, ARM_GOLD_DRAGON_ARMOUR, +5, 0, YELLOW,
    {
        0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 25,
        0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0,
        0, 0, 0, 0, 0,
        0, 0
    }
    ,
        "",
        "",
        ""
}
,
/* 50 */
{
    "hat of Alchemist", "dirty hat",
        OBJ_ARMOUR, ARM_HELMET, +2, THELM_SPECIAL, MAGENTA,
    {
        0, 0, 0, 0, 0, 0,
        1, 1, 1, 0, 0, 20,
        0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0,
        0, 0, 0, 0, 0,
        0, 0
    }
    ,
        "",
        "A dirty hat.",
        ""
}
,
/* 51 */
{
    "Fencer's gloves", "silk gloves",
        OBJ_ARMOUR, ARM_GLOVES, +0, 0, WHITE,
    {
        0, 0, 3, 0, 0, 0,
        0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0,
        0, 0, 0, 5, 0,
        0, 0
    }
    ,
        "",
        "A gloves made of white silk.",
        ""
}
,
/* 52 */
{
    "ring of Mage", "saphire ring",
        OBJ_JEWELLERY, RING_WIZARDRY, +0, 0, LIGHTBLUE,
    {
        0, 0, 0, 0, 3, 0,
        0, 0, 0, 0, 0, 50,
        0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0,
        0, 0, 0, 0, 0,
        0, 0
     }
    ,
        "",
        "",
        ""
}
#endif // USE_NEW_UNRANDS
,
/* This is a dummy */
/* 1 */
{
    "Dum", "",
/* class, type, plus (to-hit), plus2 (depends on class), colour */
        250, 250, 250, 250, 0,
/* Properties, all approx thirty of them: */
    {
/* brand, +/- to AC, +/- to ev, +/- to str, +/- to int, +/- to dex */
        0, 0, 0, 0, 0, 0,
/* res fire, res cold, res elec, res poison, life protection, res magic */
            0, 0, 0, 0, 0, 0,
/* see invis, turn invis, levitate, blink, teleport at will, go berserk */
            0, 0, 0, 0, 0, 0,
/* sense surroundings, make noise, no spells, teleport, no teleprt */
            0, 0, 0, 0, 0,
/* force berserk, speed metabolism, mutate, +/- to hit, +/- to dam (not weapons) */
            0, 0, 0, 0, 0,
/* some as yet unused properties */
            0, 0
    }
    ,
/* 3 strings for describing the item in the 'V' display. */
        "",
        "",
        ""
}
#endif

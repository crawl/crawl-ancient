/*
 *  File:       skills2.cc
 *  Summary:    More skill related functions.
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 *     01aug2000 jmf             RESTORED TITLES TO THEIR FORMER GLORY! MUA-HA!
 * <4> 22Jul2000 GDL             added warning for low throwing skill
 *                               Changed a few titles.
 * <3> 5/20/99   BWR             Changed Trapper titles, avoided
 *                               overflow on the weapon skill
 *                               column.
 * <2> -/--/--   WL              Extensive mods from Wladimir van der Laan.
 * <1> -/--/--   LRH             Created
 */

#include "AppHdr.h"
#include "skills2.h"

#include <stdio.h>
#include <stdlib.h>

#ifdef DOS
#include <conio.h>
#endif

#include "externs.h"
#include "fight.h"
#include "player.h"
#include "randart.h"
#include "wpn-misc.h"

//jmf: brent sez:
//  There's a reason curses is included after the *.h files in beam.cc.
//  There's a reason curses is included after the *.h files in beam.cc.
//  There's a reason curses is included after the *.h files in beam.cc.
//  There's a reason ...
#ifdef USE_CURSES
#include <curses.h>
#endif

//#define WIZARD

//jmf: some references for words I used below:
// Peltast http://www.geocities.com/Athens/Aegean/9659/shields_main.htm
// Phalangite http://www.users.cts.com/funtv/j/jjartist/EpiroteScenario1.htm
// Yeoman: http://snt.student.utwente.nl/campus/sagi/artikel/longbow/longbow.html

char *skills[50][5] = {
    {"Fighting", "Grunt", "Veteran", "Warrior", "Slayer"},      // 0
    {"Short Blades", "Stabber", "Cutter", "Slicer", "Knifefighter"},
    {"Long Blades", "Slasher", "Fencer", "Swordfighter", "Eviscerator"},
    {NULL},                     //  3- was: great swords {dlb}
    {"Axes", "Chopper", "Cleaver", "Hacker", "Axe Maniac"},
    {"Maces & Flails", "Basher", "Cruncher", "Smasher", "Shatterer"},   // 5
    {"Polearms", "Spear-Bearer", "Phalangite", "Lancer", "Halberdier"},
    {"Staves", "Twirler", "Cruncher", "Smasher", "Skullbreaker"},
    {"Slings", "Slinger", "Vandal", "Whirler", "Very Crazy Person"},
    {"Bows", "Shooter", "Yeoman", "Archer", "Merry Person"},
    {"Crossbows", "Shooter", "Sharpshooter", "Archer", "Ballista"},     // 10
    {"Darts", "Thrower", "Hurler", "Hurler, First Class", "Darts Champion"},
    {"Throwing", "Skirmisher", "Marksman", "Hawkeye", "Sniper"},
    {"Armour", "Grunt", "Heavy Grunt", "Tortoise", "Impregnable"},
    {"Dodging", "Ducker", "Dodger", "Nimble", "Acrobat"},
    {"Stealth", "Footpad", "Sneak", "Covert", "Ninjitsur"},
    {"Stabbing", "Backstabber", "Cutthroat", "Blackguard", "Politician"},
    {"Shields", "Shield-Bearer", "Blocker", "Peltast", "Hoplite"},
    {"Traps & Doors", "Disarmer", "Trapper", "Architect", "Engineer"},
//{"Unarmed Combat", "Brawler", "Martial Artist", "Black Belt", "Sensei"},// 19
    //jmf: remember that this applies to Troll Fighters as well as Kenku Monks:
    {"Unarmed Combat", "Brawler", "Boxer", "Martial Artist", "Black Belt"},
    {NULL},                     // 20- empty
    {NULL},                     // 21- empty
    {NULL},                     // 22- empty
    {NULL},                     // 23- empty
    {NULL},                     // 24- empty
    {"Spellcasting", "Magician", "Thaumaturge", "Eclecticist", "Archmage"},     // 25
    {"Conjurations", "Evoker", "Conjurer", "Destroyer", "Annihilator"},
    {"Enchantments", "Charm-Maker", "Enchanter", "Infuser", "Spellbinder"},
    {"Summonings", "Caller", "Summoner", "Demonologist", "Hellbinder"},
    {"Necromancy", "Grave Robber", "Necromancer", "Reanimator", "Death Mage"},
    {"Translocations", "Jumper", "Blinker", "Portalist", "Plane Walker"},       // 30
    {"Transmigration", "Changer", "Transformer", "Alchemist", "Transmuter"},
    {"Divinations", "Seer", "Soothsayer", "Diviner", "Oracle"},
    {"Fire Magic", "Firebug", "Arsonist", "Pyromaniac", "Infernalist"},
    {"Ice Magic", "Frost Mage", "Ice Mage", "Cryomancer", "Englaciator"},
    {"Air Magic", "Wind Mage", "Cloud Mage", "Sky Mage", "Storm Mage"}, // 35
    {"Earth Magic", "Digger", "Geomancer", "Petrifier", "Earth Mage"},
    {"Poison Magic", "Stinger", "Tainter", "Poisoner", "Venom Mage"},
    {"Invocations", "Believer", "Servant", "Worldly Agent", "Avatar"},  // 38

/*NOTE: If more skills are added, must change ranges in level_change() in player.cc */
/*{"",             "", "", "", ""}, */

    {NULL},                     // 39- empty
    {NULL},                     // 40- empty
    {NULL},                     // 41- empty
    {NULL},                     // 42- empty
    {NULL},                     // 43- empty
    {NULL},                     // 44- empty
    {NULL},                     // 45- empty
    {NULL},                     // 46- empty
    {NULL},                     // 47- empty
    {NULL},                     // 48- empty
    {NULL}                      // 49- empty  {end of array}
};


/* Note that this (humans have 100 for all skills) is assumed in the
   level_change function in player.cc, if CLASSES is def'd

   3.10: but it never is, and CLASSES is probably broken now. Anyway,
   the Spellcasting skill (25) is actually about 130% of what is shown here.
 */
int spec_skills[ NUM_SPECIES ][39] = {
    {                           // SP_HUMAN (1)
     100,                       // SK_FIGHTING
     100,                       // SK_SHORT_BLADES
     100,                       // SK_LONG_SWORDS
     100,                       // SK_UNUSED_1
     100,                       // SK_AXES
     100,                       // SK_MACES_FLAILS
     100,                       // SK_POLEARMS
     100,                       // SK_STAVES
     100,                       // SK_SLINGS
     100,                       // SK_BOWS
     100,                       // SK_CROSSBOWS
     100,                       // SK_DARTS
     100,                       // SK_THROWING
     100,                       // SK_ARMOUR
     100,                       // SK_DODGING
     100,                       // SK_STEALTH
     100,                       // SK_STABBING
     100,                       // SK_SHIELDS
     100,                       // SK_TRAPS_DOORS
     100,                       // SK_UNARMED_COMBAT
     100,                       // undefined
     100,                       // undefined
     100,                       // undefined
     100,                       // undefined
     100,                       // undefined
     100,                       // SK_SPELLCASTING
     100,                       // SK_CONJURATIONS
     100,                       // SK_ENCHANTMENTS
     100,                       // SK_SUMMONINGS
     100,                       // SK_NECROMANCY
     100,                       // SK_TRANSLOCATIONS
     100,                       // SK_TRANSMIGRATION
     100,                       // SK_DIVINATIONS
     100,                       // SK_FIRE_MAGIC
     100,                       // SK_ICE_MAGIC
     100,                       // SK_AIR_MAGIC
     100,                       // SK_EARTH_MAGIC
     100,                       // SK_POISON_MAGIC
     100,                       // SK_INVOCATIONS
     },

    {                           // SP_ELF (2)
     120,                       // SK_FIGHTING
     80,                        // SK_SHORT_BLADES
     80,                        // SK_LONG_SWORDS
     110,                       // SK_UNUSED_1
     120,                       // SK_AXES
     130,                       // SK_MACES_FLAILS
     130,                       // SK_POLEARMS
     100,                       // SK_STAVES
     120,                       // SK_SLINGS
     60,                        // SK_BOWS
     100,                       // SK_CROSSBOWS
     90,                        // SK_DARTS
     80,                        // SK_THROWING
     120,                       // SK_ARMOUR
     80,                        // SK_DODGING
     80,                        // SK_STEALTH
     100,                       // SK_STABBING
     120,                       // SK_SHIELDS
     100,                       // SK_TRAPS_DOORS
     110,                       // SK_UNARMED_COMBAT
     100,                       // undefined
     100,                       // undefined
     100,                       // undefined
     100,                       // undefined
     100,                       // undefined
     80,                        // SK_SPELLCASTING
     105,                       // SK_CONJURATIONS
     70,                        // SK_ENCHANTMENTS
     100,                       // SK_SUMMONINGS
     120,                       // SK_NECROMANCY
     100,                       // SK_TRANSLOCATIONS
     100,                       // SK_TRANSMIGRATION
     100,                       // SK_DIVINATIONS
     100,                       // SK_FIRE_MAGIC
     100,                       // SK_ICE_MAGIC
     70,                        // SK_AIR_MAGIC
     130,                       // SK_EARTH_MAGIC
     110,                       // SK_POISON_MAGIC
     100,                       // SK_INVOCATIONS
     },

    {                           // SP_HIGH_ELF (3)
     100,                       // SK_FIGHTING
     70,                        // SK_SHORT_BLADES
     70,                        // SK_LONG_SWORDS
     115,                       // SK_UNUSED_1
     130,                       // SK_AXES
     150,                       // SK_MACES_FLAILS
     150,                       // SK_POLEARMS
     100,                       // SK_STAVES
     140,                       // SK_SLINGS
     60,                        // SK_BOWS
     100,                       // SK_CROSSBOWS
     90,                        // SK_DARTS
     80,                        // SK_THROWING
     110,                       // SK_ARMOUR
     90,                        // SK_DODGING
     90,                        // SK_STEALTH
     110,                       // SK_STABBING
     110,                       // SK_SHIELDS
     100,                       // SK_TRAPS_DOORS
     130,                       // SK_UNARMED_COMBAT
     100,                       // undefined
     100,                       // undefined
     100,                       // undefined
     100,                       // undefined
     100,                       // undefined
     70,                        // SK_SPELLCASTING
     90,                        // SK_CONJURATIONS
     70,                        // SK_ENCHANTMENTS
     110,                       // SK_SUMMONINGS
     130,                       // SK_NECROMANCY
     90,                        // SK_TRANSLOCATIONS
     90,                        // SK_TRANSMIGRATION
     110,                       // SK_DIVINATIONS
     100,                       // SK_FIRE_MAGIC
     100,                       // SK_ICE_MAGIC
     70,                        // SK_AIR_MAGIC
     130,                       // SK_EARTH_MAGIC
     130,                       // SK_POISON_MAGIC
     100,                       // SK_INVOCATIONS
     },

    {                           // SP_GREY_ELF (4)
     140,                       // SK_FIGHTING
     90,                        // SK_SHORT_BLADES
     95,                        // SK_LONG_SWORDS
     120,                       // SK_UNUSED_1
     140,                       // SK_AXES
     160,                       // SK_MACES_FLAILS
     160,                       // SK_POLEARMS
     100,                       // SK_STAVES
     130,                       // SK_SLINGS
     70,                        // SK_BOWS
     100,                       // SK_CROSSBOWS
     90,                        // SK_DARTS
     80,                        // SK_THROWING
     140,                       // SK_ARMOUR
     75,                        // SK_DODGING
     70,                        // SK_STEALTH
     100,                       // SK_STABBING
     140,                       // SK_SHIELDS
     100,                       // SK_TRAPS_DOORS
     130,                       // SK_UNARMED_COMBAT
     100,                       // undefined
     100,                       // undefined
     100,                       // undefined
     100,                       // undefined
     100,                       // undefined
     60,                        // SK_SPELLCASTING
     90,                        // SK_CONJURATIONS
     50,                        // SK_ENCHANTMENTS
     90,                        // SK_SUMMONINGS
     130,                       // SK_NECROMANCY
     80,                        // SK_TRANSLOCATIONS
     80,                        // SK_TRANSMIGRATION
     80,                        // SK_DIVINATIONS
     90,                        // SK_FIRE_MAGIC
     90,                        // SK_ICE_MAGIC
     60,                        // SK_AIR_MAGIC
     150,                       // SK_EARTH_MAGIC
     110,                       // SK_POISON_MAGIC
     100,                       // SK_INVOCATIONS
     },

    {                           // SP_DEEP_ELF (5)
     150,                       // SK_FIGHTING
     100,                       // SK_SHORT_BLADES
     105,                       // SK_LONG_SWORDS
     120,                       // SK_UNUSED_1
     150,                       // SK_AXES
     165,                       // SK_MACES_FLAILS
     165,                       // SK_POLEARMS
     100,                       // SK_STAVES
     135,                       // SK_SLINGS
     74,                        // SK_BOWS
     75,                        // SK_CROSSBOWS
     75,                        // SK_DARTS
     80,                        // SK_THROWING
     140,                       // SK_ARMOUR
     70,                        // SK_DODGING
     65,                        // SK_STEALTH
     80,                        // SK_STABBING
     140,                       // SK_SHIELDS
     100,                       // SK_TRAPS_DOORS
     130,                       // SK_UNARMED_COMBAT
     100,                       // undefined
     100,                       // undefined
     100,                       // undefined
     100,                       // undefined
     100,                       // undefined
     55,                        // SK_SPELLCASTING
     80,                        // SK_CONJURATIONS
     50,                        // SK_ENCHANTMENTS
     80,                        // SK_SUMMONINGS
     70,                        // SK_NECROMANCY
     75,                        // SK_TRANSLOCATIONS
     75,                        // SK_TRANSMIGRATION
     75,                        // SK_DIVINATIONS
     90,                        // SK_FIRE_MAGIC
     90,                        // SK_ICE_MAGIC
     80,                        // SK_AIR_MAGIC
     100,                       // SK_EARTH_MAGIC
     80,                        // SK_POISON_MAGIC
     100,                       // SK_INVOCATIONS
     },

    {                           // SP_SLUDGE_ELF (6)
     100,                       // SK_FIGHTING
     80,                        // SK_SHORT_BLADES
     85,                        // SK_LONG_SWORDS
     110,                       // SK_UNUSED_1
     130,                       // SK_AXES
     140,                       // SK_MACES_FLAILS
     140,                       // SK_POLEARMS
     100,                       // SK_STAVES
     100,                       // SK_SLINGS
     70,                        // SK_BOWS
     100,                       // SK_CROSSBOWS
     80,                        // SK_DARTS
     70,                        // SK_THROWING
     140,                       // SK_ARMOUR
     70,                        // SK_DODGING
     75,                        // SK_STEALTH
     100,                       // SK_STABBING
     130,                       // SK_SHIELDS
     100,                       // SK_TRAPS_DOORS
     90,                        // SK_UNARMED_COMBAT
     100,                       // undefined
     100,                       // undefined
     100,                       // undefined
     100,                       // undefined
     100,                       // undefined
     70,                        // SK_SPELLCASTING
     90,                        // SK_CONJURATIONS
     110,                       // SK_ENCHANTMENTS
     90,                        // SK_SUMMONINGS
     100,                       // SK_NECROMANCY
     100,                       // SK_TRANSLOCATIONS
     60,                        // SK_TRANSMIGRATION
     100,                       // SK_DIVINATIONS
     95,                        // SK_FIRE_MAGIC
     95,                        // SK_ICE_MAGIC
     95,                        // SK_AIR_MAGIC
     95,                        // SK_EARTH_MAGIC
     100,                       // SK_POISON_MAGIC
     100,                       // SK_INVOCATIONS
     },

    {                           // SP_HILL_DWARF (7)
     70,                        // SK_FIGHTING
     80,                        // SK_SHORT_BLADES
     80,                        // SK_LONG_SWORDS
     90,                        // SK_UNUSED_1
     60,                        // SK_AXES
     70,                        // SK_MACES_FLAILS
     110,                       // SK_POLEARMS
     130,                       // SK_STAVES
     130,                       // SK_SLINGS
     150,                       // SK_BOWS
     120,                       // SK_CROSSBOWS
     120,                       // SK_DARTS
     120,                       // SK_THROWING
     70,                        // SK_ARMOUR
     120,                       // SK_DODGING
     150,                       // SK_STEALTH
     140,                       // SK_STABBING
     70,                        // SK_SHIELDS
     100,                       // SK_TRAPS_DOORS
     100,                       // SK_UNARMED_COMBAT
     100,                       // undefined
     100,                       // undefined
     100,                       // undefined
     100,                       // undefined
     100,                       // undefined
     160,                       // SK_SPELLCASTING
     120,                       // SK_CONJURATIONS
     150,                       // SK_ENCHANTMENTS
     150,                       // SK_SUMMONINGS
     160,                       // SK_NECROMANCY
     150,                       // SK_TRANSLOCATIONS
     120,                       // SK_TRANSMIGRATION
     130,                       // SK_DIVINATIONS
     80,                        // SK_FIRE_MAGIC
     120,                       // SK_ICE_MAGIC
     150,                       // SK_AIR_MAGIC
     70,                        // SK_EARTH_MAGIC
     130,                       // SK_POISON_MAGIC
     100,                       // SK_INVOCATIONS
     },

    {                           // SP_MOUNTAIN_DWARF (8)
     70,                        // SK_FIGHTING
     90,                        // SK_SHORT_BLADES
     90,                        // SK_LONG_SWORDS
     100,                       // SK_UNUSED_1
     70,                        // SK_AXES
     70,                        // SK_MACES_FLAILS
     110,                       // SK_POLEARMS
     120,                       // SK_STAVES
     125,                       // SK_SLINGS
     140,                       // SK_BOWS
     100,                       // SK_CROSSBOWS
     120,                       // SK_DARTS
     115,                       // SK_THROWING
     60,                        // SK_ARMOUR
     110,                       // SK_DODGING
     140,                       // SK_STEALTH
     130,                       // SK_STABBING
     70,                        // SK_SHIELDS
     100,                       // SK_TRAPS_DOORS
     100,                       // SK_UNARMED_COMBAT
     100,                       // undefined
     100,                       // undefined
     100,                       // undefined
     100,                       // undefined
     100,                       // undefined
     140,                       // SK_SPELLCASTING
     115,                       // SK_CONJURATIONS
     135,                       // SK_ENCHANTMENTS
     150,                       // SK_SUMMONINGS
     160,                       // SK_NECROMANCY
     150,                       // SK_TRANSLOCATIONS
     120,                       // SK_TRANSMIGRATION
     130,                       // SK_DIVINATIONS
     70,                        // SK_FIRE_MAGIC
     130,                       // SK_ICE_MAGIC
     150,                       // SK_AIR_MAGIC
     70,                        // SK_EARTH_MAGIC
     130,                       // SK_POISON_MAGIC
     100,                       // SK_INVOCATIONS
     },

    {                           // SP_HALFLING (9)
     120,                       // SK_FIGHTING
     60,                        // SK_SHORT_BLADES
     100,                       // SK_LONG_SWORDS
     130,                       // SK_UNUSED_1
     120,                       // SK_AXES
     150,                       // SK_MACES_FLAILS
     160,                       // SK_POLEARMS
     130,                       // SK_STAVES
     50,                        // SK_SLINGS
     70,                        // SK_BOWS
     90,                        // SK_CROSSBOWS
     50,                        // SK_DARTS
     60,                        // SK_THROWING
     150,                       // SK_ARMOUR
     70,                        // SK_DODGING
     60,                        // SK_STEALTH
     70,                        // SK_STABBING
     130,                       // SK_SHIELDS
     100,                       // SK_TRAPS_DOORS
     140,                       // SK_UNARMED_COMBAT
     100,                       // undefined
     100,                       // undefined
     100,                       // undefined
     100,                       // undefined
     100,                       // undefined
     130,                       // SK_SPELLCASTING
     130,                       // SK_CONJURATIONS
     100,                       // SK_ENCHANTMENTS
     120,                       // SK_SUMMONINGS
     150,                       // SK_NECROMANCY
     100,                       // SK_TRANSLOCATIONS
     150,                       // SK_TRANSMIGRATION
     140,                       // SK_DIVINATIONS
     100,                       // SK_FIRE_MAGIC
     100,                       // SK_ICE_MAGIC
     90,                        // SK_AIR_MAGIC
     100,                       // SK_EARTH_MAGIC
     120,                       // SK_POISON_MAGIC
     100,                       // SK_INVOCATIONS
     },

    {                           // SP_HILL_ORC (10)
     70,                        // SK_FIGHTING
     100,                       // SK_SHORT_BLADES
     80,                        // SK_LONG_SWORDS
     70,                        // SK_UNUSED_1
     70,                        // SK_AXES
     80,                        // SK_MACES_FLAILS
     80,                        // SK_POLEARMS
     110,                       // SK_STAVES
     130,                       // SK_SLINGS
     120,                       // SK_BOWS
     120,                       // SK_CROSSBOWS
     130,                       // SK_DARTS
     130,                       // SK_THROWING
     90,                        // SK_ARMOUR
     140,                       // SK_DODGING
     150,                       // SK_STEALTH
     100,                       // SK_STABBING
     80,                        // SK_SHIELDS
     100,                       // SK_TRAPS_DOORS
     90,                        // SK_UNARMED_COMBAT
     100,                       // undefined
     100,                       // undefined
     100,                       // undefined
     100,                       // undefined
     100,                       // undefined
     150,                       // SK_SPELLCASTING
     100,                       // SK_CONJURATIONS
     120,                       // SK_ENCHANTMENTS
     120,                       // SK_SUMMONINGS
     100,                       // SK_NECROMANCY
     150,                       // SK_TRANSLOCATIONS
     160,                       // SK_TRANSMIGRATION
     160,                       // SK_DIVINATIONS
     100,                       // SK_FIRE_MAGIC
     100,                       // SK_ICE_MAGIC
     150,                       // SK_AIR_MAGIC
     100,                       // SK_EARTH_MAGIC
     110,                       // SK_POISON_MAGIC
     100,                       // SK_INVOCATIONS
     },

    {                           // SP_KOBOLD (11)
     80,                        // SK_FIGHTING
     60,                        // SK_SHORT_BLADES
     100,                       // SK_LONG_SWORDS
     120,                       // SK_UNUSED_1
     110,                       // SK_AXES
     140,                       // SK_MACES_FLAILS
     150,                       // SK_POLEARMS
     110,                       // SK_STAVES
     70,                        // SK_SLINGS
     80,                        // SK_BOWS
     90,                        // SK_CROSSBOWS
     50,                        // SK_DARTS
     60,                        // SK_THROWING
     140,                       // SK_ARMOUR
     70,                        // SK_DODGING
     60,                        // SK_STEALTH
     70,                        // SK_STABBING
     130,                       // SK_SHIELDS
     100,                       // SK_TRAPS_DOORS
     100,                       // SK_UNARMED_COMBAT
     100,                       // undefined
     100,                       // undefined
     100,                       // undefined
     100,                       // undefined
     100,                       // undefined
     110,                       // SK_SPELLCASTING
     110,                       // SK_CONJURATIONS
     110,                       // SK_ENCHANTMENTS
     105,                       // SK_SUMMONINGS
     105,                       // SK_NECROMANCY
     100,                       // SK_TRANSLOCATIONS
     110,                       // SK_TRANSMIGRATION
     130,                       // SK_DIVINATIONS
     100,                       // SK_FIRE_MAGIC
     100,                       // SK_ICE_MAGIC
     100,                       // SK_AIR_MAGIC
     100,                       // SK_EARTH_MAGIC
     100,                       // SK_POISON_MAGIC
     100,                       // SK_INVOCATIONS
     },

    {                           // SP_MUMMY (12)
     100,                       // SK_FIGHTING
     140,                       // SK_SHORT_BLADES
     140,                       // SK_LONG_SWORDS
     140,                       // SK_UNUSED_1
     140,                       // SK_AXES
     140,                       // SK_MACES_FLAILS
     140,                       // SK_POLEARMS
     140,                       // SK_STAVES
     140,                       // SK_SLINGS
     140,                       // SK_BOWS
     140,                       // SK_CROSSBOWS
     140,                       // SK_DARTS
     140,                       // SK_THROWING
     140,                       // SK_ARMOUR
     140,                       // SK_DODGING
     140,                       // SK_STEALTH
     140,                       // SK_STABBING
     140,                       // SK_SHIELDS
     140,                       // SK_TRAPS_DOORS
     140,                       // SK_UNARMED_COMBAT
     140,                       // undefined
     140,                       // undefined
     140,                       // undefined
     140,                       // undefined
     140,                       // undefined
     100,                       // SK_SPELLCASTING
     140,                       // SK_CONJURATIONS
     140,                       // SK_ENCHANTMENTS
     140,                       // SK_SUMMONINGS
     100,                       // SK_NECROMANCY
     140,                       // SK_TRANSLOCATIONS
     140,                       // SK_TRANSMIGRATION
     140,                       // SK_DIVINATIONS
     140,                       // SK_FIRE_MAGIC
     140,                       // SK_ICE_MAGIC
     140,                       // SK_AIR_MAGIC
     140,                       // SK_EARTH_MAGIC
     140,                       // SK_POISON_MAGIC
     140,                       // SK_INVOCATIONS
     },

    {                           // SP_NAGA (13)
     100,                       // SK_FIGHTING
     100,                       // SK_SHORT_BLADES
     100,                       // SK_LONG_SWORDS
     100,                       // SK_UNUSED_1
     100,                       // SK_AXES
     100,                       // SK_MACES_FLAILS
     100,                       // SK_POLEARMS
     120,                       // SK_STAVES
     120,                       // SK_SLINGS
     120,                       // SK_BOWS
     120,                       // SK_CROSSBOWS
     120,                       // SK_DARTS
     120,                       // SK_THROWING
     150,                       // SK_ARMOUR
     150,                       // SK_DODGING
     40,                        // SK_STEALTH
     100,                       // SK_STABBING
     140,                       // SK_SHIELDS
     100,                       // SK_TRAPS_DOORS
     100,                       // SK_UNARMED_COMBAT
     100,                       // undefined
     100,                       // undefined
     100,                       // undefined
     100,                       // undefined
     100,                       // undefined
     100,                       // SK_SPELLCASTING
     100,                       // SK_CONJURATIONS
     100,                       // SK_ENCHANTMENTS
     100,                       // SK_SUMMONINGS
     100,                       // SK_NECROMANCY
     100,                       // SK_TRANSLOCATIONS
     100,                       // SK_TRANSMIGRATION
     100,                       // SK_DIVINATIONS
     100,                       // SK_FIRE_MAGIC
     100,                       // SK_ICE_MAGIC
     100,                       // SK_AIR_MAGIC
     100,                       // SK_EARTH_MAGIC
     60,                        // SK_POISON_MAGIC
     100,                       // SK_INVOCATIONS
     },

    {                           // SP_GNOME (14)
     100,                       // SK_FIGHTING
     75,                        // SK_SHORT_BLADES
     100,                       // SK_LONG_SWORDS
     130,                       // SK_UNUSED_1
     100,                       // SK_AXES
     130,                       // SK_MACES_FLAILS
     140,                       // SK_POLEARMS
     130,                       // SK_STAVES
     80,                        // SK_SLINGS
     100,                       // SK_BOWS
     90,                        // SK_CROSSBOWS
     60,                        // SK_DARTS
     100,                       // SK_THROWING
     150,                       // SK_ARMOUR
     70,                        // SK_DODGING
     70,                        // SK_STEALTH
     80,                        // SK_STABBING
     120,                       // SK_SHIELDS
     100,                       // SK_TRAPS_DOORS
     110,                       // SK_UNARMED_COMBAT
     100,                       // undefined
     100,                       // undefined
     100,                       // undefined
     100,                       // undefined
     100,                       // undefined
     120,                       // SK_SPELLCASTING
     100,                       // SK_CONJURATIONS
     100,                       // SK_ENCHANTMENTS
     110,                       // SK_SUMMONINGS
     130,                       // SK_NECROMANCY
     130,                       // SK_TRANSLOCATIONS
     120,                       // SK_TRANSMIGRATION
     120,                       // SK_DIVINATIONS
     100,                       // SK_FIRE_MAGIC
     100,                       // SK_ICE_MAGIC
     170,                       // SK_AIR_MAGIC
     60,                        // SK_EARTH_MAGIC
     130,                       // SK_POISON_MAGIC
     120,                       // SK_INVOCATIONS
     },

    {                           // SP_OGRE (15)
     100,                       // SK_FIGHTING
     140,                       // SK_SHORT_BLADES
     120,                       // SK_LONG_SWORDS
     110,                       // SK_UNUSED_1
     100,                       // SK_AXES
     100,                       // SK_MACES_FLAILS
     110,                       // SK_POLEARMS
     120,                       // SK_STAVES
     150,                       // SK_SLINGS
     150,                       // SK_BOWS
     180,                       // SK_CROSSBOWS
     150,                       // SK_DARTS
     100,                       // SK_THROWING
     140,                       // SK_ARMOUR
     150,                       // SK_DODGING
     200,                       // SK_STEALTH
     150,                       // SK_STABBING
     110,                       // SK_SHIELDS
     200,                       // SK_TRAPS_DOORS
     130,                       // SK_UNARMED_COMBAT
     100,                       // undefined
     100,                       // undefined
     100,                       // undefined
     100,                       // undefined
     100,                       // undefined
     220,                       // SK_SPELLCASTING
     180,                       // SK_CONJURATIONS
     220,                       // SK_ENCHANTMENTS
     200,                       // SK_SUMMONINGS
     150,                       // SK_NECROMANCY
     200,                       // SK_TRANSLOCATIONS
     200,                       // SK_TRANSMIGRATION
     200,                       // SK_DIVINATIONS
     150,                       // SK_FIRE_MAGIC
     150,                       // SK_ICE_MAGIC
     200,                       // SK_AIR_MAGIC
     120,                       // SK_EARTH_MAGIC
     150,                       // SK_POISON_MAGIC
     130,                       // SK_INVOCATIONS
     },

    {                           // SP_TROLL (16)
     150,                       // SK_FIGHTING
     150,                       // SK_SHORT_BLADES
     150,                       // SK_LONG_SWORDS
     150,                       // SK_UNUSED_1
     150,                       // SK_AXES
     150,                       // SK_MACES_FLAILS
     150,                       // SK_POLEARMS
     150,                       // SK_STAVES
     150,                       // SK_SLINGS
     150,                       // SK_BOWS
     180,                       // SK_CROSSBOWS
     150,                       // SK_DARTS
     150,                       // SK_THROWING
     150,                       // SK_ARMOUR
     130,                       // SK_DODGING
     250,                       // SK_STEALTH
     130,                       // SK_STABBING
     140,                       // SK_SHIELDS
     200,                       // SK_TRAPS_DOORS
     120,                       // SK_UNARMED_COMBAT
     100,                       // undefined
     100,                       // undefined
     100,                       // undefined
     100,                       // undefined
     100,                       // undefined
     200,                       // SK_SPELLCASTING
     150,                       // SK_CONJURATIONS
     200,                       // SK_ENCHANTMENTS
     150,                       // SK_SUMMONINGS
     140,                       // SK_NECROMANCY
     150,                       // SK_TRANSLOCATIONS
     150,                       // SK_TRANSMIGRATION
     180,                       // SK_DIVINATIONS
     150,                       // SK_FIRE_MAGIC
     150,                       // SK_ICE_MAGIC
     200,                       // SK_AIR_MAGIC
     110,                       // SK_EARTH_MAGIC
     130,                       // SK_POISON_MAGIC
     150,                       // SK_INVOCATIONS
     },

    {                           // SP_OGRE_MAGE (17)
     100,                       // SK_FIGHTING
     110,                       // SK_SHORT_BLADES
     100,                       // SK_LONG_SWORDS
     100,                       // SK_UNUSED_1
     100,                       // SK_AXES
     100,                       // SK_MACES_FLAILS
     100,                       // SK_POLEARMS
     100,                       // SK_STAVES
     150,                       // SK_SLINGS
     150,                       // SK_BOWS
     150,                       // SK_CROSSBOWS
     150,                       // SK_DARTS
     150,                       // SK_THROWING
     170,                       // SK_ARMOUR
     130,                       // SK_DODGING
     100,                       // SK_STEALTH
     130,                       // SK_STABBING
     150,                       // SK_SHIELDS
     150,                       // SK_TRAPS_DOORS
     100,                       // SK_UNARMED_COMBAT
     100,                       // undefined
     100,                       // undefined
     100,                       // undefined
     100,                       // undefined
     100,                       // undefined
     70,                        // SK_SPELLCASTING
     100,                       // SK_CONJURATIONS
     80,                        // SK_ENCHANTMENTS
     100,                       // SK_SUMMONINGS
     100,                       // SK_NECROMANCY
     100,                       // SK_TRANSLOCATIONS
     100,                       // SK_TRANSMIGRATION
     100,                       // SK_DIVINATIONS
     100,                       // SK_FIRE_MAGIC
     100,                       // SK_ICE_MAGIC
     100,                       // SK_AIR_MAGIC
     100,                       // SK_EARTH_MAGIC
     100,                       // SK_POISON_MAGIC
     100,                       // SK_INVOCATIONS
     },

    {                           // SP_RED_DRACONIAN (18)
     90,                        // SK_FIGHTING
     100,                       // SK_SHORT_BLADES
     100,                       // SK_LONG_SWORDS
     100,                       // SK_UNUSED_1
     100,                       // SK_AXES
     100,                       // SK_MACES_FLAILS
     100,                       // SK_POLEARMS
     100,                       // SK_STAVES
     120,                       // SK_SLINGS
     120,                       // SK_BOWS
     120,                       // SK_CROSSBOWS
     120,                       // SK_DARTS
     120,                       // SK_THROWING
     200,                       // SK_ARMOUR
     120,                       // SK_DODGING
     120,                       // SK_STEALTH
     100,                       // SK_STABBING
     100,                       // SK_SHIELDS
     100,                       // SK_TRAPS_DOORS
     100,                       // SK_UNARMED_COMBAT
     100,                       // undefined
     100,                       // undefined
     100,                       // undefined
     100,                       // undefined
     100,                       // undefined
     100,                       // SK_SPELLCASTING
     100,                       // SK_CONJURATIONS
     120,                       // SK_ENCHANTMENTS
     100,                       // SK_SUMMONINGS
     100,                       // SK_NECROMANCY
     100,                       // SK_TRANSLOCATIONS
     100,                       // SK_TRANSMIGRATION
     100,                       // SK_DIVINATIONS
     70,                        // SK_FIRE_MAGIC
     150,                       // SK_ICE_MAGIC
     100,                       // SK_AIR_MAGIC
     100,                       // SK_EARTH_MAGIC
     100,                       // SK_POISON_MAGIC
     100,                       // SK_INVOCATIONS
     },

    {                           // SP_WHITE_DRACONIAN (19)
     90,                        // SK_FIGHTING
     100,                       // SK_SHORT_BLADES
     100,                       // SK_LONG_SWORDS
     100,                       // SK_UNUSED_1
     100,                       // SK_AXES
     100,                       // SK_MACES_FLAILS
     100,                       // SK_POLEARMS
     100,                       // SK_STAVES
     120,                       // SK_SLINGS
     120,                       // SK_BOWS
     120,                       // SK_CROSSBOWS
     120,                       // SK_DARTS
     120,                       // SK_THROWING
     200,                       // SK_ARMOUR
     120,                       // SK_DODGING
     120,                       // SK_STEALTH
     100,                       // SK_STABBING
     100,                       // SK_SHIELDS
     100,                       // SK_TRAPS_DOORS
     100,                       // SK_UNARMED_COMBAT
     100,                       // undefined
     100,                       // undefined
     100,                       // undefined
     100,                       // undefined
     100,                       // undefined
     100,                       // SK_SPELLCASTING
     100,                       // SK_CONJURATIONS
     120,                       // SK_ENCHANTMENTS
     100,                       // SK_SUMMONINGS
     100,                       // SK_NECROMANCY
     100,                       // SK_TRANSLOCATIONS
     100,                       // SK_TRANSMIGRATION
     100,                       // SK_DIVINATIONS
     150,                       // SK_FIRE_MAGIC
     70,                        // SK_ICE_MAGIC
     100,                       // SK_AIR_MAGIC
     100,                       // SK_EARTH_MAGIC
     100,                       // SK_POISON_MAGIC
     100,                       // SK_INVOCATIONS
     },

    {                           // SP_GREEN_DRACONIAN (20)
     90,                        // SK_FIGHTING
     100,                       // SK_SHORT_BLADES
     100,                       // SK_LONG_SWORDS
     100,                       // SK_UNUSED_1
     100,                       // SK_AXES
     100,                       // SK_MACES_FLAILS
     100,                       // SK_POLEARMS
     100,                       // SK_STAVES
     120,                       // SK_SLINGS
     120,                       // SK_BOWS
     120,                       // SK_CROSSBOWS
     120,                       // SK_DARTS
     120,                       // SK_THROWING
     200,                       // SK_ARMOUR
     120,                       // SK_DODGING
     120,                       // SK_STEALTH
     100,                       // SK_STABBING
     100,                       // SK_SHIELDS
     100,                       // SK_TRAPS_DOORS
     100,                       // SK_UNARMED_COMBAT
     100,                       // undefined
     100,                       // undefined
     100,                       // undefined
     100,                       // undefined
     100,                       // undefined
     100,                       // SK_SPELLCASTING
     100,                       // SK_CONJURATIONS
     120,                       // SK_ENCHANTMENTS
     100,                       // SK_SUMMONINGS
     100,                       // SK_NECROMANCY
     100,                       // SK_TRANSLOCATIONS
     100,                       // SK_TRANSMIGRATION
     100,                       // SK_DIVINATIONS
     100,                       // SK_FIRE_MAGIC
     100,                       // SK_ICE_MAGIC
     100,                       // SK_AIR_MAGIC
     100,                       // SK_EARTH_MAGIC
     70,                        // SK_POISON_MAGIC
     100,                       // SK_INVOCATIONS
     },

    {                           // SP_YELLOW_DRACONIAN (21)
     90,                        // SK_FIGHTING
     100,                       // SK_SHORT_BLADES
     100,                       // SK_LONG_SWORDS
     100,                       // SK_UNUSED_1
     100,                       // SK_AXES
     100,                       // SK_MACES_FLAILS
     100,                       // SK_POLEARMS
     100,                       // SK_STAVES
     120,                       // SK_SLINGS
     120,                       // SK_BOWS
     120,                       // SK_CROSSBOWS
     120,                       // SK_DARTS
     120,                       // SK_THROWING
     200,                       // SK_ARMOUR
     120,                       // SK_DODGING
     120,                       // SK_STEALTH
     100,                       // SK_STABBING
     100,                       // SK_SHIELDS
     100,                       // SK_TRAPS_DOORS
     100,                       // SK_UNARMED_COMBAT
     100,                       // undefined
     100,                       // undefined
     100,                       // undefined
     100,                       // undefined
     100,                       // undefined
     100,                       // SK_SPELLCASTING
     100,                       // SK_CONJURATIONS
     120,                       // SK_ENCHANTMENTS
     100,                       // SK_SUMMONINGS
     100,                       // SK_NECROMANCY
     100,                       // SK_TRANSLOCATIONS
     100,                       // SK_TRANSMIGRATION
     100,                       // SK_DIVINATIONS
     100,                       // SK_FIRE_MAGIC
     100,                       // SK_ICE_MAGIC
     100,                       // SK_AIR_MAGIC
     100,                       // SK_EARTH_MAGIC
     100,                       // SK_POISON_MAGIC
     100,                       // SK_INVOCATIONS
     },

    {                           // SP_GREY_DRACONIAN (22)
     90,                        // SK_FIGHTING
     100,                       // SK_SHORT_BLADES
     100,                       // SK_LONG_SWORDS
     100,                       // SK_UNUSED_1
     100,                       // SK_AXES
     100,                       // SK_MACES_FLAILS
     100,                       // SK_POLEARMS
     100,                       // SK_STAVES
     120,                       // SK_SLINGS
     120,                       // SK_BOWS
     120,                       // SK_CROSSBOWS
     120,                       // SK_DARTS
     120,                       // SK_THROWING
     200,                       // SK_ARMOUR
     120,                       // SK_DODGING
     120,                       // SK_STEALTH
     100,                       // SK_STABBING
     100,                       // SK_SHIELDS
     100,                       // SK_TRAPS_DOORS
     100,                       // SK_UNARMED_COMBAT
     100,                       // undefined
     100,                       // undefined
     100,                       // undefined
     100,                       // undefined
     100,                       // undefined
     100,                       // SK_SPELLCASTING
     100,                       // SK_CONJURATIONS
     120,                       // SK_ENCHANTMENTS
     100,                       // SK_SUMMONINGS
     100,                       // SK_NECROMANCY
     100,                       // SK_TRANSLOCATIONS
     100,                       // SK_TRANSMIGRATION
     100,                       // SK_DIVINATIONS
     100,                       // SK_FIRE_MAGIC
     100,                       // SK_ICE_MAGIC
     100,                       // SK_AIR_MAGIC
     100,                       // SK_EARTH_MAGIC
     100,                       // SK_POISON_MAGIC
     100,                       // SK_INVOCATIONS
     },

    {                           // SP_BLACK_DRACONIAN (23)
     90,                        // SK_FIGHTING
     100,                       // SK_SHORT_BLADES
     100,                       // SK_LONG_SWORDS
     100,                       // SK_UNUSED_1
     100,                       // SK_AXES
     100,                       // SK_MACES_FLAILS
     100,                       // SK_POLEARMS
     100,                       // SK_STAVES
     120,                       // SK_SLINGS
     120,                       // SK_BOWS
     120,                       // SK_CROSSBOWS
     120,                       // SK_DARTS
     120,                       // SK_THROWING
     200,                       // SK_ARMOUR
     120,                       // SK_DODGING
     120,                       // SK_STEALTH
     100,                       // SK_STABBING
     100,                       // SK_SHIELDS
     100,                       // SK_TRAPS_DOORS
     100,                       // SK_UNARMED_COMBAT
     100,                       // undefined
     100,                       // undefined
     100,                       // undefined
     100,                       // undefined
     100,                       // undefined
     100,                       // SK_SPELLCASTING
     100,                       // SK_CONJURATIONS
     120,                       // SK_ENCHANTMENTS
     100,                       // SK_SUMMONINGS
     100,                       // SK_NECROMANCY
     100,                       // SK_TRANSLOCATIONS
     100,                       // SK_TRANSMIGRATION
     100,                       // SK_DIVINATIONS
     100,                       // SK_FIRE_MAGIC
     100,                       // SK_ICE_MAGIC
     70,                        // SK_AIR_MAGIC
     150,                       // SK_EARTH_MAGIC
     100,                       // SK_POISON_MAGIC
     100,                       // SK_INVOCATIONS
     },

    {                           // SP_PURPLE_DRACONIAN (24)
     90,                        // SK_FIGHTING
     100,                       // SK_SHORT_BLADES
     100,                       // SK_LONG_SWORDS
     100,                       // SK_UNUSED_1
     100,                       // SK_AXES
     100,                       // SK_MACES_FLAILS
     100,                       // SK_POLEARMS
     100,                       // SK_STAVES
     120,                       // SK_SLINGS
     120,                       // SK_BOWS
     120,                       // SK_CROSSBOWS
     120,                       // SK_DARTS
     120,                       // SK_THROWING
     200,                       // SK_ARMOUR
     120,                       // SK_DODGING
     120,                       // SK_STEALTH
     100,                       // SK_STABBING
     100,                       // SK_SHIELDS
     100,                       // SK_TRAPS_DOORS
     100,                       // SK_UNARMED_COMBAT
     100,                       // undefined
     100,                       // undefined
     100,                       // undefined
     100,                       // undefined
     100,                       // undefined
     70,                        // SK_SPELLCASTING
     100,                       // SK_CONJURATIONS
     120,                       // SK_ENCHANTMENTS
     100,                       // SK_SUMMONINGS
     100,                       // SK_NECROMANCY
     100,                       // SK_TRANSLOCATIONS
     100,                       // SK_TRANSMIGRATION
     100,                       // SK_DIVINATIONS
     100,                       // SK_FIRE_MAGIC
     100,                       // SK_ICE_MAGIC
     100,                       // SK_AIR_MAGIC
     100,                       // SK_EARTH_MAGIC
     100,                       // SK_POISON_MAGIC
     100,                       // SK_INVOCATIONS
     },

    {                           // SP_MOTTLED_DRACONIAN (25)
     90,                        // SK_FIGHTING
     100,                       // SK_SHORT_BLADES
     100,                       // SK_LONG_SWORDS
     100,                       // SK_UNUSED_1
     100,                       // SK_AXES
     100,                       // SK_MACES_FLAILS
     100,                       // SK_POLEARMS
     100,                       // SK_STAVES
     120,                       // SK_SLINGS
     120,                       // SK_BOWS
     120,                       // SK_CROSSBOWS
     120,                       // SK_DARTS
     120,                       // SK_THROWING
     200,                       // SK_ARMOUR
     120,                       // SK_DODGING
     120,                       // SK_STEALTH
     100,                       // SK_STABBING
     100,                       // SK_SHIELDS
     100,                       // SK_TRAPS_DOORS
     100,                       // SK_UNARMED_COMBAT
     100,                       // undefined
     100,                       // undefined
     100,                       // undefined
     100,                       // undefined
     100,                       // undefined
     100,                       // SK_SPELLCASTING
     100,                       // SK_CONJURATIONS
     120,                       // SK_ENCHANTMENTS
     100,                       // SK_SUMMONINGS
     100,                       // SK_NECROMANCY
     100,                       // SK_TRANSLOCATIONS
     100,                       // SK_TRANSMIGRATION
     100,                       // SK_DIVINATIONS
     80,                        // SK_FIRE_MAGIC
     100,                       // SK_ICE_MAGIC
     100,                       // SK_AIR_MAGIC
     100,                       // SK_EARTH_MAGIC
     100,                       // SK_POISON_MAGIC
     100,                       // SK_INVOCATIONS
     },

    {                           // SP_PALE_DRACONIAN (26)
     90,                        // SK_FIGHTING
     100,                       // SK_SHORT_BLADES
     100,                       // SK_LONG_SWORDS
     100,                       // SK_UNUSED_1
     100,                       // SK_AXES
     100,                       // SK_MACES_FLAILS
     100,                       // SK_POLEARMS
     100,                       // SK_STAVES
     120,                       // SK_SLINGS
     120,                       // SK_BOWS
     120,                       // SK_CROSSBOWS
     120,                       // SK_DARTS
     120,                       // SK_THROWING
     200,                       // SK_ARMOUR
     120,                       // SK_DODGING
     120,                       // SK_STEALTH
     100,                       // SK_STABBING
     100,                       // SK_SHIELDS
     100,                       // SK_TRAPS_DOORS
     100,                       // SK_UNARMED_COMBAT
     100,                       // undefined
     100,                       // undefined
     100,                       // undefined
     100,                       // undefined
     100,                       // undefined
     100,                       // SK_SPELLCASTING
     100,                       // SK_CONJURATIONS
     120,                       // SK_ENCHANTMENTS
     100,                       // SK_SUMMONINGS
     100,                       // SK_NECROMANCY
     100,                       // SK_TRANSLOCATIONS
     100,                       // SK_TRANSMIGRATION
     100,                       // SK_DIVINATIONS
     90,                        // SK_FIRE_MAGIC
     100,                       // SK_ICE_MAGIC
     90,                        // SK_AIR_MAGIC
     100,                       // SK_EARTH_MAGIC
     100,                       // SK_POISON_MAGIC
     100,                       // SK_INVOCATIONS
     },

    {                           // SP_UNK0_DRACONAIN (27)
     90,                        // SK_FIGHTING
     100,                       // SK_SHORT_BLADES
     100,                       // SK_LONG_SWORDS
     100,                       // SK_UNUSED_1
     100,                       // SK_AXES
     100,                       // SK_MACES_FLAILS
     100,                       // SK_POLEARMS
     100,                       // SK_STAVES
     120,                       // SK_SLINGS
     120,                       // SK_BOWS
     120,                       // SK_CROSSBOWS
     120,                       // SK_DARTS
     120,                       // SK_THROWING
     200,                       // SK_ARMOUR
     120,                       // SK_DODGING
     120,                       // SK_STEALTH
     100,                       // SK_STABBING
     100,                       // SK_SHIELDS
     100,                       // SK_TRAPS_DOORS
     100,                       // SK_UNARMED_COMBAT
     100,                       // undefined
     100,                       // undefined
     100,                       // undefined
     100,                       // undefined
     100,                       // undefined
     100,                       // SK_SPELLCASTING
     100,                       // SK_CONJURATIONS
     120,                       // SK_ENCHANTMENTS
     100,                       // SK_SUMMONINGS
     100,                       // SK_NECROMANCY
     100,                       // SK_TRANSLOCATIONS
     100,                       // SK_TRANSMIGRATION
     100,                       // SK_DIVINATIONS
     100,                       // SK_FIRE_MAGIC
     100,                       // SK_ICE_MAGIC
     100,                       // SK_AIR_MAGIC
     100,                       // SK_EARTH_MAGIC
     100,                       // SK_POISON_MAGIC
     100,                       // SK_INVOCATIONS
     },

    {                           // SP_UNK1_DRACONIAN (28)
     90,                        // SK_FIGHTING
     100,                       // SK_SHORT_BLADES
     100,                       // SK_LONG_SWORDS
     100,                       // SK_UNUSED_1
     100,                       // SK_AXES
     100,                       // SK_MACES_FLAILS
     100,                       // SK_POLEARMS
     100,                       // SK_STAVES
     120,                       // SK_SLINGS
     120,                       // SK_BOWS
     120,                       // SK_CROSSBOWS
     120,                       // SK_DARTS
     120,                       // SK_THROWING
     200,                       // SK_ARMOUR
     120,                       // SK_DODGING
     120,                       // SK_STEALTH
     100,                       // SK_STABBING
     100,                       // SK_SHIELDS
     100,                       // SK_TRAPS_DOORS
     100,                       // SK_UNARMED_COMBAT
     100,                       // undefined
     100,                       // undefined
     100,                       // undefined
     100,                       // undefined
     100,                       // undefined
     100,                       // SK_SPELLCASTING
     100,                       // SK_CONJURATIONS
     120,                       // SK_ENCHANTMENTS
     100,                       // SK_SUMMONINGS
     100,                       // SK_NECROMANCY
     100,                       // SK_TRANSLOCATIONS
     100,                       // SK_TRANSMIGRATION
     100,                       // SK_DIVINATIONS
     100,                       // SK_FIRE_MAGIC
     100,                       // SK_ICE_MAGIC
     100,                       // SK_AIR_MAGIC
     100,                       // SK_EARTH_MAGIC
     100,                       // SK_POISON_MAGIC
     100,                       // SK_INVOCATIONS
     },

    {                           // SP_UNK2_DRACONIAN (29)
     90,                        // SK_FIGHTING
     100,                       // SK_SHORT_BLADES
     100,                       // SK_LONG_SWORDS
     100,                       // SK_UNUSED_1
     100,                       // SK_AXES
     100,                       // SK_MACES_FLAILS
     100,                       // SK_POLEARMS
     100,                       // SK_STAVES
     120,                       // SK_SLINGS
     120,                       // SK_BOWS
     120,                       // SK_CROSSBOWS
     120,                       // SK_DARTS
     120,                       // SK_THROWING
     200,                       // SK_ARMOUR
     120,                       // SK_DODGING
     120,                       // SK_STEALTH
     100,                       // SK_STABBING
     100,                       // SK_SHIELDS
     100,                       // SK_TRAPS_DOORS
     100,                       // SK_UNARMED_COMBAT
     100,                       // undefined
     100,                       // undefined
     100,                       // undefined
     100,                       // undefined
     100,                       // undefined
     100,                       // SK_SPELLCASTING
     100,                       // SK_CONJURATIONS
     120,                       // SK_ENCHANTMENTS
     100,                       // SK_SUMMONINGS
     100,                       // SK_NECROMANCY
     100,                       // SK_TRANSLOCATIONS
     100,                       // SK_TRANSMIGRATION
     100,                       // SK_DIVINATIONS
     100,                       // SK_FIRE_MAGIC
     100,                       // SK_ICE_MAGIC
     100,                       // SK_AIR_MAGIC
     100,                       // SK_EARTH_MAGIC
     100,                       // SK_POISON_MAGIC
     100,                       // SK_INVOCATIONS
     },

    {                           // SP_CENTAUR (30)
     100,                       // SK_FIGHTING
     120,                       // SK_SHORT_BLADES
     110,                       // SK_LONG_SWORDS
     110,                       // SK_UNUSED_1
     110,                       // SK_AXES
     110,                       // SK_MACES_FLAILS
     110,                       // SK_POLEARMS
     110,                       // SK_STAVES
     75,                        // SK_SLINGS
     60,                        // SK_BOWS
     85,                        // SK_CROSSBOWS
     80,                        // SK_DARTS
     60,                        // SK_THROWING
     180,                       // SK_ARMOUR
     170,                       // SK_DODGING
     200,                       // SK_STEALTH
     170,                       // SK_STABBING
     180,                       // SK_SHIELDS
     150,                       // SK_TRAPS_DOORS
     100,                       // SK_UNARMED_COMBAT
     100,                       // undefined
     100,                       // undefined
     100,                       // undefined
     100,                       // undefined
     100,                       // undefined
     140,                       // SK_SPELLCASTING
     120,                       // SK_CONJURATIONS
     110,                       // SK_ENCHANTMENTS
     120,                       // SK_SUMMONINGS
     120,                       // SK_NECROMANCY
     120,                       // SK_TRANSLOCATIONS
     120,                       // SK_TRANSMIGRATION
     130,                       // SK_DIVINATIONS
     120,                       // SK_FIRE_MAGIC
     120,                       // SK_ICE_MAGIC
     120,                       // SK_AIR_MAGIC
     120,                       // SK_EARTH_MAGIC
     130,                       // SK_POISON_MAGIC
     100,                       // SK_INVOCATIONS
     },

    {                           // SP_DEMIGOD (31)
     110,                       // SK_FIGHTING
     110,                       // SK_SHORT_BLADES
     110,                       // SK_LONG_SWORDS
     110,                       // SK_UNUSED_1
     110,                       // SK_AXES
     110,                       // SK_MACES_FLAILS
     110,                       // SK_POLEARMS
     110,                       // SK_STAVES
     110,                       // SK_SLINGS
     110,                       // SK_BOWS
     110,                       // SK_CROSSBOWS
     110,                       // SK_DARTS
     110,                       // SK_THROWING
     110,                       // SK_ARMOUR
     110,                       // SK_DODGING
     110,                       // SK_STEALTH
     110,                       // SK_STABBING
     110,                       // SK_SHIELDS
     110,                       // SK_TRAPS_DOORS
     110,                       // SK_UNARMED_COMBAT
     110,                       // undefined
     110,                       // undefined
     110,                       // undefined
     110,                       // undefined
     110,                       // undefined
     110,                       // SK_SPELLCASTING
     110,                       // SK_CONJURATIONS
     110,                       // SK_ENCHANTMENTS
     110,                       // SK_SUMMONINGS
     110,                       // SK_NECROMANCY
     110,                       // SK_TRANSLOCATIONS
     110,                       // SK_TRANSMIGRATION
     110,                       // SK_DIVINATIONS
     110,                       // SK_FIRE_MAGIC
     110,                       // SK_ICE_MAGIC
     110,                       // SK_AIR_MAGIC
     110,                       // SK_EARTH_MAGIC
     110,                       // SK_POISON_MAGIC
     100,                       // SK_INVOCATIONS
     },

    {                           // SP_SPRIGGAN (32)
     150,                       // SK_FIGHTING
     90,                        // SK_SHORT_BLADES
     140,                       // SK_LONG_SWORDS
     160,                       // SK_UNUSED_1
     150,                       // SK_AXES
     160,                       // SK_MACES_FLAILS
     180,                       // SK_POLEARMS
     150,                       // SK_STAVES
     70,                        // SK_SLINGS
     70,                        // SK_BOWS
     100,                       // SK_CROSSBOWS
     70,                        // SK_DARTS
     90,                        // SK_THROWING
     170,                       // SK_ARMOUR
     50,                        // SK_DODGING
     50,                        // SK_STEALTH
     50,                        // SK_STABBING
     180,                       // SK_SHIELDS
     60,                        // SK_TRAPS_DOORS
     130,                       // SK_UNARMED_COMBAT
     100,                       // undefined
     100,                       // undefined
     100,                       // undefined
     100,                       // undefined
     100,                       // undefined
     60,                        // SK_SPELLCASTING
     160,                       // SK_CONJURATIONS
     50,                        // SK_ENCHANTMENTS
     150,                       // SK_SUMMONINGS
     120,                       // SK_NECROMANCY
     50,                        // SK_TRANSLOCATIONS
     60,                        // SK_TRANSMIGRATION
     70,                        // SK_DIVINATIONS
     140,                       // SK_FIRE_MAGIC
     140,                       // SK_ICE_MAGIC
     120,                       // SK_AIR_MAGIC
     120,                       // SK_EARTH_MAGIC
     100,                       // SK_POISON_MAGIC
     130,                       // SK_INVOCATIONS
     },

    {                           // SP_MINOTAUR (33)
     70,                        // SK_FIGHTING
     70,                        // SK_SHORT_BLADES
     70,                        // SK_LONG_SWORDS
     70,                        // SK_UNUSED_1
     70,                        // SK_AXES
     70,                        // SK_MACES_FLAILS
     70,                        // SK_POLEARMS
     70,                        // SK_STAVES
     90,                        // SK_SLINGS
     90,                        // SK_BOWS
     90,                        // SK_CROSSBOWS
     90,                        // SK_DARTS
     90,                        // SK_THROWING
     80,                        // SK_ARMOUR
     80,                        // SK_DODGING
     130,                       // SK_STEALTH
     100,                       // SK_STABBING
     80,                        // SK_SHIELDS
     120,                       // SK_TRAPS_DOORS
     80,                        // SK_UNARMED_COMBAT
     100,                       // undefined
     100,                       // undefined
     100,                       // undefined
     100,                       // undefined
     100,                       // undefined
     180,                       // SK_SPELLCASTING
     170,                       // SK_CONJURATIONS
     170,                       // SK_ENCHANTMENTS
     170,                       // SK_SUMMONINGS
     170,                       // SK_NECROMANCY
     170,                       // SK_TRANSLOCATIONS
     170,                       // SK_TRANSMIGRATION
     170,                       // SK_DIVINATIONS
     170,                       // SK_FIRE_MAGIC
     170,                       // SK_ICE_MAGIC
     170,                       // SK_AIR_MAGIC
     170,                       // SK_EARTH_MAGIC
     170,                       // SK_POISON_MAGIC
     130,                       // SK_INVOCATIONS
     },

    {                           // SP_DEMONSPAN (34)
     100,                       // SK_FIGHTING
     110,                       // SK_SHORT_BLADES
     110,                       // SK_LONG_SWORDS
     110,                       // SK_UNUSED_1
     110,                       // SK_AXES
     110,                       // SK_MACES_FLAILS
     110,                       // SK_POLEARMS
     110,                       // SK_STAVES
     110,                       // SK_SLINGS
     110,                       // SK_BOWS
     110,                       // SK_CROSSBOWS
     110,                       // SK_DARTS
     110,                       // SK_THROWING
     110,                       // SK_ARMOUR
     110,                       // SK_DODGING
     110,                       // SK_STEALTH
     110,                       // SK_STABBING
     110,                       // SK_SHIELDS
     110,                       // SK_TRAPS_DOORS
     110,                       // SK_UNARMED_COMBAT
     100,                       // undefined
     100,                       // undefined
     100,                       // undefined
     100,                       // undefined
     100,                       // undefined
     100,                       // SK_SPELLCASTING
     100,                       // SK_CONJURATIONS
     110,                       // SK_ENCHANTMENTS
     100,                       // SK_SUMMONINGS
     90,                        // SK_NECROMANCY
     110,                       // SK_TRANSLOCATIONS
     110,                       // SK_TRANSMIGRATION
     110,                       // SK_DIVINATIONS
     110,                       // SK_FIRE_MAGIC
     110,                       // SK_ICE_MAGIC
     110,                       // SK_AIR_MAGIC
     110,                       // SK_EARTH_MAGIC
     100,                       // SK_POISON_MAGIC
     80,                        // SK_INVOCATIONS
     },

    {                           // SP_GHOUL (35)
     80,                        // SK_FIGHTING
     110,                       // SK_SHORT_BLADES
     110,                       // SK_LONG_SWORDS
     110,                       // SK_UNUSED_1
     110,                       // SK_AXES
     110,                       // SK_MACES_FLAILS
     110,                       // SK_POLEARMS
     110,                       // SK_STAVES
     130,                       // SK_SLINGS
     130,                       // SK_BOWS
     130,                       // SK_CROSSBOWS
     130,                       // SK_DARTS
     130,                       // SK_THROWING
     110,                       // SK_ARMOUR
     110,                       // SK_DODGING
     80,                        // SK_STEALTH
     100,                       // SK_STABBING
     110,                       // SK_SHIELDS
     120,                       // SK_TRAPS_DOORS
     80,                        // SK_UNARMED_COMBAT
     100,                       // undefined
     100,                       // undefined
     100,                       // undefined
     100,                       // undefined
     100,                       // undefined
     120,                       // SK_SPELLCASTING
     130,                       // SK_CONJURATIONS
     130,                       // SK_ENCHANTMENTS
     120,                       // SK_SUMMONINGS
     100,                       // SK_NECROMANCY
     120,                       // SK_TRANSLOCATIONS
     120,                       // SK_TRANSMIGRATION
     120,                       // SK_DIVINATIONS
     150,                       // SK_FIRE_MAGIC
     90,                        // SK_ICE_MAGIC
     150,                       // SK_AIR_MAGIC
     90,                        // SK_EARTH_MAGIC
     100,                       // SK_POISON_MAGIC
     120,                       // SK_INVOCATIONS
     },

    {                           // SP_KENKU (36)
     100,                       // SK_FIGHTING
     75,                        // SK_SHORT_BLADES
     75,                        // SK_LONG_SWORDS
     75,                        // SK_UNUSED_1
     75,                        // SK_AXES
     75,                        // SK_MACES_FLAILS
     75,                        // SK_POLEARMS
     75,                        // SK_STAVES
     100,                       // SK_SLINGS
     80,                        // SK_BOWS
     80,                        // SK_CROSSBOWS
     90,                        // SK_DARTS
     90,                        // SK_THROWING
     90,                        // SK_ARMOUR
     90,                        // SK_DODGING
     100,                       // SK_STEALTH
     80,                        // SK_STABBING
     100,                       // SK_SHIELDS
     100,                       // SK_TRAPS_DOORS
     80,                        // SK_UNARMED_COMBAT
     100,                       // undefined
     100,                       // undefined
     100,                       // undefined
     100,                       // undefined
     100,                       // undefined
     100,                       // SK_SPELLCASTING
     60,                        // SK_CONJURATIONS
     160,                       // SK_ENCHANTMENTS
     70,                        // SK_SUMMONINGS
     80,                        // SK_NECROMANCY
     150,                       // SK_TRANSLOCATIONS
     150,                       // SK_TRANSMIGRATION
     180,                       // SK_DIVINATIONS
     90,                        // SK_FIRE_MAGIC
     120,                       // SK_ICE_MAGIC
     90,                        // SK_AIR_MAGIC
     120,                       // SK_EARTH_MAGIC
     100,                       // SK_POISON_MAGIC
     160,                       // SK_INVOCATIONS
     },

    {                           // SP_MERFOLK (37)
     80,                        // SK_FIGHTING
     70,                        // SK_SHORT_BLADES
     90,                        // SK_LONG_SWORDS
     100,                       // SK_UNUSED_1
     140,                       // SK_AXES
     150,                       // SK_MACES_FLAILS
     50,                        // SK_POLEARMS
     130,                       // SK_STAVES
     150,                       // SK_SLINGS
     140,                       // SK_BOWS
     140,                       // SK_CROSSBOWS
     100,                       // SK_DARTS
     100,                       // SK_THROWING
     160,                       // SK_ARMOUR
     60,                        // SK_DODGING
     90,                        // SK_STEALTH
     70,                        // SK_STABBING
     100,                       // SK_SHIELDS
     120,                       // SK_TRAPS_DOORS
     90,                        // SK_UNARMED_COMBAT
     100,                       // undefined
     100,                       // undefined
     100,                       // undefined
     100,                       // undefined
     100,                       // undefined
     100,                       // SK_SPELLCASTING
     140,                       // SK_CONJURATIONS
     90,                        // SK_ENCHANTMENTS
     100,                       // SK_SUMMONINGS
     150,                       // SK_NECROMANCY
     140,                       // SK_TRANSLOCATIONS
     60,                        // SK_TRANSMIGRATION
     80,                        // SK_DIVINATIONS
     160,                       // SK_FIRE_MAGIC
     80,                        // SK_ICE_MAGIC
     150,                       // SK_AIR_MAGIC
     150,                       // SK_EARTH_MAGIC
     80,                        // SK_POISON_MAGIC
     100,                       // SK_INVOCATIONS
     },



/* ******************************************************

// base draconian
    {
         90,               // SK_FIGHTING
        100,               // SK_SHORT_BLADES
        100,               // SK_LONG_SWORDS
        100,               // SK_UNUSED_1
        100,               // SK_AXES
        100,               // SK_MACES_FLAILS
        100,               // SK_POLEARMS
        100,               // SK_STAVES
        120,               // SK_SLINGS
        120,               // SK_BOWS
        120,               // SK_CROSSBOWS
        120,               // SK_DARTS
        120,               // SK_THROWING
        200,               // SK_ARMOUR
        120,               // SK_DODGING
        120,               // SK_STEALTH
        100,               // SK_STABBING
        100,               // SK_SHIELDS
        100,               // SK_TRAPS_DOORS
        100,               // SK_UNARMED_COMBAT
        100,               // undefined
        100,               // undefined
        100,               // undefined
        100,               // undefined
        100,               // undefined
        100,               // SK_SPELLCASTING
        100,               // SK_CONJURATIONS
        120,               // SK_ENCHANTMENTS
        100,               // SK_SUMMONINGS
        100,               // SK_NECROMANCY
        100,               // SK_TRANSLOCATIONS
        100,               // SK_TRANSMIGRATION
        100,               // SK_DIVINATIONS
        100,               // SK_FIRE_MAGIC
        100,               // SK_ICE_MAGIC
        100,               // SK_AIR_MAGIC
        100,               // SK_EARTH_MAGIC
        100,               // SK_POISON_MAGIC
        100,               // SK_INVOCATIONS
    },

****************************************************** */

};




/* *************************************************************

// these were unimplemented "level titles" for two classes {dlb}

JOB_PRIEST
   "Preacher";
   "Priest";
   "Evangelist";
   "Pontifex";

JOB_PALADIN:
   "Holy Warrior";
   "Holy Crusader";
   "Paladin";
   "Scourge of Evil";

************************************************************* */

void show_skills(void)
{
    char st_pass[60];
    int i;
    int x;
    char strng[5] = "";
    char lcount;

#ifdef DOS_TERM
    window(1, 1, 80, 25);
    char buffer[4600];

    gettext(1, 1, 80, 25, buffer);
#endif
    strcpy(st_pass, "");
    clrscr();

  reprint_stuff:
    gotoxy(1, 1);
    lcount = 'a';
    textcolor(LIGHTGREY);
    cprintf(" You have ");
    itoa(you.exp_available, strng, 10);
    cprintf(strng);
    cprintf(" points of unallocated experience." EOL EOL);

    char scrln = 3, scrcol = 1;

    // Don't want the help line to appear too far down a big window.
    int bottom_line = ((NUMBER_OF_LINES > 30) ? 30 : NUMBER_OF_LINES);

    for (x = 0; x < 50; x++)
    {
        /* spells in second column */
        if ((x == SK_SPELLCASTING && scrcol != 40) || scrln > bottom_line - 3)
        {
            scrln = 3;
            scrcol = 40;
        }
        gotoxy(scrcol, scrln);
        if (you.skills[x] > 0)
        {
            if (you.practise_skill[x] == 0)
                textcolor(DARKGREY);
            else
                textcolor(LIGHTGREY);

            if (you.skills[x] == 27)
                textcolor(YELLOW);

            putch(lcount);
            if (lcount == 'z')
                lcount = 'A';
            else
                lcount++;

            if (you.practise_skill[x] == 0)
                cprintf(" - ");
            else
                cprintf(" + ");

            char bufff[15];

            sprintf(bufff, "%-14s", skills[x][0]);
            cprintf(bufff);

            cprintf(" Skill ");
            itoa(you.skills[x], strng, 10);
            cprintf(strng);

            textcolor(BLUE);
            cprintf(" (");

            const int needed = skill_exp_needed(you.skills[x] + 2);
            const int prev_needed = skill_exp_needed(you.skills[x] + 1);
            const int spec_abil = species_skills(x, you.species);

            itoa( (((needed * spec_abil) / 100 - you.skill_points[x]) * 10) /
                       (((needed - prev_needed) * spec_abil) / 100),
                 strng, 10);

            cprintf(strng);
            cprintf(")");

            textcolor(LIGHTGREY);

#ifdef WIZARD
            cprintf(" / ");
            itoa(you.skill_points[x], strng, 10);
            cprintf(strng);
#endif
            scrln++;
        }
        /* Extra CR between classes of weapons and such things */
        if (x == SK_STAVES || x == SK_THROWING || x == SK_TRAPS_DOORS
            || x == SK_UNARMED_COMBAT || x == SK_TRAPS_DOORS
            || x == SK_INVOCATIONS)
        {
            scrln++;
        }
    }

    // if any more skills added, must adapt letters to go into caps
    gotoxy(1, bottom_line);
    textcolor(LIGHTGREY);
    cprintf("Press the letter of a skill to choose whether you want to practise it.");

    char get_thing;

    get_thing = getch();

    if (get_thing == 0)
        getch();
    else
    {
        if ((get_thing >= 'a' && get_thing <= 'z')
            || (get_thing >= 'A' && get_thing <= 'Z'))
        {
            lcount = 'a';       // toggle skill practise

            for (i = 0; i < 50; i++)
            {
                if (you.skills[i] == 0)
                    continue;

                if (get_thing == lcount)
                {
                    you.practise_skill[i] = (you.practise_skill[i]) ? 0 : 1;
                    break;
                }

                if (lcount == 'z')
                    lcount = 'A';
                else
                    lcount++;
            }

            goto reprint_stuff;
        }
    }

#ifdef DOS_TERM
    puttext(1, 1, 80, 25, buffer);
#endif
    return;
}


char *skill_name(unsigned char which_skill)
{
    return skills[which_skill][0];
}                               // end skill_name()


const char *skill_title(unsigned char best_skill, unsigned char skill_lev)
{
    unsigned char skill_rank;
    char *tempstr = NULL;

    // translate skill level into skill ranking {dlb}:
    skill_rank = ((skill_lev < 8) ? 0 :
                  (skill_lev < 13) ? 1 : (skill_lev < 21) ? 2 : 3);

    // increment rank by one to "skip" skill name in array {dlb}:
    skill_rank++;

    if (best_skill < NUM_SKILLS)
        tempstr = skills[best_skill][skill_rank];

    return ((tempstr == NULL) ? "Invalid Title" : tempstr);
}                               // end skill_title()

const char *player_title( void )
{
    const unsigned char best = best_skill( SK_FIGHTING, (NUM_SKILLS - 1), 99 );
    return (skill_title( best, you.skills[ best ] ));
}                               // end player_title()

unsigned char best_skill(unsigned char min_skill, unsigned char max_skill,
                         unsigned char excl_skill)
{
    unsigned char ret = SK_FIGHTING;
    unsigned int highest_points = 0;

    for (int i = max_skill; i >= min_skill; i--)    // careful!!!
    {
        if (i == excl_skill)
            continue;

        // Skill points aquired are adjusted for species difficulty here...
        // ie. 100 pts at difficulty 200 is worth 50 and not as good as
        // 100 points at difficulty 100.  This should be better than the
        // old system which just compared the overall level, resulting
        // cases where the order of the skills was important (and annoyingly
        // so in cases where one skill is at 9(0) and a 9(9) is chosen
        // solely because it clobered the first).  Ties should be fairly
        // rare here, so we're pretty safe in ignoring them. -- bwr
        const unsigned int points = (you.skill_points[i] * 100)
                                        / species_skills(i, you.species);

        if (points >= highest_points)
        {
            ret = i;
            highest_points = points;
        }
    }

    return (ret);
}                               // end best_skill()


int calc_hp(void)
{
    int hitp;

    hitp = (you.base_hp - 5000) + (you.base_hp2 - 5000);
    hitp += (you.experience_level * you.skills[SK_FIGHTING]) / 5;

    // being berserk makes you resistant to damage. I don't know why.
    if (you.berserker)
    {
        hitp *= 15;
        hitp /= 10;
    }

    // some transformations give you extra hp
    if (you.duration[DUR_TRANSFORMATION])
    {
        switch (you.attribute[ATTR_TRANSFORMATION])
        {
        case TRAN_STATUE:
            hitp *= 15;
            hitp /= 10;
            break;
        case TRAN_ICE_BEAST:
            hitp *= 12;
            hitp /= 10;
            break;
        case TRAN_DRAGON:
            hitp *= 16;
            hitp /= 10;
            break;
        }
    }

    // frail and robust mutations
    hitp *= (10 + you.mutation[MUT_ROBUST] - you.mutation[MUT_FRAIL]);
    hitp /= 10;

    you.hp_max = hitp;

    deflate_hp(you.hp_max, false);

    return hitp;
}                               // end calc_hp()


int calc_mp(void)
{
    int enp;

    enp = (you.base_magic_points - 5000) + (you.base_magic_points2 - 5000);

    int spell_extra = you.experience_level * you.skills[SK_SPELLCASTING] / 6;
    int invoc_extra = you.experience_level * you.skills[SK_INVOCATIONS] / 4;;

    enp += ((invoc_extra > spell_extra) ? invoc_extra : spell_extra);

    /* if (enp > 21) enp = ((enp - 27) / 2) + 27;
       if (enp > 36) enp = ((enp - 36) / 2) + 36;
       if (enp > 49) enp = ((enp - 49) / 2) + 49; */

    if (enp > 18)               // nested if's rather than stacked 'em
    {                           // uglier than before but slightly
        enp = ((enp - 18) / 2) + 18;    // more efficient 16jan2000 {dlb}

        if (enp > 27)
        {
            enp = ((enp - 27) / 2) + 27;

            if (enp > 36)
            {
                enp = ((enp - 36) / 2) + 36;

                if (enp > 49)
                    enp = 49;
            }
        }
    }

    you.max_magic_points = enp;

    if (you.magic_points > you.max_magic_points)
        you.magic_points = you.max_magic_points;

    you.redraw_magic_points = 1;

    return enp;
}                               // end calc_mp()


unsigned int skill_exp_needed(int lev)
{
    lev--;
    switch (lev)
    {
    case 0:
        return 0;               // old:   0
    case 1:
        return 200;             // old:  20
    case 2:
        return 300;             // old:  30
    case 3:
        return 500;             // old:  50
    case 4:
        return 750;             // old:  75
    case 5:
        return 1050;            // old: 105
    case 6:
        return 1350;            // old: 145
    case 7:
        return 1700;            // old: 200
    case 8:
        return 2100;            // old: 275
    case 9:
        return 2550;            // old: 355
    case 10:
        return 3150;            // old: 440
    case 11:
        return 3750;            // old: 560
    case 12:
        return 4400;            // old: 680
    case 13:
        return 5250;            // old: 850
    default:
        return 6200 + 1800 * (lev - 14);
        // old: 1100 + 300 * (lev - 14)
        // older: 1200 * (lev - 11) + ((lev - 11) * (lev - 11));// * (lev - 11))
    }

    return 0;
}


int species_skills(char skill, char species)
{
    if (skill == SK_SPELLCASTING)     /* Spellcasting requires more practice */
        return (spec_skills[species - 1][skill] * 130) / 100;
    else if (skill == SK_INVOCATIONS) /* Invocations requires less */
        return (spec_skills[species - 1][skill] * 70) / 100;
    else
        return spec_skills[species - 1][skill];
}                               // end species_skills()

// new: inform player if they need more throwing skill (GDL)
void wield_warning(bool newWeapon)
{
    // hold weapon name
    char wepstr[100];
    char wepstr2[50];

    // early out - no weapon
    if (you.equip[EQ_WEAPON] == -1)
         return;

    if (newWeapon)
        strcpy(wepstr, "this ");
    else
        strcpy(wepstr, "your ");

    int wepType  = you.inv_type[you.equip[EQ_WEAPON]];

    // early out - don't warn for non-weapons
    if (you.inv_class[you.equip[EQ_WEAPON]] != OBJ_WEAPONS)
        return;

    // put the standard wep name in.
    standard_name_weap(wepType, wepstr2);
    strcat(wepstr, wepstr2);
    strcat(wepstr, ".");

    // only warn about str/dex for non-launcher weapons
    if (!launches_things(wepType))
    {
#ifdef USE_NEW_COMBAT_STATS
        const int stat_bonus = effective_stat_bonus();

        strcpy( info, "Your relatively low " );
        strcat( info, (you.strength < you.dex) ? "strength " : "dexterity " );

        if (stat_bonus <= -2)
        {
            strcat( info, "limits the use of " );
            strcat( info, wepstr );
            mpr( info, MSGCH_WARN );
        }
        else if (stat_bonus <= -5)
        {
            strcat( info, "severely limits the use of " );
            strcat( info, wepstr );
            mpr( info, MSGCH_WARN );
        }
#endif
        return;
    }

    // must be a launcher
    int effSkill = you.skills[SK_THROWING] * 2 + 1;
    int shoot_skill = 0;

    switch (wepType)
    {
        case WPN_SLING:
            shoot_skill = you.skills[SK_SLINGS];
            break;
        case WPN_BOW:
            shoot_skill = you.skills[SK_BOWS];
            break;
        case WPN_CROSSBOW:
        case WPN_HAND_CROSSBOW:
            shoot_skill = you.skills[SK_CROSSBOWS];
            break;
        default:
            shoot_skill = 0;
            break;
    }

    if (shoot_skill > effSkill)
    {
        strcpy( info, "Your low throwing skill limits the effectiveness of ");
        strcat( info, wepstr );
        mpr( info, MSGCH_WARN );
    }
}

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
#include <ctype.h>
#include <string.h>

#ifdef DOS
#include <conio.h>
#endif

#include "globals.h"
#include "externs.h"
#include "fight.h"
#include "itemprop.h"
#include "player.h"
#include "randart.h"
#include "religion.h"
#include "skills.h"
#include "stuff.h"
#include "transfor.h"
#include "view.h"

// This controls the resolution of percentage to next skill level.
// This should be evenly divisible by 100 (1,2,4,5,10,20,25,50,100).
// For the traditional resolution use 10.
#define SKILL_DISPLAY_RESOLUTION        5

/* jmf: some references for words I used below:
 Peltast http://www.geocities.com/Athens/Aegean/9659/shields_main.htm
 Phalangite http://www.users.cts.com/funtv/j/jjartist/EpiroteScenario1.htm
 Yeoman: http://snt.student.utwente.nl/campus/sagi/artikel/longbow/longbow.html
*/

// Note:  Even though %s could be used with most of these, remember that
// the character's race will be listed on the next line.  Its only really
// intended for cases where things might be really awkward without it. -- bwr

#define TITLE_2ND_LEVEL     8
#define TITLE_3RD_LEVEL    15
#define TITLE_4TH_LEVEL    21
#define TITLE_5TH_LEVEL    27

const char *skills[MAX_SKILLS][6] = {
    {"Fighting", "Skirmisher", "Grunt", "Veteran", "Warrior", "Slayer"},      // 0
    {"Short Blades", "Stabber", "Cutter", "Knifefighter", "Eviscerator", "Blademaster"},
    {"Long Blades", "Slasher", "Slicer", "Fencer", "Swordfighter", "Swordmaster"},
    {NULL},                     //  3- was: great swords {dlb}
    {"Axes", "Chopper", "Cleaver", "Hacker", "Severer", "Axe Maniac"},
    {"Maces & Flails", "Basher", "Cudgeler", "Shatterer", "Bludgeoner", "Skullcrusher"},   // 5
    {"Polearms", "Spear-Bearer", "Pike-%s", "Phalangite", "Lancer", "Halberdier"},
    {"Staves", "Twirler", "Cruncher", "Smasher", "Stickfighter", "Skullbreaker"},

    {"Slings", "Vandal", "Slinger", "Whirler", "Crazy %s", "Very Crazy %s"},
    {"Bows", "Shooter", "Yeoman", "Archer", "Merry %s", "Merry %s"},
    {"Crossbows", "Shooter", "Sharpshooter", "Archer", "%s Ballista", "%s Ballista"},     // 10
    {"Darts", "Dart Thrower", "Hurler", "Hurler, First Class", "%s Darts Champion", "Universal Darts Champion"},
    {"Ranged Combat", "Chucker", "Thrower", "Deadly Accurate", "Hawkeye", "Sniper"},

    {"Armour", "Covered", "Protected", "Tortoise", "Impregnable", "Invulnerable"},
    {"Dodging", "Ducker", "Dodger", "Nimble", "Spry", "Acrobat"},
    {"Stealth", "Footpad", "Sneak", "Covert", "Unseen", "Imperceptible"},
    {"Stabbing", "Miscreant", "Blackguard", "Backstabber", "Cutthroat", "Politician"},
    {"Shields", "Shield-Bearer", "Blocker", "%s Barricade", "Peltast", "Hoplite"},
    {"Traps & Doors", "Disarmer", "Trapper", "Architect", "Engineer", "Dungeon Master"},

    // STR based fighters, for DEX/martial arts titles see below
    {"Unarmed Combat", "Ruffian", "Grappler", "Brawler", "Wrestler", "Boxer" },

    {NULL},                     // 20- empty
    {NULL},                     // 21- empty
    {NULL},                     // 22- empty
    {NULL},                     // 23- empty
    {NULL},                     // 24- empty

    {"Spellcasting", "Magician", "Thaumaturge", "Eclecticist", "Sorcerer", "Archmage"},     // 25
    {"Conjurations", "Ruinous", "Conjurer", "Destroyer", "Devastator", "Annihilator"},
    {"Enchantments", "Charm-Maker", "Infuser", "Bewitcher", "Enchanter", "Spellbinder"},
    {"Summonings", "Caller", "Summoner", "Convoker", "Demonologist", "Hellbinder"},
    {"Necromancy", "Grave Robber", "Reanimator", "Necromancer", "Thanatomancer", "%s of Death"},
    {"Translocations", "Jumper", "Blinker", "Shifter", "Portalist", "Plane Walker"},       // 30
    {"Transmigration", "Changer", "Transmogrifier", "Transformer", "Alchemist", "Transmuter"},
    {"Divinations", "Seer", "Soothsayer", "Diviner", "Augur", "Oracle"},

    {"Fire Magic", "Firebug", "Arsonist", "Scorcher", "Pyromancer", "Infernalist"},
    {"Ice Magic", "Chiller", "Frost Mage", "Ice Mage", "Cryomancer", "Englaciator"},
    {"Air Magic", "Wind Mage", "Cloud Mage", "Air Mage", "Sky Mage", "Storm Mage"}, // 35
    {"Earth Magic", "Digger", "Geomancer", "Earth Mage", "Metallomancer", "Petrodigitator"},
    {"Poison Magic", "Stinger", "Tainter", "Polluter", "Poisoner", "Envenomancer"},

    {"Invocations", "Believer", "Servant", "Worldly Agent", "Theurge", "Avatar"},  // 38
    {"Evocations", "Charlatan", "Prestidigitator", "Fetichist", "Evocator", "Talismancer"}, // 39

/*NOTE: If more skills are added, must change ranges in level_change() in player.cc */
/*{"",             "", "", "", ""}, */

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

const char *martial_arts_titles[6] =
    {"Unarmed Combat", "Martial Artist", "Black Belt", "Sensei", "Master", "Grand Master"};


/* Note that this (humans have 100 for all skills) is assumed in the
   level_change function in player.cc, if CLASSES is def'd

   3.10: but it never is, and CLASSES is probably broken now. Anyway,
   the Spellcasting skill (25) is actually about 130% of what is shown here.
 */
const int spec_skills[ NUM_SPECIES ][40] = {
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
     100,                       // SK_RANGED_COMBAT
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
     100,                       // SK_EVOCATIONS
     },

    {                           // SP_ELF (2)
     120,                       // SK_FIGHTING
     80,                        // SK_SHORT_BLADES
     80,                        // SK_LONG_SWORDS
     110,                       // SK_UNUSED_1
     130,                       // SK_AXES
     110,                       // SK_MACES_FLAILS
     130,                       // SK_POLEARMS
     100,                       // SK_STAVES
     120,                       // SK_SLINGS
     60,                        // SK_BOWS
     100,                       // SK_CROSSBOWS
     90,                        // SK_DARTS
     80,                        // SK_RANGED_COMBAT
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
     80,                        // SK_EVOCATIONS
     },

    {                           // SP_HIGH_ELF (3)
     100,                       // SK_FIGHTING
     70,                        // SK_SHORT_BLADES
     70,                        // SK_LONG_SWORDS
     115,                       // SK_UNUSED_1
     140,                       // SK_AXES
     120,                       // SK_MACES_FLAILS
     150,                       // SK_POLEARMS
     100,                       // SK_STAVES
     140,                       // SK_SLINGS
     60,                        // SK_BOWS
     100,                       // SK_CROSSBOWS
     90,                        // SK_DARTS
     80,                        // SK_RANGED_COMBAT
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
     100,                       // SK_EVOCATIONS
     },

    {                           // SP_GREY_ELF (4)
     140,                       // SK_FIGHTING
     90,                        // SK_SHORT_BLADES
     95,                        // SK_LONG_SWORDS
     120,                       // SK_UNUSED_1
     150,                       // SK_AXES
     130,                       // SK_MACES_FLAILS
     160,                       // SK_POLEARMS
     100,                       // SK_STAVES
     130,                       // SK_SLINGS
     70,                        // SK_BOWS
     100,                       // SK_CROSSBOWS
     90,                        // SK_DARTS
     80,                        // SK_RANGED_COMBAT
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
     90,                        // SK_EVOCATIONS
     },

    {                           // SP_DEEP_ELF (5)
     150,                       // SK_FIGHTING
     100,                       // SK_SHORT_BLADES
     105,                       // SK_LONG_SWORDS
     120,                       // SK_UNUSED_1
     165,                       // SK_AXES
     150,                       // SK_MACES_FLAILS
     165,                       // SK_POLEARMS
     100,                       // SK_STAVES
     135,                       // SK_SLINGS
     74,                        // SK_BOWS
     75,                        // SK_CROSSBOWS
     75,                        // SK_DARTS
     80,                        // SK_RANGED_COMBAT
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
     90,                        // SK_EVOCATIONS
     },

    {                           // SP_SLUDGE_ELF (6)
     80,                        // SK_FIGHTING
     110,                       // SK_SHORT_BLADES
     110,                       // SK_LONG_SWORDS
     110,                       // SK_UNUSED_1
     140,                       // SK_AXES
     100,                       // SK_MACES_FLAILS
     100,                       // SK_POLEARMS
     100,                       // SK_STAVES
     100,                       // SK_SLINGS
     100,                       // SK_BOWS
     100,                       // SK_CROSSBOWS
     100,                       // SK_DARTS
     70,                        // SK_RANGED_COMBAT
     140,                       // SK_ARMOUR
     70,                        // SK_DODGING
     75,                        // SK_STEALTH
     100,                       // SK_STABBING
     130,                       // SK_SHIELDS
     100,                       // SK_TRAPS_DOORS
     80,                        // SK_UNARMED_COMBAT
     100,                       // undefined
     100,                       // undefined
     100,                       // undefined
     100,                       // undefined
     100,                       // undefined
     90,                        // SK_SPELLCASTING
     130,                       // SK_CONJURATIONS
     130,                       // SK_ENCHANTMENTS
     90,                        // SK_SUMMONINGS
     90,                        // SK_NECROMANCY
     100,                       // SK_TRANSLOCATIONS
     70,                        // SK_TRANSMIGRATION
     130,                       // SK_DIVINATIONS
     100,                       // SK_FIRE_MAGIC
     100,                       // SK_ICE_MAGIC
     120,                       // SK_AIR_MAGIC
     80,                        // SK_EARTH_MAGIC
     90,                        // SK_POISON_MAGIC
     100,                       // SK_INVOCATIONS
     110,                       // SK_EVOCATIONS
     },

    {                           // SP_HILL_DWARF (7)
     70,                        // SK_FIGHTING
     80,                        // SK_SHORT_BLADES
     80,                        // SK_LONG_SWORDS
     100,                       // SK_UNUSED_1
     60,                        // SK_AXES
     100,                       // SK_MACES_FLAILS
     110,                       // SK_POLEARMS
     130,                       // SK_STAVES
     130,                       // SK_SLINGS
     150,                       // SK_BOWS
     120,                       // SK_CROSSBOWS
     120,                       // SK_DARTS
     120,                       // SK_RANGED_COMBAT
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
     60,                        // SK_EVOCATIONS
     },

    {                           // SP_MOUNTAIN_DWARF (8)
     70,                        // SK_FIGHTING
     90,                        // SK_SHORT_BLADES
     90,                        // SK_LONG_SWORDS
     100,                       // SK_UNUSED_1
     70,                        // SK_AXES
     100,                       // SK_MACES_FLAILS
     110,                       // SK_POLEARMS
     120,                       // SK_STAVES
     125,                       // SK_SLINGS
     140,                       // SK_BOWS
     100,                       // SK_CROSSBOWS
     120,                       // SK_DARTS
     115,                       // SK_RANGED_COMBAT
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
     70,                        // SK_EVOCATIONS
     },

    {                           // SP_HALFLING (9)
     120,                       // SK_FIGHTING
     60,                        // SK_SHORT_BLADES
     100,                       // SK_LONG_SWORDS
     130,                       // SK_UNUSED_1
     140,                       // SK_AXES
     140,                       // SK_MACES_FLAILS
     160,                       // SK_POLEARMS
     130,                       // SK_STAVES
     50,                        // SK_SLINGS
     70,                        // SK_BOWS
     90,                        // SK_CROSSBOWS
     50,                        // SK_DARTS
     60,                        // SK_RANGED_COMBAT
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
     90,                        // SK_EVOCATIONS
     },

    {                           // SP_HILL_ORC (10)
     70,                        // SK_FIGHTING
     100,                       // SK_SHORT_BLADES
     80,                        // SK_LONG_SWORDS
     80,                        // SK_UNUSED_1
     80,                        // SK_AXES
     80,                        // SK_MACES_FLAILS
     80,                        // SK_POLEARMS
     110,                       // SK_STAVES
     130,                       // SK_SLINGS
     120,                       // SK_BOWS
     120,                       // SK_CROSSBOWS
     130,                       // SK_DARTS
     130,                       // SK_RANGED_COMBAT
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
     100,                       // SK_EVOCATIONS
     },

    {                           // SP_KOBOLD (11)
     80,                        // SK_FIGHTING
     60,                        // SK_SHORT_BLADES
     100,                       // SK_LONG_SWORDS
     120,                       // SK_UNUSED_1
     120,                       // SK_AXES
     140,                       // SK_MACES_FLAILS
     150,                       // SK_POLEARMS
     110,                       // SK_STAVES
     70,                        // SK_SLINGS
     80,                        // SK_BOWS
     90,                        // SK_CROSSBOWS
     50,                        // SK_DARTS
     60,                        // SK_RANGED_COMBAT
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
     80,                        // SK_EVOCATIONS
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
     140,                       // SK_RANGED_COMBAT
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
     140,                       // SK_EVOCATIONS
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
     120,                       // SK_RANGED_COMBAT
     150,                       // SK_ARMOUR
     150,                       // SK_DODGING
     80,                        // SK_STEALTH
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
     80,                        // SK_POISON_MAGIC
     100,                       // SK_INVOCATIONS
     100,                       // SK_EVOCATIONS
     },

    {                           // SP_GNOME (14)
     100,                       // SK_FIGHTING
     75,                        // SK_SHORT_BLADES
     110,                       // SK_LONG_SWORDS
     130,                       // SK_UNUSED_1
     110,                       // SK_AXES
     130,                       // SK_MACES_FLAILS
     140,                       // SK_POLEARMS
     130,                       // SK_STAVES
     80,                        // SK_SLINGS
     100,                       // SK_BOWS
     90,                        // SK_CROSSBOWS
     60,                        // SK_DARTS
     100,                       // SK_RANGED_COMBAT
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
     60,                        // SK_EVOCATIONS
     },

    {                           // SP_OGRE (15)
     100,                       // SK_FIGHTING
     140,                       // SK_SHORT_BLADES
     120,                       // SK_LONG_SWORDS
     120,                       // SK_UNUSED_1
     120,                       // SK_AXES
      90,                       // SK_MACES_FLAILS
     120,                       // SK_POLEARMS
     120,                       // SK_STAVES
     150,                       // SK_SLINGS
     150,                       // SK_BOWS
     180,                       // SK_CROSSBOWS
     150,                       // SK_DARTS
     130,                       // SK_RANGED_COMBAT
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
     170,                       // SK_EVOCATIONS
     },

    {                           // SP_TROLL (16)
     140,                       // SK_FIGHTING
     150,                       // SK_SHORT_BLADES
     150,                       // SK_LONG_SWORDS
     150,                       // SK_UNUSED_1
     150,                       // SK_AXES
     130,                       // SK_MACES_FLAILS
     150,                       // SK_POLEARMS
     150,                       // SK_STAVES
     180,                       // SK_SLINGS
     180,                       // SK_BOWS
     180,                       // SK_CROSSBOWS
     180,                       // SK_DARTS
     130,                       // SK_RANGED_COMBAT
     150,                       // SK_ARMOUR
     130,                       // SK_DODGING
     250,                       // SK_STEALTH
     150,                       // SK_STABBING
     150,                       // SK_SHIELDS
     200,                       // SK_TRAPS_DOORS
     130,                       // SK_UNARMED_COMBAT
     100,                       // undefined
     100,                       // undefined
     100,                       // undefined
     100,                       // undefined
     100,                       // undefined
     200,                       // SK_SPELLCASTING
     160,                       // SK_CONJURATIONS
     200,                       // SK_ENCHANTMENTS
     160,                       // SK_SUMMONINGS
     150,                       // SK_NECROMANCY
     160,                       // SK_TRANSLOCATIONS
     160,                       // SK_TRANSMIGRATION
     200,                       // SK_DIVINATIONS
     160,                       // SK_FIRE_MAGIC
     160,                       // SK_ICE_MAGIC
     200,                       // SK_AIR_MAGIC
     130,                       // SK_EARTH_MAGIC
     160,                       // SK_POISON_MAGIC
     150,                       // SK_INVOCATIONS
     180,                       // SK_EVOCATIONS
     },

    {                           // SP_OGRE_MAGE (17)
     100,                       // SK_FIGHTING
     140,                       // SK_SHORT_BLADES
     120,                       // SK_LONG_SWORDS
     120,                       // SK_UNUSED_1
     120,                       // SK_AXES
     100,                       // SK_MACES_FLAILS
     100,                       // SK_POLEARMS
     100,                       // SK_STAVES
     150,                       // SK_SLINGS
     150,                       // SK_BOWS
     150,                       // SK_CROSSBOWS
     150,                       // SK_DARTS
     150,                       // SK_RANGED_COMBAT
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
     100,                       // SK_EVOCATIONS
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
     120,                       // SK_RANGED_COMBAT
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
     100,                       // SK_EVOCATIONS
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
     120,                       // SK_RANGED_COMBAT
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
     100,                       // SK_EVOCATIONS
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
     120,                       // SK_RANGED_COMBAT
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
     100,                       // SK_EVOCATIONS
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
     120,                       // SK_RANGED_COMBAT
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
     100,                       // SK_EVOCATIONS
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
     120,                       // SK_RANGED_COMBAT
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
     100,                       // SK_EVOCATIONS
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
     120,                       // SK_RANGED_COMBAT
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
     100,                       // SK_EVOCATIONS
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
     120,                       // SK_RANGED_COMBAT
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
     90,                        // SK_ENCHANTMENTS
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
     90,                        // SK_EVOCATIONS
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
     120,                       // SK_RANGED_COMBAT
     200,                       // SK_ARMOUR
     80,                        // SK_DODGING
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
     100,                       // SK_EVOCATIONS
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
     120,                       // SK_RANGED_COMBAT
     200,                       // SK_ARMOUR
     80,                        // SK_DODGING
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
     90,                        // SK_EVOCATIONS
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
     120,                       // SK_RANGED_COMBAT
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
     100,                       // SK_EVOCATIONS
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
     120,                       // SK_RANGED_COMBAT
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
     100,                       // SK_EVOCATIONS
     },

    // This table is currently used for draconians who do not have
    // colour (aka the Brown draconians), be careful before removing.
    {                           // SP_BASE_DRACONIAN (29)
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
     120,                       // SK_RANGED_COMBAT
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
     100,                       // SK_EVOCATIONS
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
     60,                        // SK_RANGED_COMBAT
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
     130,                       // SK_EVOCATIONS
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
     110,                       // SK_RANGED_COMBAT
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
     110,                       // SK_INVOCATIONS
     110,                       // SK_EVOCATIONS
     },

    {                           // SP_SPRIGGAN (32)
     150,                       // SK_FIGHTING
     90,                        // SK_SHORT_BLADES
     140,                       // SK_LONG_SWORDS
     160,                       // SK_UNUSED_1
     160,                       // SK_AXES
     160,                       // SK_MACES_FLAILS
     180,                       // SK_POLEARMS
     150,                       // SK_STAVES
     70,                        // SK_SLINGS
     70,                        // SK_BOWS
     100,                       // SK_CROSSBOWS
     70,                        // SK_DARTS
     90,                        // SK_RANGED_COMBAT
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
     70,                        // SK_EVOCATIONS
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
     90,                        // SK_RANGED_COMBAT
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
     170,                       // SK_EVOCATIONS
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
     110,                       // SK_RANGED_COMBAT
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
     100,                       // SK_FIRE_MAGIC
     110,                       // SK_ICE_MAGIC
     110,                       // SK_AIR_MAGIC
     110,                       // SK_EARTH_MAGIC
     100,                       // SK_POISON_MAGIC
     80,                        // SK_INVOCATIONS
     110,                       // SK_EVOCATIONS
     },

    {                           // SP_GHOUL (35)
     130,                       // SK_FIGHTING
     130,                       // SK_SHORT_BLADES
     130,                       // SK_LONG_SWORDS
     130,                       // SK_UNUSED_1
     130,                       // SK_AXES
     130,                       // SK_MACES_FLAILS
     130,                       // SK_POLEARMS
     130,                       // SK_STAVES
     150,                       // SK_SLINGS
     150,                       // SK_BOWS
     150,                       // SK_CROSSBOWS
     150,                       // SK_DARTS
     150,                       // SK_RANGED_COMBAT
     130,                       // SK_ARMOUR
     130,                       // SK_DODGING
     120,                       // SK_STEALTH
     130,                       // SK_STABBING
     130,                       // SK_SHIELDS
     130,                       // SK_TRAPS_DOORS
     130,                       // SK_UNARMED_COMBAT
     120,                       // undefined
     120,                       // undefined
     120,                       // undefined
     120,                       // undefined
     120,                       // undefined
     170,                       // SK_SPELLCASTING
     150,                       // SK_CONJURATIONS
     150,                       // SK_ENCHANTMENTS
     140,                       // SK_SUMMONINGS
     120,                       // SK_NECROMANCY
     140,                       // SK_TRANSLOCATIONS
     140,                       // SK_TRANSMIGRATION
     140,                       // SK_DIVINATIONS
     170,                       // SK_FIRE_MAGIC
     140,                       // SK_ICE_MAGIC
     170,                       // SK_AIR_MAGIC
     140,                       // SK_EARTH_MAGIC
     120,                       // SK_POISON_MAGIC
     130,                       // SK_INVOCATIONS
     170,                       // SK_EVOCATIONS
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
     90,                        // SK_RANGED_COMBAT
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
     100,                       // SK_EVOCATIONS
     },

    {                           // SP_MERFOLK (37)
     80,                        // SK_FIGHTING
     70,                        // SK_SHORT_BLADES
     100,                       // SK_LONG_SWORDS
     100,                       // SK_UNUSED_1
     150,                       // SK_AXES
     150,                       // SK_MACES_FLAILS
     60,                        // SK_POLEARMS
     130,                       // SK_STAVES
     150,                       // SK_SLINGS
     140,                       // SK_BOWS
     140,                       // SK_CROSSBOWS
     100,                       // SK_DARTS
     100,                       // SK_RANGED_COMBAT
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
     100,                       // SK_EVOCATIONS
     },

    {                           // SP_GIANT (38)
     130,                       // SK_FIGHTING
     200,                       // SK_SHORT_BLADES
     150,                       // SK_LONG_SWORDS
     200,                       // SK_UNUSED_1
     150,                       // SK_AXES
     120,                       // SK_MACES_FLAILS
     130,                       // SK_POLEARMS
     150,                       // SK_STAVES
     200,                       // SK_SLINGS
     200,                       // SK_BOWS
     200,                       // SK_CROSSBOWS
     200,                       // SK_DARTS
     130,                       // SK_RANGED_COMBAT
     180,                       // SK_ARMOUR
     150,                       // SK_DODGING
     200,                       // SK_STEALTH
     200,                       // SK_STABBING
     160,                       // SK_SHIELDS
     200,                       // SK_TRAPS_DOORS
     120,                       // SK_UNARMED_COMBAT
     100,                       // undefined
     100,                       // undefined
     100,                       // undefined
     100,                       // undefined
     100,                       // undefined
     200,                       // SK_SPELLCASTING
     180,                       // SK_CONJURATIONS
     180,                       // SK_ENCHANTMENTS
     200,                       // SK_SUMMONINGS
     200,                       // SK_NECROMANCY
     200,                       // SK_TRANSLOCATIONS
     200,                       // SK_TRANSMIGRATION
     200,                       // SK_DIVINATIONS
     170,                       // SK_FIRE_MAGIC
     170,                       // SK_ICE_MAGIC
     170,                       // SK_AIR_MAGIC
     170,                       // SK_EARTH_MAGIC
     200,                       // SK_POISON_MAGIC
     150,                       // SK_INVOCATIONS
     180,                       // SK_EVOCATIONS
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
        120,               // SK_RANGED_COMBAT
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
        100,               // SK_EVOCATIONS
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

static skill_type Skill_Order[] =
{
    SK_FIGHTING, SK_SHORT_BLADES, SK_LONG_SWORDS, SK_AXES, SK_MACES_FLAILS,
    SK_POLEARMS, SK_STAVES, SK_UNARMED_COMBAT,
    SK_BLANK_LINE,

    SK_RANGED_COMBAT, SK_DARTS, SK_SLINGS, SK_BOWS, SK_CROSSBOWS,
    SK_BLANK_LINE,

    SK_ARMOUR, SK_SHIELDS, SK_DODGING, SK_STABBING, SK_STEALTH, SK_TRAPS_DOORS,
    SK_COLUMN_BREAK,

    SK_SPELLCASTING, SK_CONJURATIONS, SK_ENCHANTMENTS, SK_SUMMONINGS,
    SK_NECROMANCY, SK_TRANSLOCATIONS, SK_TRANSMIGRATION, SK_DIVINATIONS,
    SK_FIRE_MAGIC, SK_ICE_MAGIC, SK_AIR_MAGIC, SK_EARTH_MAGIC, SK_POISON_MAGIC,
    SK_BLANK_LINE,

    SK_INVOCATIONS, SK_EVOCATIONS,
};

static int Skill_Order_Size = sizeof(Skill_Order) / sizeof(skill_type);

void show_skills(void)
{
    int num_skills;

    const int num_lines = get_number_of_lines();

#ifdef DOS_TERM
    window(1, 1, 80, 25);
    char buffer[4600];

    gettext(1, 1, 80, 25, buffer);
#endif

    clrscr();

reprint_stuff:
    num_skills = 0;

    gotoxy(1, 1);
    textcolor(LIGHTGREY);

#if DEBUG_DIAGNOSTICS
    cprintf( "You have %d points of unallocated experience (cost lvl %d; total %d)." EOL EOL,
             you.exp_available, you.skill_cost_level, you.total_skill_points );
#else
    cprintf(" You have %d points of unallocated experience." EOL EOL,
            you.exp_available );
#endif

    skill_type  toggle_index[ NUM_SKILLS ];

    char scrln = 3, scrcol = 1;

    // Don't want the help line to appear too far down a big window.
    int bottom_line = ((num_lines > 30) ? 30 : num_lines);

    bool last_line_blank = true;

    for (int x = 0; x < Skill_Order_Size; x++)
    {
        // Detect blank lines (and convert column break to one
        // if we're already in the second column).
        if (!last_line_blank
            && ((Skill_Order[x] == SK_COLUMN_BREAK && scrcol == 41)
                || Skill_Order[x] == SK_BLANK_LINE))
        {
            if (scrln > 3)      // never on the first row
            {
                scrln++;
                last_line_blank = true;
            }
        }

        // Do Column break (must come after line break).
        if ((Skill_Order[x] == SK_COLUMN_BREAK && scrcol == 1)
            || scrln > bottom_line - 2)
        {
            scrln = 3;
            scrcol = 41;
        }

        if (Skill_Order[x] == SK_COLUMN_BREAK
            || Skill_Order[x] == SK_BLANK_LINE)
        {
            continue;
        }

        const skill_type sk = Skill_Order[x];

#if (!DEBUG_DIAGNOSTICS)
        // In diagnostic mode we show skills at 0
        if (you.skills[sk] > 0)
#endif
        {
            last_line_blank = false;
            gotoxy( scrcol, scrln );

            if (you.practise_skill[sk] == 0 || you.skills[sk] == 0)
                textcolor(DARKGREY);
            else
                textcolor(LIGHTGREY);

            if (you.skills[sk] >= 27)
            {
                textcolor(YELLOW);
                putch(' ');
            }
            else
            {
#if DEBUG_DIAGNOSTICS
                if (you.skills[sk] == 0)
                    putch(' ');
                else
                {
                    toggle_index[num_skills] = sk;
                    putch( index_to_letter(num_skills) );
                    num_skills++;
                }
#else
                toggle_index[num_skills] = sk;
                putch( index_to_letter(num_skills) );
                num_skills++;
#endif
            }

            cprintf( " %c %-14s Skill %2d",
                     (you.skills[sk] == 0 || you.skills[sk] >= 27) ? ' ' :
                     (you.practise_skill[sk] == 0)                 ? '-' : '+',
                     skills[sk][0], you.skills[sk] );

            textcolor(BLUE);

#if DEBUG_DIAGNOSTICS
            cprintf( " %5d", you.skill_points[sk] );
#endif

            if (you.skills[sk] < 27)
            {
                const int spec = species_skills( sk, you.species );
                const int next = skill_exp_needed( you.skills[sk] + 2 ) * spec;
                const int prev = skill_exp_needed( you.skills[sk] + 1 ) * spec;

                int pcent = SKILL_DISPLAY_RESOLUTION *
                                ((100 / SKILL_DISPLAY_RESOLUTION) *
                         (100 * you.skill_points[sk] - prev) / (next - prev));

                if (Options.old_skill_countdown)
                    pcent = (95 - pcent) / 10;
                else if (Options.skill_countdown)
                    pcent = 100 - pcent;

                cprintf( Options.old_skill_countdown ? " (%d)" : " (%2d%%)",
                         pcent );
            }

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
            const int index = letter_to_index( get_thing );

            if (index < num_skills)
            {
                const skill_type sk = toggle_index[index];
                you.practise_skill[sk] = !you.practise_skill[sk];
            }

            goto reprint_stuff;
        }
    }

#ifdef DOS_TERM
    puttext(1, 1, 80, 25, buffer);
#endif
    return;
}


const char *skill_name( int which_skill )
{
    return (skills[which_skill][0]);
}                               // end skill_name()


const char *skill_title( int skill, int skill_lev,
                         // these used be ghosts/hiscores:
                         int species, int str, int dex,
                         int god, int piety, int penance )
{
    unsigned char skill_rank;
    const char *tempstr = NULL;

    static char title_buff[80];

    UNUSED( piety );

    // paranoia
    if (skill == SK_UNUSED_1
        || (skill > SK_UNARMED_COMBAT && skill < SK_SPELLCASTING)
        || skill >= NUM_SKILLS)
    {
        return ("Adventurer");
    }

    // This function is also used for ghosts so we pass in all these,
    // but default to the current player.
    if (species == -1)
        species = you.species;

    if (str == -1)
        str = you.str;

    if (dex == -1)
        dex = you.dex;

    if (god == -1)
        god = you.religion;

    if (penance == -1)
        penance = player_under_penance();

    // translate skill level into skill ranking {dlb}:
    // increment rank by one to "skip" skill name in array {dlb}:
    skill_rank = ((skill_lev < TITLE_2ND_LEVEL) ? 1 :
                  (skill_lev < TITLE_3RD_LEVEL) ? 2 :
                  (skill_lev < TITLE_4TH_LEVEL) ? 3 :
                  (skill_lev < TITLE_5TH_LEVEL) ? 4
                                                : 5);

    if (skill < NUM_SKILLS)
    {
        switch (skill)
        {
        case SK_UNARMED_COMBAT:
            tempstr = (dex >= str) ? martial_arts_titles[skill_rank]
                                   : skills[skill][skill_rank];

            break;

        case SK_INVOCATIONS:
            if (god == GOD_NO_GOD)
                tempstr = "Godless";
            else if (penance)
                tempstr = "Fallen";
            else
                tempstr = skills[skill][skill_rank];
            break;

        default:
            tempstr = skills[skill][skill_rank];
            break;
        }
    }

    const char *const ptr = strchr( tempstr, '%' );
    const bool species_found = (ptr != NULL);

    if (species_found)
    {
        // need species name    // XXX: use transformations here?
        snprintf( title_buff, sizeof(title_buff), tempstr,
                  species_name( species, 0, true,
                                (ptr == tempstr && skill != SK_NECROMANCY) ) );

        // The above code only capitalises start-of-string racenames
        tempstr = title_buff;
    }

    return ((tempstr == NULL) ? "Invalid Title" : tempstr);
}                               // end skill_title()

const char *player_title( void )
{
    const int best = best_skill( SK_FIGHTING, (NUM_SKILLS - 1) );

    return (skill_title( best, you.skills[ best ] ));
}                               // end player_title()

int best_skill( int min_skill, int max_skill, int excl_skill )
{
    int ret = SK_FIGHTING;
    int best_skill_level = 0;
    int best_position = 1000;

    for (int i = min_skill; i <= max_skill; i++)    // careful!!!
    {
        if (i == excl_skill
            || i == SK_UNUSED_1
            || (i > SK_UNARMED_COMBAT && i < SK_SPELLCASTING))
        {
            continue;
        }

        if (you.skills[i] > best_skill_level)
        {
            ret = i;
            best_skill_level = you.skills[i];
            best_position = you.skill_order[i];

        }
        else if (you.skills[i] == best_skill_level
                && you.skill_order[i] < best_position)
        {
            ret = i;
            best_position = you.skill_order[i];
        }
    }

    return (ret);
}                               // end best_skill()

// Calculate the skill_order array from scratch.
//
// The skill order array is used for breaking ties in best_skill.
// This is done by ranking each skill by the order in which it
// has attained its current level (the values are the number of
// skills at or above that level when the current skill reached it).
//
// In this way, the skill which has been at a level for the longest
// is judged to be the best skill (thus, nicknames are sticky)...
// other skills will have to attain the next level higher to be
// considered a better skill (thus, the first skill to reach level 27
// becomes the character's final nickname).
//
// As for other uses of best_skill:  this method is still appropriate
// in that there is no additional advantage anywhere else in the game
// for partial skill levels.  Besides, its probably best if the player
// isn't able to micromanage at that level.  -- bwr
void init_skill_order( void )
{
    for (int i = SK_FIGHTING; i < NUM_SKILLS; i++)
    {
        if (i == SK_UNUSED_1
            || (i > SK_UNARMED_COMBAT && i < SK_SPELLCASTING))
        {
            you.skill_order[i] = MAX_SKILL_ORDER;
            continue;
        }

        const int i_diff = species_skills( i, you.species );
        const unsigned int i_points = (you.skill_points[i] * 100) / i_diff;

        you.skill_order[i] = 0;

        for (int j = SK_FIGHTING; j < NUM_SKILLS; j++)
        {
            if (i == j
                || j == SK_UNUSED_1
                || (j > SK_UNARMED_COMBAT && j < SK_SPELLCASTING))
            {
                continue;
            }

            const int j_diff = species_skills( j, you.species );
            const unsigned int j_points = (you.skill_points[j] * 100) / j_diff;

            if (you.skills[j] == you.skills[i]
                && (j_points > i_points
                    || (j_points == i_points && j > i)))
            {
                you.skill_order[i]++;
            }
        }
    }
}

int calc_hp_max( int new_hp_value )
{
    int hitp;

    hitp = (you.base_hp - 5000) + (you.base_hp2 - 5000);

    hitp += ((you.xp_level + 3) * skill_bump( SK_FIGHTING )) / 7;

    // XXX: too many multiplications!  1000+ HPs are possible
    // being berserk makes you resistant to damage. I don't know why.
    if (you.berserker)
    {
        hitp *= 15;
        hitp /= 10;
    }

    // some transformations give you extra hp
    hitp = (hitp * transform_hp_factor()) / 10;

    // frail and robust mutations
    hitp *= (10 + you.mutation[MUT_ROBUST] - you.mutation[MUT_FRAIL]);
    hitp /= 10;

    you.hp_max = hitp;

    if (new_hp_value != -1)
        you.hp = new_hp_value;

    if (you.hp > you.hp_max)
        you.hp = you.hp_max;

    set_redraw_status( REDRAW_HIT_POINTS );

    return (hitp);
}                               // end calc_hp_max()


int calc_mp_max( int new_mp_value )
{
    // base_magic_points2 accounts for species and magic potions
    int enp = (you.base_magic_points2 - 5000);

    // calculate bonuses for skill
    const int spell_extra = (you.xp_level * you.skills[SK_SPELLCASTING]) / 5;
    const int invoc_extra = (you.xp_level * you.skills[SK_INVOCATIONS]) / 7;

    // apply only the largest bonus
    if (spell_extra > invoc_extra)
        enp += spell_extra;
    else
        enp += invoc_extra;

    // This curve is more friendly for the early game (no curve until 12)
    // was you.max_magic_points = stepdown_value( enp, 9, 18, 45, -1 );
    you.max_magic_points = stepdown_value( enp, 12, 12, 48, -1 );

    // Yes, we really do want this duplication... this is so the stepdown
    // doesn't truncate before we apply the rotted base.  We're doing this
    // the nice way. -- bwr
    if (you.max_magic_points > 50)
        you.max_magic_points = 50;

    // this is our "rotted" base (applied after scaling):
    you.max_magic_points += (you.base_magic_points - 5000);

    // now applied after scaling so that power items are more useful -- bwr
    you.max_magic_points += player_magical_power();

    // reduce if power item takes us past normal max of 50
    if (you.max_magic_points > 50)
        you.max_magic_points = 50 + ((you.max_magic_points - 50) / 2);

    // don't allow negative
    if (you.max_magic_points < 0)
        you.max_magic_points = 0;

    // now set and limit magic points if required
    if (new_mp_value != -1)
        you.magic_points = new_mp_value;

    if (you.magic_points > you.max_magic_points)
        you.magic_points = you.max_magic_points;

    set_redraw_status( REDRAW_MAGIC_POINTS );

    return (you.max_magic_points);
}                               // end calc_mp_max()

unsigned int skill_exp_needed( int lev )
{
    int ret = 0;

    lev--;

    switch (lev)
    {
    case 0:
        break;
    case 1:
        ret = 200;             // old:  20
        break;
    case 2:
        ret = 300;             // old:  30
        break;
    case 3:
        ret = 500;             // old:  50
        break;
    case 4:
        ret = 750;             // old:  75
        break;
    case 5:
        ret = 1050;            // old: 105
        break;
    case 6:
        ret = 1350;            // old: 145
        break;
    case 7:
        ret = 1700;            // old: 200
        break;
    case 8:
        ret = 2100;            // old: 275
        break;
    case 9:
        ret = 2550;            // old: 355
        break;
    case 10:
        ret = 3150;            // old: 440
        break;
    case 11:
        ret = 3750;            // old: 560
        break;
    case 12:
        ret = 4400;            // old: 680
        break;
    case 13:
        ret = 5250;            // old: 850
        break;
    default:
        // old: 1100 + 300 * (lev - 14)
        // older: 1200 * (lev - 11) + ((lev - 11) * (lev - 11));// * (lev - 11))
        ret = 6200 + 1800 * (lev - 14);
        break;
    }

    return (ret) ;
}

int species_skills( int skill, int species )
{
    if (species == SP_UNKNOWN)
        species = you.species;

    // Up until getting their colour, all draconians advance the same. -- bwr
    if (you.max_xp_level < 7 && player_genus( GENPC_DRACONIAN ))
        species = SP_BASE_DRACONIAN;

    // Spellcasting is more expensive, invocations and evocations are cheaper
    if (skill == SK_SPELLCASTING)
        return (spec_skills[species - 1][skill] * 130) / 100;
    else if (skill == SK_INVOCATIONS || skill == SK_EVOCATIONS)
        return (spec_skills[species - 1][skill] * 75) / 100;
    else
        return (spec_skills[species - 1][skill]);
}                               // end species_skills()

// Function to convert from base draconian tmeplate to regular template
// upon draconian first reaching level 7.
void recalc_drac_skill_points( void )
{
    for (int i = 0; i < NUM_SKILLS; i++)
    {
        // convert ratio with round:
        you.skill_points[i] *= spec_skills[ you.species - 1 ][i];
        you.skill_points[i] += spec_skills[ SP_BASE_DRACONIAN - 1 ][i] / 2;
        you.skill_points[i] /= spec_skills[ SP_BASE_DRACONIAN - 1 ][i];
    }

    calc_total_skill_points();
}

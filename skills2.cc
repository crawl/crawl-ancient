/*
This file was extensively modified by Wladimir van der Laan.
*/
#include "config.h"

#ifdef DOS
#include <conio.h>
#endif

#include "externs.h"

#include <stdio.h>
#include <stdlib.h>

int species_skills(char skill, char species);
unsigned int skill_exp_needed(int lev); /* both in skills.cc */

//#define DEBUG

char *skills[50][5] = {
{"Fighting",       "Grunt", "Fighter", "Warrior", "Slayer"}, // fighter
{"Short Blades",   "Stabber","Cutter","Slicer","Knifefighter"}, // short blades
{"Long Swords",    "Slasher", "Cutter", "Slicer", "Eviscerator"}, // long swords
{"Great Swords",   "Slasher", "Cutter", "Slicer", "Eviscerator"}, // great swords
{"Axes",           "Chopper", "Cleaver", "Hacker", "Axe Maniac"}, // axes
{"Maces & Flails", "Basher", "Cruncher", "Smasher", "Crusher"}, //  maces & flails
{"Polearms",       "Chopper", "Cleaver", "Hacker", "Axe Maniac"}, // polearms
{"Staves",         "Basher", "Cruncher", "Stickfighter", "Pole Vaulter"}, // staves

// 8:
{"Slings",         "Slinger", "Bombardier", "Crazy Person", "Really Crazy Person"}, // slings - who would bother?
{"Bows",           "Hunter", "Archer", "Sharpshooter", "Merry Person"}, // bows - as in 'Robin Hood and his...'
{"Crossbows",      "Hunter", "Archer", "Sharpshooter", "Ballista"/* silly */}, // xbows
{"Darts",          "Thrower", "Hurler", "Hurler, First Class", "Darts Champion"}, // darts
// 12
{"Throwing",       "Thrower", "Chucker", "Chucker, First Class", "Catapult"}, // throwing

// 13
{"Armour",         "Grunt", "Heavy Grunt", "Tortoise", "Invulnerable"}, // armour
{"Dodging",        "Ducker", "Dodger", "Avoider", "Evader"}, // dodging
{"Stealth",        "Sneaker", "Thief", "Shadow", "Undetectable"}, // stealth
{"Stabbing",       "Backstabber", "Cutthroat", "Assassin", "Politician"}, // stabbing
// 17
{"Shields",        "Shield-Bearer", "Blocker", "Blocker, First Class", "Hoplite"}, // shields
{"Traps & Doors",          "Disarmer", "Trapper", "Trapper", "Trapper"}, // traps

{"Unarmed Combat",         "Brawler", "Boxer", "Martial Artist", "Martial Artist"},
{NULL},
{NULL},
{NULL},
{NULL},
{NULL},

{"Spellcasting",   "Magician", "Wizard", "Eclecticist", "Archmage"}, // spellcasting
{"Conjurations",   "Evoker", "Conjurer", "Destroyer", "Annihilator"}, // conjure
{"Enchantments",   "Charm-Maker", "Enchanter", "Infuser", "Spellbinder"}, // enchant
{"Summonings",     "Caller", "Invoker", "Summoner", "Demonologist"}, // summoning
// 29:
{"Necromancy",     "Grave Robber", "Necromancer", "Reanimator", "Death Mage"}, // death
{"Translocations", "Jumper", "Shifter", "Blinker", "Translocater"}, // translocations
{"Transmigration", "Changer", "Transformer", "Transmogrifier", "Transmuter"}, // transmigrations
{"Divinations",    "Seer", "Overseer", "Diviner", "Oracle"}, // divinations

// 33:

{"Fire Magic",     "Igniter", "Burner", "Pyromancer", "Infernalist"},
{"Ice Magic",      "Freezer", "Refrigerator", "Cryomancer", "Englaciator"},
{"Air Magic",      "Air Mage", "Cloud mage", "Sky Mage", "Storm Mage"},
{"Earth Magic",    "Digger", "Geomancer", "Petrifier", "Earth Mage"},

{"Poison Magic",   "Stinger", "Tainter", "Poisoner", "Venom Mage"},

/*NOTE: If more skills are added, must change ranges in level_change() in crawl99.cc */

/*{"",             "", "", "", ""},*/

{NULL},
{NULL},
{NULL},
{NULL},
{NULL},
{NULL},
{NULL},
{NULL},
{NULL},
/*{NULL},
{NULL},
{NULL},
{NULL}*/
};



// human
/* Note that this (humans have 100 for all skills) is assumed in the
level_change function in crawl99.cc, if CLASSES is def'd */


int spec_skills [31] [38] =
{

// 1 human
{
 100, // fighting
 100, // short blades
 100, // long blades
 100, // great swords
 100, // axes
 100, // maces and flails
 100, // polearms
 100, // staves
 100, // slings
 100, // bows
 100, // xbows
 100, // darts
 100, // throwing
 100, // armour
 100, // dodge
 100, // stealth
 100, // stab
 100, // shields
 100, // traps
 100, // unarmed combat
 100,
 100,
 100,
 100,
 100,
 100, // spellcasting
 100, // conj
 100, // ench
 100, // summ
 100, // necro
 100, // transloc
 100, // transmut
 100, // divin
 100, // fire
 100, // ice
 100, // Air
 100, // Earth
 100, // poison
},

// 2: elf
{
 120, // fighting
 80, // short blades
 80, // long blades
 110, // great swords
 120, // axes
 130, // maces and flails
 130, // polearms
 100, // staves
 120, // slings
 60, // bows
 100, // xbows
 90, // darts
 80, // throwing
 120, // armour
 80, // dodge
 80, // stealth
 100, // stab
 120, // shields
 100, // traps
 110, // unarmed combat
 100,
 100,
 100,
 100,
 100,
 80, // spellcasting
 105, // conj
 70, // ench
 100, // summ
 120, // necro
 100, // transloc
 100, // transmut
 100, // divin
 100, // Fire
 100, // Ice
 70, // Air
 130, // Earth
 110, // poison
},


// 3: high elf

{
 100, // fighting
 70, // short blades
 70, // long blades
 115, // great swords
 130, // axes
 150, // maces and flails
 150, // polearms
 100, // staves
 140, // slings
 60, // bows
 100, // xbows
 90, // darts
 80, // throwing
 110, // armour
 90, // dodge
 90, // stealth
 110, // stab
 110, // shields
 100, // traps
 130, // unarmed combat
 100,
 100,
 100,
 100,
 100,
 70, // spellcasting
 90, // conj
 70, // ench
 110, // summ
 130, // necro
 90, // transloc
 90, // transmut
 110, // divin
 100, // Fire
 100, // Ice
 70, // Air
 130, // Earth
 130, // poison
},


// 4: grey elf

{
 140, // fighting
 90, // short blades
 95, // long blades
 120, // great swords
 140, // axes
 160, // maces and flails
 160, // polearms
 100, // staves
 130, // slings
 70, // bows
 100, // xbows
 90, // darts
 80, // throwing
 140, // armour
 75, // dodge
 70, // stealth
 100, // stab
 140, // shields
 100, // traps
 130, // unarmed combat
 100,
 100,
 100,
 100,
 100,
 60, // spellcasting
 90, // conj
 50, // ench
 90, // summ
 130, // necro
 80, // transloc
 80, // transmut
 80, // divin
 90, // Fire
 90, // Ice
 60, // Air
 150, // Earth
 110, // poison
},


// 5: deep elf

{
 150, // fighting
 100, // short blades
 105, // long blades
 120, // great swords
 150, // axes
 165, // maces and flails
 165, // polearms
 100, // staves
 135, // slings
 74, // bows
 75, // xbows
 75, // darts
 80, // throwing
 140, // armour
 70, // dodge
 65, // stealth
 80, // stab
 140, // shields
 100, // traps
 130, // unarmed combat
 100,
 100,
 100,
 100,
 100,
 55, // spellcasting
 80, // conj
 50, // ench
 80, // summ
 70, // necro
 75, // transloc
 75, // transmut
 75, // divin
 90, // Fire
 90, // Ice
 80, // Air
 100, // Earth
 80, // poison
},


// 6: sludge elf

{
 100, // fighting
 80, // short blades
 85, // long blades
 110, // great swords
 130, // axes
 140, // maces and flails
 140, // polearms
 100, // staves
 100, // slings
 70, // bows
 100, // xbows
 80, // darts
 70, // throwing
 140, // armour
 70, // dodge
 75, // stealth
 100, // stab
 130, // shields
 100, // traps
 100, // unarmed combat
 100,
 100,
 100,
 100,
 100,
 70, // spellcasting
 90, // conj
 80, // ench
 90, // summ
 100, // necro
 100, // transloc
 100, // transmut
 100, // divin
 90, // Fire
 90, // Ice
 80, // Air
 100, // Earth
 100, // poison
},




// 7: hill dwarf

{
 70, // fighting
 80, // short blades
 80, // long blades
 90, // great swords
 60, // axes
 70, // maces and flails
 110, // polearms
 130, // staves
 130, // slings
 150, // bows
 120, // xbows
 120, // darts
 120, // throwing
 70, // armour
 120, // dodge
 150, // stealth
 140, // stab
 70, // shields
 100, // traps
 100, // unarmed combat
 100,
 100,
 100,
 100,
 100,
 160, // spellcasting
 120, // conj
 150, // ench
 150, // summ
 160, // necro
 150, // transloc
 120, // transmut
 130, // divin
 80, // Fire
 120, // Ice
 150, // Air
 70, // Earth
 130, // poison
},


// 8: mountain dwarf

{
 70, // fighting
 90, // short blades
 90, // long blades
 100, // great swords
 70, // axes
 70, // maces and flails
 110, // polearms
 120, // staves
 125, // slings
 140, // bows
 100, // xbows
 120, // darts
 115, // throwing
 60, // armour
 110, // dodge
 140, // stealth
 130, // stab
 70, // shields
 100, // traps
 100, // unarmed combat
 100,
 100,
 100,
 100,
 100,
 140, // spellcasting
 115, // conj
 135, // ench
 150, // summ
 160, // necro
 150, // transloc
 120, // transmut
 130, // divin
 70, // Fire
 130, // Ice
 150, // Air
 70, // Earth
 130, // poison
},


// 9: halfling

{
 120, // fighting
 60, // short blades
 100, // long blades
 130, // great swords
 120, // axes
 150, // maces and flails
 160, // polearms
 130, // staves
 50, // slings
 70, // bows
 90, // xbows
 50, // darts
 60, // throwing
 150, // armour
 70, // dodge
 60, // stealth
 70, // stab
 130, // shields
 100, // traps
 140, // unarmed combat
 100,
 100,
 100,
 100,
 100,
 130, // spellcasting
 130, // conj
 100, // ench
 120, // summ
 150, // necro
 100, // transloc
 150, // transmut
 140, // divin
 100, // Fire
 100, // Ice
 90, // Air
 100, // Earth
 120, // poison
},



// 10: orc

{
 70, // fighting
 100, // short blades
 80, // long blades
 70, // great swords
 70, // axes
 80, // maces and flails
 80, // polearms
 110, // staves
 130, // slings
 120, // bows
 120, // xbows
 130, // darts
 130, // throwing
 90, // armour
 140, // dodge
 150, // stealth
 100, // stab
 80, // shields
 100, // traps
 90, // unarmed combat
 100,
 100,
 100,
 100,
 100,
 150, // spellcasting
 100, // conj
 120, // ench
 120, // summ
 100, // necro
 150, // transloc
 160, // transmut
 160, // divin
 100, // Fire
 100, // Ice
 150, // Air
 100, // Earth
 110, // poison
},



// 11: kobold

{
 80, // fighting
 60, // short blades
 100, // long blades
 120, // great swords
 110, // axes
 140, // maces and flails
 150, // polearms
 110, // staves
 70, // slings
 80, // bows
 90, // xbows
 50, // darts
 60, // throwing
 140, // armour
 70, // dodge
 60, // stealth
 70, // stab
 130, // shields
 100, // traps
 100, // unarmed combat
 100,
 100,
 100,
 100,
 100,
 110, // spellcasting
 110, // conj
 110, // ench
 105, // summ
 105, // necro
 100, // transloc
 110, // transmut
 130, // divin
 100, // Fire
 100, // Ice
 100, // Air
 100, // Earth
 100, // poison
},


// 12: mummy

{
 100, // fighting
 140, // short blades
 140, // long blades
 140, // great swords
 140, // axes
 140, // maces and flails
 140, // polearms
 140, // staves
 140, // slings
 140, // bows
 140, // xbows
 140, // darts
 140, // throwing
 140, // armour
 140, // dodge
 140, // stealth
 140, // stab
 140, // shields
 140, // traps
 140, // unarmed combat
 140,
 140,
 140,
 140,
 140,
 100, // spellcasting
 140, // conj
 140, // ench
 140, // summ
 100, // necro
 140, // transloc
 140, // transmut
 140, // divin
 140, // fire
 140, // ice
 140, // Air
 140, // Earth
 140, // poison
},


// 13: naga

{
 110, // fighting
 100, // short blades
 100, // long blades
 100, // great swords
 100, // axes
 100, // maces and flails
 100, // polearms
 120, // staves
 120, // slings
 120, // bows
 120, // xbows
 120, // darts
 120, // throwing
 150, // armour
 150, // dodge
 40, // stealth
 100, // stab
 140, // shields
 100, // traps
 100, // unarmed combat
 100,
 100,
 100,
 100,
 100,
 100, // spellcasting
 100, // conj
 100, // ench
 100, // summ
 100, // necro
 100, // transloc
 100, // transmut
 100, // divin
 100, // Fire
 100, // Ice
 100, // Air
 100, // Earth
 60, // poison
},


// 14: Gnome

{
 100, // fighting
 75, // short blades
 100, // long blades
 130, // great swords
 100, // axes
 130, // maces and flails
 140, // polearms
 130, // staves
 80, // slings
 100, // bows
 90, // xbows
 60, // darts
 100, // throwing
 150, // armour
 70, // dodge
 70, // stealth
 80, // stab
 120, // shields
 100, // traps
 110, // unarmed combat
 100,
 100,
 100,
 100,
 100,
 120, // spellcasting
 100, // conj
 100, // ench
 110, // summ
 130, // necro
 130, // transloc
 120, // transmut
 120, // divin
 100, // Fire
 100, // Ice
 170, // Air
 60, // Earth
 130, // poison
},


// 15: ogre

{
 100, // fighting
 140, // short blades
 120, // long blades
 110, // great swords
 100, // axes
 100, // maces and flails
 110, // polearms
 120, // staves
 150, // slings
 150, // bows
 180, // xbows
 150, // darts
 100, // throwing
 140, // armour
 150, // dodge
 200, // stealth
 150, // stab
 110, // shields
 200, // traps
 130, // unarmed combat
 100,
 100,
 100,
 100,
 100,
 220, // spellcasting
 180, // conj
 220, // ench
 200, // summ
 150, // necro
 200, // transloc
 200, // transmut
 200, // divin
 150, // Fire
 150, // Ice
 200, // Air
 120, // Earth
 150, // poison
},


// 16: troll

{
 150, // fighting
 150, // short blades
 150, // long blades
 150, // great swords
 150, // axes
 150, // maces and flails
 150, // polearms
 150, // staves
 150, // slings
 150, // bows
 180, // xbows
 150, // darts
 150, // throwing
 150, // armour
 130, // dodge
 250, // stealth
 130, // stab
 140, // shields
 200, // traps
 120, // unarmed combat
 100,
 100,
 100,
 100,
 100,
 200, // spellcasting
 150, // conj
 200, // ench
 150, // summ
 140, // necro
 150, // transloc
 150, // transmut
 180, // divin
 150, // Fire
 150, // Ice
 200, // Air
 110, // Earth
 130, // poison
},



// 17: ogre mage

{
 100, // fighting
 110, // short blades
 100, // long blades
 100, // great swords
 100, // axes
 100, // maces and flails
 100, // polearms
 100, // staves
 150, // slings
 150, // bows
 150, // xbows
 150, // darts
 150, // throwing
 170, // armour
 130, // dodge
 100, // stealth
 130, // stab
 150, // shields
 150, // traps
 100, // unarmed combat
 100,
 100,
 100,
 100,
 100,
 80, // spellcasting
 100, // conj
 80, // ench
 100, // summ
 100, // necro
 100, // transloc
 100, // transmut
 100, // divin
 100, // Fire
 100, // Ice
 100, // Air
 100, // Earth
 100, // poison
},


// 18: Red Draconian

{
 90, // fighting
 100, // short blades
 100, // long blades
 100, // great swords
 100, // axes
 100, // maces and flails
 100, // polearms
 100, // staves
 120, // slings
 120, // bows
 120, // xbows
 120, // darts
 120, // throwing
 200, // armour
 120, // dodge
 120, // stealth
 100, // stab
 100, // shields
 100, // traps
 100,
 100,
 100,
 100,
 100,
 100,
 100, // spellcasting
 100, // conj
 120, // ench
 100, // summ
 100, // necro
 100, // transloc
 100, // transmut
 100, // divin
 70, // fire
 150, // Ice
 100, // Air
 100, // Earth
 100, // poison
},

// 19: White drac
{
 90, // fighting
 100, // short blades
 100, // long blades
 100, // great swords
 100, // axes
 100, // maces and flails
 100, // polearms
 100, // staves
 120, // slings
 120, // bows
 120, // xbows
 120, // darts
 120, // throwing
 200, // armour
 120, // dodge
 120, // stealth
 100, // stab
 100, // shields
 100, // traps
 100,
 100,
 100,
 100,
 100,
 100,
 100, // spellcasting
 100, // conj
 120, // ench
 100, // summ
 100, // necro
 100, // transloc
 100, // transmut
 100, // divin
 150, // fire
 70, // Ice
 100, // Air
 100, // Earth
 100, // poison
},


// Green drac
{
 90, // fighting
 100, // short blades
 100, // long blades
 100, // great swords
 100, // axes
 100, // maces and flails
 100, // polearms
 100, // staves
 120, // slings
 120, // bows
 120, // xbows
 120, // darts
 120, // throwing
 200, // armour
 120, // dodge
 120, // stealth
 100, // stab
 100, // shields
 100, // traps
 100,
 100,
 100,
 100,
 100,
 100,
 100, // spellcasting
 100, // conj
 120, // ench
 100, // summ
 100, // necro
 100, // transloc
 100, // transmut
 100, // divin
 100, // fire
 100, // Ice
 100, // Air
 100, // Earth
 70, // poison
},

// 21: yellow
{
 90, // fighting
 100, // short blades
 100, // long blades
 100, // great swords
 100, // axes
 100, // maces and flails
 100, // polearms
 100, // staves
 120, // slings
 120, // bows
 120, // xbows
 120, // darts
 120, // throwing
 200, // armour
 120, // dodge
 120, // stealth
 100, // stab
 100, // shields
 100, // traps
 100,
 100,
 100,
 100,
 100,
 100,
 100, // spellcasting
 100, // conj
 120, // ench
 100, // summ
 100, // necro
 100, // transloc
 100, // transmut
 100, // divin
 100, // fire
 100, // Ice
 100, // Air
 100, // Earth
 100, // poison
},

// 22: grey
{
 90, // fighting
 100, // short blades
 100, // long blades
 100, // great swords
 100, // axes
 100, // maces and flails
 100, // polearms
 100, // staves
 120, // slings
 120, // bows
 120, // xbows
 120, // darts
 120, // throwing
 200, // armour
 120, // dodge
 120, // stealth
 100, // stab
 100, // shields
 100, // traps
 100,
 100,
 100,
 100,
 100,
 100,
 100, // spellcasting
 100, // conj
 120, // ench
 100, // summ
 100, // necro
 100, // transloc
 100, // transmut
 100, // divin
 100, // fire
 100, // Ice
 100, // Air
 100, // Earth
 100, // poison
},

// 23 black drac
{
 90, // fighting
 100, // short blades
 100, // long blades
 100, // great swords
 100, // axes
 100, // maces and flails
 100, // polearms
 100, // staves
 120, // slings
 120, // bows
 120, // xbows
 120, // darts
 120, // throwing
 200, // armour
 120, // dodge
 120, // stealth
 100, // stab
 100, // shields
 100, // traps
 100,
 100,
 100,
 100,
 100,
 100,
 100, // spellcasting
 100, // conj
 120, // ench
 100, // summ
 100, // necro
 100, // transloc
 100, // transmut
 100, // divin
 100, // fire
 100, // Ice
 70, // Air
 150, // Earth
 100, // poison
},


// 24 purple drac
{
 90, // fighting
 100, // short blades
 100, // long blades
 100, // great swords
 100, // axes
 100, // maces and flails
 100, // polearms
 100, // staves
 120, // slings
 120, // bows
 120, // xbows
 120, // darts
 120, // throwing
 200, // armour
 120, // dodge
 120, // stealth
 100, // stab
 100, // shields
 100, // traps
 100,
 100,
 100,
 100,
 100,
 100,
 70, // spellcasting
 100, // conj
 120, // ench
 100, // summ
 100, // necro
 100, // transloc
 100, // transmut
 100, // divin
 100, // fire
 100, // Ice
 100, // Air
 100, // Earth
 100, // poison
},

// 25 mottled drac
{
 90, // fighting
 100, // short blades
 100, // long blades
 100, // great swords
 100, // axes
 100, // maces and flails
 100, // polearms
 100, // staves
 120, // slings
 120, // bows
 120, // xbows
 120, // darts
 120, // throwing
 200, // armour
 120, // dodge
 120, // stealth
 100, // stab
 100, // shields
 100, // traps
 100,
 100,
 100,
 100,
 100,
 100,
 100, // spellcasting
 100, // conj
 120, // ench
 100, // summ
 100, // necro
 100, // transloc
 100, // transmut
 100, // divin
 80, // fire
 100, // Ice
 100, // Air
 100, // Earth
 100, // poison
},

// 26 pale drac
{
 90, // fighting
 100, // short blades
 100, // long blades
 100, // great swords
 100, // axes
 100, // maces and flails
 100, // polearms
 100, // staves
 120, // slings
 120, // bows
 120, // xbows
 120, // darts
 120, // throwing
 200, // armour
 120, // dodge
 120, // stealth
 100, // stab
 100, // shields
 100, // traps
 100,
 100,
 100,
 100,
 100,
 100,
 100, // spellcasting
 100, // conj
 120, // ench
 100, // summ
 100, // necro
 100, // transloc
 100, // transmut
 100, // divin
 90, // fire
 100, // Ice
 90, // Air
 100, // Earth
 100, // poison
},

// 27 drac
{
 90, // fighting
 100, // short blades
 100, // long blades
 100, // great swords
 100, // axes
 100, // maces and flails
 100, // polearms
 100, // staves
 120, // slings
 120, // bows
 120, // xbows
 120, // darts
 120, // throwing
 200, // armour
 120, // dodge
 120, // stealth
 100, // stab
 100, // shields
 100, // traps
 100,
 100,
 100,
 100,
 100,
 100,
 100, // spellcasting
 100, // conj
 120, // ench
 100, // summ
 100, // necro
 100, // transloc
 100, // transmut
 100, // divin
 100, // fire
 100, // Ice
 100, // Air
 100, // Earth
 100, // poison
},

// 27 drac
{
 90, // fighting
 100, // short blades
 100, // long blades
 100, // great swords
 100, // axes
 100, // maces and flails
 100, // polearms
 100, // staves
 120, // slings
 120, // bows
 120, // xbows
 120, // darts
 120, // throwing
 200, // armour
 120, // dodge
 120, // stealth
 100, // stab
 100, // shields
 100, // traps
 100,
 100,
 100,
 100,
 100,
 100,
 100, // spellcasting
 100, // conj
 120, // ench
 100, // summ
 100, // necro
 100, // transloc
 100, // transmut
 100, // divin
 100, // fire
 100, // Ice
 100, // Air
 100, // Earth
 100, // poison
},

// 28 drac
{
 90, // fighting
 100, // short blades
 100, // long blades
 100, // great swords
 100, // axes
 100, // maces and flails
 100, // polearms
 100, // staves
 120, // slings
 120, // bows
 120, // xbows
 120, // darts
 120, // throwing
 200, // armour
 120, // dodge
 120, // stealth
 100, // stab
 100, // shields
 100, // traps
 100,
 100,
 100,
 100,
 100,
 100,
 100, // spellcasting
 100, // conj
 120, // ench
 100, // summ
 100, // necro
 100, // transloc
 100, // transmut
 100, // divin
 100, // fire
 100, // Ice
 100, // Air
 100, // Earth
 100, // poison
},

// 30 - Centaur
{
 85, // fighting
 110, // short blades
 100, // long blades
 100, // great swords
 100, // axes
 100, // maces and flails
 100, // polearms
 100, // staves
 75, // slings
 60, // bows
 85, // xbows
 80, // darts
 60, // throwing
 180, // armour
 170, // dodge
 200, // stealth
 170, // stab
 100, // shields
 150, // traps
 100, // unarmed combat
 100,
 100,
 100,
 100,
 100,
 110, // spellcasting
 110, // conj
 100, // ench
 110, // summ
 110, // necro
 110, // transloc
 100, // transmut
 100, // divin
 100, // fire
 100, // ice
 100, // Air
 100, // Earth
 110, // poison
},

// 31 - demigod
{
 110, // fighting
 110, // short blades
 110, // long blades
 110, // great swords
 110, // axes
 110, // maces and flails
 110, // polearms
 110, // staves
 110, // slings
 110, // bows
 110, // xbows
 110, // darts
 110, // throwing
 110, // armour
 110, // dodge
 110, // stealth
 110, // stab
 110, // shields
 110, // traps
 110, // unarmed combat
 110,
 110,
 110,
 110,
 110,
 110, // spellcasting
 110, // conj
 110, // ench
 110, // summ
 110, // necro
 110, // transloc
 110, // transmut
 110, // divin
 110, // fire
 110, // ice
 110, // Air
 110, // Earth
 110, // poison
},



/*
base drac
{
 90, // fighting
 100, // short blades
 100, // long blades
 100, // great swords
 100, // axes
 100, // maces and flails
 100, // polearms
 100, // staves
 120, // slings
 120, // bows
 120, // xbows
 120, // darts
 120, // throwing
 200, // armour
 120, // dodge
 120, // stealth
 100, // stab
 100, // shields
 100, // traps
 100,
 100,
 100,
 100,
 100,
 100,
 100, // spellcasting
 100, // conj
 120, // ench
 100, // summ
 100, // necro
 100, // transloc
 100, // transmut
 100, // divin
 100, // fire
 100, // Ice
 100, // Air
 100, // Earth
 100, // poison
},
*/
/*
 case 1: return "Human";
 case 2: return "Elf";
 case 3: return "High Elf";
 case 4: return "Grey Elf";
 case 5: return "Deep Elf";
 case 6: return "Sludge Elf";
 case 7: return "Hill Dwarf";
 case 8: return "Mountain Dwarf";
 case 9: return "Halfling";
 case 10: return "Hill Orc";
 case 11: return "Kobold";
 case 12: return "Mummy";
 case 13: return "Naga";
 case 14: return "Gnome";
 case 15: return "Ogre";
 case 16: return "Troll";
 case 17: return "Ogre-Mage";
 case 18: return "Red Draconian";
 case 19: return "White Draconian";
 case 20: return "Green Draconian";
 case 21: return "Yellow Draconian";
 case 22: return "Grey Draconian";
 case 23: return "Black Draconian";
 case 24: return "Purple Draconian";
*/

/*
case 2: // elf

{
 100, // fighting
 100, // short blades
 100, // long blades
 100, // great swords
 100, // axes
 100, // maces and flails
 100, // polearms
 100, // staves
 100, // slings
 100, // bows
 100, // xbows
 100, // darts
 100, // throwing
 100, // armour
 100, // dodge
 100, // stealth
 100, // stab
 100, // shields
 100, // traps
 100,
 100,
 100,
 100,
 100,
 100,
 100, // spellcasting
 100, // conj
 100, // ench
 100, // summ
 100, // necro
 100, // transloc
 100, // transmut
 100, // divin
 100,
 100,
},

*/

};



char *skill_name(char which_skill);

void show_skills(void)
{
char st_pass [60];
int i;
int x;
char strng [5] = "";
char lcount = 0;
        _setcursortype(_NOCURSOR);

#ifdef DOS_TERM
        window(1, 1, 80, 25);
        char buffer[4600];
        gettext(1, 1, 80, 25, buffer);
#endif
        strcpy(st_pass, "");
        clrscr();

reprint_stuff:
        gotoxy(1, 1);
        lcount = 0;
        textcolor(LIGHTGREY);
        cprintf(" You have ");
        itoa(you[0].exp_available, strng, 10);
        cprintf(strng);
        cprintf(" points of unallocated experience."EOL EOL);
        char scrln=3,scrcol=1;
        for(x=0;x<50;x++) {
                /* spells in second column */
                if(x==25) { scrln=3; scrcol=40; }
                gotoxy(scrcol,scrln);
                if(you[0].skills [x] > 0)
                {
                        if (you[0].practise_skill[x] == 0) textcolor(DARKGREY); else textcolor(LIGHTGREY);
                        if (you[0].skills [x] == 27) textcolor(YELLOW);
                        putch(lcount + 97); lcount++;
            cprintf(" - ");
            char bufff[15];
            sprintf(bufff, "%-14s", skills[x][0]);
            cprintf(bufff);
                        cprintf(" Skill ");
                        itoa(you[0].skills[x], strng, 10);
                        cprintf(strng);
                        textcolor(BLUE);
                        cprintf(" (");
            itoa(((((skill_exp_needed(you[0].skills [x] + 2) * species_skills(x, you[0].species)) / 100) - you[0].skill_points [x]) * 10) / (((skill_exp_needed(you[0].skills [x] + 2) * species_skills(x, you[0].species)) / 100) - ((skill_exp_needed(you[0].skills [x] + 1) * species_skills(x, you[0].species)) / 100)), strng, 10);
                        cprintf(strng);
                        cprintf(")");
                        textcolor(LIGHTGREY);
#ifdef DEBUG
                        cprintf(" / ");
            itoa(you[0].skill_points[x], strng, 10);
                        cprintf(strng);
#endif
                        scrln++;
                }
                /* Extra CR between classes of weapons */
                if(x==7 || x==12 || x==18) scrln++;
        }
// if any more skills added, must adapt letters to go into caps
        gotoxy(1, 24);
        textcolor(LIGHTGREY);
        cprintf("Press the letter of a skill to choose whether you want to practise it.");
        char get_thing;

        get_thing = getch();

        if (get_thing == 0) getch();
        else {
                if (get_thing < 97 || get_thing > 122) goto putty;
                lcount = 0; // toggle skill practise
                for (i = 0; i < 50; i ++) {
                        if (you[0].skills [i] == 0) continue;
                        if (get_thing - 97 == lcount ++)
                                if (you[0].practise_skill [i] == 0) you[0].practise_skill [i] = 1; else you[0].practise_skill [i] = 0;
                }
                goto reprint_stuff;
        }
        putty:
#ifdef DOS_TERM
        puttext(1, 1, 80, 25, buffer);
#endif
        return;
}

char *skill_name(char which_skill)
{
 return skills [which_skill] [0];
}


char *skill_title(char best_skill, char skill_lev, char char_class, char char_lev)
{

//char title [40];

//strcpy(title, "undefined");

char skill_lev2 = char_class;
skill_lev2 = 0;
char char_lev2 = 0;
char *tempstr=NULL;

//if (skill_lev <= 21)
skill_lev2 = 3;
if (skill_lev <= 20) skill_lev2 = 2;
if (skill_lev <= 12) skill_lev2 = 1; // 12
if (skill_lev <= 7) skill_lev2 = 0; // 6 or 7

char_lev2 = 3;
//if (char_lev <= 21) char_lev2 = 3;
if (char_lev <= 20) char_lev2 = 2;
if (char_lev <= 12) char_lev2 = 1;
if (char_lev <= 7) char_lev2 = 0;

/*if (char_class == 2) // Priest
{
switch(char_lev2)
{
 case 0: return "Preacher";
 case 1: return "Priest";
 case 2: return "Evangelist";
 case 3: return "Pontifex";
}
} else
if (char_class == 6) // Paladin
{
switch(char_lev2)
{
 case 0: return "Holy Warrior";
 case 1: return "Holy Crusader";
 case 2: return "Paladin";
 case 3: return "Scourge of Evil";
}
} else {*/
        if(best_skill<50) tempstr=skills[best_skill][skill_lev2+1];
//      if(tempstr!=NULL) strcpy(title,tempstr);
//       }

//return title;
if (tempstr == NULL) return "Invalid Title";
return tempstr;

}


char best_skill(char min_skill, char max_skill, char excl_skill)
{

char skillb = 0;
char skmk = 0;

int i = 0;

for (i = max_skill; i >= min_skill; i --)
{
 if (you[0].skills [i] >= skmk && i != excl_skill)
 {
  skillb = i;
  skmk = you[0].skills [i];
 }
}

if (you[0].skills [0] >= skmk && min_skill <= 0)
{
 skillb = 0;
 skmk = you[0].skills [0];
}

if (you[0].skills [25] >= skmk && min_skill <= 25 && max_skill >= 25)
{
 skillb = 25;
 skmk = you[0].skills [25];
}

return skillb;

}


int calc_hp(void)
{

int hitp = you[0].hp_max;

hitp = you[0].base_hp - 5000 + you[0].base_hp2 - 5000;
hitp += you[0].xl * you[0].skills [0] / 5;

if (you[0].berserker != 0)
{
 hitp *= 15;
 hitp /= 10;
}

if (you[0].duration [18] != 0)
{
 switch(you[0].attribute [5])
 {
  case 3: hitp *= 15; hitp /= 10; break;
  case 4: hitp *= 12; hitp /= 10; break;
 }
}

you[0].hp_max = hitp;
if (you[0].hp > you[0].hp_max) you[0].hp = you[0].hp_max;

return hitp;

}

int calc_ep(void)
{

int enp = you[0].ep_max;

enp = you[0].base_ep - 5000 + you[0].base_ep2 - 5000;
enp += you[0].xl * you[0].skills [25] / 6;

/*if (enp > 21) enp = ((enp - 27) / 2) + 27;
if (enp > 36) enp = ((enp - 36) / 2) + 36;
if (enp > 49) enp = ((enp - 49) / 2) + 49;*/

if (enp > 18) enp = ((enp - 18) / 2) + 18;
if (enp > 27) enp = ((enp - 27) / 2) + 27;
if (enp > 36) enp = ((enp - 36) / 2) + 36;

if (enp > 49) enp = 49;

you[0].ep_max = enp;
if (you[0].ep > you[0].ep_max) you[0].ep = you[0].ep_max;

return enp;

}

#ifdef CLASSES
unsigned int skill_exp_needed(int lev)
{
lev --;
switch(lev)
{
        case 0: return 0;
        case 1: return 10;
        case 2: return 60;
        case 3: return 130;
        case 4: return 200;
        case 5: return 400;
        case 6: return 500;
        case 7: return 610;
        case 8: return 720;
        case 9: return 840;
        case 10: return 960;
        case 11: return 1100;
        case 12: return 1250;
        case 13: return 1500;
        default: return 1900 + 400 * (lev - 14); //return 1200 * (lev - 11) + ((lev - 11) * (lev - 11));// * (lev - 11));
}

return 0;

}
#else
unsigned int skill_exp_needed(int lev)
{
lev --;
switch(lev)
{
/*      case 0: return 0;
        case 1: return 20;
        case 2: return 30;
        case 3: return 50;
        case 4: return 75;
        case 5: return 105;
        case 6: return 145;
        case 7: return 200;
        case 8: return 275;
        case 9: return 355;
        case 10: return 440;
        case 11: return 560;
        case 12: return 680;
        case 13: return 850;
        default: return 1100 + 300 * (lev - 14); //return 1200 * (lev - 11) + ((lev - 11) * (lev - 11));// * (lev - 11));
*/
        case 0: return 0;
        case 1: return 20;
        case 2: return 30;
        case 3: return 50;
        case 4: return 75;
        case 5: return 105;
        case 6: return 135;
        case 7: return 170;
        case 8: return 210;
        case 9: return 255;
        case 10: return 315;
        case 11: return 375;
        case 12: return 440;
        case 13: return 525;
        default: return 620 + 180 * (lev - 14);

}

return 0;

}
#endif


int species_skills(char skill, char species)
{

return spec_skills [species - 1] [skill];

}

#ifdef CLASSES
/*
This function divides the available skill points equally among eligible skills,
and returns any which are left over (eg if all available skills are at lvl 27).
Note: I'm not applying practise_skill to all skills, because that would give
 players too much control over the development of their characters without
 having to work for it.
*/
int add_skill(int min_skill, int max_skill, int sk_tot)
{

if (max_skill >= 50) max_skill = 49;

char numb_sk = 0;
int skc = 0;

for (skc = min_skill; skc <= max_skill; skc ++)
{
 if (you[0].practise_skill [skc] == 0)
 {
  if ((skc >= 1 && skc <= 11) || (skc >= 13 && skc <= 15) || (skc >= 26 && skc <= 36))
    continue;
 }
 if (you[0].skills [skc] > 0 && you[0].skills [skc] < 27) numb_sk ++;
}

if (numb_sk <= 0) return sk_tot;

for (skc = min_skill; skc <= max_skill; skc ++)
{
 if (you[0].practise_skill [skc] == 0)
 {
  if ((skc >= 1 && skc <= 11) || (skc >= 13 && skc <= 15) || (skc >= 26 && skc <= 36))
    continue;
 }
 if (you[0].skills [skc] > 0 && you[0].skills [skc] < 27)
 {
  you[0].skill_points [skc] += sk_tot / numb_sk;
  sk_tot -= sk_tot / numb_sk;
 }
}

return sk_tot;

}

/*void class_sk(int clsk [8] [3], int chcls)
{
int ci = 0;
int cy = 0;

for (ci = 0; ci < 8; ci ++)
{
 for (cy = 0; cy < 3; cy ++)
 {
   clsk = class_skill [chcls] [ci] [cy];
 }
}

}*/

#endif

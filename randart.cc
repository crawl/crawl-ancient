#include "config.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "externs.h"
#include "enum.h"

#include "randart.h"
#include "itemname.h"
#include "stuff.h"

#define RA_PROPERTIES 30

/*
The initial generation of a randart is very simple - it occurs
in dungeon.cc and consists of giving it a few random things - plus & plus2
mainly.
*/

char *rand_wpn_names[] = {
" of Blood",
" of Death",
" of Bloody Death",
" of Pain",
" of Painful Death",
" of Pain & Death",
" of Infinite Pain",
" of Eternal Torment",
" of Power",
" of Wrath",
/* 10: */
" of Doom",
" of Tender Mercy",
" of the Apocalypse",
" of the Jester",
" of the Ring",
" of the Fool",
" of the Gods",
" of the Imperium",
" of Destruction",
" of Armageddon",
/* 20: */
" of Cruel Justice",
" of Righteous Anger",
" of Might",
" of the Orb",
" of Makhleb",
" of Trog",
" of Xom",
" of the Ancients",
" of Mana",
" of Nemelex Xobeh",
/* 30: */
" of the Magi",
" of the Archmagi",
" of the King",
" of the Queen",
" of the Spheres",
" of Circularity",
" of Linearity",
" of Conflict",
" of Battle",
" of Honour",
/* 40: */
" of the Butterfly",
" of the Wasp",
" of the Frog",
" of the Weasel",
" of the Troglodytes",
" of the Pill-Bug",
" of Sin",
" of Vengeance",
" of Execution",
" of Arbitration",
/* 50: */
" of the Seeker",
" of Truth",
" of Lies",
" of the Eggplant",
" of the Turnip",
" of Chance",
" of Curses",
" of Hell's Wrath",
" of the Undead",
" of Chaos",
/* 60: */
" of Law",
" of Life",
" of the Old World",
" of the New World",
" of the Middle World",
" of Crawl",
" of Unpleasantness",
" of Discomfort",
" of Brutal Revenge",
" of Triumph",
/* 70: */
" of Evisceration",
" of Dismemberment",
" of Terror",
" of Fear",
" of Pride",
" of the Volcano",
" of Blood-Lust",
" of Division",
" of Eternal Harmony",
" of Peace",
/* 80: */
" of Quick Death",
" of Instant Death",
" of Misery",
" of the Whale",
" of the Lobster",
" of the Whelk",
" of the Penguin",
" of the Puffin",
" of the Mushroom",
" of the Toadstool",
/* 90: */
" of the Little People",
" of the Puffball",
" of Spores",
" of Optimality",
" of Pareto-Optimality",
" of Greatest Utility",
" of Anarcho-Capitalism",
" of Ancient Evil",
" of the Revolution",
" of the People",
/* 100: */
" of the Elves",
" of the Dwarves",
" of the Orcs",
" of the Humans",
" of Sludge",
" of the Naga",
" of the Trolls",
" of the Ogres",
" of Equitable Redistribution",
" of Wealth",
/* 110: */
" of Poverty",
" of Reapportionment",
" of Fragile Peace",
" of Reinforcement",
" of Beauty",
" of the Slug",
" of the Snail",
" of the Gastropod",
" of Corporal Punishment",
" of Capital Punishment",
/* 120: */
" of the Beast",
" of Light",
" of Darkness",
" of Day",
" of the Day",
" of Night",
" of the Night",
" of Twilight",
" of the Twilight",
" of Dawn",
/* 130: */
" of the Dawn",
" of the Sun",
" of the Moon",
" of Distant Worlds",
" of the Unseen Realm",
" of Pandemonium",
" of the Abyss",
" of the Nexus",
" of the Gulag",
" of the Crusades",
/* 140: */
" of Proximity",
" of Wounding",
" of Peril",
" of the Eternal Warrior",
" of the Eternal War",
" of Evil",
" of Pounding",
" of Oozing Pus",
" of Pestilence",
" of Plague",
/* 150: */
" of Negation",
" of the Saviour",
" of Infection",
" of Defence",
" of Protection",
" of Defence by Offence",
" of Expedience",
" of Reason",
" of Unreason",
" of the Heart",
/* 160: */
" of Offence",
" of the Leaf",
" of Leaves",
" of Winter",
" of Summer",
" of Autumn",
" of Spring",
" of Midsummer",
" of Midwinter",
" of Eternal Night",
/* 170: */
" of Shrieking Terror",
" of the Lurker",
" of the Crawling Thing",
" of the Thing",
"\"Thing\"",
" of the Sea",
" of the Forest",
" of the Trees",
" of Earth",
" of the World",
/* 180: */
" of Bread",
" of Yeast",
" of the Amoeba",
" of Deformation",
" of Guilt",
" of Innocence",
" of Ascent",
" of Descent",
" of Music",
" of Brilliance",
/* 190: */
" of Disgust",
" of Feasting",
" of Sunlight",
" of Starshine",
" of the Stars",
" of Dust",
" of the Clouds",
" of the Sky",
" of Ash",
" of Slime",
/* 200: */
" of Clarity",
" of Eternal Vigilance",
" of Purpose",
" of the Moth",
" of the Goat",
" of Fortitude",
" of Equivalence",
" of Balance",
" of Unbalance",
" of Harmony",
/* 210: */
" of Disharmony",
" of the Inferno",
" of the Omega Point",
" of Inflation",
" of Deflation",
" of Supply",
" of Demand",
" of Gross Domestic Product",
" of Unjust Enrichment",
" of Detinue",
/* 220: */
" of Conversion",
" of Anton Piller",
" of Mandamus",
" of Frustration",
" of Breach",
" of Fundamental Breach",
" of Termination",
" of Extermination",
" of Satisfaction",
" of Res Nullius",
/* 230: */
" of Fee Simple",
" of Terra Nullius",
" of Context",
" of Prescription",
" of Freehold",
" of Tortfeasance",
" of Omission",
" of Negligence",
" of Pains",
" of Attainder",
/* 240: */
" of Action",
" of Inaction",
" of Truncation",
" of Defenestration",
" of Desertification",
" of the Wilderness",
" of Psychosis",
" of Neurosis",
" of Fixation",
" of the Open Hand",
/* 250: */
" of the Tooth",
" of Honesty",
" of Dishonesty",
" of Divine Compulsion",
" of the Invisible Hand",
" of Freedom",
" of Liberty",
" of Servitude",
" of Domination",
" of Tension",
/* 260: */
" of Monotheism",
" of Atheism",
" of Agnosticism",
" of Existentialism",
" of the Good",
" of Relativism",
" of Absolutism",
" of Absolution",
" of Abstinence",
" of Abomination",
/* 270: */
" of Mutilation",
" of Stasis",
" of Wonder",
" of Dullness",
" of Dim Light",
" of the Shining Light",
" of Immorality",
" of Amorality",
" of Precise Incision",
" of Orthodoxy",
/* 280: */
" of Faith",
" of Untruth",
" of the Augerer",
" of the Water Diviner",
" of the Soothsayer",
" of Punishment",
" of Amelioration",
" of Sulphur",
" of the Egg",
" of the Globe",
/* 290: */
" of the Candle",
" of the Candelabrum",
" of the Vampires",
" of the Orcs",
" of the Halflings",
" of World's End",
" of Blue Skies",
" of Red Skies",
" of Orange Skies",
" of Purple Skies",
/* 300: */
" of Articulation",
" of the Mind",
" of the Spider",
" of the Lamprey",
" of the Beginning",
" of the End",
" of Severance",
" of Sequestration",
" of Mourning",
" of Death's Door",
/* 310: */
" of the Key",
" of Earthquakes",
" of Failure",
" of Success",
" of Intimidation",
" of the Mosquito",
" of the Gnat",
" of the Blowfly",
" of the Turtle",
" of the Tortoise",
/* 320: */
" of the Pit",
" of the Grave",
" of Submission",
" of Dominance",
" of the Messenger",
" of Crystal",
" of Gravity",
" of Levity",
" of the Slorg",
" of Surprise",
/* 330: */
" of the Maze",
" of the Labyrinth",
" of Divine Intervention",
" of Rotation",
" of the Spinneret",
" of the Scorpion",
" of Demonkind",
" of the Genius",
" of Bloodstone",
" of Grontol",
/* 340: */
" \"Grim Tooth\"",
" \"Widowmaker\"",
" \"Widowermaker\"",
" \"Lifebane\"",
" \"Conservator\"",
" \"Banisher\"",
" \"Tormentor\"",
" \"Secret Weapon\"",
" \"String\"",
" \"Stringbean\"",
/* 350: */
" \"Blob\"",
" \"Globulus\"",
" \"Hulk\"",
" \"Raisin\"",
" \"Starlight\"",
" \"Giant's Toothpick\"",
" \"Pendulum\"",
" \"Backscratcher\"",
" \"Brush\"",
" \"Murmer\"",
/* 360: */
" \"Sarcophage\"",
" \"Concordance\"",
" \"Dragon's Tongue\"",
" \"Arbiter\"",
" \"Gram\"",
" \"Grom\"",
" \"Grim\"",
" \"Grum\"",
" \"Rummage\"",
" \"Omelette\"",
/* 370: */
" \"Egg\"",
" \"Aubergine\"",
" \"Z\"",
" \"X\"",
" \"O\"",
" \"Ox\"",
" \"Death Rattle\"",
" \"Tattletale\"",
" \"Fish\"",
" \"Bung\"",
/* 380: */
" \"Arcanum\"",
" \"Mud Pie of Death\"",
" \"Transmigrator\"",
" \"Ultimatum\"",
" \"Earthworm\"",
" \"Worm\"",
" \"Worm's Wrath\"",
" \"Xom's Favour\"",
" \"Bingo\"",
" \"Leviticus\"",
// Not yet possible...
/* 390: */
" of Joyful Slaughter",
" of ",
" of ",
" of ",
" of ",
" of ",
" of ",
" of ",
" of ",
" of ",



/* 390: */
"\"\"",
"\"\"",
"\"\"",
"\"\"",
"\"\"",
"\"\"",
"\"\"",
"\"\"",
"\"\"",
"\"\"",






/* 340: */
" of ",
" of ",
" of ",
" of ",
" of ",
" of ",
" of ",
" of ",
" of ",
" of ",












/* 200: */
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
};


char *rand_armour_names[] = {
/* 0: */
" of Shielding",
" of Grace",
" of Impermeability",
" of the Onion",
" of Life",
" of Defence",
" of Nonsense",
" of Eternal Vigilance",
" of Fun",
" of Joy",
/* 10: */
" of Death's Door",
" of the Gate",
" of Watchfulness",
" of Integrity",
" of Bodily Harmony",
" of Harmony",
" of the Untouchables",
" of Grot",
" of Grottiness",
" of Filth",
/* 20: */
" of Wonder",
" of Wondrous Power",
" of Power",
" of Vlad",
" of the Eternal Fruit",
" of Invincibility",
" of Hide-and-Seek",
" of the Mouse",
" of the Saviour",
" of Plasticity",
/* 30: */
" of Baldness",
" of Terror",
" of the Arcane",
" of Resist Death",
" of Anaesthesia",
" of the Guardian",
" of Inviolability",
" of the Tortoise",
" of the Turtle",
" of the Armadillo",
/* 40: */
" of the Echidna",
" of the Armoured One",
" of Weirdness",
" of Pathos",
" of Serendipity",
" of Loss",
" of Hedging",
" of Indemnity",
" of Limitation",
" of Exclusion",
/* 50: */
" of Repulsion",
" of Untold Secrets",
" of the Earth",
" of the Turtledove",
" of Limited Liability",
" of Responsibility",
" of Hadjma",
" of Glory",
" of Preservation",
" of Conservation",
/* 60: */
" of Protective Custody",
" of the Clam",
" of the Barnacle",
" of the Lobster",
" of Hairiness",
" of Supple Strength",
" of Space",
" of the Vacuum",
" of Compression",
" of Decompression",

/* 70: */
" of the Loofah",
" of ",
" of ",
" of ",
" of ",
" of ",
" of ",
" of ",
" of ",
" of ",
// Sarcophagus
};

/* Remember: disallow unrandart creation in abyss/pan */


/* The following unrandart bits were taken from $pellbinder's mstruct code
(see mstruct.h & mstruct.cc) and modified (LH). They're in randart.cc and
not randart.h because they're only used in this code module. */
#ifdef MAC
#define PACKED
#else
#define PACKED __attribute__ ((packed))
#endif
typedef char MYCHAR; // used for flags and the like
typedef unsigned char MYUCHAR; // used for flags and the like
typedef short MYSHORT; // used for flags and the like
typedef int MYINT; // used for flags and the like

//extern int unranddatasize;
//extern struct unrandart_entry {
int unranddatasize;

struct unrandart_entry {

        char *name/*[32]*/PACKED; // true name of unrandart (max 31 chars)
        char *unid_name/*[32]*/PACKED; // un-id'd name of unrandart (max 31 chars)
        unsigned char ura_cl PACKED; // class of ura
        unsigned char ura_ty PACKED; // type of ura
        unsigned char ura_pl PACKED; // plus of ura
        unsigned char ura_pl2 PACKED; // plus2 of ura
        unsigned char ura_col PACKED; // colour of ura
        short prpty [RA_PROPERTIES] PACKED;
        char *spec_descrip1/*[32]*/PACKED; // A special description which is added to the 'V' command output (max 31 chars)
        char *spec_descrip2/*[32]*/PACKED; // A special description which is added to the 'V' command output (max 31 chars)
        char *spec_descrip3/*[32]*/PACKED; // A special description which is added to the 'V' command output (max 31 chars)
};// unranddata[];



struct unrandart_entry unranddata[]={
#include "unrand.h"
};

#define sura seekunrandart(aclass, atype, aplus, aplus2)

struct unrandart_entry *seekunrandart(unsigned char aclass, unsigned char atype, unsigned char aplus, unsigned char aplus2);

char *art_n;

char unrandart_exist [NO_UNRANDARTS];
void set_unrandart_exist(int whun, char is_exist);

void standard_name_weap(unsigned char item_typ, char glog [80]);
void standard_name_armour(unsigned char item_typ, unsigned char item_plus2, char glorg [80]);

int random4(unsigned int randmax);



void set_unrandart_exist(int whun, char is_exist)
{
 unrandart_exist [whun] = is_exist;
}

char does_unrandart_exist(int whun)
{
 return unrandart_exist [whun];
}


int randart_wpn_properties(unsigned char aclass, unsigned char atype, unsigned char adam, unsigned char aplus, unsigned char aplus2, unsigned char acol, char prop)
{

if ((aclass == OBJ_JEWELLERY && adam == 201) || (aclass != OBJ_JEWELLERY && adam == 25))
{
 struct unrandart_entry *search_unrandarts = sura;
 return search_unrandarts->prpty [prop];
}

long globby = aclass * adam + acol + atype * (aplus % 100) + aplus2 * 100;
long randstore = random();
char proprt [RA_PROPERTIES];
int i = 0;
int power_level = ((aplus % 100) - 50) / 3 + (aplus2 - 50) / 3;

srandom(globby);

if (aclass == OBJ_ARMOUR) power_level = ((aplus % 100) - 50) / 2 + 2;

if (aclass == OBJ_JEWELLERY) power_level = 1 + random4(3) + random4(2);

if (power_level < 0) power_level = 0;

for (i = 0; i < RA_PROPERTIES; i ++)
{
 proprt [i] = 0;
}

if (aclass == OBJ_WEAPONS) /* Only weapons get brands, of course */
{
 proprt [RAP_BRAND] = SPWPN_FLAMING + random4(15); /* brand */
 if (random4(6) == 0) proprt [RAP_BRAND] = SPWPN_FLAMING + random4(2);
 if (random4(6) == 0) proprt [RAP_BRAND] = SPWPN_ORC_SLAYING + random4(4);
 if (random4(6) == 0) proprt [RAP_BRAND] = SPWPN_VORPAL;
 if (proprt [RAP_BRAND] == SPWPN_FLAME || proprt [RAP_BRAND] == SPWPN_FROST) proprt [RAP_BRAND] = 0; /* missile wpns */
 if (proprt [RAP_BRAND] == SPWPN_PROTECTION) proprt [RAP_BRAND] = 0; /* no protection */
 if (proprt [RAP_BRAND] == SPWPN_DISRUPTION && atype != WPN_MACE) proprt [RAP_BRAND] = SPWPN_NORMAL; /* Only maces get disruption */

 if (atype >= WPN_SLING && atype <= WPN_HAND_CROSSBOW)
 {
  if (random4(3) == 0) proprt [RAP_BRAND] = SPWPN_FLAME + random4(2); else proprt [RAP_BRAND] = SPWPN_NORMAL;
 }

 if (atype == WPN_DEMON_BLADE || atype == WPN_DEMON_WHIP)
 {
  switch(random4(9))
  {
   case 0: proprt [RAP_BRAND] = SPWPN_DRAINING; break;
   case 1: proprt [RAP_BRAND] = SPWPN_FLAMING; break;
   case 2: proprt [RAP_BRAND] = SPWPN_FREEZING; break;
   case 3: proprt [RAP_BRAND] = SPWPN_ELECTROCUTION; break;
   case 4: proprt [RAP_BRAND] = SPWPN_VAMPIRICISM; break;
   case 5: proprt [RAP_BRAND] = SPWPN_PAIN; break;
   case 6: proprt [RAP_BRAND] = SPWPN_VENOM; break;
   default: power_level -= 2;
  }
  power_level += 2;
 } else if (random4(3) == 0) proprt [RAP_BRAND] = SPWPN_NORMAL; else power_level ++;
}

if (random4(5) == 0) goto skip_mods;

if (random4(4 + power_level) == 0 && aclass != OBJ_ARMOUR) /* AC mod - not for armours */
{
 proprt [RAP_AC] = 1 + random4(3) + random4(3) + random4(3);
 power_level ++;
 if (random4(4) == 0)
 {
  proprt [RAP_AC] -= 1 + random4(3) + random4(3) + random4(3);
  power_level --;
 }
}

if (random4(4 + power_level) == 0) /* ev mod */
{
 proprt [RAP_EVASION] = 1 + random4(3) + random4(3) + random4(3);
 power_level ++;
 if (random4(4) == 0)
 {
  proprt [RAP_EVASION] -= 1 + random4(3) + random4(3) + random4(3);
  power_level --;
 }
}

if (random4(4 + power_level) == 0) /* str mod */
{
 proprt [RAP_STRENGTH] = 1 + random4(3) + random4(2);
 power_level ++;
 if (random4(4) == 0)
 {
  proprt [RAP_STRENGTH] -= 1 + random4(3) + random4(3) + random4(3);
  power_level --;
 }
}

if (random4(4 + power_level) == 0) /* int mod */
{
 proprt [RAP_INTELLIGENCE] = 1 + random4(3) + random4(2);
 power_level ++;
 if (random4(4) == 0)
 {
  proprt [RAP_INTELLIGENCE] -= 1 + random4(3) + random4(3) + random4(3);
  power_level --;
 }
}

if (random4(4 + power_level) == 0) /* dex mod */
{
 proprt [RAP_DEXTERITY] = 1 + random4(3) + random4(2);
 power_level ++;
 if (random4(4) == 0)
 {
  proprt [RAP_DEXTERITY] -= 1 + random4(3) + random4(3) + random4(3);
  power_level --;
 }
}

skip_mods : if (random4(15) < power_level || aclass == OBJ_WEAPONS) goto skip_combat;
/* Weapons can't get these */

if (random4(4 + power_level) == 0) /* to-hit */
{
 proprt [RAP_ACCURACY] = 1 + random4(3) + random4(2);
 power_level ++;
 if (random4(4) == 0)
 {
  proprt [RAP_ACCURACY] -= 1 + random4(3) + random4(3) + random4(3);
  power_level --;
 }
}

if (random4(4 + power_level) == 0) /* to-dam */
{
 proprt [RAP_DAMAGE] = 1 + random4(3) + random4(2);
 power_level ++;
 if (random4(4) == 0)
 {
  proprt [RAP_DAMAGE] -= 1 + random4(3) + random4(3) + random4(3);
  power_level --;
 }
}

skip_combat : if (random4(12) < power_level) goto finished_powers;

/* res_fire */
if (random4(4 + power_level) == 0 && (aclass != OBJ_JEWELLERY || (atype != RING_PROTECTION_FROM_FIRE && atype != RING_FIRE && atype != RING_ICE)) && (aclass != OBJ_ARMOUR || (atype != ARM_DRAGON_ARMOUR && atype != ARM_ICE_DRAGON_ARMOUR && atype != ARM_GOLD_DRAGON_ARMOUR)))
{
 proprt [RAP_FIRE] = 1;
 if (random4(5) == 0) proprt [RAP_FIRE] += 1;
 power_level ++;
}

/* res_cold */
if (random4(4 + power_level) == 0 && (aclass != OBJ_JEWELLERY || (atype != RING_PROTECTION_FROM_COLD && atype != RING_FIRE && atype != RING_ICE)) && (aclass != OBJ_ARMOUR || (atype != ARM_DRAGON_ARMOUR && atype != ARM_ICE_DRAGON_ARMOUR && atype != ARM_GOLD_DRAGON_ARMOUR)))
{
 proprt [RAP_COLD] = 1;
 if (random4(5) == 0) proprt [RAP_COLD] += 1;
 power_level ++;
}

if (random4(12) < power_level || power_level > 7) goto finished_powers;

/* res_elec */
if (random4(4 + power_level) == 0 && (aclass != OBJ_ARMOUR || atype != ARM_STORM_DRAGON_ARMOUR))
{
 proprt [RAP_ELECTRICITY] = 1;
 power_level ++;
}
/* res_poison */
if (random4(5 + power_level) == 0 && (aclass != OBJ_JEWELLERY || atype != RING_POISON_RESISTANCE) && (aclass != OBJ_ARMOUR || atype != ARM_GOLD_DRAGON_ARMOUR || atype != ARM_SWAMP_DRAGON_ARMOUR))
{
 proprt [RAP_POISON] = 1;
 power_level ++;
}

/* prot_life - no necromantic brands on weapons allowed */
if (random4(4 + power_level) == 0  && (aclass != OBJ_JEWELLERY || atype != RING_TELEPORTATION) && proprt [RAP_BRAND] != SPWPN_DRAINING && proprt [RAP_BRAND] != SPWPN_VAMPIRICISM && proprt [RAP_BRAND] != SPWPN_PAIN)
{
 proprt [RAP_NEGATIVE_ENERGY] = 1;
 power_level ++;
}

if (random4(4 + power_level) == 0 && (aclass != OBJ_JEWELLERY || atype != RING_PROTECTION_FROM_MAGIC)) /* res magic */
{
 proprt [RAP_MAGIC] = 20 + random4(100);
 power_level ++;
}

/* see_invis */
if (random4(4 + power_level) == 0 && (aclass != OBJ_JEWELLERY || atype != RING_INVISIBILITY))
{
 proprt [RAP_EYESIGHT] = 1;
 power_level ++;
}

if (random4(12) < power_level || power_level > 10) goto finished_powers;

/* turn invis */
if (random4(10) == 0 && (aclass != OBJ_JEWELLERY || atype != RING_INVISIBILITY))
{
 proprt [RAP_INVISIBLE] = 1;
 power_level ++;
}

if (random4(10) == 0 && (aclass != OBJ_JEWELLERY || atype != RING_LEVITATION)) /* levitate */
{
 proprt [RAP_LEVITATE] = 1;
 power_level ++;
}

if (random4(10) == 0) /* blink */
{
 proprt [RAP_BLINK] = 1;
 power_level ++;
}

if (random4(10) == 0 && (aclass != OBJ_JEWELLERY || atype != RING_TELEPORTATION)) /* teleport */
{
 proprt [RAP_CAN_TELEPORT] = 1;
 power_level ++;
}

if (random4(10) == 0 && (aclass != OBJ_JEWELLERY || atype != AMU_RAGE)) /* go berserk */
{
 proprt [RAP_BERSERK] = 1;
 power_level ++;
}

if (random4(10) == 0) /* sense surr */
{
 proprt [RAP_MAPPING] = 1;
 power_level ++;
}


finished_powers :

if (aclass == OBJ_ARMOUR) power_level -= 4; /* Armours get less powers, and are also less likely to be cursed that wpns */

if (random4(17) >= power_level || power_level < 2) goto finished_curses;

switch(random4(9))
{
case 0: /* makes noise */
if (aclass != 0) break;
proprt [RAP_NOISES] = 1 + random4(4);
break;
case 1: /* no magic */
proprt [RAP_PREVENT_SPELLCASTING] = 1;
break;
case 2: /* random teleport */
if (aclass != OBJ_WEAPONS) break;
proprt [RAP_CAUSE_TELEPORTATION] = 5 + random4(15);
break;
case 3: /* no teleport - doesn't affect some instantaneous teleports */
if (aclass == OBJ_JEWELLERY && atype == RING_TELEPORTATION) break; /* already is a ring of tport */
if (aclass == OBJ_JEWELLERY && atype == RING_TELEPORT_CONTROL) break; /* already is a ring of tport ctrl */
proprt [RAP_BLINK] = 0;
proprt [RAP_CAN_TELEPORT] = 0;
proprt [RAP_PREVENT_TELEPORTATION] = 1;
break;
case 4: /* berserk on attack */
if (aclass != OBJ_WEAPONS) break;
proprt [RAP_ANGRY] = 1 + random4(8);
break;
case 5: /* susceptible to fire */
if (aclass == OBJ_JEWELLERY && (atype == RING_PROTECTION_FROM_FIRE || atype == RING_FIRE || atype == RING_ICE)) break; /* already does this or something */
if (aclass == OBJ_ARMOUR && (atype == ARM_DRAGON_ARMOUR || atype == ARM_ICE_DRAGON_ARMOUR || atype == ARM_GOLD_DRAGON_ARMOUR)) break;
proprt [RAP_FIRE] = -1;
break;
case 6: /* susceptible to cold */
if (aclass == OBJ_JEWELLERY && (atype == RING_PROTECTION_FROM_COLD || atype == RING_FIRE || atype == RING_ICE)) break; /* already does this or something */
if (aclass == OBJ_ARMOUR && (atype == ARM_DRAGON_ARMOUR || atype == ARM_ICE_DRAGON_ARMOUR || atype == ARM_GOLD_DRAGON_ARMOUR)) break;
proprt [RAP_COLD] = -1;
break;
case 7: /* speed metabolism */
if (aclass == OBJ_JEWELLERY && atype == RING_HUNGER) break; /* already is a ring of hunger */
if (aclass == OBJ_JEWELLERY && atype == RING_SUSTENANCE) break; /* already is a ring of sustenance */
proprt [RAP_METABOLISM] = 1 + random4(3);
break;
case 8: /* emits mutagenic radiation - increases mpower */
proprt [RAP_MUTAGENIC] = 2 + random4(4); /* property is chance (1 in ...) of increasing mpower */
break;
}
/*
26 - +to-hit (no wpns)
27 - +to-dam (no wpns)
*/

finished_curses :

srandom(randstore);

return proprt [prop];

}



char *randart_name(unsigned char aclass, unsigned char atype, unsigned char adam, unsigned char aplus, unsigned char aplus2, unsigned char ident_lev)
{

if (adam == 25)
{
 struct unrandart_entry *search_unrandarts = sura;
 if (ident_lev == 0) return search_unrandarts->unid_name;
  else return search_unrandarts->name;
}


free(art_n);
art_n = (char *)malloc(sizeof(char) * 80);
if (art_n == NULL)
{
 return "Malloc Failed Error";
}
strcpy(art_n, "");

long globby = aclass + adam * (aplus % 100) + atype * aplus2;
long randstore = random();

srandom(globby);

if (ident_lev == 0)
{
 switch(random4(21))
 {
  case 0: strcat(art_n, "brightly glowing "); break;
  case 1: strcat(art_n, "runed "); break;
  case 2: strcat(art_n, "smoking "); break;
  case 3: strcat(art_n, "bloodstained "); break;
  case 4: strcat(art_n, "twisted "); break;
  case 5: strcat(art_n, "shimmering "); break;
  case 6: strcat(art_n, "warped "); break;
  case 7: strcat(art_n, "crystal "); break;
  case 8: strcat(art_n, "jewelled "); break;
  case 9: strcat(art_n, "transparent "); break;
  case 10: strcat(art_n, "encrusted "); break;
  case 11: strcat(art_n, "pitted "); break;
  case 12: strcat(art_n, "slimy "); break;
  case 13: strcat(art_n, "polished "); break;
  case 14: strcat(art_n, "fine "); break;
  case 15: strcat(art_n, "crude "); break;
  case 16: strcat(art_n, "ancient "); break;
  case 17: strcat(art_n, "ichor-stained "); break;
  case 18: strcat(art_n, "faintly glowing "); break;
  case 19: strcat(art_n, "steaming "); break;
  case 20: strcat(art_n, "shiny "); break;
 }
 char st_p3 [80];
 standard_name_weap(atype, st_p3);
 strcat(art_n, st_p3);
 srandom(randstore);
 return art_n;
}

char st_p [50];

if (random4(2) == 0)
{
 char st_p [80];
 standard_name_weap(atype, st_p);
 strcat(art_n, st_p);
 strcat(art_n, rand_wpn_names [random4(390)]);
}
 else
{
 char st_p2 [80];
 make_name(random4(250), random4(250), random4(250), 3, st_p);
 standard_name_weap(atype, st_p2);
 strcat(art_n, st_p2);
 if (random4(3) == 0)
  {
   strcat(art_n, " of ");
   strcat(art_n, st_p);
  }
  else
  {
   strcat(art_n, " \"");
   strcat(art_n, st_p);
   strcat(art_n, "\"");
  }
}

srandom(randstore);

return art_n;

}



char *randart_armour_name(unsigned char aclass, unsigned char atype, unsigned char adam, unsigned char aplus, unsigned char aplus2, unsigned char ident_lev)
{

if (adam == 25)
{
 struct unrandart_entry *search_unrandarts = sura;
 if (ident_lev == 0) return search_unrandarts->unid_name;
  else return search_unrandarts->name;
}


free(art_n);
art_n = (char *)malloc(sizeof(char) * 80);
if (art_n == NULL)
{
 return "Malloc Failed Error";
}
strcpy(art_n, "");

long globby = aclass + adam * (aplus % 100) + atype * aplus2;
long randstore = random();

srandom(globby);

if (ident_lev == 0)
{
 switch(random4(21))
 {
  case 0: strcat(art_n, "brightly glowing "); break;
  case 1: strcat(art_n, "runed "); break;
  case 2: strcat(art_n, "smoking "); break;
  case 3: strcat(art_n, "bloodstained "); break;
  case 4: strcat(art_n, "twisted "); break;
  case 5: strcat(art_n, "shimmering "); break;
  case 6: strcat(art_n, "warped "); break;
  case 7: strcat(art_n, "heavily runed "); break;
  case 8: strcat(art_n, "jewelled "); break;
  case 9: strcat(art_n, "transparent "); break;
  case 10: strcat(art_n, "encrusted "); break;
  case 11: strcat(art_n, "pitted "); break;
  case 12: strcat(art_n, "slimy "); break;
  case 13: strcat(art_n, "polished "); break;
  case 14: strcat(art_n, "fine "); break;
  case 15: strcat(art_n, "crude "); break;
  case 16: strcat(art_n, "ancient "); break;
  case 17: strcat(art_n, "ichor-stained "); break;
  case 18: strcat(art_n, "faintly glowing "); break;
  case 19: strcat(art_n, "steaming "); break;
  case 20: strcat(art_n, "shiny "); break;
 }
 char st_p3 [80];
 standard_name_armour(atype, aplus2, st_p3);
 strcat(art_n, st_p3);
 srandom(randstore);
 return art_n;
}

char st_p [50];

if (random4(2) == 0)
{
 char st_p [80];
 standard_name_armour(atype, aplus2, st_p);
 strcat(art_n, st_p);
 strcat(art_n, rand_armour_names [random4(71)]);
}
 else
{
 char st_p2 [80];
 make_name(random4(250), random4(250), random4(250), 3, st_p);
 standard_name_armour(atype, aplus2, st_p2);
 strcat(art_n, st_p2);
 if (random4(3) == 0)
  {
   strcat(art_n, " of ");
   strcat(art_n, st_p);
  }
  else
  {
   strcat(art_n, " \"");
   strcat(art_n, st_p);
   strcat(art_n, "\"");
  }
}

srandom(randstore);

return art_n;

}


char *randart_ring_name(unsigned char aclass, unsigned char atype, unsigned char adam, unsigned char aplus, unsigned char aplus2, unsigned char ident_lev)
{

if (adam == 201)
{
 struct unrandart_entry *search_unrandarts = sura;
 if (ident_lev == 0) return search_unrandarts->unid_name;
  else return search_unrandarts->name;
}

char st_p [80];

free(art_n);
art_n = (char *)malloc(sizeof(char) * 80);
if (art_n == NULL)
{
 return "Malloc Failed Error";
}
strcpy(art_n, "");

long globby = aclass + adam * (aplus % 100) + atype * aplus2;
long randstore = random();

srandom(globby);

if (ident_lev == 0)
{
 switch(random4(21))
 {
  case 0: strcat(art_n, "brightly glowing "); break;
  case 1: strcat(art_n, "runed "); break;
  case 2: strcat(art_n, "smoking "); break;
  case 3: strcat(art_n, "ruby "); break;
  case 4: strcat(art_n, "twisted "); break;
  case 5: strcat(art_n, "shimmering "); break;
  case 6: strcat(art_n, "warped "); break;
  case 7: strcat(art_n, "crystal "); break;
  case 8: strcat(art_n, "diamond "); break;
  case 9: strcat(art_n, "transparent "); break;
  case 10: strcat(art_n, "encrusted "); break;
  case 11: strcat(art_n, "pitted "); break;
  case 12: strcat(art_n, "slimy "); break;
  case 13: strcat(art_n, "polished "); break;
  case 14: strcat(art_n, "fine "); break;
  case 15: strcat(art_n, "crude "); break;
  case 16: strcat(art_n, "ancient "); break;
  case 17: strcat(art_n, "emerald "); break;
  case 18: strcat(art_n, "faintly glowing "); break;
  case 19: strcat(art_n, "steaming "); break;
  case 20: strcat(art_n, "shiny "); break;
 }
 if (atype < AMU_RAGE) strcat(art_n, "ring"); else strcat(art_n, "amulet");
 srandom(randstore);
 return art_n;
}

if (random4(5) == 0)
{
 if (atype < AMU_RAGE) strcat(art_n, "ring"); else strcat(art_n, "amulet");
 strcat(art_n, rand_armour_names [random4(71)]);
}
 else
{
 make_name(random4(250), random4(250), random4(250), 3, st_p);
 if (atype < AMU_RAGE) strcat(art_n, "ring"); else strcat(art_n, "amulet");
 if (random4(3) == 0)
  {
   strcat(art_n, " of ");
   strcat(art_n, st_p);
  }
  else
  {
   strcat(art_n, " \"");
   strcat(art_n, st_p);
   strcat(art_n, "\"");
  }
}

srandom(randstore);

return art_n;

}


struct unrandart_entry *seekunrandart(unsigned char aclass, unsigned char atype, unsigned char aplus, unsigned char aplus2)
{

int x = 0;

        while(x < NO_UNRANDARTS)
    {
                if (unranddata [x].ura_cl == aclass && unranddata [x].ura_ty == atype && unranddata [x].ura_pl % 100 == aplus % 100 && unranddata [x].ura_pl2 == aplus2)
        {
          return &unranddata [x];
        }
                x++;
        }
        return seekunrandart(250, 250, 250, 250); /* Should *never* happen */

}

int find_okay_unrandart(unsigned char aclass)
{
 int x = 0;
 char ura_index [NO_UNRANDARTS];
 for (x = 0; x < NO_UNRANDARTS; x ++)
 {
   ura_index [x] = 0;
 }
 int counter = 0;

 x = 0;

        while(x < NO_UNRANDARTS)
    {
                if (unranddata [x].ura_cl == aclass && does_unrandart_exist(x) == 0)
        {
         ura_index [counter] = x;
         counter ++;
        }
                x++;
        }

 if (counter == 0) return -1;

 int y = ura_index [random4(counter)];

/* output_value("Found randart no: ", y); */

 return y;
}

void make_item_unrandart(int x, int ura_item)
{

 mitm.iclass [ura_item] = unranddata [x].ura_cl;
 mitm.itype [ura_item] = unranddata [x].ura_ty;
 mitm.iplus [ura_item] = unranddata [x].ura_pl;
 mitm.iplus2 [ura_item] = unranddata [x].ura_pl2;
 mitm.icol [ura_item] = unranddata [x].ura_col;
 mitm.idam [ura_item] = 25;
 if (mitm.iclass [ura_item] == OBJ_JEWELLERY) mitm.idam [ura_item] = 201;
 set_unrandart_exist(x, 1);

}

char *unrandart_descrip(char which_descrip, unsigned char aclass, unsigned char atype, unsigned char aplus, unsigned char aplus2)
{
 switch(which_descrip)
 {
  case 0: return sura->spec_descrip1;
  case 1: return sura->spec_descrip2;
  case 2: return sura->spec_descrip3;
 }

 return "Unknown.";

/* Eventually it would be great to have randomly generated descriptions for
randarts. */

}


void standard_name_weap(unsigned char item_typ, char glorg [80])
{

strcpy(glorg, "");

        switch(item_typ)
        {
        case WPN_CLUB: strcat(glorg , "club"); break;
        case WPN_MACE: strcat(glorg , "mace"); break;
        case WPN_FLAIL: strcat(glorg , "flail"); break;
        case WPN_DAGGER: strcat(glorg , "dagger"); break;
        case WPN_MORNINGSTAR: strcat(glorg , "morningstar"); break;
        case WPN_SHORT_SWORD: strcat(glorg , "short sword"); break;
        case WPN_LONG_SWORD: strcat(glorg , "long sword"); break;
        case WPN_GREAT_SWORD: strcat(glorg , "great sword"); break;
        case WPN_SCIMITAR: strcat(glorg , "scimitar"); break;
        case WPN_HAND_AXE: strcat(glorg , "hand axe"); break;
        case WPN_BATTLEAXE: strcat(glorg , "battleaxe"); break;
        case WPN_SPEAR: strcat(glorg , "spear"); break;
        case WPN_HALBERD: strcat(glorg , "halberd"); break;
        case WPN_SLING: strcat(glorg , "sling"); break;
        case WPN_BOW: strcat(glorg , "bow"); break;
        case WPN_CROSSBOW: strcat(glorg , "crossbow"); break;
    case WPN_HAND_CROSSBOW: strcat(glorg , "hand crossbow"); break;
    case WPN_GLAIVE: strcat(glorg , "glaive"); break;
        case WPN_QUARTERSTAFF: strcat(glorg , "quarterstaff"); break;
    case WPN_SCYTHE: strcat(glorg , "scythe"); break;
        case WPN_GIANT_CLUB: strcat(glorg , "giant club"); break;
        case WPN_GIANT_SPIKED_CLUB: strcat(glorg , "giant spiked club"); break;
        case WPN_EVENINGSTAR: strcat(glorg , "eveningstar"); break;
        case WPN_QUICK_BLADE: strcat(glorg , "quick blade"); break;
        case WPN_KATANA: strcat(glorg , "katana"); break;
        case WPN_EXECUTIONERS_AXE: strcat(glorg , "executioner's axe"); break;
        case WPN_DOUBLE_SWORD: strcat(glorg , "double sword"); break;
        case WPN_TRIPLE_SWORD: strcat(glorg , "triple sword"); break;
        case WPN_HAMMER: strcat(glorg , "hammer"); break;
        case WPN_ANCUS: strcat(glorg , "ancus"); break;
        case WPN_WHIP: strcat(glorg , "whip"); break;
        case WPN_SABRE: strcat(glorg , "sabre"); break;
        case WPN_DEMON_BLADE: strcat(glorg , "demon blade"); break;
        case WPN_DEMON_WHIP: strcat(glorg , "demon whip"); break;
        case 34: strcat(glorg , ""); break;
        case 35: strcat(glorg , ""); break;
        case 36: strcat(glorg , ""); break;
        case 37: strcat(glorg , ""); break;
        case 38: strcat(glorg , ""); break;
        }

}

void standard_name_armour(unsigned char item_typ, unsigned char item_plus2, char glorg [80])
{

 strcpy(glorg, "");


 switch(item_typ)
 {
        case ARM_ROBE: strcat(glorg , "robe"); break;
        case ARM_LEATHER_ARMOUR: strcat(glorg , "leather armour"); break;
        case ARM_RING_MAIL: strcat(glorg , "ring mail"); break;
        case ARM_SCALE_MAIL: strcat(glorg , "scale mail"); break;
        case ARM_CHAIN_MAIL: strcat(glorg , "chain mail"); break;
        case ARM_SPLINT_MAIL: strcat(glorg , "splint mail"); break;
        case ARM_BANDED_MAIL: strcat(glorg , "banded mail"); break;
        case ARM_PLATE_MAIL: strcat(glorg , "plate mail"); break;
        case ARM_SHIELD: strcat(glorg , "shield"); break;
        case ARM_CLOAK: strcat(glorg , "cloak"); break;
        break;
        case ARM_HELMET:
        if (item_plus2 == 0) strcat(glorg , "helmet");
        if (item_plus2 == 1) strcat(glorg , "helm");
        if (item_plus2 == 2) strcat(glorg , "cap");
        if (item_plus2 == 3) strcat(glorg , "wizard's hat");
        break;
        case ARM_GLOVES: strcat(glorg , "gloves");
        break;
        case ARM_BOOTS: if (item_plus2 == 0) strcat(glorg, "boots");
                 if (item_plus2 == 1) strcat(glorg, "naga barding");
                 if (item_plus2 == 2) strcat(glorg, "centaur barding");
        break;
        case ARM_BUCKLER: strcat(glorg , "buckler"); break;
        case ARM_LARGE_SHIELD: strcat(glorg , "large shield"); break;
    case ARM_DRAGON_HIDE: strcat(glorg , "dragon hide"); break;
    case ARM_TROLL_HIDE: strcat(glorg , "troll hide"); break;
    case ARM_CRYSTAL_PLATE_MAIL: strcat(glorg , "crystal plate mail"); break;
    case ARM_DRAGON_ARMOUR: strcat(glorg , "dragon armour"); break;
    case ARM_TROLL_LEATHER_ARMOUR: strcat(glorg , "troll leather armour"); break;
    case ARM_ICE_DRAGON_HIDE: strcat(glorg , "ice dragon hide"); break;
    case ARM_ICE_DRAGON_ARMOUR: strcat(glorg , "ice dragon armour"); break;
    case ARM_STEAM_DRAGON_HIDE: strcat(glorg , "steam dragon hide"); break;
    case ARM_STEAM_DRAGON_ARMOUR: strcat(glorg , "steam dragon armour"); break;
    case ARM_MOTTLED_DRAGON_HIDE: strcat(glorg , "mottled dragon hide"); break;
    case ARM_MOTTLED_DRAGON_ARMOUR: strcat(glorg , "mottled dragon armour"); break;
    case ARM_STORM_DRAGON_HIDE: strcat(glorg , "storm dragon hide"); break;
    case ARM_STORM_DRAGON_ARMOUR: strcat(glorg , "storm dragon armour"); break;
    case ARM_GOLD_DRAGON_HIDE: strcat(glorg , "gold dragon hide"); break;
    case ARM_GOLD_DRAGON_ARMOUR: strcat(glorg , "gold dragon armour"); break;
    case ARM_ANIMAL_SKIN: strcat(glorg , "animal skin"); break;
    case ARM_SWAMP_DRAGON_HIDE: strcat(glorg , "swamp dragon hide"); break;
    case ARM_SWAMP_DRAGON_ARMOUR: strcat(glorg , "swamp dragon armour"); break;
 }

}



int random4(unsigned int randmax)
{
 if (randmax <= 0) return 0;
 return random() % randmax;
}

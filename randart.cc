#include "config.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "externs.h"

#include "itemname.h"
#include "stuff.h"

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

char *art_n;

void standard_name_weap(unsigned char item_typ, char glog [80]);
void standard_name_armour(unsigned char item_typ, unsigned char item_plus2, char glorg [80]);

int random4(unsigned int randmax);


int randart_wpn_properties(unsigned char aclass, unsigned char atype, unsigned char adam, unsigned char aplus, unsigned char aplus2, unsigned char acol, char prop)
{

long globby = aclass * adam + acol + atype * (aplus % 100) + aplus2 * 100;
long randstore = random();
char proprt [30];
int i = 0;
int power_level = (aplus - 50) / 3 + (aplus2 - 50) / 3;

srandom(globby);

if (aclass == 2) power_level = (aplus - 50) / 2 + 4;

if (aclass == 7) power_level = 1 + random4(4) + random4(4);

if (power_level < 0) power_level = 0;

for (i = 0; i < 30; i ++)
{
 proprt [i] = 0;
}

if (aclass == 0) /* Only weapons get brands, of course */
{
 proprt [0] = 1 + random4(15); /* brand */
 if (random4(6) == 0) proprt [0] = 1 + random4(2);
 if (random4(6) == 0) proprt [0] = 5 + random4(4);
 if (random4(6) == 0) proprt [0] = 10;
 if (proprt [0] == 11 || proprt [0] == 12) proprt [0] = 0; /* missile wpns */
 if (proprt [0] == 7) proprt [0] = 0; /* no protection */
 if (proprt [0] == 14 && atype != 1) proprt [0] = 0; /* Only maces get disruption */

 if (atype >= 13 && atype <= 16)
 {
  if (random4(3) == 0) proprt [0] = 11 + random4(2); else proprt [0] = 0;
 }

 if (atype == 32 || atype == 33)
 {
  switch(random4(9))
  {
   case 0: proprt [0] = 8; break;
   case 1: proprt [0] = 1; break;
   case 2: proprt [0] = 2; break;
   case 3: proprt [0] = 4; break;
   case 4: proprt [0] = 13; break;
   case 5: proprt [0] = 15; break;
   case 6: proprt [0] = 6; break;
   default: power_level -= 2;
  }
  power_level += 2;
 } else if (random4(2) == 0) proprt [0] = 0; else power_level ++;
}

if (random4(5) == 0) goto skip_mods;

if (random4(4 + power_level) == 0 && aclass != 2) /* AC mod - not for armours */
{
 proprt [1] = 1 + random4(3) + random4(3) + random4(3);
 power_level ++;
 if (random4(4) == 0)
 {
  proprt [1] -= 1 + random4(3) + random4(3) + random4(3);
  power_level --;
 }
}

if (random4(4 + power_level) == 0) /* ev mod */
{
 proprt [2] = 1 + random4(3) + random4(3) + random4(3);
 power_level ++;
 if (random4(4) == 0)
 {
  proprt [2] -= 1 + random4(3) + random4(3) + random4(3);
  power_level --;
 }
}

if (random4(4 + power_level) == 0) /* str mod */
{
 proprt [3] = 1 + random4(3) + random4(2);
 power_level ++;
 if (random4(4) == 0)
 {
  proprt [3] -= 1 + random4(3) + random4(3) + random4(3);
  power_level --;
 }
}

if (random4(4 + power_level) == 0) /* int mod */
{
 proprt [4] = 1 + random4(3) + random4(2);
 power_level ++;
 if (random4(4) == 0)
 {
  proprt [4] -= 1 + random4(3) + random4(3) + random4(3);
  power_level --;
 }
}

if (random4(4 + power_level) == 0) /* dex mod */
{
 proprt [5] = 1 + random4(3) + random4(2);
 power_level ++;
 if (random4(4) == 0)
 {
  proprt [5] -= 1 + random4(3) + random4(3) + random4(3);
  power_level --;
 }
}

skip_mods : if (random4(15) < power_level || aclass == 0) goto skip_combat;
/* Weapons can't get these */

if (random4(4 + power_level) == 0) /* to-hit */
{
 proprt [26] = 1 + random4(3) + random4(2);
 power_level ++;
 if (random4(4) == 0)
 {
  proprt [26] -= 1 + random4(3) + random4(3) + random4(3);
  power_level --;
 }
}

if (random4(4 + power_level) == 0) /* to-dam */
{
 proprt [27] = 1 + random4(3) + random4(2);
 power_level ++;
 if (random4(4) == 0)
 {
  proprt [27] -= 1 + random4(3) + random4(3) + random4(3);
  power_level --;
 }
}

skip_combat : if (random4(12) < power_level) goto finished_powers;

/* res_fire */
if (random4(4 + power_level) == 0 && (aclass != 7 || (atype != 2 && atype != 21 && atype != 22)) && (aclass != 2 || (atype != 18 && atype != 21 && atype != 29)))
{
 proprt [6] = 1;
 if (random4(5) == 0) proprt [6] += 1;
 power_level ++;
}

/* res_cold */
if (random4(4 + power_level) == 0 && (aclass != 7 || (atype != 4 && atype != 21 && atype != 22)) && (aclass != 2 || (atype != 18 && atype != 21 && atype != 29)))
{
 proprt [7] = 1;
 if (random4(5) == 0) proprt [7] += 1;
 power_level ++;
}

if (random4(12) < power_level || power_level > 7) goto finished_powers;

/* res_elec */
if (random4(4 + power_level) == 0 && (aclass != 2 || atype != 27))
{
 proprt [8] = 1;
 power_level ++;
}
/* res_poison */
if (random4(5 + power_level) == 0 && (aclass != 7 || atype != 3) && (aclass != 2 || atype != 29 || atype != 32))
{
 proprt [9] = 1;
 power_level ++;
}

/* prot_life - no necromantic brands on weapons allowed */
if (random4(4 + power_level) == 0  && (aclass != 7 || atype != 10) && proprt [0] != 8 && proprt [0] != 13 && proprt [0] != 15)
{
 proprt [10] = 1;
 power_level ++;
}

if (random4(4 + power_level) == 0 && (aclass != 7 || atype != 20)) /* res magic */
{
 proprt [11] = 20 + random4(100);
 power_level ++;
}

/* see_invis */
if (random4(4 + power_level) == 0 && (aclass != 7 || atype != 8))
{
 proprt [12] = 1;
 power_level ++;
}

if (random4(12) < power_level || power_level > 10) goto finished_powers;

/* turn invis */
if (random4(10) == 0 && (aclass != 7 || atype != 8))
{
 proprt [13] = 1;
 power_level ++;
}

if (random4(10) == 0 && (aclass != 7 || atype != 18)) /* levitate */
{
 proprt [14] = 1;
 power_level ++;
}

if (random4(10) == 0) /* blink */
{
 proprt [15] = 1;
 power_level ++;
}

if (random4(10) == 0 && (aclass != 7 || atype != 10)) /* teleport */
{
 proprt [16] = 1;
 power_level ++;
}

if (random4(10) == 0 && (aclass != 7 || atype != 35)) /* go berserk */
{
 proprt [17] = 1;
 power_level ++;
}

if (random4(10) == 0) /* sense surr */
{
 proprt [18] = 1;
 power_level ++;
}



/*switch(prop)
{
case 0: / * Brand * /
return brand;

case 1: / * AC mod * /
if (random4(5) == 0) return 1 + random4(3) + random4(3) + random4(3);
return 0;

case 2: / * str mod * /
if (random4(6) == 0) return 1 + random4(3) + random4(3);
return 0;
case 3: / * int mod * /
if (random4(6) == 0) return 1 + random4(3) + random4(3);
return 0;
case 4: / * dex mod * /
if (random4(6) == 0) return 1 + random4(3) + random4(3);
return 0;

Silly, this doesn't work!

}
*/

finished_powers :

if (aclass == 2) power_level -= 4; /* Armours get less powers, and are also less likely to be cursed that wpns */

if (random4(17) >= power_level || power_level < 2) goto finished_curses;

switch(random4(9))
{
case 0: /* makes noise */
if (aclass != 0) break;
proprt [19] = 1;
break;
case 1: /* no magic */
proprt [20] = 1;
break;
case 2: /* random teleport */
if (aclass != 0) break;
proprt [21] = 5 + random4(15);
break;
case 3: /* no teleport - doesn't affect some instantaneous teleports */
if (aclass == 7 && atype == 10) break; /* already is a ring of tport */
if (aclass == 7 && atype == 23) break; /* already is a ring of tport ctrl */
proprt [15] = 0;
proprt [16] = 0;
proprt [22] = 1;
break;
case 4: /* berserk on attack */
if (aclass != 0) break;
proprt [23] = 1 + random4(8);
break;
case 5: /* susceptible to fire */
if (aclass == 7 && (atype == 2 || atype == 21 || atype == 22)) break; /* already does this or something */
proprt [6] = -1;
break;
case 6: /* susceptible to cold */
if (aclass == 7 && (atype == 4 || atype == 21 || atype == 22)) break; /* already does this or something */
proprt [7] = -1;
break;
case 7: /* speed metabolism */
if (aclass == 7 && atype == 9) break; /* already is a ring of hunger */
if (aclass == 7 && atype == 13) break; /* already is a ring of sustenance */
proprt [24] = 1 + random4(3);
break;
case 8: /* emits mutagenic radiation - increases mpower */
proprt [25] = 2 + random4(4); /* property is chance (1 in ...) of increasing mpower */
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
 if (atype < 35) strcat(art_n, "ring"); else strcat(art_n, "amulet");
 srandom(randstore);
 return art_n;
}

if (random4(5) == 0)
{
 if (atype < 35) strcat(art_n, "ring"); else strcat(art_n, "amulet");
 strcat(art_n, rand_armour_names [random4(71)]);
}
 else
{
 make_name(random4(250), random4(250), random4(250), 3, st_p);
 if (atype < 35) strcat(art_n, "ring"); else strcat(art_n, "amulet");
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




void standard_name_weap(unsigned char item_typ, char glorg [80])
{

strcpy(glorg, "");

        switch(item_typ)
        {
        case 0: strcat(glorg , "club"); break;
        case 1: strcat(glorg , "mace"); break;
        case 2: strcat(glorg , "flail"); break;
        case 3: strcat(glorg , "dagger"); break;
        case 4: strcat(glorg , "morningstar"); break;
        case 5: strcat(glorg , "short sword"); break;
        case 6: strcat(glorg , "long sword"); break;
        case 7: strcat(glorg , "great sword"); break;
        case 8: strcat(glorg , "scimitar"); break;
        case 9: strcat(glorg , "hand axe"); break;
        case 10: strcat(glorg , "battleaxe"); break;
        case 11: strcat(glorg , "spear"); break;
        case 12: strcat(glorg , "halberd"); break;
        case 13: strcat(glorg , "sling"); break;
        case 14: strcat(glorg , "bow"); break;
        case 15: strcat(glorg , "crossbow"); break;
        case 16: strcat(glorg , "hand crossbow"); break;
        case 17: strcat(glorg , "glaive"); break;
        case 18: strcat(glorg , "quarterstaff"); break;
        case 19: strcat(glorg , "scythe"); break;
        case 20: strcat(glorg , "giant club"); break;
        case 21: strcat(glorg , "giant spiked club"); break;

        case 22: strcat(glorg , "eveningstar"); break;
        case 23: strcat(glorg , "quick blade"); break;
        case 24: strcat(glorg , "katana"); break;
        case 25: strcat(glorg , "executioner's axe"); break;
        case 26: strcat(glorg , "double sword"); break;
        case 27: strcat(glorg , "triple sword"); break;
        case 28: strcat(glorg , "hammer"); break;
        case 29: strcat(glorg , "ancus"); break;
        case 30: strcat(glorg , "whip"); break;
        case 31: strcat(glorg , "sabre"); break;
        case 32: strcat(glorg , "demon blade"); break;
        case 33: strcat(glorg , "demon whip"); break;
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
        case 0: strcat(glorg , "robe"); break;
        case 1: strcat(glorg , "leather armour"); break;
        case 2: strcat(glorg , "ring mail"); break;
        case 3: strcat(glorg , "scale mail"); break;
        case 4: strcat(glorg , "chain mail"); break;
        case 5: strcat(glorg , "splint mail"); break;
        case 6: strcat(glorg , "banded mail"); break;
        case 7: strcat(glorg , "plate mail"); break;
        case 8: strcat(glorg , "shield"); break;
        case 9: strcat(glorg , "cloak"); break;
        break;
        case 10:
        if (item_plus2 == 0) strcat(glorg , "helmet");
        if (item_plus2 == 1) strcat(glorg , "helm");
        if (item_plus2 == 2) strcat(glorg , "cap");
        if (item_plus2 == 3) strcat(glorg , "wizard's hat");
        break;
        case 11: strcat(glorg , "gloves");
        break;
        case 12: if (item_plus2 == 0) strcat(glorg, "boots");
                 if (item_plus2 == 1) strcat(glorg, "naga barding");
                 if (item_plus2 == 2) strcat(glorg, "centaur barding");
        break;
        case 13: strcat(glorg , "buckler"); break;
        case 14: strcat(glorg , "large shield"); break;
    case 15: strcat(glorg , "dragon hide"); break;
    case 16: strcat(glorg , "troll hide"); break;
    case 17: strcat(glorg , "crystal plate mail"); break;
    case 18: strcat(glorg , "dragon armour"); break;
    case 19: strcat(glorg , "troll leather armour"); break;
    case 20: strcat(glorg , "ice dragon hide"); break;
    case 21: strcat(glorg , "ice dragon armour"); break;
    case 22: strcat(glorg , "steam dragon hide"); break;
    case 23: strcat(glorg , "steam dragon armour"); break;
    case 24: strcat(glorg , "mottled dragon hide"); break;
    case 25: strcat(glorg , "mottled dragon armour"); break;
    case 26: strcat(glorg , "storm dragon hide"); break;
    case 27: strcat(glorg , "storm dragon armour"); break;
    case 28: strcat(glorg , "gold dragon hide"); break;
    case 29: strcat(glorg , "gold dragon armour"); break;
    case 30: strcat(glorg , "animal skin"); break;
    case 31: strcat(glorg , "swamp dragon hide"); break;
    case 32: strcat(glorg , "swamp dragon armour"); break;
 }

}



int random4(unsigned int randmax)
{
 if (randmax <= 0) return 0;
 return random() % randmax;
}

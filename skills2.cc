/*
This file was extensively modified by Wladimir van der Laan.
*/
#include "config.h"

#ifdef DOS
#include <conio.h>
#endif

#include "externs.h"

#include <stdlib.h>

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

{NULL},
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



char *skill_name(char which_skill);

void show_skills(void)
{
char st_pass [60];
int i;
int x;
char strng [5] = "";
char lcount = 0;
        _setcursortype(_NOCURSOR);

#ifdef DOS
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
        cprintf(" points of unallocated experience.\n\r\n\r");
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
                        cprintf(" - %-14s Skill ",skills[x][0]);
                        itoa(you[0].skills[x], strng, 10);
                        cprintf(strng);
#ifdef DEBUG
                        cprintf(" / ");
                        itoa(you[0].skill_points[x], strng, 10);
                        cprintf(strng);
#endif
                        scrln++;
                }
                /* Extra CR between classes of weapons */
                if(x==7 || x==12) scrln++;
        }
// if any more skills added, must adapt letters to go into caps
        gotoxy(1, 24);
        textcolor(LIGHTGREY);
        cprintf("Press the letter of a skill to choose whether you want to practise it.");
        char get_thing;

        get_thing = getch();

        if (get_thing == 0) getch();
        else {
                if (get_thing < 97 | get_thing > 122) goto putty;
                lcount = 0; // toggle skill practise
                for (i = 0; i < 50; i ++) {
                        if (you[0].skills [i] == 0) continue;
                        if (get_thing - 97 == lcount ++)
                                if (you[0].practise_skill [i] == 0) you[0].practise_skill [i] = 1; else you[0].practise_skill [i] = 0;
                }
                goto reprint_stuff;
        }
        putty:
#ifdef DOS
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

char skill_lev2 = 0;
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

if (you[0].skills [0] >= skmk)
{
 skillb = 0;
 skmk = you[0].skills [0];
}

if (you[0].skills [25] >= skmk)
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

switch(species)
{
case 1: return 100; // human
/* Note that this (humans have 100 for all skills) is assumed in the
level_change function in crawl99.cc, if CLASSES is def'd */

case 2: // elf
switch(skill)
{
 case 0: return 120; // fighting
 case 1: return 80; // short blades
 case 2: return 80; // long blades
 case 3: return 110; // great swords
 case 4: return 120; // axes
 case 5: return 130; // maces and flails
 case 6: return 130; // polearms
 case 7: return 100; // staves
 case 8: return 120; // slings
 case 9: return 60; // bows
 case 10: return 100; // xbows
 case 11: return 90; // darts
 case 12: return 80; // throwing
 case 13: return 120; // armour
 case 14: return 80; // dodge
 case 15: return 80; // stealth
 case 16: return 100; // stab
 case 17: return 120; // shields
 case 18: return 100; // traps
 case 19: return 100;
 case 20: return 100;
 case 21: return 100;
 case 22: return 100;
 case 23: return 100;
 case 24: return 100;
 case 25: return 80; // spellcasting
 case 26: return 105; // conj
 case 27: return 70; // ench
 case 28: return 100; // summ
 case 29: return 120; // necro
 case 30: return 100; // transloc
 case 31: return 100; // transmut
 case 32: return 100; // divin
 case 33: return 100; // Fire
 case 34: return 100; // Ice
 case 35: return 70; // Air
 case 36: return 130; // Earth
 case 37: return 110; // poison
}
break;

case 3: // high elf
switch(skill)
{
 case 0: return 100; // fighting
 case 1: return 70; // short blades
 case 2: return 70; // long blades
 case 3: return 115; // great swords
 case 4: return 130; // axes
 case 5: return 150; // maces and flails
 case 6: return 150; // polearms
 case 7: return 100; // staves
 case 8: return 140; // slings
 case 9: return 60; // bows
 case 10: return 100; // xbows
 case 11: return 90; // darts
 case 12: return 80; // throwing
 case 13: return 110; // armour
 case 14: return 90; // dodge
 case 15: return 90; // stealth
 case 16: return 110; // stab
 case 17: return 110; // shields
 case 18: return 100; // traps
 case 19: return 100;
 case 20: return 100;
 case 21: return 100;
 case 22: return 100;
 case 23: return 100;
 case 24: return 100;
 case 25: return 70; // spellcasting
 case 26: return 90; // conj
 case 27: return 70; // ench
 case 28: return 110; // summ
 case 29: return 130; // necro
 case 30: return 90; // transloc
 case 31: return 90; // transmut
 case 32: return 110; // divin
 case 33: return 100; // Fire
 case 34: return 100; // Ice
 case 35: return 70; // Air
 case 36: return 130; // Earth
 case 37: return 130; // poison
}
break;

case 4: // grey elf
switch(skill)
{
 case 0: return 140; // fighting
 case 1: return 90; // short blades
 case 2: return 95; // long blades
 case 3: return 120; // great swords
 case 4: return 140; // axes
 case 5: return 160; // maces and flails
 case 6: return 160; // polearms
 case 7: return 100; // staves
 case 8: return 130; // slings
 case 9: return 70; // bows
 case 10: return 100; // xbows
 case 11: return 90; // darts
 case 12: return 80; // throwing
 case 13: return 140; // armour
 case 14: return 75; // dodge
 case 15: return 70; // stealth
 case 16: return 100; // stab
 case 17: return 140; // shields
 case 18: return 100; // traps
 case 19: return 100;
 case 20: return 100;
 case 21: return 100;
 case 22: return 100;
 case 23: return 100;
 case 24: return 100;
 case 25: return 60; // spellcasting
 case 26: return 90; // conj
 case 27: return 50; // ench
 case 28: return 90; // summ
 case 29: return 130; // necro
 case 30: return 80; // transloc
 case 31: return 80; // transmut
 case 32: return 80; // divin
 case 33: return 90; // Fire
 case 34: return 90; // Ice
 case 35: return 60; // Air
 case 36: return 150; // Earth
 case 37: return 110; // poison
}
break;

case 5: // deep elf
switch(skill)
{
 case 0: return 150; // fighting
 case 1: return 100; // short blades
 case 2: return 105; // long blades
 case 3: return 120; // great swords
 case 4: return 150; // axes
 case 5: return 165; // maces and flails
 case 6: return 165; // polearms
 case 7: return 100; // staves
 case 8: return 135; // slings
 case 9: return 74; // bows
 case 10: return 75; // xbows
 case 11: return 75; // darts
 case 12: return 80; // throwing
 case 13: return 140; // armour
 case 14: return 70; // dodge
 case 15: return 65; // stealth
 case 16: return 80; // stab
 case 17: return 140; // shields
 case 18: return 100; // traps
 case 19: return 100;
 case 20: return 100;
 case 21: return 100;
 case 22: return 100;
 case 23: return 100;
 case 24: return 100;
 case 25: return 55; // spellcasting
 case 26: return 80; // conj
 case 27: return 50; // ench
 case 28: return 80; // summ
 case 29: return 70; // necro
 case 30: return 75; // transloc
 case 31: return 75; // transmut
 case 32: return 75; // divin
 case 33: return 90; // Fire
 case 34: return 90; // Ice
 case 35: return 80; // Air
 case 36: return 100; // Earth
 case 37: return 80; // poison
}
break;

case 6: // sludge elf
switch(skill)
{
 case 0: return 100; // fighting
 case 1: return 80; // short blades
 case 2: return 85; // long blades
 case 3: return 110; // great swords
 case 4: return 130; // axes
 case 5: return 140; // maces and flails
 case 6: return 140; // polearms
 case 7: return 100; // staves
 case 8: return 100; // slings
 case 9: return 70; // bows
 case 10: return 100; // xbows
 case 11: return 80; // darts
 case 12: return 70; // throwing
 case 13: return 140; // armour
 case 14: return 70; // dodge
 case 15: return 75; // stealth
 case 16: return 100; // stab
 case 17: return 130; // shields
 case 18: return 100; // traps
 case 19: return 100;
 case 20: return 100;
 case 21: return 100;
 case 22: return 100;
 case 23: return 100;
 case 24: return 100;
 case 25: return 70; // spellcasting
 case 26: return 90; // conj
 case 27: return 80; // ench
 case 28: return 90; // summ
 case 29: return 100; // necro
 case 30: return 100; // transloc
 case 31: return 100; // transmut
 case 32: return 100; // divin
 case 33: return 90; // Fire
 case 34: return 90; // Ice
 case 35: return 80; // Air
 case 36: return 100; // Earth
 case 37: return 100; // poison
}
break;



case 7: // hill dwarf
switch(skill)
{
 case 0: return 70; // fighting
 case 1: return 80; // short blades
 case 2: return 80; // long blades
 case 3: return 90; // great swords
 case 4: return 60; // axes
 case 5: return 70; // maces and flails
 case 6: return 110; // polearms
 case 7: return 130; // staves
 case 8: return 130; // slings
 case 9: return 150; // bows
 case 10: return 120; // xbows
 case 11: return 120; // darts
 case 12: return 120; // throwing
 case 13: return 70; // armour
 case 14: return 120; // dodge
 case 15: return 150; // stealth
 case 16: return 140; // stab
 case 17: return 70; // shields
 case 18: return 100; // traps
 case 19: return 100;
 case 20: return 100;
 case 21: return 100;
 case 22: return 100;
 case 23: return 100;
 case 24: return 100;
 case 25: return 160; // spellcasting
 case 26: return 120; // conj
 case 27: return 150; // ench
 case 28: return 150; // summ
 case 29: return 160; // necro
 case 30: return 150; // transloc
 case 31: return 120; // transmut
 case 32: return 130; // divin
 case 33: return 80; // Fire
 case 34: return 120; // Ice
 case 35: return 150; // Air
 case 36: return 70; // Earth
 case 37: return 130; // poison
}
break;

case 8: // mountain dwarf
switch(skill)
{
 case 0: return 70; // fighting
 case 1: return 90; // short blades
 case 2: return 90; // long blades
 case 3: return 100; // great swords
 case 4: return 70; // axes
 case 5: return 70; // maces and flails
 case 6: return 110; // polearms
 case 7: return 120; // staves
 case 8: return 125; // slings
 case 9: return 140; // bows
 case 10: return 100; // xbows
 case 11: return 120; // darts
 case 12: return 115; // throwing
 case 13: return 60; // armour
 case 14: return 110; // dodge
 case 15: return 140; // stealth
 case 16: return 130; // stab
 case 17: return 70; // shields
 case 18: return 100; // traps
 case 19: return 100;
 case 20: return 100;
 case 21: return 100;
 case 22: return 100;
 case 23: return 100;
 case 24: return 100;
 case 25: return 140; // spellcasting
 case 26: return 115; // conj
 case 27: return 135; // ench
 case 28: return 150; // summ
 case 29: return 160; // necro
 case 30: return 150; // transloc
 case 31: return 120; // transmut
 case 32: return 130; // divin
 case 33: return 70; // Fire
 case 34: return 130; // Ice
 case 35: return 150; // Air
 case 36: return 70; // Earth
 case 37: return 130; // poison
}
break;

case 9: // halfling
switch(skill)
{
 case 0: return 120; // fighting
 case 1: return 60; // short blades
 case 2: return 100; // long blades
 case 3: return 130; // great swords
 case 4: return 120; // axes
 case 5: return 150; // maces and flails
 case 6: return 160; // polearms
 case 7: return 130; // staves
 case 8: return 50; // slings
 case 9: return 70; // bows
 case 10: return 90; // xbows
 case 11: return 50; // darts
 case 12: return 60; // throwing
 case 13: return 150; // armour
 case 14: return 70; // dodge
 case 15: return 60; // stealth
 case 16: return 70; // stab
 case 17: return 130; // shields
 case 18: return 100; // traps
 case 19: return 100;
 case 20: return 100;
 case 21: return 100;
 case 22: return 100;
 case 23: return 100;
 case 24: return 100;
 case 25: return 130; // spellcasting
 case 26: return 130; // conj
 case 27: return 100; // ench
 case 28: return 120; // summ
 case 29: return 150; // necro
 case 30: return 100; // transloc
 case 31: return 150; // transmut
 case 32: return 140; // divin
 case 33: return 100; // Fire
 case 34: return 100; // Ice
 case 35: return 90; // Air
 case 36: return 100; // Earth
 case 37: return 120; // poison
}
break;


case 10: // orc
switch(skill)
{
 case 0: return 70; // fighting
 case 1: return 100; // short blades
 case 2: return 80; // long blades
 case 3: return 70; // great swords
 case 4: return 70; // axes
 case 5: return 80; // maces and flails
 case 6: return 80; // polearms
 case 7: return 110; // staves
 case 8: return 130; // slings
 case 9: return 120; // bows
 case 10: return 120; // xbows
 case 11: return 130; // darts
 case 12: return 130; // throwing
 case 13: return 90; // armour
 case 14: return 140; // dodge
 case 15: return 150; // stealth
 case 16: return 100; // stab
 case 17: return 80; // shields
 case 18: return 100; // traps
 case 19: return 100;
 case 20: return 100;
 case 21: return 100;
 case 22: return 100;
 case 23: return 100;
 case 24: return 100;
 case 25: return 150; // spellcasting
 case 26: return 100; // conj
 case 27: return 120; // ench
 case 28: return 120; // summ
 case 29: return 100; // necro
 case 30: return 150; // transloc
 case 31: return 160; // transmut
 case 32: return 160; // divin
 case 33: return 100; // Fire
 case 34: return 100; // Ice
 case 35: return 150; // Air
 case 36: return 100; // Earth
 case 37: return 110; // poison
}
break;


case 11: // kobold
switch(skill)
{
 case 0: return 80; // fighting
 case 1: return 60; // short blades
 case 2: return 100; // long blades
 case 3: return 120; // great swords
 case 4: return 110; // axes
 case 5: return 140; // maces and flails
 case 6: return 150; // polearms
 case 7: return 110; // staves
 case 8: return 70; // slings
 case 9: return 80; // bows
 case 10: return 90; // xbows
 case 11: return 50; // darts
 case 12: return 60; // throwing
 case 13: return 140; // armour
 case 14: return 70; // dodge
 case 15: return 60; // stealth
 case 16: return 70; // stab
 case 17: return 130; // shields
 case 18: return 100; // traps
 case 19: return 100;
 case 20: return 100;
 case 21: return 100;
 case 22: return 100;
 case 23: return 100;
 case 24: return 100;
 case 25: return 110; // spellcasting
 case 26: return 110; // conj
 case 27: return 110; // ench
 case 28: return 105; // summ
 case 29: return 105; // necro
 case 30: return 100; // transloc
 case 31: return 110; // transmut
 case 32: return 130; // divin
 case 33: return 100; // Fire
 case 34: return 100; // Ice
 case 35: return 100; // Air
 case 36: return 100; // Earth
 case 37: return 100; // poison
}
break;

case 12: // mummy
switch(skill)
{
 case 0: return 100; // fighting
/* case 18: return 100;
 case 19: return 100;
 case 20: return 100;
 case 21: return 100;
 case 22: return 100;
 case 23: return 100;
 case 24: return 100;*/
 case 25: return 100; // spellcasting
 case 29: return 100; // necro
 default: return 140;
}
break;

case 13: // naga
switch(skill)
{
 case 0: return 110; // fighting
 case 1: return 100; // short blades
 case 2: return 100; // long blades
 case 3: return 100; // great swords
 case 4: return 100; // axes
 case 5: return 100; // maces and flails
 case 6: return 100; // polearms
 case 7: return 120; // staves
 case 8: return 120; // slings
 case 9: return 120; // bows
 case 10: return 120; // xbows
 case 11: return 120; // darts
 case 12: return 120; // throwing
 case 13: return 150; // armour
 case 14: return 150; // dodge
 case 15: return 40; // stealth
 case 16: return 100; // stab
 case 17: return 140; // shields
/* case 18: return 100; // traps
 case 19: return 100;
 case 20: return 100;
 case 21: return 100;
 case 22: return 100;
 case 23: return 100;
 case 24: return 100;*/
 case 25: return 100; // spellcasting
 case 26: return 100; // conj
 case 27: return 100; // ench
 case 28: return 100; // summ
 case 29: return 100; // necro
 case 30: return 100; // transloc
 case 31: return 100; // transmut
 case 32: return 100; // divin
 case 33: return 100; // Fire
 case 34: return 100; // Ice
 case 35: return 100; // Air
 case 36: return 100; // Earth
 case 37: return 60; // poison
}
break;

case 14: // Gnome
switch(skill)
{
 case 0: return 100; // fighting
 case 1: return 75; // short blades
 case 2: return 100; // long blades
 case 3: return 130; // great swords
 case 4: return 100; // axes
 case 5: return 130; // maces and flails
 case 6: return 140; // polearms
 case 7: return 130; // staves
 case 8: return 80; // slings
 case 9: return 100; // bows
 case 10: return 90; // xbows
 case 11: return 60; // darts
 case 12: return 100; // throwing
 case 13: return 150; // armour
 case 14: return 70; // dodge
 case 15: return 70; // stealth
 case 16: return 80; // stab
 case 17: return 120; // shields
 case 18: return 100; // traps
 case 19: return 100;
 case 20: return 100;
 case 21: return 100;
 case 22: return 100;
 case 23: return 100;
 case 24: return 100;
 case 25: return 120; // spellcasting
 case 26: return 100; // conj
 case 27: return 100; // ench
 case 28: return 110; // summ
 case 29: return 130; // necro
 case 30: return 130; // transloc
 case 31: return 120; // transmut
 case 32: return 120; // divin
 case 33: return 100; // Fire
 case 34: return 100; // Ice
 case 35: return 170; // Air
 case 36: return 60; // Earth
 case 37: return 130; // poison
}
break;

case 15: // ogre
switch(skill)
{
 case 0: return 100; // fighting
 case 1: return 140; // short blades
 case 2: return 120; // long blades
 case 3: return 110; // great swords
 case 4: return 100; // axes
 case 5: return 100; // maces and flails
 case 6: return 110; // polearms
 case 7: return 120; // staves
 case 8: return 150; // slings
 case 9: return 150; // bows
 case 10: return 180; // xbows
 case 11: return 150; // darts
 case 12: return 100; // throwing
 case 13: return 140; // armour
 case 14: return 150; // dodge
 case 15: return 200; // stealth
 case 16: return 150; // stab
 case 17: return 110; // shields
 case 18: return 200; // traps
 case 19: return 100;
 case 20: return 100;
 case 21: return 100;
 case 22: return 100;
 case 23: return 100;
 case 24: return 100;
 case 25: return 220; // spellcasting
 case 26: return 180; // conj
 case 27: return 220; // ench
 case 28: return 200; // summ
 case 29: return 150; // necro
 case 30: return 200; // transloc
 case 31: return 200; // transmut
 case 32: return 200; // divin
 case 33: return 150; // Fire
 case 34: return 150; // Ice
 case 35: return 200; // Air
 case 36: return 120; // Earth
 case 37: return 150; // poison
}
break;

case 16: // troll
switch(skill)
{
 case 0: return 150; // fighting
 case 1: return 150; // short blades
 case 2: return 150; // long blades
 case 3: return 150; // great swords
 case 4: return 150; // axes
 case 5: return 150; // maces and flails
 case 6: return 150; // polearms
 case 7: return 150; // staves
 case 8: return 150; // slings
 case 9: return 150; // bows
 case 10: return 180; // xbows
 case 11: return 150; // darts
 case 12: return 150; // throwing
 case 13: return 150; // armour
 case 14: return 130; // dodge
 case 15: return 250; // stealth
 case 16: return 130; // stab
 case 17: return 140; // shields
 case 18: return 200; // traps
 case 19: return 100;
 case 20: return 100;
 case 21: return 100;
 case 22: return 100;
 case 23: return 100;
 case 24: return 100;
 case 25: return 200; // spellcasting
 case 26: return 150; // conj
 case 27: return 200; // ench
 case 28: return 150; // summ
 case 29: return 140; // necro
 case 30: return 150; // transloc
 case 31: return 150; // transmut
 case 32: return 180; // divin
 case 33: return 150; // Fire
 case 34: return 150; // Ice
 case 35: return 200; // Air
 case 36: return 110; // Earth
 case 37: return 130; // poison
}
break;


case 17: // ogre mage
switch(skill)
{
 case 0: return 100; // fighting
 case 1: return 110; // short blades
 case 2: return 100; // long blades
 case 3: return 100; // great swords
 case 4: return 100; // axes
 case 5: return 100; // maces and flails
 case 6: return 100; // polearms
 case 7: return 100; // staves
 case 8: return 150; // slings
 case 9: return 150; // bows
 case 10: return 150; // xbows
 case 11: return 150; // darts
 case 12: return 150; // throwing
 case 13: return 170; // armour
 case 14: return 130; // dodge
 case 15: return 100; // stealth
 case 16: return 130; // stab
 case 17: return 150; // shields
 case 18: return 150; // traps
 case 19: return 100;
 case 20: return 100;
 case 21: return 100;
 case 22: return 100;
 case 23: return 100;
 case 24: return 100;
 case 25: return 80; // spellcasting
 case 26: return 100; // conj
 case 27: return 80; // ench
 case 28: return 100; // summ
 case 29: return 100; // necro
 case 30: return 100; // transloc
 case 31: return 100; // transmut
 case 32: return 100; // divin
 case 33: return 100; // Fire
 case 34: return 100; // Ice
 case 35: return 100; // Air
 case 36: return 100; // Earth
 case 37: return 100; // poison
}
break;

default: // Draconian
switch(skill)
{
 case 0: return 90; // fighting
 case 1: return 100; // short blades
 case 2: return 100; // long blades
 case 3: return 100; // great swords
 case 4: return 100; // axes
 case 5: return 100; // maces and flails
 case 6: return 100; // polearms
 case 7: return 100; // staves
 case 8: return 120; // slings
 case 9: return 120; // bows
 case 10: return 120; // xbows
 case 11: return 120; // darts
 case 12: return 120; // throwing
 case 13: return 200; // armour
 case 14: return 120; // dodge
 case 15: return 120; // stealth
 case 16: return 100; // stab
 case 17: return 100; // shields
/* case 18: return 100; // traps
 case 19: return 100;
 case 20: return 100;
 case 21: return 100;
 case 22: return 100;
 case 23: return 100;
 case 24: return 100;*/
 case 25:
 if (species == 24) return 75;
 return 100; // spellcasting
 case 26: return 100; // conj
 case 27: return 120; // ench
 case 28: return 100; // summ
 case 29: return 100; // necro
 case 30:
 if (species == 24) return 75;
 return 100; // transloc
 case 31: return 100; // transmut
 case 32: return 100; // divin
 case 33:
 if (species == 18) return 70;
 if (species == 19) return 150;
 return 100; // Fire
 case 34:
 if (species == 18) return 150;
 if (species == 19) return 70;
 return 100; // Ice
 case 35:
 if (species == 23) return 70;
 return 100; // Air
 case 36: return 100; // Earth
 case 37:
 if (species == 20) return 70;
 return 100; // poison
}
break;

/*
case 2: // elf
switch(skill)
{
 case 0: return 100; // fighting
 case 1: return 100; // short blades
 case 2: return 100; // long blades
 case 3: return 100; // great swords
 case 4: return 100; // axes
 case 5: return 100; // maces and flails
 case 6: return 100; // polearms
 case 7: return 100; // staves
 case 8: return 100; // slings
 case 9: return 100; // bows
 case 10: return 100; // xbows
 case 11: return 100; // darts
 case 12: return 100; // throwing
 case 13: return 100; // armour
 case 14: return 100; // dodge
 case 15: return 100; // stealth
 case 16: return 100; // stab
 case 17: return 100; // shields
/ * case 18: return 100; // traps
 case 19: return 100;
 case 20: return 100;
 case 21: return 100;
 case 22: return 100;
 case 23: return 100;
 case 24: return 100;* /
 case 25: return 100; // spellcasting
 case 26: return 100; // conj
 case 27: return 100; // ench
 case 28: return 100; // summ
 case 29: return 100; // necro
 case 30: return 100; // transloc
 case 31: return 100; // transmut
 case 32: return 100; // divin
 case 33: return 100;
 case 34: return 100;
}
break;
*/
}

return 100;

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
  if ((skc >= 1 && skc <= 11) | (skc >= 13 && skc <= 15) | (skc >= 26 && skc <= 36))
    continue;
 }
 if (you[0].skills [skc] > 0 && you[0].skills [skc] < 27) numb_sk ++;
}

if (numb_sk <= 0) return sk_tot;

for (skc = min_skill; skc <= max_skill; skc ++)
{
 if (you[0].practise_skill [skc] == 0)
 {
  if ((skc >= 1 && skc <= 11) | (skc >= 13 && skc <= 15) | (skc >= 26 && skc <= 36))
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

#include "config.h"

#ifdef DOS
#include <conio.h>
#endif

#ifdef DOS
#include <file.h>
#endif
#ifdef LINUX
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#endif

#ifdef USE_EMX
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#endif

#ifdef MAC
#include <stat.h>
#else
#include <sys/stat.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "externs.h"
#include "enum.h"
#include "itemname.h"
#include "macro.h"
#include "player.h"
#include "randart.h"
#include "skills2.h"
#include "stuff.h"
#include "version.h"

char class_allowed(char speci, char clas);
void init_player(void);
void choose_weapon(void);

extern char wield_change;


char new_game(void)
{
int handle;
char your_nam [kNameLen];
int p;
char keyn;
char weap_skill = 0;

char char_fil [kFileNameSize];

int i;

init_player();

for (i = 0; i < 50; i ++)
{
 you[0].skills [i] = 0;
 you[0].skill_points [i] = 0;
 you[0].practise_skill [i] = 1;
}
you[0].exp_available = 25;




/*you[0].ring [0] = -1;
you[0].ring [1] = -1;
you[0].ring [2] = -1;
you[0].piety = 50; // noncomittal*/

you[0].max_level = 1;
for (i = 0; i < 50; i ++)
{
 you[0].unique_items [i] = 0;
}
you[0].hunger = 6000;
you[0].hunger_inc = 3;
you[0].equip [EQ_WEAPON] = -1;
/*you[0].rate_regen = 6;*/
you[0].xl = 1;
you[0].speed = 10;
you[0].pet_target = 201;
you[0].your_level = 1;
you[0].ep_incr_regen = 0;


char j = 0;
char char_name [kNameLen];

for (i = 0; i < 30; i ++)
{
/*      lev_ex [i] = 0;*/
 if (i <= 25) you[0].spells [i] = 210;
}

you[0].your_level = 0;

// Have to add resistances etc to this!!!

for (i = 0; i < 52; i ++)
{
        you[0].inv_quant [i] = 0;
        you[0].inv_ident [i] = 3;
}

textcolor(7);

cprintf(EOL"Hello, and welcome to Dungeon Crawl v"VERSION"!");
cprintf(EOL"(Copyright 1997, 1998, 1999 Linley Henzell)");
cprintf(EOL"Please read Crawl.txt for instructions and legal details."EOL EOL);
name_q : cprintf("What is your name today? ");
//cin >> your_nam;
#ifndef LINUX
gets(your_nam);
#endif

#ifdef LINUX
echo();
getstr(your_nam);
noecho();
#endif

#ifdef MAC
getstr(your_nam, sizeof(your_nam));
#endif

/*#ifdef DOS_TERM
gets(your_nam);
#endif

#ifdef PLAIN_TERM
echo();
getstr(your_nam);
noecho();
#endif*/


if (strcmp(your_nam, "bones") == 0 || strlen(your_nam) == 0) /* this would cause big probs with ghosts */
{
        cprintf(EOL"That's a silly name!"EOL);
        goto name_q;
}

unsigned int glorpo = 0;

for (glorpo = 0; glorpo < strlen(your_nam); glorpo ++)
{
#ifdef MAC
 if (your_nam [glorpo] == ':')                  // colon is Mac path seperator
 {      // $$$ shouldn't DOS and Unix path seperators be illegal?
  cprintf(EOL"No colons, please."EOL);
  goto name_q;
 }
#else
 if (your_nam [glorpo] == ' ')  // spaces are OK in Mac file names
 {
  cprintf(EOL"No spaces, please."EOL);
  goto name_q;
 }
#endif
 if (your_nam [glorpo] == '?')
 {
  cprintf(EOL"No question marks, please."EOL);
  goto name_q;
 }
}


strncpy(you[0].your_name, your_nam, kNameLen);

strncpy(char_name, your_nam, kFileNameLen);

strcpy(char_fil, "");
strncat(char_fil, you[0].your_name, kFileNameLen);
strcat(char_fil, ".sav");



 int passout = 0;


handle = open(char_fil, S_IWRITE, S_IREAD);


if (handle != -1)
{
        cprintf(EOL"Welcome back, ");
        cprintf(your_nam);
        cprintf("!");
        close(handle);

        return 0;
}
close(handle);



cprintf(EOL EOL"");
cprintf("You must be new here!"EOL);
spec_query2 : cprintf("You can be:"EOL);
cprintf("a - Human                     s - Centaur"EOL);
cprintf("b - Elf                       t - Demigod"EOL);
cprintf("c - High Elf                  u - Spriggan"EOL);
cprintf("d - Grey Elf                  v - Minotaur"EOL);
cprintf("e - Deep Elf                  w - Demonspawn"EOL);
cprintf("f - Sludge Elf                x - Ghoul"EOL);
cprintf("g - Hill Dwarf                y - Kenku"EOL);
cprintf("h - Mountain Dwarf"EOL);
cprintf("i - Halfling"EOL);
cprintf("j - Hill Orc"EOL);
cprintf("k - Kobold"EOL);
cprintf("l - Mummy"EOL);
cprintf("m - Naga"EOL);
cprintf("n - Gnome"EOL);
cprintf("o - Ogre"EOL);
cprintf("p - Troll"EOL);
cprintf("q - Ogre-Mage"EOL);
cprintf("r - Draconian"EOL);
cprintf("? - Random"EOL);
cprintf("X - Quit"EOL);

cprintf(EOL"Which one? ");
spec_query : keyn = getch();
if (keyn == 0)
{
 getch();
 goto spec_query;
}
switch_start : switch(keyn)
{
 case 'a': you[0].species = SP_HUMAN; // human
 you[0].strength = 6;
 you[0].intel = 6;
 you[0].dex = 6;
 break;
 case 'b': you[0].species = SP_ELF; // elf
 you[0].strength = 5;
 you[0].intel = 8;
 you[0].dex = 8;
 break;
 case 'c': you[0].species = SP_HIGH_ELF; // high-elf
 you[0].strength = 5;
 you[0].intel = 9;
 you[0].dex = 8;
 break;
 case 'd': you[0].species = SP_GREY_ELF; // grey-elf
 you[0].strength = 4;
 you[0].intel = 9;
 you[0].dex = 8;
 break;
 case 'e': you[0].species = SP_DEEP_ELF; // deep elf
 you[0].strength = 4;
 you[0].intel = 10;
 you[0].dex = 7;
 break;
 case 'f': you[0].species = SP_SLUDGE_ELF; // sludge elf
 you[0].strength = 5;
 you[0].intel = 7;
 you[0].dex = 8;
 break;
 case 'g': you[0].species = SP_HILL_DWARF; // hill dwarf
 you[0].strength = 9;
 you[0].intel = 3;
 you[0].dex = 4;
 break;
 case 'h': you[0].species = SP_MOUNTAIN_DWARF; // mountain dwarf
 you[0].strength = 9;
 you[0].intel = 4;
 you[0].dex = 5;
 break;
 case 'i': you[0].species = SP_HALFLING; // halfling
 you[0].strength = 4;
 you[0].intel = 6;
 you[0].dex = 9;
 break;
 case 'j':
 case 'J': you[0].species = SP_HILL_ORC; // hill orc
 you[0].strength = 9;
 you[0].intel = 3;
 you[0].dex = 4;
 break;
 case 'K':
 case 'k': you[0].species = SP_KOBOLD; // kobold
 you[0].strength = 4;
 you[0].intel = 4;
 you[0].dex = 6;
 break;
 case 'l': you[0].species = SP_MUMMY; // Mummy
 you[0].strength = 7;
 you[0].intel = 3;
 you[0].dex = 3;
 break;
 case 'm':
 case 'M': you[0].species = SP_NAGA; // Naga
 you[0].strength = 8;
 you[0].intel = 6;
 you[0].dex = 4;
 break;
 case 'n':
 you[0].species = SP_GNOME; // gnome
 you[0].strength = 5;
 you[0].intel = 6;
 you[0].dex = 7;
 break;
 case 'o':
 case 'O':
 you[0].species = SP_OGRE; // ogre
 you[0].strength = 12;
 you[0].intel = 3;
 you[0].dex = 1;
 break;
 case 'p':
 case 'P':
 you[0].species = SP_TROLL; // troll
 you[0].strength = 13;
 you[0].intel = 3;
 you[0].dex = 0;
 break;
 case 'Q':
 case 'q':
 you[0].species = SP_OGRE_MAGE; // ogre mage
 you[0].strength = 8;
 you[0].intel = 6;
 you[0].dex = 2;
 break;
 case 'r':
 case 'R': you[0].species = SP_RED_DRACONIAN + random2(9); // Draconian
 you[0].strength = 7;
 you[0].intel = 6;
 you[0].dex = 2;
 break;
 case 's':
 you[0].species = SP_CENTAUR; // centaur
 you[0].strength = 8;
 you[0].intel = 4;
 you[0].dex = 5;
 break;
 case 't':
 you[0].species = SP_DEMIGOD; // demigod - more is added to stats later
 you[0].strength = 7;
 you[0].intel = 7;
 you[0].dex = 7;
 break;
 case 'u':
 you[0].species = SP_SPRIGGAN; // spriggan
 you[0].strength = 3;
 you[0].intel = 6;
 you[0].dex = 8;
 break;
 case 'v':
 you[0].species = SP_MINOTAUR; // minotaur
 you[0].strength = 10;
 you[0].intel = 3;
 you[0].dex = 5;
 break;
 case 'w': you[0].species = SP_DEMONSPAWN; // demonspawn - more is added, like demigods
 you[0].strength = 4;
 you[0].intel = 4;
 you[0].dex = 4;
 break;
 case 'x':
 you[0].species = SP_GHOUL; // Ghoul
 you[0].strength = 7;
 you[0].intel = 1;
 you[0].dex = 2;
 break;
 case 'y':
 you[0].species = SP_KENKU; // Kenku
 you[0].strength = 5;
 you[0].intel = 6;
 you[0].dex = 7;
 break;
 case '?':
 keyn = 97 + random2(25);
 goto switch_start;
 case 'X':
 cprintf(EOL"Goodbye!");
 end(0);
 break;
 default:
// cprintf("\n\rDon't be silly. ");
 goto spec_query;
}

clrscr(); // Otherwise it looks ugly under Win NT, or so I'm told

cprintf(EOL EOL);
cprintf("Welcome, ");
cprintf(you[0].your_name);
cprintf(" the ");
cprintf(species_name(you[0].species));
cprintf("."EOL EOL);

cprintf("You can be any of the following :"EOL);
/*
for (i = 0; i < 19; i ++)
{
 if (class_allowed(you[0].species, i) == 0) continue;

 if (i > 0) cprintf(", ");
 if (wherex() > 57) cprintf("\n\r");

 switch(i)
 {
  case 0: cprintf("fighter"); break;
  case 1: cprintf("Wizard"); break;
  case 2: cprintf("priest"); break;
  case 3: cprintf("Thief"); break;
  case 4: cprintf("Gladiator"); break;
  case 5: cprintf("Necromancer"); break;
  case 6: cprintf("Paladin"); break;
  case 7: cprintf("assassin"); break;
  case 8: cprintf("Berserker"); break;
  case 9: cprintf("Ranger"); break;
  case 10: cprintf("conjurer"); break;
  case 11: cprintf("enchanter"); break;
  case 12: cprintf("Fire Elementalist"); break;
  case 13: cprintf("Ice Elementalist"); break;
  case 14: cprintf("Summoner"); break;
  case 15: cprintf("Air Elementalist"); break;
  case 16: cprintf("Earth Elementalist"); break;
  case 17: cprintf("Crusader"); break;
  case 18: cprintf("Death knight"); break;
 }

}
cprintf(", or Quit.");
*/
j = 0;

for (i = 0; i < 30; i ++)
{
 if (i == 23) i = 24;
// if (i == 25) i = 26;
 if (class_allowed(you[0].species, i) == 0) continue;

 if (i < 26 ) putch(i + 97); else putch(i + 39);
 cprintf(" - ");

 switch(i)
 {
  case JOB_FIGHTER: cprintf("fighter"); break;
  case JOB_WIZARD: cprintf("Wizard"); break;
  case JOB_PRIEST: cprintf("priest"); break;
  case JOB_THIEF: cprintf("Thief"); break;
  case JOB_GLADIATOR: cprintf("Gladiator"); break;
  case JOB_NECROMANCER: cprintf("Necromancer"); break;
  case JOB_PALADIN: cprintf("Paladin"); break;
  case JOB_ASSASSIN: cprintf("assassin"); break;
  case JOB_BERSERKER: cprintf("Berserker"); break;
  case JOB_RANGER: cprintf("Ranger"); break;
  case JOB_CONJURER: cprintf("conjurer"); break;
  case JOB_ENCHANTER: cprintf("enchanter"); break;
  case JOB_FIRE_ELEMENTALIST: cprintf("Fire Elementalist"); break;
  case JOB_ICE_ELEMENTALIST: cprintf("Ice Elementalist"); break;
  case JOB_SUMMONER: cprintf("Summoner"); break;
  case JOB_AIR_ELEMENTALIST: cprintf("Air Elementalist"); break;
  case JOB_EARTH_ELEMENTALIST: cprintf("Earth Elementalist"); break;
  case JOB_CRUSADER: cprintf("Crusader"); break;
  case JOB_DEATH_KNIGHT: cprintf("Death knight"); break;
  case JOB_VENOM_MAGE: cprintf("Venom Mage"); break;
  case JOB_CHAOS_KNIGHT: cprintf("Chaos Knight"); break;
  case JOB_TRANSMUTER: cprintf("transmuter"); break;
  case JOB_HEALER: cprintf("Healer"); break;
  case JOB_REAVER: cprintf("Reaver"); break;
  case JOB_STALKER: cprintf("stalker"); break;
  case JOB_MONK: cprintf("Monk"); break;
  case JOB_WARPER: cprintf("warper"); break;
 /* when adding more, also add to the range of the loop */
 }
 if (j % 2 == 1) cprintf(EOL); else gotoxy(40, wherey());
 j ++;

}
if (wherex() >= 40) cprintf(EOL);
cprintf("? - Random; x - Back to species selection; X - Quit"EOL);
//cprintf("x - Back to species selection\n\r");
//cprintf("X - Quit\n\r");


cprintf(EOL"What kind of character are you? ");
query : keyn = getch();


query5: if (keyn == 'a') you[0].clas = JOB_FIGHTER;
        else if (keyn == 'b') you[0].clas = JOB_WIZARD;
  else if (keyn == 'c') you[0].clas = JOB_PRIEST;
  else if (keyn == 'd') you[0].clas = JOB_THIEF;
  else if (keyn == 'e') you[0].clas = JOB_GLADIATOR;
  else if (keyn == 'f') you[0].clas = JOB_NECROMANCER;
  else if (keyn == 'g') you[0].clas = JOB_PALADIN;
  else if (keyn == 'h') you[0].clas = JOB_ASSASSIN;
  else if (keyn == 'i') you[0].clas = JOB_BERSERKER;
  else if (keyn == 'j') you[0].clas = JOB_RANGER;
  else if (keyn == 'k') you[0].clas = JOB_CONJURER;
  else if (keyn == 'l') you[0].clas = JOB_ENCHANTER;
  else if (keyn == 'm') you[0].clas = JOB_FIRE_ELEMENTALIST;
  else if (keyn == 'n') you[0].clas = JOB_ICE_ELEMENTALIST;
  else if (keyn == 'o') you[0].clas = JOB_SUMMONER;
  else if (keyn == 'p') you[0].clas = JOB_AIR_ELEMENTALIST;
  else if (keyn == 'q') you[0].clas = JOB_EARTH_ELEMENTALIST;
  else if (keyn == 'r') you[0].clas = JOB_CRUSADER;
  else if (keyn == 's') you[0].clas = JOB_DEATH_KNIGHT;
  else if (keyn == 't') you[0].clas = JOB_VENOM_MAGE;
  else if (keyn == 'u') you[0].clas = JOB_CHAOS_KNIGHT;
  else if (keyn == 'v') you[0].clas = JOB_TRANSMUTER;
  else if (keyn == 'w') you[0].clas = JOB_HEALER;
  else if (keyn == 'y') you[0].clas = JOB_REAVER;
  else if (keyn == 'z') you[0].clas = JOB_STALKER;
  else if (keyn == 'A') you[0].clas = JOB_MONK;
  else if (keyn == 'B') you[0].clas = JOB_WARPER;
  else if (keyn == '?')
  {
   do
   {
    do
    {
     keyn = 97 + random2(28);
    } while (keyn == 'x');
    if (keyn == '{') keyn = 'A';
    if (keyn == '|') keyn = 'B';
   } while (class_allowed(you[0].species, keyn - 97) == 0);
   goto query5;
  }
  else if (keyn == 'x')
  {
   clrscr();
   cprintf(EOL EOL);
   goto spec_query2;
  }
  else if (keyn == 'X')
   {
    cprintf(EOL"Goodbye!");
    end(0);
   }
                else
                {
                        cant_be_that : //cprintf("\n\rI'm sorry, you can't be that. ");
                        goto query;
                }

                if (class_allowed(you[0].species, you[0].clas) == 0) goto cant_be_that;

        for (i = 0; i < 52; i ++)
        {
                you [0].inv_quant [i] = 0;
                you [0].inv_class [i] = OBJ_WEAPONS;
                you [0].inv_type [i] = WPN_CLUB;
                you [0].inv_plus [i] = 0;
                you [0].inv_plus2 [i] = 0;
                you [0].inv_dam [i] = 0;
                you [0].inv_col [i] = 0;
        }

for (i = 0; i < 30; i ++)
{
 you[0].attribute [i] = 0;
}


switch(you[0].clas)
{
case JOB_FIGHTER: // fighter
        strcpy(you[0].clasnam, "fighter");
        you[0].hp = 14; you[0].hp_max = 14;
        you[0].ep = 0; you[0].ep_max = 0;
/*      you[0].f_abil = 10;
        you[0].mag_abil = 5;
 you[0].thr_abil = 10;
 you[0].speed = 10;*/
 you[0].spell_levels = 0;

        you[0].inv_quant [0] = 1;
        you[0].inv_class [0] = OBJ_WEAPONS;
                you[0].inv_type [0] = WPN_SHORT_SWORD;

/*  if (you[0].species == SP_KOBOLD)
  {
   you[0].inv_type [0] = 3; // kobolds just get daggers
   you[0].skills [SK_SHORT_BLADES] = 2;
  } else  */
  if (you[0].species == SP_OGRE)
  {
   you[0].inv_type [0] = WPN_CLUB; // ogre
   you[0].skills [SK_MACES_FLAILS] = 1;
  } else
  if (you[0].species == SP_TROLL)
  {
//   you[0].inv_type [0] = 0; // troll
//   you[0].skills [SK_FIGHTING] ++;
  }
/*    else
  if (random2(4) == 0 | you[0].species == SP_HILL_DWARF | you[0].species == SP_MOUNTAIN_DWARF) // dwarves
  {
   you[0].inv_type [0] = WPN_HAND_AXE;
   you[0].skills [SK_AXES] = 2;
  } else
  if (random2(4) == 0)
  {
   you[0].inv_type [0] = WPN_MACE;
   you[0].skills [SK_MACES_FLAILS] = 2;
  }
   else you[0].skills [SK_SHORT_BLADES] = 2;*/
        weap_skill = 2;


        you[0].inv_plus [0] = 50;
        you[0].inv_plus2 [0] = 50;
        you[0].inv_dam [0] = 0;
        you[0].inv_col [0] = LIGHTCYAN;


if (you[0].species == SP_OGRE)
{
        you[0].inv_quant [0] = 1;
        you[0].inv_class [0] = OBJ_WEAPONS;
        you[0].inv_type [0] = WPN_CLUB;
        you[0].inv_plus [0] = 50;
        you[0].inv_dam [0] = 0;
        you[0].inv_col [0] = BROWN;

}

if (you[0].species == SP_TROLL)
{
        you[0].inv_quant [0] = 0;
        you[0].inv_class [0] = OBJ_WEAPONS;
        you[0].inv_type [0] = WPN_CLUB;
        you[0].inv_plus [0] = 50;
        you[0].inv_dam [0] = 0;
        you[0].inv_col [0] = BROWN;

}

if (you[0].species == SP_OGRE || you[0].species == SP_TROLL || (you[0].species >= SP_RED_DRACONIAN && you[0].species <= SP_UNK2_DRACONIAN))
{
        you[0].inv_quant [1] = 1;
        you[0].inv_class [1] = OBJ_ARMOUR;
        you[0].inv_type [1] = ARM_ANIMAL_SKIN;
        you[0].inv_plus [1] = 50;
        you[0].inv_dam [1] = 0;
        you[0].inv_col [1] = BROWN;

 if (you[0].species >= SP_RED_DRACONIAN && you[0].species <= SP_UNK2_DRACONIAN)
 {
        you[0].inv_quant [2] = 1;
        you[0].inv_class [2] = OBJ_ARMOUR;
        you[0].inv_type [2] = ARM_SHIELD;
        you[0].inv_plus [2] = 50;
        you[0].inv_dam [2] = 0;
        you[0].inv_col [2] = LIGHTCYAN;
 }


}
else
if (you[0].species == SP_GHOUL || you[0].species == SP_MUMMY)
{
    you[0].inv_quant [1] = 1;
        you[0].inv_class [1] = OBJ_ARMOUR;
        you[0].inv_type [1] = ARM_ROBE;
        you[0].inv_plus [1] = 50;
        you[0].inv_dam [1] = 0;
        you[0].inv_col [1] = WHITE; /* grave shroud */

 if (you[0].species == SP_MUMMY)
 {
        you[0].inv_quant [2] = 1;
        you[0].inv_class [2] = OBJ_ARMOUR;
        you[0].inv_type [2] = 8;
        you[0].inv_plus [2] = 50;
        you[0].inv_dam [2] = 0;
        you[0].inv_col [2] = LIGHTCYAN;
 }

}
else
if (you[0].species == SP_KOBOLD)
{
        you[0].inv_quant [1] = 1;
        you[0].inv_class [1] = OBJ_ARMOUR;
        you[0].inv_type [1] = ARM_LEATHER_ARMOUR;
        you[0].inv_plus [1] = 50;
        you[0].inv_dam [1] = 0;
        you[0].inv_col [1] = BROWN;
/*      you[0].AC = 2;
        you[0].evasion = 9;*/

  you[0].inv_quant [2] = random2(8) + random2(8) + 8;
                you[0].inv_class [2] = OBJ_MISSILES;
                you[0].inv_type [2] = MI_DART; //wtype;
                you[0].inv_plus [2] = 50;
                you[0].inv_dam [2] = 0;
                you[0].inv_col [2] = LIGHTCYAN;

} else
{
        you[0].inv_quant [1] = 1;
        you[0].inv_class [1] = OBJ_ARMOUR;
        you[0].inv_type [1] = ARM_RING_MAIL;
        you[0].inv_plus [1] = 50;
        you[0].inv_dam [1] = 0;
        you[0].inv_col [1] = LIGHTCYAN;

        you[0].inv_quant [2] = 1;
        you[0].inv_class [2] = OBJ_ARMOUR;
        you[0].inv_type [2] = ARM_SHIELD;
        you[0].inv_plus [2] = 50;
        you[0].inv_dam [2] = 0;
        you[0].inv_col [2] = LIGHTCYAN;

/*      you[0].AC = 3;
        you[0].evasion = 9;*/
        choose_weapon();
    cprintf(EOL"A fine choice.");
}

        // 11 = spear
        // 9 = hand ae
        // 3 = dagger

        you[0].strength += 6;
        you[0].dex += 4;

        if (you[0].species != SP_TROLL) you[0].equip [EQ_WEAPON] = 0;
        you[0].equip [EQ_BODY_ARMOUR] = 1;
if (you[0].species != SP_KOBOLD && you[0].species != SP_OGRE && you[0].species != SP_TROLL && you[0].species != SP_GHOUL) you[0].equip [EQ_SHIELD] = 2;
        you[0].gp = random2(10);
/* you[0].res_magic = 3;*/


 you[0].skills [SK_FIGHTING] = 3;
if (you[0].species == SP_KOBOLD)
{
 you[0].skills [SK_THROWING] = 2;
 you[0].skills [SK_DODGING] = 1;
 you[0].skills [SK_STEALTH] = 1;
 you[0].skills [SK_STABBING] = 1;
 you[0].skills [SK_DODGING + random() % 3] ++;
} else
if (you[0].species != SP_OGRE && you[0].species != SP_TROLL)
{
 if (you[0].species >= SP_RED_DRACONIAN && you[0].species <= SP_UNK2_DRACONIAN) you[0].skills [SK_DODGING] = 2;
  else you[0].skills [SK_ARMOUR] = 2;
 you[0].skills [SK_SHIELDS] = 1;
 you[0].skills [SK_STABBING + random() % 2] ++;
 you[0].skills [SK_THROWING] = 1;
 you[0].skills [SK_UNARMED_COMBAT] = 1;
} else you[0].skills [SK_FIGHTING] += 2;
break;

case JOB_WIZARD: // wizard
        strcpy(you[0].clasnam, "Wizard");
        you[0].hp = 10; you[0].hp_max = 10;
        you[0].ep = 3; you[0].ep_max = 3;
/*      you[0].f_abil = 5;
        you[0].mag_abil = 10;
 you[0].thr_abil = 5;
 you[0].speed = 10;*/
 you[0].spell_levels = 12;

        you[0].inv_quant [0] = 1;
        you[0].inv_class [0] = OBJ_WEAPONS;
                you[0].inv_type [0] = WPN_DAGGER; //damage = 6; //break;

        you[0].inv_plus [0] = 50;
        you[0].inv_plus2 [0] = 50;
        you[0].inv_dam [0] = 0;
        you[0].inv_col [0] = LIGHTCYAN;

        // Robe
        you[0].inv_quant [1] = 1;
        you[0].inv_class [1] = OBJ_ARMOUR;
        you[0].inv_type [1] = ARM_ROBE;
        you[0].inv_plus [1] = 50;
        you[0].inv_dam [1] = 1 * 30;
 if (random2(3) != 0) you[0].inv_dam [1] = (4 + random2(2)) * 30;
        you[0].inv_col [1] = random2(15) + 1;

/*      you[0].AC = 1;
        you[0].evasion = 10;*/

        // 11 = spear
        // 9 = hand ae
        // 3 = dagger

//      } // end of switch random2(4)

//      int leftover = 8;

//      you[0].strength = 6;
        you[0].dex += 4;
        you[0].intel += 6;

        you[0].equip [EQ_WEAPON] = 0;
        you[0].equip [EQ_BODY_ARMOUR] = 1;

        you[0].gp = random2(10);

        // etra items being tested:


        you[0].inv_class [2] = OBJ_BOOKS;
        you[0].inv_type [2] = random2(3);
//      strcpy(you[0].inv_name [23], "wand of invisibility");
        you[0].inv_quant [2] = 1;
        you[0].inv_plus [2] = 127;
        you[0].inv_dam [2] = 1;
        // you[0].inv_no = 13;
        you[0].inv_col [2] = 3;

/* you[0].res_magic = 10;*/

        you[0].skills [SK_DODGING] = 1;
        you[0].skills [SK_STEALTH] = 1;
        you[0].skills [SK_DODGING + random() % 2] ++;

/*        if (you[0].skills [SK_DODGING] == 2) you[0].evasion ++;*/

        you[0].skills [SK_SPELLCASTING] = 2;
        you[0].skills [SK_CONJURATIONS] = 1;
        you[0].skills [SK_ENCHANTMENTS] = 1;
        you[0].skills [SK_SPELLCASTING + random() % 3] ++;
        you[0].skills [SK_SUMMONINGS + random() % 5] ++;

        you[0].skills [SK_SHORT_BLADES] = 1;
        you[0].skills [SK_STAVES] = 1;
break;


case JOB_PRIEST: // priest
        strcpy(you[0].clasnam, "priest");
        you[0].piety = 45;
        you[0].hp = 12; you[0].hp_max = 12;
        you[0].ep = 1; you[0].ep_max = 1;
/*      you[0].f_abil = 7;
        you[0].mag_abil = 7;
 you[0].thr_abil = 5;
 you[0].speed = 10;*/
 you[0].spell_levels = 8;

        you[0].inv_quant [0] = 1;
        you[0].inv_class [0] = OBJ_WEAPONS;
        //switch(random2(6))
        //{
        //      case 0 : you[0].inv_type [0] = 1; damage = 6; break;
        //      case 1 : you[0].inv_type [0] = 2; damage = 7; break;
        //      case 2 : you[0].inv_type [0] = 4; damage = 7; break;
        //      case 3 : you[0].inv_type [0] = 9; damage = 7; break;
                //case 4 :
 you[0].inv_type [0] = WPN_MACE;
                //case 5 : you[0].inv_type [0] = 5; damage = 6; break;
        //}


        you[0].inv_plus [0] = 50;
        you[0].inv_plus2 [0] = 50;
        you[0].inv_dam [0] = 0;
        you[0].inv_col [0] = LIGHTCYAN;


        // Robe
        you[0].inv_quant [1] = 1;
        you[0].inv_class [1] = OBJ_ARMOUR;
        you[0].inv_type [1] = ARM_ROBE;
        you[0].inv_plus [1] = 50;
        you[0].inv_dam [1] = 0;
        you[0].inv_col [1] = WHITE;

/*      you[0].AC = 1;
        you[0].evasion = 10;*/

        you[0].inv_class [2] = OBJ_POTIONS;
        you[0].inv_type [2] = POT_HEALING;
//      strcpy(you[0].inv_name [23], "wand of invisibility");
        you[0].inv_quant [2] = 2 + random2(2);
        you[0].inv_plus [2] = 0;
        you[0].inv_dam [2] = 0;
        // you[0].inv_no = 13;
        you[0].inv_col [2] = random2(15) + 1; // hmmm...


        // 11 = spear
        // 9 = hand ae
        // 3 = dagger

//      } // end of switch random2(4)

//      int leftover = 8;

        you[0].strength += 4;
        you[0].dex += 2;
        you[0].intel += 4;

        you[0].equip [EQ_WEAPON] = 0;
        you[0].equip [EQ_BODY_ARMOUR] = 1;

        you[0].gp = random2(10);
/* you[0].res_magic = 10;*/

 you[0].skills [SK_FIGHTING] = 2;
 you[0].skills [SK_DODGING] = 1;
 you[0].skills [SK_SHIELDS] = 1;
// you[0].skills [SK_THROWING] = 2;
 you[0].skills [SK_MACES_FLAILS] = 2;
 you[0].skills [SK_STAVES] = 1;

 you[0].skills [SK_INVOCATIONS] = 4;

clrscr();
cprintf(EOL" Which God do you wish to serve?"EOL);
cprintf("a - Zin (for traditional priests)"EOL);
cprintf("b - Yredelemnul (for priests of death)"EOL);

getkey : keyn = get_ch();
switch(keyn)
{
 case 'a': you[0].religion = GOD_ZIN; cprintf(EOL"Spread the light, my child..."); break;
 case 'b': you[0].religion = GOD_YREDELEMNUL; you[0].inv_type [0] = WPN_MORNINGSTAR; cprintf(EOL"Welcome..."); break;
 default: goto getkey;
}


break;



case JOB_THIEF: // thief
        strcpy(you[0].clasnam, "Thief");
        you[0].hp = 11; you[0].hp_max = 11;
        you[0].ep = 0; you[0].ep_max = 0;
/*      you[0].f_abil = 7;
        you[0].mag_abil = 6;
 you[0].thr_abil = 12;
 you[0].speed = 10;*/
 you[0].spell_levels = 0;

        you[0].inv_quant [0] = 1;
        you[0].inv_class [0] = OBJ_WEAPONS;
                you[0].inv_type [0] = WPN_SHORT_SWORD;// damage = 6; //break;

        you[0].inv_plus [0] = 50;
        you[0].inv_plus2 [0] = 50;
        you[0].inv_dam [0] = 0;
        you[0].inv_col [0] = LIGHTCYAN;


  you[0].inv_quant [1] = random2(10) + random2(10) + 10;
                you[0].inv_class [1] = OBJ_MISSILES;
                you[0].inv_type [1] = MI_DART; //wtype;
                you[0].inv_plus [1] = 50;
                you[0].inv_dam [1] = 0;
                you[0].inv_col [1] = LIGHTCYAN;

        // Robe
        you[0].inv_quant [2] = 1;
        you[0].inv_class [2] = OBJ_ARMOUR;
        you[0].inv_type [2] = ARM_ROBE;
        you[0].inv_plus [2] = 50;
        you[0].inv_dam [2] = 0;
        you[0].inv_col [2] = BROWN;

        you[0].inv_quant [3] = 1;
        you[0].inv_class [3] = OBJ_ARMOUR;
        you[0].inv_type [3] = ARM_CLOAK;
        you[0].inv_plus [3] = 50;
        you[0].inv_dam [3] = 0;
        you[0].inv_col [3] = DARKGREY;

/* you[0].AC = 3;
        you[0].evasion = 9;*/

        // 11 = spear
        // 9 = hand axe
        // 3 = dagger

        you[0].strength += 2;
        you[0].dex += 6;
        you[0].intel += 2;

        you[0].equip [EQ_WEAPON] = 0;
        you[0].equip [EQ_BODY_ARMOUR] = 2;
        you[0].equip [EQ_CLOAK] = 3;

        you[0].gp = random2(10);
/* you[0].res_magic = 3;*/

 you[0].skills [SK_FIGHTING] = 1;
 you[0].skills [SK_SHORT_BLADES] = 2;

 you[0].skills [SK_DODGING] = 2;
 you[0].skills [SK_STEALTH] = 2;
 you[0].skills [SK_STABBING] = 1;
 you[0].skills [SK_DODGING + random() % 3] ++;

/* if (you[0].skills [SK_DODGING] == 2) you[0].evasion ++;*/

 you[0].skills [SK_THROWING] = 1;
 you[0].skills [SK_DARTS] = 1;

 you[0].skills [SK_TRAPS_DOORS] = 2;

break;

case JOB_GLADIATOR: // Gladiator
        strcpy(you[0].clasnam, "Gladiator");
        you[0].hp = 15; you[0].hp_max = 15;
        you[0].ep = 0; you[0].ep_max = 0;
/*      you[0].f_abil = 11;
        you[0].mag_abil = 3;
 you[0].thr_abil = 3;
 you[0].speed = 10;*/
 you[0].spell_levels = 0;

        you[0].inv_quant [0] = 1;
        you[0].inv_class [0] = OBJ_WEAPONS;
                you[0].inv_type [0] = WPN_SHORT_SWORD;
/*  if (random2(3) == 0)
  {
   you[0].inv_type [0] = WPN_FLAIL;
   you[0].skills [SK_MACES_FLAILS] = 3;
  } else
  if (random2(3) == 0)
  {
   you[0].inv_type [0] = WPN_MORNINGSTAR;
   you[0].skills [SK_MACES_FLAILS] = 3;
  }
   else you[0].skills [SK_SHORT_BLADES] = 3;*/
        choose_weapon();
    cprintf(EOL"A fine choice.");
        weap_skill = 3;



        you[0].inv_plus [0] = 50;
        you[0].inv_plus2 [0] = 50;
        you[0].inv_dam [0] = 0;
        you[0].inv_col [0] = LIGHTCYAN;
        if (you[0].species >= SP_RED_DRACONIAN && you[0].species <= SP_UNK2_DRACONIAN)
        {
         you[0].inv_quant [1] = 1;
         you[0].inv_class [1] = OBJ_ARMOUR;
         you[0].inv_type [1] = ARM_ANIMAL_SKIN;
         you[0].inv_plus [1] = 50;
         you[0].inv_dam [1] = 0;
         you[0].inv_col [1] = BROWN;

         you[0].inv_quant [2] = 1;
         you[0].inv_class [2] = OBJ_ARMOUR;
         you[0].inv_type [2] = ARM_SHIELD;
         you[0].inv_plus [2] = 50;
         you[0].inv_dam [2] = 0;
         you[0].inv_col [2] = LIGHTCYAN;
        }
        else
        {
         you[0].inv_quant [1] = 1;
         you[0].inv_class [1] = OBJ_ARMOUR;
         you[0].inv_type [1] = ARM_SCALE_MAIL;
         you[0].inv_plus [1] = 50;
         you[0].inv_dam [1] = 0;
         you[0].inv_col [1] = LIGHTCYAN;

         you[0].inv_quant [2] = 1;
         you[0].inv_class [2] = OBJ_ARMOUR;
         you[0].inv_type [2] = ARM_BUCKLER;
         you[0].inv_plus [2] = 50;
         you[0].inv_dam [2] = 0;
         you[0].inv_col [2] = LIGHTCYAN;
        }
/*      you[0].AC = 4;
        you[0].evasion = 9;*/
        you[0].strength += 7;
        you[0].dex += 3;
        you[0].equip [EQ_WEAPON] = 0;
        you[0].equip [EQ_BODY_ARMOUR] = 1;
        you[0].equip [EQ_SHIELD] = 2;
        you[0].gp = random2(10);
/* you[0].res_magic = 3;*/

 you[0].skills [SK_FIGHTING] = 3;
if (you[0].species >= SP_RED_DRACONIAN && you[0].species <= SP_UNK2_DRACONIAN) you[0].skills [SK_DODGING] = 3;
 else you[0].skills [SK_ARMOUR] = 3;
/* you[0].evasion ++;*/
 you[0].skills [SK_SHIELDS] = 2;

break;


case JOB_NECROMANCER: // Necromancer
        strcpy(you[0].clasnam, "Necromancer");
        you[0].hp = 10; you[0].hp_max = 10;
        you[0].ep = 3; you[0].ep_max = 3;
/*      you[0].f_abil = 5;
        you[0].mag_abil = 7;
 you[0].thr_abil = 5;
 you[0].speed = 10;*/
 you[0].spell_levels = 4;
        you[0].inv_quant [0] = 1;
        you[0].inv_class [0] = OBJ_WEAPONS;
                you[0].inv_type [0] = WPN_DAGGER;
        you[0].inv_plus [0] = 50;
        you[0].inv_plus2 [0] = 50;
        you[0].inv_dam [0] = 0;
        you[0].inv_col [0] = LIGHTCYAN;
        you[0].inv_quant [1] = 1;
        you[0].inv_class [1] = OBJ_ARMOUR;
        you[0].inv_type [1] = ARM_ROBE;
        you[0].inv_plus [1] = 50;
        you[0].inv_dam [1] = 0;
        you[0].inv_col [1] = DARKGREY;
/*      you[0].AC = 1;
        you[0].evasion = 10;*/
        you[0].dex += 4;
        you[0].intel += 6;
        you[0].equip [EQ_WEAPON] = 0;
        you[0].equip [EQ_BODY_ARMOUR] = 1;
        you[0].gp = random2(10);
        you[0].inv_class [2] = OBJ_BOOKS;

        you[0].inv_type [2] = BOOK_NECROMANCY;
        you[0].inv_quant [2] = 1;
        you[0].inv_plus [2] = 127;
        you[0].inv_dam [2] = 0;//1;
        you[0].inv_col [2] = DARKGREY;
/* you[0].res_magic = 10;*/

        you[0].skills [SK_DODGING] = 1;
        you[0].skills [SK_STEALTH] = 1;
        you[0].skills [SK_DODGING + random() % 2] ++;
/* if (you[0].skills [SK_DODGING] == 2) you[0].evasion ++;*/

        you[0].skills [SK_SPELLCASTING] = 1;
        you[0].skills [SK_NECROMANCY] = 4;
        you[0].skills [SK_SHORT_BLADES] = 1;
        you[0].skills [SK_STAVES] = 1;

break;



case JOB_PALADIN: // paladin
        strcpy(you[0].clasnam, "Paladin");
        you[0].religion = GOD_SHINING_ONE;
        you[0].piety = 28;
        you[0].hp = 14; you[0].hp_max = 14;
        you[0].ep = 0; you[0].ep_max = 0;
/*      you[0].f_abil = 9;
        you[0].mag_abil = 6;
 you[0].thr_abil = 4;
 you[0].speed = 10;*/
 you[0].spell_levels = 4;
        you[0].inv_quant [0] = 1;
        you[0].inv_class [0] = OBJ_WEAPONS;
        you[0].inv_type [0] = WPN_SHORT_SWORD;
        you[0].inv_plus [0] = 50;
        you[0].inv_plus2 [0] = 50;
        you[0].inv_dam [0] = 0;
        you[0].inv_col [0] = LIGHTCYAN;
        you[0].inv_quant [1] = 1;
        you[0].inv_class [1] = OBJ_ARMOUR;
        you[0].inv_type [1] = ARM_ROBE;
        you[0].inv_plus [1] = 50;
        you[0].inv_dam [1] = 0;
        you[0].inv_col [1] = WHITE;
        you[0].inv_quant [2] = 1;
        you[0].inv_class [2] = OBJ_ARMOUR;
        you[0].inv_type [2] = ARM_SHIELD;
        you[0].inv_plus [2] = 50;
        you[0].inv_dam [2] = 0;
        you[0].inv_col [2] = LIGHTCYAN;
/*      you[0].AC = 1;
        you[0].evasion = 10;*/
        you[0].strength += 6;
        you[0].dex += 2;
        you[0].intel += 2;
        you[0].equip [EQ_WEAPON] = 0;
        you[0].equip [EQ_BODY_ARMOUR] = 1;
        you[0].gp = random2(10);
 you[0].equip [EQ_SHIELD] = 2;

        you[0].inv_class [3] = OBJ_POTIONS;
        you[0].inv_type [3] = POT_HEALING;
        you[0].inv_quant [3] = 1;
        you[0].inv_plus [3] = 0;
        you[0].inv_dam [3] = 0;
        you[0].inv_col [3] = random2(15) + 1;

 you[0].skills [SK_FIGHTING] = 2;
 you[0].skills [SK_ARMOUR] = 1;
 you[0].skills [SK_DODGING] = 1;
 you[0].skills [SK_ARMOUR + random() % 2] ++;
/*  if (you[0].skills [SK_DODGING] == 2) you[0].evasion ++;*/

 you[0].skills [SK_SHIELDS] = 1;
 you[0].skills [SK_SHORT_BLADES] = 2;
 you[0].skills [SK_LONG_SWORDS] = 1;
 you[0].skills [SK_GREAT_SWORDS] = 1;
 you[0].skills [SK_INVOCATIONS] = 1;
break;

case JOB_ASSASSIN: // assassin
        strcpy(you[0].clasnam, "assassin");
        you[0].hp = 12; you[0].hp_max = 12;
        you[0].ep = 0; you[0].ep_max = 0;
/*      you[0].f_abil = 9;
        you[0].mag_abil = 6;
 you[0].thr_abil = 12;
 you[0].speed = 10;*/
        you[0].inv_quant [0] = 1;
        you[0].inv_class [0] = OBJ_WEAPONS;
        you[0].inv_type [0] = WPN_SHORT_SWORD;
        you[0].inv_plus [0] = 50;
        you[0].inv_plus2 [0] = 50;
        you[0].inv_dam [0] = 0;
        you[0].inv_col [0] = LIGHTCYAN;
        you[0].inv_quant [1] = random2(10) + random2(10) + 10;
                you[0].inv_class [1] = OBJ_MISSILES;
                you[0].inv_type [1] = MI_DART; //wtype;
                you[0].inv_plus [1] = 50;
                you[0].inv_dam [1] = 3;
                you[0].inv_col [1] = LIGHTCYAN;
        you[0].inv_quant [2] = 1;
        you[0].inv_class [2] = OBJ_ARMOUR;
        you[0].inv_type [2] = ARM_ROBE;
        you[0].inv_plus [2] = 50;
        you[0].inv_dam [2] = 0;
        you[0].inv_col [2] = DARKGREY;

        you[0].inv_quant [3] = 1;
        you[0].inv_class [3] = OBJ_ARMOUR;
        you[0].inv_type [3] = ARM_CLOAK;
        you[0].inv_plus [3] = 50;
        you[0].inv_dam [3] = 0;
        you[0].inv_col [3] = DARKGREY;

/* you[0].AC = 2;
        you[0].evasion = 10;*/
        you[0].strength += 3;
        you[0].dex += 5;
        you[0].intel += 2;
        you[0].equip [EQ_WEAPON] = 0;
        you[0].equip [EQ_BODY_ARMOUR] = 2;
        you[0].equip [EQ_CLOAK] = 3;
        you[0].gp = random2(10);
/* you[0].res_magic = 3;*/

 you[0].skills [SK_FIGHTING] = 2;
 you[0].skills [SK_SHORT_BLADES] = 2;

 you[0].skills [SK_DODGING] = 1;
 you[0].skills [SK_STEALTH] = 2;
 you[0].skills [SK_STABBING] = 2;
 you[0].skills [SK_DODGING + random() % 3] ++;

/* if (you[0].skills [SK_DODGING] == 2) you[0].evasion ++;*/

 you[0].skills [SK_THROWING] = 1;
 you[0].skills [SK_DARTS] = 1;

break;

case JOB_BERSERKER: // Barbarian
        strcpy(you[0].clasnam, "Berserker");
        you[0].religion = GOD_TROG;
        you[0].piety = 35;
        you[0].hp = 15; you[0].hp_max = 15;
        you[0].ep = 0; you[0].ep_max = 0;
/*      you[0].f_abil = 10;
        you[0].mag_abil = 2;
 you[0].thr_abil = 10;
 you[0].speed = 10;*/
 you[0].spell_levels = 0;

 if (you[0].species == SP_OGRE)
 {
        you[0].inv_quant [0] = 1;
        you[0].inv_class [0] = OBJ_WEAPONS;
        you[0].inv_type [0] = WPN_CLUB;
        you[0].inv_plus [0] = 50;
        you[0].inv_plus2 [0] = 50;
        you[0].inv_dam [0] = 0;
        you[0].inv_col [0] = BROWN;

 }

 if (you[0].species == SP_TROLL)
 {
        you[0].inv_quant [0] = 0;
        you[0].inv_class [0] = OBJ_WEAPONS;
        you[0].inv_type [0] = WPN_CLUB;
        you[0].inv_plus [0] = 50;
        you[0].inv_dam [0] = 0;
        you[0].inv_col [0] = BROWN;

 }

 if (you[0].species == SP_OGRE || you[0].species == SP_TROLL || (you[0].species >= SP_RED_DRACONIAN && you[0].species <= SP_UNK2_DRACONIAN))
 {
        you[0].inv_quant [1] = 1;
        you[0].inv_class [1] = OBJ_ARMOUR;
        you[0].inv_type [1] = ARM_ANIMAL_SKIN;
        you[0].inv_plus [1] = 50;
        you[0].inv_dam [1] = 0;
        you[0].inv_col [1] = BROWN;
 } else
 {
        you[0].inv_quant [0] = 1;
        you[0].inv_class [0] = OBJ_WEAPONS;
                you[0].inv_type [0] = WPN_HAND_AXE;
        you[0].inv_plus [0] = 50;
        you[0].inv_plus2 [0] = 50;
        you[0].inv_dam [0] = 0;
        you[0].inv_col [0] = LIGHTCYAN;

        you[0].inv_quant [1] = 1;
        you[0].inv_class [1] = OBJ_WEAPONS;
        you[0].inv_type [1] = WPN_SPEAR;
        you[0].inv_plus [1] = 50;
        you[0].inv_plus2 [1] = 50;
        you[0].inv_dam [1] = 0;
        you[0].inv_col [1] = LIGHTCYAN;

        you[0].inv_quant [2] = 1;
        you[0].inv_class [2] = OBJ_WEAPONS;
        you[0].inv_type [2] = WPN_SPEAR;
        you[0].inv_plus [2] = 50;
        you[0].inv_plus2 [2] = 50;
        you[0].inv_dam [2] = 0;
        you[0].inv_col [2] = LIGHTCYAN;

        you[0].inv_quant [3] = 1;
        you[0].inv_class [3] = OBJ_WEAPONS;
        you[0].inv_type [3] = WPN_SPEAR;
        you[0].inv_plus [3] = 50;
        you[0].inv_plus2 [3] = 50;
        you[0].inv_dam [3] = 0;
        you[0].inv_col [3] = LIGHTCYAN;


        you[0].inv_quant [4] = 1;
        you[0].inv_class [4] = OBJ_ARMOUR;
        you[0].inv_type [4] = ARM_LEATHER_ARMOUR;
        you[0].inv_plus [4] = 50;
        you[0].inv_dam [4] = 0;
        you[0].inv_col [4] = BROWN;
 }


/*      you[0].AC = 2;
        you[0].evasion = 9;*/
        you[0].strength += 7;
        you[0].dex += 4;
        you[0].intel -= 1;
        you[0].equip [EQ_WEAPON] = 0;
        you[0].gp = random2(10);
/* you[0].res_magic = 5;*/

 you[0].skills [SK_FIGHTING] = 2;

        if (you[0].species != SP_TROLL)
        {
         you[0].equip [EQ_WEAPON] = 0;
         you[0].equip [EQ_BODY_ARMOUR] = 4;
        } else
        {
         you[0].equip [EQ_WEAPON] = -1;
         you[0].equip [EQ_BODY_ARMOUR] = 1;
        }

        if (you[0].species == SP_OGRE)
        {
         you[0].equip [EQ_WEAPON] = 0;
         you[0].equip [EQ_BODY_ARMOUR] = 1;
        }


if (you[0].species != SP_OGRE && you[0].species != SP_TROLL)
{
 you[0].skills [SK_ARMOUR] = 2;
 you[0].skills [SK_DODGING] = 2;
 you[0].skills [SK_THROWING] = 2;
} else you[0].skills [SK_FIGHTING] += 3;

/* you[0].evasion ++;*/
 you[0].skills [SK_AXES] = 3;
 you[0].skills [SK_POLEARMS] = 1;
break;

case JOB_RANGER: // Ranger
        strcpy(you[0].clasnam, "Ranger");
        you[0].hp = 13; you[0].hp_max = 13;
        you[0].ep = 0; you[0].ep_max = 0;
/*      you[0].f_abil = 9;
        you[0].mag_abil = 6;
 you[0].thr_abil = 11;
 you[0].speed = 10;*/
 you[0].spell_levels = 4;

        you[0].inv_quant [0] = 1;
        you[0].inv_class [0] = OBJ_WEAPONS;
                you[0].inv_type [0] = WPN_DAGGER;
        you[0].inv_plus [0] = 50;
        you[0].inv_plus2 [0] = 50;
        you[0].inv_dam [0] = 0;
        you[0].inv_col [0] = LIGHTCYAN;

        you[0].inv_quant [1] = 1;
        you[0].inv_class [1] = OBJ_WEAPONS;
        you[0].inv_type [1] = WPN_BOW;
        you[0].inv_plus [1] = 50;
        you[0].inv_plus2 [1] = 50;
        you[0].inv_dam [1] = 0;
        you[0].inv_col [1] = BROWN;

        you[0].inv_quant [2] = 15 + random() % 5 + random() % 5 + random() % 5 + random() % 5 + random() % 5;
        you[0].inv_class [2] = OBJ_MISSILES;
        you[0].inv_type [2] = MI_ARROW;
        you[0].inv_plus [2] = 50;
        you[0].inv_plus2 [2] = 50;
        you[0].inv_dam [2] = 0;
        you[0].inv_col [2] = LIGHTCYAN;

        you[0].inv_quant [3] = 1;
        you[0].inv_class [3] = OBJ_ARMOUR;
        you[0].inv_type [3] = ARM_LEATHER_ARMOUR;
        you[0].inv_plus [3] = 50;
        you[0].inv_dam [3] = 0;
        you[0].inv_col [3] = BROWN;

/*      you[0].AC = 2;
        you[0].evasion = 9;*/
        you[0].strength += 3;
        you[0].dex += 4;
        you[0].intel += 3;
        you[0].equip [EQ_WEAPON] = 0;
        you[0].equip [EQ_BODY_ARMOUR] = 3;
        you[0].gp = random2(10);
/* you[0].res_magic = 4;*/

 you[0].skills [SK_FIGHTING] = 2;
 you[0].skills [SK_DODGING] = 1;
/* if (you[0].skills [SK_DODGING] == 2) you[0].evasion ++;*/
 you[0].skills [SK_STEALTH] = 1;
 you[0].skills [SK_STABBING + random() % 2] ++;
 you[0].skills [SK_THROWING] = 2;
 you[0].skills [SK_BOWS] = 2;
 you[0].skills [SK_SPELLCASTING] = 1;
break;

case JOB_CONJURER: // Conjurer
case JOB_ENCHANTER: // Enchanter
case JOB_SUMMONER: // Summoner
case JOB_FIRE_ELEMENTALIST:
case JOB_ICE_ELEMENTALIST:
case JOB_AIR_ELEMENTALIST:
case JOB_EARTH_ELEMENTALIST:
case JOB_VENOM_MAGE:
case JOB_TRANSMUTER:
case JOB_WARPER:
    if (you[0].clas == JOB_CONJURER) strcpy(you[0].clasnam, "conjurer");
        if (you[0].clas == JOB_ENCHANTER) strcpy(you[0].clasnam, "Enchanter");
        if (you[0].clas == JOB_FIRE_ELEMENTALIST) strcpy(you[0].clasnam, "Fire Elementalist");
        if (you[0].clas == JOB_ICE_ELEMENTALIST) strcpy(you[0].clasnam, "Ice Elementalist");
        if (you[0].clas == JOB_SUMMONER) strcpy(you[0].clasnam, "Summoner");
        if (you[0].clas == JOB_AIR_ELEMENTALIST) strcpy(you[0].clasnam, "Air Elementalist");
        if (you[0].clas == JOB_EARTH_ELEMENTALIST) strcpy(you[0].clasnam, "Earth Elementalist");
        if (you[0].clas == JOB_VENOM_MAGE) strcpy(you[0].clasnam, "Venom Mage");
        if (you[0].clas == JOB_TRANSMUTER) strcpy(you[0].clasnam, "transmuter");
        if (you[0].clas == JOB_WARPER) strcpy(you[0].clasnam, "warper");

        switch(random() % 8) /* get a random lvl 1 attack spell - later overwritten for most classes*/
        {
         case 0: you[0].spells [0] = 75; break;
         case 1: you[0].spells [0] = 76; break;
         case 2: you[0].spells [0] = 132; break;
         case 3: you[0].spells [0] = 127; break;
         case 4: you[0].spells [0] = 115; break;
         case 5:
         case 6:
         case 7: you[0].spells [0] = 5; break;
        }

        you[0].hp = 10; you[0].hp_max = 10;
        you[0].ep = 3; you[0].ep_max = 3;
/*      you[0].f_abil = 5;
        you[0].mag_abil = 7;
 you[0].thr_abil = 5;
 you[0].speed = 10;*/
 you[0].spell_levels = 4;
        you[0].inv_quant [0] = 1;
        you[0].inv_class [0] = OBJ_WEAPONS;
                you[0].inv_type [0] = WPN_DAGGER;
        you[0].inv_plus [0] = 50;
        you[0].inv_plus2 [0] = 50;
        if (you[0].clas == JOB_ENCHANTER) you[0].inv_plus [0] = 51;
        if (you[0].clas == JOB_ENCHANTER) you[0].inv_plus2 [0] = 51;
        you[0].inv_dam [0] = 0;
        you[0].inv_col [0] = LIGHTCYAN;

//      if (you[0].species == 17) you[0].inv_type [0] = 17;

        you[0].inv_quant [1] = 1;
        you[0].inv_class [1] = OBJ_ARMOUR;
        you[0].inv_type [1] = ARM_ROBE;
        you[0].inv_plus [1] = 50;
        if (you[0].clas == JOB_ENCHANTER) you[0].inv_plus [1] = 51;
        you[0].inv_dam [1] = 0;
        you[0].inv_col [1] = random() % 15 + 1;
        if (you[0].clas == JOB_FIRE_ELEMENTALIST) you[0].inv_col [1] = RED;
        if (you[0].clas == JOB_ICE_ELEMENTALIST) you[0].inv_col [1] = LIGHTCYAN;
        if (you[0].clas == JOB_AIR_ELEMENTALIST) you[0].inv_col [1] = LIGHTBLUE;
        if (you[0].clas == JOB_EARTH_ELEMENTALIST) you[0].inv_col [1] = BROWN;
        if (you[0].clas == JOB_VENOM_MAGE) you[0].inv_col [1] = GREEN;
/*      you[0].AC = 1;
        if (you[0].clas == JOB_ENCHANTER) you[0].AC = 2;
        you[0].evasion = 10;*/
//      you[0].strength = 6;
        you[0].dex += 4;
        you[0].intel += 6;
        you[0].equip [EQ_WEAPON] = 0;
        you[0].equip [EQ_BODY_ARMOUR] = 1;
        you[0].gp = random2(10);
        you[0].inv_class [2] = OBJ_BOOKS;

/*        you[0].res_magic = 10;*/

        you[0].inv_type [2] = BOOK_CONJURATIONS_I + random2(2); // conj
        you[0].inv_plus [2] = 127;
        if (you[0].clas == JOB_SUMMONER)
        {
         you[0].inv_type [2] = BOOK_SUMMONINGS; // summ
         you[0].inv_plus [2] = 127;
         you[0].skills [SK_SUMMONINGS] = 4;
         // gets some darts - this class is very difficult to start off with
                you[0].inv_quant [3] = random2(5) + random2(5) + 7;
                you[0].inv_class [3] = OBJ_MISSILES;
                you[0].inv_type [3] = MI_DART; //wtype;
                you[0].inv_plus [3] = 50;
                you[0].inv_dam [3] = 0;
                you[0].inv_col [3] = LIGHTCYAN;
        }
        if (you[0].clas == JOB_CONJURER)
        {
         you[0].skills [SK_CONJURATIONS] = 4; // conjurer
        }
        if (you[0].clas == JOB_ENCHANTER) // ench
        {
/*          you[0].res_magic = 12;*/

         you[0].inv_type [2] = BOOK_CHARMS; // charms
         you[0].inv_plus [2] = 127;
         you[0].skills [SK_ENCHANTMENTS] = 4;

        you[0].inv_quant [3] = 1;
        you[0].inv_class [3] = OBJ_WANDS;
        you[0].inv_dam [3] = 0;
        you[0].inv_col [3] = random() % 15 + 1;
        switch(random() % 4)
        {
         case 0: you[0].inv_type [3] = WAND_SLOWING;
         you[0].inv_plus [3] = 7 + random() % 5;
         break;
         case 1: you[0].inv_type [3] = WAND_PARALYSIS;
         you[0].inv_plus [3] = 5 + random() % 4;
         break;
         case 2: you[0].inv_type [3] = WAND_INVISIBILITY;
         you[0].inv_plus [3] = 4 + random() % 4;
         break;
         case 3: you[0].inv_type [3] = WAND_PARALYSIS;
         you[0].inv_plus [3] = 5 + random() % 4;
         break;
        }
        you[0].inv_plus2 [3] = 0;
        } // end of enchanter
        switch(you[0].clas)
        {
         case JOB_FIRE_ELEMENTALIST: // fire elementalist
         you[0].inv_type [2] = BOOK_FLAMES;
         you[0].inv_plus [2] = 127;
         you[0].skills [SK_CONJURATIONS] = 1; // conj
//         you[0].skills [SK_ENCHANTMENTS] = 1; // ench
         you[0].skills [SK_FIRE_MAGIC] = 3; // fire magic
         you[0].spells [0] = SPELL_BURN;
         break;

         case JOB_ICE_ELEMENTALIST: // Ice elementalist
         you[0].inv_type [2] = BOOK_FROST;
         you[0].inv_plus [2] = 127;
         you[0].skills [SK_CONJURATIONS] = 1; // conj
//         you[0].skills [SK_ENCHANTMENTS] = 1; // ench
         you[0].skills [SK_ICE_MAGIC] = 3; // ice magic
         you[0].spells [0] = SPELL_FREEZE;
         break;

         case JOB_AIR_ELEMENTALIST: // Air elementalist
         you[0].inv_type [2] = BOOK_AIR;
         you[0].inv_plus [2] = 127;
         you[0].skills [SK_CONJURATIONS] = 1; // conj
//         you[0].skills [SK_ENCHANTMENTS] = 1; // ench
         you[0].skills [SK_AIR_MAGIC] = 3; // air magic
         you[0].spells [0] = SPELL_ARC;
         break;

         case JOB_EARTH_ELEMENTALIST: // Earth elementalist
         you[0].inv_type [2] = BOOK_GEOMANCY;
         you[0].inv_plus [2] = 127;
         you[0].skills [SK_CONJURATIONS] = 1; // conj
//         you[0].skills [SK_ENCHANTMENTS] = 1; // ench
         you[0].skills [SK_EARTH_MAGIC] = 3; // earth magic
         you[0].spells [0] = SPELL_CRUSH;
         break;

         case JOB_VENOM_MAGE: // Venom Mage
         you[0].inv_type [2] = BOOK_POISONINGS;
         you[0].inv_plus [2] = 126;
         you[0].skills [SK_POISON_MAGIC] = 4; // Poison magic
         you[0].spells [0] = SPELL_STING;
         break;

         case JOB_TRANSMUTER: // transmuter
         you[0].inv_type [2] = BOOK_CHANGES;
         you[0].inv_plus [2] = 126;
         you[0].skills [SK_TRANSMIGRATION] = 4; // transmigrations
         you[0].spells [0] = SPELL_DISRUPT;
         break;

         case JOB_WARPER: // warper
         you[0].inv_type [2] = BOOK_SPATIAL_TRANSLOCATIONS;
         you[0].inv_plus [2] = 124;
         you[0].skills [SK_TRANSLOCATIONS] = 4; // translocations
         break;

        }

        you[0].inv_quant [2] = 1;
//      you[0].inv_plus [2] = 124;
        you[0].inv_dam [2] = 0;
        you[0].inv_col [2] = random() % 15 + 1;
        if (you[0].clas == JOB_FIRE_ELEMENTALIST) you[0].inv_col [2] = RED;
        if (you[0].clas == JOB_ICE_ELEMENTALIST) you[0].inv_col [2] = LIGHTCYAN;
        if (you[0].clas == JOB_VENOM_MAGE) you[0].inv_col [2] = GREEN;
/*  you[0].res_magic = 10;*/

        you[0].skills [SK_DODGING] = 1;
        you[0].skills [SK_STEALTH] = 1;
        you[0].skills [SK_DODGING + random() % 2] ++;
/* if (you[0].skills [SK_DODGING] == 2) you[0].evasion ++;*/

        you[0].skills [SK_SPELLCASTING] = 1;
        you[0].skills [SK_SHORT_BLADES] = 1;
/*      if (you[0].species == SP_OGRE_MAGE)
        {
         you[0].skills [SK_SHORT_BLADES] = 0;
         you[0].skills [SK_POLEARMS] = 2;
        }*/
        you[0].skills [SK_STAVES] = 1;


break;


case JOB_CRUSADER: // Crusader
        strcpy(you[0].clasnam, "Crusader");
/*        you[0].piety = 75;*/
        you[0].hp = 13; you[0].hp_max = 13;
        you[0].ep = 1; you[0].ep_max = 1;
/*      you[0].f_abil = 9;
        you[0].mag_abil = 6;
 you[0].thr_abil = 4;
 you[0].speed = 10;*/
 you[0].spell_levels = 4;
        you[0].inv_quant [0] = 1;
        you[0].inv_class [0] = OBJ_WEAPONS;
        you[0].inv_type [0] = WPN_SHORT_SWORD;

//      if (you[0].species == 17) you[0].inv_type [0] = 17;

        you[0].inv_plus [0] = 50;
        you[0].inv_plus2 [0] = 50;
        you[0].inv_dam [0] = 0;
        you[0].inv_col [0] = LIGHTCYAN;
        choose_weapon();
    cprintf(EOL"A fine choice.");
        weap_skill = 2;
        you[0].inv_quant [1] = 1;
        you[0].inv_class [1] = OBJ_ARMOUR;
        you[0].inv_type [1] = ARM_ROBE;
        you[0].inv_plus [1] = 50;
        you[0].inv_dam [1] = 0;
        you[0].inv_col [1] = 1 + random() % 15;


        you[0].inv_class [2] = OBJ_BOOKS;
        you[0].inv_type [2] = BOOK_WAR_CHANTS;
        you[0].inv_quant [2] = 1;
        you[0].inv_plus [2] = 127;
        you[0].inv_dam [2] = 0;
        you[0].inv_col [2] = 1 + random() % 15;

/*      you[0].AC = 1;
        you[0].evasion = 10;*/
        you[0].strength += 4;
        you[0].dex += 3;
        you[0].intel += 3;
        you[0].equip [EQ_WEAPON] = 0;
        you[0].equip [EQ_BODY_ARMOUR] = 1;
        you[0].gp = random2(10);

 you[0].skills [SK_FIGHTING] = 3;
 you[0].skills [SK_ARMOUR] = 1;
 you[0].skills [SK_DODGING] = 1;
 you[0].skills [SK_STEALTH] = 1;
 you[0].skills [SK_SPELLCASTING] = 2;
 you[0].skills [SK_ENCHANTMENTS] = 2;
break;


case JOB_DEATH_KNIGHT: // Death knight
        strcpy(you[0].clasnam, "Death Knight");
/*        you[0].piety = 75;*/
        you[0].hp = 13; you[0].hp_max = 13;
        you[0].ep = 1; you[0].ep_max = 1;
        you[0].speed = 10;
        you[0].spell_levels = 4;
        you[0].inv_quant [0] = 1;
        you[0].inv_class [0] = OBJ_WEAPONS;
        you[0].inv_type [0] = WPN_SHORT_SWORD;
        you[0].inv_plus [0] = 50;
        you[0].inv_plus2 [0] = 50;
        you[0].inv_dam [0] = 0;
        you[0].inv_col [0] = LIGHTCYAN;
        choose_weapon();
    cprintf(EOL"A fine choice.");
        weap_skill = 2;

        you[0].inv_quant [1] = 1;
        you[0].inv_class [1] = OBJ_ARMOUR;
        you[0].inv_type [1] = ARM_ROBE;
        you[0].inv_plus [1] = 50;
        you[0].inv_dam [1] = 0;
        you[0].inv_col [1] = DARKGREY;


        you[0].inv_class [2] = OBJ_BOOKS;
        you[0].inv_type [2] = BOOK_NECROMANCY;
        you[0].inv_quant [2] = 1;
        you[0].inv_plus [2] = 127;
        you[0].inv_dam [2] = 0;
        you[0].inv_col [2] = DARKGREY;

/*      you[0].AC = 1;
        you[0].evasion = 10;*/
        you[0].strength += 4;
        you[0].dex += 3;
        you[0].intel += 3;
        you[0].equip [EQ_WEAPON] = 0;
        you[0].equip [EQ_BODY_ARMOUR] = 1;
        you[0].gp = random2(10);


if (you[0].species == SP_DEMIGOD)
{
       you[0].skills [SK_SPELLCASTING] = 1;
       you[0].skills [SK_NECROMANCY] = 2;
           you[0].spells [0] = SPELL_PAIN;
}
else
{
 clrscr();
 cprintf(EOL" From where do you draw your power?"EOL);
 cprintf("a - Necromantic magic"EOL);
 cprintf("b - the God Yredelemnul"EOL);

 getkey1 : keyn = get_ch();
 switch(keyn)
 {
  case 'a': cprintf(EOL"Very well.");
       you[0].skills [SK_SPELLCASTING] = 1;
       you[0].skills [SK_NECROMANCY] = 2;
           you[0].spells [0] = SPELL_PAIN;
       break;
  case 'b':
       you[0].religion = GOD_YREDELEMNUL;
           you[0].inv_plus [0] = 51;
           you[0].inv_plus2 [0] = 51;
       you[0].inv_quant [2] = 0;
       you[0].skills [SK_INVOCATIONS] = 3;
       you[0].piety = 28;
       cprintf(EOL"Welcome...");
       break;
  default: goto getkey1;
 }

}

 you[0].skills [SK_FIGHTING] = 2;
 you[0].skills [SK_ARMOUR] = 1;
 you[0].skills [SK_DODGING] = 1;
/*  if (you[0].skills [SK_DODGING] == 2) you[0].evasion ++;*/

 you[0].skills [SK_STEALTH] = 1;
// you[0].skills [SK_SHORT_BLADES] = 2;
 you[0].skills [SK_STABBING] = 1;


break;

case JOB_CHAOS_KNIGHT: // Chaos knight
        strcpy(you[0].clasnam, "Knight of Chaos");
/*        you[0].piety = 75;*/
        you[0].hp = 13; you[0].hp_max = 13;
        you[0].ep = 1; you[0].ep_max = 1;
        you[0].speed = 10;
        you[0].spell_levels = 4;
        you[0].inv_quant [0] = 1;
        you[0].inv_class [0] = OBJ_WEAPONS;
        you[0].inv_type [0] = WPN_SHORT_SWORD;
        you[0].inv_plus [0] = 50 + random2(3);
        you[0].inv_plus2 [0] = 50 + random2(3);
        you[0].inv_dam [0] = 0;
        if (random2(5) == 0) you[0].inv_dam [0] = 30;
        if (random2(5) == 0) you[0].inv_dam [0] = 60;
        you[0].inv_col [0] = LIGHTCYAN;
        choose_weapon();
        weap_skill = 2;
        you[0].inv_quant [1] = 1;
        you[0].inv_class [1] = OBJ_ARMOUR;
        you[0].inv_type [1] = ARM_ROBE;
        you[0].inv_plus [1] = 50 + random2(3);
        you[0].inv_dam [1] = 0;
        you[0].inv_col [1] = random2(15) + 1;

        you[0].strength += 4;
        you[0].dex += 3;
        you[0].intel += 3;
        you[0].equip [EQ_WEAPON] = 0;
        you[0].equip [EQ_BODY_ARMOUR] = 1;
        you[0].gp = random2(10);

 you[0].skills [SK_FIGHTING] = 3;
 you[0].skills [SK_ARMOUR] = 1;
 you[0].skills [SK_DODGING] = 1;
 you[0].skills [SK_ARMOUR + random() % 2] ++;

 you[0].skills [SK_STABBING] = 1;

clrscr();
cprintf(EOL" Which God of Chaos do you wish to serve?"EOL);
cprintf("a - Xom of Chaos"EOL);
cprintf("b - Makhleb the Destroyer"EOL);

getkey2 : keyn = get_ch();
switch(keyn)
{
 case 'a': you[0].religion = GOD_XOM; you[0].skills [SK_FIGHTING] ++; cprintf(EOL"A new plaything! Welcome..."); break;
 case 'b': you[0].religion = GOD_MAKHLEB; you[0].skills [SK_INVOCATIONS] = 2; cprintf(EOL"Blood and souls for Makhleb!"); break;
 default: goto getkey2;
}

you[0].piety = 25; /* Irrelevant for Xom, of course */

break;



case JOB_HEALER: // Healer
        strcpy(you[0].clasnam, "Healer");
        you[0].piety = 45;
        you[0].hp = 15; you[0].hp_max = 15;
        you[0].ep = 1; you[0].ep_max = 1;
        you[0].spell_levels = 8;

        you[0].inv_quant [0] = 1;
        you[0].inv_class [0] = OBJ_WEAPONS;
        you[0].inv_type [0] = WPN_QUARTERSTAFF;

        you[0].inv_plus [0] = 50;
        you[0].inv_plus2 [0] = 50;
        you[0].inv_dam [0] = 0;
        you[0].inv_col [0] = BROWN;


        // Robe
        you[0].inv_quant [1] = 1;
        you[0].inv_class [1] = OBJ_ARMOUR;
        you[0].inv_type [1] = ARM_ROBE;
        you[0].inv_plus [1] = 50;
        you[0].inv_dam [1] = 0;
        you[0].inv_col [1] = WHITE;

        you[0].inv_class [2] = OBJ_POTIONS;
        you[0].inv_type [2] = POT_HEALING;
        you[0].inv_quant [2] = 2 + random2(2);
        you[0].inv_plus [2] = 0;
        you[0].inv_dam [2] = 0;
        you[0].inv_col [2] = random2(15) + 1; // hmmm...

        you[0].inv_class [3] = OBJ_POTIONS;
        you[0].inv_type [3] = POT_HEAL_WOUNDS;
        you[0].inv_quant [3] = 2 + random2(2);
        you[0].inv_plus [3] = 0;
        you[0].inv_dam [3] = 0;
        you[0].inv_col [3] = random2(15) + 1; // hmmm...

        you[0].strength += 4;
        you[0].dex += 2;
        you[0].intel += 4;

        you[0].equip [EQ_WEAPON] = 0;
        you[0].equip [EQ_BODY_ARMOUR] = 1;

        you[0].gp = random2(100);

 you[0].skills [SK_FIGHTING] = 2;
 you[0].skills [SK_DODGING] = 1;
 you[0].skills [SK_SHIELDS] = 1;
 you[0].skills [SK_THROWING] = 2;
 you[0].skills [SK_STAVES] = 3;

 you[0].skills [SK_INVOCATIONS] = 2;

 you[0].religion = GOD_ELYVILON;

break;


case JOB_REAVER: // Reaver
        strcpy(you[0].clasnam, "Reaver");
        you[0].hp = 13; you[0].hp_max = 13;
        you[0].ep = 1; you[0].ep_max = 1;
        you[0].speed = 10;
        you[0].inv_quant [0] = 1;
        you[0].inv_class [0] = OBJ_WEAPONS;
        you[0].inv_type [0] = WPN_SHORT_SWORD;
        you[0].inv_plus [0] = 50;
        you[0].inv_plus2 [0] = 50;
        you[0].inv_dam [0] = 0;
        you[0].inv_col [0] = LIGHTCYAN;
        choose_weapon();
    cprintf(EOL"A fine choice.");
        weap_skill = 3;

        you[0].inv_quant [1] = 1;
        you[0].inv_class [1] = OBJ_ARMOUR;
        you[0].inv_type [1] = ARM_ROBE;
        you[0].inv_plus [1] = 50;
        you[0].inv_dam [1] = 0;
        you[0].inv_col [1] = RED;


        you[0].inv_class [2] = OBJ_BOOKS;
        you[0].inv_type [2] = BOOK_CONJURATIONS_I + random2(2);
        you[0].inv_quant [2] = 1;
        you[0].inv_plus [2] = 127;
        you[0].inv_dam [2] = 0;
        you[0].inv_col [2] = RED;

        you[0].strength += 4;
        you[0].dex += 2;
        you[0].intel += 4;
        you[0].equip [EQ_WEAPON] = 0;
        you[0].equip [EQ_BODY_ARMOUR] = 1;
        you[0].gp = random2(10);

 you[0].skills [SK_FIGHTING] = 2;
 you[0].skills [SK_ARMOUR] = 1;
 you[0].skills [SK_DODGING] = 1;

 you[0].skills [SK_SPELLCASTING] = 1;
 you[0].skills [SK_CONJURATIONS] = 2;
 you[0].spells [0] = SPELL_MAGIC_DART;
break;

case JOB_STALKER: // stalker
        strcpy(you[0].clasnam, "stalker");
        you[0].hp = 11; you[0].hp_max = 11;
        you[0].ep = 1; you[0].ep_max = 1;
        you[0].inv_quant [0] = 1;
        you[0].inv_class [0] = OBJ_WEAPONS;
        you[0].inv_type [0] = WPN_DAGGER;
        you[0].inv_plus [0] = 50;
        you[0].inv_plus2 [0] = 50;
        you[0].inv_dam [0] = 0;
        you[0].inv_col [0] = LIGHTCYAN;
        you[0].inv_quant [1] = 1;
        you[0].inv_class [1] = OBJ_ARMOUR;
        you[0].inv_type [1] = ARM_ROBE;
        you[0].inv_plus [1] = 50;
        you[0].inv_dam [1] = 0;
        you[0].inv_col [1] = GREEN;
        you[0].inv_quant [2] = 1;
        you[0].inv_class [2] = OBJ_ARMOUR;
        you[0].inv_type [2] = ARM_CLOAK;
        you[0].inv_plus [2] = 50;
        you[0].inv_dam [2] = 0;
        you[0].inv_col [2] = DARKGREY;
        you[0].inv_class [3] = OBJ_BOOKS;
        you[0].inv_type [3] = ARM_BUCKLER;
        you[0].inv_quant [3] = 1;
        you[0].inv_plus [3] = 126;
        you[0].inv_dam [3] = 0;
        you[0].inv_col [3] = GREEN;


        you[0].strength += 2;
        you[0].dex += 5;
        you[0].intel += 3;
        you[0].equip [EQ_WEAPON] = 0;
        you[0].equip [EQ_BODY_ARMOUR] = 1;
        you[0].equip [EQ_CLOAK] = 2;
        you[0].gp = random2(10);

        you[0].spells [0] = SPELL_STING;


 you[0].skills [SK_FIGHTING] = 1;
 you[0].skills [SK_SHORT_BLADES] = 1;

 you[0].skills [SK_POISON_MAGIC] = 1;

 you[0].skills [SK_DODGING] = 1;
 you[0].skills [SK_STEALTH] = 2;
 you[0].skills [SK_STABBING] = 2;
 you[0].skills [SK_DODGING + random() % 3] ++;

/* if (you[0].skills [SK_DODGING] == 2) you[0].evasion ++;*/

 you[0].skills [SK_THROWING] = 1;
 you[0].skills [SK_DARTS] = 1;

break;

case JOB_MONK: // Monk
        strcpy(you[0].clasnam, "Monk");
        you[0].hp = 13; you[0].hp_max = 13;
        you[0].ep = 0; you[0].ep_max = 0;
        you[0].inv_class [0] = OBJ_ARMOUR;
        you[0].inv_type [0] = ARM_ROBE;
        you[0].inv_plus [0] = 50;
        you[0].inv_dam [0] = 0;
        you[0].inv_quant [0] = 1;
        you[0].inv_col [0] = BROWN;

        you[0].strength += 3;
        you[0].dex += 5;
        you[0].intel += 2;
        you[0].equip [EQ_WEAPON] = -1;
        you[0].equip [EQ_BODY_ARMOUR] = 0;
        you[0].gp = 0;

 you[0].skills [SK_FIGHTING] = 3;
 you[0].skills [SK_UNARMED_COMBAT] = 4;

 you[0].skills [SK_DODGING] = 3;
 you[0].skills [SK_STEALTH] = 2;

break;


/*
Spellbook binary thing:
6 spells: 127
5 spells: 126
4 spells: 124
3 spells: 120
2 spells: 112
1 spells: 96
0 spells: 64
(assuming all from start, no empty spaces)
*/

}

/*you[0].mag_abil = 0;*/


/*you[0].res_magic = 3 + you[0].skills [SK_ENCHANTMENTS] * 2;*/

char points_left = 8;

if (you[0].species == SP_DEMIGOD || you[0].species == SP_DEMONSPAWN) points_left += 7; /* demigod */

//for (i = 0; i < 8; i ++)
      do
      {
                switch(random2(3))
                {
                case 0 : if (you[0].strength > 17 && random() % 2 != 0) continue;
                        you[0].strength ++; break;
                        case 1 : if (you[0].dex > 17 && random() % 2 != 0) continue;
                        you[0].dex ++; break;
                        case 2 : if (you[0].intel > 17 && random() % 2 != 0) continue;
                        you[0].intel ++; break;
                }
        points_left --;
      } while (points_left > 0);

        // end of for i


you[0].base_hp = 5000;
you[0].base_hp2 = 5000 + you[0].hp_max;

you[0].base_ep = 5000;
you[0].base_ep2 = 5000 + you[0].ep_max;

you[0].is_undead = 0;

switch(you[0].species)
{
case SP_ELF: // elf
you[0].hp_max --;
you[0].base_hp2 --;
break;
case SP_HIGH_ELF: // high elf
you[0].hp_max --;
you[0].base_hp2 --;
break;
case SP_GREY_ELF: // grey elf
you[0].hp_max --;
you[0].base_hp2 --;
you[0].ep_max ++;
you[0].base_ep2 ++;
break;
case SP_DEEP_ELF: // deep elf
you[0].hp_max --;
you[0].hp_max --;
you[0].base_hp2 -= 2;
you[0].ep_max ++;
you[0].base_ep2 ++;
break;
case SP_HILL_DWARF: // hill dwarf
you[0].hp_max ++;
you[0].base_hp2 ++;
break;
case SP_MOUNTAIN_DWARF: // mountain dwarf
you[0].hp_max ++;
you[0].base_hp2 ++;
break;
case SP_HALFLING: // halfling
you[0].hp_max --;
you[0].hp_max --;
you[0].base_hp2 --;
you[0].base_hp2 --;
/*you[0].evasion ++;*/
break;
case SP_HILL_ORC: // hill orc
you[0].hp_max ++;
you[0].base_hp2 ++;
break;
case SP_KOBOLD: // kobold
you[0].hp_max -= 2;
you[0].base_hp2 -= 2;
/*you[0].evasion ++;*/
break;
case SP_MUMMY: // mummy
you[0].hp_max ++;
you[0].base_hp2 ++;
/*you[0].res_cold ++;
you[0].res_fire --;
you[0].prot_life ++;
you[0].res_poison ++;*/
you[0].is_undead = 2;
/*you[0].sust_abil = 1;
you[0].evasion --;*/
break;
case SP_NAGA: // Naga
you[0].hp_max += 2;
you[0].base_hp2 += 2;
/*you[0].evasion -= 3;
you[0].AC += 2;
you[0].res_poison ++;
you[0].see_invis ++;*/
you[0].attribute [ATTR_WALK_SLOWLY] ++;
break;
case SP_GNOME: // Gnome
you[0].hp_max --;
you[0].hp_max --;
you[0].base_hp2 --;
you[0].base_hp2 --;
/*you[0].AC ++;*/
break;
case SP_OGRE: // Ogre
you[0].hp_max += 3;
you[0].base_hp2 += 3;
you[0].hunger_inc += 1;
break;
case SP_TROLL: // Troll
you[0].hp_max += 3;
you[0].base_hp2 += 3;
you[0].hunger_inc += 3;
break;
case SP_OGRE_MAGE: // Ogre-Mage
you[0].hp_max += 2;
you[0].base_hp2 += 2;
you[0].hunger_inc += 1;
break;
case SP_RED_DRACONIAN: // Draconian
case SP_WHITE_DRACONIAN: // Draconian
case SP_GREEN_DRACONIAN: // Draconian
case SP_GOLDEN_DRACONIAN: // Draconian
case SP_GREY_DRACONIAN: // Draconian
case SP_BLACK_DRACONIAN: // Draconian
case SP_PURPLE_DRACONIAN: // Draconian
case SP_MOTTLED_DRACONIAN: // Draconian
case SP_PALE_DRACONIAN: // Draconian
case SP_UNK0_DRACONIAN: // Draconian
case SP_UNK1_DRACONIAN: // Draconian
case SP_UNK2_DRACONIAN: // Draconian
you[0].hp_max += 1;
you[0].base_hp2 += 1;
break;
case SP_CENTAUR: // Centaur
you[0].hp_max += 3;
you[0].base_hp2 += 3;
you[0].hunger_inc += 1;
you[0].hunger_inc += 1;
break;
case SP_DEMIGOD: // Demigod
you[0].hp_max += 3;
you[0].base_hp2 += 3;
you[0].hunger_inc += 1;
you[0].ep_max ++;
you[0].base_ep2 ++;
break;
case SP_SPRIGGAN: // spriggan
you[0].hp_max -= 2;
you[0].base_hp2 -= 2;
break;
case SP_MINOTAUR: // Minotaur
you[0].hp_max += 2;
you[0].base_hp2 += 2;
break;
case SP_GHOUL: // Ghoul
you[0].hp_max += 2;
you[0].base_hp2 += 2;
you[0].is_undead = 1;
break;
case SP_KENKU: // Kenku
you[0].hp_max -= 2;
you[0].base_hp2 -= 2;
break;
}

you[0].hp = you[0].hp_max;
you[0].ep = you[0].ep_max;

you[0].max_dex = you[0].dex;
you[0].max_strength = you[0].strength;
you[0].max_intel = you[0].intel;

if (weap_skill != 0) you[0].skills [weapon_skill(0, you[0].inv_type [0])] = weap_skill;

if (you[0].is_undead == 0) // != 2)
{
 for (i = 0; i < 52; i ++)
 {
  if (you[0].inv_quant [i] == 0)
  {
   you[0].inv_quant [i] = 1;
   you[0].inv_class [i] = OBJ_FOOD;
   you[0].inv_type [i] = FOOD_BREAD_RATION;
   if (you[0].species == SP_HILL_ORC || you[0].species == SP_KOBOLD || you[0].species == SP_OGRE || you[0].species == SP_TROLL) you[0].inv_type [i] = FOOD_MEAT_RATION;
   you[0].inv_col [i] = BROWN;
   you[0].inv_no ++;
   break;
  }
 }
}

for (i = 0; i < 52; i ++)
{
 if (you[0].inv_quant [i] != 0)
 {
  if (you[0].inv_class [i] == OBJ_BOOKS)
  {
   you[0].had_item [you[0].inv_type [i]] = 1;
   if (you[0].inv_type [i] == BOOK_MINOR_MAGIC_I || you[0].inv_type [i] == BOOK_MINOR_MAGIC_II || you[0].inv_type [i] == BOOK_MINOR_MAGIC_III)
   {
    you[0].had_item [BOOK_MINOR_MAGIC_I] = 1;
    you[0].had_item [BOOK_MINOR_MAGIC_II] = 1;
    you[0].had_item [BOOK_MINOR_MAGIC_III] = 1;
   }
   if (you[0].inv_type [i] == BOOK_CONJURATIONS_I || you[0].inv_type [i] == BOOK_CONJURATIONS_II)
   {
    you[0].had_item [BOOK_CONJURATIONS_I] = 1;
    you[0].had_item [BOOK_CONJURATIONS_II] = 1;
   }
  }

  if (you[0].inv_class [i] <= OBJ_ARMOUR) // || you[0].inv_class [i] == 2)
  switch(you[0].species)
  {
   case SP_ELF:
   case SP_HIGH_ELF:
   case SP_GREY_ELF:
   case SP_DEEP_ELF:
   case SP_SLUDGE_ELF:
   you[0].inv_dam [i] += 120;
   break;

   case SP_HILL_DWARF:
   case SP_MOUNTAIN_DWARF:
   you[0].inv_dam [i] += 150;
   you[0].inv_col [i] = CYAN;
   break;

   case SP_HILL_ORC:
   if (you[0].inv_class [i] == OBJ_WEAPONS) you[0].inv_dam [i] += 90;
     else you[0].inv_dam [i] += 180;
   break;
  }
 }
}


// must remember to check for already existing colours/combinations

for (i = 0; i < 4; i ++)
{
   for (j = 0; j < 50; j ++)
   {
      you[0].item_description [i] [j] = 215;
   }
}

for (i = 0; i < 30; i ++)
{
 you[0].duration [i] = 0;
}

you[0].item_description [1] [13] = 178; // oatmeal
you[0].item_description [1] [16] = 0; // water

for (i = 0; i < 4; i ++)
{
   for (j = 0; j < 50; j ++)
   {

       do
       {
       passout = 1;

       switch(i)
       {
         // This bit is horrendous:

         case 0: // wands
         if (random2(2) != 0)
         {
            you[0].item_description [i] [j] = random2(12);
         } else
               {
//                  you[0].item_description [i] [j] = (random2(12) * 16) + random2(15);
                    you[0].item_description [i] [j] = (random2(12)) + random2(12) * 16;
               }
         break;

         case 1: // potions
         if (random2(2) != 0)
         {
            you[0].item_description [i] [j] = random2(14);
         } else
               {
                  you[0].item_description [i] [j] = (random2(14) * 14) + random2(14);
               }
         break;

         case 2: // scrolls
         you[0].item_description [i] [j] = random2(151);
         you[0].item_description [4] [j] = random2(151);
         break;

         case 3: // rings
                if (random2(2) > 0) you[0].item_description [i] [j] = random2(13);
                         else you[0].item_description [i] [j] = random2(13) + (random2(13) * 13);
         break;

       } // end switch

       for (p = 0; p < 30; p ++) // don't have p < j because some are preassigned
       {
        if (you[0].item_description [i] [p] == you[0].item_description [i] [j] && j != p)
           passout = 0;
       }

       } while (passout == 0);

   }
}



for (i = 0; i < 4; i ++)
{
   for (j = 0; j < 30; j ++)
   {
    you[0].item_description [i] [j] += 1; // NEED THIS BIT!
   }
}

for (i = 0; i < 50; i ++)
{
 if (you[0].skills [i] == 0) continue;

// you[0].skill_points [i] = skill_exp_needed(you[0].skills [i] + 1) * species_skills(i, you[0].species) / 100;

 you[0].skill_points [i] = skill_exp_needed(you[0].skills [i] + 1) + 1;
 if (i == 25)
 {
  you[0].skill_points [i] = ((skill_exp_needed(you[0].skills [i] + 1) + 1) * 130) / 100;
 }
 if (i == 38)
 {
  you[0].skill_points [i] = ((skill_exp_needed(you[0].skills [i] + 1) + 1) * 70) / 100;
 }
 if (you[0].skill_points [i] > skill_exp_needed(2) * species_skills(i, you[0].species) / 100) you[0].skills [i] = 1; else you[0].skills [i] = 0;
 if (you[0].skill_points [i] > skill_exp_needed(3) * species_skills(i, you[0].species) / 100) you[0].skills [i] = 2;
 if (you[0].skill_points [i] > skill_exp_needed(4) * species_skills(i, you[0].species) / 100) you[0].skills [i] = 3;
 if (you[0].skill_points [i] > skill_exp_needed(5) * species_skills(i, you[0].species) / 100) you[0].skills [i] = 4;
 if (you[0].skill_points [i] > skill_exp_needed(6) * species_skills(i, you[0].species) / 100) you[0].skills [i] = 5;
 if (you[0].skill_points [i] > skill_exp_needed(7) * species_skills(i, you[0].species) / 100) you[0].skills [i] = 6;
 if (you[0].skill_points [i] > skill_exp_needed(8) * species_skills(i, you[0].species) / 100) you[0].skills [i] = 7;
 if (you[0].skill_points [i] > skill_exp_needed(9) * species_skills(i, you[0].species) / 100) you[0].skills [i] = 8;
}

for (i = 0; i < 52; i ++)
{
        if (you[0].inv_class [i] != OBJ_WEAPONS)
        {
                set_id(you[0].inv_class [i], you[0].inv_type [i], 1);
        }
}


/*if (you[0].clas == JOB_FIGHTER) you [0].shield_class = 5;
if (you[0].clas == JOB_GLADIATOR) you [0].shield_class = 3;
if (you[0].clas == JOB_PALADIN) you [0].shield_class = 5;

if (you[0].clas == JOB_PRIEST || you[0].clas == JOB_PALADIN) you[0].spec_holy = 1;*/

if (you[0].clas == JOB_WIZARD || you[0].clas == JOB_CONJURER) you[0].spells [0] = SPELL_MAGIC_DART;
if (you[0].clas == JOB_NECROMANCER) you[0].spells [0] = SPELL_PAIN;
if (you[0].clas == JOB_NECROMANCER) you[0].spells [1] = SPELL_ANIMATE_SKELETON;
if (you[0].clas == JOB_FIRE_ELEMENTALIST) you[0].spells [0] = SPELL_BURN;
if (you[0].clas == JOB_ICE_ELEMENTALIST) you[0].spells [0] = SPELL_FREEZE;
if (you[0].clas == JOB_SUMMONER) you[0].spells [0] = SPELL_SUMMON_SMALL_MAMMAL;

/*case 10: // Conjurer
case 11: // Enchanter
case 12: // Fire Wizard
case 13: // Ice Wizard
case 14: // Summoner*/

if (you[0].spells [0] != 210) you[0].spell_no = 1; else you[0].spell_no = 0;
if (you[0].clas == JOB_PRIEST || you[0].clas == JOB_PALADIN) set_id(OBJ_POTIONS, POT_HEALING, 1);

you[0].spell_levels = you[0].skills [SK_SPELLCASTING] * 2 - (you[0].spells [0] != 210) - (you[0].spells [1] != 210);


        char del_file [kFileNameSize];


char glorpstr [kFileNameSize];
strncpy(glorpstr, you[0].your_name, kFileNameLen);

// glorpstr [strlen(glorpstr)] = 0;
// This is broken. Length is not valid yet! We have to check if we got a
// trailing NULL; if not, write one:
if (strlen(you[0].your_name) > kFileNameLen-1)    /* is name 6 chars or more? */
        glorpstr[kFileNameLen] = (char) NULL;   /* if so, char 7 should be NULL */

strncpy(glorpstr, you[0].your_name, kFileNameLen);

// glorpstr [strlen(glorpstr)] = 0;
// This is broken. Length is not valid yet! We have to check if we got a
// trailing NULL; if not, write one:
if (strlen(you[0].your_name) > kFileNameLen-1)    /* is name 6 chars or more? */
        glorpstr[kFileNameLen] = (char) NULL;   /* if so, char 7 should be NULL */

/*int fi = 0;
char st_prn [6];

for (fi = 0; fi < 100; fi ++)
{
strcpy(del_file, glorpstr);
strcat(del_file, ".");
itoa(fi, st_prn, 10);
strcat(del_file, st_prn);
strcat(del_file, "\0");
handle = open(del_file, S_IWRITE, S_IREAD);

if (handle != -1)
{
        close(handle);
        sysg = unlink(del_file);
} else close(handle);
}*/

int fi = 0;
int fi2 = 0;
char st_prn [6];

for (fi2 = 0; fi2 < 30; fi2 ++)
{
 for (fi = 0; fi < 50; fi ++)
 {
  strcpy(del_file, glorpstr);
  strcat(del_file, ".");
  if (fi < 10) strcat(del_file, "0");
  itoa(fi, st_prn, 10);
  strcat(del_file, st_prn);
  st_prn [0] = fi2 + 97;
  st_prn [1] = 0;
  strcat(del_file, st_prn);
  strcat(del_file, "\0");
  handle = open(del_file, S_IWRITE, S_IREAD);

  if (handle != -1)
  {
        close(handle);
        unlink(del_file);
  } else close(handle);
 }
}

/*
char hbjh [5];

if (level_saved < 10) strcpy(extens, "0");
itoa(level_saved, hbjh, 10);
strcat(extens, hbjh);
corr_level [2] = you[0].where_are_you + 97;
corr_level [3] = 0; / * null-terminate it * /
strcpy(cha_fil, "");
strncat(cha_fil, you[0].your_name, kFileNameLen);
strcat(cha_fil, ".");
if (was_a_labyrinth == 1) strcat(cha_fil, "lab"); / * temporary level * /
 else strcat(cha_fil, extens);
*/

for (i = 0; i < 30; i ++)
{
 you[0].branch_stairs [i] = 100;
}
/* Places the staircases to the branch levels: */

you[0].branch_stairs [0] = 5 + random2(6); // orc mines
you[0].branch_stairs [1] = 10 + random2(6); // hive
you[0].branch_stairs [2] = 7 + random2(6); // lair
you[0].branch_stairs [3] = you[0].branch_stairs [2] + random2(4) + 3; // slime pits
you[0].branch_stairs [4] = 13 + random2(6); // vaults
you[0].branch_stairs [5] = you[0].branch_stairs [4] + random2(3) + 2; // crypt
you[0].branch_stairs [6] = you[0].branch_stairs [5] + 4; // hall of blades
you[0].branch_stairs [7] = 26; // hall of Zot
you[0].branch_stairs [8] = 3 + random2(4); // Temple
you[0].branch_stairs [9] = you[0].branch_stairs [2] + random2(2) + 6; // Snake pit
you[0].branch_stairs [10] = you[0].branch_stairs [0] + random2(2) + 3; // elven halls
you[0].branch_stairs [11] = you[0].branch_stairs [5] + random2(2) + 2; // Tomb
you[0].branch_stairs [12] = you[0].branch_stairs [2] + random2(6) + 2; // Swamp
                return 1;

} // end of new_game()



char class_allowed(char speci, char clas)
{
switch(clas)
{
case JOB_FIGHTER: // fighter
switch(speci)
{
 case SP_OGRE_MAGE: return 0; // O-mage
 case SP_SPRIGGAN: return 0; // spriggan
}
return 1;

case JOB_WIZARD: // wizard
switch(speci)
{
 case SP_HILL_ORC:
 case SP_HALFLING:
 case SP_HILL_DWARF:
 case SP_KOBOLD:
 case SP_MUMMY:
 case SP_GNOME: // gnome
// case 8:
 case SP_OGRE:
 case SP_TROLL:
 case SP_SPRIGGAN: // spriggan
 case SP_MINOTAUR: // minotaur
 case SP_GHOUL: // Ghoul
 case SP_KENKU: // Kenku
 return 0;
}
return 1;

case JOB_PRIEST: // priest
switch(speci)
{
 case SP_HALFLING:
 case SP_KOBOLD:
 case SP_MUMMY:
 case SP_NAGA:
 case SP_GNOME: // gnome
 case SP_OGRE:
 case SP_TROLL:
 case SP_OGRE_MAGE:
 case SP_RED_DRACONIAN: /* Drac */
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
 case SP_UNK2_DRACONIAN: /* Draconians */
 case SP_CENTAUR: /* Centaur */
 case SP_DEMIGOD: /* Demigod */
 case SP_SPRIGGAN: /* spriggan */
 case SP_MINOTAUR: // minotaur
 case SP_DEMONSPAWN: // demonspawn
 case SP_GHOUL: // Ghoul
 case SP_KENKU: // Kenku
 return 0;
}
return 1;

case JOB_THIEF: // thief
switch(speci)
{
 case SP_MUMMY:
 case SP_OGRE:
 case SP_TROLL:
 case SP_OGRE_MAGE:
 case SP_CENTAUR: /* Centaur */
 case SP_SPRIGGAN: // spriggan
 case SP_MINOTAUR: // minotaur
 case SP_GHOUL: // Ghoul
 case SP_KENKU: // Kenku
 return 0;
}
return 1;

case JOB_GLADIATOR: // glad
switch(speci)
{
 case SP_HUMAN:
 case SP_HILL_DWARF:
 case SP_MOUNTAIN_DWARF:
 case SP_HILL_ORC:
 case SP_RED_DRACONIAN: /* Drac */
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
 case SP_UNK2_DRACONIAN: /* Draconians */
 case SP_CENTAUR:
 case SP_DEMIGOD:
 case SP_MINOTAUR: // minotaur
 case SP_DEMONSPAWN: // demonspawn

 case SP_KENKU: // Kenku
 return 1;
}
return 0;

case JOB_NECROMANCER: // necromancer
switch(speci)
{
 case SP_HUMAN:
 case SP_DEEP_ELF:
 case SP_SLUDGE_ELF:
 case SP_HILL_ORC:
 case SP_KOBOLD:
 case SP_MUMMY:
 case SP_NAGA:
 case SP_OGRE_MAGE: /* Ogre-mage */
 case SP_DEMIGOD: /* Demigod */
 case SP_DEMONSPAWN: // demonspawn

 case SP_KENKU: // Kenku
 return 1;
}
return 0;

case JOB_PALADIN: // Paladin
switch(speci)
{
 case SP_HUMAN:
 case SP_MOUNTAIN_DWARF:
 return 1;
}
return 0;

case JOB_ASSASSIN: // assassin
switch(speci)
{
 case SP_HUMAN:
 case SP_ELF:
 case SP_HIGH_ELF:
 case SP_GREY_ELF:
 case SP_DEEP_ELF:
 case SP_SLUDGE_ELF:
 case SP_HILL_ORC:
 case SP_KOBOLD:
 case SP_MUMMY:
 case SP_NAGA:
 case SP_DEMIGOD: /* Demigod */
 case SP_DEMONSPAWN: // demonspawn

 case SP_KENKU: // Kenku
 return 1;
}
return 0;

case JOB_BERSERKER: // berserker
switch(speci)
{
 case SP_HUMAN:
 case SP_HILL_DWARF:
 case SP_HILL_ORC:
 case SP_OGRE:
 case SP_TROLL:
 case SP_CENTAUR: /* Centaur */
 case SP_MINOTAUR: // minotaur
 case SP_DEMONSPAWN: // demonspawn

 return 1;
}
return 0;

case JOB_RANGER: // ranger
switch(speci)
{
 case SP_HUMAN:
 case SP_ELF:
 case SP_HIGH_ELF:
 case SP_GREY_ELF:
 case SP_SLUDGE_ELF:
 case SP_HILL_ORC:
 case SP_NAGA:
 case SP_RED_DRACONIAN: /* Drac */
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
 case SP_UNK2_DRACONIAN: /* Draconians */
 case SP_CENTAUR: /* Centaur */
 case SP_DEMIGOD: /* Demigod */
 case SP_MINOTAUR: // minotaur
 case SP_DEMONSPAWN: // demonspawn

 case SP_KENKU: // Kenku
 return 1;
}
return 0;

case JOB_CONJURER: // Conjurer
switch(speci)
{
 case SP_KOBOLD:
 case SP_MUMMY:
 case SP_HALFLING:
 case SP_GNOME: // gnome
 case SP_OGRE:
 case SP_TROLL:
 case SP_CENTAUR: /* Centaur */
 case SP_SPRIGGAN: // spriggan
 case SP_MINOTAUR: // minotaur
 case SP_GHOUL: // Ghoul
 return 0;
}
return 1;

case JOB_ENCHANTER: // Enchanter
switch(speci)
{
 case SP_HILL_ORC:
 case SP_KOBOLD:
 case SP_MUMMY:
 case SP_GNOME: // gnome
 case SP_OGRE:
 case SP_TROLL:
 case SP_MINOTAUR: // minotaur
 case SP_GHOUL: // Ghoul
 case SP_KENKU: // Kenku
 return 0;
}
return 1;

case JOB_FIRE_ELEMENTALIST: // Fire elementalist
switch(speci)
{
 case SP_HUMAN:
 case SP_ELF:
 case SP_HIGH_ELF:
 case SP_DEEP_ELF:
 case SP_SLUDGE_ELF:
 case SP_HILL_DWARF:
 case SP_MOUNTAIN_DWARF:
 case SP_HILL_ORC:
 case SP_OGRE_MAGE: /* Ogre-mage */
 case SP_CENTAUR: /* Centaur */
 case SP_DEMIGOD: /* Demigod */
 case SP_DEMONSPAWN: // demonspawn

 case SP_KENKU: // Kenku
 return 1;
}
return 0;

case JOB_ICE_ELEMENTALIST: // Ice elementalist
switch(speci)
{
 case SP_HUMAN:
 case SP_ELF:
 case SP_HIGH_ELF:
 case SP_DEEP_ELF:
 case SP_SLUDGE_ELF:
 case SP_OGRE_MAGE: /* Ogre-mage */
 case SP_CENTAUR: /* Centaur */
 case SP_DEMIGOD: /* Demigod */
 case SP_DEMONSPAWN: // demonspawn

 return 1;
}
return 0;

case JOB_SUMMONER: // Summoner
switch(speci)
{
 case SP_HALFLING:
 case SP_MOUNTAIN_DWARF:
 case SP_HILL_DWARF:
 case SP_MUMMY:
 case SP_GNOME: // gnome
 case SP_OGRE:
 case SP_TROLL:
 case SP_CENTAUR: /* Centaur */
 case SP_SPRIGGAN: // spriggan
 case SP_MINOTAUR: // minotaur
 case SP_GHOUL: // Ghoul
 return 0;
}
return 1;

case JOB_AIR_ELEMENTALIST: // Air elementalist
switch(speci)
{
 case SP_HUMAN:
 case SP_ELF:
 case SP_HIGH_ELF:
 case SP_GREY_ELF:
 case SP_DEEP_ELF:
 case SP_SLUDGE_ELF:
 case SP_OGRE_MAGE: /* Ogre-mage */
 case SP_CENTAUR: /* Centaur */
 case SP_DEMIGOD: /* Demigod */
 case SP_DEMONSPAWN: // demonspawn

 case SP_KENKU: // Kenku
 return 1;
}
return 0;


case JOB_EARTH_ELEMENTALIST: // Earth elementalist
switch(speci)
{
 case SP_HUMAN:
 case SP_DEEP_ELF:
 case SP_SLUDGE_ELF:
 case SP_HILL_DWARF:
 case SP_MOUNTAIN_DWARF:
 case SP_HILL_ORC:
 case SP_GNOME: // gnome
 case SP_OGRE_MAGE: /* Ogre-mage */
 case SP_CENTAUR: /* Centaur */
 case SP_DEMIGOD: /* Demigod */
 case SP_DEMONSPAWN: // demonspawn

 return 1;
}
return 0;

case JOB_CRUSADER: // Crusader
switch(speci)
{
 case SP_HUMAN:
 case SP_ELF:
 case SP_HIGH_ELF:
 case SP_GREY_ELF:
 case SP_DEEP_ELF:
 case SP_SLUDGE_ELF:
 case SP_HILL_ORC:
 case SP_OGRE_MAGE:
 case SP_RED_DRACONIAN: /* Drac */
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
 case SP_UNK2_DRACONIAN: /* Draconians */
 case SP_CENTAUR: /* Centaur */
 case SP_DEMIGOD: /* Demigod */
 case SP_DEMONSPAWN: // demonspawn

 return 1;
}
return 0;

case JOB_DEATH_KNIGHT: // Death knight
switch(speci)
{
 case SP_HUMAN:
 case SP_DEEP_ELF:
 case SP_SLUDGE_ELF:
 case SP_HILL_ORC:
 case SP_MUMMY:
 case SP_NAGA:
 case SP_RED_DRACONIAN: /* Drac */
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
 case SP_UNK2_DRACONIAN: /* Draconians */
 case SP_CENTAUR: /* Centaur */
 case SP_DEMIGOD: /* Demigod */
 case SP_DEMONSPAWN: // demonspawn

 case SP_KENKU: // Kenku
 return 1;
}
return 0;

case JOB_VENOM_MAGE: // tainter
switch(speci)
{
 case SP_HUMAN:
 case SP_DEEP_ELF:
 case SP_SLUDGE_ELF:
 case SP_HILL_ORC:
 case SP_KOBOLD:
 case SP_NAGA:
 case SP_OGRE_MAGE: /* Ogre-mage */
 case SP_RED_DRACONIAN: /* Drac */
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
 case SP_UNK2_DRACONIAN: /* Draconians */
 case SP_DEMIGOD: /* Demigod */
 case SP_SPRIGGAN: /* Spriggan */
 case SP_DEMONSPAWN: // demonspawn

 case SP_KENKU: // Kenku
 return 1;
}
return 0;

case JOB_CHAOS_KNIGHT: // chaos knight
switch(speci)
{
 case SP_HUMAN:
 case SP_ELF:
 case SP_HIGH_ELF:
 case SP_DEEP_ELF:
 case SP_SLUDGE_ELF:
 case SP_HILL_ORC:
 case SP_KOBOLD:
 case SP_NAGA:
 case SP_RED_DRACONIAN: /* Drac */
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
 case SP_UNK2_DRACONIAN: /* Draconians */
 case SP_CENTAUR: /* Centaur */
 case SP_MINOTAUR: // minotaur
 case SP_DEMONSPAWN: // demonspawn

 return 1;
}
return 0;

case JOB_TRANSMUTER: // transmuter
switch(speci)
{
 case SP_HILL_ORC:
 case SP_HALFLING:
 case SP_HILL_DWARF:
 case SP_KOBOLD:
 case SP_MUMMY:
 case SP_GNOME: // gnome
 case SP_OGRE:
 case SP_TROLL:
 case SP_RED_DRACONIAN: /* Drac */
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
 case SP_UNK2_DRACONIAN: /* Draconians */
 case SP_MINOTAUR: // minotaur
 case SP_DEMONSPAWN: // demonspawn

 case SP_GHOUL: // Ghoul
 case SP_KENKU: // Kenku
 return 0;
}
return 1;

case JOB_HEALER: // healer
switch(speci)
{
 case SP_HALFLING:
 case SP_KOBOLD:
 case SP_MUMMY:
 case SP_NAGA:
 case SP_GNOME: // gnome
 case SP_OGRE:
 case SP_TROLL:
 case SP_OGRE_MAGE:
 case SP_RED_DRACONIAN: /* Drac */
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
 case SP_UNK2_DRACONIAN: /* Draconians */
 case SP_DEMIGOD: /* Demigod */
 case SP_SPRIGGAN: // spriggan
 case SP_MINOTAUR: // minotaur
 case SP_DEMONSPAWN: // demonspawn

 case SP_GHOUL: // Ghoul
 case SP_KENKU: // Kenku
 return 0;
}
return 1;

case JOB_REAVER: // Reaver
switch(speci)
{
 case SP_HUMAN:
 case SP_ELF:
 case SP_HIGH_ELF:
 case SP_DEEP_ELF:
 case SP_SLUDGE_ELF:
 case SP_HILL_ORC:
 case SP_KOBOLD:
 case SP_NAGA:
 case SP_RED_DRACONIAN: /* Drac */
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
 case SP_UNK2_DRACONIAN: /* Draconians */
 case SP_CENTAUR: /* Centaur */
 case SP_DEMIGOD: /* Demigod */
 case SP_DEMONSPAWN: // demonspawn

 case SP_KENKU: // Kenku
 return 1;
}
return 0;

case JOB_STALKER: /* stalker */
switch(speci)
{
 case SP_HUMAN:
 case SP_ELF:
 case SP_HIGH_ELF:
 case SP_GREY_ELF:
 case SP_DEEP_ELF:
 case SP_SLUDGE_ELF:
 case SP_HILL_ORC:
 case SP_KOBOLD:
 case SP_NAGA:
 case SP_DEMIGOD: /* Demigod */
 case SP_DEMONSPAWN: // demonspawn

 case SP_KENKU: // Kenku
 return 1;
}
return 0;

case JOB_MONK: // Monk
switch(speci)
{
 case SP_HILL_DWARF:
 case SP_KOBOLD:
 case SP_MUMMY:
 case SP_NAGA:
 case SP_GNOME:
 case SP_OGRE:
 case SP_TROLL:
 case SP_OGRE_MAGE:
 case SP_CENTAUR:
 case SP_SPRIGGAN: // spriggan
 case SP_GHOUL: // Ghoul
 return 0;
}
return 1;


case JOB_WARPER: // warper
switch(speci)
{
 case SP_HILL_ORC:
 case SP_HALFLING:
 case SP_HILL_DWARF:
 case SP_KOBOLD:
 case SP_MUMMY:
 case SP_GNOME: // gnome
 case SP_OGRE:
 case SP_TROLL:
 case SP_RED_DRACONIAN: /* Drac */
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
 case SP_UNK2_DRACONIAN: /* Draconians */
 case SP_MINOTAUR: // minotaur
 case SP_GHOUL: // Ghoul
 case SP_KENKU: // Kenku
 return 0;
}
return 1;


default: return 0;

}

}
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

void choose_weapon(void)
{

char keyin = 0;
/*
switch(you[0].clas)
{
 case 0: / * fighter * /
 case 6:
 case 17:
 case 20:
 case 18: / * others * /
 weap_type [0] = 5;
 weap_type [1] = 1;
 weap_type [2] = 9;
 weap_type [3] = 11;
 weap_type [4] = 11;
 break;
 case 4: / * gladiator * /
 weap_type [0] = 5;
 weap_type [1] = 2;
 weap_type [2] = 4;
 weap_type [3] = 9;
 weap_type [4] = 11;
 break;
 default: return;
}*/

if (you[0].clas == JOB_CHAOS_KNIGHT)
{
 switch(random2(5))
 {
  case 0: you[0].inv_type [0] = WPN_SHORT_SWORD; return;
  case 1: you[0].inv_type [0] = WPN_FLAIL; return;
  case 2: you[0].inv_type [0] = WPN_MORNINGSTAR; return;
  case 3: you[0].inv_type [0] = WPN_HAND_AXE; return;
  case 4: you[0].inv_type [0] = WPN_SPEAR; return;
 }
 return;
}

clrscr();
cprintf(EOL" You have a choice of weapons:"EOL);
if (you[0].clas == JOB_GLADIATOR) goto glad_thing;
cprintf("a - short sword"EOL);
cprintf("b - mace"EOL);
cprintf("c - hand axe"EOL);
cprintf("d - spear"EOL);

getkey : keyin = get_ch();
switch(keyin)
{
 case 'a': you[0].inv_type [0] = WPN_SHORT_SWORD; return;
 case 'b': you[0].inv_type [0] = WPN_MACE; return;
 case 'c': you[0].inv_type [0] = WPN_HAND_AXE; return;
 case 'd': you[0].inv_type [0] = WPN_SPEAR; return;
 default: goto getkey;
}

glad_thing : cprintf("a - short sword"EOL);
cprintf("b - flail"EOL);
cprintf("c - morningstar"EOL);
cprintf("d - hand axe"EOL);
cprintf("e - spear"EOL);

getkey2 : keyin = get_ch();
switch(keyin)
{
 case 'a': you[0].inv_type [0] = WPN_SHORT_SWORD; return;
 case 'b': you[0].inv_type [0] = WPN_FLAIL; return;
 case 'c': you[0].inv_type [0] = WPN_MORNINGSTAR; return;
 case 'd': you[0].inv_type [0] = WPN_HAND_AXE; return;
 case 'e': you[0].inv_type [0] = WPN_SPEAR; return;
 default: goto getkey2;
}

}


void init_player(void)
{

you[0].deaths_door = 0;
you[0].special_wield = 0;
you[0].berserker = 0;
you[0].corpse_count = 0;
you[0].disease = 0;

you[0].level_type  = 0;
// 0 = normal dungeon level
// 1 = labyrinth

you[0].max_level = 1;
you[0].x_pos = 0;
you[0].y_pos = 0;
you[0].invis  = 0;
you[0].conf = 0;
you[0].paralysis = 0;
you[0].slow  = 0;
you[0].haste  = 0; // haste
you[0].might  = 0; // might
you[0].lev  = 0; // levitation
you[0].poison  = 0; // poison!!
you[0].rotting  = 0;
you[0].shock_shield = 0 ;
you[0].hp = 0;
you[0].hp_max = 0;
you[0].ep = 0;
you[0].ep_max = 0;
you[0].strength = 0;
you[0].intel = 0;
you[0].dex = 0;
you[0].max_strength = 0;
you[0].max_intel = 0;
you[0].max_dex = 0;
you[0].xp = 0;
you[0].xl = 1;
you[0].gp = 0 ;
you[0].clas = JOB_FIGHTER;
you[0].speed = 10; // 0.75;
        you[0].burden = 0; // total weight of items carried.
 you[0].burden_state  = 0;
        you[0].inv_no = 0; // number of items carried.
you[0].spell_no = 0;
you[0].spell_levels = 0;
you[0].char_direction = 0;
// 0 = going down
// 1 = going up!
you[0].where_are_you  = 0;
// 0 = normal dungeon level
// 1 = dis
// 2 = hell
you[0].pet_target = 201;
you[0].your_level = 1;
you[0].delay_t = 0;
you[0].delay_doing = 0;
you[0].running = 0;
you[0].run_x = 0;
you[0].run_y = 0;
you[0].level_type = 0;
you[0].hung_state = 3;

int i = 0;

for (i = 0; i < 10; i ++)
{
 you[0].equip [i] = -1;
}

you[0].religion = GOD_NO_GOD;
you[0].piety = 0;

for (i = 0; i < 20; i ++)
{
 you[0].gods [i] = 0;
}

int ic = 0;

strcpy(ghost.gname, "");
for (ic = 0; ic < 20; ic ++)
{
 ghost.ghs [ic] = 0;
}

for (ic = 0; ic < 100; ic ++)
{
 you[0].mutation [ic] = 0;
}

for (ic = 0; ic < 100; ic ++)
{
 you[0].demon_pow [ic] = 0;
}

for (ic = 0; ic < 50; ic ++)
{
 you[0].had_item [ic] = 0;
}

for (ic = 0; ic < NO_UNRANDARTS; ic ++)
{
 set_unrandart_exist(ic, 0);
}


}

#include "config.h"

#ifdef DOS
#include <conio.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#ifdef DOS
#include <file.h>
#endif
#ifdef LINUX
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#endif

#include "externs.h"
#include "itemname.h"
#include "player.h"
#include "skills2.h"
#include "stuff.h"

char class_allowed(char speci, char clas);
void init_player(void);
void choose_weapon(void);

extern char wield_change;


char new_game(void)
{
int handle;
char your_nam [50];
int p;
char keyn;
char weap_skill = 0;

char char_fil [50];

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
you[0].equip [0] = -1;
/*you[0].rate_regen = 6;*/
you[0].xl = 1;
you[0].speed = 10;
you[0].pet_target = 201;
you[0].your_level = 1;
you[0].ep_incr_regen = 0;


char j = 0;
char char_name [30];

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

cprintf("\n\rHello, and welcome to Dungeon Crawl v2.82!");
cprintf("\n\r(Copyright 1997 Linley Henzell)");
cprintf("\n\rPlease read Crawl.txt for instructions and legal details.\n\r\n\r");
name_q : cprintf("What is your name today? ");
//cin >> your_nam;
#ifdef DOS
gets(your_nam);
#endif

#ifdef LINUX
echo();
getstr(your_nam);
noecho();
#endif


if (strcmp(your_nam, "bones") == 0 | strlen(your_nam) == 0) /* this would cause big probs with ghosts */
{
        cprintf("\n\rThat's a silly name!\n\r");
        goto name_q;
}

unsigned int glorpo = 0;

for (glorpo = 0; glorpo < strlen(your_nam); glorpo ++)
{
 if (your_nam [glorpo] == ' ')
 {
  cprintf("\n\rNo spaces, please.\n\r");
  goto name_q;
 }
}


strcpy(you[0].your_name, your_nam);

strncpy(char_name, your_nam, 6);

strcpy(char_fil, "");
strncat(char_fil, you[0].your_name, 6);
strcat(char_fil, ".sav");



 int passout = 0;


handle = open(char_fil, S_IWRITE, S_IREAD);


if (handle != -1)
{
        cprintf("\n\rWelcome back, ");
        cprintf(your_nam);
        cprintf("!");
        close(handle);

        return 0;
}
close(handle);



cprintf("\n\r\n");
cprintf("You must be new here!\n\r");
/*cprintf("You can be a Human, an elf, a High (E)lf, a grey elf, a deep elf,\n\r");
cprintf("a sludge elf, a hill (D)warf, a mountain dwarf, a halfling, a hill (o)rc,\n\r");
cprintf("a kobold, a Mummy, a Naga or a Gnome.\n\r");*/
spec_query2 : cprintf("You can be:\n\r");
cprintf("a - Human\n\r");
cprintf("b - Elf\n\r");
cprintf("c - High Elf\n\r");
cprintf("d - Grey Elf\n\r");
cprintf("e - Deep Elf\n\r");
cprintf("f - Sludge Elf\n\r");
cprintf("g - Hill Dwarf\n\r");
cprintf("h - Mountain Dwarf\n\r");
cprintf("i - Halfling\n\r");
cprintf("j - Hill Orc\n\r");
cprintf("k - Kobold\n\r");
cprintf("l - Mummy\n\r");
cprintf("m - Naga\n\r");
cprintf("n - Gnome\n\r");
cprintf("o - Ogre\n\r");
cprintf("p - Troll\n\r");
cprintf("q - Ogre-Mage\n\r");
cprintf("r - Draconian\n\r");
cprintf("? - Random\n\r");
cprintf("X - Quit\n\r");

cprintf("\n\rWhich one? ");
spec_query : keyn = getch();
if (keyn == 0)
{
 getch();
 goto spec_query;
}
switch_start : switch(keyn)
{
 case 'a': you[0].species = 1; // human
 you[0].strength = 6;
 you[0].intel = 6;
 you[0].dex = 6;
 break;
 case 'b': you[0].species = 2; // elf
 you[0].strength = 5;
 you[0].intel = 8;
 you[0].dex = 8;
 break;
 case 'c': you[0].species = 3; // high-elf
 you[0].strength = 5;
 you[0].intel = 9;
 you[0].dex = 8;
 break;
 case 'd': you[0].species = 4; // grey-elf
 you[0].strength = 4;
 you[0].intel = 9;
 you[0].dex = 8;
 break;
 case 'e': you[0].species = 5; // deep elf
 you[0].strength = 4;
 you[0].intel = 10;
 you[0].dex = 7;
 break;
 case 'f': you[0].species = 6; // sludge elf
 you[0].strength = 5;
 you[0].intel = 7;
 you[0].dex = 8;
 break;
 case 'g': you[0].species = 7; // hill dwarf
 you[0].strength = 9;
 you[0].intel = 3;
 you[0].dex = 4;
 break;
 case 'h': you[0].species = 8; // mountain dwarf
 you[0].strength = 9;
 you[0].intel = 4;
 you[0].dex = 5;
 break;
 case 'i': you[0].species = 9; // halfling
 you[0].strength = 4;
 you[0].intel = 6;
 you[0].dex = 9;
 break;
 case 'j':
 case 'J': you[0].species = 10; // hill orc
 you[0].strength = 9;
 you[0].intel = 3;
 you[0].dex = 4;
 break;
 case 'K':
 case 'k': you[0].species = 11; // kobold
 you[0].strength = 4;
 you[0].intel = 4;
 you[0].dex = 6;
 break;
 case 'l': you[0].species = 12; // Mummy
 you[0].strength = 7;
 you[0].intel = 3;
 you[0].dex = 3;
 break;
 case 'm':
 case 'M': you[0].species = 13; // Naga
 you[0].strength = 8;
 you[0].intel = 6;
 you[0].dex = 4;
 break;
 case 'n':
 you[0].species = 14; // gnome
 you[0].strength = 5;
 you[0].intel = 6;
 you[0].dex = 7;
 break;
 case 'o':
 case 'O':
 you[0].species = 15; // ogre
 you[0].strength = 12;
 you[0].intel = 3;
 you[0].dex = 1;
 break;
 case 'p':
 case 'P':
 you[0].species = 16; // troll
 you[0].strength = 13;
 you[0].intel = 3;
 you[0].dex = 0;
 break;
 case 'Q':
 case 'q':
 you[0].species = 17; // ogre mage
 you[0].strength = 8;
 you[0].intel = 6;
 you[0].dex = 2;
 break;
 case 'r':
 case 'R': you[0].species = 18 + random2(7); // Draconian
 you[0].strength = 7;
 you[0].intel = 6;
 you[0].dex = 2;
 break;
 case '?':
 keyn = 97 + random2(25);
 goto switch_start;
 case 'X':
 cprintf("\n\rGoodbye!");
 end(0);
 break;
 default:
// cprintf("\n\rDon't be silly. ");
 goto spec_query;
}

clrscr(); // Otherwise it looks ugly under Win NT, or so I'm told

cprintf("\n\r\n\r");
cprintf("Welcome, ");
cprintf(you[0].your_name);
cprintf(" the ");
cprintf(species_name(you[0].species));
cprintf(".\n\r\n\r");

cprintf("You can be any of the following :\n\r");
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

for (i = 0; i < 23; i ++)
{
 if (class_allowed(you[0].species, i) == 0) continue;

 putch(i + 97);
 cprintf(" - ");

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
  case 19: cprintf("Venom Mage"); break;
  case 20: cprintf("Chaos Knight"); break;
  case 21: cprintf("transmuter"); break;
  case 22: cprintf("Healer"); break;
 }
 if (j % 2 == 1) cprintf("\n\r"); else gotoxy(40, wherey());
 j ++;

}
if (wherex() >= 40) cprintf("\n\r");
cprintf("? - Random; x - Back to species selection; X - Quit\n\r");
//cprintf("x - Back to species selection\n\r");
//cprintf("X - Quit\n\r");


cprintf("\n\rWhat kind of character are you? ");
query : keyn = getch();


query5: if (keyn == 'a') you[0].clas = 0;
        else if (keyn == 'b') you[0].clas = 1;
  else if (keyn == 'c') you[0].clas = 2;
  else if (keyn == 'd') you[0].clas = 3;
  else if (keyn == 'e') you[0].clas = 4;
  else if (keyn == 'f') you[0].clas = 5;
  else if (keyn == 'g') you[0].clas = 6;
  else if (keyn == 'h') you[0].clas = 7;
  else if (keyn == 'i') you[0].clas = 8;
  else if (keyn == 'j') you[0].clas = 9;
  else if (keyn == 'k') you[0].clas = 10;
  else if (keyn == 'l') you[0].clas = 11;
  else if (keyn == 'm') you[0].clas = 12;
  else if (keyn == 'n') you[0].clas = 13;
  else if (keyn == 'o') you[0].clas = 14;
  else if (keyn == 'p') you[0].clas = 15;
  else if (keyn == 'q') you[0].clas = 16;
  else if (keyn == 'r') you[0].clas = 17;
  else if (keyn == 's') you[0].clas = 18;
  else if (keyn == 't') you[0].clas = 19;
  else if (keyn == 'u') you[0].clas = 20;
  else if (keyn == 'v') you[0].clas = 21;
  else if (keyn == 'w') you[0].clas = 22;
  else if (keyn == '?')
  {
   do
   {
    keyn = 97 + random2(20);
   } while (class_allowed(you[0].species, keyn - 97) == 0);
   goto query5;
  }
  else if (keyn == 'x')
  {
   clrscr();
   cprintf("\n\r\n\r");
   goto spec_query2;
  }
  else if (keyn == 'X')
   {
    cprintf("\n\rGoodbye!");
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
                you [0].inv_class [i] = 0;
                you [0].inv_type [i] = 0;
                you [0].inv_plus [i] = 0;
                you [0].inv_plus2 [i] = 0;
                you [0].inv_dam [i] = 0;
                you [0].inv_col [i] = 0;
        }

for (i = 0; i < 10; i ++)
{
 you[0].attribute [i] = 0;
}


switch(you[0].clas)
{
case 0: // fighter
        strcpy(you[0].clasnam, "fighter");
        you[0].hp = 14; you[0].hp_max = 14;
        you[0].ep = 0; you[0].ep_max = 0;
/*      you[0].f_abil = 10;
        you[0].mag_abil = 5;
 you[0].thr_abil = 10;
 you[0].speed = 10;*/
 you[0].spell_levels = 0;

        you[0].inv_quant [0] = 1;
        you[0].inv_class [0] = 0;
                you[0].inv_type [0] = 5;

  if (you[0].species == 11)
  {
   you[0].inv_type [0] = 3; // kobolds just get daggers
   you[0].skills [1] = 2;
  } else
  if (you[0].species == 15)
  {
   you[0].inv_type [0] = 0; // ogre
   you[0].skills [5] = 1;
  } else
  if (you[0].species == 16)
  {
//   you[0].inv_type [0] = 0; // troll
//   you[0].skills [0] ++;
  }
/*    else
  if (random2(4) == 0 | you[0].species == 7 | you[0].species == 8) // dwarves
  {
   you[0].inv_type [0] = 9;
   you[0].skills [4] = 2;
  } else
  if (random2(4) == 0)
  {
   you[0].inv_type [0] = 1;
   you[0].skills [5] = 2;
  }
   else you[0].skills [1] = 2;*/
        weap_skill = 2;


        you[0].inv_plus [0] = 50;
        you[0].inv_plus2 [0] = 50;
        you[0].inv_dam [0] = 0;
        you[0].inv_col [0] = LIGHTCYAN;


if (you[0].species == 15)
{
        you[0].inv_quant [0] = 1;
        you[0].inv_class [0] = 0;
        you[0].inv_type [0] = 20;
        you[0].inv_plus [0] = 50;
        you[0].inv_dam [0] = 0;
        you[0].inv_col [0] = BROWN;

}

if (you[0].species == 16)
{
        you[0].inv_quant [0] = 0;
        you[0].inv_class [0] = 0;
        you[0].inv_type [0] = 0;
        you[0].inv_plus [0] = 50;
        you[0].inv_dam [0] = 0;
        you[0].inv_col [0] = BROWN;

}

if (you[0].species == 15 | you[0].species == 16 | (you[0].species >= 18 && you[0].species <= 29))
{
        you[0].inv_quant [1] = 1;
        you[0].inv_class [1] = 2;
        you[0].inv_type [1] = 30;
        you[0].inv_plus [1] = 50;
        you[0].inv_dam [1] = 0;
        you[0].inv_col [1] = BROWN;

 if (you[0].species >= 18 && you[0].species <= 29)
 {
        you[0].inv_quant [2] = 1;
        you[0].inv_class [2] = 2;
        you[0].inv_type [2] = 8;
        you[0].inv_plus [2] = 50;
        you[0].inv_dam [2] = 0;
        you[0].inv_col [2] = LIGHTCYAN;
 }


}
else
if (you[0].species == 11)
{
        you[0].inv_quant [1] = 1;
        you[0].inv_class [1] = 2;
        you[0].inv_type [1] = 1;
        you[0].inv_plus [1] = 50;
        you[0].inv_dam [1] = 0;
        you[0].inv_col [1] = BROWN;
/*      you[0].AC = 2;
        you[0].evasion = 9;*/

  you[0].inv_quant [2] = random2(8) + random2(8) + 8;
                you[0].inv_class [2] = 1;
                you[0].inv_type [2] = 3; //wtype;
                you[0].inv_plus [2] = 50;
                you[0].inv_dam [2] = 0;
                you[0].inv_col [2] = LIGHTCYAN;

} else
{
        you[0].inv_quant [1] = 1;
        you[0].inv_class [1] = 2;
        you[0].inv_type [1] = 2;
        you[0].inv_plus [1] = 50;
        you[0].inv_dam [1] = 0;
        you[0].inv_col [1] = LIGHTCYAN;

        you[0].inv_quant [2] = 1;
        you[0].inv_class [2] = 2;
        you[0].inv_type [2] = 8;
        you[0].inv_plus [2] = 50;
        you[0].inv_dam [2] = 0;
        you[0].inv_col [2] = LIGHTCYAN;

/*      you[0].AC = 3;
        you[0].evasion = 9;*/
        choose_weapon();
}

        // 11 = spear
        // 9 = hand ae
        // 3 = dagger

        you[0].strength += 6;
        you[0].dex += 4;

        if (you[0].species != 16) you[0].equip [0] = 0;
        you[0].equip [6] = 1;
if (you[0].species != 11 && you[0].species != 15 && you[0].species != 16) you[0].equip [5] = 2;
        you[0].gp = random2(10);
/* you[0].res_magic = 3;*/


 you[0].skills [0] = 3;
if (you[0].species == 11)
{
 you[0].skills [12] = 2;
 you[0].skills [14] = 1;
 you[0].skills [15] = 1;
 you[0].skills [16] = 1;
 you[0].skills [14 + random() % 3] ++;
} else
if (you[0].species != 15 && you[0].species != 16)
{
 if (you[0].species >= 18 && you[0].species <= 29) you[0].skills [14] = 2;
  else you[0].skills [13] = 2;
 you[0].skills [17] = 1;
 you[0].skills [16 + random() % 2] ++;
 you[0].skills [12] = 2;
} else you[0].skills [0] += 2;
break;

case 1: // wizard
        strcpy(you[0].clasnam, "Wizard");
        you[0].hp = 10; you[0].hp_max = 10;
        you[0].ep = 3; you[0].ep_max = 3;
/*      you[0].f_abil = 5;
        you[0].mag_abil = 10;
 you[0].thr_abil = 5;
 you[0].speed = 10;*/
 you[0].spell_levels = 12;

        you[0].inv_quant [0] = 1;
        you[0].inv_class [0] = 0;
                you[0].inv_type [0] = 3; //damage = 6; //break;

        you[0].inv_plus [0] = 50;
        you[0].inv_plus2 [0] = 50;
        you[0].inv_dam [0] = 0;
        you[0].inv_col [0] = LIGHTCYAN;

        // Robe
        you[0].inv_quant [1] = 1;
        you[0].inv_class [1] = 2;
        you[0].inv_type [1] = 0;
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

        you[0].equip [0] = 0;
        you[0].equip [6] = 1;

        you[0].gp = random2(10);

        // etra items being tested:


        you[0].inv_class [2] = 10;
        you[0].inv_type [2] = random2(3);
//      strcpy(you[0].inv_name [23], "wand of invisibility");
        you[0].inv_quant [2] = 1;
        you[0].inv_plus [2] = 127;
        you[0].inv_dam [2] = 1;
        // you[0].inv_no = 13;
        you[0].inv_col [2] = 3;

/* you[0].res_magic = 10;*/

        you[0].skills [14] = 1;
        you[0].skills [15] = 1;
        you[0].skills [14 + random() % 2] ++;

/*        if (you[0].skills [14] == 2) you[0].evasion ++;*/

        you[0].skills [25] = 2;
        you[0].skills [26] = 1;
        you[0].skills [27] = 1;
        you[0].skills [25 + random() % 3] ++;
        you[0].skills [28 + random() % 5] ++;

        you[0].skills [1] = 1;
        you[0].skills [7] = 1;
break;


case 2: // priest
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
        you[0].inv_class [0] = 0;
        //switch(random2(6))
        //{
        //      case 0 : you[0].inv_type [0] = 1; damage = 6; break;
        //      case 1 : you[0].inv_type [0] = 2; damage = 7; break;
        //      case 2 : you[0].inv_type [0] = 4; damage = 7; break;
        //      case 3 : you[0].inv_type [0] = 9; damage = 7; break;
                //case 4 :
 you[0].inv_type [0] = 1;
                //case 5 : you[0].inv_type [0] = 5; damage = 6; break;
        //}


        you[0].inv_plus [0] = 50;
        you[0].inv_plus2 [0] = 50;
        you[0].inv_dam [0] = 0;
        you[0].inv_col [0] = LIGHTCYAN;


        // Robe
        you[0].inv_quant [1] = 1;
        you[0].inv_class [1] = 2;
        you[0].inv_type [1] = 0;
        you[0].inv_plus [1] = 50;
        you[0].inv_dam [1] = 0;
        you[0].inv_col [1] = WHITE;

/*      you[0].AC = 1;
        you[0].evasion = 10;*/

        you[0].inv_class [2] = 8;
        you[0].inv_type [2] = 0;
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

        you[0].equip [0] = 0;
        you[0].equip [6] = 1;

        you[0].gp = random2(10);
/* you[0].res_magic = 10;*/

 you[0].skills [0] = 2;
 you[0].skills [14] = 1;
 you[0].skills [17] = 1;
 you[0].skills [12] = 2;
 you[0].skills [5] = 2;
 you[0].skills [7] = 1;

 you[0].skills [25] = 2;

 you[0].religion = 1;

break;



case 3: // thief
        strcpy(you[0].clasnam, "Thief");
        you[0].hp = 11; you[0].hp_max = 11;
        you[0].ep = 0; you[0].ep_max = 0;
/*      you[0].f_abil = 7;
        you[0].mag_abil = 6;
 you[0].thr_abil = 12;
 you[0].speed = 10;*/
 you[0].spell_levels = 0;

        you[0].inv_quant [0] = 1;
        you[0].inv_class [0] = 0;
                you[0].inv_type [0] = 3;// damage = 6; //break;

        you[0].inv_plus [0] = 50;
        you[0].inv_plus2 [0] = 50;
        you[0].inv_dam [0] = 0;
        you[0].inv_col [0] = LIGHTCYAN;


  you[0].inv_quant [1] = random2(10) + random2(10) + 10;
                you[0].inv_class [1] = 1;
                you[0].inv_type [1] = 3; //wtype;
                you[0].inv_plus [1] = 50;
                you[0].inv_dam [1] = 0;
                you[0].inv_col [1] = LIGHTCYAN;

        // Leather armour
        you[0].inv_quant [2] = 1;
        you[0].inv_class [2] = 2;
        you[0].inv_type [2] = 1;
        you[0].inv_plus [2] = 50;
        you[0].inv_dam [2] = 0;
        you[0].inv_col [2] = BROWN;

        you[0].inv_quant [3] = 1;
        you[0].inv_class [3] = 2;
        you[0].inv_type [3] = 9;
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

        you[0].equip [0] = 0;
        you[0].equip [6] = 2;
        you[0].equip [1] = 3;

        you[0].gp = random2(10);
/* you[0].res_magic = 3;*/

 you[0].skills [0] = 1;
 you[0].skills [1] = 2;

 you[0].skills [14] = 1;
 you[0].skills [15] = 1;
 you[0].skills [16] = 1;
 you[0].skills [14 + random() % 3] ++;

/* if (you[0].skills [14] == 2) you[0].evasion ++;*/

 you[0].skills [12] = 1;
 you[0].skills [11] = 1;

 you[0].skills [18] = 2;

break;

case 4: // Gladiator
        strcpy(you[0].clasnam, "Gladiator");
        you[0].hp = 15; you[0].hp_max = 15;
        you[0].ep = 0; you[0].ep_max = 0;
/*      you[0].f_abil = 11;
        you[0].mag_abil = 3;
 you[0].thr_abil = 3;
 you[0].speed = 10;*/
 you[0].spell_levels = 0;

        you[0].inv_quant [0] = 1;
        you[0].inv_class [0] = 0;
                you[0].inv_type [0] = 5;
/*  if (random2(3) == 0)
  {
   you[0].inv_type [0] = 2;
   you[0].skills [5] = 3;
  } else
  if (random2(3) == 0)
  {
   you[0].inv_type [0] = 4;
   you[0].skills [5] = 3;
  }
   else you[0].skills [1] = 3;*/
        choose_weapon();
        weap_skill = 3;



        you[0].inv_plus [0] = 50;
        you[0].inv_plus2 [0] = 50;
        you[0].inv_dam [0] = 0;
        you[0].inv_col [0] = LIGHTCYAN;
        if (you[0].species >= 18 && you[0].species <= 29)
        {
         you[0].inv_quant [1] = 1;
         you[0].inv_class [1] = 2;
         you[0].inv_type [1] = 30;
         you[0].inv_plus [1] = 50;
         you[0].inv_dam [1] = 0;
         you[0].inv_col [1] = BROWN;

         you[0].inv_quant [2] = 1;
         you[0].inv_class [2] = 2;
         you[0].inv_type [2] = 8;
         you[0].inv_plus [2] = 50;
         you[0].inv_dam [2] = 0;
         you[0].inv_col [2] = LIGHTCYAN;
        }
        else
        {
         you[0].inv_quant [1] = 1;
         you[0].inv_class [1] = 2;
         you[0].inv_type [1] = 3;
         you[0].inv_plus [1] = 50;
         you[0].inv_dam [1] = 0;
         you[0].inv_col [1] = LIGHTCYAN;

         you[0].inv_quant [2] = 1;
         you[0].inv_class [2] = 2;
         you[0].inv_type [2] = 13;
         you[0].inv_plus [2] = 50;
         you[0].inv_dam [2] = 0;
         you[0].inv_col [2] = LIGHTCYAN;
        }
/*      you[0].AC = 4;
        you[0].evasion = 9;*/
        you[0].strength += 7;
        you[0].dex += 3;
        you[0].equip [0] = 0;
        you[0].equip [6] = 1;
        you[0].equip [5] = 2;
        you[0].gp = random2(10);
/* you[0].res_magic = 3;*/

 you[0].skills [0] = 3;
if (you[0].species >= 18 && you[0].species <= 29) you[0].skills [14] = 3;
 else you[0].skills [13] = 3;
/* you[0].evasion ++;*/
 you[0].skills [17] = 2;

break;


case 5: // Necromancer
        strcpy(you[0].clasnam, "Necromancer");
        you[0].hp = 10; you[0].hp_max = 10;
        you[0].ep = 3; you[0].ep_max = 3;
/*      you[0].f_abil = 5;
        you[0].mag_abil = 7;
 you[0].thr_abil = 5;
 you[0].speed = 10;*/
 you[0].spell_levels = 4;
        you[0].inv_quant [0] = 1;
        you[0].inv_class [0] = 0;
                you[0].inv_type [0] = 3;
        you[0].inv_plus [0] = 50;
        you[0].inv_plus2 [0] = 50;
        you[0].inv_dam [0] = 0;
        you[0].inv_col [0] = LIGHTCYAN;
        you[0].inv_quant [1] = 1;
        you[0].inv_class [1] = 2;
        you[0].inv_type [1] = 0;
        you[0].inv_plus [1] = 50;
        you[0].inv_dam [1] = 0;
        you[0].inv_col [1] = DARKGREY;
/*      you[0].AC = 1;
        you[0].evasion = 10;*/
        you[0].dex += 4;
        you[0].intel += 6;
        you[0].equip [0] = 0;
        you[0].equip [6] = 1;
        you[0].gp = random2(10);
        you[0].inv_class [2] = 10;

        you[0].inv_type [2] = 23;
        you[0].inv_quant [2] = 1;
        you[0].inv_plus [2] = 127;
        you[0].inv_dam [2] = 0;//1;
        you[0].inv_col [2] = DARKGREY;
/* you[0].res_magic = 10;*/

        you[0].skills [14] = 1;
        you[0].skills [15] = 1;
        you[0].skills [14 + random() % 2] ++;
/* if (you[0].skills [14] == 2) you[0].evasion ++;*/

        you[0].skills [25] = 1;
        you[0].skills [29] = 4;
        you[0].skills [1] = 1;
        you[0].skills [7] = 1;

break;



case 6: // paladin
        strcpy(you[0].clasnam, "Paladin");
        you[0].religion = 2;
        you[0].piety = 28;
        you[0].hp = 14; you[0].hp_max = 14;
        you[0].ep = 0; you[0].ep_max = 0;
/*      you[0].f_abil = 9;
        you[0].mag_abil = 6;
 you[0].thr_abil = 4;
 you[0].speed = 10;*/
 you[0].spell_levels = 4;
        you[0].inv_quant [0] = 1;
        you[0].inv_class [0] = 0;
        you[0].inv_type [0] = 5;
        you[0].inv_plus [0] = 50;
        you[0].inv_plus2 [0] = 50;
        you[0].inv_dam [0] = 0;
        you[0].inv_col [0] = LIGHTCYAN;
        you[0].inv_quant [1] = 1;
        you[0].inv_class [1] = 2;
        you[0].inv_type [1] = 0;
        you[0].inv_plus [1] = 50;
        you[0].inv_dam [1] = 0;
        you[0].inv_col [1] = WHITE;
        you[0].inv_quant [2] = 1;
        you[0].inv_class [2] = 2;
        you[0].inv_type [2] = 8;
        you[0].inv_plus [2] = 50;
        you[0].inv_dam [2] = 0;
        you[0].inv_col [2] = LIGHTCYAN;
/*      you[0].AC = 1;
        you[0].evasion = 10;*/
        you[0].strength += 6;
        you[0].dex += 2;
        you[0].intel += 2;
        you[0].equip [0] = 0;
        you[0].equip [6] = 1;
        you[0].gp = random2(10);
 you[0].equip [5] = 2;

        you[0].inv_class [3] = 8;
        you[0].inv_type [3] = 0;
        you[0].inv_quant [3] = 1;
        you[0].inv_plus [3] = 0;
        you[0].inv_dam [3] = 0;
        you[0].inv_col [3] = random2(15) + 1;

 you[0].skills [0] = 2;
 you[0].skills [13] = 1;
 you[0].skills [14] = 1;
 you[0].skills [13 + random() % 2] ++;
/*  if (you[0].skills [14] == 2) you[0].evasion ++;*/

 you[0].skills [17] = 1;
 you[0].skills [1] = 2;
 you[0].skills [2] = 1;
 you[0].skills [3] = 1;
 you[0].skills [25] = 1;
break;

case 7: // assassin
        strcpy(you[0].clasnam, "assassin");
        you[0].hp = 12; you[0].hp_max = 12;
        you[0].ep = 0; you[0].ep_max = 0;
/*      you[0].f_abil = 9;
        you[0].mag_abil = 6;
 you[0].thr_abil = 12;
 you[0].speed = 10;*/
        you[0].inv_quant [0] = 1;
        you[0].inv_class [0] = 0;
        you[0].inv_type [0] = 3;
        you[0].inv_plus [0] = 50;
        you[0].inv_plus2 [0] = 50;
        you[0].inv_dam [0] = 0;
        you[0].inv_col [0] = LIGHTCYAN;
        you[0].inv_quant [1] = random2(10) + random2(10) + 10;
                you[0].inv_class [1] = 1;
                you[0].inv_type [1] = 3; //wtype;
                you[0].inv_plus [1] = 50;
                you[0].inv_dam [1] = 0;
                you[0].inv_col [1] = LIGHTCYAN;
        you[0].inv_quant [2] = 1;
        you[0].inv_class [2] = 2;
        you[0].inv_type [2] = 0;
        you[0].inv_plus [2] = 50;
        you[0].inv_dam [2] = 0;
        you[0].inv_col [2] = DARKGREY;

        you[0].inv_quant [3] = 1;
        you[0].inv_class [3] = 2;
        you[0].inv_type [3] = 9;
        you[0].inv_plus [3] = 50;
        you[0].inv_dam [3] = 0;
        you[0].inv_col [3] = DARKGREY;

/* you[0].AC = 2;
        you[0].evasion = 10;*/
        you[0].strength += 3;
        you[0].dex += 5;
        you[0].intel += 2;
        you[0].equip [0] = 0;
        you[0].equip [6] = 2;
        you[0].equip [1] = 3;
        you[0].gp = random2(10);
/* you[0].res_magic = 3;*/

 you[0].skills [0] = 2;
 you[0].skills [1] = 2;

 you[0].skills [14] = 1;
 you[0].skills [15] = 2;
 you[0].skills [16] = 1;
 you[0].skills [14 + random() % 3] ++;

/* if (you[0].skills [14] == 2) you[0].evasion ++;*/

 you[0].skills [12] = 1;
 you[0].skills [11] = 1;

break;

case 8: // Barbarian
        strcpy(you[0].clasnam, "Berserker");
        you[0].religion = 10;
        you[0].piety = 35;
        you[0].hp = 15; you[0].hp_max = 15;
        you[0].ep = 0; you[0].ep_max = 0;
/*      you[0].f_abil = 10;
        you[0].mag_abil = 2;
 you[0].thr_abil = 10;
 you[0].speed = 10;*/
 you[0].spell_levels = 0;

        you[0].inv_quant [0] = 1;
        you[0].inv_class [0] = 0;
                you[0].inv_type [0] = 9;
        you[0].inv_plus [0] = 50;
        you[0].inv_plus2 [0] = 50;
        you[0].inv_dam [0] = 0;
        you[0].inv_col [0] = LIGHTCYAN;

        you[0].inv_quant [1] = 1;
        you[0].inv_class [1] = 0;
        you[0].inv_type [1] = 11;
        you[0].inv_plus [1] = 50;
        you[0].inv_plus2 [1] = 50;
        you[0].inv_dam [1] = 0;
        you[0].inv_col [1] = LIGHTCYAN;

        you[0].inv_quant [2] = 1;
        you[0].inv_class [2] = 0;
        you[0].inv_type [2] = 11;
        you[0].inv_plus [2] = 50;
        you[0].inv_plus2 [2] = 50;
        you[0].inv_dam [2] = 0;
        you[0].inv_col [2] = LIGHTCYAN;

        you[0].inv_quant [3] = 1;
        you[0].inv_class [3] = 0;
        you[0].inv_type [3] = 11;
        you[0].inv_plus [3] = 50;
        you[0].inv_plus2 [3] = 50;
        you[0].inv_dam [3] = 0;
        you[0].inv_col [3] = LIGHTCYAN;


        you[0].inv_quant [4] = 1;
        you[0].inv_class [4] = 2;
        you[0].inv_type [4] = 1;
        you[0].inv_plus [4] = 50;
        you[0].inv_dam [4] = 0;
        you[0].inv_col [4] = BROWN;

/*      you[0].AC = 2;
        you[0].evasion = 9;*/
        you[0].strength += 7;
        you[0].dex += 4;
        you[0].intel -= 1;
        you[0].equip [0] = 0;
        you[0].equip [6] = 4;
        you[0].gp = random2(10);
/* you[0].res_magic = 5;*/

 you[0].skills [0] = 2;
 you[0].skills [13] = 2;
 you[0].skills [14] = 2;
 you[0].skills [12] = 2;
/* you[0].evasion ++;*/
 you[0].skills [5] = 2;
 you[0].skills [6] = 2;
break;

case 9: // Ranger
        strcpy(you[0].clasnam, "Ranger");
        you[0].hp = 13; you[0].hp_max = 13;
        you[0].ep = 0; you[0].ep_max = 0;
/*      you[0].f_abil = 9;
        you[0].mag_abil = 6;
 you[0].thr_abil = 11;
 you[0].speed = 10;*/
 you[0].spell_levels = 4;

        you[0].inv_quant [0] = 1;
        you[0].inv_class [0] = 0;
                you[0].inv_type [0] = 3;
        you[0].inv_plus [0] = 50;
        you[0].inv_plus2 [0] = 50;
        you[0].inv_dam [0] = 0;
        you[0].inv_col [0] = LIGHTCYAN;

        you[0].inv_quant [1] = 1;
        you[0].inv_class [1] = 0;
        you[0].inv_type [1] = 14;
        you[0].inv_plus [1] = 50;
        you[0].inv_plus2 [1] = 50;
        you[0].inv_dam [1] = 0;
        you[0].inv_col [1] = BROWN;

        you[0].inv_quant [2] = 15 + random() % 5 + random() % 5 + random() % 5 + random() % 5 + random() % 5;
        you[0].inv_class [2] = 1;
        you[0].inv_type [2] = 1;
        you[0].inv_plus [2] = 50;
        you[0].inv_plus2 [2] = 50;
        you[0].inv_dam [2] = 0;
        you[0].inv_col [2] = LIGHTCYAN;

        you[0].inv_quant [3] = 1;
        you[0].inv_class [3] = 2;
        you[0].inv_type [3] = 1;
        you[0].inv_plus [3] = 50;
        you[0].inv_dam [3] = 0;
        you[0].inv_col [3] = BROWN;

/*      you[0].AC = 2;
        you[0].evasion = 9;*/
        you[0].strength += 3;
        you[0].dex += 4;
        you[0].intel += 3;
        you[0].equip [0] = 0;
        you[0].equip [6] = 3;
        you[0].gp = random2(10);
/* you[0].res_magic = 4;*/

 you[0].skills [0] = 2;
 you[0].skills [14] = 1;
/* if (you[0].skills [14] == 2) you[0].evasion ++;*/
 you[0].skills [15] = 1;
 you[0].skills [16 + random() % 2] ++;
 you[0].skills [12] = 2;
 you[0].skills [9] = 2;
 you[0].skills [25] = 1;
break;

case 10: // Conjurer
case 11: // Enchanter
case 14: // Summoner
case 12:
case 13:
case 15:
case 16:
case 19:
case 21:
        if (you[0].clas == 10) strcpy(you[0].clasnam, "conjurer");
        if (you[0].clas == 11) strcpy(you[0].clasnam, "Enchanter");
        if (you[0].clas == 12) strcpy(you[0].clasnam, "Fire Elementalist");
        if (you[0].clas == 13) strcpy(you[0].clasnam, "Ice Elementalist");
        if (you[0].clas == 14) strcpy(you[0].clasnam, "Summoner");
        if (you[0].clas == 15) strcpy(you[0].clasnam, "Air Elementalist");
        if (you[0].clas == 16) strcpy(you[0].clasnam, "Earth Elementalist");
        if (you[0].clas == 19) strcpy(you[0].clasnam, "Venom Mage");
        if (you[0].clas == 21) strcpy(you[0].clasnam, "transmuter");

        you[0].hp = 10; you[0].hp_max = 10;
        you[0].ep = 3; you[0].ep_max = 3;
/*      you[0].f_abil = 5;
        you[0].mag_abil = 7;
 you[0].thr_abil = 5;
 you[0].speed = 10;*/
 you[0].spell_levels = 4;
        you[0].inv_quant [0] = 1;
        you[0].inv_class [0] = 0;
                you[0].inv_type [0] = 3;
        you[0].inv_plus [0] = 50;
        you[0].inv_plus2 [0] = 50;
        if (you[0].clas == 11) you[0].inv_plus [0] = 51;
        if (you[0].clas == 11) you[0].inv_plus2 [0] = 51;
        you[0].inv_dam [0] = 0;
        you[0].inv_col [0] = LIGHTCYAN;

//      if (you[0].species == 17) you[0].inv_type [0] = 17;

        you[0].inv_quant [1] = 1;
        you[0].inv_class [1] = 2;
        you[0].inv_type [1] = 0;
        you[0].inv_plus [1] = 50;
        if (you[0].clas == 11) you[0].inv_plus [1] = 51;
        you[0].inv_dam [1] = 0;
        you[0].inv_col [1] = random() % 15 + 1;
        if (you[0].clas == 12) you[0].inv_col [1] = RED;
        if (you[0].clas == 13) you[0].inv_col [1] = LIGHTCYAN;
        if (you[0].clas == 15) you[0].inv_col [1] = LIGHTBLUE;
        if (you[0].clas == 16) you[0].inv_col [1] = BROWN;
        if (you[0].clas == 19) you[0].inv_col [1] = GREEN;
/*      you[0].AC = 1;
        if (you[0].clas == 11) you[0].AC = 2;
        you[0].evasion = 10;*/
//      you[0].strength = 6;
        you[0].dex += 4;
        you[0].intel += 6;
        you[0].equip [0] = 0;
        you[0].equip [6] = 1;
        you[0].gp = random2(10);
        you[0].inv_class [2] = 10;

/*        you[0].res_magic = 10;*/

        you[0].inv_type [2] = 3 + random2(2); // conj
        you[0].inv_plus [2] = 127;
        if (you[0].clas == 14)
        {
         you[0].inv_type [2] = 25; // summ
         you[0].inv_plus [2] = 127;
         you[0].skills [28] = 4;
         // gets some darts - this class is very difficult to start off with
                you[0].inv_quant [3] = random2(5) + random2(5) + 7;
                you[0].inv_class [3] = 1;
                you[0].inv_type [3] = 3; //wtype;
                you[0].inv_plus [3] = 50;
                you[0].inv_dam [3] = 0;
                you[0].inv_col [3] = LIGHTCYAN;
        }
        if (you[0].clas == 10)
        {
         you[0].skills [26] = 4; // conjurer
        }
        if (you[0].clas == 11) // ench
        {
/*          you[0].res_magic = 12;*/

         you[0].inv_type [2] = 26; // charms
         you[0].inv_plus [2] = 127;
         you[0].skills [27] = 4;

        you[0].inv_quant [3] = 1;
        you[0].inv_class [3] = 3;
        you[0].inv_dam [3] = 0;
        you[0].inv_col [3] = random() % 15 + 1;
        switch(random() % 4)
        {
         case 0: you[0].inv_type [3] = 2;
         you[0].inv_plus [3] = 7 + random() % 5;
         break;
         case 1: you[0].inv_type [3] = 6;
         you[0].inv_plus [3] = 5 + random() % 4;
         break;
         case 2: you[0].inv_type [3] = 10;
         you[0].inv_plus [3] = 4 + random() % 4;
         break;
         case 3: you[0].inv_type [3] = 6;
         you[0].inv_plus [3] = 5 + random() % 4;
         break;
        }
        you[0].inv_plus2 [3] = 0;
        switch(random() % 8) /* get a random lvl 1 attack spell*/
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

        } // end of enchanter
        switch(you[0].clas)
        {
         case 12: // fire elementalist
         you[0].inv_type [2] = 5;
         you[0].inv_plus [2] = 127;
         you[0].skills [26] = 1; // conj
//         you[0].skills [27] = 1; // ench
         you[0].skills [33] = 3; // fire magic
         you[0].spells [0] = 75;
         break;

         case 13: // Ice elementalist
         you[0].inv_type [2] = 6;
         you[0].inv_plus [2] = 127;
         you[0].skills [26] = 1; // conj
//         you[0].skills [27] = 1; // ench
         you[0].skills [34] = 3; // ice magic
         you[0].spells [0] = 76;
         break;

         case 15: // Air elementalist
         you[0].inv_type [2] = 28;
         you[0].inv_plus [2] = 127;
         you[0].skills [26] = 1; // conj
//         you[0].skills [27] = 1; // ench
         you[0].skills [35] = 3; // air magic
         you[0].spells [0] = 132;
         break;

         case 16: // Earth elementalist
         you[0].inv_type [2] = 39;
         you[0].inv_plus [2] = 127;
         you[0].skills [26] = 1; // conj
//         you[0].skills [27] = 1; // ench
         you[0].skills [36] = 3; // earth magic
         you[0].spells [0] = 127;
         break;

         case 19: // Venom Mage
         you[0].inv_type [2] = 13;
         you[0].inv_plus [2] = 126;
         you[0].skills [37] = 4; // Poison magic
         you[0].spells [0] = 115;
         break;

         case 21: // transmuter
         you[0].inv_type [2] = 17;
         you[0].inv_plus [2] = 126;
         you[0].skills [31] = 4; // transmigrations
         you[0].spells [0] = 150;
         break;

        }

        you[0].inv_quant [2] = 1;
//      you[0].inv_plus [2] = 124;
        you[0].inv_dam [2] = 0;
        you[0].inv_col [2] = random() % 15 + 1;
        if (you[0].clas == 12) you[0].inv_col [2] = RED;
        if (you[0].clas == 13) you[0].inv_col [2] = LIGHTCYAN;
        if (you[0].clas == 19) you[0].inv_col [2] = GREEN;
/*  you[0].res_magic = 10;*/

        you[0].skills [14] = 1;
        you[0].skills [15] = 1;
        you[0].skills [14 + random() % 2] ++;
/* if (you[0].skills [14] == 2) you[0].evasion ++;*/

        you[0].skills [25] = 1;
        you[0].skills [1] = 1;
/*      if (you[0].species == 17)
        {
         you[0].skills [1] = 0;
         you[0].skills [6] = 2;
        }*/
        you[0].skills [7] = 1;


break;


case 17: // Crusader
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
        you[0].inv_class [0] = 0;
        you[0].inv_type [0] = 5;

//      if (you[0].species == 17) you[0].inv_type [0] = 17;

        you[0].inv_plus [0] = 50;
        you[0].inv_plus2 [0] = 50;
        you[0].inv_dam [0] = 0;
        you[0].inv_col [0] = LIGHTCYAN;
        choose_weapon();
        weap_skill = 2;
        you[0].inv_quant [1] = 1;
        you[0].inv_class [1] = 2;
        you[0].inv_type [1] = 0;
        you[0].inv_plus [1] = 50;
        you[0].inv_dam [1] = 0;
        you[0].inv_col [1] = 1 + random() % 15;


        you[0].inv_class [2] = 10;
        you[0].inv_type [2] = 20;
        you[0].inv_quant [2] = 1;
        you[0].inv_plus [2] = 124;
        you[0].inv_dam [2] = 0;
        you[0].inv_col [2] = 1 + random() % 15;

/*      you[0].AC = 1;
        you[0].evasion = 10;*/
        you[0].strength += 4;
        you[0].dex += 3;
        you[0].intel += 3;
        you[0].equip [0] = 0;
        you[0].equip [6] = 1;
        you[0].gp = random2(10);

 you[0].skills [0] = 2;
 you[0].skills [13] = 1;
 you[0].skills [14] = 1;
 you[0].skills [13 + random() % 2] ++;
/*  if (you[0].skills [14] == 2) you[0].evasion ++;*/

 you[0].skills [15] = 1;
// you[0].skills [1] = 2;

/* if (you[0].species == 17)
 {
  you[0].skills [1] = 0;
  you[0].skills [6] = 2;
 }*/

 you[0].skills [16] = 1;
 you[0].skills [25] = 1;
 you[0].skills [27] = 1;
break;


case 18: // Death knight
        strcpy(you[0].clasnam, "Death Knight");
/*        you[0].piety = 75;*/
        you[0].hp = 13; you[0].hp_max = 13;
        you[0].ep = 1; you[0].ep_max = 1;
        you[0].speed = 10;
        you[0].spell_levels = 4;
        you[0].inv_quant [0] = 1;
        you[0].inv_class [0] = 0;
        you[0].inv_type [0] = 5;
        you[0].inv_plus [0] = 50;
        you[0].inv_plus2 [0] = 50;
        you[0].inv_dam [0] = 0;
        you[0].inv_col [0] = LIGHTCYAN;
        choose_weapon();
        weap_skill = 2;

        you[0].inv_quant [1] = 1;
        you[0].inv_class [1] = 2;
        you[0].inv_type [1] = 0;
        you[0].inv_plus [1] = 50;
        you[0].inv_dam [1] = 0;
        you[0].inv_col [1] = DARKGREY;


        you[0].inv_class [2] = 10;
        you[0].inv_type [2] = 23;
        you[0].inv_quant [2] = 1;
        you[0].inv_plus [2] = 127;
        you[0].inv_dam [2] = 0;
        you[0].inv_col [2] = DARKGREY;

/*      you[0].AC = 1;
        you[0].evasion = 10;*/
        you[0].strength += 4;
        you[0].dex += 3;
        you[0].intel += 3;
        you[0].equip [0] = 0;
        you[0].equip [6] = 1;
        you[0].gp = random2(10);

 you[0].skills [0] = 2;
 you[0].skills [13] = 1;
 you[0].skills [14] = 1;
 you[0].skills [13 + random() % 2] ++;
/*  if (you[0].skills [14] == 2) you[0].evasion ++;*/

 you[0].skills [15] = 1;
// you[0].skills [1] = 2;
 you[0].skills [16] = 1;
 you[0].skills [25] = 1;
 you[0].skills [29] = 1;
break;

case 20: // Chaos knight
        strcpy(you[0].clasnam, "Knight of Chaos");
/*        you[0].piety = 75;*/
        you[0].hp = 13; you[0].hp_max = 13;
        you[0].ep = 1; you[0].ep_max = 1;
        you[0].speed = 10;
        you[0].spell_levels = 4;
        you[0].inv_quant [0] = 1;
        you[0].inv_class [0] = 0;
        you[0].inv_type [0] = 5;
        you[0].inv_plus [0] = 50 + random2(3);
        you[0].inv_plus2 [0] = 50 + random2(3);
        you[0].inv_dam [0] = 0;
        if (random2(5) == 0) you[0].inv_dam [0] = 30;
        if (random2(5) == 0) you[0].inv_dam [0] = 60;
        you[0].inv_col [0] = LIGHTCYAN;
        choose_weapon();
        weap_skill = 2;
        you[0].inv_quant [1] = 1;
        you[0].inv_class [1] = 2;
        you[0].inv_type [1] = 0;
        you[0].inv_plus [1] = 50 + random2(3);
        you[0].inv_dam [1] = 0;
        you[0].inv_col [1] = random2(15) + 1;

        you[0].strength += 4;
        you[0].dex += 3;
        you[0].intel += 3;
        you[0].equip [0] = 0;
        you[0].equip [6] = 1;
        you[0].gp = random2(10);

 you[0].skills [0] = 2;
 you[0].skills [13] = 1;
 you[0].skills [14] = 1;
 you[0].skills [13 + random() % 2] ++;
/*  if (you[0].skills [14] == 2) you[0].evasion ++;*/

 you[0].skills [15] = 1;
// you[0].skills [1] = 2;
 you[0].skills [16] = 1;
 you[0].skills [25] = 1;
 you[0].skills [26] = 1;

 you[0].religion = 5;
break;



case 22: // Healer
        strcpy(you[0].clasnam, "Healer");
        you[0].piety = 45;
        you[0].hp = 15; you[0].hp_max = 15;
        you[0].ep = 1; you[0].ep_max = 1;
        you[0].spell_levels = 8;

        you[0].inv_quant [0] = 1;
        you[0].inv_class [0] = 0;
        you[0].inv_type [0] = 18;

        you[0].inv_plus [0] = 50;
        you[0].inv_plus2 [0] = 50;
        you[0].inv_dam [0] = 0;
        you[0].inv_col [0] = BROWN;


        // Robe
        you[0].inv_quant [1] = 1;
        you[0].inv_class [1] = 2;
        you[0].inv_type [1] = 0;
        you[0].inv_plus [1] = 50;
        you[0].inv_dam [1] = 0;
        you[0].inv_col [1] = WHITE;

        you[0].inv_class [2] = 8;
        you[0].inv_type [2] = 0;
        you[0].inv_quant [2] = 2 + random2(2);
        you[0].inv_plus [2] = 0;
        you[0].inv_dam [2] = 0;
        you[0].inv_col [2] = random2(15) + 1; // hmmm...

        you[0].inv_class [3] = 8;
        you[0].inv_type [3] = 1;
        you[0].inv_quant [3] = 2 + random2(2);
        you[0].inv_plus [3] = 0;
        you[0].inv_dam [3] = 0;
        you[0].inv_col [3] = random2(15) + 1; // hmmm...

        you[0].strength += 4;
        you[0].dex += 2;
        you[0].intel += 4;

        you[0].equip [0] = 0;
        you[0].equip [6] = 1;

        you[0].gp = random2(100);

 you[0].skills [0] = 2;
 you[0].skills [14] = 1;
 you[0].skills [17] = 1;
 you[0].skills [12] = 2;
 you[0].skills [7] = 3;

 you[0].skills [25] = 2;

 you[0].religion = 12;

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

you[0].spell_levels = you[0].skills [25] * 4;
/*you[0].res_magic = 3 + you[0].skills [27] * 2;*/

char points_left = 8;

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
case 2: // elf
you[0].hp_max --;
you[0].base_hp2 --;
break;
case 3: // high elf
you[0].hp_max --;
you[0].base_hp2 --;
break;
case 4: // grey elf
you[0].hp_max --;
you[0].base_hp2 --;
you[0].ep_max ++;
you[0].base_ep2 ++;
break;
case 5: // deep elf
you[0].hp_max --;
you[0].hp_max --;
you[0].base_hp2 -= 2;
you[0].ep_max ++;
you[0].base_ep2 ++;
break;
case 7: // hill dwarf
you[0].hp_max ++;
you[0].base_hp2 ++;
break;
case 8: // mountain dwarf
you[0].hp_max ++;
you[0].base_hp2 ++;
break;
case 9: // halfling
you[0].hp_max --;
you[0].hp_max --;
you[0].base_hp2 --;
you[0].base_hp2 --;
/*you[0].evasion ++;*/
break;
case 10: // hill orc
you[0].hp_max ++;
you[0].base_hp2 ++;
break;
case 11: // kobold
you[0].hp_max -= 2;
you[0].base_hp2 -= 2;
/*you[0].evasion ++;*/
break;
case 12: // mummy
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
case 13: // Naga
you[0].hp_max += 2;
you[0].base_hp2 += 2;
/*you[0].evasion -= 3;
you[0].AC += 2;
you[0].res_poison ++;
you[0].see_invis ++;*/
you[0].attribute [4] ++;
break;
case 14: // Gnome
you[0].hp_max --;
you[0].hp_max --;
you[0].base_hp2 --;
you[0].base_hp2 --;
/*you[0].AC ++;*/
break;
case 15: // Ogre
you[0].hp_max += 3;
you[0].base_hp2 += 3;
you[0].hunger_inc += 1;
break;
case 16: // Troll
you[0].hp_max += 3;
you[0].base_hp2 += 3;
you[0].hunger_inc += 3;
break;
case 17: // Ogre-Mage
you[0].hp_max += 2;
you[0].base_hp2 += 2;
you[0].hunger_inc += 1;
break;
case 18: // Draconian
you[0].hp_max += 1;
you[0].base_hp2 += 1;
break;
}

you[0].hp = you[0].hp_max;
you[0].ep = you[0].ep_max;

you[0].max_dex = you[0].dex;
you[0].max_strength = you[0].strength;
you[0].max_intel = you[0].intel;

if (weap_skill != 0) you[0].skills [weapon_skill(0, you[0].inv_type [0])] = weap_skill;

if (you[0].is_undead != 2)
{
 for (i = 0; i < 52; i ++)
 {
  if (you[0].inv_quant [i] == 0)
  {
   you[0].inv_quant [i] = 1;
   you[0].inv_class [i] = 4;
   you[0].inv_type [i] = 1;
   if (you[0].species == 10 | you[0].species == 11 | you[0].species == 15 | you[0].species == 16) you[0].inv_type [i] = 0;
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
  if (you[0].inv_class [i] == 10)
  {
   you[0].had_item [you[0].inv_type [i]] = 1;
   if (you[0].inv_type [i] == 0 | you[0].inv_type [i] == 1 | you[0].inv_type [i] == 2)
   {
    you[0].had_item [0] = 1;
    you[0].had_item [1] = 1;
    you[0].had_item [2] = 1;
   }
   if (you[0].inv_type [i] == 3 | you[0].inv_type [i] == 4)
   {
    you[0].had_item [3] = 1;
    you[0].had_item [4] = 1;
   }
  }

  if (you[0].inv_class [i] <= 2) // | you[0].inv_class [i] == 2)
  switch(you[0].species)
  {
   case 2:
   case 3:
   case 4:
   case 5:
   case 6:
   you[0].inv_dam [i] = 120;
   break;

   case 7:
   case 8:
   you[0].inv_dam [i] = 150;
   you[0].inv_col [i] = CYAN;
   break;

   case 10:
   if (you[0].inv_class [i] == 0) you[0].inv_dam [i] = 90;
     else you[0].inv_dam [i] = 180;
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
 if (you[0].skills [i] != 0)
 you[0].skill_points [i] = skill_exp_needed(you[0].skills [i] + 1) * species_skills(i, you[0].species) / 100;
}

for (i = 0; i < 52; i ++)
{
        if (you[0].inv_class [i] != 0)
        {
                set_id(you[0].inv_class [i], you[0].inv_type [i], 1);
        }
}


/*if (you[0].clas == 0) you [0].shield_class = 5;
if (you[0].clas == 4) you [0].shield_class = 3;
if (you[0].clas == 6) you [0].shield_class = 5;

if (you[0].clas == 2 | you[0].clas == 6) you[0].spec_holy = 1;*/

if (you[0].clas == 1 | you[0].clas == 10) you[0].spells [0] = 5;
if (you[0].clas == 5) you[0].spells [0] = 67;
if (you[0].clas == 12) you[0].spells [0] = 75;
if (you[0].clas == 13) you[0].spells [0] = 76;
if (you[0].clas == 14) you[0].spells [0] = 49;

/*case 10: // Conjurer
case 11: // Enchanter
case 12: // Fire Wizard
case 13: // Ice Wizard
case 14: // Summoner*/

if (you[0].spells [0] != 210) you[0].spell_no = 1; else you[0].spell_no = 0;
if (you[0].clas == 2 | you[0].clas == 6) set_id(8, 0, 1);


        char del_file [55];


char glorpstr [40];
strncpy(glorpstr, you[0].your_name, 6);

// glorpstr [strlen(glorpstr)] = 0;
// This is broken. Length is not valid yet! We have to check if we got a
// trailing NULL; if not, write one:
if (strlen(you[0].your_name) > 5)    /* is name 6 chars or more? */
        glorpstr[6] = (char) NULL;   /* if so, char 7 should be NULL */

strncpy(glorpstr, you[0].your_name, 6);

// glorpstr [strlen(glorpstr)] = 0;
// This is broken. Length is not valid yet! We have to check if we got a
// trailing NULL; if not, write one:
if (strlen(you[0].your_name) > 5)    /* is name 6 chars or more? */
        glorpstr[6] = (char) NULL;   /* if so, char 7 should be NULL */

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
strncat(cha_fil, you[0].your_name, 6);
strcat(cha_fil, ".");
if (was_a_labyrinth == 1) strcat(cha_fil, "lab"); / * temporary level * /
 else strcat(cha_fil, extens);
*/

/* Places the staircases to the branch levels: */
you[0].branch_stairs [0] = 5 + random2(6); // orc mines
you[0].branch_stairs [1] = 12 + random2(6); // hive
you[0].branch_stairs [2] = 8 + random2(6); // lair
you[0].branch_stairs [3] = you[0].branch_stairs [2] + random2(4) + 3; // slime pits
you[0].branch_stairs [4] = 20 + random2(6); // vaults
you[0].branch_stairs [5] = you[0].branch_stairs [4] + random2(3) + 3; // crypt
you[0].branch_stairs [6] = you[0].branch_stairs [5] + 4; // hall of blades
you[0].branch_stairs [7] = 35; // hall of Zot
you[0].branch_stairs [8] = 3 + random2(4); // Temple
you[0].branch_stairs [9] = you[0].branch_stairs [2] + random2(2) + 7; // Snake pit
you[0].branch_stairs [10] = you[0].branch_stairs [0] + random2(2) + 3; // elven halls
you[0].branch_stairs [11] = you[0].branch_stairs [5] + random2(2) + 3; // Tomb
                return 1;

} // end of new_game()



char class_allowed(char speci, char clas)
{
switch(clas)
{
case 0: // fighter
switch(speci)
{
 case 17: return 0;
}
return 1;

case 1: // wizard
switch(speci)
{
 case 10:
 case 9:
 case 7:
 case 11:
 case 12:
 case 14: // gnome
// case 8:
 case 15:
 case 16:
 case 17:
 return 0;
}
return 1;

case 2: // priest
switch(speci)
{
 case 9:
 case 11:
 case 12:
 case 13:
 case 14: // gnome
 case 15:
 case 16:
 case 17:
 case 18: /* Drac */
 case 19:
 case 20:
 case 21:
 case 22:
 case 23:
 case 24:
 case 25:
 case 26:
 case 27:
 case 28:
 case 29: /* Draconians */
 return 0;
}
return 1;

case 3: // thief
switch(speci)
{
 case 12:
 case 15:
 case 16:
 case 17:
 return 0;
}
return 1;

case 4: // glad
switch(speci)
{
 case 1:
 case 7:
 case 8:
 case 10:
 case 18: /* Drac */
 case 19:
 case 20:
 case 21:
 case 22:
 case 23:
 case 24:
 case 25:
 case 26:
 case 27:
 case 28:
 case 29: /* Draconians */
 return 1;
}
return 0;

case 5: // necromancer
switch(speci)
{
 case 1:
 case 5:
 case 6:
 case 10:
 case 12:
 case 13:
 return 1;
}
return 0;

case 6: // Paladin
switch(speci)
{
 case 1:
 case 8:
 return 1;
}
return 0;

case 7: // assassin
switch(speci)
{
 case 1:
 case 2:
 case 3:
 case 4:
 case 5:
 case 6:
 case 10:
 case 12:
 case 13:
 return 1;
}
return 0;

case 8: // berserker
switch(speci)
{
 case 1:
 case 7:
 case 10:
 return 1;
}
return 0;

case 9: // ranger
switch(speci)
{
 case 1:
 case 2:
 case 3:
 case 4:
 case 6:
 case 10:
 case 13:
 case 18: /* Drac */
 case 19:
 case 20:
 case 21:
 case 22:
 case 23:
 case 24:
 case 25:
 case 26:
 case 27:
 case 28:
 case 29: /* Draconians */
 return 1;
}
return 0;

case 10: // Conjurer
switch(speci)
{
 case 11:
 case 12:
 case 9:
 case 14: // gnome
 case 15:
 case 16:
 case 17:
 return 0;
}
return 1;

case 11: // Enchanter
switch(speci)
{
 case 10:
 case 11:
 case 12:
 case 14: // gnome
 case 15:
 case 16:
 return 0;
}
return 1;

case 12: // Fire elementalist
switch(speci)
{
 case 1:
 case 2:
 case 3:
 case 5:
 case 6:
 case 7:
 case 8:
 case 10:
 return 1;
}
return 0;

case 13: // Ice elementalist
switch(speci)
{
 case 1:
 case 2:
 case 3:
 case 5:
 case 6:
 return 1;
}
return 0;

case 14: // Summoner
switch(speci)
{
 case 9:
 case 8:
 case 7:
 case 12:
 case 14: // gnome
 case 15:
 case 16:
 case 17:
 return 0;
}
return 1;

case 15: // Air elementalist
switch(speci)
{
 case 1:
 case 2:
 case 3:
 case 4:
 case 5:
 case 6:
 return 1;
}
return 0;


case 16: // Earth elementalist
switch(speci)
{
 case 1:
 case 5:
 case 6:
 case 7:
 case 8:
 case 10:
 case 14: // gnome
 return 1;
}
return 0;

case 17: // Crusader
switch(speci)
{
 case 1:
 case 2:
 case 3:
 case 4:
 case 5:
 case 6:
 case 10:
 case 17:
 case 18: /* Drac */
 case 19:
 case 20:
 case 21:
 case 22:
 case 23:
 case 24:
 case 25:
 case 26:
 case 27:
 case 28:
 case 29: /* Draconians */
 return 1;
}
return 0;

case 18: // Death knight
switch(speci)
{
 case 1:
 case 5:
 case 6:
 case 10:
 case 12:
 case 13:
 case 18: /* Drac */
 case 19:
 case 20:
 case 21:
 case 22:
 case 23:
 case 24:
 case 25:
 case 26:
 case 27:
 case 28:
 case 29: /* Draconians */
 return 1;
}
return 0;

case 19: // tainter
switch(speci)
{
 case 1:
 case 5:
 case 6:
 case 10:
 case 11:
 case 13:
 case 18: /* Drac */
 case 19:
 case 20:
 case 21:
 case 22:
 case 23:
 case 24:
 case 25:
 case 26:
 case 27:
 case 28:
 case 29: /* Draconians */
 return 1;
}
return 0;

case 20: // chaos knight
switch(speci)
{
 case 1:
 case 2:
 case 3:
 case 5:
 case 6:
 case 10:
 case 11:
 case 13:
 case 18: /* Drac */
 case 19:
 case 20:
 case 21:
 case 22:
 case 23:
 case 24:
 case 25:
 case 26:
 case 27:
 case 28:
 case 29: /* Draconians */
 return 1;
}
return 0;

case 21: // transmuter
switch(speci)
{
 case 10:
 case 9:
 case 7:
 case 11:
 case 12:
 case 14: // gnome
 case 15:
 case 16:
 case 17:
 case 18: /* Drac */
 case 19:
 case 20:
 case 21:
 case 22:
 case 23:
 case 24:
 case 25:
 case 26:
 case 27:
 case 28:
 case 29: /* Draconians */
 return 0;
}
return 1;

case 22: // healer
switch(speci)
{
 case 9:
 case 11:
 case 12:
 case 13:
 case 14: // gnome
 case 15:
 case 16:
 case 17:
 case 18: /* Drac */
 case 19:
 case 20:
 case 21:
 case 22:
 case 23:
 case 24:
 case 25:
 case 26:
 case 27:
 case 28:
 case 29: /* Draconians */
 return 0;
}
return 1;

default: return 0;

}

}


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

if (you[0].clas == 20)
{
 switch(random2(5))
 {
  case 0: you[0].inv_type [0] = 5; return;
  case 1: you[0].inv_type [0] = 2; return;
  case 2: you[0].inv_type [0] = 4; return;
  case 3: you[0].inv_type [0] = 9; return;
  case 4: you[0].inv_type [0] = 11; return;
 }
 return;
}

clrscr();
cprintf("\n\r You have a choice of weapons:\n\r");
if (you[0].clas == 4) goto glad_thing;
cprintf("a - short sword\n\r");
cprintf("b - mace\n\r");
cprintf("c - hand axe\n\r");
cprintf("d - spear\n\r");

getkey : keyin = get_ch();
switch(keyin)
{
 case 'a': you[0].inv_type [0] = 5; return;
 case 'b': you[0].inv_type [0] = 1; return;
 case 'c': you[0].inv_type [0] = 9; return;
 case 'd': you[0].inv_type [0] = 11; return;
 default: goto getkey;
}

glad_thing : cprintf("a - short sword\n\r");
cprintf("b - flail\n\r");
cprintf("c - morningstar\n\r");
cprintf("d - hand axe\n\r");
cprintf("e - spear\n\r");

getkey2 : keyin = get_ch();
switch(keyin)
{
 case 'a': you[0].inv_type [0] = 5; return;
 case 'b': you[0].inv_type [0] = 2; return;
 case 'c': you[0].inv_type [0] = 4; return;
 case 'd': you[0].inv_type [0] = 9; return;
 case 'e': you[0].inv_type [0] = 11; return;
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
you[0].clas = 0;
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

you[0].religion = 0;
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

for (ic = 0; ic < 50; ic ++)
{
 you[0].mutation [ic] = 0;
}

for (ic = 0; ic < 50; ic ++)
{
 you[0].had_item [ic] = 0;
}

}

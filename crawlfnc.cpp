#include <conio.h>
//#include <alloc.h>
#include <string.h>
#include <stdlib.h>
#include <io.h>
#include <fcntl.h>
#include <stdio.h>
#include <iostream.h>
#include <SYS\STAT.H>
#include "fn2.h"
#include "fn3.h"

#include "monsstat.h"


#define ITEMS 500
#define MNST 200
#define CLOUDS 30
#define NTRAPS 30

#define MLAVA0 220
#define MLAVA1 221
#define MLAVA2 222
#define MLAVA3 223
#define MLAVA4 224
#define MLAVA5 225

#define MWATER0 230
#define MWATER1 231
#define MWATER2 232
#define MWATER3 233
#define MWATER4 234
#define MWATER5 235


//                      REMEMBER to run levels.bat !!!

char item_name_2(char item_clas, char item_typ, unsigned char item_da, unsigned char it_plus, unsigned int it_quant, char ident_lev, char glog [60]);

void moname(char mcl, char mench, char see_inv, char descrip, char glog [40]);

char *monam(char mons_cla, char mons_e, char desc, char see_invis);

//void spellbook_template(int sbook_type, int func_pass [10]);


int dir_cursor(char rng)
{
        int xps = 7, yps = 7;
        char gotch;
        char mve_x, mve_y;
        char bk = 0;
        char keyy;

/*      strcpy(info [info_lines], "Which direction?");
        info_lines++;
        message();
        keyin_2 = getch();*/


//        if (rng == 2) skip the switch




if (rng == 100) goto cursoring;



        keyy = getch();

        if (keyy != 0 && keyy != '*' && keyy != '.')
        {
                //keyy = getch();
  switch(keyy)
  {
                case 'b': mve_x = -1; mve_y = 1; break;
                case 'j': mve_y = 1; mve_x = 0; break;
                case 'u': mve_x = 1; mve_y = -1; break;
                case 'k': mve_y = -1; mve_x = 0; break;
                case 'y': mve_y = -1; mve_x = -1; break;
                case 'h': mve_x = -1; mve_y = 0; break;
                case 'n': mve_y = 1; mve_x = 1; break;
                case 'l': mve_x = 1; mve_y = 0; break;
   case 'p': return 253;
//   case '*': break;
   default: return -1;
  }
//  bk = 1;
               return mve_x * 100 + mve_y + 707 + 10000;

/*  if (keyy == 'r') return 253; // note this!!!
                if (keyy != '*') return -1;*/


        }

        if (keyy != '*' && keyy != '.') keyy = getch();

        switch(keyy)
        {
                case 'O': mve_x = -1; mve_y = 1; bk = 1; break;
                case 'P': mve_y = 1; mve_x = 0;bk = 1; break;
                case 'I': mve_x = 1; mve_y = -1;bk = 1; break;
                case 'H': mve_y = -1; mve_x = 0;bk = 1; break;
                case 'G': mve_y = -1; mve_x = -1;bk = 1; break;
                case 'K': mve_x = -1; mve_y = 0;bk = 1; break;
                case 'Q': mve_y = 1; mve_x = 1; bk = 1; break;
                case 'M': mve_x = 1; mve_y = 0;bk = 1; break;
  case '.':
                case 'S': bk = 1; mve_x = 0; mve_y = 0; break;
                case '*': bk = 0; break; // was 'L'

                // need <, > etc

                default: return -1; //strcpy(info [info_lines], "What an unusual direction.");
                        //info_lines++; mve_x = 0; mve_y = 0; return 99;
        }

        if (bk == 1)
        {
               return mve_x * 100 + mve_y + 707 + 10000;
        }

        if (rng == 0) return 254;
//      char grokol [3];


//      gotoxy(count_x - x_pos + 6, count_y - y_pos + 6);

        cursoring: gotoxy(xps + 10,yps + 2);
        //keyin = getch();

//strcpy(info [info_lines], "Which target?");
//info_lines++;
//message();

        window(2,1,34,17);
        gotoxy(xps + 10, yps + 2);

//while (gotch != '.')
do
{
//gotch = getch();



        gotch = getch();

   if (keyy == 'p') return 253;

        if (gotch != 0 && gotch != 13 && gotch != '.' && gotch != '>')
        {
                //gotch = getch();
  switch(gotch)
  {
                case 'b': mve_x = -1; mve_y = 1; break;
                case 'j': mve_y = 1; mve_x = 0; break;
                case 'u': mve_x = 1; mve_y = -1; break;
                case 'k': mve_y = -1; mve_x = 0; break;
                case 'y': mve_y = -1; mve_x = -1; break;
                case 'h': mve_x = -1; mve_y = 0; break;
                case 'n': mve_y = 1; mve_x = 1; break;
                case 'l': mve_x = 1; mve_y = 0; break;
                default: return -1;
  }
  goto gotchy2;

        }

        if (gotch != 13 && gotch != 13 && gotch != '.' && gotch != '>') gotch = getch();

        mve_x = 0; mve_y = 0;

        switch(gotch)
        {
  case '>': break;
//  case '.':
   case '*': gotch = 'S'; break;
                case 'O': mve_x = -1; mve_y = 1;break;
                case 'P': mve_y = 1; mve_x = 0; break;
                case 'I': mve_x = 1; mve_y = -1; break;
                case 'H': mve_y = -1; mve_x = 0; break;
                case 'G': mve_y = -1; mve_x = -1; break;
                case 'K': mve_x = -1; mve_y = 0; break;
                case 'Q': mve_y = 1; mve_x = 1; break;
                case 'M': mve_x = 1; mve_y = 0; break;
  case '.':
                case 'S': break;
                //case 'L': bk = 0; break;

                // need <, > etc

                default: return -1; //strcpy(info [info_lines], "What an unusual direction.");
                //info_lines++; mve_x = 0; mve_y = 0; return 99;
        }

/*
mve_x = 0; mve_y = 0;
switch(gotch)
{
        case '1': mve_x = -1; mve_y = 1; break;
        case '2': mve_y = 1; mve_x = 0; break;
        case '9': mve_x = 1; mve_y = -1; break;
        case '8': mve_y = -1; mve_x = 0; break;
        case '7': mve_y = -1; mve_x = -1; break;
        case '4': mve_x = -1; mve_y = 0; break;
        case '3': mve_y = 1; mve_x = 1; break;
        case '6': mve_x = 1; mve_y = 0; break;
        case '.': break;
        default: return -1;
        // put in case '.'

        //default: strcpy(info [info_lines], "What an unusual direction.");
          //    info_lines++; message(); window(2,2,12,13); gotoxy(x,y); move_x = 0; move_y = 0;
}
*/
//gettext(x + 1, y + 1, x + 1, y + 1, grokol);
//textbackground(0);
gotchy2 : gotoxy(xps + 10,yps + 2);

if (gotch == '>')
{
 mve_x = xps - 7;
 mve_y = yps - 7;

 return (mve_x + 7) * 100 + mve_y + 7 + 50000;

}

//putch(grokol [0]);
//textbackground(7);

if (xps + mve_x >= 1 && xps + mve_x < 14) xps += mve_x;
if (yps + mve_y >= 1 && yps + mve_y < 14) yps += mve_y;

//gettext(x + 1, y + 1, x + 1, y + 1, grokol);
//textbackground(7);
gotoxy(xps + 10,yps + 2);
//putch(grokol [0]);
//textbackground(0);

} while (gotch != 'S' && gotch != '>' && gotch != '.');

// end of while.

//mve_x -= 6;

//mve_y -= 6;

mve_x = xps - 7;
mve_y = yps - 7;
/*
if (mve_x == 0)
{
        if (mve_y > 0) mve_y = 1;
        if (mve_y < 0) mve_y = -1;
}

if (mve_y == 0)
{
        if (mve_x > 0) mve_x = 1;
        if (mve_x < 0) mve_x = -1;
}

if (mve_x == mve_y | mve_x * -1 == mve_y)
{
        if (mve_y > 0) mve_y = 1;
        if (mve_y < 0) mve_y = -1;
        if (mve_x > 0) mve_x = 1;
        if (mve_x < 0) mve_x = -1;
}
*/
return (mve_x + 7) * 100 + mve_y + 7;

}

/*


void draw_border(int bord_col)
{
int loopi;

textcolor(bord_col);
// this bit draws the borders:
window(1,1,80,25);

clrscr();

for (loopi = 1; loopi < 80; loopi ++)
{
        gotoxy(loopi,1);
        putch(196);
        gotoxy(loopi,15);
        putch(196);
        gotoxy(loopi,25);
        putch(196);
}

for (loopi = 1; loopi < 25; loopi ++)
{
        gotoxy(1,loopi);
        putch(179);
        gotoxy(79,loopi);
        putch(179);
        if (loopi < 16)
        {
                gotoxy(15, loopi);
                putch(179);
        }
}

gotoxy(1,1); putch(218);
gotoxy(79,1); putch(191);
gotoxy(1,15); putch(195);
gotoxy(15,1); putch(194);
gotoxy(15,15); putch(193);
gotoxy(79,15); putch(180);
gotoxy(1,25); putch(192);
gotoxy(79,25); putch(217);

window(16,2,75,12);
clrscr();
gotoxy(4,2);
textcolor(7);
cprintf("HP:\n\r");
cprintf("   Magic:\n\r");

cprintf("   AC:\n\r");
cprintf("   EV:\n\r");

//cprintf("   Strength:\n\r");
//cprintf("   Intelligence:\n\r");
//cprintf("   Dexterity:\n\r");
cprintf("   Str:\n\r");
cprintf("   Int:\n\r");
cprintf("   Dex:\n\r");
cprintf("   Gold:\n\r");
cprintf("   Experience:\r");


} // end of void draw_border(int bord_col)

*/




void draw_border(int bord_col, char your_name [9], char clasnam [13])
{

//return;

int loopi;

textcolor(bord_col);
// this bit draws the borders:
window(1,1,80,25);

clrscr();
/*
for (loopi = 1; loopi < 80; loopi ++)
{
        gotoxy(loopi,1);
        putch(196);
        gotoxy(loopi,15);
        putch(196);
        gotoxy(loopi,25);
        putch(196);
}

for (loopi = 1; loopi < 25; loopi ++)
{
        gotoxy(1,loopi);
        putch(179);
        gotoxy(79,loopi);
        putch(179);
        if (loopi < 16)
        {
                gotoxy(35, loopi);
                putch(179);
        }
}

gotoxy(1,1); putch(218);
gotoxy(79,1); putch(191);
gotoxy(1,15); putch(195);
gotoxy(35,1); putch(194);
gotoxy(35,15); putch(193);
gotoxy(79,15); putch(180);
gotoxy(1,25); putch(192);
gotoxy(79,25); putch(217);
*/
window(36,1,75,15);
clrscr();
gotoxy(4,1);
textcolor(7);
//cprintf("Linley the Incredible!\n\r");
cprintf(your_name);
cprintf(" the ");
cprintf(clasnam);
cprintf("\n\r   HP:\n\r");
cprintf("   Magic:\n\r");

cprintf("   AC:\n\r");
cprintf("   EV:\n\r");

//cprintf("   Strength:\n\r");
//cprintf("   Intelligence:\n\r");
//cprintf("   Dexterity:\n\r");
cprintf("   Str:\n\r");
cprintf("   Int:\n\r");
cprintf("   Dex:\n\r");
cprintf("   Gold:\n\r");
cprintf("   Experience:\n\r");
cprintf("   Level:\r");

} // end of void draw_border(int bord_col)




















// maybe have mons_alloc tables here and call mons_define from there,
// because sometimes program may want to make a specific monster, not one
// randomly assigned for level.

/*
char mons_define(int mclss, unsigned char chrct, unsigned char mons_sec)
{

int mon_HD, mon_hp, mon_hp_max, mon_AC, mon_ev, mon_speed, mon_speed_inc; // ,mons_sec;
// used to be char

//mon_sec = 0;

//char *glog;
//char tmp_quant [5];

//char hig [10];


//glog = (int *)malloc(7);

//if (!glog) cprintf("error");



// if mons_sec = 250, determine it randomly if relevant


switch(mclss)
{

case 0: // giant ant
mon_HD = 3;
mon_hp = (random2((mon_HD) * 5) + (mon_HD) * 3);
mon_hp_max = mon_hp;
mon_AC = 4;//gmon_AC [2];
mon_ev = 10; //gmon_ev [2];
mon_speed = 12; // this is divided by 10 when reloaded
mon_speed_inc = 7;
break;

case 1: // giant bat
mon_HD = 1;
mon_hp = (random2((mon_HD) * 5) + (mon_HD) * 3);
mon_hp_max = mon_hp;
mon_AC = 1;//gmon_AC [2];
mon_ev = 20;//gmon_ev [2];
mon_speed = 30;
mon_speed_inc = 7;
break;

case 2: // centaur
mon_HD = 4;
mon_hp = (random2(mon_HD * 5) + mon_HD * 3);
mon_hp_max = mon_hp;
mon_AC = 3;//gmon_AC [2];
mon_ev = 7;//gmon_ev [2];
mon_speed = 20; // this is divided by 10 when reloaded
mon_speed_inc = 7;
//gmon_use [mons_class [k]] = 3;
break;

/*
case 2: // chicken? // cockatrice
mon_HD = 1;
mon_hp = (random2((mon_HD) * 2) + (mon_HD) * 2);
mon_hp_max = mon_hp;
mon_AC = 0;//gmon_AC [2];
mon_ev = 10;//gmon_ev [2];
mon_speed = 10; // this is divided by 10 when reloaded
mon_speed_inc = 7;
break;* /

case 3: // red devil
mon_HD = 5;
mon_hp = (random2((mon_HD) * 5) + (mon_HD) * 3);
mon_hp_max = mon_hp;
mon_AC = 10;//gmon_AC [2];
mon_ev = 10;//gmon_ev [2];
mon_speed = 10;
mon_speed_inc = 7;
break;

case 4: // ettin
mon_HD = 7;
mon_hp = (random2((mon_HD) * 5) + (mon_HD) * 3);
mon_hp_max = mon_hp;
mon_AC = 3;//gmon_AC [2];
mon_ev = 4;//gmon_ev [2];
mon_speed = 10;
mon_speed_inc = 7;
break;

case 5: // fungus
mon_HD = 8;
mon_hp = (random2((mon_HD) * 5) + (mon_HD) * 3);
mon_hp_max = mon_hp;
mon_AC = 1;//gmon_AC [2];
mon_ev = 0;//gmon_ev [2];
mon_speed = 0;
mon_speed_inc = 7;
break;

case 6: // goblin
mon_HD = 1;
mon_hp = (random2((mon_HD) * 4) + (mon_HD) * 2);
mon_hp_max = mon_hp;
mon_AC = 1;//gmon_AC [2];
mon_ev = 13;//gmon_ev [2];
mon_speed = 10;
mon_speed_inc = 7;
break;


case 7: // hound
mon_HD = 3;
mon_hp = (random2((mon_HD) * 5) + (mon_HD) * 3);
mon_hp_max = mon_hp;
mon_AC = 3;//gmon_AC [2];
mon_ev = 15;//gmon_ev [2];
mon_speed = 20;
mon_speed_inc = 7;
break;

case 8: // imp
mon_HD = 1;
mon_hp = (random2((mon_HD) * 10) + (mon_HD) * 10);
mon_hp_max = mon_hp;
mon_AC = 3;//gmon_AC [2];
mon_ev = 20;//gmon_ev [2];
mon_speed = 11;
mon_speed_inc = 7;
break;

case 9: // jackal
mon_HD = 1;
mon_hp = (random2((mon_HD) * 5) + (mon_HD) * 3);
mon_hp_max = mon_hp;
mon_AC = 2;//gmon_AC [2];
mon_ev = 12;//gmon_ev [2];
mon_speed = 14;
mon_speed_inc = 7;
break;

case 10: // killer bee
mon_HD = 5;
mon_hp = (random2((mon_HD) * 2) + (mon_HD) * 2);
mon_hp_max = mon_hp;
mon_AC = 2;//gmon_AC [2];
mon_ev = 18;//gmon_ev [2];
mon_speed = 20;
mon_speed_inc = 7;
break;

case 11: // killer bee larva
mon_HD = 1;
mon_hp = (random2((mon_HD) * 5) + (mon_HD) * 3);
mon_hp_max = mon_hp;
mon_AC = 1;//gmon_AC [2];
mon_ev = 4;//gmon_ev [2];
mon_speed = 5;
mon_speed_inc = 7;
break;

case 12: // manticore
mon_HD = 9;
mon_hp = (random2((mon_HD) * 5) + (mon_HD) * 3);
mon_hp_max = mon_hp;
mon_AC = 5;//gmon_AC [2];
mon_ev = 7;//gmon_ev [2];
mon_speed = 10;
mon_speed_inc = 7;
break;

case 13: // necrophage //ymph???
mon_HD = 3;
mon_hp = (random2((mon_HD) * 5) + (mon_HD) * 3);
mon_hp_max = mon_hp;
mon_AC = 2;//gmon_AC [2];
mon_ev = 10;//gmon_ev [2];
mon_speed = 10;
mon_speed_inc = 7; //      Look at this one!
break;

case 14: // orc
mon_HD = 1;
mon_hp = (random2((mon_HD) * 5) + (mon_HD) * 3);
mon_hp_max = mon_hp;
mon_AC = 0;//gmon_AC [2];
mon_ev = 10;//gmon_ev [2];
mon_speed = 10;
mon_speed_inc = 7;
break;

case 15: // phantom
mon_HD = 7;
mon_hp = (random2((mon_HD) * 5) + (mon_HD) * 3);
mon_hp_max = mon_hp;
mon_AC = 3;//gmon_AC [2];
mon_ev = 13;//gmon_ev [2];              // Look at this one!!
mon_speed = 10;
mon_speed_inc = 7;
break;

case 16: // quasit
mon_HD = 1;
mon_hp = (random2((mon_HD) * 12) + (mon_HD) * 8);
mon_hp_max = mon_hp;
mon_AC = 5;//gmon_AC [2];
mon_ev = 17;//gmon_ev [2];
mon_speed = 15;
mon_speed_inc = 7;
break;

case 17: // rat
mon_HD = 1;
mon_hp = (random2((mon_HD) * 3) + (mon_HD) * 1);
mon_hp_max = mon_hp;
mon_AC = 1;//gmon_AC [2];
mon_ev = 10;//gmon_ev [2];
mon_speed = 10;
mon_speed_inc = 7;
break;

case 18: // scorpion
mon_HD = 3;
mon_hp = (random2((mon_HD) * 5) + (mon_HD) * 3);
mon_hp_max = mon_hp;
mon_AC = 5;//gmon_AC [2];
mon_ev = 10;//gmon_ev [2];
mon_speed = 10;
mon_speed_inc = 7;
break;

case 19: // tunneling worm
mon_HD = 10;
mon_hp = (random2((mon_HD) * 5) + (mon_HD) * 5);
mon_hp_max = mon_hp;
mon_AC = 3;//gmon_AC [2];               // Look at this one!!!
mon_ev = 3;//gmon_ev [2];
mon_speed = 10;
mon_speed_inc = 7;
break;

case 20: // ugly thing
mon_HD = 4;
mon_hp = (random2((mon_HD) * 5) + (mon_HD) * 3);
mon_hp_max = mon_hp;
mon_AC = 3;//gmon_AC [2]; //            Look at this one!
mon_ev = 10;//gmon_ev [2];
mon_speed = 10;
mon_speed_inc = 7;
break;

case 21: // vulture
mon_HD = 7;
mon_hp = (random2((mon_HD) * 5) + (mon_HD) * 3);
mon_hp_max = mon_hp;
mon_AC = 10;//gmon_AC [2];
mon_ev = 10;//gmon_ev [2];      // Look at this one!!!
mon_speed = 10;
mon_speed_inc = 7;
break;

case 22: // worm
mon_HD = 4;
mon_hp = (random2((mon_HD) * 5) + (mon_HD) * 3);
mon_hp_max = mon_hp;
mon_AC = 1;//gmon_AC [2];
mon_ev = 5;//gmon_ev [2];
mon_speed = 6;
mon_speed_inc = 7;
break;

case 23: // x????
mon_HD = 7;
mon_hp = (random2((mon_HD) * 5) + (mon_HD) * 3);
mon_hp_max = mon_hp;
mon_AC = 10;//gmon_AC [2];
mon_ev = 10;//gmon_ev [2];   // Look at this one!!!
mon_speed = 10;
mon_speed_inc = 7;
if (mons_sec == 250) mons_sec = random2(15) + 1;
break;

case 24: // yellow wasp
mon_HD = 7;
mon_hp = (random2((mon_HD) * 2) + (mon_HD) * 2);
mon_hp_max = mon_hp;
mon_AC = 5;//gmon_AC [2];
mon_ev = 14;//gmon_ev [2];
mon_speed = 20;
mon_speed_inc = 7;
break;

case 25: // small zombie
if (mons_sec == 250)
{
   do
   {
      mons_sec = random2(52);
   } while (mons_zombie_size((int) mons_sec) == 0 | mons_zombie_size((int) mons_sec) == 2);
}
//mon_HD = random2(2) + 1;
mon_HD = mons_define(mons_sec, 0, 0);
mon_hp = (random2((mon_HD) * 5) + (mon_HD) * 3);
mon_hp_max = mon_hp;
mon_AC = 0;//gmon_AC [2];
mon_ev = 4;//gmon_ev [2];
mon_speed = 7;
mon_speed_inc = 7;
break;

case 26: // Angel
mon_HD = 7;
mon_hp = (random2((mon_HD) * 5) + (mon_HD) * 3);
mon_hp_max = mon_hp;
mon_AC = 10;//gmon_AC [2];
mon_ev = 10;//gmon_ev [2];      //      Look at this one!!!
mon_speed = 10;
mon_speed_inc = 7;
break;

case 27: // giant Beetle
mon_HD = 5;
mon_hp = (random2((mon_HD) * 7) + (mon_HD) * 6);
mon_hp_max = mon_hp;
mon_AC = 10;//gmon_AC [2];
mon_ev = 3;//gmon_ev [2];
mon_speed = 5;
mon_speed_inc = 7;
break;

case 28: // Cyclops
mon_HD = 9;
mon_hp = (random2((mon_HD) * 5) + (mon_HD) * 3);
mon_hp_max = mon_hp;
mon_AC = 5;//gmon_AC [2];
mon_ev = 3;//gmon_ev [2];
mon_speed = 7;
mon_speed_inc = 7;
break;

case 29: // Dragon
mon_HD = 10;
mon_hp = (random2((mon_HD) * 5) + (mon_HD) * 5);
mon_hp_max = mon_hp;
mon_AC = 10;//gmon_AC [2];
mon_ev = 8;//gmon_ev [2];
mon_speed = 10;
mon_speed_inc = 7;
break;

case 30: // two-headed Ogre
mon_HD = 5;
mon_hp = (random2((mon_HD) * 5) + (mon_HD) * 3);
mon_hp_max = mon_hp;
mon_AC = 1;//gmon_AC [2];
mon_ev = 4;//gmon_ev [2];       // Look at this one!!!
mon_speed = 8;
mon_speed_inc = 7;
break;

case 31: // Fiend
mon_HD = 11;
mon_hp = (random2((mon_HD) * 5) + (mon_HD) * 3);
mon_hp_max = mon_hp;
mon_AC = 15;//gmon_AC [2];
mon_ev = 6;//gmon_ev [2];
mon_speed = 10;
mon_speed_inc = 7;
break;


case 32: // Giant spore
mon_HD = 1;
mon_hp = 1;//(random2((mon_HD) * 5) + (mon_HD) * 3);
mon_hp_max = mon_hp;
mon_AC = 0;//gmon_AC [2];
mon_ev = 10;//gmon_ev [2];
mon_speed = 7;
mon_speed_inc = 7;
break;

case 33: // Hobgoblin
mon_HD = 1;
mon_hp = (random2((mon_HD) * 7) + (mon_HD) * 3);
mon_hp_max = mon_hp;
mon_AC = 0;//gmon_AC [2];
mon_ev = 10;//gmon_ev [2];
mon_speed = 10;
mon_speed_inc = 7;
break;

case 34: // Ice beast
mon_HD = 5;
mon_hp = (random2((mon_HD) * 5) + (mon_HD) * 3);
mon_hp_max = mon_hp;
mon_AC = 5;//gmon_AC [2];
mon_ev = 10;//gmon_ev [2];
mon_speed = 10;
mon_speed_inc = 7;
break;

case 35: // Jelly
mon_HD = 4;
mon_hp = (random2((mon_HD) * 5) + (mon_HD) * 5);
mon_hp_max = mon_hp;
mon_AC = 0;//gmon_AC [2];
mon_ev = 2;//gmon_ev [2];
mon_speed = 5;
mon_speed_inc = 7;
break;
// ^^^ Don't change without also changing jelly_divide in monspls3

case 36: // Kobold
mon_HD = 1;
mon_hp = (random2((mon_HD) * 4) + (mon_HD) * 2);
mon_hp_max = mon_hp;
mon_AC = 2;//gmon_AC [2];
mon_ev = 14;//gmon_ev [2];
mon_speed = 10;
mon_speed_inc = 7;
break;

case 37: // Lich
mon_HD = 20;
mon_hp = (random2((mon_HD) * 4) + (mon_HD) * 2);
mon_hp_max = mon_hp;
mon_AC = 13;//gmon_AC [2];
mon_ev = 15;//gmon_ev [2];
mon_speed = 10;
mon_speed_inc = 7;
if (mons_sec == 250) // I don't see why it wouldn't be
{
 mons_sec = random2(4) + 20;
}
break;

case 38: // Mummy
mon_HD = 3;
mon_hp = (random2((mon_HD) * 2) + (mon_HD) * 6);
mon_hp_max = mon_hp;
mon_AC = 3;//gmon_AC [2];
mon_ev = 6;//gmon_ev [2];
mon_speed = 10;
mon_speed_inc = 7;
break;

case 39: // Naga
mon_HD = 6;
mon_hp = (random2((mon_HD) * 5) + (mon_HD) * 3);
mon_hp_max = mon_hp;
mon_AC = 6;//gmon_AC [2];
mon_ev = 14;//gmon_ev [2];
mon_speed = 15;
mon_speed_inc = 7;
mons_sec = 10;
break;

case 40: // Ogre
mon_HD = 4;
mon_hp = (random2((mon_HD) * 5) + (mon_HD) * 3);
mon_hp_max = mon_hp;
mon_AC = 1;//gmon_AC [2];
mon_ev = 6;//gmon_ev [2];
mon_speed = 10;
mon_speed_inc = 7;
break;

case 41: // Plant
mon_HD = 10;
mon_hp = (random2((mon_HD) * 5) + (mon_HD) * 3);
mon_hp_max = mon_hp;
mon_AC = 10;//gmon_AC [2];
mon_ev = 0;//gmon_ev [2];
mon_speed = 0;
mon_speed_inc = 7;
break;

case 42: // Q something
mon_HD = 7;
mon_hp = (random2((mon_HD) * 5) + (mon_HD) * 3);
mon_hp_max = mon_hp;
mon_AC = 10;//gmon_AC [2];
mon_ev = 10;//gmon_ev [2];      // Look!!!
mon_speed = 10;
mon_speed_inc = 7;
break;

case 43: // Raksasha
mon_HD = 4;
mon_hp = (random2((mon_HD) * 4) + (mon_HD) * 3);
mon_hp_max = mon_hp;
mon_AC = 6;//gmon_AC [2];
mon_ev = 10;//gmon_ev [2];
mon_speed = 10;
mon_speed_inc = 7;
break;

case 44: // Snake
mon_HD = 2;
mon_hp = (random2((mon_HD) * 5) + (mon_HD) * 3);
mon_hp_max = mon_hp;
mon_AC = 1;//gmon_AC [2];
mon_ev = 15;//gmon_ev [2];      // Look!!!
mon_speed = 13;
mon_speed_inc = 7;
break;

case 45: // Troll
mon_HD = 5;
mon_hp = (random2((mon_HD) * 5) + (mon_HD) * 3);
mon_hp_max = mon_hp;
mon_AC = 3;//gmon_AC [2];
mon_ev = 10;//gmon_ev [2];
mon_speed = 10;
mon_speed_inc = 7;
break;

case 46: // Unseen horror
mon_HD = 7;
mon_hp = (random2((mon_HD) * 5) + (mon_HD) * 3);
mon_hp_max = mon_hp;
mon_AC = 5;//gmon_AC [2];
mon_ev = 10;//gmon_ev [2];      // Look!!!
mon_speed = 30;
mon_speed_inc = 7;
break;

case 47: // Vampire
mon_HD = 6;
mon_hp = (random2((mon_HD) * 5) + (mon_HD) * 3);
mon_hp_max = mon_hp;
mon_AC = 10;//gmon_AC [2];
mon_ev = 10;//gmon_ev [2];
mon_speed = 10;
mon_speed_inc = 7;
mons_sec = 40;
break;

case 48: // Wraith
mon_HD = 6;
mon_hp = (random2((mon_HD) * 5) + (mon_HD) * 3);
mon_hp_max = mon_hp;
mon_AC = 10;//gmon_AC [2];
mon_ev = 10;//gmon_ev [2];
mon_speed = 10;
mon_speed_inc = 7;
break;

case 49: // Abomination
mon_HD = 5 + random2(10);
mon_hp = (random2((mon_HD) * 5) + (mon_HD) * 3);
mon_hp_max = mon_hp;
mon_AC = random2(15);//gmon_AC [2];
mon_ev = random2(15);//gmon_ev [2];
mon_speed = 5 + random2(6) + random2(6);
mon_speed_inc = 7;
if (mons_sec == 250) mons_sec = random2(15) + 1;
break;

case 50: // Yak
mon_HD = 5;
mon_hp = (random2((mon_HD) * 5) + (mon_HD) * 3);
mon_hp_max = mon_hp;
mon_AC = 4;//gmon_AC [2];
mon_ev = 7;//gmon_ev [2];
mon_speed = 10;
mon_speed_inc = 7;
break;

case 51: // Large Zombie
mon_HD = random2(5) + 3;
mon_hp = (random2((mon_HD) * 5) + (mon_HD) * 3);
mon_hp_max = mon_hp;
mon_AC = 8;//gmon_AC [2];
mon_ev = 5;//gmon_ev [2];
mon_speed = 6;
mon_speed_inc = 7;
if (mons_sec == 250)
{
   do
   {
      mons_sec = random2(52);
   } while (mons_zombie_size((int) mons_sec) == 0 | mons_zombie_size((int) mons_sec) == 1);
}
break;

case 52: // orc warrior
mon_HD = 4;
mon_hp = (random2((mon_HD) * 8) + (mon_HD) * 3);
mon_hp_max = mon_hp;
mon_AC = 1;//gmon_AC [2];
mon_ev = 13;//gmon_ev [2];
mon_speed = 12;
mon_speed_inc = 7;
break;

case 53: // kobold demonologist
mon_HD = 3;
mon_hp = (random2((mon_HD) * 6) + (mon_HD) * 4);
mon_hp_max = mon_hp;
mon_AC = 2;//gmon_AC [2];
mon_ev = 14;//gmon_ev [2];
mon_speed = 10;
mon_speed_inc = 7;
break;

case 54: // orc wizard
mon_HD = 4;
mon_hp = (random2((mon_HD) * 6) + (mon_HD) * 2);
mon_hp_max = mon_hp;
mon_AC = 1;//gmon_AC [2];
mon_ev = 12;//gmon_ev [2];
mon_speed = 10;
mon_speed_inc = 7;
if (mons_sec == 250) // I don't see why it wouldn't be
{
        mons_sec = random2(3);
}
break;

case 55: // orc knight
mon_HD = 8;
mon_hp = (random2((mon_HD) * 8) + (mon_HD) * 3);
mon_hp_max = mon_hp;
mon_AC = 2;//gmon_AC [2];
mon_ev = 13;//gmon_ev [2];
mon_speed = 12;
mon_speed_inc = 7;
break;

case 56: // tunneling worm tail section
mon_HD = 10;
mon_hp = (random2((mon_HD) * 5) + (mon_HD) * 5);
mon_hp_max = mon_hp;
mon_AC = 3;//gmon_AC [2];               // Look at this one!!!
mon_ev = 3;//gmon_ev [2];
mon_speed = 10;
mon_speed_inc = 7;
break;


case 57: // Wyvern (D)
mon_HD = 4;
mon_hp = (random2((mon_HD) * 5) + (mon_HD) * 3);
mon_hp_max = mon_hp;
mon_AC = 5;//gmon_AC [2];
mon_ev = 10;//gmon_ev [2];
mon_speed = 15;
mon_speed_inc = 7;
break;

case 58: // Kobold lord
mon_HD = 5;
mon_hp = (random2((mon_HD) * 4) + (mon_HD) * 2);
mon_hp_max = mon_hp;
mon_AC = 3;//gmon_AC [2];
mon_ev = 14;//gmon_ev [2];
mon_speed = 10;
mon_speed_inc = 7;
break;

case 59: // giant eyeball (G)
mon_HD = 3;
mon_hp = (random2((mon_HD) * 5) + (mon_HD) * 8);
mon_hp_max = mon_hp;
mon_AC = 0;
mon_ev = 1;
mon_speed = 3;
mon_speed_inc = 7;
break;

case 60: // Wight
mon_HD = 3;
mon_hp = (random2((mon_HD) * 5) + (mon_HD) * 3);
mon_hp_max = mon_hp;
mon_AC = 4;//gmon_AC [2];
mon_ev = 10;//gmon_ev [2];
mon_speed = 10;
mon_speed_inc = 7;
break;

case 61: // oklob Plant
mon_HD = 10;
mon_hp = (random2((mon_HD) * 5) + (mon_HD) * 3);
mon_hp_max = mon_hp;
mon_AC = 10;//gmon_AC [2];
mon_ev = 0;//gmon_ev [2];
mon_speed = 10;
mon_speed_inc = 7;
break;

case 62: // wolf Spider
mon_HD = 8;
mon_hp = (random2((mon_HD) * 5) + (mon_HD) * 3);
mon_hp_max = mon_hp;
mon_AC = 3;//gmon_AC [2];
mon_ev = 10;//gmon_ev [2];
mon_speed = 15;
mon_speed_inc = 7;
break;

case 63: // shadow
mon_HD = 3;
mon_hp = (random2((mon_HD) * 5) + (mon_HD) * 3);
mon_hp_max = mon_hp;
mon_AC = 12;//gmon_AC [2];
mon_ev = 10;//gmon_ev [2];
mon_speed = 10;
mon_speed_inc = 7;
break;

case 64: // hungry ghost (p)
mon_HD = 7;
mon_hp = (random2((mon_HD) * 5) + (mon_HD) * 3);
mon_hp_max = mon_hp;
mon_AC = 0;//gmon_AC [2];
mon_ev = 17;//gmon_ev [2];              // Look at this one!!
mon_speed = 10;
mon_speed_inc = 7;
break;

case 65: // eye of draining (G)
mon_HD = 7;
mon_hp = (random2((mon_HD) * 5) + (mon_HD) * 8);
mon_hp_max = mon_hp;
mon_AC = 3;
mon_ev = 1;
mon_speed = 5;
mon_speed_inc = 7;
break;

case 66: // butterfly
mon_HD = 1;
mon_hp = (random2((mon_HD) * 5) + (mon_HD) * 3);
mon_hp_max = mon_hp;
mon_AC = 0;
mon_ev = 25;
mon_speed = 25;
mon_speed_inc = 7;
mons_sec = random2(15) + 1;
break;

case 67: // wandering fungus
mon_HD = 8;
mon_hp = (random2((mon_HD) * 5) + (mon_HD) * 3);
mon_hp_max = mon_hp;
mon_AC = 5;//gmon_AC [2];
mon_ev = 0;//gmon_ev [2];
mon_speed = 10;
mon_speed_inc = 7;
break;

case 68: // Efreet
mon_HD = 7;
mon_hp = (random2((mon_HD) * 5) + (mon_HD) * 3);
mon_hp_max = mon_hp;
mon_AC = 10;//gmon_AC [2];
mon_ev = 5;//gmon_ev [2];
mon_speed = 15;
mon_speed_inc = 7;
break;


case 80: // burning devil
mon_HD = 7;
mon_hp = (random2((mon_HD) * 5) + (mon_HD) * 3);
mon_hp_max = mon_hp;
mon_AC = 5;//gmon_AC [2];
mon_ev = 10;//gmon_ev [2];
mon_speed = 10;
mon_speed_inc = 7;
mons_sec = RED;
break;

case 81: // rotting devil
mon_HD = 5;
mon_hp = (random2((mon_HD) * 5) + (mon_HD) * 3);
mon_hp_max = mon_hp;
mon_AC = 2;//gmon_AC [2];
mon_ev = 10;//gmon_ev [2];
mon_speed = 10;
mon_speed_inc = 7;
break;

case 82: // yellow devil
mon_HD = 7;
mon_hp = (random2((mon_HD) * 5) + (mon_HD) * 3);
mon_hp_max = mon_hp;
mon_AC = 12;//gmon_AC [2];
mon_ev = 12;//gmon_ev [2];
mon_speed = 13;
mon_speed_inc = 7;
break;

case 83: // skeletal devil
mon_HD = 8;
mon_hp = (random2((mon_HD) * 5) + (mon_HD) * 3);
mon_hp_max = mon_hp;
mon_AC = 15;
mon_ev = 10;
mon_speed = 10;
mon_speed_inc = 7;
break;

case 84: // shadowy devil
mon_HD = 11;
mon_hp = (random2((mon_HD) * 5) + (mon_HD) * 3);
mon_hp_max = mon_hp;
mon_AC = 18;//gmon_AC [2];
mon_ev = 10;//gmon_ev [2];
mon_speed = 10;
mon_speed_inc = 7;
break;

case 85: // hairy devil
mon_HD = 6;
mon_hp = (random2((mon_HD) * 5) + (mon_HD) * 3);
mon_hp_max = mon_hp;
mon_AC = 7;//gmon_AC [2];
mon_ev = 10;//gmon_ev [2];
mon_speed = 10;
mon_speed_inc = 7;
break;

case 86: // ice devil
mon_HD = 11;
mon_hp = (random2((mon_HD) * 5) + (mon_HD) * 3);
mon_hp_max = mon_hp;
mon_AC = 12;//gmon_AC [2];
mon_ev = 10;//gmon_ev [2];
mon_speed = 10;
mon_speed_inc = 7;
break;

case 87: // blue devil
mon_HD = 7;
mon_hp = (random2((mon_HD) * 5) + (mon_HD) * 3);
mon_hp_max = mon_hp;
mon_AC = 14;//gmon_AC [2];
mon_ev = 10;//gmon_ev [2];
mon_speed = 10;
mon_speed_inc = 7;
break;


case MLAVA0: // lava worm
mon_HD = 6;
mon_hp = (random2((mon_HD) * 5) + (mon_HD) * 3);
mon_hp_max = mon_hp;
mon_AC = 1;//gmon_AC [2];
mon_ev = 10;//gmon_ev [2];
mon_speed = 10;
mon_speed_inc = 7;
break;

case MLAVA1: // lava fish
mon_HD = 4;
mon_hp = (random2((mon_HD) * 5) + (mon_HD) * 3);
mon_hp_max = mon_hp;
mon_AC = 4;//gmon_AC [2];
mon_ev = 15;//gmon_ev [2];
mon_speed = 10;
mon_speed_inc = 7;
break;

case MLAVA2: // lava snake
mon_HD = 3;
mon_hp = (random2((mon_HD) * 5) + (mon_HD) * 3);
mon_hp_max = mon_hp;
mon_AC = 2;//gmon_AC [2];
mon_ev = 17;//gmon_ev [2];
mon_speed = 10;
mon_speed_inc = 7;
break;



case MWATER0: // giant fish
mon_HD = 4;
mon_hp = (random2((mon_HD) * 5) + (mon_HD) * 3);
mon_hp_max = mon_hp;
mon_AC = 1;//gmon_AC [2];
mon_ev = 12;//gmon_ev [2];
mon_speed = 10;
mon_speed_inc = 7;
break;

case MWATER1: // giant goldfish
mon_HD = 7;
mon_hp = (random2((mon_HD) * 5) + (mon_HD) * 3);
mon_hp_max = mon_hp;
mon_AC = 5;//gmon_AC [2];
mon_ev = 7;//gmon_ev [2];
mon_speed = 10;
mon_speed_inc = 7;
break;

case MWATER2: // electronic eel
mon_HD = 3;
mon_hp = (random2((mon_HD) * 5) + (mon_HD) * 3);
mon_hp_max = mon_hp;
mon_AC = 1;//gmon_AC [2];
mon_ev = 15;//gmon_ev [2];
mon_speed = 10;
mon_speed_inc = 7;
break;

case MWATER3: // jellyfish
mon_HD = 4;
mon_hp = (random2((mon_HD) * 5) + (mon_HD) * 3);
mon_hp_max = mon_hp;
mon_AC = 0;//gmon_AC [2];
mon_ev = 5;//gmon_ev [2];
mon_speed = 10;
mon_speed_inc = 7;
break;

// demon butterfly


} // end of switch (mclss)

/*
glog [0] = mon_HD;
glog [1] = mon_hp;
glog [2] = mon_hp_max;
glog [3] = mon_AC;
glog [4] = mon_ev;
glog [5] = mon_speed;
glog [6] = mon_speed_inc;

free(glog);

return glog;
* /

switch(chrct)
{
case 0: return mon_HD;
case 1: return mon_hp;
case 2: return mon_hp_max;
case 3: return mon_AC;
case 4: return mon_ev;
case 5: return mon_speed;
//case 6: return mon_speed_inc;
case 6: return 70 + random2(10);
case 7: return mons_sec;
}

return 255;

} // end of mons_define

*/








char new_game(char char_name [8])
{
int handle;
unsigned char rm [69];
int temp_int = 0;
long temp_long;
char your_nam [50];
unsigned char xhaste = 0;
unsigned char xmight = 0;
unsigned char xlev = 0;
unsigned char xpoison = 0;
unsigned char xcan_see = 1;
unsigned int xhunger = 6000;
char xhunger_inc = 4; // ????

int xres_magic = 0;

char xitem_wielded = -1;
char xbody_armour = -1;
char xhead_armour = -1;
char xcloak_armour = -1;
char xhand_armour = -1;
char xfoot_armour = -1;
char xshield_armour = -1;
char xring [2];

//char xring [1] = -1;
int xhp = 0;
int xhp_max = 0;
unsigned char xep = 0;
unsigned char xep_max = 0;

char xstrength = 0;
char xintel = 0;
char xdex = 0;
char xmax_strength = 0;
char xmax_intel = 0;
char xmax_dex = 0;

char xAC = 13; // Remember that AC shown is 10 - AC, but AC still can be -ve.
unsigned char xevasion = 10;
unsigned char xdamage = 2;
unsigned char xnat_damage = 2;

//float xrate_regen = 0.06;
//float xincr_regen = 0;
//long xxp = 0;
int xxp = 0;
unsigned char xxl = 1;
int xgp = 0;
int xf_abil = 2;
int xmag_abil = 10;
int xthr_abil = 10;
//float xspeed = 1;
char xspeed = 10;
float xspeed_inc = 0;
int xspell_levels = 0;

int xx_pos = 50;
int xy_pos = 50;
char xclasnam [13] = "Hello!";
int xburden = 0;

//unsigned char xinv_name [52] [30];
unsigned char xinv_class [52];
unsigned char xinv_type [52];
char xinv_plus [52];
unsigned char xinv_dam [52];
unsigned char xinv_col [52];
unsigned char xinv_quant [52];
unsigned char xinv_ident [52];
int p;
char xclas;
char keyn;

unsigned char xthing_quant [6];
char char_fil [50];

char lev_ex [30]; // do these levels exist?

int i;
char tm [10] [30];

char kh;

xring [0] = -1;
xring [1] = -1;

unsigned char char_num = 0;

char j = 0;

unsigned char xitem_name [5] [30];



for (i = 0; i < 30; i ++)
{
        lev_ex [i] = 0;
}


// Have to add resistances etc to this!!!

for (i = 0; i < 52; i ++)
{
        xinv_quant [i] = 0;
        xinv_ident [i] = 3;
}


//window(1,1,80,25);
//gotoxy(1,1);
textcolor(7);
//clrscr();

cprintf("\n\rHello, and welcome to Dungeon Crawl v1.05!");
cprintf("\n\r(Copyright 1997 Linley Henzell)");
cprintf("\n\rPlease read Crawl.txt for instructions and legal details.\n\r\n\r");
name_q : cprintf("What is your name today? ");
//cin >> your_nam;
gets(your_nam);

if (strcmp(your_nam, "bones") == 0 | strlen(your_nam) == 0)
{
        cprintf("\n\rThat's a silly name!\n\r");
        goto name_q;
}

strncpy(char_name, your_nam, 8);

//strcpy(char_fil, "c:\\borlandc\\crawl\\levels\\");
strcpy(char_fil, "");
strcat(char_fil, char_name);
strcat(char_fil, ".sav");

//handle = open(char_fil, O_RDONLY, S_IREAD);


        int sysg = 0;

 int passout = 0;

        //< "a";
        //sysg = system("cd c:\borlandc\crawl\levels");
        //cout << "b";

        if (sysg == -1)
        {
                perror("Oh dear... ");
                exit(0);
        }



handle = open(char_fil, S_IWRITE, S_IREAD);


if (handle != -1)
{
        cprintf("\n\rWelcome back, ");
        cprintf(your_nam);
        cprintf("!");
        close(handle);



        int sysg = 0;

        //sysg = system("cd c:\borlandc\crawl");

        if (sysg == -1)
        {
                perror("Oh dear... ");
                exit(0);
        }


        return 0;
}
close(handle);

/*


//handle = open("control.dat", S_IWRITE, S_IREAD);

handle = open("control.dat", S_IREAD | O_RDONLY);

j = 0;
for (i = 0; i < 10; i ++)
{
read(handle, rm, 30);
        cout << rm << "\n\r";
        if (stricmp(rm, your_nam) == 0)
        {
                cprintf("\n\rWelcome back, ");
                cprintf(your_nam);
                cprintf("!");
                close(handle);
                return i + 10;
        }
if (strcmpi(rm, "deceased") != 0)
{
        j ++;
} else char_num = i;

}

if (j == 10)
{
        cprintf("\n\rI'm sorry, we're full right now. Please come back when somebody has moved on.");
        close(handle);
        exit(0);
}

close(handle); // control.dat





*/



/*


kh = getch();










j = 0;
for (i = 0; i < 10; i ++)
{
read(handle, rm, 30);
        cout << rm << "\n\r";
        if (strcmpi(rm, your_nam) == 0)
        {
                cprintf("\n\rWelcome back, ");
                cprintf(your_nam);
                cprintf("!");
                close(handle);
                return i + 10;
        }
if (strcmpi(rm, "deceased") != 0)
{
        j ++;
} else char_num = i;

}

if (j == 10)
{
        cprintf("\n\rI'm sorry, we're full right now. Please come back when somebody has moved on.");
        close(handle);
        exit(0);
}

close(handle); // control.dat


*/



//if (handle == -1)             create it, then! (do later)


//if (handle != -1)
//{
//}




/*
j = 0;

for (i = 0; i < 5; i ++)
{
        read(handle, rm, 50);
        if (strcmp(rm, your_nam) == 0)
        {
                cprintf("\n\rWelcome back, ");
                cprintf(your_nam);
                cprintf("!");
                close(handle);
                return i + 10;
        }
        if (strcmp(rm, "deceased") == 0) j ++;

}

if (j == 5)
{
        cprintf("Sorry, there isn't any room for you right now!

}

//close(handle);
*/


cprintf("\n\r\n");
cprintf("You must be new here!\n\r");
cprintf("You can be a Fighter, a Priest, a Thief, or a Wizard (q to quit).\n\r");
query : cprintf("What kind of character are you? ");
keyn = getche();

//cprintf("got through getche!\n\r");

if (keyn == 'F' | keyn == 'f') xclas = 0;
        else if (keyn == 'W' | keyn == 'w') xclas = 1;
  else if (keyn == 'P' | keyn == 'p') xclas = 2;
  else if (keyn == 'T' | keyn == 't') xclas = 3;
  else if (keyn == 'Q' | keyn == 'q')
   {
    cprintf("\n\rGoodbye!");
    exit(0);
   }
                else
                {
                        cprintf("\n\rI'm sorry, you can't be that. ");
                        goto query;
                }

//              cprintf("got through I'm sorry bit\n\r.");


        for (i = 0; i < 52; i ++)
        {
                xinv_quant [i] = 0;
                xinv_class [i] = 0;
                xinv_type [i] = 0;
                xinv_plus [i] = 0;
                xinv_dam [i] = 0;
                xinv_col [i] = 0;
        }


//cprintf("Okay, then.");

int wtype = 0;
int etype = 0;
int ctype = 0; // the first letters are completely arbitrary

switch(xclas)
{
case 0: // fighter
        strcpy(xclasnam, "Fighter");
        xhp = 15; xhp_max = 15;
        xep = 0; xep_max = 0;
        xf_abil = 10;
        xmag_abil = 5;
 xthr_abil = 10;
 xspeed = 10;
 xspell_levels = 0;

        xinv_quant [0] = 1;
        xinv_class [0] = 0;
        //switch(random2(6))
        //{
        //      case 0 : xinv_type [0] = 1; xdamage = 6; break;
        //      case 1 : xinv_type [0] = 2; xdamage = 7; break;
        //      case 2 : xinv_type [0] = 4; xdamage = 7; break;
        //      case 3 : xinv_type [0] = 9; xdamage = 7; break;
                //case 4 :
                xinv_type [0] = 5; xdamage = 6; //break;
  if (random2(4) == 0) xinv_type [0] = 1;
  if (random2(4) == 0) xinv_type [0] = 9;
                //case 5 : xinv_type [0] = 5; xdamage = 6; break;
        //}


        xinv_plus [0] = 50;
        xinv_dam [0] = 0;
        xinv_col [0] = LIGHTCYAN;

//      switch(3)//random2(4))
//      {
//              case 0 : ctype = 0; wtype = 11; etype = 2 + random2(2); break;
//              case 1 : ctype = 0; wtype = 9; etype = 2 + random2(3); break;
//              case 2 : ctype = 0; wtype = 3; etype = 2 + random2(3); break;
//              case 3 : ctype = 1; wtype = 3; etype = 2; break;
//      }

        //etype = 18;

/*      for (i = 1; i < etype; i ++)
        {
                if (ctype == 1) xinv_quant [i] = random2(6) + 5;
                        else xinv_quant [i] = 1;
                xinv_class [i] = ctype;
                xinv_type [i] = 3; //wtype;
                xinv_plus [i] = 50;
                xinv_dam [i] = 0;
                xinv_col [i] = LIGHTCYAN;
        }
*/

/*  xinv_quant [1] = random2(6) + 5;
                xinv_class [1] = 1;
                xinv_type [1] = 3; //wtype;
                xinv_plus [1] = 50;
                xinv_dam [1] = 0;
                xinv_col [1] = LIGHTCYAN;*/

        // Ring mail
        xinv_quant [1] = 1;
        xinv_class [1] = 2;
        xinv_type [1] = 2;
        xinv_plus [1] = 50;
        xinv_dam [1] = 0;
        xinv_col [1] = LIGHTCYAN;

        xinv_quant [2] = 1;
        xinv_class [2] = 2;
        xinv_type [2] = 8;
        xinv_plus [2] = 50;
        xinv_dam [2] = 0;
        xinv_col [2] = LIGHTCYAN;

        xAC = 3;
        xevasion = 9;

        // 11 = spear
        // 9 = hand axe
        // 3 = dagger

//      } // end of switch random2(4)

//      int leftover = 8;

        xstrength = 12;
        xdex = 10;
        xintel = 6;

        for (i = 0; i < 8; i ++)
        {
        switch(random2(3))
        {
                case 0 : if (xstrength > 18)
                {
                        i --;
                } else xstrength ++; break;
                case 1 : xdex ++; break;
                case 2 : xintel ++; break;
        }

        } // end of for i

        xitem_wielded = 0;
        xbody_armour = 1;
 xshield_armour = 2;

        xgp = random2(10);
 xres_magic = 3;

       //       */
break;

case 1: // wizard
        strcpy(xclasnam, "Wizard");
        xhp = 10; xhp_max = 10;
        xep = 3; xep_max = 3;
        xf_abil = 5;
        xmag_abil = 10;
 xthr_abil = 5;
 xspeed = 10;
 xspell_levels = 2;

        xinv_quant [0] = 1;
        xinv_class [0] = 0;
        //switch(random2(6))
        //{
        //      case 0 : xinv_type [0] = 1; xdamage = 6; break;
        //      case 1 : xinv_type [0] = 2; xdamage = 7; break;
        //      case 2 : xinv_type [0] = 4; xdamage = 7; break;
        //      case 3 : xinv_type [0] = 9; xdamage = 7; break;
                //case 4 :
                xinv_type [0] = 3; //xdamage = 6; //break;
                //case 5 : xinv_type [0] = 5; xdamage = 6; break;
        //}


        xinv_plus [0] = 50;
        xinv_dam [0] = 0;
        xinv_col [0] = LIGHTCYAN;

        // Robe
        xinv_quant [1] = 1;
        xinv_class [1] = 2;
        xinv_type [1] = 0;
        xinv_plus [1] = 50;
        xinv_dam [1] = 1 * 30;
 if (random2(3) != 0) xinv_dam [1] = (4 + random2(2)) * 30;
        xinv_col [1] = random2(15) + 1;

        xAC = 1;
        xevasion = 10;

        // 11 = spear
        // 9 = hand axe
        // 3 = dagger

//      } // end of switch random2(4)

//      int leftover = 8;

        xstrength = 6;
        xdex = 10;
        xintel = 12;

        for (i = 0; i < 8; i ++)
        {
        switch(random2(3))
        {
                case 0 : if (xintel > 18)
                {
                        i --;
                } else xintel ++; break;
                case 1 : xdex ++; break;
                case 2 : xstrength ++; break;
        }

        } // end of for i

        xitem_wielded = 0;
        xbody_armour = 1;

        xgp = random2(10);

        // extra items being tested:


/*
        xinv_class [22] = 7;
        xinv_type [22] = 20;//16;//18;//2;//8;
//      strcpy(xinv_name [22], "ring of invisibility");
        xinv_quant [22] = 1;
        xinv_plus [22] = 0;
        // xinv_no = 13;
        xinv_col [22] = 3;

        xinv_class [23] = 3;
        xinv_type [23] = 11;
//      strcpy(xinv_name [23], "wand of invisibility");
        xinv_quant [23] = 1;
        xinv_plus [23] = 50;
        xinv_dam [23] = 5;
        // xinv_no = 13;
        xinv_col [23] = 3;


        xinv_class [24] = 8;
        xinv_type [24] = 13;
//      strcpy(xinv_name [23], "wand of invisibility");
        xinv_quant [24] = 15;
        xinv_plus [24] = 0;
        xinv_dam [24] = 5;
        // xinv_no = 13;
        xinv_col [24] = 3;
*/
        xinv_class [25] = 10;
        xinv_type [25] = random2(4);
//      strcpy(xinv_name [23], "wand of invisibility");
        xinv_quant [25] = 1;
        xinv_plus [25] = 127;
        xinv_dam [25] = 5;
        // xinv_no = 13;
        xinv_col [25] = 3;




 xres_magic = 10;

       //       */




/*      xhp = 7; xhp_max = 7;
        xep = 3; xep_max = 3;
        xf_abil = 70;
        xmag_abil = 15;
        xspeed = 1;*/
break;


case 2: // priest
        strcpy(xclasnam, "Priest");
        xhp = 12; xhp_max = 12;
        xep = 1; xep_max = 1;
        xf_abil = 7;
        xmag_abil = 7;
 xthr_abil = 5;
 xspeed = 10;
 xspell_levels = 0;

        xinv_quant [0] = 1;
        xinv_class [0] = 0;
        //switch(random2(6))
        //{
        //      case 0 : xinv_type [0] = 1; xdamage = 6; break;
        //      case 1 : xinv_type [0] = 2; xdamage = 7; break;
        //      case 2 : xinv_type [0] = 4; xdamage = 7; break;
        //      case 3 : xinv_type [0] = 9; xdamage = 7; break;
                //case 4 :
 xinv_type [0] = 1;
                //case 5 : xinv_type [0] = 5; xdamage = 6; break;
        //}


        xinv_plus [0] = 50;
        xinv_dam [0] = 0;
        xinv_col [0] = LIGHTCYAN;

//      switch(3)//random2(4))
//      {
//              case 0 : ctype = 0; wtype = 11; etype = 2 + random2(2); break;
//              case 1 : ctype = 0; wtype = 9; etype = 2 + random2(3); break;
//              case 2 : ctype = 0; wtype = 3; etype = 2 + random2(3); break;
//              case 3 : ctype = 1; wtype = 3; etype = 2; break;
//      }

        //etype = 18;

/*      for (i = 1; i < etype; i ++)
        {
                if (ctype == 1) xinv_quant [i] = random2(6) + 5;
                        else xinv_quant [i] = 1;
                xinv_class [i] = ctype;
                xinv_type [i] = 3; //wtype;
                xinv_plus [i] = 50;
                xinv_dam [i] = 0;
                xinv_col [i] = LIGHTCYAN;
        }
*/


        // Robe
        xinv_quant [1] = 1;
        xinv_class [1] = 2;
        xinv_type [1] = 0;
        xinv_plus [1] = 50;
        xinv_dam [1] = 0;
        xinv_col [1] = WHITE;

        xAC = 1;
        xevasion = 10;

        xinv_class [2] = 8;
        xinv_type [2] = 0;
//      strcpy(xinv_name [23], "wand of invisibility");
        xinv_quant [2] = 2 + random2(2);
        xinv_plus [2] = 0;
        xinv_dam [2] = 0;
        // xinv_no = 13;
        xinv_col [2] = random2(15) + 1; // hmmm...


        // 11 = spear
        // 9 = hand axe
        // 3 = dagger

//      } // end of switch random2(4)

//      int leftover = 8;

        xstrength = 10;
        xdex = 8;
        xintel = 10;

        for (i = 0; i < 8; i ++)
        {
        switch(random2(3))
        {
                case 0 : if (xstrength > 18)
                {
                        i --;
                } else xstrength ++; break;
                case 1 : xdex ++; break;
                case 2 : xintel ++; break;
        }

        } // end of for i

        xitem_wielded = 0;
        xbody_armour = 1;
// xshield_armour = 3;

        xgp = random2(10);
 xres_magic = 10;
       //       */
break;



case 3: // thief
        strcpy(xclasnam, "Thief");
        xhp = 11; xhp_max = 11;
        xep = 0; xep_max = 0;
        xf_abil = 7;
        xmag_abil = 6;
 xthr_abil = 12;
 xspeed = 10;
 xspell_levels = 0;

        xinv_quant [0] = 1;
        xinv_class [0] = 0;
        //switch(random2(6))
        //{
        //      case 0 : xinv_type [0] = 1; xdamage = 6; break;
        //      case 1 : xinv_type [0] = 2; xdamage = 7; break;
        //      case 2 : xinv_type [0] = 4; xdamage = 7; break;
        //      case 3 : xinv_type [0] = 9; xdamage = 7; break;
                //case 4 :
                xinv_type [0] = 3;// xdamage = 6; //break;
//  if (random2(4) == 0) xinv_type [0] = 1;
//  if (random2(4) == 0) xinv_type [0] = 9;
                //case 5 : xinv_type [0] = 5; xdamage = 6; break;
        //}


        xinv_plus [0] = 50;
        xinv_dam [0] = 0;
        xinv_col [0] = LIGHTCYAN;

//      switch(3)//random2(4))
//      {
//              case 0 : ctype = 0; wtype = 11; etype = 2 + random2(2); break;
//              case 1 : ctype = 0; wtype = 9; etype = 2 + random2(3); break;
//              case 2 : ctype = 0; wtype = 3; etype = 2 + random2(3); break;
//              case 3 : ctype = 1; wtype = 3; etype = 2; break;
//      }

        //etype = 18;

/*      for (i = 1; i < etype; i ++)
        {
                if (ctype == 1) xinv_quant [i] = random2(6) + 5;
                        else xinv_quant [i] = 1;
                xinv_class [i] = ctype;
                xinv_type [i] = 3; //wtype;
                xinv_plus [i] = 50;
                xinv_dam [i] = 0;
                xinv_col [i] = LIGHTCYAN;
        }
*/

  xinv_quant [1] = random2(6) + 5;
                xinv_class [1] = 1;
                xinv_type [1] = 3; //wtype;
                xinv_plus [1] = 50;
                xinv_dam [1] = 0;
                xinv_col [1] = LIGHTCYAN;

        // Leather armour
        xinv_quant [2] = 1;
        xinv_class [2] = 2;
        xinv_type [2] = 1;
        xinv_plus [2] = 50;
        xinv_dam [2] = 0;
        xinv_col [2] = BROWN;


 xAC = 2;
        xevasion = 9;

        // 11 = spear
        // 9 = hand axe
        // 3 = dagger

//      } // end of switch random2(4)

//      int leftover = 8;

        xstrength = 8;
        xdex = 12;
        xintel = 8;

        for (i = 0; i < 8; i ++)
        {
        switch(random2(3))
        {
                case 0 : if (xdex > 18)
                {
                        i --;
                } else xdex ++; break;
                case 1 : xstrength ++; break;
                case 2 : xintel ++; break;
        }

        } // end of for i

        xitem_wielded = 0;
        xbody_armour = 2;
// xshield_armour = 3;

        xgp = random2(10);
 xres_magic = 3;

       //       */
break;




}

for (i = 0; i < 52; i ++)
{
 if (xinv_quant [i] == 0)
 {
  xinv_quant [i] = 1;
  xinv_class [i] = 4;
  xinv_type [i] = 1;
  xinv_col [i] = BROWN;
  break;
 }
}



int randnum = 0;
int secchar = 0;


// must remember to check for already existing colours/combinations

for (i = 0; i < 4; i ++)
{
   for (j = 0; j < 30; j ++)
   {
      xitem_name [i] [j] = 215;
   }
}


xitem_name [1] [13] = 178; // oatmeal
xitem_name [1] [16] = 0; // water

for (i = 0; i < 4; i ++)
{
   for (j = 0; j < 30; j ++)
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
            xitem_name [i] [j] = random2(12);
         } else
               {
//                  xitem_name [i] [j] = (random2(12) * 16) + random2(15);
                    xitem_name [i] [j] = (random2(12)) + random2(12) * 16;
               }
         break;

         case 1: // potions
         if (random2(2) != 0)
         {
            xitem_name [i] [j] = random2(14);
         } else
               {
                  xitem_name [i] [j] = (random2(14) * 14) + random2(14);
               }
         break;

         case 2: // scrolls
         xitem_name [i] [j] = random2(151);
         xitem_name [4] [j] = random2(151);
         break;

         case 3: // rings
                if (random2(3) > 0) xitem_name [i] [j] = random2(13);
                         else xitem_name [i] [j] = random2(13) + (random2(13) * 13);
         break;

       } // end switch

       for (p = 0; p < 30; p ++) // don't have p < j because some are preassigned
       {
        if (xitem_name [i] [p] == xitem_name [i] [j] && j != p)
           passout = 0;
       }

       } while (passout == 0);

   }
}

char igl [5];

cprintf("\n\r");

for (i = 0; i < 4; i ++)
{
   for (j = 0; j < 30; j ++)
   {
                                //itoa(xitem_name [i] [j], igl, 10);
                                //cprintf(igl);
                                //cprintf(",");






    xitem_name [i] [j] += 1; // NEED THIS BIT!




                        }
                        //cprintf("\n\r");
}

//getch();

for (i = 0; i < 52; i ++)
{
        if (xinv_class [i] != 0)
        {
                set_id(xinv_class [i], xinv_type [i], 1);
        }
}





// Use the first one to amend already created files, and the second to create
//  new - the 2nd will be used eventually, as save games will be deleted after
//  loading.
//handle = open("me.sav", O_WRONLY, S_IWRITE, S_IREAD); //O_WRONLY); //, O_CREAT | O_BINARY);
//handle = open(char_fil, O_CREAT | O_TEXT | O_RDWR);
/*
switch(char_num)
{
        case 0: strcpy(char_fil, "char00.dat"); break;
        case 1: strcpy(char_fil, "char01.dat"); break;
        case 2: strcpy(char_fil, "char02.dat"); break;
        case 3: strcpy(char_fil, "char03.dat"); break;
        case 4: strcpy(char_fil, "char04.dat"); break;
        case 5: strcpy(char_fil, "char05.dat"); break;
        case 6: strcpy(char_fil, "char06.dat"); break;
        case 7: strcpy(char_fil, "char07.dat"); break;
        case 8: strcpy(char_fil, "char08.dat"); break;
        case 9: strcpy(char_fil, "char09.dat"); break;
//case 0: strcpy(char_fil, "char01.dat"); break;
}
*/
//handle = open(char_fil, O_CREAT | O_TRUNC | O_BINARY, S_IWRITE);

handle = open(char_fil, O_CREAT | O_TRUNC | O_BINARY, S_IWRITE | S_IREAD);

if (handle == -1)
{
        perror("Unable to open file for writing");

}

rm [0] = 40; //teleport + 40;
rm [1] = 40; //see_invis + 40;
rm [2] = 40; //res_fire + 40;
rm [3] = 40; //res_poison + 40;
rm [4] = 40; //res_cold + 40;
rm [5] = 40; //fast_run + 40;
rm [6] = 40; //shock_shield + 40;
rm [7] = 40; // sust_abil
rm [8] = 40; // magic_battery
rm [9] = 40; // death's door
rm [10] = 40; // spec_fire
rm [11] = 40; // spec_cold
rm [12] = 40; // spec_poison
rm [13] = 40; // pet_target
rm [14] = 40; // prot_life
temp_int = xres_magic + 10000;
itoa(temp_int, xthing_quant, 10);
for (p = 0; p < 5; p ++)
{
                rm [p + 15] = xthing_quant [p];
}
rm [20] = xspell_levels + 80;
rm [21] = 41; // max_level
rm [22] = 40; // where_are_you
rm [23] = 40; // char_direction
rm [24] = 40; // your_level
rm [25] = 40; // shield_class
if (xclas == 0) rm [25] = 45;
rm [26] = 40; // special_wield
rm [27] = 40; // berserker
rm [28] = 40; // energy
rm [29] = 40; // level_type
write(handle, rm, 30);


//int haste = 0; // haste

temp_int = xhp + 10000;
itoa(temp_int, xthing_quant, 10);
for (p = 0; p < 5; p ++)
{
                rm [p] = xthing_quant [p];
}

temp_int = xhp_max + 10000;
itoa(temp_int, xthing_quant, 10);
for (p = 0; p < 5; p ++)
{
                rm [5 + p] = xthing_quant [p];
}

write(handle, rm, 10);



if (xhaste > 215) xhaste = 215;
rm [0] = xhaste + 40;

if (xmight > 215) xmight = 215;
rm [1] = xmight + 40;

if (xlev > 215) xlev = 215;
rm [2] = xlev + 40;

if (xpoison > 215) xpoison = 215;
rm [3] = xpoison + 40;

//int can_see; // Are you blind? (actually, I neither know nor care)
rm [4] = xcan_see + 40;

//int delay_t = 0; I don't think it's neccesary to save these, do you?
//int delay_doing = 0;

//int hunger = 1000;  //                Look at an itoa thing for this.

temp_int = xhunger + 100000;// If you're a large creature, stomach can hold much more than human.
        //ltoa(temp_int, xthing_quant, 10);
        itoa(temp_int, xthing_quant, 10);
        for (p = 0; p < 6; p ++)
        {
                rm [5 + p] = xthing_quant [p];
        } // DON'T save when hunger is -ve

//rm [10] = xhunger_inc + 40; shifted to max values

rm [11] = xitem_wielded + 40;

rm [12] = xbody_armour + 40;

rm [13] = xhead_armour + 40;

rm [14] = xcloak_armour + 40;

rm [15] = xhand_armour + 40;

rm [16] = xfoot_armour + 40;

rm [17] = xshield_armour + 40;

rm [18] = xring [0] + 40;
rm [19] = xring [1] + 40;

rm [20] = 40;//xhp + 40;

rm [21] = 40;//xhp_max + 40;

rm [22] = xep + 40;

rm [23] = xep_max + 40;


rm [24] = xstrength + 40;

rm [25] = xintel + 40;

rm [26] = xdex + 40;



rm [27] = xAC + 80; // Remember that AC shown is 10 - AC, but AC still can be -ve.

rm [28] = xevasion + 40;

rm [29] = xdamage + 40;

rm [30] = xnat_damage + 40;

//float rate_regen = 0.06;
/*temp_int = (int) (xrate_regen * 100) + 10000;
        itoa(temp_int, xthing_quant, 10);
        for (p = 0; p < 5; p ++)
        {
                rm [31 + p] = xthing_quant [p];
        }*/
rm [31] = 46; //xrate_regen + 40;


//float incr_regen = 0; // How am I going to do this???
/*temp_int = (int) (xincr_regen * 100) + 10000;
        itoa(temp_int, xthing_quant, 10);
        for (p = 0; p < 5; p ++)
        {
                rm [36 + p] = xthing_quant [p];
        }*/
rm [32] = 40; //xincr_regen + 40;

temp_long = xxp + 100000;
//temp_int = xxp + 100000;
        //ltoa(temp_long, xthing_quant, 10);
        itoa(temp_long, xthing_quant, 10);
        //itoa(temp_int, xthing_quant, 10);
        for (p = 0; p < 6; p ++)
        {
                rm [41 + p] = xthing_quant [p];
        }

rm [47] = xxl + 40;

temp_int = xgp + 10000;
        itoa(temp_int, xthing_quant, 10);
        for (p = 0; p < 5; p ++)
        {
                rm [48 + p] = xthing_quant [p];
        }

rm [53] = xclas + 40;

temp_int = (int) (xf_abil) + 10000;
        itoa(temp_int, xthing_quant, 10);
        for (p = 0; p < 5; p ++)
        {
                rm [54 + p] = xthing_quant [p];
        }


temp_int = (int) (xspeed * 100) + 10000;
        itoa(temp_int, xthing_quant, 10);
        for (p = 0; p < 5; p ++)
        {
                rm [59 + p] = xthing_quant [p];
        }

temp_int = (int) (xspeed_inc * 100) + 10000;
        itoa(temp_int, xthing_quant, 10);
        for (p = 0; p < 5; p ++)
        {
                rm [64 + p] = xthing_quant [p];
        }

        write(handle, rm, 69);

// max values
rm [0] = xstrength + 40;
rm [1] = xintel + 40;
rm [2] = xdex + 40;
rm [3] = xhunger_inc + 40;
write(handle, rm, 4);


temp_int = (int) (xmag_abil) + 10000;
        itoa(temp_int, xthing_quant, 10);
        for (p = 0; p < 5; p ++)
        {
                rm [0 + p] = xthing_quant [p];
        }


temp_int = (int) (xthr_abil) + 10000;
        itoa(temp_int, xthing_quant, 10);
        for (p = 0; p < 5; p ++)
        {
                rm [5 + p] = xthing_quant [p];
        }

 write(handle, rm, 10);


        rm [0] = (unsigned char) xx_pos;
        rm [1] = (unsigned char) xy_pos;

        write(handle, rm, 2);

        strcpy(rm, xclasnam);
        write(handle, rm, 13);

temp_int = xburden + 10000;
        itoa(temp_int, xthing_quant, 10);
        for (p = 0; p < 5; p ++)
        {
                rm [p] = xthing_quant [p];
        }
        write(handle, rm, 5);

//       clasnam [13]; // Now?
/*
        char inv_name [52] [30];
        char inv_class [52];
        char inv_type [52];
        char inv_plus [52];
        char inv_dam [52];
        char inv_col [52];
        int inv_quant [52]; // multiple items, eg ammo or potions
        int burden; // total weight of items carried.
        char inv_no; // number of items carried.


        handle = open("lev000.dat", O_RDONLY, S_IWRITE, S_IREAD); break;


        rm [0] = mons_inv [i] [j] + 40;

                write(handle, rm, 1);
        */



//       Uncomment here!!!

 for (i = 0; i < 25; i ++)
 {
  rm [i] = 29;
  if (xclas == 1 && i == 0) rm [i] = 35;
  // if is a wizard && i == 0, rm [i] = 35
 }
 write(handle, rm, 25);


        for (i = 0; i < 52; i++)
        {
//      strcpy(rm, xinv_name [i]);
//      write(handle, rm, 30);
        rm [0] = xinv_class [i] + 40;
        rm [1] = xinv_type [i] + 40;
        rm [2] = xinv_plus [i] + 40;
        rm [3] = xinv_dam [i] + 40;
        rm [4] = xinv_col [i] + 40;
        rm [5] = xinv_ident [i] + 40;
        temp_int = xinv_quant [i] + 10000;
        itoa(temp_int, xthing_quant, 10);
        for (p = 0; p < 5; p ++)
        {
                rm [6 + p] = xthing_quant [p];
        }
        write(handle, rm, 11);
        } // end of for i


        //cprintf("Done this bit!");




/*      for (i = 0; i < 4; i ++)
        {
                for (p = 0; p < 30; p ++)
                {
                        rm [p] = get_id(i + 100, p) + 40;
                }
                write(handle, rm, 30);
        }*/

        //cprintf("ANd this bit!");


        for (i = 0; i < 30; i ++)
        {
                rm [i] = lev_ex [i];
                //lev_ex [i] = 0;
        }
        write(handle, rm, 30);


for (i = 0; i < 5; i ++)
{
   for (j = 0; j < 30; j ++)
   {
    rm [j] = xitem_name [i] [j] + 40;
   }

   write(handle, rm, 30);
}

for (i = 0; i < 4; i ++)
{
   for (j = 0; j < 30; j ++)
   {
//    rm [j] = item_description [i] [j] + 40;
      rm [j] = 40; //identy [i] [j] + 40;
      if (xclas == 2 && i == 3 && j == 0) rm [j] = 41;
   }
   write(handle, rm, 30);
}



   for (j = 0; j < 50; j ++)
   {
      rm [j] = 40; // unique items
   }
   write(handle, rm, 50);



close(handle);




//      int sysg = 0;

        //sysg = system("cd c:\borlandc\crawl");

        if (sysg == -1)
        {
                perror("Oh dear... ");
                exit(0);
        }















       /*



        handle = open("control.dat", S_IWRITE, S_IREAD);

        for (i = 0; i < 10; i++)
        {
                read(handle, tm [i] , 30);
        }

        strcpy(tm [char_num], your_nam);

        close (handle);


        handle = open("control.dat", O_WRONLY, S_IWRITE, S_IREAD);

        for (i = 0; i < 10; i++)
        {
                write(handle, tm [i] , 30);

        }

        //strcpy(tm [i], "deceased");

        close (handle);

              */















//abort();
                return 1; //char_num + 100;

} // end of new_game()






//void losight(void)



int exper_value(int mclass, char mHD, char maxhp)
{
int x_val = 0;

x_val = ((maxhp / 7) + 1) * (mHD * mHD) + 1; // I know this is rather unfair. Oh well.

if (mclass == 5 | mclass == 56 | mclass == 41) x_val = 10;

return x_val;
}









void monsters_find(int mo_class [MNST], int mo_inv [MNST] [8], char it_class [ITEMS], char it_type [ITEMS], unsigned char it_dam [ITEMS], char it_col [ITEMS], unsigned char it_plus [ITEMS], unsigned int it_quant [ITEMS], int levl, unsigned char gr [80] [70], unsigned char it_x [ITEMS], unsigned char it_y [ITEMS], int n_it, char k, char mo_x [MNST], char mo_y [MNST], int proper [4] [30] [3], int mo_AC [MNST], char mo_ev [MNST])
{
int no_m = 0; // number of monsters?
int p = 0;
int i = 0;

return;

// k is the number of the monster items are being given to.

//      unsigned long long_q;
//      int iquan = 1;



//if (n_it > ITEMS - 200) return;

/*

for (i = 0; i < 4; i ++)
{
        if (mo_inv [k] [i] != 501) return;
}
*/


for (p = 0; p < ITEMS; p ++)
{
        if (it_quant [p] == 0 | gr [it_x [p]] [it_y [p]] >= 70) continue;
        if (it_class [p] == 0 && random2(3) == 0 && mo_inv [k] [0] == 501)
        // && monster not too distant from it???
        {
                mo_inv [k] [0] = p;
                it_x [p] = mo_x [k];
                it_y [p] = mo_y [k];
                // if monster is invis, item_vis = 0??
                break;
        } // end of if it_class == 0


        if (it_class [p] == 1 && random2(2) == 0 && mo_inv [k] [1] == 501)
        // && monster not too distant from it???
        {
                mo_inv [k] [1] = p;
                it_x [p] = mo_x [k];
                it_y [p] = mo_y [k];
                // if monster is invis, item_vis = 0??
                break;
        } // end of if it_class == 0

        if (it_class [p] == 2 && it_class [p] < 8 && random2(3) == 0 && mo_inv [k] [2] == 501)
        // && monster not too distant from it???
        {
                mo_inv [k] [2] = p;
                it_x [p] = mo_x [k];
                it_y [p] = mo_y [k];
                mo_AC [k] += proper [it_class [p]] [it_type [p]] [0];
                mo_ev [k] += proper [it_class [p]] [it_type [p]] [1];
                mo_AC [k] += it_plus [p] - 50;
                if (it_plus [p] > 130) mo_AC [k] -= 100;
                // if monster is invis, item_vis = 0??
                break;
        } // end of if it_class == 0



} // end of for p


//for (p = 0; p < MNST; p++)
//{



//it_quant [p] = 1;






} // end of monsters_find








void monsters_get(int mo_class [MNST], int mo_inv [MNST] [8], char it_class [ITEMS], char it_type [ITEMS], unsigned char it_dam [ITEMS], char it_col [ITEMS], unsigned char it_plus [ITEMS], unsigned int it_quant [ITEMS], int levl, unsigned char gr [80] [70], unsigned char it_x [ITEMS], unsigned char it_y [ITEMS], int n_it, char k, char mo_x [MNST], char mo_y [MNST], int mo_AC [MNST], char mo_ev [MNST], int proper [4] [30] [3])
{

int p = 0;









//for (p = 0; p < ITEMS; p ++)
//{
//      if (it_quant [p] == 0) continue;

//      if (it_x [p] == mo_x [k] && it_y [p] == mo_y [k])
/*
if (igrid [mo_x [k]] [mo_y [k]] == 501) return;

        while(





int b = 0;
int n = 0;
int m = 0;

for (i = 0; i < ITEMS; i ++)
{
        if (item_quant [i] == 0) continue;

        if (igrid [item_x [i]] [item_y [i]] == 501)
        {
                igrid [item_x [i]] [item_y [i]] = i;
                //cout << i << ", ";
                //char groth = getch();
                continue;
        } else
        {
        //b = item_link [igrid [item_x [i]] [item_y [i]]];
        b = igrid [item_x [i]] [item_y [i]];

        while(b != 501)
        {
                m = b;
                n = item_link [b];
                b = n;
        } // end of while

        item_link [m] = i;

        //cout << "\n\rLinking " << i << " to " << m;
//      char gtot = getch();
        }
} // end of for i




*/
/*























        {

                if (it_class [p] == 0 && mo_inv [k] [0] == 501)
                {
                        mo_inv [k] [0] = p; continue;
                }

                if (it_class [p] == 1 && mo_inv [k] [1] == 501)
                {
                        mo_inv [k] [1] = p; continue;
                }

                if (it_class [p] == 2 && mo_inv [k] [2] == 501)
                // maybe put limits on the kind of armour some (eg K, g) can wear?
                {
                        mo_inv [k] [2] = p; //continue;
                        mo_AC [k] += proper [2] [it_type [p]] [0];
                        mo_ev [k] += proper [2] [it_type [p]] [1];
                        mo_AC [k] += it_plus [p];
                        if (it_plus [p] > 80) mo_AC -= 100;
                        continue;
                }




        //}







































for (p = 0; p < ITEMS; p ++)
{
        if (it_quant [p] == 0) continue;

        if (it_x [p] == mo_x [k] && it_y [p] == mo_y [k])
        {

                if (it_class [p] == 0 && mo_inv [k] [0] == 501)
                {
                        mo_inv [k] [0] = p; continue;
                }

                if (it_class [p] == 1 && mo_inv [k] [1] == 501)
                {
                        mo_inv [k] [1] = p; continue;
                }

                if (it_class [p] == 2 && mo_inv [k] [2] == 501)
                // maybe put limits on the kind of armour some (eg K, g) can wear?
                {
                        mo_inv [k] [2] = p; //continue;
                        mo_AC [k] += proper [2] [it_type [p]] [0];
                        mo_ev [k] += proper [2] [it_type [p]] [1];
                        mo_AC [k] += it_plus [p];
                        if (it_plus [p] > 80) mo_AC -= 100;
                        continue;
                }




        }










} // end of for p

*/

}













char restock_monsters(int mo_class [MNST], int mo_inv [MNST] [8], char it_class [ITEMS], char it_type [ITEMS], unsigned char it_dam [ITEMS], char it_col [ITEMS], unsigned char it_plus [ITEMS], unsigned int it_quant [ITEMS], int levl, unsigned char gr [80] [70], unsigned char it_x [ITEMS], unsigned char it_y [ITEMS], int n_it, char k, char mo_x [MNST], char mo_y [MNST], int ilink [ITEMS], char it_id [ITEMS])
{

int no_m = 0; // number of monsters?
int p = 0;
int i = 0;







        // need to reset item_id too!











//strcpy(info [info_lines], "Giving: ");
//itoa(it_no, st_prn, 10);
//strcat(info [info_lines], st_prn);


//return;

// k is the number of the monster items are being given to.

//      unsigned long long_q;
//      int iquan = 1;




//if (n_it > ITEMS - 200) return;

for (i = 0; i < 8; i ++)
{
        if (mo_inv [k] [i] != 501) return 0;
}



for (p = 0; p < ITEMS; p ++)
{
        if (it_quant [p] == 0) break;
        if (p > 400) return 0;
}


//for (p = 0; p < MNST; p++)
//{




//


switch(mo_class [k])
{


case 12: // manticore
//if (random2 (5) < 1) // < 1) // give hand weapon
        {
//      it_class [p] = 1;
//      switch(random2(6))
//              {
                it_class [p] = 1;
                it_type [p] = 3;
                it_quant [p] = random2(13) + 12;
//              }
//      it_quant [p] = 1;
        }

break;







case 36: // kobold
if (random2 (5) < 1) // < 1) // give hand weapon
        {
        it_class [p] = 0;
        switch(random2(6))
                {
                        case 0: it_type [p] = 0; break;
                        case 1: it_type [p] = 3; break;
                        case 2: it_type [p] = 11; break;
                        case 3: it_type [p] = 5; break;
                        case 4: it_type [p] = 11; break;
                }
        it_quant [p] = 1;
        } else
              if (random2 (5) < 7)
                {
                        it_class [p] = 1;
                        it_type [p] = 3;
                        it_quant [p] = random2(5) + 1;
                } else return 0;

break;

case 33:  // Hobgoblin
if (random2 (5) < 5) // < 3) // give hand weapon
        {
        it_class [p] = 0;
        it_type [p] = 0;
        it_quant [p] = 1;
        } else return 0;
break;

case 6: // goblin
if (random2 (5) < 5) // 3) // give hand weapon
        {
        it_class [p] = 0;
        switch(random2(2))
                {
                        case 0: it_type [p] = 0; break;
                        case 1: it_type [p] = 3; break;
//                      case 2: it_type [p] = 11; break;
//                      case 3: it_type [p] = 5; break;
//                      case 4: it_type [p] = 14; break;
                }
        it_quant [p] = 1;
        } else return 0;
break;

case 14: // orc
if (random2 (5) < 4)
        {
        it_class [p] = 0;
        switch(random2(8))
                {
                        case 0: it_type [p] = 2; break;
                        case 1: it_type [p] = 4; break;
                        case 2: it_type [p] = 8; break;
                        case 3: it_type [p] = 9; break;
                        case 4: it_type [p] = 11; break;
                        case 5: it_type [p] = 5; break;
                        case 6: it_type [p] = 3; break;
                        case 7: it_type [p] = 0; break;
//                      case 8: it_type [p] = 5; break;
//                      case 9: it_type [p] = 14; break;
                }
        it_quant [p] = 1;
        } else return 0;
break;

case 52: // orc warrior
        it_class [p] = 0;
        switch(random2(8))
                {
                        case 0: it_type [p] = 2; break;
                        case 1: it_type [p] = 4; break;
                        case 2: it_type [p] = 8; break;
                        case 3: it_type [p] = 9; break;
                        case 4: it_type [p] = 11; break;
                        case 5: it_type [p] = 5; break;
                        case 6: it_type [p] = 6; break;
                        case 7: it_type [p] = 10; break;
                }
        it_quant [p] = 1;
break;


case 55: // goblin wizard
        it_class [p] = 6;
/*      switch(random2(8))
                {
                        case 0: it_type [p] = 2; break;
                        case 1: it_type [p] = 4; break;
                        case 2: it_type [p] = 8; break;
                        case 3: it_type [p] = 9; break;
                        case 4: it_type [p] = 11; break;
                        case 5: it_type [p] = 5; break;
                        case 6: it_type [p] = 6; break;
                        case 7: it_type [p] = 10; break;
                }
*/
        it_type [p] = 5;
        it_quant [p] = random2(6);
        it_dam [p] = 5;
break;


default: return 0;

} // end of switch(mons_class [k])


//it_quant [p] = 1;
//long_q = 100000 + iquan;
//ltoa(long_q, it_quant [p], 10);//remember that the 10 is the base (radix)!

it_plus [p] = 50; //50 //                       Look at this!!!
it_dam [p] = 0; //                       And this!!!!!!
it_x [p] = mo_x [k];
it_y [p] = mo_y [k];
it_id [p] = 0;
if (it_class [p] == 0) mo_inv [k] [0] = p;
if (it_class [p] == 1) mo_inv [k] [1] = p;
if (it_class [p] == 15) mo_inv [k] [4] = p;
if (it_class [p] == 6) mo_inv [k] [4] = p;

n_it ++;

//name_it();             unidentify should take care of this.

it_col [p] = DARKGRAY;

ilink [p] = 501;


return 1;

} // end of void give_it()







//} // end of for i









//} // end of restock_monsters













// dumpfile for items, to be reloaded later?

void item_cull(int mo_class [MNST], int mo_inv [MNST] [8], char it_class [ITEMS], char it_type [ITEMS], unsigned char it_dam [ITEMS], char it_col [ITEMS], unsigned char it_plus [ITEMS], unsigned int it_quant [ITEMS], int levl, unsigned char gr [80] [70], unsigned char it_x [ITEMS], unsigned char it_y [ITEMS], int n_it, char mo_x [MNST], char mo_y [MNST])
{
int i = 0;
int p = 0;
int j = 0;

char cull = 0;
char brek = 0;

if (n_it < 300) return;

for (i = 0; i < ITEMS; i++)
{

if (it_quant [i] == 0) continue;

if (it_class [i] == 3 && it_plus [i] == 0 && (it_type [i] < 7 && random2(3) < 2) | (it_type [i] >= 7 && random2(4) < 2))
{
        it_quant [i] = 0;
        cull = 1;
        //continue;
}


if (it_class [i] == 0 && gr [it_x [i]] [it_y [i]] < 70 && it_dam [i] == 0 && it_plus [i] == 0)
{
        it_quant [i] = 0;
        cull = 1;
        //continue;
}

if (it_class [i] == 1 && gr [it_x [i]] [it_y [i]] < 70 && it_dam [i] == 0 && it_plus [i] == 0 && it_quant [i] < 10)
{
        it_quant [i] = 0;
        cull = 1;
        //continue;
}

if (random2(5) == 0) // and it's not an artifact or whatever
{
        it_quant [i] = 0;
        cull = 1;

}




if (cull == 1 && gr [it_x [i]] [it_y [i]] >= 70)
{
        for (p = 0; p < MNST; p ++)
        {
                for (j = 0; j < 8; j ++)
                {
                        if (mo_inv [p] [j] == i) mo_inv [p] [j] = 501;
                        brek = 1;
                        break;
                }
                if (brek == 1) break;
        } // end of for p
} // end of if (cull == 1)


cull = 0;
brek = 0;
} // end of for i




} // end of void item_cull

















void replace_items(char it_class [ITEMS], char it_type [ITEMS], unsigned char it_dam [ITEMS], char it_col [ITEMS], unsigned char it_plus [ITEMS], unsigned int it_quant [ITEMS], int levl, unsigned char gr [80] [70], unsigned char it_x [ITEMS], unsigned char it_y [ITEMS])
{

return; // I don't consider this function necessary.

}

/*

int quant;
char brk = 0;
int p;
unsigned long gold_q;
int secchar;
int num_it = 0;
char randnum;

//no_it = random2 (50) + 50;

num_it = random2(100);
//no_it = 250;
//no_it = 26;
//num_it = 400;

for (p = 0; p < num_it; p ++)
{
        if (it_quant [p] > 0) continue;

        it_class [p] = random2(10) + 2;

//      it_class [p] = random2 (3) + 3;

//      it_class [p] = 5;

//it_class [p] = 5;

        switch(it_class [p])
        {
                case 0: it_type [p] = random2(11); // weapons
                quant = 1;
                if (levl < 4 | random2(4) == 0) // ie these weapons are more common on any level.
                {

                if (random2 (10) > 6) it_plus [p] = random2 (3);
                        switch(random2(6))
                        {
                                case 0: it_type [p] = 0; break;
                                case 1: it_type [p] = 1; break;
                                case 2: it_type [p] = 3; break;
                                case 3: it_type [p] = 4; break;
                                case 4: it_type [p] = 5; break;
                                case 5: it_type [p] = 14; break;
                        }
                break;
                }
 /*             if (levl >= 4 && levl < 7)
                {
                        switch(random2(6))
                        {
                                case 0: it_type [p] = 0; break;
                                case 1: it_type [p] = 1; break;
                                case 2: it_type [p] = 3; break;
                                case 3: it_type [p] = 4; break;
                                case 4: it_type [p] = 5; break;
                                case 5: it_type [p] = 14; break;
                        }
//              }             * /
                if (random2 (10) > 7) it_plus [p] = random2 (10);
                if (it_plus [p] < 4) it_plus [p] += 100;
                it_plus [p] -= 4;
                it_plus [p] += 50;

                if (levl >= 4 && levl < 7)
                {
                it_type [p] = random2 (17);
                }
                break;

                case 1: // missiles
                it_type [p] = random2(4);
                quant = random2(7) + random2 (6) + 1;// + random2 (10) + 1;
                if (random2 (10) > 7) it_plus [p] = random2 (5);
                it_plus [p] += 50;
                break;

                case 2: //it_type [p] = random2(12); //         armour
                quant = 1;
                if (levl < 4 | random2(4) == 0) // ie these armours are more common on any level.
                {
                        if (random2 (10) > 6) it_plus [p] = random2 (4);
                                else
                                    {
                                            if (random2 (5) == 0) it_plus [p] = 99 - random2(3);
                                    }
                                    it_plus [p] += 50;
                        switch(random2(7))
                        {
                                case 0: it_type [p] = 0; break;
                                case 1: it_type [p] = 1; break;
                                case 2: it_type [p] = 2; break;
                                case 3: it_type [p] = 3; break;
                                case 4: it_type [p] = 9; break;
                                case 5: it_type [p] = 12; break;
                                case 6: it_type [p] = 8; break;
                        }
                        break;
                }
                if (levl >= 4 && levl < 7)
                {
                        if (random2 (10) > 5) it_plus [p] = random2 (5);
                        else
                            {
                                    if (random2 (9) == 0) it_plus [p] = 99 - random2(4);
                            }
                        it_type [p] = random2 (12);
                        if (it_type [p] == 6 && random2(3) == 0) it_type [p] = 7;
                }
                it_plus [p] += 50;
                break;


                case 3:
//              if (levl >= 4 && levl < 7)
//              {
                        it_type [p] = random2 (9);
                        if (it_type [p] < 2) it_plus [p] = random2 (10) + random2(10) + random2(10);
                        if (it_type [p] >= 2 && it_type [p] < 9) it_plus [p] = random2 (5) + random2 (5) + random2 (5);
//              }
                if (it_type [p] < 7)
                {
                randnum = random2(12);
                it_dam [p] = randnum;
                secchar = 1;
                } else
                      {
                          secchar = random2(16);
                          randnum = random2(10);
                          it_dam [p] = (randnum * 16) + secchar;
                      } // end of else
                quant = 1;
                switch(randnum)
                {
                        case 0: it_col [p] = 11; break;
                        case 1: it_col [p] = 14; break;
                        case 2: it_col [p] = 7; break;
                        case 3: it_col [p] = 6; break;
                        case 4: it_col [p] = 6; break;
                        case 5: it_col [p] = 14; break;
                        case 6: it_col [p] = 7; break;
                        case 7: it_col [p] = 6; break;
                        case 8: it_col [p] = 7; break;
                        case 9: it_col [p] = 7; break;
                        case 10: it_col [p] = 7; break;
                        case 11: it_col [p] = 7; break;
                } // end of switch(randnum)
                if (secchar == 0) it_col [p] = 8;
                break; // end of wands



                case 4:
                it_type [p] = random2 (5); // food
                quant = random2 (3) + 1;
                if (random2(4) == 0) quant = random2(3) + 1;
                it_col [p] = BROWN; // <-- Correct this!
                break;










                case 5: it_class [p] = 8; // potions // This one not finished
                //it_type [p] = random2 (4);
                it_type [p] = random2 (10);
                quant = 1;
                if (random2(5) == 1) quant = random2 (3) + 1;

                if (it_type [p] < 4)
                {
                        randnum = random2(14);
                        it_dam [p] = randnum;
                } else //ie if it's uncommon:
                {
                        secchar = random2(13);
                        randnum = random2(14);
                        it_dam [p] = (randnum * 13) + secchar;
                } // end of else.

                switch(randnum)
                {
                        case 0: it_col [p] = 7; break;
                        case 1: it_col [p] = 1; break;
                        case 2: it_col [p] = 8; break;
                        case 3: it_col [p] = 7; break;
                        case 4: it_col [p] = 3; break;
                        case 5: it_col [p] = 5; break;
                        case 6: it_col [p] = 12; break;
                        case 7: it_col [p] = 1; break;
                        case 8: it_col [p] = 4; break;
                        case 9: it_col [p] = 14; break;
                        case 10: it_col [p] = 2; break;
                        case 11: it_col [p] = 6; break;
                        case 12: it_col [p] = 13; break;
                        case 13: it_col [p] = 15; break;
                } // end of switch(randnum)
                break;







                case 6: // scrolls
                it_type [p] = random2(6);
                if (random2 (5) == 0) it_type [p] = 0; // identify should be very common
                quant = 1;
                if (random2 (3) == 0) quant = random2 (3) + 1;
                it_col [p] = LIGHTGRAY;
                break;


                default: // money - class = 15
                it_class [p] = 15;
                quant = random2(10) + random2(3) * levl;
                break;

        }


//      gold_q = quant + 100000;


        it_quant [p] = quant;

//        char it_quant [6];

//      ltoa(gold_q, it_quant [p], 10);//remember that the 10 is the base!
        // use ltoa because gold_q is a long variable.


        do
        {
                it_x [p] = random2 (60) + 10;
                it_y [p] = random2 (50) + 10;
        } while (gr [it_x [p]] [it_y [p]] != 17);


} // end of for p


} // end of void replace_items(whatever)
*/























char item_name(char item_clas, char item_typ, unsigned char item_da, unsigned char it_plus, unsigned int it_quant, char ident_lev, char descrip, char glag [60])
{

//char *glag;
char itm_name [60] = "";
char tmp_quant [5];

strcpy(glag, "");

//glag = (char *)malloc(60);

//if (!glag) cprintf("Not enough memory to allocate buffer!");

//strcpy(glag, "");


//if (ident_lev != 2) abort();

//char *glig = item_name_2(item_clas, item_typ, item_da, it_plus, it_quant, ident_lev);

//strcpy(itm_name, item_name_2(item_clas, item_typ, item_da, it_plus, it_quant, ident_lev));


item_name_2(item_clas, item_typ, item_da, it_plus, it_quant, ident_lev, itm_name);



//free(glig);

if (item_clas == 12 | (item_clas == 0 && item_da > 180 && ident_lev > 0))
{
 switch(descrip)
 {
  case 2:
  case 4:
        case 0: strcat(glag, "The "); break;
  case 3:
  case 5:
  case 7: // !
        case 1: strcat(glag, "the "); break;
 }
} else
if (it_quant > 1)
{
switch(descrip)
{
        case 0: strcat(glag, "The "); break;
        case 1: strcat(glag, "the "); break;
        case 2: break; // A/An
        case 3: break; // a/an
        case 4: strcat(glag, "Your "); break;
        case 5: strcat(glag, "your "); break;
//      case 6: break; // nothing
        case 7: strcat(glag, "its "); break;
}

        itoa(it_quant, tmp_quant, 10);
        strcat(glag, tmp_quant);
        strcat(glag, " ");
        } else
{
        switch(descrip)
        {
                case 0: strcat(glag, "The "); break;
                case 1: strcat(glag, "the "); break;
                case 2: strcat(glag, "A");
                        if (itm_name [0] == 97 ^ itm_name [0] == 101 ^ itm_name [0] == 105 ^ itm_name [0] == 111 ^ itm_name [0] == 117)
                        {
                                strcat(glag, "n");
                        }
                strcat(glag, " ");
                break; // A/An
                case 3:strcat(glag, "a");
                        if (itm_name [0] == 97 ^ itm_name [0] == 101 ^ itm_name [0] == 105 ^ itm_name [0] == 111 ^ itm_name [0] == 117)
                        {
                                strcat(glag, "n");
                        }
                strcat(glag, " ");
                break; // a/an
                case 4: strcat(glag, "Your "); break;
                case 5: strcat(glag, "your "); break;
//              case 6: break; // nothing
                case 7: strcat(glag, "its "); break;
        }

} // end of else

strcat(glag, itm_name);

//free(glag);

//return(glag);

} // end of char name_item



























//char *moname(char mcl, char mench, char see_inv, char descrip)

void moname(int mcl, char mench, char see_inv, char descrip, char glog [40])
{
//char glog [40];
char gmon_name [40] = "";

//glog = (char *)malloc(40);

//if (!glog) cprintf("Not enough memory to allocate buffer!");

strcpy(gmon_name, "");
strcpy(glog, "");


if (mench == 6 && see_inv == 0)
{
        switch(descrip)
        {
        case 0:
        case 2: strcpy(glog, "It"); break;
        case 1:
        case 3: strcpy(glog, "it"); break;
        }
//      strcat(glog, "invisible ");
//      free(glog);
        strcpy(gmon_name, glog);
        return;
}



switch(descrip)
{
case 0: strcpy(glog, "The "); break;
case 1: strcpy(glog, "the "); break;
case 2: strcpy(glog, "A"); break;
case 3: strcpy(glog, "a"); break;
//case 4: do nothing - 4 is empty
}

if (descrip == 2 | descrip == 3 | descrip == 99) // 99 from ouch(...)
        switch(mcl)
        {
        case 4: // e
        case 8: // imp
        case 14:
        case 20:
        case 16:
 case 26: // A
        case 40:
        case 46:
        case 52:
        case 54:
 case 61: // o plant
 case MWATER2: // electrical eel
 case 65: // eye of draining
strcat(glog, "n "); break;

        default: strcat(glog, " "); break;
        }

if (descrip == 2 | descrip == 3 && (glog [1] != 110) && mench == 6 && see_inv == 0)
{
        strcat(glog, "n ");
}




//strcat(glog, gmon_name [mcl]);
switch(mcl)
{

case 0: strcat(gmon_name, "giant ant"); break;
case 1: strcat(gmon_name, "giant bat"); break;
case 2: strcat(gmon_name, "centaur"); break;
case 3: strcat(gmon_name, "red devil"); break;
case 4: strcat(gmon_name, "ettin"); break;
case 5: strcat(gmon_name, "fungus"); break;
case 6: strcat(gmon_name, "goblin"); break;
case 7: strcat(gmon_name, "hound"); break;
case 8: strcat(gmon_name, "imp"); break;
case 9: strcat(gmon_name, "jackal"); break;
case 10: strcat(gmon_name, "killer bee"); break;
case 11: strcat(gmon_name, "killer bee larva"); break;
case 12: strcat(gmon_name, "manticore"); break;
case 13: strcat(gmon_name, "necrophage"); break;
case 14: strcat(gmon_name, "orc"); break;
case 15: strcat(gmon_name, "phantom"); break;
case 16: strcat(gmon_name, "quasit"); break;
case 17: strcat(gmon_name, "rat"); break;
case 18: strcat(gmon_name, "scorpion"); break;
case 19: strcat(gmon_name, "tunneling worm"); break;
case 20: strcat(gmon_name, "ugly thing"); break;
case 21: strcat(gmon_name, "fire vortex"); break;
case 22: strcat(gmon_name, "worm"); break;
case 23: strcat(gmon_name, "abomination"); break;
case 24: strcat(gmon_name, "yellow wasp"); break;
case 25: break;
//strcat(gmon_name, " zombie");
//break;
case 26: strcat(gmon_name, "angel"); break;
case 27: strcat(gmon_name, "giant beetle"); break;
case 28: strcat(gmon_name, "cyclops"); break;
case 29: strcat(gmon_name, "dragon"); break;
case 30: strcat(gmon_name, "two-headed ogre"); break;
case 31: strcat(gmon_name, "Fiend"); break;
case 32: strcat(gmon_name, "giant spore"); break;
case 33: strcat(gmon_name, "hobgoblin"); break;
case 34: strcat(gmon_name, "ice beast"); break;
case 35: strcat(gmon_name, "jelly"); break;
case 36: strcat(gmon_name, "kobold"); break;
case 37: strcat(gmon_name, "lich"); break;
case 38: strcat(gmon_name, "mummy"); break;
case 39: strcat(gmon_name, "naga"); break;
case 40: strcat(gmon_name, "ogre"); break;
case 41: strcat(gmon_name, "plant"); break;
case 42: strcat(gmon_name, "queen bee"); break;
case 43: strcat(gmon_name, "rakshasa"); break;
case 44: strcat(gmon_name, "snake"); break;
case 45: strcat(gmon_name, "troll"); break;
case 46: strcat(gmon_name, "unseen horror"); break;
case 47: strcat(gmon_name, "vampire"); break;
case 48: strcat(gmon_name, "wraith"); break;
case 49: strcat(gmon_name, "abomination"); break;
case 50: strcat(gmon_name, "yak"); break;
case 51: break;
// strcat(gmon_name, "large zombie"); break;
case 52: strcat(gmon_name, "orc warrior"); break;
case 53: strcat(gmon_name, "kobold demonologist"); break;
case 54: strcat(gmon_name, "orc wizard"); break;
case 55: strcat(gmon_name, "orc knight"); break;
case 56: strcat(gmon_name, "worm tail"); break;

//strcat(gmon_name 54, "Type I demon");
//case 56: strcat(gmon_name, "Type II demon"); break;
case 57: strcat(gmon_name, "wyvern"); break;
case 58: strcat(gmon_name, "big kobold"); break;
case 59: strcat(gmon_name, "giant eyeball"); break;
case 60: strcat(gmon_name, "wight"); break;
case 61: strcat(gmon_name, "oklob plant"); break;
case 62: strcat(gmon_name, "wolf spider"); break;
case 63: strcat(gmon_name, "shadow"); break;
case 64: strcat(gmon_name, "hungry ghost"); break;
case 65: strcat(gmon_name, "eye of draining"); break;
case 66: strcat(gmon_name, "butterfly"); break;
case 67: strcat(gmon_name, "wandering mushroom"); break;
case 68: strcat(gmon_name, "efreet"); break;
case 69: strcat(gmon_name, "brain worm"); break;
case 70: strcat(gmon_name, "giant orange brain"); break;

case 71: strcat(gmon_name, "boulder beetle"); break;
case 72: strcat(gmon_name, "flying skull"); break;
case 73: strcat(gmon_name, "hell hound"); break;
case 74: strcat(gmon_name, "minotaur"); break;
case 75: strcat(gmon_name, "ice dragon"); break;
case 76: strcat(gmon_name, "slime creature"); break;
case 77: strcat(gmon_name, "freezing wraith"); break;
case 78: strcat(gmon_name, "rakshasa"); break;
case 79: strcat(gmon_name, "great orb of eyes"); break;

/*
case 80: strcat(gmon_name, "burning devil"); break;
case 81: strcat(gmon_name, "rotting devil"); break;
case 82: strcat(gmon_name, "yellow devil"); break;
case 83: strcat(gmon_name, "skeletal devil"); break;
case 84: strcat(gmon_name, "shadowy devil"); break;
case 85: strcat(gmon_name, "hairy devil"); break;
case 86: strcat(gmon_name, "ice devil"); break;
case 87: strcat(gmon_name, "blue devil"); break;
*/
case 80: strcat(gmon_name, "hellion"); break;
case 81: strcat(gmon_name, "rotting devil"); break;
case 82: strcat(gmon_name, "tormentor"); break;
case 83: strcat(gmon_name, "reaper"); break;
case 84: strcat(gmon_name, "soul eater"); break;
case 85: strcat(gmon_name, "hairy devil"); break;
case 86: strcat(gmon_name, "ice devil"); break;
case 87: strcat(gmon_name, "blue devil"); break;

case 100: strcat(gmon_name, "giant mite"); break;
case 101: strcat(gmon_name, "steam dragon"); break;
case 102: strcat(gmon_name, "very ugly thing"); break;
case 103: strcat(gmon_name, "orc sorceror"); break;
case 104: strcat(gmon_name, "hippogriff"); break;
case 105: strcat(gmon_name, "griffon"); break;
case 106: strcat(gmon_name, "hydra"); break;

case MLAVA0: strcat(gmon_name, "lava worm"); break;
case MLAVA1: strcat(gmon_name, "lava fish"); break;
case MLAVA2: strcat(gmon_name, "lava snake"); break;
case MLAVA3: strcat(gmon_name, "another lava thing"); break;
case MWATER0: strcat(gmon_name, "big fish"); break;
case MWATER1: strcat(gmon_name, "giant goldfish"); break;
case MWATER2: strcat(gmon_name, "electrical eel"); break;
case MWATER3: strcat(gmon_name, "jellyfish"); break;
case MWATER4: strcat(gmon_name, ""); break;
case MWATER5: strcat(gmon_name, ""); break;
//case 71: strcat(gmon_name, "
default: //strcat(gmon_name, "illegal monster"); break;
itoa(mcl, gmon_name, 10); break;


} // end of switch(mcl)


strcat(glog, gmon_name);


//free(glog);

//return glog;

}



//int func_pass [10];
//char str_pass [50];


char zappy(int func_pass [10], char str_pass [50], char z_type)
{

func_pass [7] = 0; // wand_id

/*
beam_colour = func_pass [0];
beam_range = func_pass [1];
beam_damage = func_pass [2];
beam_hit = func_pass [3];
beam_type = func_pass [4];
beam_flavour = func_pass [5];
thing_thrown = func_pass [6];
wand_id = func_pass [7];
*/
func_pass [9] = func_pass [2];
// func_pass [8] = power -> 40 for wands

//func_pass [2]

switch(z_type)
{
//if (inv_type [zap_device_2] == 0)
//{
        case 0: // flame
//      beam_source_x = x_pos;
//      beam_source_y = y_pos;
        func_pass [0] = 4;
        strcpy(str_pass, "puff of flame");
        func_pass [1] = random2(5) + 7;
        func_pass [2] = 104 + func_pass [8] / 23; // should this be the same as magic missile? no
        func_pass [3] = 60;
        func_pass [4] = 42; // maybe make missile() put a # on wherever it hits?
        func_pass [6] = 4; //3;
        func_pass [5] = 2; // fire
        func_pass [7] = 1; // wand_id
        return 1;
        //break;


        case 1: // frost
//      beam_source_y = y_pos;
        func_pass [0] = 15;
        strcpy(str_pass, "puff of frost");
        func_pass [1] = random2(5) + 7;
        func_pass [2] = 104 + func_pass [8] / 23; // should this be the same as magic missile?
        func_pass [3] = 60;
        func_pass [4] = 42; // maybe make missile() put a # on wherever it hits?
        func_pass [6] = 4;//3;
        //return 1;
        func_pass [5] = 3; // cold
        func_pass [7] = 1;
        return 1;
        //break;

        case 2: // slow monster
        strcpy(str_pass, "0");
        func_pass [1] = random2(5) + 6;
        func_pass [4] = 0;
        func_pass [0] = 0;
        func_pass [6] = 2; //1;
 func_pass [2] = 50;
        //beam();
        func_pass [5] = 4; // magic
        return 2;
        //break;

        case 3: // hasting
        strcpy(str_pass, "0");
        func_pass [1] = random2(5) + 6;
        func_pass [4] = 0;
        func_pass [0] = 1;
        func_pass [6] = 2; //1;
        //beam();
        func_pass [5] = 4; // magic
        return 2;
        //break;

        case 4: // magic missile
        //strcpy(info [info_lines], "You hurl a ball of sizzling energy!");
        //incrl();
//      beam_source_x = x_pos;
//      beam_source_y = y_pos;
        func_pass [0] = 13;//inv_col [throw_2];//icolour [inv_class [throw_2]] [inv_type [throw_2]];
        strcpy(str_pass, "magic dart");// inv_name [throw_2]);
//      if (inv_class [throw_2] == 0 | inv_class [throw_2] == 1)
//      {
//              func_pass [2] = property [inv_class [throw_2]] [inv_type [throw_2]] [0];
//              func_pass [2] += inv_plus [throw_2];
//      }
//       else func_pass [2] = 2; // later put mass thing here.
        func_pass [1] = random2(5) + 7;
        func_pass [2] = 103 + (func_pass [8] / 100);
        func_pass [3] = 1500;
        func_pass [4] = 35;
//      func_pass [3] = strength + (0.5 * dex) + (f_abil);
//      if (inv_class [throw_2] == 0 | inv_class [throw_2] == 1) func_pass [3] += inv_plus [throw_2];
        //func_pass [3] *= random2(200) / 100;
        func_pass [6] = 4;//3;
        func_pass [5] = 1; // a magical missile
        func_pass [7] = 1;
        return 1;
        //break;

        case 5: // healing
        strcpy(str_pass, "0");
        func_pass [1] = random2(5) + 6;
        func_pass [4] = 0;
        func_pass [0] = 2;
        func_pass [6] = 2; //1;
        func_pass [5] = 4; // magic
        return 2;
        //break;

        case 6: // paralysis
        strcpy(str_pass, "0");
        func_pass [1] = random2(5) + 6;
        func_pass [4] = 0;
        func_pass [0] = 3;
        func_pass [6] = 2;//1;
        func_pass [5] = 4; //  magic
 func_pass [2] = 0; //func_pass [9];
        return 2;
        //break;

        case 7:
        strcpy(str_pass, "bolt of fire");
        func_pass [1] = random2(10) + 5;
        func_pass [2] = 110 + func_pass [8] / 10;
        func_pass [0] = 4;
        func_pass [4] = 35; // 35
        func_pass [6] = 2;//1;
        func_pass [5] = 2; // fire
        func_pass [7] = 1;
 func_pass [3] = 5 + random2(func_pass [8]) / 80;
        return 2;
        //break;
//}


//if (inv_type [zap_device_2] == 1)
//{
        case 8:
        strcpy(str_pass, "bolt of cold");
        func_pass [1] = random2(10) + 5;
        func_pass [2] = 110 + func_pass [8] / 10;
        func_pass [0] = WHITE; //LIGHTCYAN;
        func_pass [4] = 35;
        func_pass [6] = 2;//1;
        func_pass [5] = 3; // cold
        func_pass [7] = 1;
 func_pass [3] = 5 + random2(func_pass [8]) / 80;
        return 2;
        //break;

        case 9: // confusion
        strcpy(str_pass, "0");
        func_pass [1] = random2(5) + 6;
        func_pass [4] = 0;
        func_pass [0] = 4;
        func_pass [6] = 2;//1;
        func_pass [5] = 4; // magic
        return 2;
        //break;

        case 10: // make invisible
        strcpy(str_pass, "0");
        func_pass [1] = random2(5) + 6;
        func_pass [4] = 0;
        func_pass [0] = 5;
        func_pass [6] = 2;//1;
        func_pass [5] = 4; // magic
        return 2;
        //break;

        case 11: // digging
        strcpy(str_pass, "0");
        func_pass [1] = random2(5) + 4 + random2(func_pass [8]) / 10;
        func_pass [4] = 0;
        func_pass [0] = 6;
        func_pass [6] = 2;//1;
        func_pass [5] = 4;
        return 2;
        //break;


        case 12: // fireball
//      beam_source_x = x_pos;
//      beam_source_y = y_pos;
        func_pass [0] = 4;
        strcpy(str_pass, "fireball");
        func_pass [1] = random2(5) + 7;
        func_pass [2] = 15; // should this be the same as magic missile?
        func_pass [3] = 40;
        func_pass [4] = 42;
        func_pass [6] = 4;//3;
        func_pass [5] = 10; // fire
        return 1;
        //break;

        case 13: // teleportation
        strcpy(str_pass, "0");
        func_pass [1] = random2(5) + 6;
        func_pass [4] = 0;
        func_pass [0] = 7; // 6 is used by digging
        func_pass [6] = 2;//1;
        func_pass [5] = 4; // magic
        return 2;
        //break;

        case 14: // lightning
        strcpy(str_pass, "bolt of lightning");
        func_pass [1] = random2(10) + 8;
        func_pass [2] = 111 + func_pass [8] / 9;
        func_pass [0] = LIGHTCYAN;
        func_pass [4] = 35;
        func_pass [6] = 2;//1;
        func_pass [5] = 5; // electricity
        func_pass [7] = 1;
 func_pass [3] = 30; // beam hit - look at it!
        return 2;

   case 15: // polymorph
        strcpy(str_pass, "0");
        func_pass [1] = random2(5) + 6;
        func_pass [4] = 0;
        func_pass [0] = 8;
        func_pass [6] = 2;//1;
        func_pass [5] = 4; // magic
        return 2;

        case 16: // venom bolt
        strcpy(str_pass, "bolt of poison");
        func_pass [1] = random2(10) + 8;
        func_pass [2] = 105 + func_pass [8] / 9;
        func_pass [0] = LIGHTGREEN;
        func_pass [4] = 35;
        func_pass [6] = 2;//1;
        func_pass [5] = 6; // poison
        func_pass [7] = 1;
 func_pass [3] = 5 + random2(func_pass [8]) / 80;
        return 2;

 case 17: // negative energy
        strcpy(str_pass, "bolt of negative energy");
        func_pass [1] = random2(10) + 8;
        func_pass [2] = 105 + func_pass [8] / 7;
        func_pass [0] = DARKGRAY;
        func_pass [4] = 35;
        func_pass [6] = 2;//1;
        func_pass [5] = 7; // -ve energy
        func_pass [7] = 1;
 func_pass [3] = 5 + random2(func_pass [8]) / 80;
 return 2;

 case 18: // splinters
        strcpy(str_pass, "spray of metal splinters");
        func_pass [1] = random2(10) + 8;
        func_pass [2] = 115 + func_pass [8] / 7;
        func_pass [0] = CYAN;
        func_pass [4] = 35;
        func_pass [6] = 2;//1;
        func_pass [5] = 17; // whatever
        func_pass [7] = 1;
 func_pass [3] = 55 + random2(func_pass [8]) / 80;
 return 2;

 case 19: // bolt of innacuracy
        strcpy(str_pass, "narrow beam of energy");
        func_pass [1] = random2(10) + 8;
        func_pass [2] = 120 + func_pass [8] / 7;
        func_pass [0] = YELLOW;
        func_pass [4] = 35;
        func_pass [6] = 2;//1;
        func_pass [5] = 17; // whatever
        func_pass [7] = 1;
 func_pass [3] = 1; //1 + random2(func_pass [8]) / 80;
 return 2;

 case 20: // Mystic Bolt
        func_pass [0] = 13;//inv_col [throw_2];//icolour [inv_class [throw_2]] [inv_type [throw_2]];
        strcpy(str_pass, "orb of energy");// inv_name [throw_2]);
        func_pass [1] = random2(5) + 7;
        func_pass [2] = 110 + (func_pass [8] / 10);
        func_pass [3] = 10 + (func_pass [8] / 20);
        func_pass [4] = 35;
        func_pass [6] = 4;//3;
        func_pass [5] = 1; // a magical missile
        func_pass [7] = 1;
        return 1;

        case 21: // charm monster
        strcpy(str_pass, "0");
        func_pass [1] = random2(5) + 6;
        func_pass [4] = 0;
        func_pass [0] = 9;
        func_pass [6] = 2; //1;
 func_pass [2] = 50;
        //beam();
        func_pass [5] = 4; // magic
        return 2;


} // end of switch

return 0;

}




int manage_clouds(char cloud_x [CLOUDS], char cloud_y [CLOUDS], char cloud_type [CLOUDS], int cloud_decay [CLOUDS], int cloud_no, unsigned char grid [80] [70], char time_taken, int haste, int slow)
{

int cc;
int cchck = 0;
int ugy = cloud_no;

for (cc = 0; cc < CLOUDS; cc++)
{

        //if (cchck > cloud_no) break;

        if (cloud_type [cc] == 0) continue;

        cchck++;

        //cout << "Yuvku:";
        //getch();
        //cout << "Hello!";

        if (haste == 0) cloud_decay [cc] -= time_taken;
                else cloud_decay [cc] -= time_taken / 2;

 if ((cloud_type [cc] == 1 && grid [cloud_x [cc]] [cloud_y [cc]] % 50 == 12) | (cloud_type [cc] == 3 && grid [cloud_x [cc]] [cloud_y [cc]] % 50 == 11))
 {
        if (haste == 0) cloud_decay [cc] -= time_taken * 2;
                else cloud_decay [cc] -= time_taken;
 }

if (slow != 0)
{
        cloud_decay [cc] -= time_taken;

 if ((cloud_type [cc] == 1 && grid [cloud_x [cc]] [cloud_y [cc]] % 50 == 12) | (cloud_type [cc] == 3 && grid [cloud_x [cc]] [cloud_y [cc]] % 50 == 11))
 {
        cloud_decay [cc] -= time_taken * 2;
 }
}


        //abort();
        if (cloud_decay [cc] <= 0)
        {
                //abort();
                cloud_type [cc] = 0;
  cloud_decay [cc] = 0;
  grid [cloud_x [cc]] [cloud_y [cc]] -= 100;
                ugy--;

//              cchck--; // <--- maybe?
        }

/*      if (cloud_type [cc] == 100)
        {
                getch();
        }*/

}

//abort();
return ugy;

}





// pass a value for mons_near to this function:


// it's gone!





char *monam(int mons_cla, char mons_e, char desc, char see_invis)
{

char gmo_n [40];

//strcpy(gmo_n, moname(mons_cla, mons_e, see_invis, desc));

moname(mons_cla, mons_e, see_invis, desc, gmo_n);

return gmo_n;

}

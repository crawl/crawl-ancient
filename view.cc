#include "config.h"

#ifdef DOS
#include <conio.h>
#endif

#include <string.h>

#include "externs.h"
#include "mstruct.h"
#include "player.h"
#include "stuff.h"


/*#define menv env[0].mons
#define mit env[0].it[0]*/
#define MINSEE 11

/* for player_see_invis */

void moname(int mcl, char mench, char see_inv, char descrip, char glog [40]);

void item(void);
void monster_grid(void);
void noisy(char loudness, char nois_x, char nois_y);
void cloud_grid(void);
int check_awaken(int mons_aw);
void losight(unsigned int sh [19] [19], unsigned char gr [80] [70], int x_p, int y_p);
unsigned char mapchar(unsigned char ldfk);
unsigned char mapchar2(unsigned char ldfk);
unsigned char mapchar3(unsigned char ldfk);
unsigned char mapchar4(unsigned char ldfk);

unsigned char (*mapch)(unsigned char);
unsigned char (*mapch2)(unsigned char);


unsigned char show_green;
extern int stealth; /* defined in acr.cc */


void viewwindow2(char draw_it)
{

   int bufcount = 0;
   unsigned char buffy [1500]; //[800]; //392];

   unsigned char showed = 0;

   int count_x, count_y;

        _setcursortype(_NOCURSOR);

        losight(env[0].show, env[0].grid, you[0].x_pos, you[0].y_pos);


        for (count_x = 0; count_x < 18; count_x++)
        {
                for (count_y = 0; count_y < 18; count_y++)
                {
                        env[0].show_col [count_x] [count_y] = 7;
                }
        }

        item();
        cloud_grid();
        monster_grid();
        bufcount = 0;


        if (draw_it == 1)
        {

        for (count_y = (you[0].y_pos - 8); (count_y < you[0].y_pos + 9); count_y++)
                {
                 bufcount += 16;
                 for (count_x = (you[0].x_pos - 8); (count_x < you[0].x_pos + 9); count_x++)
                 {
                  buffy [bufcount + 1] = env[0].show_col [count_x - you[0].x_pos + 9] [count_y - you[0].y_pos + 9];


if (count_x != you[0].x_pos | count_y != you[0].y_pos)
{



switch (env[0].show [count_x - you[0].x_pos + 9] [count_y - you[0].y_pos + 9])
        {

                case 0: showed = 0; break;
                case 1: buffy [bufcount + 1] = env[0].rock_colour;
                showed = 177; break; // rock wall - remember earth elementals
                case 2: buffy [bufcount + 1] = LIGHTGRAY;
                showed = 177; break; // stone wall
                case 3: showed = 254; break;
  case 4: showed = 177;
  buffy [bufcount + 1] = CYAN; break;
  case 5: showed = 177; // hidden secret door
  buffy [bufcount + 1] = env[0].rock_colour; break;
  case 6: showed = 177;
  buffy [bufcount + 1] = GREEN; break; // green crystal wall
  case 7: showed = '1';
  buffy [bufcount + 1] = DARKGRAY; // orcish idol
  break;
  case 8: showed = 177;
  buffy [bufcount + 1] = YELLOW; break; // wax wall
  case 21: showed = '1';
  buffy [bufcount + 1] = WHITE; // silver statue
  break;
  case 22: showed = 219; // filled square
  buffy [bufcount + 1] = LIGHTGRAY; // granite monolith
  break;
  case 35: showed = '#'; break;
  case 61: showed = 247;
  buffy [bufcount + 1] = RED; // lava!
  break;
  case 62: showed = 247; // this wavy thing also used for water elemental
  buffy [bufcount + 1] = BLUE; // water
  break;

                case 67: buffy [bufcount + 1] = env[0].floor_colour; //LIGHTGRAY;
                showed = 249; break;
                case 69: showed = 239; buffy [bufcount + 1] = RED; break; // staircase to hell
                case 70: showed = 39; break; // open door
  case 71: showed = 240; buffy [bufcount + 1] = BROWN; break; // branch staircase
                case 75: buffy [bufcount + 1] = 11;
                showed = 94; break; // ^ dart trap
                case 76: buffy [bufcount + 1] = MAGENTA;
                showed = 94; break;
                case 77: buffy [bufcount + 1] = LIGHTGRAY;
                showed = 94; break;
                case 78: showed = 249; buffy [bufcount + 1] = env[0].floor_colour;
                break; // undiscovered trap

  case 80: showed = 239; buffy [bufcount + 1] = YELLOW;
       break;
// if I change anything above here, must also change magic mapping!
  case 81: showed = 239; buffy [bufcount + 1] = LIGHTGRAY; break; // labyrinth
  case 85: buffy [bufcount + 1] = BROWN; // ladder
  case 82:
  case 83:
  case 84:
       showed = '>'; break;
  case 89: buffy [bufcount + 1] = BROWN; // ladder
  case 86:
  case 87:
  case 88:
       showed = '<'; break;

  case 92: buffy [bufcount + 1] = CYAN; showed = 239; break; // Stairway to Dis
  case 93: buffy [bufcount + 1] = RED; showed = 239; break; // Gehenna
  case 94: buffy [bufcount + 1] = LIGHTCYAN; showed = 239; break; // Cocytus
  case 95: buffy [bufcount + 1] = DARKGRAY; showed = 239; break; // Tartarus
  case 96: buffy [bufcount + 1] = LIGHTMAGENTA; showed = 239; break; // To Abyss
  case 97: buffy [bufcount + 1] = LIGHTMAGENTA; showed = 239; break; // From Abyss
  case 98: buffy [bufcount + 1] = LIGHTGRAY; showed = 239; break; // Closed gate to hell
  case 99: buffy [bufcount + 1] = LIGHTBLUE; showed = 239; break; // gate to pandemonium
  case 100: buffy [bufcount + 1] = LIGHTBLUE; showed = 239; break; // gate out of pandemonium
  case 101: buffy [bufcount + 1] = LIGHTGREEN; showed = 239; break; // gate to other part of pandemonium

  case 110:
  case 111:
  case 112:
  case 113:
  case 114:
  case 115:
  case 116: buffy [bufcount + 1] = YELLOW; showed = '>'; break;

  case 117: buffy [bufcount + 1] = MAGENTA; showed = '>'; break;

  case 130:
  case 131:
  case 132:
  case 133:
  case 134:
  case 135:
  case 136: buffy [bufcount + 1] = YELLOW; showed = '<'; break; // stair from the orcish mines to main dungeon

  case 137: buffy [bufcount + 1] = MAGENTA; showed = '<'; break;

                case 256: showed = 9; break;

                case 258: showed = 41; break; // weapon )
                case 259: showed = 91; break; // armour [
                case 260: showed = 47; break; // food %
                case 261: showed = 37; break; // wands / &c
                case 262: showed = 43; break; // books +
                case 263: showed = 63; break; // scroll ?
                case 264: showed = 61; break; // ring = etc
                case 265: showed = 33; break; // potions !
                case 266: showed = 40; break; // stones
                case 267: showed = '+'; break; // book +
                case 268: showed = 37; break; // corpses part 1
  case 269: showed = '\\'; break; // magical staves
  case 270: showed = '}'; break; // gems
  case 271: showed = '%'; break; // don't know ?
                case 272: showed = 36; break; // $ gold

default:
        int mnr = env[0].show [count_x - you[0].x_pos + 9] [count_y - you[0].y_pos + 9];
        if (mnr >= 297) {
                showed = mons_char(mnr - 297); // yeah
        } else
                showed = env[0].show [count_x - you[0].x_pos + 9] [count_y - you[0].y_pos + 9]; break;

                }

} else
      {
/*              showed = your_sign;
              buffy [bufcount + 1] = your_colour;*/
                showed = '@';
                buffy [bufcount + 1] = LIGHTGRAY;
      }


buffy [bufcount] = showed;
// have the colour defined before the switch, because some cases change it.

bufcount += 2;
                }
  bufcount += 16;
        }
  bufcount = 0;

if (you[0].level_type != 1 && you[0].level_type != 2)
{
        for (count_y = 0; count_y < 17; count_y++)
        {
  bufcount += 16;
                for (count_x = 0; count_x < 17; count_x++)
                {
   if (buffy [bufcount] != 0) env[0].map [count_x + you[0].x_pos - 9] [count_y + you[0].y_pos - 9] = buffy [bufcount];
   bufcount += 2;
                }
  bufcount += 16;
        }
}


bufcount = 0;
  for (count_y = 0; count_y < 17; count_y ++)
                {
   for (count_x = 0; count_x < 33; count_x ++)
                        {
       if (count_x + you[0].x_pos - 17 < 3 | count_y + you[0].y_pos - 9 < 3 | count_x + you[0].x_pos - 14 > 77 | count_y + you[0].y_pos - 9 > 67)
       {
            buffy [bufcount] = 0;
            bufcount ++;
            buffy [bufcount] = 0;
            bufcount ++;
            continue;
       }
       if (count_x >= 8 && count_x <= 24 && count_y >= 0 && count_y <= 16 && buffy [bufcount] != 0)
       {
            bufcount += 2;
            continue;
       }
         buffy [bufcount] = env[0].map [count_x + you[0].x_pos - 17] [count_y + you[0].y_pos - 9];
         buffy [bufcount + 1] = DARKGRAY;
                                                                        bufcount += 2;
                        }
                }

if (you[0].berserker != 0)
{
 for (count_x = 1; count_x < 1400; count_x += 2)
 {
  if (buffy [count_x] != DARKGREY) buffy [count_x] = RED;
 }
}

if (show_green != 0)
{
 for (count_x = 1; count_x < 1400; count_x += 2)
 {
  if (buffy [count_x] != DARKGREY) buffy [count_x] = show_green;
 }
 show_green = 0;
 if (you[0].special_wield == 50) show_green = DARKGREY;
}



#ifdef DOS
puttext(2,1,34,17, buffy);
#endif

#ifdef LINUX
gotoxy(2,1);
bufcount = 0;

if (you[0].running == 0) // this line is purely optional
{
 for (count_x = 0; count_x < 1120; count_x += 2) // 1056
 {
  textcolor(buffy [count_x + 1]);
  putch(buffy [count_x]);
  if (count_x % 66 == 64 && count_x > 0)
#ifdef DOS
    cprintf("\n\r ");
#endif
#ifdef LINUX
    gotoxy(2, wherey() + 1);
#endif

 }
}
#endif

        } // end of (if brek...)

}








void monster_grid(void)
{

int s; // a loop thing
int mnc = 0;

for (s = 0; s < MNST; s++)
{

//if (m_class->menv [s].m_class != -1)
if (menv [s].m_class != -1)
{
        mnc++;

        if (menv [s].m_x > you[0].x_pos - 9 && menv [s].m_x < you[0].x_pos + 9 && menv [s].m_y > you[0].y_pos - 9 && menv [s].m_y < you[0].y_pos + 9 && env[0].show [menv [s].m_x - you[0].x_pos + 9] [menv [s].m_y - you[0].y_pos + 9] != 0)
        // Put the bit commented out on the previous line back to restore shadow checking for monsters
        {
        if ((menv [s].m_beh == 0 | menv [s].m_beh == 2) && check_awaken(s) == 1)
 {
  menv [s].m_beh = 1; // put stealth/you[0].invis here.
  menv [s].m_targ_1_x = you[0].x_pos;
  menv [s].m_targ_1_y = you[0].y_pos;
  if (you[0].turnover == 1 && mons_shouts(menv [s].m_class) > 0)
  {
  // two-headed ogre
  switch(mons_shouts(menv [s].m_class))
  {
   case 1: mpr("You hear a shout!"); break;
   case 2: mpr("You hear a bark!"); break;
   case 3: mpr("You hear two shouts!"); break;
   case 4: mpr("You hear a roar!"); break;
   case 5: mpr("You hear a hideous shriek!"); break;
   case 6: mpr("You hear a bellow!"); break;
   case 7: mpr("You hear a screech!"); break;
   case 8: mpr("You hear an angry buzzing noise."); break;
   case 9: mpr("You hear a chilling moan."); break;
  }
   noisy(8, menv [s].m_x, menv [s].m_y);
 }
 }

/*      if (mons [s].m_ench [2] == 6 && (you[0].see_invis == 0 | (mons [s].m_class >= MLAVA0 && mons [s].m_sec == 1)))*/
        if (menv [s].m_ench [2] == 6 && (player_see_invis() == 0 | (menv [s].m_class >= MLAVA0 && menv [s].m_sec == 1)))
        {
                continue;
        } else if (menv [s].m_beh != 7) you[0].running = 0;
        env[0].show [menv [s].m_x - you[0].x_pos + 9] [menv [s].m_y - you[0].y_pos + 9] = menv [s].m_class + 297;
        env[0].show_col [menv [s].m_x - you[0].x_pos + 9] [menv [s].m_y - you[0].y_pos + 9] = mcolour [menv [s].m_class];

        if (mcolour [menv [s].m_class] == BLACK) env[0].show_col [menv [s].m_x - you[0].x_pos + 9] [menv [s].m_y - you[0].y_pos + 9] = menv [s].m_sec;

        }
}
} // end of for i above

} // really the end of monster_grid







int check_awaken(int mons_aw)
{
int mons_perc = 0;

mons_perc = (mons_intel(menv [mons_aw].m_class) * 4) + menv [mons_aw].m_HD + mons_see_invis(menv [mons_aw].m_class) * 5;
// I assume that creatures who can see invisible are very perceptive
mons_perc += 10;

if (you[0].invis != 0 && mons_see_invis(menv[mons_aw].m_class) == 0)
  mons_perc -= 75;

if (mons_perc <= 0) mons_perc = 0;

if (random2(stealth) <= mons_perc) return 1;

return 0;

}






void item(void)
{

char count_x, count_y;

for (count_y = (you[0].y_pos - 8); (count_y < you[0].y_pos + 9); count_y++)
{
        for (count_x = (you[0].x_pos - 8); (count_x < you[0].x_pos + 9); count_x++)
        {
                if (env[0].igrid [count_x] [count_y] != 501)
                {
                        if (env[0].show [count_x - you[0].x_pos + 9] [count_y - you[0].y_pos + 9] != 0)
                        {
                                env[0].show_col [count_x - you[0].x_pos + 9] [count_y - you[0].y_pos + 9] = mitm.icol [env[0].igrid [count_x] [count_y]];
                                switch(mitm.iclass [env[0].igrid [count_x] [count_y]])
                                {
                                case 0: env[0].show [count_x - you[0].x_pos + 9] [count_y - you[0].y_pos + 9] = 258; break;
                                // need + 6 because show is 0 - 12, not -6 - +6
                                case 1: env[0].show [count_x - you[0].x_pos + 9] [count_y - you[0].y_pos + 9] = 258; break;
                                case 2: env[0].show [count_x - you[0].x_pos + 9] [count_y - you[0].y_pos + 9] = 259; break;
                                case 3: env[0].show [count_x - you[0].x_pos + 9] [count_y - you[0].y_pos + 9] = 260; break;
                                case 4: env[0].show [count_x - you[0].x_pos + 9] [count_y - you[0].y_pos + 9] = 261; break;
                                case 5: env[0].show [count_x - you[0].x_pos + 9] [count_y - you[0].y_pos + 9] = 262; break;
                                case 6: env[0].show [count_x - you[0].x_pos + 9] [count_y - you[0].y_pos + 9] = 263; break;
                                case 7: env[0].show [count_x - you[0].x_pos + 9] [count_y - you[0].y_pos + 9] = 264; break;
                                case 8: env[0].show [count_x - you[0].x_pos + 9] [count_y - you[0].y_pos + 9] = 265; break;
                                case 9: env[0].show [count_x - you[0].x_pos + 9] [count_y - you[0].y_pos + 9] = 266; break;
                                case 10: env[0].show [count_x - you[0].x_pos + 9] [count_y - you[0].y_pos + 9] = 267; break;
                                case 11: env[0].show [count_x - you[0].x_pos + 9] [count_y - you[0].y_pos + 9] = 269; break;
                                case 12: env[0].show [count_x - you[0].x_pos + 9] [count_y - you[0].y_pos + 9] = 256; break;
    case 13: env[0].show [count_x - you[0].x_pos + 9] [count_y - you[0].y_pos + 9] = 270; break;
    case 14: env[0].show [count_x - you[0].x_pos + 9] [count_y - you[0].y_pos + 9] = 271; break;
                                case 15: env[0].show [count_x - you[0].x_pos + 9] [count_y - you[0].y_pos + 9] = 272;
                                         env[0].show_col [count_x - you[0].x_pos + 9] [count_y - you[0].y_pos + 9] = YELLOW; break;
                                default: env[0].show [count_x - you[0].x_pos + 9] [count_y - you[0].y_pos + 9] = '8'; break;
                                }
                        }
                }
        } // end of for count_x
} // end of for count_y


} // end of item()




void cloud_grid(void)
{
int s; // a loop thing
int mnc = 0;

for (s = 0; s < CLOUDS; s++)
{

if (mnc > env[0].cloud_no) break;

if (env[0].cloud_type [s] != 0)
{
        mnc++;

        if (env[0].cloud_x [s] > you[0].x_pos - 9 && env[0].cloud_x [s] < you[0].x_pos + 9 && env[0].cloud_y [s] > you[0].y_pos - 9 && env[0].cloud_y [s] < you[0].y_pos + 9 && env[0].show [env[0].cloud_x [s] - you[0].x_pos + 9] [env[0].cloud_y [s] - you[0].y_pos + 9] != 0)
        // Put the bit commented out on the previous line back to restore shadow checking for clouds
        {
        env[0].show [env[0].cloud_x [s] - you[0].x_pos + 9] [env[0].cloud_y [s] - you[0].y_pos + 9] = 35;

   switch(env[0].cloud_type [s] % 100)
   {
        case 1:
              if (env[0].cloud_decay [s] <= 20 | random2(4) == 0)
              {
                env[0].show_col [env[0].cloud_x [s] - you[0].x_pos + 9] [env[0].cloud_y [s] - you[0].y_pos + 9] = RED;
                break;
              }
              if (env[0].cloud_decay [s] <= 40 | random2(4) == 0)
              {
                env[0].show_col [env[0].cloud_x [s] - you[0].x_pos + 9] [env[0].cloud_y [s] - you[0].y_pos + 9] = LIGHTRED;
                break;
              }
              env[0].show_col [env[0].cloud_x [s] - you[0].x_pos + 9] [env[0].cloud_y [s] - you[0].y_pos + 9] = YELLOW; break;
        case 2: env[0].show_col [env[0].cloud_x [s] - you[0].x_pos + 9] [env[0].cloud_y [s] - you[0].y_pos + 9] = GREEN; break;
  case 3: //show_col [cloud_x [s] - you[0].x_pos + 9] [cloud_y [s] - you[0].y_pos + 9] = WHITE; break;
              if (env[0].cloud_decay [s] <= 20 | random2(4) == 0)
              {
                env[0].show_col [env[0].cloud_x [s] - you[0].x_pos + 9] [env[0].cloud_y [s] - you[0].y_pos + 9] = BLUE;
                break;
              }
              if (env[0].cloud_decay [s] <= 40 | random2(4) == 0)
              {
                env[0].show_col [env[0].cloud_x [s] - you[0].x_pos + 9] [env[0].cloud_y [s] - you[0].y_pos + 9] = LIGHTBLUE;
                break;
              }
              env[0].show_col [env[0].cloud_x [s] - you[0].x_pos + 9] [env[0].cloud_y [s] - you[0].y_pos + 9] = WHITE; break;
       case 4:
            if (random2(3) != 0) env[0].show_col [env[0].cloud_x [s] - you[0].x_pos + 9] [env[0].cloud_y [s] - you[0].y_pos + 9] = GREEN;
            else env[0].show_col [env[0].cloud_x [s] - you[0].x_pos + 9] [env[0].cloud_y [s] - you[0].y_pos + 9] = LIGHTGREEN;
              break;
        case 5: env[0].show_col [env[0].cloud_x [s] - you[0].x_pos + 9] [env[0].cloud_y [s] - you[0].y_pos + 9] = LIGHTGRAY; break;
  case 6: env[0].show_col [env[0].cloud_x [s] - you[0].x_pos + 9] [env[0].cloud_y [s] - you[0].y_pos + 9] = LIGHTBLUE; break;
  case 7: env[0].show_col [env[0].cloud_x [s] - you[0].x_pos + 9] [env[0].cloud_y [s] - you[0].y_pos + 9] = MAGENTA; break;
  case 8: env[0].show_col [env[0].cloud_x [s] - you[0].x_pos + 9] [env[0].cloud_y [s] - you[0].y_pos + 9] = LIGHTGRAY; break;
  case 9: env[0].show_col [env[0].cloud_x [s] - you[0].x_pos + 9] [env[0].cloud_y [s] - you[0].y_pos + 9] = DARKGRAY; break; // miasma
   }

        }
}
} // end of for i above



} // end of cloudter_grid()





















// All items must have show values >= 38, all grid squares must be < 38
// because of monster invisibility.



































void noisy(char loudness, char nois_x, char nois_y)
{

int p = 0;

 for (p = 0; p < MNST; p ++)
 {
        if (menv [p].m_x >= nois_x - loudness && menv [p].m_x <= nois_x + loudness && menv [p].m_y >= nois_y - loudness && menv [p].m_y <= nois_y + loudness)
        {
         if (menv [p].m_beh == 0) menv [p].m_beh = 1;
         menv [p].m_targ_1_x = nois_x;
         menv [p].m_targ_1_y = nois_y;
        }
 }

} // end of void noisy_2()





void losight(unsigned int sh [19] [19], unsigned char gr [80] [70], int x_p, int y_p)
{

char shad;
char see_section;
char startPoint_x; // = 8;
char startPoint_y; // = 7;
char behind = 0;
char xs; // the multiplier of the x addition thing
char ys;
char cx = 0;
char cy = 0;

short int see; // see = 1 means 'visible'











// first comes the horizontal east:
see = 1;
for (cx = (x_p + 1); (cx < x_p + 9); cx++)
        {
                if (gr [cx - 1] [y_p] <MINSEE ) see = 0;
                if (see == 1)
                {
                        sh [cx - x_p + 9] [9] = gr [cx] [y_p];
                } else sh [cx - x_p + 9] [9] = 0;
        }

// now the horizontal West:
see = 1;
for (cx = (x_p - 1); (cx > x_p - 9); cx--)
        {
                if (gr [cx + 1] [y_p] <MINSEE) see = 0;
                if (see == 1)
                {
                        sh [cx - x_p + 9] [9] = gr [cx] [y_p];
                } else sh [cx - x_p + 9] [9] = 0;
        }

// now for the North:
see = 1;
for (cy = (y_p - 1); (cy > y_p - 9); cy--)
        {
                if (gr [x_p] [cy + 1] <MINSEE) see = 0;
                if (see == 1)
                {
                        sh [9] [cy - y_p + 9] = gr [x_p] [cy];
                } else sh [9] [cy - y_p + 9] = 0;
        }

// and the South...

see = 1;
for (cy = (y_p + 1); (cy < y_p + 9); cy++)
        {
                if (gr [x_p] [cy - 1] <MINSEE) see = 0;
                if (see == 1)
                {
                        sh [9] [cy - y_p + 9] = gr [x_p] [cy];
                } else sh [9] [cy - y_p + 9] = 0;
        }

// Try the Southeast:

see = 1;
cy = y_p + 1;
for (cx = x_p + 1; cx < x_p + 7; cx++)
{
        if (gr [cx - 1] [cy - 1] <MINSEE) see = 0;
                if (see == 1)
                {
                        sh [cx - x_p + 9] [cy - y_p + 9] = gr [cx] [cy];
                } else sh [cx - x_p + 9] [cy - y_p + 9] = 0;
cy ++;
}

// Now for the Northeast:

see = 1;
cy = y_p - 1;
for (cx = x_p + 1; cx < x_p + 7; cx++)
{
        if (gr [cx - 1] [cy + 1] <MINSEE) see = 0;
                if (see == 1)
                {
                        sh [cx - x_p + 9] [cy - y_p + 9] = gr [cx] [cy];
                } else sh [cx - x_p + 9] [cy - y_p + 9] = 0;
cy --;
}

// The Northwest:

see = 1;
cy = y_p - 1;
for (cx = x_p - 1; cx > x_p - 7; cx--)
{
        if (gr [cx + 1] [cy + 1] <MINSEE) see = 0;
                if (see == 1)
                {
                        sh [cx - x_p + 9] [cy - y_p + 9] = gr [cx] [cy];
                } else sh [cx - x_p + 9] [cy - y_p + 9] = 0;
cy --;
}

// And the Southwest

see = 1;
cy = y_p + 1;
for (cx = x_p - 1; cx > x_p - 7; cx--)
{
        if (gr [cx + 1] [cy - 1] <MINSEE) see = 0;
                if (see == 1)
                {
                        sh [cx - x_p + 9] [cy - y_p + 9] = gr [cx] [cy];
                } else sh [cx - x_p + 9] [cy - y_p + 9] = 0;
cy ++;
}




// Anyway, now for the Fun part!

see = 1;



// int three; // etc
// int four; // whatever


for (shad = 1; shad < 5; shad++)
{

if (shad == 1)
{
        xs = 1;
        ys = 1;
        startPoint_x = 11; startPoint_y = 10;
}

if (shad == 2)
{
        xs = 1;
        ys = -1;
        startPoint_x = 11; startPoint_y = 8;
}

if (shad == 3)
{
        xs = -1;
        ys = 1;
        startPoint_x = 7; startPoint_y = 10;
}

if (shad == 4)
{
        xs = -1;
        ys = -1;
        startPoint_x = 7; startPoint_y = 8;

}

behind = 0;
see = 1;
see_section = 1;

        if (gr [x_p + xs] [y_p + ys] <MINSEE && gr [x_p + xs] [y_p] <MINSEE) see_section = 0;

see = see_section;

        if (see == 1) sh [startPoint_x] [startPoint_y] = gr [x_p + startPoint_x - 9] [y_p + startPoint_y - 9]; else  sh [startPoint_x] [startPoint_y] = 0;

        if (gr [x_p + startPoint_x - 9] [y_p + startPoint_y - 9] <MINSEE) see = 0;

        if (see == 1) sh [startPoint_x + xs] [startPoint_y + ys] =  gr [x_p + startPoint_x + xs - 9] [y_p + startPoint_y + ys - 9]; else sh [startPoint_x + xs] [startPoint_y + ys] = 0;

        if (gr [x_p + startPoint_x + xs - 9] [y_p + startPoint_y + ys - 9] <MINSEE) see = 0;

        if (see == 1) sh [startPoint_x + 2*xs] [startPoint_y + 2*ys] =  gr [x_p + startPoint_x + 2*xs - 9] [y_p + startPoint_y + 2*ys - 9]; else sh [startPoint_x + 2*xs] [startPoint_y + 2*ys] = 0;

        if (gr [x_p + startPoint_x + 2*xs - 9] [y_p + startPoint_y + 2*ys - 9] <MINSEE) see = 0;

        if (see == 1) sh [startPoint_x + 3*xs] [startPoint_y + 3*ys] =  gr [x_p + startPoint_x + 3*xs - 9] [y_p + startPoint_y + 3*ys - 9]; else sh [startPoint_x + 3*xs] [startPoint_y + 3*ys] = 0;

        if (see == 1) sh [startPoint_x + 3*xs] [startPoint_y + 2*ys] =  gr [x_p + startPoint_x + 3*xs - 9] [y_p + startPoint_y + 2*ys - 9]; else sh [startPoint_x + 3*xs] [startPoint_y + 2*ys] = 0;

// Wider:

        // This is done in a different way: see the >= MINSEE instead of < MINSEE

        if (gr [x_p + startPoint_x + 3*xs - 9] [y_p + startPoint_y + 3*ys - 9] >= MINSEE) //see = 0;
        {
                if (see == 1) sh [startPoint_x + 4*xs] [startPoint_y + 4*ys] =  gr [x_p + startPoint_x + 4*xs - 9] [y_p + startPoint_y + 4*ys - 9]; else
  {
                        sh [startPoint_x + 4*xs] [startPoint_y + 4*ys] = 0;
   see = 0;
  }
        } else
       {
        sh [startPoint_x + 4*xs] [startPoint_y + 4*ys] = 0;
        see = 0;
       }
/*  // new
  if (gr [x_p + startPoint_x - 9] [y_p + startPoint_y - 9] < MINSEE) see = 0;
  if (gr [x_p + startPoint_x + 4*xs - 9] [y_p + startPoint_y + 5*ys - 9] < MINSEE) see = 0;
  if (gr [x_p + startPoint_x + 4*xs - 9] [y_p + startPoint_y + 4*ys - 9] < MINSEE) see = 0;

  if (see == 1) sh [startPoint_x + 4*xs] [startPoint_y + 6*ys] =  gr [x_p + startPoint_x + 4*xs - 9] [y_p + startPoint_y + 6*ys - 9]; else sh [startPoint_x + 4*xs] [startPoint_y + 6*ys] = 0;
*/

/*  // new
  if (sh [startPoint_x] [startPoint_y] == 0) see = 0;
  if (sh [startPoint_x + 4*xs] [startPoint_y + 5*ys] == 0)// see = 0;
  if (!(gr [x_p + startPoint_x + 4*xs - 9] [y_p + startPoint_y + 5*ys - 9] < MINSEE))
  {
//see = 0;
   if (!(gr [x_p + startPoint_x + 4*xs - 9] [y_p + startPoint_y + 4*ys - 9] < MINSEE))
//see = 0;
   {
    if (see == 1) sh [startPoint_x + 4*xs] [startPoint_y + 6*ys] =  gr [x_p + startPoint_x + 4*xs - 9] [y_p + startPoint_y + 6*ys - 9]; else sh [startPoint_x + 4*xs] [startPoint_y + 6*ys] = 0;
   } else sh [startPoint_x + 4*xs] [startPoint_y + 6*ys] = 0;
  } else sh [startPoint_x + 4*xs] [startPoint_y + 6*ys] = 0;
  else sh [startPoint_x + 4*xs] [startPoint_y + 6*ys] = 0;
*/
  see = 1;
//  if (sh [startPoint_x] [startPoint_y] == 0 && sh [startPoint_x + xs] [startPoint_y] == 0) see = 0;

//  if (sh [startPoint_x] [startPoint_y] == 0) see = 0;
  if (sh [startPoint_x + 2*xs] [startPoint_y + 5*ys] != 0) //see = 0;
  if (!(gr [x_p + startPoint_x + 2*xs - 9] [y_p + startPoint_y + 5*ys - 9] < MINSEE)) //see = 0;
  {
  if (!(gr [x_p + startPoint_x + 2*xs - 9] [y_p + startPoint_y + 4*ys - 9] < MINSEE)) //see = 0;
   {
    if (see == 1) sh [startPoint_x + 3*xs] [startPoint_y + 6*ys] =  gr [x_p + startPoint_x + 3*xs - 9] [y_p + startPoint_y + 6*ys - 9]; else sh [startPoint_x + 3*xs] [startPoint_y + 6*ys] = 0;
   } else sh [startPoint_x + 3*xs] [startPoint_y + 6*ys] = 0;
  } else sh [startPoint_x + 3*xs] [startPoint_y + 6*ys] = 0;
  else sh [startPoint_x + 3*xs] [startPoint_y + 6*ys] = 0;

//  if (see == 0) sh [startPoint_x + 3*xs] [startPoint_y + 6*ys] = 0;


//  if (sh [startPoint_x] [startPoint_y] == 0) see = 0;
  if (sh [startPoint_x + 1*xs] [startPoint_y + 5*ys] != 0) //see = 0;
  if (!(gr [x_p + startPoint_x + 1*xs - 9] [y_p + startPoint_y + 5*ys - 9] < MINSEE)) //see = 0;
  {
   if (!(gr [x_p + startPoint_x + 1*xs - 9] [y_p + startPoint_y + 4*ys - 9] < MINSEE)) //see = 0;
   {
    if (see == 1) sh [startPoint_x + 2*xs] [startPoint_y + 6*ys] =  gr [x_p + startPoint_x + 2*xs - 9] [y_p + startPoint_y + 6*ys - 9]; else sh [startPoint_x + 2*xs] [startPoint_y + 6*ys] = 0;
   } else sh [startPoint_x + 2*xs] [startPoint_y + 6*ys] = 0;
  } else sh [startPoint_x + 2*xs] [startPoint_y + 6*ys] = 0;
  else sh [startPoint_x + 2*xs] [startPoint_y + 6*ys] = 0;

//  if (see == 0) sh [startPoint_x + 2*xs] [startPoint_y + 6*ys] = 0;
//  if (sh [startPoint_x] [startPoint_y] == 0) see = 0;


  if (sh [startPoint_x] [startPoint_y + 5*ys] != 0) //see = 0;
  if (!(gr [x_p + startPoint_x - 9] [y_p + startPoint_y + 5*ys - 9] < MINSEE)) //see = 0;
  {
   if (!(gr [x_p + startPoint_x - 9] [y_p + startPoint_y + 4*ys - 9] < MINSEE)) //see = 0;
   {
    if (see == 1) sh [startPoint_x + xs] [startPoint_y + 6*ys] =  gr [x_p + startPoint_x + xs - 9] [y_p + startPoint_y + 6*ys - 9]; else sh [startPoint_x + xs] [startPoint_y + 6*ys] = 0;
   } else sh [startPoint_x + xs] [startPoint_y + 6*ys] = 0;
  } else sh [startPoint_x + xs] [startPoint_y + 6*ys] = 0;
  else sh [startPoint_x + xs] [startPoint_y + 6*ys] = 0;

//  if (see == 0) sh [startPoint_x + xs] [startPoint_y + 6*ys] = 0;

  if (sh [startPoint_x - xs] [startPoint_y + 5*ys] != 0) //see = 0;
  if (!(gr [x_p + startPoint_x - xs - 9] [y_p + startPoint_y + 5*ys - 9] < MINSEE)) //see = 0;
  {
   if (!(gr [x_p + startPoint_x - xs - 9] [y_p + startPoint_y + 4*ys - 9] < MINSEE)) //see = 0;
   {
    if (see == 1) sh [startPoint_x] [startPoint_y + 6*ys] =  gr [x_p + startPoint_x - 9] [y_p + startPoint_y + 6*ys - 9]; else sh [startPoint_x] [startPoint_y + 6*ys] = 0;
   } else sh [startPoint_x] [startPoint_y + 6*ys] = 0;
  } else sh [startPoint_x] [startPoint_y + 6*ys] = 0;
  else sh [startPoint_x] [startPoint_y + 6*ys] = 0;


  if (sh [startPoint_x - 2*xs] [startPoint_y + 5*ys] != 0) //see = 0;
  if (!(gr [x_p + startPoint_x - 2*xs - 9] [y_p + startPoint_y + 5*ys - 9] < MINSEE)) //see = 0;
  {
   if (!(gr [x_p + startPoint_x - 2*xs - 9] [y_p + startPoint_y + 4*ys - 9] < MINSEE)) //see = 0;
   {
    if (see == 1) sh [startPoint_x - xs] [startPoint_y + 6*ys] =  gr [x_p + startPoint_x - xs - 9] [y_p + startPoint_y + 6*ys - 9]; else sh [startPoint_x - xs] [startPoint_y + 6*ys] = 0;
   } else sh [startPoint_x - xs] [startPoint_y + 6*ys] = 0;
  } else sh [startPoint_x - xs] [startPoint_y + 6*ys] = 0;
  else sh [startPoint_x - xs] [startPoint_y + 6*ys] = 0;






  if (sh [startPoint_x - xs] [startPoint_y + 6*ys] != 0) //see = 0;
  if (!(gr [x_p + startPoint_x - xs - 9] [y_p + startPoint_y + 6*ys - 9] < MINSEE)) //see = 0;
  {
   if (!(gr [x_p + startPoint_x - xs - 9] [y_p + startPoint_y + 5*ys - 9] < MINSEE)) //see = 0;
   {
    if (see == 1) sh [startPoint_x] [startPoint_y + 7*ys] =  gr [x_p + startPoint_x - 9] [y_p + startPoint_y + 7*ys - 9]; else sh [startPoint_x] [startPoint_y + 7*ys] = 0;
   } else sh [startPoint_x] [startPoint_y + 7*ys] = 0;
  } else sh [startPoint_x] [startPoint_y + 7*ys] = 0;
  else sh [startPoint_x] [startPoint_y + 7*ys] = 0;


  if (sh [startPoint_x - 2*xs] [startPoint_y + 6*ys] != 0) //see = 0;
  if (!(gr [x_p + startPoint_x - 2*xs - 9] [y_p + startPoint_y + 6*ys - 9] < MINSEE)) //see = 0;
  {
   if (!(gr [x_p + startPoint_x - 2*xs - 9] [y_p + startPoint_y + 5*ys - 9] < MINSEE)) //see = 0;
   {
    if (see == 1) sh [startPoint_x - xs] [startPoint_y + 7*ys] =  gr [x_p + startPoint_x - xs - 9] [y_p + startPoint_y + 7*ys - 9]; else sh [startPoint_x - xs] [startPoint_y + 7*ys] = 0;
   } else sh [startPoint_x - xs] [startPoint_y + 7*ys] = 0;
  } else sh [startPoint_x - xs] [startPoint_y + 7*ys] = 0;
  else sh [startPoint_x - xs] [startPoint_y + 7*ys] = 0;


/*  if (sh [startPoint_x - 3*xs] [startPoint_y + 6*ys] != 0) //see = 0;
  if (!(gr [x_p + startPoint_x - 3*xs - 9] [y_p + startPoint_y + 6*ys - 9] < MINSEE)) //see = 0;
  {
   if (!(gr [x_p + startPoint_x - 3*xs - 9] [y_p + startPoint_y + 5*ys - 9] < MINSEE)) //see = 0;
   {
    if (see == 1) sh [startPoint_x - 2*xs] [startPoint_y + 7*ys] =  gr [x_p + startPoint_x - 2*xs - 9] [y_p + startPoint_y + 7*ys - 9]; else sh [startPoint_x - 2*xs] [startPoint_y + 7*ys] = 0;
   } else sh [startPoint_x - 2*xs] [startPoint_y + 7*ys] = 0;
  } else sh [startPoint_x - 2*xs] [startPoint_y + 7*ys] = 0;
  else sh [startPoint_x - 2*xs] [startPoint_y + 7*ys] = 0;
*/



// That's one line done...

see_section = 1;

        if (gr [x_p + xs] [y_p + ys] <MINSEE && gr [x_p + xs] [y_p] <MINSEE) see_section = 0;

        if (gr [x_p + 2*xs] [y_p + ys] <MINSEE && gr [x_p + 2*xs] [y_p] <MINSEE) see_section = 0;

see = see_section;

        if (see == 1) sh [startPoint_x + xs] [startPoint_y] = gr [x_p + startPoint_x + xs - 9] [y_p + startPoint_y - 9]; else sh [startPoint_x + xs] [startPoint_y] = 0;

        if (gr [x_p + startPoint_x + xs - 9] [y_p + startPoint_y - 9] <MINSEE) see = 0;

                if (see == 1 && gr [x_p + startPoint_x + 2*xs - 9] [y_p + startPoint_y - 9] >= MINSEE)
                {
                        sh [startPoint_x + 3*xs] [startPoint_y + ys] = gr [x_p + startPoint_x + 3*xs - 9] [y_p + startPoint_y + ys - 9];
                        // Wider:
                        if (gr [x_p + startPoint_x + 3*xs - 9] [y_p + startPoint_y - 9] >= MINSEE)
                        {
                                                sh [startPoint_x + 4*xs] [startPoint_y + ys] = gr [x_p + startPoint_x + 4*xs - 9] [y_p + startPoint_y + ys - 9]; //else sh [startPoint_x + 4*xs] [startPoint_y + ys] = 0;
                        } else sh [startPoint_x + 4*xs] [startPoint_y + ys] = 0;
                        // Okay.

                } else
                {
                        sh [startPoint_x + 3*xs] [startPoint_y + ys] = 0;
                        sh [startPoint_x + 4*xs] [startPoint_y + ys] = 0;
    sh [startPoint_x + 5*xs] [startPoint_y + ys] = 0;
    sh [startPoint_x + 6*xs] [startPoint_y + ys] = 0;
                }

        if (see == 1) sh [startPoint_x + 2*xs] [startPoint_y + ys] = gr [x_p + startPoint_x + 2*xs - 9] [y_p + startPoint_y + ys - 9]; else sh [startPoint_x + 2*xs] [startPoint_y + ys] = 0;

        if (gr [x_p + startPoint_x + 2*xs - 9] [y_p + startPoint_y + ys - 9] < MINSEE) see = 0;

        if (see == 1) sh [startPoint_x + 3*xs] [startPoint_y + 2*ys] = gr [x_p + startPoint_x + 3*xs - 9] [y_p + startPoint_y + 2*ys - 9]; else sh [startPoint_x + 3*xs] [startPoint_y + 2*ys] = 0;

// Wider:

        if (gr [x_p + startPoint_x + 3*xs - 9] [y_p + startPoint_y + 2*ys - 9] >= MINSEE) //see = 0;
        {
                if (see == 1) sh [startPoint_x + 4*xs] [startPoint_y + 3*ys] = gr [x_p + startPoint_x + 4*xs - 9] [y_p + startPoint_y + 3*ys - 9]; else sh [startPoint_x + 4*xs] [startPoint_y + 3*ys] = 0;
        } else sh [startPoint_x + 4*xs] [startPoint_y + 3*ys] = 0;

        // This should work better:
        if (gr [x_p + startPoint_x + 3*xs - 9] [y_p + startPoint_y + ys - 9] >= MINSEE) //see = 0;
        {
                if (see == 1) sh [startPoint_x + 4*xs] [startPoint_y + 2*ys] = gr [x_p + startPoint_x + 4*xs - 9] [y_p + startPoint_y + 2*ys - 9]; else
  {
   sh [startPoint_x + 4*xs] [startPoint_y + 2*ys] = 0;
   see = 0;
  }
        } else {
         see = 1;
         sh [startPoint_x + 4*xs] [startPoint_y + 2*ys] = 0;
        }

//              if (see == 1) sh [startPoint_x + 5*xs] [startPoint_y + 2*ys] = gr [x_p + startPoint_x + 5*xs - 9] [y_p + startPoint_y + 2*ys - 9]; else sh [startPoint_x + 5*xs] [startPoint_y + 2*ys] = 0;


see = see_section;


// And one more for this section:

        if (gr [x_p + 3*xs] [y_p] < MINSEE && gr [x_p + 3*xs] [y_p + ys] < MINSEE) see_section = 0;

see = see_section;



        if (gr [x_p + startPoint_x + 2*xs - 9] [y_p + startPoint_y - 9] < MINSEE) see = 0;



        if (gr [x_p + 3*xs] [y_p + ys] < MINSEE && gr [x_p + 3*xs] [y_p + 2*ys] < MINSEE ) see_section = 0;


see = see_section;

        if (see == 1) sh [startPoint_x + 2*xs] [startPoint_y] = gr [x_p + startPoint_x + 2*xs - 9] [y_p + startPoint_y - 9]; else sh [startPoint_x + 2*xs] [startPoint_y] = 0;


        if (gr [x_p + 4*xs] [y_p + 2*ys] < MINSEE && gr [x_p + 4*xs] [y_p + ys] < MINSEE) see = 0;




if ((gr [x_p + xs] [y_p] < MINSEE && gr [x_p + xs] [y_p + ys] < MINSEE) | (gr [x_p + 2*xs] [y_p] < MINSEE && gr [x_p + 2*xs] [y_p + ys] < MINSEE) | (gr [x_p + 3*xs] [y_p] < MINSEE && gr [x_p + 3*xs] [y_p + ys] < MINSEE) | (gr [x_p + 4*xs] [y_p] < MINSEE && gr [x_p + 4*xs] [y_p + ys] < MINSEE))
{
        sh [startPoint_x + 3*xs] [startPoint_y] = 0;
        sh [startPoint_x + 4*xs] [startPoint_y] = 0;
} else
        {
                sh [startPoint_x + 3*xs] [startPoint_y] = gr [x_p + startPoint_x + 3*xs - 9] [y_p + startPoint_y - 9];


        if (gr [x_p + 5*xs] [y_p] < MINSEE && gr [x_p + 5*xs] [y_p + ys] < MINSEE)
        {
                sh [startPoint_x + 4*xs] [startPoint_y] = 0;
        } else sh [startPoint_x + 4*xs] [startPoint_y] = gr [x_p + startPoint_x + 4*xs - 9] [y_p + startPoint_y - 9];



        } // end of else







if ((gr [x_p + xs] [y_p] < MINSEE && gr [x_p + xs] [y_p + ys] < MINSEE) | (gr [x_p + 2*xs] [y_p] < MINSEE && gr [x_p + 2*xs] [y_p + ys] < MINSEE) | (gr [x_p + 3*xs] [y_p] < MINSEE && gr [x_p + 3*xs] [y_p + ys] < MINSEE))
{
        sh [startPoint_x + 2*xs] [startPoint_y] = 0;
} else sh [startPoint_x + 2*xs] [startPoint_y] = gr [x_p + startPoint_x + 2*xs - 9] [y_p + startPoint_y - 9];




} // end of the for (i) above.



// The second lot:

for (shad = 1; shad < 5; shad++)
{

if (shad == 1)
{
        xs = 1;
        ys = 1;
//      three = 3;
        startPoint_x = 10; startPoint_y = 11;
}

if (shad == 2)
{
        xs = 1;
        ys = -1;
        startPoint_x = 10; startPoint_y = 7;
}

if (shad == 3)
{
        xs = -1;
        ys = 1;
        startPoint_x = 8; startPoint_y = 11;
}

if (shad == 4)
{
        xs = -1;
        ys = -1;
        startPoint_x = 8; startPoint_y = 7;

}





behind = 0;
see = 1;
see_section = 1;

        if (gr [x_p + xs] [y_p + ys] < MINSEE && gr [x_p] [y_p + ys] < MINSEE) see_section = 0;

see = see_section;

        if (see == 1) sh [startPoint_x] [startPoint_y] = gr [x_p + startPoint_x - 9] [y_p + startPoint_y - 9]; else  sh [startPoint_x] [startPoint_y] = 0;

        if (gr [x_p + startPoint_x - 9] [y_p + startPoint_y - 9] < MINSEE) see = 0;

        if (see == 1) sh [startPoint_x + xs] [startPoint_y + ys] =  gr [x_p + startPoint_x + xs - 9] [y_p + startPoint_y + ys - 9]; else sh [startPoint_x + xs] [startPoint_y + ys] = 0;

        if (gr [x_p + startPoint_x + xs - 9] [y_p + startPoint_y + ys - 9] < MINSEE) see = 0;

        if (see == 1) sh [startPoint_x + 2*xs] [startPoint_y + 2*ys] =  gr [x_p + startPoint_x + 2*xs - 9] [y_p + startPoint_y + 2*ys - 9]; else sh [startPoint_x + 2*xs] [startPoint_y + 2*ys] = 0;

        if (gr [x_p + startPoint_x + 2*xs - 9] [y_p + startPoint_y + 2*ys - 9] < MINSEE) see = 0;

        if (see == 1) sh [startPoint_x + 3*xs] [startPoint_y + 3*ys] =  gr [x_p + startPoint_x + 3*xs - 9] [y_p + startPoint_y + 3*ys - 9]; else sh [startPoint_x + 3*xs] [startPoint_y + 3*ys] = 0;

        if (see == 1) sh [startPoint_x + 2*xs] [startPoint_y + 3*ys] =  gr [x_p + startPoint_x + 2*xs - 9] [y_p + startPoint_y + 3*ys - 9]; else sh [startPoint_x + 2*xs] [startPoint_y + 3*ys] = 0;

// Wider:


        if (gr [x_p + startPoint_x + 3*xs - 9] [y_p + startPoint_y + 3*ys - 9] >= MINSEE) //see = 0;
        {
                if (see == 1) sh [startPoint_x + 4*xs] [startPoint_y + 4*ys] =  gr [x_p + startPoint_x + 4*xs - 9] [y_p + startPoint_y + 4*ys - 9]; else sh [startPoint_x + 4*xs] [startPoint_y + 4*ys] = 0;
        } else sh [startPoint_x + 4*xs] [startPoint_y + 4*ys] = 0;

// Okay.





  see = 1;
  if (sh [startPoint_x + 5*xs] [startPoint_y + 2*ys] != 0) //see = 0;
  if (!(gr [x_p + startPoint_x + 5*xs - 9] [y_p + startPoint_y + 2*ys - 9] < MINSEE)) //see = 0;
  {
  if (!(gr [x_p + startPoint_x + 4*xs - 9] [y_p + startPoint_y + 2*ys - 9] < MINSEE)) //see = 0;
   {
    if (see == 1) sh [startPoint_x + 6*xs] [startPoint_y + 3*ys] =  gr [x_p + startPoint_x + 6*xs - 9] [y_p + startPoint_y + 3*ys - 9]; else sh [startPoint_x + 6*xs] [startPoint_y + 3*ys] = 0;
   } else sh [startPoint_x + 6*xs] [startPoint_y + 3*ys] = 0;
  } else sh [startPoint_x + 6*xs] [startPoint_y + 3*ys] = 0;
  else sh [startPoint_x + 6*xs] [startPoint_y + 3*ys] = 0;



  if (sh [startPoint_x + 5*xs] [startPoint_y + 1*ys] != 0) //see = 0;
  if (!(gr [x_p + startPoint_x + 5*xs - 9] [y_p + startPoint_y + 1*ys - 9] < MINSEE)) //see = 0;
  {
   if (!(gr [x_p + startPoint_x + 4*xs - 9] [y_p + startPoint_y + 1*ys - 9] < MINSEE)) //see = 0;
   {
    if (see == 1) sh [startPoint_x + 6*xs] [startPoint_y + 2*ys] =  gr [x_p + startPoint_x + 6*xs - 9] [y_p + startPoint_y + 2*ys - 9]; else sh [startPoint_x + 6*xs] [startPoint_y + 2*ys] = 0;
   } else sh [startPoint_x + 6*xs] [startPoint_y + 2*ys] = 0;
  } else sh [startPoint_x + 6*xs] [startPoint_y + 2*ys] = 0;
  else sh [startPoint_x + 6*xs] [startPoint_y + 2*ys] = 0;

//  if (see == 0) sh [startPoint_x + 2*xs] [startPoint_y + 6*ys] = 0;
//  if (sh [startPoint_x] [startPoint_y] == 0) see = 0;


  if (sh [startPoint_x + 5*xs] [startPoint_y + 0*ys] != 0) //see = 0;
  if (!(gr [x_p + startPoint_x + 5*xs - 9] [y_p + startPoint_y + 0*ys - 9] < MINSEE)) //see = 0;
  {
   if (!(gr [x_p + startPoint_x + 4*xs - 9] [y_p + startPoint_y + 0*ys - 9] < MINSEE)) //see = 0;
   {
    if (see == 1) sh [startPoint_x + 6*xs] [startPoint_y + 1*ys] =  gr [x_p + startPoint_x + 6*xs - 9] [y_p + startPoint_y + 1*ys - 9]; else sh [startPoint_x + 6*xs] [startPoint_y + 1*ys] = 0;
   } else sh [startPoint_x + 6*xs] [startPoint_y + 1*ys] = 0;
  } else sh [startPoint_x + 6*xs] [startPoint_y + 1*ys] = 0;
  else sh [startPoint_x + 6*xs] [startPoint_y + 1*ys] = 0;

//  if (see == 0) sh [startPoint_x + xs] [startPoint_y + 6*ys] = 0;

  if (sh [startPoint_x + 5*xs] [startPoint_y - 1*ys] != 0) //see = 0;
  if (!(gr [x_p + startPoint_x + 5*xs - 9] [y_p + startPoint_y - 1*ys - 9] < MINSEE)) //see = 0;
  {
   if (!(gr [x_p + startPoint_x + 4*xs - 9] [y_p + startPoint_y - 1*ys - 9] < MINSEE)) //see = 0;
   {
    if (see == 1) sh [startPoint_x + 6*xs] [startPoint_y + 0*ys] =  gr [x_p + startPoint_x + 6*xs - 9] [y_p + startPoint_y + 0*ys - 9]; else sh [startPoint_x + 6*xs] [startPoint_y + 0*ys] = 0;
   } else sh [startPoint_x + 6*xs] [startPoint_y + 0*ys] = 0;
  } else sh [startPoint_x + 6*xs] [startPoint_y + 0*ys] = 0;
  else sh [startPoint_x + 6*xs] [startPoint_y + 0*ys] = 0;


  if (sh [startPoint_x + 5*xs] [startPoint_y - 2*ys] != 0) //see = 0;
  if (!(gr [x_p + startPoint_x + 5*xs - 9] [y_p + startPoint_y - 2*ys - 9] < MINSEE)) //see = 0;
  {
   if (!(gr [x_p + startPoint_x + 4*xs - 9] [y_p + startPoint_y - 2*ys - 9] < MINSEE)) //see = 0;
   {
    if (see == 1) sh [startPoint_x + 6*xs] [startPoint_y - 1*ys] =  gr [x_p + startPoint_x + 6*xs - 9] [y_p + startPoint_y - 1*ys - 9]; else sh [startPoint_x + 6*xs] [startPoint_y - 1*ys] = 0;
   } else sh [startPoint_x + 6*xs] [startPoint_y - 1*ys] = 0;
  } else sh [startPoint_x + 6*xs] [startPoint_y - 1*ys] = 0;
  else sh [startPoint_x + 6*xs] [startPoint_y - 1*ys] = 0;






  if (sh [startPoint_x + 6*xs] [startPoint_y - 1*ys] != 0) //see = 0;
  if (!(gr [x_p + startPoint_x + 6*xs - 9] [y_p + startPoint_y - 1*ys - 9] < MINSEE)) //see = 0;
  {
   if (!(gr [x_p + startPoint_x + 5*xs - 9] [y_p + startPoint_y - 1*ys - 9] < MINSEE)) //see = 0;
   {
    sh [startPoint_x + 7*xs] [startPoint_y + 0*ys] =  gr [x_p + startPoint_x + 7*xs - 9] [y_p + startPoint_y + 0*ys - 9]; //else sh [startPoint_x + 7*xs] [startPoint_y + 0*ys] = 0;
   } else sh [startPoint_x + 7*xs] [startPoint_y + 0*ys] = 0;
  } else sh [startPoint_x + 7*xs] [startPoint_y + 0*ys] = 0;
  else sh [startPoint_x + 7*xs] [startPoint_y + 0*ys] = 0;


  if (sh [startPoint_x + 6*xs] [startPoint_y - 2*ys] != 0) //see = 0;
  if (!(gr [x_p + startPoint_x + 6*xs - 9] [y_p + startPoint_y - 2*ys - 9] < MINSEE)) //see = 0;
  {
   if (!(gr [x_p + startPoint_x + 5*xs - 9] [y_p + startPoint_y - 2*ys - 9] < MINSEE)) //see = 0;
   {
    sh [startPoint_x + 7*xs] [startPoint_y - 1*ys] =  gr [x_p + startPoint_x + 7*xs - 9] [y_p + startPoint_y - 1*ys - 9]; //else sh [startPoint_x + 7*xs] [startPoint_y - 1*ys] = 0;
   } else sh [startPoint_x + 7*xs] [startPoint_y - 1*ys] = 0;
  } else sh [startPoint_x + 7*xs] [startPoint_y - 1*ys] = 0;
  else sh [startPoint_x + 7*xs] [startPoint_y - 1*ys] = 0;


/*  if (sh [startPoint_x + 6*xs] [startPoint_y - 3*ys] != 0)
  if (!(gr [x_p + startPoint_x + 6*xs - 9] [y_p + startPoint_y - 3*ys - 9] < MINSEE))
  {
   if (!(gr [x_p + startPoint_x + 5*xs - 9] [y_p + startPoint_y - 3*ys - 9] < MINSEE))
   {
    sh [startPoint_x + 7*xs] [startPoint_y - 2*ys] =  gr [x_p + startPoint_x + 7*xs - 9] [y_p + startPoint_y - 2*ys - 9]; //else sh [startPoint_x + 7*xs] [startPoint_y - 2*ys] = 0;
   } else sh [startPoint_x + 7*xs] [startPoint_y - 2*ys] = 0;
  } else sh [startPoint_x + 7*xs] [startPoint_y - 2*ys] = 0;
  else sh [startPoint_x + 7*xs] [startPoint_y - 2*ys] = 0;

/ *  if (sh [startPoint_x + 6*xs] [startPoint_y - 3*ys] != 0)
  if (!(gr [x_p + startPoint_x + 6*xs - 9] [y_p + startPoint_y - 3*ys - 9] < MINSEE))
  {
   if (!(gr [x_p + startPoint_x + 5*xs - 9] [y_p + startPoint_y - 3*ys - 9] < MINSEE))
   {
    sh [startPoint_x + 7*xs] [startPoint_y - 3*ys] =  gr [x_p + startPoint_x + 7*xs - 9] [y_p + startPoint_y - 3*ys - 9]; //else sh [startPoint_x + 7*xs] [startPoint_y - 3*ys] = 0;
   } else sh [startPoint_x + 7*xs] [startPoint_y - 3*ys] = 0;
  } else sh [startPoint_x + 7*xs] [startPoint_y - 3*ys] = 0;
  else sh [startPoint_x + 7*xs] [startPoint_y - 3*ys] = 0;
*/



// That's one line done...

        if (gr [x_p + xs] [y_p + ys] < MINSEE && gr [x_p] [y_p + ys] < MINSEE) see_section = 0;

        if (gr [x_p + xs] [y_p + 2*ys] < MINSEE && gr [x_p] [y_p + 2*ys] < MINSEE) see_section = 0;

see = see_section;

        if (see == 1) sh [startPoint_x] [startPoint_y + ys] = gr [x_p + startPoint_x - 9] [y_p + startPoint_y + ys - 9]; else sh [startPoint_x] [startPoint_y + ys] = 0;

        if (gr [x_p + startPoint_x - 9] [y_p + startPoint_y + ys - 9] < MINSEE) see = 0;


        if (see == 1 && gr [x_p + startPoint_x - 9] [y_p + startPoint_y + 2*ys - 9] >= MINSEE)
                {
                        sh [startPoint_x + xs] [startPoint_y + 3*ys] = gr [x_p + startPoint_x + xs - 9] [y_p + startPoint_y + 3*ys - 9];
                        // Wider:
                        if (gr [x_p + startPoint_x - 9] [y_p + startPoint_y + 3*ys - 9] >= MINSEE)
                        {
                                                        sh [startPoint_x + xs] [startPoint_y + 4*ys] = gr [x_p + startPoint_x + xs - 9] [y_p + startPoint_y + 4*ys - 9]; //else sh [startPoint_x + 4*xs] [startPoint_y + ys] = 0;
                        } else sh [startPoint_x + xs] [startPoint_y + 4*ys] = 0;
                        // Okay.
                } else
                {
                        sh [startPoint_x + xs] [startPoint_y + 3*ys] = 0;
                        sh [startPoint_x + xs] [startPoint_y + 4*ys] = 0;
                }


        if (see == 1) sh [startPoint_x + xs] [startPoint_y + 2*ys] = gr [x_p + startPoint_x + xs - 9] [y_p + startPoint_y + 2*ys - 9]; else sh [startPoint_x + xs] [startPoint_y + 2*ys] = 0;

        if (gr [x_p + startPoint_x + xs - 9] [y_p + startPoint_y + 2*ys - 9] < MINSEE) see = 0;

        if (see == 1 && behind == 0) sh [startPoint_x + 2*xs] [startPoint_y + 3*ys] = gr [x_p + startPoint_x + 2*xs - 9] [y_p + startPoint_y + 3*ys - 9]; else sh [startPoint_x + 2*xs] [startPoint_y + 3*ys] = 0;


// Wider:

        if (gr [x_p + startPoint_x + 2*xs - 9] [y_p + startPoint_y + 3*ys - 9] >= MINSEE) //see = 0;
        {
                if (see == 1) sh [startPoint_x + 3*xs] [startPoint_y + 4*ys] = gr [x_p + startPoint_x + 3*xs - 9] [y_p + startPoint_y + 4*ys - 9]; else sh [startPoint_x + 3*xs] [startPoint_y + 4*ys] = 0;
        } else sh [startPoint_x + 3*xs] [startPoint_y + 4*ys] = 0;


        if (gr [x_p + startPoint_x + 2*xs - 9] [y_p + startPoint_y + 3*ys - 9] >= MINSEE | gr [x_p + startPoint_x + xs - 9] [y_p + startPoint_y + 3*ys - 9] >= MINSEE) //see = 0;
        {
                if (see == 1) sh [startPoint_x + 2*xs] [startPoint_y + 4*ys] = gr [x_p + startPoint_x + 2*xs - 9] [y_p + startPoint_y + 4*ys - 9]; else sh [startPoint_x + 2*xs] [startPoint_y + 4*ys] = 0;
        } else sh [startPoint_x + 2*xs] [startPoint_y + 4*ys] = 0;

// Okay.


see = see_section;

// And one more for this section:

        if (gr [x_p ] [y_p + 3*ys] < MINSEE && gr [x_p + xs] [y_p + 3*ys] < MINSEE) see_section = 0;

see = see_section;

        if (see == 1) sh [startPoint_x ] [startPoint_y + 2*ys] = gr [x_p + startPoint_x - 9] [y_p + startPoint_y + 2*ys - 9]; else sh [startPoint_x ] [startPoint_y + 2*ys] = 0;


        if (gr [x_p + startPoint_x - 9] [y_p + startPoint_y + 2*ys - 9] < MINSEE) see = 0;


        if (gr [x_p + xs] [y_p + 3*ys] < MINSEE && gr [x_p + 2*xs] [y_p + 3*ys] < MINSEE) see_section = 0;

see = see_section;

see = see_section;

        if (gr [x_p + 2*xs] [y_p + 4*ys] < MINSEE && gr [x_p + xs] [y_p + 4*ys] < MINSEE) see = 0;




if ((gr [x_p] [y_p + ys] < MINSEE && gr [x_p + xs] [y_p + ys] < MINSEE) | (gr [x_p] [y_p + 2*ys] < MINSEE && gr [x_p + xs] [y_p + 2*ys] < MINSEE) | (gr [x_p] [y_p + 3*ys] < MINSEE && gr [x_p + xs] [y_p + 3*ys] < MINSEE))
{
        sh [startPoint_x] [startPoint_y + 2*ys] = 0;
} else sh [startPoint_x] [startPoint_y + 2*ys] = gr [x_p + startPoint_x - 9] [y_p + startPoint_y + 2*ys - 9];



if ((gr [x_p] [y_p + ys] < MINSEE && gr [x_p + xs] [y_p + ys] < MINSEE) | (gr [x_p] [y_p + 2*ys] < MINSEE && gr [x_p + xs] [y_p + 2*ys] < MINSEE) | (gr [x_p] [y_p + 3*ys] < MINSEE && gr [x_p + xs] [y_p + 3*ys] < MINSEE) | (gr [x_p] [y_p + 4*ys] < MINSEE && gr [x_p + xs] [y_p + 4*ys] < MINSEE))
{
        sh [startPoint_x] [startPoint_y + 3*ys] = 0;
        sh [startPoint_x] [startPoint_y + 4*ys] = 0;
} else
        {
                sh [startPoint_x] [startPoint_y + 3*ys] = gr [x_p + startPoint_x - 9] [y_p + startPoint_y + 3*ys - 9];


        if (gr [x_p] [y_p + 5*ys] < MINSEE && gr [x_p] [y_p + 5*ys] < MINSEE)
        {
                sh [startPoint_x] [startPoint_y + 4*ys] = 0;
        } else sh [startPoint_x] [startPoint_y + 4*ys] = gr [x_p + startPoint_x - 9] [y_p + startPoint_y + 4*ys - 9];

        } // end of else

} // end of the for (shad) above.

//sh [5] [5] = 'X';

}


void draw_border(int bord_col, char your_name [30], char clasnam [40], char tspecies)
{

textcolor(bord_col);
// this bit draws the borders:
#ifdef DOS
window(1,1,80,25);
#endif

clrscr();
gotoxy(40,1);
textcolor(7);
char print_it [80];
char print_it2 [42];

int i = 0;
char spaces = 0;

strcpy(print_it, your_name);
strcat(print_it, " the ");
strcat(print_it, clasnam);

for (i = 0; i < 39; i ++)
{
 print_it2 [i] = print_it [i];
 if (print_it [i] == 0)
  break;
}

for (i = 0; i < 40; i ++)
{
 if (print_it2 [i] == 0) spaces = 1;
 if (spaces == 1) print_it2 [i] = ' ';
}

print_it2 [39] = 0;

textcolor(LIGHTGREY);
#ifdef DOS
window(1,1,80,25);
#endif
gotoxy(40,1);
textcolor(7);
cprintf(print_it2);
gotoxy(40, 2);
cprintf(species_name(tspecies));
gotoxy(40,3);
cprintf("HP:");
gotoxy(40,4);
cprintf("Magic:");
gotoxy(40,5);
cprintf("AC:");
gotoxy(40,6);
cprintf("EV:");

gotoxy(40,7);
cprintf("Str:");
gotoxy(40,8);
cprintf("Int:");
gotoxy(40,9);
cprintf("Dex:");
gotoxy(40,10);
cprintf("Gold:");
gotoxy(40,11);
cprintf("Experience:");
gotoxy(40,12);
cprintf("Level");

} // end of void draw_border(int bord_col)



void show_map(int spec_place [2])
{
   int curs_x = you[0].x_pos;
   int curs_y = 12;
   int screen_y = you[0].y_pos;

   int i;
   int bufcount2 = 0;
   int j;

   char move_x = 0;
   char move_y = 0;
   char getty = 0;
char buffer[4800];
char buffer2[4800];


#ifdef DOS
  gettext(1, 1, 80, 25, buffer);
   window(1, 1, 80, 25);
#endif

   clrscr();
   textcolor(DARKGREY);

put_screen : bufcount2 = 0;
#ifdef LINUX
gotoxy(1, 1);
#endif
for (j = 0; j < 25; j ++)
{
 for (i = 0; i < 80; i ++)
 {
  if (screen_y + j - 12 >= 65 | screen_y + j - 12 <= 4)
  {
   buffer2 [bufcount2 + 1] = DARKGREY;
   buffer2 [bufcount2] = 0;
   bufcount2 += 2;
#ifdef LINUX
goto print_it;
#endif

#ifdef DOS
   continue;
#endif
  }
//continue;
  buffer2 [bufcount2 + 1] = DARKGREY;
  if (grd [i + 1] [j + screen_y - 11] == 75)
     {
      buffer2 [bufcount2 + 1] = LIGHTCYAN;
     }
  if (grd [i + 1] [j + screen_y - 11] == 76)
     {
      buffer2 [bufcount2 + 1] = MAGENTA;
     }
  if (grd [i + 1] [j + screen_y - 11] >= 82 && grd [i + 1] [j + screen_y - 11] <= 85)
     {
      buffer2 [bufcount2 + 1] = RED;
     }
  if (grd [i + 1] [j + screen_y - 11] >= 86 && grd [i + 1] [j + screen_y - 11] <= 89)
     {
      buffer2 [bufcount2 + 1] = BLUE;
     }
  if (grd [i + 1] [j + screen_y - 11] >= 110 && grd [i + 1] [j + screen_y - 11] < 130)
     {
      buffer2 [bufcount2 + 1] = LIGHTRED;
     }
  if (grd [i + 1] [j + screen_y - 11] >= 130 && grd [i + 1] [j + screen_y - 11] < 150)
     {
      buffer2 [bufcount2 + 1] = LIGHTBLUE;
     }
  if (i == you[0].x_pos - 1 && j + screen_y - 11 == you[0].y_pos) buffer2 [bufcount2 + 1] = WHITE;
  buffer2 [bufcount2] = (env[0].map [i] [j + screen_y - 12]);
  bufcount2 += 2;
#ifdef LINUX
print_it: if (j == 24 && i == 79) continue;
textcolor(buffer2 [bufcount2 - 1]);
putch(buffer2 [bufcount2 - 2]);
#endif
//gotoxy(i, j);
//  putch(map [i] [j + you[0].y_pos - 12]);
 }
}
#ifdef DOS
   puttext(1, 1, 80, 25, buffer2);
#endif
   gotoxy(curs_x, curs_y);
   gettything : getty = getch();
   if (spec_place [0] == 0 && getty != 0 && getty != '+' && getty != '-' && getty != 'h' && getty != 'j' && getty != 'k' && getty != 'l' && getty != 'y' && getty != 'u' && getty != 'b' && getty != 'n') goto putty;
   if (spec_place [0] == 1 && getty != 0 && getty != '+' && getty != '-' && getty != 'h' && getty != 'j' && getty != 'k' && getty != 'l' && getty != 'y' && getty != 'u' && getty != 'b' && getty != 'n' && getty != '.' && getty != 'S') goto gettything;
   if (getty == 0) getty = getch();
        switch(getty)
        {
                case 'b': move_x = -1; move_y = 1; break;
                case 'j': move_y = 1; move_x = 0; break;
                case 'u': move_x = 1; move_y = -1; break;
                case 'k': move_y = -1; move_x = 0; break;
                case 'y': move_y = -1; move_x = -1; break;
                case 'h': move_x = -1; move_y = 0; break;
                case 'n': move_y = 1; move_x = 1; break;
                case 'l': move_x = 1; move_y = 0; break;

                case 'H': move_y = -1; move_x = 0; break;
                case 'P': move_y = 1; move_x = 0; break;
                case 'K': move_x = -1; move_y = 0; break;
                case 'M': move_x = 1; move_y = 0; break;

                case 'O': move_x = -1; move_y = 1; break;
                case 'I': move_x = 1; move_y = -1; break;
                case 'G': move_y = -1; move_x = -1; break;
                case 'Q': move_y = 1; move_x = 1; break;
  case '+': move_y = 20; move_x = 0; break;
  case '-': move_y = -20; move_x = 0; break;
  case '.':
  case 'S':
  spec_place [0] = curs_x;
  spec_place [1] = screen_y + curs_y - 12;
  goto putty;
  break;
  default: move_x = 0; move_y = 0; break;
        }

 if (curs_x + move_x < 1 | curs_x + move_x > 75)
    move_x = 0;

    curs_x += move_x;

    if (getty == '-' | getty == '+')
    {
     if (getty == '-') screen_y -= 20;
     if (screen_y <= 2 + 11) screen_y = 2 + 11;
     if (getty == '+') screen_y += 20;
     if (screen_y >= 68 - 11) screen_y = 68 - 11;
     goto put_screen;
    }

    if (curs_y + move_y < 1)
    {
     if (screen_y > 2 + 11)
     {
      screen_y --;
      goto put_screen;
     } else move_y = 0;
    }
    if (curs_y + move_y > 24)
    {
     if (screen_y < 68 - 11)
     {
      screen_y ++;
      goto put_screen;
     } else move_y = 0;
    }
    curs_y += move_y;
    goto put_screen;

   putty :
#ifdef DOS
   puttext(1, 1, 80, 25, buffer);
#endif

return;


}


void magic_mapping(int map_radius, int proportion)
{

int i, j, k, l, empty_count;

if (map_radius >= 50) map_radius = 50;

for (i = you[0].x_pos - map_radius; i < you[0].x_pos + map_radius; i ++)
{
 for (j = you[0].y_pos - map_radius; j < you[0].y_pos + map_radius; j ++)
 {
  if (random2(100) > proportion) continue; // note that proportion can be over 100
  if (i < 5 | j < 5 | i > 75 | j > 65) continue;
  if (env[0].map [i] [j] == mapch2(grd [i + 1] [j + 1])) continue;
  empty_count = 8;
  if (grd [i] [j] <= 60 && grd [i] [j] != 3)
   for (k = 0; k < 3; k ++)
   {
    for (l = 0; l < 3; l ++)
                        {
     if (k == 1 && l == 1) continue;
     if (grd [i + k] [j + l] <= 60 && grd [i + k] [j + l] != 3) empty_count --;
                        }
   }
 if (empty_count > 0) env[0].map [i] [j] = mapch(grd [i + 1] [j + 1]);
 }
}


}

/* mapchars 3 & 4 are for non-ibm char sets */

unsigned char mapchar(unsigned char ldfk)
{
unsigned char showed = 0;

        switch (ldfk)
        {
                case 0: showed = 0; break;
                case 5: // secret door
                case 1: showed = 176; break;  // rock wall
                case 2: showed = 176; break; // stone wall
                case 3: showed = 206; break; // door
  case 4:
  case 6:
  case 8: showed = 176; break;
  case 11: showed = 247; break;
  case 12: showed = 247; break;
  case 20: showed = '1'; break; // orcish idol
                case 61:
                case 62: showed = 247; break;
                case 67:        showed = 250; break;
                //case 68: showed = '>'; break; // < (60)
                case 69: showed = 239; break; // >
                case 70: showed = 39; break; // open door
                //case 72: showed = '<'; break;
                case 75:        showed = 94; break; // ^ dart trap
                case 76:        showed = 94; break; // ^ magic trap
                case 77:        showed = 94; break;
                case 78: showed = 250; break; // undiscovered trap
                case 81:
                case 80: showed = 239; break; // shop
  case 82:
  case 83:
  case 84:
  case 85:
  case 110:
  case 111:
  case 112:
  case 113:
  case 114:
  case 115:
  case 116:
  case 117:
  case 118:
  case 119:
       showed = '>'; break;

  case 86:
  case 87:
  case 88:
  case 89:
  case 130:
  case 131:
  case 132:
  case 133:
  case 134:
  case 135:
  case 136:
  case 137:
  case 138:
  case 139:
       showed = '<'; break;

  case 92:
  case 93:
  case 94:
  case 95:
  case 96:
  case 97:
  case 98:
  case 99:
  case 100:
  case 101:
       showed = 239; break;

                default: showed = 0; break;
}

return showed;

}

unsigned char mapchar2(unsigned char ldfk)
{
unsigned char showed = 0;

        switch (ldfk)
        {
                case 0: showed = 0; break;
                case 5: // secret door
                case 1: showed = 177; break;  // rock wall
                case 2: showed = 177; break; // stone wall
                case 3: showed = 254; break; // door
  case 4:
  case 6:
  case 8: showed = 177; break;
  case 11: showed = 247; break;
  case 12: showed = 247; break;
  case 20: showed = '1'; break; // orcish idol
                case 61:
                case 62: showed = 247; break;
                case 67:        showed = 249; break;
                case 68: showed = '>'; break; // <
                case 69: showed = 239; break; // >
                case 70: showed = 39; break; // open door
                case 72: showed = '<'; break; // <
                case 75:        showed = 94; break; // ^ dart trap
                case 76:        showed = 94; break; // ^ magic trap
                case 77:        showed = 94; break;
                case 78: showed = 249; break; // undiscovered trap
                case 81:
                case 80: showed = 239; break; // shop
  case 82:
  case 83:
  case 84:
  case 85:
  case 110:
  case 111:
  case 112:
  case 113:
  case 114:
  case 115:
  case 116:
  case 117:
  case 118:
  case 119:
       showed = '>'; break;

  case 86:
  case 87:
  case 88:
  case 89:
  case 130:
  case 131:
  case 132:
  case 133:
  case 134:
  case 135:
  case 136:
  case 137:
  case 138:
  case 139:
       showed = '<'; break;

  case 92:
  case 93:
  case 94:
  case 95:
  case 96:
  case 97:
  case 98:
  case 99:
  case 100:
  case 101:
       showed = 239; break;

             default: showed = 0; break;
}

return showed;

}


char mons_near(unsigned char monst)
{

 if (menv [monst].m_x > you[0].x_pos - 9 && menv [monst].m_x < you[0].x_pos + 9 && menv [monst].m_y > you[0].y_pos - 9 && menv [monst].m_y < you[0].y_pos + 9)
 {
        if (env[0].show [menv [monst].m_x - you[0].x_pos + 9] [menv [monst].m_y - you[0].y_pos + 9] != 0)
                return 1;
 }

 return 0;

} // end of char mons_near(char)





/*

This is the viewwindow function for computers without IBM graphic displays.
It is activated by a command line argument, which sets a function pointer.

*/


void viewwindow3(char draw_it)
{

   int bufcount = 0;
   unsigned char buffy [1500]; //[800]; //392];

   unsigned char showed = 0;

   int count_x, count_y;

        _setcursortype(_NOCURSOR);

        losight(env[0].show, env[0].grid, you[0].x_pos, you[0].y_pos);


        for (count_x = 0; count_x < 18; count_x++)
        {
                for (count_y = 0; count_y < 18; count_y++)
                {
                        env[0].show_col [count_x] [count_y] = 7;
                }
        }

        item();
        cloud_grid();
        monster_grid();
        bufcount = 0;


        if (draw_it == 1)
        {

        for (count_y = (you[0].y_pos - 8); (count_y < you[0].y_pos + 9); count_y++)
                {
                 bufcount += 16;
                 for (count_x = (you[0].x_pos - 8); (count_x < you[0].x_pos + 9); count_x++)
                 {
                  buffy [bufcount + 1] = env[0].show_col [count_x - you[0].x_pos + 9] [count_y - you[0].y_pos + 9];


if (count_x != you[0].x_pos | count_y != you[0].y_pos)
{



switch (env[0].show [count_x - you[0].x_pos + 9] [count_y - you[0].y_pos + 9])
        {

                case 0: showed = 0; break;
                case 1: buffy [bufcount + 1] = env[0].rock_colour;
                showed = '#'; break; // rock wall - remember earth elementals
                case 2: buffy [bufcount + 1] = LIGHTGRAY;
                showed = '#'; break; // stone wall
                case 3: showed = '+'; break;
  case 4: showed = '#';
  buffy [bufcount + 1] = CYAN; break;
  case 5: showed = '#'; // hidden secret door
  buffy [bufcount + 1] = env[0].rock_colour; break;
  case 6: showed = '#';
  buffy [bufcount + 1] = GREEN; break; // green crystal wall
  case 7: showed = '1';
  buffy [bufcount + 1] = DARKGRAY; // orcish idol
  break;
  case 8: showed = '#';
  buffy [bufcount + 1] = YELLOW; break; // wax wall
  case 21: showed = '1';
  buffy [bufcount + 1] = WHITE; // silver statue
  break;
  case 22: showed = '#'; // filled square
  buffy [bufcount + 1] = LIGHTGRAY; // granite monolith
  break;
  case 35: showed = '#'; break;
  case 61: showed = '{';
  buffy [bufcount + 1] = RED; // lava!
  break;
  case 62: showed = '{'; // this wavy thing also used for water elemental
   // note that some monsters which use IBM graphics aren't set for this function - too tricky for now.
  buffy [bufcount + 1] = BLUE; // water
  break;

                case 67: buffy [bufcount + 1] = env[0].floor_colour; //LIGHTGRAY;
                showed = '.'; break;
                case 69: showed = '\\'; buffy [bufcount + 1] = RED; break; // staircase to hell
                case 70: showed = 39; break; // open door
  case 71: showed = '>'; buffy [bufcount + 1] = BROWN; break; // branch staircase
                case 75: buffy [bufcount + 1] = 11;
                showed = 94; break; // ^ dart trap
                case 76: buffy [bufcount + 1] = MAGENTA;
                showed = 94; break;
                case 77: buffy [bufcount + 1] = LIGHTGRAY;
                showed = 94; break;
                case 78: showed = '.'; buffy [bufcount + 1] = env[0].floor_colour;
                break; // undiscovered trap

  case 80: showed = '\\'; buffy [bufcount + 1] = YELLOW;
       break;
// if I change anything above here, must also change magic mapping!
  case 81: showed = '\\'; buffy [bufcount + 1] = LIGHTGRAY; break; // labyrinth
  case 85: buffy [bufcount + 1] = BROWN; // ladder
  case 82:
  case 83:
  case 84:
       showed = '>'; break;
  case 89: buffy [bufcount + 1] = BROWN; // ladder
  case 86:
  case 87:
  case 88:
       showed = '<'; break;

  case 92: buffy [bufcount + 1] = CYAN; showed = '\\'; break; // Stairway to Dis
  case 93: buffy [bufcount + 1] = RED; showed = '\\'; break; // Gehenna
  case 94: buffy [bufcount + 1] = LIGHTCYAN; showed = '\\'; break; // Cocytus
  case 95: buffy [bufcount + 1] = DARKGRAY; showed = '\\'; break; // Tartarus
  case 96: buffy [bufcount + 1] = LIGHTMAGENTA; showed = '\\'; break; // To Abyss
  case 97: buffy [bufcount + 1] = LIGHTMAGENTA; showed = '\\'; break; // From Abyss
  case 98: buffy [bufcount + 1] = LIGHTGRAY; showed = '\\'; break; // Closed gate to hell
  case 99: buffy [bufcount + 1] = LIGHTBLUE; showed = '\\'; break; // gate to pandemonium
  case 100: buffy [bufcount + 1] = LIGHTBLUE; showed = '\\'; break; // gate out of pandemonium
  case 101: buffy [bufcount + 1] = LIGHTGREEN; showed = '\\'; break; // gate to other part of pandemonium

  case 110:
  case 111:
  case 112:
  case 113:
  case 114:
  case 115:
  case 116: buffy [bufcount + 1] = YELLOW; showed = '>'; break; // stair to orc mine

  case 130:
  case 131:
  case 132:
  case 133:
  case 134:
  case 135:
  case 136: buffy [bufcount + 1] = YELLOW; showed = '>'; break; // stair to orc mine

                case 256: showed = 9; break;

                case 258: showed = 41; break; // weapon )
                case 259: showed = 91; break; // armour [
                case 260: showed = 47; break; // food %
                case 261: showed = 37; break; // wands / &c
                case 262: showed = 43; break; // books +
                case 263: showed = 63; break; // scroll ?
                case 264: showed = 61; break; // ring = etc
                case 265: showed = 33; break; // potions !
                case 266: showed = 40; break; // stones
                case 267: showed = ':'; break; // book +
                case 268: showed = 37; break; // corpses part 1
  case 269: showed = '|'; break; // magical staves
  case 270: showed = '}'; break; // gems
  case 271: showed = '%'; break; // don't know ?
                case 272: showed = 36; break; // $ gold

default:
        int mnr = env[0].show [count_x - you[0].x_pos + 9] [count_y - you[0].y_pos + 9];
        if (mnr >= 297) {
                showed = mons_char(mnr - 297); // yeah
        } else
                showed = env[0].show [count_x - you[0].x_pos + 9] [count_y - you[0].y_pos + 9]; break;

                }

} else
      {
/*              showed = your_sign;
              buffy [bufcount + 1] = your_colour;*/
                showed = '@';
                buffy [bufcount + 1] = LIGHTGRAY;
      }


buffy [bufcount] = showed;
// have the colour defined before the switch, because some cases change it.

bufcount += 2;
                }
  bufcount += 16;
        }
  bufcount = 0;

if (you[0].level_type != 1 && you[0].level_type != 2)
{
        for (count_y = 0; count_y < 17; count_y++)
        {
  bufcount += 16;
                for (count_x = 0; count_x < 17; count_x++)
                {
   if (buffy [bufcount] != 0) env[0].map [count_x + you[0].x_pos - 9] [count_y + you[0].y_pos - 9] = buffy [bufcount];
   bufcount += 2;
                }
  bufcount += 16;
        }
}


bufcount = 0;
  for (count_y = 0; count_y < 17; count_y ++)
                {
   for (count_x = 0; count_x < 33; count_x ++)
                        {
       if (count_x + you[0].x_pos - 17 < 3 | count_y + you[0].y_pos - 9 < 3 | count_x + you[0].x_pos - 14 > 77 | count_y + you[0].y_pos - 9 > 67)
       {
            buffy [bufcount] = 0;
            bufcount ++;
            buffy [bufcount] = 0;
            bufcount ++;
            continue;
       }
       if (count_x >= 8 && count_x <= 24 && count_y >= 0 && count_y <= 16 && buffy [bufcount] != 0)
       {
            bufcount += 2;
            continue;
       }
         buffy [bufcount] = env[0].map [count_x + you[0].x_pos - 17] [count_y + you[0].y_pos - 9];
         buffy [bufcount + 1] = DARKGRAY;
                                                                        bufcount += 2;
                        }
                }

if (you[0].berserker != 0)
{
 for (count_x = 1; count_x < 1400; count_x += 2)
 {
  if (buffy [count_x] != DARKGREY) buffy [count_x] = RED;
 }
}

if (show_green != 0)
{
 for (count_x = 1; count_x < 1400; count_x += 2)
 {
  if (buffy [count_x] != DARKGREY) buffy [count_x] = show_green;
 }
 show_green = 0;
 if (you[0].special_wield == 50) show_green = DARKGREY;
}



#ifdef DOS
puttext(2,1,34,17, buffy);
#endif

#ifdef LINUX
gotoxy(2,1);
bufcount = 0;

if (you[0].running == 0) // this line is purely optional
{
 for (count_x = 0; count_x < 1120; count_x += 2) // 1056
 {
  textcolor(buffy [count_x + 1]);
  putch(buffy [count_x]);
  if (count_x % 66 == 64 && count_x > 0)
#ifdef DOS
    cprintf("\n\r ");
#endif
#ifdef LINUX
    gotoxy(2, wherey() + 1);
#endif

 }
}
#endif

        } // end of (if brek...)

}



unsigned char mapchar3(unsigned char ldfk)
{
unsigned char showed = 0;

        switch (ldfk)
        {
                case 0: showed = 0; break;
                case 5: // secret door
                case 1: showed = '*'; break;  // rock wall
                case 2: showed = '*'; break; // stone wall
                case 3: showed = '+'; break; // door
  case 4:
  case 6:
  case 8: showed = '*'; break;
  case 11: showed = '{'; break;
  case 12: showed = '{'; break;
  case 20: showed = '1'; break; // orcish idol
                case 61:
                case 62: showed = '{'; break;
                case 67:        showed = ','; break;
                //case 68: showed = '>'; break; // < (60)
                case 69: showed = '\\'; break; // >
                case 70: showed = 39; break; // open door
                //case 72: showed = '<'; break;
                case 75:        showed = 94; break; // ^ dart trap
                case 76:        showed = 94; break; // ^ magic trap
                case 77:        showed = 94; break;
                case 78: showed = ','; break; // undiscovered trap
                case 81:
                case 80: showed = '\\'; break; // shop
  case 82:
  case 83:
  case 84:
  case 85:
  case 110:
  case 111:
  case 112:
  case 113:
  case 114:
  case 115:
  case 116:
  case 117:
  case 118:
  case 119:
       showed = '>'; break;

  case 86:
  case 87:
  case 88:
  case 89:
  case 130:
  case 131:
  case 132:
  case 133:
  case 134:
  case 135:
  case 136:
  case 137:
  case 138:
  case 139:
       showed = '<'; break;

  case 92:
  case 93:
  case 94:
  case 95:
  case 96:
  case 97:
  case 98:
  case 99:
  case 100:
  case 101:
       showed = '\\'; break;

                default: showed = 0; break;
}

return showed;

}

unsigned char mapchar4(unsigned char ldfk)
{
unsigned char showed = 0;

        switch (ldfk)
        {
                case 0: showed = 0; break;
                case 5: // secret door
                case 1: showed = '#'; break;  // rock wall
                case 2: showed = '#'; break; // stone wall
                case 3: showed = '+'; break; // door
  case 4:
  case 6:
  case 8: showed = '#'; break;
  case 11: showed = '{'; break;
  case 12: showed = '{'; break;
  case 20: showed = '1'; break; // orcish idol
                case 61:
                case 62: showed = '{'; break;
                case 67:        showed = '.'; break;
                case 68: showed = '>'; break; // <
                case 69: showed = '\\'; break; // >
                case 70: showed = 39; break; // open door
                case 72: showed = '<'; break; // <
                case 75:        showed = 94; break; // ^ dart trap
                case 76:        showed = 94; break; // ^ magic trap
                case 77:        showed = 94; break;
                case 78: showed = '.'; break; // undiscovered trap
                case 81:
                case 80: showed = '\\'; break; // shop
  case 82:
  case 83:
  case 84:
  case 85:
  case 110:
  case 111:
  case 112:
  case 113:
  case 114:
  case 115:
  case 116:
  case 117:
  case 118:
  case 119:
       showed = '>'; break;

  case 86:
  case 87:
  case 88:
  case 89:
  case 130:
  case 131:
  case 132:
  case 133:
  case 134:
  case 135:
  case 136:
  case 137:
  case 138:
  case 139:
       showed = '<'; break;

  case 92:
  case 93:
  case 94:
  case 95:
  case 96:
  case 97:
  case 98:
  case 99:
  case 100:
  case 101:
       showed = '\\'; break;

             default: showed = 0; break;
}

return showed;

}

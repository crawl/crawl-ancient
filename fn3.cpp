#include <iostream.h>
#include <conio.h>
#include <stdio.h>
#include <string.h>
#include <dos.h>
#include <fstream.h>
#include <fcntl.h>
#include <io.h>
#include <SYS\STAT.H>



unsigned char mapchar(unsigned char ldfk);
unsigned char mapchar2(unsigned char ldfk);

void more(void)
{
char keypress = 0;

window(2, 18, 78, 25);
gotoxy(1,8);
_setcursortype(_NORMALCURSOR);
textcolor(7);
cprintf("\r--more--");
//cout << "\r--more--";
do
{
        keypress = getch();
} while (keypress != 32 && keypress != 13);

//return keypress;
keypress = 0;
clrscr();
}



void show_map(unsigned char map [80] [70], int x_pos, int y_pos)
{
   int curs_x = x_pos;
                        int curs_y = 12; //y_pos;
   int screen_y = y_pos;

   int i;
   char buffer[4800];
   char buffer2[4800];
   int bufcount2 = 0;
   int j;

   char move_x = 0;
   char move_y = 0;
   char getty = 0;

   gettext(1, 1, 80, 25, buffer);

   window(1, 1, 80, 25);

   clrscr();
   textcolor(DARKGRAY);

put_screen : bufcount2 = 0;
for (j = 0; j < 25; j ++)
{
 for (i = 0; i < 80; i ++)
 {
//  if (i + x_pos - 40 <= 5 | i + x_pos - 40 >= 75 | j + y_pos - 12 <= 5 | j + y_pos - 12 >= 65)
//     continue;
  if (screen_y + j - 12 >= 65 | screen_y + j - 12 <= 4)
  {
   buffer2 [bufcount2 + 1] = DARKGRAY;
   buffer2 [bufcount2] = 0;
   bufcount2 += 2;
   continue;
  }
//continue;
  buffer2 [bufcount2 + 1] = DARKGRAY;
                if (i == x_pos - 1 && j + screen_y - 11 == y_pos) buffer2 [bufcount2 + 1] = WHITE;
  buffer2 [bufcount2] = (map [i] [j + screen_y - 12]);
  bufcount2 += 2;
//gotoxy(i, j);
//  putch(map [i] [j + y_pos - 12]);
 }
}

   puttext(1, 1, 80, 25, buffer2);
   gotoxy(curs_x, curs_y);
   getty = getch();
   if (getty != 0 && getty != '+' && getty != '-' && getty != 'h' && getty != 'j' && getty != 'k' && getty != 'l' && getty != 'y' && getty != 'u' && getty != 'b' && getty != 'n') goto putty;
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
  default: move_x = 0; move_y = 0; break;
        }

 if (curs_x + move_x < 1 | curs_x + move_x > 75)// | curs_y + move_y < 1 | curs_y + move_y > 65)
    move_x = 0;

    //goto put_screen;

    curs_x += move_x;
/*    if (curs_y + move_y < 7)
                                {
     if (screen_y <= 7) screen_y = 1; else      screen_y -= 7;
                                        curs_y += 7;
     goto put_screen;
                                }
    if (curs_y + move_y > 18)
                                {
        if (screen_y >= 63) screen_y = 68; else screen_y += 7;
                                        curs_y -= 7;
     goto put_screen;
                                }
*/
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

//   if (getch() == 0) getch();

   putty : puttext(1, 1, 80, 25, buffer);


}


void magic_mapping(unsigned char grid [80] [70], unsigned char map [80] [70], int x_pos, int y_pos, int map_radius)
{

int i, j, k, l, empty_count;

if (map_radius >= 50) map_radius = 50;

for (i = x_pos - map_radius; i < x_pos + map_radius; i ++)
{
 for (j = y_pos - map_radius; j < y_pos + map_radius; j ++)
 {
  if (i < 5 | j < 5 | i > 75 | j > 65) continue;
  if (map [i] [j] == mapchar2(grid [i + 1] [j + 1])) continue;
  empty_count = 8;
  if (grid [i] [j] <= 10 && grid [i] [j] != 3)
   for (k = 0; k < 3; k ++)
   {
    for (l = 0; l < 3; l ++)
                        {
     if (k == 1 && l == 1) continue;
     if (grid [i + k] [j + l] <= 10 && grid [i + k] [j + l] != 3) empty_count --;
                        }
   }
 if (empty_count > 0) map [i] [j] = mapchar(grid [i + 1] [j + 1]);
 }
}


}

unsigned char mapchar(unsigned char ldfk)
{
unsigned char showed = 0;

        switch (ldfk % 50)
        {
                case 0: showed = 0; break;
                case 1: showed = 176; break;  // rock wall
                case 2: showed = 176; break; // stone wall
                case 3: showed = 206; break; // door
  case 4: showed = 176; break;
  case 11: showed = 247; break;
  case 12: showed = 247; break;
                case 17:        showed = 250; break;
                case 18: showed = 240; break; // < (60)
                case 19: showed = 240; break; // >
                case 20: showed = 39; break; // open door
                case 25:        showed = 94; break; // ^ dart trap
                case 26:        showed = 94; break; // ^ magic trap
                case 27:        showed = 94; break;
                case 28: showed = 250; break; // undiscovered trap
  default: showed = 0; break;
}

return showed;

}

unsigned char mapchar2(unsigned char ldfk)
{
unsigned char showed = 0;

        switch (ldfk % 50)
        {
                case 0: showed = 0; break;
                case 1: showed = 177; break;  // rock wall
                case 2: showed = 177; break; // stone wall
                case 3: showed = 254; break; // door
  case 4: showed = 177; break;
  case 11: showed = 247; break;
  case 12: showed = 247; break;
                case 17:        showed = 249; break;
                case 18: showed = 240; break; // <
                case 19: showed = 240; break; // >
                case 20: showed = 39; break; // open door
                case 25:        showed = 94; break; // ^ dart trap
                case 26:        showed = 94; break; // ^ magic trap
                case 27:        showed = 94; break;
                case 28: showed = 249; break; // undiscovered trap
  default: showed = 0; break;
}

return showed;

}



void highscore(char death_string [80], long points)
{

unsigned char high_scores [20] [80];
long scores [20];
int hc = 0;
int hc2 = 0;
int hc3 = 0;
char ready [2];
int your_palce = 0;
int multip = 1;

ready [0] = 0;
ready [1] = 0;

for (hc2 = strlen(death_string); hc2 < 80; hc2 ++)
{
 death_string [hc2] = ' ';
}

for (hc = 0; hc < 20; hc ++)
{
 strcpy(high_scores [hc], "-empty");
 scores [hc] = 0;
}

int handle = open("scores", S_IWRITE, S_IREAD);

if (handle == -1)
{
        perror("OH NO!!!! ");
        abort();
}


for (hc = 0; hc < 15; hc ++)
{
 for (hc2 = 0; hc2 < 80; hc2 ++)
 {
  read(handle, ready, 1);
//  cprintf(ready);
//  getch();
  if (ready [0] == 13 | ready [0] == 0 | ready [0] == '\r' | ready [0] == '\n')
//  if (ready [0] == '\n')
  {
   for (hc3 = hc2; hc3 < 80; hc3 ++)
   {
    high_scores [hc] [hc3] = 32;
    goto out_of_inner;
   }
  }
  if (ready [0] == EOF) goto out_of_ready;
  if (ready [0] == 0 | ready [0] == 7) ready [0] = 32;
  high_scores [hc] [hc2] = ready [0];
 } // end for hc2
out_of_inner : hc3 = 0;
} // end for hc

/*for (hc = 7; hc < 15; hc ++)
{
 strcpy(high_scores [hc], "0       -empty                                                                  ");
}*/

for (hc = 0; hc < 15; hc ++)
{
 multip = 1;
 for (hc2 = 5; hc2 >= 0; hc2 --)
 {
  if (high_scores [hc] [hc2] == 32) continue;
  scores [hc] += (high_scores [hc] [hc2] - 48) * multip;
  multip *= 10;
 }
}

out_of_ready : //for (hc = 0; hc < 7; hc ++)
{
// if (high_scores [0] [hc] == 32)
// {
//  hc = 5; //random3(10);

//strcpy(high_scores [hc], death_string);

//  break;
// }
}
/*
for (hc = 0; hc < 15; hc ++)
{
 for (hc2 = 0; hc2 < 80; hc2 ++)
 {
  if (high_scores [hc] [hc2] != '-') high_scores [hc] [hc2] = 'z';
 }
}*/

int placed = 0;
char has_printed = 0;

for (hc = 0; hc < 15; hc ++)
{
// if (high_scores [0] [hc] == 32)
// {
 placed ++;
if (points > scores [hc] && has_printed == 0)
{
 textcolor(YELLOW);
  itoa(placed, ready, 10);
  cprintf(ready);
  if (strlen(ready) == 1) cprintf("- "); else cprintf("-");
 for (hc2 = 0; hc2 < 80; hc2 ++)
 {
  if (death_string [hc2] == 32 && death_string [hc2 + 1] == 32 && hc2 > 15)
  {
   cprintf("\n\r");
   break;
  }
  putch(death_string [hc2]); //high_scores [hc] [hc2]);
 }
 has_printed = 1;
 hc --;
 textcolor(LIGHTGRAY);
 continue;
}
  itoa(placed, ready, 10);
  cprintf(ready);
  if (strlen(ready) == 1) cprintf("- "); else cprintf("-");
 for (hc2 = 0; hc2 < 80; hc2 ++)
 {
  if (high_scores [hc] [hc2] == 32 && high_scores [hc] [hc2 + 1] == 32 && hc2 > 15)
  {
   cprintf("\n\r");
   break;
  }
  putch(high_scores [hc] [hc2]);
 }
// cprintf("\n\r");
//  cprintf(high_scores [hc]);
//  cprintf("\n\r");

// }
}

close(handle);

//getch();

//handle = open("scores", O_CREAT | O_TRUNC | O_BINARY, S_IWRITE | S_IREAD);
handle = open("scores", O_WRONLY | O_BINARY, S_IWRITE, S_IREAD);

if (handle == -1)
{
        perror("oh NO!!!");
        return;
}

for (hc = 0; hc < 15; hc ++)
{
 for (hc2 = strlen(high_scores [hc]); hc2 < 80; hc2 ++)
 {
  high_scores [hc] [hc2] = ' ';
 }
}
has_printed = 0;
for (hc = 0; hc < 15; hc ++)
{
 if (has_printed == 1 && hc == 14) break;
 if (points > scores [hc] && has_printed == 0)
 {
  write(handle, death_string, 80);
  hc --;
  has_printed = 1;
 } else write(handle, high_scores [hc], 80);
}

close(handle);

} // end void highscores

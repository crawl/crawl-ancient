/* Some replacement routines missing in gcc
   Some of these are inspired by/stolen from the Linux-conio package
   by Mental EXPlotion. Hope you guys don't mind.
   The colour exchange system is perhaps a little overkill, but I wanted
   it to be general to support future changes.. The only thing not
   supported properly is black on black (used by curses for "normal" mode)
   and white on white (used by me for "bright black" (darkgray) on black

   Jan 1998 Svante Gerhard <svante@algonet.se>                          */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <curses.h>
#include <stdarg.h>
#define _IMPLEMENTATION
#include "linuxlib.h"

// Globals holding current text/backg. colors
short FG_COL = COLOR_WHITE;
short BG_COL = COLOR_BLACK;


// Translate DOS colors to curses. 128 just means use high intens./bold.
short translatecolor(short col)
{
        switch (col)
        {
               case BLACK        : return COLOR_BLACK;         break;
               case BLUE         : return COLOR_BLUE;          break;
               case GREEN        : return COLOR_GREEN;         break;
               case CYAN         : return COLOR_CYAN;          break;
               case RED          : return COLOR_RED;           break;
               case MAGENTA      : return COLOR_MAGENTA;       break;
               case BROWN        : return COLOR_YELLOW;        break;
               case LIGHTGRAY    : return COLOR_WHITE;         break;
               case DARKGRAY     : return COLOR_BLACK   + 128; break;
               case LIGHTBLUE    : return COLOR_BLUE    + 128; break;
               case LIGHTGREEN   : return COLOR_GREEN   + 128; break;
               case LIGHTCYAN    : return COLOR_CYAN    + 128; break;
               case LIGHTRED     : return COLOR_RED     + 128; break;
               case LIGHTMAGENTA : return COLOR_MAGENTA + 128; break;
               case YELLOW       : return COLOR_YELLOW  + 128; break;
               case WHITE        : return COLOR_WHITE   + 128; break;
               default: return COLOR_GREEN; break; //mainly for debugging
        }
}



void setupcolorpairs()
{
        short i, j;

        for (i = 0; i < 8; i++)
                for (j = 0; j < 8; j++)
                        if ((i > 0) || (j > 0))
                                init_pair(i * 8 + j, j, i);

        init_pair(63, COLOR_BLACK, COLOR_BLACK);
}


void lincurses_startup()
{
        initscr();
        savetty();
        noecho();
        keypad(stdscr, TRUE);
        cbreak();
        meta(stdscr, TRUE);
        start_color();
        setupcolorpairs();
}


void lincurses_shutdown()
{
        resetty();
        endwin();
}


/* Convert value to string */
int itoa(int value, char *strptr, int radix)
{
        unsigned int bitmask = 32768;
        int ctr = 0;
        int startflag = 0;

        if (radix == 10)
        {
                sprintf(strptr, "%i", value);
        }
        if (radix == 2)       /* int to "binary string" */
        {
                while (bitmask)
                {
                        if (value & bitmask)
                        {
                                startflag = 1;
                                sprintf(strptr + ctr, "1");
                        }
                        else
                        {
                                if (startflag)
                                sprintf(strptr + ctr, "0");
                        }
                        bitmask = bitmask >> 1;
                        if (startflag)
                                ctr++;
                }
                if (!startflag) /* Special case if value == 0 */
                        sprintf((strptr + ctr++), "0");
                strptr[ctr] = (char) NULL;
        }
        return(OK);                     /* Me? Fail? Nah. */
}


int cprintf (char *format, ... )
{
   int i;
   char buffer[2048]; // One full screen if no control seq...
   va_list argp;
   va_start(argp,format);
   vsprintf(buffer,format,argp);
   va_end(argp);
   i=addstr(buffer);
   refresh();
   return(i);
}


int putch(unsigned char chr)
{
        if (chr == 0)
                chr = ' ';
        return(addch(chr));
}


char getche()
{
        char chr;

        chr = getch();
        addch(chr);
        refresh();
        return(chr);
}


int window(int x1, int y1, int x2, int y2)
{
        x1 = y1 = x2 = y2 = 0; /* Do something to them.. makes gcc happy :) */

        return(refresh());
}


int clrscr()
{
        int retval;

        retval = clear();
        refresh();
        return(retval);
}


void _setcursortype(int curstype)
{
        curs_set(curstype);
}


void textcolor(int col)
{
        short fg, bg;

        FG_COL = col;
        fg = translatecolor(FG_COL);
        bg = translatecolor(BG_COL);
        if (bg & 128)
               bg = bg - 128;
        if ((fg == 128) && (bg == 0)) // Special case: 0/0/bold = darkgray/black!
                attrset(COLOR_PAIR(63) | A_BOLD | A_ALTCHARSET);
        else
        {
                if (fg & 128)
                {
                        attrset((COLOR_PAIR((fg - 128) + bg * 8)) | A_BOLD | A_ALTCHARSET);
                }
                else
                        attrset(COLOR_PAIR(fg + bg * 8) | A_NORMAL | A_ALTCHARSET);
        }
}


void textbackground(int col)
{
        short fg, bg;

        BG_COL = col;
        fg = translatecolor(FG_COL);
        bg = translatecolor(BG_COL);
        if (bg & 128)
               bg = bg - 128;
        if ((fg == 128) && (bg == 0)) // Special case: 0/0/bold = darkgray/black!
                attrset(COLOR_PAIR(63) | A_BOLD | A_ALTCHARSET);
        else
        {
                if (fg & 128)
                {
                        attrset((COLOR_PAIR((fg - 128) + bg * 8)) | A_BOLD | A_ALTCHARSET);
                }
                else
                        attrset(COLOR_PAIR(fg + bg * 8) | A_NORMAL | A_ALTCHARSET);
        }
}


int gotoxy(int x, int y)
{
        return(move(y-1, x-1));
}


int wherex()
{
        int x, y;

        getyx(stdscr, y, x);
        return(x+1);
}


int wherey()
{
        int x, y;

        getyx(stdscr, y, x);
        return(y+1);
}


int stricmp(char *str1, char *str2)
{
        return(strcmp(str1, str2));
}


void delay(long time)
{
        usleep(time * 1000);
}


int kbhit()
{
        int i;

        nodelay(stdscr, TRUE);
        i = getch();
        nodelay(stdscr, FALSE);
        if (i == -1)
              i = 0;
        return(i);
}


#ifdef LINUX
// this function is used for systems without gettext/puttext to redraw the
// playing screen after a call to for example inventory.
void redraw_screen(void)
{

char title [40];

strcpy(title, skill_title(best_skill(you, 0, 50, 99), you[0].skills [best_skill(you, 0, 50, 99)], you[0].clas, you[0].xl));
draw_border(BROWN, you[0].your_name, title, you[0].species);

//draw_border(BROWN, you[0].your_name, title);

you[0].hp_ch = 1;
you[0].ep_ch = 1;
you[0].strength_ch = 1;
you[0].intel_ch = 1;
you[0].dex_ch = 1;
you[0].AC_ch = 1;
you[0].evasion_ch = 1;
you[0].gp_ch = 1;
you[0].xp_ch = 1;
you[0].hung_ch = 1;
you[0].burden_ch = 1;

print_stats();

new_level();

viewwindow(1);


}

#endif

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
#include <ncursesw/curses.h>
#include <stdarg.h>
#include <ctype.h>
#include <locale.h>
#define _LINUXLIB_IMPLEMENTATION
#include "linuxlib.h"
#include "defines.h"

// Globals holding current text/backg. colors
short FG_COL = COLOR_WHITE;
short BG_COL = COLOR_BLACK;

static unsigned short charset_cp437[256] =
{
    0x0000, 0x263a, 0x263b, 0x2665, 0x2666, 0x2663, 0x2660, 0x2022,
    0x25d8, 0x25cb, 0x25d9, 0x2642, 0x2640, 0x266a, 0x266b, 0x263c,
    0x25b6, 0x25c0, 0x2195, 0x203c, 0x00b6, 0x00a7, 0x25ac, 0x21a8,
    0x2191, 0x2193, 0x2192, 0x2190, 0x221f, 0x2194, 0x25b2, 0x25bc,
    0x0020, 0x0021, 0x0022, 0x0023, 0x0024, 0x0025, 0x0026, 0x0027,
    0x0028, 0x0029, 0x002a, 0x002b, 0x002c, 0x002d, 0x002e, 0x002f,
    0x0030, 0x0031, 0x0032, 0x0033, 0x0034, 0x0035, 0x0036, 0x0037,
    0x0038, 0x0039, 0x003a, 0x003b, 0x003c, 0x003d, 0x003e, 0x003f,
    0x0040, 0x0041, 0x0042, 0x0043, 0x0044, 0x0045, 0x0046, 0x0047,
    0x0048, 0x0049, 0x004a, 0x004b, 0x004c, 0x004d, 0x004e, 0x004f,
    0x0050, 0x0051, 0x0052, 0x0053, 0x0054, 0x0055, 0x0056, 0x0057,
    0x0058, 0x0059, 0x005a, 0x005b, 0x005c, 0x005d, 0x005e, 0x005f,
    0x0060, 0x0061, 0x0062, 0x0063, 0x0064, 0x0065, 0x0066, 0x0067,
    0x0068, 0x0069, 0x006a, 0x006b, 0x006c, 0x006d, 0x006e, 0x006f,
    0x0070, 0x0071, 0x0072, 0x0073, 0x0074, 0x0075, 0x0076, 0x0077,
    0x0078, 0x0079, 0x007a, 0x007b, 0x007c, 0x007d, 0x007e, 0x2302,
    0x00c7, 0x00fc, 0x00e9, 0x00e2, 0x00e4, 0x00e0, 0x00e5, 0x00e7,
    0x00ea, 0x00eb, 0x00e8, 0x00ef, 0x00ee, 0x00ec, 0x00c4, 0x00c5,
    0x00c9, 0x00e6, 0x00c6, 0x00f4, 0x00f6, 0x00f2, 0x00fb, 0x00f9,
    0x00ff, 0x00d6, 0x00dc, 0x00a2, 0x00a3, 0x00a5, 0x20a7, 0x0192,
    0x00e1, 0x00ed, 0x00f3, 0x00fa, 0x00f1, 0x00d1, 0x00aa, 0x00ba,
    0x00bf, 0x2310, 0x00ac, 0x00bd, 0x00bc, 0x00a1, 0x00ab, 0x00bb,
    0x2591, 0x2592, 0x2593, 0x2502, 0x2524, 0x2561, 0x2562, 0x2556,
    0x2555, 0x2563, 0x2551, 0x2557, 0x255d, 0x255c, 0x255b, 0x2510,
    0x2514, 0x2534, 0x252c, 0x251c, 0x2500, 0x253c, 0x255e, 0x255f,
    0x255a, 0x2554, 0x2569, 0x2566, 0x2560, 0x2550, 0x256c, 0x2567,
    0x2568, 0x2564, 0x2565, 0x2559, 0x2558, 0x2552, 0x2553, 0x256b,
    0x256a, 0x2518, 0x250c, 0x2588, 0x2584, 0x258c, 0x2590, 0x2580,
    0x03b1, 0x00df, 0x0393, 0x03c0, 0x03a3, 0x03c3, 0x00b5, 0x03c4,
    0x03a6, 0x0398, 0x03a9, 0x03b4, 0x221e, 0x03c6, 0x03b5, 0x2229,
    0x2261, 0x00b1, 0x2265, 0x2264, 0x2320, 0x2321, 0x00f7, 0x2248,
    0x00b0, 0x2219, 0x00b7, 0x221a, 0x207f, 0x00b2, 0x25a0, 0x00a0,
};


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
               case LIGHTGREY    : return COLOR_WHITE;         break;
               case DARKGREY     : return COLOR_BLACK   + 128; break;
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
        setlocale(LC_CTYPE, "");
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


// Convert string to lowercase.
char * strlwr(char *str)
{
        unsigned int i;

        for(i = 0; i < strlen(str); i++)
                str[i] = tolower(str[i]);
        return(str);
}


int cprintf (char *format, ... )
{
   int i;
   char buffer[2048]; // One full screen if no control seq...
   va_list argp;
   va_start(argp,format);
   vsprintf(buffer,format,argp);
   va_end(argp);
   wchar_t wbuf[2048];
   char *bp = buffer;
   wchar_t *wp = wbuf;
   while (*wp++ = charset_cp437[*bp++]) ;
   i=addstr(buffer);
   refresh();
   return(i);
}


int putch(unsigned char chr)
{
        if (chr == 0)
                chr = ' ';
        wchar_t c = charset_cp437[chr];
        return(addnwstr(&c, 1));
}


char getche()
{
        char chr;

        chr = getch();
        putch(chr);
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
                attrset(COLOR_PAIR(63) | A_BOLD);
        else
        {
                if (fg & 128)
                {
                        attrset((COLOR_PAIR((fg - 128) + bg * 8)) | A_BOLD);
                }
                else
                        attrset(COLOR_PAIR(fg + bg * 8) | A_NORMAL);
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
                attrset(COLOR_PAIR(63) | A_BOLD);
        else
        {
                if (fg & 128)
                {
                        attrset((COLOR_PAIR((fg - 128) + bg * 8)) | A_BOLD);
                }
                else
                        attrset(COLOR_PAIR(fg + bg * 8) | A_NORMAL);
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

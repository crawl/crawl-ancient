/* Some replacement routines missing in gcc */

#define _NORMALCURSOR 1
#define _NOCURSOR 0
#define O_BINARY O_RDWR
//#include "colours.h"

void lincurses_startup();

int itoa(int value, char *strptr, int radix);

char * strlwr(char *str);

int cprintf (char *format, ... );

int putch(unsigned char chr);

char getche();

int window(int x1, int y1, int x2, int y2);

int clrscr();

void _setcursortype(int curstype);

void textcolor(int col);

int gotoxy(int x, int y);

int wherex();

int wherey();

int stricmp(char *str1, char *str2);

void delay(long time);

int kbhit();

void textbackground(int bg);

void lincurses_shutdown();

void redraw_screen(void);

#ifndef _LINUXLIB_IMPLEMENTATION
/* Some stuff from curses, to remove compiling warnings.. */
extern "C" {
int getstr(char *);
int getch(void);
int noecho(void);
int echo(void);
}
#endif

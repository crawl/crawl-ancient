#ifndef LIBLINUX_H
#define LIBLINUX_H


// Some replacement routines missing in gcc


#define _NORMALCURSOR 1
#define _NOCURSOR 0
#define O_BINARY O_RDWR


char *strlwr(char *str);
char getche(void);


int clrscr(void);
int cprintf(const char *format,...);
int gotoxy(int x, int y);
int itoa(int value, char *strptr, int radix);
int kbhit(void);
int key_to_command(int);
int putch(unsigned char chr);
int stricmp(char *str1, char *str2);
int translate_keypad(int keyin);
int wherex(void);
int wherey(void);
int window(int x1, int y1, int x2, int y2);


void _setcursortype(int curstype);
void delay(long time);
void init_key_to_command();
void lincurses_shutdown(void);
void lincurses_startup(void);
void textbackground(int bg);
void textcolor(int col);




#ifndef _LIBLINUX_IMPLEMENTATION
/* Some stuff from curses, to remove compiling warnings.. */
extern "C"
{
    int getstr(char *);
    int getch(void);
    int noecho(void);
    int echo(void);
}
#endif


#endif

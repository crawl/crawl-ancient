#ifndef LIBW32C_H
#define LIBW32C_H

#define _X86_
#include <string>
// I think the following definition is all we need from STD namespace..
typedef std::basic_string<char> string;

#include <windef.h>
#include <winbase.h>
#include <wincon.h>

#define _NORMALCURSOR   true
#define _NOCURSOR       false

void init_libw32c(void);
void _setcursortype(int curstype);
void clrscr(void);
void gotoxy(int x, int y);
void textcolor(int c);
void cprintf(const char *s);
void setStringInput(bool value);
void setBuffering(bool value);

void window(int x, int y, int lx, int ly);
int wherex(void);
int wherey(void);
void putch(char c);
int getch(void);
int getche(void);
int kbhit(void);
void delay(int ms);
void textbackground(int c);

inline void srandom(unsigned int seed) { srand(seed); }
inline int random() { return rand(); }

#endif

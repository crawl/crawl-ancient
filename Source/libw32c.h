#ifndef LIBW32C_H
#define LIBW32C_H

#define _X86_
#include <string>
// I think the following definition is all we need from STD namespace..
#ifdef __IBMCPP__                       // Borland 5.01 doesn't seem to need this
typedef std::basic_string<char> string;
#endif

#include <excpt.h>
#include <stdarg.h>
#include <windef.h>
#include <winbase.h>
#include <wingdi.h>
#include <winuser.h>
#include <winnls.h>
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
DWORD getConsoleString(char *buf, DWORD maxlen);

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

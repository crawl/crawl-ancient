/*
 *  File:       libw32c.cc
 *  Summary:    Functions for windows32 console mode support
 *  Written by: Gordon Lipford
 *
 *  Change History (most recent first):
 *
 *      <1>      1 Mar 2000        GDL       Created
 *
 */

#include "AppHdr.h"
#include "version.h"
#include "defines.h"
#include <winuser.h>
#ifdef __BCPLUSPLUS__
#include <stdio.h>
#endif

static HANDLE inbuf = NULL;
static HANDLE outbuf = NULL;
static int current_color = -1;
//static FILE *foo = NULL;  //DEBUG

// buffering stuff
#define BUF_SIZE 128
static char cbuf[BUF_SIZE];
static int bcount = 0;
static bool buffering = false;

// function prototype to make BCPP happy
WORD translatecolor(int col);

void bFlush(void)
{
   DWORD outlength;

   if (bcount > 0)
      if (WriteConsole(outbuf, cbuf, bcount, &outlength, NULL) == 0)
         fputs("WriteConsole() failed!", stderr);

   bcount = 0;
}


void setStringInput(bool value)
{
   DWORD modes;
   if (value == TRUE)
      modes = ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT | ENABLE_PROCESSED_INPUT;
   else
      modes = NULL;

   if ( SetConsoleMode( inbuf,  modes ) == 0) {
      fputs("Error initializing console input mode.", stderr);
      exit(0);
   }
   // now flush it
   FlushConsoleInputBuffer( inbuf );
}

void init_libw32c(void)
{
   inbuf = GetStdHandle( STD_INPUT_HANDLE );
   outbuf = GetStdHandle( STD_OUTPUT_HANDLE );

   if (inbuf == INVALID_HANDLE_VALUE || outbuf == INVALID_HANDLE_VALUE) {
      fputs("Could not initialize libw32c console support.", stderr);
      exit(0);
   }
   SetConsoleTitle( "Crawl " VERSION );
   // by default,  set string input to false:  use char-input only
   setStringInput( false );
   if (SetConsoleMode( outbuf, ENABLE_PROCESSED_OUTPUT ) == 0) {
      fputs("Error initializing console output mode.", stderr);
      exit(0);
   }

   // initialize static text color (cuts down on calls to
   // SetConsoleTextAttribute()
   textcolor(0);

   //DEBUG
   //foo = fopen("debug.txt", "w");
}

void _setcursortype(int curstype)
{
   CONSOLE_CURSOR_INFO cci;

   cci.dwSize = 5;
   cci.bVisible = (bool)curstype;
   SetConsoleCursorInfo( outbuf, &cci );
}

void clrscr(void)
{
   int i;
   _setcursortype(_NOCURSOR);
   for (i=0; i<NUMBER_OF_LINES; i++)
   {
       gotoxy(1, i+1);
       cprintf("                                        "
               "                                        ");
   }
   gotoxy(1,1);
   _setcursortype(_NORMALCURSOR);
}

void gotoxy(int x, int y)
{
   // assume the Crawl coordinate system starts at (1,1)
   COORD xy;
   xy.X = x-1;
   xy.Y = y-1;

   // always flush on goto
   bFlush();

   if (SetConsoleCursorPosition(outbuf, xy) == 0)
      fputs("SetConsoleCursorPosition() failed!", stderr);
}

void textcolor(int c)
{
   if (c != current_color)
   {
      // always flush on color change
      bFlush();
      if (SetConsoleTextAttribute( outbuf, translatecolor(c)) == 0)
         fputs("SetConsoleTextAttribute() failed!", stderr);
      current_color = c;
   }
}

void cprintf(const char *s)
{
   DWORD outlength;
   if (WriteConsole(outbuf, s, strlen(s), &outlength, NULL) == 0)
      fputs("WriteConsole() failed!", stderr);
}

void window(int x, int y, int lx, int ly)
{
   // do nothing
}

int wherex(void)
{
   CONSOLE_SCREEN_BUFFER_INFO csbi;
   if (GetConsoleScreenBufferInfo( outbuf, &csbi ) == 0)
      fputs("GetConsoleScreenBufferInfo() failed!", stderr);
   return csbi.dwCursorPosition.X + 1;
}

int wherey(void)
{
   CONSOLE_SCREEN_BUFFER_INFO csbi;
   if (GetConsoleScreenBufferInfo( outbuf, &csbi ) == 0)
      fputs("GetConsoleScreenBufferInfo() failed!", stderr);
   return csbi.dwCursorPosition.Y + 1;
}

void putch(char c)
{
   static char tinybuf[2] = "x";
   // special case: check for '0' char: map to space
   if (c==0)
      c = ' ';

   // buffer if appropriate
   if (buffering)
   {
      cbuf[bcount] = c;
      bcount += 1;
      if (bcount == BUF_SIZE)
         bFlush();
   }
   else
   {
      tinybuf[0] = c;
      cprintf(tinybuf);
   }
}

// translate virtual keys

static int vk_tr[4][9] = // virtual key, unmodified, shifted,  control
   {
   { VK_END, VK_DOWN, VK_NEXT, VK_LEFT, VK_CLEAR, VK_RIGHT, VK_HOME, VK_UP, VK_PRIOR },
   { 'b', 'j', 'n', 'h', '.', 'l', 'y', 'k', 'u' },
   { '1', '2', '3', '4', '5', '6', '7', '8', '9' },
   { 2, 10, 14, 8, 0, 12, 25, 11, 21 },
   };

int vk_translate( WORD VirtCode, CHAR c, DWORD cKeys)
{
   bool shftDown = false;
   bool ctrlDown = false;

   // DEBUG
   //fprintf(foo, "Received code %d (%c) with modifiers: %d\n", VirtCode, c, cKeys);

   // step 1 - we don't care about shift or control
   if (VirtCode == VK_SHIFT || VirtCode == VK_CONTROL)
      return 0;

   // step 2 - translate the <Esc> key to 0x1b
   if (VirtCode == VK_ESCAPE)
      return 0x1b;            // same as it ever was..

   // step 3 - translate shifted or controlled numeric keypad keys
   if (cKeys & SHIFT_PRESSED)
      shftDown = true;
   if (cKeys & (RIGHT_CTRL_PRESSED | LEFT_CTRL_PRESSED))
   {
      ctrlDown = true;           // control takes precedence over shift
      shftDown = false;
   }

   // hack - translate ^P and ^Q since 16 and 17 are taken by CTRL and SHIFT
   if ((VirtCode == 80 || VirtCode == 81) && ctrlDown)
      return VirtCode & 0x003f;     // shift back down

   if (VirtCode == VK_DELETE && !ctrlDown)         // assume keypad '.'
      return '.';

   // see if we're a vkey
   int mkey;
   for(mkey = 0; mkey<9; mkey++)
      if (VirtCode == vk_tr[0][mkey]) break;

   // step 4 - just return the damn key.
   if (mkey == 9)
      return (int)c;

   // now translate the key.  Dammit.  This is !@#$(*& garbage.
   if (shftDown)
      return vk_tr[2][mkey];
   // control key?
   if (ctrlDown)
      return vk_tr[3][mkey];
   return vk_tr[1][mkey];
}

int getch(void)
{
    INPUT_RECORD ir;
    DWORD nread;
    int key = 0;
    static int repeat_count = 0;
    static int repeat_key = 0;

    KEY_EVENT_RECORD *kr;

    // handle key repeats
    if (repeat_count > 0)
    {
       repeat_count -= 1;
       return repeat_key;
    }

    while(1)
    {
       if (ReadConsoleInput( inbuf, &ir, 1, &nread) == 0)
           fputs("Error in ReadConsoleInput()!", stderr);
       if (nread > 0)
       {
          // ignore if it isn't a keyboard event.
          if (ir.EventType == KEY_EVENT)
          {
             kr = &(ir.Event.KeyEvent);
             // ignore if it is a 'key up' - we only want 'key down'
             if (kr->bKeyDown == true)
             {
                key = vk_translate( kr->wVirtualKeyCode, kr->uChar.AsciiChar, kr->dwControlKeyState );
                if (key > 0)
                {
                   repeat_count = kr->wRepeatCount - 1;
                   repeat_key = key;
                   break;
                }
             }
          }
       }
    }
    // DEBUG
    //fprintf(foo, "getch() returning %02x (%c)\n", key, key);

    return key;
}

int getche(void)
{
   int val = getch();

   if (val != 0)
      putch(val);
   return val;
}

int kbhit()
{
   // do nothing.  We could use PeekConsoleInput,  I suppose..
   return 0;
}

void delay(int ms)
{
   Sleep((DWORD)ms);
}

void textbackground(int c)
{
   // do nothing
}

// Translate DOS colors.
WORD translatecolor(int col)
{
    // all we have to work with are:
    // FOREGROUND_BLUE, FOREGROUND_GREEN, FOREGROUND_RED, and FOREGROUND_INTENSITY

    const WORD blue = FOREGROUND_BLUE;
    const WORD green = FOREGROUND_GREEN;
    const WORD red = FOREGROUND_RED;
    const WORD bright = FOREGROUND_INTENSITY;

    WORD tcol = 0;

    switch (col)
    {
    case BLACK:
        break;
    case BLUE:
        tcol = blue;
        break;
    case GREEN:
        tcol = green;
        break;
    case CYAN:
        tcol = blue | green;
        break;
    case RED:
        tcol = red;
        break;
    case MAGENTA:
        tcol = red;
        break;
    case BROWN:
        tcol = red | green;
        break;
    case LIGHTGREY:
        tcol = red | blue | green | bright;
        break;
    case DARKGREY:
        tcol = red | blue | green;
        break;
    case LIGHTBLUE:
        tcol = blue | bright;
        break;
    case LIGHTGREEN:
        tcol = green | bright;
        break;
    case LIGHTCYAN:
        tcol = blue | green | bright;
        break;
    case LIGHTRED:
        tcol = red | bright;
        break;
    case LIGHTMAGENTA:
        tcol = red | bright;
        break;
    case YELLOW:
        tcol =  green | red | bright;
        break;
    case WHITE:
        tcol = red | blue | green | bright;
        break;
    default:
        tcol = green;
        break;                  //mainly for debugging
    }
    return tcol;
}

DWORD getConsoleString(char *buf, DWORD maxlen)
{
   DWORD nread;
   // set console input to line mode
   setStringInput( true );
   if (ReadConsole( inbuf, buf, maxlen-1, &nread, NULL) == 0)
      fputs("Error in ReadConsole()!", stderr);

   // terminate string,  then strip CRLF, replace with \0
   buf[maxlen-1] = '\0';
   int i;
   for (i=(nread<3 ? 0 : nread-3); i<nread; i++)
   {
      if (buf[i] == 0x0A || buf[i] == 0x0D)
      {
         buf[i] = '\0';
         break;
      }
    }

   // reset console mode - also flushes if player has typed in
   // too long of a name so we don't get silly garbage on return.
   setStringInput( false );
   return nread;
}

void setBuffering( bool value )
{
   if (value == false)
   {
       // must flush buffer
       bFlush();
   }
   buffering = value;
}

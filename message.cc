/*
 *  File:       message.cc
 *  Summary:    Functions used to print messages.
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 *      <3>      5/20/99        BWR             Extended screen lines support
 *      <2>      5/08/99        JDJ             mpr takes a const char* instead of a char array.
 *      <1>      -/--/--        LRH             Created
 */

#include "AppHdr.h"
#include "message.h"

#ifdef DOS
#include <conio.h>
#endif

#ifdef USE_CURSES
#include <curses.h>
#endif

#include "externs.h"
#include "enum.h"

#include <string.h>

#ifdef MACROS
  #include "macro.h"
#endif

void more(void);

char scrloc = 1;                /* Line of next (previous?) message */

char store_message[30][200];    /* buffer of old messages */
char store_count = 0;           /* current position in store_message */

char message_colour;                    /* What colour to print the next message */

void mpr(const char *inf)
{
    char inf_screens = 0;

    char info2[80];

    you.running = 0;

#ifdef DOS_TERM
    window(1, 1, 80, 25);
#endif

    textcolor(7);

//if (scrloc == 8)
    if (scrloc == NUMBER_OF_LINES - 18)
    {

#ifdef PLAIN_TERM
        gotoxy(2, NUMBER_OF_LINES);
        _setcursortype(_NORMALCURSOR);
        textcolor(7);

// cdl -- cprintf("\r--more--");
#ifdef DOS
        cprintf(EOL);
#endif
        cprintf("--more--");

        char keypress = 0;

        do
        {
            keypress = getch();
        }
        while (keypress != 32 && keypress != 13);

        int startLine = 18;
        gotoxy(1, startLine);
#ifdef USE_CURSES
        clrtobot();
#else
        int numLines = NUMBER_OF_LINES - startLine + 1;

        for (int i = 0; i < numLines; i++)
        {
            cprintf("                                                                               ");
            if (i < numLines - 1)
            {
                cprintf(EOL);
            }
        }
#endif
#endif

#ifdef DOS_TERM
        window(1, 18, 78, 25);
        more();
        window(1, 1, 80, 25);
#endif
        scrloc = 0;
    }

//gotoxy(1, scrloc + 17);
    gotoxy(1, scrloc + 18);
    strncpy(info2, inf, 78);
    info2[78] = 0;
    textcolor(message_colour);
    cprintf(info2);
    message_colour = LIGHTGREY;
    textcolor(LIGHTGREY);

/* Put the message into store_message, and move the '---' line forward */
    strncpy(store_message[store_count], inf, 78);
    store_count++;
    if (store_count > 23)
        store_count -= 24;
    strcpy(store_message[store_count], "------------------------------------------------------------------------------");

    inf_screens = 0;

    scrloc++;

}                               // end of message function

void mesclr()
{
    _setcursortype(_NOCURSOR);
#ifdef DOS_TERM
    window(1, 18, 78, 25);
    clrscr();
    window(1, 1, 80, 25);
#endif

#ifdef PLAIN_TERM
    int startLine = 18;
    gotoxy(1, startLine);

#ifdef USE_CURSES
    clrtobot();
#else
    int numLines = NUMBER_OF_LINES - startLine + 1;
    for (int i = 0; i < numLines; i++)
    {
        cprintf("                                                                               ");
        if (i < numLines - 1)
        {
            cprintf(EOL);
        }
    }
#endif

#endif

    scrloc = 0;
    _setcursortype(_NORMALCURSOR);
    gotoxy(18, 9);
}


void more()
{
    char keypress = 0;

#ifdef PLAIN_TERM
    gotoxy(2, NUMBER_OF_LINES);
#endif
#ifdef DOS_TERM
    window(1, 18, 80, 25);
    gotoxy(2, 7);
#endif
    _setcursortype(_NORMALCURSOR);
    textcolor(7);

// cdl -- cprintf("\r--more--");
#ifdef DOS
    cprintf(EOL);
#endif
    cprintf("--more--");

    do
    {
        keypress = getch();
    }
    while (keypress != 32 && keypress != 13);

/* Juho Snellman rewrote this part of the function: */
    keypress = 0;
#ifdef DOS_TERM
    window(1, 18, 80, 25);
    clrscr();
#endif
    /* clrscr() should be inside the DOS-define, and the message
     * buffer cleared in a different way for Linux to fix annoying
     * redraw bug whenever the more-prompt showed up. -- jsnell */
#ifdef PLAIN_TERM
    int startLine = 18;
    gotoxy(1, startLine);

#ifdef USE_CURSES
    clrtobot();
#else
    int numLines = NUMBER_OF_LINES - startLine + 1;

    for (int i = 0; i < numLines; i++)
    {
        cprintf("                                                                               ");

        if (i < numLines - 1)
        {
            cprintf(EOL);
        }
    }
#endif

#endif

    scrloc = 0;
}



/*
   keypress = 0;
   #ifdef DOS_TERM
   window(1, 18, 80, 25);
   #endif
   clrscr();
   scrloc = 0;

   } */






void replay_messages(void)
{
    _setcursortype(_NOCURSOR);

    int i = 0;
    int j = 0;
    int line = 0;

#ifdef DOS_TERM
    char buffer[4800];

    window(1, 1, 80, 25);
    gettext(1, 1, 80, 25, buffer);
#endif

    clrscr();

    gotoxy(1, 1);

    line = 0;

    for (j = 0; j < 24; j++)
    {
        if (strncmp(store_message[j], "--------------", 10) == 0)
        {
            line = j;
            break;
        }
    }

    i = line + 1;
    if (i == 24)
        i = 0;

    do
    {
        cprintf(store_message[i]);
        cprintf(EOL);
        if (i == line)
            break;
        i++;
        if (i == 24)
            i = 0;
    }
    while (1);

/*
   for (i = 0; i < 24; i ++)
   {
   cprintf(store_message [i]);
   cprintf("\n\r");
   }
 */
    if (getch() == 0)
        getch();


#ifdef DOS_TERM
    puttext(1, 1, 80, 25, buffer);
#endif

    _setcursortype(_NORMALCURSOR);

    return;

}


void set_colour(char set_message_colour)
{
 message_colour = set_message_colour;
}

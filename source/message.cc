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
#include "religion.h"

#include <string.h>

#ifdef DOS
#include <conio.h>
#endif

#include "externs.h"
#include "stuff.h"

#ifdef MACROS
#include "macro.h"
#endif

//jmf: brent sez:
//  There's a reason curses is included after the *.h files in beam.cc.
//  There's a reason curses is included after the *.h files in beam.cc.
//  There's a reason curses is included after the *.h files in beam.cc.
//  There's a reason ...
#ifdef USE_CURSES
#include <curses.h>
#endif

char scrloc = 0;                // line of next (previous?) message
message_item store_message[30];    // buffer of old messages
char store_count = 0;           // current position in store_message


static char god_message_altar_colour( char god )
{
    int  rnd;

    switch (god)
    {
    case GOD_SHINING_ONE:
        return (YELLOW);

    case GOD_ZIN:
        return (WHITE);

    case GOD_ELYVILON:
        return (LIGHTBLUE);     // really, LIGHTGREY but that's plain text

    case GOD_OKAWARU:
        return (CYAN);

    case GOD_YREDELEMNUL:
        return (coinflip() ? DARKGREY : RED);

    case GOD_KIKUBAAQUDGHA:
        return (DARKGREY);

    case GOD_XOM:
        return (random2(15) + 1);

    case GOD_VEHUMET:
        rnd = random2(3);
        return ((rnd == 0) ? LIGHTMAGENTA :
                (rnd == 1) ? LIGHTRED
                           : LIGHTBLUE);

    case GOD_MAKHLEB:
        rnd = random2(3);
        return ((rnd == 0) ? RED :
                (rnd == 1) ? LIGHTRED
                           : YELLOW);

    case GOD_TROG:
        return (RED);

    case GOD_NEMELEX_XOBEH:
        return (LIGHTMAGENTA);

    case GOD_SIF_MUNA:
        return (BLUE);

    case GOD_NO_GOD:
    default:
        return(YELLOW);
    }
}

// returns a colour or MSGCOL_MUTED
static char channel_to_colour( int channel, int param )
{
    char        ret;

    switch (Options.channels[ channel ])
    {
    case MSGCOL_PLAIN:
        // note that if the plain channel is muted, then we're protecting
        // the player from having that spead to other other channels here.
        // The intent of plain is to give non-coloured messages, not to
        // supress them.
        if (Options.channels[ MSGCH_PLAIN ] >= MSGCOL_DEFAULT)
            ret = LIGHTGREY;
        else
            ret = Options.channels[ MSGCH_PLAIN ];
        break;

    case MSGCOL_DEFAULT:
    case MSGCOL_ALTERNATE:
        switch (channel)
        {
        case MSGCH_GOD:
            ret = (Options.channels[ channel ] == MSGCOL_DEFAULT)
                                    ? god_colour( param )
                                    : god_message_altar_colour( param );
            break;

        case MSGCH_DURATION:
            ret = LIGHTBLUE;
            break;

        case MSGCH_DANGER:
            ret = RED;
            break;

        case MSGCH_WARN:
            ret = LIGHTRED;
            break;

        case MSGCH_FOOD:
            ret = YELLOW;
            break;

        case MSGCH_INTRINSIC_GAIN:
            ret = GREEN;
            break;

        case MSGCH_RECOVERY:
            ret = LIGHTGREEN;
            break;

        case MSGCH_TALK:
            ret = WHITE;
            break;

        case MSGCH_MONSTER_SPELL:
        case MSGCH_MONSTER_ENCHANT:
            ret = LIGHTMAGENTA;
            break;

        case MSGCH_MONSTER_DAMAGE:
            ret =  ((param == MDAM_DEAD)               ? RED :
                    (param >= MDAM_HORRIBLY_DAMAGED)   ? LIGHTRED :
                    (param >= MDAM_MODERATELY_DAMAGED) ? YELLOW
                                                       : LIGHTGREY);
            break;

        case MSGCH_PROMPT:
            ret = CYAN;
            break;

        case MSGCH_PLAIN:
        default:
            ret = LIGHTGREY;
            break;
        }
        break;

    case MSGCOL_MUTED:
        ret = MSGCOL_MUTED;
        break;

    default:
        // Setting to a specific colour is handled here, special
        // cases should be handled above.
        if (channel == MSGCH_MONSTER_DAMAGE)
        {
            // a special case right now for monster damage (at least until
            // the init system is improved)... selecting a specific
            // colour here will result in only the death messages coloured
            if (param == MDAM_DEAD)
                ret = Options.channels[ channel ];
            else if (Options.channels[ MSGCH_PLAIN ] >= MSGCOL_DEFAULT)
                ret = LIGHTGREY;
            else
                ret = Options.channels[ MSGCH_PLAIN ];
        }
        else
            ret = Options.channels[ channel ];
        break;
    }

    return (ret);
}

void mpr(const char *inf, int channel, int param)
{
    char info2[80];

    int colour = channel_to_colour( channel, param );
    if (colour == MSGCOL_MUTED)
        return;

    you.running = 0;

#ifdef DOS_TERM
    window(1, 1, 80, 25);
#endif

    textcolor(LIGHTGREY);

    if (scrloc == NUMBER_OF_LINES - 18) // ( scrloc == 8 )
        more();

    gotoxy(1, scrloc + 18);     // (1, scrloc + 17)
    strncpy(info2, inf, 78);
    info2[78] = 0;

    textcolor( colour );
    cprintf(info2);

    /* Put the message into store_message, and move the '---' line forward */
    store_message[store_count].text = inf;
    store_message[store_count].channel = channel;
    store_message[store_count].param = param;
    store_count++;

    // reset colour
    textcolor(LIGHTGREY);

    if (store_count > 23)
        store_count -= 24;

    store_message[store_count].text = "------------------------------------------------------------------------------";
    store_message[store_count].channel = MSGCH_PLAIN;
    store_message[store_count].param = 0;

    scrloc++;
}                               // end mpr()

bool any_messages(void)
{
    return (scrloc > 0);
}

void mesclr(void)
{
    // if no messages, return.
    if (scrloc == 0)
        return;

    // turn cursor off -- avoid 'cursor dance'
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
        cprintf( "                                                                               " );

        if (i < numLines - 1)
        {
            cprintf(EOL);
        }
    }
#endif
#endif

    // turn cursor back on
    _setcursortype(_NORMALCURSOR);

    scrloc = 0;
}                               // end mseclr()

void more(void)
{
    char keypress = 0;

#ifdef PLAIN_TERM
    gotoxy(2, NUMBER_OF_LINES);
#endif

#ifdef DOS_TERM
    window(1, 18, 80, 25);
    gotoxy(2, 7);
#endif

    textcolor(LIGHTGREY);

    //cdl -- cprintf("\r--more--");

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

    mesclr();
}                               // end more()

void replay_messages(void)
{
    int i = 0;
    int j = 0;
    int line = 0;

#ifdef DOS_TERM
    char buffer[4800];

    window(1, 1, 80, 25);
    gettext(1, 1, 80, 25, buffer);
#endif

    // turn cursor off
    _setcursortype(_NOCURSOR);

    clrscr();

    gotoxy(1, 1);

    line = 0;

    for (j = 0; j < 24; j++)
    {
        if (strncmp(store_message[j].text.c_str(), "----------", 10) == 0)
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
        int colour = channel_to_colour( store_message[i].channel,
                                        store_message[i].param );
        if (colour == MSGCOL_MUTED)
            continue;

        textcolor( colour );
        cprintf(store_message[i].text.c_str());
        cprintf(EOL);
        textcolor(LIGHTGREY);

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

    // turn cursor back on
    _setcursortype(_NORMALCURSOR);

    if (getch() == 0)
        getch();

#ifdef DOS_TERM
    puttext(1, 1, 80, 25, buffer);
#endif

    return;
}                               // end replay_messages()

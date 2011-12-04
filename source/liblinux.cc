/*
 *  File:       liblinux.cc
 *  Summary:    Functions for linux, unix, and curses support
 *  Written by: ?
 *
 *  Change History (most recent first):
 *
 *      <6>     10/11/99        BCR     Swapped 'v' and 'V' commands, fixed
 *                                      & for debug command.
 *      <5>     9/25/99         CDL     linuxlib -> liblinux
 *                                      changes to fix "macro problem"
 *                                      keypad -> command lookup
 *      <4>     99/07/13        BWR     added translate_keypad(), to try and
 *                                      translate keypad escape sequences into
 *                                      numeric char values.
 *      <3>     99/06/18        BCR     moved CHARACTER_SET #define to AppHdr.h
 *      <2>     99/05/12        BWR     Tchars, signals, solaris support
 *
 *      <1>      -/--/--        ?       Created
 *
 */

/* Some replacement routines missing in gcc
   Some of these are inspired by/stolen from the Linux-conio package
   by Mental EXPlotion. Hope you guys don't mind.
   The colour exchange system is perhaps a little overkill, but I wanted
   it to be general to support future changes.. The only thing not
   supported properly is black on black (used by curses for "normal" mode)
   and white on white (used by me for "bright black" (darkgray) on black

   Jan 1998 Svante Gerhard <svante@algonet.se>                          */

#include "AppHdr.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdarg.h>
#include <ctype.h>
#define _LIBLINUX_IMPLEMENTATION
#include "liblinux.h"
#include "defines.h"

#include "enum.h"
#include "globals.h"
#include "externs.h"


#if defined(USE_POSIX_TERMIOS)
#include <termios.h>

static struct termios def_term;
static struct termios game_term;

#elif defined(USE_TCHARS_IOCTL)

#include <sys/ttold.h>
#include <sys/time.h>
#include <sys/types.h>
#include <errno.h>

static struct ltchars def_term;
static struct ltchars game_term;

#endif

#ifdef USE_UNIX_SIGNALS
#include <signal.h>
#endif

// Its best if curses comes at the end (name conflicts with Solaris). -- bwr
#ifndef CURSES_INCLUDE_FILE
    #include <curses.h>
#else
    #include CURSES_INCLUDE_FILE
#endif

static int Character_Set = 0;

// Globals holding current text/backg. colors
static short FG_COL = WHITE;
static short BG_COL = BLACK;

// a lookup table to convert keypresses to command enums
static int key_to_command_table[KEY_MAX];

static unsigned int convert_to_curses_attr( int chattr )
{
    switch (chattr)
    {
    case CHATTR_STANDOUT:       return (A_STANDOUT);
    case CHATTR_BOLD:           return (A_BOLD);
    case CHATTR_BLINK:          return (A_BLINK);
    case CHATTR_UNDERLINE:      return (A_UNDERLINE);
    case CHATTR_REVERSE:        return (A_REVERSE);
    case CHATTR_DIM:            return (A_DIM);
    default:                    return (A_NORMAL);
    }
}

static inline short macro_colour( short col )
{
    return (Options.colour[ col ]);
}

// Translate DOS colors to curses.
static short translate_colour( short col )
{
    switch (col)
    {
    case BLACK:         return (COLOR_BLACK);
    case BLUE:          return (COLOR_BLUE);
    case GREEN:         return (COLOR_GREEN);
    case CYAN:          return (COLOR_CYAN);
    case RED:           return (COLOR_RED);
    case MAGENTA:       return (COLOR_MAGENTA);
    case BROWN:         return (COLOR_YELLOW);
    case LIGHTGREY:     return (COLOR_WHITE);

    // adding a flag to these to mark that we need to use highlights
    case DARKGREY:      return (COLOR_BLACK   | COLFLAG_CURSES_BRIGHTEN);
    case LIGHTBLUE:     return (COLOR_BLUE    | COLFLAG_CURSES_BRIGHTEN);
    case LIGHTGREEN:    return (COLOR_GREEN   | COLFLAG_CURSES_BRIGHTEN);
    case LIGHTCYAN:     return (COLOR_CYAN    | COLFLAG_CURSES_BRIGHTEN);
    case LIGHTRED:      return (COLOR_RED     | COLFLAG_CURSES_BRIGHTEN);
    case LIGHTMAGENTA:  return (COLOR_MAGENTA | COLFLAG_CURSES_BRIGHTEN);
    case YELLOW:        return (COLOR_YELLOW  | COLFLAG_CURSES_BRIGHTEN);
    case WHITE:         return (COLOR_WHITE   | COLFLAG_CURSES_BRIGHTEN);
    default:
        break;
    }

    return (COLOR_GREEN);
}

void set_altcharset( bool alt_on )
{
    Character_Set = ((alt_on) ? A_ALTCHARSET : 0);
}

bool get_altcharset( void )
{
    return (Character_Set != 0);
}

static void setup_colour_pairs( void )
{

    short i, j;

    for (i = 0; i < 8; i++)
    {
        for (j = 0; j < 8; j++)
        {
            if (( i > 0 ) || ( j > 0 ))
                init_pair(i * 8 + j, j, i);
        }
    }

    init_pair(63, COLOR_BLACK, Options.background);
}          // end setup_colour_pairs()


#if defined(USE_POSIX_TERMIOS)

static void termio_init()
{
    tcgetattr( 0, &def_term );
    memcpy( &game_term, &def_term, sizeof(struct termios) );

    // Lets recover some control sequences
#ifdef VERASE
    game_term.c_cc[VERASE] = static_cast<char>( -1 );     // ctrl-H
#endif

#ifdef VKILL
    game_term.c_cc[VKILL] = static_cast<char>( -1 );      // ctrl-U
#endif

#ifdef VINTR
    game_term.c_cc[VINTR] = static_cast<char>( -1 );      // ctrl-C
#endif

#ifdef VSTART
    game_term.c_cc[VSTART] = static_cast<char>( -1 );     // ctrl-Q
#endif

#ifdef VSTOP
    game_term.c_cc[VSTOP] = static_cast<char>( -1 );      // ctrl-S
#endif

#ifdef VSUSP
    game_term.c_cc[VSUSP] = static_cast<char>( -1 );      // ctrl-Z
#endif

#ifdef VQUIT
    game_term.c_cc[VQUIT] = static_cast<char>( -1 );      // ctrl-backslash
#endif

#ifdef VDSUSP
    game_term.c_cc[VDSUSP] = static_cast<char>( -1 );     // ctrl-Y
#endif

    tcsetattr( 0, TCSAFLUSH, &game_term );
}

#elif defined(USE_TCHARS_IOCTL)

static void termio_init()
{
    ioctl( 0, TIOCGLTC, &def_term );
    memcpy( &game_term, &def_term, sizeof(struct ltchars) );

    game_term.t_suspc = -1;
    game_term.t_dsuspc = -1;
    ioctl( 0, TIOCSLTC, &game_term );
}

#endif

static inline void register_key( int key, int command )
{
    key_to_command_table[key] = command;
}

int getch_ck()
{
    int c = getch();

    switch (c)
    {
    case KEY_HOME:  return (CK_HOME);
    case KEY_PPAGE: return (CK_PGUP);
    case KEY_END:   return (CK_END);
    case KEY_NPAGE: return (CK_PGDN);
    case KEY_UP:    return (CK_UP);
    case KEY_DOWN:  return (CK_DOWN);
    case KEY_LEFT:  return (CK_LEFT);
    case KEY_RIGHT: return (CK_RIGHT);
    default:        return (c);
    }
}

void init_key_to_command()
{
    int i;

    // initialize to "do nothing"
    for (i = 0; i < KEY_MAX; i++)
    {
        key_to_command_table[i] = CMD_NO_CMD;
    }

    // lower case
    register_key( 'a', CMD_USE_ABILITY );
    register_key( 'b', CMD_MOVE_DOWN_LEFT );
    register_key( 'c', CMD_CLOSE_DOOR );
    register_key( 'd', CMD_DROP );
    register_key( 'e', CMD_EAT );
    register_key( 'f', CMD_FIRE );
    register_key( 'g', CMD_PICKUP );
    register_key( 'h', CMD_MOVE_LEFT );
    register_key( 'i', CMD_DISPLAY_INVENTORY );
    register_key( 'j', CMD_MOVE_DOWN );
    register_key( 'k', CMD_MOVE_UP );
    register_key( 'l', CMD_MOVE_RIGHT );
    register_key( 'm', CMD_DISPLAY_SKILLS );
    register_key( 'n', CMD_MOVE_DOWN_RIGHT );
    register_key( 'o', CMD_OPEN_DOOR );
    register_key( 'p', CMD_PRAY );
    register_key( 'q', CMD_QUAFF );
    register_key( 'r', CMD_READ );
    register_key( 's', CMD_SEARCH );
    register_key( 't', CMD_THROW );
    register_key( 'u', CMD_MOVE_UP_RIGHT );
    register_key( 'v', CMD_EXAMINE_OBJECT );
    register_key( 'w', CMD_WIELD_WEAPON );
    register_key( 'x', CMD_LOOK_AROUND );
    register_key( 'y', CMD_MOVE_UP_LEFT );
    register_key( 'z', CMD_ZAP_WAND );

    // upper case
    register_key( 'A', CMD_DISPLAY_MUTATIONS );
    register_key( 'B', CMD_RUN_DOWN_LEFT );
    register_key( 'C', CMD_EXPERIENCE_CHECK );
    register_key( 'D', CMD_BUTCHER );
    register_key( 'E', CMD_EVOKE );
    register_key( 'F', CMD_NO_CMD );
    register_key( 'G', CMD_NO_CMD );
    register_key( 'H', CMD_RUN_LEFT );
    register_key( 'I', CMD_OBSOLETE_INVOKE );
    register_key( 'J', CMD_RUN_DOWN );
    register_key( 'K', CMD_RUN_UP );
    register_key( 'L', CMD_RUN_RIGHT );
    register_key( 'M', CMD_MEMORISE_SPELL );
    register_key( 'N', CMD_RUN_DOWN_RIGHT );
    register_key( 'O', CMD_DISPLAY_OVERMAP );
    register_key( 'P', CMD_WEAR_JEWELLERY );
    register_key( 'Q', CMD_QUIT );
    register_key( 'R', CMD_REMOVE_JEWELLERY );
    register_key( 'S', CMD_SAVE_GAME );
    register_key( 'T', CMD_REMOVE_ARMOUR );
    register_key( 'U', CMD_RUN_UP_RIGHT );
    register_key( 'V', CMD_NO_CMD );
    register_key( 'W', CMD_WEAR_ARMOUR );
    register_key( 'X', CMD_DISPLAY_MAP );
    register_key( 'Y', CMD_RUN_UP_LEFT );
    register_key( 'Z', CMD_CAST_SPELL );

    // control
    register_key( CONTROL('A'), CMD_TOGGLE_AUTOPICKUP );
    register_key( CONTROL('B'), CMD_OPEN_DOOR_DOWN_LEFT );
    register_key( CONTROL('C'), CMD_CLEAN_MAP );
    register_key( CONTROL('D'), CMD_NO_CMD );
    register_key( CONTROL('E'), CMD_FORGET_STASH );
    register_key( CONTROL('F'), CMD_FIX_WAYPOINT );
    register_key( CONTROL('G'), CMD_INTERLEVEL_TRAVEL );
    register_key( CONTROL('H'), CMD_OPEN_DOOR_LEFT );
    register_key( CONTROL('I'), CMD_NO_CMD );
    register_key( CONTROL('J'), CMD_OPEN_DOOR_DOWN );
    register_key( CONTROL('K'), CMD_OPEN_DOOR_UP );
    register_key( CONTROL('L'), CMD_OPEN_DOOR_RIGHT );
    register_key( CONTROL('M'), CMD_NO_CMD );
    register_key( CONTROL('N'), CMD_OPEN_DOOR_DOWN_RIGHT );
    register_key( CONTROL('O'), CMD_EXPLORE );
    register_key( CONTROL('P'), CMD_REPLAY_MESSAGES );
    register_key( CONTROL('Q'), CMD_NO_CMD );
    register_key( CONTROL('R'), CMD_REDRAW_SCREEN );
    register_key( CONTROL('S'), CMD_MARK_STASH );
    register_key( CONTROL('T'), CMD_NO_CMD );
    register_key( CONTROL('U'), CMD_OPEN_DOOR_UP_LEFT );
    register_key( CONTROL('V'), CMD_GET_VERSION );
    register_key( CONTROL('W'), CMD_NO_CMD );
    register_key( CONTROL('X'), CMD_SAVE_GAME_NOW );
    register_key( CONTROL('Y'), CMD_OPEN_DOOR_UP_RIGHT );
    register_key( CONTROL('Z'), CMD_SUSPEND_GAME );

    // other printables
    register_key( '.', CMD_MOVE_NOWHERE );
    register_key( '<', CMD_GO_UPSTAIRS );
    register_key( '>', CMD_GO_DOWNSTAIRS );
    register_key( '@', CMD_DISPLAY_CHARACTER_STATUS );
    register_key( ',', CMD_PICKUP );
    register_key( ';', CMD_INSPECT_FLOOR );
    register_key( '!', CMD_SHOUT );
    register_key( '^', CMD_DISPLAY_RELIGION );
    register_key( '#', CMD_CHARACTER_DUMP );
    register_key( '=', CMD_ADJUST_INVENTORY );
    register_key( '?', CMD_DISPLAY_COMMANDS );
    register_key( '`', CMD_MACRO_ADD );
    register_key( '~', CMD_MACRO_SAVE );
    register_key( '&', CMD_WIZARD );
    register_key( '"', CMD_LIST_JEWELLERY );


    // I'm making this both, because I'm tried of changing it
    // back to '[' (the character that represents armour on the map) -- bwr
    register_key( '[', CMD_LIST_ARMOUR );
    register_key( ']', CMD_LIST_ARMOUR );

    // This one also ended up backwards this time, so it's also going on
    // both now -- should be ')'... the same character that's used to
    // represent weapons.
    register_key( ')', CMD_LIST_WEAPONS );
    register_key( '(', CMD_LIST_WEAPONS );

    register_key( '\\', CMD_DISPLAY_KNOWN_OBJECTS );
    register_key( '\'', CMD_WEAPON_SWAP );

    // digits
    register_key( '1', CMD_MOVE_DOWN_LEFT );
    register_key( '2', CMD_MOVE_DOWN );
    register_key( '3', CMD_MOVE_DOWN_RIGHT );
    register_key( '4', CMD_MOVE_LEFT );
    register_key( '5', CMD_REST );
    register_key( '6', CMD_MOVE_RIGHT );
    register_key( '7', CMD_MOVE_UP_LEFT );
    register_key( '8', CMD_MOVE_UP );
    register_key( '9', CMD_MOVE_UP_RIGHT );

    // keypad
    register_key( KEY_A1, CMD_MOVE_UP_LEFT );
    register_key( KEY_A3, CMD_MOVE_UP_RIGHT );
    register_key( KEY_C1, CMD_MOVE_DOWN_LEFT );
    register_key( KEY_C3, CMD_MOVE_DOWN_RIGHT );

    register_key( KEY_HOME, CMD_MOVE_UP_LEFT );
    register_key( KEY_PPAGE, CMD_MOVE_UP_RIGHT );
    register_key( KEY_END, CMD_MOVE_DOWN_LEFT );
    register_key( KEY_NPAGE, CMD_MOVE_DOWN_RIGHT );

    register_key( KEY_B2, CMD_REST );

    register_key( KEY_UP, CMD_MOVE_UP );
    register_key( KEY_DOWN, CMD_MOVE_DOWN );
    register_key( KEY_LEFT, CMD_MOVE_LEFT );
    register_key( KEY_RIGHT, CMD_MOVE_RIGHT );

    // other odd things
    // key_to_command_table[ 263 ] = CMD_OPEN_DOOR_LEFT;   // backspace

    // these are invalid keys, but to help kludge running
    // pass them through unmolested
    register_key( 128, 128 );
    register_key( '*', '*' );
    register_key( '/', '/' );
}

int key_to_command(int keyin)
{
    return (key_to_command_table[keyin]);
}

void lincurses_startup( void )
{
    initscr();
    cbreak();
    noecho();

    nonl();
    intrflush(stdscr, FALSE);

    meta(stdscr, TRUE);
    start_color();
    setup_colour_pairs();
    set_altcharset( false );

    init_key_to_command();

#if defined(USE_POSIX_TERMIOS) || defined(USE_TCHARS_IOCTL)
    termio_init();
#endif

#ifdef USE_UNIX_SIGNALS
#ifdef SIGQUIT
    signal( SIGQUIT, SIG_IGN );
#endif

#ifdef SIGINT
    signal( SIGINT, SIG_IGN );
#endif
#endif

#ifndef SOLARIS
    // Remove this if need be -- it's caused nothing but trouble anyways.
    keypad( stdscr, TRUE );

    // This can cause some display problems under Solaris
    scrollok( stdscr, TRUE );
#endif
}


void lincurses_shutdown()
{
#if defined(USE_POSIX_TERMIOS)
    tcsetattr( 0, TCSAFLUSH, &def_term );
#elif defined(USE_TCHARS_IOCTL)
    ioctl( 0, TIOCSLTC, &def_term );
#endif

#ifdef USE_UNIX_SIGNALS
#ifdef SIGQUIT
    signal( SIGQUIT, SIG_DFL );
#endif

#ifdef SIGINT
    signal( SIGINT, SIG_DFL );
#endif
#endif

    set_altcharset( false );

    // resetty();
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
        sprintf( strptr, "%i", value );
    }

    if (radix == 2)             /* int to "binary string" */
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

        if (!startflag)         /* Special case if value == 0 */
            sprintf( (strptr + ctr++), "0" );

        strptr[ctr] = '\0';
    }

    return (OK);                /* Me? Fail? Nah. */
}


// Convert string to lowercase.
char *strlwr( char *str )
{
    unsigned int i;

    for (i = 0; i < strlen(str); i++)
        str[i] = tolower(str[i]);

    return (str);
}


int cprintf( const char *format, ... )
{
    int i;
    char buffer[2048];          // One full screen if no control seq...

    va_list argp;

    va_start( argp, format );
    vsnprintf( buffer, sizeof buffer, format, argp );
    va_end( argp );

    i = addstr( buffer );
    refresh();

    return (i);
}


int putch( unsigned char chr )
{
    if (chr == 0)
        chr = ' ';

    return (addch( chr ));
}


char getche()
{
    char chr;

    chr = getch();
    addch( chr );
    refresh();

    return (chr);
}


int window( int x1, int y1, int x2, int y2 )
{
    x1 = y1 = x2 = y2 = 0;      /* Do something to them.. makes gcc happy :) */
    return (refresh());
}

// These next four are front functions so that we can reduce
// the amount of curses special code that occurs outside this
// this file.  This is good, since there are some issues with
// name space collisions between curses macros and the standard
// C++ string class.  -- bwr
void update_screen(void)
{
    refresh();
}

void clear_to_end_of_line(void)
{
    textcolor( LIGHTGREY );
    textbackground( BLACK );
    clrtoeol();
}

void clear_to_end_of_screen(void)
{
    textcolor( LIGHTGREY );
    textbackground( BLACK );
    clrtobot();
}

int get_number_of_lines_from_curses(void)
{
    return (LINES);
}

void get_input_line_from_curses( char *const buff, int len )
{
    echo();
    wgetnstr( stdscr, buff, len );
    noecho();
}

int clrscr()
{
    int retval;

    textcolor( LIGHTGREY );
    textbackground( BLACK );
    retval = clear();
    refresh();
    return (retval);
}


void _setcursortype( int curstype )
{
    curs_set(curstype);
}


void textcolor( int col )
{
    short fg, bg;

    FG_COL = col & 0x00ff;

    fg = translate_colour( macro_colour( FG_COL ) );
    bg = translate_colour( (BG_COL == BLACK) ? Options.background : BG_COL );

    // calculate which curses flags we need...
    unsigned int flags = 0;

    if ((col & COLFLAG_FRIENDLY_MONSTER)
        && Options.friend_brand != CHATTR_NORMAL)
    {
        flags |= convert_to_curses_attr( Options.friend_brand );

        // If we can't do a dark grey friend brand, then we'll
        // switch the colour to light grey.
        if (Options.no_dark_brand
                && fg == (COLOR_BLACK | COLFLAG_CURSES_BRIGHTEN)
                && bg == 0)
        {
            fg = COLOR_WHITE;
        }
    }

    // curses typically uses A_BOLD to give bright foreground colour,
    // but various termcaps may disagree
    if (fg & COLFLAG_CURSES_BRIGHTEN)
        flags |= A_BOLD;

    // curses typically uses A_BLINK to give bright background colour,
    // but various termcaps may disagree (in whole or in part)
    if (bg & COLFLAG_CURSES_BRIGHTEN)
        flags |= A_BLINK;

    // Strip out all the bits above the raw 3-bit colour definition
    fg &= 0x0007;
    bg &= 0x0007;

    // figure out which colour pair we want
    const int pair = (fg == 0 && bg == 0) ? 63 : (bg * 8 + fg);

    attrset( COLOR_PAIR(pair) | flags | Character_Set );
}


void textbackground(int col)
{
    short fg, bg;

    BG_COL = col & 0x00ff;
    fg = translate_colour( macro_colour( FG_COL ) );
    bg = translate_colour( (BG_COL == BLACK) ? Options.background : BG_COL );

    unsigned int flags = 0;

    if ((col & COLFLAG_FRIENDLY_MONSTER)
        && Options.friend_brand != CHATTR_NORMAL)
    {
        flags |= convert_to_curses_attr( Options.friend_brand );

        // If we can't do a dark grey friend brand, then we'll
        // switch the colour to light grey.
        if (Options.no_dark_brand
                && fg == (COLOR_BLACK | COLFLAG_CURSES_BRIGHTEN)
                && bg == 0)
        {
            fg = COLOR_WHITE;
        }
    }

    // curses typically uses A_BOLD to give bright foreground colour,
    // but various termcaps may disagree
    if (fg & COLFLAG_CURSES_BRIGHTEN)
        flags |= A_BOLD;

    // curses typically uses A_BLINK to give bright background colour,
    // but various termcaps may disagree
    if (bg & COLFLAG_CURSES_BRIGHTEN)
        flags |= A_BLINK;

    // Strip out all the bits above the raw 3-bit colour definition
    fg &= 0x0007;
    bg &= 0x0007;

    // figure out which colour pair we want
    const int pair = (fg == 0 && bg == 0) ? 63 : (bg * 8 + fg);

    attrset( COLOR_PAIR(pair) | flags | Character_Set );
}

int gotoxy(int x, int y)
{
    return (move( y - 1, x - 1 ));
}

int wherex()
{
    int x, y;

    getyx( stdscr, y, x );
    return (x + 1);
}

int wherey()
{
    int x, y;

    getyx( stdscr, y, x );
    return (y + 1);
}

int stricmp( const char *str1, const char *str2 )
{
    return (strcmp( str1, str2 ));
}

void delay( unsigned long time )
{
    usleep( time * 1000 );
}


/*
   Note: kbhit now in macro.cc
 */

/* This is Juho Snellman's modified kbhit, to work with macros */
int kbhit()
{
    int i;

    nodelay(stdscr, TRUE);
    timeout(0);  // apparently some need this to guarantee non-blocking -- bwr
    i = wgetch(stdscr);
    nodelay(stdscr, FALSE);

    if (i == -1)
        i = 0;
    else
        ungetch(i);

    return (i);
}

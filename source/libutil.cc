/*
 *  File:       libutil.cc
 *  Summary:    Functions that may be missing from some systems
 *
 *  Change History (most recent first):
 *
 *      <1> 2001/Nov/01        BWR     Created
 *
 */

#include "AppHdr.h"
#include "defines.h"
#include "libutil.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#ifdef WIN32CONSOLE
#include <windows.h>
#include <mmsystem.h>
#endif

void play_sound( const char *file )
{
#if defined(WIN32CONSOLE) || defined(WINDOWS)
    // Check whether file exists, is readable, etc.?
    if (file && *file)
        sndPlaySound( file, SND_ASYNC | SND_NODEFAULT );
#else
    UNUSED( file );
#endif
}

// XXX: make regex available here
// Determines whether the pattern specified by 'pattern' matches the given
// text. A pattern is a simple glob, with the traditional * and ? wildcards.
bool pattern_match( const char *pattern, const char *text )
{
    char p, t;
    bool special;

    for (;;)
    {
        p = *pattern++;
        t = *text++;
        special = true;

        if (!p)
            return (t == 0);

        if (p == '\\' && *pattern)
        {
            p       = *pattern++;
            special = false;
        }

        if (p == '*' && special)
        {
            // Try to match exactly at the current text position...
            // Or skip one character in the text and try the wildcard
            // match again. If this is the end of the text, the match has
            // failed.
            return ((!*pattern || pattern_match(pattern, text - 1)) ? true
                    : (t) ? pattern_match(pattern - 1, text) : false);
        }
        else if (!t || (p != t && (p != '?' || !special)))
        {
            break;
        }
    }

    return (false);
}

void get_input_line( char *const buff, int len )
{
    buff[0] = '\0';         // just in case

#if defined(LINUX)
    get_input_line_from_curses( buff, len ); // inplemented in liblinux.cc
#elif defined(MAC) || defined(WIN32CONSOLE)
    getstr( buff, len );        // implemented in libmac.cc
#else
    fgets( buff, len, stdin );  // much safer than gets()
#endif

    buff[ len - 1 ] = '\0';  // just in case

    // Removing white space from the end in order to get rid of any
    // newlines or carriage returns that any of the above might have
    // left there (esp fgets).  -- bwr
    const int end = strlen( buff );
    int i;

    for (i = end - 1; i >= 0; i++)
    {
        if (isspace( buff[i] ))
            buff[i] = '\0';
        else
            break;
    }
}

#ifdef DOS
static int getch_ck()
{
    int c = getch();
    if (!c)
    {
        switch (c = getch())
        {
        case 'O':  return (CK_END);
        case 'P':  return (CK_DOWN);
        case 'I':  return (CK_PGUP);
        case 'H':  return (CK_UP);
        case 'G':  return (CK_HOME);
        case 'K':  return (CK_LEFT);
        case 'Q':  return (CK_PGDN);
        case 'M':  return (CK_RIGHT);
        case 119:  return (CK_CTRL_HOME);
        case 141:  return (CK_CTRL_UP);
        case 132:  return (CK_CTRL_PGUP);
        case 116:  return (CK_CTRL_RIGHT);
        case 118:  return (CK_CTRL_PGDN);
        case 145:  return (CK_CTRL_DOWN);
        case 117:  return (CK_CTRL_END);
        case 115:  return (CK_CTRL_LEFT);
        case 'S':  return (CK_DELETE);
        }
    }

    return (c);
}
#endif

// Hacky wrapper around getch() that returns CK_ codes for keys
// we want to use in cancelable_get_line() and menus.
int c_getch()
{
#if defined(DOS) || defined(LINUX) || defined(WIN32CONSOLE)
    return getch_ck();
#else
    return getch();
#endif
}

// XXX: can't we use readline for this?  at least as an option?
bool cancelable_get_line( char *buf, int len )
{
    if (len <= 0)
        return (false);

    buf[0] = 0;

    char *cur = buf;
    int start = wherex(), line = wherey();
    int length = 0, pos = 0;

    for (;;)
    {
        int ch = c_getch();

        switch (ch)
        {
        case CK_ESCAPE:
            return (false);

        case CK_ENTER:
            buf[length] = 0;
            return (true);

        case CK_DELETE:
            if (pos < length)
            {
                char *c = cur;
                while (c - buf < length)
                {
                    *c = c[1];
                    c++;
                }
                --length;

                gotoxy( start + pos, line );
                buf[length] = 0;
                cprintf( "%s ", cur );
                gotoxy( start + pos, line );
            }
            break;

        case CK_BKSP:
            if (pos)
            {
                --cur;
                char *c = cur;
                while (*c) {
                    *c = c[1];
                    c++;
                }
                --pos;
                --length;

                gotoxy( start + pos, line );
                buf[length] = 0;
                cprintf( "%s ", cur );
                gotoxy( start + pos, line );
            }
            break;

        case CK_LEFT:
            if (pos)
            {
                --pos;
                cur = buf + pos;
                gotoxy( start + pos, line );
            }
            break;

        case CK_RIGHT:
            if (pos < length)
            {
                ++pos;
                cur = buf + pos;
                gotoxy( start + pos, line );
            }
            break;

        case CK_HOME:
            pos = 0;
            cur = buf + pos;
            gotoxy( start, line );
            break;

        case CK_END:
            pos = length;
            cur = buf + pos;
            gotoxy( start + length, line );
            break;

        default:
            if (isprint(ch) && length < len - 1)
            {
                if (pos < length)
                {
                    char *c = buf + length - 1;
                    while (c >= cur)
                    {
                        c[1] = *c;
                        c--;
                    }
                }

                *cur++ = static_cast<char>( ch );
                ++length;
                ++pos;
                putch(ch);

                if (pos < length)
                {
                    buf[length] = 0;
                    cprintf( "%s", cur );
                    gotoxy( start + pos, line );
                }
            }
            break;
        }
    }
}

// The old school way of doing short delays via low level I/O sync.
// Good for systems like old versions of Solaris that don't have usleep.
#ifdef NEED_USLEEP

#include <sys/time.h>
#include <sys/types.h>
#include <sys/unistd.h>

void usleep(unsigned long time)
{
    struct timeval timer;

    timer.tv_sec = (time / 1000000L);
    timer.tv_usec = (time % 1000000L);

    select(0, NULL, NULL, NULL, &timer);
}
#endif

// Not the greatest version of snprintf, but a functional one that's
// a bit safer than raw sprintf().  Note that this doesn't do the
// special behaviour for size == 0, largely because the return value
// in that case varies depending on which standard is being used (SUSv2
// returns an unspecified value < 1, whereas C99 allows str == NULL
// and returns the number of characters that would have been written). -- bwr
#ifdef NEED_SNPRINTF

#include <stdarg.h>

int vsnprintf( char *str, size_t size, const char *format, va_list argp )
{
    char buff[ 10 * size ];  // hopefully enough

    vsprintf( buff, format, argp );
    strncpy( str, buff, size );
    str[ size - 1 ] = '\0';

    int ret = strlen( str );
    if ((unsigned int) ret == size - 1 && strlen( buff ) >= size)
        ret = -1;

    return (ret);
}

int snprintf( char *str, size_t size, const char *format, ... )
{
    va_list argp;
    va_start( argp, format );

    const int ret = vsnprintf( str, size, format, argp );

    va_end( argp );

    return (ret);
}

#endif

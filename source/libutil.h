/*
 *  File:       libutil.h
 *  Summary:    System indepentant functions
 *
 *  Change History (most recent first):
 *
 *      <1>   2001/Nov/01        BWR     Created
 *
 */

#ifndef LIBUTIL_H
#define LIBUTIL_H

// getch() that returns a consistent set of values for all platforms.
int c_getch();

void play_sound(const char *file);

bool pattern_match(const char *pattern, const char *text);

void get_input_line( char *const buff, int len );

// Returns true if user pressed Enter, false if user hit Escape.
bool cancelable_get_line( char *buf, int len );

#ifdef NEED_USLEEP
void usleep( unsigned long time );
#endif

#ifdef NEED_SNPRINTF
#include <stdarg.h>

int vsnprintf( char *str, size_t size, const char *format, va_list argp );
int snprintf( char *str, size_t size, const char *format, ... );
#endif

// Keys that getch() must return for keys Crawl is interested in.
enum KEYS {
    CK_ENTER  = '\r',
    CK_BKSP   = 8,
    CK_ESCAPE = '\x1b',

    // 128 is off-limits because it's the code that's used when running
    CK_DELETE = 129,

    // This sequence of enums should not be rearranged.
    CK_UP,
    CK_DOWN,
    CK_LEFT,
    CK_RIGHT,

    CK_INSERT,

    CK_HOME,
    CK_END,
    CK_CLEAR,

    CK_PGUP,
    CK_PGDN,

    CK_SHIFT_UP,
    CK_SHIFT_DOWN,
    CK_SHIFT_LEFT,
    CK_SHIFT_RIGHT,

    CK_SHIFT_INSERT,

    CK_SHIFT_HOME,
    CK_SHIFT_END,
    CK_SHIFT_CLEAR,

    CK_SHIFT_PGUP,
    CK_SHIFT_PGDN,

    CK_CTRL_UP,
    CK_CTRL_DOWN,
    CK_CTRL_LEFT,
    CK_CTRL_RIGHT,

    CK_CTRL_INSERT,

    CK_CTRL_HOME,
    CK_CTRL_END,
    CK_CTRL_CLEAR,

    CK_CTRL_PGUP,
    CK_CTRL_PGDN
};

#endif

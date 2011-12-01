/*
 *  File:       message.cc
 *  Summary:    Functions used to print messages.
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 *               <2>     5/08/99        JDJ             mpr takes a const char* instead of a char array.
 *               <1>     -/--/--        LRH             Created
 */

#ifndef MESSAGE_H
#define MESSAGE_H

#include <string>

#include "externs.h"

#define MORE_STRING     "-- more --"

struct message_item {
    int                 channel;        // message channel
    int                 param;          // param for channel (god, enchantment)
    std::string         text;           // text of message
};

void mesclr( bool force = false );
void more(void);

void mpr( const char *format, ... );
void mpr( msg_channel_type channel, const char *format, ... );
void mpr( msg_channel_type channel, int param, const char *format, ... );

void replay_messages(void);

void set_colour(char set_message_colour);

bool any_messages(void);

// last updated 13oct2003 {dlb}
/* ***********************************************************************
 * called from: chardump
 * *********************************************************************** */
std::string get_last_messages(int mcount);

#endif

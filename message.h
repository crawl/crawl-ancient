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

void mpr(const char *inf);
void mesclr();
void more();
void mesclr();
void replay_messages();
void set_colour(char set_message_colour);

#endif

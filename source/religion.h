/*
 *  File:       religion.cc
 *  Summary:    Misc religion related functions.
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 *               <1>     -/--/--        LRH             Created
 */


#ifndef RELIGION_H
#define RELIGION_H

#include "enum.h"

void Xom_acts(bool niceness, int sever, bool force_sever);

void simple_god_message( const char *event, int which_deity = GOD_NO_GOD );
char *god_name(int which_god,bool long_name=false); //mv
void god_speaks( int god, const char *mesg );
char god_colour(char god);
void god_pitch(unsigned char which_god);

void gain_piety( int pgn );
void lose_piety( int pgn );
void dec_penance(int val);
void excommunication(void);

bool did_god_conduct( int thing_done, int level );

bool offer_corpse(int corpse);

void pray(void);

void handle_god_time(void);

#endif

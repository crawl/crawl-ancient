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

void dec_penance( int god, int val );
void dec_penance( int val );
void inc_penance( int god, int val );
void inc_penance( int val );
void pray(void);
char *god_name(int which_god);
void Xom_acts(char niceness, int sever, char force_sever);
void naughty(char type_naughty, int naughtiness);
void done_good(char thing_done, int pgain);
void divine_retribution( int god );
void excommunication(void);
void god_speaks( int god, const char * mesg );
void god_pitch(unsigned char which_god);
void offer_corpse(int corpse);
void gain_piety(char pgn);
void lose_piety(char pgn);

#endif

#ifndef RELIGION_H
#define RELIGION_H
/*
 *  File:       religion.cc
 *  Summary:    Misc religion related functions.
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 *               <1>     -/--/--        LRH             Created
 */

void pray(void);
char *god_name(int which_god);
void Xom_acts(char niceness, int sever, char force_sever);
void naughty(char type_naughty, int naughtiness);
void done_good(char thing_done, int pgain);
void excommunication(void);
void god_pitch(unsigned char which_god);
void offer_corpse(int corpse);
void gain_piety(char pgn);
void lose_piety(char pgn);
#endif

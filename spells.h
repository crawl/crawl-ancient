#ifndef SPELLS_H
#define SPELLS_H
/*
 *  File:       spells.cc
 *  Summary:    Some spell related functions.
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 *               <1>     -/--/--        LRH             Created
 */


char miscast_effect(char sp_type, char mag_pow, char mag_fail, char force_effect);
char learn_a_spell(unsigned char splbook, char bitty);
int which_spellbook(void);
void read_book(char book_read);
void which_spell(void);
#endif

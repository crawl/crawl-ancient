/*
 *  File:       spells.cc
 *  Summary:    Some spell related functions.
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 *               <1>     -/--/--        LRH             Created
 */

#ifndef SPELLS_H
#define SPELLS_H

char miscast_effect(char sp_type, char mag_pow, char mag_fail, char force_effect);
bool learn_a_spell(unsigned char splbook, char bitty);
bool which_spellbook(void);
void read_book(char book_read);
void which_spell(void);

#endif

/*
 *  File:       spells0.cc
 *  Summary:    Some spell related functions.
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 *               <1>     -/--/--        LRH             Created
 */


#ifndef SPELLS0_H
#define SPELLS0_H

#include "enum.h"    // just for NUM_SPELL_TYPES {dlb}

// last updated 24may2000 {dlb}
/* ***********************************************************************
 * called from: chardump - spell - spl-book - spells0
 * *********************************************************************** */
int spell_fail(int spell);


// last updated 24may2000 {dlb}
/* ***********************************************************************
 * called from: command - spell - spells3
 * *********************************************************************** */
char spell_list(void);


// last updated 24may2000 {dlb}
/* ***********************************************************************
 * called from: spl-book
 * *********************************************************************** */
bool undead_cannot_memorise(unsigned char spell, unsigned char being);


// last updated 24may2000 {dlb}
/* ***********************************************************************
 * called from: spell - spl-book - spells0
 * *********************************************************************** */
int spell_spec(int spell);


// last updated 24may2000 {dlb}
/* ***********************************************************************
 * called from: spl-book - spells0
 * *********************************************************************** */
void print_slash(bool already);


// last updated 04jun2000 {dlb}
/* ***********************************************************************
 * called from: spell - spells0
 * *********************************************************************** */
int spell_enhancement (unsigned int);


/* ***********************************************************************
 * called from: spell - spells0
 * *********************************************************************** */
//unsigned int set_spellflags (int which_spell );


// last updated 05jun2000 {dlb}
/* ***********************************************************************
 * called from: spell - spells0
 * *********************************************************************** */
int spell_type2skill (unsigned int which_spelltype);


#endif

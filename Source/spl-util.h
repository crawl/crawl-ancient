/*
 *  File:       spl-util.h
 *  Summary:    data handlers for player spell list
 *  Written by: don brodale <dbrodale@bigfootinteractive.com>
 *
 *  Changelog(most recent first):
 *
 *           24jun2000     jmf     simplified structures
 *  <00>     12jun2000     dlb     created after much thought
 */


#ifndef SPLUTIL_H
#define SPLUTIL_H

struct playerspell
{
  int id;
  char *title;
  unsigned int disciplines; //jmf: a bitfield
  unsigned int level;
};


//* * called from: acr
void init_playerspells(void);

// * called from: spell
int spell_hunger(int which_spell);

// * called from: it_use3 - spell - spells3
int spell_mana(int which_spell);

// * called from: chardump - it_use3 - player - spell - spl-book -
// *              spells0 - spells3
int spell_difficulty(int which_spell);

// * called from: chardump - spell - spl-book - spells0
bool spell_typematch(int which_spell, unsigned int which_discipline);
unsigned int spell_type( int which_spell ); //jmf: simplification of above
int count_bits( unsigned int bits );

// * called from: chardump - command - debug - spl-book - spells0
const char *spell_title(int which_spell);

//int spell_restriction(int which_spell, int which_restriction);

#endif

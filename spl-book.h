/*
 *  File:       spl-book.h
 *  Summary:    Some spellbook related functions.
 *  Written by: Josh Fishman
 *
 *  Change History (most recent first):
 *
 * 22mar2000   jmf   Created
 */


#ifndef SPLBOOK_H
#define SPLBOOK_H

// used in dungeon.cc, it_use3.cc, spl-book.cc, spl-book.h - {dlb}
#define SPELLBOOK_SIZE 9
// used in spl-book.cc, spl-book.h - {dlb}
#define NUMBER_SPELLBOOKS 59


// updated 24may2000 {dlb}
/* ***********************************************************************
 * called from: dungeon - effects - shopping
 * *********************************************************************** */
char book_rarity(unsigned char which_book);


// updated 24may2000 {dlb}
/* ***********************************************************************
 * called from: it_use3 - item_use - spl-book
 * *********************************************************************** */
bool learn_a_spell(unsigned int splbook, int bitty);


// updated 24may2000 {dlb}
/* ***********************************************************************
 * called from: it_use3 - item_use - spl-book
 * *********************************************************************** */
void read_book(unsigned int book_read);


// updated 24may2000 {dlb}
/* ***********************************************************************
 * called from: chardump - spl-book - spells0
 * *********************************************************************** */
char *spelltype_name(unsigned int which_spelltype);


// updated 24may2000 {dlb}
/* ***********************************************************************
 * called from: dungeon - spl-book
 * *********************************************************************** */
void spellbook_template(int sbook_type, int sbtemplate_pass[SPELLBOOK_SIZE]);    // was foo[10] {dlb}


// updated 24may2000 {dlb}
/* ***********************************************************************
 * called from: acr
 * *********************************************************************** */
void which_spell(void);


// updated 24may2000 {dlb}
/* ***********************************************************************
 * called from: it_use3 - item_use - spl-book
 * *********************************************************************** */
int which_spell_in_book(int sbook_type, int spl);


#endif

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

int spell_hunger(int spell_value, unsigned char spell);
int which_spell_in_book(int sbook_type, int spl);
char spellbook_contents(unsigned char plus, unsigned char type);
void spell_name(unsigned char spell, char spln[60]);
char spell_value(unsigned char spell);
bool spell_type(unsigned char spell, unsigned char typy);
int spell_spec(int spell, int power);
char spell_fail(unsigned char spell);
void print_slash(char already);
char spell_list(void);
char undead_can_memorise(unsigned char spell);

#endif

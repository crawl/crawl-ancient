/*
 *  File:       spell.cc
 *  Summary:    Spell casting functions.
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 *               <1>     -/--/--        LRH             Created
 */


#ifndef SPELL_H
#define SPELL_H


// last updaetd 12may2000 {dlb}
/* ***********************************************************************
 * called from: it_use3 - spell
 * *********************************************************************** */
void exercise_spell(int spell_ex, bool spc, bool divide);


// last updaetd 12may2000 {dlb}
/* ***********************************************************************
 * called from: acr
 * *********************************************************************** */
void cast_a_spell(void);


// last updaetd 12may2000 {dlb}
/* ***********************************************************************
 * called from: ability - debug - it_use3 - spell
 * *********************************************************************** */
bool your_spells(int spc2, int powc, bool allow_fail);


#endif

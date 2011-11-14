/*
 *  File:       it_use2.cc
 *  Summary:    Functions for using wands, potions, and weapon/armor removal.
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 *               <1>     -/--/--        LRH             Created
 */


#ifndef IT_USE2_H
#define IT_USE2_H


#include "externs.h"


/* ***********************************************************************
 * called from: ability - beam - decks - item_use - misc - religion -
 *              spell - spells - spells1
 * *********************************************************************** */
void potion_effect(char pot_eff, int pow);


/* ***********************************************************************
 * called from: item_use
 * *********************************************************************** */
void unuse_randart(unsigned char unw);


/* ***********************************************************************
 * called from: item_use - transfor
 * *********************************************************************** */
void unwear_armour(char unw);


/* ***********************************************************************
 * called from: decks - it_use3 - item_use - items - spells3 - transfor
 * *********************************************************************** */
void unwield_item(char unw);


/* ***********************************************************************
 * called from: ability - it_use3 - item_use - spell - spells1 - spells3
 * *********************************************************************** */
void zapping(char ztype, int power, struct bolt &pbolt);


#endif

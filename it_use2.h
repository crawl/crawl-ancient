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

void zapping(char ztype, int power, struct bolt beams[1]);
void potion_effect(char pot_eff, int pow);
void unwield_item(char unw);
void unwear_armour(char unw);
void skill_manual(char sc_read_2);
void tome_of_power(char sc_read_2);
void unuse_randart(unsigned char unw);

#endif

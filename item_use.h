#ifndef ITEM_USE_H
#define ITEM_USE_H
/*
 *  File:       item_use.cc
 *  Summary:    Functions for making use of inventory items.
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 *              <2>             5/26/99         JDJ             Exposed armour_prompt. takeoff_armour takes an index argument.
 *              <1>             -/--/--         LRH             Created
 */

#include <string>


void wield_weapon(char auto_wield);
void wear_armour();

bool armour_prompt(const string& mesg, int* index);
bool takeoff_armour(int index);

void throw_anything();
void shoot_thing();
void puton_ring();
void remove_ring();

void zap_wand();
void eat_food();
void drink();

void read_scroll();
void original_name();

#endif  // ITEM_USE_H

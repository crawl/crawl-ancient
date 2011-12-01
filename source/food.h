/*
 *  File:       food.cc
 *  Summary:    Functions for eating and butchering.
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 *               <1>     -/--/--        LRH             Created
 */


#ifndef FOOD_H
#define FOOD_H

int find_butcher_knife(void);
bool butchery(void);
void eat_food(void);

void make_hungry(int hunger_amount, bool suppress_msg);
void lessen_hunger(int statiated_amount, bool suppress_msg);
void set_hunger(int new_hunger_level, bool suppress_msg);

void weapon_switch( int targ );

#endif

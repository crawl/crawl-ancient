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

void food_change(void);
void eaten(int food_eat_2, int food_eat_3);
void eat_meat(int fod_eat_2);
char eat_from_floor(void);
char butchery(void);
bool is_carnivore(void);

#endif

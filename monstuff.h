/*
 *  File:       monstuff.cc
 *  Summary:    Misc monster related functions.
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 *               <1>     -/--/--        LRH             Created
 */

#ifndef MONSTUFF_H
#define MONSTUFF_H

char curse_an_item(char which, char power);
void monster_blink(int mn);
char random_near_space(int passed[2]);
bool monster_regenerates(int monster_type);

int distance(int x1, int x2, int y1, int y2);
void swap_places(int swap);
void print_wounds(int wounded);
int wounded_damaged(int wound_class);
void monster(void);
#endif

/*
 *  File:       it_use3.cc
 *  Summary:    Functions for using some of the wackier inventory items.
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 *               <1>     -/--/--        LRH             Created
 */


#ifndef IT_USE3_H
#define IT_USE3_H

void skill_manual( int manual );
void tome_of_power( int tome );

bool evoke_wielded(void);

void special_wielded(void);

bool reaching_weapon_attack( const int x = -1, const int y = -1 );

#endif

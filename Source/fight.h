/*
 *  File:       fight.cc
 *  Summary:    Functions used during combat.
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 *               <1>     -/--/--        LRH             Created
 */


#ifndef FIGHT_H
#define FIGHT_H


#include "externs.h"

// added Sept 18, 2000 -- bwr
/* ***********************************************************************
 * called from: item_use.cc
 * *********************************************************************** */
int effective_stat_bonus( void );

// added Sept 18, 2000 -- bwr
/* ***********************************************************************
 * called from: describe.cc
 * *********************************************************************** */
int weapon_str_weight( int wpn_class, int wpn_type );


// last updated: 08jun2000 {dlb}
/* ***********************************************************************
 * called from: bang - beam - effects - fight - misc - monstuff - mstuff2 -
 *              spells1 - spells2 - spells3 - spells4
 * *********************************************************************** */
void monster_die(struct monsters *monster, char killer, int i);

// last updated: 17dec2000 {gdl}
/* ***********************************************************************
 * called from: monstuff - fight
 * *********************************************************************** */
void monster_cleanup(struct monsters *monster);

// last updated: 08jun2000 {dlb}
/* ***********************************************************************
 * called from: acr - it_use3
 * *********************************************************************** */
void you_attack(int monster_attacked, bool unarmed_attacks);


// last updated: 08jun2000 {dlb}
/* ***********************************************************************
 * called from: monstuff
 * *********************************************************************** */
void monster_attack(int monster_attacking);


// last updated: 08jun2000 {dlb}
/* ***********************************************************************
 * called from: monstuff
 * *********************************************************************** */
bool monsters_fight(int monster_attacking, int monster_attacked);


// last updated: 08jun2000 {dlb}
/* ***********************************************************************
 * called from: monstuff
 * *********************************************************************** */
bool jelly_divide(struct monsters *parent);


// last updated: 08jun2000 {dlb}
/* ***********************************************************************
 * called from: fight - item_use - items - spell
 * *********************************************************************** */
void alert(void);


// last updated: 08jun2000 {dlb}
/* ***********************************************************************
 * called from: beam - effects - monstuff
 * *********************************************************************** */
bool monster_polymorph(struct monsters *monster, int targetc, int power);


#endif

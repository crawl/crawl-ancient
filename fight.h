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

void monster_die(int monster_killed, char killer, int i);
void you_attack(int monster_attacked, bool unarmed_attacks);
void monster_attack(int monster_attacking);
char monsters_fight(int monster_attacking, int monster_attacked);
bool jelly_divide(int jel);
void alert(void);
void monster_polymorph(unsigned char monsc, unsigned char targetc, int power);
void monster_drop_ething(int monster_killed);

#endif

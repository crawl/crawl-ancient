/*
 *  File:       spells1.cc
 *  Summary:    Implementations of some additional spells.
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 *               <1>     -/--/--        LRH             Created
 */


#ifndef SPELLS1_H
#define SPELLS1_H


#include "externs.h"
#include "direct.h"

bool something_there( int x, int y );

void big_cloud(char clouds, char cl_x, char cl_y, int pow, int size);

bool cast_sure_blade(int power);
int  cast_healing(int power);
void cast_big_cloud( int pow, struct bolt &beam, int cloud );
void cast_confusing_touch(int power);
void cast_cure_poison(int mabil);
int  allowed_deaths_door_hp(void);
void cast_deaths_door(int pow);
void cast_fire_storm( int powc, struct bolt &beam );
void cast_chain_lightning( int powc );
bool cast_revivification(int power);
void cast_berserk(void);
void cast_ring_of_flames(int power);
int  cast_small_cloud( int pow, struct bolt &beam, int cloud );
void extension(int pow);
int  fireball(int power);
void stinking_cloud( int pow, struct bolt &beam );
void abjuration(int pow);

void cast_fly(int power);
void cast_insulation(int power);
void cast_regen(int pow);
void cast_resist_poison(int power);
void cast_swiftness(int power);
void cast_teleport_control(int power);
void deflection(int pow);
void ice_armour(int pow, bool extending);
void missile_prot(int pow);
void stone_scales(int pow);
void identify( int power );
void purification(void);

void antimagic(void);
void manage_fire_shield(void);

#endif

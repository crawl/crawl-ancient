/*
 *  File:       spells4.cc
 *  Summary:    Yet More Spell Function Declarations
 *  Written by: Josh Fishman
 *
 *  Change History (most recent first):
 * <2> 12jul2000  jmf  Fixed random (undocumented) damage
 * <1> 07jan2000  jmf  Created
 */


#ifndef SPELLS4_H
#define SPELLS4_H

#include "externs.h"

const char *your_hand(bool plural);
bool backlight_monsters(int x, int y, int pow, int garbage);
int make_a_normal_cloud(int x, int y, int pow, int ctype);
int disperse_monsters(int x, int y, int pow, int message);

void cast_condensation_shield(int pow);
void cast_detect_secret_doors(int pow);
void cast_discharge(int pow);
int  cast_evaporate(int pow);
int  cast_fulsome_distillation(int powc);
void cast_forescry(int pow);
int  cast_fragmentation(int powc);
int  cast_twist( int powc, struct bolt &beam  );
void cast_swap(int powc);
int  cast_apportation(int powc);
void cast_glamour(int pow);
void cast_ignite_poison(int pow);
void cast_intoxicate(int pow);
void cast_mass_sleep(int pow);
void cast_passwall(int pow);
void cast_rotting(int pow);
void cast_sandblast( int powc, struct bolt &beam );
void cast_see_invisible(int pow);

void cast_shatter( int pow );
void cast_silence( int pow );
int  cast_quiet( int pow, const struct bolt &beam );

void cast_sticks_to_snakes(int pow);
bool cast_summon_butterflies(int pow);
void cast_summon_dragon(int pow);
void cast_conjure_ball_lightning(int pow);
void cast_summon_large_mammal(int pow);
void cast_tame_beasts(int pow);
void cast_dispersal(int pow);
void cast_snake_charm(int pow);
void cast_stoneskin(int pow);

int distort_being( int mid, int killer, bool apply_damage = false );

#endif

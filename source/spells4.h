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


bool backlight_monsters(char x, char y, int pow, int garbage);
int torment_monsters(char x, char y, int pow, int message);
bool monster_has_enchantment( int mon, int ench );
int enchant_monster( int mon, int ench );

int make_a_normal_cloud(char x, char y, int pow, int ctype);
void apply_area_cloud( int (*func)(char,char,int,int), char x, char y, int pow, int number, int ctype);
void apply_area_within_radius(int (*func)(char,char,int,int), char x, char y, int pow, int radius, int ctype);

void cast_bend(int pow);
void cast_condensation_shield(int pow);
void cast_detect_secret_doors(int pow);
void cast_discharge(int pow);
void cast_evaporate(int pow);
void cast_forescry(int pow);
void cast_fragmentation(int powc);
void cast_twist(int powc);
void cast_far_strike(int powc);
void cast_swap(int powc);
void cast_apportation(int powc);

#ifdef USE_ELVISH_GLAMOUR_ABILITY
void cast_glamour(int pow);
#endif

void cast_ignite_poison(int pow);
void cast_intoxicate(int pow);
void cast_mass_sleep(int pow);
void cast_passwall(int pow);
void cast_rotting(int pow);
void cast_sandblast(int powc);
void cast_see_invisible(int pow);

void cast_shatter(int pow);
void cast_silence(int pow);
void cast_sticks_to_snakes(int pow);
void cast_summon_butterflies(int pow);
void cast_summon_dragon(int pow);
void cast_conjure_ball_lightning(int pow);
void cast_summon_large_mammal(int pow);
void cast_tame_beasts(int pow);
void cast_dispersal(int pow);
void cast_snake_charm(int pow);
void cast_stoneskin(int pow);

void cast_shuggoth_seed(int powc);
void make_shuggoth(char x, char y, int hp);

void cast_semi_controlled_blink(int pow);


#endif

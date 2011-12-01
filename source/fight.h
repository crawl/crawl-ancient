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

#define MULTIBLOCK_FACTOR               10

#include "externs.h"

bool test_hit( int to_hit, int ev, int &margin );
bool test_hit( int to_hit, int ev );

bool test_block( int shield, int attack );

bool check_monk_style( void );
bool check_hand_and_half_style( bool ignore_weight = false );
bool check_using_both_ends( void ); // note: check is_double_ended first!

int your_shield_blocks_penalty( void );

int your_melee_to_hit( struct monsters *targ, int extra_hit, int max,
                       bool hand_and_half, bool ignore_hidden );

int your_melee_speed( bool hand_and_half, bool speed_brand, bool ignore_blocks );

int heavy_weapon_penalty( void );
int heavy_shield_penalty( void );
int heavy_armour_penalty( void );

int effective_stat_bonus( int wepType = -1 );

int apply_player_armour( int damage_size, int damage_taken, bool energy = false );

void you_attack( int monster_attacked, bool unarmed_attacks );
bool monster_attack( int att_id, int def_id );
// void monster_attack(int monster_attacking);
// bool monsters_fight(int monster_attacking, int monster_attacked);


#endif

/*
 *  File:       effects.cc
 *  Summary:    Misc stuff.
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 *               <1>     -/--/--        LRH             Created
 */


#ifndef EFFECTS_H
#define EFFECTS_H

#include "externs.h"

void banished( unsigned char gate_type );

int forget_spells(void);

stat_type random_stat( unsigned int str_weight = 10, unsigned int int_weight = 10, unsigned int dex_weight = 10 );

bool lose_stat( stat_type which_stat, int stat_loss, bool force = false);
bool restore_stat( stat_type which_stat, bool suppress_msg = false );
void modify_stat( stat_type which_stat, int amount, bool suppress_msg );

void random_uselessness( int scroll_id = -1 );

bool acquirement( unsigned char force_class, bool god_gift = false );

bool recharge_wand( void );

void direct_effect( struct bolt &pbolt );
void mons_direct_effect( struct bolt &pbolt, int i );

void yell( void );

void torment( int tx, int ty );
void dispel_undead_area( int tx, int ty, int pow, int caster_id );

#endif

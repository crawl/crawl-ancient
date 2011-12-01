/*
 *  File:       beam.cc
 *  Summary:    Functions related to ranged attacks.
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 *               <1>     -/--/--        LRH             Created
 */


#ifndef BEAM_H
#define BEAM_H


#include "externs.h"

dice_def calc_dice( int num_dice, int max_damage );

void zap_animation(int colour, struct monsters *mon = NULL, bool force = false);

void zapping( char ztype, int power, struct bolt &pbolt );
void fire_beam( struct bolt &pbolt, item_def *const item = NULL );

bool nasty_beam( struct monsters *mon, struct bolt &beam );

void explosion( struct bolt &pbolt, bool hole_in_the_middle = false );

int mons_adjust_flavoured( struct monsters *monster, struct bolt &pbolt,
                           int hurted, bool doFlavouredEffects = true );

bool mass_enchantment( enchant_type ench, int pow, int origin,
                       bool show_resists = true );

int mons_ench_f2( struct monsters *monster, struct bolt &pbolt );

void poison_monster( struct monsters *monster, bool fromPlayer, int levels = 1,
                     bool force = false );

bool check_line_of_sight( int sx, int sy, int tx, int ty );
void fire_tracer( struct monsters *monster, struct bolt &pbolt );

void mimic_alert( struct monsters *mimic );

#endif

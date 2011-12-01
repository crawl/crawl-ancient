/*
 *  File:       ouch.cc
 *  Summary:    Functions used when Bad Things happen to the player.
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 *     <2>     9/29/99     BCR     Added the DEATH_NAME_LENGTH define
 *                                 It determines how many characters of
 *                                 a long player name are printed in
 *                                 the score list.
 *     <1>     -/--/--     LRH     Created
 */


#ifndef OUCH_H
#define OUCH_H

#include "externs.h"
#include "enum.h"

#define DEATH_NAME_LENGTH 10

int check_your_resists( int hurted, beam_type flavour, bool warn_only = false );

void splash_with_acid( int acid_strength );
void item_corrode( item_def &item );

void weapon_acid(char acid_strength);

void expose_player_to_element( beam_type flavour, int strength = 0 );
void expose_floor_to_element( beam_type flavour, int x, int y );

void ouch(int dam, int death_source, char death_type, const char *aux = 0);

void lose_level(void);

void drain_exp(void);

#endif

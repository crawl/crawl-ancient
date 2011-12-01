/*
 *  File:       mstuff2.h
 *  Summary:    Misc monster related functions.
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 *               <1>     4/24/99        JDJ             mons_spells returns an
 *                                                      SBeam instead of using
 *                                                      func_pass.
 */


#ifndef MSTUFF2_H
#define MSTUFF2_H


#include <string>
#include "externs.h"

void mons_spells( int spell_cast, int power, struct bolt &beam );

void setup_dragon(struct monsters *monster, struct bolt &pbolt);

void mons_cast( struct monsters *monster, struct bolt &pbolt, int spell_cast );

void setup_mons_cast(struct monsters *monster, struct bolt &pbolt, int spell_cast);

bool mons_throw( struct monsters *mon, struct bolt &pbolt, int ammo_id );

void setup_generic_throw(struct monsters *monster, struct bolt &pbolt);

void mons_trap(struct monsters *monster);

void monster_teleport( struct monsters *monster, int delay = 0,
                       bool silent = false, bool away = false );

void spore_goes_pop(struct monsters *monster);

#endif

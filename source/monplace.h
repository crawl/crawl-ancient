/*
 *  File:       monsplace.cc
 *  Summary:    Functions used when placing monsters in the dungeon.
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 *               <1>     -/--/--        LRH             Created
 */


#ifndef MONPLACE_H
#define MONPLACE_H

#include "defines.h"
#include "enum.h"
#include "FixVec.h"

/* ***********************************************************************
 * Usage:
 * mclass       WANDERING_MONSTER, RANDOM_MONSTER, or monster type
 * behaviour    standard behaviours (BEH_ENSLAVED, etc)
 * target       MHITYOU, MHITNOT, or monster id
 * extra        various things like skeleton/zombie types, colours, etc
 * summoned     monster is summoned?
 * px           placement x
 * py           placement y
 * level_type   LEVEL_DUNGEON, LEVEL_ABYSS, LEVEL_PANDEMONIUM.
 *              LEVEL_DUNGEON will generate appropriate power monsters
 * proximity    0 = no extra restrictions on monster placement
 *              1 = try to place the monster near the player
 *              2 = don't place the monster near the player
 *              3 = place the monster near stairs (regardless of player pos)
 * power        level of monster
 * *********************************************************************** */
int mons_place( int mclass, beh_type behaviour, int target, bool summoned,
                int px, int py, int level_type = LEVEL_DUNGEON,
                int proximity = PROX_ANYWHERE, int power = -1, int extra = 250 );

int create_monster( int cls, beh_type beha = BEH_HOSTILE, int dur = 0,
                    int cr_x = -1, int cr_y = -1, int hitting = MHITYOU,
                    int zsec = 250, bool force_pos = false, int power = -1 );

// simple front end for the common case of summoning around the player
int simple_summon( int cls, int dur, bool hostile );

bool empty_surrounds( int emx, int emy, unsigned char spc_wanted,
                      int radius, bool allow_centre, bool check_los,
                      FixedVector<char, 2>& empty );

monster_type rand_imp( void );
monster_type rand_demon( demon_class_type demon_class = DEMON_RANDOM );
monster_type rand_dragon( dragon_class_type type );

/* ***********************************************************************
 * This isn't really meant to be a public function.  It is a low level
 * monster placement function used by dungeon building routines and
 * mons_place().  If you need to put a monster somewhere,  use mons_place().
 * Summoned creatures can be created with create_monster().
 * *********************************************************************** */
bool place_monster( int &id, int mclass, int power, beh_type behaviour,
                    int target, bool summoned, int px, int py, bool allow_bands,
                    int proximity = PROX_ANYWHERE, int extra = 250 );

int place_unique_monsters( bool just_one, int prox );

#endif  // MONPLACE_H

/*
 *  File:       dungeon.cc
 *  Summary:    Functions used when building new levels.
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 *               <1>     -/--/--        LRH             Created
 */


#ifndef DUNGEON_H
#define DUNGEON_H

#include "FixVec.h"


// last updated 12may2000 {dlb}
/* ***********************************************************************
 * called from: files
 * *********************************************************************** */
int builder(unsigned int lev_numb, char level_type);


// last updated 12may2000 {dlb}
/* ***********************************************************************
 * called from: abyss - dungeon - monplace
 * *********************************************************************** */
int place_monster(unsigned char plus_seventy, int typed, bool is_summoning,
                  int px, int py, char behaviour, int hitting,
                  bool allow_bands, int many_many, FixedVector<int, 2>& passed);


// last updated 12may2000 {dlb}
/* ***********************************************************************
 * called from: abyss - debug - dungeon - effects - religion - spells4
 * *********************************************************************** */
int items(unsigned char allow_uniques, unsigned char force_class,
          unsigned char force_type, int force_place, int many_many,
          int force_spec);


#endif

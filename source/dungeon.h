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
 * called from: abyss - debug - dungeon - effects - religion - spells4
 * *********************************************************************** */
int items(unsigned char allow_uniques, unsigned char force_class,
          unsigned char force_type, int force_place, int many_many,
          int force_spec);

// last updated 13mar2001 {gdl}
/* ***********************************************************************
 * called from: dungeon monplace
 * *********************************************************************** */
void give_item(int mid);


// last updated 13mar2001 {gdl}
/* ***********************************************************************
 * called from: dungeon monplace
 * *********************************************************************** */
void define_zombie(int mid, char not_zombsize, int ztype, int cs);

#endif

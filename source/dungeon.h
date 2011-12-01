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
#include "externs.h"

#define MAKE_GOOD_ITEM          351

void item_colour( item_def &item );
void arm_trap( int trap_id, int amount = -1 );

void builder( int level_number, int level_type );

// used for wizard shop generation in acr.cc
void place_spec_shop( int level_number, int shop_x, int shop_y, shop_type force_type );

int make_item( int allow_uniques, int force_class, int force_type,
               bool dont_place, int item_level, int item_race );

void give_item(int mid, int level_number);

void define_zombie(int mid, int ztype, int cs, int power);

#endif

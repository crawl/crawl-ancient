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

int builder(unsigned int lev_numb, char level_type);


int place_monster(
                     unsigned char plus_seventy,
     int typed, int type_place, int px, int py, char behaviour, int hitting,
                     char allow_bands,
                     int many_many, int passed[2]
);


void spellbook_template(int sbook_type, int func_pass[10]);


int items(
             unsigned char allow_uniques,
             int force_class,
             int force_type,
             int force_place,
             int many_many,
             int force_spec
);
#endif

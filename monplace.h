#ifndef MONPLACE_H
#define MONPLACE_H
/*
 *  File:       monsplace.cc
 *  Summary:    Functions used when placing monsters in the dungeon.
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 *               <1>     -/--/--        LRH             Created
 */


int mons_place(int typed, int type_place, int px, int py, char behaviour, int hitting, unsigned char plus_seventy, int lev_mons);
int create_monster(int cls, int dur, int beha, int cr_x, int cr_y, int hitting, int zsec);
char empty_surrounds(int emx, int emy, unsigned char spc_wanted, char allow_centre, char empty[2]);
int summon_any_demon(char demon_class);
#endif

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


// last updated 12may2000 {dlb}
/* ***********************************************************************
 * called from: acr - levels - monplace - spells1
 * *********************************************************************** */
int mons_place(int typed, bool is_summoning, int px, int py, char behaviour, int hitting, unsigned char plus_seventy, int lev_mons);


// last updated 12may2000 {dlb}
/* ***********************************************************************
 * called from: acr - debug - decks - effects - fight - it_use3 - item_use -
 *              items - monstuff - mstuff2 - religion - spell - spells -
 *              spells2 - spells3 - spells4
 * *********************************************************************** */
int create_monster(int cls, int dur, int beha, int cr_x, int cr_y, int hitting, int zsec);


// last updated 12may2000 {dlb}
/* ***********************************************************************
 * called from: misc - monplace - spells3
 * *********************************************************************** */
bool empty_surrounds(int emx, int emy, unsigned char spc_wanted, bool allow_centre, char empty[2]);


// last updated 12may2000 {dlb}
/* ***********************************************************************
 * called from: ability - acr - items - maps - mstuff2 - spell - spells
 * *********************************************************************** */
int summon_any_demon(char demon_class);


#endif

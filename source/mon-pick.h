/*
 *  File:       mon-pick.h
 *  Summary:    Functions used to help determine which monsters should appear.
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 *               <1>     -/--/--        LRH             Created
 */


#ifndef MONPICK_H
#define MONPICK_H

int  mons_rarity( int mcls );
int  mons_level( int mcls );
bool mons_abyss( int mcls );
int  mons_rare_abyss( int mcls );
int  branch_depth( int branch );
bool mons_pan( int mcls );

#endif

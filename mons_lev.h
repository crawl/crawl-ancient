/*
 *  File:       mons_lev.cc
 *  Summary:    Functions used to help determine which monsters should appear.
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 *               <1>     -/--/--        LRH             Created
 */

#ifndef MONS_LEV_H
#define MONS_LEV_H

int mons_rarity(int mcls);
int mons_level(int mcls);

int mons_level_gehenna(int mcls);
int mons_level_cocytus(int mcls);
int mons_level_dis(int mcls);
int mons_level_tartarus(int mcls);
int mons_level_abyss(int mcls);

int mons_rare_gehenna(int mcls);
int mons_rare_cocytus(int mcls);
int mons_rare_dis(int mcls);
int mons_rare_tartarus(int mcls);
int mons_rare_abyss(int mcls);
int branch_depth(int branch);
#endif

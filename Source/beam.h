/*
 *  File:       beam.cc
 *  Summary:    Functions related to ranged attacks.
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 *               <1>     -/--/--        LRH             Created
 */


#ifndef BEAM_H
#define BEAM_H


#include "externs.h"


/* ***********************************************************************
 * called from: it_use2 - item_use - monstuff - mstuff2 - spells1
 * *********************************************************************** */
void missile(struct bolt &pbolt, int throw_2);


/* ***********************************************************************
 * called from: bang - effects - spells2
 * *********************************************************************** */
int check_mons_resists(struct monsters *monster, struct bolt &pbolt, int hurted);


/* ***********************************************************************
 * called from: bang - it_use2 - monstuff - mstuff2
 * *********************************************************************** */
void beam(struct bolt &pbolt);


/* ***********************************************************************
 * called from: effects
 * *********************************************************************** */
bool check_mons_magres(struct monsters *monster, int pow);


/* ***********************************************************************
 * called from: ability - item_use - spell
 * *********************************************************************** */
void mass_enchantment(int wh_enchant, int pow);


/* ***********************************************************************
 * called from: fight - monstuff - mstuff2
 * *********************************************************************** */
bool mons_ench_f2(struct monsters *monster, struct bolt &pbolt);


/* ***********************************************************************
 * called from: fight - monstuff - spells2
 * *********************************************************************** */
void poison_monster(struct monsters *monster, bool fromPlayer);


/* ***********************************************************************
 * called from: fight - monstuff - spells - spells1 - spells2
 * *********************************************************************** */
void place_cloud(unsigned char cl_type, unsigned char ctarget_x, unsigned char ctarget_y, unsigned char cl_range);


/* ***********************************************************************
 * called from: monstuff
 * *********************************************************************** */
void tracer_f(struct monsters *monster, struct bolt &pbolt);


/* ***********************************************************************
 * called from: bang - monstuff
 * *********************************************************************** */
void mimic_alert(struct monsters *mimic);


#endif

/*
 *  File:       bang.cc
 *  Summary:    Functions related to explosions.
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 *               <1>     -/--/--        LRH             Created
 */


#ifndef BANG_H
#define BANG_H


#include "externs.h"


// last updated 12may2000 {dlb}
/* ***********************************************************************
 * called from: beam
 * *********************************************************************** */
void explosion1(struct bolt &pbolt);


// last updated 12may2000 {dlb}
/* ***********************************************************************
 * called from: ability - bang - it_use3 - item_use - mstuff2 - religion -
 *              spells - spells4
 * *********************************************************************** */
void explosion(bool is5x5, struct bolt &pbolt);


#endif

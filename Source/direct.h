/*
 *  File:       direct.cc
 *  Summary:    Functions used when picking squares.
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 *               <1>     -/--/--        LRH             Created
 */


#ifndef DIRECT_H
#define DIRECT_H


#include "externs.h"


// last updated 12may2000 {dlb}
/* ***********************************************************************
 * called from: acr - debug - effects - it_use3 - item_use - spells1 -
 *              spells2 - spells3 - spells4
 * *********************************************************************** */

void direction(char rnge, struct dist &moves);

// last updated 12may2000 {dlb}
/* ***********************************************************************
 * called from: acr - direct
 * *********************************************************************** */
int look_around(struct dist &moves);


#endif

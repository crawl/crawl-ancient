#ifndef DIRECT_H
#define DIRECT_H
/*
 *  File:       direct.cc
 *  Summary:    Functions used when picking squares.
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 *               <1>     -/--/--        LRH             Created
 */

#include "externs.h"

void direction(char rnge, struct dist moves[1]);
int dir_cursor(char rng);
int look_around(struct dist moves[1]);
#endif

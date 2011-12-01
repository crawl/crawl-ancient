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
#include "enum.h"

#define STD_DIRECTION_PROMPT    "Which direction ([*+-] to target)? "

#define DIR_NONE    0
#define DIR_TARGET  1
#define DIR_DIR     2
#define DIR_GRID    3

void direction( struct dist &moves, int restriction = DIR_NONE,
                int in_mode = TARG_ANY );

void look_around( struct dist &moves, bool justLooking, int first_move = -1,
                  int in_mode = TARG_ANY, int toggle_mode = TARG_ENEMY );

#endif

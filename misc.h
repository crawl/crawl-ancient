/*
 *  File:       misc.cc
 *  Summary:    Misc functions.
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 *               <1>     -/--/--        LRH             Created
 */
#ifndef MISC_H
#define MISC_H

#include "externs.h"

void destroy_item(int dest);
void place_chunks(int mcls, unsigned char rot_status, unsigned char chx, unsigned char chy, unsigned char ch_col);
char search_around(void);
void in_a_cloud(void);
void up_stairs(void);
void down_stairs(char remove_stairs, int old_level);
void dart_trap(int trap_known, int trapped, struct bolt beam[1]);
void itrap(struct bolt beam[1], int trapped);
void cull_items(void);
void handle_traps(char trt, int i, char trap_known);
void disarm_trap(struct dist disa[1]);
int manage_clouds(void);
void weird_writing(char stringy[40]);
void fall_into_a_pool(char place, unsigned char grype);
void new_level(void);
void weird_colours(unsigned char coll, char wc[30]);
bool go_berserk(void);

#endif

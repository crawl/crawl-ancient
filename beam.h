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

void missile(struct bolt beam[1], int throw_2);
int check_mons_resists(struct bolt beam[1], int o, int hurted);
void beam(struct bolt beam[1]);
int check_mons_magres(int mn, int pow);
void mass_enchantment(int wh_enchant, int pow);
int mons_ench_f2(int o, char is_near, int func_pass[10], struct bolt beam[1]);
void poison_monster(int mn, char source);
void sticky_flame_monster(int mn, char source, int power);
void place_cloud(unsigned char cl_type, unsigned char ctarget_x, unsigned char ctarget_y, unsigned char cl_range);
void tracer_f(int i, struct bolt beem[1]);
void mimic_alert(int mimic);

#endif

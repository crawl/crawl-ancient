/*
 *  File:       effects.cc
 *  Summary:    Misc stuff.
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 *               <1>     -/--/--        LRH             Created
 */
#ifndef EFFECTS_H
#define EFFECTS_H

#include "externs.h"

void torment(void);
void banished(unsigned char gate_type);
char forget_spell(void);
char lose_stat(char force_stat, char stat_loss);
void random_uselessness(unsigned char ru, unsigned char sc_read_2);
void acquirement(unsigned char force_class);
char recharge_wand(void);
void direct_effect(struct bolt beam[1]);
void mons_direct_effect(struct bolt beam[1], int i);
void yell(void);

#endif

/*
 *  File:       mstuff2.h
 *  Summary:    Misc monster related functions.
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 *               <1>     4/24/99        JDJ             mons_spells returns an SBeam instead of using func_pass.
 */
#ifndef MSTUFF2_H
#define MSTUFF2_H

#include <string>
#include "externs.h"


struct SBeam
{
    string name;
    int colour;
    int range;
    int damage;
    int hit;
    int type;
    int flavour;
    int thrown;
    bool isBeam;
};

/*
   beam_colour = func_pass [0];
   beam_range = func_pass [1];
   beam_damage = func_pass [2];
   beam_hit = func_pass [3];
   beam_type = func_pass [4];
   beam_flavour = func_pass [5];
   thing_thrown = func_pass [6];
 */

void spore_goes_pop(int i);
void mons_throw(int i, struct bolt beem[1], int hand_used);
void dragon(int i, struct bolt beem[1]);
void monster_teleport(char monstel, char instan);
void mons_cast(int i, struct bolt beem[1], int spell_cast);
void mons_trap(int i);
SBeam mons_spells(char spell_cast, int power);

#endif // MSTUFF2_H

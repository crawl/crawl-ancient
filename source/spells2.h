/*
 *  File:       spells2.cc
 *  Summary:    Implementations of some additional spells.
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 *               <1>     -/--/--        LRH             Created
 */

#ifndef SPELLS2_H
#define SPELLS2_H

#include "enum.h"

bool brand_weapon(int which_brand, int power);

bool animate_a_corpse( int axps, int ayps, beh_type corps_beh, int corps_hit,
                       int anim_type );

int animate_dead( int power, int cx, int cy, beh_type corps_beh, int corps_hit,
                  int actual );

int burn_freeze( int pow, struct bolt &beam, int flavour );

int corpse_rot(int power);

int summon_elemental( int pow, int force_type, int unfriendly, bool charm );

int vampiric_drain( int pow, struct bolt &beam );

int detect_creatures( int pow );
int detect_items( int pow );
int detect_traps( int pow );

void cast_refrigeration(int pow);
void cast_toxic_radiance(void);
void cast_twisted( int power, beh_type corps_beh, int corps_hit );

void drain_life(int pow);
void holy_word( int pow, bool silent );

void call_imp( int pow );
void summon_ice_beast_etc( int pow, int type, beh_type behav = BEH_FRIENDLY );
void summon_demons( int pow, bool group );

void summon_scorpions(int pow);
void summon_small_mammals(int pow);
bool summon_swarm( int pow, bool unfriendly, bool god_gift );
void summon_things(int pow);
void summon_undead(int pow);

void turn_undead(int pow);      // what should I use for pow?

#endif

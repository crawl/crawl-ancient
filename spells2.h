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

   int corpse_rot(int power);
   unsigned char detect_traps(void);
   unsigned char detect_items(int map_radius);
   unsigned char detect_creatures(int map_radius);
   int animate_dead(int power, int corps_beh, int corps_hit, int actual);
   int animate_a_corpse(char axps, char ayps, int corps_beh, int corps_hit, char class_allowed);
   int raise_corpse(int corps, char corx, char cory, int corps_beh, int corps_hit, int actual);
   void cast_twisted(int power, int corps_beh, int corps_hit);
   bool brand_weapon(char which_brand, int power);
   void manage_shock_shield(void);
   int vampiric_drain(int pow);
   int burn_freeze(int pow, char b_f);
   int summon_elemental(int pow, unsigned char restricted_type, unsigned char unfriendly);
   //void summon_small_mammals(void);
   void summon_small_mammals(int pow); //jmf: changed
   void summon_scorpions(int pow);
   void summon_ice_beast_etc(int pow, int ibc);
   void summon_swarm(int pow);
   void summon_undead(int pow);
   void summon_butter(void);
   void restore_str(void);
   void restore_int(void);
   void restore_dex(void);
   void cast_refrigeration(int pow);
   void drain_life(int pow);
   void summon_things(int pow);
   void turn_undead(int pow);      // what should I use for pow?

   void holy_word(int pow);
   void cast_toxic_radiance(void);
#endif

/*
 *  File:       item_use.cc
 *  Summary:    Functions for making use of inventory items.
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 *              <2>             5/26/99         JDJ             Exposed armour_prompt. takeoff_armour takes an index argument.
 *              <1>             -/--/--         LRH             Created
 */


#ifndef ITEM_USE_H
#define ITEM_USE_H

#include <string>

bool enchant_weapon( int which_stat, bool quiet = false );

bool armour_prompt(const std::string & mesg, int *index);
bool takeoff_armour(int index);

void drink(void);

void original_name(void);

void puton_ring(void);

void read_scroll(void);

void unwear_ring( int ring );
void remove_ring(void);

int get_fire_item_index(void);
void shoot_thing(void);
void throw_anything(void);

void wear_armour(void);
bool do_wear_armour( int item, bool quiet );

struct item_def;
bool can_wield( const item_def &weapon );

void wield_weapon( bool auto_wield, int slot = -1 );

void zap_wand(void);

void wield_effects( bool showMsgs );
void wield_warning( bool newWeapon );

void use_randart( int item_wield_2 );

#endif

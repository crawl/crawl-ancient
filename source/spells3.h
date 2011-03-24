/*
 *  File:       spells3.cc
 *  Summary:    Implementations of some additional spells.
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 *               <1>     -/--/--        LRH             Created
 */


#ifndef SPELLS3_H
#define SPELLS3_H

bool allow_control_teleport( bool silent = false );
void you_teleport( void );
void you_teleport2( bool allow_control, bool new_abyss_area = false );
int  blink( void );
void random_blink( int power );
int  cast_semi_controlled_blink( int pow );

int  airstrike( int power, const struct bolt &beam );
int  cast_bone_shards( int power );
bool cast_death_channel( int power );
void cast_poison_ammo( void );
void cast_selective_amnesia( bool force );
int  cast_smiting( int power );
int  project_noise( void );
void dancing_weapon( int pow, bool force_hostile );
bool detect_curse( bool suppress_msg );
bool cast_entomb( int power );
void portal( void );
bool recall( bool undead_only );
bool remove_curse( bool suppress_msg );
void sublimation( int power );
void simulacrum( int power );

#endif

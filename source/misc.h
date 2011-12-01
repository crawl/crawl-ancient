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

// running system commands
run_dir_type convert_to_compass( int dx, int dy );
bool check_naked_ok();
void start_running( run_dir_type dir, run_mode_type mode );
void stop_running( void );
bool is_resting( void );
bool check_stop_running( void );

// grid id predicates and utility stuff
bool grid_is_wall( unsigned char grid );
bool grid_is_opaque( unsigned char grid );
bool grid_is_solid( unsigned char grid );
bool grid_is_water( unsigned char grid );
bool grid_destroys_items( unsigned char grid );
bool grid_is_branch_stairs( unsigned char grid );
god_type grid_altar_god( unsigned char grid );
unsigned char grid_secret_door_appearance( int gx, int gy );
unsigned char grid_base_type( int x, int y );

void handle_player_shield_blocks( int damage, int exer );
void handle_mon_shield_blocks( struct monsters *mon );

void exercise_armour( int exer, int chance = 1 );
void exercise_dodging( int exer, int chance = 1 );

bool fired_ammo_recoverable( const item_def &ammo );

int adjust_time_for_speed( int time_taken, speed_type speed );
int adjust_energy_for_speed( int energy, speed_type speed );

void inc_berserk( int inc );
void dec_berserk( int dec );
bool go_berserk( bool intentional );

void search_around(void);

void manage_clouds(void);

void disarm_trap(struct dist &disa);

void down_stairs( unsigned char stair_used, int old_level, bool real_stairs );

bool fall_into_a_pool( int entry_x, int entry_y, bool allow_random_scramble,
                       unsigned char terrain );

void handle_traps( char trt, int i, bool trap_known, bool disarm );

void in_a_cloud(void);

void drop_trap_item( int trap_id, int x, int y, short quant, bool attrition = true );

void enter_new_level(void);

void turn_corpse_into_chunks( item_def &item );

int trap_at_xy(int which_x, int which_y);
void remove_trap( int trap_id );

void up_stairs( unsigned char stair_used, bool real_stairs );

void weird_colours(unsigned char coll, char wc[30]);
void weird_writing(char stringy[40]);

int trap_category( int type );

// used to fake str and dex for nicknames
void fudge_ghost_stats( int &str, int &dex );

const char *const damage_punctuation( int damage );

int conv_char_to_obj_class( char keyin );

#endif

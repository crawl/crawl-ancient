/*
 *  File:       stuff.cc
 *  Summary:    Misc stuff.
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 *   <3>    11/14/99     cdl    added random40
 *   <2>    11/06/99     cdl    added random22
 *   <1>     -/--/--     LRH    Created
 */


#ifndef STUFF_H
#define STUFF_H

#include "enum.h"
#include "defines.h"
#include "externs.h"

char *const make_time_string( time_t abs_time, char *const buff,
                              int buff_size, bool terse = false );

char *const number_string( int num );

bool in_bounds( int x, int y );
bool map_bounds( int x, int y );

void set_redraw_status( unsigned long flags );

void tag_followers( void );
void untag_followers( void );

void seed_rng( void );

bool coinflip(void);
bool one_chance_in( int a_million );
unsigned long random_int( void );
int random2( int randmax );

// regular one based dice
int roll_dice( int num, int size, bool zero_base = false );
int roll_dice( const struct dice_def &dice );

// zero based dice cover functions
int roll_zdice( int num, int size );
int roll_zdice( const struct dice_def &dice );

int roll_curved_percentile( void );

int stepdown_value(int base_value, int stepping, int first_step, int last_step, int ceiling_value);

int div_rand_round( int num, int den );
int conv_double_rand_round( double num );

unsigned char get_ch(void);

void end(int end_arg);

void modify_all_stats(int STmod, int IQmod, int DXmod);

void redraw_screen( void );

void canned_msg(unsigned char which_message);
bool yesno( const char * str, bool safe = true, bool clear_after = true );

int grid_distance( int x, int y, int x2, int y2 );
int distance( int x, int y, int x2, int y2);
bool adjacent( int x, int y, int x2, int y2 );

int skill_bump( int skill );

int stat_mult( int stat_level, int value, int div = 20, int shift = 3 );
int stat_div( int stat_level, int value, int mult = 20, int shift = 3 );

bool silenced( int x, int y );

bool player_can_hear( int x, int y );

// random in bounds grid (clear guarantees floor, empty guarantees NON_MONSTER)
void random_in_bounds( int &x_pos, int &y_pos, int terr = DNGN_FLOOR,
                       bool empty = false, bool excl = true );

void random_place_rectangle( int &x1, int &y1, int &x2, int &y2,
                             bool excl = true );

bool in_rectangle( int px, int py, int rx1, int ry1, int rx2, int ry2,
                   bool excl = false );

bool rectangles_overlap( int ax1, int ay1, int ax2, int ay2,
                         int bx1, int by1, int bx2, int by2,
                         bool excl = false );

int  race_colour( unsigned long race );
bool is_element_colour( int col );
int  element_colour( int element, bool no_random = false );

char index_to_letter (int the_index);
int  letter_to_index(int the_letter);
int  near_stairs( int px, int py, int max_dist, unsigned char &stair_gfx );

void init_id_array( char temp_id[NUM_IDTYPE][MAX_SUBTYPES],
                    item_type_id_state_type val );

inline bool testbits(unsigned int flags, unsigned int test)
{
    return ((flags & test) == test);
}

inline int sgn( int x )
{
    return ((x < 0) ? -1 : (x > 0));
}

#endif

/*
 *  File:       monstuff.cc
 *  Summary:    Misc monster related functions.
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 *               <1>     -/--/--        LRH             Created
 */


#ifndef MONSTUFF_H
#define MONSTUFF_H


// useful macro
#define SAME_ATTITUDE(x) (mons_friendly(x)?BEH_FRIENDLY:BEH_HOSTILE)

// for definition of type monsters {dlb}
#include "externs.h"
// for definition of type monsters {dlb}

void get_mimic_item( const struct monsters *mimic, item_def & item );
int  get_mimic_colour( struct monsters *mimic );

void set_wander_target( struct monsters *mon );

void alert_nearby_monsters(void);
bool monster_polymorph(struct monsters *monster, int targetc, int power);
void monster_die(struct monsters *monster, killer_type killer, int src);
void monster_cleanup(struct monsters *monster);
void behaviour_event( struct monsters *mon, int event_type,
                      int src = MHITNOT, int src_x = 0, int src_y = 0 );

bool curse_random_item( bool special_curses );

bool monster_blink( struct monsters *monster, bool control = false,
                    bool quiet = false );

bool random_near_space( int ox, int oy, int &tx, int &ty,
    bool allow_adjacent = false, bool restrict_LOS = true);

bool mon_msg( struct monsters *monster, const char *event,
              bool check_vis = true,
              msg_channel_type channel = MSGCH_PLAIN,
              int param = 0 );

bool swap_places( struct monsters *monster );

void print_wounds( struct monsters *monster );

bool wounded_damaged( int wound_class );

void handle_monsters(void);

void swap_monsters_positions( int m1_id, int m2_id );
void move_monster_to_grid( int mon_id, int x, int y );

unsigned char monster_habitat( int which_class );
void mons_in_cloud( struct monsters *monster );

bool message_current_target(void);

int monster_index( struct monsters *monster );

bool hurt_monster( struct monsters *victim, int damage );
bool hurt_monster_to_kill( struct monsters *victim, int damage,
                           killer_type killer, int src );
bool you_hurt_monster( struct monsters *victim, int damage );

bool heal_monster( struct monsters *patient, int health_boost, bool permit_growth );

#endif

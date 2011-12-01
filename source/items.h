/*
 *  File:       items.cc
 *  Summary:    Misc (mostly) inventory related functions.
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 *               <2>     6/9/99         DML             Autopickup
 *               <1>     -/--/--        LRH             Created
 */


#ifndef ITEMS_H
#define ITEMS_H

#include "externs.h"

bool is_valid_item( const item_def &item );

bool dec_inv_item_quantity( int obj, int amount );
bool dec_mitm_item_quantity( int obj, int amount );

void inc_inv_item_quantity( int obj, int amount );
void inc_mitm_item_quantity( int obj, int amount );

void move_item_to_grid( int *const obj, int x, int y );
void move_item_stack_to_grid( int x, int y, int targ_x, int targ_y );
int  move_item_to_player( int obj, int quant_got, bool quiet = false );
bool items_stack( const item_def &item1, const item_def &item2 );

bool shift_monster( struct monsters *mon, int x = 0, int y = 0 );

void init_item( item_def &item, int base = OBJ_UNASSIGNED, int sub = 0, int quant = 0 );

void invalidate_item( item_def &item );

void link_items(void);
void fix_item_coordinates(void);
int  cull_items(void);
int  get_item_slot( int reserve = 50 );
void unlink_item(int dest);
void destroy_item(int dest);
void destroy_item_stack( int x, int y );
void item_check( bool looking );

void do_pickup( void );

bool copy_item_to_grid( const item_def &item, int x_plos, int y_plos,
                        int quant_drop = -1,    // item.quantity by default
                        bool mark_dropped = false );

bool move_top_item( int src_x, int src_y, int dest_x, int dest_y );

bool do_drop( void );

void update_corpses( double elapsedTime );
void update_level( double elapsedTime );
void handle_time( unsigned int time_delta );

int inv_count(void);

#endif

/*
 *  File:       view.cc
 *  Summary:    Misc function used to render the dungeon.
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 *     <2>     9/29/99     BCR     Added the BORDER_COLOR define
 *     <1>     -/--/--     LRH     Created
 */


#ifndef VIEW_H
#define VIEW_H

#include "externs.h"
#include <string>

void find_features( const std::vector<coord_def>& features,
                    unsigned char feature, std::vector<coord_def> *found );

unsigned char get_envmap_char( int x, int y );
void set_envmap_char( int x, int y, unsigned char chr );
bool is_terrain_known( int x, int y );
bool is_terrain_seen( int x, int y );
bool is_terrain_changed( int x, int y );
void set_terrain_seen( int x, int y );
void set_terrain_mapped( int x, int y );
void set_terrain_changed( int x, int y );
void clear_envmap_grid( int x, int y );
void clear_envmap( void );

int get_number_of_lines(void);

bool mons_near(struct monsters *monster, unsigned int foe = MHITYOU);
bool see_grid( int grx, int gry );

void item_grid( void );

void losight( FixedArray<unsigned int, 19, 19>& sh,
              FixedArray<unsigned char, 80, 70>& gr,
              int x_p, int y_p );

void magic_mapping( int map_radius, int proportion );

bool noisy( int loudness, int nois_x, int nois_y, const char *msg = 0 );
bool mon_noisy( int loudness, struct monsters* mon, const char *msg = 0 );

bool show_map( FixedVector<int, 2>& spec_place, bool travel_mode = false );

unsigned char get_magicmap_char( int feature );
unsigned char get_sightmap_char( int feature );

void init_char_table( char_set_type set );
void init_feature_table( void );

std::string screenshot( bool fullscreen = false );
void viewwindow( bool draw_it, bool do_updates );

void setLOSRadius(int newLR);

bool check_awaken(int mons_aw);

#endif

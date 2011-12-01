/*
 *  File:       transfor.cc
 *  Summary:    Misc function related to player transformations.
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 *               <1>     -/--/--        LRH             Created
 */


#ifndef TRANSFOR_H
#define TRANSFOR_H

#include "FixVec.h"
#include "enum.h"

// macro to convert EQ_WEAPON..EQ_BODY_ARMOUR into bit flags
#define RMFLAG(xxx)             (1 << (xxx))

// and some helpers
#define RM_NONE                 0x0000
#define RM_ALL                  0x007f
#define RM_ALL_BUT(xxx)         (0x03ff & ~(xxx))

#define RM_RINGS                0x0180
// setting the extra high bit here to signal the drop in inventory
#define RM_EVERYTHING           0x83ff

bool can_equip( char use_which );
bool transform_can_equip_type( int eq_type );

bool remove_equipment( unsigned int rem );

void untransform( void );
bool transform( int pow, int form );

int transform_hp_factor( void );
int transform_stat_bonus( int stat );

size_type  transform_size( int psize = PSIZE_BODY );

brand_type   transform_hand_brand( void );
int          transform_hand_type( void );

int transform_hand_damage( void );
int transform_hand_to_hit( void );
int transform_hand_speed( void );

bool transform_changed_physiology( bool phys_scales = false );

int transform_res_fire( void );
int transform_res_cold( void );
int transform_res_elec( void );
int transform_res_pois( void );
int transform_prot_life( void );
int transform_res_magic( void );
int transform_metabolism( void );
int transform_spell_penalty( void );
bool transform_can_untransform( void );

void merfolk_start_swimming( void );

#endif

/*
 *  File:       spl-cast.cc
 *  Summary:    Spell casting functions.
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 *               <1>     -/--/--        LRH             Created
 */


#ifndef SPL_CAST_H
#define SPL_CAST_H

const char * failure_description( int fail_rate );

unsigned char list_spells( void );
int spell_fail( int spell, bool raw = false );

int spell_memorise_fail( int spell );
int calc_spell_power( int spell, bool quiet = false );

bool cast_a_spell( void );

int your_spells( int spc2, int powc = -1, bool allow_fail = true );

bool miscast_effect( unsigned int sp_type, int mag_pow, int mag_fail,
                     int force_effect, const char *cause = 0 );


#endif

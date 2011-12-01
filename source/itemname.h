/*
 *  File:       itemname.cc
 *  Summary:    Misc functions.
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 *               <1>     -/--/--        LRH             Created
 */


#ifndef ITEMNAME_H
#define ITEMNAME_H

#include "externs.h"

void save_id( char identy[NUM_IDTYPE][MAX_SUBTYPES] );
void clear_ids(void);
void set_ident_type( int cla, int ty, char setting, bool force = false );
char get_ident_type( int cla, int ty );
unsigned char check_item_knowledge(void);

bool is_vowel( const char chr );

void item_name( const item_def &item, char descrip, char buff[ITEMNAME_SIZE],
                bool terse = false, bool verbose = false );

const char *it_name( int itn, char des, char buff[ITEMNAME_SIZE],
                     bool terse = false, bool verbose = false );

const char *in_name( int inn, char des, char buff[ITEMNAME_SIZE],
                     bool terse = false, bool verbose = false );

void quant_name( const item_def &item, int quant, char des,
                 char buff[ITEMNAME_SIZE], bool terse = false );

int make_name( unsigned long seed, bool all_caps, char buff[ ITEMNAME_SIZE ] );

#endif

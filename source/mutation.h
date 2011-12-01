/*
 *  File:       mutation.cc
 *  Summary:    Functions for handling player mutations.
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 *               <1>     -/--/--        LRH             Created
 */


#ifndef MUTATION_H
#define MUTATION_H

#include "enum.h"

int get_scale_property( armour_property_type prop );

bool mutate( int which_mutation, bool force = false, bool failMsg = true );

void init_mutations(void);
void display_mutations(void);

bool delete_mutation( int which_mutation, bool force = false,
                      bool need_mutate_msg = true );

// default of level == -1, means to use the player's current level
const char *mutation_desc( int which_mutat, int level = -1 );

mutation_type give_good_mutation( bool failMsg = true );
mutation_type give_bad_mutation( bool forceMutation = false, bool failMsg = true );
mutation_type give_cosmetic_mutation( void );

void demonspawn(void);

#endif

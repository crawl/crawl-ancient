/*
 *  File:       shopping.cc
 *  Summary:    Shop keeper functions.
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 *               <1>     -/--/--        LRH             Created
 */


#ifndef SHOPPING_H
#define SHOPPING_H

#include "externs.h"

int randart_value( const item_def &item );

// ident == true overrides the item ident level and gives the price
// as if the item was fully id'd
unsigned int item_value( item_def item, char id[NUM_IDTYPE][MAX_SUBTYPES],
                         bool ident = false );

unsigned int item_value( item_def item );

void shop(void);

const char *shop_name(int sx, int sy);

#endif

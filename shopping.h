#ifndef SHOPPING_H
#define SHOPPING_H
/*
 *  File:       shopping.cc
 *  Summary:    Shop keeper functions.
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 *               <1>     -/--/--        LRH             Created
 */

char in_a_shop(char shoppy, char id[4][50]);
unsigned int item_value(unsigned char item_clas, unsigned char item_typ, unsigned char item_da, unsigned char it_plus, unsigned char it_plus2, unsigned int item_quant, char ident_lev, char id[4][50]);
char book_rarity(char which_book);
void shop(void);
#endif

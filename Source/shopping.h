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


// last updated 12may2000 {dlb}
/* ***********************************************************************
 * called from: chardump - invent - ouch - religion - shopping
 * *********************************************************************** */
unsigned int item_value(unsigned char item_clas, unsigned char item_typ,
                        unsigned char item_da, unsigned char it_plus,
                        unsigned char it_plus2, unsigned int item_quant,
                        char ident_lev, char id[4][50]);


// last updated 12may2000 {dlb}
/* ***********************************************************************
 * called from: misc
 * *********************************************************************** */
void shop(void);


#endif

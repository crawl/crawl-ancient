/*
 *  File:       randart.cc
 *  Summary:    Random and unrandom artifact functions.
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 *               <1>     -/--/--        LRH             Created
 */
#ifndef RANDART_H
#define RANDART_H

#define NO_UNRANDARTS 14


char *randart_name(unsigned char aclass, unsigned char atype, unsigned char adam, unsigned char aplus, unsigned char aplus2, unsigned char ident_lev);
char *randart_armour_name(unsigned char aclass, unsigned char atype, unsigned char adam, unsigned char aplus, unsigned char aplus2, unsigned char ident_lev);
char *randart_ring_name(unsigned char aclass, unsigned char atype, unsigned char adam, unsigned char aplus, unsigned char aplus2, unsigned char ident_lev);
void standard_name_weap(unsigned char item_typ, char glog[80]);
int randart_wpn_properties(unsigned char aclass, unsigned char atype, unsigned char adam, unsigned char aplus, unsigned char aplus2, unsigned char acol, char prop);
void standard_name_armour(unsigned char item_typ, unsigned char item_plus2, char glorg[80]);

int number_unrandarts(void);
void set_unrandart_exist(int whun, char is_exist);
char does_unrandart_exist(int whun);
int find_okay_unrandart(unsigned char aclass);
void make_item_unrandart(int x, int ura_item);
char *unrandart_descrip(char which_descrip, unsigned char aclass, unsigned char atype, unsigned char aplus, unsigned char aplus2);

#endif

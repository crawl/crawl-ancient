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


// used in files.cc, newgame.cc, randart.cc {dlb}
#define NO_UNRANDARTS 14


/* ***********************************************************************
 * called from: itemname
 * *********************************************************************** */
char *randart_armour_name(unsigned char aclass, unsigned char atype,
                          unsigned char adam, unsigned char aplus,
                          unsigned char aplus2, unsigned char ident_lev);


/* ***********************************************************************
 * called from: itemname
 * *********************************************************************** */
char *randart_name(unsigned char aclass, unsigned char atype,
                   unsigned char adam, unsigned char aplus,
                   unsigned char aplus2, unsigned char ident_lev);


/* ***********************************************************************
 * called from: itemname
 * *********************************************************************** */
char *randart_ring_name(unsigned char aclass, unsigned char atype,
                        unsigned char adam, unsigned char aplus,
                        unsigned char aplus2, unsigned char ident_lev);


/* ***********************************************************************
 * called from: describe
 * *********************************************************************** */
char *unrandart_descrip(char which_descrip, unsigned char aclass,
                        unsigned char atype, unsigned char aplus,
                        unsigned char aplus2);


/* ***********************************************************************
 * called from: files
 * *********************************************************************** */
char does_unrandart_exist(int whun);


/* ***********************************************************************
 * called from: dungeon
 * *********************************************************************** */
int find_okay_unrandart(unsigned char aclass);


/* ***********************************************************************
 * called from: describe - fight - it_use2 - item_use - player
 * *********************************************************************** */
int randart_wpn_properties(unsigned char aclass, unsigned char atype,
                           unsigned char adam, unsigned char aplus,
                           unsigned char aplus2, unsigned char acol,
                           char prop);


/* ***********************************************************************
 * called from: dungeon
 * *********************************************************************** */
void make_item_unrandart(int x, int ura_item);


/* ***********************************************************************
 * called from: files - newgame
 * *********************************************************************** */
void set_unrandart_exist(int whun, char is_exist);


/* ***********************************************************************
 * called from: itemname
 * *********************************************************************** */
void standard_name_armour(unsigned char item_typ, unsigned char item_plus2,
                          char glorg[80]);


/* ***********************************************************************
 * called from: itemname
 * *********************************************************************** */
void standard_name_weap(unsigned char item_typ, char glog[80]);


#endif

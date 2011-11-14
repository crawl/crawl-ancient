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


/* ***********************************************************************
 * called from: describe - effects - item_use - shopping
 * *********************************************************************** */
char get_id(char cla, char ty);


/* ***********************************************************************
 * called from: acr - chardump - direct - effects - fight - invent -
 *              it_use2 - item_use - items - monstuff - mstuff2 - ouch -
 *              shopping - spells1 - spells2 - spells3
 * *********************************************************************** */
char item_name(unsigned char item_plus2, char item_clas, char item_typ,
               unsigned char item_da, unsigned char it_plus,
               unsigned int it_quant, char ident_lev, char descrip,
               char glag[60]);


/* ***********************************************************************
 * called from: beam - describe - fight - item_use - items - monstuff -
 *              player
 * *********************************************************************** */
int mass(int pr1, int pr2);


/* ***********************************************************************
 * called from: debug - describe - dungeon - fight - files - item_use -
 *              monstuff - mstuff2 - players - spells0
 * *********************************************************************** */
int property(int pr1, int pr2, int pr3);


/* ***********************************************************************
 * called from: acr
 * *********************************************************************** */
unsigned char check_item_knowledge(void);


/* ***********************************************************************
 * called from: acr
 * *********************************************************************** */
void clear_ids(void);


/* ***********************************************************************
 * called from: acr - chardump - command - effects - fight - invent -
 *              it_use2 - it_use3 - item_use - items - ouch - output -
 *              spell - spells1 - spells2 - spells3 - spells4 - transfor
 * *********************************************************************** */
void in_name(int inn, char des, char str_pass[80]);
void in_quant_name(int inn, int quant, char des, char str_pass[80]);


/* ***********************************************************************
 * called from: acr
 * *********************************************************************** */
void init_properties(void);


/* ***********************************************************************
 * called from: direct - fight - food - items - monstuff - religion -
 *              shopping
 * *********************************************************************** */
void it_name(int itn, char des, char str_pass[80]);


/* ***********************************************************************
 * called from: files - randart - shopping
 * *********************************************************************** */
void make_name(unsigned char var1, unsigned char var2, unsigned char var3, char ncase, char str_pass[50]);


/* ***********************************************************************
 * called from: files - shopping
 * *********************************************************************** */
void save_id(char identy[4][50]);


/* ***********************************************************************
 * called from: files - item_use - newgame - ouch - shopping - spells1
 * *********************************************************************** */
void set_id(char cla, char ty, char setting);


/* ***********************************************************************
 * called from: dungeon - item_use
 * *********************************************************************** */
bool hide2armour(unsigned char *which_subtype);


#endif

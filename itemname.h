#ifndef ITEMNAME_H
#define ITEMNAME_H
/*
 *  File:       itemname.cc
 *  Summary:    Misc functions.
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 *               <1>     -/--/--        LRH             Created
 */


char item_name(unsigned char item_plus2, char item_clas, char item_typ, unsigned char item_da, unsigned char it_plus, unsigned int it_quant, char ident_lev, char descrip, char glag[60]);
char item_name_2(unsigned char item_plus2, char item_clas, char item_typ, unsigned char item_da, unsigned char it_plus, unsigned int it_quant, char ident_lev, char glog[60]);
void save_id(char identy[4][50]);
void initial(void);
void set_id(char cla, char ty, char setting);
char get_id(char cla, char ty);
void it_name(int itn, char des, char str_pass[80]);
void in_name(int inn, char des, char str_pass[80]);
int property(int pr1, int pr2, int pr3);
int mass(int pr1, int pr2);
void init_properties(void);
unsigned char check_item_knowledge(void);
char weapon_skill(char wclass, char wtype);
char damage_type(char wclass, char wtype);
void make_name(unsigned char var1, unsigned char var2, unsigned char var3, char ncase, char str_pass[50]);
#endif

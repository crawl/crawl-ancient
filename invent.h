/*
 *  File:       invent.cc
 *  Summary:    Functions for inventory related commands.
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 *               <1>     -/--/--        LRH             Created
 */

#ifndef INVENT_H
#define INVENT_H


//void inventory(int item_class_inv, int inv_quantity [52], unsigned char inv_dam [52], unsigned char inv_class [52], unsigned char inv_type [52], unsigned char inv_plus [52]);

//char invent(unsigned char inv_plus2 [52], int item_class_inv, int inv_quantity [52], unsigned char inv_dam [52], unsigned char inv_class [52], unsigned char inv_type [52], unsigned char inv_plus [52], unsigned char inv_ident [52], char item_wielded, char body_armour, char shield_armour, char cloak_armour, char head_armour, char hand_armour, char foot_armour, char ring [2], char show_price);
char invent(int item_class_inv, char show_price);

unsigned char get_invent(int invent_type);

// BCR - Default argument of 0 to keep code compatibility
void list_commands(int wizard = 0);
#endif

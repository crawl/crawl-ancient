/*
 *  File:       command.cc
 *  Summary:    Misc commands.
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 *               <1>     -/--/--        LRH             Created
 */

#ifndef COMMAND_H
#define COMMAND_H
void stethoscope(int mwh);
void quit_game(void);
void version(void);
void adjust(void);
void get_letter_id(char buff[5], int item_id);
void list_weapons(void);
void list_armour(void);
void list_jewellery(void);

#endif

#ifndef ITEMS_H
#define ITEMS_H
/*
 *  File:       items.cc
 *  Summary:    Misc (mostly) inventory related functions.
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 *               <2>     6/9/99         DML             Autopickup
 *               <1>     -/--/--        LRH             Created
 */


void item_check(char keyin);
int add_item(int item_got);
void pickup();
void item_place(int item_drop_2, int x_plos, int y_plos, int quant_drop);
void drop();
int conv_lett(int item_drop_3);
void update_corpses(double elapsedTime);
void handle_time( int time_delta );

extern long autopickups;
extern int autopickup_on;
#endif

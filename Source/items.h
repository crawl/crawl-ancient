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


#ifndef ITEMS_H
#define ITEMS_H


// used in acr.cc {dlb}:
extern int autopickup_on;

// used in initfile.cc {dlb}:
extern long autopickups;

// last updated: Oct 15 2000 -- bwr
/* ***********************************************************************
 * called from: spells4.cc items.cc
 * *********************************************************************** */
int mass_item( int item );

// last updated: 08jun2000 {dlb}
/* ***********************************************************************
 * called from: acr
 * *********************************************************************** */
void item_check(char keyin);


// last updated: 08jun2000 {dlb}
/* ***********************************************************************
 * called from: acr
 * *********************************************************************** */
void pickup(void);


// last updated 08jun2000 {dlb}
/* ***********************************************************************
 * called from: beam - items - transfor
 * *********************************************************************** */
void item_place(int item_drop_2, int x_plos, int y_plos, int quant_drop);

// last updated Oct 15, 2000 -- bwr
/* ***********************************************************************
 * called from: spells4.cc
 * *********************************************************************** */
bool move_top_item( int src_x, int src_y, int dest_x, int dest_y );

// last updated: 08jun2000 {dlb}
/* ***********************************************************************
 * called from: acr
 * *********************************************************************** */
void drop(void);


// last updated: 08jun2000 {dlb}
/* ***********************************************************************
 * called from: files - items
 * *********************************************************************** */
void update_corpses(double elapsedTime);


// last updated: 08jun2000 {dlb}
/* ***********************************************************************
 * called from: acr
 * *********************************************************************** */
void handle_time(int time_delta);


#endif

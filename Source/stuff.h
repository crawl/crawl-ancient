/*
 *  File:       stuff.cc
 *  Summary:    Misc stuff.
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 *   <3>    11/14/99     cdl    added random40
 *   <2>    11/06/99     cdl    added random22
 *   <1>     -/--/--     LRH    Created
 */


#ifndef STUFF_H
#define STUFF_H


// last updated 12may2000 {dlb}
/* ***********************************************************************
 * called from: acr
 * *********************************************************************** */
void cf_setseed(void);


/* ***********************************************************************
 * called from: xxx
 * *********************************************************************** */
bool coinflip(void);


/* ***********************************************************************
 * called from: xxx
 * *********************************************************************** */
bool one_chance_in(int a_million);


/* ***********************************************************************
 * called from: xxx
 * *********************************************************************** */
int random2(int randmax);


/* ***********************************************************************
 * called from: xxx
 * *********************************************************************** */
int random2avg(int max, int rolls);


// last updated 12may2000 {dlb}
/* ***********************************************************************
 * called from: beam - fight - misc
 * *********************************************************************** */
int random2limit(int max, int limit);


// last updated 12may2000 {dlb}
/* ***********************************************************************
 * called from: bang - beam - monplace - monstuff - spells1 - spells2 -
 *              spells4 - view
 * *********************************************************************** */
bool see_grid(unsigned char grx, unsigned char gry);


// last updated 12may2000 {dlb}
/* ***********************************************************************
 * called from: spells2
 * *********************************************************************** */
int stepdown_value(int base_value, int stepping, int first_step, int last_step, int ceiling_value);


// last updated 12may2000 {dlb}
/* ***********************************************************************
 * called from: ability - command - effects - food - item_use - items -
 *              newgame - ouch - shopping - spell - spl-book - spells1 -
 *              spells3
 * *********************************************************************** */
unsigned char get_ch(void);


// last updated 12may2000 {dlb}
/* ***********************************************************************
 * called from: files - newgame - ouch
 * *********************************************************************** */
void end(int end_arg);


// last updated 12may2000 {dlb}
/* ***********************************************************************
 * called from: newgame
 * *********************************************************************** */
void modify_stats(int STmod, int IQmod, int DXmod);


// last updated 12may2000 {dlb}
/* ***********************************************************************
 * called from: ability - acr - command - direct - invent - item_use -
 *              religion - shopping - spell - spl-book - spells3
 * *********************************************************************** */
void redraw_screen(void);


// last updated 12may2000 {dlb}
/* ***********************************************************************
 * called from: ability - acr - beam - command - decks - debug - effects -
 *              it_use3 - item_use - items - misc - ouch - religion -
 *              spell - spl-book - spells - spells1 - spells2 - spells3 -
 *              spells4
 * *********************************************************************** */
void canned_msg(unsigned char which_message);


// last updated 12may2000 {dlb}
/* ***********************************************************************
 * called from: ability - acr - command - it_use3 - item_use - items -
 *              misc - ouch - religion - spl-book - spells4
 * *********************************************************************** */
bool yesno(const char * str, bool safe = true);


// last updated 21may2000 {dlb}
/* ***********************************************************************
 * called from: fight - monstuff - spells4 - view
 * *********************************************************************** */
int distance(char x1, char x2, char y1, char y2);


// last updated 21may2000 {dlb}
/* ***********************************************************************
 * called from: ability - acr - bang - beam - effects - fight - it_use2 -
 *              it_use3 - monstuff - mstuff2 - religion - spell - spells -
 *              spells3 - spells4 - stuff - view
 * *********************************************************************** */
bool silenced(char x, char y);


// last updated XXmay2000 {dlb}
/* ***********************************************************************
 * called from: xxx
 * *********************************************************************** */
bool player_can_hear(char x, char y);


// last updated 27may2000 {dlb}
/* ***********************************************************************
 * called from: dungeon - files - it_use2 - it_use3 - monstuff - mon-util -
 *              mstuff2 - newgame - spells - view
 * *********************************************************************** */
unsigned char random_colour(void);


// last updated 08jun2000 {dlb}
/* ***********************************************************************
 * called from: ability - chardump - command - files - invent - item_use -
 *              items - newgame - spl-book - spells0 - spells1
 * *********************************************************************** */
char index_to_letter (int the_index);


// last updated 08jun2000 {dlb}
/* ***********************************************************************
 * called from: ability - command - food - it_use3 - item_use - items -
 *              spell - spl-book - spells1 - spells3
 * *********************************************************************** */
int letter_to_index(int the_letter);


#endif
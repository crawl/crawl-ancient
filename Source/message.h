/*
 *  File:       message.cc
 *  Summary:    Functions used to print messages.
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 *               <2>     5/08/99        JDJ             mpr takes a const char* instead of a char array.
 *               <1>     -/--/--        LRH             Created
 */


#ifndef MESSAGE_H
#define MESSAGE_H


// last updated 12may2000 {dlb}
/* ***********************************************************************
 * called from: ability - acr - command - direct - effects - item_use -
 *              misc - player - spell - spl-book - spells1 - spells2 -
 *              spells3
 * *********************************************************************** */
void mesclr(void);


// last updated 12may2000 {dlb}
/* ***********************************************************************
 * called from: acr - bang - beam - decks - fight - files - it_use3 -
 *              item_use - items - message - misc - ouch - player -
 *              religion - spell - spells - spells2 - spells3
 * *********************************************************************** */
void more(void);


// last updated 12may2000 {dlb}
/* ***********************************************************************
 * called from: ability - acr - bang - beam - chardump - command - debug -
 *              decks - direct - effects - fight - files - food - it_use2 -
 *              it_use3 - item_use - items - macro - misc - monplace -
 *              monstuff - mstuff2 - mutation - ouch - overmap - player -
 *              religion - shopping - skills - spell - spl-book - spells -
 *              spells1 - spells2 - spells3 - spells4 - stuff - transfor -
 *              view
 * *********************************************************************** */
void mpr(const char *inf);


// last updated 12may2000 {dlb}
/* ***********************************************************************
 * called from: acr
 * *********************************************************************** */
void replay_messages(void);


// last updated 12may2000 {dlb}
/* ***********************************************************************
 * called from: acr - it_use3 - items - religion
 * *********************************************************************** */
void set_colour(char set_message_colour);


#endif

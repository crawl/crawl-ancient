/*
 *********************************************************************
 *  File:       spl-util.h                                           *
 *  Summary:    data handlers for player spell list                  *
 *  Written by: don brodale <dbrodale@bigfootinteractive.com>        *
 *                                                                   *
 *  Changelog(most recent first):                                    *
 *                                                                   *
 *  <00>     12jun2000     dlb     created after much thought        *
 *********************************************************************
*/


#ifndef SPLUTIL_H
#define SPLUTIL_H


struct spellfields
{
    unsigned int primary    : 4;
    unsigned int secondary  : 4;
    unsigned int tertiary   : 4;
    unsigned int quaternary : 4;
};

struct spellstats
{
    unsigned int difficulty : 4;    // traditionally, the following three fields are synonymous {dlb}
    unsigned int mana_cost  : 4;
    unsigned int food_cost  : 4;
    unsigned int exclusion  : 4;    // still working this one out {dlb}
};

/*
struct spellcharacteristics
{
    target (none, tile, item, being, any, global, foo, foo)        = 3 (level?)
    ownership (none, self, other, self/other)                      = 2
    multiple (none, single, multiple, all)                         = 2 (all+tile = level)
    effect type (none, beam, cone, blast, cloud, foo, foo, foo)    = 3
    effect range (none, ranged, touch, foo)                        = 2 (is touch == range 0)?
    color (colours)                                                = 4
};
*/

struct plyrspell
{
    short id;
    char *title;
    struct spellfields disciplines;
    //struct spellcharacteristics qualities;
    struct spellstats parameters;
};




// last updated 12jun2000 {dlb}
/* ***********************************************************************
 * called from: acr
 * *********************************************************************** */
void init_playerspells(void);


// last updated 12jun2000 {dlb}
/* ***********************************************************************
 * called from: spell
 * *********************************************************************** */
int spell_hunger(int which_spell);


// last updated 12jun2000 {dlb}
/* ***********************************************************************
 * called from: it_use3 - spell - spells3
 * *********************************************************************** */
int spell_mana(int which_spell);


// last updated 12jun2000 {dlb}
/* ***********************************************************************
 * called from: chardump - it_use3 - player - spell - spl-book -
 *              spells0 - spells3
 * *********************************************************************** */
int spell_difficulty(int which_spell);


// last updated 12jun2000 {dlb}
/* ***********************************************************************
 * called from: chardump - spell - spl-book - spells0
 * *********************************************************************** */
bool spell_typematch(int which_spell, int which_discipline);


// last updated 12jun2000 {dlb}
/* ***********************************************************************
 * called from: chardump - command - debug - spl-book - spells0
 * *********************************************************************** */
char *spell_title(int which_spell);


// last updated 14jun2000 {dlb}
/* ***********************************************************************
 * called from:
 * *********************************************************************** */
int spell_restriction(int which_spell, int which_restriction);


#endif

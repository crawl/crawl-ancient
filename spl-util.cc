/*
 *********************************************************************
 *  File:       spl-util.h                                           *
 *  Summary:    data handlers for player-avilable spell list         *
 *  Written by: don brodale <dbrodale@bigfootinteractive.com>        *
 *                                                                   *
 *  Changelog(most recent first):                                    *
 *                                                                   *
 *  <00>     12jun2000     dlb     created after much thought        *
 *********************************************************************
*/


#include "AppHdr.h"
#include "spl-util.h"


#include <ctype.h>
#include <string.h>


#include "externs.h"


static struct plyrspell spelldata[] =
{
#include "spl-data.h"
};


#define PLYRSPELLDATASIZE (sizeof(spelldata)/sizeof(struct plyrspell))


static struct plyrspell *seekspell(int *p_spellid);




/*
 **************************************************
 *                                                *
 *             BEGIN PUBLIC FUNCTIONS             *
 *                                                *
 **************************************************
*/


// all this does is merely refresh the internal spell list {dlb}:
void init_playerspells( void )
{

    seekspell((int *) 0);

    return;

};         // end init_playerspells()




int spell_hunger( int which_spell )
{

    int hunger_value = (int) seekspell(&which_spell)->parameters.food_cost;

    return ( (hunger_value == SPELL_FOOD(1)) ?   50 :
             (hunger_value == SPELL_FOOD(2)) ?   95 :
             (hunger_value == SPELL_FOOD(3)) ?  160 :
             (hunger_value == SPELL_FOOD(4)) ?  250 :
             (hunger_value == SPELL_FOOD(5)) ?  350 :
             (hunger_value == SPELL_FOOD(6)) ?  550 :
             (hunger_value == SPELL_FOOD(7)) ?  700 :
             (hunger_value == SPELL_FOOD(8)) ?  850 :
             (hunger_value == SPELL_FOOD(9)) ? 1000
                                             :    0 );

/* ******************************************************************
// these values were never returned <spell_value() capped at 9> {dlb}
      case 10: return 1000;
      case 11: return 1100;
      case 12: return 1250;
      case 13: return 1380;
      case 14: return 1500;
      case 15: return 1600;
      default: return 1600 + (20 * spell_value);
****************************************************************** */

}          // end spell_hunger()




// applied to spell misfires (more power = worse) and triggers
// for Xom acting (more power = more likely to grab his attention) {dlb}
int spell_mana( int which_spell )
{

    int mana_value = (int) seekspell(&which_spell)->parameters.mana_cost;

    return ( (mana_value == SPELL_MANA(1)) ? 1 :
             (mana_value == SPELL_MANA(2)) ? 2 :
             (mana_value == SPELL_MANA(3)) ? 3 :
             (mana_value == SPELL_MANA(4)) ? 4 :
             (mana_value == SPELL_MANA(5)) ? 5 :
             (mana_value == SPELL_MANA(6)) ? 6 :
             (mana_value == SPELL_MANA(7)) ? 7 :
             (mana_value == SPELL_MANA(8)) ? 8 :
             (mana_value == SPELL_MANA(9)) ? 9
                                           : 0 );

}          // end spell_mana()




// applied in naughties (more difficult = higher level knowledge = worse)
// and triggers for Sif acting (same reasoning as above, just good) {dlb}
int spell_difficulty( int which_spell )
{

    int difficulty = (int) seekspell(&which_spell)->parameters.difficulty;

    return ( (difficulty == SPELL_LEVEL(1)) ? 1 :
             (difficulty == SPELL_LEVEL(2)) ? 2 :
             (difficulty == SPELL_LEVEL(3)) ? 3 :
             (difficulty == SPELL_LEVEL(4)) ? 4 :
             (difficulty == SPELL_LEVEL(5)) ? 5 :
             (difficulty == SPELL_LEVEL(6)) ? 6 :
             (difficulty == SPELL_LEVEL(7)) ? 7 :
             (difficulty == SPELL_LEVEL(8)) ? 8 :
             (difficulty == SPELL_LEVEL(9)) ? 9
                                            : 0 );

}          // end spell_difficulty()




bool spell_typematch( int which_spell, int which_discipline )
{

    bool match_found = false;
    struct spellfields *these_disciplines = &(seekspell(&which_spell)->disciplines);

    if ( (int) these_disciplines->primary == which_discipline )
      match_found = true;
    else if ( (int) these_disciplines->secondary == which_discipline )
      match_found = true;
    else if ( (int) these_disciplines->tertiary == which_discipline )
      match_found = true;
    else if ( (int) these_disciplines->quaternary == which_discipline )
      match_found = true;
    else
      match_found = false;

    return ( match_found );

};         // end spell_typematch()




// this will probably be used often, so rather than use malloc/free
// (which may lead to memory fragmentation) I'll just use a static
// array of characters -- if/when the String changeover takes place,
// this will all shift, no doubt {dlb}
char *spell_title( int which_spell )
{

    static char this_title[41] = "";    // this is generous, to say the least {dlb}

    strncpy(this_title, seekspell(&which_spell)->title, 41);    // truncation better than overrun {dlb}

    return ( this_title );

};         // end spell_title()




int spell_restriction( int which_spell, int which_restriction )
{

    int this_restriction = (int) seekspell(&which_spell)->parameters.exclusion;

    return ( this_restriction == which_restriction );

}          // end spell_restriction()


/*
 **************************************************
 *                                                *
 *              END PUBLIC FUNCTIONS              *
 *                                                *
 **************************************************
*/




// simply a retread of the new mon-util::seekmosnter() applied to another
// context -- see mon-util.cc for details on how it works ... {dlb}
static struct plyrspell *seekspell( int *p_spellid )
{

    static int plyrspell_list[NUM_SPELLS];

// passing null pointer forces refresh -- typically, at gamestart, only {dlb}
    if ( p_spellid == 0 )
    {
        unsigned int x = 0;    // must be unsigned to match size_t {dlb}

        for (x = 0; x < NUM_SPELLS; x++)
          plyrspell_list[x] = -1;

        for (x = 0; x < PLYRSPELLDATASIZE; x++)
          plyrspell_list[spelldata[x].id] = x;

        for (x = 0; x < NUM_SPELLS; x++)
          if ( plyrspell_list[x] == -1 )
            plyrspell_list[x] = plyrspell_list[SPELL_NO_SPELL];

        return (plyrspell *) 0;    // return value should not matter here {dlb}
    }
    else
      return &spelldata[plyrspell_list[(*p_spellid)]];

}          // end seekspell()

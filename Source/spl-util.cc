/*
 *  File:       spl-util.h                                           *
 *  Summary:    data handlers for player-avilable spell list         *
 *  Written by: don brodale <dbrodale@bigfootinteractive.com>        *
 *                                                                   *
 *  Changelog(most recent first):                                    *
 *
 *  <2>     24jun2000     jmf     changed to use new data structure
 *  <1>     12jun2000     dlb     created after much thought        *
 */

#include "AppHdr.h"
#include "spl-util.h"

#include <ctype.h>
#include <string.h>
#include <values.h>

#include "externs.h"
#include "debug.h"

static struct playerspell spelldata[] = {
#include "spl-data.h"
};

static int plyrspell_list[NUM_SPELLS];

#define PLYRSPELLDATASIZE (sizeof(spelldata)/sizeof(struct playerspell))

static struct playerspell *seekspell(int spellid);

/*
 *             BEGIN PUBLIC FUNCTIONS
 */

// all this does is merely refresh the internal spell list {dlb}:
void init_playerspells(void)
{
    unsigned int x = 0;

    for (x = 0; x < NUM_SPELLS; x++)
        plyrspell_list[x] = -1;

    // can only use up to PLYRSPELLDATASIZE _MINUS ONE_,  or the
    // last entry tries to set plyrspell_list[SPELL_NO_SPELL]
    // which corrupts the heap.
    for (x = 0; x < PLYRSPELLDATASIZE - 1; x++)
        plyrspell_list[spelldata[x].id] = x;

    for (x = 0; x < NUM_SPELLS; x++)
    {
        if (plyrspell_list[x] == -1)
            plyrspell_list[x] = plyrspell_list[SPELL_NO_SPELL];
    }

    return;                     // return value should not matter here {dlb}
};                              // end init_playerspells()

int spell_hunger(int which_spell)
{
    int level = seekspell(which_spell)->level;

    switch (level)
    {
    case  1: return   50;
    case  2: return   95;
    case  3: return  160;
    case  4: return  250;
    case  5: return  350;
    case  6: return  550;
    case  7: return  700;
    case  8: return  850;
    case  9: return 1000;
    case 10: return 1000;
    case 11: return 1100;
    case 12: return 1250;
    case 13: return 1380;
    case 14: return 1500;
    case 15: return 1600;
    default: return 1600 + (20 * level);
    }

}                               // end spell_hunger();

// applied to spell misfires (more power = worse) and triggers
// for Xom acting (more power = more likely to grab his attention) {dlb}
int spell_mana(int which_spell)
{
    return seekspell(which_spell)->level;
}

// applied in naughties (more difficult = higher level knowledge = worse)
// and triggers for Sif acting (same reasoning as above, just good) {dlb}
int spell_difficulty(int which_spell)
{
    return seekspell(which_spell)->level;
}

bool spell_typematch(int which_spell, unsigned int which_discipline)
{
    return seekspell(which_spell)->disciplines & which_discipline;
}

//jmf: next two for simple bit handling
unsigned int spell_type(int spell)
{
    return seekspell(spell)->disciplines;
}

int count_bits(unsigned int bits)
{
    unsigned int n;
    int c = 0;

    for (n = 1; n < MAXINT; n <<= 1)
        if (n & bits)
            c++;

    return c;
}

// this will probably be used often, so rather than use malloc/free
// (which may lead to memory fragmentation) I'll just use a static
// array of characters -- if/when the String changeover takes place,
// this will all shift, no doubt {dlb}
/*
  const char *spell_title( int which_spell )
  {
  static char this_title[41] = ""; // this is generous, to say the least {dlb}
  strncpy(this_title, seekspell(which_spell)->title, 41);
  // truncation better than overrun {dlb}
  return ( this_title );
  }         // end spell_title()
*/

const char *spell_title(int spell)      //jmf: ah the joys of driving ms. data
{
    return seekspell(spell)->title;
}

/* //jmf: commented out; add field `restriction' to spell struct if desired
   //     (and if anyone finds a use for such a thing)
   int spell_restriction( int which_spell, int which_restriction )
   {
   int this_restriction = (int) seekspell(which_spell)->restriction;
   return ( this_restriction == which_restriction );
   }          // end spell_restriction()
 */

/*
 **************************************************
 *                                                *
 *              END PUBLIC FUNCTIONS              *
 *                                                *
 **************************************************
 */

//jmf: simplified; moved init code to top function, init_playerspells()
static struct playerspell *seekspell(int spell)
{
    return &spelldata[plyrspell_list[spell]];
}

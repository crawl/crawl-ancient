/*
 *  File:       stuff.cc
 *  Summary:    Misc stuff.
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 *   <4>    11/14/99     cdl    added random40(), made arg to random*() signed
 *   <3>    11/06/99     cdl    added random22()
 *   <2>     9/25/99     cdl    linuxlib -> liblinux
 *   <1>     -/--/--     LRH    Created
 */

#include "AppHdr.h"
#include "stuff.h"

#ifdef DOS
#include <conio.h>
#endif

#ifdef LINUX
#include "liblinux.h"
#endif

#include <stdlib.h>
#include <string.h>

// required for table_lookup() {dlb}
#include <stdarg.h>
// required for table_lookup() {dlb}

#include "externs.h"

#include "message.h"
#include "misc.h"
#include "output.h"
#include "view.h"
#include "skills2.h"

#ifdef MACROS
#include "macro.h"
#endif

unsigned long cfseed;           // required for stuff::coinflip() and cf_setseed()

extern char wield_change;

#ifdef USE_NEW_RANDOM
int random2(int max)
{
    if (max <= 0 || max >= RAND_MAX)
        return 0;
    else
//    return (int) ((((float) max) * rand()) / RAND_MAX); - this is bad!
        //      Uses FP, so is horribly slow on computers without coprocessors.
        return (int) rand() / (RAND_MAX / max + 1);
// Taken from comp.lang.c FAQ. May have problems as max approaches
    //   RAND_MAX, but this is rather unlikely.
    //   We've used rand() rather than random() for the portability, I think.
}
#else
int random2(int randmax)
{
    if (randmax <= 0)
        return 0;

    return random() % randmax;
}
#endif




int random2avg(int max, int rolls)
{
    // this has the same mean as random2(), but a lower variance
    // never use with rolls < 2! that would be silly - use random2() {dlb}
    int sum = 0;

    sum += random2(max);

    for (int i = 0; i < (rolls - 1); i++)
        sum += random2(max + 1);

    return (sum / rolls);
}




int random2limit(int max, int limit)
{
    // designed to randomize evasion.
    // low values of max are slightly lowered
    // high values tend to max out near LIMIT/2

    int i;
    int sum = 0;

    if (max <= 0)
        return 0;

    for (i = 0; i < max; i++)
    {
        if (random2(limit) >= i)
            sum++;
    }

    return sum;

}




unsigned char get_ch()
{
    unsigned char gotched = getch();

    if (gotched == 0)
        gotched = getch();
    return gotched;
}




/* Is a grid within character's line of sight? */
bool see_grid(unsigned char grx, unsigned char gry)
{
    if (grx > you.x_pos - 9 && grx < you.x_pos + 9 && gry > you.y_pos - 9 && gry < you.y_pos + 9)
    {
        if (env.show[grx - you.x_pos + 9][gry - you.y_pos + 9] != 0)
            return true;
    }

    return false;
}




int magic_ability(int mag_abil, int intel)
{

    int retv = mag_abil * intel;

    retv /= 10;


    return retv;

}




void end(int end_arg)
{
#ifdef LINUX
    lincurses_shutdown();
#endif

#ifdef MAC
    deinit_mac();
#endif

    exit(end_arg);
}




void output_value(char string[100], int value)
{
    strcpy(info, string);
    itoa(value, st_prn, 10);
    strcat(info, st_prn);
    mpr(info);
}




#ifdef PLAIN_TERM
// this function is used for systems without gettext/puttext to redraw the
// playing screen after a call to for example inventory.
void redraw_screen(void)
{
    char title[40];

    strcpy(title, skill_title(best_skill(0, 50, 99), you.skills[best_skill(0, 50, 99)], you.char_class, you.experience_level));
    draw_border(you.your_name, title, you.species);

    you.redraw_hit_points = 1;
    you.redraw_magic_points = 1;
    you.redraw_strength = 1;
    you.redraw_intelligence = 1;
    you.redraw_dexterity = 1;
    you.redraw_armor_class = 1;
    you.redraw_evasion = 1;
    you.redraw_gold = 1;
    you.redraw_experience = 1;
    you.redraw_hunger = 1;
    you.redraw_burden = 1;
    wield_change = 1;

    print_stats();

    new_level();

    viewwindow(1, false);


}
#endif




// STEPDOWN FUNCTION to replace conditional chains in spells2.cc 12jan2000 {dlb}
// it is a bit more extensible and optimizes the logical structure, as well
// usage: summon_swarm() summon_undead() summon_scorpions() summon_things()
// ex(1): stepdown_value (foo, 2, 2, 6, 8) replaces the following block:
//
/*
   if (foo > 2)
   foo = (foo - 2) / 2 + 2;
   if (foo > 4)
   foo = (foo - 4) / 2 + 4;
   if (foo > 6)
   foo = (foo - 6) / 2 + 6;
   if (foo > 8)
   foo = 8;
 */
//
// ex(2): bar = stepdown_value(bar, 2, 2, 6, -1) replaces the following block:
//
/*
   if (bar > 2)
   bar = (bar - 2) / 2 + 2;
   if (bar > 4)
   bar = (bar - 4) / 2 + 4;
   if (bar > 6)
   bar = (bar - 6) / 2 + 6;
 */
//
// I hope this permits easier/more experimentation with value stepdowns in the code
// it really needs to be rewritten to accept arbitrary (unevenly spaced) steppings

int stepdown_value(int base_value, int stepping, int first_step, int last_step, int ceiling_value)
{
    int return_value = base_value;

    if (return_value <= first_step)     // values up to the first "step" returned unchanged

    {
        return return_value;
    }

    for (int this_step = first_step; this_step <= last_step; this_step += stepping)
    {
        if (return_value > this_step)
        {
            return_value = ((return_value - this_step) / 2) + this_step;
        }
        else
        {
            break;              // exit loop iff value fully "stepped down"

        }
    }

    if (ceiling_value != -1 && return_value > ceiling_value)    // "no final ceiling" == -1

    {
        return ceiling_value;   // highest value to return is "ceiling"

    }
    else
    {
        return return_value;    // otherwise, value returned "as is"

    }

}




// I got so tired of seeing: ".. && random2(foo) == 0 && .." in the code
// that I broke down and wrote this little -- very little -- function.
// anyway, I think improving the readability of the code offsets whatever
// overhead the additional (intermediary) function call added to Crawl -
// we'll just make it up by tightening code elsewhere, right guys?
// [use with == and != only .. never directly with comparisons or math]
//                                                      -- 14jan2000 {dlb}


bool one_chance_in(int a_million)
{

    return (random2(a_million) == 0);   // that's it? :P {dlb}

}




// I got to thinking a bit more about how much people talk
// about RNGs and RLs and also about the issue of performance
// when it comes to Crawl's RNG ... turning to *Numerical
// Recipies in C* (Chapter 7-4, page 298), I hit upon what
// struck me as a fine solution.
// You can read all the details about this function (pretty
// much stolen shamelessly from NRinC) elsewhere, but having
// tested it out myself I think it satisfies Crawl's incessant
// need to decide things on a 50-50 flip of the coin. No call
// to random2() required -- along with all that wonderful math
// and type casting -- and only a single variable its pointer,
// and some bitwise operations to randomly generate 1s and 0s!
// No parameter passing, nothing. Too good to be true, but it
// works as long as cfseed is not set to absolute zero when it
// is initialized ... good for 2**n-1 random bits before the
// pattern repeats (n = long's bitlength on your platform).
// It also avoids problems with poor implementations of rand()
// on some platforms in regards to low-order bits ... a big
// problem if one is only looking for a 1 or a 0 with random2()!

// Talk about a hard sell! Anyway, it returns bool, so please
// use appropriately -- I set it to bool to prevent such
// tomfoolery, as I think that pure RNG and quickly grabbing
// either a value of 1 or 0 should be separated where possible
// to lower overhead in Crawl ... at least until it assembles
// itself into something a bit more orderly :P 16jan2000 {dlb}

// NB(1): cfseed is defined atop stuff.cc
// NB(2): IB(foo) and MASK are defined in defines.h - probably
//        not the best place for them, but someone else can move 'em.
// NB(3): the function assumes that cf_setseed() has been called
//        beforehand - the call is presently made in acr::initialise()
//        right after srandom() and srand() are called (note also
//        that cf_setseed() requires rand() - random2 returns int
//        but a long can't hurt there).

bool coinflip(void)
{
    extern unsigned long cfseed;        // defined atop stuff.cc

    unsigned long *ptr_cfseed = &cfseed;

    if (*ptr_cfseed & IB18)
    {
        *ptr_cfseed = ((*ptr_cfseed ^ MASK) << 1) | IB1;
        return true;
    }
    else
    {
        *ptr_cfseed <<= 1;
        return false;
    }
}

// cf_setseed should only be called but once in all of Crawl!!! {dlb}

void cf_setseed(void)
{
    extern unsigned long cfseed;        // defined atop stuff.cc

    unsigned long *ptr_cfseed = &cfseed;

    do
    {

        *ptr_cfseed = rand();

    }
    while (*ptr_cfseed == 0);

}




// a function that accepts a value for value range and a
// (possibly limited?) list of value-results pairings to
// be compared against a random number within the range
// supplied. intended to replace short outcome lists that
// are not worthy of more sophisticated handling. lengthy
// results tables or tables shared across several f(x)'s
// should probably be arrayed and handled by a function
// working with pointers to the array(s) themselves -- I
// think this latter approach is easier to maintan, offers
// lower overhead in manipulating the tables, etc. like I
// said, this is for one-shot, short lists to improve
// program readability and code maintainence/tweaking.

// make certain that the values following the first come
// in pairs (always) and that the last value pair has 0
// as its second half .. look at the code below and you
// will see why.

// lists whose probabilites can be easily reduced to a
// common denominator may be better handled as a switch -
// the overhead will be lower, though I think it's cooler
// to use table_lookup() instead :P

// ex: thing_called = table_lookup( 25,
//                                  MONS_WRAITH, 9,
//                                  MONS_SPECTRAL_WARRIOR, 4,
//                                  MONS_FREEZING_WRAITH, 0 );
// suitably replaces the following code block (from spells2):

/*
   int temp_rand = random2(25);
   // recreated odds 14jan2000 {dlb}
   if (temp_rand > 8)                                 // 64% chance {dlb}
   {
   thing_called = MONS_WRAITH;
   }
   else if (temp_rand > 3)                            // 20% chance {dlb}
   {
   thing_called = MONS_SPECTRAL_WARRIOR;
   }
   else                                          // 16% chance {dlb}
   {
   thing_called = MONS_FREEZING_WRAITH;
   }
 */

// the original is quite efficient compared to table_lookup(),
// which has to pass all the values to a function for processing
// (this is why pointers to an array would be more efficient for
// *long* lists) - but, hey, the original "original" called rand2()
// for each choice of monster -- either way, rand2() is now called
// only once and not eight times -- and it need not compare values
// eight times, either (but it may), so I think: overall net gain.
// I'll let you decide which is more concise - 16jan2000 {dlb}
// NB(1): this function requires <stdargs.h> which is at the top
//        of stuff.cc - I *think* it belongs there. {dlb}
// NB(2): table_lookup() returns an int and all arguments passed
//        must also be ints -- if there is ever need, it could
//        be reworked to handle other/any data types, but ints
//        seem to have the run of the place around here.
// NB(3): I usually avoid using >= when > is as appropriate,
//        but that would mean tagging the final value with a -1
//        so that ( 0 > -1 ) would be true if the rolled value
//        was a 0 or the event occurred only once in n times ...
//        live with it, I guess. By avoiding -1, the function
//        can later be turned to unsigned int should that be a
//        good thing(tm) to do.
// may be best for unbalanced lists of 4 to 9 members, inclusive???
// (wow - the comments far surpass the actual function in size!)

int table_lookup(int die_roll,...)
{
    va_list which_entry;
    int outcome = random2(die_roll);
    int result = 0;

    va_start(which_entry, die_roll);

    do
    {
        result = va_arg(which_entry, int);
        if (outcome >= va_arg(which_entry, int))
              break;
    }
    while (1);

    va_end(which_entry);

    return result;
}




// simple little function to quickly modify all three stats
// at once - does check for '0' modifiers to prevent needless
// adding .. could use checking for sums less than zero, I guess.
// used in conjunction with newgame::species_stat_init() and
// newgame::job_stat_init() routines 24jan2000 {dlb}

void modify_stats(int STmod, int IQmod, int DXmod)
{
    if (STmod != 0)
    {
        you.strength += STmod;
        you.max_strength += STmod;
    }
    if (IQmod != 0)
    {
        you.intel += IQmod;
        you.max_intel += IQmod;
    }
    if (DXmod != 0)
    {
        you.dex += DXmod;
        you.max_dex += DXmod;
    }

    return;
}

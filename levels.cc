/*
 *  File:       levels.cc
 *  Summary:    Functions used in Pandemonium.
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 *               <1>     -/--/--        LRH             Created
 */

#include "AppHdr.h"
#include "levels.h"

#include "externs.h"

#include "monplace.h"
#include "mstruct.h"
#include "stuff.h"

void init_pandemonium()
{
    int pc = 0;

    for (pc = 0; pc < MNST; pc++)
    {
        // this looks for special (unique) demons and sets appropriate lists of demons. Note that this also sets the level colours.
        if (menv[pc].type == 251)       // Nemelex

        {
            env.mons_alloc[0] = 23;
            env.mons_alloc[1] = 23;
            env.mons_alloc[2] = 23;
            env.mons_alloc[3] = 49;
            env.mons_alloc[4] = 225;
            env.mons_alloc[5] = 224;
            env.mons_alloc[6] = 225;
            env.mons_alloc[7] = 232;
            env.mons_alloc[8] = 233;
            env.mons_alloc[9] = 221;
            return;
        }

        if (menv[pc].type == 252)       // Sif Muna

        {
            env.mons_alloc[0] = 227;
            env.mons_alloc[1] = 228;
            env.mons_alloc[2] = 228;
            env.mons_alloc[3] = 229;
            env.mons_alloc[4] = 231;
            env.mons_alloc[5] = 232;
            env.mons_alloc[6] = 228;
            env.mons_alloc[7] = 227;
            env.mons_alloc[8] = 220;
            env.mons_alloc[9] = 227;
            return;
        }

        if (menv[pc].type == 253)       // Okawaru

        {
            env.mons_alloc[0] = 68;     // Efreet

            env.mons_alloc[1] = 23;
            env.mons_alloc[2] = 226;
            env.mons_alloc[3] = 226;
            env.mons_alloc[4] = 225;
            env.mons_alloc[5] = 221;
            env.mons_alloc[6] = 226;
            env.mons_alloc[7] = 229;
            env.mons_alloc[8] = 233;
            env.mons_alloc[9] = 233;
            return;
        }

        if (menv[pc].type == 254)       // Kikubaaqudgha

        {
            env.mons_alloc[0] = 107;
            env.mons_alloc[1] = 107;
            env.mons_alloc[2] = 108;
            env.mons_alloc[3] = 220;
            env.mons_alloc[4] = 234;
            env.mons_alloc[5] = 227;
            env.mons_alloc[6] = 228;
            env.mons_alloc[7] = 230;
            env.mons_alloc[8] = 230;
            env.mons_alloc[9] = 230;
            return;
        }

        if (menv[pc].type == 255)       // mollusc lord

        {
            env.mons_alloc[0] = 242;
            env.mons_alloc[1] = 23;
            env.mons_alloc[2] = 23;
            env.mons_alloc[3] = 242;
            env.mons_alloc[4] = 243;
            env.mons_alloc[5] = 242;
            env.mons_alloc[6] = 243;
            env.mons_alloc[7] = 49;
            env.mons_alloc[8] = 49;
            env.mons_alloc[9] = 49;
            return;
        }

    }
// colour of monster 9 is colour of floor, 8 is colour of rock
    //  IIRC, BLACK is set to LIGHTGRAY


    for (pc = 0; pc < 10; pc++)
    {

        switch (random2(17))
        {
        case 10:
        case 0:
            env.mons_alloc[pc] = 220;
            break;
        case 11:
        case 1:
            env.mons_alloc[pc] = 221;
            break;
        case 12:
        case 2:
            env.mons_alloc[pc] = 222;
            break;
        case 13:
        case 3:
            env.mons_alloc[pc] = 223;
            break;
        case 14:
        case 4:
            env.mons_alloc[pc] = 224;
            break;

        case 5:
            env.mons_alloc[pc] = 225;
            break;
        case 6:
            env.mons_alloc[pc] = 226;
            break;
        case 7:
            env.mons_alloc[pc] = 227;
            break;
        case 8:
            env.mons_alloc[pc] = 228;
            break;
        case 9:
            env.mons_alloc[pc] = 229;
            break;

        case 15:
            env.mons_alloc[pc] = 23;
            break;
        case 16:
            env.mons_alloc[pc] = 49;
            break;
        }

        if ( one_chance_in(10) )
            env.mons_alloc[pc] = 80 + random2(10);
        if ( one_chance_in(30) )
            env.mons_alloc[pc] = 3;
        if ( one_chance_in(30) )
            env.mons_alloc[pc] = 8;
        if ( one_chance_in(20) )
            env.mons_alloc[pc] = 235 + random2(5);

    }

    if ( one_chance_in(8) )
        env.mons_alloc[7] = 230 + random2(5);
    if ( one_chance_in(5) )
        env.mons_alloc[8] = 230 + random2(5);
    if ( one_chance_in(3) )
        env.mons_alloc[9] = 230 + random2(5);

    if ( one_chance_in(10) )
        env.mons_alloc[7 + random2(3)] = 31;
    if ( one_chance_in(10) )
        env.mons_alloc[7 + random2(3)] = 126;
    if ( one_chance_in(10) )
        env.mons_alloc[7 + random2(3)] = 127;
    if ( one_chance_in(10) )
        env.mons_alloc[7 + random2(3)] = 245;

// set at least some specific monsters for the special levels - this
    //  can also be used to set some colours
}

void pandemonium_mons(void)
{
// must leave allowance for monsters rare on pandemonium (eg wizards etc)

    int pan_mons = env.mons_alloc[random2(10)];

    if ( one_chance_in(40) )
    {
        do
        {
            pan_mons = random2(400);
        }
        while (mons_pan(pan_mons) == 0);
    }

    mons_place(pan_mons, 0, 50, 50, 1, MHITNOT, 250, 52);
}

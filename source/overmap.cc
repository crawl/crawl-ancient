/*
 *  File:       overmap.cc
 *  Idea:       allow player to make notes about levels. I don't know how
 *              to do this (I expect it will require some kind of dynamic
 *              memory management thing). - LH
 *  Summary:    Records location of stairs etc
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 *      <3>      30/7/00        MV      Made Over-map full-screen
 *      <2>      8/10/99        BCR     Changed Linley's macros
 *                                      to an enum in overmap.h
 *      <1>      29/8/99        LRH     Created
 */

#include "AppHdr.h"
#include "overmap.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "globals.h"
#include "externs.h"

// for #definitions of MAX_BRANCHES & MAX_LEVELS
#include "files.h"

#include "misc.h"
#include "religion.h"
#include "stuff.h"
#include "view.h"

enum feature_type
{
     NO_FEATURE          = 0x00,
     FEATURE_SHOP        = 0x01,
     FEATURE_LABYRINTH   = 0x02,
     FEATURE_HELL        = 0x04,
     FEATURE_ABYSS       = 0x08,
     FEATURE_PANDEMONIUM = 0x10
};

// These variables need to become part of the player struct
// and need to be stored in the .sav file:

//   0 == no altars;
// 100 == more than one altar; or
//   # == which god for remaining numbers.

static void print_one_simple_line( const char *line, int colour );
static void print_one_highlighted_line( int branch, int depth,
                                        const char *pre, const char *text,
                                        const char *post, int colour );

static void print_level_name( int branch, int depth, bool continued );

//mv: number of lines already printed on "over-map" screen
static int   Map_Lines = 0;
static bool  Printed_Branch = false;
static bool  Printed_Depth = false;

static void seen_staircase( int which_staircase )
{
    // which_staircase holds the grid value of the stair, must be converted
    // Only handles stairs, not gates or arches
    // Don't worry about:
    //   - stairs returning to dungeon - predictable
    //   - entrances to the hells - always in vestibule
    int which_branch = BRANCH_MAIN_DUNGEON;

    switch (which_staircase)
    {
    case DNGN_ENTER_ORCISH_MINES:
        which_branch = BRANCH_ORCISH_MINES;
        break;
    case DNGN_ENTER_HIVE:
        which_branch = BRANCH_HIVE;
        break;
    case DNGN_ENTER_LAIR:
        which_branch = BRANCH_LAIR;
        break;
    case DNGN_ENTER_SLIME_PITS:
        which_branch = BRANCH_SLIME_PITS;
        break;
    case DNGN_ENTER_VAULTS:
        which_branch = BRANCH_VAULTS;
        break;
    case DNGN_ENTER_CRYPT:
        which_branch = BRANCH_CRYPT;
        break;
    case DNGN_ENTER_HALL_OF_BLADES:
        which_branch = BRANCH_HALL_OF_BLADES;
        break;
    case DNGN_ENTER_ZOT:
        which_branch = BRANCH_HALL_OF_ZOT;
        break;
    case DNGN_ENTER_TEMPLE:
        which_branch = BRANCH_ECUMENICAL_TEMPLE;
        break;
    case DNGN_ENTER_SNAKE_PIT:
        which_branch = BRANCH_SNAKE_PIT;
        break;
    case DNGN_ENTER_ELVEN_HALLS:
        which_branch = BRANCH_ELVEN_HALLS;
        break;
    case DNGN_ENTER_TOMB:
        which_branch = BRANCH_TOMB;
        break;
    case DNGN_ENTER_SWAMP:
        which_branch = BRANCH_SWAMP;
        break;
    case DNGN_ENTER_DIS:
        which_branch = BRANCH_DIS;
        break;
    case DNGN_ENTER_GEHENNA:
        which_branch = BRANCH_GEHENNA;
        break;
    case DNGN_ENTER_COCYTUS:
        which_branch = BRANCH_COCYTUS;
        break;
    case DNGN_ENTER_TARTARUS:
        which_branch = BRANCH_TARTARUS;
        break;
    default:
        break;
    }

    ASSERT( which_branch != BRANCH_MAIN_DUNGEON );

    env.stair_level[which_branch] = you.depth;
}          // end seen_staircase()


// if player has seen an altar; record it
static void seen_altar( int which_altar )
{
    // portable; no point in recording
    if (which_altar == GOD_NEMELEX_XOBEH)
        return;

    // already seen
    if (env.altars_present[you.depth][you.where_are_you] == which_altar)
        return;

    if (env.altars_present[you.depth][you.where_are_you] == 0)
        env.altars_present[you.depth][you.where_are_you] = which_altar;
    else
        env.altars_present[you.depth][you.where_are_you] = 100;
}          // end seen_altar()


// if player has seen any other thing; record it
static void seen_other_thing( int which_thing )
{
    switch (which_thing)
    {
    case DNGN_ENTER_SHOP:
        env.feature[you.depth][you.where_are_you] |= FEATURE_SHOP;
        break;
    case DNGN_ENTER_LABYRINTH:
        env.feature[you.depth][you.where_are_you] |= FEATURE_LABYRINTH;
        break;
    case DNGN_ENTER_HELL:
        env.feature[you.depth][you.where_are_you] |= FEATURE_HELL;
        break;
    case DNGN_ENTER_ABYSS:
        env.feature[you.depth][you.where_are_you] |= FEATURE_ABYSS;
        break;
    case DNGN_ENTER_PANDEMONIUM:
        env.feature[you.depth][you.where_are_you] |= FEATURE_PANDEMONIUM;
        break;
    }
}          // end seen_other_thing()


void seen_notable_thing( int which_thing )
{
    // Don't record in tempary terrains (labyrinth, abyss, pandemonium)
    if (you.level_type != LEVEL_DUNGEON)
        return;

    const god_type god = grid_altar_god( static_cast<unsigned char>(which_thing) );

    if (god != GOD_NO_GOD)
        seen_altar( god );
    else if (grid_is_branch_stairs( which_thing ))
        seen_staircase( which_thing );
    else
        seen_other_thing( which_thing );
}

void init_overmap( void )
{
    for (int i = 0; i < MAX_LEVELS; i++)
    {
        for (int j = 0; j < MAX_BRANCHES; j++)
        {
            env.altars_present[i][j] = 0;
            env.feature[i][j] = 0;
        }
    }

    for (int i = 0; i < MAX_BRANCHES; i++)
        env.stair_level[i] = -1;
}          // end init_overmap()


static void more_overmap( int branch = -1, int depth = -1 )
{
    int keypress;

    gotoxy( 2, get_number_of_lines() - 1 );

    textcolor( LIGHTGREY );
    cprintf( EOL );
    cprintf( MORE_STRING );

    do
    {
        keypress = getch();
    }
    while (keypress != ' ' && keypress != '\r' && keypress != '\n'
            && keypress != ESCAPE);

    clrscr();

    // start new page:
    Map_Lines = 0;

    if (branch != -1 && depth != -1)
    {
        bool continued = (Printed_Branch || Printed_Depth);

        Printed_Branch = false;
        Printed_Depth = false;

        print_level_name( branch, depth, continued );
    }
}


void display_overmap( void )
{
#ifdef DOS_TERM
    char buffer[4800];

    window(1, 1, 80, 25);
    gettext(1, 1, 80, 25, buffer);
#endif

    //mv: must be set to 0 so "More..." message appears really at the
    // bottom of the screen
    //Don't forget it could be changed since the last call of display_overmap
    Map_Lines = 0;

    clrscr();
    bool pr_lev = false;
    bool output = false;

    print_one_simple_line("                            Overview of the Dungeon", WHITE);

    // This is a more sensible order than the order of the enums -- bwr
    const int list_order[] =
    {
        BRANCH_MAIN_DUNGEON,
        BRANCH_ECUMENICAL_TEMPLE,
        BRANCH_ORCISH_MINES, BRANCH_ELVEN_HALLS,
        BRANCH_LAIR, BRANCH_SWAMP, BRANCH_SLIME_PITS, BRANCH_SNAKE_PIT,
        BRANCH_HIVE,
        BRANCH_VAULTS, BRANCH_HALL_OF_BLADES, BRANCH_CRYPT, BRANCH_TOMB,
        BRANCH_VESTIBULE_OF_HELL,
        BRANCH_DIS, BRANCH_GEHENNA, BRANCH_COCYTUS, BRANCH_TARTARUS,
        BRANCH_HALL_OF_ZOT
    };

    const unsigned int num_branches = sizeof( list_order ) / sizeof( int );

    for (unsigned int index = 0; index < num_branches; index++)
    {
        const int i = list_order[index];
        Printed_Branch = false;

        for (int j = 0; j < MAX_LEVELS; j++)
        {
            Printed_Depth = false;

            if (env.altars_present[j][i] != 0)
            {
                output = true;

                if (env.altars_present[j][i] == 100)
                {
                    print_one_highlighted_line( i, j, "    - some ",
                                                "altars to the gods", ".",
                                                WHITE );
                }
                else
                {
                    snprintf( info, INFO_SIZE, "altar to %s",
                              god_name( env.altars_present[j][i] ) );

                    print_one_highlighted_line( i, j, "    - an ", info, ".", WHITE );
                }
            }

            if (env.feature[j][i] & FEATURE_SHOP)
            {
                print_one_highlighted_line( i, j, "    - facilities for the ",
                                            "purchase of goods", ".", LIGHTGREEN );
                output = true;
            }

            if (env.feature[j][i] & FEATURE_ABYSS)
            {
                print_one_highlighted_line( i, j, "    - a gateway into ",
                                            "the Abyss", ".", MAGENTA );
                output = true;
            }

            if (env.feature[j][i] & FEATURE_PANDEMONIUM)
            {
                print_one_highlighted_line( i, j, "    - a link to ", "Pandemonium",
                                            ".", LIGHTBLUE );
                output = true;
            }

            if (env.feature[j][i] & FEATURE_HELL)
            {
                print_one_highlighted_line( i, j, "    - a mouth of ", "Hell", ".", RED );
                output = true;
            }

            if (env.feature[j][i] & FEATURE_LABYRINTH)
            {
                print_one_highlighted_line( i, j, "    - an entrance to ",
                                            "a Labyrinth", ".", CYAN );
                output = true;
            }


            // NB: k starts at 1 because there aren't any staircases
            // to the main dungeon
            for (int k = 1; k < MAX_BRANCHES; k++)
            {
                pr_lev = false;
                // strcpy(info, "    - a staircase leading to ");
                info[0] = '\0';

                if (env.stair_level[k] == j)
                {
                    switch (i)
                    {
                    case BRANCH_LAIR:
                        switch (k)
                        {
                        case BRANCH_SLIME_PITS:
                            strcat(info, "the Slime Pits");
                            pr_lev = true;
                            break;
                        case BRANCH_SNAKE_PIT:
                            strcat(info, "the Snake Pit");
                            pr_lev = true;
                            break;
                        case BRANCH_SWAMP:
                            strcat(info, "the Swamp");
                            pr_lev = true;
                            break;
                        }
                        break;

                    case BRANCH_VAULTS:
                        switch (k)
                        {
                        case BRANCH_HALL_OF_BLADES:
                            strcat(info, "the Hall of Blades");
                            pr_lev = true;
                            break;
                        case BRANCH_CRYPT:
                            strcat(info, "the Crypt");
                            pr_lev = true;
                            break;
                        }
                        break;

                    case BRANCH_CRYPT:
                        switch (k)
                        {
                        case BRANCH_TOMB:
                            strcat(info, "the Tomb");
                            pr_lev = true;
                            break;
                        }
                        break;

                    case BRANCH_ORCISH_MINES:
                        switch (k)
                        {
                        case BRANCH_ELVEN_HALLS:
                            strcat(info, "the Elven Halls");
                            pr_lev = true;
                            break;
                        }
                        break;

                    case BRANCH_MAIN_DUNGEON:
                        switch (k)
                        {
                        case BRANCH_ORCISH_MINES:
                            strcat(info, "the Orcish Mines");
                            pr_lev = true;
                            break;
                        case BRANCH_HIVE:
                            strcat(info, "the Hive");
                            pr_lev = true;
                            break;
                        case BRANCH_LAIR:
                            strcat(info, "the Lair");
                            pr_lev = true;
                            break;
                        case BRANCH_VAULTS:
                            strcat(info, "the Vaults");
                            pr_lev = true;
                            break;
                        case BRANCH_HALL_OF_ZOT:
                            strcat(info, "the Hall of Zot");
                            pr_lev = true;
                            break;
                        case BRANCH_ECUMENICAL_TEMPLE:
                            strcat(info, "the Ecumenical Temple");
                            pr_lev = true;
                            break;
                        }
                        break;

                    case BRANCH_VESTIBULE_OF_HELL:
                        switch (k)
                        {
                        case BRANCH_DIS:
                            strcat(info, "The Iron City of Dis");
                            pr_lev = true;
                            break;
                        case BRANCH_GEHENNA:
                            strcat(info, "Gehenna");
                            pr_lev = true;
                            break;
                        case BRANCH_COCYTUS:
                            strcat(info, "Cocytus");
                            pr_lev = true;
                            break;
                        case BRANCH_TARTARUS:
                            strcat(info, "Tartarus");
                            pr_lev = true;
                            break;
                        }
                        break;
                    }
                }

                if (pr_lev)
                {
                    print_one_highlighted_line( i, j, "    - the entrance to ", info,
                                                ".", YELLOW );
                    output = true;
                }
            }
        }
    }

    textcolor( LIGHTGREY );

    if (!output)
        cprintf( EOL "You have yet to discover anything worth noting." EOL );

    more_overmap();
    redraw_screen();

#ifdef DOS_TERM
    puttext( 1, 1, 80, 25, buffer );
#endif
}          // end display_overmap()


static void print_level_name( int branch, int depth, bool continued )
{
    // we need our own buffer in here (info is used):
    char buff[ INFO_SIZE ] = "";

    if (!Printed_Branch && Map_Lines >= (get_number_of_lines() - 5))
        more_overmap( branch, depth );

    if (!Printed_Branch)
    {

        Printed_Branch = true;

        print_one_simple_line( "", YELLOW );

        snprintf( buff, sizeof( buff ), "%s%s",
                    (branch == BRANCH_MAIN_DUNGEON)      ? "Main Dungeon" :
                    (branch == BRANCH_ORCISH_MINES)      ? "The Orcish Mines" :
                    (branch == BRANCH_HIVE)              ? "The Hive" :
                    (branch == BRANCH_LAIR)              ? "The Lair" :
                    (branch == BRANCH_SLIME_PITS)        ? "The Slime Pits" :
                    (branch == BRANCH_VAULTS)            ? "The Vaults" :
                    (branch == BRANCH_CRYPT)             ? "The Crypt" :
                    (branch == BRANCH_HALL_OF_BLADES)    ? "The Hall of Blades" :
                    (branch == BRANCH_HALL_OF_ZOT)       ? "The Hall of Zot" :
                    (branch == BRANCH_ECUMENICAL_TEMPLE) ? "The Ecumenical Temple" :
                    (branch == BRANCH_SNAKE_PIT)         ? "The Snake Pit" :
                    (branch == BRANCH_ELVEN_HALLS)       ? "The Elven Halls" :
                    (branch == BRANCH_TOMB)              ? "The Tomb" :
                    (branch == BRANCH_SWAMP)             ? "The Swamp" :

                    (branch == BRANCH_DIS)               ? "The Iron City of Dis" :
                    (branch == BRANCH_GEHENNA)           ? "Gehenna" :
                    (branch == BRANCH_VESTIBULE_OF_HELL) ? "The Vestibule of Hell" :
                    (branch == BRANCH_COCYTUS)           ? "Cocytus" :
                    (branch == BRANCH_TARTARUS)          ? "Tartarus"
                                                         : "Unknown Area",
                    (continued) ? " (continued)" : "" );

        print_one_simple_line( buff, YELLOW );
    }

    if (!Printed_Depth && Map_Lines >= (get_number_of_lines() - 3))
        more_overmap( branch, depth );

    if (!Printed_Depth)
    {
        if (Map_Lines >= (get_number_of_lines() - 3))
            more_overmap( branch, depth );

        Printed_Depth = true;

        // these areas only have one level... let's save the space
        if (branch != BRANCH_ECUMENICAL_TEMPLE
            && branch != BRANCH_HALL_OF_BLADES
            && branch != BRANCH_VESTIBULE_OF_HELL)
        {
            if (branch == BRANCH_MAIN_DUNGEON)
                depth += 1;
            else if (branch >= BRANCH_ORCISH_MINES && branch <= BRANCH_SWAMP)
                depth -= you.branch_stairs[ branch - BRANCH_ORCISH_MINES ];
            else // branch is in hell (all of which start at depth 28)
                depth -= 26;

            snprintf( buff, INFO_SIZE, "  Level %d:", depth );
            print_one_simple_line( buff, LIGHTRED );
        }
    }
}


// This function should only be used for headers which have already
// been checked to see if there is enough room.
static void print_one_simple_line( const char *line, int colour )
{
    ASSERT( Map_Lines < get_number_of_lines() - 2 );

    textcolor( colour );
    cprintf( line );
    cprintf( EOL );

    Map_Lines++;
}

static void print_one_highlighted_line( int branch, int depth,
                                        const char *pre, const char *text,
                                        const char *post, int colour )
{
    if (Map_Lines == (get_number_of_lines() - 2))
        more_overmap( branch, depth );
    else
        print_level_name( branch, depth, false );

    if (pre[0] != '\0')
    {
        textcolor( LIGHTGREY );
        cprintf( pre );
    }

    textcolor( colour );
    cprintf( text );

    if (post[0] != '\0')
    {
        textcolor( LIGHTGREY );
        cprintf( post );
    }

    cprintf( EOL );

    Map_Lines++;
}

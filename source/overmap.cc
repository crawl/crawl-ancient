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

#include "externs.h"

// for #definitions of MAX_BRANCHES & MAX_LEVELS
#include "files.h"
// for #definitions of MAX_BRANCHES & MAX_LEVELS
#include "religion.h"
#include "stuff.h"
#include "view.h"

enum
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

FixedArray<unsigned char, MAX_LEVELS, MAX_BRANCHES> altars_present;
FixedVector<char, MAX_BRANCHES> stair_level;
FixedArray<unsigned char, MAX_LEVELS, MAX_BRANCHES> feature;

int map_lines = 0; //mv: number of lines already printed on "over-map" screen

//mv: prints one line in specified color
void print_one_map_line( const char *line, int color );

static bool print_level_name(int i, int j, bool already_printed);

void init_overmap( void )
{
    for (int i = 0; i < MAX_LEVELS; i++)
      for (int j = 0; j < MAX_BRANCHES; j++)
      {
          altars_present[i][j] = 0;
          feature[i][j] = 0;
      }

    for (int i = 0; i < MAX_BRANCHES; i++)
      stair_level[i] = -1;
}          // end init_overmap()

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
    map_lines = 0;

    clrscr();
    bool already_printed = false;
    bool pr_lev = false;

    print_one_map_line("                                    Over-map", WHITE);
    print_one_map_line("",LIGHTGREY);
    //mv: this empty line must be done this way
    //so number of line stored in map_lines fit

    for (int i = 0; i < MAX_BRANCHES; i++)
    {
        for (int j = 0; j < MAX_LEVELS; j++)
        {
            already_printed = false;

            if (altars_present[j][i] != 0)
            {
                already_printed = print_level_name(i, j, already_printed);

                if (altars_present[j][i] == 100)
                    print_one_map_line("  - some altars to the gods.", LIGHTGREY);
                else
                {
                    strcpy(info, "  - an altar to ");
                    strcat(info, god_name(altars_present[j][i]));
                    strcat(info, ".");
                    print_one_map_line(info,LIGHTGREY);
                }
            }

            if ( (feature[j][i] & FEATURE_SHOP) )
            {
                already_printed = print_level_name(i, j, already_printed);
                print_one_map_line("  - facilities for the purchase of goods.",LIGHTGREY);
            }

            if ( (feature[j][i] & FEATURE_ABYSS) )
            {
                already_printed = print_level_name(i, j, already_printed);
                print_one_map_line("  - a gateway into the Abyss.", LIGHTRED);
            }

            if ( (feature[j][i] & FEATURE_PANDEMONIUM) )
            {
                already_printed = print_level_name(i, j, already_printed);
                print_one_map_line("  - a link to Pandemonium.", LIGHTRED);
            }

            if ( (feature[j][i] & FEATURE_HELL) )
            {
                already_printed = print_level_name(i, j, already_printed);
                print_one_map_line("  - a mouth of Hell.", LIGHTRED);
            }

            if ( (feature[j][i] & FEATURE_LABYRINTH) )
            {
                already_printed = print_level_name(i, j, already_printed);
                print_one_map_line("  - the entrance of a Labyrinth.", LIGHTRED);
            }


            // NB: k starts at 1 because there aren't any staircases
            // to the main dungeon
            for (int k = 1; k < MAX_BRANCHES; k++)
            {
                pr_lev = false;
                strcpy(info, "  - a staircase leading to ");

                if ( stair_level[k] == j )
                    switch ( i )
                    {
                    case BRANCH_LAIR:
                        switch (k)
                        {
                        case BRANCH_SLIME_PITS:
                            strcat(info, "the Slime Pits.");
                            pr_lev = true;
                            break;
                        case BRANCH_SNAKE_PIT:
                            strcat(info, "the Snake Pit.");
                            pr_lev = true;
                            break;
                        case BRANCH_SWAMP:
                            strcat(info, "the Swamp.");
                            pr_lev = true;
                            break;
                        }
                        break;

                    case BRANCH_VAULTS:
                        switch (k)
                        {
                        case BRANCH_HALL_OF_BLADES:
                            strcat(info, "the Hall of Blades.");
                            pr_lev = true;
                            break;
                        case BRANCH_CRYPT:
                            strcat(info, "the Crypt.");
                            pr_lev = true;
                            break;
                        }
                        break;

                    case BRANCH_CRYPT:
                        switch (k)
                        {
                        case BRANCH_TOMB:
                            strcat(info, "the Tomb.");
                            pr_lev = true;
                            break;
                        }
                        break;

                    case BRANCH_ORCISH_MINES:
                        switch (k)
                        {
                        case BRANCH_ELVEN_HALLS:
                            strcat(info, "the Elven Halls.");
                            pr_lev = true;
                            break;
                        }
                        break;

                    case BRANCH_MAIN_DUNGEON:
                        switch (k)
                        {
                        case BRANCH_ORCISH_MINES:
                            strcat(info, "the Orcish Mines.");
                            pr_lev = true;
                            break;
                        case BRANCH_HIVE:
                            strcat(info, "the Hive.");
                            pr_lev = true;
                            break;
                        case BRANCH_LAIR:
                            strcat(info, "the Lair.");
                            pr_lev = true;
                            break;
                        case BRANCH_VAULTS:
                            strcat(info, "the Vaults.");
                            pr_lev = true;
                            break;
                        case BRANCH_HALL_OF_ZOT:
                            strcat(info, "the Hall of Zot.");
                            pr_lev = true;
                            break;
                        case BRANCH_ECUMENICAL_TEMPLE:
                            strcat(info, "the Ecumenical Temple.");
                            pr_lev = true;
                            break;
                        }
                        break;
                    }

                if ( pr_lev )
                {
                    already_printed = print_level_name(i, j, already_printed);
                    print_one_map_line(info, YELLOW);

                }
            }
        }
    }

    getch();

    redraw_screen();

#ifdef DOS_TERM
    puttext(1, 1, 80, 25, buffer);
#endif
}          // end display_overmap()


static bool print_level_name( int i, int j, bool already_printed )
{
    if ( already_printed )
        return true;

    char level_name[100];
    char temp_quant[5];

    strcpy(level_name, "Level ");

    itoa(j + 1, temp_quant, 10);

    // replaces lengthy switch 11mar2000 {dlb}
    if ( i >= BRANCH_ORCISH_MINES && i <= BRANCH_SWAMP )
        itoa(j - you.branch_stairs[(i - 10)], temp_quant, 10);

    strcat(level_name, temp_quant);

    strcat(level_name, (i == BRANCH_ORCISH_MINES)      ? " of the Mines" :
                       (i == BRANCH_HIVE)              ? " of the Hive" :
                       (i == BRANCH_LAIR)              ? " of the Lair" :
                       (i == BRANCH_SLIME_PITS)        ? " of the Slime Pits" :
                       (i == BRANCH_VAULTS)            ? " of the Vaults" :
                       (i == BRANCH_CRYPT)             ? " of the Crypt" :
                       (i == BRANCH_HALL_OF_BLADES)    ? " of the Hall of Blades" :
                       (i == BRANCH_HALL_OF_ZOT)       ? " of the Hall of Zot" :
                       (i == BRANCH_ECUMENICAL_TEMPLE) ? " of the Temple" :
                       (i == BRANCH_SNAKE_PIT)         ? " of the Snake Pit" :
                       (i == BRANCH_ELVEN_HALLS)       ? " of the Elven Halls" :
                       (i == BRANCH_TOMB)              ? " of the Tomb" :
                       (i == BRANCH_SWAMP)             ? " of the Swamp"
                                                       : "" );

    strcat(level_name, " :");
    print_one_map_line(level_name, LIGHTGREY);

    return true;
}

void seen_staircase( unsigned char which_staircase )
{
    // which_staircase holds the grid value of the stair, must be converted
    // Only handles stairs, not gates or arches
    // Don't worry about:
    //   - stairs returning to dungeon - predictable
    //   - entrances to the hells - always in vestibule

    unsigned char which_branch = BRANCH_MAIN_DUNGEON;

    switch ( which_staircase )
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
    default:
        exit(-1);               // shouldn't happen
    }

    stair_level[which_branch] = you.your_level;
}          // end seen_staircase()


// if player has seen an altar; record it
void seen_altar( unsigned char which_altar )
{
    // can't record in abyss or pan.
    if ( you.level_type != LEVEL_DUNGEON )
        return;

    // portable; no point in recording
    if ( which_altar == GOD_NEMELEX_XOBEH )
        return;

    // already seen
    if ( altars_present[you.your_level][you.where_are_you] == which_altar )
        return;

    if ( altars_present[you.your_level][you.where_are_you] == 0 )
        altars_present[you.your_level][you.where_are_you] = which_altar;
    else
        altars_present[you.your_level][you.where_are_you] = 100;
}          // end seen_altar()


// if player has seen any other thing; record it
void seen_other_thing( unsigned char which_thing )
{
    if ( you.level_type != LEVEL_DUNGEON )     // can't record in abyss or pan.
        return;

    switch ( which_thing )
    {
    case DNGN_ENTER_SHOP:
        feature[you.your_level][you.where_are_you] |= FEATURE_SHOP;
        break;
    case DNGN_ENTER_LABYRINTH:
        feature[you.your_level][you.where_are_you] |= FEATURE_LABYRINTH;
        break;
    case DNGN_ENTER_HELL:
        feature[you.your_level][you.where_are_you] |= FEATURE_HELL;
        break;
    case DNGN_ENTER_ABYSS:
        feature[you.your_level][you.where_are_you] |= FEATURE_ABYSS;
        break;
    case DNGN_ENTER_PANDEMONIUM:
        feature[you.your_level][you.where_are_you] |= FEATURE_PANDEMONIUM;
        break;
    }
}          // end seen_other_thing()


/* mv: this function prints one line at "Over-map screen" in specified color.
 * If map_lines = maximum number of lines (it means the screen is full) it
 * prints "More..." message, read key, clear screen and after that prints new
 * line
 */
void print_one_map_line( const char *line , int color)
{
    if (map_lines == (get_number_of_lines() - 2))
    {
        textcolor (LIGHTGREY);
        cprintf(EOL);
        cprintf("More...");
        getch();
        clrscr();
        map_lines = 0;
    }

    textcolor (color);
    cprintf(line);
    cprintf(EOL);

    map_lines++;

    return;
} // end print_one_map_line()

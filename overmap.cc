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


enum
{
     NO_FEATURE = 0,             // clear
     FEATURE_SHOP = 1,           // (1<<0)
     FEATURE_LABYRINTH = 2,      // (1<<1)
     FEATURE_HELL = 4,           // (1<<2)
     FEATURE_ABYSS = 8,          // (1<<3)
     FEATURE_PANDEMONIUM = 16    // (1<<4)
};


// These variables need to become part of the player struct
// and need to be stored in the .sav file:


//   0 == no altars;
// 100 == more than one altar; or
//   # == which god for remaining numbers.
unsigned char altars_present[MAX_LEVELS][MAX_BRANCHES];
char stair_level[MAX_BRANCHES];
unsigned char feature[MAX_LEVELS][MAX_BRANCHES];


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

// just use the message window for now

    bool already_printed = false;
    bool pr_lev = false;

    mpr("You cast your mind back along this journey...");

    for (int i = 0; i < MAX_BRANCHES; i++)
    {
        for (int j = 0; j < MAX_LEVELS; j++)
        {
            already_printed = false;

            if (altars_present[j][i] != 0)
            {
                already_printed = print_level_name(i, j, already_printed);

                if (altars_present[j][i] == 100)
                    mpr(" - some altars to the gods.");
                else
                {
                    strcpy(info, " - an altar to ");
                    strcat(info, god_name(altars_present[j][i]));
                    strcat(info, ".");
                    mpr(info);
                }
            }

            if ( (feature[j][i] & FEATURE_SHOP) )
            {
                already_printed = print_level_name(i, j, already_printed);
                mpr(" - facilities for the purchase of goods.");
            }

            if ( (feature[j][i] & FEATURE_ABYSS) )
            {
                already_printed = print_level_name(i, j, already_printed);
                mpr(" - a way into the Abyss.");
            }

            if ( (feature[j][i] & FEATURE_PANDEMONIUM) )
            {
                already_printed = print_level_name(i, j, already_printed);
                mpr(" - a link to Pandemonium.");
            }

            if ( (feature[j][i] & FEATURE_HELL) )
            {
                already_printed = print_level_name(i, j, already_printed);
                mpr(" - a mouth of Hell.");
            }

            if ( (feature[j][i] & FEATURE_LABYRINTH) )
            {
                already_printed = print_level_name(i, j, already_printed);
                mpr(" - the entrance of a Labyrinth.");
            }


            for (int k = 1; k < MAX_BRANCHES; k++)      // NB: k starts at 1 because there aren't any staircases to the main dungeon
            {
                pr_lev = false;
                strcpy(info, " - a staircase leading to ");

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
                    mpr(info);  // aren't I clever? Seriously, though, this is terrible. Right?

                }

            }

        }

    }

}          // end display_overmap()




static bool print_level_name( int i, int j, bool already_printed )
{

    if ( already_printed )
      return true;

    char level_name[100];
    char temp_quant[5];

    strcpy(level_name, "On level ");

    itoa(j + 1, temp_quant, 10);

    if ( i >= BRANCH_ORCISH_MINES && i <= BRANCH_SWAMP )
      itoa(j - you.branch_stairs[(i - 10)], temp_quant, 10);     // replaces lengthy switch 11mar2000 {dlb}

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

    strcat(level_name, ":");

    mpr(level_name);

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
      case DNGN_ENTER_LAIR_I:
        which_branch = BRANCH_LAIR;
        break;
      case DNGN_ENTER_SLIME_PITS:
        which_branch = BRANCH_SLIME_PITS;
        break;
      case DNGN_ENTER_VAULTS:
        which_branch = BRANCH_VAULTS;
        break;
      case DNGN_ENTER_CRYPT_I:
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

    if ( you.level_type != LEVEL_DUNGEON )     // can't record in abyss or pan.
      return;

    if ( which_altar == GOD_NEMELEX_XOBEH )     // portable; no point in recording
      return;

    if ( altars_present[you.your_level][you.where_are_you] == which_altar )     // already seen
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

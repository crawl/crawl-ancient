/*
 *  File:       overmap.cc
 *  Summary:    Records location of stairs etc
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 *      <2>      8/10/99        BCR     Changed Linley's macros
 *                                      to an enum in overmap.h
 *      <1>      29/8/99        LRH     Created
 */

/* BCR This is kinda silly, because each time these macros are
       used, they each have to be recomputed.  See my replacement
       in overmap.h.

#define NO_FEATURE 0 // clear
#define FEATURE_SHOP (1<<0)
#define FEATURE_LABYRINTH (1<<1)
#define FEATURE_HELL (1<<2)
#define FEATURE_ABYSS (1<<3)
#define FEATURE_PANDEMONIUM (1<<4)
*/

#include "AppHdr.h"

#include <string.h>
#include <stdlib.h>

#include "externs.h"
#include "enum.h"
#include "files.h" // for #definitions of MAX_BRANCHES & MAX_LEVELS
#include "message.h"
#include "overmap.h"
#include "religion.h"
#include "stuff.h"

/*
Idea: allow player to make notes about levels. I don't know how to do
this (I expect it will require some kind of dynamic memory management
thing). - LH
*/


// These variables need to become part of the player struct and need to
//  be saved in the .sav file:
unsigned char altars_present [MAX_LEVELS] [MAX_BRANCHES];
/* 0 means no altars; 100 means more than one; otherwise, which god */
char stair_level [MAX_BRANCHES];

unsigned char feature [MAX_LEVELS] [MAX_BRANCHES];

void init_overmap(void)
{

 int i = 0;
 int j = 0;
 for (i = 0; i < MAX_LEVELS; i ++)
 {
  for (j = 0; j < MAX_BRANCHES; j ++)
  {
   altars_present [i] [j] = 0;
   feature [i] [j] = 0;
  }
 }
 for (i = 0; i < MAX_BRANCHES; i ++)
 {
  stair_level [i] = -1;
 }

}


void display_overmap(void)
{
// just use the message window for now

int i = 0;
int j = 0;
int k = 0;

char already_printed = 0;
char pr_lev = 0;

mpr("You cast your mind back along this journey...");

for (i = 0; i < MAX_BRANCHES; i ++)
{
 for (j = 0; j < MAX_LEVELS; j ++)
 {
  already_printed = 0;

  if (altars_present [j] [i] != 0)
  {
        already_printed = print_level_name(i, j, already_printed);
    if (altars_present [j] [i] == 100)
      mpr(" - some altars to the gods.");
       else
                {
         strcpy(info, " - an altar to ");
         strcat(info, god_name(altars_present [j] [i]));
         strcat(info, ".");
         mpr(info);
        }
  }

  if ((feature [j] [i] & FEATURE_SHOP) != 0)
  {
        already_printed = print_level_name(i, j, already_printed);
    mpr(" - facilities for the purchase of goods.");
  }

  if ((feature [j] [i] & FEATURE_ABYSS) != 0)
  {
        already_printed = print_level_name(i, j, already_printed);
    mpr(" - a way into the Abyss.");
  }

  if ((feature [j] [i] & FEATURE_PANDEMONIUM) != 0)
  {
        already_printed = print_level_name(i, j, already_printed);
    mpr(" - a link to Pandemonium.");
  }

  if ((feature [j] [i] & FEATURE_HELL) != 0)
  {
        already_printed = print_level_name(i, j, already_printed);
    mpr(" - a mouth of Hell.");
  }

  if ((feature [j] [i] & FEATURE_LABYRINTH) != 0)
  {
        already_printed = print_level_name(i, j, already_printed);
    mpr(" - the entrance of a Labyrinth.");
  }


  for (k = 1; k < MAX_BRANCHES; k ++) // Note: k starts at 1 because there aren't any staircases to the main dungeon
  {
     pr_lev = 0;
     strcpy(info, " - a staircase leading to ");
     if (stair_level [k] == j)
     switch(i)
     {
          case BRANCH_LAIR:
      switch(k)
      {
       case BRANCH_SLIME_PITS: strcat(info, "the Slime Pits."); pr_lev = 1; break;
       case BRANCH_SNAKE_PIT: strcat(info, "the Snake Pit."); pr_lev = 1; break;
       case BRANCH_SWAMP: strcat(info, "the Swamp."); pr_lev = 1; break;
          }
      break;
          case BRANCH_VAULTS:
      switch(k)
      {
       case BRANCH_HALL_OF_BLADES: strcat(info, "the Hall of Blades."); pr_lev = 1; break;
       case BRANCH_CRYPT: strcat(info, "the Crypt."); pr_lev = 1; break;
          }
      break;
          case BRANCH_CRYPT:
      switch(k)
      {
       case BRANCH_TOMB: strcat(info, "the Tomb."); pr_lev = 1; break;
          }
      break;
          case BRANCH_ORCISH_MINES:
      switch(k)
      {
       case BRANCH_ELVEN_HALLS: strcat(info, "the Elven Halls."); pr_lev = 1; break;
          }
      break;
      case BRANCH_MAIN_DUNGEON:
      switch(k)
      {
       case BRANCH_ORCISH_MINES: strcat(info, "the Orcish Mines."); pr_lev = 1; break;
       case BRANCH_HIVE: strcat(info, "the Hive."); pr_lev = 1; break;
       case BRANCH_LAIR: strcat(info, "the Lair."); pr_lev = 1; break;
       case BRANCH_VAULTS: strcat(info, "the Vaults."); pr_lev = 1; break;
       case BRANCH_HALL_OF_ZOT: strcat(info, "the Hall of Zot."); pr_lev = 1; break;
       case BRANCH_ECUMENICAL_TEMPLE: strcat(info, "the Ecumenical Temple."); pr_lev = 1; break;
      }
      break;
     }
         if (pr_lev == 1)
     {
           already_printed = print_level_name(i, j, already_printed);
       mpr(info); // aren't I clever? Seriously, though, this is terrible. Right?
     }
  }

 }
}

}


char print_level_name(int i, int j, char already_printed)
{

   if (already_printed == 1) return 1;

   char level_name [100];
   char temp_quant [5];

   strcpy(level_name, "On level ");
   itoa(j + 1, temp_quant, 10);
   switch (i) // ugly, isn't it?
   {
    case 10:
        itoa(j - you.branch_stairs[0], temp_quant, 10);
        break;
    case 11:
        itoa(j - you.branch_stairs[1], temp_quant, 10);
        break;
    case 12:
        itoa(j - you.branch_stairs[2], temp_quant, 10);
        break;
    case 13:
        itoa(j - you.branch_stairs[3], temp_quant, 10);
        break;
    case 14:
        itoa(j - you.branch_stairs[4], temp_quant, 10);
        break;
    case 15:
        itoa(j - you.branch_stairs[5], temp_quant, 10);
        break;
    case 16:
        itoa(j - you.branch_stairs[6], temp_quant, 10);
        break;
    case 17:
        itoa(j - you.branch_stairs[7], temp_quant, 10);
        break;
    case 18:
        itoa(j - you.branch_stairs[8], temp_quant, 10);
        break;
    case 19:
        itoa(j - you.branch_stairs[9], temp_quant, 10);
        break;
    case 20:
        itoa(j - you.branch_stairs[10], temp_quant, 10);
        break;
    case 21:
        itoa(j - you.branch_stairs[11], temp_quant, 10);
        break;
    case 22:
        itoa(j - you.branch_stairs[12], temp_quant, 10);
        break;
    }

   strcat(level_name, temp_quant);

        switch (i)
        {
/*        case 1:
            strcat(level_name, " of Dis");
            break;
        case 2:
            strcat(level_name, " of Gehenna");
            break;
        case 3:
            strcat(level_name, " in the Vestibule");
            break;
        case 4:
            strcat(level_name, " of Cocytus");
            break;
        case 5:
            strcat(level_name, " of Tartarus");
            break;*/
        case 10:
            strcat(level_name, " of the Mines");
            break;
        case 11:
            strcat(level_name, " of the Hive");
            break;
        case 12:
            strcat(level_name, " of the Lair");
            break;
        case 13:
            strcat(level_name, " of the Slime Pits");
            break;
        case 14:
            strcat(level_name, " of the Vaults");
            break;
        case 15:
            strcat(level_name, " of the Crypt");
            break;
        case 16:
            strcat(level_name, " of the Hall of Blades");
            break;
        case 17:
            strcat(level_name, " of the Hall of Zot");
            break;
        case 18:
            strcat(level_name, " of the Temple");
            break;
        case 19:
            strcat(level_name, " of the Snake Pit");
            break;
        case 20:
            strcat(level_name, " of the Elven Halls");
            break;
        case 21:
            strcat(level_name, " of the Tomb");
            break;
        case 22:
            strcat(level_name, " of the Swamp");
            break;
        }

                strcat(level_name, ":");

                mpr(level_name);

        return 1;

}


void seen_staircase(unsigned char which_staircase)
{
/* which_staircase holds the grid value of the stair, must be converted
Only handles stairs, not gates or arches
Don't worry about:
 - stairs returning to dungeon - predictable
 - entrances to the hells - always in vestibule
*/
unsigned char which_branch = 0;

switch(which_staircase)
{
 case DNGN_ENTER_ORCISH_MINES: which_branch = BRANCH_ORCISH_MINES; break;
 case DNGN_ENTER_HIVE: which_branch = BRANCH_HIVE; break;
 case DNGN_ENTER_LAIR_I: which_branch = BRANCH_LAIR; break;
 case DNGN_ENTER_SLIME_PITS: which_branch = BRANCH_SLIME_PITS; break;
 case DNGN_ENTER_VAULTS: which_branch = BRANCH_VAULTS; break;
 case DNGN_ENTER_CRYPT_I: which_branch = BRANCH_CRYPT; break;
 case DNGN_ENTER_HALL_OF_BLADES: which_branch = BRANCH_HALL_OF_BLADES; break;
 case DNGN_ENTER_ZOT: which_branch = BRANCH_HALL_OF_ZOT; break;
 case DNGN_ENTER_TEMPLE: which_branch = BRANCH_ECUMENICAL_TEMPLE; break;
 case DNGN_ENTER_SNAKE_PIT: which_branch = BRANCH_SNAKE_PIT; break;
 case DNGN_ENTER_ELVEN_HALLS: which_branch = BRANCH_ELVEN_HALLS; break;
 case DNGN_ENTER_TOMB: which_branch = BRANCH_TOMB; break;
 case DNGN_ENTER_SWAMP: which_branch = BRANCH_SWAMP; break;
 default: exit(-1); // shouldn't happen
}

stair_level [which_branch] = you.your_level;

}


// player has seen an altar; record it.
void seen_altar(unsigned char which_altar)
{
 if (you.level_type != LEVEL_DUNGEON) return; // can't record in abyss/pan
 if (which_altar == GOD_NEMELEX_XOBEH) return; // portable; no point in recording
 if (altars_present [you.your_level] [you.where_are_you] == which_altar) return; // already seen
 if (altars_present [you.your_level] [you.where_are_you] == 0) altars_present [you.your_level] [you.where_are_you] = which_altar;
   else altars_present [you.your_level] [you.where_are_you] = 100;
}

// player has seen any other thing; record it.
void seen_other_thing(unsigned char which_thing)
{
 if (you.level_type != LEVEL_DUNGEON) return; // can't record in abyss/pan
 switch(which_thing)
 {
  case DNGN_ENTER_SHOP: feature [you.your_level] [you.where_are_you] |= FEATURE_SHOP; break;
  case DNGN_ENTER_LABYRINTH: feature [you.your_level] [you.where_are_you] |= FEATURE_LABYRINTH; break;
  case DNGN_ENTER_HELL: feature [you.your_level] [you.where_are_you] |= FEATURE_HELL; break;
  case DNGN_ENTER_ABYSS: feature [you.your_level] [you.where_are_you] |= FEATURE_ABYSS; break;
  case DNGN_ENTER_PANDEMONIUM: feature [you.your_level] [you.where_are_you] |= FEATURE_PANDEMONIUM; break;
 }
}

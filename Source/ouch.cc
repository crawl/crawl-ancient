/*
 *  File:       ouch.cc
 *  Summary:    Functions used when Bad Things happen to the player.
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 *      <8>      7/30/00        JDJ     Fixed end_game so that it works with filenames longer than 6 characters.
 *      <7>      19 June 2000   GDL     Changed handle to FILE *
 *      <6>      11/23/99       LRH     Fixed file purging for DOS?
 *      <5>      9/29/99        BCR     Fixed highscore so that it
 *                                      doesn't take so long.  Also
 *                                      added some whitespace to the scores.
 *                                      Fixed problem with uniques and 'a'.
 *      <4>      6/13/99        BWR     applied a mix of DML and my tmp
 *                                      file purging improvements.
 *      <3>      5/26/99        JDJ     highscore() will print more scores on
 *                                      larger windows.
 *      <2>      5/21/99        BWR     Added SCORE_FILE_ENTRIES, so
 *                                      that more top scores can be
 *                                      saved.
 *      <1>      -/--/--        LRH     Created
 */

#include "AppHdr.h"
#include "ouch.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
//#include <fstream.h>

#ifdef DOS
#include <conio.h>
#include <file.h>
#endif

#ifdef LINUX
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#endif

#ifdef USE_CURSES
#include <curses.h>
#endif

#ifdef USE_EMX
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#endif

#ifdef MAC
#include <stat.h>
#else
#include <sys/stat.h>
#endif

#include "externs.h"

#include "chardump.h"
#include "files.h"
#include "invent.h"
#include "itemname.h"
#include "mon-util.h"
#include "player.h"
#include "religion.h"
#include "shopping.h"
#include "skills2.h"
#include "stuff.h"

#ifdef MACROS
#include "macro.h"
#endif


extern bool wield_change;    // defined in output.cc


char death_string[256];
long points = 0;


int set_status(int stat);
void end_game(char end_status);
void highscore(char death_string[256], long points);
void item_corrode(char itco);
static char *pad(char *str);



/* NOTE: DOES NOT check for hellfire!!! */
int check_your_resists( int hurted, int flavour )
{

    switch ( flavour )
    {
      case BEAM_FIRE:
        if (player_res_fire() > 100)
        {
            canned_msg(MSG_YOU_RESIST);
            hurted /= 2 + ((player_res_fire() - 100) * (player_res_fire() - 100));
        }
        else if (player_res_fire() < 100)
        {
            mpr("It burns terribly!");
            hurted *= 15;
            hurted /= 10;
        }
        break;

      case BEAM_COLD:
        if (player_res_cold() > 100)
        {
            canned_msg(MSG_YOU_RESIST);
            hurted /= 2 + ((player_res_cold() - 100) * (player_res_cold() - 100));
        }
        else if (player_res_cold() < 100)
        {
            mpr("You feel a terrible chill!");
            hurted *= 15;
            hurted /= 10;
        }
        break;

      case BEAM_ELECTRICITY:
        if (player_res_electricity() > 0)
        {
            canned_msg(MSG_YOU_RESIST);
            hurted /= 3;
        }
        break;


      case BEAM_POISON:
        if ( player_res_poison() )
        {
            canned_msg(MSG_YOU_RESIST);
            hurted /= 3;
        }
        else
          you.poison += ( coinflip() ? 2 : 1 );
        break;

      case BEAM_NEG:
        if (player_prot_life() > 0)
        {
            hurted -= (player_prot_life() * hurted) / 3;
        }
        drain_exp();
        break;

      case BEAM_ICE:
        if (player_res_cold() > 100)
        {
            mpr("You partially resist.");
            hurted /= 2;        /* should really be more complex, but I don't have time right now */
        }
        else if (player_res_cold() < 100)
        {
            mpr("You feel a painful chill!");
            hurted *= 13;
            hurted /= 10;
        }
        break;

      case BEAM_LAVA:
        if (player_res_fire() > 100)
        {
            mpr("You partially resist.");
            hurted /= 1 + (player_res_fire() - 100);
        }
        else if (player_res_fire() < 100)
        {
            mpr("It burns terribly!");
            hurted *= 15;
            hurted /= 10;
        }
        break;
    }                           /* end switch */

    return hurted;

}          // end check_your_resists()




void splash_with_acid( char acid_strength )
{

    /* affects equip only?

       assume that a message has already been sent, also that damage has
       already been done */


    //abort();

    char splc = 0;

    for (splc = EQ_CLOAK; splc <= EQ_BODY_ARMOUR; splc++)
    {
        if ( you.equip[splc] == -1 )
        {
            ouch(random2(acid_strength) / 5, 0, KILLED_BY_BEAM);

            continue;
            /* should take extra damage if little armour worn */
        }

        if ( random2(20) <= acid_strength )
          item_corrode(you.equip[splc]);
    }

}          // end splash_with_acid()




void weapon_acid( char acid_strength )
{

    char hand_thing = you.equip[EQ_WEAPON];

    if ( you.equip[EQ_WEAPON] == -1 )
    {
        if ( you.equip[EQ_GLOVES] != -1 )
          hand_thing = you.equip[EQ_GLOVES];
        else
          return;             // take extra damage
    }

    if ( random2(20) <= acid_strength )
      item_corrode(hand_thing);

}          // end weapon_acid()




void item_corrode( char itco )
{

    int chance_corr = 0;         // no idea what its full range is {dlb}
    bool it_resists = false;     // code simplifier {dlb}
    bool suppress_msg = false;   // code simplifier {dlb}
    unsigned char how_rusty = ( (you.inv_class[itco] == OBJ_WEAPONS) ? you.inv_plus2[itco]
                                                                     : you.inv_plus[itco] );

// early return for "oRC and cloak/preservation {dlb}:
    if ( wearing_amulet(AMU_RESIST_CORROSION) && !one_chance_in(10) )
    {

#ifdef WIZARD
        mpr("Amulet protects.");
#endif

        return;
    }


// early return for items already pretty d*** rusty {dlb}:
    if ( how_rusty < 45 )
      return;


// determine possibility of resistance by object type {dlb}:
    switch ( you.inv_class[itco] )
    {
        case OBJ_ARMOUR:
            if ( you.inv_dam[itco] % 30 >= SPARM_RANDART_I )
            {
                it_resists = true;
                suppress_msg = true;
            }
            else if ( ( you.inv_type[itco] == ARM_CRYSTAL_PLATE_MAIL
                         || you.inv_dam[itco] / 30 == DARM_DWARVEN ) && !one_chance_in(5) )
            {
                it_resists = true;
                suppress_msg = false;
            }
            break;

        case OBJ_WEAPONS:
            if ( you.inv_dam[itco] >= NWPN_SINGING_SWORD || you.inv_dam[itco] % 30 >= SPWPN_RANDART_I )
            {
                it_resists = true;
                suppress_msg = true;
            }
            else if ( you.inv_dam[itco] / 30 == DWPN_DWARVEN && !one_chance_in(5) )
            {
                it_resists = true;
                suppress_msg = false;
            }
            break;

        case OBJ_MISSILES:
            if ( you.inv_dam[itco] / 30 == DAMMO_DWARVEN && !one_chance_in(5) )
            {
                it_resists = true;
                suppress_msg = false;
            }
            break;
    }


// determine chance of corrosion {dlb}:
    if ( !it_resists )
    {
        chance_corr = how_rusty;

        // the following may seem odd -------------------------------
        if ( chance_corr > 130 )
          chance_corr -= 100;

        if ( chance_corr < 50 )    // lowest possible value is 45(?) - see above {dlb}
          chance_corr = 100 - chance_corr;

        chance_corr -= 50;
        // ---------------------------- but it needs to stay this way
        //                              (as it *was* this way)

        // the embedded equation may look funny, but it actually works well
        // to generate a pretty probability ramp {10%, 19%, 35%, 67%, 99%}
        // for values [0,4] which closely matches the original, ugly switch {dlb}
        if ( chance_corr >= 0 && chance_corr <= 4 )
          it_resists = ( random2(100) < 2 + (2 << (1 + chance_corr)) + (chance_corr << 3) );
        else
          it_resists = true;     // no idea how often this occurs {dlb}

        // if the checks get this far, you should hear about it {dlb}
        suppress_msg = false;
    }

// handle message output and item damage {dlb}:
    if ( !suppress_msg )
    {
        in_name(itco, 4, str_pass);
        strcpy(info, str_pass);
        strcat(info, (it_resists) ? " resists." : " is eaten away!");
        mpr(info);
    }

    if ( !it_resists)
    {
        how_rusty--;

        if ( you.inv_class[itco] == OBJ_WEAPONS )
          you.inv_plus2[itco] = how_rusty;
        else
          you.inv_plus[itco] = how_rusty;

        you.redraw_armor_class = 1;    // for armour, rings, etc. {dlb}

        if ( you.equip[EQ_WEAPON] == itco )
          wield_change = true;
    }

    return;

}          // end item_corrode()




void scrolls_burn( char burn_strength, char target_class )
{

    unsigned char burnc;
    unsigned char burn2;
    unsigned char burn_no = 0;

    if ( wearing_amulet(AMU_CONSERVATION) && !one_chance_in(10) )
    {
#ifdef WIZARD
        mpr("Amulet conserves.");
#endif
        return;
    }

    for (burnc = 0; burnc < ENDOFPACK; burnc++)
    {
        if ( !you.inv_quantity[burnc] )
          continue;
        if ( you.inv_class[burnc] != target_class )
          continue;

        for (burn2 = 0; burn2 < you.inv_quantity[burnc]; burn2++)
          if ( random2(70) < burn_strength )
          {
              you.inv_quantity[burnc]--;
              burn_no++;

              if ( you.inv_quantity[burnc] < 1 )
              {
                  you.num_inv_items--;

                  if ( burnc == you.equip[EQ_WEAPON] )  // I can't assume any level of intelligence on the player's behalf
                  {
                      you.equip[EQ_WEAPON] = -1;
                      mpr("You are now empty handed.");
                  }
                  break;
              }
          }

    }

    if ( burn_no == 1 )
    {
        if ( target_class == OBJ_SCROLLS )
          mpr("A scroll you are carrying catches fire!");
        else if ( target_class == OBJ_POTIONS )
          mpr("A potion you are carrying freezes and shatters!");
        else if ( target_class == OBJ_FOOD )
          mpr("Some of your food is covered with spores!");
    }
    else if ( burn_no > 1 )
    {
        if ( target_class == OBJ_SCROLLS )
          mpr("Some of the scrolls you are carrying catch fire!");
        else if ( target_class == OBJ_POTIONS )
          mpr("Some of the potions you are carrying freeze and shatter!");
        else if ( target_class == OBJ_FOOD )
          mpr("Some of your food is covered with spores!");
    }

    /* burn_no could be 0 */

}          // end scrolls_burn()



// death_source should be set to zero for non-monsters {dlb}
void ouch( int dam, int death_source, char death_type )
{

    struct monsters *monster = &menv[death_source];    // playing with fire, I know {dlb}
    char point_print[10];
    int d = 0;
    int e = 0;


    if ( you.deaths_door
        && death_type != KILLED_BY_LAVA
        && death_type != KILLED_BY_WATER )
      return;

    if ( dam > 300 )
      return;                 // assumed bug for high damage amounts

    if ( dam > -9000 )        // that is, a "death" caused by hp loss {dlb}
      {
        switch (you.religion)
        {
          case GOD_XOM:
            if (random2(you.hp_max) > you.hp && dam > random2(you.hp) && one_chance_in(5) )
              {
                mpr("Xom protects you from harm!");
                return;
              }
            break;

          case GOD_ZIN:
          case GOD_SHINING_ONE:
          case GOD_ELYVILON:
          case GOD_OKAWARU:
          case GOD_KIKUBAAQUDGHA:
            if ( dam >= you.hp
                && you.duration[DUR_PRAYER]
                && random2(you.piety) >= 30 )
              {
                simple_god_message(" protects you from harm!");
                return;
              }
            break;
        }
      }

    if ( dam > -9000 )
      {
        dec_hp(dam, true);

        if ( you.hp > 0 )
          return;
      }

#ifdef WIZARD
    if ( death_type != KILLED_BY_QUITTING
        && death_type != KILLED_BY_WINNING
        && death_type != KILLED_BY_LEAVING )
    {
#ifdef USE_OPTIONAL_WIZARD_DEATH
        if ( !yesno("Die?") )
        {
            set_hp(you.hp_max, false);
            return;
        }
#else
        mpr("Since you're a debugger, I'll let you live.");
        mpr("Be more careful next time, okay?");

        set_hp(you.hp_max, false);
        return;
#endif
    }
#endif

//okay, so you're dead:

    points += you.gold;
    points += (you.experience * 7) / 10;

    //if (death_type == KILLED_BY_WINNING) points += points / 2;
    //if (death_type == KILLED_BY_LEAVING) points += points / 10; // these now handled by giving player the value of their inventory

    char temp_id[4][50];

    for (d = 0; d < 4; d++)
      for (e = 0; e < 50; e++)
        temp_id[d][e] = 1;

    if ( death_type == KILLED_BY_LEAVING || death_type == KILLED_BY_WINNING )
      for (d = 0; d < ENDOFPACK; d++)
        points += item_value(you.inv_class[d], you.inv_type[d], you.inv_dam[d], you.inv_plus[d], you.inv_plus2[d], you.inv_quantity[d], 3, temp_id);

    if ( points > 99999999 )
      points = 99999999;

    itoa(points, point_print, 10);
    strcpy(death_string, point_print);

    if (points < 10000000)
      strcat(death_string, " ");
    if (points < 1000000)
      strcat(death_string, " ");
    if (points < 100000)
      strcat(death_string, " ");
    if (points < 10000)
      strcat(death_string, " ");
    if (points < 1000)
      strcat(death_string, " ");
    if (points < 100)
      strcat(death_string, " ");
    if (points < 10)
      strcat(death_string, " ");

    strncat(death_string, you.your_name, DEATH_NAME_LENGTH);
// BCR - I added some spaces here so the scores look nicer.
    int extra = DEATH_NAME_LENGTH - strlen(you.your_name);

    if ( extra > 0 )
      strncat(death_string, "              ", DEATH_NAME_LENGTH - strlen(you.your_name));

    strcat(death_string, " - ");
    strcat(death_string, species_abbrev(you.species));

    death_string[strlen(death_string)] = you.class_name[0];
    itoa(you.experience_level, point_print, 10);
    strcat(death_string, point_print);

#ifdef WIZARD
    strcat(death_string, " Wiz");
#endif

    strcat(death_string, ",");

    switch ( death_type )
    {
      case KILLED_BY_MONSTER:

/* BCR
 * Note: There was a bug where deep elves weren't getting the 'a' before
 *       their names.  It turns out that the code originally assumed that
 *       Monsters with type between 250 and 310 would be uniques.  However,
 *       Some new monsters were added between 260 and 280 that are not unique.
 *       For now, I've updated the check to be accurate, but there may be other
 *       issues with this.
 */
        strcat(death_string, " killed by ");
        if ( ( monster->type < MONS_PROGRAM_BUG )
               || ( ( monster->type < MONS_TERENCE ) && ( monster->type >= MONS_NAGA_MAGE ) )
               || ( monster->type > MONS_BORIS )
            && ( monster->type != MONS_PLAYER_GHOST ) )
          strcat(death_string, "a");

        strcat(death_string, monam(monster->number, monster->type, 0, 99));
        break;

      case KILLED_BY_POISON:
        //if (dam == -9999) strcat(death_string, "an overload of ");
        strcat(death_string, " killed by a lethal dose of poison");
        break;

      case KILLED_BY_CLOUD:
        strcat(death_string, " killed by a cloud");
        break;

      case KILLED_BY_BEAM:        // beam - beam[0].name is a local variable, so can't access it without horrible hacks
        strcat(death_string, " killed from afar by ");

        if ( monster->type < MONS_PROGRAM_BUG
              || monster->type > MONS_BORIS
            && monster->type != MONS_PLAYER_GHOST )    // this looks very, very wrong to me {dlb}
          strcat(death_string, "a");

        strcat(death_string, monam(monster->number, monster->type, 0, 99));
        break;

/*
      case KILLED_BY_DEATHS_DOOR: // death's door running out - NOTE: This is no longer fatal
        strcat(death_string, " ran out of time");
        break;
*/

      case KILLED_BY_LAVA:
        strcat(death_string, " took a swim in molten lava");
        break;

      case KILLED_BY_WATER:
        if ( you.species == SP_MUMMY )
          strcat(death_string, " soaked and fell apart");
        else
          strcat(death_string, " drowned");
        break;

// these three are probably only possible if you wear a you.ring of >= +3 ability,
// get drained to 3, then take it off, or have a very low abil and wear a -ve you.ring.
// or, as of 2.7x, mutations can cause this
      case KILLED_BY_STUPIDITY:
        strcat(death_string, " died of stupidity");
        break;

      case KILLED_BY_WEAKNESS:
        strcat(death_string, " too weak to continue adventuring");
        break;

      case KILLED_BY_CLUMSINESS:
        strcat(death_string, " slipped on a banana peel");
        break;

      case KILLED_BY_TRAP:
        strcat(death_string, " killed by a trap");
        break;

      case KILLED_BY_LEAVING:
        strcat(death_string, " got out of the dungeon alive.");
        break;

      case KILLED_BY_WINNING:
        strcat(death_string, " escaped with the Orb.");
        break;

      case KILLED_BY_QUITTING:
        strcat(death_string, " quit");
        break;

      case KILLED_BY_DRAINING:
        strcat(death_string, " was drained of all life");
        break;

      case KILLED_BY_STARVATION:
        strcat(death_string, " starved to death");
        break;

      case KILLED_BY_FREEZING:
        strcat(death_string, " froze to death");
        break;

      case KILLED_BY_BURNING:
        strcat(death_string, " burnt to a crisp");
        break;

      case KILLED_BY_WILD_MAGIC:
        strcat(death_string, " killed by wild magic");
        break;

      case KILLED_BY_XOM:
        strcat(death_string, " killed by Xom");
        break;

      case KILLED_BY_STATUE:
        strcat(death_string, " killed by a statue");
        break;

      case KILLED_BY_ROTTING:
        strcat(death_string, " rotted away");
        break;

      case KILLED_BY_TARGETTING:
        strcat(death_string, " killed by bad targetting");
        break;

      case KILLED_BY_SPORE:
        strcat(death_string, " killed by an exploding spore");
        break;

      case KILLED_BY_TSO_SMITING:
        strcat(death_string, " smote by The Shining One");
        break;

      case KILLED_BY_PETRIFICATION:
        strcat(death_string, " turned to stone");
        break;

      case KILLED_BY_SHUGGOTH:
        strcat(death_string, " eviscerated by a hatching shuggoth");
        break;

      default:
        strcat(death_string, " killed by a nasty bug in ouch::ouch()");
        break;
    }                           // end switch

    if ( death_type != KILLED_BY_LEAVING && death_type != KILLED_BY_WINNING )
      {
        if ( you.level_type == LEVEL_ABYSS )
          {
            strcat(death_string, " in the Abyss.");
            goto ending;
          }
        else if ( you.level_type == LEVEL_PANDEMONIUM )
          {
            strcat(death_string, " in Pandemonium.");
            goto ending;
          }
        else if ( you.level_type == LEVEL_LABYRINTH )
          {
            strcat(death_string, " in a labyrinth.");
            goto ending;
          }

        itoa((you.your_level + 1), st_prn, 10);

        switch ( you.where_are_you )
        {
          case BRANCH_ORCISH_MINES:
          case BRANCH_HIVE:
          case BRANCH_LAIR:
          case BRANCH_SLIME_PITS:
          case BRANCH_VAULTS:
          case BRANCH_CRYPT:
          case BRANCH_HALL_OF_BLADES:
          case BRANCH_HALL_OF_ZOT:
          case BRANCH_ECUMENICAL_TEMPLE:
          case BRANCH_SNAKE_PIT:
          case BRANCH_ELVEN_HALLS:
          case BRANCH_TOMB:
          case BRANCH_SWAMP:
            itoa(you.your_level - you.branch_stairs[you.where_are_you - 10], st_prn, 10);
            break;

          case BRANCH_DIS:
          case BRANCH_GEHENNA:
          case BRANCH_VESTIBULE_OF_HELL:
          case BRANCH_COCYTUS:
          case BRANCH_TARTARUS:
          case BRANCH_INFERNO:
          case BRANCH_THE_PIT:
            itoa(you.your_level + 1 - 26, st_prn, 10);
            break;
        }

        if ( you.where_are_you != BRANCH_VESTIBULE_OF_HELL )
          {
            strcat(death_string, " on L");
            strcat(death_string, st_prn);
          }

        switch ( you.where_are_you )
        {
          case BRANCH_DIS:
            strcat(death_string, " of Dis");
            break;
          case BRANCH_GEHENNA:
            strcat(death_string, " of Gehenna");
            break;
          case BRANCH_VESTIBULE_OF_HELL:
            strcat(death_string, " in the Vestibule");
            break;
          case BRANCH_COCYTUS:
            strcat(death_string, " of Cocytus");
            break;
          case BRANCH_TARTARUS:
            strcat(death_string, " of Tartarus");
            break;
          case BRANCH_ORCISH_MINES:
            strcat(death_string, " of the Mines");
            break;
          case BRANCH_HIVE:
            strcat(death_string, " of the Hive");
            break;
          case BRANCH_LAIR:
            strcat(death_string, " of the Lair");
            break;
          case BRANCH_SLIME_PITS:
            strcat(death_string, " of the Slime Pits");
            break;
          case BRANCH_VAULTS:
            strcat(death_string, " of the Vaults");
            break;
          case BRANCH_CRYPT:
            strcat(death_string, " of the Crypt");
            break;
          case BRANCH_HALL_OF_BLADES:
            strcat(death_string, " of the Hall");
            break;
          case BRANCH_HALL_OF_ZOT:
            strcat(death_string, " of Zot's Hall");
            break;
          case BRANCH_ECUMENICAL_TEMPLE:
            strcat(death_string, " of the Temple");
            break;
          case BRANCH_SNAKE_PIT:
            strcat(death_string, " of the Snake Pit");
            break;
          case BRANCH_ELVEN_HALLS:
            strcat(death_string, " of the Elf Hall");
            break;
          case BRANCH_TOMB:
            strcat(death_string, " of the Tomb");
            break;
          case BRANCH_SWAMP:
            strcat(death_string, " of the Swamp");
            break;
        }

        strcat(death_string, ".");
        save_ghost();

ending:
        end_game(1);
    }

    end_game(0);                // must have won! (or at least escaped)

}          // end ouch()




#ifndef SOLARIS

void end_game(char end_status)
{
    int i;
    char del_file[kFileNameSize];
    char glorpstr[kFileNameSize];
    char status2 = end_status;
    int sysg = 0;

    set_status(end_status);

#ifdef SAVE_DIR_PATH
    sprintf(glorpstr, SAVE_DIR_PATH "%s%d", you.your_name, getuid());
#else

#ifdef DOS

    strupr(glorpstr);

    strcpy(glorpstr, "");
    for (i = 0; i < kFileNameLen; i++)
    {
        glorpstr[i] = you.your_name[i];
        if (you.your_name[i] == 0)
            break;
    }
    glorpstr[kFileNameLen] = 0;
#endif

    strncpy(glorpstr, you.your_name, kFileNameLen);

// This is broken. Length is not valid yet! We have to check if we got a
// trailing NULL; if not, write one:
    if (strlen(you.your_name) > kFileNameLen - 1)       // is name 6 chars or more?
        glorpstr[kFileNameLen] = '\0';                  // if so, char 7 should be NULL
#endif


/* For some reason, the new file-purging method isn't working - I'll use my
   old one instead (LRH): */
    strncpy(glorpstr, you.your_name, kFileNameLen);

// This is broken. Length is not valid yet! We have to check if we got a
// trailing NULL; if not, write one:
    if (strlen(you.your_name) > kFileNameLen - 1)      // is name 6 chars or more?
        glorpstr[kFileNameLen] = '\0';                 // if so, char 7 should be NULL

    strncpy(glorpstr, you.your_name, kFileNameLen);

// This is broken. Length is not valid yet! We have to check if we got a
// trailing NULL; if not, write one:
    if (strlen(you.your_name) > kFileNameLen - 1)      // is name 6 chars or more?
        glorpstr[kFileNameLen] = '\0';                 // if so, char 7 should be NULL

    int fi = 0;
    int fi2 = 0;
    char st_prn[6];

        int count = 0;
    for (fi2 = 0; fi2 < 30; fi2++)
    {
        for (fi = 0; fi < 50; fi++)
        {
            if (!tmp_file_pairs[fi][fi2])
                continue;
            strcpy(del_file, "");
#ifdef SAVE_DIR_PATH
            strcpy(del_file, SAVE_DIR_PATH);
#endif
            strcat(del_file, glorpstr);
#ifdef SAVE_DIR_PATH
            strcat(del_file, getuid());
#endif
            strcat(del_file, ".");
            if (fi < 10)
              strcat(del_file, "0");
            itoa(fi, st_prn, 10);
            strcat(del_file, st_prn);

            st_prn[0] = fi2 + 97;
            st_prn[1] = '\0';

            strcat(del_file, st_prn);
            strcat(del_file, "\0");

            // unlink file. Since we can't do anything about
            // a failure, ignore it.
            unlink(del_file);
            ++count;
        }
    }
    ASSERT(count > 0);

/*
   strncpy( del_file, glorpstr, kFileNameLen );

   // Calculate the positions of the characters
   const int point = strlen( del_file );
   const int tens = point + 1;
   const int ones = tens + 1;
   const int dun = ones + 1;

   // the constant characters
   del_file[point] = '.';
   del_file[dun + 1] = '\0';

   // the variable ones
   for (int level = 0; level < MAX_LEVELS; level++)
   {
   for (int dungeon = 0; dungeon < MAX_BRANCHES; dungeon++)
   {
   if (tmp_file_pairs[ level ][ dungeon ])
   {
   del_file[dun] = 'a' + dungeon;
   del_file[tens] = '0' + (level / 10);
   del_file[ones] = '0' + (level % 10);
   unlink(del_file);
   }
   }
   }
 */

#ifdef PACKAGE_SUFFIX
    // this is to catch the game package if it still exists.
    del_file[point] = '\0';

    strcat(del_file, PACKAGE_SUFFIX);
    unlink(del_file);
#endif

    /*int fi = 0;

       for (fi = 0; fi < 100; fi ++)
       {
       strcpy(del_file, glorpstr);
       strcat(del_file, ".");
       itoa(fi, st_prn, 10);
       strcat(del_file, st_prn);
       strcat(del_file, "\0");
       handle = open(del_file, S_IWRITE, S_IREAD);

       if (handle != -1)
       {
       close(handle);
       sysg = unlink(del_file);
       } else close(handle);
       } */


    strcpy(del_file, glorpstr);

    strcat(del_file, ".lab");
#ifdef DOS
    strupr(del_file);
#endif
    sysg = unlink(del_file);

    strcpy(del_file, glorpstr);

    strcat(del_file, ".sav");
#ifdef DOS
    strupr(del_file);
#endif
    sysg = unlink(del_file);

    status2 = set_status(100);


    if ( status2 == 1 )
    {
        mpr("You die...");    // insert player name here? {dlb}
    }

    viewwindow(1, false);
    more();


    for (i = 0; i < ENDOFPACK; i++)
      you.inv_ident[i] = 3;

    for (i = 0; i < ENDOFPACK; i++)
      if ( you.inv_class[i] != 0 )
      {
          set_id(you.inv_class[i], you.inv_type[i], 1);
      }

    invent( -1, ( status2 == 0 ) );

#ifdef USE_CURSES
    clear();
#endif

    if (! dump_char((status2 == 0), "morgue.txt") )
      {
        mpr("Char dump unsuccessful! Sorry about that.");
        more();
      }
#ifdef DEBUG
    //jmf: switched logic and moved "success" message to debug-only
    else
      {
        mpr("Char dump successful! (morgue.txt).");
        more();
      }
#endif // DEBUG

    int p = 0;

    for (p = 0; p < ENDOFPACK; p++)
      for (i = 0; i < MAX_ITEMS; i++)
      {
          if ( !mitm.quantity[i] )
          {
              mitm.id[i] = 0;
              mitm.base_type[i] = you.inv_class[p];
              mitm.sub_type[i] = you.inv_type[p];
              mitm.pluses[i] = you.inv_plus[p];
              mitm.pluses2[i] = you.inv_plus2[p];
              mitm.special[i] = you.inv_dam[p];
              mitm.colour[i] = you.inv_colour[p];
              mitm.x[i] = you.x_pos;
              mitm.y[i] = you.y_pos;
              mitm.quantity[i] = you.inv_quantity[p];
              break;
          }
      }        // end "for p,i"



    for (i = 0; i < MAX_ITEMS; i++)
      mitm.id[i] = 0;


    clrscr();
#ifdef DOS_TERM
    window(1, 1, 80, 25);
#endif
    clrscr();
    cprintf("Goodbye, ");
    cprintf(you.your_name);
    cprintf(".");
    cprintf(EOL EOL);
    cprintf(death_string);
    cprintf(EOL EOL " Best Crawlers - " EOL);


    highscore(death_string, points);
    get_ch();     // just to pause, actual value returned does not matter {dlb}
    end(0);
}
#else

void end_game(char end_status)
{
    int i;
    char del_file[kFileNameSize];
    char glorpstr[kFileNameSize];
    char status2 = end_status;
    int sysg = 0;

    set_status(end_status);

#ifdef SAVE_DIR_PATH
    sprintf(glorpstr, SAVE_DIR_PATH "%s%d", you.your_name, getuid());

#else

#ifdef DOS

    strupr(glorpstr);

    strcpy(glorpstr, "");
    for (i = 0; i < kFileNameLen; i++)
    {
        glorpstr[i] = you.your_name[i];
        if (you.your_name[i] == 0)
            break;
    }
    glorpstr[kFileNameLen] = 0;

#endif

    strncpy(glorpstr, you.your_name, kFileNameLen);

    /* This is broken. Length is not valid yet! We have to check if we got a
       trailing NULL; if not, write one: */
    if (strlen(you.your_name) > kFileNameLen - 1)       /* is name 6 chars or more? */
        glorpstr[kFileNameLen] = '\0';  /* if so, char 7 should be NULL */
#endif

    strncpy(del_file, glorpstr, kFileNameLen);

    // Calculate the positions of the characters
    const int point = strlen(del_file);
    const int tens = point + 1;
    const int ones = tens + 1;
    const int dun = ones + 1;

    // the constant characters
    del_file[point] = '.';
    del_file[dun + 1] = '\0';

    // the variable ones
    for (int level = 0; level < MAX_LEVELS; level++)
    {
        for (int dungeon = 0; dungeon < MAX_BRANCHES; dungeon++)
        {
            if (tmp_file_pairs[level][dungeon])
            {
                del_file[dun] = 'a' + dungeon;
                del_file[tens] = '0' + (level / 10);
                del_file[ones] = '0' + (level % 10);
                unlink(del_file);
            }
        }
    }

#ifdef PACKAGE_SUFFIX
    // this is to catch the game package if it still exists.
    del_file[point] = '\0';

    strcat(del_file, PACKAGE_SUFFIX);
    unlink(del_file);
#endif

    /*int fi = 0;

       for (fi = 0; fi < 100; fi ++)
       {
       strcpy(del_file, glorpstr);
       strcat(del_file, ".");
       itoa(fi, st_prn, 10);
       strcat(del_file, st_prn);
       strcat(del_file, "\0");
       handle = open(del_file, S_IWRITE, S_IREAD);

       if (handle != -1)
       {
       close(handle);
       sysg = unlink(del_file);
       } else close(handle);
       } */


    strcpy(del_file, glorpstr);

    strcat(del_file, ".lab");
#ifdef DOS
    strupr(del_file);
#endif
    sysg = unlink(del_file);

    strcpy(del_file, glorpstr);

    strcat(del_file, ".sav");
#ifdef DOS
    strupr(del_file);
#endif
    sysg = unlink(del_file);

    status2 = set_status(100);


    if (status2 == 1)
    {
        mpr("You die...");    // insert player name here? {dlb}
    }

    viewwindow(1, false);
    more();


    for (i = 0; i < ENDOFPACK; i++)
      you.inv_ident[i] = 3;

    for (i = 0; i < ENDOFPACK; i++)
      if ( you.inv_class[i] != 0 )
        set_id(you.inv_class[i], you.inv_type[i], 1);

    invent( -1, ( status2 == 0 ) );

#ifdef USE_CURSES
    clear();
#endif

    if (! dump_char((status2 == 0), "morgue.txt") )
      mpr("Char dump unsuccessful! Sorry about that.");
#ifdef DEBUG
    //jmf: switched logic and moved "success" message to debug-only
    else
      mpr("Char dump successful! (morgue.txt).");
#endif // DEBUG

    more();

    int p = 0;

    for (p = 0; p < ENDOFPACK; p++)
      for (i = 0; i < MAX_ITEMS; i++)
      {
          if ( !mitm.quantity[i] )
          {
              mitm.id[i] = 0;
              mitm.base_type[i] = you.inv_class[p];
              mitm.sub_type[i] = you.inv_type[p];
              mitm.pluses[i] = you.inv_plus[p];
              mitm.pluses2[i] = you.inv_plus2[p];
              mitm.special[i] = you.inv_dam[p];
              mitm.colour[i] = you.inv_colour[p];
              mitm.x[i] = you.x_pos;
              mitm.y[i] = you.y_pos;
              mitm.quantity[i] = you.inv_quantity[p];
              break;
          }
      }        // end "for p,i"


    for (i = 0; i < MAX_ITEMS; i++)
    {
        mitm.id[i] = 0;
    }


    clrscr();
#ifdef DOS_TERM
    window(1, 1, 80, 25);
#endif
    clrscr();
    cprintf("Goodbye, ");
    cprintf(you.your_name);
    cprintf(".");
    cprintf(EOL EOL);
    cprintf(death_string);
    cprintf(EOL EOL " Best Crawlers - " EOL);


    highscore(death_string, points);
    get_ch();     // just to pause, actual value returned does not matter {dlb}
    end(0);
}

#endif




// BCR - used to pad a string out to 80 characters
static char *pad( char *str )
{

    static char *padding = "                                                                                 ";
    int length = strlen(str);

    strncpy(str + length, padding, 80 - (length - 1));
    str[80] = 0;
    return str;

}          // end pad()




// BCR - I did a lot of cleanup in here, removing gotos, etc.
void highscore(char death_string[256], long points)
{
    char high_scores[SCORE_FILE_ENTRIES][80];
    long scores[SCORE_FILE_ENTRIES];
    int i = 0;
    int j = 0;
    char ready[80];

    pad(death_string);

    for (i = 0; i < SCORE_FILE_ENTRIES; i++)
    {
        strcpy(high_scores[i], "0       empty                                                                  ");
        scores[i] = 0;
    }

#ifdef SAVE_DIR_PATH
    FILE *handle = fopen(SAVE_DIR_PATH "scores", "rb");

#else
    FILE *handle = fopen("scores", "rb");

#endif

    if (handle != NULL)
    {
#ifdef SOLARIS
        lockf(fileno(handle), F_LOCK, 0);
#endif
        i = 0;
        int endOfFile = 0;

        while ((i < SCORE_FILE_ENTRIES) && !endOfFile)
        {
            // BCR - reading in 80 chars at a time now instead of 1.  80x faster.
            fread(ready, 80, 1, handle);

            for (j = 0; j < 80; j++)
            {
                if (isspace(ready[j]) || isalpha(ready[j]) || isdigit(ready[j]) || ispunct(ready[j]))
                    high_scores[i][j] = ready[j];
                else if (ready[j] == EOF)
                {
                    endOfFile = 1;
                    break;
                }
                else
                {
                    pad(high_scores[i]);
                    break;
                }
            }                   // end for j

            i++;
        }                       // end for i

        for (i = 0; i < SCORE_FILE_ENTRIES; i++)
        {
            int multip = 1;

            for (j = 6; j >= 0; j--)
            {
                if (high_scores[i][j] == 32)
                    continue;
                scores[i] += (high_scores[i][j] - 48) * multip;
                multip *= 10;
            }
        }

    }

    char has_printed = 0;
    int numEntries = NUMBER_OF_LINES - 7;

    for (i = 0; i < numEntries; i++)
    {
        if ((points >= scores[i]) && (!has_printed))
        {
            textcolor(YELLOW);
            itoa(i + 1, ready, 10);
            cprintf(ready);
            if (strlen(ready) == 1)
                cprintf("- ");
            else
                cprintf("-");

            j = 0;
            while ((j < 75) && (death_string[j] != EOF))
                putch(death_string[j++]);
            cprintf(EOL);

            has_printed = 1;
            i--;
            textcolor(LIGHTGREY);
        }
        else
        {
            itoa(i + 1, ready, 10);
            cprintf(ready);

            if (strlen(ready) == 1)
                cprintf("- ");
            else
                cprintf("-");

            j = 0;
            while ((j < 75) && (death_string[j] != EOF))
                putch(high_scores[i][j++]);
            cprintf(EOL);
        }
    }

#ifdef SOLARIS
    // here too... sigh.  GDL
    lockf(fileno(handle), F_ULOCK, 0);
#endif

    fclose(handle);

#ifdef SAVE_DIR_PATH
    handle = fopen(SAVE_DIR_PATH "scores", "wb");
#else
    handle = fopen("scores", "wb");
#endif

    if (handle == NULL)
    {
        perror("oh NO!!!");
        if (getch() == 0)
            getch();
    }
    else
    {
#ifdef SOLARIS
        lockf(fileno(handle), F_LOCK, 0);
#endif

/*
           for (i = 0; i < SCORE_FILE_ENTRIES; i++)
             for (j = strlen(high_scores[i]); j < 80; j++)
               high_scores[i][j] = ' ';
*/

        has_printed = 0;
        for (i = 0; i < SCORE_FILE_ENTRIES; i++)
        {

            if (has_printed == 1 && i == SCORE_FILE_ENTRIES - 1)
                break;

            if (points > scores[i] && has_printed == 0)
            {
                fwrite(death_string, 80, 1, handle);
                i--;
                has_printed = 1;
            }
            else
                fwrite(high_scores[i], 80, 1, handle);
        }

#ifdef SOLARIS
        lockf(fileno(handle), F_ULOCK, 0);
#endif

        fclose(handle);

#ifdef SHARED_FILES_CHMOD_VAL

#ifdef SAVE_DIR_PATH
        chmod(SAVE_DIR_PATH "scores", SHARED_FILES_CHMOD_VAL);
#else
        chmod("scores", SHARED_FILES_CHMOD_VAL);
#endif

#endif
    }
}                               // end void highscores




void lose_level( void )
{

    char temp_quant[5];

    if ( you.experience_level == 1 )
      ouch(-9999, 0, KILLED_BY_DRAINING);
    // because you.experience is unsigned long, if it's going to be -ve must die straightaway.

    you.experience = exp_needed(you.experience_level + 1, you.species) - 1;
    you.experience_level--;

    strcpy(info, "You are now a level ");
    itoa(you.experience_level, temp_quant, 10);
    strcat(info, temp_quant);
    strcat(info, " ");
    strcat(info, you.class_name);
    strcat(info, "!");
    mpr(info);

    int brek = 0;

    if ( you.experience_level > 20 )
      brek = ( coinflip() ? 3 : 2 );
    else if ( you.experience_level > 11)
      brek = random2(3) + 2;
    else
      brek = random2(3) + 4;

    ouch(brek, 0, KILLED_BY_DRAINING);
    you.base_hp2 -= brek;

    dec_mp(1);
    you.base_magic_points2--;

    //you.spell_levels--;

    calc_hp();
    calc_mp();

    //if ( you.spell_levels < -40 )
    //  you.spell_levels = -40;

    you.redraw_experience = 1;

}          // end lose_level()




void drain_exp( void )
{
    int protection = player_prot_life();

    if ( you.duration[DUR_PRAYER]
        && ( you.religion == GOD_ZIN || you.religion == GOD_SHINING_ONE )
        && random2(150) < you.piety )
    {
        simple_god_message(" protects your life force!");
        return;
    }

    if ( protection >= 3 || you.is_undead )
    {
        mpr("You fully resist.");
        return;
    }

    if ( you.experience == 0 )
      ouch(-9999, 0, KILLED_BY_DRAINING);

    if ( you.experience_level == 1 )
    {
        you.experience = 0;
        return;
    }

    unsigned long total_exp = exp_needed(you.experience_level + 2, you.species) - exp_needed(you.experience_level + 1, you.species);
    unsigned long exp_drained = total_exp * (10 + random2(11));

    exp_drained /= 100;

    if (protection > 0)
    {
        mpr("You partially resist.");
        exp_drained -= (protection * exp_drained) / 3;
    }

    if (exp_drained > 0)
    {
        mpr("You feel drained.");
        you.experience -= exp_drained;
        you.exp_available -= exp_drained;

        if (you.exp_available < 0)
          you.exp_available = 0;

#ifdef WIZARD
        strcpy(info, "You lose ");

        char temp_quant[20];
        itoa(exp_drained, temp_quant, 10);

        strcat(info, temp_quant);
        strcat(info, " experience points.");
        mpr(info);
#endif

        you.redraw_experience = 1;

        if ( you.experience < exp_needed(you.experience_level + 1, you.species) )
          lose_level();

    }

}          // end drain_exp()




int set_status( int stat )
{

    static int stat2;

    if (stat != 100)
      stat2 = stat;

    return stat2;

}

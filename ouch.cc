/*
 *  File:       ouch.cc
 *  Summary:    Functions used when Bad Things happen to the player.
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 *      <4>      6/13/99        BWR             applied a mix of DML and my tmp
 *                                              file purging improvements.
 *      <3>      5/26/99        JDJ             highscore() will print more scores on
 *                                                                                              larger windows.
 *      <2>      5/21/99        BWR             Added SCORE_FILE_ENTRIES, so
 *                                              that more top scores can be
 *                                              saved.
 *      <1>      -/--/--        LRH             Created
 */

#include "AppHdr.h"
#include "ouch.h"

#ifdef DOS
#include <conio.h>
#endif

#ifdef DOS
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

//#include <fstream.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "externs.h"
#include "enum.h"

#include "chardump.h"
#include "view.h"
#include "invent.h"
#include "shopping.h"
#include "itemname.h"
#include "mstruct.h"
#include "player.h"
#include "randart.h"
#include "religion.h"
#include "skills2.h"
#include "stuff.h"
#include "files.h"

#ifdef MACROS
  #include "macro.h"
#endif

void highscore(char death_string[256], long points);
void item_corrode(char itco);
void end_game(char end_status);
int set_status(int stat);


char death_string[256];
long points = 0;

extern char wield_change;       /* defined in output.cc */

/* NOTE: DOES NOT check for hellfire!!! */
int check_your_resists(int hurted, int flavour)
{

    switch (flavour)
    {
    case 2:                     /* fire */
        if (player_res_fire() > 100)
        {
            mpr("You resist.");
            hurted /= 2 + ((player_res_fire() - 100) * (player_res_fire() - 100));
        }
        else if (player_res_fire() < 100)
        {
            mpr("It burns terribly!");
            hurted *= 15;
            hurted /= 10;
        }
        break;

    case 3:                     /* cold */
        if (player_res_cold() > 100)
        {
            mpr("You resist.");
            hurted /= 2 + ((player_res_cold() - 100) * (player_res_cold() - 100));
        }
        else if (player_res_cold() < 100)
        {
            mpr("You feel a terrible chill!");
            hurted *= 15;
            hurted /= 10;
        }
        break;

    case 5:                     /* electricity */
        if (you.attribute[ATTR_RESIST_LIGHTNING] > 0)
        {
            mpr("You resist.");
            hurted = 0;
        }
        break;


    case 6:                     /* poison */
        if (player_res_poison() > 0)
        {
            mpr("You resist.");
            hurted = 0;
        }
        else
            you.poison += random() % 2 + 1;
        break;

    case 7:                     /* negativity */
        if (player_prot_life() != 0)
        {
            mpr("You resist.");
            hurted = 0;
            break;
        }
        mpr("You feel drained!");
        drain_exp();
        break;

    case 23:                    /* ice */
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


    }                           /* end switch */

//      if (stricmp(beam_name, "hellfire") == 0 || beam[0].flavour == 20) // 20 is lava
    if (flavour == 20)
    {
        if (player_res_fire() > 100)
        {
            mpr("You partially resist.");
            hurted /= 1 + (player_res_fire() - 100);
        }
        if (player_res_fire() < 100)
        {
            mpr("It burns terribly!");
            hurted *= 15;
            hurted /= 10;
        }
    }

    return hurted;

}




void splash_with_acid(char acid_strength)
{

/* affects equip only?

   assume that a message has already been sent, also that damage has
   already been done */


//abort();

    char splc = 0;

    for (splc = 1; splc < 7; splc++)
    {
        if (you.equip[splc] == -1)
        {
            ouch(random2(acid_strength) / 5, 0, 3);

            continue;
            /* should take extra damage if little armour worn */
        }

        if (random2(20) > acid_strength)
            continue;

        item_corrode(you.equip[splc]);

    }

}


void weapon_acid(char acid_strength)
{
    char hand_thing = you.equip[EQ_WEAPON];

    if (you.equip[EQ_WEAPON] == -1)
    {
        if (you.equip[EQ_GLOVES] != -1)
            hand_thing = you.equip[EQ_GLOVES];
        else
            return;             /* take extra damage */
    }

    if (random2(20) > acid_strength)
        return;

    item_corrode(hand_thing);

}



void item_corrode(char itco)
{

    int chance_corr = 0;
    unsigned char rusty = 0;

    if (you.inv_class[itco] == 0)
        rusty = you.inv_plus2[itco];
    else
        rusty = you.inv_plus[itco];

    if (you.inv_class[itco] == 0 || you.inv_class[itco] == 1)
    {
        if (you.inv_dam[itco] / 30 == 5 && random() % 5 != 0)
            return;
        /* dwarven stuff is resistant to acids */
    }

    if (rusty < 45)
        return;
    if (you.inv_class[itco] == 0 && you.inv_dam[itco] > 180)
        return;                 // unique

    if (you.inv_class[itco] == 0 && you.inv_dam[itco] % 30 >= 25)
        return;                 // unique

    if (you.inv_class[itco] == 2 && you.inv_dam[itco] % 30 >= 25)
        return;                 // unique

    if (wearing_amulet(AMU_RESIST_CORROSION) == 1 && random() % 10 != 0)
    {
#ifdef WIZARD
        strcpy(info, "Amulet protects.");
        mpr(info);
#endif
        return;                 /* amulet of resist corrosion/cloak of preservation */
    }
    chance_corr = rusty;
    if (chance_corr > 130)
        chance_corr -= 100;

    if (chance_corr < 50)
    {
        chance_corr = 50 + ((chance_corr - 50) * -1);
    }

    chance_corr -= 50;

    switch (chance_corr)
    {
    case 0:
        if (random2(100) < 10)
            return;
        break;
    case 1:
        if (random2(100) < 25)
            return;
        break;
    case 2:
        if (random2(100) < 40)
            return;
        break;
    case 3:
        if (random2(100) < 70)
            return;
        break;
    case 4:
        if (random2(100) < 95)
            return;
        break;
    default:
        return;
    }

    item_name(you.inv_plus2[itco], you.inv_class[itco], you.inv_type[itco], you.inv_dam[itco], you.inv_plus[itco], you.inv_quantity[itco], you.inv_ident[itco], 4, str_pass);

    strcpy(info, str_pass);

    strcat(info, " is eaten away!");
    mpr(info);

    rusty--;
    if (you.inv_class[itco] == 0)
        you.inv_plus2[itco] = rusty;
    else
        you.inv_plus[itco] = rusty;

    you.redraw_armor_class = 1;

    if (you.equip[EQ_WEAPON] == itco)
        wield_change = 1;

}



void scrolls_burn(char burn_strength, char target_class)
{

    unsigned char burnc;
    unsigned char burn2;
    unsigned char burn_no = 0;

    if (wearing_amulet(AMU_CONSERVATION) == 1 && random() % 10 != 0)
    {
#ifdef WIZARD
        strcpy(info, "Amulet conserves.");
        mpr(info);
#endif
        return;
    }

    for (burnc = 0; burnc < 52; burnc++)
    {
        if (you.inv_quantity[burnc] == 0)
            continue;
        if (you.inv_class[burnc] != target_class)
            continue;           /* 6 scrolls, 8 potions */

        for (burn2 = 0; burn2 < you.inv_quantity[burnc]; burn2++)
        {
            if (random2(70) < burn_strength)
            {
                you.inv_quantity[burnc]--;
                burn_no++;
                if (you.inv_quantity[burnc] <= 0)
                {
                    you.num_inv_items--;
                    if (burnc == you.equip[EQ_WEAPON])  // I can't assume any level of intelligence on the player's behalf.

                    {
                        you.equip[EQ_WEAPON] = -1;
                        strcpy(info, "You are now empty handed.");
                        mpr(info);
                    }
                    break;
                }
            }
        }
    }

    if (burn_no == 1)
    {
        if (target_class == 6)
            strcpy(info, "A scroll you are carrying catches fire!");
        if (target_class == 8)
            strcpy(info, "A potion you are carrying freezes and shatters!");
        if (target_class == 4)
            strcpy(info, "Some of your food is covered with spores!");
        mpr(info);
    }

    if (burn_no > 1)
    {
        if (target_class == 6)
            strcpy(info, "Some of the scrolls you are carrying catch fire!");
        if (target_class == 8)
            strcpy(info, "Some of the potions you are carrying freeze and shatter!");
        if (target_class == 4)
            strcpy(info, "Some of your food is covered with spores!");
        mpr(info);
    }
/* burn_no could be 0 */

}



void ouch(int dam, char death_source, char death_type)
{
    char point_print[10];
    int d = 0;
    int e = 0;


    if (you.deaths_door > 0 && death_type != 4 && death_type != 5 && death_type != 6)
    {
        return;
    }

    if (dam > -9000)
    {

        switch (you.religion)
        {
        case 5:
            if (random2(you.hp_max) > you.hp && dam > random2(you.hp) && random2(5) == 0)
            {
                mpr("Xom protects you from harm!");
                return;
            }
            break;
        case 1:
        case 2:
        case 3:
        case 7:
        case 12:
            if (dam >= you.hp && you.duration[DUR_PRAYER] > 0 && random2(you.piety) >= 30)
            {
                strcpy(info, god_name(you.religion));
                strcat(info, " protects you from harm!");
                mpr(info);
                return;
            }
            break;

        }

    }

    you.redraw_hit_points = 1;

    if (dam > 300)
        return;                 /* assume it's a bug */

    if (dam > -9000)
    {
        you.hp -= dam;
        if (you.hp > 0)
            return;
    }

#ifdef WIZARD
    if (death_type != 13 && death_type != 12 && death_type != 11)       // quit or escaped

    {
        strcpy(info, "Since you're a debugger, I'll let you live.");
        mpr(info);
        strcpy(info, "Be more careful next time, okay?");
        mpr(info);
// more2();
        you.hp = you.hp_max;
        you.redraw_hit_points = 1;
        return;
    }
#endif

// okay, so you're dead.

    points += you.gold;
    points += (you.experience * 7) / 10;
//if (death_type == 12) points += points / 2;
    //if (death_type == 11) points += points / 10; // these now handled by giving player the value of their inventory
    char temp_id[4][50];

    for (d = 0; d < 4; d++)
    {
        for (e = 0; e < 50; e++)
        {
            temp_id[d][e] = 1;
        }
    }
    if (death_type == 11 || death_type == 12)
    {
        for (d = 0; d < 52; d++)
        {
            points += item_value(you.inv_class[d], you.inv_type[d], you.inv_dam[d], you.inv_plus[d], you.inv_plus2[d], you.inv_quantity[d], 3, temp_id);
        }
    }

    if (points > 99999999)
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
/*for (i = 0; i < 12 - strlen(death_string); i ++)
   {
   strcat(death_string, " ");
   } */

    strncat(death_string, you.your_name, 14);
    strcat(death_string, "-");
    switch (you.species)
    {
    case 1:
        strcat(death_string, "Hu");
        break;
    case 2:
        strcat(death_string, "El");
        break;
    case 3:
        strcat(death_string, "HE");
        break;
    case 4:
        strcat(death_string, "GE");
        break;
    case 5:
        strcat(death_string, "DE");
        break;
    case 6:
        strcat(death_string, "SE");
        break;
    case 7:
        strcat(death_string, "HD");
        break;
    case 8:
        strcat(death_string, "MD");
        break;
    case 9:
        strcat(death_string, "Ha");
        break;
    case 10:
        strcat(death_string, "HO");
        break;
    case 11:
        strcat(death_string, "Ko");
        break;
    case 12:
        strcat(death_string, "Mu");
        break;
    case 13:
        strcat(death_string, "Na");
        break;
    case 14:
        strcat(death_string, "Gn");
        break;
    case 15:
        strcat(death_string, "Og");
        break;
    case 16:
        strcat(death_string, "Tr");
        break;
    case 17:
        strcat(death_string, "OM");
        break;
    case 18:
    case 19:
    case 20:
    case 21:
    case 22:
    case 23:
    case 24:
    case 25:
    case 26:
    case 27:
    case 28:
    case 29:
        strcat(death_string, "Dr");
        break;
    case 30:
        strcat(death_string, "Ce");
        break;
    case 31:
        strcat(death_string, "DG");
        break;
    case 32:
        strcat(death_string, "Sp");
        break;
    case 33:
        strcat(death_string, "Mi");
        break;
    case 34:
        strcat(death_string, "DS");
        break;
    case 35:
        strcat(death_string, "Gh");
        break;
    case 36:
        strcat(death_string, "Ke");
        break;
    }
    death_string[strlen(death_string)] = you.class_name[0];

    itoa(you.experience_level, point_print, 10);
    strcat(death_string, point_print);
//if (strlen(point_print) == 1) strcat(death_string, " ");

//      strcat(death_string, ", killed by ");
    switch (death_type)
    {

    case 0:                     // monster

        strcat(death_string, ", killed by ");
        if (menv[death_source].type < 250 || menv[death_source].type > 310 && menv[death_source].type != 400)
            strcat(death_string, "a");
        strcat(death_string, monam(menv[death_source].number, menv[death_source].type, 0, 99));
        break;

    case 1:                     // you.poison
        //              if (dam == -9999) strcat(death_string, "an overload of ");

        strcat(death_string, ", killed by a lethal dose of poison");
        break;

    case 2:                     // cloud

        strcat(death_string, ", killed by a cloud");
        break;

    case 3:                     // beam - beam[0].name is a local variable, so can't access it without horrible hacks

        strcat(death_string, ", killed from afar by ");
        if (menv[death_source].type < 250 || menv[death_source].type > 310 && menv[death_source].type != 400)
            strcat(death_string, "a");
        strcat(death_string, monam(menv[death_source].number, menv[death_source].type, 0, 99));
        break;

/* case 4: // death's door running out - NOTE: This is no longer fatal
   strcat(death_string, " ran out of time");
   break; */

    case 5:                     // falling into lava

        strcat(death_string, " took a swim in molten lava");
        break;

    case 6:                     // falling into water

        if (you.species == SP_MUMMY)
            strcat(death_string, " soaked and fell apart");
        else
            strcat(death_string, " drowned");
        break;

// these three are probably only possible if you wear a you.ring of >= +3 ability,
        //  get drained to 3, then take it off, or have a very low abil and wear a -ve you.ring.
        // or, as of 2.7x, mutations can cause this
    case 7:                     // lack of intelligence

        strcat(death_string, " died of stupidity");
        break;
    case 8:                     // lack of str

        strcat(death_string, " died of muscular atrophy");
        break;
    case 9:                     // lack of dex

        strcat(death_string, " died of clumsiness");    // crappy message

        break;

    case 10:
        strcat(death_string, ", killed by a trap");
        break;

    case 11:
        strcat(death_string, " got out of the dungeon alive.");
        break;

    case 12:
        strcat(death_string, " escaped with the Orb.");
        break;

    case 13:
        strcat(death_string, " quit");
        break;

    case 14:
        strcat(death_string, " was drained of all life");
        break;

    case 15:
        strcat(death_string, " starved to death");
        break;

    case 16:
        strcat(death_string, " froze to death");
        break;

    case 17:
        strcat(death_string, " burned to death");
        break;

    case 18:                    /* from function miscast_effect */
        strcat(death_string, ", killed by wild magic");
        break;

    case 19:
        strcat(death_string, ", killed by Xom");
        break;

    case 20:
        strcat(death_string, ", killed by a statue");
        break;

    case 21:
        strcat(death_string, " rotted away");
        break;

    case 22:
        strcat(death_string, ", killed by bad targetting");
        break;


    }                           // end switch

    if (death_type != 11 && death_type != 12)
    {

        if (you.level_type == 2)
        {
            strcat(death_string, " in the Abyss.");
            goto ending;
        }
        if (you.level_type == 3)
        {
            strcat(death_string, " in Pandemonium.");
            goto ending;
        }
        if (you.level_type == 1)
        {
            strcat(death_string, " in a labyrinth.");
            goto ending;
        }

        itoa((you.your_level + 1), st_prn, 10);

        if (you.where_are_you >= 1 && you.where_are_you <= 9)
            itoa(you.your_level + 1 - 26, st_prn, 10);

        if (you.where_are_you >= 10)
        {
            itoa(you.your_level - you.branch_stairs[you.where_are_you - 10], st_prn, 10);
        }
/* switch(you.where_are_you)
   {
   case 10: itoa(you.your_level - you.branch_stairs [0], st_prn, 10); break;
   case 11: itoa(you.your_level - you.branch_stairs [1], st_prn, 10); break;
   case 12: itoa(you.your_level - you.branch_stairs [2], st_prn, 10); break;
   case 13: itoa(you.your_level - you.branch_stairs [3], st_prn, 10); break;
   case 14: itoa(you.your_level - you.branch_stairs [4], st_prn, 10); break;
   } */
        if (you.where_are_you != 3)
        {
            strcat(death_string, " on L");
            strcat(death_string, st_prn);
        }

        switch (you.where_are_you)
        {
        case 1:
            strcat(death_string, " of Dis");
            break;
        case 2:
            strcat(death_string, " of Gehenna");
            break;
        case 3:
            strcat(death_string, " in the Vestibule");
            break;
        case 4:
            strcat(death_string, " of Cocytus");
            break;
        case 5:
            strcat(death_string, " of Tartarus");
            break;
        case 10:
            strcat(death_string, " of the Mines");
            break;
        case 11:
            strcat(death_string, " of the Hive");
            break;
        case 12:
            strcat(death_string, " of the Lair");
            break;
        case 13:
            strcat(death_string, " of the Slime Pits");
            break;
        case 14:
            strcat(death_string, " of the Vaults");
            break;
        case 15:
            strcat(death_string, " of the Crypt");
            break;
        case 16:
            strcat(death_string, " of the Hall");
            break;
        case 17:
            strcat(death_string, " of Zot's Hall");
            break;
        case 18:
            strcat(death_string, " of the Temple");
            break;
        case 19:
            strcat(death_string, " of the Snake Pit");
            break;
        case 20:
            strcat(death_string, " of the Elf Hall");
            break;
        case 21:
            strcat(death_string, " of the Tomb");
            break;
        case 22:
            strcat(death_string, " of the Swamp");
            break;
        }

        strcat(death_string, ".");
        save_ghost();

ending:
        end_game(1);
    }

    end_game(0);                // must have won! (or at least escaped)

}





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
        for (int dungeon = 0; dungeon < MAX_DUNGEONS; dungeon++)
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
        strcpy(info, "You die...");
        mpr(info);
    }

    viewwindow(1);
    more();


    for (i = 0; i < 52; i++)
    {
        you.inv_ident[i] = 3;
    }

    for (i = 0; i < 52; i++)
    {
        if (you.inv_class[i] != 0)
        {
            set_id(you.inv_class[i], you.inv_type[i], 1);
        }
    }

    if (status2 == 0)           /* invent(you.inv_plus2, -1, you.inv_quantity, you.inv_dam, you.inv_class, you.inv_type, you.inv_plus, you.inv_ident, you.equip [EQ_WEAPON], you.equip [EQ_BODY_ARMOUR], you.equip [EQ_SHIELD], you.equip [EQ_HELMET], you.equip [EQ_CLOAK], you.equip [EQ_GLOVES], you.equip [EQ_BOOTS], you.ring, 1); */
        invent(-1, 1);
    else                        /* invent(you.inv_plus2, -1, you.inv_quantity, you.inv_dam, you.inv_class, you.inv_type, you.inv_plus, you.inv_ident, you.equip [EQ_WEAPON], you.equip [EQ_BODY_ARMOUR], you.equip [EQ_SHIELD], you.equip [EQ_HELMET], you.equip [EQ_CLOAK], you.equip [EQ_GLOVES], you.equip [EQ_BOOTS], you.ring, 0); */
        invent(-1, 0);

#ifdef USE_CURSES
    clear();
#endif
    if (dump_char((status2 == 0), "morgue.txt") == 1)
        strcpy(info, "Char dumped successfully (morgue.txt).");
    else
        strcpy(info, "Char dump unsuccessful! Sorry about that.");
    mpr(info);
    more();
    int p = 0;

    for (p = 0; p < 52; p++)
    {
        for (i = 0; i < ITEMS; i++)
        {
            if (mitm.quantity[i] == 0)
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
/*                it_no ++; */
                break;
            }
        }                       // end of i loop

    }                           // end of p loop


    for (i = 0; i < ITEMS; i++)
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
    get_ch();
    end(0);
}



void highscore(char death_string[256], long points)
{
    char high_scores [SCORE_FILE_ENTRIES] [80];
    long scores [SCORE_FILE_ENTRIES];
    int hc = 0;
    int hc2 = 0;
    int hc3 = 0;
    char ready[2];
    int multip = 1;

    ready[0] = 0;
    ready[1] = 0;

    for (hc2 = strlen(death_string); hc2 < 80; hc2++)
    {
        death_string[hc2] = ' ';
    }
    death_string[80] = '\0';

    for (hc = 0; hc < SCORE_FILE_ENTRIES; hc++)
    {
        strcpy(high_scores[hc], "0       empty                                                                  ");
        scores[hc] = 0;
    }

#ifdef SAVE_DIR_PATH
    int handle = open(SAVE_DIR_PATH "scores", S_IWRITE, S_IREAD);
#else
    int handle = open("scores", S_IWRITE, S_IREAD);
#endif

    if (handle == -1)
    {
        goto out_of_ready;
    }

    for (hc = 0; hc < SCORE_FILE_ENTRIES; hc++)
    {
        for (hc2 = 0; hc2 < 80; hc2++)
        {
            read(handle, ready, 1);
            if (ready[0] == 13 || ready[0] == 0
                            || ready[0] == '\r' || ready[0] == '\n')
            {
                for (hc3 = hc2; hc3 < 80; hc3++)
                {
                    high_scores[hc][hc3] = 32;
                    goto out_of_inner;
                }
            }
            if (ready[0] == EOF)
                goto out_of_ready;
            if (ready[0] == 0 || ready[0] == 7)
                ready[0] = 32;
            high_scores[hc][hc2] = ready[0];
        }                       // end for hc2

out_of_inner:
        hc3 = 0;
    }                           // end for hc

    for (hc = 0; hc < SCORE_FILE_ENTRIES; hc++)
    {
        multip = 1;
        for (hc2 = 6; hc2 >= 0; hc2--)
        {
            if (high_scores[hc][hc2] == 32)
                continue;
            scores[hc] += (high_scores[hc][hc2] - 48) * multip;
            multip *= 10;
        }
    }

out_of_ready:
    int placed = 0;
    char has_printed = 0;

        int numEntries = NUMBER_OF_LINES - 7;
    for (hc = 0; hc < numEntries; hc++)
    {
        placed++;
        if (points > scores[hc] && has_printed == 0)
        {
            textcolor(YELLOW);
            itoa(placed, ready, 10);
            cprintf(ready);
            if (strlen(ready) == 1)
                cprintf("- ");
            else
                cprintf("-");
            for (hc2 = 0; hc2 < 80; hc2++)
            {
                 if (death_string[hc2] == 32 && death_string[hc2 + 1] == 32 && hc2 > numEntries)
                {
                    cprintf(EOL);
                    break;
                }
                putch(death_string[hc2]);
            }
            has_printed = 1;
            hc--;
            textcolor(LIGHTGREY);
            continue;
        }
        itoa(placed, ready, 10);
        cprintf(ready);
        if (strlen(ready) == 1)
            cprintf("- ");
        else
            cprintf("-");
        for (hc2 = 0; hc2 < 80; hc2++)
        {
            if (high_scores[hc][hc2] == 32 && high_scores[hc][hc2 + 1] == 32 && hc2 > numEntries)
            {
                cprintf(EOL);
                break;
            }
            putch(high_scores[hc][hc2]);
        }
    }

    close(handle);

//handle = open("scores", O_CREAT || O_TRUNC | O_BINARY, S_IWRITE | S_IREAD);
    //handle = open("scores", O_WRONLY | O_BINARY, S_IWRITE, S_IREAD);
    //handle = open("scores", O_BINARY, 0660);
#ifdef SAVE_DIR_PATH
    handle = open(SAVE_DIR_PATH "scores", O_RDWR | O_CREAT | O_TRUNC | O_BINARY, 0660);         //S_IREAD | S_IWRITE);
#else
    handle = open("scores", O_RDWR | O_CREAT | O_TRUNC | O_BINARY, 0660);       //S_IREAD | S_IWRITE);
#endif

    if (handle == -1)
    {
        perror("oh NO!!!");
        if (getch() == 0)
            getch();
        return;
    }

    for (hc = 0; hc < SCORE_FILE_ENTRIES; hc++)
    {
        for (hc2 = strlen(high_scores[hc]); hc2 < 80; hc2++)
        {
            high_scores[hc][hc2] = ' ';
        }
    }

    has_printed = 0;
    for (hc = 0; hc < SCORE_FILE_ENTRIES; hc++)
    {
        if (has_printed == 1 && hc == SCORE_FILE_ENTRIES - 1)
            break;
        if (points > scores[hc] && has_printed == 0)
        {
            write(handle, death_string, 80);
            hc--;
            has_printed = 1;
        }
        else
            write(handle, high_scores[hc], 80);
    }

    close(handle);

#ifdef SHARED_FILES_CHMOD_VAL

#ifdef SAVE_DIR_PATH
    chmod(SAVE_DIR_PATH "scores", SHARED_FILES_CHMOD_VAL);
#else
    chmod("scores", SHARED_FILES_CHMOD_VAL);
#endif

#endif

}                               // end void highscores




void lose_level(void)
{
    char temp_quant[5];

    if (you.experience_level == 1)
    {
        ouch(-9999, 0, 14);
    }
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

//you.res_magic -= 3;

    int brek = random2(3) + 4;

    if (you.experience_level >= 12)
        brek = random2(3) + 2;
    if (you.experience_level >= 21)
        brek = random2(2) + 2;

    ouch(brek, 0, 14);
    you.base_hp2 -= brek;

    brek = random2(4) + 3;
    if (you.experience_level > 12)
        brek = random2(3) + 2;
    if (you.experience_level > 21)
        brek = random2(2) + 2;


    you.magic_points -= 1;
    you.base_magic_points2--;

    you.spell_levels--;

    calc_hp();
    calc_ep();

    if (you.spell_levels < -40)
        you.spell_levels = -40;
    if (you.magic_points < 0)
        you.magic_points = 0;

    you.redraw_experience = 1;
    you.redraw_hit_points = 1;
    you.redraw_magic_points = 1;

    brek = 0;

}


void drain_exp(void)
{

    if (you.duration[DUR_PRAYER] != 0 && (you.religion == GOD_ZIN || you.religion == GOD_SHINING_ONE || you.religion == GOD_ELYVILON) && random2(150) < you.piety)
    {
        strcpy(info, god_name(you.religion));
        strcat(info, " protects your life force!");
        mpr(info);
        return;
    }


    if (you.experience == 0)
    {
        ouch(-9999, 0, 14);
    }

    if (you.experience_level == 1)
    {
        you.experience = 0;
        return;
    }

    unsigned long total_exp = exp_needed(you.experience_level + 2, you.species) - exp_needed(you.experience_level + 1, you.species);
    unsigned long exp_drained = total_exp * (10 + random() % 11);

    exp_drained /= 100;

    you.experience -= exp_drained;

#ifdef WIZARD
    strcpy(info, "You lose ");
    itoa(exp_drained, temp_quant, 10);
    strcat(info, temp_quant);
    strcat(info, " experience points.");
    mpr(info);
#endif

    you.redraw_experience = 1;

    if (you.experience < exp_needed(you.experience_level + 1, you.species))
    {
        lose_level();
    }

}



int set_status(int stat)
{

    static int stat2;

    if (stat != 100)
        stat2 = stat;

    return stat2;

}

/*
 *  File:       chardump.cc
 *  Summary:    Dumps character info out to the morgue file.
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 *
 *      <3>     6/13/99         BWR             Improved spell listing
 *      <2>     5/30/99         JDJ             dump_spells dumps failure rates (from Brent).
 *      <1>     4/20/99         JDJ             Reformatted, uses string objects, split out 7
 *                                              functions from dump_char, dumps artifact info.
 */

#include "AppHdr.h"
#include "chardump.h"

#ifdef USE_EMX
#include <sys/types.h>
#endif

#ifdef MAC
#include <stat.h>
#else
#include <sys/stat.h>
#endif

#include <string>

#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

#ifdef DOS
#include <conio.h>
#endif

#include "describe.h"
#include "externs.h"
#include "enum.h"
#include "player.h"
#include "religion.h"
#include "itemname.h"
#include "message.h"
#include "mutation.h"
#include "skills.h"
#include "spells0.h"
#include "shopping.h"
#include "version.h"

#include "debug.h"
#include "new.h"

#ifdef MACROS
  #include "macro.h"
#endif

// ========================================================================
//      Internal Functions
// ========================================================================

//---------------------------------------------------------------
//
// munge_description
//
// Convert dollar signs to EOL and word wrap to 80 characters.
// (for some obscure reason get_item_description uses dollar
// signs instead of EOL).
//
//---------------------------------------------------------------
static string munge_description(const string & inStr)
{
    string outStr;

    outStr.reserve(inStr.length() + 32);

    const long kIndent = 3;

    outStr += string(kIndent, ' ');
    long lineLen = kIndent;

    long i = 0;

    while (i < (long)inStr.length())
    {
        char ch = inStr[i];

        if (ch == '$')
        {
            outStr += EOL;
            outStr += string(kIndent, ' ');
            lineLen = kIndent;
            ++i;

        }
        else if (isspace(ch))
        {
            if (lineLen >= 79)
            {
                outStr += EOL;
                outStr += string(kIndent, ' ');
                lineLen = kIndent;

            }
            else if (lineLen > 0)
            {
                outStr += ch;
                ++lineLen;
            }
            ++i;

        }
        else
        {
            string word;

            while (i < (long)inStr.length() && lineLen + (long)word.length() < 79 && !isspace(inStr[i]) && inStr[i] != '$')
            {
                word += inStr[i++];
            }

            if (lineLen + word.length() >= 79)
            {
                outStr += EOL;
                outStr += string(kIndent, ' ');
                lineLen = kIndent;
            }

            outStr += word;
            lineLen += word.length();
        }
    }

    outStr += EOL;

    return outStr;
}


//---------------------------------------------------------------
//
// dump_stats
//
//---------------------------------------------------------------
static void dump_stats(string & text)
{
    char st_prn[15];
    char title[40];

    text += you.your_name;
    text += " the ";
    strcpy(title, skill_title(best_skill(0, 50, 99), you.skills[best_skill(0, 50, 99)], you.char_class, you.experience_level));
    text += title;
    text += " (";
    text += species_name(you.species);
    text += ")";
    text += EOL;

    text += "(Level ";
    itoa(you.experience_level, st_prn, 10);
    text += st_prn;
    text += " ";
    text += you.class_name;
    text += ")";
    text += EOL;

    text += "Experience : ";
    itoa(you.experience_level, st_prn, 10);
    text += st_prn;
    text += "/";
    itoa(you.experience, st_prn, 10);
    text += st_prn;
    text += EOL;

    text += "Strength ";
    itoa(you.strength, st_prn, 10);
    text += st_prn;
    if (you.strength < you.max_strength)
    {
        text += "/";
        itoa(you.max_strength, st_prn, 10);
        text += st_prn;
    }

    text += "     Dexterity ";
    itoa(you.dex, st_prn, 10);
    text += st_prn;
    if (you.dex < you.max_dex)
    {
        text += "/";
        itoa(you.max_dex, st_prn, 10);
        text += st_prn;
    }

    text += "     Intelligence ";
    itoa(you.intel, st_prn, 10);
    text += st_prn;
    if (you.intel < you.max_intel)
    {
        text += "/";
        itoa(you.max_intel, st_prn, 10);
        text += st_prn;
    }
    text += EOL;

    text += "Hit Points : ";
    itoa(you.hp, st_prn, 10);
    text += st_prn;
    if (you.hp < you.hp_max)
    {
        text += "/";
        itoa(you.hp_max, st_prn, 10);
        text += st_prn;
        if (you.hp <= 0)
        {
            if (you.deaths_door == 0)
            {
                text += " ";
                text += "(dead)";
            }
            else
            {
                text += " ";
                text += "(almost dead)";
            }
        }
    }

    text += "          Magic Points : ";
    itoa(you.magic_points, st_prn, 10);
    text += st_prn;
    if (you.magic_points < you.max_magic_points)
    {
        text += "/";
        itoa(you.max_magic_points, st_prn, 10);
        text += st_prn;
    }
    text += EOL;

    text += "AC : ";
    itoa(player_AC(), st_prn, 10);
    text += st_prn;

    text += "          Evasion : ";
    itoa(player_evasion(), st_prn, 10);
    text += st_prn;

    text += "          Shield : ";
    itoa(player_shield_class(), st_prn, 10);
    text += st_prn;
    text += EOL;

    text += "Gp : ";
    itoa(you.gold, st_prn, 10);
    text += st_prn;
    text += EOL;
    text += EOL;
}


//---------------------------------------------------------------
//
// dump_location
//
//---------------------------------------------------------------
static void dump_location(string & text)
{

    if (you.your_level != -1)
        text += "You are ";

    if (you.level_type == 3)
        text += "in Pandemonium";
    else if (you.level_type == 2)
        text += "in the Abyss";
    else if (you.level_type == 1)
        text += "in a labyrinth";
    else if (you.where_are_you == 1)
        text += "in Dis";
    else if (you.where_are_you == 2)
        text += "in Gehenna";
    else if (you.where_are_you == 3)
        text += "in the Vestibule of Hell";
    else if (you.where_are_you == 4)
        text += "in Cocytus";
    else if (you.where_are_you == 5)
        text += "in Tartarus";
    else if (you.where_are_you == 6)
        text += "in the Inferno";
    else if (you.where_are_you == 7)
        text += "in the Pit";
    else if (you.where_are_you == 10)
        text += "in the Mines";
    else if (you.where_are_you == 11)
        text += "in the Hive";
    else if (you.where_are_you == 12)
        text += "in the Lair";
    else if (you.where_are_you == 13)
        text += "in the Slime Pits";
    else if (you.where_are_you == 14)
        text += "in the Vaults";
    else if (you.where_are_you == 15)
        text += "in the Crypt";
    else if (you.where_are_you == 16)
        text += "in the Hall of Blades";
    else if (you.where_are_you == 17)
        text += "in the Hall of Zot";
    else if (you.where_are_you == 18)
        text += "in the Ecumenical Temple";
    else if (you.where_are_you == 19)
        text += "in the Snake Pit";
    else if (you.where_are_you == 20)
        text += "in the Elven Halls";
    else if (you.where_are_you == 21)
        text += "in the Tomb";
    else if (you.where_are_you == 22)
        text += "in the Swamp";
    else
    {
        if (you.your_level == -1)
        {
            text += "You escaped.";

        }
        else
        {
            char st_prn[15];

            text += "on level ";
            itoa(you.your_level + 1, st_prn, 10);
            text += st_prn;
        }
    }
    text += EOL;
}


//---------------------------------------------------------------
//
// dump_religion
//
//---------------------------------------------------------------
static void dump_religion(string & text)
{
    if (you.religion != GOD_NO_GOD)
    {
        text += "You worship ";
        text += god_name(you.religion);
        text += ".";
        text += EOL;

        if (you.religion != GOD_XOM)
        {                       // Xom doesn't care

            text += god_name(you.religion);
            if (you.piety <= 5)
                text += " is displeased.";
            else if (you.piety <= 20)
                text += " is noncommittal.";
            else if (you.piety <= 40)
                text += " is pleased with you.";
            else if (you.piety <= 70)
                text += " is most pleased with you.";
            else if (you.piety <= 100)
                text += " is greatly pleased with you.";
            else if (you.piety <= 130)
                text += " is extremely pleased with you.";
            else
                text += " is exalted by your worship.";

            text += EOL;
        }
    }
}


//---------------------------------------------------------------
//
// dump_inventory
//
//---------------------------------------------------------------
static void dump_inventory(string & text, char show_prices)
{
    int i, j;
    char temp_id[4][50];

    for (i = 0; i < 4; i++)
        for (j = 0; j < 50; j++)
            temp_id[i][j] = 1;

    char st_pass[60];

    strcpy(st_pass, "");

    int inv_class2[15];
    int inv_count = 0;
    char strng[80];

    for (i = 0; i < 15; i++)
        inv_class2[i] = 0;

    for (i = 0; i < 52; i++)
    {
        if (you.inv_quantity[i] != 0)
        {
            inv_class2[you.inv_class[i]]++;     // adds up number of each class in invent.

            inv_count++;
        }
    }

    if (inv_count == 0)
    {
        text += "You aren't carrying anything.";
        text += EOL;

    }
    else
    {
        text += "  Inventory:";
        text += EOL;

        for (i = 0; i < 15; i++)
        {
            if (inv_class2[i] != 0)
            {
                switch (i)
                {
                case OBJ_WEAPONS:
                    text += "Hand weapons";
                    break;
                case OBJ_MISSILES:
                    text += "Missiles";
                    break;
                case OBJ_ARMOUR:
                    text += "Armour";
                    break;
                case OBJ_WANDS:
                    text += "Magical devices";
                    break;
                case OBJ_FOOD:
                    text += "Comestibles";
                    break;
//                                      case 5:                          text += "Books"; break;
                case OBJ_SCROLLS:
                    text += "Scrolls";
                    break;
                case OBJ_JEWELLERY:
                    text += "Jewellery";
                    break;
                case OBJ_POTIONS:
                    text += "Potions";
                    break;
//                                      case 9:                          text += "Gems"; break;
                case OBJ_BOOKS:
                    text += "Books";
                    break;
                case OBJ_STAVES:
                    text += "Magical staves";
                    break;
                case OBJ_ORBS:
                    text += "Orbs of Power";
                    break;
                case OBJ_MISCELLANY:
                    text += "Miscellaneous";
                    break;
                case OBJ_CORPSES:
                    text += "Carrion";
                    break;

                default:
                    DEBUGSTR("Bad item class");
                }
                text += EOL;

                for (j = 0; j < 52; j++)
                {
                    if (you.inv_class[j] == i && you.inv_quantity[j] > 0)
                    {
                        text += " ";

                        char ft;

                        if (j < 26)
                            ft = (char) j + 'a';
                        else
                            ft = (char) j + 'A' - 26;

                        text += ft;
                        text += " - ";

                        /*
                          Remove DML 6/2/99: not used!
                          char yps = wherey();
                        */

                        item_name(you.inv_plus2[j], you.inv_class[j], you.inv_type[j], you.inv_dam[j], you.inv_plus[j], you.inv_quantity[j], you.inv_ident[j], 3, st_pass);
                        text += st_pass;

                        inv_count--;

                        if (j == you.equip[EQ_WEAPON])
                            text += " (weapon)";

                        if (j == you.equip[EQ_BODY_ARMOUR] || j == you.equip[EQ_CLOAK] || j == you.equip[EQ_HELMET] || j == you.equip[EQ_GLOVES] || j == you.equip[EQ_BOOTS] || j == you.equip[EQ_SHIELD])
                            text += " (worn)";

                        if (j == you.equip[EQ_LEFT_RING])
                            text += " (left hand)";

                        if (j == you.equip[EQ_RIGHT_RING])
                            text += " (right hand)";

                        if (j == you.equip[EQ_AMULET])
                            text += " (neck)";

                        if (show_prices == 1)
                        {
                            text += " (";
                            itoa(item_value(you.inv_class[j], you.inv_type[j], you.inv_dam[j], you.inv_plus[j], you.inv_plus2[j], you.inv_quantity[j], 3, temp_id), strng, 10);
                            text += strng;
                            text += "gold)";
                        }

                        text += EOL;

                        if (is_dumpable_artifact(you.inv_class[j], you.inv_type[j], you.inv_plus[j], you.inv_plus2[j], you.inv_dam[j], you.inv_ident[j]))
                        {
                            text += munge_description(get_item_description(you.inv_class[j], you.inv_type[j], you.inv_plus[j], you.inv_plus2[j], you.inv_dam[j], you.inv_ident[j]));
                            text += EOL;
                        }
                    }
                }
            }
        }
    }
}


//---------------------------------------------------------------
//
// dump_skills
//
//---------------------------------------------------------------
static void dump_skills(string & text)
{
    text += EOL;
    text += EOL;
    text += "   Skills:";
    text += EOL;

    for (int i = 0; i < 50; i++)
    {
        if (you.skills[i] > 0)
        {
            if (you.practise_skill[i] == 0)
                text += " - ";
            else
                text += " + ";

            char strng[80];

            text += "Level ";
            itoa(you.skills[i], strng, 10);
            text += strng;
            text += " ";
            text += skill_name(i);
            text += EOL;
        }
    }

    text += EOL;
    text += EOL;
}

//---------------------------------------------------------------
//
// Return string of the i-th spell type, with slash if required
//
//---------------------------------------------------------------
static string spell_type_name( int spell_class, bool slash )
{
    string  ret;

    if (slash)
        ret = "/";

    switch (spell_class) {
    case SPTYP_CONJURATION:
        ret += "Conjuration";
        break;

    case SPTYP_ENCHANTMENT:
        ret += "Enchantment";
        break;

    case SPTYP_FIRE:
        ret += "Fire";
        break;

    case SPTYP_ICE:
        ret += "Ice";
        break;

    case SPTYP_TRANSMIGRATION:
        ret += "Transmigration";
        break;

    case SPTYP_NECROMANCY:
        ret += "Necromancy";
        break;

    case SPTYP_HOLY:
        ret += "Holy";
        break;

    case SPTYP_SUMMONING:
        ret += "Summoning";
        break;

    case SPTYP_DIVINATION:
        ret += "Divination";
        break;

    case SPTYP_TRANSLOCATION:
        ret += "Translocation";
        break;

    case SPTYP_POISON:
        ret += "Poison";
        break;

    case SPTYP_EARTH:
        ret += "Earth";
        break;

    case SPTYP_AIR:
        ret += "Air";
        break;
    }

    return( ret );
}


//---------------------------------------------------------------
//
// dump_spells
//
//---------------------------------------------------------------
static void dump_spells(string & text)
{
    char  strng[80];

    // This array helps output the spell types in the traditional order.
    const int  spell_type_index[] =
    {
        SPTYP_HOLY,
        SPTYP_POISON,
        SPTYP_FIRE,
        SPTYP_ICE,
        SPTYP_EARTH,
        SPTYP_AIR,
        SPTYP_CONJURATION,
        SPTYP_ENCHANTMENT,
        SPTYP_DIVINATION,
        SPTYP_TRANSLOCATION,
        SPTYP_SUMMONING,
        SPTYP_TRANSMIGRATION,
        SPTYP_NECROMANCY,
        0
    };

    if (you.spell_levels == 1)
        text += "You have one spell level left.";
    else if (you.spell_levels == 0)
        text += "You cannot memorise any spells.";
    else
    {
        text += "You have ";
        itoa(you.spell_levels, strng, 10);
        text += strng;
        text += " spell levels left.";
    }

    text += EOL;

    if (you.spell_no == 0)
    {
        text += "You don't know any spells.";
        text += EOL;

    }
    else
    {
        text += "You know the following spells:" EOL;
        text += EOL;

        text += "  Your Spells                       Type                  Success   Level" EOL;

        for (int j = 0; j < 25; j++)
        {
            if (you.spells[j] != 210)
            {
                string spell_line = " ";
                char ft;

                if (j < 26)
                    ft = (char) j + 'a';
                else
                    ft = (char) j + 'A';

                char st_pass[60];

                strng[0] = ft;
                strng[1] = 0;
                spell_line += strng;
                spell_line += " - ";
                spell_name(you.spells[j], st_pass);

                spell_line += st_pass;

                for (int i = spell_line.length(); i < 34; i++)
                {
                    spell_line += ' ';
                }

                bool already = false;
                for (int i = 0; spell_type_index[i] != 0; i++)
                {
                    if (spell_type( you.spells[j], spell_type_index[i] ) == 1)
                    {
                        spell_line += spell_type_name( spell_type_index[i],
                                                                    already );

                        already = true;
                    }
                }

                for (int i = spell_line.length(); i < 58; i++)
                {
                    spell_line += ' ';
                }

                int fail_rate = spell_fail(you.spells[j]);

                if (fail_rate == 100)
                {
                    spell_line += "Useless";
                }
                else if (fail_rate > 90)
                {
                    spell_line += "Terrible";
                }
                else if (fail_rate > 80)
                {
                    spell_line += "Cruddy";
                }
                else if (fail_rate > 70)
                {
                    spell_line += "Bad";
                }
                else if (fail_rate > 60)
                {
                    spell_line += "Very Poor";
                }
                else if (fail_rate > 50)
                {
                    spell_line += "Poor";
                }
                else if (fail_rate > 40)
                {
                    spell_line += "Fair";
                }
                else if (fail_rate > 30)
                {
                    spell_line += "Good";
                }
                else if (fail_rate > 20)
                {
                    spell_line += "Very Good";
                }
                else if (fail_rate > 10)
                {
                    spell_line += "Great";
                }
                else if (fail_rate > 0)
                {
                    spell_line += "Excellent";
                }
                else
                {
                    spell_line += "Perfect";
                }

                for (int i = spell_line.length(); i < 70; i++)
                {
                    spell_line += ' ';
                }

                itoa( (int) spell_value( you.spells[j] ), st_pass, 10 );
                spell_line += st_pass;
                spell_line += EOL;

                text += spell_line;
            }
        }
    }
}


//---------------------------------------------------------------
//
// dump_mutations
//
//---------------------------------------------------------------
static void dump_mutations(string & text)
{
    // Can't use how_mutated() here, as it doesn't count demonic powers
    int xz = 0;

    for (int xy = 0; xy < 100; xy++)
        if (you.mutation[xy] > 0)
            xz++;

    if (xz > 0)
    {
        text += "";
        text += EOL;
        text += "           Mutations & Other Weirdness";
        text += EOL;

        for (int j = 0; j < 100; j++)
        {
            if (you.mutation[j])
            {
                if (you.demon_pow[j] > 0)
                    text += "* ";

                text += mutation_name(j);
                text += EOL;
            }
        }
    }
}

#if MAC
#pragma mark -
#endif

// ========================================================================
//      Public Functions
// ========================================================================

//---------------------------------------------------------------
//
// dump_char
//
// Creates a disk record of a character. Returns true if the
// character was successfully saved.
//
//---------------------------------------------------------------
bool dump_char(char show_prices, char fname[30])        // $$$ a try block?
 {
    bool succeeded = false;

    string text;

    text.reserve(100 * 80);     // start with enough room for 100 80 character lines

    text += " Dungeon Crawl version " VERSION " character file.";
    text += EOL;
    text += EOL;

    dump_stats(text);
    dump_location(text);
    dump_religion(text);

    switch (you.burden_state)
    {
    case 5:
        text += "You are overloaded with stuff.";
        text += EOL;
        break;
    case 2:
        text += "You are encumbered.";
        text += EOL;
        break;
    }

    if (you.hunger <= 1000)
        text += "You are starving.";
    else if (you.hunger <= 2600)
        text += "You are hungry.";
    else if (you.hunger < 7000)
        text += "You are not hungry.";
    else if (you.hunger < 11000)
        text += "You are full.";
    else
        text += "You are completely stuffed.";

    text += EOL;
    text += EOL;

    switch (you.attribute[ATTR_TRANSFORMATION])
    {
    case 1:
        text += "You are in spider-form.";
        text += EOL;
        text += EOL;
        break;
    case 2:
        text += "Your hands are blades.";
        text += EOL;
        text += EOL;
        break;
    case 3:
        text += "You are a stone statue.";
        text += EOL;
        text += EOL;
        break;
    }

    dump_inventory(text, show_prices);

    char strng[80];

    text += EOL;
    text += EOL;
    text += " You have ";
    itoa(you.exp_available, strng, 10);
    text += strng;
    text += " experience left.";

    dump_skills(text);
    dump_spells(text);
    dump_mutations(text);

    char  file_name [kPathLen] = "\0";

    if (sys_env.crawl_dir)
        strncpy( file_name, sys_env.crawl_dir, kPathLen );

    strncat(file_name, fname, kPathLen);

    if (strcmp(fname, "morgue.txt") != 0)
        strncat(file_name, ".txt", kPathLen);

    int handle = open(file_name, O_RDWR | O_CREAT | O_TRUNC | O_BINARY, 0660);

    strcpy(info, "File name: ");
    strcat(info, file_name);
    mpr(info);

    if (handle != -1)
    {
        size_t begin = 0;
        size_t end = text.find(EOL);

      while (end != string::npos)
        {
            end += strlen(EOL);

            size_t len = end - begin;

            if (len > 80)
                len = 80;
            write(handle, text.c_str() + begin, len);

            begin = end;
            end = text.find(EOL, end);
        }

        close(handle);
        succeeded = true;

    }
    else
        mpr("Error opening file.");

    return succeeded;
}

/*
 *  File:       chardump.cc
 *  Summary:    Dumps character info out to the morgue file.
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 *
 * <4> 19 June 2000    GDL Changed handles to FILE *
 * <3> 6/13/99         BWR Improved spell listing
 * <2> 5/30/99         JDJ dump_spells dumps failure rates (from Brent).
 * <1> 4/20/99         JDJ Reformatted, uses string objects, split out 7
 *                         functions from dump_char, dumps artifact info.
  */

 #include "AppHdr.h"
 #include "chardump.h"

 #include <string>
 #include <stdio.h>
 #include <string.h>
 #include <fcntl.h>
 #include <stdlib.h>
 #ifndef __IBMCPP__
 #include <unistd.h>
 #endif
 #include <ctype.h>

 #ifdef USE_EMX
 #include <sys/types.h>
 #endif

 #ifdef MAC
 #include <stat.h>
 #else
 #include <sys/stat.h>
 #endif

 #ifdef DOS
 #include <conio.h>
 #endif

 #include "externs.h"

 #include "debug.h"
 #include "describe.h"
 #include "itemname.h"
 #include "mutation.h"
 #include "new.h"
 #include "player.h"
 #include "religion.h"
 #include "shopping.h"
 #include "skills2.h"
 #include "spells0.h"
 #include "spl-book.h"
 #include "spl-util.h"
 #include "stuff.h"
 #include "version.h"

 #ifdef MACROS
 #include "macro.h"
 #endif


 char verbose_dump;



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
 //  - It uses $ signs because they're easier to manipulate than the EOL
 //  macro, which is of uncertain length (well, that and I didn't know how
 //  to do it any better at the time) (LH)
 //---------------------------------------------------------------
 static string munge_description(const string & inStr)
 {
     string outStr;

     outStr.reserve(inStr.length() + 32);

     const long kIndent = 3;

     outStr += string(kIndent, ' ');
     long lineLen = kIndent;

     long i = 0;

     while (i < (long) inStr.length())
     {
         char ch = inStr[i];

         if (ch == '$')
         {
             outStr += EOL;
             outStr += string(kIndent, ' ');
             lineLen = kIndent;
             ++i;

         }
         else if ( isspace(ch) )
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

             while (i < (long) inStr.length() && lineLen + (long) word.length() < 79 && !isspace(inStr[i]) && inStr[i] != '$')
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

 }          // end munge_description()




 //---------------------------------------------------------------
 //
 // dump_stats
 //
 //---------------------------------------------------------------
 static void dump_stats( string & text )
 {

     char st_prn[15];
     char title[40];

     text += you.your_name;
     text += " the ";
     strcpy(title, skill_title(best_skill(SK_FIGHTING, (NUM_SKILLS - 1), 99), you.skills[best_skill(SK_FIGHTING, (NUM_SKILLS - 1), 99)]));
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

     int max_max_hp = you.hp_max - you.base_hp + 5000;

     if (you.hp < you.hp_max || max_max_hp != you.hp_max)
     {
         text += "/";
         itoa(you.hp_max, st_prn, 10);
         text += st_prn;

         if (max_max_hp != you.hp_max)
         {
             text += " (";
             itoa(max_max_hp, st_prn, 10);
             text += st_prn;
             text += ")";
         }

         if ( you.hp < 1 )
         {
             text += " ";
             text += ( (!you.deaths_door) ? "(dead)" : "(almost dead)" );
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

     text += "GP : ";
     itoa(you.gold, st_prn, 10);
     text += st_prn;
     text += EOL;
     text += EOL;

 }          // end dump_stats()




 //---------------------------------------------------------------
 //
 // dump_location
 //
 //---------------------------------------------------------------
 static void dump_location( string & text )
 {

     if (you.your_level != -1)
         text += "You are ";

     if (you.level_type == LEVEL_PANDEMONIUM)
         text += "in Pandemonium";
     else if (you.level_type == LEVEL_ABYSS)
         text += "in the Abyss";
     else if (you.level_type == LEVEL_LABYRINTH)
         text += "in a labyrinth";
     else if (you.where_are_you == BRANCH_DIS)
         text += "in Dis";
     else if (you.where_are_you == BRANCH_GEHENNA)
         text += "in Gehenna";
     else if (you.where_are_you == BRANCH_VESTIBULE_OF_HELL)
         text += "in the Vestibule of Hell";
     else if (you.where_are_you == BRANCH_COCYTUS)
         text += "in Cocytus";
     else if (you.where_are_you == BRANCH_TARTARUS)
         text += "in Tartarus";
     else if (you.where_are_you == BRANCH_INFERNO)
         text += "in the Inferno";
     else if (you.where_are_you == BRANCH_THE_PIT)
         text += "in the Pit";
     else if (you.where_are_you == BRANCH_ORCISH_MINES)
         text += "in the Mines";
     else if (you.where_are_you == BRANCH_HIVE)
         text += "in the Hive";
     else if (you.where_are_you == BRANCH_LAIR)
         text += "in the Lair";
     else if (you.where_are_you == BRANCH_SLIME_PITS)
         text += "in the Slime Pits";
     else if (you.where_are_you == BRANCH_VAULTS)
         text += "in the Vaults";
     else if (you.where_are_you == BRANCH_CRYPT)
         text += "in the Crypt";
     else if (you.where_are_you == BRANCH_HALL_OF_BLADES)
         text += "in the Hall of Blades";
     else if (you.where_are_you == BRANCH_HALL_OF_ZOT)
         text += "in the Hall of Zot";
     else if (you.where_are_you == BRANCH_ECUMENICAL_TEMPLE)
         text += "in the Ecumenical Temple";
     else if (you.where_are_you == BRANCH_SNAKE_PIT)
         text += "in the Snake Pit";
     else if (you.where_are_you == BRANCH_ELVEN_HALLS)
         text += "in the Elven Halls";
     else if (you.where_are_you == BRANCH_TOMB)
         text += "in the Tomb";
     else if (you.where_are_you == BRANCH_SWAMP)
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

 }          // end dump_location()




 //---------------------------------------------------------------
 //
 // dump_religion
 //
 //---------------------------------------------------------------
 static void dump_religion( string & text )
 {

     if ( you.religion != GOD_NO_GOD )
     {
         text += "You worship ";
         text += god_name(you.religion);
         text += ".";
         text += EOL;

         if ( !player_under_penance() )
         {
             if (you.religion != GOD_XOM)
             {                   // Xom doesn't care
                 text += god_name(you.religion);
                 text += " is ";
                 text += ( (you.piety <=   5) ? "displeased" :
                           (you.piety <=  20) ? "noncommittal" :
                           (you.piety <=  40) ? "pleased with you" :
                           (you.piety <=  70) ? "most pleased with you" :
                           (you.piety <= 100) ? "greatly pleased with you" :
                           (you.piety <= 130) ? "extremely pleased with you"
                                              : "exalted by your worship" );
                 text += ".";
                 text += EOL;
             }
         }
         else
         {
             text += god_name(you.religion);
             text += " is demanding penance.";
             text += EOL;
         }
     }

 }          // end dump_religion()




 //---------------------------------------------------------------
 //
 // dump_inventory
 //
 //---------------------------------------------------------------
 static void dump_inventory( string & text, char show_prices )
 {

     int i, j;
     char temp_id[4][50];

     string text2;

     for (i = 0; i < 4; i++)
       for (j = 0; j < 50; j++)
          temp_id[i][j] = 1;

     char st_pass[60];

     strcpy(st_pass, "");

     int inv_class2[OBJ_GOLD];
     int inv_count = 0;
     char strng[80];

     for (i = 0; i < OBJ_GOLD; i++)
       inv_class2[i] = 0;

     for (i = 0; i < ENDOFPACK; i++)
       if (you.inv_quantity[i] != 0)
       {
           inv_class2[you.inv_class[i]]++;     // adds up number of each class in invent.
           inv_count++;
       }

     if ( !inv_count )
     {
         text += "You aren't carrying anything.";
         text += EOL;
     }
     else
     {
         text += "  Inventory:";
         text += EOL;

         for (i = 0; i < OBJ_GOLD; i++)
         {
             if ( inv_class2[i] != 0 )
             {
                 switch ( i )
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
                   case OBJ_SCROLLS:
                     text += "Scrolls";
                     break;
                   case OBJ_JEWELLERY:
                     text += "Jewellery";
                     break;
                   case OBJ_POTIONS:
                     text += "Potions";
                     break;
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

                for (j = 0; j < ENDOFPACK; j++)
                {
                    if (you.inv_class[j] == i && you.inv_quantity[j] > 0)
                    {
                        text += " ";

                        char ft;

                        ft = index_to_letter(j);

                        text += ft;
                        text += " - ";

                        /*
                           Remove DML 6/2/99: not used!
                           char yps = wherey();
                         */

                        in_name(j, 3, st_pass);
                        text += st_pass;

                        inv_count--;

                        if (j == you.equip[EQ_WEAPON])
                          text += " (weapon)";
                        else if (j == you.equip[EQ_BODY_ARMOUR] || j == you.equip[EQ_CLOAK] || j == you.equip[EQ_HELMET] || j == you.equip[EQ_GLOVES] || j == you.equip[EQ_BOOTS] || j == you.equip[EQ_SHIELD])
                          text += " (worn)";
                        else if (j == you.equip[EQ_LEFT_RING])
                          text += " (left hand)";
                        else if (j == you.equip[EQ_RIGHT_RING])
                          text += " (right hand)";
                        else if (j == you.equip[EQ_AMULET])
                          text += " (neck)";

                        if ( show_prices == 1 )
                        {
                            text += " (";
                            itoa(item_value(you.inv_class[j], you.inv_type[j], you.inv_dam[j], you.inv_plus[j], you.inv_plus2[j], you.inv_quantity[j], 3, temp_id), strng, 10);
                            text += strng;
                            text += "gold)";
                        }

                        //text += EOL;

                        if (is_dumpable_artifact(you.inv_class[j], you.inv_type[j], you.inv_plus[j], you.inv_plus2[j], you.inv_dam[j], you.inv_ident[j], verbose_dump))
                        {
                            text2 = get_item_description(you.inv_class[j], you.inv_type[j], you.inv_plus[j], you.inv_plus2[j], you.inv_dam[j], you.inv_ident[j], verbose_dump);

                            if (text2.length() > 0 && text2[text2.length() - 1] == '$')
                                text2[text2.length() - 1] = '\0';

                            // For some reason, this must be done twice
                            //  or it doesn't work properly for unrandarts
                            if (text2.length() > 0 && text2[text2.length() - 1] == '$')
                                text2[text2.length() - 1] = '\0';

                            //if (text2 [0] == '$')
                            //  text2 [0] = 32;

                            if (text2[0] != '$' && text2.length() > 0)
                                text += EOL;

/*                            if (text2.length() > 0 && text2 [text2.length() - 1] != '$')
   text2 += '$'; */
                            text += munge_description(text2);   // get_item_description(you.inv_class[j], you.inv_type[j], you.inv_plus[j], you.inv_plus2[j], you.inv_dam[j], you.inv_ident[j], verbose_dump));

                        }
                        else
                            text += EOL;


                    }
                }
            }
        }
    }

}          // end dump_inventory()




//---------------------------------------------------------------
//
// dump_skills
//
//---------------------------------------------------------------
static void dump_skills( string & text )
{

    text += EOL;
    text += EOL;
    text += "   Skills:";
    text += EOL;

    for (unsigned char i = 0; i < 50; i++)
      if ( you.skills[i] > 0 )
      {
          text += ( (you.skills[i] == 27)        ? " * " :
                    (!you.practise_skill[i])     ? " - "
                                                 : " + " );

          char strng[80];

          text += "Level ";
          itoa(you.skills[i], strng, 10);
          text += strng;
          text += " ";
          text += skill_name(i);
          text += EOL;
      }

    text += EOL;
    text += EOL;

}          // end dump_skills()




//---------------------------------------------------------------
//
// Return string of the i-th spell type, with slash if required
//
//---------------------------------------------------------------
static string spell_type_name( int spell_class, bool slash )
{

    string ret;

    if ( slash )
      ret = "/";

    ret += spelltype_name(spell_class);

    return (ret);

}          // end spell_type_name()




//---------------------------------------------------------------
//
// dump_spells
//
//---------------------------------------------------------------
static void dump_spells( string & text )
{
    char strng[80];

// This array helps output the spell types in the traditional order.
// this can be tossed as soon as I reorder the enum to the traditional order {dlb}
    const int spell_type_index[] =
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

    int spell_levels = player_spell_levels();

    if (spell_levels == 1)
        text += "You have one spell level left.";
    else if (spell_levels == 0)
        text += "You cannot memorise any spells.";
    else
    {
        text += "You have ";
        itoa(spell_levels, strng, 10);
        text += strng;
        text += " spell levels left.";
    }

    text += EOL;

    if ( !you.spell_no )
    {
        text += "You don't know any spells.";
        text += EOL;

    }
    else
    {
        text += "You know the following spells:" EOL;
        text += EOL;

        text += "  Your Spells                       Type                  Success   Level" EOL;

        for (unsigned char j = 0; j < 25; j++)
        {
            if (you.spells[j] != SPELL_NO_SPELL)
            {
                string spell_line = " ";
                char ft;

                ft = index_to_letter(j);

                char st_pass[60];

                strng[0] = ft;
                strng[1] = '\0';
                spell_line += strng;
                spell_line += " - ";
                spell_line += spell_title(you.spells[j]);

                for (int i = spell_line.length(); i < 34; i++)
                  spell_line += ' ';

                bool already = false;

                for (int i = 0; spell_type_index[i] != 0; i++)
                  if (spell_typematch(you.spells[j], spell_type_index[i]))
                  {
                      spell_line += spell_type_name(spell_type_index[i], already);
                      already = true;
                  }

                for (int i = spell_line.length(); i < 58; i++)
                  spell_line += ' ';

                int fail_rate = spell_fail(you.spells[j]);

                spell_line += (fail_rate == 100) ? "Useless" :
                              (fail_rate  >  90) ? "Terrible" :
                              (fail_rate  >  80) ? "Cruddy" :
                              (fail_rate  >  70) ? "Bad" :
                              (fail_rate  >  60) ? "Very Poor" :
                              (fail_rate  >  50) ? "Poor" :
                              (fail_rate  >  40) ? "Fair" :
                              (fail_rate  >  30) ? "Good" :
                              (fail_rate  >  20) ? "Very Good" :
                              (fail_rate  >  10) ? "Great" :
                              (fail_rate  >   0) ? "Excellent"
                                                 : "Perfect";

                for (int i = spell_line.length(); i < 70; i++)
                  spell_line += ' ';

                itoa((int) spell_difficulty(you.spells[j]), st_pass, 10);
                spell_line += st_pass;
                spell_line += EOL;

                text += spell_line;

            }

        }

    }

}          // end dump_spells()




//---------------------------------------------------------------
//
// dump_mutations
//
//---------------------------------------------------------------
static void dump_mutations( string & text )
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
          if ( you.mutation[j] )
          {
              if ( you.demon_pow[j] > 0 )
                text += "* ";

              text += mutation_name(j);
              text += EOL;
          }
    }

}          // end dump_mutations()




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
bool dump_char( char show_prices, char fname[30] )        // $$$ a try block?
{
  bool succeeded = false;

  string text;

  text.reserve(100 * 80); // start with enough room for 100 80 character lines

  text += " Dungeon Crawl version " VERSION " character file.";
  text += EOL;
  text += EOL;

  dump_stats(text);
  dump_location(text);
  dump_religion(text);

  switch (you.burden_state)
    {
    case BS_OVERLOADED:
      text += "You are overloaded with stuff.";
      text += EOL;
      break;
    case BS_ENCUMBERED:
      text += "You are encumbered.";
      text += EOL;
      break;
    }

  text += "You are ";

  text += ( (you.hunger <= 1000) ? "starving" :
            (you.hunger <= 2600) ? "hungry" :
            (you.hunger <  7000) ? "not hungry" :
            (you.hunger < 11000) ? "full"
            : "completely stuffed" );

  text += ".";
  text += EOL;
  text += EOL;

  if ( you.attribute[ATTR_TRANSFORMATION] )
    {
      switch ( you.attribute[ATTR_TRANSFORMATION] )
        {
        case TRAN_SPIDER:
          text += "You are in spider-form.";
          break;
        case TRAN_BLADE_HANDS:
          text += "Your hands are blades.";
          break;
        case TRAN_STATUE:
          text += "You are a stone statue.";
          break;
        case TRAN_ICE_BEAST:
          text += "You are a creature of crystalline ice.";
          break;
        case TRAN_DRAGON:
          text += "You are a fearsome dragon!";
          break;
        case TRAN_LICH:
          text += "You are in lich-form.";
          break;
        case TRAN_SERPENT_OF_HELL:
          text += "You are a huge, demonic serpent!";
          break;
        case TRAN_AIR:
          text += "You are a cloud of diffuse gas.";
          break;
        }

      text += EOL;
      text += EOL;
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

  char file_name[kPathLen] = "\0";

  if (sys_env.crawl_dir)
    strncpy(file_name, sys_env.crawl_dir, kPathLen);

  strncat(file_name, fname, kPathLen);

  if (strcmp(fname, "morgue.txt") != 0)
    strncat(file_name, ".txt", kPathLen);

  FILE *handle = fopen(file_name, "wb");

#ifdef DEBUG
  strcpy(info, "File name: ");
  strcat(info, file_name);
  mpr(info);
#endif

  if (handle != NULL)
    {
      size_t begin = 0;
      size_t end = text.find(EOL);

      while (end != string::npos)
        {
          end += strlen(EOL);

          size_t len = end - begin;

          if (len > 80)
            len = 80;
          fwrite(text.c_str()+begin, len, 1, handle);

          begin = end;
          end = text.find(EOL, end);
        }

      fclose(handle);
      succeeded = true;

    }
  else
    mpr("Error opening file.");

  return succeeded;
}          // end dump_char()

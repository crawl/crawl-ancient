/*
 *  File:       newgame.cc
 *  Summary:    Functions used when starting a new game.
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 *      <9>      10/12/99       BCR             Made sure all the classes are
 *                                              capitalized correctly.
 *      <8>      9/09/99        BWR             Changed character selection
 *                                              screens look (added sub-species
 *                                              menus from Dustin Ragan)
 *      <7>      7/13/99        BWR             Changed assassins to use
 *                                              hand crossbows, changed
 *                                              rangers into hunters.
 *      <6>      6/22/99        BWR             Added new rangers/slingers
 *      <5>      6/17/99        BCR             Removed some Linux/Mac filename
 *                                              weirdness
 *      <4>      6/13/99        BWR             sys_env support
 *      <3>      6/11/99        DML             Removed tmpfile purging.
 *      <2>      5/20/99        BWR             CRAWL_NAME, new berserk, upped
 *                                              troll food consumption, added
 *                                              demonspawn transmuters.
 *      <1>      -/--/--        LRH             Created
 */

#include "AppHdr.h"
#include "newgame.h"

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
  #include <ctype.h>
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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "externs.h"
#include "enum.h"
#include "files.h"
#include "itemname.h"
#include "player.h"
#include "randart.h"
#include "skills2.h"
#include "stuff.h"
#include "version.h"

#ifdef MACROS
  #include "macro.h"
#endif

char class_allowed(char speci, char char_class);
void init_player(void);
void choose_weapon(void);

extern char wield_change;


char new_game(void)
{
    int handle;
    char your_nam[kNameLen];
    int p;
    char keyn;
    char weap_skill = 0;
    int to_hit_bonus;

    char char_fil[kFileNameSize];

    int i;

    init_player();

    for (i = 0; i < 50; i++)
    {
        you.skills[i] = 0;
        you.skill_points[i] = 0;
        you.practise_skill[i] = 1;
    }
    you.exp_available = 25;




/*you.ring [0] = -1;
   you.ring [1] = -1;
   you.ring [2] = -1;
   you.piety = 50; // noncomittal */

    you.max_level = 1;
    for (i = 0; i < 50; i++)
    {
        you.unique_items[i] = 0;
    }
    you.hunger = 6000;
    // you.hunger_inc = 3;
    you.equip[EQ_WEAPON] = -1;
/*you.rate_regen = 6; */
    you.experience_level = 1;
    you.speed = 10;
    you.pet_target = 201;
    you.your_level = 1;
    you.magic_points_regeneration = 0;


    char j = 0;

    for (i = 0; i < 30; i++)
    {
/*      lev_ex [i] = 0; */
        if (i <= 25)
            you.spells[i] = 210;
    }

    you.your_level = 0;

// Have to add resistances etc to this!!!

    for (i = 0; i < 52; i++)
    {
        you.inv_quantity[i] = 0;
        you.inv_ident[i] = 3;
    }

    textcolor(7);

    if (sys_env.crawl_name)
    {
        // Name is set from environment variable
        strncpy( you.your_name, sys_env.crawl_name, kNameLen );
    }
    else if (you.your_name[0] != '\0')
    {
        // Name was set from the init file
    }
    else
    {
        cprintf(EOL "Hello, and welcome to Dungeon Crawl v" VERSION "!");
        cprintf(EOL "(Copyright 1997, 1998, 1999 Linley Henzell)");
        cprintf(EOL "Please read Crawl.txt for instructions and legal details." EOL EOL);
name_q:
        cprintf("What is your name today? ");

#if defined(LINUX)
        echo();
        getstr(your_nam);
        noecho();

#elif defined(MAC)
        getstr(your_nam, sizeof(your_nam));

#else
        gets(your_nam);
#endif

        strncpy( you.your_name, your_nam, kNameLen );
    }


// *BCR* #ifdef LINUX
/*
 * This shouldn't be here...
 * Why only check for non alphanumerics on Linux?
 */
    // This is an easy way to avoid "bad" characters
    for (unsigned int glorpo = 0; glorpo < strlen(you.your_name); glorpo++)
    {
        if (!isalnum(you.your_name[glorpo]))
        {
            cprintf(EOL "No non-alphanumerics, please." EOL);
            goto name_q;
        }
    }

    if (you.your_name[0] == '\0') {
        cprintf(EOL "That's a silly name!" EOL);
        goto name_q;
    }

// *BCR* #else
/*
 * And why check for a filename called bones onlt when not on Linux?
 */

// if SAVE_DIR_PATH is defined, the userid will be taccked onto the end of
// the character's files, making bones a valid player name.

#ifndef SAVE_DIR_PATH
    /* this would cause big probs with ghosts */
    if (strcmp(you.your_name, "bones") == 0 || strlen(you.your_name) == 0)
    {
        cprintf(EOL "That's a silly name!" EOL);
        goto name_q;
    }
#endif

    unsigned int glorpo = 0;

    for (glorpo = 0; glorpo < strlen(you.your_name); glorpo++)
    {
#ifdef MAC
        if (you.your_name[glorpo] == ':')    // colon is Mac path seperator

        {                       // $$$ shouldn't DOS and Unix path seperators be illegal?

            cprintf(EOL "No colons, please." EOL);
            goto name_q;
        }
#endif
        if (you.your_name[glorpo] == '?')
        {
            cprintf(EOL "No question marks, please." EOL);
            goto name_q;
        }
    }
// *BCR #endif
/*
 * This was the #endif for the #ifdef LINUX above
 */

#ifdef LOAD_UNPACKAGE_CMD
    // Create the file name base
    char name_buff[ kFileNameLen ];

    sprintf(name_buff, SAVE_DIR_PATH "%s%d", you.your_name, getuid());

    char zip_buff[ kFileNameLen ];

    strcpy(zip_buff, name_buff);
    strcat(zip_buff, PACKAGE_SUFFIX);

    // Create save dir name
    strcpy(char_fil, name_buff);
    strcat(char_fil, ".sav");

    handle = open(zip_buff, S_IWRITE, S_IREAD);
    if (handle != -1)
    {
        cprintf(EOL "Loading game..." EOL);

        // Create command
        char cmd_buff[1024];

        sprintf(cmd_buff, LOAD_UNPACKAGE_CMD, name_buff);

        system(cmd_buff);

        close(handle);

        // Remove save game package
        unlink(zip_buff);
    }
    else
    {
  #ifdef DO_ANTICHEAT_CHECKS
        // Simple security patch -- must have zip file otherwise invalidate
        // the character.  Right now this just renames the .sav file to
        // .bak, allowing anyone with the appropriate permissions to
        // fix a character in the case of a bug.  This could be changed
        // to unlinking the file(s) which would remove the character.
        strcat( name_buff, ".bak" );
        rename( char_fil, name_buff );
  #endif
    }

#else
    strcpy(char_fil, "");
    strncat(char_fil, you.your_name, kFileNameLen);
    strcat(char_fil, ".sav");
#endif

    int passout = 0;


    handle = open(char_fil, S_IWRITE, S_IREAD);


    if (handle != -1)
    {
        cprintf(EOL "Welcome back, ");
        cprintf(you.your_name);
        cprintf("!");
        close(handle);

        return 0;
    }
    close(handle);


// This works better if it starts from the top of the screen here. -- bwross
#ifdef SOLARIS
    clrscr();
#else
    cprintf(EOL EOL "");
#endif

// Same with DOS
#ifdef DOS_TERM
    clrscr();
#endif

    cprintf("You must be new here!" EOL);

spec_query2:

    cprintf("You can be:" EOL EOL);

#ifdef SEPARATE_SELECTION_SCREENS_FOR_SUBSPECIES
    cprintf("a - Human                     p - Troll" EOL);
    cprintf("E - Elf                       q - Ogre-Mage" EOL);
    cprintf("D - Dwarf                     r - Draconian" EOL);
    cprintf("i - Halfling                  s - Centaur" EOL);
    cprintf("j - Hill Orc                  t - Demigod" EOL);
    cprintf("k - Kobold                    u - Spriggan" EOL);
    cprintf("l - Mummy                     v - Minotaur" EOL);
    cprintf("m - Naga                      w - Demonspawn" EOL);
    cprintf("n - Gnome                     x - Ghoul" EOL);
    cprintf("o - Ogre                      y - Kenku" EOL);
#else
    cprintf("a - Human                     n - Gnome" EOL);
    cprintf("b - Elf                       o - Ogre" EOL);
    cprintf("c - High Elf                  p - Troll" EOL);
    cprintf("d - Grey Elf                  q - Ogre-Mage" EOL);
    cprintf("e - Deep Elf                  r - Draconian" EOL);
    cprintf("f - Sludge Elf                s - Centaur" EOL);
    cprintf("g - Hill Dwarf                t - Demigod" EOL);
    cprintf("h - Mountain Dwarf            u - Spriggan" EOL);
    cprintf("i - Halfling                  v - Minotaur" EOL);
    cprintf("j - Hill Orc                  w - Demonspawn" EOL);
    cprintf("k - Kobold                    x - Ghoul" EOL);
    cprintf("l - Mummy                     y - Kenku" EOL);
    cprintf("m - Naga" EOL);
#endif

    cprintf(EOL "? - Random                    X - Quit" EOL);

    cprintf(EOL "Which one? ");


spec_query:
    keyn = getch();
    if (keyn == 0)
    {
        getch();
        goto spec_query;
    }

switch_start:
    switch (keyn)
    {
    case 'E':
        clrscr();
        cprintf("What type of elf?" EOL EOL);

        cprintf("a - Common Elf" EOL);
        cprintf("b - High Elf" EOL);
        cprintf("c - Grey Elf" EOL);
        cprintf("d - Deep Elf" EOL);
        cprintf("e - Sludge Elf" EOL);

        cprintf(EOL "? - Random; x - Back to species selection; X - Quit" EOL);
        cprintf(EOL "Which one? ");

        do {
            keyn = getch();
        } while (keyn != '?' && keyn != 'x' && keyn != 'X'
                                        && !(keyn >= 'a' && keyn <= 'e'));

        if (keyn == '?')
        {
            keyn = 'a' + random2(5);
        }
        else if (keyn == 'x')
        {
            clrscr();
            cprintf(EOL EOL);
            goto spec_query2;
        }
        else if (keyn == 'X')
        {
            cprintf(EOL "Goodbye!");
            end(0);
        }

        switch (keyn)
        {
        case 'a':
            you.species = SP_ELF;   // elf
            you.strength = 5;
            you.intel = 8;
            you.dex = 8;
            break;

        case 'b':
            you.species = SP_HIGH_ELF;      // high-elf
            you.strength = 5;
            you.intel = 9;
            you.dex = 8;
            break;

        case 'c':
            you.species = SP_GREY_ELF;      // grey-elf
            you.strength = 4;
            you.intel = 9;
            you.dex = 8;
            break;

        case 'd':
            you.species = SP_DEEP_ELF;      // deep elf
            you.strength = 4;
            you.intel = 10;
            you.dex = 7;
            break;

        case 'e':
            you.species = SP_SLUDGE_ELF;    // sludge elf
            you.strength = 5;
            you.intel = 7;
            you.dex = 8;
            break;
        }
        break;

    case 'D':
        clrscr();
        cprintf("What type of dwarf?" EOL EOL);

        cprintf("a - Hill Dwarf" EOL);
        cprintf("b - Mountain Dwarf" EOL);

        cprintf(EOL "? - Random; x - Back to species selection; X - Quit" EOL);
        cprintf(EOL "Which one? ");

        do {
            keyn = getch();
        } while (keyn != '?' && keyn != 'x' && keyn != 'X'
                                        && !(keyn >= 'a' && keyn <= 'b'));

        if (keyn == '?')
        {
            keyn = 'a' + random2(2);
        }
        else if (keyn == 'x')
        {
            clrscr();
            cprintf(EOL EOL);
            goto spec_query2;
        }
        else if (keyn == 'X')
        {
            cprintf(EOL "Goodbye!");
            end(0);
        }

        switch (keyn)
        {
        case 'a':
            you.species = SP_HILL_DWARF;    // hill dwarf
            you.strength = 9;
            you.intel = 3;
            you.dex = 4;
            break;

        case 'b':
            you.species = SP_MOUNTAIN_DWARF;        // mountain dwarf
            you.strength = 9;
            you.intel = 4;
            you.dex = 5;
            break;
        }
        break;

    case 'a':
        you.species = SP_HUMAN; // human
        you.strength = 6;
        you.intel = 6;
        you.dex = 6;
        break;

    case 'b':
        you.species = SP_ELF;   // elf
        you.strength = 5;
        you.intel = 8;
        you.dex = 8;
        break;

    case 'c':
        you.species = SP_HIGH_ELF;      // high-elf
        you.strength = 5;
        you.intel = 9;
        you.dex = 8;
        break;

    case 'd':
        you.species = SP_GREY_ELF;      // grey-elf
        you.strength = 4;
        you.intel = 9;
        you.dex = 8;
        break;

    case 'e':
        you.species = SP_DEEP_ELF;      // deep elf
        you.strength = 4;
        you.intel = 10;
        you.dex = 7;
        break;

    case 'f':
        you.species = SP_SLUDGE_ELF;    // sludge elf
        you.strength = 5;
        you.intel = 7;
        you.dex = 8;
        break;

    case 'g':
        you.species = SP_HILL_DWARF;    // hill dwarf
        you.strength = 9;
        you.intel = 3;
        you.dex = 4;
        break;

    case 'h':
        you.species = SP_MOUNTAIN_DWARF;        // mountain dwarf
        you.strength = 9;
        you.intel = 4;
        you.dex = 5;
        break;

    case 'i':
        you.species = SP_HALFLING;      // halfling
        you.strength = 4;
        you.intel = 6;
        you.dex = 9;
        break;

    case 'j':
        you.species = SP_HILL_ORC;      // hill orc
        you.strength = 9;
        you.intel = 3;
        you.dex = 4;
        break;

    case 'k':
        you.species = SP_KOBOLD;        // kobold
        you.strength = 4;
        you.intel = 4;
        you.dex = 6;
        break;

    case 'l':
        you.species = SP_MUMMY; // Mummy
        you.strength = 7;
        you.intel = 3;
        you.dex = 3;
        break;

    case 'm':
        you.species = SP_NAGA;  // Naga
        you.strength = 8;
        you.intel = 6;
        you.dex = 4;
        break;

    case 'n':
        you.species = SP_GNOME; // gnome
        you.strength = 5;
        you.intel = 6;
        you.dex = 7;
        break;

    case 'o':
        you.species = SP_OGRE;  // ogre
        you.strength = 12;
        you.intel = 3;
        you.dex = 1;
        break;

    case 'p':
        you.species = SP_TROLL; // troll
        you.strength = 13;
        you.intel = 3;
        you.dex = 0;
        break;

    case 'q':
        you.species = SP_OGRE_MAGE;     // ogre mage
        you.strength = 8;
        you.intel = 6;
        you.dex = 2;
        break;

    case 'r':                           // draconian
        you.strength = 7;
        you.intel = 6;
        you.dex = 2;

#ifdef ALLOW_DRACONIAN_TYPE_SELECTION

        clrscr();
        cprintf("What type of draconian are you?" EOL EOL);

        cprintf("a - Red Draconian" EOL);
        cprintf("b - White Draconian" EOL);
        cprintf("c - Green Draconian" EOL);
        cprintf("d - Gold Draconian" EOL);
        cprintf("e - Grey Draconian" EOL);
        cprintf("f - Black Draconian" EOL);
        cprintf("g - Purple Draconian" EOL);
        cprintf("h - Mottled Draconian" EOL);
        cprintf("i - Pale Draconian" EOL);

        cprintf(EOL "? - Random; x - Back to species selection; X - Quit" EOL);
        cprintf(EOL "Which one? ");

        do {
            keyn = getch();
        } while (keyn != '?' && keyn != 'x' && keyn != 'X'
                                            && !(keyn >= 'a' && keyn <= 'i'));

        if (keyn == '?')
        {
            you.species = SP_RED_DRACONIAN + random2(9);
        }
        else if (keyn == 'x')
        {
            clrscr();
            cprintf(EOL EOL);
            goto spec_query2;
        }
        else if (keyn == 'X')
        {
            cprintf(EOL "Goodbye!");
            end(0);
        }
        else
        {
            you.species = SP_RED_DRACONIAN + keyn - 'a';
        }

#else
        you.species = SP_RED_DRACONIAN + random2(9);    // Draconian
#endif
        break;

    case 's':
        you.species = SP_CENTAUR;       // centaur
        you.strength = 8;
        you.intel = 4;
        you.dex = 5;
        break;

    case 't':
        you.species = SP_DEMIGOD;
        // demigod - more is added to stats later
        you.strength = 7;
        you.intel = 7;
        you.dex = 7;
        break;

    case 'u':
        you.species = SP_SPRIGGAN;      // spriggan
        you.strength = 3;
        you.intel = 6;
        you.dex = 8;
        break;

    case 'v':
        you.species = SP_MINOTAUR;      // minotaur
        you.strength = 10;
        you.intel = 3;
        you.dex = 5;
        break;

    case 'w':
        you.species = SP_DEMONSPAWN;
        // demonspawn - more is added, like demigods
        you.strength = 4;
        you.intel = 4;
        you.dex = 4;
        break;

    case 'x':
        you.species = SP_GHOUL; // Ghoul
        you.strength = 7;
        you.intel = 1;
        you.dex = 2;
        break;

    case 'y':
        you.species = SP_KENKU; // Kenku
        you.strength = 5;
        you.intel = 6;
        you.dex = 7;
        break;

    case '?':
        keyn = 97 + random2(25);
        goto switch_start;

    case 'X':
        cprintf(EOL "Goodbye!");
        end(0);
        break;

    default:
        // cprintf("\n\rDon't be silly. ");
        goto spec_query;
    }

    // Otherwise it looks ugly under Win NT, or so I'm told
    clrscr();

    cprintf(EOL EOL);
    cprintf("Welcome, ");
    cprintf(you.your_name);
    cprintf(" the ");
    cprintf(species_name(you.species));
    cprintf("." EOL EOL);

    cprintf("You can be any of the following :" EOL);

    j = 0;

    for (i = 0; i < 30; i++)
    {
        if (i == 23)
            i = 24;

        if (class_allowed(you.species, i) == 0)
            continue;

        if (i < 26)
            putch(i + 97);
        else
            putch(i + 39);
        cprintf(" - ");

        switch (i)
        {
        case JOB_FIGHTER:
            cprintf("Fighter");
            break;
        case JOB_WIZARD:
            cprintf("Wizard");
            break;
        case JOB_PRIEST:
            cprintf("Priest");
            break;
        case JOB_THIEF:
            cprintf("Thief");
            break;
        case JOB_GLADIATOR:
            cprintf("Gladiator");
            break;
        case JOB_NECROMANCER:
            cprintf("Necromancer");
            break;
        case JOB_PALADIN:
            cprintf("Paladin");
            break;
        case JOB_ASSASSIN:
            cprintf("Assassin");
            break;
        case JOB_BERSERKER:
            cprintf("Berserker");
            break;
        case JOB_HUNTER:
            cprintf("Hunter");
            break;
        case JOB_CONJURER:
            cprintf("Conjurer");
            break;
        case JOB_ENCHANTER:
            cprintf("Enchanter");
            break;
        case JOB_FIRE_ELEMENTALIST:
            cprintf("Fire Elementalist");
            break;
        case JOB_ICE_ELEMENTALIST:
            cprintf("Ice Elementalist");
            break;
        case JOB_SUMMONER:
            cprintf("Summoner");
            break;
        case JOB_AIR_ELEMENTALIST:
            cprintf("Air Elementalist");
            break;
        case JOB_EARTH_ELEMENTALIST:
            cprintf("Earth Elementalist");
            break;
        case JOB_CRUSADER:
            cprintf("Crusader");
            break;
        case JOB_DEATH_KNIGHT:
            cprintf("Death knight");
            break;
        case JOB_VENOM_MAGE:
            cprintf("Venom Mage");
            break;
        case JOB_CHAOS_KNIGHT:
            cprintf("Chaos Knight");
            break;
        case JOB_TRANSMUTER:
            cprintf("Transmuter");
            break;
        case JOB_HEALER:
            cprintf("Healer");
            break;
        case JOB_REAVER:
            cprintf("Reaver");
            break;
        case JOB_STALKER:
            cprintf("Stalker");
            break;
        case JOB_MONK:
            cprintf("Monk");
            break;
        case JOB_WARPER:
            cprintf("Warper");
            break;
            /* when adding more, also add to the range of the loop */
        }

        if (j % 2 == 1)
            cprintf(EOL);
        else
            gotoxy(40, wherey());
        j++;

    }
    if (wherex() >= 40)
        cprintf(EOL);

    cprintf(EOL "? - Random; x - Back to species selection; X - Quit" EOL);
    //cprintf("x - Back to species selection\n\r");
    //cprintf("X - Quit\n\r");


    cprintf(EOL "What kind of character are you? ");
query:
    keyn = getch();


query5:
    if (keyn == 'a')
        you.char_class = JOB_FIGHTER;
    else if (keyn == 'b')
        you.char_class = JOB_WIZARD;
    else if (keyn == 'c')
        you.char_class = JOB_PRIEST;
    else if (keyn == 'd')
        you.char_class = JOB_THIEF;
    else if (keyn == 'e')
        you.char_class = JOB_GLADIATOR;
    else if (keyn == 'f')
        you.char_class = JOB_NECROMANCER;
    else if (keyn == 'g')
        you.char_class = JOB_PALADIN;
    else if (keyn == 'h')
        you.char_class = JOB_ASSASSIN;
    else if (keyn == 'i')
        you.char_class = JOB_BERSERKER;
    else if (keyn == 'j')
        you.char_class = JOB_HUNTER;
    else if (keyn == 'k')
        you.char_class = JOB_CONJURER;
    else if (keyn == 'l')
        you.char_class = JOB_ENCHANTER;
    else if (keyn == 'm')
        you.char_class = JOB_FIRE_ELEMENTALIST;
    else if (keyn == 'n')
        you.char_class = JOB_ICE_ELEMENTALIST;
    else if (keyn == 'o')
        you.char_class = JOB_SUMMONER;
    else if (keyn == 'p')
        you.char_class = JOB_AIR_ELEMENTALIST;
    else if (keyn == 'q')
        you.char_class = JOB_EARTH_ELEMENTALIST;
    else if (keyn == 'r')
        you.char_class = JOB_CRUSADER;
    else if (keyn == 's')
        you.char_class = JOB_DEATH_KNIGHT;
    else if (keyn == 't')
        you.char_class = JOB_VENOM_MAGE;
    else if (keyn == 'u')
        you.char_class = JOB_CHAOS_KNIGHT;
    else if (keyn == 'v')
        you.char_class = JOB_TRANSMUTER;
    else if (keyn == 'w')
        you.char_class = JOB_HEALER;
    else if (keyn == 'y')
        you.char_class = JOB_REAVER;
    else if (keyn == 'z')
        you.char_class = JOB_STALKER;
    else if (keyn == 'A')
        you.char_class = JOB_MONK;
    else if (keyn == 'B')
        you.char_class = JOB_WARPER;
    else if (keyn == '?')
    {
        do
        {
            do
            {
                keyn = 97 + random2(28);
            }
            while (keyn == 'x');
            if (keyn == '{')
                keyn = 'A';
            if (keyn == '|')
                keyn = 'B';
        }
        while (class_allowed(you.species, keyn - 97) == 0);
        goto query5;
    }
    else if (keyn == 'x')
    {
        clrscr();
        cprintf(EOL EOL);
        goto spec_query2;
    }
    else if (keyn == 'X')
    {
        cprintf(EOL "Goodbye!");
        end(0);
    }
    else
    {
cant_be_that:           //cprintf("\n\rI'm sorry, you can't be that. ");
        goto query;
    }

    if (class_allowed(you.species, you.char_class) == 0)
        goto cant_be_that;

    for (i = 0; i < 52; i++)
    {
        you.inv_quantity[i] = 0;
        you.inv_class[i] = OBJ_WEAPONS;
        you.inv_type[i] = WPN_CLUB;
        you.inv_plus[i] = 0;
        you.inv_plus2[i] = 0;
        you.inv_dam[i] = 0;
        you.inv_colour[i] = 0;
    }

    for (i = 0; i < 30; i++)
    {
        you.attribute[i] = 0;
    }


    switch (you.char_class)
    {
    case JOB_FIGHTER:           // fighter

        strcpy(you.class_name, "fighter");
        you.hp = 15;
        you.hp_max = 15;
        you.magic_points = 0;
        you.max_magic_points = 0;

        you.inv_quantity[0] = 1;
        you.inv_class[0] = OBJ_WEAPONS;
        you.inv_type[0] = WPN_SHORT_SWORD;

        you.inv_plus[0] = 50;
        you.inv_plus2[0] = 50;
        you.inv_dam[0] = 0;
        you.inv_colour[0] = LIGHTCYAN;

        if (you.species == SP_OGRE || you.species == SP_TROLL
                            || (you.species >= SP_RED_DRACONIAN
                                    && you.species <= SP_UNK2_DRACONIAN))
        {
            you.inv_quantity[1] = 1;
            you.inv_class[1] = OBJ_ARMOUR;
            you.inv_type[1] = ARM_ANIMAL_SKIN;
            you.inv_plus[1] = 50;
            you.inv_dam[1] = 0;
            you.inv_colour[1] = BROWN;

            if (you.species == SP_OGRE)
            {
                you.inv_quantity[0] = 1;
                you.inv_class[0] = OBJ_WEAPONS;
                you.inv_type[0] = WPN_CLUB;
                you.inv_plus[0] = 50;
                you.inv_dam[0] = 0;
                you.inv_colour[0] = BROWN;

            }

            if (you.species == SP_TROLL)
            {
                you.inv_quantity[0] = 0;
                you.inv_class[0] = OBJ_WEAPONS;
                you.inv_type[0] = WPN_CLUB;
                you.inv_plus[0] = 50;
                you.inv_dam[0] = 0;
                you.inv_colour[0] = BROWN;

            }

            if (you.species >= SP_RED_DRACONIAN
                                        && you.species <= SP_UNK2_DRACONIAN)
            {
                you.inv_quantity[2] = 1;
                you.inv_class[2] = OBJ_ARMOUR;
                you.inv_type[2] = ARM_SHIELD;
                you.inv_plus[2] = 50;
                you.inv_dam[2] = 0;
                you.inv_colour[2] = LIGHTCYAN;
            }


        }
        else if (you.species == SP_GHOUL || you.species == SP_MUMMY)
        {
            you.inv_quantity[1] = 1;
            you.inv_class[1] = OBJ_ARMOUR;
            you.inv_type[1] = ARM_ROBE;
            you.inv_plus[1] = 50;
            you.inv_dam[1] = 0;
            you.inv_colour[1] = WHITE;  /* grave shroud */

            if (you.species == SP_MUMMY)
            {
                you.inv_quantity[2] = 1;
                you.inv_class[2] = OBJ_ARMOUR;
                you.inv_type[2] = ARM_SHIELD;
                you.inv_plus[2] = 50;
                you.inv_dam[2] = 0;
                you.inv_colour[2] = LIGHTCYAN;
            }

        }
        else if (you.species == SP_KOBOLD)
        {
            you.inv_quantity[1] = 1;
            you.inv_class[1] = OBJ_ARMOUR;
            you.inv_type[1] = ARM_LEATHER_ARMOUR;
            you.inv_plus[1] = 50;
            you.inv_dam[1] = 0;
            you.inv_colour[1] = BROWN;
/*      you.AC = 2;
   you.evasion = 9; */

            you.inv_quantity[2] = random2(8) + random2(8) + 8;
            you.inv_class[2] = OBJ_MISSILES;
            you.inv_type[2] = MI_DART;  //wtype;

            you.inv_plus[2] = 50;
            you.inv_dam[2] = 0;
            you.inv_colour[2] = LIGHTCYAN;

        }
        else
        {
            you.inv_quantity[1] = 1;
            you.inv_class[1] = OBJ_ARMOUR;
            you.inv_type[1] = ARM_SCALE_MAIL;
            you.inv_plus[1] = 50;
            you.inv_dam[1] = 0;
            you.inv_colour[1] = LIGHTCYAN;

            you.inv_quantity[2] = 1;
            you.inv_class[2] = OBJ_ARMOUR;
            you.inv_type[2] = ARM_SHIELD;
            you.inv_plus[2] = 50;
            you.inv_dam[2] = 0;
            you.inv_colour[2] = LIGHTCYAN;

/*      you.AC = 3;
   you.evasion = 9; */
            choose_weapon();
            cprintf(EOL "A fine choice.");
        }

        // 11 = spear
        // 9 = hand ae
        // 3 = dagger

        you.strength += 7;
        you.dex += 3;

        if (you.species != SP_TROLL)
            you.equip[EQ_WEAPON] = 0;

        you.equip[EQ_BODY_ARMOUR] = 1;

        if (you.species != SP_KOBOLD && you.species != SP_OGRE
                        && you.species != SP_TROLL && you.species != SP_GHOUL)
            you.equip[EQ_SHIELD] = 2;

        you.gold = random2(10);
/* you.res_magic = 3; */

        you.skills[SK_FIGHTING] = 3;
        weap_skill = 2;
        if (you.species == SP_KOBOLD)
        {
            you.skills[SK_THROWING] = 2;
            you.skills[SK_DODGING] = 1;
            you.skills[SK_STEALTH] = 1;
            you.skills[SK_STABBING] = 1;
            you.skills[SK_DODGING + random() % 3]++;
        }
        else if (you.species != SP_OGRE && you.species != SP_TROLL)
        {
            // Players get dodging or armour skill depending on their
            // starting armour now (note: the armour has to be quiped
            // for this function to work)
            if (player_light_armour())
                you.skills[SK_DODGING] = 2;
            else
                you.skills[SK_ARMOUR] = 2;

            you.skills[SK_SHIELDS] = 2;
            you.skills[SK_STABBING + random() % 2]++;
            you.skills[SK_THROWING] = 2;
            // you.skills[SK_UNARMED_COMBAT] = 1;
        }
        else
            you.skills[SK_FIGHTING] += 2;
        break;

    case JOB_WIZARD:            // wizard

        strcpy(you.class_name, "Wizard");
        you.hp = 10;
        you.hp_max = 10;
        you.magic_points = 3;
        you.max_magic_points = 3;
/*      you.f_abil = 5;
   you.mag_abil = 10;
   you.thr_abil = 5;
   you.speed = 10; */
        you.inv_quantity[0] = 1;
        you.inv_class[0] = OBJ_WEAPONS;

        if (you.species == SP_OGRE_MAGE)
        {
            you.inv_type[0] = WPN_QUARTERSTAFF;
            you.inv_colour[0] = BROWN;
        }
        else
        {
            you.inv_type[0] = WPN_DAGGER;
            you.inv_colour[0] = LIGHTCYAN;
        }

        you.inv_plus[0] = 50;
        you.inv_plus2[0] = 50;
        you.inv_dam[0] = 0;

        // Robe
        you.inv_quantity[1] = 1;
        you.inv_class[1] = OBJ_ARMOUR;
        you.inv_type[1] = ARM_ROBE;
        you.inv_plus[1] = 50;
        you.inv_dam[1] = 1 * 30;
        if (random2(3) != 0)
            you.inv_dam[1] = (4 + random2(2)) * 30;
        you.inv_colour[1] = random2(15) + 1;

/*      you.AC = 1;
   you.evasion = 10; */

        // 11 = spear
        // 9 = hand ae
        // 3 = dagger

//      } // end of switch random2(4)

//      int leftover = 8;

//      you.strength = 6;
        you.dex += 3;
        you.intel += 7;

        you.equip[EQ_WEAPON] = 0;
        you.equip[EQ_BODY_ARMOUR] = 1;

        you.gold = random2(10);

        // extra items being tested:

        you.inv_class[2] = OBJ_BOOKS;
        you.inv_type[2] = random2(3);
//      strcpy(you.inv_name [23], "wand of invisibility");
        you.inv_quantity[2] = 1;
        you.inv_plus[2] = 127;
        you.inv_dam[2] = 1;
        // you.num_inv_items = 13;
        you.inv_colour[2] = 3;

/* you.res_magic = 10; */

        you.skills[SK_DODGING] = 1;
        you.skills[SK_STEALTH] = 1;
        you.skills[SK_DODGING + random() % 2]++;

/*        if (you.skills [SK_DODGING] == 2) you.evasion ++; */

        you.skills[SK_SPELLCASTING] = 2;
        you.skills[SK_CONJURATIONS] = 1;
        you.skills[SK_ENCHANTMENTS] = 1;
        you.skills[SK_SPELLCASTING + random() % 3]++;
        you.skills[SK_SUMMONINGS + random() % 5]++;

        you.skills[SK_SHORT_BLADES] = 1;
        you.skills[SK_STAVES] = 1;
        break;


    case JOB_PRIEST:            // priest

        strcpy(you.class_name, "priest");
        you.piety = 45;
        you.hp = 12;
        you.hp_max = 12;
        you.magic_points = 1;
        you.max_magic_points = 1;
/*      you.f_abil = 7;
   you.mag_abil = 7;
   you.thr_abil = 5;
   you.speed = 10; */

        you.inv_quantity[0] = 1;
        you.inv_class[0] = OBJ_WEAPONS;
        //switch(random2(6))
        //{
        //      case 0 : you.inv_type [0] = 1; damage = 6; break;
        //      case 1 : you.inv_type [0] = 2; damage = 7; break;
        //      case 2 : you.inv_type [0] = 4; damage = 7; break;
        //      case 3 : you.inv_type [0] = 9; damage = 7; break;
        //case 4 :
        you.inv_type[0] = WPN_MACE;
        //case 5 : you.inv_type [0] = 5; damage = 6; break;
        //}


        you.inv_plus[0] = 50;
        you.inv_plus2[0] = 50;
        you.inv_dam[0] = 0;
        you.inv_colour[0] = LIGHTCYAN;


        // Robe
        you.inv_quantity[1] = 1;
        you.inv_class[1] = OBJ_ARMOUR;
        you.inv_type[1] = ARM_ROBE;
        you.inv_plus[1] = 50;
        you.inv_dam[1] = 0;
        you.inv_colour[1] = WHITE;

/*      you.AC = 1;
   you.evasion = 10; */

        you.inv_class[2] = OBJ_POTIONS;
        you.inv_type[2] = POT_HEALING;
//      strcpy(you.inv_name [23], "wand of invisibility");
        you.inv_quantity[2] = 2 + random2(2);
        you.inv_plus[2] = 0;
        you.inv_dam[2] = 0;
        // you.num_inv_items = 13;
        you.inv_colour[2] = random2(15) + 1;    // hmmm...


        // 11 = spear
        // 9 = hand ae
        // 3 = dagger

//      } // end of switch random2(4)

//      int leftover = 8;

        you.strength += 4;
        you.dex += 2;
        you.intel += 4;

        you.equip[EQ_WEAPON] = 0;
        you.equip[EQ_BODY_ARMOUR] = 1;

        you.gold = random2(10);
/* you.res_magic = 10; */

        you.skills[SK_FIGHTING] = 2;
        you.skills[SK_DODGING] = 1;
        you.skills[SK_SHIELDS] = 1;
// you.skills [SK_THROWING] = 2;
        you.skills[SK_MACES_FLAILS] = 2;
        you.skills[SK_STAVES] = 1;

        you.skills[SK_INVOCATIONS] = 4;

        clrscr();
        cprintf(EOL " Which God do you wish to serve?" EOL);
        cprintf("a - Zin (for traditional priests)" EOL);
        cprintf("b - Yredelemnul (for priests of death)" EOL);

getkey:
        keyn = get_ch();
        switch (keyn)
        {
        case 'a':
            you.religion = GOD_ZIN;
            cprintf(EOL "Spread the light, my child...");
            break;
        case 'b':
            you.religion = GOD_YREDELEMNUL;
            you.inv_type[0] = WPN_MACE;
            cprintf(EOL "Welcome...");
            break;
        default:
            goto getkey;
        }


        break;



    case JOB_THIEF:             // thief

        strcpy(you.class_name, "Thief");
        you.hp = 11;
        you.hp_max = 11;
        you.magic_points = 0;
        you.max_magic_points = 0;
/*      you.f_abil = 7;
   you.mag_abil = 6;
   you.thr_abil = 12;
   you.speed = 10; */

        you.inv_quantity[0] = 1;
        you.inv_class[0] = OBJ_WEAPONS;
        you.inv_type[0] = WPN_SHORT_SWORD;      // damage = 6; //break;
        you.inv_plus[0] = 50;
        you.inv_plus2[0] = 50;
        you.inv_dam[0] = 0;
        you.inv_colour[0] = LIGHTCYAN;

        you.inv_quantity[1] = 1;
        you.inv_class[1] = OBJ_WEAPONS;
        you.inv_type[1] = WPN_DAGGER;      // damage = 6; //break;
        you.inv_plus[1] = 50;
        you.inv_plus2[1] = 50;
        you.inv_dam[1] = 0;
        you.inv_colour[1] = LIGHTCYAN;

        // Robe
        you.inv_quantity[2] = 1;
        you.inv_class[2] = OBJ_ARMOUR;
        you.inv_type[2] = ARM_ROBE;
        you.inv_plus[2] = 50;
        you.inv_dam[2] = 0;
        you.inv_colour[2] = BROWN;

        you.inv_quantity[3] = 1;
        you.inv_class[3] = OBJ_ARMOUR;
        you.inv_type[3] = ARM_CLOAK;
        you.inv_plus[3] = 50;
        you.inv_dam[3] = 0;
        you.inv_colour[3] = DARKGREY;

        you.inv_quantity[4] = random2(10) + random2(10) + 10;
        you.inv_class[4] = OBJ_MISSILES;
        you.inv_type[4] = MI_DART;      //wtype;
        you.inv_plus[4] = 50;
        you.inv_dam[4] = 0;
        you.inv_colour[4] = LIGHTCYAN;

/* you.AC = 3;
   you.evasion = 9; */

        // 11 = spear
        // 9 = hand axe
        // 3 = dagger

        you.strength += 3;
        you.dex += 5;
        you.intel += 2;

        you.equip[EQ_WEAPON] = 0;
        you.equip[EQ_BODY_ARMOUR] = 2;
        you.equip[EQ_CLOAK] = 3;

        you.gold = random2(10);
/* you.res_magic = 3; */

        you.skills[SK_FIGHTING] = 1;
        you.skills[SK_SHORT_BLADES] = 2;

        you.skills[SK_DODGING] = 2;
        you.skills[SK_STEALTH] = 2;
        you.skills[SK_STABBING] = 1;
        you.skills[SK_DODGING + random() % 3]++;

/* if (you.skills [SK_DODGING] == 2) you.evasion ++; */

        you.skills[SK_THROWING] = 1;
        you.skills[SK_DARTS] = 1;

        you.skills[SK_TRAPS_DOORS] = 2;

        break;

    case JOB_GLADIATOR: // Gladiator

        strcpy(you.class_name, "Gladiator");
        you.hp = 14;
        you.hp_max = 14;
        you.magic_points = 0;
        you.max_magic_points = 0;

        you.inv_quantity[0] = 1;
        you.inv_class[0] = OBJ_WEAPONS;
        you.inv_type[0] = WPN_SHORT_SWORD;
        choose_weapon();
        cprintf(EOL "A fine choice.");

        you.inv_plus[0] = 50;
        you.inv_plus2[0] = 50;
        you.inv_dam[0] = 0;
        you.inv_colour[0] = LIGHTCYAN;
        if (you.species >= SP_RED_DRACONIAN && you.species <= SP_UNK2_DRACONIAN)
        {
            you.inv_quantity[1] = 1;
            you.inv_class[1] = OBJ_ARMOUR;
            you.inv_type[1] = ARM_ANIMAL_SKIN;
            you.inv_plus[1] = 50;
            you.inv_dam[1] = 0;
            you.inv_colour[1] = BROWN;

            you.inv_quantity[2] = 1;
            you.inv_class[2] = OBJ_ARMOUR;
            you.inv_type[2] = ARM_SHIELD;
            you.inv_plus[2] = 50;
            you.inv_dam[2] = 0;
            you.inv_colour[2] = LIGHTCYAN;
        }
        else
        {
            you.inv_quantity[1] = 1;
            you.inv_class[1] = OBJ_ARMOUR;
            you.inv_type[1] = ARM_RING_MAIL;
            you.inv_plus[1] = 50;
            you.inv_dam[1] = 0;
            you.inv_colour[1] = LIGHTCYAN;

            you.inv_quantity[2] = 1;
            you.inv_class[2] = OBJ_ARMOUR;
            you.inv_type[2] = ARM_BUCKLER;
            you.inv_plus[2] = 50;
            you.inv_dam[2] = 0;
            you.inv_colour[2] = LIGHTCYAN;
        }

        you.strength += 6;
        you.dex += 4;
        you.equip[EQ_WEAPON] = 0;
        you.equip[EQ_BODY_ARMOUR] = 1;
        you.equip[EQ_SHIELD] = 2;
        you.gold = random2(10);

        you.skills[SK_FIGHTING] = 3;
        weap_skill = 3;

        if (you.species >= SP_RED_DRACONIAN && you.species <= SP_UNK2_DRACONIAN)
            you.skills[SK_DODGING] = 2;
        else
            you.skills[SK_ARMOUR] = 2;
/* you.evasion ++; */
        you.skills[SK_SHIELDS] = 1;
        you.skills[SK_UNARMED_COMBAT] = 2;
        break;


    case JOB_NECROMANCER:       // Necromancer

        strcpy(you.class_name, "Necromancer");
        you.hp = 10;
        you.hp_max = 10;
        you.magic_points = 3;
        you.max_magic_points = 3;
/*      you.f_abil = 5;
   you.mag_abil = 7;
   you.thr_abil = 5;
   you.speed = 10; */
        you.inv_quantity[0] = 1;
        you.inv_class[0] = OBJ_WEAPONS;
        you.inv_type[0] = WPN_DAGGER;
        you.inv_plus[0] = 50;
        you.inv_plus2[0] = 50;
        you.inv_dam[0] = 0;
        you.inv_colour[0] = LIGHTCYAN;
        you.inv_quantity[1] = 1;
        you.inv_class[1] = OBJ_ARMOUR;
        you.inv_type[1] = ARM_ROBE;
        you.inv_plus[1] = 50;
        you.inv_dam[1] = 0;
        you.inv_colour[1] = DARKGREY;
/*      you.AC = 1;
   you.evasion = 10; */
        you.dex += 4;
        you.intel += 6;
        you.equip[EQ_WEAPON] = 0;
        you.equip[EQ_BODY_ARMOUR] = 1;
        you.gold = random2(10);
        you.inv_class[2] = OBJ_BOOKS;

        you.inv_type[2] = BOOK_NECROMANCY;
        you.inv_quantity[2] = 1;
        you.inv_plus[2] = 127;
        you.inv_dam[2] = 0;     //1;

        you.inv_colour[2] = DARKGREY;
/* you.res_magic = 10; */

        you.skills[SK_DODGING] = 1;
        you.skills[SK_STEALTH] = 1;
        you.skills[SK_DODGING + random() % 2]++;
/* if (you.skills [SK_DODGING] == 2) you.evasion ++; */

        you.skills[SK_SPELLCASTING] = 1;
        you.skills[SK_NECROMANCY] = 4;
        you.skills[SK_SHORT_BLADES] = 1;
        you.skills[SK_STAVES] = 1;

        break;



    case JOB_PALADIN:           // paladin

        strcpy(you.class_name, "Paladin");
        you.religion = GOD_SHINING_ONE;
        you.piety = 28;
        you.hp = 14;
        you.hp_max = 14;
        you.magic_points = 0;
        you.max_magic_points = 0;

        you.inv_quantity[0] = 1;
        you.inv_class[0] = OBJ_WEAPONS;
        you.inv_type[0] = WPN_SHORT_SWORD;
        you.inv_plus[0] = 50;
        you.inv_plus2[0] = 50;
        you.inv_dam[0] = 0;
        you.inv_colour[0] = LIGHTCYAN;

        you.inv_quantity[1] = 1;
        you.inv_class[1] = OBJ_ARMOUR;
        you.inv_type[1] = ARM_ROBE;
        you.inv_plus[1] = 50;
        you.inv_dam[1] = 0;
        you.inv_colour[1] = WHITE;

        you.inv_quantity[2] = 1;
        you.inv_class[2] = OBJ_ARMOUR;
        you.inv_type[2] = ARM_SHIELD;
        you.inv_plus[2] = 50;
        you.inv_dam[2] = 0;
        you.inv_colour[2] = LIGHTCYAN;

        you.strength += 6;
        you.dex += 2;
        you.intel += 2;

        you.equip[EQ_WEAPON] = 0;
        you.equip[EQ_BODY_ARMOUR] = 1;
        you.gold = random2(10);
        you.equip[EQ_SHIELD] = 2;

        you.inv_class[3] = OBJ_POTIONS;
        you.inv_type[3] = POT_HEALING;
        you.inv_quantity[3] = 1;
        you.inv_plus[3] = 0;
        you.inv_dam[3] = 0;
        you.inv_colour[3] = random2(15) + 1;

        you.skills[SK_FIGHTING] = 2;
        you.skills[SK_ARMOUR] = 1;
        you.skills[SK_DODGING] = 1;
        you.skills[SK_ARMOUR + random() % 2]++;

        you.skills[SK_SHIELDS] = 2;
        you.skills[SK_SHORT_BLADES] = 2;
        you.skills[SK_LONG_SWORDS] = 2;
        you.skills[SK_INVOCATIONS] = 1;
        break;

    case JOB_ASSASSIN:          // assassin
        strcpy(you.class_name, "assassin");
        you.hp = 12;
        you.hp_max = 12;
        you.magic_points = 0;
        you.max_magic_points = 0;

        you.inv_quantity[0] = 1;
        you.inv_class[0] = OBJ_WEAPONS;
        you.inv_type[0] = WPN_DAGGER;
        to_hit_bonus = random2(3);
        you.inv_plus[0] = 51 + to_hit_bonus;
        you.inv_plus2[0] = 51 + (2 - to_hit_bonus);
        you.inv_dam[0] = 0;
        you.inv_colour[0] = LIGHTCYAN;

        you.inv_quantity[1] = 1;
        you.inv_class[1] = OBJ_WEAPONS;
        you.inv_type[1] = WPN_HAND_CROSSBOW;
        you.inv_plus[1] = 50;
        you.inv_plus2[1] = 50;
        you.inv_dam[1] = 0;
        you.inv_colour[1] = BROWN;

        you.inv_quantity[2] = 1;
        you.inv_class[2] = OBJ_ARMOUR;
        you.inv_type[2] = ARM_ROBE;
        you.inv_plus[2] = 50;
        you.inv_dam[2] = 0;
        you.inv_colour[2] = DARKGREY;

        you.inv_quantity[3] = 1;
        you.inv_class[3] = OBJ_ARMOUR;
        you.inv_type[3] = ARM_CLOAK;
        you.inv_plus[3] = 50;
        you.inv_dam[3] = 0;
        you.inv_colour[3] = DARKGREY;

        you.inv_quantity[4] = random2(10) + random2(10) + 10;
        you.inv_class[4] = OBJ_MISSILES;
        you.inv_type[4] = MI_DART;      //wtype;
        you.inv_plus[4] = 50;
        you.inv_dam[4] = 3;
        you.inv_colour[4] = LIGHTCYAN;

        you.strength += 2;
        you.dex += 6;
        you.intel += 2;

        you.equip[EQ_WEAPON] = 0;
        you.equip[EQ_BODY_ARMOUR] = 2;
        you.equip[EQ_CLOAK] = 3;
        you.gold = random2(10);

        you.skills[SK_FIGHTING] = 2;
        you.skills[SK_SHORT_BLADES] = 2;

        you.skills[SK_DODGING] = 1;
        you.skills[SK_STEALTH] = 3;
        you.skills[SK_STABBING] = 2;

        you.skills[SK_THROWING] = 1;
        you.skills[SK_DARTS] = 1;
        you.skills[SK_CROSSBOWS] = 1;
        break;

    case JOB_BERSERKER: // Barbarian

        strcpy(you.class_name, "Berserker");
        you.religion = GOD_TROG;
        you.piety = 35;
        you.hp = 15;
        you.hp_max = 15;
        you.magic_points = 0;
        you.max_magic_points = 0;

        if (you.species == SP_OGRE)
        {
            you.inv_quantity[0] = 1;
            you.inv_class[0] = OBJ_WEAPONS;
            you.inv_type[0] = WPN_CLUB;
            you.inv_plus[0] = 50;
            you.inv_plus2[0] = 50;
            you.inv_dam[0] = 0;
            you.inv_colour[0] = BROWN;

        }

        if (you.species == SP_TROLL)
        {
            you.inv_quantity[0] = 0;
            you.inv_class[0] = OBJ_WEAPONS;
            you.inv_type[0] = WPN_CLUB;
            you.inv_plus[0] = 50;
            you.inv_dam[0] = 0;
            you.inv_colour[0] = BROWN;

        }

        if (you.species == SP_OGRE || you.species == SP_TROLL
                            || (you.species >= SP_RED_DRACONIAN
                                        && you.species <= SP_UNK2_DRACONIAN))
        {
            you.inv_quantity[1] = 1;
            you.inv_class[1] = OBJ_ARMOUR;
            you.inv_type[1] = ARM_ANIMAL_SKIN;
            you.inv_plus[1] = 50;
            you.inv_dam[1] = 0;
            you.inv_colour[1] = BROWN;
        }
        else
        {
            you.inv_quantity[0] = 1;
            you.inv_class[0] = OBJ_WEAPONS;
            you.inv_type[0] = WPN_HAND_AXE;
            you.inv_plus[0] = 50;
            you.inv_plus2[0] = 50;
            you.inv_dam[0] = 0;
            you.inv_colour[0] = LIGHTCYAN;

            you.inv_quantity[1] = 1;
            you.inv_class[1] = OBJ_WEAPONS;
            you.inv_type[1] = WPN_SPEAR;
            you.inv_plus[1] = 50;
            you.inv_plus2[1] = 50;
            you.inv_dam[1] = 0;
            you.inv_colour[1] = LIGHTCYAN;

            you.inv_quantity[2] = 1;
            you.inv_class[2] = OBJ_WEAPONS;
            you.inv_type[2] = WPN_SPEAR;
            you.inv_plus[2] = 50;
            you.inv_plus2[2] = 50;
            you.inv_dam[2] = 0;
            you.inv_colour[2] = LIGHTCYAN;

            you.inv_quantity[3] = 1;
            you.inv_class[3] = OBJ_WEAPONS;
            you.inv_type[3] = WPN_SPEAR;
            you.inv_plus[3] = 50;
            you.inv_plus2[3] = 50;
            you.inv_dam[3] = 0;
            you.inv_colour[3] = LIGHTCYAN;


            you.inv_quantity[4] = 1;
            you.inv_class[4] = OBJ_ARMOUR;
            you.inv_type[4] = ARM_LEATHER_ARMOUR;
            you.inv_plus[4] = 50;
            you.inv_dam[4] = 0;
            you.inv_colour[4] = BROWN;
        }

        you.strength += 7;
        you.dex += 4;
        you.intel -= 1;
        you.equip[EQ_WEAPON] = 0;
        you.gold = random2(10);

        you.skills[SK_FIGHTING] = 2;

        if (you.species != SP_TROLL)
        {
            you.equip[EQ_WEAPON] = 0;
            you.equip[EQ_BODY_ARMOUR] = 4;
        }
        else
        {
            you.equip[EQ_WEAPON] = -1;
            you.equip[EQ_BODY_ARMOUR] = 1;
        }

        if (you.species == SP_OGRE)
        {
            you.equip[EQ_WEAPON] = 0;
            you.equip[EQ_BODY_ARMOUR] = 1;
        }


        if (you.species != SP_OGRE && you.species != SP_TROLL)
        {
            you.skills[SK_ARMOUR] = 2;
            you.skills[SK_DODGING] = 2;
            you.skills[SK_THROWING] = 2;
        }
        else
            you.skills[SK_FIGHTING] += 3;

        you.skills[SK_AXES] = 3;
        you.skills[SK_POLEARMS] = 1;
        break;

    case JOB_HUNTER:

        strcpy(you.class_name, "hunter");
        you.hp = 13;
        you.hp_max = 13;
        you.magic_points = 0;
        you.max_magic_points = 0;

        you.inv_quantity[0] = 1;
        you.inv_class[0] = OBJ_WEAPONS;
        you.inv_type[0] = WPN_DAGGER;
        you.inv_plus[0] = 50;
        you.inv_plus2[0] = 50;
        you.inv_dam[0] = 0;
        you.inv_colour[0] = LIGHTCYAN;

        you.inv_quantity[1] = 1;
        you.inv_class[1] = OBJ_WEAPONS;
        you.inv_type[1] = WPN_BOW;
        you.inv_plus[1] = 50;
        you.inv_plus2[1] = 50;
        you.inv_dam[1] = 0;
        you.inv_colour[1] = BROWN;

        you.inv_quantity[2] = 15 + random() % 5 + random() % 5 + random() % 5
                                                + random() % 5 + random() % 5;
        you.inv_class[2] = OBJ_MISSILES;
        you.inv_type[2] = MI_ARROW;
        you.inv_plus[2] = 50;
        you.inv_plus2[2] = 50;
        you.inv_dam[2] = 0;
        you.inv_colour[2] = LIGHTCYAN;

        you.inv_quantity[3] = 1;
        you.inv_class[3] = OBJ_ARMOUR;
        you.inv_type[3] = ARM_LEATHER_ARMOUR;
        you.inv_plus[3] = 50;
        you.inv_dam[3] = 0;
        you.inv_colour[3] = BROWN;

        if (you.species >= SP_RED_DRACONIAN
                                    && you.species <= SP_UNK2_DRACONIAN)
        {
            you.inv_type[3] = ARM_ROBE;
            you.inv_colour[3] = GREEN;
        }

        you.strength += 3;
        you.dex += 4;
        you.intel += 3;
        you.equip[EQ_WEAPON] = 0;
        you.equip[EQ_BODY_ARMOUR] = 3;
        you.gold = random2(10);

        you.skills[SK_FIGHTING] = 2;
        you.skills[SK_THROWING] = 2;
        you.skills[SK_SPELLCASTING] = 1;

        switch (you.species)
        {
            case SP_HALFLING:
            case SP_GNOME:
                you.inv_type[1] = WPN_SLING;
                you.inv_type[2] = MI_STONE;
                you.inv_colour[2] = BROWN;
                you.inv_quantity[2] += 10 + random2(10);

                you.skills[SK_DODGING] = 2;
                you.skills[SK_STEALTH] = 2;
                you.skills[SK_SLINGS] = 2;
                break;

            case SP_HILL_DWARF:
            case SP_MOUNTAIN_DWARF:
            case SP_HILL_ORC:
                you.inv_type[1] = WPN_CROSSBOW;
                you.inv_type[2] = MI_BOLT;

                if (you.species == SP_HILL_ORC)
                {
                    you.inv_type[0] = WPN_SHORT_SWORD;
                    you.skills[SK_SHORT_BLADES] = 1;
                }
                else
                {
                    you.inv_type[0] = WPN_HAND_AXE;
                    you.skills[SK_AXES] = 1;
                }

                you.skills[SK_DODGING] = 1;
                you.skills[SK_SHIELDS] = 1;
                you.skills[SK_CROSSBOWS] = 2;
                break;

            default:
                you.skills[SK_DODGING] = 1;
                you.skills[SK_STEALTH] = 1;
                you.skills[SK_STABBING + random() % 2]++; // stabbing or shield
                you.skills[SK_BOWS] = 2;
                break;
        }


        break;

    case JOB_CONJURER:
    case JOB_ENCHANTER:
    case JOB_SUMMONER:
    case JOB_FIRE_ELEMENTALIST:
    case JOB_ICE_ELEMENTALIST:
    case JOB_AIR_ELEMENTALIST:
    case JOB_EARTH_ELEMENTALIST:
    case JOB_VENOM_MAGE:
    case JOB_TRANSMUTER:
    case JOB_WARPER:
        if (you.char_class == JOB_CONJURER)
            strcpy(you.class_name, "conjurer");
        if (you.char_class == JOB_ENCHANTER)
            strcpy(you.class_name, "Enchanter");
        if (you.char_class == JOB_FIRE_ELEMENTALIST)
            strcpy(you.class_name, "Fire Elementalist");
        if (you.char_class == JOB_ICE_ELEMENTALIST)
            strcpy(you.class_name, "Ice Elementalist");
        if (you.char_class == JOB_SUMMONER)
            strcpy(you.class_name, "Summoner");
        if (you.char_class == JOB_AIR_ELEMENTALIST)
            strcpy(you.class_name, "Air Elementalist");
        if (you.char_class == JOB_EARTH_ELEMENTALIST)
            strcpy(you.class_name, "Earth Elementalist");
        if (you.char_class == JOB_VENOM_MAGE)
            strcpy(you.class_name, "Venom Mage");
        if (you.char_class == JOB_TRANSMUTER)
            strcpy(you.class_name, "transmuter");
        if (you.char_class == JOB_WARPER)
            strcpy(you.class_name, "warper");

        switch (random() % 8)   /* get a random lvl 1 attack spell - later overwritten for most classes */
        {
        case 0:
            you.spells[0] = SPELL_BURN;
            break;
        case 1:
            you.spells[0] = SPELL_FREEZE;
            break;
        case 2:
            you.spells[0] = SPELL_ARC;
            break;
        case 3:
            you.spells[0] = SPELL_CRUSH;
            break;
        case 4:
            you.spells[0] = SPELL_STING;
            break;
        case 5:
        case 6:
        case 7:
            you.spells[0] = SPELL_MAGIC_DART;
            break;
        }

        you.hp = 10;
        you.hp_max = 10;
        you.magic_points = 3;
        you.max_magic_points = 3;

        you.inv_quantity[0] = 1;
        you.inv_class[0] = OBJ_WEAPONS;

        if (you.species == SP_OGRE_MAGE)
            you.inv_type[0] = WPN_SHORT_SWORD;
        else
            you.inv_type[0] = WPN_DAGGER;

        you.inv_plus[0] = 50;
        you.inv_plus2[0] = 50;
        if (you.char_class == JOB_ENCHANTER)
            you.inv_plus[0] = 51;
        if (you.char_class == JOB_ENCHANTER)
            you.inv_plus2[0] = 51;
        you.inv_dam[0] = 0;
        you.inv_colour[0] = LIGHTCYAN;

        you.inv_quantity[1] = 1;
        you.inv_class[1] = OBJ_ARMOUR;
        you.inv_type[1] = ARM_ROBE;
        you.inv_plus[1] = 50;
        if (you.char_class == JOB_ENCHANTER)
            you.inv_plus[1] = 51;
        you.inv_dam[1] = 0;
        you.inv_colour[1] = random() % 15 + 1;
        if (you.char_class == JOB_FIRE_ELEMENTALIST)
            you.inv_colour[1] = RED;
        if (you.char_class == JOB_ICE_ELEMENTALIST)
            you.inv_colour[1] = LIGHTCYAN;
        if (you.char_class == JOB_AIR_ELEMENTALIST)
            you.inv_colour[1] = LIGHTBLUE;
        if (you.char_class == JOB_EARTH_ELEMENTALIST)
            you.inv_colour[1] = BROWN;
        if (you.char_class == JOB_VENOM_MAGE)
            you.inv_colour[1] = GREEN;
/*      you.AC = 1;
   if (you.char_class == JOB_ENCHANTER) you.AC = 2;
   you.evasion = 10; */
//      you.strength = 6;
        you.dex += 4;
        you.intel += 6;
        you.equip[EQ_WEAPON] = 0;
        you.equip[EQ_BODY_ARMOUR] = 1;
        you.gold = random2(10);
        you.inv_class[2] = OBJ_BOOKS;

/*        you.res_magic = 10; */

        you.inv_type[2] = BOOK_CONJURATIONS_I + random2(2);     // conj

        you.inv_plus[2] = 127;
        if (you.char_class == JOB_SUMMONER)
        {
            you.inv_type[2] = BOOK_SUMMONINGS;  // summ

            you.inv_plus[2] = 127;
            you.skills[SK_SUMMONINGS] = 4;
            // gets some darts - this class is very difficult to start off with
            you.inv_quantity[3] = random2(5) + random2(5) + 7;
            you.inv_class[3] = OBJ_MISSILES;
            you.inv_type[3] = MI_DART;  //wtype;

            you.inv_plus[3] = 50;
            you.inv_dam[3] = 0;
            you.inv_colour[3] = LIGHTCYAN;
        }
        if (you.char_class == JOB_CONJURER)
        {
            you.skills[SK_CONJURATIONS] = 4;    // conjurer

        }
        if (you.char_class == JOB_ENCHANTER)    // ench

        {
/*          you.res_magic = 12; */

            you.inv_type[2] = BOOK_CHARMS;      // charms

            you.inv_plus[2] = 127;
            you.skills[SK_ENCHANTMENTS] = 4;

            you.inv_quantity[3] = 1;
            you.inv_class[3] = OBJ_WANDS;
            you.inv_dam[3] = 0;
            you.inv_colour[3] = random() % 15 + 1;
            switch (random() % 4)
            {
            case 0:
                you.inv_type[3] = WAND_SLOWING;
                you.inv_plus[3] = 7 + random() % 5;
                break;
            case 1:
                you.inv_type[3] = WAND_PARALYSIS;
                you.inv_plus[3] = 5 + random() % 4;
                break;
            case 2:
                you.inv_type[3] = WAND_INVISIBILITY;
                you.inv_plus[3] = 4 + random() % 4;
                break;
            case 3:
                you.inv_type[3] = WAND_PARALYSIS;
                you.inv_plus[3] = 5 + random() % 4;
                break;
            }
            you.inv_plus2[3] = 0;
        }                       // end of enchanter

        switch (you.char_class)
        {
        case JOB_FIRE_ELEMENTALIST:     // fire elementalist

            you.inv_type[2] = BOOK_FLAMES;
            you.inv_plus[2] = 127;
            you.skills[SK_CONJURATIONS] = 1;    // conj
            //         you.skills [SK_ENCHANTMENTS] = 1; // ench

            you.skills[SK_FIRE_MAGIC] = 3;      // fire magic

            you.spells[0] = SPELL_BURN;
            break;

        case JOB_ICE_ELEMENTALIST:      // Ice elementalist

            you.inv_type[2] = BOOK_FROST;
            you.inv_plus[2] = 127;
            you.skills[SK_CONJURATIONS] = 1;    // conj
            //         you.skills [SK_ENCHANTMENTS] = 1; // ench

            you.skills[SK_ICE_MAGIC] = 3;       // ice magic

            you.spells[0] = SPELL_FREEZE;
            break;

        case JOB_AIR_ELEMENTALIST:      // Air elementalist

            you.inv_type[2] = BOOK_AIR;
            you.inv_plus[2] = 127;
            you.skills[SK_CONJURATIONS] = 1;    // conj
            //         you.skills [SK_ENCHANTMENTS] = 1; // ench

            you.skills[SK_AIR_MAGIC] = 3;       // air magic

            you.spells[0] = SPELL_ARC;
            break;

        case JOB_EARTH_ELEMENTALIST:    // Earth elementalist

            you.inv_type[2] = BOOK_GEOMANCY;
            you.inv_plus[2] = 127;
            you.skills[SK_CONJURATIONS] = 1;    // conj
            //         you.skills [SK_ENCHANTMENTS] = 1; // ench

            you.skills[SK_EARTH_MAGIC] = 3;     // earth magic

            you.spells[0] = SPELL_CRUSH;
            break;

        case JOB_VENOM_MAGE:    // Venom Mage

            you.inv_type[2] = BOOK_POISONINGS;
            you.inv_plus[2] = 126;
            you.skills[SK_POISON_MAGIC] = 4;    // Poison magic

            you.spells[0] = SPELL_STING;
            break;

        case JOB_TRANSMUTER:    // transmuter

            you.inv_type[2] = BOOK_CHANGES;
            you.inv_plus[2] = 126;
            you.skills[SK_TRANSMIGRATION] = 4;  // transmigrations

            you.spells[0] = SPELL_DISRUPT;
            break;

        case JOB_WARPER:        // warper

            you.inv_type[2] = BOOK_SPATIAL_TRANSLOCATIONS;
            you.inv_plus[2] = 124;
            you.skills[SK_TRANSLOCATIONS] = 4;  // translocations

            break;

        }

        you.inv_quantity[2] = 1;
//      you.inv_plus [2] = 124;
        you.inv_dam[2] = 0;
        you.inv_colour[2] = random() % 15 + 1;
        if (you.char_class == JOB_FIRE_ELEMENTALIST)
            you.inv_colour[2] = RED;
        if (you.char_class == JOB_ICE_ELEMENTALIST)
            you.inv_colour[2] = LIGHTCYAN;
        if (you.char_class == JOB_VENOM_MAGE)
            you.inv_colour[2] = GREEN;
/*  you.res_magic = 10; */

        you.skills[SK_DODGING] = 1;
        you.skills[SK_STEALTH] = 1;
        you.skills[SK_DODGING + random() % 2]++;
/* if (you.skills [SK_DODGING] == 2) you.evasion ++; */

        you.skills[SK_SPELLCASTING] = 1;
        you.skills[SK_SHORT_BLADES] = 1;
/*      if (you.species == SP_OGRE_MAGE)
   {
   you.skills [SK_SHORT_BLADES] = 0;
   you.skills [SK_POLEARMS] = 2;
   } */
        you.skills[SK_STAVES] = 1;


        break;


    case JOB_CRUSADER:          // Crusader

        strcpy(you.class_name, "Crusader");
/*        you.piety = 75; */
        you.hp = 13;
        you.hp_max = 13;
        you.magic_points = 1;
        you.max_magic_points = 1;
/*      you.f_abil = 9;
   you.mag_abil = 6;
   you.thr_abil = 4;
   you.speed = 10; */
        you.inv_quantity[0] = 1;
        you.inv_class[0] = OBJ_WEAPONS;
        you.inv_type[0] = WPN_SHORT_SWORD;

//      if (you.species == 17) you.inv_type [0] = 17;

        you.inv_plus[0] = 50;
        you.inv_plus2[0] = 50;
        you.inv_dam[0] = 0;
        you.inv_colour[0] = LIGHTCYAN;
        choose_weapon();
        cprintf(EOL "A fine choice.");
        weap_skill = 2;
        you.inv_quantity[1] = 1;
        you.inv_class[1] = OBJ_ARMOUR;
        you.inv_type[1] = ARM_ROBE;
        you.inv_plus[1] = 50;
        you.inv_dam[1] = 0;
        you.inv_colour[1] = 1 + random() % 15;


        you.inv_class[2] = OBJ_BOOKS;
        you.inv_type[2] = BOOK_WAR_CHANTS;
        you.inv_quantity[2] = 1;
        you.inv_plus[2] = 127;
        you.inv_dam[2] = 0;
        you.inv_colour[2] = 1 + random() % 15;

/*      you.AC = 1;
   you.evasion = 10; */
        you.strength += 4;
        you.dex += 3;
        you.intel += 3;
        you.equip[EQ_WEAPON] = 0;
        you.equip[EQ_BODY_ARMOUR] = 1;
        you.gold = random2(10);

        you.skills[SK_FIGHTING] = 3;
        you.skills[SK_ARMOUR] = 1;
        you.skills[SK_DODGING] = 1;
        you.skills[SK_STEALTH] = 1;
        you.skills[SK_SPELLCASTING] = 2;
        you.skills[SK_ENCHANTMENTS] = 2;
        break;


    case JOB_DEATH_KNIGHT:      // Death knight

        strcpy(you.class_name, "Death Knight");
/*        you.piety = 75; */
        you.hp = 13;
        you.hp_max = 13;
        you.magic_points = 1;
        you.max_magic_points = 1;
        you.speed = 10;
        you.inv_quantity[0] = 1;
        you.inv_class[0] = OBJ_WEAPONS;
        you.inv_type[0] = WPN_SHORT_SWORD;
        you.inv_plus[0] = 50;
        you.inv_plus2[0] = 50;
        you.inv_dam[0] = 0;
        you.inv_colour[0] = LIGHTCYAN;
        choose_weapon();
        cprintf(EOL "A fine choice.");
        weap_skill = 2;

        you.inv_quantity[1] = 1;
        you.inv_class[1] = OBJ_ARMOUR;
        you.inv_type[1] = ARM_ROBE;
        you.inv_plus[1] = 50;
        you.inv_dam[1] = 0;
        you.inv_colour[1] = DARKGREY;


        you.inv_class[2] = OBJ_BOOKS;
        you.inv_type[2] = BOOK_NECROMANCY;
        you.inv_quantity[2] = 1;
        you.inv_plus[2] = 127;
        you.inv_dam[2] = 0;
        you.inv_colour[2] = DARKGREY;

/*      you.AC = 1;
   you.evasion = 10; */
        you.strength += 4;
        you.dex += 3;
        you.intel += 3;
        you.equip[EQ_WEAPON] = 0;
        you.equip[EQ_BODY_ARMOUR] = 1;
        you.gold = random2(10);


        if (you.species == SP_DEMIGOD)
        {
            you.skills[SK_SPELLCASTING] = 1;
            you.skills[SK_NECROMANCY] = 2;
            you.spells[0] = SPELL_PAIN;
        }
        else
        {
            clrscr();
            cprintf(EOL " From where do you draw your power?" EOL);
            cprintf("a - Necromantic magic" EOL);
            cprintf("b - the God Yredelemnul" EOL);

          getkey1:keyn = get_ch();
            switch (keyn)
            {
            case 'a':
                cprintf(EOL "Very well.");
                you.skills[SK_SPELLCASTING] = 1;
                you.skills[SK_NECROMANCY] = 2;
                you.spells[0] = SPELL_PAIN;
                break;
            case 'b':
                you.religion = GOD_YREDELEMNUL;
                you.inv_plus[0] = 51;
                you.inv_plus2[0] = 51;
                you.inv_quantity[2] = 0;
                you.skills[SK_INVOCATIONS] = 3;
                you.piety = 28;
                cprintf(EOL "Welcome...");
                break;
            default:
                goto getkey1;
            }

        }

        you.skills[SK_FIGHTING] = 2;
        you.skills[SK_ARMOUR] = 1;
        you.skills[SK_DODGING] = 1;
/*  if (you.skills [SK_DODGING] == 2) you.evasion ++; */

        you.skills[SK_STEALTH] = 1;
// you.skills [SK_SHORT_BLADES] = 2;
        you.skills[SK_STABBING] = 1;


        break;

    case JOB_CHAOS_KNIGHT:      // Chaos knight

        strcpy(you.class_name, "Knight of Chaos");
/*        you.piety = 75; */
        you.hp = 13;
        you.hp_max = 13;
        you.magic_points = 1;
        you.max_magic_points = 1;
        you.speed = 10;
        you.inv_quantity[0] = 1;
        you.inv_class[0] = OBJ_WEAPONS;
        you.inv_type[0] = WPN_SHORT_SWORD;
        you.inv_plus[0] = 50 + random2(3);
        you.inv_plus2[0] = 50 + random2(3);
        you.inv_dam[0] = 0;
        if (random2(5) == 0)
            you.inv_dam[0] = 30;
        if (random2(5) == 0)
            you.inv_dam[0] = 60;
        you.inv_colour[0] = LIGHTCYAN;
        choose_weapon();
        weap_skill = 2;
        you.inv_quantity[1] = 1;
        you.inv_class[1] = OBJ_ARMOUR;
        you.inv_type[1] = ARM_ROBE;
        you.inv_plus[1] = 50 + random2(3);
        you.inv_dam[1] = 0;
        you.inv_colour[1] = random2(15) + 1;

        you.strength += 4;
        you.dex += 3;
        you.intel += 3;
        you.equip[EQ_WEAPON] = 0;
        you.equip[EQ_BODY_ARMOUR] = 1;
        you.gold = random2(10);

        you.skills[SK_FIGHTING] = 3;
        you.skills[SK_ARMOUR] = 1;
        you.skills[SK_DODGING] = 1;
        you.skills[SK_ARMOUR + random() % 2]++;

        you.skills[SK_STABBING] = 1;

        clrscr();
        cprintf(EOL " Which God of Chaos do you wish to serve?" EOL);
        cprintf("a - Xom of Chaos" EOL);
        cprintf("b - Makhleb the Destroyer" EOL);

      getkey2:keyn = get_ch();
        switch (keyn)
        {
        case 'a':
            you.religion = GOD_XOM;
            you.skills[SK_FIGHTING]++;
            cprintf(EOL "A new plaything! Welcome...");
            break;
        case 'b':
            you.religion = GOD_MAKHLEB;
            you.skills[SK_INVOCATIONS] = 2;
            cprintf(EOL "Blood and souls for Makhleb!");
            break;
        default:
            goto getkey2;
        }

        you.piety = 25;         /* Irrelevant for Xom, of course */

        break;



    case JOB_HEALER:            // Healer

        strcpy(you.class_name, "Healer");
        you.piety = 45;
        you.hp = 15;
        you.hp_max = 15;
        you.magic_points = 1;
        you.max_magic_points = 1;

        you.inv_quantity[0] = 1;
        you.inv_class[0] = OBJ_WEAPONS;
        you.inv_type[0] = WPN_QUARTERSTAFF;

        you.inv_plus[0] = 50;
        you.inv_plus2[0] = 50;
        you.inv_dam[0] = 0;
        you.inv_colour[0] = BROWN;


        // Robe
        you.inv_quantity[1] = 1;
        you.inv_class[1] = OBJ_ARMOUR;
        you.inv_type[1] = ARM_ROBE;
        you.inv_plus[1] = 50;
        you.inv_dam[1] = 0;
        you.inv_colour[1] = WHITE;

        you.inv_class[2] = OBJ_POTIONS;
        you.inv_type[2] = POT_HEALING;
        you.inv_quantity[2] = 2 + random2(2);
        you.inv_plus[2] = 0;
        you.inv_dam[2] = 0;
        you.inv_colour[2] = random2(15) + 1;    // hmmm...

        you.inv_class[3] = OBJ_POTIONS;
        you.inv_type[3] = POT_HEAL_WOUNDS;
        you.inv_quantity[3] = 2 + random2(2);
        you.inv_plus[3] = 0;
        you.inv_dam[3] = 0;
        you.inv_colour[3] = random2(15) + 1;    // hmmm...

        you.strength += 4;
        you.dex += 2;
        you.intel += 4;

        you.equip[EQ_WEAPON] = 0;
        you.equip[EQ_BODY_ARMOUR] = 1;

        you.gold = random2(100);

        you.skills[SK_FIGHTING] = 2;
        you.skills[SK_DODGING] = 1;
        you.skills[SK_SHIELDS] = 1;
        you.skills[SK_THROWING] = 2;
        you.skills[SK_STAVES] = 3;

        you.skills[SK_INVOCATIONS] = 2;

        you.religion = GOD_ELYVILON;

        break;


    case JOB_REAVER:            // Reaver

        strcpy(you.class_name, "Reaver");
        you.hp = 13;
        you.hp_max = 13;
        you.magic_points = 1;
        you.max_magic_points = 1;
        you.speed = 10;
        you.inv_quantity[0] = 1;
        you.inv_class[0] = OBJ_WEAPONS;
        you.inv_type[0] = WPN_SHORT_SWORD;
        you.inv_plus[0] = 50;
        you.inv_plus2[0] = 50;
        you.inv_dam[0] = 0;
        you.inv_colour[0] = LIGHTCYAN;
        choose_weapon();
        cprintf(EOL "A fine choice.");
        weap_skill = 3;

        you.inv_quantity[1] = 1;
        you.inv_class[1] = OBJ_ARMOUR;
        you.inv_type[1] = ARM_ROBE;
        you.inv_plus[1] = 50;
        you.inv_dam[1] = 0;
        you.inv_colour[1] = RED;


        you.inv_class[2] = OBJ_BOOKS;
        you.inv_type[2] = BOOK_CONJURATIONS_I + random2(2);
        you.inv_quantity[2] = 1;
        you.inv_plus[2] = 127;
        you.inv_dam[2] = 0;
        you.inv_colour[2] = RED;

        you.strength += 4;
        you.dex += 2;
        you.intel += 4;
        you.equip[EQ_WEAPON] = 0;
        you.equip[EQ_BODY_ARMOUR] = 1;
        you.gold = random2(10);

        you.skills[SK_FIGHTING] = 2;
        you.skills[SK_ARMOUR] = 1;
        you.skills[SK_DODGING] = 1;

        you.skills[SK_SPELLCASTING] = 1;
        you.skills[SK_CONJURATIONS] = 2;
        you.spells[0] = SPELL_MAGIC_DART;
        break;

    case JOB_STALKER:           // stalker

        strcpy(you.class_name, "stalker");
        you.hp = 11;
        you.hp_max = 11;
        you.magic_points = 1;
        you.max_magic_points = 1;
        you.inv_quantity[0] = 1;
        you.inv_class[0] = OBJ_WEAPONS;
        you.inv_type[0] = WPN_DAGGER;
        to_hit_bonus = random2(3);
        you.inv_plus[0] = 51 + to_hit_bonus;
        you.inv_plus2[0] = 51 + (2 - to_hit_bonus);
        you.inv_dam[0] = 0;
        you.inv_colour[0] = LIGHTCYAN;
        you.inv_quantity[1] = 1;
        you.inv_class[1] = OBJ_ARMOUR;
        you.inv_type[1] = ARM_ROBE;
        you.inv_plus[1] = 50;
        you.inv_dam[1] = 0;
        you.inv_colour[1] = GREEN;
        you.inv_quantity[2] = 1;
        you.inv_class[2] = OBJ_ARMOUR;
        you.inv_type[2] = ARM_CLOAK;
        you.inv_plus[2] = 50;
        you.inv_dam[2] = 0;
        you.inv_colour[2] = DARKGREY;
        you.inv_class[3] = OBJ_BOOKS;
        you.inv_type[3] = ARM_BUCKLER;
        you.inv_quantity[3] = 1;
        you.inv_plus[3] = 126;
        you.inv_dam[3] = 0;
        you.inv_colour[3] = GREEN;


        you.strength += 2;
        you.dex += 5;
        you.intel += 3;
        you.equip[EQ_WEAPON] = 0;
        you.equip[EQ_BODY_ARMOUR] = 1;
        you.equip[EQ_CLOAK] = 2;
        you.gold = random2(10);

        you.spells[0] = SPELL_STING;


        you.skills[SK_FIGHTING] = 1;
        you.skills[SK_SHORT_BLADES] = 1;

        you.skills[SK_POISON_MAGIC] = 1;

        you.skills[SK_DODGING] = 1;
        you.skills[SK_STEALTH] = 2;
        you.skills[SK_STABBING] = 2;
        you.skills[SK_DODGING + random() % 3]++;

/* if (you.skills [SK_DODGING] == 2) you.evasion ++; */

        you.skills[SK_THROWING] = 1;
        you.skills[SK_DARTS] = 1;

        break;

    case JOB_MONK:              // Monk

        strcpy(you.class_name, "Monk");
        you.hp = 13;
        you.hp_max = 13;
        you.magic_points = 0;
        you.max_magic_points = 0;
        you.inv_class[0] = OBJ_ARMOUR;
        you.inv_type[0] = ARM_ROBE;
        you.inv_plus[0] = 50;
        you.inv_dam[0] = 0;
        you.inv_quantity[0] = 1;
        you.inv_colour[0] = BROWN;

        you.strength += 3;
        you.dex += 5;
        you.intel += 2;
        you.equip[EQ_WEAPON] = -1;
        you.equip[EQ_BODY_ARMOUR] = 0;
        you.gold = 0;

        you.skills[SK_FIGHTING] = 3;
        you.skills[SK_UNARMED_COMBAT] = 4;

        you.skills[SK_DODGING] = 3;
        you.skills[SK_STEALTH] = 2;

        break;


/*
   Spellbook binary thing:
   6 spells: 127
   5 spells: 126
   4 spells: 124
   3 spells: 120
   2 spells: 112
   1 spells: 96
   0 spells: 64
   (assuming all from start, no empty spaces)
 */

    }

/*you.mag_abil = 0; */


/*you.res_magic = 3 + you.skills [SK_ENCHANTMENTS] * 2; */

    char points_left = 8;

    if (you.species == SP_DEMIGOD || you.species == SP_DEMONSPAWN)
        points_left += 7;       /* demigod */

//for (i = 0; i < 8; i ++)
    do
    {
        switch (random2(3))
        {
        case 0:
            if (you.strength > 17 && random() % 2 != 0)
                continue;
            you.strength++;
            break;
        case 1:
            if (you.dex > 17 && random() % 2 != 0)
                continue;
            you.dex++;
            break;
        case 2:
            if (you.intel > 17 && random() % 2 != 0)
                continue;
            you.intel++;
            break;
        }
        points_left--;
    }
    while (points_left > 0);

    // end of for i


    you.base_hp = 5000;
    you.base_hp2 = 5000 + you.hp_max;

    you.base_magic_points = 5000;
    you.base_magic_points2 = 5000 + you.max_magic_points;

    you.is_undead = 0;

    switch (you.species)
    {
    case SP_ELF:                // elf
        you.hp_max--;
        you.base_hp2--;
        break;

    case SP_HIGH_ELF:           // high elf
        you.hp_max--;
        you.base_hp2--;
        break;

    case SP_GREY_ELF:           // grey elf
        you.hp_max--;
        you.base_hp2--;
        you.max_magic_points++;
        you.base_magic_points2++;
        break;

    case SP_DEEP_ELF:           // deep elf
        you.hp_max -= 2;
        you.base_hp2 -= 2;
        you.max_magic_points++;
        you.base_magic_points2++;
        break;

    case SP_HILL_DWARF: // hill dwarf
        you.hp_max++;
        you.base_hp2++;
        break;

    case SP_MOUNTAIN_DWARF:     // mountain dwarf
        you.hp_max++;
        you.base_hp2++;
        break;

    case SP_HALFLING:           // halfling
        you.hp_max -= 2;
        you.base_hp2 -= 2;
        // you.hunger_inc -= 1;
    /*you.evasion ++; */
        break;

    case SP_HILL_ORC:           // hill orc
        you.hp_max++;
        you.base_hp2++;
        break;

    case SP_KOBOLD:             // kobold
        you.hp_max -= 2;
        you.base_hp2 -= 2;
    /*you.evasion ++; */
        break;

    case SP_MUMMY:              // mummy
        you.hp_max++;
        you.base_hp2++;
     /*you.res_cold ++;
       you.res_fire --;
       you.prot_life ++;
       you.res_poison ++; */
        you.is_undead = 2;
     /*you.sust_abil = 1;
       you.evasion --; */
        break;

    case SP_NAGA:               // Naga
        you.hp_max += 2;
        you.base_hp2 += 2;
     /*you.evasion -= 3;
       you.AC += 2;
       you.res_poison ++;
       you.see_invis ++; */
        you.attribute[ATTR_WALK_SLOWLY] = 1;
        break;

    case SP_GNOME:              // Gnome
        /*you.AC ++; */
        you.hp_max -= 2;
        you.base_hp2 -= 2;
        break;

    case SP_OGRE:               // Ogre
        you.hp_max += 3;
        you.base_hp2 += 3;
        // you.hunger_inc += 1;
        break;

    case SP_TROLL:              // Troll
        you.hp_max += 3;
        you.base_hp2 += 3;
        // you.hunger_inc += 6;
        break;

    case SP_OGRE_MAGE:          // Ogre-Mage
        you.hp_max += 2;
        you.base_hp2 += 2;
        // you.hunger_inc += 1;
        break;

    case SP_RED_DRACONIAN:      // Draconian
    case SP_WHITE_DRACONIAN:    // Draconian
    case SP_GREEN_DRACONIAN:    // Draconian
    case SP_GOLDEN_DRACONIAN:   // Draconian
    case SP_GREY_DRACONIAN:     // Draconian
    case SP_BLACK_DRACONIAN:    // Draconian
    case SP_PURPLE_DRACONIAN:   // Draconian
    case SP_MOTTLED_DRACONIAN:  // Draconian
    case SP_PALE_DRACONIAN:     // Draconian
    case SP_UNK0_DRACONIAN:     // Draconian
    case SP_UNK1_DRACONIAN:     // Draconian
    case SP_UNK2_DRACONIAN:     // Draconian
        you.hp_max += 1;
        you.base_hp2 += 1;
        break;

    case SP_CENTAUR:            // Centaur
        you.hp_max += 3;
        you.base_hp2 += 3;
        // you.hunger_inc += 2;
        break;

    case SP_DEMIGOD:            // Demigod
        you.hp_max += 3;
        you.base_hp2 += 3;
        // you.hunger_inc += 1;
        you.max_magic_points++;
        you.base_magic_points2++;
        break;

    case SP_SPRIGGAN:           // spriggan
        you.hp_max -= 2;
        you.base_hp2 -= 2;
        // you.hunger_inc -= 1;
        break;

    case SP_MINOTAUR:           // Minotaur
        you.hp_max += 2;
        you.base_hp2 += 2;
        break;

    case SP_GHOUL:              // Ghoul
        you.hp_max += 2;
        you.base_hp2 += 2;
        you.is_undead = 1;
        break;

    case SP_KENKU:              // Kenku
        you.hp_max -= 2;
        you.base_hp2 -= 2;
        break;
    }

    you.hp = you.hp_max;
    you.magic_points = you.max_magic_points;

    you.max_dex = you.dex;
    you.max_strength = you.strength;
    you.max_intel = you.intel;

    if (weap_skill != 0)
        you.skills[weapon_skill(0, you.inv_type[0])] = weap_skill;

    if (you.is_undead == 0)     // != 2)

    {
        for (i = 0; i < 52; i++)
        {
            if (you.inv_quantity[i] == 0)
            {
                you.inv_quantity[i] = 1;
                you.inv_class[i] = OBJ_FOOD;
                you.inv_type[i] = FOOD_BREAD_RATION;
                if (you.species == SP_HILL_ORC || you.species == SP_KOBOLD || you.species == SP_OGRE || you.species == SP_TROLL)
                    you.inv_type[i] = FOOD_MEAT_RATION;
                you.inv_colour[i] = BROWN;
                you.num_inv_items++;
                break;
            }
        }
    }

    for (i = 0; i < 52; i++)
    {
        if (you.inv_quantity[i] != 0)
        {
            if (you.inv_class[i] == OBJ_BOOKS)
            {
                you.had_item[you.inv_type[i]] = 1;
                if (you.inv_type[i] == BOOK_MINOR_MAGIC_I || you.inv_type[i] == BOOK_MINOR_MAGIC_II || you.inv_type[i] == BOOK_MINOR_MAGIC_III)
                {
                    you.had_item[BOOK_MINOR_MAGIC_I] = 1;
                    you.had_item[BOOK_MINOR_MAGIC_II] = 1;
                    you.had_item[BOOK_MINOR_MAGIC_III] = 1;
                }
                if (you.inv_type[i] == BOOK_CONJURATIONS_I || you.inv_type[i] == BOOK_CONJURATIONS_II)
                {
                    you.had_item[BOOK_CONJURATIONS_I] = 1;
                    you.had_item[BOOK_CONJURATIONS_II] = 1;
                }
            }

            if (you.inv_class[i] <= OBJ_ARMOUR)         // || you.inv_class [i] == 2)

                switch (you.species)
                {
                case SP_ELF:
                case SP_HIGH_ELF:
                case SP_GREY_ELF:
                case SP_DEEP_ELF:
                case SP_SLUDGE_ELF:
                    you.inv_dam[i] += 120;
                    break;

                case SP_HILL_DWARF:
                case SP_MOUNTAIN_DWARF:
                    you.inv_dam[i] += 150;
                    you.inv_colour[i] = CYAN;
                    break;

                case SP_HILL_ORC:
                    if (you.inv_class[i] == OBJ_WEAPONS
                                        || you.inv_class[i] == OBJ_MISSILES)
                        you.inv_dam[i] += 90;
                    else
                        you.inv_dam[i] += 180;
                    break;
                }
        }
    }


// must remember to check for already existing colours/combinations

    for (i = 0; i < 4; i++)
    {
        for (j = 0; j < 50; j++)
        {
            you.item_description[i][j] = 215;
        }
    }

    for (i = 0; i < 30; i++)
    {
        you.duration[i] = 0;
    }

    you.item_description[1][13] = 178;  // oatmeal

    you.item_description[1][16] = 0;    // water

    for (i = 0; i < 4; i++)
    {
        for (j = 0; j < 50; j++)
        {

            do
            {
                passout = 1;

                switch (i)
                {
                    // This bit is horrendous:

                case 0: // wands

                    if (random2(2) != 0)
                    {
                        you.item_description[i][j] = random2(12);
                    }
                    else
                    {
//                  you.item_description [i] [j] = (random2(12) * 16) + random2(15);
                        you.item_description[i][j] = (random2(12)) + random2(12) * 16;
                    }
                    break;

                case 1: // potions

                    if (random2(2) != 0)
                    {
                        you.item_description[i][j] = random2(14);
                    }
                    else
                    {
                        you.item_description[i][j] = (random2(14) * 14) + random2(14);
                    }
                    break;

                case 2: // scrolls

                    you.item_description[i][j] = random2(151);
                    you.item_description[4][j] = random2(151);
                    break;

                case 3: // rings

                    if (random2(2) > 0)
                        you.item_description[i][j] = random2(13);
                    else
                        you.item_description[i][j] = random2(13) + (random2(13) * 13);
                    break;

                }               // end switch

                for (p = 0; p < 30; p++)        // don't have p < j because some are preassigned

                {
                    if (you.item_description[i][p] == you.item_description[i][j] && j != p)
                        passout = 0;
                }

            }
            while (passout == 0);

        }
    }



    for (i = 0; i < 4; i++)
    {
        for (j = 0; j < 30; j++)
        {
            you.item_description[i][j] += 1;    // NEED THIS BIT!

        }
    }

    for (i = 0; i < 50; i++)
    {
        if (you.skills[i] == 0)
            continue;

// you.skill_points [i] = skill_exp_needed(you.skills [i] + 1) * species_skills(i, you.species) / 100;

        you.skill_points[i] = skill_exp_needed(you.skills[i] + 1) + 1;
        if (i == 25)
        {
            you.skill_points[i] = ((skill_exp_needed(you.skills[i] + 1) + 1) * 130) / 100;
        }
        if (i == 38)
        {
            you.skill_points[i] = ((skill_exp_needed(you.skills[i] + 1) + 1) * 70) / 100;
        }
        if (you.skill_points[i] > skill_exp_needed(2) * species_skills(i, you.species) / 100)
            you.skills[i] = 1;
        else
            you.skills[i] = 0;
        if (you.skill_points[i] > skill_exp_needed(3) * species_skills(i, you.species) / 100)
            you.skills[i] = 2;
        if (you.skill_points[i] > skill_exp_needed(4) * species_skills(i, you.species) / 100)
            you.skills[i] = 3;
        if (you.skill_points[i] > skill_exp_needed(5) * species_skills(i, you.species) / 100)
            you.skills[i] = 4;
        if (you.skill_points[i] > skill_exp_needed(6) * species_skills(i, you.species) / 100)
            you.skills[i] = 5;
        if (you.skill_points[i] > skill_exp_needed(7) * species_skills(i, you.species) / 100)
            you.skills[i] = 6;
        if (you.skill_points[i] > skill_exp_needed(8) * species_skills(i, you.species) / 100)
            you.skills[i] = 7;
        if (you.skill_points[i] > skill_exp_needed(9) * species_skills(i, you.species) / 100)
            you.skills[i] = 8;
    }

    for (i = 0; i < 52; i++)
    {
        if (you.inv_class[i] != OBJ_WEAPONS)
        {
            set_id(you.inv_class[i], you.inv_type[i], 1);
        }
    }


    if (you.char_class == JOB_WIZARD || you.char_class == JOB_CONJURER)
        you.spells[0] = SPELL_MAGIC_DART;
    if (you.char_class == JOB_NECROMANCER)
        you.spells[0] = SPELL_PAIN;
    if (you.char_class == JOB_NECROMANCER)
        you.spells[1] = SPELL_ANIMATE_SKELETON;
    if (you.char_class == JOB_FIRE_ELEMENTALIST)
        you.spells[0] = SPELL_BURN;
    if (you.char_class == JOB_ICE_ELEMENTALIST)
        you.spells[0] = SPELL_FREEZE;
    if (you.char_class == JOB_SUMMONER)
        you.spells[0] = SPELL_SUMMON_SMALL_MAMMAL;

    if (you.spells[0] != 210)
        you.spell_no = 1;
    else
        you.spell_no = 0;
    if (you.char_class == JOB_PRIEST || you.char_class == JOB_PALADIN)
        set_id(OBJ_POTIONS, POT_HEALING, 1);

    // Now handled by a function in player.cc
    // you.spell_levels = you.skills[SK_SPELLCASTING] * 2 - (you.spells[0] != 210) - (you.spells[1] != 210);


    // tmpfile purging removed in favour of marking
    for (int lvl= 0; lvl < MAX_LEVELS; lvl++)
        for (int dng= 0; dng < MAX_BRANCHES; dng++)
            tmp_file_pairs[ lvl ][ dng ] = false;

// This is the temporary file purging code
#if 0
    char del_file[kFileNameSize];
    char glorpstr[kFileNameSize];

#ifdef SAVE_DIR_PATH
    sprintf(glorpstr, SAVE_DIR_PATH "%s%d", you.your_name, getuid());
#else
    strncpy(glorpstr, you.your_name, kFileNameLen);

// glorpstr [strlen(glorpstr)] = 0;
    // This is broken. Length is not valid yet! We have to check if we got a
    // trailing NULL; if not, write one:
    if (strlen(you.your_name) > kFileNameLen - 1)       /* is name 6 chars or more? */
        glorpstr[kFileNameLen] = '\0';  /* if so, char 7 should be NULL */
#endif

/*int fi = 0;
   char st_prn [6];

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
    for (del_file[dun] = 'a'; del_file[dun] <= '~'; del_file[dun]++)
    {
        for (del_file[tens] = '0'; del_file[tens] < '5'; del_file[tens]++)
        {
            for (del_file[ones] = '0'; del_file[ones] <= '9'; del_file[ones]++)
            {
                unlink(del_file);
            }
        }
    }
#endif

/*
   char hbjh [5];

   if (level_saved < 10) strcpy(extens, "0");
   itoa(level_saved, hbjh, 10);
   strcat(extens, hbjh);
   corr_level [2] = you.where_are_you + 97;
   corr_level [3] = 0; / * null-terminate it * /
   strcpy(cha_fil, "");
   strncat(cha_fil, you.your_name, kFileNameLen);
   strcat(cha_fil, ".");
   if (was_a_labyrinth == 1) strcat(cha_fil, "lab"); / * temporary level * /
   else strcat(cha_fil, extens);
 */

    for (i = 0; i < 30; i++)
    {
        you.branch_stairs[i] = 100;
    }
/* Places the staircases to the branch levels: */

    you.branch_stairs[STAIRS_ORCISH_MINES] = 5 + random2(6);

    you.branch_stairs[STAIRS_HIVE] = 10 + random2(6);

    you.branch_stairs[STAIRS_LAIR] = 7 + random2(6);

    you.branch_stairs[STAIRS_SLIME_PITS] = you.branch_stairs[STAIRS_LAIR]
                                                            + random2(4) + 3;

    you.branch_stairs[STAIRS_VAULTS] = 13 + random2(6);

    you.branch_stairs[STAIRS_CRYPT] = you.branch_stairs[STAIRS_VAULTS]
                                                            + random2(3) + 2;

    you.branch_stairs[STAIRS_HALL_OF_BLADES] = you.branch_stairs[STAIRS_CRYPT]
                                                            + 4;

    you.branch_stairs[STAIRS_HALL_OF_ZOT] = 26;

    you.branch_stairs[STAIRS_ECUMENICAL_TEMPLE] = 3 + random2(4);

    you.branch_stairs[STAIRS_SNAKE_PIT] = you.branch_stairs[STAIRS_LAIR]
                                                            + random2(2) + 6;

    you.branch_stairs[STAIRS_ELVEN_HALLS] =
                    you.branch_stairs[STAIRS_ORCISH_MINES] + random2(2) + 3;

    you.branch_stairs[STAIRS_TOMB] = you.branch_stairs[STAIRS_CRYPT]
                                                            + random2(2) + 2;

    you.branch_stairs[STAIRS_SWAMP] = you.branch_stairs[2] + random2(6) + 2;

    return 1;

}                               // end of new_game()



char class_allowed(char speci, char char_class)
{
    switch (char_class)
    {
    case JOB_FIGHTER:           // fighter

        switch (speci)
        {
        case SP_OGRE_MAGE:
            return 0;           // O-mage

        case SP_SPRIGGAN:
            return 0;           // spriggan

        }
        return 1;

    case JOB_WIZARD:            // wizard

        switch (speci)
        {
        case SP_HILL_ORC:
        case SP_HALFLING:
        case SP_HILL_DWARF:
        case SP_KOBOLD:
        case SP_MUMMY:
        case SP_GNOME:          // gnome
            // case 8:

        case SP_OGRE:
        case SP_TROLL:
        case SP_SPRIGGAN:       // spriggan

        case SP_MINOTAUR:       // minotaur

        case SP_GHOUL:          // Ghoul

        case SP_KENKU:          // Kenku

            return 0;
        }
        return 1;

    case JOB_PRIEST:            // priest

        switch (speci)
        {
        case SP_HALFLING:
        case SP_KOBOLD:
        case SP_MUMMY:
        case SP_NAGA:
        case SP_GNOME:          // gnome

        case SP_OGRE:
        case SP_TROLL:
        case SP_OGRE_MAGE:
        case SP_RED_DRACONIAN:  /* Drac */
        case SP_WHITE_DRACONIAN:
        case SP_GREEN_DRACONIAN:
        case SP_GOLDEN_DRACONIAN:
        case SP_GREY_DRACONIAN:
        case SP_BLACK_DRACONIAN:
        case SP_PURPLE_DRACONIAN:
        case SP_MOTTLED_DRACONIAN:
        case SP_PALE_DRACONIAN:
        case SP_UNK0_DRACONIAN:
        case SP_UNK1_DRACONIAN:
        case SP_UNK2_DRACONIAN: /* Draconians */
        case SP_CENTAUR:        /* Centaur */
        case SP_DEMIGOD:        /* Demigod */
        case SP_SPRIGGAN:       /* spriggan */
        case SP_MINOTAUR:       // minotaur

        case SP_DEMONSPAWN:     // demonspawn

        case SP_GHOUL:          // Ghoul

        case SP_KENKU:          // Kenku

            return 0;
        }
        return 1;

    case JOB_THIEF:             // thief

        switch (speci)
        {
        case SP_MUMMY:
        case SP_OGRE:
        case SP_TROLL:
        case SP_OGRE_MAGE:
        case SP_CENTAUR:        /* Centaur */
        case SP_SPRIGGAN:       // spriggan

        case SP_MINOTAUR:       // minotaur

        case SP_GHOUL:          // Ghoul

        case SP_KENKU:          // Kenku

            return 0;
        }
        return 1;

    case JOB_GLADIATOR: // glad

        switch (speci)
        {
        case SP_HUMAN:
        case SP_HILL_DWARF:
        case SP_MOUNTAIN_DWARF:
        case SP_HILL_ORC:
        case SP_RED_DRACONIAN:  /* Drac */
        case SP_WHITE_DRACONIAN:
        case SP_GREEN_DRACONIAN:
        case SP_GOLDEN_DRACONIAN:
        case SP_GREY_DRACONIAN:
        case SP_BLACK_DRACONIAN:
        case SP_PURPLE_DRACONIAN:
        case SP_MOTTLED_DRACONIAN:
        case SP_PALE_DRACONIAN:
        case SP_UNK0_DRACONIAN:
        case SP_UNK1_DRACONIAN:
        case SP_UNK2_DRACONIAN: /* Draconians */
        case SP_CENTAUR:
        case SP_DEMIGOD:
        case SP_MINOTAUR:       // minotaur

        case SP_DEMONSPAWN:     // demonspawn

        case SP_KENKU:          // Kenku

            return 1;
        }
        return 0;

    case JOB_NECROMANCER:       // necromancer

        switch (speci)
        {
        case SP_HUMAN:
        case SP_DEEP_ELF:
        case SP_SLUDGE_ELF:
        case SP_HILL_ORC:
        case SP_KOBOLD:
        case SP_MUMMY:
        case SP_NAGA:
        case SP_OGRE_MAGE:      /* Ogre-mage */
        case SP_DEMIGOD:        /* Demigod */
        case SP_DEMONSPAWN:     // demonspawn

        case SP_KENKU:          // Kenku

            return 1;
        }
        return 0;

    case JOB_PALADIN:           // Paladin

        switch (speci)
        {
        case SP_HUMAN:
        case SP_MOUNTAIN_DWARF:
            return 1;
        }
        return 0;

    case JOB_ASSASSIN:          // assassin

        switch (speci)
        {
        case SP_HUMAN:
        case SP_ELF:
        case SP_HIGH_ELF:
        case SP_GREY_ELF:
        case SP_DEEP_ELF:
        case SP_SLUDGE_ELF:
        case SP_HILL_ORC:
        case SP_KOBOLD:
        case SP_MUMMY:
        case SP_NAGA:
        case SP_DEMIGOD:        /* Demigod */
        case SP_DEMONSPAWN:     // demonspawn

        case SP_KENKU:          // Kenku

            return 1;
        }
        return 0;

    case JOB_BERSERKER: // berserker

        switch (speci)
        {
        case SP_HUMAN:
        case SP_HILL_DWARF:
        case SP_HILL_ORC:
        case SP_OGRE:
        case SP_TROLL:
        case SP_CENTAUR:        /* Centaur */
        case SP_MINOTAUR:       // minotaur

        case SP_DEMONSPAWN:     // demonspawn

            return 1;
        }
        return 0;

    case JOB_HUNTER:

        switch (speci)
        {
        case SP_HUMAN:              // these use bows
        case SP_ELF:
        case SP_HIGH_ELF:
        case SP_GREY_ELF:
        case SP_SLUDGE_ELF:
        case SP_NAGA:
        case SP_RED_DRACONIAN:
        case SP_WHITE_DRACONIAN:
        case SP_GREEN_DRACONIAN:
        case SP_GOLDEN_DRACONIAN:
        case SP_GREY_DRACONIAN:
        case SP_BLACK_DRACONIAN:
        case SP_PURPLE_DRACONIAN:
        case SP_MOTTLED_DRACONIAN:
        case SP_PALE_DRACONIAN:
        case SP_UNK0_DRACONIAN:
        case SP_UNK1_DRACONIAN:
        case SP_UNK2_DRACONIAN:
        case SP_CENTAUR:
        case SP_DEMIGOD:
        case SP_MINOTAUR:
        case SP_DEMONSPAWN:
        case SP_KENKU:

        case SP_HILL_DWARF:         // these use crossbows
        case SP_MOUNTAIN_DWARF:
        case SP_HILL_ORC:

        case SP_GNOME:              // these are slingers
        case SP_HALFLING:

            return 1;
        }
        return 0;

    case JOB_CONJURER:          // Conjurer

        switch (speci)
        {
        case SP_KOBOLD:
        case SP_MUMMY:
        case SP_HALFLING:
        case SP_GNOME:          // gnome

        case SP_OGRE:
        case SP_TROLL:
        case SP_CENTAUR:        /* Centaur */
        case SP_SPRIGGAN:       // spriggan

        case SP_MINOTAUR:       // minotaur

        case SP_GHOUL:          // Ghoul

            return 0;
        }
        return 1;

    case JOB_ENCHANTER: // Enchanter

        switch (speci)
        {
        case SP_HILL_ORC:
        case SP_KOBOLD:
        case SP_MUMMY:
        case SP_GNOME:          // gnome

        case SP_OGRE:
        case SP_TROLL:
        case SP_MINOTAUR:       // minotaur

        case SP_GHOUL:          // Ghoul

        case SP_KENKU:          // Kenku

            return 0;
        }
        return 1;

    case JOB_FIRE_ELEMENTALIST: // Fire elementalist

        switch (speci)
        {
        case SP_HUMAN:
        case SP_ELF:
        case SP_HIGH_ELF:
        case SP_DEEP_ELF:
        case SP_SLUDGE_ELF:
        case SP_HILL_DWARF:
        case SP_MOUNTAIN_DWARF:
        case SP_HILL_ORC:
        case SP_OGRE_MAGE:      /* Ogre-mage */
        case SP_CENTAUR:        /* Centaur */
        case SP_DEMIGOD:        /* Demigod */
        case SP_DEMONSPAWN:     // demonspawn

        case SP_KENKU:          // Kenku

            return 1;
        }
        return 0;

    case JOB_ICE_ELEMENTALIST:  // Ice elementalist

        switch (speci)
        {
        case SP_HUMAN:
        case SP_ELF:
        case SP_HIGH_ELF:
        case SP_DEEP_ELF:
        case SP_SLUDGE_ELF:
        case SP_OGRE_MAGE:      /* Ogre-mage */
        case SP_CENTAUR:        /* Centaur */
        case SP_DEMIGOD:        /* Demigod */
        case SP_DEMONSPAWN:     // demonspawn

            return 1;
        }
        return 0;

    case JOB_SUMMONER:          // Summoner

        switch (speci)
        {
        case SP_HALFLING:
        case SP_MOUNTAIN_DWARF:
        case SP_HILL_DWARF:
        case SP_MUMMY:
        case SP_GNOME:          // gnome

        case SP_OGRE:
        case SP_TROLL:
        case SP_CENTAUR:        /* Centaur */
        case SP_SPRIGGAN:       // spriggan

        case SP_MINOTAUR:       // minotaur

        case SP_GHOUL:          // Ghoul

            return 0;
        }
        return 1;

    case JOB_AIR_ELEMENTALIST:  // Air elementalist

        switch (speci)
        {
        case SP_HUMAN:
        case SP_ELF:
        case SP_HIGH_ELF:
        case SP_GREY_ELF:
        case SP_DEEP_ELF:
        case SP_SLUDGE_ELF:
        case SP_OGRE_MAGE:      /* Ogre-mage */
        case SP_CENTAUR:        /* Centaur */
        case SP_DEMIGOD:        /* Demigod */
        case SP_DEMONSPAWN:     // demonspawn

        case SP_KENKU:          // Kenku

            return 1;
        }
        return 0;


    case JOB_EARTH_ELEMENTALIST:        // Earth elementalist

        switch (speci)
        {
        case SP_HUMAN:
        case SP_DEEP_ELF:
        case SP_SLUDGE_ELF:
        case SP_HILL_DWARF:
        case SP_MOUNTAIN_DWARF:
        case SP_HILL_ORC:
        case SP_GNOME:          // gnome

        case SP_OGRE_MAGE:      /* Ogre-mage */
        case SP_CENTAUR:        /* Centaur */
        case SP_DEMIGOD:        /* Demigod */
        case SP_DEMONSPAWN:     // demonspawn

            return 1;
        }
        return 0;

    case JOB_CRUSADER:          // Crusader

        switch (speci)
        {
        case SP_HUMAN:
        case SP_ELF:
        case SP_HIGH_ELF:
        case SP_GREY_ELF:
        case SP_DEEP_ELF:
        case SP_SLUDGE_ELF:
        case SP_HILL_ORC:
        case SP_OGRE_MAGE:
        case SP_RED_DRACONIAN:  /* Drac */
        case SP_WHITE_DRACONIAN:
        case SP_GREEN_DRACONIAN:
        case SP_GOLDEN_DRACONIAN:
        case SP_GREY_DRACONIAN:
        case SP_BLACK_DRACONIAN:
        case SP_PURPLE_DRACONIAN:
        case SP_MOTTLED_DRACONIAN:
        case SP_PALE_DRACONIAN:
        case SP_UNK0_DRACONIAN:
        case SP_UNK1_DRACONIAN:
        case SP_UNK2_DRACONIAN: /* Draconians */
        case SP_CENTAUR:        /* Centaur */
        case SP_DEMIGOD:        /* Demigod */
        case SP_DEMONSPAWN:     // demonspawn

            return 1;
        }
        return 0;

    case JOB_DEATH_KNIGHT:      // Death knight

        switch (speci)
        {
        case SP_HUMAN:
        case SP_DEEP_ELF:
        case SP_SLUDGE_ELF:
        case SP_HILL_ORC:
        case SP_MUMMY:
        case SP_NAGA:
        case SP_RED_DRACONIAN:  /* Drac */
        case SP_WHITE_DRACONIAN:
        case SP_GREEN_DRACONIAN:
        case SP_GOLDEN_DRACONIAN:
        case SP_GREY_DRACONIAN:
        case SP_BLACK_DRACONIAN:
        case SP_PURPLE_DRACONIAN:
        case SP_MOTTLED_DRACONIAN:
        case SP_PALE_DRACONIAN:
        case SP_UNK0_DRACONIAN:
        case SP_UNK1_DRACONIAN:
        case SP_UNK2_DRACONIAN: /* Draconians */
        case SP_CENTAUR:        /* Centaur */
        case SP_DEMIGOD:        /* Demigod */
        case SP_DEMONSPAWN:     // demonspawn

        case SP_KENKU:          // Kenku

            return 1;
        }
        return 0;

    case JOB_VENOM_MAGE:        // tainter

        switch (speci)
        {
        case SP_HUMAN:
        case SP_DEEP_ELF:
        case SP_SLUDGE_ELF:
        case SP_HILL_ORC:
        case SP_KOBOLD:
        case SP_NAGA:
        case SP_OGRE_MAGE:      /* Ogre-mage */
        case SP_RED_DRACONIAN:  /* Drac */
        case SP_WHITE_DRACONIAN:
        case SP_GREEN_DRACONIAN:
        case SP_GOLDEN_DRACONIAN:
        case SP_GREY_DRACONIAN:
        case SP_BLACK_DRACONIAN:
        case SP_PURPLE_DRACONIAN:
        case SP_MOTTLED_DRACONIAN:
        case SP_PALE_DRACONIAN:
        case SP_UNK0_DRACONIAN:
        case SP_UNK1_DRACONIAN:
        case SP_UNK2_DRACONIAN: /* Draconians */
        case SP_DEMIGOD:        /* Demigod */
        case SP_SPRIGGAN:       /* Spriggan */
        case SP_DEMONSPAWN:     // demonspawn

        case SP_KENKU:          // Kenku

            return 1;
        }
        return 0;

    case JOB_CHAOS_KNIGHT:      // chaos knight

        switch (speci)
        {
        case SP_HUMAN:
        case SP_ELF:
        case SP_HIGH_ELF:
        case SP_DEEP_ELF:
        case SP_SLUDGE_ELF:
        case SP_HILL_ORC:
        case SP_KOBOLD:
        case SP_NAGA:
        case SP_RED_DRACONIAN:  /* Drac */
        case SP_WHITE_DRACONIAN:
        case SP_GREEN_DRACONIAN:
        case SP_GOLDEN_DRACONIAN:
        case SP_GREY_DRACONIAN:
        case SP_BLACK_DRACONIAN:
        case SP_PURPLE_DRACONIAN:
        case SP_MOTTLED_DRACONIAN:
        case SP_PALE_DRACONIAN:
        case SP_UNK0_DRACONIAN:
        case SP_UNK1_DRACONIAN:
        case SP_UNK2_DRACONIAN: /* Draconians */
        case SP_CENTAUR:        /* Centaur */
        case SP_MINOTAUR:       // minotaur

        case SP_DEMONSPAWN:     // demonspawn

            return 1;
        }
        return 0;

    case JOB_TRANSMUTER:        // transmuter

        switch (speci)
        {
        case SP_HILL_ORC:
        case SP_HALFLING:
        case SP_HILL_DWARF:
        case SP_KOBOLD:
        case SP_MUMMY:
        case SP_GNOME:          // gnome

        case SP_OGRE:
        case SP_TROLL:
        case SP_RED_DRACONIAN:  /* Drac */
        case SP_WHITE_DRACONIAN:
        case SP_GREEN_DRACONIAN:
        case SP_GOLDEN_DRACONIAN:
        case SP_GREY_DRACONIAN:
        case SP_BLACK_DRACONIAN:
        case SP_PURPLE_DRACONIAN:
        case SP_MOTTLED_DRACONIAN:
        case SP_PALE_DRACONIAN:
        case SP_UNK0_DRACONIAN:
        case SP_UNK1_DRACONIAN:
        case SP_UNK2_DRACONIAN: /* Draconians */
        case SP_MINOTAUR:       // minotaur

        case SP_GHOUL:          // Ghoul

        case SP_KENKU:          // Kenku

            return 0;
        }
        return 1;

    case JOB_HEALER:            // healer

        switch (speci)
        {
        case SP_HALFLING:
        case SP_KOBOLD:
        case SP_MUMMY:
        case SP_NAGA:
        case SP_GNOME:          // gnome

        case SP_OGRE:
        case SP_TROLL:
        case SP_OGRE_MAGE:
        case SP_RED_DRACONIAN:  /* Drac */
        case SP_WHITE_DRACONIAN:
        case SP_GREEN_DRACONIAN:
        case SP_GOLDEN_DRACONIAN:
        case SP_GREY_DRACONIAN:
        case SP_BLACK_DRACONIAN:
        case SP_PURPLE_DRACONIAN:
        case SP_MOTTLED_DRACONIAN:
        case SP_PALE_DRACONIAN:
        case SP_UNK0_DRACONIAN:
        case SP_UNK1_DRACONIAN:
        case SP_UNK2_DRACONIAN: /* Draconians */
        case SP_DEMIGOD:        /* Demigod */
        case SP_SPRIGGAN:       // spriggan

        case SP_MINOTAUR:       // minotaur

        case SP_DEMONSPAWN:     // demonspawn

        case SP_GHOUL:          // Ghoul

        case SP_KENKU:          // Kenku

            return 0;
        }
        return 1;

    case JOB_REAVER:            // Reaver

        switch (speci)
        {
        case SP_HUMAN:
        case SP_ELF:
        case SP_HIGH_ELF:
        case SP_DEEP_ELF:
        case SP_SLUDGE_ELF:
        case SP_HILL_ORC:
        case SP_KOBOLD:
        case SP_NAGA:
        case SP_RED_DRACONIAN:  /* Drac */
        case SP_WHITE_DRACONIAN:
        case SP_GREEN_DRACONIAN:
        case SP_GOLDEN_DRACONIAN:
        case SP_GREY_DRACONIAN:
        case SP_BLACK_DRACONIAN:
        case SP_PURPLE_DRACONIAN:
        case SP_MOTTLED_DRACONIAN:
        case SP_PALE_DRACONIAN:
        case SP_UNK0_DRACONIAN:
        case SP_UNK1_DRACONIAN:
        case SP_UNK2_DRACONIAN: /* Draconians */
        case SP_CENTAUR:        /* Centaur */
        case SP_DEMIGOD:        /* Demigod */
        case SP_DEMONSPAWN:     // demonspawn

        case SP_KENKU:          // Kenku

            return 1;
        }
        return 0;

    case JOB_STALKER:           /* stalker */
        switch (speci)
        {
        case SP_HUMAN:
        case SP_ELF:
        case SP_HIGH_ELF:
        case SP_GREY_ELF:
        case SP_DEEP_ELF:
        case SP_SLUDGE_ELF:
        case SP_HILL_ORC:
        case SP_KOBOLD:
        case SP_NAGA:
        case SP_DEMIGOD:        /* Demigod */
        case SP_DEMONSPAWN:     // demonspawn

        case SP_KENKU:          // Kenku

            return 1;
        }
        return 0;

    case JOB_MONK:              // Monk

        switch (speci)
        {
        case SP_HILL_DWARF:
        case SP_KOBOLD:
        case SP_MUMMY:
        case SP_NAGA:
        case SP_GNOME:
        case SP_OGRE:
        case SP_TROLL:
        case SP_OGRE_MAGE:
        case SP_CENTAUR:
        case SP_SPRIGGAN:       // spriggan

        case SP_GHOUL:          // Ghoul

            return 0;
        }
        return 1;


    case JOB_WARPER:            // warper

        switch (speci)
        {
        case SP_HILL_ORC:
        case SP_HALFLING:
        case SP_HILL_DWARF:
        case SP_KOBOLD:
        case SP_MUMMY:
        case SP_GNOME:          // gnome

        case SP_OGRE:
        case SP_TROLL:
        case SP_RED_DRACONIAN:  /* Drac */
        case SP_WHITE_DRACONIAN:
        case SP_GREEN_DRACONIAN:
        case SP_GOLDEN_DRACONIAN:
        case SP_GREY_DRACONIAN:
        case SP_BLACK_DRACONIAN:
        case SP_PURPLE_DRACONIAN:
        case SP_MOTTLED_DRACONIAN:
        case SP_PALE_DRACONIAN:
        case SP_UNK0_DRACONIAN:
        case SP_UNK1_DRACONIAN:
        case SP_UNK2_DRACONIAN: /* Draconians */
        case SP_MINOTAUR:       // minotaur

        case SP_GHOUL:          // Ghoul

        case SP_KENKU:          // Kenku

            return 0;
        }
        return 1;


    default:
        return 0;

    }

}

void choose_weapon(void)
{

    char keyin = 0;

    if (you.char_class == JOB_CHAOS_KNIGHT)
    {
        switch (random2(4))
        {
        case 0:
            you.inv_type[0] = WPN_SHORT_SWORD;
            return;
        case 1:
            you.inv_type[0] = WPN_MACE;
            return;
        case 2:
            you.inv_type[0] = WPN_HAND_AXE;
            return;
        case 3:
            you.inv_type[0] = WPN_SPEAR;
            return;
        }
        return;
    }

    clrscr();
    cprintf(EOL " You have a choice of weapons:" EOL);
    cprintf("a - short sword" EOL);
    cprintf("b - mace" EOL);
    cprintf("c - hand axe" EOL);
    cprintf("d - spear" EOL);

    if (you.char_class == JOB_GLADIATOR)
        cprintf("e - trident" EOL);

    cprintf(EOL "Which weapon? ");

getkey:
    keyin = get_ch();
    switch (keyin)
    {
    case 'a':
        you.inv_type[0] = WPN_SHORT_SWORD;
        return;
    case 'b':
        you.inv_type[0] = WPN_MACE;
        return;
    case 'c':
        you.inv_type[0] = WPN_HAND_AXE;
        return;
    case 'd':
        you.inv_type[0] = WPN_SPEAR;
        return;
    case 'e':
        if (you.char_class == JOB_GLADIATOR)
        {
            you.inv_type[0] = WPN_TRIDENT;
            return;
        }
        // fall through

    default:
        goto getkey;
    }
}


void init_player(void)
{

    you.deaths_door = 0;
    you.special_wield = 0;
    you.berserker = 0;
    you.elapsed_time = 0;
    you.synch_time = 0;
    you.disease = 0;

    you.berserk_penalty = 0;
    you.exhausted = 0;

    you.level_type = 0;
// 0 = normal dungeon level
    // 1 = labyrinth

    you.max_level = 1;
    you.x_pos = 0;
    you.y_pos = 0;
    you.invis = 0;
    you.conf = 0;
    you.paralysis = 0;
    you.slow = 0;
    you.haste = 0;              // haste

    you.might = 0;              // might

    you.levitation = 0;         // levitation

    you.poison = 0;             // poison!!

    you.rotting = 0;
    you.shock_shield = 0;
    you.hp = 0;
    you.hp_max = 0;
    you.magic_points = 0;
    you.max_magic_points = 0;
    you.strength = 0;
    you.intel = 0;
    you.dex = 0;
    you.max_strength = 0;
    you.max_intel = 0;
    you.max_dex = 0;
    you.experience = 0;
    you.experience_level = 1;
    you.gold = 0;
    you.char_class = JOB_FIGHTER;
    you.speed = 10;             // 0.75;

    you.burden = 0;             // total weight of items carried.

    you.burden_state = 0;
    you.num_inv_items = 0;      // number of items carried.

    you.spell_no = 0;
    // you.spell_levels = 0;
    you.char_direction = 0;
// 0 = going down
    // 1 = going up!
    you.where_are_you = 0;
// 0 = normal dungeon level
    // 1 = dis
    // 2 = hell
    you.pet_target = 201;
    you.your_level = 1;
    you.delay_t = 0;
    you.delay_doing = 0;
    you.running = 0;
    you.run_x = 0;
    you.run_y = 0;
    you.level_type = 0;
    you.hunger_state = 3;

    int i = 0;

    for (i = 0; i < 10; i++)
    {
        you.equip[i] = -1;
    }

    you.religion = GOD_NO_GOD;
    you.piety = 0;

    for (i = 0; i < 20; i++)
    {
        you.gods[i] = 0;
    }

    int ic = 0;

    strcpy(ghost.name, "");
    for (ic = 0; ic < 20; ic++)
    {
        ghost.values[ic] = 0;
    }

    for (ic = 0; ic < 100; ic++)
    {
        you.mutation[ic] = 0;
    }

    for (ic = 0; ic < 100; ic++)
    {
        you.demon_pow[ic] = 0;
    }

    for (ic = 0; ic < 50; ic++)
    {
        you.had_item[ic] = 0;
    }

    for (ic = 0; ic < NO_UNRANDARTS; ic++)
    {
        set_unrandart_exist(ic, 0);
    }


}

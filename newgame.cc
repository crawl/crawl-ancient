/*
 *  File:       newgame.cc
 *  Summary:    Functions used when starting a new game.
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 *     <14>      10-Jan-2000    DLB   class_allowed() lists excluded
 *                                       species for all but hunters
 *                                    some clean-up of init_player()
 *     <13>      1/10/2000      BCR   Made ogre berserkers get club
 *                                    skill, Trolls get unarmed skill
 *                                    Halflings can be assasins and
 *                                    warpers
 *     <12>      12/4/99        jmf   Gave Paladins more armour skill + a
 *                                    long sword (to compensate for
 *                                    their inability to use poison).
 *                                    Allowed Spriggan Stalkers (since
 *                                    that's basically just a venom mage
 *                                    + assassin, both of which are now
 *                                    legal).
 *     <11>      11/22/99       LRH   Er, re-un-capitalised class
 *                                    names (makes them distinguish-
 *                                    able in score list)
 *     <10>      10/31/99       CDL   Allow Spriggan Assassins
 *                                    Remove some old comments
 *      <9>      10/12/99       BCR   Made sure all the classes are
 *                                    capitalized correctly.
 *      <8>      9/09/99        BWR   Changed character selection
 *                                    screens look (added sub-species
 *                                    menus from Dustin Ragan)
 *      <7>      7/13/99        BWR   Changed assassins to use
 *                                    hand crossbows, changed
 *                                    rangers into hunters.
 *      <6>      6/22/99        BWR   Added new rangers/slingers
 *      <5>      6/17/99        BCR   Removed some Linux/Mac filename
 *                                    weirdness
 *      <4>      6/13/99        BWR   sys_env support
 *      <3>      6/11/99        DML   Removed tmpfile purging.
 *      <2>      5/20/99        BWR   CRAWL_NAME, new berserk, upped
 *                                    troll food consumption, added
 *                                    demonspawn transmuters.
 *      <1>      -/--/--        LRH   Created
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

bool class_allowed(unsigned char speci, char char_class);
void init_player(void);
void choose_weapon(void);
void last_paycheck( void );
void species_stat_init( unsigned char which_species );
void jobs_stat_init( int which_job );

extern char wield_change;

char new_game(void)
{
    int i;                       // loop variable
    int j;                       // another loop variable
    int handle;
    char your_nam[kNameLen];
    char char_fil[kFileNameSize];
    char keyn;
    char weap_skill = 0;
    int to_hit_bonus;

    init_player();

    you.exp_available = 25;

    you.hunger = 6000;
    //you.hunger_inc = 3;
    //you.rate_regen = 6;
    you.magic_points_regeneration = 0;

    for (i = 0; i < 30; i++)
    {
        //lev_ex [i] = 0;
        if (i <= 25)
          you.spells[i] = SPELL_NO_SPELL;
    }

// Have to add resistances etc to this!!!

    for (i = 0; i < 52; i++)
    {
        you.inv_quantity[i] = 0;
        you.inv_ident[i] = 3;
    }

    textcolor(LIGHTGREY);

    if (sys_env.crawl_name)                   // Name is set from environment variable
      strncpy( you.your_name, sys_env.crawl_name, kNameLen );
    else if (you.your_name[0] != '\0')        // Name was set from the init file
    {    }
    else
    {
        cprintf(EOL "Hello, and welcome to Dungeon Crawl v" VERSION "!");
        cprintf(EOL "(Copyright 1997, 1998, 1999, 2000 Linley Henzell)");
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
 * And why check for a filename called bones only when not on Linux?
 */

// if SAVE_DIR_PATH is defined, the userid will be tacked onto the end of
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
    cprintf("a - Human                     b - Elf" EOL);
    cprintf("c - High Elf                  d - Grey Elf" EOL);
    cprintf("e - Deep Elf                  f - Sludge Elf" EOL);
    cprintf("g - Hill Dwarf                h - Mountain Dwarf" EOL);
    cprintf("i - Halfling                  j - Hill Orc" EOL);
    cprintf("k - Kobold                    l - Mummy" EOL);
    cprintf("m - Naga                      n - Gnome" EOL);
    cprintf("o - Ogre                      p - Troll" EOL);
    cprintf("q - Ogre-Mage                 r - Draconian" EOL);
    cprintf("s - Centaur                   t - Demigod" EOL);
    cprintf("u - Spriggan                  v - Minotaur" EOL);
    cprintf("w - Demonspawn                x - Ghoul" EOL);
    cprintf("y - Kenku" EOL);
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
#ifdef SEPARATE_SELECTION_SCREENS_FOR_SUBSPECIES
/* *BCR* I changed the way the alternate species selection works.
 * Before, there was a separate case for the super classes, and
 * they would assign the race seperately.  In the new way of doing it,
 * I convert the choice from the subspecies selection screen into the
 * keyin value used in the original switch statement.  This way, if
 * we change the stats for these races, we won't have to modify in two
 * places.
 */
    switch (keyn)
    {
    // the E and D cases allow you to select elf/dwarf subspecies
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

        // *BCR* Elves start at 'b' and go from there, so 'a' + 1 works
        keyn++;
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
            keyn = ( (coinflip()) ? 'a' : 'b' );
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

        // *BCR* Dwarves start at 'g' and go from there, so 'a' + 6 works
        keyn += 6;
        break;
    }
#endif // ends the alternate species selection method
    switch (keyn)
    {

    case 'a':
        you.species = SP_HUMAN;
        you.strength = 6;
        you.intel = 6;
        you.dex = 6;
        break;

    case 'b':
        you.species = SP_ELF;
        you.strength = 5;
        you.intel = 8;
        you.dex = 8;
        break;

    case 'c':
        you.species = SP_HIGH_ELF;
        you.strength = 5;
        you.intel = 9;
        you.dex = 8;
        break;

    case 'd':
        you.species = SP_GREY_ELF;
        you.strength = 4;
        you.intel = 9;
        you.dex = 8;
        break;

    case 'e':
        you.species = SP_DEEP_ELF;
        you.strength = 4;
        you.intel = 10;
        you.dex = 7;
        break;

    case 'f':
        you.species = SP_SLUDGE_ELF;
        you.strength = 5;
        you.intel = 7;
        you.dex = 8;
        break;

    case 'g':
        you.species = SP_HILL_DWARF;
        you.strength = 9;
        you.intel = 3;
        you.dex = 4;
        break;

    case 'h':
        you.species = SP_MOUNTAIN_DWARF;
        you.strength = 9;
        you.intel = 4;
        you.dex = 5;
        break;

    case 'i':
        you.species = SP_HALFLING;
        you.strength = 4;
        you.intel = 6;
        you.dex = 9;
        break;

    case 'j':
        you.species = SP_HILL_ORC;
        you.strength = 9;
        you.intel = 3;
        you.dex = 4;
        break;

    case 'k':
        you.species = SP_KOBOLD;
        you.strength = 4;
        you.intel = 4;
        you.dex = 6;
        break;

    case 'l':
        you.species = SP_MUMMY;
        you.strength = 7;
        you.intel = 3;
        you.dex = 3;
        break;

    case 'm':
        you.species = SP_NAGA;
        you.strength = 8;
        you.intel = 6;
        you.dex = 4;
        break;

    case 'n':
        you.species = SP_GNOME;
        you.strength = 5;
        you.intel = 6;
        you.dex = 7;
        break;

    case 'o':
        you.species = SP_OGRE;
        you.strength = 12;
        you.intel = 3;
        you.dex = 1;
        break;

    case 'p':
        you.species = SP_TROLL;
        you.strength = 13;
        you.intel = 3;
        you.dex = 0;
        break;

    case 'q':
        you.species = SP_OGRE_MAGE;
        you.strength = 9;
        you.intel = 7;
        you.dex = 3;
        break;

    case 'r':                           // draconian
        you.strength = 9;
        you.intel = 6;
        you.dex = 2;

#ifdef ALLOW_DRACONIAN_TYPE_SELECTION
/*
Note - this is a cheat. Draconians are supposed to be random (or why would
the game go to such effort to conceal their type until maturity?)
*/
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
        you.species = SP_CENTAUR;
        you.strength = 8;
        you.intel = 4;
        you.dex = 5;
        break;

    case 't':
        you.species = SP_DEMIGOD;           // more is added to stats later
        you.strength = 7;
        you.intel = 7;
        you.dex = 7;
        break;

    case 'u':
        you.species = SP_SPRIGGAN;
        you.strength = 3;
        you.intel = 6;
        you.dex = 8;
        break;

    case 'v':
        you.species = SP_MINOTAUR;
        you.strength = 10;
        you.intel = 3;
        you.dex = 5;
        break;

    case 'w':
        you.species = SP_DEMONSPAWN;        // more is added, like demigods
        you.strength = 4;
        you.intel = 4;
        you.dex = 4;
        break;

    case 'x':
        you.species = SP_GHOUL;
        you.strength = 7;
        you.intel = 1;
        you.dex = 2;
        break;

    case 'y':
        you.species = SP_KENKU;
        you.strength = 5;
        you.intel = 6;
        you.dex = 7;
        break;

    case '?':
        keyn = 'a' + random2(25);
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

        if ( !class_allowed(you.species, i) )
            continue;

        if (i < 26)
            putch(i + 'a');
        else
            putch((i - 26) + 'A');
        cprintf(" - ");

        switch (i)
        {
        case JOB_FIGHTER:
            cprintf("fighter");
            break;
        case JOB_WIZARD:
            cprintf("Wizard");
            break;
        case JOB_PRIEST:
            cprintf("priest");
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
            cprintf("assassin");
            break;
        case JOB_BERSERKER:
            cprintf("Berserker");
            break;
        case JOB_HUNTER:
            cprintf("Hunter");
            break;
        case JOB_CONJURER:
            cprintf("conjurer");
            break;
        case JOB_ENCHANTER:
            cprintf("enchanter");
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
            cprintf("transmuter");
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
                keyn = 'a' + random2(28);     // was: 97 + ... 22jan2000 {dlb}
            }
            while (keyn == 'x');
            if (keyn == '{')
                keyn = 'A';
            if (keyn == '|')
                keyn = 'B';
        }
        while ( !class_allowed(you.species, keyn - 'a') );     // was: keyn - 97 22jan2000 {dlb}
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

    if ( !class_allowed(you.species, you.char_class) )
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
            //you.AC = 2;
            //you.evasion = 9;

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

            //you.AC = 3;
            //you.evasion = 9;
            choose_weapon();
            cprintf(EOL "A fine choice.");
        }

        you.strength += 7;
        you.dex += 3;

        if (you.species != SP_TROLL)
            you.equip[EQ_WEAPON] = 0;

        you.equip[EQ_BODY_ARMOUR] = 1;

        if (you.species != SP_KOBOLD && you.species != SP_OGRE
                        && you.species != SP_TROLL && you.species != SP_GHOUL)
            you.equip[EQ_SHIELD] = 2;

        last_paycheck();
        // you.res_magic = 3;

        you.skills[SK_FIGHTING] = 3;
        weap_skill = 2;
        if (you.species == SP_KOBOLD)
        {
            you.skills[SK_THROWING] = 2;
            you.skills[SK_DODGING] = 1;
            you.skills[SK_STEALTH] = 1;
            you.skills[SK_STABBING] = 1;
            you.skills[SK_DODGING + random2(3)]++;
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
            you.skills[ ( (coinflip()) ? SK_STABBING : SK_SHIELDS ) ]++;
            you.skills[SK_THROWING] = 2;
            // you.skills[SK_UNARMED_COMBAT] = 1;
        }
        else
            you.skills[SK_FIGHTING] += 2;
        break;

    case JOB_WIZARD:
        strcpy(you.class_name, "Wizard");
        you.hp = 10;
        you.hp_max = 10;
        you.magic_points = 3;
        you.max_magic_points = 3;
        //you.f_abil = 5;
        //you.mag_abil = 10;
        //you.thr_abil = 5;
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
        if ( !one_chance_in(3) )
            you.inv_dam[1] = (4 + random2(2)) * 30;
        you.inv_colour[1] = random2(15) + 1;

        you.strength += 0;  // no change
        you.dex += 3;
        you.intel += 7;

        you.equip[EQ_WEAPON] = 0;
        you.equip[EQ_BODY_ARMOUR] = 1;
        last_paycheck();

        // extra items being tested:

        you.inv_class[2] = OBJ_BOOKS;
        you.inv_type[2] = random2(3);
        you.inv_quantity[2] = 1;
        you.inv_plus[2] = 127;
        you.inv_dam[2] = 1;
        you.inv_colour[2] = CYAN;

        you.skills[SK_DODGING] = 1;
        you.skills[SK_STEALTH] = 1;
        you.skills[ ( (coinflip()) ? SK_DODGING : SK_STEALTH ) ]++;
        you.skills[SK_SPELLCASTING] = 2;
        you.skills[SK_CONJURATIONS] = 1;
        you.skills[SK_ENCHANTMENTS] = 1;
        you.skills[SK_SPELLCASTING + random2(3)]++;
        you.skills[SK_SUMMONINGS + random2(5)]++;
        you.skills[SK_SHORT_BLADES] = 1;
        you.skills[SK_STAVES] = 1;
        break;


    case JOB_PRIEST:
        strcpy(you.class_name, "priest");
        you.piety = 45;
        you.hp = 12;
        you.hp_max = 12;
        you.magic_points = 1;
        you.max_magic_points = 1;

        you.inv_quantity[0] = 1;
        you.inv_class[0] = OBJ_WEAPONS;
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

        you.inv_class[2] = OBJ_POTIONS;
        you.inv_type[2] = POT_HEALING;
        you.inv_quantity[2] = ( (coinflip()) ? 3 : 2 );
        you.inv_plus[2] = 0;
        you.inv_dam[2] = 0;
        you.inv_colour[2] = random2(15) + 1;    // hmmm...

        you.strength += 4;
        you.dex += 2;
        you.intel += 4;

        you.equip[EQ_WEAPON] = 0;
        you.equip[EQ_BODY_ARMOUR] = 1;
        last_paycheck();

        you.skills[SK_FIGHTING] = 2;
        you.skills[SK_DODGING] = 1;
        you.skills[SK_SHIELDS] = 1;
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

    case JOB_THIEF:
        strcpy(you.class_name, "Thief");
        you.hp = 11;
        you.hp_max = 11;
        you.magic_points = 0;
        you.max_magic_points = 0;
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

        you.strength += 3;
        you.dex += 5;
        you.intel += 2;

        you.equip[EQ_WEAPON] = 0;
        you.equip[EQ_BODY_ARMOUR] = 2;
        you.equip[EQ_CLOAK] = 3;
        last_paycheck();

        you.skills[SK_FIGHTING] = 1;
        you.skills[SK_SHORT_BLADES] = 2;

        you.skills[SK_DODGING] = 2;
        you.skills[SK_STEALTH] = 2;
        you.skills[SK_STABBING] = 1;
        you.skills[SK_DODGING + random2(3)]++;

/* if (you.skills [SK_DODGING] == 2) you.evasion ++; */

        you.skills[SK_THROWING] = 1;
        you.skills[SK_DARTS] = 1;

        you.skills[SK_TRAPS_DOORS] = 2;

        break;

    case JOB_GLADIATOR:
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
        last_paycheck();

        you.skills[SK_FIGHTING] = 3;
        weap_skill = 3;

        if (you.species >= SP_RED_DRACONIAN && you.species <= SP_UNK2_DRACONIAN)
            you.skills[SK_DODGING] = 2;
        else
            you.skills[SK_ARMOUR] = 2;
        //you.evasion++;
        you.skills[SK_SHIELDS] = 1;
        you.skills[SK_UNARMED_COMBAT] = 2;
        break;


    case JOB_NECROMANCER:
        strcpy(you.class_name, "Necromancer");
        you.hp = 10;
        you.hp_max = 10;
        you.magic_points = 3;
        you.max_magic_points = 3;
        //you.f_abil = 5;
        //you.mag_abil = 7;
        //you.thr_abil = 5;
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
        //you.AC = 1;
        //you.evasion = 10;
        you.dex += 4;
        you.intel += 6;
        you.equip[EQ_WEAPON] = 0;
        you.equip[EQ_BODY_ARMOUR] = 1;
        last_paycheck();
        you.inv_class[2] = OBJ_BOOKS;

        you.inv_type[2] = BOOK_NECROMANCY;
        you.inv_quantity[2] = 1;
        you.inv_plus[2] = 127;
        you.inv_dam[2] = 0;     //= 1;

        you.inv_colour[2] = DARKGREY;
        //you.res_magic = 10;

        you.skills[SK_DODGING] = 1;
        you.skills[SK_STEALTH] = 1;
        you.skills[ ( (coinflip()) ? SK_DODGING : SK_STEALTH ) ]++;
        //if (you.skills [SK_DODGING] == 2) you.evasion ++;

        you.skills[SK_SPELLCASTING] = 1;
        you.skills[SK_NECROMANCY] = 4;
        you.skills[SK_SHORT_BLADES] = 1;
        you.skills[SK_STAVES] = 1;

        break;



    case JOB_PALADIN:
        strcpy(you.class_name, "Paladin");
        you.religion = GOD_SHINING_ONE;
        you.piety = 28;
        you.hp = 14;
        you.hp_max = 14;
        you.magic_points = 0;
        you.max_magic_points = 0;

        you.inv_quantity[0] = 1;
        you.inv_class[0] = OBJ_WEAPONS;
        you.inv_type[0] = WPN_LONG_SWORD;
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
        last_paycheck();
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
        you.skills[ ( (coinflip()) ? SK_ARMOUR : SK_DODGING ) ]++;

        you.skills[SK_SHIELDS] = 2;
        you.skills[SK_LONG_SWORDS] = 3;
        you.skills[SK_INVOCATIONS] = 1;
        break;

    case JOB_ASSASSIN:
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
        last_paycheck();

        you.skills[SK_FIGHTING] = 2;
        you.skills[SK_SHORT_BLADES] = 2;

        you.skills[SK_DODGING] = 1;
        you.skills[SK_STEALTH] = 3;
        you.skills[SK_STABBING] = 2;

        you.skills[SK_THROWING] = 1;
        you.skills[SK_DARTS] = 1;
        you.skills[SK_CROSSBOWS] = 1;
        break;

    case JOB_BERSERKER:
        strcpy(you.class_name, "Berserker");
        you.religion = GOD_TROG;
        you.piety = 35;
        you.hp = 15;
        you.hp_max = 15;
        you.magic_points = 0;
        you.max_magic_points = 0;

// WEAPONS

        if (you.species == SP_OGRE)
        {
            you.inv_quantity[0] = 1;
            you.inv_class[0] = OBJ_WEAPONS;
            you.inv_type[0] = WPN_CLUB;
            you.inv_plus[0] = 50;
            you.inv_plus2[0] = 50;
            you.inv_dam[0] = 0;
            you.inv_colour[0] = LIGHTCYAN;
            you.equip[EQ_WEAPON] = 0;
        }
        else if (you.species == SP_TROLL)
        {
            you.equip[EQ_WEAPON] = -1;
// *BCR* Looks like Troll berserkers don't get weapons; should get unarmed?
/*
            you.inv_quantity[0] = 0;
            you.inv_class[0] = OBJ_WEAPONS;
            you.inv_type[0] = WPN_CLUB;
            you.inv_plus[0] = 50;
            you.inv_dam[0] = 0;
            you.inv_colour[0] = BROWN;
*/
        }
        else
        {
            you.inv_quantity[0] = 1;
            you.inv_class[0] = OBJ_WEAPONS;
            you.inv_type[0] = WPN_AXE;
            you.inv_plus[0] = 50;
            you.inv_plus2[0] = 50;
            you.inv_dam[0] = 0;
            you.inv_colour[0] = LIGHTCYAN;
            you.equip[EQ_WEAPON] = 0;
        }

// ARMOUR

        if (   you.species == SP_OGRE
            || you.species == SP_TROLL
            || (you.species >= SP_RED_DRACONIAN
                && you.species <= SP_UNK2_DRACONIAN))
        {
            you.inv_quantity[1] = 1;
            you.inv_class[1] = OBJ_ARMOUR;
            you.inv_type[1] = ARM_ANIMAL_SKIN;
            you.inv_plus[1] = 50;
            you.inv_dam[1] = 0;
            you.inv_colour[1] = BROWN;
            you.equip[EQ_BODY_ARMOUR] = 1;
        }
        else
        {
            you.inv_quantity[4] = 1;
            you.inv_class[4] = OBJ_ARMOUR;
            you.inv_type[4] = ARM_LEATHER_ARMOUR;
            you.inv_plus[4] = 50;
            you.inv_dam[4] = 0;
            you.inv_colour[4] = BROWN;
            you.equip[EQ_BODY_ARMOUR] = 4;
        }

// STATS

        you.strength += 7;
        you.dex += 4;
        you.intel -= 1;
        last_paycheck();
        you.skills[SK_FIGHTING] = 2;

// SKILLS
        if (you.species == SP_TROLL)
        {
// *BCR* Troll Berserkers get no weapon, so I gave them unarmed/dodging
            you.skills[SK_FIGHTING] += 3;
            you.skills[SK_DODGING] = 2;
            you.skills[SK_UNARMED_COMBAT] = 3;
        }
        else if (you.species == SP_OGRE)
        {
            you.skills[SK_FIGHTING] += 3;
            you.skills[SK_AXES] = 1;
            you.skills[SK_MACES_FLAILS] = 3;
        }
        else
        {
            you.skills[SK_AXES] = 3;
            you.skills[SK_MACES_FLAILS] = 1;
            you.skills[SK_ARMOUR] = 2;
            you.skills[SK_DODGING] = 2;
            you.skills[SK_THROWING] = 2;
        }

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

        you.inv_quantity[2] = 15 + random2(5) + random2(5) + random2(5)
                                                + random2(5) + random2(5);
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
        last_paycheck();

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
                you.skills[ ( (coinflip()) ? SK_STABBING : SK_SHIELDS ) ]++;
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

        // get a random lvl 1 attack spell; later overwritten for most classes
        switch (random2(8))
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
            you.inv_type[0] = WPN_QUARTERSTAFF;
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
        you.inv_colour[1] = random2(15) + 1;
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
        you.strength += 0; // no change
        you.dex += 4;
        you.intel += 6;
        you.equip[EQ_WEAPON] = 0;
        you.equip[EQ_BODY_ARMOUR] = 1;
        last_paycheck();
        you.inv_class[2] = OBJ_BOOKS;

        you.inv_type[2] = ( (coinflip()) ? BOOK_CONJURATIONS_I : BOOK_CONJURATIONS_II );

        you.inv_plus[2] = 127;
        if (you.char_class == JOB_SUMMONER)
        {
            you.inv_type[2] = BOOK_SUMMONINGS;

            you.inv_plus[2] = 127;
            you.skills[SK_SUMMONINGS] = 4;
// gets some darts - this class is difficult to start off with
            you.inv_quantity[3] = random2(5) + random2(5) + 7;
            you.inv_class[3] = OBJ_MISSILES;
            you.inv_type[3] = MI_DART;

            you.inv_plus[3] = 50;
            you.inv_dam[3] = 0;
            you.inv_colour[3] = LIGHTCYAN;
        }

        switch (you.char_class)
        {
        case JOB_CONJURER:
            you.skills[SK_CONJURATIONS] = 4;
            break;

        case JOB_ENCHANTER:
            you.inv_type[2] = BOOK_CHARMS;
            you.inv_plus[2] = SPBK_SIX_SLOTS;

            you.skills[SK_ENCHANTMENTS] = 4;

            you.inv_quantity[3] = 1;
            you.inv_class[3] = OBJ_WANDS;
            you.inv_dam[3] = 0;
            you.inv_colour[3] = random2(15) + 1;
            switch (random2(4))
            {
            case 0:
                you.inv_type[3] = WAND_SLOWING;
                you.inv_plus[3] = 7 + random2(5);
                break;
            case 1:
                you.inv_type[3] = WAND_PARALYSIS;
                you.inv_plus[3] = 5 + random2(4);
                break;
            case 2:
                you.inv_type[3] = WAND_INVISIBILITY;
                you.inv_plus[3] = 4 + random2(4);
                break;
            case 3:
                you.inv_type[3] = WAND_PARALYSIS;
                you.inv_plus[3] = 5 + random2(4);
                break;
            }
            you.inv_plus2[3] = 0;
            break;

        case JOB_FIRE_ELEMENTALIST:
            you.inv_type[2] = BOOK_FLAMES;
            you.inv_plus[2] = SPBK_SIX_SLOTS;

            you.skills[SK_CONJURATIONS] = 1;
            //you.skills [SK_ENCHANTMENTS] = 1;
            you.skills[SK_FIRE_MAGIC] = 3;

            you.spells[0] = SPELL_BURN;
            break;

        case JOB_ICE_ELEMENTALIST:
            you.inv_type[2] = BOOK_FROST;
            you.inv_plus[2] = SPBK_SIX_SLOTS;

            you.skills[SK_CONJURATIONS] = 1;
            // you.skills [SK_ENCHANTMENTS] = 1;
            you.skills[SK_ICE_MAGIC] = 3;

            you.spells[0] = SPELL_FREEZE;
            break;

        case JOB_AIR_ELEMENTALIST:
            you.inv_type[2] = BOOK_AIR;
            you.inv_plus[2] = SPBK_SIX_SLOTS;

            you.skills[SK_CONJURATIONS] = 1;
            // you.skills [SK_ENCHANTMENTS] = 1;
            you.skills[SK_AIR_MAGIC] = 3;

            you.spells[0] = SPELL_ARC;
            break;

        case JOB_EARTH_ELEMENTALIST:
            you.inv_type[2] = BOOK_GEOMANCY;
            you.inv_plus[2] = SPBK_SIX_SLOTS;

            you.skills[SK_CONJURATIONS] = 1;
            you.skills[SK_EARTH_MAGIC] = 3;

            you.spells[0] = SPELL_CRUSH;
            break;

        case JOB_VENOM_MAGE:
            you.inv_type[2] = BOOK_YOUNG_POISONERS;
            you.inv_plus[2] = SPBK_FIVE_SLOTS;

            you.skills[SK_POISON_MAGIC] = 4;

            you.spells[0] = SPELL_STING;
            break;

        case JOB_TRANSMUTER:
            you.inv_type[2] = BOOK_CHANGES;
            you.inv_plus[2] = SPBK_FIVE_SLOTS;

            you.skills[SK_TRANSMIGRATION] = 4;

            you.spells[0] = SPELL_DISRUPT;
            break;

        case JOB_WARPER:
            you.inv_type[2] = BOOK_SPATIAL_TRANSLOCATIONS;
            you.inv_plus[2] = SPBK_FOUR_SLOTS;

            you.skills[SK_TRANSLOCATIONS] = 4;
            break;
        }

        you.inv_quantity[2] = 1;
        you.inv_dam[2] = 0;
        you.inv_colour[2] = random2(15) + 1;
        if (you.char_class == JOB_FIRE_ELEMENTALIST)
            you.inv_colour[2] = RED;
        if (you.char_class == JOB_ICE_ELEMENTALIST)
            you.inv_colour[2] = LIGHTCYAN;
        if (you.char_class == JOB_VENOM_MAGE)
            you.inv_colour[2] = GREEN;

        you.skills[SK_DODGING] = 1;
        you.skills[SK_STEALTH] = 1;
        you.skills[ ( (coinflip()) ? SK_DODGING : SK_STEALTH ) ]++;
        you.skills[SK_SPELLCASTING] = 1;
        you.skills[SK_SHORT_BLADES] = 1;
        you.skills[SK_STAVES] = 1;
        break;

    case JOB_CRUSADER:
        strcpy(you.class_name, "Crusader");
        //you.piety = 75;
        you.hp = 13;
        you.hp_max = 13;
        you.magic_points = 1;
        you.max_magic_points = 1;
        //you.f_abil = 9;
        //you.mag_abil = 6;
        //you.thr_abil = 4;
        you.inv_quantity[0] = 1;
        you.inv_class[0] = OBJ_WEAPONS;
        you.inv_type[0] = WPN_SHORT_SWORD;

        //if (you.species == SP_OGRE_MAGE) you.inv_type [0] = WPN_GLAIVE;

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
        you.inv_colour[1] = 1 + random2(15);

        you.inv_class[2] = OBJ_BOOKS;
        you.inv_type[2] = BOOK_WAR_CHANTS;
        you.inv_quantity[2] = 1;
        you.inv_plus[2] = SPBK_SIX_SLOTS;
        you.inv_dam[2] = 0;
        you.inv_colour[2] = 1 + random2(15);

        // you.AC = 1;
        // you.evasion = 10;
        you.strength += 4;
        you.dex += 3;
        you.intel += 3;
        you.equip[EQ_WEAPON] = 0;
        you.equip[EQ_BODY_ARMOUR] = 1;
        last_paycheck();

        you.skills[SK_FIGHTING] = 3;
        you.skills[SK_ARMOUR] = 1;
        you.skills[SK_DODGING] = 1;
        you.skills[SK_STEALTH] = 1;
        you.skills[SK_SPELLCASTING] = 2;
        you.skills[SK_ENCHANTMENTS] = 2;
        break;


    case JOB_DEATH_KNIGHT:
        strcpy(you.class_name, "Death Knight");
        // you.piety = 75;
        you.hp = 13;
        you.hp_max = 13;
        you.magic_points = 1;
        you.max_magic_points = 1;
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
        you.inv_plus[2] = SPBK_SIX_SLOTS;
        you.inv_dam[2] = 0;
        you.inv_colour[2] = DARKGREY;

        // you.AC = 1;
        // you.evasion = 10;
        you.strength += 4;
        you.dex += 3;
        you.intel += 3;
        you.equip[EQ_WEAPON] = 0;
        you.equip[EQ_BODY_ARMOUR] = 1;
        last_paycheck();

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
        //if (you.skills [SK_DODGING] == 2) you.evasion ++;

        you.skills[SK_STEALTH] = 1;
        //you.skills [SK_SHORT_BLADES] = 2;
        you.skills[SK_STABBING] = 1;


        break;

    case JOB_CHAOS_KNIGHT:
        strcpy(you.class_name, "Knight of Chaos");
        //you.piety = 75;
        you.hp = 13;
        you.hp_max = 13;
        you.magic_points = 1;
        you.max_magic_points = 1;
        you.inv_quantity[0] = 1;
        you.inv_class[0] = OBJ_WEAPONS;
        you.inv_type[0] = WPN_SHORT_SWORD;
        you.inv_plus[0] = 50 + random2(3);
        you.inv_plus2[0] = 50 + random2(3);
        you.inv_dam[0] = 0;
        if ( one_chance_in(5) )
            you.inv_dam[0] = 30;
        if ( one_chance_in(5) )
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
        last_paycheck();

        you.skills[SK_FIGHTING] = 3;
        you.skills[SK_ARMOUR] = 1;
        you.skills[SK_DODGING] = 1;
        you.skills[ ( (coinflip()) ? SK_ARMOUR : SK_DODGING ) ]++;

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

        you.piety = 25;         // irrelevant for Xom, of course

        break;



    case JOB_HEALER:
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
        you.inv_quantity[2] = ( (coinflip()) ? 3 : 2 );
        you.inv_plus[2] = 0;
        you.inv_dam[2] = 0;
        you.inv_colour[2] = random2(15) + 1;    // hmmm...

        you.inv_class[3] = OBJ_POTIONS;
        you.inv_type[3] = POT_HEAL_WOUNDS;
        you.inv_quantity[3] = ( (coinflip()) ? 3 : 2 );
        you.inv_plus[3] = 0;
        you.inv_dam[3] = 0;
        you.inv_colour[3] = random2(15) + 1;    // hmmm...

        you.strength += 4;
        you.dex += 2;
        you.intel += 4;

        you.equip[EQ_WEAPON] = 0;
        you.equip[EQ_BODY_ARMOUR] = 1;
        last_paycheck();

        you.skills[SK_FIGHTING] = 2;
        you.skills[SK_DODGING] = 1;
        you.skills[SK_SHIELDS] = 1;
        you.skills[SK_THROWING] = 2;
        you.skills[SK_STAVES] = 3;

        you.skills[SK_INVOCATIONS] = 2;

        you.religion = GOD_ELYVILON;

        break;


    case JOB_REAVER:
        strcpy(you.class_name, "Reaver");
        you.hp = 13;
        you.hp_max = 13;
        you.magic_points = 1;
        you.max_magic_points = 1;
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
        you.inv_type[2] = ( (coinflip()) ? BOOK_CONJURATIONS_I : BOOK_CONJURATIONS_II );
        you.inv_quantity[2] = 1;
        you.inv_plus[2] = 127;
        you.inv_dam[2] = 0;
        you.inv_colour[2] = RED;

        you.strength += 4;
        you.dex += 2;
        you.intel += 4;
        you.equip[EQ_WEAPON] = 0;
        you.equip[EQ_BODY_ARMOUR] = 1;
        last_paycheck();

        you.skills[SK_FIGHTING] = 2;
        you.skills[SK_ARMOUR] = 1;
        you.skills[SK_DODGING] = 1;

        you.skills[SK_SPELLCASTING] = 1;
        you.skills[SK_CONJURATIONS] = 2;
        you.spells[0] = SPELL_MAGIC_DART;
        break;

    case JOB_STALKER:
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
        last_paycheck();

        you.spells[0] = SPELL_STING;


        you.skills[SK_FIGHTING] = 1;
        you.skills[SK_SHORT_BLADES] = 1;

        you.skills[SK_POISON_MAGIC] = 1;

        you.skills[SK_DODGING] = 1;
        you.skills[SK_STEALTH] = 2;
        you.skills[SK_STABBING] = 2;
        you.skills[SK_DODGING + random2(3)]++;

// if (you.skills [SK_DODGING] == 2) you.evasion ++;

        you.skills[SK_THROWING] = 1;
        you.skills[SK_DARTS] = 1;

        break;

    case JOB_MONK:

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
        last_paycheck();

        you.skills[SK_FIGHTING] = 3;
        you.skills[SK_UNARMED_COMBAT] = 4;

        you.skills[SK_DODGING] = 3;
        you.skills[SK_STEALTH] = 2;

        break;
    }

/*you.mag_abil = 0; */


/*you.res_magic = 3 + you.skills [SK_ENCHANTMENTS] * 2; */

    char points_left = 8;

    if (you.species == SP_DEMIGOD || you.species == SP_DEMONSPAWN)
        points_left += 7;

//for (i = 0; i < 8; i ++)
    do
    {
        switch (random2(3))
        {
        case 0:
            if ( you.strength > 17 && coinflip() )
                continue;
            you.strength++;
            break;
        case 1:
            if ( you.dex > 17 && coinflip() )
                continue;
            you.dex++;
            break;
        case 2:
            if ( you.intel > 17 && coinflip() )
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

    you.is_undead = US_ALIVE;

    switch (you.species)
    {
    case SP_ELF:
        you.hp_max--;
        you.base_hp2--;
        break;

    case SP_HIGH_ELF:
        you.hp_max--;
        you.base_hp2--;
        break;

    case SP_GREY_ELF:
        you.hp_max--;
        you.base_hp2--;
        you.max_magic_points++;
        you.base_magic_points2++;
        break;

    case SP_DEEP_ELF:
        you.hp_max -= 2;
        you.base_hp2 -= 2;
        you.max_magic_points++;
        you.base_magic_points2++;
        break;

    case SP_HILL_DWARF:
        you.hp_max++;
        you.base_hp2++;
        break;

    case SP_MOUNTAIN_DWARF:
        you.hp_max++;
        you.base_hp2++;
        break;

    case SP_HALFLING:
        you.hp_max -= 2;
        you.base_hp2 -= 2;
        //you.hunger_inc--;
        //you.evasion++;
        break;

    case SP_HILL_ORC:
        you.hp_max++;
        you.base_hp2++;
        break;

    case SP_KOBOLD:
        you.hp_max -= 2;
        you.base_hp2 -= 2;
        //you.evasion++;
        break;

    case SP_MUMMY:
        you.hp_max++;
        you.base_hp2++;
        //you.res_cold++;
        //you.res_fire--;
        //you.prot_life++;
        //you.res_poison++;
        you.is_undead = US_UNDEAD;
        //you.sust_abil = 1;
        //you.evasion--;
        break;

    case SP_NAGA:
        you.hp_max += 2;
        you.base_hp2 += 2;
        //you.evasion -= 3;
        //you.AC += 2;
        //you.res_poison++;
        //you.see_invis++;
        you.attribute[ATTR_WALK_SLOWLY] = 1;
        break;

    case SP_GNOME:
        //you.AC++;
        you.hp_max -= 2;
        you.base_hp2 -= 2;
        break;

    case SP_OGRE:
        you.hp_max += 3;
        you.base_hp2 += 3;
        //you.hunger_inc++;
        break;

    case SP_TROLL:
        you.hp_max += 3;
        you.base_hp2 += 3;
        //you.hunger_inc += 6;
        break;

    case SP_OGRE_MAGE:
        you.hp_max += 2;
        you.base_hp2 += 2;
        //you.hunger_inc++;
        break;

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
        you.hp_max += 1;
        you.base_hp2 += 1;
        break;

    case SP_CENTAUR:
        you.hp_max += 3;
        you.base_hp2 += 3;
        // you.hunger_inc += 2;
        break;

    case SP_DEMIGOD:
        you.hp_max += 3;
        you.base_hp2 += 3;
        // you.hunger_inc += 1;
        you.max_magic_points++;
        you.base_magic_points2++;
        break;

    case SP_SPRIGGAN:
        you.hp_max -= 2;
        you.base_hp2 -= 2;
        // you.hunger_inc -= 1;
        break;

    case SP_MINOTAUR:
        you.hp_max += 2;
        you.base_hp2 += 2;
        break;

    case SP_GHOUL:
        you.hp_max += 2;
        you.base_hp2 += 2;
        you.is_undead = US_HUNGRY_DEAD;
        break;

    case SP_KENKU:
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

    if (!you.is_undead)     // != US_UNDEAD)

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

    you.item_description[1][POT_PORRIDGE] = 178;
    you.item_description[1][POT_WATER] = 0;

    for (i = 0; i < 4; i++)
    {
        for (j = 0; j < 50; j++)
        {

            do
            {
                passout = 1;

                switch (i)
                {
                    // This bit is horrendous:     // I cleaned it up with the conditional operator 15jan2000 {dlb}

                case 0: // wands
                    you.item_description[i][j] = (coinflip()) ? random2(12) : random2(12) + random2(12) * 16;
                                                                          //: (random2(12) * 16) + random2(15);
                    break;

                case 1: // potions
                    you.item_description[i][j] = (coinflip()) ? random2(14) : (random2(14) * 14) + random2(14);
                    break;

                case 2: // scrolls
                    you.item_description[i][j] = random2(151);
                    you.item_description[4][j] = random2(151);
                    break;

                case 3: // rings
                    you.item_description[i][j] = (coinflip()) ? random2(13) : random2(13) + (random2(13) * 13);
                    break;

                }               // end switch

                for (int p = 0; p < 30; p++)        // don't have p < j because some are preassigned
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

    if (you.spells[0] != SPELL_NO_SPELL)
        you.spell_no = 1;
    else
        you.spell_no = 0;

    if ( you.char_class == JOB_PRIEST
          || you.char_class == JOB_PALADIN )
        set_id(OBJ_POTIONS, POT_HEALING, 1);

    if ( you.char_class == JOB_ASSASSIN
          || you.char_class == JOB_STALKER
          || you.char_class == JOB_VENOM_MAGE )
        set_id(OBJ_POTIONS, POT_POISON, 1);

    // Now handled by a function in player.cc
    // you.spell_levels = you.skills[SK_SPELLCASTING] * 2 - (you.spells[0] != SPELL_NO_SPELL) - (you.spells[1] != SPELL_NO_SPELL);


    // tmpfile purging removed in favour of marking
    for (int lvl= 0; lvl < MAX_LEVELS; lvl++)
        for (int dng= 0; dng < MAX_BRANCHES; dng++)
            tmp_file_pairs[ lvl ][ dng ] = false;

// This is the temporary file purging code. Unfortunately it doesn't work.
//  Use the other block (also in #if 0 right now) if you want to resume
//  file purging.
#if 0
    char del_file[kFileNameSize];
    char glorpstr[kFileNameSize];

#ifdef SAVE_DIR_PATH
    sprintf(glorpstr, SAVE_DIR_PATH "%s%d", you.your_name, getuid());
#else
    strncpy(glorpstr, you.your_name, kFileNameLen);

    glorpstr [strlen(you.your_name)] = '\0';
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
                if (!unlink(del_file)) cprintf("\n\rFailed to delete: ");
                        else cprintf("\n\rDeleted: ");
                cprintf(del_file);
                if (!getch()) getch();
            }
        }
    }
#endif


#if 0
char del_file [kFileNameSize];


char glorpstr [kFileNameSize];
strncpy(glorpstr, you.your_name, 6);

// This is broken. Length is not valid yet! We have to check if we got a
// trailing NULL; if not, write one:
if (strlen(you.your_name) > 5)    /* is name 6 chars or more? */
        glorpstr[6] = (char) NULL;   /* if so, char 7 should be NULL */

strncpy(glorpstr, you.your_name, 6);

// This is broken. Length is not valid yet! We have to check if we got a
// trailing NULL; if not, write one:
if (strlen(you.your_name) > 5)    /* is name 6 chars or more? */
        glorpstr[6] = (char) NULL;   /* if so, char 7 should be NULL */

int fi = 0;
int fi2 = 0;
char st_prn [6];

for (fi2 = 0; fi2 < 30; fi2 ++)
{
 for (fi = 0; fi < 50; fi ++)
 {
  strcpy(del_file, glorpstr);
  strcat(del_file, ".");
  if (fi < 10) strcat(del_file, "0");
  itoa(fi, st_prn, 10);
  strcat(del_file, st_prn);
  st_prn [0] = fi2 + 97;
  st_prn [1] = 0;
  strcat(del_file, st_prn);
  strcat(del_file, "\0");
  handle = open(del_file, S_IWRITE, S_IREAD);

  if (handle != -1)
  {
        close(handle);
        unlink(del_file);
  } else close(handle);
 }
}
#endif

    for (i = 0; i < 30; i++)
    {
        you.branch_stairs[i] = 100;
    }
/* Places the staircases to the branch levels: */

    you.branch_stairs[STAIRS_ECUMENICAL_TEMPLE] =
         3 + random2(4);                                                        // avg:  4.5

    you.branch_stairs[STAIRS_ORCISH_MINES] =
         5 + random2(6);                                                        // avg:  7.5

    you.branch_stairs[STAIRS_ELVEN_HALLS] =
         you.branch_stairs[STAIRS_ORCISH_MINES] + ( (coinflip()) ? 4 : 3 );     // avg: 11.0

    you.branch_stairs[STAIRS_HIVE] =
         10 + random2(6);                                                       // avg: 12.5

    you.branch_stairs[STAIRS_LAIR] =
         7 + random2(6);                                                        // avg:  9.5

    you.branch_stairs[STAIRS_SLIME_PITS] =
         you.branch_stairs[STAIRS_LAIR] + 3 + random2(4);                       // avg: 14.0

    you.branch_stairs[STAIRS_SWAMP] =
         you.branch_stairs[STAIRS_LAIR] + 2 + random2(6);                       // avg: 14.0

    you.branch_stairs[STAIRS_SNAKE_PIT] =
         you.branch_stairs[STAIRS_LAIR] + ( (coinflip()) ? 7 : 6 );             // avg: 16.0

    you.branch_stairs[STAIRS_VAULTS] =
         13 + random2(6);                                                       // avg: 15.5

    you.branch_stairs[STAIRS_CRYPT] =
         you.branch_stairs[STAIRS_VAULTS] + 2 + random2(3);                     // avg: 18.5

    you.branch_stairs[STAIRS_TOMB] =
         you.branch_stairs[STAIRS_CRYPT] + ( (coinflip()) ? 3 : 2 );            // avg: 20.0

    you.branch_stairs[STAIRS_HALL_OF_BLADES] =
         you.branch_stairs[STAIRS_CRYPT] + 4;                                   // avg: 22.5

    you.branch_stairs[STAIRS_HALL_OF_ZOT] = 26;                                 // always 26

    return 1;

}                               // end of new_game()




bool class_allowed(unsigned char speci, char char_class)
{
    switch (char_class)
    {
    case JOB_FIGHTER:

        switch (speci)
        {
        case SP_OGRE_MAGE:
        case SP_SPRIGGAN:
            return false;
        }
        return true;

    case JOB_WIZARD:
        switch (speci)
        {
        case SP_HILL_ORC:
        case SP_HALFLING:
        case SP_HILL_DWARF:
        case SP_KOBOLD:
        case SP_MUMMY:
        case SP_GNOME:
        case SP_OGRE:
        case SP_TROLL:
        case SP_SPRIGGAN:
        case SP_MINOTAUR:
        case SP_GHOUL:
        case SP_KENKU:
            return false;
        }
        return true;

    case JOB_PRIEST:
        switch (speci)
        {
        case SP_HALFLING:
        case SP_KOBOLD:
        case SP_MUMMY:
        case SP_NAGA:
        case SP_GNOME:
        case SP_OGRE:
        case SP_TROLL:
        case SP_OGRE_MAGE:
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
        case SP_SPRIGGAN:
        case SP_MINOTAUR:
        case SP_DEMONSPAWN:
        case SP_GHOUL:
        case SP_KENKU:
            return false;
        }
        return true;

    case JOB_THIEF:
        switch (speci)
        {
        case SP_MUMMY:
        case SP_OGRE:
        case SP_TROLL:
        case SP_OGRE_MAGE:
        case SP_CENTAUR:
        case SP_SPRIGGAN:
        case SP_MINOTAUR:
        case SP_GHOUL:
        case SP_KENKU:
            return false;
        }
        return true;

    case JOB_GLADIATOR:
        switch (speci)
        {
        case SP_ELF:
        case SP_HIGH_ELF:
        case SP_GREY_ELF:
        case SP_DEEP_ELF:
        case SP_SLUDGE_ELF:
        case SP_HALFLING:
        case SP_KOBOLD:
        case SP_MUMMY:
        case SP_NAGA:
        case SP_GNOME:
        case SP_OGRE:
        case SP_TROLL:
        case SP_OGRE_MAGE:
        case SP_SPRIGGAN:
        case SP_GHOUL:
            return false;
        }
        return true;

    case JOB_NECROMANCER:
        switch (speci)
        {
        case SP_ELF:
        case SP_HIGH_ELF:
        case SP_GREY_ELF:
        case SP_HILL_DWARF:
        case SP_MOUNTAIN_DWARF:
        case SP_HALFLING:
        case SP_GNOME:
        case SP_OGRE:
        case SP_TROLL:
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
        case SP_SPRIGGAN:
        case SP_MINOTAUR:
        case SP_GHOUL:
            return false;
        }
        return true;

    case JOB_PALADIN:
        switch (speci)
        {
        case SP_ELF:
        case SP_HIGH_ELF:
        case SP_GREY_ELF:
        case SP_DEEP_ELF:
        case SP_SLUDGE_ELF:
        case SP_HILL_DWARF:
        case SP_HALFLING:
        case SP_HILL_ORC:
        case SP_KOBOLD:
        case SP_MUMMY:
        case SP_NAGA:
        case SP_GNOME:
        case SP_OGRE:
        case SP_TROLL:
        case SP_OGRE_MAGE:
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
        case SP_SPRIGGAN:
        case SP_MINOTAUR:
        case SP_DEMONSPAWN:
        case SP_GHOUL:
        case SP_KENKU:
            return false;
        }
        return true;

    case JOB_ASSASSIN:
        switch (speci)
        {
        case SP_HILL_DWARF:
        case SP_MOUNTAIN_DWARF:
        case SP_GNOME:
        case SP_OGRE:
        case SP_TROLL:
        case SP_OGRE_MAGE:
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
        case SP_MINOTAUR:
        case SP_GHOUL:
            return false;
        }
        return true;

    case JOB_BERSERKER:
        switch (speci)
        {
        case SP_ELF:
        case SP_HIGH_ELF:
        case SP_GREY_ELF:
        case SP_DEEP_ELF:
        case SP_SLUDGE_ELF:
        case SP_MOUNTAIN_DWARF:
        case SP_HALFLING:
        case SP_KOBOLD:
        case SP_MUMMY:
        case SP_NAGA:
        case SP_GNOME:
        case SP_OGRE_MAGE:
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
        case SP_DEMIGOD:
        case SP_SPRIGGAN:
        case SP_GHOUL:
        case SP_KENKU:
            return false;
        }
        return true;

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
            return true;
        }
        return false;

    case JOB_CONJURER:
        switch (speci)
        {
        case SP_KOBOLD:
        case SP_MUMMY:
        case SP_HALFLING:
        case SP_GNOME:
        case SP_OGRE:
        case SP_TROLL:
        case SP_CENTAUR:
        case SP_SPRIGGAN:
        case SP_MINOTAUR:
        case SP_GHOUL:
            return false;
        }
        return true;

    case JOB_ENCHANTER:
        switch (speci)
        {
        case SP_HILL_ORC:
        case SP_KOBOLD:
        case SP_MUMMY:
        case SP_GNOME:
        case SP_OGRE:
        case SP_TROLL:
        case SP_MINOTAUR:
        case SP_GHOUL:
        case SP_KENKU:
            return false;
        }
        return true;

    case JOB_FIRE_ELEMENTALIST:
        switch (speci)
        {
        case SP_GREY_ELF:
        case SP_HALFLING:
        case SP_KOBOLD:
        case SP_MUMMY:
        case SP_NAGA:
        case SP_GNOME:
        case SP_OGRE:
        case SP_TROLL:
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
        case SP_SPRIGGAN:
        case SP_MINOTAUR:
        case SP_GHOUL:
            return false;
        }
        return true;

    case JOB_ICE_ELEMENTALIST:
        switch (speci)
        {
        case SP_GREY_ELF:
        case SP_HILL_DWARF:
        case SP_MOUNTAIN_DWARF:
        case SP_HALFLING:
        case SP_HILL_ORC:
        case SP_KOBOLD:
        case SP_MUMMY:
        case SP_NAGA:
        case SP_GNOME:
        case SP_OGRE:
        case SP_TROLL:
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
        case SP_SPRIGGAN:
        case SP_MINOTAUR:
        case SP_GHOUL:
        case SP_KENKU:
            return false;
        }
        return true;

    case JOB_SUMMONER:
        switch (speci)
        {
        case SP_HALFLING:
        case SP_MOUNTAIN_DWARF:
        case SP_HILL_DWARF:
        case SP_MUMMY:
        case SP_GNOME:
        case SP_OGRE:
        case SP_TROLL:
        case SP_CENTAUR:
        case SP_SPRIGGAN:
        case SP_MINOTAUR:
        case SP_GHOUL:
            return false;
        }
        return true;

    case JOB_AIR_ELEMENTALIST:
        switch (speci)
        {
        case SP_HILL_DWARF:
        case SP_MOUNTAIN_DWARF:
        case SP_HALFLING:
        case SP_HILL_ORC:
        case SP_KOBOLD:
        case SP_MUMMY:
        case SP_NAGA:
        case SP_GNOME:
        case SP_OGRE:
        case SP_TROLL:
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
        case SP_SPRIGGAN:
        case SP_MINOTAUR:
        case SP_GHOUL:
            return false;
        }
        return true;

    case JOB_EARTH_ELEMENTALIST:
        switch (speci)
        {
        case SP_ELF:
        case SP_HIGH_ELF:
        case SP_GREY_ELF:
        case SP_HALFLING:
        case SP_KOBOLD:
        case SP_MUMMY:
        case SP_NAGA:
        case SP_OGRE:
        case SP_TROLL:
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
        case SP_SPRIGGAN:
        case SP_MINOTAUR:
        case SP_GHOUL:
        case SP_KENKU:
            return false;
        }
        return true;

    case JOB_CRUSADER:
        switch (speci)
        {
        case SP_HILL_DWARF:
        case SP_MOUNTAIN_DWARF:
        case SP_HALFLING:
        case SP_KOBOLD:
        case SP_MUMMY:
        case SP_NAGA:
        case SP_GNOME:
        case SP_OGRE:
        case SP_TROLL:
        case SP_SPRIGGAN:
        case SP_MINOTAUR:
        case SP_GHOUL:
        case SP_KENKU:
            return false;
        }
        return true;

    case JOB_DEATH_KNIGHT:
        switch (speci)
        {
        case SP_ELF:
        case SP_HIGH_ELF:
        case SP_GREY_ELF:
        case SP_HILL_DWARF:
        case SP_MOUNTAIN_DWARF:
        case SP_HALFLING:
        case SP_KOBOLD:
        case SP_GNOME:
        case SP_OGRE:
        case SP_TROLL:
        case SP_OGRE_MAGE:
        case SP_SPRIGGAN:
        case SP_MINOTAUR:
        case SP_GHOUL:
            return false;
        }
        return true;

    case JOB_VENOM_MAGE:
        switch (speci)
        {
        case SP_ELF:
        case SP_HIGH_ELF:
        case SP_GREY_ELF:
        case SP_HILL_DWARF:
        case SP_MOUNTAIN_DWARF:
        case SP_HALFLING:
        case SP_MUMMY:
        case SP_GNOME:
        case SP_OGRE:
        case SP_TROLL:
        case SP_CENTAUR:
        case SP_MINOTAUR:
        case SP_GHOUL:
            return false;
        }
        return true;

    case JOB_CHAOS_KNIGHT:
        switch (speci)
        {
        case SP_GREY_ELF:
        case SP_HILL_DWARF:
        case SP_MOUNTAIN_DWARF:
        case SP_HALFLING:
        case SP_MUMMY:
        case SP_GNOME:
        case SP_OGRE:
        case SP_TROLL:
        case SP_OGRE_MAGE:
        case SP_DEMIGOD:
        case SP_SPRIGGAN:
        case SP_GHOUL:
        case SP_KENKU:
            return false;
        }
        return true;

    case JOB_TRANSMUTER:
        switch (speci)
        {
        case SP_HILL_ORC:
        case SP_HALFLING:
        case SP_HILL_DWARF:
        case SP_KOBOLD:
        case SP_MUMMY:
        case SP_GNOME:
        case SP_OGRE:
        case SP_TROLL:
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
        case SP_MINOTAUR:
        case SP_GHOUL:
        case SP_KENKU:
            return false;
        }
        return true;

    case JOB_HEALER:
        switch (speci)
        {
        case SP_HALFLING:
        case SP_KOBOLD:
        case SP_MUMMY:
        case SP_NAGA:
        case SP_GNOME:
        case SP_OGRE:
        case SP_TROLL:
        case SP_OGRE_MAGE:
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
        case SP_DEMIGOD:
        case SP_SPRIGGAN:
        case SP_MINOTAUR:
        case SP_DEMONSPAWN:
        case SP_GHOUL:
        case SP_KENKU:
            return false;
        }
        return true;

    case JOB_REAVER:
        switch (speci)
        {
        case SP_GREY_ELF:
        case SP_HILL_DWARF:
        case SP_MOUNTAIN_DWARF:
        case SP_HALFLING:
        case SP_MUMMY:
        case SP_GNOME:
        case SP_OGRE:
        case SP_TROLL:
        case SP_OGRE_MAGE:
        case SP_SPRIGGAN:
        case SP_MINOTAUR:
        case SP_GHOUL:
            return false;
        }
        return true;

    case JOB_STALKER:
        switch (speci)
        {
        case SP_HILL_DWARF:
        case SP_MOUNTAIN_DWARF:
        case SP_HALFLING:
        case SP_MUMMY:
        case SP_GNOME:
        case SP_OGRE:
        case SP_TROLL:
        case SP_OGRE_MAGE:
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
        case SP_MINOTAUR:
        case SP_GHOUL:
            return false;
        }
        return true;

    case JOB_MONK:
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
        case SP_SPRIGGAN:
        case SP_GHOUL:
            return false;
        }
        return true;

    case JOB_WARPER:
        switch (speci)
        {
        case SP_HILL_ORC:
        case SP_HILL_DWARF:
        case SP_KOBOLD:
        case SP_MUMMY:
        case SP_GNOME:
        case SP_OGRE:
        case SP_TROLL:
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
        case SP_MINOTAUR:
        case SP_GHOUL:
        case SP_KENKU:
            return false;
        }
        return true;


    default:
        return false;

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
        // intentional fall through

    default:
        goto getkey;
    }
}




void init_player(void)
{
    int i = 0;     // loop variable

    you.deaths_door = 0;
    you.special_wield = 0;
    you.berserker = 0;
    you.elapsed_time = 0;
    you.synch_time = 0;
    you.berserk_penalty = 0;
    you.exhausted = 0;

    you.disease = 0;
    you.invis = 0;
    you.conf = 0;
    you.paralysis = 0;
    you.slow = 0;
    you.haste = 0;
    you.might = 0;
    you.levitation = 0;
    you.poison = 0;
    you.rotting = 0;
    you.shock_shield = 0;

    you.hp = 0;
    you.hp_max = 0;
    you.magic_points = 0;
    you.max_magic_points = 0;
    you.strength = 0;
    you.max_strength = 0;
    you.intel = 0;
    you.max_intel = 0;
    you.dex = 0;
    you.max_dex = 0;
    you.experience = 0;
    you.experience_level = 1;
    you.max_level = 1;
    you.char_class = JOB_FIGHTER;
    you.hunger_state = HS_SATIATED;
    you.gold = 0;
    you.speed = 10;                         // 0.75;
    you.burden = 0;                         // total weight of items carried
    you.burden_state = BS_UNENCUMBERED;
    you.num_inv_items = 0;                  // number of items carried
    you.spell_no = 0;
    //you.spell_levels = 0;

    you.your_level = 0;
    you.level_type = LEVEL_DUNGEON;
    you.where_are_you = BRANCH_MAIN_DUNGEON;
    you.char_direction = 0;                 // 0 = going down
                                            // 1 = going up!
    you.prev_targ = MHITNOT;
    you.pet_target = MHITNOT;

    you.x_pos = 0;
    you.y_pos = 0;
    you.delay_t = 0;
    you.delay_doing = 0;
    you.running = 0;
    you.run_x = 0;
    you.run_y = 0;

    you.religion = GOD_NO_GOD;
    you.piety = 0;

    for (i = 0; i < 20; i++)
      you.gods[i] = 0;

    strcpy(ghost.name, "");      //

    for (i = 0; i < 20; i++)
      ghost.values[i] = 0;

    for (i = 0; i < 52; i++)
      you.inv_quantity[i] = 0;

    for (i = EQ_WEAPON; i < NUM_EQUIP; i++)
      you.equip[i] = -1;

    for (i = 0; i < 25; i++)
      you.spells[i] = SPELL_NO_SPELL;

    for (i = 0; i < 100; i++)    // clear out mutations
      you.mutation[i] = 0;

    for (i = 0; i < 100; i++)    // clear out demon powers
      you.demon_pow[i] = 0;

    for (i = 0; i < 50; i++)     // clear out spellbooks "had"
      you.had_item[i] = 0;

    for (i = 0; i < 50; i++)
      you.unique_items[i] = 0;

    for (i = 0; i < NO_UNRANDARTS; i++)
      set_unrandart_exist(i, 0);

    for (i = 0; i < 50; i++)     // clear out skills-related fields
      {
        you.skills[i] = 0;
        you.skill_points[i] = 0;
        you.practise_skill[i] = 1;
      }


}




void last_paycheck( void )
{
    switch ( you.char_class )
    {
        case JOB_MONK:
          you.gold = 0;
          break;
        case JOB_HEALER:
          you.gold = random2avg(100,2);     // normalized with random2avg 23jan2000 {dlb}
          break;
        case JOB_THIEF:
          you.gold = (random2(10) * 6) + (random2(10) * 4);     // BCR upped the amount
          break;
        default:
          you.gold = random2(10);
          break;
    }

}




// requires stuff::modify_stats() and works because
// stats zeroed out by newgame::init_player()  {dlb}

void species_stat_init( unsigned char which_species )
{
  int strength_base = 0;
  int intellect_base = 0;
  int dexterity_base = 0;

  switch ( which_species )          // strength
  {
    case SP_TROLL:
      strength_base = 13;

    case SP_OGRE:
      strength_base = 12;

    case SP_MINOTAUR:
      strength_base = 10;

    case SP_HILL_DWARF:
    case SP_MOUNTAIN_DWARF:
    case SP_HILL_ORC:
    case SP_OGRE_MAGE:
      strength_base = 9;

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
      strength_base = 9;

    case SP_NAGA:
    case SP_CENTAUR:
      strength_base = 8;

    case SP_MUMMY:
    case SP_DEMIGOD:
    case SP_GHOUL:
      strength_base = 7;

    case SP_ELF:
    case SP_HIGH_ELF:
    case SP_SLUDGE_ELF:
    case SP_GNOME:
    case SP_KENKU:
      strength_base = 5;

    case SP_GREY_ELF:
    case SP_DEEP_ELF:
    case SP_HALFLING:
    case SP_KOBOLD:
    case SP_DEMONSPAWN:
      strength_base = 4;

    case SP_SPRIGGAN:
      strength_base = 3;

    default:                          // SP_HUMAN, too {dlb}
      strength_base = 6;
  }


  switch ( which_species )          // intellect
  {
    case SP_DEEP_ELF:
      intellect_base = 10;

    case SP_HIGH_ELF:
    case SP_GREY_ELF:
      intellect_base = 9;

    case SP_ELF:
      intellect_base = 8;

    case SP_SLUDGE_ELF:
    case SP_OGRE_MAGE:
    case SP_DEMIGOD:
      intellect_base = 7;

    case SP_HALFLING:
    case SP_NAGA:
    case SP_GNOME:
    case SP_SPRIGGAN:
    case SP_KENKU:
      intellect_base = 6;

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
      intellect_base = 6;

    case SP_MOUNTAIN_DWARF:
    case SP_KOBOLD:
    case SP_CENTAUR:
    case SP_DEMONSPAWN:
      intellect_base = 4;

    case SP_HILL_DWARF:
    case SP_HILL_ORC:
    case SP_MUMMY:
    case SP_OGRE:
    case SP_TROLL:
    case SP_MINOTAUR:
      intellect_base = 3;

    case SP_GHOUL:
      intellect_base = 1;

    default:                          // SP_HUMAN, too {dlb}
      intellect_base = 6;
  }


  switch ( which_species )          // dexterity
  {
    case SP_HALFLING:
      dexterity_base = 9;

    case SP_ELF:
    case SP_HIGH_ELF:
    case SP_GREY_ELF:
    case SP_SLUDGE_ELF:
    case SP_SPRIGGAN:
      dexterity_base = 8;

    case SP_DEEP_ELF:
    case SP_GNOME:
    case SP_DEMIGOD:
    case SP_KENKU:
      dexterity_base = 7;

    case SP_KOBOLD:
      dexterity_base = 6;

    case SP_MOUNTAIN_DWARF:
    case SP_CENTAUR:
    case SP_MINOTAUR:
      dexterity_base = 5;

    case SP_HILL_DWARF:
    case SP_HILL_ORC:
    case SP_NAGA:
    case SP_DEMONSPAWN:
      dexterity_base = 4;

    case SP_MUMMY:
    case SP_OGRE_MAGE:
      dexterity_base = 3;

    case SP_GHOUL:
      dexterity_base = 2;

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
      dexterity_base = 2;

    case SP_OGRE:
      dexterity_base = 1;

    case SP_TROLL:
      dexterity_base = 0;

    default:                          // SP_HUMAN, too {dlb}
      dexterity_base = 6;
  }

  modify_stats(strength_base, intellect_base, dexterity_base);

  return;

}




void jobs_stat_init( int which_job )
{
  switch ( which_job )
  {}

}

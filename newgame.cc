/*
 *  File:       newgame.cc
 *  Summary:    Functions used when starting a new game.
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 *     <16>      19-Jun-2000    GDL   changed handle to FILE *
 *     <15>      06-Mar-2000    bwr   changes to berserer, paladin, enchanter
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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#ifdef DOS
#include <conio.h>
#include <file.h>
#endif

#ifdef LINUX
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
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

#include "files.h"
#include "itemname.h"
#include "player.h"
#include "randart.h"
#include "skills2.h"
#include "stuff.h"
#include "version.h"
#include "wpn-misc.h"

#ifdef MACROS
#include "macro.h"
#endif


//extern bool wield_change;    // defined in output.cc


bool class_allowed(unsigned char speci, int char_class);
bool verifyPlayerName(void);
void choose_weapon(void);
void enterPlayerName(void);
void give_basic_knowledge(int which_job);
void give_basic_spells(int which_job);
void give_last_paycheck(int which_job);
void init_player(void);
void jobs_hpmp_init(int which_job);
void jobs_stat_init(int which_job);
void openingScreen(void);
void species_stat_init(unsigned char which_species);




bool new_game( void )
{
    int i, j;                      // loop variables {dlb}

    FILE *handle;
    char char_fil[kFileNameSize];
    unsigned char keyn;
    unsigned char weap_skill = 0;
    int to_hit_bonus;             // used for assigning primary weapons {dlb}

    init_player();

    you.exp_available = 25;      // now why is this all the way up here? {dlb}

    textcolor(LIGHTGREY);

// copy name into you.your_name if set from environment --
// note that you.your_name could already be set from init.txt
// this, clearly, will overwrite such information {dlb}
    if ( sys_env.crawl_name )
      strncpy(you.your_name, sys_env.crawl_name, kNameLen);

    openingScreen();

    enterPlayerName();





#ifdef LOAD_UNPACKAGE_CMD
    // Create the file name base
    char name_buff[kFileNameLen];

    sprintf(name_buff, SAVE_DIR_PATH "%s%d", you.your_name, getuid());

    char zip_buff[kFileNameLen];

    strcpy(zip_buff, name_buff);
    strcat(zip_buff, PACKAGE_SUFFIX);

    // Create save dir name
    strcpy(char_fil, name_buff);
    strcat(char_fil, ".sav");

    handle = fopen(zip_buff, "rb+");
    if (handle != NULL)
    {
        cprintf(EOL "Loading game..." EOL);

        // Create command
        char cmd_buff[1024];

        sprintf(cmd_buff, LOAD_UNPACKAGE_CMD, name_buff);

        system(cmd_buff);

        fclose(handle);

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
        strcat(name_buff, ".bak");
        rename(char_fil, name_buff);
#endif
    }

#else
    strcpy(char_fil, "");
    strncat(char_fil, you.your_name, kFileNameLen);
    strcat(char_fil, ".sav");
#endif

    int passout = 0;

    handle = fopen(char_fil, "rb+");

    if (handle != NULL)
    {
        cprintf(EOL "Welcome back, ");
        cprintf(you.your_name);
        cprintf("!");
        fclose(handle);

        return false;
    }

    clrscr();    // this replaces the following commented chunk b/c
                 // things also look better under linux, so why not
                 // just make all platforms start from the top? {dlb}
                 // problem for Macs?

/* ************************************************************************
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
************************************************************************ */

    cprintf("You must be new here!" EOL EOL);

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

        do
        {
            keyn = getch();
        }
        while (keyn != '?' && keyn != 'x' && keyn != 'X'
               && !(keyn >= 'a' && keyn <= 'e'));

        if (keyn == '?')
          keyn = 'a' + random2(5);
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

        do
        {
            keyn = getch();
        }
        while (keyn != '?' && keyn != 'x' && keyn != 'X'
               && !(keyn >= 'a' && keyn <= 'b'));

        if (keyn == '?')
          keyn = ( coinflip() ? 'a' : 'b' );
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
        break;
      case 'b':
        you.species = SP_ELF;
        break;
      case 'c':
        you.species = SP_HIGH_ELF;
        break;
      case 'd':
        you.species = SP_GREY_ELF;
        break;
      case 'e':
        you.species = SP_DEEP_ELF;
        break;
      case 'f':
        you.species = SP_SLUDGE_ELF;
        break;
      case 'g':
        you.species = SP_HILL_DWARF;
        break;
      case 'h':
        you.species = SP_MOUNTAIN_DWARF;
        break;
      case 'i':
        you.species = SP_HALFLING;
        break;
      case 'j':
        you.species = SP_HILL_ORC;
        break;
      case 'k':
        you.species = SP_KOBOLD;
        break;
      case 'l':
        you.species = SP_MUMMY;
        break;
      case 'm':
        you.species = SP_NAGA;
        break;
      case 'n':
        you.species = SP_GNOME;
        break;
      case 'o':
        you.species = SP_OGRE;
        break;
      case 'p':
        you.species = SP_TROLL;
        break;
      case 'q':
        you.species = SP_OGRE_MAGE;
        break;
      case 'r':                   // draconian
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

        do
        {
            keyn = getch();
        }
        while (keyn != '?' && keyn != 'x' && keyn != 'X'
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
        you.species = SP_RED_DRACONIAN + random2(9);    // random drac
#endif
        break;

      case 's':
        you.species = SP_CENTAUR;
        break;
      case 't':
        you.species = SP_DEMIGOD;
        break;
      case 'u':
        you.species = SP_SPRIGGAN;
        break;
      case 'v':
        you.species = SP_MINOTAUR;
        break;
      case 'w':
        you.species = SP_DEMONSPAWN;
        break;
      case 'x':
        you.species = SP_GHOUL;
        break;
      case 'y':
        you.species = SP_KENKU;
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

    j = 0;    // used within for loop to determine newline {dlb}

    for (i = 0; i < 30; i++)
    {
        if ( i == 23 )    // hackish {dlb}
          i = JOB_REAVER;

        if ( !class_allowed(you.species, i) )
          continue;

        putch(index_to_letter(i));
        cprintf(" - ");
        cprintf(job_title(i));

        if ( j % 2 )
          cprintf(EOL);
        else
          gotoxy(40, wherey());
        j++;

    }

    if ( wherex() >= 40 )
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
                keyn = 'a' + random2(28);
            }
            while (keyn == 'x');

            if (keyn == '{') // as ASCII is not contiguously alphebetized...
              keyn = 'A';
            else if (keyn == '|')
              keyn = 'B';
        }
        while (!class_allowed(you.species, keyn - 'a'));

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
cant_be_that:
        goto query;
    }

    if ( !class_allowed(you.species, you.char_class) )
      goto cant_be_that;




// ************ round-out character statistics and such ************

    species_stat_init(you.species);          // must be down here {dlb}

    you.is_undead = ( (you.species == SP_MUMMY) ? US_UNDEAD :
                      (you.species == SP_GHOUL) ? US_HUNGRY_DEAD
                                                : US_ALIVE );

    // before we get into the inventory init,  set light radius based
    // on species vision.

    // currently,  all species see out to 8 squares.
    you.normal_vision = 8;
    you.current_vision = 8;

    you.attribute[ATTR_WALK_SLOWLY] = (you.species == SP_NAGA);

    jobs_stat_init(you.char_class);
    jobs_hpmp_init(you.char_class);
    strcpy(you.class_name, job_title(you.char_class));
    give_last_paycheck(you.char_class);

    switch ( you.char_class )
    {
      case JOB_FIGHTER:
        you.inv_quantity[0] = 1;
        you.inv_class[0] = OBJ_WEAPONS;
        you.inv_type[0] = WPN_SHORT_SWORD;
        you.inv_plus[0] = 50;
        you.inv_plus2[0] = 50;
        you.inv_dam[0] = 0;
        you.inv_colour[0] = LIGHTCYAN;

        if ( you.species == SP_OGRE || you.species == SP_TROLL
             || player_genus(GENPC_DRACONIAN) )
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
            else if (you.species == SP_TROLL)
            {
                you.inv_quantity[0] = 0;
                you.inv_class[0] = OBJ_WEAPONS;
                you.inv_type[0] = WPN_CLUB;
                you.inv_plus[0] = 50;
                you.inv_dam[0] = 0;
                you.inv_colour[0] = BROWN;

            }
            else if ( player_genus(GENPC_DRACONIAN) )
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
            you.inv_colour[1] = WHITE;  // grave shroud

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

            you.inv_quantity[2] = 8 + random2avg(15,2);
            you.inv_class[2] = OBJ_MISSILES;
            you.inv_type[2] = MI_DART;

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

            choose_weapon();
            cprintf(EOL "A fine choice.");
        }


        if ( you.species != SP_TROLL )
          you.equip[EQ_WEAPON] = 0;

        you.equip[EQ_BODY_ARMOUR] = 1;

        if (you.species != SP_KOBOLD && you.species != SP_OGRE
            && you.species != SP_TROLL && you.species != SP_GHOUL)
            you.equip[EQ_SHIELD] = 2;

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

            you.skills[(player_light_armour() ? SK_DODGING : SK_ARMOUR)] = 2;

            you.skills[SK_SHIELDS] = 2;
            you.skills[SK_THROWING] = 2;
            you.skills[( coinflip() ? SK_STABBING : SK_SHIELDS )]++;
            //you.skills[SK_UNARMED_COMBAT] = 1;
        }
        else
            you.skills[SK_FIGHTING] += 2;
        break;

    case JOB_WIZARD:
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

        you.inv_quantity[1] = 1;
        you.inv_class[1] = OBJ_ARMOUR;
        you.inv_type[1] = ARM_ROBE;
        you.inv_plus[1] = 50;

    // this bit is a bit odd: {dlb}
        if ( !one_chance_in(3) )
          you.inv_dam[1] = ( coinflip() ? 150 : 120 );
        else
          you.inv_dam[1] = 30;

        you.inv_colour[1] = random_colour();
        you.equip[EQ_WEAPON] = 0;
        you.equip[EQ_BODY_ARMOUR] = 1;

// extra items being tested:
        you.inv_class[2] = OBJ_BOOKS;
        you.inv_type[2] = random2(3);
        you.inv_quantity[2] = 1;
        you.inv_plus[2] = 0;    // = 127
        you.inv_dam[2] = 1;
        you.inv_colour[2] = CYAN;

        you.skills[SK_DODGING] = 1;
        you.skills[SK_STEALTH] = 1;
        you.skills[( coinflip() ? SK_DODGING : SK_STEALTH )]++;
        you.skills[SK_SPELLCASTING] = 2;
        you.skills[SK_CONJURATIONS] = 1;
        you.skills[SK_ENCHANTMENTS] = 1;
        you.skills[SK_SPELLCASTING + random2(3)]++;
        you.skills[SK_SUMMONINGS + random2(5)]++;
        you.skills[SK_SHORT_BLADES] = 1;
        you.skills[SK_STAVES] = 1;
        break;


    case JOB_PRIEST:
        you.piety = 45;

        you.inv_quantity[0] = 1;
        you.inv_class[0] = OBJ_WEAPONS;
        you.inv_type[0] = WPN_MACE; //jmf: moved from being in "case 'b'" below
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

        you.inv_class[2] = OBJ_POTIONS;
        you.inv_type[2] = POT_HEALING;
        you.inv_quantity[2] = ( coinflip() ? 3 : 2 );
        you.inv_plus[2] = 0;
        you.inv_dam[2] = 0;
        you.inv_colour[2] = random_colour();    // hmmm...

        you.equip[EQ_WEAPON] = 0;

        you.equip[EQ_BODY_ARMOUR] = 1;

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
            cprintf(EOL "Welcome...");
            break;
        default:
            goto getkey;
        }
        break;

    case JOB_THIEF:
        you.inv_quantity[0] = 1;
        you.inv_class[0] = OBJ_WEAPONS;
        you.inv_type[0] = WPN_SHORT_SWORD;

        you.inv_plus[0] = 50;
        you.inv_plus2[0] = 50;
        you.inv_dam[0] = 0;
        you.inv_colour[0] = LIGHTCYAN;

        you.inv_quantity[1] = 1;
        you.inv_class[1] = OBJ_WEAPONS;
        you.inv_type[1] = WPN_DAGGER;

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

        you.inv_quantity[4] = 10 + random2avg(19,2);
        you.inv_class[4] = OBJ_MISSILES;
        you.inv_type[4] = MI_DART;

        you.inv_plus[4] = 50;
        you.inv_dam[4] = 0;
        you.inv_colour[4] = LIGHTCYAN;

        you.equip[EQ_WEAPON] = 0;
        you.equip[EQ_BODY_ARMOUR] = 2;
        you.equip[EQ_CLOAK] = 3;

        you.skills[SK_FIGHTING] = 1;
        you.skills[SK_SHORT_BLADES] = 2;
        you.skills[SK_DODGING] = 2;
        you.skills[SK_STEALTH] = 2;
        you.skills[SK_STABBING] = 1;
        you.skills[SK_DODGING + random2(3)]++;
        you.skills[SK_THROWING] = 1;
        you.skills[SK_DARTS] = 1;
        you.skills[SK_TRAPS_DOORS] = 2;
        break;

    case JOB_GLADIATOR:
        you.inv_quantity[0] = 1;
        you.inv_class[0] = OBJ_WEAPONS;
        you.inv_type[0] = WPN_SHORT_SWORD;
        choose_weapon();
        cprintf(EOL "A fine choice.");

        you.inv_plus[0] = 50;
        you.inv_plus2[0] = 50;
        you.inv_dam[0] = 0;
        you.inv_colour[0] = LIGHTCYAN;
        if ( player_genus(GENPC_DRACONIAN) )
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

        you.equip[EQ_WEAPON] = 0;
        you.equip[EQ_BODY_ARMOUR] = 1;
        you.equip[EQ_SHIELD] = 2;

        you.skills[SK_FIGHTING] = 3;
        weap_skill = 3;

        if ( player_genus(GENPC_DRACONIAN) )
          you.skills[SK_DODGING] = 2;
        else
          you.skills[SK_ARMOUR] = 2;
        you.skills[SK_SHIELDS] = 1;
        you.skills[SK_UNARMED_COMBAT] = 2;
        break;


    case JOB_NECROMANCER:
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
        you.equip[EQ_WEAPON] = 0;
        you.equip[EQ_BODY_ARMOUR] = 1;
        you.inv_class[2] = OBJ_BOOKS;

        you.inv_type[2] = BOOK_NECROMANCY;
        you.inv_quantity[2] = 1;
        you.inv_plus[2] = 0;    // = 127
        you.inv_dam[2] = 0;     // = 1;

        you.inv_colour[2] = DARKGREY;

        you.skills[SK_DODGING] = 1;
        you.skills[SK_STEALTH] = 1;
        you.skills[( coinflip() ? SK_DODGING : SK_STEALTH )]++;
        you.skills[SK_SPELLCASTING] = 1;
        you.skills[SK_NECROMANCY] = 4;
        you.skills[SK_SHORT_BLADES] = 1;
        you.skills[SK_STAVES] = 1;

        break;



    case JOB_PALADIN:
        you.religion = GOD_SHINING_ONE;
        you.piety = 28;

        you.inv_quantity[0] = 1;
        you.inv_class[0] = OBJ_WEAPONS;
        you.inv_type[0] = WPN_SABRE;
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

        you.equip[EQ_WEAPON] = 0;
        you.equip[EQ_BODY_ARMOUR] = 1;
        you.equip[EQ_SHIELD] = 2;

        you.inv_class[3] = OBJ_POTIONS;
        you.inv_type[3] = POT_HEALING;
        you.inv_quantity[3] = 1;
        you.inv_plus[3] = 0;
        you.inv_dam[3] = 0;
        you.inv_colour[3] = random_colour();

        you.skills[SK_FIGHTING] = 2;
        you.skills[SK_ARMOUR] = 1;
        you.skills[SK_DODGING] = 1;
        you.skills[(coinflip() ? SK_ARMOUR : SK_DODGING)]++;
        you.skills[SK_SHIELDS] = 2;
        you.skills[SK_SHORT_BLADES] = 2;
        you.skills[SK_LONG_SWORDS] = 2;
        you.skills[SK_INVOCATIONS] = 1;
        break;

    case JOB_ASSASSIN:
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

        you.inv_quantity[4] = random2avg(19,2) + 10;
        you.inv_class[4] = OBJ_MISSILES;
        you.inv_type[4] = MI_DART;

        you.inv_plus[4] = 50;
        you.inv_dam[4] = 3;
        you.inv_colour[4] = LIGHTCYAN;

        you.equip[EQ_WEAPON] = 0;
        you.equip[EQ_BODY_ARMOUR] = 2;
        you.equip[EQ_CLOAK] = 3;

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
        you.religion = GOD_TROG;
        you.piety = 35;

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
            you.inv_type[0] = WPN_HAND_AXE;
            you.inv_plus[0] = 50;
            you.inv_plus2[0] = 50;
            you.inv_dam[0] = 0;
            you.inv_colour[0] = LIGHTCYAN;
            you.equip[EQ_WEAPON] = 0;

            for (unsigned char i = 1; i <= 3; i++)
            {
                you.inv_quantity[i] = 1;
                you.inv_class[i] = OBJ_WEAPONS;
                you.inv_type[i] = WPN_SPEAR;
                you.inv_plus[i] = 50;
                you.inv_plus2[i] = 50;
                you.inv_dam[i] = 0;
                you.inv_colour[i] = LIGHTCYAN;
            }
        }

// ARMOUR

        if ( you.species == SP_OGRE || you.species == SP_TROLL
              || player_genus(GENPC_DRACONIAN) )
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

// SKILLS
        you.skills[SK_FIGHTING] = 2;

        if (you.species == SP_TROLL)
        {
// *BCR* Troll Berserkers get no weapon, so I gave them unarmed/dodging
            you.skills[SK_FIGHTING] += 3;
            you.skills[SK_DODGING] = 2;
            you.skills[SK_UNARMED_COMBAT] = 3;
        }
        else if ( you.species == SP_OGRE )
        {
            you.skills[SK_FIGHTING] += 3;
            you.skills[SK_AXES] = 1;
            you.skills[SK_MACES_FLAILS] = 3;
        }
        else
        {
            you.skills[SK_AXES] = 3;
            you.skills[SK_POLEARMS] = 1;
            you.skills[SK_ARMOUR] = 2;
            you.skills[SK_DODGING] = 2;
            you.skills[SK_THROWING] = 2;
        }
        break;

    case JOB_HUNTER:
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

        you.inv_quantity[2] = 15 + random2avg(21,5);
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

        if ( player_genus(GENPC_DRACONIAN) )
        {
            you.inv_type[3] = ARM_ROBE;
            you.inv_colour[3] = GREEN;
        }

        you.equip[EQ_WEAPON] = 0;
        you.equip[EQ_BODY_ARMOUR] = 3;

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
            you.skills[( coinflip() ? SK_STABBING : SK_SHIELDS )]++;
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
        you.inv_quantity[0] = 1;
        you.inv_class[0] = OBJ_WEAPONS;
        you.inv_colour[0] = LIGHTCYAN;

        if (you.species == SP_OGRE_MAGE)
          {
    // Enchanters have a low level spell that wants a short blade
    //jmf: 24jun2000 No longer the case. Commented out.
            /*
            if (you.char_class == JOB_ENCHANTER)
              {
                you.inv_type[0] = WPN_SHORT_SWORD;
              }
            else
            */
             {
               you.inv_type[0] = WPN_QUARTERSTAFF;
               you.inv_colour[0] = BROWN;
             }
          }
        else
          {
            you.inv_type[0] = WPN_DAGGER;
          }

        you.inv_plus[0] = 50;
        you.inv_plus2[0] = 50;

        you.inv_dam[0] = 0;
        you.inv_quantity[1] = 1;
        you.inv_class[1] = OBJ_ARMOUR;
        you.inv_type[1] = ARM_ROBE;
        you.inv_plus[1] = 50;

        if (you.char_class == JOB_ENCHANTER)
          {
            you.inv_plus[0] = 51;
            you.inv_plus2[0] = 51;
            you.inv_plus[1] = 51;
          }
#ifdef USE_WARPER_BETTER_WEAPON
        else if (you.char_class == JOB_WARPER)
          {
            you.inv_plus[0]  = 51;
            you.inv_dam[0]  += SPWPN_DISTORTION;
          }
#endif

        you.inv_dam[1] = 0;
        you.inv_colour[1] = random_colour();

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

        you.equip[EQ_WEAPON] = 0;
        you.equip[EQ_BODY_ARMOUR] = 1;
        you.inv_class[2] = OBJ_BOOKS;
        you.inv_type[2] = coinflip() ? BOOK_CONJURATIONS_I : BOOK_CONJURATIONS_II;
        you.inv_plus[2] = 0;    // = 127

#if 0 // moved to switch, below
        if (you.char_class == JOB_SUMMONER)
          {
            you.inv_type[2] = BOOK_SUMMONINGS;
            you.inv_plus[2] = 0;
            you.skills[SK_SUMMONINGS] = 4;
            // gets some darts - this class is difficult to start off with
            you.inv_quantity[3] = random2avg(9,2) + 7;
            you.inv_class[3] = OBJ_MISSILES;
            you.inv_type[3] = MI_DART;
            you.inv_plus[3] = 50;
            you.inv_dam[3] = 0;
            you.inv_colour[3] = LIGHTCYAN;
          }
#endif // 0

        switch (you.char_class)
          {
          case JOB_SUMMONER:
            you.inv_type[2] = BOOK_SUMMONINGS;
            you.inv_plus[2] = 0;
            you.skills[SK_SUMMONINGS] = 4;
            // gets some darts - this class is difficult to start off with
            you.inv_quantity[3] = random2avg(9,2) + 7;
            you.inv_class[3] = OBJ_MISSILES;
            you.inv_type[3] = MI_DART;
            you.inv_plus[3] = 50;
            you.inv_dam[3] = 0;
            you.inv_colour[3] = LIGHTCYAN;
            break;

          case JOB_CONJURER:
            you.skills[SK_CONJURATIONS] = 4;
            break;

          case JOB_ENCHANTER:
            you.inv_type[2] = BOOK_CHARMS;
            you.inv_plus[2] = 0;
            you.skills[SK_ENCHANTMENTS] = 4;

// replacing wand with darts
#if 0
            you.inv_quantity[3] = 1;
            you.inv_class[3] = OBJ_WANDS;
            you.inv_dam[3] = 0;
            you.inv_colour[3] = random_colour();
            switch ( random2(4) )
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
#else
            you.inv_quantity[3] = random2avg(15,2) + 7;
            you.inv_class[3] = OBJ_MISSILES;
            you.inv_type[3] = MI_DART;
            you.inv_plus[3] = 51;
            you.inv_dam[3] = 0;
            you.inv_colour[3] = LIGHTCYAN;
#endif
            break;

          case JOB_FIRE_ELEMENTALIST:
            you.inv_type[2] = BOOK_FLAMES;
            you.inv_plus[2] = 0;
            you.skills[SK_CONJURATIONS] = 1;
            //you.skills [SK_ENCHANTMENTS] = 1;
            you.skills[SK_FIRE_MAGIC] = 3;
            break;

          case JOB_ICE_ELEMENTALIST:
            you.inv_type[2] = BOOK_FROST;
            you.inv_plus[2] = 0;
            you.skills[SK_CONJURATIONS] = 1;
            //you.skills [SK_ENCHANTMENTS] = 1;
            you.skills[SK_ICE_MAGIC] = 3;
            break;

          case JOB_AIR_ELEMENTALIST:
            you.inv_type[2] = BOOK_AIR;
            you.inv_plus[2] = 0;
            you.skills[SK_CONJURATIONS] = 1;
            //you.skills [SK_ENCHANTMENTS] = 1;
            you.skills[SK_AIR_MAGIC] = 3;
            break;

          case JOB_EARTH_ELEMENTALIST:
            you.inv_type[2] = BOOK_GEOMANCY;
            you.inv_plus[2] = 0;
            //you.skills[SK_CONJURATIONS] = 1;
            you.skills[SK_TRANSMIGRATION] = 1;
            you.skills[SK_EARTH_MAGIC] = 3;
            break;

        case JOB_VENOM_MAGE:
            you.inv_type[2] = BOOK_YOUNG_POISONERS;
            you.inv_plus[2] = 0;
            you.skills[SK_POISON_MAGIC] = 4;
            break;

        case JOB_TRANSMUTER:
            you.inv_type[2] = BOOK_CHANGES;
            you.inv_plus[2] = 0;
            you.skills[SK_TRANSMIGRATION] = 4;
            break;

        case JOB_WARPER:
            you.inv_type[2] = BOOK_SPATIAL_TRANSLOCATIONS;
            you.inv_plus[2] = 0;
            you.skills[SK_TRANSLOCATIONS] = 4;
            break;
        }

        you.inv_quantity[2] = 1;
        you.inv_dam[2] = 0;
        you.inv_colour[2] = random_colour();
        if (you.char_class == JOB_FIRE_ELEMENTALIST)
            you.inv_colour[2] = RED;
        if (you.char_class == JOB_ICE_ELEMENTALIST)
            you.inv_colour[2] = LIGHTCYAN;
        if (you.char_class == JOB_VENOM_MAGE)
            you.inv_colour[2] = GREEN;

        you.skills[SK_DODGING] = 1;
        you.skills[SK_STEALTH] = 1;
        you.skills[( coinflip() ? SK_DODGING : SK_STEALTH )]++;
        you.skills[SK_SPELLCASTING] = 1;
        you.skills[SK_SHORT_BLADES] = 1;
        you.skills[SK_STAVES] = 1;
        break;

    case JOB_CRUSADER:
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
        you.inv_colour[1] = random_colour();

        you.inv_class[2] = OBJ_BOOKS;
        you.inv_type[2] = BOOK_WAR_CHANTS;
        you.inv_quantity[2] = 1;
        you.inv_plus[2] = 0;
        you.inv_dam[2] = 0;
        you.inv_colour[2] = random_colour();

        you.equip[EQ_WEAPON] = 0;
        you.equip[EQ_BODY_ARMOUR] = 1;

        you.skills[SK_FIGHTING] = 3;
        you.skills[SK_ARMOUR] = 1;
        you.skills[SK_DODGING] = 1;
        you.skills[SK_STEALTH] = 1;
        you.skills[SK_SPELLCASTING] = 2;
        you.skills[SK_ENCHANTMENTS] = 2;
        break;


    case JOB_DEATH_KNIGHT:
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
        you.inv_plus[2] = 0;
        you.inv_dam[2] = 0;
        you.inv_colour[2] = DARKGREY;

        you.equip[EQ_WEAPON] = 0;
        you.equip[EQ_BODY_ARMOUR] = 1;

        if (you.species == SP_DEMIGOD)
        {
            you.skills[SK_SPELLCASTING] = 1;
            you.skills[SK_NECROMANCY] = 2;
        }
        else
        {
            clrscr();
            cprintf(EOL " From where do you draw your power?" EOL);
            cprintf("a - Necromantic magic" EOL);
            cprintf("b - the God Yredelemnul" EOL);

getkey1:
            keyn = get_ch();

            switch (keyn)
            {
            case 'a':
                cprintf(EOL "Very well.");
                you.skills[SK_SPELLCASTING] = 1;
                you.skills[SK_NECROMANCY] = 2;
                break;
            case 'b':
                you.religion = GOD_YREDELEMNUL;
                you.piety = 28;
                you.inv_plus[0] = 51;
                you.inv_plus2[0] = 51;
                you.inv_quantity[2] = 0;
                you.skills[SK_INVOCATIONS] = 3;
                cprintf(EOL "Welcome...");
                break;
            default:
                goto getkey1;
            }

        }

        you.skills[SK_FIGHTING] = 2;
        you.skills[SK_ARMOUR] = 1;
        you.skills[SK_DODGING] = 1;
        you.skills[SK_STEALTH] = 1;
        //you.skills [SK_SHORT_BLADES] = 2;
        you.skills[SK_STABBING] = 1;
        break;

    case JOB_CHAOS_KNIGHT:
        you.piety = 25;         // irrelevant for Xom, of course
        you.inv_quantity[0] = 1;
        you.inv_class[0] = OBJ_WEAPONS;
        you.inv_type[0] = WPN_SHORT_SWORD;
        you.inv_plus[0] = 50 + random2(3);
        you.inv_plus2[0] = 50 + random2(3);
        you.inv_dam[0] = 0;
        if (one_chance_in(5))
            you.inv_dam[0] = 30;
        if (one_chance_in(5))
            you.inv_dam[0] = 60;
        you.inv_colour[0] = LIGHTCYAN;
        choose_weapon();
        weap_skill = 2;
        you.inv_quantity[1] = 1;
        you.inv_class[1] = OBJ_ARMOUR;
        you.inv_type[1] = ARM_ROBE;
        you.inv_plus[1] = 50 + random2(3);
        you.inv_dam[1] = 0;
        you.inv_colour[1] = random_colour();

        you.equip[EQ_WEAPON] = 0;
        you.equip[EQ_BODY_ARMOUR] = 1;

        you.skills[SK_FIGHTING] = 3;
        you.skills[SK_ARMOUR] = 1;
        you.skills[SK_DODGING] = 1;
        you.skills[(coinflip() ? SK_ARMOUR : SK_DODGING)]++;
        you.skills[SK_STABBING] = 1;

        clrscr();
        cprintf(EOL " Which God of Chaos do you wish to serve?" EOL);
        cprintf("a - Xom of Chaos" EOL);
        cprintf("b - Makhleb the Destroyer" EOL);

getkey2:

        keyn = get_ch();

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
        break;

    case JOB_HEALER:
        you.religion = GOD_ELYVILON;
        you.piety = 45;

        you.inv_quantity[0] = 1;
        you.inv_class[0] = OBJ_WEAPONS;
        you.inv_type[0] = WPN_QUARTERSTAFF;
        you.inv_colour[0] = BROWN;
        you.inv_plus[0] = 50;
        you.inv_plus2[0] = 50;
        you.inv_dam[0] = 0;


        // Robe
        you.inv_quantity[1] = 1;
        you.inv_class[1] = OBJ_ARMOUR;
        you.inv_type[1] = ARM_ROBE;
        you.inv_plus[1] = 50;
        you.inv_dam[1] = 0;
        you.inv_colour[1] = WHITE;

        you.inv_class[2] = OBJ_POTIONS;
        you.inv_type[2] = POT_HEALING;
        you.inv_quantity[2] = ( coinflip() ? 3 : 2 );
        you.inv_plus[2] = 0;
        you.inv_dam[2] = 0;
        you.inv_colour[2] = random_colour();    // hmmm...

        you.inv_class[3] = OBJ_POTIONS;
        you.inv_type[3] = POT_HEAL_WOUNDS;
        you.inv_quantity[3] = ( coinflip() ? 3 : 2);
        you.inv_plus[3] = 0;
        you.inv_dam[3] = 0;
        you.inv_colour[3] = random_colour();    // hmmm...

        you.equip[EQ_WEAPON] = 0;
        you.equip[EQ_BODY_ARMOUR] = 1;

        you.skills[SK_FIGHTING] = 2;
        you.skills[SK_DODGING] = 1;
        you.skills[SK_SHIELDS] = 1;
        you.skills[SK_THROWING] = 2;
        you.skills[SK_STAVES] = 3;
        you.skills[SK_INVOCATIONS] = 2;
        break;


    case JOB_REAVER:
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
        you.inv_type[2] = coinflip() ? BOOK_CONJURATIONS_I : BOOK_CONJURATIONS_II;
        you.inv_quantity[2] = 1;
        you.inv_plus[2] = 0;    // = 127
        you.inv_dam[2] = 0;
        you.inv_colour[2] = RED;

        you.equip[EQ_WEAPON] = 0;
        you.equip[EQ_BODY_ARMOUR] = 1;

        you.skills[SK_FIGHTING] = 2;
        you.skills[SK_ARMOUR] = 1;
        you.skills[SK_DODGING] = 1;

        you.skills[SK_SPELLCASTING] = 1;
        you.skills[SK_CONJURATIONS] = 2;
        break;

    case JOB_STALKER:
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
        //you.inv_type[3] = BOOK_YOUNG_POISONERS;
        you.inv_type[3] = BOOK_ASSASSINATION; //jmf: new book!
        you.inv_quantity[3] = 1;
        you.inv_plus[3] = 126;
        you.inv_dam[3] = 0;
        you.inv_colour[3] = GREEN;
        you.equip[EQ_WEAPON] = 0;
        you.equip[EQ_BODY_ARMOUR] = 1;
        you.equip[EQ_CLOAK] = 2;

        you.skills[SK_FIGHTING] = 1;
        you.skills[SK_SHORT_BLADES] = 1;
        you.skills[SK_POISON_MAGIC] = 1;
        you.skills[SK_DODGING] = 1;
        you.skills[SK_STEALTH] = 2;
        you.skills[SK_STABBING] = 2;
        you.skills[SK_DODGING + random2(3)]++;
        //you.skills[SK_THROWING] = 1; //jmf: removed these, added magic below
        //you.skills[SK_DARTS] = 1;
        you.skills[SK_SPELLCASTING] = 1;
        you.skills[SK_ENCHANTMENTS] = 1;
        break;

    case JOB_MONK:
        you.inv_class[0] = OBJ_ARMOUR;
        you.inv_type[0] = ARM_ROBE;
        you.inv_plus[0] = 50;
        you.inv_dam[0] = 0;
        you.inv_quantity[0] = 1;
        you.inv_colour[0] = BROWN;

        you.equip[EQ_WEAPON] = -1;
        you.equip[EQ_BODY_ARMOUR] = 0;

        you.skills[SK_FIGHTING] = 3;
        you.skills[SK_UNARMED_COMBAT] = 4;
        you.skills[SK_DODGING] = 3;
        you.skills[SK_STEALTH] = 2;
        break;
    }

// randomly boost stats a number of times based on species - should be a function {dlb}
    unsigned char points_left = ( you.species == SP_DEMIGOD || you.species == SP_DEMONSPAWN ) ? 15 : 8;

    do
    {
        switch ( random2(NUM_STATS) )
        {
          case STAT_STRENGTH:
            if ( you.strength > 17 && coinflip() )
              continue;
            you.strength++;
            break;
          case STAT_DEXTERITY:
            if ( you.dex > 17 && coinflip() )
              continue;
            you.dex++;
            break;
          case STAT_INTELLIGENCE:
            if ( you.intel > 17 && coinflip() )
              continue;
            you.intel++;
            break;
        }
        points_left--;
    }
    while (points_left > 0);


// first: set base_hp {dlb}
    you.base_hp = 5000;
// then: adjust hp_max by species {dlb}
    if ( player_genus(GENPC_DRACONIAN) || player_genus(GENPC_DWARVEN) )
      you.hp_max++;
    else
    {
        switch ( you.species )
        {
          case SP_CENTAUR:
          case SP_DEMIGOD:
          case SP_OGRE:
          case SP_TROLL:
            you.hp_max += 3;
            break;

          case SP_GHOUL:
          case SP_MINOTAUR:
          case SP_NAGA:
          case SP_OGRE_MAGE:
            you.hp_max += 2;
            break;

          case SP_HILL_ORC:
          case SP_MUMMY:
            you.hp_max++;
            break;

          case SP_ELF:
          case SP_GREY_ELF:
          case SP_HIGH_ELF:
            you.hp_max--;
            break;

          case SP_DEEP_ELF:
          case SP_GNOME:
          case SP_HALFLING:
          case SP_KENKU:
          case SP_KOBOLD:
          case SP_SPRIGGAN:
            you.hp_max -= 2;
            break;

          default:
            break;
        }
    }
// finally: set base_hp2 - must come after species adjustments! {dlb}
    you.base_hp2 = 5000 + you.hp_max;


// first: set base_magic_points {dlb}
    you.base_magic_points = 5000;
// then: adjust max_magic_points by species {dlb}
    switch ( you.species )
    {
      case SP_DEEP_ELF:
      case SP_DEMIGOD:
      case SP_GREY_ELF:
        you.max_magic_points++;
        break;

      default:
        break;
    }
// finally: set base_magic_points2 - must come after species adjustments! {dlb}
    you.base_magic_points2 = 5000 + you.max_magic_points;

// apply bonuses and recalculate hitpoints and experience points {dlb}
    calc_hp();
    calc_mp();
    set_hp(you.hp_max, false);

// these need to be set above using functions!!! {dlb}
    you.magic_points = you.max_magic_points;
    you.max_dex = you.dex;
    you.max_strength = you.strength;
    you.max_intel = you.intel;

    if ( weap_skill )
      you.skills[weapon_skill(OBJ_WEAPONS, you.inv_type[0])] = weap_skill;

    if ( !you.is_undead )
      {
        for (i = 0; i < ENDOFPACK; i++)
          if ( !you.inv_quantity[i] )
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

    for (i = 0; i < ENDOFPACK; i++)
    {
        if ( you.inv_quantity[i] )
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

            if (you.inv_class[i] <= OBJ_ARMOUR)  // || you.inv_class [i] == 2)

                switch ( you.species )
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
                    if (you.inv_class[i] == OBJ_WEAPONS || you.inv_class[i] == OBJ_MISSILES)
                        you.inv_dam[i] += 90;
                    else
                        you.inv_dam[i] += 180;
                    break;
                }
        }
    }


// must remember to check for already existing colours/combinations

    for (i = 0; i < 4; i++)
      for (j = 0; j < 50; j++)
        you.item_description[i][j] = 215;

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
                  case 0: // wands
                    you.item_description[i][j] = (coinflip()) ? random2(12) : random2(12) + (random2(12) * 16);
                    //: (random2(12) * 16) + random2(15);
                    break;
                  case 1: // potions
                    you.item_description[i][j] = (coinflip())? random2(14) : random2(14) + (random2(14) * 14);
                    break;
                  case 2: // scrolls
                    you.item_description[i][j] = random2(151);
                    you.item_description[4][j] = random2(151);
                    break;
                  case 3: // rings
                    you.item_description[i][j] = (coinflip())? random2(13) : random2(13) + (random2(13) * 13);
                    break;
                }

                for (int p = 0; p < 30; p++)    // don't have p < j because some are preassigned
                  if ( you.item_description[i][p] == you.item_description[i][j] && j != p )
                    passout = 0;
            }
            while (passout == 0);
        }
    }

    for (i = 0; i < 4; i++)
      for (j = 0; j < 30; j++)
        you.item_description[i][j]++;    // NEED THIS BIT!

    for (i = 0; i < 50; i++)
    {
        if ( !you.skills[i] )
          continue;

// you.skill_points [i] = skill_exp_needed(you.skills [i] + 1) * species_skills(i, you.species) / 100;

        you.skill_points[i] = skill_exp_needed(you.skills[i] + 1) + 1;

        if ( i == SK_SPELLCASTING )
          you.skill_points[i] = ((skill_exp_needed(you.skills[i] + 1) + 1) * 130) / 100;
        else if ( i == SK_INVOCATIONS )
          you.skill_points[i] = ((skill_exp_needed(you.skills[i] + 1) + 1) * 70) / 100;

        if ( you.skill_points[i] > skill_exp_needed(2) * species_skills(i, you.species) / 100 )
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

    for (i = 0; i < ENDOFPACK; i++)
      if ( you.inv_class[i] != OBJ_WEAPONS )
        set_id(you.inv_class[i], you.inv_type[i], 1);

    give_basic_spells(you.char_class);
    give_basic_knowledge(you.char_class);

// tmpfile purging removed in favour of marking
    for (int lvl = 0; lvl < MAX_LEVELS; lvl++)
      for (int dng = 0; dng < MAX_BRANCHES; dng++)
        tmp_file_pairs[lvl][dng] = false;

// This is the temporary file purging code. Unfortunately it doesn't work.
// Use the other block (also in #if 0 right now) if you want to resume
// file purging.
#if 0
    char del_file[kFileNameSize];
    char glorpstr[kFileNameSize];

#ifdef SAVE_DIR_PATH
    sprintf(glorpstr, SAVE_DIR_PATH "%s%d", you.your_name, getuid());
#else
    strncpy(glorpstr, you.your_name, kFileNameLen);

    glorpstr[strlen(you.your_name)] = '\0';

// This is broken. Length is not valid yet! We have to check if we got a
// trailing NULL; if not, write one:
    if (strlen(you.your_name) > kFileNameLen - 1)       /* is name 6 chars or more? */
      glorpstr[kFileNameLen] = '\0';  /* if so, char 7 should be NULL */
#endif

/*
   int fi = 0;
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
   }
*/

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
    for (del_file[dun] = 'a'; del_file[dun] <= '~'; del_file[dun]++)
    {
        for (del_file[tens] = '0'; del_file[tens] < '5'; del_file[tens]++)
        {
            for (del_file[ones] = '0'; del_file[ones] <= '9'; del_file[ones]++)
            {
                if (!unlink(del_file))
                    cprintf("\n\rFailed to delete: ");
                else
                    cprintf("\n\rDeleted: ");
                cprintf(del_file);
                if (!getch())
                    getch();
            }
        }
    }
#endif


#if 0
    char del_file[kFileNameSize];


    char glorpstr[kFileNameSize];

    strncpy(glorpstr, you.your_name, 6);

// This is broken. Length is not valid yet! We have to check if we got a
// trailing NULL; if not, write one:
    if (strlen(you.your_name) > 5)      /* is name 6 chars or more? */
        glorpstr[6] = (char) NULL;      /* if so, char 7 should be NULL */

    strncpy(glorpstr, you.your_name, 6);

// This is broken. Length is not valid yet! We have to check if we got a
// trailing NULL; if not, write one:
    if (strlen(you.your_name) > 5)      /* is name 6 chars or more? */
        glorpstr[6] = (char) NULL;      /* if so, char 7 should be NULL */

    int fi = 0;
    int fi2 = 0;
    char st_prn[6];

    for (fi2 = 0; fi2 < 30; fi2++)
    {
        for (fi = 0; fi < 50; fi++)
        {
            strcpy(del_file, glorpstr);
            strcat(del_file, ".");
            if (fi < 10)
                strcat(del_file, "0");
            itoa(fi, st_prn, 10);
            strcat(del_file, st_prn);
            st_prn[0] = fi2 + 97;
            st_prn[1] = '\0';
            strcat(del_file, st_prn);
            strcat(del_file, "\0");

            unlink(del_file);
        }
    }
#endif


// places staircases to the branch levels:
    for ( i = 0; i < 30; i++ )
      you.branch_stairs[i] = 100;

    you.branch_stairs[STAIRS_ECUMENICAL_TEMPLE] =
        3 + random2(4);         // avg:  4.5

    you.branch_stairs[STAIRS_ORCISH_MINES] =
        5 + random2(6);                                         // avg:  7.5

    you.branch_stairs[STAIRS_ELVEN_HALLS] =
        you.branch_stairs[STAIRS_ORCISH_MINES] + ( coinflip() ? 4 : 3); // 11.0

    you.branch_stairs[STAIRS_HIVE] =
        10 + random2(6);                                        // avg: 12.5

    you.branch_stairs[STAIRS_LAIR] =
        7 + random2(6);                                         // avg:  9.5

    you.branch_stairs[STAIRS_SLIME_PITS] =
        you.branch_stairs[STAIRS_LAIR] + 3 + random2(4);        // avg: 14.0

    you.branch_stairs[STAIRS_SWAMP] =
        you.branch_stairs[STAIRS_LAIR] + 2 + random2(6);        // avg: 14.0

    you.branch_stairs[STAIRS_SNAKE_PIT] =
        you.branch_stairs[STAIRS_LAIR] + ( coinflip() ? 7 : 6); // avg: 16.0

    you.branch_stairs[STAIRS_VAULTS] =
        13 + random2(6);                                        // avg: 15.5

    you.branch_stairs[STAIRS_CRYPT] =
        you.branch_stairs[STAIRS_VAULTS] + 2 + random2(3);      // avg: 18.5

    you.branch_stairs[STAIRS_TOMB] =
        you.branch_stairs[STAIRS_CRYPT] + ((coinflip())? 3 : 2);// avg: 20.0

    you.branch_stairs[STAIRS_HALL_OF_BLADES] =
        you.branch_stairs[STAIRS_VAULTS] + 4;                   // avg: 22.5

    you.branch_stairs[STAIRS_HALL_OF_ZOT] = 26;                 // always 26

    return true;

}          // end of new_game()




bool class_allowed( unsigned char speci, int char_class )
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
        if ( player_descriptor(PDSC_UNDEAD) )
          return false;

        switch (speci)
        {
          case SP_GNOME:
          case SP_HALFLING:
          case SP_HILL_DWARF:
          case SP_HILL_ORC:
          case SP_KENKU:
          case SP_KOBOLD:
          case SP_MINOTAUR:
          case SP_OGRE:
          case SP_SPRIGGAN:
          case SP_TROLL:
            return false;
        }
        return true;

      case JOB_PRIEST:
        if ( player_genus(GENPC_DRACONIAN) )
          return false;
        if ( player_descriptor(PDSC_UNDEAD) )
          return false;

        switch (speci)
        {
          case SP_CENTAUR:
          case SP_DEMIGOD:
          case SP_DEMONSPAWN:
          case SP_GNOME:
          case SP_HALFLING:
          case SP_KENKU:
          case SP_KOBOLD:
          case SP_MINOTAUR:
          case SP_NAGA:
          case SP_OGRE:
          case SP_OGRE_MAGE:
          case SP_SPRIGGAN:
          case SP_TROLL:
            return false;
        }

        return true;

      case JOB_THIEF:
        if ( player_descriptor(PDSC_UNDEAD) )
          return false;

        switch (speci)
        {
          case SP_CENTAUR:
          case SP_KENKU:
          case SP_MINOTAUR:
          case SP_OGRE:
          case SP_OGRE_MAGE:
          case SP_SPRIGGAN:
          case SP_TROLL:
            return false;
        }
        return true;

      case JOB_GLADIATOR:
        if ( player_genus(GENPC_ELVEN) )
          return false;
        if ( player_descriptor(PDSC_UNDEAD) )
          return false;

        switch (speci)
        {
          case SP_GNOME:
          case SP_HALFLING:
          case SP_KOBOLD:
          case SP_NAGA:
          case SP_OGRE:
          case SP_OGRE_MAGE:
          case SP_SPRIGGAN:
          case SP_TROLL:
            return false;
        }
        return true;

      case JOB_NECROMANCER:
        if ( player_genus(GENPC_DWARVEN) || player_genus(GENPC_DRACONIAN) )
          return false;

        switch (speci)
        {
          case SP_CENTAUR:
          case SP_ELF:
          case SP_GHOUL:
          case SP_GNOME:
          case SP_GREY_ELF:
          case SP_HALFLING:
          case SP_HIGH_ELF:
          case SP_MINOTAUR:
          case SP_OGRE:
          case SP_SPRIGGAN:
          case SP_TROLL:
            return false;
        }
        return true;

      case JOB_PALADIN:
        if ( player_genus(GENPC_ELVEN) || player_genus(GENPC_DRACONIAN) )
          return false;
        if ( player_descriptor(PDSC_UNDEAD) )
          return false;

        switch (speci)
        {
          case SP_CENTAUR:
          case SP_DEMIGOD:
          case SP_DEMONSPAWN:
          case SP_GNOME:
          case SP_HALFLING:
          case SP_HILL_DWARF:
          case SP_HILL_ORC:
          case SP_KENKU:
          case SP_KOBOLD:
          case SP_MINOTAUR:
          case SP_NAGA:
          case SP_OGRE:
          case SP_OGRE_MAGE:
          case SP_SPRIGGAN:
          case SP_TROLL:
            return false;
        }
        return true;

    case JOB_ASSASSIN:
        if ( player_genus(GENPC_DWARVEN) || player_genus(GENPC_DRACONIAN) )
          return false;

        switch (speci)
        {
          case SP_CENTAUR:
          case SP_GHOUL:
          case SP_GNOME:
          case SP_MINOTAUR:
          case SP_OGRE:
          case SP_OGRE_MAGE:
          case SP_TROLL:
            return false;
        }
        return true;

    case JOB_BERSERKER:
        if ( player_genus(GENPC_ELVEN) || player_genus(GENPC_DRACONIAN) )
          return false;
        if ( player_descriptor(PDSC_UNDEAD) )
          return false;

        switch (speci)
        {
          case SP_DEMIGOD:
          case SP_GNOME:
          case SP_HALFLING:
          case SP_KENKU:
          case SP_KOBOLD:
          case SP_MOUNTAIN_DWARF:
          case SP_NAGA:
          case SP_OGRE_MAGE:
          case SP_SPRIGGAN:
            return false;
        }
        return true;

      case JOB_HUNTER:
        if ( player_genus(GENPC_DRACONIAN)        // use bows
              || player_genus(GENPC_DWARVEN) )    // use xbows
          return true;

        switch (speci)
        {
        // bows --
          case SP_CENTAUR:
          case SP_DEMIGOD:
          case SP_DEMONSPAWN:
          case SP_ELF:
          case SP_GREY_ELF:
          case SP_HIGH_ELF:
          case SP_HUMAN:
          case SP_KENKU:
          case SP_MINOTAUR:
          case SP_NAGA:
          case SP_SLUDGE_ELF:
        // xbows --
          case SP_HILL_ORC:
        // slings --
          case SP_GNOME:
          case SP_HALFLING:
            return true;
        }
        return false;

      case JOB_CONJURER:
        if ( player_descriptor(PDSC_UNDEAD) )
          return false;

        switch (speci)
        {
          case SP_CENTAUR:
          case SP_GNOME:
          case SP_HALFLING:
          case SP_KOBOLD:
          case SP_MINOTAUR:
          case SP_OGRE:
          case SP_SPRIGGAN:
          case SP_TROLL:
            return false;
        }
        return true;

      case JOB_ENCHANTER:
        if ( player_genus(GENPC_DRACONIAN) )
          return false;
        if ( player_descriptor(PDSC_UNDEAD) )
          return false;

        switch (speci)
        {
          case SP_GNOME:
          case SP_HILL_ORC:
          case SP_KENKU:
          case SP_KOBOLD:
          case SP_MINOTAUR:
          case SP_OGRE:
          case SP_TROLL:
            return false;
        }
        return true;

      case JOB_FIRE_ELEMENTALIST:
        if ( player_genus(GENPC_DRACONIAN) )
          return false;
        if ( player_descriptor(PDSC_UNDEAD) )
          return false;

        switch (speci)
        {
          case SP_GNOME:
          case SP_GREY_ELF:
          case SP_HALFLING:
          case SP_KOBOLD:
          case SP_MINOTAUR:
          case SP_NAGA:
          case SP_OGRE:
          case SP_SPRIGGAN:
          case SP_TROLL:
            return false;
        }
        return true;

      case JOB_ICE_ELEMENTALIST:
        if ( player_genus(GENPC_DWARVEN) || player_genus(GENPC_DRACONIAN) )
          return false;
        if ( player_descriptor(PDSC_UNDEAD) )
          return false;

        switch (speci)
        {
          case SP_GNOME:
          case SP_GREY_ELF:
          case SP_HALFLING:
          case SP_HILL_ORC:
          case SP_KENKU:
          case SP_KOBOLD:
          case SP_MINOTAUR:
          case SP_NAGA:
          case SP_OGRE:
          case SP_SPRIGGAN:
          case SP_TROLL:
            return false;
        }
        return true;

      case JOB_SUMMONER:
        if ( player_genus(GENPC_DWARVEN) )
          return false;
        if ( player_descriptor(PDSC_UNDEAD) )
          return false;

        switch (speci)
        {
          case SP_CENTAUR:
          case SP_GNOME:
          case SP_HALFLING:
          case SP_MINOTAUR:
          case SP_OGRE:
          case SP_SPRIGGAN:
          case SP_TROLL:
            return false;
        }
        return true;

      case JOB_AIR_ELEMENTALIST:
        if ( player_genus(GENPC_DWARVEN) || player_genus(GENPC_DRACONIAN) )
          return false;
        if ( player_descriptor(PDSC_UNDEAD) )
          return false;

        switch (speci)
        {
          case SP_GNOME:
          case SP_HALFLING:
          case SP_HILL_ORC:
          case SP_KOBOLD:
          case SP_MINOTAUR:
          case SP_NAGA:
          case SP_OGRE:
          case SP_SPRIGGAN:
          case SP_TROLL:
            return false;
        }
        return true;

      case JOB_EARTH_ELEMENTALIST:
        if ( player_genus(GENPC_DRACONIAN) )
          return false;
        if ( player_descriptor(PDSC_UNDEAD) )
          return false;

        switch (speci)
        {
          case SP_ELF:
          case SP_GREY_ELF:
          case SP_HALFLING:
          case SP_HIGH_ELF:
          case SP_KENKU:
          case SP_KOBOLD:
          case SP_MINOTAUR:
          case SP_NAGA:
          case SP_OGRE:
          case SP_SPRIGGAN:
          case SP_TROLL:
            return false;
        }
        return true;

      case JOB_CRUSADER:
        if ( player_genus(GENPC_DWARVEN) )
          return false;
        if ( player_descriptor(PDSC_UNDEAD) )
          return false;

        switch (speci)
        {
          case SP_GNOME:
          //case SP_HALFLING: //jmf: they're such good enchanters...
          case SP_KENKU:
          case SP_KOBOLD:
          case SP_MINOTAUR:
          case SP_NAGA:
          case SP_OGRE:
          case SP_SPRIGGAN:
          case SP_TROLL:
            return false;
        }
        return true;

      case JOB_DEATH_KNIGHT:
        if ( player_genus(GENPC_DWARVEN) )
          return false;

        switch (speci)
        {
          case SP_ELF:
          case SP_GHOUL:
          case SP_GNOME:
          case SP_GREY_ELF:
          case SP_HALFLING:
          case SP_HIGH_ELF:
         // case SP_KOBOLD:
          case SP_MINOTAUR:
          case SP_OGRE:
          case SP_OGRE_MAGE:
          case SP_SPRIGGAN:
          case SP_TROLL:
            return false;
        }
        return true;

      case JOB_VENOM_MAGE:
        if ( player_genus(GENPC_DWARVEN) )
          return false;
        if ( player_descriptor(PDSC_UNDEAD) )
          return false;

        switch (speci)
        {
          case SP_CENTAUR:
          case SP_ELF:
          case SP_GNOME:
          case SP_GREY_ELF:
          case SP_HALFLING:
          case SP_HIGH_ELF:
          case SP_MINOTAUR:
          case SP_OGRE:
          case SP_TROLL:
            return false;
        }
        return true;

      case JOB_CHAOS_KNIGHT:
        if ( player_genus(GENPC_DWARVEN) )
          return false;
        if ( player_descriptor(PDSC_UNDEAD) )
          return false;

        switch (speci)
        {
          case SP_DEMIGOD:
          case SP_GNOME:
          case SP_GREY_ELF:
          case SP_HALFLING:
          case SP_KENKU:
          case SP_OGRE:
          case SP_OGRE_MAGE:
          case SP_SPRIGGAN:
          case SP_TROLL:
            return false;
        }
        return true;

      case JOB_TRANSMUTER:
        if ( player_descriptor(PDSC_UNDEAD) )
          return false;

        switch (speci)
        {
          case SP_GNOME:
          case SP_HALFLING:
          case SP_HILL_DWARF:
          case SP_HILL_ORC:
          case SP_KENKU:
          case SP_KOBOLD:
          case SP_MINOTAUR:
          case SP_OGRE:
          case SP_TROLL:
            return false;
        }
        return true;

      case JOB_HEALER:
        if ( player_genus(GENPC_DRACONIAN) )
          return false;
        if ( player_descriptor(PDSC_UNDEAD) )
          return false;

        switch (speci)
        {
          case SP_DEMIGOD:
          case SP_DEMONSPAWN:
          case SP_GNOME:
          case SP_HALFLING:
          case SP_KENKU:
          case SP_KOBOLD:
          case SP_MINOTAUR:
          case SP_NAGA:
          case SP_OGRE:
          case SP_OGRE_MAGE:
          case SP_SPRIGGAN:
          case SP_TROLL:
            return false;
        }
        return true;

      case JOB_REAVER:
        if ( player_descriptor(PDSC_UNDEAD) )
          return false;

        switch (speci)
        {
          case SP_GNOME:
          case SP_GREY_ELF:
          case SP_HALFLING:
          case SP_HILL_DWARF:
          case SP_MINOTAUR:
          case SP_MOUNTAIN_DWARF:
          case SP_OGRE:
          case SP_OGRE_MAGE:
          case SP_SPRIGGAN:
          case SP_TROLL:
            return false;
        }
        return true;

      case JOB_STALKER:
        if ( player_genus(GENPC_DWARVEN) || player_genus(GENPC_DRACONIAN) )
          return false;
        if ( player_descriptor(PDSC_UNDEAD) )
          return false;

        switch (speci)
        {
          case SP_CENTAUR:
          case SP_GNOME:
          case SP_HALFLING:
          case SP_MINOTAUR:
          case SP_OGRE:
          case SP_OGRE_MAGE:
          case SP_TROLL:
            return false;
        }
        return true;

      case JOB_MONK:
        if ( player_descriptor(PDSC_UNDEAD) )
          return false;

        switch (speci)
        {
          case SP_CENTAUR:
          case SP_GNOME:
          case SP_HILL_DWARF:
          case SP_KOBOLD:
          case SP_NAGA:
          case SP_OGRE:
          case SP_OGRE_MAGE:
          case SP_SPRIGGAN:
          case SP_TROLL:
            return false;
        }
        return true;

      case JOB_WARPER:
        if ( player_genus(GENPC_DRACONIAN) )
          return false;
        if ( player_descriptor(PDSC_UNDEAD) )
          return false;

        switch (speci)
        {
          case SP_GNOME:
          case SP_HILL_DWARF:
          case SP_HILL_ORC:
          case SP_KENKU:
          case SP_KOBOLD:
          case SP_MINOTAUR:
          case SP_OGRE:
          case SP_TROLL:
            return false;
        }
        return true;

      default:
        return false;
    }

}          // end class_allowed()




void choose_weapon( void )
{

    unsigned char keyin = 0;
    int temp_rand;    // probability determination {dlb}

    if ( you.char_class == JOB_CHAOS_KNIGHT )
    {

        temp_rand = random2(4);

        you.inv_type[0] = ( (temp_rand == 0) ? WPN_SHORT_SWORD :
                            (temp_rand == 1) ? WPN_MACE :
                            (temp_rand == 2) ? WPN_HAND_AXE
                                             : WPN_SPEAR );
        return;
    }

    clrscr();

    cprintf(EOL " You have a choice of weapons:" EOL);

    cprintf("a - short sword" EOL);
    cprintf("b - mace" EOL);
    cprintf("c - hand axe" EOL);
    cprintf("d - spear" EOL);

    if ( you.char_class == JOB_GLADIATOR )
      cprintf("e - trident" EOL);

    cprintf(EOL "Which weapon? ");

getkey:
    keyin = get_ch();
    switch ( keyin )
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
        if ( you.char_class == JOB_GLADIATOR )
          {
            you.inv_type[0] = WPN_TRIDENT;
            return;
          }
      // intentional fall through
      default:
        goto getkey;
    }

}




void init_player( void )
{

    unsigned char i = 0;                  // loop variable

    you.berserk_penalty = 0;
    you.berserker = 0;
    you.conf = 0;
    you.confusing_touch = 0;
    you.deaths_door = 0;
    you.disease = 0;
    you.elapsed_time = 0;
    you.exhausted = 0;
    you.haste = 0;
    you.invis = 0;
    you.levitation = 0;
    you.might = 0;
    you.paralysis = 0;
    you.poison = 0;
    you.rotting = 0;
    you.shock_shield = 0;
    you.slow = 0;
    you.special_wield = SPWLD_NONE;
    you.sure_blade = 0;
    you.synch_time = 0;

    set_hp(0, true);
    set_mp(0, true);
    you.magic_points_regeneration = 0;
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

    you.hunger = 6000;
    you.hunger_state = HS_SATIATED;

    you.gold = 0;
    you.speed = 10;             // 0.75;

    you.burden = 0;
    you.burden_state = BS_UNENCUMBERED;

    you.num_inv_items = 0;
    you.spell_no = 0;

    you.your_level = 0;
    you.level_type = LEVEL_DUNGEON;
    you.where_are_you = BRANCH_MAIN_DUNGEON;
    you.char_direction = DIR_DESCENDING;

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

    you.gift_timeout = 0;

    for (i = 0; i < 100; i++)
      you.penance[i] = 0;

    strcpy(ghost.name, "");

    for (i = 0; i < 20; i++)
      ghost.values[i] = 0;

    for (i = EQ_WEAPON; i < NUM_EQUIP; i++)
      you.equip[i] = -1;

    for (i = 0; i < 25; i++)
      you.spells[i] = SPELL_NO_SPELL;

    for (i = 0; i < 100; i++)
      you.mutation[i] = 0;

    for (i = 0; i < 100; i++)
      you.demon_pow[i] = 0;

    for (i = 0; i < 50; i++)
      you.had_item[i] = 0;

    for (i = 0; i < 50; i++)
      you.unique_items[i] = 0;

    for (i = 0; i < NO_UNRANDARTS; i++)
      set_unrandart_exist(i, 0);

    for (i = 0; i < 50; i++)
    {
        you.skills[i] = 0;
        you.skill_points[i] = 0;
        you.practise_skill[i] = 1;
    }

    for (i = 0; i < 30; i++)
      you.attribute[i] = 0;

    for (i = 0; i < ENDOFPACK; i++)
    {
        you.inv_quantity[i] = 0;
        you.inv_ident[i] = 3;
        you.inv_class[i] = OBJ_WEAPONS;
        you.inv_type[i] = WPN_CLUB;
        you.inv_plus[i] = 0;
        you.inv_plus2[i] = 0;
        you.inv_dam[i] = 0;
        you.inv_colour[i] = 0;
    }

    for (i = 0; i < NUM_DURATIONS; i++)
      you.duration[i] = 0;

}




void give_last_paycheck( int which_job )
{

    switch ( which_job )
    {
      case JOB_HEALER:
      case JOB_THIEF:
        you.gold = random2avg(100, 2);  // normalized with random2avg 23jan2000 {dlb}
        break;
      default:
        you.gold = random2(10);
        break;
      case JOB_MONK:
        you.gold = 0;
        break;
    }

    return;

}




// requires stuff::modify_stats() and works because
// stats zeroed out by newgame::init_player()... recall
// that demonspawn & demingods get more later on {dlb}
void species_stat_init( unsigned char which_species )
{

    int strength_base = 0;
    int intellect_base = 0;
    int dexterity_base = 0;

    switch ( which_species )      // strength
    {
      case SP_TROLL:
        strength_base = 13;
        break;
      case SP_OGRE:
        strength_base = 12;
        break;
      case SP_MINOTAUR:
        strength_base = 10;
        break;
      case SP_HILL_DWARF:
      case SP_MOUNTAIN_DWARF:
      case SP_HILL_ORC:
      case SP_OGRE_MAGE:
        strength_base = 9;
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
        strength_base = 9;
        break;
      case SP_NAGA:
      case SP_CENTAUR:
        strength_base = 8;
        break;
      case SP_MUMMY:
      case SP_DEMIGOD:
      case SP_GHOUL:
        strength_base = 7;
        break;
      case SP_ELF:
      case SP_HIGH_ELF:
      case SP_SLUDGE_ELF:
      case SP_GNOME:
      case SP_KENKU:
        strength_base = 5;
        break;
      case SP_GREY_ELF:
      case SP_DEEP_ELF:
      case SP_HALFLING:
      case SP_KOBOLD:
      case SP_DEMONSPAWN:
        strength_base = 4;
        break;
      case SP_SPRIGGAN:
        strength_base = 3;
        break;
      default:                    // SP_HUMAN {dlb}
        strength_base = 6;
        break;
    }

    switch ( which_species )      // intellect
    {
      case SP_DEEP_ELF:
        intellect_base = 10;
        break;
      case SP_HIGH_ELF:
      case SP_GREY_ELF:
        intellect_base = 9;
        break;
      case SP_ELF:
        intellect_base = 8;
        break;
      case SP_SLUDGE_ELF:
      case SP_OGRE_MAGE:
      case SP_DEMIGOD:
        intellect_base = 7;
        break;
      case SP_MOUNTAIN_DWARF:
      case SP_KOBOLD:
      case SP_CENTAUR:
      case SP_DEMONSPAWN:
        intellect_base = 4;
        break;
      case SP_HILL_DWARF:
      case SP_HILL_ORC:
      case SP_MUMMY:
      case SP_OGRE:
      case SP_TROLL:
      case SP_MINOTAUR:
        intellect_base = 3;
        break;
      case SP_GHOUL:
        intellect_base = 1;
        break;
      default:                    // anything unlisted {dlb}
        intellect_base = 6;
        break;
    }

    switch ( which_species )      // dexterity
    {
      case SP_HALFLING:
        dexterity_base = 9;
        break;
      case SP_ELF:
      case SP_HIGH_ELF:
      case SP_GREY_ELF:
      case SP_SLUDGE_ELF:
      case SP_SPRIGGAN:
        dexterity_base = 8;
        break;
      case SP_DEEP_ELF:
      case SP_GNOME:
      case SP_DEMIGOD:
      case SP_KENKU:
        dexterity_base = 7;
        break;
      case SP_MOUNTAIN_DWARF:
      case SP_CENTAUR:
      case SP_MINOTAUR:
        dexterity_base = 5;
        break;
      case SP_HILL_DWARF:
      case SP_HILL_ORC:
      case SP_NAGA:
      case SP_DEMONSPAWN:
        dexterity_base = 4;
        break;
      case SP_MUMMY:
      case SP_OGRE_MAGE:
        dexterity_base = 3;
        break;
// draconians
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
// draconians
      case SP_GHOUL:
        dexterity_base = 2;
        break;
      case SP_OGRE:
        dexterity_base = 1;
        break;
      case SP_TROLL:
        dexterity_base = 0;
        break;
      default:                    // SP_HUMAN and SP_KOBOLD {dlb}
        dexterity_base = 6;
        break;
    }

    modify_stats(strength_base, intellect_base, dexterity_base);

    return;

}




void jobs_stat_init( int which_job )
{

    int strength_mod = 0;
    int intellect_mod = 0;
    int dexterity_mod = 0;

    switch ( which_job )
    {
      case JOB_FIGHTER:
      case JOB_BERSERKER:
        strength_mod = 7;
        break;
      case JOB_GLADIATOR:
      case JOB_PALADIN:
        strength_mod = 6;
        break;
      case JOB_PRIEST:
      case JOB_CRUSADER:
      case JOB_DEATH_KNIGHT:
      case JOB_CHAOS_KNIGHT:
      case JOB_HEALER:
      case JOB_REAVER:
        strength_mod = 4;
        break;
      case JOB_THIEF:
      case JOB_HUNTER:
      case JOB_MONK:
        strength_mod = 3;
        break;
      case JOB_ASSASSIN:
      case JOB_STALKER:
        strength_mod = 2;
        break;
      default:
        strength_mod = 0;
        break;
    }

    switch ( which_job )
    {
      case JOB_WIZARD:
        intellect_mod = 7;
        break;
      case JOB_NECROMANCER:
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
        intellect_mod = 6;
        break;
      case JOB_PRIEST:
      case JOB_HEALER:
      case JOB_REAVER:
        intellect_mod = 4;
        break;
      case JOB_HUNTER:
      case JOB_CRUSADER:
      case JOB_DEATH_KNIGHT:
      case JOB_CHAOS_KNIGHT:
      case JOB_STALKER:
        intellect_mod = 3;
        break;
      case JOB_THIEF:
      case JOB_PALADIN:
      case JOB_ASSASSIN:
      case JOB_MONK:
        intellect_mod = 2;
        break;
      default:
        intellect_mod = 0;
        break;
      case JOB_BERSERKER:
        intellect_mod = -1;
        break;
    }

    switch ( which_job )
    {
      case JOB_ASSASSIN:
        dexterity_mod = 6;
        break;
      case JOB_THIEF:
      case JOB_STALKER:
      case JOB_MONK:
        dexterity_mod = 5;
        break;
      case JOB_GLADIATOR:
      case JOB_NECROMANCER:
      case JOB_BERSERKER:
      case JOB_HUNTER:
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
        dexterity_mod = 4;
        break;
      case JOB_FIGHTER:
      case JOB_WIZARD:
      case JOB_CRUSADER:
      case JOB_DEATH_KNIGHT:
      case JOB_CHAOS_KNIGHT:
        dexterity_mod = 3;
        break;
      case JOB_PRIEST:
      case JOB_PALADIN:
      case JOB_HEALER:
      case JOB_REAVER:
        dexterity_mod = 2;
        break;
      default:
        dexterity_mod = 0;
        break;
    }

    modify_stats(strength_mod, intellect_mod, dexterity_mod);

    return;

}




void jobs_hpmp_init( int which_job )
{

    int job_hp_base = 0;
    int job_mp_base = 0;

    switch ( which_job )
    {
      case JOB_FIGHTER:
      case JOB_BERSERKER:
      case JOB_HEALER:
        job_hp_base = 15;
        break;
      case JOB_GLADIATOR:
      case JOB_PALADIN:
        job_hp_base = 14;
        break;
      case JOB_HUNTER:
      case JOB_CRUSADER:
      case JOB_DEATH_KNIGHT:
      case JOB_CHAOS_KNIGHT:
      case JOB_REAVER:
      case JOB_MONK:
        job_hp_base = 13;
        break;
      case JOB_PRIEST:
      case JOB_ASSASSIN:
        job_hp_base = 12;
        break;
      case JOB_THIEF:
      case JOB_STALKER:
        job_hp_base = 11;
        break;
      case JOB_WIZARD:
      case JOB_NECROMANCER:
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
        job_hp_base = 10;
        break;
      default:
        job_hp_base = 0;
        break;
    }

    switch ( which_job )
    {
      case JOB_WIZARD:
      case JOB_NECROMANCER:
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
        job_mp_base = 3;
        break;
      case JOB_PRIEST:
      case JOB_CRUSADER:
      case JOB_DEATH_KNIGHT:
      case JOB_CHAOS_KNIGHT:
      case JOB_HEALER:
      case JOB_REAVER:
      case JOB_STALKER:
        job_mp_base = 1;
        break;
      default:
        job_mp_base = 0;    // anything unlisted {dlb}
        break;
    }

    set_hp(job_hp_base, true);
    set_mp(job_mp_base, true);

    return;

}




void give_basic_knowledge( int which_job )
{

    switch ( which_job )
    {
        case JOB_PRIEST:
        case JOB_PALADIN:
          set_id(OBJ_POTIONS, POT_HEALING, 1);
          break;

        case JOB_ASSASSIN:
        case JOB_STALKER:
        case JOB_VENOM_MAGE:
          set_id(OBJ_POTIONS, POT_POISON, 1);
          break;

        default:
          break;
    }

    return;

}          // end give_basic_knowledge()




void give_basic_spells( int which_job )
{
    unsigned char which_spell = SPELL_NO_SPELL;

    switch ( which_job )
    {
         case JOB_CONJURER:
         case JOB_REAVER:
         case JOB_WIZARD:
           which_spell = SPELL_MAGIC_DART;
           break;
         case JOB_STALKER:
         case JOB_VENOM_MAGE:
           which_spell = SPELL_STING;
           break;
         case JOB_SUMMONER:
           which_spell = SPELL_SUMMON_SMALL_MAMMAL ;
           break;
         case JOB_ICE_ELEMENTALIST:
           which_spell = SPELL_FREEZE;
           break;
         case JOB_TRANSMUTER:
           which_spell = SPELL_DISRUPT;
           break;
#ifdef USE_WARPER_SPELL_BEND
         case JOB_WARPER:
           which_spell = SPELL_BEND;
           break;
#endif
         case JOB_NECROMANCER:
           which_spell = SPELL_PAIN;
           break;
         case JOB_ENCHANTER:
           which_spell = SPELL_BACKLIGHT;
           break;
         case JOB_FIRE_ELEMENTALIST:
           which_spell = SPELL_FLAME_TONGUE;
           break;
         case JOB_AIR_ELEMENTALIST:
           which_spell = SPELL_SHOCK;
           break;
         case JOB_EARTH_ELEMENTALIST:
           which_spell = SPELL_SANDBLAST;
           break;
         case JOB_DEATH_KNIGHT:
           if ( you.species == SP_DEMIGOD || you.religion != GOD_YREDELEMNUL )
             which_spell = SPELL_PAIN;
           break;
         default:
           break;
    }

    you.spells[0] = which_spell;
    you.spell_no = ( (you.spells[0] != SPELL_NO_SPELL) ? 1 : 0 );
    return;
}          // end give_basic_spells()



/* ************************************************************************

// MAKE INTO FUNCTION!!! {dlb}
// randomly boost stats a number of times based on species {dlb}
    unsigned char points_left = ( you.species == SP_DEMIGOD || you.species == SP_DEMONSPAWN ) ? 15 : 8;

    do
    {
        switch ( random2(NUM_STATS) )
        {
          case STAT_STRENGTH:
            if ( you.strength > 17 && coinflip() )
              continue;
            you.strength++;
            break;
          case STAT_DEXTERITY:
            if ( you.dex > 17 && coinflip() )
              continue;
            you.dex++;
            break;
          case STAT_INTELLIGENCE:
            if ( you.intel > 17 && coinflip() )
              continue;
            you.intel++;
            break;
        }
        points_left--;
    }
    while (points_left > 0);

************************************************************************ */




// eventually, this should be something more grand {dlb}
void openingScreen( void )
{

/* **********************************************
// this does not work just yet ... {dlb}:
    cprintf(EOL "Hello, ");

    if ( you.your_name[0] != '\0' )
    {
       cprintf(you.your_name);        // better be less than 31 characters :P {dlb}
                                      // of course, invalid names will appear {dlb}
       cprintf(", ");
    }
********************************************** */

    cprintf("Hello, welcome to Dungeon Crawl " VERSION "!");
    cprintf(EOL "(c) Copyright 1997-2000 Linley Henzell");
    cprintf(EOL "Please consult crawl.txt for instructions and legal details." EOL);

    return;

}          // end openingScreen()




void enterPlayerName( void )
{

    char name_entered[kNameLen];     // temporary 'til copyover to you.your_name {dlb}
    bool acceptable_name = false;    // anything to avoid goto statements {dlb}

// first time -- names set through init.txt/environment assumed ok {dlb}
    if ( you.your_name[0] != '\0' )
      acceptable_name = true;

    do
    {

    // prompt for a new name if current one unsatisfactory {dlb}:
        if ( !acceptable_name )
        {
            cprintf(EOL "What is your name today? ");

#if defined(LINUX)
            echo();
            getstr(name_entered);
            noecho();
#elif defined(MAC) || defined(WIN32CONSOLE)
            getstr(name_entered, sizeof(name_entered));
#else
            gets(name_entered);
#endif

            strncpy(you.your_name, name_entered, kNameLen);
        }

    // verification begins here {dlb}:
        if ( you.your_name[0] == '\0' )
        {
            cprintf(EOL "That's a silly name!" EOL);
            acceptable_name = false;
        }

// if SAVE_DIR_PATH is defined, userid will be tacked onto the end
// of each character's files, making bones a valid player name.
#ifndef SAVE_DIR_PATH

    // this would cause big probs with ghosts    // what would? {dlb}

        else if ( strcmp(you.your_name, "bones") == 0 || strlen(you.your_name) == 0 )
        {
            cprintf(EOL "That's a silly name!" EOL);
            acceptable_name = false;
        }

#endif

        else
          acceptable_name = verifyPlayerName();

    }
    while ( !acceptable_name );


}          // end enterPlayerName()




bool verifyPlayerName( void )
{

    unsigned int i;     // loop variable

    for (i = 0; i < strlen(you.your_name); i++)
    {

    // This is an easy way to avoid "bad" characters
    // doesn't this clobber the Mac checking below? {dlb}
        if ( !isalnum(you.your_name[i]) )
        {
            cprintf(EOL "No non-alphanumerics, please." EOL);
            return false;
        }

// colon is Mac path seperator
// $$$ shouldn't DOS and Unix path seperators be illegal?
#ifdef MAC

        else if ( you.your_name[i] == ':' )
        {
            cprintf(EOL "No colons, please." EOL);
            return false;
        }

#endif

        else if ( you.your_name[i] == '?' )
        {
            cprintf(EOL "No question marks, please." EOL);
            return false;
        }

    }

    return true;

}          // end verifyPlayerName()

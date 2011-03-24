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
 *      <4>      6/13/99        BWR   SysEnv support
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
#include <time.h>

#ifdef DOS
#include <conio.h>
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
#endif

#ifdef OS9
#include <stat.h>
#else
#include <sys/stat.h>
#endif

#include "globals.h"
#include "externs.h"

#include "abl-show.h"
#include "dungeon.h"
#include "files.h"
#include "fight.h"
#include "itemname.h"
#include "itemprop.h"
#include "items.h"
#include "macro.h"
#include "player.h"
#include "randart.h"
#include "skills.h"
#include "skills2.h"
#include "spl-util.h"
#include "stuff.h"
#include "version.h"

// stats are bought up to this level before random are assigned
#define MIN_START_STAT       3

// these two are used in the debug code:
bool class_allowed( unsigned char speci, int char_class,
                    bool check_valid = false );
void init_player( void );

static void opening_screen( void );
static void enter_player_name( bool blankOK );
static bool verify_player_name( void );
static void choose_weapon( void );
static bool choose_race( void );
static bool choose_class( void );

static void give_basic_knowledge( void );
static void give_basic_spells( void );
static void give_last_paycheck( void );
static void init_race_class_stats( void );
static void jobs_stat_init( void );
static void species_stat_init( void );
static void give_items_skills( void );

static void create_wanderer(void);
static bool give_wanderer_weapon( int slot, int wpn_skill );
static void give_random_potion( int slot );
static void give_random_secondary_armour( int slot );

#if 0
// currently unused -- bwr
static void give_random_wand( int slot );
static void give_random_scroll( int slot );
#endif


int give_first_conjuration_book( void )
{
    // magnifying the skill variance and adding a small constant (50):
    const int fire_earth  = (species_skills( SK_FIRE_MAGIC ) - 100) * 3
                            + (species_skills( SK_EARTH_MAGIC ) - 100) * 3
                            + 250;

    const int ice_air     = (species_skills( SK_ICE_MAGIC ) - 100) * 3
                            + (species_skills( SK_AIR_MAGIC ) - 100) * 3
                            + 250;

    // random weighted chance of getting either book:
    return ((random2(fire_earth) <= random2(ice_air)) ? BOOK_CONJURATIONS_I
                                                      : BOOK_CONJURATIONS_II);
}

static void pick_random_species_and_class( void )
{
    //
    // We pick both species and class at the same time to give each
    // valid possibility a fair chance.  For proof that this will
    // work correctly see the proof in religion.cc:handle_god_time().
    //
    int job_count = 0;

    int species = SP_UNKNOWN;
    int job = JOB_UNKNOWN;

    // for each valid (species, class) choose one randomly
    for (int sp = SP_HUMAN; sp < NUM_SPECIES; sp++)
    {
        // we only want draconians counted once in this loop...
        // we'll add the variety lower down -- bwr
        if (sp >= SP_WHITE_DRACONIAN && sp <= SP_BASE_DRACONIAN)
            continue;

        for (int cl = JOB_FIGHTER; cl < NUM_JOBS; cl++)
        {
            if (class_allowed( sp, cl, true ))
            {
                job_count++;
                if (one_chance_in( job_count ))
                {
                    species = sp;
                    job = cl;
                }
            }
        }
    }

    // at least one job must exist in the game else we're in big trouble
    ASSERT( species != SP_UNKNOWN && job != JOB_UNKNOWN );

    // return draconian variety here
    if (species == SP_RED_DRACONIAN)
        you.species = SP_RED_DRACONIAN + random2(9);
    else
        you.species = species;

    you.char_class = job;
}

static bool check_saved_game(void)
{
    FILE *handle;
    char char_fil[kFileNameSize];

#ifdef LOAD_UNPACKAGE_CMD
    // Create the file name base
    char name_buff[kFileNameLen];

    snprintf( name_buff, sizeof(name_buff),
              SAVE_DIR_PATH "%s-%d", you.your_name,
              static_cast<int>( getuid() ) );

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

        snprintf( cmd_buff, sizeof(cmd_buff), LOAD_UNPACKAGE_CMD, name_buff );

        // XXX: this should be replaced by a proper fork/exec
        if (system( cmd_buff ) != 0)
        {
            cprintf( EOL "Warning: Zip command (LOAD_UNPACKAGE_CMD) returned non-zero value!" EOL );
        }

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
#ifdef SAVE_DIR_PATH
    snprintf( char_fil, sizeof(char_fil),
              SAVE_DIR_PATH "%s-%d", you.your_name, static_cast<int>(getuid()) );
#else
    strcpy(char_fil, "");
    strncat(char_fil, you.your_name, kFileNameLen);
#endif
    strcat(char_fil, ".sav");
#endif

    handle = fopen(char_fil, "rb+");

    if (handle != NULL)
    {
        fclose(handle);
        return true;
    }
    return false;
}

bool new_game(void)
{
    int i, j;                   // loop variables {dlb}

    //jmf: NEW ASSERTS: we ought to do a *lot* of these
    ASSERT(NUM_SPELLS < SPELL_NO_SPELL);
    ASSERT(NUM_JOBS < JOB_UNKNOWN);
    ASSERT(NUM_DURATIONS >= 30);
    ASSERT(NUM_ATTRIBUTES >= 30);

#ifndef DEBUG_DUMP_SKILL_TOTALS
    init_player();
    textcolor(LIGHTGREY);

    // copy name into you.your_name if set from environment --
    // note that you.your_name could already be set from init.txt
    // this, clearly, will overwrite such information {dlb}
    if (SysEnv.crawl_name)
    {
        strncpy( you.your_name, SysEnv.crawl_name, kNameLen );
        you.your_name[ kNameLen - 1 ] = '\0';
    }

    opening_screen();
    enter_player_name( true );

    if (you.your_name[0] != '\0')
    {
        if (check_saved_game())
        {
            textcolor( BROWN );
            cprintf( EOL "Welcome back, " );
            textcolor( YELLOW );
            cprintf( you.your_name );
            cprintf( "!" );
            textcolor( LIGHTGREY );

            return (false);
        }
    }

    if (Options.random_pick)
        pick_random_species_and_class();
    else
    {
        while (choose_race() && !choose_class())
            ;
    }

    strcpy( you.class_name, get_class_name( you.char_class ) );

    // new: pick name _after_ race and class choices
    if (you.your_name[0] == '\0')
    {
        clrscr();

        char spec_buff[80];
        strncpy( spec_buff, species_name( you.species ), 80 );

        snprintf( info, INFO_SIZE, "You are a%s %s %s." EOL,
                  (is_vowel( spec_buff[0] )) ? "n" : "", spec_buff,
                  you.class_name );

        cprintf( info );

        enter_player_name( false );

        if (check_saved_game())
        {
            cprintf(EOL "Do you really want to overwrite your old game?");
            char c = getch();
            if (!(c == 'Y' || c == 'y'))
            {
                textcolor( BROWN );
                cprintf(EOL EOL "Welcome back, ");
                textcolor( YELLOW );
                cprintf(you.your_name);
                cprintf("!");
                textcolor( LIGHTGREY );

                return (false);
            }
        }
    }
#endif


// ************ round-out character statistics and such ************

    you.is_undead = ((you.species == SP_MUMMY) ? US_UNDEAD :
                     (you.species == SP_GHOUL) ? US_HUNGRY_DEAD
                                               : US_ALIVE);

    // before we get into the inventory init,  set light radius based
    // on species vision. currently,  all species see out to 8 squares.
    you.normal_vision = VISION_RANGE_DEFAULT;
    you.current_vision = VISION_RANGE_DEFAULT;

    init_race_class_stats();

    you.exp_available = 25;

    // this function depends on stats being finalized
    give_items_skills();
    give_last_paycheck();

    if (!you.is_undead)
    {
        for (i = 0; i < ENDOFPACK; i++)
        {
            if (!you.inv[i].quantity)
            {
                you.inv[i].quantity = 1;
                you.inv[i].base_type = OBJ_FOOD;
                you.inv[i].sub_type = FOOD_BREAD_RATION;

                if (you.species == SP_HILL_ORC || you.species == SP_KOBOLD
                    || you.species == SP_OGRE || you.species == SP_TROLL
                    || you.species == SP_GIANT)
                {
                    you.inv[i].sub_type = FOOD_MEAT_RATION;
                }

                you.inv[i].colour = BROWN;
                break;
            }
        }
    }

    // must remember to check for already existing colours/combinations
    for (i = 0; i < NUM_IDESC; i++)
    {
        for (j = 0; j < MAX_SUBTYPES; j++)
        {
            you.item_description[i][j] = -1;
        }
    }

    you.item_description[IDESC_POTIONS][POT_PORRIDGE] = 153;  // "gluggy white"
    you.item_description[IDESC_POTIONS][POT_WATER] = 0;       // "clear"

    int passout;

    for (i = 0; i < NUM_IDESC; i++)
    {
        for (j = 0; j < MAX_SUBTYPES; j++)
        {
            if (you.item_description[i][j] != -1)
                continue;

            do
            {
                passout = 1;

                switch (i)
                {
                case IDESC_WANDS: // wands
                    you.item_description[i][j] = random2( NUM_WANDS_ADJ * NUM_WANDS_MAT );
                    if (one_chance_in(3))
                        you.item_description[i][j] %= NUM_WANDS_MAT;
                    break;

                case IDESC_POTIONS: // potions
                    you.item_description[i][j] = random2( NUM_POTIONS_ADJ * NUM_POTIONS_MAT );
                    if (one_chance_in(5))
                        you.item_description[i][j] %= NUM_POTIONS_MAT;
                    break;

                case IDESC_SCROLLS: // scrolls
                    // seeds for name creation:
                    you.item_description[i][j] = random_int();
                    break;

                case IDESC_RINGS: // rings
                    you.item_description[i][j] = random2( NUM_RINGS_ADJ * NUM_RINGS_MAT );
                    if (one_chance_in(3))
                        you.item_description[i][j] %= NUM_RINGS_MAT;
                    break;
                }

                // don't have p < j because some are preassigned
                for (int p = 0; p < MAX_SUBTYPES; p++)
                {
                    if (you.item_description[i][p] == you.item_description[i][j]
                        && j != p)
                    {
                        passout = 0;
                    }
                }
            }
            while (passout == 0);
        }
    }

    for (i = 0; i < NUM_SKILLS; i++)
    {
        if (!you.skills[i])
            continue;

        // Start with the amount of skill points required for a human...
        const int points = skill_exp_needed( you.skills[i] + 1 );

        you.skill_points[i] = points + 1;

        if (i == SK_SPELLCASTING)
            you.skill_points[i] = (points * 130) / 100 + 1;
        else if (i == SK_INVOCATIONS || i == SK_EVOCATIONS)
            you.skill_points[i] = (points * 75) / 100 + 1;

        // ...and find out what level that earns this character.
        const int sp_diff = species_skills(i);
        you.skills[i] = 0;

        for (int lvl = 1; lvl <= 8; lvl++)
        {
            const unsigned int needed = (skill_exp_needed( lvl + 1 ) * sp_diff) / 100;

            if (you.skill_points[i] >= needed)
            {
                you.skills[i] = lvl;
                continue;
            }

            // Non-wanderering spellcasters we guarantee the first
            // level of spellcasting (perhaps we should somehow take
            // the skill points from somewhere else to pay for this).
            // This explains why they could learn their starting spell,
            // as well as providing the key skill to being able to
            // actually cast it.
            if (i == SK_SPELLCASTING && lvl == 1
                && you.char_class != JOB_WANDERER)
            {
                you.skills[i] = 1;
                you.skill_points[i] = needed;
            }
            break;
        }
    }

    calc_total_skill_points();

    for (i = 0; i < ENDOFPACK; i++)
    {
        if (!is_valid_item( you.inv[i] ))
            continue;

        if (you.inv[i].base_type != OBJ_WEAPONS)
        {
            set_ident_type( you.inv[i].base_type,
                            you.inv[i].sub_type, ID_KNOWN_TYPE );
        }

        if (you.inv[i].base_type == OBJ_POTIONS
            || you.inv[i].base_type == OBJ_WANDS
            || you.inv[i].base_type == OBJ_JEWELLERY)
        {
            item_colour( you.inv[i] );  // set correct special and colour
        }

        if (you.inv[i].base_type == OBJ_BOOKS)
        {
            you.had_book[you.inv[i].sub_type] = 1;
            if (you.inv[i].sub_type == BOOK_MINOR_MAGIC_I
                || you.inv[i].sub_type == BOOK_MINOR_MAGIC_II
                || you.inv[i].sub_type == BOOK_MINOR_MAGIC_III)
            {
                you.had_book[BOOK_MINOR_MAGIC_I] = 1;
                you.had_book[BOOK_MINOR_MAGIC_II] = 1;
                you.had_book[BOOK_MINOR_MAGIC_III] = 1;
            }
            if (you.inv[i].sub_type == BOOK_CONJURATIONS_I
                || you.inv[i].sub_type == BOOK_CONJURATIONS_II)
            {
                you.had_book[BOOK_CONJURATIONS_I] = 1;
                you.had_book[BOOK_CONJURATIONS_II] = 1;
            }
        }

        // don't change object type modifier unless it starts plain
        if (you.inv[i].base_type <= OBJ_ARMOUR
            && get_equip_race( you.inv[i] ) == ISFLAG_NO_RACE)
        {
            // now add appropriate species type mod:
            switch (you.species)
            {
            case SP_ELF:
            case SP_HIGH_ELF:
            case SP_GREY_ELF:
            case SP_DEEP_ELF:
            case SP_SLUDGE_ELF:
                set_equip_race( you.inv[i], ISFLAG_ELVEN );
                break;

            case SP_HILL_DWARF:
            case SP_MOUNTAIN_DWARF:
                set_equip_race( you.inv[i], ISFLAG_DWARVEN );
                if (you.inv[i].colour == LIGHTCYAN)
                    you.inv[i].colour = CYAN;
                break;

            case SP_HILL_ORC:
                set_equip_race( you.inv[i], ISFLAG_ORCISH );
                break;
            }
        }
    }

    // we calculate hp and mp here;  all relevant factors should be
    // finalized by now (GDL)
    calc_hp_max();
    calc_mp_max();

    // make sure the starting player is fully charged up
    set_hp( you.hp_max, false );
    set_mp( you.max_magic_points, false );

    give_basic_spells();
    give_basic_knowledge();

    // tmpfile purging removed in favour of marking
    for (int lvl = 0; lvl < MAX_LEVELS; lvl++)
    {
        for (int dng = 0; dng < MAX_BRANCHES; dng++)
        {
            env.level_files[lvl][dng] = false;
        }
    }

    // places staircases to the branch levels:
    for (i = 0; i < 30; i++)
    {
        you.branch_stairs[i] = 100;
    }

    you.branch_stairs[STAIRS_ECUMENICAL_TEMPLE] = 3 + random2(4); // avg:  4.5

    you.branch_stairs[STAIRS_ORCISH_MINES] = 5 + random2(6);    // avg:  7.5

    you.branch_stairs[STAIRS_ELVEN_HALLS] =
        you.branch_stairs[STAIRS_ORCISH_MINES] + (coinflip() ? 4 : 3);  // 11.0

    you.branch_stairs[STAIRS_LAIR] = 7 + random2(6);    // avg:  9.5

    you.branch_stairs[STAIRS_HIVE] = 10 + random2(6);   // avg: 12.5

    you.branch_stairs[STAIRS_SLIME_PITS] =
        you.branch_stairs[STAIRS_LAIR] + 3 + random2(4);        // avg: 14.0

    you.branch_stairs[STAIRS_SWAMP] =
        you.branch_stairs[STAIRS_LAIR] + 2 + random2(6);        // avg: 14.0

    you.branch_stairs[STAIRS_SNAKE_PIT] =
        you.branch_stairs[STAIRS_LAIR] + (coinflip() ? 7 : 6);   // avg: 16.0

    you.branch_stairs[STAIRS_VAULTS] = 13 + random2(6); // avg: 15.5

    you.branch_stairs[STAIRS_CRYPT] =
        you.branch_stairs[STAIRS_VAULTS] + 2 + random2(3);      // avg: 18.5

    you.branch_stairs[STAIRS_HALL_OF_BLADES] =
        you.branch_stairs[STAIRS_VAULTS] + 4;   // avg: 19.5

    you.branch_stairs[STAIRS_TOMB] =
        you.branch_stairs[STAIRS_CRYPT] + ((coinflip()) ? 3 : 2);   // avg: 20.0

    you.branch_stairs[STAIRS_HALL_OF_ZOT] = 26; // always 26

    return (true);
}                               // end of new_game()

static bool species_is_undead( unsigned char speci )
{
    return (speci == SP_MUMMY || speci == SP_GHOUL);
}

bool class_allowed( unsigned char speci, int char_class, bool check_valid )
{
    // Quitters are never allowed, unknown species/class is either
    // always allowed or never allowed depending on whether the
    // combo must be completely specified (valid).
    if (char_class == JOB_QUITTER)
        return (false);
    else if (speci == SP_UNKNOWN || char_class == JOB_UNKNOWN)
        return (!check_valid);

    switch (char_class)
    {
    case JOB_FIGHTER:
        switch (speci)
        {
        case SP_OGRE_MAGE:
        case SP_SPRIGGAN:
            return (false);
        }
        return (true);

    case JOB_WIZARD:
        if (species_is_undead( speci ))
            return (false);

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
        case SP_GIANT:
            return (false);
        }
        return (true);

    case JOB_PRIEST:
        if (player_genus(GENPC_DRACONIAN, speci))
            return (false);
        if (species_is_undead( speci ))
            return (false);

        switch (speci)
        {
        case SP_CENTAUR:
        case SP_DEMIGOD:
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
        case SP_GIANT:
            return (false);
        }
        return (true);

    case JOB_THIEF:
        if (species_is_undead( speci ))
            return (false);

        switch (speci)
        {
        case SP_CENTAUR:
        case SP_KENKU:
        case SP_MINOTAUR:
        case SP_OGRE:
        case SP_OGRE_MAGE:
        case SP_SPRIGGAN:
        case SP_TROLL:
        case SP_GIANT:
            return (false);
        }
        return (true);

    case JOB_GLADIATOR:
        if (player_genus(GENPC_ELVEN, speci))
            return (false);
        if (species_is_undead( speci ))
            return (false);

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
        case SP_GIANT:
            return (false);
        }
        return (true);

    case JOB_NECROMANCER:
        if (player_genus(GENPC_DWARVEN, speci))
            return (false);
        if (player_genus(GENPC_DRACONIAN, speci))
            return (false);

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
        case SP_GIANT:
        case SP_MERFOLK:
            return (false);
        }
        return (true);

    case JOB_PALADIN:
        switch (speci)
        {
        case SP_HUMAN:
        case SP_MOUNTAIN_DWARF:
        case SP_HIGH_ELF:
            return (true);
        }
        return (false);

    case JOB_ASSASSIN:
        if (player_genus(GENPC_DWARVEN, speci))
            return (false);
        if (player_genus(GENPC_DRACONIAN, speci))
            return (false);

        switch (speci)
        {
        case SP_CENTAUR:
        case SP_GHOUL:
        case SP_GNOME:
        case SP_MINOTAUR:
        case SP_OGRE:
        case SP_OGRE_MAGE:
        case SP_TROLL:
        case SP_GIANT:
            return (false);
        }
        return (true);

    case JOB_BERSERKER:
        if (player_genus(GENPC_ELVEN, speci))
            return (false);
        if (player_genus(GENPC_DRACONIAN, speci))
            return (false);
        if (species_is_undead( speci ))
            return (false);

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
        case SP_MERFOLK:
        case SP_GIANT:
            return (false);
        }
        return (true);

    case JOB_HUNTER:
        if (player_genus(GENPC_DRACONIAN, speci))   // use bows
            return (true);
        if (player_genus(GENPC_DWARVEN, speci))     // use xbows
            return (true);

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
            // spear
        case SP_MERFOLK:
            return (true);
        }
        return (false);

    case JOB_CONJURER:
        if (species_is_undead( speci ))
            return (false);

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
        case SP_GIANT:
        case SP_MERFOLK:
        case SP_SLUDGE_ELF:
            return (false);
        }
        return (true);

    case JOB_ENCHANTER:
        if (player_genus(GENPC_DRACONIAN, speci))
            return (false);
        if (species_is_undead( speci ))
            return (false);

        switch (speci)
        {
        case SP_GNOME:
        case SP_HILL_ORC:
        case SP_KENKU:
        case SP_KOBOLD:
        case SP_MINOTAUR:
        case SP_OGRE:
        case SP_TROLL:
        case SP_GIANT:
        case SP_SLUDGE_ELF:
            return (false);
        }
        return (true);

    case JOB_FIRE_ELEMENTALIST:
        if (player_genus(GENPC_DRACONIAN, speci))
            return (false);
        if (species_is_undead( speci ))
            return (false);

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
        case SP_GIANT:
        case SP_MERFOLK:
            return (false);
        }
        return (true);

    case JOB_ICE_ELEMENTALIST:
        if (player_genus(GENPC_DWARVEN, speci))
            return (false);
        if (player_genus(GENPC_DRACONIAN, speci))
            return (false);
        if (species_is_undead( speci ))
            return (false);

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
        case SP_GIANT:
            return (false);
        }
        return (true);

    case JOB_SUMMONER:
        if (player_genus(GENPC_DWARVEN, speci))
            return (false);
        if (species_is_undead( speci ))
            return (false);

        switch (speci)
        {
        case SP_CENTAUR:
        case SP_GNOME:
        case SP_HALFLING:
        case SP_MINOTAUR:
        case SP_OGRE:
        case SP_SPRIGGAN:
        case SP_TROLL:
        case SP_GIANT:
            return (false);
        }
        return (true);

    case JOB_AIR_ELEMENTALIST:
        if (player_genus(GENPC_DWARVEN, speci))
            return (false);
        if (player_genus(GENPC_DRACONIAN, speci))
            return (false);
        if (species_is_undead( speci ))
            return (false);

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
        case SP_GIANT:
        case SP_MERFOLK:
            return (false);
        }
        return (true);

    case JOB_EARTH_ELEMENTALIST:
        if (player_genus(GENPC_DRACONIAN, speci))
            return (false);
        if (species_is_undead( speci ))
            return (false);

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
        case SP_GIANT:
        case SP_MERFOLK:
            return (false);
        }
        return (true);

    case JOB_CRUSADER:
        if (player_genus(GENPC_DWARVEN, speci))
            return (false);
        if (species_is_undead( speci ))
            return (false);

        switch (speci)
        {
        case SP_GNOME:
        case SP_KENKU:
        case SP_KOBOLD:
        case SP_MINOTAUR:
        case SP_NAGA:
        case SP_OGRE:
        case SP_SPRIGGAN:
        case SP_TROLL:
        case SP_GIANT:
        case SP_MERFOLK:
        case SP_SLUDGE_ELF:
            return (false);
        }
        return (true);

    case JOB_DEATH_KNIGHT:
        if (player_genus(GENPC_DWARVEN, speci))
            return (false);

        switch (speci)
        {
        case SP_ELF:
        case SP_GHOUL:
        case SP_GNOME:
        case SP_GREY_ELF:
        case SP_HALFLING:
        case SP_HIGH_ELF:
        case SP_MINOTAUR:
        case SP_OGRE:
        case SP_OGRE_MAGE:
        case SP_SPRIGGAN:
        case SP_TROLL:
        case SP_GIANT:
        case SP_MERFOLK:
            return (false);
        }
        return (true);

    case JOB_VENOM_MAGE:
        if (player_genus(GENPC_DWARVEN, speci))
            return (false);
        if (species_is_undead( speci ))
            return (false);

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
        case SP_GIANT:
            return (false);
        }
        return (true);

    case JOB_CHAOS_KNIGHT:
        if (player_genus(GENPC_DWARVEN, speci))
            return (false);
        if (species_is_undead( speci ))
            return (false);

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
        case SP_GIANT:
        case SP_MERFOLK:
            return (false);
        }
        return (true);

    case JOB_TRANSMUTER:
        if (species_is_undead( speci ))
            return (false);

        switch (speci)
        {
        case SP_HALFLING:
        case SP_HILL_DWARF:
        case SP_HILL_ORC:
        case SP_KENKU:
        case SP_MINOTAUR:
        case SP_OGRE:
        case SP_TROLL:
        case SP_GIANT:
            return (false);
        }
        return (true);

    case JOB_HEALER:
        if (player_genus(GENPC_DRACONIAN, speci))
            return (false);
        if (species_is_undead( speci ))
            return (false);

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
        case SP_GIANT:
            return (false);
        }
        return (true);

    case JOB_REAVER:
        if (species_is_undead( speci ))
            return (false);

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
        case SP_GIANT:
        case SP_MERFOLK:
        case SP_SLUDGE_ELF:
            return (false);
        }
        return (true);

    case JOB_STALKER:
        if (player_genus(GENPC_DWARVEN, speci))
            return (false);
        if (player_genus(GENPC_DRACONIAN, speci))
            return (false);
        if (species_is_undead( speci ))
            return (false);

        switch (speci)
        {
        case SP_CENTAUR:
        case SP_GNOME:
        case SP_HALFLING:
        case SP_MINOTAUR:
        case SP_OGRE:
        case SP_OGRE_MAGE:
        case SP_TROLL:
        case SP_GIANT:
            return (false);
        }
        return (true);

    case JOB_MONK:
        if (species_is_undead( speci ))
            return (false);

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
        case SP_GIANT:
            return (false);
        }
        return (true);

    case JOB_WARPER:
        if (player_genus(GENPC_DWARVEN, speci))
            return (false);
        if (player_genus(GENPC_DRACONIAN, speci))
            return (false);
        if (species_is_undead( speci ))
            return (false);

        switch (speci)
        {
        case SP_CENTAUR:
        case SP_GNOME:
        case SP_HILL_ORC:
        case SP_HALFLING:
        case SP_KENKU:
        case SP_MINOTAUR:
        case SP_OGRE:
        case SP_TROLL:
        case SP_GIANT:
        case SP_MERFOLK:
            return (false);
        }
        return (true);

    case JOB_WANDERER:
        switch (speci)
        {
        case SP_HUMAN:
        case SP_DEMIGOD:
        case SP_DEMONSPAWN:
        case SP_GHOUL:          // just testing
            return (true);
        }
        return (false);

    case JOB_QUITTER:   // shouldn't happen since 'x' is handled specially
    default:
        return (false);
    }
}                               // end class_allowed()

static weapon_type  startwep[5] = { WPN_SHORT_SWORD,
                                    WPN_MACE,
                                    WPN_HAND_AXE,
                                    WPN_SPEAR,
                                    WPN_TRIDENT };

// XXX: this screen (and the other 3rd screens) needs prettying up. -- bwr
static void choose_weapon( void )
{
    char wepName[ITEMNAME_SIZE];
    unsigned char keyin = 0;
    int num_choices = 4;

    if (you.char_class == JOB_CHAOS_KNIGHT)
    {
        you.inv[0].sub_type = startwep[ random2(num_choices) ];
        return;
    }

    // gladiators and merfolk can get tridents
    if (you.char_class == JOB_GLADIATOR || you.species == SP_MERFOLK)
        num_choices = 5;

    if (Options.weapon != WPN_UNKNOWN
        && Options.weapon != WPN_RANDOM
        && (Options.weapon != WPN_TRIDENT || num_choices == 5))
    {
        you.inv[0].sub_type = Options.weapon;
        return;
    }

    if (!Options.random_pick && Options.weapon != WPN_RANDOM)
    {
        clrscr();

        textcolor( CYAN );
        cprintf(EOL " You have a choice of weapons:" EOL);
        textcolor( LIGHTGREY );

        for (int i = 0; i < num_choices; i++)
        {
            // int x = effective_stat_bonus( startwep[i] );
            standard_name_weap( startwep[i], wepName );

            // snprintf( info, INFO_SIZE, "%c - %s%s" EOL, 'a' + i, wepName,
            //           (x <= -4) ? " (not ideal)" : "" );

            snprintf( info, INFO_SIZE, "%c - %s" EOL, 'a' + i, wepName );
            cprintf(info);
        }

        cprintf( EOL "? - Random" EOL );

        do
        {
            textcolor( CYAN );
            cprintf(EOL "Which weapon? ");
            textcolor( LIGHTGREY );

            keyin = get_ch();
        }
        while (keyin != '?' && (keyin < 'a' || keyin > ('a' + num_choices)));

        if (keyin != '?')
            cprintf(EOL "A fine choice. " EOL);
    }

    if (Options.random_pick || Options.weapon == WPN_RANDOM || keyin == '?')
        keyin = 'a' + random2( num_choices );

    you.inv[0].sub_type = startwep[ keyin - 'a' ];

    // shift str/dex if weapon selected is more than 3 away from 5/5
    const int shift = (weapon_str_weight( you.inv[0] ) - 5) / 3;

    you.str += shift;
    you.dex -= shift;

    you.max_str = you.str;
    you.max_dex = you.dex;
}

void init_player( void )
{
    int i = 0;        // loop variable

    you.birth_time = time( NULL );
    you.real_time = 0;
    you.num_turns = 0;

#ifdef WIZARD
    you.wizard = (Options.wiz_mode == WIZ_YES) ? true : false;
#else
    you.wizard = false;
#endif

    you.activity = ACT_NONE;
    you.berserk_penalty = 0;
    you.berserker = 0;
    you.conf = 0;
    you.confusing_touch = 0;
    you.deaths_door = 0;
    you.disease = 0;
    you.elapsed_time = 0.0;
    you.exhausted = 0;
    you.haste = 0;
    you.invis = 0;
    you.levitation = 0;
    you.might = 0;
    you.paralysis = 0;
    you.poison = 0;
    you.rotting = 0;
    you.fire_shield = 0;
    you.slow = 0;
    you.special_wield = SPWLD_NONE;
    you.sure_blade = 0;
    you.synch_time = 0;

    you.base_hp = 5000;
    you.base_hp2 = 5000;
    you.base_magic_points = 5000;
    you.base_magic_points2 = 5000;

    you.magic_points_regeneration = 0;
    you.str = 0;
    you.max_str = 0;
    you.intel = 0;
    you.max_intel = 0;
    you.dex = 0;
    you.max_dex = 0;
    you.xp = 0;
    you.xp_level = 1;
    you.max_xp_level = 1;

    you.species = SP_UNKNOWN;
    you.char_class = JOB_UNKNOWN;

    you.naked_okay = false;
    you.delay_work = 0;

    you.hunger = HUNGER_STARTING_LEVEL;
    you.hunger_state = HS_SATIATED;

    you.gold = 0;

    you.nemelex_altar_index = -1;

    you.burden = 0;
    you.burden_state = BS_UNENCUMBERED;

    you.spell_no = 0;

    you.depth = 0;
    you.level_type = LEVEL_DUNGEON;
    you.where_are_you = BRANCH_MAIN_DUNGEON;
    you.char_direction = DIR_DESCENDING;

    you.prev_targ = MHITNOT;
    you.pet_target = MHITNOT;

    you.x_pos = 0;
    you.y_pos = 0;

    you.stepping = false;
    you.running = RMODE_NOT_RUNNING;
    you.run_x = 0;
    you.run_y = 0;
    for (i = 0; i < 3; i++)
    {
        you.run_check[i].grid = 0;
        you.run_check[i].dx = 0;
        you.run_check[i].dy = 0;
    }

    you.religion = GOD_NO_GOD;
    you.piety = 0;
    you.gift_timeout = 0;

    for (i = 0; i < MAX_NUM_GODS; i++)
    {
        you.penance[i] = 0;
        you.worshipped[i] = 0;
        you.num_gifts[i] = 0;
    }

    env.ghost.name[0] = '\0';

    for (i = 0; i < NUM_GHOST_VALUES; i++)
        env.ghost.values[i] = 0;

    for (i = EQ_WEAPON; i < NUM_EQUIP; i++)
        you.equip[i] = -1;

    for (i = 0; i < 25; i++)
        you.spells[i] = SPELL_NO_SPELL;

    for (i = 0; i < 52; i++)
    {
        you.spell_letter_table[i] = -1;
        you.ability_letter_table[i] = ABIL_NON_ABILITY;
    }

    for (i = 0; i < 100; i++)
        you.mutation[i] = 0;

    for (i = 0; i < 100; i++)
        you.demon_pow[i] = 0;

    for (i = 0; i < MAX_SUBTYPES; i++)
        you.had_book[i] = 0;

    for (i = 0; i < MAX_UNIQ_ITEMS; i++)
        you.unique_items[i] = UNIQ_NOT_EXISTS;

    for (i = 0; i < NO_UNRANDARTS; i++)
        set_unrandart_exist(i, 0);

    for (i = 0; i < MAX_SKILLS; i++)
    {
        you.skills[i] = 0;
        you.skill_points[i] = 0;
        you.skill_order[i] = MAX_SKILL_ORDER;
        you.practise_skill[i] = 1;
    }

    you.skill_cost_level = 1;
    you.total_skill_points = 0;

    for (i = 0; i < 30; i++)
    {
        if (i >= ATTR_DEMONIC_POWER_1 && i <= ATTR_DEMONIC_POWER_5)
            you.attribute[i] = MUT_NONE;
        else
            you.attribute[i] = 0;
    }

    for (i = 0; i < ENDOFPACK; i++)
    {
        init_item( you.inv[i] );
        set_ident_flags( you.inv[i], ISFLAG_IDENT_MASK );

        // set special inventory marker values:
        you.inv[i].x = -1;
        you.inv[i].y = -1;
        you.inv[i].slot = i;
    }

    for (i = 0; i < NUM_DURATIONS; i++)
        you.duration[i] = 0;
}

static void give_last_paycheck( void )
{
    if (you.species == SP_GHOUL)
        return;

    switch (you.char_class)
    {
    case JOB_HEALER:
    case JOB_THIEF:
        you.gold = roll_dice( 2, 100 );
        break;

    case JOB_WANDERER:
    case JOB_WARPER:
    case JOB_ASSASSIN:
        you.gold = roll_dice( 2, 50 );
        break;

    default:
        you.gold = roll_dice( 2, 20 );
        break;

    case JOB_PALADIN:
    case JOB_MONK:
        you.gold = 0;
        break;
    }
}

// requires stuff::modify_all_stats() and works because
// stats zeroed out by newgame::init_player()... recall
// that demonspawn & demingods get more later on {dlb}
static void species_stat_init( void )
{
    int sb = 0; // strength base
    int ib = 0; // intelligence base
    int db = 0; // dexterity base

    // Note: The stats in in this list aren't intended to sum the same
    // for all races.  The fact that Mummies and Ghouls are really low
    // is considered acceptable (Mummies don't have to eat, and Ghouls
    // are supposted to be a really hard race).  Also note that Demigods
    // and Demonspawn get seven more random points added later. -- bwr
    switch (you.species)
    {
    default:                    sb =  6; ib =  6; db =  6;      break;  // 18
    case SP_HUMAN:              sb =  6; ib =  6; db =  6;      break;  // 18
    case SP_DEMIGOD:            sb =  6; ib =  6; db =  6;      break;  // 18+6
    case SP_DEMONSPAWN:         sb =  4; ib =  4; db =  4;      break;  // 12+6

    case SP_ELF:                sb =  4; ib =  7; db =  7;      break;  // 18
    case SP_HIGH_ELF:           sb =  4; ib =  8; db =  7;      break;  // 19
    case SP_GREY_ELF:           sb =  3; ib =  8; db =  7;      break;  // 18
    case SP_DEEP_ELF:           sb =  2; ib =  9; db =  7;      break;  // 18
    case SP_SLUDGE_ELF:         sb =  5; ib =  6; db =  7;      break;  // 18

    case SP_HILL_DWARF:         sb =  9; ib =  5; db =  4;      break;  // 18
    case SP_MOUNTAIN_DWARF:     sb =  8; ib =  6; db =  4;      break;  // 18

    case SP_GIANT:              sb = 16; ib =  2; db =  0;      break;  // 18
    case SP_TROLL:              sb = 13; ib =  1; db =  4;      break;  // 18
    case SP_OGRE:               sb = 12; ib =  2; db =  4;      break;  // 18
    case SP_OGRE_MAGE:          sb = 10; ib =  5; db =  4;      break;  // 19

    case SP_MINOTAUR:           sb = 10; ib =  3; db =  5;      break;  // 18
    case SP_HILL_ORC:           sb =  9; ib =  3; db =  5;      break;  // 17
    case SP_CENTAUR:            sb =  8; ib =  5; db =  3;      break;  // 16
    case SP_NAGA:               sb =  6; ib =  6; db =  4;      break;  // 16
    case SP_MERFOLK:            sb =  5; ib =  6; db =  7;      break;  // 18
    case SP_KENKU:              sb =  5; ib =  6; db =  7;      break;  // 18

    case SP_GNOME:              sb =  5; ib =  6; db =  7;      break;  // 18
    case SP_KOBOLD:             sb =  5; ib =  4; db =  8;      break;  // 17
    case SP_HALFLING:           sb =  3; ib =  6; db =  9;      break;  // 18
    case SP_SPRIGGAN:           sb =  1; ib =  8; db =  9;      break;  // 18

    case SP_MUMMY:              sb =  7; ib =  3; db =  3;      break;  // 13
    case SP_GHOUL:              sb =  7; ib =  0; db =  3;      break;  // 10

    case SP_RED_DRACONIAN:
    case SP_WHITE_DRACONIAN:
    case SP_GREEN_DRACONIAN:
    case SP_YELLOW_DRACONIAN:
    case SP_GREY_DRACONIAN:
    case SP_BLACK_DRACONIAN:
    case SP_PURPLE_DRACONIAN:
    case SP_MOTTLED_DRACONIAN:
    case SP_PALE_DRACONIAN:
    case SP_UNK0_DRACONIAN:
    case SP_UNK1_DRACONIAN:
    case SP_BASE_DRACONIAN:     sb =  8; ib =  6; db =  4;      break;  // 18
    }

    modify_all_stats( sb, ib, db );
}

static void jobs_stat_init()
{
    int s = 0;   // strength mod
    int i = 0;   // intelligence mod
    int d = 0;   // dexterity mod
    int hp = 0;  // HP base
    int mp = 0;  // MP base

    // Note: Wanderers are correct, they're a challenging class. -- bwr
    // Also: Jobs which allow for weapon choice may modify str/dex +/-1
    switch (you.char_class)
    {
    case JOB_FIGHTER:           s =  7; i = -1; d =  4; hp = 15; mp = 0; break;
    case JOB_GLADIATOR:         s =  5; i =  0; d =  5; hp = 15; mp = 0; break;
    case JOB_BERSERKER:         s =  8; i = -2; d =  4; hp = 14; mp = 0; break;

    case JOB_HUNTER:            s =  4; i =  0; d =  6; hp = 14; mp = 0; break;

    case JOB_PALADIN:           s =  6; i =  0; d =  4; hp = 14; mp = 1; break;
    case JOB_PRIEST:            s =  6; i =  1; d =  3; hp = 13; mp = 1; break;
    case JOB_CHAOS_KNIGHT:      s =  5; i =  1; d =  4; hp = 12; mp = 1; break;
    case JOB_HEALER:            s =  4; i =  2; d =  4; hp = 12; mp = 1; break;
    case JOB_DEATH_KNIGHT:      s =  4; i =  3; d =  3; hp = 12; mp = 1; break;

    case JOB_CRUSADER:          s =  3; i =  4; d =  3; hp = 13; mp = 1; break;
    case JOB_WARPER:            s =  3; i =  4; d =  3; hp = 12; mp = 1; break;
    case JOB_REAVER:            s =  3; i =  4; d =  3; hp = 12; mp = 1; break;

    case JOB_THIEF:             s =  3; i =  1; d =  6; hp = 13; mp = 0; break;
    case JOB_ASSASSIN:          s =  1; i =  1; d =  8; hp = 13; mp = 0; break;
    case JOB_STALKER:           s =  0; i =  4; d =  6; hp = 12; mp = 1; break;

    case JOB_MONK:              s =  2; i =  1; d =  7; hp = 13; mp = 0; break;
    case JOB_TRANSMUTER:        s =  2; i =  4; d =  4; hp = 12; mp = 1; break;

    case JOB_WIZARD:            s = -1; i =  8; d =  3; hp =  8; mp = 4; break;
    case JOB_CONJURER:          s =  0; i =  6; d =  4; hp = 10; mp = 3; break;
    case JOB_ENCHANTER:         s =  0; i =  6; d =  4; hp = 10; mp = 3; break;
    case JOB_FIRE_ELEMENTALIST: s =  0; i =  6; d =  4; hp = 10; mp = 3; break;
    case JOB_ICE_ELEMENTALIST:  s =  0; i =  6; d =  4; hp = 10; mp = 3; break;
    case JOB_AIR_ELEMENTALIST:  s =  0; i =  6; d =  4; hp = 10; mp = 3; break;
    case JOB_EARTH_ELEMENTALIST:s =  0; i =  6; d =  4; hp = 10; mp = 3; break;
    case JOB_SUMMONER:          s =  0; i =  6; d =  4; hp = 10; mp = 3; break;
    case JOB_VENOM_MAGE:        s =  0; i =  6; d =  4; hp = 10; mp = 3; break;
    case JOB_NECROMANCER:       s =  0; i =  6; d =  4; hp = 10; mp = 3; break;

    case JOB_WANDERER:          s =  2; i =  2; d =  2; hp = 11; mp = 1; break;
    default:                    s =  0; i =  0; d =  0; hp = 10; mp = 0; break;
    }

    modify_all_stats( s, i, d );

    set_hp( hp, true );
    set_mp( mp, true );
}

static void init_race_class_stats( void )
{
    // set initial base stats for race and class:
    species_stat_init();
    jobs_stat_init();

    // randomly boost stats a number of times based on species
    unsigned int points_left = (you.species == SP_DEMIGOD
                                 || you.species == SP_DEMONSPAWN) ? 14 : 8;

    // first spend points to get us to the minimum allowed value -- bwr
    if (you.str < MIN_START_STAT)
    {
        points_left -= (MIN_START_STAT - you.str);
        you.str = MIN_START_STAT;
    }

    if (you.intel < MIN_START_STAT)
    {
        points_left -= (MIN_START_STAT - you.intel);
        you.intel = MIN_START_STAT;
    }

    if (you.dex < MIN_START_STAT)
    {
        points_left -= (MIN_START_STAT - you.dex);
        you.dex = MIN_START_STAT;
    }

    // now randomly assign the remaining points --bwr
    while (points_left > 0)
    {
        switch (random2( NUM_STATS ))
        {
        case STAT_STRENGTH:
            if (you.str > 17 && coinflip())
                continue;

            you.str++;
            break;

        case STAT_DEXTERITY:
            if (you.dex > 17 && coinflip())
                continue;

            you.dex++;
            break;

        case STAT_INTELLIGENCE:
            if (you.intel > 17 && coinflip())
                continue;

            you.intel++;
            break;
        }

        points_left--;
    }

    // these need to be set above using functions!!! {dlb}
    you.max_dex = you.dex;
    you.max_str = you.str;
    you.max_intel = you.intel;

    // then: adjust hp_max by species {dlb}
    if (player_genus( GENPC_DRACONIAN ) || player_genus( GENPC_DWARVEN ))
        inc_max_hp(1);
    else
    {
        switch (you.species)
        {
        case SP_GIANT:
        case SP_CENTAUR:
        case SP_OGRE:
        case SP_TROLL:
            inc_max_hp(3);
            break;

        case SP_MINOTAUR:
        case SP_NAGA:
        case SP_OGRE_MAGE:
        case SP_DEMIGOD:
            inc_max_hp(2);
            break;

        case SP_HILL_ORC:
        case SP_MUMMY:
        case SP_MERFOLK:
            inc_max_hp(1);
            break;

        case SP_ELF:
        case SP_GREY_ELF:
        case SP_HIGH_ELF:
            dec_max_hp(1);
            break;

        case SP_DEEP_ELF:
        case SP_GHOUL:
        case SP_GNOME:
        case SP_HALFLING:
        case SP_KENKU:
        case SP_KOBOLD:
        case SP_SPRIGGAN:
            dec_max_hp(2);
            break;

        default:
            break;
        }
    }

    // then: adjust max_magic_points by species {dlb}
    switch (you.species)
    {
    case SP_SPRIGGAN:
    case SP_DEMIGOD:
    case SP_GREY_ELF:
    case SP_DEEP_ELF:
        inc_max_mp(1);
        break;

    default:
        break;
    }
}

static void give_basic_knowledge( void )
{
    switch (you.char_class)
    {
    case JOB_PRIEST:
        if (you.religion == GOD_YREDELEMNUL)
            set_ident_type( OBJ_POTIONS, POT_MIGHT, ID_KNOWN_TYPE );
        else
            set_ident_type( OBJ_POTIONS, POT_HEALING, ID_KNOWN_TYPE );
        break;

    case JOB_PALADIN:
        set_ident_type( OBJ_POTIONS, POT_HEALING, ID_KNOWN_TYPE );
        break;

    case JOB_HEALER:
        set_ident_type( OBJ_POTIONS, POT_HEALING, ID_KNOWN_TYPE );
        set_ident_type( OBJ_POTIONS, POT_HEAL_WOUNDS, ID_KNOWN_TYPE );
        break;

    case JOB_ASSASSIN:
    case JOB_STALKER:
    case JOB_VENOM_MAGE:
        set_ident_type( OBJ_POTIONS, POT_POISON, ID_KNOWN_TYPE );
        break;

    case JOB_WARPER:
        set_ident_type( OBJ_SCROLLS, SCR_BLINKING, ID_KNOWN_TYPE );
        break;

    case JOB_TRANSMUTER:
        set_ident_type( OBJ_POTIONS, POT_WATER, ID_KNOWN_TYPE );
        set_ident_type( OBJ_POTIONS, POT_CONFUSION, ID_KNOWN_TYPE );
        set_ident_type( OBJ_POTIONS, POT_POISON, ID_KNOWN_TYPE );
        break;

    default:
        break;
    }

    return;
}                               // end give_basic_knowledge()

static void give_basic_spells( void )
{
    unsigned char which_spell = SPELL_NO_SPELL;

    switch (you.char_class)
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
        which_spell = SPELL_SUMMON_SMALL_MAMMAL;
        break;
    case JOB_ICE_ELEMENTALIST:
        which_spell = SPELL_FREEZE;
        break;
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
        if (you.species == SP_DEMIGOD || you.religion != GOD_YREDELEMNUL)
            which_spell = SPELL_PAIN;
        break;

    case JOB_WANDERER: // wanderers may or may not already have a spell -- bwr
    default:
        break;
    }

    if (which_spell != SPELL_NO_SPELL)
        add_spell_to_memory( which_spell );

    return;
}                               // end give_basic_spells()

// eventually, this should be something more grand {dlb}
static void opening_screen( void )
{
/* **********************************************
// this does not work just yet ... {dlb}:
    cprintf(EOL "Hello, ");

    if ( you.your_name[0] != '\0' )
    {
       cprintf(you.your_name); // better be less than 31 characters :P {dlb}
                               // of course, invalid names will appear {dlb}
       cprintf(", ");
    }
********************************************** */

    textcolor( YELLOW );
    cprintf("Hello, welcome to Dungeon Crawl " VERSION "!");
    textcolor( BROWN );
    cprintf(EOL "(c) Copyright 1997-2003 Linley Henzell");
    cprintf(EOL "Please consult crawl.txt for instructions and legal details."
            EOL);
    textcolor( LIGHTGREY );

    return;
}                               // end opening_screen()


static void enter_player_name( bool blankOK )
{
    // temporary 'til copyover to you.your_name {dlb}
    // made this rediculously long so that the game doesn't
    // crash if a really really long name is entered (argh).  {gdl}
    char name_entered[200];

    // anything to avoid goto statements {dlb}
    bool acceptable_name = false;
    bool first_time = true;

    // first time -- names set through init.txt/environment assumed ok {dlb}
    if (you.your_name[0] != '\0')
        acceptable_name = true;

    do
    {
        // prompt for a new name if current one unsatisfactory {dlb}:
        if (!acceptable_name)
        {
            textcolor( CYAN );
            if (blankOK && first_time)
                cprintf(EOL "Press <Enter> to answer this after race and class are chosen."EOL);

            first_time = false;

            cprintf(EOL "What is your name today? ");
            textcolor( LIGHTGREY );
            get_input_line( name_entered, sizeof( name_entered ) );

            strncpy( you.your_name, name_entered, kNameLen );
            you.your_name[ kNameLen - 1 ] = '\0';
        }

        // verification begins here {dlb}:
        if (you.your_name[0] == '\0')
        {
            if (blankOK)
                return;

            cprintf(EOL "That's a silly name!" EOL);
            acceptable_name = false;
        }

        // if SAVE_DIR_PATH is defined, userid will be tacked onto the end
        // of each character's files, making bones a valid player name.
#ifndef SAVE_DIR_PATH
        // this would cause big probs with ghosts
        // what would? {dlb}
        // ... having the name "bones" of course! The problem comes from
        // the fact that bones files would have the exact same filename
        // as level files for a character named "bones".  -- bwr
        else if (stricmp(you.your_name, "bones") == 0)
        {
            cprintf(EOL "That's a silly name!" EOL);
            acceptable_name = false;
        }
#endif
        else
            acceptable_name = verify_player_name();

    }
    while (!acceptable_name);
}                               // end enter_player_name()

bool verify_player_name(void)
{
#if defined(DOS) || defined(WIN32CONSOLE)
    // quick check for CON/LPTx -- blows up real good under DOS/Windows
    if (stricmp( you.your_name, "CON" ) == 0
        || strnicmp( you.your_name, "LPT", 3 ) == 0)
    {
        cprintf(EOL "Sorry, that name gives your OS a headache." EOL);
        return (false);
    }
#endif

    const size_t len = strlen( you.your_name );
    for (unsigned int i = 0; i < len; i++)
    {
#if MAC
        // the only bad character on Macs is the path seperator
        if (you.your_name[i] == ':')
        {
            cprintf(EOL "No colons, please." EOL);
            return (false);
        }
#else
        // Note that this includes systems which may be using the
        // packaging system.  The packaging system is very simple
        // and doesn't take the time to escape every characters that
        // might be a problem for some random shell or OS... so we
        // play it very conservative here.  -- bwr
        if (!isalnum( you.your_name[i] ) && you.your_name[i] != '_')
        {
            cprintf( EOL "Alpha-numerics and underscores only, please." EOL );
            return (false);
        }
#endif
    }

#ifdef SAVE_DIR_PATH
    // Until we have a better way to handle the fact that this could lead
    // to some confusion with where the name ends and the uid begins. -- bwr
    if (you.your_name[len - 1] == '-')
    {
        cprintf( EOL "Sorry, your name cannot end with a dash." EOL );
        return (false);
    }
#endif

    return (true);
}                               // end verify_player_name()

#if 0
// currently unused
static void give_random_scroll( int slot )
{
    you.inv[slot].quantity = 1;
    you.inv[slot].base_type = OBJ_SCROLLS;
    you.inv[slot].plus = 0;
    you.inv[slot].special = 0;
    you.inv[slot].colour = WHITE;

    switch (random2(8))
    {
    case 0:
        you.inv[slot].sub_type = SCR_REMOVE_CURSE;
        break;

    case 1:
        you.inv[slot].sub_type = SCR_IDENTIFY;
        break;

    case 2:
    case 3:
        you.inv[slot].sub_type = SCR_BLINKING;
        break;

    case 4:
        you.inv[slot].sub_type = SCR_FEAR;
        break;

    case 5:
        you.inv[slot].sub_type = SCR_SUMMONING;
        break;

    case 6:
    case 7:
    default:
        you.inv[slot].sub_type = SCR_TELEPORTATION;
        break;
    }
}
#endif

static void give_random_potion( int slot )
{
    you.inv[slot].quantity = 1;
    you.inv[slot].base_type = OBJ_POTIONS;
    you.inv[slot].plus = 0;
    you.inv[slot].plus2 = 0;

    switch (random2(8))
    {
    case 0:
    case 1:
    case 2:
        you.inv[slot].sub_type = POT_HEALING;
        break;
    case 3:
    case 4:
        you.inv[slot].sub_type = POT_HEAL_WOUNDS;
        break;
    case 5:
        you.inv[slot].sub_type = POT_SPEED;
        break;
    case 6:
        you.inv[slot].sub_type = POT_MIGHT;
        break;
    case 7:
        you.inv[slot].sub_type = POT_BERSERK_RAGE;
        break;
    }
}

#if 0
// currently unused
static void give_random_wand( int slot )
{
    you.inv[slot].quantity = 1;
    you.inv[slot].base_type = OBJ_WANDS;
    you.inv[slot].special = 0;
    you.inv[slot].plus2 = 0;

    switch (random2(4))
    {
    case 0:
        you.inv[slot].sub_type = WAND_SLOWING;
        you.inv[slot].plus = 7 + random2(5);
        break;
    case 1:
        you.inv[slot].sub_type = WAND_PARALYSIS;
        you.inv[slot].plus = 5 + random2(4);
        break;
    case 2:
        you.inv[slot].sub_type = coinflip() ? WAND_FROST : WAND_FLAME;
        you.inv[slot].plus = 6 + random2(4);
        break;
    case 3:
        you.inv[slot].sub_type = WAND_TELEPORTATION;
        you.inv[slot].plus = 3 + random2(4);
        break;
    }
}
#endif

static void give_random_secondary_armour( int slot )
{
    you.inv[slot].quantity = 1;
    you.inv[slot].base_type = OBJ_ARMOUR;
    you.inv[slot].special = 0;
    you.inv[slot].plus = 0;
    you.inv[slot].plus2 = 0;
    you.inv[slot].colour = BROWN;

    switch (random2(4))
    {
    case 0:
        you.inv[slot].sub_type = ARM_CLOAK;
        you.equip[EQ_CLOAK] = slot;
        break;
    case 1:
        you.inv[slot].sub_type = ARM_BOOTS;
        you.equip[EQ_BOOTS] = slot;
        break;
    case 2:
        you.inv[slot].sub_type = ARM_GLOVES;
        you.equip[EQ_GLOVES] = slot;
        break;
    case 3:
        you.inv[slot].sub_type = ARM_HELMET;
        you.equip[EQ_HELMET] = slot;
        break;
    }
}

// Returns true if a "good" weapon is given
static bool give_wanderer_weapon( int slot, int wpn_skill )
{
    bool ret = false;

    // Slot's always zero, but we pass it anyways.

    // We'll also re-fill the template, all this for later possible
    // safe reuse of code in the future.
    you.inv[slot].quantity = 1;
    you.inv[slot].base_type = OBJ_WEAPONS;
    you.inv[slot].colour = LIGHTCYAN;
    you.inv[slot].plus = 0;
    you.inv[slot].plus2 = 0;
    you.inv[slot].special = 0;

    // Now fill in the type according to the random wpn_skill
    switch (wpn_skill)
    {
    case SK_MACES_FLAILS:
        you.inv[slot].sub_type = WPN_CLUB;
        you.inv[slot].colour = BROWN;
        break;

    case SK_POLEARMS:
        you.inv[slot].sub_type = WPN_SPEAR;
        break;

    case SK_SHORT_BLADES:
        you.inv[slot].sub_type = WPN_DAGGER;
        break;

    case SK_AXES:
        you.inv[slot].sub_type = WPN_HAND_AXE;
        ret = true;
        break;

    case SK_STAVES:
        you.inv[slot].sub_type = WPN_STAFF;
        you.inv[slot].colour = BROWN;
        ret = true;
        break;

    case SK_LONG_SWORDS:
    default:
        // all long swords are too good for a starting character...
        // especially this class where we have to be careful about
        // giving away anything good at all.
        // We default here if the character only has fighting skill -- bwr
        you.inv[slot].sub_type = WPN_SHORT_SWORD;
        ret = true;
        break;
    }

    return (ret);
}

//
// The idea behind wanderers is a class that has various different
// random skills that's a challenge to play... not a class that can
// be continually rerolled to gain the ideal character.  To maintain
// this, we have to try and make sure that they typically get worse
// equipment than any other class... this for certain means no
// spellbooks ever, and the bows and xbows down below might be too
// much... so pretty much things should be removed rather than
// added here. -- bwr
//
static void create_wanderer( void )
{
    const int util_skills[] =
        { SK_DARTS, SK_RANGED_COMBAT, SK_ARMOUR, SK_DODGING, SK_STEALTH,
          SK_STABBING, SK_SHIELDS, SK_TRAPS_DOORS, SK_UNARMED_COMBAT,
          SK_INVOCATIONS, SK_EVOCATIONS };
    const int num_util_skills = sizeof(util_skills) / sizeof(int);

    // Long swords is missing to increae it's rarity because we
    // can't give out a long sword to a starting character (they're
    // all too good)... Staves is also removed because it's not
    // one of the fighter options.-- bwr
    const int fight_util_skills[] =
        { SK_FIGHTING, SK_SHORT_BLADES, SK_AXES,
          SK_MACES_FLAILS, SK_POLEARMS,
          SK_DARTS, SK_RANGED_COMBAT, SK_ARMOUR, SK_DODGING, SK_STEALTH,
          SK_STABBING, SK_SHIELDS, SK_TRAPS_DOORS, SK_UNARMED_COMBAT,
          SK_INVOCATIONS, SK_EVOCATIONS };
    const int num_fight_util_skills = sizeof(fight_util_skills) / sizeof(int);

    const int not_rare_skills[] =
        { SK_SLINGS, SK_BOWS, SK_CROSSBOWS,
          SK_SPELLCASTING, SK_CONJURATIONS, SK_ENCHANTMENTS,
          SK_FIRE_MAGIC, SK_ICE_MAGIC, SK_AIR_MAGIC, SK_EARTH_MAGIC,
          SK_FIGHTING, SK_SHORT_BLADES, SK_LONG_SWORDS, SK_AXES,
          SK_MACES_FLAILS, SK_POLEARMS, SK_STAVES,
          SK_DARTS, SK_RANGED_COMBAT, SK_ARMOUR, SK_DODGING, SK_STEALTH,
          SK_STABBING, SK_SHIELDS, SK_TRAPS_DOORS, SK_UNARMED_COMBAT,
          SK_INVOCATIONS, SK_EVOCATIONS };
    const int num_not_rare_skills = sizeof(not_rare_skills) / sizeof(int);

    const int all_skills[] =
        { SK_SUMMONINGS, SK_NECROMANCY, SK_TRANSLOCATIONS, SK_TRANSMIGRATION,
          SK_DIVINATIONS, SK_POISON_MAGIC,
          SK_SLINGS, SK_BOWS, SK_CROSSBOWS,
          SK_SPELLCASTING, SK_CONJURATIONS, SK_ENCHANTMENTS,
          SK_FIRE_MAGIC, SK_ICE_MAGIC, SK_AIR_MAGIC, SK_EARTH_MAGIC,
          SK_FIGHTING, SK_SHORT_BLADES, SK_LONG_SWORDS, SK_AXES,
          SK_MACES_FLAILS, SK_POLEARMS, SK_STAVES,
          SK_DARTS, SK_RANGED_COMBAT, SK_ARMOUR, SK_DODGING, SK_STEALTH,
          SK_STABBING, SK_SHIELDS, SK_TRAPS_DOORS, SK_UNARMED_COMBAT,
          SK_INVOCATIONS, SK_EVOCATIONS };
    const int num_all_skills = sizeof(all_skills) / sizeof(int);

    int skill;

    for (int i = 0; i < 2; i++)
    {
        do
        {
            skill = random2( num_util_skills );
        }
        while (you.skills[ util_skills[skill] ] >= 2);

        you.skills[ util_skills[skill] ] += 1;
    }

    for (int i = 0; i < 3; i++)
    {
        do
        {
            skill = random2( num_fight_util_skills );
        }
        while (you.skills[ fight_util_skills[skill] ] >= 2);

        you.skills[ fight_util_skills[skill] ] += 1;
    }

    // Spell skills are possible past this point, but we won't
    // allow two levels of any of them -- bwr
    for (int i = 0; i < 3; i++)
    {
        do
        {
            skill = random2( num_not_rare_skills );
        }
        while (you.skills[ not_rare_skills[skill] ] >= 2
                || (not_rare_skills[ skill ] >= SK_SPELLCASTING
                    && you.skills[ not_rare_skills[skill] ]));

        you.skills[ not_rare_skills[skill] ] += 1;
    }

    for (int i = 0; i < 2; i++)
    {
        do
        {
            skill = random2( num_all_skills );
        }
        while (you.skills[ all_skills[skill] ] >= 2
                || (all_skills[skill] >= SK_SPELLCASTING
                    && you.skills[ all_skills[skill] ]));

        you.skills[ all_skills[skill] ] += 1;
    }

    // Demigods can't use invocations so we'll swap it for something else
    if (you.species == SP_DEMIGOD && you.skills[SK_INVOCATIONS])
    {
        you.skills[SK_INVOCATIONS] = 0;

        do
        {
            skill = random2( num_all_skills );
        }
        while (skill == SK_INVOCATIONS && you.skills[ all_skills[skill] ]);

        you.skills[skill] = 1;
    }

    int wpn_skill = SK_FIGHTING;  // prefered weapon type
    int wpn_skill_size = 0;       // level of skill in prefered weapon type
    int num_wpn_skills = 0;       // used to choose prefered weapon
    int total_wpn_skills = 0;     // used to choose template

    // This algorithm is the same as the one used to pick a random
    // angry god for retribution, except that whenever a higher skill
    // is found than the current one, we automatically take it and
    // only consider skills at that level or higher from that point on,
    // This should give a random wpn skill from the set of skills with
    // the highest value. -- bwr
    for (int i = SK_SHORT_BLADES; i <= SK_STAVES; i++)
    {
        if (you.skills[i] > 0)
        {
            total_wpn_skills++;

            if (you.skills[i] > wpn_skill_size)
            {
                // switch to looking in the new set of better skills
                num_wpn_skills = 1; // reset to one, because it's a new set
                wpn_skill = i;
                wpn_skill_size = you.skills[i];
            }
            else if (you.skills[i] == wpn_skill_size)
            {
                // still looking at the old level
                num_wpn_skills++;
                if (one_chance_in( num_wpn_skills ))
                {
                    wpn_skill = i;
                    wpn_skill_size = you.skills[i];
                }
            }
        }
    }

    if (you.species == SP_GHOUL)
    {
        you.inv[2].quantity = 1;
        you.inv[2].base_type = OBJ_ARMOUR;
        you.inv[2].sub_type = ARM_ROBE;
        you.inv[2].colour = WHITE;
        you.inv[2].plus = 0;
        you.inv[2].special = 0;

        you.equip[EQ_WEAPON] = -1;
        you.equip[EQ_BODY_ARMOUR] = 2;

        return;
    }

    // Let's try to make an appropriate weapon
    // Start with a template for a weapon
    you.inv[0].quantity = 1;
    you.inv[0].base_type = OBJ_WEAPONS;
    you.inv[0].sub_type = WPN_KNIFE;
    you.inv[0].colour = LIGHTCYAN;
    you.inv[0].plus = 0;
    you.inv[0].plus2 = 0;
    you.inv[0].special = 0;

    // And a default armour template for a robe (leaving slot 1 open for
    // a secondary weapon).
    you.inv[2].quantity = 1;
    you.inv[2].base_type = OBJ_ARMOUR;
    you.inv[2].sub_type = ARM_ROBE;
    you.inv[2].colour = BROWN;
    you.inv[2].plus = 0;
    you.inv[2].special = 0;

    // Wanderers have at least seen one type of potion, and if they
    // don't get anything else good, they'll get to keep this one...
    // Note:  even if this is taken away, the knowledge of the potion
    // type is still given to the character.
    give_random_potion(3);

    if (you.skills[SK_FIGHTING] || total_wpn_skills >= 3)
    {
        // Fighter style wanderer
        if (you.skills[SK_ARMOUR])
        {
            you.inv[2].sub_type = ARM_RING_MAIL;
            you.inv[2].colour = LIGHTCYAN;

            you.inv[3].quantity = 0;            // remove potion
        }
        else if (you.skills[SK_SHIELDS] && wpn_skill != SK_STAVES)
        {
            you.inv[4].quantity = 1;
            you.inv[4].base_type = OBJ_ARMOUR;
            you.inv[4].sub_type = ARM_BUCKLER;
            you.inv[4].plus = 0;
            you.inv[4].special = 0;
            you.inv[4].colour = LIGHTCYAN;
            you.equip[EQ_SHIELD] = 4;

            you.inv[3].quantity = 0;            // remove potion
        }
        else
        {
            give_random_secondary_armour(5);
        }

        // remove potion if good weapon is given:
        if (give_wanderer_weapon( 0, wpn_skill ))
            you.inv[3].quantity = 0;
    }
#ifdef USE_SPELLCASTER_AND_RANGER_WANDERER_TEMPLATES
    else if (you.skills[SK_SPELLCASTING])
    {
        // Spellcaster style wanderer

        // Could only have learned spells in common schools...
        const int school_list[5] =
                    { SK_CONJURATIONS, SK_ENCHANTMENTS, SK_ENCHANTMENTS,
                      SK_TRANSLOCATIONS, SK_NECROMANCY };

        //jmf: Two of those spells are gone due to their munchkinicity.
        //     crush() and arc() are like having good melee capability.
        //     Therefore giving them to "harder" class makes less-than-
        //     zero sense, and they're now gone.
        const int spell_list[5] =
                    { SPELL_MAGIC_DART, SPELL_CONFUSING_TOUCH, SPELL_BACKLIGHT,
                      SPELL_APPORTATION, SPELL_ANIMATE_SKELETON };

        // Choose one of the schools we have at random.
        int school = SK_SPELLCASTING;
        int num_schools = 0;
        for (int i = 0; i < 5; i++)
        {
            if (you.skills[ school_list[i] ])
            {
                num_schools++;
                if (one_chance_in( num_schools ))
                    school = i;
            }
        }

        // Magic dart is quite a good spell, so if the player only has
        // spellcasting and conjurations, we sometimes hold off... and
        // treat them like an unskilled spellcaster.
        if (school == SK_SPELLCASTING
            || (num_schools == 1 && school == SK_CONJURATIONS && coinflip()))
        {
            // Not much melee potential and no common spell school,
            // we'll give the player a dagger.
            you.inv[0].sub_type = WPN_DAGGER;

            // ... and a random scroll
            give_random_scroll(4);

            // ... and knowledge of another
            give_random_scroll(5);
            you.inv[5].quantity = 0;

            // ... and a wand.
            give_random_wand(6);
        }
        else
        {
            // Give them an appropriate spell
            add_spell_to_memory( spell_list[ school ] );
        }
    }
    else if (you.skills[ SK_RANGED_COMBAT ] && one_chance_in(3)) // these are rare
    {
        // Ranger style wanderer
        // Rare since starting with a throwing weapon is very good

        // Create a default launcher template, but the
        // quantity may be reset to 0 if we don't want one -- bwr
        // thorwing weapons are lowered to -1 to make them
        // not as good as the one's hunters get, ammo is
        // also much smaller -- bwr
        you.inv[1].quantity = 1;
        you.inv[1].base_type = OBJ_WEAPONS;
        you.inv[1].sub_type = WPN_BOW;
        you.inv[1].plus = -1;
        you.inv[1].plus2 = -1;
        you.inv[1].special = 0;
        you.inv[1].colour = BROWN;

        // Create default ammo template (darts) (armour is slot 2)
        you.inv[4].base_type = OBJ_MISSILES;
        you.inv[4].sub_type = MI_DART;
        you.inv[4].quantity = 10 + roll_dice( 2, 6 );
        you.inv[4].plus = 0;
        you.inv[4].plus2 = 0;
        you.inv[4].special = 0;
        you.inv[4].colour = LIGHTCYAN;

        if (you.skills[SK_SLINGS])
        {
            // slingers get some extra ammo
            you.inv[4].quantity += roll_zdice(5,5);
            you.inv[4].sub_type = MI_STONE;
            you.inv[4].colour = LIGHTGREY;
            you.inv[1].sub_type = WPN_SLING;
            you.inv[1].plus = 0;               // slings aren't so good
            you.inv[1].plus2 = 0;              // so we'll make them +0

            you.inv[3].quantity = 0;            // remove potion
            you.inv[3].base_type = 0;               // forget potion
            you.inv[3].sub_type = 0;
        }
        else if (you.skills[SK_BOWS])
        {
            you.inv[4].sub_type = MI_ARROW;
            you.inv[4].colour = BROWN;
            you.inv[1].sub_type = WPN_BOW;

            you.inv[3].quantity = 0;            // remove potion
            you.inv[3].base_type = 0;               // forget potion
            you.inv[3].sub_type = 0;
        }
        else if (you.skills[SK_CROSSBOWS])
        {
            // Hand crossbows want the darts.
            you.inv[1].sub_type = WPN_HAND_CROSSBOW;

            you.inv[3].quantity = 0;            // remove potion
            you.inv[3].base_type = 0;               // forget potion
            you.inv[3].sub_type = 0;
        }
        else
        {
            // little extra poisoned darts for throwers
            you.inv[4].quantity += roll_zdice(2,6);
            set_item_ego_type( you.inv[4], OBJ_MISSILES, SPMSL_POISONED );

            you.inv[0].sub_type = WPN_DAGGER;       // up knife to dagger
            you.inv[1].quantity = 0;            // remove bow
        }
    }
#endif
    else
    {
        // Generic wanderer
        give_wanderer_weapon( 0, wpn_skill );
        give_random_secondary_armour(5);
    }

    you.equip[EQ_WEAPON] = 0;
    you.equip[EQ_BODY_ARMOUR] = 2;
}

static char letter_to_class( int keyn )
{
    if (keyn == 'a')
        return (JOB_FIGHTER);
    else if (keyn == 'b')
        return (JOB_WIZARD);
    else if (keyn == 'c')
        return (JOB_PRIEST);
    else if (keyn == 'd')
        return (JOB_THIEF);
    else if (keyn == 'e')
        return (JOB_GLADIATOR);
    else if (keyn == 'f')
        return (JOB_NECROMANCER);
    else if (keyn == 'g')
        return (JOB_PALADIN);
    else if (keyn == 'h')
        return (JOB_ASSASSIN);
    else if (keyn == 'i')
        return (JOB_BERSERKER);
    else if (keyn == 'j')
        return (JOB_HUNTER);
    else if (keyn == 'k')
        return (JOB_CONJURER);
    else if (keyn == 'l')
        return (JOB_ENCHANTER);
    else if (keyn == 'm')
        return (JOB_FIRE_ELEMENTALIST);
    else if (keyn == 'n')
        return (JOB_ICE_ELEMENTALIST);
    else if (keyn == 'o')
        return (JOB_SUMMONER);
    else if (keyn == 'p')
        return (JOB_AIR_ELEMENTALIST);
    else if (keyn == 'q')
        return (JOB_EARTH_ELEMENTALIST);
    else if (keyn == 'r')
        return (JOB_CRUSADER);
    else if (keyn == 's')
        return (JOB_DEATH_KNIGHT);
    else if (keyn == 't')
        return (JOB_VENOM_MAGE);
    else if (keyn == 'u')
        return (JOB_CHAOS_KNIGHT);
    else if (keyn == 'v')
        return (JOB_TRANSMUTER);
    else if (keyn == 'w')
        return (JOB_HEALER);
    else if (keyn == 'y')
        return (JOB_REAVER);
    else if (keyn == 'z')
        return (JOB_STALKER);
    else if (keyn == 'A')
        return (JOB_MONK);
    else if (keyn == 'B')
        return (JOB_WARPER);
    else if (keyn == 'C')
        return (JOB_WANDERER);

    return (JOB_UNKNOWN);
}

static char letter_to_species( int keyn )
{
    switch (keyn)
    {
    case 'a':
        return (SP_HUMAN);
    case 'b':
        return (SP_ELF);
    case 'c':
        return (SP_HIGH_ELF);
    case 'd':
        return (SP_GREY_ELF);
    case 'e':
        return (SP_DEEP_ELF);
    case 'f':
        return (SP_SLUDGE_ELF);
    case 'g':
        return (SP_HILL_DWARF);
    case 'h':
        return (SP_MOUNTAIN_DWARF);
    case 'i':
        return (SP_HALFLING);
    case 'j':
        return (SP_HILL_ORC);
    case 'k':
        return (SP_KOBOLD);
    case 'l':
        return (SP_MUMMY);
    case 'm':
        return (SP_NAGA);
    case 'n':
        return (SP_GNOME);
    case 'o':
        return (SP_OGRE);
    case 'p':
        return (SP_TROLL);
    case 'q':
        return (SP_OGRE_MAGE);
    case 'r':                   // draconian
        return (SP_RED_DRACONIAN + random2(9));    // random drac
    case 's':
        return (SP_CENTAUR);
    case 't':
        return (SP_DEMIGOD);
    case 'u':
        return (SP_SPRIGGAN);
    case 'v':
        return (SP_MINOTAUR);
    case 'w':
        return (SP_DEMONSPAWN);
    case 'x':
        return (SP_GHOUL);
    case 'y':
        return (SP_KENKU);
    case 'z':
        return (SP_MERFOLK);
    case 'A':
        return (SP_GIANT);
    default:
        break;
    }

    return (SP_UNKNOWN);
}

static char species_to_letter(int spec)
{
    if (spec == SP_GIANT)
        return ('A');

    if (spec > SP_RED_DRACONIAN && spec <= SP_BASE_DRACONIAN)
        spec = SP_RED_DRACONIAN;
    else if (spec > SP_BASE_DRACONIAN)
        spec -= SP_BASE_DRACONIAN - SP_RED_DRACONIAN;

    return ('a' + spec - 1);
}

// TODO: unify the state machine that moves between choose_race and
// choose_class into a single cleaner function.
//
// choose_race returns true if the player should also pick a class.
// This is done because of the '*' option which will pick a random
// character, obviating the necessity of choosing a class.
static bool choose_race( void )
{
    char keyn;

    bool printed = false;

    if (Options.cls)
        you.char_class = letter_to_class( Options.cls );

    if (Options.race)
        printed = true;

spec_query:
    if (!printed)
    {
        clrscr();

        if (you.char_class != JOB_UNKNOWN)
        {
            bool shortgreet = false;
            const bool have_name = (you.your_name[0] != '\0');

            textcolor( BROWN );
            if (have_name || you.char_class != JOB_UNKNOWN)
                cprintf("Welcome, ");
            else
            {
                cprintf("Welcome.");
                shortgreet = true;
            }

            textcolor( YELLOW );
            if (have_name)
            {
                cprintf( you.your_name );
                if (you.char_class != JOB_UNKNOWN)
                    cprintf(" the ");
            }

            if (you.char_class != JOB_UNKNOWN)
                cprintf( get_class_name( you.char_class ) );

            if (!shortgreet)
                cprintf(".");
        }
        else
        {
            textcolor( WHITE );
            cprintf("You must be new here!");
        }

        cprintf( EOL EOL );
        textcolor( CYAN );
        cprintf( "You can be:" EOL EOL );
        textcolor( LIGHTGREY );

        int linec = 0;
        char linebuf[200];
        *linebuf = 0;

        for (int i = SP_HUMAN; i < NUM_SPECIES; ++i)
        {
            if (i > SP_RED_DRACONIAN && i <= SP_BASE_DRACONIAN)
                continue;

            if (!class_allowed( i, you.char_class ))
                continue;

            char buf[100];

            // XXX:
            // [ds] Using species_name alone means giants lose their
            // 'experimental' status. Yeeargh.
            snprintf( buf, sizeof(buf), "%c - %-26s", species_to_letter(i),
                        (i == SP_GIANT) ? "Giant (experimental)"
                                        : species_name(i, 1) );

            strncat( linebuf, buf, sizeof(linebuf) );
            if (++linec >= 2)
            {
                cprintf( "%s" EOL, linebuf );
                *linebuf = 0;
                linec = 0;
            }
        }

        if (linec)
            cprintf( "%s" EOL, linebuf );

        textcolor( BROWN );
        if (you.char_class == JOB_UNKNOWN)
        {
            cprintf(EOL "Enter - Choose class first; "
                        "? - Random species; * - Random character; X - Quit"
                    EOL);
        }
        else
        {
            cprintf(EOL "? - Random; Bksp - Back to class selection; X - Quit"
                    EOL);
        }

        textcolor( CYAN );
        cprintf( EOL "What is your species? " );
        textcolor( LIGHTGREY );

        printed = true;
    }

    if (Options.race)
        keyn = Options.race;
    else
    {
        keyn = getch();
        if (keyn == 0)
        {
            getch();
            goto spec_query;
        }
    }

    you.species = letter_to_species(keyn);
    if (you.species == SP_UNKNOWN)
    {
        if (keyn == '?')
        {
            // pick a species at random
            int race_count = 0;
            int race = -1;

            for (int k = SP_HUMAN; k < NUM_SPECIES; k++)
            {
                if (class_allowed( k, you.char_class ))
                {
                    race_count++;
                    if (one_chance_in( race_count ))
                        race = k;
                }
            }

            ASSERT( race != -1 ); // at least one should have been allowed
            you.species = race;
        }
        else if (keyn == '*')
        {
            pick_random_species_and_class();
            Options.random_pick = true; // used to give random weapon/god as well
            return (false);
        }
        else if (((keyn == '\n' || keyn == '\r') && you.char_class == JOB_UNKNOWN)
                    || keyn == 'x' || keyn == ESCAPE || keyn == CK_BKSP)
        {
            you.species = SP_UNKNOWN;
            Options.race = '\0';
            return (true);
        }
        else if (keyn == 'X')
        {
            cprintf(EOL "Goodbye!");
            end(0);
        }
        else
        {
            if (Options.race)
            {
                Options.race = '\0';
                printed = false;
            }
            goto spec_query;
        }
    }

    if (you.species != SP_UNKNOWN && you.char_class != JOB_UNKNOWN
        && !class_allowed( you.species, you.char_class, true ))
    {
        goto spec_query;
    }

    // set to 0 in case we come back from choose_class()
    Options.race = '\0';

    return (true);
}

// returns true if a class was chosen,  false if we should go back to
// race selection.
static bool choose_class( void )
{
    char keyn;
    int i, j;
    bool printed = false;

    if (Options.cls)
        printed = true;

    if (you.species != SP_UNKNOWN && you.char_class != JOB_UNKNOWN)
        return (true);

job_query:
    if (!printed)
    {
        clrscr();

        if (you.species)
        {
            bool shortgreet = false;
            const bool have_name = (you.your_name[0] != '\0');

            textcolor( BROWN );
            if (have_name || you.species != SP_UNKNOWN)
                cprintf( "Welcome, " );
            else
            {
                shortgreet = true;
                cprintf( "Welcome." );
            }

            textcolor( YELLOW );
            if (have_name)
            {
                cprintf( you.your_name );
                if (you.species != SP_UNKNOWN)
                    cprintf(" the ");
            }

            if (you.species != SP_UNKNOWN)
                cprintf( species_name( you.species, you.xp_level ) );

            if (!shortgreet)
                cprintf(".");
        }
        else
        {
            textcolor( WHITE );
            cprintf( "You must be new here!" );
        }

        cprintf( EOL EOL );
        textcolor( CYAN );
        cprintf( "You can be any of the following:" EOL EOL );
        textcolor( LIGHTGREY );

        j = 0;               // used within for loop to determine newline {dlb}

        for (i = 0; i < NUM_JOBS; i++)
        {
            if (i == JOB_QUITTER || !class_allowed( you.species, i ))
                continue;

            putch( index_to_letter(i) );
            cprintf( " - " );
            cprintf( get_class_name(i) );

            if (j % 2)
                cprintf( EOL );
            else
                gotoxy( 31, wherey() );

            j++;
        }

        if (j % 2)
            cprintf(EOL);

        textcolor( BROWN );

        if (you.species == SP_UNKNOWN)
        {
            cprintf(EOL "Enter - Choose species first; "
                        "? - Random class; * - Random character; X - Quit"
                    EOL);
        }
        else
        {
            cprintf(EOL "? - Random; Bksp - Back to species selection; X - Quit"
                    EOL);
        }

        textcolor( CYAN );
        cprintf( EOL "What is your recent occupation? " );
        textcolor( LIGHTGREY );

        printed = true;
    }

    if (Options.cls)
        keyn = Options.cls;
    else
    {
        keyn = getch();
        if (keyn == 0)
        {
            getch();
            goto job_query;
        }
    }

    you.char_class = letter_to_class(keyn);
    if (you.char_class == JOB_UNKNOWN)
    {
        if (keyn == '?')
        {
            // pick a job at random... see god retribution for proof this
            // is uniform. -- bwr
            int job_count = 0;
            int job = -1;

            for (int k = 0; k < NUM_JOBS; k++)
            {
                if (class_allowed( you.species, k ))
                {
                    job_count++;
                    if (one_chance_in( job_count ))
                        job = k;
                }
            }

            ASSERT( job != -1 ); // at least one class should have been allowed
            you.char_class = job;
        }
        else if (keyn == '*')
        {
            pick_random_species_and_class();
            // used to give random weapon/god as well
            Options.random_pick = true;
            return (true);
        }
        else if (((keyn == '\n' || keyn == '\r') && you.species == SP_UNKNOWN)
                    || keyn == 'x' || keyn == ESCAPE || keyn == CK_BKSP)
        {
            you.char_class = JOB_UNKNOWN;
            Options.cls = '\0';
            return (false);
        }
        else if (keyn == 'X')
        {
            cprintf(EOL "Goodbye!");
            end(0);
        }
        else
        {
            if (Options.cls)
            {
                Options.cls = '\0';
                printed = false;
            }
            goto job_query;
        }
    }

    if (you.species != SP_UNKNOWN
        && !class_allowed( you.species, you.char_class ))
    {
        if (Options.cls)
        {
            Options.cls = '\0';
            printed = false;
        }
        goto job_query;
    }

    return (you.char_class != JOB_UNKNOWN && you.species != SP_UNKNOWN);
}


static void give_items_skills( void )
{
    char keyn;
    int weap_skill = 0;
    int to_hit_bonus;           // used for assigning primary weapons {dlb}
    int choice;                 // used for third-screen choices
    int tmp;
    int sk;

    const size_type  size = player_size();

    switch (you.char_class)
    {
    case JOB_FIGHTER:
        you.inv[0].quantity = 1;
        you.inv[0].base_type = OBJ_WEAPONS;
        you.inv[0].sub_type = WPN_SHORT_SWORD;
        you.inv[0].plus = 0;
        you.inv[0].plus2 = 0;
        you.inv[0].special = 0;
        you.inv[0].colour = LIGHTCYAN;

        if (size >= SIZE_LARGE)
        {
            you.inv[1].quantity = 1;
            you.inv[1].base_type = OBJ_ARMOUR;
            you.inv[1].sub_type = ARM_ANIMAL_SKIN;
            you.inv[1].plus = 0;
            you.inv[1].special = 0;
            you.inv[1].colour = BROWN;

            you.inv[0].quantity = 1;
            you.inv[0].base_type = OBJ_WEAPONS;
            you.inv[0].sub_type = WPN_CLUB;
            you.inv[0].plus = 0;
            you.inv[0].special = 0;
            you.inv[0].colour = BROWN;
        }
        else if (you.species == SP_GHOUL
                || you.species == SP_MUMMY
                || player_genus(GENPC_DRACONIAN))
        {
            you.inv[1].quantity = 1;
            you.inv[1].base_type = OBJ_ARMOUR;
            you.inv[1].sub_type = ARM_ROBE;
            you.inv[1].plus = 0;
            you.inv[1].special = 0;

            // undead robe is a grave shroud
            you.inv[1].colour = ((you.is_undead) ? WHITE
                                                 : element_colour( EC_RANDOM ));

            if (you.species != SP_GHOUL)
            {
                you.inv[2].quantity = 1;
                you.inv[2].base_type = OBJ_ARMOUR;
                you.inv[2].sub_type = ARM_SHIELD;
                you.inv[2].plus = 0;
                you.inv[2].special = 0;
                you.inv[2].colour = LIGHTCYAN;
            }
        }
        else if (size == SIZE_SMALL)
        {
            you.inv[1].quantity = 1;
            you.inv[1].base_type = OBJ_ARMOUR;
            you.inv[1].sub_type = ARM_STUDDED_LEATHER_ARMOUR;
            you.inv[1].plus = 0;
            you.inv[1].special = 0;
            you.inv[1].colour = BROWN;

            you.inv[2].quantity = 1;
            you.inv[2].base_type = OBJ_ARMOUR;
            you.inv[2].sub_type = ARM_BUCKLER;
            you.inv[2].plus = 0;
            you.inv[2].special = 0;
            you.inv[2].colour = BROWN;

            you.inv[3].base_type = OBJ_MISSILES;
            you.inv[3].sub_type = MI_DART;
            you.inv[3].quantity = 15 + roll_dice( 2, 10 );
            you.inv[3].plus = 0;
            you.inv[3].special = 0;
            you.inv[3].colour = LIGHTCYAN;

        }
        else
        {
            you.inv[1].quantity = 1;
            you.inv[1].base_type = OBJ_ARMOUR;
            you.inv[1].sub_type = ARM_SCALE_MAIL;
            you.inv[1].plus = 0;
            you.inv[1].special = 0;
            you.inv[1].colour = LIGHTCYAN;

            // these don't do so well with heavy armour
            if (you.species == SP_CENTAUR || you.species == SP_NAGA)
            {
                you.inv[1].sub_type = ARM_STUDDED_LEATHER_ARMOUR;
                you.inv[1].colour = BROWN;
            }

            you.inv[2].quantity = 1;
            you.inv[2].base_type = OBJ_ARMOUR;
            you.inv[2].sub_type = ARM_SHIELD;
            you.inv[2].plus = 0;
            you.inv[2].special = 0;
            you.inv[2].colour = LIGHTCYAN;
        }

        // wield weapon for those who use them, remove for those who don't
        if (you.species == SP_TROLL
            || you.species == SP_GIANT
            || you.species == SP_GHOUL)
        {
            you.inv[0].quantity = 0;
        }
        else
        {
            if (size == SIZE_MEDIUM)
                choose_weapon();

            you.equip[EQ_WEAPON] = 0;
        }

        you.equip[EQ_BODY_ARMOUR] = 1;

        // equip shield/buckler
        if (you.inv[2].quantity == 1 && you.inv[2].base_type == OBJ_ARMOUR)
            you.equip[EQ_SHIELD] = 2;

        you.skills[SK_FIGHTING] = 3;
        weap_skill = 2;

        if (size == SIZE_SMALL)
        {
            you.skills[SK_RANGED_COMBAT] = 1;
            you.skills[SK_DARTS] = 1;
            you.skills[SK_DODGING] = 2;
            you.skills[SK_STEALTH] = 1;
            you.skills[SK_SHIELDS] = 2;
        }
        else if (you.species == SP_GHOUL)
        {
            you.skills[SK_UNARMED_COMBAT] = 2;
            you.skills[SK_DODGING] = 2;
            you.skills[SK_STEALTH] = 2;
        }
        else if (you.equip[EQ_WEAPON] == -1)
        {
            // troll, giant, ghoul -- but ghoul's already handled above
            you.skills[SK_UNARMED_COMBAT] = 3;
            you.skills[SK_DODGING] = 3;
        }
        else if (you.species == SP_OGRE)
        {
            you.skills[SK_FIGHTING] += 2;
            you.skills[SK_DODGING] = 3;
        }
        else
        {
            // Players get dodging or armour skill depending on their
            // starting armour now (note: the armour has to be quiped
            // for this function to work)
            ASSERT( you.equip[EQ_BODY_ARMOUR] != -1 );

            you.skills[(player_light_armour() ? SK_DODGING : SK_ARMOUR)] = 3;

            you.skills[SK_SHIELDS] = 2;
            you.skills[SK_RANGED_COMBAT] = 2;
        }
        break;

    case JOB_WIZARD:
        you.inv[0].quantity = 1;
        you.inv[0].base_type = OBJ_WEAPONS;

        if (you.species == SP_OGRE_MAGE)
        {
            you.inv[0].sub_type = WPN_STAFF;
            you.inv[0].colour = BROWN;
        }
        else if (player_genus(GENPC_DWARVEN))
        {
            you.inv[0].sub_type = WPN_HAMMER;
            you.inv[0].colour = CYAN;
        }
        else
        {
            you.inv[0].sub_type = WPN_DAGGER;
            you.inv[0].colour = LIGHTCYAN;
        }

        you.inv[0].plus = 0;
        you.inv[0].plus2 = 0;
        you.inv[0].special = 0;

        you.inv[1].quantity = 1;
        you.inv[1].base_type = OBJ_ARMOUR;
        you.inv[1].sub_type = ARM_ROBE;
        you.inv[1].plus = 0;
        you.inv[1].colour = element_colour( EC_MAGIC );

        switch (random2(7))
        {
        case 0:
        case 1:
        default:
            set_equip_desc( you.inv[1], ISFLAG_SHINY );
            break;
        case 2:
        case 3:
            set_equip_desc( you.inv[1], ISFLAG_GLOWING );
            break;
        case 4:
        case 5:
            set_equip_desc( you.inv[1], ISFLAG_RUNED );
            break;
        case 6:
            set_equip_race( you.inv[1], ISFLAG_ELVEN );
            break;
        }


        you.equip[EQ_WEAPON] = 0;
        you.equip[EQ_BODY_ARMOUR] = 1;

        // extra items being tested:
        you.inv[2].base_type = OBJ_BOOKS;
        you.inv[2].sub_type = BOOK_MINOR_MAGIC_I + random2(3);
        you.inv[2].quantity = 1;
        you.inv[2].plus = 0;    // = 127
        you.inv[2].special = 1;
        you.inv[2].colour = element_colour( EC_MAGIC );

        weap_skill = 1;

        you.skills[SK_SPELLCASTING] = 3;
        you.skills[SK_CONJURATIONS] = 1;
        you.skills[SK_ENCHANTMENTS] = 1;
        you.skills[(coinflip() ? SK_CONJURATIONS : SK_ENCHANTMENTS)]++;

        // This used to be SK_SUMMONING + random2(5), this is
        // less interesting but more appropriate for use with
        // the starting books. -- bwr
        you.skills[(coinflip() ? SK_SUMMONINGS : SK_TRANSLOCATIONS)] = 1;

        if (you.species == SP_OGRE_MAGE || you.species == SP_CENTAUR)
        {
            weap_skill = 2;
            you.skills[SK_DODGING] = 3;
        }
        else
        {
            if (species_skills( SK_STAVES ) <= 100)
                you.skills[SK_STAVES] = 1;
            else
                weap_skill = 2;

            you.skills[SK_DODGING] = 1;
            you.skills[SK_STEALTH] = 1;
            you.skills[(coinflip() ? SK_DODGING : SK_STEALTH)]++;
        }
        break;

    case JOB_PRIEST:
        if (you.species == SP_DEMONSPAWN)
            you.religion = GOD_YREDELEMNUL;
        else if (Options.priest != GOD_NO_GOD && Options.priest != GOD_RANDOM)
            you.religion = Options.priest;
        else if (Options.random_pick || Options.priest == GOD_RANDOM)
            you.religion = coinflip() ? GOD_YREDELEMNUL : GOD_ZIN;
        else
        {
            clrscr();

            textcolor( CYAN );
            cprintf(EOL " Which god do you wish to serve?" EOL);

            textcolor( LIGHTGREY );
            cprintf("a - Zin (for traditional priests)" EOL);
            cprintf("b - Yredelemnul (for priests of death)" EOL);

            for (;;)
            {
                keyn = get_ch();

                if (keyn == 'a')
                    you.religion = GOD_ZIN;
                else if (keyn == 'b')
                    you.religion = GOD_YREDELEMNUL;

                if (you.religion != GOD_NO_GOD)
                    break;
            }
        }

        you.piety = 45;

        you.inv[0].quantity = 1;
        you.inv[0].base_type = OBJ_WEAPONS;
        you.inv[0].sub_type = WPN_MACE;
        you.inv[0].plus = 0;
        you.inv[0].plus2 = 0;
        you.inv[0].special = 0;
        you.inv[0].colour = LIGHTCYAN;

        you.inv[1].quantity = 1;
        you.inv[1].base_type = OBJ_ARMOUR;
        you.inv[1].sub_type = ARM_ROBE;
        you.inv[1].plus = 0;
        you.inv[1].special = 0;
        you.inv[1].colour = element_colour( EC_HOLY );

        you.inv[2].base_type = OBJ_POTIONS;
        you.inv[2].sub_type = POT_HEALING;
        you.inv[2].quantity = 2;
        you.inv[2].plus = 0;

        if (you.religion == GOD_YREDELEMNUL)
        {
            // equipment changes for dark priests
            you.inv[0].sub_type = WPN_FLAIL; // replacing mace
            you.inv[1].colour = element_colour( EC_UNHOLY ); // robe colour
            you.inv[2].sub_type = POT_MIGHT; // replacing healing potion
        }

        you.equip[EQ_WEAPON] = 0;
        you.equip[EQ_BODY_ARMOUR] = 1;

        you.skills[SK_FIGHTING] = 1;
        you.skills[SK_MACES_FLAILS] = 3;

        you.skills[SK_DODGING] = 2;

        if (you.religion == GOD_ZIN)
            you.skills[SK_SHIELDS] = 2;
        else
            you.skills[SK_TRAPS_DOORS] = 2;

        you.skills[SK_INVOCATIONS] = 4;
        break;

    case JOB_THIEF:
        you.inv[0].quantity = 1;
        you.inv[0].base_type = OBJ_WEAPONS;
        you.inv[0].sub_type = WPN_SHORT_SWORD;

        you.inv[0].plus = 0;
        you.inv[0].plus2 = 0;
        you.inv[0].special = 0;
        you.inv[0].colour = LIGHTCYAN;

        you.inv[1].quantity = 1;
        you.inv[1].base_type = OBJ_WEAPONS;
        you.inv[1].sub_type = WPN_DAGGER;

        you.inv[1].plus = 0;
        you.inv[1].plus2 = 0;
        you.inv[1].special = 0;
        you.inv[1].colour = LIGHTCYAN;

        you.inv[2].quantity = 1;
        you.inv[2].base_type = OBJ_ARMOUR;
        you.inv[2].sub_type = ARM_ROBE;
        you.inv[2].plus = 0;
        you.inv[2].special = 0;
        you.inv[2].colour = element_colour( EC_DARK );

        you.inv[3].quantity = 1;
        you.inv[3].base_type = OBJ_ARMOUR;
        you.inv[3].sub_type = ARM_CLOAK;
        you.inv[3].plus = 0;
        you.inv[3].special = 0;
        you.inv[3].colour = element_colour( EC_DARK );

        you.inv[4].base_type = OBJ_MISSILES;
        you.inv[4].sub_type = MI_DART;
        you.inv[4].quantity = 15 + roll_dice( 2, 10 );
        you.inv[4].plus = 0;
        you.inv[4].special = 0;
        you.inv[4].colour = LIGHTCYAN;

        you.equip[EQ_WEAPON] = 0;
        you.equip[EQ_BODY_ARMOUR] = 2;
        you.equip[EQ_CLOAK] = 3;

        you.skills[SK_FIGHTING] = 1;
        you.skills[SK_SHORT_BLADES] = 2;

        you.skills[SK_DODGING] = 2;
        you.skills[SK_STEALTH] = 2;
        you.skills[SK_STABBING] = 1;
        you.skills[SK_DODGING + random2(3)]++;

        you.skills[SK_TRAPS_DOORS] = 2;

        you.skills[SK_RANGED_COMBAT] = 1;
        you.skills[SK_DARTS] = 1;
        break;

    case JOB_GLADIATOR:
        you.inv[0].quantity = 1;
        you.inv[0].base_type = OBJ_WEAPONS;
        you.inv[0].sub_type = WPN_SHORT_SWORD;
        choose_weapon();

        you.inv[0].plus = 0;
        you.inv[0].plus2 = 0;
        you.inv[0].special = 0;
        you.inv[0].colour = LIGHTCYAN;

        if (player_genus(GENPC_DRACONIAN))
        {
            you.inv[1].quantity = 1;
            you.inv[1].base_type = OBJ_ARMOUR;
            you.inv[1].sub_type = ARM_ANIMAL_SKIN;
            you.inv[1].plus = 0;
            you.inv[1].special = 0;
            you.inv[1].colour = BROWN;

            you.inv[2].quantity = 1;
            you.inv[2].base_type = OBJ_ARMOUR;
            you.inv[2].sub_type = ARM_SHIELD;
            you.inv[2].plus = 0;
            you.inv[2].special = 0;
            you.inv[2].colour = LIGHTCYAN;
        }
        else
        {
            you.inv[1].quantity = 1;
            you.inv[1].base_type = OBJ_ARMOUR;
            you.inv[1].sub_type = ARM_STUDDED_LEATHER_ARMOUR;
            you.inv[1].plus = 0;
            you.inv[1].special = 0;
            you.inv[1].colour = BROWN;

            you.inv[2].quantity = 1;
            you.inv[2].base_type = OBJ_ARMOUR;
            you.inv[2].sub_type = ARM_BUCKLER;
            you.inv[2].plus = 0;
            you.inv[2].special = 0;
            you.inv[2].colour = BROWN;
        }

        you.equip[EQ_WEAPON] = 0;
        you.equip[EQ_BODY_ARMOUR] = 1;
        you.equip[EQ_SHIELD] = 2;

        you.skills[SK_FIGHTING] = 3;
        weap_skill = 3;

        you.skills[SK_DODGING] = 2;
        you.skills[SK_SHIELDS] = 1;
        you.skills[SK_UNARMED_COMBAT] = 2;
        break;


    case JOB_NECROMANCER:
        you.inv[0].quantity = 1;
        you.inv[0].base_type = OBJ_WEAPONS;
        you.inv[0].sub_type = WPN_DAGGER;
        you.inv[0].plus = 0;
        you.inv[0].plus2 = 0;
        you.inv[0].special = 0;
        you.inv[0].colour = LIGHTCYAN;

        you.inv[1].quantity = 1;
        you.inv[1].base_type = OBJ_ARMOUR;
        you.inv[1].sub_type = ARM_ROBE;
        you.inv[1].plus = 0;
        you.inv[1].special = 0;
        you.inv[1].colour = element_colour( EC_NECRO );

        you.equip[EQ_WEAPON] = 0;
        you.equip[EQ_BODY_ARMOUR] = 1;

        you.inv[2].base_type = OBJ_BOOKS;
        you.inv[2].sub_type = BOOK_NECROMANCY;
        you.inv[2].quantity = 1;
        you.inv[2].plus = 0;    // = 127
        you.inv[2].special = 0;     // = 1;
        you.inv[2].colour = element_colour( EC_NECRO );

        weap_skill = 1;

        if (you.species == SP_OGRE_MAGE)
        {
            you.inv[0].sub_type = WPN_STAFF;
            you.inv[0].colour = BROWN;

            weap_skill = 0;             // uses staff
            you.skills[SK_STAVES] = 1;  // boosted to 2 below
        }

        you.skills[SK_SPELLCASTING] = 2;
        you.skills[SK_NECROMANCY] = 4;

        you.skills[SK_DODGING] = 1;
        you.skills[SK_STEALTH] = 1;
        you.skills[(coinflip()? SK_DODGING : SK_STEALTH)]++;

        // staves to those good enough, else boost weapon (if appropriate)
        if (species_skills( SK_STAVES ) <= 100)
            you.skills[SK_STAVES]++;
        else if (weap_skill)
            weap_skill++;
        break;

    case JOB_PALADIN:
        you.religion = GOD_SHINING_ONE;
        you.piety = 28;

        you.inv[0].quantity = 1;
        you.inv[0].base_type = OBJ_WEAPONS;
        you.inv[0].sub_type = WPN_FALCHION;
        you.inv[0].plus = 0;
        you.inv[0].plus2 = 0;
        you.inv[0].special = 0;
        you.inv[0].colour = LIGHTCYAN;

        you.inv[1].quantity = 1;
        you.inv[1].base_type = OBJ_ARMOUR;
        you.inv[1].sub_type = ARM_RING_MAIL;
        you.inv[1].plus = 0;
        you.inv[1].special = 0;
        you.inv[1].colour = LIGHTCYAN;

        you.inv[2].quantity = 1;
        you.inv[2].base_type = OBJ_ARMOUR;
        you.inv[2].sub_type = ARM_SHIELD;
        you.inv[2].plus = 0;
        you.inv[2].special = 0;
        you.inv[2].colour = LIGHTCYAN;

        you.equip[EQ_WEAPON] = 0;
        you.equip[EQ_BODY_ARMOUR] = 1;
        you.equip[EQ_SHIELD] = 2;

        you.inv[3].base_type = OBJ_POTIONS;
        you.inv[3].sub_type = POT_HEALING;
        you.inv[3].quantity = 1;
        you.inv[3].plus = 0;

        you.skills[SK_FIGHTING] = 3;
        you.skills[SK_LONG_SWORDS] = 2;

        you.skills[SK_ARMOUR] = 2;
        you.skills[SK_SHIELDS] = 2;
        you.skills[(coinflip() ? SK_ARMOUR : SK_SHIELDS)]++;

        you.skills[SK_INVOCATIONS] = 3;
        break;

    case JOB_ASSASSIN:
        you.inv[0].quantity = 1;
        you.inv[0].base_type = OBJ_WEAPONS;
        you.inv[0].sub_type = WPN_DAGGER;
        to_hit_bonus = random2(3);
        you.inv[0].plus = 1 + to_hit_bonus;
        you.inv[0].plus2 = 1 + (2 - to_hit_bonus);
        you.inv[0].special = 0;
        you.inv[0].colour = LIGHTCYAN;

        you.inv[1].quantity = 1;
        you.inv[1].base_type = OBJ_WEAPONS;
        you.inv[1].sub_type = WPN_BLOWGUN;
        you.inv[1].plus = 0;
        you.inv[1].plus2 = 0;
        you.inv[1].special = 0;
        you.inv[1].colour = LIGHTGREY;

        you.inv[2].quantity = 1;
        you.inv[2].base_type = OBJ_ARMOUR;
        you.inv[2].sub_type = ARM_ROBE;
        you.inv[2].plus = 0;
        you.inv[2].special = 0;
        you.inv[2].colour = element_colour( EC_DEATH );

        you.inv[3].quantity = 1;
        you.inv[3].base_type = OBJ_ARMOUR;
        you.inv[3].sub_type = ARM_CLOAK;
        you.inv[3].plus = 0;
        you.inv[3].special = 0;
        you.inv[3].colour = element_colour( EC_DARK ); // outer layer

        you.inv[4].base_type = OBJ_MISSILES;
        you.inv[4].sub_type = MI_NEEDLE;
        you.inv[4].quantity = 15 + roll_dice( 2, 10 );
        you.inv[4].plus = 0;
        you.inv[4].colour = WHITE;
        set_item_ego_type( you.inv[4], OBJ_MISSILES, SPMSL_POISONED );

        // deep elves get hand crossbows, everyone else gets blowguns
        // (deep elves tend to suck at melee and need something that
        // can do ranged damage)
        if (you.species == SP_DEEP_ELF)
        {
            you.inv[1].sub_type = WPN_HAND_CROSSBOW;
            you.inv[1].colour = BROWN;

            you.inv[4].sub_type = MI_DART;
            you.inv[4].colour = LIGHTCYAN;
        }


        you.equip[EQ_WEAPON] = 0;
        you.equip[EQ_BODY_ARMOUR] = 2;
        you.equip[EQ_CLOAK] = 3;

        you.skills[SK_FIGHTING] = 1;
        you.skills[SK_SHORT_BLADES] = 3;

        you.skills[SK_RANGED_COMBAT] = 1;
        if (you.species == SP_DEEP_ELF)
            you.skills[SK_CROSSBOWS] = 1;
        else
            you.skills[SK_DARTS] = 1;

        you.skills[SK_STEALTH] = 3;
        you.skills[SK_STABBING] = 2;
        you.skills[SK_TRAPS_DOORS] = 1;
        break;

    case JOB_BERSERKER:
        you.religion = GOD_TROG;
        you.piety = 35;

        you.inv[0].quantity = 1;
        you.inv[0].base_type = OBJ_WEAPONS;
        you.inv[0].sub_type = WPN_HAND_AXE;
        you.inv[0].plus = 0;
        you.inv[0].plus2 = 0;
        you.inv[0].special = 0;
        you.inv[0].colour = LIGHTCYAN;
        you.equip[EQ_WEAPON] = 0;

        // WEAPONS
        if (you.species != SP_OGRE && you.species != SP_TROLL)
        {
            for (int i = 1; i <= 3; i++)
            {
                you.inv[i].quantity = 1;
                you.inv[i].base_type = OBJ_WEAPONS;
                you.inv[i].sub_type = WPN_SPEAR;
                you.inv[i].plus = 0;
                you.inv[i].plus2 = 0;
                you.inv[i].special = 0;
                you.inv[i].colour = LIGHTCYAN;
            }
        }

        // ARMOUR

        if (you.species == SP_OGRE || you.species == SP_TROLL
            || player_genus(GENPC_DRACONIAN))
        {
            you.inv[1].quantity = 1;
            you.inv[1].base_type = OBJ_ARMOUR;
            you.inv[1].sub_type = ARM_ANIMAL_SKIN;
            you.inv[1].plus = 0;
            you.inv[1].special = 0;
            you.inv[1].colour = BROWN;
            you.equip[EQ_BODY_ARMOUR] = 1;
        }
        else
        {
            you.inv[4].quantity = 1;
            you.inv[4].base_type = OBJ_ARMOUR;
            you.inv[4].sub_type = ARM_RING_MAIL;
            you.inv[4].plus = 0;
            you.inv[4].special = 0;
            you.inv[4].colour = LIGHTCYAN;
            you.equip[EQ_BODY_ARMOUR] = 4;
        }

        // SKILLS
        you.skills[SK_AXES] = 3;

        if (you.species == SP_OGRE || you.species == SP_TROLL)
        {
            you.skills[SK_FIGHTING] = 4;
            you.skills[SK_DODGING] = 3;
        }
        else
        {
            you.skills[SK_FIGHTING] = 2;
            you.skills[SK_POLEARMS] = 1;

            you.skills[SK_RANGED_COMBAT] = 3;
            you.skills[SK_ARMOUR] = 2;
        }
        break;

    case JOB_HUNTER:
        you.inv[0].quantity = 1;
        you.inv[0].base_type = OBJ_WEAPONS;
        you.inv[0].sub_type = WPN_DAGGER;
        you.inv[0].plus = 0;
        you.inv[0].plus2 = 0;
        you.inv[0].special = 0;
        you.inv[0].colour = LIGHTCYAN;

        you.inv[4].quantity = 1;
        you.inv[4].base_type = OBJ_ARMOUR;
        you.inv[4].sub_type = ARM_LEATHER_ARMOUR;
        you.inv[4].plus = 0;
        you.inv[4].special = 0;
        you.inv[4].colour = BROWN;

        if (you.species == SP_MERFOLK)
        {
            // Merfolk are spear hunters -- clobber bow, give three spears
            for (tmp = 1; tmp <= 3; tmp++)
            {
                you.inv[tmp].quantity = 1;
                you.inv[tmp].base_type = OBJ_WEAPONS;
                you.inv[tmp].sub_type = WPN_SPEAR;
                you.inv[tmp].plus = 0;
                you.inv[tmp].plus2 = 0;
                you.inv[tmp].special = 0;
                you.inv[tmp].colour = LIGHTCYAN;
            }
        }
        else
        {
            you.inv[2].quantity = 40 + roll_dice(4,5);
            you.inv[2].base_type = OBJ_MISSILES;
            you.inv[2].sub_type = MI_ARROW;
            you.inv[2].plus = 0;
            you.inv[2].plus2 = 0;
            you.inv[2].special = 0;
            you.inv[2].colour = BROWN;

            you.inv[1].quantity = 1;
            you.inv[1].base_type = OBJ_WEAPONS;
            you.inv[1].sub_type = WPN_BOW;
            you.inv[1].plus = 0;
            you.inv[1].plus2 = 0;
            you.inv[1].special = 0;
            you.inv[1].colour = BROWN;
        }

        if (player_genus(GENPC_DRACONIAN))
        {
            you.inv[4].sub_type = ARM_ROBE;
            you.inv[4].colour = GREEN;
        }

        you.equip[EQ_WEAPON] = 0;
        you.equip[EQ_BODY_ARMOUR] = 4;

        you.skills[SK_FIGHTING] = 1;
        weap_skill = 2;

        you.skills[SK_DODGING] = 2;
        you.skills[SK_STEALTH] = 2;
        you.skills[SK_TRAPS_DOORS] = 2;

        you.skills[SK_RANGED_COMBAT] = 3;

        switch (you.species)
        {
        case SP_HALFLING:
            you.inv[2].quantity += roll_zdice(2,8);
            you.inv[2].sub_type = MI_STONE;
            you.inv[2].colour = LIGHTGREY;
            you.inv[1].sub_type = WPN_SLING;

            you.skills[SK_SLINGS] = 2;
            break;

        case SP_GNOME:
            you.inv[1].sub_type = WPN_HAND_CROSSBOW;
            you.inv[2].sub_type = MI_DART;
            you.inv[2].colour = LIGHTCYAN;

            you.skills[SK_RANGED_COMBAT] = 2;
            you.skills[SK_CROSSBOWS] = 2;
            you.skills[SK_DARTS] = 2;
            break;

        case SP_HILL_DWARF:
        case SP_MOUNTAIN_DWARF:
            you.inv[0].sub_type = WPN_HAND_AXE;
            you.inv[1].sub_type = WPN_CROSSBOW;
            you.inv[2].sub_type = MI_BOLT;
            you.inv[2].colour = LIGHTCYAN;

            you.skills[SK_CROSSBOWS] = 2;
            break;

        case SP_MERFOLK:
            you.inv[0].sub_type = WPN_TRIDENT;
            you.skills[SK_RANGED_COMBAT]++;
            break;

        default:
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
        you.inv[0].quantity = 1;
        you.inv[0].base_type = OBJ_WEAPONS;
        you.inv[0].sub_type = WPN_DAGGER;
        you.inv[0].colour = LIGHTCYAN;
        you.inv[0].plus = 0;
        you.inv[0].plus2 = 0;
        you.inv[0].special = 0;

        you.inv[1].quantity = 1;
        you.inv[1].base_type = OBJ_ARMOUR;
        you.inv[1].sub_type = ARM_ROBE;
        you.inv[1].plus = 0;

        if (you.char_class == JOB_ENCHANTER)
        {
            you.inv[0].plus = 1;
            you.inv[0].plus2 = 1;
            you.inv[1].plus = 1;
        }

        you.inv[1].special = 0;

        you.equip[EQ_WEAPON] = 0;
        you.equip[EQ_BODY_ARMOUR] = 1;
        you.inv[2].base_type = OBJ_BOOKS;
        you.inv[2].sub_type = give_first_conjuration_book();
        you.inv[2].plus = 0;

        switch (you.char_class)
        {
        case JOB_SUMMONER:
            you.inv[1].colour = element_colour( EC_UNHOLY );

            you.inv[2].sub_type = BOOK_CALLINGS;
            you.inv[2].colour = element_colour( EC_UNHOLY );

            you.skills[SK_SUMMONINGS] = 4;

            // gets some darts - this class is difficult to start off with
            you.inv[3].base_type = OBJ_MISSILES;
            you.inv[3].sub_type = MI_DART;
            you.inv[3].quantity = 12 + roll_dice( 2, 8 );
            you.inv[3].plus = 0;
            you.inv[3].special = 0;
            you.inv[3].colour = LIGHTCYAN;
            break;

        case JOB_CONJURER:
            you.skills[SK_CONJURATIONS] = 4;
            you.inv[1].colour = element_colour( EC_RANDOM );
            you.inv[2].colour = element_colour( EC_RANDOM );
            break;

        case JOB_ENCHANTER:
            you.inv[1].colour = element_colour( EC_ENCHANT );
            you.inv[2].sub_type = BOOK_CHARMS;
            you.inv[2].colour = element_colour( EC_ENCHANT );

            you.skills[SK_ENCHANTMENTS] = 4;

            // gets some darts - this class is difficult to start off with
            you.inv[3].base_type = OBJ_MISSILES;
            you.inv[3].sub_type = MI_DART;
            you.inv[3].quantity = 12 + roll_dice( 2, 8 );
            you.inv[3].plus = 1;
            you.inv[3].special = 0;
            you.inv[3].colour = LIGHTCYAN;

            if (you.species == SP_SPRIGGAN)
            {
                you.inv[4].base_type = OBJ_MISCELLANY;
                you.inv[4].sub_type = MISC_ROD_OF_STRIKING;
                you.inv[4].quantity = 1;
                you.inv[4].special = 0;
                you.inv[4].colour = BROWN;
                you.inv[4].plus = 6;
                you.inv[4].plus2 = 6;
            }
            break;

        case JOB_FIRE_ELEMENTALIST:
            you.inv[1].colour = element_colour( EC_FIRE );
            you.inv[2].sub_type = BOOK_FLAMES;
            you.inv[2].colour = element_colour( EC_FIRE );

            you.skills[SK_FIRE_MAGIC] = 3;
            you.skills[SK_CONJURATIONS] = 1;
            break;

        case JOB_ICE_ELEMENTALIST:
            you.inv[1].colour = element_colour( EC_ICE );
            you.inv[2].sub_type = BOOK_FROST;
            you.inv[2].colour = element_colour( EC_ICE );

            you.skills[SK_ICE_MAGIC] = 3;
            you.skills[SK_CONJURATIONS] = 1;
            break;

        case JOB_AIR_ELEMENTALIST:
            you.inv[1].colour = element_colour( EC_AIR );
            you.inv[2].sub_type = BOOK_AIR;
            you.inv[2].colour = element_colour( EC_AIR );

            you.skills[SK_AIR_MAGIC] = 2;
            you.skills[SK_CONJURATIONS] = 1;
            you.skills[SK_ENCHANTMENTS] = 1;
            break;

        case JOB_EARTH_ELEMENTALIST:
            you.inv[1].colour = element_colour( EC_EARTH );
            you.inv[2].sub_type = BOOK_GEOMANCY;
            you.inv[2].colour = element_colour( EC_EARTH );

            you.inv[3].quantity = 10 + roll_dice(2,5);
            you.inv[3].base_type = OBJ_MISSILES;
            you.inv[3].sub_type = MI_STONE;
            you.inv[3].plus = 0;
            you.inv[3].plus2 = 0;
            you.inv[3].special = 0;
            you.inv[3].colour = LIGHTGREY;

            if (you.species == SP_GNOME)
            {
                you.inv[1].quantity = 1;
                you.inv[1].base_type = OBJ_WEAPONS;
                you.inv[1].sub_type = WPN_SLING;
                you.inv[1].plus = 0;
                you.inv[1].plus2 = 0;
                you.inv[1].special = 0;
                you.inv[1].colour = BROWN;

                you.inv[4].quantity = 1;
                you.inv[4].base_type = OBJ_ARMOUR;
                you.inv[4].sub_type = ARM_ROBE;
                you.inv[4].plus = 0;
                you.inv[4].plus2 = 0;
                you.inv[4].special = 0;
                you.inv[4].colour = element_colour( EC_EARTH );
                you.equip[EQ_BODY_ARMOUR] = 4;
            }

            you.skills[SK_EARTH_MAGIC] = 3;
            you.skills[SK_TRANSMIGRATION] = 1;
            break;

        case JOB_VENOM_MAGE:
            you.inv[1].colour = element_colour( EC_POISON );
            you.inv[2].sub_type = BOOK_YOUNG_POISONERS;
            you.inv[2].colour = element_colour( EC_POISON );

            you.skills[SK_POISON_MAGIC] = 4;
            break;
        }

        if (you.species == SP_OGRE_MAGE)
        {
            if (you.char_class == JOB_VENOM_MAGE)
                you.inv[0].sub_type = WPN_SHORT_SWORD;
            else
            {
                you.inv[0].sub_type = WPN_STAFF;
                you.inv[0].colour = BROWN;
            }
        }
        else if (player_genus(GENPC_DWARVEN))
        {
            you.inv[0].sub_type = WPN_HAMMER;
            you.inv[0].colour = CYAN;
        }

        you.inv[2].quantity = 1;
        you.inv[2].special = 0;

        you.skills[SK_SPELLCASTING] = 2;
        weap_skill = 1;

        if (you.char_class == JOB_SUMMONER)
        {
            if (species_skills(SK_POLEARMS) < species_skills(SK_MACES_FLAILS))
                you.inv[0].sub_type = WPN_SPEAR;
            else
            {
                you.inv[0].sub_type = WPN_WHIP;
                you.inv[0].colour = BROWN;
            }
        }
        else if (you.species == SP_SPRIGGAN && you.char_class == JOB_ENCHANTER)
        {
            you.skills[SK_EVOCATIONS] = 1;
            you.skills[SK_SHORT_BLADES] = 1;
            weap_skill = 0;
        }

        if (you.species == SP_GNOME && you.char_class == JOB_EARTH_ELEMENTALIST)
        {
            you.skills[SK_RANGED_COMBAT] = 1;
            you.skills[SK_SLINGS] = 1;
            you.skills[SK_DODGING] = 1;
            you.skills[SK_STEALTH] = 1;
        }
        else if (you.species == SP_CENTAUR || you.species == SP_DEMIGOD
                    || you.species == SP_HILL_DWARF)
        {
            you.skills[SK_FIGHTING] = 1;
            you.skills[SK_DODGING] = 2;

            if (you.skills[SK_TRANSMIGRATION] == 1)
                you.skills[SK_TRANSMIGRATION]++;
            else if (you.skills[SK_CONJURATIONS] == 1)
                you.skills[SK_CONJURATIONS]++;
            else if (you.skills[SK_ENCHANTMENTS] == 1)
                you.skills[SK_ENCHANTMENTS]++;
        }
        else if (you.species == SP_OGRE_MAGE)
        {
            if (you.char_class == JOB_VENOM_MAGE)
            {
                weap_skill = 2;
                you.skills[SK_DODGING] = 2;
            }
            else
            {
                weap_skill = 0;             // uses staff
                you.skills[SK_STAVES] = 1;  // boosted to 2 below

                you.skills[SK_FIGHTING] = 1;
                you.skills[SK_DODGING] = 2;
            }
        }
        else
        {
            you.skills[SK_DODGING] = 1;
            you.skills[SK_STEALTH] = 1;
            you.skills[ coinflip() ? SK_DODGING : SK_STEALTH ]++;
        }

        // staves to those good enough, else boost weapon (if appropriate)...
        // venom mages prefer non-blunt weapons
        if (you.char_class != JOB_VENOM_MAGE && species_skills(SK_STAVES) <= 100)
            you.skills[SK_STAVES]++;
        else if (weap_skill)
            weap_skill++;
        break;

    case JOB_TRANSMUTER:
        // some sticks for sticks to snakes:
        you.inv[1].quantity = 10 + roll_dice(3, 4);
        you.inv[1].base_type = OBJ_MISSILES;
        you.inv[1].sub_type = MI_ARROW;
        you.inv[1].plus = 0;
        you.inv[1].plus2 = 0;
        you.inv[1].special = 0;
        you.inv[1].colour = BROWN;

        you.inv[2].base_type = OBJ_ARMOUR;
        you.inv[2].sub_type = ARM_ROBE;
        you.inv[2].plus = 0;
        you.inv[2].special = 0;
        you.inv[2].quantity = 1;
        you.inv[2].colour = element_colour( EC_MUTAGENIC );

        you.inv[3].base_type = OBJ_BOOKS;
        you.inv[3].sub_type = BOOK_CHANGES;
        you.inv[3].quantity = 1;
        you.inv[3].plus = 0;
        you.inv[3].special = 0;
        you.inv[3].colour = element_colour( EC_MUTAGENIC );

        // A little bit of starting ammo for evaporate... don't need too
        // much now that the character can make their own. -- bwr
        //
        // some ammo for evaporate:
        you.inv[4].base_type = OBJ_POTIONS;
        you.inv[4].sub_type = POT_CONFUSION;
        you.inv[4].quantity = 2;
        you.inv[4].plus = 0;

        // some more ammo for evaporate:
        you.inv[5].base_type = OBJ_POTIONS;
        you.inv[5].sub_type = POT_POISON;
        you.inv[5].quantity = 1;
        you.inv[5].plus = 0;

        you.equip[EQ_WEAPON] = -1;
        you.equip[EQ_BODY_ARMOUR] = 2;

        you.skills[SK_FIGHTING] = 1;
        you.skills[SK_UNARMED_COMBAT] = 3;

        you.skills[SK_RANGED_COMBAT] = 2;

        you.skills[SK_DODGING] = 2;

        you.skills[SK_SPELLCASTING] = 2;
        you.skills[SK_TRANSMIGRATION] = 2;

        if (you.species == SP_SPRIGGAN)
        {
            you.inv[0].base_type = OBJ_MISCELLANY;
            you.inv[0].sub_type = MISC_ROD_OF_STRIKING;
            you.inv[0].quantity = 1;
            you.inv[0].special = 0;
            you.inv[0].colour = BROWN;
            you.inv[0].plus = 6;
            you.inv[0].plus2 = 6;

            you.skills[SK_EVOCATIONS] = 2;
            you.skills[SK_FIGHTING] = 0;

            you.equip[EQ_WEAPON] = 0;
        }
        break;

    case JOB_WARPER:
        you.inv[0].quantity = 1;
        you.inv[0].plus = 0;
        you.inv[0].plus2 = 0;
        you.inv[0].special = 0;
        you.inv[0].colour = LIGHTCYAN;

        you.inv[1].base_type = OBJ_ARMOUR;
        you.inv[1].sub_type = ARM_LEATHER_ARMOUR;
        you.inv[1].quantity = 1;
        you.inv[1].plus = 0;
        you.inv[1].special = 0;
        you.inv[1].colour = BROWN;

        if (you.species == SP_SPRIGGAN || you.species == SP_OGRE_MAGE)
        {
            you.inv[1].sub_type = ARM_ROBE;
            you.inv[1].colour = element_colour( EC_WARP );
        }

        you.inv[2].base_type = OBJ_BOOKS;
        you.inv[2].sub_type = BOOK_SPATIAL_TRANSLOCATIONS;
        you.inv[2].quantity = 1;
        you.inv[2].plus = 0;
        you.inv[2].special = 0;
        you.inv[2].colour = element_colour( EC_WARP );

        // one free escape:
        you.inv[3].base_type = OBJ_SCROLLS;
        you.inv[3].sub_type = SCR_BLINKING;
        you.inv[3].quantity = 1;
        you.inv[3].plus = 0;
        you.inv[3].special = 0;
        you.inv[3].colour = WHITE;

        you.inv[4].base_type = OBJ_MISSILES;
        you.inv[4].sub_type = MI_DART;
        you.inv[4].quantity = 15 + roll_dice(2, 10);
        you.inv[4].plus = 0;
        you.inv[4].special = 0;
        you.inv[4].colour = LIGHTCYAN;

        you.equip[EQ_WEAPON] = 0;
        you.equip[EQ_BODY_ARMOUR] = 1;

        if (you.species == SP_SPRIGGAN)
        {
            you.inv[0].base_type = OBJ_WEAPONS;
            you.inv[0].sub_type = WPN_DAGGER;

            you.inv[5].base_type = OBJ_MISCELLANY;
            you.inv[5].sub_type = MISC_ROD_OF_STRIKING;
            you.inv[5].quantity = 1;
            you.inv[5].special = 0;
            you.inv[5].colour = BROWN;
            you.inv[5].plus = 6;
            you.inv[5].plus2 = 6;

            you.skills[SK_EVOCATIONS] = 3;
        }
        else
        {
            you.inv[0].base_type = OBJ_WEAPONS;
            you.inv[0].sub_type = WPN_SHORT_SWORD;

            if (you.species == SP_OGRE_MAGE)
            {
                you.inv[0].sub_type = WPN_STAFF;
                you.inv[0].colour = BROWN;
            }

            you.skills[SK_FIGHTING] = 2;
            weap_skill = 2;
        }

        you.skills[SK_RANGED_COMBAT] = 1;
        you.skills[SK_DARTS] = 2;

        you.skills[SK_TRAPS_DOORS] = 1;

        you.skills[SK_SPELLCASTING] = 2;
        you.skills[SK_TRANSLOCATIONS] = 3;
        break;

    case JOB_CRUSADER:
        you.inv[0].quantity = 1;
        you.inv[0].base_type = OBJ_WEAPONS;
        you.inv[0].sub_type = WPN_SHORT_SWORD;

        //if (you.species == SP_OGRE_MAGE) you.inv_sub_type [0] = WPN_GLAIVE;

        you.inv[0].plus = 0;
        you.inv[0].plus2 = 0;
        you.inv[0].special = 0;
        you.inv[0].colour = LIGHTCYAN;
        choose_weapon();

        you.inv[1].quantity = 1;
        you.inv[1].base_type = OBJ_ARMOUR;
        you.inv[1].sub_type = ARM_ROBE;
        you.inv[1].plus = 0;
        you.inv[1].special = 0;
        you.inv[1].colour = element_colour( EC_ENCHANT );

        you.inv[2].base_type = OBJ_BOOKS;
        you.inv[2].sub_type = BOOK_WAR_CHANTS;
        you.inv[2].quantity = 1;
        you.inv[2].plus = 0;
        you.inv[2].special = 0;
        you.inv[2].colour = element_colour( EC_ENCHANT );

        you.equip[EQ_WEAPON] = 0;
        you.equip[EQ_BODY_ARMOUR] = 1;

        you.skills[SK_FIGHTING] = 3;
        weap_skill = 2;

        you.skills[SK_ARMOUR] = 1;
        you.skills[SK_DODGING] = 1;
        you.skills[SK_STEALTH] = 1;

        you.skills[SK_SPELLCASTING] = 2;
        you.skills[SK_ENCHANTMENTS] = 2;
        break;


    case JOB_DEATH_KNIGHT:
        you.inv[0].quantity = 1;
        you.inv[0].base_type = OBJ_WEAPONS;
        you.inv[0].sub_type = WPN_SHORT_SWORD;
        you.inv[0].plus = 0;
        you.inv[0].plus2 = 0;
        you.inv[0].special = 0;
        you.inv[0].colour = LIGHTCYAN;
        choose_weapon();

        you.inv[1].quantity = 1;
        you.inv[1].base_type = OBJ_ARMOUR;
        you.inv[1].sub_type = ARM_STUDDED_LEATHER_ARMOUR;
        you.inv[1].plus = 0;
        you.inv[1].special = 0;
        you.inv[1].colour = BROWN;

        you.equip[EQ_WEAPON] = 0;
        you.equip[EQ_BODY_ARMOUR] = 1;

        choice = DK_NO_SELECTION;

        // order is important here -- bwr
        if (you.species == SP_DEMIGOD)
            choice = DK_NECROMANCY;
        else if (Options.death_knight != DK_NO_SELECTION
                && Options.death_knight != DK_RANDOM)
        {
            choice = Options.death_knight;
        }
        else if (Options.random_pick || Options.death_knight == DK_RANDOM)
            choice = (coinflip() ? DK_NECROMANCY : DK_YREDELEMNUL);
        else
        {
            clrscr();

            textcolor( CYAN );
            cprintf(EOL " From where do you draw your power?" EOL);

            textcolor( LIGHTGREY );
            cprintf("a - Necromantic magic" EOL);
            cprintf("b - the god Yredelemnul" EOL);

          getkey1:
            keyn = get_ch();

            switch (keyn)
            {
            case 'a':
                cprintf(EOL "Very well.");
                choice = DK_NECROMANCY;
                break;
            case 'b':
                choice = DK_YREDELEMNUL;
                break;
            default:
                goto getkey1;
            }
        }

        weap_skill = 3;

        you.skills[SK_DODGING] = 3;
        you.skills[SK_STEALTH] = 2;

        switch (choice)
        {
        default:  // this shouldn't happen anyways -- bwr
        case DK_NECROMANCY:
            you.skills[SK_FIGHTING] = 1;
            you.skills[SK_SPELLCASTING] = 1;
            you.skills[SK_NECROMANCY] = 2;

            you.inv[2].base_type = OBJ_BOOKS;
            you.inv[2].sub_type = BOOK_NECROMANCY;
            you.inv[2].quantity = 1;
            you.inv[2].plus = 0;
            you.inv[2].special = 0;
            you.inv[2].colour = element_colour( EC_NECRO );
            break;

        case DK_YREDELEMNUL:
            you.religion = GOD_YREDELEMNUL;
            you.piety = 28;

            you.skills[SK_FIGHTING] = 2;
            you.skills[SK_INVOCATIONS] = 2;
            break;
        }
        break;

    case JOB_CHAOS_KNIGHT:
        you.piety = 25;         // irrelevant for Xom, of course
        you.inv[0].quantity = 1;
        you.inv[0].base_type = OBJ_WEAPONS;
        you.inv[0].sub_type = WPN_SHORT_SWORD;
        you.inv[0].plus = random2(3);
        you.inv[0].plus2 = random2(3);
        you.inv[0].special = 0;
        you.inv[0].colour = LIGHTCYAN;

        if (one_chance_in(5))
            set_equip_desc( you.inv[0], ISFLAG_RUNED );

        if (one_chance_in(5))
            set_equip_desc( you.inv[0], ISFLAG_GLOWING );

        choose_weapon();

        you.inv[1].quantity = 1;
        you.inv[1].base_type = OBJ_ARMOUR;
        you.inv[1].sub_type = ARM_ROBE;
        you.inv[1].plus = random2(3);
        you.inv[1].special = 0;
        you.inv[1].colour = element_colour( EC_RANDOM );

        you.equip[EQ_WEAPON] = 0;
        you.equip[EQ_BODY_ARMOUR] = 1;

        if (Options.chaos_knight != GOD_NO_GOD
            && Options.chaos_knight != GOD_RANDOM)
        {
            you.religion = Options.chaos_knight;
        }
        else if (Options.random_pick || Options.chaos_knight == GOD_RANDOM)
            you.religion = coinflip() ? GOD_XOM : GOD_MAKHLEB;
        else
        {
            clrscr();

            textcolor( CYAN );
            cprintf(EOL " Which god of chaos do you wish to serve?" EOL);

            textcolor( LIGHTGREY );
            cprintf("a - Xom of Chaos" EOL);
            cprintf("b - Makhleb the Destroyer" EOL);

          getkey2:

            keyn = get_ch();

            switch (keyn)
            {
            case 'a':
                you.religion = GOD_XOM;
                break;
            case 'b':
                you.religion = GOD_MAKHLEB;
                break;
            default:
                goto getkey2;
            }
        }

        you.skills[SK_FIGHTING] = 2;
        weap_skill = 3;

        if (you.religion == GOD_MAKHLEB)
            you.skills[SK_INVOCATIONS] = 3;
        else
        {
            you.skills[SK_FIGHTING]++;
            you.skills[SK_DODGING] = 1;
            inc_max_hp(3);
            dec_max_mp(1);
        }

        sk = 7;
        while (sk > 0)
        {
            tmp = SK_ARMOUR + random2(6);
            if (you.skills[tmp] >= 3)
                continue;

            sk -= ((you.skills[tmp] == 1) ? 1 : 2);
            you.skills[tmp]++;
        }
        break;

    case JOB_HEALER:
        you.religion = GOD_ELYVILON;
        you.piety = 45;

        you.inv[0].quantity = 1;
        you.inv[0].base_type = OBJ_WEAPONS;
        you.inv[0].sub_type = (size < SIZE_MEDIUM) ? WPN_STAFF
                                                   : WPN_QUARTERSTAFF;
        you.inv[0].colour = BROWN;
        you.inv[0].plus = 0;
        you.inv[0].plus2 = 0;
        you.inv[0].special = 0;

        // Robe
        you.inv[1].quantity = 1;
        you.inv[1].base_type = OBJ_ARMOUR;
        you.inv[1].sub_type = ARM_ROBE;
        you.inv[1].plus = 0;
        you.inv[1].special = 0;
        you.inv[1].colour = element_colour( EC_HEAL );

        you.inv[2].base_type = OBJ_POTIONS;
        you.inv[2].sub_type = POT_HEALING;
        you.inv[2].quantity = 1;
        you.inv[2].plus = 0;

        you.inv[3].base_type = OBJ_POTIONS;
        you.inv[3].sub_type = POT_HEAL_WOUNDS;
        you.inv[3].quantity = 1;
        you.inv[3].plus = 0;

        you.equip[EQ_WEAPON] = 0;
        you.equip[EQ_BODY_ARMOUR] = 1;

        you.skills[SK_FIGHTING] = 2;
        you.skills[SK_STAVES] = 3;

        you.skills[SK_RANGED_COMBAT] = 2;
        you.skills[SK_DODGING] = 2;
        you.skills[SK_SHIELDS] = 1;

        you.skills[SK_INVOCATIONS] = 3;
        break;

    case JOB_REAVER:
        you.inv[0].quantity = 1;
        you.inv[0].base_type = OBJ_WEAPONS;
        you.inv[0].sub_type = WPN_SHORT_SWORD;
        you.inv[0].plus = 0;
        you.inv[0].plus2 = 0;
        you.inv[0].special = 0;
        you.inv[0].colour = LIGHTCYAN;
        choose_weapon();

        you.inv[1].quantity = 1;
        you.inv[1].base_type = OBJ_ARMOUR;
        you.inv[1].sub_type = ARM_ROBE;
        you.inv[1].plus = 0;
        you.inv[1].special = 0;
        you.inv[1].colour = RED;

        you.inv[2].base_type = OBJ_BOOKS;
        you.inv[2].sub_type = give_first_conjuration_book();
        you.inv[2].quantity = 1;
        you.inv[2].plus = 0;    // = 127
        you.inv[2].special = 0;
        you.inv[2].colour = RED;

        you.equip[EQ_WEAPON] = 0;
        you.equip[EQ_BODY_ARMOUR] = 1;

        you.skills[SK_FIGHTING] = 2;
        weap_skill = 3;

        you.skills[SK_ARMOUR] = 1;
        you.skills[SK_DODGING] = 1;

        you.skills[SK_SPELLCASTING] = 2;
        you.skills[SK_CONJURATIONS] = 2;
        break;

    case JOB_STALKER:
        you.inv[0].quantity = 1;
        you.inv[0].base_type = OBJ_WEAPONS;
        you.inv[0].sub_type = WPN_DAGGER;
        to_hit_bonus = random2(3);
        you.inv[0].plus = 1 + to_hit_bonus;
        you.inv[0].plus2 = 1 + (2 - to_hit_bonus);
        you.inv[0].special = 0;
        you.inv[0].colour = LIGHTCYAN;

        you.inv[1].quantity = 1;
        you.inv[1].base_type = OBJ_ARMOUR;
        you.inv[1].sub_type = ARM_ROBE;
        you.inv[1].plus = 0;
        you.inv[1].special = 0;
        you.inv[1].colour = element_colour( EC_POISON );

        you.inv[2].quantity = 1;
        you.inv[2].base_type = OBJ_ARMOUR;
        you.inv[2].sub_type = ARM_CLOAK;
        you.inv[2].plus = 0;
        you.inv[2].special = 0;
        you.inv[2].colour = element_colour( EC_DARK );

        you.inv[3].base_type = OBJ_BOOKS;
        you.inv[3].sub_type = BOOK_STALKING;   //jmf: new book!
        you.inv[3].quantity = 1;
        you.inv[3].plus = 0;
        you.inv[3].special = 0;
        you.inv[3].colour = element_colour( EC_POISON );

        you.equip[EQ_WEAPON] = 0;
        you.equip[EQ_BODY_ARMOUR] = 1;
        you.equip[EQ_CLOAK] = 2;

        you.skills[SK_FIGHTING] = 1;
        you.skills[SK_SHORT_BLADES] = 1;

        you.skills[SK_STEALTH] = 2;
        you.skills[SK_STABBING] = 2;

        you.skills[SK_SPELLCASTING] = 1;
        you.skills[SK_ENCHANTMENTS] = 2;
        you.skills[SK_POISON_MAGIC] = 2;

        if (you.species == SP_DEMIGOD)
        {
            you.skills[SK_SHORT_BLADES]++;
            you.skills[SK_SPELLCASTING]++;
        }
        else
        {
            tmp = random2(3);
            switch (tmp)
            {
            case 0: you.skills[SK_STEALTH]++; break;
            case 1: you.skills[SK_STABBING]++; break;
            case 2: you.skills[SK_TRAPS_DOORS]++; break;
            }
        }
        break;

    case JOB_MONK:
        you.inv[0].base_type = OBJ_ARMOUR;
        you.inv[0].sub_type = ARM_ROBE;
        you.inv[0].plus = 0;
        you.inv[0].special = 0;
        you.inv[0].quantity = 1;
        you.inv[0].colour = LIGHTRED;

        you.equip[EQ_WEAPON] = -1;
        you.equip[EQ_BODY_ARMOUR] = 0;

        you.skills[SK_FIGHTING] = 3;
        you.skills[SK_UNARMED_COMBAT] = 4;

        you.skills[SK_DODGING] = 3;
        you.skills[SK_STEALTH] = 2;
        break;

    case JOB_WANDERER:
        create_wanderer();
        break;
    }

    if (weap_skill)
        you.skills[ weapon_skill(you.inv[0]) ] = weap_skill;

    init_skill_order();

    if (you.religion != GOD_NO_GOD)
    {
        you.worshipped[you.religion] = 1;
        set_god_ability_slots();
    }
}

/*
 *  File:       acr.cc
 *  Summary:    Main entry point, event loop, and some initialization functions
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 * <18> 7/29/00         JDJ             values.h isn't included on Macs
 * <17> 19jun2000       GDL             added Windows console support
 * <16> 06mar2000       bwr             changes to berserk
 * <15> 09jan2000       BCR             new Wiz command: blink
 * <14> 10/13/99        BCR             Added auto door opening,
 *                                       move "you have no
 *                                       religion" to describe.cc
 * <13> 10/11/99        BCR             Added Daniel's wizard patch
 * <12> 10/9/99         BCR             swapped 'v' and 'V' commands,
 *                                       added wizard help command
 * <11> 10/7/99         BCR             haste and slow now take amulet of
 *                                       resist slow into account
 * <10> 9/25/99         CDL             Changes to Linux input
 *                                       switch on command enums
 * <9>  6/12/99         BWR             New init code, restructured
 *                                       wiz commands, added equipment
 *                                       listing commands
 * <8>   6/7/99         DML             Autopickup
 * <7>  5/30/99         JDJ             Added game_has_started.
 * <6>  5/25/99         BWR             Changed move() to move_player()
 * <5>  5/20/99         BWR             New berserk code, checking
 *                                       scan_randarts for NO_TELEPORT
 *                                       and NO_SPELLCASTING.
 * <4>  5/12/99         BWR             Solaris support.
 * <3>  5/09/99         JDJ             look_around no longer prints a prompt.
 * <2>  5/08/99         JDJ             you and env are no longer arrays.
 * <1>  -/--/--         LRH             Created
 */

#include "AppHdr.h"

#include <string>

// I don't seem to need values.h for VACPP..
#if !defined(__IBMCPP__) && !defined(MAC)
#include <limits.h>
#endif

#if DEBUG
  // this contains the DBL_MAX constant
  #include <float.h>
#endif

#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <stdio.h>

#ifdef DOS
#include <dos.h>
#include <conio.h>
#include <file.h>
#endif

#ifdef USE_UNIX_SIGNALS
#include <signal.h>
#endif

#ifdef USE_EMX
#include <sys/types.h>
#endif

#ifdef OS9
#include <stat.h>
#else
#include <sys/stat.h>
#endif

#include "globals.h"
#include "externs.h"

#include "abl-show.h"
#include "abyss.h"
#include "chardump.h"
#include "cloud.h"
#include "command.h"
#include "debug.h"
#include "decks.h"
#include "delay.h"
#include "describe.h"
#include "direct.h"
#include "dungeon.h"
#include "effects.h"
#include "fight.h"
#include "files.h"
#include "food.h"
#include "hiscores.h"
#include "initfile.h"
#include "invent.h"
#include "item_use.h"
#include "it_use2.h"
#include "it_use3.h"
#include "itemname.h"
#include "itemprop.h"
#include "items.h"
#include "lev-pand.h"
#include "macro.h"
#include "misc.h"
#include "monplace.h"
#include "monstuff.h"
#include "mon-util.h"
#include "mutation.h"
#include "newgame.h"
#include "ouch.h"
#include "output.h"
#include "overmap.h"
#include "player.h"
#include "randart.h"
#include "religion.h"
#include "shopping.h"
#include "skills.h"
#include "skills2.h"
#include "spells1.h"
#include "spells2.h"
#include "spells3.h"
#include "spells4.h"
#include "spl-book.h"
#include "spl-cast.h"
#include "spl-util.h"
#include "stuff.h"
#include "tags.h"
#include "transfor.h"
#include "travel.h"
#include "stash.h"
#include "view.h"

#ifdef WIZARD

#include "shopping.h"

#endif

// Functions in main module
static void close_door(char move_x, char move_y);
static void do_berserk_no_combat_penalty(void);
static bool initialise(void);
static void input(void);
static void open_door( const int move_x, const int move_y );
static void move_player(char move_x, char move_y);
static void handle_player_enchantments( void );

/*
   It all starts here. Some initialisations are run first, then straight to
   new_game and then input.
*/
int main( int argc, char *argv[] )
{
#ifdef DEBUG_DUMP_SKILL_TOTALS
    // need to init a few things for this dump to work:
    seed_rng();
    init_playerspells();
    init_properties();

    dump_skill_totals();
    exit(0);
#endif

    // Load in the system environment variables
    get_system_environment();

    // parse command line args -- look only for initfile & crawl_dir entries
    if (!parse_args(argc, argv, true))
    {
        // print help
        puts("Command line options:");
        puts("  -name <string>   character name");
        puts("  -race <arg>      preselect race (by letter, abbreviation, or name)");
        puts("  -class <arg>     preselect class (by letter, abbreviation, or name)");
        puts("  -pizza <string>  crawl pizza");
        puts("  -plain           don't use IBM extended characters");
        puts("  -dir <path>      crawl directory");
        puts("  -rc <file>       init file name");
        puts("  -macro <file>    macro file name");
        puts("  -morgue <dir>    directory to save character dumps");
        puts("");
        puts("Command line options override init file options, which override");
        puts("environment options (CRAWL_NAME, CRAWL_PIZZA, CRAWL_DIR, CRAWL_RC).");
        puts("");
        puts("Highscore list options: (Can now be redirected to more, etc)");
        puts("  -scores [N]      highscore list");
        puts("  -tscores [N]     terse highscore list");
        puts("  -vscores [N]     verbose highscore list");
        exit(1);
    }

    // Read the init file
    read_init_file();

    // now parse the args again, looking for everything else.
    parse_args( argc, argv, false );

    // print hiscores and exit if requested
    if (Options.sc_entries > 0)
    {
        printf( " Best Crawlers -" EOL );
        hiscores_print_list( Options.sc_entries, Options.sc_format );
        exit(0);
    }

    bool game_start = initialise();

    if (game_start || Options.always_greet)
    {
        mpr( "Welcome, %s the %s %s.",
             you.your_name, species_name( you.species ), you.class_name );

        // Starting messages can go here as this should only happen
        // at the start of a new game -- bwr
        // This message isn't appropriate for Options.always_greet
        if (you.char_class == JOB_WANDERER && game_start)
        {
            int skill_levels = 0;
            for (int i = 0; i <= NUM_SKILLS; i++)
                skill_levels += you.skills[i];

            if (skill_levels <= 2)
            {
                // Demigods and Demonspawn wanderers stand to not be
                // able to see any of their skills at the start of
                // the game (one or two skills should be easily guessed
                // from starting equipment)... Anyways, we'll give the
                // player a message to warn them (and give a reason why). -- bwr
                mpr("You wake up in a daze, and can't recall much.");
            }
        }

        // These need some work -- should make sure that the god's
        // name is metioned, else the message might be confusing.
        switch (you.religion)
        {
        case GOD_ZIN:
            simple_god_message( " says: Spread the light, my child." );
            break;
        case GOD_SHINING_ONE:
            simple_god_message( " says: Smite the infidels!" );
            break;
        case GOD_KIKUBAAQUDGHA:
        case GOD_YREDELEMNUL:
        case GOD_NEMELEX_XOBEH:
            simple_god_message( " says: Welcome..." );
            break;
        case GOD_XOM:
            if (game_start)
                simple_god_message( " says: A new plaything!" );
            break;
        case GOD_VEHUMET:
            god_speaks( you.religion, "Let it end in hellfire!");
            break;
        case GOD_OKAWARU:
            simple_god_message(" says: Welcome, disciple.");
            break;
        case GOD_MAKHLEB:
            god_speaks( you.religion, "Blood and souls for Makhleb!" );
            break;
        case GOD_SIF_MUNA:
            simple_god_message( " whispers: I know many secrets...");
            break;
        case GOD_TROG:
            simple_god_message( " says: Kill them all!" );
            break;
        case GOD_ELYVILON:
            simple_god_message( " says: Go forth and aid the weak!" );
            break;
        default:
            break;
        }

        // warn player about their weapon, if unsuitable
        wield_warning( false );
    }

    // Make sure you.stealth_cache is calculated for the first turn
    you.stealth_cache = player_stealth();

    while (true)
    {
        input();
        //      cprintf("x");
    }

    // Should never reach this stage, right?

#ifdef LINUX
    lincurses_shutdown();
#endif

#ifdef MAC
    deinit_mac();
#endif

#ifdef USE_EMX
    deinit_emx();
#endif

    return 0;
}                               // end main()

#ifdef WIZARD
static void handle_wizard_command( void )
{
    int   wiz_command, i, j, tmp;
    char  specs[256];

    // WIZ_NEVER gives protection for those who have wiz compiles,
    // and don't want to risk their characters.
    if (Options.wiz_mode == WIZ_NEVER)
        return;

    if (!you.wizard)
    {
        mpr( MSGCH_WARN, "WARNING: ABOUT TO ENTER WIZARD MODE!" );

#ifndef SCORE_WIZARD_MODE
        mpr( MSGCH_WARN, "If you continue, your game will not be scored!" );
#endif

        if (!yesno( "Do you really want to enter wizard mode?", false ))
            return;

        you.wizard = true;
        redraw_screen();
    }

    mpr( MSGCH_PROMPT, "Enter Wizard Command: " );
    wiz_command = getch();

    switch (wiz_command)
    {
    case '?':
        list_commands(true);        // tell it to list wizard commands
        redraw_screen();
        break;

    case CONTROL('G'):
        save_ghost(true);
        break;

    case 'x':
        you.xp = 1 + exp_needed( 2 + you.xp_level );
        level_change();
        break;

    case 's':
        you.exp_available = 20000;
        set_redraw_status( REDRAW_EXPERIENCE );
        break;

    case 'S':
        debug_set_skills();
        break;

    case 'A':
        debug_set_all_skills();
        break;

    case '$':
        you.gold += 1000;
        set_redraw_status( REDRAW_GOLD );
        break;

    case 'a':
        acquirement( OBJ_RANDOM );
        break;

#if DEBUG_DIAGNOSTICS
    case '#':
        dump_random_items();
        break;
#endif

    case '+':
        i = prompt_invent_item( "Make an artefact out of which item?", -1 );
        if (i == PROMPT_ABORT)
        {
            canned_msg( MSG_OK );
            break;
        }

        if (you.inv[i].base_type != OBJ_WEAPONS
            && you.inv[i].base_type != OBJ_ARMOUR
            && you.inv[i].base_type != OBJ_JEWELLERY)
        {
            canned_msg( MSG_OK );
            break;
        }


        // set j == equipment slot of chosen item, remove old randart benefits
        for (j = 0; j < NUM_EQUIP; j++)
        {
            if (you.equip[j] == i)
            {
                if (j == EQ_WEAPON)
                    set_redraw_status( REDRAW_WIELD );

                if (is_random_artefact( you.inv[i] ))
                    unuse_randart( i );

                break;
            }
        }

        make_item_randart( you.inv[i] );

        // if equiped, apply new randart benefits
        if (j != NUM_EQUIP)
            use_randart( i );

        item_name( you.inv[i], DESC_INVENTORY_EQUIP, info );
        mpr( info );


#if DEBUG_DIAGNOSTICS
        mpr( MSGCH_DIAGNOSTICS, "randart val: %d", randart_value(you.inv[i]) );
#endif
        break;

    case '|':
        // create all unrand arts
        for (tmp = 1; tmp < NO_UNRANDARTS; tmp++)
        {
            int islot = get_item_slot();
            if (islot == NON_ITEM)
                break;

            make_item_unrandart( mitm[islot], tmp );

            mitm[ islot ].quantity = 1;
            set_ident_flags( mitm[ islot ], ISFLAG_IDENT_MASK );

            move_item_to_grid( &islot, you.x_pos, you.y_pos );
        }

        // create all fixed artefacts
        for (tmp = SPWPN_SINGING_SWORD; tmp <= SPWPN_STAFF_OF_WUCAD_MU; tmp++)
        {
            int islot = get_item_slot();
            if (islot == NON_ITEM)
                break;

            if (make_item_fixed_artefact( mitm[ islot ], false, tmp ))
            {
                mitm[ islot ].quantity = 1;
                item_colour( mitm[ islot ] );
                set_ident_flags( mitm[ islot ], ISFLAG_IDENT_MASK );

                move_item_to_grid( &islot, you.x_pos, you.y_pos );
            }
        }
        break;

    case 'B':
        if (you.level_type != LEVEL_ABYSS)
            banished( DNGN_ENTER_ABYSS );
        else
        {
            down_stairs( DNGN_EXIT_ABYSS, you.depth, false );
            untag_followers();
        }
        break;

    case 'g':
        debug_add_skills();
        break;

    case 'G':
        // Genocide... "unsummon" all the monsters from the level.
        for (int mon = 0; mon < MAX_MONSTERS; mon++)
        {
            struct monsters *monster = &menv[mon];

            if (monster->type == -1)
                continue;

            monster_die( monster, KILL_RESET, MHITYOU );
        }
        break;

    case 'h':
        you.magic_contamination = 0;
        purification();
        set_hp( you.hp_max, false );
        inc_mp( 50, false );
        set_hunger( 5000 + abs(you.hunger), true );
        break;

    case 'H':
        you.magic_contamination = 0;
        purification();
        you.duration[DUR_LIQUID_FLAMES] = 0;
        inc_hp( 50, true );
        set_hp( you.hp_max, false );
        inc_mp( 50, true );
        set_hunger( 12000, true );
        set_redraw_status( REDRAW_HIT_POINTS );
        break;

    case 'b':
        blink();            // wizards can always blink
        break;

    case 'c':
        mpr( MSGCH_PROMPT, "Which card effect (by number)? " );
        get_input_line( specs, sizeof( specs ) );

        if (specs[0] != '\0')
            cards( atoi(specs), false );
        break;

    case '\"':
    case '~':
        level_travel(0);
        break;

    case 'd':
    case 'D':
        level_travel(1);
        break;

    case CONTROL('D'):
        debug_destroy_item();
        break;

    case 'u':
    case 'U':
        level_travel(-1);
        break;

    case '.':
        {
            // This is completely unsafe, but is somewhat useful for
            // debuging those rare game hangs in level creation.
            builder( you.depth, you.level_type );
            clear_envmap();
            you_teleport2( false );
        }
        break;

    case '%':
    case 'o':
        create_spec_object();
        break;

    case 'E':
    case 'O':
        create_object( (wiz_command == 'E') );
        break;

    case 't':
        tweak_object();
        break;

    case 'e':
        {
            struct dist spd;
            struct bolt beam;

            if (spell_direction( spd, beam, DIR_TARGET, TARG_ENEMY ))
            {
                const int mon = mgrd[beam.target_x][beam.target_y];

                if (mon != NON_MONSTER)
                    menv[mon].attitude = ATT_FRIENDLY;
            }
        }
        break;

    case 'm':
        create_spec_monster();
        break;

    case 'M':
        create_spec_monster_name();
        break;

    case CONTROL('M'):
        mpr( MSGCH_DIAGNOSTICS, "Scaning monsters:" );
        for (i = 0; i < MAX_MONSTERS; i++)
        {
            struct monsters *mon = &menv[i];

            if (mon->type == -1)
                continue;

            mpr( MSGCH_DIAGNOSTICS,
                 "%s (#%d): location (%d,%d)",
                 ptr_monam( mon, DESC_PLAIN ), i, mon->x, mon->y );
        }

        mpr( MSGCH_DIAGNOSTICS, "Scaning grid:" );
        for (i = 0; i < GXM; i++)
        {
            for (j = 0; j < GYM; j++)
            {
                const int mid = mgrd[i][j];

                if (mid != NON_MONSTER)
                {
                    struct monsters *mon = &menv[mid];

                    mpr( MSGCH_DIAGNOSTICS,
                         "location (%d,%d): %s (#%d)",
                         i, j, ptr_monam( mon, DESC_PLAIN ), mid );
                }
            }
        }
        break;

    case 'r':
        debug_change_species();
        break;

    case '>':
        grd[you.x_pos][you.y_pos] = DNGN_STONE_STAIRS_DOWN_I;
        break;

    case '<':
        grd[you.x_pos][you.y_pos] = DNGN_ROCK_STAIRS_UP;
        break;

    case 'p':
        if (you.level_type != LEVEL_PANDEMONIUM)
            grd[you.x_pos][you.y_pos] = DNGN_ENTER_PANDEMONIUM;
        else
            grd[you.x_pos][you.y_pos] = DNGN_TRANSIT_PANDEMONIUM;
        break;

    case 'P':
        gain_exp( 9000000 );
        debug_set_all_skills( 27 );
        break;

    case 'l':
        grd[you.x_pos][you.y_pos] = DNGN_ENTER_LABYRINTH;
        break;

    case '\\':
        if (grd[you.x_pos][you.y_pos] == DNGN_ENTER_SHOP)
        {
            // replace shop underfoot
            for (i = 0; i < MAX_SHOPS; i++)
            {
                if (env.shop[i].x == you.x_pos && env.shop[i].y == you.y_pos)
                    break;
            }

            if (i < MAX_SHOPS)
            {
                destroy_item_stack( 0, 5 + i );
                grd[ env.shop[i].x ][ env.shop[i].y ] = DNGN_FLOOR;
                env.shop[i].type = SHOP_UNASSIGNED;
            }
        }
        else
        {
            // check if there's a free shop available
            for (i = 0; i < MAX_SHOPS; i++)
            {
                if (env.shop[i].type == SHOP_UNASSIGNED)
                    break;
            }
        }

        // if there isn't, pick one and clear it out
        if (i == MAX_SHOPS)
        {
            i = random2(MAX_SHOPS);

            destroy_item_stack( 0, 5 + i );
            grd[ env.shop[i].x ][ env.shop[i].y ] = DNGN_FLOOR;
            env.shop[i].type = SHOP_UNASSIGNED;
        }

        grd[you.x_pos][you.y_pos] = DNGN_FLOOR;

        place_spec_shop( you.depth, you.x_pos, you.y_pos, SHOP_RANDOM );
        link_items();
        item_check( false );
        break;

    case 'i':
        mpr( "You feel a rush of knowledge." );
        for (i = 0; i < ENDOFPACK; i++)
        {
            if (is_valid_item( you.inv[i] ))
            {
                set_ident_type( you.inv[i].base_type, you.inv[i].sub_type,
                                ID_KNOWN_TYPE );

                set_ident_flags( you.inv[i], ISFLAG_IDENT_MASK );
            }
        }
        set_redraw_status( REDRAW_WIELD );
        break;

    case 'I':
        mpr( "You feel a rush of antiknowledge." );
        for (i = 0; i < ENDOFPACK; i++)
        {
            if (is_valid_item( you.inv[i] ))
            {
                set_ident_type( you.inv[i].base_type, you.inv[i].sub_type,
                                ID_UNKNOWN_TYPE );

                unset_ident_flags( you.inv[i], ISFLAG_IDENT_MASK );
            }
        }
        set_redraw_status( REDRAW_WIELD );
        break;

    case 'X':
        Xom_acts(true, 20, true);
        break;

    case 'z':
        cast_spec_spell();
        break;              /* cast spell by number */

    case 'Z':
        cast_spec_spell_name();
        break;              /* cast spell by name */

    case '(':
        mpr( MSGCH_PROMPT, "Create which feature (by number)? " );
        get_input_line( specs, sizeof( specs ) );

        if (specs[0] != '\0')
            grd[you.x_pos][you.y_pos] = atoi(specs);
        break;

    case ')':
        mpr( MSGCH_PROMPT, "Create pool of which feature (by number)? " );
        get_input_line( specs, sizeof( specs ) );

        if (specs[0] != '\0')
        {
            const int grid = atoi(specs);

            for (int gx = you.x_pos - 2; gx <= you.x_pos + 2; gx++)
            {
                for (int gy = you.y_pos - 2; gy <= you.y_pos + 2; gy++)
                {
                    if (in_bounds( gx, gy ))
                        grd[gx][gy] = grid;
                }
            }
        }
        break;

    case ']':
        if (!debug_add_mutation())
            mpr( "Failure to give mutation." );
        break;

    case '[':
        demonspawn();
        break;

    case ':':
        {
            const char bnch[13][20] =
            {
                "Orcish Mines", "Hive", "Lair", "Slime Pits", "Vaults",
                "Crypt", "Hall of Blades", "Hall of Zot", "Ecumenical Temple",
                "Snake Pit", "Elven Halls", "Tomb", "Swamp"
            };

            j = 0;
            for (i = 0; i < 13; i++)
            {
                if (you.branch_stairs[i] == 0)
                    continue;

                mpr( "%2d: %s", you.branch_stairs[i] + 1, bnch[i] );
            }
        }
        break;

    case '{':
        magic_mapping(99, 100);
        detect_traps(100);
        detect_items(100);
        detect_creatures(100);
        break;

    case '^':
        {
            const int old_piety = you.piety;

            gain_piety(20);

            mpr( "Congratulations, your piety went from %d to %d!",
                 old_piety, you.piety);
        }
        break;

    case '=':
        mpr( "Cost level: %d  Skill points: %ld  Next cost level: %d",
              you.skill_cost_level, you.total_skill_points,
              skill_cost_needed( you.skill_cost_level + 1 ) );
        break;

    case '_':
        debug_get_religion();
        break;

    case '\'':
        for (i = 0; i < MAX_ITEMS; i++)
        {
            if (mitm[i].link == NON_ITEM)
                continue;

            mpr( "item:%3d link:%3d cl:%3d ty:%3d pl:%3d pl2:%3d sp:%3ld q:%3d",
                     i, mitm[i].link,
                     mitm[i].base_type, mitm[i].sub_type,
                     mitm[i].plus, mitm[i].plus2, mitm[i].special,
                     mitm[i].quantity );
        }

        mpr("igrid:");

        for (i = 0; i < GXM; i++)
        {
            for (j = 0; j < GYM; j++)
            {
                if (igrd[i][j] != NON_ITEM)
                {
                    mpr( "%3d at (%2d,%2d), cl:%3d ty:%3d pl:%3d pl2:%3d sp:%3ld q:%3d",
                             igrd[i][j], i, j,
                             mitm[i].base_type, mitm[i].sub_type,
                             mitm[i].plus, mitm[i].plus2, mitm[i].special,
                             mitm[i].quantity );
                }
            }
        }
        break;

    default:
        mpr("Not a Wizard Command.");
        break;
    }
}
#endif

/*
   This function handles the player's input. It's called from main(), from
   inside an endless loop.
 */
static void input(void)
{

    bool its_quiet;             //jmf: for silence messages
    FixedVector < int, 2 > plox;
    char move_x = 0;
    char move_y = 0;

    int keyin = 0;

#ifdef LINUX
    // Stuff for the Unix keypad kludge
    bool running = false;
    bool opening = false;
#endif

    you.stepping = false;

#ifdef LINUX
    update_screen();
#else
    window( 1, 1, 80, get_number_of_lines() );
#endif

    textcolor(LIGHTGREY);

    set_redraw_status( REDRAW_STATUS );
    print_stats();

    you.time_taken = player_base_time_taken();

#if WIZARD
    if (you.wizard && you.paralysis)
    {
        if (yesno( "End paralysis?" ))
            you.paralysis = 0;
    }
#endif

    if (you.paralysis)
    {
        keyin = '.';

        // Which channel to use here?  Note that paralysis == 1 will
        // result in the "able to move" message below.
        if (you.paralysis > 1)
            mpr( MSGCH_MULTITURN_ACTION, "You still cannot move!" );
    }
    else
    {
        handle_delay();

        gotoxy( 18, 9 );

        if (you_are_delayed())
            keyin = '.';
        else if (you.activity)
        {
            keyin = 128;
            perform_activity();
        }
        else
        {
            if (you.running < 0)        // Travel and explore
                travel(&keyin, &move_x, &move_y);
            else if (you.running)
            {
                keyin = 128;

                move_x = you.run_x;
                move_y = you.run_y;

                if (kbhit())
                {
                    stop_running();
                    goto gutch;
                }

                if (is_resting())
                {
                    you.running--;
                    keyin = '.';
                }
            }
            else if (!you.running)
            {

#if DEBUG_DIAGNOSTICS
                // save hunger at start of round
                // for use with hunger "delta-meter" in  output.cc
                you.old_hunger = you.hunger;
#endif

#if DEBUG_ITEM_SCAN
                debug_item_scan();
#endif

              gutch:
                flush_input_buffer( FLUSH_BEFORE_COMMAND );
                keyin = getch_with_command_macros();
            }

            mesclr();

            if (keyin == ':')
            {
                you.stepping = true;

                if (Options.prompt_move_modifiers)
                    mpr( MSGCH_PROMPT, "Step in which direction?" );

                keyin = getch();
            }

#ifdef LINUX
            // Kludging running and opening as two character sequences
            // for Unix systems.  This is an easy way out... all the
            // player has to do is find a termcap and numlock setting
            // that will get curses the numbers from the keypad.  This
            // will hopefully be easy.

            if (keyin == '*')
            {
                opening = true;

                if (Options.prompt_move_modifiers)
                    mpr( MSGCH_PROMPT, "Interact in which direction?" );

                keyin = getch();
            }
            else if (keyin == '/')
            {
                running = true;

                if (Options.prompt_move_modifiers)
                    mpr( MSGCH_PROMPT, "Run in which direction?" );

                keyin = getch();
            }

            // Translate keypad codes into command enums
            keyin = key_to_command( keyin );
#else
            // Old DOS keypad support
            if (keyin == 0)     // ALT also works - see ..\KEYTEST.CPP
            {
                keyin = getch();
                switch (keyin)
                {
                case 'O': move_x = -1; move_y =  1; break;
                case 'P': move_x =  0; move_y =  1; break;
                case 'I': move_x =  1; move_y = -1; break;
                case 'H': move_x =  0; move_y = -1; break;
                case 'G': move_x = -1; move_y = -1; break;
                case 'K': move_x = -1; move_y =  0; break;
                case 'Q': move_x =  1; move_y =  1; break;
                case 'M': move_x =  1; move_y =  0; break;

                case 119: open_door(-1, -1); move_x = 0; move_y = 0; break;
                case 141: open_door( 0, -1); move_x = 0; move_y = 0; break;
                case 132: open_door( 1, -1); move_x = 0; move_y = 0; break;
                case 116: open_door( 1,  0); move_x = 0; move_y = 0; break;
                case 118: open_door( 1,  1); move_x = 0; move_y = 0; break;
                case 145: open_door( 0,  1); move_x = 0; move_y = 0; break;
                case 117: open_door(-1,  1); move_x = 0; move_y = 0; break;
                case 115: open_door(-1,  0); move_x = 0; move_y = 0; break;

                case 76:
                case 'S':
                    keyin = '.';
                    goto get_keyin_again;
                }

                keyin = 128;
            }
#endif
        }
    }

    if (keyin != 128)
    {
        move_x = 0;
        move_y = 0;
        you.turn_is_over = false;
    }

#ifndef LINUX
  get_keyin_again:
#endif //jmf: just stops an annoying gcc warning

    switch (keyin)
    {
    case CONTROL('Y'):
    case CMD_OPEN_DOOR_UP_RIGHT:
        open_door(-1, -1); move_x = 0; move_y = 0; break;

    case CONTROL('K'):
    case CMD_OPEN_DOOR_UP:
        open_door( 0, -1); move_x = 0; move_y = 0; break;

    case CONTROL('U'):
    case CMD_OPEN_DOOR_UP_LEFT:
        open_door( 1, -1); move_x = 0; move_y = 0; break;

    case CONTROL('L'):
    case CMD_OPEN_DOOR_RIGHT:
        open_door( 1,  0); move_x = 0; move_y = 0; break;

    case CONTROL('N'):
    case CMD_OPEN_DOOR_DOWN_RIGHT:
        open_door( 1,  1); move_x = 0; move_y = 0; break;

    case CONTROL('J'):
    case CMD_OPEN_DOOR_DOWN:
        open_door( 0,  1); move_x = 0; move_y = 0; break;

    case CONTROL('B'):
    case CMD_OPEN_DOOR_DOWN_LEFT:
        open_door(-1,  1); move_x = 0; move_y = 0; break;

    case CONTROL('H'):
    case CMD_OPEN_DOOR_LEFT:
        open_door(-1,  0); move_x = 0; move_y = 0; break;

    case 'b': case CMD_MOVE_DOWN_LEFT:  move_x = -1; move_y =  1; break;
    case 'j': case CMD_MOVE_DOWN:       move_y =  1; move_x =  0; break;
    case 'u': case CMD_MOVE_UP_RIGHT:   move_x =  1; move_y = -1; break;
    case 'k': case CMD_MOVE_UP:         move_y = -1; move_x =  0; break;
    case 'y': case CMD_MOVE_UP_LEFT:    move_y = -1; move_x = -1; break;
    case 'h': case CMD_MOVE_LEFT:       move_x = -1; move_y =  0; break;
    case 'n': case CMD_MOVE_DOWN_RIGHT: move_y =  1; move_x =  1; break;
    case 'l': case CMD_MOVE_RIGHT:      move_x =  1; move_y =  0; break;

    case CMD_REST:
        // Yes this is backwards, but everyone here is used to using
        // straight 5s for long rests... might need to define a roguelike
        // rest key and get people switched over. -- bwr

#ifdef LINUX
        if (!running && !opening)
            start_running( RDIR_REST, RMODE_REST_DURATION );
        else
        {
            search_around();
            move_x = 0;
            move_y = 0;
            you.turn_is_over = true;
        }
#endif
        break;

    case 'B': case CMD_RUN_DOWN_LEFT:
        start_running( RDIR_DOWN_LEFT, RMODE_START ); break;

    case 'J': case CMD_RUN_DOWN:
        start_running( RDIR_DOWN, RMODE_START ); break;

    case 'U': case CMD_RUN_UP_RIGHT:
        start_running( RDIR_UP_RIGHT, RMODE_START ); break;

    case 'K': case CMD_RUN_UP:
        start_running( RDIR_UP, RMODE_START ); break;

    case 'Y': case CMD_RUN_UP_LEFT:
        start_running( RDIR_UP_LEFT, RMODE_START ); break;

    case 'H': case CMD_RUN_LEFT:
        start_running( RDIR_LEFT, RMODE_START ); break;

    case 'N': case CMD_RUN_DOWN_RIGHT:
        start_running( RDIR_DOWN_RIGHT, RMODE_START ); break;

    case 'L': case CMD_RUN_RIGHT:
        start_running( RDIR_RIGHT, RMODE_START ); break;

#ifdef LINUX
        // taken care of via key -> command mapping
#else
        // Old DOS keypad support
    case '1': start_running( RDIR_DOWN_LEFT, RMODE_START ); break;
    case '2': start_running( RDIR_DOWN, RMODE_START ); break;
    case '9': start_running( RDIR_UP_RIGHT, RMODE_START ); break;
    case '8': start_running( RDIR_UP, RMODE_START ); break;
    case '7': start_running( RDIR_UP_LEFT, RMODE_START ); break;
    case '4': start_running( RDIR_LEFT, RMODE_START ); break;
    case '3': start_running( RDIR_DOWN_RIGHT, RMODE_START ); break;
    case '6': start_running( RDIR_RIGHT, RMODE_START ); break;
    case '5': start_running( RDIR_REST, RMODE_REST_DURATION ); break;

#endif

    case CONTROL('A'):
    case CMD_TOGGLE_AUTOPICKUP:
        you.autopickup_on = !you.autopickup_on;
        mpr( "Autopickup is now %s.", (you.autopickup_on) ? "on" : "off" );
        break;

    case CONTROL('C'):
    case CMD_CLEAN_MAP:
        {
            mpr( "Cleaning map." );
            for (int x = X_BOUND_1; x <= X_BOUND_2; x++)
            {
                for (int y = Y_BOUND_1; y <= Y_BOUND_2; y++)
                {
                    // Don't expose new dug out areas:
                    // Note: assumptions are being made here about how
                    // terrain can change (eg it used to be solid, and
                    // thus monster/item free).
                    if (is_terrain_changed(x, y))
                        continue;

                    if (is_terrain_seen( x, y ))
                        set_envmap_char( x, y, get_sightmap_char(grd[x][y]) );
                    else if (is_terrain_known( x, y ))
                        set_envmap_char( x, y, get_magicmap_char(grd[x][y]) );
                    else
                        set_envmap_char( x, y, 0 );
                }
            }
        }
        break;

    case '<':
    case CMD_GO_UPSTAIRS:
        if (grd[you.x_pos][you.y_pos] == DNGN_ENTER_SHOP)
        {
            shop();
            break;
        }
        else if ((grd[you.x_pos][you.y_pos] < DNGN_STONE_STAIRS_UP_I
                    || grd[you.x_pos][you.y_pos] > DNGN_ROCK_STAIRS_UP)
                && (grd[you.x_pos][you.y_pos] < DNGN_RETURN_FROM_ORCISH_MINES
                    || grd[you.x_pos][you.y_pos] >= 150)
                && grd[you.x_pos][you.y_pos] != DNGN_EXIT_HELL)
        {
            mpr( "You can't go up here!" );
            break;
        }

        tag_followers();  // only those beside us right now can follow
        start_delay( DELAY_ASCENDING_STAIRS,
                     1 + (you.burden_state > BS_UNENCUMBERED) );

        // stairs are harder for the big:
        if (player_size( PSIZE_BODY ) > SIZE_LARGE)
            you.time_taken = (you.time_taken * 3) / 2;

        you.turn_is_over = true;
        break;

    case '>':
    case CMD_GO_DOWNSTAIRS:
        if ((grd[you.x_pos][you.y_pos] < DNGN_ENTER_LABYRINTH
                || grd[you.x_pos][you.y_pos] > DNGN_ROCK_STAIRS_DOWN)
            && grd[you.x_pos][you.y_pos] != DNGN_ENTER_HELL
            && ((grd[you.x_pos][you.y_pos] < DNGN_ENTER_DIS
                    || grd[you.x_pos][you.y_pos] > DNGN_TRANSIT_PANDEMONIUM)
                && grd[you.x_pos][you.y_pos] != DNGN_STONE_ARCH)
            && !(grd[you.x_pos][you.y_pos] >= DNGN_ENTER_ORCISH_MINES
                && grd[you.x_pos][you.y_pos] < DNGN_RETURN_FROM_ORCISH_MINES))
        {
            mpr( "You can't go down here!" );
            break;
        }

        tag_followers();  // only those beside us right now can follow
        start_delay( DELAY_DESCENDING_STAIRS,
                     1 + (you.burden_state > BS_UNENCUMBERED),
                     you.depth );

        // stairs are harder for the big:
        if (player_size( PSIZE_BODY ) > SIZE_LARGE)
            you.time_taken = (you.time_taken * 3) / 2;

        you.turn_is_over = true;
        break;

    case 'O':
    case CMD_DISPLAY_OVERMAP:
        display_overmap();
        break;

    case 'o':
    case CMD_OPEN_DOOR:
        open_door(0, 0);
        break;
    case 'c':
    case CMD_CLOSE_DOOR:
        close_door(0, 0);
        break;

    case 'd':
    case CMD_DROP:
        do_drop();
#ifdef STASH_TRACKING
        if (Options.stash_tracking >= STM_DROPPED)
            stashes.add_stash();
#endif
        break;

#ifdef STASH_TRACKING
    case CONTROL('S'):
    case CMD_MARK_STASH:
        if (Options.stash_tracking >= STM_EXPLICIT)
            stashes.add_stash();
        break;

    case CONTROL('E'):
    case CMD_FORGET_STASH:
        if (Options.stash_tracking >= STM_EXPLICIT)
            stashes.no_stash();
        break;
#endif

    case 'D':
    case CMD_BUTCHER:
        butchery();
        break;

    case 'i':
    case CMD_DISPLAY_INVENTORY:
        show_invent(-1);
        redraw_screen();
        break;

    case 'I':
    case CMD_OBSOLETE_INVOKE:
        // We'll leave this message in for a while.  Eventually, this
        // might be some special for of inventory command, or perhaps
        // actual god invocations will be split to here from abilities. -- bwr
        // mpr( MSGCH_WARN, "This command is now 'E'voke wielded item." );
        break;

    case 'E':
    case CMD_EVOKE:
        if (!evoke_wielded())
            flush_input_buffer( FLUSH_ON_FAILURE );
        break;

    case 'g':
    case ',':
    case CMD_PICKUP:
        do_pickup();
        break;

    case ';':
    case CMD_INSPECT_FLOOR:
        item_check( true );
        break;

    case 'w':
    case CMD_WIELD_WEAPON:
        wield_weapon(false);
        break;

    case 't':
    case CMD_THROW:
        throw_anything();
        break;

    case 'f':
    case CMD_FIRE:
        shoot_thing();
        break;

    case 'W':
    case CMD_WEAR_ARMOUR:
        wear_armour();
        break;

    case 'T':
    case CMD_REMOVE_ARMOUR:
        {
            int index=0;

            if (armour_prompt("Take off which item?", &index))
                takeoff_armour(index);
        }
        break;

    case 'R':
    case CMD_REMOVE_JEWELLERY:
        remove_ring();
        break;
    case 'P':
    case CMD_WEAR_JEWELLERY:
        puton_ring();
        break;

    case '=':
    case CMD_ADJUST_INVENTORY:
        adjust();
        return;

    case 'M':
    case CMD_MEMORISE_SPELL:
        if (!learn_spell())
            flush_input_buffer( FLUSH_ON_FAILURE );
        break;

    case 'z':
    case CMD_ZAP_WAND:
        zap_wand();
        break;

    case 'e':
    case CMD_EAT:
        eat_food();
        break;

    case 'a':
    case CMD_USE_ABILITY:
        if (!activate_ability())
            flush_input_buffer( FLUSH_ON_FAILURE );
        break;

    case 'A':
    case CMD_DISPLAY_MUTATIONS:
        display_mutations();
        redraw_screen();
        break;

    case 'v':
    case CMD_EXAMINE_OBJECT:
        original_name();
        break;

    case 'p':
    case CMD_PRAY:
        pray();
        break;

    case '^':
    case CMD_DISPLAY_RELIGION:
        describe_god( you.religion, true );
        redraw_screen();
        break;

    case '.':
    case CMD_MOVE_NOWHERE:
        search_around();
        move_x = 0;
        move_y = 0;
        you.turn_is_over = true;
        break;

    case 'q':
    case CMD_QUAFF:
        drink();
        break;

    case 'r':
    case CMD_READ:
        read_scroll();
        break;

    case 'x':
    case CMD_LOOK_AROUND:
        mpr(MSGCH_PROMPT, "Move the cursor around to observe a square." );
        mpr(MSGCH_PROMPT, "Press '?' for a monster description." );

        struct dist lmove;
        lmove.isValid = lmove.isTarget = lmove.isCancel = false;
        look_around( lmove, true );
        if (lmove.isValid && lmove.isTarget && !lmove.isCancel)
            start_travel( lmove.tx, lmove.ty );
        break;

    case 's':
    case CMD_SEARCH:
        search_around();
        you.turn_is_over = true;
        break;

    case 'Z':
    case CMD_CAST_SPELL:
        /* randart wpns */
        if (scan_randarts(RAP_PREVENT_SPELLCASTING))
        {
            mpr("Something interferes with your magic!");
            flush_input_buffer( FLUSH_ON_FAILURE );
            break;
        }

        if (!cast_a_spell())
            flush_input_buffer( FLUSH_ON_FAILURE );
        break;

    case '\'':
    case CMD_WEAPON_SWAP:
        wield_weapon(true);
        break;

    case CMD_FIX_WAYPOINT:
    case CONTROL('F'):
        if (you.level_type == LEVEL_LABYRINTH || you.level_type == LEVEL_ABYSS
                || you.level_type == LEVEL_PANDEMONIUM)
        {
            mpr("Sorry, you can't set a waypoint here.");
            break;
        }
        travel_cache.add_waypoint();
        break;

    case CMD_INTERLEVEL_TRAVEL:
    case CONTROL('G'):
        if (you.level_type == LEVEL_LABYRINTH || you.level_type == LEVEL_ABYSS
                || you.level_type == LEVEL_PANDEMONIUM)
        {
            mpr("Sorry, you can't auto-travel out of here.");
            break;
        }
        start_translevel_travel();
        redraw_screen();
        break;

    case CONTROL('O'):
    case CMD_EXPLORE:
        if (you.level_type == LEVEL_LABYRINTH || you.level_type == LEVEL_ABYSS)
        {
            mpr("It would help if you knew where you were, first.");
            break;
        }
        // Start exploring
        start_explore();
        break;

    case 'X':
    case CMD_DISPLAY_MAP:
    {
#if (!DEBUG_DIAGNOSTICS)
        if (you.level_type == LEVEL_LABYRINTH || you.level_type == LEVEL_ABYSS)
        {
            mpr("It would help if you knew where you were, first.");
            break;
        }
#endif
        bool smret = show_map( plox, true );
        redraw_screen();
        if (smret && plox[0] > 0)
            start_travel(plox[0], plox[1]);
        break;
    }
    case '\\':
    case CMD_DISPLAY_KNOWN_OBJECTS:
        check_item_knowledge(); //nothing = check_item_knowledge();
        redraw_screen();
        break;

    case CONTROL('P'):
    case CMD_REPLAY_MESSAGES:
        replay_messages();
        redraw_screen();
        break;

    case CONTROL('R'):
    case CMD_REDRAW_SCREEN:
        redraw_screen();
        break;

    case CONTROL('X'):
    case CMD_SAVE_GAME_NOW:
        mpr("Saving game... please wait.");
        save_game(true);
        break;

#ifdef USE_UNIX_SIGNALS
    case CONTROL('Z'):
    case CMD_SUSPEND_GAME:
        // CTRL-Z suspend behaviour is implemented here,
        // because we want to have CTRL-Y available,
        // and they tend to be stuck together.
        clrscr();
        lincurses_shutdown();
        kill(0, SIGTSTP);
        lincurses_startup();
        redraw_screen();
        break;
#endif

    case '?':
    case CMD_DISPLAY_COMMANDS:
        list_commands(false);
        redraw_screen();
        break;

    case 'C':
    case CMD_EXPERIENCE_CHECK:
        mpr( "You are a level %d %s %s.",
              you.xp_level, species_name( you.species ), you.class_name );

        if (you.xp_level < 27)
        {
            int xp_needed = (exp_needed(you.xp_level+2) - you.xp) + 1;
            mpr("Level %d requires %ld experience (%d point%s to go!)",
                    you.xp_level + 1,
                    exp_needed(you.xp_level + 2) + 1,
                    xp_needed,
                    (xp_needed > 1) ? "s" : "" );
        }
        else
        {
            mpr( "I'm sorry, level 27 is as high as you can go." );

            // XXX; add some more special level 27 messages?
            mpr( "With the way you've been playing, I'm surprised you got this far." );
        }

        if (you.real_time != -1)
        {
            const time_t curr = you.real_time + (time(NULL) - you.start_time);
            char buff[200];

            make_time_string( curr, buff, sizeof(buff) );

            mpr( "Play time: %s (%ld turns)", buff, you.num_turns );
        }

#if DEBUG_DIAGNOSTICS
        // too annoying for regular diagnostics

        mpr( MSGCH_DIAGNOSTICS, "Stealth: %d; MR: %d; Elapsed Time: %.0f",
              you.stealth_cache, player_res_magic(), you.elapsed_time );
#endif
        break;


    case '!':
    case CMD_SHOUT:
        yell();                 /* in effects.cc */
        break;

    case '@':
    case CMD_DISPLAY_CHARACTER_STATUS:
        display_char_status();
        break;

    case 'm':
    case CMD_DISPLAY_SKILLS:
        show_skills();
        redraw_screen();
        break;

    case '#':
    case CMD_CHARACTER_DUMP:
        char name_your[kNameLen+1];

        strncpy(name_your, you.your_name, kNameLen);
        name_your[kNameLen] = '\0';
        if (dump_char( name_your, false ))
            mpr("Char dumped successfully.");
        else
            mpr("Char dump unsuccessful! Sorry about that.");
        break;

#ifdef USE_MACROS
    case '`':
    case CMD_MACRO_ADD:
        macro_add_query();
        break;
    case '~':
    case CMD_MACRO_SAVE:
        mpr("Saving macros.");
        macro_save();
        break;
#endif

    case ')':
    case CMD_LIST_WEAPONS:
        list_weapons();
        return;

    case ']':
    case CMD_LIST_ARMOUR:
        list_armour();
        return;

    case '"':
    case CMD_LIST_JEWELLERY:
        list_jewellery();
        return;

#ifdef WIZARD
    case '&':
    case CMD_WIZARD:
        handle_wizard_command();
        break;
#endif

    case 'S':
    case CMD_SAVE_GAME:
        if (yesno("Save game and exit?", false))
            save_game(true);
        break;

    case 'Q':
    case CMD_QUIT:
        quit_game();
        break;

    case CONTROL('V'):
    case CMD_GET_VERSION:
        version();
        break;

    case 128:   // Can't use this char -- it's the special value 128
        break;

    default:
    case CMD_NO_CMD:
        mpr("Unknown command.");
        break;
    }

#ifdef STASH_TRACKING
    if (Options.stash_tracking)
    {
        stashes.update_visible_stashes( (Options.stash_tracking == STM_ALL)
                                            ? StashTracker::ST_AGGRESSIVE
                                            : StashTracker::ST_PASSIVE );
    }
#endif

#ifdef LINUX
    // New Unix keypad stuff
    if (running)
    {
        const run_dir_type  dir = convert_to_compass( move_x, move_y );

        if (dir != RDIR_NONDIR)
        {
            start_running( dir, RMODE_START );
            move_x = 0;
            move_y = 0;
        }
    }
    else if (opening)
    {
        open_door( move_x, move_y );
        move_x = 0;
        move_y = 0;
    }
#endif

    if (move_x || move_y)
        move_player( move_x, move_y );
    else if (you.turn_is_over)      // we did something other than move/attack
        do_berserk_no_combat_penalty();

    if (!you.turn_is_over)              // did a no time cost command
    {
        viewwindow( true, false );
        return;
    }

    ASSERT( you.time_taken >= 0 );

    if (you.num_turns != -1)
        you.num_turns++;

    you.time_taken = adjust_time_for_speed( you.time_taken,
                                            player_net_speed_effect() );

    you.shield_blocks = 0;                  // no blocks yet this round
    you.stealth_cache = player_stealth();

    handle_player_enchantments();

    viewwindow( true, true );

    handle_monsters();

    // make sure we don't overflow
    ASSERT( DBL_MAX - you.elapsed_time > you.time_taken );

    you.elapsed_time += you.time_taken;

    // handle the long effect "synch" time counter
    if (you.synch_time > you.time_taken)
        you.synch_time -= you.time_taken;
    else
    {
        handle_time( static_cast<unsigned int>(200 + (you.time_taken - you.synch_time)) );
        you.synch_time = 200;
    }

    manage_clouds();

    if (you.fire_shield > 0)
        manage_fire_shield();

    // There used to be signs of intent to have statues as some sort
    // of more complex state machine... I'm boiling them down to bare
    // basics for now.  -- bwr
    if (you.visible_statue[ STATUE_SILVER ])
    {
        interrupt_activity( AI_STATUE );

        if ((!you.invis && one_chance_in(3)) || one_chance_in(5))
        {
            char wc[30];

            weird_colours( random2(256), wc );
            mpr( MSGCH_WARN, "The silver statue's eyes glow %s.", wc );

            const int demon = rand_demon((coinflip() ? DEMON_COMMON

                                                           : DEMON_LESSER));
            create_monster( demon, BEH_HOSTILE, 5 );
        }

        you.visible_statue[ STATUE_SILVER ] = 0;
    }

    if (you.visible_statue[ STATUE_ORANGE_CRYSTAL ])
    {
        interrupt_activity( AI_STATUE );

        if ((!you.invis && coinflip()) || one_chance_in(4))
        {
            mpr( MSGCH_WARN,"A hostile presence attacks your mind!" );

            miscast_effect( SPTYP_DIVINATION, random2(15), random2(150), 100,
                            "an orange crystal statue" );
        }

        you.visible_statue[ STATUE_ORANGE_CRYSTAL ] = 0;
    }

    // food death check:
    if (you.is_undead != US_UNDEAD && you.hunger <= HUNGER_PARALYSIS_LEVEL)
    {
        if (!you.paralysis && one_chance_in(40))
        {
            mpr(MSGCH_FOOD,"You lose consciousness!" );
            paralyse_player( 5 + random2(8), true );
        }

        if (you.hunger <= HUNGER_STARVATION_DEATH)
        {
            mpr( MSGCH_FOOD, "You have starved to death." );
            ouch( INSTANT_DEATH, 0, KILLED_BY_STARVATION );
        }
    }

    //jmf: added silence messages
    its_quiet = silenced( you.x_pos, you.y_pos );

    if (you.attribute[ATTR_WAS_SILENCED] != its_quiet)
    {
        if (its_quiet)
        {
            if (random2(30))
                mpr(MSGCH_WARN,"You are enveloped in profound silence." );
            else
                mpr(MSGCH_WARN,"The dungeon seems quiet ... too quiet!" );
        }
        else
        {
            mpr(MSGCH_RECOVERY,"Your hearing returns." );
        }

        you.attribute[ATTR_WAS_SILENCED] = its_quiet;
    }

    viewwindow( true, false );

    // if (you.paralysis && any_messages())
    //     more();

    // place normal dungeon monsters,  but not in player LOS
    if (you.level_type == LEVEL_DUNGEON
        && !player_in_branch( BRANCH_ECUMENICAL_TEMPLE )
        && one_chance_in((you.char_direction == DIR_DESCENDING) ? 240 : 10))
    {
        int prox = (one_chance_in(10) ? PROX_NEAR_STAIRS
                                      : PROX_AWAY_FROM_PLAYER);

        // The rules change once the player has picked up the Orb...
        if (you.char_direction == DIR_ASCENDING)
            prox = (one_chance_in(10) ? PROX_CLOSE_TO_PLAYER : PROX_ANYWHERE);

        int succ = 0;

        if (one_chance_in(40))
            succ = place_unique_monsters( true, prox );

        if (!succ)
        {
            mons_place( WANDERING_MONSTER, BEH_HOSTILE, MHITNOT, false,
                        50, 50, LEVEL_DUNGEON, prox );
        }
    }

    // place Abyss monsters.
    if (you.level_type == LEVEL_ABYSS && one_chance_in(5))
    {
        mons_place( WANDERING_MONSTER, BEH_HOSTILE, MHITNOT, false,
                    50, 50, LEVEL_ABYSS, PROX_ANYWHERE );
    }

    // place Pandemonium monsters
    if (you.level_type == LEVEL_PANDEMONIUM && one_chance_in(50))
        pandemonium_mons();

    // No monsters in the Labyrinth,  or Ecumenical Temple
    return;
}

// Currently assumes the player is toggling the door by hand in the messages.
static void toggle_door( int x, int y )
{
    ASSERT( grd[x][y] == DNGN_OPEN_DOOR || grd[x][y] == DNGN_CLOSED_DOOR );

    const char *const verb = (grd[x][y] == DNGN_OPEN_DOOR) ? "close" : "open";

    // const int skill = you.dex * (1 + (2 * you.skills[SK_TRAPS_DOORS] + you.skills[SK_STEALTH] + 1) / 2);
    const int skill = (you.dex * (1 + skill_bump(SK_TRAPS_DOORS) + skill_bump(SK_STEALTH))) / 2;

#if DEBUG_DIAGNOSTICS
    mpr( MSGCH_DIAGNOSTICS, "Door skill (rolled against 5): %d", skill );
#endif

    if (random2( skill ) < 5)
    {
        snprintf( info, INFO_SIZE, "As you %s the door, it creaks loudly!", verb );
        noisy( SL_CREAK, x, y, info );
    }
    else
    {
        mpr( "You %s%s the door.",
              player_is_levitating() ? "reach down and " : "", verb );
    }

    grd[x][y] = (grd[x][y] == DNGN_OPEN_DOOR) ? DNGN_CLOSED_DOOR
                                              : DNGN_OPEN_DOOR;
}

/*
   Opens doors and handles some aspects of untrapping. If either move_x or
   move_y are non-zero,  the pair carries a specific direction for the door
   to be opened (eg if you type ctrl - dir).
 */
static void open_door( const int move_x, const int move_y )
{
    struct dist door_move;
    int dx, dy;             // door x, door y

    door_move.dx = move_x;
    door_move.dy = move_y;

    if (move_x || move_y)
    {
        dx = you.x_pos + move_x;
        dy = you.y_pos + move_y;
        const int mon = mgrd[dx][dy];

        // used for reaching checks
        const int rx = dx + move_x;
        const int ry = dy + move_y;
        const int rmon = mgrd[rx][ry];

        if (Options.easy_reach
            && player_sound_mind_and_body()
            && player_damage_brand() == SPWPN_REACHING
            && (rmon != NON_MONSTER && player_monster_visible( &menv[rmon] ))
            && (mon == NON_MONSTER || !player_monster_visible( &menv[mon] )))
        {
            reaching_weapon_attack( rx, ry );
            you.turn_is_over = true;
            return;
        }
        else if (mon != NON_MONSTER && !mons_is_submerged( &menv[mon] ))
        {
            you_attack( mon, true );
            you.turn_is_over = true;

            if (you.berserk_penalty != NO_BERSERK_PENALTY)
                you.berserk_penalty = 0;

            return;
        }

        if (grd[dx][dy] >= DNGN_TRAP_MECHANICAL && grd[dx][dy] <= DNGN_TRAP_III)
        {
            if (is_cloud( dx, dy ))
            {
                mpr("You can't get to that trap right now.");
                return;
            }

            disarm_trap( door_move );
            return;
        }
    }
    else
    {
        mpr(MSGCH_PROMPT,"Which direction?" );
        direction( door_move, DIR_DIR );
        if (!door_move.isValid)
            return;

        // convenience
        dx = you.x_pos + door_move.dx;
        dy = you.y_pos + door_move.dy;
    }

    if (grd[dx][dy] == DNGN_CLOSED_DOOR)
    {
        toggle_door( dx, dy );
        you.turn_is_over = true;
    }
    else
    {
        mpr("You swing at nothing.");

        // Food cost since we have to assume the player is trying to swat
        // an invisible monster without moving (we can't know the player's
        // motives or assumptions).  This does have an added benefit of
        // providing the player with a way to increase hunger if they
        // want to eat something (although it does mean that one should
        // us '.' while waiting for an oppenent to close. -- bwr
        make_hungry( 3, true );
        you.turn_is_over = true;
    }
}                               // end open_door()

/*
   Similar to open_door. Can you spot the difference?
 */
static void close_door(char door_x, char door_y)
{
    struct dist door_move;
    int dx, dy;             // door x, door y

    door_move.dx = door_x;
    door_move.dy = door_y;

    if (!(door_x || door_y))
    {
        mpr(MSGCH_PROMPT,"Which direction?" );
        direction( door_move, DIR_DIR );
        if (!door_move.isValid)
            return;
    }

    if (door_move.dx == 0 && door_move.dy == 0)
    {
        mpr("You can't close doors on yourself!");
        return;
    }

    // convenience
    dx = you.x_pos + door_move.dx;
    dy = you.y_pos + door_move.dy;

    if (grd[dx][dy] == DNGN_OPEN_DOOR)
    {
        if (mgrd[dx][dy] != NON_MONSTER)
        {
            // Need to make sure that turn_is_over if creature is invisible
            mpr("There's a creature in the doorway!");
            door_move.dx = 0;
            door_move.dy = 0;
            return;
        }

        if (igrd[dx][dy] != NON_ITEM)
        {
            mpr("There's something blocking the doorway.");
            door_move.dx = 0;
            door_move.dy = 0;
            return;
        }

        toggle_door( dx, dy );
        you.turn_is_over = true;
    }
    else
    {
        mpr("There isn't anything that you can close there!");
    }
}                               // end close_door()


// initialise whole lot of stuff...
// returns true if a new character
static bool initialise(void)
{
    bool ret;
    int  i = 0, j = 0;

#ifdef DOS
    directvideo = 1;
#endif

#ifdef LINUX
    lincurses_startup();
#endif

#ifdef MAC
    init_mac();
#endif

#ifdef WIN32CONSOLE
    init_libw32c();
#endif

#ifdef USE_EMX
    init_emx();
#endif

#ifdef USE_MACROS
    // Load macros
    macro_init();
#endif

    seed_rng();                 // init and seed RNGs

    init_overmap();             // in overmap.cc (duh?)
    clear_ids();                // in itemname.cc
    init_feature_table();       // must be after char set and table are done

    init_properties();          // this needs to be way up top too -- bwr
    init_monsters();            // this needs to be way up top {dlb}
    init_playerspells();        // this needs to be way up top {dlb}
    init_mutations();           // this needs to be way up top too -- bwr

    clrscr();

    // init item array:
    for (i = 1; i < MAX_ITEMS; i++)
        init_item( mitm[i] );

    info[0] = '\0';             // empty messaging string
    clear_envmap();

    for (i = 0; i < GXM; i++)
    {
        for (j = 0; j < GYM; j++)
        {
            igrd[i][j] = NON_ITEM;
            mgrd[i][j] = NON_MONSTER;
        }
    }

    for (i = 0; i < MAX_UNIQ_MONS; i++)
        you.unique_creatures[i] = 0;

    for (i = 0; i < MAX_UNIQ_ITEMS; i++)
        you.unique_items[i] = UNIQ_NOT_EXISTS;

    for (i = 0; i < NUM_STATUE_TYPES; i++)
        you.visible_statue[i] = 0;

    you.symbol = '@';
    you.colour = LIGHTGREY;
    you.autopickup_on = true;

    textbackground( BLACK );

    // initialize tag system before we try loading anything!
    tag_init();

    // sets up a new game:
    bool newc = new_game();
    ret = newc;  // newc will be mangled later so we'll take a copy --bwr

    if (!newc)
        restore_game();

    Game_Has_Started = true;

    calc_hp_max();
    calc_mp_max();

    load( DNGN_STONE_STAIRS_DOWN_I, (newc ? LOAD_START_GAME
                                          : LOAD_RESTART_GAME),
          false, 0, you.where_are_you );

#if DEBUG_DIAGNOSTICS
    // Debug compiles display a lot of "hidden" information, so we auto-wiz
    you.wizard = true;
#endif

    burden_change();
    make_hungry( 0, true );

    set_redraw_status( REDRAW_STRENGTH );
    set_redraw_status( REDRAW_INTELLIGENCE );
    set_redraw_status( REDRAW_DEXTERITY );
    set_redraw_status( REDRAW_ARMOUR_CLASS );
    set_redraw_status( REDRAW_EVASION );
    set_redraw_status( REDRAW_EXPERIENCE );
    set_redraw_status( REDRAW_GOLD );
    set_redraw_status( REDRAW_WIELD );

    you.start_time = time( NULL );      // start timer on session

    draw_border();
    enter_new_level();

    // set vision radius to player's current vision
    setLOSRadius( you.current_vision );
    viewwindow( true, false ); // This just puts the view up for the first turn.
    item_grid();

    return (ret);
}

// An attempt to tone down berserk a little bit. -- bwross
//
// This function does the accounting for not attacking while berserk
// This gives a triangluar number function for the additional penalty
// Turn:    1  2  3   4   5   6   7   8
// Penalty: 1  3  6  10  15  21  28  36
//
// Total penalty (including the standard one during upkeep is:
//          2  5  9  14  20  27  35  44
//
static void do_berserk_no_combat_penalty(void)
{
    // Butchering/eating a corpse will maintain a blood rage.
    const int delay = current_delay_action();
    if (delay == DELAY_BUTCHER || delay == DELAY_EAT)
        return;

    if (you.berserk_penalty == NO_BERSERK_PENALTY)
        return;

    if (you.berserker)
    {
        you.berserk_penalty++;

        switch (you.berserk_penalty)
        {
        case 2:
            mpr(MSGCH_DURATION,"You feel a strong urge to attack something." );
            break;
        case 4:
            mpr(MSGCH_DURATION,"You feel your anger subside." );
            break;
        case 6:
            mpr(MSGCH_DURATION,"Your blood rage is quickly leaving you." );
            break;
        }

        dec_berserk( you.berserk_penalty );
    }
    return;
}                               // end do_berserk_no_combat_penalty()


// Called when the player moves by walking/running. Also calls
// attack function and trap function etc when necessary.
static void move_player( char move_x, char move_y )
{
    bool attacking = false;
    bool moving = true;         // used to prevent eventual movement (swap)
    bool swap = false;

    if (you.conf)
    {
        if (!one_chance_in(3))
        {
            move_x = random2(3) - 1;
            move_y = random2(3) - 1;
        }

        if (!in_bounds( you.x_pos + move_x, you.y_pos + move_y )
            || grid_is_solid( grd[you.x_pos + move_x][you.y_pos + move_y] ))
        {
            you.turn_is_over = true;
            mpr("Ouch!");
            return;
        }
    }                           // end of if you.conf

    // We only check for stopping if we're not just starting the run...
    // this guarantees us that the run command generates at least one
    // square of movement.
    if (you.running == RMODE_CONTINUE && check_stop_running())
    {
        stop_running();
        move_x = 0;
        move_y = 0;
        you.turn_is_over = false;
        return;
    }

    const int targ_x = you.x_pos + move_x;
    const int targ_y = you.y_pos + move_y;

    const unsigned char targ_grid  =  grd[ targ_x ][ targ_y ];
    const unsigned char targ_monst = mgrd[ targ_x ][ targ_y ];
    const bool          targ_solid = grid_is_solid( targ_grid );

    if (targ_monst != NON_MONSTER && !mons_is_submerged( &menv[targ_monst] ))
    {
        struct monsters *mon = &menv[targ_monst];

        if (mons_friendly( mon )
            && ((player_monster_visible( mon )
                    && player_sound_mind_and_body())
                || (mons_player_visible( mon )
                    && mons_sound_mind_and_body( mon ))))
        {
            if (swap_places( mon ))
                swap = true;
            else
                moving = false;
        }
        else // attack!
        {
            you_attack( targ_monst, true );
            you.turn_is_over = true;

            // we don't want to create a penalty if there isn't
            // supposed to be one
            if (you.berserk_penalty != NO_BERSERK_PENALTY)
                you.berserk_penalty = 0;

            attacking = true;
        }
    }

    if (!attacking && !targ_solid && moving)
    {
        you.time_taken *= player_movement_speed();
        you.time_taken /= 10;

        move_player_to_grid( you.x_pos + move_x, you.y_pos + move_y,
                             true, false, swap );

        // Returning the random trap scans as a way to get more use from the
        // skill and acute mutations.
        if (you.mutation[MUT_ACUTE_VISION] >= 2
            || (!you.mutation[MUT_BLURRY_VISION]
                && random2(100) < stat_mult(you.intel, skill_bump(SK_TRAPS_DOORS))))
        {
            search_around();
        }

        move_x = 0;
        move_y = 0;

        you.turn_is_over = true;
        item_check( false );
    }

    if (targ_grid == DNGN_CLOSED_DOOR && Options.easy_open)
        open_door( move_x, move_y );
    else if (targ_solid)
    {
        stop_running();
        move_x = 0;
        move_y = 0;
        you.turn_is_over = false;
    }

    if (you.running == RMODE_START)
        you.running = RMODE_CONTINUE;

    // shift abyss when we get far enough off center
    if (you.level_type == LEVEL_ABYSS
        && (you.x_pos < X_ABYSS_1 || you.x_pos > X_ABYSS_2
            || you.y_pos < Y_ABYSS_1 || you.y_pos > Y_ABYSS_2))
    {
        area_shift();
        you.pet_target = MHITNOT;

#if DEBUG_DIAGNOSTICS
        int igly = 0;
        int ig2 = 0;

        for (igly = 0; igly < MAX_ITEMS; igly++)
        {
            if (is_valid_item( mitm[igly] ))
                ig2++;
        }

        mpr( MSGCH_DIAGNOSTICS, "Number of items present: %d", ig2 );

        ig2 = 0;
        for (igly = 0; igly < MAX_MONSTERS; igly++)
        {
            if (menv[igly].type != -1)
                ig2++;
        }

        mpr( MSGCH_DIAGNOSTICS, "Number of monsters present: %d", ig2 );
        mpr( MSGCH_DIAGNOSTICS, "Number of clouds present: %d", env.cloud_no );
#endif
    }

    if (!attacking)
        do_berserk_no_combat_penalty();

    return;
}                               // end move_player()

static void handle_player_enchantments( void )
{
    if (you.special_wield != SPWLD_NONE)
        special_wielded();

    const int jumpiness = player_teleport(); // teleportitis

    if (jumpiness && random2(1000) < jumpiness)
        you_teleport2( true );
    else if (you.level_type == LEVEL_ABYSS && one_chance_in(300))
        you_teleport2( false, true ); // to new area of the Abyss

    if (is_cloud( you.x_pos, you.y_pos ))
        in_a_cloud();

    if (you.duration[DUR_REPEL_UNDEAD] > 1)
        you.duration[DUR_REPEL_UNDEAD]--;

    if (you.duration[DUR_REPEL_UNDEAD] == 4)
    {
        mpr( MSGCH_DURATION, "Your holy aura is starting to fade." );
        you.duration[DUR_REPEL_UNDEAD] -= random2(3);
    }
    else if (you.duration[DUR_REPEL_UNDEAD] == 1)
    {
        mpr( MSGCH_DURATION, "Your holy aura fades away." );
        you.duration[DUR_REPEL_UNDEAD] = 0;
    }

    // paradox: it both lasts longer & does more damage overall if you're
    //          moving slower.
    // rationalisation: I guess it gets rubbed off/falls off/etc if you
    //          move around more.
    if (you.duration[DUR_LIQUID_FLAMES] > 0)
        you.duration[DUR_LIQUID_FLAMES]--;

    if (you.duration[DUR_LIQUID_FLAMES] != 0)
    {
        mpr( MSGCH_WARN, "You are covered in liquid flames!" );
        expose_player_to_element( BEAM_NAPALM, 12 );

        int dam = roll_dice(2,5) * you.time_taken;

        dam = (check_your_resists( dam, BEAM_NAPALM ) + 5) / 10;
        ouch( dam, 0, KILLED_BY_BURNING );
    }

    if (you.duration[DUR_ICY_ARMOUR] > 1)
    {
        you.duration[DUR_ICY_ARMOUR]--;
        //expose_player_to_element( BEAM_COLD, 6 );
    }
    else if (you.duration[DUR_ICY_ARMOUR] == 1)
    {
        mpr(MSGCH_DURATION,"Your icy armour evaporates." );
        set_redraw_status( REDRAW_ARMOUR_CLASS );     // is this needed? 2apr2000 {dlb}
        you.duration[DUR_ICY_ARMOUR] = 0;
    }

    if (you.duration[DUR_REPEL_MISSILES] > 1)
    {
        you.duration[DUR_REPEL_MISSILES]--;
        if (you.duration[DUR_REPEL_MISSILES] == 6)
        {
            mpr(MSGCH_DURATION,"Your repel missiles spell is about to expire..." );
            if (coinflip())
                you.duration[DUR_REPEL_MISSILES]--;
        }
    }
    else if (you.duration[DUR_REPEL_MISSILES] == 1)
    {
        mpr(MSGCH_DURATION,"You feel less protected from missiles." );
        you.duration[DUR_REPEL_MISSILES] = 0;
    }

    if (you.duration[DUR_DEFLECT_MISSILES] > 1)
    {
        you.duration[DUR_DEFLECT_MISSILES]--;
        if (you.duration[DUR_DEFLECT_MISSILES] == 6)
        {
            mpr(MSGCH_DURATION,"Your deflect missiles spell is about to expire..." );
            if (coinflip())
                you.duration[DUR_DEFLECT_MISSILES]--;
        }
    }
    else if (you.duration[DUR_DEFLECT_MISSILES] == 1)
    {
        mpr(MSGCH_DURATION,"You feel less protected from missiles." );
        you.duration[DUR_DEFLECT_MISSILES] = 0;
    }

    // regeneration - if starving regernation goes down.
    if (you.hunger < HUNGER_STARVING && you.duration[DUR_REGENERATION] > 0)
        you.duration[DUR_REGENERATION] = 1;

    if (you.duration[DUR_REGENERATION] > 1)
    {
        you.duration[DUR_REGENERATION]--;

        if (you.duration[DUR_REGENERATION] == 6)
        {
            mpr(MSGCH_DURATION,"Your skin is crawling a little less now." );
            if (coinflip())
                you.duration[DUR_REGENERATION]--;
        }
    }
    else if (you.duration[DUR_REGENERATION] == 1)
    {
        mpr(MSGCH_DURATION,"Your skin stops crawling." );
        you.duration[DUR_REGENERATION] = 0;
    }

    if (you.duration[DUR_PRAYER] > 1)
        you.duration[DUR_PRAYER]--;
    else if (you.duration[DUR_PRAYER] == 1)
    {
        god_speaks(you.religion, "Your prayer is over.");
        you.duration[DUR_PRAYER] = 0;
    }


    //jmf: more flexible weapon branding code
    if (you.duration[DUR_WEAPON_BRAND] > 1)
        you.duration[DUR_WEAPON_BRAND]--;
    else if (you.duration[DUR_WEAPON_BRAND] == 1)
    {
        // Note: it's not important if the weapon is no longer wieldable
        // here, only that the durational effect is ending and we still
        // should have the weapon at least "in hand".
        const int wpn = get_inv_in_hand();
        ASSERT( wpn != -1 );

        const int temp_effect = get_weapon_brand( you.inv[wpn] );

        you.duration[DUR_WEAPON_BRAND] = 0;

        char str_pass[ITEMNAME_SIZE];

        set_item_ego_type( you.inv[wpn], OBJ_WEAPONS, SPWPN_NORMAL );
        in_name( wpn, DESC_CAP_YOUR, str_pass );
        strncpy( info, str_pass, INFO_SIZE );

        switch (temp_effect)
        {
        case SPWPN_VORPAL:
            // XXX: Note that these messages are highly dependant on the
            // fact that there are two different temporary vorpalise spells
            // which work on completely different sets of weapons.  Tukima's
            // Vorpal Blade works on vorpal weapons described as "slicing",
            // and Maxwell's Silver Hammer works on any weapon that does
            // bludgeoning damage (which includes weapons like the
            // "piercing" spiked flails).
            if (get_vorpal_type( you.inv[wpn] ) == DVORP_SLICING)
                strcat(info, " seems blunter.");        // Tukima's
            else
                strcat(info, " feels lighter.");        // Maxwell's
            break;

        case SPWPN_FLAMING:
        case SPWPN_FREEZING:
            if (is_range_weapon(you.inv[wpn]) || temp_effect == SPWPN_FREEZING)
                strcat(info, " stops glowing.");
            else
                strcat(info, " goes out.");
            break;
        case SPWPN_VENOM:
            strcat(info, " stops dripping with poison.");
            break;
        case SPWPN_DRAINING:
            strcat(info, " stops crackling.");
            break;
        case SPWPN_DISTORTION:
            strcat( info, " seems straighter." );
            miscast_effect( SPTYP_TRANSLOCATION, 9, 90, 100, "a distortion effect" );
            break;
        default:
            strcat(info, " seems inexplicably less special.");
            break;
        }

        //you.attribute[ATTR_WEAPON_BRAND] = 0;
        mpr( MSGCH_DURATION, info );
        set_redraw_status( REDRAW_WIELD );
    }

    if (you.duration[DUR_BREATH_WEAPON] > 1)
        you.duration[DUR_BREATH_WEAPON]--;
    else if (you.duration[DUR_BREATH_WEAPON] == 1)
    {
        mpr(MSGCH_RECOVERY,"You have got your breath back." );
        you.duration[DUR_BREATH_WEAPON] = 0;
    }

    if (you.duration[DUR_TRANSFORMATION] > 1)
    {
        you.duration[DUR_TRANSFORMATION]--;

        if (you.duration[DUR_TRANSFORMATION] == 10)
        {
            mpr(MSGCH_DURATION,"Your transformation is almost over." );
            you.duration[DUR_TRANSFORMATION] -= random2(3);
        }
    }
    else if (you.duration[DUR_TRANSFORMATION] == 1)
    {
        untransform();
        // XXX: surely this cannot be the correct thing to do... some people
        // have breath weapons without transformation.
        // you.duration[DUR_BREATH_WEAPON] = 0;
    }

    if (you.duration[DUR_SWIFTNESS] > 1)
    {
        you.duration[DUR_SWIFTNESS]--;
        if (you.duration[DUR_SWIFTNESS] == 6)
        {
            mpr(MSGCH_DURATION,"You start to feel a little slower." );
            if (coinflip())
                you.duration[DUR_SWIFTNESS]--;
        }
    }
    else if (you.duration[DUR_SWIFTNESS] == 1)
    {
        mpr(MSGCH_DURATION,"You feel sluggish." );
        you.duration[DUR_SWIFTNESS] = 0;
    }

    if (you.duration[DUR_INSULATION] > 1)
    {
        you.duration[DUR_INSULATION]--;
        if (you.duration[DUR_INSULATION] == 6)
        {
            mpr(MSGCH_DURATION,"You start to feel a little less insulated." );
            if (coinflip())
                you.duration[DUR_INSULATION]--;
        }
    }
    else if (you.duration[DUR_INSULATION] == 1)
    {
        mpr(MSGCH_DURATION,"You feel conductive." );
        you.duration[DUR_INSULATION] = 0;
    }

    if (you.duration[DUR_STONEMAIL] > 1)
    {
        you.duration[DUR_STONEMAIL]--;
        if (you.duration[DUR_STONEMAIL] == 6)
        {
            mpr(MSGCH_DURATION,"Your scaley stone armour is starting to flake away." );
            set_redraw_status( REDRAW_ARMOUR_CLASS );
            if (coinflip())
                you.duration[DUR_STONEMAIL]--;
        }
    }
    else if (you.duration[DUR_STONEMAIL] == 1)
    {
        mpr(MSGCH_DURATION,"Your scaley stone armour disappears." );
        you.duration[DUR_STONEMAIL] = 0;
        set_redraw_status( REDRAW_ARMOUR_CLASS );
        burden_change();
    }

    if (you.duration[DUR_FORESCRY] > 1) //jmf: added
        you.duration[DUR_FORESCRY]--;
    else if (you.duration[DUR_FORESCRY] == 1)
    {
        mpr(MSGCH_DURATION,"You feel firmly rooted in the present." );
        you.duration[DUR_FORESCRY] = 0;
        set_redraw_status( REDRAW_EVASION );
    }

    if (you.duration[DUR_SEE_INVISIBLE] > 1)    //jmf: added
        you.duration[DUR_SEE_INVISIBLE]--;
    else if (you.duration[DUR_SEE_INVISIBLE] == 1)
    {
        you.duration[DUR_SEE_INVISIBLE] = 0;

        if (!player_see_invis())
            mpr(MSGCH_DURATION,"Your eyesight blurs momentarily." );
    }

    if (you.duration[DUR_SILENCE] > 0)  //jmf: cute message handled elsewhere
        you.duration[DUR_SILENCE]--;

    if (you.duration[DUR_CONDENSATION_SHIELD] > 1)
    {
        you.duration[DUR_CONDENSATION_SHIELD]--;

        if (player_res_cold() <= 0)
        {
            mpr( "You feel cold." );
            int dam = roll_dice(2,5) * you.time_taken;

            dam = (check_your_resists( dam, BEAM_COLD ) + 5) / 10;
            ouch( dam, 0, KILLED_BY_FREEZING );
        }
    }
    else if (you.duration[DUR_CONDENSATION_SHIELD] == 1)
    {
        you.duration[DUR_CONDENSATION_SHIELD] = 0;
        mpr(MSGCH_DURATION,"Your icy shield evaporates." );
        set_redraw_status( REDRAW_ARMOUR_CLASS );
    }

    if (you.duration[DUR_STONESKIN] > 1)
        you.duration[DUR_STONESKIN]--;
    else if (you.duration[DUR_STONESKIN] == 1)
    {
        mpr(MSGCH_DURATION,"Your skin feels tender." );
        set_redraw_status( REDRAW_ARMOUR_CLASS );
        you.duration[DUR_STONESKIN] = 0;
    }

    if (you.duration[DUR_TELEPORT] > 1)
        you.duration[DUR_TELEPORT]--;
    else if (you.duration[DUR_TELEPORT] == 1)
    {
        // only to a new area of the abyss sometimes (for abyss teleports)
        you_teleport2( true, one_chance_in(5) );
        you.duration[DUR_TELEPORT] = 0;
    }

    if (you.duration[DUR_CONTROL_TELEPORT] > 1)
    {
        you.duration[DUR_CONTROL_TELEPORT]--;

        if (you.duration[DUR_CONTROL_TELEPORT] == 6)
        {
            mpr(MSGCH_DURATION,"You start to feel a little uncertain." );
            if (coinflip())
                you.duration[DUR_CONTROL_TELEPORT]--;
        }
    }
    else if (you.duration[DUR_CONTROL_TELEPORT] == 1)
    {
        mpr(MSGCH_DURATION,"You feel uncertain." );
        you.duration[DUR_CONTROL_TELEPORT] = 0;
    }

    if (you.duration[DUR_RESIST_POISON] > 1)
    {
        you.duration[DUR_RESIST_POISON]--;
        if (you.duration[DUR_RESIST_POISON] == 6)
        {
            mpr(MSGCH_DURATION,"Your poison resistance is about to expire." );
            if (coinflip())
                you.duration[DUR_RESIST_POISON]--;
        }
    }
    else if (you.duration[DUR_RESIST_POISON] == 1)
    {
        mpr(MSGCH_DURATION,"Your poison resistance expires." );
        you.duration[DUR_RESIST_POISON] = 0;
    }

    if (you.duration[DUR_DEATH_CHANNEL] > 1)
    {
        you.duration[DUR_DEATH_CHANNEL]--;
        if (you.duration[DUR_DEATH_CHANNEL] == 6)
        {
            mpr(MSGCH_DURATION,"Your unholy channel is weakening." );
            if (coinflip())
                you.duration[DUR_DEATH_CHANNEL]--;
        }
    }
    else if (you.duration[DUR_DEATH_CHANNEL] == 1)
    {
        mpr(MSGCH_DURATION,"Your unholy channel expires." );    // Death channel woreoff
        you.duration[DUR_DEATH_CHANNEL] = 0;
    }

    // XXX: still needs work
    if (you.duration[DUR_CUT] > 1)
    {
        if (one_chance_in(3))
        {
            const int dam = roll_dice( 1, 3 + you.duration[DUR_CUT] / 5 );

            mpr( "Your wounds bleed!" );
            ouch( dam, 0, KILLED_BY_BLEEDING );

            if (one_chance_in(3))
                you.duration[DUR_CUT]--;
        }
    }
    else if (you.duration[DUR_CUT] == 1)
    {
        mpr( "Your wounds stop bleeding." );
        you.duration[DUR_CUT] = 0;
    }

    if (you.invis > 1)
    {
        you.invis--;

        if (you.invis == 6)
        {
            mpr(MSGCH_DURATION,"You flicker for a moment." );
            if (coinflip())
                you.invis--;
        }
    }
    else if (you.invis == 1)
    {
        mpr(MSGCH_DURATION,"You flicker back into view." );
        you.invis = 0;
    }

    if (you.duration[DUR_STUN] > 0)
    {
        you.duration[DUR_STUN]--;

        if (you.duration[DUR_STUN] == 0)
            mpr( "You are no longer stunned." );
    }

    if (you.conf > 0)
        reduce_confuse_player(1);

    if (you.paralysis > 1)
        you.paralysis--;
    else if (you.paralysis == 1)
    {
        mpr(MSGCH_DURATION,"You can move again." );
        you.paralysis = 0;
    }

    if (you.exhausted > 1)
        you.exhausted--;
    else if (you.exhausted == 1)
    {
        mpr( MSGCH_DURATION,"You feel less fatigued."  );
        you.exhausted = 0;
    }

    dec_slow_player();
    dec_haste_player();

    // Might and berserk are now combined here so that there's only one
    // subtraction to strength (prevents stat loss bug).  Because of this,
    // the berserk going-down code is here and not in dec_berserk (which
    // redirects to here).
    if (you.might || you.berserker)
    {
        if (you.might > 0)
        {
            you.might--;

            // strength reduction handled below
            if (!you.might && !you.berserker)
                mpr(MSGCH_DURATION,"You feel a little less mighty now." );
        }

        if (you.berserker > 1)
            dec_berserk(1);
        else if (you.berserker == 1)
        {
            mpr( MSGCH_DURATION, "You are no longer berserk." );
            you.berserker = 0;

            set_redraw_status( REDRAW_STRENGTH );    // always redraw

            //jmf: guilty for berserking /after/ berserk
            did_god_conduct( DID_STIMULANTS, 6 + random2(6) );

            // Sometimes berserk leaves us physically drained
            //
            // chance of passing out:
            //     - mutation gives a large plus in order to try and
            //       avoid the mutation being a "death sentence" to
            //       certain characters.
            //     - knowing the spell gives an advantage just
            //       so that people who have invested 3 spell levels
            //       are better off than the casual potion drinker...
            //       this should make it a bit more interesting for
            //       Crusaders again.
            //     - similarly for the amulet
            int chance = 10 + you.mutation[MUT_BERSERK] * 25
                            + (player_equip( EQ_AMULET, AMU_RAGE ) ? 10 : 0)
                            + (player_has_spell( SPELL_BERSERKER_RAGE ) ? 10 : 0);

            // Note the beauty of Trog!  They get an extra save that's at
            // the very least 20% and goes up to 100%.
            if (you.berserk_penalty == NO_BERSERK_PENALTY
                || (you.religion == GOD_TROG && you.piety > random2(150))
                || !one_chance_in( chance ))
            {
                mpr("You are exhausted.");
            }
            else
            {
                mpr( MSGCH_WARN,"You pass out from exhaustion."  );
                paralyse_player( roll_dice(2, 2), true );
            }

            // this resets from an actual penalty or from NO_BERSERK_PENALTY
            you.berserk_penalty = 0;

            fatigue_player( 12 + roll_dice( 2, 12 ) );
            make_hungry( 800, false );

            if (you.hunger < HUNGER_STARVATION_DEATH + 50)
                you.hunger = HUNGER_STARVATION_DEATH + 50;

            calc_hp_max();
        }

        if (!you.might && !you.berserker)
            modify_stat( STAT_STRENGTH, -5, true );
    }

    if (you.confusing_touch > 1)
        you.confusing_touch--;
    else if (you.confusing_touch == 1)
    {
        mpr( MSGCH_DURATION, "Your %s stop glowing.", your_hand(true) );
        you.confusing_touch = 0;
    }

    if (you.sure_blade > 1)
        you.sure_blade--;
    else if (you.sure_blade == 1)
    {
        mpr(MSGCH_DURATION,"The bond with your blade fades away." );
        you.sure_blade = 0;
        set_redraw_status( REDRAW_WIELD );
    }

    if (you.levitation > 1)
    {
        if (you.species != SP_KENKU || you.xp_level < 15)
            you.levitation--;

        if (player_equip_ego_type( EQ_BOOTS, SPARM_LEVITATION ))
            you.levitation = 2;

        if (you.levitation == 10)
        {
            mpr(MSGCH_DURATION,"You are starting to lose your buoyancy!" );
            you.levitation -= random2(6);

            if (you.duration[DUR_CONTROLLED_FLIGHT] > 0)
                you.duration[DUR_CONTROLLED_FLIGHT] = you.levitation;
        }
    }
    else if (you.levitation == 1)
    {
        mpr(MSGCH_DURATION,"You float gracefully downwards." );
        you.levitation = 0;
        burden_change();
        you.duration[DUR_CONTROLLED_FLIGHT] = 0;

        // re-enter the terrain:
        move_player_to_grid( you.x_pos, you.y_pos, false, true, true );
    }

    const int rot_chance = 150 * you.rotting + 10 * (you.species == SP_GHOUL);

    if (random2(3000) < rot_chance)
    {
        const int amount = 1 + div_rand_round( you.hp_max, 50 );

        mpr( MSGCH_WARN, "You feel your flesh rotting away." ); ouch( amount,
                0, KILLED_BY_ROTTING ); rot_hp( amount );

        if (you.rotting > 0)
            you.rotting--;
    }

    dec_disease_player();

    if (you.poison > 0)
    {
        // Trying out Gavin's multi-level poison resistance ideas.
        // Question: Does this make swapping in and out a ring a cheap
        // way to dodge poisoning?  Is it too twinky?
        // XXX: better define these levels or go back to a simple one level?
        if (random2(5) < you.poison)
        {
            const int res = player_res_poison();
            int poison = 0;

            // get poison level
            if (you.poison > 10 && random2( you.poison ) >= 8)
                poison = 3;
            else if (you.poison > 5 && random2( you.poison ) >= 3)
                poison = 2;
            else
                poison = 1;

            // apply resistance to level
            if (res >= 3 || res <= 0)
                poison -= res;
            else // 1 or 2 levels
                poison -= random2( res + 1 );

            // apply level to player

            // Like before: 3 -> 5 + random2(10), but this gives
            // values in case player susceptibility is ever used
            // (ie 4 -> 8 + random2(16), 5 -> 13 + random2(26), ...)
            //
            // Now merging that back to handle levels 1 and 2 since
            // it was pretty close and it makes the progression more
            // consistant (although the lower levels are now slightly
            // stronger).
            //
            // Reduced random part by 1... 1st level poison is now the
            // same as it used to be (level two is still a bit harsher)
            const int mag = (poison * poison + 1) / 2;

            mpr( (poison >= 3) ? MSGCH_WARN : MSGCH_PLAIN,
                    "You feel %ssick.", (poison >= 3) ? "extremely " :
                                        (poison == 2) ? "very "
                                                      : "" );

            ouch( mag + random2( 2 * mag - 1 ), 0, KILLED_BY_POISON );

            // reduce poison counter
            if ((you.hp == 1 && one_chance_in(3)) || one_chance_in(8))
                reduce_poison_player(1);
        }
    }

    if (you.deaths_door)
    {
        if (you.hp > allowed_deaths_door_hp())
        {
            mpr(MSGCH_DURATION,"Your life is in your own hands once again." );
            paralyse_player( 5 + random2(5) );
            confuse_player( 10 + random2(10) );
            dec_max_hp( coinflip() ? 3 : 2 );
            you.deaths_door = 0;
        }
        else
            you.deaths_door--;

        if (you.deaths_door == 10)
        {
            mpr( MSGCH_DURATION,"Your time is quickly running out!"  );
            you.deaths_door -= random2(6);
        }

        if (you.deaths_door == 1)
        {
            mpr( MSGCH_DURATION,"Your life is in your own hands again!"  );
            more();
        }
    }

    const int food_use = player_hunger_rate();

    if (food_use > 0 && you.hunger > HUNGER_STARVATION_DEATH)
        make_hungry( food_use, true );

    // XXX: using an int tmp to fix the fact that hit_points_regeneration
    // is only an unsigned char and is thus likely to overflow. -- bwr
    int tmp = static_cast< int >( you.hit_points_regeneration );

    if (you.hp < you.hp_max && !you.disease && !you.deaths_door)
        tmp += player_regen();

    while (tmp >= 100)
    {
        if (you.hp >= you.hp_max - 1 && is_resting())
            stop_running();

        inc_hp(1, false);
        tmp -= 100;
    }

    ASSERT( tmp >= 0 && tmp < 100 );
    you.hit_points_regeneration = static_cast< unsigned char >( tmp );

    // XXX: Doing the same as the above, although overflow isn't an
    // issue with magic point regeneration, yet. -- bwr
    tmp = static_cast< int >( you.magic_points_regeneration );

    if (you.magic_points < you.max_magic_points)
        tmp += player_magic_regen();

    while (tmp >= 100)
    {
        if (you.magic_points >= you.max_magic_points - 1 && is_resting())
            stop_running();

        inc_mp(1, false);
        tmp -= 100;
    }

    ASSERT( tmp >= 0 && tmp < 100 );
    you.magic_points_regeneration = static_cast< unsigned char >( tmp );

    const int rod = get_inv_hand_tool();

    if (rod != -1
        && item_is_rod( you.inv[rod] )
        && you.inv[rod].plus < you.inv[rod].plus2
        && enough_mp( 1, true ))
    {
        const int charge = you.inv[rod].plus / ROD_CHARGE_MULT;

        int rate = ((charge + 1) * ROD_CHARGE_MULT) / 10;

        rate *= (10 + skill_bump( SK_EVOCATIONS ));
        rate = div_rand_round( rate, 100 );

        if (rate < 5)
            rate = 5;
        else if (rate > ROD_CHARGE_MULT / 2)
            rate = ROD_CHARGE_MULT / 2;

        if (you.inv[rod].plus / ROD_CHARGE_MULT
            != (you.inv[rod].plus + rate) / ROD_CHARGE_MULT)
        {
            dec_mp(1);
            if (item_ident( you.inv[rod], ISFLAG_KNOW_PLUSES ))
                set_redraw_status( REDRAW_WIELD );
        }

        you.inv[rod].plus += rate;

        if (you.inv[rod].plus == you.inv[rod].plus2 && is_resting())
            stop_running();
    }
}

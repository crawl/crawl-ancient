/*
 *  File:       acr.cc
 *  Summary:    Main entry point, event loop, and some initialization functions.
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 *
 *      <9>     6/12/99         BWR             New init code, restructured
 *                                              wiz commands, added equipment
 *                                              listing commands
 *      <8>     6/7/99          DML             Autopickup
 *      <7>     5/30/99         JDJ             Added game_has_started.
 *      <6>     5/25/99         BWR             Changed move() to move_player()
 *      <5>     5/20/99         BWR             NEW_BERSERK code, checking
 *                                              scan_randarts for NO_TELEPORT
 *                                              and NO_SPELLCASTING.
 *      <4>     5/12/99         BWR             Solaris support.
 *      <3>     5/09/99         JDJ             look_around no longer prints a prompt.
 *      <2>     5/08/99         JDJ             you and env are no longer arrays.
 *      <1>     -/--/--         LRH             Created
 */

#include "AppHdr.h"

#ifdef DOS
#include <conio.h>
#endif

#ifdef USE_UNIX_SIGNALS
#include <signal.h>
#endif

#include <time.h>
#include <stdlib.h>
#include <string.h>

#ifdef DOS
#include <file.h>
#endif

#ifdef USE_EMX
#include <sys/types.h>
#endif

#ifdef MAC
#include <stat.h>
#else
#include <sys/stat.h>
#endif

#include <fcntl.h>
#include <stdio.h>

#include "externs.h"
#include "enum.h"


#include "ability.h"
#include "chardump.h"
#include "command.h"
#include "debug.h"
#include "describe.h"
#include "direct.h"
#include "dungeon.h"
#include "effects.h"
#include "fight.h"
#include "files.h"
#include "food.h"
#include "initfile.h"
#include "invent.h"
#include "it_use2.h"
#include "it_use3.h"
#include "item_use.h"
#include "itemname.h"
#include "items.h"
#include "levels.h"

#ifdef MACROS
  #include "macro.h"
#endif

#ifdef WIZARD
  #include "spells1.h"
#endif

#include "message.h"
#include "misc.h"
#include "monplace.h"
#include "monstuff.h"
#include "mstruct.h"
//#include "mstuff2.h"
#include "mutation.h"
#include "newgame.h"
#include "ouch.h"
#include "output.h"
#include "player.h"
#include "abyss.h"
#include "randart.h"
#include "religion.h"
//#include "shopping.h"
#include "skills.h"
#include "skills2.h"
#include "spell.h"
#include "spells.h"
//#include "spells1.h"
#include "spells2.h"
#include "spells3.h"
#include "stuff.h"
#include "transfor.h"
#include "view.h"

/*
   #include "player.h"
   #include "message.h"
   #include "output.h"
   #include "view.h"
   #include "newgame.h"
   #include "items.h"
   #include "files.h"
   #include "itemname.h"
   #include "fight.h"
   #include "mstruct.h"
   #include "item_use.h"
   #include "it_use2.h"
   #include "ouch.h"
   #include "spells1.h"
   #include "invent.h"
   #include "direct.h"
   #include "skills.h"
   #include "skills2.h"
   #include "player.h"
   #include "chardump.h"
   #include "misc.h"
   #include "newgame.h"
 */
/* Functions in main module */
void input(void);
void open_door(char move_x, char move_y);
void close_door(char move_x, char move_y);
void move_player(char move_x, char move_y);

#ifdef USE_NEW_BERSERK
void do_berserk_no_combat_penalty(void);
#endif

//void missile(struct player you [1], struct environ env [1], struct bolt beam [1], int throw_2);
//void beam(struct player you [1], struct environ env [1], struct bolt beam [1]);


void more();
void initialise();



struct environ env;
struct player you;
struct system_environment sys_env;
char info[200];
char st_prn[20];

char str_pass[80];
char gmon_use[1000];

int stealth;                    /* externed in view.h */

char use_colour = 1;

char visible[10];

bool game_has_started = false;  // set to true once a new game has been started or an old game has been loaded

/*

   Functions needed:
   New:
   int player_speed(player you);
   mpr(char message []);
   hit_player(damage, flavour, then last two ouch values, env);


   Old:
   print_stats(player you);
   wield(player you);
   show_map
   noisy()
   losight

 */

void (*viewwindow) (char, bool);

/* these are all defined in view.cc: */
extern unsigned char (*mapch) (unsigned char);
extern unsigned char (*mapch2) (unsigned char);
unsigned char mapchar(unsigned char ldfk);
unsigned char mapchar2(unsigned char ldfk);
unsigned char mapchar3(unsigned char ldfk);
unsigned char mapchar4(unsigned char ldfk);

/*
   Function pointers are used to make switching between Linux and DOS char sets
   possible as a runtime option (command-line -c)
 */

extern unsigned char your_sign; /* these two are defined in view.cc. What does the player look like? (changed for shapechanging */
extern unsigned char your_colour;

extern char wield_change;       /* defined in output.cc */

#ifdef LINUX
static bool Use_No_Black = false;
#endif

/*
   It all starts here. Some initialisations are run first, then straight to
   new_game and then input.
 */
int main(int argc, char *argv[])
{

    // Load in the system environment variables
    get_system_environment();

    // Read the init file
    read_init_file();

#ifdef USE_ASCII_CHARACTERS
    // Default to the non-ibm set when it makes sense.
    viewwindow = &viewwindow3;
    mapch = &mapchar3;
    mapch2 = &mapchar4;
#else
    // Use the standard ibm default
    viewwindow = &viewwindow2;
    mapch = &mapchar;
    mapch2 = &mapchar2;
#endif

    if (argc > 1)
    {
        if (stricmp(argv[1], "-c") == 0 || stricmp(argv[1], "-nc") == 0
                                           || stricmp(argv[1], "-nb") == 0)
        {
            viewwindow = &viewwindow3;
            mapch = &mapchar3;
            mapch2 = &mapchar4;

            if (stricmp(argv[1], "-nc") == 0)
            {
                use_colour = 0; /* this is global to this function, so can either be
                                   passed eg to lincurses_startup or defined as an
                                   extern in another module */
            }
#ifdef LINUX
            else if (stricmp(argv[1], "-nb") == 0)
            {
                Use_No_Black = true;
            }
#endif
        }
        else
        {
            printf(EOL "Crawl accepts the following arguments only:" EOL);
            printf(" -c   Use non-ibm character set" EOL);
            printf(" -nc  Use non-ibm character set, but no colour" EOL);
#ifdef LINUX
            printf(" -nb  Use colour and non-ibm charater set, but no "
                                        "black/dark grey characters" EOL);
#endif
            printf(EOL "Any others will cause this message to be printed again." EOL);
            exit(1);
        }
    }

#ifdef LINUX
    lincurses_startup( Use_No_Black );
#endif

#ifdef MAC
    init_mac();
#endif

#ifdef MACROS
    // Load macros
    macro_init();
#endif

//new_game();

    initial();
    initialise();

    while (true)
    {
        input();
//      cprintf("x");
    }
/* Should never reach this stage, right? */
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
}



/*
   This function handles the player's input. It's called from main(), from inside
   an endless loop.
 */
void input()
{
    int plox[2];
    char move_x = 0;
    char move_y = 0;

    int keyin = 0;
    char str_pass[50];

#ifdef LINUX
    // Stuff for the Unix keypad kludge
    bool running = false;
    bool opening = false;
#endif

    you.time_taken = player_speed();

#ifdef USE_CURSES
    // yes this isn't pretty, but it avoids an evil namespace collision
    // between <string> and <curses.h>.
    window(1, 1, 80, 25);
#else
    window(1, 1, 80, NUMBER_OF_LINES);
#endif

    textcolor(7);

    print_stats();

    if (you.paralysis == 0)
    {
        if (you.delay_t > 1)
            you.delay_t--;
        if (you.delay_t == 1)
        {
            switch (you.delay_doing)
            {
            case DELAY_EAT:
                mpr("You finish eating.");
                break;

            case DELAY_ARMOR_ON:
                mpr("You finish putting on your armour.");
                break;

            case DELAY_ARMOR_OFF:
                mpr("You finish taking off your armour.");
                break;

            case DELAY_MEMORIZE:
                mpr("You finish memorising.");
                break;

            case DELAY_BUTCHER:
                if (you.species == SP_TROLL || you.species == SP_GHOUL)
                    mpr("You finish ripping the corpse into pieces.");
                else
                    mpr("You finish chopping the corpse into pieces.");

                if (you.berserker && you.berserk_penalty != NO_BERSERK_PENALTY)
                {
                    mpr("You enjoyed that.");
                    you.berserk_penalty = 0;
                }
                break;

            case DELAY_AUTOPICKUP:
              /* no point in saying anything -- it's obvious from the
                 list of things picked up.*/
              break;
            }
            you.delay_t = 0;
        }

        gotoxy(18, 9);

        if (you.delay_t == 0)
        {
            _setcursortype(_NORMALCURSOR);

            if (you.running > 0)
            {
                keyin = 125;
                move_x = you.run_x;
                move_y = you.run_y;

                if (kbhit())
                {
                    you.running = 0;
                    goto gutch;
                }

                if (you.run_x == 0 && you.run_y == 0)
                {
                    you.running--;
                    keyin = '.';
                }
            }
            else
              gutch:keyin = getch();

            mesclr();

#ifdef LINUX
            // Kludging running and opening as two character sequences
            // for Unix systems.  This is an easy way out... all the
            // player has to do is find a termcap and numlock setting
            // that will get curses the numbers from the keypad.  This
            // will hopefully be easy.
            if (keyin == '*')
            {
                opening = true;
                keyin = getch();
            }
            else if (keyin == '/')
            {
                running = true;
                keyin = getch();
            }

            // Translate keypad codes into numeric chars
            keyin = translate_keypad( keyin );

#else
            // Old DOS keypad support
            if (keyin == 0)     // ALT also works - see ..\KEYTEST.CPP
            {
                keyin = getch();
                switch (keyin)
                {
                case 'O':
                    move_x = -1;
                    move_y = 1;
                    break;
                case 'P':
                    move_y = 1;
                    move_x = 0;
                    break;
                case 'I':
                    move_x = 1;
                    move_y = -1;
                    break;
                case 'H':
                    move_y = -1;
                    move_x = 0;
                    break;
                case 'G':
                    move_y = -1;
                    move_x = -1;
                    break;
                case 'K':
                    move_x = -1;
                    move_y = 0;
                    break;
                case 'Q':
                    move_y = 1;
                    move_x = 1;
                    break;
                case 'M':
                    move_x = 1;
                    move_y = 0;
                    break;

                case 119:
                    open_door(-1, -1);
                    move_x = 0;
                    move_y = 0;
                    break;
                case -115:
                    open_door(0, -1);
                    move_x = 0;
                    move_y = 0;
                    break;
                case -124:
                    open_door(1, -1);
                    move_x = 0;
                    move_y = 0;
                    break;
                case 116:
                    open_door(1, 0);
                    move_x = 0;
                    move_y = 0;
                    break;
                case 118:
                    open_door(1, 1);
                    move_x = 0;
                    move_y = 0;
                    break;
                case -111:
                    open_door(0, 1);
                    move_x = 0;
                    move_y = 0;
                    break;
                case 117:
                    open_door(-1, 1);
                    move_x = 0;
                    move_y = 0;
                    break;
                case 115:
                    open_door(-1, 0);
                    move_x = 0;
                    move_y = 0;
                    break;

                case 76:
                case 'S':
                    keyin = '.';
                    goto get_keyin_again;

                }

                keyin = 125;
            }
#endif
        }
        else
            keyin = '.';
    }
    else
        keyin = '.';            // end of if you.paralysis == 0

    if (keyin != 125)
    {
        move_x = 0;
        move_y = 0;
        you.turn_is_over = 0;
    }

get_keyin_again:
    //putch(keyin);
    //getch();

    char save_key;
#ifdef WIZARD
    int wiz_command, i, j;
#endif

    switch (keyin)
    {
    case 25:
        open_door(-1, -1);
        move_x = 0;
        move_y = 0;
        break;
    case 11:
        open_door(0, -1);
        move_x = 0;
        move_y = 0;
        break;
    case 21:
        open_door(1, -1);
        move_x = 0;
        move_y = 0;
        break;
    case 12:
        open_door(1, 0);
        move_x = 0;
        move_y = 0;
        break;
    case 14:
        open_door(1, 1);
        move_x = 0;
        move_y = 0;
        break;
    case 10:
        open_door(0, 1);
        move_x = 0;
        move_y = 0;
        break;
    case 2:
        open_door(-1, 1);
        move_x = 0;
        move_y = 0;
        break;
    case 8:
        open_door(-1, 0);
        move_x = 0;
        move_y = 0;
        break;
    case 'b':
        move_x = -1;
        move_y = 1;
        break;
    case 'j':
        move_y = 1;
        move_x = 0;
        break;
    case 'u':
        move_x = 1;
        move_y = -1;
        break;
    case 'k':
        move_y = -1;
        move_x = 0;
        break;
    case 'y':
        move_y = -1;
        move_x = -1;
        break;
    case 'h':
        move_x = -1;
        move_y = 0;
        break;
    case 'n':
        move_y = 1;
        move_x = 1;
        break;
    case 'l':
        move_x = 1;
        move_y = 0;
        break;

    case 'B':
        you.run_x = -1;
        you.run_y = 1;
        you.running = 2;
        break;
    case 'J':
        you.run_y = 1;
        you.run_x = 0;
        you.running = 2;
        break;
    case 'U':
        you.run_x = 1;
        you.run_y = -1;
        you.running = 2;
        break;
    case 'K':
        you.run_y = -1;
        you.run_x = 0;
        you.running = 2;
        break;
    case 'Y':
        you.run_y = -1;
        you.run_x = -1;
        you.running = 2;
        break;
    case 'H':
        you.run_x = -1;
        you.run_y = 0;
        you.running = 2;
        break;
    case 'N':
        you.run_y = 1;
        you.run_x = 1;
        you.running = 2;
        break;
    case 'L':
        you.run_x = 1;
        you.run_y = 0;
        you.running = 2;
        break;

#ifdef LINUX
    // Nex Unix keypad support
    case '1':
        move_x = -1;
        move_y = 1;
        break;

    case '2':
        move_y = 1;
        move_x = 0;
        break;

    case '9':
        move_x = 1;
        move_y = -1;
        break;

    case '8':
        move_y = -1;
        move_x = 0;
        break;

    case '7':
        move_y = -1;
        move_x = -1;
        break;

    case '4':
        move_x = -1;
        move_y = 0;
        break;

    case '3':
        move_y = 1;
        move_x = 1;
        break;

    case '6':
        move_x = 1;
        move_y = 0;
        break;

    case '5':
        // Yes this is backwards, but everyone here is used to using
        // straight 5s for long rests... might need to a roguelike
        // rest key and get people switched over.

        if (!running && !opening)
        {
            you.run_x = 0;
            you.run_y = 0;
            you.running = 100;
        }
        else
        {
            search_around();
            move_x = 0;
            move_y = 0;
            you.turn_is_over = 1;
        }
        break;
#else
    // Old DOS keypad support
    case '1':
        you.run_x = -1;
        you.run_y = 1;
        you.running = 2;
        break;
    case '2':
        you.run_y = 1;
        you.run_x = 0;
        you.running = 2;
        break;
    case '9':
        you.run_x = 1;
        you.run_y = -1;
        you.running = 2;
        break;
    case '8':
        you.run_y = -1;
        you.run_x = 0;
        you.running = 2;
        break;
    case '7':
        you.run_y = -1;
        you.run_x = -1;
        you.running = 2;
        break;
    case '4':
        you.run_x = -1;
        you.run_y = 0;
        you.running = 2;
        break;
    case '3':
        you.run_y = 1;
        you.run_x = 1;
        you.running = 2;
        break;
    case '6':
        you.run_x = 1;
        you.run_y = 0;
        you.running = 2;
        break;
    case '5':
        you.run_x = 0;
        you.run_y = 0;
        you.running = 100;
        break;
#endif

    case '':
      autopickup_on = !autopickup_on;
      if(autopickup_on)
        mpr("Autopickup is now on.");
      else
        mpr("Autopickup is now off.");
      break;

    case '<':
        up_stairs();
        break;
    case '>':
        down_stairs(0, you.your_level);
        break;
    case 'o':
        open_door(100, 100);
        break;
    case 'c':
        close_door(100, 100);
        break;
    case 'd':
        drop();
        break;
    case 'D':
        butchery();
        break;
    case 'i':
        get_invent(-1);
        break;
    case 'I':
        invoke_wielded();
        break;
    case 'g':
    case ',':
        pickup();
        break;
    case ';':
        item_check(';');
        break;
    case 'w':
        wield_weapon(0);
        break;
    case 't':
        throw_anything();
        break;
    case 'f':
        shoot_thing();
        break;
    case 'T':
        {
        int index;
        if (armour_prompt("Take off which item?", &index))
                        takeoff_armour(index);
                }
        break;
    case 'R':
        remove_ring();
        break;
    case 'P':
        puton_ring();
        break;
    case 'W':
        wear_armour();
        break;
    case '=':
        adjust();
        return;
    case 'M':
        which_spell();
        break;
    case 'z':
        zap_wand();
        break;
    case 'e':
        eat_food();
        break;
    case 'a':
        species_ability();
#ifdef PLAIN_TERM
        // If we do this we'll never see the "no abilities" message.
        //redraw_screen();
#endif
        break;
    case 'A':
        display_mutations();
#ifdef PLAIN_TERM
        redraw_screen();
#endif
        break;
    case 'V':
        original_name();
        break;
    case 'p':
        pray();
        break;
    case '^':
        if (you.religion == GOD_NO_GOD)
        {
            mpr("You aren't religious.");
            break;
        }
        describe_god(you.religion);
#ifdef PLAIN_TERM
        redraw_screen();
#endif
        break;
    case '.':
        search_around();
        move_x = 0;
        move_y = 0;
        you.turn_is_over = 1;
        break;
    case 'q':
        drink();
        break;
    case 'r':
        read_scroll();
        break;
    case 'x':
        mpr("Move the cursor around to observe a square.");
        mpr("Press '?' for a monster description.");

        struct dist lmove[1];

        look_around(lmove);
        break;
    case 's':
#ifdef WIZARD
        stethoscope(250);
        break;
#endif
        search_around();
        you.turn_is_over = 1;
        break;
    case 'Z':
        /* randart wpns */
        if (scan_randarts(RAP_PREVENT_SPELLCASTING))
        {
            mpr("Something is interfering with your magic!");
            break;
        }
        cast_a_spell();
        break;

//              case 'M': which_spell(); break;      //memorise_spell(); break;
    case '\'':
        wield_weapon(1);
        break;
    case 'X':
        if (you.level_type == 1 || you.level_type == 2)
        {
            strcpy(info, "You have no idea where you are!");
            mpr(info);
            break;
        }
        plox[0] = 0;
        show_map(plox);
#ifdef PLAIN_TERM
        redraw_screen();
#endif
        break;
    case '\\':
        check_item_knowledge(); //nothing = check_item_knowledge();
#ifdef PLAIN_TERM
        redraw_screen();
#endif
        break;
    case 16:
        replay_messages();
#ifdef PLAIN_TERM
        redraw_screen();
#endif
        break;

#ifdef PLAIN_TERM
    case 18:
        // CTRL-R redraws the screen
        redraw_screen();
        break;
#endif

    case 24:
        mpr("Saving game... please wait");
        save_game(1);
        break;

#ifdef USE_UNIX_SIGNALS
    case 26:
        // CTRL-Z suspend behaiour is implemented here.
        // because we want to have CTRL-Y available...
        // we have to stop both from sending the signals,
        // so we reimplement here
        clrscr();
        lincurses_shutdown();
        kill(0, SIGTSTP);
        lincurses_startup( Use_No_Black );
        redraw_screen();
        break;
#endif

    case '?':
        list_commands();
#ifdef PLAIN_TERM
        redraw_screen();
#endif
        break;
    case 'C':
        strcpy(info, "You are a level ");
        itoa(you.experience_level, st_prn, 10);
        strcat(info, st_prn);
        strcat(info, " ");
        strcat(info, species_name(you.species));
        strcat(info, " ");
        strcat(info, you.class_name);
        strcat(info, ".");
        mpr(info);

        if (you.experience_level == 27)
        {
            strcpy(info, "I'm sorry, level 27 is as high as you can go.");
            mpr(info);
            strcpy(info, "With the way you've been playing, I'm surprised you got this far.");
            mpr(info);
            break;
        }
        strcpy(info, "Your next level is at ");
        itoa(exp_needed(you.experience_level + 2, you.species) + 1, st_prn, 10);
        strcat(info, st_prn);
        strcat(info, " experience points.");
        mpr(info);
        break;
    case '!':
        yell();                 /* in effects.cc */
        break;
    case '@':
        display_char_status();
        break;


    case 'm':
        show_skills();
#ifdef PLAIN_TERM
        redraw_screen();
#endif
        break;

//  case '^': disarm_trap(); break;

    case '#':
        char name_your[kNameLen];

        strncpy(name_your, you.your_name, kFileNameLen);
        name_your[kFileNameLen] = 0;
        if (dump_char(0, name_your) == 1)
            strcpy(info, "Char dumped successfully.");
        else
            strcpy(info, "Char dump unsuccessful! Sorry about that.");
        mpr(info);
        break;

#ifdef MACROS
    case '`':
        macro_add_query();
        break;
    case '~':
        mpr("Saving macros.");
        macro_save();
        break;
#endif



/* Debug starts: * /


   case '&': cast_spec_spell(); break;
   case '%': create_spec_object2(); break;
   case '*': grd [you.x_pos] [you.y_pos] = 82; break;
   case '(':
   char specs [3];
   strcpy(info, "Create which feature? ");
   mpr(info);
   specs [0] = getche();
   specs [1] = getche();
   specs [2] = getche();
   grd [you.x_pos] [you.y_pos] = atoi(specs); break;
   case 'G': debug_add_skills(); break;
   //  case '\"': grd [you.x_pos] [you.y_pos] = 99; break;
   //  case '\'': grd [you.x_pos] [you.y_pos] = 101; break;
   case '+': create_spec_monster(); break;

   //case '`':
   //   itoa(grd [you.x_pos] [you.y_pos], st_prn, 10);
   //   itoa(you.magic_contamination, st_prn, 10);
   //                   strcpy(info, st_prn);
   //   mpr(info);
   //break;

   case '[':
   itoa(you.x_pos, st_prn, 10);
   strcpy(info, st_prn);
   strcat(info, "/");
   itoa(you.y_pos, st_prn, 10);
   strcat(info, st_prn);
   mpr(info);
   itoa(player_res_fire(), st_prn, 10);
   strcpy(info, st_prn);
   mpr(info);
   itoa(player_res_cold(), st_prn, 10);
   strcpy(info, st_prn);
   mpr(info);
   itoa(player_res_poison(), st_prn, 10);
   strcpy(info, st_prn);
   mpr(info);
   itoa(player_prot_life(), st_prn, 10);
   strcpy(info, st_prn);
   mpr(info);
   //   acquirement();
   break;

   case ']':
   char specx [2];
   strcpy(info, "Gain which mutation? ");
   mpr(info);
   specx [0] = getche();
   specx [1] = getche();
   mutate(atoi(specx));
   break;

   case ')': you.experience += 5000; you.exp_available += 500; you.redraw_experience = 1;
   level_change();
   break;
   case '\"': level_travel(); break;
   case '-': you.hp = you.hp_max; you.redraw_hit_points = 1; break;
   case '$': you.gold += 500; you.redraw_gold = 1; break;

   case 'F':    stethoscope(250); break;

   case ':':
   int i, j;
   j = 0;
   for (i = 0; i < 20; i ++)
   {
   if (you.branch_stairs [i] == 0) continue;
   strcpy(info, "Branch ");
   itoa(i, st_prn, 10);
   strcat(info, st_prn);
   strcat(info, " is on level ");
   itoa(you.branch_stairs [i], st_prn, 10);
   strcat(info, st_prn);
   strcat(info, ".");
   mpr(info);
   }
   break;
   case '{':
   magic_mapping(99, 100);
   break;
   case '_':
   mpr("You feel more pious! Well done.");
   gain_piety(10);
   break;
   //  case '}': Xom_acts(1, 50, 1); break;
   case '|': acquirement(250); break;
 */

    case ')':
        list_weapons();
        return;

    case ']':
        list_armour();
        return;

    case '"':
        list_jewellery();
        return;

#ifdef WIZARD
    case 23:
        mpr( "Enter Wizard Command: " );
        wiz_command = getch();

        switch (wiz_command) {
        case '&':
            //cdl
            mpr("0");
            mpr("1");
            mpr("2");
            mpr("3");
            mpr("4");
            mpr("5");
            mpr("6");
            mpr("7");
            mpr("8");
            mpr("9");
            mpr("10");
            mpr("11");
            mpr("12");
            mpr("13");
            mpr("14");
            mpr("15");
            mpr("16");
            mpr("17");
            mpr("18");
            mpr("19");
            mpr("20");
            break;
        case 'x':
            you.experience += 500;
            you.exp_available += 500;
            you.redraw_experience = 1;
            level_change();
            break;
        case '$':
            you.experience += 500;
            you.hp += 50;
            you.hp_max += 50;
            you.redraw_experience = 1;
            you.redraw_hit_points = 1;
            you.gold += 50;
            you.exp_available += 500;
            level_change();
            break;
    //  case '^': shop(); //in_a_shop(-1, you.inv_quantity, you.inv_dam, you.inv_class, you.inv_type, you.inv_plus, you.inv_ident, you.equip [0], you.armour [0], you.armour [5], you.armour [2], you.armour [1], you.armour [3], you.armour [4], you.ring, it, igrid, you);
            //  break;
        case 'a':
            acquirement(250);
            break;                  //animate_dead(5, 7, you.pet_target, 1); break;

        case 'h':
            you.hp = you.hp_max;
            you.redraw_hit_points = 1;
            break;
        case '~':
            level_travel();
            break;
        case 'o':
            create_spec_object2();
            break;
        case 'm':
            create_spec_monster();
            break;
        case 'd':
            grd[you.x_pos][you.y_pos] = 82;
            break;
    //  case '#': grid [you.x_pos] [you.y_pos] = 100; break;
        case 'p':
            grd[you.x_pos][you.y_pos] = 99;
            break;
        case 'l':
            grd[you.x_pos][you.y_pos] = 81;
            break;
        case 'i':
            identify(0);
            break;
        case '_':
            j = 0;
            for (i = 0; i < 50; i++)
            {
                j += you.skill_points[i];
            }
            strcpy(info, "You have a total of ");
            itoa(j, st_prn, 10);
            strcat(info, st_prn);
            strcat(info, " skill points.");
            mpr(info);
            break;
    /*  case '=':
       new_level();
       strcpy(info, "");
       for (i = 0; i < 20; i ++)
       {
       itoa(mons_alloc [i], st_prn, 10);
       strcat(info, st_prn);
       strcat(info, ",");
       }
       mpr(info);
       break; */

        case '\'':
            for (i = 0; i < ITEMS; i++)
            {
                if (mitm.link[i] == ING)
                    continue;
                itoa(i, st_prn, 10);
                strcpy(info, st_prn);
                strcat(info, " is linked to ");
                itoa(mitm.link[i], st_prn, 10);
                strcat(info, st_prn);
                strcat(info, " c:");
                itoa(mitm.base_type[i], st_prn, 10);
                strcat(info, st_prn);
                strcat(info, " t:");
                itoa(mitm.sub_type[i], st_prn, 10);
                strcat(info, st_prn);
                strcat(info, " p:");
                itoa(mitm.pluses[i], st_prn, 10);
                strcat(info, st_prn);
                strcat(info, " p2:");
                itoa(mitm.pluses2[i], st_prn, 10);
                strcat(info, st_prn);
                strcat(info, " d:");
                itoa(mitm.special[i], st_prn, 10);
                strcat(info, st_prn);
                strcat(info, " q: ");
                itoa(mitm.quantity[i], st_prn, 10);
                strcat(info, st_prn);
                mpr(info);
            }
            strcpy(info, "igrid:");
            mpr(info);
            for (i = 0; i < GXM; i++)
            {
                for (j = 0; j < GYM; j++)
                {
                    if (env.igrid[i][j] != ING)
                    {
                        itoa(env.igrid[i][j], st_prn, 10);
                        strcpy(info, st_prn);
                        strcat(info, " at ");
                        itoa(i, st_prn, 10);
                        strcat(info, st_prn);
                        strcat(info, ", ");
                        itoa(j, st_prn, 10);
                        strcat(info, st_prn);
                        strcat(info, " c:");
                        itoa(mitm.base_type[env.igrid[i][j]], st_prn, 10);
                        strcat(info, st_prn);
                        strcat(info, " t:");
                        itoa(mitm.sub_type[env.igrid[i][j]], st_prn, 10);
                        strcat(info, st_prn);
                        strcat(info, " p:");
                        itoa(mitm.pluses[env.igrid[i][j]], st_prn, 10);
                        strcat(info, st_prn);
                        strcat(info, " p2:");
                        itoa(mitm.pluses2[env.igrid[i][j]], st_prn, 10);
                        strcat(info, st_prn);
                        strcat(info, " d:");
                        itoa(mitm.special[env.igrid[i][j]], st_prn, 10);
                        strcat(info, st_prn);
                        strcat(info, " q: ");
                        itoa(mitm.quantity[env.igrid[i][j]], st_prn, 10);
                        strcat(info, st_prn);
                        mpr(info);
                    }
                }
            }
            break;

        default:
            mpr( "Not a Wizard Command." );
            break;
        }

        break;
#endif

    case 'S':
        mpr("Save game and exit?");
        save_key = getch();
        if (save_key != 'y')
            break;
        save_game(1);
        break;

    case 'Q':
        quit_game();
        break;
    case 'v':
        version();
        break;
    case '}':
//        Can't use this char
        break;
    default:
        strcpy(info, "Unknown command.");
        mpr(info);
        break;

    }

#ifdef LINUX
    // New Unix keypad stuff
    if (running)
    {
        you.run_x = move_x;
        you.run_y = move_y;
        you.running = 2;
        move_x = 0;
        move_y = 0;
    }
    else if (opening)
    {
        open_door(move_x, move_y);
        move_x = 0;
        move_y = 0;
    }
#endif

    if (move_x != 0 || move_y != 0)
    {
        move_player(move_x, move_y);
    }

#ifdef USE_NEW_BERSERK
    else if (you.turn_is_over)
    {
        // we did something other than move/attack
        do_berserk_no_combat_penalty();
    }
#endif

    if (you.turn_is_over == 0)
    {
        viewwindow(1, false);
        return;
    }

//if (random2(10) < you.skills [SK_TRAPS_DOORS] + 2) search_around();

    stealth = check_stealth();

    if (you.special_wield != 0)
    {
        special_wielded();
    }


    if (random2(10) == 0)
    {
        if ((player_teleport() > 0 && random2(100 / player_teleport()) == 0) || (you.level_type == 2 && random() % 30 == 0))
        {
            you_teleport2(1);   // this is instantaneous

        }
    }


    if (env.cgrid[you.x_pos][you.y_pos] != CNG)
        in_a_cloud();



// paradox: it both lasts longer & does more damage overall if you're
//          moving slower.
// rationalisation: I guess it gets rubbed off/falls off/etc if you
//          move around more.

    if (you.duration[DUR_LIQUID_FLAMES] > 0)
        you.duration[DUR_LIQUID_FLAMES]--;

    if (you.duration[DUR_LIQUID_FLAMES] != 0)
    {
        mpr("You are covered in liquid flames!");
        scrolls_burn(8, 6);
/* hit_player((random2(5) + random2(5) + 1) * you.time_taken / 10, 1, 0, 17, env); */


        if (player_res_fire() > 100)
            ouch((((random2(5) + random2(5) + 1) / 2 + (player_res_fire() - 100) * (player_res_fire() - 100)) * you.time_taken) / 10, 0, 17);

        if (player_res_fire() <= 100)
            ouch(((random2(5) + random2(5) + 1) * you.time_taken) / 10, 0, 17);

        if (player_res_fire() < 100)
            ouch(((random2(5) + random2(5) + 1) * you.time_taken) / 10, 0, 17);

    }


    if (you.duration[DUR_ICY_ARMOUR] > 1)
    {
        you.duration[DUR_ICY_ARMOUR]--;
//              scrolls_burn(4, 8);
    }

    if (you.duration[DUR_ICY_ARMOUR] == 1)
    {
        mpr("Your icy armour evaporates.");
/* you.AC -= 6; */
        you.redraw_armor_class = 1;
        you.duration[DUR_ICY_ARMOUR] = 0;
    }

    if (you.duration[DUR_REPEL_MISSILES] > 0)
    {
        you.duration[DUR_REPEL_MISSILES]--;
        if (you.duration[DUR_REPEL_MISSILES] == 6)
        {
            mpr("Your repel missiles spell is about to expire...");
            you.duration[DUR_REPEL_MISSILES] -= random2(2);
        }
    }
    if (you.duration[DUR_REPEL_MISSILES] == 1)
    {
        mpr("You feel less protected from missiles.");
        you.duration[DUR_REPEL_MISSILES] = 0;
    }

    if (you.duration[DUR_DEFLECT_MISSILES] > 0)
    {
        you.duration[DUR_DEFLECT_MISSILES]--;
        if (you.duration[DUR_DEFLECT_MISSILES] == 6)
        {
            mpr("Your deflect missiles spell is about to expire...");
            you.duration[DUR_DEFLECT_MISSILES] -= random2(2);
        }
    }
    if (you.duration[DUR_DEFLECT_MISSILES] == 1)
    {
        mpr("You feel less protected from missiles.");
        you.duration[DUR_DEFLECT_MISSILES] = 0;
    }

    if (you.duration[DUR_REGENERATION] > 0)
    {
        you.duration[DUR_REGENERATION]--;
        if (you.duration[DUR_REGENERATION] == 6)
        {
            mpr("Your skin is crawling a little less now.");
            you.duration[DUR_REGENERATION] -= random2(2);
        }
        if (you.duration[DUR_REGENERATION] == 1)
        {
            mpr("Your skin stops crawling.");
            you.duration[DUR_REGENERATION] = 0;
            /* you.rate_regen -= 100; */
            // you.hunger_inc -= 4;
        }
    }

    if (you.duration[DUR_PRAYER] > 0)
        you.duration[DUR_PRAYER]--;
    if (you.duration[DUR_PRAYER] == 1)
    {
        mpr("Your prayer is over.");
        you.duration[DUR_PRAYER] = 0;
    }

    if (you.duration[DUR_VORPAL_BLADE] > 0)
        you.duration[DUR_VORPAL_BLADE]--;
    if (you.duration[DUR_VORPAL_BLADE] == 1)
    {
        item_name(you.inv_plus2[you.equip[EQ_WEAPON]], you.inv_class[you.equip[EQ_WEAPON]], you.inv_type[you.equip[EQ_WEAPON]], you.inv_dam[you.equip[EQ_WEAPON]], you.inv_plus[you.equip[EQ_WEAPON]], you.inv_quantity[you.equip[EQ_WEAPON]], you.inv_ident[you.equip[EQ_WEAPON]], 4, str_pass);
        strcpy(info, str_pass);
        strcat(info, " seems blunter.");
        mpr(info);
        you.duration[DUR_VORPAL_BLADE] = 0;
        you.inv_dam[you.equip[EQ_WEAPON]] -= 10;
        wield_change = 1;
    }

    if (you.duration[DUR_FIRE_BRAND] > 0)
        you.duration[DUR_FIRE_BRAND]--;
    if (you.duration[DUR_FIRE_BRAND] == 1)
    {
        item_name(you.inv_plus2[you.equip[EQ_WEAPON]], you.inv_class[you.equip[EQ_WEAPON]], you.inv_type[you.equip[EQ_WEAPON]], you.inv_dam[you.equip[EQ_WEAPON]], you.inv_plus[you.equip[EQ_WEAPON]], you.inv_quantity[you.equip[EQ_WEAPON]], you.inv_ident[you.equip[EQ_WEAPON]], 4, str_pass);
        strcpy(info, str_pass);
        strcat(info, " goes out.");
        mpr(info);
        you.duration[DUR_FIRE_BRAND] = 0;
        you.inv_dam[you.equip[EQ_WEAPON]] -= 1;
        wield_change = 1;
    }

    if (you.duration[DUR_ICE_BRAND] > 0)
        you.duration[DUR_ICE_BRAND]--;
    if (you.duration[DUR_ICE_BRAND] == 1)
    {
        item_name(you.inv_plus2[you.equip[EQ_WEAPON]], you.inv_class[you.equip[EQ_WEAPON]], you.inv_type[you.equip[EQ_WEAPON]], you.inv_dam[you.equip[EQ_WEAPON]], you.inv_plus[you.equip[EQ_WEAPON]], you.inv_quantity[you.equip[EQ_WEAPON]], you.inv_ident[you.equip[EQ_WEAPON]], 4, str_pass);
        strcpy(info, str_pass);
        strcat(info, " stops glowing.");
        mpr(info);
        you.duration[DUR_ICE_BRAND] = 0;
        you.inv_dam[you.equip[EQ_WEAPON]] -= 2;
        wield_change = 1;

    }


    if (you.duration[DUR_LETHAL_INFUSION] > 0)
        you.duration[DUR_LETHAL_INFUSION]--;
    if (you.duration[DUR_LETHAL_INFUSION] == 1)
    {
        item_name(you.inv_plus2[you.equip[EQ_WEAPON]], you.inv_class[you.equip[EQ_WEAPON]], you.inv_type[you.equip[EQ_WEAPON]], you.inv_dam[you.equip[EQ_WEAPON]], you.inv_plus[you.equip[EQ_WEAPON]], you.inv_quantity[you.equip[EQ_WEAPON]], you.inv_ident[you.equip[EQ_WEAPON]], 4, str_pass);
        strcpy(info, str_pass);
        strcat(info, " stops crackling.");
        mpr(info);
        you.duration[DUR_LETHAL_INFUSION] = 0;
        you.inv_dam[you.equip[EQ_WEAPON]] -= 8;
        wield_change = 1;
    }

    if (you.duration[DUR_POISON_WEAPON] > 0)
        you.duration[DUR_POISON_WEAPON]--;
    if (you.duration[DUR_POISON_WEAPON] == 1)
    {
        item_name(you.inv_plus2[you.equip[EQ_WEAPON]], you.inv_class[you.equip[EQ_WEAPON]], you.inv_type[you.equip[EQ_WEAPON]], you.inv_dam[you.equip[EQ_WEAPON]], you.inv_plus[you.equip[EQ_WEAPON]], you.inv_quantity[you.equip[EQ_WEAPON]], you.inv_ident[you.equip[EQ_WEAPON]], 4, str_pass);
        strcpy(info, str_pass);
        strcat(info, " stops dripping with poison.");
        mpr(info);
        you.duration[DUR_POISON_WEAPON] = 0;
        you.inv_dam[you.equip[EQ_WEAPON]] -= 6;
        wield_change = 1;
    }

    if (you.duration[DUR_BREATH_WEAPON] > 0)
        you.duration[DUR_BREATH_WEAPON]--;
    if (you.duration[DUR_BREATH_WEAPON] == 1)
    {
        mpr("You have got your breath back.");
        you.duration[DUR_BREATH_WEAPON] = 0;
    }

    if (you.duration[DUR_TRANSFORMATION] > 0)
    {
        you.duration[DUR_TRANSFORMATION]--;
        if (you.duration[DUR_TRANSFORMATION] == 10)
        {
            mpr("Your transformation is almost over.");
            you.duration[DUR_TRANSFORMATION] -= random2(3);
        }
    }
    if (you.duration[DUR_TRANSFORMATION] == 1)
    {
        untransform();
        you.duration[DUR_BREATH_WEAPON] = 0;
    }


    if (you.duration[DUR_SWIFTNESS] > 0)
    {
        you.duration[DUR_SWIFTNESS]--;
        if (you.duration[DUR_SWIFTNESS] == 6)
        {
            mpr("You start to feel a little slower.");
            you.duration[DUR_SWIFTNESS] -= random2(2);
        }
    }
    if (you.duration[DUR_SWIFTNESS] == 1)
    {
        mpr("You feel sluggish.");      // swiftness runs out

        you.duration[DUR_SWIFTNESS] = 0;
/* you.fast_you[0].run --; */
    }

    if (you.duration[DUR_INSULATION] > 0)
    {
        you.duration[DUR_INSULATION]--;
        if (you.duration[DUR_INSULATION] == 6)
        {
            mpr("You start to feel a little less insulated.");
            you.duration[DUR_INSULATION] -= random2(2);
        }
    }
    if (you.duration[DUR_INSULATION] == 1)
    {
        mpr("You feel conductive.");    // insulation (lightning resistance) wore off

        you.duration[DUR_INSULATION] = 0;
        // Handled in player_res_electricity() now
        // you.attribute[ATTR_RESIST_LIGHTNING]--;
    }

    if (you.duration[DUR_STONEMAIL] > 1)
    {
        you.duration[DUR_STONEMAIL]--;
        if (you.duration[DUR_STONEMAIL] == 6)
        {
            mpr("Your scaley stone armour is starting to flake away.");
            you.duration[DUR_STONEMAIL] -= random2(2);
        }
    }
    if (you.duration[DUR_STONEMAIL] == 1)
    {
        mpr("Your scaley stone armour disappears.");
/* you.AC -= 7;
   you.evasion += 2;
   you.redraw_evasion = 1;
   you.redraw_armor_class = 1; */
        you.duration[DUR_STONEMAIL] = 0;
        burden_change();
    }

    if (you.duration[DUR_TELEPORT] > 1)
        you.duration[DUR_TELEPORT]--;
    if (you.duration[DUR_TELEPORT] == 1)
    {
        you_teleport2(1);
        you.duration[DUR_TELEPORT] = 0;
    }

    if (you.duration[DUR_CONTROL_TELEPORT] > 1)
    {
        you.duration[DUR_CONTROL_TELEPORT]--;
        if (you.duration[DUR_CONTROL_TELEPORT] == 6)
        {
            mpr("You start to feel a little uncertain.");
            you.duration[DUR_CONTROL_TELEPORT] -= random2(2);
        }
    }
    if (you.duration[DUR_CONTROL_TELEPORT] == 1)
    {
        mpr("You feel uncertain.");     // teleport control

        you.duration[DUR_CONTROL_TELEPORT] = 0;
        you.attribute[ATTR_CONTROL_TELEPORT]--;
    }

    if (you.duration[DUR_RESIST_POISON] > 1)
    {
        you.duration[DUR_RESIST_POISON]--;
        if (you.duration[DUR_RESIST_POISON] == 6)
        {
            mpr("Your poison resistance is about to expire.");
            you.duration[DUR_RESIST_POISON] -= random2(2);
        }
    }
    if (you.duration[DUR_RESIST_POISON] == 1)
    {
        mpr("Your poison resistance expires.");         // poison resistance wore off

        you.duration[DUR_RESIST_POISON] = 0;
    }

    if (you.duration[DUR_DEATH_CHANNEL] > 1)
    {
        you.duration[DUR_DEATH_CHANNEL]--;
        if (you.duration[DUR_DEATH_CHANNEL] == 6)
        {
            mpr("Your unholy channel is weakening.");
            you.duration[DUR_DEATH_CHANNEL] -= random2(2);
        }
    }
    if (you.duration[DUR_DEATH_CHANNEL] == 1)
    {
        mpr("Your unholy channel expires.");    // Death channel wore off

        you.duration[DUR_DEATH_CHANNEL] = 0;
    }

    if (you.duration[DUR_TELEPORT] > 1)
        you.duration[DUR_TELEPORT]--;
    if (you.duration[DUR_TELEPORT] == 1)
    {
        you_teleport2(1);
        you.duration[DUR_TELEPORT] = 0;
    }

    if (you.invis > 1)
    {
        you.invis--;
        if (you.hunger >= 40 && you.is_undead != 2)
            you.hunger -= 5;
        if (you.invis == 6)
        {
            mpr("You flicker for a moment.");
            you.invis -= random2(2);
        }
    }
    if (you.invis == 1)
    {
        strcpy(info, "You flicker back into view.");
        mpr(info);
        you.invis = 0;
    }



    if (you.conf > 1)
        you.conf--;
    if (you.conf == 1)
    {
        strcpy(info, "You feel steadier.");
        mpr(info);
        you.conf = 0;
    }


    if (you.paralysis > 1)
        you.paralysis--;
    if (you.paralysis == 1)
    {
        strcpy(info, "You can move again.");
        mpr(info);
        you.paralysis = 0;
    }

#ifdef USE_NEW_BERSERK
    if (you.exhausted > 1)
        you.exhausted--;
    if (you.exhausted == 1)
    {
        strcpy(info, "You feel less fatigued.");
        mpr(info);
        you.exhausted = 0;
    }
#endif

    if (you.slow > 1)
        you.slow--;
    if (you.slow == 1)
    {
        strcpy(info, "You feel yourself speed up.");
        mpr(info);
        you.slow = 0;
    }

    if (you.haste > 1)
    {
        you.haste--;
        if (you.haste == 6)
        {
            mpr("Your extra speed is starting to run out.");
            you.haste -= random2(2);
        }
    }

    if (you.haste == 1)
    {
        strcpy(info, "You feel yourself slow down.");
        mpr(info);
        you.haste = 0;
    }

    if (you.might > 1)
        you.might--;
    if (you.might == 1)
    {
        strcpy(info, "You feel a little less mighty now.");
        mpr(info);
        you.might = 0;
        you.strength -= 5;
        you.max_strength -= 5;
        you.redraw_strength = 1;
    }

    if (you.berserker > 1)
        you.berserker--;
    if (you.berserker == 1)
    {
        strcpy(info, "You are no longer berserk.");
        mpr(info);
        you.berserker = 0;

#ifdef USE_NEW_BERSERK
        //
        // Sometimes berserk leaves us physically drained
        //
        if (you.berserk_penalty != NO_BERSERK_PENALTY)
        {
            if (!random2(5) || random2( you.berserk_penalty ) > 4)
            {
                mpr( "You pass out from exhaustion." );
                you.paralysis += 2 + random2(3);
            }
            else if (!random2(15 - you.berserk_penalty))
            {
                mpr( "You feel very exhausted." );
                lose_stat( STAT_STRENGTH, random2(you.strength / 5) );
            }
            else
            {
                mpr( "You are exhausted." );
            }
        }

        // this resets either from an actual penalty, or from
        // a NO_BERSERK_PENALTY
        you.berserk_penalty = 0;
        you.exhausted += 12 + random2(12) + random2(12);
        you.slow += you.exhausted;
#else

        strcpy(info, "You feel exhausted.");
        mpr(info);

        you.slow += 4 + random2(4) + random2(4);
#endif

        you.hunger -= 700;
        if (you.hunger <= 50)
            you.hunger = 50;
        calc_hp();
        you.redraw_hit_points = 1;
    }

    if (you.levitation > 1)
    {
        if (you.species != SP_KENKU || you.experience_level < 15)
            you.levitation--;
        if (you.levitation == 10)
        {
            strcpy(info, "You are starting to lose your buoyancy!");
            mpr(info);
            you.levitation -= random2(6);       // so you never know how much time you have left!

            if (you.duration[DUR_CONTROLLED_FLIGHT] > 0)
                you.duration[DUR_CONTROLLED_FLIGHT] = you.levitation;
        }
    }

    if (you.levitation == 1)
    {
        strcpy(info, "You float gracefully downwards.");
        mpr(info);
        you.levitation = 0;
        burden_change();
        you.duration[DUR_CONTROLLED_FLIGHT] = 0;
        if (grd[you.x_pos][you.y_pos] == 61 || grd[you.x_pos][you.y_pos] == 62)
            fall_into_a_pool(1, grd[you.x_pos][you.y_pos]);
    }


    if (you.rotting > 0 || (you.species == SP_GHOUL && random2(2) == 0))
    {
        if (you.species == SP_MUMMY)
            you.rotting = 0;
        else if (random2(20) <= (you.rotting - 1) || (you.species == SP_GHOUL && random2(200) == 0))
        {
            ouch(1, 0, 21);
            you.hp_max--;
            you.base_hp--;
            you.redraw_hit_points = 1;
            mpr("You feel your flesh rotting away.");
            you.rotting--;
        }
    }

    if (you.disease > 0)
    {
        you.disease--;
        if (you.species == SP_KOBOLD && you.disease > 5)
            you.disease -= 2;
        if (you.disease == 0)
        {
            strcpy(info, "You feel your health improve.");
            mpr(info);
        }
    }

    if (you.poison > 0)
    {
        if (random2(5) <= (you.poison - 1))
        {
            if (you.poison > 10 && random2(you.poison) >= 8)
            {
                ouch(random2(10) + 5, 0, 1);
                you.redraw_hit_points = 1;
                strcpy(info, "You feel extremely sick.");
                mpr(info);
            }
            else if (you.poison > 5 && random2(2) == 0)
            {
                ouch(random2(2) + 2, 0, 1);
                you.redraw_hit_points = 1;
                strcpy(info, "You feel very sick.");
                mpr(info);

            }
            else
            {
                ouch(1, 0, 1);
                you.redraw_hit_points = 1;
                strcpy(info, "You feel sick.");         // //the poison running through your veins.");

                mpr(info);
            }

            if (random2(8) == 0 || (you.hp == 1 && random2(3) == 0))
            {
                strcpy(info, "You feel a little better.");
                mpr(info);
                you.poison--;
            }

        }

    }

    if (you.deaths_door > 0)
    {
        if (you.hp > you.skills[SK_NECROMANCY] + (you.religion == GOD_KIKUBAAQUDGHA) * 13)
        {
            mpr("Your life is in your own hands once again.");
            you.paralysis += 5 + random2(5);
            you.conf += 10 + random2(10);
            you.hp_max--;
            if (you.hp > you.hp_max)
                you.hp = you.hp_max;
            you.redraw_hit_points = 1;
            you.deaths_door = 0;
        }
        else
            you.deaths_door--;

        if (you.deaths_door == 10)
        {
            mpr("Your time is quickly running out!");
            you.deaths_door -= random2(6);      // so that you never know how many turns you have left. Evil, huh?

        }

        if (you.deaths_door == 1)
        {
            mpr("Your life is in your own hands again!");
//  relay_message();
            more();
/*  you.hp = 1; */
            you.redraw_hit_points = 1;
        }

    }

    if (you.is_undead != 2)
    {
        int total_food = player_hunger_rate() + you.burden_state;

        if (total_food > 0 && you.hunger >= 40)
        {
            you.hunger -= total_food;
        }
    }
    else
        you.hunger = 6000;

    if (you.hp < you.hp_max && you.disease == 0 && you.deaths_door == 0)
        you.hit_points_regeneration += player_regen();
    if (you.magic_points < you.max_magic_points)
        you.magic_points_regeneration += 7 + you.max_magic_points / 2;

    while (you.hit_points_regeneration >= 100)
    {
        if (you.hp == you.hp_max - 1 && you.running != 0 && you.run_x == 0 && you.run_y == 0)
            you.running = 0;
        you.hp++;
        if (you.hp > you.hp_max)
            you.hp = you.hp_max;
        you.hit_points_regeneration -= 100;
        you.redraw_hit_points = 1;
    }

    while (you.magic_points_regeneration >= 100)
    {
        if (you.magic_points == you.max_magic_points - 1 && you.running != 0 && you.run_x == 0 && you.run_y == 0)
            you.running = 0;
        you.magic_points++;
        if (you.magic_points > you.max_magic_points)
            you.magic_points = you.max_magic_points;
        you.magic_points_regeneration -= 100;
        you.redraw_magic_points = 1;
    }

//losight(show, grid, you.x_pos, you.y_pos);
    viewwindow(0, true);

    monster();

    ASSERT(you.time_taken >= 0);
    ASSERT(DBL_MAX - you.elapsed_time > you.time_taken);        // make sure we don't overflow

    you.elapsed_time += you.time_taken;
    if (you.synch_time <= you.time_taken)
    {
        handle_time( 200 + (you.time_taken - you.synch_time) );
        you.synch_time = 200;
        if (random2(50) == 0)
            cull_items();
    }
    else
        you.synch_time -= you.time_taken;

    env.cloud_no = manage_clouds();     //cloud_x, cloud_y, cloud_type, cloud_decay, cloud_no, grid, time_taken, you.haste, you.slow, cgrid);

    if (you.shock_shield > 0)
        manage_shock_shield();

    /*
       If visible [0] is 1, at least one statue is visible.
       This was supposed to be more complex (with values of 2 etc), but I
       couldn't get it to work.
     */
    if (visible[0] != 0)
    {
        char wc[30];

        if (visible[1] != 0)
        {
            switch (visible[1])
            {
            case 0:
                break;
//       case 1: mpr("You feel a pleasing absence."); break;
            case 2:
                if (random2(4) == 0)
                {
                    strcpy(info, "The silver statue's eyes glow a ");
                    weird_colours(random2(200), wc);
                    strcat(info, wc);
                    strcat(info, " colour.");
                    mpr(info);
                    create_monster(summon_any_demon(random2(2)), 25, 1, you.x_pos, you.y_pos, MHITYOU, 250);
                }
                break;
//       case 3: mpr("You feel a terrible presence observing you."); break;
            }
            visible[1]--;
        }
        if (visible[2] != 0)
        {
            if (random2(3) == 0)
            {
                mpr("A hostile presence attacks your mind!");
                miscast_effect(SPTYP_DIVINATION, random2(15), random2(150), 100);
            }
            visible[2]--;
        }
        visible[0]--;
    }



    if (you.hunger <= 500)
    {
        if (you.paralysis == 0 && random2(40) == 0)
        {
            strcpy(info, "You lose consciousness!");
            mpr(info);
            you.paralysis += random2(8) + 5;
            if (you.paralysis > 13)
                you.paralysis = 13;
        }
        if (you.hunger <= 100)
        {
            strcpy(info, "You have starved to death.");
            mpr(info);
            ouch(-9999, 0, 15);
        }
    }

    if (you.hunger_state == 3 && you.hunger <= 2600)    // && you.hunger_inc > 0)

    {
        strcpy(info, "You are feeling hungry.");
        mpr(info);
        you.hunger_state = 2;
        you.redraw_hunger = 1;
    }


    if (you.hunger_state == 2 && you.hunger <= 1000)    // && you.hunger_inc > 0)

    {
        strcpy(info, "You are starving!");
        mpr(info);
        you.hunger_state = 1;
        you.redraw_hunger = 1;
    }


    if (you.hunger_state == 4 && you.hunger < 7000)     // && you.hunger_inc > 0)

    {
        you.hunger_state = 3;
        you.redraw_hunger = 1;
    }

    if (you.hunger_state == 5 && you.hunger < 11000)    // && you.hunger_inc > 0)

    {
        you.hunger_state = 4;
        you.redraw_hunger = 1;
    }



    _setcursortype(_NOCURSOR);

    viewwindow(1, false);

    _setcursortype(_NORMALCURSOR);

    if (you.paralysis > 0)
    {
        more();
    }



    if (you.level_type != 0)    /* No monsters in labyrinths */
    {
        switch (you.level_type)
        {
        case 1:
            break;              /* labyrinth */
        case 2:
            if (random2(5) == 0)
                mons_place(2500, 0, 50, 50, 1, MHITNOT, 250, 51);
            break;              /* Abyss  */
        case 3:
            if (random2(50) == 0)
                pandemonium_mons();     //mons_place(250, 0, 50, 50, 0, MHITNOT, 250, 52);

            break;              /* Pandemonium */
        }
    }
    else if (random2(240) == 0 && you.level_type != 1 && you.where_are_you != 18)
        mons_place(2500, 0, 50, 50, 1, MHITNOT, 250, you.your_level);

    return;

}


/*
   Opens doors and handles some aspects of untrapping. If move_x != 100, it
   carries a specific direction for the door to be opened (eg if you type
   ctrl - dir).
 */
void open_door(char move_x, char move_y)
{
/*move_x = door_x;
   move_y = door_y; */
//int nothing = 0;
    struct dist door_move[1];

    door_move[0].move_x = move_x;
    door_move[0].move_y = move_y;

    if (move_x != 100 && env.mgrid[you.x_pos + door_move[0].move_x][you.y_pos + door_move[0].move_y] != MNG && (menv[env.mgrid[you.x_pos + door_move[0].move_x][you.y_pos + door_move[0].move_y]].type < MLAVA0 || menv[env.mgrid[you.x_pos + door_move[0].move_x][you.y_pos + door_move[0].move_y]].number == 0))
    {
/* if (menv [env.mgrid [you.x_pos + door_move[0].move_x] [you.y_pos + door_move[0].move_y]].enchantment [2] == 6 && player_see_invis() == 0)
   {
   strcpy(info, "Something seems to be in the way.");
   mpr(info);
   you.turn_is_over = 1;
   return;
   }
   strcpy(info, "You might want to wait for the creature standing in your way to move.");
   mpr(info);
   return; */
        you_attack(mgrd[you.x_pos + move_x][you.y_pos + move_y]);
        you.turn_is_over = 1;
        return;
    }

    if (move_x != 100 && grd[you.x_pos + door_move[0].move_x][you.y_pos + door_move[0].move_y] >= 75 && grd[you.x_pos + door_move[0].move_x][you.y_pos + door_move[0].move_y] <= 77)
    {
        if (env.cgrid[you.x_pos + door_move[0].move_x][you.y_pos + door_move[0].move_y] != CNG)
        {
            mpr("You can't get to that trap right now.");
            return;
        }
        disarm_trap(door_move);
        return;
    }

    if (move_x == 100)
    {
        door_move[0].move_x = 0;
        door_move[0].move_y = 0;
        strcpy(info, "Which direction?");
        mpr(info);
        direction(0, door_move);
    }                           /* else
                                   {
                                   if (grd [you.x_pos + door_move[0].move_x] [you.y_pos + door_move[0].move_y] == 70)
                                   {
                                   close_door(door_move[0].move_x, door_move[0].move_x);
                                   return;
                                   }
                                   } */

    if (door_move[0].nothing == -1)
        return;

    if (door_move[0].move_x > 1 || door_move[0].move_y > 1 || door_move[0].move_x < -1 || door_move[0].move_y < -1)
    {
        strcpy(info, "I'm afraid your arm isn't that long.");
        mpr(info);
        return;
    }

    if (grd[you.x_pos + door_move[0].move_x][you.y_pos + door_move[0].move_y] == 3)
    {
        int  skill = you.dex + (SK_TRAPS_DOORS + SK_STEALTH) / 2;

        if (random2(skill) == 0)
        {
            strcpy(info, "As you open the door, it creaks loudly!");
            noisy(15, you.x_pos, you.y_pos);
        }
        else
        {
            if (you.levitation != 0)
            {
                strcpy(info, "You reach down and open the door.");
            }
            else
            {
                strcpy(info, "You open the door.");
            }

        }

        mpr(info);
        grd[you.x_pos + door_move[0].move_x][you.y_pos + door_move[0].move_y] = 70;
        you.turn_is_over = 1;
    }
    else
    {
        strcpy(info, "You swing at nothing.");
        mpr(info);
        you.turn_is_over = 1;
        if (you.is_undead != 2)
            you.hunger -= 3;
    }

}                               // end of void open_door()


/*
   Similar to open_door. Can you spot the difference?
 */
void close_door(char door_x, char door_y)
{
    struct dist door_move[1];

    door_move[0].move_x = door_x;
    door_move[0].move_y = door_y;

    if (door_move[0].move_x == 100)
    {
        door_move[0].move_x = 0;
        door_move[0].move_y = 0;
        strcpy(info, "Which direction?");
        mpr(info);
        direction(0, door_move);
    }

    if (door_move[0].move_x > 1 || door_move[0].move_y > 1)
    {
        strcpy(info, "I'm afraid your arm isn't that long.");
        mpr(info);
        return;
    }

    if (door_move[0].move_x == 0 && door_move[0].move_y == 0)
    {
        strcpy(info, "You can't close doors on yourself!");
        mpr(info);
        return;
    }

//if (env.eenv[0].mgrid [you.x_pos + door_move[0].move_x] [you.y_pos + door_move[0].move_y] != MNG)
    if (env.mgrid[you.x_pos + door_move[0].move_x][you.y_pos + door_move[0].move_y] != MNG)
    {
        // Need to make sure that turn_is_over = 1 if creature is invisible
        strcpy(info, "There's a creature in the doorway!");
        mpr(info);
        door_move[0].move_x = 0;
        door_move[0].move_y = 0;
        return;
    }



    if (grd[you.x_pos + door_move[0].move_x][you.y_pos + door_move[0].move_y] == 70)
    {

        if (env.igrid[you.x_pos + door_move[0].move_x][you.y_pos + door_move[0].move_y] != 501)
        {
            strcpy(info, "There's something blocking the doorway.");
            mpr(info);
            door_move[0].move_x = 0;
            door_move[0].move_y = 0;
            return;
        }

        int  skill = you.dex + (SK_TRAPS_DOORS + SK_STEALTH) / 2;

        if (random2(skill) == 0)
        {
            strcpy(info, "As you close the door, it creaks loudly!");
            noisy(15, you.x_pos, you.y_pos);
        }
        else
        {
            if (you.levitation != 0)
            {
                strcpy(info, "You reach down and close the door.");
            }
            else
            {
                strcpy(info, "You close the door.");
            }
        }

        mpr(info);
        grd[you.x_pos + door_move[0].move_x][you.y_pos + door_move[0].move_y] = 3;
        you.turn_is_over = 1;
    }
    else
    {
        strcpy(info, "There isn't anything that you can close there!");
        mpr(info);
    }

}                               // end of void open_door()




/*
   Initialises a whole lot of stuff.
 */
void initialise()
{
    int i = 0;

    your_sign = '@';
    your_colour = LIGHTGREY;

/*for (i = 0; i < 10; i ++)
   {
   func_pass [i] = 0;
   } */

    for (i = 0; i < NO_EQUIP; i++)
    {
        you.equip[NO_EQUIP] = -1;
    }
    /*for (i = 0; i < 20; i++)
       {
       mons_alloc [i] = 250;
       } */

/* system initialisation stuff */
    textbackground(0);
    you.your_level = 0;

#ifdef DOS
    directvideo = 1;
#endif

#ifdef USE_EMX
    init_emx();
#endif

    srandom(time(NULL));
    clrscr();

/* init item array */
    for (i = 1; i < ITEMS; i++)
    {
        env.it[0].base_type[i] = 0;
        env.it[0].sub_type[i] = 0;
        env.it[0].x[i] = 1;
        env.it[0].y[i] = 1;
        env.it[0].quantity[i] = 0;
        env.it[0].special[i] = 0;
        env.it[0].pluses[i] = 0;
        env.it[0].pluses2[i] = 0;
        env.it[0].link[i] = ING;
    }

    strcpy(info, "");

    int j = 0;

//env.it [0] = &it[0];
    //env.elvl [0] = &lvl[0];

    for (i = 0; i < MNST; i++)
    {
//        env.mons [i] = &mons [i];
        env.mons[i].type = -1;
        env.mons[i].speed_increment = 10;
        env.mons[i].target_x = 155;
        env.mons[i].enchantment1 = 0;
        env.mons[i].behavior = 0;       // sleeping

        env.mons[i].monster_foe = MNG;  // nothing

        for (j = 0; j < 3; j++)
        {
            env.mons[i].enchantment[j] = 0;
        }
        for (j = 0; j < 8; j++)
        {
            env.mons[i].inv[j] = ING;
        }
        env.mons[i].number = 0;
    }

    for (i = 0; i < GXM; i++)
    {
        for (j = 0; j < GYM; j++)
        {
            env.igrid[i][j] = ING;
            env.mgrid[i][j] = MNG;
            env.map[i][j] = 0;
        }
    }

    for (i = 0; i < 52; i++)
    {
        you.inv_quantity[i] = 0;
    }

/*for (i = 0; i < 50; i++)
   {
   unique_creatures [i] = 0;
   } */

    for (i = 0; i < 25; i++)
    {
        you.spells[i] = 210;
    }

    for (i = 0; i < 10; i++)
    {
        visible[i] = 0;
    }

    you.prev_targ = MHITNOT;

/*for (i = 0; i < 19; i ++)
   {
   for (j = 0; j < 19; j ++)
   {
   show [i] [j] = 0;
   show_col [i] [j] = LIGHTGRAY;
   }
   } */

/* sets up a new game */
    int newc = new_game();

    if (newc == 0)
        restore_game();

    game_has_started = true;

    calc_hp();
    calc_ep();

//if (you.species == 12) you.is_undead = 2; else you.is_undead = 0;
    /*switch(you.species)
       {
       case 10: your_sign = 'o'; break;
       case 11: your_sign = 'K'; break;
       case 12: your_sign = 'M'; break;
       default: your_sign = '@'; break;
       } */

/*if (newc == 1) stair_taken = 82; */

    you.num_inv_items = 0;
    for (i = 0; i < 52; i++)
    {
        if (you.inv_quantity[i] != 0)
            you.num_inv_items++;
    }
    char just_made_new_lev;

    if (newc == 0)
        just_made_new_lev = 1;
    else
        just_made_new_lev = 0;

    char moving_level = 0;

    if (newc == 1)
        moving_level = 1;

/*load(82, moving_level, level_saved, was_a_labyrinth, old_level, just_made_new_lev); */
//load(82, moving_level, 0, 0, 0, 0, just_made_new_lev);
    load(82, moving_level, 0, 0, 0, just_made_new_lev, you.where_are_you);

    moving_level = 0;
    just_made_new_lev = 0;
    newc = 0;

//new_level();

    mon_init(gmon_use, mcolour);        //, mcolour);
    //new_level(); // - must come after mon_init

//def_letters(letters);

/*def_properties(property, mass); */

    init_properties();
    burden_change();
    food_change();

    if (newc == 1)
    {

        for (i = 0; i < GXM; i++)
        {
            for (j = 0; j < GYM; j++)
            {
                if (grd[i][j] == 68)
                {
                    you.x_pos = i;
                    you.y_pos = j;
                }
            }
            if (grd[i][j] == 67)
                break;
        }


        // burden_change();
        // food_change();
        new_level();            // - must come after mon_init

    }                           // end if newc

    you.redraw_hit_points = 1;
    you.redraw_magic_points = 1;
    you.redraw_strength = 1;
    you.redraw_intelligence = 1;
    you.redraw_dexterity = 1;
    you.redraw_armor_class = 1;
    you.redraw_evasion = 1;
    you.redraw_experience = 1;
    you.redraw_gold = 1;
    you.redraw_hunger = 1;
    wield_change = 1;
    char title[40];

    strcpy(title, skill_title(best_skill(0, 50, 99), you.skills[best_skill(0, 50, 99)], you.char_class, you.experience_level));
    draw_border(BROWN, you.your_name, title, you.species);

    new_level();

    viewwindow(1, false); // This just puts the view up for the first turn.

    item();


}


#ifdef USE_NEW_BERSERK
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
void do_berserk_no_combat_penalty()
{
    // Butchering/eating a corpse will maintain a blood rage.
    if (you.delay_t && (you.delay_doing == 4 || you.delay_doing == 0))
        return;

    if (you.berserk_penalty == NO_BERSERK_PENALTY)
        return;

    if (you.berserker)
    {
        you.berserk_penalty++;
        switch (you.berserk_penalty)
        {
        case 2:
            mpr("You feel a strong urge to attack something.");
            break;

        case 4:
            mpr("You feel your anger fading.");
            break;

        case 6:
            mpr("Your blood rage is quickly leaving you.");
            break;
        }

        // I do these three separate, because the might and
        // haste counters can different.
        you.berserker -= you.berserk_penalty;
        if (you.berserker <= 0)
        {
            you.berserker = 1;
        }

        you.might -= you.berserk_penalty;
        if (you.might <= 0)
        {
            you.might = 1;
        }

        you.haste -= you.berserk_penalty;
        if (you.haste <= 0)
        {
            you.haste = 1;
        }
    }
}
#endif


/*
   Called when the player moves by walking/running. Also calls attack function
   and trap function etc when necessary.
 */
void move_player(char move_x, char move_y)
{
    char attacking = 0;
    char stepping = 0;

//char move_x, move_y;
    char info[200];
    int i;
    int trap_known;

    if (you.conf > 0)
    {
        if (random2(3) != 0)
        {
            move_x = random2(3) - 1;
            move_y = random2(3) - 1;
        }

        if (grd[you.x_pos + move_x][you.y_pos + move_y] < MINMOVE)
        {
            you.turn_is_over = 1;
            mpr("Ouch!");
            return;
        }

        if ((grd[you.x_pos + move_x][you.y_pos + move_y] == 61 || grd[you.x_pos + move_x][you.y_pos + move_y] == 62) && you.levitation == 0)
        {
            fall_into_a_pool(0, grd[you.x_pos + move_x][you.y_pos + move_y]);
            you.turn_is_over = 1;
#ifdef USE_NEW_BERSERK
            do_berserk_no_combat_penalty();
#endif
            return;
        }
    }                           // end of if you.conf

    if (you.running > 0 && you.running != 2 && grd[you.x_pos + move_x][you.y_pos + move_y] != 67 && grd[you.x_pos + move_x][you.y_pos + move_y] != 78)
    {
        you.running = 0;
        move_x = 0;
        move_y = 0;
        you.turn_is_over = 0;
        return;
    }

    if (mgrd[you.x_pos + move_x][you.y_pos + move_y] != MNG)
    {
        if (menv[mgrd[you.x_pos + move_x][you.y_pos + move_y]].type >= MLAVA0 && menv[mgrd[you.x_pos + move_x][you.y_pos + move_y]].number == 1)
            goto break_out;
        if (menv[mgrd[you.x_pos + move_x][you.y_pos + move_y]].behavior == 7 && menv[mgrd[you.x_pos + move_x][you.y_pos + move_y]].enchantment[2] != 6 && you.conf == 0)
        {
            swap_places(mgrd[you.x_pos + move_x][you.y_pos + move_y]);
            goto break_out;
        }
        you_attack(mgrd[you.x_pos + move_x][you.y_pos + move_y]);
        you.turn_is_over = 1;
#ifdef USE_NEW_BERSERK
        if (you.berserk_penalty != NO_BERSERK_PENALTY)
        {
            // we don't want to create a penalty if there isn't
            // supposed to be one
            you.berserk_penalty = 0;
        }
#endif
        attacking = 1;
    }

break_out:
    if ((grd[you.x_pos + move_x][you.y_pos + move_y] == 61 || grd[you.x_pos + move_x][you.y_pos + move_y] == 62) && attacking == 0 && you.levitation == 0)
    {
        mpr("Do you really want to step there?");
        stepping = get_ch();
        if (stepping == 'y' || stepping == 'Y')
        {
            fall_into_a_pool(0, grd[you.x_pos + move_x][you.y_pos + move_y]);
            you.turn_is_over = 1;
#ifdef USE_NEW_BERSERK
            do_berserk_no_combat_penalty();
#endif
            return;
        }
        mpr("Okay, then.");
        return;
    }

    if (attacking == 0 && (grd[you.x_pos + move_x][you.y_pos + move_y] >= MINMOVE))
    {
        if (grd[you.x_pos + move_x][you.y_pos + move_y] == 78 && random() % (you.skills[SK_TRAPS_DOORS] + 1) > 3)
        {
            strcpy(info, "Wait a moment, ");
            strcat(info, you.your_name);
            strcat(info, "! Do you really want to step there?");
            mpr(info);
            more();
            you.turn_is_over = 0;
            for (i = 0; i < NTRAPS; i++)
            {
                if (env.trap_x[i] == you.x_pos + move_x && env.trap_y[i] == you.y_pos + move_y)
                    break;
            }
            if (env.trap_type[i] < 4 || env.trap_type[i] == 6 || env.trap_type[i] == 7)
                grd[you.x_pos + move_x][you.y_pos + move_y] = 75;
            if (env.trap_type[i] == 4 || env.trap_type[i] == 5 || env.trap_type[i] == 8)
                grd[you.x_pos + move_x][you.y_pos + move_y] = 76;
            return;
        }
        you.x_pos += move_x;
        you.y_pos += move_y;
        if (grd[you.x_pos][you.y_pos] == 65 && you.levitation == 0)
        {
            if (random2(3) == 0)
            {
                mpr("Splash!");
                noisy(10, you.x_pos, you.y_pos);
            }
            you.time_taken *= 13 + random2(8);
            you.time_taken /= 10;
            if (grd[you.x_pos - move_x][you.y_pos - move_y] != 65)
            {
                mpr("You enter the shallow water. Moving in this stuff is going to be slow.");
                if (you.invis != 0)
                {
                    mpr("And don't expect to remain undetected.");
                }
            }
        }
        move_x = 0;
        move_y = 0;
        if (player_fast_run() != 0)
        {
            you.time_taken *= 6;
            you.time_taken /= 10;
        }
        if (you.attribute[ATTR_WALK_SLOWLY] != 0)
        {
            you.time_taken *= 14;
            you.time_taken /= 10;
        }
        you.turn_is_over = 1;
        item_check(0);

        if (grd[you.x_pos][you.y_pos] > 74 && grd[you.x_pos][you.y_pos] < 79)
        {

            if (grd[you.x_pos][you.y_pos] == 78)
            {
                //abort();
                for (i = 0; i < NTRAPS; i++)
                {
                    if (env.trap_x[i] == you.x_pos && env.trap_y[i] == you.y_pos)
                        break;
                }
                if (env.trap_type[i] < 4 || env.trap_type[i] == 6 || env.trap_type[i] == 7)
                    grd[you.x_pos][you.y_pos] = 75;
                if (env.trap_type[i] == 4 || env.trap_type[i] == 5 || env.trap_type[i] == 8)
                    grd[you.x_pos][you.y_pos] = 76;
                trap_known = 0;

                // else if (trap_type is magic etc
            }
            else
                trap_known = 1;


            for (i = 0; i < NTRAPS; i++)
            {
                if (env.trap_x[i] == you.x_pos && env.trap_y[i] == you.y_pos)
                    break;
            }

            if (you.levitation != 0 && (env.trap_type[i] < 4 || env.trap_type[i] == 6 || env.trap_type[i] == 7))
                goto out_of_traps;      /* Can fly over mechanical traps */

            handle_traps(env.trap_type[i], i, trap_known);

        }                       // end of if another grd == trap
    }


out_of_traps:
    if (grd[you.x_pos + move_x][you.y_pos + move_y] <= MINMOVE)
    {
        move_x = 0;
        move_y = 0;
        you.running = 0;
        you.turn_is_over = 0;
    }


    if (you.running == 2)
        you.running = 1;

    if (you.level_type == 2 && (you.x_pos <= 21 || you.x_pos >= 59 || you.y_pos <= 15 || you.y_pos >= 54))
    {
        env.cloud_no = area_shift();
        you.pet_target = MHITNOT;
#ifdef WIZARD
        mpr("Shifting.");
        int igly = 0;
        int ig2 = 0;

        for (igly = 0; igly < ITEMS; igly++)
        {
            if (mitm.quantity[igly] != 0)
                ig2++;
        }
        strcpy(info, "No of items present: ");
        itoa(ig2, st_prn, 10);
        strcat(info, st_prn);
        mpr(info);
        ig2 = 0;
        for (igly = 0; igly < MNST; igly++)
        {
            if (menv[igly].type != -1)
                ig2++;
        }
        strcpy(info, "No of monsters present: ");
        itoa(ig2, st_prn, 10);
        strcat(info, st_prn);
        mpr(info);
        strcpy(info, "No of clouds present: ");
        itoa(env.cloud_no, st_prn, 10);
        strcat(info, st_prn);
        mpr(info);

#endif
    }

#ifdef USE_NEW_BERSERK
    if (!attacking)
    {
        do_berserk_no_combat_penalty();
    }
#endif
}                               // end of void move_player()



/*
   TO DO:

   Stealth in check_awaken

 */

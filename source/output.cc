/*
 *  File:       output.cc
 *  Summary:    Functions used to print player related info.
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 *      <2>      5/20/99        BWR             Efficiency changes for curses.
 *      <1>      -/--/--        LRH             Created
 */

#include "AppHdr.h"
#include "output.h"

#include <stdlib.h>
#include <string.h>

#ifdef DOS
#include <conio.h>
#endif

#include "globals.h"
#include "externs.h"

#include "fight.h"
#include "itemname.h"
#include "itemprop.h"
#include "ouch.h"
#include "player.h"
#include "religion.h"
#include "skills2.h"
#include "stuff.h"

#define BORDER_COLOUR           BROWN
#define FIRST_STATUS_LINE       14
#define LAST_STATUS_LINE        17

static int bad_ench_colour( int lvl, int orange, int red )
{
    if (lvl > red)
        return (RED);
    else if (lvl > orange)
        return (LIGHTRED);

    return (YELLOW);
}

static void dur_colour( int colour, bool running_out )
{
    if (running_out)
        textcolor( colour );
    else
    {
        switch (colour)
        {
        case GREEN:     textcolor( LIGHTGREEN );        break;
        case BLUE:      textcolor( LIGHTBLUE );         break;
        case MAGENTA:   textcolor( LIGHTMAGENTA );      break;
        case LIGHTGREY: textcolor( WHITE );             break;
        }
    }
}

static inline bool check_redraw( unsigned long flag )
{
    return (you.redraw_status_flags & flag);
}

static void redraw_skill( const char your_name[kNameLen],
                          const char class_name[80] )
{
    char print_it[80];

    memset( print_it, ' ', sizeof(print_it) );
    snprintf( print_it, sizeof(print_it), "%s the %s", your_name, class_name );

    int in_len = strlen( print_it );
    if (in_len > 40)
    {
        in_len -= 3;  // what we're getting back from removing "the"

        const int name_len = strlen(your_name);
        char name_buff[kNameLen];
        strncpy( name_buff, your_name, kNameLen );

        // squeeze name if required, the "- 8" is too not squueze too much
        if (in_len > 40 && (name_len - 8) > (in_len - 40))
            name_buff[ name_len - (in_len - 40) - 1 ] = '\0';
        else
            name_buff[ kNameLen - 1 ] = '\0';

        snprintf( print_it, sizeof(print_it), "%s, %s", name_buff, class_name );
    }

    for (int i = strlen(print_it); i < 41; i++)
        print_it[i] = ' ';

    print_it[40] = '\0';

#ifdef DOS_TERM
    window(1, 1, 80, 25);
#endif
    gotoxy(40, 1);

    textcolor( YELLOW );
    cprintf( print_it );
    textcolor( LIGHTGREY );
}                               // end redraw_skill()

static inline void place_border_text( int x, int y, const char *txt )
{
    textcolor( BORDER_COLOUR );
    gotoxy( x,y );
    cprintf( txt );
}

void draw_border( void )
{
    clrscr();
    redraw_skill( you.your_name, player_title() );

    gotoxy( 40, 2 );
    textcolor( YELLOW );
    cprintf( species_name( you.species ) );

    if (you.wizard)
    {
        textcolor( LIGHTMAGENTA );
        cprintf( " *WIZARD*" );
    }

    place_border_text( 40,  3, "HP:" );
    place_border_text( 40,  4, "Magic:" );
    place_border_text( 40,  5, "AC:" );
    place_border_text( 40,  6, "EV:" );
    place_border_text( 40,  7, "Str:" );
    place_border_text( 40,  8, "Int:" );
    place_border_text( 40,  9, "Dex:" );
    place_border_text( 40, 10, "Gold:" );
    place_border_text( 40, 11, "Experience:" );
    place_border_text( 40, 12, "Level" );
}                               // end draw_border()

static void output_status_flag( const char *txt )
{
    const int x = wherex();
    const int y = wherey();
    const int len = strlen( txt );

    if (x + len + 1 > 79)
    {
        // XXX: need better way to squeeze than just return on the second
        // line... or redo the whole display to give more lines for flags.
        if (y < LAST_STATUS_LINE)
            gotoxy( 40, y + 1 );
        else
            return;
    }
    else if (x != 40)
        cprintf( " " );

    cprintf( txt );
}

// Maintaining the traditional break point in the status flags.
static void output_status_line_break( int line_num )
{
    const int x = wherex();
    const int y = wherey();

    // Wrap if we haven't already, and avoid leaving blank lines
    if (y < FIRST_STATUS_LINE + line_num && x != 40)
        gotoxy( 40, y + 1 );
}

void print_stats(void)
{
    if (!you.redraw_status_flags)
        return;

    if (check_redraw( REDRAW_SKILL ))
        redraw_skill( you.your_name, player_title() );

    if (check_redraw( REDRAW_HIT_POINTS ))
    {
        const int max_max_hp = you.hp_max + player_rotted();
        const int hp_warn = MAXIMUM( 25, Options.hp_warning );
        const int hp_attent = MAXIMUM( 10, Options.hp_attention );

        gotoxy(44, 3);

        if (you.hp <= (you.hp_max * hp_warn) / 100)
            textcolor( RED );
        else if (you.hp <= (you.hp_max * hp_attent) / 100)
            textcolor( YELLOW );
        else
            textcolor( LIGHTGREY );

        cprintf( "%d", you.hp );

        textcolor(LIGHTGREY);
        cprintf( "/%d", you.hp_max );

        if (max_max_hp != you.hp_max)
            cprintf( " (%d)", max_max_hp );

#ifdef LINUX
        clear_to_end_of_line();
#else
        cprintf("        ");
#endif
    }

    if (check_redraw( REDRAW_MAGIC_POINTS ))
    {
        gotoxy(47, 4);

        textcolor( LIGHTGREY );
        cprintf( "%d/%d", you.magic_points, you.max_magic_points );

#ifdef LINUX
        clear_to_end_of_line();
#else
        cprintf("        ");
#endif
    }

    if (check_redraw( REDRAW_STRENGTH ))
    {
        if (you.str < 0)
            you.str = 0;
        else if (you.str > 72)
            you.str = 72;

        if (you.max_str > 72)
            you.max_str = 72;

        gotoxy(45, 7);

        if (you.berserker)
            textcolor( RED );
        else if (you.might)
            textcolor( LIGHTBLUE );  // no end of effect warning
        else if (you.str < you.max_str)
            textcolor( YELLOW );

        cprintf( "%d", you.str );
        textcolor( LIGHTGREY );

        if (you.str != you.max_str)
            cprintf( " (%d)", you.max_str );

#ifdef LINUX
        clear_to_end_of_line();
#else
        cprintf("       ");
#endif

        if (you.str < 1)
            ouch( INSTANT_DEATH, 0, KILLED_BY_WEAKNESS );

        burden_change();
    }

    if (check_redraw( REDRAW_INTELLIGENCE ))
    {
        if (you.intel < 0)
            you.intel = 0;
        else if (you.intel > 72)
            you.intel = 72;

        if (you.max_intel > 72)
            you.max_intel = 72;

        gotoxy(45, 8);

        if (you.intel < you.max_intel)
            textcolor(YELLOW);

        cprintf( "%d", you.intel );
        textcolor(LIGHTGREY);

        if (you.intel != you.max_intel)
            cprintf( " (%d)", you.max_intel );

#ifdef LINUX
        clear_to_end_of_line();
#else
        cprintf("       ");
#endif

        if (you.intel < 1)
            ouch( INSTANT_DEATH, 0, KILLED_BY_STUPIDITY );
    }

    if (check_redraw( REDRAW_DEXTERITY ))
    {
        if (you.dex < 0)
            you.dex = 0;
        else if (you.dex > 72)
            you.dex = 72;

        if (you.max_dex > 72)
            you.max_dex = 72;

        gotoxy(45, 9);

        if (you.dex < you.max_dex)
            textcolor(YELLOW);

        cprintf( "%d", you.dex );
        textcolor(LIGHTGREY);

        if (you.dex != you.max_dex)
            cprintf( " (%d)", you.max_dex );

#ifdef LINUX
        clear_to_end_of_line();
#else
        cprintf("       ");
#endif

        if (you.dex < 1)
            ouch( INSTANT_DEATH, 0, KILLED_BY_CLUMSINESS );
    }

    if (check_redraw( REDRAW_ARMOUR_CLASS ))
    {
        gotoxy(44, 5);

        if (you.duration[DUR_STONEMAIL])
            dur_colour( BLUE, (you.duration[DUR_STONEMAIL] <= 6) );
        else if (you.duration[DUR_ICY_ARMOUR] || you.duration[DUR_STONESKIN])
            textcolor( LIGHTBLUE );  // no end of effect warning

        cprintf( "%d  ", player_armour_class() );
        textcolor( LIGHTGREY );

        gotoxy(50, 5);

        textcolor( BORDER_COLOUR );
        cprintf( "SH:" );

        textcolor( LIGHTGREY );
        if (you.duration[DUR_CONDENSATION_SHIELD])      //jmf: added 24mar2000
            textcolor( LIGHTBLUE );  // no end of effect warning

        cprintf( " %d ", player_shield_class() );
        textcolor( LIGHTGREY );
    }

    if (check_redraw( REDRAW_EVASION ))
    {
        gotoxy(44, 6);

        if (you.duration[DUR_FORESCRY])
            textcolor(LIGHTBLUE);  // no end of effect warning

        cprintf( "%d", player_evasion() );
        textcolor(LIGHTGREY);

        const int wpn = get_inv_wielded();
        if (wpn != -1)
        {
            const int skill = player_long_skill( you.inv[wpn] );
            const int bon = weapon_ev_bonus( you.inv[wpn], skill,
                                             player_size( PSIZE_BODY ),
                                             you.dex, true );

            if (bon)
                cprintf( " (%+d)", bon );
        }

#ifdef LINUX
        clear_to_end_of_line();
#else
        cprintf( "       " );
#endif
    }

    if (check_redraw( REDRAW_GOLD ))
    {
        gotoxy(46, 10);
        cprintf( "%d     ", you.gold );
    }

    if (check_redraw( REDRAW_EXPERIENCE ))
    {
        gotoxy(52, 11);

#if DEBUG_DIAGNOSTICS
        cprintf( "%d/%d  (%d/%d)",
                 you.xp_level, you.xp,
                 you.skill_cost_level, you.exp_available );
#else
        cprintf( "%d/%d  (%d)",
                 you.xp_level, you.xp, you.exp_available );
#endif

#ifdef LINUX
        clear_to_end_of_line();
#else
        cprintf("   ");
#endif
    }

    if (check_redraw( REDRAW_WIELD ))
    {
        gotoxy(40, 13);
#ifdef LINUX
        clear_to_end_of_line();
#else
        cprintf("                                       ");
#endif

        if (you.equip[EQ_WEAPON] != -1)
        {
            gotoxy(40, 13);

            const int wpn = you.equip[EQ_WEAPON];
            int colour = you.inv[wpn].colour;

            if (you.sure_blade)
            {
                // Need to worry about colour here, because Sure Blade
                // can be used with artefacts... hopefully we won't see
                // the "dagger of variablity"
                if (you.inv[wpn].colour == MAGENTA)
                    colour = LIGHTMAGENTA;
                else
                    colour =  MAGENTA;
            }
            else if (you.duration[DUR_WEAPON_BRAND])
            {
                // Using non-random elemental colours here since this
                // line is redrawn only as required (not every turn),
                // so random shifting won't really happen anyways (and
                // this will keep things from changing after forced
                // redraws).  It's a feature that these don't change...
                // they're supposted to be an additional visual cue
                // to the status of the enchant spell, and having the
                // colour change only happen when the spell goes down
                // is more useful for that.
                switch (you.inv[wpn].special)
                {
                case SPWPN_FLAMING:
                    colour = element_colour( EC_FIRE, true );
                    break;

                case SPWPN_FREEZING:
                    colour = element_colour( EC_ICE, true );
                    break;

                case SPWPN_VENOM:
                    colour = element_colour( EC_POISON, true );
                    break;

                case SPWPN_DRAINING:
                    colour = element_colour( EC_UNHOLY, true );
                    break;

                case SPWPN_VORPAL:
                    colour = LIGHTGREY;     // silver
                    break;

                default:
                    break;
                }
            }

            char str_pass[ ITEMNAME_SIZE ];
            in_name( wpn, DESC_INVENTORY, str_pass, Options.terse_hand );
            str_pass[39] = '\0';

            textcolor( colour );
            cprintf( str_pass );
            textcolor( LIGHTGREY );
        }
        else
        {
            gotoxy(40, 13);

            if (you.attribute[ATTR_TRANSFORMATION] == TRAN_BLADE_HANDS)
            {
                textcolor(RED);
                cprintf("Blade Hands");
                textcolor(LIGHTGREY);
            }
            else
            {
                textcolor(LIGHTGREY);
                cprintf("Nothing wielded");
            }
        }
    }


    // The colour scheme for these flags is currently:
    //
    // - yellow, "orange", red      for bad conditions
    // - light grey, white          for god based conditions
    // - green, light green         for good conditions
    // - blue, light blue           for good enchantments
    // - magenta, light magenta     for "better" enchantments (deflect, fly)
    if (check_redraw( REDRAW_STATUS ))
    {
        char str_pass[200];

        for (int y = FIRST_STATUS_LINE; y <= LAST_STATUS_LINE; y++)
        {
            gotoxy( 40, y );
#ifdef LINUX
            clear_to_end_of_line();
#else
            cprintf( "                                       " );
#endif
        }

        gotoxy( 40, FIRST_STATUS_LINE );

        switch (you.burden_state)
        {
        case BS_OVERLOADED:
            textcolor( RED );
            output_status_flag( "Overloaded" );
            break;

        case BS_ENCUMBERED:
            textcolor( LIGHTRED );
            output_status_flag( "Encumbered" );
            break;

        case BS_UNENCUMBERED:
        default:
            break;
        }

        switch (you.hunger_state)
        {
        case HS_ENGORGED:
            textcolor( LIGHTGREEN );
            output_status_flag( "Engorged" );
            break;

        case HS_FULL:
            textcolor( GREEN );
            output_status_flag( "Full" );
            break;

        case HS_SATIATED:
        default:
            break;

        case HS_HUNGRY:
            textcolor( YELLOW );
            output_status_flag( "Hungry" );
            break;

        case HS_STARVING:
            textcolor( RED );
            output_status_flag( "Starving" );
            break;
        }

#if DEBUG_DIAGNOSTICS
        // debug mode hunger-o-meter
        snprintf( str_pass, sizeof( str_pass ), "Food %d:%d",
                  you.hunger - you.old_hunger, you.hunger );

        textcolor( DARKGREY );
        output_status_flag( str_pass );
#endif

        output_status_line_break(1);

        if (you.duration[DUR_PRAYER])
        {
            textcolor( WHITE );  // no end of effect warning
            output_status_flag( "Pray" );
        }

        if (you.duration[DUR_REPEL_UNDEAD])
        {
            dur_colour( LIGHTGREY, (you.duration[DUR_REPEL_UNDEAD] <= 4) );
            output_status_flag( "Holy" );
        }

        if (you.deaths_door)
        {
            dur_colour( BLUE, (you.deaths_door < 10) );
            output_status_flag( "DDoor" );
        }

        if (you.duration[DUR_DEFLECT_MISSILES])
        {
            dur_colour( MAGENTA, (you.duration[DUR_DEFLECT_MISSILES] <= 6) );
            output_status_flag( "DMsl" );
        }
        else if (you.duration[DUR_REPEL_MISSILES])
        {
            dur_colour( BLUE, (you.duration[DUR_REPEL_MISSILES] <= 6) );
            output_status_flag( "RMsl" );
        }

        if (you.duration[DUR_REGENERATION])
        {
            dur_colour( BLUE, (you.duration[DUR_REGENERATION] <= 6) );
            output_status_flag( "Regen" );
        }

        if (you.duration[DUR_INSULATION])
        {
            dur_colour( BLUE, (you.duration[DUR_INSULATION] <= 6) );
            output_status_flag( "Ins" );
        }

        if (player_is_levitating())
        {
            bool perm = (you.species == SP_KENKU && you.xp_level >= 15)
                        || (player_equip_ego_type( EQ_BOOTS, SPARM_LEVITATION ))
                        || (you.attribute[ATTR_TRANSFORMATION] == TRAN_DRAGON);

            if (player_control_flight())
            {
                dur_colour( MAGENTA, (you.levitation <= 10 && !perm) );
                output_status_flag( "Fly" );
            }
            else
            {
                dur_colour( BLUE, (you.levitation <= 10 && !perm) );
                output_status_flag( "Lev" );
            }
        }

        if (you.invis)
        {
            dur_colour( BLUE, (you.invis <= 6) );
            output_status_flag( "Invis" );
        }

        // Perhaps this should be reversed to show when it can be used?
        // In that case, it should be probably be GREEN, and we'd have
        // to check to see if the player does have a breath weapon. -- bwr
        if (you.duration[DUR_BREATH_WEAPON])
        {
            textcolor( YELLOW );  // no warning
            output_status_flag( "BWpn" );
        }

        output_status_line_break(2);

        // Note the usage of bad_ench_colour() correspond to levels that
        // can be found in player.cc, ie those that the player can tell by
        // using the '@' command.  Things like confusion and sticky flame
        // hide their amounts and are thus always the same colour (so
        // we're not really exposing any new information). --bwr
        if (you.duration[DUR_CUT])
        {
            textcolor( RED );   // no different levels
            output_status_flag( "Cut" );
        }

        if (you.duration[DUR_STUN])
        {
            textcolor( RED );   // no different levels
            output_status_flag( "Stun" );
        }

        if (you.conf)
        {
            textcolor( RED );   // no different levels
            output_status_flag( "Conf" );
        }

        if (you.duration[DUR_LIQUID_FLAMES])
        {
            textcolor( RED );   // no different levels
            output_status_flag( "Fire" );
        }

        if (you.poison)
        {
            // We skip marking "quite" poisoned and instead mark the
            // levels where the rules for dealing poison damage change
            // significantly.  See acr.cc for that code. -- bwr
            textcolor( bad_ench_colour( you.poison, 5, 10 ) );
            output_status_flag( "Pois" );
        }

        if (you.disease)
        {
            textcolor( bad_ench_colour( you.disease, 40, 120 ) );
            output_status_flag( "Sick" );
        }

        if (you.rotting)
        {
            textcolor( bad_ench_colour( you.rotting, 4, 8 ) );
            output_status_flag( "Rot" );
        }

        if (you.magic_contamination >= 5)
        {
            textcolor( bad_ench_colour( you.magic_contamination, 15, 25 ) );
            output_status_flag( "Glow" );
        }

        if (you.duration[DUR_SWIFTNESS])
        {
            dur_colour( BLUE, (you.duration[DUR_SWIFTNESS] <= 6) );
            output_status_flag( "Swift" );
        }

        const speed_type  speed = player_net_speed_effect();
        if (speed == SPEED_SLOWED)
        {
            textcolor( RED );  // no end of effect warning
            output_status_flag( "Slow" );
        }
        else if (speed == SPEED_HASTED)
        {
            if (you.berserker)
                textcolor( RED );
            else
                dur_colour( BLUE, (you.haste <= 6) );

            output_status_flag( "Fast" );
        }

#if DEBUG_DIAGNOSTICS
        output_status_line_break(3);

        // debug mode GPS
        snprintf( str_pass, sizeof( str_pass ), "GPS (%2d,%2d); time %2d",
                  you.x_pos, you.y_pos, you.time_taken );

        textcolor( DARKGREY );
        output_status_flag( str_pass );
#endif

        textcolor( LIGHTGREY );
    }

    you.redraw_status_flags = 0;

#ifdef LINUX
    // get curses to redraw screen
    update_screen();
#endif
}                               // end print_stats()

static const char * itosym1( int stat )
{
    return ((stat >= 1) ? "+  " : ".  " );
}

static const char * itosym3( int stat )
{
    return ((stat >=  3) ? "+ + +" :
            (stat ==  2) ? "+ + ." :
            (stat ==  1) ? "+ . ." :
            (stat ==  0) ? ". . ." :
            (stat == -1) ? "x . ." :
            (stat == -2) ? "x x ."
                         : "x x x" );
}

// XXX: This had some bug problems... it still might.  At the very
// least it could be improved (the stat/hp/mp section is quite a bit
// less readable than before).
void get_full_detail( char* buffer, bool calc_unid )
{
#define FIR_AD buffer,44
#define CUR_AD &buffer[++lines*45],44
#define BUF_SIZE 25*3*45
    int lines = 0;

    memset( buffer, 0, BUF_SIZE );

    snprintf( CUR_AD, "%s the %s", you.your_name, player_title() );
    lines++;

    snprintf( CUR_AD, "Race       : %s",
              species_name(you.species,you.xp_level) );

    snprintf( CUR_AD, "Class      : %s", you.class_name );
    snprintf( CUR_AD, "Religion   : %s",
              (you.religion == GOD_NO_GOD) ? "" : god_name(you.religion) );

    snprintf( CUR_AD, "Level      : %7d", you.xp_level );
    snprintf( CUR_AD, "Exp        : %7lu", you.xp );

    if (you.xp_level < 27)
    {
        const int xp_needed = (exp_needed(you.xp_level + 2) - you.xp) + 1;

        snprintf(CUR_AD, "Next Level : %7lu", exp_needed(you.xp_level + 2) + 1);
        snprintf(CUR_AD, "Exp Needed : %7d", xp_needed);
    }
    else
    {
        snprintf(CUR_AD, "                  ");
        snprintf(CUR_AD, "                  ");
    }

    snprintf(CUR_AD, "Spls.Left  : %7d", player_spell_levels() );
    snprintf(CUR_AD, "Gold       : %7d", you.gold );

    lines++;

    if (!player_rotted())
    {
        if (you.hp < you.hp_max)
            snprintf(CUR_AD, "HP         : %3d/%d", you.hp, you.hp_max);
        else
            snprintf(CUR_AD, "HP         : %3d", you.hp);
    }
    else
    {
        snprintf(CUR_AD,  "HP         : %3d/%d (%d)",
                 you.hp, you.hp_max, you.hp_max + player_rotted() );
    }

    if (you.magic_points < you.max_magic_points)
    {
        snprintf(CUR_AD, "MP         : %3d/%d", you.magic_points, you.max_magic_points);
    }
    else
    {
        snprintf(CUR_AD, "MP         : %3d", you.magic_points);
    }

    if (you.str == you.max_str)
        snprintf(CUR_AD, "Str        : %3d", you.str);
    else
        snprintf(CUR_AD, "Str        : %3d (%d)", you.str, you.max_str);

    if (you.intel == you.max_intel)
        snprintf(CUR_AD, "Int        : %3d", you.intel);
    else
        snprintf(CUR_AD, "Int        : %3d (%d)", you.intel, you.max_intel);

    if (you.dex == you.max_dex)
        snprintf(CUR_AD, "Dex        : %3d", you.dex);
    else
        snprintf(CUR_AD, "Dex        : %3d (%d)", you.dex, you.max_dex);

    snprintf( CUR_AD, "AC         : %3d", player_armour_class() );

    const int wpn = get_inv_wielded();
    bool need_evasion = true;
    if (wpn != -1)
    {
        const int skill = player_long_skill( you.inv[wpn] );
        const int bon = weapon_ev_bonus( you.inv[wpn], skill,
                                         player_size( PSIZE_BODY ),
                                         you.dex, true );

        if (bon)
        {
            need_evasion = false;
            snprintf( CUR_AD, "Evasion    : %3d (%d)", player_evasion(), bon );
        }
    }

    if (need_evasion)
        snprintf( CUR_AD, "Evasion    : %3d", player_evasion() );

    snprintf( CUR_AD, "Shield     : %3d", player_shield_class() );
    lines++;

    if (you.real_time != -1)
    {
        const time_t curr = you.real_time + (time(NULL) - you.start_time);
        char buff[200];
        make_time_string( curr, buff, sizeof(buff), true );

        snprintf(CUR_AD, "Play time  : %10s", buff);
        snprintf(CUR_AD, "Turns      : %10ld", you.num_turns );
    }

    lines = 27;

    snprintf(CUR_AD, "Res.Fire  : %s",
            itosym3( player_res_fire(!calc_unid)) );
    snprintf(CUR_AD, "Res.Cold  : %s",
            itosym3( player_res_cold(!calc_unid) ) );
    snprintf(CUR_AD, "Life Prot.: %s",
            itosym3( player_prot_life(!calc_unid) ) );
    snprintf(CUR_AD, "Res.Poison: %s",
            itosym3( player_res_poison(!calc_unid)) );
    snprintf(CUR_AD, "Res.Elec. : %s",
            itosym1( player_res_electricity(!calc_unid) ) );
    lines++;

    snprintf(CUR_AD, "Sust.Abil.: %s",
            itosym1( player_sust_abil(!calc_unid) ) );
    snprintf(CUR_AD, "Res.Mut.  : %s",
            itosym1( player_equip( EQ_AMULET, AMU_RESIST_MUTATION, !calc_unid ) ) );
    snprintf(CUR_AD, "Res.Slow  : %s",
            itosym1( player_equip( EQ_AMULET, AMU_RESIST_SLOW, !calc_unid ) ) );
    snprintf(CUR_AD, "Clarity   : %s",
            itosym3( player_mental_clarity( !calc_unid ) ));
    lines++;
    lines++;

    char str_pass[ITEMNAME_SIZE];

    // TODO: reduce duplication of these labels in the code.
    const char *e_items[] =
    {
        "Weapon ", "Armour ", "Shield ", "Helmet ", "Cloak  ",
        "Gloves ", "Boots  ", "Amulet ", "Ring   ", "Ring   "
    };

    // TODO: this order is also inconsistant with the list armour
    // command... standardize!
    const int e_order[] =
    {
        EQ_WEAPON, EQ_BODY_ARMOUR, EQ_SHIELD, EQ_HELMET, EQ_CLOAK,
        EQ_GLOVES, EQ_BOOTS, EQ_AMULET, EQ_RIGHT_RING, EQ_LEFT_RING
    };

    for (int i = 0; i < NUM_EQUIP; i++)
    {
        if (you.equip[ e_order[i] ] != -1)
        {
            in_name( you.equip[ e_order[i] ], DESC_PLAIN,
                     str_pass, Options.terse_hand );

            if (e_order[i] == EQ_BOOTS
                && (you.species == SP_CENTAUR || you.species == SP_NAGA))
            {
                snprintf(CUR_AD, "Cover  : %s", str_pass);
            }
            else
            {
                snprintf(CUR_AD, "%s: %s", e_items[i], str_pass);
            }
        }
        else
        {
            if (e_order[i] == EQ_WEAPON)
            {
                if (you.attribute[ATTR_TRANSFORMATION] == TRAN_BLADE_HANDS)
                    snprintf(CUR_AD, "%s: Blade Hands", e_items[i]);
                else if (you.skills[SK_UNARMED_COMBAT])
                    snprintf(CUR_AD, "%s: Unarmed", e_items[i]);
                else
                    snprintf(CUR_AD, "%s:", e_items[i]);
            }
            else
            {
                snprintf(CUR_AD, "%s:", e_items[i]);
            }
        }
    }

    lines = 52;
    snprintf(CUR_AD, "See Invis. : %s",
            itosym1( player_see_invis(!calc_unid) ) );
    snprintf(CUR_AD, "Warding    : %s",
            itosym1( player_has_warding( !calc_unid ) ));
    snprintf(CUR_AD, "Conserve   : %s",
            itosym1( player_item_conserve( !calc_unid ) ));
    snprintf(CUR_AD, "Res.Corr.  : %s",
            itosym1( player_res_corrosion( !calc_unid ) ));
    snprintf(CUR_AD, "Gourmand   : %s",
            itosym1( player_equip( EQ_AMULET, AMU_THE_GOURMAND, !calc_unid) ));

    lines++;

    if (scan_randarts( RAP_PREVENT_TELEPORTATION, !calc_unid ))
    {
        snprintf(CUR_AD, "Prev.Telep.: %s",
            itosym1( scan_randarts(RAP_PREVENT_TELEPORTATION, !calc_unid) ) );
    }
    else
    {
        snprintf(CUR_AD, "Rnd.Telep. : %s",
            itosym1( player_teleport(!calc_unid) ) );
    }

    snprintf(CUR_AD, "Ctrl.Telep.: %s",
            itosym1( player_teleport_control(!calc_unid) ) );
    snprintf(CUR_AD, "Levitation : %s",
            itosym1( player_is_levitating() ) );
    snprintf(CUR_AD, "Ctrl.Flight: %s",
            itosym1( player_control_flight(!calc_unid)) );

    lines++;

    return;
#undef FIR_AD
#undef CUR_AD
#undef BUF_SIZE
}

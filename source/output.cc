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

#ifdef DOS
#include <conio.h>
#endif

#include "externs.h"

#include "itemname.h"
#include "ouch.h"
#include "player.h"

void print_stats(void)
{
    textcolor(LIGHTGREY);

    char temp_quant[15];

    if (you.redraw_hit_points)
    {
        int max_max_hp = you.hp_max + player_rotted();

        if (Options.hp_warning
            && (you.hp <= (you.hp_max * Options.hp_warning) / 100))
        {
            textcolor(RED);
        }
        else if (Options.hp_attention
            && (you.hp <= (you.hp_max * Options.hp_attention) / 100))
        {
            textcolor(YELLOW);
        }

        itoa(you.hp, st_prn, 10);
        gotoxy(44, 3);
        cprintf(st_prn);

        textcolor(LIGHTGREY);
        itoa(you.hp_max, st_prn, 10);

        cprintf("/");
        cprintf(st_prn);

        if (max_max_hp != you.hp_max)
        {
            cprintf(" (");
            itoa(max_max_hp, st_prn, 10);
            cprintf(st_prn);
            cprintf(")");
        }

#ifdef LINUX
        clear_to_end_of_line();
#else
        cprintf("        ");
#endif

        you.redraw_hit_points = 0;
    }

    if (you.redraw_magic_points)
    {
        itoa(you.magic_points, st_prn, 10);
        gotoxy(47, 4);
        cprintf(st_prn);
        itoa(you.max_magic_points, st_prn, 10);
        cprintf("/");
        cprintf(st_prn);

#ifdef LINUX
        clear_to_end_of_line();
#else
        cprintf("        ");
#endif

        you.redraw_magic_points = 0;
    }

    if (you.redraw_strength)
    {
        if (you.strength < 0)
            you.strength = 0;
        else if (you.strength > 72)
            you.strength = 72;

        if (you.max_strength > 72)
            you.max_strength = 72;

        itoa(you.strength, st_prn, 10);
        gotoxy(45, 7);

        if (you.strength < you.max_strength)
            textcolor(YELLOW);

        cprintf(st_prn);

        if (you.strength != you.max_strength)
        {
            textcolor(LIGHTGREY);
            itoa(you.max_strength, st_prn, 10);
            cprintf(" (");
            cprintf(st_prn);
            cprintf(")   ");
        }
        else
        {
            cprintf("       ");
        }

        you.redraw_strength = 0;

        if (you.strength < 1)
            ouch(-9999, 0, KILLED_BY_WEAKNESS);

        burden_change();
    }

    if (you.redraw_intelligence)
    {
        if (you.intel < 0)
            you.intel = 0;
        else if (you.intel > 72)
            you.intel = 72;

        if (you.max_intel > 72)
            you.max_intel = 72;

        itoa(you.intel, st_prn, 10);
        gotoxy(45, 8);

        if (you.intel < you.max_intel)
            textcolor(YELLOW);

        cprintf(st_prn);

        if (you.intel != you.max_intel)
        {
            textcolor(LIGHTGREY);
            itoa(you.max_intel, st_prn, 10);
            cprintf(" (");
            cprintf(st_prn);
            cprintf(")   ");
        }
        else
        {
            cprintf("       ");
        }

        you.redraw_intelligence = 0;

        if (you.intel < 1)
            ouch(-9999, 0, KILLED_BY_STUPIDITY);
    }

    if (you.redraw_dexterity)
    {
        if (you.dex < 0)
            you.dex = 0;
        else if (you.dex > 72)
            you.dex = 72;

        if (you.max_dex > 72)
            you.max_dex = 72;

        itoa(you.dex, st_prn, 10);
        gotoxy(45, 9);

        if (you.dex < you.max_dex)
            textcolor(YELLOW);

        cprintf(st_prn);

        if (you.dex != you.max_dex)
        {
            textcolor(LIGHTGREY);
            itoa(you.max_dex, st_prn, 10);
            cprintf(" (");
            cprintf(st_prn);
            cprintf(")   ");
        }
        else
        {
            cprintf("       ");
        }

        you.redraw_dexterity = 0;

        if (you.dex < 1)
            ouch(-9999, 0, KILLED_BY_CLUMSINESS);

        textcolor(LIGHTGREY);
    }

    if (you.redraw_armour_class)
    {
        itoa(player_AC(), st_prn, 10);
        gotoxy(44, 5);
        cprintf(st_prn);

        if (strlen(st_prn) <= 1)
            cprintf(" ");
        if (strlen(st_prn) <= 2)
            cprintf(" ");
        if (strlen(st_prn) <= 3)
            cprintf(" ");

        if (you.duration[DUR_CONDENSATION_SHIELD])      //jmf: added 24mar2000
            textcolor(LIGHTBLUE);

        cprintf("(");
        itoa(player_shield_class(), st_prn, 10);
        cprintf(st_prn);
        cprintf(")   ");

        if (you.duration[DUR_CONDENSATION_SHIELD])      //jmf: added 24mar2000
            textcolor(LIGHTGREY);

        you.redraw_armour_class = 0;
    }

    if (you.redraw_evasion)
    {
        itoa(player_evasion(), st_prn, 10);
        gotoxy(44, 6);
        cprintf(st_prn);
        cprintf("  ");

        you.redraw_evasion = 0;
    }

    if (you.redraw_gold)
    {
        itoa(you.gold, st_prn, 10);
        gotoxy(46, 10);
        cprintf(st_prn);
        cprintf("    ");

        you.redraw_gold = 0;
    }

    if (you.redraw_experience)
    {
        itoa(you.experience_level, temp_quant, 10);
        itoa(you.experience, st_prn, 10);
        gotoxy(52, 11);
        cprintf(temp_quant);
        cprintf("/");
        cprintf(st_prn);

        cprintf("  (");
        itoa(you.exp_available, st_prn, 10);
        cprintf(st_prn);
        cprintf(")  ");

        you.redraw_experience = 0;
    }

    if (you.redraw_hunger)
    {
        gotoxy(40, 14);

        switch (you.hunger_state)
        {
        case HS_ENGORGED:
            textcolor(BLUE);
            cprintf("Engorged");
            textcolor(LIGHTGREY);
            break;

        case HS_FULL:
            textcolor(GREEN);
            cprintf("Full    ");
            textcolor(LIGHTGREY);
            break;

        case HS_SATIATED:
#ifdef LINUX
            clear_to_end_of_line();
#else
            cprintf("        ");
#endif
            break;

        case HS_HUNGRY:
            textcolor(YELLOW);
            cprintf("Hungry  ");
            textcolor(LIGHTGREY);
            break;

        case HS_STARVING:
            textcolor(RED);
            cprintf("Starving");
            textcolor(LIGHTGREY);
            break;
        }

#if DEBUG_DIAGNOSTICS
        // debug mode hunger-o-meter
        cprintf( " (%d:%d) ", you.hunger - you.old_hunger, you.hunger );
#endif

        you.redraw_hunger = 0;
    }

    if (you.redraw_burden)
    {
        gotoxy(40, 15);

        switch (you.burden_state)
        {
        case BS_OVERLOADED:
            textcolor(YELLOW);
            cprintf("Overloaded");
            textcolor(LIGHTGREY);
            break;

        case BS_ENCUMBERED:
            textcolor(LIGHTRED);
            cprintf("Encumbered");
            textcolor(LIGHTGREY);
            break;

        case BS_UNENCUMBERED:
#ifdef LINUX
            clear_to_end_of_line();
#else
            cprintf("          ");
#endif
            break;
        }

#if DEBUG_DIAGNOSTICS
        // debug mode burden-o-meter
        cprintf( " (%d/%d) ", you.burden, carrying_capacity() );
#endif

        you.redraw_burden = 0;
    }

    if (you.wield_change)
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
            textcolor(you.inv[you.equip[EQ_WEAPON]].colour);

            in_name( you.equip[EQ_WEAPON], DESC_INVENTORY, str_pass );
            str_pass[35] = '\0';

            cprintf(str_pass);
            textcolor(LIGHTGREY);
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
        you.wield_change = false;
    }

#if DEBUG_DIAGNOSTICS

    // debug mode GPS
    gotoxy(40, 16);
    cprintf( "Position(%2d,%2d)", you.x_pos, you.y_pos );

#endif


#ifdef LINUX
    // get curses to redraw screen
    update_screen();
#endif
}                               // end print_stats()

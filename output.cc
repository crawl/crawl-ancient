/*
 *  File:       output.cc
 *  Summary:    Functions used to print player related info.
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 *      <2>      5/20/99        BWR             Efficience changes for curses.
 *      <1>      -/--/--        LRH             Created
 */

#include "AppHdr.h"
#include "output.h"

#ifdef DOS
#include <conio.h>
#endif

#include <stdlib.h>

#include "externs.h"
#include "enum.h"
#include "itemname.h"
#include "player.h"
#include "ouch.h"

#ifdef USE_CURSES
#include <curses.h>
#endif

char wield_change;


void print_stats()
{
    textcolor(7);

    char temp_quant[15];

    if (you.redraw_hit_points == 1)
    {
        if (you.hp <= you.hp_max / 4)
            textcolor(YELLOW);
        if (you.hp <= you.hp_max / 10)
            textcolor(RED);
        itoa(you.hp, st_prn, 10);
        gotoxy(44, 3);
        cprintf(st_prn);
        textcolor(LIGHTGREY);
        itoa(you.hp_max, st_prn, 10);
        cprintf("(");
        cprintf(st_prn);
        cprintf(")    ");
        you.redraw_hit_points = 0;
    }

    if (you.redraw_magic_points == 1)
    {
        itoa(you.magic_points, st_prn, 10);
        gotoxy(47, 4);
        cprintf(st_prn);
        itoa(you.max_magic_points, st_prn, 10);
        cprintf("(");
        cprintf(st_prn);
        cprintf(")   ");
        you.redraw_magic_points = 0;
    }

    if (you.redraw_strength == 1)
    {
        if (you.strength <= 0)
            you.strength = 0;
        if (you.strength >= 72)
            you.strength = 72;
        if (you.max_strength >= 72)
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
            cprintf("(");
            cprintf(st_prn);
            cprintf(")   ");
        }
        else
            cprintf("      ");
        you.redraw_strength = 0;
        if (you.strength <= 0)
            ouch(-9999, 0, 8);
        burden_change();
    }

    if (you.redraw_intelligence == 1)
    {
        if (you.intel <= 0)
            you.intel = 0;
        if (you.intel >= 72)
            you.intel = 72;
        if (you.max_intel >= 72)
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
            cprintf("(");
            cprintf(st_prn);
            cprintf(")   ");
        }
        else
            cprintf("      ");
        you.redraw_intelligence = 0;
        if (you.intel <= 0)
            ouch(-9999, 0, 7);
    }

    if (you.redraw_dexterity == 1)
    {
        if (you.dex <= 0)
            you.dex = 0;
        if (you.dex >= 72)
            you.dex = 72;
        if (you.max_dex >= 72)
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
            cprintf("(");
            cprintf(st_prn);
            cprintf(")   ");
        }
        else
            cprintf("      ");
        you.redraw_dexterity = 0;
        if (you.dex <= 0)
            ouch(-9999, 0, 9);
        textcolor(LIGHTGREY);
    }

    if (you.redraw_armor_class == 1)
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
        cprintf("(");
        itoa(player_shield_class(), st_prn, 10);
        cprintf(st_prn);
        cprintf(")   ");
        you.redraw_armor_class = 0;
    }

    if (you.redraw_evasion == 1)
    {
        itoa(player_evasion(), st_prn, 10);
        gotoxy(44, 6);
        cprintf(st_prn);
        cprintf("  ");
        you.redraw_evasion = 0;
    }

    if (you.redraw_gold == 1)
    {
        itoa(you.gold, st_prn, 10);
        gotoxy(46, 10);
        cprintf(st_prn);
        cprintf("    ");
        you.redraw_gold = 0;
    }

    if (you.redraw_experience == 1)
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

    if (you.redraw_hunger == 1)
    {
        gotoxy(40, 14);
        switch (you.hunger_state)
        {
        case 5:
            textcolor(BLUE);
            cprintf("Engorged");
            textcolor(LIGHTGREY);
            break;
        case 4:
            textcolor(GREEN);
            cprintf("Full    ");
            textcolor(LIGHTGREY);
            break;
        case 3:
#ifdef USE_CURSES
            clrtoeol();
#else
            cprintf("        ");
#endif
            break;

        case 2:
            textcolor(YELLOW);
            cprintf("Hungry  ");
            textcolor(LIGHTGREY);
            break;
        case 1:
            textcolor(RED);
            cprintf("Starving");
            textcolor(LIGHTGREY);
            break;
        }
        you.redraw_hunger = 0;
    }

    if (you.redraw_burden == 1)
    {
        gotoxy(40, 15);
        switch (you.burden_state)
        {
        case 5:
            textcolor(YELLOW);
            cprintf("Overloaded");
            textcolor(LIGHTGREY);
            break;
        case 2:
            textcolor(LIGHTRED);
            cprintf("Encumbered");
            textcolor(LIGHTGREY);
            break;
        case 0:
#ifdef USE_CURSES
            clrtoeol();
#else
            cprintf("          ");
#endif
            break;
        }
        you.redraw_burden = 0;
    }
    if (wield_change == 1)
    {
        gotoxy(40, 13);

#ifdef USE_CURSES
        clrtoeol();
#else
        cprintf("                                       ");
#endif

        if (you.equip[EQ_WEAPON] != -1)
        {
            gotoxy(40, 13);
            textcolor(you.inv_colour[you.equip[EQ_WEAPON]]);
            if (you.equip[EQ_WEAPON] <= 25)
                putch(you.equip[EQ_WEAPON] + 97);
            else
                putch(you.equip[EQ_WEAPON] + 39);
            cprintf(" - ");
            in_name(you.equip[EQ_WEAPON], 3, str_pass);
            str_pass[35] = 0;
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
                cprintf("Empty hands");
            }
        }
        wield_change = 0;
    }

}

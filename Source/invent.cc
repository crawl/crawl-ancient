/*
 *  File:       invent.cc
 *  Summary:    Functions for inventory related commands.
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 *      <5>     10/9/99     BCR     Added wizard help screen
 *      <4>     10/1/99     BCR     Clarified help screen
 *      <3>     6/9/99      DML     Autopickup
 *      <2>     5/20/99     BWR     Extended screen lines support
 *      <1>     -/--/--     LRH     Created
 */

#include "AppHdr.h"
#include "invent.h"

#include <stdlib.h>
#include <string.h>

#ifdef DOS
#include <conio.h>
#endif

#include "externs.h"

#include "itemname.h"
#include "shopping.h"
#include "stuff.h"

#ifdef MACROS
#include "macro.h"
#endif

//jmf: brent sez:
//  There's a reason curses is included after the *.h files in beam.cc.
//  There's a reason curses is included after the *.h files in beam.cc.
//  There's a reason curses is included after the *.h files in beam.cc.
//  There's a reason ...
#ifdef USE_CURSES
#include <curses.h>
#endif

void command_string(char comm[50], int i);
void wizard_string(char comm[50], int i);

unsigned char get_invent(int invent_type)
{
    char nothing = invent(invent_type, false);

    redraw_screen();

    return nothing;
}                               // end get_invent()

char invent(int item_class_inv, bool show_price)
{
    char st_pass[60];

    int i, j;
    char lines = 0;
    unsigned char anything = 0;
    char strng[10] = "";
    char ft;

    char yps = 0;

    char temp_id[4][50];

    for (i = 0; i < 4; i++)
    {
        for (j = 0; j < 50; j++)
        {
            temp_id[i][j] = 1;
        }
    }

#ifdef DOS_TERM
    char buffer[4600];

    gettext(1, 1, 80, 25, buffer);
    window(1, 1, 80, 25);
#endif

    strcpy(st_pass, "");
    clrscr();

    int Inv_class2[15];
    int inv_count = 0;
    unsigned char ki = 0;

    for (i = 0; i < 15; i++)
        Inv_class2[i] = 0;

    //if (num_inv_items != 0)
    //{

    //strcpy(info [info_lines], " You are carrying"); info_lines++;

    for (i = 0; i < ENDOFPACK; i++)
    {
        if (you.inv_quantity[i])
        {
            Inv_class2[you.inv_class[i]]++;
            inv_count++;
        }
    }

    if (!inv_count)
    {
        cprintf("You aren't carrying anything.");

        if (getch() == 0)
            getch();

        goto putty;
    }

    if (item_class_inv != -1)
    {
        for (i = 0; i < 15; i++)
        {
            if (item_class_inv == OBJ_MISSILES && i == 0)
                i++;

            if (item_class_inv == OBJ_WEAPONS && i == 11)
                i++;

            if (item_class_inv == OBJ_SCROLLS && i == 10)
                i++;

            if (item_class_inv != i)
                Inv_class2[i] = 0;
        }
    }

    if ((item_class_inv == -1 && inv_count > 0)
        || (item_class_inv != -1 && Inv_class2[item_class_inv] > 0)
        || (item_class_inv == OBJ_MISSILES
            && (Inv_class2[0] > 0 || Inv_class2[1] > 0))
        || (item_class_inv == OBJ_WEAPONS
            && (Inv_class2[0] > 0 || Inv_class2[11] > 0))
        || (item_class_inv == OBJ_WEAPONS
            && (Inv_class2[0] > 0 || Inv_class2[13] > 0))
        || (item_class_inv == OBJ_SCROLLS
            && (Inv_class2[6] > 0 || Inv_class2[10] > 0)))
            // || (item_class_inv == OBJ_WANDS && (Inv_class2[3] > 0 || Inv_class2[11] > 0)))
    {
        cprintf("  Inventory");
        lines++;

        for (i = 0; i < 15; i++)
        {
            if (Inv_class2[i] != 0)
            {
                if (lines > NUMBER_OF_LINES - 3)
                {
                    gotoxy(1, NUMBER_OF_LINES);
                    cprintf("-more-");

                    ki = getch();

                    if (ki == 27)
                    {
#ifdef DOS_TERM
                        puttext(1, 1, 80, 25, buffer);
#endif
                        return 27;
                    }

                    if (ki >= 'A' && ki <= 'z')
                    {
#ifdef DOS_TERM
                        puttext(1, 1, 80, 25, buffer);
#endif
                        return ki;
                    }

                    if (ki == 0)
                        ki = getch();

                    lines = 0;
                    clrscr();
                    gotoxy(1, 1);
                    anything = 0;

                }

                if (lines > 0)
                    cprintf(EOL " ");

                textcolor(BLUE);

                switch (i)
                {
                case OBJ_WEAPONS:    cprintf("Hand weapons");    break;
                case OBJ_MISSILES:   cprintf("Missiles");        break;
                case OBJ_ARMOUR:     cprintf("Armour");          break;
                case OBJ_WANDS:      cprintf("Magical devices"); break;
                case OBJ_FOOD:       cprintf("Comestibles");     break;
                case OBJ_UNKNOWN_I:  cprintf("Books");           break;
                case OBJ_SCROLLS:    cprintf("Scrolls");         break;
                case OBJ_JEWELLERY:  cprintf("Jewellery");       break;
                case OBJ_POTIONS:    cprintf("Potions");         break;
                case OBJ_UNKNOWN_II: cprintf("Gems");            break;
                case OBJ_BOOKS:      cprintf("Books");           break;
                case OBJ_STAVES:     cprintf("Magical staves");  break;
                case OBJ_ORBS:       cprintf("Orbs of Power");   break;
                case OBJ_MISCELLANY: cprintf("Miscellaneous");   break;
                case OBJ_CORPSES:    cprintf("Carrion");         break;
                //case OBJ_GEMSTONES: cprintf("Miscellaneous"); break;
                }

                textcolor(LIGHTGREY);
                lines++;

                for (j = 0; j < ENDOFPACK; j++)
                {
                    if (lines > NUMBER_OF_LINES - 2 && inv_count > 0)
                    {
                        gotoxy(1, NUMBER_OF_LINES);
                        cprintf("-more-");
                        ki = getch();

                        if (ki == 27)
                        {
#ifdef DOS_TERM
                            puttext(1, 1, 80, 25, buffer);
#endif
                            return 27;
                        }

                        if (ki >= 'A' && ki <= 'z')
                        {
#ifdef DOS_TERM
                            puttext(1, 1, 80, 25, buffer);
#endif
                            return ki;
                        }

                        if (ki == 0)
                            ki = getch();

                        lines = 0;
                        clrscr();
                        gotoxy(1, 1);
                        anything = 0;
                    }

                    if (you.inv_class[j] == i && you.inv_quantity[j] > 0)
                    {
                        anything++;

                        //strcpy(info [info_lines], " ");
                        //strncpy(info [info_lines], letters [j], 1);
                        //strcat(info [info_lines], " - ");
                        //printf(" ");

                        if (lines > 0)
                            cprintf(EOL);

                        lines++;
                        cprintf(" ");

                        ft = index_to_letter(j);

                        //itoa(j, strng, 10);
                        strng[0] = ft;
                        strng[1] = '\0';
                        //strng [0] += 49;
                        cprintf(" ");
                        cprintf(strng);
                        //cout << " " << ft;
                        //cprintf(" ");
                        //cprintf(ft);
                        cprintf(" - ");

                        yps = wherey();

                        in_name(j, 3, st_pass);
                        cprintf(st_pass);

                        inv_count--;

                        if (j == you.equip[EQ_WEAPON])
                            cprintf(" (weapon)");

                        if (j == you.equip[EQ_CLOAK]
                            || j == you.equip[EQ_HELMET]
                            || j == you.equip[EQ_GLOVES]
                            || j == you.equip[EQ_BOOTS]
                            || j == you.equip[EQ_SHIELD]
                            || j == you.equip[EQ_BODY_ARMOUR])
                        {
                            cprintf(" (worn)");
                        }

                        if (j == you.equip[EQ_LEFT_RING])
                            cprintf(" (left hand)");
                        if (j == you.equip[EQ_RIGHT_RING])
                            cprintf(" (right hand)");
                        if (j == you.equip[EQ_AMULET])
                            cprintf(" (around neck)");

                        if (show_price)
                        {
                            cprintf(" (");
                            itoa(item_value
                                 (you.inv_class[j], you.inv_type[j],
                                  you.inv_dam[j], you.inv_plus[j],
                                  you.inv_plus2[j], you.inv_quantity[j], 3,
                                  temp_id), strng, 10);
                            cprintf(strng);
                            cprintf("gold)");
                        }

                        if (wherey() != yps)
                            lines++;
                        //info_lines++;
                    }
                }               // end of j loop
            }                   // end of if inv_class2
        }                       // end of i loop.
    }
    else
    {
        if (item_class_inv == -1)
            cprintf("You aren't carrying anything.");
        else
        {
            if (item_class_inv == OBJ_WEAPONS)
                cprintf("You aren't carrying any weapons.");
            else
            {
                if (item_class_inv == OBJ_MISSILES)
                    cprintf("You aren't carrying any ammunition.");
                else
                    cprintf("You aren't carrying any such object.");
            }
            anything++;
        }
        //info_lines ++;
    }
    //} // end of if (num_inv_items)
    //more();

    //} // end of inventory()

    if (anything > 0)
    {
        ki = getch();

        if (ki >= 'A' && ki <= 'z')
        {
#ifdef DOS_TERM
            puttext(1, 1, 80, 25, buffer);
#endif
            return ki;
        }

        if (ki == 0)
            ki = getch();

#ifdef DOS_TERM
        puttext(1, 1, 80, 25, buffer);
#endif
        return anything;
    }

/* ******************************************************************
       gotoxy(1, 25);
       cprintf("Press any key to clear screen...");
       getch();
       clrscr();
       gotoxy(1, 25);
       cprintf("Press any key to restore screen...");
       getch();
****************************************************************** */

  putty:
#ifdef DOS_TERM
    puttext(1, 1, 80, 25, buffer);
#endif

    //ki = getch();

    //   gotoxy(80,25);
    //   cprintf(" \r");

    //   gotoxy(1, 25);
    //   cprintf("Press any key to quit...");
    //   getch();

    return ki;
}                               // end invent()

void list_commands(bool wizard)
{
    char st_pass[50];
    int j = 0;

#ifdef DOS_TERM
    char buffer[4800];

    window(1, 1, 80, 25);
    gettext(1, 1, 80, 25, buffer);
#endif

    st_pass[0] = '\0';
    clrscr();

    // BCR - Set to screen length - 1 to display the "more" string
    int moreLength = (NUMBER_OF_LINES - 1) * 2;

    for (int i = 0; i < 500; i++)
    {
        if (wizard)
            wizard_string(st_pass, i);
        else
            command_string(st_pass, i);

        if (strlen(st_pass) != 0)
        {
            // BCR - If we've reached the end of the screen, clear
            if (j == moreLength)
            {
                gotoxy(2, j / 2 + 1);
                cprintf("More...");
                getch();
                clrscr();
                j = 0;
            }

            gotoxy(((j % 2) ? 40 : 2), ((j / 2) + 1));
            cprintf(st_pass);

            j++;
        }
    }

    getch();

#ifdef DOS_TERM
    puttext(1, 1, 80, 25, buffer);
#endif

    return;
}                               // end list_commands()

void wizard_string(char comm[50], int i)
{

#ifdef WIZARD

    strcpy( comm,
           (i ==  10) ? "a    - (a)cquirement"                  :
           (i ==  15) ? "b    - (b)anish yourself to the Abyss" :
           (i ==  20) ? "d/*  - create (d)own stairdcase"       :
           (i ==  30) ? "g    - add a skill"                    :
           (i ==  40) ? "h    - (h)eal yourself"                :
           (i ==  50) ? "i    - (i)dentify an item"             :
           (i ==  70) ? "l    - make entrance to (l)abyrinth"   :
           (i ==  75) ? "k    - controlled blin(k)"             :
           (i ==  80) ? "m/+  - create a (m)onster by number"   :
           (i ==  85) ? "M    - create a (m)onster by name"     :
           (i ==  90) ? "o/%%  - create an (o)bject"            :
           (i == 100) ? "p    - make entrance to (p)andemonium" :
           (i == 105) ? "u    - create (u)p staircase"          :
           (i == 110) ? "x    - gain 5000 e(x)perience"         :
           (i == 112) ? "s    - gain 20000 (s)kill points"      :
           (i == 115) ? "X    - Receive a gift from (X)om"      :
           (i == 120) ? "z    - cast any spell by number"       :
           (i == 125) ? "Z    - cast any spell by name"         :
           (i == 130) ? "$    - get heal + gold + xp"           :
           (i == 140) ? "~/\"  - goto a level"                  :
           (i == 150) ? "(    - create a feature"               :
           (i == 160) ? "]    - get a mutation"                 :
           (i == 170) ? ":    - find branch"                    :
           (i == 180) ? "{    - magic mapping"                  :
           (i == 190) ? "^    - gain piety"                     :
           (i == 200) ? "\'    - list items"                    :
           (i == 210) ? "_    - sum skill points"               :
           (i == 220) ? "?    - list wizard commands"
                      : "" );

#else
    strcpy(comm, "");
#endif
}                               // end wizard_string()

void command_string(char comm[50], int i)
{
    /*
     * BCR - Command printing, case statement
     * Note: The numbers in this case indicate the order in which the
     *       commands will be printed out.  Make sure none of these
     *       numbers is greater than 500, because that is the limit.
     *
     * Arranged alpha lower, alpha upper, punctuation, ctrl.
     *
     */

    strcpy( comm,
           (i ==  10) ? "a    - use special (a)bility"              :
           (i ==  20) ? "d(#) - (d)rop (#) item(s)"                 :
           (i ==  30) ? "e    - (e)at some food"                    :
           (i ==  40) ? "f    - (f)ire an appropriate thing"        :
           (i ==  50) ? "i    - (i)nventory listing"                :
           (i ==  55) ? "m    - check skills"                       :
           (i ==  60) ? "o/c  - (o)pen/(c)lose a door"              :
           (i ==  65) ? "p    - (p)ray"                             :
           (i ==  70) ? "q    - (q)uaff a potion"                   :
           (i ==  80) ? "r    - (r)ead a scroll or a book"          :
           (i ==  90) ? "s    - (s)earch"                           :
           (i == 100) ? "t    - (t)hrow or shoot something"         :
           (i == 110) ? "v    - (v)iew details on an item"          :
           (i == 120) ? "w    - (w)ield a weapon"                   :
           (i == 130) ? "x    - e(x)amine your surroundings"        :
           (i == 135) ? "z    - (z)ap a wand"                       :
           (i == 140) ? "A    - check mutations"                    :
           (i == 141) ? "C    - (C)heck experience"                 :
           (i == 142) ? "D    - (D)issect a corpse"                 :
           (i == 145) ? "I    - (I)nvoke power of wielded item"     :
           (i == 150) ? "M    - (M)emorise a spell"                 :
           (i == 155) ? "O    - View (O)ver-map of important sites" :
           (i == 160) ? "P/R  - (P)ut on/(R)emove a ring"           :
           (i == 165) ? "Q    - commit suicide/(Q)uit"              :
           (i == 179) ? "V    - get (V)ersion information"          :
           (i == 200) ? "W/T  - (W)ear/(T)ake off armour"           :
           (i == 210) ? "X    - e(X)amine level map"                :
           (i == 220) ? "Z    - (Z)ap a spell"                      :
           (i == 240) ? ",/g  - pick something up"                  :
           (i == 242) ? "./del- wait"                               :
           (i == 250) ? "<>   - go up/down a staircase"             :
           (i == 270) ? ";    - see what items you're standing on"  :
           (i == 280) ? "\\    - check your item knowledge"         :
#ifdef WIZARD
           (i == 290) ? "&    - invoke your Wizardly powers"        :
#endif
           (i == 300) ? "+ &  - on map screen - fast scroll"        :
           (i == 310) ? "!    - shout/command allies"               :
           (i == 325) ? "^    - describe your religion"             :
           (i == 337) ? "@    - character status"                   :
           (i == 340) ? "#    - dump character to file <name>.txt"  :
           (i == 350) ? "=    - adjust inventory/spell letters"     :
           (i == 360) ? "\'    - wield item a, or switch to b"      :
#ifdef MACROS
           (i == 400) ? "`    - add macro"                          :
           (i == 410) ? "~    - save macros"                        :
#endif
           (i == 420) ? "]    - list armour"                        :
           (i == 425) ? "\"    - list jewelry"                      :
           (i == 430) ? "Shift & direction   - long walk"           :
           (i == 440) ? "Shift & 5 on keypad - rest 100 turns"      :
           (i == 451) ? "S    - (S)ave your game and exit"          :
           (i == 462) ? "Ctrl-P - see old messages"                 :
#ifdef PLAIN_TERM
           (i == 475) ? "Ctrl-R - Redraw screen"                    :
#endif
           (i == 484) ? "Ctrl-A - toggle autopickup"                :
           (i == 488) ? "Ctrl-X - Save game without query"          :
           (i == 493) ? "Ctrl & dir - door; untrap; attack"
                      : "" );
}                               // end command_string()

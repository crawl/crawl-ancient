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
#include "items.h"
#include "shopping.h"
#include "stuff.h"
#include "view.h"

#ifdef MACROS
#include "macro.h"
#endif

void command_string(char comm[50], int i);
void wizard_string(char comm[50], int i);

unsigned char get_invent(int invent_type)
{
    unsigned char nothing = invent(invent_type, false);

    redraw_screen();

    return (nothing);
}                               // end get_invent()

unsigned char invent(int item_class_inv, bool show_price)
{
    char st_pass[60];

    int i, j;
    char lines = 0;
    unsigned char anything = 0;
    char strng[10] = "";
    char yps = 0;
    char temp_id[4][50];

    const int num_lines = get_number_of_lines();

    FixedVector< int, NUM_OBJECT_CLASSES >  inv_class2;
    int inv_count = 0;
    unsigned char ki = 0;

#ifdef DOS_TERM
    char buffer[4600];

    gettext(1, 1, 80, 25, buffer);
    window(1, 1, 80, 25);
#endif

    for (i = 0; i < 4; i++)
    {
        for (j = 0; j < 50; j++)
        {
            temp_id[i][j] = 1;
        }
    }

    strcpy(st_pass, "");
    clrscr();

    for (i = 0; i < NUM_OBJECT_CLASSES; i++)
        inv_class2[i] = 0;

    for (i = 0; i < ENDOFPACK; i++)
    {
        if (you.inv[i].quantity)
        {
            inv_class2[ you.inv[i].base_type ]++;
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
        for (i = 0; i < NUM_OBJECT_CLASSES; i++)
        {
            if (item_class_inv == OBJ_MISSILES && i == OBJ_WEAPONS)
                i++;

            if (item_class_inv == OBJ_WEAPONS
                && (i == OBJ_STAVES || i == OBJ_MISCELLANY))
            {
                i++;
            }

            if (item_class_inv == OBJ_SCROLLS && i == OBJ_BOOKS)
                i++;

            if (i < NUM_OBJECT_CLASSES && item_class_inv != i)
                inv_class2[i] = 0;
        }
    }

    if ((item_class_inv == -1 && inv_count > 0)
        || (item_class_inv != -1 && inv_class2[item_class_inv] > 0)
        || (item_class_inv == OBJ_MISSILES && inv_class2[OBJ_WEAPONS] > 0)
        || (item_class_inv == OBJ_WEAPONS
            && (inv_class2[OBJ_STAVES] > 0 || inv_class2[OBJ_MISCELLANY] > 0))
        || (item_class_inv == OBJ_SCROLLS && inv_class2[OBJ_BOOKS] > 0))
    {
        cprintf("  Inventory");
        lines++;

        for (i = 0; i < 15; i++)
        {
            if (inv_class2[i] != 0)
            {
                if (lines > num_lines - 3)
                {
                    gotoxy(1, num_lines);
                    cprintf("-more-");

                    ki = getch();

                    if (ki == ESCAPE)
                    {
#ifdef DOS_TERM
                        puttext(1, 1, 80, 25, buffer);
#endif
                        return (ESCAPE);
                    }
                    else if (isalpha(ki) || ki == '?' || ki == '*')
                    {
#ifdef DOS_TERM
                        puttext(1, 1, 80, 25, buffer);
#endif
                        return (ki);
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
                    if (lines > num_lines - 2 && inv_count > 0)
                    {
                        gotoxy(1, num_lines);
                        cprintf("-more-");
                        ki = getch();

                        if (ki == ESCAPE)
                        {
#ifdef DOS_TERM
                            puttext(1, 1, 80, 25, buffer);
#endif
                            return (ESCAPE);
                        }
                        else if (isalpha(ki) || ki == '?' || ki == '*')
                        {
#ifdef DOS_TERM
                            puttext(1, 1, 80, 25, buffer);
#endif
                            return (ki);
                        }

                        if (ki == 0)
                            ki = getch();

                        lines = 0;
                        clrscr();
                        gotoxy(1, 1);
                        anything = 0;
                    }

                    if (is_valid_item(you.inv[j]) && you.inv[j].base_type==i)
                    {
                        anything++;

                        if (lines > 0)
                            cprintf(EOL);

                        lines++;

                        yps = wherey();

                        in_name( j, DESC_INVENTORY_EQUIP, st_pass );
                        cprintf( st_pass );

                        inv_count--;


                        if (show_price)
                        {
                            cprintf(" (");

                            itoa( item_value( you.inv[j], temp_id, true ), strng, 10 );

                            cprintf(strng);
                            cprintf("gold)");
                        }

                        if (wherey() != yps)
                            lines++;
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
            else if (item_class_inv == OBJ_MISSILES)
                cprintf("You aren't carrying any ammunition.");
            else
                cprintf("You aren't carrying any such object.");

            anything++;
        }
    }

    if (anything > 0)
    {
        ki = getch();

        if (isalpha(ki) || ki == '?' || ki == '*')
        {
#ifdef DOS_TERM
            puttext(1, 1, 80, 25, buffer);
#endif
            return (ki);
        }

        if (ki == 0)
            ki = getch();
    }

  putty:
#ifdef DOS_TERM
    puttext(1, 1, 80, 25, buffer);
#endif

    return (ki);
}                               // end invent()


// Reads in digits for a count and apprends then to val, the
// return value is the character that stopped the reading.
static unsigned char get_invent_quant( unsigned char keyin, int &quant )
{
    quant = keyin - '0';

    for(;;)
    {
        keyin = get_ch();

        if (!isdigit( keyin ))
            break;

        quant *= 10;
        quant += (keyin - '0');

        if (quant > 9999999)
        {
            quant = 9999999;
            keyin = '\0';
            break;
        }
    }

    return (keyin);
}


// This function prompts the user for an item, handles the '?' and '*'
// listings, and returns the inventory slot to the caller (which if
// must_exist is true (the default) will be an assigned item, with
// a positive quantity.
//
// It returns PROMPT_ABORT       if the player hits escape.
// It returns PROMPT_GOT_SPECIAL if the player hits the "other_valid_char".
//
// Note: This function never checks if the item is appropriate.
int prompt_invent_item( const char *prompt, int type_expect,
                        bool must_exist, const char other_valid_char,
                        int *const count )
{
    unsigned char  keyin = 0;
    int            ret = -1;

    bool           need_redraw = false;
    bool           need_prompt = true;
    bool           need_getch  = true;

    for (;;)
    {
        if (need_redraw)
        {
            mesclr();
            redraw_screen();
        }

        if (need_prompt)
            mpr( prompt, MSGCH_PROMPT );

        if (need_getch)
            keyin = get_ch();

        need_redraw = false;
        need_prompt = true;
        need_getch  = true;

        // Note:  We handle any "special" character first, so that
        //        it can be used to override the others.
        if (other_valid_char != '\0' && keyin == other_valid_char)
        {
            ret = PROMPT_GOT_SPECIAL;
            break;
        }
        else if (keyin == '?' || keyin == '*')
        {
            // The "view inventory listing" mode.
            if (keyin == '*')
                keyin = invent( -1, false );
            else
                keyin = invent( type_expect, false );

            need_getch  = false;

            // Don't redraw if we're just going to display another listing
            need_redraw = (keyin != '?' && keyin != '*');

            // A prompt is nice for when we're moving to "count" mode.
            need_prompt = (count != NULL && isdigit( keyin ));
        }
        else if (count != NULL && isdigit( keyin ))
        {
            // The "read in quantity" mode
            keyin = get_invent_quant( keyin, *count );

            need_prompt = false;
            need_getch  = false;
        }
        else if (keyin == ESCAPE || keyin == ' '
                || keyin == '\r' || keyin == '\n')
        {
            ret = PROMPT_ABORT;
            break;
        }
        else if (isalpha( keyin ))
        {
            ret = letter_to_index( keyin );

            if (must_exist && !is_valid_item( you.inv[ret] ))
                mpr( "You do not have any such object." );
            else
                break;
        }
        else    // we've got a character we don't understand...
        {
            canned_msg( MSG_HUH );
        }
    }

    return (ret);
}

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
    int moreLength = (get_number_of_lines() - 1) * 2;

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
           (i ==  10) ? "a    : acquirement"                  :
           (i ==  15) ? "b    : controlled blink"             :
           (i ==  20) ? "B    : banish yourself to the Abyss" :
           (i ==  30) ? "g    : add a skill"                  :
           (i ==  35) ? "G    : remove all monsters"          :
           (i ==  40) ? "h/H  : heal yourself (super-Heal)"   :
           (i ==  50) ? "i/I  : identify/unidentify inventory":
           (i ==  70) ? "l    : make entrance to labyrinth"   :
           (i ==  80) ? "m    : create a monster by number"   :
           (i ==  85) ? "M    : create a monster by name"     :
           (i ==  90) ? "o/%%  : create an object"            :
           (i == 100) ? "p    : make entrance to pandemonium" :
           (i == 110) ? "x    : gain an experience level"     :
           (i == 112) ? "s    : gain 20000 skill points"      :
           (i == 113) ? "t    : tweak object properties"      :
           (i == 115) ? "X    : Receive a gift from Xom"      :
           (i == 120) ? "z    : cast any spell by number"     :
           (i == 125) ? "Z    : cast any spell by name"       :
           (i == 130) ? "$    : get 1000 gold"                :
           (i == 135) ? "</>  : create up/down staircase"     :
           (i == 135) ? "u/d  : shift up/down one level"      :
           (i == 140) ? "~/\"  : goto a level"                :
           (i == 150) ? "(    : create a feature"             :
           (i == 160) ? "]    : get a mutation"               :
           (i == 165) ? "[    : get a demonspawn mutation"    :
           (i == 170) ? ":    : find branch"                  :
           (i == 180) ? "{    : magic mapping"                :
           (i == 190) ? "^    : gain piety"                   :
           (i == 200) ? "_    : gain religion"                :
           (i == 210) ? "\'    : list items"                  :
           (i == 220) ? "?    : list wizard commands"         :
           (i == 230) ? "|    : acquire all unrand artefacts" :
           (i == 240) ? "+    : turn item into random artefact"
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
           (i ==  10) ? "a    : use special ability"              :
           (i ==  20) ? "d(#) : drop (exact quantity of) items"   :
           (i ==  30) ? "e    : eat food"                         :
           (i ==  40) ? "f    : fire first available missile"     :
           (i ==  50) ? "i    : inventory listing"                :
           (i ==  55) ? "m    : check skills"                     :
           (i ==  60) ? "o/c  : open / close a door"              :
           (i ==  65) ? "p    : pray"                             :
           (i ==  70) ? "q    : quaff a potion"                   :
           (i ==  80) ? "r    : read a scroll or book"            :
           (i ==  90) ? "s    : search adjacent tiles"            :
           (i == 100) ? "t    : throw/shoot an item"              :
           (i == 110) ? "v    : view item description"            :
           (i == 120) ? "w    : wield an item"                    :
           (i == 130) ? "x    : examine visible surroundings"     :
           (i == 135) ? "z    : zap a wand"                       :
           (i == 140) ? "A    : list abilities/mutations"         :
           (i == 141) ? "C    : check experience"                 :
           (i == 142) ? "D    : dissect a corpse"                 :
           (i == 145) ? "I    : invoke power of wielded item"     :
           (i == 150) ? "M    : memorise a spell"                 :
           (i == 155) ? "O    : view level map"                   :
           (i == 160) ? "P/R  : put on / remove jewellery"        :
           (i == 165) ? "Q    : quit without saving"              :
           (i == 168) ? "S    : save game and exit"               :
           (i == 179) ? "V    : version information"              :
           (i == 200) ? "W/T  : wear / take off armour"           :
           (i == 210) ? "X    : examine level map"                :
           (i == 220) ? "Z    : cast a spell"                     :
           (i == 240) ? ",/g  : pick up items"                    :
           (i == 242) ? "./del: rest one turn"                    :
           (i == 250) ? "</>  : ascend / descend a staircase"     :
           (i == 270) ? ";    : examine occupied tile"            :
           (i == 280) ? "\\    : check item knowledge"            :
#ifdef WIZARD
           (i == 290) ? "&    : invoke your Wizardly powers"      :
#endif
           (i == 300) ? "+/-  : scroll up/down [level map only]"  :
           (i == 310) ? "!    : shout or command allies"          :
           (i == 325) ? "^    : describe religion"                :
           (i == 337) ? "@    : status"                           :
           (i == 340) ? "#    : dump character to file"           :
           (i == 350) ? "=    : reassign inventory/spell letters" :
           (i == 360) ? "\'    : wield item a, or switch to b"    :
#ifdef MACROS
           (i == 400) ? "`    : add macro"                        :
           (i == 410) ? "~    : save macros"                      :
#endif
           (i == 420) ? "]    : display worn armour"              :
           (i == 425) ? "\"    : display worn jewellery"          :
           (i == 430) ? "Shift & direction   : long walk"         :
           (i == 440) ? "Shift & 5 on keypad : rest 100 turns"    :
           (i == 462) ? "Ctrl-P : see old messages"               :
#ifdef PLAIN_TERM
           (i == 475) ? "Ctrl-R : Redraw screen"                  :
#endif
           (i == 484) ? "Ctrl-A : toggle autopickup"              :
           (i == 488) ? "Ctrl-X : Save game without query"        :
           (i == 493) ? "Ctrl & dir : door; untrap; attack"
                      : "" );
}                               // end command_string()

/*
 *  File:       invent.cc
 *  Summary:    Functions for inventory related commands.
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 *      <3>     6/9/99         DML              Autopickup
 *      <2>     5/20/99        BWR              Extended screen lines support
 *      <1>     -/--/--        LRH              Created
 */

#include "AppHdr.h"
#include "invent.h"

#ifdef DOS
#include <conio.h>
#endif

#include "defines.h"
#include "externs.h"
#include "enum.h"

#ifdef USE_CURSES
#include <curses.h>
#endif

#include <stdlib.h>
#include <string.h>

#include "itemname.h"

//#include "crawlfnc.h"
#include "shopping.h"
#include "stuff.h"

#ifdef MACROS
  #include "macro.h"
#endif

void command_string(char comm[50], int i);

//char invent(unsigned char inv_plus2 [52], int item_class_inv, int inv_quantity [52], unsigned char inv_dam [52], unsigned char inv_class [52], unsigned char inv_type [52], unsigned char inv_plus [52], unsigned char inv_ident [52], char item_wielded, char body_armour, char shield_armour, char cloak_armour, char head_armour, char hand_armour, char foot_armour, char ring [2], char show_price);
char invent(int item_class_inv, char show_price);

unsigned char get_invent(int invent_type)
{
    char nothing;

    nothing = invent(invent_type, 0);
    //you.inv_plus2, invent_type, you.inv_quantity, you.inv_dam, you.inv_class, you.inv_type, you.inv_plus, you.inv_ident, you.item_wielded, you.armour [0], you.armour [5], you.armour [2], you.armour [1], you.armour [3], you.armour [4], you.ring, 0);

#ifdef PLAIN_TERM
    redraw_screen();
#endif

    return nothing;
}

//char invent(unsigned char inv_plus2 [52], int item_class_inv, int inv_quantity [52], unsigned char inv_dam [52], unsigned char inv_class [52], unsigned char inv_type [52], unsigned char inv_plus [52], unsigned char inv_ident [52], char item_wielded, char body_armour, char shield_armour, char cloak_armour, char head_armour, char hand_armour, char foot_armour, char ring [2], char show_price)
char invent(int item_class_inv, char show_price)
{
    char st_pass[60];

    int i;
    int j;
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
    //clrscr();
    //   for (i = 0; i <= 20; i++)
    //      cprintf("Line #%d\r\n", i);





#ifdef DOS_TERM
    char buffer[4600];

    gettext(1, 1, 80, 25, buffer);
    window(1, 1, 80, 25);
#endif



    strcpy(st_pass, "");

    clrscr();


//void inventory(int item_class_inv)

//{

    int Inv_class2[15];
    int inv_count = 0;
    unsigned char ki = 0;

//int ickyo = 0;

    for (i = 0; i < 15; i++)
    {
        Inv_class2[i] = 0;
    }


//if (num_inv_items != 0)
    //{

//strcpy(info [info_lines], " You are carrying"); info_lines++;

    for (i = 0; i < 52; i++)
    {
        if (you.inv_quantity[i] != 0)
        {
//  if (inv_class [i] == 11) Inv_class2 [3] ++; else
            Inv_class2[you.inv_class[i]]++;     // adds up number of each class in invent.

            inv_count++;
        }
    }

    if (inv_count == 0)
    {
        cprintf("You aren't carrying anything.");
        if (getch() == 0)
            getch();
        goto putty;
        //return;
    }

    if (item_class_inv != -1)
    {
        for (i = 0; i < 15; i++)
        {
            if (item_class_inv == 1 && i == 0)
                i++;
            if (item_class_inv == 0 && i == 11)
                i++;
// if (item_class_inv == 11 && i == 3) i++;
            if (item_class_inv == 6 && i == 10)
                i++;
            if (item_class_inv != i)
                Inv_class2[i] = 0;
        }
    }


//cprintf("hello!");
    //abort();
    //if (item_class_inv > 1) Inv_class2 [0] = 0;


    if ((item_class_inv == -1 && inv_count > 0) || (item_class_inv != -1 && Inv_class2[item_class_inv] > 0) || (item_class_inv == 1 && (Inv_class2[0] > 0 || Inv_class2[1] > 0)) || (item_class_inv == 0 && (Inv_class2[0] > 0 || Inv_class2[11] > 0)) || (item_class_inv == 0 && (Inv_class2[0] > 0 || Inv_class2[13] > 0)) || (item_class_inv == 6 && (Inv_class2[6] > 0 || Inv_class2[10] > 0)))     // || (item_class_inv == 3 && (Inv_class2 [3] > 0 || Inv_class2 [11] > 0)))

    {

//if (item_class_inv != 1) //this is so you can get the '?' invent from throw_it
        //{
        cprintf("  Inventory");
        lines++;
//info_lines++;
        //}


        for (i = 0; i < 15; i++)
        {
            if (Inv_class2[i] != 0)
            {
//      cprintf("\n\r");
                //lines++;
                //      cprintf(" ");
                if (lines > NUMBER_OF_LINES - 3)        // && inv_count > 0)

                {
                    //gotoxy(1,25);
                    //cprintf("\n\r-more-");
                    //ki = getch();
                    //lines = 0;
                    //clrscr();
                    //gotoxy(1,1);
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
                    if (ki >= 65 && ki < 123)
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
                case 0:
                    cprintf("Hand weapons");
                    break;
                case 1:
                    cprintf("Missiles");
                    break;
                case 2:
                    cprintf("Armour");
                    break;
                case 3:
                    cprintf("Magical devices");
                    break;
                case 4:
                    cprintf("Comestibles");
                    break;
                case 5:
                    cprintf("Books");
                    break;
                case 6:
                    cprintf("Scrolls");
                    break;
                case 7:
                    cprintf("Jewellery");
                    break;
                case 8:
                    cprintf("Potions");
                    break;
                case 9:
                    cprintf("Gems");
                    break;
                case 10:
                    cprintf("Books");
                    break;
                case 11:
                    cprintf("Magical staves");
                    break;
                case 12:
                    cprintf("Orbs of Power");
                    break;
                case 13:
                    cprintf("Miscellaneous");
                    break;
                case 14:
                    cprintf("Carrion");
                    break;
//   case 16: cprintf("Miscellaneous"); break;
                }
                textcolor(LIGHTGREY);
                //cprintf("\n\r");
                lines++;

                for (j = 0; j < 52; j++)
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
                        if (ki >= 65 && ki < 123)
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
                        if (j < 26)
                            ft = (char) j + 97;
                        else
                            ft = (char) j + 65 - 26;
                        //itoa(j, strng, 10);

                        strng[0] = ft;
                        strng[1] = 0;
                        //strng [0] += 49;
                        cprintf(" ");
                        cprintf(strng);
                        //cout << " " << ft;
                        //cprintf(" ");
                        //cprintf(ft);
                        cprintf(" - ");



                        yps = wherey();

                        item_name(you.inv_plus2[j], you.inv_class[j], you.inv_type[j], you.inv_dam[j], you.inv_plus[j], you.inv_quantity[j], you.inv_ident[j], 3, st_pass);
                        cprintf(st_pass);

                        inv_count--;

                        if (j == you.equip[EQ_WEAPON])
                            cprintf(" (weapon)");
                        if (j == you.equip[EQ_CLOAK] || j == you.equip[EQ_HELMET] || j == you.equip[EQ_GLOVES] || j == you.equip[EQ_BOOTS] || j == you.equip[EQ_SHIELD] || j == you.equip[EQ_BODY_ARMOUR])
                            cprintf(" (worn)");
                        if (j == you.equip[EQ_LEFT_RING])
                            cprintf(" (left hand)");
                        if (j == you.equip[EQ_RIGHT_RING])
                            cprintf(" (right hand)");
                        if (j == you.equip[EQ_AMULET])
                            cprintf(" (around neck)");

                        if (show_price == 1)
                        {
                            cprintf(" (");
                            itoa(item_value(you.inv_class[j], you.inv_type[j], you.inv_dam[j], you.inv_plus[j], you.inv_plus2[j], you.inv_quantity[j], 3, temp_id), strng, 10);
                            cprintf(strng);
                            cprintf("gold)");
                        }
                        if (wherey() != yps)
                            lines++;


//                      cprintf("\n\r");

                        //       cprintf("\n\r");

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
            if (item_class_inv == 0)
                cprintf("You aren't carrying any weapons.");
            else
            {
                if (item_class_inv == 1)
                    cprintf("You aren't carrying any ammunition.");
                else
                {
                    cprintf("You aren't carrying any such object.");
                }
            }
            anything++;
        }
        //info_lines ++;
    }
//} // end of if (num_inv_items)
    //relay_message();
    //more();

//} // end of inventory()




    if (anything > 0)
    {
        ki = getch();
        //ki = getch();
        //ki = anything;
        if (ki >= 65 && ki < 123)
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

    /*
       gotoxy(1, 25);
       cprintf("Press any key to clear screen...");
       getch();
       clrscr();
       gotoxy(1, 25);
       cprintf("Press any key to restore screen...");
       getch();
     */
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

    //return 0;
}



void list_commands(void)
{

    char st_pass[50];

    int i;
    int j = 0;

#ifdef DOS_TERM
    char buffer[4800];

    window(1, 1, 80, 25);
    gettext(1, 1, 80, 25, buffer);
#endif
    strcpy(st_pass, "");
    clrscr();

    for (i = 0; i < 5000; i++)
    {
        command_string(st_pass, i);
        if (strlen(st_pass) < 5)
        {
//     i--;
            continue;
        }
        if (j % 2 == 0)
        {
            gotoxy(2, j / 2 + 1);
            cprintf(st_pass);
            j++;
            continue;
        }
        gotoxy(40, j / 2 + 1);
        cprintf(st_pass);
        j++;
    }
//   j ++;
    getch();
#ifdef DOS_TERM
    puttext(1, 1, 80, 25, buffer);
#endif

//   cprintf("xxxxxxxxxxxxx");
    //   last_requested = 0;
    return;
}


void command_string(char comm[50], int i)
{

    switch (i)
    {
// case 1: strcpy(comm, "a - adijsd"); break;
        // case 10: strcpy(comm, "c - close a door"); break;
    case 10:
        strcpy(comm, "a - use special ability");
        break;
    case 20:
        strcpy(comm, "d(#) - drop (#) item(s)");
        break;
    case 30:
        strcpy(comm, "e - eat");
        break;
    case 40:
        strcpy(comm, "f - fire an appropriate thing");
        break;
    case 50:
        strcpy(comm, "i - inventory");
        break;
    case 55:
        strcpy(comm, "m - check skills");
        break;
    case 60:
        strcpy(comm, "o/c - open/close a door");
        break;
    case 65:
        strcpy(comm, "p - pray");
        break;
    case 70:
        strcpy(comm, "q - quaff a potion");
        break;
    case 80:
        strcpy(comm, "r - read a scroll or a book");
        break;
    case 90:
        strcpy(comm, "s/./del - wait");
        break;
    case 100:
        strcpy(comm, "t - throw or shoot something");
        break;
    case 110:
        strcpy(comm, "v - get version number");
        break;
    case 120:
        strcpy(comm, "w - wield a weapon");
        break;
    case 130:
        strcpy(comm, "x - look around you");
        break;
    case 135:
        strcpy(comm, "z - zap a wand");
        break;
    case 140:
        strcpy(comm, "A - check mutations");
        break;
    case 141:
        strcpy(comm, "C - check experience");
        break;
    case 142:
        strcpy(comm, "D - butcher (dissect) a corpse");
        break;
    case 145:
        strcpy(comm, "I - Invoke power of wielded item");
        break;
    case 150:
        strcpy(comm, "M - Memorise a spell");
        break;
    case 160:
        strcpy(comm, "P/R - Put on/Remove a ring");
        break;
    case 165:
        strcpy(comm, "Q - commit suicide (quit)");
        break;
// case 170: strcpy(comm, "R - Remove a ring"); break;
    case 180:
        strcpy(comm, "S/Ctrl-X - Save your game and exit");
        break;
// case 190: strcpy(comm, "T - Take off armour"); break;
    case 209:
        strcpy(comm, "V - Examine an identified item");
        break;
    case 200:
        strcpy(comm, "W/T - Wear/Take off armour");
        break;
    case 210:
        strcpy(comm, "X - show map of level");
        break;
    case 220:
        strcpy(comm, "Z - cast a spell");
        break;
// case 230: strcpy(comm, ". - wait"); break;
    case 240:
        strcpy(comm, ", or g - pick something up");
        break;
    case 250:
        strcpy(comm, "<> - go up/down a staircase");
        break;
// case 260: strcpy(comm, "> - go down a staircase"); break;
    case 270:
        strcpy(comm, "; - see what items you're standing on");
        break;
    case 280:
        strcpy(comm, "\\ - check your item knowledge");
        break;
// case 290: strcpy(comm, "? - get this list of commands"); break;
    case 300:
        strcpy(comm, "Ctrl-P - see old messages");
        break;
    case 310:
        strcpy(comm, "Shift & direction - long walk");
        break;
    case 320:
        strcpy(comm, "Shift & 5 on keypad - rest 100 turns");
        break;
// case 330: strcpy(comm, "Rogue movement keys also work"); break;
    case 340:
        strcpy(comm, "+ & - on map screen - fast scroll");
        break;
    case 350:
        strcpy(comm, "! - shout/command allies");
        break;
    case 360:
        strcpy(comm, "Ctrl & dir - door; untrap; attack");
        break;
    case 365:
        strcpy(comm, "^ - describe your religion");
        break;
    case 367:
        strcpy(comm, "@ - character status");
        break;
    case 370:
        strcpy(comm, "# - dump character to file <name>.txt");
        break;
    case 380:
        strcpy(comm, "= - adjust inventory/spell letters");
        break;
    case 390:
        strcpy(comm, "\' - wield item a, or switch to b");
        break;
#ifdef MACROS
    case 400:
        strcpy(comm, "` - add macro");
        break;
    case 410:
        strcpy(comm, "~ - save macros");
        break;
#endif
/* case 380: strcpy(comm, "d - drop"); break;
   case 390: strcpy(comm, "e - eat"); break; */
    case 415:
      strcpy(comm, "Ctrl-A toggle autopickup");
      break;
    case 420:
#ifdef PLAIN_TERM
        strcpy(comm, "Ctrl-R - Redraw screen");
#else
        strcpy(comm, "");
#endif
        break;

    case 430:
        strcpy(comm, ")] or \" - List equipment");
        break;

    default:
        strcpy(comm, "");
        break;
    }

//last_requested = i;
}

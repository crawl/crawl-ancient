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

#ifdef DOS
#include <conio.h>
#endif

#ifdef USE_CURSES
#include <curses.h>
#endif

#include <stdlib.h>
#include <string.h>

#include "externs.h"

#include "itemname.h"
#include "shopping.h"
#include "stuff.h"

#ifdef MACROS
  #include "macro.h"
#endif

   void command_string(char comm[50], int i);
   void wizard_string(char comm[50], int i);




   unsigned char get_invent(int invent_type)
   {
      char nothing;

      nothing = invent(invent_type, false);
    //you.inv_plus2, invent_type, you.inv_quantity, you.inv_dam, you.inv_class, you.inv_type, you.inv_plus, you.inv_ident, you.item_wielded, you.armour [0], you.armour [5], you.armour [2], you.armour [1], you.armour [3], you.armour [4], you.ring, 0);

   #ifdef PLAIN_TERM
    redraw_screen();
   #endif

      return nothing;
   }




   //char invent(unsigned char inv_plus2 [52], int item_class_inv, int inv_quantity [52], unsigned char inv_dam [52], unsigned char inv_class [52], unsigned char inv_type [52], unsigned char inv_plus [52], unsigned char inv_ident [52], char item_wielded, char body_armour, char shield_armour, char cloak_armour, char head_armour, char hand_armour, char foot_armour, char ring [2], char show_price)
   char invent(int item_class_inv, bool show_price)
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
      //for (i = 0; i <= 20; i++)
      //  cprintf("Line #%d\r\n", i);


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
            if (item_class_inv == OBJ_MISSILES && i == 0)
              i++;
            if (item_class_inv == OBJ_WEAPONS && i == 11)
              i++;
            //if (item_class_inv == OBJ_STAVES && i == 3) i++;
            if (item_class_inv == OBJ_SCROLLS && i == 10)
              i++;
            if (item_class_inv != i)
              Inv_class2[i] = 0;
         }
      }


      //cprintf("hello!");
      //abort();
      //if (item_class_inv > OBJ_MISSILES) Inv_class2 [0] = 0;


      if ((item_class_inv == -1 && inv_count > 0) || (item_class_inv != -1 && Inv_class2[item_class_inv] > 0) || (item_class_inv == OBJ_MISSILES && (Inv_class2[0] > 0 || Inv_class2[1] > 0)) || (item_class_inv == OBJ_WEAPONS && (Inv_class2[0] > 0 || Inv_class2[11] > 0)) || (item_class_inv == OBJ_WEAPONS && (Inv_class2[0] > 0 || Inv_class2[13] > 0)) || (item_class_inv == OBJ_SCROLLS && (Inv_class2[6] > 0 || Inv_class2[10] > 0)))     // || (item_class_inv == OBJ_WANDS && (Inv_class2 [3] > 0 || Inv_class2 [11] > 0)))

      {

      //if (item_class_inv != OBJ_MISSILES) //this is so you can get the '?' invent from throw_it
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
                  case OBJ_WEAPONS:
                     cprintf("Hand weapons");
                     break;
                  case OBJ_MISSILES:
                     cprintf("Missiles");
                     break;
                  case OBJ_ARMOUR:
                     cprintf("Armour");
                     break;
                  case OBJ_WANDS:
                     cprintf("Magical devices");
                     break;
                  case OBJ_FOOD:
                     cprintf("Comestibles");
                     break;
                  case OBJ_UNKNOWN_I:
                     cprintf("Books");
                     break;
                  case OBJ_SCROLLS:
                     cprintf("Scrolls");
                     break;
                  case OBJ_JEWELLERY:
                     cprintf("Jewellery");
                     break;
                  case OBJ_POTIONS:
                     cprintf("Potions");
                     break;
                  case OBJ_UNKNOWN_II:
                     cprintf("Gems");
                     break;
                  case OBJ_BOOKS:
                     cprintf("Books");
                     break;
                  case OBJ_STAVES:
                     cprintf("Magical staves");
                     break;
                  case OBJ_ORBS:
                     cprintf("Orbs of Power");
                     break;
                  case OBJ_MISCELLANY:
                     cprintf("Miscellaneous");
                     break;
                  case OBJ_CORPSES:
                     cprintf("Carrion");
                     break;
                  //case OBJ_GEMSTONES: cprintf("Miscellaneous"); break;
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

                     if ( show_price )
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
            if (item_class_inv == OBJ_WEAPONS)
               cprintf("You aren't carrying any weapons.");
            else
            {
               if (item_class_inv == OBJ_MISSILES)
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



   void list_commands(int wizard)
   {

      char st_pass[50];

      int i;
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

      for (i = 0; i < 500; i++)
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

            if (j % 2 == 0)
            {
               gotoxy(2, j / 2 + 1);
               cprintf(st_pass);
            }
            else
            {
               gotoxy(40, j / 2 + 1);
               cprintf(st_pass);
            }
            j++;
         }
      }
      getch();
   #ifdef DOS_TERM
    puttext(1, 1, 80, 25, buffer);
   #endif

      return;
   }




   void wizard_string(char comm[50], int i)
#ifdef WIZARD
   {
      switch (i)
      {
         case 10:
            strcpy(comm, "a    - (a)cquirement");
            break;
         case 15:
            strcpy(comm, "b    - (b)anish yourself to the Abyss");
            break;
         case 20:
            strcpy(comm, "d/*  - create (d)own stairdcase");
            break;
         case 30:
            strcpy(comm, "g    - add a skill");
            break;
         case 40:
            strcpy(comm, "h    - (h)eal yourself");
            break;
         case 50:
            strcpy(comm, "i    - (i)dentify an item");
            break;
         case 70:
            strcpy(comm, "l    - make entrance to (l)abyrinth");
            break;
         case 75:
            strcpy(comm, "k    - controlled blin(k)");
            break;
         case 80:
            strcpy(comm, "m/+  - create a (m)onster by number");
            break;
         case 85:
            strcpy(comm, "M    - create a (m)onster by name");
            break;
         case 90:
            strcpy(comm, "o/%%  - create an (o)bject");
            break;
         case 100:
            strcpy(comm, "p    - make entrance to (p)andemonium");
            break;
         case 105:
            strcpy(comm, "u    - create (u)p staircase");
            break;
         case 110:
            strcpy(comm, "x    - gain 5000 e(x)perience");
            break;
         case 115:
            strcpy(comm, "X    - Receive a gift from (X)om");
            break;
         case 120:
            strcpy(comm, "z    - cast any spell by number");
            break;
         case 125:
            strcpy(comm, "Z    - cast any spell by name");
            break;
         case 130:
            strcpy(comm, "$    - get heal + gold + xp");
            break;
         case 140:
            strcpy(comm, "~/\"  - goto a level");
            break;
         case 150:
            strcpy(comm, "(    - create a feature");
            break;
         case 160:
            strcpy(comm, "]    - get a mutation");
            break;
         case 170:
            strcpy(comm, ":    - find branch");
            break;
         case 180:
            strcpy(comm, "{    - magic mapping");
            break;
         case 190:
            strcpy(comm, "^    - gain piety");
            break;
         case 200:
            strcpy(comm, "\'    - list items");
            break;
         case 210:
            strcpy(comm, "_    - sum skill points");
            break;
         case 220:
            strcpy(comm, "?    - list wizard commands");
            break;
         default:
            strcpy(comm, "");
            break;
      }
   }
#else
{
            strcpy(comm, "");
}
#endif




   void command_string(char comm[50], int i)
   {

   /* BCR - Command printing, case statement
   * Note: The numbers in this case indicate the order in which the
   *       commands will be printed out.  Make sure none of these
   *       numbers is greater than 500, because that is the limit.
   *
   * I've arranged these alpha lower, alpha upper, punctuation, ctrl.
   */

      switch (i)
      {
         case 10:
            strcpy(comm, "a    - use special (a)bility");
            break;
         case 20:
            strcpy(comm, "d(#) - (d)rop (#) item(s)");
            break;
         case 30:
            strcpy(comm, "e    - (e)at some food");
            break;
         case 40:
            strcpy(comm, "f    - (f)ire an appropriate thing");
            break;
         case 50:
            strcpy(comm, "i    - (i)nventory listing");
            break;
         case 55:
            strcpy(comm, "m    - check skills");
            break;
         case 60:
            strcpy(comm, "o/c  - (o)pen/(c)lose a door");
            break;
         case 65:
            strcpy(comm, "p    - (p)ray");
            break;
         case 70:
            strcpy(comm, "q    - (q)uaff a potion");
            break;
         case 80:
            strcpy(comm, "r    - (r)ead a scroll or a book");
            break;
         case 90:
            strcpy(comm, "s    - (s)earch");
            break;
         case 100:
            strcpy(comm, "t    - (t)hrow or shoot something");
            break;
         case 110:
            strcpy(comm, "v    - (v)iew details on an item");
            break;
         case 120:
            strcpy(comm, "w    - (w)ield a weapon");
            break;
         case 130:
            strcpy(comm, "x    - e(x)amine your surroundings");
            break;
         case 135:
            strcpy(comm, "z    - (z)ap a wand");
            break;
         case 140:
            strcpy(comm, "A    - check mutations");
            break;
         case 141:
            strcpy(comm, "C    - (C)heck experience");
            break;
         case 142:
            strcpy(comm, "D    - (D)issect a corpse");
            break;
         case 145:
            strcpy(comm, "I    - (I)nvoke power of wielded item");
            break;
         case 150:
            strcpy(comm, "M    - (M)emorise a spell");
            break;
         case 155:
            strcpy(comm, "O    - View (O)ver-map of important sites");
            break;
         case 160:
            strcpy(comm, "P/R  - (P)ut on/(R)emove a ring");
            break;
         case 165:
            strcpy(comm, "Q    - commit suicide/(Q)uit");
            break;
         case 179:
            strcpy(comm, "V    - get (V)ersion information");
            break;
         case 200:
            strcpy(comm, "W/T  - (W)ear/(T)ake off armour");
            break;
         case 210:
            strcpy(comm, "X    - e(X)amine level map");
            break;
         case 220:
            strcpy(comm, "Z    - (Z)ap a spell");
            break;
         case 240:
            strcpy(comm, ",/g  - pick something up");
            break;
         case 242:
            strcpy(comm, "./del- wait");
            break;
         case 250:
            strcpy(comm, "<>   - go up/down a staircase");
            break;
         case 270:
            strcpy(comm, ";    - see what items you're standing on");
            break;
         case 280:
            strcpy(comm, "\\    - check your item knowledge");
            break;
      #ifdef WIZARD
         case 290:
            strcpy(comm, "&    - invoke your Wizardly powers");
            break;
      #endif
         case 300:
            strcpy(comm, "+ &  - on map screen - fast scroll");
            break;
         case 310:
            strcpy(comm, "!    - shout/command allies");
            break;
         case 325:
            strcpy(comm, "^    - describe your religion");
            break;
         case 337:
            strcpy(comm, "@    - character status");
            break;
         case 340:
            strcpy(comm, "#    - dump character to file <name>.txt");
            break;
         case 350:
            strcpy(comm, "=    - adjust inventory/spell letters");
            break;
         case 360:
            strcpy(comm, "\'    - wield item a, or switch to b");
            break;
      #ifdef MACROS
         case 400:
            strcpy(comm, "`    - add macro");
            break;
         case 410:
            strcpy(comm, "~    - save macros");
            break;
      #endif
         case 420:
            strcpy(comm, "]    - list armour");
            break;
         case 425:
            strcpy(comm, "\"    - list jewellery");
            break;
         case 430:
            strcpy(comm, "Shift & direction - long walk");
            break;
         case 440:
            strcpy(comm, "Shift & 5 on keypad - rest 100 turns");
            break;
         case 451:
            strcpy(comm, "S - (S)ave your game and exit");
            break;
         case 462:
            strcpy(comm, "Ctrl-P - see old messages");
            break;
      #ifdef PLAIN_TERM
      case 475:
        strcpy(comm, "Ctrl-R - Redraw screen");
        break;
      #endif
         case 484:
            strcpy(comm, "Ctrl-A - toggle autopickup");
            break;
         case 488:
            strcpy(comm, "Ctrl-X - Save game without query");
            break;
         case 493:
            strcpy(comm, "Ctrl & dir - door; untrap; attack");
            break;

         default:
            strcpy(comm, "");
            break;
      }

   }

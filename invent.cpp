#include <conio.h>
#include <stdlib.h>
#include <iostream.h>
#include <string.h>

#include "crawlfnc.h"


//char buffer[4096];


//int last_requested = 0;


void command_string(char comm [50], int i);




//      #include <process.h>





char invent(int item_class_inv, int inv_quant [52], unsigned char inv_dam [52], unsigned char inv_class [52], unsigned char inv_type [52], unsigned char inv_plus [52], char inv_ident [52], char item_wielded, char body_armour, char shield_armour, char cloak_armour, char head_armour, char hand_armour, char foot_armour, char ring [2])
{


//int hairy_egg [80] [70] [3];

   char st_pass [60];

   int i;
   char buffer[2400];
   int j;
   char lines = 0;
   unsigned char anything = 0;
   unsigned char strng [5] = "";
   char ft;

   char yps = 0;

   //clrscr();
//   for (i = 0; i <= 20; i++)
//      cprintf("Line #%d\r\n", i);
   gettext(35, 1, 80, 25, buffer);

   strcpy(st_pass, "");

   window(35, 1, 80, 25);

   clrscr();


//void inventory(int item_class_inv)

//{

int Inv_class2 [13];
int inv_count = 0;
unsigned char ki;

//int ickyo = 0;

for (i = 0; i < 13; i++)
{
        Inv_class2 [i] = 0;
}


//if (inv_no != 0)
//{

//strcpy(info [info_lines], " You are carrying"); info_lines++;

for (i = 0; i < 52; i++)
{
        if (inv_quant [i] != 0)
        {
//  if (inv_class [i] == 11) Inv_class2 [3] ++; else
                Inv_class2 [inv_class [i]] ++; // adds up number of each class in invent.
                inv_count++;
        }
}

if (inv_count == 0)
{
   cprintf("You aren't carrying anything.");
   if (getch() == 0) getch();
   goto putty;
   //return;
}

if (item_class_inv != -1)
{
for (i = 0; i < 13; i++)
{
        if (item_class_inv == 1 && i == 0) i++;
        if (item_class_inv == 0 && i == 11) i++;
// if (item_class_inv == 11 && i == 3) i++;
        if (item_class_inv != i) Inv_class2 [i] = 0;
}
}


//cprintf("hello!");
//abort();
//if (item_class_inv > 1) Inv_class2 [0] = 0;


if ((item_class_inv == -1 && inv_count > 0) | (item_class_inv != -1 && Inv_class2 [item_class_inv] > 0) | (item_class_inv == 1 && (Inv_class2 [0] > 0 | Inv_class2 [1] > 0)))// | (item_class_inv == 3 && (Inv_class2 [3] > 0 | Inv_class2 [11] > 0)))
{

//if (item_class_inv != 1) //this is so you can get the '?' invent from throw_it
//{
cprintf("  You are carrying");
lines++;
//info_lines++;
//}


for (i = 0; i < 13; i ++)
        {
        if (Inv_class2 [i] != 0)
        {
//      cprintf("\n\r");
        //lines++;
//      cprintf(" ");
        if (lines > 22) // && inv_count > 0)
        {
                //gotoxy(1,25);
                //cprintf("\n\r-more-");
                //ki = getch();
                //lines = 0;
                //clrscr();
                //gotoxy(1,1);
                gotoxy(1,25);
                cprintf("-more-");
                ki = getch();
                if (ki == 27)
                {
                        puttext(35, 1, 80, 25, buffer);
                        return 27;
                }
                if (ki >= 65 && ki < 123)
                {
                        puttext(35, 1, 80, 25, buffer);
                        return ki;
                }
                if (ki == 0) ki = getch();
                lines = 0;
                clrscr();
                gotoxy(1,1);
                anything = 0;

        }
        if (lines > 0) cprintf("\n\r ");
        textcolor(BLUE);
        switch(i)
        {
        case 0: cprintf("Hand weapons"); break;
        case 1: cprintf("Missiles"); break;
        case 2: cprintf("Armour"); break;
        case 3: cprintf("Magical devices"); break;
        case 4: cprintf("Comestibles"); break;
        case 5: cprintf("Books"); break;
        case 6: cprintf("Scrolls"); break;
        case 7: cprintf("Jewellery"); break;
        case 8: cprintf("Potions"); break;
        case 9: cprintf("Gems"); break;
   case 10: cprintf("Books"); break;
   case 11: cprintf("Magical staves"); break;
   case 12: cprintf("Orbs of Zot"); break;
   case 13: cprintf("Gems"); break;
        }
        textcolor(LIGHTGRAY);
        //cprintf("\n\r");
        lines++;

        for (j = 0; j < 52; j++)
        {

                if (lines > 23 && inv_count > 0)
                {
                        gotoxy(1,25);
                        cprintf("-more-");
                        ki = getch();
                        if (ki == 27)
                        {
                                puttext(35, 1, 80, 25, buffer);
                                return 27;
                        }
                        if (ki >= 65 && ki < 123)
                        {
                                puttext(35, 1, 80, 25, buffer);
                                return ki;
                        }
                        if (ki == 0) ki = getch();
                        lines = 0;
                        clrscr();
                        gotoxy(1,1);
                        anything = 0;
                }

                if (inv_class [j] == i && inv_quant [j] > 0)
                {

                        anything ++;
                        //strcpy(info [info_lines], " ");
                        //strncpy(info [info_lines], letters [j], 1);
                        //strcat(info [info_lines], " - ");
                        //printf(" ");
                        if (lines > 0) cprintf("\n\r");
                        lines++;
                                cprintf(" ");
                                if (j < 26) ft = (char) j + 97;
                                        else ft = (char) j  + 65 - 26;
                                //itoa(j, strng, 10);

                                strng [0] = ft;
                                //strng [0] += 49;
                                cprintf(" ");
                                cprintf(strng);
                                //cout << " " << ft;
                                //cprintf(" ");
                                //cprintf(ft);
                        cprintf(" - ");


                        //strcat(info [info_lines], item_name(inv_class [j], inv_type [j], inv_dam [j], inv_plus [j], inv_quant [j], inv_ident [j], 3));

   yps = wherey();

                        item_name(inv_class [j], inv_type [j], inv_dam [j], inv_plus [j], inv_quant [j], inv_ident [j], 3, st_pass);
                        //strcpy(st_pass, "Hello!");
                        cprintf(st_pass);

         inv_count --;

//                      if (inv_quant [j] != 1 && inv_class [j] != 6 && inv_class [j] != 8)
//                              strcat(info [info_lines], "s");


                        if (j == item_wielded) cprintf(" (weapon)");
                        if (j == body_armour | j == shield_armour | j == cloak_armour | j == head_armour | j == hand_armour | j == foot_armour) cprintf(" (worn)");
                        if (j == ring [0]) cprintf(" (left hand)");
                        if (j == ring [1]) cprintf(" (right hand)");

   if (wherey() != yps) lines++;


//                      cprintf("\n\r");

                       //       cprintf("\n\r");

                        //info_lines++;

                }
        } // end of j loop
        } // end of if inv_class2
} // end of i loop.
} else
        {
        if (item_class_inv == -1) cprintf("You aren't carrying anything.");
        else
        {
        if (item_class_inv == 0) cprintf("You aren't carrying any weapons.");
         else
         {
                if (item_class_inv == 1) cprintf("You aren't carrying any ammunition.");
                 else
                 {
                        cprintf("You aren't carrying any such object.");
                 }
         }
        anything++;
        }
        //info_lines ++;
        }
//} // end of if (inv_no)
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
                   puttext(35, 1, 80, 25, buffer);
                   return ki;
           }
           if (ki == 0) ki = getch();
           puttext(35, 1, 80, 25, buffer);
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
   putty : puttext(35, 1, 80, 25, buffer);

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

   char st_pass [50];

   int i;
   int j = 0;
   char buffer[4800];
   window(1, 1, 80, 20);
   gettext(1, 1, 80, 20, buffer);
   strcpy(st_pass, "");
   clrscr();

   for (i = 0; i < 5000; i ++)
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
    j ++;
   }
//   j ++;
   getty: getch();

   puttext(1, 1, 80, 20, buffer);

//   cprintf("xxxxxxxxxxxxx");
//   last_requested = 0;
return;
}


void command_string(char comm [50], int i)
{

switch(i)
{
// case 1: strcpy(comm, "a - adijsd"); break;
 case 10: strcpy(comm, "c - close a door"); break;
 case 20: strcpy(comm, "d - drop"); break;
 case 30: strcpy(comm, "e - eat"); break;
 case 40: strcpy(comm, "f - fire an appropriate thing"); break;
 case 50: strcpy(comm, "i - inventory"); break;
 case 60: strcpy(comm, "o - open a door"); break;
 case 70: strcpy(comm, "q - quaff a potion"); break;
 case 80: strcpy(comm, "r - read a scroll"); break;
 case 90: strcpy(comm, "s - wait"); break;
 case 100: strcpy(comm, "t - throw or shoot something"); break;
 case 110: strcpy(comm, "v - get version number"); break;
 case 120: strcpy(comm, "w - wield a weapon"); break;
 case 130: strcpy(comm, "x - look around you"); break;
 case 140: strcpy(comm, "z - zap a wand"); break;
 case 145: strcpy(comm, "I - Invoke power of wielded item"); break;
 case 150: strcpy(comm, "M - Memorise a spell"); break;
 case 160: strcpy(comm, "P - Put on a ring"); break;
 case 165: strcpy(comm, "Q - commit suicide (quit)"); break;
 case 170: strcpy(comm, "R - Remove a ring"); break;
 case 180: strcpy(comm, "S - Save your game and quit"); break;
 case 190: strcpy(comm, "T - Take off armour"); break;
 case 209: strcpy(comm, "V - Examine an identified item"); break;
 case 200: strcpy(comm, "W - Wear armour"); break;
 case 210: strcpy(comm, "X - show map of level"); break;
 case 220: strcpy(comm, "Z - cast a spell"); break;
 case 230: strcpy(comm, ". - wait"); break;
 case 240: strcpy(comm, ", - pick something up"); break;
 case 250: strcpy(comm, "< - go up a staircase"); break;
 case 260: strcpy(comm, "> - go down a staircase"); break;
 case 270: strcpy(comm, "; - see what items you're standing on"); break;
 case 280: strcpy(comm, "\\ - check your item knowledge"); break;
 case 290: strcpy(comm, "? - get this list of commands"); break;
 case 300: strcpy(comm, "Ctrl-P - see old messages"); break;
 case 310: strcpy(comm, "Shift & direction - long walk"); break;
 case 320: strcpy(comm, "Shift & 5 on keypad - rest 50 turns"); break;
 case 330: strcpy(comm, "Rogue movement keys also work"); break;
 case 340: strcpy(comm, "+ & - on map screen - fast scroll"); break;
/* case 350: strcpy(comm, "d - drop"); break;
 case 360: strcpy(comm, "e - eat"); break;
 case 370: strcpy(comm, "e - eat"); break;
 case 380: strcpy(comm, "d - drop"); break;
 case 390: strcpy(comm, "e - eat"); break;*/

 default: strcpy(comm, ""); break;
}

//last_requested = i;
}

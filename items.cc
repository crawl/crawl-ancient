#include "config.h"

#ifdef DOS
#include <conio.h>
#endif

#include <string.h>
#include <stdlib.h>

#include "externs.h"

#include "effects.h"
#include "fight.h"
#include "invent.h"
#include "itemname.h"
#include "it_use2.h"
#include "misc.h"
#include "monplace.h"
#include "mstruct.h"
#include "mutation.h"
#include "player.h"
#include "randart.h"
#include "religion.h"
#include "skills.h"
#include "stuff.h"



int add_item(int item_got, int it_quant);
void item_place(int item_drop_2, int x_plos, int y_plos, int quant_drop);
int conv_lett(int item_drop_3);


int last_item = ING;

extern unsigned char wield_change; /* defined in output.cc */

/*
Takes keyin as an argument because it will only display a long list of items
 if ; is pressed.
*/
void item_check(char keyin)
{
char item_show [50] [50];
char temp_quant [10];
char str_pass [50];

int counter = 0;
int counter_max = 0;
if (env[0].grid [you[0].x_pos] [you[0].y_pos] >= 69 && env[0].grid [you[0].x_pos] [you[0].y_pos] <= 200)
{
 if (env[0].grid [you[0].x_pos] [you[0].y_pos] >= 82 && env[0].grid [you[0].x_pos] [you[0].y_pos] <= 85)
 {
  if (env[0].grid [you[0].x_pos] [you[0].y_pos] == 85) mpr("There is a ladder leading down here.");
   else mpr("There is a staircase down here.");
 } else
 if (env[0].grid [you[0].x_pos] [you[0].y_pos] >= 86 && env[0].grid [you[0].x_pos] [you[0].y_pos] <= 89)
 {
  if (env[0].grid [you[0].x_pos] [you[0].y_pos] == 89) mpr("There is a ladder leading upwards here.");
   else mpr("There is a staircase up here.");
 } else
switch(env[0].grid [you[0].x_pos] [you[0].y_pos])
{
  case 69: mpr("There is a gateway to hell here."); break;
  case 80: mpr("There is an entrance to a shop here."); break;
  case 81: mpr("There is an entrance to a labyrinth here.");
  mpr("Beware, for starvation awaits!"); break;
  case 92: mpr("There is a gateway to the Iron City of Dis here."); break;
  case 93: mpr("There is a gateway to Gehenna here."); break;
  case 94: mpr("There is a gateway to the frozen wastes of Cocytus here."); break;
  case 95: mpr("There is a gateway to Tartarus here."); break;
  case 96: mpr("There is a one-way gate to the infinite horrors of the Abyss here."); break;
  case 97: mpr("There is a gateway leading out of the Abyss here."); break;
  case 98: mpr("There is an empty stone archway here."); break;
  case 99: mpr("There is a gate leading to the halls of Pandemonium here."); break;
  case 100: mpr("There is a gate leading out of Pandemonium here."); break;
  case 101: mpr("There is a gate leading to another region of Pandemonium here."); break;

  case 110: mpr("There is a staircase to the orcish mines here."); break;
  case 111: mpr("There is a staircase to the Hive here."); break;
  case 112: mpr("There is a staircase to the Lair here."); break;
  case 113: mpr("There is a staircase to the Slime Pits here."); break;
  case 114: mpr("There is a staircase to the Vaults here."); break;
  case 115: mpr("There is a staircase to the Crypt here."); break;
  case 116: mpr("There is a staircase to the Hall of Blades here."); break;
  case 117: mpr("There is a gate to the Realm of Zot here."); break;
  case 118: mpr("There is a staircase to the Ecumenical Temple here."); break;
  case 119: mpr("There is a staircase to the Snake Pit here."); break;
  case 120: mpr("There is a staircase to the Elven Halls here."); break;
  case 121: mpr("There is a staircase to the Tomb here."); break;
  case 122: mpr("There is a staircase to the Swamp here."); break;
  case 130:
  case 131:
  case 134:
  case 138:
  case 132: mpr("There is a staircase back to the Dungeon here."); break;
  case 142: mpr("There is a staircase back to the Mines here."); break;
  case 133: mpr("There is a staircase back to the Lair here."); break;
  case 135: mpr("There is a staircase back to the Vaults here."); break;
  case 141:
  case 136: mpr("There is a staircase back to the Crypt here."); break;
  case 139: mpr("There is a staircase back to the Lair here."); break;
  case 140: mpr("There is a staircase back to the Mines here."); break;
  case 137: mpr("There is a gate leading back out of this place here."); break;

  case 180: mpr("There is a glowing white marble altar of Zin here."); break;
  case 181: mpr("There is a glowing golden altar of the Shining One here."); break;
  case 182: mpr("There is an ancient bone altar of Kikubaaqudgha here."); break;
//  case 183:
  case 184: mpr("There is a shimmering altar of Xom here."); break;
  case 185: mpr("There is a shining altar of Vehumet here."); break;
// case 185
  case 186: mpr("There is an iron altar of Okawaru here."); break;
  case 187: mpr("There is a burning altar of Makhleb here."); break;
  case 188: mpr("There is a deep blue altar of Sif Muna here."); break;
  case 189: mpr("There is a bloodstained altar of Trog here."); break;
  case 190: mpr("There is a sparkling altar of Nemelex Xobeh here."); break;
  case 191: mpr("There is a silver altar of Elyvilon here."); break;

 }
}



if (env[0].igrid [you[0].x_pos] [you[0].y_pos] == ING)
{
 if (keyin == ';') mpr("There are no items here.");
 return;
}

int objl = env[0].igrid [you[0].x_pos] [you[0].y_pos];
int hrg = 0;

while(objl != 501)
{
                        counter++;

                        if (counter > 45)
                        {
                             strcpy(item_show [counter], "Too many items.");
                             break;
                        }
                        if (mitm.iclass [objl] == 15)
                        {
                         itoa(mitm.iquant [objl], temp_quant, 10);
                         strcpy(item_show [counter], temp_quant);
                         if (mitm.iquant [objl] > 1) strcat(item_show [counter], " gold pieces");
                          else strcat(item_show [counter], " gold piece");
                         goto linking; //continue;
                        }
                it_name(objl, 3, str_pass);
                strcpy(item_show [counter], str_pass);
linking : hrg = mitm.ilink [objl];
objl = hrg;
}

counter_max = counter;
counter = 0;


if (counter_max == 1)
{
        strcpy(info, "You see here "); // remember 'an'.
        strcat(info, item_show [counter_max]);
        strcat(info, ".");
        mpr(info);
        counter++;
        counter_max = 0; // to skip next part.
}


if ((counter_max > 0 && counter_max < 6) || (counter_max > 1 && keyin == 59))
{
        mpr("Things that are here:");
        while (counter < counter_max)
        {
                counter++; // this is before the strcpy because item_show start at 1, not 0.
                mpr(item_show [counter]);
        }
}

if (counter_max > 5 && keyin != 59)
        {
                mpr("There are several objects here.");
        }

}





void pickup(void)
{
//int gloggo = 0;
int items_here = 0;
int counter = 0;
//int counter_max = 0;
int item_got = 0;
int o = 0;
int k = 0;
int nothing = 0;
char str_pass [50];
char keyin;

if (you[0].lev != 0 && wearing_amulet(42) == 0)
{
        mpr("You can't reach the floor from up here.");
        return;
}

if (env[0].igrid [you[0].x_pos] [you[0].y_pos] == ING)
{
        mpr("There isn't anything here.");
        return;
}

last_item = ING;

int objl = env[0].igrid [you[0].x_pos] [you[0].y_pos];
int hrg = 0;

while(objl != ING)
{
                                counter++;

last_item = objl;

hrg = mitm.ilink [objl];
objl = hrg;
items_here++;

if (counter > 1000)
{
 mpr("Oh dear. There appears to be a bug in the program.");
 mpr("I suggest you leave this level then save as soon as possible.");
 return;
}

}

// Anywhere which calls add_item() doesn't need to arrange it[0].ilink etc.


if (items_here == 1)
{
                        item_got = env[0].igrid [you[0].x_pos] [you[0].y_pos];
                        last_item = ING;
                        nothing = add_item(item_got, mitm.iquant [env[0].igrid [you[0].x_pos] [you[0].y_pos]]);
                        if (nothing == ING)
                                {
                                        mpr("You can't carry that many items.");
                                } else
                                if (nothing != 1)
                                {
                                        mpr("You can't carry that much weight.");
                                }
                                return;

} // end of if items_here

last_item = ING;


if (items_here > 1)
{
        mpr("There are several objects here.");

        o = env[0].igrid [you[0].x_pos] [you[0].y_pos];

        for (k = 0; k < items_here; k++)
        {
                        if (keyin != 'a')
                        {
                         strcpy(info, "pick up ");

                         if (mitm.iclass [o] == 15)
                         {
                                itoa(mitm.iquant [o], st_prn, 10);
                                strcat(info, st_prn);
                                strcat(info, " gold piece");
                                if (mitm.iquant [o] > 1) strcat(info, "s");
                         } else
                         {
                                it_name(o, 3, str_pass);
                                strcat(info, str_pass);
                         }
                         strcat(info, "\? (y,n,a,q)");
                         mpr(info);
                        }

                if (keyin != 'a') keyin = get_ch();

                if (keyin == 'q')
                {
                        return;
                }
                if (keyin == 'y' || keyin == 'a')
                        {
                        item_got = o;
                        int grunk = add_item(o, mitm.iquant [o]);
                        if (grunk == 0)
                                {
                                        mpr("You can't carry that much weight.");
                                        keyin = 'x'; // resets from 'a'
                                }
                        if (grunk == ING)
                                {
                                        mpr("You can't carry that many items.");
                                        keyin = 'x'; // resets from 'a'
                                }


                                if (grunk != 1) // ie if the item picked up is still there.
                                {
                                        last_item = item_got;
                                }

                        }

                                if (mitm.iquant [o] > 0) last_item = o;

                                hrg = mitm.ilink [o];

                                if (mitm.iquant [o] == 0) mitm.ilink [o] = ING;

                                o = hrg;
                                if (o == ING) return;

        if (items_here == 0) break;
        } // end of while k loop

        mpr("That's all.");
} // end of if items_here

} // end of pickup() function























int add_item(int item_got, int quant_got)
{

/*int quant_got = 0; */
int item_mass = 0;
int unit_mass = 0;
int retval = 1;
char brek = 0;
//int last_item = ING;
int m = 0;
char str_pass [50];

if (you[0].inv_no >= 52)
{
 return ING;
}

if (mitm.iclass [item_got] < 3 || mitm.iclass [item_got] == 4 || mitm.iclass [item_got] == 13)
{
        unit_mass = mass(mitm.iclass [item_got], mitm.itype [item_got]);// * mitm.iquant [item_got];
} else
      switch(mitm.iclass [item_got])
       {
        case 3: unit_mass = 100; break;
        case 4: unit_mass = 100; break;
        case 5: unit_mass = 200; break;
        case 6: unit_mass = 50; break;
        case 7: unit_mass = 20; break;
        case 8: unit_mass = 60; break;
        case 9: unit_mass = 5; break;
 case 10: unit_mass = 250; break;
 case 11: unit_mass = 130; break;
 case 14:
 if (mitm.itype [item_got] == 0) unit_mass = mons_weight(mitm.iplus [item_got]);
 if (mitm.itype [item_got] == 1) unit_mass = mons_weight(mitm.iplus [item_got]) / 2;
 break;
        case 15: unit_mass = 0; break; // For now!
       }

      item_mass = unit_mass * mitm.iquant [item_got];

quant_got = mitm.iquant [item_got];

brek = 0;

if ((int) you[0].burden + item_mass > (int) 1000 + you[0].strength * 200 + (you[0].lev != 0) * 1000) // multiply both constants * 10
{
        if (mitm.iquant [item_got] == 1) return 0;

        for (m = mitm.iquant [item_got]; m > 0; m --)
        {
                if (m == 0) break;
                if (unit_mass * m + you[0].burden <= 1000 + you[0].strength * 200 + (you[0].lev != 0) * 1000)
                {
                        quant_got = m;
                        brek = 1;
                        item_mass = unit_mass * m;
                        mpr("You can only carry some of what is here.");
                        break;
                }
        }
        if (brek == 0) return 0;
                else retval = 2;

}

brek = 0;

if (mitm.iclass [item_got] == 15)
{
        you[0].gp += quant_got;
        you[0].gp_ch = 1;
        strcpy(info, "You pick up ");
        itoa(quant_got, st_prn, 10);
        strcat(info, st_prn);
        strcat(info, " gold pieces.");
        mpr(info);
        you[0].turnover = 1;
        alert();
        goto change_igrid;
}

for (m = 0; m < 52; m++)
{

        if ((mitm.iclass [item_got] == 1 || (mitm.iclass [item_got] == 4 && mitm.itype [item_got] != 21) || mitm.iclass [item_got] == 6 || mitm.iclass [item_got] == 8 || mitm.iclass [item_got] == 9) && you[0].inv_class [m] == mitm.iclass [item_got] && you[0].inv_type [m] == mitm.itype [item_got] && (((mitm.iclass [item_got] == 4 && mitm.itype [item_got] != 21) || mitm.iclass [item_got] == 6 || mitm.iclass [item_got] == 8) || (you[0].inv_plus [m] == mitm.iplus [item_got] && you[0].inv_plus2 [m] == mitm.iplus2 [item_got] && you[0].inv_dam [m] == mitm.idam [item_got])) && you[0].inv_quant [m] > 0)
        {
        if (mitm.iid [item_got] == you[0].inv_ident [m] || mitm.iclass [item_got] == 4 || mitm.iclass [item_got] == 6 || mitm.iclass [item_got] == 8)
        {
                you[0].inv_quant [m] += quant_got;//mitm.iquant [item_got];

                burden_change();

//              strcpy(info, " ");
/*                      strncpy(info, letters [m], 1);*/
                        if (m <= 25) info [0] = m + 97;
                                else info [0] = m + 39;

                        info [1] = 0;

                        strcat(info, " - ");

                        in_name(m, 3, str_pass);
                        strcat(info, str_pass);
                        mpr(info);

                you[0].turnover = 1;
  alert();
                goto change_igrid;

        }
        }
} // end of for m loop.

for (m = 0; m < 52; m++)
{


        if (you[0].inv_quant [m] == 0)
        {
                you[0].inv_ident [m] = mitm.iid [item_got];
                you[0].inv_class [m] = mitm.iclass [item_got];
                you[0].inv_type [m] = mitm.itype [item_got];
                you[0].inv_plus [m] = mitm.iplus [item_got];
                you[0].inv_plus2 [m] = mitm.iplus2 [item_got];
                you[0].inv_dam [m] = mitm.idam [item_got];
                you[0].inv_col [m] = mitm.icol [item_got];
                you[0].inv_quant [m] = quant_got;
                burden_change();

//                      strcpy(info, " ");
/*                      strncpy(info, letters [m], 1);*/
                        if (m <= 25) info [0] = m + 97;
                                else info [0] = m + 39;

                        info [1] = 0;

                        strcat(info, " - ");
        in_name(m, 3, str_pass);
        strcat(info, str_pass);

                        mpr(info);

if (mitm.iclass [item_got] == 12 && you[0].char_direction == 0)
{
 mpr("Now all you have to do is get back out of the dungeon!");
 you[0].char_direction = 1;
// env[0].grid [you[0].x_pos] [you[0].y_pos] = 137;
  // not in the abyss or pandemonium
}

                you[0].inv_no++; // increases number of items in inventory


              break;
        }




} // end of for m loopy thing.
you[0].turnover = 1;

change_igrid:


                mitm.iquant [item_got] -= quant_got;//= 0;

                if (mitm.iquant [item_got] == 0)
                {
                        if (last_item == ING)
                        {
                                env[0].igrid [you[0].x_pos] [you[0].y_pos] = mitm.ilink [item_got];
                        } else
                        {
                                mitm.ilink [last_item] = mitm.ilink [item_got];
                        }
/*              it_no --;*/
                }


return retval;

} // end of int add_item() function




void item_place(int item_drop_2, int x_plos, int y_plos, int quant_drop)
{

int item_mass = 0;
int unit_mass = 0;

if (you[0].inv_class [item_drop_2] < 3 || you[0].inv_class [item_drop_2] == 4 || you[0].inv_class [item_drop_2] == 13)
{
        unit_mass = mass(you[0].inv_class [item_drop_2], you[0].inv_type [item_drop_2]);
} else
       switch(you[0].inv_class [item_drop_2])
       {
        case 3: unit_mass = 100; break;
        case 4: unit_mass = 100; break;
        case 5: unit_mass = 200; break;
        case 6: unit_mass = 50; break;
        case 7: unit_mass = 20; break;
        case 8: unit_mass = 60; break;
        case 9: unit_mass = 5; break;
 case 10: unit_mass = 250; break;
 case 11: unit_mass = 130; break;
 case 13: unit_mass = 250; break;
 case 14:
/* if (mitm.itype [item_got] == 0) unit_mass = mons_weight(mitm.iplus [item_got]);
 if (mitm.itype [item_got] == 1) unit_mass = mons_weight(mitm.iplus [item_got]) / 2;*/
 if (mitm.itype [item_drop_2] == 0) unit_mass = mons_weight(mitm.iplus [item_drop_2]);
 if (mitm.itype [item_drop_2] == 1) unit_mass = mons_weight(mitm.iplus [item_drop_2]) / 2; break;
       }

       item_mass = unit_mass * quant_drop;

int m = 0, i = 0;


if (igrd [x_plos] [y_plos] != 501)
{
        if ((you[0].inv_class [item_drop_2] == 1 || you[0].inv_class [item_drop_2] == 4 || you[0].inv_class [item_drop_2] == 6 || you[0].inv_class [item_drop_2] == 8 || you[0].inv_class [item_drop_2] == 9) && you[0].inv_class [item_drop_2] == mitm.iclass [igrd [x_plos] [y_plos]] && you[0].inv_type [item_drop_2] == mitm.itype [igrd [x_plos] [y_plos] ] && you[0].inv_plus [item_drop_2] == mitm.iplus [igrd [x_plos] [y_plos]] && you[0].inv_plus2 [item_drop_2] == mitm.iplus2 [igrd [x_plos] [y_plos]] && you[0].inv_dam [item_drop_2] == mitm.idam [igrd [x_plos] [y_plos]] && mitm.iquant [igrd [x_plos] [y_plos]] > 0)
        {
        if (you[0].inv_ident [item_drop_2] == mitm.iid [igrd [x_plos] [y_plos] ])
        {
                mitm.iquant [igrd [x_plos] [y_plos]] += quant_drop;
                you[0].turnover = 1;
                return;
        }
        }
}


for (i = 0; i < ITEMS; i++)
{
 if (i >= 480)
 {
  strcpy(info, "The demon of the infinite void grins at you.");
  mpr(info);
  return;
 }
        if (mitm.iquant [i] == 0)
        {
                mitm.iid [i] = you[0].inv_ident [item_drop_2];
                mitm.iclass [i] = you[0].inv_class [item_drop_2];
                mitm.itype [i] = you[0].inv_type [item_drop_2];
                mitm.iplus [i] = you[0].inv_plus [item_drop_2];
                mitm.iplus2 [i] = you[0].inv_plus2 [item_drop_2];
                mitm.idam [i] = you[0].inv_dam [item_drop_2];
                mitm.icol [i] = you[0].inv_col [item_drop_2];
                mitm.iquant [i] = quant_drop;
/*              it_no ++;*/
                break;
        }
} // end of i loop


m = igrd [x_plos] [y_plos];

igrd [x_plos] [y_plos] = i;
mitm.ilink [i] = m;

you[0].turnover = 1;

} // end of item_place.



void drop(void)
{

unsigned char nthing;
int i;
unsigned char item_drop_1;
unsigned char item_drop_2;
char str_pass [80];

if (you[0].inv_no == 0)
        {
        strcpy(info, "You aren't carrying anything.");
        mpr(info);
        return;
        }

query2 : strcpy(info, "Drop which item?");
mpr(info);

unsigned char keyin = get_ch();
int quant_drop;
char temp_quant [10];

if (keyin == '$')
{

quant_drop = you[0].gp; /* needs quantity selection. */

if (you[0].gp == 0)
{
        strcpy(info, "You don't have any money.");
        mpr(info);
        return;
}

strcpy(info, "You drop ");
itoa(you[0].gp, temp_quant, 10);
strcat(info, temp_quant);

if (you[0].gp > 1) strcat(info, " gold pieces.");
            else strcat(info, " gold piece.");

mpr(info);

if (igrd [you[0].x_pos] [you[0].y_pos] != 501)
{
        if (mitm.iclass [igrd [you[0].x_pos] [you[0].y_pos]] == 15)
                {
                        mitm.iquant [igrd [you[0].x_pos] [you[0].y_pos]] += quant_drop;
                        you[0].gp -= quant_drop; you[0].
                        gp_ch = 1;
                        return;
                }
}


for (i = 0; i < ITEMS; i++)
{
        if (mitm.iquant [i] == 0)
        {
                mitm.iid [i] = 0;
                mitm.iclass [i] = 15;
                mitm.iquant [i] = quant_drop;
/*              it_no ++;*/
                break;
        }
} /* end of i loop */


int m = igrd [you[0].x_pos] [you[0].y_pos];

igrd [you[0].x_pos] [you[0].y_pos] = i;
mitm.ilink [i] = m;

you[0].gp -= quant_drop; you[0].gp_ch = 1;

return;

}


if (keyin == '?' || keyin == '*')
{
        nthing = get_invent(-1);
        if ((nthing >= 65 && nthing <= 90) || (nthing >= 97 && nthing <= 122))
        {
                keyin = nthing;
        } else
        {

           goto query2;

        }

}
item_drop_1 = (int) keyin;

quant_drop = 0;

if (item_drop_1 > 47 && item_drop_1 < 58)
{
        quant_drop = item_drop_1 - 48;
        keyin = get_ch();
        item_drop_1 = (int) keyin;
        if (item_drop_1 > 47 && item_drop_1 < 58)
                {
                quant_drop = (quant_drop * 10 + (item_drop_1 - 48));
                keyin = get_ch();
                item_drop_1 = (int) keyin;
                }
}

if ((item_drop_1 < 65 || (item_drop_1 > 90 && item_drop_1 < 97) || item_drop_1 > 122)) // && item_drop_1 != '$')
{
        strcpy(info, "You don't have any such object.");
        mpr(info);
        return;
}

        item_drop_2 = conv_lett(item_drop_1);
        if (quant_drop == 0) quant_drop = you[0].inv_quant [item_drop_2];


        if (you[0].inv_quant [item_drop_2] == 0)
        {
                strcpy(info, "You don't have any such object.");
                mpr(info);
                return;
        }
        for (i = 1; i < 7; i++)
        {
                if (item_drop_2 == you[0].equip [i] && you[0].equip [i] != -1)
                {
                        strcpy(info, "You will have to take that off first.");
                        mpr(info);
                        return;
                }
        }

        if (item_drop_2 == you[0].equip [7] || item_drop_2 == you[0].equip [8] || item_drop_2 == you[0].equip [9])
        {
                strcpy(info, "You will have to take that off first.");
                mpr(info);
                return;
        }

        if (item_drop_2 == you[0].equip [0] && you[0].inv_class [item_drop_2] == 0 && you[0].inv_plus [item_drop_2] >= 130)
        {
                strcpy(info, "That object is stuck to you!");
                mpr(info);
                return;
        }

        if (quant_drop > you[0].inv_quant [item_drop_2]) quant_drop = you[0].inv_quant [item_drop_2];

        strcpy(info, "You drop ");


        item_name(you[0].inv_plus2 [item_drop_2], you[0].inv_class [item_drop_2], you[0].inv_type [item_drop_2], you[0].inv_dam [item_drop_2], you[0].inv_plus [item_drop_2], quant_drop, you[0].inv_ident [item_drop_2], 3, str_pass);
        strcat(info, str_pass);

        strcat(info, ".");
        mpr(info);

        if (item_drop_2 == you[0].equip [0])
        {
        unwield_item(item_drop_2);
                you[0].equip [0] = -1;
                mpr("You are empty handed.");
        }

        item_place(item_drop_2, you[0].x_pos, you[0].y_pos, quant_drop);

        you[0].inv_quant [item_drop_2] -= quant_drop;
        if (you[0].inv_quant [item_drop_2] <= 0) you[0].inv_no--;


        burden_change();

} /* end of drop func. */


int conv_lett(int item_drop_3)
{
        if (item_drop_3 > 96 && item_drop_3 < 123)
        {
         item_drop_3 -= 97;
        }

        if (item_drop_3 > 64 && item_drop_3 < 91)
        {
         item_drop_3 -= 39;
        }

return item_drop_3;

} /* end of conv_lett func */






void manage_corpses(void) /* causes corpses to rot away */
{
/* first item, then inventory */
/* because it occurs periodically, this function is useful for things which
happen every now and then. */

/* Nasty things happen to people who spend too long in Hell: */

if (you[0].where_are_you > 0 && you[0].where_are_you < 10 && you[0].where_are_you != 3 && random2(3) == 0)
{
 switch(random2(10))
 {
  case 0: mpr("You hear diabolical laughter!"); break;
  case 1: mpr("\"Die, mortal!\""); break;
  case 2: mpr("\"Trespassers are not welcome here!\""); break;
  case 3: mpr("Twisted shapes form in the air around you."); break;
  case 4: mpr("You feel a terrible foreboding..."); break;
  case 5: mpr("You hear words spoken in a strange and terrible language..."); break;
  case 6: mpr("You smell brimstone."); break;
  case 7: mpr("Something frightening happens."); break;
 }
 create_monster(250, 0, 1, you[0].x_pos, you[0].y_pos, MHITYOU, 250);
 if (random2(3) == 0) create_monster(250, 0, 1, you[0].x_pos, you[0].y_pos, MHITYOU, 250);
 if (random2(3) == 0) create_monster(250, 0, 1, you[0].x_pos, you[0].y_pos, MHITYOU, 250);
 if (random2(3) == 0) create_monster(250, 0, 1, you[0].x_pos, you[0].y_pos, MHITYOU, 250);
 if (random2(3) == 0) create_monster(250, 0, 1, you[0].x_pos, you[0].y_pos, MHITYOU, 250);

/* mons_place(2500, 1, you[0].x_pos, you[0].y_pos, 0, MHITNOT, 250, you[0].your_level);
 if (random2(3) == 0) mons_place(2500, 0, 50, 50, 0, MHITNOT, 250, you[0].your_level);
 if (random2(3) == 0) mons_place(2500, 0, 50, 50, 0, MHITNOT, 250, you[0].your_level);
 if (random2(3) == 0) mons_place(2500, 0, 50, 50, 0, MHITNOT, 250, you[0].your_level);*/
}


if (you[0].disease == 0)
{

 if (you[0].strength < you[0].max_strength && random2(100) == 0)
 {
  mpr("You feel your strength returning.");
  you[0].strength ++;
  you[0].strength_ch = 1;
 }

 if (you[0].dex < you[0].max_dex && random2(100) == 0)
 {
  mpr("You feel your dexterity returning.");
  you[0].dex ++;
  you[0].dex_ch = 1;
 }

 if (you[0].intel < you[0].max_intel && random2(100) == 0)
 {
  mpr("You feel your intelligence returning.");
  you[0].intel ++;
  you[0].intel_ch = 1;
 }
} else
   {
    if (random2(30) == 0)
    {
     mpr("Your disease is taking its toll.");
     lose_stat(100, 1);
    }
   }


if (you[0].invis > 0 || (you[0].haste > 0 && you[0].berserker == 0))
{
 if (random2(2) == 0 && you[0].mpower < 100) you[0].mpower ++;
}

you[0].mpower += random2(scan_randarts(25) + 1); // mutagenic radiation

if (you[0].mpower > 0 && random2(2) == 0)
{
 if (you[0].mpower > 4 && random2(100) <= you[0].mpower)
 {
  mpr("You've accumulated too much magical radiation!");
  if (random2(2) == 0) mutate(100); else give_bad_mutation();
 }
 you[0].mpower --;
}

/*
case 1: return "Zin";
case 2: return "The Shining One";
case 3: return "Kikubaaqudgha";
case 4: return "Yredelemnul";
case 5: return "Xom";
case 6: return "Vehumet";
case 7: return "Okawaru";
case 8: return "Makhleb";
case 9: return "Sif Muna";
case 10: return "Trog";
case 11: return "Nemelex Xobeh";
case 12: return "Elyvilon";
*/

if (you[0].religion != 0)
{
 switch(you[0].religion)
 {
  case 5:
  if (random2(75) == 0) Xom_acts(1, you[0].xl + random2(15), 1);
  break;

  case 1: /* These gods like long-standing worshippers */
  case 2:
  case 12:
  if (random2(20) == 0 && you[0].piety < 100) gain_piety(1);
  break;

  case 3: /* These gods require constant appeasement */
  case 6: /* Vehumet */
  case 7:
  case 10:
  if (random2(9) == 0) lose_piety(1);
  if (you[0].piety <= 0) excommunication();
  break;

  case 8:
  if (random2(7) == 0) lose_piety(1);
  if (you[0].piety <= 0) excommunication();
  break;

  case 9: /* Sif Muna */
  if (random2(10) == 0) lose_piety(1);
  if (you[0].piety <= 0) excommunication();
  break;

 }
}


int c = 0;
int rotted = 0;

for (c = 0; c < ITEMS; c ++)
{
 if (mitm.iquant [c] <= 0) continue;
 if (mitm.iclass [c] != 14 && mitm.iclass [c] != 4) continue;
 if (mitm.iclass [c] == 14 && mitm.itype [c] > 1) continue;
 if (mitm.iclass [c] == 4 && mitm.itype [c] != 21) continue;
 rotted = you[0].time_taken;
 if (rotted >= mitm.idam [c])
 {
   if (mitm.iclass [c] == 4)
   {
    destroy_item(c);
    continue;
   }
   if (mitm.itype [c] == 1 || mons_skeleton(mitm.iplus [c]) == 0)
   {
    destroy_item(c);
    continue;
   }
   mitm.itype [c] = 1;
   mitm.idam [c] = 200;
   mitm.icol [c] = LIGHTGREY;
   continue;
 }
 mitm.idam [c] -= rotted;
} // end for c

for (c = 0; c < 52; c ++)
{
 if (you[0].inv_quant [c] <= 0) continue;
 if (you[0].inv_class [c] != 14 && you[0].inv_class [c] != 4) continue;
 if (you[0].inv_class [c] == 14 && you[0].inv_type [c] > 1) continue;
 if (you[0].inv_class [c] == 4 && you[0].inv_type [c] != 21) continue;
 rotted = you[0].time_taken;
 if (rotted >= you[0].inv_dam [c])
 {
   if (you[0].inv_class [c] == 4)
   {
    if (you[0].equip [0] == c)
    {
     unwield_item(you[0].equip [0]);
     you[0].equip [0] = -1;
    }
    you[0].inv_quant [c] = 0;
    burden_change();
    you[0].inv_no --;
    continue;
   }
   if (you[0].inv_type [c] == 1)
   {
    // carried skeletons are not destroyed
    continue;
   }
   if (mons_skeleton(you[0].inv_plus [c]) == 0)
   {
    if (you[0].equip [0] == c)
    {
     unwield_item(you[0].equip [0]);
     you[0].equip [0] = -1;
    }
    you[0].inv_quant [c] = 0;
    burden_change();
    you[0].inv_no --;
    continue;
   }
   you[0].inv_type [c] = 1;
   you[0].inv_dam [c] = 0;
   you[0].inv_col [c] = LIGHTGREY;
   wield_change = 1;
   continue;
 }
 you[0].inv_dam [c] -= rotted;
} // end for c

if (you[0].equip [6] == -1) goto practise_stealth;
if (you[0].inv_type [you[0].equip [6]] < 2) goto practise_stealth;
if (random() % 1000 <= mass(2, you[0].inv_type [you[0].equip [6]]) && random() % 7 == 0)
 exercise(13, 1);


// also skills:
practise_stealth : if (you[0].burden_state != 0) return;

if (you[0].equip [6] != -1)
{
 if (you[0].inv_dam [you[0].equip [6]] / 30 != 4) /* elven armours don't hamper stealth */
  if (you[0].inv_type [you[0].equip [6]] > 1 && (you[0].inv_type [you[0].equip [6]] < 22 || you[0].inv_type [you[0].equip [6]] > 25))
     if (random() % mass(2, you[0].inv_type [you[0].equip [6]]) >= 100 || random() % 3 != 0) return;
}

//if (you[0].lev != 0) return; // can't really practise stealth while floating, and an amulet of control flight shouldn't make much difference

if (you[0].special_wield == 50) return; // shadow lantern

if (random() % 6 == 0) exercise(15, 1);


} // end manage_corpses

#include "config.h"

#ifdef DOS
#include <conio.h>
#endif
#include <string.h>

//#include <stdlib.h> only needed for itoa

#include "externs.h"
#include "spells2.h"
#include "macro.h"
#include "mutation.h"
#include "player.h"
#include "religion.h"
#include "stuff.h"
#include "misc.h"
#include "itemname.h"
#include "mstruct.h"

/*
IMPORTANT: There is also an eating bit in item_use.cc, so alter that with
carnivores and things if it's changed here.
*/


void eat_meat(int fod_eat_2);
char eat_from_floor(void);
char butchery(void);
int eating(int func_pass [10], unsigned char item_class, int food_eat_3);


void food_change(void)
{
you[0].hung_state = 0;

if (you[0].hunger <= 1000)
{
 you[0].hung_state = 1; // starving
 return;
}
if (you[0].hunger <= 2600)
{
 you[0].hung_state = 2; // hungry
 return;
}
if (you[0].hunger < 7000)
{
 you[0].hung_state = 3; // -
 return;
}
if (you[0].hunger < 11000)
{
 you[0].hung_state = 4; // Full
 return;
}
you[0].hung_state = 5; // Engorged

} /* end of void food_change() */


void eaten(int food_eat_2, int food_eat_3) /* this contains results of eating things. */
{

you[0].hung_ch = 1;


if (food_eat_3 == 21)
{
 if (you[0].inv_dam [food_eat_2] < 100 && (mons_corpse_thingy(you[0].inv_plus [food_eat_2]) == 1 || mons_corpse_thingy(you[0].inv_plus [food_eat_2]) == 2) || (mons_corpse_thingy(you[0].inv_plus [food_eat_2]) == 3 && player_res_poison() != 0)) eat_meat(50);
  else eat_meat(mons_corpse_thingy(you[0].inv_plus [food_eat_2]));
 you[0].hung_ch = 1;
 return;
}

int func_pass [10];

func_pass [0] = you[0].hunger;
func_pass [1] = you[0].delay_t;
func_pass [2] = you[0].delay_doing;


eating(func_pass, 4, food_eat_3);

you[0].hunger = func_pass [0];
you[0].delay_t = func_pass [1];
you[0].delay_doing = func_pass [2];


//mpr(info);

if (food_eat_3 == 6)
{
 restore_str();
 restore_int();
 restore_dex();
}

} /* end of void eaten () */





char eat_from_floor(void)
{
int gloggj;
int o = igrd [you[0].x_pos] [you[0].y_pos];
int k;
unsigned char keyin;
//gloggo = 0;
int items_here = 0;
int item_got = 0;

if (you[0].lev != 0 && wearing_amulet(42) == 0)
{
//      strcpy(info, "You can't reach the floor from up here.");
//      mpr(info);
        return 0;
}


int last_item = 501;

int objl = igrd [you[0].x_pos] [you[0].y_pos];
int hrg = 0;
int counter = 0;

while(objl != 501)
{
                                counter++;

last_item = objl;

hrg = mitm.ilink [objl];
objl = hrg;
items_here++;

if (counter > 1000)
{
 strcpy(info, "Oh dear. There appears to be a bug in the program.");
 mpr(info);
 strcpy(info, "I suggest you leave this level then save as soon as possible.");
 mpr(info);
 return 0;
}

}



if (items_here == 1)
{
   if (mitm.iclass [igrd [you[0].x_pos] [you[0].y_pos]] != 4) goto out_of_eating;// && mitm.iclass [igrd [you[0].x_pos] [you[0].y_pos]] != 14) return 0;
                        strcpy(info, "Eat ");
   if (mitm.iquant [igrd [you[0].x_pos] [you[0].y_pos]] > 1) strcat(info, "one of ");
                        it_name(igrd [you[0].x_pos] [you[0].y_pos], 3, str_pass);
                        strcat(info, str_pass);
                        strcat(info, "\?");
                        mpr(info);
   unsigned char keyin = getch();
   if (keyin == 0)
   {
     getch();
     keyin = 0;
   }
   if (keyin != 'y' && keyin != 'Y') return 0;

                        int item_got = igrd [you[0].x_pos] [you[0].y_pos];
                        last_item = 501;
                        you[0].hung_ch = 1;


   if (you[0].species == 11 || you[0].mutation [10] == 3) // kobold
   {
    if ((mitm.itype [item_got] >= 1 && mitm.itype [item_got] <= 4) || (mitm.itype [item_got] >= 7 && mitm.itype [item_got] <= 17))
     {
      strcpy(info, "Sorry, you're a carnivore.");
      mpr(info);
      return 0;
     }
   }

   if (mitm.itype [item_got] == 21)
   {
    if (you[0].mutation [11] > 1)
    {
     mpr("You can't eat raw meat!");
     return 0;
    }
    if (you[0].hung_state > 2 && wearing_amulet(40) == 0 && you[0].species != 11 && you[0].species != 15 && you[0].species != 16 && you[0].mutation [10] == 0)
    {
     strcpy(info, "You aren't quite hungry enough to eat that!");
     mpr(info);
     return 0;
    }

/*    itoa(mons_corpse_thingy(mitm.iplus [item_got]), st_prn, 10);
    strcpy(info, st_prn);
    mpr(info);*/

//   if ((mitm.idam [item_got] < 100 && mons_corpse_thingy(mitm.iplus [item_got]) == 1) || mons_corpse_thingy(mitm.iplus [item_got]) == 2 || (mons_corpse_thingy(mitm.iplus [item_got]) == 3 && player_res_poison() != 0)) eat_meat(50);
   if (mitm.idam [item_got] < 100 && (mons_corpse_thingy(mitm.iplus [item_got]) == 1 || mons_corpse_thingy(mitm.iplus [item_got]) == 2) || (mons_corpse_thingy(mitm.iplus [item_got]) == 3 && player_res_poison() != 0)) eat_meat(50);
    else eat_meat(mons_corpse_thingy(mitm.iplus [item_got]));
//    eat_meat(mons_corpse_thingy(mitm.iplus [item_got]));
    goto out_of_eating3;
   }
   int func_pass [10];
   func_pass [0] = you[0].hunger;
   func_pass [1] = you[0].delay_t;
   func_pass [2] = you[0].delay_doing;
   gloggj = eating(func_pass, mitm.iclass [item_got], mitm.itype [item_got]);

   you[0].hunger = func_pass [0];
   if (you[0].hunger > 12000) you[0].hunger = 12000;
   you[0].delay_t = func_pass [1];
   you[0].delay_doing = func_pass [2];

//   mpr(info);
   if (mitm.itype [item_got] == 6)
   {
    restore_str();
    restore_int();
    restore_dex();
   }
//   more();
   out_of_eating3 : you[0].turnover = 1;
   you[0].hung_ch = 1;
   food_change();
   if (mitm.iquant [item_got] <= 1) destroy_item(item_got);
    else mitm.iquant [item_got] --;
                        return 1;

} // end of if items_here


last_item = 501;


if (items_here > 1)
{

        o = igrd [you[0].x_pos] [you[0].y_pos];

        for (k = 0; k < items_here; k++) // use k because of call to relay_message()
        {
   if (mitm.iclass [o] != 4) goto out_of_eating; // && mitm.iclass [o] != 14) goto out_of_eating;
                        strcpy(info, "Eat ");
   if (mitm.iquant [o] > 1) strcat(info, "one of ");
                        it_name(o, 3, str_pass);
                        strcat(info, str_pass);
                        strcat(info, "\?");
                        mpr(info);
   keyin = getch();
   if (keyin == 0)
   {
     getch();
     keyin = 0;
   }

                if (keyin == 'q')
                {
                        return 0;
                }
                if (keyin == 'y')
                        {

                        item_got = o;

   if (you[0].species == 11 || you[0].mutation [10] == 3) // kobold
   {
    if ((mitm.itype [item_got] >= 1 && mitm.itype [item_got] <= 4) || (mitm.itype [item_got] >= 7 && mitm.itype [item_got] <= 17))
     {
      strcpy(info, "Sorry, you're a carnivore.");
      mpr(info);
      return 0;
     }
   }

   if (mitm.itype [item_got] == 21)
   {
    if (you[0].mutation [11] > 1)
    {
     mpr("You can't eat raw meat!");
     return 0;
    }
    if (you[0].hung_state > 2 && wearing_amulet(40) == 0 && you[0].species != 11 && you[0].species != 15 && you[0].species != 16 && you[0].mutation [10] == 0)
    {
     strcpy(info, "You aren't quite hungry enough to eat that!");
     mpr(info);
                                        return 0;
    }
   if (mitm.idam [item_got] < 100 && (mons_corpse_thingy(mitm.iplus [item_got]) == 1 || mons_corpse_thingy(mitm.iplus [item_got]) == 2) || (mons_corpse_thingy(mitm.iplus [item_got]) == 3 && player_res_poison() != 0)) eat_meat(50);
    else eat_meat(mons_corpse_thingy(mitm.iplus [item_got]));
//    eat_meat(mons_corpse_thingy(mitm.iplus [item_got]));
    goto out_of_eating2;
   }
   int func_pass [10];
   func_pass [0] = you[0].hunger;
   if (you[0].hunger > 12000) you[0].hunger = 12000;
   func_pass [1] = you[0].delay_t;
   func_pass [2] = you[0].delay_doing;
   gloggj = eating(func_pass, mitm.iclass [item_got], mitm.itype [item_got]);

   you[0].hunger = func_pass [0];
   you[0].delay_t = func_pass [1];
   you[0].delay_doing = func_pass [2];

//   mpr(info);
   if (mitm.itype [item_got] == 6)
   {
    restore_str();
    restore_int();
    restore_dex();
   }

   out_of_eating2 : food_change();
   you[0].hung_ch = 1;
//   more();
   you[0].turnover = 1;
   if (mitm.iquant [item_got] == 1) destroy_item(item_got);
     else mitm.iquant [item_got] --;
   return 1;
//   destroy_item(item_got);


//                              if (grunk != 1) // ie if the item picked up is still there.
//                              {
//                                      last_item = item_got;
//                              }

                        }


                                out_of_eating : if (mitm.iquant [o] > 0) last_item = o;

                                hrg = mitm.ilink [o];
                                o = hrg;
                                if (o == 501) return 0;

        if (items_here == 0) break;
        } // end of while k loop

//      strcpy(info, "That's all.");
} // end of if items_here
//itc = 0;

return 0;
} // end of eat_from_floor() function





















char butchery(void)
{

//gloggo = 0;
int items_here = 0;
int o, k, item_got;
unsigned char keyin;

if (igrd [you[0].x_pos] [you[0].y_pos] == 501)
{
        strcpy(info, "There isn't anything here!");
        mpr(info);
        return 0;
}


if (you[0].lev != 0 && wearing_amulet(42) == 0)
{
        strcpy(info, "You can't reach the floor from up here.");
        mpr(info);
        return 0;
}

if (you[0].equip [0] == -1 && you[0].species != 16 && you[0].attribute [5] != 2 && you[0].attribute [5] != 5)
{
 strcpy(info, "What, with your bare hands?");
 mpr(info);
 return 0;
}

if (you[0].species != 16  && you[0].attribute [5] != 2 && you[0].attribute [5] != 5 && damage_type(you[0].inv_class [you[0].equip [0]], you[0].inv_type [you[0].equip [0]]) != 1 && damage_type(you[0].inv_class [you[0].equip [0]], you[0].inv_type [you[0].equip [0]]) != 3)
{
 strcpy(info, "Maybe you should try using a sharper implement.");
 mpr(info);
 return 0;
}


int last_item = 501;

int objl = igrd [you[0].x_pos] [you[0].y_pos];
int hrg = 0;
int counter = 0;

while(objl != 501)
{
                                counter++;

last_item = objl;

hrg = mitm.ilink [objl];
objl = hrg;
items_here++;

if (counter > 1000)
{
 strcpy(info, "Oh dear. There appears to be a bug in the program.");
 mpr(info);
 strcpy(info, "I suggest you leave this level then save as soon as possible.");
 mpr(info);
 return 0;
}

}



if (items_here == 1)
{
   if (mitm.iclass [igrd [you[0].x_pos] [you[0].y_pos]] != 14 || mitm.itype [igrd [you[0].x_pos] [you[0].y_pos]] != 0) goto out_of_eating;// && mitm.iclass [igrd [you[0].x_pos] [you[0].y_pos]] != 14) return 0;
                        strcpy(info, "Butcher ");
                        it_name(igrd [you[0].x_pos] [you[0].y_pos], 3, str_pass);
                        strcat(info, str_pass);
                        strcat(info, "\?");
                        mpr(info);
   unsigned char keyin = getch();
   if (keyin == 0)
   {
     getch();
     keyin = 0;
   }
   if (keyin != 'y' && keyin != 'Y') return 0;

                        int item_got = igrd [you[0].x_pos] [you[0].y_pos];
                        last_item = 501;
   you[0].hung_ch = 1;

   you[0].delay_t = 4;
   you[0].delay_doing = 4;

   if (you[0].species == 16 || you[0].attribute [5] == 2 || you[0].attribute [5] == 5) mpr("You start tearing the corpse apart.");
    else mpr("You start hacking away.");

   if (you[0].duration [3] != 0 && (you[0].religion == 3 || you[0].religion == 8 || you[0].religion == 10))
   {
    offer_corpse(item_got);
    you[0].turnover = 1;
    destroy_item(item_got);
    you[0].delay_t = 0;
    you[0].delay_doing = 0;
    return 1; /* no chunks */
   }

   you[0].turnover = 1;
   place_chunks(mitm.iplus [item_got], mitm.idam [item_got], you[0].x_pos, you[0].y_pos, mitm.icol [item_got]);
   destroy_item(item_got);
                        return 1;

} // end of if items_here


last_item = 501;


if (items_here > 1)
{

        o = igrd [you[0].x_pos] [you[0].y_pos];

        for (k = 0; k < items_here; k++) // use k because of call to relay_message()
        {
   if (mitm.iclass [o] != 14 || mitm.itype [o] != 0) goto out_of_eating; // && mitm.iclass [o] != 14) goto out_of_eating;
                        strcpy(info, "Butcher ");
                        it_name(o, 3, str_pass);
                        strcat(info, str_pass);
                        strcat(info, "\?");
                        mpr(info);
   keyin = getch();
   if (keyin == 0)
   {
     getch();
     keyin = 0;
   }

                if (keyin == 'q')
                {
                        return 0;
                }
                if (keyin == 'y')
                        {

                        item_got = o;

   you[0].delay_t = 4;
   you[0].delay_doing = 4;

   if (you[0].species == 16 || you[0].attribute [5] == 2 || you[0].attribute [5] == 5) mpr("You start tearing the corpse apart.");
    else mpr("You start hacking away.");

   if (you[0].duration [3] != 0 && (you[0].religion == 3 || you[0].religion == 8 || you[0].religion == 10))
   {
    offer_corpse(item_got);
    you[0].turnover = 1;
    destroy_item(item_got);
    you[0].delay_t = 0;
    you[0].delay_doing = 0;
    return 1; /* no chunks */
   }

   place_chunks(mitm.iplus [item_got], mitm.idam [item_got], you[0].x_pos, you[0].y_pos, mitm.icol [item_got]);
   you[0].turnover = 1;
   destroy_item(item_got);
   return 1;

                        }


                                out_of_eating : if (mitm.iquant [o] > 0) last_item = o;

                                hrg = mitm.ilink [o];
                                o = hrg;
                                if (o == 501) goto failed;

        if (items_here == 0) break;
        } // end of while k loop

//      strcpy(info, "That's all.");
} // end of if items_here
//itc = 0;

//return 0;

failed:
strcpy(info, "There isn't anything to dissect here.");
mpr(info);
return 0;

} // end of eat_from_floor() function








void eat_meat(int fod_eat_2)
{

 if (wearing_amulet(40) == 1 && (fod_eat_2 == 2 || fod_eat_2 == 3)) fod_eat_2 = 1;

 switch(fod_eat_2)
 {
  case 1:
  appetising :
  if (you[0].species == 10 || you[0].species == 11 || you[0].species == 15 || you[0].species == 16 || you[0].mutation [10] > 0)
   mpr("This raw flesh tastes good.");
    else
     mpr("This raw flesh is not very appetising.");
  you[0].hunger += 1000;
  you[0].delay_t = 3;
  you[0].delay_doing = 0;
  break;

  case 2:
  if (random2(3) != 0) goto appetising;
  case 50:
  if ((you[0].species == 11 || you[0].species == 16) && random2(15) != 0) goto appetising; // kobold, troll
  if ((you[0].species == 10  || you[0].species == 15) && random2(5) != 0) goto appetising; // orc, ogre
  if (you[0].is_undead != 0) goto appetising;
  strcpy(info, "There is something wrong with this meat.");
  mpr(info);
  strcpy(info, "You feel ill.");
  mpr(info);
  if (you[0].disease > 100) you[0].disease = 210; else you[0].disease += 50 + random2(100);
  break;

  case 3:
  if (player_res_poison() != 0) goto appetising;
  strcpy(info, "Yeeuch - this meat is poisonous!");
  mpr(info);
  you[0].poison += 3 + random2(4);
  break;

  case 4:
  if (you[0].is_undead != 0) goto appetising;

        strcpy(info, "You feel your flesh start to rot away!");
        mpr(info);
        you[0].rotting += random2(10) + 10;
  if (you[0].disease > 100) you[0].disease = 210; else you[0].disease += 50 + random2(100);
  break;

  case 5:
  mpr("This meat tastes really weird.");
  mutate(100);
  break;
 }

food_change();
}



int eating(int func_pass [10], unsigned char item_class, int food_eat_3)
{

if (item_class == 4)
{
switch(food_eat_3)
{
        case 0:
        mpr("That meat ration really hit the spot!");
        func_pass [0] += 5000;
        func_pass [0] += you[0].mutation [10] * 500;
        func_pass [0] -= you[0].mutation [11] * 1500; /* should never be 3 * 2000 */
        func_pass [1] = 4;
        func_pass [2] = 0;
        //hung_ch = 1;
        return 0;

        case 1:
        if (you[0].mutation [10] > 0) mpr ("Blech - you need meat!");
         else mpr("That bread ration really hit the spot!");
        func_pass [0] -= you[0].mutation [10] * 1000;
        func_pass [0] += you[0].mutation [11] * 500;
        func_pass [0] += 4400;
        func_pass [1] = 4;
        func_pass [2] = 0;
        return 0;

        case 3: // apple
        if (you[0].mutation [10] > 0) mpr ("Blech - you need meat!");
         else mpr("Mmmm... Yummy apple.");
        func_pass [0] += 700;
        func_pass [0] -= you[0].mutation [10] * 200;
        func_pass [0] += you[0].mutation [11] * 200;
        func_pass [1] = 1;
        func_pass [2] = 0;
        //hung_ch = 1;
        return 0;

        case 2: // pear
        if (you[0].mutation [10] > 0) mpr ("Blech - you need meat!");
         else mpr("Mmmm... Yummy pear.");
        //incrl();
        //hung += 300;
        //delay_t = 1;
        //delay_doing = 0;
        func_pass [0] += 700;
        func_pass [0] -= you[0].mutation [10] * 200;
        func_pass [0] += you[0].mutation [11] * 200;
        func_pass [1] = 1;
        func_pass [2] = 0;
        //hung_ch = 1;
        return 0;

        case 4:
        if (you[0].mutation [10] > 0) mpr ("Blech - you need meat!");
         else   mpr("That choko was very bland.");
        //incrl();
        //hung += 300;
        //delay_t = 1;
        //delay_doing = 0;
        func_pass [0] += 600;
        func_pass [0] -= you[0].mutation [10] * 200;
        func_pass [0] += you[0].mutation [11] * 200;
        func_pass [1] = 1;
        func_pass [2] = 0;
        //hung_ch = 1;
        return 0;

        case 5:
        mpr("That honeycomb was delicious.");
        func_pass [0] += 2000;
        func_pass [1] = 1;
        func_pass [2] = 0;
        return 0;

        case 6:
        mpr("That royal jelly was delicious!");
        func_pass [0] += 5000;
        func_pass [1] = 1;
        func_pass [2] = 0;
        return 0;

        case 7: // maybe a nasty side-effect from RD's book?
        if (you[0].mutation [10] > 0) mpr ("Blech - you need meat!");
         else mpr("That snozzcumber tasted truly putrid!");
        func_pass [0] += 1500;
        func_pass [0] -= you[0].mutation [10] * 200;
        func_pass [0] += you[0].mutation [11] * 200;
        func_pass [1] = 1;
        func_pass [2] = 0;
        return 0;

        case 8:
        strcpy(info, "Mmm... ");
 switch(random2(9))
        {
  case 0: strcat(info, "Ham and pineapple."); break;
  case 1: strcat(info, "Extra thick crust."); break;
  case 2: strcat(info, "Vegetable."); break;
  case 3: strcat(info, "Pepperoni."); break;
  case 4: strcpy(info, "Yeuchh - Anchovies!"); break;
  case 5: strcat(info, "Cheesy."); break;
  case 6: strcat(info, "Supreme."); break;
  case 7: strcat(info, "Super Supreme!"); break;
  case 8: strcat(info, "Chicken."); break;
        }
        mpr(info);
        func_pass [0] += 1500;
        func_pass [1] = 1;
        func_pass [2] = 0;
        return 0;

        case 9:
        if (you[0].mutation [10] > 0) mpr ("Blech - you need meat!");
         else mpr("That apricot was delicious!");
        func_pass [0] += 700;
        func_pass [0] -= you[0].mutation [10] * 200;
        func_pass [0] += you[0].mutation [11] * 200;
        func_pass [1] = 1;
        func_pass [2] = 0;
        return 0;

        case 10:
        if (you[0].mutation [10] > 0) mpr ("Blech - you need meat!");
         else   {       mpr("That orange was delicious!");
                        if (random2(8) == 0) strcat(info, " Even the peel tasted good!");
                }
        func_pass [0] += 1000;
        func_pass [0] -= you[0].mutation [10] * 300;
        func_pass [0] += you[0].mutation [11] * 300;
        func_pass [1] = 1;
        func_pass [2] = 0;
        return 0;

        case 11:
        if (you[0].mutation [10] > 0) mpr ("Blech - you need meat!");
         else {mpr("That banana was delicious!");
 if (random2(8) == 0) strcat(info, " Even the peel tasted good!");
              }
        func_pass [0] += 1000;
        func_pass [0] -= you[0].mutation [10] * 300;
        func_pass [0] += you[0].mutation [11] * 300;
        func_pass [1] = 1;
        func_pass [2] = 0;
        return 0;

        case 12:
        if (you[0].mutation [10] > 0) mpr ("Blech - you need meat!");
         else   mpr("That strawberry was delicious!");
        func_pass [0] += 200;
        func_pass [0] -= you[0].mutation [10] * 50;
        func_pass [0] += you[0].mutation [11] * 50;
        func_pass [1] = 1;
        func_pass [2] = 0;
        return 0;

        case 13:
        if (you[0].mutation [10] > 0) mpr ("Blech - you need meat!");
         else   mpr("That rambutan was delicious!");
        func_pass [0] += 600;
        func_pass [0] -= you[0].mutation [10] * 200;
        func_pass [0] += you[0].mutation [11] * 200;
        func_pass [1] = 1;
        func_pass [2] = 0;
        return 0;

        case 14:
        if (you[0].mutation [10] > 0) mpr ("Blech - you need meat!");
         else   mpr("That lemon was rather sour... But delicious nonetheless!");
        func_pass [0] += 1000;
        func_pass [0] -= you[0].mutation [10] * 300;
        func_pass [0] += you[0].mutation [11] * 300;
        func_pass [1] = 1;
        func_pass [2] = 0;
        return 0;

        case 15:
        if (you[0].mutation [10] > 0) mpr ("Blech - you need meat!");
         else   mpr("That grape was delicious!");
        func_pass [0] += 100;
        func_pass [0] -= you[0].mutation [10] * 20;
        func_pass [0] += you[0].mutation [11] * 20;
        func_pass [1] = 1;
        func_pass [2] = 0;
        return 0;

        case 16:
        if (you[0].mutation [10] > 0) mpr ("Blech - you need meat!");
         else   mpr("That sultana was delicious! (but very small)");
        func_pass [0] += 70; // won't rescue you from starvation
        func_pass [0] -= you[0].mutation [10] * 20;
        func_pass [0] += you[0].mutation [11] * 20;
        func_pass [1] = 1;
        func_pass [2] = 0;
        return 0;

        case 17:
        if (you[0].mutation [10] > 0) mpr ("Blech - you need meat!");
         else   mpr("That lychee was delicious!");
        func_pass [0] += 600;
        func_pass [0] -= you[0].mutation [10] * 200;
        func_pass [0] += you[0].mutation [11] * 200;
        func_pass [1] = 1;
        func_pass [2] = 0;
        return 0;

        case 18:
        if (you[0].mutation [11] > 0) mpr ("Blech - you need vegetation!");
         else { mpr("That beef jerky was delicious!");
 if (random2(4) == 0) mpr("That beef jerky was jerk-a-riffic!");
              }
        func_pass [0] += 1500;
        func_pass [0] -= you[0].mutation [11] * 200;
        func_pass [0] += you[0].mutation [10] * 200;
        func_pass [1] = 1;
        func_pass [2] = 0;
        return 0;

        case 19:
        strcpy(info, "Mmm... ");
 switch(random2(9))
        {
  case 0: strcat(info, "Cheddar."); break;
  case 1: strcat(info, "Edam."); break;
  case 2: strcat(info, "Wensleydale."); break;
  case 3: strcat(info, "Camembert."); break;
  case 4: strcat(info, "Goat's milk cheese."); break;
  case 5: strcat(info, "Fruit cheese."); break;
  case 6: strcat(info, "Mozzarella."); break;
  case 7: strcat(info, "Sheep cheese."); break;
  case 8: strcat(info, "Yak cheese."); break;
        }
        mpr(info);
        func_pass [0] += 1200;
        func_pass [1] = 1;
        func_pass [2] = 0;
        return 0;

        case 20:
        mpr("That sausage was delicious!");
        func_pass [0] += 1200;
        func_pass [1] = 1;
        func_pass [2] = 0;
        return 0;

} // end of switch
return 0;
} // end of if food

// must therefore be carrion:

return 0;

}// end of int eaten(...)

#include "config.h"

#ifdef DOS
#include <conio.h>
#endif
#include <string.h>

//#include <stdlib.h> only needed for itoa

#include "externs.h"
#include "enum.h"
#include "spells2.h"
#include "macro.h"
#include "mutation.h"
#include "player.h"
#include "religion.h"
#include "stuff.h"
#include "misc.h"
#include "itemname.h"
#include "mstruct.h"
#include "skills2.h"

/*
IMPORTANT: There is also an eating bit in item_use.cc, so alter that with
carnivores and things if it's changed here.
*/


void eat_meat(int fod_eat_2);
char eat_from_floor(void);
char butchery(void);
int eating(int func_pass [10], unsigned char item_class, int food_eat_3);
void ghoul_eat_flesh(char rotting);


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
if (you[0].species == SP_GHOUL)
{
 you[0].hunger = 10999;
 you[0].hung_state = 3;
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


if (food_eat_3 == FOOD_CHUNK)
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

if (food_eat_3 == FOOD_ROYAL_JELLY)
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
int k = 0; /* initialised in the loop */
unsigned char keyin;
//gloggo = 0;
int items_here = 0;
int item_got = 0;

if (you[0].lev != 0 && wearing_amulet(AMU_CONTROLLED_FLIGHT) == 0)
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
   if (mitm.iclass [igrd [you[0].x_pos] [you[0].y_pos]] != OBJ_FOOD) goto out_of_eating;// && mitm.iclass [igrd [you[0].x_pos] [you[0].y_pos]] != 14) return 0;
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


   if (you[0].species == SP_GHOUL || you[0].species == SP_KOBOLD || you[0].mutation [MUT_CARNIVOROUS] == 3) // kobold, ghoul, mutant
   {
    if ((mitm.itype [item_got] >= FOOD_BREAD_RATION && mitm.itype [item_got] <= FOOD_CHOKO) || (mitm.itype [item_got] >= FOOD_SNOZZCUMBER && mitm.itype [item_got] <= FOOD_LYCHEE))
     {
      strcpy(info, "Sorry, you're a carnivore.");
      mpr(info);
      return 0;
     }
   }

   if (mitm.itype [item_got] == FOOD_CHUNK)
   {
    if (you[0].mutation [MUT_HERBIVOROUS] > 1)
    {
     mpr("You can't eat raw meat!");
     return 0;
    }
    if (you[0].hung_state > 2 && wearing_amulet(AMU_THE_GOURMAND) == 0 && you[0].species != SP_KOBOLD && you[0].species != SP_OGRE && you[0].species != SP_TROLL && you[0].species != SP_GHOUL && you[0].mutation [MUT_CARNIVOROUS] == 0)
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
   if (mitm.itype [item_got] == FOOD_ROYAL_JELLY)
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
   if (mitm.iclass [o] != OBJ_FOOD) goto out_of_eating; // && mitm.iclass [o] != 14) goto out_of_eating;
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

   if (you[0].species == SP_GHOUL || you[0].species == SP_KOBOLD || you[0].mutation [MUT_CARNIVOROUS] == 3) // kobold etc
   {
    if ((mitm.itype [item_got] >= FOOD_BREAD_RATION && mitm.itype [item_got] <= FOOD_CHOKO) || (mitm.itype [item_got] >= FOOD_SNOZZCUMBER && mitm.itype [item_got] <= FOOD_LYCHEE))
     {
      strcpy(info, "Sorry, you're a carnivore.");
      mpr(info);
      return 0;
     }
   }

   if (mitm.itype [item_got] == FOOD_CHUNK)
   {
    if (you[0].mutation [MUT_HERBIVOROUS] > 1)
    {
     mpr("You can't eat raw meat!");
     return 0;
    }
    if (you[0].hung_state > 2 && wearing_amulet(AMU_THE_GOURMAND) == 0 && you[0].species != SP_KOBOLD && you[0].species != SP_OGRE && you[0].species != SP_TROLL && you[0].species != SP_GHOUL && you[0].mutation [MUT_CARNIVOROUS] == 0)
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
   if (mitm.itype [item_got] == FOOD_ROYAL_JELLY)
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
int o = igrd [you[0].x_pos] [you[0].y_pos];
int k = 0;
int item_got;
unsigned char keyin;

if (igrd [you[0].x_pos] [you[0].y_pos] == 501)
{
        strcpy(info, "There isn't anything here!");
        mpr(info);
        return 0;
}


if (you[0].lev != 0 && wearing_amulet(AMU_CONTROLLED_FLIGHT) == 0)
{
        strcpy(info, "You can't reach the floor from up here.");
        mpr(info);
        return 0;
}

if (you[0].equip [EQ_WEAPON] == -1 && you[0].species != SP_TROLL && you[0].species != SP_GHOUL && you[0].attribute [ATTR_TRANSFORMATION] != TRAN_BLADE_HANDS && you[0].attribute [ATTR_TRANSFORMATION] != TRAN_DRAGON)
{
 strcpy(info, "What, with your bare hands?");
 mpr(info);
 return 0;
}

if (you[0].species != SP_TROLL && you[0].species != SP_GHOUL && you[0].attribute [ATTR_TRANSFORMATION] != TRAN_BLADE_HANDS && you[0].attribute [ATTR_TRANSFORMATION] != TRAN_DRAGON && damage_type(you[0].inv_class [you[0].equip [EQ_WEAPON]], you[0].inv_type [you[0].equip [EQ_WEAPON]]) != 1 && damage_type(you[0].inv_class [you[0].equip [EQ_WEAPON]], you[0].inv_type [you[0].equip [EQ_WEAPON]]) != 3)
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
   if (mitm.iclass [igrd [you[0].x_pos] [you[0].y_pos]] != OBJ_CORPSES || mitm.itype [igrd [you[0].x_pos] [you[0].y_pos]] != 0) goto out_of_eating;// && mitm.iclass [igrd [you[0].x_pos] [you[0].y_pos]] != 14) return 0;
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

   if (you[0].species == SP_TROLL || you[0].species == SP_GHOUL || you[0].attribute [ATTR_TRANSFORMATION] == TRAN_BLADE_HANDS || you[0].attribute [ATTR_TRANSFORMATION] == TRAN_DRAGON) mpr("You start tearing the corpse apart.");
    else mpr("You start hacking away.");

   if (you[0].duration [DUR_PRAYER] != 0 && (you[0].religion == GOD_OKAWARU || you[0].religion == GOD_MAKHLEB || you[0].religion == GOD_TROG))
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

        for (k = 0; k < items_here; k++)
        {
                    if (mitm.iclass [o] != OBJ_CORPSES || mitm.itype [o] != 0) goto out_of_eating; // && mitm.iclass [o] != 14) goto out_of_eating;
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

   if (you[0].species == SP_TROLL || you[0].species == SP_GHOUL || you[0].attribute [ATTR_TRANSFORMATION] == TRAN_BLADE_HANDS || you[0].attribute [ATTR_TRANSFORMATION] == TRAN_DRAGON) mpr("You start tearing the corpse apart.");
    else mpr("You start hacking away.");

   if (you[0].duration [DUR_PRAYER] != 0 && (you[0].religion == GOD_OKAWARU || you[0].religion == GOD_MAKHLEB || you[0].religion == GOD_TROG))
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

 if (wearing_amulet(AMU_THE_GOURMAND) == 1 && (fod_eat_2 == 2 || fod_eat_2 == 3)) fod_eat_2 = 1;

 switch(fod_eat_2)
 {
  case 1:
  appetising :
  if (you[0].species == SP_GHOUL)
  {
   ghoul_eat_flesh(0);
   break;
  }
  if (you[0].species == SP_HILL_ORC || you[0].species == SP_KOBOLD || you[0].species == SP_OGRE || you[0].species == SP_TROLL || you[0].mutation [MUT_CARNIVOROUS] > 0)
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
  if (you[0].species == SP_GHOUL)
  {
   ghoul_eat_flesh(fod_eat_2 == 50);
   break;
  }
  if ((you[0].species == SP_KOBOLD || you[0].species == SP_TROLL) && random2(15) != 0) goto appetising; // kobold, troll
  if ((you[0].species == SP_HILL_ORC  || you[0].species == SP_OGRE) && random2(5) != 0) goto appetising; // orc, ogre
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

void ghoul_eat_flesh(char rotting)
{

 char healed = 0;

 if (rotting == 0)
 {
   mpr("This raw flesh tastes good.");
   if (random2(5) != 0) healed = 1;
   if (random2(3) != 0 && you[0].base_hp < 5000)
   {
     you[0].base_hp ++;
     mpr("You feel more resilient.");
   }
 }
  else
 {
   mpr("This rotting flesh tastes delicious!");
   healed = 1;
   if (random2(4) != 0 && you[0].base_hp < 5000)
   {
     you[0].base_hp ++;
     mpr("You feel more resilient.");
         you[0].hunger += 500;
   }
 }

 if (you[0].strength < you[0].max_strength && random2(5) == 0)
 {
  mpr("You feel your strength returning.");
  you[0].strength ++;
  you[0].strength_ch = 1;
 }


if (healed == 1 && you[0].hp < you[0].hp_max)
{
 you[0].hp += 1 + random2(5) + random2(you[0].xl + 1);
 if (you[0].hp > you[0].hp_max) you[0].hp = you[0].hp_max;
}

you[0].hunger += 1500;
you[0].delay_t = 3;
you[0].delay_doing = 0;

calc_hp();
you[0].hp_ch = 1;

}


int eating(int func_pass [10], unsigned char item_class, int food_eat_3)
{

if (item_class == OBJ_FOOD)
{
switch(food_eat_3)
{
        case FOOD_MEAT_RATION:
        mpr("That meat ration really hit the spot!");
        func_pass [0] += 5000;
        func_pass [0] += you[0].mutation [MUT_CARNIVOROUS] * 500;
        func_pass [0] -= you[0].mutation [MUT_HERBIVOROUS] * 1500; /* should never be 3 * 2000 */
        func_pass [1] = 4;
        func_pass [2] = 0;
        //hung_ch = 1;
        return 0;

        case FOOD_BREAD_RATION:
    if (you[0].mutation [MUT_CARNIVOROUS] > 0) mpr ("Blech - you need meat!");
         else mpr("That bread ration really hit the spot!");
        func_pass [0] -= you[0].mutation [MUT_CARNIVOROUS] * 1000;
        func_pass [0] += you[0].mutation [MUT_HERBIVOROUS] * 500;
        func_pass [0] += 4400;
        func_pass [1] = 4;
        func_pass [2] = 0;
        return 0;

        case FOOD_APPLE: // apple
        if (you[0].mutation [MUT_CARNIVOROUS] > 0) mpr ("Blech - you need meat!");
         else mpr("Mmmm... Yummy apple.");
        func_pass [0] += 700;
        func_pass [0] -= you[0].mutation [MUT_CARNIVOROUS] * 200;
        func_pass [0] += you[0].mutation [MUT_HERBIVOROUS] * 200;
        func_pass [1] = 1;
        func_pass [2] = 0;
        //hung_ch = 1;
        return 0;

        case FOOD_PEAR: // pear
        if (you[0].mutation [MUT_CARNIVOROUS] > 0) mpr ("Blech - you need meat!");
         else mpr("Mmmm... Yummy pear.");
        //incrl();
        //hung += 300;
        //delay_t = 1;
        //delay_doing = 0;
        func_pass [0] += 700;
        func_pass [0] -= you[0].mutation [MUT_CARNIVOROUS] * 200;
        func_pass [0] += you[0].mutation [MUT_HERBIVOROUS] * 200;
        func_pass [1] = 1;
        func_pass [2] = 0;
        //hung_ch = 1;
        return 0;

        case FOOD_CHOKO:
        if (you[0].mutation [MUT_CARNIVOROUS] > 0) mpr ("Blech - you need meat!");
         else   mpr("That choko was very bland.");
        //incrl();
        //hung += 300;
        //delay_t = 1;
        //delay_doing = 0;
        func_pass [0] += 600;
        func_pass [0] -= you[0].mutation [MUT_CARNIVOROUS] * 200;
        func_pass [0] += you[0].mutation [MUT_HERBIVOROUS] * 200;
        func_pass [1] = 1;
        func_pass [2] = 0;
        //hung_ch = 1;
        return 0;

        case FOOD_HONEYCOMB:
        mpr("That honeycomb was delicious.");
        func_pass [0] += 2000;
        func_pass [1] = 1;
        func_pass [2] = 0;
        return 0;

        case FOOD_ROYAL_JELLY:
        mpr("That royal jelly was delicious!");
        func_pass [0] += 5000;
        func_pass [1] = 1;
        func_pass [2] = 0;
        return 0;

        case FOOD_SNOZZCUMBER: // maybe a nasty side-effect from RD's book?
        if (you[0].mutation [MUT_CARNIVOROUS] > 0) mpr ("Blech - you need meat!");
         else mpr("That snozzcumber tasted truly putrid!");
        func_pass [0] += 1500;
        func_pass [0] -= you[0].mutation [MUT_CARNIVOROUS] * 200;
        func_pass [0] += you[0].mutation [MUT_HERBIVOROUS] * 200;
        func_pass [1] = 1;
        func_pass [2] = 0;
        return 0;

        case FOOD_PIZZA:
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

        case FOOD_APRICOT:
        if (you[0].mutation [MUT_CARNIVOROUS] > 0) mpr ("Blech - you need meat!");
         else mpr("That apricot was delicious!");
        func_pass [0] += 700;
        func_pass [0] -= you[0].mutation [MUT_CARNIVOROUS] * 200;
        func_pass [0] += you[0].mutation [MUT_HERBIVOROUS] * 200;
        func_pass [1] = 1;
        func_pass [2] = 0;
        return 0;

        case FOOD_ORANGE:
        if (you[0].mutation [MUT_CARNIVOROUS] > 0) mpr ("Blech - you need meat!");
         else   {       mpr("That orange was delicious!");
                        if (random2(8) == 0) strcat(info, " Even the peel tasted good!");
                }
        func_pass [0] += 1000;
        func_pass [0] -= you[0].mutation [MUT_CARNIVOROUS] * 300;
        func_pass [0] += you[0].mutation [MUT_HERBIVOROUS] * 300;
        func_pass [1] = 1;
        func_pass [2] = 0;
        return 0;

        case FOOD_BANANA:
        if (you[0].mutation [MUT_CARNIVOROUS] > 0) mpr ("Blech - you need meat!");
         else {mpr("That banana was delicious!");
 if (random2(8) == 0) strcat(info, " Even the peel tasted good!");
              }
        func_pass [0] += 1000;
        func_pass [0] -= you[0].mutation [MUT_CARNIVOROUS] * 300;
        func_pass [0] += you[0].mutation [MUT_HERBIVOROUS] * 300;
        func_pass [1] = 1;
        func_pass [2] = 0;
        return 0;

        case FOOD_STRAWBERRY:
        if (you[0].mutation [MUT_CARNIVOROUS] > 0) mpr ("Blech - you need meat!");
         else   mpr("That strawberry was delicious!");
        func_pass [0] += 200;
        func_pass [0] -= you[0].mutation [MUT_CARNIVOROUS] * 50;
        func_pass [0] += you[0].mutation [MUT_HERBIVOROUS] * 50;
        func_pass [1] = 1;
        func_pass [2] = 0;
        return 0;

        case FOOD_RAMBUTAN:
        if (you[0].mutation [MUT_CARNIVOROUS] > 0) mpr ("Blech - you need meat!");
         else   mpr("That rambutan was delicious!");
        func_pass [0] += 600;
        func_pass [0] -= you[0].mutation [MUT_CARNIVOROUS] * 200;
        func_pass [0] += you[0].mutation [MUT_HERBIVOROUS] * 200;
        func_pass [1] = 1;
        func_pass [2] = 0;
        return 0;

        case FOOD_LEMON:
        if (you[0].mutation [MUT_CARNIVOROUS] > 0) mpr ("Blech - you need meat!");
         else   mpr("That lemon was rather sour... But delicious nonetheless!");
        func_pass [0] += 1000;
        func_pass [0] -= you[0].mutation [MUT_CARNIVOROUS] * 300;
        func_pass [0] += you[0].mutation [MUT_HERBIVOROUS] * 300;
        func_pass [1] = 1;
        func_pass [2] = 0;
        return 0;

        case FOOD_GRAPE:
        if (you[0].mutation [MUT_CARNIVOROUS] > 0) mpr ("Blech - you need meat!");
         else   mpr("That grape was delicious!");
        func_pass [0] += 100;
        func_pass [0] -= you[0].mutation [MUT_CARNIVOROUS] * 20;
        func_pass [0] += you[0].mutation [MUT_HERBIVOROUS] * 20;
        func_pass [1] = 1;
        func_pass [2] = 0;
        return 0;

        case FOOD_SULTANA:
        if (you[0].mutation [MUT_CARNIVOROUS] > 0) mpr ("Blech - you need meat!");
         else   mpr("That sultana was delicious! (but very small)");
        func_pass [0] += 70; // won't rescue you from starvation
        func_pass [0] -= you[0].mutation [MUT_CARNIVOROUS] * 20;
        func_pass [0] += you[0].mutation [MUT_HERBIVOROUS] * 20;
        func_pass [1] = 1;
        func_pass [2] = 0;
        return 0;

        case FOOD_LYCHEE:
        if (you[0].mutation [MUT_CARNIVOROUS] > 0) mpr ("Blech - you need meat!");
         else   mpr("That lychee was delicious!");
        func_pass [0] += 600;
        func_pass [0] -= you[0].mutation [MUT_CARNIVOROUS] * 200;
        func_pass [0] += you[0].mutation [MUT_HERBIVOROUS] * 200;
        func_pass [1] = 1;
        func_pass [2] = 0;
        return 0;

        case FOOD_BEEF_JERKY:
        if (you[0].mutation [MUT_HERBIVOROUS] > 0) mpr ("Blech - you need vegetation!");
         else { mpr("That beef jerky was delicious!");
 if (random2(4) == 0) mpr("That beef jerky was jerk-a-riffic!");
              }
        func_pass [0] += 1500;
        func_pass [0] -= you[0].mutation [MUT_HERBIVOROUS] * 200;
        func_pass [0] += you[0].mutation [MUT_CARNIVOROUS] * 200;
        func_pass [1] = 1;
        func_pass [2] = 0;
        return 0;

        case FOOD_CHEESE:
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

        case FOOD_SAUSAGE:
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

/*
 *  File:       food.cc
 *  Summary:    Functions for eating and butchering.
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 *      <2>      5/20/99        BWR             Added CRAWL_PIZZA.
 *      <1>      -/--/--        LRH             Created
 */

#include "AppHdr.h"
#include "food.h"

#ifdef DOS
#include <conio.h>
#endif

#include <string.h>

//#include <stdlib.h> only needed for itoa

#include "externs.h"
#include "enum.h"
#include "spells2.h"
#include "mutation.h"
#include "player.h"
#include "religion.h"
#include "stuff.h"
#include "misc.h"
#include "itemname.h"
#include "mstruct.h"
#include "skills2.h"
#include "debug.h"

#ifdef MACROS
  #include "macro.h"
#endif

/*
   IMPORTANT: There is also an eating bit in item_use.cc, so alter that with
   carnivores and things if it's changed here.
 */


   void eat_meat(int fod_eat_2);
   char eat_from_floor(void);
   char butchery(void);
   int eating(int func_pass[10], unsigned char item_class, int food_eat_3);
   void ghoul_eat_flesh(char rotting);


   void food_change()
   {
      you.hunger_state = 0;

      if (you.hunger <= 1000)
      {
         if (you.hunger_state != 1)
            you.redraw_hunger = 1;

         you.hunger_state = 1;   // starving
         return;
      }

      if (you.hunger <= 2600)
      {
         if (you.hunger_state != 2)
            you.redraw_hunger = 1;

         you.hunger_state = 2;   // hungry
         return;
      }

      if (you.hunger < 7000)
      {
         if (you.hunger_state != 3)
            you.redraw_hunger = 1;

         you.hunger_state = 3;   // -
         return;
      }

      if (you.species == SP_GHOUL)
      {
         if (you.hunger_state != 3)
            you.redraw_hunger = 1;

         you.hunger = 10999;
         you.hunger_state = 3;
         return;
      }

      if (you.hunger < 11000)
      {
         if (you.hunger_state != 4)
            you.redraw_hunger = 1;

         you.hunger_state = 4;   // Full
         return;
      }

      if (you.hunger_state != 5)
         you.redraw_hunger = 1;

      you.hunger_state = 5;       // Engorged
   }


   void eaten(int food_eat_2, int food_eat_3)      /* this contains results of eating things. */
   {
      you.redraw_hunger = 1;

      if (food_eat_3 == FOOD_CHUNK)
      {
         if (you.inv_dam [food_eat_2] < 100
            && (mons_corpse_thingy(you.inv_plus [food_eat_2]) == 1
               || mons_corpse_thingy(you.inv_plus [food_eat_2]) == 2)
            || (mons_corpse_thingy(you.inv_plus[food_eat_2]) == 3
               && player_res_poison() != 0))
            eat_meat(50);
         else
            eat_meat(mons_corpse_thingy(you.inv_plus[food_eat_2]));
         you.redraw_hunger = 1;
         return;
      }

      int func_pass[10];

      func_pass[0] = you.hunger;
      func_pass[1] = you.delay_t;
      func_pass[2] = you.delay_doing;

      eating(func_pass, 4, food_eat_3);

      you.hunger = func_pass[0];
      you.delay_t = func_pass[1];
      you.delay_doing = func_pass[2];

      if (food_eat_3 == FOOD_ROYAL_JELLY)
      {
         restore_str();
         restore_int();
         restore_dex();
      }
   }                               /* end of void eaten () */

   char eat_from_floor()
   {
      int gloggj;
      int o = igrd[you.x_pos][you.y_pos];
      int k = 0;                  /* initialised in the loop */
      unsigned char keyin;

    //gloggo = 0;
      int items_here = 0;
      int item_got = 0;

      if (you.levitation != 0 && wearing_amulet(AMU_CONTROLLED_FLIGHT) == 0)
      {
        //      strcpy(info, "You can't reach the floor from up here.");
        //      mpr(info);
         return 0;
      }


      int last_item = 501;

      int objl = igrd[you.x_pos][you.y_pos];
      int hrg = 0;
      int counter = 0;

      while (objl != 501)
      {
         counter++;

         last_item = objl;

         hrg = mitm.link[objl];
         objl = hrg;
         items_here++;

         if (counter > 1000)
         {
            error_message_to_player();
            return 0;
         }

      }

      if (items_here == 1)
      {
         if (mitm.base_type[igrd[you.x_pos][you.y_pos]] != OBJ_FOOD)
            goto out_of_eating; // && mitm.base_type [igrd [you.x_pos] [you.y_pos]] != 14) return 0;

         strcpy(info, "Eat ");
         if (mitm.quantity[igrd[you.x_pos][you.y_pos]] > 1)
            strcat(info, "one of ");
         it_name(igrd[you.x_pos][you.y_pos], 3, str_pass);
         strcat(info, str_pass);
         strcat(info, "\?");
         mpr(info);
         unsigned char keyin = getch();

         if (keyin == 0)
         {
            getch();
            keyin = 0;
         }
         if (keyin != 'y' && keyin != 'Y')
            return 0;

         int item_got = igrd[you.x_pos][you.y_pos];

         last_item = 501;
         you.redraw_hunger = 1;

         if (you.species == SP_GHOUL || you.species == SP_KOBOLD || you.mutation[MUT_CARNIVOROUS] == 3)  // kobold, ghoul, mutant
         {
            if ((mitm.sub_type[item_got] >= FOOD_BREAD_RATION && mitm.sub_type[item_got] <= FOOD_CHOKO) || (mitm.sub_type[item_got
                                 ]
                                 >= FOOD_SNOZZCUMBER
                                 && mitm.sub_type
                                 [item_got] <= FOOD_LYCHEE
                                 ))
            {
               strcpy(info, "Sorry, you're a carnivore.");
               mpr(info);
               return 0;
            }
         }

         if (mitm.sub_type[item_got] == FOOD_CHUNK)
         {
            if (you.mutation[MUT_HERBIVOROUS] > 1)
            {
               mpr("You can't eat raw meat!");
               return 0;
            }
            if (you.hunger_state > 2 && wearing_amulet(AMU_THE_GOURMAND) == 0 && you.species != SP_KOBOLD && you.
               species
               != SP_OGRE
               && you.species
               != SP_TROLL
               && you.species
               != SP_GHOUL
               && you.mutation[MUT_CARNIVOROUS] == 0)
            {
               strcpy(info, "You aren't quite hungry enough to eat that!");
               mpr(info);
               return 0;
            }

            /*    itoa(mons_corpse_thingy(mitm.pluses [item_got]), st_prn, 10);
               strcpy(info, st_prn);
               mpr(info); */

            //   if ((mitm.special [item_got] < 100 && mons_corpse_thingy(mitm.pluses [item_got]) == 1) || mons_corpse_thingy(mitm.pluses [item_got]) == 2 || (mons_corpse_thingy(mitm.pluses [item_got]) == 3 && player_res_poison() != 0)) eat_meat(50);
            if (mitm.special[item_got] < 100
               && (mons_corpse_thingy(mitm.pluses[item_got]) == 1
                  || mons_corpse_thingy(mitm.pluses[item_got]) == 2)
               || (mons_corpse_thingy(mitm.pluses[item_got]) == 3
                  && player_res_poison() != 0))
               eat_meat(50);
            else
               eat_meat(mons_corpse_thingy(mitm.pluses[item_got]));
            //    eat_meat(mons_corpse_thingy(mitm.pluses [item_got]));
            goto out_of_eating3;
         }
         int func_pass[10];

         func_pass[0] = you.hunger;
         func_pass[1] = you.delay_t;
         func_pass[2] = you.delay_doing;
         gloggj = eating(func_pass, mitm.base_type[item_got], mitm.sub_type[item_got]);

         you.hunger = func_pass[0];
         if (you.hunger > 12000)
            you.hunger = 12000;
         you.delay_t = func_pass[1];
         you.delay_doing = func_pass[2];

        //   mpr(info);
         if (mitm.sub_type[item_got] == FOOD_ROYAL_JELLY)
         {
            restore_str();
            restore_int();
            restore_dex();
         }
        //   more();
      out_of_eating3:you.turn_is_over = 1;
         you.redraw_hunger = 1;
         food_change();
         if (mitm.quantity[item_got] <= 1)
            destroy_item(item_got);
         else
            mitm.quantity[item_got]--;
         return 1;

      }
    // end of if items_here

      last_item = 501;

      if (items_here > 1)
      {

         o = igrd[you.x_pos][you.y_pos];

         for (k = 0; k < items_here; k++)        // use k because of call to relay_message()

         {
            if (mitm.base_type[o] != OBJ_FOOD)
               goto out_of_eating;     // && mitm.base_type [o] != 14) goto out_of_eating;

            strcpy(info, "Eat ");
            if (mitm.quantity[o] > 1)
               strcat(info, "one of ");
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

               if (you.species == SP_GHOUL || you.species == SP_KOBOLD || you.mutation[MUT_CARNIVOROUS] == 3)  // kobold etc

               {
                  if ((mitm.sub_type[item_got] >= FOOD_BREAD_RATION && mitm.sub_type[item_got] <= FOOD_CHOKO) || (mitm.sub_type
                                       [item_got]
                                       >= FOOD_SNOZZCUMBER
                                       && mitm.sub_type
                                       [item_got]
                                       <= FOOD_LYCHEE
                                       ))
                  {
                     strcpy(info, "Sorry, you're a carnivore.");
                     mpr(info);
                     return 0;
                  }
               }

               if (mitm.sub_type[item_got] == FOOD_CHUNK)
               {
                  if (you.mutation[MUT_HERBIVOROUS] > 1)
                  {
                     mpr("You can't eat raw meat!");
                     return 0;
                  }
                  if (you.hunger_state > 2 && wearing_amulet(AMU_THE_GOURMAND) == 0 &&
                     you.species != SP_KOBOLD &&
                     you.species != SP_OGRE &&
                     you.species != SP_TROLL &&
                     you.species != SP_GHOUL &&
                     you.mutation[MUT_CARNIVOROUS] == 0)
                  {
                     strcpy(info, "You aren't quite hungry enough to eat that!");
                     mpr(info);
                     return 0;
                  }
                  if (mitm.special[item_got] < 100
                     && (mons_corpse_thingy(mitm.pluses[item_got]) == 1
                        || mons_corpse_thingy (mitm.pluses [item_got]) == 2)
                     || (mons_corpse_thingy(mitm.pluses [item_got]) == 3
                        && player_res_poison() != 0))
                  {
                     eat_meat(50);
                  }
                  else
                  {
                     eat_meat(mons_corpse_thingy(mitm.pluses[item_got]));
                  }
                    //    eat_meat(mons_corpse_thingy(mitm.pluses [item_got]));
                  goto out_of_eating2;
               }
               int func_pass[10];

               func_pass[0] = you.hunger;
               if (you.hunger > 12000)
                  you.hunger = 12000;
               func_pass[1] = you.delay_t;
               func_pass[2] = you.delay_doing;
               gloggj = eating(func_pass, mitm.base_type[item_got], mitm.sub_type[item_got]);

               you.hunger = func_pass[0];
               you.delay_t = func_pass[1];
               you.delay_doing = func_pass[2];

                //   mpr(info);
               if (mitm.sub_type[item_got] == FOOD_ROYAL_JELLY)
               {
                  restore_str();
                  restore_int();
                  restore_dex();
               }

            out_of_eating2:food_change();
               you.redraw_hunger = 1;
                //   more();
               you.turn_is_over = 1;
               if (mitm.quantity[item_got] == 1)
                  destroy_item(item_got);
               else
                  mitm.quantity[item_got]--;
               return 1;
                //   destroy_item(item_got);


                //                              if (grunk != 1) // ie if the item picked up is still there.
                //                              {
                //                                      last_item = item_got;
                //                              }

            }


         out_of_eating:
            if (mitm.quantity[o] > 0)
               last_item = o;

            hrg = mitm.link[o];
            o = hrg;
            if (o == 501)
               return 0;

            if (items_here == 0)
               break;
         }                       // end of while k loop

        //      strcpy(info, "That's all.");
      }
    // end of if items_here
    //itc = 0;

      return 0;
   }                               // end of eat_from_floor() function

   char butchery(void)
   {
    //gloggo = 0;
      int items_here = 0;
      int o = igrd[you.x_pos][you.y_pos];
      int k = 0;
      int item_got;
      unsigned char keyin;

      if (igrd[you.x_pos][you.y_pos] == 501)
      {
         strcpy(info, "There isn't anything here!");
         mpr(info);
         return 0;
      }

      if (you.levitation != 0 && wearing_amulet(AMU_CONTROLLED_FLIGHT) == 0)
      {
         strcpy(info, "You can't reach the floor from up here.");
         mpr(info);
         return 0;
      }

      if (you.species != SP_TROLL
         && you.species != SP_GHOUL
         && you.attribute[ATTR_TRANSFORMATION] != TRAN_BLADE_HANDS
         && you.attribute[ATTR_TRANSFORMATION] != TRAN_DRAGON)
      {
         if (you.equip[EQ_WEAPON] == -1)
         {
            mpr( "What, with your bare hands?" );
            return 0;
         }
         else if (damage_type(you.inv_class[you.equip[EQ_WEAPON]],
                             you.inv_type[you.equip[EQ_WEAPON]]) != 1
                 && damage_type(you.inv_class[you.equip[EQ_WEAPON]],
                               you.inv_type[you.equip[EQ_WEAPON]]) != 3)
         {
            mpr( "Maybe you should try using a sharper implement." );
            return 0;
         }
      }


      int last_item = 501;

      int objl = igrd[you.x_pos][you.y_pos];
      int hrg = 0;
      int counter = 0;

      while (objl != 501)
      {
         counter++;

         last_item = objl;

         hrg = mitm.link[objl];
         objl = hrg;
         items_here++;

         if (counter > 1000)
         {
            error_message_to_player();
            return 0;
         }

      }

      if (items_here == 1)
      {
         if (mitm.base_type[igrd[you.x_pos][you.y_pos]] != OBJ_CORPSES ||
            mitm.sub_type[igrd[you.x_pos][you.y_pos]] != CORPSE_BODY)
            goto out_of_eating;

         strcpy(info, "Butcher ");
         it_name(igrd[you.x_pos][you.y_pos], 3, str_pass);
         strcat(info, str_pass);
         strcat(info, "\?");
         mpr(info);
         unsigned char keyin = getch();

         if (keyin == 0)
         {
            getch();
            keyin = 0;
         }
         if (keyin != 'y' && keyin != 'Y')
            return 0;

         int item_got = igrd[you.x_pos][you.y_pos];

         last_item = 501;
         you.redraw_hunger = 1;

         you.delay_t = 4;
         you.delay_doing = 4;

         if (you.species == SP_TROLL || you.species == SP_GHOUL
            || you.attribute[ATTR_TRANSFORMATION] == TRAN_BLADE_HANDS
            || you.attribute[ATTR_TRANSFORMATION] == TRAN_DRAGON)
            mpr("You start tearing the corpse apart.");
         else
            mpr("You start hacking away.");

         if (you.duration[DUR_PRAYER] != 0
            && (you.religion == GOD_OKAWARU
               || you.religion == GOD_MAKHLEB
               || you.religion == GOD_TROG))
         {
            offer_corpse(item_got);
            you.turn_is_over = 1;
            destroy_item(item_got);
            you.delay_t = 0;
            you.delay_doing = 0;
            return 1;           /* no chunks */
         }

         you.turn_is_over = 1;
         place_chunks(mitm.pluses[item_got], mitm.special[item_got],
                     you.x_pos, you.y_pos, mitm.colour[item_got]);
         destroy_item(item_got);
         return 1;
      }
    // end of if items_here

      last_item = 501;

      if (items_here > 1)
      {
         o = igrd[you.x_pos][you.y_pos];

         for (k = 0; k < items_here; k++)
         {
            if (mitm.base_type[o] != OBJ_CORPSES
               || mitm.sub_type[o] != CORPSE_BODY)
               goto out_of_eating;

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

               you.delay_t = 4;
               you.delay_doing = 4;

               if (you.species == SP_TROLL || you.species == SP_GHOUL
                  || you.attribute[ATTR_TRANSFORMATION] == TRAN_BLADE_HANDS
                  || you.attribute[ATTR_TRANSFORMATION] == TRAN_DRAGON)
               {
                  mpr("You start tearing the corpse apart.");
               }
               else
               {
                  mpr("You start hacking away.");
               }

               if (you.duration[DUR_PRAYER] != 0
                  && (you.religion == GOD_OKAWARU
                     || you.religion == GOD_MAKHLEB
                     || you.religion == GOD_TROG))
               {
                  offer_corpse(item_got);
                  you.turn_is_over = 1;
                  destroy_item(item_got);
                  you.delay_t = 0;
                  you.delay_doing = 0;
                  return 1;   /* no chunks */
               }

               place_chunks(mitm.pluses[item_got], mitm.special[item_got],
                           you.x_pos, you.y_pos, mitm.colour[item_got]);
               you.turn_is_over = 1;
               destroy_item(item_got);
               return 1;

            }


         out_of_eating:
            if (mitm.quantity[o] > 0)
               last_item = o;

            hrg = mitm.link[o];
            o = hrg;

            if (o == 501)
               goto failed;

            if (items_here == 0)
               break;
         }                       // end of while k loop
      }
    //return 0;

   failed:
      strcpy(info, "There isn't anything to dissect here.");
      mpr(info);
      return 0;

   }                               // end of eat_from_floor() function


   void eat_meat(int fod_eat_2)
   {
      if (wearing_amulet(AMU_THE_GOURMAND) == 1 && (fod_eat_2 == 2 || fod_eat_2 == 3))
         fod_eat_2 = 1;

      switch (fod_eat_2)
      {
         case 1:
         appetising:
            if (you.species == SP_GHOUL)
            {
               ghoul_eat_flesh(0);
               break;
            }
            if (you.species == SP_HILL_ORC
            || you.species == SP_KOBOLD
            || you.species == SP_OGRE
            || you.species == SP_TROLL
            || you.mutation[MUT_CARNIVOROUS] > 0)
               mpr("This raw flesh tastes good.");
            else
               mpr("This raw flesh is not very appetising.");
            you.hunger += 1000;
            you.delay_t = 3;
            you.delay_doing = 0;
            break;

         case 2:
            if (random2(3) != 0)
               goto appetising;
         case 50:
            if (you.species == SP_GHOUL)
            {
               ghoul_eat_flesh(fod_eat_2 == 50);
               break;
            }
            if ((you.species == SP_KOBOLD || you.species == SP_TROLL) && random2(15) != 0)
               goto appetising;    // kobold, troll

            if ((you.species == SP_HILL_ORC || you.species == SP_OGRE) && random2(5) != 0)
               goto appetising;    // orc, ogre

            if (you.is_undead != 0)
               goto appetising;
            strcpy(info, "There is something wrong with this meat.");
            mpr(info);
            strcpy(info, "You feel ill.");
            mpr(info);
            if (you.disease > 100)
               you.disease = 210;
            else
               you.disease += 50 + random2(100);
            break;

         case 3:
            if (player_res_poison() != 0)
               goto appetising;
            strcpy(info, "Yeeuch - this meat is poisonous!");
            mpr(info);
            you.poison += 3 + random2(4);
            break;

         case 4:
            if (you.is_undead != 0)
               goto appetising;

            strcpy(info, "You feel your flesh start to rot away!");
            mpr(info);
            you.rotting += random2(10) + 10;
            if (you.disease > 100)
               you.disease = 210;
            else
               you.disease += 50 + random2(100);
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
         if (random2(5) != 0)
            healed = 1;
         if (random2(3) != 0 && you.base_hp < 5000)
         {
            you.base_hp++;
            mpr("You feel more resilient.");
         }
      }
      else
      {
         mpr("This rotting flesh tastes delicious!");
         healed = 1;
         if (random2(4) != 0 && you.base_hp < 5000)
         {
            you.base_hp++;
            mpr("You feel more resilient.");
            you.hunger += 500;
         }
      }

      if (you.strength < you.max_strength && random2(5) == 0)
      {
         mpr("You feel your strength returning.");
         you.strength++;
         you.redraw_strength = 1;
      }


      if (healed == 1 && you.hp < you.hp_max)
      {
         you.hp += 1 + random2(5) + random2(you.experience_level + 1);
         if (you.hp > you.hp_max)
            you.hp = you.hp_max;
      }

      you.hunger += 1500;
      you.delay_t = 3;
      you.delay_doing = 0;

      calc_hp();
      you.redraw_hit_points = 1;

   }


   int eating(int func_pass[10], unsigned char item_class, int food_eat_3)
   {
      if (item_class == OBJ_FOOD)
      {
         switch (food_eat_3)
         {
            case FOOD_MEAT_RATION:
               mpr("That meat ration really hit the spot!");
               func_pass[0] += 5000;
               func_pass[0] += you.mutation[MUT_CARNIVOROUS] * 500;
               func_pass[0] -= you.mutation[MUT_HERBIVOROUS] * 1500;       /* should never be 3 * 2000 */
               func_pass[1] = 4;
               func_pass[2] = 0;
            return 0;

            case FOOD_BREAD_RATION:
               if (you.mutation[MUT_CARNIVOROUS] > 0)
                  mpr("Blech - you need meat!");
               else
                  mpr("That bread ration really hit the spot!");
               func_pass[0] -= you.mutation[MUT_CARNIVOROUS] * 1000;
               func_pass[0] += you.mutation[MUT_HERBIVOROUS] * 500;
               func_pass[0] += 4400;
               func_pass[1] = 4;
               func_pass[2] = 0;
               return 0;

            case FOOD_APPLE:        // apple

               if (you.mutation[MUT_CARNIVOROUS] > 0)
                  mpr("Blech - you need meat!");
               else
                  mpr("Mmmm... Yummy apple.");
               func_pass[0] += 700;
               func_pass[0] -= you.mutation[MUT_CARNIVOROUS] * 200;
               func_pass[0] += you.mutation[MUT_HERBIVOROUS] * 200;
               func_pass[1] = 1;
               func_pass[2] = 0;
            //redraw_hunger = 1;
               return 0;

            case FOOD_PEAR: // pear

               if (you.mutation[MUT_CARNIVOROUS] > 0)
                  mpr("Blech - you need meat!");
               else
                  mpr("Mmmm... Yummy pear.");
            //incrl();
            //hung += 300;
            //delay_t = 1;
            //delay_doing = 0;
               func_pass[0] += 700;
               func_pass[0] -= you.mutation[MUT_CARNIVOROUS] * 200;
               func_pass[0] += you.mutation[MUT_HERBIVOROUS] * 200;
               func_pass[1] = 1;
               func_pass[2] = 0;
            //redraw_hunger = 1;
               return 0;

            case FOOD_CHOKO:
               if (you.mutation[MUT_CARNIVOROUS] > 0)
                  mpr("Blech - you need meat!");
               else
                  mpr("That choko was very bland.");
            //incrl();
            //hung += 300;
            //delay_t = 1;
            //delay_doing = 0;
               func_pass[0] += 600;
               func_pass[0] -= you.mutation[MUT_CARNIVOROUS] * 200;
               func_pass[0] += you.mutation[MUT_HERBIVOROUS] * 200;
               func_pass[1] = 1;
               func_pass[2] = 0;
            //redraw_hunger = 1;
               return 0;

            case FOOD_HONEYCOMB:
               mpr("That honeycomb was delicious.");
               func_pass[0] += 2000;
               func_pass[1] = 1;
               func_pass[2] = 0;
               return 0;

            case FOOD_ROYAL_JELLY:
               mpr("That royal jelly was delicious!");
               func_pass[0] += 5000;
               func_pass[1] = 1;
               func_pass[2] = 0;
               return 0;

            case FOOD_SNOZZCUMBER:  // maybe a nasty side-effect from RD's book?

               if (you.mutation[MUT_CARNIVOROUS] > 0)
                  mpr("Blech - you need meat!");
               else
                  mpr("That snozzcumber tasted truly putrid!");
               func_pass[0] += 1500;
               func_pass[0] -= you.mutation[MUT_CARNIVOROUS] * 200;
               func_pass[0] += you.mutation[MUT_HERBIVOROUS] * 200;
               func_pass[1] = 1;
               func_pass[2] = 0;
               return 0;

            case FOOD_PIZZA:
               strcpy(info, "Mmm... ");

               if (sys_env.crawl_pizza && !random2(3))
               {
                  mpr(sys_env.crawl_pizza);
               }
               else
               {
                  switch (random2(9))
                  {
                     case 0:
                        strcat(info, "Ham and pineapple.");
                        break;
                     case 1:
                        strcat(info, "Extra thick crust.");
                        break;
                     case 2:
                        strcat(info, "Vegetable.");
                        break;
                     case 3:
                        strcat(info, "Pepperoni.");
                        break;
                     case 4:
                        strcpy(info, "Yeuchh - Anchovies!");
                        break;
                     case 5:
                        strcat(info, "Cheesy.");
                        break;
                     case 6:
                        strcat(info, "Supreme.");
                        break;
                     case 7:
                        strcat(info, "Super Supreme!");
                        break;
                     case 8:
                        strcat(info, "Chicken.");
                        break;
                  }
                  mpr(info);
               }
               func_pass[0] += 1500;
               func_pass[1] = 1;
               func_pass[2] = 0;
               return 0;

            case FOOD_APRICOT:
               if (you.mutation[MUT_CARNIVOROUS] > 0)
                  mpr("Blech - you need meat!");
               else
                  mpr("That apricot was delicious!");
               func_pass[0] += 700;
               func_pass[0] -= you.mutation[MUT_CARNIVOROUS] * 200;
               func_pass[0] += you.mutation[MUT_HERBIVOROUS] * 200;
               func_pass[1] = 1;
               func_pass[2] = 0;
               return 0;

            case FOOD_ORANGE:
               if (you.mutation[MUT_CARNIVOROUS] > 0)
                  mpr("Blech - you need meat!");
               else
               {
                  mpr("That orange was delicious!");
                  if (random2(8) == 0)
                     strcat(info, " Even the peel tasted good!");
               }
               func_pass[0] += 1000;
               func_pass[0] -= you.mutation[MUT_CARNIVOROUS] * 300;
               func_pass[0] += you.mutation[MUT_HERBIVOROUS] * 300;
               func_pass[1] = 1;
               func_pass[2] = 0;
               return 0;

            case FOOD_BANANA:
               if (you.mutation[MUT_CARNIVOROUS] > 0)
                  mpr("Blech - you need meat!");
               else
               {
                  mpr("That banana was delicious!");
                  if (random2(8) == 0)
                     strcat(info, " Even the peel tasted good!");
               }
               func_pass[0] += 1000;
               func_pass[0] -= you.mutation[MUT_CARNIVOROUS] * 300;
               func_pass[0] += you.mutation[MUT_HERBIVOROUS] * 300;
               func_pass[1] = 1;
               func_pass[2] = 0;
               return 0;

            case FOOD_STRAWBERRY:
               if (you.mutation[MUT_CARNIVOROUS] > 0)
                  mpr("Blech - you need meat!");
               else
                  mpr("That strawberry was delicious!");
               func_pass[0] += 200;
               func_pass[0] -= you.mutation[MUT_CARNIVOROUS] * 50;
               func_pass[0] += you.mutation[MUT_HERBIVOROUS] * 50;
               func_pass[1] = 1;
               func_pass[2] = 0;
               return 0;

            case FOOD_RAMBUTAN:
               if (you.mutation[MUT_CARNIVOROUS] > 0)
                  mpr("Blech - you need meat!");
               else
                  mpr("That rambutan was delicious!");
               func_pass[0] += 600;
               func_pass[0] -= you.mutation[MUT_CARNIVOROUS] * 200;
               func_pass[0] += you.mutation[MUT_HERBIVOROUS] * 200;
               func_pass[1] = 1;
               func_pass[2] = 0;
               return 0;

            case FOOD_LEMON:
               if (you.mutation[MUT_CARNIVOROUS] > 0)
                  mpr("Blech - you need meat!");
               else
                  mpr("That lemon was rather sour... But delicious nonetheless!");
               func_pass[0] += 1000;
               func_pass[0] -= you.mutation[MUT_CARNIVOROUS] * 300;
               func_pass[0] += you.mutation[MUT_HERBIVOROUS] * 300;
               func_pass[1] = 1;
               func_pass[2] = 0;
               return 0;

            case FOOD_GRAPE:
               if (you.mutation[MUT_CARNIVOROUS] > 0)
                  mpr("Blech - you need meat!");
               else
                  mpr("That grape was delicious!");
               func_pass[0] += 100;
               func_pass[0] -= you.mutation[MUT_CARNIVOROUS] * 20;
               func_pass[0] += you.mutation[MUT_HERBIVOROUS] * 20;
               func_pass[1] = 1;
               func_pass[2] = 0;
               return 0;

            case FOOD_SULTANA:
               if (you.mutation[MUT_CARNIVOROUS] > 0)
                  mpr("Blech - you need meat!");
               else
                  mpr("That sultana was delicious! (but very small)");
               func_pass[0] += 70; // won't rescue you from starvation

               func_pass[0] -= you.mutation[MUT_CARNIVOROUS] * 20;
               func_pass[0] += you.mutation[MUT_HERBIVOROUS] * 20;
               func_pass[1] = 1;
               func_pass[2] = 0;
               return 0;

            case FOOD_LYCHEE:
               if (you.mutation[MUT_CARNIVOROUS] > 0)
                  mpr("Blech - you need meat!");
               else
                  mpr("That lychee was delicious!");
               func_pass[0] += 600;
               func_pass[0] -= you.mutation[MUT_CARNIVOROUS] * 200;
               func_pass[0] += you.mutation[MUT_HERBIVOROUS] * 200;
               func_pass[1] = 1;
               func_pass[2] = 0;
               return 0;

            case FOOD_BEEF_JERKY:
               if (you.mutation[MUT_HERBIVOROUS] > 0)
                  mpr("Blech - you need vegetation!");
               else
               {
                  mpr("That beef jerky was delicious!");
                  if (random2(4) == 0)
                     mpr("That beef jerky was jerk-a-riffic!");
               }
               func_pass[0] += 1500;
               func_pass[0] -= you.mutation[MUT_HERBIVOROUS] * 200;
               func_pass[0] += you.mutation[MUT_CARNIVOROUS] * 200;
               func_pass[1] = 1;
               func_pass[2] = 0;
               return 0;

            case FOOD_CHEESE:
               strcpy(info, "Mmm... ");
               switch (random2(9))
               {
                  case 0:
                     strcat(info, "Cheddar.");
                     break;
                  case 1:
                     strcat(info, "Edam.");
                     break;
                  case 2:
                     strcat(info, "Wensleydale.");
                     break;
                  case 3:
                     strcat(info, "Camembert.");
                     break;
                  case 4:
                     strcat(info, "Goat's milk cheese.");
                     break;
                  case 5:
                     strcat(info, "Fruit cheese.");
                     break;
                  case 6:
                     strcat(info, "Mozzarella.");
                     break;
                  case 7:
                     strcat(info, "Sheep cheese.");
                     break;
                  case 8:
                     strcat(info, "Yak cheese.");
                     break;
               }
               mpr(info);
               func_pass[0] += 1200;
               func_pass[1] = 1;
               func_pass[2] = 0;
               return 0;

            case FOOD_SAUSAGE:
               mpr("That sausage was delicious!");
               func_pass[0] += 1200;
               func_pass[1] = 1;
               func_pass[2] = 0;
               return 0;

         }                       // end of switch

         return 0;
      }
    // end of if food

    // must therefore be carrion:

      return 0;
   }                               // end of int eaten(...)

/*
 *  File:       spells2.cc
 *  Summary:    Implementations of some additional spells.
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 *     <3>     10/11/99    BCR     fixed range bug in burn_freeze,
 *                                 vamp_drain, and summon_elemental
 *     <2>     5/26/99     JDJ     detect_items uses '~' instead of '*'.
 *     <1>     -/--/--     LRH     Created
 */

#include "AppHdr.h"
#include "spells2.h"

#include <stdio.h>
#include <string.h>

#include "externs.h"
#include "enum.h"

#include "mstruct.h"
#include "message.h"
#include "player.h"
#include "ouch.h"
#include "beam.h"
#include "direct.h"
#include "monplace.h"
#include "monstuff.h"
#include "player.h"
#include "stuff.h"
#include "view.h"
#include "misc.h"
#include "fight.h"
#include "itemname.h"
#include "monstuff.h"

   int raise_corpse(int corps, char corx, char cory, int corps_beh, int corps_hit, int actual);

   extern char wield_change;       /* defined in output.cc */


   char detect_traps(void)
   {
      char traps_found = 0;
      int count_x;

      for (count_x = 0; count_x < NTRAPS; count_x++)
      {
         if (env.trap_x[count_x] > you.x_pos - 15 && env.trap_x[count_x] < you.x_pos + 15 && env.trap_y[count_x] > you.y_pos - 8 && env.trap_y[count_x] < you.y_pos + 8)
         {
            if (grd[env.trap_x[count_x]][env.trap_y[count_x]] == 78)
            {
               if (env.trap_type[count_x] < 4 || env.trap_type[count_x] == 6 || env.trap_type[count_x] == 7)
                  grd[env.trap_x[count_x]][env.trap_y[count_x]] = 75;
               if (env.trap_type[count_x] == 4 || env.trap_type[count_x] == 5 || env.trap_type[count_x] == 8)
                  grd[env.trap_x[count_x]][env.trap_y[count_x]] = 76;
               env.map[env.trap_x[count_x] - 1][env.trap_y[count_x] - 1] = '^';
               traps_found++;
            }
         }
      }

      return traps_found;
   }


   char detect_items(int map_radius)
   {
      char traps_found = 0;

      mpr("You detect items!");

      int i, j;

      if (map_radius >= 50)
         map_radius = 50;

      for (i = you.x_pos - map_radius; i < you.x_pos + map_radius; i++)
      {
         for (j = you.y_pos - map_radius; j < you.y_pos + map_radius; j++)
         {
            if (i < 5 || j < 5 || i > 75 || j > 65)
               continue;
            if (igrd[i][j] != 501)
               env.map[i - 1][j - 1] = '~';
         }
      }

      return traps_found;
   }


   char detect_creatures(int map_radius)
   {
      char traps_found = 0;

      mpr("You detect creatures!");

      int i, j;

      map_radius = 50;

      for (i = you.x_pos - map_radius; i < you.x_pos + map_radius; i++)
      {
         for (j = you.y_pos - map_radius; j < you.y_pos + map_radius; j++)
         {
            if (i < 5 || j < 5 || i > 75 || j > 65)
               continue;
            if (mgrd[i][j] != MNG)
            {
               env.map[i - 1][j - 1] = mons_char(menv[mgrd[i][j]].type);
            }
         }
      }

      return traps_found;
   }


   int corpse_rot(int power)
   {

      char adx = 0;
      char ady = 0;

      char minx = you.x_pos - 6;
      char maxx = you.x_pos + 7;
      char miny = you.y_pos - 6;
      char maxy = you.y_pos + 6;
      char xinc = 1;
      char yinc = 1;

      if (random2(2) == 0)
      {
         minx = you.x_pos + 6;
         maxx = you.x_pos - 7;
         xinc = -1;
      }

      if (random2(2) == 0)
      {
         miny = you.y_pos + 6;
         maxy = you.y_pos - 7;
         yinc = -1;
      }


      for (adx = minx; adx != maxx; adx += xinc)
      {

         if (adx == 7 || adx == -7)
         {
            return 0;
         }

         for (ady = miny; ady != maxy; ady += yinc)
         {

            if (see_grid(adx, ady) == 1)
            {
               if (igrd[adx][ady] == 501 || env.cgrid[adx][ady] != CNG)
                  continue;

               int objl = igrd[adx][ady];
               int hrg = 0;

               while (objl != 501)
               {
                  if (mitm.base_type[objl] == 14 && mitm.sub_type[objl] == 0)
                  {
                     if (mons_skeleton(mitm.pluses[objl]) == 0)
                     {
                        destroy_item(objl);
                     }
                     else
                     {
                        mitm.sub_type[objl] = 1;
                        mitm.special[objl] = 200;
                        mitm.colour[objl] = LIGHTGREY;
                     }
                     place_cloud(9, adx, ady, 4 + random2(6) + random2(6) + random2(6));
                     goto out_of_raise;
                  }
                  hrg = mitm.link[objl];
                  objl = hrg;
               }
            out_of_raise:objl = 1;
            }
         }
      }

      strcpy(info, "You smell decay.");
      mpr(info);

      power = 0;
   // should make zombies decay into skeletons

      return 0;

   }



   int animate_dead(int power, int corps_beh, int corps_hit, int actual)
   {
      char adx = 0;
      char ady = 0;

      char minx = you.x_pos - 6;
      char maxx = you.x_pos + 7;
      char miny = you.y_pos - 6;
      char maxy = you.y_pos + 6;
      char xinc = 1;
      char yinc = 1;

      char number_raised = 0;

      if (random2(2) == 0)
      {
         minx = you.x_pos + 6;
         maxx = you.x_pos - 7;
         xinc = -1;
      }

      if (random2(2) == 0)
      {
         miny = you.y_pos + 6;
         maxy = you.y_pos - 7;
         yinc = -1;
      }

      for (adx = minx; adx != maxx; adx += xinc)
      {
         if ((adx == 7) || (adx == -7))
         {
            return 0;
         }

         for (ady = miny; ady != maxy; ady += yinc)
         {
            if (see_grid(adx, ady) == 1)
            {
               if (igrd[adx][ady] != 501)
               {
                  int objl = igrd[adx][ady];
                  int hrg = 0;

                 // this searches thru all the items on the ground for a corpse
                  while (objl != 501)
                  {
                     if (mitm.base_type[objl] == 14)
                     {
                        number_raised += raise_corpse(objl, adx, ady, corps_beh,
                                             corps_hit, actual);
                        power--;
                        break;
                     }
                     hrg = mitm.link[objl];
                     objl = hrg;
                  }
                  objl = 1;
               }
            }
         }
      }

      if (actual == 0)
         return number_raised;

      if (number_raised > 0)
         strcpy(info, "The dead are walking!");
      else
         strcpy(info, "The dark energy consumes the dead!");
      mpr(info);

      return number_raised;
   }

   int animate_a_corpse(char axps, char ayps, int corps_beh, int corps_hit, char class_allowed)
   {
      if (igrd[axps][ayps] == 501)
         return 0;
      else if (mitm.base_type[igrd[axps][ayps]] != 14)
         return 0;
      else if (class_allowed == 1 && mitm.sub_type[igrd[axps][ayps]] != 1)
         return 0;
      else if (raise_corpse(igrd[axps][ayps], axps, ayps, corps_beh, corps_hit, 1) > 0)
         strcpy(info, "The dead are walking!");
      else
         strcpy(info, "Dark energy shatters the bones!");

      mpr(info);

      return 0;

   }                               // end of int animate_a_corpse

   int raise_corpse(int corps, char corx, char cory, int corps_beh, int corps_hit, int actual)
   {
      int returnVal = 1;

      if (mons_zombie_size(mitm.pluses[corps]) == 0)
         returnVal =  0;
      else if (actual != 0)
      {
         if (mitm.sub_type[corps] == 0)
            create_monster(MONS_SMALL_ZOMBIE, 0, corps_beh, corx, cory, corps_hit,
                          mitm.pluses[corps]);
         else
            create_monster(MONS_SMALL_SKELETON, 0, corps_beh, corx, cory, corps_hit,
                          mitm.pluses[corps]);
      }
      destroy_item(corps);

      return returnVal;
   }

   void cast_twisted(int power, int corps_beh, int corps_hit)
   {

      int total_mass = 0;
      int old_item = 501;
      int number_raised = 0;
      char coloured = corps_beh;

      coloured = corps_hit;
      coloured = 0;
      char type_resurr = 23;

      unsigned char rotted = 0;

      if (igrd[you.x_pos][you.y_pos] == 501)
      {
         strcpy(info, "There's nothing here!");
         mpr(info);
         return;
      }

      int objl = igrd[you.x_pos][you.y_pos];
      int hrg = 0;

      while (objl != 501)
      {
         if (mitm.base_type[objl] == 14 && mitm.sub_type[objl] == 0)
         {
         //      number_raised += raise_corpse(objl, adx, ady, corps_beh, corps_hit, actual);
            //      power --;
            total_mass += mons_weight(mitm.pluses[objl]);
            if (mitm.special[objl] < 100)
               rotted++;
            old_item = objl;
            hrg = mitm.link[objl];
            objl = hrg;
            destroy_item(old_item);
            number_raised++;
            if (power <= 0)
               goto finished;
            continue;
         //      goto out_of_raise;
         }
         hrg = mitm.link[objl];
         objl = hrg;
      }
   //   out_of_raise : objl = 1;
    //  }
    // }

   //if (total_mass == 0) return;

   finished:                       // now to raise the thing
   #ifdef WIZARD
    sprintf( info, "Total mass: %d", total_mass );
    mpr(info);
   #endif

      total_mass += random2(power) * 3 + random2(power) + random2(power) + random2(power) + random2(power) + random2(power) + random2(power) + random2(power) + random2(power) * 3 + random2(power) * 3 + random2(power) + random2(power) * 3;

      if (total_mass <= 400 + random2(500) + random2(500) || number_raised < 2 + random2(2))
      {
         strcpy(info, "The spell fails.");
         mpr(info);
         strcpy(info, "The corpses disintegrate into a pulpy mess.");
         mpr(info);
         return;
      }

      if (total_mass >= 500 + random2(700) + random2(900) + random2(1000))
         type_resurr = 49;

      strcpy(info, "The heap of corpses melds into an agglomeration of writhing flesh!");
      mpr(info);

      coloured = LIGHTRED;
      if (rotted >= random2(number_raised))
         coloured = RED;
      if (rotted >= number_raised)
         coloured = BROWN;

      create_monster(type_resurr, 0, 7, you.x_pos, you.y_pos, you.pet_target, coloured);


   }


   char brand_weapon(char which_brand, int power)
   {

      char duration_affected = 0;

      if (you.duration[DUR_VORPAL_BLADE] != 0 || you.duration[DUR_FIRE_BRAND] != 0 || you.duration[DUR_ICE_BRAND] != 0 || you.duration[DUR_LETHAL_INFUSION] != 0)
         return 0;

      if (you.equip[EQ_WEAPON] == -1)
      {
         return 0;
      }

      if (you.inv_class[you.equip[EQ_WEAPON]] != 0 || (you.inv_type[you.equip[EQ_WEAPON]] >= 13 && you.inv_type[you.equip[EQ_WEAPON]] <= 16) || you.inv_type[you.equip[EQ_WEAPON]] == 0)
      {
         return 0;
      }

      if (you.inv_dam[you.equip[EQ_WEAPON]] % 30 != 0 || you.inv_dam[you.equip[EQ_WEAPON]] > 180 || you.inv_dam[you.equip[EQ_WEAPON]] % 30 >= 25)
      {
         return 0;
      }

      item_name(you.inv_plus2[you.equip[EQ_WEAPON]], you.inv_class[you.equip[EQ_WEAPON]], you.inv_type[you.equip[EQ_WEAPON]], you.inv_dam[you.equip[EQ_WEAPON]], you.inv_plus[you.equip[EQ_WEAPON]], you.inv_quantity[you.equip[EQ_WEAPON]], you.inv_ident[you.equip[EQ_WEAPON]], 4, str_pass);
      strcpy(info, str_pass);

      switch (which_brand)
      {
         case 1:                     // flaming

            you.inv_dam[you.equip[EQ_WEAPON]] += 1;
            strcat(info, " bursts into flame!");
            duration_affected = 6;
            break;

         case 2:                     // freezing

            you.inv_dam[you.equip[EQ_WEAPON]] += 2;
            strcat(info, " glows blue.");
            duration_affected = 7;
            break;

         case 6:                     // venom

            if (damage_type(you.inv_class[you.equip[EQ_WEAPON]], you.inv_type[you.equip[EQ_WEAPON]]) == 0)
               return 0;
            you.inv_dam[you.equip[EQ_WEAPON]] += 6;
            strcat(info, " starts dripping with poison.");
            duration_affected = 15;
            break;

         case 7:                     // draining

            you.inv_dam[you.equip[EQ_WEAPON]] += 8;
            strcat(info, " crackles with unholy energy.");
            duration_affected = 8;
            break;

         case 10:                    // vorpal blade

            if (damage_type(0, you.inv_type[you.equip[EQ_WEAPON]]) != 1)
            {
               return 0;
            }
            you.inv_dam[you.equip[EQ_WEAPON]] += 10;
            strcat(info, " glows silver and looks extremely sharp.");
            duration_affected = 5;
            break;

      }

      mpr(info);

      wield_change = 1;

      int dur_change = 7 + random2(power) + random2(power);

      if (dur_change + you.duration[duration_affected] >= 100)
         you.duration[duration_affected] = 100;
      else
         you.duration[duration_affected] += dur_change;


      return 1;

   }



   void manage_shock_shield(void)
   {
      you.shock_shield--;
      if (you.shock_shield == 0)
         return;

      char stx = 0;
      char sty = 0;

      for (stx = -1; stx < 2; stx++)
      {

         for (sty = -1; sty < 2; sty++)
         {
            if (sty == 0 && stx == 0)
               continue;

         //          if (random2(3) == 0) beam[0].range ++;

            if (grd[you.x_pos + stx][you.y_pos + sty] > 10 && env.cgrid[you.x_pos + stx][you.y_pos + sty] == CNG)
            {
               place_cloud(1, you.x_pos + stx, you.y_pos + sty, 1 + random2(2) + (random2(3) == 0));
            }

         }                       // end of sty

      }                           // end of stx

   }



   void restore_str(void)
   {
      if (you.strength < you.max_strength)
      {
         strcpy(info, "You feel your strength returning.");
         mpr(info);
      }
      you.strength = you.max_strength;
      you.redraw_strength = 1;
   }

   void restore_int(void)
   {
      if (you.intel < you.max_intel)
      {
         strcpy(info, "You feel your intelligence returning.");
         mpr(info);
      }
      you.intel = you.max_intel;
      you.redraw_intelligence = 1;
   }

   void restore_dex(void)
   {
      if (you.dex < you.max_dex)
      {
         strcpy(info, "You feel your dexterity returning.");
         mpr(info);
      }
      you.dex = you.max_dex;
      you.redraw_dexterity = 1;
   }

   void turn_undead(int pow)
   {

      int tu = 0, p;
      char brek = 0;

      mpr("You attempt to repel the undead.");

      for (tu = 0; tu < MNST; tu++)
      {
         if (menv[tu].type == -1 || !mons_near(tu))
            continue;

         if (mons_holiness(menv[tu].type) == 1)
         {
         /*  menv [tu].hit_points -= random2(5) + random2(pow) / 20;
         if (menv [tu].hit_points <= 0)
         {
         monster_die(tu, 1, 0);
         continue;
         } */

            if (random2(pow) + you.experience_level < menv[tu].hit_dice * 5)
               break;


            if (menv[tu].enchantment1 == 1)
               for (p = 0; p < 3; p++)
               {
                  if (menv[tu].enchantment[p] == 4)
                     brek = 1;
               }               // end of for p

            for (p = 0; p < 3; p++)
            {
               if (brek == 1)
               {
                  brek = 0;
                  break;
               }
               if (menv[tu].enchantment[p] == 0)
               {
                  menv[tu].enchantment[p] = 4;
                  menv[tu].enchantment1 = 1;
                  strcpy(info, monam(menv[tu].number, menv[tu].type, menv[tu].enchantment[2], 0));
                  strcat(info, " is repelled.");
                  mpr(info);
                  break;
               }
            }

         }                       // end of if mons_holiness

      }                           // end of for tu

   }                               // end of turn_undead


   void holy_word(int pow)
   {
      int tu = 0, p;
      char brek = 0;

      strcpy(info, "You speak a Word of immense power!");
      mpr(info);

      for (tu = 0; tu < MNST; tu++)
      {
         if (menv[tu].type == -1 || !mons_near(tu))
            continue;

         if (mons_holiness(menv[tu].type) > 0)
         {
            menv[tu].hit_points -= random2(15) + random2(15) + random2(pow) / 3;
            if (menv[tu].enchantment[2] == 6 && player_see_invis() == 0)
            {
               strcpy(info, monam(menv[tu].number, menv[tu].type, menv[tu].enchantment[2], 0));
               strcat(info, " convulses!");
               mpr(info);
            }
            if (menv[tu].hit_points <= 0)
            {
               monster_die(tu, 1, 0);
               continue;
            }

            if (menv[tu].speed_increment >= 25)
               menv[tu].speed_increment -= 20;

            if (menv[tu].enchantment1 == 1)
               for (p = 0; p < 3; p++)
               {
                  if (menv[tu].enchantment[p] == 4)
                     brek = 1;
               }               // end of for p

            for (p = 0; p < 3; p++)
            {
               if (brek == 1)
               {
                  brek = 0;
                  break;
               }
               if (menv[tu].enchantment[p] == 0)
               {
                  menv[tu].enchantment[p] = 4;
                  menv[tu].enchantment1 = 1;
                  break;
               }
            }

         }                       // end of if mons_holiness

      }                           // end of for tu



   }                               // end of holy word


   void cast_toxic_radiance(void)
   {

      unsigned char toxy = 0;

      strcpy(info, "You radiate a sickly green light!");
      mpr(info);
      show_green = GREEN;
      viewwindow(1, false);
      more();
      mesclr();

      if (you.invis != 0)
      {
         strcpy(info, "The light passes straight through your body.");
         mpr(info);
      }
      else if (player_res_poison() == 0)
      {
         you.poison += 2;
         strcpy(info, "You feel rather sick.");
         mpr(info);
      }


      for (toxy = 0; toxy < MNST; toxy++)
      {
         if (menv[toxy].type == -1)
            continue;
         if (mons_near(toxy))
         {
            if (menv[toxy].enchantment[2] != 6)
               poison_monster(toxy, 0);        // assumes only you can cast this spell (or would want to)

            if (menv[toxy].enchantment[2] != 6 && random2(2) == 0)
               poison_monster(toxy, 0);        // assumes only you can cast this spell (or would want to)

         }

      }                           // end loop

   }                               // end toxic rad

   void cast_refrigeration(int pow)
   {

      unsigned char toxy = 0;
      struct bolt beam[1];

      strcpy(info, "The heat is drained from your surroundings.");
      mpr(info);
      show_green = LIGHTCYAN;
      viewwindow(1, false);
      more();
      mesclr();

      if (player_res_cold() <= 100)
      {
         strcpy(info, "You freeze!");
         mpr(info);
         ouch(3 + random2(7) + random2(7) + random2(pow) / 20, 0, KILLED_BY_FREEZING);
      }
      if (player_res_cold() > 100)
      {
         strcpy(info, "You feel very cold.");
         mpr(info);
         ouch((3 + random2(7) + random2(7) + random2(pow) / 20) / (2 + (player_res_cold() - 100) * (player_res_cold() - 100)), 0, KILLED_BY_FREEZING);
      }
      if (player_res_cold() < 100)
      {
         ouch(3 + random2(7) + random2(pow) / 30, 0, KILLED_BY_FREEZING);        /* this is extra damage */
      }
      scrolls_burn(12, 8);

      for (toxy = 0; toxy < MNST; toxy++)
      {
         if (menv[toxy].type == -1)
            continue;
         if (mons_near(toxy))
         {
            strcpy(info, "You freeze ");
            strcat(info, monam(menv[toxy].number, menv[toxy].type, menv[toxy].enchantment[2], 1));
            strcat(info, ".");
            mpr(info);
            int hurted = 3 + random2(7) + random2(pow) / 20;

            beam[0].flavour = 3;
         //   o = toxy;
            hurted = check_mons_resists(beam, toxy, hurted);
            menv[toxy].hit_points -= hurted;
            if (menv[toxy].hit_points <= 0)
            {
               monster_die(toxy, 1, 0);
            }
            else
               print_wounds(toxy);
            // assumes only you can cast this spell (or would want to)
         }

      }                           // end loop

   }                               // end toxic rad

   void drain_life(int pow)
   {

      unsigned char toxy = 0;
      int hp_gain = 0;

      strcpy(info, "You draw life from your surroundings.");
      mpr(info);

      show_green = DARKGREY;
      viewwindow(1, false);
      more();
      mesclr();

      for (toxy = 0; toxy < MNST; toxy++)
      {
         if (menv[toxy].type == -1)
            continue;
         if (mons_holiness(menv[toxy].type) > 0)
            continue;
         if (mons_near(toxy))
         {
            strcpy(info, "You draw life from ");
            strcat(info, monam(menv[toxy].number, menv[toxy].type, menv[toxy].enchantment[2], 1));
            strcat(info, ".");
            mpr(info);
            int hurted = 3 + random2(7) + random2(pow);

            menv[toxy].hit_points -= hurted;
            hp_gain += hurted / 2;
            if (menv[toxy].hit_points <= 0)
            {
               monster_die(toxy, 1, 0);
            }
            else
               print_wounds(toxy);
            // assumes only you can cast this spell (or would want to)
         }

      }                           // end loop

      if (hp_gain > pow * 2)
         hp_gain = pow * 2;
      you.hp += hp_gain;
      if (you.hp > you.hp_max)
         you.hp = you.hp_max;
      if (hp_gain != 0)
         mpr("You feel life flooding into your body.");
      you.redraw_hit_points = 1;

   }

   int vampiric_drain(int pow)
   {
      int inflicted = 0, mgr = 0;
      struct dist vmove[1];

   dirc:
      strcpy(info, "Which direction?");
      mpr(info);
      direction(0, vmove);

      if (vmove[0].nothing == -1)
      {
         strcpy(info, "The spell fizzles!");
         mpr(info);
         return -1;
      }

      if (abs(vmove[0].move_x) > 1 || abs(vmove[0].move_y) > 1)
      {
         mpr("This spell doesn't reach that far.");
         return -1;
      }

      mgr = mgrd[you.x_pos + vmove[0].move_x][you.y_pos + vmove[0].move_y];


      if (vmove[0].move_x == 0 && vmove[0].move_y == 0)
      {
         strcpy(info, "That would be silly!");
         mpr(info);
         goto dirc;
      }

      if (mgr == MNG)
      {
         mpr("There isn't anything there!");
         return -1;
      }

      if (mons_holiness(menv[mgr].type) > 0)
      {
         strcpy(info, "Oops! That was rather foolish.");
         mpr(info);
         you.hp -= random2(20) + random2(20) + 10;
         if (you.hp <= 1)
            you.hp = 1;
         you.redraw_hit_points = 1;
         return -1;
      }

      inflicted = 3 + random2(5) + random2(5) + random2(pow) / 7;
      if (inflicted >= menv[mgr].hit_points)
         inflicted = menv[mgr].hit_points;
      if (inflicted >= you.hp_max - you.hp)
         inflicted = you.hp_max - you.hp;
      if (inflicted == 0)
      {
         mpr("Nothing appears to happen.");
         return -1;
      }

      menv[mgr].hit_points -= inflicted;

      strcpy(info, "You feel life coursing from ");
      strcat(info, monam(menv[mgr].number, menv[mgr].type, menv[mgr].enchantment[2], 1));
      strcat(info, " into your body!");
      mpr(info);
      print_wounds(mgr);

      if (menv[mgr].hit_points <= 0)
      {
         monster_die(mgr, 1, 0);
      }

      you.hp += inflicted / 2;
      if (you.hp > you.hp_max)
         you.hp = you.hp_max;
      you.redraw_hit_points = 1;

      vmove[0].move_x = 0;
      vmove[0].move_y = 0;

      return 1;

   }                               // end vamp drain


   int burn_freeze(int pow, char flavour)
   {
      int mgr = MNG;
      struct dist bmove[1];

      while (mgr == MNG)
      {
         mpr("Which direction?");
         direction(0, bmove);

         if (bmove[0].nothing == -1)
         {
            mpr("The spell fizzles!");
            bmove[0].move_x = 0;
            bmove[0].move_y = 0;
            return -1;
         }

         if ((abs(bmove[0].move_x) > 1) || (abs(bmove[0].move_y) > 1))
         {
            mpr("This spell doesn't reach that far.");
            return -1;
         }

         if (bmove[0].move_x == 0 && bmove[0].move_y == 0)
         {
            mpr("That would be silly!");
            continue;
         }

         mgr = mgrd [you.x_pos+bmove[0].move_x] [you.y_pos+bmove[0].move_y];

         // Yes, this is stange, but it does maintain the original behaviour
         if (mgr == MNG)
         {
            mpr("There isn't anything close enough!");
            bmove[0].move_x = 0;
            bmove[0].move_y = 0;
            return -1;
         }
      }

      switch (flavour)
      {
         case BEAM_FIRE:
            strcpy(info, "You burn ");
            break;
         case BEAM_COLD:
            strcpy(info, "You freeze ");
            break;
         case BEAM_MISSILE:
            strcpy(info, "You crush ");
            break;
         case BEAM_ELECTRICITY:
            strcpy(info, "You zap ");
            break;
      };

      strcat( info, monam(menv[mgr].number, menv[mgr].type,
                         menv[mgr].enchantment[2], 1) );
      strcat(info, ".");
      mpr(info);

      int hurted = 1 + random2(4) + random2(3) + random2(pow) / 25;

      struct bolt beam[1];

      beam[0].flavour = flavour;
      if (flavour != BEAM_MISSILE)
         hurted = check_mons_resists(beam, mgr, hurted);

      if (hurted) {
         menv[mgr].hit_points -= hurted;

         if (menv[mgr].hit_points <= 0)
         {
            monster_die(mgr, 1, 0);
         }
         else
         {
            print_wounds(mgr);
         }
      }

      return 1;
   }                               // end burn_freeze


   int summon_elemental(int pow, unsigned char restricted_type, unsigned char unfriendly)
   {
      int type_summoned = 0;
      int numsc = 21 + random2(pow) / 5;

      char summ_success = 0;
      struct dist smove[1];

      if (numsc > 25)
         numsc = 25;

   dirc:
      strcpy(info, "Summon from what material?");
    // cannot summon earth elemental if you are floating in the air.
    // problem: what if you're floating over water/lava and are surrounded by it and a wall, and summon an earth elemental? hmmm...
    //strcat(info, ", < for air)");
      mpr(info);
      strcpy(info, "Which direction?");
      mpr(info);
      direction(0, smove);

      if (smove[0].nothing == -1)
      {
      fizzles:
         strcpy(info, "Nothing appears to happen.");
         mpr(info);
         return -1;
      }

      if (mgrd[you.x_pos + smove[0].move_x][you.y_pos + smove[0].move_y] != MNG)
      {
         strcpy(info, "Not there!");
         mpr(info);
         goto dirc;
      }

      if (abs(smove[0].move_x) > 1 || abs(smove[0].move_y) > 1)
      {
         strcpy(info, "This spell doesn't reach that far.");
         mpr(info);
         return -1;
      }

      if (smove[0].move_x == 0 && smove[0].move_y == 0)
      {
         strcpy(info, "You can't summon an elemental from yourself!");
         mpr(info);
         goto dirc;
      }

      if (grd[you.x_pos + smove[0].move_x][you.y_pos + smove[0].move_y] == 1)
      {
         type_summoned = 123;
         if (restricted_type != 0 && type_summoned != restricted_type)
            goto summon_it;
         if (you.x_pos + smove[0].move_x > 6 && you.x_pos + smove[0].move_x < 74 && you.y_pos + smove[0].move_y > 6 && you.y_pos + smove[0].move_y < 64)
            grd[you.x_pos + smove[0].move_x][you.y_pos + smove[0].move_y] = 67;
         goto summon_it;
      }

      if (env.cgrid[you.x_pos + smove[0].move_x][you.y_pos + smove[0].move_y] != CNG && env.cloud_type[env.cgrid[you.x_pos + smove[0].move_x][you.y_pos + smove[0].move_y]] % 100 == 1)
      {
         type_summoned = 124;
         if (restricted_type != 0 && type_summoned != restricted_type)
            goto summon_it;
         env.cloud_type[env.cgrid[you.x_pos + smove[0].move_x][you.y_pos + smove[0].move_y]] = 0;
         env.cloud_decay[env.cgrid[you.x_pos + smove[0].move_x][you.y_pos + smove[0].move_y]] = 0;
         env.cgrid[you.x_pos + smove[0].move_x][you.y_pos + smove[0].move_y] = CNG;
         env.cloud_no--;
         goto summon_it;
      }

      if (grd[you.x_pos + smove[0].move_x][you.y_pos + smove[0].move_y] >= 67 && env.cgrid[you.x_pos + smove[0].move_x][you.y_pos + smove[0].move_y] == CNG)
      {
         type_summoned = 125;
         goto summon_it;
      }

      if (grd[you.x_pos + smove[0].move_x][you.y_pos + smove[0].move_y] == 62 || grd[you.x_pos + smove[0].move_x][you.y_pos + smove[0].move_y] == 65 || grd[you.x_pos + smove[0].move_x][you.y_pos + smove[0].move_y] == 200)
      {
        /* water squares or fountains */
         type_summoned = MWATER4;
         goto summon_it;
      }

      goto fizzles;

   summon_it:
      if (restricted_type != 0 && type_summoned != restricted_type)
      {
         strcpy(info, "Nothing appears to happen.");
         mpr(info);
         return 0;
      }
      if (random2(100) <= unfriendly || (type_summoned == 124 && random2(5) >= you.skills[SK_FIRE_MAGIC]) || (type_summoned == MWATER4 && random2(5) >= you.skills[SK_ICE_MAGIC]) || (type_summoned == 125 && random2(5) >= you.skills[SK_AIR_MAGIC]) || (type_summoned == 123 && random2(5) >= you.skills[SK_EARTH_MAGIC]))
      {
         strcpy(info, "The elemental doesn't seem to appreciate being summoned.");
         mpr(info);
         summ_success = create_monster(type_summoned, numsc, 0, you.x_pos + smove[0].move_x, you.y_pos + smove[0].move_y, MHITYOU, 250);
      }
      else
         summ_success = create_monster(type_summoned, numsc, 7, you.x_pos + smove[0].move_x, you.y_pos + smove[0].move_y, MHITNOT, 250);


      return 1;

   }                               // end summ elementals





   void summon_small_mammals(void)
   {

      if (random2(2) == 0)
         create_monster(MONS_RAT, 22, 7, you.x_pos, you.y_pos, MHITNOT, 250);
      else
         create_monster(MONS_GIANT_BAT, 22, 7, you.x_pos, you.y_pos, MHITNOT, 250);
   }                               // end of summon_small_mammals

   void summon_scorpions(int pow)
   {
      int numsc;
      int scount = 0;

      numsc = 1 + random2(pow) / 10 + random2(pow) / 10;

      if (numsc > 2)
         numsc = (numsc - 2) / 2 + 2;
      if (numsc > 4)
         numsc = (numsc - 4) / 2 + 4;
      if (numsc > 6)
         numsc = (numsc - 6) / 2 + 6;

      if (numsc > 8)
         numsc = 8;

      for (scount = 0; scount < numsc; scount++)
      {
         if (random2(pow) <= 3)
         {
            if (create_monster(MONS_SCORPION, 22, 1, you.x_pos, you.y_pos, MHITYOU, 250) != -1)
            {
               strcpy(info, "A scorpion appears. It doesn't look very happy.");
               mpr(info);
            }
         }
         else
         {
            if (create_monster(MONS_SCORPION, 22, 7, you.x_pos, you.y_pos, MHITNOT, 250) != -1)
            {
               strcpy(info, "A scorpion appears.");
               mpr(info);
            }
         }
      }

   }                               // end of summon_scopions


   void summon_ice_beast_etc(int pow, int ibc)
   {
      int numsc = 21 + random2(pow) / 4;

      if (numsc > 25)
         numsc = 25;
      int beha = 7;

      switch (ibc)
      {
         case 34:
            mpr("A chill wind blows around you.");
            break;

         case 8:
            mpr("A beastly little devil appears in a puff of flame.");
            break;

         case 220:
            mpr("A beastly little devil appears in a puff of frigid air.");
            break;

         case 237:
            mpr("A shadowy apparition takes form in the air.");
            break;

         case 26:
            mpr("You open a gate to the realm of Zin!");
            break;

         case 366:
            mpr("You are momentarily dazzled by a brilliant golden light.");
            break;

      /* case 225:
      case 226:
      case 227:
      case 228:
      case 229: */
         default:
            mpr("A demon appears!");
            if (random2(pow) <= 3)
            {
               beha = 1;
               mpr("It doesn't look very happy.");
            }
            break;

      }
      create_monster(ibc, numsc, beha, you.x_pos, you.y_pos, MHITNOT, 250);

   }                               // end of summon_ice_beast_etc

   void summon_swarm(int pow)
   {
      int numsc = 1 + random2(pow) / 2;
      int scount = 0;
      int thing_called = 0;

      numsc = 1 + random2(pow) / 25 + random2(pow) / 25;

      if (numsc > 2)
         numsc = (numsc - 2) / 2 + 2;
      if (numsc > 4)
         numsc = (numsc - 4) / 2 + 4;
      if (numsc > 6)
         numsc = (numsc - 6) / 2 + 6;

      if (numsc > 8)
         numsc = 8;

      for (scount = 0; scount < numsc; scount++)
      {
         switch (random2(14))
         {
            case 10:
            case 12:
            case 0:
               thing_called = 0;
               break;              // giant ant

            case 13:
            case 1:
               thing_called = 10;
               break;              // killer bee

            case 2:
               thing_called = 18;
               break;              //   " larva

            case 3:
               thing_called = 22;
               break;              // scorpion

            case 11:
            case 4:
               thing_called = 24;
               break;              // wasp

            case 5:
               thing_called = 27;
               break;              // B

            case 6:
               thing_called = 42;
               break;              // Q

            case 8:
               thing_called = 62;
               break;              // wolf spider

            case 9:
               thing_called = 66;
               break;              // butterfly

         }                       // end switch

         create_monster(thing_called, 22, 1 + (random2(pow) > 7) * 6, you.x_pos, you.y_pos, MHITNOT, 250);
      }
      strcpy(info, "You call forth a swarm of pestilential beasts!");
      mpr(info);
   }                               // end of summon_swarm

   void summon_undead(int pow)
   {
      int numsc = 1 + random2(pow) / 3;
      int scount = 0;
      int thing_called = 0;

      numsc = 1 + random2(pow) / 30 + random2(pow) / 30;

      if (numsc > 2)
         numsc = (numsc - 2) / 2 + 2;
      if (numsc > 4)
         numsc = (numsc - 4) / 2 + 4;
      if (numsc > 6)
         numsc = (numsc - 6) / 2 + 6;

      if (numsc > 8)
         numsc = 8;

      strcpy(info, "You call on the undead to aid you!");
      mpr(info);


      for (scount = 0; scount < numsc; scount++)
      {

         thing_called = 48;      // wraith

         if (random2(5) == 0)
            thing_called = 77;
         if (random2(5) == 0)
            thing_called = 130;

         if (random2(pow) <= 5)
         {
            if (create_monster(thing_called, 22, 1, you.x_pos, you.y_pos, MHITYOU, 250) != -1)
            {
               strcpy(info, "You sense a hostile presence.");
               mpr(info);
            }
         }
         else
         {
            if (create_monster(thing_called, 22, 7, you.x_pos, you.y_pos, MHITNOT, 250) != -1)
            {
               strcpy(info, "An insubstantial figure forms in the air.");
               mpr(info);
            }
         }

      }
      if (you.is_undead == 0)
      {
         strcpy(info, "You feel rather ill.");
         mpr(info);
         you.disease = 200;
      }
   }                               // end of summon_undead


   void summon_things(int pow)
   {
      int numsc = 1 + random2(pow) / 30 + random2(pow) / 30;
      int big_things = 0;

   //int little_things = 0;

      if (numsc > 2)
         numsc = (numsc - 2) / 2 + 2;
      if (numsc > 4)
         numsc = (numsc - 4) / 2 + 4;
      if (numsc > 6)
         numsc = (numsc - 6) / 2 + 6;

      while (numsc > 2)
      {
         if (random2(4) == 0)
            break;
         numsc -= 2;
         big_things++;
      }

      if (numsc > 8)
         numsc = 8;
      if (big_things > 8)
         big_things = 8;

   //for (scount = 0; scount < numsc; scount ++)
      while (big_things > 0)
      {
         create_monster(MONS_SMALL_ABOMINATION, 22, 7, you.x_pos, you.y_pos, MHITNOT, 250);
         big_things--;
      }

      while (numsc > 0)
      {
         create_monster(MONS_LARGE_ABOMINATION, 22, 7, you.x_pos, you.y_pos, MHITNOT, 250);
         numsc--;
      }

      strcpy(info, "Something answered your call!");
      mpr(info);

      you.intel--;
      you.redraw_intelligence = 1;
      strcpy(info, "Your brain shrivels slightly.");
      mpr(info);

   }                               // end of summon_things

   void summon_butter(void)
   {
      int scount = 0;

      for (scount = 0; scount < 8; scount++)
      {
         create_monster(MONS_BUTTERFLY, 21, 7, you.x_pos, you.y_pos, MHITNOT, 250);
      }
   // maybe in some rare circumstances produce caterpillars?
   }                               // end butterflies

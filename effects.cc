/*
 *  File:       effects.cc
 *  Summary:    Misc stuff.
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 *               <1>     -/--/--        LRH             Created
 */

#include "AppHdr.h"
#include "effects.h"

#include <string.h>

#include "externs.h"

#include "beam.h"
#include "direct.h"
#include "files.h"
#include "fight.h"
#include "player.h"
#include "ouch.h"
#include "mstruct.h"
#include "misc.h"
#include "mutation.h"
#include "player.h"
#include "stuff.h"
#include "shopping.h"
#include "dungeon.h"
#include "itemname.h"
#include "skills2.h"
#include "view.h"
#include "spells0.h"
#include "spells2.h"
#include "spells3.h"


   void torment()
   {
      int dmi = 0;

      if (you.is_undead || you.mutation[MUT_TORMENT_RESISTANCE] != 0)
      {
         strcpy(info, "You feel a surge of unholy energy.");
         mpr(info);
      }
      else
      {
         strcpy(info, "Your body is wracked with pain!");
         mpr(info);
        //you.hp -= (you.hp / 2) - 1;
         int tormdam = (you.hp / 2) - 1;

         if (tormdam <= 0)
            tormdam = 0;
         ouch(tormdam, 0, KILLED_BY_MONSTER);
         you.redraw_hit_points = 1;
      }

      for (dmi = 0; dmi < MNST; dmi++)
      {
         if (menv[dmi].type == -1)
            continue;

         if (!mons_near(dmi))
            continue;

        //              int mres = random2(100);
        //              int mres2 = mon_resist_mag(mons_class [dmi], mons_HD [dmi]);

        //              if (mres < mres2) continue;

         if (mons_holiness(menv[dmi].type) > 0)
            continue;

        // maybe an exemption for undead?
        // maybe do something fun for magic circles?

         menv[dmi].hit_points -= (menv[dmi].hit_points / 2) - 1;

         if (menv[dmi].enchantment[2] == ENCH_INVIS)
            continue;           // can't see it

         strcpy(info, monam(menv[dmi].number, menv[dmi].type, menv[dmi].enchantment[2], 0));
         strcat(info, " convulses!");
         mpr(info);
      }

   }


/*
   char go_berserk(void)
   {
   if (you.berserker != 0 || you.slow != 0) return 0;
   if (you.is_undead == US_UNDEAD) return 0;
   strcpy(info, "A red film seems to cover your vision as you go berserk!");
   mpr(info);
   strcpy(info, "You feel yourself moving faster!");
   mpr(info);
   strcpy(info, "You feel mighty!");
   mpr(info);
   you.berserker += 10 + random2(10) + random2(10);
   if (you.might == 0)
   {
   you.redraw_strength = 1;
   you.strength += 5;
   you.max_strength += 5;
   }
   you.might += you.berserker;
   you.haste += you.berserker;
   return 1;
   }
 */
   void banished(unsigned char gate_type)
   {
      you_teleport2(false);           // this is to ensure that you're standing on a suitable space (67)

      grd[you.x_pos][you.y_pos] = gate_type;
      down_stairs(1, you.your_level);     // heh heh

   }


   char forget_spell(void)
   {
      char spc2;

      if (you.spell_no <= 0)
         return 0;

      do
      {
         spc2 = random2(20);
         if ( one_chance_in(1000) )
            return 0;           // safety check

      }
      while (you.spells[spc2] == SPELL_NO_SPELL);


    // This is now handled by a function in player.cc
    //you.spell_levels += spell_value(you.spells[spc2]);
      you.spell_no--;
      you.spells[spc2] = SPELL_NO_SPELL;
      return 1;
   }


   char lose_stat(char force_stat, char stat_loss)
   {

      if (force_stat == 100)
         force_stat = random2(3);

      if (player_sust_abil() != 0)
      {
         strcpy(info, "You feel slightly ill for a moment.");
         mpr(info);
         return 0;
      }
      switch (force_stat)
      {
         case STAT_STRENGTH:
            strcpy(info, "You feel weakened.");
            mpr(info);
            you.strength -= stat_loss;
            if (you.strength < 3)
               you.strength = 3;
            you.redraw_strength = 1;
            burden_change();
            break;

         case STAT_DEXTERITY:
            strcpy(info, "You feel clumsy.");
            mpr(info);
            you.dex -= stat_loss;
            if (you.dex < 3)
               you.dex = 3;
            you.redraw_dexterity = 1;
            break;

         case STAT_INTELLIGENCE:
            strcpy(info, "You feel dopey.");
            mpr(info);
            you.intel -= stat_loss;
            if (you.intel < 3)
               you.intel = 3;
            you.redraw_intelligence = 1;
            break;

      }

      return 1;

   }





   void direct_effect(struct bolt beam[1])
   {
      int hurted = 0;

      switch (beam[0].type)
      {
         case DMNBM_HELLFIRE:
            strcpy(info, "You are engulfed in a burst of hellfire!");
            mpr(info);
            hurted = random2(10) + random2(10) + 5;
            beam[0].flavour = BEAM_LAVA;    // but it's hellfire anyway

            strcpy(beam[0].beam_name, "hellfire");  // for ouch

            hurted = check_your_resists(hurted, beam[0].flavour);
            scrolls_burn(4, OBJ_SCROLLS);
            ouch(hurted, beam[0].beam_source, KILLED_BY_BEAM);
            you.redraw_hit_points = 1;
            break;

         case DMNBM_SMITING:
            strcpy(info, "Something smites you!");
            mpr(info);
            hurted = random2(6) + random2(6) + 7;
            strcpy(beam[0].beam_name, "smiting");   // for ouch

            ouch(hurted, beam[0].beam_source, KILLED_BY_BEAM);
            you.redraw_hit_points = 1;
            break;

         case DMNBM_BRAIN_FEED:
            if ( you.intel > 3 && player_sust_abil() == 0 && one_chance_in(3) )
            {
               strcpy(info, "Something feeds on your intelligence!");
               mpr(info);
               strcpy(info, "You feel stupid.");
               mpr(info);
               you.intel--;
               you.redraw_intelligence = 1;
            }
            else
            {
               strcpy(info, "Something tries to feed on your intelligence!");
               mpr(info);
            }
            break;

         case DMNBM_MUTATION:
            mpr("Strange energies course through your body.");
            if (mutate(100) == 0)
               mpr("You feel very weird for a moment.");
            break;
      }

      return;
   }


   void mons_direct_effect(struct bolt beam[1], int i)
   {

      int o = menv[i].monster_foe;
      int hurted = 0;

      switch (beam[0].type)
      {


         case DMNBM_HELLFIRE:
            if (mons_near(menv[i].monster_foe))
            {
               strcpy(info, monam(menv[o].number, menv[o].type, menv[o].enchantment[2], 0));
               strcat(info, " is engulfed in hellfire.");
               mpr(info);
            }
            strcpy(beam[0].beam_name, "hellfire");
            beam[0].flavour = BEAM_LAVA;
            hurted = 0;
            hurted += random2(10) + random2(5) + 5;
            check_mons_resists(beam, o, hurted);
            menv[o].hit_points -= hurted;
            if (menv[o].behavior == 0)
               menv[o].behavior = 1;
            if (menv[o].hit_points <= 0)
            {
               monster_die(o, KILL_MON_MISSILE, i);
            }
            break;

         case DMNBM_SMITING:
            if (mons_near(o))
            {
               strcpy(info, monam(menv[o].number, menv[o].type, menv[o].enchantment[2], 0));
               strcat(info, " is smitten.");
               mpr(info);
            }
            strcpy(beam[0].beam_name, "smiting");
            beam[0].flavour = BEAM_MISSILE;
            hurted = 0;
            hurted += random2(6) + random2(6) + 7;
            menv[o].hit_points -= hurted;
            if (menv[o].hit_points <= 0)
            {
               monster_die(o, KILL_MON_MISSILE, i);
            }
            break;


         case DMNBM_BRAIN_FEED:      // brain feed - not implemented here (nor, probably, can be)

            break;

         case DMNBM_MUTATION:
            if (check_mons_magres(o, beam[0].ench_power) == 0)
            {
               if (mons_near(o))
               {
                  strcpy(info, monam(menv[o].number, menv[o].type, menv[o].enchantment[2], 0));
                  strcat(info, " resists.");
                  mpr(info);
               }
               break;
            }
            monster_polymorph(o, 250, 100);
            break;

      }                           // end switch

   }                               // end monster_direct_effect()



   void random_uselessness(unsigned char ru, unsigned char sc_read_2)
   {

      char wc[30];

      switch (ru)
      {
         case 0:
            strcpy(info, "The dust glows a ");
            weird_colours(random2(256), wc);
            strcat(info, wc);
            strcat(info, " colour!");
            mpr(info);
            break;

         case 1:
            strcpy(info, "The scroll reassembles itself in your hand!");
            you.inv_quantity[sc_read_2]++;
            mpr(info);
            burden_change();
            break;

         case 2:
            if (you.equip[EQ_WEAPON] != -1)
            {
               in_name(you.equip[EQ_WEAPON], 4, str_pass);
               strcpy(info, str_pass);
               strcat(info, " glows ");
               weird_colours(random2(256), wc);
               strcat(info, wc);
               strcat(info, " for a moment.");
               mpr(info);
               return;
            //strcat(info
            }
            else
            {
               strcpy(info, "Nothing appears to happen.");
               mpr(info);
               return;
            }
         //      break;

         case 3:
            strcpy(info, "You hear the distant roaring of an enraged ");
            switch (random2(8))
            {
               case 0:
                  strcat(info, "frog!");
                  break;
               case 1:
                  strcat(info, "pill bug!");
                  break;
               case 2:
                  strcat(info, "millipede!");
                  break;
               case 3:
                  strcat(info, "eggplant!");
                  break;
               case 4:
                  strcat(info, "albino dragon!");
                  break;
               case 5:
                  strcat(info, "dragon!");
                  break;
               case 6:
                  strcat(info, "human!");
                  break;
               case 7:
                  strcat(info, "slug!");
                  break;
            }
            mpr(info);
            return;

         case 4:
            strcpy(info, "You smell ");
            switch (random2(8))
            {
               case 0:
                  strcat(info, "coffee.");
                  break;
               case 1:
                  strcat(info, "salt.");
                  break;
               case 2:
                  strcat(info, "burning hair!");
                  break;
               case 3:
                  strcat(info, "baking bread.");
                  break;
               case 4:
                  strcat(info, "something weird.");
                  break;
               case 5:
                  strcat(info, "wet wool.");
                  break;
               case 6:
                  strcat(info, "sulphur.");
                  break;
               case 7:
                  strcat(info, "fire and brimstone!");
                  break;
            }
            mpr(info);
            return;



         case 5:
            strcpy(info, "You experience a momentary feeling of inescapable doom!");
            mpr(info);
            return;

         case 6:
            strcpy(info, "Your ");
            switch (random2(3))
            {
               case 0:
                  strcat(info, "ears itch.");
                  break;
               case 1:
                  strcat(info, "brain hurts!");
                  break;
               case 2:
                  strcat(info, "nose twitches suddenly!");
                  break;
            }
            mpr(info);
            return;

         case 7:
            strcpy(info, "You hear the tinkle of a tiny bell.");
            mpr(info);
            summon_butter();
            return;

         case 8:
            strcpy(info, "You hear ");
            switch (random2(9))
            {
               case 0:
                  strcat(info, "snatches of song.");
                  break;
               case 1:
                  strcat(info, "a voice call someone else's name.");
                  break;
               case 2:
                  strcat(info, "a very strange noise.");
                  break;
               case 3:
                  strcat(info, "roaring flame.");
                  break;
               case 4:
                  strcat(info, "a very strange noise indeed.");
                  break;
               case 5:
                  strcat(info, "the chiming of a distant gong.");
                  break;
               case 6:
                  strcat(info, "the bellowing of a yak.");
                  break;
               case 7:
                  strcat(info, "a crunching sound.");
                  break;
               case 8:
                  strcat(info, "the tinkle of an enormous bell.");
                  break;
            }
            mpr(info);
            return;


      }

    //abort();

   }

   void acquirement(unsigned char force_class)
   {
      int thing_created = 0;

    // Remember lava!
      unsigned char class_wanted = 250;
      unsigned char type_wanted = 250;

      unsigned char force_plus = 0;

      unsigned char unique = 1;

      unsigned char acqc = 0;
      char already_has[50];

      char glof = 99;
      unsigned char keyin;
      int func_pass[10];

      for (acqc = 0; acqc < 50; acqc++)
      {
         already_has[acqc] = 0;
      }

      if (force_class == 250)
      {

         mpr("This is a scroll of acquirement!");
      query:
         mpr("a - Weapon, b - Armour, c - Jewellery,");
         mpr("d - Book, e - Staff, f - Miscellaneous, g - Food");
         mpr("What kind of item would you like to acquire? ");
         keyin = get_ch();

         switch (keyin)
         {
            case 'a':
            case 'A':
               class_wanted = OBJ_WEAPONS;
               break;

            case 'b':
            case 'B':
               class_wanted = OBJ_ARMOUR;
               break;

            case 'c':
            case 'C':
               class_wanted = OBJ_JEWELLERY;
               break;

            case 'd':
            case 'D':
               class_wanted = OBJ_BOOKS;
               break;

            case 'e':
            case 'E':
               class_wanted = OBJ_STAVES;
               break;

            case 'f':
            case 'F':
               class_wanted = OBJ_MISCELLANY;
               break;

            case 'g':
            case 'G':
               class_wanted = OBJ_FOOD;
               break;
         }
      }
      else
         class_wanted = force_class;

      if (class_wanted > OBJ_ARMOUR)
      {
         for (acqc = 0; acqc < 52; acqc++)
         {
            if (you.inv_quantity[acqc] > 0 && you.inv_class[acqc] == class_wanted)
            {
               already_has[you.inv_type[acqc]] = 1;
            }
         }
         do
         {
            switch (class_wanted)
            {
               case OBJ_JEWELLERY:
                  // Try for a base type the player hasn't identified
                  for (int i = 0; i < 10; i++)
                  {
                     type_wanted = random2(24);
                     if ( one_chance_in(3))
                        type_wanted = 35 + random2(10);

                     if (!get_id(OBJ_JEWELLERY, type_wanted))
                        break;
                  }
                  break;

               case OBJ_BOOKS:
                  // remember, put rarer books higher in the list
                  type_wanted = 99;
                  glof = best_skill(26, 50, glof);
               which_book:
                  switch (glof)
                  {
                     default:
                     case SK_SPELLCASTING:
                        if ( !you.had_item[BOOK_POWER] )
                           type_wanted = BOOK_POWER;
                        if ( !you.had_item[BOOK_WIZARDRY] )
                           type_wanted = BOOK_WIZARDRY;
                        break;

                     case SK_POISON_MAGIC:
                        if ( !you.had_item[BOOK_TOXINS] )
                           type_wanted = BOOK_TOXINS;
                        if ( !you.had_item[BOOK_YOUNG_POISONERS] )
                           type_wanted = BOOK_YOUNG_POISONERS;
                        break;

                     case SK_EARTH_MAGIC:
                        if ( !you.had_item[BOOK_EARTH] )
                           type_wanted = BOOK_EARTH;
                        if ( !you.had_item[BOOK_GEOMANCY] )
                           type_wanted = BOOK_GEOMANCY;
                        break;

                     case SK_AIR_MAGIC:
                        if ( !you.had_item[BOOK_SKY] )
                           type_wanted = BOOK_SKY;
                        if ( !you.had_item[BOOK_AIR] )
                           type_wanted = BOOK_AIR;
                        break;

                     case SK_ICE_MAGIC:
                        if ( !you.had_item[BOOK_ICE] )
                           type_wanted = BOOK_ICE;
                        if ( !you.had_item[BOOK_FROST] )
                           type_wanted = BOOK_FROST;
                        break;

                     case SK_FIRE_MAGIC:
                        if ( !you.had_item[BOOK_FIRE] )
                           type_wanted = BOOK_FIRE;
                        if ( !you.had_item[BOOK_FLAMES] )
                           type_wanted = BOOK_FLAMES;
                        break;

                     case SK_SUMMONINGS:
                        if ( !you.had_item[BOOK_DEMONOLOGY] )
                           type_wanted = BOOK_DEMONOLOGY;
                        if ( !you.had_item[BOOK_INVOCATIONS] )
                           type_wanted = BOOK_INVOCATIONS;
                        if ( !you.had_item[BOOK_SUMMONINGS] )
                           type_wanted = BOOK_SUMMONINGS;
                        break;

                     case SK_ENCHANTMENTS:
                        if ( !you.had_item[BOOK_ENCHANTMENTS] )
                           type_wanted = BOOK_ENCHANTMENTS;
                        if ( !you.had_item[BOOK_WAR_CHANTS] )
                           type_wanted = BOOK_WAR_CHANTS;
                        if ( !you.had_item[BOOK_CONTROL] )
                           type_wanted = BOOK_CONTROL;
                        if ( !you.had_item[BOOK_HINDERANCE] )
                           type_wanted = BOOK_HINDERANCE;
                        if ( !you.had_item[BOOK_CHARMS] )
                           type_wanted = BOOK_CHARMS;
                        break;

                     case SK_CONJURATIONS:
                        if ( !you.had_item[BOOK_ANNIHILATIONS] )
                           type_wanted = BOOK_ANNIHILATIONS;
                        if ( !you.had_item[BOOK_CONJURATIONS_I] )
                           type_wanted = BOOK_CONJURATIONS_I;
                        break;

                     case SK_NECROMANCY:
                        if ( !you.had_item[BOOK_NECRONOMICON] )
                           type_wanted = BOOK_NECRONOMICON;
                        if ( !you.had_item[BOOK_UNLIFE] )
                           type_wanted = BOOK_UNLIFE;
                        if ( !you.had_item[BOOK_DEATH] )
                           type_wanted = BOOK_DEATH;
                        if ( !you.had_item[BOOK_NECROMANCY] )
                           type_wanted = BOOK_NECROMANCY;
                        break;

                     case SK_TRANSLOCATIONS:
                        if ( !you.had_item[BOOK_SPATIAL_TRANSLOCATIONS] )
                           type_wanted = BOOK_SPATIAL_TRANSLOCATIONS;
                        if ( !you.had_item[BOOK_WARP] )
                           type_wanted = BOOK_WARP;
                        break;

                     case SK_TRANSMIGRATION:
                        if ( !you.had_item[BOOK_TRANSFIGURATIONS] )
                           type_wanted = BOOK_TRANSFIGURATIONS;
                        if ( !you.had_item[BOOK_CHANGES] )
                           type_wanted = BOOK_CHANGES;
                        break;
                  }

                  if (type_wanted == 99 && glof == best_skill(26, 50, 99))
                  {
                     glof = best_skill(26, 50, best_skill(26, 50, 99));
                     goto which_book;
                  }

                  // if we don't have a book, try and get a new one.
                  if (type_wanted == 99)
                  {
                     do
                     {
                        type_wanted = random2(NUM_BOOKS);
                        if ( one_chance_in(500) )
                           break;
                     }
                     while ( you.had_item[type_wanted] );
                  }

                  // if the book is invalid find any valid one.
                  while ( book_rarity(type_wanted) == 100
                           || type_wanted == BOOK_DESTRUCTION
                           || type_wanted == BOOK_MANUAL )
                  {
                     type_wanted = random2(NUM_BOOKS);
                  }

                  force_plus = SPBK_SIX_SLOTS;

                  // should guarantee a full spellbook, so:
                  spellbook_template(type_wanted, func_pass);
                  if (func_pass[6] == SPELL_NO_SPELL)
                     force_plus = SPBK_FIVE_SLOTS;
                  if (func_pass[5] == SPELL_NO_SPELL)
                     force_plus = SPBK_FOUR_SLOTS;
                  if (func_pass[4] == SPELL_NO_SPELL)
                     force_plus = SPBK_THREE_SLOTS;
                  if (func_pass[3] == SPELL_NO_SPELL)
                     force_plus = SPBK_TWO_SLOTS;
                  if (func_pass[2] == SPELL_NO_SPELL)
                     force_plus = SPBK_ONE_SLOT;
                  break;

               case OBJ_STAVES:
                  type_wanted = random2(18);
                  if ( class_wanted == OBJ_STAVES
                      && type_wanted > STAFF_SUMMONING_I
                      && !one_chance_in(5) )
                     type_wanted = random2(10);
                  break;

               case OBJ_MISCELLANY:
                  type_wanted = random2(6);
                  break;

               // BCR - You can now acquire food!
               case OBJ_FOOD:
                  unique = random2(7) + 4;
                  if ( you.species == SP_GHOUL )
                  {
                      if ( one_chance_in(3) )
                        type_wanted = FOOD_ROYAL_JELLY;
                      else
                      {
                        type_wanted = FOOD_CHUNK;
                        unique += 10;
                      }
                  }
                  else
                  {
                    type_wanted = FOOD_ROYAL_JELLY;
                  }
                  break;

               default:
                  mesclr();
                  goto query;
            }
         } while (already_has[type_wanted] == 1 && !one_chance_in(200) );

      }
      else if (class_wanted == OBJ_WEAPONS)
         type_wanted = 250;  // 1 + random2(18); // weapons - no clubs
      else
         type_wanted = 250;  // always get random armour

      if (grd[you.x_pos][you.y_pos] == DNGN_LAVA || grd[you.x_pos][you.y_pos] == DNGN_DEEP_WATER)
      {
         mpr("You hear a splash."); // how sad (and stupid)
      }
      else
      {
         // BCR - unique is now used for food quantity.
         thing_created = items(unique, class_wanted, type_wanted, 1, 351, 250);

         if (  type_wanted == 250
            && ( class_wanted == OBJ_WEAPONS
                  || class_wanted == OBJ_ARMOUR
                  || class_wanted == OBJ_JEWELLERY )
            )
         {
            if (mitm.pluses[thing_created] > 130)
               mitm.pluses[thing_created] -= 100;
         }

         if (you.species != SP_NAGA)
            strcpy(info, "Something appears at your feet!");
         else
            strcpy(info, "Something appears before you!");
         mpr(info);

         if (force_plus != 0)
            mitm.pluses[thing_created] = force_plus;

         int what_was_there = igrd[you.x_pos][you.y_pos];

         mitm.link[thing_created] = what_was_there;
         igrd[you.x_pos][you.y_pos] = thing_created;
      }
      return;
   }



   char recharge_wand(void)
   {

      if (you.equip[EQ_WEAPON] == -1)
      {
         return 0;
      }

      if (you.inv_class[you.equip[EQ_WEAPON]] != OBJ_WANDS)
      {
         return 0;               // not a wand

      }

      char charge_gain = 8;

      if (you.inv_type[you.equip[EQ_WEAPON]] == WAND_FIRE || you.inv_type[you.equip[EQ_WEAPON]] == WAND_COLD)
         charge_gain = 5;

      if (you.inv_type[you.equip[EQ_WEAPON]] == WAND_FIREBALL
                 || you.inv_type[you.equip[EQ_WEAPON]] == WAND_LIGHTNING
                 || you.inv_type[you.equip[EQ_WEAPON]] == WAND_DRAINING
                 || you.inv_type[you.equip[EQ_WEAPON]] == WAND_HEALING)
         charge_gain = 4;

      item_name(you.inv_plus2[you.equip[EQ_WEAPON]], you.inv_class[you.equip[EQ_WEAPON]], you.inv_type[you.equip[EQ_WEAPON
               ]],
               you.inv_dam[you.equip[EQ_WEAPON]], you.inv_plus[you.equip[EQ_WEAPON]], you.inv_quantity[you.equip[EQ_WEAPON]]
               ,you.inv_ident[you.equip[EQ_WEAPON]], 4, str_pass);
      strcpy(info, str_pass);
      strcat(info, " glows for a moment.");
      mpr(info);

      you.inv_plus[you.equip[EQ_WEAPON]] += random2(charge_gain) + random2(charge_gain) + random2(charge_gain) + 1;

      if (you.inv_plus[you.equip[EQ_WEAPON]] > charge_gain * 3)
         you.inv_plus[you.equip[EQ_WEAPON]] = charge_gain * 3;

      return 1;

   }


   void yell(void)
   {
      char targ_prev = 0;
      int mons_targd = 0;
      struct dist beam[1];

      mpr("What do you say?");
      mpr(" ! - Yell");
      mpr(" a - Order allies to attack a monster");
      if (you.prev_targ != MHITNOT && you.prev_targ < MNST)
         if (mons_near(you.prev_targ) && (menv[you.prev_targ].enchantment[2] != ENCH_INVIS || player_see_invis() != 0))
         {
            mpr(" p - Order allies to attack your previous target");
            targ_prev = 1;
         }
      strcpy(info, " Anything else - Stay silent ");
      if ( one_chance_in(10) )
         strcat(info, "(and be thought of as a fool)");
      mpr(info);

      char keyn = get_ch();

      switch (keyn)
      {

         case '!':
            strcpy(info, "You yell for attention!");
            mpr(info);
            you.turn_is_over = 1;
            noisy(12, you.x_pos, you.y_pos);
            return;

         case 'a':
            mpr("Gang up on whom?");
            direction(100, beam);

            if (beam[0].nothing == -1 || mgrd[beam[0].target_x][beam[0].target_y] == MNG)
            {
               mpr("Yeah, whatever.");
               return;
            }
            mons_targd = mgrd[beam[0].target_x][beam[0].target_y];
            break;


         case 'p':
            if (targ_prev == 1)
            {
               mons_targd = you.prev_targ;
               break;
            }
         /* fall through... */
         default:
            mpr("Okely-dokely.");
            return;
      }

      int i = 0;

      for (i = 0; i < MNST; i++)
      {
         if (menv[i].type == -1)
            continue;
         if (menv[i].behavior != BEH_ENSLAVED)
            continue;
         if (!mons_near(i))
            continue;
         menv[i].monster_foe = mons_targd;
         menv[i].target_x = menv[mons_targd].x;
         menv[i].target_y = menv[mons_targd].y;
      }

      noisy(10, you.x_pos, you.y_pos);
      mpr("Attack!");

   }

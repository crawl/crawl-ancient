#include "config.h"

#ifdef DOS
#include <conio.h>
#endif DOS

#include <string.h>
#include <stdlib.h>

#include "externs.h"

//#include "bang.h"
#include "beam.h"
//#include "chardump.h"
//#include "command.h"
//#include "debug.h"
//#include "decks.h"
//#include "describe.h"
//#include "direct.h"
#include "dungeon.h"
#include "effects.h"
#include "fight.h"
//#include "files.h"
//#include "food.h"
//#include "invent.h"
#include "it_use2.h"
#include "it_use3.h"
#include "item_use.h"
#include "itemname.h"
#include "items.h"
//#include "levels.h"
//#include "maps.h"
#include "message.h"
#include "misc.h"
#include "monplace.h"
#include "monstuff.h"
#include "mstruct.h"
#include "mstuff2.h"
#include "mutation.h"
//#include "newgame.h"
#include "ouch.h"
#include "output.h"
#include "player.h"
#include "priest.h"
#include "religion.h"
#include "shopping.h"
#include "skills.h"
#include "skills2.h"
#include "spell.h"
#include "spells.h"
#include "spells0.h"
#include "spells1.h"
#include "spells2.h"
#include "spells3.h"
#include "stuff.h"
#include "view.h"

int jelly_divide(int jel);
void place_monster_corpse(unsigned char mcr);


void you_attack(int monster_attacked)
{
int your_to_hit;
int damage_done = 0;
char hit = 0;
char stabbed = 0;
//int mmov_x = 0;
char str_pass [80];

int special_brand = 0;

/*
IMPORTANT: If damage routines are changed, must also change in ouch.cc
for saving of player ghosts.

*/


if (menv [monster_attacked].m_beh != 7) you[0].pet_target = monster_attacked;
if (menv [monster_attacked].m_beh == 7)
{
 menv [monster_attacked].m_beh = 1;
 naughty(5, 5);
}

your_to_hit = 13 + you[0].dex / 2;// + (0.5 * you[0].strength);// + (you[0].f_abil / 10); // / 100

if (wearing_amulet(43) == 1) your_to_hit -= 5;

if (you[0].equip [0] != -1)
{
     your_to_hit += random2(you[0].skills [0] + 1);

if (weapon_skill(you[0].inv_class [you[0].equip [0]], you[0].inv_type [you[0].equip [0]]) != 0)
  your_to_hit += random2(you[0].skills [weapon_skill(you[0].inv_class [you[0].equip [0]], you[0].inv_type [you[0].equip [0]])] + 1);
}


if (you[0].is_undead != 2) you[0].hunger -= 3;

if (you[0].equip [0] != -1 && you[0].inv_class [you[0].equip [0]] == 0 && you[0].inv_dam [you[0].equip [0]] % 30 == 9)
{
 you[0].time_taken *= 5;
 you[0].time_taken /= 10;
} /* weapon of speed */

if (you[0].special_wield != 0)
{

 switch(you[0].special_wield)
 {
  case 2:
  go_berserk();
  break;

  case 9:
  if (random2(9) == 0) miscast_effect(19, random2(9), random2(70), 100);
  break;

 }
}


if (you[0].equip [0] != -1)
{
 if (you[0].inv_class [you[0].equip [0]] == 0)
 {
        int ghoggl = you[0].inv_plus [you[0].equip [0]] - 50;
        if (you[0].inv_plus [you[0].equip [0]] > 130) ghoggl -= 100;

        if (ghoggl >= 0) your_to_hit += ghoggl;
                else your_to_hit += ghoggl;

        your_to_hit += property(0, you[0].inv_type [you[0].equip [0]], 1);

  if (you[0].inv_dam [you[0].equip [0]] / 30 == 4 && you[0].species >= 2 && you[0].species <= 6)
  {
   your_to_hit += 1 + random2(2);
  }

 }
 if (you[0].inv_class [you[0].equip [0]] == 11)
 {
   your_to_hit += 8;
 }
}

if (you[0].hung_state == 1)
{
 your_to_hit -= 3;
}

char heavy_armour = 0;

if (you[0].equip [5] != -1)
{
  if (you[0].inv_type [you[0].equip [5]] == 8 && you[0].skills [17] > random2(6)) heavy_armour ++;
  if (you[0].inv_type [you[0].equip [5]] == 14 && you[0].skills [17] > random2(10)) heavy_armour ++;
  if (you[0].inv_type [you[0].equip [5]] == 14 && you[0].skills [17] > random2(10)) heavy_armour ++;
  if (you[0].inv_type [you[0].equip [5]] == 14 && you[0].skills [17] > random2(10)) heavy_armour ++;
}

if (you[0].equip [6] != -1)
{
  if (property(2, you[0].inv_type [you[0].equip [5]], 1) > 0 && random2(you[0].skills [13]) > abs(property(2, you[0].inv_type [you[0].equip [5]], 1)))
        heavy_armour += random2(abs(property(2, you[0].inv_type [you[0].equip [5]], 1)));
}

your_to_hit -= heavy_armour;

your_to_hit = random2(your_to_hit);

int damage = 1;


if (you[0].equip [0] == -1) /* empty hands */
{
 if (you[0].attribute [5] != 0)
 {
  switch(you[0].attribute [5])
  {
   case 1: damage = 7; special_brand = 6; your_to_hit += random2(10); break;
   case 2: damage = 15 + you[0].xl; your_to_hit += random2(20); break;
   case 3: damage = 12 + you[0].strength; your_to_hit += random2(9); break;
   case 4: damage = 10; special_brand = 2; your_to_hit += random2(10); break;
   case 5: damage = 18 + you[0].strength; your_to_hit += random2(10); break;
   case 6: damage = 5; special_brand = 8; your_to_hit += random2(10); break;
  }
 } else
   if (you[0].species == 16)
   {
    damage = 4 + you[0].xl;
   }
}


if (you[0].equip [0] != -1)
{
 if (you[0].inv_class [you[0].equip [0]] == 0)
 {
   damage = property(you[0].inv_class [you[0].equip [0]], you[0].inv_type [you[0].equip [0]], 0);
 }
 if (you[0].inv_class [you[0].equip [0]] == 11)
        {
   damage = 5;
        }
}

if (you[0].equip [0] != -1)
{
        if (you[0].inv_class [you[0].equip [0]] == 0 | you[0].inv_class [you[0].equip [0]] == 11)
        {
                int weapon_speed2 = 0;

                if (you[0].inv_class [you[0].equip [0]] == 11)
                        weapon_speed2 = 12; else weapon_speed2 = property(you[0].inv_class [you[0].equip [0]], you[0].inv_type [you[0].equip [0]], 2);

                weapon_speed2 -= you[0].skills [weapon_skill(you[0].inv_class [you[0].equip [0]], you[0].inv_type [you[0].equip [0]])] / 2;

                if (you[0].inv_class [you[0].equip [0]] == 11)
                {
                        if (weapon_speed2 < property(0, 18, 2) / 2) weapon_speed2 = property(0, 18, 2) / 2;
                }
                        else
                             if (weapon_speed2 < property(you[0].inv_class [you[0].equip [0]], you[0].inv_type [you[0].equip [0]], 2) / 2) weapon_speed2 = property(you[0].inv_class [you[0].equip [0]], you[0].inv_type [you[0].equip [0]], 2) / 2;
                you[0].time_taken *= weapon_speed2;
                you[0].time_taken /= 10;
#ifdef DEBUG
                itoa(you[0].time_taken, st_prn, 10);
                strcpy(info, st_prn);
                mpr(info);
#endif
        }
}




if (menv [monster_attacked].m_speed_inc <= 40 | menv [monster_attacked].m_beh == 0)
{
  strcpy(info, monam(menv[monster_attacked].m_sec,menv[monster_attacked].m_class, menv [monster_attacked].m_ench [2], 0));
  strcat(info, " fails to defend itself.");
  mpr(info);
  stabbed = 1;
  exercise(16, 1 + random2(2) + random2(2) + random2(2) + random2(2));
  naughty(8, 4);
} else alert();


if ((your_to_hit >= menv [monster_attacked].m_ev | random2(15) == 0) | ((menv [monster_attacked].m_speed_inc <= 60 | menv [monster_attacked].m_beh == 0) && random2 (10 + you[0].skills [16]) != 0))
{
        hit = 1;
        damage_done = (int) random2(damage);


if (you[0].equip [0] != -1 && you[0].inv_class [you[0].equip [0]] == 0 | you[0].inv_class [you[0].equip [0]] == 11)
{
 damage_done *= 20 + (random2(you[0].skills [weapon_skill(you[0].inv_class [you[0].equip [0]], you[0].inv_type [you[0].equip [0]])] + 1));
 damage_done /= 20;
}

damage_done *= 20 + (random2(you[0].skills [0] + 1));
damage_done /= 20;

        damage_done += random2(you[0].strength) / 4;
        if (you[0].might > 1) damage_done += random2(10) + 1;
        if (you[0].berserker > 1) damage_done += random2(10) + 1; /* remember these two are cumulative */
 if (you[0].hung_state == 1) damage_done -= random2(5);
        if (you[0].equip [0] != -1 && you[0].inv_class [you[0].equip [0]] == 0)
        {
                int hoggl = you[0].inv_plus2 [you[0].equip [0]] - 50;

                if (hoggl >= 0)
                {
                 damage_done += random2(hoggl + 1);
                } else damage_done += hoggl;

                if (you[0].inv_dam [you[0].equip [0]] / 30 == 5 && you[0].species >= 7 && you[0].species <= 8)
                {
                 damage_done += random2(3);
                }

                if (you[0].inv_dam [you[0].equip [0]] / 30 == 3 && you[0].species == 10)
                {
                 damage_done += random2(2);
                }

                if (you[0].inv_ident [you[0].equip [0]] < 3 && random2(100) < you[0].skills [weapon_skill(you[0].inv_class [you[0].equip [0]], you[0].inv_type [you[0].equip [0]])])
                {
                 strcpy(info, "You are wielding ");
                 in_name(you[0].equip [0], 3, str_pass);
                 strcat(info, str_pass);
                 strcat(info, ".");
                 mpr(info);
                 you[0].inv_ident [you[0].equip [0]] = 3;
                }

        }
  else (damage_done -= random2(5));

        if (stabbed == 1)
        {
         damage_done *= 12 + you[0].skills [16];
         damage_done /= 12;
         if (you[0].equip [0] != -1)
         {
            if (weapon_skill(you[0].inv_class [you[0].equip [0]], you[0].inv_type [you[0].equip [0]]) == 1 | weapon_skill(you[0].inv_class [you[0].equip [0]], you[0].inv_type [you[0].equip [0]]) == 2)
            {
                 damage_done *= 10 + you[0].skills [16] / weapon_skill(you[0].inv_class [you[0].equip [0]], you[0].inv_type [you[0].equip [0]]);
                 damage_done /= 10;
                 if (you[0].inv_type [you[0].equip [0]] == 3) ;
                    {
                         damage_done *= 12 + you[0].skills [16];
                         damage_done /= 12;
                    }
            }
         }
        }

        damage_done -= random2 (menv [monster_attacked].m_AC + 1);

        if (damage_done < 0) damage_done = 0;
        menv [monster_attacked].m_hp -= damage_done;

        if (damage_done >= 1)
        {
         if (you[0].equip [0] != -1 && weapon_skill(you[0].inv_class [you[0].equip [0]], you[0].inv_type [you[0].equip [0]]) != 0)
                exercise(weapon_skill(you[0].inv_class [you[0].equip [0]], you[0].inv_type [you[0].equip [0]]), 1);
         if (random2(3) == 0) exercise(0, 1);
        }

        if (menv [monster_attacked].m_hp <= 0)
        {
                /* thing_thrown = 1; */
#ifdef DEBUG
                itoa(damage_done, st_prn, 10);
                strcpy(info, "Hit for ");
                strcat(info, st_prn); /* note: doesn't take account of special weapons etc */
                strcat(info, ".");
                mpr(info);
#endif
                if (you[0].equip [0] != -1 && you[0].inv_class [you[0].equip [0]] == 0 && you[0].inv_dam [you[0].equip [0]] < 180 && you[0].inv_dam [you[0].equip [0]] % 30 == 13)
                {
                   if (mons_holiness(menv [monster_attacked].m_class) <= 0 && damage_done >= 1 && random2(5) != 0 && you[0].hp < you[0].hp_max)
                   {
                           you[0].hp += random2(damage_done) + 1; // more than if not killed
                           you[0].hp_ch = 1;
                           if (you[0].hp > you[0].hp_max) you[0].hp = you[0].hp_max;
                           if (you[0].hunger <= 11000 && you[0].is_undead < 2) you[0].hunger += random2(30) + random2(30) + 30;
                           mpr("You feel better.");
                           naughty(1, 2);
                   }

                }
                monster_die(monster_attacked, 1, 0);
                if (menv [monster_attacked].m_class == 32)
                {
                        strcpy(info, "You hit the giant spore.");
                        mpr(info);
                }
                return;
        }

        if (damage_done < 1 && menv [monster_attacked].m_ench [2] != 6)
        {
         hit = 1;
         strcpy(info, "You hit ");
         strcat(info, monam(menv[monster_attacked].m_sec,menv[monster_attacked].m_class, menv [monster_attacked].m_ench [2], 1));
         strcat(info, ", but do no damage.");
         mpr(info);
        }

} else
        {
         hit = 0;
         if (your_to_hit + heavy_armour >= menv [monster_attacked].m_ev)
         {
          strcpy(info, "Your armour prevents you from hitting ");
         } else strcpy(info, "You miss ");
         strcat(info, monam(menv[monster_attacked].m_sec,menv[monster_attacked].m_class, menv [monster_attacked].m_ench [2], 1));
         strcat(info, ".");
         mpr(info);
        }

if (hit == 1 && damage_done >= 1 | (hit == 1 && damage_done < 1 && menv [monster_attacked].m_ench [2] == 6))
        {
        strcpy(info, "You hit ");
        strcat(info, monam(menv[monster_attacked].m_sec,menv[monster_attacked].m_class, menv [monster_attacked].m_ench [2], 1));
#ifdef DEBUG
itoa(damage_done, st_prn, 10);
strcat(info, " for ");
strcat(info, st_prn); /* note: doesn't take account of special weapons etc */
#endif
        if (damage_done < 5) strcat(info, ".");
 if (damage_done >= 5 && damage_done < 12) strcat(info, "!");
 if (damage_done >= 12 && damage_done < 21) strcat(info, "!!");
 if (damage_done >= 21) strcat(info, "!!!");
 mpr(info);

 if (mons_holiness(menv [monster_attacked].m_class) == -1)
          done_good(4, 1);

 if (you[0].special_wield == 6)
 {
  torment();
  naughty(2, 5);
 }
 if (you[0].special_wield == 7 | you[0].special_wield == 3) naughty(1, 3);
 }

if (menv [monster_attacked].m_class == 35 | menv [monster_attacked].m_class == 275 | menv [monster_attacked].m_class == 278 | menv [monster_attacked].m_class == 279) weapon_acid(5);

int specdam = 0;
/* remember, damage_done is still useful! */

int weap_dam = 0;


if (hit == 1 && special_brand != 0 && you[0].equip [0] == -1)
{
 weap_dam = special_brand; /* No electrocution - this references inv_plus2 */
 goto dam_thing;
}

if (you[0].equip [0] != -1 && you[0].inv_class [you[0].equip [0]] == 0 && hit == 1)
{
 weap_dam = you[0].inv_dam [you[0].equip [0]] % 30;
 if (you[0].inv_dam [you[0].equip [0]] >= 180) weap_dam = 0;
 switch(you[0].inv_dam [you[0].equip [0]])
 {
  case 188: /* sword of cerebov */
  weap_dam = 1;
  break;

  case 193: /* staff of Olgreb */
  weap_dam = 6;
  break;

  case 194: /* vampire's tooth */
  weap_dam = 13;
  break;

 }
}

if (you[0].equip [0] != -1 && you[0].inv_class [you[0].equip [0]] == 0 && hit == 1)// && you[0].inv_dam [you[0].equip [0]] <= 180)
{
 dam_thing : switch(weap_dam)
  {
   case 0: /* nothing */
   break;

   case 1: /* flaming */
   specdam = 0;
   if (mons_res_fire(menv [monster_attacked].m_class) == 0)
   {
    specdam = random2(damage_done) / 2 + 1;
    if (menv [monster_attacked].m_inv [2] != 501 && mitm.idam [menv [monster_attacked].m_inv [2]] % 30 == 2)
      specdam = (random2(damage_done) / 2 + 1) / 3;
   }
   if (mons_res_fire(menv [monster_attacked].m_class) == -1 && (menv [monster_attacked].m_inv [2] == 501 | mitm.idam [menv [monster_attacked].m_inv [2]] % 30 != 2))
   {
    specdam = random2(damage_done) + 1;
   }
    if (specdam != 0)
    {
        strcpy(info, "You burn ");
        strcat(info, monam(menv[monster_attacked].m_sec,menv[monster_attacked].m_class, menv [monster_attacked].m_ench [2], 1));
        if (specdam < 3) strcat(info, ".");
        if (specdam >= 3 && specdam < 7) strcat(info, "!");
        if (specdam >= 7) strcat(info, "!!");
        mpr(info);
    }
   break;

   case 2: /* freezing */
   specdam = 0;
   if (mons_res_cold(menv [monster_attacked].m_class) == 0)
   {
    specdam = random2(damage_done) / 2 + 1;
    if (menv [monster_attacked].m_inv [2] != 501 && mitm.idam [menv [monster_attacked].m_inv [2]] % 30 == 3)
      specdam = (random2(damage_done) / 2 + 1) / 3;
   }
   if (mons_res_cold(menv [monster_attacked].m_class) == -1 && (menv [monster_attacked].m_inv [2] == 501 | mitm.idam [menv [monster_attacked].m_inv [2]] % 30 != 3))
   {
    specdam = random2(damage_done) + 1;
   }
    if (specdam != 0)
    {
        strcpy(info, "You freeze ");
        strcat(info, monam(menv[monster_attacked].m_sec,menv[monster_attacked].m_class, menv [monster_attacked].m_ench [2], 1));
        if (specdam < 3) strcat(info, ".");
        if (specdam >= 3) strcat(info, "!");
        if (specdam >= 7) strcat(info, "!!");
        mpr(info);
    }
   break;

   case 3: /* holy wrath */
   specdam = 0;
   switch(mons_holiness(menv [monster_attacked].m_class))
   {
/*    case -1:
      damage_done -= 5 + random2(5);
    break;*/

    case 1:
      specdam += random2(damage_done) + 1;
    break;

    case 2:
      specdam += (random2(damage_done * 15) / 10) + 1; // does * 1.5 do anything?
    break;
   }
   break;


   case 4: /* electrocution */
   specdam = 0;
   if (mons_flies(menv [monster_attacked].m_class) == 2) break;
   if (mons_res_elec(menv [monster_attacked].m_class) != 0) break;
   if (you[0].inv_plus2 [you[0].equip [0]] <= 50 | you[0].inv_plus2 [you[0].equip [0]] > 130 && you[0].inv_plus2 [you[0].equip [0]] <= 150) break;
   if (random2(3) == 0)
   {
    strcpy(info, "There is a sudden explosion of sparks!");
    mpr(info);
    specdam += 10 + random2(15);
    you[0].inv_plus2 [you[0].equip [0]] --;
   }
   break;

   case 5: /* orc slaying */
   if (mons_charclass(menv [monster_attacked].m_class) == 14)
   {
     menv [monster_attacked].m_hp -= 1 + random2(damage_done);
   }
   break;

   case 6: /* venom */
   if (random2(4) != 0) poison_monster(monster_attacked, 0);
   break;

   case 8:
   if (mons_holiness(menv [monster_attacked].m_class) > 0 | random2(3) == 0) break;
   strcpy(info, "You drain ");
   strcat(info, monam(menv[monster_attacked].m_sec,menv[monster_attacked].m_class, menv [monster_attacked].m_ench [2], 1));
   strcat(info, "!");
                 mpr(info);
   if (random2(5) == 0) menv [monster_attacked].m_HD --;
   menv [monster_attacked].m_hp_max -= 2 + random2(3);
   menv [monster_attacked].m_hp -= 2 + random2(3);
   if (menv [monster_attacked].m_hp >= menv [monster_attacked].m_hp_max) menv [monster_attacked].m_hp = menv [monster_attacked].m_hp_max;
   if (menv [monster_attacked].m_HD <= 0) menv [monster_attacked].m_hp = 0;
   specdam = random2(damage_done) / 2 + 1;
   naughty(1, 2);
   break;

   /* 9 = speed - done before */

   case 10: /* slicing etc */
   specdam = random2(damage_done) / 2 + 1;
   if (menv [monster_attacked].m_class == 56 && (damage_type(0, you[0].inv_type [you[0].equip [0]]) == 1 | damage_type(0, you[0].inv_type [you[0].equip [0]]) == 3))
   {
     strcpy(info, "You slice the worm tail!");
     mpr(info);
     menv [monster_attacked].m_hp -= 20 + random2(20);
   }
   break;

   case 13: /* vampiric */
   specdam = 0; /* note does no extra damage */
   if (mons_holiness(menv [monster_attacked].m_class) > 0) break; /* should really also prevent draining eg golems */
   if (damage_done < 1) break;
   if (random2(5) == 0 | you[0].hp == you[0].hp_max) break;
   if (you[0].equip [0] != -1 && you[0].inv_dam [you[0].equip [0]] == 194) you[0].hp += damage_done;
        else you[0].hp += random2(damage_done) + 1; /* thus is probably more valuable on larger weapons? */
   you[0].hp_ch = 1;
   if (you[0].hp > you[0].hp_max) you[0].hp = you[0].hp_max;
   if (you[0].hunger <= 11000 && you[0].is_undead < 2) you[0].hunger += random2(30) + random2(30);
   strcpy(info, "You feel better.");
   mpr(info);
   naughty(1, 2);
   break;

   case 14: /* mace of disruption */
   specdam = 0;
   if (mons_holiness(menv [monster_attacked].m_class) == 1 && random2(3) != 0)
   {
    strcpy(info, monam(menv[monster_attacked].m_sec,menv[monster_attacked].m_class, menv [monster_attacked].m_ench [2], 0));
    strcat(info, " shudders.");
    mpr(info);
    specdam += random2(damage_done + 1);
    specdam += random2(damage_done + 1);
    specdam += random2(damage_done + 1);
   }
   break;

   case 15: /* pain */
   specdam = 0;
   if (mons_holiness(menv [monster_attacked].m_class) <= 0 && random2(8) <= you[0].skills [29])
   {
    strcpy(info, monam(menv[monster_attacked].m_sec,menv[monster_attacked].m_class, menv [monster_attacked].m_ench [2], 0));
    strcat(info, " convulses in agony.");
    mpr(info);
    specdam += random2((you[0].skills [29] * 2) + 1);
   }
   naughty(1, 4);
   break;

   case 16: /* distortion */
   if (random2(3) == 0)
   {
    strcpy(info, "Space bends around ");
    strcat(info, monam(menv[monster_attacked].m_sec,menv[monster_attacked].m_class, menv [monster_attacked].m_ench [2], 1));
    strcat(info, ".");
    mpr(info);
    specdam += random2(5) + random2(3) + 1;
    break;
   }
   if (random2(3) == 0)
   {
    strcpy(info, "Space warps horribly around ");
    strcat(info, monam(menv[monster_attacked].m_sec,menv[monster_attacked].m_class, menv [monster_attacked].m_ench [2], 1));
    strcat(info, "!");
    mpr(info);
    specdam += random2(12) + random2(13) + 3;
    break;
   }
   if (random2(3) == 0)
   {
    monster_blink(monster_attacked);
    break;
   }
   if (random2(2) == 0)
   {
    monster_teleport(monster_attacked, 0);
    break;
   }
   if (random2(2) == 0)
   {
    monster_teleport(monster_attacked, 1);
    break;
   }
   if (random2(2) == 0)
   {
    monster_die(monster_attacked, 6, 0);
    break;
   }
   break;

/* 11 & 12 are bow/xbow things */

  } /* end switch */


}

if (mons_holiness(menv [monster_attacked].m_class) < 0) naughty(4, menv [monster_attacked].m_HD);

if (menv [monster_attacked].m_class == 106) // hydra
{
 if (you[0].equip [0] != -1 && (damage_type(you[0].inv_class [you[0].equip [0]], you[0].inv_type [you[0].equip [0]]) == 1 | damage_type(you[0].inv_class [you[0].equip [0]], you[0].inv_type [you[0].equip [0]]) == 3))
 {
  if ((random2(2) != 0 | damage_done < 4) && (you[0].inv_class [you[0].equip [0]] != 0 | you[0].inv_dam [you[0].equip [0]] != 10)) goto mons_dies;
  strcpy(info, "You ");
  switch(random2(4))
  {
   case 0: strcat(info, "slice"); break;
   case 1: strcat(info, "lop"); break;
   case 2: strcat(info, "chop"); break;
   case 3: strcat(info, "hack"); break;
  }
  strcat(info, " one of ");
  strcat(info, monam(menv[monster_attacked].m_sec,menv[monster_attacked].m_class, menv [monster_attacked].m_ench [2], 1));
  strcat(info, "'s heads off.");
  mpr(info);
  menv [monster_attacked].m_sec --;
  if (menv [monster_attacked].m_sec <= 0)
  {
    menv [monster_attacked].m_hp = -1;
    goto mons_dies;
  }
  if (you[0].inv_class [you[0].equip [0]] == 0 && you[0].inv_dam [you[0].equip [0]] % 30 == 1) goto mons_dies; // cauterised
  if (menv [monster_attacked].m_sec >= 18) goto mons_dies;
  strcpy(info, "It grows two more!");
  mpr(info);
  menv [monster_attacked].m_sec += 2;
 }
}
/* remember, the hydra function sometimes skips straight to mons_dies */

menv [monster_attacked].m_hp -= specdam;

mons_dies :
if (menv [monster_attacked].m_hp <= 0)
{
                /* thing_thrown = 1; */
                /* vampiric weapon: */
                monster_die(monster_attacked, 1, 0);
                return;
}


if (menv [monster_attacked].m_hp <= 0)
{
                /* thing_thrown = 1; */
                monster_die(monster_attacked, 1, 0);
                return;
}


if (hit != 0)
{
 print_wounds(monster_attacked);
}

if (damage_done >= 1 && you[0].equip [0] != -1 && you[0].inv_class [you[0].equip [0]] == 0 && you[0].inv_type [you[0].equip [0]] > 12 && you[0].inv_type [you[0].equip [0]] < 17) //       18??
{
        if (random2 (5) == 0)
        {
                item_name(you[0].inv_plus2 [you[0].equip [0]], you[0].inv_class [you[0].equip [0]], you[0].inv_type [you[0].equip [0]], you[0].inv_dam [you[0].equip [0]], you[0].inv_plus [you[0].equip [0]], you[0].inv_quant [you[0].equip [0]], you[0].inv_ident [you[0].equip [0]], 4, str_pass);
                strcpy(info, str_pass);
                strcat(info, " breaks!");
                mpr(info);
                unwield_item(you[0].equip [0]);
                you[0].inv_quant [you[0].equip [0]] --;
                you[0].equip [0] = -1;
        }
}

if (you[0].invis != 0 && menv [monster_attacked].m_beh == 0) menv [monster_attacked].m_beh = 1;

if (you[0].invis != 0 && random2(2) == 0)
{
 menv [monster_attacked].m_targ_1_x = you[0].x_pos;
 menv [monster_attacked].m_targ_1_y = you[0].y_pos;
 menv [monster_attacked].m_beh = 1;
}




if (menv [monster_attacked].m_beh == 3) menv [monster_attacked].m_beh = 1;

//if (getch() == 0) getch();

return;
}




void monster_attack(int monster_attacking)
{
int damage_taken = 0;
char blocked = 0;
char hit = 0;
int mmov_x = 0;
char str_pass [80];

int specdam = 0;

char heads = 0;
if (menv [monster_attacking].m_class == 106) heads = menv [monster_attacking].m_sec;

int hand_used = 0;

if (menv [monster_attacking].m_class == 32)
{
                menv [monster_attacking].m_hp = -1;
                return;
}

{
 you[0].pet_target = monster_attacking;
}

if (menv [monster_attacking].m_beh == 7) return;

if (menv [monster_attacking].m_class >= MLAVA0 && menv [monster_attacking].m_sec == 1) return;

if (wearing_amulet(38) == 1)
{
 if (menv [monster_attacking].m_ench [1] >= 20 && menv [monster_attacking].m_ench [1] <= 25)
  if (random2(5) <= 1)
  {
   strcpy(info, monam(menv[monster_attacking].m_sec,menv[monster_attacking].m_class, menv [monster_attacking].m_ench [2], 0));
   strcat(info, " tries to attack you, but flinches away.");
   mpr(info);
   return;
  }
}

char runthru = 0;

for (runthru = 0; runthru < 4; runthru ++)
{

blocked = 0;

if (menv [monster_attacking].m_class == 106)
{
 if (heads <= 0) break;
 runthru = 0;
 heads --;
}

char mdam = mondamage(menv [monster_attacking].m_class, runthru);

if (menv [monster_attacking].m_class == 25 | menv [monster_attacking].m_class == 51 | menv [monster_attacking].m_class == 107 | menv [monster_attacking].m_class == 108 | menv [monster_attacking].m_class == 367)
{
        mdam = mondamage(menv [monster_attacking].m_sec, runthru);
        if (mdam > 1 && mdam < 6)
        {
                mdam --;
        }

        if (mdam > 5 && mdam < 14)
        {
                mdam -= 2;
        }

        if (mdam > 13 && mdam < 18)
        {
                mdam -= 3;
        }

        if (mdam > 17)
        {
                mdam -= 4;
        }
}


if (mdam == 0) break;

if (menv [monster_attacking].m_class == 30 && runthru == 1 && menv [monster_attacking].m_inv [1] != 501)
{
        hand_used = 1;
}

damage_taken = 0;


int mons_to_hit = 20 + menv [monster_attacking].m_HD; // * menv [monster_attacking].m_HD; // * 3  //you[0].strength + (0.5 * you[0].you[0].max_dex) + (you[0].f_abil);

if (menv [monster_attacking].m_inv [hand_used] != 501)
{

item_name(mitm.iplus2 [menv [monster_attacking].m_inv [hand_used]], mitm.iclass [menv [monster_attacking].m_inv [hand_used]], mitm.itype [menv [monster_attacking].m_inv [hand_used]], mitm.idam [menv [monster_attacking].m_inv [hand_used]], mitm.iplus [menv [monster_attacking].m_inv [hand_used]], mitm.iquant [menv [monster_attacking].m_inv [hand_used]], mitm.iid [menv [monster_attacking].m_inv [hand_used]], 0, str_pass);

  if (mitm.iclass [menv [monster_attacking].m_inv [hand_used]] != 0 | strstr(str_pass, "questionable item") != NULL)
  {
   destroy_item(menv [monster_attacking].m_inv [hand_used]);
   menv [monster_attacking].m_inv [hand_used] = 501;
  }
}


if (menv [monster_attacking].m_inv [hand_used] != 501)
{
        if (mitm.iplus [menv [monster_attacking].m_inv [hand_used]] - 50 > 130)
        {
                mons_to_hit += mitm.iplus [menv [monster_attacking].m_inv [hand_used]] - 50 - 100; // + 1; //random2(mitm.iplus [menv [monster_attacking].m_inv [hand_used]] - 50 - 100 + 1);
        } else
              {
                mons_to_hit += mitm.iplus [menv [monster_attacking].m_inv [hand_used]] - 50; // + 1; //random2(mitm.iplus [menv [monster_attacking].m_inv [hand_used]] - 50 + 1); /// 10;
              }

        mons_to_hit += property(0, mitm.itype [menv [monster_attacking].m_inv [0]], 1);

   if (menv [monster_attacking].m_speed_inc >= 50) menv [monster_attacking].m_speed_inc -= (property(mitm.iclass [menv [monster_attacking].m_inv [0]], mitm.itype [menv [monster_attacking].m_inv [0]], 2) - 10); // / 10);
}

mons_to_hit = random2(mons_to_hit);

        if (menv [monster_attacking].m_class == 10 | menv [monster_attacking].m_class == 22)
        {
                /* maybe this will work better: */
  if (menv [monster_attacking].m_speed_inc >= 20)
                                                menv [monster_attacking].m_speed_inc -= 10; //--
        }

if (player_shield_class() > 0 && you[0].paralysis == 0 && you[0].conf == 0 && random2(menv [monster_attacking].m_HD + 10) <= random2(player_shield_class()))
{
 strcpy(info, "You block ");
        int mmov_x = menv [monster_attacking].m_inv [hand_used];

        if (menv [monster_attacking].m_class == 144)
        {
         char str_pass [80];
         item_name(mitm.iplus2 [mmov_x], mitm.iclass [mmov_x], mitm.itype [mmov_x], mitm.idam [mmov_x], mitm.iplus [mmov_x], mitm.iquant [mmov_x], mitm.iid [mmov_x], 0, str_pass);
         strcpy(info, str_pass);
        } else  strcat(info, monam(menv[monster_attacking].m_sec,menv[monster_attacking].m_class, menv [monster_attacking].m_ench [2], 1));  //gmon_name [menv [monster_attacking].m_class]);
 strcat(info, "'s attack.");
 mpr(info);
 blocked = 1;
 hit = 0;
 if (you[0].equip [5] != -1 && random2(4) == 0) exercise(17, 1);
 /* continue; */
} else
if ((you[0].equip [6] == -1 | you[0].inv_type [you[0].equip [6]] < 2 | (you[0].inv_type [you[0].equip [6]] >= 22 && you[0].inv_type [you[0].equip [6]] <= 25) | you[0].inv_dam [you[0].equip [6]] / 30 == 4) && random2(3) == 0)// && move_x != 0 | move_y != 0)
 exercise(14, 1);

if (blocked == 0 && mons_to_hit >= player_evasion() + random2(you[0].dex) / 3 - 2)// | random2(6) == 0) // menv [monster_attacked].m_ev)
{
hit = 1;


if (menv [monster_attacking].m_inv [hand_used] != 501 && mitm.iclass [menv [monster_attacking].m_inv [hand_used]] == 0 && mitm.itype [menv [monster_attacking].m_inv [hand_used]] < 13 | mitm.itype [menv [monster_attacking].m_inv [hand_used]] > 15)
{

  if (mitm.idam [menv [monster_attacking].m_inv [hand_used]] / 30 == 3 && mons_charclass(menv [monster_attacking].m_class) == 14)
  {
   damage_taken += random2(2);
  }

damage_taken = random2(property(mitm.iclass [menv [monster_attacking].m_inv [0]], mitm.itype [menv [monster_attacking].m_inv [0]], 0));
        if (mitm.iplus2 [menv [monster_attacking].m_inv [hand_used]] > 130)
        {
                damage_taken += random2(mitm.iplus2 [menv [monster_attacking].m_inv [hand_used]] - 150 + 1);
        } else
              {
                damage_taken += random2(mitm.iplus2 [menv [monster_attacking].m_inv [hand_used]] - 50 + 1);// % 10;
              }
damage_taken -= random2(3) + 1;//1;
}

damage_taken += random2(mdam) + 1;

if (player_AC() > 0) damage_taken -= random2(player_AC() + 1);

if (damage_taken < 1) damage_taken = 0;

} /* end of if mons_to_hit >= player_evasion() */
 else
 if (blocked == 0)
 {
        hit = 0;

        int mmov_x = menv [monster_attacking].m_inv [hand_used];
        char str_pass [80];

        if (menv [monster_attacking].m_class == 144)
        {
         item_name(mitm.iplus2 [mmov_x], mitm.iclass [mmov_x], mitm.itype [mmov_x], mitm.idam [mmov_x], mitm.iplus [mmov_x], mitm.iquant [mmov_x], mitm.iid [mmov_x], 0, str_pass);
         strcpy(info, str_pass);
        } else strcpy(info, monam(menv[monster_attacking].m_sec,menv[monster_attacking].m_class, menv [monster_attacking].m_ench [2], 0));  //gmon_name [menv [monster_attacking].m_class]);
        strcat(info, " misses you.");
        mpr(info);
 }


if (damage_taken < 1 && hit == 1 && blocked == 0)
{
        int mmov_x = menv [monster_attacking].m_inv [hand_used];
        char str_pass [80];

        if (menv [monster_attacking].m_class == 144)
        {
         item_name(mitm.iplus2 [mmov_x], mitm.iclass [mmov_x], mitm.itype [mmov_x], mitm.idam [mmov_x], mitm.iplus [mmov_x], mitm.iquant [mmov_x], mitm.iid [mmov_x], 0, str_pass);
         strcpy(info, str_pass);
        } else strcpy(info, monam(menv[monster_attacking].m_sec,menv[monster_attacking].m_class, menv [monster_attacking].m_ench [2], 0));  //gmon_name [menv [monster_attacking].m_class]);
        strcat(info, " hits you but doesn't do any damage.");
        mpr(info);
}



if ((int) damage_taken >= 1)
{
 hit = 1;
        mmov_x = menv [monster_attacking].m_inv [hand_used];

        mmov_x = menv [monster_attacking].m_inv [hand_used];

        if (menv [monster_attacking].m_class == 144)
        {
         item_name(mitm.iplus2 [mmov_x], mitm.iclass [mmov_x], mitm.itype [mmov_x], mitm.idam [mmov_x], mitm.iplus [mmov_x], mitm.iquant [mmov_x], mitm.iid [mmov_x], 0, str_pass);
         strcpy(info, str_pass);
        } else strcpy(info, monam(menv[monster_attacking].m_sec,menv[monster_attacking].m_class, menv [monster_attacking].m_ench [2], 0));
        strcat(info, " hits you");

#ifdef DEBUG
itoa(damage_taken, st_prn, 10);
strcat(info, " for ");
strcat(info, st_prn); // note: doesn't take account of special weapons etc
#endif


        if (menv [monster_attacking].m_class != 144 && menv [monster_attacking].m_inv [hand_used] != 501 && mitm.iclass [menv [monster_attacking].m_inv [hand_used]] == 0 && mitm.itype [menv [monster_attacking].m_inv [hand_used]] < 13 | mitm.itype [menv [monster_attacking].m_inv [hand_used]] > 16)
        {
        strcat(info, " with ");

        item_name(mitm.iplus2 [mmov_x], mitm.iclass [mmov_x], mitm.itype [mmov_x], mitm.idam [mmov_x], mitm.iplus [mmov_x], mitm.iquant [mmov_x], mitm.iid [mmov_x], 3, str_pass); // was 7
        strcat(info, str_pass);
 strcat(info, "!");

 } else
   {
    strcat(info, "!");
   }

mpr(info);

if (hit == 1)
{
 if (you[0].equip [6] != -1)
  if (you[0].inv_type [you[0].equip [6]] > 1 && !(you[0].inv_type [you[0].equip [6]] >= 22 && you[0].inv_type [you[0].equip [6]] <= 25) && random2(1000) <= mass(2, you[0].inv_type [you[0].equip [6]]) && random2(2) == 0)
   exercise(13, 1); /* practises armour */
}


/* special attacks: */

        char brek = 0;



        switch(menv [monster_attacking].m_class)
        {
        case 0: /* giant ant */
        case 62: /* wolf spider */
        case 139: /* redback */
        if (player_res_poison() != 0) break;
        if ((damage_taken >= 4 && random2(4) == 0) | random2(20) == 0)
        {
 strcpy(info, monam(menv[monster_attacking].m_sec,menv[monster_attacking].m_class, menv [monster_attacking].m_ench [2], 0));
        strcat(info, " stings you!");
        mpr(info);
        you[0].poison++;
        if (menv [monster_attacking].m_class == 139) you[0].poison += 3 + random2(5) + random2(5);
        }
        break;

        case 10: /* killer bee */
        case 138: /* bumblebee */
        if (player_res_poison() == 0 && ((damage_taken >= 3 && random2(3) == 0) | random2(20) == 0))
        {
 strcpy(info, monam(menv[monster_attacking].m_sec,menv[monster_attacking].m_class, menv [monster_attacking].m_ench [2], 0));
        strcat(info, " stings you!");
        mpr(info);
        you[0].poison += 1;
        if (menv [monster_attacking].m_class == 138) you[0].poison += random2(3);
        }
        break;

 case 81: /* rotting devil */
 case 13: /* necrophage */
 case 156: /* ghoul */
 case 277: // death ooze
        if (you[0].is_undead != 0) break;
                if ((damage_taken >= 3 && random2(3) == 0) | random2(20) == 0)
                {
                        strcpy(info, "You feel your flesh start to rot away!");
                        mpr(info);
                        you[0].rotting += random2(3) + 1;
                }
  if (random2(4) == 0)
  {
   strcpy(info, "You feel ill.");
   mpr(info);
   if (you[0].disease > 100) you[0].disease = 210; else you[0].disease += 50 + random2(100);
  }
                break;



 case 21: /* fire vortex */
 menv [monster_attacking].m_hp = -10;
 case 124: /* fire elemental */
 case 233: /* balrug */
        strcpy(info, "You are engulfed in flame!");
        mpr(info);
   if (player_res_fire() > 100)
   {
    damage_taken += (15 + random2(15)) / 2 + ((player_res_fire() - 100) * (player_res_fire() - 100));
   }
   if (player_res_fire() <= 100)
   {
        damage_taken += 15 + random2(15);
   }
   if (player_res_fire() < 100)
   {
        damage_taken += 8 + random2(8);
   }
 scrolls_burn(1, 6);
 break;


        case 44: /* scorpion */
 case 100: /* giant mite */
/* case 224: less demon */
        if (player_res_poison() == 0 && ((damage_taken >= 3 && random2(4) == 0) | random2(15) == 0))
        {
 strcpy(info, monam(menv[monster_attacking].m_sec,menv[monster_attacking].m_class, menv [monster_attacking].m_ench [2], 0));
        strcat(info, " poisons you!");
        mpr(info);
        you[0].poison++;
        }
        break;



        case 42: /* Queen bee */
if (player_res_poison() == 0)
{
 strcpy(info, monam(menv[monster_attacking].m_sec,menv[monster_attacking].m_class, menv [monster_attacking].m_ench [2], 0));
        strcat(info, " stings you!");
        mpr(info);
        you[0].poison += 2;
}
        break;


        case 18: // snake
        case 128: // br snake
        case 154: // black snake
        case 166: // yellow snake
        if (player_res_poison() == 0 && ((damage_taken >= 3 && random2(4) == 0) | random2(20) == 0))
        {
 strcpy(info, monam(menv[monster_attacking].m_sec,menv[monster_attacking].m_class, menv [monster_attacking].m_ench [2], 0));
        strcat(info, " poisons you!");
        mpr(info);
        you[0].poison++;
        }
        break;

 case 165: // shadow dragon
 case 367: /* spectral thing */
 if (random2(2) == 0) break;
 case 60: // Wight. Is less likely because wights do less damage
 case 48: // wraith
 case 84: // shadow devil
 case 127: // shad fiend
 case 130: // spectre
 case 153: // orange rat
 case 240: // shadow wraith
//        if (runthru != 0) break; // shadow fiends have multiple attacks
        if (((damage_taken >= 6 && random2(2) == 0) | random2(30) == 0) && player_prot_life() == 0)
        {
/*        if (you[0].duration [3] != 0 && you[0].religion == 1 && random2(150) < you[0].piety)
        {
         strcpy(info, "Zin protects your life force!");
         mpr(info);
         brek = 0;
         break;
        }*/
        strcpy(info, "You feel drained...");
        mpr(info);
        brek = 1;
        }
        break;

        case 24: // yellow wasp
        if (player_res_poison() == 0 && ((damage_taken >= 3 && random2(3) == 0) | random2(20) == 0))
        {
        if (you[0].paralysis > 0)
        {
                strcpy(info, "You still can't move!");
        } else strcpy(info, "You suddenly lose the ability to move!");
        mpr(info);
        you[0].paralysis += random2(3) + 1;
        }
        break;

        case 143: // spiny worm
        if (player_res_poison() == 0)
        {
        strcpy(info, monam(menv[monster_attacking].m_sec,menv[monster_attacking].m_class, menv [monster_attacking].m_ench [2], 0));
        strcat(info, " stings you!");
        mpr(info);
        you[0].poison += 2 + random2(4);
        } // no break is intentional
        case 275: // ooze
        case 278: // acid blob
        case 279: // royal jelly
        case 35: // Jelly
        strcpy(info, "You are splashed with acid!");
        mpr(info);
        splash_with_acid(3);
        break;

 case 86: // ice devil
 case 34: // Ice beast
 case 77: // blue wraith
 case 126: // I Fiend
 case 220: // lesser demon
 case 343: // Antaeus
 case 276: // azure jelly
   if (player_res_cold() <= 100)
   {
        damage_taken += 5 + random2(10);
        strcpy(info, monam(menv[monster_attacking].m_sec,menv[monster_attacking].m_class, menv [monster_attacking].m_ench [2], 0));
        strcat(info, " freezes you!");
        mpr(info);
   } else
   {
        damage_taken += 5 + random2(10);
        damage_taken /= (2 + (player_res_cold() - 100) * (player_res_cold() - 100));
        strcpy(info, monam(menv[monster_attacking].m_sec,menv[monster_attacking].m_class, menv [monster_attacking].m_ench [2], 0));
        strcat(info, " chills you.");
        mpr(info);
   }

   if (player_res_cold() < 100) damage_taken += 3 + random2(6);

 scrolls_burn(1, 8);
 break;

 case 47: // Vampire
 if (you[0].is_undead != 0) break;
/*      if (damage_taken >= 7 && random2(3) == 0 | random2(20) == 0)
      {
        strcpy(info, "You feel less resilient.");
        mpr(info);
        you[0].hp_max -= random2(2) + 1;
        you[0].hp_ch = 1;
        if (you[0].hp >= you[0].hp_max) you[0].hp = you[0].hp_max;
        menv [monster_attacking].m_hp += 5 + random2(8);
        if (menv [monster_attacking].m_hp > menv [monster_attacking].m_hp_max) menv [monster_attacking].m_hp = menv [monster_attacking].m_hp_max;
      }*/
 strcpy(info, monam(menv[monster_attacking].m_sec,menv[monster_attacking].m_class, menv [monster_attacking].m_ench [2], 0));
 strcat(info, " is healed.");
 mpr(info);
 menv [monster_attacking].m_hp += random2(damage_taken);
 if (menv [monster_attacking].m_hp > menv [monster_attacking].m_hp_max) menv [monster_attacking].m_hp = menv [monster_attacking].m_hp_max;
 break;

 case 63: // shadow
 if (player_prot_life() != 0 | you[0].is_undead != 0) break;
      if (((damage_taken >= 1 && random2(3)) == 0 | random2(20) == 0) && you[0].strength > 3 && player_sust_abil() == 0)
      {
        strcpy(info, "You feel weaker.");
        mpr(info);
        you[0].strength --;
        you[0].strength_ch = 1;
      }
 break;

 case 64: // hungry ghost
 if (you[0].is_undead == 2) break;
      if (damage_taken >= 1 && random2(2) == 0 | random2(20) == 0)
      {
        strcpy(info, "You feel hungry.");
        mpr(info);
        you[0].hunger -= 400;
        //strength_ch = 1;
      }
 break;


        case 39: // Naga

        break;

 case 15: // phantom
 case 140: // wisp
 case 224: // less demon
 if (random2(3) == 0)
 {
   strcpy(info, monam(menv[monster_attacking].m_sec,menv[monster_attacking].m_class, menv [monster_attacking].m_ench [2], 0));
   strcat(info, " blinks.");
   mpr(info);
   monster_blink(monster_attacking);
 }
 break;

        case MWATER3: // jellyfish
        case 226:
// if (random2(3) != 0) break;
 if (player_res_poison() != 0) break;
 if (menv [monster_attacking].m_class == 226 && (random2(4) != 0 | runthru != 1)) break;
 strcpy(info, monam(menv[monster_attacking].m_sec,menv[monster_attacking].m_class, menv [monster_attacking].m_ench [2], 0));
        strcat(info, " stings you!");
        mpr(info);
        you[0].poison++;
 if (player_sust_abil() == 0 && you[0].strength > 3)
 {
  strcpy(info, "You feel weaker.");
  mpr(info);
  you[0].strength_ch = 1;
                you[0].strength --;
 }
        break;

        case 131: // pulsating lump
        if (random2(3) == 0) mutate(100);
        break;


        } // end of switch for special attacks.

        /* use brek for level drain, maybe with beam variables, because so many creatures use it. */

if (brek == 1)
{
drain_exp();
}

}





char drained = 0;





/* special weapons */
if (hit == 1 && menv [monster_attacking].m_inv [hand_used] != 501 | (menv [monster_attacking].m_class == 400 && ghost.ghs [8] != 0))
{
unsigned char itdam;
if (menv [monster_attacking].m_class == 400)
{
 itdam = ghost.ghs [8];
} else
{
 itdam = mitm.idam [menv [monster_attacking].m_inv [hand_used]];
}

specdam = 0;

 if (itdam == 188) goto flaming; // sword of Okawaru

if (menv [monster_attacking].m_class == 400 | mitm.idam [menv [monster_attacking].m_inv [hand_used]] < 180)
 switch(itdam % 30)
  {
   case 0: // nothing
   break;

   case 1: // flaming
   flaming : specdam = 0;
   if (player_res_fire() > 100)
   {
    damage_taken += (random2(damage_taken) / 2 + 1) / 2 + ((player_res_fire() - 100) * (player_res_fire() - 100));
   }
   if (player_res_fire() <= 100)
   {
    specdam = random2(damage_taken) / 2 + 1;
   }
   if (player_res_fire() < 100)
   {
    specdam += random2(damage_taken) / 2 + 1;
   }

    if (specdam != 0)
    {
        mmov_x = menv [monster_attacking].m_inv [hand_used];

        if (menv [monster_attacking].m_class == 144)
        {
         item_name(mitm.iplus2 [mmov_x], mitm.iclass [mmov_x], mitm.itype [mmov_x], mitm.idam [mmov_x], mitm.iplus [mmov_x], mitm.iquant [mmov_x], mitm.iid [mmov_x], 0, str_pass);
         strcpy(info, str_pass);
        } else  strcpy(info, monam(menv[monster_attacking].m_sec,menv[monster_attacking].m_class, menv [monster_attacking].m_ench [2], 0));
        strcat(info, " burns you");
        if (specdam < 3) strcat(info, ".");
        if (specdam >= 3 && specdam < 7) strcat(info, "!");
        if (specdam >= 7) strcat(info, "!!");
        mpr(info);
    }
   break;

   case 2: // freezing
   specdam = 0;
   if (player_res_cold() <= 100)
   {
    specdam = random2(damage_taken) / 2 + 1;
   }
   if (player_res_cold() < 100)
   {
    specdam += random2(damage_taken) / 2 + 1;
   }
   if (player_res_cold() > 100)
   {
    damage_taken += (random2(damage_taken) / 2 + 1) / 2 + ((player_res_cold() - 100) * (player_res_cold() - 100));
   }

    if (specdam != 0)
    {
        mmov_x = menv [monster_attacking].m_inv [hand_used];

        if (menv [monster_attacking].m_class == 144)
        {
         item_name(mitm.iplus2 [mmov_x], mitm.iclass [mmov_x], mitm.itype [mmov_x], mitm.idam [mmov_x], mitm.iplus [mmov_x], mitm.iquant [mmov_x], mitm.iid [mmov_x], 0, str_pass);
         strcpy(info, str_pass);
        } else strcpy(info, monam(menv[monster_attacking].m_sec,menv[monster_attacking].m_class, menv [monster_attacking].m_ench [2], 0));
        strcat(info, " freezes you");
        if (specdam < 3) strcat(info, ".");
        if (specdam >= 3 && specdam < 7) strcat(info, "!");
        if (specdam >= 7) strcat(info, "!!");
        mpr(info);
    }
   break;


   case 3: // holy wrath
   if (menv [monster_attacking].m_class == 400) break; /* ghosts can't wield holy wrath */
   if (you[0].is_undead != 0)
   {
    specdam = random2(damage_taken);
    if (specdam != 0)
    {
        strcpy(info, "The wound is extremely painful");
        if (specdam < 3) strcat(info, ".");
        if (specdam >= 3 && specdam < 7) strcat(info, "!");
        if (specdam >= 7) strcat(info, "!!");
        mpr(info);
    }
   }
   break;

   case 4: // electrocution
   if (you[0].lev != 0) break; // you're not grounded
   if (you[0].attribute [0] != 0) break; // resist lightning
   if (menv [monster_attacking].m_class == 400) break;
   specdam = 0;

   if (mitm.iplus2 [menv [monster_attacking].m_inv [hand_used]] <= 50 | mitm.iplus2 [menv [monster_attacking].m_inv [hand_used]] > 130 && mitm.iplus2 [menv [monster_attacking].m_inv [hand_used]] <= 150) break;
   if (random2(3) == 0)
   {
    strcpy(info, "You are electrocuted!");
    mpr(info);
    specdam += 10 + random2(15);
    mitm.iplus2 [menv [monster_attacking].m_inv [hand_used]] --;
   }
   break;

   case 5: // orc slaying
   if (you[0].species == 10)
   {
    specdam = random2(damage_taken);
    if (specdam != 0)
    {
        strcpy(info, "The wound is extremely painful");
        if (specdam < 3) strcat(info, ".");
        if (specdam >= 3 && specdam < 7) strcat(info, "!");
        if (specdam >= 7) strcat(info, "!!");
        mpr(info);
    }
   }
   break;

   case 6: // venom
   if (random2(3) == 0 && player_res_poison() == 0)
   {
        mmov_x = menv [monster_attacking].m_inv [hand_used];

        if (menv [monster_attacking].m_class == 144)
        {
         item_name(mitm.iplus2 [mmov_x], mitm.iclass [mmov_x], mitm.itype [mmov_x], mitm.idam [mmov_x], mitm.iplus [mmov_x], mitm.iquant [mmov_x], mitm.iid [mmov_x], 0, str_pass);
         strcpy(info, str_pass);
        } else strcpy(info, monam(menv[monster_attacking].m_sec,menv[monster_attacking].m_class, menv [monster_attacking].m_ench [2], 0));
        if (menv [monster_attacking].m_class != 144)
        strcat(info, "'s weapon is poisoned!");
        else strcat(info, " is poisoned!");

                                        you[0].poison += 2;
   }
   break;

   case 7: // protection
   break;

   case 8: if (player_prot_life() != 0) break;
   strcpy(info, "You feel drained...");
   mpr(info);
   drained = 1;
   specdam = random2(damage_taken) / 2 + 1;
   break;

   case 9: // you[0].speed
   menv [monster_attacking].m_speed_inc -= menv [monster_attacking].m_speed / 2;
   break;

   case 10: // slicing etc
   specdam = random2(damage_taken) / 2 + 1;
   break;

   case 13: // vampiric
   specdam = 0; // note does no extra damage
   if (you[0].is_undead != 0) break;
   if (random2(5) == 0 | menv [monster_attacking].m_hp == menv [monster_attacking].m_hp_max) break;
   menv [monster_attacking].m_hp += random2(damage_taken) + 1; // thus is probably more valuable on larger weapons?
   if (menv [monster_attacking].m_hp > menv [monster_attacking].m_hp_max) menv [monster_attacking].m_hp = menv [monster_attacking].m_hp_max;
//   if (you[0].hunger <= 11000) you[0].hunger += random() % 30;
   if (menv [monster_attacking].m_ench [2] != 6 | player_see_invis() != 0)
   {
    if (menv [monster_attacking].m_class == 144)
    {
     item_name(mitm.iplus2 [mmov_x], mitm.iclass [mmov_x], mitm.itype [mmov_x], mitm.idam [mmov_x], mitm.iplus [mmov_x], mitm.iquant [mmov_x], mitm.iid [mmov_x], 0, str_pass);
     strcpy(info, str_pass);
    } else strcpy(info, monam(menv[monster_attacking].m_sec,menv[monster_attacking].m_class, menv [monster_attacking].m_ench [2], 0));
    strcat(info, " is healed.");
    mpr(info);
   }
   break;

   case 14: // disruption
   if (menv [monster_attacking].m_class == 400) break;
   if (you[0].is_undead != 0)
   {
    specdam = random2(damage_taken) + random2(damage_taken) + random2(damage_taken) + random2(damage_taken);
    if (specdam != 0)
    {
        strcpy(info, "You are blasted by holy energy");
        if (specdam < 7) strcat(info, ".");
        if (specdam >= 7 && specdam < 15) strcat(info, "!");
        if (specdam >= 15) strcat(info, "!!");
        mpr(info);
    }
   }
   break;


   case 16: // distortion
//   if (random2(3) != 0) break;
   if (random2(3) == 0)
   {
    strcpy(info, "Your body is twisted painfully.");
    mpr(info);
    specdam += random2(5) + random2(3) + 1;
    break;
   }
   if (random2(3) == 0)
   {
    strcpy(info, "Your body is terribly warped!");
    mpr(info);
    specdam += random2(12) + random2(13) + 3;
    break;
   }
   if (random2(3) == 0)
   {
    random_blink();
    break;
   }
   if (random2(2) == 0)
   {
    you_teleport();
    break;
   }
   if (random2(2) == 0)
   {
    you_teleport2(1);
    break;
   }
   if (random2(2) == 0)
   {
    banished();
    break;
   }
   break;


  } // end of switch
} // end of special weapons

if (drained == 1) drain_exp();

damage_taken += specdam;

if (damage_taken > 0 && damage_taken < 150) /* ultra-high damages are assumed buggy */
{
        ouch(damage_taken, monster_attacking, 0);
        you[0].hp_ch = 1;
        if (you[0].religion == 5 && you[0].hp <= you[0].hp / 3 && random2(10) == 0) Xom_acts(1, you[0].xl, 0);

}


} // end of for runthru


return;
} // end of void monster_attack(int monster_attacking)










char monsters_fight(int monster_attacking, int monster_attacked)
{
//float mons_to_hit;
int damage_taken = 0;
char hit = 0;
int mmov_x = 0;
char str_pass [80];

int specdam = 0;

int hand_used = 0;

int sees = 0;

if (menv [monster_attacking].m_class == 32)
{
                menv [monster_attacking].m_hp = -1;
                return 0;
}

if (menv [monster_attacking].m_class >= MLAVA0 && menv [monster_attacking].m_sec == 1 && menv [monster_attacked].m_class < MLAVA0) return 0;

if (menv [monster_attacking].m_beh == 7)
{
        if (menv [monster_attacked].m_beh == 7) return 0;
        menv [monster_attacking].m_hit = monster_attacked;
}

if (menv [monster_attacked].m_beh == 6)
{
        if (menv [monster_attacked].m_beh != 7 && monster_attacked != menv [monster_attacking].m_hit)
                        return 0;
}

//int runthru = 0;

//mons_to_hit = menv [monster_attacking].m_HD; // not finished yet.

if (mons_near(monster_attacking) == 1 && mons_near(monster_attacked) == 1) sees = 1;

char runthru;

for (runthru = 0; runthru < 4; runthru ++)
{

char mdam = mondamage(menv [monster_attacking].m_class, runthru);

if (menv [monster_attacking].m_class == 25 | menv [monster_attacking].m_class == 51 | menv [monster_attacking].m_class == 107 | menv [monster_attacking].m_class == 108 | menv [monster_attacking].m_class == 367)
{
        mdam = mondamage(menv [monster_attacking].m_sec, runthru);
        if (mdam > 1 && mdam < 4)
        {
                mdam --;
        }

        if (mdam > 3 && mdam < 8)
        {
                mdam -= 2;
        }

        if (mdam > 7 && mdam < 13)
        {
                mdam -= 3;
        }

        if (mdam > 12)
        {
                mdam -= 5;
        }
}


if (mdam == 0) break;
//gmon_att [menv [monster_attacking].m_class] [runthru] == 0) break;

if (menv [monster_attacking].m_class == 30 && runthru == 1 && menv [monster_attacking].m_inv [1] != 501)
{
        hand_used = 1;
}

damage_taken = 0;


int mons_to_hit = 20 + menv [monster_attacking].m_HD; // * menv [monster_attacking].m_HD; // * 3  //you[0].strength + (0.5 * you[0].you[0].max_dex) + (you[0].f_abil);

if (menv [monster_attacking].m_inv [hand_used] != 501)
{
        if (mitm.iplus [menv [monster_attacking].m_inv [hand_used]] - 50 > 130)
        {
                mons_to_hit += mitm.iplus [menv [monster_attacking].m_inv [hand_used]] - 50 - 100;  //random2(mitm.iplus [menv [monster_attacking].m_inv [hand_used]] - 50 - 100 + 1);
        } else
              {
                mons_to_hit += mitm.iplus [menv [monster_attacking].m_inv [hand_used]] - 50; //random2(mitm.iplus [menv [monster_attacking].m_inv [hand_used]] - 50 + 1); /// 10;
              }

//      mons_to_hit += mitm.iplus [menv [monster_attacking].m_inv [0]];
        mons_to_hit += 3 * property(0, mitm.itype [menv [monster_attacking].m_inv [0]], 1);

   menv [monster_attacking].m_speed_inc -= ((property(you[0].inv_class [you[0].equip [0]], you[0].inv_type [you[0].equip [0]], 2) - 10) / 2); // / 10);

}

//if (you[0].inv_class [you[0].equip [0]] == 0) mons_to_hit += you[0].inv_plus [you[0].equip [0]];


//mons_to_hit *= random2(200) / 100;

mons_to_hit = random2(mons_to_hit);

/*
//if (mons_to_hit >= (player_evasion() - 6)) //&& mons_to_hit < player_evasion() && shield_armour != -1) // && random2(6) != 0)
if (mons_to_hit < player_evasion())
{
strcpy(info, "You block the ");
strcat(info, gmon_name [menv [monster_attacking].m_class]);
if (menv [monster_attacking].m_inv [0] != 255)
        {
        strcat(info, "'s ");
        strcat(info, fake_name [mmov_x]);
        }
strcat(info, " on your shield!");
mpr(info);
return;
}
*/


        // This bit is a bit clumsy:
   //      not any more.
        if (menv [monster_attacking].m_class == 10 | menv [monster_attacking].m_class == 22)
        {
/*              if (you[0].haste == 0) menv [monster_attacking].m_speed_inc -= (menv [monster_attacking].m_speed * time_taken);
                        else menv [monster_attacking].m_speed_inc -= (menv [monster_attacking].m_speed * (time_taken / 2));

                if (you[0].slow > 0) menv [monster_attacking].m_speed_inc -= (menv [monster_attacking].m_speed * time_taken);*/

                // maybe this will work better:
                menv [monster_attacking].m_speed_inc -= 10; //--
        }






if (mons_to_hit >= menv [monster_attacked].m_ev | ((menv [monster_attacked].m_speed_inc <= 60 | menv [monster_attacked].m_beh == 0) && random2 (20) != 0))// | random2(6) == 0) // menv [monster_attacked].m_ev)
//if (random2(20) + mons_to_hit >= 20 - (10 - player_AC())) // hit roll
{
hit = 1;

//te = menv [monster_attacking].m_inv [0];

//k = mitm.iclass [te];
//p = mitm.itype [te];
//j = property [k] [p] [0];
//o = menv [monster_attacking].m_class;
//k = gmon_att [o] [runthru];


if (menv [monster_attacking].m_inv [hand_used] != 501 && mitm.iclass [menv [monster_attacking].m_inv [hand_used]] == 0 && mitm.itype [menv [monster_attacking].m_inv [hand_used]] < 13 | mitm.itype [menv [monster_attacking].m_inv [hand_used]] > 15)
{
//damage_taken = random2(mmov_x);

damage_taken = random2(property(mitm.iclass [menv [monster_attacking].m_inv [0]], mitm.itype [menv [monster_attacking].m_inv [0]], 0));

  if (mitm.idam [menv [monster_attacking].m_inv [hand_used]] / 30 == 3 && mons_charclass(menv [monster_attacking].m_class) == 14)
  {
   damage_taken += random2(2);
  }

//if (mitm.iplus [mons_inv [i] [0]] > 80) damage_taken -= 100;
//damage_taken += mitm.iplus [mons_inv [i] [0]];
        if (mitm.iplus2 [menv [monster_attacking].m_inv [hand_used]] > 130)
        {
                damage_taken += random2(mitm.iplus2 [menv [monster_attacking].m_inv [hand_used]] - 150 + 1);
        } else
              {
                damage_taken += random2(mitm.iplus2 [menv [monster_attacking].m_inv [hand_used]] - 50 + 1);// % 10;
              }
damage_taken -= random2(3) + 1;//1;
}

damage_taken += random2(mdam) + 1;
//gmon_att [menv [monster_attacking].m_class] [runthru]) + 1;
//damage_taken /= (random2 (player_AC()) + 1); // / 3
//damage_taken *= 2;

//if (menv [monster_attacking].m_inv [0] != 501)
//{
//      damage_taken /= ((random2 (player_AC()) / property [mitm.iclass [mons_inv [i] [0]]] [mitm.itype [mons_inv [i] [0]]] [2]) + 1);
//} else
//      damage_taken /= (random2 (player_AC()) + 1);

        damage_taken -= random2(menv [monster_attacked].m_AC + 1);

if (damage_taken < 1) damage_taken = 0;
//you[0].hp -= (int) damage_taken;
//ouch(damage_taken, monster_attacking, 0);

//you[0].hp_ch = 1;

} // end of if mons_to_hit >= player_evasion()
 else
 {
        hit = 0;
//      strcpy(info, "The ");
        //} else
        //strcat(info, "The ");

        int mmov_x = menv [monster_attacking].m_inv [hand_used];

        char str_pass [80];

        if (menv [monster_attacking].m_class == 144)
        {
         item_name(mitm.iplus2 [mmov_x], mitm.iclass [mmov_x], mitm.itype [mmov_x], mitm.idam [mmov_x], mitm.iplus [mmov_x], mitm.iquant [mmov_x], mitm.iid [mmov_x], 0, str_pass);
         strcpy(info, str_pass);
        } else  strcpy(info, monam(menv[monster_attacking].m_sec,menv[monster_attacking].m_class, menv [monster_attacking].m_ench [2], 0));  //gmon_name [menv [monster_attacking].m_class]);
        strcat(info, " misses ");
        strcat(info, monam(menv[monster_attacked].m_sec,menv[monster_attacked].m_class, menv [monster_attacked].m_ench [2], 1));
   strcat(info, ".");
   if (sees == 1) mpr(info);
 }


if (damage_taken < 1 && hit == 1)
{
//if (78 - strlen(info) < strlen(gmon_name [menv [monster_attacking].m_class]) + 17)
//      {
        //mpr(info);
//      strcpy(info, "The ");
        //} else
        //strcat(info, "The ");


        int mmov_x = menv [monster_attacking].m_inv [hand_used];
        char str_pass [80];

        if (menv [monster_attacking].m_class == 144)
        {
         item_name(mitm.iplus2 [mmov_x], mitm.iclass [mmov_x], mitm.itype [mmov_x], mitm.idam [mmov_x], mitm.iplus [mmov_x], mitm.iquant [mmov_x], mitm.iid [mmov_x], 0, str_pass);
         strcpy(info, str_pass);
        } else  strcpy(info, monam(menv[monster_attacking].m_sec,menv[monster_attacking].m_class, menv [monster_attacking].m_ench [2], 0));  //gmon_name [menv [monster_attacking].m_class]);
        strcat(info, " hits ");
        strcat(info, monam(menv[monster_attacked].m_sec,menv[monster_attacked].m_class, menv [monster_attacked].m_ench [2], 1));
#ifdef DEBUG
itoa(damage_taken, st_prn, 10);
strcat(info, " for ");
strcat(info, st_prn); // note: doesn't take account of special weapons etc
#endif
        strcat(info, ".");// but doesn't do any you[0].damage.");
 if (sees == 1) mpr(info);
}



if (hit == 1) //(int) damage_taken >= 1)
{
//if (78 - strlen(info) < strlen(gmon_name [menv [monster_attacking].m_class]) + 15)
//      {
//      mpr(info);
//      strcpy(info, "The ");
//      } else
//      strcpy(info, "The ");

        int mmov_x = menv [monster_attacking].m_inv [hand_used];

        if (menv [monster_attacking].m_class == 144)
        {
         char str_pass [80];
         item_name(mitm.iplus2 [mmov_x], mitm.iclass [mmov_x], mitm.itype [mmov_x], mitm.idam [mmov_x], mitm.iplus [mmov_x], mitm.iquant [mmov_x], mitm.iid [mmov_x], 0, str_pass);
         strcpy(info, str_pass);
        } else strcpy(info, monam(menv[monster_attacking].m_sec,menv[monster_attacking].m_class, menv [monster_attacking].m_ench [2], 0));
        strcat(info, " hits ");
        strcat(info, monam(menv[monster_attacked].m_sec,menv[monster_attacked].m_class, menv [monster_attacked].m_ench [2], 1));


/*        strcat(info, " for ");
        itoa(damage_taken, st_prn, 10);
        strcat(info, st_prn);
*/


//      itoa(

        if (menv [monster_attacking].m_class != 144 && menv [monster_attacking].m_inv [hand_used] != 501 && mitm.iclass [menv [monster_attacking].m_inv [hand_used]] == 0 && mitm.itype [menv [monster_attacking].m_inv [hand_used]] < 13 | mitm.itype [menv [monster_attacking].m_inv [hand_used]] > 16)
        {
        strcat(info, " with ");

        item_name(mitm.iplus2 [mmov_x], mitm.iclass [mmov_x], mitm.itype [mmov_x], mitm.idam [mmov_x], mitm.iplus [mmov_x], mitm.iquant [mmov_x], mitm.iid [mmov_x], 3, str_pass); // was 7
        strcat(info, str_pass);
        }
        strcat(info, "! "); // put something for '.', '!', '!!' etc depending on what % of you[0].damage you took.

 if (sees == 1) mpr(info);

        // special attacks:

        /* brek = 0; */



        switch(menv [monster_attacking].m_class)
        {
        case 0: // giant ant
        case 62:
        case 139:
        case 143: // spiny worm
        case MWATER3: // jellyfish
        case 226: // demon
        if ((damage_taken >= 4 && random2(4) == 0) | random2(20) == 0 | menv [monster_attacking].m_class == 143)
        {
      strcpy(info, monam(menv[monster_attacking].m_sec,menv[monster_attacking].m_class, menv [monster_attacking].m_ench [2], 0));
      strcat(info, " stings ");
      strcat(info, monam(menv[monster_attacked].m_sec,menv[monster_attacked].m_class, menv [monster_attacked].m_ench [2], 1));
      strcat(info, ".");
      if (sees == 1) mpr(info);
      //        you[0].poison++;
      poison_monster(monster_attacked, 1);
   }
        break;

        case 2: // cockatrice.
        break;

        case 10: // killer bee
        case 138:
        if (((damage_taken >= 3 && random2(3) == 0) | random2(20) == 0))
        {
     strcpy(info, monam(menv[monster_attacking].m_sec,menv[monster_attacking].m_class, menv [monster_attacking].m_ench [2], 0));
     strcat(info, " stings ");
     strcat(info, monam(menv[monster_attacked].m_sec,menv[monster_attacked].m_class, menv [monster_attacked].m_ench [2], 1));
     strcat(info, ".");
     if (sees == 1) mpr(info);
     // you[0].poison += 1;
     poison_monster(monster_attacked, 1);
   }
        break;

        case 13: // necrophage
        case 81: /* rotting devil */
        case 156: /* ghoul */
        case 277: // death ooze
        if ((damage_taken >= 3 && random2(3) == 0) | random2(20) == 0)
        {
                //strcpy(info, "You feel your flesh start to rot away!");
                //mpr(info);
                //you[0].poison += random2(3) + 1;
      menv [monster_attacked].m_hp_max -= random2(3) + 1;
      if (menv [monster_attacked].m_hp > menv [monster_attacked].m_hp_max) menv [monster_attacked].m_hp = menv [monster_attacked].m_hp_max;
        }
        break;

 case 21: // fire_vortex
 menv [monster_attacking].m_hp = -10;
 case 124: // fire elem
 case 233: // gr demon
   specdam = 0;
   if (mons_res_fire(menv [monster_attacked].m_class) == 0)
   {
    specdam = random2(15) + 15;
    if (menv [monster_attacked].m_inv [2] != 501 && mitm.idam [menv [monster_attacked].m_inv [2]] % 30 == 2)
     specdam /= 3;
   }
   if (mons_res_fire(menv [monster_attacked].m_class) == -1 | (menv [monster_attacked].m_inv [2] == 501 | mitm.idam [menv [monster_attacked].m_inv [2]] % 30 != 2))
   {
    specdam = random2(25) + 20;
   }
    if (specdam != 0)
    {
      if (sees == 1)
      {
        strcpy(info, monam(menv[monster_attacked].m_sec,menv[monster_attacked].m_class, menv [monster_attacked].m_ench [2], 0));
        strcat(info, " is engulfed in flame!");
        //if (sees == 1)
                                                                mpr(info);
      }
    }
   damage_taken += specdam;
 break;

        case 42: // killer bee
//      if ((damage_taken >= 3 && random2(3) == 0) | random2(20) == 0)
//      {
        strcpy(info, "The queen bee stings ");
   strcat(info, monam(menv[monster_attacked].m_sec,menv[monster_attacked].m_class, menv [monster_attacked].m_ench [2], 1));
   strcat(info, ".");
   if (sees == 1) mpr(info);
   poison_monster(monster_attacked, 1);
//      you[0].poison += 2;
//      }
        break;


        case 18: // snake
        case 128: // br snake
        case 154: // black snake
        case 166: // yellow snake
/*        case 224: // less demon */
        if ((damage_taken >= 3 && random2(4) == 0) | random2(20) == 0)
        {
//        strcpy(info, "The ");
   strcpy(info, monam(menv[monster_attacked].m_sec,menv[monster_attacked].m_class, menv [monster_attacked].m_ench [2], 0));
   strcat(info, " is poisoned.");
   if (sees == 1) mpr(info);
        poison_monster(monster_attacked, 1);
        }
        break;

        case 165: // shadow dragon
        case 367: /* spectral thing */
        if (random2(2) == 0) break;
        case 60: // wight
        case 48: // wraith
        case 84: // soul eater
        case 127: // sh F
        case 130: // spectre
        case 153: // orange rat
        if ((damage_taken >= 6 && random2(2) == 0) | random2(30) == 0)
        {
      strcpy(info, monam(menv[monster_attacked].m_sec,menv[monster_attacked].m_class, menv [monster_attacked].m_ench [2], 0));
      strcat(info, " is drained.");
      if (sees == 1) mpr(info);
      if (random2(5) == 0) menv [monster_attacked].m_HD --;
      menv [monster_attacked].m_hp_max -= 2 + random2(3);
      menv [monster_attacked].m_hp -= 2 + random2(3);
      if (menv [monster_attacked].m_hp >= menv [monster_attacked].m_hp_max) menv [monster_attacked].m_hp = menv [monster_attacked].m_hp_max;
      if (menv [monster_attacked].m_hp <= 0 | menv [monster_attacked].m_HD <= 0)
      {
            monster_die(monster_attacked, 2, monster_attacking);
            return 1;
      }
      //brek = 1;
        }
        break;

        case 22: // giant wasp
/*      if ((damage_taken >= 3 && random2(3) == 0) | random2(20) == 0)
        {
        if (you[0].paralysis > 0)
        {
                strcpy(info, "You still can't move!");
        } else strcpy(info, "You suddenly lose the ability to move!");
        mpr(info);
        you[0].paralysis += random2(3) + 1;
        }*/
        break;

        case 35: // Jelly
//      strcpy(info, "You are splashed with acid!");
//      mpr(info);
//      splash_with_acid(10);
        break;




 case 86: // ice devil
 case 34: // Ice beast
 case 77: // blue wraith
 case 126: // I Fiend
 case 220: // lesser demon
 case 276: // azure jelly
 case 343: // Antaeus
 specdam = 0;
   if (mons_res_cold(menv [monster_attacked].m_class) == 0)
   {
    specdam = random2(10) + 5;
    if (menv [monster_attacked].m_inv [2] != 501 && mitm.idam [menv [monster_attacked].m_inv [2]] % 30 == 3)
     specdam = (random2(10) + 5) / 3;
   }
   if (mons_res_cold(menv [monster_attacked].m_class) == -1 && (menv [monster_attacked].m_inv [2] == 501 | mitm.idam [menv [monster_attacked].m_inv [2]] % 30 != 3))
   {
    specdam = random2(15) + 10;
   }
    if (specdam != 0)
    {
      if (sees == 1)
      {
        strcpy(info, monam(menv[monster_attacking].m_sec,menv[monster_attacking].m_class, menv [monster_attacking].m_ench [2], 0));
        strcat(info, " freezes ");
        strcat(info, monam(menv[monster_attacked].m_sec,menv[monster_attacked].m_class, menv [monster_attacked].m_ench [2], 1));
        if (sees == 1) mpr(info);
      }
    }
   damage_taken += specdam;
 break;

 case 47: // Vampire
/* if (damage_taken >= 7 && random2(2) == 0 | random2(10) == 0)
 menv [monster_attacked].m_hp_max -= 3;
 if (menv [monster_attacked].m_hp >= menv [monster_attacked].m_hp_max) menv [monster_attacked].m_hp = menv [monster_attacked].m_hp_max;
 menv [monster_attacking].m_hp += random2(8);
 if (menv [monster_attacking].m_hp > menv [monster_attacking].m_hp_max) menv [monster_attacking].m_hp = menv [monster_attacking].m_hp_max;*/
 if (mons_holiness(menv [monster_attacked].m_class) >= 1) break;
 strcpy(info, monam(menv[monster_attacking].m_sec,menv[monster_attacking].m_class, menv [monster_attacking].m_ench [2], 0));
 strcat(info, " is healed.");
 if (mons_near(monster_attacking)) mpr(info);
 menv [monster_attacking].m_hp += random2(damage_taken);
 if (menv [monster_attacking].m_hp > menv [monster_attacking].m_hp_max) menv [monster_attacking].m_hp = menv [monster_attacking].m_hp_max;
 break;




        case 39: // Naga

        break;


        } // end of switch for special attacks.


}
/* special weapons */
if (hit == 1 && menv [monster_attacking].m_inv [hand_used] != 501 | (menv [monster_attacking].m_class == 400 && ghost.ghs [8] != 0))
{
unsigned char itdam;
if (menv [monster_attacking].m_class == 400)
{
 itdam = ghost.ghs [8];
} else
{
 itdam = mitm.idam [menv [monster_attacking].m_inv [hand_used]];
}

specdam = 0;

 if (itdam == 188) goto flaming; // sword of Okawaru

if (menv [monster_attacking].m_class == 400 | mitm.idam [menv [monster_attacking].m_inv [hand_used]] < 180)
 switch(itdam % 30)
  {

   case 0: // nothing
   break;

   case 1: // flaming
   flaming : specdam = 0;
   if (mons_res_fire(menv [monster_attacked].m_class) == 0)
   {
    specdam = random2(damage_taken) / 2 + 1;
    if (menv [monster_attacked].m_inv [2] != 501 && mitm.idam [menv [monster_attacked].m_inv [2]] % 30 == 2)
     specdam /= 3;
   }
   if (mons_res_fire(menv [monster_attacked].m_class) == -1 && (menv [monster_attacked].m_inv [2] == 501 | mitm.idam [menv [monster_attacked].m_inv [2]] % 30 != 2))
   {
    specdam = random2(damage_taken) + 1;
   }
    if (specdam != 0)
    {
      if (sees == 1)
      {
        int mmov_x = menv [monster_attacking].m_inv [hand_used];
        char str_pass [80];

        if (menv [monster_attacking].m_class == 144)
        {
         item_name(mitm.iplus2 [mmov_x], mitm.iclass [mmov_x], mitm.itype [mmov_x], mitm.idam [mmov_x], mitm.iplus [mmov_x], mitm.iquant [mmov_x], mitm.iid [mmov_x], 0, str_pass);
         strcpy(info, str_pass);
        } else  strcpy(info, monam(menv[monster_attacking].m_sec,menv[monster_attacking].m_class, menv [monster_attacking].m_ench [2], 0));
        strcat(info, " burns ");
        strcat(info, monam(menv[monster_attacked].m_sec,menv[monster_attacked].m_class, menv [monster_attacked].m_ench [2], 1));
        if (specdam < 3) strcat(info, ".");
        if (specdam >= 3 && specdam < 7) strcat(info, "!");
        if (specdam >= 7) strcat(info, "!!");
        //    strcat(info, " is burned.");
        mpr(info);
      }
    }
   break;

   case 2: // freezing
   specdam = 0;
   if (mons_res_cold(menv [monster_attacked].m_class) == 0)
   {
    specdam = random2(damage_taken) / 2 + 1;
    if (menv [monster_attacked].m_inv [2] != 501 && mitm.idam [menv [monster_attacked].m_inv [2]] % 30 == 3)
     specdam = (random2(damage_taken) / 2 + 1) / 3;
   }
   if (mons_res_cold(menv [monster_attacked].m_class) == -1 && (menv [monster_attacked].m_inv [2] == 501 | mitm.idam [menv [monster_attacked].m_inv [2]] % 30 != 3))
   {
    specdam = random2(damage_taken) + 1;
   }
    if (specdam != 0)
    {
      if (sees == 1)
      {
        mmov_x = menv [monster_attacking].m_inv [hand_used];

        if (menv [monster_attacking].m_class == 144)
        {
         item_name(mitm.iplus2 [mmov_x], mitm.iclass [mmov_x], mitm.itype [mmov_x], mitm.idam [mmov_x], mitm.iplus [mmov_x], mitm.iquant [mmov_x], mitm.iid [mmov_x], 0, str_pass);
         strcpy(info, str_pass);
        } else strcpy(info, monam(menv[monster_attacking].m_sec,menv[monster_attacking].m_class, menv [monster_attacking].m_ench [2], 0));
        strcat(info, " freezes ");
        strcat(info, monam(menv[monster_attacked].m_sec,menv[monster_attacked].m_class, menv [monster_attacked].m_ench [2], 1));
        if (specdam < 3) strcat(info, ".");
        if (specdam >= 3 && specdam < 7) strcat(info, "!");
        if (specdam >= 7) strcat(info, "!!");
        //    strcat(info, " is burned.");
        mpr(info);
      }
    }
   break;

   case 3: // holy wrath
   if (menv [monster_attacking].m_class == 400) break;
   specdam = 0;
   switch(mons_holiness(menv [monster_attacked].m_class))
   {
    case -1:
//    strcpy(info, "
      damage_taken -= 5 + random2(5);
    break;

    case 1:
      specdam += random2(damage_taken) + 1;
    break;

    case 2:
      specdam += (random2(damage_taken) * 15) / 10 + 1; // does * 1.5 do anything?
    break;
   }
   break;

   case 4: // electrocution
//   if (you[0].lev != 0) break; // you're not grounded
   if (menv [monster_attacking].m_class == 400) break;
   if (mons_flies(menv [monster_attacked].m_class) == 2) break;
   if (mons_res_elec(menv [monster_attacked].m_class) != 0) break;
   specdam = 0;

   if (mitm.iplus2 [menv [monster_attacking].m_inv [hand_used]] <= 50 | mitm.iplus2 [menv [monster_attacking].m_inv [hand_used]] > 130 && mitm.iplus2 [menv [monster_attacking].m_inv [hand_used]] <= 150) break;
   if (random2(3) == 0)
   {
    strcpy(info, "There is a sudden explosion of sparks!");
    if (sees == 1) mpr(info);
    specdam += 10 + random2(15);
    mitm.iplus2 [menv [monster_attacking].m_inv [hand_used]] --;
   }
   break;

   case 5: // orc slaying
   if (mons_charclass(menv [monster_attacked].m_class) == 14)
//   if (menv [monster_attacked].m_class == 14 | menv [monster_attacked].m_class == 52 | menv [monster_attacked].m_class == 54 | menv [monster_attacked].m_class == 55 | menv [monster_attacked].m_class == 103)
   {
     menv [monster_attacked].m_hp -= 1 + random2(damage_taken);
   }
   break;

   case 6: // venom
   if (random2(3) == 0) poison_monster(monster_attacked, 0);
   break;

//   case 7: // protection
 case 8:
        if (mons_holiness(menv [monster_attacked].m_class) > 0 && ((damage_taken >= 6 && random2(2) == 0) | random2(30) == 0))
        {
      strcpy(info, monam(menv[monster_attacked].m_sec,menv[monster_attacked].m_class, menv [monster_attacked].m_ench [2], 0));
      strcat(info, " is drained.");
      if (sees == 1) mpr(info);
      if (random2(5) == 0) menv [monster_attacked].m_HD --;
      menv [monster_attacked].m_hp_max -= 2 + random2(3);
      menv [monster_attacked].m_hp -= 2 + random2(3);
      if (menv [monster_attacked].m_hp >= menv [monster_attacked].m_hp_max) menv [monster_attacked].m_hp = menv [monster_attacked].m_hp_max;
      if (menv [monster_attacked].m_hp <= 0 | menv [monster_attacked].m_HD <= 0)
      {
            monster_die(monster_attacked, 2, monster_attacking);
            return 1;
      }
      specdam = random2(damage_taken) / 2 + 1;
        }
 break;

  case 9: // you[0].speed
  menv [monster_attacking].m_speed_inc -= menv [monster_attacking].m_speed / 2;
  break;

  case 10: // slicing etc
  specdam += (random2(damage_taken) / 2) + 1;
  break;

   case 13: // vampiric
   specdam = 0; // note does no extra damage
   if (mons_holiness(menv [monster_attacked].m_class) > 0) break;
   if (random2(5) == 0 | menv [monster_attacking].m_hp == menv [monster_attacking].m_hp_max) break;
   menv [monster_attacking].m_hp += random2(damage_taken) + 1; // thus is probably more valuable on larger weapons?
   if (menv [monster_attacking].m_hp > menv [monster_attacking].m_hp_max) menv [monster_attacking].m_hp = menv [monster_attacking].m_hp_max;
//   if (you[0].hunger <= 11000) you[0].hunger += random() % 30;
   if (mons_near(monster_attacking) == 1 && (menv [monster_attacking].m_ench [2] != 6 | player_see_invis() != 0))
   {
    if (menv [monster_attacking].m_class == 144)
    {
     item_name(mitm.iplus2 [mmov_x], mitm.iclass [mmov_x], mitm.itype [mmov_x], mitm.idam [mmov_x], mitm.iplus [mmov_x], mitm.iquant [mmov_x], mitm.iid [mmov_x], 0, str_pass);
     strcpy(info, str_pass);
    } else strcpy(info, monam(menv[monster_attacking].m_sec,menv[monster_attacking].m_class, menv [monster_attacking].m_ench [2], 0));
    strcat(info, " is healed.");
    mpr(info);
   }
   break;

   case 14: // mace of disruption
   if (menv [monster_attacking].m_class == 400) break;
   specdam = 0;
   if (mons_holiness(menv [monster_attacked].m_class) == 1 && random2(3) != 0)
   {
    strcpy(info, monam(menv[monster_attacked].m_sec,menv[monster_attacked].m_class, menv [monster_attacked].m_ench [2], 0));
    strcat(info, " shudders.");
    if (mons_near(monster_attacked) == 1) mpr(info);
    specdam += random2(damage_taken + 1);
    specdam += random2(damage_taken + 1);
    specdam += random2(damage_taken + 1);
/*    if (menv [monster_attacked].m_ench [1] >= 20 && menv [monster_attacked].m_ench [1] <= 25)
        menv [monster_attacked].m_ench [1] -= 1;
    if (menv [monster_attacked].m_ench [1] == 19) menv [monster_attacked].m_ench [1] == 20;*/
   }
   break;


   case 16: // distortion
//   if (random2(3) != 0) break;
   if (random2(3) == 0)
   {
    strcpy(info, "Space bends around ");
    strcat(info, monam(menv[monster_attacked].m_sec,menv[monster_attacked].m_class, menv [monster_attacked].m_ench [2], 1));
    strcat(info, ".");
    if (mons_near(monster_attacked) == 1) mpr(info);
    specdam += random2(5) + random2(3) + 1;
    break;
   }
   if (random2(3) == 0)
   {
    strcpy(info, "Space warps horribly around ");
    strcat(info, monam(menv[monster_attacked].m_sec,menv[monster_attacked].m_class, menv [monster_attacked].m_ench [2], 1));
    strcat(info, "!");
    if (mons_near(monster_attacked) == 1) mpr(info);
    specdam += random2(12) + random2(13) + 3;
    break;
   }
   if (random2(3) == 0)
   {
    monster_blink(monster_attacked);
    break;
   }
   if (random2(2) == 0)
   {
    monster_teleport(monster_attacked, 0);
    break;
   }
   if (random2(2) == 0)
   {
    monster_teleport(monster_attacked, 1);
    break;
   }
   if (random2(2) == 0)
   {
    monster_die(monster_attacked, 6, monster_attacking);
    break;
   }
   break;



  }
} // end of if special weapon

   damage_taken += specdam;

if (damage_taken > 0)
{
        //ouch(damage_taken, monster_attacking, 0);
        //you[0].hp_ch = 1;
   menv [monster_attacked].m_hp -= damage_taken;
   if (menv [monster_attacked].m_hp <= 0)
   {
      monster_die(monster_attacked, 2, monster_attacking);
      return 1;
   }
}


} // end of for runthru

if (menv [monster_attacked].m_beh == 7)
{
//   menv [monster_attacked].m_beh = 6;
                menv [monster_attacked].m_hit = monster_attacking;
   menv [monster_attacking].m_hit = monster_attacked;
//  }
}

if (menv [monster_attacking].m_beh != 4)// && menv [monster_attacking].m_beh != 7) // the latter is so that mons_beh [attacker] doesn't get reset to 6
{
  if (menv [monster_attacked].m_beh <= 2 && distance(you[0].x_pos, menv [monster_attacked].m_x, you[0].y_pos, menv [monster_attacked].m_y) > 1)
  {
    menv [monster_attacked].m_beh = 6;
    menv [monster_attacked].m_hit = monster_attacking; // This'll do for now.
  }
}

return 1;
} // end of void monsters_fight(int monster_attacking, ...)



void monster_die(int monster_killed, char killer, int i)
{

int dmi; /* dead monster's inventory */

if (you[0].prev_targ == monster_killed) you[0].prev_targ = MHITNOT;

if (menv [monster_killed].m_class == 32)
{
        if (menv [monster_killed].m_hp < 1 && menv [monster_killed].m_hp > -15)
        {
                return;
        }
}

if (menv [monster_killed].m_class == 21 | menv [monster_killed].m_class == 244)
{
strcpy(info, monam (menv[monster_killed].m_sec,menv[monster_killed].m_class, menv [monster_killed].m_ench [2], 0));
strcat(info, " dissipates!"); // <- spelling?
if (mons_near(monster_killed) == 1) mpr(info);
if (killer == 1 | killer == 3)
{
 gain_exp(exper_value(menv [monster_killed].m_class, menv [monster_killed].m_HD, menv [monster_killed].m_hp_max));
}
if (menv [monster_killed].m_class == 21) place_cloud(101, menv [monster_killed].m_x, menv [monster_killed].m_y, 2 + random2(4));
goto out_of_switch;
} /* end fire vortex */

if (menv [monster_killed].m_class == 144)
{
strcpy(info, monam (menv[monster_killed].m_sec,menv[monster_killed].m_class, menv [monster_killed].m_ench [2], 0));
strcat(info, " falls from the air.");
if (mons_near(monster_killed) == 1) mpr(info);
if (killer == 1 | killer == 3)
{
 gain_exp(exper_value(menv [monster_killed].m_class, menv [monster_killed].m_HD, menv [monster_killed].m_hp_max));
}
goto out_of_switch;
} /* end dancing weapon */

switch(killer)
{
case 1: /* You kill in combat. */
case 3: /* You kill by missile or beam. */
strcpy(info, "You kill ");
if (wounded_damaged(menv[monster_killed].m_class) == 1)
 strcpy(info, "You destroy ");
strcat(info, monam (menv[monster_killed].m_sec,menv[monster_killed].m_class, menv [monster_killed].m_ench [2], 1));  //gmon_name [menv [monster_killed].m_class]);
strcat(info, "!");
mpr(info);
gain_exp(exper_value(menv [monster_killed].m_class, menv [monster_killed].m_HD, menv [monster_killed].m_hp_max));
if (you[0].religion == 5 && random2(70) <= 10 + menv [monster_killed].m_HD) Xom_acts(1, random2(menv [monster_killed].m_HD) + 1, 0);
if (you[0].duration [3] > 0)
{
 if (mons_holiness(menv [monster_killed].m_class) == 0)
  done_good(1, menv [monster_killed].m_HD);
 if (mons_holiness(menv [monster_killed].m_class) == 1)
  done_good(2, menv [monster_killed].m_HD);
 if (mons_holiness(menv [monster_killed].m_class) == 2)
  done_good(3, menv [monster_killed].m_HD);
 if (mons_holiness(menv [monster_killed].m_class) == -1)
  done_good(5, menv [monster_killed].m_HD);
} else if (mons_holiness(menv [monster_killed].m_class) == -1)
          done_good(4, menv [monster_killed].m_HD);

if (you[0].religion == 8 && you[0].duration [3] != 0 && random2(you[0].piety) >= 20)
{
 if (you[0].hp < you[0].hp_max)
 {
  mpr("You feel a little better.");
  you[0].hp += menv [monster_killed].m_HD + random2(menv [monster_killed].m_HD);
  if (you[0].hp > you[0].hp_max) you[0].hp = you[0].hp_max;
  you[0].hp_ch = 1;
 }
 if (you[0].ep < you[0].ep_max)
 {
  mpr("You feel your power returning.");
  you[0].ep += random2(menv [monster_killed].m_HD) + 1;
  if (you[0].ep > you[0].ep_max) you[0].ep = you[0].ep_max;
  you[0].ep_ch = 1;
 }
}

if (you[0].duration [19] != 0 && mons_holiness(menv [monster_killed].m_class) == 0)
{
 if (create_monster(367, 0, 7, menv [monster_killed].m_x, menv [monster_killed].m_y, you[0].pet_target, mons_charclass(menv [monster_killed].m_class)) != -1)
  mpr("A strange glowing mist starts to gather...");
}

break;

case 2: /* Monster kills in combat */
case 4: /* Monster kills by missile or beam */
if (mons_near(monster_killed) == 1)
{
        strcpy(info, monam (menv[monster_killed].m_sec,menv[monster_killed].m_class, menv [monster_killed].m_ench [2], 0)); //gmon_name [menv [monster_killed].m_class]);
        strcat(info, " dies!");
        mpr(info);
}

if (menv [monster_killed].m_beh == 7) naughty(6, (menv [monster_killed].m_HD / 2) + 1);

if ((i >= 0 && i < 200) && menv [i].m_beh == 7)
{
 gain_exp(exper_value(menv [monster_killed].m_class, menv [monster_killed].m_HD, menv [monster_killed].m_hp_max) / 2 + 1);
 if (mons_holiness(menv [i].m_class) == 1)
 {
  if (mons_holiness(menv [monster_killed].m_class) == 0)
   done_good(9, menv [monster_killed].m_HD);
 }

}
break;

/*
case 3: You kill by missile or beam
strcpy(info, "The ");
strcat(info, beam_name);
if (wounded_damaged(menv[monster_killed].m_class) == 0) strcat(info, " kills ");
 else strcat(info, " destroys ");
strcat(info, monam (menv[monster_killed].m_sec,menv[monster_killed].m_class, menv [monster_killed].m_ench [2], 1)); // gmon_name [menv [monster_killed].m_class]);
strcat(info, "!");
mpr(info);
gain_exp(exper_value(menv [monster_killed].m_class, menv [monster_killed].m_HD, menv [monster_killed].m_hp_max));
break;
*/

/*
case 4: Monster kills by missile or beam
strcpy(info, "The ");
strcat(info, beam_name);
strcat(info, " kills ");
strcat(info, monam (menv[monster_killed].m_sec,menv[monster_killed].m_class, menv [monster_killed].m_ench [2], 1)); //gmon_name [menv [monster_killed].m_class]);
strcat(info, "!");
mpr(info);
if (menv [i].m_beh == 7)
{
 gain_exp(exper_value(menv [monster_killed].m_class, menv [monster_killed].m_HD, menv [monster_killed].m_hp_max) / 2 + 1);
}
break;
*/

case 5: /* Monster killed by trap/inanimate thing/itself/poison not from you */
if (mons_near(monster_killed))
{
strcpy(info, monam (menv[monster_killed].m_sec,menv[monster_killed].m_class, menv [monster_killed].m_ench [2], 0)); //gmon_name [mons_class [i]]);
strcat(info, " dies!");
mpr(info);
}
break;

case 6: /* Monster doesn't die, just goes back to wherever it came from
 This must only be called by monsters running out of time (or abjuration),
 because it uses the beam variables! Or does it??? */
if (mons_near(monster_killed))
{
 strcpy(info, monam (menv[monster_killed].m_sec,menv[monster_killed].m_class, menv [monster_killed].m_ench [2], 0)); //gmon_name [mons_class [i]]);
 strcat(info, " disappears in a puff of smoke!");
 mpr(info);
 place_cloud(random2(3) + 105, menv [monster_killed].m_x, menv [monster_killed].m_y, 1 + random2(3));
}
for (dmi = 7; dmi >= 0; dmi --) /* takes whatever it's carrying back home */
        {
         if (menv [monster_killed].m_inv [dmi] != 501)
         {
           destroy_item(menv [monster_killed].m_inv [dmi]);
         }
         menv [monster_killed].m_inv [dmi] = 501;
        }
break;

} /* end of switch */


out_of_switch : if (menv [monster_killed].m_class == 38) /* mummy! */
{
  if (killer == 1 | killer == 3)
  {
    if (curse_an_item(1, 0) == 1)
    {
      strcpy(info, "You feel nervous for a moment...");
      mpr(info);
    }
  }
}

if (menv [monster_killed].m_class == 373 | menv [monster_killed].m_class == 374 | menv [monster_killed].m_class == 375) /* other mummies */
{
  if (killer == 1 | killer == 3)
  {
      mpr("You feel extremely nervous for a moment...");
      miscast_effect(16, 3 + (menv [monster_killed].m_class == 374) * 8 + (menv [monster_killed].m_class == 375) * 5, random2(30) + random2(30) + random2(30), 100);
  }
}

if (menv [monster_killed].m_class == 56)
{
 dmi = monster_killed;
 while(menv [dmi].m_class != 19)
        {
          dmi--;
          if ((menv [dmi].m_class != 56 && menv [dmi].m_class != 19) | dmi < 0)
          {
           strcpy(info, "Error: Worm head not found!");
           mpr(info);
           strcpy(info, "Oh well. Carry on.");
           mpr(info);
           goto out_of_worm;
          }
        }

 if (menv [dmi].m_HD >= 3)
 {
                menv [dmi].m_HD -= 2;
  menv [dmi].m_hp_max -= 8 + random2(8);
  menv [dmi].m_hp -= random2(8) + 8;
         if (menv [dmi].m_hp_max <= 0) menv [dmi].m_hp_max = 1;
  if (menv [dmi].m_hp > menv [dmi].m_hp_max) menv [dmi].m_hp = menv [dmi].m_hp_max;
  if (menv [dmi].m_hp <= 0) menv [dmi].m_hp = 1;
 }
}


out_of_worm : if (menv [monster_killed].m_class == 19 | menv [monster_killed].m_class == 56)
{
 dmi = monster_killed;

        while(menv [dmi + 1].m_class == 56)
        {
  dmi ++;
  menv [dmi].m_class = -1;
  mgrd [menv [dmi].m_x] [menv [dmi].m_y] = MNG;
        }

}


  if (killer != 6) place_monster_corpse(monster_killed);

  monster_drop_ething(monster_killed);

  int j;

  for (j = 0; j < 3; j++)
        {
                menv [monster_killed].m_ench [j] = 0;
        }

        menv [monster_killed].m_ench_1 = 0;

/*no_mons --;*/
menv [monster_killed].m_class = -1;
menv [monster_killed].m_hp = 0;
menv [monster_killed].m_hp_max = 0;
menv [monster_killed].m_HD = 0;
menv [monster_killed].m_AC = 0;
menv [monster_killed].m_ev = 0;

mgrd [menv [monster_killed].m_x] [menv [monster_killed].m_y] = MNG;

for (dmi = 7; dmi >= 0; dmi --)
{
 menv [monster_killed].m_inv [dmi] = 501;
}


/*brek = 1;*/
viewwindow(1);

for (dmi = 0; dmi < MNST; dmi ++)
{
        if (menv [dmi].m_hit == monster_killed) menv [dmi].m_hit = MHITNOT;
}

if (you[0].pet_target == monster_killed) you[0].pet_target = MHITNOT;

return;
}



int jelly_divide(int jel)
{

int k = 0;

if (menv [jel].m_class != 35) return 0;

         if (menv [jel].m_hp == 1) return 0;

         int jex = 0;
         int jey = 0;

         for (jex = -1; jex < 3; jex ++)
         {

            if (jex == 2) return 0;

            for (jey = -1; jey < 2; jey ++)
            {
               if (mgrd [menv [jel].m_x + jex] [menv [jel].m_y + jey] == MNG && mgrd [menv [jel].m_x + jex] [menv [jel].m_y + jey] > 10 && (menv [jel].m_x + jex != you[0].x_pos | menv [jel].m_y + jey != you[0].y_pos))
               /* 10-50 for now - must take clouds into account */
               goto jel2;
            } /* end of for jey */

         } /* end of for jex */

         jel2: for (k = 0; k < MNST; k++)
         {
               if (k == MNST - 1) return 0;

               if (menv [k].m_class == -1) break;
         }

         menv [jel].m_hp_max /= 2;
         if (menv [jel].m_hp > menv [jel].m_hp_max) menv [jel].m_hp = menv [jel].m_hp_max;

menv [k].m_class = 35;
menv [k].m_HD = 4;
menv [k].m_hp = menv [jel].m_hp;
menv [k].m_hp_max = menv [k].m_hp;
menv [k].m_AC = 0;
menv [k].m_ev = 2;
menv [k].m_speed = 5;
menv [k].m_speed_inc = 70 + random2(5);

menv [k].m_x = menv [jel].m_x + jex;
menv [k].m_y = menv [jel].m_y + jey;

mgrd [menv [k].m_x] [menv [k].m_y] = k;

/*         no_mons ++;
         mn_no ++;*/

         menv [k].m_beh = menv [jel].m_beh; /* Look at this! */

         if (mons_near(jel) == 1)
         {
            strcpy(info, "The jelly splits in two!");
            mpr(info);
         } else
           {
               strcpy(info, "You hear a squelching noise in the distance.");
               mpr(info);
           }

         return 1;

}



void alert(void)
{
/* if you're invis and throw/zap whatever, alerts menv to your position */

int it;

for (it = 0; it < MNST; it++)
{

if (menv [it].m_class == -1) continue;

if (mons_near(it) == 1)
{
   if (menv [it].m_beh == 1 | menv [it].m_beh == 2 | menv [it].m_beh == 0)
   {
      menv [it].m_beh = 1;
      if (you[0].invis == 0 | mons_see_invis(menv [it].m_class) == 1)
      {
       menv [it].m_targ_1_x = you[0].x_pos;
       menv [it].m_targ_1_y = you[0].y_pos;
      }
   }
}

} /* end of for it */

} /* end of void alert() */




void monster_polymorph(unsigned char monsc, unsigned char targetc, int power)
{

int k = power;
k = 0; /* to avoid that annoying warning */

if (targetc == 250)
{
   do
   {
      targetc = random2(52); /* I think. Shouldn't poly into eg orc _wizard_ */
   } while (targetc == 31 | targetc == 25 | targetc == 51); /* no fiends or zombies */
}

/* if (power != -1) // automatic success */

unsigned char old_class = menv [monsc].m_class;
unsigned char old_hp = menv [monsc].m_hp;
unsigned char old_hp_max = menv [monsc].m_hp_max;
unsigned char old_sec = menv [monsc].m_sec;

/* deal with mons_sec */


menv [monsc].m_class = targetc;


menv [monsc].m_sec = 250;

k = monsc;

define_monster(monsc, menv);

menv [monsc].m_hp = menv [monsc].m_hp_max * ((old_hp * 100) / old_hp_max) / 100;
menv [monsc].m_speed_inc = 70 + random2(5);

/* make sure that eg manticores don't drop their darts */

monster_drop_ething(monsc);

int unenc = 0;

for (unenc = 0; unenc < 3; unenc++)
{
   if (menv [monsc].m_ench [unenc] >= 20 && menv [monsc].m_ench [unenc] <= 25)
    continue; /* Summoned creatures are still going to disappear */
   menv [monsc].m_ench [unenc] = 0;
}

menv [monsc].m_ench_1 = 0;
/* need to tweak this for you[0].invis */

strcpy(info, monam (old_sec, old_class, menv [monsc].m_ench [2], 0));
if (targetc == 131) strcat(info, " degenerates into ");
 else strcat(info, " evaporates, and reforms as ");

strcat(info, monam (menv [monsc].m_sec, menv [monsc].m_class, menv [monsc].m_ench [2], 3));
if (targetc == 131) strcat(info, " of flesh");
strcat(info, "!");
mpr(info);

} /* end of monster_polymorph */


void monster_drop_ething(int monster_killed)
{
/* use monster_killed for convenience
   drop weapons & missiles last (ie on top) so others pick up */

int ygy;
char splashes = 0;

if (grd [menv [monster_killed].m_x] [menv [monster_killed].m_y] == 61 | grd [menv [monster_killed].m_x] [menv [monster_killed].m_y] == 62)
{
 for (ygy = 0; ygy < 8; ygy ++)
 {
  if (menv [monster_killed].m_inv [ygy] != 501)
  {
   destroy_item(menv [monster_killed].m_inv [ygy]);
   splashes ++;
  }
 }
 if (splashes > 0)
 {
  strcpy(info, "You hear a splashing sound.");
  mpr(info);
 }
 return;
}

for (ygy = 7; ygy >= 0; ygy --)
        {
        if (menv [monster_killed].m_inv [ygy] != 501)
        {
  mitm.ilink [menv [monster_killed].m_inv [ygy]] = 501;
                if (igrd [menv [monster_killed].m_x] [menv [monster_killed].m_y] == 501)
                /* this bit is wrong, BTW:
                if (mons_inv [monster_killed] [j] != 501) */
                {
                        igrd [menv [monster_killed].m_x] [menv [monster_killed].m_y] = menv [monster_killed].m_inv [ygy];
                } else
                {
                        int huj = igrd [menv [monster_killed].m_x] [menv [monster_killed].m_y];
                        igrd [menv [monster_killed].m_x] [menv [monster_killed].m_y] = menv [monster_killed].m_inv [ygy];
                        mitm.ilink [menv [monster_killed].m_inv [ygy]] = huj;
                }
        }


        menv [monster_killed].m_inv [ygy] = 501;
        }



} /* end of void monster_drop_ething */



void place_monster_corpse(unsigned char mcr)
{
int corpse_class = mons_charclass(menv [mcr].m_class);


 if (mons_weight(corpse_class) == 0 | random2(2) == 0) return;
 if (grd [menv [mcr].m_x] [menv [mcr].m_y] == 61 | grd [menv [mcr].m_x] [menv [mcr].m_y] == 62) return;

        int o;

        for (o = 0; o < ITEMS; o++)
        {
  if (o >= 200 + random2(150)) return;
                if (mitm.iquant [o] == 0)
                {
                        mitm.iid [o] = 0;
                        mitm.iclass [o] = 14;
                        mitm.iplus [o] = corpse_class;
                        mitm.iplus2 [o] = 0;
                        mitm.itype [o] = 0;
                        mitm.idam [o] = 210;
                        mitm.icol [o] = mcolour [corpse_class];
                        if (mcolour [corpse_class] == BLACK) mitm.icol [o] = menv [mcr].m_sec;
                        mitm.iquant [o] = 1;
                        mitm.ilink [o] = 501;
                        break;
                }
        } // end of o loop

        if (igrd [menv [mcr].m_x] [menv [mcr].m_y] == 501)
        {
                igrd [menv [mcr].m_x] [menv [mcr].m_y] = o;
        } else
        {
                int hug = igrd [menv [mcr].m_x] [menv [mcr].m_y];
                igrd [menv [mcr].m_x] [menv [mcr].m_y] = o;
                mitm.ilink [o] = hug;
        }

return;

}

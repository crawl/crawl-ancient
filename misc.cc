#include "config.h"

#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#ifdef DOS
#include <conio.h>
#endif

#include "misc.h"

#include "direct.h"
#include "externs.h"
#include "files.h"
#include "fight.h"
#include "items.h"
#include "it_use2.h"
#include "levels.h"
#include "macro.h"
#include "mstruct.h"
#include "misc.h"
#include "monplace.h"
#include "ouch.h"
#include "output.h"
#include "player.h"
#include "shopping.h"
#include "skills.h"
#include "spells1.h"
#include "skills2.h"
#include "spells3.h"
#include "stuff.h"
#include "view.h"

char scramble(void);
int trap_item(char iclass, char itype, char beam_x, char beam_y);


void destroy_item(int dest)
{

int c = 0;
int cy = 0;

//if (mitm.iquant [dest] <= 0) return;

if (dest == 501) return;

for (c = 0; c < MNST; c++)
{
 if (menv [c].m_class == -1) continue;
 for (cy = 0; cy < 8; cy++)
 {
  if (menv [c].m_inv [cy] == dest) //menv [c].m_inv [cy] = 501;
  {
   menv [c].m_inv [cy] = 501;
   mitm.iquant [dest] = 0;
   mitm.ilink [dest] = 501;
   if (menv [c].m_class == 144)
   {
    monster_die(c, 6, 0);
   }
   return;
  }

 }
}

unsigned int udest = dest;

for (c = 0; c < ITEMS; c++)
{
  if (mitm.ilink [c] == udest)
  {
   mitm.ilink [c] = mitm.ilink [dest];
   mitm.iquant [dest] = 0;
   mitm.ilink [dest] = 501;
   return;
  }
}

// Okay, so that didn't work. Now we go for the slower way:

for (c = 2; c < 79; c ++)
{
 for (cy = 2; cy < 69; cy ++)
 {
  if (igrd [c] [cy] == dest)
  {
   igrd [c] [cy] = mitm.ilink [dest];
   mitm.iquant [dest] = 0;
   mitm.ilink [dest] = 501;
   return;
  }
 }
}

//strcpy(info, "Warning: Invalid item destroyed!");
//mpr(info);
//more();

} // end void destroy_item(int dest)


void place_chunks(int mcls, unsigned char rot_status, unsigned char chx, unsigned char chy, unsigned char ch_col)
{

        int o;

        for (o = 0; o < ITEMS; o++)
        {
  if (o >= 200 + random2(150)) return;
                if (mitm.iquant [o] == 0)
                {
                        mitm.iid [o] = 0;
                        mitm.iclass [o] = 4;
                        mitm.iplus [o] = mcls;
                        mitm.iplus2 [o] = 0;
                        mitm.itype [o] = 21;
                        mitm.idam [o] = rot_status;
                        if (ch_col == BLACK) ch_col = LIGHTRED;
                        mitm.icol [o] = ch_col;
                        mitm.iquant [o] = 1 + random2(mons_weight(mcls) / 150);
                        mitm.ilink [o] = 501;
                        break;
                }
        } // end of o loop

        if (igrd [chx] [chy] == 501)
        {
                igrd [chx] [chy] = o;
        } else
        {
                int hug = igrd [chx] [chy];
                igrd [chx] [chy] = o;
                mitm.ilink [o] = hug;
        }

/*if (mons_organs(mcls) != 0)
{

// One day I'll add code for various monster organs.

}*/

if ((mcls == 29 || mcls == 45 || mcls == 75 || mcls == 101 || mcls == 122 || mcls == 132 || mcls == 146 || mcls == 171) && random2(3) != 0)
{
// this places skins of various creatures, which are classed as armour and can be enchanted to become special magical armours.
        for (o = 0; o < ITEMS; o++)
        {
          if (o >= 200 + random2(150)) return;
          if (mitm.iquant [o] == 0)
          {
          mitm.iquant [o] = 1;
          mitm.ilink [o] = 501;
          switch(mcls)
          {
                case 29: // dragon scales
                        mitm.iid [o] = 0;
                        mitm.iclass [o] = 2;
                        mitm.iplus [o] = 50;
                        mitm.iplus2 [o] = 0;
                        mitm.itype [o] = 15;
                        mitm.idam [o] = 0;
                        mitm.icol [o] = GREEN;
                break;
                case 45: // troll hide
                        mitm.iid [o] = 0;
                        mitm.iclass [o] = 2;
                        mitm.iplus [o] = 50;
                        mitm.iplus2 [o] = 0;
                        mitm.itype [o] = 16;
                        mitm.idam [o] = 0;
                        mitm.icol [o] = BROWN;
                break;
                case 75: // ice dragon scales
                        mitm.iid [o] = 0;
                        mitm.iclass [o] = 2;
                        mitm.iplus [o] = 50;
                        mitm.iplus2 [o] = 0;
                        mitm.itype [o] = 20;
                        mitm.idam [o] = 0;
                        mitm.icol [o] = WHITE;
                break;
                case 101: // steam dragon scales
                        mitm.iid [o] = 0;
                        mitm.iclass [o] = 2;
                        mitm.iplus [o] = 50;
                        mitm.iplus2 [o] = 0;
                        mitm.itype [o] = 22;
                        mitm.idam [o] = 0;
                        mitm.icol [o] = LIGHTGREY;
                break;
                case 122: // mottled dragon scales
                        mitm.iid [o] = 0;
                        mitm.iclass [o] = 2;
                        mitm.iplus [o] = 50;
                        mitm.iplus2 [o] = 0;
                        mitm.itype [o] = 24;
                        mitm.idam [o] = 0;
                        mitm.icol [o] = LIGHTMAGENTA;
                break;
                case 132: // storm dragon scales
                        mitm.iid [o] = 0;
                        mitm.iclass [o] = 2;
                        mitm.iplus [o] = 50;
                        mitm.iplus2 [o] = 0;
                        mitm.itype [o] = 26;
                        mitm.idam [o] = 0;
                        mitm.icol [o] = LIGHTBLUE;
                break;
                case 146: // golden dragon scales
                        mitm.iid [o] = 0;
                        mitm.iclass [o] = 2;
                        mitm.iplus [o] = 50;
                        mitm.iplus2 [o] = 0;
                        mitm.itype [o] = 28;
                        mitm.idam [o] = 0;
                        mitm.icol [o] = YELLOW;
                break;
                case 171: // swamp dragon scales
                        mitm.iid [o] = 0;
                        mitm.iclass [o] = 2;
                        mitm.iplus [o] = 50;
                        mitm.iplus2 [o] = 0;
                        mitm.itype [o] = 31;
                        mitm.idam [o] = 0;
                        mitm.icol [o] = BROWN;
                break;
          }
          break;
          }
        }
        if (igrd [chx] [chy] == 501)
        {
                igrd [chx] [chy] = o;
        } else
        {
                int hug = igrd [chx] [chy];
                igrd [chx] [chy] = o;
                mitm.ilink [o] = hug;

        }
}

} // end place_chunks



char search_around(void)
{
char srx = 0;
char sry = 0;
int i;

//if (you[0].clas == 3) chance_found = 2;
for (srx = you[0].x_pos - 1; srx < you[0].x_pos + 2; srx ++)
{
 for (sry = you[0].y_pos - 1; sry < you[0].y_pos + 2; sry ++)
 {
  // don't exclude own square; may be levitating
  if (grd [srx] [sry] == 5 && random2(17) <= you[0].skills [18] + 1) //) >= chance_found)
  {
   grd [srx] [sry] = 3;
   strcpy(info, "You found a secret door!");
   mpr(info);
   exercise(18, 1 + random2(2));
  }
  if (grd [srx] [sry] == 78 && random2(17) <= you[0].skills [18] + 1)
  {
                        for (i = 0; i < NTRAPS; i ++)
                        {
                                if (env[0].trap_x [i] == srx && env[0].trap_y [i] == sry) break;
                        }
                        if (env[0].trap_type [i] < 4 || env[0].trap_type [i] == 6) grd [srx] [sry] = 75;
                        if (env[0].trap_type [i] == 4 || env[0].trap_type [i] == 5) grd [srx] [sry] = 76;
   strcpy(info, "You found a trap!");
   mpr(info);
  }
 }
}
you[0].turnover = 1;
return 1;
}


void in_a_cloud(void)
{

int cl = env[0].cgrid [you[0].x_pos] [you[0].y_pos];
int hurted = 0;

switch(env[0].cloud_type [cl] % 100)
{
        case 1:
        strcpy(info, "You are engulfed in roaring flames!");
        mpr(info);
        if (player_res_fire() <= 100)
        {
                //strcat(info, "!");
                //mpr(info);
                hurted += ((random2(5) + random2(5) + random2(5) + 3) * you[0].time_taken) / 10;
    if (player_res_fire() < 100) hurted += ((random2(5) + random2(5) + 3) * you[0].time_taken) / 10;
  hurted -= random2(player_AC());
  if (hurted <= 0) hurted = 0;
                ouch(hurted, cl, 2);
        } else
        {
                strcpy(info, "You resist.");
                mpr(info);
                hurted += ((random2(5) + random2(5) + random2(5) + 3) * you[0].time_taken) / 10;
                hurted /= 2 + (player_res_fire() - 100) * (player_res_fire() - 100);
                ouch(hurted, cl, 2);
        }
       scrolls_burn(7, 6);
        break;

   case 2:
   // If you don't have to breathe, unaffected
   strcpy(info, "You are engulfed in noxious fumes!");
   mpr(info);
   if (player_res_poison() != 0) break;
//   ouch(random2(3) * (float) (you[0].time_taken / 10), cl, 2);
                hurted += (random2(3) * you[0].time_taken) / 10;
//  hurted -= random2(player_AC());
   if (hurted <= 0) hurted = 0;
                ouch((hurted * you[0].time_taken) / 10, cl, 2);
   if (random2(27) + 1 >= you[0].xl)
   {
     mpr("You choke on the stench!");
     you[0].conf += random2(2) + 2;
   }
   break;

        case 3:
        strcpy(info, "You are engulfed in freezing vapours");
        mpr(info);
        if (player_res_cold() <= 100)
        {
                strcat(info, "!");
                mpr(info);
//              ouch((random2(5) + random2(5) + random2(5) + 3) * (float) (you[0].time_taken / 10), cl, 2);
                hurted += ((random2(5) + random2(5) + random2(5) + 3) * you[0].time_taken) / 10;
    if (player_res_cold() < 100) hurted += ((random2(5) + random2(5) + 3) * you[0].time_taken) / 10;
  hurted -= random2(player_AC());
  if (hurted <= 0) hurted = 0;
                ouch((hurted * you[0].time_taken) / 10, cl, 2);
        } else
        {
                strcpy(info, "You resist.");
                mpr(info);
                hurted += ((random2(5) + random2(5) + random2(5) + 3) * you[0].time_taken) / 10;
                hurted /= 2 + (player_res_cold() - 100) * (player_res_cold() - 100);
                ouch(hurted, cl, 2);
        }
 scrolls_burn(7, 8);
        break;

   case 4: // you[0].poison
   // If you don't have to breathe, unaffected
   strcpy(info, "You are engulfed in poison gas!");
   mpr(info);
   if (player_res_poison() != 0) break;
   ouch((random2(3) * you[0].time_taken) / 10, cl, 2);
   you[0].poison ++;
   break;

   case 5:
   case 6:
   case 7:
   case 10:
   mpr("You are engulfed in a cloud of smoke!");
   break;

   case 8:
   strcpy(info, "You are engulfed in a cloud of scalding steam!");
   mpr(info);
   if (you[0].species == 26 && you[0].xl >= 6)
   {
    mpr("It doesn't seem to affect you.");
    return;
   }
   if (you[0].equip [6] != -1 && you[0].inv_type [you[0].equip [6]] == 23)
   {
    mpr("It doesn't seem to affect you.");
    return;
   }
   hurted += (random2(6) * you[0].time_taken) / 10;
   if (hurted <= 0 || player_res_fire() > 100) hurted = 0;
   ouch((hurted * you[0].time_taken) / 10, cl, 2);
   break;

  case 9: // dark miasma
   strcpy(info, "You are engulfed in a dark miasma.");
   mpr(info);
   if (player_prot_life() != 0) return;
/*   beam_colour = 4; */
   if (player_res_poison() <= 0) you[0].poison ++;
   hurted += (random2(5) + random2(5) + random2(4) * you[0].time_taken) / 10; // 3
   if (hurted <= 0) hurted = 0;
   you[0].hp -= hurted;
   ouch(hurted, cl, 2);
   potion_effect(9, 5);
   if (you[0].hp_max > 4)
   {
    you[0].base_hp -= random2(2);
    calc_hp();
   }
  break;


// case 5 // grey smoke - harmless
// case 6 blue smoke - also harmless
// case 7 purple smoke - mostly harmless

}

}






void up_stairs(void)
{

unsigned char stair_find = grd [you[0].x_pos] [you[0].y_pos];
char old_level_where = you[0].where_are_you;

if (stair_find == 80)
{
 shop();
 return;
}


if ((stair_find < 86 || stair_find > 89) && (stair_find < 130 || stair_find > 150))
{
        strcpy(info, "You can't go up here.");
        mpr(info);
        return;
}

if (you[0].burden_state == 5)
{
        strcpy(info, "You are carrying too much to climb upwards.");
 mpr(info);
        you[0].turnover = 1;
        return;
}


if (you[0].your_level == 0)
{
 mpr("Are you sure you want to leave the Dungeon?");
 char kein = get_ch();
 if (kein != 'y' && kein != 'Y')
 {
  mpr("Alright, then stay!");
  return;
 }
}

unsigned char old_level = you[0].your_level;

you[0].your_level--;

int i = 0;

if (you[0].your_level == -1)
{
 strcpy(info, "You have escaped!");
 mpr(info);
 for (i = 0; i < 52; i++)
 {
  if (you[0].inv_quant [i] > 0 && you[0].inv_class [i] == 12) ouch(-9999, 0, 12);
 }

 ouch(-9999, 0, 11);

}

mpr("Entering...");
you[0].prev_targ = MHITNOT;

if (you[0].where_are_you == 3)
{
 mpr("Thank you for visiting Hell. Please come again soon.");
 you[0].where_are_you = 0;
 stair_find = 69;
}

if (you[0].where_are_you > 0 && you[0].where_are_you != 3 && you[0].where_are_you < 10)
{
/* strcpy(info, "You hear a howl of diabolical anger from deep beneath you!");
 mpr(info);
 more();*/
 you[0].where_are_you = 3;
 you[0].your_level = 27;
}
switch(stair_find)
{
 case 130:
 case 131:
 case 132:
 case 134:
 case 137:
 case 138:
 mpr("Welcome back to the Dungeon!");
 you[0].where_are_you = 0;
 break;
 case 133:
 case 139:
 case 142:
 mpr("Welcome back to the Lair of Beasts!");
 you[0].where_are_you = 12;
 break;
 case 135:
 mpr("Welcome back to the Vaults!");
 you[0].where_are_you = 14;
 break;
 case 141:
 case 136:
 mpr("Welcome back to the Crypt!");
 you[0].where_are_you = 15;
 break;
 case 140:
 mpr("Welcome back to the Orcish Mines!");
 you[0].where_are_you = 10;
 break;
}
char stair_taken = stair_find;
char moving_level = 1;
char want_followers = 1;
/*load(stair_taken, moving_level, level_saved, was_a_labyrinth, old_level, want_followers, just_made_new_lev);*/
load(stair_taken, moving_level, 0, old_level, want_followers, 0, old_level_where);
moving_level = 0;

int j = 0;

/*if (you[0].your_level >= 40)
{
 for (i = 0; i < 80; i ++)
 {
        for (j = 0; j < 70; j ++)
        {
           if (grd [i] [j] >= 86 && grd [i] [j] <= 89) grd [i] [j] = 67;
        }
 }
} // end if*/

if (you[0].char_direction == 1)
{
 for (i = 0; i < 80; i ++)
 {
        for (j = 0; j < 70; j ++)
        {
           if (grd [i] [j] == 69) grd [i] [j] = 67;
        }
 }
}

new_level();


if (you[0].lev != 0)
{
 mpr("You float upwards... And bob straight up to the ceiling!");
} else mpr("You climb upwards.");

save_game(0);

int count_x, count_y;

for (count_x = 0; count_x < GXM; count_x ++)
{
        for (count_y = 0; count_y < GYM; count_y ++)
        {
          if (mgrd [count_x] [count_y] != MNG && (menv [mgrd [count_x] [count_y]].m_class == -1 || menv [mgrd [count_x] [count_y]].m_x != count_x || menv [mgrd [count_x] [count_y]].m_y != count_y))
          {
           mgrd [count_x] [count_y] = MNG; /* This is one of the worst things I've ever done */
          }
        }
}



} /* end of void up_stairs(void) */






void down_stairs(char remove_stairs, int old_level)
{

int i;
char old_level_type = you[0].level_type;
char was_a_labyrinth = 0;
unsigned char stair_find = grd [you[0].x_pos] [you[0].y_pos];
//int old_level = you[0].your_level;
char leaving_abyss = 0;
char old_where = you[0].where_are_you;

if ((stair_find < 81 || stair_find > 85) && stair_find != 69 && ((stair_find < 92 || stair_find > 101) && stair_find != 98) && !(stair_find >= 110 && stair_find < 130))
{
        strcpy(info, "You can't go down here!");
 mpr(info);
 return;
}

if (stair_find >= 81 && stair_find <= 85 && you[0].where_are_you == 3)
{
 mpr("A mysterious force prevents you from descending the staircase.");
 return;
} /* down stairs in vestibule are one-way */

if (stair_find == 98)
{
 strcpy(info, "You can't go down here!");
 mpr(info);
 return;
}


if (you[0].lev != 0 && wearing_amulet(42) == 0)
{
        strcpy(info, "You're floating high up above the floor!");
        mpr(info);
        return;
}

if (stair_find == 117)
{
 for (i = 0; i < 52; i ++)
 {
  if (you[0].inv_class [i] == 13 && you[0].inv_type [i] == 14) goto out_of_rune;
 }
 mpr("You need a Rune to enter this staircase.");
 return;
}

out_of_rune : if (you[0].level_type == 3 && grd [you[0].x_pos] [you[0].y_pos] == 101)
{
  was_a_labyrinth = 1;
} else
{
 if (you[0].level_type != 0) was_a_labyrinth = 1;
 you[0].level_type = 0;
}

mpr("Entering...");
you[0].prev_targ = MHITNOT;

if (grd [you[0].x_pos] [you[0].y_pos] == 69)
{
 you[0].where_are_you = 3; /* go to hell! */
 strcpy(info, "Welcome to Hell!");
 mpr(info);
 strcpy(info, "Please enjoy your stay.");
 mpr(info);
 more();
// you[0].your_level = 59;
}

if ((grd [you[0].x_pos] [you[0].y_pos] >= 92 && grd [you[0].x_pos] [you[0].y_pos] <= 95) || (grd [you[0].x_pos] [you[0].y_pos] >= 110 && grd [you[0].x_pos] [you[0].y_pos] <= 150))
{
 switch(grd [you[0].x_pos] [you[0].y_pos])
 {
  case 92: strcpy(info, "Welcome to the Iron City of Dis!");
  you[0].where_are_you = 1;
  you[0].your_level = 26;
  break;
  case 93: strcpy(info, "Welcome to Gehenna!");
  you[0].where_are_you = 2;
  you[0].your_level = 26;
  break;
  case 94: strcpy(info, "Welcome to Cocytus!");
  you[0].where_are_you = 4;
  you[0].your_level = 26;
  break;
  case 95: strcpy(info, "Welcome to Tartarus!");
  you[0].where_are_you = 5;
  you[0].your_level = 26;
  break;
  case 110: strcpy(info, "Welcome to the Orcish Mines!");
  you[0].where_are_you = 10;
  break;
  case 111: strcpy(info, "You hear a buzzing sound coming from all directions.");
  you[0].where_are_you = 11;
  break;
  case 112: strcpy(info, "Welcome to the Lair of Beasts!");
  you[0].where_are_you = 12;
  break;
  case 113: strcpy(info, "Welcome to the Pits of Slime!");
  you[0].where_are_you = 13;
  break;
  case 114: strcpy(info, "Welcome to the Vaults!");
  you[0].where_are_you = 14;
  break;
  case 115: strcpy(info, "Welcome to the Crypt!");
  you[0].where_are_you = 15;
  break;
  case 116: strcpy(info, "Welcome to the Hall of Blades!");
  you[0].where_are_you = 16;
  break;
  case 117: strcpy(info, "Welcome to the Hall of Zot!");
  you[0].where_are_you = 17;
  break;
  case 118: strcpy(info, "Welcome to the Ecumenical Temple!");
  you[0].where_are_you = 18;
  break;
  case 119: strcpy(info, "Welcome to the Snake Pit!");
  you[0].where_are_you = 19;
  break;
  case 120: strcpy(info, "Welcome to the Elven Halls!");
  you[0].where_are_you = 20;
  break;
  case 121: strcpy(info, "Welcome to the Tomb!");
  you[0].where_are_you = 21;
  break;
  case 122: strcpy(info, "Welcome to the Swamp!");
  you[0].where_are_you = 22;
  break;
 }
 mpr(info);
}

if (grd [you[0].x_pos] [you[0].y_pos] == 81)
{
 you[0].level_type = 1;
 grd [you[0].x_pos] [you[0].y_pos] = 67;
}

if (grd [you[0].x_pos] [you[0].y_pos] == 96)
{
 you[0].level_type = 2;
}

if (grd [you[0].x_pos] [you[0].y_pos] == 99)
{
 you[0].level_type = 3;
}

if (you[0].level_type == 1 || you[0].level_type == 2 || you[0].level_type == 3)
{

char glorpstr [40];
char del_file [40];
int sysg;
strncpy(glorpstr, you[0].your_name, 6);

// glorpstr [strlen(glorpstr)] = 0;
// This is broken. Length is not valid yet! We have to check if we got a
// trailing NULL; if not, write one:
if (strlen(you[0].your_name) > 5)    /* is name 6 chars or more? */
        glorpstr[6] = (char) 0; // NULL;   /* if so, char 7 should be NULL */

strcpy(del_file, glorpstr);
strcat(del_file, ".lab");
#ifdef DOS
strupr(del_file);
#endif
sysg = unlink(del_file);

#ifdef DEBUG
strcpy(info, "Deleting: ");
strcat(info, del_file);
mpr(info);
more();
#endif

}

if (grd [you[0].x_pos] [you[0].y_pos] == 97 || grd [you[0].x_pos] [you[0].y_pos] == 100)
{
 leaving_abyss = 1; /* or pan */
 you[0].your_level --;
 strcpy(info, "You pass through the gate, and find yourself at the top of a staircase.");
 mpr(info);
 more();
}


if (you[0].level_type == 0) you[0].your_level++;

int stair_taken = stair_find;
//unsigned char save_old = 1;
unsigned char moving_level = 1;
unsigned char want_followers = 1;
if (you[0].level_type == 1 || you[0].level_type == 2) stair_find = 81;
if (you[0].level_type == 3) stair_find = 101;

if (remove_stairs == 1)
{
 grd [you[0].x_pos] [you[0].y_pos] = 67;
}

/*load(you[0].your_level);*/
load(stair_taken, moving_level, was_a_labyrinth, old_level, want_followers, 0, old_where);

moving_level = 0;




/*if (you[0].your_level >= 40)
{
 for (i = 0; i < 80; i ++)
 {
        for (j = 0; j < 70; j ++)
        {
           if (grd [i] [j] >= 86 && grd [i] [j] <= 89) grd [i] [j] = 67;
        }
 }
} // end if

if (you[0].your_level == 65 || you[0].your_level == 84 || you[0].your_level == 79 || you[0].your_level == 89 || you[0].your_level == 94)
{
 for (i = 0; i < 80; i ++)
 {
        for (j = 0; j < 70; j ++)
        {
           if (grd [i] [j] >= 82 && grd [i] [j] <= 89) grd [i] [j] = 67;
        }
 }
} // end if*/

unsigned char pc = 0;
unsigned char pt = random2(10) + random2(10) + random2(10);

if (you[0].level_type == 1)
{
 strcpy(info, "You enter a dark and forbidding labyrinth.");
} else
if (you[0].level_type == 2)
{
 strcpy(info, "You enter the Abyss!");
 mpr(info);
 strcpy(info, "To return, you must find a gate leading back.");
 grd [you[0].x_pos] [you[0].y_pos] = 67;
 you[0].your_level --;
 init_pandemonium(); /* colours only */
} else
if (you[0].level_type == 3)
{
if (old_level_type == 3)
{
 strcpy(info, "You pass into a different region of Pandemonium.");
 init_pandemonium();
 for (pc = 0; pc < pt; pc ++)
  pandemonium_mons();
} else
{
 strcpy(info, "You enter the halls of Pandemonium!");
 mpr(info);
 strcpy(info, "To return, you must find a gate leading back.");
 init_pandemonium();
 for (pc = 0; pc < pt; pc ++)
  pandemonium_mons();
}
} else
strcpy(info, "You climb downwards.");
mpr(info);

new_level();

you[0].turnover = 1;

save_game(0);

int count_x, count_y;

for (count_x = 0; count_x < GXM; count_x ++)
{
        for (count_y = 0; count_y < GYM; count_y ++)
        {
          if (mgrd [count_x] [count_y] != MNG && (menv [mgrd [count_x] [count_y]].m_class == -1 || menv [mgrd [count_x] [count_y]].m_x != count_x || menv [mgrd [count_x] [count_y]].m_y != count_y))
          {
           mgrd [count_x] [count_y] = MNG; /* This is one of the worst things I've ever done */
          }
        }
}

} // end of void down_stairs(void)


void new_level(void)
{

 char temp_quant [10];
 textcolor(7);
 itoa(you[0].your_level + 1, temp_quant, 10);
 if (you[0].where_are_you >= 1 && you[0].where_are_you <= 9)
    itoa(you[0].your_level - 26, temp_quant, 10);
 switch(you[0].where_are_you)
 {
/* Remember, must add this to the death_string in ouch */
  case 10: itoa(you[0].your_level - you[0].branch_stairs [0], temp_quant, 10); break;
  case 11: itoa(you[0].your_level - you[0].branch_stairs [1], temp_quant, 10); break;
  case 12: itoa(you[0].your_level - you[0].branch_stairs [2], temp_quant, 10); break;
  case 13: itoa(you[0].your_level - you[0].branch_stairs [3], temp_quant, 10); break;
  case 14: itoa(you[0].your_level - you[0].branch_stairs [4], temp_quant, 10); break;
  case 15: itoa(you[0].your_level - you[0].branch_stairs [5], temp_quant, 10); break;
  case 16: itoa(you[0].your_level - you[0].branch_stairs [6], temp_quant, 10); break;
  case 17: itoa(you[0].your_level - you[0].branch_stairs [7], temp_quant, 10); break;
  case 18: itoa(you[0].your_level - you[0].branch_stairs [8], temp_quant, 10); break;
  case 19: itoa(you[0].your_level - you[0].branch_stairs [9], temp_quant, 10); break;
  case 20: itoa(you[0].your_level - you[0].branch_stairs [10], temp_quant, 10); break;
  case 21: itoa(you[0].your_level - you[0].branch_stairs [11], temp_quant, 10); break;
  case 22: itoa(you[0].your_level - you[0].branch_stairs [12], temp_quant, 10); break;
 }
 gotoxy (46,12);
 env[0].floor_colour = LIGHTGREY;
 env[0].rock_colour = BROWN;
 if (you[0].level_type == 3)
 {
  cprintf("- Pandemonium            ");
  unsigned char pcol = 0;
  pcol = mcolour [env[0].mons_alloc [9]];
  if (pcol == 0) pcol = LIGHTGREY;
  env[0].floor_colour = pcol;
  pcol = mcolour [env[0].mons_alloc [8]];
  if (pcol == 0) pcol = LIGHTGREY;
  env[0].rock_colour = pcol;
 }
 else if (you[0].level_type == 2)
 {
  cprintf("- The Abyss               ");
  unsigned char pcol = 0;
  pcol = mcolour [env[0].mons_alloc [9]];
  if (pcol == 0) pcol = LIGHTGREY;
  env[0].floor_colour = pcol;
  pcol = mcolour [env[0].mons_alloc [8]];
  if (pcol == 0) pcol = LIGHTGREY;
  env[0].rock_colour = pcol;
 }
 else if (you[0].level_type == 1)
 {
  cprintf("- a Labyrinth           ");
 }
 else
 {
 if (you[0].where_are_you != 3) cprintf(temp_quant);

 switch(you[0].where_are_you)
 {
 case 0:
 cprintf(" of the Dungeon         ");
 break;
 case 1:
  env[0].floor_colour = CYAN;
  env[0].rock_colour = CYAN;
  cprintf(" of Dis              ");
 break;
 case 2:
   env[0].floor_colour = DARKGREY;
   env[0].rock_colour = RED;
   cprintf(" of Gehenna           ");
 break;
 case 3:
   env[0].floor_colour = LIGHTGREY;
   env[0].rock_colour = LIGHTGREY;
   cprintf("the Vestibule of Hell          ");
 break;
 case 4:
   env[0].floor_colour = LIGHTBLUE;
   env[0].rock_colour = LIGHTCYAN;
   cprintf(" of Cocytus                  ");
 break;
 case 5:
   env[0].floor_colour = DARKGREY;
   env[0].rock_colour = DARKGREY;
   cprintf(" of Tartarus              ");
 break;
 case 6:
   env[0].floor_colour = LIGHTRED;
   env[0].rock_colour = RED;
   cprintf(" of the Inferno               ");
 break;
 case 7:
   env[0].floor_colour = RED;
   env[0].rock_colour = DARKGREY;
   cprintf(" of the Pit              ");
 break;
 case 10:
   env[0].floor_colour = BROWN;
   env[0].rock_colour = BROWN;
   cprintf(" of the Orcish Mines          ");
 break;
 case 11:
   env[0].floor_colour = YELLOW;
   env[0].rock_colour = BROWN;
   cprintf(" of the Hive                  ");
 break;
 case 12:
   env[0].floor_colour = GREEN;
   env[0].rock_colour = BROWN;
   cprintf(" of the Lair                  ");
 break;
 case 13:
   env[0].floor_colour = GREEN;
   env[0].rock_colour = LIGHTGREEN;
   cprintf(" of the Slime Pits            ");
 break;
 case 14:
   env[0].floor_colour = LIGHTGREY;
   env[0].rock_colour = BROWN;
   cprintf(" of the Vaults                ");
 break;
 case 15:
   env[0].floor_colour = LIGHTGREY;
   env[0].rock_colour = LIGHTGREY;
   cprintf(" of the Crypt                 ");
 break;
 case 16:
   env[0].floor_colour = LIGHTGREY;
   env[0].rock_colour = LIGHTGREY;
   cprintf(" of the Hall of Blades        ");
 break;
 case 17:
 if (you[0].your_level - you[0].branch_stairs [7] <= 1)
 {
   env[0].floor_colour = LIGHTGREY;
   env[0].rock_colour = LIGHTGREY;
 } else
     switch(you[0].your_level - you[0].branch_stairs [7])
     {
      case 2: env[0].rock_colour = LIGHTGREY;
              env[0].floor_colour = BLUE; break;
      case 3: env[0].rock_colour = BLUE;
              env[0].floor_colour = LIGHTBLUE; break;
      case 4: env[0].rock_colour = LIGHTBLUE;
              env[0].floor_colour = MAGENTA; break;
      case 5: env[0].rock_colour = MAGENTA;
              env[0].floor_colour = LIGHTMAGENTA; break;
     }
   cprintf(" of the Realm of Zot          ");
 break;
 case 18:
   env[0].floor_colour = LIGHTGREY;
   env[0].rock_colour = LIGHTGREY;
   cprintf(" of the Temple                ");
 break;
 case 19:
   env[0].floor_colour = LIGHTGREEN;
   env[0].rock_colour = YELLOW;
   cprintf(" of the Snake Pit             ");
 break;
 case 20:
   env[0].floor_colour = DARKGREY;
   env[0].rock_colour = LIGHTGREY;
   cprintf(" of the Elven Halls           ");
 break;
 case 21:
   env[0].floor_colour = YELLOW;
   env[0].rock_colour = LIGHTGREY;
   cprintf(" of the Tomb                  ");
 break;
 case 22:
   env[0].floor_colour = BROWN;
   env[0].rock_colour = BROWN;
   cprintf(" of the Swamp                 ");
 break;
 }
 } // end else
}



void dart_trap(int trap_known, int trapped, struct bolt beam [1])
{

int damage_taken = 0;


                        if (random2(10) < 2 || (trap_known == 1 && random2(4) != 0))
                        {
                                strcpy(info, "You avoid triggering a");
                                strcat(info, beam[0].beam_name);
                                strcat(info, " trap.");
                                mpr(info);
                                return;
                        }
                        if (you[0].equip [5] != -1) exercise(17, (random2(3)) / 2);
                        strcpy(info, "A");
                        strcat(info, beam[0].beam_name);
                        strcat(info, " shoots out and ");

   if (random2(50) < player_shield_class())
   {
    strcat(info, "hits your shield.");
    mpr(info);
    goto out_of_trap;
   }

                        if ((20 + you[0].your_level * 2) * random2(200) / 100 >= player_evasion() + random2(you[0].dex) / 3 - 2)
                        {
                                damage_taken = random2(beam[0].damage);
                                damage_taken -= random2 (player_AC() + 1);
                                strcat(info, "hits you!");
                                mpr(info);
                                if (damage_taken > 0) ouch(damage_taken, 0, 10);
                        } else
                        {
                                strcat(info, "misses you.");
                                mpr(info);
                        }

                        if ((you[0].equip [6] == -1 || you[0].inv_type [you[0].equip [6]] < 2 || (you[0].inv_type [you[0].equip [6]] >= 22 && you[0].inv_type [you[0].equip [6]] <= 25) || you[0].inv_dam [you[0].equip [6]] / 30 == 4) && random2(2) == 0) /* && move_x != 0 || move_y != 0) */
                         exercise(14, 1);

                        out_of_trap : beam[0].bx = you[0].x_pos;
                        beam[0].by = you[0].y_pos;

                        if (random2(2) != 0) itrap(beam, trapped);


                        if (random2(10) + damage_taken < 7)
                        {

                } // end of if grd is a trap
                return; // This is to avoid going through the you_attack bit.
        }






void itrap(struct bolt beam [1], int trapped)
{

/*                      if (random2(10) > 6 || it_no > 300) return;*/

                        switch(env[0].trap_type [trapped])
                        {
                                case 0: //dart
                                beam[0].colour = 1; // class
                                beam[0].damage = 3; // type
                                break;

                                case 1: // arrow
                                beam[0].colour = 1;
                                beam[0].damage = 1;
                                break;

                                case 2: //spear
                                beam[0].colour = 0;
                                beam[0].damage = 11;
                                break;

                                case 3: // (hand) axe
                                beam[0].colour = 0;
                                beam[0].damage = 9;
                                break;

                                default:
                                getch();
                                break;

                        }

//it_no +=
trap_item(beam[0].colour, beam[0].damage, beam[0].bx, beam[0].by);


}




void cull_items(void)
{

int cull = 0;
int destr = 0;
int county = 0;

for (cull = 0; cull < ITEMS; cull ++)
{
 if (mitm.iquant [cull] != 0) county ++;
}

if (county < 350) return;
destr = 3;
if (county < 450) destr = 2;
if (county < 400) destr = 1;
strcpy(info, "The floor is straining under the weight of all the items on this level!");
mpr(info);
more();
strcpy(info, "The dungeon's self-correcting mechanism removes a few of them.");
mpr(info);

for (cull = 0; cull < ITEMS; cull ++)
{
 if (mitm.iquant [cull] <= 0) continue;
 if (mitm.iclass [cull] == 14) destroy_item(cull);
 if (mitm.iclass [cull] == 1 && mitm.iquant [cull] < 3) destroy_item(cull);
 if (mitm.iclass [cull] == 4 && mitm.itype [cull] == 21) destroy_item(cull);
 if (mitm.iclass [cull] == 3 && mitm.iplus [cull] == 0) destroy_item(cull);
 if (mitm.iclass [cull] == 0 && mitm.iplus [cull] % 100 == 50 && random2(3) == 0) destroy_item(cull);
}

} // end cull_items


void handle_traps(char trt, char trap_known)
{

switch (trt)
{
 case 6:
 if (trap_known == 1 && random2(3) == 0)
 {
  strcpy(info, "You avoid triggering a blade trap.");
  mpr(info);
  return;
 }
 if (random2(player_evasion()) + random2(you[0].dex) / 3 + env[0].trap_known * 3 > 8)
 {
  strcpy(info, "A huge blade swings just past you!");
  mpr(info);
  return;
 }
  strcpy(info, "A huge blade swings out and slices into you!");
  mpr(info);
  ouch(10 + random2(15) + random2(15) - random2(player_AC() + 1), 0, 10);
 break;

}

}



void disarm_trap(struct dist disa [1])
{

/*
strcpy(info, "Disarm which trap?");
mpr(info);
strcpy(info, "Which direction?");
mpr(info);
struct dist disa [1];
direction(0, disa);


if (disa[0].nothing == -1)
{
        strcpy(info, "Huh?");
        mpr(info);
        return;
}

if (mgrd [you[0].x_pos + disa[0].move_x] [you[0].y_pos + disa[0].move_y] != MNG)
{
        strcpy(info, "Not there!");
        mpr(info);
        return;
}

if (disa[0].move_x > 1 || disa[0].move_y > 1)
{
        strcpy(info, "You can't reach that space.");
        mpr(info);
        return;
}

if (disa[0].move_x == 0 && disa[0].move_y == 0)
{
        strcpy(info, "You aren't a trap!");
        mpr(info);
        return;
}

if (grd [you[0].x_pos + disa[0].move_x] [you[0].y_pos + disa[0].move_y] < 75 || grd [you[0].x_pos + disa[0].move_x] [you[0].y_pos + disa[0].move_y] > 77)
{
        strcpy(info, "You can't see a trap there.");
        mpr(info);
        return;
}
*/

if (you[0].berserker != 0)
{
 mpr("You're too berserk!");
 return;
}

int i, j;

for (i = 0; i < NTRAPS; i ++)
{
 if (env[0].trap_x [i] == you[0].x_pos + disa[0].move_x && env[0].trap_y [i] == you[0].y_pos + disa[0].move_y) break;
 if (i == NTRAPS - 1)
 {
        strcpy(info, "Error - couldn't find that trap.");
        mpr(info);
        return;
 }
}

if (env[0].trap_type [i] == 4 || env[0].trap_type [i] == 5)
{
        strcpy(info, "You can't disarm that trap.");
        mpr(info);
        return;
}

if (random2(you[0].skills [18] + 5) <= 3) // && you[0].clas != 3)
{
        strcpy(info, "You failed to disarm the trap.");
        mpr(info);
        you[0].turnover = 1;
/*        if (random2(2) == 0)
        {
          disa[0].move_x = 0; disa[0].move_y = 0;
        }*/
        if (random2(2) == 0) exercise(18, 1);
        return;
}

strcpy(info, "You have disarmed the trap.");
mpr(info);

//int trapped = i;

struct bolt beam [1];
beam[0].bx = you[0].x_pos + disa[0].move_x;
beam[0].by = you[0].y_pos + disa[0].move_y;

if (env[0].trap_type [i] <= 3)
{
 for (j = 0; j < 20; j ++)
 {
  itrap(beam, i); /* places items (eg darts), which will automatically stack */
  if (j > 10 && random2(3) == 0) break;
 }
}


grd [you[0].x_pos + disa[0].move_x] [you[0].y_pos + disa[0].move_y] = 67;
env[0].trap_type [i] = 100;

you[0].turnover = 1;

exercise(18, 5 + random2(5));

}


int manage_clouds(void)
{

int cc;
int cchck = 0;
int ugy = env[0].cloud_no;

for (cc = 0; cc < CLOUDS; cc++)
{

        if (env[0].cloud_type [cc] == 0) continue;

        cchck++;

        if (you[0].haste == 0) env[0].cloud_decay [cc] -= you[0].time_taken;
                else env[0].cloud_decay [cc] -= you[0].time_taken / 2;

 if ((env[0].cloud_type [cc] == 1 && grd [env[0].cloud_x [cc]] [env[0].cloud_y [cc]] == 12) || (env[0].cloud_type [cc] == 3 && grd [env[0].cloud_x [cc]] [env[0].cloud_y [cc]] == 11))
 {
        if (you[0].haste == 0) env[0].cloud_decay [cc] -= you[0].time_taken * 2;
                else env[0].cloud_decay [cc] -= you[0].time_taken;
 }

if (you[0].slow != 0)
{
        env[0].cloud_decay [cc] -= you[0].time_taken;

 if ((env[0].cloud_type [cc] == 1 && grd [env[0].cloud_x [cc]] [env[0].cloud_y [cc]] == 62) || (env[0].cloud_type [cc] == 3 && grd [env[0].cloud_x [cc]] [env[0].cloud_y [cc]] == 61))
 {
        env[0].cloud_decay [cc] -= you[0].time_taken * 2;
 } /* lava makes cold clouds dissipate, ditto for water and flaming clouds */
}


        if (env[0].cloud_decay [cc] <= 0)
        {
                env[0].cloud_type [cc] = 0;
                env[0].cloud_decay [cc] = 0;
                env[0].cgrid [env[0].cloud_x [cc]] [env[0].cloud_y [cc]] = CNG;
                ugy--;

//              cchck--; // <--- maybe?
        }

}

return ugy;

}


void weird_writing(char stringy [40])
{

strcpy(stringy, "");

switch(random2(15))
{
 case 0: strcpy(stringy, "writhing "); break;
 case 1: strcpy(stringy, "bold "); break;
 case 2: strcpy(stringy, "faint "); break;
 case 3: strcpy(stringy, "spidery "); break;
 case 4: strcpy(stringy, "blocky "); break;
 case 5: strcpy(stringy, "angular "); break;
 case 6: strcpy(stringy, "shimmering "); break;
 case 7: strcpy(stringy, "glowing "); break;
}

switch(random2(14))
{
 case 0: strcat(stringy, "yellow "); break;
 case 1: strcat(stringy, "brown "); break;
 case 2: strcat(stringy, "black "); break;
 case 3: strcat(stringy, "purple "); break;
 case 4: strcat(stringy, "orange "); break;
 case 5: strcat(stringy, "lime-green "); break;
 case 6: strcat(stringy, "blue "); break;
 case 7: strcat(stringy, "grey "); break;
 case 8: strcat(stringy, "silver "); break;
 case 9: strcat(stringy, "gold "); break;
 case 10: strcat(stringy, "umber "); break;
 case 11: strcat(stringy, "charcoal "); break;
 case 12: strcat(stringy, "pastel "); break;
 case 13: strcat(stringy, "mauve "); break;
}

switch(random2(14))
{
 case 0: strcat(stringy, "writing"); break;
 case 1: strcat(stringy, "scrawl"); break;
 case 2: strcat(stringy, "sigils"); break;
 case 3: strcat(stringy, "runes"); break;
 case 4: strcat(stringy, "hieroglyphics"); break;
 case 5: strcat(stringy, "scrawl"); break;
 case 6: strcat(stringy, "print-out"); break;
 case 7: strcat(stringy, "binary code"); break;
 case 8: strcat(stringy, "glyphs"); break;
 case 9: strcat(stringy, "symbols"); break;
 default: strcat(stringy, "text"); break;
}

return;

}



void fall_into_a_pool(char place, unsigned char grype)
{

char escape = 0;
char empty [2];

switch(grype)
{
 case 61: strcpy(info, "You fall into the lava!"); mpr(info); break;
 case 62: strcpy(info, "You fall into the water!"); mpr(info); break;
}

//mpr(info);
more();
mesclr();

if (grype == 61 && player_res_fire() <= 100)
{
  strcpy(info, "You burn to a cinder!");
  mpr(info);
  ouch(-9999, 0, 5);
}

// a distinction between stepping and falling from you[0].lev stops you stepping into a thin stream of lava to get to the other side.

if (place == 1 && scramble() == 1)
{
        if (empty_surrounds(you[0].x_pos, you[0].y_pos, 67, 0, empty) == 1)
        {
        you[0].x_pos = empty [0];
                you[0].y_pos = empty [1];
  escape = 1;
        } else goto drowning;
}

if (place == 0 && scramble() == 1)
{
                escape = 1;
}

if (escape == 1)
{
 strcpy(info, "You manage to scramble free!");
 mpr(info);
 if (grype == 61) scrolls_burn(10, 6);
 return;
}

if (place == 0) // don't display this if you fall in from levitating
{
 strcpy(info, "You try to escape, but your burden drags you down!");
 mpr(info);
}

drowning: strcpy(info, "You drown...");
mpr(info);
if (grype == 61) ouch(-9999, 0, 5);
if (grype == 62) ouch(-9999, 0, 6);
// Okay, so you don't trigger a trap when you scramble onto it. I really can't be bothered right now.

} // end of fall_into_a_pool()


char scramble(void)
{
if (((1000 + you[0].strength * 200) / 2) + random2((1000 + you[0].strength * 200) / 2) <= you[0].burden)
                         return 0; // failed

return 1; // success
} // end of scramble



void weird_colours(unsigned char coll, char wc [30])
{

strcpy(wc, "");

//if (coll > 15)
//{
switch(coll / 16)
{
        case 0: strcat(wc, "brilliant "); break;
        case 1: strcat(wc, "pale "); break;
        case 2: strcat(wc, "mottled "); break;
        case 3: strcat(wc, "shimmering "); break;
        case 4: strcat(wc, "bright "); break;
        case 5: strcat(wc, "dark "); break;
        case 6: strcat(wc, "shining "); break;
        case 7: strcat(wc, "brilliant "); break;
        case 8: strcat(wc, "pale "); break;
        case 9: strcat(wc, "mottled "); break;
        case 10: strcat(wc, "shimmering "); break;
        case 11: strcat(wc, "bright "); break;
        case 12: strcat(wc, "dark "); break;
        case 13: strcat(wc, "shining "); break;

        // Must start with a consonant!


//      case 7: strcat(wc, " "); break;
//      case 8: strcat(wc, " "); break;

        default: strcat(wc, "faint "); break;
}
//} // end of if coll > 15

while(coll > 17)
{
        coll -= 10;
}

switch(coll)
{
        case 0: strcat(wc, "red"); break;
        case 1: strcat(wc, "purple"); break;
        case 2: strcat(wc, "green"); break;
        case 3: strcat(wc, "orange"); break;
        case 4: strcat(wc, "magenta"); break;
        case 5: strcat(wc, "black"); break;
        case 6: strcat(wc, "grey"); break;
        case 7: strcat(wc, "silver"); break;
        case 8: strcat(wc, "gold"); break;
        case 9: strcat(wc, "pink"); break;
        case 10: strcat(wc, "yellow"); break;
        case 11: strcat(wc, "white"); break;
        case 12: strcat(wc, "brown"); break;
        case 13: strcat(wc, "aubergine"); break;
        case 14: strcat(wc, "ochre"); break;
        case 15: strcat(wc, "leaf green"); break;
        case 16: strcat(wc, "mauve"); break;
        case 17: strcat(wc, "azure"); break;
//      case 18: strcat(wc, ""); break;
//      case 19: strcat(wc, ""); break;
}


return;

}

char go_berserk(void)
{
  if (you[0].berserker != 0 || you[0].slow != 0) return 0;
  if (you[0].is_undead == 2) return 0;
  strcpy(info, "A red film seems to cover your vision as you go berserk!");
  mpr(info);
  strcpy(info, "You feel yourself moving faster!");
  mpr(info);
  strcpy(info, "You feel mighty!");
  mpr(info);
  you[0].berserker += 20 + random2(10) + random2(10);
  calc_hp();
  you[0].hp *= 15;
  you[0].hp /= 10;
  you[0].hp_ch = 1;
  if (you[0].hp >= you[0].hp_max) you[0].hp = you[0].hp_max;
  if (you[0].might == 0)
  {
   you[0].strength_ch = 1;
   you[0].strength += 5;
   you[0].max_strength += 5;
  }
  you[0].might += you[0].berserker;
  you[0].haste += you[0].berserker;
  return 1;
}



int trap_item(char iclass, char itype, char beam_x, char beam_y)
{

if (igrd [beam_x] [beam_y] != 501)
        {
                if ((iclass == 1 || iclass == 4 || iclass == 6 || iclass == 8 || iclass == 9) && iclass == mitm.iclass [igrd [beam_x] [beam_y]] && itype == mitm.itype [igrd [beam_x] [beam_y]] && mitm.iplus [igrd [beam_x] [beam_y]] == 50 && mitm.idam [igrd [beam_x] [beam_y]] == 0)
                {
                        mitm.iquant [igrd [beam_x] [beam_y]] ++;
                        return 0;
                }
        } // end of if igrd != 501

if (igrd [beam_x] [beam_y] != 501)
if (mitm.ilink [igrd [beam_x] [beam_y]] != 501)
if (mitm.ilink [mitm.ilink [igrd [beam_x] [beam_y]]] != 501)
 return 0;
// don't want to go overboard here. Will only generate up to three separate trap items, or less if there are other items on the square.


        int o;

        for (o = 0; o < ITEMS; o++)
        {
        if (o == 1) continue;
  if (o >= 200) return 0;
                if (mitm.iquant [o] == 0)
                {
                        mitm.iid [o] = 0;
                        mitm.iclass [o] = iclass;
                        mitm.itype [o] = itype;
                        mitm.iplus [o] = 50;
                        mitm.iplus2 [o] = 50;
                        mitm.idam [o] = 0;
                        mitm.icol [o] = LIGHTCYAN;
                        mitm.iquant [o] = 1;
                        mitm.ilink [o] = 501;
//                      it_no ++;
                        break;
                }
        } // end of o loop






        if (igrd [beam_x] [beam_y] == 501)
        {
                igrd [beam_x] [beam_y] = o;
        } else
        {
                int hug = igrd [beam_x] [beam_y];
                igrd [beam_x] [beam_y] = o;
                mitm.ilink [o] = hug;
        }
return 1;

}

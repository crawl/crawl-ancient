#include "config.h"

#include <string.h>

#include "externs.h"

#include "beam.h"
#include "bang.h"
#include "effects.h"
#include "fight.h"
#include "itemname.h"
#include "monplace.h"
#include "mstruct.h"
#include "mstuff2.h"
#include "player.h"
#include "spells2.h"
#include "stuff.h"
#include "view.h"

int mons_spells(char spell_cast, int func_pass [10], char beam_name [30]);
void itrap(struct bolt beam [1], int trapped);

/*
NOTE: must fix species abils to not use duration 15


*/


void mons_trap(int i)
{
struct bolt beem [1];
int damage_taken;

                        int tr;

                        for (tr = 0; tr < NTRAPS; tr ++)
                        {
                                if (env[0].trap_x [tr] == menv [i].m_x && env[0].trap_y [tr] == menv [i].m_y) break;
                        }

                        if (mons_near(i) == 1 && grd [env[0].trap_x [tr]] [env[0].trap_y [tr]] == 78)
                        {
                         grd [env[0].trap_x [tr]] [env[0].trap_y [tr]] -= 3;
                         if (env[0].trap_type [tr] >= 4) grd [env[0].trap_x [tr]] [env[0].trap_y [tr]] ++;
                         if (env[0].trap_type [tr] == 6) grd [env[0].trap_x [tr]] [env[0].trap_y [tr]] --;
                        }

                        switch(env[0].trap_type [tr])
                        {

                        case 0:
                        strcpy(beem[0].beam_name, " dart");
                        beem[0].damage = 4;
                        beem[0].colour = 1;
                        beem[0].type = 3;
                        break;

                        case 1:
                        strcpy(beem[0].beam_name, "n arrow");
                        beem[0].damage = 7;
                        beem[0].colour = 1;
                        beem[0].type = 1;
                        break;

                        case 2:
                        strcpy(beem[0].beam_name, " spear");
                        beem[0].damage = 10;
                        beem[0].colour = 0;
                        beem[0].type = 11;
                        break;

                        case 3:
                        strcpy(beem[0].beam_name, "n axe");
                        beem[0].damage = 15;
                        beem[0].colour = 0;
                        beem[0].type = 9;
                        break;

   case 4:
   if (menv [i].m_class == 19 || menv [i].m_class == 56) return;
   monster_teleport(i, 1);
   return;

   case 5:
        if (mons_near(i) == 1)
        {
                strcpy(info, monam (menv [i].m_sec, menv [i].m_class, menv [i].m_ench [2], 0));
     strcat(info, " looks momentarily disoriented.");
     mpr(info);
   }
   return;

   case 6:
  if (random2(5) == 0)
  {
   strcpy(info, monam (menv [i].m_sec, menv [i].m_class, menv [i].m_ench [2], 0));
   strcat(info, " avoids triggering a blade trap.");
   if (mons_near(i) == 1) mpr(info);
   return;
  }
  if (random2(menv [i].m_ev) > 8)
  {
   strcpy(info, monam (menv [i].m_sec, menv [i].m_class, menv [i].m_ench [2], 0));
   strcat(info, " avoids a huge swinging blade.");
   if (mons_near(i) == 1) mpr(info);
   return;
  }
   strcpy(info, "A huge blade swings out and slices into ");
   strcat(info, monam (menv [i].m_sec, menv [i].m_class, menv [i].m_ench [2], 1));
   strcat(info, "!");
   if (mons_near(i) == 1) mpr(info);
   damage_taken = 10 + random2(15) + random2(15) - random2(menv[i].m_AC + 1);
   if (damage_taken <= 0) damage_taken = 0;
   menv [i].m_hp -= damage_taken;
   if (menv [i].m_hp <= 0)
   {
        monster_die(i, 5, 0);
        menv [i].m_speed_inc = 1;
   }
   return;


                        } // end of switch

        if ((20 + you[0].your_level * 2) * random2(200) / 100 >= menv [i].m_ev);
        {
                damage_taken = random2(beem[0].damage);
                damage_taken -= random2 (menv [i].m_AC + 1);
                if (damage_taken > 0) menv [i].m_hp -= damage_taken;
        }
        if (damage_taken > 0)
        {
                if (mons_near(i) == 1)
                {
                        strcpy(info, "A");
                        strcat(info, beem[0].beam_name);
                        strcat(info, " hits ");
                        strcat(info, monam (menv [i].m_sec, menv [i].m_class, menv [i].m_ench [2], 1)); //gmon_name [mons_class [i]]);
                        strcat(info, "!");
                        mpr(info);
                }
                if (menv [i].m_hp <= 0)
                {
                        monster_die(i, 5, 0);
                        menv [i].m_speed_inc = 1;
                }
        } else
        if (mons_near(i) == 1)
        {
                strcpy(info, "A");
                strcat(info, beem[0].beam_name);
                strcat(info, " misses ");
                strcat(info, monam (menv [i].m_sec, menv [i].m_class, menv [i].m_ench [2], 1)); //gmon_name [mons_class [i]]);
                strcat(info, "!");
                mpr(info);
        }




/*      if (random2(10) > 6 && it_no > 300) return; */

        if (random2(10) > 6) return;

        beem[0].bx = menv [i].m_x;
        beem[0].by = menv [i].m_y;

/*      trapped = tr; */

        itrap(beem, tr);

} // end of mons_trap()




void mons_cast(int i, struct bolt beem [1], int spell_cast)
{
int sumcount = 0;
int sumcount2;
int summonik = 0;
beem[0].beam_source = i;

if (spell_cast == 20 || spell_cast == 23 || spell_cast == 30 || spell_cast == 43) // etc...
{
        switch(spell_cast)
        {
  case 20: beem[0].type = 0; break; // burst of h-fire
  case 23: beem[0].type = 2; break; // brain feed
  case 30: beem[0].type = 1; break; // smiting
  case 43: beem[0].type = 3; break; // mutation
        } // end switch
if (beem[0].trac_targ == MHITYOU || beem[0].trac_targ == MHITNOT)
{
 if (mons_near(i) == 1) direct_effect(beem);
 return;
}
mons_direct_effect(beem, i);
return;
} // end of if (spell_cast)/

switch(spell_cast)
{

case 21: // vampire summoning
 sumcount2 = random2(4) + 1;
 for (sumcount = 0; sumcount < sumcount2; sumcount ++)
 if (random2(3) != 0) create_monster(1, 24, menv [i].m_beh, menv [i].m_x, menv [i].m_y, menv [i].m_hit, 250);
    else create_monster(17, 24, menv [i].m_beh, menv [i].m_x, menv [i].m_y, menv [i].m_hit, 250);
        return;


case 24: // summon anything
 sumcount2 = random2(4) + 1;
 for (sumcount = 0; sumcount < sumcount2; sumcount ++)
                        create_monster(random2(70), 24, menv [i].m_beh, menv [i].m_x, menv [i].m_y, menv [i].m_hit, 250);
                        return;

case 25: // summon fake R
  sumcount2 = random2(2) + 1;
 for (sumcount = 0; sumcount < sumcount2; sumcount ++)
                        create_monster(78, 21, menv [i].m_beh, menv [i].m_x, menv [i].m_y, menv [i].m_hit, 250);
                        return;

case 27: // summon demon
/*    summonik = random2(8) + 80;
    if (random2(4) == 0 || (summonik == 84 && random2(3) != 0)) summonik = 3;
    if (random2(4) == 0) summonik = random2(15) + 220;
    if (random2(20) == 0) summonik = random2(2) + 235;*/
    create_monster(summon_any_demon(1), 22, menv [i].m_beh, menv [i].m_x, menv [i].m_y, menv [i].m_hit, 250);
    return;

case 28: // animate dead
    if (mons_near(i) == 0 || animate_dead(100, menv [i].m_beh, menv [i].m_hit, 0) <= 0)
    {
       spell_cast = 100;
       return;
    }
    animate_dead(5 + random2(5), menv [i].m_beh, menv [i].m_hit, 1);
    return;

case 33: // summon lesser demon
        create_monster(summon_any_demon(0), 21, menv [i].m_beh, menv [i].m_x, menv [i].m_y, menv [i].m_hit, 250);
    return;

case 34: // summon LIGHTCYAN lesser demon
        create_monster(222, 21, menv [i].m_beh, menv [i].m_x, menv [i].m_y, menv [i].m_hit, 250);
    return;

/*case 37: // summon any demon
        create_monster(summonik, 21, menv [i].m_beh, menv [i].m_x, menv [i].m_y, menv [i].m_hit, 250);
    return;*/


case 36: // Geryon
 create_monster(88, 23, menv [i].m_beh, menv [i].m_x, menv [i].m_y, menv [i].m_hit, 250);
 return;

case 42: // summon undead around player
  sumcount2 = random2(3) + 2;
 for (sumcount = 0; sumcount < sumcount2; sumcount ++)
 {
  do
  {
   summonik = random2(241);
  } while (mons_holiness(summonik) != 1);
  create_monster(summonik, 21, menv [i].m_beh, you[0].x_pos, you[0].y_pos, menv [i].m_hit, 250);
 }
                        return;


case 48: // torment

  if (mons_near(i) == 0 || menv [i].m_beh == 7) return;
  if (menv [i].m_ench [2] != 6)
  {
        strcpy(info, monam (menv [i].m_sec, menv [i].m_class, menv [i].m_ench [2], 0));
        strcat(info, " calls on the powers of Hell!");
        mpr(info);
  }
/*  mmov_x = 0; mmov_y = 0;*/
  torment();
  return;
} // end switch

//out_of_spec :
viewwindow(1);


beem[0].move_x = beem[0].trac_targ_x - menv [i].m_x;
beem[0].move_y = beem[0].trac_targ_y - menv [i].m_y;

beem[0].target_x = beem[0].trac_targ_x;
beem[0].target_y = beem[0].trac_targ_y;

beem[0].source_x = menv [i].m_x;
beem[0].source_y = menv [i].m_y;

// Need to correct this for power of spellcaster
int func_pass [10];
func_pass [8] = menv [i].m_HD * 6;

int ufdg = mons_spells(spell_cast, func_pass, beem[0].beam_name);

beem[0].ench_power = beem[0].damage; // !!!
beem[0].colour = func_pass [0];
beem[0].range = func_pass [1];
beem[0].damage = func_pass [2];
beem[0].hit = func_pass [3];
beem[0].type = func_pass [4];
beem[0].flavour = func_pass [5];
beem[0].thing_thrown = func_pass [6];

beem[0].ench_power = 5 * menv [i].m_HD;

if (spell_cast == 5 || spell_cast == 11 || spell_cast == 13 || spell_cast == 14) // you[0].haste/you[0].invis
{
        beem[0].move_x = 0;
        beem[0].move_y = 0;
}

if (spell_cast == 14)
 beem[0].ench_power = 2000;

if (spell_cast == 20) // burst of hellfire
{

 return;
}

switch(ufdg)
{
        case 0: missile(beem, 0); break;
        case 1: beam(beem); break;
}






} // end of void mons_cast()




void monster_teleport(char monstel, char instan)
{
int p = 0;

if (instan == 0)
{
                for (p = 0; p < 3; p ++)
                {
                        if (menv [monstel].m_ench [p] >= 40 && menv [monstel].m_ench [p] <= 43)
                        {
                                menv [monstel].m_ench [p] = 0;
                                if (menv [monstel].m_ench [0] == 0 && menv [monstel].m_ench [1] == 0 && menv [monstel].m_ench [2] == 0)
                                        menv [monstel].m_ench_1 = 0;
                                        if (mons_near(monstel) == 1 && (player_see_invis() != 0 || menv [monstel].m_ench [2] != 6))
                                        {
                                           strcpy(info, monam (menv [monstel].m_sec, menv [monstel].m_class, menv [monstel].m_ench [2], 0));
                                           strcat(info, " seems more stable.");
                                           mpr(info);
                                        }
                                return;
                        }
                } // end of for p

                        for (p = 0; p < 3; p ++)
                        {
                         if (menv [monstel].m_ench [p] == 0)
                                        {

                                                menv [monstel].m_ench [p] = 42 + random2(2);
                                                menv [monstel].m_ench_1 = 1;
                                                break;
                                        }
                        }
 return;
}


if (mons_near(monstel) == 1 && (player_see_invis() != 0 || menv [monstel].m_ench [2] != 6))
{
     strcpy(info, monam (menv [monstel].m_sec, menv [monstel].m_class, menv [monstel].m_ench [2], 0));
     strcat(info, " disappears!");
     mpr(info);
}
mgrd [menv [monstel].m_x] [menv [monstel].m_y] = MNG;

char ogrid = 67;

if (menv [monstel].m_class >= MLAVA0) ogrid = 61;
if (menv [monstel].m_class >= MWATER0) ogrid = 62;

do
{
        menv [monstel].m_x = random2(60) + 10;
        menv [monstel].m_y = random2(50) + 10;
} while (grd [menv [monstel].m_x] [menv [monstel].m_y] != ogrid || mgrd [menv [monstel].m_x] [menv [monstel].m_y] != MNG);

mgrd [menv [monstel].m_x] [menv [monstel].m_y] = monstel;

/* Mimics change form/colour when tported */
if (menv [monstel].m_class >= 389 && menv [monstel].m_class <= 393)
{
 menv [monstel].m_class = 389 + random2(5);
 menv [monstel].m_sec = 1 + random2(15);
 if (menv [monstel].m_class == 390 || menv [monstel].m_class == 391) /* weapon/armour mimic */
 {
  if (random2(4) == 0) menv [monstel].m_sec = LIGHTCYAN;
   else menv [monstel].m_sec = BROWN;
  if (random2(10) == 0) menv [monstel].m_sec = CYAN;
  if (random2(20) == 0) menv [monstel].m_sec = random2(15) + 1;
 }
}

} // end of void monster_teleport



void dragon(int i, struct bolt beem [1])
{

viewwindow(1);
beem[0].move_x = 0; beem[0].move_y = 0;

beem[0].move_x = beem[0].trac_targ_x - menv [i].m_x;
beem[0].move_y = beem[0].trac_targ_y - menv [i].m_y;



if (beem[0].move_x != 0 || beem[0].move_y != 0)
{
        beem[0].range = random2(10) + 5;

        beem[0].source_x = menv [i].m_x; beem[0].source_y = menv [i].m_y;

                strcpy(beem[0].beam_name, monam (menv [i].m_sec, menv [i].m_class, menv [i].m_ench [2], 4)); //gmon_name [mons_class [i]]);
  switch(menv [i].m_class)
  {
  case 73:
  case 29:
  case 148:
  case 306: // Xtahua
  strcat(beem[0].beam_name, "'s blast of flame");
  beem[0].flavour = 2;
  beem[0].colour = RED;
  break;
  case 75:
  strcat(beem[0].beam_name, "'s blast of cold");
  beem[0].flavour = 3;
  beem[0].colour = WHITE;
  break;
  }

                beem[0].damage = 100 + menv [i].m_HD;
                beem[0].type = '#';
  beem[0].hit = 30;
  beem[0].beam_source = i;

  if (mons_near(i) == 1)
                {
                strcpy(info, monam (menv [i].m_sec, menv [i].m_class, menv [i].m_ench [2], 0)); //gmon_name [mons_class [i]]);
                strcat(info, " breathes.");
                mpr(info);
  }
        beem[0].thing_thrown = 2;
        beam(beem);
}
} // end of dragon();




void mons_throw(int i, struct bolt beem [1], int hand_used)
{

char shoot = 0;
beem[0].range = 7;
beem[0].beam_source = i;
/*if (menv [i].m_x > you[0].x_pos - 6 && menv [i].m_x < you[0].x_pos + 6 && menv [i].m_y > you[0].y_pos - 6 && menv [i].m_y < you[0].y_pos + 6)
{

if (show [menv [i].m_x - you[0].x_pos + 6] [menv [i].m_y - you[0].y_pos + 6] != 0)
{*/
if (mons_near(i) == 1) /* This is obviously a problem; no throwing unless mons on screen. */
{
beem[0].move_x = 0; beem[0].move_y = 0;

beem[0].move_x = beem[0].trac_targ_x - menv [i].m_x;
beem[0].move_y = beem[0].trac_targ_y - menv [i].m_y;

if (beem[0].move_x != 0 || beem[0].move_y != 0)
{
        beem[0].type = 41;
        beem[0].source_x = menv [i].m_x;
        beem[0].source_y = menv [i].m_y;
        beem[0].colour = mitm.icol [hand_used];
 beem[0].flavour = 0;
        beem[0].damage = property(mitm.iclass [hand_used], mitm.itype [hand_used], 0) + mitm.iplus [hand_used] - 50;
 beem[0].hit = 7;
        beem[0].hit += mitm.iplus [hand_used] - 50;

        beem[0].thing_thrown = 2;

        if (menv [i].m_class == 12)
        {
                strcpy(info, monam (menv [i].m_sec, menv [i].m_class, menv [i].m_ench [2], 0)); //gmon_name [mons_class [i]]);
                strcat(info, " flicks its tail!");
                beem[0].damage = 10;
                beem[0].hit *= 2;
        }





        if (menv [i].m_class != 12)
        {

        if (menv [i].m_inv [0] != 501 && mitm.iclass [menv [i].m_inv [0]] == 0 && mitm.itype [menv [i].m_inv [0]] == mitm.itype [menv [i].m_inv [1]] + 13 && mitm.itype [menv [i].m_inv [1]] <= 16)
        {

                beem[0].damage = property(mitm.iclass [menv [i].m_inv [1]], mitm.itype [menv [i].m_inv [1]], 1);

                int hoggl = mitm.iplus2 [menv [i].m_inv [0]] - 50;

                if (mitm.iplus2 [menv [i].m_inv [0]] > 80) hoggl -= 100;

                beem[0].damage += hoggl;
                beem[0].damage += menv [i].m_HD;


                /* elven bow w/ elven arrow, also orcis */
                if (mitm.idam [menv [i].m_inv [0]] / 30 == mitm.idam [menv [i].m_inv [1]] / 30)
                {
                 beem[0].hit ++;
                 beem[0].damage ++;
                 if (mitm.idam [menv [i].m_inv [0]] / 30 == 120) beem[0].hit ++;
                }


                int ghoggl = mitm.iplus [menv [i].m_inv [0]] - 50;

                if (mitm.iplus [menv [i].m_inv [0]] > 80) ghoggl -= 100;

                beem[0].hit += ghoggl;

                beem[0].range += 5;

                if (mitm.iplus [menv [i].m_inv [0]] > 80)
                {
                        beem[0].damage -= 100;
                        beem[0].hit -= 100;
                }
                shoot = 1;


                if (mitm.idam [menv [i].m_inv [0]] % 30 == 11)
                {
                 beem[0].hit += 2;
                 beem[0].damage += 1 + random2(5);
                 beem[0].flavour = 2;
                 strcpy(beem[0].beam_name, "bolt of ");
                 if (mitm.idam [hand_used] % 30 == 3 || mitm.idam [hand_used] % 30 ==  4)
                        strcat(beem[0].beam_name, "poison ");
                 strcat(beem[0].beam_name, "flame");
                 beem[0].colour = RED;
                 beem[0].type = 35;
                 beem[0].thing_thrown = 4;
                }
                if (mitm.idam [menv [i].m_inv [0]] % 30 == 12)
                {
                 beem[0].hit += 2;
                 beem[0].damage += 1 + random2(5);
                 beem[0].flavour = 3;
                 strcpy(beem[0].beam_name, "bolt of ");
                 if (mitm.idam [hand_used] % 30 == 3 || mitm.idam [hand_used] % 30 ==  4)
                        strcat(beem[0].beam_name, "poison ");
                 strcat(beem[0].beam_name, "frost");
                 beem[0].colour = WHITE;
                 beem[0].type = 35;
                 beem[0].thing_thrown = 4;
                }

        }


                strcpy(info, monam (menv [i].m_sec, menv [i].m_class, menv [i].m_ench [2], 0)); //gmon_name [mons_class [i]]);

                if (shoot == 1)
                {
                        strcat(info, " shoots ");
                } else strcat(info, " throws ");

                item_name(mitm.iplus2 [hand_used], mitm.iclass [hand_used], mitm.itype [hand_used], mitm.idam [hand_used], mitm.iplus [hand_used], 1, mitm.iid [hand_used], 3, str_pass);
                strcat(info, str_pass);
                strcat(info, ".");

        }

        mpr(info);
        missile(beem, hand_used);
        mitm.iquant [hand_used] --;
        if (mitm.iquant [hand_used] == 0)
        {
         menv [i].m_inv [1] = 501;
/*       it_no --; */
        }
/*      brek = 1;*/
}
//}
}

if (menv [i].m_class == 2) menv [i].m_speed_inc -= 10;


}


void spore_goes_pop(int i) /* should really do something about mons_hit, but can't be bothered */
{

struct bolt beam [1];

if (mons_near(i) == 1)
{
        viewwindow(1);
        strcpy(info, "The giant spore explodes!");
        mpr(info);
}
        beam[0].beam_source = i;
        beam[0].type = 43;
        beam[0].damage = 115;
        beam[0].flavour = 11; // <=- not sure about this
        beam[0].bx = menv [i].m_x;
        beam[0].by = menv [i].m_y;
        strcpy(beam[0].beam_name, "explosion of spores");
        beam[0].colour = LIGHTGREY;
        beam[0].thing_thrown = 2; // someone else's explosion
        explosion(1, beam);
}



int mons_spells(char spell_cast, int func_pass [10], char beam_name [30])
{

/*
beam_colour = func_pass [0];
beam_range = func_pass [1];
beam_damage = func_pass [2];
beam_hit = func_pass [3];
beam_type = func_pass [4];
beam_flavour = func_pass [5];
thing_thrown = func_pass [6];
*/

switch(spell_cast)
{

case 0: // magic missile
//strcpy(info [info_lines], "The ");
//strcpy(info [info_lines], monam (mons_class [i], mons_ench [i] [2], 0)); //gmon_name [mons_class [i]]);
//strcat(info [info_lines], " hurls a ball of sizzling energy!");
//incrl();
func_pass [0] = 13;//inv_col [throw_2];//icolour [inv_class [throw_2]] [inv_type [throw_2]];
strcpy(beam_name, "magic dart");// inv_name [throw_2]);
func_pass [1] = random2(5) + 7;
func_pass [2] = 104 + (func_pass [8] / 100);
func_pass [3] = 1500;
func_pass [4] = 35;
func_pass [6] = 4;
//return 0;
func_pass [5] = 1; // a magical missile
return 0;
//break;

case 1: // flame
//strcpy(info [info_lines], "The ");
//strcpy(info [info_lines], monam (mons_class [i], mons_ench [i] [2], 0)); //gmon_name [mons_class [i]]);
//strcat(info [info_lines], " hurls a puff of flame!");
//incrl();
func_pass [0] = 4;
strcpy(beam_name, "puff of flame");
func_pass [1] = random2(5) + 7;
func_pass [2] = 105 + func_pass [8] / 40; // should this be the same as magic missile?
func_pass [3] = 60;
func_pass [4] = '#'; // maybe make return 0 put a # on wherever it hits?
func_pass [6] = 4;
func_pass [5] = 2; // fire
return 0;
//break;


case 2: // frost
//strcpy(info [info_lines], "The ");
//strcpy(info [info_lines], monam (mons_class [i], mons_ench [i] [2], 0)); //gmon_name [mons_class [i]]);
//strcat(info [info_lines], " hurls a puff of frost!");
//incrl();
func_pass [0] = 15;
strcpy(beam_name, "puff of frost");
func_pass [1] = random2(5) + 7;
func_pass [2] = 105 + func_pass [8] / 40; // should this be the same as magic missile?
func_pass [3] = 60;
func_pass [4] = '#'; // maybe make return 0 put a # on wherever it hits?
func_pass [6] = 4;
func_pass [5] = 3; // cold
return 0;
//break;

case 3: // paralysis
strcpy(beam_name, "0");
func_pass [1] = random2(5) + 6;
func_pass [4] = 0;
func_pass [0] = 3;
func_pass [6] = 4;
func_pass [5] = 4; // magic
return 1;
//break;

case 4: // slow monster
strcpy(beam_name, "0");
func_pass [1] = random2(5) + 6;
func_pass [4] = 0;
func_pass [0] = 0;
func_pass [6] = 4;
func_pass [5] = 4; // magic
return 1;
//break;

case 5: // haste self
strcpy(beam_name, "0");
func_pass [1] = random2(5) + 6;
func_pass [4] = 0;
func_pass [0] = 1;
func_pass [6] = 4;
func_pass [5] = 4; // magic
return 1;

case 6: // slow monster
strcpy(beam_name, "0");
func_pass [1] = random2(5) + 6;
func_pass [4] = 0;
func_pass [0] = 4;
func_pass [6] = 4;
func_pass [5] = 4; // magic
return 1;

case 7: // venom bolt
strcpy(beam_name, "bolt of poison");
func_pass [1] = random2(10) + 8;
func_pass [2] = 106 + func_pass [8] / 15;
func_pass [0] = LIGHTGREEN;
func_pass [4] = 35;
func_pass [6] = 2;
func_pass [5] = 6; // poison
func_pass [7] = 1;
func_pass [3] = 7 + random2(func_pass [8]) / 80;
return 1;

case 8:
strcpy(beam_name, "bolt of fire");
func_pass [1] = random2(10) + 5;
func_pass [2] = 108 + func_pass [8] / 15;
func_pass [0] = 4;
func_pass [4] = 35; // 35
func_pass [6] = 2;
func_pass [5] = 2; // fire
func_pass [3] = 8 + random2(func_pass [8]) / 80; // hit
return 1;
//break;


case 9:
strcpy(beam_name, "bolt of cold");
func_pass [1] = random2(10) + 5;
func_pass [2] = 108 + func_pass [8] / 15;
func_pass [0] = 15;
func_pass [4] = 35;
func_pass [6] = 2;
func_pass [5] = 3; // cold
func_pass [3] = 8 + random2(func_pass [8]) / 80; // hit
return 1;

case 10: // lightning
strcpy(beam_name, "bolt of lightning");
func_pass [1] = random2(10) + 8;
func_pass [2] = 108 + func_pass [8] / 11;
func_pass [0] = LIGHTCYAN;
func_pass [4] = 35;
func_pass [6] = 2;
func_pass [5] = 5; // electricity
func_pass [7] = 1;
func_pass [3] = 10 + random2(func_pass [8]) / 40;
return 1;

case 11: // make invisible
strcpy(beam_name, "0");
func_pass [1] = random2(5) + 6;
func_pass [4] = 0;
func_pass [0] = 5;
func_pass [6] = 2;
func_pass [5] = 4; // magic
return 1;

case 12: // fireball
func_pass [0] = 4;
strcpy(beam_name, "fireball");
func_pass [1] = random2(5) + 7;
func_pass [2] = 7 + func_pass [2] / 10;
func_pass [3] = 40;
func_pass [4] = '#';
func_pass [6] = 3;
func_pass [5] = 10; // fire
return 0;

case 13: // healing
strcpy(beam_name, "0");
func_pass [1] = random2(5) + 6;
func_pass [4] = 0;
func_pass [0] = 2;
func_pass [6] = 2;
func_pass [5] = 4; // magic
return 1;

case 14: // teleportation
strcpy(beam_name, "0");
func_pass [1] = random2(5) + 6;
func_pass [4] = 0;
func_pass [0] = 7; // 6 is used by digging
func_pass [6] = 2;
func_pass [5] = 4; // magic
return 1;

case 15: // teleport target away
strcpy(beam_name, "0");
func_pass [1] = random2(5) + 6;
func_pass [4] = 0;
func_pass [0] = 7; // 6 is used by digging
func_pass [6] = 2;
func_pass [5] = 4; // magic
return 1;

case 16:
return 0; // blink

case 17: // was splinters
strcpy(beam_name, "crystal spear");
func_pass [1] = random2(10) + 8;
func_pass [2] = 112 + func_pass [8] / 10;
func_pass [0] = WHITE;
func_pass [4] = ')';
func_pass [6] = 2;
func_pass [5] = 1;
func_pass [7] = 1;
func_pass [3] = 6; // + random2(func_pass [8]) / 10;
return 0;

case 18: // digging
strcpy(beam_name, "0");
func_pass [1] = random2(5) + 4 + random2(func_pass [8]) / 10;
func_pass [4] = 0;
func_pass [0] = 6;
func_pass [6] = 1;
func_pass [5] = 4;
return 1;

case 19: // negative energy
strcpy(beam_name, "bolt of negative energy");
func_pass [1] = random2(10) + 8;
func_pass [2] = 105 + func_pass [8] / 16;
func_pass [0] = DARKGREY;
func_pass [4] = 35;
func_pass [6] = 2;
func_pass [5] = 7; // -ve energy
func_pass [7] = 1;
func_pass [3] = 7 + random2(func_pass [8]) / 80;
return 1;
// 20, 21 are used

case 22: // mystic blast
func_pass [0] = 13;
strcpy(beam_name, "orb of energy");
func_pass [1] = random2(5) + 7;
func_pass [2] = 107 + (func_pass [8] / 12);;
func_pass [3] = 10 + (func_pass [8] / 20);
func_pass [4] = 35;
func_pass [6] = 4;
func_pass [5] = 1; // a magical missile
return 0;


// 23 is brain feed

case 26: // ball of steam
func_pass [0] = LIGHTGREY;
strcpy(beam_name, "ball of steam");
func_pass [1] = random2(5) + 7;
func_pass [2] = 106;
func_pass [3] = 11;
func_pass [4] = 35;
func_pass [6] = 4;
func_pass [5] = 2; // fire - I think this is appropriate
return 0;

// 27 is summon devils
// 28 is animate dead

case 29: // pain
strcpy(beam_name, "0");
func_pass [1] = random2(8) + 8;
func_pass [4] = 0;
func_pass [0] = 13; // pain
func_pass [6] = 2;
func_pass [2] = 50;
func_pass [3] = 7 + (func_pass [8] / 40);
func_pass [5] = 4; // magic
return 1;

// 30 is smiting

case 31: // sticky flame
func_pass [0] = 4;
strcpy(beam_name, "sticky flame");
func_pass [1] = random2(5) + 7;
func_pass [2] = 102 + func_pass [8] / 50;
func_pass [3] = 8 + func_pass [8] / 25;
func_pass [4] = '#';
func_pass [6] = 4;
func_pass [5] = 2; // fire
return 0;

case 32: // demon
strcpy(beam_name, "blast of poison");
func_pass [1] = random2(10) + 8;
func_pass [2] = 103 + func_pass [8] / 25;
func_pass [0] = LIGHTGREEN;
func_pass [4] = 35;
func_pass [6] = 2;
func_pass [5] = 6; // poison
func_pass [7] = 1;
func_pass [3] = 7 + random2(func_pass [8]) / 80;
return 1;

case 35: // purple bang thing
func_pass [0] = 13;
strcpy(beam_name, "blast");
func_pass [1] = random2(5) + 7;
func_pass [2] = 106;
func_pass [3] = 9;
func_pass [4] = 35;
func_pass [6] = 4;
func_pass [5] = 10; // an exploding magical missile
return 0;


        case 38: // sting
        func_pass [0] = GREEN;
        strcpy(beam_name, "sting");
        func_pass [1] = random2(5) + 9;
        func_pass [2] = 6 + func_pass [8] / 25;
        func_pass [3] = 60;
        func_pass [4] = '#';
        func_pass [6] = 4;
        func_pass [5] = 6; // fire
        func_pass [7] = 1; // wand_id
        return 0;

        case 39: // Iron Bolt
        func_pass [0] = LIGHTCYAN;
        strcpy(beam_name, "iron bolt");
        func_pass [1] = random2(5) + 5;
        func_pass [2] = 108 + (func_pass [8] / 7);
        func_pass [3] = 6 + (func_pass [8] / 25);
        func_pass [4] = ')';
        func_pass [6] = 4;
        func_pass [5] = 1; // a magical missile - or similarly unresisted thing
        func_pass [7] = 1;
        return 0;

        case 40: // stone arrow
        func_pass [0] = LIGHTGREY;
        strcpy(beam_name, "stone arrow");
        func_pass [1] = random2(5) + 9;
        func_pass [2] = 105 + (func_pass [8] / 10);
        func_pass [3] = 5 + func_pass [8] / 47;
        func_pass [4] = ')';
        func_pass [6] = 4;
        func_pass [5] = 1; // a magical missile - or similarly unresisted thing
        func_pass [7] = 1;
        return 0;

        case 41: // splash of poison
        func_pass [0] = GREEN;
        strcpy(beam_name, "splash of poison");
        func_pass [1] = random2(6) + 6;
        func_pass [2] = 4 + func_pass [8] / 10;
        func_pass [3] = 9;
        func_pass [4] = '#';
        func_pass [6] = 4;
        func_pass [5] = 6; // poison
        func_pass [7] = 1; // wand_id
        return 0;

        case 44: // zulzer
        func_pass [0] = YELLOW;
        strcpy(beam_name, "zulzer blast");
        func_pass [1] = random2(4) + 15;
        func_pass [2] = 120;
        func_pass [3] = 7;
        func_pass [4] = '#';
        func_pass [6] = 4;
        func_pass [5] = 1;
        func_pass [7] = 1;
        return 0;

        case 45: // disintegrate
        strcpy(beam_name, "0");
        func_pass [1] = random2(8) + 8;
        func_pass [4] = 0;
        func_pass [0] = 15; // disint
        func_pass [6] = 2;
        func_pass [2] = 50;
        func_pass [3] = 30 + (func_pass [8] / 10);
        func_pass [5] = 4; // magic
        return 1;

case 46: // swamp drake
strcpy(beam_name, "foul vapour");
func_pass [1] = random2(10) + 8;
func_pass [2] = 102 + func_pass [8] / 25;
func_pass [0] = GREEN;
func_pass [4] = 35;
func_pass [6] = 2;
func_pass [5] = 6; // poison
func_pass [7] = 1;
func_pass [3] = 7 + random2(func_pass [8]) / 80;
return 0;



/*
beam_colour = func_pass [0];
beam_range = func_pass [1];
beam_damage = func_pass [2];
beam_hit = func_pass [3];
beam_type = func_pass [4];
beam_flavour = func_pass [5];
thing_thrown = func_pass [6];
*/



case 49: // fiend's hellfire
strcpy(beam_name, "hellfire");
func_pass [1] = random2(10) + 5;
func_pass [2] = 20;
func_pass [3] = 20;
func_pass [0] = 20;
func_pass [4] = 35;
func_pass [6] = 2;
func_pass [5] = 10; // hellfire
return 1;




}

return 0; // Should never reach this. Stupid pedantic gcc warning thing.

}

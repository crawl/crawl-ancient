#include "config.h"

#include <string.h>

#include "externs.h"

#include "beam.h"
#include "direct.h"
#include "fight.h"
#include "itemname.h"
#include "it_use2.h"
#include "items.h"
#include "misc.h"
#include "monplace.h"
#include "mstruct.h"
#include "player.h"
#include "priest.h"
#include "spells0.h"
#include "spells1.h"
#include "stuff.h"
#include "view.h"

extern char wield_change; /* defined in output.cc */


void cast_selective_amnesia(void)
{

char spc = 0;
char spc2 = 0;
char ep_gain = 0;
int keyin = 0;

if (you[0].spell_no == 0)
{
        strcpy(info, "You don't know any spells."); // can this happen?
        mpr(info);
        return;
}

query : strcpy(info, "Forget which spell?");
mpr(info);

// test relay_message();

keyin = get_ch();

if (keyin == '?' | keyin == '*')
{
        char unthing = spell_list(); //invent(0, you[0].inv_quant, you[0].inv_dam, you[0].inv_class, you[0].inv_type, you[0].inv_plus, you[0].inv_ident, you[0].equip [0], you[0].equip [6], you[0].equip [5], you[0].equip [2], you[0].equip [1], you[0].equip [3], you[0].equip [4], you[0].ring);
#ifdef LINUX
redraw_screen();
#endif

        if (unthing == 2) return;

        if ((unthing >= 65 && unthing <= 90) | (unthing >= 97 && unthing <= 122))
        {
                keyin = unthing;
        } else
        {
       mesclr();
                goto query;
        }
}

spc = (int) keyin;

if (spc < 65 | (spc > 90 && spc < 97) | spc > 122)
{
        unknown : strcpy(info, "You don't know that spell.");
        mpr(info);
        return;
}

spc2 = conv_lett(spc);

if (you[0].spells [spc2] == 210)
{
        goto unknown;
}

if (random2(25) == 0)
{
 strcpy(info, "Oops! This spell sure is a blunt instrument.");
 mpr(info);
 forget_map(20 + random2(50));
 return;
}

ep_gain = spell_value(you[0].spells [spc2]);
if (ep_gain < 0) ep_gain = 0;

if (ep_gain == spell_value(you[0].spells [spc2]))
{
 strcpy(info, "The spell releases its latent energy back to you as it unravels.");
}

mpr(info);

you[0].spell_levels += ep_gain;
you[0].ep_ch = 1;
you[0].spell_no --;

you[0].spells [spc2] = 210;

} /* end of cast_selective_amn */


void remove_curse(void)
{

if (you[0].equip [0] != -1 && you[0].inv_class [you[0].equip [0]] == 0)
        {
        if (you[0].inv_plus [you[0].equip [0]] > 130) you[0].inv_plus [you[0].equip [0]] -= 100;
        }

        if (you[0].equip [7] != -1)
        {
        if (you[0].inv_plus [you[0].equip [7]] > 80) you[0].inv_plus [you[0].equip [7]] -= 100;
        }

        if (you[0].equip [8] != -1)
        {
        if (you[0].inv_plus [you[0].equip [8]] > 130) you[0].inv_plus [you[0].equip [8]] -= 100;
        }

        if (you[0].equip [9] != -1)
        {
        if (you[0].inv_plus [you[0].equip [9]] > 130) you[0].inv_plus [you[0].equip [9]] -= 100;
        }
        int i;
        for (i = 1; i < 7; i ++)
        {
                if (you[0].inv_plus [you[0].equip [i]] > 130) you[0].inv_plus [you[0].equip [i]] -= 100;
        }


        strcpy(info, "You feel as if something is helping you.");
        mpr(info);

} /* end of remove_curse() */


void detect_curse(void)
{

int curse_found = 0;
int i;

for (i = 0; i < 52; i ++)
{

   if (you[0].inv_quant [i] == 0) continue;

   if (you[0].inv_class [i] == 0 | you[0].inv_class [i] == 2 | you[0].inv_class [i] == 7)
   {
      if (you[0].inv_ident [i] == 0)
      {
        curse_found ++;
                                                                you[0].inv_ident [i] = 1;
      }
   }
} /* end of for i */

//if (curse_found > 0)
//{
// strcpy(info, "You sense a malignant aura.");
//} else
strcpy(info, "You sense the presence of curses on your possessions.");

 mpr(info);

}

void cast_smiting(int pow)
{

struct dist beam [1];
int i;

strcpy(info, "Smite whom?");
mpr(info);

direction(100, beam);

if (beam[0].nothing == -1 | mgrd [beam[0].target_x] [beam[0].target_y] == MNG)
{
        strcpy(info, "The spell fizzles.");
        mpr(info);
        return;
}

i = mgrd [beam[0].target_x] [beam[0].target_y];

strcpy(info, "You smite ");
strcat(info, monam (menv [i].m_sec, menv [i].m_class, menv [i].m_ench [2], 1));
strcat(info, "!");
mpr(info);

menv [i].m_hp -= random2(8) + random2(pow) / 3;
menv [i].m_hp -= random2(8) + random2(pow) / 3;
menv [i].m_hp -= random2(8) + random2(pow) / 3;

if (menv [i].m_hp <= 0) monster_die(i, 1, 0);

} /* end of smiting */



void cast_bone_shards(int power)
{

if (you[0].equip [0] == -1 | you[0].inv_class [you[0].equip [0]] != 14)//  | you[0].inv_type [you[0].equip [0]] != 1)
{
 strcpy(info, "The spell fails.");
 mpr(info);
 return;
}

if (you[0].inv_type [you[0].equip [0]] != 1)
{
 strcpy(info, "The corpse collapses into a mass of pulpy flesh.");
 mpr(info);

 return;
}

power *= 15;
power += mons_weight(you[0].inv_plus [you[0].equip [0]]);

struct dist spelld [1];
struct bolt beam [1];

if (spell_direction(spelld, beam) == -1) return;

   strcpy(info, "The skeleton explodes into sharp fragments of bone!");
   mpr(info);

unwield_item(you[0].equip [0]);
you[0].inv_quant [you[0].equip [0]] --;
if (you[0].inv_quant [you[0].equip [0]] == 0) /* can this be false? */
{
                you[0].inv_no --;
                you[0].equip [0] = -1;
                strcpy(info, "You are now empty handed.");
                mpr(info);
}

   zapping(26, power, beam);
/*   zapping(26, power);*/
   burden_change();

}


void sublimation(int pow)
{

unsigned char was_wielded = 0;

if (you[0].equip [0] == -1 | you[0].inv_class [you[0].equip [0]] != 4 | you[0].inv_type [you[0].equip [0]] != 21)
{
 if (player_prot_life() != 0 | you[0].deaths_door != 0)
 {
  strcpy(info, "A conflicting enchantment prevents the spell from coming into effect.");
  mpr(info);
  return;
 }

 if (you[0].hp <= 1)
 {
  strcpy(info, "Your attempt to draw power from your own body fails.");
  mpr(info);
  return;
 }

 strcpy(info, "You draw magical energy from your own body!");
 mpr(info);

 while(you[0].ep < you[0].ep_max && you[0].hp > 1)
 {
  you[0].ep ++;
  you[0].hp --;
  if (random2(pow) < 6 && you[0].hp > 1) you[0].hp --;
  if (random2(pow) < 6 && you[0].hp > 1) you[0].hp --;
  if (random2(pow) < 6 && you[0].hp > 1) you[0].hp --;
 }

 you[0].hp_ch = 1;
 you[0].ep_ch = 1;

 return;
}

strcpy(info, "The chunk of flesh you are holding crumbles to dust.");
mpr(info);
strcpy(info, "A flood of magical energy pours into your mind!");
mpr(info);

if (pow > 100) pow = 100;

you[0].ep += 7;
you[0].ep += random2(pow) / 3;
you[0].ep += random2(pow) / 3;
you[0].ep += random2(pow) / 3;

was_wielded = you[0].equip [0];
unwield_item(you[0].equip [0]);
you[0].equip [0] = -1;

you[0].inv_quant [was_wielded] --;
if (you[0].inv_quant [was_wielded] <= 0)
{
 you[0].inv_quant [was_wielded] = 0;
 you[0].inv_no --;
}
burden_change();

if (you[0].ep > you[0].ep_max) you[0].ep = you[0].ep_max;
you[0].ep_ch = 1;

}

void dancing_weapon(int pow, char force_hostile)
{
int numsc = 21 + random2(pow) / 5;
if (numsc > 25) numsc = 25;

int summs = 0;
int i = 0;
char behavi = 7;
char empty [2];

if (empty_surrounds(you[0].x_pos, you[0].y_pos, 67, 0, empty) == 0)
{
 failed_spell: strcpy(info, "You hear a popping sound.");
 mpr(info);
 return;
}

if (you[0].equip [0] == -1 | you[0].inv_class [you[0].equip [0]] != 0 | (you[0].inv_type [you[0].equip [0]] >= 13 && you[0].inv_type [you[0].equip [0]] <= 16) | you[0].inv_dam [you[0].equip [0]] >= 180)
{
 goto failed_spell;
}

if (you[0].inv_plus [you[0].equip [0]] >= 100 | force_hostile == 1) behavi = 1; /* a cursed weapon becomes hostile */

summs = create_monster(144, numsc, behavi, empty [0], empty [1], you[0].pet_target, 1);

if (summs == -1) goto failed_spell;

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
                mitm.iid [i] = you[0].inv_ident [you[0].equip [0]];
                mitm.iclass [i] = you[0].inv_class [you[0].equip [0]];
                mitm.itype [i] = you[0].inv_type [you[0].equip [0]];
                mitm.iplus [i] = you[0].inv_plus [you[0].equip [0]];
                mitm.iplus2 [i] = you[0].inv_plus2 [you[0].equip [0]];
                mitm.idam [i] = you[0].inv_dam [you[0].equip [0]];
                mitm.icol [i] = you[0].inv_col [you[0].equip [0]];
                mitm.iquant [i] = 1;
                mitm.ilink [i] = 501;
                /* it_no ++; */
                break;
        }
} /* end of i loop */

item_name(you[0].inv_plus2 [you[0].equip [0]], you[0].inv_class [you[0].equip [0]], you[0].inv_type [you[0].equip [0]], you[0].inv_dam [you[0].equip [0]], you[0].inv_plus [you[0].equip [0]], you[0].inv_quant [you[0].equip [0]], you[0].inv_ident [you[0].equip [0]], 4, str_pass);
strcpy(info, str_pass);
strcat(info, " dances into the air!");
mpr(info);


unwield_item(you[0].equip [0]);

you[0].inv_quant [you[0].equip [0]] = 0;
you[0].equip [0] = -1;
you[0].inv_no --;

menv [summs].m_inv [0] = i;
menv [summs].m_sec = mitm.icol [i];

}


void you_teleport(void)
{

if (you[0].duration [13] != 0)
{
 strcpy(info, "You feel strangely stable.");
 mpr(info);
 you[0].duration [13] = 0;
 return;
}

strcpy(info, "You feel strangely unstable.");
mpr(info);
you[0].duration [13] = 4 + random2(3);

}



void you_teleport2(char allow_control)
{

int plox [2];

if (you[0].attribute [3] != 0 && you[0].level_type != 2 && you[0].conf == 0 && allow_control == 1)
{
 strcpy(info, "You may choose your destination (press '.' or delete to select).");
 mpr(info);
 more();
 plox [0] = 1;
 show_map(plox);

 plox [0] += random2(3) - 1;
 plox [1] += random2(3) - 1;

 if (random2(4) == 0)
 {
  plox [0] += random2(3) - 1;
  plox [1] += random2(3) - 1;
 }

#ifdef DEBUG
strcpy(info, "Target square: ");
itoa(plox [0], st_prn, 10);
strcat(info, st_prn);
strcat(info, ", ");
itoa(plox [1], st_prn, 10);
strcat(info, st_prn);
mpr(info);
#endif

 if (plox [0] < 6 | plox [1] < 6 | plox [0] > 75 | plox [1] > 65)
 {
  strcpy(info, "Oops!");
  mpr(info);
  goto random_teleport;
 }

 you[0].x_pos = plox [0];
 you[0].y_pos = plox [1];

 if (grd [you[0].x_pos] [you[0].y_pos] != 67 | mgrd [you[0].x_pos] [you[0].y_pos] != MNG | env[0].cgrid [you[0].x_pos] [you[0].y_pos] != CNG)
 {
  strcpy(info, "Oops!");
  mpr(info);
  goto random_teleport;
 }

} else
{

        random_teleport : strcpy(info, "Your surroundings suddenly seem different.");
        mpr(info);

        do
        {
                you[0].x_pos = random2(70) + 10;
                you[0].y_pos = random2(60) + 10;
        } while (grd [you[0].x_pos] [you[0].y_pos] != 67 | mgrd [you[0].x_pos] [you[0].y_pos] != MNG | env[0].cgrid [you[0].x_pos] [you[0].y_pos] != CNG);
}

if (you[0].level_type == 2)
{
 abyss_teleport();
 env[0].cloud_no = 0;
 you[0].pet_target = MHITNOT;
}

} /* end of you_teleport */



void entomb(void)
{

char srx = 0;
char sry = 0;

char chance_found = 4;

char number_built = 0;

//if (you[0].clas == 3) chance_found = 2;
for (srx = you[0].x_pos - 1; srx < you[0].x_pos + 2; srx ++)
{
 for (sry = you[0].y_pos - 1; sry < you[0].y_pos + 2; sry ++)
 {

  if (srx == you[0].x_pos && sry == you[0].y_pos) continue;

  if (grd [srx] [sry] != 67 && grd [srx] [sry] != 70 && (grd [srx] [sry] < 75 | grd [srx] [sry] > 78)) continue;
  if (mgrd [srx] [sry] != MNG) continue;

   int objl = igrd [srx] [sry];
   int hrg = 0;

   while(objl != 501)
   {
    if (mitm.iclass [objl] == 12) goto finish_loop; // Wouldn't want to do this (ie destroy an Orb)
    hrg = mitm.ilink [objl];
    objl = hrg;
   }

   objl = igrd [srx] [sry];
   hrg = 0;
//   int objl2;

   while(objl != 501)
   {
//    objl2 = objl;
    hrg = mitm.ilink [objl];
    destroy_item(objl);
    objl = hrg;
//    strcpy(info, "Destroying.");
//    mpr(info);
   }

  if (env[0].cgrid [srx] [sry] != CNG)
  {
   env[0].cloud_type [env[0].cgrid [srx] [sry]] = 0;
   env[0].cgrid [env[0].cloud_x [env[0].cgrid [srx] [sry]]] [env[0].cloud_y [env[0].cgrid [srx] [sry]]] = CNG;
   env[0].cloud_decay [env[0].cgrid [srx] [sry]] = 0;
   env[0].cloud_no--;
  }

  grd [srx] [sry] = 1;
  number_built ++;
  finish_loop: chance_found ++; // this does nothing
 }
}

if (number_built > 0)
{
 strcpy(info, "Walls grow out of the floor around you!");
} else strcpy(info, "Nothing appears to happen.");
mpr(info);

}


void cast_poison_ammo(void)
{

if (you[0].equip [0] == -1 | you[0].inv_class [you[0].equip [0]] != 1 | you[0].inv_dam [you[0].equip [0]] != 0 | you[0].inv_type [you[0].equip [0]] == 0 | you[0].inv_type [you[0].equip [0]] == 5)
{
 mpr("Nothing appears to happen.");
 return;
}

item_name(you[0].inv_plus2 [you[0].equip [0]], you[0].inv_class [you[0].equip [0]], you[0].inv_type [you[0].equip [0]], you[0].inv_dam [you[0].equip [0]], you[0].inv_plus [you[0].equip [0]], you[0].inv_quant [you[0].equip [0]], you[0].inv_ident [you[0].equip [0]], 4, str_pass);
strcpy(info, str_pass);
if (you[0].inv_quant [you[0].equip [0]] == 1) strcat(info, " is covered in a thin film of poison.");
 else strcat(info, " are covered in a thin film of poison.");
mpr(info);

you[0].inv_dam [you[0].equip [0]] = 3;
wield_change = 1;

}


void create_noise2(void)
{

int plox [2];

 strcpy(info, "Choose the noise's source (press '.' or delete to select).");
 mpr(info);
 more();
 plox [0] = 1;
 show_map(plox);

#ifdef DEBUG
strcpy(info, "Target square: ");
itoa(plox [0], st_prn, 10);
strcat(info, st_prn);
strcat(info, ", ");
itoa(plox [1], st_prn, 10);
strcat(info, st_prn);
mpr(info);
#endif

 if (plox [0] < 1 | plox [1] < 1 | plox [0] > 78 | plox [1] > 68)
 {
  mpr("You hear a muffled thud.");
  return;
 }

mpr("You hear a distant voice call your name.");

noisy(30, plox [0], plox [1]);


}

/*
Type recalled:
0 = anything
1 = undead only (Kiku religion ability)
*/
void recall(char type_recalled)
{
int i;
int j = 0;
int k = 1;
int l = MNST - 1;
char recalled = 0;

if (random2(2) == 0)
{
 j = MNST - 1;
 l = 0;
 k = -1;
} /* sometimes goes through monster list backwards */

char empty [2];

for (i = j; i != l; i += k)
{
 if (menv [i].m_class == -1) continue;
 if (menv [i].m_beh != 7) continue;
 if (menv [i].m_class >= MLAVA0) continue;
 if (type_recalled == 1 && mons_holiness(menv [i].m_class) != 1) continue;

 if (empty_surrounds(you[0].x_pos, you[0].y_pos, 67, 0, empty) == 0)
 {
  break; /* time to stop */
 } else
       {
        mgrd [menv [i].m_x] [menv [i].m_y] = MNG;
        menv [i].m_x = empty [0];
        menv [i].m_y = empty [1];
        mgrd [menv [i].m_x] [menv [i].m_y] = i;
        recalled ++;
        if (menv [i].m_ench [2] != 6 | player_see_invis() != 0)
        {
         strcpy(info, "You recall your ");
         strcat(info, monam (menv [i].m_sec, menv [i].m_class, menv [i].m_ench [2], 4));
         strcat(info, ".");
         mpr(info);
        } else recalled --; /* you're not informed if you've recalled an invis creature */
       } /* end else */

}

if (recalled == 0) mpr("Nothing appears to have answered your call.");

}


void portal(void)
{

char dir_sign = 0;

if (you[0].where_are_you != 0) // && you[0].where_are_you < 10)
{
 mpr("This spell doesn't work here.");
 return;
}
if (you[0].level_type != 0)
{
 mpr("This spell doesn't work here.");
 return;
}

if (grd [you[0].x_pos] [you[0].y_pos] != 67)
{
 mpr("You must be on a piece of normal floor to cast this spell.");
 return;
}

mpr("Which direction ('<' for up, '>' for down, 'x' to quit)?");
get_dir : char keyi = get_ch();
if (keyi != '<' && keyi != '>' && keyi != 'x') goto get_dir;
if (keyi == '<' && you[0].your_level == 0)
{
 mpr("You can't go any further upwards with this spell.");
 goto get_dir;
}
if (keyi == '>' && you[0].your_level == 35)
{
 mpr("You can't go any further downwards with this spell.");
 goto get_dir;
}

if (keyi == 'x')
{
 mpr("Okay, then.");
 return;
}

if (keyi == '>') dir_sign = 1; else dir_sign = -1;

mpr("How many levels (1 - 9, 'x' to quit)?");
get_dir2 : keyi = get_ch();
if (keyi < 49 && keyi > 57 && keyi != 'x') goto get_dir2;

if (keyi == 'x')
{
 mpr("Okay, then.");
 return;
}

keyi -= 48;

int target_level = you[0].your_level + keyi * dir_sign;

if (you[0].where_are_you == 0)
{
 if (target_level < 0) target_level = 0;
 if (target_level > 35) target_level = 35;
}

mpr("You fall through a mystic portal, and materialise at the foot of a staircase.");
more();
int old_lev = you[0].your_level;
you[0].your_level = target_level - 1;
grd [you[0].x_pos] [you[0].y_pos] = 82;

down_stairs(1, old_lev);

}

void cast_death_channel(int pow)
{

mpr("You feel a great wave of evil energy pour through your body.");

you[0].duration [19] += 15 + random2((pow / 3) + 1);
if (you[0].duration [19] > 30) you[0].duration [19] = 30;

}

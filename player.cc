#include "config.h"

#ifdef DOS
#include <conio.h>
#endif

#include <string.h>
#include <stdlib.h>

#include "externs.h"

#include "itemname.h"
#include "misc.h"
#include "mstruct.h"
#include "output.h"
#include "player.h"
#include "priest.h"
#include "religion.h"
#include "skills2.h"
#include "spells0.h"
#include "stuff.h"
#include "view.h"

/*
you[0].duration []:
0 - liquid flames
1 - icy armour
2 - deflect missiles
3 - prayer
4 - regeneration
5 - vorpal blade
6 - fire brand
7 - ice brand
8 - lethal infusion
9 - swiftness
10 - insulation
11 - stonemail
12 - controlled flight
13 - teleport
14 - control teleport
15 - poison weapon
16 - resist poison
17 - breathe something
18 - transformation (duration)
19 - death channel
*/

/* attributes
0 - resist lightning
1 - spec_air
2 - spec_earth
3 - control teleport
4 - walk slowly (eg naga)
5 - transformation (form)
*/

/* armour list
0 - wielded

1 - cloak
2 - helmet
3 - gloves
4 - boots
5 - shield
6 - body armour

7 - ring 0
8 - ring 1
9 - amulet
*/


/* Contains functions which return various player state vars,
and other stuff related to the player. */

int species_exp_mod(char species);
void ability_increase(void);
void increase_stats(char which_stat);


int player_teleport(void)
{
 int tp = 0;
/* rings */
 if (you[0].equip [7] != -1 && you[0].inv_type [you[0].equip [7]] == 10) tp += 8;
 if (you[0].equip [8] != -1 && you[0].inv_type [you[0].equip [8]] == 10) tp += 8;
/* mutations */
 tp += you[0].mutation [22] * 3;
 return tp;
}

int player_regen(void)
{
 int rr = you[0].hp_max / 3;
 if (rr > 20) rr = 20 + ((rr - 20) / 2);
 /* rings */
 if (you[0].equip [7] != -1 && you[0].inv_type [you[0].equip [7]] == 0) rr += 40;
 if (you[0].equip [8] != -1 && you[0].inv_type [you[0].equip [8]] == 0) rr += 40;
 /* spell */
 if (you[0].duration [4] != 0) rr += 100;
/* troll leather */
 if (you[0].equip [6] != -1 && you[0].inv_type [you[0].equip [6]] == 19) rr += 30;
  else /* trolls gain no benefit from troll leather */
   if (you[0].species == 16) rr += 40;
/* fast heal mutation */
 rr += you[0].mutation [15] * 20;
 return rr;
}


int player_res_magic(void)
{
 int rm = you[0].xl * 3;

 switch(you[0].species)
 {
  case 3: // high elf
  case 4: // grey elf
  case 6: // sludge elf
  case 7: // hill dwarf
  case 8: // mountain dwarf
  rm += you[0].xl;
  break;
  case 13: // Naga
  rm += you[0].xl * 2;
  break;
  case 24: // purple drac
  case 14: // Gnome
  case 5: // deep elf
  rm += you[0].xl * 3;
  break;
 }

/* armour: (checks cloak only) */
 if (you[0].equip [1] != -1 && you[0].inv_dam [you[0].equip [1]] % 30 == 12) rm += 30;
/* rings of magic resistance */
 if (you[0].equip [7] != -1 && you[0].inv_type [you[0].equip [7]] == 20) rm += 40;
 if (you[0].equip [8] != -1 && you[0].inv_type [you[0].equip [8]] == 20) rm += 40;
/* Enchantment skill */
 rm += you[0].skills [27] * 2;
/* Mutations */
 rm += you[0].mutation [23] * 30;
/* transformations */
 switch(you[0].attribute [5])
 {
  case 6: rm += 50; break; /* Lich */
 }
 return rm;
}
/*        case 18: strcat(glog , "dragon scale mail"); break;
        case 19: strcat(glog , "troll leather armour"); break;
        case 20: strcat(glog , "ice dragon hide"); break;
        case 21: strcat(glog , "ice dragon scale mail"); break;

        case 22: strcat(glog , "steam dragon hide"); break;
        case 23: strcat(glog , "steam dragon scale mail"); break;
        case 24: strcat(glog , "mottled dragon hide"); break;
        case 25: strcat(glog , "mottled dragon scale mail"); break;
        case 26: strcat(glog , "storm dragon hide"); break;
        case 27: strcat(glog , "storm dragon scale mail"); break;
        case 28: strcat(glog , "gold dragon hide"); break;
        case 29: strcat(glog , "gold dragon scale mail"); break;

   case 1: strcat(glog, " of running"); break;
   case 2: strcat(glog, " of fire resistance"); break;
   case 3: strcat(glog, " of cold resistance"); break;
   case 4: strcat(glog, " of poison resistance"); break;
   case 5: strcat(glog, " of see invisible"); break;
   case 6: strcat(glog, " of darkness"); break;
   case 7: strcat(glog, " of strength"); break;
   case 8: strcat(glog, " of dexterity"); break;
   case 9: strcat(glog, " of intelligence"); break;
  case 10: strcat(glog, " of ponderousness"); break;
  case 11: strcat(glog, " of levitation"); break;
  case 12: strcat(glog, " of magic resistance"); break;
  case 13: strcat(glog, " of protection"); break;
  case 14: strcat(glog, " of stealth"); break;
  case 15: strcat(glog, " of resistance"); break;
  case 16: strcat(glog, " of positive energy"); break;
  case 17: strcat(glog, " of the Archmagi"); break;
  case 18: strcat(glog, " of preservation"); break;

        case 0: strcat(glog , "ring of regeneration"); break;
        case 1: strcat(glog , "ring of protection"); break;
        case 2: strcat(glog , "ring of protection from fire"); break;
        case 3: strcat(glog , "ring of poison resistance"); break;
        case 4: strcat(glog , "ring of protection from cold"); break;
        case 5: strcat(glog , "ring of strength"); break;
        case 6: strcat(glog , "ring of slaying"); break;
        case 7: strcat(glog , "ring of see invisible"); break;
        case 8: strcat(glog , "ring of invisibility"); break;
        case 9: strcat(glog , "ring of hunger"); break;
        case 10: strcat(glog , "ring of teleportation"); break;
        case 11: strcat(glog , "ring of evasion"); break;
 case 12: strcat(glog , "ring of sustain abilities"); break;
 case 13: strcat(glog , "ring of sustenance"); break;
 case 14: strcat(glog , "ring of dexterity"); break;
 case 15: strcat(glog , "ring of intelligence"); break;
 case 16: strcat(glog , "ring of wizardry"); break;
 case 17: strcat(glog , "ring of magical power"); break;
 case 18: strcat(glog , "ring of levitation"); break;
 case 19: strcat(glog , "ring of life protection"); break;
 case 20: strcat(glog , "ring of protection from magic"); break;
 case 21: strcat(glog , "ring of fire"); break;
 case 22: strcat(glog , "ring of ice"); break;
 case 23: strcat(glog , "ring of teleport control"); break;
 case 35: strcat(glog , "amulet of rage"); break;
 case 36: strcat(glog , "amulet of maintain speed"); break; // not foolproof
 case 37: strcat(glog , "amulet of clarity"); break; // not foolproof
 case 38: strcat(glog , "amulet of warding"); break;
 case 39: strcat(glog , "amulet of resist corrosion"); break;
 case 40: strcat(glog , "amulet of the gourmand"); break;
 case 41: strcat(glog , "amulet of conservation"); break;
 case 42: strcat(glog , "amulet of controlled flight"); break;
 case 43: strcat(glog , "amulet of inaccuracy"); break;
 }
*/


int player_res_fire(void)
{
 int rf = 100;
/* rings of fire resistance/fire */
 if (you[0].equip [7] != -1 && (you[0].inv_type [you[0].equip [7]] == 2 | you[0].inv_type [you[0].equip [7]] == 21)) rf ++;
 if (you[0].equip [8] != -1 && (you[0].inv_type [you[0].equip [8]] == 2 | you[0].inv_type [you[0].equip [8]] == 21)) rf ++;
/* rings of ice */
 if (you[0].equip [7] != -1 && you[0].inv_type [you[0].equip [7]] == 22) rf --;
 if (you[0].equip [8] != -1 && you[0].inv_type [you[0].equip [8]] == 22) rf --;
/* Staves */
 if (you[0].equip [0] != -1 && you[0].inv_class [you[0].equip [0]] == 11 && you[0].inv_type [you[0].equip [0]] == 2) rf ++;
/* armour: (checks body armour only) */
 if (you[0].equip [6] != -1 && (you[0].inv_dam [you[0].equip [6]] % 30 == 2 | you[0].inv_dam [you[0].equip [6]] % 30 == 15)) rf ++;
/* DSMails */
 if (you[0].equip [6] != -1 && you[0].inv_type [you[0].equip [6]] == 18) rf += 2;
 if (you[0].equip [6] != -1 && you[0].inv_type [you[0].equip [6]] == 29) rf ++;
 if (you[0].equip [6] != -1 && you[0].inv_type [you[0].equip [6]] == 21) rf --;
/* Mummies are highly flammable */
 if (you[0].species == 12) rf --;
/* Mutations */
 rf += you[0].mutation [12];
/* Red drac */
 if (you[0].species == 18 && you[0].xl >= 18) rf ++;
/* transformations */
 switch(you[0].attribute [5])
 {
  case 4: rf -= 1; break; /* Ice beast */
  case 5: rf += 2; break; /* Dragon */
 }
 return rf;
}

int player_res_cold(void)
{
 int rc = 100;
/* rings of cold resistance/ice */
 if (you[0].equip [7] != -1 && (you[0].inv_type [you[0].equip [7]] == 4 | you[0].inv_type [you[0].equip [7]] == 22)) rc ++;
 if (you[0].equip [8] != -1 && (you[0].inv_type [you[0].equip [8]] == 4 | you[0].inv_type [you[0].equip [8]] == 22)) rc ++;
/* rings of fire */
 if (you[0].equip [7] != -1 && you[0].inv_type [you[0].equip [7]] == 21) rc --;
 if (you[0].equip [8] != -1 && you[0].inv_type [you[0].equip [8]] == 21) rc --;
/* Staves */
 if (you[0].equip [0] != -1 && you[0].inv_class [you[0].equip [0]] == 11 && you[0].inv_type [you[0].equip [0]] == 3) rc ++;
/* armour: (checks body armour only) */
 if (you[0].equip [6] != -1 && (you[0].inv_dam [you[0].equip [6]] % 30 == 3 | you[0].inv_dam [you[0].equip [6]] % 30 == 15)) rc ++;
/* DSMails */
 if (you[0].equip [6] != -1 && you[0].inv_type [you[0].equip [6]] == 21) rc += 2;
 if (you[0].equip [6] != -1 && you[0].inv_type [you[0].equip [6]] == 29) rc ++;
 if (you[0].equip [6] != -1 && you[0].inv_type [you[0].equip [6]] == 18) rc --;
/* Mummies are cold resistant */
 if (you[0].species == 12) rc ++;
/* White drac */
 if (you[0].species == 19 && you[0].xl >= 18) rc ++;
/* Mutations */
 rc += you[0].mutation [13];
/* transformations */
 switch(you[0].attribute [5])
 {
  case 4: rc += 3; break; /* Ice beast */
  case 5: rc --; break; /* Dragon */
  case 6: rc ++; break; /* Lich */
 }
 return rc;
}

int player_res_poison(void)
{
 int rp = 0;
/* rings of poison resistance */
 if (you[0].equip [7] != -1 && you[0].inv_type [you[0].equip [7]] == 3) rp ++;
 if (you[0].equip [8] != -1 && you[0].inv_type [you[0].equip [8]] == 3) rp ++;
/* Staves */
 if (you[0].equip [0] != -1 && you[0].inv_class [you[0].equip [0]] == 11 && you[0].inv_type [you[0].equip [0]] == 4) rp ++;
/* the staff of Olgreb: */
 if (you[0].equip [0] != -1 && you[0].inv_class [you[0].equip [0]] == 0 && you[0].inv_dam [you[0].equip [0]] == 193) rp ++;
/* armour: (checks body armour only) */
 if (you[0].equip [6] != -1 && you[0].inv_dam [you[0].equip [6]] % 30 == 4) rp ++;
/* DSMails */
 if (you[0].equip [6] != -1 && you[0].inv_type [you[0].equip [6]] == 29) rp ++;
/* Mummies and Nagas are poison resistant */
 if (you[0].species == 12 | you[0].species == 13) rp ++;
/* spell of resist poison */
 if (you[0].duration [16] > 0) rp ++;
/* Mutations */
 rp += you[0].mutation [9];
/* Green drac */
 if (you[0].species == 20 && you[0].xl >= 7) rp ++;
/* transformations */
 switch(you[0].attribute [5])
 {
  case 6: rp ++; break; /* Lich */
 }
 return rp;
}

int player_spec_death(void)
{
 int sd = 0;
/* Staves */
 if (you[0].equip [0] != -1 && you[0].inv_class [you[0].equip [0]] == 11 && you[0].inv_type [you[0].equip [0]] == 6) sd ++;
/* Mummies */
 if (you[0].species == 12 && you[0].xl > 12) sd ++;
 if (you[0].species == 12 && you[0].xl > 26) sd ++;
/* armour of the Archmagi (checks body armour only) */
 if (you[0].equip [6] != -1 && you[0].inv_dam [you[0].equip [6]] % 30 == 17) sd ++;
/* transformations */
 switch(you[0].attribute [5])
 {
  case 6: sd ++; break; /* Lich */
 }
 return sd;
}

int player_spec_holy(void)
{
 return 0;
/* if (you[0].clas == 2 | you[0].clas == 6) return 1;
 return 0;*/
}

int player_spec_fire(void)
{
 int sf = 0;
/* Staves */
 if (you[0].equip [0] != -1 && you[0].inv_class [you[0].equip [0]] == 11 && you[0].inv_type [you[0].equip [0]] == 2) sf ++;
/* rings of fire */
 if (you[0].equip [7] != -1 && you[0].inv_type [you[0].equip [7]] == 21) sf ++;
 if (you[0].equip [8] != -1 && you[0].inv_type [you[0].equip [8]] == 21) sf ++;
 return sf;
}

int player_spec_cold(void)
{
 int sc = 0;
/* Staves */
 if (you[0].equip [0] != -1 && you[0].inv_class [you[0].equip [0]] == 11 && you[0].inv_type [you[0].equip [0]] == 3) sc ++;
/* rings of fire */
 if (you[0].equip [7] != -1 && you[0].inv_type [you[0].equip [7]] == 22) sc ++;
 if (you[0].equip [8] != -1 && you[0].inv_type [you[0].equip [8]] == 22) sc ++;
 return sc;
}

int player_spec_earth(void)
{
 int se = 0;
/* Staves */
 if (you[0].equip [0] != -1 && you[0].inv_class [you[0].equip [0]] == 11 && you[0].inv_type [you[0].equip [0]] == 26) se ++;
 return se;
}

int player_spec_air(void)
{
 int sa = 0;
/* Staves */
 if (you[0].equip [0] != -1 && you[0].inv_class [you[0].equip [0]] == 11 && you[0].inv_type [you[0].equip [0]] == 25) sa ++;
 return sa;
}

int player_spec_conj(void)
{
 int sc = 0;
/* Staves */
 if (you[0].equip [0] != -1 && you[0].inv_class [you[0].equip [0]] == 11 && you[0].inv_type [you[0].equip [0]] == 7) sc ++;
/* armour of the Archmagi (checks body armour only) */
 if (you[0].equip [6] != -1 && you[0].inv_dam [you[0].equip [6]] % 30 == 17) sc ++;
 return sc;
}

int player_spec_ench(void)
{
 int se = 0;
/* Staves */
 if (you[0].equip [0] != -1 && you[0].inv_class [you[0].equip [0]] == 11 && you[0].inv_type [you[0].equip [0]] == 8) se ++;
/* armour of the Archmagi (checks body armour only) */
 if (you[0].equip [6] != -1 && you[0].inv_dam [you[0].equip [6]] % 30 == 17) se ++;
 return se;
}

int player_spec_summ(void)
{
 int ss = 0;
/* Staves */
 if (you[0].equip [0] != -1 && you[0].inv_class [you[0].equip [0]] == 11 && you[0].inv_type [you[0].equip [0]] == 9) ss ++;
/* armour of the Archmagi (checks body armour only) */
 if (you[0].equip [6] != -1 && you[0].inv_dam [you[0].equip [6]] % 30 == 17) ss ++;
 return ss;
}

int player_spec_poison(void)
{
 int sp = 0;
/* Staves */
 if (you[0].equip [0] != -1 && you[0].inv_class [you[0].equip [0]] == 11 && you[0].inv_type [you[0].equip [0]] == 4) sp ++;
/* the staff of Olgreb: */
 if (you[0].equip [0] != -1 && you[0].inv_class [you[0].equip [0]] == 0 && you[0].inv_dam [you[0].equip [0]] == 193) sp ++;
 return sp;
}



int player_energy(void)
{
 int pe = 0;
/* Staves */
 if (you[0].equip [0] != -1 && you[0].inv_class [you[0].equip [0]] == 11 && you[0].inv_type [you[0].equip [0]] == 5) pe ++;
 return pe;
}

int player_prot_life(void)
{
 int pl = 0;
/* rings of life resistance */
 if (you[0].equip [7] != -1 && you[0].inv_type [you[0].equip [7]] == 19) pl ++;
 if (you[0].equip [8] != -1 && you[0].inv_type [you[0].equip [8]] == 19) pl ++;
/* armour: (checks body armour only) */
 if (you[0].equip [6] != -1 && you[0].inv_dam [you[0].equip [6]] % 30 == 16) pl ++;
/* DSMails
 if (you[0].equip [6] != -1 && you[0].inv_type [you[0].equip [6]] == 29) pl ++;*/
/* Mummies are undead */
 if (you[0].species == 12) pl ++;
/* transformations */
 switch(you[0].attribute [5])
 {
  case 6: pl ++; break; /* Lich */
 }
 return pl;
}

int player_fast_run(void)
{
 int fr = 0;
/* armour: (checks boots only) */
 if (you[0].equip [4] != -1 && you[0].inv_dam [you[0].equip [4]] % 30 == 1) fr ++;
 if (you[0].duration [9] != 0) fr ++;
/* Mutations */
 if (you[0].mutation [24] > 0) fr ++;
/* transformations */
 switch(you[0].attribute [5])
 {
  case 0: break;
  case 1: fr ++; break; /* spider */
 }
 return fr;
}

int player_speed(void)
{
 if (you[0].duration [18] == 3) return 15;
 return 10;
}

int player_AC(void)
{
 int AC = 0;
 int i;
 if (you[0].equip [2] != -1 && you[0].inv_plus2 [you[0].equip [2]] >= 2)
 {
/*      if (you[0].inv_plus [armour_wear_2] > 130)
                AC += (you[0].inv_plus [armour_wear_2] - 150);
         else AC += you[0].inv_plus [armour_wear_2] - 50;*/

        AC --; /* no AC points for hats */
 }

 for (i = 1; i < 7; i ++)
 {
  if (you[0].equip [i] == -1) continue;
  if (i == 5) continue;
  if (you[0].inv_plus [you[0].equip [i]] > 130)
   AC += you[0].inv_plus [you[0].equip [i]] - 150; //property [you[0].inv_class [armour_wear_2]] [you[0].inv_type [armour_wear_2]] [0] + (you[0].inv_plus [armour_wear_2] - 150);*/
    else AC += you[0].inv_plus [you[0].equip [i]] - 50; //property [you[0].inv_class [armour_wear_2]] [you[0].inv_type [armour_wear_2]] [0] + you[0].inv_plus [armour_wear_2] - 50;
  if (i == 2 && you[0].inv_plus2 [you[0].equip [i]] >= 2) continue;

  AC += property(2, you[0].inv_type [you[0].equip [i]], 0) * (15 + you[0].skills [13]) / 15;
  if ((you[0].species == 13 | you[0].mutation [26] > 0) && i == 6) /* Nagas/the deformed don't fit into body armour very well */
  {
   AC -= property(2, you[0].inv_type [you[0].equip [i]], 0) / 2;
  }
 }

 if (you[0].equip [8] != -1 && you[0].inv_type [you[0].equip [8]] == 1)
 {
  if (you[0].inv_plus [you[0].equip [8]] > 130) AC -= 100;
  AC += you[0].inv_plus [you[0].equip [8]];
  AC -= 50;
 }
 if (you[0].equip [7] != -1 && you[0].inv_type [you[0].equip [7]] == 1)
 {
  if (you[0].inv_plus [you[0].equip [7]] > 130) AC -= 100;
  AC += you[0].inv_plus [you[0].equip [7]];
  AC -= 50;
 }
 if (you[0].equip [0] != -1 && you[0].inv_dam [you[0].equip [0]] % 30 == 7)
  AC += 5;
 if (you[0].equip [5] != -1 && you[0].inv_dam [you[0].equip [5]] % 30 == 13)
  AC += 3;
 if (you[0].species == 13) AC += you[0].xl / 3; /* naga */
 if (you[0].species == 22 && you[0].xl > 7) AC += (you[0].xl - 7) / 2; /* grey drac */
 if (you[0].species == 22 && you[0].xl >= 4) AC ++; /* grey drac */
 if (you[0].species >= 18 && you[0].species <= 29 && you[0].species != 22) AC += you[0].xl / 4; /* all dracs, exc grey */
 if (you[0].species == 15) AC ++; /* ogre */
 if (you[0].species == 16) AC += 3; /* troll */

 if (you[0].duration [1] > 0) AC += 4; // ice armour
 if (you[0].duration [11] > 0) AC += 7; // stonemail

/* mutations */
 AC += you[0].mutation [0];
 if (you[0].mutation [4] > 0) AC += you[0].mutation [4] * 2 - 1;
 AC += you[0].mutation [5] * 3;
 AC += you[0].mutation [6];
 if (you[0].mutation [7] > 0) AC += you[0].mutation [7] + 1;

/* transformations */
 switch(you[0].attribute [5])
 {
  case 0: break;
  case 1: AC += 2; break; /* spider */
  case 3: AC += 20; break; /* statue */
  case 4: AC += 2; break; /* Ice beast */
  case 5: AC += 7; break; /* Dragon */
  case 6: AC += 3; break; /* Lich */
 }

 return AC;
}

int player_evasion(void)
{
 int ev = 10;
 int ev_change = 0;

 ev_change = property(2, you[0].inv_type [you[0].equip [6]], 1);
 ev_change += you[0].skills [13] / 3;
 if (ev_change > property(2, you[0].inv_type [you[0].equip [6]], 1) / 3) ev_change = property(2, you[0].inv_type [you[0].equip [6]], 1) / 3;
 ev += ev_change; /* remember that it's negative */

 if (you[0].equip [8] != -1 && you[0].inv_type [you[0].equip [8]] == 11)
 {
  if (you[0].inv_plus [you[0].equip [8]] > 130) ev -= 100;
  ev += you[0].inv_plus [you[0].equip [8]];
  ev -= 50;
 }
 if (you[0].equip [7] != -1 && you[0].inv_type [you[0].equip [7]] == 11)
 {
  if (you[0].inv_plus [you[0].equip [7]] > 130) ev -= 100;
  ev += you[0].inv_plus [you[0].equip [7]];
  ev -= 50;
 }
 if (you[0].duration [11] > 0) ev -= 2; // stonemail

int emod = 0;

if (you[0].equip [6] != -1)
{
 if (you[0].inv_type [you[0].equip [6]] > 1 && you[0].inv_type [you[0].equip [6]] != 30 && (you[0].inv_type [you[0].equip [6]] < 22 | you[0].inv_type [you[0].equip [6]] > 25) && you[0].inv_dam [you[0].equip [6]] / 30 != 4) emod += (property(2, you[0].inv_type [you[0].equip [6]], 1) * 14) / 10;
/* meaning that the armour evasion modifier is often effectively applied
twice, but not if you're wearing elven armour */
}

emod += you[0].skills [14] / 2; // is this too generous?

if (emod > 0) ev += emod;

#ifdef DEBUG
itoa(emod, st_prn, 10);
strcpy(info, st_prn);
mpr(info);
#endif

 /* repulsion field */
 if (you[0].mutation [8] > 0) ev += you[0].mutation [8] * 2 - 1;

/* transformations */
 switch(you[0].attribute [5])
 {
  case 0: break;
  case 1: ev += 3; break; /* spider */
  case 3: ev -= 5; break; /* statue */
  case 5: ev -= 3; break; /* Dragon */
 }

 return ev;
}

int player_mag_abil(void)
{
 int ma = 0;
/* rings of wizardry */
 if (you[0].equip [7] != -1 && you[0].inv_type [you[0].equip [7]] == 16) ma += 3;
 if (you[0].equip [8] != -1 && you[0].inv_type [you[0].equip [8]] == 16) ma += 3;
/* Staves */
 if (you[0].equip [0] != -1 && you[0].inv_class [you[0].equip [0]] == 11 && you[0].inv_type [you[0].equip [0]] == 0) ma += 4;
/* armour of the Archmagi (checks body armour only) */
 if (you[0].equip [6] != -1 && you[0].inv_dam [you[0].equip [6]] % 30 == 17) ma += 2;
 return ma;
}

int player_shield_class(void)
{

 int base_shield = 0;

 if (you[0].equip [5] == -1) return 0;

 switch (you[0].inv_type [you[0].equip [5]])
 {
  case 13: base_shield += 3; break;
  case 14: base_shield += 7; break;
  case 8: base_shield += 5; break;
 }

 base_shield *= 10 + you[0].skills [17] / 2;
 base_shield /= 10;

        if (you[0].inv_plus [you[0].equip [5]] > 130)
                base_shield += you[0].inv_plus [you[0].equip [5]] - 150;
         else
        base_shield += you[0].inv_plus [you[0].equip [5]] - 50;

 return base_shield;

}

int player_see_invis(void)
{
 int si = 0;
/* rings of see invis */
 if (you[0].equip [7] != -1 && you[0].inv_type [you[0].equip [7]] == 7) si ++;
 if (you[0].equip [8] != -1 && you[0].inv_type [you[0].equip [8]] == 7) si ++;
/* armour: (checks head armour only) */
 if (you[0].equip [6] != -1 && you[0].inv_dam [you[0].equip [6]] % 30 == 5) si ++;
/* Nagas have good eyesight */
 if (you[0].species == 13) si ++;
 return si;
}

int player_sust_abil(void)
{
 int sa = 0;
/* rings of sust abil */
 if (you[0].equip [7] != -1 && you[0].inv_type [you[0].equip [7]] == 12) sa ++;
 if (you[0].equip [8] != -1 && you[0].inv_type [you[0].equip [8]] == 12) sa ++;
/* Mummies have no way to restore stats, so only fair: */
 if (you[0].species == 12) sa ++;
 return sa;
}



int burden_change(void)
{

char old_burden = you[0].burden_state;

you[0].burden = 0;

char bu = 0;

int max_carried = 1000 + you[0].strength * 200;

if (you[0].lev != 0) max_carried += 1000;

if (you[0].duration [11] != 0) you[0].burden += 800; // stonemail

for (bu = 0; bu < 52; bu ++)
{
        if (you[0].inv_quant [bu] < 1) continue;


 if (you[0].inv_class [bu] == 14)
 {
  if (you[0].inv_type [bu] == 0) you[0].burden += mons_weight(you[0].inv_plus [bu]);
  if (you[0].inv_type [bu] == 1) you[0].burden += mons_weight(you[0].inv_plus [bu]) / 2;
 } else you[0].burden += mass(you[0].inv_class [bu], you[0].inv_type [bu]) * you[0].inv_quant [bu];
}


you[0].burden_state = 0;
you[0].burden_ch = 1;

if (you[0].burden < max_carried - 1000)
{
 you[0].burden_state = 0; /* unencumbered */
 return you[0].burden;
}

if (you[0].burden < max_carried - 500)
{
 you[0].burden_state = 2; /* encumbered */
 if (old_burden != you[0].burden_state)
 {
  strcpy(info, "You are being weighed down by all of your possessions.");
  mpr(info);
 }
 return you[0].burden;
}

you[0].burden_state = 5;
if (old_burden != you[0].burden_state)
{
 strcpy(info, "You are being crushed by all of your possessions.");
 mpr(info);
}

return you[0].burden;

}




char you_resist_magic(int power)
{
/*  itoa(power, st_prn, 10);
                strcpy(info, st_prn);
  mpr(info);
  itoa(you[0].res_magic, st_prn, 10);
                strcpy(info, st_prn);
  mpr(info);*/

int ench_power = power;

if (ench_power > 40) ench_power = ((ench_power - 40) / 2) + 40;
if (ench_power > 70) ench_power = ((ench_power - 70) / 2) + 70;
if (ench_power > 90) ench_power = ((ench_power - 90) / 2) + 90;

if (ench_power > 120) ench_power = 120;


int mrchance = 100 + player_res_magic();
mrchance -= ench_power;
int mrch2 = random2(100) + random2(101);

#ifdef DEBUG
strcpy(info, "Pow:");
itoa(ench_power, st_prn, 10);
strcat(info, st_prn);
strcat(info, ", mrs: ");
itoa(you[0].res_magic, st_prn, 10);
strcat(info, st_prn);
strcat(info, ", mrchance:");
itoa(mrchance, st_prn, 10);
strcat(info, st_prn);
strcat(info, ", mrch2:");
itoa(mrch2, st_prn, 10);
strcat(info, st_prn);
incrl();
#endif

if (mrch2 < mrchance) return 0; // ie saved successfully

return 1;



 if (random2(power) / 3 + random2(power) / 3 + random2(power) / 3 >= player_res_magic()) return 0;
 return 1;
}



void forget_map(char chance_forgotten)
{
char xcount = 0;
char ycount = 0;

for (xcount = 0; xcount < 80; xcount ++)
{
 for (ycount = 0; ycount < 70; ycount ++)
 {
  if (random2(100) < chance_forgotten) env[0].map [xcount] [ycount] = 0;
 }
}


}

void how_hungered(int hunge)
{
if (hunge <= 1) return;

if (hunge <= 100)
{
 strcpy(info, "You feel slightly");
 goto full_or_hungry;
}
if (hunge <= 350)
{
 strcpy(info, "You feel somewhat");
 goto full_or_hungry;
}
if (hunge <= 800)
{
 strcpy(info, "You feel a quite a bit");
 goto full_or_hungry;
}
 strcpy(info, "You feel a lot");

full_or_hungry:
if (you[0].hung_state >= 4)
{
 strcat(info, " less full.");
 mpr(info);
 return;
}
strcat(info, " more hungry.");
mpr(info);
return;

}


void gain_exp(unsigned int exp_gained)
{

 if (you[0].equip [6] != -1)
 {
  if (you[0].inv_dam [you[0].equip [6]] % 30 == 17) return; // robe of archmagi
 }

 if (you[0].xp + exp_gained > 8999999)
    you[0].xp = 8999999; else
 you[0].xp += exp_gained;
 if (you[0].exp_available + exp_gained > 20000)
    you[0].exp_available = 20000; else
 you[0].exp_available += exp_gained;
 level_change();
}




void level_change(void) // Look at this !!!!
{

#ifdef CLASSES
/*
case 0: // fighter
case 1: // wizard
case 2: // priest
case 3: // thief
case 4: // Gladiator
case 5: // necromancer
case 6: // paladin
case 7: // Assassin
case 8: // Barbarian
case 9: // Ranger
case 10: // Conjurer
case 11: // Enchanter
case 12: // Fire Wizard
case 13: // Ice Wizard
case 14: // Summoner
*/

// total about 250
int lev_skill [19] [8] [3] =
{
{ // Fighter
{0, 0, 80},
{1, 7, 70},
{8, 12, 50},
{13, 17, 50},
{0, 0, 0},
{0, 0, 0},
{0, 0, 0},
{0, 0, 0}
},
{ // Wizard
{1, 7, 20},
{14, 15, 50},
{8, 12, 20},
{26, 49, 90},
{25, 25, 70},
{0, 0, 0},
{0, 0, 0},
{0, 0, 0}
},
{ // Priest - only gets 230 points
{0, 0, 50},
{1, 7, 40},
{13, 13, 30},
{17, 17, 30},
{25, 25, 50},
{32, 32, 30}, // Div
{0, 0, 0},
{0, 0, 0}
},
{ // Thief
{0, 0, 40},
{1, 7, 50},
{14, 16, 80},
{18, 18, 30},
{25, 25, 20},
{8, 12, 30},
{0, 0, 0},
{0, 0, 0}
},
{ // Glad
{0, 0, 80},
{1, 7, 90},
{13, 17, 80},
{0, 0, 0},
{0, 0, 0},
{0, 0, 0},
{0, 0, 0},
{0, 0, 0}
},
{ // Necromancer
{1, 7, 20},
{14, 15, 40},
{8, 12, 20},
{26, 49, 40},
{25, 25, 40},
{29, 29, 90},
{0, 0, 0},
{0, 0, 0}
},
{ // Paladin - only gets 240 points
{0, 0, 60},
{1, 7, 60},
{13, 13, 45},
{17, 17, 45},
{25, 25, 30},
{0, 0, 0},
{0, 0, 0},
{0, 0, 0}
},
{ // Assassin
{0, 0, 60},
{1, 7, 80},
{14, 16, 60},
{0, 0, 0},
{25, 25, 20},
{8, 12, 30},
{0, 0, 0},
{0, 0, 0}
},
{ // Berserker
{0, 0, 80},
{1, 7, 40},
{8, 12, 60},
{14, 15, 50},
{17, 17, 20},
{0, 0, 0},
{0, 0, 0},
{0, 0, 0}
},
{ // Ranger
{0, 0, 80},
{1, 7, 40},
{8, 12, 100},
{14, 16, 50},
{25, 25, 30},
{0, 0, 0},
{0, 0, 0},
{0, 0, 0}
},
{ // Conjurer
{1, 7, 20},
{14, 15, 40},
{8, 12, 20},
{26, 49, 40},
{25, 25, 40},
{26, 26, 90},
{0, 0, 0},
{0, 0, 0}
},
{ // Enchanter
{1, 7, 20},
{14, 15, 40},
{8, 12, 20},
{26, 49, 30},
{25, 25, 50},
{27, 27, 90},
{0, 0, 0},
{0, 0, 0}
},
{ // Fire Wizard
{1, 7, 20},
{14, 15, 40},
{8, 12, 20},
{26, 49, 50},
{25, 25, 40},
{33, 33, 80},
{0, 0, 0},
{0, 0, 0}
},
{ // Ice wizard
{1, 7, 20},
{14, 15, 40},
{8, 12, 20},
{26, 49, 50},
{25, 25, 40},
{34, 34, 80},
{0, 0, 0},
{0, 0, 0}
},
{ // Summoner
{1, 7, 20},
{14, 15, 40},
{8, 12, 20},
{26, 49, 40},
{25, 25, 40},
{28, 28, 90},
{0, 0, 0},
{0, 0, 0}
},
{ // Air
{1, 7, 20},
{14, 15, 40},
{8, 12, 20},
{26, 49, 50},
{25, 25, 40},
{35, 35, 80},
{0, 0, 0},
{0, 0, 0}
},
{ // Earth
{1, 7, 20},
{14, 15, 40},
{8, 12, 20},
{26, 49, 50},
{25, 25, 40},
{35, 35, 80},
{0, 0, 0},
{0, 0, 0}
},
{ // Crusader
{0, 0, 60},
{1, 7, 60},
{13, 17, 50},
{0, 0, 0},
{25, 25, 40},
{8, 12, 20},
{26, 49, 20},
{0, 0, 0}
},
{ // Death knight
{0, 0, 60},
{1, 7, 60},
{13, 17, 50},
{0, 0, 0},
{25, 25, 30},
{29, 29, 50},
{0, 0, 0},
{0, 0, 0}
}
};
#endif

char temp_quant [10];

//while (you[0].xp > (20 * ((you[0].xl * 0.6) * (you[0].xl * 0.7)) + 10) && you[0].xl < 27)
while (you[0].xp > exp_needed(you[0].xl + 2, you[0].species) && you[0].xl < 27) //(20 * you[0].xl * you[0].xl)) // && you[0].xl < 27)
//while(you[0].xp > exp_needed(you[0].xl))
{
        you[0].xl++;
        strcpy(info, "You are now a level ");
        itoa(you[0].xl, temp_quant, 10);
        strcat(info, temp_quant);
        strcat(info, " ");
        strcat(info, you[0].clasnam);
        strcat(info, "!");
        mpr(info);
        more();
/* scrloc = 0;
switch(you[0].clas)
{
case 0: // fighter
break;
case 1: // wizard
case 5: // necromancer
case 10: // Conjurer
case 11: // Enchanter
case 12: // Fire Wizard
case 13: // Ice Wizard
case 14: // Summoner
break;

case 3: // thief
break;

case 4: // Gladiator
break;

case 2: // priest
if (you[0].xl > you[0].max_level && you[0].xl % 3 == 0)
{
 if (you[0].spell_no >= 21)
 {
  break;
 }
prtry2 = you[0].xl / 3;
if (you[0].xl > 12) prtry2 = (you[0].xl - 12) / 2 + 4;
priest_spells(func_pass, 1);
if (prtry2 >= 8 | func_pass [prtry2] == -1) goto get_out;
prtry = func_pass [prtry2];
for (prtry3 = 0; prtry3 < 25; prtry3 ++)
{
 if (you[0].spells [prtry3] == 210)
 {
  you[0].spells [prtry3] = prtry;
  you[0].spell_no ++;
  you[0].spell_levels -= spell_value(prtry);
  break;
 }
}
strcpy(info, "You have been granted a spell!");
mpr(info);
}
break;


case 6: // paladin
if (you[0].xl > you[0].max_level && you[0].xl % 4 == 0)
{
 if (you[0].spell_no >= 21)
 {
  break; //goto infuse2;
 }
prtry2 = you[0].xl / 4;
if (you[0].xl > 16) prtry2 = (you[0].xl - 16) / 2 + 4;
switch(prtry2)
{
 case 1: prtry = 46; break; // repel undead
 case 2: prtry = 38; break; // lesser healing
 case 3: prtry = 65; break; // heal other
 case 4: prtry = 45; break; // smiting
 case 5: prtry = 39; break; // greater healing
 case 6: prtry = 41; break; // purification
 case 7: prtry = 47; break; // holy word
 default: goto get_out;
}
for (prtry3 = 0; prtry3 < 25; prtry3 ++)
{
 if (you[0].spells [prtry3] == 210)
 {
  you[0].spells [prtry3] = prtry;
  you[0].spell_no ++;
  you[0].spell_levels -= spell_value(prtry);
  break;
 }
}
strcpy(info, "You have been granted a spell!");
mpr(info);
}
get_out : break;


case 7: // Assassin
break;

case 8: // Barbarian
break;

case 9: // Ranger
break;

// etcetera

}*/


#ifdef CLASSES
if (you[0].xl > you[0].max_level)
{
int sklev1 = 0;
int sklev2 = 0;
int ski = 0;
int unapplied_points = 0;

sklev1 = skill_exp_needed(you[0].xl + 2) - skill_exp_needed(you[0].xl + 1);


for (ski = 0; ski < 8; ski ++)
{
 if (lev_skill [you[0].clas] [ski] [2] == 0) continue;
 unapplied_points += add_skill(you, lev_skill [you[0].clas] [ski] [0], lev_skill [you[0].clas] [ski] [1], sklev1 * lev_skill [you[0].clas] [ski] [2] / 100);
}

add_skill(you, 0, 49, unapplied_points);

for (ski = 0; ski < 50; ski ++)
{
 exercise2(ski, 0, 1);
}

}
#endif

// you[0].res_magic += 3;

int brek;

 brek = random2(4) + 3;
 if (you[0].xl > 12) brek = random2(3) + 2;
 if (you[0].xl > 21) brek = random2(2) + 2;
 you[0].hp += brek;
 you[0].base_hp2 += brek;

 you[0].ep += 1;
 you[0].base_ep2 ++;

 if (you[0].spell_levels < 99) you[0].spell_levels ++;

 if (you[0].xl > you[0].max_level && you[0].xl % 3 == 0)
 {
   ability_increase();
 }

if (you[0].xl > you[0].max_level)
{
switch(you[0].species)
{
case 2: // elf
if (you[0].xl % 3 != 0)
{
 you[0].hp_max --;
 you[0].base_hp2 --;
}
if (you[0].xl % 3 == 0)
{
 you[0].base_ep2 ++;
}
if (you[0].xl % 4 == 0) increase_stats(1 + random() % 2);
break;
case 3: // high elf
if (you[0].xl % 3 != 0)
{
 you[0].hp_max --;
 you[0].base_hp2 --;
}
if (you[0].xl % 2 == 0)
{
 you[0].base_ep2 ++;
}
//you[0].res_magic ++;
if (you[0].xl % 3 == 0) increase_stats(1 + random() % 2);
break;
case 4: // grey elf
if (you[0].xl <= 13)
{
 you[0].hp_max --;
 you[0].base_hp2 --;
}
if (you[0].xl % 3 != 0)
{
 you[0].base_ep2 ++;
}
//you[0].res_magic ++;
if (you[0].xl % 4 == 0) increase_stats(1 + random() % 2);
break;
case 5: // deep elf
if (you[0].xl <= 16)
{
 you[0].hp_max --;
 you[0].base_hp2 --;
}
if (you[0].xl % 3 == 0)
{
 you[0].hp_max --;
 you[0].base_hp2 --;
}
you[0].base_ep2 ++;
if (you[0].xl % 4 == 0) increase_stats(2);
break;
case 6: // sludge elf
if (you[0].xl % 3 != 0)
{
 you[0].hp_max --;
 you[0].base_hp2 --;
}
if (you[0].xl % 3 == 0)
{
 you[0].base_ep2 ++;
}
if (you[0].xl % 4 == 0) increase_stats(1 + random() % 2);
break;
case 7: // hill dwarf
if (you[0].xl <= 13)
{
 you[0].hp_max ++;
 you[0].base_hp2 ++;
}
if (you[0].xl % 3 != 0)
{
 you[0].hp_max ++;
 you[0].base_hp2 ++;
}
if (you[0].xl % 2 == 0)
{
 you[0].base_ep2 --;
}
if (you[0].xl % 4 == 0) increase_stats(0);
break;
case 8: // mountain dwarf
if (you[0].xl <= 13)
{
 you[0].hp_max ++;
 you[0].base_hp2 ++;
}
if (you[0].xl % 2 == 0)
{
 you[0].hp_max ++;
 you[0].base_hp2 ++;
}
if (you[0].xl % 3 == 0)
{
 you[0].base_ep2 --;
}
if (you[0].xl % 4 == 0) increase_stats(0);
break;
case 9: // halfling
if (you[0].xl % 5 == 0) increase_stats(1); // note: falls through to kobold
case 11: // kobold
if (you[0].xl <= 16)
{
 you[0].hp_max --;
 you[0].base_hp2 --;
}
if (you[0].xl % 2 == 0)
{
 you[0].base_hp2 --;
}
break;
case 10: // hill orc
if (you[0].xl <= 16)
{
 you[0].hp_max ++;
 you[0].base_hp2 ++;
}
if (you[0].xl % 2 == 0)
{
 you[0].base_hp2 ++;
 you[0].hp_max ++;
}
if (you[0].xl % 3 == 0)
{
 you[0].base_ep2 --;
}
if (you[0].xl % 5 == 0) increase_stats(0);
break;
// 11 (kobold) is with halfling
case 12: // Mummy
if (you[0].xl == 13 | you[0].xl == 23)
{
/* you[0].spec_death ++;*/
 strcpy(info, "You feel more in touch with the powers of death.");
 mpr(info);
}
break;
case 13: // Naga
if (you[0].xl <= 13)
{
 you[0].hp_max ++;
 you[0].base_hp2 ++;
}
you[0].base_hp2 ++;
you[0].hp_max ++;
//you[0].res_magic += 2;
if (you[0].xl % 4 == 0) increase_stats(random2(3));
if (you[0].xl % 3 == 0)
{
 strcpy(info, "Your skin feels tougher.");
 mpr(info);
/* player_AC(you) ++;*/
 you[0].AC_ch = 1;
}
break;
case 14: // Gnome
if (you[0].xl <= 12)
{
 you[0].hp_max --;
 you[0].base_hp2 --;
}
if (you[0].xl % 3 == 0)
{
 you[0].hp_max --;
 you[0].base_hp2 --;
}
//you[0].res_magic += 3;
if (you[0].xl % 4 == 0) increase_stats(1 + random2(2));
break;
case 15: // ogre
case 16: // troll
you[0].hp_max ++;
you[0].base_hp2 ++;
if (you[0].xl <= 13)
{
 you[0].hp_max ++;
 you[0].base_hp2 ++;
}
if (you[0].xl % 2 == 0)
{
 you[0].base_hp2 ++;
 you[0].hp_max ++;
}
if (you[0].xl % 3 != 0)
{
 you[0].base_ep2 --;
}
if (you[0].xl % 3 == 0) increase_stats(0);
break;
case 17: // ogre-mage
you[0].hp_max ++;
you[0].base_hp2 ++;
if (you[0].xl <= 13)
{
 you[0].hp_max ++;
 you[0].base_hp2 ++;
}
if (you[0].xl % 5 == 0) increase_stats(random2(2) * 2);
break;
case 18:
case 19:
case 20:
case 21:
/* Grey is later */
case 23:
case 24:
case 25:
case 26:
case 27:
case 28:
case 29: // Draconian
if (you[0].xl == 7)
{
 switch(you[0].species)
 {
  case 18: mpr("Your scales start taking on a fiery red colour."); break;
  case 19: mpr("Your scales start taking on an icy white colour."); break;
  case 20: mpr("Your scales start taking on a green colour.");
  mpr("You feel resistant to poison."); break;
  case 21: mpr("Your scales start taking on a golden yellow colour."); break;

  case 23: mpr("Your scales start turning black."); break;
  case 24: mpr("Your scales start taking on a rich purple colour."); break;
  case 25: mpr(""); break;
  case 26: mpr(""); break;
  case 27: mpr(""); break;
  case 28: mpr(""); break;
  case 29: mpr(""); break;
 }
 more();
 char title [40];
 strcpy(title, skill_title(best_skill(0, 50, 99), you[0].skills [best_skill(0, 50, 99)], you[0].clas, you[0].xl));
 draw_border(BROWN, you[0].your_name, title, you[0].species);
 you[0].hp_ch = 1;
 you[0].ep_ch = 1;
 you[0].strength_ch = 1;
 you[0].intel_ch = 1;
 you[0].dex_ch = 1;
 you[0].AC_ch = 1;
 you[0].evasion_ch = 1;
 you[0].gp_ch = 1;
 you[0].xp_ch = 1;
 you[0].hung_ch = 1;
 you[0].burden_ch = 1;
 print_stats();
 new_level();
}
if (you[0].xl == 18)
{
 switch(you[0].species)
 {
  case 18: mpr("You feel resistant to fire."); break;
  case 19: mpr("You feel resistant to cold."); break;
  case 23: mpr("You feel resistant to electrical energy.");
  you[0].attribute [0] ++; break;
 }
}

if (you[0].xl % 3 == 0)
{
 you[0].hp_max += 1;
 you[0].base_hp2 += 1;
}
if (you[0].xl % 4 == 0) /* need to add to player_AC */
{
 mpr("Your scales feel tougher.");
 you[0].AC_ch = 1;
}
//if (you[0].xl % 5 == 0) increase_stats(random2(3));
break;

case 22: /* grey drac */
if (you[0].xl == 7)
{
 mpr("Your scales start turning grey.");
 more();
 char title [40];
 strcpy(title, skill_title(best_skill(0, 50, 99), you[0].skills [best_skill(0, 50, 99)], you[0].clas, you[0].xl));
 draw_border(BROWN, you[0].your_name, title, you[0].species);
 you[0].hp_ch = 1;
 you[0].ep_ch = 1;
 you[0].strength_ch = 1;
 you[0].intel_ch = 1;
 you[0].dex_ch = 1;
 you[0].AC_ch = 1;
 you[0].evasion_ch = 1;
 you[0].gp_ch = 1;
 you[0].xp_ch = 1;
 you[0].hung_ch = 1;
 you[0].burden_ch = 1;
 print_stats();
 new_level();
}
if (you[0].xl % 3 == 0)
{
 you[0].hp_max += 1;
 you[0].base_hp2 += 1;
 if (you[0].xl > 7)
 {
  you[0].hp_max += 1;
  you[0].base_hp2 += 1;
 }
}
if (you[0].xl == 4 | (you[0].xl > 7 && you[0].xl % 2 == 0)) /* need to add to player_AC */
{
 mpr("Your scales feel tougher.");
 you[0].AC_ch = 1;
}
if (you[0].xl > 7 && you[0].xl % 4 == 0) increase_stats(random2(2) * 2);
break;
}
}


if (you[0].hp > you[0].hp_max) you[0].hp = you[0].hp_max;
if (you[0].ep > you[0].ep_max) you[0].ep = you[0].ep_max;
if (you[0].ep <= 0) you[0].ep = 0;

calc_hp();
calc_ep();

 if (you[0].xl > you[0].max_level) you[0].max_level = you[0].xl;



/*
if (you[0].xl < 5) you[0].rate_regen += 2;
if (you[0].xl > 4 && you[0].xl <= 10 && you[0].xl % 2 == 0) you[0].rate_regen += 2;
if (you[0].xl > 10 && you[0].xl % 2 == 0) you[0].rate_regen ++;
*/

you[0].hp_ch = 1; you[0].ep_ch = 1;

if (you[0].religion == 5) Xom_acts(1, you[0].xl, 1);


}

you[0].xp_ch = 1;

}


void increase_stats(char which_stat)
{

switch(which_stat)
{
 case 0:
 you[0].strength ++;
 you[0].max_strength ++;
 you[0].strength_ch = 1;
 strcpy(info, "You feel stronger.");
 mpr(info);
 break;
 case 1:
 you[0].dex ++;
 you[0].max_dex ++;
 you[0].dex_ch = 1;
 strcpy(info, "You feel agile.");
 mpr(info);
 break;
 case 2:
 you[0].intel ++;
 you[0].max_intel ++;
 you[0].intel_ch = 1;
 strcpy(info, "You feel clever.");
 mpr(info);
 break;
}

}

int check_stealth(void)
{

int stealth = 20 + you[0].skills [15] * 8;

if (you[0].burden_state == 2) stealth /= 2;
if (you[0].burden_state == 5) stealth /= 5;

if (you[0].equip [6] != -1)
{
 if (you[0].inv_type [you [0].equip [6]] > 1 && you[0].inv_type [you [0].equip [6]] != 30 && (you[0].inv_type [you[0].equip [6]] < 22 | you[0].inv_type [you[0].equip [6]] > 25)) stealth -= mass(2, you[0].inv_type [you[0].equip [6]]) / 10;
/* reduces stealth if you're wearing heavy non-elven armour */
}

if (you[0].equip [4] != -1)
{
 if (you[0].inv_dam [you[0].equip [4]] % 30 == 14) stealth *= 2;
 if (you[0].inv_dam [you[0].equip [4]] / 30 == 4) stealth += 20; // elven
}

if (you[0].equip [1] != -1)
{
 if (you[0].inv_dam [you[0].equip [1]] / 30 == 4) stealth += 20; // elven
}

if (you[0].lev != 0) stealth += 10;

if (you[0].special_wield == 50) stealth = 0; // shadow lantern

if (stealth <= 0) stealth = 0;

stealth *= 2;

return stealth;

}


void ability_increase(void)
{
unsigned char keyin;

#ifdef DEBUG
return; // that prompt was irritating when gaining levels via '$'
#endif

strcpy(info, "Your experience leads to an increase in your attributes!");
mpr(info);
more();
mesclr();
strcpy(info, "Increase Strength, Intelligence, or Dexterity? ");
mpr(info);

get_key:
keyin = getch();
if (keyin == 0)
{
 getch();
 goto get_key;
}

switch(keyin)
{
case 's':
case 'S':
increase_stats(0);
return;

case 'i':
case 'I':
increase_stats(2);
return;

case 'd':
case 'D':
increase_stats(1);
return;

}

goto get_key;
/* this is an infinite loop because it is reasonable to assume that you're not going to want to leave it prematurely. */

}




void display_char_status(void)
{

if (you[0].is_undead == 0)
 strcpy(info, "You are alive.");
  else strcpy(info, "You are undead.");
mpr(info);

switch(you[0].attribute [5])
{
 case 1: mpr("You are in spider-form."); break;
 case 2: mpr("You have blades for hands."); break;
 case 3: mpr("You are a statue."); break;
}

if (you[0].duration [0] != 0)
{
 strcpy(info, "You are covered in liquid flames.");
 mpr(info);
}
if (you[0].duration [1] != 0)
{
 strcpy(info, "You are protected by an icy shield.");
 mpr(info);
}
if (you[0].duration [2] != 0)
{
 strcpy(info, "You are protected from missiles.");
 mpr(info);
}

if (you[0].duration [3] != 0)
{
 strcpy(info, "You are praying."); // not yet implemented
 mpr(info);
}

if (you[0].duration [4] != 0)
{
 strcpy(info, "You are regenerating.");
 mpr(info);
}

if (you[0].duration [9] != 0)
{
 strcpy(info, "You can move swiftly.");
 mpr(info);
}

if (you[0].duration [10] != 0)
{
 strcpy(info, "You are insulated.");
 mpr(info);
}

if (you[0].duration [11] != 0)
{
 strcpy(info, "You are covered in scales of stone.");
 mpr(info);
}

if (you[0].duration [12] != 0)
{
 strcpy(info, "You can control your flight.");
 mpr(info);
}

if (you[0].duration [13] != 0)
{
 strcpy(info, "You are about to teleport.");
 mpr(info);
}

if (you[0].duration [14] != 0)
{
 strcpy(info, "You can control teleportation.");
 mpr(info);
}

if (you[0].duration [19] != 0)
{
 strcpy(info, "You are channeling the dead.");
 mpr(info);
}


if (you[0].invis != 0)
{
        strcpy(info, "You are invisible.");
        mpr(info);
}

if (you[0].conf != 0)
{
        strcpy(info, "You are confused.");
        mpr(info);
}

if (you[0].paralysis != 0)
{
        strcpy(info, "You are paralysed.");
        mpr(info);
}

if (you[0].slow != 0)
{
        strcpy(info, "You are moving very slowly.");
        mpr(info);
}

if (you[0].haste != 0)
{
        strcpy(info, "You are moving very quickly.");
        mpr(info);
}

if (you[0].might != 0)
{
        strcpy(info, "You are mighty.");
        mpr(info);
}

if (you[0].berserker != 0)
{
        strcpy(info, "You are possessed by a berserker rage.");
        mpr(info);
}


if (you[0].lev != 0)
{
        strcpy(info, "You are hovering above the floor.");
        mpr(info);
}


if (you[0].poison != 0)
{
        strcpy(info, "You are poisoned.");
        mpr(info);
}

if (you[0].deaths_door != 0)
{
        strcpy(info, "You are standing in death's doorway.");
        mpr(info);
}


}



void redraw_skill(char your_name [30], char clasnam [40])
{

char print_it [80];
char print_it2 [42];

int i = 0;
char spaces = 0;

strcpy(print_it, your_name);
strcat(print_it, " the ");
strcat(print_it, clasnam);

strncpy(print_it2, print_it, 39);

for (i = 0; i < 40; i ++)
{
 if (print_it2 [i] == 0) spaces = 1;
 if (spaces == 1) print_it2 [i] = ' ';
}

print_it2 [39] = 0;

textcolor(LIGHTGREY);
#ifdef DOS
window(1,1,80,25);
#endif
gotoxy(40,1);
textcolor(7);
cprintf(print_it2);
}


char *species_name(char speci)
{

if (you[0].species >= 18 && you[0].species <= 29 && you[0].xl < 7)
 return "Draconian"; /* Causes minor problems with ghosts, but nevermind */

switch(speci)
{
 case 1: return "Human";
 case 2: return "Elf";
 case 3: return "High Elf";
 case 4: return "Grey Elf";
 case 5: return "Deep Elf";
 case 6: return "Sludge Elf";
 case 7: return "Hill Dwarf";
 case 8: return "Mountain Dwarf";
 case 9: return "Halfling";
 case 10: return "Hill Orc";
 case 11: return "Kobold";
 case 12: return "Mummy";
 case 13: return "Naga";
 case 14: return "Gnome";
 case 15: return "Ogre";
 case 16: return "Troll";
 case 17: return "Ogre-Mage";
 case 18: return "Red Draconian"; /* Fire */
 case 19: return "White Draconian"; /* Cold */
 case 20: return "Green Draconian"; /* Poison */
 case 21: return "Yellow Draconian"; /* Acid */
 case 22: return "Grey Draconian"; /* Nothing */
 case 23: return "Black Draconian"; /* Elec */
 case 24: return "Purple Draconian"; /* Energy */
 case 25: return "Draconian"; /*  */
 case 26: return "Draconian"; /*  */
 case 27: return "Draconian"; /*  */
 case 28: return "Draconian"; /*  */
 case 29: return "Draconian"; /*  */

}

return "Yak";

}

char wearing_amulet(char which_am)
{

 if (which_am == 42 && (you[0].duration [12] != 0 | (you[0].species >= 18 && you[0].species <= 29) | you[0].attribute [5] == 5)) return 1; // controlled flight - duration [10] : flight spell in operation
 if (which_am == 39 | which_am == 41)
 {
  if (you[0].equip [1] != -1 && you[0].inv_dam [you[0].equip [1]] % 30 == 18)
     return 1;
 }
 if (you[0].equip [9] == -1) return 0;
 if (you[0].inv_type [you[0].equip [9]] == which_am) return 1;

 return 0;

}


int species_exp_mod(char species)
{
switch(species)
{
 case 1: return 10; // human
 case 2: return 12; // elf
 case 3: return 15; // h elf
 case 4: return 14; // g elf
 case 5: return 14; // d e
 case 6: return 12; // s e
 case 7: return 13; // h dw
 case 8: return 13; // m dw
 case 9: return 10; // 1/2
 case 10: return 10; // ho
 case 11: return 10; // kobold
 case 12: return 15; // mummy
 case 13: return 15; // naga
 case 14: return 11; // Gnome
 case 15: return 14; // ogre
 case 16: return 16; // troll
 case 17: return 15; // ogre mage
 case 18:
 case 19:
 case 20:
 case 21:
 case 22:
 case 23:
 case 24:
 case 25:
 case 26:
 case 27:
 case 28:
 case 29: return 16; // draconian

 default: return 0;
}

}

unsigned long exp_needed(int lev, char species)
{
lev --;
unsigned long level = 0;
switch(lev)
{
        case 1: level = 1; break;
        case 2: level = 10; break;
        case 3: level = 35; break;
        case 4: level = 70; break;
        case 5: level = 120; break;
        case 6: level = 250; break;
        case 7: level = 510; break;
        case 8: level = 900; break;
        case 9: level = 1700; break;
        case 10: level = 3500; break;
        case 11: level = 8000; break;
        case 12: level = 20000; break;
/*        case 1: return 1;
        case 2: return 10;
        case 3: return 20;
        case 4: return 45;
        case 5: return 90;
        case 6: return 200;
        case 7: return 410;
        case 8: return 800;
        case 9: return 1400;
        case 10: return 3000;
        case 11: return 7000;
        case 12: return 14000;*/
//      case 13: return 20480;
//      case 14: return 40960;
//      case 15: return 81920;
//      case 16: return 163840;
//      case 17: return 327760;
 default: //return 14000 * (lev - 11);
 level = 20000 * (lev - 11) + ((lev - 11) * (lev - 11) * (lev - 11)) * 130;
 break;
}

return (level - 1) * species_exp_mod(species) / 10;

}

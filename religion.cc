#include "config.h"

#include <stdlib.h>
#include <string.h>

#include "externs.h"

#include "bang.h"
#include "describe.h"
#include "dungeon.h"
#include "effects.h"
#include "itemname.h"
#include "it_use2.h"
#include "misc.h"
#include "monplace.h"
#include "mutation.h"
#include "ouch.h"
#include "player.h"
#include "shopping.h"
#include "spells.h"
#include "spells1.h"
#include "spells3.h"
#include "stuff.h"


char *sacrifice [] =
{
{" glows silver and disappears."},
{" glows a brilliant golden colour and disappears."},
{" rots away in an instant."},
{" "},
{" "}, /* Xom - no sacrifices */
{" "},
{" is consumed in a burst of flame."},
{" is consumed in a roaring column of flame."},
{" glows faintly for a moment, then is gone."},
{" sinks into the altar and is absorbed."},
{" vanishes in a puff of smoke."},
{" evaporates."}
};

char *god_name(int which_god);
char *god_name_long(int which_god);
void gain_piety(char pgn);
void naughty(char type_naughty, int naughtiness);
void excommunication(void);
void god_pitch(unsigned char which_god);
void altar_prayer();
void lose_piety(char pgn);

void pray(void)
{

unsigned char was_praying = you[0].duration [3];

if (you[0].religion != 0 && grd [you[0].x_pos] [you[0].y_pos] == 179 + you[0].religion)
{
 altar_prayer();
} else
if (grd [you[0].x_pos] [you[0].y_pos] >= 180 && grd [you[0].x_pos] [you[0].y_pos] <= 200)
{
 god_pitch(grd [you[0].x_pos] [you[0].y_pos] - 179);
 return;
}


if (you[0].religion == 0 | you[0].religion == 5)
{
strcpy(info, "You spend a moment contemplating the meaning of life.");
mpr(info);
return;
}
strcpy(info, "You offer a prayer to ");
strcat(info, god_name(you[0].religion));
strcat(info, ".");
mpr(info);

you[0].duration [3] = 9 + random2(you[0].piety) / 20 + random2(you[0].piety) / 20;

//if (you[0].clas == 2 | you[0].clas == 6) you[0].duration [3] = 4 + random2(you[0].piety) / 20 + random2(you[0].piety) / 20 + random2(you[0].piety) / 20;

strcpy(info, god_name(you[0].religion));

if (you[0].piety <= 5)
{
strcat(info, " is displeased.");
} else
if (you[0].piety <= 20)
{
strcat(info, " is noncommittal.");
} else
if (you[0].piety <= 40)
{
strcat(info, " is pleased with you.");
} else
if (you[0].piety <= 70)
{
strcat(info, " is most pleased with you.");
} else
if (you[0].piety <= 100)
{
strcat(info, " is greatly pleased with you.");
you[0].duration [3] *= 2;
} else
if (you[0].piety <= 130)
{
strcat(info, " is extremely pleased with you.");
you[0].duration [3] *= 2;
} else
{
strcat(info, " is exalted by your worship!");
you[0].duration [3] *= 3;
}

/*if (you[0].piety <= 20)
{
strcpy(info, "You sense hostility.");
you[0].duration [3] = 0;
} else
if (you[0].piety <= 35)
{
strcat(info, " is most displeased with you.");
you[0].duration [3] = 0;
} else
if (you[0].piety <= 45)
{
strcat(info, " is displeased with you.");
you[0].duration [3] = 0;
} else
if (you[0].piety <= 65)
{
strcat(info, " is noncommittal.");
} else
if (you[0].piety <= 75)
{
strcat(info, " is pleased with you.");
} else
if (you[0].piety <= 90)
{
strcat(info, " is most pleased with you.");
} else
if (you[0].piety <= 110)
{
strcat(info, " favours you.");
you[0].duration [3] *= 2;
} else
if (you[0].piety <= 130)
{
strcat(info, " favours you greatly.");
you[0].duration [3] *= 2;
} else
{
strcat(info, " is exalted by your worship!");
you[0].duration [3] *= 3;
}
*/
mpr(info);

/* itoa(you[0].piety, st_prn, 10);
 strcpy(info, "Debug info - Piety: ");
 strcat(info, st_prn);
 mpr(info);*/

you[0].turnover = 1;

if (was_praying == 0)
{

 if ((you[0].religion == 7 | you[0].religion == 10) && you[0].piety > 130 && random2(you[0].piety) > 120 && random2(4) == 0 && grd [you[0].x_pos] [you[0].y_pos] != 61 && grd [you[0].x_pos] [you[0].y_pos] != 62)
 {
   strcpy(info, god_name(you[0].religion));
   strcat(info, " grants you a gift!");
   mpr(info);
   acquirement(0);
   lose_piety(30 + random2(10) + random2(10));
 }


 if ((you[0].religion == 3 | you[0].religion == 9) && you[0].piety > 130 && random2(you[0].piety) > 100)
 {
  int gift = 0;
  if (you[0].had_item [24] == 0) gift = 24;
  if (you[0].had_item [34] == 0) gift = 34;
  if (you[0].had_item [15] == 0) gift = 15;
  if (you[0].had_item [23] == 0) gift = 23;

  if (you[0].religion == 9) gift = 250;

  if (gift != 0 && (grd [you[0].x_pos] [you[0].y_pos] != 61 && grd [you[0].x_pos] [you[0].y_pos] != 62))
  { /* shouldn't give you something if it's just going to fall in a pool */
   int thing_created = items(1, 10, gift, 1, 1, 250);
   strcpy(info, god_name(you[0].religion));
   strcat(info, " grants you a gift!");
   mpr(info);
   if (you[0].species != 13) mpr("Something appears at your feet!");
    else mpr("Something appears before you!");
   int what_was_there = igrd [you[0].x_pos] [you[0].y_pos];
   mitm.ilink [thing_created] = what_was_there;
   igrd [you[0].x_pos] [you[0].y_pos] = thing_created;
   lose_piety(30 + random2(10) + random2(10));
  }
 }
}

}

char *god_name(int which_god)
{

switch(which_god)
{
/*case 0: strcpy(info, "You aren't religious!"); mpr(info); return;
case 1: strcat(info, "Zin."); break;
case 2: strcat(info, "The Shining One."); break;
case 3: strcat(info, "Elyvilon."); break;
case 4: strcat(info, "Nemelex Xobeh."); break;
case 5: strcat(info, "Jurubetut."); break;
case 6: strcat(info, "Vuhemeti."); break;
case 7: strcat(info, "Okawaru."); break;
case 8: strcat(info, "Kikubaaqudgha."); break;
case 9: strcat(info, "Sif Muna."); break;
case 10: strcat(info, "Trog."); break;*/
case 0: return "You aren't religious!";
case 1: return "Zin";
case 2: return "The Shining One";
case 3: return "Kikubaaqudgha";
case 4: return "Yredelemnul";
case 5: return "Xom";
case 6: return "Vehumet";
case 7: return "Okawaru";
case 8: return "Makhleb";
case 9: return "Sif Muna";
case 10: return "Trog";
case 11: return "Nemelex Xobeh";
case 12: return "Elyvilon";
}

return "Illegal god";

}


char *god_name_long(int which_god)
{

switch(which_god)
{
case 0: return "Nobody";
case 1: return "Zin the Law-Giver";
case 2: return "The Shining One";
case 3: return "Kikubaaqudgha";
case 4: return "Yredelemnul";
case 5: return "Xom of Chaos";
case 6: return "Vehumet";
case 7: return "Okawaru";
case 8: return "Makhleb";
case 9: return "Sif Muna";
case 10: return "Trog";
case 11: return "Nemelex Xobeh";
case 12: return "Elyvilon the Healer";
}

return "Illegal God";

}


void Xom_acts(char niceness, int sever, char force_sever)
{

/*
niceness = 0 - bad, 1 - nice
*/

struct bolt beam [1];

if (sever <= 1) sever = 1;

if (force_sever == 0) sever = random2(sever);

if (sever == 0) return;

okay_try_again : if (niceness == 0 | random2(3) == 0) /* bad things */
{
 if (random2(sever) <= 2) /* this should always be first - it will often be called deliberately, with a low sever value */
 {
  switch(random2(4))
  {
   case 0: mpr("Xom notices you."); break;
   case 1: mpr("Xom's attention turns to you for a moment."); break;
   case 2: mpr("Xom's power touches on you for a moment."); break;
   case 3: mpr("You hear Xom's maniacal laughter."); break;
  }
  miscast_effect(10 + random2(15), random2(15) + 1, random2(100), 0);
  return;
 }
 if (random2(sever) <= 2)
 {
  switch(random2(4))
  {
   case 0: mpr("\"Suffer!\""); break;
   case 1: mpr("Xom's malign attention turns to you for a moment."); break;
   case 2: mpr("Xom's power touches on you for a moment."); break;
   case 3: mpr("You hear Xom's maniacal laughter."); break;
  }
  lose_stat(100, 1 + random2(3));
  return;
 }
 if (random2(sever) <= 3 && you[0].is_undead == 0)
 {
  switch(random2(4))
  {
   case 0: mpr("\"You need some minor adjustments, mortal!\""); break;
   case 1: mpr("\"Let me alter your pitiful body.\""); break;
   case 2: mpr("Xom's power touches on you for a moment."); break;
   case 3: mpr("You hear Xom's maniacal laughter."); break;
  }
  mpr("Your body is suffused with distortional energy.");
  you[0].hp = random2(you[0].hp) + 1;
  if (you[0].hp < you[0].hp_max / 2) you[0].hp = you[0].hp_max / 2;
  you[0].hp_ch = 1;
  if (mutate(100) == 0)
   if (mutate(100) == 0)
    if (mutate(100) == 0)
     if (mutate(100) == 0) mpr("Nothing seems to happen.");
 return;
 }

 if (random2(sever) <= 3 && you[0].is_undead == 0)
 {
  switch(random2(4))
  {
   case 0: mpr("\"You have displeased me, mortal.\""); break;
   case 1: mpr("\"You have grown too confident for your meagre worth.\""); break;
   case 2: mpr("Xom's power touches on you for a moment."); break;
   case 3: mpr("You hear Xom's maniacal laughter."); break;
  }
 if (random2(4) == 0 && player_prot_life() == 0)
 {
  mpr("You feel drained!");
  drain_exp();
  if (random2(sever) > 3) drain_exp();
  if (random2(sever) > 3) drain_exp();
  return;
 } else
 {
  mpr("A wave of agony tears through your body!");
  you[0].hp = you[0].hp / 2 + 1;
 }
 }

 if (random2(sever) <= 3)
 {
  switch(random2(3))
  {
   case 0: mpr("\"Time to have some fun!\""); break;
   case 1: mpr("\"Fight to survive, mortal.\""); break;
   case 2: mpr("You hear Xom's maniacal laughter."); break;
  }
  if (random2(4) != 0)
  {
   create_monster(225 + random2(5), 22, 1, you[0].x_pos, you[0].y_pos, MHITNOT, 250);
   if (random2(2) == 0) create_monster(225 + random2(5), 22, 1, you[0].x_pos, you[0].y_pos, MHITNOT, 250);
   if (random2(2) == 0) create_monster(225 + random2(5), 22, 1, you[0].x_pos, you[0].y_pos, MHITNOT, 250);
   if (random2(2) == 0) create_monster(80 + random2(10), 22, 1, you[0].x_pos, you[0].y_pos, MHITNOT, 250);
   if (random2(2) == 0) create_monster(80 + random2(10), 22, 1, you[0].x_pos, you[0].y_pos, MHITNOT, 250);
  } else dancing_weapon(100, 1); /* nasty, but fun */
  return;
 }

 /* And, finally: (remember, miscast_effect can cause this) */
 if (you[0].your_level == 0)
 {
  switch(random2(3))
  {
   case 0: mpr("\"You have grown too comfortable in your little world, mortal!\""); break;
   case 1: mpr("Xom casts you into the Abyss!"); break;
   case 2: mpr("The world seems to spin as Xom's maniacal laughter rings in your ears."); break;
  }
  banished();
  return;
 }

 if (random2(4) != 0) goto okay_try_again;

 mpr("You hear Xom's maniacal laughter.");
 return;
}
/* ouch(x, 19, y); for killed by Xom message */

/* Okay, now for the nicer stuff (note: these things are not necess nice): */
 if (random2(sever) <= 2)
 {
  switch(random2(4))
  {
   case 0: mpr("\"Go forth and destroy!\""); break;
   case 1: mpr("\"Go forth and destroy, mortal!\""); break;
   case 2: mpr("Xom grants you a minor favour."); break;
   case 3: mpr("Xom smiles on you."); break;
  }
  switch(random2(7))
  {
   case 0: potion_effect(0, 150); break;
   case 1: potion_effect(1, 150); break;
   case 2: potion_effect(2, 150); break;
   case 3: potion_effect(3, 150); break;
   case 4: potion_effect(12, 150); break;
   case 5: if (random2(6) == 0) potion_effect(17, 150); else potion_effect(21, 150); break;
   case 6: potion_effect(21, 150); break;
  }
  return;
 }
 if (random2(sever) <= 4)
 {
  switch(random2(3))
  {
   case 0: mpr("\"Serve the mortal, my children!\""); break;
   case 1: mpr("Xom grants you some temporary aid."); break;
   case 2: mpr("Xom open a gate."); break;
  }
   create_monster(225 + random2(5), 22, 7, you[0].x_pos, you[0].y_pos, you[0].pet_target, 250);
   create_monster(225 + random2(5), 22, 7, you[0].x_pos, you[0].y_pos, you[0].pet_target, 250);
   if (random2(you[0].xl) >= 8) create_monster(225 + random2(5), 22, 7, you[0].x_pos, you[0].y_pos, you[0].pet_target, 250);
   if (random2(you[0].xl) >= 8) create_monster(80 + random2(10), 22, 7, you[0].x_pos, you[0].y_pos, you[0].pet_target, 250);
   if (random2(you[0].xl) >= 8) create_monster(80 + random2(10), 22, 7, you[0].x_pos, you[0].y_pos, you[0].pet_target, 250);
  return;
 }
 if (random2(sever) <= 3)
 {
  switch(random2(3))
  {
   case 0: mpr("\"Take this token of my esteem.\""); break;
   case 1: mpr("Xom grants you a gift!"); break;
   case 2: mpr("Xom's generous nature manifests itself."); break;
  }
  if (grd [you[0].x_pos] [you[0].y_pos] == 61 | grd [you[0].x_pos] [you[0].y_pos] == 62)
  {
   mpr("You hear a splash."); /* How unfortunate. I'll bet Xom feels sorry for you. */
   return;
  }
  int thing_created = items(1, 250, 250, 1, you[0].xl * 2, 250);
  if (you[0].species != 13) mpr("Something appears at your feet!");
   else mpr("Something appears before you!");
  int what_was_there = igrd [you[0].x_pos] [you[0].y_pos];
  mitm.ilink [thing_created] = what_was_there;
  igrd [you[0].x_pos] [you[0].y_pos] = thing_created;
  return;
 }
 if (random2(sever) <= 4)
 {
  switch(random2(3))
  {
   case 0: mpr("\"Serve the mortal, my child!\""); break;
   case 1: mpr("Xom grants you a demonic servitor."); break;
   case 2: mpr("Xom open a gate."); break;
  }
  if (random2(you[0].xl) <= 5) create_monster(220 + random2(5), 0, 7, you[0].x_pos, you[0].y_pos, you[0].pet_target, 250);
   else create_monster(225 + random2(5), 0, 7, you[0].x_pos, you[0].y_pos, you[0].pet_target, 250);
  return;
 }
 if (random2(sever) <= 4)
 {
  switch(random2(4))
  {
   case 0: mpr("\"Take this instrument of destruction!\""); break;
   case 1: mpr("\"You have earned yourself a gift.\""); break;
   case 2: mpr("Xom grants you an implement of death."); break;
   case 3: mpr("Xom smiles on you."); break;
  }
  acquirement(0);
  return;
 }
 if (random2(sever) <= 5 && you[0].is_undead == 0)
 {
  switch(random2(4))
  {
   case 0: mpr("\"You need some minor adjustments, mortal!\""); break;
   case 1: mpr("\"Let me alter your pitiful body.\""); break;
   case 2: mpr("Xom's power touches on you for a moment."); break;
   case 3: mpr("You hear Xom's maniacal chuckling."); break;
  }
  mpr("Your body is suffused with distortional energy.");
  you[0].hp = random2(you[0].hp) + 1;
  if (you[0].hp < you[0].hp_max / 2) you[0].hp = you[0].hp_max / 2;
  you[0].hp_ch = 1;
  give_good_mutation();
  return;
 }
 if (random2(sever) <= 2)
 {
  you[0].attribute [0] ++;
  mpr("Xom hurls a blast of lightning!");
  beam[0].beam_source = MNG;
  beam[0].type = 43;
  beam[0].damage = 130;
  beam[0].flavour = 5;
  beam[0].bx = you[0].x_pos;
  beam[0].by = you[0].y_pos;
  strcpy(beam[0].beam_name, "blast of lightning");
  beam[0].colour = LIGHTCYAN;
  beam[0].thing_thrown = 1; /* your explosion */
  explosion(1, beam);
  you[0].attribute [0] --;
  return;
 }

if (random2(4) != 0) goto okay_try_again;

}

/*
Nemelex should sometimes give a card from a deck.
*/


/*
case 1: return "Zin";
case 2: return "The Shining One";
case 3: return "Kikubaaqudgha";
case 4: return "Yredelemnul";
case 5: return "Xom";
case 6: return "Vehumet";
case 7: return "Okawaru";
case 8: return "Makhleb";
case 9: return "Sif Muna";
case 10: return "Trog";
case 11: return "Nemelex Xobeh";
case 12: return "Elyvilon";
Thing_done:
1 - killed a monster in god's name
2 - killed an undead in god's name
3 - killed a demon in god's name
4 - killed an angel (any time)
5 - killed an angel in god's name (all of these pass HD of monster as pgain)
6 - hacked up a corpse in god's name
7 - offered inanimate stuff at an altar
8 - offered stuff, including at least one corpse at an altar
9 - undead slaves killed a living thing
*/
void done_good(char thing_done, int pgain)
{

if (you[0].religion == 0) return;

switch(thing_done)
{
 case 1: /* killed a living monster in god's name */
 switch(you[0].religion)
 {
  case 12: mpr("Elyvilon did not appreciate that!");
  naughty(3, 10);
  break;
  case 3:
  case 7:
  case 8:
  case 10:
  strcpy(info, god_name(you[0].religion));
  strcat(info, " accepts your kill.");
  mpr(info);
  if (random2(pgain + 18) > 5) gain_piety(1);
  break;
 }
 break;

 case 2: /* killed an undead in god's name */
 switch(you[0].religion)
 {
  case 1:
  case 2:
  case 8:
  strcpy(info, god_name(you[0].religion));
  strcat(info, " accepts your kill.");
  mpr(info);
  if (random2(pgain + 18) > 4) gain_piety(1);
  break;
 }
 break;

 case 3: /* killed a demon in god's name */
 switch(you[0].religion)
 {
  case 1:
  case 2:
  case 8:
  strcpy(info, god_name(you[0].religion));
  strcat(info, " accepts your kill.");
  mpr(info);
  if (random2(pgain + 18) > 3) gain_piety(1);
  break;
 }
 break;

 case 4:
 case 5:
 switch(you[0].religion)
 {
/*case 3:
  case 8:
  strcpy(info, god_name(you[0].religion);
  strcat(info, " accepts your kill with pleasure!");
  gain_piety(random2(2) + 1);
  break;*/
  case 1:
  case 2:
  case 12:
  strcpy(info, god_name(you[0].religion));
  strcat(info, " did not appreciate that!");
  mpr(info);
  if (you[0].conf != 0) naughty(4, 3);
   else naughty(4, pgain * 3);
  break;
 }
 break;

 case 6: /* Note - pgain is you[0].xl, maybe */
 switch(you[0].religion)
 {
  case 3:
  case 8:
  case 10:
  strcpy(info, god_name(you[0].religion));
  strcat(info, " accepts your offering.");
  mpr(info);
  if (random2(pgain + 10) > 5) gain_piety(1);
  break;
  case 1:
  case 2:
  case 12:
  strcpy(info, god_name(you[0].religion));
  strcat(info, " did not appreciate that!");
  mpr(info);
  naughty(7, 8);
  break;
 }
 break;

 case 7:
 strcpy(info, god_name(you[0].religion));
 strcat(info, " is pleased with your offering.");
 mpr(info);
 gain_piety(1);
 break;

 case 9: /* slaves killed a living monster */
 switch(you[0].religion)
 {
  case 3:
  strcpy(info, god_name(you[0].religion));
  strcat(info, " accepts your slave's kill.");
  mpr(info);
  if (random2(pgain + 18) > 5) gain_piety(1);
  break;
 }
 break;

/*
 Offering at altars is covered in another function.
*/

}

}

void gain_piety(char pgn)
{

if (you[0].piety > 100 && random2(3) == 0) return;
if (you[0].piety > 150 && random2(3) == 0) return;
if (you[0].piety > 199) return;

int old_piety = you[0].piety;
you[0].piety += pgn;

if (you[0].piety >= 30 && old_piety < 30)
{
 switch(you[0].religion)
 {
  case 1:
  case 2:
  mpr("You can now repel the undead.");
  break;
  case 3:
  mpr("You can now recall your undead servants at will.");
  break;
  case 7:
  mpr("You can now give your body great, if temporary, strength.");
  break;
  case 8:
  mpr("You can now gain power by killing in Makhleb's name.");
  break;
  case 10:
  mpr("You can now go berserk at will.");
  break;
  case 12:
  mpr("You now have minor healing powers.");
  break;
 }
}

if (you[0].piety >= 50 && old_piety < 50)
{
 switch(you[0].religion)
 {
  case 1:
  mpr("You can now call upon Zin for minor healing.");
  break;
  case 2:
  mpr("You can now smite your foes.");
  break;
  case 3:
  mpr("You now have some protection from the side-effects of necromantic magic.");
  break;
  case 7:
  mpr("You can now call upon Okawaru for minor healing.");
  break;
  case 8:
  mpr("You can now call on Makhleb's destructive powers.");
  break;
  case 9:
  mpr("You can now forget spells at will.");
  break;
  case 10:
  mpr("You can now give your body great, if temporary, strength.");
  break;
  case 12:
  mpr("You can now neutralise the effects of poison.");
  break;
 }
}

if (you[0].piety >= 75 && old_piety < 75)
{
 switch(you[0].religion)
 {
  case 1:
  mpr("You can now call a plague of pestilential beasts upon your enemies!");
  break;
  case 2:
  mpr("You can now dispel the undead.");
  break;
  case 3:
  mpr("You can now permanently enslave the undead.");
  break;
  case 8:
  mpr("You can now summon a lesser servant of Makhleb.");
  break;
  case 12:
  mpr("You now have more powerful healing abilities.");
  break;
 }
}

if (you[0].piety >= 100 && old_piety < 100)
{
 switch(you[0].religion)
 {
  case 1:
  mpr("You can now speak a Holy Word.");
  break;
  case 2:
  mpr("You can now hurl bolts of divine anger.");
  break;
  case 8:
  mpr("You can now call on the greater of Makhleb's destructive powers.");
  break;
  case 9:
  mpr("You now have some protection from the side-effects of magic.");
  break;
  case 10:
  mpr("You can now haste yourself.");
  break;
  case 12:
  mpr("You have gained the power to purify your body.");
  break;
 }
}

if (you[0].piety >= 120 && old_piety < 120)
{
 switch(you[0].religion)
 {
  case 1:
  mpr("You can now summon a guardian angel.");
  break;
  case 2:
  mpr("You can now summon a divine warrior!");
  break;
  case 3:
  mpr("You can now summon an emissary of Death!"); /* ie a reaper */
  break;
  case 7:
  mpr("You can now haste yourself.");
  break;
  case 8:
  mpr("You can now summon a greater servant of Makhleb.");
  break;
  case 12:
  mpr("You now have incredible healing powers.");
  break;
 }
}

}

/*

*/
void naughty(char type_naughty, int naughtiness)
{
/*
 1 = using necromancy (spell or device)
 2 = using unholy stuff (eg call imp, summon horrible things)
 3 = killing in the name of peaceful diety
 4 = attacking holy things
 5 = attacking friendly
 6 = allowing friendly to die
 7 = butchering in the name of peaceful diety
 8 = stabbing
*/

int piety_loss = 0;

switch(type_naughty)
{
 case 1:
 case 2:
 case 4:
 switch(you[0].religion)
 {
  case 1:
  case 2:
  case 12:
  piety_loss = naughtiness;
  break;
 }
 break;

 case 3:
 switch(you[0].religion)
 {
  case 12:
  piety_loss = naughtiness;
  break;
 }
 break;

 case 5:
 case 6:
 switch(you[0].religion)
 {
  case 1:
  case 2:
  case 12:
  case 7:
  piety_loss = naughtiness;
  break;
 }
 break;

 case 7: /* butchering in the name of */
 switch(you[0].religion)
 {
  case 1:
  case 2:
  case 12:
  piety_loss = naughtiness;
  break;
 }
 break;

 case 8: /* stabbing */
 switch(you[0].religion)
 {
  case 2:
  piety_loss = naughtiness;
  break;
 }
 break;


}

if (piety_loss == 0) return;

if (piety_loss == 1)
 mpr("You feel a little guilty.");
  else
   if (piety_loss <= 4)
    mpr("You feel guilty.");
     else
      if (piety_loss <= 9)
       mpr("You feel very guilty.");
        else mpr("You feel extremely guilty.");

lose_piety(piety_loss);

if (you[0].piety <= 0) excommunication();

}


void lose_piety(char pgn)
{

int old_piety = you[0].piety;
you[0].piety -= pgn;

if (you[0].piety < 120 && old_piety >= 120)
{
 switch(you[0].religion)
 {
  case 1:
  mpr("You can no longer summon a guardian angel.");
  break;
  case 2:
  mpr("You can no longer summon a divine warrior.");
  break;
  case 3:
  mpr("You can no longer summon an emissary of Death."); /* ie a reaper */
  break;
  case 8:
  mpr("You can no longer summon a greater servant of Makhleb.");
  break;
  case 12:
  mpr("You no longer have incredible healing powers.");
  break;
 }
}

if (you[0].piety < 100 && old_piety >= 100)
{
 switch(you[0].religion)
 {
  case 1:
  mpr("You can no longer speak a Holy Word.");
  break;
  case 2:
  mpr("You can no longer hurl bolts of divine anger.");
  break;
  case 10:
  mpr("You can no longer haste yourself.");
  break;
  case 8:
  mpr("You can no longer call on Makhleb's greater destructive powers.");
  break;
  case 9:
  mpr("You no longer have special protection from the side-effects of magic.");
  break;
  case 12:
  mpr("You can no longer purify your body.");
  break;
 }
}


if (you[0].piety < 75 && old_piety >= 75)
{
 switch(you[0].religion)
 {
  case 1:
  mpr("You can no longer call a plague.");
  break;
  case 2:
  mpr("You can no longer dispel the undead.");
  break;
  case 3:
  mpr("You can no longer permanently enslave the undead.");
  break;
  case 8:
  mpr("You can no longer summon any servants of Makhleb.");
  break;
  case 12:
  mpr("You lose your powerful healing abilities.");
  break;
 }
}


if (you[0].piety < 50 && old_piety >= 50)
{
 switch(you[0].religion)
 {
  case 1:
  mpr("You can no longer call upon Zin for minor healing.");
  break;
  case 2:
  mpr("You can no longer smite your foes.");
  break;
  case 3:
  mpr("You no longer have any special protection from the side-effects of necromancy.");
  break;
  case 8:
  mpr("You can no longer call on Makhleb's destructive powers.");
  break;
  case 10:
  mpr("You can no longer give your body strength.");
  break;
  case 9:
  mpr("You can no longer forget spells at will.");
  break;
  case 12:
  mpr("You can no longer neutralise the effects of poison.");
  break;
 }
}


if (you[0].piety < 30 && old_piety >= 30)
{
 switch(you[0].religion)
 {
  case 1:
  case 2:
  mpr("You can no longer repel the undead.");
  break;
  case 3:
  mpr("You can no longer recall your undead servants at will.");
  break;
  case 8:
  mpr("You can no longer gain power from your kills.");
  break;
  case 10:
  mpr("You can no longer go berserk at will.");
  break;
  case 12:
  mpr("You have lost your minor healing powers.");
  break;
 }
}


}




void excommunication(void)
{
 mpr("You have lost your religion!");
 if (you[0].religion == 5) Xom_acts(0, you[0].xl * 2, 1);
 you[0].religion = 0;
 you[0].piety = 0;
}

void altar_prayer(void)
{

int item_sacr;
int i, j;
char subst_id [4] [50];


 item_sacr = 501;

for (i = 0; i < 4; i ++)
{
 for (j = 0; j < 50; j ++)
 {
  subst_id [i] [j] = 1;
 }
}

 if (igrd [you[0].x_pos] [you[0].y_pos] == 501 | you[0].religion == 2 | you[0].religion == 5)
 {
  mpr("You kneel at the altar and pray.");
  return;
 }

mpr("You kneel at the altar and pray.");

i = igrd [you[0].x_pos] [you[0].y_pos];

do
{
if (random2(1000) == 0) break;
j = mitm.ilink [i];
switch(you[0].religion)
{
 case 1:
 case 7:
 case 8:
 it_name(i, 0, str_pass);
 strcpy(info, str_pass);
 strcat(info, sacrifice [you[0].religion]);
 mpr(info);
 if (mitm.iclass [i] == 14 | random2(item_value(mitm.iclass [i], mitm.itype [i], mitm.idam [i], mitm.iplus [i], mitm.iplus2 [i], mitm.iquant [i], 3, subst_id)) >= 50)
     gain_piety(1);
 destroy_item(i);
 break;

 case 9:
 it_name(i, 0, str_pass);
 strcpy(info, str_pass);
 strcat(info, sacrifice [you[0].religion]);
 mpr(info);
 if (item_value(mitm.iclass [i], mitm.itype [i], mitm.idam [i], mitm.iplus [i], mitm.iplus2 [i], mitm.iquant [i], 3, subst_id) >= 150)
     gain_piety(1 + random2(4));
 destroy_item(i);
 break;

 case 3:
 case 10:
 if (mitm.iclass [i] != 14) break;
 it_name(i, 0, str_pass);
 strcpy(info, str_pass);
 strcat(info, sacrifice [you[0].religion]);
 mpr(info);
 gain_piety(1);
 destroy_item(i);
 break;

 case 12:
 if (mitm.iclass [i] != 0 && mitm.iclass [i] != 1) break;
 it_name(i, 0, str_pass);
 strcpy(info, str_pass);
 strcat(info, sacrifice [you[0].religion]);
 mpr(info);
 gain_piety(1);
 destroy_item(i);
 break;

}
i = j;
} while (i != 501);

}



void god_pitch(unsigned char which_god)
{

strcpy(info, "You kneel at the altar of ");
strcat(info, god_name(which_god));
strcat(info, ".");
mpr(info);

more();

if (you[0].is_undead != 0 && (which_god == 1 | which_god == 2 | which_god == 12))
{
 strcpy(info, god_name(which_god));
 strcat(info, " does not accept worship from those such as you!");
 mpr(info);
 return;
}

describe_god(which_god);

mpr("Do you wish to join this religion?");
char keyi = get_ch();
if (keyi != 'Y' && keyi != 'y') return;

mpr("Are you sure?");
keyi = get_ch();
if (keyi != 'Y' && keyi != 'y') return;

if (you[0].religion != 0) excommunication();

strcpy(info, god_name(which_god));
strcat(info, " welcomes you!");
mpr(info);

you[0].religion = which_god;
you[0].piety = 15;

}


void offer_corpse(int corpse)
{
 it_name(corpse, 0, str_pass);
 strcpy(info, str_pass);
 strcat(info, sacrifice [you[0].religion]);
 mpr(info);
 done_good(6, 10);
}

#include "config.h"

#include <stdlib.h>
#include <string.h>

#ifdef DOS
#include "conio.h"
#endif

#ifdef LINUX
#include "linuxlib.h"
#endif

#include "externs.h"
#include "message.h"
#include "player.h"

#include "stuff.h"

int how_mutated(void);
char delete_mutation(char which_mutation);
char body_covered(void);

char mut_string [80];

/*char *char_skin[] = {
"You have fair skin, ",
"You have very pale skin, ",
"You have pale freckly skin, ",
"You have olive skin, ",
"You have dark skin, ",
"You have black skin, ",
"You have pink blotchy skin, ",
"You have pale, green-tinged skin, ",
"You have rough brown skin, ",
"You have rough grey skin, ",
"You have yellowish skin, "
"You have red blotchy skin, ",
"You have slimy, scabby grey skin, ",
"You have dessicated, long-dead flesh, ",
"You have ",
"You have "
};

char *char_hair_adj[] = {
"wavy ",
"straight ",
"curly ",
""
};

char *char_hair[] = {
"fair hair, ",
"blond hair, ",
"no hair, ",
"brown hair, ",
"red hair, ",
"black hair, ",
"dark brown hair, ",
"",
"",
""
};


char *odd_legs[] = {
". ",
"",
""
};
*/


char *mutation_descrip [] [3] =
{
{"You have tough skin (AC +1).", "You have very tough skin (AC +2).", "You have extremely tough skin (AC +3)."},
{"Your muscles are strong (Str +", "", ""},
{"Your mind is acute (Int +", "", ""},
{"You are agile (Dex +", "", ""},
{"You are partially covered in green scales (AC +1).", "You are mostly covered in green scales (AC +3).", "You are covered in green scales (AC +5)."},
{"You are partially covered in thick black scales (AC + 3, dex - 1).", "You are mostly covered in thick black scales (AC + 6, dex - 2).", "You are completely covered in thick black scales (AC + 9, dex - 3)."},
{"You are partially covered in supple grey scales (AC + 1).", "You are mostly covered in supple grey scales (AC + 2).", "You are completely covered in supple grey scales (AC + 3)."},
{"You are protected by plates of bone (AC + 2, dex - 1).", "You are protected by plates of bone (AC + 3, dex - 2).", "You are protected by plates of bone (AC + 4, dex - 3)."},
{"You are surrounded by a mild repulsion field (ev + 1).", "You are surrounded by a moderate repulsion field (ev + 3).", "You are surrounded by a strong repulsion field (ev + 5)."},
{"Your system is immune to poisons.", "Your system is immune to poisons.", "Your system is immune to poisons."},
// 10
{"Your digestive system is specialised to digest meat.", "Your digestive system is specialised to digest meat.", "You are primarily a carnivore."},
{"You digest meat inefficiently.", "You digest meat inefficiently.", "You are primarily a herbivore."},
{"Your flesh is heat-resistant.", "Your flesh is very heat-resistant.", "Your flesh is almost immune to the effects of heat."},
{"Your flesh is cold-resistant.", "Your flesh is very cold-resistant.", "Your flesh is almost immune to the effects of cold."},
{"You are immune to electric shocks.", "You are immune to electric shocks.", "You are immune to electric shocks."},
{"Your natural rate of healing is unusually fast.", "You heal very quickly.", "You regenerate."},
{"You have a fast metabolism.", "You have a very fast metabolism.", "Your metabolism is lightning-fast."},
{"You have a slow metabolism.", "You have a slow metabolism.", "You need consume almost no food."},
{"You are weak (Str -", "", ""},
{"You are dopey (Int -", "", ""},
// 20
{"You are clumsy (Dex -", "", ""},
{"You can control translocations.", "You can control translocations.", "You can control translocations."},
{"Space occasionally distorts in your vicinity.", "Space sometimes distorts in your vicinity.", "Space frequently distorts in your vicinity."},
{"You are resistant to magic.", "You are highly resistant to magic.", "You are almost impervious to the effects of magic."},
{"You cover the ground very quickly.", "You cover the ground very quickly.", "You cover the ground very quickly."},
{"You have supernaturally acute eyesight.", "You have supernaturally acute eyesight.", "You have supernaturally acute eyesight."},
{"Armour fits poorly on your deformed body.", "Armour fits poorly on your badly deformed body.", "Armour fits poorly on your hideously deformed body."},
{"", "", ""},
{"", "", ""},
{"", "", ""},
};

/*
future:
Forgetful
*/


char *gain_mutation [] [3] =
{
{"Your skin toughens.", "Your skin toughens.", "Your skin toughens."},
{"You feel stronger.", "You feel stronger.", "You feel stronger."},
{"You feel smarter.", "You feel smarter.", "You feel smarter."},
{"You feel agile.", "You feel agile.", "You feel agile."},
{"Green scales grow over part of your body.", "Green scales spread over more of your body.", "Green scales cover you completely."},
{"Thick black scales grow over part of your body.", "Thick black scales spread over more of your body.", "Thick black scales cover you completely."},
{"Supple grey scales grow over part of your body.", "Supple grey scales spread over more of your body.", "Supple grey scales cover you completely."},
{"You grow protective plates of bone.", "You grow more protective plates of bone.", "You grow more protective plates of bone."},
{"You begin to radiate repulsive energy.", "Your repulsive radiation grows stronger.", "Your repulsive radiation grows stronger."},
{"You feel healthy.", "You feel healthy.", "You feel healthy."},
// 10
{"You hunger for flesh.", "You hunger for flesh.", "You hunger for flesh."},
{"You feel less carnivorous.", "You feel less carnivorous.", "You feel less carnivorous."},
{"You feel a sudden chill.", "You feel a sudden chill.", "You feel a sudden chill."},
{"You feel hot for a moment.", "You feel hot for a moment.", "You feel hot for a moment."},
{"You feel insulated.", "You feel insulated.", "You feel insulated."},
{"You begin to heal more quickly.", "You begin to heal more quickly.", "You begin to regenerate."},
{"You feel a little hungry.", "You feel a little hungry.", "You feel a little hungry."},
{"Your metabolism slows.", "Your metabolism slows.", "Your metabolism slows."},
{"You feel weaker.", "You feel weaker.", "You feel weaker."},
{"You feel less intelligent.", "You feel less intelligent", "You feel less intelligent"},
// 20
{"You feel clumsy.", "You feel clumsy.", "You feel clumsy."},
{"You feel controlled.", "You feel controlled.", "You feel controlled."},
{"You feel weirdly uncertain.", "You feel even more weirdly uncertain.", "You feel even more weirdly uncertain."},
{"You feel resistant to magic.", "You feel more resistant to magic.", "You feel almost impervious to the effects of magic."},
{"You feel quick.", "You feel quick.", "You feel quick."},
{"Your vision sharpens.", "Your vision sharpens.", "Your vision sharpens."},
{"Your body twists and deforms.", "Your body twists and deforms.", "Your body twists and deforms."},
{"", "", ""},
{"", "", ""},
{"", "", ""},
};


char *lose_mutation [] [3] =
{
{"Your skin feels delicate.", "Your skin feels delicate.", "Your skin feels delicate."},
{"You feel weaker.", "You feel weaker.", "You feel weaker."},
{"You feel less intelligent.", "You feel less intelligent", "You feel less intelligent"},
{"You feel clumsy.", "You feel clumsy.", "You feel clumsy."},
{"Your green scales disappear.", "Your green scales recede somewhat.", "Your green scales recede somewhat."},
{"Your black scales disappear.", "Your black scales recede somewhat.", "Your black scales recede somewhat."},
{"Your grey scales disappear.", "Your grey scales recede somewhat.", "Your grey scales recede somewhat."},
{"Your bony plates shrink away.", "Your bony plates shrink.", "Your bony plates shrink."},
{"You feel attractive.", "You feel attractive.", "You feel attractive."},
{"You feel a little less healthy.", "You feel a little less healthy.", "You feel a little less healthy."},
{"You feel able to eat a more balanced diet.", "You feel able to eat a more balanced diet.", "You feel able to eat a more balanced diet."},
{"You feel able to eat a more balanced diet.", "You feel able to eat a more balanced diet.", "You feel able to eat a more balanced diet."},
{"You feel hot for a moment.", "You feel hot for a moment.", "You feel hot for a moment."},
{"You feel a sudden chill.", "You feel a sudden chill.", "You feel a sudden chill."},
{"You feel conductive.", "You feel conductive.", "You feel conductive."},
{"Your rate of healing slows.", "Your rate of healing slows.", "Your rate of healing slows."},
{"Your metabolism slows.", "Your metabolism slows.", "Your metabolism slows."},
{"You feel a little hungry.", "You feel a little hungry.", "You feel a little hungry."},
{"You feel stronger.", "You feel stronger.", "You feel stronger."},
{"You feel smarter.", "You feel smarter.", "You feel smarter."},
// 20
{"You feel agile.", "You feel agile.", "You feel agile."},
{"You feel random.", "You feel uncontrolled.", "You feel uncontrolled."},
{"You feel stable.", "You feel stable.", "You feel stable."},
{"You feel less resistant to magic.", "You feel less resistant to magic.", "You feel vulnerable to magic again."},
{"You feel sluggish.", "You feel sluggish.", "You feel sluggish."},
{"Your vision seems duller.", "Your vision seems duller.", "Your vision seems duller."},
{"Your body's shape seems more normal.", "Your body's shape seems slightly more normal.", "Your body's shape seems slightly more normal."},
{"", "", ""},
{"", "", ""},
{"", "", ""},
};


char mutation_rarity [] =
{
10,
10,
10,
10,
3,
2,
2,
1,
1,
4,
5,
5,
4, // res fire
4, // res cold
2, // res elec
3, // regen
7, // slow meta
10, // fast meta
10, // abil loss
10, // ""
10, // ""
2, // tele control
3, // teleport
5, // res magic
8 // deformation
};


void display_mutations(void)
{

   int i;
   int j = 0;
   char st_prn [5];
#ifdef DOS
   char buffer[4800];
   window(1, 1, 80, 25);
   gettext(1, 1, 80, 25, buffer);
#endif


   clrscr();
   textcolor(WHITE);
        cprintf("                  Mutations & Other Weirdness\n\r");
   textcolor(LIGHTGREY);

   for (i = 0; i < 50; i ++)
   {
    if (you[0].mutation [i] != 0)
    {
     j ++;
     if (i == 1 | i == 2 | i == 3 | i == 18 | i == 19 | i == 20)
     {
      cprintf(mutation_descrip [i] [0]);
      itoa(you[0].mutation [i], st_prn, 10);
      cprintf(st_prn);
      cprintf(").\n\r");
      continue;
     }
     cprintf(mutation_descrip [i] [you[0].mutation [i] - 1]);
     cprintf("\n\r");
    }
   }

   if (j == 0) cprintf("You aren't a mutant.\n\r");

   if (getch() == 0) getch();
#ifdef DOS
   puttext(1, 1, 80, 25, buffer);
#endif

//   cprintf("xxxxxxxxxxxxx");
//   last_requested = 0;
return;


}


char mutate(char which_mutation)
{
char mutat = which_mutation;

//char st_prn [10];

if (you[0].is_undead != 0 | (wearing_amulet(44) != 0 && random2(10) != 0))
 return 0;

if (which_mutation == 100 && random2(15) < how_mutated())
{
 if (random2(5) != 0) return 0;
 return delete_mutation(100);
}

if (which_mutation == 100)
 do
 {
  mutat = random2(27);
  if (random2(1000) == 0) return 0;
 } while ((you[0].mutation [mutat] >= 3 && mutat != 1 && mutat != 2 && mutat != 3 && mutat != 18 && mutat != 19 && mutat != 20) | random2(10) > mutation_rarity [mutat] | you[0].mutation [mutat] > 13);

if (you[0].mutation [mutat] >= 3 && mutat != 1 && mutat != 2 && mutat != 3 && mutat != 18 && mutat != 19 && mutat != 20) return 0;
if (you[0].mutation [mutat] > 13) return 0;
if (mutat == 0 | (mutat >= 4 && mutat <= 7) && body_covered() > 2) return 0;

if ((mutat == 25 | mutat == 9) && you[0].species == 13) return 0; // nagas have see invis and res poison
if (mutat == 15 && you[0].mutation [17] > 0) return 0; /* if you have a slow metabolism, no regen */
if (mutat == 17 && you[0].mutation [15] > 0) return 0; /* if you have a slow metabolism, no regen */


mpr("You mutate.");
/*itoa(mutat, st_prn, 10);
mpr(st_prn);
itoa(you[0].mutation [mutat], st_prn, 10);
mpr(st_prn);*/


        you[0].hp_ch = 1;
        you[0].ep_ch = 1;
        you[0].strength_ch = 1;
        you[0].intel_ch = 1;
        you[0].dex_ch = 1;
        you[0].AC_ch =  1;
        you[0].evasion_ch = 1;
        you[0].xp_ch = 1;
        you[0].gp_ch = 1;
        you[0].hung_ch = 1;



switch(mutat)
{
 case 1:
 if (you[0].mutation [18] > 0)
 {
  delete_mutation(18);
  return 1;
 }
 you[0].strength ++;
 you[0].max_strength ++;
 you[0].strength_ch = 1;
 mpr(gain_mutation [mutat] [0]);
 break;
 case 2:
 if (you[0].mutation [19] > 0)
 {
  delete_mutation(19);
  return 1;
 }
 you[0].intel ++;
 you[0].max_intel ++;
 you[0].intel_ch = 1;
 mpr(gain_mutation [mutat] [0]);
 break;
 case 3:
 if (you[0].mutation [20] > 0)
 {
  delete_mutation(20);
  return 1;
 }
 you[0].dex ++;
 you[0].max_dex ++;
 you[0].dex_ch = 1;
 mpr(gain_mutation [mutat] [0]);
 break;

 case 18:
 if (you[0].mutation [1] > 0)
 {
  delete_mutation(1);
  return 1;
 }
 you[0].strength --;
 you[0].max_strength --;
 you[0].strength_ch = 1;
 mpr(gain_mutation [mutat] [0]);
 break;
 case 19:
 if (you[0].mutation [2] > 0)
 {
  delete_mutation(2);
  return 1;
 }
 you[0].intel --;
 you[0].max_intel --;
 you[0].intel_ch = 1;
 mpr(gain_mutation [mutat] [0]);
 break;
 case 20:
 if (you[0].mutation [3] > 0)
 {
  delete_mutation(3);
  return 1;
 }
 you[0].dex --;
 you[0].max_dex --;
 you[0].dex_ch = 1;
 mpr(gain_mutation [mutat] [0]);
 break;

 case 14: // res elec
 you[0].attribute [0] ++;
 mpr(gain_mutation [mutat] [you[0].mutation [mutat]]);
 break;

 case 16: // fast meta
 if (you[0].mutation [17] > 0)
 {
  delete_mutation(17);
  return 1;
 }
 you[0].hunger_inc ++;
 mpr(gain_mutation [mutat] [you[0].mutation [mutat]]);
 break;

 case 17: // slow meta
 if (you[0].mutation [16] > 0)
 {
  delete_mutation(16);
  return 1;
 }
 if (you[0].mutation [mutat] == 0 | you[0].mutation [mutat] == 2) you[0].hunger_inc --;
 mpr(gain_mutation [mutat] [you[0].mutation [mutat]]);
 break;

 case 21: // control tele
 you[0].attribute [3] ++;
 mpr(gain_mutation [mutat] [you[0].mutation [mutat]]);
 break;

 case 5:
 case 7:
 you[0].dex --;
 you[0].max_dex --;
 you[0].dex_ch = 1;
 default: mpr(gain_mutation [mutat] [you[0].mutation [mutat]]);
 break;
}

you[0].mutation [mutat] ++;

return 1;

}


int how_mutated(void)
{

int i;
int j = 0;

for (i = 0; i < 50; i ++)
{
 j += you[0].mutation [i];
}

return j;

}



char delete_mutation(char which_mutation)
{
char mutat = which_mutation;

if (which_mutation == 100)
 do
 {
  mutat = random2(27);
  if (random2(1000) == 0) return 0;
 } while ((you[0].mutation [mutat] == 0 && mutat != 1 && mutat != 2 && mutat != 3 && mutat != 18 && mutat != 19 && mutat != 20) | random2(10) > mutation_rarity [mutat]);


if (you[0].mutation [mutat] == 0) return 0;

/*mpr("Deleting:");
itoa(mutat, st_prn, 10);
mpr(st_prn);
itoa(you[0].mutation [mutat], st_prn, 10);
mpr(st_prn);*/

mpr("You mutate.");

switch(mutat)
{
 case 1:
/* if (you[0].mutation [18] > 0)
 {
  mutate(18);
  break;
 }*/
 you[0].strength --;
 you[0].max_strength --;
 you[0].strength_ch = 1;
 mpr(lose_mutation [mutat] [0]);
 break;
 case 2:
/* if (you[0].mutation [19] > 0)
 {
  mutate(19);
  break;
 }*/
 you[0].intel --;
 you[0].max_intel --;
 you[0].intel_ch = 1;
 mpr(lose_mutation [mutat] [0]);
 break;
 case 3:
/* if (you[0].mutation [20] > 0)
 {
  mutate(20);
  break;
 }*/
 you[0].dex --;
 you[0].max_dex --;
 you[0].dex_ch = 1;
 mpr(lose_mutation [mutat] [0]);
 break;

 case 18:
/* if (you[0].mutation [1] > 0)
 {
  mutate(1);
  break;
 }*/
 you[0].strength ++;
 you[0].max_strength ++;
 you[0].strength_ch = 1;
 mpr(lose_mutation [mutat] [0]);
 break;
 case 19:
/* if (you[0].mutation [2] > 0)
 {
  mutate(2);
  break;
 }*/
 you[0].intel ++;
 you[0].max_intel ++;
 you[0].intel_ch = 1;
 mpr(lose_mutation [mutat] [0]);
 break;
 case 20:
/* if (you[0].mutation [3] > 0)
 {
  mutate(3);
  break;
 }*/
 you[0].dex ++;
 you[0].max_dex ++;
 you[0].dex_ch = 1;
 mpr(lose_mutation [mutat] [0]);
 break;


 case 14: // res elec
 you[0].attribute [0] --;
 mpr(lose_mutation [mutat] [you[0].mutation [mutat]]);
 break;

 case 16: // fast meta
/* if (you[0].mutation [17] > 0)
 {
  mutate(17);
  break;
 }*/
 you[0].hunger_inc --;
 mpr(lose_mutation [mutat] [you[0].mutation [mutat]]);
 break;

 case 17: // slow meta
/* if (you[0].mutation [16] > 0)
 {
  mutate(16);
  break;
 }*/
 if (you[0].mutation [mutat] == 0 | you[0].mutation [mutat] == 2) you[0].hunger_inc ++;
 mpr(lose_mutation [mutat] [you[0].mutation [mutat]]);
 break;

 case 21: // control tele
 you[0].attribute [3] --;
 mpr(lose_mutation [mutat] [you[0].mutation [mutat]]);
 break;

 case 5:
 case 7:
 you[0].dex ++;
 you[0].max_dex ++;
 you[0].dex_ch = 1;
 default: mpr(lose_mutation [mutat] [you[0].mutation [mutat]]);
 break;
}

        you[0].hp_ch = 1;
        you[0].ep_ch = 1;
        you[0].strength_ch = 1;
        you[0].intel_ch = 1;
        you[0].dex_ch = 1;
        you[0].AC_ch =  1;
        you[0].evasion_ch = 1;
        you[0].xp_ch = 1;
        you[0].gp_ch = 1;
        you[0].hung_ch = 1;


if (you[0].mutation [mutat] > 0) you[0].mutation [mutat] --;

return 1;

}


char body_covered(void)
{
/* checks how much of your body is covered by scales etc */
char covered = 0;
if (you[0].species == 13) covered ++; /* naga */

covered += you[0].mutation [0]; // thick skin
covered += you[0].mutation [4];
covered += you[0].mutation [5];
covered += you[0].mutation [6];
covered += you[0].mutation [7];

return covered;

}


char *mutation_name(char which_mutat)
{
 char st_prn [5];

 if (which_mutat == 1 | which_mutat == 2 | which_mutat == 3 | which_mutat == 18 | which_mutat == 19 | which_mutat == 20)
 {
   strcpy(mut_string, mutation_descrip [which_mutat] [0]);
   itoa(you[0].mutation [which_mutat], st_prn, 10);
   strcat(mut_string, st_prn);
   strcat(mut_string, ").");
   return mut_string;
 }

 return mutation_descrip [which_mutat] [you[0].mutation [which_mutat]];

}

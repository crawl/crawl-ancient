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
#include "macro.h"
#include "message.h"
#include "player.h"
#include "transfor.h"
#include "skills2.h"
#include "stuff.h"

#define NO_MUT 57

int how_mutated(void);
char delete_mutation(char which_mutation);
char body_covered(void);
void perma_mutate(int which_mut, char how_much);

char mut_string [80];

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
{"You are surrounded by a mild repulsion field (ev + 1).", "You are surrounded by a moderate repulsion field (ev + 3).", "You are surrounded by a strong repulsion field (ev + 5; repel missiles)."},
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
{"You are resistant to magic.", "You are highly resistant to magic.", "You are extremely resistant to the effects of magic."},
{"You cover the ground very quickly.", "You cover the ground very quickly.", "You cover the ground very quickly."},
{"You have supernaturally acute eyesight.", "You have supernaturally acute eyesight.", "You have supernaturally acute eyesight."},
{"Armour fits poorly on your deformed body.", "Armour fits poorly on your badly deformed body.", "Armour fits poorly on your hideously deformed body."},
{"You can teleport at will.", "You are good at teleporting at will.", "You can teleport instantly at will."},
{"You can spit poison.", "You can spit poison.", "You can spit poison."},
{"You can sense your immediate surroundings.", "You can sense your surroundings.", "You can sense a large area of your surroundings."},
// 30
{"You can breathe flames.", "You can breathe fire.", "You can breathe blasts of fire."},
{"You can translocate small distances instantaneously.", "You can translocate small distances instantaneously.", "You can translocate small distances instantaneously."},
{"You have a pair of small horns on your head.", "You have a pair of horns on your head.", "You have a pair of large horns on your head."},
{"Your muscles are strong (Str +1), but stiff (Dex -1).", "Your muscles are very strong (Str +2), but stiff (Dex -2).", "Your muscles are extremely strong (Str +3), but stiff (Dex -3)."},
{"Your muscles are flexible (Dex +1), but weak (Str -1).", "Your muscles are very flexible (Dex +2), but weak (Str -2).", "Your muscles are extremely flexible (Dex +3), but weak (Str -3)."},
{"You occasionally forget where you are.", "You sometimes forget where you are.", "You frequently forget where you are."},
{"You possess an exceptional clarity of mind.", "You possess an unnatural clarity of mind.", "You possess a supernatural clarity of mind."},
{"You tend to lose your temper in combat.", "You often lose your temper in combat.", "You have an uncontrollable temper."},
{"Your body is slowly deteriorating.", "Your body is deteriorating.", "Your body is rapidly deteriorating."},
{"Your vision is a little blurry.", "Your vision is quite blurry.", "Your vision is extremely blurry."},
// 40
{"You are somewhat resistant to further mutation.", "You are somewhat resistant to both further mutation and mutation removal.", "Your current mutations are irrevocably fixed, and you can mutate no more."},
{"You are frail (-10 percent hp).", "You are very frail (-20 percent hp).", "You are extremely frail (-30 percent hp)."},
{"You are robust (+10 percent hp).", "You are very robust (+20 percent hp).", "You are extremely robust (+30 percent hp)."},

{"You are immune to unholy pain and torment.", "", ""},
{"You are immune to negative energy.", "", ""},
{"You can summon minor demons to your aid.", "", ""}, /* Use find_spell in files.cc to avoid duplication */
{"You can summon demons to your aid.", "", ""},
{"You can hurl blasts of hellfire.", "", ""},
{"You can call on the torments of Hell.", "", ""},
{"You can raise the dead to walk for you.", "", ""},
// 50
{"You can control demons.", "", ""},
{"You can travel to (but not from) Pandemonium at will.", "", ""},
{"You can draw strength from death and destruction.", "", ""},
{"You can channel magical energy from Hell.", "", ""}, /* Not worshippers of Vehumet */
{"You can drain life in unarmed combat.", "", ""},
{"You can throw forth the flames of Gehenna.", "", ""}, /* Not conjurers/worshippers of Makhleb */
{"You can throw forth the frost of Cocytus.", "", ""},
{"You can invoke the powers of Tartarus to smite your living foes.", "", ""},
{"", "", ""},
{"", "", ""},
// 60
{"", "", ""},
{"", "", ""},
{"", "", ""},
{"", "", ""},
{"", "", ""},
{"", "", ""},
{"", "", ""},
{"", "", ""},
{"", "", ""},
{"", "", ""},

};

/*
If giving a mutation which must succeed (eg demonspawn), must add exception
to the "resist mutation" mutation thing.
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
{"You hunger for vegetation.", "You hunger for vegetation.", "You hunger for vegetation."},
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
{"You feel jumpy.", "You feel more jumpy.", "You feel even more jumpy."},
{"There is a nasty taste in your mouth for a moment.", "There is a nasty taste in your mouth for a moment.", "There is a nasty taste in your mouth for a moment."},
{"You feel aware of your surroundings.", "You feel more aware of your surroundings.", "You feel even more aware of your surroundings."},
// 30
{"Your throat feels hot.", "Your throat feels hot.", "Your throat feels hot."},
{"You feel a little jumpy.", "You feel more jumpy.", "You feel even more jumpy."},
{"A pair of horns grows on your head!", "The horns on your head grow some more.", "The horns on your head grow some more."},
{"Your muscles feel sore.", "Your muscles feel sore.", "Your muscles feel sore."},
{"Your muscles feel loose.", "Your muscles feel loose.", "Your muscles feel loose."},
{"You feel a little disoriented.", "You feel a little disoriented.", "Where the Hell are you?"},
{"Your thoughts seem clearer.", "Your thoughts seem clearer.", "Your thoughts seem clearer."},
{"You feel a little pissed off.", "You feel angry.", "You feel extremely angry at everything!"},
{"You feel yourself wasting away.", "You feel yourself wasting away.", "You feel your body start to fall apart."},
{"Your vision blurs.", "Your vision blurs.", "Your vision blurs."},
// 40
{"You feel genetically stable.", "You feel genetically stable.", "You feel genetically immutable."},
{"You feel frail.", "You feel frail.", "You feel frail."},
{"You feel robust.", "You feel robust.", "You feel robust."},
{"You feel a strange anaesthesia.", "", ""},
{"You feel negative.", "", ""},
{"A thousand chattering voices call out to you.", "", ""},
{"Help is not far away!", "", ""},
{"You smell fire and brimstone.", "", ""},
{"You feel a terrifying power at your call.", "", ""},
{"You feel an affinity for the dead.", "", ""},
// 50
{"You feel an affinity for all demonkind.", "", ""},
{"You feel something pulling you to a strange and terrible place.", "", ""},
{"You feel hungry for death.", "", ""},
{"You feel a flux of magical energy.", "", ""},
{"Your skin tingles in a strangely unpleasant way.", "", ""},
{"You smell the fires of Gehenna.", "", ""},
{"You feel the icy cold of Cocytus chill your soul.", "", ""},
{"A shadow passes over the world around you.", "", ""},
{"", "", ""},
{"", "", ""},
{"", "", ""},
// 60
{"", "", ""},
{"", "", ""},
{"", "", ""},
{"", "", ""},
{"", "", ""},
{"", "", ""},
{"", "", ""},
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
{"You feel static.", "You feel less jumpy.", "You feel less jumpy."},
{"You feel an ache in your throat.", "You feel an ache in your throat.", "You feel an ache in your throat."},
{"You feel slightly disorientated.", "You feel slightly disorientated.", "You feel slightly disorientated."},
// 30
{"A chill runs up and down your throat.", "A chill runs up and down your throat.", "A chill runs up and down your throat."},
{"You feel a little less jumpy.", "You feel less jumpy.", "You feel less jumpy."},
{"The horns on your head shrink away.", "The horns on your head shrink a bit.", "The horns on your head shrink a bit."},
{"Your muscles feel loose.", "Your muscles feel loose.", "Your muscles feel loose."},
{"Your muscles feel sore.", "Your muscles feel sore.", "Your muscles feel sore."},
{"You feel less disoriented.", "You feel less disoriented.", "You feel less disoriented."},
{"Your thinking seems confused.", "Your thinking seems confused.", "Your thinking seems confused."},
{"You feel a little more calm.", "You feel a little less angry.", "You feel a little less angry."},
{"You feel healthier.", "You feel a little healthier.", "You feel a little healthier."},
{"Your vision sharpens.", "Your vision sharpens a little.", "Your vision sharpens a little."},
// 40
{"You feel genetically unstable.", "You feel genetically unstable.", "You should not be reading this message. Bug reports to zel@olis.net.au."},
{"You feel robust.", "You feel robust.", "You feel robust."},
{"You feel frail.", "You feel frail.", "You feel frail."},
/* Some demonic powers start here... */
{"", "", ""},
{"", "", ""},
{"", "", ""},
{"", "", ""},
};


/*
Chance out of 10 that mutation will be given/removed randomly. 0 means never.
*/
char mutation_rarity [] =
{
10, // tough skin
8, // str
8, // int
8, // dex
3, // gr scales
2, // bl scales
2, // grey scales
1, // bone
1, // repuls field
4, // res poison
// 10
5, // carn
5, // herb
4, // res fire
4, // res cold
2, // res elec
3, // regen
10, // fast meta
7, // slow meta
10, // abil loss
10, // ""
// 20
10, // ""
2, // tele control
3, // teleport
5, // res magic
1, // run
2, // see invis
8, // deformation
2, // teleport at will
8, // spit poison
3, // sense surr
// 30
4, // breathe fire
3, // blink
7, // horns
10, // strong/stiff muscles
10, // weak/loose muscles
6, // forgetfulness
6, // clarity (as the amulet)
7, // berserk/temper
10, // deterioration
10, // blurred vision
// 40
4, // resist mutation
10, // frail
5, // robust
/* Some demonic powers start here: */
0,
0,
0,
0,
0,
0,
0,
// 50
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
// 60
0
};

void display_mutations(void)
{

   int i;
   int j = 0;
   char st_prn [5];
#ifdef DOS_TERM
   char buffer[4800];
   window(1, 1, 80, 25);
   gettext(1, 1, 80, 25, buffer);
#endif


   clrscr();
   textcolor(WHITE);
        cprintf("                  Mutations & Other Weirdness"EOL);
   textcolor(LIGHTGREY);

   for (i = 0; i < 100; i ++)
   {
    if (you[0].mutation [i] != 0)
    {
     j ++;
     textcolor(LIGHTGREY);
     if (you[0].demon_pow [i] != 0) textcolor(RED); /* mutation is actually a demonic power */
     if (you[0].demon_pow [i] != 0 && you[0].demon_pow [i] < you[0].mutation [i]) textcolor(LIGHTRED); /* same as above, but power is enhanced by mutation */
     if (i == 1 || i == 2 || i == 3 || i == 18 || i == 19 || i == 20)
     {
      cprintf(mutation_descrip [i] [0]);
      itoa(you[0].mutation [i], st_prn, 10);
      cprintf(st_prn);
      cprintf(")."EOL);
      continue;
     }
     cprintf(mutation_descrip [i] [you[0].mutation [i] - 1]);
     cprintf(EOL);
    }
   }

   if (j == 0) cprintf("You aren't a mutant."EOL);

   if (getch() == 0) getch();
#ifdef DOS_TERM
   puttext(1, 1, 80, 25, buffer);
#endif

//   cprintf("xxxxxxxxxxxxx");
//   last_requested = 0;
return;


}


char mutate(int which_mutation)
{
char mutat = which_mutation;

char force = 0;

if (which_mutation >= 1000) /* Must give this mutation - no failure */
{
 force = 1;
 mutat -= 1000;
 which_mutation -= 1000;
}

//char st_prn [10];

int i = 0;

if (you[0].is_undead != 0 && force == 0)
 return 0;

if (wearing_amulet(44) != 0 && random2(10) != 0 && force == 0)
{
 mpr("You feel rather odd for a moment.");
 return 0;
}

if (you[0].mutation [40] != 0 && (you[0].mutation [40] == 3 || random2(3) != 0) && force == 0)
{
 mpr("You feel rather odd for a moment.");
 return 0;
}

if (which_mutation == 100 && random2(15) < how_mutated())
{
 if (random2(3) != 0) return 0;
 return delete_mutation(100);
}

if (which_mutation == 100)
 do
 {
  mutat = random2(NO_MUT);
  if (random2(1000) == 0) return 0;
 } while ((you[0].mutation [mutat] >= 3 && mutat != 1 && mutat != 2 && mutat != 3 && mutat != 18 && mutat != 19 && mutat != 20) || random2(10) >= mutation_rarity [mutat] || you[0].mutation [mutat] > 13);

if (you[0].mutation [mutat] >= 3 && mutat != 1 && mutat != 2 && mutat != 3 && mutat != 18 && mutat != 19 && mutat != 20) return 0;
if (you[0].mutation [mutat] > 13 && force == 0) return 0;
if (mutat == 0 || (mutat >= 4 && mutat <= 7) && body_covered() > 2) return 0;

if ((mutat == 25 || mutat == 9 || mutat == 28) && you[0].species == 13) return 0; // nagas have see invis and res poison and can spit poison
if (you[0].species == 14 && mutat == 29) return 0; /* gnomes can't sense surroundings */
if ((you[0].species == 30 || you[0].species == 32) && mutat == 24) return 0; /* centaurs/spriggans are already swift */
if (mutat == 15 && you[0].mutation [17] > 0) return 0; /* if you have a slow metabolism, no regen */
if (mutat == 17 && you[0].mutation [15] > 0) return 0; /* if you have a slow metabolism, no regen */
if (mutat == 25 && you[0].mutation [39] > 0) return 0;
if (mutat == 39 && you[0].mutation [25] > 0) return 0; /* blurred vision/see invis */


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

 case 10: // carnivorous
 if (you[0].mutation [11] > 0)
 {
  delete_mutation(11);
  return 1;
 }
 mpr(gain_mutation [mutat] [you[0].mutation [mutat]]);
 break;

 case 11: // herbivorous
 if (you[0].mutation [10] > 0)
 {
  delete_mutation(10);
  return 1;
 }
 mpr(gain_mutation [mutat] [you[0].mutation [mutat]]);
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
 if (you[0].mutation [mutat] == 0 || you[0].mutation [mutat] == 2) you[0].hunger_inc --;
 mpr(gain_mutation [mutat] [you[0].mutation [mutat]]);
 break;

 case 21: // control tele
 you[0].attribute [3] ++;
 mpr(gain_mutation [mutat] [you[0].mutation [mutat]]);
 break;

 case 32: // horns force your helmet off
 mpr(gain_mutation [mutat] [you[0].mutation [mutat]]);
 char removed [8];
 for (i = 0; i < 8; i ++)
 {
  removed [i] = 0;
 }
 removed [2] = 1;
 remove_equipment(removed);
 break;

 case 33: // strong/stiff
 if (you[0].mutation [34] > 0)
 {
  delete_mutation(34);
  return 1;
 }
 you[0].strength ++;
 you[0].max_strength ++;
 you[0].strength_ch = 1;
 you[0].dex --;
 you[0].max_dex --;
 you[0].dex_ch = 1;
 mpr(gain_mutation [mutat] [0]);
 break;

 case 34: // weak/loose
 if (you[0].mutation [33] > 0)
 {
  delete_mutation(33);
  return 1;
 }
 you[0].strength --;
 you[0].max_strength --;
 you[0].strength_ch = 1;
 you[0].dex ++;
 you[0].max_dex ++;
 you[0].dex_ch = 1;
 mpr(gain_mutation [mutat] [0]);
 break;

 case 41: // frail
 if (you[0].mutation [42] > 0)
 {
  delete_mutation(42);
  return 1;
 }
 mpr(gain_mutation [mutat] [you[0].mutation [mutat]]);
 you[0].mutation [mutat] ++;
 calc_hp();
 return 1;

 case 42: // robust
 if (you[0].mutation [41] > 0)
 {
  delete_mutation(41);
  return 1;
 }
 mpr(gain_mutation [mutat] [you[0].mutation [mutat]]);
 you[0].mutation [mutat] ++;
 calc_hp();
 return 1;

 case 5:
 case 7:
 you[0].dex --;
 you[0].max_dex --;
 you[0].dex_ch = 1;
 default: mpr(gain_mutation [mutat] [you[0].mutation [mutat]]);
 break;
}

you[0].mutation [mutat] ++;

/* remember, some mutations don't get this far (eg frail) */

return 1;

}


int how_mutated(void)
{

int i;
int j = 0;

for (i = 0; i < 100; i ++)
{
 if (you[0].demon_pow [i] < you[0].mutation [i]) j += you[0].mutation [i];
}

return j;

}



char delete_mutation(char which_mutation)
{
char mutat = which_mutation;

if (you[0].mutation [40] > 1 && (you[0].mutation [40] == 3 || random2(2) == 0))
{
 mpr("You feel rather odd for a moment.");
 return 0;
}

if (which_mutation == 100)
 do
 {
  mutat = random2(NO_MUT);
  if (random2(1000) == 0) return 0;
 } while ((you[0].mutation [mutat] == 0 && mutat != 1 && mutat != 2 && mutat != 3 && mutat != 18 && mutat != 19 && mutat != 20) || random2(10) >= mutation_rarity [mutat] || you[0].demon_pow [mutat] >= you[0].mutation [mutat]);


if (you[0].mutation [mutat] == 0) return 0;

if (you[0].demon_pow [mutat] >= you[0].mutation [mutat]) return 0;

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
 mpr(lose_mutation [mutat] [you[0].mutation [mutat] - 1]);
 break;

 case 16: // fast meta
/* if (you[0].mutation [17] > 0)
 {
  mutate(17);
  break;
 }*/
 you[0].hunger_inc --;
 mpr(lose_mutation [mutat] [you[0].mutation [mutat] - 1]);
 break;

 case 17: // slow meta
/* if (you[0].mutation [16] > 0)
 {
  mutate(16);
  break;
 }*/
 if (you[0].mutation [mutat] == 0 || you[0].mutation [mutat] == 2) you[0].hunger_inc ++;
 mpr(lose_mutation [mutat] [you[0].mutation [mutat] - 1]);
 break;

 case 21: // control tele
 you[0].attribute [3] --;
 mpr(lose_mutation [mutat] [you[0].mutation [mutat] - 1]);
 break;

 case 33: // strong/stiff
 you[0].strength --;
 you[0].max_strength --;
 you[0].strength_ch = 1;
 you[0].dex ++;
 you[0].max_dex ++;
 you[0].dex_ch = 1;
 mpr(lose_mutation [mutat] [0]);
 break;

 case 34: // weak/loose
 you[0].strength ++;
 you[0].max_strength ++;
 you[0].strength_ch = 1;
 you[0].dex --;
 you[0].max_dex --;
 you[0].dex_ch = 1;
 mpr(lose_mutation [mutat] [0]);
 break;

 case 41: // frail
 mpr(lose_mutation [mutat] [0]);
 if (you[0].mutation [mutat] > 0) you[0].mutation [mutat] --;
 calc_hp();
 you[0].hp_ch = 1;
 return 1;

 case 42: // robust
 mpr(lose_mutation [mutat] [0]);
 if (you[0].mutation [mutat] > 0) you[0].mutation [mutat] --;
 calc_hp();
 you[0].hp_ch = 1;
 return 1;

 case 5:
 case 7:
 you[0].dex ++;
 you[0].max_dex ++;
 you[0].dex_ch = 1;
 default: mpr(lose_mutation [mutat] [you[0].mutation [mutat] - 1]);
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
if (you[0].species >= 18 && you[0].species <= 29) return 3; /* Dracon */

covered += you[0].mutation [0]; // thick skin
covered += you[0].mutation [4];
covered += you[0].mutation [5];
covered += you[0].mutation [6];
//covered += you[0].mutation [7]; boney plates

return covered;

}


char *mutation_name(char which_mutat)
{
 char st_prn [5];

 if (which_mutat == 1 || which_mutat == 2 || which_mutat == 3 || which_mutat == 18 || which_mutat == 19 || which_mutat == 20)
 {
   strcpy(mut_string, mutation_descrip [which_mutat] [0]);
   itoa(you[0].mutation [which_mutat], st_prn, 10);
   strcat(mut_string, st_prn);
   strcat(mut_string, ").");
   return mut_string;
 }

 return mutation_descrip [which_mutat] [you[0].mutation [which_mutat] - 1];

}

/* Use an attribute counter for how many demonic mutations a dspawn has */
void demonspawn(void)
{

 int whichm = -1;
 char howm = 1;
 int counter = 0;

 you[0].attribute [8] ++;

 mpr("Your demonic ancestry asserts itself...");

 if (you[0].xl <= 9)
 {
  do
  {
   if (you[0].skills [26] == 0) /* conjurers don't get throw fr/fl */
   {
    whichm = 55 + random2(2);
    howm = 1;
   }
   if (random2(3) == 0 && you[0].skills [28] == 0) /* summoners don't get summon imp */
   {
    whichm = 45;
    howm = 1;
   }

   if (random2(3) == 0) { whichm = 13 + random2(2); howm = 1; } /* res fire or cold */
   if (random2(5) == 0) { whichm = 25; howm = 1; } /* see invis */
   if (random2(7) == 0) { whichm = 28; howm = 1; } /* spit poison */
   if (random2(10) == 0) { whichm = 29; howm = 2; } /* sense surr */
   if (random2(12) == 0) { whichm = 21; howm = 1; } /* control teleport */
   if (random2(5) == 0) { whichm = 30; howm = 2; } /* breathe fire */
   if (random2(10) == 0) { whichm = 31; howm = 1; } /* blink */
   if (random2(10) == 0) { whichm = 0; howm = 2 + random2(2); } /* tough skin */
   if (random2(12) == 0) { whichm = 4; howm = 1; } /* green scales */
   if (random2(12) == 0) { whichm = 5; howm = 1; } /* scales */
   if (random2(12) == 0) { whichm = 6; howm = 1; } /* scales */
   if (random2(30) == 0) { whichm = 7; howm = 1 + random2(2); } /* bone plates */
   if (random2(30) == 0) { whichm = 8; howm = 1 + random2(3); } /* repulsion field */
   if (random2(4) == 0) { whichm = 32; howm = 1; if (you[0].xl > 5) howm = 3; }/* horns */

   if (you[0].mutation [whichm] != 0) whichm = -1;

   counter ++;
  } while (whichm == -1 && counter < 5000);
  if (whichm == -1) /* unlikely but remotely possible */
  {
   you[0].max_strength ++;
   you[0].max_dex ++;
   you[0].max_intel ++;
   you[0].strength ++;
   you[0].dex ++;
   you[0].intel ++;
   you[0].strength_ch = 1;
   you[0].intel_ch = 1;
   you[0].dex_ch = 1;
   /* I know this is a cop-out */
  }
   else perma_mutate(whichm, howm);
   return;
 }


  do
  {
   if (you[0].skills [26] <= 2) /* good conjurers don't get bolt of draing */
   {
    whichm = 57;
    howm = 1;
   }
   if (random2(4) == 0 && you[0].skills [26] <= 3) /* good conjurers don't get hellfire */
   {
    whichm = 47;
    howm = 1;
   }
   if (random2(3) == 0 && you[0].skills [28] <= 2) /* good summoners don't get summon demon */
   {
    whichm = 46;
    howm = 1;
   }

   if (random2(8) == 0) { whichm = 23; howm = 3; } /* res magic */
   if (random2(12) == 0) { whichm = 24; howm = 1; } /* swift */
   if (random2(7) == 0) { whichm = 27; howm = 2; } /* teleport */
   if (random2(10) == 0) { whichm = 25; howm = 2; } /* regenerate */
   if (random2(12) == 0) { whichm = 24; howm = 1; } /* resist electric */
   if (random2(15) == 0 && you[0].mutation [48] == 0) { whichm = 43; howm = 1; } /* resist torment */
   if (random2(12) == 0) { whichm = 44; howm = 1; } /* resist -ve energy */
   if (random2(20) == 0 && you[0].mutation [43] == 0) { whichm = 48; howm = 1; } /* symbol of torment */
   if (random2(12) == 0 && you[0].skills [28] <= 2 && you[0].skills [29] <= 2) { whichm = 50; howm = 1; } /* control demons */
   if (random2(15) == 0 && you[0].skills [30] <= 2) { whichm = 51; howm = 1; } /* gate to pan */
   if (random2(11) == 0 && you[0].religion != 6) { whichm = 52; howm = 1; } /* gain power - no worshippers of Vehumet */
   if (random2(11) == 0 && you[0].religion != 6) { whichm = 53; howm = 1; } /* channel - no worshippers of Vehumet */
   if (random2(10) == 0 && you[0].skills [28] <= 2 && you[0].skills [29] <= 2) { whichm = 49; howm = 1; } /* raise dead */
   if (random2(14) == 0 && you[0].skills [19] >= 10) { whichm = 54; howm = 1; } /* drain life */

   if (you[0].mutation [whichm] != 0) whichm = -1;

   counter ++;
  } while (whichm == -1 && counter < 5000);
  if (whichm == -1) /* unlikely but remotely possible */
  {
   you[0].max_strength ++;
   you[0].max_dex ++;
   you[0].max_intel ++;
   you[0].strength ++;
   you[0].dex ++;
   you[0].intel ++;
   you[0].strength_ch = 1;
   you[0].intel_ch = 1;
   you[0].dex_ch = 1;
   /* I know this is a cop-out */
  }
   else perma_mutate(whichm, howm);


}


void perma_mutate(int which_mut, char how_much)
{

 mutate(which_mut + 1000);
 if (how_much == 2) mutate(which_mut + 1000);
 if (how_much == 3) mutate(which_mut + 1000);

 you[0].demon_pow [which_mut] = how_much;

}


char give_good_mutation(void)
{
  switch(random2(25)) /* beneficial mutates */
  {
   case 0: return mutate(0); break;
   case 1: return mutate(1); break;
   case 2: return mutate(2); break;
   case 3: return mutate(3); break;
   case 4: return mutate(12); break;
   case 5: return mutate(13); break;
   case 6: return mutate(14); break;
   case 7: return mutate(15); break;
   case 8: return mutate(21); break;
   case 9: return mutate(23); break;
   case 10: return mutate(24); break;
   case 11: return mutate(25); break;
   case 12: return mutate(4); break;
   case 13: return mutate(5); break;
   case 14: return mutate(6); break;
   case 15: return mutate(7); break;
   case 16: return mutate(8); break;
   case 17: return mutate(9); break;
   case 18: return mutate(27); break;
   case 19: return mutate(28); break;
   case 20: return mutate(29); break;
   case 21: return mutate(30); break;
   case 22: return mutate(31); break;
   case 23: return mutate(36); break;
   case 24: return mutate(42); break;
  }
 return 0;
}

char give_bad_mutation(void)
{
  switch(random2(12)) /* bad mutations */
  {
   case 0: return mutate(10); break;
   case 1: return mutate(11); break;
   case 2: return mutate(16); break;
   case 3: return mutate(18); break;
   case 4: return mutate(19); break;
   case 5: return mutate(20); break;
   case 6: return mutate(22); break;
   case 7: return mutate(26); break;
   case 8: return mutate(35); break;
   case 9: return mutate(38); break;
   case 10: return mutate(39); break;
   case 11: return mutate(41); break;
  }
 return 0;
}

#include "config.h"

#ifdef DOS
#include <conio.h>
#endif

#include <string.h>
#include <stdlib.h>

#include "bang.h"
#include "beam.h"
#include "effects.h"
#include "externs.h"
#include "invent.h"
#include "items.h"
#include "it_use2.h"
#include "it_use3.h"
#include "macro.h"
#include "monplace.h"
#include "monstuff.h"
#include "misc.h"
#include "mutation.h"
#include "player.h"
#include "ouch.h"
#include "religion.h"
#include "spell.h"
#include "spells.h"
#include "spells0.h"
#include "spells1.h"
#include "spells3.h"
#include "stuff.h"
#include "view.h"

char mutate(int which_mutation);


int learned = 0;
int spell_container = 0;
int sc_read_1, sc_read_2;
int i;
int book_thing;
int keyin;


char miscast_effect(char sp_type, char mag_pow, char mag_fail, char force_effect)
{

/*
sp_type is the type of the spell
mag_pow is overall power of the spell or effect (ie its level)
mag_fail is the degree to which you failed
force_effect forces a certain effect to occur. Currently unused.
*/

struct bolt beam [1];

char loopj = 0;
int spec_effect = 0;
int hurted = 0;

spec_effect = mag_pow * mag_fail * (10 + mag_pow) / 10;
if (random2(50) > spec_effect && random2(50) > spec_effect)
{
 goto nothing_happening;
}
spec_effect = spec_effect / 100;

#ifdef DEBUG
strcpy(info, "Sptype: ");
itoa(sp_type, st_prn, 10);
strcat(info, st_prn);
strcat(info, ", failure1: ");
itoa(spec_effect, st_prn, 10);
strcat(info, st_prn);
#endif


spec_effect = random2(spec_effect);
if (spec_effect > 3) spec_effect = 3;
if (spec_effect < 0) spec_effect = 0;

#ifdef DEBUG
strcat(info, ", failure2: ");
itoa(spec_effect, st_prn, 10);
strcat(info, st_prn);
mpr(info);
#endif



if (force_effect != 100) spec_effect = force_effect;


switch(sp_type)
{

case 11: // conjuration
switch(spec_effect)
{
 case 0: // just a harmless message
 switch(random2(10))
 {
  case 0: strcpy(info, "Sparks fly from your hands!"); break;
  case 1: strcpy(info, "The air around you crackles with energy!"); break;
  case 2: strcpy(info, "Wisps of smoke drift from your fingertips."); break;
  case 3: strcpy(info, "You feel a strange surge of energy!"); break;
  case 4: strcpy(info, "You are momentarily dazzled by a flash of light!"); break;
  case 5: strcpy(info, "Strange energies run through your body."); break;
  case 6: strcpy(info, "Your skin tingles."); break;
  case 7: strcpy(info, "Your skin glows momentarily."); break;
  case 8: strcpy(info, "Nothing appears to happen."); break;
  case 9: strcpy(info, "You smell something strange."); break;
 }
 mpr(info);
 break;

 case 1: // a bit less harmless stuff
 switch(random2(2))
 {
  case 0:
  strcpy(info, "Smoke pours from your fingertips!");
  mpr(info);
  big_cloud(5, you[0].x_pos, you[0].y_pos, 20);
  break;
  case 1:
  strcpy(info, "A wave of violent energy washes through your body!");
  mpr(info);
  ouch(2 + random2(3) + random2(3), 0, 18);
  break;
 }
 break;

 case 2: // rather less harmless stuff
 switch(random2(2))
 {
  case 0:
  strcpy(info, "You are blasted with magical energy!");
  mpr(info);
  ouch(4 + random2(7) + random2(7), 0, 18);
  break;
  case 1:
  strcpy(info, "You conjure up a violent explosion!");
  mpr(info);
  strcpy(info, "Oops.");
  mpr(info);
  beam[0].type = 43;
  beam[0].damage = 107;
  beam[0].flavour = 0; // <=- not sure about this
  beam[0].bx = you[0].x_pos;
  beam[0].by = you[0].y_pos;
  strcpy(beam[0].beam_name, "explosion");
  beam[0].colour = random2(15) + 1;
  beam[0].thing_thrown = 1; // your explosion (is this right?)
  explosion(0, beam);
  noisy(10, you[0].x_pos, you[0].y_pos);
  break;
 }
 break;

 case 3: // considerably less harmless stuff
 switch(random2(2))
 {
  case 0:
  strcpy(info, "Energy rips through your body!");
  mpr(info);
  ouch(9 + random2(12) + random2(12), 0, 18);
  break;
  case 1:
  strcpy(info, "There is a sudden explosion of magical energy!");
  mpr(info);
  beam[0].type = 43;
  beam[0].damage = 110;
  beam[0].flavour = 0; // <=- not sure about this
  beam[0].bx = you[0].x_pos;
  beam[0].by = you[0].y_pos;
  strcpy(beam[0].beam_name, "explosion");
  beam[0].colour = random2(15) + 1;
  beam[0].thing_thrown = 1; // your explosion (is this right?)
  explosion(random2(2), beam);
  noisy(20, you[0].x_pos, you[0].y_pos);
  break;
 }
 break;

}
break; // end conjuration

case 12:
switch(spec_effect)
{
 case 0: // just a harmless message
 switch(random2(10))
 {
  case 0: strcpy(info, "Your hands glow momentarily."); break;
  case 1: strcpy(info, "The air around you crackles with energy!"); break;
  case 2: strcpy(info, "Multicolored lights dance before your eyes!"); break;
  case 3: strcpy(info, "You feel a strange surge of energy!"); break;
  case 4: strcpy(info, "Waves of light ripple over your body."); break;
  case 5: strcpy(info, "Strange energies run through your body."); break;
  case 6: strcpy(info, "Your skin tingles."); break;
  case 7: strcpy(info, "Your skin glows momentarily."); break;
  case 8: strcpy(info, "Nothing appears to happen."); break;
  case 9: strcpy(info, "You hear something strange."); break;
 }
 mpr(info);
 break;

 case 1: // slightly annoying
 switch(random2(2))
 {
  case 0:
  potion_effect(7, 20); // levitation
  break;
  case 1:
  random_uselessness(2 + random2(7), 0);
  break;
 }
 break;

 case 2: // much more annoying
 switch(random2(3))
 {
  case 0: // curse
  curse_an_item(0, 0);
  strcpy(info, "You sense a malignant aura.");
  mpr(info);
  break;
  case 1:
  potion_effect(9, 10); // slow
  break;
  case 2:
  potion_effect(21, 10); // berserk
  break;
 }
 break;

 case 3: // potentially lethal
 switch(random2(4))
 {
  case 0: // curse
  do
  {
   curse_an_item(0, 0);
   loopj = random2(3);
  } while (loopj != 0);
  strcpy(info, "You sense an overwhelmingly malignant aura!");
  mpr(info);
  break;
  case 1:
  potion_effect(10, 10); // paral
  break;
  case 2:
  potion_effect(11, 10); // conf
  break;
  case 3:
  if (mutate(100) == 0) mpr("Nothing appears to happen.");
  break;
 }
 break;

}
break; // end enchantments

case 20: // translocations
switch(spec_effect)
{
 case 0: // just a harmless message
 switch(random2(10))
 {
  case 0: strcpy(info, "Space warps around you."); break;
  case 1: strcpy(info, "The air around you crackles with energy!"); break;
  case 2: strcpy(info, "You feel a wrenching sensation."); break;
  case 3: strcpy(info, "You feel a strange surge of energy!"); break;
  case 4: strcpy(info, "You spin around."); break;
  case 5: strcpy(info, "Strange energies run through your body."); break;
  case 6: strcpy(info, "Your skin tingles."); break;
  case 7: strcpy(info, "The world appears momentarily distorted!"); break;
  case 8: strcpy(info, "Nothing appears to happen."); break;
  case 9: strcpy(info, "You feel uncomfortable."); break;
 }
 mpr(info);
 break;

 case 1: // mostly harmless
 switch(random2(3))
 {
  case 0:
  strcpy(info, "You create a localised field of spatial distortion.");
  mpr(info);
  strcpy(info, "Ouch!");
  mpr(info);
  ouch(3 + random2(3) + random2(3), 0, 18);
  break;
  case 1:
  strcpy(info, "Space bends around you!");
  mpr(info);
  random_blink();
  ouch(2 + random2(2) + random2(2), 0, 18);
  break;
  case 2:
  strcpy(info, "Space twists in upon itself!");
  mpr(info);
  create_monster(244, 24, 1, you[0].x_pos, you[0].y_pos, MHITNOT, 250);
  break;
 }
 break;

 case 2: // less harmless
 switch(random2(2))
 {
  case 0:
  strcpy(info, "You create a strong localised field of spatial distortion.");
  mpr(info);
  strcpy(info, "Ouch!!");
  mpr(info);
  ouch(7 + random2(7) + random2(7), 0, 18);
  break;
  case 1:
  strcpy(info, "Space warps around you!");
  mpr(info);
  if (random2(3) == 0) you_teleport2(1);
        else random_blink();
  ouch(5 + random2(5) + random2(5), 0, 18);
  potion_effect(11, 10); // conf
  break;
  case 2:
  strcpy(info, "Space twists in upon itself!");
  mpr(info);
  for (loopj = 0; loopj < 2 + random2(3); loopj ++)
  {
   create_monster(244, 22, 1, you[0].x_pos, you[0].y_pos, MHITNOT, 250);
  }
  break;
 }
 break;

 case 3: // much less harmless
 switch(random2(4))
 {
  case 0:
  strcpy(info, "You create an extremely strong localised field of spatial distortion!");
  mpr(info);
  strcpy(info, "Ouch!!!");
  mpr(info);
  ouch(11 + random2(9) + random2(9), 0, 18);
  break;
  case 1:
  strcpy(info, "Space warps crazily around you!");
  mpr(info);
  you_teleport2(1);
  ouch(7 + random2(7) + random2(7), 0, 18);
  potion_effect(11, 30); // conf
  break;
  case 2:
  strcpy(info, "You are cast into the Abyss!");
  mpr(info);
  more();
  banished(96); // sends you to the abyss
  break;
  case 3:
  if (mutate(100) == 0) mpr("Nothing appears to happen.");
  break;
 }
 break;


}
break; // end translocations

case 18: // Summonings
switch(spec_effect)
{
 case 0: // just a harmless message
 switch(random2(10))
 {
  case 0: strcpy(info, "Shadowy shapes form in the air around you, then vanish."); break;
  case 1: strcpy(info, "You hear strange voices."); break;
  case 2: strcpy(info, "Your head hurts."); break;
  case 3: strcpy(info, "You feel a strange surge of energy!"); break;
  case 4: strcpy(info, "Your brain hurts!"); break;
  case 5: strcpy(info, "Strange energies run through your body."); break;
  case 6: strcpy(info, "The world appears momentarily distorted."); break;
  case 7: strcpy(info, "Space warps around you."); break;
  case 8: strcpy(info, "Nothing appears to happen."); break;
  case 9: strcpy(info, "Distant voices call out to you!"); break;
 }
 mpr(info);
 break;

 case 1: // a little bad
 switch(random2(2))
 {
  case 0: // identical to translocation
  strcpy(info, "You create a localised field of spatial distortion.");
  mpr(info);
  strcpy(info, "Ouch!");
  mpr(info);
  ouch(3 + random2(3) + random2(3), 0, 18);
  break;
  case 1:
  strcpy(info, "Space twists in upon itself!");
  mpr(info);
  create_monster(244, 24, 1, you[0].x_pos, you[0].y_pos, MHITNOT, 250);
  break;
 }

 case 2: // more bad
 switch(random2(2))
 {
  case 0:
  strcpy(info, "Space twists in upon itself!");
  mpr(info);
  for (loopj = 0; loopj < 2 + random2(3); loopj ++)
  {
   create_monster(244, 22, 1, you[0].x_pos, you[0].y_pos, MHITNOT, 250);
  }
  break;
  case 1:
  strcpy(info, "Something appears in a flash of light!");
  mpr(info);
  create_monster(220 + random2(5), 24, 1, you[0].x_pos, you[0].y_pos, MHITNOT, 250);
  break;
 }
 break;


 case 3: // more bad
 switch(random2(4))
 {
  case 0:
  strcpy(info, "Something forms out of thin air.");
  mpr(info);
  create_monster(23, 0, 1, you[0].x_pos, you[0].y_pos, MHITNOT, 250);
  break;
  case 1:
  strcpy(info, "You sense a hostile presence.");
  mpr(info);
  create_monster(220 + random2(10), 0, 1, you[0].x_pos, you[0].y_pos, MHITNOT, 250);
  break;
  case 2:
  strcpy(info, "Something turns its attention towards you...");
  mpr(info);
  for (loopj = 0; loopj < 2 + random2(3); loopj ++)
  {
   create_monster(220 + random2(5), 22, 1, you[0].x_pos, you[0].y_pos, MHITNOT, 250);
  }
  break;
  case 3:
  strcpy(info, "You are cast into the Abyss!");
  mpr(info);
  banished(96); // sends you to the abyss
  break;
 }
 break;
// A powerful entity turns its attention onto you

} // end Summonings
break;


case 19: // Divinations
switch(spec_effect)
{
 case 0: // just a harmless message
 switch(random2(10))
 {
  case 0: strcpy(info, "Weird images run through your mind."); break;
  case 1: strcpy(info, "You hear strange voices."); break;
  case 2: strcpy(info, "Your head hurts."); break;
  case 3: strcpy(info, "You feel a strange surge of energy!"); break;
  case 4: strcpy(info, "Your brain hurts!"); break;
  case 5: strcpy(info, "Strange energies run through your body."); break;
  case 6: strcpy(info, "Everything looks hazy for a moment."); break;
  case 7: strcpy(info, "You seem to have forgotten something, but you can't remember what it was!"); break;
  case 8: strcpy(info, "Nothing appears to happen."); break;
  case 9: strcpy(info, "You feel uncomfortable."); break;
 }
 mpr(info);
 break;

 case 1: // more annoying things
 switch(random2(2))
 {
  case 0:
  strcpy(info, "You feel slightly disoriented.");
  mpr(info);
  forget_map(10 + random2(10));
  break;
  case 1:
  potion_effect(11, 1); // conf
  break;
 }
 break;

 case 2: // even more annoying things
 switch(random2(2))
 {
  case 0:
  if (you[0].is_undead != 0)
  {
   strcpy(info, "You suddenly recall your previous life!");
   mpr(info);
   break;
  }
  if (player_sust_abil() != 0 || you[0].intel <= 3)
  {
   strcpy(info, "You have a terrible headache.");
   mpr(info);
   break;
  }
  strcpy(info, "You have damaged your brain!");
  mpr(info);
  you[0].intel --;
  you[0].intel_ch = 1;
  potion_effect(11, 1); // conf
  break;
  case 1:
  strcpy(info, "You feel lost.");
  mpr(info);
  forget_map(40 + random2(40));
  potion_effect(11, 1); // conf
  break;

 }
 break;

 case 3: // nasty
 switch(random2(3))
 {
  case 0:
  if (forget_spell() == 1)
  {
   strcpy(info, "You have forgotten a spell!");
  } else strcpy(info, "You get a splitting headache.");
  mpr(info);
  break;
  case 1:
  strcpy(info, "You feel completely lost.");
  mpr(info);
  forget_map(100);
  potion_effect(11, 100); // conf
  break;
  case 2:
  if (you[0].is_undead != 0)
  {
   strcpy(info, "You suddenly recall your previous life.");
   mpr(info);
   break;
  }
  if (player_sust_abil() != 0 || you[0].intel <= 3)
  {
   strcpy(info, "You have a terrible headache.");
   mpr(info);
   break;
  }
  strcpy(info, "You have damaged your brain!");
  mpr(info);
  you[0].intel -= 2 + random2(2);
  if (you[0].intel <= 3) you[0].intel = 3;
  you[0].intel_ch = 1;
  potion_effect(11, 100); // conf
  break;
 }
 break;

}
break; // end divinations

 case 16: // necromancy
 if (you[0].religion == 3 && you[0].piety >= 50 && random2(150) <= you[0].piety)
 {
  mpr("Nothing appears to happen.");
  break;
 }
 switch(spec_effect)
 {
  case 0:
  switch(random2(10))
  {
   case 0: strcpy(info, "You smell decay."); break;
   case 1: strcpy(info, "You hear strange and distant voices."); break;
   case 2: strcpy(info, "Pain shoots through your body."); break;
   case 3: strcpy(info, "Your bones ache."); break;
   case 4: strcpy(info, "The world around you seems to dim momentarily."); break;
   case 5: strcpy(info, "Strange energies run through your body."); break;
   case 6: strcpy(info, "You shiver with cold."); break;
   case 7: strcpy(info, "You sense a malignant aura."); break;
   case 8: strcpy(info, "Nothing appears to happen."); break;
   case 9: strcpy(info, "You feel very uncomfortable."); break;
  }
  mpr(info);
  break;

  case 1: // a bit nasty
  switch(random2(3))
  {
   case 0:
   if (you[0].is_undead != 0)
   {
    strcpy(info, "You feel weird for a moment.");
    mpr(info);
    break;
   }
   strcpy(info, "Pain shoots through your body!");
   mpr(info);
   ouch(random2(4) + random2(4), 0, 18);
   break;
   case 1:
   strcpy(info, "You feel horribly lethargic.");
   mpr(info);
   potion_effect(9, 15); // slow
   break;
   case 2:
   strcpy(info, "You smell decay."); // identical to a harmless message
   mpr(info);
   you[0].rotting ++;
   break;
  }
  break;

  case 2: // much nastier
  switch(random2(3))
  {
   case 0:
   strcpy(info, "You are surrounded by flickering shadows.");
   mpr(info);
   create_monster(63, 21, 1, you[0].x_pos, you[0].y_pos, MHITNOT, 250);
   break;
   case 1:
   if (random2(3) == 0 && player_prot_life() == 0)
   {
    drain_exp();
    break;
   } // otherwise it just flows through...
   case 2:
   if (you[0].is_undead != 0)
   {
    strcpy(info, "You feel weird for a moment.");
    mpr(info);
    break;
   }
   strcpy(info, "You convulse helplessly as pain tears through your body!");
   mpr(info);
   ouch(random2(12) + random2(12) + 5, 0, 18);
   break;
  }
  break;

  case 3: // even nastier
  switch(random2(5))
  {
   case 0:
   if (you[0].is_undead != 0)
   {
    strcpy(info, "Something just walked over your grave. No, really!");
    mpr(info);
    break;
   }
   strcpy(info, "Your body is wracked with pain!");
   mpr(info);
   loopj = (you[0].hp / 2) - 1;
   if (loopj <= 0) loopj = 0;
   ouch(loopj, 0, 0); // can never die from this, right?
   you[0].hp_ch = 1;
   break;
   case 1:
   strcpy(info, "You are engulfed in negative energy!");
   mpr(info);
   if (player_prot_life() == 0)
   {
    drain_exp();
    break;
   } // otherwise it just flows through...
   case 2:
   lose_stat(100, 1);
   break;
   case 3:
   if (you[0].is_undead != 0)
   {
    strcpy(info, "You feel terrible.");
    mpr(info);
    break;
   }
   strcpy(info, "You feel your flesh start to rot away!");
   mpr(info);
   you[0].rotting += random2(4) + 1;
   break;
   case 4:
   strcpy(info, "Something reaches out for you...");
   mpr(info);
   create_monster(48, 23, 1, you[0].x_pos, you[0].y_pos, MHITNOT, 250);
   break;
  }
  break;

 }
 break; // end necromancy

  case 15: /* transmigr */
  switch(spec_effect)
  {
   case 0: // just a harmless message
   switch(random2(10))
   {
    case 0: strcpy(info, "Your hands glow momentarily."); break;
    case 1: strcpy(info, "The air around you crackles with energy!"); break;
    case 2: strcpy(info, "Multicolored lights dance before your eyes!"); break;
    case 3: strcpy(info, "You feel a strange surge of energy!"); break;
    case 4: strcpy(info, "Waves of light ripple over your body."); break;
    case 5: strcpy(info, "Strange energies run through your body."); break;
    case 6: strcpy(info, "Your skin tingles."); break;
    case 7: strcpy(info, "Your skin glows momentarily."); break;
    case 8: strcpy(info, "Nothing appears to happen."); break;
    case 9: strcpy(info, "You smell something strange."); break;
   }
   mpr(info);
   break;

   case 1: // slightly annoying
   switch(random2(2))
   {
    case 0:
    mpr("Your body is twisted painfully.");
    ouch(random2(4) + random2(4), 0, 18);
    break;
    case 1:
    random_uselessness(2 + random2(7), 0);
    break;
   }
   break;

   case 2: // much more annoying
   switch(random2(4))
   {
    case 0:
    mpr("Your body is twisted very painfully!");
    ouch(random2(12) + random2(12), 0, 18);
    break;
    case 1:
    mpr("Strange energies tear through your body!");
    mutate(100);
    break;
    case 2:
    potion_effect(10, 10); // paral
    break;
    case 3:
    potion_effect(11, 10); // conf
    break;
   }
   break;

   case 3: // even nastier
   switch(random2(3))
   {
    case 0:
    mpr("Your body is distorted in a weird and horrible way!");
    mutate(100);
    mutate(100);
    mutate(100);
    mutate(100);
    ouch(random2(12) + random2(12), 0, 18);
    break;
    case 1:
    mpr("You feel very strange.");
    delete_mutation(100);
    delete_mutation(100);
    delete_mutation(100);
    delete_mutation(100);
    delete_mutation(100);
    delete_mutation(100);
    delete_mutation(100);
    ouch(random2(12) + random2(12), 0, 18);
    break;
    case 2:
    mpr("Your body is distorted in a weirdly horrible way!");
    if (give_bad_mutation() == 0)
     if (give_bad_mutation() == 0)
      if (give_bad_mutation() == 0)
        give_bad_mutation();
    ouch(random2(12) + random2(12), 0, 18);
    break;
   }
 break;

}
break; // end enchantments



case 13: // fire
switch(spec_effect)
{
 case 0: // just a harmless message
 switch(random2(10))
 {
  case 0: strcpy(info, "Sparks fly from your hands!"); break;
  case 1: strcpy(info, "The air around you burns with energy!"); break;
  case 2: strcpy(info, "Wisps of smoke drift from your fingertips."); break;
  case 3: strcpy(info, "You feel a strange surge of energy!"); break;
  case 4: strcpy(info, "You smell smoke."); break;
  case 5: strcpy(info, "Heat runs through your body."); break;
  case 6: strcpy(info, "You feel uncomfortably hot."); break;
  case 7: strcpy(info, "Lukewarm flames ripple over your body."); break;
  case 8: strcpy(info, "Nothing appears to happen."); break;
  case 9: strcpy(info, "You hear a sizzling sound."); break;
 }
 mpr(info);
 break;

 case 1: // a bit less harmless stuff
 switch(random2(2))
 {
  case 0:
  strcpy(info, "Smoke pours from your fingertips!");
  mpr(info);
  big_cloud(5 + random2(3), you[0].x_pos, you[0].y_pos, 20);
  break;
  case 1:
  strcpy(info, "Flames dance in the air around you.");
  mpr(info);
  scrolls_burn(3, 6);
  if (player_res_fire() < 100) ouch(2 + random2(3) + random2(3), 0, 18);
  break;
 }
 break;

 case 2: // rather less harmless stuff
 switch(random2(2))
 {
  case 0:
  strcpy(info, "You are blasted with flames.");
  mpr(info);
  ouch(check_your_resists(3 + random2(7) + random2(7), 2), 0, 18);
  scrolls_burn(5, 6);
  break;
  case 1:
  strcpy(info, "You conjure up a fiery explosion!");
  mpr(info);
  strcpy(info, "Oops.");
  mpr(info);
  beam[0].type = 43;
  beam[0].damage = 107;
  beam[0].flavour = 2;
  beam[0].bx = you[0].x_pos;
  beam[0].by = you[0].y_pos;
  strcpy(beam[0].beam_name, "explosion");
  beam[0].colour = RED;
  beam[0].thing_thrown = 1; // your explosion (is this right?)
  explosion(0, beam);
  noisy(10, you[0].x_pos, you[0].y_pos);
  break;
 }
 break;

 case 3: // considerably less harmless stuff
 switch(random2(3))
 {
  case 0:
  mpr("You are blasted with searing flames!");
  ouch(check_your_resists(8 + random2(12) + random2(12), 2), 0, 18);
  scrolls_burn(10, 6);
  break;
  case 1:
  strcpy(info, "There is a sudden and violent explosion of flames!");
  mpr(info);
  beam[0].type = 43;
  beam[0].damage = 110;
  beam[0].flavour = 2;
  beam[0].bx = you[0].x_pos;
  beam[0].by = you[0].y_pos;
  strcpy(beam[0].beam_name, "fireball");
  beam[0].colour = RED;
  beam[0].thing_thrown = 1; // your explosion (is this right?)
  explosion(random2(2), beam);
  noisy(20, you[0].x_pos, you[0].y_pos);
  break;
  case 2:
  strcpy(info, "You are covered in liquid fire!");
  mpr(info);
  you[0].duration [0] += 1 + random2(3) + random2(3) + random2(3);
  break;
 }
 break;
}
break; // end fire


case 14: // ice
switch(spec_effect)
{
 case 0: // just a harmless message
 switch(random2(10))
 {
  case 0: strcpy(info, "You shiver with cold."); break;
  case 1: strcpy(info, "A chill runs through your body."); break;
  case 2: strcpy(info, "Wisps of condensation drift from your fingertips."); break;
  case 3: strcpy(info, "You feel a strange surge of energy!"); break;
  case 4: strcpy(info, "Your hands feel numb with cold."); break;
  case 5: strcpy(info, "A chill runs through your body."); break;
  case 6: strcpy(info, "You feel uncomfortably cold."); break;
  case 7: strcpy(info, "Frost covers your body."); break;
  case 8: strcpy(info, "Nothing appears to happen."); break;
  case 9: strcpy(info, "You hear a crackling sound."); break;
 }
 mpr(info);
 break;

 case 1: // a bit less harmless stuff
 switch(random2(2))
 {
  case 0:
  strcpy(info, "You feel extremely cold.");
  mpr(info);
  break;
  case 1:
  strcpy(info, "You are covered in a thin layer of ice");
  mpr(info);
  scrolls_burn(2, 8);
  if (player_res_cold() < 100) ouch(2 + random2(3) + random2(3), 0, 18);
  break;
 }
 break;

 case 2: // rather less harmless stuff
 switch(random2(2))
 {
  case 0:
  mpr("Heat is drained from your body.");
  ouch(check_your_resists(3 + random2(4) + random2(4), 3), 0, 18);
  scrolls_burn(4, 8);
  break;
  case 1:
  strcpy(info, "You conjure up an explosion of ice and frost!");
  mpr(info);
  strcpy(info, "Oops.");
  mpr(info);
  beam[0].type = 43;
  beam[0].damage = 107;
  beam[0].flavour = 3;
  beam[0].bx = you[0].x_pos;
  beam[0].by = you[0].y_pos;
  strcpy(beam[0].beam_name, "explosion");
  beam[0].colour = WHITE;
  beam[0].thing_thrown = 1; // your explosion (is this right?)
  explosion(0, beam);
  noisy(10, you[0].x_pos, you[0].y_pos);
  break;
 }
 break;

 case 3: // less harmless stuff
 switch(random2(2))
 {
  case 0:
  mpr("You are blasted with ice!");
  ouch(check_your_resists(5 + random2(6) + random2(6), 3), 0, 18);
  scrolls_burn(9, 8);
  break;
  case 1:
  strcpy(info, "Freezing gasses pour from your hands!");
  mpr(info);
  big_cloud(3, you[0].x_pos, you[0].y_pos, 20);
  break;
 }
 break;

}
break; // end ice

case 22: // Earth
switch(spec_effect)
{
 case 0: // just a harmless message
 case 1:
 switch(random2(10))
 {
  case 0: strcpy(info, "You feel earthy."); break;
  case 1: strcpy(info, "You are showered with tiny particles of grit."); break;
  case 2: strcpy(info, "Sand pours from your fingers."); break;
  case 3: strcpy(info, "You feel a strange surge of energy!"); break;
  case 4: strcpy(info, "You sympathise with the stones."); break;
  case 5: strcpy(info, "You feel gritty."); break;
  case 6: strcpy(info, "You feel like a piece of rock."); break;
  case 7: strcpy(info, "You feel like a paving stone."); break;
  case 8: strcpy(info, "Nothing appears to happen."); break;
  case 9: strcpy(info, "You feel chalky."); break;
 }
 mpr(info);
 break;

 case 2: // slightly less harmless stuff
 switch(random2(1))
 {
  case 0:
  switch(random2(3))
  {
   case 0: strcpy(info, "You are hit by flying rocks!"); break;
   case 1: strcpy(info, "You are blasted with sand!"); break;
   case 2: strcpy(info, "Rocks fall onto you out of nowhere!"); break;
  }
  mpr(info);
  hurted = 7 + random2(7) + random2(7);
  if (player_AC() > 0) hurted -= random2(player_AC());
  ouch(hurted, 0, 18);
  break;
 }
 break;

 case 3: // less harmless stuff
 switch(random2(1))
 {
  case 0:
  strcpy(info, "You conjure up an explosion of flying shrapnel!");
  mpr(info);
  strcpy(info, "Oops.");
  mpr(info);
  beam[0].type = 43;
  beam[0].damage = 108;
  beam[0].flavour = 19; // shrapnel
  beam[0].bx = you[0].x_pos;
  beam[0].by = you[0].y_pos;
  strcpy(beam[0].beam_name, "explosion");
  beam[0].colour = CYAN;
  if (random2(5) == 0) beam[0].colour = BROWN;
  if (random2(5) == 0) beam[0].colour = LIGHTCYAN;
  beam[0].thing_thrown = 1; // your explosion (is this right?)
  explosion(0, beam);
  noisy(10, you[0].x_pos, you[0].y_pos);
  break;
 }
 break;

}
break; // end Earth

case 23: // air
switch(spec_effect)
{
 case 0: // just a harmless message
 switch(random2(10))
 {
  case 0: strcpy(info, "Ouch! You gave yourself an electric shock."); break;
  case 1: strcpy(info, "You feel momentarily weightless."); break;
  case 2: strcpy(info, "Wisps of vapour drift from your fingertips."); break;
  case 3: strcpy(info, "You feel a strange surge of energy!"); break;
  case 4: strcpy(info, "You feel electric!"); break;
  case 5: strcpy(info, "Sparks of electricity dance on your fingertips."); break;
  case 6: strcpy(info, "You are blasted with air!"); break;
  case 7: strcpy(info, "You hear a whooshing sound."); break;
  case 8: strcpy(info, "Nothing appears to happen."); break;
  case 9: strcpy(info, "You hear a crackling sound."); break;
 }
 mpr(info);
 break;

 case 1: // a bit less harmless stuff
 switch(random2(2))
 {
  case 0:
  strcpy(info, "There is a shower of sparks.");
  mpr(info);
  break;
  case 1:
  strcpy(info, "The wind howls around you!");
  mpr(info);
  break;
 }
 break;

 case 2: // rather less harmless stuff
 switch(random2(2))
 {
  case 0:
  mpr("Electricity courses through your body.");
  ouch(check_your_resists(3 + random2(4) + random2(4), 5), 0, 18);
  break;
  case 1:
  strcpy(info, "Noxious gasses pour from your hands!");
  mpr(info);
  big_cloud(2, you[0].x_pos, you[0].y_pos, 20);
  break;
 }
 break;

 case 3: // less harmless stuff
 switch(random2(2))
 {
  case 0:
  strcpy(info, "You conjure up an explosion of electrical discharges!");
  mpr(info);
  strcpy(info, "Oops.");
  mpr(info);
  beam[0].type = 43;
  beam[0].damage = 105;
  beam[0].flavour = 5;
  beam[0].bx = you[0].x_pos;
  beam[0].by = you[0].y_pos;
  strcpy(beam[0].beam_name, "explosion");
  beam[0].colour = LIGHTBLUE;
  beam[0].thing_thrown = 1; // your explosion (is this right?)
  explosion(0 + (random2(4) != 0), beam);
  noisy(10, you[0].x_pos, you[0].y_pos);
  break;
  case 1:
  strcpy(info, "Venomous gasses pour from your hands!");
  mpr(info);
  big_cloud(4, you[0].x_pos, you[0].y_pos, 20);
  break;
 }
 break;

}
break; // end air


case 21: // poison
switch(spec_effect)
{
 case 0: // just a harmless message
 switch(random2(10))
 {
  case 0: strcpy(info, "You feel mildly nauseous."); break;
  case 1: strcpy(info, "You feel slightly ill."); break;
  case 2: strcpy(info, "Wisps of poison gas drift from your fingertips."); break;
  case 3: strcpy(info, "You feel a strange surge of energy!"); break;
  case 4: strcpy(info, "You feel faint for a moment."); break;
  case 5: strcpy(info, "You feel sick."); break;
  case 6: strcpy(info, "You feel odd."); break;
  case 7: strcpy(info, "You feel weak for a moment."); break;
  case 8: strcpy(info, "Nothing appears to happen."); break;
  case 9: strcpy(info, "You hear a slurping sound."); break;
 }
 mpr(info);
 break;

 case 1: // a bit less harmless stuff
 switch(random2(2))
 {
  case 0:
  if (player_res_poison() != 0) goto nothing_happening;
  strcpy(info, "You feel sick.");
  you[0].poison += 2 + random2(3);
  break;
  case 1:
  strcpy(info, "Noxious gasses pour from your hands!");
  mpr(info);
  place_cloud(2, you[0].x_pos, you[0].y_pos, 2 + random2(4));
  break;
 }
 break;

 case 2: // rather less harmless stuff
 switch(random2(2))
 {
  case 0:
  if (player_res_poison() != 0) goto nothing_happening;
  strcpy(info, "You feel very sick.");
  you[0].poison += 3 + random2(5) + random2(5);
  break;
  case 1:
  strcpy(info, "Noxious gasses pour from your hands!");
  mpr(info);
  big_cloud(2, you[0].x_pos, you[0].y_pos, 20);
  break;
 }
 break;

 case 3: // less harmless stuff
 switch(random2(3))
 {
  case 0:
  if (player_res_poison() != 0) goto nothing_happening;
  strcpy(info, "You feel incredibly sick.");
  you[0].poison += 10 + random2(10) + random2(10);
  break;
  case 1:
  strcpy(info, "Venomous gasses pour from your hands!");
  mpr(info);
  big_cloud(4, you[0].x_pos, you[0].y_pos, 20);
  break;
  case 2:
  if (player_res_poison() != 0) goto nothing_happening;
  lose_stat(100, 1);
  break;
 }
 break;

}
break; // end poison


/*
 11 = conjuration
 12 = enchantment
 13 = fire
 14 = ice
 15 = transmigration
 16 = necromancy
 17 = holy
 18 = summoning
 19 = divination
 20 = translocation
21 = poison
22 = earth
23 = air
*/
//}
//}
}
//break;

//}

return 1;

nothing_happening :
mpr("Nothing appears to happen.");

return 0;

}





char learn_a_spell(unsigned char splbook, char bitty)
{


char strungy [9];

unsigned int numbo;

int j;
int h;
int multip = 1;

int found_a_one = 0;

int icky = 0;


      bitty ++;




      numbo = you[0].inv_plus [splbook];


      itoa(numbo, strungy, 2);

      int kl = 0;

      for (h = 1; h < 7; h ++)
      {

      if (strungy [h] == '1') kl ++;

      if (kl == bitty)
      {
         learned = h;
         goto succes;
      }

      }

      return 0;







      succes : multip = 1;
      numbo = 0;
      found_a_one = 0;

      icky = strlen(strungy);

      multip = 1;
      numbo = 0;

      for (j = icky; j >= 1; j --)
      {

         if (strungy [j] != '1' && strungy [j] != '0') continue;
         if (strungy [j] == '0')
         {
            multip *= 2;
            continue;

         }

         if (strungy [j] == '1')
         {
               numbo += multip;
               multip *= 2;

         }
       }



      strcpy(strungy, "");

you[0].inv_plus [splbook] = numbo + 64;

   return 1;

} // end of change spellbook


int which_spellbook(void)
{
unsigned char nthing = 0;

if (you[0].spell_levels <= 0)
{
 strcpy(info, "You can't memorise any more spells yet.");
 mpr(info);
 return 0;
}

if (you[0].inv_no == 0)
        {
        strcpy(info, "You aren't carrying anything.");
        mpr(info);
        return 0;
        }

query :
strcpy(info, "You can memorise ");
itoa(you[0].spell_levels, st_prn, 10);
strcat(info, st_prn);
strcat(info, " more level");
if (!(st_prn [0] == '1' && st_prn [1] == 0)) strcat(info, "s");
strcat(info, " of spells");
 strcat(info, ".");

mpr(info);
strcpy(info, "Memorise from which spellbook?");
mpr(info);

keyin = get_ch();
if (keyin == '?' || keyin == '*')
{
        if (keyin == '?') nthing = get_invent(10);
        if (keyin == '*') nthing = get_invent(-1);
        if ((nthing >= 65 && nthing <= 90) || (nthing >= 97 && nthing <= 122))
        {
                keyin = nthing;
        } else
        {
       mesclr();
                goto query;
        }

}


sc_read_1 = (int) keyin;

if (sc_read_1 < 65 || (sc_read_1 > 90 && sc_read_1 < 97) || sc_read_1 > 122)
{
        strcpy(info, "You don't have any such object.");
        mpr(info);
        return 0;
}

sc_read_2 = conv_lett(sc_read_1);

if (you[0].inv_quant [sc_read_2] == 0)
{
        mpr("You don't have any such object.");
        return 0;
}

if (you[0].inv_class [sc_read_2] != 10)
{
        mpr("That isn't a spellbook!");
        return 0;
}

if (you[0].inv_type [sc_read_2] == 41) /* manuals */
{
        mpr("That isn't a spellbook!");
        return 0;
}

if (you[0].inv_type [sc_read_2] == 35) /* tome of destr */
{
  tome_of_power(sc_read_2);
  return 0;
}

spell_container = sc_read_2;

read_book(spell_container);

if (you[0].inv_plus [spell_container] == 64)
{
   strcpy(info, "That spellbook is empty.");
   mpr(info);
   return 0;
}

clrscr();
/*scrloc = 0;*/
return 1;

}

void read_book(char book_read)
{
char key2 = 0;

/* remember that this function is called from staff spells as well. */
if (you[0].inv_class [book_read] == 11) key2 = spellbook_contents(you[0].inv_plus [book_read], you[0].inv_type [book_read] + 40);
 else key2 = spellbook_contents(you[0].inv_plus [book_read], you[0].inv_type [book_read]);

if (you[0].inv_class [book_read] == 10)
{
 you[0].had_item [you[0].inv_type [book_read]] = 1;
 if (you[0].inv_type [book_read] == 0 || you[0].inv_type [book_read] == 1 || you[0].inv_type [book_read] == 2)
 {
  you[0].had_item [0] = 1;
  you[0].had_item [1] = 1;
  you[0].had_item [2] = 1;
 }
 if (you[0].inv_type [book_read] == 3 || you[0].inv_type [book_read] == 4)
 {
  you[0].had_item [3] = 1;
  you[0].had_item [4] = 1;
 }
}

#ifdef PLAIN_TERM
redraw_screen();
#endif
/* Put special book effects in another function which can be called from
   memorise as well */
you[0].turnover = 1;
you[0].inv_ident [book_read] = 1;
book_thing = key2;
keyin = key2;         // FIX this should probably go...

return;

}


void which_spell(void)
{
int chance = 0;
int powm;
char spell_string [60];
int levels_needed = 0;


if (which_spellbook() == 0) return;

sc_read_1 = (int) book_thing;

if (sc_read_1 < 65 || (sc_read_1 > 90 && sc_read_1 < 97) || sc_read_1 > 122)
{
        whatt : strcpy(info, "What?");
#ifdef PLAIN_TERM
        redraw_screen();
#endif
        mpr(info);
        return;
}


sc_read_2 = conv_lett(sc_read_1);


if (sc_read_2 > 7)
{
   goto whatt;
}


if (learn_a_spell(spell_container, sc_read_2) != 1)
{
   goto whatt;
}



unsigned char specspell = which_spell_in_book(you[0].inv_type [spell_container], learned);
char is_good = 1;


if (you[0].spell_no == 21 && specspell != 43) //if changed, must also change for priest in level_change. You can always memorise selective amnesia
{
 strcpy(info, "Your head is already too full of spells!");
 mpr(info);
 return;
}

if (you[0].species == 12 && spell_type(specspell, 17) == 1)
{
  strcpy(info, "You can't use this type of magic!");
  mpr(info);
  return;
}

//if (you[0].is_undead != 0 && specspell == 42)
if ((you[0].is_undead == 1 && undead_can_memorise(specspell) == 2) || (you[0].is_undead == 2 && undead_can_memorise(specspell) != 0))
{
  strcpy(info, "You can't use this spell.");
  mpr(info);
  return;
}

for (i = 0; i < 25; i ++)
{
        if (you[0].spells [i] == specspell)
   {
      #ifdef PLAIN_TERM
      redraw_screen();
      #endif
      strcpy(info, "You already know that spell!");
      mpr(info);
      you[0].turnover = 1;
      return;
   }
}

levels_needed = spell_value(specspell);

if (you[0].spell_levels < levels_needed)
{
   too_high :
   #ifdef PLAIN_TERM
   redraw_screen();
   #endif
   strcpy(info, "You can't memorise that many levels of magic yet!");
   mpr(info);
   you[0].turnover = 1;
   return;

}


if (you[0].xl < spell_value(specspell) || (is_good == 0 && you[0].xl < spell_value(specspell) * 2))
 goto too_high;
/*
types of spells:
11 = conjuration
12 = enchantment
13 = fire
14 = ice
15 = transmigration
16 = necromancy
17 = holy
18 = summoning
19 = divination
20 = translocation
21 = you[0].poison
*/


chance = 0;

powm = spell_spec(specspell, 0);

chance = spell_fail(specspell);

 if (chance >= 80)
 {
  strcpy(info, "This spell is very difficult to memorise.");
  mpr(info);
 } else
 if (chance >= 60)
 {
  strcpy(info, "This spell is quite difficult to commit to memory.");
  mpr(info);
 } else
 if (chance >= 45)
 {
  strcpy(info, "This spell is rather tricky to learn.");
  mpr(info);
 } else
 if (chance >= 30)
 {
  strcpy(info, "This spell is a little tricky to absorb.");
  mpr(info);
 }

#ifdef PLAIN_TERM
redraw_screen();
#endif

strcpy(info, "Memorise ");
spell_name(specspell, spell_string);
strcat(info, spell_string);
strcat(info, "?");
mpr(info);
keyin = getch();

if (keyin == 0) getch();
if (keyin != 'y' && keyin != 'Y')
{
#ifdef PLAIN_TERM
redraw_screen();
#endif
 return;
}
mesclr();

if (you[0].mutation [39] > 0 && random2(4) < you[0].mutation [39])
{
 mpr("The writing blurs into unreadable gibberish.");
 you[0].turnover = 1;
 return;
}

if (random2(40) + random2(40) + random2(40) < chance) //powm <= random2(chance) && spell_value(specspell) > 3)
{
#ifdef PLAIN_TERM
redraw_screen();
#endif
 strcpy(info, "You fail to memorise the spell.");
 mpr(info);
 you[0].turnover = 1;
 if (you[0].inv_type [spell_container] == 24)
 {
   mpr("The pages of the Necronomicon glow with a malevolent light...");
   miscast_effect(16, 8, random2(30) + random2(30) + random2(30), 100);
 }
 if (you[0].inv_type [spell_container] == 27) /* Demonology */
 {
   mpr("This book does not appreciate being disturbed by one of your ineptitude!");
   miscast_effect(18, 7, random2(30) + random2(30) + random2(30), 100);
 }
 if (you[0].inv_type [spell_container] == 33) /* Annihilations */
 {
   mpr("This book does not appreciate being disturbed by one of your ineptitude!");
   miscast_effect(11, 8, random2(30) + random2(30) + random2(30), 100);
 }

#ifndef DEBUG
 return;
#endif

#ifdef DEBUG
 strcpy(info, "But I'll let you memorise it anyway, okay?");
 mpr(info);
#endif

}


for (i = 0; i < 25; i ++)
{
        if (you[0].spells [i] == 210) break;
}


you[0].spells [i] = specspell;



you[0].spell_levels -= levels_needed;
you[0].spell_no ++;

you[0].delay_t = spell_value(you[0].spells [i]);
you[0].delay_doing = 3;
//scruggle = 0;

you[0].turnover = 1;
#ifdef PLAIN_TERM
redraw_screen();
#endif

naughty(9, 2 + random2(5));

}

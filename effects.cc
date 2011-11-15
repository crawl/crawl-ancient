#include "config.h"

#include <strings.h>
#include <string.h>

#include "externs.h"
#include "beam.h"
#include "files.h"
#include "fight.h"
#include "player.h"
#include "ouch.h"
#include "mstruct.h"
#include "misc.h"
#include "mutation.h"
#include "stuff.h"
#include "dungeon.h"
#include "itemname.h"
#include "skills2.h"
#include "view.h"
#include "spells0.h"
#include "spells2.h"
#include "spells3.h"

void torment(void)
{
 int dmi = 0;

 if (you[0].is_undead != 0)
 {
  strcpy(info, "You feel a surge of unholy energy.");
  mpr(info);
 } else
       {
        strcpy(info, "Your body is wracked with pain!");
        mpr(info);
        //you[0].hp -= (you[0].hp / 2) - 1;
        int tormdam = (you[0].hp / 2) - 1;
        if (tormdam <= 0) tormdam = 0;
        ouch(tormdam, 0, 0);
        you[0].hp_ch = 1;
       }

        for (dmi = 0; dmi < MNST; dmi++)
        {
                if (menv [dmi].m_class == -1) continue;

                if (mons_near(dmi) != 1) continue;

//              int mres = random2(100);
//              int mres2 = mon_resist_mag(mons_class [dmi], mons_HD [dmi]);

//              if (mres < mres2) continue;

  if (mons_holiness(menv [dmi].m_class) > 0) continue;

                // maybe an exemption for undead?
                // maybe do something fun for magic circles?

                menv [dmi].m_hp -= (menv [dmi].m_hp / 2) - 1;

                if (menv [dmi].m_ench [2] == 6) continue; // can't see it

                strcpy(info, monam (menv [dmi].m_sec, menv [dmi].m_class, menv [dmi].m_ench [2], 0));
                strcat(info, " convulses!");
                mpr(info);
        }

}


/*
char go_berserk(void)
{
  if (you[0].berserker != 0 | you[0].slow != 0) return 0;
  if (you[0].is_undead == 2) return 0;
  strcpy(info, "A red film seems to cover your vision as you go berserk!");
  mpr(info);
  strcpy(info, "You feel yourself moving faster!");
  mpr(info);
  strcpy(info, "You feel mighty!");
  mpr(info);
  you[0].berserker += 10 + random2(10) + random2(10);
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
*/
void banished(void)
{
 you_teleport2(0); // this is to ensure that you're standing on a suitable space
 grd [you[0].x_pos] [you[0].y_pos] = 96;
 down_stairs(1); // heh heh
}


char forget_spell(void)
{
char spc2;
if (you[0].spell_no <= 0) return 0;

do
{
 spc2 = random2(20);
 if (random2(1000) == 0) return 0; // safety check
} while (you[0].spells [spc2] == 210);


 you[0].spell_levels += spell_value(you[0].spells [spc2]);
 you[0].spell_no --;
 you[0].spells [spc2] = 210;
 return 1;
}


char lose_stat(char force_stat, char stat_loss)
{

if (force_stat == 100) force_stat = random2(3);

  if (player_sust_abil() != 0)
  {
   strcpy(info, "You feel slightly ill for a moment.");
   mpr(info);
   return 0;
  }
        switch(force_stat)
        {
                case 0: strcpy(info, "You feel weakened.");
                mpr(info);
                you[0].strength -= stat_loss;
                if (you[0].strength < 3) you[0].strength = 3;
                you[0].strength_ch = 1;
                burden_change();
                break;

                case 1: strcpy(info, "You feel clumsy.");
                mpr(info);
                you[0].dex -= stat_loss;
                if (you[0].dex < 3) you[0].dex = 3;
                you[0].dex_ch = 1;
                break;

                case 2: strcpy(info, "You feel dopey.");
                mpr(info);
                you[0].intel -= stat_loss;
                if (you[0].intel < 3) you[0].intel = 3;
                you[0].intel_ch = 1;
                break;

        }

return 1;

}





void direct_effect(struct bolt beam [1])
{
int hurted = 0;

switch(beam[0].type)
{
 case 0: // hellfire
 strcpy(info, "You are engulfed in a burst of hellfire!");
 mpr(info);
 hurted = random2(10) + random2(5) + 5;
 beam[0].flavour = 20; // lava, but it's hellfire anyway
 strcpy(beam[0].beam_name, "hellfire"); // for ouch
 check_your_resists(hurted, beam[0].flavour);
 scrolls_burn(4, 6);
 ouch(hurted, 0, 3);
 you[0].hp_ch = 1;
 break;

 case 1: // smiting
 strcpy(info, "Something smites you!");
 mpr(info);
 hurted = random2(5) + random2(5) + 3;
 strcpy(beam[0].beam_name, "smiting"); // for ouch
 ouch(hurted, 0, 3);
 you[0].hp_ch = 1;
 break;

 case 2: // brain feed
      if (random2(3) == 0 && you[0].intel > 3 && player_sust_abil() == 0)
      {
        strcpy(info, "Something feeds on your intelligence!");
        mpr(info);
        strcpy(info, "You feel stupid.");
        mpr(info);
        you[0].intel --;
        you[0].intel_ch = 1;
      } else {
                strcpy(info, "Something tries to feed on your intelligence!");
                mpr(info);
             }
 break;

 case 3: // mutation
 if (mutate(100) == 0) mpr("You feel very strange for a moment.");
 break;



}

return;

}


void mons_direct_effect(struct bolt beam [1], int i)
{

int o = menv [i].m_hit;
int hurted = 0;

switch(beam[0].type)
{


  case 0:
  if (mons_near(menv [i].m_hit) == 1)
  {
                strcpy(info, monam (menv [o].m_sec, menv [o].m_class, menv [o].m_ench [2], 0));
                strcat(info, " is engulfed in hellfire.");
                mpr(info);
  }
  strcpy(beam[0].beam_name, "hellfire");
  beam[0].flavour = 20;
  hurted = 0;
  hurted += random2(10) + random2(5) + 5;
  check_mons_resists(beam, o, hurted);
  menv [o].m_hp -= hurted;
  if (menv [o].m_beh == 0) menv [o].m_beh = 1;
  if (menv [o].m_hp <= 0)
  {
                monster_die(o, 4, i);
  }
  break;

  case 1:
  if (mons_near(o) == 1)
  {
                strcpy(info, monam (menv [o].m_sec, menv [o].m_class, menv [o].m_ench [2], 0));
                strcat(info, " is smitten.");
                mpr(info);
  }
  strcpy(beam[0].beam_name, "smiting");
  beam[0].flavour = 0;
  hurted = 0;
  hurted += random2(5) + random2(5) + 3;
  menv [o].m_hp -= hurted;
  if (menv [o].m_hp <= 0)
  {
    monster_die(o, 4, i);
  }
  break;


  case 2: // brain feed - not implemented here (nor, probably, can be)
                break;

} // end switch

} // end monster_direct_effect()



void random_uselessness(unsigned char ru, unsigned char sc_read_2)
{

char wc [30];

switch(ru)
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
        you[0].inv_quant [sc_read_2]++;
        mpr(info);
 burden_change();
        break;

        case 2:
        if (you[0].equip [0] != -1)
        {
        in_name(you[0].equip [0], 4, str_pass);
        strcpy(info, str_pass);
        strcat(info, " glows ");
        weird_colours(random2(256), wc);
        strcat(info, wc);
        strcat(info, " for a moment.");
        mpr(info);
        return;
        //strcat(info
        } else
             {
                strcpy(info, "Nothing appears to happen.");
                mpr(info);
                return;
             }
//      break;

        case 3:
        strcpy(info, "You hear the distant roaring of an enraged ");
                switch(random2(8))
                {
                        case 0: strcat(info, "frog!"); break;
                        case 1: strcat(info, "pill bug!"); break;
                        case 2: strcat(info, "millipede!"); break;
                        case 3: strcat(info, "eggplant!"); break;
                        case 4: strcat(info, "albino dragon!"); break;
                        case 5: strcat(info, "dragon!"); break;
                        case 6: strcat(info, "human!"); break;
                        case 7: strcat(info, "slug!"); break;
                }
        mpr(info);
        return;

        case 4:
        strcpy(info, "You smell ");
                switch(random2(8))
                {
                        case 0: strcat(info, "coffee."); break;
                        case 1: strcat(info, "salt."); break;
                        case 2: strcat(info, "burning hair!"); break;
                        case 3: strcat(info, "baking bread."); break;
                        case 4: strcat(info, "something weird."); break;
                        case 5: strcat(info, "wet wool."); break;
                        case 6: strcat(info, "sulphur."); break;
                        case 7: strcat(info, "fire and brimstone!"); break;
                }
        mpr(info);
        return;



        case 5:
        strcpy(info, "You experience a momentary feeling of inescapable doom!");
        mpr(info);
        return;

        case 6:
        strcpy(info, "Your ");
 switch(random2(3))
 {
                        case 0: strcat(info, "ears itch."); break;
                        case 1: strcat(info, "brain hurts!"); break;
                        case 2: strcat(info, "nose twitches suddenly!"); break;
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
                switch(random2(9))
                {
                        case 0: strcat(info, "snatches of song."); break;
                        case 1: strcat(info, "a voice call someone else's name."); break;
                        case 2: strcat(info, "a very strange noise."); break;
                        case 3: strcat(info, "roaring flame."); break;
                        case 4: strcat(info, "a very strange noise indeed."); break;
                        case 5: strcat(info, "the chiming of a distant gong."); break;
                        case 6: strcat(info, "the bellowing of a yak."); break;
                        case 7: strcat(info, "a crunching sound."); break;
                        case 8: strcat(info, "the tinkle of an enormous bell."); break;
                }
        mpr(info);
        return;


}

//abort();

}


void acquirement(void)
{
int thing_created = 0;
// Remember lava!
unsigned char class_wanted = 250;
unsigned char type_wanted = 250;

unsigned char force_plus = 0;

unsigned char acqc = 0;
char already_has [50];

char glof = 99;
unsigned char keyin;
int func_pass [10];

for (acqc = 0; acqc < 50; acqc ++)
{
 already_has [acqc] = 0;
}

strcpy(info, "This is a scroll of acquirement!");
mpr(info);
query :
strcpy(info, "a - weapon, b - armour, c - ring,");
mpr(info);
strcpy(info, "d - book, e - staff, f - miscellaneous");
mpr(info);
strcpy(info, "What kind of item would you like to acquire? ");
mpr(info);
keyin = get_ch();

switch(keyin)
{
 case 'a':
 case 'A':
 class_wanted = 0;
 break;

 case 'b':
 case 'B':
 class_wanted = 2;
 break;

 case 'c':
 case 'C':
 class_wanted = 7;
 break;

 case 'd':
 case 'D':
 class_wanted = 10;
 break;

 case 'e':
 case 'E':
 class_wanted = 11;
 break;

 case 'f':
 case 'F':
 class_wanted = 13;
 break;

}

if (class_wanted > 2)
{
for (acqc = 0; acqc < 52; acqc ++)
{
 if (you[0].inv_quant [acqc] > 0 && you[0].inv_class [acqc] == class_wanted)
 {
  already_has [you[0].inv_type [acqc]] = 1;

  if (class_wanted == 10)
  {
   if (you[0].inv_type [acqc] == 0)
   {
    already_has [1] = 1;
    already_has [2] = 1;
   }
   if (you[0].inv_type [acqc] == 1)
   {
    already_has [0] = 1;
    already_has [2] = 1;
   }
   if (you[0].inv_type [acqc] == 3) already_has [4] = 1;
   if (you[0].inv_type [acqc] == 4) already_has [3] = 1;
   //if (you[0].inv_type [acqc] == 23) already_has [24] = 1;
   //if (you[0].inv_type [acqc] == 24) already_has [23] = 1;
  }
/*  strcpy(info, "Found: ");
//  mpr(info);
itoa((int) you[0].inv_type [acqc], st_prn, 10); //grd [you[0].x_pos + xps - 17] [you[0].y_pos + yps - 7], st_prn, 10);
strcat(info, st_prn);
mpr(info);
*/
 }
}

if (class_wanted == 10) // these are books which that class wouldn't want (or shouldn't get, anyway)
{
/* if (you[0].clas == 5)
 {
  already_has [20] = 1; // holy books
  already_has [17] = 1;
  already_has [18] = 1;
 }
 if (you[0].clas == 12)
 {
  already_has [6] = 1;
  already_has [9] = 1;
 }
 if (you[0].clas == 13)
 {
  already_has [5] = 1;
  already_has [8] = 1;
 }*/
 if (you[0].clas == 2 | you[0].clas == 6)
 {
  already_has [15] = 1; // necromancy
  already_has [23] = 1;
  already_has [24] = 1;
  already_has [34] = 1;
 }
}

do
{
 switch(class_wanted)
 {

  case 7:
  type_wanted = random2(21);
  break;

  case 10:

/*  if (you[0].clas == 5 && already_has [24] == 0) type_wanted = 24; // necronomicon
  if (you[0].clas == 5 && already_has [15] == 0) type_wanted = 15; // death
  if (you[0].clas == 12 && already_has [8] == 0) type_wanted = 8; // fire
  if (you[0].clas == 13 && already_has [9] == 0) type_wanted = 9; // ice
  if (you[0].clas == 10 && already_has [14] == 0) type_wanted = 14; // conj
  if (you[0].clas == 14 && already_has [7] == 0) type_wanted = 7; // summ
  if (you[0].clas == 11 && already_has [12] == 0) type_wanted = 12; // ench 1
  if (you[0].clas == 11 && already_has [16] == 0) type_wanted = 16; // ench 1
  if (you[0].clas == 11 && already_has [22] == 0) type_wanted = 22; // ench 2
*/

// remember, put rarer books higher

 type_wanted = 99;

glof = 99;

which_book :
 switch(best_skill(25, 50, glof))
 {
  default:
  case 25: // spellcasting
  if (already_has [43] == 0) type_wanted = 43;
  if (already_has [42] == 0) type_wanted = 42;
  break;

  case 37: // Poison Magic
  if (already_has [32] == 0) type_wanted = 32;
  if (already_has [13] == 0) type_wanted = 13;
  break;

  case 36: // Earth
  if (already_has [40] == 0) type_wanted = 40;
  if (already_has [39] == 0) type_wanted = 39;
  break;

  case 35: // Air
  if (already_has [29] == 0) type_wanted = 29;
  if (already_has [28] == 0) type_wanted = 28;
  break;

  case 34: // Ice
  if (already_has [9] == 0) type_wanted = 9;
  if (already_has [6] == 0) type_wanted = 6;
  break;

  case 33: // fire
  if (already_has [8] == 0) type_wanted = 8;
  if (already_has [5] == 0) type_wanted = 5;
  break;

  case 28: // summ
  if (already_has [27] == 0) type_wanted = 27;
  if (already_has [7] == 0) type_wanted = 7;
  if (already_has [25] == 0) type_wanted = 25;
  break;

  case 27: // ench
  if (already_has [12] == 0) type_wanted = 12;
  if (already_has [20] == 0) type_wanted = 20;
  if (already_has [36] == 0) type_wanted = 36;
  if (already_has [16] == 0) type_wanted = 16;
  if (already_has [26] == 0) type_wanted = 26;
  break;

  case 26: // conj
  if (already_has [33] == 0) type_wanted = 33;
  if (already_has [3] == 0) type_wanted = 3;
  break;

  case 29: // necro
  if (already_has [24] == 0) type_wanted = 24;
  if (already_has [34] == 0) type_wanted = 34;
  if (already_has [15] == 0) type_wanted = 15;
  if (already_has [23] == 0) type_wanted = 23;
  break;

 }

if (type_wanted == 99 && glof == 99)
{
 glof = best_skill(26, 50, 99);
 goto which_book;
}


if (type_wanted == 99) type_wanted = random2(27);


  force_plus = 127;

  // should guarantee a full spellbook, so:
  spellbook_template(type_wanted, func_pass);
  if (func_pass [6] == 210) force_plus = 126;
  if (func_pass [5] == 210) force_plus = 124;
  if (func_pass [4] == 210) force_plus = 120;
  if (func_pass [3] == 210) force_plus = 112;
  if (func_pass [2] == 210) force_plus = 96;

  break;

  case 11:
  type_wanted = random2(18);
  if (class_wanted == 11 && type_wanted > 9 && random2(5) != 0) type_wanted = random2(10);
  break;

  case 13:
  type_wanted = random2(6);
  break;

  default: mesclr();
  goto query;
 }
} while(already_has [type_wanted] == 1 && random2(200) != 0);

} else
{
 if (class_wanted == 0) type_wanted = 1 + random2(18); // weapons - no clubs
  else type_wanted = 250; // always get random armour
}

if (grd [you[0].x_pos] [you[0].y_pos] == 61 | grd [you[0].x_pos] [you[0].y_pos] == 62)
{
 strcpy(info, "You hear a splash."); // how sad (and stupid)
 mpr(info);
 return;
}

thing_created = /*items(you[0].unique_items, 1, you[0].item_description,
       grd, class_wanted, type_wanted, 1, 351, 250);*/
                items(1, class_wanted, type_wanted, 1, 351, 250);


if (you[0].species != 13) strcpy(info, "Something appears at your feet!");
 else strcpy(info, "Something appears before you!");
mpr(info);

if (force_plus != 0) mitm.iplus [thing_created] = force_plus;

int what_was_there = igrd [you[0].x_pos] [you[0].y_pos];
mitm.ilink [thing_created] = what_was_there;
igrd [you[0].x_pos] [you[0].y_pos] = thing_created;

return;

}



char recharge_wand(void)
{

// note that the scroll of recharging also recharges weapons of electrocution; see the scroll function

if (you[0].equip [0] == -1)
{
 return 0;
}

if (you[0].inv_class [you[0].equip [0]] != 3)
{
 return 0; // not a wand
}

char charge_gain = 8;

if (you[0].inv_type [you[0].equip [0]] == 7 | you[0].inv_type [you[0].equip [0]] == 8)
 charge_gain = 5;

if (you[0].inv_type [you[0].equip [0]] == 12 | you[0].inv_type [you[0].equip [0]] == 14 | you[0].inv_type [you[0].equip [0]] == 17)
 charge_gain = 4;

item_name(you[0].inv_plus2 [you[0].equip [0]], you[0].inv_class [you[0].equip [0]], you[0].inv_type [you[0].equip [0]], you[0].inv_dam [you[0].equip [0]], you[0].inv_plus [you[0].equip [0]], you[0].inv_quant [you[0].equip [0]], you[0].inv_ident [you[0].equip [0]], 4, str_pass);
strcpy(info, str_pass);
strcat(info, " glows for a moment.");
mpr(info);

you[0].inv_plus [you[0].equip [0]] += random2(charge_gain) + random2(charge_gain) + random2(charge_gain) + 1;

if (you[0].inv_plus [you[0].equip [0]] > charge_gain * 3)
 you[0].inv_plus [you[0].equip [0]] = charge_gain * 3;

return 1;

}

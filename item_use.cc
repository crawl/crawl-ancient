#include "config.h"

#include <string.h>

#include "externs.h"
//#include "it_use2.h"


#include "bang.h"
#include "beam.h"
//#include "chardump.h"
//#include "command.h"
//#include "debug.h"
//#include "decks.h"
#include "describe.h"
#include "direct.h"
//#include "dungeon.h"
#include "effects.h"
#include "fight.h"
#include "files.h"
#include "food.h"
#include "invent.h"
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
//#include "monstuff.h"
//#include "mstruct.h"
//#include "mstuff2.h"
#include "newgame.h"
#include "ouch.h"
#include "output.h"
#include "player.h"
//#include "priest.h"
//#include "shopping.h"
#include "skills.h"
#include "skills2.h"
//#include "spell.h"
#include "spells.h"
#include "spells0.h"
#include "spells1.h"
#include "spells2.h"
#include "spells3.h"
#include "stuff.h"
#include "transform.h"
#include "view.h"

void throw_it(struct bolt beam [1], int throw_2);

extern int book_thing; /* defined in spells.cc */


void wield_weapon(void)
{

unsigned char nthing = 0;
unsigned char i_dam = 0;
char str_pass [80];

if (you[0].inv_no == 0)
        {
        strcpy(info, "You aren't carrying anything.");
        mpr(info);
        return;
        }

if (you[0].berserker != 0)
{
 strcpy(info, "You are too berserk!");
 mpr(info);
 return;
}

if (you[0].attribute [5] != 0)
{
 if (can_equip(0) == 0)
 {
  mpr("You can't wield anything in your present form.");
  return;
 }
}

if ((you[0].equip [0] != -1) && you[0].inv_class [you[0].equip [0]] == 0 && you[0].inv_plus [you[0].equip [0]] > 80)
{
        strcpy(info, "You can't unwield your weapon to draw a new one!");
        mpr(info);
        return;
}


query : strcpy(info, "Wield which item (- for none)?");
mpr(info);

// test relay_message();

unsigned char keyin = get_ch();

if (keyin == '?')
{
        //inventory(0);
        nthing = get_invent(0);
//invent(you[0].inv_plus2, 0, you[0].inv_quant, you[0].inv_dam, you[0].inv_class, you[0].inv_type, you[0].inv_plus, you[0].inv_ident, you[0].equip [0], you[0].equip [6], you[0].equip [5], you[0].equip [2], you[0].equip [1], you[0].equip [3], you[0].equip [4], you[0].ring);
        if ((nthing >= 65 && nthing <= 90) | (nthing >= 97 && nthing <= 122))
        {
                keyin = nthing;
        } else
        {
      mesclr();
      goto query;
        /*              strcpy(info, "Wield which item?"); mpr(info);
                relay_message();
                keyin = get_ch();*/
        }
}

if (keyin == '*')
{
        //inventory(-1);
        nthing = get_invent(-1);
//invent(you[0].inv_plus2, -1, you[0].inv_quant, you[0].inv_dam, you[0].inv_class, you[0].inv_type, you[0].inv_plus, you[0].inv_ident, you[0].equip [0], you[0].equip [6], you[0].equip [5], you[0].equip [2], you[0].equip [1], you[0].equip [3], you[0].equip [4], you[0].ring);
        if ((nthing >= 65 && nthing <= 90) | (nthing >= 97 && nthing <= 122))
        {
                keyin = nthing;
        } else
        {
                strcpy(info, "Wield which item?"); mpr(info);
                // test relay_message();
                keyin = get_ch();
        }
}


int item_wield_1 = (int) keyin;

if (keyin == '-')
{
 if (you[0].equip [0] != -1)
 {
  unwield_item(you[0].equip [0]);
  you[0].turnover = 1;
 }
 you[0].equip [0] = -1;
 strcpy(info, "You are empty handed.");
 mpr(info);
 you[0].time_taken *= 3;
 you[0].time_taken /= 10;
 return;
}

if (item_wield_1 < 65 | (item_wield_1 > 90 && item_wield_1 < 97) | item_wield_1 > 122)
{
        strcpy(info, "You don't have any such object.");
        mpr(info);
        return;
}

int item_wield_2 = conv_lett(item_wield_1);

if (item_wield_2 == you[0].equip [0])
{
        strcpy(info, "You are already wielding that!");
        mpr(info);
        return;
}

if (item_wield_2 == you[0].equip [6] | item_wield_2 == you[0].equip [5] | item_wield_2 == you[0].equip [2] | item_wield_2 == you[0].equip [1] | item_wield_2 == you[0].equip [4] | item_wield_2 == you[0].equip [3]) // later add:   | == helmet etc
{
        strcpy(info, "You are wearing that object!");
        mpr(info);
        return;
}


if (you[0].inv_quant [item_wield_2] == 0)
{
        strcpy(info, "You don't have any such object.");
        mpr(info);
        return;
}                                // 2-hands 7,10,12, {20,21}

if (you[0].inv_class [item_wield_2] != 0)
{
 if (you[0].inv_class [item_wield_2] == 11 && you[0].equip [5] != -1)
 {
      strcpy(info, "You can't wield that with a shield.");
      mpr(info);
      return;
 }

 if (you[0].equip [0] != -1) unwield_item(you[0].equip [0]);
        you[0].equip [0] = item_wield_2;
} else
        {

   if ((you[0].species < 15 | you[0].species > 17) && mass(you[0].inv_class [item_wield_2], you[0].inv_type [item_wield_2]) >= 500)
   {
      strcpy(info, "That's too large and heavy for you to wield.");
      mpr(info);
      return;
   }

   if ((you[0].species == 9 | you[0].species == 14 | you[0].species == 11) && (you[0].inv_type [item_wield_2] == 7 | you[0].inv_type [item_wield_2] == 10 | you[0].inv_type [item_wield_2] == 12 | you[0].inv_type [item_wield_2] == 17 | you[0].inv_type [item_wield_2] == 19))
   {
      strcpy(info, "That's too large for you to wield.");
      mpr(info);
      return; // halflings or kobolds
   }


   if ((you[0].inv_type [item_wield_2] == 7 | you[0].inv_type [item_wield_2] == 10 | you[0].inv_type [item_wield_2] == 12 | you[0].inv_type [item_wield_2] == 17 | you[0].inv_type [item_wield_2] == 18 | you[0].inv_type [item_wield_2] == 19 | you[0].inv_type [item_wield_2] == 20 | you[0].inv_type [item_wield_2] == 21 | you[0].inv_type [item_wield_2] == 25 | you[0].inv_type [item_wield_2] == 27) && you[0].equip [5] != -1)
   {
      strcpy(info, "You can't wield that with a shield.");
      mpr(info);
      return;
   }

   if (you[0].is_undead != 0 && (you[0].inv_dam [item_wield_2] % 30 == 3 | you[0].inv_dam [item_wield_2] % 30 == 14))
   {
      strcpy(info, "This weapon will not allow you to wield it.");
      mpr(info);
      you[0].turnover = 1;
      return; // Mummies can't wield holy weapons
   }


 if (you[0].equip [0] != -1) unwield_item(you[0].equip [0]);
        you[0].equip [0] = item_wield_2;
        }

strcpy(info, " ");
/*                      strncpy(info, letters [item_wield_2], 1);*/
                        if (item_wield_2 <= 25) info [0] = item_wield_2 + 97;
                                else info [0] = item_wield_2 + 39;

                        info [1] = 0; /* This null-terminates it, right? */

                        strcat(info, " - ");

                        in_name(item_wield_2, 3, str_pass);
                        strcat(info, str_pass);


                        strcat(info, " (weapon in hand)");
                        mpr(info);

you[0].turnover = 1;

if (you[0].inv_plus [item_wield_2] > 80 && you[0].inv_class [item_wield_2] == 0)
{
        strcpy(info, "It sticks to your hand!");
// if (you[0].inv_ident [item_wield_2] == 0)    you[0].inv_ident [item_wield_2] = 2;
        mpr(info);
}

 if (you[0].inv_class [item_wield_2] == 13 && you[0].inv_type [item_wield_2] == 5)
 {
  strcpy(info, "The area is filled with flickering shadows.");
  mpr(info);
  you[0].special_wield = 50;
 }

 if (you[0].inv_class [item_wield_2] == 0 && you[0].inv_ident [item_wield_2] == 0)      you[0].inv_ident [item_wield_2] = 2;

if (you[0].inv_class [item_wield_2] == 0 && you[0].inv_dam [item_wield_2] != 0)
{

if (you[0].inv_ident [item_wield_2] == 1) you[0].inv_ident [item_wield_2] = 2;

i_dam = you[0].inv_dam [item_wield_2];

if (you[0].inv_dam [item_wield_2] <= 180)
{
 i_dam = you[0].inv_dam [item_wield_2] % 30;
}

switch(i_dam) //you[0].inv_dam [item_wield_2] % 30)
{

  case 1:
  strcpy(info, "It bursts into flame!");
  mpr(info);
  break;

  case 2:
  strcpy(info, "It glows with a cold blue light!");
  mpr(info);
  break;

  case 3:
  strcpy(info, "It softly glows with a divine radiance!");
  mpr(info);
  break;

  case 4:
  strcpy(info, "You hear the crackle of electricity.");
  mpr(info);
  break;

  case 5:
  strcpy(info, "You feel a sudden desire to kill orcs!");
  if (you[0].species == 10) strcpy(info, "You feel a sudden desire to commit suicide.");
  mpr(info);
  break;

  case 6:
  strcpy(info, "It begins to drip with poison!");
  mpr(info);
  break;

  case 7:
  strcpy(info, "You feel protected!");
  mpr(info);
/*  player_AC() += 5;*/
  you[0].AC_ch = 1;
  break;

  case 8:
  strcpy(info, "You sense an unholy aura.");
                mpr(info);
                break;

  case 9:
  strcpy(info, "Your hands tingle!");
                mpr(info);
                break;

  case 11:
  strcpy(info, "It glows red for a moment.");
                mpr(info);
                break;

  case 12:
  strcpy(info, "It is covered in frost.");
                mpr(info);
                break;

  case 13:
  strcpy(info, "You feel a strange hunger.");
  you[0].hunger_inc += 6;
                mpr(info);
                break;

  case 14:
  strcpy(info, "You sense a holy aura.");
                mpr(info);
                break;

  case 15:
  strcpy(info, "A searing pain shoots up your arm!");
                mpr(info);
                break;

  case 16:
  miscast_effect(20, 9, 90, 100);
  break;

  case 181:
  strcpy(info, "The Singing Sword hums in delight!");
                mpr(info);
  you[0].special_wield = 1;
                break;

  case 182:
  strcpy(info, "You feel bloodthirsty!");
                mpr(info);
  you[0].special_wield = 2;
                break;

  case 183:
  strcpy(info, "A shiver runs down your spine.");
                mpr(info);
  you[0].special_wield = 3;
                break;

  case 184:
//  strcpy(info, "You feel bloodthirsty!");
//              mpr(info);
  you[0].special_wield = 4;
                break;

  case 185:
  strcpy(info, "You feel pruney.");
                mpr(info);
  you[0].special_wield = 0;
                break;

  case 186:
  strcpy(info, "A terribly searing pain shoots up your arm!");
                mpr(info);
  you[0].special_wield = 6;
                break;

  case 187:
  strcpy(info, "You sense an extremely unholy aura.");
                mpr(info);
  you[0].special_wield = 7;
                break;

  case 191:
  strcpy(info, "You sense an aura of extreme power.");
                mpr(info);
  you[0].special_wield = 8;
                break;

  case 193:
  strcpy(info, "You smell chlorine.");
                mpr(info);
/*                you[0].res_poison ++;
                you[0].spec_poison ++;
                you[0].mag_abil ++;*/
//  you[0].special_wield = 8;
                break;

  case 194:
  strcpy(info, "You feel a strange hunger, and smell blood in the distance...");
  you[0].hunger_inc += 9;
                mpr(info);
                break;

  case 195:
  miscast_effect(19, 9, 90, 100);
  you[0].special_wield = 9;
  break;



} // end of switch

}

if (you[0].inv_class [item_wield_2] == 11)
{
 switch(you[0].inv_type [item_wield_2])
        {
 case 0: // wizardry
/* you[0].mag_abil += 4;*/
        break;

 case 1: // magic
 you[0].ep_max += 13;
 you[0].base_ep2 += 13;
 you[0].ep_ch = 1;
/* you[0].magic_battery ++;*/
 calc_ep();
 break;

/* case 2: // fire
        you[0].spec_fire ++;
  player_res_fire() ++;
        break;

        case 3: // cold
 you[0].spec_cold ++;
 player_res_cold() ++
        break;

 case 4:
 you[0].spec_poison ++;
 you[0].res_poison ++;
        break;

 case 5:
 you[0].energy ++;
 break;

 case 6:
 you[0].spec_death ++;
 break;

 case 7:
 you[0].spec_conj ++;
 break;

 case 8:
 you[0].spec_ench ++;
 break;

 case 9:
 you[0].spec_summ ++;
 break;

 case 25: // air
 you[0].attribute [1] ++;
 you[0].attribute [0] ++; // res elec
 break;

 case 26:
 you[0].attribute [2] ++;
 break;
*/
 }
}

/*if (you[0].inv_dam [item_wield_2] == 6)
{
  strcpy(info, "You feel protected.");
  mpr(info);
  if (you[0].inv_ident [item_wield_2] == 1) you[0].inv_ident [item_wield_2] = 2;
}*/
 you[0].time_taken *= 5;
 you[0].time_taken /= 10;


} // end of wield_weapon()



void wear_armour(void)
{

unsigned char nthing;
char armr = you[0].equip [6];
/*char ev_change = 0;*/
char wh_equip = 0;

if (you[0].inv_no == 0)
        {
        strcpy(info, "You aren't carrying anything.");
        mpr(info);
        return;
        }

if (you[0].berserker != 0)
{
 strcpy(info, "You are too berserk!");
 mpr(info);
 return;
}

query : strcpy(info, "Wear which item?");
mpr(info);

// test relay_message();

unsigned char keyin = get_ch();

if (keyin == '?')
{
        nthing = get_invent(2);
//invent(you[0].inv_plus2, 2, you[0].inv_quant, you[0].inv_dam, you[0].inv_class, you[0].inv_type, you[0].inv_plus, you[0].inv_ident, you[0].equip [0], you[0].equip [6], you[0].equip [5], you[0].equip [2], you[0].equip [1], you[0].equip [3], you[0].equip [4], you[0].ring);
        if ((nthing >= 65 && nthing <= 90) | (nthing >= 97 && nthing <= 122))
        {
                keyin = nthing;
        } else
        {
      mesclr();
   goto query;
        /*              strcpy(info, "Wear which item?"); mpr(info);
                relay_message();
                keyin = get_ch();*/
        }
}

if (keyin == '*')
{
        nthing = get_invent(-1);
//invent(you[0].inv_plus2, -1, you[0].inv_quant, you[0].inv_dam, you[0].inv_class, you[0].inv_type, you[0].inv_plus, you[0].inv_ident, you[0].equip [0], you[0].equip [6], you[0].equip [5], you[0].equip [2], you[0].equip [1], you[0].equip [3], you[0].equip [4], you[0].ring);
        if ((nthing >= 65 && nthing <= 90) | (nthing >= 97 && nthing <= 122))
        {
                keyin = nthing;
        } else
        {
                strcpy(info, "Wear which item?"); mpr(info);
                // test relay_message();
                keyin = get_ch();
        }
}


int armour_wear_1 = (int) keyin; // have to do shield, helmet etc later

/*if (keyin == '-')
{
you[0].damage = 1;
you[0].equip [0] = -1;
strcpy(info, "You are empty handed.");
mpr(info);
damage_ch = 1;
return;
}*/

if (armour_wear_1 < 65 | (armour_wear_1 > 90 && armour_wear_1 < 97) | armour_wear_1 > 122)
{
        strcpy(info, "You don't have any such object.");
        mpr(info);
        return;
}

/*
int you[0].equip [6] = -1;
int you[0].equip [1] = -1;
int you[0].equip [2] = -1;
int you[0].equip [3] = -1;
int you[0].equip [4] = -1;
int you[0].equip [5] = -1;
*/

int armour_wear_2 = conv_lett(armour_wear_1);

armr = armour_wear_2;

if (you[0].inv_class [armour_wear_2] != 2)
{
        strcpy(info, "You can't wear that thing.");
        mpr(info);
        return;
}

if (armour_wear_2 == you[0].equip [6] | armour_wear_2 == you[0].equip [1] | armour_wear_2 == you[0].equip [5] | armour_wear_2 == you[0].equip [2] | armour_wear_2 == you[0].equip [3] | armour_wear_2 == you[0].equip [4]) // or armour_wear_w == helmet etc
{
        strcpy(info, "You are already wearing that!");
        mpr(info);
        return;
}

if (you[0].inv_quant [armour_wear_2] == 0)
{
        strcpy(info, "You don't have any such object.");
        mpr(info);
        return;
}

if (armour_wear_2 == you[0].equip [0])
{
        strcpy(info, "You are wielding that object!");
        mpr(info);
        return;
}

if ((you[0].inv_type [armour_wear_2] < 8 | you[0].inv_type [armour_wear_2] > 14) && you[0].equip [1] != -1)
{
        strcpy(info, "You can't wear that over your cloak.");
        mpr(info);
        return;
}


if ((you[0].inv_type [armour_wear_2] < 8 | you[0].inv_type [armour_wear_2] > 14) && you[0].equip [6] != -1)
{
        strcpy(info, "You are already wearing some body armour.");
        mpr(info);
        return;
}


if ((you[0].inv_type [armour_wear_2] == 8 | you[0].inv_type [armour_wear_2] == 14 | you[0].inv_type [armour_wear_2] == 13) && you[0].equip [5] != -1)
{
        strcpy(info, "You are already holding a shield.");
        mpr(info);
        return;
}

if (you[0].equip [0] != -1 && (you[0].inv_type [armour_wear_2] == 8 | you[0].inv_type [armour_wear_2] == 13 | you[0].inv_type [armour_wear_2] == 14) && (you[0].inv_class [you[0].equip [0]] == 0 && (you[0].inv_type [you[0].equip [0]] == 7 | you[0].inv_type [you[0].equip [0]] == 10 | you[0].inv_type [you[0].equip [0]] == 12 | you[0].inv_type [you[0].equip [0]] == 17 | you[0].inv_type [you[0].equip [0]] == 18 | you[0].inv_type [you[0].equip [0]] == 19 | you[0].inv_type [you[0].equip [0]] == 20 | you[0].inv_type [you[0].equip [0]] == 21)) | you[0].inv_class [you[0].equip [0]] == 11)
{
   strcpy(info, "You can't wear a shield with a two-handed weapon.");
   mpr(info);
   return;
}

if (you[0].inv_type [armour_wear_2] == 10 && you[0].equip [2] != -1)
{
        strcpy(info, "You are already wearing a helmet.");
        mpr(info);
        return;
}

if (you[0].inv_type [armour_wear_2] == 9 && you[0].equip [1] != -1)
{
        strcpy(info, "You are already wearing a cloak.");
        mpr(info);
        return;
}

if (you[0].inv_type [armour_wear_2] == 11 && you[0].equip [3] != -1)
{
        strcpy(info, "You are already wearing some gloves.");
        mpr(info);
        return;
}

/*if (you[0].inv_type [armour_wear_2] == 12 && you[0].species == 13 && you[0].attribute [5] == 0) / * Naga * /
{
        strcpy(info, "You don't have feet!"); / * Naga * /
        mpr(info);
        return;
}*/



//if (you[0].attribute [5] != 0)
//{
 wh_equip = 6;
 switch(you[0].inv_type [armour_wear_2])
 {
  case 13:
  case 14:
  case 8: wh_equip = 5; break;
  case 9: wh_equip = 1; break;
  case 10: wh_equip = 2; break;
  case 11: wh_equip = 3; break;
  case 12: wh_equip = 4; break;
 }

 if (can_equip(wh_equip) == 0)
 {
  mpr("You can't wear that in your present form.");
  return;
 }
//}


if (you[0].inv_type [armour_wear_2] == 12 && you[0].equip [4] != -1)
{
        strcpy(info, "You are already wearing some boots.");
        mpr(info);
        return;
}

if (you[0].inv_type [armour_wear_2] == 17 && (you[0].skills [13] < 10 | you[0].species == 11 | you[0].species == 9 | you[0].species == 14 | (you[0].species >= 2 && you[0].species <= 6))) // no halflings or elves
{
/* if (you[0].species == 13 | you[0].species == 11 | you[0].species == 14 | you[0].species == 9 | (you[0].species >= 2 && you[0].species <= 6))
 {
  strcpy(info, "I'm afraid you have the wrong body type. Try something lighter.");
  mpr(info);
  return;
 } else*/
 {
  mpr("This armour is very cumbersome.");
 }
}

if (you[0].species >= 15 && you[0].species <= 29)
{
 if (you[0].inv_type [armour_wear_2] >= 1 && you[0].inv_type [armour_wear_2] <= 7 | you[0].inv_type [armour_wear_2] >= 10 && you[0].inv_type [armour_wear_2] <= 13 | you[0].inv_type [armour_wear_2] == 17)
 {
  strcpy(info, "This armour doesn't fit on your body.");
  mpr(info);
  return;
 }
}

if (you[0].inv_type [armour_wear_2] == 7 && (you[0].skills [13] < 5 | you[0].species == 9 | you[0].species == 14 | (you[0].species >= 2 && you[0].species <= 6) | you[0].species == 11))
{
/* if (you[0].species == 13 | you[0].species == 9 | you[0].species == 14 | (you[0].species >= 2 && you[0].species <= 6) | you[0].species == 11)
 {
  mpr("I'm afraid you have the wrong body type. Try something lighter.");
  return;
 } else*/ mpr("This armour is uncomfortably cumbersome.");
}

if (you[0].inv_type [armour_wear_2] == 6 | you[0].inv_type [armour_wear_2] == 5 && (you[0].skills [13] < 3 | you[0].species == 14 | you[0].species == 9))
{
/* if (you[0].species == 14 | you[0].species == 9)
 {
  mpr("I'm afraid you have the wrong body type. Try something lighter.");
  return;
 } else*/ mpr("You find this armour uncomfortably cumbersome.");
}

//if (you[0].inv_type [armour_wear_2] == 17 && you[0].skills [13] < 7)
// crystal plate:
if (you[0].inv_type [armour_wear_2] == 17 && (you[0].skills [13] < 8 | you[0].species == 14 | you[0].species == 9 | (you[0].species >= 2 && you[0].species <= 6)))
{
/* if (you[0].species == 14 | you[0].species == 9 | (you[0].species >= 2 && you[0].species <= 6))
 {
  mpr("I'm afraid you have the wrong body type. Try something lighter.");
  return;
 } else */ mpr("This armour is quite cumbersome.");
}

// storm dragon scale mail/hide
if ((you[0].inv_type [armour_wear_2] == 26 | you[0].inv_type [armour_wear_2] == 27) && (you[0].skills [13] < 5))
{
 mpr("This armour is quite cumbersome.");
}

// golden dragon scale mail/hide
if ((you[0].inv_type [armour_wear_2] == 28 | you[0].inv_type [armour_wear_2] == 29) && (you[0].skills [13] < 7))
{
 mpr("This armour is quite cumbersome.");
}



 if (you[0].inv_type [armour_wear_2] == 10 && you[0].inv_plus2 [armour_wear_2] >= 2)
 {
/*      if (you[0].inv_plus [armour_wear_2] > 130)
                player_AC() += (you[0].inv_plus [armour_wear_2] - 150);
         else player_AC() += you[0].inv_plus [armour_wear_2] - 50;*/
 } else
  if (you[0].inv_type [armour_wear_2] == 8 | you[0].inv_type [armour_wear_2] == 13 | you[0].inv_type [armour_wear_2] == 14)
 {
/*   player_shield_class() = 0; // this is later
        if (you[0].inv_plus [armour_wear_2] > 130)
                player_shield_class() += you[0].inv_plus [armour_wear_2] - 150;
         else
        player_shield_class() += you[0].inv_plus [armour_wear_2] - 50;*/
 } else
        {
/*      if (you[0].inv_plus [armour_wear_2] > 130)
                player_AC() += you[0].inv_plus [armour_wear_2] - 150; //property [you[0].inv_class [armour_wear_2]] [you[0].inv_type [armour_wear_2]] [0] + (you[0].inv_plus [armour_wear_2] - 150);*/
/*       else*/
/*      player_AC() += you[0].inv_plus [armour_wear_2] - 50; //property [you[0].inv_class [armour_wear_2]] [you[0].inv_type [armour_wear_2]] [0] + you[0].inv_plus [armour_wear_2] - 50;
*/
        armr = armour_wear_2;

/*if (you[0].inv_type [armour_wear_2] < 8 | you[0].inv_type [armour_wear_2] > 14)
   player_AC() += property(2, you[0].inv_type [armr], 0) * (15 + you[0].skills [13]) / 15;
      else player_AC() += property [2] [you[0].inv_type [armr]] [0];*/

if (you[0].species == 13 && (you[0].inv_type [armr] < 8 | you[0].inv_type [armr] > 14))
{
/* player_AC() -= property [2] [you[0].inv_type [armr]] [0] / 2;*/
}

//      player_evasion() += property [you[0].inv_class [armour_wear_2]] [you[0].inv_type [armour_wear_2]] [1];



/*
if (class_armour(you[0].clas) == 1) //you[0].clas == 0 | you[0].clas == 2 | you[0].clas == 4 | you[0].clas == 6) // fighter or priest
{
 player_evasion() += property [you[0].inv_class [armour_wear_2]] [you[0].inv_type [armour_wear_2]] [1] / 2;
} else
      {
       player_evasion() += property [you[0].inv_class [armour_wear_2]] [you[0].inv_type [armour_wear_2]] [1];
      }
*/
/*
if (you[0].inv_type [armour_wear_2] < 8 | you[0].inv_type [armour_wear_2] > 14)
{
 ev_change = property [2] [you[0].inv_type [armour_wear_2]] [1];
 ev_change += you[0].skills [13] / 3;
 if (ev_change > property [2] [you[0].inv_type [armour_wear_2]] [1] / 3) ev_change = property [2] [you[0].inv_type [armour_wear_2]] [1] / 3;
/ * player_evasion() += ev_change;  remember that it's negative * /
}
*/

        }

strcpy(info, " ");
/*                      strncpy(info, letters [armour_wear_2], 1);*/
                        if (armour_wear_2 <= 25) info [0] = armour_wear_2 + 97;
                                else info [0] = armour_wear_2 + 39;

                        info [1] = 0;

                        strcat(info, " - ");

/*
                        if (inv_name [armour_wear_2] [0] == 97 ^ inv_name [armour_wear_2] [0] == 101 ^ inv_name [armour_wear_2] [0] == 105 ^ inv_name [armour_wear_2] [0] == 111 ^ inv_name [armour_wear_2] [0] == 117)
                        {
                                strcat(info, "an ");
                        }
                        if ((inv_name [armour_wear_2] [0] != 97 && inv_name [armour_wear_2] [0] != 101 && inv_name [armour_wear_2] [0] != 105 && inv_name [armour_wear_2] [0] != 111 && inv_name [armour_wear_2] [0] != 117 && inv_name [armour_wear_2] [0] > 96) | inv_name [armour_wear_2] [0] == 43 | inv_name [armour_wear_2] [0] == 45)
                        {
                                strcat(info, "a ");
                        }
*/
                        //item_name(you[0].inv_class [armour_wear_2], you[0].inv_type [armour_wear_2], you[0].inv_dam [armour_wear_2], you[0].inv_plus [armour_wear_2], you[0].inv_quant [armour_wear_2], you[0].inv_ident [armour_wear_2], 3, str_pass);


if (you[0].inv_type [armour_wear_2] < 8 | you[0].inv_type [armour_wear_2] > 14)
{
/* player_evasion() -= ev_mod(); */
 you[0].equip [6] = armour_wear_2;
/* player_evasion() += ev_mod(); */
 if (you[0].duration [1] != 0)
 {
  strcpy(info, "Your icy armour melts away.");
  mpr(info);
/*  player_AC() -= 6; */
  you[0].AC_ch = 1;
  you[0].duration [1] = 0;
 }
}
 else
switch (you[0].inv_type [armour_wear_2])
{
case 13:
case 14:
case 8:
/*you[0].equip [5] = armour_wear_2;
player_shield_class() += 3; // because takes account of plusses above
break;

case 14:you[0].equip [5] = armour_wear_2;
player_shield_class() += 7;
break;

case 8: you[0].equip [5] = armour_wear_2;
player_shield_class() += 5;*/
you[0].equip [5] = armour_wear_2;
/*player_shield_class() = get_shield_class();*/
break;
case 9: you[0].equip [1] = armour_wear_2; break;
case 10: you[0].equip [2] = armour_wear_2; break;
case 11: you[0].equip [3] = armour_wear_2; break;
case 12: you[0].equip [4] = armour_wear_2; break;
//case 10: you[0].equip [5] = armour_wear_2;
}
you[0].AC_ch = 1;
you[0].evasion_ch = 1;
you[0].turnover = 1;
you[0].delay_t = property(you[0].inv_class [armour_wear_2], you[0].inv_type [armour_wear_2], 0);
if (you[0].inv_type [armour_wear_2] == 8) you[0].delay_t = 0; // shield
you[0].delay_doing = 1;

if (you[0].inv_ident [armour_wear_2] < 3) you[0].inv_ident [armour_wear_2] = 3;

                        in_name(armour_wear_2, 3, str_pass);
                        strcat(info, str_pass);
                        //if (you[0].inv_quant [j] != 1) strcat(info, "s");

                        strcat(info, " (being worn)");
                        mpr(info);

switch(you[0].inv_type [armour_wear_2])
{
/* case 18:
 player_res_fire() ++;
 player_res_cold() --;
 break;*/

 case 19:
// you[0].rate_regen += 50;
 you[0].hunger_inc += 1;
 break;
/*
 case 21:
 player_res_fire() --;
 player_res_cold() ++;
 break;*/

                 case 27: // storm dragon
                 you[0].attribute [0] ++;
                 break;

/*                 case 29: // gold dragon
                 player_res_fire() ++;
                 player_res_cold() ++;
                 you[0].res_poison ++;
                 break;
*/

}

if (you[0].inv_dam [armour_wear_2] != 0)
{

//if (you[0].inv_ident [item_wield_2] == 1) you[0].inv_ident [item_wield_2] = 2;

//      Should I make these increase food consumption?

switch(you[0].inv_dam [armour_wear_2] % 30)
{

  case 1:
  if (you[0].species != 13) strcpy(info, "You feel quick on your feet.");
   else strcpy(info, "You feel quick.");
  mpr(info);
/*  if (you[0].fast_run > 0) you[0].fast_run = 0;
  you[0].fast_run ++;*/
  break;

  case 2:
  strcpy(info, "You feel resistant to fire."); // What a boring message
  mpr(info);
/*  player_res_fire() ++;*/
  break;

  case 3:
  strcpy(info, "You feel resistant to cold.");
  mpr(info);
/*  player_res_cold() ++;*/
  break;

  case 4:
  strcpy(info, "You feel healthy.");
  mpr(info);
/*  you[0].res_poison ++;*/
  break;

  case 5:
  strcpy(info, "You feel perceptive.");
  mpr(info);
/*  if (you[0].see_invis > 0) you[0].see_invis = 0;
  you[0].see_invis --;*/
  break;

  case 6:
  if (you[0].invis == 0)
  {
   mpr("You become transparent for a moment.");
  }
  break;

 case 7:
 strcpy(info, "You feel stronger.");
 mpr(info);
 you[0].strength += 3;
 you[0].max_strength += 3;
 you[0].strength_ch = 1;
 break;

 case 8:
 strcpy(info, "You feel dextrous.");
 mpr(info);
 you[0].dex += 3;
 you[0].max_dex += 3;
 you[0].dex_ch = 1;
 break;

 case 9:
 strcpy(info, "You feel intelligent."); // inspired messages, aren't they?
 mpr(info);
 you[0].intel += 3;
 you[0].max_intel += 3;
 you[0].intel_ch = 1;
 break;

 case 10:
 strcpy(info, "You feel rather ponderous.");
 you[0].speed += 2;
/* player_evasion() -= 1; */
 you[0].evasion_ch = 1;
 break;

        case 11: // levitation
        mpr("You feel light on your feet.");
/*      if (you[0].lev > 0) you[0].lev = 0;
           else if (you[0].lev == 0)
           {
                strcpy(info, "You feel very buoyant!");
                mpr(info);
                strcpy(info, "You gently float upwards from the floor.");
                mpr(info);
           }
        you[0].lev --;
        burden_change();*/
        break;

 case 12:
 strcpy(info, "You feel resistant to magic.");
 mpr(info);
/* you[0].res_magic += 40;*/
 break;

 case 13:
 strcpy(info, "You feel protected.");
 mpr(info);
/* player_AC() += 3;*/
 break;

 case 14:
 strcpy(info, "You feel stealthy.");
 mpr(info);
 break;

  case 15:
  strcpy(info, "You feel resistant to extremes of temperature.");
  mpr(info);
/*  player_res_cold() ++;
  player_res_fire() ++;*/
  break;

  case 16:
  strcpy(info, "Your life-force is being protected.");
  mpr(info);
/*  player_prot_life() ++;*/
  break;

  case 17: // archmagi
  if (you[0].skills [25] == 0)
  {
   strcpy(info, "You feel strangely numb.");
  } else
  strcpy(info, "You feel extremely powerful.");
  mpr(info);
/*  you[0].mag_abil += 2;
  you[0].spec_conj ++;
  you[0].spec_ench ++;
  you[0].spec_summ ++;
  you[0].spec_death ++;*/
  break;

}

/* case 2: strcat(glog, " of fire resistance"); break;
 case 3: strcat(glog, " of cold resistance"); break;
 case 4: strcat(glog, " of you[0].poison resistance"); break;
 case 5: strcat(glog, " of see invisible"); break;
*/

}

/*if (delay_t > 0)
{
 more();
 scrloc = 0;
}
*/

} // end of wear_armour()






void takeoff_armour(void)
{

unsigned char nthing;

if (you[0].berserker != 0)
{
 strcpy(info, "You are too berserk!");
 mpr(info);
 return;
}

query : strcpy(info, "Take off which item?");
mpr(info);

// test relay_message();

unsigned char keyin = get_ch();

if (keyin == '?' | keyin == '*')
{
        if (keyin == '?') nthing = get_invent(2);
//invent(you[0].inv_plus2, 2, you[0].inv_quant, you[0].inv_dam, you[0].inv_class, you[0].inv_type, you[0].inv_plus, you[0].inv_ident, you[0].equip [0], you[0].equip [6], you[0].equip [5], you[0].equip [2], you[0].equip [1], you[0].equip [3], you[0].equip [4], you[0].ring);
        if (keyin == '*') nthing = get_invent(-1);
//invent(you[0].inv_plus2, -1, you[0].inv_quant, you[0].inv_dam, you[0].inv_class, you[0].inv_type, you[0].inv_plus, you[0].inv_ident, you[0].equip [0], you[0].equip [6], you[0].equip [5], you[0].equip [2], you[0].equip [1], you[0].equip [3], you[0].equip [4], you[0].ring);
        if ((nthing >= 65 && nthing <= 90) | (nthing >= 97 && nthing <= 122))
        {
                keyin = nthing;
        } else
        {
      mesclr();
    goto query;
        /*              strcpy(info, "Take off which item?"); mpr(info);
                relay_message();
                keyin = get_ch();*/
        }
}

/*if (keyin == '*')
{
        inventory(-1);
        strcpy(info, "Take off which item?"); mpr(info);
        relay_message();
        keyin = get_ch();
}*/


int armour_wear_1 = (int) keyin;

if (armour_wear_1 < 65 | (armour_wear_1 > 90 && armour_wear_1 < 97) | armour_wear_1 > 122)
{
        strcpy(info, "You don't have any such object.");
        mpr(info);
        return;
}



/*
if (you[0].equip [6] == -1) // or helmet etc
{
        strcpy(info, "You aren't wearing any you[0].equip.");
        mpr(info);
        return;
}
*/


int armour_wear_2 = conv_lett(armour_wear_1); //you[0].equip [6]; // may as well keep on using armour_wear_2

if (you[0].inv_class [armour_wear_2] != 2)
{
        strcpy(info, "You aren't wearing that!");
        mpr(info);
        return;
}

if (you[0].inv_plus [armour_wear_2] > 130 && (armour_wear_2 == you[0].equip [6] | armour_wear_2 == you[0].equip [5] | armour_wear_2 == you[0].equip [2] | armour_wear_2 == you[0].equip [3] | armour_wear_2 == you[0].equip [1] | armour_wear_2 == you[0].equip [4]))
{
        strcpy(info, "That piece of armour seems to be stuck to your body!");
        mpr(info);
        return;
}

if (you[0].inv_type [armour_wear_2] < 8 | you[0].inv_type [armour_wear_2] > 14)
{
        if (armour_wear_2 != you[0].equip [6])
        {
                strcpy(info, "You aren't wearing that!");
                mpr(info);
                return;
        }

        if (you[0].equip [1] != -1)
        {
        strcpy(info, "You'll have to take your cloak off first.");
        mpr(info);
        return;
        }

//      itoa(you[0].equip [1], st_prn, 10);
//      strcpy(info, st_prn);
//      mpr(info);
/*        player_evasion() -= ev_mod();*/
        you[0].equip [6] = -1;
/*        player_evasion() += ev_mod();*/
} else
switch(you[0].inv_type [armour_wear_2])
{

case 13:
case 14:
case 8:
if (armour_wear_2 != you[0].equip [5])
{
        strcpy(info, "You aren't wearing that!");
        mpr(info);
        return;
} else you[0].equip [5] = -1; break;


case 9:
if (armour_wear_2 != you[0].equip [1])
{
        strcpy(info, "You aren't wearing that!");
        mpr(info);
        return;
} else you[0].equip [1] = -1; break;


//you[0].equip [1] = armour_wear_2; break;
case 10:
if (armour_wear_2 != you[0].equip [2])
{
        strcpy(info, "You aren't wearing that!");
        mpr(info);
        return;
} else you[0].equip [2] = -1; break;

//you[0].equip [2] = armour_wear_2; break;

case 11:
if (armour_wear_2 != you[0].equip [3])
{
        strcpy(info, "You aren't wearing that!");
        mpr(info);
        return;
} else you[0].equip [3] = -1; break;



//you[0].equip [3] = armour_wear_2; break;
case 12:
if (armour_wear_2 != you[0].equip [4])
{
        strcpy(info, "You aren't wearing that!");
        mpr(info);
        return;
} else you[0].equip [4] = -1; break;


//you[0].equip [4] = armour_wear_2; break;

}



//you[0].equip [6] = -1;

you[0].turnover = 1;
you[0].delay_t = property(you[0].inv_class [armour_wear_2], you[0].inv_type [armour_wear_2], 0);
you[0].delay_doing = 2;

strcpy(info, "You take off ");
//item_name(you[0].inv_class [armour_wear_2], you[0].inv_type [armour_wear_2], you[0].inv_dam [armour_wear_2], you[0].inv_plus [armour_wear_2], you[0].inv_quant [armour_wear_2], you[0].inv_ident [armour_wear_2], 5, str_pass);
in_name(armour_wear_2, 5, str_pass);
strcat(info, str_pass);
strcat(info, ".");
mpr(info);

unwear_armour(armour_wear_2);

/*if (delay_t > 0)
{
 more();
 scrloc = 0;
}
*/

} // end of void takeoff_armour()


void throw_anything(void)
{
unsigned char nthing;
struct bolt beam [1];

if (you[0].berserker != 0)
{
 strcpy(info, "You are too berserk!");
 mpr(info);
 return;
}

if (you[0].inv_no == 0)
        {
        strcpy(info, "You aren't carrying anything.");
        mpr(info);
        return;
        }

query : strcpy(info, "Throw which item?");
mpr(info);

unsigned char keyin = get_ch();

if (keyin == '?' | keyin == '*')
{
        if (keyin == '?')
        {
                nthing = get_invent(1);
        }
        if (keyin == '*') nthing = get_invent(-1);
        if ((nthing >= 65 && nthing <= 90) | (nthing >= 97 && nthing <= 122))
        {
                keyin = nthing;
        } else
        {
    mesclr();
    goto query;
        }
}

int throw_1 = (int) keyin;

if (throw_1 < 65 | (throw_1 > 90 && throw_1 < 97) | throw_1 > 122)
{
        strcpy(info, "You don't have any such object.");
        mpr(info);
        return;
}

int throw_2 = conv_lett(throw_1);

if (throw_2 == you[0].equip [7] | throw_2 == you[0].equip [8] | throw_2 == you[0].equip [9] | throw_2 == you[0].equip [6] | throw_2 == you[0].equip [5] | throw_2 == you[0].equip [2] | throw_2 == you[0].equip [1] | throw_2 == you[0].equip [4] | throw_2 == you[0].equip [3])
{
        strcpy(info, "You are wearing that object!");
        mpr(info);
        return;
}

if (throw_2 == you[0].equip [0] && you[0].inv_plus [you[0].equip [0]] > 80)
{
        strcpy(info, "That thing is stuck to your hand!");
        mpr(info);
        return;
}

if (you[0].inv_quant [throw_2] == 0)
{
        strcpy(info, "You don't have any such object.");
        mpr(info);
        return;
}

throw_it(beam, throw_2);

} /* end of throw_anything */






void shoot_thing(void)
{

struct bolt beam [1];

int i;
int throw_2;

if (you[0].berserker != 0)
{
 strcpy(info, "You are too berserk!");
 mpr(info);
 return;
}

int type_wanted = 3;

if (you[0].equip [0] == -1) goto find;

if (you[0].inv_class [you[0].equip [0]] != 0 | you[0].inv_type [you[0].equip [0]] < 13 | you[0].inv_type [you[0].equip [0]] > 15)
{
        type_wanted = 3;
} else type_wanted = you[0].inv_type [you[0].equip [0]] - 13;

find: for (i = 0; i < 53; i ++)
{
  if (i == 52)
                {
   strcpy(info, "You have nothing suitable.");
                        mpr(info);
                        return;
                }
  if (you[0].inv_quant [i] == 0) continue;
                if (you[0].inv_class [i] == 1 && type_wanted == you[0].inv_type [i])
  {
                                throw_2 = i;
    break;
  }
}

throw_it(beam, throw_2);

} /* end shoot_thing */


void throw_it(struct bolt beam [1], int throw_2)
{
char shoot = 0;
//char nothing;
struct dist thr [1];

strcpy(info, "Which direction? (* to target)");
mpr(info);

//direction(1);
direction(1, thr);

if (thr[0].nothing == -1) return;


if (you[0].conf != 0)
{
 thr[0].move_x = random2(13) - 6;
 thr[0].move_y = random2(13) - 6;
}

beam[0].move_x = thr[0].move_x;
beam[0].move_y = thr[0].move_y;
beam[0].target_x = thr[0].target_x;
beam[0].target_y = thr[0].target_y;

beam[0].range = 2; /* provisional until mass variable used */

beam[0].flavour = 0;

switch (you[0].inv_class [throw_2])
{
        case 0: beam[0].type = 41;
 beam[0].range = 6;
 break;
        case 1: beam[0].type = 41;
 beam[0].range = 7;
 break;
        case 2: beam[0].type = 91; break;
        case 3: beam[0].type = 47; break;
        case 4: beam[0].type = 37; break;
        case 5: beam[0].type = 43; break;
        case 6: beam[0].type = 63; break;
        case 7: beam[0].type = 61; break;
        case 8: beam[0].type = 33; break;
        case 9: beam[0].type = 42; break;
        case 10: beam[0].type = 40; break;
        case 11: beam[0].type = 37; break;
}

beam[0].source_x = you[0].x_pos;
beam[0].source_y = you[0].y_pos;
beam[0].colour = you[0].inv_col [throw_2];
item_name(you[0].inv_plus2 [throw_2],you[0].inv_class [throw_2], you[0].inv_type [throw_2], you[0].inv_dam [throw_2], you[0].inv_plus [throw_2], 1, you[0].inv_ident [throw_2], 6, str_pass);
/*in_name(throw_2, 6);    <-- Don't use this: "the 10 +0 darts hit the x" */
strcpy(beam[0].beam_name, str_pass);
beam[0].thing_thrown = 1;


beam[0].hit = (random2(you[0].dex)) / 2;

beam[0].hit += random2(you[0].skills [12] + 1);
beam[0].damage += random2(you[0].skills [12] + 1);

if (you[0].inv_class [throw_2] == 1 && you[0].inv_type [throw_2] == 3) // darts
{
 if (you[0].equip [0] == -1 | you[0].inv_class [you[0].equip [0]] != 0 | you[0].inv_type [you[0].equip [0]] != 16)
 {
   exercise(11 , 1 + random2(2) + random2(2) + random2(2));
   beam[0].hit += 2;
   beam[0].hit += random2(you[0].skills [11] + 1) + random2(you[0].skills [11] + 1);
 }
}

if (you[0].inv_class [throw_2] == 0 | you[0].inv_class [throw_2] == 1)
{
        beam[0].damage = property(you[0].inv_class [throw_2], you[0].inv_type [throw_2], 0);

/*      Missile devices eg bows */
        if (you[0].inv_class [you[0].equip [0]] == 0 && you[0].inv_type [you[0].equip [0]] == you[0].inv_type [throw_2] + 13 && you[0].inv_type [you[0].equip [0]] <= 16)
        {
                beam[0].damage = property(you[0].inv_class [throw_2], you[0].inv_type [throw_2], 1);

  if (you[0].inv_type [you[0].equip [0]] == 15)
  {
   you[0].time_taken *= 15;
   you[0].time_taken /= 10;
  }

                int hoggl = you[0].inv_plus2 [you[0].equip [0]] - 50;

                if (you[0].inv_plus2 [you[0].equip [0]] > 80) hoggl -= 100;

                beam[0].damage += hoggl;
  beam[0].range += 5;

                switch(you[0].inv_type [you[0].equip [0]])
                {
                 case 13: /* sling */
                 exercise(8, 1 + random2(2) + random2(2) + random2(2));
                 beam[0].hit += random2(you[0].skills [8] + 1) + random2(you[0].skills [8] + 1);
                 beam[0].damage += random2(you[0].skills [8] + 1);
                 break;
                 case 14: /* bow */
                 exercise(9, 1 + random2(2) + random2(2) + random2(2));
                 beam[0].hit += random2(you[0].skills [9] + 1) + random2(you[0].skills [9] + 1);
                 beam[0].damage += random2(you[0].skills [9] + 1) + random2(you[0].skills [9] + 1);
                 break;
                 case 15: /* crossbow */
                 case 16: /* hand crossbow */
                 exercise(12, 1 + random2(2) + random2(2) + random2(2));
                 beam[0].hit += random2(you[0].skills [10] + 1);
                 beam[0].damage += random2(you[0].skills [10] + 1);
                 break;
                }

                int ghoggl = you[0].inv_plus [you[0].equip [0]] - 50;

                if (you[0].inv_plus [you[0].equip [0]] > 80) ghoggl -= 100;

                beam[0].hit += ghoggl;

                beam[0].range += 3;

                if (you[0].inv_dam [you[0].equip [0]] / 30 == you[0].inv_dam [throw_2] / 30)
                {
                 beam[0].hit ++;
                 beam[0].damage ++;
                 if (you[0].inv_dam [you[0].equip [0]] / 30 == 4 && you[0].species >= 2 && you[0].species <= 6) beam[0].hit ++;
                }


                if ((you[0].inv_dam [you[0].equip [0]] % 30 == 11 | you[0].inv_dam [throw_2] % 30 == 1) && you[0].inv_dam [throw_2] % 30 != 2 && you[0].inv_dam [you[0].equip [0]] % 30 != 12)
                {
                 beam[0].damage += 1 + random2(5);
                 beam[0].flavour = 2;
                 strcpy(beam[0].beam_name, "bolt of ");
                 if (you[0].inv_dam [throw_2] % 30 == 3 | you[0].inv_dam [throw_2] % 30 == 4)
                        strcat(beam[0].beam_name, "poison ");
                 strcat(beam[0].beam_name, "flame");
                 beam[0].colour = RED;
                 beam[0].type = 35;
                 beam[0].thing_thrown = 3;
                }
//                if (you[0].inv_dam [you[0].equip [0]] % 30 == 12)
                if ((you[0].inv_dam [you[0].equip [0]] % 30 == 12 | you[0].inv_dam [throw_2] % 30 == 2) && you[0].inv_dam [throw_2] % 30 != 1 && you[0].inv_dam [you[0].equip [0]] % 30 != 11)
                {
                 beam[0].damage += 1 + random2(5);
                 beam[0].flavour = 3;
                 strcpy(beam[0].beam_name, "bolt of ");
                 if (you[0].inv_dam [throw_2] % 30 == 3 | you[0].inv_dam [throw_2] % 30 == 4)
                        strcat(beam[0].beam_name, "poison ");
                 strcat(beam[0].beam_name, "frost");
                 beam[0].colour = WHITE;
                 beam[0].type = 35;
                 beam[0].thing_thrown = 3;
                } /* the chief advantage here is the extra damage this does against susceptible creatures */

/* Note: weapons & ammo of eg fire are not cumulative
ammo of fire and weapons of frost don't work together, and vice versa */

                if (you[0].inv_plus [you[0].equip [0]] > 80)
                {
                        beam[0].damage -= 100;
                        beam[0].hit -= 100;
                }
                shoot = 1;
                strcpy(info, "You shoot ");
        }
/*
        13 = sling
        14 = bow
        15 = crossbow - gotta put loading in
        16 = hand crossbow
*/


        int hoggl = you[0].inv_plus [throw_2] - 50;
        if (you[0].inv_plus [throw_2] > 80) hoggl -= 100;
        //damage_done += you[0].inv_plus [throw_2];
        hoggl = (hoggl % 10);
        beam[0].damage += hoggl;


        int ghoggl = you[0].inv_plus [throw_2] - 50;
        if (you[0].inv_plus [throw_2] > 80) ghoggl -= 100;
        //damage_done += you[0].inv_plus [throw_2];
        ghoggl = (ghoggl / 10);
        beam[0].hit += ghoggl;



        if (you[0].inv_class [throw_2] == 0)
        {
                if (you[0].inv_type [throw_2] == 3 | you[0].inv_type [throw_2] == 9 | you[0].inv_type [throw_2] == 11)
                {
                        beam[0].damage += 1;
                } else
                {
                        beam[0].damage -= 1;
                        beam[0].hit -= 3;
                }
        }

}
 else beam[0].damage = 2; /* later put mass thing here. */


if (shoot == 0) strcpy(info, "You throw ");
/* if shoot == 1, it has already strcpy'd "You shoot " */
        item_name(you[0].inv_plus2 [throw_2], you[0].inv_class [throw_2], you[0].inv_type [throw_2], you[0].inv_dam [throw_2], you[0].inv_plus [throw_2], 1, you[0].inv_ident [throw_2], 3, str_pass);
        strcat(info, str_pass);
        strcat(info, ".");
        mpr(info);

 if (you[0].equip [0] == throw_2) unwield_item(throw_2);

missile(beam, throw_2);
you[0].inv_quant [throw_2] --;
if (you[0].inv_quant [throw_2] == 0)
{
        you[0].inv_no --;
        if (you[0].equip [0] == throw_2)
        {
                you[0].equip [0] = -1;
                strcpy(info, "You are now empty handed.");
                mpr(info);
        }
}

burden_change();

/*move_x = 0; move_y = 0;*/

alert();

you[0].turnover = 1;

} // end of void throw_it()



void puton_ring(void)
{

unsigned char nthing;

if (you[0].inv_no == 0)
        {
        strcpy(info, "You aren't carrying anything.");
        mpr(info);
        return;
        }

if (you[0].berserker != 0)
{
 strcpy(info, "You are too berserk!");
 mpr(info);
 return;
}

query : strcpy(info, "Put on which item?");
mpr(info);

unsigned char keyin = get_ch();

if (keyin == '?' | keyin == '*')
{
        if (keyin == '?') nthing = get_invent(7);
        if (keyin == '*') nthing = get_invent(-1);
        if ((nthing >= 65 && nthing <= 90) | (nthing >= 97 && nthing <= 122))
        {
                keyin = nthing;
        } else
        {
      mesclr();
       goto query;

        }
}
int ring_wear_1 = (int) keyin;

if (ring_wear_1 < 65 | (ring_wear_1 > 90 && ring_wear_1 < 97) | ring_wear_1 > 122)
{
        strcpy(info, "You don't have any such object.");
        mpr(info);
        return;
}

int ring_wear_2 = conv_lett(ring_wear_1);

if (ring_wear_2 == you[0].equip [7] | ring_wear_2 == you[0].equip [8] | ring_wear_2 == you[0].equip [9])
{
        strcpy(info, "You are already wearing that!");
        mpr(info);
        return;
}

if (you[0].inv_quant [ring_wear_2] == 0)
{
        strcpy(info, "You don't have any such object.");
        mpr(info);
        return;
}

if (ring_wear_2 == you[0].equip [0])
{
        strcpy(info, "You are wielding that object.");
        mpr(info);
        return;
}

if (you[0].inv_class [ring_wear_2] != 7)
{
        strcpy(info, "I think there's some kind of mistake here.");
        mpr(info);
        return;
}


if (you[0].inv_type [ring_wear_2] < 35)
{
 if (you[0].equip [3] != -1 && you[0].inv_plus [you[0].equip [3]] > 80)
 {
        strcpy(info, "You can't take your gloves off to wear a ring!");
        mpr(info);
        return;
 }

 if (you[0].inv_class [ring_wear_2] == 7 && you[0].equip [7] != -1 && you[0].equip [8] != -1) // and you are trying to wear body you[0].equip.
 {
        strcpy(info, "You are already wearing a ring on each hand.");
        mpr(info);
        return;
 }
} else
if (you[0].equip [9] != -1)
{
        strcpy(info, "You are already wearing an amulet.");
        if (random2(20))
        {
         strcat(info, " And I must say it looks quite fetching.");
        }
        mpr(info);
        return;
}

int hand_used;

if (you[0].equip [7] != -1) hand_used = 1;
if (you[0].equip [8] != -1) hand_used = 0;

if (you[0].inv_type [ring_wear_2] >= 35)
 hand_used = 2;
  else
if (you[0].equip [7] == -1 && you[0].equip [8] == -1)
{
        strcpy(info, "Put on which hand (l or r)?");
        mpr(info);

        keyin = get_ch();

        if (keyin != 'r' && keyin != 'l')
        {
                strcpy(info, "You don't have such a hand!");
                mpr(info);
                return;
        }

        if (keyin == 'l') hand_used = 0;
        if (keyin == 'r') hand_used = 1;
}

you[0].equip [hand_used + 7] = ring_wear_2;



strcpy(info, " ");
/*                      strncpy(info, letters [ring_wear_2], 1); */
                        if (ring_wear_2 <= 25) info [0] = ring_wear_2 + 97;
                                else info [0] = ring_wear_2 + 39;

                        info [1] = 0;

                        strcat(info, " - ");
                        item_name(you[0].inv_plus2 [ring_wear_2], you[0].inv_class [ring_wear_2], you[0].inv_type [ring_wear_2], you[0].inv_dam [ring_wear_2], you[0].inv_plus [ring_wear_2], you[0].inv_quant [ring_wear_2], you[0].inv_ident [ring_wear_2], 3, str_pass);
                        strcat(info, str_pass);

                        if (hand_used == 0) strcat(info, " (on left hand)");
                        if (hand_used == 1) strcat(info, " (on right hand)");
                        if (hand_used == 2) strcat(info, " (around neck)");
                        mpr(info);


switch(you[0].inv_type [ring_wear_2])
{

        case 0: /* you[0].ring of regeneration */
/*      you[0].rate_regen += 100; */
        you[0].hunger_inc += 2;
        if (you[0].hp < you[0].hp_max) set_id(you[0].inv_class [ring_wear_2], you[0].inv_type [ring_wear_2], 1); // this is a bit dodgy
        break;

        case 1: /* protection */
/*      if (you[0].inv_plus [ring_wear_2] > 130) player_AC() -= 100;
        player_AC() += you[0].inv_plus [ring_wear_2];
        player_AC() -= 50;*/
        you[0].AC_ch = 1;
//      you[0].hunger_inc += 1;
        if (you[0].inv_plus [ring_wear_2] != 0 && you[0].inv_plus [ring_wear_2] != 100)
        {
           set_id(you[0].inv_class [ring_wear_2], you[0].inv_type [ring_wear_2], 1);
           you[0].inv_ident [ring_wear_2] = 3;
        }
        break;

        case 2: /* fire resistance */
/*      player_res_fire() += 1;*/
//      you[0].hunger_inc += 1;
        break;

        case 3: // you[0].poison resistance
/*      you[0].res_poison += 1; */
///     you[0].hunger_inc += 1;
        break;

        case 4: // cold resistance
//      you[0].hunger_inc += 1;
        break;

        case 5: // increase you[0].strength
        you[0].strength += (you[0].inv_plus [ring_wear_2] % 100) - 50;
        you[0].max_strength += (you[0].inv_plus [ring_wear_2] % 100) - 50;
        you[0].strength_ch = 1;
//      you[0].hunger_inc += 1;
        burden_change();
        if (you[0].inv_plus [ring_wear_2] != 0 && you[0].inv_plus [ring_wear_2] != 100)
        {
          set_id(you[0].inv_class [ring_wear_2], you[0].inv_type [ring_wear_2], 1);
          you[0].inv_ident [ring_wear_2] = 3;
        }
        break;

        case 6: // slaying
/*      you[0].f_abil += 30; */
//      you[0].hunger_inc += 1;
        break;

        case 7: // see invisible
/*      you[0].see_invis --; */
//      you[0].hunger_inc += 1;
        break;

        case 8: // invisibility
//      if (you[0].invis > 0) you[0].invis = 0;
           if (you[0].invis == 0)
           {
                mpr("You become transparent for a moment.");
           }
                set_id(you[0].inv_class [ring_wear_2], you[0].inv_type [ring_wear_2], 1);
                you[0].inv_ident [ring_wear_2] = 3;
//      you[0].invis --;
//      you[0].hunger_inc += 4;
        break;

        case 9: // you[0].hunger
        you[0].hunger_inc += 4;
        break;

        case 10: // teleportation
//      you[0].hunger_inc += 1;
/*      player_teleport() += 1;*/
        break;

        case 11: // player_evasion()
//      you[0].hunger_inc += 1;
/*      if (you[0].inv_plus [ring_wear_2] > 130) player_evasion() -= 100;
        player_evasion() += you[0].inv_plus [ring_wear_2];
        player_evasion() -= 50;*/
        you[0].evasion_ch = 1;
        if (you[0].inv_plus [ring_wear_2] != 0 && you[0].inv_plus [ring_wear_2] != 100)
        {
           you[0].inv_ident [ring_wear_2] = 3;
           set_id(you[0].inv_class [ring_wear_2], you[0].inv_type [ring_wear_2], 1);
        }
        break;

        case 12:
// you[0].hunger_inc += 1;
/* you[0].sust_abil++;*/
 break;

 case 13:
 you[0].hunger_inc -= 2;
 break;

 case 14: // increase you[0].dex
        you[0].dex += (you[0].inv_plus [ring_wear_2] % 100) - 50;
        you[0].max_dex += (you[0].inv_plus [ring_wear_2] % 100) - 50;
        you[0].dex_ch = 1;
//      you[0].hunger_inc += 1;
        if (you[0].inv_plus [ring_wear_2] != 0 && you[0].inv_plus [ring_wear_2] != 100)
        {
           you[0].inv_ident [ring_wear_2] = 3;
           set_id(you[0].inv_class [ring_wear_2], you[0].inv_type [ring_wear_2], 1);
        }
        break;

 case 15: // increase you[0].intel
        you[0].intel += (you[0].inv_plus [ring_wear_2] % 100) - 50;
        you[0].max_intel += (you[0].inv_plus [ring_wear_2] % 100) - 50;
        you[0].intel_ch = 1;
//      you[0].hunger_inc += 1;
        if (you[0].inv_plus [ring_wear_2] != 0 && you[0].inv_plus [ring_wear_2] != 100)
        {
           you[0].inv_ident [ring_wear_2] = 3;
           set_id(you[0].inv_class [ring_wear_2], you[0].inv_type [ring_wear_2], 1);
        }
        break;

 case 16: // wizardry
/* you[0].mag_abil += 3;*/
// you[0].hunger_inc += 1;
 break;

 case 17: // magic
 you[0].ep_max += 9;
 you[0].base_ep2 += 9;
// you[0].hunger_inc ++;
 you[0].ep_ch = 1;
/* you[0].magic_battery ++; */
 calc_ep();
 set_id(you[0].inv_class [ring_wear_2], you[0].inv_type [ring_wear_2], 1);
 break;

        case 18: // levitation
/*      if (you[0].lev > 0) you[0].lev = 0;
           else if (you[0].lev == 0)
           {
                strcpy(info, "You feel very buoyant!");
                mpr(info);
                strcpy(info, "You gently float upwards from the floor.");
                mpr(info);
       set_id(you[0].inv_class [ring_wear_2], you[0].inv_type [ring_wear_2], 1);
       you[0].inv_ident [ring_wear_2] = 3;
           }
        you[0].lev --;
        you[0].hunger_inc += 1;
        burden_change();*/
        mpr("You feel buoyant.");
        break;

 case 19: // player_prot_life()
//      you[0].hunger_inc ++;
        break;

 case 20: // magic resistance
// you[0].hunger_inc += 1;
 break;

 case 21: // fire
/* player_res_fire() += 1;
 you[0].spec_fire += 1;
 player_res_cold() -= 1;*/
// you[0].hunger_inc += 1;
 break;

 case 22: // ice
/* player_res_fire() -= 1;
 you[0].spec_cold += 1;
 player_res_cold() += 1;*/
// you[0].hunger_inc += 1;
 break;

 case 23: // teleport control
 you[0].attribute [3] ++;
// you[0].hunger_inc += 1;
 break;


/* all of the amulets so far existing are not needed here, but have their
   presence queried whenever relevant through wearing_amulet in fn2.cc */
}
you[0].turnover = 1;

} // end of puton_ring()





void remove_ring(void)
{

unsigned char nthing;
int hand_used = 10;
unsigned char keyin;
int ring_wear_2;

if (you[0].equip [7] == -1 && you[0].equip [8] == -1 && you[0].equip [9] == -1)
{
        strcpy(info, "You aren't wearing any rings or amulets.");
        mpr(info);
        return;
}

if (you[0].berserker != 0)
{
 strcpy(info, "You are too berserk!");
 mpr(info);
 return;
}

if (you[0].equip [3] != -1 && you[0].inv_plus [you[0].equip [3]] > 80 && you[0].equip [9] == -1)
{
        strcpy(info, "You can't take your gloves off to remove any rings!");
        mpr(info);
        return;
}


if (you[0].equip [7] != -1 && you[0].equip [8] == -1 && you[0].equip [9] == -1) hand_used = 0;
if (you[0].equip [7] == -1 && you[0].equip [8] != -1 && you[0].equip [9] == -1) hand_used = 1;
if (you[0].equip [7] == -1 && you[0].equip [8] == -1 && you[0].equip [9] != -1) hand_used = 2;

if (hand_used == 10)
{
        query : strcpy(info, "Remove from where (l or r hand, or n for neck)?");
        mpr(info);

        keyin = get_ch();

       if (keyin == '?' | keyin == '*')
       {
        if (keyin == '?') nthing = get_invent(7);
        if (keyin == '*') nthing = get_invent(-1);
        if ((nthing >= 65 && nthing <= 90) | (nthing >= 97 && nthing <= 122))
        {
         keyin = nthing;
        } else
        {
         mesclr();
         goto query;
        }
       }

        if (keyin != 'r' && keyin != 'l' && keyin != 'n')
        {
                strcpy(info, "You don't have such a hand or neck.");
                mpr(info);
                return;
        }

        if (keyin == 'l') hand_used = 0;
        if (keyin == 'r') hand_used = 1;
        if (keyin == 'n') hand_used = 2;

}

if (you[0].equip [3] != -1 && you[0].inv_plus [you[0].equip [3]] > 80 && hand_used < 2)
{
        strcpy(info, "You can't take your gloves off to remove any rings!");
        mpr(info);
        return;
}

if (you[0].equip [hand_used + 7] == -1)
{
        strcpy(info, "I don't think you really meant that.");
        mpr(info);
        return;
}

if (you[0].inv_plus [you[0].equip [hand_used + 7]] > 80) // cursed
{
        strcpy(info, "It's stuck to you!");
        mpr(info);
  if (you[0].inv_ident [you[0].equip [hand_used + 7]] == 0) you[0].inv_ident [you[0].equip [hand_used + 7]] = 1;
  return;
}

strcpy(info, "You remove your ");
item_name(you[0].inv_plus2 [you[0].equip [hand_used + 7]], you[0].inv_class [you[0].equip [hand_used + 7]], you[0].inv_type [you[0].equip [hand_used + 7]], you[0].inv_dam [you[0].equip [hand_used + 7]], you[0].inv_plus [you[0].equip [hand_used + 7]], you[0].inv_quant [you[0].equip [hand_used + 7]], you[0].inv_ident [you[0].equip [hand_used + 7]], 8, str_pass);
strcat(info, str_pass);
strcat(info, ".");
mpr(info);

ring_wear_2 = you[0].equip [hand_used + 7]; // I'll still use ring_wear_2 here.


switch(you[0].inv_type [ring_wear_2])
{

        case 0: // regen
        you[0].hunger_inc -= 2;
        break;

        case 1: // prot
        you[0].AC_ch = 1;
//      you[0].hunger_inc -= 1;
        break;

        case 2: // fire resistance
//      you[0].hunger_inc -= 1;
        break;

        case 3: // you[0].poison resistance
//      you[0].hunger_inc -= 1;
        break;

        case 4: // cold resistance
//      you[0].hunger_inc -= 1;
        break;

        case 5: // increase you[0].strength
        you[0].strength -= (you[0].inv_plus [ring_wear_2] % 100);
        you[0].max_strength -= (you[0].inv_plus [ring_wear_2] % 100);
        you[0].strength += 50;
        you[0].max_strength += 50;
        you[0].strength_ch = 1;
//      you[0].hunger_inc -= 1;
        burden_change();
        break;

        case 6: // slaying
//      you[0].hunger_inc -= 1;
        break;

        case 7: // see invisible
//      you[0].hunger_inc -= 1;
        break;

        case 8: // invisibility
//      you[0].invis ++;
//      if (you[0].invis == 0) you[0].invis = 1;
//      you[0].hunger_inc -= 4;
        if (you[0].invis != 0) you[0].invis = 1;
        break;

        case 9: // you[0].hunger
        you[0].hunger_inc -= 4;
        break;

        case 10: // teleportation
//      you[0].hunger_inc -= 1;
        break;

        case 11: // player_evasion()
//      you[0].hunger_inc -= 1;
        you[0].evasion_ch = 1;
        break;

 case 12: // you[0].sust_abil
// you[0].hunger_inc -= 1;
 break;

 case 13: // sustenance
 you[0].hunger_inc += 2;
 break;

 case 14: // increase you[0].dex
        you[0].dex -= (you[0].inv_plus [ring_wear_2] % 100) - 50;
        you[0].max_dex -= (you[0].inv_plus [ring_wear_2] % 100) - 50;
        you[0].dex_ch = 1;
//      you[0].hunger_inc -= 1;
        break;

 case 15: // increase you[0].intel
        you[0].intel -= (you[0].inv_plus [ring_wear_2] % 100) - 50;
        you[0].max_intel -= (you[0].inv_plus [ring_wear_2] % 100) - 50;
        you[0].intel_ch = 1;
//      you[0].hunger_inc -= 1;
        break;

 case 16: // wizardry
// you[0].hunger_inc -= 1;
 break;

 case 17: // magic
 you[0].ep_max -= 9;
 you[0].base_ep2 -= 9;
 if (you[0].ep > you[0].ep_max) you[0].ep = you[0].ep_max;
// you[0].hunger_inc --;
 you[0].ep_ch = 1;
/* you[0].magic_battery --; */
 calc_ep();
 break;

        case 18: // levitation
/*      you[0].lev ++;
        if (you[0].lev == 0) you[0].lev = 1;
        you[0].hunger_inc -= 1;*/
        if (you[0].lev != 0) you[0].lev = 1;
        break;

 case 19: // player_prot_life()
//      you[0].hunger_inc --;
        break;

 case 20: // magic resistance
// you[0].hunger_inc -= 1;
 break;

 case 21: // fire
/* player_res_fire() -= 1;
 you[0].spec_fire -= 1;
 player_res_cold() += 1;*/
// you[0].hunger_inc -= 1;
 break;

 case 22: // ice
/* player_res_fire() += 1;
 you[0].spec_cold -= 1;
 player_res_cold() -= 1;*/
// you[0].hunger_inc -= 1;
 break;

 case 23: // teleport control
 you[0].attribute [3] --;
// you[0].hunger_inc -= 1;
 break;


} // end of switch




you[0].equip [hand_used + 7] = -1;


you[0].turnover = 1;


} // end of remove_ring






void zap_wand(void)
{
int zap_device_1;
int zap_device_2;
unsigned char nthing;

struct bolt beam [1];
struct dist zap_wand [1];

beam[0].wand_id = 0;

if (you[0].inv_no == 0)
        {
        strcpy(info, "You aren't carrying anything.");
        mpr(info);
        return;
        }

if (you[0].berserker != 0)
{
 strcpy(info, "You are too berserk!");
 mpr(info);
 return;
}


query : strcpy(info, "Zap which item?");
mpr(info);

unsigned char keyin = get_ch();

if (keyin == '?' | keyin == '*')
{
        if (keyin == '?') nthing = get_invent(3);
        if (keyin == '*') nthing = get_invent(-1);
        if ((nthing >= 65 && nthing <= 90) | (nthing >= 97 && nthing <= 122))
        {
                keyin = nthing;
        } else
        {
/*   mesclr(); */
   goto query;

        }
}

zap_device_1 = (int) keyin;

if (zap_device_1 < 65 | (zap_device_1 > 90 && zap_device_1 < 97) | zap_device_1 > 122)
{
        Dont_have : strcpy(info, "You don't have any such object.");
        mpr(info);
        return;
}

zap_device_2 = conv_lett(zap_device_1);

if (you[0].inv_quant [zap_device_2] == 0)
{
        goto Dont_have;
}

if (you[0].inv_class [zap_device_2] != 3 | you[0].inv_plus [zap_device_2] <= 0)
{
        strcpy(info, "Nothing appears to happen.");
        mpr(info);
        you[0].turnover = 1;
        return;
}

strcpy(info, "Which direction? (* to target)");
mpr(info);

direction(1, zap_wand);

if (zap_wand[0].nothing == -1) return;

if (you[0].conf != 0)
{
 zap_wand[0].move_x = random2(13) - 6;
 zap_wand[0].move_y = random2(13) - 6;
}

beam[0].source_x = you[0].x_pos; beam[0].source_y = you[0].y_pos;

char type_zapped = you[0].inv_type [zap_device_2];
if (type_zapped == 16) type_zapped = 21; /* enslavement */
if (type_zapped == 17) type_zapped = 17; /* draining */
if (type_zapped == 18) /* random effects */
{
 type_zapped = random2(16);
 if (random2(20) == 0) type_zapped = 17;
 if (random2(17) == 0) type_zapped = 21;
}

beam[0].move_x = zap_wand[0].move_x;
beam[0].move_y = zap_wand[0].move_y;
beam[0].target_x = zap_wand[0].target_x;
beam[0].target_y = zap_wand[0].target_y;

zapping(type_zapped, 40, beam);

if (beam[0].wand_id == 1 | you[0].inv_type [zap_device_2] == 12)
{
        set_id(you[0].inv_class [zap_device_2], you[0].inv_type [zap_device_2], 1);
}

you[0].inv_plus [zap_device_2] --;

you[0].turnover = 1;

alert();

} /* end of void zap_wand(); */



void eat_food(void)
{
int food_eat_1;
int food_eat_2;

unsigned char nthing;

if (you[0].is_undead == 2)
{
 strcpy(info, "You can't eat.");
 mpr(info);
 return;
}



if (igrd [you[0].x_pos] [you[0].y_pos] != 501)
{
 if (eat_from_floor() == 1) return;
}

if (you[0].inv_no == 0)
        {
        strcpy(info, "You aren't carrying anything.");
        mpr(info);
        return;
        }

query : strcpy(info, "Eat which item?");
mpr(info);

unsigned char keyin = get_ch();

if (keyin == '?' | keyin == '*')
{
        if (keyin == '?') nthing = get_invent(4);
        if (keyin == '*') nthing = get_invent(-1);
        if ((nthing >= 65 && nthing <= 90) | (nthing >= 97 && nthing <= 122))
        {
                keyin = nthing;
        } else
        {
/*       mesclr(); */
       goto query;

        }

}

food_eat_1 = (int) keyin;

if (food_eat_1 < 65 | (food_eat_1 > 90 && food_eat_1 < 97) | food_eat_1 > 122)
{
        strcpy(info, "You don't have any such object.");
        mpr(info);
        return;
}

food_eat_2 = conv_lett(food_eat_1);

if (you[0].inv_quant [food_eat_2] == 0)
{
        strcpy(info, "You don't have any such object.");
        mpr(info);
        return;
}


if (you[0].inv_class [food_eat_2] != 4)
{
        strcpy(info, "You can't eat that!");
        mpr(info);
        return;
}

if (you[0].species == 11 | you[0].mutation [10] == 3) // kobold
{
 if ((you[0].inv_type [food_eat_2] >= 1 && you[0].inv_type [food_eat_2] <= 4) | (you[0].inv_type [food_eat_2] >= 7 && you[0].inv_type [food_eat_2] <= 17))
 {
  strcpy(info, "Sorry, you're a carnivore.");
  mpr(info);
  return;
 }
}

    if (you[0].mutation [11] > 1 && you[0].inv_type [food_eat_2] == 21)
    {
     mpr("You can't eat raw meat!");
     return;
    }


if (you[0].inv_type [food_eat_2] == 21 && you[0].hung_state > 2 && wearing_amulet(40) == 0 && you[0].species != 11 && you[0].species != 15 && you[0].species != 16 && you[0].mutation [10] == 0)
{
 strcpy(info, "You aren't quite hungry enough to eat that!");
 mpr(info);
 return;
}

you[0].inv_quant [food_eat_2] --;

if (you[0].inv_quant [food_eat_2] == 0)
{
        you[0].inv_no --;
        if (you[0].equip [0] == food_eat_2)
        {
                you[0].equip [0] = -1;
                strcpy(info, "You are now empty handed.");
                mpr(info);
        }
}


eaten(food_eat_2, you[0].inv_type [food_eat_2]); /* put different values (ie above 30) for eating things not in class 4, eg corpses. */

if (you[0].hunger > 12000) you[0].hunger = 12000;

food_change();

you[0].turnover = 1;

burden_change();

} /* end of void eat_food() */


void drink(void)
{
int drink_1;
int drink_2;
unsigned char nthing;
unsigned char keyin;

if (you[0].is_undead == 2)
{
 strcpy(info, "You can't drink.");
 mpr(info);
 return;
}

if (you[0].inv_no == 0)
        {
         strcpy(info, "You aren't carrying anything.");
         mpr(info);
         return;
        }

if (you[0].berserker != 0)
{
 strcpy(info, "You are too berserk!");
 mpr(info);
 return;
}

query : strcpy(info, "Drink which item?");
mpr(info);

keyin = get_ch();

if (keyin == '?' | keyin == '*')
{
        if (keyin == '?') nthing = get_invent(8);
        if (keyin == '*') nthing = get_invent(-1);
        if ((nthing >= 65 && nthing <= 90) | (nthing >= 97 && nthing <= 122))
        {
                keyin = nthing;
        } else
        {
       mesclr();
    goto query;
        }
}

drink_1 = (int) keyin;

if (drink_1 < 65 | (drink_1 > 90 && drink_1 < 97) | drink_1 > 122)
{
        strcpy(info, "You don't have any such object.");
        mpr(info);
        return;
}

drink_2 = conv_lett(drink_1);

if (you[0].inv_quant [drink_2] == 0)
{
        strcpy(info, "You don't have any such object.");
        mpr(info);
        return;
}


if (you[0].inv_class [drink_2] != 8)
{
        strcpy(info, "You can't drink that!");
        mpr(info);
        return;
}

you[0].inv_quant [drink_2] --;

if (you[0].inv_quant [drink_2] == 0)
{
        you[0].inv_no --;
        if (you[0].equip [0] == drink_2)
        {
                you[0].equip [0] = -1;
                strcpy(info, "You are now empty handed.");
                mpr(info);
        }
}

burden_change();

you[0].turnover = 1;

potion_effect(you[0].inv_type [drink_2], 40);

you[0].hunger += 40;

you[0].inv_ident [drink_2] = 1;

set_id(you[0].inv_class [drink_2], you[0].inv_type [drink_2], 1);

if (you[0].hunger > 12000) you[0].hunger = 12000;

food_change();

} /* end of void drink() */






void read_scroll(void)
{

int sc_read_1;
int sc_read_2;

int id_used = 0;
unsigned char nthing;

char affected = 0;

int i;
struct bolt beam [1];

if (you[0].inv_no == 0)
        {
         strcpy(info, "You aren't carrying anything.");
         mpr(info);
         return;
        }

if (you[0].berserker != 0)
{
 strcpy(info, "You are too berserk!");
 mpr(info);
 return;
}


query : strcpy(info, "Read which item?");
mpr(info);

unsigned char keyin = get_ch();

if (keyin == '?' | keyin == '*')
{
        if (keyin == '?') nthing = get_invent(6);
        if (keyin == '*') nthing = get_invent(-1);
        if ((nthing >= 65 && nthing <= 90) | (nthing >= 97 && nthing <= 122))
        {
                keyin = nthing;
        } else
        {
       mesclr();
      goto query;

        }
}

sc_read_1 = (int) keyin;

if (sc_read_1 < 65 | (sc_read_1 > 90 && sc_read_1 < 97) | sc_read_1 > 122)
{
        strcpy(info, "You don't have any such object.");
        mpr(info);
        return;
}

sc_read_2 = conv_lett(sc_read_1);

if (you[0].inv_quant [sc_read_2] == 0)
{
        strcpy(info, "You don't have any such object.");
        mpr(info);
        return;
}

if (you[0].inv_class [sc_read_2] == 10)
{
if (you[0].inv_type [sc_read_2] == 35)
{
 tome_of_power(sc_read_2);
 return;
}
if (you[0].inv_type [sc_read_2] == 41)
{
 skill_manual(sc_read_2);
 return;
}
 read_book(sc_read_2);
 if (book_thing < 'a' | book_thing > 'f')
 {
  mesclr();
  return;
 }
 if (learn_a_spell(sc_read_2, conv_lett(book_thing)) != 1)
 {
  mesclr();
  return;
 }
 nthing = which_spell_in_book(you[0].inv_type [sc_read_2], conv_lett(book_thing) + 1);
 if (nthing == 210)
 {
  mesclr();
  return;
 }
 describe_spell(which_spell_in_book(you[0].inv_type [sc_read_2], conv_lett(book_thing) + 1));
#ifdef LINUX
redraw_screen();
#endif
    mesclr();
    return;
}

if (you[0].inv_class [sc_read_2] != 6)
{
        strcpy(info, "You can't read that!");
        mpr(info);
        return;
}

if (you[0].inv_type [sc_read_2] == 15)
{
  strcpy(info, "This scroll appears to be blank.");
  mpr(info);
  you[0].turnover = 1;
  set_id(you[0].inv_class [sc_read_2], you[0].inv_type [sc_read_2], 1);
  return;
}




you[0].inv_quant [sc_read_2] --;

if (you[0].inv_quant [sc_read_2] == 0)
{
        you[0].inv_no --;
        if (you[0].equip [0] == sc_read_2)
        {
                you[0].equip [0] = -1;
                strcpy(info, "You are now empty handed.");
                mpr(info);
        }
}

you[0].turnover = 1;

unsigned char scroll = 0;

scroll = sc_read_2;

strcpy(info, "As you read the scroll, it crumbles to dust.");
mpr(info);
burden_change();

if (you[0].conf != 0)
{
 random_uselessness(random2(9), sc_read_2);
 return;
}

switch(you[0].inv_type [sc_read_2])
{


case 0: // identify
you[0].inv_ident [scroll] = 2;
set_id(you[0].inv_class [sc_read_2], you[0].inv_type [sc_read_2], 1);
identify(1);
return;


case 1: // teleport
        you_teleport();
        set_id(you[0].inv_class [sc_read_2], you[0].inv_type [sc_read_2], 1);
return;

case 2: // fear
strcpy(info, "You feel scary!");
mpr(info);
set_id(you[0].inv_class [sc_read_2], you[0].inv_type [sc_read_2], 1);
 mass_enchantment(4, 1000);
break;

case 3: // noise
strcpy(info, "You hear a loud clanging noise!");
mpr(info);
noisy(25, you[0].x_pos, you[0].y_pos);
set_id(you[0].inv_class [sc_read_2], you[0].inv_type [sc_read_2], 1);
return;

case 4: // scroll of remove curse
        remove_curse();
        set_id(you[0].inv_class [sc_read_2], you[0].inv_type [sc_read_2], 1);
break;

case 5: // detect curse
 detect_curse();
set_id(you[0].inv_class [sc_read_2], you[0].inv_type [sc_read_2], 1);
break;

case 6: // summoning
strcpy(info, "A horrible Thing appears!");
mpr(info);
create_monster(23, 25, 7, you[0].x_pos, you[0].y_pos, MHITNOT, 250);
set_id(you[0].inv_class [sc_read_2], you[0].inv_type [sc_read_2], 1);
break;


case 7: // scroll of enchant weapon I
case 19: // II
case 22: // III
weapon_enchant : if (you[0].equip [0] == -1)
{
        strcpy(info, "Nothing appears to happen.");
        mpr(info);
        return;
}


if (you[0].inv_class [you[0].equip [0]] == 0 | you[0].inv_class [you[0].equip [0]] == 1)
{

if (you[0].inv_type [sc_read_2] == 19 && you[0].inv_class [you[0].equip [0]] == 0)
{
 id_used = you[0].inv_plus2 [you[0].equip [0]];
} else id_used = you[0].inv_plus [you[0].equip [0]];


if (you[0].inv_class [you[0].equip [0]] == 0 && (you[0].inv_dam [you[0].equip [0]] > 180)) // | you[0].inv_dam [you[0].equip [0]] % 30 == 13)) // unique items or vampiric weapons - note vampiric weapons are no longer restricted
{
        strcpy(info, "Nothing appears to happen.");
        mpr(info);
        return;
}

 if (you[0].inv_dam [you[0].equip [0]] == 4) // electrocution
 {
   if (id_used > 130)
   {
    id_used -= 100;
   }
                item_name(you[0].inv_plus2 [you[0].equip [0]], you[0].inv_class [you[0].equip [0]], you[0].inv_type [you[0].equip [0]], you[0].inv_dam [you[0].equip [0]], you[0].inv_plus [you[0].equip [0]], you[0].inv_quant [you[0].equip [0]], you[0].inv_ident [you[0].equip [0]], 4, str_pass);

   id_used += 3 + random2(3) + random2(3);
   if (id_used >= 59) id_used = 59;
   you[0].inv_plus2 [you[0].equip [0]] = id_used;
   strcpy(info, str_pass);
   strcat(info, " glows electric blue for a moment.");
   mpr(info);
   return;
 }

 affected = 1;

 if (id_used >= 154 | (id_used < 100 && id_used >= 54))
 {
   if (random2(9) < id_used % 50) affected = 0;
 }

// if it isn't affected by the enchantment, it will still be uncursed:
        if (you[0].inv_plus [you[0].equip [0]] >= 154)
        {
                if (affected == 0)
                {
                item_name(you[0].inv_plus2 [you[0].equip [0]], you[0].inv_class [you[0].equip [0]], you[0].inv_type [you[0].equip [0]], you[0].inv_dam [you[0].equip [0]], you[0].inv_plus [you[0].equip [0]], you[0].inv_quant [you[0].equip [0]], you[0].inv_ident [you[0].equip [0]], 4, str_pass);

                strcpy(info, str_pass);
                strcat(info, " glows silver for a moment.");
                mpr(info);
                you[0].inv_plus [you[0].equip [0]] -= 100;
                set_id(you[0].inv_class [sc_read_2], you[0].inv_type [sc_read_2], 1);
                return;
                }
        }

//      if (id_used - 50 >= 4 && id_used < 130)
//      {
                if (affected == 0) goto nothing_happened;
//              if (random2(9) < (you[0].inv_plus [you[0].equip [0]] - 50)) goto nothing_happened;
//      }



        // vVvVv    This is *here* (as opposed to lower down) for a reason!
        item_name(you[0].inv_plus2 [you[0].equip [0]], you[0].inv_class [you[0].equip [0]], you[0].inv_type [you[0].equip [0]], you[0].inv_dam [you[0].equip [0]], you[0].inv_plus [you[0].equip [0]], you[0].inv_quant [you[0].equip [0]], you[0].inv_ident [you[0].equip [0]], 4, str_pass);

        strcpy(info, str_pass);
        if (you[0].inv_type [sc_read_2] == 19 && you[0].inv_class [you[0].equip [0]] == 0)
        {
         strcat(info, " glows red for a moment.");
         mpr(info);
        } else
        if (you[0].inv_type [sc_read_2] == 7 && you[0].inv_class [you[0].equip [0]] == 0)
        {
         strcat(info, " glows green for a moment.");
         mpr(info);
        } else if (you[0].inv_class [you[0].equip [0]] == 0)
        {
         id_used --; /* no plusses for using this */
         if (you[0].duration [5] > 0)
         {
          you[0].duration [5] = 0;
          strcat(info, "'s sharpness seems more permanent.");
          mpr(info);
         } else
         if (you[0].duration [6] > 0)
         {
          you[0].duration [6] = 0;
          strcat(info, " is engulfed in an explosion of flames!");
          mpr(info);
          beam[0].type = 43;
          beam[0].damage = 110;
          beam[0].flavour = 2;
          beam[0].bx = you[0].x_pos;
          beam[0].by = you[0].y_pos;
          strcpy(beam[0].beam_name, "fiery explosion");
          beam[0].colour = RED;
          beam[0].thing_thrown = 1;
          explosion(1, beam);
         } else
         if (you[0].duration [7] > 0)
         {
          you[0].duration [7] = 0;
          strcat(info, " glows brilliantly blue for a moment.");
          mpr(info);
          cast_refrigeration(60);
         } else
         if (you[0].duration [8] > 0)
         {
          you[0].duration [8] = 0;
          strcat(info, " thirsts for the lives of mortals!");
          mpr(info);
          if (player_prot_life() == 0 && you[0].is_undead == 0)
          {
           mpr("You feel drained.");
           drain_exp();
          }
         } else
         if (you[0].duration [15] > 0)
         {
          you[0].duration [15] = 0;
          strcat(info, " seems more permanently poisoned.");
          mpr(info);
          cast_toxic_radiance();
         } else
         {
           strcat(info, " glows bright yellow for a while.");
           mpr(info);
           you[0].inv_plus [you[0].equip [0]] += 1 + random2(2);
           you[0].inv_plus2 [you[0].equip [0]] += 1 + random2(2);
           if (you[0].inv_plus [you[0].equip [0]] > 130) you[0].inv_plus [you[0].equip [0]] -= 100;
           return;
         }
        }
        set_id(you[0].inv_class [sc_read_2], you[0].inv_type [sc_read_2], 1);


        id_used ++;
        if (you[0].inv_plus [you[0].equip [0]] > 130) you[0].inv_plus [you[0].equip [0]] -= 100;
        if (id_used > 130) id_used -= 100;

        if (you[0].inv_type [sc_read_2] == 19 && you[0].inv_class [you[0].equip [0]] == 0)
        {
         you[0].inv_plus2 [you[0].equip [0]] = id_used;
        } else if (you[0].inv_type [sc_read_2] == 7 && you[0].inv_class [you[0].equip [0]] == 0)
        {
         you[0].inv_plus [you[0].equip [0]] = id_used;
        }
        return;

//        strcat(info,
}

nothing_happened: strcpy(info, "Nothing appears to happen.");
mpr(info);
return; // end of case 7: enc weapon

case 20: // vorpalise weapon
if (you[0].equip [0] == -1 | you[0].inv_class [you[0].equip [0]] != 0 | (you[0].inv_class [you[0].equip [0]] == 0 && (you[0].inv_dam [you[0].equip [0]] > 180 | (you[0].inv_type [you[0].equip [0]] >= 13 && you[0].inv_type [you[0].equip [0]] <= 16))))
{
        strcpy(info, "Nothing appears to happen.");
        mpr(info);
        return;
}

 item_name(you[0].inv_plus2 [you[0].equip [0]], you[0].inv_class [you[0].equip [0]], you[0].inv_type [you[0].equip [0]], you[0].inv_dam [you[0].equip [0]], you[0].inv_plus [you[0].equip [0]], you[0].inv_quant [you[0].equip [0]], you[0].inv_ident [you[0].equip [0]], 4, str_pass);
 strcpy(info, str_pass);
 strcat(info, " emits a brilliant flash of light!");
 alert();
 mpr(info);
 set_id(you[0].inv_class [sc_read_2], you[0].inv_type [sc_read_2], 1);

if (you[0].inv_dam [you[0].equip [0]] % 30 != 0)
{
 you[0].inv_plus [you[0].equip [0]] = 50;
 you[0].inv_plus2 [you[0].equip [0]] = 50;
 switch(you[0].inv_dam [you[0].equip [0]] % 30)
 {
  case 7:
/*  player_AC() -= 3;*/
  you[0].AC_ch = 1;
  break;

  case 13:
  you[0].hunger_inc -= 6;
  break;
 }
 you[0].inv_dam [you[0].equip [0]] = (you[0].inv_dam [you[0].equip [0]] / 30) * 30;
 if (you[0].inv_dam [you[0].equip [0]] == 90) you[0].inv_dam [you[0].equip [0]] = 0; // stops glowing
 you[0].duration [5] = 0;
 you[0].duration [6] = 0;
 you[0].duration [7] = 0;
 you[0].duration [8] = 0;
 return;
}
you[0].inv_dam [you[0].equip [0]] += 10;
return;


case 21: // recharge wand
if (you[0].equip [0] != -1 && you[0].inv_class [you[0].equip [0]] == 0 && you[0].inv_dam [you[0].equip [0]] == 4) // electrocution
 goto weapon_enchant;

if (recharge_wand() == 0)
{
 strcpy(info, "Nothing appears to happen.");
 mpr(info);
} else set_id(you[0].inv_class [sc_read_2], you[0].inv_type [sc_read_2], 1);
return;


case 8: // ench you[0].equip
for (i = 1; i < 8; i ++)
{
        if (i == 7)
        {
                strcpy(info, "Nothing appears to happen.");
                mpr(info);
                return;
        }

        if (you[0].equip [i] != -1) break;
}

do
{
        affected = random2(6) + 1;
} while (you[0].equip [affected] == -1);

// NOTE: It is assumed that armour which changes in this way does not change
//  into a form of armour with a different evasion modifier.


        if (you[0].inv_type [you[0].equip [6]] == 15 | you[0].inv_type [you[0].equip [6]] == 20 | you[0].inv_type [you[0].equip [6]] == 22 | you[0].inv_type [you[0].equip [6]] == 24 | you[0].inv_type [you[0].equip [6]] == 26 | you[0].inv_type [you[0].equip [6]] == 28) // dragon hide
        {
                affected = 6;
                item_name(you[0].inv_plus2 [you[0].equip [affected]], you[0].inv_class [you[0].equip [affected]], you[0].inv_type [you[0].equip [affected]], you[0].inv_dam [you[0].equip [affected]], you[0].inv_plus [you[0].equip [affected]], you[0].inv_quant [you[0].equip [affected]], you[0].inv_ident [you[0].equip [affected]], 4, str_pass);
                strcpy(info, str_pass);
                strcat(info, " glows purple and changes!");
                mpr(info);
                you[0].AC_ch = 1;

/* player_AC() -= property [2] [you[0].inv_type [you[0].equip [6]]] [0] * (15 + you[0].skills [13]) / 15;


        player_evasion() -= ev_mod();*/

                switch(you[0].inv_type [you[0].equip [6]])
                {

                 case 15:
/*                 player_res_fire() ++;
                 player_res_cold() --;*/
                 you[0].inv_type [you[0].equip [6]] = 18;
                 break;

                 case 20:
/*                 player_res_cold() ++;
                 player_res_fire() --;*/
                 you[0].inv_type [you[0].equip [6]] = 21;
                 break;

                 case 22: // steam dragon
                 you[0].inv_type [you[0].equip [6]] = 23;
                 break;

                 case 24: // mottled dragon
                 you[0].inv_type [you[0].equip [6]] = 25;
                 break;

                 case 26: // storm dragon
                 you[0].inv_type [you[0].equip [6]] = 27;
                 you[0].attribute [0] ++;
                 break;

                 case 28: // gold dragon
                 you[0].inv_type [you[0].equip [6]] = 29;
/*                 player_res_fire() ++;
                 player_res_cold() ++;
                 you[0].res_poison ++;*/
                 break;

                }
/*        player_evasion() += ev_mod();
 player_AC() += property [2] [you[0].inv_type [you[0].equip [6]]] [0] * (15 + you[0].skills [13]) / 15;*/

        set_id(you[0].inv_class [sc_read_2], you[0].inv_type [sc_read_2], 1);
        return;
        }


        if (you[0].inv_type [you[0].equip [6]] == 16) // troll hide
        {
                affected = 6;
                item_name(you[0].inv_plus2 [you[0].equip [affected]], you[0].inv_class [you[0].equip [affected]], you[0].inv_type [you[0].equip [affected]], you[0].inv_dam [you[0].equip [affected]], you[0].inv_plus [you[0].equip [affected]], you[0].inv_quant [you[0].equip [affected]], you[0].inv_ident [you[0].equip [affected]], 4, str_pass);
                strcpy(info, str_pass);
                strcat(info, " glows purple and changes!");
                mpr(info);
/*                player_AC() += 1;*/
                you[0].AC_ch = 1;
/*                you[0].rate_regen += 50;*/
                you[0].hunger_inc += 1;
/* player_AC() -= property [2] [you[0].inv_type [you[0].equip [6]]] [0] * (15 + you[0].skills [13]) / 15;
        player_evasion() -= ev_mod();*/
                you[0].inv_type [you[0].equip [6]] = 19;
/*        player_evasion() += ev_mod();
 player_AC() += property [2] [you[0].inv_type [you[0].equip [6]]] [0] * (15 + you[0].skills [13]) / 15;*/
        set_id(you[0].inv_class [sc_read_2], you[0].inv_type [sc_read_2], 1);
        return;
        }

        if (you[0].inv_plus [you[0].equip [affected]] >= 153)
        {
                if (random2(8) >= (you[0].inv_plus [you[0].equip [affected]] - 150))
                {
                item_name(you[0].inv_plus2 [you[0].equip [affected]], you[0].inv_class [you[0].equip [affected]], you[0].inv_type [you[0].equip [affected]], you[0].inv_dam [you[0].equip [affected]], you[0].inv_plus [you[0].equip [affected]], you[0].inv_quant [you[0].equip [affected]], you[0].inv_ident [you[0].equip [affected]], 4, str_pass);

                you[0].inv_plus [you[0].equip [affected]] -= 100;
                set_id(you[0].inv_class [sc_read_2], you[0].inv_type [sc_read_2], 1);

                strcpy(info, str_pass);
                strcat(info, " glows silver for a moment.");
                mpr(info);
                return;
                }
        }

        if ((you[0].inv_plus [you[0].equip [affected]] - 50 >= 3 && you[0].inv_plus [you[0].equip [affected]] < 130) | you[0].inv_plus [you[0].equip [affected]] >= 153)
        {
                if (random2(8) < (you[0].inv_plus [you[0].equip [affected]] - 50)) goto nothing_happened_2;
        }

        if (you[0].inv_type [you[0].equip [affected]] >= 9 && you[0].inv_type [you[0].equip [affected]] <= 12)
          if ((you[0].inv_plus [you[0].equip [affected]] - 50 >= 2 && you[0].inv_plus [you[0].equip [affected]] < 130) | you[0].inv_plus [you[0].equip [affected]] >= 152)
              goto nothing_happened_2;

        // vVvVv    This is *here* for a reason!
        item_name(you[0].inv_plus2 [you[0].equip [affected]], you[0].inv_class [you[0].equip [affected]], you[0].inv_type [you[0].equip [affected]], you[0].inv_dam [you[0].equip [affected]], you[0].inv_plus [you[0].equip [affected]], you[0].inv_quant [you[0].equip [affected]], you[0].inv_ident [you[0].equip [affected]], 4, str_pass);

        you[0].inv_plus [you[0].equip [affected]] ++;
        if (you[0].inv_plus [you[0].equip [affected]] > 130) you[0].inv_plus [you[0].equip [affected]] -= 100;
        //strcpy(info, "Your ");
        //in_name(you[0].equip [affected], 4);

        set_id(you[0].inv_class [sc_read_2], you[0].inv_type [sc_read_2], 1);

        strcpy(info, str_pass);
        strcat(info, " glows green for a moment.");
/* if (affected != 5) player_AC() ++; else player_shield_class() ++;*/
 you[0].AC_ch = 1;
        mpr(info);
        return; // end of ench you[0].equip

case 9: // torment
/*      beam[0].bx = you[0].x_pos;
        beam[0].by = you[0].y_pos;*/
        torment(); //you[0].x_pos, you[0].y_pos);
        if (get_id(you[0].inv_class [sc_read_2], you[0].inv_type [sc_read_2]) == 1)
//         naughty(20, 2); // is only naughty if you know you're doing it
 set_id(you[0].inv_class [sc_read_2], you[0].inv_type [sc_read_2], 1);
        return;

case 10: // random uselessness
        random_uselessness(random2(9), sc_read_2); //random2 (1));
        return;

case 11: // curse weapon
        if (you[0].equip [0] == -1 | you[0].inv_class [you[0].equip [0]] > 0 | you[0].inv_plus [you[0].equip [0]] > 130)
        {
                goto nothing_happened_2;
        }
                in_name(you[0].equip [0], 4, str_pass);
                strcpy(info, str_pass);
                strcat(info, " glows black for a moment.");
                mpr(info);
                you[0].inv_plus [you[0].equip [0]] += 100;
        set_id(you[0].inv_class [sc_read_2], you[0].inv_type [sc_read_2], 1);
                return;


case 12: // curse you[0].equip
for (i = 1; i < 8; i ++)
{
        if (i == 7)
        {
                strcpy(info, "Nothing appears to happen.");
                mpr(info);
                return;
        }
        if (you[0].equip [i] != -1 && you[0].inv_plus [you[0].equip [i]] < 130) break;
}

do
{
        affected = random2(6) + 1;
} while (you[0].equip [affected] == -1 | you[0].inv_plus [you[0].equip [affected]] > 130);


        // vVvVv    This is *here* for a reason!
        item_name(you[0].inv_plus2 [you[0].equip [affected]], you[0].inv_class [you[0].equip [affected]], you[0].inv_type [you[0].equip [affected]], you[0].inv_dam [you[0].equip [affected]], you[0].inv_plus [you[0].equip [affected]], you[0].inv_quant [you[0].equip [affected]], you[0].inv_ident [you[0].equip [affected]], 4, str_pass);
        you[0].inv_plus [you[0].equip [affected]] += 100;
        set_id(you[0].inv_class [sc_read_2], you[0].inv_type [sc_read_2], 1);
        strcpy(info, str_pass);
        strcat(info, " glows black for a moment.");
        mpr(info);
        return;


case 13: // immolation
strcpy(info, "The scroll explodes in your hands!");
mpr(info);

        beam[0].type = 43;
        beam[0].damage = 110;
        beam[0].flavour = 2; // <=- not sure about this
        beam[0].bx = you[0].x_pos;
        beam[0].by = you[0].y_pos;
        strcpy(beam[0].beam_name, "fiery explosion");
        beam[0].colour = RED;
        beam[0].thing_thrown = 1; // your explosion, (not someone else's explosion)
        explosion(1, beam);
 set_id(you[0].inv_class [sc_read_2], you[0].inv_type [sc_read_2], 1);
break;

case 14: // blink

   blink();

   set_id(you[0].inv_class [sc_read_2], you[0].inv_type [sc_read_2], 1);

break;

// This is never called; it is dealt with earlier:
case 15: // paper
break;

case 16: // magic mapping
if (you[0].level_type == 1 | you[0].level_type == 2)
{
 strcpy(info, "You feel momentarily disoriented.");
 mpr(info);
 set_id(you[0].inv_class [sc_read_2], you[0].inv_type [sc_read_2], 1);
 break;
}
strcpy(info, "You feel aware of your surroundings.");
mpr(info);
magic_mapping(21, 95 + random2(10));
set_id(you[0].inv_class [sc_read_2], you[0].inv_type [sc_read_2], 1);
break;

case 17: // forgetting
strcpy(info, "You feel momentarily disoriented.");
mpr(info);
forget_map(50 + random2(50));
set_id(you[0].inv_class [sc_read_2], you[0].inv_type [sc_read_2], 1);
break;

case 18:
acquirement(250);
set_id(you[0].inv_class [sc_read_2], you[0].inv_type [sc_read_2], 1);
break;


} // end of switch


return;

nothing_happened_2: strcpy(info, "Nothing appears to happen.");
mpr(info);
return;


} // end of void read_scroll()



void original_name(void)
{
char drink_1;
char drink_2;
char inn = 0;
unsigned char nthing = 0;

query : strcpy(info, "Examine which item?");
mpr(info);

int keyin = get_ch();

if (keyin == '?' | keyin == '*')
{
        if (keyin == '?') nthing = get_invent(-1);
        if (keyin == '*') nthing = get_invent(-1);
        if ((nthing >= 65 && nthing <= 90) | (nthing >= 97 && nthing <= 122))
        {
                keyin = nthing;
        } else
        {
       mesclr();
    goto query;
        }
}

drink_1 = (int) keyin;

if (drink_1 < 65 | (drink_1 > 90 && drink_1 < 97) | drink_1 > 122)
{
        strcpy(info, "You don't have any such object.");
        mpr(info);
        return;
}

drink_2 = conv_lett(drink_1);

if (you[0].inv_quant [drink_2] == 0)
{
        strcpy(info, "You don't have any such object.");
        mpr(info);
        return;
}

inn = drink_2;
describe_item(you[0].inv_class [inn], you[0].inv_type [inn], you[0].inv_plus [inn], you[0].inv_plus2 [inn], you[0].inv_dam [inn], you[0].inv_ident [inn]);
#ifdef LINUX
redraw_screen();
#endif

} // end original_name

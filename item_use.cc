#include "config.h"

#include <string.h>
#include <stdlib.h>

#include "externs.h"
#include "enum.h"
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
#include "mstruct.h"
//#include "mstuff2.h"
#include "newgame.h"
#include "ouch.h"
#include "output.h"
#include "player.h"
//#include "priest.h"
#include "randart.h"
#include "religion.h"
#include "skills.h"
#include "skills2.h"
//#include "spell.h"
#include "spells.h"
#include "spells0.h"
#include "spells1.h"
#include "spells2.h"
#include "spells3.h"
#include "stuff.h"
#include "transfor.h"
#include "view.h"

void throw_it(struct bolt beam [1], int throw_2);
void use_randart(unsigned char item_wield_2);
char drink_fountain(void);

extern int book_thing; /* defined in spells.cc */
extern char wield_change; /* defined in output.cc */


void wield_weapon(char auto_wield)
{

unsigned char nthing = 0;
unsigned char i_dam = 0;
char str_pass [80];
unsigned char keyin;

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

if (you[0].attribute [ATTR_TRANSFORMATION] != TRAN_NONE)
{
 if (can_equip(0) == 0)
 {
  mpr("You can't wield anything in your present form.");
  return;
 }
}

if ((you[0].equip [EQ_WEAPON] != -1) && you[0].inv_class [you[0].equip [EQ_WEAPON]] == OBJ_WEAPONS && you[0].inv_plus [you[0].equip [EQ_WEAPON]] > 80)
{
        strcpy(info, "You can't unwield your weapon to draw a new one!");
        mpr(info);
        return;
}


if (auto_wield == 1)
{
  if (you[0].equip [EQ_WEAPON] == OBJ_WEAPONS) keyin = 'b';
   else keyin = 'a';
} else
{

query : strcpy(info, "Wield which item (- for none)?");
mpr(info);

// test relay_message();

keyin = get_ch();

if (keyin == '?')
{
        //inventory(0);
        nthing = get_invent(OBJ_WEAPONS);
//invent(you[0].inv_plus2, 0, you[0].inv_quant, you[0].inv_dam, you[0].inv_class, you[0].inv_type, you[0].inv_plus, you[0].inv_ident, you[0].equip [0], you[0].equip [6], you[0].equip [5], you[0].equip [2], you[0].equip [1], you[0].equip [3], you[0].equip [4], you[0].ring);
        if ((nthing >= 65 && nthing <= 90) || (nthing >= 97 && nthing <= 122))
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
        if ((nthing >= 65 && nthing <= 90) || (nthing >= 97 && nthing <= 122))
        {
                keyin = nthing;
        } else
        {
                strcpy(info, "Wield which item?"); mpr(info);
                // test relay_message();
                keyin = get_ch();
        }
}

}

int item_wield_1 = (int) keyin;

if (keyin == '-')
{
 if (you[0].equip [EQ_WEAPON] != -1)
 {
  unwield_item(you[0].equip [EQ_WEAPON]);
  you[0].turnover = 1;
 }
 you[0].equip [EQ_WEAPON] = -1;
 strcpy(info, "You are empty handed.");
 mpr(info);
 you[0].time_taken *= 3;
 you[0].time_taken /= 10;
 return;
}

if (item_wield_1 < 65 || (item_wield_1 > 90 && item_wield_1 < 97) || item_wield_1 > 122)
{
        strcpy(info, "You don't have any such object.");
        mpr(info);
        return;
}

int item_wield_2 = conv_lett(item_wield_1);

if (item_wield_2 == you[0].equip [EQ_WEAPON])
{
        strcpy(info, "You are already wielding that!");
        mpr(info);
        return;
}

if (item_wield_2 == you[0].equip [EQ_BODY_ARMOUR] || item_wield_2 == you[0].equip [EQ_SHIELD] || item_wield_2 == you[0].equip [EQ_HELMET] || item_wield_2 == you[0].equip [EQ_CLOAK] || item_wield_2 == you[0].equip [EQ_BOOTS] || item_wield_2 == you[0].equip [EQ_GLOVES]) // later add:   || == helmet etc
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

if (you[0].inv_class [item_wield_2] != OBJ_WEAPONS)
{
 if (you[0].inv_class [item_wield_2] == OBJ_STAVES && you[0].equip [EQ_SHIELD] != -1)
 {
      strcpy(info, "You can't wield that with a shield.");
      mpr(info);
      return;
 }

 if (you[0].equip [EQ_WEAPON] != -1) unwield_item(you[0].equip [EQ_WEAPON]);
        you[0].equip [EQ_WEAPON] = item_wield_2;
} else
        {

   if ((you[0].species < SP_OGRE || you[0].species > SP_OGRE_MAGE) && mass(you[0].inv_class [item_wield_2], you[0].inv_type [item_wield_2]) >= 500)
   {
      strcpy(info, "That's too large and heavy for you to wield.");
      mpr(info);
      return;
   }

   if ((you[0].species == SP_HALFLING || you[0].species == SP_GNOME || you[0].species == SP_KOBOLD) && (you[0].inv_type [item_wield_2] == WPN_GREAT_SWORD || you[0].inv_type [item_wield_2] == WPN_BATTLEAXE || you[0].inv_type [item_wield_2] == WPN_HALBERD || you[0].inv_type [item_wield_2] == WPN_GLAIVE || you[0].inv_type [item_wield_2] == WPN_SCYTHE))
   {
      strcpy(info, "That's too large for you to wield.");
      mpr(info);
      return; // halflings or kobolds
   }


   if ((you[0].inv_type [item_wield_2] == WPN_GREAT_SWORD || you[0].inv_type [item_wield_2] == WPN_BATTLEAXE || you[0].inv_type [item_wield_2] == WPN_HALBERD || you[0].inv_type [item_wield_2] == WPN_GLAIVE || you[0].inv_type [item_wield_2] == WPN_QUARTERSTAFF || you[0].inv_type [item_wield_2] == WPN_SCYTHE || you[0].inv_type [item_wield_2] == WPN_GIANT_CLUB || you[0].inv_type [item_wield_2] == WPN_GIANT_SPIKED_CLUB || you[0].inv_type [item_wield_2] == WPN_EXECUTIONERS_AXE || you[0].inv_type [item_wield_2] == WPN_TRIPLE_SWORD) && you[0].equip [EQ_SHIELD] != -1)
   {
      strcpy(info, "You can't wield that with a shield.");
      mpr(info);
      return;
   }

   if ((you[0].is_undead != 0 || you[0].species == SP_DEMONSPAWN) && (you[0].inv_dam [item_wield_2] % 30 == SPWPN_HOLY_WRATH || you[0].inv_dam [item_wield_2] % 30 == SPWPN_DISRUPTION))
   {
      strcpy(info, "This weapon will not allow you to wield it.");
      mpr(info);
      you[0].turnover = 1;
      return; // Mummies can't wield holy weapons
   }


 if (you[0].equip [EQ_WEAPON] != -1) unwield_item(you[0].equip [EQ_WEAPON]);
        you[0].equip [EQ_WEAPON] = item_wield_2;
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
wield_change = 1;

if (you[0].inv_plus [item_wield_2] > 80 && you[0].inv_class [item_wield_2] == OBJ_WEAPONS)
{
        strcpy(info, "It sticks to your hand!");
// if (you[0].inv_ident [item_wield_2] == 0)    you[0].inv_ident [item_wield_2] = 2;
        mpr(info);
}

 if (you[0].inv_class [item_wield_2] == OBJ_MISCELLANY && you[0].inv_type [item_wield_2] == MISC_LANTERN_OF_SHADOWS)
 {
  strcpy(info, "The area is filled with flickering shadows.");
  mpr(info);
  you[0].special_wield = SPWLD_SHADOW;
 }

 if (you[0].inv_class [item_wield_2] == OBJ_WEAPONS && you[0].inv_ident [item_wield_2] == 0)    you[0].inv_ident [item_wield_2] = 2;

if (you[0].inv_class [item_wield_2] == OBJ_WEAPONS && you[0].inv_dam [item_wield_2] != 0)
{

if (you[0].inv_ident [item_wield_2] == 1) you[0].inv_ident [item_wield_2] = 2;

i_dam = you[0].inv_dam [item_wield_2];

if (you[0].inv_dam [item_wield_2] <= 180)
{
 i_dam = you[0].inv_dam [item_wield_2] % 30;
}

if (you[0].inv_dam [item_wield_2] % 30 >= 25)
{
 i_dam = randart_wpn_properties(you[0].inv_class [item_wield_2], you[0].inv_type [item_wield_2], you[0].inv_dam [item_wield_2], you[0].inv_plus [item_wield_2], you[0].inv_plus2 [item_wield_2], 0, OBJ_WEAPONS);
}

switch(i_dam) //you[0].inv_dam [item_wield_2] % 30)
{

  case SPWPN_FLAMING:
  strcpy(info, "It bursts into flame!");
  mpr(info);
  break;

  case SPWPN_FREEZING:
  strcpy(info, "It glows with a cold blue light!");
  mpr(info);
  break;

  case SPWPN_HOLY_WRATH:
  strcpy(info, "It softly glows with a divine radiance!");
  mpr(info);
  break;

  case SPWPN_ELECTROCUTION:
  strcpy(info, "You hear the crackle of electricity.");
  mpr(info);
  break;

  case SPWPN_ORC_SLAYING:
  strcpy(info, "You feel a sudden desire to kill orcs!");
  if (you[0].species == SP_HILL_ORC) strcpy(info, "You feel a sudden desire to commit suicide.");
  mpr(info);
  break;

  case SPWPN_VENOM:
  strcpy(info, "It begins to drip with poison!");
  mpr(info);
  break;

  case SPWPN_PROTECTION:
  strcpy(info, "You feel protected!");
  mpr(info);
/*  player_AC() += 5;*/
  you[0].AC_ch = 1;
  break;

  case SPWPN_DRAINING:
  strcpy(info, "You sense an unholy aura.");
                mpr(info);
                break;

  case SPWPN_SPEED:
  strcpy(info, "Your hands tingle!");
                mpr(info);
                break;

  case SPWPN_FLAME:
  strcpy(info, "It glows red for a moment.");
                mpr(info);
                break;

  case SPWPN_FROST:
  strcpy(info, "It is covered in frost.");
                mpr(info);
                break;

  case SPWPN_VAMPIRICISM:
  strcpy(info, "You feel a strange hunger.");
  you[0].hunger_inc += 6;
                mpr(info);
                break;

  case SPWPN_DISRUPTION:
  strcpy(info, "You sense a holy aura.");
                mpr(info);
                break;

  case SPWPN_PAIN:
  strcpy(info, "A searing pain shoots up your arm!");
                mpr(info);
                break;

  case SPWPN_DISTORTION:
  miscast_effect(20, 9, 90, 100);
  break;

  case NWPN_SINGING_SWORD:
  strcpy(info, "The Singing Sword hums in delight!");
                mpr(info);
  you[0].special_wield = SPWLD_SING;
                break;

  case NWPN_WRATH_OF_TROG:
  strcpy(info, "You feel bloodthirsty!");
                mpr(info);
  you[0].special_wield = SPWLD_TROG;
                break;

  case NWPN_SCYTHE_OF_CURSES:
  strcpy(info, "A shiver runs down your spine.");
                mpr(info);
  you[0].special_wield = SPWLD_CURSE;
                break;

  case NWPN_MACE_OF_VARIABILITY:
//  strcpy(info, "You feel bloodthirsty!");
//              mpr(info);
  you[0].special_wield = SPWLD_VARIABLE;
                break;

  case NWPN_GLAIVE_OF_PRUNE:
  strcpy(info, "You feel pruney.");
                mpr(info);
  you[0].special_wield = SPWLD_NONE;
                break;

  case NWPN_SCEPTRE_OF_TORMENT:
  strcpy(info, "A terribly searing pain shoots up your arm!");
                mpr(info);
  you[0].special_wield = SPWLD_TORMENT;
                break;

  case NWPN_SWORD_OF_ZONGULDROK:
  strcpy(info, "You sense an extremely unholy aura.");
                mpr(info);
  you[0].special_wield = SPWLD_ZONGULDROK;
                break;

  case NWPN_SWORD_OF_POWER:
  strcpy(info, "You sense an aura of extreme power.");
                mpr(info);
  you[0].special_wield = SPWLD_POWER;
                break;

  case NWPN_STAFF_OF_OLGREB:
  strcpy(info, "You smell chlorine.");
                mpr(info);
/*                you[0].res_poison ++;
                you[0].spec_poison ++;
                you[0].mag_abil ++;*/
//  you[0].special_wield = 8;
                break;

  case NWPN_VAMPIRE_S_TOOTH:
  strcpy(info, "You feel a strange hunger, and smell blood on the air...");
  you[0].hunger_inc += 9;
                mpr(info);
                break;

  case NWPN_STAFF_OF_WUCAD_MU:
  miscast_effect(19, 9, 90, 100);
  you[0].special_wield = SPWLD_WUCAD_MU;
  break;



} // end of switch

if (you[0].inv_dam [item_wield_2] % 30 >= 25)
{
 use_randart(item_wield_2);
}

if ((you[0].inv_type [item_wield_2] == WPN_DEMON_BLADE || you[0].inv_type [item_wield_2] == WPN_DEMON_WHIP) && (you[0].religion == GOD_ZIN || you[0].religion == GOD_SHINING_ONE || you[0].religion == GOD_ELYVILON))
 mpr("You really shouldn't be using a nasty item like this.");

}

if (you[0].inv_class [item_wield_2] == OBJ_STAVES)
{
 switch(you[0].inv_type [item_wield_2])
        {
 case STAFF_WIZARDRY: // wizardry
/* you[0].mag_abil += 4;*/
        break;

 case STAFF_POWER: // magic
 you[0].ep_max += 13;
 you[0].base_ep2 += 13;
 you[0].ep_ch = 1;
/* you[0].magic_battery ++;*/
 calc_ep();
 break;

/* case STAFF_FIRE: // fire
        you[0].spec_fire ++;
  player_res_fire() ++;
        break;

        case STAFF_COLD: // cold
 you[0].spec_cold ++;
 player_res_cold() ++
        break;

 case STAFF_POISON:
 you[0].spec_poison ++;
 you[0].res_poison ++;
        break;

 case STAFF_ENERGY:
 you[0].energy ++;
 break;

 case STAFF_DEATH:
 you[0].spec_death ++;
 break;

 case STAFF_CONJURATION:
 you[0].spec_conj ++;
 break;

 case STAFF_ENCHANTMENT:
 you[0].spec_ench ++;
 break;

 case STAFF_SUMMONING_I:
 you[0].spec_summ ++;
 break;

 case STAFF_AIR: // air
 you[0].attribute [ATTR_SPEC_AIR] ++;
 you[0].attribute [ATTR_RESIST_LIGHTNING] ++; // res elec
 break;

 case STAFF_EARTH:
 you[0].attribute [ATTR_SPEC_EARTH] ++;
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

 wield_change = 1;


} // end of wield_weapon()



void wear_armour(void)
{

unsigned char nthing = 0;
char armr = you[0].equip [EQ_BODY_ARMOUR];
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
        nthing = get_invent(OBJ_ARMOUR);
//invent(you[0].inv_plus2, 2, you[0].inv_quant, you[0].inv_dam, you[0].inv_class, you[0].inv_type, you[0].inv_plus, you[0].inv_ident, you[0].equip [0], you[0].equip [6], you[0].equip [5], you[0].equip [2], you[0].equip [1], you[0].equip [3], you[0].equip [4], you[0].ring);
        if ((nthing >= 65 && nthing <= 90) || (nthing >= 97 && nthing <= 122))
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
        if ((nthing >= 65 && nthing <= 90) || (nthing >= 97 && nthing <= 122))
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
you[0].equip [EQ_WEAPON] = -1;
strcpy(info, "You are empty handed.");
mpr(info);
damage_ch = 1;
return;
}*/

if (armour_wear_1 < 65 || (armour_wear_1 > 90 && armour_wear_1 < 97) || armour_wear_1 > 122)
{
        strcpy(info, "You don't have any such object.");
        mpr(info);
        return;
}

/*
int you[0].equip [EQ_BODY_ARMOUR] = -1;
int you[0].equip [EQ_CLOAK] = -1;
int you[0].equip [EQ_HELMET] = -1;
int you[0].equip [EQ_GLOVES] = -1;
int you[0].equip [EQ_BOOTS] = -1;
int you[0].equip [EQ_SHIELD] = -1;
*/

int armour_wear_2 = conv_lett(armour_wear_1);

armr = armour_wear_2;

if (you[0].inv_class [armour_wear_2] != OBJ_ARMOUR)
{
        mpr("You can't wear that thing.");
        return;
}

if (armour_wear_2 == you[0].equip [EQ_BODY_ARMOUR] || armour_wear_2 == you[0].equip [EQ_CLOAK] || armour_wear_2 == you[0].equip [EQ_SHIELD] || armour_wear_2 == you[0].equip [EQ_HELMET] || armour_wear_2 == you[0].equip [EQ_GLOVES] || armour_wear_2 == you[0].equip [EQ_BOOTS]) // or armour_wear_w == helmet etc
{
        mpr("You are already wearing that!");
        return;
}

if (you[0].inv_quant [armour_wear_2] == 0)
{
        mpr("You don't have any such object.");
        return;
}

if (armour_wear_2 == you[0].equip [EQ_WEAPON])
{
        mpr("You are wielding that object!");
        return;
}

if ((you[0].inv_type [armour_wear_2] < ARM_SHIELD || you[0].inv_type [armour_wear_2] > ARM_LARGE_SHIELD) && you[0].equip [EQ_CLOAK] != -1)
{
        mpr("You can't wear that over your cloak.");
        return;
}


if ((you[0].inv_type [armour_wear_2] < ARM_SHIELD || you[0].inv_type [armour_wear_2] > ARM_LARGE_SHIELD) && you[0].equip [EQ_BODY_ARMOUR] != -1)
{
        mpr("You are already wearing some body armour.");
        return;
}


if ((you[0].inv_type [armour_wear_2] == ARM_SHIELD || you[0].inv_type [armour_wear_2] == ARM_LARGE_SHIELD || you[0].inv_type [armour_wear_2] == ARM_BUCKLER) && you[0].equip [EQ_SHIELD] != -1)
{
        mpr("You are already holding a shield.");
        return;
}

/* This next bit was fixed and restructured by Daniel Ligon: */
if (
          you[0].equip [EQ_WEAPON] != -1   /* you're wielding something */
       && (
            /* three types of shields */
               you[0].inv_type [armour_wear_2] == ARM_SHIELD
            || you[0].inv_type [armour_wear_2] == ARM_BUCKLER
            || you[0].inv_type [armour_wear_2] == ARM_LARGE_SHIELD
          )
       && (    /* you are wielding a weapon */
               you[0].inv_class [you[0].equip [EQ_WEAPON]] == OBJ_WEAPONS
            && (
                 /* and it's two handed */
                    you[0].inv_type [you[0].equip [EQ_WEAPON]] == WPN_GREAT_SWORD
                 || you[0].inv_type [you[0].equip [EQ_WEAPON]] == WPN_BATTLEAXE
                 || you[0].inv_type [you[0].equip [EQ_WEAPON]] == WPN_HALBERD
                 || you[0].inv_type [you[0].equip [EQ_WEAPON]] == WPN_GLAIVE
                 || you[0].inv_type [you[0].equip [EQ_WEAPON]] == WPN_QUARTERSTAFF
                 || you[0].inv_type [you[0].equip [EQ_WEAPON]] == WPN_SCYTHE
                 || you[0].inv_type [you[0].equip [EQ_WEAPON]] == WPN_GIANT_CLUB
                 || you[0].inv_type [you[0].equip [EQ_WEAPON]] == WPN_GIANT_SPIKED_CLUB
               )
       || /* you are wielding a staff */
          you[0].inv_class [you[0].equip [EQ_WEAPON]] == OBJ_STAVES
          )
     )
  {
     strcpy(info, "You can't wear a shield with a two-handed weapon.");
     mpr(info);
     return;
  }

if (you[0].inv_type [armour_wear_2] == ARM_HELMET && you[0].equip [EQ_HELMET] != -1)
{
        mpr("You are already wearing a helmet.");
        return;
}

if (you[0].inv_type [armour_wear_2] == ARM_CLOAK && you[0].equip [EQ_CLOAK] != -1)
{
        mpr("You are already wearing a cloak.");
        return;
}

if (you[0].inv_type [armour_wear_2] == ARM_GLOVES && you[0].equip [EQ_GLOVES] != -1)
{
        mpr("You are already wearing some gloves.");
        return;
}

if (you[0].species != SP_NAGA && you[0].inv_type [armour_wear_2] == ARM_BOOTS && you[0].inv_plus2 [armour_wear_2] == 1)
{
        mpr("You can't wear that!");
        return;
}

if (you[0].species != SP_CENTAUR && you[0].inv_type [armour_wear_2] == ARM_BOOTS && you[0].inv_plus2 [armour_wear_2] == 2)
{
        mpr("You can't wear that!");
        return;
}

/*if (you[0].inv_type [armour_wear_2] == ARM_BOOTS && you[0].species == SP_NAGA && you[0].attribute [ATTR_TRANSFORMATION] == TRAN_NONE) / * Naga * /
{
        strcpy(info, "You don't have feet!"); / * Naga * /
        mpr(info);
        return;
}*/



//if (you[0].attribute [5] != 0)
//{
 wh_equip = EQ_BODY_ARMOUR;
 switch(you[0].inv_type [armour_wear_2])
 {
  case ARM_BUCKLER:
  case ARM_LARGE_SHIELD:
  case ARM_SHIELD: wh_equip = EQ_SHIELD; break;
  case ARM_CLOAK: wh_equip = EQ_CLOAK; break;
  case ARM_HELMET: wh_equip = EQ_HELMET; break;
  case ARM_GLOVES: wh_equip = EQ_GLOVES; break;
  case ARM_BOOTS: wh_equip = EQ_BOOTS; break;
 }

 if (you[0].species == SP_NAGA && you[0].inv_type [armour_wear_2] == ARM_BOOTS && you[0].inv_plus2 [armour_wear_2] == 1 && (you[0].attribute [ATTR_TRANSFORMATION] == TRAN_NONE || you[0].attribute [ATTR_TRANSFORMATION] == TRAN_BLADE_HANDS || you[0].attribute [ATTR_TRANSFORMATION] == TRAN_LICH))
 {
  goto fits_on_body;
 } else
 if (you[0].species == SP_CENTAUR && you[0].inv_type [armour_wear_2] == ARM_BOOTS && you[0].inv_plus2 [armour_wear_2] == 2 && (you[0].attribute [ATTR_TRANSFORMATION] == TRAN_NONE || you[0].attribute [ATTR_TRANSFORMATION] == TRAN_BLADE_HANDS || you[0].attribute [ATTR_TRANSFORMATION] == TRAN_LICH))
 {
  goto fits_on_body;
 } else
 if (can_equip(wh_equip) == 0)
 {
  mpr("You can't wear that in your present form.");
  return;
 }
//}


fits_on_body : if (you[0].inv_type [armour_wear_2] == ARM_BOOTS && you[0].equip [EQ_BOOTS] != -1)
{
        strcpy(info, "You can't wear that.");
        mpr(info);
        return;
}

if (you[0].inv_type [armour_wear_2] == ARM_CRYSTAL_PLATE_MAIL && (you[0].skills [SK_ARMOUR] < 10 || you[0].species == SP_KOBOLD || you[0].species == SP_HALFLING || you[0].species == SP_GNOME || (you[0].species >= SP_ELF && you[0].species <= SP_SLUDGE_ELF))) // no halflings or elves
{
/* if (you[0].species == SP_NAGA || you[0].species == SP_KOBOLD || you[0].species == SP_GNOME || you[0].species == SP_HALFLING || (you[0].species >= SP_ELF && you[0].species <= SP_SLUDGE_ELF))
 {
  strcpy(info, "I'm afraid you have the wrong body type. Try something lighter.");
  mpr(info);
  return;
 } else*/
 {
  mpr("This armour is very cumbersome.");
 }
}

if ((you[0].species >= SP_OGRE && you[0].species <= SP_UNK2_DRACONIAN) || you[0].species == SP_SPRIGGAN)
{
 if (you[0].inv_type [armour_wear_2] >= ARM_LEATHER_ARMOUR && you[0].inv_type [armour_wear_2] <= ARM_PLATE_MAIL || you[0].inv_type [armour_wear_2] >= ARM_HELMET && you[0].inv_type [armour_wear_2] <= ARM_BUCKLER || you[0].inv_type [armour_wear_2] == ARM_CRYSTAL_PLATE_MAIL)
 {
  strcpy(info, "This armour doesn't fit on your body.");
  mpr(info);
  return;
 }
}

if (you[0].inv_type [armour_wear_2] == ARM_PLATE_MAIL && (you[0].skills [SK_ARMOUR] < 5 || you[0].species == SP_HALFLING || you[0].species == SP_GNOME || (you[0].species >= SP_ELF && you[0].species <= SP_SLUDGE_ELF) || you[0].species == SP_KOBOLD))
{
/* if (you[0].species == SP_NAGA || you[0].species == SP_HALFLING || you[0].species == SP_GNOME || (you[0].species >= SP_ELF && you[0].species <= SP_SLUDGE_ELF) || you[0].species == SP_KOBOLD)
 {
  mpr("I'm afraid you have the wrong body type. Try something lighter.");
  return;
 } else*/ mpr("This armour is uncomfortably cumbersome.");
}

if (you[0].inv_type [armour_wear_2] == ARM_BANDED_MAIL || you[0].inv_type [armour_wear_2] == ARM_SPLINT_MAIL && (you[0].skills [SK_ARMOUR] < 3 || you[0].species == SP_GNOME || you[0].species == SP_HALFLING))
{
/* if (you[0].species == SP_GNOME || you[0].species == SP_HALFLING)
 {
  mpr("I'm afraid you have the wrong body type. Try something lighter.");
  return;
 } else*/ mpr("You find this armour uncomfortably cumbersome.");
}

//if (you[0].inv_type [armour_wear_2] == 17 && you[0].skills [SK_ARMOUR] < 7)
// crystal plate:
if (you[0].inv_type [armour_wear_2] == ARM_CRYSTAL_PLATE_MAIL && (you[0].skills [SK_ARMOUR] < 8 || you[0].species == SP_GNOME || you[0].species == SP_HALFLING || you[0].species == SP_SPRIGGAN || (you[0].species >= SP_ELF && you[0].species <= SP_SLUDGE_ELF)))
{
/* if (you[0].species == SP_GNOME || you[0].species == SP_HALFLING || (you[0].species >= SP_ELF && you[0].species <= SP_SLUDGE_ELF))
 {
  mpr("I'm afraid you have the wrong body type. Try something lighter.");
  return;
 } else */ mpr("This armour is quite cumbersome.");
}

// storm dragon scale mail/hide
if ((you[0].inv_type [armour_wear_2] == ARM_STORM_DRAGON_HIDE || you[0].inv_type [armour_wear_2] == ARM_STORM_DRAGON_ARMOUR) && (you[0].skills [SK_ARMOUR] < 5))
{
 mpr("This armour is quite cumbersome.");
}

// golden dragon scale mail/hide
if ((you[0].inv_type [armour_wear_2] == ARM_GOLD_DRAGON_HIDE || you[0].inv_type [armour_wear_2] == ARM_GOLD_DRAGON_ARMOUR) && (you[0].skills [SK_ARMOUR] < 7))
{
 mpr("This armour is quite cumbersome.");
}



 if (you[0].inv_type [armour_wear_2] == ARM_HELMET && you[0].inv_plus2 [armour_wear_2] >= 2)
 {
/*      if (you[0].inv_plus [armour_wear_2] > 130)
                player_AC() += (you[0].inv_plus [armour_wear_2] - 150);
         else player_AC() += you[0].inv_plus [armour_wear_2] - 50;*/
 } else
  if (you[0].inv_type [armour_wear_2] == ARM_SHIELD || you[0].inv_type [armour_wear_2] == ARM_BUCKLER || you[0].inv_type [armour_wear_2] == ARM_LARGE_SHIELD)
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

/*if (you[0].inv_type [armour_wear_2] < ARM_SHIELD || you[0].inv_type [armour_wear_2] > ARM_LARGE_SHIELD)
   player_AC() += property(2, you[0].inv_type [armr], 0) * (15 + you[0].skills [SK_ARMOUR]) / 15;
      else player_AC() += property [2] [you[0].inv_type [armr]] [0];*/

//if (you[0].species == SP_NAGA && (you[0].inv_type [armr] < ARM_SHIELD || you[0].inv_type [armr] > ARM_LARGE_SHIELD))
//{
/* player_AC() -= property [2] [you[0].inv_type [armr]] [0] / 2;*/
//}

//      player_evasion() += property [you[0].inv_class [armour_wear_2]] [you[0].inv_type [armour_wear_2]] [1];



/*
if (class_armour(you[0].clas) == 1) //you[0].clas == 0 || you[0].clas == 2 || you[0].clas == 4 || you[0].clas == 6) // fighter or priest
{
 player_evasion() += property [you[0].inv_class [armour_wear_2]] [you[0].inv_type [armour_wear_2]] [1] / 2;
} else
      {
       player_evasion() += property [you[0].inv_class [armour_wear_2]] [you[0].inv_type [armour_wear_2]] [1];
      }
*/
/*
if (you[0].inv_type [armour_wear_2] < 8 || you[0].inv_type [armour_wear_2] > 14)
{
 ev_change = property [2] [you[0].inv_type [armour_wear_2]] [1];
 ev_change += you[0].skills [SK_ARMOUR] / 3;
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
                        if ((inv_name [armour_wear_2] [0] != 97 && inv_name [armour_wear_2] [0] != 101 && inv_name [armour_wear_2] [0] != 105 && inv_name [armour_wear_2] [0] != 111 && inv_name [armour_wear_2] [0] != 117 && inv_name [armour_wear_2] [0] > 96) || inv_name [armour_wear_2] [0] == 43 || inv_name [armour_wear_2] [0] == 45)
                        {
                                strcat(info, "a ");
                        }
*/
                        //item_name(you[0].inv_class [armour_wear_2], you[0].inv_type [armour_wear_2], you[0].inv_dam [armour_wear_2], you[0].inv_plus [armour_wear_2], you[0].inv_quant [armour_wear_2], you[0].inv_ident [armour_wear_2], 3, str_pass);


if (you[0].inv_type [armour_wear_2] < ARM_SHIELD || you[0].inv_type [armour_wear_2] > ARM_LARGE_SHIELD)
{
/* player_evasion() -= ev_mod(); */
 you[0].equip [EQ_BODY_ARMOUR] = armour_wear_2;
/* player_evasion() += ev_mod(); */
 if (you[0].duration [DUR_ICY_ARMOUR] != 0)
 {
  strcpy(info, "Your icy armour melts away.");
  mpr(info);
/*  player_AC() -= 6; */
  you[0].AC_ch = 1;
  you[0].duration [DUR_ICY_ARMOUR] = 0;
 }
}
 else
switch (you[0].inv_type [armour_wear_2])
{
case ARM_BUCKLER:
case ARM_LARGE_SHIELD:
case ARM_SHIELD:
/*you[0].equip [EQ_SHIELD] = armour_wear_2;
player_shield_class() += 3; // because takes account of plusses above
break;

case ARM_LARGE_SHIELD:you[0].equip [EQ_SHIELD] = armour_wear_2;
player_shield_class() += 7;
break;

case ARM_SHIELD: you[0].equip [EQ_SHIELD] = armour_wear_2;
player_shield_class() += 5;*/
you[0].equip [EQ_SHIELD] = armour_wear_2;
/*player_shield_class() = get_shield_class();*/
break;
case ARM_CLOAK: you[0].equip [EQ_CLOAK] = armour_wear_2; break;
case ARM_HELMET: you[0].equip [EQ_HELMET] = armour_wear_2; break;
case ARM_GLOVES: you[0].equip [EQ_GLOVES] = armour_wear_2; break;
case ARM_BOOTS: you[0].equip [EQ_BOOTS] = armour_wear_2; break;
//case 10: you[0].equip [5] = armour_wear_2;
}
you[0].AC_ch = 1;
you[0].evasion_ch = 1;
you[0].turnover = 1;
you[0].delay_t = property(you[0].inv_class [armour_wear_2], you[0].inv_type [armour_wear_2], 0);
if (you[0].inv_type [armour_wear_2] == ARM_SHIELD) you[0].delay_t = 0; // shield
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

 case ARM_TROLL_LEATHER_ARMOUR:
// you[0].rate_regen += 50;
// you[0].hunger_inc += 1;
 break;
/*
 case ARM_ICE_DRAGON_ARMOUR:
 player_res_fire() --;
 player_res_cold() ++;
 break;*/

                 case ARM_STORM_DRAGON_ARMOUR: // storm dragon
                 you[0].attribute [ATTR_RESIST_LIGHTNING] ++;
                 break;

/*                 case 29: // gold dragon
                 player_res_fire() ++;
                 player_res_cold() ++;
                 you[0].res_poison ++;
                 break;
*/

}

if (you[0].inv_dam [armour_wear_2] != SPARM_NORMAL)
{

//if (you[0].inv_ident [item_wield_2] == 1) you[0].inv_ident [item_wield_2] = 2;

//      Should I make these increase food consumption?

switch(you[0].inv_dam [armour_wear_2] % 30)
{

  case SPARM_RUNNING:
  if (you[0].species != SP_NAGA) strcpy(info, "You feel quick on your feet.");
   else strcpy(info, "You feel quick.");
  mpr(info);
/*  if (you[0].fast_run > 0) you[0].fast_run = 0;
  you[0].fast_run ++;*/
  break;

  case SPARM_FIRE_RESISTANCE:
  strcpy(info, "You feel resistant to fire."); // What a boring message
  mpr(info);
/*  player_res_fire() ++;*/
  break;

  case SPARM_COLD_RESISTANCE:
  strcpy(info, "You feel resistant to cold.");
  mpr(info);
/*  player_res_cold() ++;*/
  break;

  case SPARM_POISON_RESISTANCE:
  strcpy(info, "You feel healthy.");
  mpr(info);
/*  you[0].res_poison ++;*/
  break;

  case SPARM_SEE_INVISIBLE:
  strcpy(info, "You feel perceptive.");
  mpr(info);
/*  if (you[0].see_invis > 0) you[0].see_invis = 0;
  you[0].see_invis --;*/
  break;

  case SPARM_DARKNESS:
  if (you[0].invis == 0)
  {
   mpr("You become transparent for a moment.");
  }
  break;

 case SPARM_STRENGTH:
 strcpy(info, "You feel stronger.");
 mpr(info);
 you[0].strength += 3;
 you[0].max_strength += 3;
 you[0].strength_ch = 1;
 break;

 case SPARM_DEXTERITY:
 strcpy(info, "You feel dextrous.");
 mpr(info);
 you[0].dex += 3;
 you[0].max_dex += 3;
 you[0].dex_ch = 1;
 break;

 case SPARM_INTELLIGENCE:
 strcpy(info, "You feel intelligent."); // inspired messages, aren't they?
 mpr(info);
 you[0].intel += 3;
 you[0].max_intel += 3;
 you[0].intel_ch = 1;
 break;

 case SPARM_PONDEROUSNESS:
 strcpy(info, "You feel rather ponderous.");
 you[0].speed += 2;
/* player_evasion() -= 1; */
 you[0].evasion_ch = 1;
 break;

        case SPARM_LEVITATION: // levitation
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

 case SPARM_MAGIC_RESISTANCE:
 strcpy(info, "You feel resistant to magic.");
 mpr(info);
/* you[0].res_magic += 40;*/
 break;

 case SPARM_PROTECTION:
 strcpy(info, "You feel protected.");
 mpr(info);
/* player_AC() += 3;*/
 break;

 case SPARM_STEALTH:
 strcpy(info, "You feel stealthy.");
 mpr(info);
 break;

  case SPARM_RESISTANCE:
  strcpy(info, "You feel resistant to extremes of temperature.");
  mpr(info);
/*  player_res_cold() ++;
  player_res_fire() ++;*/
  break;

  case SPARM_POSITIVE_ENERGY:
  strcpy(info, "Your life-force is being protected.");
  mpr(info);
/*  player_prot_life() ++;*/
  break;

  case SPARM_ARCHMAGI: // archmagi
  if (you[0].skills [SK_SPELLCASTING] == 0)
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

if (you[0].inv_dam [armour_wear_2] % 30 >= 25)
{
 use_randart(armour_wear_2);
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

unsigned char nthing = 0;

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

if (keyin == '?' || keyin == '*')
{
        if (keyin == '?') nthing = get_invent(OBJ_ARMOUR);
//invent(you[0].inv_plus2, 2, you[0].inv_quant, you[0].inv_dam, you[0].inv_class, you[0].inv_type, you[0].inv_plus, you[0].inv_ident, you[0].equip [0], you[0].equip [6], you[0].equip [5], you[0].equip [2], you[0].equip [1], you[0].equip [3], you[0].equip [4], you[0].ring);
        if (keyin == '*') nthing = get_invent(-1);
//invent(you[0].inv_plus2, -1, you[0].inv_quant, you[0].inv_dam, you[0].inv_class, you[0].inv_type, you[0].inv_plus, you[0].inv_ident, you[0].equip [0], you[0].equip [6], you[0].equip [5], you[0].equip [2], you[0].equip [1], you[0].equip [3], you[0].equip [4], you[0].ring);
        if ((nthing >= 65 && nthing <= 90) || (nthing >= 97 && nthing <= 122))
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

if (armour_wear_1 < 65 || (armour_wear_1 > 90 && armour_wear_1 < 97) || armour_wear_1 > 122)
{
        strcpy(info, "You don't have any such object.");
        mpr(info);
        return;
}



/*
if (you[0].equip [EQ_BODY_ARMOUR] == -1) // or helmet etc
{
        strcpy(info, "You aren't wearing any you[0].equip.");
        mpr(info);
        return;
}
*/


int armour_wear_2 = conv_lett(armour_wear_1); //you[0].equip [6]; // may as well keep on using armour_wear_2

if (you[0].inv_class [armour_wear_2] != OBJ_ARMOUR)
{
        strcpy(info, "You aren't wearing that!");
        mpr(info);
        return;
}

if (you[0].inv_plus [armour_wear_2] > 130 && (armour_wear_2 == you[0].equip [EQ_BODY_ARMOUR] || armour_wear_2 == you[0].equip [EQ_SHIELD] || armour_wear_2 == you[0].equip [EQ_HELMET] || armour_wear_2 == you[0].equip [EQ_GLOVES] || armour_wear_2 == you[0].equip [EQ_CLOAK] || armour_wear_2 == you[0].equip [EQ_BOOTS]))
{
        strcpy(info, "That piece of armour seems to be stuck to your body!");
        mpr(info);
        return;
}

if (you[0].inv_type [armour_wear_2] < ARM_SHIELD || you[0].inv_type [armour_wear_2] > ARM_LARGE_SHIELD)
{
        if (armour_wear_2 != you[0].equip [EQ_BODY_ARMOUR])
        {
                strcpy(info, "You aren't wearing that!");
                mpr(info);
                return;
        }

        if (you[0].equip [EQ_CLOAK] != -1)
        {
        strcpy(info, "You'll have to take your cloak off first.");
        mpr(info);
        return;
        }

//      itoa(you[0].equip [1], st_prn, 10);
//      strcpy(info, st_prn);
//      mpr(info);
/*        player_evasion() -= ev_mod();*/
        you[0].equip [EQ_BODY_ARMOUR] = -1;
/*        player_evasion() += ev_mod();*/
} else
switch(you[0].inv_type [armour_wear_2])
{

case ARM_BUCKLER:
case ARM_LARGE_SHIELD:
case ARM_SHIELD:
if (armour_wear_2 != you[0].equip [EQ_SHIELD])
{
        strcpy(info, "You aren't wearing that!");
        mpr(info);
        return;
} else you[0].equip [EQ_SHIELD] = -1; break;


case ARM_CLOAK:
if (armour_wear_2 != you[0].equip [EQ_CLOAK])
{
        strcpy(info, "You aren't wearing that!");
        mpr(info);
        return;
} else you[0].equip [EQ_CLOAK] = -1; break;


//you[0].equip [1] = armour_wear_2; break;
case ARM_HELMET:
if (armour_wear_2 != you[0].equip [EQ_HELMET])
{
        strcpy(info, "You aren't wearing that!");
        mpr(info);
        return;
} else you[0].equip [EQ_HELMET] = -1; break;

//you[0].equip [2] = armour_wear_2; break;

case ARM_GLOVES:
if (armour_wear_2 != you[0].equip [EQ_GLOVES])
{
        strcpy(info, "You aren't wearing that!");
        mpr(info);
        return;
} else you[0].equip [EQ_GLOVES] = -1; break;



//you[0].equip [3] = armour_wear_2; break;
case ARM_BOOTS:
if (armour_wear_2 != you[0].equip [EQ_BOOTS])
{
        strcpy(info, "You aren't wearing that!");
        mpr(info);
        return;
} else you[0].equip [EQ_BOOTS] = -1; break;


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
unsigned char nthing = 0;
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

if (keyin == '?' || keyin == '*')
{
        if (keyin == '?')
        {
                nthing = get_invent(OBJ_MISSILES);
        }
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

int throw_1 = (int) keyin;

if (throw_1 < 65 || (throw_1 > 90 && throw_1 < 97) || throw_1 > 122)
{
        strcpy(info, "You don't have any such object.");
        mpr(info);
        return;
}

int throw_2 = conv_lett(throw_1);

if (throw_2 == you[0].equip [EQ_LEFT_RING] || throw_2 == you[0].equip [EQ_RIGHT_RING] || throw_2 == you[0].equip [EQ_AMULET] || throw_2 == you[0].equip [EQ_BODY_ARMOUR] || throw_2 == you[0].equip [EQ_SHIELD] || throw_2 == you[0].equip [EQ_HELMET] || throw_2 == you[0].equip [EQ_CLOAK] || throw_2 == you[0].equip [EQ_BOOTS] || throw_2 == you[0].equip [EQ_GLOVES])
{
        strcpy(info, "You are wearing that object!");
        mpr(info);
        return;
}

if (throw_2 == you[0].equip [EQ_WEAPON] && you[0].inv_plus [you[0].equip [EQ_WEAPON]] > 80)
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
int throw_2 = 0;

if (you[0].berserker != 0)
{
 strcpy(info, "You are too berserk!");
 mpr(info);
 return;
}

int type_wanted = 3;

if (you[0].equip [EQ_WEAPON] == -1) goto find;

if (you[0].inv_class [you[0].equip [EQ_WEAPON]] != OBJ_WEAPONS || you[0].inv_type [you[0].equip [EQ_WEAPON]] < WPN_SLING || you[0].inv_type [you[0].equip [EQ_WEAPON]] > WPN_CROSSBOW)
{
        type_wanted = 3;
} else type_wanted = you[0].inv_type [you[0].equip [EQ_WEAPON]] - 13;

find: for (i = 0; i < 53; i ++)
{
  if (i == 52)
                {
   strcpy(info, "You have nothing suitable.");
                        mpr(info);
                        return;
                }
  if (you[0].inv_quant [i] == 0) continue;
                if (you[0].inv_class [i] == OBJ_MISSILES && type_wanted == you[0].inv_type [i])
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

char shoot_skill = 0;

strcpy(info, "Which direction? (* to target)");
mpr(info);

 if (you[0].prev_targ != MHITNOT && you[0].prev_targ < MNST)
  if (mons_near(you[0].prev_targ) == 1 && (menv [you[0].prev_targ].m_ench [2] != 6 || player_see_invis() != 0))
  {
        strcpy(info, "You are currently targetting ");
        strcat(info, monam (menv [you[0].prev_targ].m_sec, menv [you[0].prev_targ].m_class, menv [you[0].prev_targ].m_ench [2], 1));
        strcat(info, " (p to target).");
        mpr(info);
  } else mpr("You have no current target.");

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

beam[0].range = 5; /* provisional until mass variable used */

beam[0].flavour = 0;

switch (you[0].inv_class [throw_2])
{
        case OBJ_WEAPONS: beam[0].type = 41;
 beam[0].range = 9;
 break;
        case OBJ_MISSILES: beam[0].type = 41;
 beam[0].range = 9;
 break;
        case OBJ_ARMOUR: beam[0].type = 91; break;
        case OBJ_WANDS: beam[0].type = 47; break;
        case OBJ_FOOD: beam[0].type = 37; break;
        case 5: beam[0].type = 43; break;
        case OBJ_SCROLLS: beam[0].type = 63; break;
        case OBJ_JEWELLERY: beam[0].type = 61; break;
        case OBJ_POTIONS: beam[0].type = 33; break;
        case 9: beam[0].type = 42; break;
        case OBJ_BOOKS: beam[0].type = 40; break;
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

beam[0].hit += random2(you[0].skills [SK_THROWING] + 1);
beam[0].hit += slaying_bonus(0);
beam[0].damage += random2(you[0].skills [SK_THROWING] + 1);
beam[0].damage += slaying_bonus(1);

if (you[0].inv_class [throw_2] == OBJ_MISSILES && you[0].inv_type [throw_2] == MI_DART) // darts
{
 if (you[0].equip [EQ_WEAPON] == -1 || you[0].inv_class [you[0].equip [EQ_WEAPON]] != OBJ_WEAPONS || you[0].inv_type [you[0].equip [EQ_WEAPON]] != WPN_HAND_CROSSBOW)
 {
   exercise(SK_DARTS , 1 + random2(2) + random2(2) + random2(2));
   beam[0].hit += 2;
   beam[0].hit += random2(you[0].skills [SK_DARTS] + 1) + random2(you[0].skills [SK_DARTS] + 1);
 }
}

if (you[0].inv_class [throw_2] == OBJ_WEAPONS || you[0].inv_class [throw_2] == OBJ_MISSILES)
{
        beam[0].damage = property(you[0].inv_class [throw_2], you[0].inv_type [throw_2], 0);

/*      Missile devices eg bows */
        if (you[0].inv_class [you[0].equip [EQ_WEAPON]] == OBJ_WEAPONS && you[0].inv_type [you[0].equip [EQ_WEAPON]] == you[0].inv_type [throw_2] + 13 && you[0].inv_type [you[0].equip [EQ_WEAPON]] <= WPN_HAND_CROSSBOW)
        {
                beam[0].damage = property(you[0].inv_class [throw_2], you[0].inv_type [throw_2], 1);

  if (you[0].inv_type [you[0].equip [EQ_WEAPON]] == WPN_CROSSBOW)
  {
   you[0].time_taken *= 12;
   you[0].time_taken /= 10;
  }

                int hoggl = you[0].inv_plus2 [you[0].equip [EQ_WEAPON]] - 50;

                if (you[0].inv_plus2 [you[0].equip [EQ_WEAPON]] > 80) hoggl -= 100;

                beam[0].damage += hoggl;
  beam[0].range += 5;

                switch(you[0].inv_type [you[0].equip [EQ_WEAPON]])
                {
                 case WPN_SLING: /* sling */
                 exercise(SK_SLINGS, 1 + random2(2) + random2(2) + random2(2));
                 beam[0].hit += random2(you[0].skills [SK_SLINGS] + 1) + random2(random2(you[0].skills [SK_SLINGS] + 1));
                 beam[0].damage += random2(you[0].skills [SK_SLINGS] + 1);
                 shoot_skill = you[0].skills [SK_SLINGS];
                 break;
                 case WPN_BOW: /* bow */
                 exercise(SK_BOWS, 1 + random2(2) + random2(2) + random2(2));
                 beam[0].hit += random2(you[0].skills [SK_BOWS] + 1) + random2(you[0].skills [SK_BOWS] + 1);
                 beam[0].damage += random2(you[0].skills [SK_BOWS] + 1) + random2(random2(you[0].skills [SK_BOWS] + 1));
                 shoot_skill = you[0].skills [SK_BOWS];
                 break;
                 case WPN_CROSSBOW: /* crossbow */
                 case WPN_HAND_CROSSBOW: /* hand crossbow */
                 exercise(SK_CROSSBOWS, 1 + random2(2) + random2(2) + random2(2));
                 beam[0].hit += random2(you[0].skills [SK_CROSSBOWS] + 1);
                 beam[0].damage += random2(you[0].skills [SK_CROSSBOWS] + 1);
                 shoot_skill = you[0].skills [SK_THROWING];
                 break;
                }
                exercise(SK_THROWING, 1 + random2(2));

                int ghoggl = you[0].inv_plus [you[0].equip [EQ_WEAPON]] - 50;

                if (you[0].inv_plus [you[0].equip [EQ_WEAPON]] > 80) ghoggl -= 100;

                beam[0].hit += ghoggl;

                beam[0].range += 3;

                if (you[0].inv_dam [you[0].equip [EQ_WEAPON]] / 30 == you[0].inv_dam [throw_2] / 30)
                {
                 beam[0].hit ++;
                 beam[0].damage ++;
                 if (you[0].inv_dam [you[0].equip [EQ_WEAPON]] / 30 == DWPN_ELVEN && you[0].species >= SP_ELF && you[0].species <= SP_SLUDGE_ELF) beam[0].hit ++;
                }


                if ((you[0].inv_dam [you[0].equip [EQ_WEAPON]] % 30 == SPWPN_FLAME || you[0].inv_dam [throw_2] % 30 == SPWPN_FLAMING || (you[0].inv_dam [you[0].equip [EQ_WEAPON]] % 30 >= 25 && randart_wpn_properties(you[0].inv_class [you[0].equip [EQ_WEAPON]], you[0].inv_type [you[0].equip [EQ_WEAPON]], you[0].inv_dam [you[0].equip [EQ_WEAPON]], you[0].inv_plus [you[0].equip [EQ_WEAPON]], you[0].inv_plus2 [you[0].equip [EQ_WEAPON]], 0, RAP_BRAND) == SPWPN_FLAME)) && you[0].inv_dam [throw_2] % 30 != SPWPN_FREEZING && you[0].inv_dam [you[0].equip [EQ_WEAPON]] % 30 != SPWPN_FROST)
                {
                 beam[0].damage += 1 + random2(5);
                 beam[0].flavour = 2;
                 strcpy(beam[0].beam_name, "bolt of ");
                 if (you[0].inv_dam [throw_2] % 30 == 3 || you[0].inv_dam [throw_2] % 30 == 4)
                        strcat(beam[0].beam_name, "poison ");
                 strcat(beam[0].beam_name, "flame");
                 beam[0].colour = RED;
                 beam[0].type = 35;
                 beam[0].thing_thrown = 3;
                 you[0].inv_ident [throw_2] = 2;
                }
//                if (you[0].inv_dam [you[0].equip [0]] % 30 == 12)
                if ((you[0].inv_dam [you[0].equip [EQ_WEAPON]] % 30 == SPWPN_FROST || you[0].inv_dam [throw_2] % 30 == SPWPN_FREEZING || (you[0].inv_dam [you[0].equip [EQ_WEAPON]] % 30 >= 25 && randart_wpn_properties(you[0].inv_class [you[0].equip [EQ_WEAPON]], you[0].inv_type [you[0].equip [EQ_WEAPON]], you[0].inv_dam [you[0].equip [EQ_WEAPON]], you[0].inv_plus [you[0].equip [EQ_WEAPON]], you[0].inv_plus2 [you[0].equip [EQ_WEAPON]], 0, RAP_BRAND) == SPWPN_FROST)) && you[0].inv_dam [throw_2] % 30 != SPWPN_FLAMING && you[0].inv_dam [you[0].equip [EQ_WEAPON]] % 30 != SPWPN_FLAME)
                {
                 beam[0].damage += 1 + random2(5);
                 beam[0].flavour = 3;
                 strcpy(beam[0].beam_name, "bolt of ");
                 if (you[0].inv_dam [throw_2] % 30 == 3 || you[0].inv_dam [throw_2] % 30 == 4)
                        strcat(beam[0].beam_name, "poison ");
                 strcat(beam[0].beam_name, "frost");
                 beam[0].colour = WHITE;
                 beam[0].type = 35;
                 beam[0].thing_thrown = 3;
                 you[0].inv_ident [throw_2] = 2;
                } /* the chief advantage here is the extra damage this does against susceptible creatures */

/* Note: weapons & ammo of eg fire are not cumulative
ammo of fire and weapons of frost don't work together, and vice versa */

                if (you[0].inv_plus [you[0].equip [EQ_WEAPON]] > 80)
                {
                        beam[0].damage -= 100;
                        beam[0].hit -= 100;
                }
                shoot = 1;
                if (you[0].inv_ident [you[0].equip [EQ_WEAPON]] < 3 && random2(100) < shoot_skill)
                {
                 you[0].inv_ident [you[0].equip [EQ_WEAPON]] = 3;
                 strcpy(info, "You are wielding ");
                 in_name(you[0].equip [EQ_WEAPON], 3, str_pass);
                 strcat(info, str_pass);
                 strcat(info, ".");
                 mpr(info);
                 more();
                 wield_change = 1;
                }
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



        if (you[0].inv_class [throw_2] == OBJ_WEAPONS)
        {
                if (you[0].inv_type [throw_2] == WPN_DAGGER || you[0].inv_type [throw_2] == WPN_HAND_AXE || you[0].inv_type [throw_2] == WPN_SPEAR)
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

 if (you[0].equip [EQ_WEAPON] == throw_2) unwield_item(throw_2);

missile(beam, throw_2);
you[0].inv_quant [throw_2] --;
if (you[0].inv_quant [throw_2] == 0)
{
        you[0].inv_no --;
        if (you[0].equip [EQ_WEAPON] == throw_2)
        {
                you[0].equip [EQ_WEAPON] = -1;
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

unsigned char nthing = 0;

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

if (keyin == '?' || keyin == '*')
{
        if (keyin == '?') nthing = get_invent(OBJ_JEWELLERY);
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
int ring_wear_1 = (int) keyin;

if (ring_wear_1 < 65 || (ring_wear_1 > 90 && ring_wear_1 < 97) || ring_wear_1 > 122)
{
        strcpy(info, "You don't have any such object.");
        mpr(info);
        return;
}

int ring_wear_2 = conv_lett(ring_wear_1);

if (ring_wear_2 == you[0].equip [EQ_LEFT_RING] || ring_wear_2 == you[0].equip [EQ_RIGHT_RING] || ring_wear_2 == you[0].equip [EQ_AMULET])
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

if (ring_wear_2 == you[0].equip [EQ_WEAPON])
{
        strcpy(info, "You are wielding that object.");
        mpr(info);
        return;
}

if (you[0].inv_class [ring_wear_2] != OBJ_JEWELLERY)
{
        strcpy(info, "I think there's some kind of mistake here.");
        mpr(info);
        return;
}


if (you[0].inv_type [ring_wear_2] < AMU_RAGE)
{
 if (you[0].equip [EQ_GLOVES] != -1 && you[0].inv_plus [you[0].equip [EQ_GLOVES]] > 80)
 {
        strcpy(info, "You can't take your gloves off to wear a ring!");
        mpr(info);
        return;
 }

 if (you[0].inv_class [ring_wear_2] == OBJ_JEWELLERY && you[0].equip [EQ_LEFT_RING] != -1 && you[0].equip [EQ_RIGHT_RING] != -1) // and you are trying to wear body you[0].equip.
 {
        strcpy(info, "You are already wearing a ring on each hand.");
        mpr(info);
        return;
 }
} else
if (you[0].equip [EQ_AMULET] != -1)
{
        strcpy(info, "You are already wearing an amulet.");
        if (random2(20))
        {
         strcat(info, " And I must say it looks quite fetching.");
        }
        mpr(info);
        return;
}

int hand_used = 0;

if (you[0].equip [EQ_LEFT_RING] != -1) hand_used = 1;
if (you[0].equip [EQ_RIGHT_RING] != -1) hand_used = 0;

if (you[0].inv_type [ring_wear_2] >= AMU_RAGE)
 hand_used = 2;
  else
if (you[0].equip [EQ_LEFT_RING] == -1 && you[0].equip [EQ_RIGHT_RING] == -1)
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

        case RING_REGENERATION: /* you[0].ring of regeneration */
/*      you[0].rate_regen += 100; */
        you[0].hunger_inc += 2;
        if (you[0].hp < you[0].hp_max) set_id(you[0].inv_class [ring_wear_2], you[0].inv_type [ring_wear_2], 1); // this is a bit dodgy
        break;

        case RING_PROTECTION: /* protection */
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

        case RING_PROTECTION_FROM_FIRE: /* fire resistance */
/*      player_res_fire() += 1;*/
//      you[0].hunger_inc += 1;
        break;

        case RING_POISON_RESISTANCE: // you[0].poison resistance
/*      you[0].res_poison += 1; */
///     you[0].hunger_inc += 1;
        break;

        case RING_PROTECTION_FROM_COLD: // cold resistance
//      you[0].hunger_inc += 1;
        break;

        case RING_STRENGTH: // increase you[0].strength
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

        case RING_SLAYING: // slaying
/*      you[0].f_abil += 30; */
//      you[0].hunger_inc += 1;
        break;

        case RING_SEE_INVISIBLE: // see invisible
/*      you[0].see_invis --; */
//      you[0].hunger_inc += 1;
        break;

        case RING_INVISIBILITY: // invisibility
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

        case RING_HUNGER: // you[0].hunger
        you[0].hunger_inc += 4;
        break;

        case RING_TELEPORTATION: // teleportation
//      you[0].hunger_inc += 1;
/*      player_teleport() += 1;*/
        break;

        case RING_EVASION: // player_evasion()
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

        case RING_SUSTAIN_ABILITIES:
// you[0].hunger_inc += 1;
/* you[0].sust_abil++;*/
 break;

 case RING_SUSTENANCE:
 you[0].hunger_inc -= 2;
 break;

 case RING_DEXTERITY: // increase you[0].dex
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

 case RING_INTELLIGENCE: // increase you[0].intel
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

 case RING_WIZARDRY: // wizardry
/* you[0].mag_abil += 3;*/
// you[0].hunger_inc += 1;
 break;

 case RING_MAGICAL_POWER: // magic
 you[0].ep_max += 9;
 you[0].base_ep2 += 9;
// you[0].hunger_inc ++;
 you[0].ep_ch = 1;
/* you[0].magic_battery ++; */
 calc_ep();
 set_id(you[0].inv_class [ring_wear_2], you[0].inv_type [ring_wear_2], 1);
 break;

        case RING_LEVITATION: // levitation
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
    set_id(you[0].inv_class [ring_wear_2], you[0].inv_type [ring_wear_2], 1);
        break;

 case RING_LIFE_PROTECTION: // player_prot_life()
//      you[0].hunger_inc ++;
        break;

 case RING_PROTECTION_FROM_MAGIC: // magic resistance
// you[0].hunger_inc += 1;
 break;

 case RING_FIRE: // fire
/* player_res_fire() += 1;
 you[0].spec_fire += 1;
 player_res_cold() -= 1;*/
// you[0].hunger_inc += 1;
 break;

 case RING_ICE: // ice
/* player_res_fire() -= 1;
 you[0].spec_cold += 1;
 player_res_cold() += 1;*/
// you[0].hunger_inc += 1;
 break;

 case RING_TELEPORT_CONTROL: // teleport control
 you[0].attribute [ATTR_CONTROL_TELEPORT] ++;
// you[0].hunger_inc += 1;
 break;


/* all of the amulets so far existing are not needed here, but have their
   presence queried whenever relevant through wearing_amulet in fn2.cc */
}
you[0].turnover = 1;

if (you[0].inv_dam [ring_wear_2] == 200 || you[0].inv_dam [ring_wear_2] == 201)
{
 use_randart(ring_wear_2);
}

} // end of puton_ring()





void remove_ring(void)
{

unsigned char nthing = 0;
int hand_used = 10;
unsigned char keyin;
int ring_wear_2;

if (you[0].equip [EQ_LEFT_RING] == -1 && you[0].equip [EQ_RIGHT_RING] == -1 && you[0].equip [EQ_AMULET] == -1)
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

if (you[0].equip [EQ_GLOVES] != -1 && you[0].inv_plus [you[0].equip [EQ_GLOVES]] > 80 && you[0].equip [EQ_AMULET] == -1)
{
        strcpy(info, "You can't take your gloves off to remove any rings!");
        mpr(info);
        return;
}


if (you[0].equip [EQ_LEFT_RING] != -1 && you[0].equip [EQ_RIGHT_RING] == -1 && you[0].equip [EQ_AMULET] == -1) hand_used = 0;
if (you[0].equip [EQ_LEFT_RING] == -1 && you[0].equip [EQ_RIGHT_RING] != -1 && you[0].equip [EQ_AMULET] == -1) hand_used = 1;
if (you[0].equip [EQ_LEFT_RING] == -1 && you[0].equip [EQ_RIGHT_RING] == -1 && you[0].equip [EQ_AMULET] != -1) hand_used = 2;

if (hand_used == 10)
{
        query : strcpy(info, "Remove from where (l or r hand, or n for neck)?");
        mpr(info);

        keyin = get_ch();

       if (keyin == '?' || keyin == '*')
       {
        if (keyin == '?') nthing = get_invent(OBJ_JEWELLERY);
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

if (you[0].equip [EQ_GLOVES] != -1 && you[0].inv_plus [you[0].equip [EQ_GLOVES]] > 80 && hand_used < 2)
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

        case RING_REGENERATION: // regen
        you[0].hunger_inc -= 2;
        break;

        case RING_PROTECTION: // prot
        you[0].AC_ch = 1;
//      you[0].hunger_inc -= 1;
        break;

        case RING_PROTECTION_FROM_FIRE: // fire resistance
//      you[0].hunger_inc -= 1;
        break;

        case RING_POISON_RESISTANCE: // you[0].poison resistance
//      you[0].hunger_inc -= 1;
        break;

        case RING_PROTECTION_FROM_COLD: // cold resistance
//      you[0].hunger_inc -= 1;
        break;

        case RING_STRENGTH: // increase you[0].strength
        you[0].strength -= (you[0].inv_plus [ring_wear_2] % 100);
        you[0].max_strength -= (you[0].inv_plus [ring_wear_2] % 100);
        you[0].strength += 50;
        you[0].max_strength += 50;
        you[0].strength_ch = 1;
//      you[0].hunger_inc -= 1;
        burden_change();
        break;

        case RING_SLAYING: // slaying
//      you[0].hunger_inc -= 1;
        break;

        case RING_SEE_INVISIBLE: // see invisible
//      you[0].hunger_inc -= 1;
        break;

        case RING_INVISIBILITY: // invisibility
//      you[0].invis ++;
//      if (you[0].invis == 0) you[0].invis = 1;
//      you[0].hunger_inc -= 4;
        if (you[0].invis != 0) you[0].invis = 1;
        break;

        case RING_HUNGER: // you[0].hunger
        you[0].hunger_inc -= 4;
        break;

        case RING_TELEPORTATION: // teleportation
//      you[0].hunger_inc -= 1;
        break;

        case 11: // player_evasion()
//      you[0].hunger_inc -= 1;
        you[0].evasion_ch = 1;
        break;

 case RING_SUSTAIN_ABILITIES: // you[0].sust_abil
// you[0].hunger_inc -= 1;
 break;

 case RING_SUSTENANCE: // sustenance
 you[0].hunger_inc += 2;
 break;

 case RING_DEXTERITY: // increase you[0].dex
        you[0].dex -= (you[0].inv_plus [ring_wear_2] % 100) - 50;
        you[0].max_dex -= (you[0].inv_plus [ring_wear_2] % 100) - 50;
        you[0].dex_ch = 1;
//      you[0].hunger_inc -= 1;
        break;

 case RING_INTELLIGENCE: // increase you[0].intel
        you[0].intel -= (you[0].inv_plus [ring_wear_2] % 100) - 50;
        you[0].max_intel -= (you[0].inv_plus [ring_wear_2] % 100) - 50;
        you[0].intel_ch = 1;
//      you[0].hunger_inc -= 1;
        break;

 case RING_WIZARDRY: // wizardry
// you[0].hunger_inc -= 1;
 break;

 case RING_MAGICAL_POWER: // magic
 you[0].ep_max -= 9;
 you[0].base_ep2 -= 9;
 if (you[0].ep > you[0].ep_max) you[0].ep = you[0].ep_max;
// you[0].hunger_inc --;
 you[0].ep_ch = 1;
/* you[0].magic_battery --; */
 calc_ep();
 break;

        case RING_LEVITATION: // levitation
/*      you[0].lev ++;
        if (you[0].lev == 0) you[0].lev = 1;
        you[0].hunger_inc -= 1;*/
        if (you[0].lev != 0) you[0].lev = 1;
        break;

 case RING_LIFE_PROTECTION: // player_prot_life()
//      you[0].hunger_inc --;
        break;

 case RING_PROTECTION_FROM_MAGIC: // magic resistance
// you[0].hunger_inc -= 1;
 break;

 case RING_FIRE: // fire
/* player_res_fire() -= 1;
 you[0].spec_fire -= 1;
 player_res_cold() += 1;*/
// you[0].hunger_inc -= 1;
 break;

 case RING_ICE: // ice
/* player_res_fire() += 1;
 you[0].spec_cold -= 1;
 player_res_cold() -= 1;*/
// you[0].hunger_inc -= 1;
 break;

 case RING_TELEPORT_CONTROL: // teleport control
 you[0].attribute [ATTR_CONTROL_TELEPORT] --;
// you[0].hunger_inc -= 1;
 break;


} // end of switch

if (you[0].inv_dam [ring_wear_2] == 200 || you[0].inv_dam [ring_wear_2] == 201)
{
 unuse_randart(ring_wear_2);
}



you[0].equip [hand_used + 7] = -1;


you[0].turnover = 1;


} // end of remove_ring






void zap_wand(void)
{
int zap_device_1;
int zap_device_2;
unsigned char nthing = 0;

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

if (keyin == '?' || keyin == '*')
{
        if (keyin == '?') nthing = get_invent(OBJ_WANDS);
        if (keyin == '*') nthing = get_invent(-1);
        if ((nthing >= 65 && nthing <= 90) || (nthing >= 97 && nthing <= 122))
        {
                keyin = nthing;
        } else
        {
/*   mesclr(); */
   goto query;

        }
}

zap_device_1 = (int) keyin;

if (zap_device_1 < 65 || (zap_device_1 > 90 && zap_device_1 < 97) || zap_device_1 > 122)
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

if (you[0].inv_class [zap_device_2] != OBJ_WANDS || you[0].inv_plus [zap_device_2] <= 0)
{
        strcpy(info, "Nothing appears to happen.");
        mpr(info);
        you[0].turnover = 1;
        return;
}

strcpy(info, "Which direction? (* to target)");
mpr(info);
 if (you[0].prev_targ != MHITNOT && you[0].prev_targ < MNST)
  if (mons_near(you[0].prev_targ) == 1 && (menv [you[0].prev_targ].m_ench [2] != 6 || player_see_invis() != 0))
  {
        strcpy(info, "You are currently targetting ");
        strcat(info, monam (menv [you[0].prev_targ].m_sec, menv [you[0].prev_targ].m_class, menv [you[0].prev_targ].m_ench [2], 1));
        strcat(info, " (p to target).");
        mpr(info);
  } else mpr("You have no current target.");

direction(1, zap_wand);

if (zap_wand[0].nothing == -1) return;

if (you[0].conf != 0)
{
 zap_wand[0].move_x = random2(13) - 6;
 zap_wand[0].move_y = random2(13) - 6;
}

beam[0].source_x = you[0].x_pos; beam[0].source_y = you[0].y_pos;

char type_zapped = you[0].inv_type [zap_device_2];
if (type_zapped == WAND_ENSLAVEMENT) type_zapped = 21; /* enslavement */
if (type_zapped == WAND_DRAINING) type_zapped = 17; /* draining */
if (type_zapped == WAND_DISINTEGRATION) type_zapped = 46; /* disintegration */
if (type_zapped == WAND_RANDOM_EFFECTS) /* random effects */
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

if (beam[0].wand_id == 1 || you[0].inv_type [zap_device_2] == WAND_FIREBALL)
{
        set_id(you[0].inv_class [zap_device_2], you[0].inv_type [zap_device_2], 1);
}

you[0].inv_plus [zap_device_2] --;

if (get_id(you[0].inv_class [zap_device_2], you[0].inv_type [zap_device_2]) == 1 && (you[0].inv_ident [zap_device_2] > 1 || you[0].skills [SK_ENCHANTMENTS] > random2(15) + 5))
{
 if (you[0].inv_ident [zap_device_2] <= 1) mpr("Your skill with enchantments lets you calculate the power of this device...");
 strcpy(info, "This wand has ");
 itoa(you[0].inv_plus [zap_device_2], st_prn, 10);
 strcat(info, st_prn);
 strcat(info, " charge");
 if (you[0].inv_plus [zap_device_2] == 1) strcat(info, " left.");
  else strcat(info, "s left.");
 mpr(info);
 you[0].inv_ident [zap_device_2] = 3;
}

you[0].turnover = 1;

alert();

} /* end of void zap_wand(); */



void eat_food(void)
{
int food_eat_1;
int food_eat_2;

unsigned char nthing = 0;

if (you[0].is_undead == 2)
{
 strcpy(info, "You can't eat.");
 mpr(info);
 return;
}

if (you[0].hunger >= 11000) // || (you[0].hunger >= 7000 && you[0].species == 35))
{
 mpr("You're too full to eat anything.");
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

if (keyin == '?' || keyin == '*')
{
        if (keyin == '?') nthing = get_invent(OBJ_FOOD);
        if (keyin == '*') nthing = get_invent(-1);
        if ((nthing >= 65 && nthing <= 90) || (nthing >= 97 && nthing <= 122))
        {
                keyin = nthing;
        } else
        {
/*       mesclr(); */
       goto query;

        }

}

food_eat_1 = (int) keyin;

if (food_eat_1 < 65 || (food_eat_1 > 90 && food_eat_1 < 97) || food_eat_1 > 122)
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


if (you[0].inv_class [food_eat_2] != OBJ_FOOD)
{
        strcpy(info, "You can't eat that!");
        mpr(info);
        return;
}

if (you[0].species == SP_GHOUL || you[0].species == SP_KOBOLD || you[0].mutation [MUT_CARNIVOROUS] == 3) // kobold etc
{
 if ((you[0].inv_type [food_eat_2] >= FOOD_BREAD_RATION && you[0].inv_type [food_eat_2] <= FOOD_CHOKO) || (you[0].inv_type [food_eat_2] >= FOOD_SNOZZCUMBER && you[0].inv_type [food_eat_2] <= FOOD_LYCHEE))
 {
  strcpy(info, "Sorry, you're a carnivore.");
  mpr(info);
  return;
 }
}

    if (you[0].mutation [MUT_HERBIVOROUS] > 1 && you[0].inv_type [food_eat_2] == FOOD_CHUNK)
    {
     mpr("You can't eat raw meat!");
     return;
    }


if (you[0].inv_type [food_eat_2] == FOOD_CHUNK && you[0].hung_state > 2 && wearing_amulet(AMU_THE_GOURMAND) == 0 && you[0].species != SP_KOBOLD && you[0].species != SP_OGRE && you[0].species != SP_TROLL && you[0].species != SP_GHOUL && you[0].mutation [MUT_CARNIVOROUS] == 0)
{
 strcpy(info, "You aren't quite hungry enough to eat that!");
 mpr(info);
 return;
}

you[0].inv_quant [food_eat_2] --;

if (you[0].inv_quant [food_eat_2] == 0)
{
        you[0].inv_no --;
        if (you[0].equip [EQ_WEAPON] == food_eat_2)
        {
                you[0].equip [EQ_WEAPON] = -1;
                strcpy(info, "You are now empty handed.");
                mpr(info);
                wield_change = 1;
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
unsigned char nthing = 0;
unsigned char keyin;

if (you[0].is_undead == 2)
{
 strcpy(info, "You can't drink.");
 mpr(info);
 return;
}

if (grd [you[0].x_pos] [you[0].y_pos] >= 200 && grd [you[0].x_pos] [you[0].y_pos] <= 209 && grd [you[0].x_pos] [you[0].y_pos] % 2 == 0)
{
 if (drink_fountain() == 1) return;
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

if (keyin == '?' || keyin == '*')
{
        if (keyin == '?') nthing = get_invent(OBJ_POTIONS);
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

drink_1 = (int) keyin;

if (drink_1 < 65 || (drink_1 > 90 && drink_1 < 97) || drink_1 > 122)
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


if (you[0].inv_class [drink_2] != OBJ_POTIONS)
{
        strcpy(info, "You can't drink that!");
        mpr(info);
        return;
}

you[0].inv_quant [drink_2] --;

if (you[0].inv_quant [drink_2] == 0)
{
        you[0].inv_no --;
        if (you[0].equip [EQ_WEAPON] == drink_2)
        {
                you[0].equip [EQ_WEAPON] = -1;
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


char drink_fountain(void)
{
char dry = 0;

switch(grd [you[0].x_pos] [you[0].y_pos])
{
 case 200: mpr("Drink from the fountain?"); break;
 case 202: mpr("Drink from the sparkling fountain?"); break;
}

char keyin = get_ch();
if (keyin == 'y' || keyin == 'Y')
{
 switch(grd [you[0].x_pos] [you[0].y_pos])
 {
  case 200: /* water fountain */
  mpr("You drink the pure, clear water.");
  if (random2(20) == 0) dry = 1;
  break;
  case 202: /* magic fountain */
  mpr("You drink the sparkling water.");
  switch(random2(25))
  {
   case 0:
   case 1:
   case 2:
   case 3: potion_effect(random2(4), 100); break;
   case 4: if (random2(3) == 0) potion_effect(POT_GAIN_STRENGTH + random2(3), 100); break;
   case 5:
   case 6:
   case 7:
   case 8: potion_effect(POT_LEVITATION + random2(6), 100); break;
   case 9: if (random2(5) == 0) potion_effect(POT_DECAY, 100); else potion_effect(POT_DEGENERATION, 100); break;
   case 10: potion_effect(POT_DECAY, 100); break;
   case 11:
   case 12: potion_effect(POT_MAGIC + random2(4), 100); break;
   case 13: potion_effect(POT_MUTATION, 100); break;
   default: mpr("Nothing appears to happen."); break; /* Does nothing */
  }
  if (random2(10) == 0)
  {
    dry = 1;
    if (random2(5) == 0) grd [you[0].x_pos] [you[0].y_pos] = 200; /* back to a normal fountain, but you won't know it yet */
  }
  break;
 }
} else return 0;

if (dry == 1)
{
   mpr("The fountain dries up!");
   grd [you[0].x_pos] [you[0].y_pos] ++;
}

you[0].turnover = 1;

return 1;

}

void read_scroll(void)
{

int sc_read_1;
int sc_read_2;

int id_used = 0;
unsigned char nthing = 0;

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

if (keyin == '?' || keyin == '*')
{
        if (keyin == '?') nthing = get_invent(OBJ_SCROLLS);
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
        return;
}

sc_read_2 = conv_lett(sc_read_1);

if (you[0].inv_quant [sc_read_2] == 0)
{
        strcpy(info, "You don't have any such object.");
        mpr(info);
        return;
}

if (you[0].inv_class [sc_read_2] == OBJ_BOOKS)
{
if (you[0].inv_type [sc_read_2] == BOOK_DESTRUCTION)
{
 tome_of_power(sc_read_2);
 return;
}
if (you[0].inv_type [sc_read_2] == BOOK_MANUAL)
{
 skill_manual(sc_read_2);
 return;
}
 read_book(sc_read_2);
 if (book_thing < 'a' || book_thing > 'f')
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
#ifdef PLAIN_TERM
redraw_screen();
#endif
    mesclr();
    return;
}

if (you[0].inv_class [sc_read_2] != OBJ_SCROLLS)
{
        strcpy(info, "You can't read that!");
        mpr(info);
        return;
}

if (you[0].inv_type [sc_read_2] == SCR_PAPER)
{
  strcpy(info, "This scroll appears to be blank.");
  mpr(info);
  you[0].turnover = 1;
  set_id(you[0].inv_class [sc_read_2], you[0].inv_type [sc_read_2], 1);
  return;
}

if (you[0].mutation [MUT_BLURRY_VISION] > 0 && random2(5) < you[0].mutation [MUT_BLURRY_VISION])
{
 if (you[0].mutation [MUT_BLURRY_VISION] == 3 && random2(3) == 0)
   mpr("This scroll appears to be blank.");
    else mpr("The writing blurs in front of your eyes.");
 you[0].turnover = 1;
 return;
}

you[0].inv_quant [sc_read_2] --;

if (you[0].inv_quant [sc_read_2] == 0)
{
        you[0].inv_no --;
        if (you[0].equip [EQ_WEAPON] == sc_read_2)
        {
                you[0].equip [EQ_WEAPON] = -1;
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

if (you[0].skills [SK_SPELLCASTING] == 0) exercise(SK_SPELLCASTING, 3 + random2(4));

switch(you[0].inv_type [sc_read_2])
{


case SCR_IDENTIFY: // identify
you[0].inv_ident [scroll] = 2;
set_id(you[0].inv_class [sc_read_2], you[0].inv_type [sc_read_2], 1);
identify(1);
return;


case SCR_TELEPORTATION: // teleport
        you_teleport();
        set_id(you[0].inv_class [sc_read_2], you[0].inv_type [sc_read_2], 1);
return;

case SCR_FEAR: // fear
strcpy(info, "You feel scary!");
mpr(info);
set_id(you[0].inv_class [sc_read_2], you[0].inv_type [sc_read_2], 1);
 mass_enchantment(4, 1000);
break;

case SCR_NOISE: // noise
strcpy(info, "You hear a loud clanging noise!");
mpr(info);
noisy(25, you[0].x_pos, you[0].y_pos);
set_id(you[0].inv_class [sc_read_2], you[0].inv_type [sc_read_2], 1);
return;

case SCR_REMOVE_CURSE: // scroll of remove curse
        remove_curse();
        set_id(you[0].inv_class [sc_read_2], you[0].inv_type [sc_read_2], 1);
break;

case SCR_DETECT_CURSE: // detect curse
 detect_curse();
set_id(you[0].inv_class [sc_read_2], you[0].inv_type [sc_read_2], 1);
break;

case SCR_SUMMONING: // summoning
strcpy(info, "A horrible Thing appears!");
mpr(info);
create_monster(MONS_LARGE_ABOMINATION, 25, 7, you[0].x_pos, you[0].y_pos, MHITNOT, 250);
set_id(you[0].inv_class [sc_read_2], you[0].inv_type [sc_read_2], 1);
break;


case SCR_ENCHANT_WEAPON_I: // scroll of enchant weapon I
case SCR_ENCHANT_WEAPON_II: // II
case SCR_ENCHANT_WEAPON_III: // III
weapon_enchant : if (you[0].equip [EQ_WEAPON] == -1)
{
        strcpy(info, "Nothing appears to happen.");
        mpr(info);
        return;
}


if (you[0].inv_class [you[0].equip [EQ_WEAPON]] == OBJ_WEAPONS || you[0].inv_class [you[0].equip [EQ_WEAPON]] == OBJ_MISSILES)
{

if (you[0].inv_type [sc_read_2] == SCR_ENCHANT_WEAPON_II && you[0].inv_class [you[0].equip [EQ_WEAPON]] == OBJ_WEAPONS)
{
 id_used = you[0].inv_plus2 [you[0].equip [EQ_WEAPON]];
} else id_used = you[0].inv_plus [you[0].equip [EQ_WEAPON]];

affected = 1;

if (you[0].inv_class [you[0].equip [EQ_WEAPON]] == OBJ_WEAPONS && (you[0].inv_dam [you[0].equip [EQ_WEAPON]] > 180 || you[0].inv_dam [you[0].equip [EQ_WEAPON]] % 30 >= 25)) // artefacts can't be enchanted, but these scrolls still remove curses
{
/*      strcpy(info, "Nothing appears to happen.");
        mpr(info);
        return;*/
    affected = 0;
}

 wield_change = 1;

/* if (you[0].inv_dam [you[0].equip [EQ_WEAPON]] == 4) // electrocution
 {
   if (id_used > 130)
   {
    id_used -= 100;
   }
                item_name(you[0].inv_plus2 [you[0].equip [EQ_WEAPON]], you[0].inv_class [you[0].equip [EQ_WEAPON]], you[0].inv_type [you[0].equip [EQ_WEAPON]], you[0].inv_dam [you[0].equip [EQ_WEAPON]], you[0].inv_plus [you[0].equip [EQ_WEAPON]], you[0].inv_quant [you[0].equip [EQ_WEAPON]], you[0].inv_ident [you[0].equip [EQ_WEAPON]], 4, str_pass);

   id_used += 3 + random2(3) + random2(3);
   if (id_used >= 59) id_used = 59;
   you[0].inv_plus2 [you[0].equip [EQ_WEAPON]] = id_used;
   strcpy(info, str_pass);
   strcat(info, " glows electric blue for a moment.");
   mpr(info);
   return;
 }*/

 if (id_used >= 154 || (id_used < 100 && id_used >= 54))
 {
   if (random2(9) < id_used % 50) affected = 0;
 }

// if it isn't affected by the enchantment, it will still be uncursed:
        if (you[0].inv_plus [you[0].equip [EQ_WEAPON]] >= 154 || (you[0].inv_dam [you[0].equip [EQ_WEAPON]] % 30 >= 25 && you[0].inv_plus [you[0].equip [EQ_WEAPON]] >= 100))
        {
                if (affected == 0)
                {
                 item_name(you[0].inv_plus2 [you[0].equip [EQ_WEAPON]], you[0].inv_class [you[0].equip [EQ_WEAPON]], you[0].inv_type [you[0].equip [EQ_WEAPON]], you[0].inv_dam [you[0].equip [EQ_WEAPON]], you[0].inv_plus [you[0].equip [EQ_WEAPON]], you[0].inv_quant [you[0].equip [EQ_WEAPON]], you[0].inv_ident [you[0].equip [EQ_WEAPON]], 4, str_pass);
                 strcpy(info, str_pass);
                 strcat(info, " glows silver for a moment.");
                 mpr(info);
                 you[0].inv_plus [you[0].equip [EQ_WEAPON]] -= 100;
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
        item_name(you[0].inv_plus2 [you[0].equip [EQ_WEAPON]], you[0].inv_class [you[0].equip [EQ_WEAPON]], you[0].inv_type [you[0].equip [EQ_WEAPON]], you[0].inv_dam [you[0].equip [EQ_WEAPON]], you[0].inv_plus [you[0].equip [EQ_WEAPON]], you[0].inv_quant [you[0].equip [EQ_WEAPON]], you[0].inv_ident [you[0].equip [EQ_WEAPON]], 4, str_pass);

        strcpy(info, str_pass);
        if (you[0].inv_type [sc_read_2] == SCR_ENCHANT_WEAPON_II && you[0].inv_class [you[0].equip [EQ_WEAPON]] == OBJ_WEAPONS)
        {
         strcat(info, " glows red for a moment.");
         mpr(info);
        } else
        if (you[0].inv_type [sc_read_2] == SCR_ENCHANT_WEAPON_I && you[0].inv_class [you[0].equip [EQ_WEAPON]] == OBJ_WEAPONS)
        {
         strcat(info, " glows green for a moment.");
         mpr(info);
        } else if (you[0].inv_class [you[0].equip [EQ_WEAPON]] == OBJ_WEAPONS)
        {
         id_used --; /* no plusses for using this */
         if (you[0].duration [DUR_VORPAL_BLADE] > 0)
         {
          you[0].duration [DUR_VORPAL_BLADE] = 0;
          strcat(info, "'s sharpness seems more permanent.");
          mpr(info);
         } else
         if (you[0].duration [DUR_FIRE_BRAND] > 0)
         {
          you[0].duration [DUR_FIRE_BRAND] = 0;
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
         if (you[0].duration [DUR_ICE_BRAND] > 0)
         {
          you[0].duration [DUR_ICE_BRAND] = 0;
          strcat(info, " glows brilliantly blue for a moment.");
          mpr(info);
          cast_refrigeration(60);
         } else
         if (you[0].duration [DUR_LETHAL_INFUSION] > 0)
         {
          you[0].duration [DUR_LETHAL_INFUSION] = 0;
          strcat(info, " thirsts for the lives of mortals!");
          mpr(info);
          if (player_prot_life() == 0 && you[0].is_undead == 0)
          {
           mpr("You feel drained.");
           drain_exp();
          }
         } else
         if (you[0].duration [DUR_POISON_WEAPON] > 0)
         {
          you[0].duration [DUR_POISON_WEAPON] = 0;
          strcat(info, " seems more permanently poisoned.");
          mpr(info);
          cast_toxic_radiance();
         } else
         {
           strcat(info, " glows bright yellow for a while.");
           mpr(info);
           you[0].inv_plus [you[0].equip [EQ_WEAPON]] += 1 + random2(2);
           you[0].inv_plus2 [you[0].equip [EQ_WEAPON]] += 1 + random2(2);
           if (you[0].inv_plus [you[0].equip [EQ_WEAPON]] > 130) you[0].inv_plus [you[0].equip [EQ_WEAPON]] -= 100;
           set_id(you[0].inv_class [sc_read_2], you[0].inv_type [sc_read_2], 1);
           return;
         }
        }
        set_id(you[0].inv_class [sc_read_2], you[0].inv_type [sc_read_2], 1);


        id_used ++;
        if (you[0].inv_plus [you[0].equip [EQ_WEAPON]] > 130) you[0].inv_plus [you[0].equip [EQ_WEAPON]] -= 100;
        if (id_used > 130) id_used -= 100;

        if (you[0].inv_type [sc_read_2] == SCR_ENCHANT_WEAPON_II && you[0].inv_class [you[0].equip [EQ_WEAPON]] == OBJ_WEAPONS)
        {
         you[0].inv_plus2 [you[0].equip [EQ_WEAPON]] = id_used;
        } else if (you[0].inv_type [sc_read_2] == SCR_ENCHANT_WEAPON_I && you[0].inv_class [you[0].equip [EQ_WEAPON]] == OBJ_WEAPONS)
        {
         you[0].inv_plus [you[0].equip [EQ_WEAPON]] = id_used;
        }
        return;

//        strcat(info,
}

nothing_happened: strcpy(info, "Nothing appears to happen.");
mpr(info);
return; // end of case 7: enc weapon

case SCR_VORPALISE_WEAPON: // vorpalise weapon
if (you[0].equip [EQ_WEAPON] == -1 || you[0].inv_class [you[0].equip [EQ_WEAPON]] != OBJ_WEAPONS || (you[0].inv_class [you[0].equip [EQ_WEAPON]] == OBJ_WEAPONS && (you[0].inv_dam [you[0].equip [EQ_WEAPON]] > 180 || you[0].inv_dam [you[0].equip [EQ_WEAPON]] % 30 >= 25 || (you[0].inv_type [you[0].equip [EQ_WEAPON]] >= WPN_SLING && you[0].inv_type [you[0].equip [EQ_WEAPON]] <= WPN_HAND_CROSSBOW))))
{
        strcpy(info, "Nothing appears to happen.");
        mpr(info);
        return;
}

 item_name(you[0].inv_plus2 [you[0].equip [EQ_WEAPON]], you[0].inv_class [you[0].equip [EQ_WEAPON]], you[0].inv_type [you[0].equip [EQ_WEAPON]], you[0].inv_dam [you[0].equip [EQ_WEAPON]], you[0].inv_plus [you[0].equip [EQ_WEAPON]], you[0].inv_quant [you[0].equip [EQ_WEAPON]], you[0].inv_ident [you[0].equip [EQ_WEAPON]], 4, str_pass);
 strcpy(info, str_pass);
 strcat(info, " emits a brilliant flash of light!");
 alert();
 mpr(info);
 set_id(you[0].inv_class [sc_read_2], you[0].inv_type [sc_read_2], 1);

if (you[0].inv_dam [you[0].equip [EQ_WEAPON]] % 30 != SPWPN_NORMAL)
{
/* you[0].inv_plus [you[0].equip [EQ_WEAPON]] = 50;
 you[0].inv_plus2 [you[0].equip [EQ_WEAPON]] = 50;
 switch(you[0].inv_dam [you[0].equip [EQ_WEAPON]] % 30)
 {
  case SPWPN_PROTECTION:
/ *  player_AC() -= 3;* /
  you[0].AC_ch = 1;
  break;

  case SPWPN_VAMPIRICISM:
  you[0].hunger_inc -= 6;
  break;
 }
 you[0].inv_dam [you[0].equip [EQ_WEAPON]] = (you[0].inv_dam [you[0].equip [EQ_WEAPON]] / 30) * 30;
 if (you[0].inv_dam [you[0].equip [EQ_WEAPON]] == 90) you[0].inv_dam [you[0].equip [EQ_WEAPON]] = SPWPN_NORMAL; // stops glowing
 you[0].duration [DUR_VORPAL_BLADE] = 0;
 you[0].duration [DUR_FIRE_BRAND] = 0;
 you[0].duration [DUR_ICE_BRAND] = 0;
 you[0].duration [DUR_LETHAL_INFUSION] = 0;*/
 mpr("You feel strangely frustrated.");
 return;
}
you[0].inv_dam [you[0].equip [EQ_WEAPON]] += 10;
return;


case SCR_RECHARGING: // recharge wand
if (you[0].equip [EQ_WEAPON] != -1 && you[0].inv_class [you[0].equip [EQ_WEAPON]] == OBJ_WEAPONS && you[0].inv_dam [you[0].equip [EQ_WEAPON]] == SPWPN_ELECTROCUTION) // electrocution
 goto weapon_enchant;

if (recharge_wand() == 0)
{
 strcpy(info, "Nothing appears to happen.");
 mpr(info);
} else set_id(you[0].inv_class [sc_read_2], you[0].inv_type [sc_read_2], 1);
return;


case SCR_ENCHANT_ARMOUR: // ench you[0].equip
for (i = EQ_CLOAK; i < EQ_RIGHT_RING; i ++)
{
        if (i == EQ_LEFT_RING)
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

        if (you[0].inv_type [you[0].equip [EQ_BODY_ARMOUR]] == ARM_DRAGON_HIDE || you[0].inv_type [you[0].equip [EQ_BODY_ARMOUR]] == ARM_ICE_DRAGON_HIDE || you[0].inv_type [you[0].equip [EQ_BODY_ARMOUR]] == ARM_STEAM_DRAGON_HIDE || you[0].inv_type [you[0].equip [EQ_BODY_ARMOUR]] == ARM_MOTTLED_DRAGON_HIDE || you[0].inv_type [you[0].equip [EQ_BODY_ARMOUR]] == ARM_STORM_DRAGON_HIDE || you[0].inv_type [you[0].equip [EQ_BODY_ARMOUR]] == ARM_GOLD_DRAGON_HIDE || you[0].inv_type [you[0].equip [EQ_BODY_ARMOUR]] == ARM_SWAMP_DRAGON_HIDE) // dragon hide
        {
                affected = EQ_BODY_ARMOUR;
                item_name(you[0].inv_plus2 [you[0].equip [affected]], you[0].inv_class [you[0].equip [affected]], you[0].inv_type [you[0].equip [affected]], you[0].inv_dam [you[0].equip [affected]], you[0].inv_plus [you[0].equip [affected]], you[0].inv_quant [you[0].equip [affected]], you[0].inv_ident [you[0].equip [affected]], 4, str_pass);
                strcpy(info, str_pass);
                strcat(info, " glows purple and changes!");
                mpr(info);
                you[0].AC_ch = 1;

/* player_AC() -= property [2] [you[0].inv_type [you[0].equip [EQ_BODY_ARMOUR]]] [0] * (15 + you[0].skills [SK_ARMOUR]) / 15;


        player_evasion() -= ev_mod();*/

                switch(you[0].inv_type [you[0].equip [EQ_BODY_ARMOUR]])
                {

                 case ARM_DRAGON_HIDE:
/*                 player_res_fire() ++;
                 player_res_cold() --;*/
                 you[0].inv_type [you[0].equip [EQ_BODY_ARMOUR]] = ARM_DRAGON_ARMOUR;
                 break;

                 case ARM_ICE_DRAGON_HIDE:
/*                 player_res_cold() ++;
                 player_res_fire() --;*/
                 you[0].inv_type [you[0].equip [EQ_BODY_ARMOUR]] = ARM_ICE_DRAGON_ARMOUR;
                 break;

                 case ARM_STEAM_DRAGON_HIDE: // steam dragon
                 you[0].inv_type [you[0].equip [EQ_BODY_ARMOUR]] = ARM_STEAM_DRAGON_ARMOUR;
                 break;

                 case ARM_MOTTLED_DRAGON_HIDE: // mottled dragon
                 you[0].inv_type [you[0].equip [EQ_BODY_ARMOUR]] = ARM_MOTTLED_DRAGON_ARMOUR;
                 break;

                 case ARM_STORM_DRAGON_HIDE: // storm dragon
                 you[0].inv_type [you[0].equip [EQ_BODY_ARMOUR]] = ARM_STORM_DRAGON_ARMOUR;
                 you[0].attribute [ATTR_RESIST_LIGHTNING] ++;
                 break;

                 case ARM_GOLD_DRAGON_HIDE: // gold dragon
                 you[0].inv_type [you[0].equip [EQ_BODY_ARMOUR]] = ARM_GOLD_DRAGON_ARMOUR;
/*                 player_res_fire() ++;
                 player_res_cold() ++;
                 you[0].res_poison ++;*/
                 break;

                 case ARM_SWAMP_DRAGON_HIDE: // swamp dragon
                 you[0].inv_type [you[0].equip [EQ_BODY_ARMOUR]] = ARM_SWAMP_DRAGON_ARMOUR;
                 break;

                }
/*        player_evasion() += ev_mod();
 player_AC() += property [2] [you[0].inv_type [you[0].equip [EQ_BODY_ARMOUR]]] [0] * (15 + you[0].skills [SK_ARMOUR]) / 15;*/

        set_id(you[0].inv_class [sc_read_2], you[0].inv_type [sc_read_2], 1);
        return;
        }


        if (you[0].inv_type [you[0].equip [EQ_BODY_ARMOUR]] == ARM_TROLL_HIDE) // troll hide
        {
                affected = EQ_BODY_ARMOUR;
                item_name(you[0].inv_plus2 [you[0].equip [affected]], you[0].inv_class [you[0].equip [affected]], you[0].inv_type [you[0].equip [affected]], you[0].inv_dam [you[0].equip [affected]], you[0].inv_plus [you[0].equip [affected]], you[0].inv_quant [you[0].equip [affected]], you[0].inv_ident [you[0].equip [affected]], 4, str_pass);
                strcpy(info, str_pass);
                strcat(info, " glows purple and changes!");
                mpr(info);
/*                player_AC() += 1;*/
                you[0].AC_ch = 1;
/*                you[0].rate_regen += 50;*/
//                you[0].hunger_inc += 1;
/* player_AC() -= property [2] [you[0].inv_type [you[0].equip [EQ_BODY_ARMOUR]]] [0] * (15 + you[0].skills [SK_ARMOUR]) / 15;
        player_evasion() -= ev_mod();*/
                you[0].inv_type [you[0].equip [EQ_BODY_ARMOUR]] = ARM_TROLL_LEATHER_ARMOUR;
/*        player_evasion() += ev_mod();
 player_AC() += property [2] [you[0].inv_type [you[0].equip [EQ_BODY_ARMOUR]]] [0] * (15 + you[0].skills [SK_ARMOUR]) / 15;*/
        set_id(you[0].inv_class [sc_read_2], you[0].inv_type [sc_read_2], 1);
        return;
        }

    if (you[0].inv_dam [you[0].equip [affected]] % 30 >= 25)
        {
                item_name(you[0].inv_plus2 [you[0].equip [affected]], you[0].inv_class [you[0].equip [affected]], you[0].inv_type [you[0].equip [affected]], you[0].inv_dam [you[0].equip [affected]], you[0].inv_plus [you[0].equip [affected]], you[0].inv_quant [you[0].equip [affected]], you[0].inv_ident [you[0].equip [affected]], 4, str_pass);
                set_id(you[0].inv_class [sc_read_2], you[0].inv_type [sc_read_2], 1);
                strcpy(info, str_pass);
                strcat(info, " glows faintly for a moment.");
                mpr(info);
        if (you[0].inv_plus [you[0].equip [affected]] > 100) you[0].inv_plus [you[0].equip [affected]] -= 100;
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

        if ((you[0].inv_plus [you[0].equip [affected]] - 50 >= 3 && you[0].inv_plus [you[0].equip [affected]] < 130) || you[0].inv_plus [you[0].equip [affected]] >= 153)
        {
                if (random2(8) < (you[0].inv_plus [you[0].equip [affected]] - 50)) goto nothing_happened_2;
        }

        if (you[0].inv_type [you[0].equip [affected]] >= ARM_CLOAK && you[0].inv_type [you[0].equip [affected]] <= ARM_BOOTS)
          if ((you[0].inv_plus [you[0].equip [affected]] - 50 >= 2 && you[0].inv_plus [you[0].equip [affected]] < 130) || you[0].inv_plus [you[0].equip [affected]] >= 152)
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

case SCR_TORMENT: // torment
/*      beam[0].bx = you[0].x_pos;
        beam[0].by = you[0].y_pos;*/
        torment(); //you[0].x_pos, you[0].y_pos);
        if (get_id(you[0].inv_class [sc_read_2], you[0].inv_type [sc_read_2]) == 1)
          naughty(NAUGHTY_UNHOLY, 10); // is only naughty if you know you're doing it
        set_id(you[0].inv_class [sc_read_2], you[0].inv_type [sc_read_2], 1);
        return;

case SCR_RANDOM_USELESSNESS: // random uselessness
        random_uselessness(random2(9), sc_read_2); //random2 (1));
        return;

case SCR_CURSE_WEAPON: // curse weapon
        if (you[0].equip [EQ_WEAPON] == -1 || you[0].inv_class [you[0].equip [EQ_WEAPON]] > OBJ_WEAPONS || you[0].inv_plus [you[0].equip [EQ_WEAPON]] > 130) // || you[0].inv_dam [you[0].equip [EQ_WEAPON]] % 30 >= 25)
        {
                goto nothing_happened_2;
        }
                in_name(you[0].equip [EQ_WEAPON], 4, str_pass);
                strcpy(info, str_pass);
                strcat(info, " glows black for a moment.");
                mpr(info);
                you[0].inv_plus [you[0].equip [EQ_WEAPON]] += 100;
                set_id(you[0].inv_class [sc_read_2], you[0].inv_type [sc_read_2], 1);
                wield_change = 1;
                return;


case SCR_CURSE_ARMOUR: // curse you[0].equip
for (i = EQ_CLOAK; i < EQ_RIGHT_RING; i ++)
{
        if (i == EQ_LEFT_RING)
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
} while (you[0].equip [affected] == -1 || you[0].inv_plus [you[0].equip [affected]] > 130);


        // vVvVv    This is *here* for a reason!
        item_name(you[0].inv_plus2 [you[0].equip [affected]], you[0].inv_class [you[0].equip [affected]], you[0].inv_type [you[0].equip [affected]], you[0].inv_dam [you[0].equip [affected]], you[0].inv_plus [you[0].equip [affected]], you[0].inv_quant [you[0].equip [affected]], you[0].inv_ident [you[0].equip [affected]], 4, str_pass);
        you[0].inv_plus [you[0].equip [affected]] += 100;
        set_id(you[0].inv_class [sc_read_2], you[0].inv_type [sc_read_2], 1);
        strcpy(info, str_pass);
        strcat(info, " glows black for a moment.");
        mpr(info);
        return;


case SCR_IMMOLATION: // immolation
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

case SCR_BLINKING: // blink

   blink();

   set_id(you[0].inv_class [sc_read_2], you[0].inv_type [sc_read_2], 1);

break;

// This is never called; it is dealt with earlier:
case SCR_PAPER: // paper
break;

case SCR_MAGIC_MAPPING: // magic mapping
if (you[0].level_type == 1 || you[0].level_type == 2)
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

case SCR_FORGETFULNESS: // forgetting
strcpy(info, "You feel momentarily disoriented.");
mpr(info);
forget_map(50 + random2(50));
set_id(you[0].inv_class [sc_read_2], you[0].inv_type [sc_read_2], 1);
break;

case SCR_ACQUIREMENT:
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

if (keyin == '?' || keyin == '*')
{
        if (keyin == '?') nthing = get_invent(-1);
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

drink_1 = (int) keyin;

if (drink_1 < 65 || (drink_1 > 90 && drink_1 < 97) || drink_1 > 122)
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
#ifdef PLAIN_TERM
redraw_screen();
#endif

} // end original_name


void use_randart(unsigned char item_wield_2)
{

 if (randart_wpn_properties(you[0].inv_class [item_wield_2], you[0].inv_type [item_wield_2], you[0].inv_dam [item_wield_2], you[0].inv_plus [item_wield_2], you[0].inv_plus2 [item_wield_2], 0, RAP_AC) != 0)
 {
  you[0].AC_ch = 1;
 }
 if (randart_wpn_properties(you[0].inv_class [item_wield_2], you[0].inv_type [item_wield_2], you[0].inv_dam [item_wield_2], you[0].inv_plus [item_wield_2], you[0].inv_plus2 [item_wield_2], 0, RAP_EVASION) != 0)
 {
  you[0].evasion_ch = 1;
 }
 if (randart_wpn_properties(you[0].inv_class [item_wield_2], you[0].inv_type [item_wield_2], you[0].inv_dam [item_wield_2], you[0].inv_plus [item_wield_2], you[0].inv_plus2 [item_wield_2], 0, RAP_STRENGTH) != 0)
 {
  you[0].strength_ch = 1;
  you[0].strength += randart_wpn_properties(you[0].inv_class [item_wield_2], you[0].inv_type [item_wield_2], you[0].inv_dam [item_wield_2], you[0].inv_plus [item_wield_2], you[0].inv_plus2 [item_wield_2], 0, RAP_STRENGTH);
  you[0].max_strength += randart_wpn_properties(you[0].inv_class [item_wield_2], you[0].inv_type [item_wield_2], you[0].inv_dam [item_wield_2], you[0].inv_plus [item_wield_2], you[0].inv_plus2 [item_wield_2], 0, RAP_STRENGTH);
 }
 if (randart_wpn_properties(you[0].inv_class [item_wield_2], you[0].inv_type [item_wield_2], you[0].inv_dam [item_wield_2], you[0].inv_plus [item_wield_2], you[0].inv_plus2 [item_wield_2], 0, RAP_INTELLIGENCE) != 0)
 {
  you[0].intel_ch = 1;
  you[0].intel += randart_wpn_properties(you[0].inv_class [item_wield_2], you[0].inv_type [item_wield_2], you[0].inv_dam [item_wield_2], you[0].inv_plus [item_wield_2], you[0].inv_plus2 [item_wield_2], 0, RAP_INTELLIGENCE);
  you[0].max_intel += randart_wpn_properties(you[0].inv_class [item_wield_2], you[0].inv_type [item_wield_2], you[0].inv_dam [item_wield_2], you[0].inv_plus [item_wield_2], you[0].inv_plus2 [item_wield_2], 0, RAP_INTELLIGENCE);
 }
 if (randart_wpn_properties(you[0].inv_class [item_wield_2], you[0].inv_type [item_wield_2], you[0].inv_dam [item_wield_2], you[0].inv_plus [item_wield_2], you[0].inv_plus2 [item_wield_2], 0, RAP_DEXTERITY) != 0)
 {
  you[0].dex_ch = 1;
  you[0].dex += randart_wpn_properties(you[0].inv_class [item_wield_2], you[0].inv_type [item_wield_2], you[0].inv_dam [item_wield_2], you[0].inv_plus [item_wield_2], you[0].inv_plus2 [item_wield_2], 0, RAP_DEXTERITY);
  you[0].max_dex += randart_wpn_properties(you[0].inv_class [item_wield_2], you[0].inv_type [item_wield_2], you[0].inv_dam [item_wield_2], you[0].inv_plus [item_wield_2], you[0].inv_plus2 [item_wield_2], 0, RAP_DEXTERITY);
 }
 if (randart_wpn_properties(you[0].inv_class [item_wield_2], you[0].inv_type [item_wield_2], you[0].inv_dam [item_wield_2], you[0].inv_plus [item_wield_2], you[0].inv_plus2 [item_wield_2], 0, RAP_ELECTRICITY) != 0)
 {
  you[0].attribute [ATTR_RESIST_LIGHTNING] ++;
 }
 if (randart_wpn_properties(you[0].inv_class [item_wield_2], you[0].inv_type [item_wield_2], you[0].inv_dam [item_wield_2], you[0].inv_plus [item_wield_2], you[0].inv_plus2 [item_wield_2], 0, RAP_METABOLISM) != 0)
 {
  you[0].hunger_inc += randart_wpn_properties(you[0].inv_class [item_wield_2], you[0].inv_type [item_wield_2], you[0].inv_dam [item_wield_2], you[0].inv_plus [item_wield_2], you[0].inv_plus2 [item_wield_2], 0, RAP_METABOLISM);
 }
 if (randart_wpn_properties(you[0].inv_class [item_wield_2], you[0].inv_type [item_wield_2], you[0].inv_dam [item_wield_2], you[0].inv_plus [item_wield_2], you[0].inv_plus2 [item_wield_2], 0, RAP_NOISES) != 0)
 {
  you[0].special_wield = 50 + randart_wpn_properties(you[0].inv_class [item_wield_2], you[0].inv_type [item_wield_2], you[0].inv_dam [item_wield_2], you[0].inv_plus [item_wield_2], you[0].inv_plus2 [item_wield_2], 0, RAP_NOISES);
 }

}

/*
 *  File:       item_use.cc
 *  Summary:    Functions for making use of inventory items.
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 *      <6>     7/13/99         BWR             Lowered learning rates for
 *                                              throwing skills, and other
 *                                              balance tweaks
 *      <5>     5/28/99         JDJ             Changed wear_armour to allow Spriggan's to wear bucklers.
 *      <4>     5/26/99         JDJ             body armor can be removed and worn if an uncursed cloak is being worn.
 *                                                                      Removed lots of unnessary mpr string copying. Added missing
 *                                                                      ponderous message.
 *      <3>     5/20/99         BWR             Fixed staff of air bug, output of trial identified
 *                                                                      items, a few wield_changes so that the weapon gets
 *                                                                      updated.
 *      <2>     5/08/99         JDJ             Added armour_prompt.
 *      <1>     -/--/--         LRH             Created
 */

#include "AppHdr.h"
#include "item_use.h"

#include <string.h>
#include <stdlib.h>

#include "externs.h"
#include "enum.h"

#include "bang.h"
#include "beam.h"
#include "debug.h"
#include "describe.h"
#include "direct.h"
#include "effects.h"
#include "fight.h"
#include "files.h"
#include "food.h"
#include "invent.h"
#include "it_use2.h"
#include "it_use3.h"
#include "itemname.h"
#include "items.h"
#include "message.h"
#include "misc.h"
#include "monplace.h"
#include "mstruct.h"
#include "newgame.h"
#include "ouch.h"
#include "output.h"
#include "player.h"
#include "randart.h"
#include "religion.h"
#include "skills.h"
#include "skills2.h"
#include "spells.h"
#include "spells0.h"
#include "spells1.h"
#include "spells2.h"
#include "spells3.h"
#include "stuff.h"
#include "transfor.h"
#include "view.h"

void throw_it(struct bolt beam[1], int throw_2);
void use_randart(unsigned char item_wield_2);
char drink_fountain();

extern int book_thing;          /* defined in spells.cc */
extern char wield_change;       /* defined in output.cc */


void wield_weapon(char auto_wield)
{
    unsigned char nthing = 0;
    unsigned char i_dam = 0;
    char str_pass[80];
    unsigned char keyin;

    if (you.num_inv_items == 0)
    {
        mpr("You aren't carrying anything.");
        return;
    }

    if (you.berserker != 0)
    {
        mpr("You are too berserk!");
        return;
    }

    if (you.attribute[ATTR_TRANSFORMATION] != TRAN_NONE)
    {
        if (can_equip(0) == 0)
        {
            mpr("You can't wield anything in your present form.");
            return;
        }
    }

    if ((you.equip[EQ_WEAPON] != -1)
        && you.inv_class[you.equip[EQ_WEAPON]] == OBJ_WEAPONS
        && you.inv_plus[you.equip[EQ_WEAPON]] > 80)
    {
        mpr("You can't unwield your weapon to draw a new one!");
        return;
    }

    if (auto_wield == 1)
    {
        if (you.equip[EQ_WEAPON] == OBJ_WEAPONS)
            keyin = 'b';
        else
            keyin = 'a';
    }
    else
    {

      query:
        mpr("Wield which item (- for none)?");

        // test relay_message();

        keyin = get_ch();

        if (keyin == '?')
        {
            //inventory(0);
            nthing = get_invent(OBJ_WEAPONS);
            //invent(you.inv_plus2, 0, you.inv_quantity, you.inv_dam, you.inv_class, you.inv_type, you.inv_plus, you.inv_ident, you.equip [0], you.equip [6], you.equip [5], you.equip [2], you.equip [1], you.equip [3], you.equip [4], you.ring);
            if ((nthing >= 65 && nthing <= 90)
                                    || (nthing >= 97 && nthing <= 122))
            {
                keyin = nthing;
            }
            else
            {
                mesclr();
                goto query;
                /*              strcpy(info, "Wield which item?"); mpr(info);
                   relay_message();
                   keyin = get_ch(); */
            }
        }

        if (keyin == '*')
        {
            //inventory(-1);
            nthing = get_invent(-1);

            if ((nthing >= 65 && nthing <= 90)
                                || (nthing >= 97 && nthing <= 122))
            {
                keyin = nthing;
            }
            else
            {
                mpr("Wield which item?");
                keyin = get_ch();
            }
        }

    }

    int item_wield_1 = (int) keyin;

    if (keyin == '-')
    {
        if (you.equip[EQ_WEAPON] != -1)
        {
            unwield_item(you.equip[EQ_WEAPON]);
            you.turn_is_over = 1;
        }
        you.equip[EQ_WEAPON] = -1;
        mpr("You are empty handed.");
        you.time_taken *= 3;
        you.time_taken /= 10;
        return;
    }

    if (item_wield_1 < 65 || (item_wield_1 > 90 && item_wield_1 < 97)
                                                    || item_wield_1 > 122)
    {
        mpr("You don't have any such object.");
        return;
    }

    int item_wield_2 = conv_lett(item_wield_1);

    if (item_wield_2 == you.equip[EQ_WEAPON])
    {
        mpr("You are already wielding that!");
        return;
    }

    for (int i = EQ_CLOAK; i <= EQ_AMULET; i++)
    {
        if (item_wield_2 == you.equip[i])
        {
            mpr("You are wearing that object!");
            return;
        }
    }

    if (you.inv_quantity[item_wield_2] == 0)
    {
        mpr("You don't have any such object.");
        return;
    }
    // 2-hands 7,10,12, {20,21}

    if (you.inv_class[item_wield_2] != OBJ_WEAPONS)
    {
        if (   you.inv_class[item_wield_2] == OBJ_STAVES
            && you.equip[EQ_SHIELD] != -1)
        {
            mpr("You can't wield that with a shield.");
            return;
        }

        if (you.equip[EQ_WEAPON] != -1)
            unwield_item(you.equip[EQ_WEAPON]);

        you.equip[EQ_WEAPON] = item_wield_2;
    }
    else
    {

        if ((you.species < SP_OGRE || you.species > SP_OGRE_MAGE)
                                && mass(you.inv_class[item_wield_2],
                                            you.inv_type[item_wield_2]) >= 500)
        {
            mpr("That's too large and heavy for you to wield.");
            return;
        }

        if ((you.species == SP_HALFLING || you.species == SP_GNOME
             || you.species == SP_KOBOLD || you.species == SP_SPRIGGAN)
            && (you.inv_type[item_wield_2] == WPN_GREAT_SWORD
                || you.inv_type[item_wield_2] == WPN_TRIPLE_SWORD
                || you.inv_type[item_wield_2] == WPN_GREAT_MACE
                || you.inv_type[item_wield_2] == WPN_GREAT_FLAIL
                || you.inv_type[item_wield_2] == WPN_BATTLEAXE
                || you.inv_type[item_wield_2] == WPN_EXECUTIONERS_AXE
                || you.inv_type[item_wield_2] == WPN_HALBERD
                || you.inv_type[item_wield_2] == WPN_GLAIVE
                || you.inv_type[item_wield_2] == WPN_GIANT_CLUB
                || you.inv_type[item_wield_2] == WPN_GIANT_SPIKED_CLUB
                || you.inv_type[item_wield_2] == WPN_SCYTHE))
        {
            mpr("That's too large for you to wield.");
            return;             // halflings or kobolds

        }

        if (hands_required_for_weapon(
              you.inv_class[item_wield_2],
              you.inv_type[item_wield_2]   ) == HANDS_TWO_HANDED
            && you.equip[EQ_SHIELD] != -1)
        {
            mpr("You can't wield that with a shield.");
            return;
        }

        if ((you.is_undead != 0 || you.species == SP_DEMONSPAWN)
                && (you.inv_dam[item_wield_2] < 180
                    && (you.inv_dam[item_wield_2] % 30 == SPWPN_HOLY_WRATH
                    || you.inv_dam[item_wield_2] % 30 == SPWPN_DISRUPTION)))
        {
            mpr("This weapon will not allow you to wield it.");
            you.turn_is_over = 1;
            return;             // Mummies can't wield holy weapons

        }

        if (you.equip[EQ_WEAPON] != -1)
            unwield_item(you.equip[EQ_WEAPON]);
        you.equip[EQ_WEAPON] = item_wield_2;
    }

    strcpy(info, " ");
    /*                      strncpy(info, letters [item_wield_2], 1); */
    if (item_wield_2 <= 25)
        info[0] = item_wield_2 + 97;
    else
        info[0] = item_wield_2 + 39;

    //info[1] = 0;                /* This null-terminates it, right? */
    // BCR - Yeah, but this is clearer...
    info[1] = '\0';

    strcat(info, " - ");

    in_name(item_wield_2, 3, str_pass);
    strcat(info, str_pass);

    strcat(info, " (weapon in hand)");
    mpr(info);

    you.turn_is_over = 1;
    wield_change = 1;

    if (you.inv_plus[item_wield_2] > 80
                                && you.inv_class[item_wield_2] == OBJ_WEAPONS)
        mpr("It sticks to your hand!");

    if (you.inv_class[item_wield_2] == OBJ_MISCELLANY
                    && you.inv_type[item_wield_2] == MISC_LANTERN_OF_SHADOWS)
    {
        mpr("The area is filled with flickering shadows.");
        you.special_wield = SPWLD_SHADOW;
    }

    if (you.inv_class[item_wield_2] == OBJ_WEAPONS
                                        && you.inv_ident[item_wield_2] == 0)
        you.inv_ident[item_wield_2] = 2;

    if (you.inv_class[item_wield_2] == OBJ_WEAPONS
                                        && you.inv_dam[item_wield_2] != 0)
    {

        if (you.inv_ident[item_wield_2] == 1)
            you.inv_ident[item_wield_2] = 2;

        i_dam = you.inv_dam[item_wield_2];

        if (you.inv_dam[item_wield_2] <= 180)
        {
            i_dam = you.inv_dam[item_wield_2] % 30;
        }

        if (you.inv_dam[item_wield_2] % 30 >= 25)
        {
            i_dam = randart_wpn_properties(you.inv_class[item_wield_2], you.inv_type[item_wield_2], you.inv_dam [item_wield_2], you.inv_plus[item_wield_2], you.inv_plus2[item_wield_2] ,0, OBJ_WEAPONS);
        }

        switch (i_dam)          //you.inv_dam [item_wield_2] % 30)

        {
        case SPWPN_FLAMING:
            mpr("It bursts into flame!");
            break;

        case SPWPN_FREEZING:
            mpr("It glows with a cold blue light!");
            break;

        case SPWPN_HOLY_WRATH:
            mpr("It softly glows with a divine radiance!");
            break;

        case SPWPN_ELECTROCUTION:
            mpr("You hear the crackle of electricity.");
            break;

        case SPWPN_ORC_SLAYING:
            if (you.species == SP_HILL_ORC)
                mpr("You feel a sudden desire to commit suicide.");
            else
                mpr("You feel a sudden desire to kill orcs!");
            break;

        case SPWPN_VENOM:
            mpr("It begins to drip with poison!");
            break;

        case SPWPN_PROTECTION:
            mpr("You feel protected!");
            you.redraw_armor_class = 1;
            break;

        case SPWPN_DRAINING:
            mpr("You sense an unholy aura.");
            break;

        case SPWPN_SPEED:
            mpr("Your hands tingle!");
            break;

        case SPWPN_FLAME:
            mpr("It glows red for a moment.");
            break;

        case SPWPN_FROST:
            mpr("It is covered in frost.");
            break;

        case SPWPN_VAMPIRICISM:
            // you.hunger_inc += 6;
            mpr("You feel a strange hunger.");
            break;

        case SPWPN_DISRUPTION:
            mpr("You sense a holy aura.");
            break;

        case SPWPN_PAIN:
            mpr("A searing pain shoots up your arm!");
            break;

        case SPWPN_DISTORTION:
            miscast_effect(20, 9, 90, 100);
            break;

        case NWPN_SINGING_SWORD:
            mpr("The Singing Sword hums in delight!");
            you.special_wield = SPWLD_SING;
            break;

        case NWPN_WRATH_OF_TROG:
            mpr("You feel bloodthirsty!");
            you.special_wield = SPWLD_TROG;
            break;

        case NWPN_SCYTHE_OF_CURSES:
            mpr("A shiver runs down your spine.");
            you.special_wield = SPWLD_CURSE;
            break;

        case NWPN_MACE_OF_VARIABILITY:
            you.special_wield = SPWLD_VARIABLE;
            break;

        case NWPN_GLAIVE_OF_PRUNE:
            mpr("You feel pruney.");
            you.special_wield = SPWLD_NONE;
            break;

        case NWPN_SCEPTRE_OF_TORMENT:
            mpr("A terribly searing pain shoots up your arm!");
            you.special_wield = SPWLD_TORMENT;
            break;

        case NWPN_SWORD_OF_ZONGULDROK:
            mpr("You sense an extremely unholy aura.");
            you.special_wield = SPWLD_ZONGULDROK;
            break;

        case NWPN_SWORD_OF_POWER:
            mpr("You sense an aura of extreme power.");
            you.special_wield = SPWLD_POWER;
            break;

        case NWPN_STAFF_OF_OLGREB:
            mpr("You smell chlorine.");
            /*                you.res_poison ++;
               you.spec_poison ++;
               you.mag_abil ++; */
            //  you.special_wield = 8;
            break;

        case NWPN_VAMPIRE_S_TOOTH:
            // you.hunger_inc += 9;
            mpr("You feel a strange hunger, and smell blood on the air...");
            break;

        case NWPN_STAFF_OF_WUCAD_MU:
            miscast_effect(19, 9, 90, 100);
            you.special_wield = SPWLD_WUCAD_MU;
            break;
        }

        if (you.inv_dam[item_wield_2] % 30 >= 25)
        {
            use_randart(item_wield_2);
        }

        if ((you.inv_type[item_wield_2] == WPN_DEMON_BLADE
             || you.inv_type[item_wield_2] == WPN_DEMON_WHIP
             || you.inv_type[item_wield_2] == WPN_DEMON_TRIDENT)
            && (you.religion == GOD_ZIN || you.religion == GOD_SHINING_ONE
                || you.religion == GOD_ELYVILON))
            mpr("You really shouldn't be using a nasty item like this.");

    }

    if (you.inv_class[item_wield_2] == OBJ_STAVES)
    {
        switch (you.inv_type[item_wield_2])
        {
        case STAFF_POWER:
            you.max_magic_points += 13;
            you.base_magic_points2 += 13;
            you.redraw_magic_points = 1;
            /* you.magic_battery ++; */
            calc_ep();

            if (you.inv_ident[you.equip[EQ_WEAPON]] < 3)
            {
                you.inv_ident[you.equip[EQ_WEAPON]] = 3;
                strcpy(info, "You are wielding ");
                in_name(you.equip[EQ_WEAPON], 3, str_pass);
                strcat(info, str_pass);
                strcat(info, ".");
                mpr(info);
                more();
                wield_change = 1;
            }
            break;
        }
    }

    /*if (you.inv_dam [item_wield_2] == 6)
       {
       strcpy(info, "You feel protected.");
       mpr(info);
       if (you.inv_ident [item_wield_2] == 1) you.inv_ident [item_wield_2] = 2;
       } */
    you.time_taken *= 5;
    you.time_taken /= 10;

    wield_change = 1;
}                               // end of wield_weapon


//---------------------------------------------------------------
//
// armour_prompt
//
// Prompt the user for some armor. Returns true if the user picked
// something legit.
//
//---------------------------------------------------------------
bool armour_prompt(const string & mesg, int *index)
{
    ASSERT(index != NULL);

    bool succeeded = false;

    if (you.num_inv_items == 0)
    {
        mpr("You aren't carrying anything.");
    }
    else if (you.berserker != 0)
    {
        mpr("You are too berserk!");
    }
    else
    {
      query:
        mpr(mesg.c_str());

        unsigned char key = get_ch();

        if (key == '?' || key == '*')
        {
            unsigned char letter = 0;

            if (key == '?')
                letter = get_invent(OBJ_ARMOUR);
            else
                letter = get_invent(-1);

            if ((letter >= 65 && letter <= 90) || (letter >= 97 && letter <= 122))
            {
                key = letter;
            }
            else
            {
                mesclr();
                goto query;
            }
        }

        if (key < 65 || (key > 90 && key < 97) || key > 122)
        {
            mpr("You don't have any such object.");
        }
        else
        {
            *index = conv_lett(key);
            succeeded = true;
        }
    }

    return succeeded;
}


//---------------------------------------------------------------
//
// wear_armour
//
//---------------------------------------------------------------
void wear_armour()
{
    char wh_equip = 0;

    int armour_wear_2;

    if (!armour_prompt("Wear which item?", &armour_wear_2))
        return;

    if (you.inv_class[armour_wear_2] != OBJ_ARMOUR)
    {
        mpr("You can't wear that thing.");
        return;
    }

    if (armour_wear_2 == you.equip[EQ_BODY_ARMOUR]
        || armour_wear_2 == you.equip[EQ_CLOAK]
        || armour_wear_2 == you.equip[EQ_SHIELD]
        || armour_wear_2 == you.equip[EQ_HELMET]
        || armour_wear_2 == you.equip[EQ_GLOVES]
        || armour_wear_2 == you.equip[EQ_BOOTS])
    {
        mpr("You are already wearing that!");
        return;
    }

    if (you.inv_quantity[armour_wear_2] == 0)
    {
        mpr("You don't have any such object.");
        return;
    }

    if (armour_wear_2 == you.equip[EQ_WEAPON])
    {
        mpr("You are wielding that object!");
        return;
    }

    // if you're wielding something,
    if (you.equip[EQ_WEAPON] != -1 &&

    // attempting to wear a shield,
        (you.inv_type[armour_wear_2] == ARM_SHIELD ||
         you.inv_type[armour_wear_2] == ARM_BUCKLER ||
         you.inv_type[armour_wear_2] == ARM_LARGE_SHIELD) &&

    // weapon is two-handed
        hands_required_for_weapon( you.inv_class[you.equip[EQ_WEAPON]],
                    you.inv_type[you.equip[EQ_WEAPON]] ) == HANDS_TWO_HANDED)
    {
        mpr("You can't wear a shield with a two-handed weapon.");
        return;
    }

    if (you.species != SP_NAGA && you.inv_type[armour_wear_2] == ARM_BOOTS && you.inv_plus2[armour_wear_2] == 1)
    {
        mpr("You can't wear that!");
        return;
    }

    if (you.species != SP_CENTAUR && you.inv_type[armour_wear_2] == ARM_BOOTS && you.inv_plus2[armour_wear_2] == 2)
    {
        mpr("You can't wear that!");
        return;
    }

    wh_equip = EQ_BODY_ARMOUR;
    switch (you.inv_type[armour_wear_2])
    {
    case ARM_BUCKLER:
    case ARM_LARGE_SHIELD:
    case ARM_SHIELD:
        wh_equip = EQ_SHIELD;
        break;
    case ARM_CLOAK:
        wh_equip = EQ_CLOAK;
        break;
    case ARM_HELMET:
        wh_equip = EQ_HELMET;
        break;
    case ARM_GLOVES:
        wh_equip = EQ_GLOVES;
        break;
    case ARM_BOOTS:
        wh_equip = EQ_BOOTS;
        break;
    }

    if (you.species == SP_NAGA && you.inv_type[armour_wear_2] == ARM_BOOTS
        && you.inv_plus2[armour_wear_2] == 1
        && (you.attribute[ATTR_TRANSFORMATION] == TRAN_NONE
            || you.attribute[ATTR_TRANSFORMATION] == TRAN_BLADE_HANDS
            || you.attribute[ATTR_TRANSFORMATION] == TRAN_LICH))
    {
        // it fits
    }
    else if (you.species == SP_CENTAUR
             && you.inv_type[armour_wear_2] == ARM_BOOTS
             && you.inv_plus2[armour_wear_2] == 2
             && (you.attribute[ATTR_TRANSFORMATION] == TRAN_NONE
                 || you.attribute[ATTR_TRANSFORMATION] == TRAN_BLADE_HANDS
                 || you.attribute[ATTR_TRANSFORMATION] == TRAN_LICH))
    {
        // it fits
    }
    else if (can_equip(wh_equip) == 0)
    {
        mpr("You can't wear that in your present form.");
        return;
    }

    // Giant races
    if (you.species >= SP_OGRE && you.species <= SP_UNK2_DRACONIAN)
    {
        if ((you.inv_type[armour_wear_2] >= ARM_LEATHER_ARMOUR
             && you.inv_type[armour_wear_2] <= ARM_PLATE_MAIL)

            || (you.inv_type[armour_wear_2] >= ARM_HELMET
                && you.inv_type[armour_wear_2] <= ARM_BUCKLER)

            || you.inv_type[armour_wear_2] == ARM_CRYSTAL_PLATE_MAIL)
        {
            mpr("This armour doesn't fit on your body.");
            return;
        }
    }

    // Tiny races
    if (you.species == SP_SPRIGGAN)
    {
        if ((you.inv_type[armour_wear_2] >= ARM_LEATHER_ARMOUR
             && you.inv_type[armour_wear_2] <= ARM_PLATE_MAIL)

            || (you.inv_type[armour_wear_2] >= ARM_HELMET
                && you.inv_type[armour_wear_2] <= ARM_BOOTS)

            || you.inv_type[armour_wear_2] == ARM_SHIELD
            || you.inv_type[armour_wear_2] == ARM_LARGE_SHIELD

            || you.inv_type[armour_wear_2] == ARM_CRYSTAL_PLATE_MAIL)
        {
            mpr("This armour doesn't fit on your body.");
            return;
        }
    }

    bool removedCloak = false;

    if ((you.inv_type[armour_wear_2] < ARM_SHIELD
         || you.inv_type[armour_wear_2] > ARM_LARGE_SHIELD)
        && you.equip[EQ_CLOAK] != -1)
    {
        if (you.inv_plus[you.equip[EQ_CLOAK]] <= 130)
        {
            removedCloak = true;
        }
        else
        {
            mpr("Your cloak prevents you from wearing the armour.");
            return;
        }
    }

    you.delay_t = 0;            // may be set in takeoff_armour

    if (you.inv_type[armour_wear_2] == ARM_CLOAK && you.equip[EQ_CLOAK] != -1)
    {
        if (!takeoff_armour(you.equip[EQ_CLOAK]))
            return;
    }

    if (you.inv_type[armour_wear_2] == ARM_HELMET && you.equip[EQ_HELMET] != -1)
    {
        if (!takeoff_armour(you.equip[EQ_HELMET]))
            return;
    }

    if (you.inv_type[armour_wear_2] == ARM_GLOVES && you.equip[EQ_GLOVES] != -1)
    {
        if (!takeoff_armour(you.equip[EQ_GLOVES]))
            return;
    }

    if (you.inv_type[armour_wear_2] == ARM_BOOTS && you.equip[EQ_BOOTS] != -1)
    {
        if (!takeoff_armour(you.equip[EQ_BOOTS]))
            return;
    }

    if ((you.inv_type[armour_wear_2] == ARM_SHIELD
         || you.inv_type[armour_wear_2] == ARM_LARGE_SHIELD
         || you.inv_type[armour_wear_2] == ARM_BUCKLER)
        && you.equip[EQ_SHIELD] != -1)
    {
        if (!takeoff_armour(you.equip[EQ_SHIELD]))
            return;
    }


    bool removedBodyArmour = false;

    if ((you.inv_type[armour_wear_2] < ARM_SHIELD
         || you.inv_type[armour_wear_2] > ARM_LARGE_SHIELD)
        && you.equip[EQ_BODY_ARMOUR] != -1)
    {
        if (!takeoff_armour(you.equip[EQ_BODY_ARMOUR]))
            return;
        removedBodyArmour = true;
    }

    if (you.inv_type[armour_wear_2] == ARM_CRYSTAL_PLATE_MAIL
        && (you.skills[SK_ARMOUR] < 10 || you.species == SP_KOBOLD
            || you.species == SP_HALFLING
            || you.species == SP_GNOME
            || (you.species >= SP_ELF
                && you.species <= SP_SLUDGE_ELF)))
    {
        mpr("This armour is very cumbersome.");
    }

    if (you.inv_type[armour_wear_2] == ARM_PLATE_MAIL
        && (you.skills[SK_ARMOUR] < 5
            || you.species == SP_HALFLING
            || you.species == SP_GNOME
            || (you.species >= SP_ELF
                && you.species <= SP_SLUDGE_ELF)
            || you.species == SP_KOBOLD))
    {
        mpr("This armour is uncomfortably cumbersome.");
    }

    if (you.inv_type[armour_wear_2] == ARM_BANDED_MAIL
        || you.inv_type[armour_wear_2] == ARM_SPLINT_MAIL
        && (you.skills[SK_ARMOUR] < 3
            || you.species == SP_GNOME
            || you.species == SP_HALFLING))
    {
        mpr("You find this armour uncomfortably cumbersome.");
    }

    if (you.inv_type[armour_wear_2] == ARM_CRYSTAL_PLATE_MAIL
        && (you.skills[SK_ARMOUR] < 8
            || you.species == SP_GNOME
            || you.species == SP_HALFLING
            || you.species == SP_SPRIGGAN
            || (you.species >= SP_ELF
                && you.species <= SP_SLUDGE_ELF)))
    {
        mpr("This armour is quite cumbersome.");
    }

    if ((you.inv_type[armour_wear_2] == ARM_STORM_DRAGON_HIDE
         || you.inv_type[armour_wear_2] == ARM_STORM_DRAGON_ARMOUR)
        && (you.skills[SK_ARMOUR] < 5))
    {
        mpr("This armour is quite cumbersome.");
    }

    if ((you.inv_type[armour_wear_2] == ARM_GOLD_DRAGON_HIDE
         || you.inv_type[armour_wear_2] == ARM_GOLD_DRAGON_ARMOUR)
        && (you.skills[SK_ARMOUR] < 7))
    {
        mpr("This armour is quite cumbersome.");
    }

    if (you.inv_type[armour_wear_2] < ARM_SHIELD
        || you.inv_type[armour_wear_2] > ARM_LARGE_SHIELD)
    {
        you.equip[EQ_BODY_ARMOUR] = armour_wear_2;

        if (you.duration[DUR_ICY_ARMOUR] != 0)
        {
            mpr("Your icy armour melts away.");
            you.redraw_armor_class = 1;
            you.duration[DUR_ICY_ARMOUR] = 0;
        }
    }
    else
    {
        switch (you.inv_type[armour_wear_2])
        {
        case ARM_BUCKLER:
        case ARM_LARGE_SHIELD:
        case ARM_SHIELD:
            you.equip[EQ_SHIELD] = armour_wear_2;
            break;
        case ARM_CLOAK:
            you.equip[EQ_CLOAK] = armour_wear_2;
            break;
        case ARM_HELMET:
            you.equip[EQ_HELMET] = armour_wear_2;
            break;
        case ARM_GLOVES:
            you.equip[EQ_GLOVES] = armour_wear_2;
            break;
        case ARM_BOOTS:
            you.equip[EQ_BOOTS] = armour_wear_2;
            break;
        }
    }

    you.redraw_armor_class = 1;
    you.redraw_evasion = 1;
    you.turn_is_over = 1;
    if (you.inv_type[armour_wear_2] != ARM_SHIELD)
    {
        you.delay_t += property(you.inv_class[armour_wear_2],
                                you.inv_type[armour_wear_2], 0);
    }

    you.delay_doing = 1;

    if (you.inv_ident[armour_wear_2] < 3)
        you.inv_ident[armour_wear_2] = 3;

    if (removedCloak)
    {
        strcpy(info, "Your cloak hinders you, but you put on ");

        // if we are exhanging body armor then we've already
        // accounted for taking the cloak on and off
        if (!removedBodyArmour)
            you.delay_t += 2 * property(you.inv_class[you.equip[EQ_CLOAK]], you.inv_type[you.equip[EQ_CLOAK]], 0);
    }
    else
        strcpy(info, "You put on ");

    in_name(armour_wear_2, 5, str_pass);
    strcat(info, str_pass);
    strcat(info, ".");
    mpr(info);

    switch (you.inv_type[armour_wear_2])
    {
    case ARM_TROLL_LEATHER_ARMOUR:
        break;

    case ARM_STORM_DRAGON_ARMOUR:
        // you.attribute[ATTR_RESIST_LIGHTNING]++;
        break;
    }

    if (you.inv_dam[armour_wear_2] != SPARM_NORMAL)
    {
        switch (you.inv_dam[armour_wear_2] % 30)
        {
        case SPARM_RUNNING:
            if (you.species != SP_NAGA)
                mpr("You feel quick on your feet.");
            else
                mpr("You feel quick.");
            break;

        case SPARM_FIRE_RESISTANCE:
            mpr("You feel resistant to fire.");
            break;

        case SPARM_COLD_RESISTANCE:
            mpr("You feel resistant to cold.");
            break;

        case SPARM_POISON_RESISTANCE:
            mpr("You feel healthy.");
            break;

        case SPARM_SEE_INVISIBLE:
            mpr("You feel perceptive.");
            break;

        case SPARM_DARKNESS:
            if (you.invis == 0)
                mpr("You become transparent for a moment.");
            break;

        case SPARM_STRENGTH:
            mpr("You feel stronger.");
            you.strength += 3;
            you.max_strength += 3;
            you.redraw_strength = 1;
            break;

        case SPARM_DEXTERITY:
            mpr("You feel dextrous.");
            you.dex += 3;
            you.max_dex += 3;
            you.redraw_dexterity = 1;
            break;

        case SPARM_INTELLIGENCE:
            mpr("You feel intelligent.");
            you.intel += 3;
            you.max_intel += 3;
            you.redraw_intelligence = 1;
            break;

        case SPARM_PONDEROUSNESS:
            mpr("You feel rather ponderous.");
            you.speed += 2;
            you.redraw_evasion = 1;
            break;

        case SPARM_LEVITATION:
            mpr("You feel light on your feet.");
            break;

        case SPARM_MAGIC_RESISTANCE:
            mpr("You feel resistant to magic.");
            break;

        case SPARM_PROTECTION:
            mpr("You feel protected.");
            break;

        case SPARM_STEALTH:
            mpr("You feel stealthy.");
            break;

        case SPARM_RESISTANCE:
            mpr("You feel resistant to extremes of temperature.");
            break;

        case SPARM_POSITIVE_ENERGY:
            mpr("Your life-force is being protected.");
            break;

        case SPARM_ARCHMAGI:
            if (you.skills[SK_SPELLCASTING] == 0)
                mpr("You feel strangely numb.");
            else
                mpr("You feel extremely powerful.");
            break;
        }
    }

    if (you.inv_dam[armour_wear_2] % 30 >= 25)
        use_randart(armour_wear_2);
}                               // end of wear_armour


bool takeoff_armour(int item)
{
    if (you.inv_class[item] != OBJ_ARMOUR)
    {
        mpr("You aren't wearing that!");
        return false;
    }

    if (you.inv_plus[item] > 130 &&
        (item == you.equip[EQ_BODY_ARMOUR] || item == you.equip[EQ_SHIELD] ||
         item == you.equip[EQ_HELMET] || item == you.equip[EQ_GLOVES] ||
         item == you.equip[EQ_CLOAK] || item == you.equip[EQ_BOOTS]))
    {
        strcpy(info, "The ");
        in_name(item, 5, str_pass);
        strcat(info, str_pass);
        strcat(info, " is stuck to your body!");
        mpr(info);
        return false;
    }

    bool removedCloak = false;

    if (you.inv_type[item] < ARM_SHIELD || you.inv_type[item] > ARM_LARGE_SHIELD)
    {
        if (item != you.equip[EQ_BODY_ARMOUR])
        {
            mpr("You aren't wearing that!");
            return false;
        }

        if (you.equip[EQ_CLOAK] != -1)
        {
            if (you.inv_plus[you.equip[EQ_CLOAK]] <= 130)
            {
                removedCloak = true;
            }
            else
            {
                mpr("Your cloak prevents you from removing the armour.");
                return false;
            }
        }

        you.equip[EQ_BODY_ARMOUR] = -1;
    }
    else
    {
        switch (you.inv_type[item])
        {
        case ARM_BUCKLER:
        case ARM_LARGE_SHIELD:
        case ARM_SHIELD:
            if (item != you.equip[EQ_SHIELD])
            {
                mpr("You aren't wearing that!");
                return false;
            }
            else
                you.equip[EQ_SHIELD] = -1;
            break;

        case ARM_CLOAK:
            if (item != you.equip[EQ_CLOAK])
            {
                mpr("You aren't wearing that!");
                return false;
            }
            else
                you.equip[EQ_CLOAK] = -1;
            break;

        case ARM_HELMET:
            if (item != you.equip[EQ_HELMET])
            {
                mpr("You aren't wearing that!");
                return false;
            }
            else
                you.equip[EQ_HELMET] = -1;
            break;


        case ARM_GLOVES:
            if (item != you.equip[EQ_GLOVES])
            {
                mpr("You aren't wearing that!");
                return false;
            }
            else
                you.equip[EQ_GLOVES] = -1;
            break;

        case ARM_BOOTS:
            if (item != you.equip[EQ_BOOTS])
            {
                mpr("You aren't wearing that!");
                return false;
            }
            else
                you.equip[EQ_BOOTS] = -1;
            break;
        }
    }

    you.turn_is_over = 1;
    you.delay_t = property(you.inv_class[item], you.inv_type[item], 0);
    you.delay_doing = 2;

    if (removedCloak)
    {
        you.delay_t += 2 * property(you.inv_class[you.equip[EQ_CLOAK]], you.inv_type[you.equip[EQ_CLOAK]], 0);
        strcpy(info, "Your cloak hinders you, but you take off ");
    }
    else
        strcpy(info, "You take off ");

    in_name(item, 5, str_pass);
    strcat(info, str_pass);
    strcat(info, ".");
    mpr(info);

    unwear_armour(item);

    return true;
}                               // takeoff_armour


void throw_anything()
{
    unsigned char nthing = 0;
    struct bolt beam[1];

    if (you.berserker != 0)
    {
        mpr("You are too berserk!");
        return;
    }

    if (you.num_inv_items == 0)
    {
        mpr("You aren't carrying anything.");
        return;
    }

  query:
    mpr("Throw which item?");

    unsigned char keyin = get_ch();

    if (keyin == '?' || keyin == '*')
    {
        if (keyin == '?')
        {
            nthing = get_invent(OBJ_MISSILES);
        }
        if (keyin == '*')
            nthing = get_invent(-1);
        if ((nthing >= 65 && nthing <= 90) || (nthing >= 97 && nthing <= 122))
        {
            keyin = nthing;
        }
        else
        {
            mesclr();
            goto query;
        }
    }

    int throw_1 = (int) keyin;

    if (throw_1 < 65 || (throw_1 > 90 && throw_1 < 97) || throw_1 > 122)
    {
        mpr("You don't have any such object.");
        return;
    }

    int throw_2 = conv_lett(throw_1);

    if (throw_2
        == you.equip[EQ_LEFT_RING]
        || throw_2
        == you.equip[EQ_RIGHT_RING]
        || throw_2
        == you.equip[EQ_AMULET] ||
        throw_2 ==
        you.
        equip[EQ_BODY_ARMOUR] ||
        throw_2 ==
        you.
        equip[EQ_SHIELD] ||
        throw_2 ==
        you.
        equip[EQ_HELMET] ||
        throw_2 ==
        you.
        equip[EQ_CLOAK] ||
        throw_2 ==
        you.
        equip[EQ_BOOTS] ||
        throw_2 ==
        you.
        equip[EQ_GLOVES])
    {
        mpr("You are wearing that object!");
        return;
    }

    if (throw_2 == you.equip[EQ_WEAPON] && you.inv_plus[you.equip[EQ_WEAPON]] > 80)
    {
        mpr("That thing is stuck to your hand!");
        return;
    }

    if (you.inv_quantity[throw_2] == 0)
    {
        mpr("You don't have any such object.");
        return;
    }

    throw_it(beam, throw_2);
}                               /* end of throw_anything */


void shoot_thing()
{
    struct bolt beam[1];

    int i;
    int throw_2 = 0;

    if (you.berserker != 0)
    {
        mpr("You are too berserk!");
        return;
    }

    int type_wanted = 3;

    if (you.equip[EQ_WEAPON] == -1)
        goto find;

    if (you.inv_class[you.equip[EQ_WEAPON]] != OBJ_WEAPONS
                || you.inv_type[you.equip[EQ_WEAPON]] < WPN_SLING
                || you.inv_type[you.equip[EQ_WEAPON]] > WPN_CROSSBOW)
    {
        type_wanted = 3;
    }
    else
        type_wanted = you.inv_type[you.equip[EQ_WEAPON]] - 13;

  find:
    for (i = 0; i < 53; i++)
    {
        if (i == 52)
        {
            mpr("You have nothing suitable.");
            return;
        }
        if (you.inv_quantity[i] == 0)
            continue;
        if (you.inv_class[i] == OBJ_MISSILES && type_wanted == you.inv_type[i])
        {
            throw_2 = i;
            break;
        }
    }

    throw_it(beam, throw_2);
}                               /* end shoot_thing */


void throw_it(struct bolt beam[1], int throw_2)
{
    char shoot = 0;

    struct dist thr[1];

    char shoot_skill = 0;

    mpr("Which direction? (*/+ to target)");

    if (you.prev_targ != MHITNOT && you.prev_targ < MNST)
        if (mons_near(you.prev_targ) && (menv[you.prev_targ].enchantment[2] != 6 || player_see_invis() != 0))
        {
            strcpy(info, "You are currently targetting ");
            strcat(info, monam(menv[you.prev_targ].number, menv[you.prev_targ].type, menv[you.prev_targ].enchantment[
                                                                           2
                               ]
                               ,1));
            strcat(info, " (p to target).");
            mpr(info);
        }
        else
            mpr("You have no current target.");

    //direction(1);
    direction(1, thr);

    if (thr[0].nothing == -1)
        return;


    if (you.conf != 0)
    {
        thr[0].move_x = random2(13) - 6;
        thr[0].move_y = random2(13) - 6;
    }

    beam[0].move_x = thr[0].move_x;
    beam[0].move_y = thr[0].move_y;
    beam[0].target_x = thr[0].target_x;
    beam[0].target_y = thr[0].target_y;

    beam[0].range = 5;          /* provisional until mass variable used */

    beam[0].flavour = 0;

    switch (you.inv_class[throw_2])
    {
    case OBJ_WEAPONS:
        beam[0].type = 41;
        beam[0].range = 9;
        break;
    case OBJ_MISSILES:
        beam[0].type = 41;
        beam[0].range = 9;
        break;
    case OBJ_ARMOUR:
        beam[0].type = 91;
        break;
    case OBJ_WANDS:
        beam[0].type = 47;
        break;
    case OBJ_FOOD:
        beam[0].type = 37;
        break;
    case 5:
        beam[0].type = 43;
        break;
    case OBJ_SCROLLS:
        beam[0].type = 63;
        break;
    case OBJ_JEWELLERY:
        beam[0].type = 61;
        break;
    case OBJ_POTIONS:
        beam[0].type = 33;
        break;
    case 9:
        beam[0].type = 42;
        break;
    case OBJ_BOOKS:
        beam[0].type = 40;
        break;
    case 11:
        beam[0].type = 37;
        break;
    }

    beam[0].source_x = you.x_pos;
    beam[0].source_y = you.y_pos;
    beam[0].colour = you.inv_colour[throw_2];
    item_name(you.inv_plus2[throw_2], you.inv_class[throw_2], you.inv_type[throw_2], you.inv_dam[throw_2], you
              .inv_plus[throw_2], 1, you.inv_ident[throw_2], 6, str_pass);
    /*in_name(throw_2, 6);    <-- Don't use this: "the 10 +0 darts hit the x" */
    strcpy(beam[0].beam_name, str_pass);
    beam[0].thing_thrown = 1;


    beam[0].hit = (random2(you.dex)) / 2;

    beam[0].hit += random2(you.skills[SK_THROWING] + 1);
    beam[0].hit += slaying_bonus(0);
    beam[0].damage += random2(you.skills[SK_THROWING] + 1);
    beam[0].damage += slaying_bonus(1);

    if (you.inv_class[throw_2] == OBJ_MISSILES
                                    && you.inv_type[throw_2] == MI_DART)

    {
        if (you.equip[EQ_WEAPON] == -1
                    || you.inv_class[you.equip[EQ_WEAPON]] != OBJ_WEAPONS
                    || you.inv_type[you.equip[EQ_WEAPON]] != WPN_HAND_CROSSBOW)
        {
            exercise(SK_DARTS, 1 + random2(2) + random2(2));
            exercise(SK_THROWING, random2(2));
            beam[0].hit += random2(you.skills[SK_DARTS] + 1)
                                    + random2(you.skills[SK_DARTS] + 1)
                                    + random2(you.skills[SK_THROWING] / 2 + 1);
        }
    }

    if (you.inv_class[throw_2] == OBJ_WEAPONS ||
                                    you.inv_class[throw_2] == OBJ_MISSILES)
    {
        beam[0].damage = property(you.inv_class[throw_2],
                                                    you.inv_type[throw_2], 0);

        /*      Missile devices eg bows */
        if (you.inv_class[you.equip[EQ_WEAPON]] == OBJ_WEAPONS
            && you.inv_type[you.equip[EQ_WEAPON]] == you.inv_type[throw_2] + 13
            && you.inv_type[you.equip[EQ_WEAPON]] <= WPN_HAND_CROSSBOW)
        {
            beam[0].damage = property(you.inv_class[throw_2],
                                                    you.inv_type[throw_2], 1);

            if (you.inv_type[you.equip[EQ_WEAPON]] == WPN_CROSSBOW)
            {
                you.time_taken *= 12;
                you.time_taken /= 10;
            }

            int hoggl = you.inv_plus2[you.equip[EQ_WEAPON]] - 50;

            if (you.inv_plus2[you.equip[EQ_WEAPON]] > 80)
                hoggl -= 100;

            beam[0].damage += hoggl;
            beam[0].range += 5;

            // removed 2 random2(2)s from each of the learning curves, but
            // left slings because they're hard enough to develop without
            // a good source of shot in the dungeon.
            switch (you.inv_type[you.equip[EQ_WEAPON]])
            {
            case WPN_SLING:     /* sling */
                // Slings are really easy to learn because they're not
                // really all that good, and its harder to get ammo anyways.
                exercise(SK_SLINGS, 1 + random2(2) + random2(2));
                beam[0].hit += random2(you.skills[SK_SLINGS] + 1)
                                + random2(random2(you.skills[SK_SLINGS] + 1));
                beam[0].damage += random2(you.skills[SK_SLINGS] + 1);
                shoot_skill = you.skills[SK_SLINGS];
                break;

            case WPN_BOW:       /* bow */
                exercise(SK_BOWS, 1 + random2(2));
                beam[0].hit += random2(you.skills[SK_BOWS] + 1)
                                + random2(you.skills[SK_BOWS] + 1);
                beam[0].damage += random2(you.skills[SK_BOWS] + 1)
                                + random2(random2(you.skills[SK_BOWS] + 1));
                shoot_skill = you.skills[SK_BOWS];
                break;

            // Crossbows are easy for unskilled people so they get a
            // contant bonus added to hit and dam.  Note that the
            // values of 10 and 5 make bows better at skill level 10.

            case WPN_CROSSBOW:  /* crossbow */
                exercise(SK_CROSSBOWS, 1 + random2(2));
                beam[0].hit += random2(you.skills[SK_CROSSBOWS] + 1)
                                                                + random2(10);
                beam[0].damage += random2(you.skills[SK_CROSSBOWS] + 1)
                                                                + random2(5);
                shoot_skill = you.skills[SK_CROSSBOWS];
                break;

            case WPN_HAND_CROSSBOW:     /* hand crossbow */
                exercise(SK_CROSSBOWS, 1 + random2(2));
                beam[0].hit += random2(you.skills[SK_CROSSBOWS] + 1)
                                                                + random2(6);
                beam[0].damage += random2(you.skills[SK_CROSSBOWS] + 1)
                                                                + random2(3);
                shoot_skill = you.skills[SK_CROSSBOWS];
                break;
            }

            exercise(SK_THROWING, random2(2));

            int ghoggl = you.inv_plus[you.equip[EQ_WEAPON]] - 50;

            if (you.inv_plus[you.equip[EQ_WEAPON]] > 80)
                ghoggl -= 100;

            beam[0].hit += ghoggl;

            beam[0].range += 3;

            if (you.inv_dam[you.equip[EQ_WEAPON]] / 30
                                            == you.inv_dam[throw_2] / 30)
            {
                beam[0].hit++;
                beam[0].damage++;
                if (you.inv_dam[you.equip[EQ_WEAPON]] / 30 == DWPN_ELVEN
                     && you.species >= SP_ELF && you.species <= SP_SLUDGE_ELF)
                    beam[0].hit++;
            }


            if ((you.inv_dam[you.equip[EQ_WEAPON]] % 30 == SPWPN_FLAME
                        || you.inv_dam[throw_2] % 30 == SPWPN_FLAMING
                        || (you.inv_dam[you.equip[EQ_WEAPON]] % 30 >= 25
                        && randart_wpn_properties(
                                        you.inv_class[you.equip[EQ_WEAPON]],
                                        you.inv_type[you.equip[EQ_WEAPON]],
                                        you.inv_dam[you.equip[EQ_WEAPON]],
                                        you.inv_plus[you.equip[EQ_WEAPON]],
                                        you.inv_plus2 [you.equip [EQ_WEAPON]],
                                                0, RAP_BRAND) == SPWPN_FLAME))
                    && you.inv_dam[throw_2] % 30 != SPWPN_FREEZING
                    && you.inv_dam[you.equip[EQ_WEAPON]]% 30 != SPWPN_FROST)
            {
                beam[0].damage += 1 + random2(5);
                beam[0].flavour = 2;
                strcpy(beam[0].beam_name, "bolt of ");
                if (you.inv_dam[throw_2] % 30 == 3
                                        || you.inv_dam[throw_2] % 30 == 4)
                    strcat(beam[0].beam_name, "poison ");
                strcat(beam[0].beam_name, "flame");
                beam[0].colour = RED;
                beam[0].type = 35;
                beam[0].thing_thrown = 3;
                you.inv_ident[throw_2] = 2;
            }
            //                if (you.inv_dam [you.equip [0]] % 30 == 12)
            if ((you.inv_dam[you.equip [EQ_WEAPON]] % 30 == SPWPN_FROST
                        || you.inv_dam [throw_2] % 30 == SPWPN_FREEZING
                        || (you.inv_dam [you.equip [EQ_WEAPON]] % 30 >= 25
                        && randart_wpn_properties(
                                    you.inv_class [you.equip [EQ_WEAPON]],
                                    you.inv_type [you.equip [EQ_WEAPON]],
                                    you.inv_dam [you.equip [EQ_WEAPON]],
                                    you.inv_plus [you.equip [EQ_WEAPON]],
                                    you.inv_plus2 [you.equip [EQ_WEAPON]],
                                                0, RAP_BRAND) == SPWPN_FROST))

                    && you.inv_dam[throw_2] % 30 != SPWPN_FLAMING
                    && you.inv_dam[you.equip[EQ_WEAPON]] % 30 != SPWPN_FLAME)
            {
                beam[0].damage += 1 + random2(5);
                beam[0].flavour = 3;
                strcpy(beam[0].beam_name, "bolt of ");
                if (you.inv_dam[throw_2] % 30 == 3
                                        || you.inv_dam[throw_2] % 30 == 4)
                    strcat(beam[0].beam_name, "poison ");
                strcat(beam[0].beam_name, "frost");
                beam[0].colour = WHITE;
                beam[0].type = 35;
                beam[0].thing_thrown = 3;
                you.inv_ident[throw_2] = 2;
            }
            /* the chief advantage here is the extra damage this does against susceptible creatures */

            /* Note: weapons & ammo of eg fire are not cumulative
               ammo of fire and weapons of frost don't work together, and vice versa */

            if (you.inv_plus[you.equip[EQ_WEAPON]] > 80)
            {
                beam[0].damage -= 100;
                beam[0].hit -= 100;
            }

            shoot = 1;

            if (you.inv_ident[you.equip[EQ_WEAPON]] < 3
                                        && random2(100) < shoot_skill)
            {
                you.inv_ident[you.equip[EQ_WEAPON]] = 3;
                strcpy(info, "You are wielding ");
                in_name(you.equip[EQ_WEAPON], 3, str_pass);
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


        int hoggl = you.inv_plus[throw_2] - 50;

        if (you.inv_plus[throw_2] > 80)
            hoggl -= 100;
        //damage_done += you.inv_plus [throw_2];
        hoggl = (hoggl % 10);
        beam[0].damage += hoggl;


        int ghoggl = you.inv_plus[throw_2] - 50;

        if (you.inv_plus[throw_2] > 80)
            ghoggl -= 100;
        //damage_done += you.inv_plus [throw_2];
        ghoggl = (ghoggl / 10);
        beam[0].hit += ghoggl;


        if (you.inv_class[throw_2] == OBJ_WEAPONS)
        {
            if (you.inv_type[throw_2] == WPN_DAGGER
                            || you.inv_type[throw_2] == WPN_HAND_AXE
                            || you.inv_type[throw_2] == WPN_SPEAR)
            {
                beam[0].hit += random2( you.skills[SK_THROWING] / 2 + 1);
                beam[0].damage += 1;
                exercise(SK_THROWING, random2(2));
            }
            else
            {
                beam[0].damage -= 1;
                beam[0].hit -= 3;
            }
        }
    }
    else
        beam[0].damage = 2;     /* later put mass thing here. */


    if (shoot == 0)
        strcpy(info, "You throw ");
    /* if shoot == 1, it has already strcpy'd "You shoot " */
    item_name(you.inv_plus2[throw_2], you.inv_class[throw_2],
                you.inv_type[throw_2], you.inv_dam[throw_2],
                you.inv_plus[throw_2], 1, you.inv_ident[throw_2], 3, str_pass);

    strcat(info, str_pass);
    strcat(info, ".");
    mpr(info);

    if (you.equip[EQ_WEAPON] == throw_2)
        unwield_item(throw_2);

    missile(beam, throw_2);
    you.inv_quantity[throw_2]--;
    if (you.inv_quantity[throw_2] == 0)
    {
        you.num_inv_items--;
        if (you.equip[EQ_WEAPON] == throw_2)
        {
            you.equip[EQ_WEAPON] = -1;
            mpr("You are now empty handed.");
        }
    }

    burden_change();

    /*move_x = 0; move_y = 0; */

    alert();

    you.turn_is_over = 1;
}                               // end of void throw_it()


void puton_ring()
{
    unsigned char nthing = 0;

    if (you.num_inv_items == 0)
    {
        mpr("You aren't carrying anything.");
        return;
    }

    if (you.berserker != 0)
    {
        mpr("You are too berserk!");
        return;
    }

  query:
    mpr("Put on which item?");

    unsigned char keyin = get_ch();

    if (keyin == '?' || keyin == '*')
    {
        if (keyin == '?')
            nthing = get_invent(OBJ_JEWELLERY);
        if (keyin == '*')
            nthing = get_invent(-1);
        if ((nthing >= 65 && nthing <= 90) || (nthing >= 97 && nthing <= 122))
        {
            keyin = nthing;
        }
        else
        {
            mesclr();
            goto query;

        }
    }
    int ring_wear_1 = (int) keyin;

    if (ring_wear_1 < 65 || (ring_wear_1 > 90 && ring_wear_1 < 97)
        || ring_wear_1 > 122)
    {
        mpr("You don't have any such object.");
        return;
    }

    int ring_wear_2 = conv_lett(ring_wear_1);

    if (ring_wear_2 == you.equip[EQ_LEFT_RING]
        || ring_wear_2 == you.equip[EQ_RIGHT_RING]
        || ring_wear_2 == you.equip[EQ_AMULET])
    {
        mpr("You are already wearing that!");
        return;
    }

    if (you.inv_quantity[ring_wear_2] == 0)
    {
        mpr("You don't have any such object.");
        return;
    }

    if (ring_wear_2 == you.equip[EQ_WEAPON])
    {
        mpr("You are wielding that object.");
        return;
    }

    if (you.inv_class[ring_wear_2] != OBJ_JEWELLERY)
    {
        mpr("I think there's some kind of mistake here.");
        return;
    }

    if (you.inv_type[ring_wear_2] < AMU_RAGE)
    {
        if (you.equip[EQ_GLOVES] != -1 && you.inv_plus[you.equip[EQ_GLOVES]] > 80)
        {
            mpr("You can't take your gloves off to wear a ring!");
            return;
        }

        if (you.inv_class[ring_wear_2] == OBJ_JEWELLERY
            && you.equip[EQ_LEFT_RING] != -1
            && you.equip[EQ_RIGHT_RING] != -1)
        {
            // and you are trying to wear body you.equip.
            mpr("You are already wearing a ring on each hand.");
            return;
        }
    }
    else if (you.equip[EQ_AMULET] != -1)
    {
        strcpy(info, "You are already wearing an amulet.");
        if (!random2(20))
        {
            strcat(info, " And I must say it looks quite fetching.");
        }
        mpr(info);
        return;
    }

    int hand_used = 0;

    if (you.equip[EQ_LEFT_RING] != -1)
        hand_used = 1;
    if (you.equip[EQ_RIGHT_RING] != -1)
        hand_used = 0;

    if (you.inv_type[ring_wear_2] >= AMU_RAGE)
        hand_used = 2;
    else if (you.equip[EQ_LEFT_RING] == -1 && you.equip[EQ_RIGHT_RING] == -1)
    {
        mpr("Put on which hand (l or r)?");

        keyin = get_ch();

        if (keyin != 'r' && keyin != 'l')
        {
            mpr("You don't have such a hand!");
            return;
        }

        if (keyin == 'l')
            hand_used = 0;
        if (keyin == 'r')
            hand_used = 1;
    }

    you.equip[hand_used + 7] = ring_wear_2;

    switch (you.inv_type[ring_wear_2])
    {

    case RING_REGENERATION:     /* you.ring of regeneration */
        /*      you.rate_regen += 100; */
        // you.hunger_inc += 2;
        if (you.hp < you.hp_max)
            set_id(you.inv_class[ring_wear_2], you.inv_type[ring_wear_2], 1);   // this is a bit dodgy

        break;

    case RING_PROTECTION:       /* protection */
        /*      if (you.inv_plus [ring_wear_2] > 130) player_AC() -= 100;
           player_AC() += you.inv_plus [ring_wear_2];
           player_AC() -= 50; */
        you.redraw_armor_class = 1;
        //      you.hunger_inc += 1;
        if (you.inv_plus[ring_wear_2] != 0 && you.inv_plus[ring_wear_2] != 100)
        {
            set_id(you.inv_class[ring_wear_2], you.inv_type[ring_wear_2], 1);
            you.inv_ident[ring_wear_2] = 3;
        }
        break;

    case RING_PROTECTION_FROM_FIRE:     /* fire resistance */
        /*      player_res_fire() += 1; */
        //      you.hunger_inc += 1;
        break;

    case RING_POISON_RESISTANCE:        // you.poison resistance
        /*      you.res_poison += 1; */
        ///     you.hunger_inc += 1;
        break;

    case RING_PROTECTION_FROM_COLD:     // cold resistance
        //      you.hunger_inc += 1;

        break;

    case RING_STRENGTH: // increase you.strength

        you.strength += (you.inv_plus[ring_wear_2] % 100) - 50;
        you.max_strength += (you.inv_plus[ring_wear_2] % 100) - 50;
        you.redraw_strength = 1;
        //      you.hunger_inc += 1;
        burden_change();
        if (you.inv_plus[ring_wear_2] != 0 && you.inv_plus[ring_wear_2] != 100)
        {
            set_id(you.inv_class[ring_wear_2], you.inv_type[ring_wear_2], 1);
            you.inv_ident[ring_wear_2] = 3;
        }
        break;

    case RING_SLAYING:          // slaying
        /*      you.f_abil += 30; */
        //      you.hunger_inc += 1;
        break;

    case RING_SEE_INVISIBLE:    // see invisible
        /*      you.see_invis --; */
        //      you.hunger_inc += 1;
        break;

    case RING_INVISIBILITY:     // invisibility
        //      if (you.invis > 0) you.invis = 0;

        if (you.invis == 0)
        {
            mpr("You become transparent for a moment.");
        }
        set_id(you.inv_class[ring_wear_2], you.inv_type[ring_wear_2], 1);
        you.inv_ident[ring_wear_2] = 3;
        //      you.invis --;
        //      you.hunger_inc += 4;
        break;

    case RING_HUNGER:           // you.hunger

        // you.hunger_inc += 4;
        break;

    case RING_TELEPORTATION:    // teleportation
        //      you.hunger_inc += 1;
        /*      player_teleport() += 1; */
        break;

    case RING_EVASION:          // player_evasion()
        //      you.hunger_inc += 1;
        /*      if (you.inv_plus [ring_wear_2] > 130) player_evasion() -= 100;
           player_evasion() += you.inv_plus [ring_wear_2];
           player_evasion() -= 50; */
        you.redraw_evasion = 1;
        if (you.inv_plus[ring_wear_2] != 0 && you.inv_plus[ring_wear_2] != 100)
        {
            you.inv_ident[ring_wear_2] = 3;
            set_id(you.inv_class[ring_wear_2], you.inv_type[ring_wear_2], 1);
        }
        break;

    case RING_SUSTAIN_ABILITIES:
        // you.hunger_inc += 1;
        /* you.sust_abil++; */
        break;

    case RING_SUSTENANCE:
        // you.hunger_inc -= 2;
        break;

    case RING_DEXTERITY:        // increase you.dex

        you.dex += (you.inv_plus[ring_wear_2] % 100) - 50;
        you.max_dex += (you.inv_plus[ring_wear_2] % 100) - 50;
        you.redraw_dexterity = 1;
        //      you.hunger_inc += 1;
        if (you.inv_plus[ring_wear_2] != 0 && you.inv_plus[ring_wear_2] != 100)
        {
            you.inv_ident[ring_wear_2] = 3;
            set_id(you.inv_class[ring_wear_2], you.inv_type[ring_wear_2], 1);
        }
        break;

    case RING_INTELLIGENCE:     // increase you.intel

        you.intel += (you.inv_plus[ring_wear_2] % 100) - 50;
        you.max_intel += (you.inv_plus[ring_wear_2] % 100) - 50;
        you.redraw_intelligence = 1;
        //      you.hunger_inc += 1;
        if (you.inv_plus[ring_wear_2] != 0 && you.inv_plus[ring_wear_2] != 100)
        {
            you.inv_ident[ring_wear_2] = 3;
            set_id(you.inv_class[ring_wear_2], you.inv_type[ring_wear_2], 1);
        }
        break;

    case RING_WIZARDRY: // wizardry
        /* you.mag_abil += 3; */
        // you.hunger_inc += 1;
        break;

    case RING_MAGICAL_POWER:    // magic

        you.max_magic_points += 9;
        you.base_magic_points2 += 9;
        // you.hunger_inc ++;
        you.redraw_magic_points = 1;
        /* you.magic_battery ++; */
        calc_ep();
        set_id(you.inv_class[ring_wear_2], you.inv_type[ring_wear_2], 1);
        break;

    case RING_LEVITATION:       // levitation
        /*      if (you.levitation > 0) you.levitation = 0;
           else if (you.levitation == 0)
           {
           strcpy(info, "You feel very buoyant!");
           mpr(info);
           strcpy(info, "You gently float upwards from the floor.");
           mpr(info);
           set_id(you.inv_class [ring_wear_2], you.inv_type [ring_wear_2], 1);
           you.inv_ident [ring_wear_2] = 3;
           }
           you.levitation --;
           you.hunger_inc += 1;
           burden_change(); */
        mpr("You feel buoyant.");
        set_id(you.inv_class[ring_wear_2], you.inv_type[ring_wear_2], 1);
        break;

    case RING_LIFE_PROTECTION:  // player_prot_life()
        //      you.hunger_inc ++;

        break;

    case RING_PROTECTION_FROM_MAGIC:    // magic resistance
        // you.hunger_inc += 1;

        break;

    case RING_FIRE:             // fire
        /* player_res_fire() += 1;
           you.spec_fire += 1;
           player_res_cold() -= 1; */
        // you.hunger_inc += 1;
        break;

    case RING_ICE:              // ice
        /* player_res_fire() -= 1;
           you.spec_cold += 1;
           player_res_cold() += 1; */
        // you.hunger_inc += 1;
        break;

    case RING_TELEPORT_CONTROL: // teleport control

        you.attribute[ATTR_CONTROL_TELEPORT]++;
        // you.hunger_inc += 1;
        break;


        /* all of the amulets so far existing are not needed here, but have their
           presence queried whenever relevant through wearing_amulet in fn2.cc */
    }

    you.turn_is_over = 1;

    if (you.inv_dam[ring_wear_2] == 200 || you.inv_dam[ring_wear_2] == 201)
    {
        use_randart(ring_wear_2);
    }

    if (you.inv_plus[ring_wear_2] > 80)         // cursed

    {
        mpr("Oops, that ring feels deathly cold.");

        if (you.inv_ident[ring_wear_2] == 0)
        {
            you.inv_ident[ring_wear_2] = 1;
        }
    }

    strcpy(info, " ");
    /*                      strncpy(info, letters [ring_wear_2], 1); */
    if (ring_wear_2 <= 25)
        info[0] = ring_wear_2 + 97;
    else
        info[0] = ring_wear_2 + 39;

    info[1] = 0;

    strcat(info, " - ");

    item_name(you.inv_plus2[ring_wear_2], you.inv_class[ring_wear_2],
              you.inv_type[ring_wear_2], you.inv_dam[ring_wear_2],
              you.inv_plus[ring_wear_2], you.inv_quantity[ring_wear_2],
              you.inv_ident[ring_wear_2], 3, str_pass);

    strcat(info, str_pass);

    if (hand_used == 0)
        strcat(info, " (on left hand)");
    if (hand_used == 1)
        strcat(info, " (on right hand)");
    if (hand_used == 2)
        strcat(info, " (around neck)");
    mpr(info);
}                               // end of puton_ring()


void remove_ring()
{
    unsigned char nthing = 0;
    int hand_used = 10;
    unsigned char keyin;
    int ring_wear_2;

    if (you.equip[EQ_LEFT_RING] == -1 && you.equip[EQ_RIGHT_RING] == -1 && you.equip[EQ_AMULET] == -1)
    {
        mpr("You aren't wearing any rings or amulets.");
        return;
    }

    if (you.berserker != 0)
    {
        mpr("You are too berserk!");
        return;
    }

    if (you.equip[EQ_GLOVES] != -1 && you.inv_plus[you.equip[EQ_GLOVES]] > 80 && you.equip[EQ_AMULET] == -1)
    {
        mpr("You can't take your gloves off to remove any rings!");
        return;
    }

    if (you.equip[EQ_LEFT_RING] != -1 && you.equip[EQ_RIGHT_RING] == -1 && you.equip[EQ_AMULET] == -1)
        hand_used = 0;
    if (you.equip[EQ_LEFT_RING] == -1 && you.equip[EQ_RIGHT_RING] != -1 && you.equip[EQ_AMULET] == -1)
        hand_used = 1;
    if (you.equip[EQ_LEFT_RING] == -1 && you.equip[EQ_RIGHT_RING] == -1 && you.equip[EQ_AMULET] != -1)
        hand_used = 2;

    if (hand_used == 10)
    {
      query:
        mpr("Remove which piece of jewellery?");

        keyin = get_ch();

        if (keyin == '?' || keyin == '*')
        {
            if (keyin == '?')
                nthing = get_invent(OBJ_JEWELLERY);
            if (keyin == '*')
                nthing = get_invent(-1);
            if ((nthing >= 65 && nthing <= 90) || (nthing >= 97 && nthing <= 122))
            {
                keyin = nthing;
            }
            else
            {
                mesclr();
                goto query;
            }
        }

        int equipn = conv_lett((int) keyin);

        if (you.inv_quantity[equipn] == 0)
        {
            mpr("You don't have such an object.");
            return;
        }

        if (you.inv_class[equipn] != OBJ_JEWELLERY)
        {
            mpr("That isn't a piece of jewelry.");
            return;
        }
        if (you.equip[EQ_LEFT_RING] == equipn)
            hand_used = 0;
        else if (you.equip[EQ_RIGHT_RING] == equipn)
            hand_used = 1;
        else if (you.equip[EQ_AMULET] == equipn)
            hand_used = 2;
        else
        {
            mpr("You aren't wearing that.");
            return;
        }
    }

    if (you.equip[EQ_GLOVES] != -1 && you.inv_plus[you.equip[EQ_GLOVES]] > 80 && hand_used < 2)
    {
        mpr("You can't take your gloves off to remove any rings!");
        return;
    }

    if (you.equip[hand_used + 7] == -1)
    {
        mpr("I don't think you really meant that.");
        return;
    }

    if (you.inv_plus[you.equip[hand_used + 7]] > 80)    // cursed

    {
        mpr("It's stuck to you!");
        if (you.inv_ident[you.equip[hand_used + 7]] == 0)
            you.inv_ident[you.equip[hand_used + 7]] = 1;
        return;
    }

    strcpy(info, "You remove your ");
    item_name(you.inv_plus2[you.equip[hand_used + 7]],
              you.inv_class[you.equip[hand_used + 7]],
              you.inv_type[you.equip[hand_used + 7]],
              you.inv_dam[you.equip[hand_used + 7]],
              you.inv_plus[you.equip[hand_used + 7]],
              you.inv_quantity[you.equip[hand_used + 7]],
              you.inv_ident[you.equip[hand_used + 7]], 8, str_pass);

    strcat(info, str_pass);
    strcat(info, ".");
    mpr(info);

    ring_wear_2 = you.equip[hand_used + 7];     // I'll still use ring_wear_2 here.


    switch (you.inv_type[ring_wear_2])
    {

    case RING_REGENERATION:     // regen

        // you.hunger_inc -= 2;
        break;

    case RING_PROTECTION:       // prot

        you.redraw_armor_class = 1;
        //      you.hunger_inc -= 1;
        break;

    case RING_PROTECTION_FROM_FIRE:     // fire resistance
        //      you.hunger_inc -= 1;

        break;

    case RING_POISON_RESISTANCE:        // you.poison resistance
        //      you.hunger_inc -= 1;

        break;

    case RING_PROTECTION_FROM_COLD:     // cold resistance
        //      you.hunger_inc -= 1;

        break;

    case RING_STRENGTH: // increase you.strength

        you.strength -= (you.inv_plus[ring_wear_2] % 100);
        you.max_strength -= (you.inv_plus[ring_wear_2] % 100);
        you.strength += 50;
        you.max_strength += 50;
        you.redraw_strength = 1;
        //      you.hunger_inc -= 1;
        burden_change();
        break;

    case RING_SLAYING:          // slaying
        //      you.hunger_inc -= 1;

        break;

    case RING_SEE_INVISIBLE:    // see invisible
        //      you.hunger_inc -= 1;

        break;

    case RING_INVISIBILITY:     // invisibility
        //      you.invis ++;
        //      if (you.invis == 0) you.invis = 1;
        //      you.hunger_inc -= 4;

        if (you.invis != 0)
            you.invis = 1;
        break;

    case RING_HUNGER:           // you.hunger

        // you.hunger_inc -= 4;
        break;

    case RING_TELEPORTATION:    // teleportation
        //      you.hunger_inc -= 1;

        break;

    case 11:                    // player_evasion()
        //      you.hunger_inc -= 1;

        you.redraw_evasion = 1;
        break;

    case RING_SUSTAIN_ABILITIES:        // you.sust_abil
        // you.hunger_inc -= 1;

        break;

    case RING_SUSTENANCE:       // sustenance

        // you.hunger_inc += 2;
        break;

    case RING_DEXTERITY:        // increase you.dex

        you.dex -= (you.inv_plus[ring_wear_2] % 100) - 50;
        you.max_dex -= (you.inv_plus[ring_wear_2] % 100) - 50;
        you.redraw_dexterity = 1;
        //      you.hunger_inc -= 1;
        break;

    case RING_INTELLIGENCE:     // increase you.intel

        you.intel -= (you.inv_plus[ring_wear_2] % 100) - 50;
        you.max_intel -= (you.inv_plus[ring_wear_2] % 100) - 50;
        you.redraw_intelligence = 1;
        //      you.hunger_inc -= 1;
        break;

    case RING_WIZARDRY: // wizardry
        // you.hunger_inc -= 1;

        break;

    case RING_MAGICAL_POWER:    // magic

        you.max_magic_points -= 9;
        you.base_magic_points2 -= 9;
        if (you.magic_points > you.max_magic_points)
            you.magic_points = you.max_magic_points;
        // you.hunger_inc --;
        you.redraw_magic_points = 1;
        /* you.magic_battery --; */
        calc_ep();
        break;

    case RING_LEVITATION:       // levitation
        /*      you.levitation ++;
           if (you.levitation == 0) you.levitation = 1;
           you.hunger_inc -= 1; */
        if (you.levitation != 0)
            you.levitation = 1;
        break;

    case RING_LIFE_PROTECTION:  // player_prot_life()
        //      you.hunger_inc --;

        break;

    case RING_PROTECTION_FROM_MAGIC:    // magic resistance
        // you.hunger_inc -= 1;

        break;

    case RING_FIRE:             // fire
        /* player_res_fire() -= 1;
           you.spec_fire -= 1;
           player_res_cold() += 1; */
        // you.hunger_inc -= 1;
        break;

    case RING_ICE:              // ice
        /* player_res_fire() += 1;
           you.spec_cold -= 1;
           player_res_cold() -= 1; */
        // you.hunger_inc -= 1;
        break;

    case RING_TELEPORT_CONTROL: // teleport control

        you.attribute[ATTR_CONTROL_TELEPORT]--;
        // you.hunger_inc -= 1;
        break;


    }                           // end of switch

    if (you.inv_dam[ring_wear_2] == 200 || you.inv_dam[ring_wear_2] == 201)
    {
        unuse_randart(ring_wear_2);
    }

    you.equip[hand_used + 7] = -1;

    you.turn_is_over = 1;
}                               // end of remove_ring


void zap_wand()
{
    int zap_device_1;
    int zap_device_2;
    unsigned char nthing = 0;

    struct bolt beam[1];
    struct dist zap_wand[1];

    beam[0].wand_id = 0;

    if (you.num_inv_items == 0)
    {
        mpr("You aren't carrying anything.");
        return;
    }

    if (you.berserker != 0)
    {
        mpr("You are too berserk!");
        return;
    }

  query:
    mpr("Zap which item?");

    unsigned char keyin = get_ch();

    if (keyin == '?' || keyin == '*')
    {
        if (keyin == '?')
            nthing = get_invent(OBJ_WANDS);
        if (keyin == '*')
            nthing = get_invent(-1);
        if ((nthing >= 65 && nthing <= 90) || (nthing >= 97 && nthing <= 122))
        {
            keyin = nthing;
        }
        else
        {
            /*   mesclr(); */
            goto query;

        }
    }

    zap_device_1 = (int) keyin;

    if (zap_device_1 < 65 || (zap_device_1 > 90 && zap_device_1 < 97) || zap_device_1 > 122)
    {
      Dont_have:
        mpr("You don't have any such object.");
        return;
    }

    zap_device_2 = conv_lett(zap_device_1);

    if (you.inv_quantity[zap_device_2] == 0)
    {
        goto Dont_have;
    }

    if (you.inv_class[zap_device_2] != OBJ_WANDS || you.inv_plus[zap_device_2] <= 0)
    {
        mpr("Nothing appears to happen.");
        you.turn_is_over = 1;
        return;
    }

    mpr("Which direction? (*/+ to target)");
    if (you.prev_targ != MHITNOT && you.prev_targ < MNST)
        if (mons_near(you.prev_targ) && (menv[you.prev_targ].enchantment[2] != 6 || player_see_invis() != 0))
        {
            strcpy(info, "You are currently targetting ");
            strcat(info, monam(menv[you.prev_targ].number, menv[you.prev_targ].type, menv[you.prev_targ].enchantment[
                                                                           2
                               ]
                               ,1));
            strcat(info, " (p to target).");
            mpr(info);
        }
        else
            mpr("You have no current target.");

    direction(1, zap_wand);

    if (zap_wand[0].nothing == -1)
        return;

    if (you.conf != 0)
    {
        zap_wand[0].move_x = random2(13) - 6;
        zap_wand[0].move_y = random2(13) - 6;
    }

    beam[0].source_x = you.x_pos;
    beam[0].source_y = you.y_pos;

    char type_zapped = you.inv_type[zap_device_2];

    if (type_zapped == WAND_ENSLAVEMENT)
        type_zapped = 21;       /* enslavement */
    if (type_zapped == WAND_DRAINING)
        type_zapped = 17;       /* draining */
    if (type_zapped == WAND_DISINTEGRATION)
        type_zapped = 46;       /* disintegration */
    if (type_zapped == WAND_RANDOM_EFFECTS)     /* random effects */
    {
        type_zapped = random2(16);
        if (random2(20) == 0)
            type_zapped = 17;
        if (random2(17) == 0)
            type_zapped = 21;
    }

    beam[0].move_x = zap_wand[0].move_x;
    beam[0].move_y = zap_wand[0].move_y;
    beam[0].target_x = zap_wand[0].target_x;
    beam[0].target_y = zap_wand[0].target_y;

    zapping(type_zapped, 40, beam);

    if (beam[0].wand_id == 1 || you.inv_type[zap_device_2] == WAND_FIREBALL)
    {
        if (!get_id(you.inv_class[zap_device_2], you.inv_type[zap_device_2]))
        {
            set_id(you.inv_class[zap_device_2], you.inv_type[zap_device_2], 1);

            char buff[80];

            buff[0] = zap_device_1;
            buff[1] = '\0';
            strcat(buff, " - ");
            in_name(zap_device_2, 3, str_pass);
            strcat(buff, str_pass);
            mpr(buff);
        }
    }

    you.inv_plus[zap_device_2]--;

    if (get_id(you.inv_class[zap_device_2], you.inv_type[zap_device_2]) == 1 && (you.inv_ident[zap_device_2] > 1 || you.skills[SK_ENCHANTMENTS] > random2(15) + 5))
    {
        if (you.inv_ident[zap_device_2] <= 1)
            mpr("Your skill with enchantments lets you calculate the power of this device...");

        strcpy(info, "This wand has ");
        itoa(you.inv_plus[zap_device_2], st_prn, 10);
        strcat(info, st_prn);
        strcat(info, " charge");
        if (you.inv_plus[zap_device_2] == 1)
            strcat(info, " left.");
        else
            strcat(info, "s left.");
        mpr(info);
        you.inv_ident[zap_device_2] = 3;
    }

    you.turn_is_over = 1;
    alert();
}                               /* end of void zap_wand(); */


void eat_food()
{
    int food_eat_1;
    int food_eat_2;

    unsigned char nthing = 0;

    if (you.is_undead == 2)
    {
        mpr("You can't eat.");
        return;
    }

    if (you.hunger >= 11000)    // || (you.hunger >= 7000 && you.species == 35))

    {
        mpr("You're too full to eat anything.");
        return;
    }

    if (igrd[you.x_pos][you.y_pos] != 501)
    {
        if (eat_from_floor() == 1)
            return;
    }

    if (you.num_inv_items == 0)
    {
        mpr("You aren't carrying anything.");
        return;
    }

  query:
    mpr("Eat which item?");

    unsigned char keyin = get_ch();

    if (keyin == '?' || keyin == '*')
    {
        if (keyin == '?')
            nthing = get_invent(OBJ_FOOD);
        if (keyin == '*')
            nthing = get_invent(-1);
        if ((nthing >= 65 && nthing <= 90) || (nthing >= 97 && nthing <= 122))
        {
            keyin = nthing;
        }
        else
        {
            /*       mesclr(); */
            goto query;

        }
    }

    food_eat_1 = (int) keyin;

    if (food_eat_1 < 65 || (food_eat_1 > 90 && food_eat_1 < 97) || food_eat_1 > 122)
    {
        mpr("You don't have any such object.");
        return;
    }

    food_eat_2 = conv_lett(food_eat_1);

    if (you.inv_quantity[food_eat_2] == 0)
    {
        mpr("You don't have any such object.");
        return;
    }

    if (you.inv_class[food_eat_2] != OBJ_FOOD)
    {
        mpr("You can't eat that!");
        return;
    }

    if (you.species == SP_GHOUL || you.species == SP_KOBOLD || you.mutation[MUT_CARNIVOROUS] == 3)      // kobold etc

    {
        if (
               (you.inv_type
                [food_eat_2] >= FOOD_BREAD_RATION && you.inv_type
                [food_eat_2] <= FOOD_CHOKO) ||
               (you.inv_type[food_eat_2] >= FOOD_SNOZZCUMBER && you.inv_type[food_eat_2] <= FOOD_LYCHEE))
        {
            mpr("Sorry, you're a carnivore.");
            return;
        }
    }

    if (you.mutation[MUT_HERBIVOROUS] > 1 && you.inv_type[food_eat_2] == FOOD_CHUNK)
    {
        mpr("You can't eat raw meat!");
        return;
    }


    if (you.inv_type
        [food_eat_2] == FOOD_CHUNK && you.hunger_state > 2 && wearing_amulet(AMU_THE_GOURMAND) == 0 && you.
        species !=
        SP_KOBOLD &&
        you.
        species !=
        SP_OGRE &&
        you.
        species !=
        SP_TROLL &&
        you.
        species !=
        SP_GHOUL &&
        you.
        mutation[MUT_CARNIVOROUS] ==
        0)
    {
        mpr("You aren't quite hungry enough to eat that!");
        return;
    }

    you.inv_quantity[food_eat_2]--;

    if (you.inv_quantity[food_eat_2] == 0)
    {
        you.num_inv_items--;
        if (you.equip[EQ_WEAPON] == food_eat_2)
        {
            you.equip[EQ_WEAPON] = -1;
            mpr("You are now empty handed.");
            wield_change = 1;
        }
    }

    eaten(food_eat_2, you.inv_type[food_eat_2]);        /* put different values (ie above 30) for eating things not in class 4, eg corpses. */

    if (you.hunger > 12000)
        you.hunger = 12000;

    food_change();

    you.turn_is_over = 1;

    burden_change();
}                               /* end of void eat_food() */


void drink()
{
    int drink_1;
    int drink_2;
    unsigned char nthing = 0;
    unsigned char keyin;

    if (you.is_undead == 2)
    {
        mpr("You can't drink.");
        return;
    }

    if (grd
        [you.x_pos]
        [you.y_pos] >= 200 && grd
        [you.x_pos]
        [you.y_pos] <= 209 && grd
        [you.x_pos]
        [you.y_pos] %
        2 ==
        0)
    {
        if (drink_fountain() == 1)
            return;
    }

    if (you.num_inv_items == 0)
    {
        mpr("You aren't carrying anything.");
        return;
    }

    if (you.berserker != 0)
    {
        mpr("You are too berserk!");
        return;
    }

  query:
    mpr("Drink which item?");

    keyin = get_ch();

    if (keyin == '?' || keyin == '*')
    {
        if (keyin == '?')
            nthing = get_invent(OBJ_POTIONS);
        if (keyin == '*')
            nthing = get_invent(-1);
        if ((nthing >= 65 && nthing <= 90) || (nthing >= 97 && nthing <= 122))
        {
            keyin = nthing;
        }
        else
        {
            mesclr();
            goto query;
        }
    }

    drink_1 = (int) keyin;

    if (drink_1 < 65 || (drink_1 > 90 && drink_1 < 97) || drink_1 > 122)
    {
        mpr("You don't have any such object.");
        return;
    }

    drink_2 = conv_lett(drink_1);

    if (you.inv_quantity[drink_2] == 0)
    {
        mpr("You don't have any such object.");
        return;
    }

    if (you.inv_class[drink_2] != OBJ_POTIONS)
    {
        mpr("You can't drink that!");
        return;
    }

    you.inv_quantity[drink_2]--;

    if (you.inv_quantity[drink_2] == 0)
    {
        you.num_inv_items--;
        if (you.equip[EQ_WEAPON] == drink_2)
        {
            you.equip[EQ_WEAPON] = -1;
            mpr("You are now empty handed.");
        }
    }

    burden_change();

    you.turn_is_over = 1;

    potion_effect(you.inv_type[drink_2], 40);

    you.hunger += 40;

    you.inv_ident[drink_2] = 1;

    set_id(you.inv_class[drink_2], you.inv_type[drink_2], 1);

    if (you.hunger > 12000)
        you.hunger = 12000;

    food_change();

}                               /* end of void drink() */


char drink_fountain()
{
    char dry = 0;

    switch (grd[you.x_pos][you.y_pos])
    {
    case 200:
        mpr("Drink from the fountain?");
        break;
    case 202:
        mpr("Drink from the sparkling fountain?");
        break;
    }

    char keyin = get_ch();

    if (keyin == 'y' || keyin == 'Y')
    {
        switch (grd[you.x_pos][you.y_pos])
        {
        case 200:               /* water fountain */
            mpr("You drink the pure, clear water.");
            if (random2(20) == 0)
                dry = 1;
            break;
        case 202:               /* magic fountain */
            mpr("You drink the sparkling water.");
            switch (random2(25))
            {
            case 0:
            case 1:
            case 2:
            case 3:
                potion_effect(random2(4), 100);
                break;
            case 4:
                if (random2(3) == 0)
                    potion_effect(POT_GAIN_STRENGTH + random2(3), 100);
                break;
            case 5:
            case 6:
            case 7:
            case 8:
                potion_effect(POT_LEVITATION + random2(6), 100);
                break;
            case 9:
                if (random2(5) == 0)
                    potion_effect(POT_DECAY, 100);
                else
                    potion_effect(POT_DEGENERATION, 100);
                break;
            case 10:
                potion_effect(POT_DECAY, 100);
                break;
            case 11:
            case 12:
                potion_effect(POT_MAGIC + random2(4), 100);
                break;
            case 13:
                potion_effect(POT_MUTATION, 100);
                break;
            default:
                mpr("Nothing appears to happen.");
                break;          /* Does nothing */
            }
            if (random2(10) == 0)
            {
                dry = 1;
                if (random2(5) == 0)
                    grd[you.x_pos][you.y_pos] = 200;    /* back to a normal fountain, but you won't know it yet */
            }
            break;
        }
    }
    else
        return 0;

    if (dry == 1)
    {
        mpr("The fountain dries up!");
        grd[you.x_pos][you.y_pos]++;
    }

    you.turn_is_over = 1;

    return 1;
}


void read_scroll()
{
    int sc_read_1;
    int sc_read_2;

    int id_used = 0;
    unsigned char nthing = 0;

    char affected = 0;

    int i;
    struct bolt beam[1];

    if (you.num_inv_items == 0)
    {
        mpr("You aren't carrying anything.");
        return;
    }

    if (you.berserker != 0)
    {
        mpr("You are too berserk!");
        return;
    }

  query:
    mpr("Read which item?");

    unsigned char keyin = get_ch();

    if (keyin == '?' || keyin == '*')
    {
        if (keyin == '?')
            nthing = get_invent(OBJ_SCROLLS);
        if (keyin == '*')
            nthing = get_invent(-1);
        if ((nthing >= 65 && nthing <= 90) || (nthing >= 97 && nthing <= 122))
        {
            keyin = nthing;
        }
        else
        {
            mesclr();
            goto query;

        }
    }

    sc_read_1 = (int) keyin;

    if (sc_read_1 < 65 || (sc_read_1 > 90 && sc_read_1 < 97) || sc_read_1 > 122)
    {
        mpr("You don't have any such object.");
        return;
    }

    sc_read_2 = conv_lett(sc_read_1);

    if (you.inv_quantity[sc_read_2] == 0)
    {
        mpr("You don't have any such object.");
        return;
    }

    if (you.inv_class[sc_read_2] == OBJ_BOOKS)
    {
        if (you.inv_type[sc_read_2] == BOOK_DESTRUCTION)
        {
            tome_of_power(sc_read_2);
            return;
        }
        if (you.inv_type[sc_read_2] == BOOK_MANUAL)
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
        nthing = which_spell_in_book(you.inv_type[sc_read_2], conv_lett(book_thing) + 1);
        if (nthing == 210)
        {
            mesclr();
            return;
        }
        describe_spell(which_spell_in_book(you.inv_type[sc_read_2], conv_lett(book_thing) + 1));
#ifdef PLAIN_TERM
        redraw_screen();
#endif

        mesclr();
        return;
    }

    if (you.inv_class[sc_read_2] != OBJ_SCROLLS)
    {
        mpr("You can't read that!");
        return;
    }

    if (you.inv_type[sc_read_2] == SCR_PAPER)
    {
        mpr("This scroll appears to be blank.");
        you.turn_is_over = 1;
        set_id(you.inv_class[sc_read_2], you.inv_type[sc_read_2], 1);
        return;
    }

    if (you.mutation[MUT_BLURRY_VISION] > 0 && random2(5) < you.mutation[MUT_BLURRY_VISION])
    {
        if (you.mutation[MUT_BLURRY_VISION] == 3 && random2(3) == 0)
            mpr("This scroll appears to be blank.");
        else
            mpr("The writing blurs in front of your eyes.");
        you.turn_is_over = 1;
        return;
    }

    you.inv_quantity[sc_read_2]--;

    if (you.inv_quantity[sc_read_2] == 0)
    {
        you.num_inv_items--;
        if (you.equip[EQ_WEAPON] == sc_read_2)
        {
            you.equip[EQ_WEAPON] = -1;
            mpr("You are now empty handed.");
        }
    }

    you.turn_is_over = 1;

    unsigned char scroll = 0;

    scroll = sc_read_2;

    mpr("As you read the scroll, it crumbles to dust.");
    burden_change();

    if (you.conf != 0)
    {
        random_uselessness(random2(9), sc_read_2);
        return;
    }

    if (you.skills[SK_SPELLCASTING] == 0)
        exercise(SK_SPELLCASTING, 1 + random2(2));

    switch (you.inv_type[sc_read_2])
    {
    case SCR_IDENTIFY:
        you.inv_ident[scroll] = 2;
        set_id(you.inv_class[sc_read_2], you.inv_type[sc_read_2], 1);
        identify(1);
        wield_change = 1;
        return;


    case SCR_TELEPORTATION:
        you_teleport();
        set_id(you.inv_class[sc_read_2], you.inv_type[sc_read_2], 1);
        return;

    case SCR_FEAR:
        mpr("You feel scary!");
        set_id(you.inv_class[sc_read_2], you.inv_type[sc_read_2], 1);
        mass_enchantment(4, 1000);
        break;

    case SCR_NOISE:
        mpr("You hear a loud clanging noise!");
        noisy(25, you.x_pos, you.y_pos);
        set_id(you.inv_class[sc_read_2], you.inv_type[sc_read_2], 1);
        return;

    case SCR_REMOVE_CURSE:
        remove_curse();
        set_id(you.inv_class[sc_read_2], you.inv_type[sc_read_2], 1);
        break;

    case SCR_DETECT_CURSE:
        detect_curse();
        set_id(you.inv_class[sc_read_2], you.inv_type[sc_read_2], 1);
        break;

    case SCR_SUMMONING:
        mpr("A horrible Thing appears!");
        create_monster(MONS_LARGE_ABOMINATION, 25, 7, you.x_pos, you.y_pos, MHITNOT, 250);
        set_id(you.inv_class[sc_read_2], you.inv_type[sc_read_2], 1);
        break;

    case SCR_ENCHANT_WEAPON_I:
    case SCR_ENCHANT_WEAPON_II:
    case SCR_ENCHANT_WEAPON_III:
      weapon_enchant:
        if (you.equip[EQ_WEAPON] == -1)
        {
            mpr("Nothing appears to happen.");
            return;
        }

        if (you.inv_class
            [you.equip
             [EQ_WEAPON]] == OBJ_WEAPONS || you.inv_class
            [you.equip
             [EQ_WEAPON]] == OBJ_MISSILES)
        {

            if (you.inv_type
                [sc_read_2] == SCR_ENCHANT_WEAPON_II && you.inv_class
                [you.equip
                 [EQ_WEAPON]] == OBJ_WEAPONS)
            {
                id_used = you.inv_plus2[you.equip[EQ_WEAPON]];
            }
            else
                id_used = you.inv_plus[you.equip[EQ_WEAPON]];

            affected = 1;

            if (you.inv_class
                [you.equip
                 [EQ_WEAPON]] == OBJ_WEAPONS &&
                (you.inv_dam
                 [you.equip
                  [EQ_WEAPON]] > 180 || you.inv_dam[you.equip[EQ_WEAPON]] % 30 >= 25))  // artefacts can't be enchanted, but these scrolls still remove curses

            {
                /*      strcpy(info, "Nothing appears to happen.");
                   mpr(info);
                   return; */
                affected = 0;
            }

            wield_change = 1;

            /* if (you.inv_dam [you.equip [EQ_WEAPON]] == 4) // electrocution
               {
               if (id_used > 130)
               {
               id_used -= 100;
               }
               item_name(you.inv_plus2 [you.equip [EQ_WEAPON]], you.inv_class [you.equip [EQ_WEAPON]], you.inv_type [you.equip [EQ_WEAPON]], you.inv_dam [you.equip [EQ_WEAPON]], you.inv_plus [you.equip [EQ_WEAPON]], you.inv_quantity [you.equip [EQ_WEAPON]], you.inv_ident [you.equip [EQ_WEAPON]], 4, str_pass);

               id_used += 3 + random2(3) + random2(3);
               if (id_used >= 59) id_used = 59;
               you.inv_plus2 [you.equip [EQ_WEAPON]] = id_used;
               strcpy(info, str_pass);
               strcat(info, " glows electric blue for a moment.");
               mpr(info);
               return;
               } */

            if (id_used >= 154 || (id_used < 100 && id_used >= 54))
            {
                if (random2(9) < id_used % 50)
                    affected = 0;
            }

            // if it isn't affected by the enchantment, it will still be uncursed:
            if (you.inv_plus[you.equip[EQ_WEAPON]] >= 154 || (you.inv_dam[you.equip[EQ_WEAPON]] % 30 >= 25 &&
                                 you.inv_plus[you.equip[EQ_WEAPON]] >= 100))
            {
                if (affected == 0)
                {
                    item_name(you.inv_plus2[you.equip[EQ_WEAPON]], you.inv_class[you.equip[EQ_WEAPON]], you
                              .inv_type[you.equip[EQ_WEAPON]], you.inv_dam[you.equip[EQ_WEAPON]], you.inv_plus
                              [you.equip[EQ_WEAPON]], you.inv_quantity[you.equip[EQ_WEAPON]], you.inv_ident[you
                                                                      .equip
                                                                  [EQ_WEAPON
                                                                          ]]
                              ,4, str_pass);
                    strcpy(info, str_pass);
                    strcat(info, " glows silver for a moment.");
                    mpr(info);
                    you.inv_plus[you.equip[EQ_WEAPON]] -= 100;
                    set_id(you.inv_class[sc_read_2], you.inv_type[sc_read_2], 1);
                    return;
                }
            }

            //      if (id_used - 50 >= 4 && id_used < 130)
            //      {
            if (affected == 0)
                goto nothing_happened;
            //              if (random2(9) < (you.inv_plus [you.equip [0]] - 50)) goto nothing_happened;
            //      }



            // vVvVv    This is *here* (as opposed to lower down) for a reason!
            item_name(you.inv_plus2
                      [you.equip
                       [EQ_WEAPON]], you.inv_class
                      [you.equip
                       [EQ_WEAPON]], you.inv_type
                      [you.equip
                       [EQ_WEAPON]], you.inv_dam
                      [you.equip
                       [EQ_WEAPON]], you.inv_plus
                      [you.equip
                       [EQ_WEAPON]], you.inv_quantity
                      [you.equip
                       [EQ_WEAPON]], you.inv_ident
                      [you.equip[EQ_WEAPON]], 4, str_pass);

            strcpy(info, str_pass);
            if (you.inv_type
                [sc_read_2] == SCR_ENCHANT_WEAPON_II && you.inv_class
                [you.equip
                 [EQ_WEAPON]] == OBJ_WEAPONS)
            {
                strcat(info, " glows red for a moment.");
                mpr(info);
            }
            else if (you.inv_type
                     [sc_read_2] == SCR_ENCHANT_WEAPON_I && you.inv_class
                     [you.equip
                      [EQ_WEAPON]] == OBJ_WEAPONS)
            {
                strcat(info, " glows green for a moment.");
                mpr(info);
            }
            else if (you.inv_class[you.equip[EQ_WEAPON]] == OBJ_WEAPONS)
            {
                id_used--;      /* no plusses for using this */
                if (you.duration[DUR_VORPAL_BLADE] > 0)
                {
                    you.duration[DUR_VORPAL_BLADE] = 0;
                    strcat(info, "'s sharpness seems more permanent.");
                    mpr(info);
                }
                else if (you.duration[DUR_FIRE_BRAND] > 0)
                {
                    you.duration[DUR_FIRE_BRAND] = 0;
                    strcat(info, " is engulfed in an explosion of flames!");
                    mpr(info);
                    beam[0].type = 43;
                    beam[0].damage = 110;
                    beam[0].flavour = 2;
                    beam[0].bx = you.x_pos;
                    beam[0].by = you.y_pos;
                    strcpy(beam[0].beam_name, "fiery explosion");
                    beam[0].colour = RED;
                    beam[0].thing_thrown = 1;
                    explosion(1, beam);
                }
                else if (you.duration[DUR_ICE_BRAND] > 0)
                {
                    you.duration[DUR_ICE_BRAND] = 0;
                    strcat(info, " glows brilliantly blue for a moment.");
                    mpr(info);
                    cast_refrigeration(60);
                }
                else if (you.duration[DUR_LETHAL_INFUSION] > 0)
                {
                    you.duration[DUR_LETHAL_INFUSION] = 0;
                    strcat(info, " thirsts for the lives of mortals!");
                    mpr(info);
                    drain_exp();
                }
                else if (you.duration[DUR_POISON_WEAPON] > 0)
                {
                    you.duration[DUR_POISON_WEAPON] = 0;
                    strcat(info, " seems more permanently poisoned.");
                    mpr(info);
                    cast_toxic_radiance();
                }
                else
                {
                    strcat(info, " glows bright yellow for a while.");
                    mpr(info);
                    you.inv_plus[you.equip[EQ_WEAPON]] += 1 + random2(2);
                    you.inv_plus2[you.equip[EQ_WEAPON]] += 1 + random2(2);
                    if (you.inv_plus[you.equip[EQ_WEAPON]] > 130)
                        you.inv_plus[you.equip[EQ_WEAPON]] -= 100;
                    set_id(you.inv_class[sc_read_2], you.inv_type[sc_read_2], 1);
                    return;
                }
            }
            set_id(you.inv_class[sc_read_2], you.inv_type[sc_read_2], 1);


            id_used++;
            if (you.inv_plus[you.equip[EQ_WEAPON]] > 130)
                you.inv_plus[you.equip[EQ_WEAPON]] -= 100;
            if (id_used > 130)
                id_used -= 100;

            if (you.inv_type
                [sc_read_2] == SCR_ENCHANT_WEAPON_II && you.inv_class
                [you.equip
                 [EQ_WEAPON]] == OBJ_WEAPONS)
            {
                you.inv_plus2[you.equip[EQ_WEAPON]] = id_used;
            }
            else if (you.inv_type
                     [sc_read_2] == SCR_ENCHANT_WEAPON_I && you.inv_class
                     [you.equip
                      [EQ_WEAPON]] == OBJ_WEAPONS)
            {
                you.inv_plus[you.equip[EQ_WEAPON]] = id_used;
            }
            return;

            //        strcat(info,
        }

      nothing_happened:
        mpr("Nothing appears to happen.");
        return;                 // end of case 7: enc weapon

    case SCR_VORPALISE_WEAPON:  // vorpalise weapon
        if (you.equip[EQ_WEAPON] == -1
                || you.inv_class [you.equip [EQ_WEAPON]] != OBJ_WEAPONS
                || (you.inv_class[you.equip[EQ_WEAPON]] == OBJ_WEAPONS
                && (you.inv_dam[you.equip[EQ_WEAPON]] > 180
                || you.inv_dam [you.equip [EQ_WEAPON]] % 30 >= 25
                || (you.inv_type[you .equip [EQ_WEAPON ] ] >= WPN_SLING
                && you.inv_type[you.equip[EQ_WEAPON]] <= WPN_HAND_CROSSBOW))))
        {
            mpr("Nothing appears to happen.");
            return;
        }

        item_name(you.inv_plus2[you.equip[EQ_WEAPON]],
                    you.inv_class[you.equip[EQ_WEAPON]],
                    you.inv_type[you.equip[EQ_WEAPON]],
                    you.inv_dam[you.equip[EQ_WEAPON]],
                    you.inv_plus[you.equip[EQ_WEAPON]],
                    you.inv_quantity[you.equip[EQ_WEAPON]],
                    you.inv_ident[you.equip[EQ_WEAPON]], 4, str_pass);

        strcpy(info, str_pass);
        strcat(info, " emits a brilliant flash of light!");
        alert();
        mpr(info);
        set_id(you.inv_class[sc_read_2], you.inv_type[sc_read_2], 1);
        wield_change = 1;

        if (you.inv_dam[you.equip[EQ_WEAPON]] > 180
                    || you.inv_dam[you.equip[EQ_WEAPON]] % 30 != SPWPN_NORMAL)
        {
            /* you.inv_plus [you.equip [EQ_WEAPON]] = 50;
               you.inv_plus2 [you.equip [EQ_WEAPON]] = 50;
               switch(you.inv_dam [you.equip [EQ_WEAPON]] % 30)
               {
               case SPWPN_PROTECTION:
               / *  player_AC() -= 3;* /
               you.redraw_armor_class = 1;
               break;

               case SPWPN_VAMPIRICISM:
               you.hunger_inc -= 6;
               break;
               }
               you.inv_dam [you.equip [EQ_WEAPON]] = (you.inv_dam [you.equip [EQ_WEAPON]] / 30) * 30;
               if (you.inv_dam [you.equip [EQ_WEAPON]] == 90) you.inv_dam [you.equip [EQ_WEAPON]] = SPWPN_NORMAL; // stops glowing
               you.duration [DUR_VORPAL_BLADE] = 0;
               you.duration [DUR_FIRE_BRAND] = 0;
               you.duration [DUR_ICE_BRAND] = 0;
               you.duration [DUR_LETHAL_INFUSION] = 0; */
            mpr("You feel strangely frustrated.");
            return;
        }
        you.inv_dam[you.equip[EQ_WEAPON]] += 10;
        return;


    case SCR_RECHARGING:
        if (you.equip[EQ_WEAPON] != -1 && you.inv_class[you.equip[EQ_WEAPON]] == OBJ_WEAPONS && you.inv_dam[you.equip[EQ_WEAPON]] == SPWPN_ELECTROCUTION)       // electrocution

            goto weapon_enchant;

        if (recharge_wand() == 0)
        {
            mpr("Nothing appears to happen.");
        }
        else
        {
            set_id(you.inv_class[sc_read_2], you.inv_type[sc_read_2], 1);
            wield_change = 1;
        }
        return;


    case SCR_ENCHANT_ARMOUR:    // ench you.equip

        for (i = EQ_CLOAK; i < EQ_RIGHT_RING; i++)
        {
            if (i == EQ_LEFT_RING)
            {
                mpr("Nothing appears to happen.");
                return;
            }

            if (you.equip[i] != -1)
                break;
        }

        do
        {
            affected = random2(6) + 1;
        }
        while (you.equip[affected] == -1);

        // NOTE: It is assumed that armour which changes in this way does not change
        //  into a form of armour with a different evasion modifier.

        if (you.inv_type
            [you.equip
             [EQ_BODY_ARMOUR]] == ARM_DRAGON_HIDE || you.inv_type
            [you.equip
             [EQ_BODY_ARMOUR]] == ARM_ICE_DRAGON_HIDE || you.inv_type
            [you.equip
             [EQ_BODY_ARMOUR]] == ARM_STEAM_DRAGON_HIDE || you.inv_type
            [you.equip
             [EQ_BODY_ARMOUR]] == ARM_MOTTLED_DRAGON_HIDE || you.inv_type
            [you.equip
             [EQ_BODY_ARMOUR]] == ARM_STORM_DRAGON_HIDE || you.inv_type
            [you.equip
             [EQ_BODY_ARMOUR]] == ARM_GOLD_DRAGON_HIDE || you.inv_type
            [you.equip
             [EQ_BODY_ARMOUR]] == ARM_SWAMP_DRAGON_HIDE)        // dragon hide

        {
            affected = EQ_BODY_ARMOUR;
            item_name(you.inv_plus2[you.equip[affected]], you.inv_class[you.equip[affected]], you.inv_type[
                                                                         you

                                                                           .
                                                                       equip
                                                                           [
                                                                    affected
                                                                           ]
                      ]
                      ,you.inv_dam[you.equip[affected]], you.inv_plus[you.equip[affected]], you.inv_quantity[you

                                                                           .
                                                                       equip
                                                                           [
                                                                    affected
                                                                           ]
                      ]
                      ,you.inv_ident[you.equip[affected]], 4, str_pass);
            strcpy(info, str_pass);
            strcat(info, " glows purple and changes!");
            mpr(info);
            you.redraw_armor_class = 1;

            /* player_AC() -= property [2] [you.inv_type [you.equip [EQ_BODY_ARMOUR]]] [0] * (15 + you.skills [SK_ARMOUR]) / 15;


               player_evasion() -= ev_mod(); */

            switch (you.inv_type[you.equip[EQ_BODY_ARMOUR]])
            {

            case ARM_DRAGON_HIDE:
                /*                 player_res_fire() ++;
                   player_res_cold() --; */
                you.inv_type[you.equip[EQ_BODY_ARMOUR]] = ARM_DRAGON_ARMOUR;
                break;

            case ARM_ICE_DRAGON_HIDE:
                /*                 player_res_cold() ++;
                   player_res_fire() --; */
                you.inv_type[you.equip[EQ_BODY_ARMOUR]] = ARM_ICE_DRAGON_ARMOUR;
                break;

            case ARM_STEAM_DRAGON_HIDE: // steam dragon

                you.inv_type[you.equip[EQ_BODY_ARMOUR]] = ARM_STEAM_DRAGON_ARMOUR;
                break;

            case ARM_MOTTLED_DRAGON_HIDE:       // mottled dragon

                you.inv_type[you.equip[EQ_BODY_ARMOUR]] = ARM_MOTTLED_DRAGON_ARMOUR;
                break;

            case ARM_STORM_DRAGON_HIDE: // storm dragon

                you.inv_type[you.equip[EQ_BODY_ARMOUR]] = ARM_STORM_DRAGON_ARMOUR;
                // you.attribute[ATTR_RESIST_LIGHTNING]++;
                break;

            case ARM_GOLD_DRAGON_HIDE:          // gold dragon

                you.inv_type[you.equip[EQ_BODY_ARMOUR]] = ARM_GOLD_DRAGON_ARMOUR;
                /*                 player_res_fire() ++;
                   player_res_cold() ++;
                   you.res_poison ++; */
                break;

            case ARM_SWAMP_DRAGON_HIDE: // swamp dragon

                you.inv_type[you.equip[EQ_BODY_ARMOUR]] = ARM_SWAMP_DRAGON_ARMOUR;
                break;

            }
            /*        player_evasion() += ev_mod();
               player_AC() += property [2] [you.inv_type [you.equip [EQ_BODY_ARMOUR]]] [0] * (15 + you.skills [SK_ARMOUR]) / 15; */

            set_id(you.inv_class[sc_read_2], you.inv_type[sc_read_2], 1);
            return;
        }


        if (you.inv_type[you.equip[EQ_BODY_ARMOUR]] == ARM_TROLL_HIDE)  // troll hide

        {
            affected = EQ_BODY_ARMOUR;
            item_name(you.inv_plus2[you.equip[affected]], you.inv_class[you.equip[affected]], you.inv_type[
                                                                         you
                                                                           .
                                                                       equip
                                                                           [
                                                                    affected
                                                                           ]
                      ]
                      ,you.inv_dam[you.equip[affected]], you.inv_plus[you.equip[affected]], you.inv_quantity[you
                                                                           .
                                                                       equip
                                                                           [
                                                                    affected
                                                                           ]
                      ]
                      ,you.inv_ident[you.equip[affected]], 4, str_pass);
            strcpy(info, str_pass);
            strcat(info, " glows purple and changes!");
            mpr(info);
            /*                player_AC() += 1; */
            you.redraw_armor_class = 1;
            /*                you.rate_regen += 50; */
            //                you.hunger_inc += 1;
            /* player_AC() -= property [2] [you.inv_type [you.equip [EQ_BODY_ARMOUR]]] [0] * (15 + you.skills [SK_ARMOUR]) / 15;
               player_evasion() -= ev_mod(); */
            you.inv_type[you.equip[EQ_BODY_ARMOUR]] = ARM_TROLL_LEATHER_ARMOUR;
            /*        player_evasion() += ev_mod();
               player_AC() += property [2] [you.inv_type [you.equip [EQ_BODY_ARMOUR]]] [0] * (15 + you.skills [SK_ARMOUR]) / 15; */
            set_id(you.inv_class[sc_read_2], you.inv_type[sc_read_2], 1);
            return;
        }

        if (you.inv_dam[you.equip[affected]] % 30 >= 25)
        {
            item_name(you.inv_plus2[you.equip[affected]], you.inv_class[you.equip[affected]], you.inv_type[
                                                                         you

                                                                           .
                                                                       equip
                                                                           [
                                                                    affected
                                                                           ]
                      ]
                      ,you.inv_dam[you.equip[affected]], you.inv_plus[you.equip[affected]], you.inv_quantity[you

                                                                           .
                                                                       equip
                                                                           [
                                                                    affected
                                                                           ]
                      ]
                      ,you.inv_ident[you.equip[affected]], 4, str_pass);
            set_id(you.inv_class[sc_read_2], you.inv_type[sc_read_2], 1);
            strcpy(info, str_pass);
            strcat(info, " glows faintly for a moment.");
            mpr(info);
            if (you.inv_plus[you.equip[affected]] > 100)
                you.inv_plus[you.equip[affected]] -= 100;
            return;
        }

        if (you.inv_plus[you.equip[affected]] >= 153)
        {
            if (random2(8) >= (you.inv_plus[you.equip[affected]] - 150))
            {
                item_name(you.inv_plus2
                          [you.equip
                           [affected]], you.inv_class
                          [you.equip
                           [affected]], you.inv_type[you.equip[affected]], you.inv_dam[you.equip[affected]], you.inv_plus[
                                                                         you

                                                                           .
                                                                       equip
                                                                           [
                                                                    affected
                                                                           ]
                          ]
                          ,you.inv_quantity[you.equip[affected]], you.inv_ident[you.equip[affected]], 4, str_pass)
                    ;

                you.inv_plus[you.equip[affected]] -= 100;
                set_id(you.inv_class[sc_read_2], you.inv_type[sc_read_2], 1);

                strcpy(info, str_pass);
                strcat(info, " glows silver for a moment.");
                mpr(info);
                return;
            }
        }

        if ((you.inv_plus[you.equip[affected]] - 50 >= 3 && you.inv_plus[you.equip[affected]] < 130) || you
            .inv_plus[you.equip[affected]] >= 153)
        {
            if (random2(8) < (you.inv_plus[you.equip[affected]] - 50))
                goto nothing_happened_2;
        }

        if (you.inv_type[you.equip[affected]] >= ARM_CLOAK && you.inv_type[you.equip[affected]] <= ARM_BOOTS
            )
            if ((you.inv_plus[you.equip[affected]] - 50 >= 2 && you.inv_plus[you.equip[affected]] < 130) ||
                you.inv_plus[you.equip[affected]] >= 152)
                goto nothing_happened_2;

        // vVvVv    This is *here* for a reason!
        item_name(you.inv_plus2[you.equip[affected]], you.inv_class[you.equip[affected]], you.inv_type[you
                                                                      .equip
                                                                   [affected
                                                                         ]],
                  you.inv_dam[you.equip[affected]], you.inv_plus[you.equip[affected]], you.inv_quantity[you
                                                                      .equip
                                                                   [affected
                                                                         ]],
                  you.inv_ident[you.equip[affected]], 4, str_pass);

        you.inv_plus[you.equip[affected]]++;
        if (you.inv_plus[you.equip[affected]] > 130)
            you.inv_plus[you.equip[affected]] -= 100;
        //strcpy(info, "Your ");
        //in_name(you.equip [affected], 4);

        set_id(you.inv_class[sc_read_2], you.inv_type[sc_read_2], 1);

        strcpy(info, str_pass);
        strcat(info, " glows green for a moment.");
        /* if (affected != 5) player_AC() ++; else player_shield_class() ++; */
        you.redraw_armor_class = 1;
        mpr(info);
        return;                 // end of ench you.equip

    case SCR_TORMENT:           // torment
        /*      beam[0].bx = you.x_pos;
           beam[0].by = you.y_pos; */
        torment();              //you.x_pos, you.y_pos);

        if (get_id(you.inv_class[sc_read_2], you.inv_type[sc_read_2]) == 1)
            naughty(NAUGHTY_UNHOLY, 10);        // is only naughty if you know you're doing it

        set_id(you.inv_class[sc_read_2], you.inv_type[sc_read_2], 1);
        return;

    case SCR_RANDOM_USELESSNESS:        // random uselessness

        random_uselessness(random2(9), sc_read_2);      //random2 (1));

        return;

    case SCR_CURSE_WEAPON:      // curse weapon

        if (you.equip[EQ_WEAPON] == -1 || you.inv_class[you.equip[EQ_WEAPON]] > OBJ_WEAPONS || you.inv_plus[
                                                                         you

                                                                           .
                                                                       equip
                                                                           [
                                                                   EQ_WEAPON
                                                                           ]
            ]
            > 130)              // || you.inv_dam [you.equip [EQ_WEAPON]] % 30 >= 25)

        {
            goto nothing_happened_2;
        }
        in_name(you.equip[EQ_WEAPON], 4, str_pass);
        strcpy(info, str_pass);
        strcat(info, " glows black for a moment.");
        mpr(info);
        you.inv_plus[you.equip[EQ_WEAPON]] += 100;
        set_id(you.inv_class[sc_read_2], you.inv_type[sc_read_2], 1);
        wield_change = 1;
        return;


    case SCR_CURSE_ARMOUR:      // curse you.equip

        for (i = EQ_CLOAK; i < EQ_RIGHT_RING; i++)
        {
            if (i == EQ_LEFT_RING)
            {
                mpr("Nothing appears to happen.");
                return;
            }
            if (you.equip[i] != -1 && you.inv_plus[you.equip[i]] < 130)
                break;
        }

        do
        {
            affected = random2(6) + 1;
        }
        while (you.equip[affected] == -1 || you.inv_plus[you.equip[affected]] > 130);


        // vVvVv    This is *here* for a reason!
        item_name(you.inv_plus2[you.equip[affected]], you.inv_class[you.equip[affected]], you.inv_type[you
                                                                      .equip
                                                                   [affected
                                                                         ]],
                  you.inv_dam[you.equip[affected]], you.inv_plus[you.equip[affected]], you.inv_quantity[you
                                                                      .equip
                                                                   [affected
                                                                         ]],
                  you.inv_ident[you.equip[affected]], 4, str_pass);
        you.inv_plus[you.equip[affected]] += 100;
        set_id(you.inv_class[sc_read_2], you.inv_type[sc_read_2], 1);
        strcpy(info, str_pass);
        strcat(info, " glows black for a moment.");
        mpr(info);
        return;


    case SCR_IMMOLATION:
        mpr("The scroll explodes in your hands!");

        beam[0].type = 43;
        beam[0].damage = 110;
        beam[0].flavour = 2;    // <=- not sure about this

        beam[0].bx = you.x_pos;
        beam[0].by = you.y_pos;
        strcpy(beam[0].beam_name, "fiery explosion");
        beam[0].colour = RED;
        beam[0].thing_thrown = 1;       // your explosion, (not someone else's explosion)

        explosion(1, beam);
        set_id(you.inv_class[sc_read_2], you.inv_type[sc_read_2], 1);
        break;

    case SCR_BLINKING:          // blink

        blink();

        set_id(you.inv_class[sc_read_2], you.inv_type[sc_read_2], 1);

        break;

        // This is never called; it is dealt with earlier:
    case SCR_PAPER:             // paper

        break;

    case SCR_MAGIC_MAPPING:     // magic mapping

        if (you.level_type == 1 || you.level_type == 2)
        {
            mpr("You feel momentarily disoriented.");
            set_id(you.inv_class[sc_read_2], you.inv_type[sc_read_2], 1);
            break;
        }
        mpr("You feel aware of your surroundings.");
        magic_mapping(21, 95 + random2(10));
        set_id(you.inv_class[sc_read_2], you.inv_type[sc_read_2], 1);
        break;

    case SCR_FORGETFULNESS:
        mpr("You feel momentarily disoriented.");
        forget_map(50 + random2(50));
        set_id(you.inv_class[sc_read_2], you.inv_type[sc_read_2], 1);
        break;

    case SCR_ACQUIREMENT:
        acquirement(250);
        set_id(you.inv_class[sc_read_2], you.inv_type[sc_read_2], 1);
        break;
    }
    return;

  nothing_happened_2:
    mpr("Nothing appears to happen.");
    return;
}                               // end of void read_scroll()


void original_name()
{
    char drink_1;
    char drink_2;
    char inn = 0;
    unsigned char nthing = 0;

  query:
    mpr("Examine which item?");

    int keyin = get_ch();

    if (keyin == '?' || keyin == '*')
    {
        if (keyin == '?')
            nthing = get_invent(-1);
        if (keyin == '*')
            nthing = get_invent(-1);
        if ((nthing >= 65 && nthing <= 90) || (nthing >= 97 && nthing <= 122))
        {
            keyin = nthing;
        }
        else
        {
            mesclr();
            goto query;
        }
    }

    drink_1 = (int) keyin;

    if (drink_1 < 65 || (drink_1 > 90 && drink_1 < 97) || drink_1 > 122)
    {
        mpr("You don't have any such object.");
        return;
    }

    drink_2 = conv_lett(drink_1);

    if (you.inv_quantity[drink_2] == 0)
    {
        mpr("You don't have any such object.");
        return;
    }

    inn = drink_2;
    describe_item(you.inv_class
                  [inn], you.inv_type
                  [inn], you.inv_plus
                  [inn], you.inv_plus2
                  [inn], you.inv_dam
                  [inn], you.inv_ident[inn]);
#ifdef PLAIN_TERM
    redraw_screen();
#endif

}                               // end original_name


void use_randart(unsigned char item_wield_2)
{

    if (randart_wpn_properties(you.inv_class[item_wield_2], you.inv_type[item_wield_2], you.inv_dam[item_wield_2], you.inv_plus[item_wield_2], you.inv_plus2[item_wield_2], 0, RAP_AC) != 0)
    {
        you.redraw_armor_class = 1;
    }

    if (randart_wpn_properties(you.inv_class[item_wield_2], you.inv_type[item_wield_2], you.inv_dam[item_wield_2], you.inv_plus[item_wield_2], you.inv_plus2[item_wield_2], 0, RAP_EVASION) != 0)
    {
        you.redraw_evasion = 1;
    }

    if (randart_wpn_properties(you.inv_class[item_wield_2], you.inv_type[item_wield_2], you.inv_dam[item_wield_2], you.inv_plus[item_wield_2], you.inv_plus2[item_wield_2], 0, RAP_STRENGTH) != 0)
    {
        you.redraw_strength = 1;
        you.strength += randart_wpn_properties(you.inv_class[item_wield_2], you.inv_type[item_wield_2], you.inv_dam[item_wield_2], you.inv_plus[item_wield_2], you.inv_plus2 [item_wield_2], 0, RAP_STRENGTH);
        you.max_strength += randart_wpn_properties(you.inv_class[item_wield_2], you.inv_type[item_wield_2], you.inv_dam[item_wield_2], you.inv_plus[item_wield_2] ,you.inv_plus2[item_wield_2], 0, RAP_STRENGTH);
    }

    if (randart_wpn_properties(you.inv_class[item_wield_2], you.inv_type[item_wield_2], you.inv_dam[item_wield_2], you.inv_plus[item_wield_2], you.inv_plus2[item_wield_2], 0, RAP_INTELLIGENCE) != 0)
    {
        you.redraw_intelligence = 1;
        you.intel += randart_wpn_properties(you.inv_class[item_wield_2], you.inv_type[item_wield_2], you.inv_dam[item_wield_2], you.inv_plus[item_wield_2], you.inv_plus2[item_wield_2], 0, RAP_INTELLIGENCE);
        you.max_intel += randart_wpn_properties(you.inv_class[item_wield_2], you.inv_type[item_wield_2], you.inv_dam[item_wield_2], you.inv_plus[item_wield_2], you.inv_plus2[item_wield_2], 0, RAP_INTELLIGENCE);
    }

    if (randart_wpn_properties(you.inv_class[item_wield_2], you.inv_type[item_wield_2], you.inv_dam[item_wield_2], you.inv_plus[item_wield_2], you.inv_plus2[item_wield_2], 0, RAP_DEXTERITY) != 0)
    {
        you.redraw_dexterity = 1;
        you.dex += randart_wpn_properties(you.inv_class[item_wield_2], you.inv_type[item_wield_2], you.inv_dam [item_wield_2], you.inv_plus[item_wield_2], you.inv_plus2[item_wield_2] ,0, RAP_DEXTERITY);
        you.max_dex += randart_wpn_properties(you.inv_class [item_wield_2], you.inv_type [item_wield_2], you.inv_dam[item_wield_2], you.inv_plus[item_wield_2], you.inv_plus2 [item_wield_2], 0, RAP_DEXTERITY);
    }

// this is handled by player_res_electricity() now
//    if (randart_wpn_properties(you.inv_class[item_wield_2], you.inv_type[item_wield_2], you.inv_dam[item_wield_2], you.inv_plus[item_wield_2], you.inv_plus2[item_wield_2], 0, RAP_ELECTRICITY) != 0)
//    {
//      you.attribute[ATTR_RESIST_LIGHTNING]++;
//    }

// this is handled by player_hunger_rate() now
//     if (randart_wpn_properties(you.inv_class[item_wield_2], you.inv_type[item_wield_2], you.inv_dam[item_wield_2], you.  inv_plus[item_wield_2], you.  inv_plus2[item_wield_2], 0, RAP_METABOLISM) != 0)
//    {
//      you.hunger_inc += randart_wpn_properties(you.inv_class[item_wield_2], you.inv_type[item_wield_2], you .inv_dam [item_wield_2], you.inv_plus [item_wield_2], you.inv_plus2[item_wield_2], 0, RAP_METABOLISM);
//     }

    if (randart_wpn_properties(you.inv_class[item_wield_2], you.inv_type[item_wield_2], you.inv_dam[item_wield_2], you.  inv_plus[item_wield_2], you.  inv_plus2[item_wield_2], 0, RAP_NOISES) != 0)
    {
        you.special_wield = 50 + randart_wpn_properties(you.inv_class[item_wield_2], you.inv_type[item_wield_2], you.inv_dam[item_wield_2], you.inv_plus[item_wield_2], you.inv_plus2[item_wield_2], 0, RAP_NOISES);
    }
}

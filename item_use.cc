/*
 *  File:       item_use.cc
 *  Summary:    Functions for making use of inventory items.
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 *   <7>     11/23/99    LRH    Horned characters can wear hats/caps
 *   <6>     7/13/99     BWR    Lowered learning rates for
 *                              throwing skills, and other
 *                              balance tweaks
 *   <5>     5/28/99     JDJ    Changed wear_armour to allow Spriggans to
 *                              wear bucklers.
 *   <4>     5/26/99     JDJ    body armor can be removed and worn if an
 *                              uncursed cloak is being worn.
 *                              Removed lots of unnessary mpr string copying.
 *                              Added missing ponderous message.
 *   <3>     5/20/99     BWR    Fixed staff of air bug, output of trial
 *                              identified items, a few wield_changes so
 *                              that the weapon gets updated.
 *   <2>     5/08/99     JDJ    Added armour_prompt.
 *   <1>     -/--/--     LRH    Created
 */

#include "AppHdr.h"
#include "item_use.h"

#include <string.h>
#include <stdlib.h>

#include "externs.h"

#include "bang.h"
#include "beam.h"
#include "debug.h"
#include "describe.h"
#include "direct.h"
#include "effects.h"
#include "fight.h"
#include "food.h"
#include "invent.h"
#include "it_use2.h"
#include "it_use3.h"
#include "itemname.h"
#include "monplace.h"
#include "monstuff.h"
#include "mon-util.h"
#include "ouch.h"
#include "player.h"
#include "randart.h"
#include "religion.h"
#include "skills.h"
#include "skills2.h"
#include "spells.h"
#include "spells1.h"
#include "spells2.h"
#include "spells3.h"
#include "spl-book.h"
#include "stuff.h"
#include "transfor.h"
#include "view.h"
#include "wpn-misc.h"


extern int book_thing;       // defined in spells.cc
extern bool wield_change;    // defined in output.cc


bool drink_fountain(void);
static void throw_it(struct bolt *pbolt, int throw_2);
void use_randart(unsigned char item_wield_2);




void wield_weapon( bool auto_wield )
{

    unsigned char nthing = 0;
    unsigned char i_dam = 0;
    char str_pass[80];
    unsigned char keyin;

    if ( you.num_inv_items < 1 )
    {
        canned_msg(MSG_NOTHING_CARRIED);
        return;
    }

    if ( you.berserker )
    {
        canned_msg(MSG_TOO_BERSERK);
        return;
    }

    if (you.attribute[ATTR_TRANSFORMATION] != TRAN_NONE)
    {
        if ( !can_equip(0) )
        {
            mpr("You can't wield anything in your present form.");
            return;
        }
    }

    if ( you.equip[EQ_WEAPON] != -1
          && you.inv_class[you.equip[EQ_WEAPON]] == OBJ_WEAPONS
          && you.inv_plus[you.equip[EQ_WEAPON]] > 80 )
    {
        mpr("You can't unwield your weapon to draw a new one!");
        return;
    }

    if ( you.sure_blade )
    {
        mpr("The bond with your blade fades away.");
        you.sure_blade = 0;
    }

    if ( auto_wield )
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

        keyin = get_ch();

        if (keyin == '?')
        {
            //inventory(0);
            nthing = get_invent(OBJ_WEAPONS);

            if ( ( nthing >= 'A' && nthing <= 'Z' ) || ( nthing >= 'a' && nthing <= 'z' ) )
            {
                keyin = nthing;
            }
            else
            {
                mesclr();
                goto query;
                //mpr("Wield which item?");
                //keyin = get_ch();
            }
        }

        if (keyin == '*')
        {
            //inventory(-1);
            nthing = get_invent(-1);

            if ( ( nthing >= 'A' && nthing <= 'Z' ) || ( nthing >= 'a' && nthing <= 'z' ) )
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

    if ( item_wield_1 < 'A' || ( item_wield_1 > 'Z' && item_wield_1 < 'a' ) || item_wield_1 > 'z' )
    {
        mpr("You don't have any such object.");
        return;
    }

    int item_wield_2 = letter_to_index(item_wield_1);

    if ( item_wield_2 == you.equip[EQ_WEAPON] )
    {
        mpr("You are already wielding that!");
        return;
    }

    for (int i = EQ_CLOAK; i <= EQ_AMULET; i++)
      if  (item_wield_2 == you.equip[i] )
      {
          mpr("You are wearing that object!");
          return;
      }

    if (you.inv_quantity[item_wield_2] == 0)
    {
        mpr("You don't have any such object.");
        return;
    }

    if ( you.inv_class[item_wield_2] != OBJ_WEAPONS )
    {
        if (you.inv_class[item_wield_2] == OBJ_STAVES
            && you.equip[EQ_SHIELD] != -1)
        {
            mpr("You can't wield that with a shield.");
            return;
        }

        if ( you.equip[EQ_WEAPON] != -1 )
          unwield_item(you.equip[EQ_WEAPON]);

        you.equip[EQ_WEAPON] = item_wield_2;
    }
    else
    {

        if ((you.species < SP_OGRE || you.species > SP_OGRE_MAGE)
            && mass(you.inv_class[item_wield_2], you.inv_type[item_wield_2]) >= 500)
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
            return;

        }

        if (hands_reqd_for_weapon(you.inv_class[item_wield_2], you.inv_type[item_wield_2]) == HANDS_TWO_HANDED
            && you.equip[EQ_SHIELD] != -1)
        {
            mpr("You can't wield that with a shield.");
            return;
        }

        if ( ( you.is_undead || you.species == SP_DEMONSPAWN )
            && ( you.inv_dam[item_wield_2] < NWPN_SINGING_SWORD
                && ( you.inv_dam[item_wield_2] % 30 == SPWPN_HOLY_WRATH
                      || you.inv_dam[item_wield_2] % 30 == SPWPN_DISRUPTION ) ) )
        {
            mpr("This weapon will not allow you to wield it.");
            you.turn_is_over = 1;
            return;
        }

        if ( you.equip[EQ_WEAPON] != -1 )
          unwield_item(you.equip[EQ_WEAPON]);

        you.equip[EQ_WEAPON] = item_wield_2;
    }

    strcpy(info, " ");

    info[0] = index_to_letter(item_wield_2);
    info[1] = '\0';

    strcat(info, " - ");

    in_name(item_wield_2, 3, str_pass);
    strcat(info, str_pass);
    strcat(info, " (weapon in hand)");
    mpr(info);


// and here we finally get to the special effects of wielding {dlb}
    if ( you.inv_class[item_wield_2] == OBJ_MISCELLANY )
    {
        if ( you.inv_type[item_wield_2] == MISC_LANTERN_OF_SHADOWS )
        {
            mpr("The area is filled with flickering shadows.");
            you.special_wield = SPWLD_SHADOW;
        }
    }

    if (you.inv_class[item_wield_2] == OBJ_STAVES)
    {
        if ( you.inv_type[item_wield_2] == STAFF_POWER )
        {
            you.max_magic_points += 13;
            you.base_magic_points2 += 13;
            calc_mp();

            if ( you.inv_ident[you.equip[EQ_WEAPON]] < 3 )
            {
                you.inv_ident[you.equip[EQ_WEAPON]] = 3;
                strcpy(info, "You are wielding ");
                in_name(you.equip[EQ_WEAPON], 3, str_pass);
                strcat(info, str_pass);
                strcat(info, ".");
                mpr(info);

                more();

                wield_change = true;
            }
        }
    }

    if ( you.inv_class[item_wield_2] == OBJ_WEAPONS )
    {
        if ( is_demonic(you.inv_type[item_wield_2])
            && ( you.religion == GOD_ZIN || you.religion == GOD_SHINING_ONE || you.religion == GOD_ELYVILON ) )
          mpr("You really shouldn't be using a nasty item like this.");

        if ( you.inv_plus[item_wield_2] > 80 )
          mpr("It sticks to your hand!");

    // hmmm ... this seems a little fishy {dlb}:
        if ( you.inv_ident[item_wield_2] == 0 )
          you.inv_ident[item_wield_2] = 2;

        if ( you.inv_dam[item_wield_2] != SPWPN_NORMAL )
        {
            if ( you.inv_ident[item_wield_2] == 1 )
              you.inv_ident[item_wield_2] = 2;

            i_dam = you.inv_dam[item_wield_2];

            if ( you.inv_dam[item_wield_2] < NWPN_SINGING_SWORD )
              i_dam = (you.inv_dam[item_wield_2] % 30);

            if ( you.inv_dam[item_wield_2] % 30 >= SPWPN_RANDART_I )
              i_dam = randart_wpn_properties(you.inv_class[item_wield_2], you.inv_type[item_wield_2], you.inv_dam[item_wield_2], you.inv_plus[item_wield_2], you.inv_plus2[item_wield_2], 0, OBJ_WEAPONS);

            switch ( i_dam )
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
                  mpr ( (you.species == SP_HILL_ORC) ? "You feel a sudden desire to commit suicide."
                                                     : "You feel a sudden desire to kill orcs!" );
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
                  mpr("You feel a strange hunger.");
                  break;

                case SPWPN_DISRUPTION:
                  mpr("You sense a holy aura.");
                  break;

                case SPWPN_PAIN:
                  mpr("A searing pain shoots up your arm!");
                  break;

                case SPWPN_DISTORTION:
                  miscast_effect(SPTYP_TRANSLOCATION, 9, 90, 100);
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
                  if ( you.species != SP_MUMMY )    // josh declares mummies cannot smell {dlb}
                    mpr("You smell chlorine.");
                  break;

                case NWPN_VAMPIRES_TOOTH:
                  if ( you.species != SP_MUMMY )    // josh declares mummies cannot smell, and do not hunger {dlb}
                    mpr("You feel a strange hunger, and smell blood on the air...");
                  break;

                case NWPN_STAFF_OF_WUCAD_MU:
                  miscast_effect(SPTYP_DIVINATION, 9, 90, 100);
                  you.special_wield = SPWLD_WUCAD_MU;
                  break;
            }

            if ( you.inv_dam[item_wield_2] % 30 >= 25 )
              use_randart(item_wield_2);
        }
    }

    you.time_taken *= 5;
    you.time_taken /= 10;

    wield_change = true;
    you.turn_is_over = 1;

}                               // end wield_weapon()




//---------------------------------------------------------------
//
// armour_prompt
//
// Prompt the user for some armor. Returns true if the user picked
// something legit.
//
//---------------------------------------------------------------
bool armour_prompt( const string & mesg, int *index )
{
    ASSERT(index != NULL);

    bool succeeded = false;

    if ( you.num_inv_items < 1 )
      canned_msg(MSG_NOTHING_CARRIED);
    else if ( you.berserker )
      canned_msg(MSG_TOO_BERSERK);
    else
    {
query:
        mpr(mesg.c_str());

        unsigned char key = get_ch();

        if (key == '?' || key == '*')
        {
            unsigned char letter = '\0';

            if (key == '?')
              letter = get_invent(OBJ_ARMOUR);
            else
              letter = get_invent(-1);

            if ( ( letter >= 'A' && letter <= 'Z' ) || ( letter >= 'a' && letter <= 'z' ) )
            {
                key = letter;
            }
            else
            {
                mesclr();
                goto query;
            }
        }

        if ( key < 'A' || ( key > 'Z' && key < 'a' ) || key > 'z' )
        {
            mpr("You don't have any such object.");
        }
        else
        {
            *index = letter_to_index(key);
            succeeded = true;
        }
    }

    return succeeded;

}          // end armour_prompt()




//---------------------------------------------------------------
//
// wear_armour
//
//---------------------------------------------------------------
void wear_armour( void )
{

    char wh_equip = 0;

    int armour_wear_2;

    if ( !armour_prompt("Wear which item?", &armour_wear_2) )
      return;

    if ( you.inv_class[armour_wear_2] != OBJ_ARMOUR )
    {
        mpr("You can't wear any such thing.");
        return;
    }

    if ( you.inv_quantity[armour_wear_2] == 0 )
    {
        mpr("You don't have any such object.");
        return;
    }

    if ( armour_wear_2 == you.equip[EQ_WEAPON] )
    {
        mpr("You are wielding that object!");
        return;
    }

    for (int loopy = EQ_CLOAK; loopy <= EQ_BODY_ARMOUR ; loopy++)
      if ( armour_wear_2 == you.equip[loopy] )
      {
        mpr("You are already wearing that!");
        return;
      }

    // if you're wielding something,
    if ( you.equip[EQ_WEAPON] != -1 &&
    // attempting to wear a shield,
        (you.inv_type[armour_wear_2] == ARM_SHIELD ||
         you.inv_type[armour_wear_2] == ARM_BUCKLER ||
         you.inv_type[armour_wear_2] == ARM_LARGE_SHIELD) &&
    // weapon is two-handed
        hands_reqd_for_weapon(you.inv_class[you.equip[EQ_WEAPON]], you.inv_type[you.equip[EQ_WEAPON]]) == HANDS_TWO_HANDED )
    {
        mpr("You can't bear a shield *and* wield a two-handed weapon.");
        return;
    }

    if ( you.inv_type[armour_wear_2] == ARM_BOOTS )
    {
        if ( you.species != SP_NAGA && you.inv_plus2[armour_wear_2] == 1 )
        {
            mpr("You can't wear that!");
            return;
        }

        if ( you.species != SP_CENTAUR && you.inv_plus2[armour_wear_2] == 2 )
        {
            mpr("You can't wear that!");
            return;
        }
    }

    wh_equip = EQ_BODY_ARMOUR;

    switch ( you.inv_type[armour_wear_2] )
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
    else if (you.inv_type[armour_wear_2] == ARM_HELMET
             && you.inv_plus2[armour_wear_2] > 1)
    {
        // caps & wiz hats always fit, unless your head's too big (ogres &c)
    }
    else if ( !can_equip(wh_equip) )
    {
        mpr("You can't wear that in your present form.");
        return;
    }

    // Giant races
    if ( ( you.species >= SP_OGRE && you.species <= SP_OGRE_MAGE ) || player_genus(GENPC_DRACONIAN) )
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
        && ( you.skills[SK_ARMOUR] < 10
              || you.species == SP_KOBOLD
              || you.species == SP_HALFLING
              || you.species == SP_GNOME
              || player_genus(GENPC_ELVEN) ) )
    {
        mpr("This armour is very cumbersome.");
    }

    if (you.inv_type[armour_wear_2] == ARM_PLATE_MAIL
        && ( you.skills[SK_ARMOUR] < 5
              || you.species == SP_HALFLING
              || you.species == SP_GNOME
              || player_genus(GENPC_ELVEN)
              || you.species == SP_KOBOLD) )
    {
        mpr("This armour is uncomfortably cumbersome.");
    }

    if ( you.inv_type[armour_wear_2] == ARM_BANDED_MAIL
          || you.inv_type[armour_wear_2] == ARM_SPLINT_MAIL
        && ( you.skills[SK_ARMOUR] < 3
              || you.species == SP_GNOME
              || you.species == SP_HALFLING ) )
    {
        mpr("You find this armour uncomfortably cumbersome.");
    }

    if ( you.inv_type[armour_wear_2] == ARM_CRYSTAL_PLATE_MAIL
        && ( you.skills[SK_ARMOUR] < 8
              || you.species == SP_GNOME
              || you.species == SP_HALFLING
              || you.species == SP_SPRIGGAN
              || player_genus(GENPC_ELVEN) ) )
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
              if (you.duration[ DUR_CONDENSATION_SHIELD ])
              {
                  mpr("Your icy shield evaporates.");
                  you.duration[ DUR_CONDENSATION_SHIELD ] = 0;
              }
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
      you.delay_t += property(you.inv_class[armour_wear_2], you.inv_type[armour_wear_2], PARM_AC);

    you.delay_doing = 1;

    if ( you.inv_ident[armour_wear_2] < 3 )
      you.inv_ident[armour_wear_2] = 3;

    if ( removedCloak )
    {
        strcpy(info, "Your cloak hinders you, but you put on ");

        // if we are exhanging body armor then we've already
        // accounted for taking the cloak on and off
        if (!removedBodyArmour)
            you.delay_t += 2 * property(you.inv_class[you.equip[EQ_CLOAK]], you.inv_type[you.equip[EQ_CLOAK]], PARM_AC);
    }
    else
      strcpy(info, "You put on ");

    in_name(armour_wear_2, 5, str_pass);
    strcat(info, str_pass);
    strcat(info, ".");
    mpr(info);

    if ( you.inv_dam[armour_wear_2] != SPARM_NORMAL )
    {
        switch ( you.inv_dam[armour_wear_2] % 30 )
         {
        case SPARM_RUNNING:
            strcpy(info, "You feel quick");
            strcat(info, (you.species == SP_NAGA || you.species == SP_CENTAUR) ? "." : " on your feet." );
            mpr(info);
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
            if ( !you.invis )
              mpr("You become transparent for a moment.");
            break;

        case SPARM_STRENGTH:
            increase_stats(STAT_STRENGTH, 3, false);
            break;

        case SPARM_DEXTERITY:
            increase_stats(STAT_DEXTERITY, 3, false);
            break;

        case SPARM_INTELLIGENCE:
            increase_stats(STAT_INTELLIGENCE, 3, false);
            break;

        case SPARM_PONDEROUSNESS:
            mpr("You feel rather ponderous.");
            you.speed += 2;
            you.redraw_evasion = 1;
            break;

        case SPARM_LEVITATION:
            mpr("You feel rather swift.");
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
            if ( !you.skills[SK_SPELLCASTING] )
              mpr("You feel strangely numb.");
            else
              mpr("You feel extremely powerful.");
            break;
        }
    }

    if ( you.inv_dam[armour_wear_2] % 30 >= SPARM_RANDART_I )
      use_randart(armour_wear_2);

}          // end wear_armour()




bool takeoff_armour( int item )
{

    if ( you.inv_class[item] != OBJ_ARMOUR )
    {
        mpr("You aren't wearing that!");
        return false;
    }

    if ( you.inv_plus[item] > 130 )
    {
        for (int loopy = EQ_CLOAK; loopy <= EQ_BODY_ARMOUR ; loopy++)
          if ( item == you.equip[loopy] )
          {
              strcpy(info, "The ");
              in_name(item, 5, str_pass);
              strcat(info, str_pass);
              strcat(info, " is stuck to your body!");
              mpr(info);
              return false;
          }
    }

    bool removedCloak = false;

    if ( you.inv_type[item] < ARM_SHIELD || you.inv_type[item] > ARM_LARGE_SHIELD )
    {
        if ( item != you.equip[EQ_BODY_ARMOUR] )
        {
            mpr("You aren't wearing that!");
            return false;
        }

        if ( you.equip[EQ_CLOAK] != -1 )
        {
            if ( you.inv_plus[you.equip[EQ_CLOAK]] <= 130 )
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
        switch ( you.inv_type[item] )
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
    you.delay_t = property(you.inv_class[item], you.inv_type[item], PARM_AC);
    you.delay_doing = 2;

    if ( removedCloak )
    {
        you.delay_t += 2 * property(you.inv_class[you.equip[EQ_CLOAK]], you.inv_type[you.equip[EQ_CLOAK]], PARM_AC);
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

}          // end takeoff_armour()




void throw_anything( void )
{

    unsigned char nthing = 0;
    struct bolt beam[1];

    if ( you.berserker )
    {
        canned_msg(MSG_TOO_BERSERK);
        return;
    }
    else if ( you.num_inv_items < 1 )
    {
        canned_msg(MSG_NOTHING_CARRIED);
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
        if ( ( nthing >= 'A' && nthing <= 'Z' ) || ( nthing >= 'a' && nthing <= 'z' ) )
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

    if ( throw_1 < 'A' || ( throw_1 > 'Z' && throw_1 < 'a' ) || throw_1 > 'z' )
    {
        mpr("You don't have any such object.");
        return;
    }

    int throw_2 = letter_to_index(throw_1);

    if ( you.inv_quantity[throw_2] == 0 )
    {
        mpr("You don't have any such object.");
        return;
    }
    else if ( throw_2 == you.equip[EQ_WEAPON] && you.inv_plus[you.equip[EQ_WEAPON]] > 80 )
    {
        mpr("That thing is stuck to your hand!");
        return;
    }
    else
    {
        for ( int loopy = EQ_CLOAK; loopy <= EQ_AMULET; loopy++)
          if ( throw_2 == you.equip[loopy] )
          {
              mpr("You are wearing that object!");
              return;
          }
    }

    throw_it(&beam[0], throw_2);

}          // end throw_anything()




void shoot_thing( void )
{

    int i = 0;
    struct bolt beam[1];

    if ( you.berserker )
    {
        canned_msg(MSG_TOO_BERSERK);
        return;
    }

    int type_wanted = MI_DART;

// the order here is *terribly* important {dlb}:
    type_wanted = ( (you.equip[EQ_WEAPON] == -1)                           ? MI_DART :
                    (you.inv_class[you.equip[EQ_WEAPON]] != OBJ_WEAPONS)   ? MI_DART :
                    (!launches_things(you.inv_type[you.equip[EQ_WEAPON]])) ? MI_DART
                                                                           : launched_by(you.inv_type[you.equip[EQ_WEAPON]]) );

    for (i = 0; i < (ENDOFPACK + 1); i++)
    {
        if ( i == ENDOFPACK )
        {
            mpr("You have nothing suitable.");
            return;
        }

        if ( you.inv_quantity[i] && you.inv_class[i] == OBJ_MISSILES && you.inv_type[i] == type_wanted )
          break;
    }

    throw_it(&beam[0], i);    // recall that i is the slot that breaks the above loop {dlb}

}          // end shoot_thing()




static void throw_it( struct bolt *pbolt, int throw_2 )
{

    char shoot = 0;
    struct dist thr[1];
    char shoot_skill = 0;

    mpr("Which direction? (*/+ to target)");

    message_current_target();

    direction(1, thr);

    if ( thr[0].nothing == -1 )
      return;

    if ( you.conf )
    {
        thr[0].move_x = random2(13) - 6;
        thr[0].move_y = random2(13) - 6;
    }

    pbolt->move_x = thr[0].move_x;
    pbolt->move_y = thr[0].move_y;
    pbolt->target_x = thr[0].target_x;
    pbolt->target_y = thr[0].target_y;
    pbolt->range = 5;          /* provisional until mass variable used */
    pbolt->flavour = BEAM_MISSILE;

    switch (you.inv_class[throw_2])
    {
    case OBJ_WEAPONS:
        pbolt->type = SYM_WEAPON;
        pbolt->range = 9;
        break;
    case OBJ_MISSILES:
        pbolt->type = SYM_MISSILE;
        pbolt->range = 9;
        break;
    case OBJ_ARMOUR:
        pbolt->type = SYM_ARMOUR;
        break;
    case OBJ_WANDS:
        pbolt->type = SYM_STICK;
        break;
    case OBJ_FOOD:
        pbolt->type = SYM_CHUNK;
        break;
    case OBJ_UNKNOWN_I:
        pbolt->type = SYM_BURST;
        break;
    case OBJ_SCROLLS:
        pbolt->type = SYM_SCROLL;
        break;
    case OBJ_JEWELLERY:
        pbolt->type = SYM_TRINKET;
        break;
    case OBJ_POTIONS:
        pbolt->type = SYM_FLASK;
        break;
    case OBJ_UNKNOWN_II:
        pbolt->type = SYM_ZAP;
        break;
    case OBJ_BOOKS:
        pbolt->type = SYM_OBJECT;
        break;
    case OBJ_STAVES:              // this does not seem right, but value was 11 {dlb}
        pbolt->type = SYM_CHUNK; // notice how the .type does not match the class -- hmmm... {dlb}
        break;
    }

    pbolt->source_x = you.x_pos;
    pbolt->source_y = you.y_pos;
    pbolt->colour = you.inv_colour[throw_2];

    item_name(you.inv_plus2[throw_2], you.inv_class[throw_2], you.inv_type[throw_2], you.inv_dam[throw_2], you.inv_plus[throw_2], 1, you.inv_ident[throw_2], 6, str_pass);
    strcpy(pbolt->beam_name, str_pass);

    pbolt->thing_thrown = KILL_YOU;

    pbolt->hit = (random2(you.dex)) / 2;
    pbolt->hit += random2(you.skills[SK_THROWING] + 1);
    pbolt->hit += slaying_bonus(PWPN_HIT);

    pbolt->damage += random2(you.skills[SK_THROWING] + 1);
    pbolt->damage += slaying_bonus(PWPN_DAMAGE);

    if ( you.inv_class[throw_2] == OBJ_MISSILES && you.inv_type[throw_2] == MI_DART )
    {
        if ( you.equip[EQ_WEAPON] == -1
              || you.inv_class[you.equip[EQ_WEAPON]] != OBJ_WEAPONS
              || you.inv_type[you.equip[EQ_WEAPON]] != WPN_HAND_CROSSBOW )
        {
            exercise(SK_DARTS, 1 + random2avg(3,2));

            if ( coinflip() )
              exercise(SK_THROWING, 1);

            pbolt->hit += random2avg(1 + (you.skills[SK_DARTS] << 1), 2) + random2(1 + (you.skills[SK_THROWING] / 2));
        }
    }

    if ( you.inv_class[throw_2] == OBJ_WEAPONS || you.inv_class[throw_2] == OBJ_MISSILES )
    {
        pbolt->damage = property(you.inv_class[throw_2], you.inv_type[throw_2], PWPN_DAMAGE);

    // Missile devices (e.g. bows):
        if ( you.inv_class[you.equip[EQ_WEAPON]] == OBJ_WEAPONS
            && launches_things(you.inv_type[you.equip[EQ_WEAPON]])
            && you.inv_type[throw_2] == launched_by(you.inv_type[you.equip[EQ_WEAPON]]) )
        {
            pbolt->damage = property(you.inv_class[throw_2], you.inv_type[throw_2], PWPN_HIT);    // value replaced was "1" -- was it intended to be that? {dlb}

            if (you.inv_type[you.equip[EQ_WEAPON]] == WPN_CROSSBOW)
            {
                you.time_taken *= 12;
                you.time_taken /= 10;
            }

            int hoggl = you.inv_plus2[you.equip[EQ_WEAPON]] - 50;

            if (you.inv_plus2[you.equip[EQ_WEAPON]] > 80)
                hoggl -= 100;

            pbolt->damage += hoggl;
            pbolt->range += 5;

            // removed 2 random2(2)s from each of the learning curves, but
            // left slings because they're hard enough to develop without
            // a good source of shot in the dungeon.
            switch (you.inv_type[you.equip[EQ_WEAPON]])
            {
            case WPN_SLING:
                // Slings are really easy to learn because they're not
                // really all that good, and its harder to get ammo anyways.
                exercise(SK_SLINGS, 1 + random2avg(3,2));
                pbolt->hit += random2(1 + you.skills[SK_SLINGS]) + random2(1 + random2(you.skills[SK_SLINGS]));
                pbolt->damage += random2(1 + you.skills[SK_SLINGS]);
                shoot_skill = you.skills[SK_SLINGS];
                break;

            case WPN_BOW:
                exercise(SK_BOWS, (coinflip() ? 2 : 1));
                pbolt->hit += random2avg(1 + (you.skills[SK_BOWS] << 1), 2);
                pbolt->damage += random2(1 + you.skills[SK_BOWS]) + random2(1 + random2(you.skills[SK_BOWS]));
                shoot_skill = you.skills[SK_BOWS];
                break;

                // Crossbows are easy for unskilled people so they get a
                // contant bonus added to hit and dam.  Note that the
                // values of 10 and 5 make bows better at skill level 10.

            case WPN_CROSSBOW:
                exercise(SK_CROSSBOWS, (coinflip() ? 2 : 1));
                pbolt->hit += random2(1 + you.skills[SK_CROSSBOWS]) + random2(10);
                pbolt->damage += random2(1 + you.skills[SK_CROSSBOWS]) + random2(5);
                shoot_skill = you.skills[SK_CROSSBOWS];
                break;

            case WPN_HAND_CROSSBOW:
                exercise(SK_CROSSBOWS, (coinflip() ? 2 : 1));
                pbolt->hit += random2(1 + you.skills[SK_CROSSBOWS]) + random2(6);
                pbolt->damage += random2(1 + you.skills[SK_CROSSBOWS]) + random2(3);
                shoot_skill = you.skills[SK_CROSSBOWS];
                break;
            }


        // I'm sorry, I just don't understand how throwing can be such
        // a general skills as to be exercised by everything {dlb}:
            if ( coinflip() )
              exercise(SK_THROWING, 1);

            int ghoggl = you.inv_plus[you.equip[EQ_WEAPON]] - 50;

            if ( you.inv_plus[you.equip[EQ_WEAPON]] > 80 )
              ghoggl -= 100;

            pbolt->hit += ghoggl;

            pbolt->range += 3;

            if ( you.inv_dam[you.equip[EQ_WEAPON]] / 30 == you.inv_dam[throw_2] / 30 )
            {
                pbolt->hit++;
                pbolt->damage++;
                if ( you.inv_dam[you.equip[EQ_WEAPON]] / 30 == DWPN_ELVEN
                    && player_genus(GENPC_ELVEN) )
                  pbolt->hit++;
            }


            if ((you.inv_dam[you.equip[EQ_WEAPON]] % 30 == SPWPN_FLAME
                 || you.inv_dam[throw_2] % 30 == SPMSL_FLAME
                 || (you.inv_dam[you.equip[EQ_WEAPON]] % 30 >= 25
                     && randart_wpn_properties(you.inv_class[you.equip[EQ_WEAPON]], you.inv_type[you.equip[EQ_WEAPON]], you.inv_dam[you.equip[EQ_WEAPON]], you.inv_plus[you.equip[EQ_WEAPON]], you.inv_plus2[you.equip[EQ_WEAPON]], 0, RAP_BRAND) == SPWPN_FLAME))
                && you.inv_dam[throw_2] % 30 != SPMSL_ICE
                && you.inv_dam[you.equip[EQ_WEAPON]] % 30 != SPWPN_FROST)
            {
                pbolt->damage += 1 + random2(5);
                pbolt->flavour = BEAM_FIRE;
                strcpy(pbolt->beam_name, "bolt of ");
                if ( you.inv_dam[throw_2] % 30 == SPMSL_POISONED || you.inv_dam[throw_2] % 30 == SPMSL_POISONED_II )
                  strcat(pbolt->beam_name, "poison ");
                strcat(pbolt->beam_name, "flame");
                pbolt->colour = RED;
                pbolt->type = SYM_BOLT;
                pbolt->thing_thrown = KILL_YOU_MISSILE;
                you.inv_ident[throw_2] = 2;
            }

            if ((you.inv_dam[you.equip[EQ_WEAPON]] % 30 == SPWPN_FROST
                 || you.inv_dam[throw_2] % 30 == SPMSL_ICE
                 || (you.inv_dam[you.equip[EQ_WEAPON]] % 30 >= 25
                     && randart_wpn_properties(you.inv_class[you.equip[EQ_WEAPON]], you.inv_type[you.equip[EQ_WEAPON]], you.inv_dam[you.equip[EQ_WEAPON]], you.inv_plus[you.equip[EQ_WEAPON]], you.inv_plus2[you.equip[EQ_WEAPON]], 0, RAP_BRAND) == SPWPN_FROST))
                && you.inv_dam[throw_2] % 30 != SPMSL_FLAME
                && you.inv_dam[you.equip[EQ_WEAPON]] % 30 != SPWPN_FLAME)
            {
                pbolt->damage += 1 + random2(5);
                pbolt->flavour = BEAM_COLD;
                strcpy(pbolt->beam_name, "bolt of ");
                if ( you.inv_dam[throw_2] % 30 == SPMSL_POISONED || you.inv_dam[throw_2] % 30 == SPMSL_POISONED_II )
                  strcat(pbolt->beam_name, "poison ");
                strcat(pbolt->beam_name, "frost");
                pbolt->colour = WHITE;
                pbolt->type = SYM_BOLT;
                pbolt->thing_thrown = KILL_YOU_MISSILE;
                you.inv_ident[throw_2] = 2;
            }
            /* the chief advantage here is the extra damage this does against susceptible creatures */

            /* Note: weapons & ammo of eg fire are not cumulative
               ammo of fire and weapons of frost don't work together, and vice versa */

            if (you.inv_plus[you.equip[EQ_WEAPON]] > 80)
            {
                pbolt->damage -= 100;
                pbolt->hit -= 100;
            }

            shoot = 1;

            if ( you.inv_ident[you.equip[EQ_WEAPON]] < 3
                && random2(100) < shoot_skill )
            {
                you.inv_ident[you.equip[EQ_WEAPON]] = 3;
                strcpy(info, "You are wielding ");
                in_name(you.equip[EQ_WEAPON], 3, str_pass);
                strcat(info, str_pass);
                strcat(info, ".");
                mpr(info);

                more();

                wield_change = true;
            }
            strcpy(info, "You shoot ");
        }

        int hoggl = you.inv_plus[throw_2] - 50;

        if ( you.inv_plus[throw_2] > 80 )
          hoggl -= 100;

        hoggl = (hoggl % 10);

        pbolt->damage += hoggl;

        int ghoggl = you.inv_plus[throw_2] - 50;

        if ( you.inv_plus[throw_2] > 80 )
          ghoggl -= 100;

        ghoggl = (ghoggl / 10);
        pbolt->hit += ghoggl;

        if ( you.inv_class[throw_2] == OBJ_WEAPONS )
        {
            if ( you.inv_type[throw_2] == WPN_DAGGER
                  || you.inv_type[throw_2] == WPN_HAND_AXE
                  || you.inv_type[throw_2] == WPN_SPEAR )
            {
                pbolt->hit += random2(1 + (you.skills[SK_THROWING] / 2));
                pbolt->damage++;

                if ( coinflip() )
                  exercise(SK_THROWING, 1);
            }
            else
            {
                pbolt->hit -= 3;
                pbolt->damage--;
            }
        }
    }
    else
      pbolt->damage = 2;     /* later put mass thing here. */


    if ( shoot == 0 )
      strcpy(info, "You throw ");
    /* if shoot == 1, it has already strcpy'd "You shoot " */
    item_name(you.inv_plus2[throw_2], you.inv_class[throw_2], you.inv_type[throw_2], you.inv_dam[throw_2], you.inv_plus[throw_2], 1, you.inv_ident[throw_2], 3, str_pass);

    strcat(info, str_pass);
    strcat(info, ".");
    mpr(info);

    if ( you.equip[EQ_WEAPON] == throw_2 )
      unwield_item(throw_2);

    missile(pbolt, throw_2);

    you.inv_quantity[throw_2]--;

    if ( you.inv_quantity[throw_2] == 0 )
    {
        you.num_inv_items--;

        if (you.equip[EQ_WEAPON] == throw_2)
        {
            you.equip[EQ_WEAPON] = -1;
            mpr("You are now empty handed.");
        }
    }

    burden_change();

    alert();

    you.turn_is_over = 1;

}          // end throw_it()




void puton_ring( void )
{

    unsigned char nthing = 0;

    if ( you.num_inv_items < 1 )
    {
        canned_msg(MSG_NOTHING_CARRIED);
        return;
    }

    if ( you.berserker )
    {
        canned_msg(MSG_TOO_BERSERK);
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
        if ( ( nthing >= 'A' && nthing <= 'Z' ) || ( nthing >= 'a' && nthing <= 'z' ) )
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

    if ( ring_wear_1 < 'A' || (ring_wear_1 > 'Z' && ring_wear_1 < 'a' ) || ring_wear_1 > 'z' )
    {
        mpr("You don't have any such object.");
        return;
    }

    int ring_wear_2 = letter_to_index(ring_wear_1);

    if (ring_wear_2 == you.equip[EQ_LEFT_RING]
        || ring_wear_2 == you.equip[EQ_RIGHT_RING]
        || ring_wear_2 == you.equip[EQ_AMULET])
    {
        mpr("You've already put that on!");
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
      //jmf: let's not take our inferiority complex out on players, eh? :-p
      //mpr("You're sadly mistaken if you consider that jewellery.")
      mpr("You can only put on jewelry.");
      return;
    }

    if (you.inv_type[ring_wear_2] < AMU_RAGE)
    {
      if (you.equip[EQ_GLOVES] != -1 && you.inv_plus[you.equip[EQ_GLOVES]] >80)
        {
          mpr("You can't take your gloves off to put on a ring!");
          return;
        }

      if (you.inv_class[ring_wear_2] == OBJ_JEWELLERY
          && you.equip[EQ_LEFT_RING] != -1
          && you.equip[EQ_RIGHT_RING] != -1)
        {
          // and you are trying to wear body you.equip.
          mpr("You've already put a ring on each hand.");
          return;
        }
    }
    else if (you.equip[EQ_AMULET] != -1)
    {
      strcpy(info, "You are already wearing an amulet.");

      if (one_chance_in(20))
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

    switch ( you.inv_type[ring_wear_2] )
    {
    case RING_FIRE:
    case RING_HUNGER:
    case RING_ICE:
    case RING_LIFE_PROTECTION:
    case RING_POISON_RESISTANCE:
    case RING_PROTECTION_FROM_COLD:
    case RING_PROTECTION_FROM_FIRE:
    case RING_PROTECTION_FROM_MAGIC:
    case RING_SUSTAIN_ABILITIES:
    case RING_SUSTENANCE:
    case RING_SLAYING:
    case RING_SEE_INVISIBLE:
    case RING_TELEPORTATION:
    case RING_WIZARDRY:
        break;

    case RING_REGENERATION:
        if ( you.hp < you.hp_max )
          set_id(you.inv_class[ring_wear_2], you.inv_type[ring_wear_2], 1);   // this is a bit dodgy
        break;

    case RING_PROTECTION:
        you.redraw_armor_class = 1;

        if ( you.inv_plus[ring_wear_2] != 0 && you.inv_plus[ring_wear_2] != 100 )
        {
            set_id(you.inv_class[ring_wear_2], you.inv_type[ring_wear_2], 1);
            you.inv_ident[ring_wear_2] = 3;
        }
        break;

    case RING_INVISIBILITY:
        //if (you.invis > 0) you.invis = 0;
        if ( !you.invis )
          mpr("You become transparent for a moment.");
        set_id(you.inv_class[ring_wear_2], you.inv_type[ring_wear_2], 1);
        you.inv_ident[ring_wear_2] = 3;
        //you.invis--;
        break;

    case RING_EVASION:
        you.redraw_evasion = 1;
        if (you.inv_plus[ring_wear_2] != 0 && you.inv_plus[ring_wear_2] != 100)
        {
            you.inv_ident[ring_wear_2] = 3;
            set_id(you.inv_class[ring_wear_2], you.inv_type[ring_wear_2], 1);
        }
        break;

    case RING_STRENGTH:
        increase_stats(STAT_STRENGTH, (you.inv_plus[ring_wear_2] % 100), true);
        decrease_stats(STAT_STRENGTH, 50, true);
        if (you.inv_plus[ring_wear_2] != 0 && you.inv_plus[ring_wear_2] != 100)
        {
            set_id(you.inv_class[ring_wear_2], you.inv_type[ring_wear_2], 1);
            you.inv_ident[ring_wear_2] = 3;
        }
        break;

    case RING_DEXTERITY:
        increase_stats(STAT_DEXTERITY, (you.inv_plus[ring_wear_2] % 100), true);
        decrease_stats(STAT_DEXTERITY, 50, true);
        if (you.inv_plus[ring_wear_2] != 0 && you.inv_plus[ring_wear_2] != 100)
        {
            you.inv_ident[ring_wear_2] = 3;
            set_id(you.inv_class[ring_wear_2], you.inv_type[ring_wear_2], 1);
        }
        break;

    case RING_INTELLIGENCE:
        increase_stats(STAT_INTELLIGENCE, (you.inv_plus[ring_wear_2] % 100), true);
        decrease_stats(STAT_INTELLIGENCE, 50, true);
        if (you.inv_plus[ring_wear_2] != 0 && you.inv_plus[ring_wear_2] != 100)
        {
            you.inv_ident[ring_wear_2] = 3;
            set_id(you.inv_class[ring_wear_2], you.inv_type[ring_wear_2], 1);
        }
        break;

    case RING_MAGICAL_POWER:
        you.max_magic_points += 9;
        you.base_magic_points2 += 9;
        calc_mp();
        set_id(you.inv_class[ring_wear_2], you.inv_type[ring_wear_2], 1);
        break;

    case RING_LEVITATION:
        mpr("You feel buoyant.");
        set_id(you.inv_class[ring_wear_2], you.inv_type[ring_wear_2], 1);
        break;

    case RING_TELEPORT_CONTROL:
        you.attribute[ATTR_CONTROL_TELEPORT]++;
        break;


        /* all of the amulets so far existing are not needed here, but have their
           presence queried whenever relevant through wearing_amulet in fn2.cc */
    }

    you.turn_is_over = 1;

    if ( you.inv_dam[ring_wear_2] == 200 || you.inv_dam[ring_wear_2] == 201 )
      use_randart(ring_wear_2);

    if ( you.inv_plus[ring_wear_2] > 80 )         // cursed
    {
        mpr("Oops, that ring feels deathly cold.");

        if ( you.inv_ident[ring_wear_2] == 0 )
          you.inv_ident[ring_wear_2] = 1;
    }

    strcpy(info, " ");

    info[0] = index_to_letter(ring_wear_2);
    info[1] = '\0';

    strcat(info, " - ");

    in_name(ring_wear_2, 3, str_pass);

    strcat(info, str_pass);

    strcat(info, (hand_used == 0) ? " (on left hand)" :
                 (hand_used == 1) ? " (on right hand)" :
                 (hand_used == 2) ? " (around neck)"
                                  : "" );

    mpr(info);

}                               // end puton_ring()




void remove_ring( void )
{

    unsigned char nthing = 0;
    int hand_used = 10;
    unsigned char keyin;
    int ring_wear_2;

    if ( you.equip[EQ_LEFT_RING] == -1 && you.equip[EQ_RIGHT_RING] == -1 && you.equip[EQ_AMULET] == -1 )
    {
        mpr("You aren't wearing any rings or amulets.");
        return;
    }

    if ( you.berserker )
    {
        canned_msg(MSG_TOO_BERSERK);
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
            if ( ( nthing >= 'A' && nthing <= 'Z' ) || ( nthing >= 'a' && nthing <= 'z' ) )
            {
                keyin = nthing;
            }
            else
            {
                mesclr();
                goto query;
            }
        }

        int equipn = letter_to_index(keyin);

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

    if ( you.equip[EQ_GLOVES] != -1 && you.inv_plus[you.equip[EQ_GLOVES]] > 80 && ( hand_used == 0 || hand_used == 1 ) )
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
        if ( you.inv_ident[you.equip[hand_used + 7]] == 0 )
          you.inv_ident[you.equip[hand_used + 7]] = 1;
        return;
    }

    strcpy(info, "You remove your ");
    in_name(you.equip[hand_used + 7], 8, str_pass);

    strcat(info, str_pass);
    strcat(info, ".");
    mpr(info);

    ring_wear_2 = you.equip[hand_used + 7];     // I'll still use ring_wear_2 here.


    switch ( you.inv_type[ring_wear_2] )
    {
      case RING_FIRE:
      case RING_HUNGER:
      case RING_ICE:
      case RING_LIFE_PROTECTION:
      case RING_POISON_RESISTANCE:
      case RING_PROTECTION_FROM_COLD:
      case RING_PROTECTION_FROM_FIRE:
      case RING_PROTECTION_FROM_MAGIC:
      case RING_REGENERATION:
      case RING_SEE_INVISIBLE:
      case RING_SLAYING:
      case RING_SUSTAIN_ABILITIES:
      case RING_SUSTENANCE:
      case RING_TELEPORTATION:
      case RING_WIZARDRY:
        break;

      case RING_PROTECTION:
        you.redraw_armor_class = 1;
        break;

      case RING_EVASION:
        you.redraw_evasion = 1;
        break;

      case RING_STRENGTH:
        decrease_stats(STAT_STRENGTH, (you.inv_plus[ring_wear_2] % 100), true);
        increase_stats(STAT_STRENGTH, 50, true);
        break;

      case RING_DEXTERITY:
        decrease_stats(STAT_DEXTERITY, (you.inv_plus[ring_wear_2] % 100), true);
        increase_stats(STAT_DEXTERITY, 50, true);
        break;

      case RING_INTELLIGENCE:
        decrease_stats(STAT_INTELLIGENCE, (you.inv_plus[ring_wear_2] % 100), true);
        increase_stats(STAT_INTELLIGENCE, 50, true);
        break;

      case RING_INVISIBILITY:
        if ( you.invis )
          you.invis = 1;     // removing this ring effectively cancels all standing invisibility {dlb}
        break;

      case RING_LEVITATION:
        if ( you.levitation )
          you.levitation = 1;     // removing this ring effectively cancels all standing levitation {dlb}
        break;

      case RING_MAGICAL_POWER:
        you.max_magic_points -= 9;
        you.base_magic_points2 -= 9;
        if ( you.magic_points > you.max_magic_points )
          you.magic_points = you.max_magic_points;
        calc_mp();
        break;

      case RING_TELEPORT_CONTROL:
        you.attribute[ATTR_CONTROL_TELEPORT]--;
        break;
    }

    if ( you.inv_dam[ring_wear_2] == 200 || you.inv_dam[ring_wear_2] == 201 )
      unuse_randart(ring_wear_2);

    you.equip[hand_used + 7] = -1;

    you.turn_is_over = 1;

}          // end remove_ring()




void zap_wand( void )
{

    int zap_device_1;
    int zap_device_2;
    unsigned char nthing = 0;

    struct bolt beam[1];
    struct dist zap_wand[1];

    beam[0].wand_id = 0;

    if ( you.num_inv_items < 1 )
    {
        canned_msg(MSG_NOTHING_CARRIED);
        return;
    }

    if ( you.berserker )
    {
        canned_msg(MSG_TOO_BERSERK);
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
        if ( ( nthing >= 'A' && nthing <= 'Z' ) || ( nthing >= 'a' && nthing <= 'z' ) )
        {
            keyin = nthing;
        }
        else
        {
            //mesclr();
            goto query;

        }
    }

    zap_device_1 = (int) keyin;

    if ( zap_device_1 < 'A' || ( zap_device_1 > 'Z' && zap_device_1 < 'a' ) || zap_device_1 > 'z' )
    {
Dont_have:
        mpr("You don't have any such object.");
        return;
    }

    zap_device_2 = letter_to_index(zap_device_1);

    if (you.inv_quantity[zap_device_2] == 0)
    {
        goto Dont_have;
    }

    if ( you.inv_class[zap_device_2] != OBJ_WANDS || you.inv_plus[zap_device_2] < 1 )
    {
        canned_msg(MSG_NOTHING_HAPPENS);
        you.turn_is_over = 1;
        return;
    }

    mpr("Which direction? (*/+ to target)");

    message_current_target();

    direction(1, zap_wand);

    if ( zap_wand[0].nothing == -1 )
      return;

    if ( you.conf )
    {
        zap_wand[0].move_x = random2(13) - 6;
        zap_wand[0].move_y = random2(13) - 6;
    }

    beam[0].source_x = you.x_pos;
    beam[0].source_y = you.y_pos;


// blargh! blech! this is just begging to be a problem ...
// not to mention work-around after work-around as wands are
// added, removed, or altered {dlb}:
    char type_zapped = you.inv_type[zap_device_2];

    if (type_zapped == WAND_ENSLAVEMENT)
        type_zapped = ZAP_ENSLAVEMENT;
    if (type_zapped == WAND_DRAINING)
        type_zapped = ZAP_NEGATIVE_ENERGY;
    if (type_zapped == WAND_DISINTEGRATION)
        type_zapped = ZAP_DISINTEGRATION;
    if (type_zapped == WAND_RANDOM_EFFECTS)
    {
        type_zapped = random2(16);
        if (one_chance_in(20))
            type_zapped = ZAP_NEGATIVE_ENERGY;
        if (one_chance_in(17))
            type_zapped = ZAP_ENSLAVEMENT;
    }

    beam[0].move_x = zap_wand[0].move_x;
    beam[0].move_y = zap_wand[0].move_y;
    beam[0].target_x = zap_wand[0].target_x;
    beam[0].target_y = zap_wand[0].target_y;

    zapping(type_zapped, 40, &beam[0]);

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

    if (get_id(you.inv_class[zap_device_2], you.inv_type[zap_device_2]) == 1 && (you.inv_ident[zap_device_2] > 1 || you.skills[SK_ENCHANTMENTS] > 5 + random2(15)))
    {
        if (you.inv_ident[zap_device_2] <= 1)
            mpr("Your skill with enchantments lets you calculate the power of this device...");

        strcpy(info, "This wand has ");
        itoa(you.inv_plus[zap_device_2], st_prn, 10);
        strcat(info, st_prn);
        strcat(info, " charge");

        if ( you.inv_plus[zap_device_2] > 1 )
          strcat(info, "s");

        strcat(info, " left.");
        mpr(info);

        you.inv_ident[zap_device_2] = 3;
    }

    you.turn_is_over = 1;
    alert();

}                               // end zap_wand()




void drink( void )
{
    int drink_1, drink_2;
    unsigned char nthing = 0;
    unsigned char keyin;

    if (you.is_undead == US_UNDEAD)
    {
        mpr("You can't drink.");
        return;
    }

    if ( grd[you.x_pos][you.y_pos] == DNGN_BLUE_FOUNTAIN
          || grd[you.x_pos][you.y_pos] == DNGN_SPARKLING_FOUNTAIN )
    {
        if ( drink_fountain() )
          return;
    }

    if ( you.num_inv_items < 1 )
    {
        canned_msg(MSG_NOTHING_CARRIED);
        return;
    }

    if ( you.berserker )
    {
        canned_msg(MSG_TOO_BERSERK);
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
        if ( ( nthing >= 'A' && nthing <= 'Z' ) || ( nthing >= 'a' && nthing <= 'z' ) )
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

    if ( drink_1 < 'A' || ( drink_1 > 'Z' && drink_1 < 'a' ) || drink_1 > 'z' )
    {
        mpr("You don't have any such object.");
        return;
    }

    drink_2 = letter_to_index(drink_1);

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

    lessen_hunger(40, true);

    you.inv_ident[drink_2] = 1;

    set_id(you.inv_class[drink_2], you.inv_type[drink_2], 1);

}          // end drink()




bool drink_fountain( void )
{

    bool gone_dry = false;
    int temp_rand;                      // for probability determinations {dlb}
    int fountain_effect = POT_WATER;    // for fountain effects {dlb}

    switch ( grd[you.x_pos][you.y_pos] )
    {
      case DNGN_BLUE_FOUNTAIN:
        if ( !yesno("Drink from the fountain?") )
          return false;
        mpr("You drink the pure, clear water.");
        break;
      case DNGN_SPARKLING_FOUNTAIN:
        if ( !yesno("Drink from the sparkling fountain?") )
          return false;
        mpr("You drink the sparkling water.");
        break;
    }

    if ( grd[you.x_pos][you.y_pos] == DNGN_SPARKLING_FOUNTAIN )
    {
        temp_rand = random2(4500);

        fountain_effect = ( (temp_rand > 2399) ? POT_WATER :                // 46.7% chance {dlb}
                            (temp_rand > 2183) ? POT_DECAY :                //  4.8% chance {dlb}
                            (temp_rand > 2003) ? POT_MUTATION :             //  4.0% chance {dlb}
                            (temp_rand > 1823) ? POT_HEALING :              //  4.0% chance {dlb}
                            (temp_rand > 1643) ? POT_HEAL_WOUNDS :          //  4.0% chance {dlb}
                            (temp_rand > 1463) ? POT_SPEED :                //  4.0% chance {dlb}
                            (temp_rand > 1283) ? POT_MIGHT :                //  4.0% chance {dlb}
                            (temp_rand > 1139) ? POT_DEGENERATION :         //  3.2% chance {dlb}
                            (temp_rand > 1019) ? POT_LEVITATION :           //  2.7% chance {dlb}
                            (temp_rand >  899) ? POT_POISON :               //  2.7% chance {dlb}
                            (temp_rand >  779) ? POT_SLOWING :              //  2.7% chance {dlb}
                            (temp_rand >  659) ? POT_PARALYSIS :            //  2.7% chance {dlb}
                            (temp_rand >  539) ? POT_CONFUSION :            //  2.7% chance {dlb}
                            (temp_rand >  419) ? POT_INVISIBILITY :         //  2.7% chance {dlb}
                            (temp_rand >  329) ? POT_MAGIC :                //  2.0% chance {dlb}
                            (temp_rand >  239) ? POT_RESTORE_ABILITIES :    //  2.0% chance {dlb}
                            (temp_rand >  149) ? POT_STRONG_POISON :        //  2.0% chance {dlb}
                            (temp_rand >   59) ? POT_BERSERK_RAGE :         //  2.0% chance {dlb}
                            (temp_rand >   39) ? POT_GAIN_STRENGTH :        //  0.4% chance {dlb}
                            (temp_rand >   19) ? POT_GAIN_DEXTERITY         //  0.4% chance {dlb}
                                               : POT_GAIN_INTELLIGENCE );   //  0.4% chance {dlb}
    }

    potion_effect(fountain_effect, 100);

    switch ( grd[you.x_pos][you.y_pos] )
    {
      case DNGN_BLUE_FOUNTAIN:
        if (one_chance_in(20))
          gone_dry = true;
        break;
      case DNGN_SPARKLING_FOUNTAIN:
        if (one_chance_in(10))
        {
            gone_dry = true;
            break;
        }
        else
        {
            temp_rand = random2(50);

            if ( temp_rand > 40 ) // 18% probability
              grd[you.x_pos][you.y_pos] = DNGN_BLUE_FOUNTAIN;         // you won't know it (yet)
        }
        break;
    }

    if (gone_dry)
    {
        mpr("The fountain dries up!");
        if ( grd[you.x_pos][you.y_pos] == DNGN_BLUE_FOUNTAIN )
          grd[you.x_pos][you.y_pos] = DNGN_DRY_FOUNTAIN_I;
        else if ( grd[you.x_pos][you.y_pos] == DNGN_SPARKLING_FOUNTAIN )
          grd[you.x_pos][you.y_pos] = DNGN_DRY_FOUNTAIN_II;
    }

    you.turn_is_over = 1;

    return true;

}          // end drink_fountain()




void read_scroll( void )
{

    int sc_read_1, sc_read_2;
    int id_used = 0;
    unsigned char nthing = 0;
    char affected = 0;
    int i;
    struct bolt beam[1];

    if ( you.berserker )
    {
        canned_msg(MSG_TOO_BERSERK);
        return;
    }

    if ( you.num_inv_items < 1 )
    {
        canned_msg(MSG_NOTHING_CARRIED);
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
        if ( ( nthing >= 'A' && nthing <= 'Z' ) || ( nthing >= 'a' && nthing <= 'z' ) )
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

    if ( sc_read_1 < 'A' || ( sc_read_1 > 'Z' && sc_read_1 < 'a' ) || sc_read_1 > 'z' )
    {
        mpr("You don't have any such object.");
        return;
    }

    sc_read_2 = letter_to_index(sc_read_1);

    if ( you.inv_quantity[sc_read_2] == 0 )
    {
        mpr("You don't have any such object.");
        return;
    }

    if ( !(you.inv_class[sc_read_2] == OBJ_BOOKS || you.inv_class[sc_read_2] == OBJ_SCROLLS) )
    {
        mpr("You can't read that!");
        return;
    }


// here we try to read a book {dlb}:
    if ( you.inv_class[sc_read_2] == OBJ_BOOKS )
    {
        if (you.inv_type[sc_read_2] == BOOK_DESTRUCTION)
        {
            tome_of_power(sc_read_2);
            return;
        }
        else if (you.inv_type[sc_read_2] == BOOK_MANUAL)
        {
            skill_manual(sc_read_2);
            return;
        }
        else
          read_book(sc_read_2);

    // where does book_thing come from? well, it's an externed variable
    // set by read_book -- confusing, I know {dlb}:
        if (book_thing < 'a' || book_thing > 'h') //jmf: was 'g', but 8=h
        {
            mesclr();
            return;
        }

        if (!learn_a_spell(sc_read_2, letter_to_index(book_thing)))
        {
            mesclr();
            return;
        }

        nthing = which_spell_in_book(you.inv_type[sc_read_2], 1 + letter_to_index(book_thing));

        if (nthing == SPELL_NO_SPELL)
        {
            mesclr();
            return;
        }

        describe_spell(which_spell_in_book(you.inv_type[sc_read_2], 1 + letter_to_index(book_thing)));
#ifdef PLAIN_TERM
        redraw_screen();
#endif

        mesclr();
        return;
    }


// ok - now we FINALLY get to read a scroll !!! {dlb}
    you.turn_is_over = 1;

// imperfect vision prevents players from reading actual content {dlb}:
    if ( you.mutation[MUT_BLURRY_VISION] && random2(5) < you.mutation[MUT_BLURRY_VISION] )
    {
        mpr ( (you.mutation[MUT_BLURRY_VISION] == 3 && one_chance_in(3)) ? "This scroll appears to be blank."
                                                                         : "The writing blurs in front of your eyes." );
        return;
    }

// decrement and handle inventory if any scroll other than paper {dlb}:
    if ( you.inv_type[sc_read_2] != SCR_PAPER )
    {
        you.inv_quantity[sc_read_2]--;

        if ( you.inv_quantity[sc_read_2] == 0 )
        {
            you.num_inv_items--;

            if ( you.equip[EQ_WEAPON] == sc_read_2 )
            {
                you.equip[EQ_WEAPON] = -1;
                mpr("You are now empty handed.");
            }
        }

        mpr("As you read the scroll, it crumbles to dust.");
        burden_change();
    }

// scrolls of paper are also exempted from this handling {dlb}:
    if ( you.inv_type[sc_read_2] != SCR_PAPER )
    {
        if ( you.conf )
        {
            random_uselessness(random2(9), sc_read_2);
            return;
        }

        if ( !you.skills[SK_SPELLCASTING] )
          exercise(SK_SPELLCASTING, ( coinflip() ? 2 : 1 ) );
    }


    bool id_the_scroll = true;    // to prevent unnecessary repetition
                                  // it is the exception, not the rule, that
                                  // the scroll will not be identified {dlb}:

    switch ( you.inv_type[sc_read_2] )
    {
    // remember paper scrolls handled as special case above, too:
        case SCR_PAPER:
          mpr("This scroll appears to be blank.");
          break;

        case SCR_RANDOM_USELESSNESS:
          random_uselessness(random2(9), sc_read_2);
          id_the_scroll = false;
          break;

        case SCR_BLINKING:
          blink();
          break;

        case SCR_TELEPORTATION:
          you_teleport();
          break;

        case SCR_REMOVE_CURSE:
          if ( !remove_curse(false) )
            id_the_scroll = false;
          break;

        case SCR_DETECT_CURSE:
          if ( !detect_curse(false) )
            id_the_scroll = false;
          break;

        case SCR_ACQUIREMENT:
          acquirement(OBJ_RANDOM);
          break;

        case SCR_FEAR:
          mpr("You feel scary!");
          mass_enchantment(4, 1000);
          break;

        case SCR_NOISE:
          mpr("You hear a loud clanging noise!");
          noisy(25, you.x_pos, you.y_pos);
          break;

        case SCR_SUMMONING:
          if ( create_monster(MONS_ABOMINATION_SMALL, 25, BEH_ENSLAVED,
                              you.x_pos, you.y_pos, MHITNOT, 250) != -1 )
            mpr("A horrible Thing appears!");
          break;

        case SCR_FORGETFULNESS:
          mpr("You feel momentarily disoriented.");
          if ( !wearing_amulet(AMU_CLARITY) )
            forget_map(50 + random2(50));
          break;

        case SCR_MAGIC_MAPPING:
          if (you.level_type == LEVEL_LABYRINTH
              || you.level_type == LEVEL_ABYSS)
          {
              mpr("You feel momentarily disoriented.");
              id_the_scroll = false;
          }
          else
          {
              mpr("You feel aware of your surroundings.");
              magic_mapping(21, 95 + random2(10));
          }
          break;

        case SCR_TORMENT:
          torment(you.x_pos, you.y_pos);

      // is only naughty if you know you're doing it
          if ( get_id(you.inv_class[sc_read_2], you.inv_type[sc_read_2]) == 1 )
            naughty(NAUGHTY_UNHOLY, 10);
          break;

        case SCR_IMMOLATION:
          mpr("The scroll explodes in your hands!");

          beam[0].type = SYM_BURST;
          beam[0].damage = 110;
          beam[0].flavour = BEAM_FIRE;    // unsure about this    // BEAM_EXPLOSION instead? {dlb}
          beam[0].bx = you.x_pos;
          beam[0].by = you.y_pos;
          strcpy(beam[0].beam_name, "fiery explosion");
          beam[0].colour = RED;
          beam[0].thing_thrown = KILL_YOU;       // your explosion, (not someone else's explosion)

          explosion(true, &beam[0]);
          break;

        case SCR_IDENTIFY:
          you.inv_ident[sc_read_2] = 2;    // I think this works {dlb}
          set_id(you.inv_class[sc_read_2], you.inv_type[sc_read_2], 1);    // important {dlb}
          identify(-1);
          wield_change = true;
          return;

        case SCR_CURSE_WEAPON:
          if ( you.equip[EQ_WEAPON] == -1
                || you.inv_class[you.equip[EQ_WEAPON]] != OBJ_WEAPONS
                || you.inv_plus[you.equip[EQ_WEAPON]] > 130 )
          {
              canned_msg(MSG_NOTHING_HAPPENS);
              id_the_scroll = false;
          }
          else
          {
              in_name(you.equip[EQ_WEAPON], 4, str_pass);
              strcpy(info, str_pass);
              strcat(info, " glows black for a moment.");
              mpr(info);

              you.inv_plus[you.equip[EQ_WEAPON]] += 100;

              wield_change = true;
          }
          break;







// everything [in the switch] below this line is a nightmare {dlb}:

        case SCR_ENCHANT_WEAPON_I:
        case SCR_ENCHANT_WEAPON_II:
        case SCR_ENCHANT_WEAPON_III:
weapon_enchant:
          if ( you.equip[EQ_WEAPON] == -1 )
          {
              canned_msg(MSG_NOTHING_HAPPENS);
              return;
          }

          if ( you.inv_class[you.equip[EQ_WEAPON]] == OBJ_WEAPONS || you.inv_class[you.equip[EQ_WEAPON]] == OBJ_MISSILES )
          {
              if ( you.inv_type[sc_read_2] == SCR_ENCHANT_WEAPON_II && you.inv_class[you.equip[EQ_WEAPON]] == OBJ_WEAPONS )
                id_used = you.inv_plus2[you.equip[EQ_WEAPON]];
              else
                id_used = you.inv_plus[you.equip[EQ_WEAPON]];

              affected = 1;

              if ( you.inv_class[you.equip[EQ_WEAPON]] == OBJ_WEAPONS
                  && ( you.inv_dam[you.equip[EQ_WEAPON]] >= NWPN_SINGING_SWORD
                        || you.inv_dam[you.equip[EQ_WEAPON]] % 30 >= SPWPN_RANDART_I ) )   // artefacts can't be enchanted, but these scrolls still remove curses
                affected = 0;

              wield_change = true;

              if ( id_used >= 154 || ( id_used < 100 && id_used >= 54 ) )
                if ( random2(9) < id_used % 50 )
                  affected = 0;

            // if it isn't affected by the enchantment, it will still be uncursed:
              if ( you.inv_plus[you.equip[EQ_WEAPON]] >= 154
                    || ( you.inv_dam[you.equip[EQ_WEAPON]] % 30 >= 25
                        && you.inv_plus[you.equip[EQ_WEAPON]] >= 100 ) )
              {
                  if ( affected == 0 )
                  {
                      in_name(you.equip[EQ_WEAPON], 4, str_pass);
                      strcpy(info, str_pass);
                      strcat(info, " glows silver for a moment.");
                      mpr(info);

                      you.inv_plus[you.equip[EQ_WEAPON]] -= 100;

                      set_id(you.inv_class[sc_read_2], you.inv_type[sc_read_2], 1);
                      return;
                  }
              }

              if ( affected == 0 )
              {
                  canned_msg(MSG_NOTHING_HAPPENS);
                  return;
              }

              // vVvVv    This is *here* (as opposed to lower down) for a reason!
              in_name(you.equip[EQ_WEAPON], 4, str_pass);
              strcpy(info, str_pass);

              if ( you.inv_class[you.equip[EQ_WEAPON]] == OBJ_WEAPONS )
              {

                  if ( you.inv_type[sc_read_2] == SCR_ENCHANT_WEAPON_II )
                  {
                      strcat(info, " glows red for a moment.");
                      mpr(info);
                  }
                  else if ( you.inv_type[sc_read_2] == SCR_ENCHANT_WEAPON_I )
                  {
                      strcat(info, " glows green for a moment.");
                      mpr(info);
                  }
                  else
                  {
                      id_used--;      /* no plusses for using this */

                      if ( you.duration[DUR_WEAPON_BRAND] )
                      {
                          you.duration[DUR_WEAPON_BRAND] = 0;

                          switch ( you.inv_dam[you.equip[EQ_WEAPON]] % 30 )
                          {
                              case SPWPN_VORPAL:
                                if ( damage_type(you.inv_class[you.equip[EQ_WEAPON]], you.inv_type[you.equip[EQ_WEAPON]]) != DVORP_CRUSHING )
                                  strcat(info, "'s sharpness seems more permanent.");
                                else
                                  strcat(info, "'s heaviness feels very stable.");
                                mpr(info);
                                break;

                              case SPWPN_FLAMING:
                                strcat(info, " is engulfed in an explosion of flames!");
                                mpr(info);

                                beam[0].type = SYM_BURST;
                                beam[0].damage = 110;
                                beam[0].flavour = 2;
                                beam[0].bx = you.x_pos;
                                beam[0].by = you.y_pos;
                                strcpy(beam[0].beam_name, "fiery explosion");
                                beam[0].colour = RED;
                                beam[0].thing_thrown = 1;
                                explosion(true, &beam[0]);
                                break;

                              case SPWPN_FREEZING:
                                strcat(info, " glows brilliantly blue for a moment.");
                                mpr(info);
                                cast_refrigeration(60);
                                break;

                              case SPWPN_DRAINING:
                                strcat(info, " thirsts for the lives of mortals!");
                                mpr(info);
                                drain_exp();
                                break;

                              case SPWPN_VENOM:
                                strcat(info, " seems more permanently poisoned.");
                                mpr(info);
                                cast_toxic_radiance();
                                break;

                              case SPWPN_DISTORTION:
                                strcat(info, " twongs alarmingly.");
                                mpr(info);
                                miscast_effect(SPTYP_TRANSLOCATION, 9, 90, 100); // from unwield_item
                                break;
                          }
                      }
                      else
                      {
                          strcat(info, " glows bright yellow for a while.");
                          mpr(info);

                          you.inv_plus[you.equip[EQ_WEAPON]] += ((coinflip())? 2 : 1);
                          you.inv_plus2[you.equip[EQ_WEAPON]] += ((coinflip())? 2 : 1);

                          if ( you.inv_plus[you.equip[EQ_WEAPON]] > 130 )
                            you.inv_plus[you.equip[EQ_WEAPON]] -= 100;

                          set_id(you.inv_class[sc_read_2], you.inv_type[sc_read_2], 1);
                          return;
                      }
                  }

              }    // end "if you.inv_class[you.equip[EQ_WEAPON]] == OBJ_WEAPONS"

              set_id(you.inv_class[sc_read_2], you.inv_type[sc_read_2], 1);

              id_used++;

              if ( you.inv_plus[you.equip[EQ_WEAPON]] > 130 )
                you.inv_plus[you.equip[EQ_WEAPON]] -= 100;

              if ( id_used > 130 )
                id_used -= 100;

              if ( you.inv_class[you.equip[EQ_WEAPON]] == OBJ_WEAPONS )
              {
                  if ( you.inv_type[sc_read_2] == SCR_ENCHANT_WEAPON_II )
                    you.inv_plus2[you.equip[EQ_WEAPON]] = id_used;
                  else if ( you.inv_type[sc_read_2] == SCR_ENCHANT_WEAPON_I )
                    you.inv_plus[you.equip[EQ_WEAPON]] = id_used;
              }
              return;
          }

          canned_msg(MSG_NOTHING_HAPPENS);
          return;                 // end of case 7: enc weapon

        case SCR_VORPALISE_WEAPON:
          if ( you.equip[EQ_WEAPON] == -1
                || you.inv_class[you.equip[EQ_WEAPON]] != OBJ_WEAPONS
                || ( you.inv_class[you.equip[EQ_WEAPON]] == OBJ_WEAPONS
                    && ( you.inv_dam[you.equip[EQ_WEAPON]] >= NWPN_SINGING_SWORD
                          || you.inv_dam[you.equip[EQ_WEAPON]] % 30 >= SPWPN_RANDART_I
                          || launches_things(you.inv_type[you.equip[EQ_WEAPON]]) ) ) )
          {
              canned_msg(MSG_NOTHING_HAPPENS);
              return;
          }

          in_name(you.equip[EQ_WEAPON], 4, str_pass);

          strcpy(info, str_pass);
          strcat(info, " emits a brilliant flash of light!");
          mpr(info);
          alert();
          set_id(you.inv_class[sc_read_2], you.inv_type[sc_read_2], 1);
          wield_change = true;

          if ( you.inv_dam[you.equip[EQ_WEAPON]] >= NWPN_SINGING_SWORD
                || you.inv_dam[you.equip[EQ_WEAPON]] % 30 != SPWPN_NORMAL )
          {
              mpr("You feel strangely frustrated.");
              return;
          }

          you.inv_dam[you.equip[EQ_WEAPON]] += 10;
          return;

        case SCR_RECHARGING:
          if ( you.equip[EQ_WEAPON] != -1
              && you.inv_class[you.equip[EQ_WEAPON]] == OBJ_WEAPONS
              && you.inv_dam[you.equip[EQ_WEAPON]] == SPWPN_ELECTROCUTION )
            goto weapon_enchant;

          if ( !recharge_wand() )
            canned_msg(MSG_NOTHING_HAPPENS);
          else
          {
              set_id(you.inv_class[sc_read_2], you.inv_type[sc_read_2], 1);
              wield_change = true;
          }
          return;

        case SCR_ENCHANT_ARMOUR:    // ench you.equip
          for (i = EQ_CLOAK; i < EQ_RIGHT_RING; i++)
          {
              if (i == EQ_LEFT_RING)
              {
                  canned_msg(MSG_NOTHING_HAPPENS);
                  return;
              }

              if (you.equip[i] != -1)
                break;
          }

          do
          {
              affected = 1 + random2(6);
          }
          while ( you.equip[affected] == -1 );

        // NOTE: It is assumed that armour which changes in this way does not change
        //  into a form of armour with a different evasion modifier.

          if ( you.inv_type[you.equip[EQ_BODY_ARMOUR]] == ARM_DRAGON_HIDE
                || you.inv_type[you.equip[EQ_BODY_ARMOUR]] == ARM_ICE_DRAGON_HIDE
                || you.inv_type[you.equip[EQ_BODY_ARMOUR]] == ARM_STEAM_DRAGON_HIDE
                || you.inv_type[you.equip[EQ_BODY_ARMOUR]] == ARM_MOTTLED_DRAGON_HIDE
                || you.inv_type[you.equip[EQ_BODY_ARMOUR]] == ARM_STORM_DRAGON_HIDE
                || you.inv_type[you.equip[EQ_BODY_ARMOUR]] == ARM_GOLD_DRAGON_HIDE
                || you.inv_type[you.equip[EQ_BODY_ARMOUR]] == ARM_SWAMP_DRAGON_HIDE
                || you.inv_type[you.equip[EQ_BODY_ARMOUR]] == ARM_TROLL_HIDE )
          {
              affected = EQ_BODY_ARMOUR;
              in_name(you.equip[affected], 4, str_pass);
              strcpy(info, str_pass);
              strcat(info, " glows purple and changes!");
              mpr(info);

              you.redraw_armor_class = 1;

              hide2armour(&(you.inv_type[you.equip[EQ_BODY_ARMOUR]]));
              set_id(you.inv_class[sc_read_2], you.inv_type[sc_read_2], 1);
              return;
          }

          if (you.inv_dam[you.equip[affected]] % 30 >= 25)
          {
              in_name(you.equip[affected], 4, str_pass);
              set_id(you.inv_class[sc_read_2], you.inv_type[sc_read_2], 1);
              strcpy(info, str_pass);
              strcat(info, " glows faintly for a moment.");
              mpr(info);

              if ( you.inv_plus[you.equip[affected]] > 100 )
                you.inv_plus[you.equip[affected]] -= 100;
              return;
          }

          if (you.inv_plus[you.equip[affected]] >= 153)
          {
              if (random2(8) >= (you.inv_plus[you.equip[affected]] - 150))
              {
                  in_name(you.equip[affected], 4, str_pass);
                  you.inv_plus[you.equip[affected]] -= 100;
                  set_id(you.inv_class[sc_read_2], you.inv_type[sc_read_2], 1);

                  strcpy(info, str_pass);
                  strcat(info, " glows silver for a moment.");
                  mpr(info);
                  return;
              }
          }

          if ((you.inv_plus[you.equip[affected]] - 50 >= 3 && you.inv_plus[you.equip[affected]] < 130) || you.inv_plus[you.equip[affected]] >= 153)
          {
              if ( random2(8) < (you.inv_plus[you.equip[affected]] - 50) )
              {
                  canned_msg(MSG_NOTHING_HAPPENS);
                  return;
              }
          }

          if ( you.inv_type[you.equip[affected]] >= ARM_CLOAK
              && you.inv_type[you.equip[affected]] <= ARM_BOOTS )
            if ( ( you.inv_plus[you.equip[affected]] - 50 >= 2 && you.inv_plus[you.equip[affected]] < 130 )
                   || you.inv_plus[you.equip[affected]] >= 152 )
            {
                canned_msg(MSG_NOTHING_HAPPENS);
                return;
            }

        // vVvVv    This is *here* for a reason!
          in_name(you.equip[affected], 4, str_pass);
          you.inv_plus[you.equip[affected]]++;

          if ( you.inv_plus[you.equip[affected]] > 130 )
            you.inv_plus[you.equip[affected]] -= 100;

          set_id(you.inv_class[sc_read_2], you.inv_type[sc_read_2], 1);

          strcpy(info, str_pass);
          strcat(info, " glows green for a moment.");
          mpr(info);
          you.redraw_armor_class = 1;
          return;                 // end of ench you.equip


        case SCR_CURSE_ARMOUR:      // curse you.equip
          for (i = EQ_CLOAK; i < EQ_RIGHT_RING; i++)
          {
              if (i == EQ_LEFT_RING)
              {
                  canned_msg(MSG_NOTHING_HAPPENS);
                  return;
              }

              if ( you.equip[i] != -1 && you.inv_plus[you.equip[i]] < 130 )
                break;
          }

          do
          {
              affected = 1 + random2(6);
          }
          while ( you.equip[affected] == -1 || you.inv_plus[you.equip[affected]] > 130 );

        // vVvVv    This is *here* for a reason!
          in_name(you.equip[affected], 4, str_pass);
          you.inv_plus[you.equip[affected]] += 100;
          set_id(you.inv_class[sc_read_2], you.inv_type[sc_read_2], 1);

          strcpy(info, str_pass);
          strcat(info, " glows black for a moment.");
          mpr(info);
          return;

    }        // end switch


// finally, identify the scroll {dlb}:
    if ( id_the_scroll )
      set_id(you.inv_class[sc_read_2], you.inv_type[sc_read_2], 1);

    return;

}          // end read_scroll()




void original_name( void )
{

    char drink_1, drink_2;
    char inn = 0;
    unsigned char nthing = 0;

query:
    mpr("Examine which item?");

    unsigned char keyin = get_ch();

    if (keyin == '?' || keyin == '*')
    {
        if (keyin == '?')
            nthing = get_invent(-1);
        if (keyin == '*')
            nthing = get_invent(-1);
        if ( ( nthing >= 'A' && nthing <= 'Z' ) || ( nthing >= 'a' && nthing <= 'z' ) )
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

    if ( drink_1 < 'A' || ( drink_1 > 'Z' && drink_1 < 'a' ) || drink_1 > 'z' )
    {
        mpr("You don't have any such object.");
        return;
    }

    drink_2 = letter_to_index(drink_1);

    if (you.inv_quantity[drink_2] == 0)
    {
        mpr("You don't have any such object.");
        return;
    }

    inn = drink_2;
    describe_item(you.inv_class[inn], you.inv_type[inn], you.inv_plus[inn], you.inv_plus2[inn], you.inv_dam[inn], you.inv_ident[inn]);
#ifdef PLAIN_TERM
    redraw_screen();
#endif

}                               // end original_name()




void use_randart( unsigned char item_wield_2 )
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
        you.strength += randart_wpn_properties(you.inv_class[item_wield_2], you.inv_type[item_wield_2], you.inv_dam[item_wield_2], you.inv_plus[item_wield_2], you.inv_plus2[item_wield_2], 0, RAP_STRENGTH);
        you.max_strength += randart_wpn_properties(you.inv_class[item_wield_2], you.inv_type[item_wield_2], you.inv_dam[item_wield_2], you.inv_plus[item_wield_2], you.inv_plus2[item_wield_2], 0, RAP_STRENGTH);
        you.redraw_strength = 1;
    }

    if (randart_wpn_properties(you.inv_class[item_wield_2], you.inv_type[item_wield_2], you.inv_dam[item_wield_2], you.inv_plus[item_wield_2], you.inv_plus2[item_wield_2], 0, RAP_INTELLIGENCE) != 0)
    {
        you.intel += randart_wpn_properties(you.inv_class[item_wield_2], you.inv_type[item_wield_2], you.inv_dam[item_wield_2], you.inv_plus[item_wield_2], you.inv_plus2[item_wield_2], 0, RAP_INTELLIGENCE);
        you.max_intel += randart_wpn_properties(you.inv_class[item_wield_2], you.inv_type[item_wield_2], you.inv_dam[item_wield_2], you.inv_plus[item_wield_2], you.inv_plus2[item_wield_2], 0, RAP_INTELLIGENCE);
        you.redraw_intelligence = 1;
    }

    if (randart_wpn_properties(you.inv_class[item_wield_2], you.inv_type[item_wield_2], you.inv_dam[item_wield_2], you.inv_plus[item_wield_2], you.inv_plus2[item_wield_2], 0, RAP_DEXTERITY) != 0)
    {
        you.dex += randart_wpn_properties(you.inv_class[item_wield_2], you.inv_type[item_wield_2], you.inv_dam[item_wield_2], you.inv_plus[item_wield_2], you.inv_plus2[item_wield_2], 0, RAP_DEXTERITY);
        you.max_dex += randart_wpn_properties(you.inv_class[item_wield_2], you.inv_type[item_wield_2], you.inv_dam[item_wield_2], you.inv_plus[item_wield_2], you.inv_plus2[item_wield_2], 0, RAP_DEXTERITY);
        you.redraw_dexterity = 1;
    }

// this is handled by player_res_electricity() now
    //    if (randart_wpn_properties(you.inv_class[item_wield_2], you.inv_type[item_wield_2], you.inv_dam[item_wield_2], you.inv_plus[item_wield_2], you.inv_plus2[item_wield_2], 0, RAP_ELECTRICITY) != 0)
    //    {
    //      you.attribute[ATTR_RESIST_LIGHTNING]++;
    //    }

    if (randart_wpn_properties(you.inv_class[item_wield_2], you.inv_type[item_wield_2], you.inv_dam[item_wield_2], you.inv_plus[item_wield_2], you.inv_plus2[item_wield_2], 0, RAP_NOISES) != 0)
    {
        you.special_wield = 50 + randart_wpn_properties(you.inv_class[item_wield_2], you.inv_type[item_wield_2], you.inv_dam[item_wield_2], you.inv_plus[item_wield_2], you.inv_plus2[item_wield_2], 0, RAP_NOISES);
    }

}

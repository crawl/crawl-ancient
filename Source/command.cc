/*
 *  File:       command.cc
 *  Summary:    Misc commands.
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 *      <4>     10/12/99        BCR             BUILD_DATE is now used in version()
 *      <3>     6/13/99         BWR             New equipment listing commands
 *      <2>     5/20/99         BWR             Swapping inventory letters.
 *      <1>     -/--/--         LRH             Created
 */

#include "AppHdr.h"
#include "command.h"

#include <stdio.h>
#include <string.h>

#include "externs.h"

#include "invent.h"
#include "itemname.h"
#include "ouch.h"
#include "spells0.h"
#include "spl-util.h"
#include "stuff.h"
#include "version.h"

extern bool wield_change;       // defined in output.cc

void adjust_item(void);
void adjust_spells(void);
void get_letter_id(char buff[5], int item_id);

void quit_game(void)
{
    if (yesno("Really quit?", false))
        ouch(-9999, 0, KILLED_BY_QUITTING);
}                               // end quit_game()

void version(void)
{
    mpr("This is Dungeon Crawl " VERSION " (Last build " BUILD_DATE ").");
}                               // end version()

void adjust(void)
{
    mpr("Adjust (i)tems or (s)pells?", MSGCH_PROMPT);

    unsigned char keyin = get_ch();

    if (keyin == 'i' || keyin == 'I')
    {
        adjust_item();
        return;
    }
    else if (keyin == 's' || keyin == 'S')
    {
        adjust_spells();
        return;
    }
    else
        mpr("Huh?");

    return;
}                               // end adjust()

void adjust_item(void)
{
    unsigned char throw_2, throw_3;
    unsigned nthing = 0;

    if (you.num_inv_items < 1)
    {
        canned_msg(MSG_NOTHING_CARRIED);
        return;
    }

  query:
    mpr("Adjust which item?", MSGCH_PROMPT);

    unsigned char keyin = get_ch();

    if (keyin == '?' || keyin == '*')
    {
        if (keyin == '*' || keyin == '?')
            nthing = get_invent(-1);

        if ((nthing >= 'A' && nthing <= 'Z')
                || (nthing >= 'a' && nthing <= 'z'))
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

    if (throw_1 < 'A' || (throw_1 > 'Z' && throw_1 < 'a') || throw_1 > 'z')
    {
        mpr("You don't have any such object.");
        return;
    }

    throw_2 = letter_to_index(throw_1);

    if (you.inv_quantity[throw_2] == 0)
    {
        mpr("You don't have any such object.");
        return;
    }

    strcpy(info, " ");

    info[0] = index_to_letter(throw_2);
    info[1] = '\0';

    strcat(info, " - ");

    in_name(throw_2, 3, str_pass);
    strcat(info, str_pass);
    mpr(info);

    mpr("Adjust to which letter?", MSGCH_PROMPT);

    keyin = get_ch();

    if (keyin == '?' || keyin == '*')
    {
        if (keyin == '*' || keyin == '?')
            nthing = get_invent(-1);

        if ((nthing >= 'A' && nthing <= 'Z')
                || (nthing >= 'a' && nthing <= 'z'))
        {
            keyin = nthing;
        }
        else
        {
            mesclr();
            goto query;
        }
    }

    throw_1 = (int) keyin;

    if (throw_1 < 'A' || (throw_1 > 'Z' && throw_1 < 'a') || throw_1 > 'z')
    {
        mpr("What?");
        return;
    }

    throw_3 = letter_to_index(throw_1);

    // if (you.inv_quantity [throw_3] != 0)
    // {
    //      mpr("Sorry, that inventory letter is already in use.");
    //      return;
    // }
    unsigned char tmp_ident = you.inv_ident[throw_3];
    unsigned char tmp_class = you.inv_class[throw_3];
    unsigned char tmp_type = you.inv_type[throw_3];
    unsigned char tmp_plus = you.inv_plus[throw_3];
    unsigned char tmp_plus2 = you.inv_plus2[throw_3];
    unsigned char tmp_dam = you.inv_dam[throw_3];
    char tmp_colour = you.inv_colour[throw_3];
    int tmp_quantity = you.inv_quantity[throw_3];

    you.inv_ident[throw_3] = you.inv_ident[throw_2];
    you.inv_class[throw_3] = you.inv_class[throw_2];
    you.inv_type[throw_3] = you.inv_type[throw_2];
    you.inv_plus[throw_3] = you.inv_plus[throw_2];
    you.inv_plus2[throw_3] = you.inv_plus2[throw_2];
    you.inv_dam[throw_3] = you.inv_dam[throw_2];
    you.inv_colour[throw_3] = you.inv_colour[throw_2];
    you.inv_quantity[throw_3] = you.inv_quantity[throw_2];

    you.inv_ident[throw_2] = tmp_ident;
    you.inv_class[throw_2] = tmp_class;
    you.inv_type[throw_2] = tmp_type;
    you.inv_plus[throw_2] = tmp_plus;
    you.inv_plus2[throw_2] = tmp_plus2;
    you.inv_dam[throw_2] = tmp_dam;
    you.inv_colour[throw_2] = tmp_colour;
    you.inv_quantity[throw_2] = tmp_quantity;

    for (int i = 0; i < 10; i++)
    {
        if (you.equip[i] == throw_2)
            you.equip[i] = throw_3;
        else if (you.equip[i] == throw_3)
            you.equip[i] = throw_2;
    }

    strcpy(info, " ");

    info[0] = index_to_letter(throw_3);
    info[1] = '\0';

    strcat(info, " - ");

    in_name(throw_3, 3, str_pass);
    strcat(info, str_pass);
    mpr(info);

    if (you.inv_quantity[throw_2] > 0)
    {
        strcpy(info, " ");

        info[0] = index_to_letter(throw_2);
        info[1] = '\0';

        strcat(info, " - ");

        in_name(throw_2, 3, str_pass);
        strcat(info, str_pass);
        mpr(info);
    }

    if (throw_3 == you.equip[EQ_WEAPON] || throw_2 == you.equip[EQ_WEAPON])
        wield_change = true;

}                               // end adjust_item()

static void adjust_spells_cleanup(bool needs_redraw)
{
    if (needs_redraw)
        redraw_screen();
}

void adjust_spells(void)
{
    unsigned char throw_2, throw_3;
    unsigned char nthing = 0;

    bool needs_redraw = false;

    if (!you.spell_no)
    {
        mpr("You don't know any spells.");
        return;
    }

  query:
    mpr("Adjust which spell?", MSGCH_PROMPT);

    unsigned char keyin = get_ch();

    if (keyin == '?' || keyin == '*')
    {
        if (keyin == '*' || keyin == '?')
        {
            nthing = spell_list();
            needs_redraw = true;
        }

        if ((nthing >= 'A' && nthing <= 'Z')
            || (nthing >= 'a' && nthing <= 'z'))
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

    if (throw_1 < 'a' || throw_1 > 'w')
    {
        adjust_spells_cleanup(needs_redraw);
        mpr("You don't know that spell.");
        return;
    }

    throw_2 = letter_to_index(throw_1);

    if (you.spells[throw_2] == SPELL_NO_SPELL)
    {
        adjust_spells_cleanup(needs_redraw);
        strcpy(info, "You don't know that spell.");
        mpr(info);
        return;
    }

    strcpy(info, " ");
    info[0] = index_to_letter(throw_2);
    info[1] = '\0';
    strcat(info, " - ");
    strcat(info, spell_title(you.spells[throw_2]));
    mpr(info);

    mpr("Adjust to which letter?", MSGCH_PROMPT);

    keyin = get_ch();

    if (keyin == '?' || keyin == '*')
    {
        if (keyin == '*' || keyin == '?')
        {
            nthing = spell_list();
            needs_redraw = true;
        }

        if ((nthing >= 'A' && nthing <= 'Z')
            || (nthing >= 'a' && nthing <= 'z'))
        {
            keyin = nthing;
        }
        else
        {
            mesclr();
            goto query;
        }
    }

    throw_1 = (int) keyin;

    //if (throw_1 < 'a' || throw_1 > 'z' )
    if (throw_1 < 'a' || throw_1 > 'v')
    {
        adjust_spells_cleanup(needs_redraw);
        mpr("What?");
        return;
    }

    adjust_spells_cleanup(needs_redraw);

    throw_3 = letter_to_index(throw_1);

    int backup = you.spells[throw_3];

    you.spells[throw_3] = you.spells[throw_2];
    you.spells[throw_2] = backup;

    strcpy(info, " ");
    info[0] = index_to_letter(throw_3);
    info[1] = '\0';
    strcat(info, " - ");
    strcat(info, spell_title(you.spells[throw_3]));
    mpr(info);

    if (you.spells[throw_2] != SPELL_NO_SPELL)
    {
        strcpy(info, " ");
        info[0] = index_to_letter(throw_2);
        info[1] = '\0';
        strcat(info, " - ");
        strcat(info, spell_title(you.spells[throw_2]));
        mpr(info);
    }

}                               // end adjust_spells()

void get_letter_id(char buff[5], int item_id)
{
    buff[0] = index_to_letter(item_id);
    buff[1] = '\0';

    strcat(buff, " - ");
}                               // end get_letter_id()

void list_armour(void)
{
    for (int i = EQ_CLOAK; i <= EQ_BODY_ARMOUR; i++)
    {
        int armour_id = you.equip[i];
        char letter_buff[5];

        strcpy( info,
                (i == EQ_CLOAK)       ? "Cloak  " :
                (i == EQ_HELMET)      ? "Helmet " :
                (i == EQ_GLOVES)      ? "Gloves " :
                (i == EQ_SHIELD)      ? "Shield " :
                (i == EQ_BODY_ARMOUR) ? "Armour " :

                (i == EQ_BOOTS
                    && !(you.species == SP_CENTAUR || you.species == SP_NAGA))
                                      ? "Boots  " :
                (i == EQ_BOOTS
                    && (you.species == SP_CENTAUR || you.species == SP_NAGA))
                                      ? "Barding"
                                      : "unknown" );

        strcat(info, " : ");

        if (armour_id != -1)
        {
            get_letter_id(letter_buff, armour_id);
            strcat(info, letter_buff);

            in_name(armour_id, 3, str_pass);
            strcat(info, str_pass);
        }
        else
        {
            strcat(info, "    none");
        }

        mpr(info);
    }
}                               // end list_armour()

void list_jewellery(void)
{
    for (int i = EQ_LEFT_RING; i <= EQ_AMULET; i++)
    {
        int jewellery_id = you.equip[i];
        char letter_buff[5];

        strcpy( info, (i == EQ_LEFT_RING)  ? "Left ring " :
                      (i == EQ_RIGHT_RING) ? "Right ring" :
                      (i == EQ_AMULET)     ? "Amulet    "
                                           : "unknown   " );

        strcat(info, " : ");

        if (jewellery_id != -1)
        {
            get_letter_id(letter_buff, jewellery_id);
            strcat(info, letter_buff);

            in_name(jewellery_id, 3, str_pass);
            strcat(info, str_pass);
        }
        else
        {
            strcat(info, "    none");
        }

        mpr(info);
    }
}                               // end list_jewellery()

void list_weapons(void)
{
    char letter_buff[5];
    const int weapon_id = you.equip[EQ_WEAPON];

    // Output the current weapon
    //
    // Yes, this is already on the screen... I'm outputing it
    // for completeness and to avoid confusion.
    strcpy(info, "Current   : ");

    if (weapon_id != -1)
    {
        get_letter_id(letter_buff, weapon_id);
        strcat(info, letter_buff);

        in_name(weapon_id, 3, str_pass);
        strcat(info, str_pass);
        strcat(info, " (weapon)");
    }
    else
    {
        if (you.attribute[ATTR_TRANSFORMATION] == TRAN_BLADE_HANDS)
            strcat(info, "    blade hands");
        else
            strcat(info, "    empty hands");
    }

    mpr(info);

    // Print out the swap slots
    for (int i = 0; i <= 1; i++)
    {
        // We'll avoid repeating the current weapon for these slots,
        // in order to keep things clean.
        if (weapon_id == i)
            continue;

        strcpy(info, (i == 0) ? "Primary   : " : "Secondary : ");

        if (you.inv_quantity[i] > 0)
        {
            get_letter_id(letter_buff, i);
            strcat(info, letter_buff);

            in_name(i, 3, str_pass);
            strcat(info, str_pass);

            // This is useful information
            for (int j = EQ_CLOAK; j <= EQ_AMULET; j++)
            {
                if (you.equip[j] == i)
                    strcat(info, " (worn)");
            }
        }
        else
            strcat(info, "    none");

        mpr(info);              // Output slot
    }

    // Now we print out the current default throwing weapon
    int type_wanted;
    int throw_id = -1;

    strcpy(info, "Firing    : ");

    if (weapon_id == -1 || you.inv_class[weapon_id] != OBJ_WEAPONS
                        || you.inv_type[weapon_id] < WPN_SLING
                        || you.inv_type[weapon_id] > WPN_CROSSBOW)
    {
        type_wanted = MI_DART;
    }
    else
    {
        type_wanted = you.inv_type[weapon_id] - 13;
    }

    for (int i = 0; i < ENDOFPACK; i++)
    {
        if (you.inv_quantity[i]
                && (you.inv_class[i] == OBJ_MISSILES
                        && you.inv_type[i] == type_wanted))
        {
            throw_id = i;
            break;
        }
    }

    // We'll print this one even if it is the current weapon
    if (throw_id != -1)
    {
        get_letter_id(letter_buff, throw_id);
        strcat(info, letter_buff);

        in_name(throw_id, 3, str_pass);
        strcat(info, str_pass);

        if (weapon_id == throw_id)
            strcat(info, " (weapon)");
    }
    else
    {
        strcat(info, "    nothing");
    }

    mpr(info);
}                               // end list_weapons()

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
#include <ctype.h>

#include "externs.h"

#include "invent.h"
#include "itemname.h"
#include "item_use.h"
#include "items.h"
#include "ouch.h"
#include "spl-cast.h"
#include "spl-util.h"
#include "stuff.h"
#include "version.h"
#include "wpn-misc.h"

void adjust_item(void);
void adjust_spells(void);

void quit_game(void)
{
    if (yesno("Really quit?", false))
        ouch(-9999, 0, KILLED_BY_QUITTING);
}                               // end quit_game()

void version(void)
{
    mpr( "This is Dungeon Crawl " VERSION " (Last build " BUILD_DATE ")." );
}                               // end version()

void adjust(void)
{
    mpr("Adjust (i)tems or (s)pells?", MSGCH_PROMPT);

    unsigned char keyin = tolower( get_ch() );

    if (keyin == 'i')
        adjust_item();
    else if (keyin == 's')
        adjust_spells();
    else if (keyin == ESCAPE)
        canned_msg( MSG_OK );
    else
        canned_msg( MSG_HUH );
}                               // end adjust()

void adjust_item(void)
{
    int from_slot, to_slot;
    char str_pass[ ITEMNAME_SIZE ];

    if (inv_count() < 1)
    {
        canned_msg(MSG_NOTHING_CARRIED);
        return;
    }

    from_slot = prompt_invent_item( "Adjust which item?", -1 );
    if (from_slot == PROMPT_ABORT)
    {
        canned_msg( MSG_OK );
        return;
    }

    in_name( from_slot, DESC_INVENTORY_EQUIP, str_pass );
    mpr( str_pass );

    to_slot = prompt_invent_item( "Adjust to which letter?", -1, false, false );
    if (to_slot ==  PROMPT_ABORT)
    {
        canned_msg( MSG_OK );
        return;
    }

    // swap items
    item_def tmp = you.inv[to_slot];
    you.inv[to_slot] = you.inv[from_slot];
    you.inv[from_slot] = tmp;

    you.inv[from_slot].link = from_slot;
    you.inv[to_slot].link = to_slot;

    for (int i = 0; i < NUM_EQUIP; i++)
    {
        if (you.equip[i] == from_slot)
            you.equip[i] = to_slot;
        else if (you.equip[i] == to_slot)
            you.equip[i] = from_slot;
    }

    in_name( to_slot, DESC_INVENTORY_EQUIP, str_pass );
    mpr( str_pass );

    if (is_valid_item( you.inv[from_slot] ))
    {
        in_name( from_slot, DESC_INVENTORY_EQUIP, str_pass );
        mpr( str_pass );
    }

    if (to_slot == you.equip[EQ_WEAPON] || from_slot == you.equip[EQ_WEAPON])
        you.wield_change = true;
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
            nthing = list_spells();
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

    if (keyin == ESCAPE)
    {
        canned_msg( MSG_OK );
        return;
    }

    int throw_1 = (int) keyin;

    if (throw_1 < 'a' || throw_1 > 'v')
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
            nthing = list_spells();
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

    if (keyin == ESCAPE)
    {
        canned_msg( MSG_OK );
        return;
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

void list_armour(void)
{
    for (int i = EQ_CLOAK; i <= EQ_BODY_ARMOUR; i++)
    {
        int armour_id = you.equip[i];

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
            char str_pass[ ITEMNAME_SIZE ];
            in_name(armour_id, DESC_INVENTORY, str_pass);
            strcat(info, str_pass);
        }
        else
        {
            strcat(info, "    none");
        }

        mpr( info, MSGCH_EQUIPMENT );
    }
}                               // end list_armour()

void list_jewellery(void)
{
    for (int i = EQ_LEFT_RING; i <= EQ_AMULET; i++)
    {
        int jewellery_id = you.equip[i];

        strcpy( info, (i == EQ_LEFT_RING)  ? "Left ring " :
                      (i == EQ_RIGHT_RING) ? "Right ring" :
                      (i == EQ_AMULET)     ? "Amulet    "
                                           : "unknown   " );

        strcat(info, " : ");

        if (jewellery_id != -1)
        {
            char str_pass[ ITEMNAME_SIZE ];
            in_name(jewellery_id, DESC_INVENTORY, str_pass);
            strcat(info, str_pass);
        }
        else
        {
            strcat(info, "    none");
        }

        mpr( info, MSGCH_EQUIPMENT );
    }
}                               // end list_jewellery()

void list_weapons(void)
{
    const int weapon_id = you.equip[EQ_WEAPON];

    // Output the current weapon
    //
    // Yes, this is already on the screen... I'm outputing it
    // for completeness and to avoid confusion.
    strcpy(info, "Current   : ");

    if (weapon_id != -1)
    {
        char str_pass[ ITEMNAME_SIZE ];
        in_name( weapon_id, DESC_INVENTORY_EQUIP, str_pass );
        strcat(info, str_pass);
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

        if (is_valid_item( you.inv[i] ))
        {
            char str_pass[ ITEMNAME_SIZE ];
            in_name(i, DESC_INVENTORY_EQUIP, str_pass);
            strcat(info, str_pass);
        }
        else
            strcat(info, "    none");

        mpr(info);              // Output slot
    }

    // Now we print out the current default fire weapon
    strcpy(info, "Firing    : ");

    const int item = get_fire_item_index();

    if (item == ENDOFPACK)
        strcat(info, "    nothing");
    else
    {
        char str_pass[ ITEMNAME_SIZE ];
        in_name( item, DESC_INVENTORY_EQUIP, str_pass );
        strcat( info, str_pass );
    }

    mpr( info, MSGCH_EQUIPMENT );
}                               // end list_weapons()

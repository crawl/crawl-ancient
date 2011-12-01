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

#include "globals.h"
#include "externs.h"

#include "abl-show.h"
#include "food.h"
#include "invent.h"
#include "itemname.h"
#include "itemprop.h"
#include "item_use.h"
#include "items.h"
#include "ouch.h"
#include "player.h"
#include "spl-cast.h"
#include "spl-util.h"
#include "stuff.h"
#include "version.h"

static void adjust_item(void);
static void adjust_spells(void);
static void adjust_ability(void);

void quit_game(void)
{
    if (yesno("Really quit?", false))
        ouch( INSTANT_DEATH, 0, KILLED_BY_QUITTING );
}                               // end quit_game()

void version(void)
{
    mpr( "This is Dungeon Crawl " VERSION " (Last build " BUILD_DATE ")." );
}                               // end version()

void adjust(void)
{
    mpr( MSGCH_PROMPT, "Adjust (i)tems, (s)pells, or (a)bilities?" );

    unsigned char keyin = tolower( get_ch() );

    if (keyin == 'i')
        adjust_item();
    else if (keyin == 's')
        adjust_spells();
    else if (keyin == 'a')
        adjust_ability();
    else if (keyin == ESCAPE)
        canned_msg( MSG_OK );
    else
        canned_msg( MSG_HUH );
}                               // end adjust()

static void adjust_item(void)
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

    you.inv[from_slot].slot = from_slot;
    you.inv[to_slot].slot = to_slot;

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
        set_redraw_status( REDRAW_WIELD );
}                               // end adjust_item()

static void adjust_spells_cleanup(bool needs_redraw)
{
    if (needs_redraw)
        redraw_screen();
}

static unsigned char read_spell_letter( const char *prompt,
                                        unsigned char (*show_func)(void),
                                        bool &needs_redraw )
{
    unsigned char keyin;

    for (;;)
    {
        mpr( MSGCH_PROMPT, prompt );
        keyin = get_ch();

        if (keyin == '?' || keyin == '*')
        {
            keyin = show_func();
            needs_redraw = true;
        }

        if (isalpha( keyin ) || keyin == ESCAPE)
            break;
        else
            mesclr( true );
    }

    return (keyin);
}

static void adjust_spells(void)
{
    unsigned char index_1, index_2;
    unsigned char keyin;

    bool needs_redraw = false;

    if (!you.spell_no)
    {
        mpr("You don't know any spells.");
        return;
    }

    keyin = read_spell_letter( "Adjust which spell?", list_spells, needs_redraw );

    if (keyin == ESCAPE)
    {
        adjust_spells_cleanup( needs_redraw );
        canned_msg( MSG_OK );
        return;
    }

    int input_1 = static_cast<int>( keyin );

    if (!isalpha( input_1 ))
    {
        adjust_spells_cleanup( needs_redraw );
        mpr("You don't know that spell.");
        return;
    }

    index_1 = letter_to_index( input_1 );
    int spell = get_spell_by_letter( input_1 );

    if (spell == SPELL_NO_SPELL)
    {
        adjust_spells_cleanup( needs_redraw );
        mpr("You don't know that spell.");
        return;
    }

    // print out targeted spell:
    adjust_spells_cleanup( needs_redraw );
    mpr( "%c - %s", input_1, spell_title( spell ) );

    keyin = read_spell_letter( "Adjust to which letter?", list_spells, needs_redraw );

    if (keyin == ESCAPE)
    {
        adjust_spells_cleanup( needs_redraw );
        canned_msg( MSG_OK );
        return;
    }

    int input_2 = static_cast<int>( keyin );

    if (!isalpha( input_2 ))
    {
        adjust_spells_cleanup( needs_redraw );
        mpr("What?");
        return;
    }

    adjust_spells_cleanup( needs_redraw );

    index_2 = letter_to_index( input_2 );

    // swap references in the letter table:
    int tmp = you.spell_letter_table[index_2];
    you.spell_letter_table[index_2] = you.spell_letter_table[index_1];
    you.spell_letter_table[index_1] = tmp;

    // print out spell in new slot (now at input_2)
    mpr( "%c - %s", input_2, spell_title( get_spell_by_letter(input_2) ) );

    // print out other spell if one was involved (now at input_1)
    spell = get_spell_by_letter( input_1 );

    if (spell != SPELL_NO_SPELL)
        mpr( "%c - %s", input_1, spell_title(spell) );
}                               // end adjust_spells()

static void adjust_ability(void)
{
    int index_1, index_2;
    unsigned char keyin;
    bool needs_redraw = false;

    if (!generate_abilities())
    {
        mpr( "You don't currently have any abilities." );
        return;
    }

    keyin = read_spell_letter( "Adjust which ability?", show_abilities, needs_redraw );

    if (keyin == ESCAPE)
    {
        adjust_spells_cleanup( needs_redraw );
        canned_msg( MSG_OK );
        return;
    }

    int input_1 = static_cast<int>( keyin );

    if (!isalpha( input_1 ))
    {
        adjust_spells_cleanup( needs_redraw );
        mpr("You don't have that ability.");
        return;
    }

    index_1 = letter_to_index( input_1 );

    if (you.ability_letter_table[index_1] == ABIL_NON_ABILITY)
    {
        adjust_spells_cleanup( needs_redraw );
        mpr("You don't have that ability.");
        return;
    }

    // print out targeted spell:
    adjust_spells_cleanup( needs_redraw );

    mpr( "%c - %s", input_1, get_ability_name_by_index( index_1 ) );

    keyin = read_spell_letter( "Adjust to which letter?", show_abilities, needs_redraw );

    if (keyin == ESCAPE)
    {
        adjust_spells_cleanup( needs_redraw );
        canned_msg( MSG_OK );
        return;
    }

    int input_2 = static_cast<int>( keyin );

    if (!isalpha( input_2 ))
    {
        adjust_spells_cleanup( needs_redraw );
        mpr("What?");
        return;
    }

    adjust_spells_cleanup( needs_redraw );

    index_2 = letter_to_index( input_2 );

    // swap references in the letter table:
    int tmp = you.ability_letter_table[index_2];
    you.ability_letter_table[index_2] = you.ability_letter_table[index_1];
    you.ability_letter_table[index_1] = tmp;

    // Note:  the input_2/index_1 and input_1/index_2 here is intentional.
    // This is because nothing actually moves until generate_abilities is
    // called again... fortunately that has to be done everytime because
    // that's the silly way this system currently works.  -- bwr
    mpr( "%c - %s", input_2, get_ability_name_by_index( index_1 ) );

    if (you.ability_letter_table[index_1] != ABIL_NON_ABILITY)
        mpr( "%c - %s", input_1, get_ability_name_by_index( index_2 ) );
}                               // end adjust_ability()

void list_armour(void)
{
    for (int i = EQ_CLOAK; i <= EQ_BODY_ARMOUR; i++)
    {
        int armour_id = you.equip[i];
        int colour = LIGHTGREY;

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
            colour = you.inv[armour_id].colour;
        }
        else
        {
            strcat(info, "    none");
        }

        mpr( MSGCH_EQUIPMENT, colour, info );
    }
}                               // end list_armour()

void list_jewellery(void)
{
    for (int i = EQ_LEFT_RING; i <= EQ_AMULET; i++)
    {
        int jewellery_id = you.equip[i];
        int colour = LIGHTGREY;

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
            colour = you.inv[jewellery_id].colour;
        }
        else
        {
            strcat(info, "    none");
        }

        mpr( MSGCH_EQUIPMENT, colour, info );
    }
}                               // end list_jewellery()

void list_weapons(void)
{
    char str_pass[ ITEMNAME_SIZE ];
    const int weapon_id = you.equip[EQ_WEAPON];
    int colour = LIGHTGREY;

    // Output the current weapon
    //
    // Yes, this is already on the screen... I'm outputing it
    // for completeness and to avoid confusion.
    strcpy( info, "Current      : " );

    if (weapon_id != -1)
    {
        in_name( weapon_id, DESC_INVENTORY_EQUIP, str_pass );
        strcat( info, str_pass );
        colour = you.inv[weapon_id].colour;
    }
    else
    {
        if (you.attribute[ATTR_TRANSFORMATION] == TRAN_BLADE_HANDS)
        {
            strcat( info, "    blade hands" );
            colour = RED;
        }
        else
            strcat( info, "    empty hands" );
    }

    mpr( MSGCH_EQUIPMENT, colour, info );

    // Print out the swap slots
    for (int i = 0; i <= 1; i++)
    {
        colour = LIGHTGREY;

        // We'll avoid repeating the current weapon for these slots,
        // in order to keep things clean.
        if (weapon_id == i)
            continue;

        strcpy( info, (i == 0) ? "Primary      : " : "Secondary    : " );

        if (is_valid_item( you.inv[i] ))
        {
            in_name(i, DESC_INVENTORY_EQUIP, str_pass);
            strcat(info, str_pass);
            colour = you.inv[i].colour;
        }
        else
            strcat(info, "    none");

        mpr( MSGCH_EQUIPMENT, colour, info );              // Output lot
    }

    // Now we print out the current default fire weapon
    strcpy(info, "Firing       : ");
    colour = LIGHTGREY;

    const int item = get_fire_item_index();
    if (item == ENDOFPACK)
        strcat(info, "    nothing");
    else
    {
        in_name( item, DESC_INVENTORY_EQUIP, str_pass );
        strcat( info, str_pass );
        colour = you.inv[item].colour;
    }

    mpr( MSGCH_EQUIPMENT, colour, info );

    if (!player_can_cut_meat() && Options.easy_butcher)
    {
        strcpy( info, "Cutting Tool : " );
        colour = LIGHTGREY;

        const int knife = find_butcher_knife();
        if (knife == ENDOFPACK)
            strcat( info, "    nothing" );
        else
        {
            in_name( knife, DESC_INVENTORY_EQUIP, str_pass );
            strcat( info, str_pass );
            colour = you.inv[knife].colour;
        }

        mpr( MSGCH_EQUIPMENT, colour, info );
    }

}                               // end list_weapons()

/*
 *  File:       command.cc
 *  Summary:    Misc commands.
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 *      <3>     6/13/99         BWR             New equipment listing commands
 *      <2>     5/20/99         BWR             Swapping inventory letters.
 *      <1>     -/--/--         LRH             Created
 */

#include "AppHdr.h"
#include "command.h"

#include <stdio.h>
#include <string.h>

#include "externs.h"
#include "enum.h"
#include "ouch.h"
#include "message.h"
#include "stuff.h"
#include "items.h"
#include "itemname.h"
#include "invent.h"
#include "spells0.h"
#include "version.h"

void adjust_item(void);
void adjust_spells(void);

extern char wield_change;

void quit_game()
{
    strcpy(info, "Really quit?");
    mpr(info);

    char keyin = get_ch();

    if (keyin == 'y' || keyin == 'Y')
    {
        ouch(-9999, 0, 13);

    }
}                               // end of void quit_game

void version(void)
{
    strcpy(info, "This is Dungeon Crawl v" VERSION ". (Last build 26/3/99)");
    mpr(info);
}


void adjust()
{
    strcpy(info, "Adjust (i)tems or (s)pells?");
    mpr(info);

    unsigned char keyin = get_ch();

    if (keyin == 'i' || keyin == 'I')
    {
        adjust_item();
        return;
    }
    if (keyin == 's' || keyin == 'S')
    {
        adjust_spells();
        return;
    }

    mpr("Huh?");
    return;
}


void adjust_item()
{
    unsigned char throw_2, throw_3;
    unsigned nthing = 0;

    if (you.num_inv_items == 0)
    {
        strcpy(info, "You aren't carrying anything.");
        mpr(info);
        return;
    }

query:
    strcpy(info, "Adjust which item?");
    mpr(info);

    unsigned char keyin = get_ch();

    if (keyin == '?' || keyin == '*')
    {
        if (keyin == '*' || keyin == '?')
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
        strcpy(info, "You don't have any such object.");
        mpr(info);
        return;
    }

    throw_2 = conv_lett(throw_1);

    if (you.inv_quantity[throw_2] == 0)
    {
        strcpy(info, "You don't have any such object.");
        mpr(info);
        return;
    }

    strcpy(info, " ");
    if (throw_2 <= 25)
        info[0] = throw_2 + 97;
    else
        info[0] = throw_2 + 39;

    info[1] = '\0';                /* This null-terminates it, right? */
    strcat(info, " - ");

    in_name(throw_2, 3, str_pass);
    strcat(info, str_pass);
    mpr(info);

    strcpy(info, "Adjust to which letter?");
    mpr(info);

    keyin = get_ch();

    if (keyin == '?' || keyin == '*')
    {
        if (keyin == '*' || keyin == '?')
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

    throw_1 = (int) keyin;

    if (throw_1 < 65 || (throw_1 > 90 && throw_1 < 97) || throw_1 > 122)
    {
        strcpy(info, "What?");
        mpr(info);
        return;
    }

    throw_3 = conv_lett(throw_1);

    // if (you.inv_quantity [throw_3] != 0)
    // {
    //      strcpy(info, "Sorry, that inventory letter is already in use.");
    //      mpr(info);
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
        {
            you.equip[i] = throw_3;
        }
        else if (you.equip[i] == throw_3)
        {
            you.equip[i] = throw_2;
        }
    }

    // you.inv_quantity [throw_2] = 0;

    strcpy(info, " ");
    if (throw_3 <= 25)
        info[0] = throw_3 + 97;
    else
        info[0] = throw_3 + 39;

    info[1] = '\0';                /* This null-terminates it, right? */
    strcat(info, " - ");

    in_name(throw_3, 3, str_pass);
    strcat(info, str_pass);
    mpr(info);

    if (you.inv_quantity[throw_2] > 0)
    {
        strcpy(info, " ");
        if (throw_2 <= 25)
            info[0] = throw_2 + 97;
        else
            info[0] = throw_2 + 39;

        info[1] = '\0';                /* This null-terminates it, right? */
        strcat(info, " - ");

        in_name(throw_2, 3, str_pass);
        strcat(info, str_pass);
        mpr(info);
    }

    if (throw_3 == you.equip[EQ_WEAPON] || throw_2 == you.equip[EQ_WEAPON])
        wield_change = 1;

}

#ifdef PLAIN_TERM

static void adjust_spells_cleanup( bool needs_redraw )
{
    if (needs_redraw) {
        redraw_screen();
    }
}

#endif

void adjust_spells(void)
{
    unsigned char throw_2, throw_3;
    unsigned char nthing = 0;

#ifdef PLAIN_TERM
    bool needs_redraw = false;
#endif

    if (you.spell_no == 0)
    {
        strcpy(info, "You don't know any spells.");
        mpr(info);
        return;
    }

query:
    strcpy(info, "Adjust which spell?");
    mpr(info);

    unsigned char keyin = get_ch();

    if (keyin == '?' || keyin == '*')
    {
        if (keyin == '*' || keyin == '?')
        {
            nthing = spell_list();
#ifdef PLAIN_TERM
            needs_redraw = true;
#endif
        }

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

    if (throw_1 < 97 || throw_1 > 119)
    {
#ifdef PLAIN_TERM
        adjust_spells_cleanup(needs_redraw);
#endif
        strcpy(info, "You don't know that spell.");
        mpr(info);
        return;
    }

    throw_2 = conv_lett(throw_1);

    if (you.spells[throw_2] == 210)
    {
#ifdef PLAIN_TERM
        adjust_spells_cleanup(needs_redraw);
#endif
        strcpy(info, "You don't know that spell.");
        mpr(info);
        return;
    }

    strcpy(info, " ");
    info[0] = throw_2 + 97;
    info[1] = '\0';                /* This null-terminates it, right? */
    strcat(info, " - ");
    spell_name(you.spells[throw_2], str_pass);
    strcat(info, str_pass);
    mpr(info);

    strcpy(info, "Adjust to which letter?");
    mpr(info);

    keyin = get_ch();

    if (keyin == '?' || keyin == '*')
    {
        if (keyin == '*' || keyin == '?')
        {
            nthing = spell_list();
#ifdef PLAIN_TERM
            needs_redraw = true;
#endif
        }

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

    throw_1 = (int) keyin;

    //if (throw_1 < 97 || throw_1 > 122)
    if (throw_1 < 97 || throw_1 > 118)
    {
#ifdef PLAIN_TERM
        adjust_spells_cleanup(needs_redraw);
#endif
        strcpy(info, "What?");
        mpr(info);
        return;
    }

#ifdef PLAIN_TERM
        adjust_spells_cleanup(needs_redraw);
#endif

    throw_3 = conv_lett(throw_1);

    int backup = you.spells[throw_3];

    you.spells[throw_3] = you.spells[throw_2];
    you.spells[throw_2] = backup;

    strcpy(info, " ");
    info[0] = throw_3 + 97;
    info[1] = '\0';                /* This null-terminates it, right? */
    strcat(info, " - ");
    spell_name(you.spells[throw_3], str_pass);
    strcat(info, str_pass);
    mpr(info);

    if (you.spells[throw_2] != 210)
    {
        strcpy(info, " ");
        info[0] = throw_2 + 97;
        info[1] = '\0';            /* This null-terminates it, right? */
        strcat(info, " - ");
        spell_name(you.spells[throw_2], str_pass);
        strcat(info, str_pass);
        mpr(info);
    }

}

void get_letter_id( char buff[5], int item_id )
{
    if (item_id <= 25)
        buff[0] = item_id + 97;
    else
        buff[0] = item_id + 39;

    buff[1] = '\0';
    strcat( buff, " - " );
}

void list_armour( void )
{
    for(int i = EQ_CLOAK; i <= EQ_BODY_ARMOUR; i++)
    {
        int  armour_id = you.equip[i];
        char letter_buff[5];

        switch (i) {
        case EQ_CLOAK:
            strcpy(info, "Cloak  : ");
            break;

        case EQ_HELMET:
            strcpy(info, "Helmet : ");
            break;

        case EQ_GLOVES:
            strcpy(info, "Gloves : ");
            break;

        case EQ_BOOTS:
            if (you.species == SP_CENTAUR || you.species == SP_NAGA)
                strcpy(info, "Barding: ");
            else
                strcpy(info, "Boots  : ");
            break;

        case EQ_SHIELD:
            strcpy(info, "Shield : ");
            break;

        case EQ_BODY_ARMOUR:
            strcpy(info, "Armour : ");
            break;
        }

        if (armour_id != -1)
        {
            get_letter_id( letter_buff, armour_id );
            strcat( info, letter_buff );

            in_name( armour_id, 3, str_pass );
            strcat( info, str_pass );
        }
        else
        {
            strcat( info, "    none" );
        }

        mpr( info );
    }
}

void list_jewellery( void )
{
    for(int i = EQ_LEFT_RING; i <= EQ_AMULET; i++)
    {
        int   jewellery_id = you.equip[i];
        char  letter_buff[5];

        switch (i) {
        case EQ_LEFT_RING:
            strcpy(info, "Left ring  : ");
            break;

        case EQ_RIGHT_RING:
            strcpy(info, "Right ring : ");
            break;

        case EQ_AMULET:
            strcpy(info, "Amulet     : ");
            break;
        }

        if (jewellery_id != -1)
        {
            get_letter_id( letter_buff, jewellery_id );
            strcat( info, letter_buff );

            in_name( jewellery_id, 3, str_pass );
            strcat( info, str_pass );
        }
        else
        {
            strcat( info, "    none" );
        }

        mpr( info );
    }
}

void list_weapons( void )
{
    char       letter_buff[5];
    const int  weapon_id = you.equip[EQ_WEAPON];

    // Output the current weapon
    //
    // Yes, this is already on the screen... I'm outputing it
    // for completeness and to avoid confusion.

    strcpy( info, "Current   : " );

    if (weapon_id != -1)
    {
        get_letter_id( letter_buff, weapon_id );
        strcat( info, letter_buff );

        in_name( weapon_id, 3, str_pass );
        strcat( info, str_pass );
        strcat( info, " (weapon)" );
    }
    else
    {
        if (you.attribute[ATTR_TRANSFORMATION] == TRAN_BLADE_HANDS)
            strcat( info, "    blade hands" );
        else
            strcat( info, "    empty hands" );
    }

    mpr( info );


    // Print out the swap slots
    for (int i = 0; i <= 1; i++)
    {
        // We'll avoid repeating the current weapon for these slots,
        // in order to keep things clean.
        if (weapon_id == i)
            continue;

        strcpy( info, (i == 0) ? "Primary   : " : "Secondary : " );

        if (you.inv_quantity[i] > 0)
        {
            get_letter_id( letter_buff, i );
            strcat( info, letter_buff );

            in_name( i, 3, str_pass );
            strcat( info, str_pass );

            // This is useful information
            for (int j = EQ_CLOAK; j <= EQ_AMULET; j++)
            {
                if (you.equip[j] == i)
                    strcat( info, " (worn)" );
            }
        }
        else
        {
            strcat( info, "    none" );
        }

        mpr( info );    // Output slot
    }


    // Now we print out the current default throwing weapon
    int  type_wanted;
    int  throw_id = -1;

    strcpy( info, "Firing    : " );

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

    for (int i = 0; i < 52; i++)
    {
        if (you.inv_quantity[i] == 0)
            continue;

        if (you.inv_class[i] == OBJ_MISSILES && you.inv_type[i] == type_wanted)
        {
            throw_id = i;
            break;
        }
    }

    // We'll print this one even if it is the current weapon
    if (throw_id != -1)
    {
        get_letter_id( letter_buff, throw_id );
        strcat( info, letter_buff );

        in_name( throw_id, 3, str_pass );
        strcat( info, str_pass );

        if (weapon_id == throw_id)
        {
            strcat( info, " (weapon)" );
        }
    }
    else
    {
        strcat( info, "    nothing" );
    }

    mpr( info );
}

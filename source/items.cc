/*
 *  File:       items.cc
 *  Summary:    Misc (mostly) inventory related functions.
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 * <8> 8/07/99   BWR  Added Rune stacking
 * <7> 6/13/99   BWR  Added auto staff detection
 * <6> 6/12/99   BWR  Fixed time system.
 * <5> 6/9/99    DML  Autopickup
 * <4> 5/26/99   JDJ  Drop will attempt to take off armor.
 * <3> 5/21/99   BWR  Upped armour skill learning slightly.
 * <2> 5/20/99   BWR  Added assurance that against inventory count being wrong.
 * <1> -/--/--   LRH  Created
 */

#include "AppHdr.h"
#include "items.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#ifdef DOS
#include <conio.h>
#endif

#include "externs.h"

#include "beam.h"
#include "debug.h"
#include "effects.h"
#include "fight.h"
#include "invent.h"
#include "it_use2.h"
#include "item_use.h"
#include "itemname.h"
#include "misc.h"
#include "monplace.h"
#include "mon-util.h"
#include "mutation.h"
#include "player.h"
#include "religion.h"
#include "shopping.h"
#include "skills.h"
#include "spells.h"
#include "stuff.h"

extern bool wield_change;       // defined in output.cc

int last_item = NON_ITEM;

void autopickup(void);
int add_item(int item_got, int it_quant);

// This function returns the mass of *one* of mitm #item, multiply
// by quantity for total mass.
int mass_item( int item )
{
    int unit_mass = 0;

    if (mitm.base_type[ item ] == OBJ_GOLD)
        unit_mass = 0;
    else if (mitm.base_type[ item ] == OBJ_CORPSES)
    {
        unit_mass = mons_weight( mitm.pluses[ item ] );

        if (mitm.sub_type[ item ] == CORPSE_SKELETON)
            unit_mass /= 2;
    }
    else
        unit_mass = mass( mitm.base_type[ item ], mitm.sub_type[ item ] );

    return (unit_mass > 0 ? unit_mass : 0);
}

/*
   Takes keyin as an argument because it will only display a long list of items
   if ; is pressed.
 */
void item_check(char keyin)
{
    char item_show[50][50];
    char temp_quant[10];
    char str_pass[50];

    int counter = 0;
    int counter_max = 0;

    const int grid = grd[you.x_pos][you.y_pos];

    if (grid >= DNGN_ENTER_HELL && grid <= DNGN_PERMADRY_FOUNTAIN)
    {
        if (grid >= DNGN_STONE_STAIRS_DOWN_I && grid <= DNGN_ROCK_STAIRS_DOWN)
        {
            strcpy(info, "There is a ");
            strcat(info, (grid == DNGN_ROCK_STAIRS_DOWN) ? "rock" : "stone");
            strcat(info, " staircase leading down here.");
            mpr(info);
        }
        else if (grid >= DNGN_STONE_STAIRS_UP_I && grid <= DNGN_ROCK_STAIRS_UP)
        {
            strcpy(info, "There is a ");
            strcat(info, (grid == DNGN_ROCK_STAIRS_UP) ? "rock" : "stone");
            strcat(info, " staircase leading upwards here.");
            mpr(info);
        }
        else
        {
            switch (grid)
            {
            case DNGN_ENTER_HELL:
                mpr("There is a gateway to hell here.");
                break;
            case DNGN_ENTER_GEHENNA:
                mpr("There is a gateway to Gehenna here.");
                break;
            case DNGN_ENTER_COCYTUS:
                mpr("There is a gateway to the frozen wastes of Cocytus here.");
                break;
            case DNGN_ENTER_TARTARUS:
                mpr("There is a gateway to Tartarus here.");
                break;
            case DNGN_ENTER_DIS:
                mpr("There is a gateway to the Iron City of Dis here.");
                break;
            case DNGN_ENTER_SHOP:
                sprintf(info, "There is an entrance to %s here.", shop_name(you.x_pos, you.y_pos));
                mpr(info);
                break;
            case DNGN_ENTER_LABYRINTH:
                mpr("There is an entrance to a labyrinth here.");
                mpr("Beware, for starvation awaits!");
                break;
            case DNGN_ENTER_ABYSS:
                mpr("There is a one-way gate to the infinite horrors of the Abyss here.");
                break;
            case DNGN_STONE_ARCH:
                mpr("There is an empty stone archway here.");
                break;
            case DNGN_EXIT_ABYSS:
                mpr("There is a gateway leading out of the Abyss here.");
                break;
            case DNGN_ENTER_PANDEMONIUM:
                mpr("There is a gate leading to the halls of Pandemonium here.");
                break;
            case DNGN_EXIT_PANDEMONIUM:
                mpr("There is a gate leading out of Pandemonium here.");
                break;
            case DNGN_TRANSIT_PANDEMONIUM:
                mpr("There is a gate leading to another region of Pandemonium here.");
                break;
            case DNGN_ENTER_ORCISH_MINES:
                mpr("There is a staircase to the orcish mines here.");
                break;
            case DNGN_ENTER_HIVE:
                mpr("There is a staircase to the Hive here.");
                break;
            case DNGN_ENTER_LAIR:
                mpr("There is a staircase to the Lair here.");
                break;
            case DNGN_ENTER_SLIME_PITS:
                mpr("There is a staircase to the Slime Pits here.");
                break;
            case DNGN_ENTER_VAULTS:
                mpr("There is a staircase to the Vaults here.");
                break;
            case DNGN_ENTER_CRYPT:
                mpr("There is a staircase to the Crypt here.");
                break;
            case DNGN_ENTER_HALL_OF_BLADES:
                mpr("There is a staircase to the Hall of Blades here.");
                break;
            case DNGN_ENTER_ZOT:
                mpr("There is a gate to the Realm of Zot here.");
                break;
            case DNGN_ENTER_TEMPLE:
                mpr("There is a staircase to the Ecumenical Temple here.");
                break;
            case DNGN_ENTER_SNAKE_PIT:
                mpr("There is a staircase to the Snake Pit here.");
                break;
            case DNGN_ENTER_ELVEN_HALLS:
                mpr("There is a staircase to the Elven Halls here.");
                break;
            case DNGN_ENTER_TOMB:
                mpr("There is a staircase to the Tomb here.");
                break;
            case DNGN_ENTER_SWAMP:
                mpr("There is a staircase to the Swamp here.");
                break;
            case DNGN_RETURN_DUNGEON_I:
            case DNGN_RETURN_DUNGEON_II:
            case DNGN_RETURN_DUNGEON_III:
            case DNGN_RETURN_DUNGEON_IV:
            case DNGN_RETURN_DUNGEON_V:
                mpr("There is a staircase back to the Dungeon here.");
                break;
            case DNGN_RETURN_LAIR_II:
            case DNGN_RETURN_LAIR_III:
            case DNGN_RETURN_LAIR_IV:
                mpr("There is a staircase back to the Lair here.");
                break;
            case DNGN_RETURN_VAULTS_II:
            case DNGN_RETURN_VAULTS_III:
                mpr("There is a staircase back to the Vaults here.");
                break;
            case DNGN_RETURN_CRYPT:
                mpr("There is a staircase back to the Crypt here.");
                break;
            case DNGN_RETURN_MINES:
                mpr("There is a staircase back to the Mines here.");
                break;
            case DNGN_EXIT_ZOT:
                mpr("There is a gate leading back out of this place here.");
                break;
            case DNGN_ALTAR_ZIN:
                mpr("There is a glowing white marble altar of Zin here.");
                break;
            case DNGN_ALTAR_SHINING_ONE:
                mpr("There is a glowing golden altar of the Shining One here.");
                break;
            case DNGN_ALTAR_KIKUBAAQUDGHA:
                mpr("There is an ancient bone altar of Kikubaaqudgha here.");
                break;
            case DNGN_ALTAR_YREDELEMNUL:
                mpr("There is a basalt altar of Yredelemnul here.");
                break;
            case DNGN_ALTAR_XOM:
                mpr("There is a shimmering altar of Xom here.");
                break;
            case DNGN_ALTAR_VEHUMET:
                mpr("There is a shining altar of Vehumet here.");
                break;
            case DNGN_ALTAR_OKAWARU:
                mpr("There is an iron altar of Okawaru here.");
                break;
            case DNGN_ALTAR_MAKHLEB:
                mpr("There is a burning altar of Makhleb here.");
                break;
            case DNGN_ALTAR_SIF_MUNA:
                mpr("There is a deep blue altar of Sif Muna here.");
                break;
            case DNGN_ALTAR_TROG:
                mpr("There is a bloodstained altar of Trog here.");
                break;
            case DNGN_ALTAR_NEMELEX_XOBEH:
                mpr("There is a sparkling altar of Nemelex Xobeh here.");
                break;
            case DNGN_ALTAR_ELYVILON:
                mpr("There is a silver altar of Elyvilon here.");
                break;
            case DNGN_BLUE_FOUNTAIN:
                mpr("There is a fountain here (q to drink).");
                break;
            case DNGN_SPARKLING_FOUNTAIN:
                mpr("There is a sparkling fountain here (q to drink).");
                break;
            case DNGN_DRY_FOUNTAIN_I:
            case DNGN_DRY_FOUNTAIN_II:
            case DNGN_DRY_FOUNTAIN_IV:
            case DNGN_DRY_FOUNTAIN_VI:
            case DNGN_DRY_FOUNTAIN_VIII:
            case DNGN_PERMADRY_FOUNTAIN:
                mpr("There is a dry fountain here.");
                break;
            }
        }
    }

    if (igrd[you.x_pos][you.y_pos] == NON_ITEM && keyin == ';')
    {
        mpr("There are no items here.");
        return;
    }

    autopickup();

    int objl = igrd[you.x_pos][you.y_pos];
    int hrg = 0;

    while (objl != NON_ITEM)
    {
        counter++;

        if (counter > 45)
        {
            strcpy(item_show[counter], "Too many items.");
            break;
        }

        if (mitm.base_type[objl] == OBJ_GOLD)
        {
            itoa(mitm.quantity[objl], temp_quant, 10);
            strcpy(item_show[counter], temp_quant);
            strcat(item_show[counter], " gold piece");
            if (mitm.quantity[objl] > 1)
                strcat(item_show[counter], "s");

            goto linking;       //continue;
        }

        it_name(objl, 3, str_pass);
        strcpy(item_show[counter], str_pass);

      linking:
        hrg = mitm.link[objl];
        objl = hrg;
    }

    counter_max = counter;
    counter = 0;

    if (counter_max == 1)
    {
        strcpy(info, "You see here ");  // remember 'an'.

        strcat(info, item_show[counter_max]);
        strcat(info, ".");
        mpr(info);

        counter++;
        counter_max = 0;        // to skip next part.

    }

    if ((counter_max > 0 && counter_max < 6)
        || (counter_max > 1 && keyin == ';'))
    {
        mpr("Things that are here:");

        while (counter < counter_max)
        {
            // this is before the strcpy because item_show start at 1, not 0.
            counter++;
            mpr(item_show[counter]);
        }
    }

    if (counter_max > 5 && keyin != ';')
        mpr("There are several objects here.");
}

void pickup(void)
{
    int items_here = 0;
    int counter = 0;

    int item_got = 0;
    int o = 0;
    int k = 0;
    int m = 0;
    int nothing = 0;
    char str_pass[50];
    unsigned char keyin = 0;

    if (you.levitation && !wearing_amulet(AMU_CONTROLLED_FLIGHT))
    {
        mpr("You can't reach the floor from up here.");
        return;
    }

    // XXX: This is probably bad... what if the player tries out their
    // new portable altar in the temple? -- bwr
    if (grd[you.x_pos][you.y_pos] == DNGN_ALTAR_NEMELEX_XOBEH
        && you.where_are_you != BRANCH_ECUMENICAL_TEMPLE)
    {
        if (inv_count() >= ENDOFPACK)
        {
            mpr("There is a portable altar here, but you can't carry anything else.");
            return;
        }

        if (yesno("There is a portable altar here. Pick it up?"))
        {
            for (m = 0; m < ENDOFPACK; m++)
            {
                if (you.inv_quantity[m] == 0)
                {
                    you.inv_ident[m] = 3;
                    you.inv_class[m] = OBJ_MISCELLANY;
                    you.inv_type[m] = MISC_PORTABLE_ALTAR_OF_NEMELEX;
                    you.inv_plus[m] = 0;
                    you.inv_plus2[m] = 0;
                    you.inv_dam[m] = 0;
                    you.inv_colour[m] = LIGHTMAGENTA;
                    you.inv_quantity[m] = 1;
                    burden_change();
                    info[0] = index_to_letter(m);
                    info[1] = '\0';
                    strcat(info, " - ");
                    in_name(m, 3, str_pass);
                    strcat(info, str_pass);
                    mpr(info);
                    break;
                }
            }

            grd[you.x_pos][you.y_pos] = DNGN_FLOOR;
        }
    }

    if (igrd[you.x_pos][you.y_pos] == NON_ITEM)
    {
        mpr("There are no items here.");
        return;
    }

    last_item = NON_ITEM;

    int objl = igrd[you.x_pos][you.y_pos];
    int hrg = 0;

    while (objl != NON_ITEM)
    {
        counter++;

        last_item = objl;

        hrg = mitm.link[objl];
        objl = hrg;
        items_here++;

        if (counter > 1000)
        {
            error_message_to_player();
            return;
        }
    }

    // Anywhere which calls add_item() doesn't need to arrange it[0].link etc.
    if (items_here == 1)
    {
        item_got = igrd[you.x_pos][you.y_pos];
        last_item = NON_ITEM;
        nothing = add_item(item_got, mitm.quantity[igrd[you.x_pos][you.y_pos]]);

        if (nothing == NON_ITEM)
            mpr("You can't carry that many items.");
        else if (nothing != 1)
            mpr("You can't carry that much weight.");

        return;
    }                           // end of if items_here

    last_item = NON_ITEM;

    if (items_here > 1)
    {
        mpr("There are several objects here.");

        o = igrd[you.x_pos][you.y_pos];

        for (k = 0; k < items_here; k++)
        {
            if (keyin != 'a')
            {
                strcpy(info, "pick up ");

                if (mitm.base_type[o] == OBJ_GOLD)
                {
                    itoa(mitm.quantity[o], st_prn, 10);
                    strcat(info, st_prn);
                    strcat(info, " gold piece");

                    if (mitm.quantity[o] > 1)
                        strcat(info, "s");
                }
                else
                {
                    it_name(o, 3, str_pass);
                    strcat(info, str_pass);
                }

                strcat(info, "\? (y,n,a,q)");
                mpr(info);
            }

            if (keyin != 'a')
                keyin = get_ch();

            if (keyin == 'q')
                return;

            if (keyin == 'y' || keyin == 'a')
            {
                item_got = o;

                int grunk = add_item(o, mitm.quantity[o]);

                if (grunk == 0)
                {
                    mpr("You can't carry that much weight.");
                    keyin = 'x';        // resets from 'a'
                }

                if (grunk == NON_ITEM)
                {
                    mpr("You can't carry that many items.");
                    keyin = 'x';        // resets from 'a'
                }

                if (grunk != 1) // ie if the item picked up is still there.
                    last_item = item_got;
            }

            if (mitm.quantity[o] > 0)
                last_item = o;

            hrg = mitm.link[o];

            if (mitm.quantity[o] == 0)
                mitm.link[o] = NON_ITEM;

            o = hrg;
            if (o == NON_ITEM)
                return;

            if (items_here == 0)
                break;
        }                       // end of while k loop

        mpr("That's all.");
    }                           // end of if items_here
}                               // end pickup()

int add_item(int item_got, int quant_got)
{
    int item_mass = 0;
    int unit_mass = 0;
    int retval = 1;
    char brek = 0;
    bool partialPickup = false;

    //int last_item = NON_ITEM;
    int m = 0;
    char str_pass[50];

    unit_mass = mass_item( item_got );
    item_mass = unit_mass * mitm.quantity[item_got];
    quant_got = mitm.quantity[item_got];
    brek = 0;

    // multiply both constants * 10

    if ((int) you.burden + item_mass > carrying_capacity())
    {
        if (mitm.quantity[item_got] == 1)
            return 0;

        for (m = mitm.quantity[item_got]; m > 0; m--)
        {
            if (m == 0)
                break;

            if (unit_mass * m + you.burden <= carrying_capacity())
            {
                quant_got = m;
                brek = 1;
                item_mass = unit_mass * m;
                partialPickup = true;
                break;
            }
        }

        if (brek == 0)
            return 0;
        else
            retval = 2;
    }

    brek = 0;

    if (mitm.base_type[item_got] == OBJ_GOLD)
    {
        you.gold += quant_got;
        you.redraw_gold = 1;
        strcpy(info, "You pick up ");
        itoa(quant_got, st_prn, 10);
        strcat(info, st_prn);
        strcat(info, " gold piece");
        strcat(info, (quant_got > 1)?"s.":".");
        mpr(info);

        you.turn_is_over = 1;
        alert();

        goto change_igrid;
    }

    for (m = 0; m < ENDOFPACK; m++)
    {
        const int base_type = mitm.base_type[item_got];
        const int sub_type  = mitm.sub_type[item_got];

        if ((base_type == OBJ_MISSILES
                || (base_type == OBJ_FOOD && sub_type != FOOD_CHUNK)
                || base_type == OBJ_SCROLLS
                || base_type == OBJ_POTIONS
                || (base_type == OBJ_MISCELLANY && sub_type == MISC_RUNE_OF_ZOT)
                || base_type == OBJ_UNKNOWN_II)
            && you.inv_class[m] == base_type
            && you.inv_type[m] == sub_type

            && (((base_type == OBJ_FOOD && sub_type != FOOD_CHUNK)
                 || base_type == OBJ_SCROLLS
                 || base_type == OBJ_POTIONS)
                 || (you.inv_plus[m] == mitm.pluses[item_got]
                     && you.inv_plus2[m] == mitm.pluses2[item_got]
                     && you.inv_dam[m] == mitm.special[item_got]))
            && you.inv_quantity[m] > 0)
        {
            if (mitm.id[item_got] == you.inv_ident[m]
                || base_type == OBJ_FOOD
                || base_type == OBJ_SCROLLS
                || base_type == OBJ_POTIONS)
            {
                if (partialPickup)
                    mpr("You can only carry some of what is here.");

                you.inv_quantity[m] += quant_got;
                burden_change();

                info[0] = index_to_letter(m);
                info[1] = '\0';

                strcat(info, " - ");

                in_name(m, 3, str_pass);
                strcat(info, str_pass);
                mpr(info);

                you.turn_is_over = 1;
                alert();
                goto change_igrid;
            }
        }
    }                           // end of for m loop.

    // can't combine, check for slot space
    if (inv_count() >= ENDOFPACK)
        return NON_ITEM;

    if (partialPickup)
        mpr("You can only carry some of what is here.");

    for (m = 0; m < ENDOFPACK; m++)
        if (!you.inv_quantity[m])
        {
            you.inv_ident[m] = mitm.id[item_got];
            you.inv_class[m] = mitm.base_type[item_got];
            you.inv_type[m] = mitm.sub_type[item_got];
            you.inv_plus[m] = mitm.pluses[item_got];
            you.inv_plus2[m] = mitm.pluses2[item_got];
            you.inv_dam[m] = mitm.special[item_got];
            you.inv_colour[m] = mitm.colour[item_got];
            you.inv_quantity[m] = quant_got;
            burden_change();

            //strcpy(info, " ");
            //strncpy(info, letters [m], 1);

            info[0] = index_to_letter(m);
            info[1] = '\0';

            strcat(info, " - ");
            in_name(m, 3, str_pass);
            strcat(info, str_pass);
            mpr(info);

            if (mitm.base_type[item_got] == OBJ_ORBS
                && you.char_direction == DIR_DESCENDING)
            {
                mpr("Now all you have to do is get back out of the dungeon!");
                you.char_direction = DIR_ASCENDING;
            }
            break;
        }

    you.turn_is_over = 1;

  change_igrid:
    mitm.quantity[item_got] -= quant_got;       //= 0;

    if (mitm.quantity[item_got] == 0)
    {
        // is this (last_item) ever set or even used properly? {dlb}
        if (last_item == NON_ITEM)
            igrd[you.x_pos][you.y_pos] = mitm.link[item_got];
        else
            mitm.link[last_item] = mitm.link[item_got];
    }

    return retval;
}                               // end add_item()

void item_place(int item_drop_2, int x_plos, int y_plos, int quant_drop)
{
    int m = 0, i = 0;
    const int base_type = you.inv_class[item_drop_2];
    const int sub_type  = you.inv_type[item_drop_2];

    // loop through items at current location
    i = igrd[x_plos][y_plos];

    while(i != NON_ITEM)
    {

        if ((base_type == OBJ_MISSILES
                || base_type == OBJ_FOOD
                || base_type == OBJ_SCROLLS
                || base_type == OBJ_POTIONS
                || base_type == OBJ_UNKNOWN_II)
            && base_type == mitm.base_type[ i ]
            && sub_type == mitm.sub_type[ i ]
            && you.inv_plus[item_drop_2] == mitm.pluses[ i ]
            && you.inv_plus2[item_drop_2] == mitm.pluses2[ i ]
            && you.inv_dam[item_drop_2] == mitm.special[ i ]
            && mitm.quantity[ i ] > 0)
        {
            if (you.inv_ident[item_drop_2] == mitm.id[ i ])
            {
                mitm.quantity[ i ] += quant_drop;
                you.turn_is_over = 1;
                return;
            }
        }
        // try to follow link
        i = mitm.link[i];
    }

    // item not found in current stack, add to top.
    for (i = 0; i < MAX_ITEMS; i++)
    {
        if (i >= (MAX_ITEMS - 20))
        {
            mpr("The demon of the infinite void grins at you.");
            return;
        }
        else if (mitm.quantity[i] == 0)
        {
            mitm.id[i] = you.inv_ident[item_drop_2];
            mitm.base_type[i] = you.inv_class[item_drop_2];
            mitm.sub_type[i] = you.inv_type[item_drop_2];
            mitm.pluses[i] = you.inv_plus[item_drop_2];
            mitm.pluses2[i] = you.inv_plus2[item_drop_2];
            mitm.special[i] = you.inv_dam[item_drop_2];
            mitm.colour[i] = you.inv_colour[item_drop_2];
            mitm.quantity[i] = quant_drop;
            break;
        }
    }

    // link item to top of list.
    m = igrd[x_plos][y_plos];
    igrd[x_plos][y_plos] = i;
    mitm.link[i] = m;

    you.turn_is_over = 1;
}                               // end item_place()


//---------------------------------------------------------------
//
// move_top_item -- moves the top item of a stack to a new
// location.
//
//---------------------------------------------------------------
bool move_top_item( int src_x, int src_y, int dest_x, int dest_y )
{
    const int item = igrd[ src_x ][ src_y ];

    if (item == NON_ITEM)
        return (false);

    // First we'll remove the item from it's old stack, by pointing
    // the grid reference to the link reference of the item.
    igrd[ src_x ][ src_y ] = mitm.link[ item ];

    // Now we'll add the item to the top of the destination stack
    int tmp = igrd[ dest_x ][ dest_y ];
    igrd[ dest_x ][ dest_y ] = item;
    mitm.link[ item ] = tmp;

    return (true);
}


//---------------------------------------------------------------
//
// drop_gold
//
//---------------------------------------------------------------
static void drop_gold(int amount)
{
    if (you.gold > 0)
    {
        if (amount > you.gold)
            amount = you.gold;

        char temp_quant[10];

        strcpy(info, "You drop ");
        itoa(amount, temp_quant, 10);

        strcat(info, temp_quant);
        strcat(info, " gold piece");
        strcat(info, (amount > 1) ? "s." : ".");
        mpr(info);

        // loop through items at grid location, look for gold
        int i = igrd[you.x_pos][you.y_pos];

        while(i != NON_ITEM)
        {
            if (mitm.base_type[i] == OBJ_GOLD)
            {
                mitm.quantity[i] += amount;
                you.gold -= amount;
                you.redraw_gold = 1;
                return;
            }
            // follow link
            i = mitm.link[i];
        }

        // place on top.
        for (i = 0; i < MAX_ITEMS; i++)
        {
            if (mitm.quantity[i] == 0)
            {
                mitm.id[i] = 0;
                mitm.base_type[i] = OBJ_GOLD;
                mitm.quantity[i] = amount;
                break;
            }
        }

        int m = igrd[you.x_pos][you.y_pos];

        igrd[you.x_pos][you.y_pos] = i;
        mitm.link[i] = m;

        you.gold -= amount;
        you.redraw_gold = 1;

    }
    else
    {
        mpr("You don't have any money.");
    }
}                               // end drop_gold()

// gets a quantity and item letter
// keyin should be the first key typed
// output: assigns -1 to quant_drop if no quant. specified.
static unsigned char get_item_quant(unsigned char keyin, int &quant_drop)
{
    quant_drop = -1;

    while(true)
    {
        if ((keyin >= 'a' && keyin <= 'z')
            || (keyin >= 'A' && keyin <= 'Z')
            || keyin == '$')
        {
            break;
        }

        if (keyin >= '0' && keyin <= '9')
        {
            if (quant_drop < 0)
                quant_drop = 0;
            else
                quant_drop *= 10;

            quant_drop += keyin - '0';
            // silliness
            if (quant_drop > 9999999)
                quant_drop = 9999999;
        }
        else
            break;

        keyin = get_ch();
    }

    return keyin;
}


//---------------------------------------------------------------
//
// drop
//
// Prompts the user for an item to drop
//
//---------------------------------------------------------------
void drop(void)
{
    unsigned char nthing;
    int i;
    unsigned char item_drop_1;
    unsigned char item_drop_2;
    char str_pass[80];

    if (inv_count() < 1 && you.gold == 0)
    {
        canned_msg(MSG_NOTHING_CARRIED);
        return;
    }

  query2:
    mpr("Drop which item? ", MSGCH_PROMPT);

    unsigned char keyin = get_ch();
    int quant_drop;

    if (keyin == '$')
    {
        // drop all gold
        drop_gold(you.gold);
        return;
    }

    if (keyin == '?' || keyin == '*')
    {
        nthing = get_invent(-1);
        if ((nthing >= 'A' && nthing <= 'Z')
            || (nthing >= 'a' && nthing <= 'z'))
        {
            keyin = nthing;
        }
        else
            goto query2;
    }

    item_drop_1 = get_item_quant(keyin, quant_drop);

    // do gold check again
    if (item_drop_1 == '$')
    {
        drop_gold(quant_drop);
        return;
    }

    if ((item_drop_1 < 'A' || (item_drop_1 > 'Z' && item_drop_1 < 'a')
         || item_drop_1 > 'z'))
    {
        mpr("You don't have any such object.");
        return;
    }

    if (quant_drop == 0)
    {
        canned_msg(MSG_OK);
        return;
    }

/*
    if (item_drop_1 >= '0' && item_drop_1 <= '9')
    {
        quant_drop = item_drop_1 - '0';
        putch(keyin);
        keyin = get_ch();
        item_drop_1 = (int) keyin;

        if (item_drop_1 >= '0' && item_drop_1 <= '9')
        {
            quant_drop = (10 * quant_drop + (item_drop_1 - '0'));
            putch(keyin);
            keyin = get_ch();
            item_drop_1 = (int) keyin;
        }
    }
*/

    item_drop_2 = letter_to_index(item_drop_1);

    if (you.inv_quantity[item_drop_2] == 0)
    {
        mpr("You don't have any such object.");
        return;
    }

    for (i = EQ_CLOAK; i <= EQ_BODY_ARMOUR; i++)
    {
        if (item_drop_2 == you.equip[i] && you.equip[i] != -1)
        {
            if (!Options.easy_armour)
            {
                mpr("You will have to take that off first.");
                return;
            }
            else if (!takeoff_armour(item_drop_2))
                return;
        }
    }

    if (item_drop_2 == you.equip[EQ_LEFT_RING]
        || item_drop_2 == you.equip[EQ_RIGHT_RING]
        || item_drop_2 == you.equip[EQ_AMULET])
    {
        mpr("You will have to take that off first.");
        return;
    }

    if (item_drop_2 == you.equip[EQ_WEAPON] && you.inv_class[item_drop_2] == 0
        && you.inv_plus[item_drop_2] >= 130)
    {
        mpr("That object is stuck to you!");
        return;
    }

    if (quant_drop < 0)
        quant_drop = you.inv_quantity[item_drop_2];

    if (quant_drop > you.inv_quantity[item_drop_2])
        quant_drop = you.inv_quantity[item_drop_2];

    // XXX dropping items seems to take zero time (if this changes
    // the time should be added to the un-wield/un-wear time)
    strcpy(info, "You drop ");
    in_quant_name( item_drop_2, quant_drop, 3, str_pass );
    strcat(info, str_pass);
    strcat(info, ".");
    mpr(info);

    if (item_drop_2 == you.equip[EQ_WEAPON])
    {
        unwield_item(item_drop_2);
        you.equip[EQ_WEAPON] = -1;
        mpr("You are empty-handed.");
    }

    item_place(item_drop_2, you.x_pos, you.y_pos, quant_drop);

    you.inv_quantity[item_drop_2] -= quant_drop;

    if (you.inv_quantity[item_drop_2] < 1)
        you.inv_quantity[item_drop_2] = 0;

    burden_change();
}                               // end drop()

//---------------------------------------------------------------
//
// update_corpses
//
// Update all of the corpses and food chunks on the floor. (The
// elapsed time is a double because this is called when we re-
// enter a level and a *long* time may have elapsed).
//
//---------------------------------------------------------------
void update_corpses(double elapsedTime)
{
    ASSERT(elapsedTime >= 0.0);

    if (elapsedTime > 0.0)
    {
        const int rot_time = (int) (elapsedTime / 20.0);

        for (int c = 0; c < MAX_ITEMS; c++)
        {
            if (mitm.quantity[c] < 1)
                continue;

            if (mitm.base_type[c] != OBJ_CORPSES
                && mitm.base_type[c] != OBJ_FOOD)
            {
                continue;
            }

            if (mitm.base_type[c] == OBJ_CORPSES
                && mitm.sub_type[c] > CORPSE_SKELETON)
            {
                continue;
            }

            if (mitm.base_type[c] == OBJ_FOOD
                && mitm.sub_type[c] != FOOD_CHUNK)
            {
                continue;
            }

            if (rot_time >= mitm.special[c])
            {
                if (mitm.base_type[c] == OBJ_FOOD)
                {
                    destroy_item(c);
                }
                else
                {
                    if (mitm.sub_type[c] == CORPSE_SKELETON
                        || !mons_skeleton(mitm.pluses[c]))
                    {
                        destroy_item(c);
                    }
                    else
                    {
                        mitm.sub_type[c] = CORPSE_SKELETON;
                        mitm.special[c] = 200;
                        mitm.colour[c] = LIGHTGREY;
                    }
                }
            }
            else
            {
                ASSERT(rot_time < 256);
                mitm.special[c] -= rot_time;
            }
        }
    }
}

//---------------------------------------------------------------
//
// handle_time
//
// Do various time related actions.
//
//---------------------------------------------------------------
void handle_time(int time_delta)
{
    int temp_rand;              // probability determination {dlb}

    // so as not to reduplicate f(x) calls {dlb}
    unsigned char which_miscast = SPTYP_RANDOM;

    bool summon_instead;        // for branching within a single switch {dlb}
    int which_beastie = MONS_PROGRAM_BUG;       // error trapping {dlb}
    unsigned char i;            // loop variable {dlb}

    // BEGIN - Nasty things happen to people who spend too long in Hell:
    if (you.where_are_you > BRANCH_MAIN_DUNGEON
        && you.where_are_you < BRANCH_ORCISH_MINES
        && you.where_are_you != BRANCH_VESTIBULE_OF_HELL && coinflip())
    {
        temp_rand = random2(17);

        mpr((temp_rand == 0) ? "\"You will not leave this place.\"" :
            (temp_rand == 1) ? "\"Die, mortal!\"" :
            (temp_rand == 2) ? "\"We do not forgive those who trespass against us!\"" :
            (temp_rand == 3) ? "\"Trespassers are not welcome here!\"" :
            (temp_rand == 4) ? "\"You do not belong in this place!\"" :
            (temp_rand == 5) ? "\"Leave now, before it is too late!\"" :
            (temp_rand == 6) ? "\"We have you now!\"" :
            (temp_rand == 7) ? "You feel a terrible foreboding..." :
            (temp_rand == 8) ? "You hear words spoken in a strange and terrible language..." :

            (temp_rand == 9) ? ((you.species != SP_MUMMY)
                    ? "You smell brimstone." : "Brimstone rains from above.") :

            (temp_rand == 10) ? "Something frightening happens." :
            (temp_rand == 11) ? "You sense an ancient evil watching you..." :
            (temp_rand == 12) ? "You feel lost and a long, long way from home..." :
            (temp_rand == 13) ? "You suddenly feel all small and vulnerable." :
            (temp_rand == 14) ? "A gut-wrenching scream fills the air!" :
            (temp_rand == 15) ? "You shiver with fear." :
            (temp_rand == 16) ? "You sense a hostile presence."
                              : "You hear diabolical laughter!");

        temp_rand = random2(27);

        if (temp_rand > 17)     // 9 in 27 odds {dlb}
        {
            temp_rand = random2(8);

            if (temp_rand > 3)  // 4 in 8 odds {dlb}
                which_miscast = SPTYP_NECROMANCY;
            else if (temp_rand > 1)     // 2 in 8 odds {dlb}
                which_miscast = SPTYP_SUMMONING;
            else if (temp_rand > 0)     // 1 in 8 odds {dlb}
                which_miscast = SPTYP_CONJURATION;
            else                // 1 in 8 odds {dlb}
                which_miscast = SPTYP_ENCHANTMENT;

            miscast_effect( which_miscast, 4 + random2(6), random2avg(97, 3),
                            100 );
        }
        else if (temp_rand > 7) // 10 in 27 odds {dlb}
        {
            // 60:40 miscast:summon split {dlb}
            summon_instead = (random2(5) > 2);

            switch (you.where_are_you)
            {
            case BRANCH_DIS:
                if (summon_instead)
                    which_beastie = summon_any_demon(DEMON_GREATER);
                else
                    which_miscast = SPTYP_EARTH;
                break;
            case BRANCH_GEHENNA:
                if (summon_instead)
                    which_beastie = MONS_FIEND;
                else
                    which_miscast = SPTYP_FIRE;
                break;
            case BRANCH_COCYTUS:
                if (summon_instead)
                    which_beastie = MONS_ICE_FIEND;
                else
                    which_miscast = SPTYP_ICE;
                break;
            case BRANCH_TARTARUS:
                if (summon_instead)
                    which_beastie = MONS_SHADOW_FIEND;
                else
                    which_miscast = SPTYP_NECROMANCY;
                break;
            default:        // this is to silence gcc compiler warnings {dlb}
                if (summon_instead)
                    which_beastie = MONS_FIEND;
                else
                    which_miscast = SPTYP_NECROMANCY;
                break;
            }

            if (summon_instead)
            {
                create_monster(which_beastie, 0, BEH_HOSTILE, you.x_pos,
                               you.y_pos, MHITYOU, 250);
            }
            else
            {
                miscast_effect(which_miscast, 4 + random2(6),
                               random2avg(97, 3), 100);
            }
        }

        // NB: no "else" - 8 in 27 odds that nothing happens through
        // first chain {dlb}
        // also note that the following is distinct from and in
        // addition to the above chain:

        // try to summon at least one and up to five random monsters {dlb}
        if (one_chance_in(3))
        {
            create_monster(RANDOM_MONSTER, 0, BEH_HOSTILE, you.x_pos,
                           you.y_pos, MHITYOU, 250);

            for (i = 0; i < 4; i++)
            {
                if (one_chance_in(3))
                {
                    create_monster(RANDOM_MONSTER, 0, BEH_HOSTILE,
                                   you.x_pos, you.y_pos, MHITYOU, 250);
                }
            }
        }
    }
    // END - special Hellish things...

    // Adjust the player's stats if s/he's diseased (or recovering).
    if (!you.disease)
    {
        if (you.strength < you.max_strength && one_chance_in(100))
        {
            mpr("You feel your strength returning.", MSGCH_RECOVERY);
            you.strength++;
            you.redraw_strength = 1;
        }

        if (you.dex < you.max_dex && one_chance_in(100))
        {
            mpr("You feel your dexterity returning.", MSGCH_RECOVERY);
            you.dex++;
            you.redraw_dexterity = 1;
        }

        if (you.intel < you.max_intel && one_chance_in(100))
        {
            mpr("You feel your intelligence returning.", MSGCH_RECOVERY);
            you.intel++;
            you.redraw_intelligence = 1;
        }
    }
    else
    {
        if (one_chance_in(30))
        {
            mpr("Your disease is taking its toll.", MSGCH_WARN);
            lose_stat(STAT_RANDOM, 1);
        }
    }

    // Adjust the player's stats if s/he has the deterioration mutation
    if (you.mutation[MUT_DETERIORATION]
        && random2(200) <= you.mutation[MUT_DETERIORATION] * 5 - 2)
    {
        lose_stat(STAT_RANDOM, 1);
    }

    // Account for mutagenic radiation.  Invis and haste will give the
    // player about .1 points per turn,  mutagenic randarts will give
    // about 1.5 points on average,  so they can corrupt the player
    // quite quickly.  Wielding one for a short battle is OK,  which is
    // as things should be.   -- GDL
    if (you.invis && random2(10) < 6)
        you.magic_contamination ++;

    if (you.haste && !you.berserker && random2(10) < 6)
        you.magic_contamination++;

    // randarts are usually about 20x worse than running around invisible
    // or hasted.. this seems OK.
    you.magic_contamination += random2(1 + scan_randarts(RAP_MUTAGENIC));

    if (you.magic_contamination > 0 && coinflip())
    {
        if (you.magic_contamination > 4
            && random2(150) <= you.magic_contamination)
        {
            mpr("Your body shudders with the violent release of wild energies!", MSGCH_WARN);

            // for particularly violent releases,  make a little boom
            if (you.magic_contamination > 25)
            {
                struct bolt boom;
                boom.type = SYM_BURST;
                boom.colour = random_colour();
                boom.flavour = BEAM_FIRE + random2(7);
                boom.target_x = you.x_pos;
                boom.target_y = you.y_pos;
                boom.damage = 100 + (you.magic_contamination / 2);
                boom.thrower = KILL_YOU;
                boom.ex_size = (you.magic_contamination / 15);
                boom.ench_power = (you.magic_contamination * 5);
                boom.beam_source = NON_MONSTER;
                boom.isBeam = false;
                boom.isTracer = false;
                strcpy(boom.beam_name, "magical storm");
                explosion(boom);
            }

            // we want to warp the player,  not do good stuff!
            if (one_chance_in(4))
                mutate(100);
            else
                give_bad_mutation();

            // we're meaner now,  what with explosions and whatnot,  but
            // we dial down the contamination a little faster if its actually
            // mutating you.  -- GDL
            you.magic_contamination -= (random2(you.magic_contamination / 4) + 1);
        }
        else
            you.magic_contamination--;
    }

    // Random chance to identify staff in hand based off of Spellcasting
    // and an appropriate other spell skill... is 1/20 too fast?
    if (you.equip[EQ_WEAPON] != -1
        && you.inv_class[you.equip[EQ_WEAPON]] == OBJ_STAVES
        && you.inv_ident[you.equip[EQ_WEAPON]] == 0 && one_chance_in(20))
    {
        int total_skill = you.skills[SK_SPELLCASTING];

        switch (you.inv_type[you.equip[EQ_WEAPON]])
        {
        case STAFF_WIZARDRY:
        case STAFF_ENERGY:
            total_skill += you.skills[SK_SPELLCASTING];
            break;
        case STAFF_FIRE:
            if (you.skills[SK_FIRE_MAGIC] > you.skills[SK_ICE_MAGIC])
                total_skill += you.skills[SK_FIRE_MAGIC];
            else
                total_skill += you.skills[SK_ICE_MAGIC];
            break;
        case STAFF_COLD:
            if (you.skills[SK_ICE_MAGIC] > you.skills[SK_FIRE_MAGIC])
                total_skill += you.skills[SK_ICE_MAGIC];
            else
                total_skill += you.skills[SK_FIRE_MAGIC];
            break;
        case STAFF_AIR:
            if (you.skills[SK_AIR_MAGIC] > you.skills[SK_EARTH_MAGIC])
                total_skill += you.skills[SK_AIR_MAGIC];
            else
                total_skill += you.skills[SK_EARTH_MAGIC];
            break;
        case STAFF_EARTH:
            if (you.skills[SK_EARTH_MAGIC] > you.skills[SK_AIR_MAGIC])
                total_skill += you.skills[SK_EARTH_MAGIC];
            else
                total_skill += you.skills[SK_AIR_MAGIC];
            break;
        case STAFF_POISON:
            total_skill += you.skills[SK_POISON_MAGIC];
            break;
        case STAFF_DEATH:
            total_skill += you.skills[SK_NECROMANCY];
            break;
        case STAFF_CONJURATION:
            total_skill += you.skills[SK_CONJURATIONS];
            break;
        case STAFF_ENCHANTMENT:
            total_skill += you.skills[SK_ENCHANTMENTS];
            break;
        case STAFF_SUMMONING_I:
            total_skill += you.skills[SK_SUMMONINGS];
            break;
        }

        if (random2(100) < total_skill)
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

    // Check to see if an upset god wants to do something to the player
    // jmf: moved huge thing to religion.cc
    handle_god_time();

    // If the player has the lost mutation forget portions of the map
    if (you.mutation[MUT_LOST])
    {
        if (random2(100) <= you.mutation[MUT_LOST] * 5)
            forget_map(5 + random2(you.mutation[MUT_LOST] * 10));
    }

    // Update all of the corpses and food chunks on the floor
    update_corpses(time_delta);

    // Update all of the corpses and food chunks in the player's
    // inventory {should be moved elsewhere - dlb}
    for (i = 0; i < ENDOFPACK; i++)
    {
        if (you.inv_quantity[i] < 1)
            continue;

        if (you.inv_class[i] != OBJ_CORPSES && you.inv_class[i] != OBJ_FOOD)
            continue;

        if (you.inv_class[i] == OBJ_CORPSES
            && you.inv_type[i] > CORPSE_SKELETON)
        {
            continue;
        }

        if (you.inv_class[i] == OBJ_FOOD && you.inv_type[i] != FOOD_CHUNK)
            continue;

        if ((time_delta / 20) >= you.inv_dam[i])
        {
            if (you.inv_class[i] == OBJ_FOOD)
            {
                if (you.equip[EQ_WEAPON] == i)
                {
                    unwield_item(you.equip[EQ_WEAPON]);
                    you.equip[EQ_WEAPON] = -1;
                    wield_change = true;
                }

                you.inv_quantity[i] = 0;
                burden_change();
                continue;
            }

            if (you.inv_type[i] == CORPSE_SKELETON)
                continue;       // carried skeletons are not destroyed

            if (!mons_skeleton(you.inv_plus[i]))
            {
                if (you.equip[EQ_WEAPON] == i)
                {
                    unwield_item(you.equip[EQ_WEAPON]);
                    you.equip[EQ_WEAPON] = -1;
                }

                you.inv_quantity[i] = 0;
                burden_change();
                continue;
            }

            you.inv_type[i] = 1;
            you.inv_dam[i] = 0;
            you.inv_colour[i] = LIGHTGREY;
            wield_change = true;
            continue;
        }

        you.inv_dam[i] -= (time_delta / 20);
    }

    // exercise armor *xor* stealth skill: {dlb}
    if (!player_light_armour())
    {
        if (random2(1000)
                <= mass(OBJ_ARMOUR, you.inv_type[you.equip[EQ_BODY_ARMOUR]]))
        {
            return;
        }

        if (one_chance_in(6))   // lowered random roll from 7 to 6 -- bwross
            exercise(SK_ARMOUR, 1);
    }
    else                        // exercise stealth skill:
    {
        if (you.burden_state != BS_UNENCUMBERED || you.berserker)
            return;

        if (you.special_wield == SPWLD_SHADOW)
            return;

        //if ( you.levitation ) return;    // can't really practise stealth while floating - amulet of control flight shouldn't matter, either

        if (you.equip[EQ_BODY_ARMOUR] != -1
            && random2(mass(2, you.inv_type[you.equip[EQ_BODY_ARMOUR]])) >= 100)
        {
            return;
        }

        if (one_chance_in(18))
            exercise(SK_STEALTH, 1);
    }

    return;
}                               // end handle_time()

int autopickup_on = 1;

void autopickup(void)
{
    //David Loewenstern 6/99
    int items_here = 0;
    int result, o, hrg;
    bool did_pickup = false;

    if (autopickup_on == 0 || Options.autopickups == 0L)
        return;

    if (you.levitation && !wearing_amulet(AMU_CONTROLLED_FLIGHT))
        return;

    o = igrd[you.x_pos][you.y_pos];
    if (o == NON_ITEM)          //no objs
        return;

    last_item = NON_ITEM;

    while (o != NON_ITEM)
    {
        items_here++;
        if (Options.autopickups & (1L << mitm.base_type[o]))
        {
            result = add_item(o, mitm.quantity[o]);
            if (result == 0)
            {
                mpr("You can't carry any more.");
                return;
            }

            if (result == NON_ITEM)
            {
                mpr("Your pack is full.");
                return;
            }

            if (result != 1)    //item still there?
                last_item = o;

            did_pickup = true;
        }

        if (items_here > 1000)
        {
            error_message_to_player();
            return;
        }

        if (mitm.quantity[o] > 0)
            last_item = o;

        hrg = mitm.link[o];

        if (mitm.quantity[o] == 0)
            mitm.link[o] = NON_ITEM;

        o = hrg;
    }

    if (did_pickup && you.delay_t == 0)
    {
        you.delay_t = 3;
        you.delay_doing = DELAY_AUTOPICKUP;
    }
}

int inv_count(void)
{
    int count=0;

    for(int i=0; i< ENDOFPACK; i++)
    {
        if (you.inv_quantity[i] > 0)
            count += 1;
    }

    return count;
}


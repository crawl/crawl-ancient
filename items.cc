/*
 *  File:       items.cc
 *  Summary:    Misc (mostly) inventory related functions.
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 *      <8>     8/07/99         BWR             Added Rune stacking
 *      <7>     6/13/99         BWR             Added auto staff detection
 *      <6>     6/12/99         BWR             Fixed time system.
 *      <5>     6/9/99          DML             Autopickup
 *      <4>     5/26/99         JDJ             Drop will attempt to take off armor.
 *      <3>     5/21/99         BWR             Upped armour skill learning slightly.
 *      <2>     5/20/99         BWR             Added assurance that against inventory count being wrong.
 *      <1>     -/--/--         LRH             Created
 */

#include "AppHdr.h"
#include "items.h"

#ifdef DOS
#include <conio.h>
#endif

#include <string.h>
#include <stdlib.h>

#include "debug.h"
#include "externs.h"
#include "enum.h"

#include "effects.h"
#include "fight.h"
#include "invent.h"
#include "itemname.h"
#include "item_use.h"
#include "it_use2.h"
#include "misc.h"
#include "monplace.h"
#include "mstruct.h"
#include "mutation.h"
#include "player.h"
#include "randart.h"
#include "religion.h"
#include "skills.h"
#include "spells.h"
#include "stuff.h"

char mutate(int which_mutation);
void autopickup(void);


int add_item(int item_got, int it_quant);
void item_place(int item_drop_2, int x_plos, int y_plos, int quant_drop);
int conv_lett(int item_drop_3);


int last_item = ING;

extern unsigned char wield_change;      /* defined in output.cc */

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

    if (env.grid[you.x_pos][you.y_pos] >= 69 && env.grid[you.x_pos][you.y_pos] <= 210)
    {
        if (env.grid[you.x_pos][you.y_pos] >= 82 && env.grid[you.x_pos][you.y_pos] <= 85)
        {
            if (env.grid[you.x_pos][you.y_pos] == 85)
                mpr("There is a rock staircase leading down here.");
            else
                mpr("There is a stone staircase down here.");
        }
        else if (env.grid[you.x_pos][you.y_pos] >= 86 && env.grid[you.x_pos][you.y_pos] <= 89)
        {
            if (env.grid[you.x_pos][you.y_pos] == 89)
                mpr("There is a rock staircase leading upwards here.");
            else
                mpr("There is a stone staircase up here.");
        }
        else
            switch (env.grid[you.x_pos][you.y_pos])
            {
            case 69:
                mpr("There is a gateway to hell here.");
                break;
            case 80:
                mpr("There is an entrance to a shop here.");
                break;
            case 81:
                mpr("There is an entrance to a labyrinth here.");
                mpr("Beware, for starvation awaits!");
                break;
            case 92:
                mpr("There is a gateway to the Iron City of Dis here.");
                break;
            case 93:
                mpr("There is a gateway to Gehenna here.");
                break;
            case 94:
                mpr("There is a gateway to the frozen wastes of Cocytus here.");
                break;
            case 95:
                mpr("There is a gateway to Tartarus here.");
                break;
            case 96:
                mpr("There is a one-way gate to the infinite horrors of the Abyss here.");
                break;
            case 97:
                mpr("There is a gateway leading out of the Abyss here.");
                break;
            case 98:
                mpr("There is an empty stone archway here.");
                break;
            case 99:
                mpr("There is a gate leading to the halls of Pandemonium here.");
                break;
            case 100:
                mpr("There is a gate leading out of Pandemonium here.");
                break;
            case 101:
                mpr("There is a gate leading to another region of Pandemonium here.");
                break;

            case 110:
                mpr("There is a staircase to the orcish mines here.");
                break;
            case 111:
                mpr("There is a staircase to the Hive here.");
                break;
            case 112:
                mpr("There is a staircase to the Lair here.");
                break;
            case 113:
                mpr("There is a staircase to the Slime Pits here.");
                break;
            case 114:
                mpr("There is a staircase to the Vaults here.");
                break;
            case 115:
                mpr("There is a staircase to the Crypt here.");
                break;
            case 116:
                mpr("There is a staircase to the Hall of Blades here.");
                break;
            case 117:
                mpr("There is a gate to the Realm of Zot here.");
                break;
            case 118:
                mpr("There is a staircase to the Ecumenical Temple here.");
                break;
            case 119:
                mpr("There is a staircase to the Snake Pit here.");
                break;
            case 120:
                mpr("There is a staircase to the Elven Halls here.");
                break;
            case 121:
                mpr("There is a staircase to the Tomb here.");
                break;
            case 122:
                mpr("There is a staircase to the Swamp here.");
                break;
            case 130:
            case 131:
            case 134:
            case 138:
            case 132:
                mpr("There is a staircase back to the Dungeon here.");
                break;
            case 142:
                mpr("There is a staircase back to the Lair here.");
                break;
            case 133:
                mpr("There is a staircase back to the Lair here.");
                break;
            case 135:
                mpr("There is a staircase back to the Vaults here.");
                break;
            case 141:
            case 136:
                mpr("There is a staircase back to the Crypt here.");
                break;
            case 139:
                mpr("There is a staircase back to the Lair here.");
                break;
            case 140:
                mpr("There is a staircase back to the Mines here.");
                break;
            case 137:
                mpr("There is a gate leading back out of this place here.");
                break;

            case 180:
                mpr("There is a glowing white marble altar of Zin here.");
                break;
            case 181:
                mpr("There is a glowing golden altar of the Shining One here.");
                break;
            case 182:
                mpr("There is an ancient bone altar of Kikubaaqudgha here.");
                break;
            case 183:
                mpr("There is a basalt altar of Yredelemnul here.");
                break;
            case 184:
                mpr("There is a shimmering altar of Xom here.");
                break;
            case 185:
                mpr("There is a shining altar of Vehumet here.");
                break;
            case 186:
                mpr("There is an iron altar of Okawaru here.");
                break;
            case 187:
                mpr("There is a burning altar of Makhleb here.");
                break;
            case 188:
                mpr("There is a deep blue altar of Sif Muna here.");
                break;
            case 189:
                mpr("There is a bloodstained altar of Trog here.");
                break;
            case 190:
                mpr("There is a sparkling altar of Nemelex Xobeh here.");
                break;
            case 191:
                mpr("There is a silver altar of Elyvilon here.");
                break;

            case 200:
                mpr("There is a fountain here (q to drink).");
                break;
            case 202:
                mpr("There is a sparkling fountain here (q to drink).");
                break;
            case 201:
            case 203:
            case 205:
            case 207:
            case 209:
            case 210:
                mpr("There is a dry fountain here.");
                break;

            }
    }

    if (env.igrid[you.x_pos][you.y_pos] == ING)
    {
        if (keyin == ';')
            mpr("There are no items here.");
        return;
    }

    autopickup();
    int objl = env.igrid[you.x_pos][you.y_pos];
    int hrg = 0;

    while (objl != 501)
    {
        counter++;

        if (counter > 45)
        {
            strcpy(item_show[counter], "Too many items.");
            break;
        }

        if (mitm.base_type[objl] == 15)
        {
            itoa(mitm.quantity[objl], temp_quant, 10);
            strcpy(item_show[counter], temp_quant);
            if (mitm.quantity[objl] > 1)
                strcat(item_show[counter], " gold pieces");
            else
                strcat(item_show[counter], " gold piece");
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

    if ((counter_max > 0 && counter_max < 6) || (counter_max > 1 && keyin == 59))
    {
        mpr("Things that are here:");
        while (counter < counter_max)
        {
            counter++;          // this is before the strcpy because item_show start at 1, not 0.

            mpr(item_show[counter]);
        }
    }

    if (counter_max > 5 && keyin != 59)
    {
        mpr("There are several objects here.");
    }
}


void pickup()
{
    int items_here = 0;
    int counter = 0;

    int item_got = 0;
    int o = 0;
    int k = 0;
    int m = 0;
    int nothing = 0;
    char str_pass[50];
    char keyin = 0;

    if (you.levitation != 0 && wearing_amulet(AMU_CONTROLLED_FLIGHT) == 0)
    {
        mpr("You can't reach the floor from up here.");
        return;
    }

    if (grd[you.x_pos][you.y_pos] == 190 && you.where_are_you != 18)
    {
        if (you.num_inv_items >= 52)
        {
            mpr("There is a portable altar here, but you can't carry anything else.");
            return;
        }

        mpr("There is a portable altar here. Pick it up?");
        keyin = get_ch();
        if (keyin == 'y' || keyin == 'Y')
        {
            for (m = 0; m < 52; m++)
            {
                if (you.inv_quantity[m] == 0)
                {
                    you.inv_ident[m] = 3;
                    you.inv_class[m] = 13;
                    you.inv_type[m] = 17;
                    you.inv_plus[m] = 0;
                    you.inv_plus2[m] = 0;
                    you.inv_dam[m] = 0;
                    you.inv_colour[m] = LIGHTMAGENTA;
                    you.inv_quantity[m] = 1;
                    you.num_inv_items++;
                    burden_change();
                    if (m <= 25)
                        info[0] = m + 97;
                    else
                        info[0] = m + 39;
                    info[1] = 0;
                    strcat(info, " - ");
                    in_name(m, 3, str_pass);
                    strcat(info, str_pass);
                    mpr(info);
                    break;
                }
            }

            // This is here to catch things when the count gets out of sync.
            if (m == 52)
            {
                ASSERT(you.num_inv_items == 52);
                mpr("You can't carry anything else.");
                you.num_inv_items = 52;
                return;
            }

            grd[you.x_pos][you.y_pos] = 67;
        }
    }

    if (env.igrid[you.x_pos][you.y_pos] == ING)
    {
        mpr("There are no items here.");
        return;
    }

    last_item = ING;

    int objl = env.igrid[you.x_pos][you.y_pos];
    int hrg = 0;

    while (objl != ING)
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
        item_got = env.igrid[you.x_pos][you.y_pos];
        last_item = ING;
        nothing = add_item(item_got, mitm.quantity[env.igrid[you.x_pos][you.y_pos]]);
        if (nothing == ING)
        {
            mpr("You can't carry that many items.");
        }
        else if (nothing != 1)
        {
            mpr("You can't carry that much weight.");
        }
        return;

    }                           // end of if items_here

    last_item = ING;

    if (items_here > 1)
    {
        mpr("There are several objects here.");

        o = env.igrid[you.x_pos][you.y_pos];

        for (k = 0; k < items_here; k++)
        {
            if (keyin != 'a')
            {
                strcpy(info, "pick up ");

                if (mitm.base_type[o] == 15)
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
            {
                return;
            }
            if (keyin == 'y' || keyin == 'a')
            {
                item_got = o;
                int grunk = add_item(o, mitm.quantity[o]);

                if (grunk == 0)
                {
                    mpr("You can't carry that much weight.");
                    keyin = 'x';        // resets from 'a'

                }
                if (grunk == ING)
                {
                    mpr("You can't carry that many items.");
                    keyin = 'x';        // resets from 'a'

                }


                if (grunk != 1) // ie if the item picked up is still there.

                {
                    last_item = item_got;
                }

            }

            if (mitm.quantity[o] > 0)
                last_item = o;

            hrg = mitm.link[o];

            if (mitm.quantity[o] == 0)
                mitm.link[o] = ING;

            o = hrg;
            if (o == ING)
                return;

            if (items_here == 0)
                break;
        }                       // end of while k loop

        mpr("That's all.");
    }                           // end of if items_here

}                               // end of pickup() function


int add_item(int item_got, int quant_got)
{
/*int quant_got = 0; */
    int item_mass = 0;
    int unit_mass = 0;
    int retval = 1;
    char brek = 0;

//int last_item = ING;
    int m = 0;
    char str_pass[50];

    if (you.num_inv_items >= 52)
    {
        return ING;
    }

    if (mitm.base_type[item_got] <= OBJ_ARMOUR
                || mitm.base_type[item_got] == OBJ_FOOD
                || mitm.base_type[item_got] == OBJ_MISCELLANY)
    {
        unit_mass = mass(mitm.base_type[item_got], mitm.sub_type[item_got]);
    }
    else
    {
        switch (mitm.base_type[item_got])
        {
        case OBJ_WANDS:
            unit_mass = 100;
            break;
        case 5:
            unit_mass = 200;
            break;
        case OBJ_SCROLLS:
            unit_mass = 50;
            break;
        case OBJ_JEWELLERY:
            unit_mass = 20;
            break;
        case OBJ_POTIONS:
            unit_mass = 60;
            break;
        case 9:
            unit_mass = 5;
            break;
        case OBJ_BOOKS:
            unit_mass = 250;
            break;
        case OBJ_STAVES:
            unit_mass = 130;
            break;
        case OBJ_CORPSES:
            if (mitm.sub_type[item_got] == 0)
                unit_mass = mons_weight(mitm.pluses[item_got]);
            if (mitm.sub_type[item_got] == 1)
                unit_mass = mons_weight(mitm.pluses[item_got]) / 2;
            break;
        case OBJ_GOLD:
            unit_mass = 0;
            break;              // For now!

        }
    }

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
                mpr("You can only carry some of what is here.");
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
        strcat(info, " gold pieces.");
        mpr(info);
        you.turn_is_over = 1;
        alert();
        goto change_igrid;
    }

    for (m = 0; m < 52; m++)
    {

        if ((mitm.base_type[item_got] == OBJ_MISSILES
                || (mitm.base_type[item_got] == OBJ_FOOD
                    && mitm.sub_type[item_got] != FOOD_CHUNK)
                || mitm.base_type[item_got] == OBJ_SCROLLS
                || mitm.base_type[item_got] == OBJ_POTIONS
                || (mitm.base_type[item_got] == OBJ_MISCELLANY
                    && mitm.sub_type[item_got] == MISC_RUNE_OF_ZOT)
                || mitm.base_type[item_got] == 9)

            && you.inv_class[m] == mitm.base_type[item_got]
            && you.inv_type[m] == mitm.sub_type[item_got]

            && (((mitm.base_type[item_got] == OBJ_FOOD
                            && mitm.sub_type[item_got] != FOOD_CHUNK)
                    || mitm.base_type[item_got] == OBJ_SCROLLS
                    || mitm.base_type[item_got] == OBJ_POTIONS)
                || (you.inv_plus[m] == mitm.pluses[item_got]
                    && you.inv_plus2[m] == mitm.pluses2[item_got]
                    && you.inv_dam[m] == mitm.special[item_got]))

            && you.inv_quantity[m] > 0)
        {
            if (mitm.id[item_got] == you.inv_ident[m]
                    || mitm.base_type[item_got] == OBJ_FOOD
                    || mitm.base_type[item_got] == OBJ_SCROLLS
                    || mitm.base_type[item_got] == OBJ_POTIONS)
            {
                you.inv_quantity[m] += quant_got;
                burden_change();

                if (m <= 25)
                    info[0] = m + 97;
                else
                    info[0] = m + 39;

                info[1] = 0;

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

    for (m = 0; m < 52; m++)
    {
        if (you.inv_quantity[m] == 0)
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

//                      strcpy(info, " ");
            /*                      strncpy(info, letters [m], 1); */
            if (m <= 25)
                info[0] = m + 97;
            else
                info[0] = m + 39;

            info[1] = 0;

            strcat(info, " - ");
            in_name(m, 3, str_pass);
            strcat(info, str_pass);

            mpr(info);

            if (mitm.base_type[item_got] == 12 && you.char_direction == 0)
            {
                mpr("Now all you have to do is get back out of the dungeon!");
                you.char_direction = 1;
            }

            you.num_inv_items++;
            break;
        }
    }                           // end of for m loopy thing.

    // This is here to catch when the count gets out of sync.
    if (m == 52)
    {
        ASSERT(you.num_inv_items == 52);
        you.num_inv_items = 52;
        return (ING);
    }


    you.turn_is_over = 1;

change_igrid:
    mitm.quantity[item_got] -= quant_got;       //= 0;

    if (mitm.quantity[item_got] == 0)
    {
        if (last_item == ING)
        {
            env.igrid[you.x_pos][you.y_pos] = mitm.link[item_got];
        }
        else
        {
            mitm.link[last_item] = mitm.link[item_got];
        }
    }

    return retval;
}                               // end of int add_item() function


void item_place(int item_drop_2, int x_plos, int y_plos, int quant_drop)
{
    int item_mass = 0;
    int unit_mass = 0;

    if (you.inv_class[item_drop_2] < 3 || you.inv_class[item_drop_2] == 4 || you.inv_class[item_drop_2] == 13)
    {
        unit_mass = mass(you.inv_class[item_drop_2], you.inv_type[item_drop_2]);
    }
    else
    {
        switch (you.inv_class[item_drop_2])
        {
        case 3:
            unit_mass = 100;
            break;
        case 4:
            unit_mass = 100;
            break;
        case 5:
            unit_mass = 200;
            break;
        case 6:
            unit_mass = 50;
            break;
        case 7:
            unit_mass = 20;
            break;
        case 8:
            unit_mass = 60;
            break;
        case 9:
            unit_mass = 5;
            break;
        case 10:
            unit_mass = 250;
            break;
        case 11:
            unit_mass = 130;
            break;
        case 13:
            unit_mass = 250;
            break;
        case 14:
/* if (mitm.sub_type [item_got] == 0) unit_mass = mons_weight(mitm.pluses [item_got]);
   if (mitm.sub_type [item_got] == 1) unit_mass = mons_weight(mitm.pluses [item_got]) / 2; */
            if (mitm.sub_type[item_drop_2] == 0)
                unit_mass = mons_weight(mitm.pluses[item_drop_2]);
            if (mitm.sub_type[item_drop_2] == 1)
                unit_mass = mons_weight(mitm.pluses[item_drop_2]) / 2;
            break;
        }
    }

    item_mass = unit_mass * quant_drop;

    int m = 0, i = 0;

    if (igrd[x_plos][y_plos] != 501)
    {
        if ((you.inv_class[item_drop_2] == 1 || you.inv_class[item_drop_2] == 4 || you.inv_class[item_drop_2] == 6 || you.inv_class[item_drop_2] == 8 || you.inv_class[item_drop_2] == 9) && you.inv_class[item_drop_2] == mitm.base_type[igrd[x_plos][y_plos]] && you.inv_type[item_drop_2] == mitm.sub_type[igrd[x_plos][y_plos]] && you.inv_plus[item_drop_2] == mitm.pluses[igrd[x_plos][y_plos]] && you.inv_plus2[item_drop_2] == mitm.pluses2[igrd[x_plos][y_plos]] && you.inv_dam[item_drop_2] == mitm.special[igrd[x_plos][y_plos]] && mitm.quantity[igrd[x_plos][y_plos]] > 0)
        {
            if (you.inv_ident[item_drop_2] == mitm.id[igrd[x_plos][y_plos]])
            {
                mitm.quantity[igrd[x_plos][y_plos]] += quant_drop;
                you.turn_is_over = 1;
                return;
            }
        }
    }

    for (i = 0; i < ITEMS; i++)
    {
        if (i >= 480)
        {
            strcpy(info, "The demon of the infinite void grins at you.");
            mpr(info);
            return;
        }
        if (mitm.quantity[i] == 0)
        {
            mitm.id[i] = you.inv_ident[item_drop_2];
            mitm.base_type[i] = you.inv_class[item_drop_2];
            mitm.sub_type[i] = you.inv_type[item_drop_2];
            mitm.pluses[i] = you.inv_plus[item_drop_2];
            mitm.pluses2[i] = you.inv_plus2[item_drop_2];
            mitm.special[i] = you.inv_dam[item_drop_2];
            mitm.colour[i] = you.inv_colour[item_drop_2];
            mitm.quantity[i] = quant_drop;
/*              it_no ++; */
            break;
        }
    }

    m = igrd[x_plos][y_plos];

    igrd[x_plos][y_plos] = i;
    mitm.link[i] = m;

    you.turn_is_over = 1;
}                               // end of item_place


//---------------------------------------------------------------
//
// drop_gold
//
//---------------------------------------------------------------
static void drop_gold()
{
    if (you.gold > 0)
    {
        int quant_drop = you.gold;      /* needs quantity selection. */

        char temp_quant[10];

        strcpy(info, "You drop ");
        itoa(you.gold, temp_quant, 10);
        strcat(info, temp_quant);

        if (you.gold > 1)
            strcat(info, " gold pieces.");
        else
            strcat(info, " gold piece.");

        mpr(info);

        if (igrd[you.x_pos][you.y_pos] != 501)
        {
            if (mitm.base_type[igrd[you.x_pos][you.y_pos]] == 15)
            {
                mitm.quantity[igrd[you.x_pos][you.y_pos]] += quant_drop;
                you.gold -= quant_drop;
                you.
                    redraw_gold = 1;
                return;
            }
        }

        int i;

        for (i = 0; i < ITEMS; i++)
        {
            if (mitm.quantity[i] == 0)
            {
                mitm.id[i] = 0;
                mitm.base_type[i] = 15;
                mitm.quantity[i] = quant_drop;
                break;
            }
        }

        int m = igrd[you.x_pos][you.y_pos];

        igrd[you.x_pos][you.y_pos] = i;
        mitm.link[i] = m;

        you.gold -= quant_drop;
        you.redraw_gold = 1;

    }
    else
        mpr("You don't have any money.");
}


//---------------------------------------------------------------
//
// drop
//
// Prompts the user for an item to drop
//
//---------------------------------------------------------------
void drop()
{
    unsigned char nthing;
    int i;
    unsigned char item_drop_1;
    unsigned char item_drop_2;
    char str_pass[80];

    if (you.num_inv_items == 0)
    {
        mpr("You aren't carrying anything.");
        return;
    }

  query2:
    mpr("Drop which item? ");

    unsigned char keyin = get_ch();
    int quant_drop;

    if (keyin == '$')
    {
        drop_gold();

        return;
    }

    if (keyin == '?' || keyin == '*')
    {
        nthing = get_invent(-1);
        if ((nthing >= 65 && nthing <= 90) || (nthing >= 97 && nthing <= 122))
            keyin = nthing;
        else
            goto query2;
    }
    item_drop_1 = (int) keyin;

    quant_drop = 0;

    if (item_drop_1 > 47 && item_drop_1 < 58)
    {
        quant_drop = item_drop_1 - 48;
        putch(keyin);
        keyin = get_ch();
        item_drop_1 = (int) keyin;
        if (item_drop_1 > 47 && item_drop_1 < 58)
        {
            quant_drop = (quant_drop * 10 + (item_drop_1 - 48));
            putch(keyin);
            keyin = get_ch();
            item_drop_1 = (int) keyin;
        }
    }

    if ((item_drop_1 < 65 || (item_drop_1 > 90 && item_drop_1 < 97) || item_drop_1 > 122))
    {
        mpr("You don't have any such object.");
        return;
    }

    item_drop_2 = conv_lett(item_drop_1);
    if (quant_drop == 0)
        quant_drop = you.inv_quantity[item_drop_2];

    if (you.inv_quantity[item_drop_2] == 0)
    {
        strcpy(info, "You don't have any such object.");
        mpr(info);
        return;
    }

    for (i = EQ_CLOAK; i <= EQ_BODY_ARMOUR; i++)
    {
        if (item_drop_2 == you.equip[i] && you.equip[i] != -1)
        {
            if (!takeoff_armour(item_drop_2))
                return;
        }
    }

    if (item_drop_2 == you.equip[EQ_LEFT_RING] || item_drop_2 == you.equip[EQ_RIGHT_RING] || item_drop_2 == you.equip[EQ_AMULET])
    {
        mpr("You will have to take that off first.");
        return;
    }

    if (item_drop_2 == you.equip[EQ_WEAPON] && you.inv_class[item_drop_2] == 0 && you.inv_plus[item_drop_2] >= 130)
    {
        mpr("That object is stuck to you!");
        return;
    }

    if (quant_drop > you.inv_quantity[item_drop_2])
        quant_drop = you.inv_quantity[item_drop_2];

    strcpy(info, "You drop ");  // $$$ dropping items seems to take zero time (if this changes the time should be added to the un-wield/un-wear time)

    item_name(you.inv_plus2[item_drop_2], you.inv_class[item_drop_2], you.inv_type[item_drop_2], you.inv_dam[item_drop_2], you.inv_plus[item_drop_2], quant_drop, you.inv_ident[item_drop_2], 3, str_pass);
    strcat(info, str_pass);

    strcat(info, ".");
    mpr(info);

    if (item_drop_2 == you.equip[EQ_WEAPON])
    {
        unwield_item(item_drop_2);
        you.equip[EQ_WEAPON] = -1;
        mpr("You are empty handed.");
    }

    item_place(item_drop_2, you.x_pos, you.y_pos, quant_drop);

    you.inv_quantity[item_drop_2] -= quant_drop;
    if (you.inv_quantity[item_drop_2] <= 0)
        you.num_inv_items--;

    burden_change();
}                               // end of drop


//---------------------------------------------------------------
//
// conv_lett
//
//---------------------------------------------------------------
int conv_lett(int item_drop_3)
{
    if (item_drop_3 > 96 && item_drop_3 < 123)
    {
        item_drop_3 -= 97;
    }

    if (item_drop_3 > 64 && item_drop_3 < 91)
    {
        item_drop_3 -= 39;
    }

    return item_drop_3;
}


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

        for (int c = 0; c < ITEMS; c++)
        {
            if (mitm.quantity[c] <= 0)
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

            if (mitm.base_type[c] == OBJ_FOOD && mitm.sub_type[c] != FOOD_CHUNK)
                continue;

            if (rot_time >= mitm.special[c])
            {
                if (mitm.base_type[c] == OBJ_FOOD)
                {
                    destroy_item(c);

                }
                else
                {
                    if (mitm.sub_type[c] == CORPSE_SKELETON
                                    || mons_skeleton(mitm.pluses[c]) == 0)
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
    // Nasty things happen to people who spend too long in Hell
    if (you.where_are_you > 0 && you.where_are_you < 10 && you.where_are_you != 3 && random2(2) == 0)
    {
        switch (random2(17))
        {
        case 0:
            mpr("You hear diabolical laughter!");
            break;
        case 1:
            set_colour(RED);
            mpr("\"Die, mortal!\"");
            break;
        case 2:
            set_colour(RED);
            mpr("\"Trespassers are not welcome here!\"");
            break;
        case 3:
            set_colour(RED);
            mpr("\"You do not belong in this place!\"");
            break;
        case 4:
            mpr("You feel a terrible foreboding...");
            break;
        case 5:
            mpr("You hear words spoken in a strange and terrible language...");
            break;
        case 6:
            mpr("You smell brimstone.");
            break;
        case 7:
            mpr("Something frightening happens.");
            break;
        case 8:
            mpr("You sense an ancient evil watching you...");
            break;
        case 9:
            mpr("You feel lost and a long, long way from home...");
            break;
        case 10:
            mpr("You suddenly feel all small and vulnerable.");
            break;
        case 11:
            mpr("A gut-wrenching scream fills the air!");
            break;
        case 12:
            mpr("You shiver with fear.");
            break;
        case 13:
            mpr("You sense a hostile presence.");
            break;
        case 14:
            set_colour(RED);
            mpr("\"Leave now, before it is too late!\"");
            break;
        case 15:
            set_colour(RED);
            mpr("\"You will not leave this place.\"");
            break;
        case 16:
            set_colour(RED);
            mpr("\"We have you now!\"");
            break;
        }

        if (random2(3) == 0)
        {
         if (random2(2) == 0) miscast_effect(SPTYP_NECROMANCY, 4 + random2(6), random2(33) + random2(33) + random2(33), 100);
          else if (random2(2) == 0) miscast_effect(SPTYP_SUMMONING, 4 + random2(6), random2(33) + random2(33) + random2(33), 100);
                else if (random2(2) == 0) miscast_effect(SPTYP_CONJURATION, 4 + random2(6), random2(33) + random2(33) + random2(33), 100);
                        else if (random2(2) == 0) miscast_effect(SPTYP_ENCHANTMENT, 4 + random2(6), random2(33) + random2(33) + random2(33), 100);
        }
        else
                if (random2(3) == 0)
                switch(you.where_are_you)
                {
                 case BRANCH_DIS: miscast_effect(SPTYP_EARTH, 4 + random2(6), random2(33) + random2(33) + random2(33), 100);
                      break;
                 case BRANCH_GEHENNA: miscast_effect(SPTYP_FIRE, 4 + random2(6), random2(33) + random2(33) + random2(33), 100);
                      break;
                 case BRANCH_COCYTUS: miscast_effect(SPTYP_ICE, 4 + random2(6), random2(33) + random2(33) + random2(33), 100);
                      break;
                 case BRANCH_TARTARUS: miscast_effect(SPTYP_NECROMANCY, 4 + random2(6), random2(33) + random2(33) + random2(33), 100);
                      break;
                }
        else
          if (random2(3) == 0)
            switch(you.where_are_you)
                {
                 case BRANCH_DIS: create_monster(summon_any_demon(2), 0, 1, you.x_pos, you.y_pos, MHITYOU, 250);
                      break;
                 case BRANCH_GEHENNA: create_monster(MONS_FIEND, 0, 1, you.x_pos, you.y_pos, MHITYOU, 250);
                      break;
                 case BRANCH_COCYTUS: create_monster(MONS_ICE_FIEND, 0, 1, you.x_pos, you.y_pos, MHITYOU, 250);
                      break;
                 case BRANCH_TARTARUS: create_monster(MONS_SHADOW_FIEND, 0, 1, you.x_pos, you.y_pos, MHITYOU, 250);
                      break;
                }

// Note no "else". This can happen in addition to the above...

        if (random2(3) == 0)
    {
        create_monster(250, 0, 1, you.x_pos, you.y_pos, MHITYOU, 250);
        if (random2(3) == 0)
            create_monster(250, 0, 1, you.x_pos, you.y_pos, MHITYOU, 250);

        if (random2(3) == 0)
            create_monster(250, 0, 1, you.x_pos, you.y_pos, MHITYOU, 250);

        if (random2(3) == 0)
            create_monster(250, 0, 1, you.x_pos, you.y_pos, MHITYOU, 250);

        if (random2(3) == 0)
            create_monster(250, 0, 1, you.x_pos, you.y_pos, MHITYOU, 250);
    }
    } // End of special Hellish things

    // Adjust the player's stats if s/he's deseased (or recovering).
    if (you.disease == 0)
    {
        if (you.strength < you.max_strength && random2(100) == 0)
        {
            mpr("You feel your strength returning.");
            you.strength++;
            you.redraw_strength = 1;
        }

        if (you.dex < you.max_dex && random2(100) == 0)
        {
            mpr("You feel your dexterity returning.");
            you.dex++;
            you.redraw_dexterity = 1;
        }

        if (you.intel < you.max_intel && random2(100) == 0)
        {
            mpr("You feel your intelligence returning.");
            you.intel++;
            you.redraw_intelligence = 1;
        }

    }
    else
    {
        if (random2(30) == 0)
        {
            mpr("Your disease is taking its toll.");
            lose_stat(100, 1);
        }
    }

    // Adjust the player's stats if s/he has the deterioration mutation
    if (you.mutation[MUT_DETERIORATION] > 0 && random2(200) <= you.mutation[MUT_DETERIORATION] * 5 - 2)
        lose_stat(100, 1);

    // Account for mutagenic radiation
    if (you.invis > 0 || (you.haste > 0 && you.berserker == 0))
        if (random2(10) == 0 && you.magic_contamination < 100)
            you.magic_contamination++;

    you.magic_contamination += random2(scan_randarts(RAP_MUTAGENIC) + 1);

    if (you.magic_contamination > 0 && random2(2) == 0)
    {
        if (you.magic_contamination > 4 && random2(150) <= you.magic_contamination)
        {
            mpr("You've accumulated too much magical radiation!");
            if (random2(2) == 0)
                mutate(100);
            else
                give_bad_mutation();
        }
        you.magic_contamination--;
    }

    // Random chance to identify staff in hand based off of Spellcasting
    // and an appropriate other spell skill... is 1/20 too fast?
    if (you.equip[EQ_WEAPON] != -1
        && you.inv_class[you.equip[EQ_WEAPON]] == OBJ_STAVES
        && you.inv_ident[you.equip[EQ_WEAPON]] == 0
        && random2(20) == 0)
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

            wield_change = 1;
        }
    }

    // Update the god's opinion of the player
    if (you.religion != GOD_NO_GOD)
    {
        switch (you.religion)
        {
        case GOD_XOM:
            if (random2(75) == 0)
                Xom_acts(1, you.experience_level + random2(15), 1);
            break;

        case GOD_ZIN:           // These gods like long-standing worshippers

        case GOD_ELYVILON:
            if (random2(20) == 0 && you.piety < 150)
                gain_piety(1);
            break;

        case GOD_SHINING_ONE:
            if (random2(15) == 0 && you.piety < 150)
                gain_piety(1);
            break;

        case GOD_YREDELEMNUL:
        case GOD_KIKUBAAQUDGHA:
        case GOD_VEHUMET:
            if (random2(17) == 0)
                lose_piety(1);
            if (you.piety <= 0)
                excommunication();
            break;

        case GOD_OKAWARU:       // These gods accept corpses, so they time-out faster:

        case GOD_TROG:
            if (random2(14) == 0)
                lose_piety(1);
            if (you.piety <= 0)
                excommunication();
            break;

        case GOD_MAKHLEB:
            if (random2(16) == 0)
                lose_piety(1);
            if (you.piety <= 0)
                excommunication();
            break;

        case GOD_SIF_MUNA:
            if (random2(20) == 0)
                lose_piety(1);
            if (you.piety <= 0)
                excommunication();
            break;

        case GOD_NEMELEX_XOBEH: // relatively patient

            if (random2(35) == 0)
                lose_piety(1);
            if (you.attribute[ATTR_CARD_COUNTDOWN] > 0 && random2(2) == 0)
                you.attribute[ATTR_CARD_COUNTDOWN]--;
            if (you.piety <= 0)
                excommunication();
            break;

        default:
            DEBUGSTR("Bad god");
        }
    }

    // If the player has the lost mutation forget portions of the map
    if (you.mutation[MUT_LOST] > 0)
        if (random2(100) <= you.mutation[MUT_LOST] * 5)
            forget_map(5 + random2(you.mutation[MUT_LOST] * 10));

    // Update all of the corpses and food chunks on the floor
    update_corpses(time_delta);

    // Update all of the corpses and food chunks in the player's inventory
    for (int c = 0; c < 52; c++)
    {
        if (you.inv_quantity[c] <= 0)
            continue;
        if (you.inv_class[c] != OBJ_CORPSES && you.inv_class[c] != OBJ_FOOD)
            continue;
        if (you.inv_class[c] == OBJ_CORPSES && you.inv_type[c] > CORPSE_SKELETON)
            continue;
        if (you.inv_class[c] == OBJ_FOOD && you.inv_type[c] != FOOD_CHUNK)
            continue;

        if ((time_delta / 20) >= you.inv_dam[c])
        {
            if (you.inv_class[c] == OBJ_FOOD)
            {
                if (you.equip[EQ_WEAPON] == c)
                {
                    unwield_item(you.equip[EQ_WEAPON]);
                    you.equip[EQ_WEAPON] = -1;
                    wield_change = 1;
                }

                you.inv_quantity[c] = 0;
                burden_change();
                you.num_inv_items--;
                continue;
            }

            if (you.inv_type[c] == CORPSE_SKELETON)
                continue;       // carried skeletons are not destroyed

            if (mons_skeleton(you.inv_plus[c]) == 0)
            {
                if (you.equip[EQ_WEAPON] == c)
                {
                    unwield_item(you.equip[EQ_WEAPON]);
                    you.equip[EQ_WEAPON] = -1;
                }

                you.inv_quantity[c] = 0;
                burden_change();
                you.num_inv_items--;
                continue;
            }

            you.inv_type[c] = 1;
            you.inv_dam[c] = 0;
            you.inv_colour[c] = LIGHTGREY;
            wield_change = 1;
            continue;
        }

        you.inv_dam[c] -= (time_delta / 20);
    }

    // Exercise armor and stealth skills
    if (you.equip[EQ_BODY_ARMOUR] == -1)
        goto practise_stealth;
    if (you.inv_type[you.equip[EQ_BODY_ARMOUR]] < 2)
        goto practise_stealth;

    // lowered the random roll from % 7 to % 6 -- bwross
    if (random2(1000) <= mass(OBJ_ARMOUR, you.inv_type[you.equip[EQ_BODY_ARMOUR]]) && random2(6) == 0)
        exercise(SK_ARMOUR, 1);

    // also skills:
practise_stealth:
    if (you.burden_state != 0 || you.berserker)
        return;

    if (you.equip[EQ_BODY_ARMOUR] != -1)
    {
        if (you.inv_dam[you.equip[EQ_BODY_ARMOUR]] / 30 != 4)   /* elven armours don't hamper stealth */
            if (you.inv_type[you.equip[EQ_BODY_ARMOUR]] > 1 && (you.inv_type[you.equip[EQ_BODY_ARMOUR]] < 22 || you.inv_type[you.equip[EQ_BODY_ARMOUR]] > 25))  /* neither do robes or steam/mottled DSM */
                if (random2( mass(2, you.inv_type[you.equip[EQ_BODY_ARMOUR]]) ) >= 100 || random2(3) != 0)
                    return;
    }

    //if (you.levitation != 0) return; // can't really practise stealth while floating, and an amulet of control flight shouldn't make much difference

    if (you.special_wield == 50)
        return;                 // shadow lantern stops stealth

    if (random2(6) == 0)
        exercise(SK_STEALTH, 1);
}                               // end handle_time


long autopickups = 0L;          //set in init file, options, etc.

int autopickup_on = 1;

void autopickup()
{
    //David Loewenstern 6/99
    int  items_here = 0;
    int  result, o, hrg;
    bool did_pickup = false;

    if (autopickup_on == 0 || autopickups == 0L)
        return;

    if (you.levitation != 0 && wearing_amulet(AMU_CONTROLLED_FLIGHT) == 0)
        return;                 //flying

    o = env.igrid[you.x_pos][you.y_pos];
    if (o == ING)               //no objs
        return;

    last_item = ING;
    while (o != ING)
    {
        items_here++;
        if (autopickups & (1L << mitm.base_type[o]))
        {
            result = add_item(o, mitm.quantity[o]);
            if (result == 0)
            {
                mpr("You can't carry any more.");
                return;
            }

            if (result == ING)
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
            mitm.link[o] = ING;

        o = hrg;
    }

    if (did_pickup && you.delay_t == 0)
    {
        you.delay_t = 3;
        you.delay_doing = DELAY_AUTOPICKUP;
    }
}

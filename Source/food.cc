/*
 *  File:       food.cc
 *  Summary:    Functions for eating and butchering.
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 *      <2>      5/20/99        BWR           Added CRAWL_PIZZA.
 *      <1>      -/--/--        LRH           Created
 */

#include "AppHdr.h"
#include "food.h"

#include <string.h>
// required for abs() {dlb}:
#include <stdlib.h>
#include <stdio.h>

#ifdef DOS
#include <conio.h>
#endif

#include "externs.h"

#include "debug.h"
#include "invent.h"
#include "itemname.h"
#include "item_use.h"
#include "misc.h"
#include "mon-util.h"
#include "mutation.h"
#include "player.h"
#include "religion.h"
#include "skills2.h"
#include "spells2.h"
#include "stuff.h"
#include "wpn-misc.h"

#ifdef MACROS
#include "macro.h"
#endif

extern bool wield_change;       // defined in output.cc

static bool can_ingest(int what_isit, int kindof_thing, bool suppress_msg);
static bool eat_from_floor(void);
static int determine_chunk_effect(int which_chunk_type, bool rotten_chunk);
static void eat_chunk(int chunk_effect);
static void eat_from_inventory(int which_inventory_slot);
static void eating(unsigned char item_class, int item_type);
static void ghoul_eat_flesh(bool rotting_chunk);
static void how_hungered(int hunger_increment);
static void how_satiated(int hunger_decrement);

/*
 **************************************************
 *                                                *
 *             BEGIN PUBLIC FUNCTIONS             *
 *                                                *
 **************************************************
*/

// the parameter passed is whether the change is toward
// more hunger (-1), less (+1), or neither (0) {dlb}
void food_change(char type_of_change)
{
    if (you.is_undead == US_UNDEAD)
    {
        // cannot call set_hunger() here, or an infinite loop begins {dlb}
        you.hunger = 6000;
        you.hunger_state = HS_SATIATED;
    }
    else if (you.species == SP_GHOUL && (type_of_change == 1))
    {
        // cannot call set_hunger() here, or an infinite loop begins {dlb}
        you.hunger = 6999;
        you.hunger_state = HS_SATIATED;
    }
    else if (you.hunger <= 1000)
        you.hunger_state = HS_STARVING;
    else if (you.hunger <= 2600)
        you.hunger_state = HS_HUNGRY;
    else if (you.hunger < 7000)
        you.hunger_state = HS_SATIATED;
    else if (you.hunger < 11000)
        you.hunger_state = HS_FULL;
    else
        you.hunger_state = HS_ENGORGED;

    you.redraw_hunger = 1;

    return;
}                               // end food_change()

// note the similarities to food_change() - I think they
// can be merged on some level, but how escapes me at the
// moment ... I'll come back to it in time. 2apr2000 {dlb}
void hunger_warning(void)
{
    switch (you.hunger_state)
    {
    case HS_HUNGRY:
        if (you.hunger <= 1000)
        {
            you.hunger_state = HS_STARVING;
            you.redraw_hunger = 1;

            mpr("You are starving!", MSGCH_FOOD);
        }
        break;
    case HS_SATIATED:
        if (you.hunger <= 2600)
        {
            you.hunger_state = HS_HUNGRY;
            you.redraw_hunger = 1;

            mpr("You are feeling hungry.", MSGCH_FOOD);
        }
        break;
    case HS_FULL:
        if (you.hunger < 7000)
        {
            you.hunger_state = HS_SATIATED;
            you.redraw_hunger = 1;
        }
        break;
    case HS_ENGORGED:
        if (you.hunger < 11000)
        {
            you.hunger_state = HS_FULL;
            you.redraw_hunger = 1;
        }
        break;
    default:
        break;
    }

    return;
}                               // end hunger_warning()

void make_hungry(int hunger_amount, bool suppress_msg)
{
    if (you.is_undead != US_UNDEAD)
    {
        if (!suppress_msg)
            how_hungered(hunger_amount);

        you.hunger -= hunger_amount;

        if (you.hunger < 0)
            you.hunger = 0;

        food_change(-1);
    }

    return;
}                               // end make_hungry()

void lessen_hunger(int satiated_amount, bool suppress_msg)
{
    if (you.is_undead != US_UNDEAD)
    {
        if (!suppress_msg)
            how_satiated(satiated_amount);

        you.hunger += satiated_amount;

        if (you.hunger > 12000)
            you.hunger = 12000;

        food_change(1);
    }

    return;
}                               // end lessen_hunger()

void set_hunger(int new_hunger_level, bool suppress_msg)
{
    int hunger_difference = (new_hunger_level - you.hunger);

    if (you.is_undead != US_UNDEAD)
    {
        if (hunger_difference < 0)
            make_hungry(abs(hunger_difference), suppress_msg);
        else if (hunger_difference > 0)
            lessen_hunger(hunger_difference, suppress_msg);
    }

    return;
}                               // end set_hunger()

// more of a "weapon_switch back from butchering" function, switching
// to a weapon is done using the wield_weapon code.
static void weapon_switch( int targ )
{
    you.equip[EQ_WEAPON] = targ;

    if (targ != -1)
    {
        char buff[80];
        in_name( targ, 3, buff );

        char let = index_to_letter( targ );

        sprintf( info, "Switching back to %c - %s.", let, buff );
        mpr( info );
    }
    else
    {
        mpr( "You switch back to your bare hands." );
    }
}

bool butchery(void)
{
    int items_here = 0;
    int o = igrd[you.x_pos][you.y_pos];
    int k = 0;
    int item_got;
    unsigned char keyin;

    bool can_butcher = false;
    bool wpn_switch = false;
    int old_weapon = you.equip[EQ_WEAPON];

    bool barehand_butcher = (you.species == SP_TROLL || you.species == SP_GHOUL
                     || you.attribute[ATTR_TRANSFORMATION] == TRAN_BLADE_HANDS
                     || you.attribute[ATTR_TRANSFORMATION] == TRAN_DRAGON);


    if (igrd[you.x_pos][you.y_pos] == NON_ITEM)
    {
        mpr("There isn't anything here!");
        return false;
    }

    if (you.levitation && !wearing_amulet(AMU_CONTROLLED_FLIGHT))
    {
        mpr("You can't reach the floor from up here.");
        return false;
    }

    if (barehand_butcher)
        can_butcher = true;
    else
    {
        if (you.equip[EQ_WEAPON] != -1)
        {
            can_butcher = can_cut_meat( you.inv_class[you.equip[EQ_WEAPON]],
                                        you.inv_type[you.equip[EQ_WEAPON]] );
        }

        // XXX: Should probably check for cursed-weapons, bare hands and
        // non-weapons in hand here, but wield_weapon will be used for
        // this swap and it will do all that (although the player might
        // be annoyed with the excess prompt).
        if (Options.easy_butcher && !can_butcher)
        {
            const int a_slot = letter_to_index('a');
            const int b_slot = letter_to_index('b');

            // Find out which slot is our auto-swap slot
            int swap_slot = a_slot;
            if (you.equip[EQ_WEAPON] == a_slot)
                swap_slot = b_slot;

            // check if the swap slot is appropriate first
            if (you.equip[EQ_WEAPON] != swap_slot)
            {
                if (you.inv_quantity[ swap_slot ] > 0  // must have one

                    // must be able to cut with it
                    && can_cut_meat( you.inv_class[ swap_slot ],
                                     you.inv_type[ swap_slot ] )

                    // must be uncursed curse-status-known weapon
                    && you.inv_ident[ swap_slot ] > 0
                    && you.inv_class[ swap_slot ] == OBJ_WEAPONS
                    && you.inv_plus[ swap_slot ] < 80)
                {
                    mpr( "Switching to your swap slot weapon." );
                    wpn_switch = true;
                    wield_weapon( true );
                }
            }

            // if we didn't swap above, then we still can't cut... let's
            // call wield_weapon() in the "prompt the user" way...
            if (!wpn_switch)
            {
                // prompt for new weapon
                mpr( "What would you like to use?", MSGCH_PROMPT );
                wield_weapon( false );

                // let's see if the user did something...
                if (you.equip[EQ_WEAPON] != old_weapon)
                    wpn_switch = true;
            }
        }

        // weapon might have changed (to bare hands as well), we'll
        // update the can_butcher status accordingly (note: if we could
        // butcher with our bare hands we wouldn't be here) -- bwr
        if (wpn_switch && you.equip[EQ_WEAPON] != -1)
        {
            can_butcher = can_cut_meat( you.inv_class[you.equip[EQ_WEAPON]],
                                        you.inv_type[you.equip[EQ_WEAPON]] );
        }
    }

    // Final checks and clue-giving...
    if (!barehand_butcher && you.equip[EQ_WEAPON] == -1)
    {
        mpr("What, with your bare hands?");

        // Switching back to avoid possible bug where player can use
        // this to switch weapons in zero time.
        if (wpn_switch)
            weapon_switch( old_weapon );

        return false;
    }
    else if (!can_butcher)
    {
        mpr("Maybe you should try using a sharper implement.");

        // Switching back to avoid possible bug where player can use
        // this to switch weapons in zero time.
        if (wpn_switch)
            weapon_switch( old_weapon );

        return false;
    }

    // No turning back at this point, we better be qualified.
    ASSERT( can_butcher );

    int last_item = NON_ITEM;

    int objl = igrd[you.x_pos][you.y_pos];
    int hrg = 0;
    int counter = 0;

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

            if (wpn_switch)
                weapon_switch( old_weapon );

            return false;
        }

    }

    if (items_here == 1)
    {
        if (mitm.base_type[igrd[you.x_pos][you.y_pos]] != OBJ_CORPSES ||
            mitm.sub_type[igrd[you.x_pos][you.y_pos]] != CORPSE_BODY)
        {
            goto out_of_eating;
        }

        strcpy(info, "Butcher ");
        it_name(igrd[you.x_pos][you.y_pos], 3, str_pass);
        strcat(info, str_pass);
        strcat(info, "\?");
        mpr(info);

        unsigned char keyin = getch();

        if (keyin == 0)
        {
            getch();
            keyin = 0;
        }
        if (keyin != 'y' && keyin != 'Y')
        {
            if (wpn_switch)
                weapon_switch( old_weapon );

            return false;
        }

        int item_got = igrd[you.x_pos][you.y_pos];

        last_item = NON_ITEM;
        //you.redraw_hunger = 1;

        you.delay_t = 4 + (wpn_switch);  // add extra if we swapped
        you.delay_doing = DELAY_BUTCHER;

        if (barehand_butcher)
            mpr("You start tearing the corpse apart.");
        else
            mpr("You start hacking away.");

        if (you.duration[DUR_PRAYER]
            && (you.religion == GOD_OKAWARU
                || you.religion == GOD_MAKHLEB || you.religion == GOD_TROG))
        {
            offer_corpse(item_got);
            you.turn_is_over = 1;
            destroy_item(item_got);
            you.delay_t = 0;
            you.delay_doing = 0;

            if (wpn_switch)
            {
                weapon_switch( old_weapon );
                // need to count the swap delay in this case
                you.delay_t = 2;  // yes, two is corrent, 1 == no delay
                you.delay_doing = DELAY_WEAPON_SWAP;
            }

            return true;        // no chunks
        }

        you.turn_is_over = 1;
        place_chunks( mitm.pluses[item_got], mitm.special[item_got], you.x_pos,
                      you.y_pos, mitm.colour[item_got] );
        destroy_item(item_got);

        if (wpn_switch)
            weapon_switch( old_weapon );

        return true;

    }                           // end "if items_here == 1"

    last_item = NON_ITEM;

    if (items_here > 1)
    {
        o = igrd[you.x_pos][you.y_pos];

        for (k = 0; k < items_here; k++)
        {
            if (mitm.base_type[o] != OBJ_CORPSES
                    || mitm.sub_type[o] != CORPSE_BODY)
            {
                goto out_of_eating;
            }

            strcpy(info, "Butcher ");
            it_name(o, 3, str_pass);
            strcat(info, str_pass);
            strcat(info, "\?");
            mpr(info);

            keyin = getch();
            if (keyin == 0)
            {
                getch();
                keyin = 0;
            }

            if (keyin == 'q')
            {
                if (wpn_switch)
                    weapon_switch( old_weapon );

                return false;
            }

            if (keyin == 'y')
            {
                item_got = o;

                you.delay_t = 4 + (wpn_switch); // extra if we swapped
                you.delay_doing = DELAY_BUTCHER;

                if (barehand_butcher)
                    mpr("You start tearing the corpse apart.");
                else
                    mpr("You start hacking away.");

                if (you.duration[DUR_PRAYER]
                    && (you.religion == GOD_OKAWARU
                        || you.religion == GOD_MAKHLEB
                        || you.religion == GOD_TROG))
                {
                    offer_corpse(item_got);
                    you.turn_is_over = 1;
                    destroy_item(item_got);
                    you.delay_t = 0;
                    you.delay_doing = 0;

                    if (wpn_switch)
                    {
                        weapon_switch( old_weapon );
                        // need to count the swap delay in this case
                        you.delay_t = 2;  // yes, two is correct, 1 == no delay
                        you.delay_doing = DELAY_WEAPON_SWAP;
                    }

                    return true;        // no chunks
                }

                place_chunks(mitm.pluses[item_got], mitm.special[item_got],
                             you.x_pos, you.y_pos, mitm.colour[item_got] );

                you.turn_is_over = 1;
                destroy_item(item_got);

                if (wpn_switch)
                    weapon_switch( old_weapon );
                return true;
            }

          out_of_eating:

            if (mitm.quantity[o] > 0)
                last_item = o;

            hrg = mitm.link[o];
            o = hrg;

            if (o == NON_ITEM)
                goto failed;

            if (items_here == 0)
                break;
        }                       // end "for k" loop
    }
    //return false;

  failed:
    mpr("There isn't anything to dissect here.");

    if (wpn_switch)
        weapon_switch( old_weapon );

    return false;
}                               // end butchery()

void eat_food(void)
{
    int food_eat_1;
    int which_inventory_slot;

    unsigned char nthing = 0;

    if (you.is_undead == US_UNDEAD)
    {
        mpr("You can't eat.");
        return;
    }

    if (you.hunger >= 11000)
    {
        mpr("You're too full to eat anything.");
        return;
    }

    if (igrd[you.x_pos][you.y_pos] != NON_ITEM)
    {
        if (eat_from_floor())
            return;
    }

    if (you.num_inv_items < 1)
    {
        canned_msg(MSG_NOTHING_CARRIED);
        return;
    }

  query:
    mpr("Eat which item?", MSGCH_PROMPT);

    unsigned char keyin = get_ch();

    if (keyin == '?' || keyin == '*')
    {
        if (keyin == '?')
            nthing = get_invent(OBJ_FOOD);
        if (keyin == '*')
            nthing = get_invent(-1);
        if ((nthing >= 'A' && nthing <= 'Z')
            || (nthing >= 'a' && nthing <= 'z'))
        {
            keyin = nthing;
        }
        else
        {
            //mesclr();
            goto query;

        }
    }

    food_eat_1 = (int) keyin;

    if (food_eat_1 < 'A' || (food_eat_1 > 'Z' && food_eat_1 < 'a')
        || food_eat_1 > 'z')
    {
        mpr("You don't have any such object.");
        return;
    }

    which_inventory_slot = letter_to_index(food_eat_1);

    if (you.inv_quantity[which_inventory_slot] == 0)
    {
        mpr("You don't have any such object.");
        return;
    }

    // this conditional can later be merged into food::can_ingest() when
    // expanded to handle more than just OBJ_FOOD 16mar200 {dlb}
    if (you.inv_class[which_inventory_slot] != OBJ_FOOD)
    {
        mpr("You can't eat that!");
        return;
    }

    if (!can_ingest( you.inv_class[which_inventory_slot],
                        you.inv_type[which_inventory_slot], false ))
    {
        return;
    }

    you.inv_quantity[which_inventory_slot]--;

    if (you.inv_quantity[which_inventory_slot] == 0)
    {
        you.num_inv_items--;
        if (you.equip[EQ_WEAPON] == which_inventory_slot)
        {
            you.equip[EQ_WEAPON] = -1;
            mpr("You are now empty handed.");
            wield_change = true;
        }
    }

    eat_from_inventory(which_inventory_slot);
    burden_change();
    you.turn_is_over = 1;
}                               // end eat_food()

/*
 **************************************************
 *                                                *
 *              END PUBLIC FUNCTIONS              *
 *                                                *
 **************************************************
*/

static void how_hungered(int hunger_increment)
{
    if (hunger_increment > 0)
    {
        strcpy(info, (hunger_increment <= 100) ? "You feel slightly " :
                     (hunger_increment <= 350) ? "You feel somewhat " :
                     (hunger_increment <= 800) ? "You feel a quite a bit "
                                               : "You feel a lot ");

        strcat(info, (you.hunger_state > HS_SATIATED) ? "less full."
                                                      : "more hungry.");

        mpr(info);
    }

    return;
}                               // end how_hungered()

static void how_satiated(int hunger_decrement)
{
    if (hunger_decrement > 0)
    {
        strcpy(info, (hunger_decrement <= 100) ? "You feel slightly " :
                     (hunger_decrement <= 350) ? "You feel somewhat " :
                     (hunger_decrement <= 800) ? "You feel a quite a bit "
                                               : "You feel a lot ");

        strcat(info, (you.hunger_state > HS_SATIATED) ? "more full."
                                                      : "less hungry.");

        mpr(info);
    }

    return;
}                               // end how_satiated()

static void eat_from_inventory(int which_inventory_slot)
{
    if (you.inv_type[which_inventory_slot] == FOOD_CHUNK)
    {
        // this is a bit easier to read... most compilers should
        // handle this the same -- bwr
        const int mons_type = you.inv_plus[ which_inventory_slot ];
        const int chunk_type = mons_corpse_thingy( mons_type );
        const bool rotten = (you.inv_dam[which_inventory_slot] < 100);

        eat_chunk( determine_chunk_effect( chunk_type, rotten ) );
        return;
    }

    eating( you.inv_class[which_inventory_slot],
                            you.inv_type[which_inventory_slot] );
}                               // end eat_from_inventory()

static bool eat_from_floor(void)
{
    int o = igrd[you.x_pos][you.y_pos];
    int k = 0;                  /* initialised in the loop */
    unsigned char keyin;

    int items_here = 0;
    int item_got = 0;

    if (you.levitation && !wearing_amulet(AMU_CONTROLLED_FLIGHT))
    {
        //mpr("You can't reach the floor from up here.");
        return false;
    }

    int last_item = NON_ITEM;

    int objl = igrd[you.x_pos][you.y_pos];
    int hrg = 0;
    int counter = 0;

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
            return false;
        }
    }

    if (items_here == 1)
    {
        if (mitm.base_type[igrd[you.x_pos][you.y_pos]] != OBJ_FOOD)
            goto out_of_eating;

        strcpy(info, "Eat ");

        if (mitm.quantity[igrd[you.x_pos][you.y_pos]] > 1)
            strcat(info, "one of ");

        it_name(igrd[you.x_pos][you.y_pos], 3, str_pass);
        strcat(info, str_pass);
        strcat(info, "\?");
        mpr(info);

        unsigned char keyin = getch();

        if (keyin == 0)
        {
            getch();
            keyin = 0;
        }

        if (keyin != 'y' && keyin != 'Y')
            return false;

        int item_got = igrd[you.x_pos][you.y_pos];

        last_item = NON_ITEM;
        //you.redraw_hunger = 1;

        if (!can_ingest( mitm.base_type[item_got], mitm.sub_type[item_got],
                                                                    false) )
        {
            return false;
        }
        else if (mitm.sub_type[item_got] == FOOD_CHUNK)
        {
            // this is a bit easier to read... most compilers should
            // handle this the same -- bwr
            const int mons_type = mitm.pluses[ item_got ];
            const int chunk_type = mons_corpse_thingy( mons_type );
            const bool rotten = (mitm.special[ item_got ] < 100);

            eat_chunk( determine_chunk_effect( chunk_type, rotten ) );
            goto out_of_eating3;
        }

        eating(mitm.base_type[item_got], mitm.sub_type[item_got]);

      out_of_eating3:
        you.turn_is_over = 1;

        if (mitm.quantity[item_got] <= 1)
            destroy_item(item_got);
        else
            mitm.quantity[item_got]--;

        return true;
    }
    // end of if items_here

    last_item = NON_ITEM;

    if (items_here > 1)
    {
        o = igrd[you.x_pos][you.y_pos];

        // use k because of call to relay_message()
        for (k = 0; k < items_here; k++)
        {
            if (mitm.base_type[o] != OBJ_FOOD)
                goto out_of_eating;

            strcpy(info, "Eat ");

            if (mitm.quantity[o] > 1)
                strcat(info, "one of ");

            it_name(o, 3, str_pass);
            strcat(info, str_pass);
            strcat(info, "\?");
            mpr(info);

            keyin = getch();
            if (keyin == 0)
            {
                getch();
                keyin = 0;
            }

            if (keyin == 'q')
            {
                return false;
            }
            if (keyin == 'y')
            {
                item_got = o;

                if (!can_ingest( mitm.base_type[item_got],
                                            mitm.sub_type[item_got], false ))
                {
                    return false;
                }
                else if (mitm.sub_type[item_got] == FOOD_CHUNK)
                {
                    // this is a bit easier to read... most compilers should
                    // handle this the same -- bwr
                    const int mons_type = mitm.pluses[ item_got ];
                    const int chunk_type = mons_corpse_thingy( mons_type );
                    const bool rotten = (mitm.special[ item_got ] < 100);

                    eat_chunk( determine_chunk_effect( chunk_type, rotten ) );
                    goto out_of_eating2;
                }

                eating( mitm.base_type[item_got], mitm.sub_type[item_got] );

              out_of_eating2:
                you.turn_is_over = 1;

                if (mitm.quantity[item_got] == 1)
                    destroy_item(item_got);
                else
                    mitm.quantity[item_got]--;

                return true;

                //if (grunk != 1) // ie if the item picked up is still there.
                //  last_item = item_got;
            }

          out_of_eating:
            if (mitm.quantity[o] > 0)
                last_item = o;

            hrg = mitm.link[o];
            o = hrg;
            if (o == NON_ITEM)
                return false;

            if (items_here == 0)
                break;
        }                       // end of while k loop
        //      strcpy(info, "That's all.");
    }
    // end of if items_here
    //itc = 0;
    return false;
}                               // end eat_from_floor()

// never called directly - chunk_effect values must pass
// through food::determine_chunk_effect() first {dlb}:
static void eat_chunk(int chunk_effect)
{

    bool likes_chunks = (you.species == SP_KOBOLD || you.species == SP_OGRE
                         || you.species == SP_TROLL
                         || you.mutation[MUT_CARNIVOROUS] > 0);

    if (you.species == SP_GHOUL)
    {
        ghoul_eat_flesh(chunk_effect == CE_ROTTEN);

        you.delay_t = 3;
        you.delay_doing = DELAY_EAT;

        food_change(1);
    }
    else
    {
        switch (chunk_effect)
        {
        case CE_MUTAGEN_RANDOM:
            mpr("This meat tastes really weird.");
            mutate(100);
            break;

        case CE_MUTAGEN_BAD:
            mpr("This meat tastes *really* weird.");
            give_bad_mutation();
            break;

        case CE_HCL:
            mpr("You feel your flesh start to rot away!");

            you.rotting += 10 + random2(10);

            if (you.disease > 100)
                you.disease = 210;
            else
                you.disease += 50 + random2(100);
            break;

        case CE_POISONOUS:
            mpr("Yeeuch - this meat is poisonous!");

            you.poison += 3 + random2(4);
            break;

        case CE_ROTTEN:
        case CE_CONTAMINATED:
            mpr("There is something wrong with this meat.");
            mpr("You feel ill.");

            if (you.disease > 100)
                you.disease = 210;
            else
                you.disease += 50 + random2(100);
            break;

        // note that this is the only case that takes time and forces redraw
        case CE_CLEAN:
            strcpy(info, "This raw flesh ");

            strcat(info, (likes_chunks) ? "tastes good."
                                        : "is not very appetising.");
            mpr(info);

            you.delay_t = 3;
            you.delay_doing = DELAY_EAT;

            lessen_hunger(1000, true);
            break;
        }
    }

    return;
}                               // end eat_chunk()

static void ghoul_eat_flesh(bool rotting_chunk)
{

    bool healed = false;

    if (!rotting_chunk)
    {
        mpr("This raw flesh tastes good.");

        if (!one_chance_in(5))
            healed = true;

        if (you.base_hp < 5000 && !one_chance_in(3))
        {
            mpr("You feel more resilient.");
            you.base_hp++;
        }
    }
    else
    {
        mpr("This rotting flesh tastes delicious!");

        healed = true;

        if (you.base_hp < 5000 && !one_chance_in(4))
        {
            mpr("You feel more resilient.");
            you.base_hp++;
        }
    }

    if (you.strength < you.max_strength && one_chance_in(5))
    {
        mpr("You feel your strength returning.");
        you.strength++;
        you.redraw_strength = 1;
    }

    if (healed && you.hp < you.hp_max)
        inc_hp(1 + random2(5) + random2(1 + you.experience_level), false);

    calc_hp();

    return;
}                               // end ghoul_eat_flesh()

static void eating(unsigned char item_class, int item_type)
{
    int temp_rand;              // probability determination {dlb}
    int hunger_level_now = you.hunger;  // this should clarify things {dlb}
    int how_herbivorous = you.mutation[MUT_HERBIVOROUS];
    int how_carnivorous = you.mutation[MUT_CARNIVOROUS];
    int carnivore_modifier = 0;
    int herbivore_modifier = 0;

    switch (item_class)
    {
    case OBJ_FOOD:
        you.delay_t = ((item_type == FOOD_MEAT_RATION
                            || item_type == FOOD_BREAD_RATION) ? 4 : 1);
        you.delay_doing = DELAY_EAT;

        // apply base sustenance {dlb}:
        switch (item_type)
        {
        case FOOD_MEAT_RATION:
        case FOOD_ROYAL_JELLY:
            hunger_level_now += 5000;
            break;
        case FOOD_BREAD_RATION:
            hunger_level_now += 4400;
            break;
        case FOOD_HONEYCOMB:
            hunger_level_now += 2000;
            break;
        case FOOD_SNOZZCUMBER:  // maybe a nasty side-effect from RD's book?
        case FOOD_PIZZA:
        case FOOD_BEEF_JERKY:
            hunger_level_now += 1500;
            break;
        case FOOD_CHEESE:
        case FOOD_SAUSAGE:
            hunger_level_now += 1200;
            break;
        case FOOD_ORANGE:
        case FOOD_BANANA:
        case FOOD_LEMON:
            hunger_level_now += 1000;
            break;
        case FOOD_PEAR:
        case FOOD_APPLE:
        case FOOD_APRICOT:
            hunger_level_now += 700;
            break;
        case FOOD_CHOKO:
        case FOOD_RAMBUTAN:
        case FOOD_LYCHEE:
            hunger_level_now += 600;
            break;
        case FOOD_STRAWBERRY:
            hunger_level_now += 200;
            break;
        case FOOD_GRAPE:
            hunger_level_now += 100;
            break;
        case FOOD_SULTANA:
            hunger_level_now += 70;     // will not save you from starvation
            break;
        default:
            break;
        }                       // end base sustenance listing {dlb}

        // next, sustenance modifier for carnivores/herbivores {dlb}:
        // for some reason, sausages do not penalize herbivores {dlb}:
        switch (item_type)
        {
        case FOOD_MEAT_RATION:
            carnivore_modifier = 500;
            herbivore_modifier = -1500;
            break;
        case FOOD_BEEF_JERKY:
            carnivore_modifier = 200;
            herbivore_modifier = -200;
            break;
        case FOOD_BREAD_RATION:
            carnivore_modifier = -1000;
            herbivore_modifier = 500;
            break;
        case FOOD_BANANA:
        case FOOD_ORANGE:
        case FOOD_LEMON:
            carnivore_modifier = -300;
            herbivore_modifier = 300;
            break;
        case FOOD_PEAR:
        case FOOD_APPLE:
        case FOOD_APRICOT:
        case FOOD_CHOKO:
        case FOOD_SNOZZCUMBER:
        case FOOD_RAMBUTAN:
        case FOOD_LYCHEE:
            carnivore_modifier = -200;
            herbivore_modifier = 200;
            break;
        case FOOD_STRAWBERRY:
            carnivore_modifier = -50;
            herbivore_modifier = 50;
            break;
        case FOOD_GRAPE:
        case FOOD_SULTANA:
            carnivore_modifier = -20;
            herbivore_modifier = 20;
            break;
        default:
            carnivore_modifier = 0;
            herbivore_modifier = 0;
            break;
        }                    // end carnivore/herbivore modifier listing {dlb}

        // next, let's take care of messaging {dlb}:
        if (how_carnivorous > 0 && carnivore_modifier < 0)
            mpr("Blech - you need meat!");
        else if (how_herbivorous > 0 && herbivore_modifier < 0)
            mpr("Blech - you need greens!");

        if (how_herbivorous < 1)
        {
            switch (item_type)
            {
            case FOOD_MEAT_RATION:
                mpr("That meat ration really hit the spot!");
                break;
            case FOOD_BEEF_JERKY:
                strcpy(info, "That beef jerky was ");
                strcat(info, (one_chance_in(4)) ? "jerk-a-riffic"
                                                : "delicious");
                strcat(info, "!");
                mpr(info);
                break;
            default:
                break;
            }
        }

        if (how_carnivorous < 1)
        {
            switch (item_type)
            {
            case FOOD_BREAD_RATION:
                mpr("That bread ration really hit the spot!");
                break;
            case FOOD_PEAR:
            case FOOD_APPLE:
            case FOOD_APRICOT:
                strcpy(info, "Mmmm... Yummy ");
                strcat(info, (item_type == FOOD_APPLE)   ? "apple." :
                             (item_type == FOOD_PEAR)    ? "pear." :
                             (item_type == FOOD_APRICOT) ? "apricot."
                                                         : "fruit.");
                mpr(info);
                break;
            case FOOD_CHOKO:
                mpr("That choko was very bland.");
                break;
            case FOOD_SNOZZCUMBER:
                mpr("That snozzcumber tasted truly putrid!");
                break;
            case FOOD_ORANGE:
                strcpy(info, "That orange was delicious!");
                if (one_chance_in(8))
                    strcat(info, " Even the peel tasted good!");
                mpr(info);
                break;
            case FOOD_BANANA:
                mpr("That banana was delicious!");
                if (one_chance_in(8))
                    strcat(info, " Even the peel tasted good!");
                mpr(info);
                break;
            case FOOD_STRAWBERRY:
                mpr("That strawberry was delicious!");
                break;
            case FOOD_RAMBUTAN:
                mpr("That rambutan was delicious!");
                break;
            case FOOD_LEMON:
                mpr("That lemon was rather sour... But delicious nonetheless!");
                break;
            case FOOD_GRAPE:
                mpr("That grape was delicious!");
                break;
            case FOOD_SULTANA:
                mpr("That sultana was delicious! (but very small)");
                break;
            case FOOD_LYCHEE:
                mpr("That lychee was delicious!");
                break;
            default:
                break;
            }
        }

        switch (item_type)
        {
        case FOOD_HONEYCOMB:
            mpr("That honeycomb was delicious.");
            break;
        case FOOD_ROYAL_JELLY:
            mpr("That royal jelly was delicious!");
            restore_stat(STAT_ALL, false);
            break;
        case FOOD_PIZZA:
            strcpy(info, "Mmm... ");

            if (SysEnv.crawl_pizza && !one_chance_in(3))
                strcat(info, SysEnv.crawl_pizza);
            else
            {
                temp_rand = random2(9);

                strcat(info, (temp_rand == 0) ? "Ham and pineapple." :
                             (temp_rand == 1) ? "Extra thick crust." :
                             (temp_rand == 2) ? "Vegetable." :
                             (temp_rand == 3) ? "Pepperoni." :
                             (temp_rand == 4) ? "Yeuchh - Anchovies!" :
                             (temp_rand == 5) ? "Cheesy." :
                             (temp_rand == 6) ? "Supreme." :
                             (temp_rand == 7) ? "Super Supreme!"
                                              : "Chicken.");
            }
            mpr(info);
            break;
        case FOOD_CHEESE:
            strcpy(info, "Mmm... ");
            temp_rand = random2(9);

            strcat(info, (temp_rand == 0) ? "Cheddar" :
                         (temp_rand == 1) ? "Edam" :
                         (temp_rand == 2) ? "Wensleydale" :
                         (temp_rand == 3) ? "Camembert" :
                         (temp_rand == 4) ? "Goat cheese" :
                         (temp_rand == 5) ? "Fruit cheese" :
                         (temp_rand == 6) ? "Mozzarella" :
                         (temp_rand == 7) ? "Sheep cheese"
                                          : "Yak cheese");

            strcat(info, ".");
            mpr(info);
            break;
        case FOOD_SAUSAGE:
            mpr("That sausage was delicious!");
            break;
        default:
            break;
        }

        // finally, modify player's hunger level {dlb}:
        if (carnivore_modifier && how_carnivorous > 0)
            hunger_level_now += (carnivore_modifier * how_carnivorous);

        if (herbivore_modifier && how_herbivorous > 0)
            hunger_level_now += (herbivore_modifier * how_herbivorous);

        if (hunger_level_now != you.hunger)
            set_hunger(hunger_level_now, true);
        break;

    default:
        break;
    }

    return;
}                               // end eating()

static bool can_ingest(int what_isit, int kindof_thing, bool suppress_msg)
{
    bool survey_says = false;

    bool ur_carnivorous = (you.species == SP_GHOUL
                           || you.species == SP_KOBOLD
                           || you.mutation[MUT_CARNIVOROUS] == 3);

    bool ur_herbivorous = (you.mutation[MUT_HERBIVOROUS] > 1);

    // ur_chunkslover not defined in terms of ur_carnivorous because
    // a player could be one and not the other IMHO - 13mar2000 {dlb}
    bool ur_chunkslover = (you.hunger_state <= HS_HUNGRY
                           || wearing_amulet(AMU_THE_GOURMAND)
                           || you.species == SP_KOBOLD
                           || you.species == SP_OGRE
                           || you.species == SP_TROLL
                           || you.species == SP_GHOUL
                           || you.mutation[MUT_CARNIVOROUS]);

    switch (what_isit)
    {
    case OBJ_FOOD:
        switch (kindof_thing)
        {
        case FOOD_BREAD_RATION:
        case FOOD_PEAR:
        case FOOD_APPLE:
        case FOOD_CHOKO:
        case FOOD_SNOZZCUMBER:
        case FOOD_PIZZA:
        case FOOD_APRICOT:
        case FOOD_ORANGE:
        case FOOD_BANANA:
        case FOOD_STRAWBERRY:
        case FOOD_RAMBUTAN:
        case FOOD_LEMON:
        case FOOD_GRAPE:
        case FOOD_SULTANA:
        case FOOD_LYCHEE:
            if (ur_carnivorous)
            {
                survey_says = false;
                if (!suppress_msg)
                    mpr("Sorry, you're a carnivore.");
            }
            else
                survey_says = true;
            break;

        case FOOD_CHUNK:
            if (ur_herbivorous)
            {
                survey_says = false;
                if (!suppress_msg)
                    mpr("You can't eat raw meat!");
            }
            else if (!ur_chunkslover)
            {
                survey_says = false;
                if (!suppress_msg)
                    mpr("You aren't quite hungry enough to eat that!");
            }
            else
                survey_says = true;
            break;

        default:
            return true;
            break;

        }
        break;

    // other object types are set to return false for now until
    // someone wants to recode the eating code to permit consumption
    // of things other than just food -- corpses first, then more
    // exotic stuff later would be good to explore - 13mar2000 {dlb}
    case OBJ_CORPSES:
    default:
        return false;
        break;

    }

    return survey_says;
}                               // end can_ingest()

// see if you can follow along here -- except for the Amulet of the Gourmand
// addition (long missing and requested), what follows is an expansion of how
// chunks were handled in the codebase up to this date -- I have never really
// understood why liches are hungry and not true undead beings ... {dlb}:
static int determine_chunk_effect(int which_chunk_type, bool rotten_chunk)
{
    int poison_resistance_level = player_res_poison();
    int this_chunk_effect = which_chunk_type;

    // determine the initial effect of eating a particular chunk {dlb}:
    switch (this_chunk_effect)
    {
    case CE_HCL:
    case CE_MUTAGEN_RANDOM:
        if (you.species == SP_GHOUL
                || you.attribute[ATTR_TRANSFORMATION] == TRAN_LICH)
        {
            this_chunk_effect = CE_CLEAN;
        }
        break;

    case CE_POISONOUS:
        if (you.species == SP_GHOUL
                || you.attribute[ATTR_TRANSFORMATION] == TRAN_LICH)
        {
            this_chunk_effect = CE_CLEAN;
        }
        else if (poison_resistance_level > 0)
            this_chunk_effect = CE_CLEAN;
        break;

    case CE_CONTAMINATED:
        if (you.attribute[ATTR_TRANSFORMATION] == TRAN_LICH)
            this_chunk_effect = CE_CLEAN;
        else
        {
            switch (you.species)
            {
            case SP_GHOUL:
                this_chunk_effect = CE_ROTTEN;
                break;
            case SP_KOBOLD:
            case SP_TROLL:
                if (!one_chance_in(45))
                    this_chunk_effect = CE_CLEAN;
                break;
            case SP_HILL_ORC:
            case SP_OGRE:
                if (!one_chance_in(15))
                    this_chunk_effect = CE_CLEAN;
                break;
            default:
                if (!one_chance_in(3))
                    this_chunk_effect = CE_CLEAN;
                break;
            }
        }
        break;

    default:
        break;
    }

    // determine effects of rotting on base chunk effect {dlb}:
    if (rotten_chunk)
    {
        switch (this_chunk_effect)
        {
        case CE_CLEAN:
        case CE_CONTAMINATED:
            this_chunk_effect = CE_ROTTEN;
            break;
        case CE_MUTAGEN_RANDOM:
            this_chunk_effect = CE_MUTAGEN_BAD;
            break;
        default:
            break;
        }
    }

    // one last chance for some species to safely eat rotten food {dlb}:
    if (this_chunk_effect == CE_ROTTEN)
    {
        if (you.attribute[ATTR_TRANSFORMATION] == TRAN_LICH)
            this_chunk_effect = CE_CLEAN;
        else
        {
            switch (you.species)
            {
            case SP_KOBOLD:
            case SP_TROLL:
                if (!one_chance_in(15))
                    this_chunk_effect = CE_CLEAN;
                break;
            case SP_HILL_ORC:
            case SP_OGRE:
                if (!one_chance_in(5))
                    this_chunk_effect = CE_CLEAN;
                break;
            default:
                break;
            }
        }
    }

    // the amulet of the gourmad will permit consumption of rotting meat as
    // though it were "clean" meat - ghouls can expect the reverse, as they
    // prize rotten meat ... yum! {dlb}:
    if (wearing_amulet(AMU_THE_GOURMAND))
    {
        if (you.species == SP_GHOUL)
        {
            if (this_chunk_effect == CE_CLEAN)
                this_chunk_effect = CE_ROTTEN;
        }
        else
        {
            if (this_chunk_effect == CE_ROTTEN)
                this_chunk_effect = CE_CLEAN;
        }
    }

    return (this_chunk_effect);
}                               // end determine_chunk_effect()

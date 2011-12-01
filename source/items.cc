/*
 *  File:       items.cc
 *  Summary:    Misc (mostly) inventory related functions.
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 * <9> 7/08/01   MV   Added messages for chunks/corpses rotting
 * <8> 8/07/99   BWR  Added Rune stacking
 * <7> 6/13/99   BWR  Added auto staff detection
 * <6> 6/12/99   BWR  Fixed time system.
 * <5> 6/9/99    DML  Autopickup
 * <4> 5/26/99   JDJ  Drop will attempt to take off armour.
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

#include "globals.h"
#include "externs.h"

#include "beam.h"
#include "cloud.h"
#include "debug.h"
#include "delay.h"
#include "effects.h"
#include "invent.h"
#include "it_use2.h"
#include "item_use.h"
#include "itemname.h"
#include "itemprop.h"
#include "misc.h"
#include "monplace.h"
#include "monstuff.h"
#include "mstuff2.h"
#include "mon-util.h"
#include "mutation.h"
#include "player.h"
#include "randart.h"
#include "religion.h"
#include "shopping.h"
#include "skills.h"
#include "skills2.h"
#include "spl-cast.h"
#include "stuff.h"
#include "view.h"

static void do_autopickup(void);

// Used to be called "unlink_items", but all it really does is make
// sure item coordinates are correct to the stack they're in. -- bwr
void fix_item_coordinates(void)
{
    int x,y,i;

    // nails all items to the ground (i.e. sets x,y)
    for (x = 0; x < GXM; x++)
    {
        for (y = 0; y < GYM; y++)
        {
            i = igrd[x][y];

            while (i != NON_ITEM)
            {
                mitm[i].x = x;
                mitm[i].y = y;
                i = mitm[i].link;
            }
        }
    }
}

// This function uses the items coordinates to relink all the igrd lists.
void link_items(void)
{
    int i,j;

    // first, initailize igrd array
    for (i = 0; i < GXM; i++)
    {
        for (j = 0; j < GYM; j++)
        {
            igrd[i][j] = NON_ITEM;
        }
    }

    // link all items on the grid, plus shop inventory,
    // DON'T link the huge pile of monster items at (0,0)

    for (i = 0; i < MAX_ITEMS; i++)
    {
        if (!is_valid_item(mitm[i]) || (mitm[i].x == 0 && mitm[i].y == 0))
        {
            // item is not assigned,  or is monster item.  ignore.
            mitm[i].link = NON_ITEM;
            continue;
        }

        // link to top
        mitm[i].link = igrd[ mitm[i].x ][ mitm[i].y ];
        igrd[ mitm[i].x ][ mitm[i].y ] = i;
    }
}                               // end link_items()

static bool item_ok_to_clean( int item )
{
    // 5. never clean food or Orbs
    if (mitm[item].base_type == OBJ_FOOD || mitm[item].base_type == OBJ_ORBS)
        return (false);

    // never clean runes
    if (mitm[item].base_type == OBJ_MISCELLANY
        && mitm[item].sub_type == MISC_RUNE_OF_ZOT)
    {
        return (false);
    }

    return (true);
}

// returns index number of first available space, or NON_ITEM for
// unsuccessful cleanup (should be exceedingly rare!)
int cull_items(void)
{
    // XXX: Not the prettiest of messages, but the player
    // deserves to know whenever this kicks in. -- bwr
    mpr( MSGCH_WARN, "Too many items on level, removing some." );

    /* rules:
       1. Don't cleanup anything nearby the player
       2. Don't cleanup shops
       3. Don't cleanup monster inventory
       4. Clean 15% of items
       5. never remove food, orbs, runes
       7. uniques weapons are moved to the abyss
       8. randarts are simply lost
       9. unrandarts are 'destroyed', but may be generated again
    */

    int x, y, item, next;
    int first_cleaned = NON_ITEM;

    // 2. avoid shops by avoiding (0,5..9)
    // 3. avoid monster inventory by iterating over the dungeon grid
    for (x = X_BOUND_1 + 1; x < X_BOUND_2; x++)
    {
        for (y = Y_BOUND_1 + 1; y < Y_BOUND_2; y++)
        {
            // 1. not near player!
            if (grid_distance(x, y, you.x_pos, you.y_pos) <= MAX_VISION_RANGE)
                continue;

            // iterate through the grids list of items:
            for (item = igrd[x][y]; item != NON_ITEM; item = next)
            {
                next = mitm[item].link; // in case we can't get it later.

                if (item_ok_to_clean(item) && random2(100) < 15)
                {
                    if (is_fixed_artefact( mitm[item] ))
                    {
                        // 7. move uniques to abyss
                        set_unique_item_status( OBJ_WEAPONS, mitm[item].special,
                                                UNIQ_LOST_IN_ABYSS );
                    }
                    else if (is_unrandom_artefact( mitm[item] ))
                    {
                        // 9. unmark unrandart
                        x = find_unrandart_index( item );
                        if (x >= 0)
                            set_unrandart_exist( x, 0 );
                    }

                    // POOF!
                    destroy_item( item );
                    if (first_cleaned == NON_ITEM)
                        first_cleaned = item;
                }
            } // end for item

        } // end y
    } // end x

    return (first_cleaned);
}

// Note:  This function is to isolate all the checks to see if
//        an item is valid (often just checking the quantity).
//
//        It shouldn't be used a a substitute for those cases
//        which actually want to check the quantity (as the
//        rules for unused objects might change).
bool is_valid_item( const item_def &item )
{
    return (item.base_type != OBJ_UNASSIGNED && item.quantity > 0);
}

// Reduce quantity of an inventory item, do cleanup if item goes away.
//
// Returns true if stack of items no longer exists.
bool dec_inv_item_quantity( int obj, int amount )
{
    bool ret = false;

    if (you.equip[EQ_WEAPON] == obj)
        set_redraw_status( REDRAW_WIELD );

    if (you.inv[obj].quantity > amount)
        you.inv[obj].quantity -= amount;
    else
    {
        for (int i = 0; i < NUM_EQUIP; i++)
        {
            if (you.equip[i] == obj)
            {
                you.equip[i] = -1;

                if (i == EQ_WEAPON)
                    unwield_item();
            }
        }

        invalidate_item( you.inv[obj] );
        ret = true;
    }

    burden_change();

    return (ret);
}

// Reduce quantity of a monster/grid item, do cleanup if item goes away.
//
// Returns true if stack of items no longer exists.
bool dec_mitm_item_quantity( int obj, int amount )
{
    if (mitm[obj].quantity <= amount)
    {
        destroy_item( obj );
        return (true);
    }

    mitm[obj].quantity -= amount;

    return (false);
}

void inc_inv_item_quantity( int obj, int amount )
{
    if (you.equip[EQ_WEAPON] == obj)
        set_redraw_status( REDRAW_WIELD );

    you.inv[obj].quantity += amount;
    burden_change();
}

void inc_mitm_item_quantity( int obj, int amount )
{
    mitm[obj].quantity += amount;
}

void init_item( item_def &item, int base, int sub, int quant )
{
    item.base_type = base;
    item.sub_type  = sub;
    item.quantity = quant;

    item.plus  = 0;
    item.plus2  = 0;
    item.special  = 0;
    item.colour  = 0;
    item.flags  = 0;

    for (int i = 0; i < RAP_NUM_PROPERTIES; i++)
        item.ra_props[i] = 0;

    item.x  = 0;
    item.y  = 0;
    item.link  = NON_ITEM;
    item.slot  = -1;
}

void invalidate_item( item_def &item )
{
    item.base_type = OBJ_UNASSIGNED;
    item.quantity  = 0;
}

// Returns an unused mitm slot, or NON_ITEM if none available.
// The reserve is the number of item slots to not check.
// Items may be culled if a reserve <= 10 is specified.
int get_item_slot( int reserve )
{
    ASSERT( reserve >= 0 );

    int item = NON_ITEM;

    for (item = 0; item < (MAX_ITEMS - reserve); item++)
    {
        if (!is_valid_item( mitm[item] ))
            break;
    }

    if (item >= MAX_ITEMS - reserve)
    {
        item = (reserve <= 10) ? cull_items() : NON_ITEM;

        if (item == NON_ITEM)
            return (NON_ITEM);
    }

    ASSERT( item != NON_ITEM );

    init_item( mitm[item] );

    return (item);
}

// Find an empty inventory slot.  Take "pref" if != -1 and unused.
// Returns ENDOFPACK if no slot is found.
static int get_inv_slot( int pref )
{
    int ret;

    if (pref != -1 && !is_valid_item( you.inv[pref] ))
        ret = pref;
    else
    {
        for (ret = 0; ret < ENDOFPACK; ret++)
        {
            if (!is_valid_item( you.inv[ret] ))
                break;
        }
    }

    return (ret);
}

void unlink_item( int dest )
{
    int c = 0;
    int cy = 0;

    // Don't destroy non-items, may be called after an item has been
    // reduced to zero quantity however.
    if (dest == NON_ITEM || !is_valid_item( mitm[dest] ))
        return;

    if (mitm[dest].x == 0 && mitm[dest].y == 0)
    {
        // (0,0) is where the monster items are (and they're unlinked by igrd),
        // although it also contains items that are not linked in yet.
        //
        // Check if a monster has it:
        for (c = 0; c < MAX_MONSTERS; c++)
        {
            struct monsters *monster = &menv[c];

            if (monster->type == -1)
                continue;

            for (cy = 0; cy < NUM_MONSTER_SLOTS; cy++)
            {
                if (monster->inv[cy] == dest)
                {
                    monster->inv[cy] = NON_ITEM;

                    mitm[dest].x = 0;
                    mitm[dest].y = 0;
                    mitm[dest].link = NON_ITEM;
                    mitm[dest].slot = -1;
                    return;
                }
            }
        }

        // Always return because this item might just be temporary.
        return;
    }
    else
    {
        // Linked item on map:
        //
        // Use the items (x,y) to access the list (igrd[x][y]) where
        // the item should be linked.

        // First check the top:
        if (igrd[ mitm[dest].x ][ mitm[dest].y ] == dest)
        {
            // link igrd to the second item
            igrd[ mitm[dest].x ][ mitm[dest].y ] = mitm[dest].link;

            mitm[dest].x = 0;
            mitm[dest].y = 0;
            mitm[dest].link = NON_ITEM;
            mitm[dest].slot = -1;
            return;
        }

        // Okay, item is buried, find item that's on top of it:
        for (c = igrd[ mitm[dest].x ][ mitm[dest].y ]; c != NON_ITEM; c = mitm[c].link)
        {
            // find item linking to dest item
            if (is_valid_item( mitm[c] ) && mitm[c].link == dest)
            {
                // unlink dest
                mitm[c].link = mitm[dest].link;

                mitm[dest].x = 0;
                mitm[dest].y = 0;
                mitm[dest].link = NON_ITEM;
                mitm[dest].slot = -1;
                return;
            }
        }
    }

#if DEBUG
    // Okay, the sane ways are gone... let's warn the player:
    mpr( MSGCH_DANGER, "BUG WARNING: Problems unlinking item!!!" );

    // Okay, first we scan all items to see if we have something
    // linked to this item.  We're not going to return if we find
    // such a case... instead, since things are already out of
    // alignment, let's assume there might be multiple links as well.
    bool  linked = false;
    int   old_link = mitm[dest].link; // used to try linking the first

    // clean the relevant parts of the object:
    invalidate_item( mitm[dest] );

    // Look through all items for links to this item.
    for (c = 0; c < MAX_ITEMS; c++)
    {
        if (is_valid_item( mitm[c] ) && mitm[c].link == dest)
        {
            // unlink item
            mitm[c].link = old_link;

            if (!linked)
            {
                old_link = NON_ITEM;
                linked = true;
            }
        }
    }

    // Now check the grids to see if it's linked as a list top.
    for (c = X_BOUND_1 + 1; c < X_BOUND_2; c++)
    {
        for (cy = Y_BOUND_1 + 1; cy < Y_BOUND_2; cy++)
        {
            if (igrd[c][cy] == dest)
            {
                igrd[c][cy] = old_link;

                if (!linked)
                {
                    old_link = NON_ITEM;  // cleaned after the first
                    linked = true;
                }
            }
        }
    }


    // Okay, finally warn player if we didn't do anything.
    if (!linked)
        mpr(MSGCH_DANGER,"BUG WARNING: Item didn't seem to be linked at all." );
#endif
}                               // end unlink_item()

void destroy_item( int dest )
{
    // Don't destroy non-items, but this function may be called upon
    // to remove items reduced to zero quantity, so we allow "invalid"
    // objects in.
    if (dest == NON_ITEM || !is_valid_item( mitm[dest] ))
        return;

    unlink_item( dest );
}

void destroy_item_stack( int x, int y )
{
    int o = igrd[x][y];

    igrd[x][y] = NON_ITEM;

    while (o != NON_ITEM)
    {
        int next = mitm[o].link;

        if (is_valid_item( mitm[o] ))
        {
            if (mitm[o].base_type == OBJ_ORBS)
            {
                set_unique_item_status( OBJ_ORBS, mitm[o].sub_type,
                                        UNIQ_LOST_IN_ABYSS );
            }
            else if (is_fixed_artefact( mitm[o] ))
            {
                set_unique_item_status( OBJ_WEAPONS, mitm[o].special,
                                        UNIQ_LOST_IN_ABYSS );
            }

            invalidate_item( mitm[o] );
        }

        o = next;
    }
}


/*
 * If looking is true, then we're using the ';' command to get a full
 * list of items... otherwise, things may be abbreviated.
 */
void item_check( bool looking )
{
    const int grid = grd[you.x_pos][you.y_pos];

    if (grid >= DNGN_EXIT_HELL && grid <= DNGN_PERMADRY_FOUNTAIN)
    {
        if (grid >= DNGN_STONE_STAIRS_DOWN_I && grid <= DNGN_ROCK_STAIRS_DOWN)
        {
            mpr( "There is a %s staircase leading down here.",
                 (grid == DNGN_ROCK_STAIRS_DOWN) ? "rock" : "stone" );
        }
        else if (grid >= DNGN_STONE_STAIRS_UP_I && grid <= DNGN_ROCK_STAIRS_UP)
        {
            mpr( "There is a %s staircase leading upwards here.",
                 (grid == DNGN_ROCK_STAIRS_UP) ? "rock" : "stone" );
        }
        else
        {
            switch (grid)
            {
            case DNGN_EXIT_HELL:
                mpr("There is a gateway back to the Vestibule here (up).");
                break;
            case DNGN_ENTER_HELL:
                mpr("There is a gateway to Hell here (down).");
                break;
            case DNGN_ENTER_GEHENNA:
                mpr("There is a gateway to Gehenna here (down).");
                break;
            case DNGN_ENTER_COCYTUS:
                mpr("There is a gateway to the frozen wastes of Cocytus here (down).");
                break;
            case DNGN_ENTER_TARTARUS:
                mpr("There is a gateway to Tartarus here (down).");
                break;
            case DNGN_ENTER_DIS:
                mpr("There is a gateway to the Iron City of Dis here (down).");
                break;
            case DNGN_ENTER_SHOP:
                mpr( "There is an entrance to %s here (up).",
                      shop_name( you.x_pos, you.y_pos ) );
                break;
            case DNGN_ENTER_LABYRINTH:
                mpr("There is an entrance to to a labyrinth here (down).");
                mpr("Beware, for starvation awaits!");
                break;
            case DNGN_ENTER_ABYSS:
                mpr("There is a one-way gate into the infinite horrors of the Abyss here (down).");
                break;
            case DNGN_STONE_ARCH:
                mpr("There is an empty stone archway here.");
                break;
            case DNGN_EXIT_ABYSS:
                mpr("There is a gateway leading out of the Abyss here (down).");
                break;
            case DNGN_ENTER_PANDEMONIUM:
                mpr("There is a gate leading to the halls of Pandemonium here (down).");
                break;
            case DNGN_EXIT_PANDEMONIUM:
                mpr("There is a gate leading out of Pandemonium here (down).");
                break;
            case DNGN_TRANSIT_PANDEMONIUM:
                mpr("There is a gate leading to another region of Pandemonium here (down).");
                break;
            case DNGN_ENTER_ORCISH_MINES:
                mpr("There is a staircase to the Orcish Mines here.");
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
                mpr("There is a gateway to the Realm of Zot here (down).");
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
            case DNGN_RETURN_FROM_ORCISH_MINES:
            case DNGN_RETURN_FROM_HIVE:
            case DNGN_RETURN_FROM_LAIR:
            case DNGN_RETURN_FROM_VAULTS:
            case DNGN_RETURN_FROM_TEMPLE:
                mpr("There is a staircase back to the Dungeon here.");
                break;
            case DNGN_RETURN_FROM_SLIME_PITS:
            case DNGN_RETURN_FROM_SNAKE_PIT:
            case DNGN_RETURN_FROM_SWAMP:
                mpr("There is a staircase back to the Lair here.");
                break;
            case DNGN_RETURN_FROM_CRYPT:
            case DNGN_RETURN_FROM_HALL_OF_BLADES:
                mpr("There is a staircase back to the Vaults here.");
                break;
            case DNGN_RETURN_FROM_TOMB:
                mpr("There is a staircase back to the Crypt here.");
                break;
            case DNGN_RETURN_FROM_ELVEN_HALLS:
                mpr("There is a staircase back to the Mines here.");
                break;
            case DNGN_RETURN_FROM_ZOT:
                mpr("There is a gateway leading out of this Realm here (up).");
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
                mpr("There is a fountain here.");
                break;
            case DNGN_SPARKLING_FOUNTAIN:
                mpr("There is a sparkling fountain here.");
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

    if (is_cloud( you.x_pos, you.y_pos ))
    {
        const int cloud = get_cloud_type( you.x_pos, you.y_pos );

        mpr( "There is a cloud of %s here.",
                (cloud == CLOUD_FIRE
                  || cloud == CLOUD_FIRE_MON) ? "flame" :
                (cloud == CLOUD_STINK
                  || cloud == CLOUD_STINK_MON) ? "noxious fumes" :
                (cloud == CLOUD_COLD
                  || cloud == CLOUD_COLD_MON) ? "freezing vapour" :
                (cloud == CLOUD_POISON
                  || cloud == CLOUD_POISON_MON) ? "poison gases" :
                (cloud == CLOUD_GREY_SMOKE
                  || cloud == CLOUD_GREY_SMOKE_MON) ? "grey smoke" :
                (cloud == CLOUD_BLUE_SMOKE
                  || cloud == CLOUD_BLUE_SMOKE_MON) ? "blue smoke" :
                (cloud == CLOUD_PURP_SMOKE
                  || cloud == CLOUD_PURP_SMOKE_MON) ? "purple smoke" :
                (cloud == CLOUD_STEAM
                  || cloud == CLOUD_STEAM_MON) ? "steam" :
                (cloud == CLOUD_MIASMA
                  || cloud == CLOUD_MIASMA_MON) ? "foul pestilence" :
                (cloud == CLOUD_BLACK_SMOKE
                  || cloud == CLOUD_BLACK_SMOKE_MON) ? "black smoke"
                                                     : "buggy goodness" );
    }

    if (igrd[you.x_pos][you.y_pos] == NON_ITEM && looking)
    {
        mpr("There are no items here.");
        return;
    }

    if (Options.autopickup_on_look || !looking)
        do_autopickup();

    // Must be looking to see items if number_walk_items is set to zero.
    if (igrd[you.x_pos][you.y_pos] != NON_ITEM
        && (looking || Options.number_walk_items))
    {
        char  str_pass[ ITEMNAME_SIZE ];
        int   objl = igrd[you.x_pos][you.y_pos];

        if (mitm[objl].link == NON_ITEM)
        {
            // one item case (always one simple line)
            it_name( objl, DESC_NOCAP_A, str_pass );

            mpr( MSGCH_FLOOR_ITEMS, mitm[objl].colour,
                     "You see here %s.", str_pass );
        }
        else if (Options.number_walk_items == 1)
        {
            // always just a brief one-liner when number_walk_items == 1
            mpr( "There are several objects here." );
        }
        else
        {
            mpr( "Things that are here:" );

            for (int num = 0; objl != NON_ITEM; objl = mitm[objl].link, num++)
            {
                if (!looking
                    && num == Options.number_walk_items - 1
                    && mitm[objl].link != NON_ITEM)
                {
                    mpr( "There are several more objects here." );
                    break;
                }
                else
                {
                    it_name( objl, DESC_NOCAP_A, str_pass );

                    mpr( MSGCH_FLOOR_ITEMS, mitm[objl].colour,
                            "You see here %s.", str_pass );
                }
            }
        }
    }
}


void pickup_menu( int item_link )
{
    std::vector<item_def*> items;

    for (int i = item_link; i != NON_ITEM; i = mitm[i].link)
        items.push_back( &mitm[i] );

    std::vector<SelItem> selected = select_items( items, "Select items to pick up" );

    redraw_screen();

    for (int i = 0, count = selected.size(); i < count; ++i)
    {
        for (int j = item_link; j != NON_ITEM; j = mitm[j].link)
        {
            if (&mitm[j] == selected[i].item)
            {
                if (j == item_link)
                    item_link = mitm[j].link;

                unsigned long oldflags = mitm[j].flags;
                mitm[j].flags &= ~(ISFLAG_THROWN | ISFLAG_DROPPED);

                int result = move_item_to_player( j, selected[i].quantity );

                // If we cleared any flags on the items, but the pickup was
                // partial, reset the flags for the items that remain on the
                // floor.
                if (is_valid_item( mitm[j] ))
                    mitm[j].flags = oldflags;

                if (result == 0)
                {
                    mpr("You can't carry that much weight.");
                    return;
                }
                else if (result == -1)
                {
                    mpr("You can't carry that many items.");
                    return;
                }
                break;
            }
        }
    }
}


void do_pickup( void )
{
    int o = 0;
    int m = 0;
    int num = 0;
    unsigned char keyin = 0;
    int next;
    char str_pass[ ITEMNAME_SIZE ];

    if (you.attribute[ATTR_TRANSFORMATION] == TRAN_AIR
        && you.duration[DUR_TRANSFORMATION] > 0)
    {
        mpr("You can't pick up anything in this form!");
        return;
    }

    if (!player_access_floor())
    {
        canned_msg( MSG_CANT_REACH );
        return;
    }

    // Fortunately, the player is prevented from testing their
    // portable altar in the Ecumenical Temple. -- bwr
    if (grd[you.x_pos][you.y_pos] == DNGN_ALTAR_NEMELEX_XOBEH
        && !player_in_branch( BRANCH_ECUMENICAL_TEMPLE ))
    {
        if (inv_count() >= ENDOFPACK)
        {
            mpr("There is a portable altar here, but you can't carry anything else.");
            return;
        }

        if (yesno("There is a portable altar here. Pick it up?"))
        {
            m = get_inv_slot( you.nemelex_altar_index );

            if (m != ENDOFPACK) // paranoia: shouldn't happen
            {
                grd[you.x_pos][you.y_pos] = DNGN_FLOOR;

                you.inv[m].base_type = OBJ_MISCELLANY;
                you.inv[m].sub_type = MISC_PORTABLE_ALTAR_OF_NEMELEX;
                you.inv[m].plus = 0;
                you.inv[m].plus2 = 0;
                you.inv[m].special = 0;
                you.inv[m].colour = LIGHTMAGENTA;
                you.inv[m].quantity = 1;
                set_ident_flags( you.inv[m], ISFLAG_IDENT_MASK );

                you.inv[m].x = -1;
                you.inv[m].y = -1;
                you.inv[m].link = NON_ITEM;
                you.inv[m].slot = m;

                burden_change();

                in_name( m, DESC_INVENTORY_EQUIP, str_pass );
                mpr( MSGCH_FLOOR_ITEMS, you.inv[m].colour, str_pass );
            }
        }
    }

    o = igrd[you.x_pos][you.y_pos];

    if (o == NON_ITEM)
    {
        mpr("There are no items here.");
    }
    else if (mitm[o].link == NON_ITEM)      // just one item?
    {
        mitm[o].flags &= ~(ISFLAG_THROWN | ISFLAG_DROPPED);

        num = move_item_to_player( o, mitm[o].quantity );

        if (num == -1)
            mpr("You can't carry that many items.");
        else if (num == 0)
            mpr("You can't carry that much weight.");
    }                           // end of if items_here
    else
    {
        mpr("There are several objects here.");

        while (o != NON_ITEM)
        {
            next = mitm[o].link;

            if (keyin != 'a')
            {
                strcpy(info, "Pick up ");

                if (mitm[o].base_type == OBJ_GOLD)
                {
                    char st_prn[20];
                    itoa(mitm[o].quantity, st_prn, 10);
                    strcat(info, st_prn);
                    strcat(info, " gold piece");

                    if (mitm[o].quantity > 1)
                        strcat(info, "s");
                }
                else
                {
                    it_name(o, DESC_NOCAP_A, str_pass);
                    strcat(info, str_pass);
                }

                strcat(info, "\? (y,n,a,*,q)");
                mpr(  MSGCH_PROMPT, info );

                keyin = get_ch();
            }

            if (keyin == '*' || keyin == '?' || keyin == ',')
            {
                pickup_menu(o);
                break;
            }

            if (keyin == 'q')
                break;

            if (keyin == 'y' || keyin == 'a')
            {
                mitm[o].flags &= ~(ISFLAG_THROWN | ISFLAG_DROPPED);

                const int result = move_item_to_player( o, mitm[o].quantity );

                if (result == 0)
                {
                    mpr("You can't carry that much weight.");
                    keyin = 'x';        // resets from 'a'
                }
                else if (result == -1)
                {
                    mpr("You can't carry that many items.");
                    break;
                }
            }

            o = next;
        }
    }
}                               // end pickup()

static bool is_stackable_item( const item_def &item )
{
    ASSERT( is_valid_item( item ) );

    bool ret = false;

    switch (item.base_type)
    {
    case OBJ_MISSILES:
    case OBJ_SCROLLS:
    case OBJ_POTIONS:
        ret = true;
        break;

    case OBJ_FOOD:
        ret = (item.sub_type != FOOD_CHUNK);
        break;

    case OBJ_MISCELLANY:
        ret = (item.sub_type == MISC_RUNE_OF_ZOT);
        break;

    default:
        break;
    }

    return (ret);
}

bool items_stack( const item_def &item1, const item_def &item2 )
{
    // must check if items are valid!
    if (!is_valid_item( item1 ) || !is_valid_item( item2 ))
        return (false);

    // base and sub-types must always be the same to stack
    if (item1.base_type != item2.base_type || item1.sub_type != item2.sub_type)
        return (false);

    // items must be stackable
    if (!is_stackable_item( item1 ) || !is_stackable_item( item2 ))
        return (false);

    // These classes also require pluses and special
    if (item1.base_type == OBJ_MISSILES
         || item1.base_type == OBJ_MISCELLANY)  // only runes
    {
        if (item1.plus != item2.plus
             || item1.plus2 != item2.plus2
             || item1.special != item2.special)
        {
            return (false);
        }
    }

    // Check the flags, food/scrolls/potions don't care about the item's
    // ident status (scrolls and potions are known by identifying any
    // one of them, the individual status might not be the same).
    if (item1.base_type == OBJ_FOOD
        || item1.base_type == OBJ_SCROLLS
        || item1.base_type == OBJ_POTIONS)
    {
        if ((item1.flags & ~ISFLAG_IDENT_MASK)
            != (item2.flags & ~ISFLAG_IDENT_MASK))
        {
            return (false);
        }

        // Partially eaten items CANNOT stack currently... the delay
        // stopping code assumes that the stack of items started whole
        // and will reset the remaining items to such!
        if (item1.base_type == OBJ_FOOD && (item1.plus2 > 0 || item2.plus2 > 0))
            return (false);

        // Thanks to mummy cursing, we can have potions of decay
        // that don't look alike... so we don't stack potions
        // if either isn't identified and they look different.  -- bwr
        if (item1.base_type == OBJ_POTIONS
            && item1.special != item2.special
            && (!item_ident( item1, ISFLAG_KNOW_TYPE )
                || !item_ident( item2, ISFLAG_KNOW_TYPE )))
        {
            return (false);
        }

        // Scrolls of random uselessness now have the power of delusion.
        const int ru_id = get_ident_type( OBJ_SCROLLS, SCR_RANDOM_USELESSNESS );

        if (item1.base_type == OBJ_SCROLLS
            && item1.sub_type == SCR_RANDOM_USELESSNESS
            && (ru_id != ID_KNOWN_TYPE
                && (item1.plus != item2.plus || item1.special != item2.special)))
        {
            return (false);
        }
    }
    else if (item1.flags != item2.flags)
    {
        return (false);
    }

    return (true);
}

// Returns quantity of items moved into player's inventory and -1 if
// the player's inventory is full.
int move_item_to_player( int obj, int quant_got, bool quiet )
{
    int mass = 0;
    int unit_mass = 0;
    int retval = quant_got;
    char brek = 0;
    bool partialPickup = false;

    int m = 0;

    // Gold has no mass, so we handle it first.
    if (mitm[obj].base_type == OBJ_GOLD)
    {
        you.gold += quant_got;
        dec_mitm_item_quantity( obj, quant_got );
        set_redraw_status( REDRAW_GOLD );

        if (!quiet)
        {
            mpr( MSGCH_FLOOR_ITEMS, YELLOW, "You pick up %d gold piece%s.",
                 quant_got, (quant_got > 1) ? "s" : "" );
        }

        you.turn_is_over = true;
        return (retval);
    }

    unit_mass = item_mass( mitm[obj] );

    if (quant_got > mitm[obj].quantity || quant_got <= 0)
        quant_got = mitm[obj].quantity;

    mass = unit_mass * quant_got;
    brek = 0;

    // multiply both constants * 10

    if (you.burden + mass > carrying_capacity())
    {
        // calculate quantity we can actually pick up
        int part = (carrying_capacity() - you.burden) / unit_mass;

        if (part < 1)
            return (0);

        // only pickup 'part' items
        quant_got = part;
        partialPickup = true;

        retval = part;
    }

    if (is_stackable_item( mitm[obj] ))
    {
        for (m = 0; m < ENDOFPACK; m++)
        {
            if (items_stack( you.inv[m], mitm[obj] ))
            {
                if (!quiet && partialPickup)
                    mpr("You can only carry some of what is here.");

                inc_inv_item_quantity( m, quant_got );
                dec_mitm_item_quantity( obj, quant_got );
                burden_change();

                if (!quiet)
                {
                    in_name( m, DESC_INVENTORY, info );
                    mpr( MSGCH_FLOOR_ITEMS, you.inv[m].colour, info );
                }

                you.turn_is_over = true;

                return (retval);
            }
        }                           // end of for m loop.
    }

    // can't combine, check for slot space
    if (inv_count() >= ENDOFPACK)
        return (-1);

    if (!quiet && partialPickup)
        mpr("You can only carry some of what is here.");

    // search for empty slot:
    m = get_inv_slot( mitm[obj].slot );

    if (m == ENDOFPACK)
        retval = -1;
    else
    {
        you.inv[m] = mitm[obj];         // copy item

        you.inv[m].x = -1;
        you.inv[m].y = -1;
        you.inv[m].link = NON_ITEM;
        you.inv[m].slot = m;

        you.inv[m].quantity = quant_got;
        dec_mitm_item_quantity( obj, quant_got );
        burden_change();

        if (!quiet)
        {
            in_name( m, DESC_INVENTORY, info );
            mpr( MSGCH_FLOOR_ITEMS, you.inv[m].colour, info );
        }

        if (you.inv[m].base_type == OBJ_ORBS
            && you.char_direction == DIR_DESCENDING)
        {
            if (!quiet)
                mpr("Now all you have to do is get back out of the dungeon!");

            you.char_direction = DIR_ASCENDING;

            // record the total_skill_points when the Orb was taken.
            you.inv[m].special = you.total_skill_points;
        }
    }

    you.turn_is_over = true;

    return (retval);
}                               // end move_item_to_player()


// Moves mitm[obj] to (x,y)... will modify the value of obj to
// be the index of the final object (possibly different).
//
// Done this way in the hopes that it will be obvious from
// calling code that "obj" is possibly modified.
void move_item_to_grid( int *const obj, int x, int y )
{
    // must be a valid reference to a valid object
    if (*obj == NON_ITEM || !is_valid_item( mitm[*obj] ))
        return;

    // If it's a stackable type...
    if (is_stackable_item( mitm[*obj] ))
    {
        // Look for similar item to stack:
        for (int i = igrd[x][y]; i != NON_ITEM; i = mitm[i].link)
        {
            // check if item already linked here -- don't want to unlink it
            if (*obj == i)
                return;

            if (items_stack( mitm[*obj], mitm[i] ))
            {
                // Add quantity to item already here, and dispose
                // of obj, while returning the found item. -- bwr
                inc_mitm_item_quantity( i, mitm[*obj].quantity );
                destroy_item( *obj );
                *obj = i;
                return;
            }
        }
    }

    ASSERT( *obj != NON_ITEM );

    // Need to actually move object, so first unlink from old position.
    unlink_item( *obj );

    // move item to coord:
    mitm[*obj].x = x;
    mitm[*obj].y = y;

    // link item to top of list.
    mitm[*obj].link = igrd[x][y];
    igrd[x][y] = *obj;

    return;
}

void move_item_stack_to_grid( int x, int y, int targ_x, int targ_y )
{
    // Tell all items in stack what the new coordinate is.
    for (int o = igrd[x][y]; o != NON_ITEM; o = mitm[o].link)
    {
        mitm[o].x = targ_x;
        mitm[o].y = targ_y;
    }

    igrd[targ_x][targ_y] = igrd[x][y];
    igrd[x][y] = NON_ITEM;
}


// returns quantity dropped
bool copy_item_to_grid( const item_def &item, int x_plos, int y_plos,
                        int quant_drop, bool mark_dropped )
{
    if (quant_drop == 0)
        return (false);

    // default quant_drop == -1 => drop all
    if (quant_drop < 0)
        quant_drop = item.quantity;

    // loop through items at current location
    if (is_stackable_item( item ))
    {
        for (int i = igrd[x_plos][y_plos]; i != NON_ITEM; i = mitm[i].link)
        {
            if (items_stack( item, mitm[i] ))
            {
                inc_mitm_item_quantity( i, quant_drop );
                return (true);
            }
        }
    }

    // item not found in current stack, add new item to top.
    int new_item = get_item_slot(10);
    if (new_item == NON_ITEM)
        return (false);

    // copy item
    mitm[new_item] = item;

    // set quantity, and set the item as unlinked
    mitm[new_item].quantity = quant_drop;
    mitm[new_item].x = 0;
    mitm[new_item].y = 0;
    mitm[new_item].link = NON_ITEM;

    if (mark_dropped)
    {
        mitm[new_item].flags |= ISFLAG_DROPPED;
        mitm[new_item].flags &= ~ISFLAG_THROWN;
    }

    move_item_to_grid( &new_item, x_plos, y_plos );

    return (true);
}                               // end copy_item_to_grid()


//---------------------------------------------------------------
//
// move_top_item -- moves the top item of a stack to a new
// location.
//
//---------------------------------------------------------------
bool move_top_item( int src_x, int src_y, int dest_x, int dest_y )
{
    int item = igrd[ src_x ][ src_y ];
    if (item == NON_ITEM)
        return (false);

    // Now move the item to its new possition...
    move_item_to_grid( &item, dest_x, dest_y );

    return (true);
}


//---------------------------------------------------------------
//
// drop_gold
//
//---------------------------------------------------------------
static void drop_gold(unsigned int amount)
{
    if (you.gold > 0)
    {
        if (amount > you.gold)
            amount = you.gold;

        mpr( MSGCH_FLOOR_ITEMS, YELLOW, "You drop %d gold piece%s.",
                 amount, (amount > 1) ? "s" : "" );

        // loop through items at grid location, look for gold
        int i = igrd[you.x_pos][you.y_pos];

        while(i != NON_ITEM)
        {
            if (mitm[i].base_type == OBJ_GOLD)
            {
                inc_mitm_item_quantity( i, amount );
                you.gold -= amount;
                set_redraw_status( REDRAW_GOLD );
                return;
            }

            // follow link
            i = mitm[i].link;
        }

        // place on top.
        i = get_item_slot(10);
        if (i == NON_ITEM)
        {
            mpr( "Too many items on this level, not dropping the gold." );
            return;
        }

        mitm[i].base_type = OBJ_GOLD;
        mitm[i].colour = YELLOW;
        mitm[i].quantity = amount;
        mitm[i].flags = 0;

        move_item_to_grid( &i, you.x_pos, you.y_pos );

        you.gold -= amount;
        set_redraw_status( REDRAW_GOLD );
    }
    else
    {
        mpr("You don't have any money.");
    }
}                               // end drop_gold()


bool drop_item( int item_dropped, int quant_drop )
{
    if (item_dropped == PROMPT_GOT_SPECIAL)  // ie '$' for gold
    {
        // drop gold
        if (quant_drop < 0 || quant_drop > static_cast< int >( you.gold ))
            quant_drop = you.gold;

        drop_gold( quant_drop );
        return (true);
    }

    if (quant_drop < 0 || quant_drop > you.inv[item_dropped].quantity)
        quant_drop = you.inv[item_dropped].quantity;

    if (item_dropped == you.equip[EQ_LEFT_RING]
        || item_dropped == you.equip[EQ_RIGHT_RING]
        || item_dropped == you.equip[EQ_AMULET])
    {
        mpr("You will have to take that off first.");
        return (false);
    }

    if (item_dropped == get_inv_hand_tool()
        && item_cursed( you.inv[item_dropped] ))
    {
        mpr("That object is stuck to you!");
        return (false);
    }

    for (int i = EQ_CLOAK; i <= EQ_BODY_ARMOUR; i++)
    {
        if (item_dropped == you.equip[i] && you.equip[i] != -1)
        {
            if (!Options.easy_armour)
            {
                mpr("You will have to take that off first.");
                return (false);
            }
            else
            {
                // If we take off the item, cue up the item being dropped
                if (takeoff_armour( item_dropped ))
                {
                    start_delay( DELAY_DROP_ITEM, 1, item_dropped, 1 );
                    you.turn_is_over = false; // turn happens later
                }
                return (true);
            }
        }
    }

    // Unwield needs to be done before copy in order to clear things
    // like temporary brands. -- bwr
    if (item_dropped == you.equip[EQ_WEAPON])
        unwield_item();

    if (!copy_item_to_grid( you.inv[item_dropped],
                            you.x_pos, you.y_pos, quant_drop, true ))
    {
        mpr( "Too many items on this level, not dropping the item." );
        return (false);
    }

    char str_pass[ ITEMNAME_SIZE ];
    quant_name( you.inv[item_dropped], quant_drop, DESC_NOCAP_A, str_pass );

    mpr( MSGCH_FLOOR_ITEMS, you.inv[item_dropped].colour,
            "You drop %s.", str_pass );

    dec_inv_item_quantity( item_dropped, quant_drop );
    alert_nearby_monsters();
    you.turn_is_over = true;

    return (true);
}

static std::string drop_menu_title( int menuflags, const std::string &oldt )
{
    std::string res = oldt;

    res.erase( 0, res.find_first_not_of(" \n\t") );

    if (menuflags & MF_MULTISELECT)
        res = "[Multidrop] " + res;

    return ("  " + res);
}

//---------------------------------------------------------------
//
// do_drop
//
// Prompts the user for an item to drop
//
//---------------------------------------------------------------
bool do_drop(void)
{
    if (inv_count() < 1 && you.gold == 0)
    {
        canned_msg( MSG_NOTHING_CARRIED );
        return (false);
    }

    static std::vector<SelItem> selected;

    if (!you.activity || selected.empty())
    {
        selected = prompt_invent_items( "Drop which item?", -1,
                                        drop_menu_title,
                                        true, true, '$',
                                        &Options.drop_filter );
    }

    if (selected.empty())
    {
        canned_msg( MSG_OK );
        return (false);
    }

    drop_item( selected[0].slot, selected[0].quantity );

    // Forget the item we just dropped
    selected.erase( selected.begin() );

    // If we still have items that want to be dropped, start the multiturn
    // activity
    you.activity = (selected.empty()) ? ACT_NONE : ACT_MULTIDROP;

    return (true);
}                               // end do_drop()

//---------------------------------------------------------------
//
// shift_monster
//
// Moves a monster to approximately (x,y) and returns true
// if monster was moved.
//
//---------------------------------------------------------------
bool shift_monster( struct monsters *mon, int x, int y )
{
    bool found_move = false;

    int i, j;
    int tx, ty;
    int nx = 0, ny = 0;

    int count = 0;

    if (x == 0 && y == 0)
    {
        // try and find a random floor space some distance away
        for (i = 0; i < 50; i++)
        {
            random_in_bounds( tx, ty, DNGN_FLOOR, true );

            if (grid_distance( mon->x, mon->y, tx, ty ) > 10)
                break;
        }

        if (i == 50)
            return (false);
    }

    for (i = -1; i <= 1; i++)
    {
        for (j = -1; j <= 1; j++)
        {
            tx = x + i;
            ty = y + j;

            if (!in_bounds( tx, ty ))
                continue;

            // won't drop on anything but vanilla floor right now
            if (grd[tx][ty] != DNGN_FLOOR)
                continue;

            if (mgrd[tx][ty] != NON_MONSTER)
                continue;

            if (tx == you.x_pos && ty == you.y_pos)
                continue;

            count++;
            if (one_chance_in(count))
            {
                nx = tx;
                ny = ty;
                found_move = true;
            }
        }
    }

    if (found_move)
        move_monster_to_grid( monster_index( mon ), nx, ny );

    return (found_move);
}

//---------------------------------------------------------------
//
// update_corpses
//
// Update all of the corpses and food chunks on the floor. (The
// elapsed time is a double because this is called when we re-
// enter a level and a *long* time may have elapsed).
//
// XXX: However it's never been verified that a double is really
// needed... there's also the issue that the time values are
// saved out as single precision floats anyways!
//
//---------------------------------------------------------------
void update_corpses( double elapsedTime )
{
    int cx, cy;

    if (elapsedTime <= 0.0)
        return;

    const unsigned long rot_time = static_cast<unsigned long>( elapsedTime / 20.0 );

    for (int c = 0; c < MAX_ITEMS; c++)
    {
        if (mitm[c].quantity < 1)
            continue;

        if (mitm[c].base_type != OBJ_CORPSES && mitm[c].base_type != OBJ_FOOD)
            continue;

        if (mitm[c].base_type == OBJ_FOOD && mitm[c].sub_type != FOOD_CHUNK)
            continue;

        // XXX: is this possible? what does it mean?
        if (mitm[c].base_type == OBJ_CORPSES
            && mitm[c].sub_type > CORPSE_SKELETON)
        {
            continue;
        }

        if (rot_time >= mitm[c].special)
        {
            if (mitm[c].base_type == OBJ_FOOD)  // chunk
                destroy_item(c);
            else                                // body
            {
                if (mitm[c].sub_type == CORPSE_SKELETON
                    || !mons_skeleton( mitm[c].plus ))
                {
                    destroy_item(c);
                }
                else
                {
                    mitm[c].sub_type = CORPSE_SKELETON;
                    mitm[c].special = 200;
                    mitm[c].colour = LIGHTGREY;
                }
            }
        }
        else
        {
            ASSERT( rot_time < 256 );
            mitm[c].special -= rot_time;

            // change fleshy corpses/meat chunks colour when rotten
            if (mitm[c].special < 100
                && (mitm[c].base_type == OBJ_FOOD
                    || mitm[c].sub_type == CORPSE_BODY)
                && Options.colour_rotten != BLACK)
            {
                mitm[c].colour = Options.colour_rotten;
            }
        }
    }


    int fountain_checks = static_cast<int>( elapsedTime / 1000.0 );
    if (random2(1000) < static_cast<int>( elapsedTime ) % 1000)
        fountain_checks += 1;

    // dry fountains may start flowing again
    if (fountain_checks > 0)
    {
        for (cx = X_BOUND_1 + 1; cx < X_BOUND_2; cx++)
        {
            for (cy = Y_BOUND_1 + 1; cy < Y_BOUND_2; cy++)
            {
                if (grd[cx][cy] > DNGN_SPARKLING_FOUNTAIN
                    && grd[cx][cy] < DNGN_PERMADRY_FOUNTAIN)
                {
                    for (int i = 0; i < fountain_checks; i++)
                    {
                        if (one_chance_in(100))
                        {
                            if (grd[cx][cy] > DNGN_SPARKLING_FOUNTAIN)
                                grd[cx][cy]--;
                        }
                    }
                }
            }
        }
    }
}

//---------------------------------------------------------------
//
// update_enchantments
//
// Update a monster's enchantments when the player returns
// to the level.
//
// Management for enchantments... problems with this are the oddities
// (monster dying from poison several thousands of turns later), and
// game balance.
//
// Consider: Poison/Sticky Flame a monster at range and leave, monster
// dies but can't leave level to get to player (implied game balance of
// the delayed damage is that the monster could be a danger before
// it dies).  This could be fixed by keeping some monsters active
// off level and allowing them to take stairs (a very serious change).
//
// Compare this to the current abuse where the player gets
// effectively extended duration of these effects (although only
// the actual effects only occur on level, the player can leave
// and heal up without having the effect disappear).
//
// This is a simple compromise between the two... the enchantments
// go away, but the effects don't happen off level.  -- bwr
//
// This function returns true if monster still exists.
//
//---------------------------------------------------------------
static bool update_enchantments( struct monsters *mon, int turns )
{
    const int time_passed = 10 * turns;

    if (mons_flag( mon, MF_SHORT_LIVED ))
    {
        monster_die( mon, KILL_RESET, MHITNOT );
        return (false);
    }

    for (int i = 0; i < NUM_MON_ENCHANTS; i++)
    {
        mon_enchant_def *const ench = &(mon->ench[i]);

        // quietly decrement the enchantments
        if (ench->type != ENCH_NONE
            && mons_dec_ench_ptr( mon, ench, time_passed, true ))
        {
            switch (ench->type)
            {
            case ENCH_TELEPORT:
                monster_teleport( mon, 0, true, true );
                break;

            case ENCH_CONFUSION:
                monster_blink( mon, false, true );  // pretend walking around
                break;

            default:
                break;
            }
        }
    }

    return (true);
}


//---------------------------------------------------------------
//
// update_level
//
// Update the level when the player returns to it.
//
//---------------------------------------------------------------
void update_level( double elapsedTime )
{
    const int turns = ((elapsedTime > 20000.0)
                            ? 2000 : static_cast<int>( elapsedTime / 10.0 ));
    int m, i;

#if DEBUG_DIAGNOSTICS
    int mons_total = 0;

    mpr( MSGCH_DIAGNOSTICS, "turns: %d", turns );
#endif

    for (m = 0; m < MAX_MONSTERS; m++)
    {
        struct monsters *mon = &menv[m];

        if (mon->type == -1)
            continue;

        if (testbits( mon->flags, MF_TAKING_STAIRS ))
        {
            // just making sure that following monsters don't get lost:
            mon->behaviour = BEH_SEEK;
            continue;
        }

#if DEBUG_DIAGNOSTICS
        mons_total++;
#endif

        // update enchantments and continue if monster no longer exists
        if (!update_enchantments( mon, turns ) || mon->type == -1)
            continue;

        const bool targ_you = (mon->foe == MHITYOU && !mons_friendly(mon));
        const int  dist = grid_distance( mon->x, mon->y, you.x_pos, you.y_pos );
        const bool pnear = (dist <= 8);

        // off level spellcasting simulation... not allowing friendly
        // monsters to cast while the player is off level to avoid any
        // possible abuses.
        if (mons_class_flag( mon->type, M_SPELLCASTER ) && !mons_friendly( mon ))
        {
            const bool healthy = (mon->hit_points > mon->max_hit_points / 2);
            const bool stealthy = !check_awaken(m);

            // for passing to mons_cast which will set it
            struct bolt dummy;

#if DEBUG_DIAGNOSTICS
            mpr( MSGCH_DIAGNOSTICS,
                  "spells: %s; targ_you=%d; near=%d; health=%s; stealth=%s",
                  ptr_monam( mon, DESC_PLAIN ),
                  targ_you, pnear, (healthy ? "yes" : "no"),
                  (stealthy ? "yes" : "no") );
#endif

            // things to consider if the player is near:
            if (targ_you && pnear)
            {
                if (!healthy && (mons_has_teleport_spell(mon) != MS_NO_SPELL))
                    monster_teleport( mon, 0, true, true );

                // XXX: monster animate dead is currently player centered:
                if (mons_has_spell( mon, MS_ANIMATE_DEAD ))
                    mons_cast( mon, dummy, MS_ANIMATE_DEAD );
            }

            // things to cast in preparation for the player's soon arrival:
            if (targ_you && (!healthy || (pnear && !stealthy)))
            {
                if (mons_has_spell( mon, MS_INVIS ))
                    mons_add_ench( mon, ENCH_INVIS );

                if (mons_has_spell( mon, MS_HASTE ))
                    mons_add_ench( mon, ENCH_HASTE );

                const int summon = mons_has_summon_spell( mon );

                if (summon != MS_NO_SPELL)
                {
#if DEBUG_DIAGNOSTICS
                    mpr( MSGCH_DIAGNOSTICS, "... summon" );
#endif
                    mons_cast( mon, dummy, summon );
                }
            }

            // healers are always top upped when we arrive
            if (mons_has_spell( mon, MS_HEAL ))
                mon->hit_points = mon->max_hit_points;
        }

        // hitpoint recovery
        if (mon->hit_points < mon->max_hit_points)
        {
            if (mon->type == MONS_PLAYER_GHOST)
                mon->hit_points = mon->max_hit_points;
            else if (mons_class_flag( mon->type, M_REGEN ))
                heal_monster( mon, turns, false );
            else
                heal_monster( mon, turns / 5, false );
        }

        // Don't move water or lava monsters around, let sleeping monsters lie
        if (monster_habitat( mon->type ) != DNGN_FLOOR
            || mons_stationary( mon->type )
            || mon->behaviour == BEH_SLEEP)
        {
            continue;
        }

        const int range = (turns * mons_speed( mon )) / 10;
        const int moves = (range > 50) ? 50 : range;

        // const bool short_time = (range >= 5 + random2(10));
        const bool long_time  = (range >= (500 + roll_dice( 2, 500 )));

        const bool ranged_attack = (mons_has_ranged_spell( mon )
                                    || mons_has_ranged_attack( mon ));

#if DEBUG_DIAGNOSTICS
        // probably too annoying even for DEBUG_DIAGNOSTICS
        mpr( MSGCH_DIAGNOSTICS,
              "mon #%d: range %d; long %d; pos (%d,%d); targ %d(%d,%d); flags %d",
              m, range, long_time, mon->x, mon->y,
              mon->foe, mon->target_x, mon->target_y, mon->flags );

#endif

        if (range <= 0)
            continue;

        // check to see if friends went feral
        if (long_time && mon->attitude == ATT_FRIENDLY)
            mon->attitude = ATT_HOSTILE;

        bool back_off = false;

        if (long_time
            && (mon->behaviour == BEH_FLEE
                || mon->behaviour == BEH_CORNERED
                || mon->behaviour == BEH_PANIC
                || mon->behaviour == BEH_INVESTIGATE
                || mons_is_batty( mon )
                || ranged_attack
                || coinflip()))
        {
            if (mon->behaviour != BEH_WANDER)
            {
                mon->behaviour = BEH_WANDER;
                mon->foe = MHITNOT;
                set_wander_target( mon );
            }
            else
            {
                // monster will be sleeping after we move it
                mon->behaviour = BEH_SLEEP;
            }
        }
        else if (ranged_attack)
        {
            // if we're doing short time movement and the monster has a
            // ranged attack (missile or spell), then the monster will
            // flee to gain distance if it's "too close", else it will
            // just shift its position rather than charge the player. -- bwr
            if (targ_you && dist < 3)
            {
                back_off = true;

                // flee from player's position if different
                mon->target_x = you.x_pos;
                mon->target_y = you.y_pos;

#if DEBUG_DIAGNOSTICS
                mpr( MSGCH_DIAGNOSTICS, "backing off..." );
#endif
            }
            else
            {
                shift_monster( mon );

#if DEBUG_DIAGNOSTICS
                mpr( MSGCH_DIAGNOSTICS, "shifted to (%d,%d)", mon->x, mon->y );
#endif
                continue;
            }
        }

        int pos_x = mon->x, pos_y = mon->y;

        // dirt simple movement:
        for (i = 0; i < moves; i++)
        {
            int mx = sgn( mon->target_x - pos_x );
            int my = sgn( mon->target_y - pos_y );

            if (back_off)
            {
                mx *= -1;
                my *= -1;
            }

            if (!in_bounds( pos_x + mx, pos_y + my ))
            {
                mx = 0;
                my = 0;
            }

            if (mx == 0 && my == 0)
                break;

            if (grd[pos_x + mx][pos_y + my] < DNGN_FLOOR)
                break;

            pos_x += mx;
            pos_y += my;
        }

        // if we can't find a space need the target, shift at current location
        if (!shift_monster( mon, pos_x, pos_y ))
            shift_monster( mon );

#if DEBUG_DIAGNOSTICS
        mpr( MSGCH_DIAGNOSTICS, "moved to (%d,%d)", mon->x, mon->y );
#endif
    }

#if DEBUG_DIAGNOSTICS
    mpr( MSGCH_DIAGNOSTICS, "total monsters on level = %d", mons_total );
#endif

    // done afterwards in case monster animated
    update_corpses( elapsedTime );

    for (i = 0; i < MAX_CLOUDS; i++)
    {
        if (env.cloud[i].type == CLOUD_NONE)
            continue;

        if (turns * 10 >= env.cloud[i].decay)
            delete_cloud(i);
        else
            env.cloud[i].decay -= turns * 10;
    }
}


//---------------------------------------------------------------
//
// handle_time
//
// Do various time related actions...
// This function is called about every 20 turns.
//
//---------------------------------------------------------------
void handle_time( unsigned int time_delta )
{
    int temp_rand;              // probability determination {dlb}

    // so as not to reduplicate f(x) calls {dlb}
    unsigned char which_miscast = SPTYP_RANDOM;

    bool summon_instead;        // for branching within a single switch {dlb}
    int which_beastie = MONS_PROGRAM_BUG;       // error trapping {dlb}
    unsigned char i;            // loop variable {dlb}
    bool new_rotting_item = false; //mv: becomes true when some new item becomes rotting

    // BEGIN - Nasty things happen to people who spend too long in Hell:
    if (player_in_hell() && coinflip())
    {
        temp_rand = random2(17);

        mpr( MSGCH_TALK,
            (temp_rand == 0) ? "\"You will not leave this place.\"" :
            (temp_rand == 1) ? "\"Die, mortal!\"" :
            (temp_rand == 2) ? "\"We do not forgive those who trespass against us!\"" :
            (temp_rand == 3) ? "\"Trespassers are not welcome here!\"" :
            (temp_rand == 4) ? "\"You do not belong in this place!\"" :
            (temp_rand == 5) ? "\"Leave now, before it is too late!\"" :
            (temp_rand == 6) ? "\"We have you now!\"" :
            (temp_rand == 7) ? "You feel a terrible foreboding..." :
            (temp_rand == 8) ? "You hear words spoken in a strange and terrible language..." :

            (temp_rand == 9) ? ((player_can_smell()) ? "You smell brimstone."
                                            : "Brimstone rains from above.") :

            (temp_rand == 10) ? "Something frightening happens." :
            (temp_rand == 11) ? "You sense an ancient evil watching you..." :
            (temp_rand == 12) ? "You feel lost and a long, long way from home..." :
            (temp_rand == 13) ? "You suddenly feel all small and vulnerable." :
            (temp_rand == 14) ? "A gut-wrenching scream fills the air!" :
            (temp_rand == 15) ? "You shiver with fear." :
            (temp_rand == 16) ? "You sense a hostile presence."
                              : "You hear diabolical laughter!" );

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

            miscast_effect( which_miscast, 4 + random2(6), roll_dice(3,33),
                            100, "the effects of Hell" );
        }
        else if (temp_rand > 7) // 10 in 27 odds {dlb}
        {
            // 60:40 miscast:summon split {dlb}
            summon_instead = (random2(5) > 2);

            switch (you.where_are_you)
            {
            case BRANCH_DIS:
                if (summon_instead)
                    which_beastie = rand_demon(DEMON_GREATER);
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
                create_monster( which_beastie );
            else
            {
                miscast_effect( which_miscast, 4 + random2(6), roll_dice(3,33),
                                100, "the effects of Hell" );
            }
        }

        // NB: no "else" - 8 in 27 odds that nothing happens through
        // first chain {dlb}
        // also note that the following is distinct from and in
        // addition to the above chain:

        // try to summon at least one and up to five random monsters {dlb}
        if (one_chance_in(3))
        {
            create_monster( RANDOM_MONSTER );

            for (i = 0; i < 4; i++)
            {
                if (one_chance_in(3))
                    create_monster( RANDOM_MONSTER );
            }
        }
    }
    // END - special Hellish things...

    // Adjust the player's stats if s/he's diseased (or recovering).
    if (you.disease)
    {
        if (one_chance_in(30))
        {
            mpr(MSGCH_WARN,"Your disease is taking its toll." );
            lose_stat(STAT_RANDOM, 1);
        }
    }
    else if (!you.is_undead)
    {
        if (you.str < you.max_str && one_chance_in(100))
        {
            mpr(MSGCH_RECOVERY,"You feel your strength returning." );
            you.str++;
            set_redraw_status( REDRAW_STRENGTH );
        }

        if (you.dex < you.max_dex && one_chance_in(100))
        {
            mpr(MSGCH_RECOVERY,"You feel your dexterity returning." );
            you.dex++;
            set_redraw_status( REDRAW_DEXTERITY );
        }

        if (you.intel < you.max_intel && one_chance_in(100))
        {
            mpr(MSGCH_RECOVERY,"You feel your intelligence returning." );
            you.intel++;
            set_redraw_status( REDRAW_INTELLIGENCE );
        }
    }

    if (you.duration[DUR_GLAMOUR] > 0)
        you.duration[DUR_GLAMOUR]--;


    // Adjust the player's stats if s/he has the deterioration mutation
    if (you.mutation[MUT_DETERIORATION]
        && random2(200) <= you.mutation[MUT_DETERIORATION] * 5 - 2)
    {
        lose_stat( STAT_RANDOM, 1 );
    }

    int added_contamination = 0;

    // Account for mutagenic radiation.  Invis and haste will give the
    // player about .1 points per turn,  mutagenic randarts will give
    // about 1.5 points on average,  so they can corrupt the player
    // quite quickly.  Wielding one for a short battle is OK,  which is
    // as things should be.   -- GDL
    if (you.invis && random2(10) < 6)
        added_contamination++;

    if (you.haste && random2(10) < 6)
        added_contamination++;

    // randarts are usually about 20x worse than running around invisible
    // or hasted.. this seems OK.
    added_contamination += random2( 1 + scan_randarts( RAP_MUTAGENIC ) );

    // we take off about .5 points per turn
    if (!you.invis && !you.haste && coinflip())
        added_contamination -= 1;

    contaminate_player( added_contamination );

    // only check for badness once every other turn
    if (you.magic_contamination >= 5 && random2(150) <= you.magic_contamination)
    {
        int reduc = -1;
        mpr( MSGCH_WARN, "Your body shudders with the violent release of wild energies!" );

        // for particularly violent releases,  make a little boom
        if (you.magic_contamination > 25 && one_chance_in(3))
        {
            struct bolt boom;
            boom.type = SYM_BURST;
            boom.colour = element_colour( EC_MUTAGENIC );
            boom.flavour = BEAM_RANDOM;
            boom.target_x = you.x_pos;
            boom.target_y = you.y_pos;
            boom.damage = dice_def( 3, (you.magic_contamination / 2) );
            boom.thrower = KILL_MISC;
            boom.aux_source = "a magical explosion";
            boom.beam_source = MHITNOT;
            boom.is_beam = false;
            boom.is_enchant = false;
            boom.is_tracer = false;
            strcpy( boom.name, "magical storm" );

            boom.ench_power = (you.magic_contamination * 5);
            boom.ex_size = (you.magic_contamination / 15);
            if (boom.ex_size > 9)
                boom.ex_size = 9;

            explosion(boom);
            reduc -= 3;
        }

        // we want to warp the player,  not do good stuff!
        if (one_chance_in(5))
            mutate( PICK_RANDOM_MUTATION );
        else
            reduc -= 2 * (give_bad_mutation( coinflip() ) != MUT_NONE);

        contaminate_player( reduc );
    }

    // Random chance to identify staff in hand based off of Spellcasting
    // and an appropriate other spell skill... is 1/20 too fast?
    const int staff = get_inv_hand_tool();

    if (staff != -1
        && item_is_staff( you.inv[staff] )
        && !item_ident( you.inv[staff], ISFLAG_KNOW_TYPE )
        && one_chance_in(20))
    {
        int total_skill = you.skills[SK_SPELLCASTING];

        switch (you.inv[staff].sub_type)
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
        case STAFF_SUMMONING:
            total_skill += you.skills[SK_SUMMONINGS];
            break;
        }

        if (random2(100) < total_skill)
        {
            set_ident_flags( you.inv[staff], ISFLAG_IDENT_MASK );

            char str_pass[ITEMNAME_SIZE];

            in_name( staff, DESC_NOCAP_A, str_pass );
            mpr( "You are wielding %s.", str_pass );
            more();

            set_redraw_status( REDRAW_WIELD );
        }
    }

    // Check to see if an upset god wants to do something to the player
    // jmf: moved huge thing to religion.cc
    handle_god_time();

    // If the player has the lost mutation forget portions of the map
    if (you.mutation[MUT_LOST])
    {
        if (random2(100) <= you.mutation[MUT_LOST] * 5)
            forget_map( 5 + random2(you.mutation[MUT_LOST] * 10)  );
    }

    // Update all of the corpses and food chunks on the floor
    update_corpses( static_cast<double>(time_delta) );

    // Update all of the corpses and food chunks in the player's inventory
    const unsigned int corpse_time = time_delta / 20;
    bool reduced_weight = false;

    for (i = 0; i < ENDOFPACK; i++)
    {
        if (you.inv[i].quantity < 1)
            continue;

        if (you.inv[i].base_type == OBJ_CORPSES)
        {
            // carried skeletons don't fall apart
            if (you.inv[i].sub_type >= CORPSE_SKELETON)
                continue;
        }
        else if (you.inv[i].base_type == OBJ_FOOD)
        {
            if (you.inv[i].sub_type != FOOD_CHUNK)
                continue;
        }
        else
        {
            continue;
        }

        if (corpse_time >= you.inv[i].special)
        {
            reduced_weight = true;

            if (you.inv[i].base_type == OBJ_FOOD)
            {
                dec_inv_item_quantity( i, you.inv[i].quantity );
                continue;
            }

            if (!mons_skeleton( you.inv[i].plus ))
            {
                dec_inv_item_quantity( i, you.inv[i].quantity );
                continue;
            }

            you.inv[i].sub_type = CORPSE_SKELETON;
            you.inv[i].special = 200;
            you.inv[i].colour = LIGHTGREY;

            if (you.equip[EQ_WEAPON] == i)
                set_redraw_status( REDRAW_WIELD );

            continue;
        }

        you.inv[i].special -= corpse_time;

        if (you.inv[i].special < 100
            && (you.inv[i].special + corpse_time >= 100))
        {
            new_rotting_item = true;

            if (you.equip[EQ_WEAPON] == i)
                set_redraw_status( REDRAW_WIELD );

            if (Options.colour_rotten != BLACK)
                you.inv[i].colour = Options.colour_rotten;
        }
    }

    //mv: messages when chunks/corpses become rotten
    if (new_rotting_item)
    {
        switch (you.species)
        {
        // XXX: should probably still notice?
        case SP_MUMMY: // no smell
        case SP_TROLL: // stupid, living in mess - doesn't care about it
            break;

        case SP_GHOUL: //likes it
            temp_rand = random2(8);
            mpr( MSGCH_ROTTEN_MEAT,
                  ((temp_rand  < 5) ? "You smell something rotten." :
                  (temp_rand == 5) ? "Smell of rotting flesh makes you more hungry." :
                  (temp_rand == 6) ? "You smell decay.  Yum-yum."
                                   : "Wow!  There is something tasty in your inventory.") );
            break;

        case SP_KOBOLD: //mv: IMO these race aren't so "touchy"
        case SP_OGRE:
        case SP_MINOTAUR:
        case SP_HILL_ORC:
            temp_rand = random2(8);
            mpr( MSGCH_ROTTEN_MEAT,
                  ((temp_rand  < 5) ? "You smell something rotten." :
                  (temp_rand == 5) ? "You smell rotting flesh." :
                  (temp_rand == 6) ? "You smell decay."
                                   : "There is something rotten in your inventory.") );
            break;

        default:
            temp_rand = random2(8);
            mpr( MSGCH_ROTTEN_MEAT,
                  ((temp_rand  < 5) ? "You smell something rotten." :
                  (temp_rand == 5) ? "Smell of rotting flesh makes you sick." :
                  (temp_rand == 6) ? "You smell decay.  Yuck..."
                                   : "Ugh!  There is something really disgusting in your inventory.") );
            break;
        }
    }

    if (reduced_weight)
        mpr(MSGCH_ROTTEN_MEAT,"Your equipment suddenly weighs less." );

    // exercise armour *xor* stealth skill: {dlb}
    const int arm_mass = (you.equip[EQ_BODY_ARMOUR] != -1)
                        ? item_mass( you.inv[you.equip[EQ_BODY_ARMOUR]] ) : 0;

    if (!player_light_armour())
    {
        exercise_armour( 1, 6 );
    }
    else                        // exercise stealth skill:
    {
        if (you.burden_state != BS_UNENCUMBERED || you.berserker)
            return;

        if (you.special_wield == SPWLD_SHADOW)
            return;

        if (random2( arm_mass ) >= 100)
            return;

        if (one_chance_in(12))
            exercise( SK_STEALTH, 1 );
    }

    return;
}                               // end handle_time()

static bool is_banned(const item_def &item)
{
    static char name[ITEMNAME_SIZE];

    item_name( item, DESC_INVENTORY, name, false );

    std::string iname = name;
    for (unsigned int i = 0; i < Options.banned_objects.size(); ++i)
    {
        if (iname.find( Options.banned_objects[i], 0 ) != std::string::npos)
            return (true);
    }

    return (false);
}

static void do_autopickup(void)
{
    //David Loewenstern 6/99
    int result, o, next;
    bool did_pickup = false;

    if (you.stepping)
        return;

    if (Options.autopickups == 0L || !you.autopickup_on)
        return;

    if (you.attribute[ATTR_TRANSFORMATION] == TRAN_AIR
        && you.duration[DUR_TRANSFORMATION] > 0)
    {
        return;
    }

    if (!player_access_floor())
        return;

    o = igrd[you.x_pos][you.y_pos];

    while (o != NON_ITEM)
    {
        next = mitm[o].link;

        if (((mitm[o].flags & ISFLAG_THROWN) && Options.pickup_thrown)
            || (Options.autopickups & (1L << mitm[o].base_type)
                && (Options.pickup_dropped || !(mitm[o].flags & ISFLAG_DROPPED))
                && !is_banned(mitm[o])))
        {
            mitm[o].flags &= ~(ISFLAG_THROWN | ISFLAG_DROPPED);

            result = move_item_to_player( o, mitm[o].quantity);

            if (result == 0)
            {
                mpr("You can't carry any more.");
                break;
            }
            else if (result == -1)
            {
                mpr("Your pack is full.");
                break;
            }

            did_pickup = true;
        }

        o = next;
    }

    // move_item_to_player should leave things linked. -- bwr
    // relink_cell(you.x_pos, you.y_pos);

    if (did_pickup)
    {
        you.turn_is_over = true;
        start_delay( DELAY_AUTOPICKUP, 1 );
    }
}

int inv_count(void)
{
    int count=0;

    for(int i=0; i< ENDOFPACK; i++)
    {
        if (is_valid_item( you.inv[i] ))
            count += 1;
    }

    return count;
}

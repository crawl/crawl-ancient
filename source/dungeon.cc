/*
 *  File:       dungeon.cc
 *  Summary:    Functions used when building new levels.
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 *
 *   <8>     02-Apr-2001 gdl    cleanup; nuked all globals
 *   <7>     06-Mar-2000 bwr    reduced vorpal weapon freq,
 *                              spellbooks now hold up to eight spells.
 *   <6>     11/06/99    cdl    random3 -> random2
 *   <5>      8/08/99    BWR    Upped rarity of unique artefacts
 *   <4>      7/13/99    BWR    Made pole arms of speed.
 *   <3>      5/22/99    BWR    Made named artefact weapons
 *                              rarer, Sword of Power esp.
 *   <2>      5/09/99    LRH    Replaced some sanity checking code in
 *                              spellbook_template with a corrected version
 *                                              using ASSERTs.
 *   <1>      -/--/--    LRH    Created
 */

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "AppHdr.h"
#include "abyss.h"
#include "defines.h"
#include "enum.h"
#include "externs.h"
#include "dungeon.h"
#include "itemname.h"
#include "maps.h"
#include "mon-util.h"
#include "mon-pick.h"
#include "monplace.h"
#include "randart.h"
#include "spl-book.h"
#include "stuff.h"
#include "wpn-misc.h"

struct spec_t {
    bool created;
    bool hooked_up;
    int x1;
    int y1;
    int x2;
    int y2;
};
typedef struct spec_t spec_room;

// DUNGEON BUILDERS
static bool find_in_area(int sx, int sy, int ex, int ey, unsigned char feature);
static bool make_box(int room_x1, int room_y1, int room_x2, int room_y2,
    unsigned char floor=0, unsigned char wall=0, unsigned char avoid=0);
static void replace_area(int sx, int sy, int ex, int ey, unsigned char replace,
    unsigned char feature);
static int builder_by_type(int level_number, char level_type);
static int builder_by_branch(int level_number);
static int builder_normal(int level_number, char level_type, spec_room &s);
static int builder_basic(int level_number);
static void builder_extras(int level_number, int level_type);
static void builder_items(int level_number, char level_type, int items_wanted);
static void builder_monsters(int level_number, char level_type, int mon_wanted);
static void place_specific_stair(unsigned char stair);
static void place_branch_entrances(int dlevel, char level_type);
static bool place_specific_trap(unsigned char spec_x, unsigned char spec_y,
    unsigned char spec_type);
static void place_traps(int level_number);
static void prepare_swamp(void);
static void prepare_water(void);
static void check_doors(void);
static void hide_doors(void);
static void make_trail(int xs, int xr, int ys, int yr,int corrlength, int intersect_chance,
    int no_corr, unsigned char begin, unsigned char end=0);
static bool make_room(int sx,int sy,int ex,int ey,int max_doors, int doorlevel);
static void join_the_dots(unsigned char dotx1, unsigned char doty1,
    unsigned char dotx2, unsigned char doty2, char forbid_x1, char forbid_y1,
    char forbid_x2, char forbid_y2);
static void place_pool(unsigned char pool_type, unsigned char pool_x1,
                       unsigned char pool_y1, unsigned char pool_x2,
                       unsigned char pool_y2);
static void many_pools(unsigned char pool_type);
static void spotty_level(bool seeded, int iterations, bool boxy);
static void bigger_room(void);
static void plan_main(int level_number, char force_plan);
static char plan_1(void);
static char plan_2(void);
static char plan_3(void);
static char plan_4(char forbid_x1, char forbid_y1, char forbid_x2,
                   char forbid_y2, unsigned char force_wall);
static char plan_5(void);
static char plan_6(int level_number);
static bool octa_room(spec_room &sr, int oblique_max, unsigned char type_floor);
static void labyrinth_level(int level_number);
static void box_room(int bx1, int bx2, int by1, int by2, int wall_type);
static int box_room_doors( int bx1, int bx2, int by1, int by2, int new_doors);
static void city_level(int level_number);
static void diamond_rooms(int level_number);

// ITEM & SHOP FUNCTIONS
static void place_shops(int level_number);
static void place_spec_shop(int level_number, unsigned char shop_x,
    unsigned char shop_y, unsigned char force_s_type);
static unsigned char item_in_shop(unsigned char shop_type);
static bool treasure_area(int level_number, unsigned char ta1_x,
                          unsigned char ta2_x, unsigned char ta1_y,
                          unsigned char ta2_y);
static void link_items(void);
static void item_colour(int p);
static char rare_weapon(unsigned char w_type);
static bool is_weapon_special(int the_weapon);
static void set_weapon_special(int the_weapon, int spwpn);
static void big_room(int level_number);
static void chequerboard(spec_room &sr, unsigned char
    target,  unsigned char floor1, unsigned char floor2);
static void roguey_level(int level_number, spec_room &sr);
static void morgue(spec_room &sr);

// SPECIAL ROOM BUILDERS
static void special_room(int level_number, spec_room &sr);
static void specr_2(spec_room &sr);
static void beehive(spec_room &sr);

// VAULT FUNCTIONS
static void build_vaults(int level_number, int force_vault);
static void build_minivaults(int level_number, int force_vault);
static int vault_grid(int level_number, int vx, int vy, int altar_count,
    FixedVector < char, 7 > &acq_item_class, FixedVector < int, 7 > &mons_array,
    char vgrid, int &initial_x, int &initial_y);

// ALTAR FUNCTIONS
static int pick_an_altar(void);
static void place_altar(void);


/*
 **************************************************
 *                                                *
 *             BEGIN PUBLIC FUNCTIONS             *
 *                                                *
 **************************************************
*/

// this doesn't really 'unlink' items from the grid,  it
// just nails them down for purposes of save/reload.

void unlink_items(void)
{
    int x,y,i;

    // nails all items to the ground (i.e. sets x,y)
    for(x=0; x<GXM; x++)
    {
        for(y=0; y<GYM; y++)
        {
            i = igrd[x][y];
            while (i != NON_ITEM)
            {
                mitm.x[i] = x;
                mitm.y[i] = y;
                i = mitm.link[i];
            }
        }
    }
}


void link_items(void)
{
    int i,j;

    // first, blank igrd
    for (i = 0; i < GXM; i++)
        for (j = 0; j < GYM; j++)
            igrd[i][j] = NON_ITEM;

    // link all items on the grid, plus shop inventory,
    // DON'T link the huge pile of monster items at (0,0)

    for (i = 0; i < MAX_ITEMS; i++)
    {
        if (mitm.base_type[i] == OBJ_UNASSIGNED || mitm.quantity[i] == 0
            || mitm.y[i] == 0)
        {
            // item is not assigned,  or is monster item.  ignore.
            mitm.link[i] = NON_ITEM;
            continue;
        }

        // link to top
        mitm.link[i] = igrd[mitm.x[i]][mitm.y[i]];
        igrd[mitm.x[i]][mitm.y[i]] = i;
    }
}                               // end link_items()

void builder(int level_number, char level_type)
{
    int i;          // generic loop variable
    int x,y;        // generic map loop variables

    srandom(time(NULL));

    // blank level with DNGN_ROCK_WALL
    make_box(0,0,GXM-1,GYM-1,DNGN_ROCK_WALL,DNGN_ROCK_WALL);

    // delete all traps
    for (i = 0; i < MAX_TRAPS; i++)
        env.trap_type[i] = TRAP_UNASSIGNED;

    // delete all items
    for (i = 0; i < MAX_ITEMS; i++)
    {
        mitm.base_type[i] = OBJ_UNASSIGNED;
        mitm.sub_type[i] = 0;
        mitm.pluses[i] = 0;
        mitm.pluses2[i] = 0;
        mitm.special[i] = 0;
        mitm.quantity[i] = 0;
        mitm.colour[i] = 0;
        mitm.x[i] = 0;
        mitm.y[i] = 0;
        mitm.id[i] = 0;
        mitm.link[i] = NON_ITEM;
    }

    // reset all monsters
    for (i = 0; i < MAX_MONSTERS; i++)
        menv[i].type = -1;

    // unlink all monsters and items from the grid
    for(x=0; x<GXM; x++)
    {
        for(y=0; y<GYM; y++)
        {
            mgrd[x][y] = NON_MONSTER;
            igrd[x][y] = NON_ITEM;
        }
    }

    // reset all shops
    for (unsigned char shcount = 0; shcount < 5; shcount++)
        env.shop_type[shcount] = SHOP_UNASSIGNED;

    int skip_build;

    skip_build = builder_by_type(level_number, level_type);
    if (skip_build < 0)
        return;
    if (skip_build == 0)
    {
        skip_build = builder_by_branch(level_number);
        if (skip_build < 0)
            return;
    }

    spec_room sr = { false, false, 0, 0, 0, 0 };

    if (skip_build == 0)
    {
        // do 'normal' building.  Well, except for the swamp.
        if (you.where_are_you != BRANCH_SWAMP)
            skip_build = builder_normal(level_number, level_type, sr);

        if (skip_build == 0)
        {
            skip_build = builder_basic(level_number);
            if (skip_build == 0)
                builder_extras(level_number, level_type);
        }
    }

    // hook up the special room (if there is one,  and it hasn't
    // been hooked up already in roguey_level()
    if (sr.created && !sr.hooked_up)
       specr_2(sr);

    // now place items, monster, gates, etc.
    // stairs must exist by this point. Some items and monsters
    // already exist.

    // time to make the swamp {dlb}:
    if (you.where_are_you == BRANCH_SWAMP)
        prepare_swamp();

    // figure out how many 'normal' monsters we should place
    int mon_wanted = 0;
    if (level_type == LEVEL_ABYSS
        || you.where_are_you == BRANCH_ECUMENICAL_TEMPLE)
    {
        mon_wanted = 0;
    }
    else
    {
        mon_wanted = random2avg(29, 3);

        if (you.where_are_you == BRANCH_DIS
            || you.where_are_you == BRANCH_GEHENNA
            || you.where_are_you == BRANCH_COCYTUS
            || you.where_are_you == BRANCH_TARTARUS
            || you.where_are_you == BRANCH_HALL_OF_BLADES)
        {
            mon_wanted += random2avg(24, 3);
        }

        if (you.where_are_you == BRANCH_HALL_OF_BLADES)
            mon_wanted += random2avg(24, 3);

        // unlikely - now only possible in HoB {dlb} 10mar2000
        if (mon_wanted > 60)
            mon_wanted = 60;
    }

    place_branch_entrances(level_number, level_type);

    check_doors();

    if (you.where_are_you != BRANCH_DIS && you.where_are_you != BRANCH_VAULTS)
        hide_doors();

    if (you.where_are_you != BRANCH_ECUMENICAL_TEMPLE)
        place_traps(level_number);

    int items_wanted = random2avg(32, 3);

    if (level_number > 5 && random2(500 - level_number) <= 3)
        items_wanted += 10 + random2(90);  // rich level!

    // change pre-rock (105) to rock,  and pre-floor (106) to floor
    replace_area(0,0,GXM-1,GYM-1,DNGN_BUILDER_SPECIAL_WALL, DNGN_ROCK_WALL);
    replace_area(0,0,GXM-1,GYM-1,DNGN_BUILDER_SPECIAL_FLOOR, DNGN_FLOOR);

    // place items
    builder_items(level_number, level_type, items_wanted);

    // place monsters
    builder_monsters(level_number, level_type, mon_wanted);

    // place shops,  if appropriate
    if (level_type == LEVEL_DUNGEON
        && !(you.where_are_you == BRANCH_DIS
             || you.where_are_you == BRANCH_GEHENNA
             || you.where_are_you == BRANCH_COCYTUS
             || you.where_are_you == BRANCH_TARTARUS
             || you.where_are_you == BRANCH_LAIR
             || you.where_are_you == BRANCH_ECUMENICAL_TEMPLE))
    {
        place_shops(level_number);
    }

    // If level part of Dis -> all walls metal;
    // If part of vaults -> walls depend on level;
    // If part of crypt -> all walls stone:
    if (you.where_are_you == BRANCH_DIS
        || you.where_are_you == BRANCH_VAULTS
        || you.where_are_you == BRANCH_CRYPT)
    {
        // always the case with Dis {dlb}
        unsigned char vault_wall = DNGN_METAL_WALL;

        if (you.where_are_you == BRANCH_VAULTS)
        {
            vault_wall = DNGN_ROCK_WALL;

            if (level_number > you.branch_stairs[STAIRS_VAULTS] + 2)
                vault_wall = DNGN_STONE_WALL;
            if (level_number > you.branch_stairs[STAIRS_VAULTS] + 4)
                vault_wall = DNGN_METAL_WALL;
            if (level_number > you.branch_stairs[STAIRS_VAULTS] + 6
                && one_chance_in(10))
                vault_wall = DNGN_GREEN_CRYSTAL_WALL;
        }
        else if (you.where_are_you == BRANCH_CRYPT)
        {
            vault_wall = DNGN_STONE_WALL;
        }

        replace_area(0,0,GXM-1,GYM-1,DNGN_ROCK_WALL,vault_wall);
    }

    // Top level of branch levels - replaces up stairs
    // with stairs back to dungeon or wherever:
    for (i = 0; i< 30; i++)
    {
        if (you.branch_stairs[i] == 0)
            break;

        if (level_number == you.branch_stairs[i] + 1
            && level_type == LEVEL_DUNGEON
            && you.where_are_you == BRANCH_ORCISH_MINES + i)
        {
            for (x = 1; x < GXM; x++)
                for (y = 1; y < GYM; y++)
                    if (grd[x][y] >= DNGN_STONE_STAIRS_UP_I
                        && grd[x][y] <= DNGN_ROCK_STAIRS_UP)
                        grd[x][y] = DNGN_RETURN_DUNGEON_I + i;
        }
    }

    // bottom level of branch - replaces down stairs with up ladders:
    for (i= 0; i< 30; i++)
    {
        if (level_number == you.branch_stairs[i] + branch_depth(i)
            && level_type == LEVEL_DUNGEON
            && you.where_are_you == BRANCH_ORCISH_MINES + i)
        {
            for (x = 1; x < GXM; x++)
                for (y = 1; y < GYM; y++)
                    if (grd[x][y] >= DNGN_STONE_STAIRS_DOWN_I
                        && grd[x][y] <= DNGN_ROCK_STAIRS_DOWN)
                        grd[x][y] = DNGN_ROCK_STAIRS_UP;
        }
    }

    if (you.where_are_you == BRANCH_CRYPT)
    {
        if (one_chance_in(3))
            mons_place(MONS_CURSE_SKULL, BEH_SLEEP, MHITNOT, false,0,0);

        if (one_chance_in(7))
            mons_place(MONS_CURSE_SKULL, BEH_SLEEP, MHITNOT, false,0,0);
    }

    if (you.where_are_you >= BRANCH_ORCISH_MINES
        && level_type == LEVEL_DUNGEON && one_chance_in(5))
        place_altar();

    // hall of blades (1 level deal) - no down staircases, thanks!
    if (you.where_are_you == BRANCH_HALL_OF_BLADES)
    {
        for (x = 1; x < GXM; x++)
            for (y = 1; y < GYM; y++)
                if (grd[x][y] >= DNGN_STONE_STAIRS_DOWN_I
                    && grd[x][y] <= DNGN_ROCK_STAIRS_UP)
                    grd[x][y] = DNGN_FLOOR;
    }

    link_items();
    prepare_water();
}                               // end builder()

int items(unsigned char allow_uniques,  // not just true-false,
                                        // because of BCR acquirement hack
          unsigned char force_class,    // desired OBJECTS class {dlb}
          unsigned char force_type,     // desired SUBTYPE - enum varies by OBJ
          int force_place,              // ???
          int item_power,        // level of the item, can differ from global
          int force_spec)       // weapon / armour racial categories
{

    int temp_rand = 0;          // probability determination {dlb}
    int range_charges = 0;      // for OBJ_WANDS charge count {dlb}
    unsigned char temp_value = 0;       // temporary value storage {dlb}
    int loopy = 0;              // just another loop variable {dlb}


    int quant = 0;
    int pot = 0;                // argh! just for potions!!! {dlb}
    FixedVector < int, SPELLBOOK_SIZE > fpass;
    int icky = 0;
    int p = 0;

    int no_unique = 0;

    // find an emtpy space for the item:
    for (p = 0; p < MAX_ITEMS; p++)
    {
        if (mitm.base_type[p] == OBJ_UNASSIGNED || mitm.quantity[p] == 0)
            break;
        if (p > MAX_ITEMS - 100)
            return NON_ITEM;
    }

    // clear all properties except mitm.base_type <used in switch below> {dlb}:
    mitm.sub_type[p] = 0;
    mitm.id[p] = 0;
    mitm.special[p] = 0;
    mitm.pluses[p] = 0;
    mitm.pluses2[p] = 0;

    // cap item_power unless an acquirement-level item {dlb}:
    if (item_power > 50 && item_power != 351)
        item_power = 50;

    // determine base_type for item generated {dlb}:
    if (force_class != OBJ_RANDOM)
        mitm.base_type[p] = force_class;
    else
    {
        // nice and large for subtle differences {dlb}
        temp_rand = random2(10000);

        mitm.base_type[p] = ((temp_rand <  74)  ? OBJ_STAVES :   //  0.74%
                              (temp_rand <  222) ? OBJ_BOOKS :    //  1.48%
                              (temp_rand <  471) ? OBJ_JEWELLERY ://  2.49%
                              (temp_rand <  720) ? OBJ_WANDS :    //  2.49%
                              (temp_rand < 1459) ? OBJ_ARMOUR :   //  7.39%
                              (temp_rand < 2198) ? OBJ_WEAPONS :  //  7.39%
                              (temp_rand < 3103) ? OBJ_POTIONS :  //  9.05%
                              (temp_rand < 4342) ? OBJ_FOOD :     // 12.39%
                              (temp_rand < 5894) ? OBJ_MISSILES : // 15.52%
                              (temp_rand < 7889) ? OBJ_SCROLLS    // 19.95%
                                                 : OBJ_GOLD);     // 21.11%

        // misc items placement wholly dependent upon current depth {dlb}:
        if (item_power > 7 && (20 + item_power) >= random2(3500))
            mitm.base_type[p] = OBJ_MISCELLANY;
    }

    // determine sub_type accordingly {dlb}:
    switch (mitm.base_type[p])
    {
    case OBJ_WEAPONS:
        // generate initial weapon subtype using weighted function --
        // indefinite loop now more evident and fewer array lookups {dlb}:
        for (;;)
        {
            temp_value = (unsigned char) random2(NUM_WEAPONS);

            // "0" rare weapons are handled below -- bwr
            if (rare_weapon(temp_value) >= random2(10) + 1)
            {
                mitm.sub_type[p] = temp_value;
                break;
            }
        }

        // this all needs to be compacted (wasteful) {dlb}:
        if (random2(200) < 20 - item_power)
            mitm.sub_type[p] = WPN_KNIFE;
        if (random2(200) < 20 - item_power)
            mitm.sub_type[p] = WPN_QUARTERSTAFF;
        if (random2(200) < 20 - item_power)
            mitm.sub_type[p] = WPN_SLING;
        if (random2(200) < 20 - item_power)
            mitm.sub_type[p] = WPN_SPEAR;
        if (random2(200) < 20 - item_power)
            mitm.sub_type[p] = WPN_HAND_AXE;
        if (random2(200) < 20 - item_power)
            mitm.sub_type[p] = WPN_DAGGER;
        if (random2(200) < 20 - item_power)
            mitm.sub_type[p] = WPN_MACE;
        if (random2(200) < 20 - item_power)
            mitm.sub_type[p] = WPN_DAGGER;
        if (random2(200) < 20 - item_power)
            mitm.sub_type[p] = WPN_CLUB;
        if (random2(200) < 20 - item_power)
            mitm.sub_type[p] = WPN_HAMMER;
        if (random2(200) < 20 - item_power)
            mitm.sub_type[p] = WPN_WHIP;
        if (random2(200) < 20 - item_power)
            mitm.sub_type[p] = WPN_SABRE;

        // place ultra-rare "0" weapons:
        if (item_power > 6 && random2(100) < (10 + item_power)
                                                        && one_chance_in(30))
        {
            // this replaced the infinite loop (wasteful) -- may need
            // to make into its own function to allow ease of tweaking
            // distribution {dlb}:
            temp_rand = random2(9);

            mitm.sub_type[p] = ((temp_rand == 8) ? WPN_DEMON_BLADE :
                                 (temp_rand == 7) ? WPN_DEMON_TRIDENT :
                                 (temp_rand == 6) ? WPN_DEMON_WHIP :
                                 (temp_rand == 5) ? WPN_DOUBLE_SWORD :
                                 (temp_rand == 4) ? WPN_EVENINGSTAR :
                                 (temp_rand == 3) ? WPN_EXECUTIONERS_AXE :
                                 (temp_rand == 2) ? WPN_KATANA :
                                 (temp_rand == 1) ? WPN_QUICK_BLADE
                              /*(temp_rand == 0)*/: WPN_TRIPLE_SWORD);
        }

        if (allow_uniques)
        {
            // Note there is nothing to stop randarts being reproduced,
            // except vast improbability.
            if (mitm.sub_type[p] != WPN_CLUB && item_power > 2
                && random2(2000) <= 100 + (item_power * 3) && coinflip())
            {
                if (you.level_type != LEVEL_ABYSS
                    && you.level_type != LEVEL_PANDEMONIUM
                    && one_chance_in(50))
                {
                    icky = find_okay_unrandart(OBJ_WEAPONS);

                    if (icky != -1)
                    {
                        quant = 1;
                        make_item_unrandart(icky, p);
                        break;
                    }
                }

                mitm.special[p] = 26 + random2(4);
                mitm.pluses[p] = 50;
                mitm.pluses2[p] = 50;
                mitm.pluses[p] += random2(7);
                mitm.pluses2[p] += random2(7);

                if (one_chance_in(3))
                    mitm.pluses[p] += random2(7);

                if (one_chance_in(3))
                    mitm.pluses2[p] += random2(7);

                if (one_chance_in(9))
                    mitm.pluses[p] -= random2(7);

                if (one_chance_in(9))
                    mitm.pluses2[p] -= random2(7);

                quant = 1;

                if (one_chance_in(4))
                {
                    mitm.pluses[p] = 150 - random2(6);
                    mitm.pluses2[p] = 50 - random2(6);
                }
                else if ((mitm.pluses[p] < 50 || mitm.pluses2[p] < 50)
                         && !one_chance_in(3))
                {
                    mitm.pluses[p] += 100;     /* cursed! nasty */
                }
                break;
            }

            if (item_power > 6
                && random2(3000) <= 30 + (item_power * 3) && one_chance_in(20))
            {
                no_unique = random2(12);

                if (no_unique < 7)
                {
                    if (you.unique_items[no_unique] == 1
                        || (item_power == 51
                            && you.unique_items[no_unique] == 0)
                        || (you.unique_items[no_unique] == 2
                            && item_power != 51))
                    {
                        goto out_of_uniques;
                    }
                }
                else if (you.unique_items[24 + no_unique - 7] == 1
                         || (item_power == 51
                             && you.unique_items[24 + no_unique - 7] == 0)
                         || (you.unique_items[24 + no_unique - 7] == 2
                             && item_power != 51))
                {
                    goto out_of_uniques;
                }

                // unique_items:
                // 1 = created and still exists (may be lost underwater etc),
                // 2 = created and lost in the abyss - may be generated
                //     again (in the abyss).
                //
                // Problem: a unique can be generated in the abyss and
                // never be seen by the player, then will be classed as
                // lost in the abyss and will only be found again in the abyss
                //
                // This is not a problem... it's a feature, crawl doesn't and
                // shouldn't be guaranteeing all of these items in every game
                // anyways, so if some get lost in the abyss that's fine--bwr
                mitm.pluses[p] = 50;
                mitm.pluses2[p] = 50;

                switch (no_unique)
                {
                case 0:
                    mitm.sub_type[p] = WPN_LONG_SWORD;
                    mitm.special[p] = NWPN_SINGING_SWORD;
                    mitm.pluses[p] += 7;
                    mitm.pluses2[p] += 6;
                    break;
                case 1:
                    mitm.sub_type[p] = WPN_BATTLEAXE;
                    mitm.special[p] = NWPN_WRATH_OF_TROG;
                    mitm.pluses[p] += 3;
                    mitm.pluses2[p] += 11;
                    break;
                case 2:
                    mitm.sub_type[p] = WPN_SCYTHE;
                    mitm.special[p] = NWPN_SCYTHE_OF_CURSES;
                    mitm.pluses[p] += 11;
                    mitm.pluses2[p] += 11;
                    break;
                case 3:
                    mitm.sub_type[p] = WPN_MACE;
                    mitm.special[p] = NWPN_MACE_OF_VARIABILITY;
                    mitm.pluses[p] += random2(12) - 4;
                    mitm.pluses2[p] += random2(12) - 4;
                    break;
                case 4:
                    mitm.sub_type[p] = WPN_GLAIVE;
                    mitm.special[p] = NWPN_GLAIVE_OF_PRUNE;
                    mitm.pluses[p] += 0;
                    mitm.pluses2[p] += 12;
                    break;
                case 5:
                    mitm.sub_type[p] = WPN_MACE;
                    mitm.special[p] = NWPN_SCEPTRE_OF_TORMENT;
                    mitm.pluses[p] += 7;
                    mitm.pluses2[p] += 6;
                    break;
                case 6:
                    mitm.sub_type[p] = WPN_LONG_SWORD;
                    mitm.special[p] = NWPN_SWORD_OF_ZONGULDROK;
                    mitm.pluses[p] += 9;
                    mitm.pluses2[p] += 9;
                    break;
                case 7:
                    if (coinflip())
                    {
                        mitm.sub_type[p] = WPN_GREAT_SWORD;
                        mitm.special[p] = NWPN_SWORD_OF_POWER;
                        mitm.pluses[p] += 0;
                        mitm.pluses2[p] += 0;
                    }
                    else
                    {
                        // must avoid setting flag if not created.
                        goto out_of_uniques;
                    }
                    break;
                case 8:
                    mitm.sub_type[p] = WPN_DAGGER;
                    mitm.special[p] = NWPN_KNIFE_OF_ACCURACY;
                    mitm.pluses[p] += 27;
                    mitm.pluses2[p] -= 1;
                    break;
                case 9:
                    mitm.sub_type[p] = WPN_QUARTERSTAFF;
                    mitm.special[p] = NWPN_STAFF_OF_OLGREB;
                    break;
                case 10:
                    mitm.sub_type[p] = WPN_DAGGER;
                    mitm.special[p] = NWPN_VAMPIRES_TOOTH;
                    mitm.pluses[p] += 3;
                    mitm.pluses2[p] += 4;
                    break;
                case 11:
                    mitm.sub_type[p] = WPN_QUARTERSTAFF;
                    mitm.special[p] = NWPN_STAFF_OF_WUCAD_MU;
                    mitm.pluses[p] += 0;
                    mitm.pluses2[p] += 0;
                    break;
                }

                quant = 1;

                if (no_unique <= 6)
                    you.unique_items[no_unique] = 1;
                else
                    you.unique_items[24 + no_unique - 7] = 1;

                break;
            }
        }

      out_of_uniques:
        if (force_type != OBJ_RANDOM)
            mitm.sub_type[p] = force_type;

        if (item_power == 351 && (mitm.sub_type[p] == WPN_CLUB
                                        || mitm.sub_type[p] == WPN_SLING))
        {
            mitm.sub_type[p] = WPN_LONG_SWORD;
        }

        quant = 1;

        mitm.pluses[p] = 50;
        mitm.pluses2[p] = 50;
        mitm.special[p] = 0;

        if (force_spec == 250 && coinflip())
        {
            // XXX: The "* 30"s here are because of an ugly hack that
            // should be removed -- bwr
            switch (mitm.sub_type[p])
            {
            case WPN_CLUB:
                if (coinflip())
                    mitm.special[p] = DWPN_ORCISH * 30;
                break;

            case WPN_MACE:
            case WPN_FLAIL:
            case WPN_SPIKED_FLAIL:
            case WPN_GREAT_MACE:
            case WPN_GREAT_FLAIL:
                if (one_chance_in(3))
                    mitm.special[p] = DWPN_ORCISH * 30;
                if (one_chance_in(4))
                    mitm.special[p] = DWPN_DWARVEN * 30;
                if (one_chance_in(6))
                    mitm.special[p] = DWPN_ELVEN * 30;
                break;

            case WPN_MORNINGSTAR:
            case WPN_HAMMER:
                if (one_chance_in(3))
                    mitm.special[p] = DWPN_ORCISH * 30;
                if (one_chance_in(3))
                    mitm.special[p] = DWPN_DWARVEN * 30;
                break;

            case WPN_DAGGER:
                if (one_chance_in(3))
                    mitm.special[p] = DWPN_ORCISH * 30;
                if (one_chance_in(4))
                    mitm.special[p] = DWPN_DWARVEN * 30;
                if (one_chance_in(4))
                    mitm.special[p] = DWPN_ELVEN * 30;
                break;

            case WPN_SHORT_SWORD:
                if (one_chance_in(3))
                    mitm.special[p] = DWPN_ORCISH * 30;
                if (one_chance_in(3))
                    mitm.special[p] = DWPN_DWARVEN * 30;
                if (one_chance_in(3))
                    mitm.special[p] = DWPN_ELVEN * 30;
                break;

            case WPN_LONG_SWORD:
                if (one_chance_in(4))
                    mitm.special[p] = DWPN_ORCISH * 30;
                if (coinflip())
                    mitm.special[p] = DWPN_ELVEN * 30;
                break;

            case WPN_GREAT_SWORD:
                if (one_chance_in(3))
                    mitm.special[p] = DWPN_ORCISH * 30;
                break;

            case WPN_SCIMITAR:
                if (coinflip())
                    mitm.special[p] = DWPN_ORCISH * 30;
                break;

            case WPN_AXE:
            case WPN_HAND_AXE:
            case WPN_BROAD_AXE:
            case WPN_BATTLEAXE:
                if (one_chance_in(3))
                    mitm.special[p] = DWPN_ORCISH * 30;
                if (coinflip())
                    mitm.special[p] = DWPN_DWARVEN * 30;
                break;

            case WPN_SPEAR:
                if (one_chance_in(4))
                    mitm.special[p] = DWPN_ORCISH * 30;
                if (one_chance_in(4))
                    mitm.special[p] = DWPN_ELVEN * 30;
                break;

            case WPN_HALBERD:
            case WPN_GLAIVE:
            case WPN_TRIDENT:
            case WPN_EXECUTIONERS_AXE:
                if (one_chance_in(5))
                    mitm.special[p] = DWPN_ORCISH * 30;
                break;

            case WPN_QUICK_BLADE:
                if (one_chance_in(4))
                    mitm.special[p] = DWPN_ELVEN * 30;
                break;

            case WPN_KATANA:
            case WPN_KNIFE:
                mitm.special[p] = 0;
                break;

                // 13 - sling

            case WPN_BOW:
                if (one_chance_in(6))
                    mitm.special[p] = DWPN_ORCISH * 30;
                if (coinflip())
                    mitm.special[p] = DWPN_ELVEN * 30;
                break;

            case WPN_CROSSBOW:
                if (one_chance_in(4))
                    mitm.special[p] = DWPN_ORCISH * 30;
                if (one_chance_in(4))
                    mitm.special[p] = DWPN_DWARVEN * 30;
                break;

            case WPN_HAND_CROSSBOW:
                if (one_chance_in(3))
                    mitm.special[p] = DWPN_ELVEN * 30;
                break;
            }
        }

        // fine, but out-of-order relative to mitm.special[] ordering {dlb}
        switch (force_spec)
        {
        case 1:         // elf
            mitm.special[p] = DWPN_ELVEN * 30;
            break;

        case 2:         // dwarf
            mitm.special[p] = DWPN_DWARVEN * 30;
            break;

        case 3:         // orc
            mitm.special[p] = DWPN_ORCISH * 30;
            break;
        }

        switch (mitm.special[p])
        {
        case DWPN_ORCISH * 30:
            if (coinflip())
                mitm.pluses[p]--;
            if (coinflip())
                mitm.pluses2[p]++;
            break;

        case DWPN_ELVEN * 30:
            if (coinflip())
                mitm.pluses[p]++;
            break;

        case DWPN_DWARVEN * 30:
            if (coinflip())
                mitm.pluses[p]++;
            if (coinflip())
                mitm.pluses2[p]++;
            break;
        }

        if ((random2(200) <= 50 + item_power
                || item_power == 351
                || is_demonic(mitm.sub_type[p]))
            // nobody would bother enchanting a club
            && mitm.sub_type[p] != WPN_CLUB
            && mitm.sub_type[p] != WPN_GIANT_CLUB
            && mitm.sub_type[p] != WPN_GIANT_SPIKED_CLUB)
        {
            // this is dangerous -- not change below {dlb}
            if (item_power == 351)
                item_power = 200;

            // odd-looking, but this is how the algorithm compacts {dlb}:
            for (loopy = 0; loopy < 4; loopy++)
            {
                mitm.pluses[p] += random2(3);

                if (!(random2(350) <= 20 + item_power))
                    break;
            }

            // odd-looking, but this is how the algorithm compacts {dlb}:
            for (loopy = 0; loopy < 4; loopy++)
            {
                mitm.pluses2[p] += random2(3);

                if (!(random2(500) <= 50 + item_power))
                    break;
            }

            if (random2(300) <= 100 + item_power
                || (item_power == 200 && coinflip()) // was "== 351", see above
                || is_demonic(mitm.sub_type[p]))
            {
                // note: even this doesn't guarantee special enchantment
                switch (mitm.sub_type[p])
                {
                case WPN_EVENINGSTAR:
                    if (coinflip())
                        set_weapon_special(p, SPWPN_DRAINING);
                    // **** intentional fall through here ****
                case WPN_MORNINGSTAR:
                    if (one_chance_in(4))
                        set_weapon_special(p, SPWPN_VENOM);

                    if (one_chance_in(4))
                    {
                        set_weapon_special(p, (coinflip() ? SPWPN_FLAMING
                                                           : SPWPN_FREEZING));
                    }

                    if (one_chance_in(20))
                        set_weapon_special(p, SPWPN_VAMPIRICISM);
                    // **** intentional fall through here ****
                case WPN_MACE:
                case WPN_GREAT_MACE:
                    if ((mitm.sub_type[p] == WPN_MACE
                            || mitm.sub_type[p] == WPN_GREAT_MACE)
                        && one_chance_in(4))
                    {
                        set_weapon_special(p, SPWPN_DISRUPTION);
                    }
                    // **** intentional fall through here ****
                case WPN_FLAIL:
                case WPN_SPIKED_FLAIL:
                case WPN_GREAT_FLAIL:
                case WPN_HAMMER:
                    if (one_chance_in(15))
                        set_weapon_special(p, SPWPN_DISTORTION);

                    if (one_chance_in(25))
                        set_weapon_special(p, SPWPN_PAIN);

                    if (one_chance_in(3) &&
                        (!is_weapon_special(p) || one_chance_in(5)))
                        set_weapon_special(p, SPWPN_VORPAL);

                    if (one_chance_in(4))
                        set_weapon_special(p, SPWPN_HOLY_WRATH);

                    if (one_chance_in(3))
                        set_weapon_special(p, SPWPN_PROTECTION);

                    if (one_chance_in(10))
                        set_weapon_special(p, SPWPN_DRAINING);
                    break;


                case WPN_DAGGER:
                    if (one_chance_in(3))
                        set_weapon_special(p, SPWPN_VENOM);

                    if (one_chance_in(10))
                        set_weapon_special(p, SPWPN_VAMPIRICISM);
                    // **** intentional fall through here ****
                case WPN_SHORT_SWORD:
                    if (one_chance_in(8))
                        set_weapon_special(p, SPWPN_VENOM);
                    // **** intentional fall through here ****
                case WPN_SCIMITAR:
                    if (one_chance_in(7))
                        set_weapon_special(p, SPWPN_SPEED);
                    // **** intentional fall through here ****
                case WPN_LONG_SWORD:
                    if (one_chance_in(12))
                        set_weapon_special(p, SPWPN_VENOM);
                    // **** intentional fall through here ****
                case WPN_GREAT_SWORD:
                case WPN_DOUBLE_SWORD:
                case WPN_TRIPLE_SWORD:
                    if (one_chance_in(10))
                        set_weapon_special(p, SPWPN_VAMPIRICISM);

                    if (one_chance_in(15))
                        set_weapon_special(p, SPWPN_DISTORTION);

                    if (one_chance_in(25))
                        set_weapon_special(p, SPWPN_PAIN);

                    if (one_chance_in(5))
                    {
                        set_weapon_special(p, (coinflip() ? SPWPN_FLAMING
                                                           : SPWPN_FREEZING));
                    }

                    if (one_chance_in(7))
                        set_weapon_special(p, SPWPN_ELECTROCUTION);

                    if (one_chance_in(7))
                        set_weapon_special(p, SPWPN_PROTECTION);

                    if (one_chance_in(8))
                        set_weapon_special(p, SPWPN_ORC_SLAYING);

                    if (one_chance_in(12))
                        set_weapon_special(p, SPWPN_DRAINING);

                    if (one_chance_in(4))
                        set_weapon_special(p, SPWPN_HOLY_WRATH);

                    if (one_chance_in(4)
                            && (!is_weapon_special(p) || one_chance_in(3)))
                    {
                        set_weapon_special(p, SPWPN_VORPAL);
                    }

                    break;


                case WPN_AXE:
                case WPN_BROAD_AXE:
                case WPN_BATTLEAXE:
                case WPN_EXECUTIONERS_AXE:
                    if (one_chance_in(25))
                        set_weapon_special(p, SPWPN_HOLY_WRATH);

                    if (one_chance_in(14))
                        set_weapon_special(p, SPWPN_DRAINING);
                    // **** intentional fall through here ****
                case WPN_HAND_AXE:
                    if (one_chance_in(10))
                        set_weapon_special(p, SPWPN_VAMPIRICISM);

                    if (one_chance_in(15))
                        set_weapon_special(p, SPWPN_DISTORTION);

                    if (one_chance_in(25))
                        set_weapon_special(p, SPWPN_PAIN);

                    if (one_chance_in(3)
                            && (!is_weapon_special(p) || one_chance_in(5)))
                    {
                        set_weapon_special(p, SPWPN_VORPAL);
                    }

                    if (one_chance_in(6))
                        set_weapon_special(p, SPWPN_ORC_SLAYING);

                    if (one_chance_in(4))
                    {
                        set_weapon_special(p, (coinflip() ? SPWPN_FLAMING
                                                           : SPWPN_FREEZING));
                    }

                    if (one_chance_in(8))
                        set_weapon_special(p, SPWPN_ELECTROCUTION);

                    if (one_chance_in(12))
                        set_weapon_special(p, SPWPN_VENOM);

                    break;

                case WPN_WHIP:
                    if (one_chance_in(20))
                        set_weapon_special(p, SPWPN_DISTORTION);

                    if (one_chance_in(6))
                    {
                        set_weapon_special(p, (coinflip() ? SPWPN_FLAMING
                                                           : SPWPN_FREEZING));
                    }

                    if (one_chance_in(6))
                        set_weapon_special(p, SPWPN_VENOM);

                    if (coinflip())
                        set_weapon_special(p, SPWPN_REACHING);

                    if (one_chance_in(5))
                        set_weapon_special(p, SPWPN_SPEED);

                    if (one_chance_in(5))
                        set_weapon_special(p, SPWPN_ELECTROCUTION);
                    break;

                case WPN_HALBERD:
                case WPN_GLAIVE:
                    if (one_chance_in(30))
                        set_weapon_special(p, SPWPN_HOLY_WRATH);

                    if (one_chance_in(4))
                        set_weapon_special(p, SPWPN_PROTECTION);
                    // **** intentional fall through here ****
                case WPN_SCYTHE:
                case WPN_TRIDENT:
                    if (one_chance_in(5))
                        set_weapon_special(p, SPWPN_SPEED);
                    // **** intentional fall through here ****
                case WPN_SPEAR:
                    if (one_chance_in(10))
                        set_weapon_special(p, SPWPN_VAMPIRICISM);

                    if (one_chance_in(20))
                        set_weapon_special(p, SPWPN_DISTORTION);

                    if (one_chance_in(25))
                        set_weapon_special(p, SPWPN_PAIN);

                    if (one_chance_in(5) &&
                        (!is_weapon_special(p) || one_chance_in(6)))
                        set_weapon_special(p, SPWPN_VORPAL);

                    if (one_chance_in(6))
                        set_weapon_special(p, SPWPN_ORC_SLAYING);

                    if (one_chance_in(6))
                    {
                        set_weapon_special(p, (coinflip() ? SPWPN_FLAMING
                                                           : SPWPN_FREEZING));
                    }

                    if (one_chance_in(6))
                        set_weapon_special(p, SPWPN_VENOM);

                    if (one_chance_in(3))
                        set_weapon_special(p, SPWPN_REACHING);
                    break;


                case WPN_SLING:
                    if (!one_chance_in(4))
                        break;
                    // **** possible intentional fall through here ****
                case WPN_HAND_CROSSBOW:
                    if (coinflip())
                        break;
                    // **** possible intentional fall through here ****
                case WPN_BOW:
                case WPN_CROSSBOW:
                    if (coinflip())
                    {
                        set_weapon_special(p, (coinflip() ? SPWPN_FLAME
                                                           : SPWPN_FROST));
                    }
                    break;

                // quarterstaff - not powerful, as this would make
                // the 'staves' skill just too good
                case WPN_QUARTERSTAFF:
                    if (one_chance_in(15))
                        set_weapon_special(p, SPWPN_DISTORTION);

                    if (one_chance_in(25))
                        set_weapon_special(p, SPWPN_PAIN);

                    if (one_chance_in(5))
                        set_weapon_special(p, SPWPN_PROTECTION);

                    if (one_chance_in(5))
                        set_weapon_special(p, SPWPN_SPEED);
                    break;


                case WPN_DEMON_TRIDENT:
                case WPN_DEMON_WHIP:
                case WPN_DEMON_BLADE:
                    if (mitm.special[p] >= 90)
                        mitm.special[p] = mitm.special[p] % 30;

                    if (one_chance_in(3)
                        && (mitm.sub_type[p] == WPN_DEMON_WHIP
                            || mitm.sub_type[p] == WPN_DEMON_TRIDENT))
                    {
                        set_weapon_special(p, SPWPN_REACHING);
                    }

                    if (one_chance_in(5))
                        set_weapon_special(p, SPWPN_DRAINING);

                    if (one_chance_in(5))
                        set_weapon_special(p, SPWPN_FLAMING);

                    if (one_chance_in(5))
                        set_weapon_special(p, SPWPN_FREEZING);

                    if (one_chance_in(5))
                        set_weapon_special(p, SPWPN_ELECTROCUTION);

                    if (one_chance_in(5))
                        set_weapon_special(p, SPWPN_VAMPIRICISM);

                    if (one_chance_in(5))
                        set_weapon_special(p, SPWPN_PAIN);

                    if (one_chance_in(5))
                        set_weapon_special(p, SPWPN_VENOM);
                    break;

                // unlisted weapons have no associated, standard ego-types {dlb}
                default:
                    break;
                }
            }                   // end if specially enchanted
        }
        else
        {
            if (one_chance_in(12))
            {
                mitm.pluses[p] = 150;
                mitm.pluses[p] -= random2(4);
                mitm.pluses2[p] = 50;
                mitm.pluses2[p] -= random2(4);

                // clear specials {dlb}
                mitm.special[p] = (mitm.special[p] / 30) * 30;
            }
        }

        // value was "0" comment said "orc" so I went with comment {dlb}
        if (mitm.special[p] / 30 == DWPN_ORCISH)
        {
            if (mitm.special[p] % 30 != SPWPN_NORMAL)
            {
                if (mitm.special[p] % 30 == SPWPN_HOLY_WRATH
                    || mitm.special[p] % 30 == SPWPN_ORC_SLAYING
                    || coinflip())
                {
                    // this makes no sense {dlb}
                    if (mitm.special[p] % 30 == SPWPN_ELECTROCUTION)
                        mitm.pluses[p] = 50;

                    // no holy wrath or slay orc and 1/2 the time no-ego
                    // that is, set it to plain' ol orcishness ... {dlb}
                    mitm.special[p] = 90;
                }
            }
        }


        if ((mitm.special[p] != 0
                || (mitm.pluses[p] != 50 && one_chance_in(3)))
            && mitm.sub_type[p] != WPN_CLUB
            && mitm.special[p] / 30 == DWPN_PLAIN)
        {
            // that is, runed or glowing {dlb}
            mitm.special[p] += (coinflip() ? DWPN_GLOWING : DWPN_RUNED) * 30;
        }
        break;

    case OBJ_MISSILES:
        mitm.pluses[p] = 0;
        mitm.sub_type[p] = random2(4);
        quant = 0;

        if (force_type != OBJ_RANDOM)
            mitm.sub_type[p] = force_type;

        // "* 30"s are important here too -- bwr
        if (force_spec != 250)
        {
            switch (force_spec)
            {
            case 3:             // orc
                mitm.special[p] = DAMMO_ORCISH * 30;
                break;
            case 1:             // elf
                mitm.special[p] = DAMMO_ELVEN * 30;
                break;
            case 2:             // dw
                mitm.special[p] = DAMMO_DWARVEN * 30;
                break;
            }
        }
        else
        {
            if ((mitm.sub_type[p] == MI_ARROW
                    || mitm.sub_type[p] == MI_DART)
                && one_chance_in(4))
            {
                // elven - not for bolts, though
                mitm.special[p] = DAMMO_ELVEN * 30;
            }

            if ((mitm.sub_type[p] == MI_ARROW
                    || mitm.sub_type[p] == MI_BOLT
                    || mitm.sub_type[p] == MI_DART)
                && one_chance_in(4))
            {
                mitm.special[p] = DAMMO_ORCISH * 30;
            }

            if ((mitm.sub_type[p] == MI_DART
                    || mitm.sub_type[p] == MI_BOLT)
                && one_chance_in(6))
            {
                mitm.special[p] = DAMMO_DWARVEN * 30;
            }
        }

        if (mitm.sub_type[p] == MI_ARROW
            || mitm.sub_type[p] == MI_BOLT
            || mitm.sub_type[p] == MI_DART)
        {
            if (item_power == 351)
                temp_rand = random2(150);
            else
                temp_rand = random2(2000 - 55 * item_power);

            mitm.special[p] += (temp_rand < 60) ? SPMSL_FLAME :
                                 (temp_rand < 120) ? SPMSL_ICE  :
                                 (temp_rand < 150) ? SPMSL_POISONED_II :
                                 0;

            // orcish ammo gets poisoned a lot more often -- in the original
            // code it was poisoned every time!?
            if (mitm.special[p] == DAMMO_ORCISH * 30 && one_chance_in(3))
                mitm.special[p] += SPMSL_POISONED_II;

            // reduce quantity if special
            if (mitm.special[p] % 30 != SPMSL_NORMAL)
                quant = 1 + random2(9) + random2(12) + random2(12);
        }

        // quant will have already been set if the ammo is 'special'
        if (quant == 0)
            quant = 1 + random2(9) + random2(12) + random2(15) + random2(12);

        if (mitm.sub_type[p] == MI_LARGE_ROCK)
            quant = 1 + random2avg(5, 2);

        if (10 + item_power >= random2(100))
            mitm.pluses[p] = random2(5);

        mitm.pluses[p] += 50;
        break;

    case OBJ_ARMOUR:
        quant = 1;

        mitm.sub_type[p] = random2(3);
        mitm.pluses[p] = 0;
        mitm.pluses2[p] = 0;

        if (random2(35) <= item_power + 10)
        {
            mitm.sub_type[p] = random2(5);
            if (one_chance_in(4))
                mitm.sub_type[p] = ARM_ANIMAL_SKIN;

        }

        if (random2(60) <= item_power + 10)
            mitm.sub_type[p] = random2(8);

        if (10 + item_power >= random2(400) && one_chance_in(10))
            mitm.sub_type[p] = ARM_DRAGON_HIDE + random2(7);

        if (10 + item_power >= random2(500) && one_chance_in(20))
        {
            mitm.sub_type[p] = ARM_STEAM_DRAGON_HIDE + random2(11);

            if (mitm.sub_type[p] == ARM_ANIMAL_SKIN)
                mitm.sub_type[p] = ARM_CRYSTAL_PLATE_MAIL;
        }

        if (one_chance_in(4))
            mitm.sub_type[p] = ARM_SHIELD + random2(7);

        if (force_type != OBJ_RANDOM)
            mitm.sub_type[p] = force_type;

        if (allow_uniques == 1
            && item_power > 2
            && random2(2000) <= (100 + item_power * 3)
            && coinflip())
        {
            if ((you.level_type != LEVEL_ABYSS
                    && you.level_type != LEVEL_PANDEMONIUM)
                && one_chance_in(50))
            {
                icky = find_okay_unrandart(OBJ_ARMOUR);
                if (icky != -1)
                {
                    quant = 1;
                    make_item_unrandart(icky, p);
                    break;
                }
            }

            hide2armour(&(mitm.sub_type[p]));

            mitm.special[p] = 26 + random2(4);
            mitm.pluses[p] = 50;
            mitm.pluses2[p] = random2(150);

            if (mitm.sub_type[p] == ARM_BOOTS)
            {
                mitm.pluses2[p] = 0;
                if (one_chance_in(6))
                    mitm.pluses2[p] += random2(3);
            }

            if (mitm.sub_type[p] == ARM_HELMET)
            {
                mitm.pluses2[p] = 0;
                if (one_chance_in(3))
                    mitm.pluses2[p] += random2(4);
            }

            mitm.pluses[p] += random2(4);

            if (one_chance_in(5))
                mitm.pluses[p] += random2(4);

            if (one_chance_in(6))
                mitm.pluses[p] -= random2(8);

            quant = 1;

            if (one_chance_in(5))
            {
                mitm.pluses[p] = 150 - random2(6);
            }
            else if (mitm.pluses[p] < 50 && !one_chance_in(3))
                mitm.pluses[p] += 100; /* cursed! nasty */
            break;
        }

        mitm.pluses[p] = 0;

        if (mitm.sub_type[p] == ARM_HELMET && coinflip())
            mitm.pluses2[p] = random2(4);

        // 180 - orc, 150 - dwar, 120 - elf
        // XXX: "* 30" important!  please fix -- bwr
        if (force_spec == 250 && coinflip())
        {
            switch (mitm.sub_type[p])
            {
            case ARM_SHIELD:    // shield - must do special things for this!
            case ARM_BUCKLER:
                if (one_chance_in(4))
                    mitm.special[p] = DARM_ELVEN * 30;
                // ****** deliberate fall-through here? ******
            case ARM_LARGE_SHIELD:
                if (one_chance_in(3))
                    mitm.special[p] = DARM_DWARVEN * 30;
                break;

            case ARM_CLOAK:
                if (one_chance_in(4))
                    mitm.special[p] = DARM_ORCISH * 30;
                if (one_chance_in(4))
                    mitm.special[p] = DARM_DWARVEN * 30;
                if (one_chance_in(4))
                    mitm.special[p] = DARM_ELVEN * 30;
                break;

            // no helmet, for various reasons
            case ARM_GLOVES:
                if (one_chance_in(4))
                    mitm.special[p] = DARM_ELVEN * 30;
                break;

            case ARM_BOOTS:
                if (one_chance_in(4))
                {
                    mitm.pluses2[p] = 1;
                    break;      /* naga barding */
                }
                if (one_chance_in(4))
                {
                    mitm.pluses2[p] = 2;
                    break;      /* naga barding */
                }
                if (one_chance_in(4))
                    mitm.special[p] = DARM_ORCISH * 30;
                if (one_chance_in(4))
                    mitm.special[p] = DARM_ELVEN * 30;
                if (one_chance_in(6))
                    mitm.special[p] = DARM_DWARVEN * 30;
                break;

            case ARM_ROBE:
                if (one_chance_in(4))
                    mitm.special[p] = DARM_ELVEN * 30;
                break;

            default:            // other body armours:

                if (mitm.sub_type[p] <= ARM_CHAIN_MAIL && one_chance_in(6))
                    mitm.special[p] = DARM_ELVEN * 30;
                if (mitm.sub_type[p] >= ARM_RING_MAIL && one_chance_in(5))
                    mitm.special[p] = DARM_DWARVEN * 30;
                if (one_chance_in(5))
                    mitm.special[p] = DARM_ORCISH * 30;
                break;
            }
        }

        switch (force_spec)
        {
        case 1:         // elf
            mitm.special[p] = DARM_ELVEN * 30;
            break;

        case 2:         // dwarf
            mitm.special[p] = DARM_DWARVEN * 30;
            if (coinflip())
                mitm.pluses[p]++;
            break;

        case 3:         // orc
            mitm.special[p] = DARM_ORCISH * 30;
            break;
        }


        if (50 + item_power >= random2(250)
            || (mitm.sub_type[p] == ARM_HELMET && mitm.pluses2[p] == 3))
        {
            mitm.pluses[p] += random2(3);

            if (mitm.sub_type[p] <= 7 && 20 + item_power >= random2(300))
                mitm.pluses[p] += random2(3);

            if (30 + item_power >= random2(350)
                && (mitm.special[p] != DARM_ORCISH * 30
                    || mitm.sub_type[p] <= ARM_PLATE_MAIL))
            {
                switch (mitm.sub_type[p])
                {
                case ARM_SHIELD:   // shield - must do special things for this!
                case ARM_LARGE_SHIELD:
                case ARM_BUCKLER:
                    mitm.special[p] += SPARM_PROTECTION;
                    break;  // prot
                    //break;

                case ARM_CLOAK:
                    if (mitm.special[p] == DARM_DWARVEN * 30)
                        break;

                    switch (random2(4))
                    {
                    case 0:
                        mitm.special[p] += SPARM_POISON_RESISTANCE;
                        break;
                    case 1:
                        mitm.special[p] += SPARM_DARKNESS;
                        break;
                    case 2:
                        mitm.special[p] += SPARM_MAGIC_RESISTANCE;
                        break;
                    case 3:
                        mitm.special[p] += SPARM_PRESERVATION;
                        break;
                    }
                    break;

                case ARM_HELMET:
                    if (mitm.pluses2[p] == 2 && one_chance_in(4))
                        mitm.special[p] += SPARM_DARKNESS;
                    else if (mitm.pluses2[p] == 3 && one_chance_in(4))
                        mitm.special[p] += SPARM_MAGIC_RESISTANCE;
                    else
                        switch (random2(3))
                        {
                        case 0:
                            mitm.special[p] += SPARM_SEE_INVISIBLE;
                            break;
                        case 1:
                            mitm.special[p] += SPARM_DARKNESS;
                            break;
                        case 2:
                            mitm.special[p] += SPARM_INTELLIGENCE;
                            break;
                        }
                    break;

                case ARM_GLOVES:
                    mitm.special[p] += (coinflip() ? SPARM_DEXTERITY
                                                    : SPARM_STRENGTH);
                    break;

                case ARM_BOOTS:
                    switch (random2(3))
                    {
                    case 0:
                        mitm.special[p] += SPARM_RUNNING;
                        break;
                    case 1:
                        mitm.special[p] += SPARM_LEVITATION;
                        break;
                    case 2:
                        mitm.special[p] += SPARM_STEALTH;
                        break;
                    }
                    break;

                case ARM_ROBE:
                    switch (random2(4))
                    {
                    case 0:
                        mitm.special[p] += (coinflip() ? SPARM_COLD_RESISTANCE
                                                        : SPARM_FIRE_RESISTANCE);
                        break;
                    case 1:
                        mitm.special[p] += SPARM_MAGIC_RESISTANCE;
                        break;
                    case 2:
                        mitm.special[p] += (coinflip() ? SPARM_POSITIVE_ENERGY
                                                        : SPARM_RESISTANCE);
                        break;
                    case 3:
                        if (force_type != OBJ_RANDOM
                            || mitm.special[p] != 0
                            || random2(50) > 10 + item_power)
                        {
                            break;
                        }

                        mitm.special[p] += SPARM_ARCHMAGI;
                        break;
                    }
                    break;

                default:    // other body armours:
                    mitm.special[p] += (coinflip() ? 3 : 2);
                    if (one_chance_in(9))
                        mitm.special[p] = SPARM_POSITIVE_ENERGY;
                    if (one_chance_in(5))
                        mitm.special[p] = SPARM_MAGIC_RESISTANCE;
                    if (one_chance_in(5))
                        mitm.special[p] = SPARM_POISON_RESISTANCE;

                    if (mitm.sub_type[p] == ARM_PLATE_MAIL
                        && one_chance_in(15))
                    {
                        mitm.special[p] += SPARM_PONDEROUSNESS;
                        mitm.pluses[p] += 1 + random2(4);
                    }
                    break;
                }
            }
        }
        else if (one_chance_in(12))
        {
            mitm.pluses[p] = (coinflip() ? 99 : 98);
            if (one_chance_in(5))
                mitm.pluses[p] -= random2(3);
            mitm.special[p] = 0;   // heh heh heh
        }

        if (mitm.special[p] < 120
            && (mitm.special[p] != 0
                || (mitm.pluses[p] != 0 && one_chance_in(3))))
        {
            mitm.special[p] += random2(3) * 30;
        }

        if (mitm.sub_type[p] > ARM_LARGE_SHIELD)
            mitm.special[p] = 0;       // sorry about that.

        mitm.pluses[p] += 50;

        // make sure you don't get a hide from a scroll of acquirement
        if (item_power == 351)
            hide2armour( &(mitm.sub_type[p]) ); // what of animal hides? {dlb}

        if (mitm.sub_type[p] >= ARM_DRAGON_HIDE
            && mitm.sub_type[p] <= ARM_ANIMAL_SKIN)
        {
            mitm.special[p] = 0;
        }

        /* skin armours + Crystal PM don't get special enchantments
           or species, but can be randarts
         */
        break;

    case OBJ_WANDS:
        // determine sub_type:
        mitm.sub_type[p] = ((force_type != OBJ_RANDOM) ? force_type
                                                        : random2(NUM_WANDS));

        // determine upper bound on charges:
        range_charges = ((mitm.sub_type[p] == WAND_HEALING)           ? 10 :
                         (mitm.sub_type[p] == WAND_FLAME
                          || mitm.sub_type[p] == WAND_FROST
                          || mitm.sub_type[p] == WAND_MAGIC_DARTS
                          || mitm.sub_type[p] == WAND_RANDOM_EFFECTS) ? 28
                                                                       : 16);

        // generate charges randomly:
        mitm.pluses[p] = random2avg(range_charges, 3);
        // set quantity to one:
        quant = 1;
        break;

    case OBJ_FOOD:              // this can be parsed out {dlb}
        // determine sub_type:
        if (force_type == OBJ_RANDOM)
        {
            temp_rand = random2(1344);

            mitm.sub_type[p] =
                    ((temp_rand > 899) ? FOOD_MEAT_RATION : // 33.11% chance
                     (temp_rand > 453) ? FOOD_BREAD_RATION :// 33.11% chance
                     (temp_rand > 323) ? FOOD_PEAR :        //  9.67% chance
                     (temp_rand > 193) ? FOOD_APPLE :       //  9.67% chance
                     (temp_rand >  63) ? FOOD_CHOKO :       //  9.67% chance
                     (temp_rand >  59) ? FOOD_HONEYCOMB :   //  0.30% chance
                     (temp_rand >  55) ? FOOD_ROYAL_JELLY : //  0.30% chance
                     (temp_rand >  51) ? FOOD_SNOZZCUMBER : //  0.30% chance
                     (temp_rand >  47) ? FOOD_PIZZA :       //  0.30% chance
                     (temp_rand >  43) ? FOOD_APRICOT :     //  0.30% chance
                     (temp_rand >  39) ? FOOD_ORANGE :      //  0.30% chance
                     (temp_rand >  35) ? FOOD_BANANA :      //  0.30% chance
                     (temp_rand >  31) ? FOOD_STRAWBERRY :  //  0.30% chance
                     (temp_rand >  27) ? FOOD_RAMBUTAN :    //  0.30% chance
                     (temp_rand >  23) ? FOOD_LEMON :       //  0.30% chance
                     (temp_rand >  19) ? FOOD_GRAPE :       //  0.30% chance
                     (temp_rand >  15) ? FOOD_SULTANA :     //  0.30% chance
                     (temp_rand >  11) ? FOOD_LYCHEE :      //  0.30% chance
                     (temp_rand >   7) ? FOOD_BEEF_JERKY :  //  0.30% chance
                     (temp_rand >   3) ? FOOD_CHEESE        //  0.30% chance
                                       : FOOD_SAUSAGE);     //  0.30% chance
        }
        else
            mitm.sub_type[p] = force_type;

        // determine quantity:
        if (allow_uniques > 1)
            quant = allow_uniques;
        else
        {
            quant = 1;

            if (one_chance_in(80))
                quant += random2(3);

            if (mitm.sub_type[p] == FOOD_STRAWBERRY
                || mitm.sub_type[p] == FOOD_GRAPE
                || mitm.sub_type[p] == FOOD_SULTANA)
            {
                quant = 3 + random2(15);
            }
        }
        break;

    case OBJ_POTIONS:
        pot = random2(9);       // general type of potion;

        quant = 1;

        if (one_chance_in(18))
            quant++;

        if (one_chance_in(25))
            quant++;

        switch (pot)
        {
        case POT_HEALING:
        case POT_HEAL_WOUNDS:
        case POT_SPEED:
        case POT_POISON:
            // healing potions
            if (one_chance_in(3))
                mitm.sub_type[p] = POT_HEAL_WOUNDS;
            else
                mitm.sub_type[p] = POT_HEALING;

            if (one_chance_in(20))
                mitm.sub_type[p] = POT_CURE_MUTATION;
            break;

        case POT_MIGHT:
        case POT_GAIN_STRENGTH:
            // enhancements
            if (coinflip())
                mitm.sub_type[p] = POT_SPEED;
            else
                mitm.sub_type[p] = POT_MIGHT;

            if (one_chance_in(10))
                mitm.sub_type[p] = POT_BERSERK_RAGE;

            if (one_chance_in(5))
                mitm.sub_type[p] = POT_INVISIBILITY;

            if (one_chance_in(6))
                mitm.sub_type[p] = POT_LEVITATION;

            if (one_chance_in(30))
                mitm.sub_type[p] = POT_PORRIDGE;
            break;

        case POT_GAIN_DEXTERITY:
            // gain ability
            mitm.sub_type[p] = POT_GAIN_STRENGTH + random2(3);
            if (one_chance_in(10))
                mitm.sub_type[p] = POT_EXPERIENCE;
            if (one_chance_in(10))
                mitm.sub_type[p] = POT_MAGIC;
            if (!one_chance_in(8))
                mitm.sub_type[p] = POT_RESTORE_ABILITIES;
            quant = 1;
            break;

        case POT_GAIN_INTELLIGENCE:
        case POT_LEVITATION:
            // bad things
            switch (random2(6))
            {
            case 0:
            case 4:
                // is this not always the case? - no, level one is 0 {dlb}
                if (item_power > 0)
                {
                    mitm.sub_type[p] = POT_POISON;

                    if (item_power > 10 && one_chance_in(4))
                        mitm.sub_type[p] = POT_STRONG_POISON;

                    break;
                }
            /* **** intentional fall through **** */
            case 1:
                mitm.sub_type[p] = POT_SLOWING;
                break;
            case 2:
                mitm.sub_type[p] = POT_PARALYSIS;
                break;
            case 3:
                mitm.sub_type[p] = POT_CONFUSION;
                break;
            case 5:
                if (item_power > 6)
                {
                    mitm.sub_type[p] = POT_MUTATION;
                    break;
                }
            }

            if (one_chance_in(8))
                mitm.sub_type[p] = POT_DEGENERATION;

            if (one_chance_in(1000))
                mitm.sub_type[p] = POT_DECAY;

            break;
        }

        if (force_type != OBJ_RANDOM)
            mitm.sub_type[p] = force_type;

        mitm.pluses[p] = 0;
        break;

    case OBJ_SCROLLS:
        // determine sub_type:
        if (force_type == OBJ_RANDOM)
        {
            // only used in certain cases {dlb}
            int depth_mod = random2(1 + item_power);

            temp_rand = random2(920);

            mitm.sub_type[p] =
                    ((temp_rand > 751) ? SCR_IDENTIFY :          // 18.26%
                     (temp_rand > 629) ? SCR_REMOVE_CURSE :      // 13.26%
                     (temp_rand > 554) ? SCR_TELEPORTATION :     //  8.15%
                     (temp_rand > 494) ? SCR_DETECT_CURSE :      //  6.52%
                     (temp_rand > 464) ? SCR_FEAR :              //  3.26%
                     (temp_rand > 434) ? SCR_NOISE :             //  3.26%
                     (temp_rand > 404) ? SCR_MAGIC_MAPPING :     //  3.26%
                     (temp_rand > 374) ? SCR_FORGETFULNESS :     //  3.26%
                     (temp_rand > 344) ? SCR_RANDOM_USELESSNESS ://  3.26%
                     (temp_rand > 314) ? SCR_CURSE_WEAPON :      //  3.26%
                     (temp_rand > 284) ? SCR_CURSE_ARMOUR :      //  3.26%
                     (temp_rand > 254) ? SCR_RECHARGING :        //  3.26%
                     (temp_rand > 224) ? SCR_BLINKING :          //  3.26%
                     (temp_rand > 194) ? SCR_PAPER :             //  3.26%
                     (temp_rand > 164) ? SCR_ENCHANT_ARMOUR :    //  3.26%
                     (temp_rand > 134) ? SCR_ENCHANT_WEAPON_I :  //  3.26%
                     (temp_rand > 104) ? SCR_ENCHANT_WEAPON_II : //  3.26%

             // Crawl is kind to newbie adventurers {dlb}:
             // yes -- these five are messy {dlb}:
             // yes they are a hellish mess of tri-ops and long lines,
             // this formating is better -- bwr
                     (temp_rand > 74) ?
                         ((item_power < 4) ? SCR_TELEPORTATION
                                          : SCR_IMMOLATION) :    //  3.26%
                     (temp_rand > 59) ?
                         ((depth_mod < 4) ? SCR_TELEPORTATION
                                          : SCR_ACQUIREMENT) :   //  1.63%
                     (temp_rand > 44) ?
                         ((depth_mod < 4) ? SCR_DETECT_CURSE
                                          : SCR_SUMMONING) :     //  1.63%
                     (temp_rand > 29) ?
                         ((depth_mod < 4) ? SCR_TELEPORTATION    //  1.63%
                                          : SCR_ENCHANT_WEAPON_III) :
                     (temp_rand > 14) ?
                         ((depth_mod < 7) ? SCR_DETECT_CURSE
                                          : SCR_TORMENT)         //  1.63%
                     // default:
                       : ((depth_mod < 7) ? SCR_TELEPORTATION    //  1.63%
                                          : SCR_VORPALISE_WEAPON));
        }
        else
            mitm.sub_type[p] = force_type;

        // determine quantity:
        temp_rand = random2(48);

        quant = ((temp_rand > 1
                  || mitm.sub_type[p] == SCR_VORPALISE_WEAPON
                  || mitm.sub_type[p] == SCR_ACQUIREMENT
                  || mitm.sub_type[p] == SCR_TORMENT) ? 1 :    // 95.83%
                                       (temp_rand > 0) ? 2      //  2.08%
                                                       : 3);    //  2.08%
        mitm.pluses[p] = 0;
        break;

    case OBJ_JEWELLERY:
        // determine whether an unrandart will be generated {dlb}:
        if (item_power > 2 && you.level_type != LEVEL_ABYSS
            && you.level_type != LEVEL_PANDEMONIUM
            && random2(2000) <= 100 + (item_power * 3) && one_chance_in(4))
        {
            icky = find_okay_unrandart(OBJ_JEWELLERY);

            if (icky != -1)
            {
                quant = 1;
                make_item_unrandart(icky, p);
                break;
            }
        }

        // otherwise, determine jewellery type {dlb}:
        if (force_type == OBJ_RANDOM)
        {
            mitm.sub_type[p] = (!one_chance_in(4) ? random2(24)   // rings
                                                   : AMU_RAGE + random2(10));
        }
        else
            mitm.sub_type[p] = force_type;

        // quantity is always one {dlb}:
        quant = 1;

        // everything begins as uncursed, unenchanted jewellery {dlb}:
        mitm.pluses[p] = 50;

        // set pluses for rings that require them {dlb}:
        switch (mitm.sub_type[p])
        {
        case RING_PROTECTION:
        case RING_STRENGTH:
        case RING_SLAYING:
        case RING_EVASION:
        case RING_DEXTERITY:
        case RING_INTELLIGENCE:
            if (one_chance_in(5))       // 20% of such rings are cursed {dlb}
            {
                mitm.pluses[p] = 150;
                mitm.pluses[p] -= (coinflip() ? 2 : 1);

                if (one_chance_in(3))
                    mitm.pluses[p] -= random2(4);
            }
            else
            {
                mitm.pluses[p] += 1 + (one_chance_in(3) ? random2(3)
                                                         : random2avg(6, 2));
            }
            break;

        default:
            break;
        }

        // rings of slaying also require that pluses2 be set {dlb}:
        if (mitm.sub_type[p] == RING_SLAYING)
        {
            mitm.pluses2[p] = 50;
            mitm.pluses2[p] += 1 + (one_chance_in(3) ? random2(3)
                                                      : random2avg(6, 2));

            // should this happen to cursed rings? {dlb}
            if (random2(25) < 9)        // 36% of such rings {dlb}
            {
                mitm.pluses[p] = 50;
                mitm.pluses2[p] += 2;
            }
        }

        // confusing generation of seeming randarts
        // -- how does this fit in? {dlb}
        if (allow_uniques == 1 && item_power > 2
            && random2(2000) <= 100 + (item_power * 3) && coinflip())
        {
            if (mitm.sub_type[p] == RING_PROTECTION)
                mitm.sub_type[p] = RING_PROTECTION_FROM_FIRE + random2(3);

            if (mitm.sub_type[p] == RING_STRENGTH
                || mitm.sub_type[p] == RING_SLAYING)
            {
                mitm.sub_type[p] = RING_SEE_INVISIBLE + random2(4);
            }

            if (mitm.sub_type[p] == RING_EVASION)
                mitm.sub_type[p] = RING_SUSTAIN_ABILITIES;

            if (mitm.sub_type[p] == RING_DEXTERITY
                || mitm.sub_type[p] == RING_INTELLIGENCE)
            {
                mitm.sub_type[p] = RING_WIZARDRY + random2(8);
            }

            // hmmm ... maybe this should work off of pluses? {dlb}
            if (mitm.sub_type[p] == AMU_INACCURACY)
                mitm.sub_type[p] = AMU_RAGE + random2(8);

            /* Can't allow base ring types with +s */
            mitm.special[p] = 200;
            mitm.pluses[p] = random2(70);
            mitm.pluses2[p] = random2(150);
            break;
        }

        // rings of hunger and teleportation are always cursed {dlb}:
        if (mitm.sub_type[p] == RING_HUNGER
            || mitm.sub_type[p] == RING_TELEPORTATION)
        {
            mitm.pluses[p] = 150;
        }
        break;

    case OBJ_BOOKS:
      create_book:
        do
        {
            mitm.sub_type[p] = random2(NUM_BOOKS);

            if (book_rarity(mitm.sub_type[p]) == 100)
                continue;

            if (mitm.sub_type[p] != BOOK_DESTRUCTION
                && mitm.sub_type[p] != BOOK_MANUAL)
            {
                if (one_chance_in(10))
                {
                    if (coinflip())
                        mitm.sub_type[p] = BOOK_WIZARDRY;
                    else
                        mitm.sub_type[p] = BOOK_POWER;
                }

                if (random2(item_power + 1) + 1 >= book_rarity(mitm.sub_type[p])
                    || one_chance_in(100))
                {
                    break;
                }
                else
                {
                    mitm.sub_type[p] = BOOK_DESTRUCTION;
                    continue;
                }
            }
        }
        while (mitm.sub_type[p] == BOOK_DESTRUCTION
                   || mitm.sub_type[p] == BOOK_MANUAL);

        if (book_rarity(mitm.sub_type[p]) == 100)
            goto create_book;

// now why isn't the same sequence #ifdef'd out for staves??? {dlb}
#if 0
        mitm.pluses[p] = 127;
        itoa(127, strungy, 2);

        if (force_type != OBJ_RANDOM)
            mitm.sub_type[p] = force_type;

        // shouldn't let spell no 1 be erased

        spellbook_template(mitm.sub_type[p], fpass);

        for (bkk = 1; bkk < SPELLBOOK_SIZE; bkk++)
            if (fpass[bkk] == SPELL_NO_SPELL)
                strungy[bkk] = '0';

        icky = strlen(strungy);

        multip = 1;
        numbo = 0;

        for (xj = icky; xj >= 1; xj--)
        {
            if (strungy[xj] == '0')
            {
                multip *= 2;
            }
            else if (strungy[xj] == '1')
            {
                numbo += multip;
                multip *= 2;
            }
        }

        strcpy(strungy, "");

        mitm.pluses[p] = numbo + 64;
#endif

        mitm.special[p] = random2(5);

        if (one_chance_in(10))
            mitm.special[p] += random2(8) * 10;

        if (force_type != OBJ_RANDOM)
            mitm.sub_type[p] = force_type;

        quant = 1;

        // tome of destruction : rare!
        if (force_type == BOOK_DESTRUCTION
            || (random2(7000) <= item_power + 20 && item_power > 10
                && force_type == OBJ_RANDOM))
        {
            mitm.sub_type[p] = BOOK_DESTRUCTION;
        }

        // skill manuals - also rare
        // fixed to generate manuals for *all* extant skills - 14mar2000 {dlb}
        if (force_type == BOOK_MANUAL
            || (random2(4000) <= item_power + 20 && item_power > 6
                && force_type == OBJ_RANDOM))
        {
            mitm.sub_type[p] = BOOK_MANUAL;
            mitm.pluses[p] = (one_chance_in(4) ? random2(14) + 25
                                                : random2(19));

            if (mitm.pluses[p] == SK_UNUSED_1)
                mitm.pluses[p] = SK_UNARMED_COMBAT;
        }
        break;

    case OBJ_STAVES:            // this can be parsed, too {dlb}
        mitm.sub_type[p] = random2(10);

        // spell staves {dlb}
        if (one_chance_in(5))
            mitm.sub_type[p] = STAFF_SMITING + random2(9);
        // remaining non-spell staves {dlb}
        if (one_chance_in(15))
            mitm.sub_type[p] = STAFF_AIR + random2(3);

        if (mitm.sub_type[p] >= STAFF_DESTRUCTION_I
            && mitm.sub_type[p] <= STAFF_DESTRUCTION_IV && one_chance_in(3))
        {
            mitm.sub_type[p] = random2(6);
        }

        if (force_type != OBJ_RANDOM)
            mitm.sub_type[p] = force_type;

        mitm.special[p] = random2(9);

        // do we need all this special stuff for staves if it was removed
        // for spellbooks? {dlb}
        // Hopefully, not... that was an ugly hack I removed -- bwr
#if 0
        if (mitm.sub_type[p] >= STAFF_SMITING   // that is, spell staves {dlb}
                && mitm.sub_type[p] <= STAFF_DEMONOLOGY)
        {
            mitm.pluses[p] = 127;

            if (force_type != OBJ_RANDOM)
                mitm.sub_type[p] = force_type;

            itoa(127, strungy, 2);

            if (force_type != OBJ_RANDOM)
                mitm.sub_type[p] = force_type;

            spellbook_template(mitm.sub_type[p] + 40, fpass);

            for (bkk = 1; bkk < SPELLBOOK_SIZE; bkk++)
            {
                if (fpass[bkk] == SPELL_NO_SPELL)
                    strungy[bkk] = '0';
            }

            icky = strlen(strungy);

            multip = 1;
            numbo = 0;

            for (xj = icky; xj >= 1; xj--)
            {
                if (strungy[xj] == '0')
                {
                    multip *= 2;
                }
                else if (strungy[xj] == '1')
                {
                    numbo += multip;
                    multip *= 2;
                }
            }

            strcpy(strungy, "");

            mitm.pluses[p] = numbo + 64;
        }
#endif
        // XXX: Is this the only magic we need from above? -- bwr
        if (force_type != OBJ_RANDOM)
            mitm.sub_type[p] = force_type;

        quant = 1;
        break;

    case OBJ_ORBS:              // always forced in current setup {dlb}
        quant = 1;

        if (force_type != OBJ_RANDOM)
            mitm.sub_type[p] = force_type;

        // I think we only have one type of orb now, so ... {dlb}
        if (mitm.sub_type[p] >= 4 && mitm.sub_type[p] <= 19)
            you.unique_items[mitm.sub_type[p] + 3] = 1;
        break;

    // I think these must always be forced, too ... {dlb}
    case OBJ_MISCELLANY:
        if (force_type == OBJ_RANDOM)
        {
            mitm.sub_type[p] = random2(6);

            if (one_chance_in(6))
                mitm.sub_type[p] = MISC_BOX_OF_BEASTS + random2(10);

            if (mitm.sub_type[p] == MISC_RUNE_OF_ZOT)
                mitm.sub_type[p] = MISC_CRYSTAL_BALL_OF_FIXATION;

            if (mitm.sub_type[p] == MISC_DECK_OF_POWER)
            {
                if (one_chance_in(4))
                    mitm.sub_type[p] = MISC_DECK_OF_POWER;
                else
                    mitm.sub_type[p] = MISC_DECK_OF_TRICKS;
            }

            if (one_chance_in(20))
                mitm.sub_type[p] = MISC_DECK_OF_SUMMONINGS;
        }
        else
        {
            mitm.sub_type[p] = force_type;
        }

        if (mitm.sub_type[p] == MISC_DECK_OF_WONDERS
            || mitm.sub_type[p] == MISC_DECK_OF_SUMMONINGS
            || mitm.sub_type[p] == MISC_DECK_OF_POWER)
        {
            mitm.pluses[p] = 4 + random2(10);
        }

        if (mitm.sub_type[p] == MISC_DECK_OF_TRICKS)
            mitm.pluses[p] = 6 + random2avg(15, 2);

        if (mitm.sub_type[p] == MISC_RUNE_OF_ZOT)
        {
            mitm.pluses[p] =
                   ((you.level_type == LEVEL_PANDEMONIUM) ? 50 :
                    (you.level_type == LEVEL_ABYSS)       ? 51
                                                          : you.where_are_you);
        }

        quant = 1;
        break;

    // that is, everything turns to gold if not enumerated above, so ... {dlb}
    default:
        mitm.base_type[p] = OBJ_GOLD;
        quant = 1 + random2avg(19, 2) + random2(item_power);
        break;
    }

    mitm.quantity[p] = quant;

    // should really only be used for monster inventories.
    if (force_place == 1)
    {
        mitm.x[p] = 0;
        mitm.y[p] = 0;
    }
    else
    {
        do
        {
            mitm.x[p] = random2(GXM);
            mitm.y[p] = random2(GYM);
        }
        while (grd[mitm.x[p]][mitm.y[p]] != DNGN_FLOOR);
    }

    item_colour(p);

    return p;
}                               // end items()

void give_item(int mid, int level_number)
{
    int temp_rand = 0;          // probability determination {dlb}

    int bp = 0;
    int thing_created = 0;
    char hand_used = 0;         // for Ettins etc.

    unsigned char xitc = 0;
    unsigned char xitt = 0;

    int iquan = 0;
    // forces colour and quantity, too for intial weapons {dlb}
    int force_item = 0;

    int force_spec = 250;
    int give_level = level_number;

    // find an usued item slot {dlb}:
    for (bp = 0; bp < MAX_ITEMS - 100; bp++)
    {
        if (mitm.quantity[bp] == 0 || mitm.base_type[bp] == OBJ_UNASSIGNED)
            break;
    }

    if (bp >= MAX_ITEMS - 101)
        return;                 // already too many.

    mitm.quantity[bp] = 0;      // hmmm ... why, I wonder? {dlb}
    mitm.pluses[bp] = 50;
    mitm.pluses2[bp] = 50;
    mitm.special[bp] = 0;

    // this flags things to "goto give_armour" below ... {dlb}
    mitm.base_type[bp] = 101;

    mitm.x[bp] = 0;
    mitm.y[bp] = 0;

    if (menv[mid].type == MONS_DANCING_WEAPON
        && you.where_are_you == BRANCH_HALL_OF_BLADES && one_chance_in(3))
    {
        give_level = 351;
    }

    // moved setting of quantity here to keep it in mind {dlb}
    iquan = 1;
    // I wonder if this is even used, given calls to item() {dlb}

    switch (menv[mid].type)
    {
    case MONS_KOBOLD:
    case MONS_BIG_KOBOLD:
        if (random2(5) < 3)     // give hand weapon
        {
            mitm.base_type[bp] = OBJ_WEAPONS;

            temp_rand = random2(5);
            mitm.sub_type[bp] = ((temp_rand > 2) ? WPN_DAGGER :     // 40%
                                 (temp_rand > 0) ? WPN_SHORT_SWORD  // 40%
                                                 : WPN_CLUB);       // 20%
        }
        else if (random2(5) < 2)        // give darts
        {
            force_spec = 100;
            mitm.base_type[bp] = OBJ_MISSILES;
            mitm.sub_type[bp] = MI_DART;
            iquan = 1 + random2(5);
        }
        else
            goto give_armour;
        break;

    case MONS_HOBGOBLIN:
        if (one_chance_in(3))
            force_spec = 3;

        if (random2(5) < 3)     // give hand weapon
        {
            mitm.base_type[bp] = OBJ_WEAPONS;
            mitm.sub_type[bp] = WPN_CLUB;
        }
        else
            goto give_armour;
        break;

    case MONS_GOBLIN:
        if (one_chance_in(3))
            force_spec = 3;
        // deliberate fall through {dlb}
    case MONS_JESSICA:
    case MONS_IJYB:
        if (random2(5) < 3)     // < 1 // give hand weapon
        {
            mitm.base_type[bp] = OBJ_WEAPONS;
            mitm.sub_type[bp] = (coinflip() ? WPN_DAGGER : WPN_CLUB);
        }
        else
            goto give_armour;
        break;

    case MONS_WIGHT:
    case MONS_NORRIS:
        mitm.base_type[bp] = OBJ_WEAPONS;
        mitm.sub_type[bp] = (one_chance_in(6) ? WPN_AXE + random2(4)
                                              : WPN_MACE + random2(12));

        if (coinflip())
        {
            mitm.pluses[bp] += 1 + random2(3);

            if (one_chance_in(10))
                mitm.special[bp] = SPWPN_FREEZING;
        }

        if (one_chance_in(3))
            mitm.pluses[bp] += 100;
        break;

    case MONS_GNOLL:
    case MONS_OGRE_MAGE:
    case MONS_NAGA_WARRIOR:
    case MONS_GREATER_NAGA:
    case MONS_EDMUND:
    case MONS_DUANE:
        force_spec = 100;

        if (!one_chance_in(5))
        {
            mitm.base_type[bp] = OBJ_WEAPONS;

            temp_rand = random2(5);
            mitm.sub_type[bp] = ((temp_rand >  2) ? WPN_SPEAR : // 40%
                                 (temp_rand == 2) ? WPN_FLAIL : // 20%
                                 (temp_rand == 1) ? WPN_HALBERD // 20%
                                                  : WPN_CLUB);  // 20%
        }
        break;

    case MONS_ORC:
    case MONS_ORC_PRIEST:
        force_spec = 3;
        // deliberate fall through {dlb}
    case MONS_TERENCE:
        if (!one_chance_in(5))
        {
            mitm.base_type[bp] = OBJ_WEAPONS;

            temp_rand = random2(240);
            mitm.sub_type[bp] = ((temp_rand > 209) ? WPN_DAGGER :      //12.50%
                                 (temp_rand > 179) ? WPN_CLUB :        //12.50%
                                 (temp_rand > 152) ? WPN_FLAIL :       //11.25%
                                 (temp_rand > 128) ? WPN_HAND_AXE :    //10.00%
                                 (temp_rand > 108) ? WPN_HAMMER :      // 8.33%
                                 (temp_rand >  88) ? WPN_HALBERD :     // 8.33%
                                 (temp_rand >  68) ? WPN_SHORT_SWORD : // 8.33%
                                 (temp_rand >  48) ? WPN_MACE :        // 8.33%
                                 (temp_rand >  38) ? WPN_WHIP :        // 4.17%
                                 (temp_rand >  28) ? WPN_TRIDENT :     // 4.17%
                                 (temp_rand >  18) ? WPN_SCIMITAR :    // 4.17%
                                 (temp_rand >   8) ? WPN_MORNINGSTAR : // 4.17%
                                 (temp_rand >   2) ? WPN_AXE           // 2.50%
                                                   : WPN_SPIKED_FLAIL);// 1.25%
        }
        else
            goto give_armour;
        break;

    case MONS_DEEP_ELF_FIGHTER:
    case MONS_DEEP_ELF_HIGH_PRIEST:
    case MONS_DEEP_ELF_KNIGHT:
    case MONS_DEEP_ELF_PRIEST:
    case MONS_DEEP_ELF_SOLDIER:
        force_spec = 1;
        mitm.base_type[bp] = OBJ_WEAPONS;

        temp_rand = random2(8);
        mitm.sub_type[bp] = ((temp_rand > 5) ? WPN_LONG_SWORD :    // 2 in 8
                             (temp_rand > 3) ? WPN_SHORT_SWORD :   // 2 in 8
                             (temp_rand > 2) ? WPN_SCIMITAR :      // 1 in 8
                             (temp_rand > 1) ? WPN_MACE :          // 1 in 8
                             (temp_rand > 0) ? WPN_BOW             // 1 in 8
                                             : WPN_HAND_CROSSBOW); // 1 in 8
        break;

    case MONS_DEEP_ELF_ANNIHILATOR:
    case MONS_DEEP_ELF_CONJURER:
    case MONS_DEEP_ELF_DEATH_MAGE:
    case MONS_DEEP_ELF_DEMONOLOGIST:
    case MONS_DEEP_ELF_MAGE:
    case MONS_DEEP_ELF_SORCEROR:
    case MONS_DEEP_ELF_SUMMONER:
        force_spec = 1;
        mitm.base_type[bp] = OBJ_WEAPONS;

        temp_rand = random2(6);
        mitm.sub_type[bp] = ((temp_rand > 3) ? WPN_LONG_SWORD : // 2 in 6
                             (temp_rand > 2) ? WPN_SHORT_SWORD :// 1 in 6
                             (temp_rand > 1) ? WPN_SABRE :      // 1 in 6
                             (temp_rand > 0) ? WPN_DAGGER       // 1 in 6
                                             : WPN_WHIP);       // 1 in 6
        break;

    case MONS_ORC_WARRIOR:
    case MONS_ORC_HIGH_PRIEST:
    case MONS_BLORK_THE_ORC:
        if (force_spec == 250)
            force_spec = 3;
        // deliberate fall-through {dlb}
    case MONS_DANCING_WEAPON:   // give_level may have been adjusted above
    case MONS_FRANCES:
    case MONS_FRANCIS:
    case MONS_HAROLD:
    case MONS_JOSEPH:
    case MONS_LOUISE:
    case MONS_MICHAEL:
    case MONS_NAGA:
    case MONS_NAGA_MAGE:
    case MONS_RUPERT:
    case MONS_SKELETAL_WARRIOR:
    case MONS_WAYNE:
        mitm.base_type[bp] = OBJ_WEAPONS;

        temp_rand = random2(120);
        mitm.sub_type[bp] = ((temp_rand > 109) ? WPN_LONG_SWORD :   // 8.33%
                             (temp_rand >  99) ? WPN_SHORT_SWORD :  // 8.33%
                             (temp_rand >  89) ? WPN_SCIMITAR :     // 8.33%
                             (temp_rand >  79) ? WPN_BATTLEAXE :    // 8.33%
                             (temp_rand >  69) ? WPN_HAND_AXE :     // 8.33%
                             (temp_rand >  59) ? WPN_HALBERD :      // 8.33%
                             (temp_rand >  49) ? WPN_GLAIVE :       // 8.33%
                             (temp_rand >  39) ? WPN_MORNINGSTAR :  // 8.33%
                             (temp_rand >  29) ? WPN_GREAT_MACE :   // 8.33%
                             (temp_rand >  19) ? WPN_TRIDENT :      // 8.33%
                             (temp_rand >  10) ? WPN_AXE :          // 7.50%
                             (temp_rand >   1) ? WPN_FLAIL :        // 7.50%
                             (temp_rand >   0) ? WPN_BROAD_AXE      // 0.83%
                                               : WPN_SPIKED_FLAIL); // 0.83%
        break;

    case MONS_ORC_KNIGHT:
    case MONS_ORC_WARLORD:
        force_spec = 3;
        // deliberate fall-through, I guess {dlb}
    case MONS_NORBERT:
    case MONS_JOZEF:
    case MONS_URUG:
    case MONS_VAULT_GUARD:
    case MONS_VAMPIRE_KNIGHT:
        mitm.base_type[bp] = OBJ_WEAPONS;

        temp_rand = random2(24);
        mitm.sub_type[bp] = ((temp_rand > 19) ? WPN_GREAT_SWORD :// 16.67%
                             (temp_rand > 15) ? WPN_LONG_SWORD : // 16.67%
                             (temp_rand > 11) ? WPN_BATTLEAXE :  // 16.67%
                             (temp_rand >  7) ? WPN_AXE :        // 16.67%
                             (temp_rand >  5) ? WPN_GREAT_MACE : //  8.33%
                             (temp_rand >  3) ? WPN_GREAT_FLAIL ://  8.33%
                             (temp_rand >  1) ? WPN_GLAIVE :     //  8.33%
                             (temp_rand >  0) ? WPN_BROAD_AXE    //  4.17%
                                              : WPN_HALBERD);    //  4.17%

        if (one_chance_in(4))
            mitm.pluses[bp] += 1 + random2(3);
        break;

    case MONS_CYCLOPS:
    case MONS_STONE_GIANT:
        force_spec = 100;
        mitm.base_type[bp] = OBJ_MISSILES;
        mitm.sub_type[bp] = MI_LARGE_ROCK;
        iquan = 1 + random2(3);
        break;

    case MONS_TWO_HEADED_OGRE:
        force_spec = 100;
        hand_used = 0;

        if (menv[mid].inv[MSLOT_WEAPON] != NON_ITEM)
            hand_used = 1;

        mitm.base_type[bp] = OBJ_WEAPONS;
        mitm.sub_type[bp] = (one_chance_in(3) ? WPN_GIANT_SPIKED_CLUB
                                              : WPN_GIANT_CLUB);

        if (one_chance_in(10))
        {
            mitm.sub_type[bp] = ((one_chance_in(10)) ? WPN_GREAT_FLAIL
                                                     : WPN_GREAT_MACE);
        }
        break;

    case MONS_REAPER:
    case MONS_SIGMUND:
        force_spec = 100;
        mitm.base_type[bp] = OBJ_WEAPONS;
        mitm.sub_type[bp] = WPN_SCYTHE;
        break;

    case MONS_BALRUG:
        force_spec = 100;
        mitm.base_type[bp] = OBJ_WEAPONS;
        mitm.sub_type[bp] = WPN_DEMON_WHIP;
        break;

    case MONS_RED_DEVIL:
        if (!one_chance_in(3))
        {
            force_spec = 100;
            mitm.base_type[bp] = OBJ_WEAPONS;
            mitm.sub_type[bp] = (one_chance_in(3) ? WPN_DEMON_TRIDENT
                                                  : WPN_TRIDENT);
        }
        break;

    case MONS_OGRE:
    case MONS_HILL_GIANT:
    case MONS_EROLCHA:
        force_spec = 100;
        mitm.base_type[bp] = OBJ_WEAPONS;

        mitm.sub_type[bp] = (one_chance_in(3) ? WPN_GIANT_SPIKED_CLUB
                                              : WPN_GIANT_CLUB);

        if (one_chance_in(10))
        {
            mitm.sub_type[bp] = (one_chance_in(10) ? WPN_GREAT_FLAIL
                                                   : WPN_GREAT_MACE);
        }
        break;

    case MONS_CENTAUR:
    case MONS_CENTAUR_WARRIOR:
        force_spec = 100;
        mitm.base_type[bp] = OBJ_WEAPONS;
        mitm.sub_type[bp] = WPN_BOW;
        break;

    case MONS_YAKTAUR:
    case MONS_YAKTAUR_CAPTAIN:
        force_spec = 100;
        mitm.base_type[bp] = OBJ_WEAPONS;
        mitm.sub_type[bp] = WPN_CROSSBOW;
        break;

    case MONS_EFREET:
    case MONS_ERICA:
        force_item = 1;
        force_spec = 100;
        mitm.base_type[bp] = OBJ_WEAPONS;
        mitm.sub_type[bp] = WPN_SCIMITAR;
        mitm.special[bp] = SPWPN_FLAMING;
        mitm.pluses[bp] = 50 + random2(5);
        mitm.pluses2[bp] = 50 + random2(5);
        mitm.colour[bp] = RED;  // forced by force_item above {dlb}
        break;

    case MONS_ANGEL:
        force_item = 1;
        mitm.base_type[bp] = OBJ_WEAPONS;
        mitm.colour[bp] = WHITE;        // forced by force_item above {dlb}

        if (one_chance_in(3))
        {
            mitm.sub_type[bp] = (one_chance_in(3) ? WPN_GREAT_MACE : WPN_MACE);
            mitm.special[bp] = 74;      /* glowing, disruption */
        }
        else
        {
            mitm.sub_type[bp] = WPN_LONG_SWORD;
            mitm.special[bp] = 60;      /* glowing */
        }

        mitm.pluses[bp] = 51 + random2(3);
        mitm.pluses2[bp] = 51 + random2(3);
        break;

    case MONS_DAEVA:
        force_item = 1;
        mitm.base_type[bp] = OBJ_WEAPONS;
        mitm.colour[bp] = WHITE;        // forced by force_item above {dlb}

        mitm.sub_type[bp] = (one_chance_in(4) ? WPN_GREAT_SWORD
                                              : WPN_LONG_SWORD);

        mitm.special[bp] = 63;  /* glowing, holy wrath */
        mitm.pluses[bp] = 51 + random2(3);
        mitm.pluses2[bp] = 51 + random2(3);
        break;

    case MONS_HELL_KNIGHT:
    case MONS_MAUD:
    case MONS_ADOLF:
    case MONS_MARGERY:
        force_item = 1;
        mitm.base_type[bp] = OBJ_WEAPONS;
        mitm.sub_type[bp] = WPN_LONG_SWORD + random2(3);

        if (one_chance_in(7))
            mitm.sub_type[bp] = WPN_HALBERD;
        if (one_chance_in(7))
            mitm.sub_type[bp] = WPN_GLAIVE;
        if (one_chance_in(7))
            mitm.sub_type[bp] = WPN_GREAT_MACE;
        if (one_chance_in(7))
            mitm.sub_type[bp] = WPN_BATTLEAXE;
        if (one_chance_in(7))
            mitm.sub_type[bp] = WPN_AXE;
        if (one_chance_in(7))
            mitm.sub_type[bp] = WPN_BROAD_AXE;
        if (one_chance_in(7))
            mitm.sub_type[bp] = WPN_DEMON_TRIDENT;
        if (one_chance_in(7))
            mitm.sub_type[bp] = WPN_DEMON_BLADE;
        if (one_chance_in(7))
            mitm.sub_type[bp] = WPN_DEMON_WHIP;

        mitm.special[bp] = 30 * random2(3); // normal, runed, or glowing {dlb}

        if (one_chance_in(5))
            mitm.special[bp]++; // normal or flaming {dlb}

        if (one_chance_in(5))
            mitm.special[bp] += 8;      // draining or speed {dlb}

        if (one_chance_in(5))
            mitm.special[bp] += 10;     // vorpal, flame, (18?), or (19?) {dlb}

        mitm.pluses[bp] += random2(6);
        mitm.pluses2[bp] = 50 + random2(6);

        mitm.colour[bp] = RED;  // forced by force_item above {dlb}

        if (one_chance_in(3))
            mitm.colour[bp] = DARKGREY;
        if (one_chance_in(5))
            mitm.colour[bp] = CYAN;
        break;

    case MONS_FIRE_GIANT:
        force_item = 1;
        mitm.base_type[bp] = OBJ_WEAPONS;
        mitm.sub_type[bp] = WPN_GREAT_SWORD;
        mitm.special[bp] = SPWPN_FLAMING;
        mitm.pluses[bp] = 50;
        mitm.pluses2[bp] = 50;

        mitm.colour[bp] = RED;  // forced by force_item above {dlb}
        if (one_chance_in(3))
            mitm.colour[bp] = DARKGREY;
        if (one_chance_in(5))
            mitm.colour[bp] = CYAN;
        break;

    case MONS_FROST_GIANT:
        force_item = 1;
        mitm.base_type[bp] = OBJ_WEAPONS;
        mitm.sub_type[bp] = WPN_BATTLEAXE;
        mitm.special[bp] = SPWPN_FREEZING;
        mitm.pluses[bp] = 50;
        mitm.pluses2[bp] = 50;
        // forced by force_item above {dlb}
        mitm.colour[bp] = (one_chance_in(3) ? WHITE : CYAN);
        break;

    case MONS_KOBOLD_DEMONOLOGIST:
    case MONS_ORC_WIZARD:
    case MONS_ORC_SORCEROR:
        force_spec = 3;
        // deliberate fall-through, I guess {dlb}
    case MONS_NECROMANCER:
    case MONS_WIZARD:
    case MONS_PSYCHE:
    case MONS_DONALD:
    case MONS_JOSEPHINE:
    case MONS_AGNES:
        mitm.base_type[bp] = OBJ_WEAPONS;
        mitm.sub_type[bp] = WPN_DAGGER;
        break;

    case MONS_CEREBOV:
        force_item = 1;
        mitm.base_type[bp] = OBJ_WEAPONS;
        mitm.sub_type[bp] = WPN_GREAT_SWORD;
        mitm.special[bp] = NWPN_SWORD_OF_CEREBOV;
        mitm.pluses[bp] = 156;
        mitm.pluses2[bp] = 56;
        mitm.colour[bp] = YELLOW;       // forced by force_item above {dlb}
        break;

    case MONS_DISPATER:
        force_item = 1;
        mitm.base_type[bp] = OBJ_WEAPONS;
        mitm.sub_type[bp] = WPN_QUARTERSTAFF;
        mitm.special[bp] = NWPN_STAFF_OF_DISPATER;
        mitm.pluses[bp] = 54;
        mitm.pluses2[bp] = 54;
        mitm.colour[bp] = YELLOW;       // forced by force_item above {dlb}
        break;

    case MONS_ASMODEUS:
        force_item = 1;
        mitm.base_type[bp] = OBJ_WEAPONS;
        mitm.sub_type[bp] = WPN_MACE;
        mitm.special[bp] = NWPN_SCEPTRE_OF_ASMODEUS;
        mitm.pluses[bp] = 57;
        mitm.pluses2[bp] = 57;
        mitm.colour[bp] = RED;  // forced by force_item above {dlb}
        break;

    case MONS_GERYON:
        mitm.base_type[bp] = OBJ_MISCELLANY;
        mitm.sub_type[bp] = MISC_HORN_OF_GERYON;
        break;
    }                           // end "switch(menv[mid].type)"


    // only happens if something in above switch doesn't set it {dlb}
    if (mitm.base_type[bp] == 101)
    {
        mitm.base_type[bp] = OBJ_UNASSIGNED;
        goto give_armour;
    }

    if (force_item)
        mitm.quantity[bp] = iquan;

    xitc = mitm.base_type[bp];
    xitt = mitm.sub_type[bp];

    thing_created = ((force_item) ? bp : items(0, xitc, xitt, 1,
                                                    give_level, force_spec));

    mitm.x[thing_created] = 0;
    mitm.y[thing_created] = 0;
    mitm.id[thing_created] = 0;

    if (mitm.base_type[thing_created] == OBJ_WEAPONS)
        // hand_used = 0 unless Ettin's 2nd hand etc.
        menv[mid].inv[hand_used] = thing_created;
    else if (mitm.base_type[thing_created] == OBJ_MISSILES)
        menv[mid].inv[MSLOT_MISSILE] = thing_created;
    else if (mitm.base_type[thing_created] == OBJ_SCROLLS)
        menv[mid].inv[MSLOT_SCROLL] = thing_created;
    // but not potions? huh? {dlb}
    // only Geryon gets something other than weapon explicitly {dlb}
    else if (mitm.base_type[thing_created] == OBJ_GOLD
             || mitm.base_type[thing_created] == OBJ_MISCELLANY)
    {
        menv[mid].inv[MSLOT_POTION] = thing_created;
    }

    // SPWPN_PROTECTION and NWPN_S_o_Z ??? {dlb}
    if (mitm.base_type[thing_created] == OBJ_WEAPONS
        && mitm.special[thing_created] % 30 == SPWPN_PROTECTION)
    {
        menv[mid].armor_class += 5;
    }

    if (!force_item)
        item_colour(thing_created);

  give_armour:

    // actually, should be give_ammo now

    // this bit gives ammunition to go with missile hand weapons.
    // only gives darts for hand xbows

    // note that force_spec is not reset for this section

    if (menv[mid].inv[MSLOT_WEAPON] != NON_ITEM
        && launches_things(mitm.sub_type[menv[mid].inv[MSLOT_WEAPON]]))
    {
        for (bp = 0; bp < MAX_ITEMS - 100; bp++)
        {
            if (mitm.quantity[bp] == 0 || mitm.base_type[bp] == OBJ_UNASSIGNED)
                break;
        }

        if (bp >= MAX_ITEMS - 100)
            return;             // already too many.

        mitm.pluses[bp] = 50;
        mitm.pluses2[bp] = 0;
        mitm.special[bp] = SPWPN_NORMAL;
        force_item = 0;

        mitm.base_type[bp] = OBJ_MISSILES;
        mitm.sub_type[bp] = launched_by(mitm.sub_type[menv[mid].inv[MSLOT_WEAPON]]);
        iquan = 3 + random2avg(16, 2);

        // that is, lose SPWPN's, I think, but retain racial typing {dlb}
        mitm.special[bp] = (mitm.special[menv[mid].inv[MSLOT_WEAPON]] / 30) * 30;

        if (force_item)
            mitm.quantity[bp] = iquan;

        xitc = mitm.base_type[bp];
        xitt = mitm.sub_type[bp];

        thing_created = ((force_item) ? bp : items( 0, xitc, xitt, 1,
                                                    give_level, force_spec ));

        mitm.x[thing_created] = 0;
        mitm.y[thing_created] = 0;
        mitm.id[thing_created] = 0;
        menv[mid].inv[MSLOT_MISSILE] = thing_created;

        // again, SPWPN_PROTECTION + ???, I think {dlb}
        if (mitm.base_type[thing_created] == OBJ_WEAPONS
            && mitm.special[thing_created] % 30 == SPWPN_PROTECTION)
        {
            menv[mid].armor_class += 3;
        }

        item_colour(thing_created);
    }                           // end if needs ammo


    // now, the section that gives armour out {dlb}:
    //bp = 0;    // I do not think this is necessary, see following loop: {dlb}
    for (bp = 0; bp < MAX_ITEMS - 100; bp++)
    {
        if (mitm.quantity[bp] == 0 || mitm.base_type[bp] == OBJ_UNASSIGNED)
            break;
    }

    if (bp >= MAX_ITEMS - 100)
        return;                 // already too many.

    force_spec = 250;

    switch (menv[mid].type)
    {
    case MONS_DEEP_ELF_ANNIHILATOR:
    case MONS_DEEP_ELF_CONJURER:
    case MONS_DEEP_ELF_DEATH_MAGE:
    case MONS_DEEP_ELF_DEMONOLOGIST:
    case MONS_DEEP_ELF_FIGHTER:
    case MONS_DEEP_ELF_HIGH_PRIEST:
    case MONS_DEEP_ELF_KNIGHT:
    case MONS_DEEP_ELF_MAGE:
    case MONS_DEEP_ELF_PRIEST:
    case MONS_DEEP_ELF_SOLDIER:
    case MONS_DEEP_ELF_SORCEROR:
    case MONS_DEEP_ELF_SUMMONER:
        if (force_spec == 250)
            force_spec = 1;
        // deliberate fall through {dlb}
    case MONS_ORC_KNIGHT:
    case MONS_ORC_WARLORD:
    case MONS_ORC_WARRIOR:
        if (force_spec == 250)
            force_spec = 3;
        // deliberate fall through {dlb}
    case MONS_IJYB:
    case MONS_ORC:
    case MONS_ORC_HIGH_PRIEST:
    case MONS_ORC_PRIEST:
    case MONS_ORC_SORCEROR:
        if (force_spec == 250)
            force_spec = 3;
        // deliberate fall through {dlb}
    case MONS_ERICA:
    case MONS_HAROLD:
    case MONS_JOSEPH:
    case MONS_JOSEPHINE:
    case MONS_JOZEF:
    case MONS_NORBERT:
    case MONS_PSYCHE:
    case MONS_TERENCE:
        if (random2(5) < 2)
        {
            mitm.base_type[bp] = OBJ_ARMOUR;

            switch (random2(8))
            {
            case 0:
            case 1:
            case 2:
            case 3:
                mitm.sub_type[bp] = ARM_LEATHER_ARMOUR;
                break;
            case 4:
            case 5:
                mitm.sub_type[bp] = ARM_RING_MAIL;
                break;
            case 6:
                mitm.sub_type[bp] = ARM_SCALE_MAIL;
                break;
            case 7:
                mitm.sub_type[bp] = ARM_CHAIN_MAIL;
                break;
            }

            if (mitm.sub_type[bp] == ARM_LEATHER_ARMOUR)
                mitm.colour[bp] = BROWN;
            else
                mitm.colour[bp] = LIGHTCYAN;
        }
        else
            return;
        break;

    case MONS_DUANE:
    case MONS_EDMUND:
    case MONS_RUPERT:
    case MONS_URUG:
    case MONS_WAYNE:
        mitm.base_type[bp] = OBJ_ARMOUR;
        mitm.sub_type[bp] = 1 + random2(4);
        if (mitm.sub_type[bp] == ARM_LEATHER_ARMOUR)
            mitm.colour[bp] = BROWN;
        else
            mitm.colour[bp] = LIGHTCYAN;
        break;

    case MONS_ADOLF:
    case MONS_HELL_KNIGHT:
    case MONS_LOUISE:
    case MONS_MARGERY:
    case MONS_MAUD:
    case MONS_VAMPIRE_KNIGHT:
    case MONS_VAULT_GUARD:
        mitm.base_type[bp] = OBJ_ARMOUR;
        mitm.sub_type[bp] = ARM_CHAIN_MAIL + random2(4);
        mitm.colour[bp] = LIGHTCYAN;
        break;

    case MONS_ANGEL:
    case MONS_SIGMUND:
    case MONS_WIGHT:
        force_spec = 100;
        mitm.base_type[bp] = OBJ_ARMOUR;
        mitm.sub_type[bp] = ARM_ROBE;
        mitm.colour[bp] = WHITE;
        break;

    case MONS_NAGA:
    case MONS_NAGA_MAGE:
    case MONS_NAGA_WARRIOR:
        if (!one_chance_in(3))
            return;
        // deliberate fall through {dlb}
    case MONS_DONALD:
    case MONS_GREATER_NAGA:
    case MONS_JESSICA:
    case MONS_KOBOLD_DEMONOLOGIST:
    case MONS_OGRE_MAGE:
    case MONS_ORC_WIZARD:
    case MONS_WIZARD:
        force_spec = 100;
        mitm.base_type[bp] = OBJ_ARMOUR;
        mitm.sub_type[bp] = ARM_ROBE;
        mitm.colour[bp] = random_colour();
        break;

    case MONS_AGNES:
    case MONS_BLORK_THE_ORC:
    case MONS_BORIS:
    case MONS_FRANCES:
    case MONS_FRANCIS:
    case MONS_NECROMANCER:
    case MONS_VAMPIRE_MAGE:
        mitm.base_type[bp] = OBJ_ARMOUR;
        mitm.sub_type[bp] = ARM_ROBE;
        mitm.colour[bp] = DARKGREY;
        break;

    default:
        return;
    }                           // end of switch(menv [mid].type)

    // because it may have been set earlier by giving ammo or weapons {dlb}
    iquan = 1;

    xitc = mitm.base_type[bp];
    xitt = mitm.sub_type[bp];

    thing_created = items(0, xitc, xitt, 1, 1 + (level_number / 2), force_spec);

    mitm.x[thing_created] = 0;
    mitm.y[thing_created] = 0;
    menv[mid].inv[MSLOT_ARMOUR] = thing_created;

    // Wights' robes are white:
    // this completely overrides colouring above -- d'oh! {dlb}
    if (menv[mid].type != MONS_WIGHT)
        item_colour(thing_created);
    else
        mitm.colour[thing_created] = WHITE;

    menv[mid].armor_class += property( mitm.base_type[thing_created],
                                        mitm.sub_type[thing_created], PARM_AC );

    int armour_plus = 0;

    armour_plus = mitm.pluses[thing_created]
                        - ((mitm.pluses[thing_created] > 130) ? 150 : 50);

    ASSERT(abs(armour_plus) < 20);

    if (abs(armour_plus) < 20)
        menv[mid].armor_class += armour_plus;

    menv[mid].evasion += property( mitm.base_type[thing_created],
                                    mitm.sub_type[thing_created],
                                    PARM_EVASION ) / 2;

    if (menv[mid].evasion < 1)
        menv[mid].evasion = 1;   // This *shouldn't* happen.
}                               // end give_item()

//---------------------------------------------------------------------------
//                           PRIVATE HELPER FUNCTIONS
//---------------------------------------------------------------------------

static bool is_weapon_special(int the_weapon)
{
    return ((mitm.special[the_weapon] % 30) != SPWPN_NORMAL);
}                               // end is_weapon_special()

static void set_weapon_special(int the_weapon, int spwpn)
{
    mitm.special[the_weapon] = (mitm.special[the_weapon] / 30) * 30 + spwpn;
}                               // end set_weapon_special()

static void check_doors(void)
{
    unsigned char ig;
    unsigned char solid_count = 0;      // clarifies innermost loop {dlb}
    int x,y;

    for (x = 1; x < GXM-1; x++)
    {
        for (y = 1; y < GYM-1; y++)
        {
            ig = grd[x][y];

            if (ig != DNGN_CLOSED_DOOR)
                continue;

            solid_count = 0;

            // first half of each conditional represents bounds checking {dlb}:
            if (grd[x - 1][y] < DNGN_LAST_SOLID_TILE)
                solid_count++;

            if (grd[x + 1][y] < DNGN_LAST_SOLID_TILE)
                solid_count++;

            if (grd[x][y - 1] < DNGN_LAST_SOLID_TILE)
                solid_count++;

            if (grd[x][y + 1] < DNGN_LAST_SOLID_TILE)
                solid_count++;

            grd[x][y] = ((solid_count < 2) ? DNGN_FLOOR : DNGN_CLOSED_DOOR);
        }
    }
}                               // end check_doors()

static void hide_doors(void)
{
    unsigned char dx = 0, dy = 0;     // loop variables
    unsigned char wall_count = 0;     // clarifies inner loop {dlb}

    for (dx = 1; dx < GXM-1; dx++)
    {
        for (dy = 1; dy < GYM-1; dy++)
        {
            // only one out of four doors are candidates for hiding {gdl}:
            if (grd[dx][dy] == DNGN_CLOSED_DOOR && one_chance_in(4))
            {
                wall_count = 0;

                if (grd[dx - 1][dy] == DNGN_ROCK_WALL)
                    wall_count++;

                if (grd[dx + 1][dy] == DNGN_ROCK_WALL)
                    wall_count++;

                if (grd[dx][dy - 1] == DNGN_ROCK_WALL)
                    wall_count++;

                if (grd[dx][dy + 1] == DNGN_ROCK_WALL)
                    wall_count++;

                // if door is attached to more than one wall, hide it {dlb}:
                if (wall_count > 1)
                    grd[dx][dy] = DNGN_SECRET_DOOR;
            }
        }
    }
}                               // end hide_doors()

static void prepare_swamp(void)
{
    int i, j;                   // loop variables
    int temp_rand;              // probability determination {dlb}

    for (i = 10; i < (GXM - 10); i++)
    {
        for (j = 10; j < (GYM - 10); j++)
        {
            // doors -> floors {dlb}
            if (grd[i][j] == DNGN_CLOSED_DOOR || grd[i][j] == DNGN_SECRET_DOOR)
                grd[i][j] = DNGN_FLOOR;

            // floors -> shallow water 1 in 3 times {dlb}
            if (grd[i][j] == DNGN_FLOOR && one_chance_in(3))
                grd[i][j] = DNGN_SHALLOW_WATER;

            // walls -> deep/shallow water or remain unchanged {dlb}
            if (grd[i][j] == DNGN_ROCK_WALL)
            {
                temp_rand = random2(6);

                if (temp_rand > 0)      // 17% chance unchanged {dlb}
                {
                    grd[i][j] = ((temp_rand > 2) ? DNGN_SHALLOW_WATER // 50%
                                                 : DNGN_DEEP_WATER);  // 33%
                }
            }
        }
    }
}                               // end prepare_swamp()

// Gives water which is next to ground/shallow water a chance of being
// shallow. Checks each water space.
static void prepare_water(void)
{
    int i, j, k, l;             // loop variables {dlb}
    unsigned char which_grid;   // code compaction {dlb}

    for (i = 10; i < (GXM - 10); i++)
    {
        for (j = 10; j < (GYM - 10); j++)
        {
            if (grd[i][j] == DNGN_DEEP_WATER)
            {
                for (k = -1; k < 2; k++)
                {
                    for (l = -1; l < 2; l++)
                    {
                        if (k != 0 || l != 0)
                        {
                            which_grid = grd[i + k][j + l];

                            if (which_grid == DNGN_SHALLOW_WATER
                                && one_chance_in(8))  // must come first {dlb}
                            {
                                grd[i][j] = DNGN_SHALLOW_WATER;
                            }
                            else if (which_grid != DNGN_DEEP_WATER
                                     && !one_chance_in(6))
                            {
                                grd[i][j] = DNGN_SHALLOW_WATER;
                            }
                        }
                    }
                }
            }
        }
    }
}                               // end prepare_water()

static bool find_in_area(int sx, int sy, int ex, int ey, unsigned char feature)
{
    int x,y;

    if (feature != 0)
        for(x=sx; x<=ex; x++)
            for(y=sy; y<=ey; y++)
                if (grd[x][y] == feature)
                    return true;

    return false;
}

// stamp a box.  can avoid a possible type,  and walls and floors can
// be different (or not stamped at all)
// Note that the box boundaries are INclusive.
static bool make_box(int room_x1, int room_y1, int room_x2, int room_y2,
    unsigned char floor, unsigned char wall, unsigned char avoid)
{
    int bx,by;

    // check for avoidance
    if (find_in_area(room_x1, room_y1, room_x2, room_y2, avoid))
        return false;

    // draw walls
    if (wall != 0)
    {
        for(bx=room_x1; bx<=room_x2; bx++)
        {
            grd[bx][room_y1] = wall;
            grd[bx][room_y2] = wall;
        }
        for(by=room_y1+1; by<room_y2; by++)
        {
            grd[room_x1][by] = wall;
            grd[room_x2][by] = wall;
        }
    }

    // draw floor
    if (floor != 0)
    {
        for(bx=room_x1 + 1; bx < room_x2; bx++)
            for(by=room_y1 + 1; by < room_y2; by++)
                grd[bx][by] = floor;
    }

    return true;
}

// take care of labyrinth, abyss, pandemonium
// returns 1 if we should skip further generation,
// -1 if we should immediately quit,  and 0 otherwise.
static int builder_by_type(int level_number, char level_type)
{
    if (level_type == LEVEL_LABYRINTH)
    {
        labyrinth_level(level_number);
        return -1;
    }

    if (level_type == LEVEL_ABYSS)
    {
        generate_abyss();
        return 1;
    }

    if (level_type == LEVEL_PANDEMONIUM)
    {
        char which_demon = -1;
        // Could do spotty_level, but that doesn't always put all paired
        // stairs reachable from each other which isn't a problem in normal
        // dungeon but could be in Pandemonium
        if (one_chance_in(15))
        {
            do
            {
                which_demon = random2(4);

                // makes these things less likely as you find more
                if (one_chance_in(4))
                {
                    which_demon = -1;
                    break;
                }
            }
            while (you.unique_creatures[40 + which_demon] == 1);
        }

        if (which_demon >= 0)
        {
            you.unique_creatures[40 + which_demon] = 1;
            build_vaults(level_number, which_demon + 60);
        }
        else
        {
            plan_main(level_number, 0);
            build_minivaults(level_number, 300 + random2(9));
        }

        return 1;
    }

    // must be normal dungeon
    return 0;
}

// returns 1 if we should skip further generation,
// -1 if we should immediately quit,  and 0 otherwise.
static int builder_by_branch(int level_number)
{
    switch (you.where_are_you)
    {
    case BRANCH_HIVE:
        if (level_number == you.branch_stairs[STAIRS_HIVE]
            + branch_depth(STAIRS_HIVE))
            build_vaults(level_number, 80);
        else
            spotty_level(false, 100 + random2(500), false);
        return 1;

    case BRANCH_SLIME_PITS:
        if (level_number == you.branch_stairs[STAIRS_SLIME_PITS]
            + branch_depth(STAIRS_SLIME_PITS))
            build_vaults(level_number, 81);
        else
            spotty_level(false, 100 + random2(500), false);
        return 1;

    case BRANCH_VAULTS:
        if (level_number == you.branch_stairs[STAIRS_VAULTS]
            + branch_depth(STAIRS_VAULTS))
        {
            build_vaults(level_number, 82);
            return 1;
        }
        break;

    case BRANCH_HALL_OF_BLADES:
        if (level_number == you.branch_stairs[STAIRS_HALL_OF_BLADES]
            + branch_depth(STAIRS_HALL_OF_BLADES))
        {
            build_vaults(level_number, 83);
            return 1;
        }
        break;

    case BRANCH_HALL_OF_ZOT:
        if (level_number == you.branch_stairs[STAIRS_HALL_OF_ZOT]
            + branch_depth(STAIRS_HALL_OF_ZOT))
        {
            build_vaults(level_number, 84);
            return 1;
        }
        break;

    case BRANCH_ECUMENICAL_TEMPLE:
        if (level_number == you.branch_stairs[STAIRS_ECUMENICAL_TEMPLE]
                                    + branch_depth(STAIRS_ECUMENICAL_TEMPLE))
        {
            build_vaults(level_number, 85);
            return 1;
        }
        break;

    case BRANCH_SNAKE_PIT:
        if (level_number == you.branch_stairs[STAIRS_SNAKE_PIT]
                                    + branch_depth(STAIRS_SNAKE_PIT))
        {
            build_vaults(level_number, 86);
            return 1;
        }
        break;

    case BRANCH_ELVEN_HALLS:
        if (level_number == you.branch_stairs[STAIRS_ELVEN_HALLS]
                                    + branch_depth(STAIRS_ELVEN_HALLS))
        {
            build_vaults(level_number, 87);
            return 1;
        }
        break;

    case BRANCH_TOMB:
        if (level_number == you.branch_stairs[STAIRS_TOMB] + 1)
        {
            build_vaults(level_number, 88);
            return 1;
        }
        else if (level_number == you.branch_stairs[STAIRS_TOMB] + 2)
        {
            build_vaults(level_number, 89);
            return 1;
        }
        else if (level_number == you.branch_stairs[STAIRS_TOMB] + 3)
        {
            build_vaults(level_number, 90);
            return 1;
        }
        break;

    case BRANCH_SWAMP:
        if (level_number == you.branch_stairs[STAIRS_SWAMP]
                                            + branch_depth(STAIRS_SWAMP))
        {
            build_vaults(level_number, 91);
            return 1;
        }
        break;

    case BRANCH_ORCISH_MINES:
        spotty_level(false, 100 + random2(500), false);
        return 1;

    case BRANCH_LAIR:
        if (!one_chance_in(3))
        {
            spotty_level(false, 100 + random2(500), false);
            return 1;
        }
        break;

    case BRANCH_VESTIBULE_OF_HELL:
        build_vaults(level_number, 50);
        link_items();
        return -1;

    case BRANCH_DIS:
        if (level_number == 27)
        {
            build_vaults(level_number, 51);
            return 1;
        }
        break;

    case BRANCH_GEHENNA:
        if (level_number == 27)
        {
            build_vaults(level_number, 52);
            return 1;
        }
        break;

    case BRANCH_COCYTUS:
        if (level_number == 27)
        {
            build_vaults(level_number, 53);
            return 1;
        }
        break;

    case BRANCH_TARTARUS:
        if (level_number == 27)
        {
            build_vaults(level_number, 54);
            return 1;
        }
        break;

    default:
        break;
    }
    return 0;
}

// returns 1 if we should dispense with city building,
// 0 otherwise.  Also sets special_room if one is generated
// so that we can link it up later.

static int builder_normal(int level_number, char level_type, spec_room &sr)
{
    bool skipped = false;
    bool done_city = false;

    if (you.where_are_you == BRANCH_MAIN_DUNGEON && level_type == LEVEL_DUNGEON
        && level_number > 7 && level_number < 23 && one_chance_in(9))
    {
        // Can't have vaults on you.where_are_you != BRANCH_MAIN_DUNGEON levels
        build_vaults(level_number, 100);
        return 1;
    }

    if (you.where_are_you == BRANCH_VAULTS)
    {
        if (one_chance_in(3))
            city_level(level_number);
        else
            plan_main(level_number, 4);
        return 1;
    }

    if (level_number > 7 && level_number < 23)
    {
        if (one_chance_in(18))
        {
            spotty_level(false, 0, coinflip());
            return 1;
        }

        if (one_chance_in(18))
        {
            bigger_room();
            return 1;
        }
    }

    if (level_number > 2 && level_number < 23 && one_chance_in(3))
    {
        plan_main(level_number, 0);
        if (one_chance_in(3))
            build_minivaults(level_number, 200);
        return 1;
    }

    if (one_chance_in(3))
        skipped = true;

    //V was 3
    if (!skipped && one_chance_in(7))
    {
        // sometimes roguey_levels generate a special room
        roguey_level(level_number, sr);

        if (level_number > 6
            && you.where_are_you == BRANCH_MAIN_DUNGEON
            && level_type == LEVEL_DUNGEON && one_chance_in(4))
        {
            build_minivaults(level_number, 200);
            return 1;
        }
    }
    else
    {
        if (!skipped && level_number > 13 && one_chance_in(8))
        {
            if (one_chance_in(3))
                city_level(level_number);
            else
                plan_main(level_number, 4);
            done_city = true;
        }
    }

    // maybe create a special room,  if roguey_level hasn't done it
    // already.
    if (!sr.created && level_number > 5 && !done_city && one_chance_in(5))
        special_room(level_number, sr);

    return 0;
}

// returns 1 if we should skip extras(),  otherwise 0
static int builder_basic(int level_number)
{
    int temp_rand;
    int doorlevel = random2(11);
    int corrlength = 2 + random2(14);
    int roomsize = 4 + random2(5) + random2(6);
    int no_corr = (one_chance_in(100) ? 500 + random2(500) : 30 + random2(200));
    int intersect_chance = (one_chance_in(20) ? 400 : random2(20));

    make_trail(35,30,35,20,corrlength,intersect_chance,no_corr,
        DNGN_STONE_STAIRS_DOWN_I, DNGN_STONE_STAIRS_UP_I);

    make_trail(10,15,10,15,corrlength,intersect_chance,no_corr,
        DNGN_STONE_STAIRS_DOWN_II, DNGN_STONE_STAIRS_UP_II);

    make_trail(50,20,10,15,corrlength,intersect_chance,no_corr,
        DNGN_STONE_STAIRS_DOWN_III, DNGN_STONE_STAIRS_UP_III);

    if (one_chance_in(4))
        make_trail(10,20,40,20,corrlength,intersect_chance,no_corr,
            DNGN_ROCK_STAIRS_DOWN);

    if (one_chance_in(4))
        make_trail(50,20,40,20,corrlength,intersect_chance,no_corr,
            DNGN_ROCK_STAIRS_UP);


    if (level_number > 1 && one_chance_in(16))
        big_room(level_number);

    if (random2(level_number) > 6 && one_chance_in(3))
        diamond_rooms(level_number);

    // make some rooms:
    int i, no_rooms, max_doors;
    int sx,sy,ex,ey, time_run;

    temp_rand = random2(750);
    time_run = 0;

    no_rooms = ((temp_rand > 63) ? (5 + random2avg(29, 2)) : // 91.47% {dlb}
                (temp_rand > 14) ? 100                       //  6.53% {dlb}
                                 : 1);                       //  2.00% {dlb}

    max_doors = 2 + random2(8);

    for (i = 0; i < no_rooms; i++)
    {
        sx = 8 + random2(50);
        sy = 8 + random2(40);
        ex = sx + 2 + random2(roomsize);
        ey = sy + 2 + random2(roomsize);

        if (!make_room(sx,sy,ex,ey,max_doors, doorlevel))
        {
            time_run++;
            i--;
        }

        if (time_run > 30)
        {
            time_run = 0;
            i++;
        }
    }

    // make some more rooms:
    no_rooms = 1 + random2(3);
    max_doors = 1;

    for (i = 0; i < no_rooms; i++)
    {
        sx = 8 + random2(55);
        sy = 8 + random2(45);
        ex = sx + 5 + random2(6);
        ey = sy + 5 + random2(6);

        if (!make_room(sx,sy,ex,ey,max_doors, doorlevel))
        {
            time_run++;
            i--;
        }

        if (time_run > 30)
        {
            time_run = 0;
            i++;
        }
    }
    return 0;
}

static void builder_extras(int level_number, int level_type)
{
    if (level_number > 6 && you.where_are_you == BRANCH_MAIN_DUNGEON
                        && level_type == LEVEL_DUNGEON && one_chance_in(3))
    {
        build_minivaults(level_number, 200);
        return;
    }

    if (level_number > 5 && one_chance_in(10))
        many_pools((coinflip()? DNGN_DEEP_WATER : DNGN_LAVA));

    if (level_number >= 11 && level_number <= 23
        && one_chance_in(15))
        place_specific_stair(DNGN_ENTER_LABYRINTH);
}

static void place_traps(int level_number)
{
    int i;
    int num_traps = random2avg(9, 2);

    for (i = 0; i < num_traps; i++)
    {
        // traps can be placed in vaults
        if (env.trap_type[i] != TRAP_UNASSIGNED)
            continue;

        do
        {
            env.trap_x[i] = 10 + random2(GXM - 20);
            env.trap_y[i] = 10 + random2(GYM - 20);
        }
        while (grd[env.trap_x[i]][env.trap_y[i]] != DNGN_FLOOR);

        env.trap_type[i] = TRAP_DART;

        if (random2(1 + level_number) > 1)
            env.trap_type[i] = TRAP_ARROW;
        if (random2(1 + level_number) > 3)
            env.trap_type[i] = TRAP_SPEAR;
        if (random2(1 + level_number) > 5)
            env.trap_type[i] = TRAP_AXE;
        if (random2(1 + level_number) > 7)
            env.trap_type[i] = TRAP_BOLT;
        if (random2(1 + level_number) > 11)
            env.trap_type[i] = TRAP_BLADE;

        if ((random2(1 + level_number) > 14 && one_chance_in(3))
            || (you.where_are_you == BRANCH_HALL_OF_ZOT && coinflip()))
        {
            env.trap_type[i] = TRAP_ZOT;
        }

        if (one_chance_in(20))
            env.trap_type[i] = TRAP_TELEPORT;
        if (one_chance_in(40))
            env.trap_type[i] = TRAP_AMNESIA;

        grd[env.trap_x[i]][env.trap_y[i]] = DNGN_UNDISCOVERED_TRAP;
    }                           // end "for i"
}                               // end place_traps()

static void place_specific_stair(unsigned char stair)
{
    int sx, sy;

    do
    {
        sx = random2(GXM-10);
        sy = random2(GYM-10);
    }
    while(grd[sx][sy] != DNGN_FLOOR || mgrd[sx][sy] != NON_MONSTER);

    grd[sx][sy] = stair;
}


static void place_branch_entrances(int dlevel, char level_type)
{
    unsigned char stair;
    unsigned char entrance;
    int sx, sy;

    if (!level_type == LEVEL_DUNGEON)
        return;

    if (you.where_are_you == BRANCH_MAIN_DUNGEON)
    {
        // stair to HELL
        if (dlevel >= 20 && dlevel <= 27)
            place_specific_stair(DNGN_ENTER_HELL);

        // stair to PANDEMONIUM
        if (dlevel >= 20 && dlevel <= 50
            && (dlevel == 23 || one_chance_in(4)))
            place_specific_stair(DNGN_ENTER_PANDEMONIUM);

        // stairs to ABYSS
        if (dlevel >= 20 && dlevel <= 30
            && (dlevel == 24 || one_chance_in(3)))
            place_specific_stair(DNGN_ENTER_ABYSS);

        // level 26: replaces all down stairs with staircases to Zot:
        if (dlevel == 26)
        {
            for (sx = 1; sx < GXM; sx++)
                for (sy = 1; sy < GYM; sy++)
                    if (grd[sx][sy] >= DNGN_STONE_STAIRS_DOWN_I
                            && grd[sx][sy] <= DNGN_ROCK_STAIRS_DOWN)
                    {
                        grd[sx][sy] = DNGN_ENTER_ZOT;
                    }
        }
    }

    // place actual branch entrances
    for (int branch = 0; branch < 30; branch++)
    {
        stair = 0;
        entrance = 100;

        if (you.branch_stairs[branch] == 100)   // set in newgame
            break;

        if (you.branch_stairs[branch] != dlevel)
            continue;

        // decide if this branch leaves from this level
        switch(branch)
        {
            case STAIRS_ORCISH_MINES:
            case STAIRS_HIVE:
            case STAIRS_LAIR:
            case STAIRS_VAULTS:
            case STAIRS_ECUMENICAL_TEMPLE:
                entrance = BRANCH_MAIN_DUNGEON;
                break;

            case STAIRS_SLIME_PITS:
            case STAIRS_SWAMP:
            case STAIRS_SNAKE_PIT:
                entrance = BRANCH_LAIR;
                break;

            case STAIRS_ELVEN_HALLS:
                entrance = BRANCH_ORCISH_MINES;
                break;

            case STAIRS_CRYPT:
            case STAIRS_HALL_OF_BLADES:
                entrance = BRANCH_VAULTS;
                break;

            case STAIRS_TOMB:
                entrance = BRANCH_CRYPT;
                break;

            default:
                entrance = 100;
                break;
        }

        if (you.where_are_you != entrance)
            continue;

        stair = branch + DNGN_ENTER_ORCISH_MINES;
        place_specific_stair(stair);
    }   // end loop - possible branch entrances
}

static void make_trail(int xs, int xr, int ys, int yr,int corrlength, int intersect_chance,
    int no_corr, unsigned char begin, unsigned char end)
{
    int x_start, y_start;                   // begin point
    int x_ps, y_ps;                         // end point
    int finish = 0;
    int dir_x2, dir_y2,dir_x, dir_y;        // temp positions
    int temp_rand;
    int length = 0;

    do
    {
        x_start = xs + random2(xr);
        y_start = ys + random2(yr);
    }
    while (grd[x_start][y_start] != DNGN_ROCK_WALL
                && grd[x_start][y_start] != DNGN_FLOOR);

    // assign begin feature
    if (begin != 0)
        grd[x_start][y_start] = begin;
    x_ps = x_start;
    y_ps = y_start;

    // wander
    do                          // (while finish < no_corr)
    {
        dir_x2 = ((x_ps < 15) ? 1 : 0);

        if (x_ps > 65)
            dir_x2 = -1;

        dir_y2 = ((y_ps < 15) ? 1 : 0);

        if (y_ps > 55)
            dir_y2 = -1;

        temp_rand = random2(10);

        // Put something in to make it go to parts of map it isn't in now
        if (coinflip())
        {
            if (dir_x2 != 0 && temp_rand < 6)
                dir_x = dir_x2;

            if (dir_x2 == 0 || temp_rand >= 6)
                dir_x = (coinflip()? -1 : 1);

            dir_y = 0;
        }
        else
        {
            if (dir_y2 != 0 && temp_rand < 6)
                dir_y = dir_y2;

            if (dir_y2 == 0 || temp_rand >= 6)
                dir_y = (coinflip()? -1 : 1);

            dir_x = 0;
        }

        if (dir_x == 0 && dir_y == 0)
            continue;

        if (x_ps < 8)
        {
            dir_x = 1;
            dir_y = 0;
        }

        if (y_ps < 8)
        {
            dir_y = 1;
            dir_x = 0;
        }

        if (x_ps > (GXM - 8))
        {
            dir_x = -1;
            dir_y = 0;
        }

        if (y_ps > (GYM - 8))
        {
            dir_y = -1;
            dir_x = 0;
        }

        // corridor length.. change only when going vertical?
        if (dir_x == 0 || length == 0)
            length = random2(corrlength) + 2;

        int bi = 0;

        for (bi = 0; bi < length; bi++)
        {
            // Below, I've changed the values of the unimportant variable from
            // 0 to random2(3) - 1 to avoid getting stuck on the "stuck!" bit
            if (x_ps < 9)
            {
                dir_y = 0;      //random2(3) - 1;
                dir_x = 1;
            }

            if (x_ps > (GXM - 9))
            {
                dir_y = 0;      //random2(3) - 1;
                dir_x = -1;
            }

            if (y_ps < 9)
            {
                dir_y = 1;
                dir_x = 0;      //random2(3) - 1;
            }

            if (y_ps > (GYM - 9))
            {
                dir_y = -1;
                dir_x = 0;      //random2(3) - 1;
            }

            // don't interfere with special rooms
            if (grd[x_ps + dir_x][y_ps + dir_y] == DNGN_BUILDER_SPECIAL_WALL)
                break;

            // see if we stop due to intersection with another corridor/room
            if (grd[x_ps + 2 * dir_x][y_ps + 2 * dir_y] == DNGN_FLOOR
                && !one_chance_in(intersect_chance))
                break;

            x_ps += dir_x;
            y_ps += dir_y;

            if (grd[x_ps][y_ps] == DNGN_ROCK_WALL)
                grd[x_ps][y_ps] = DNGN_FLOOR;
        }

        if (finish == no_corr - 1 && grd[x_ps][y_ps] != DNGN_FLOOR)
            finish -= 2;

        finish++;
    }
    while (finish < no_corr);

    // assign end feature
    if (end != 0)
        grd[x_ps][y_ps] = end;
}

static int good_door_spot(int x, int y)
{
    if ((grd[x][y] > DNGN_WATER_X && grd[x][y] < DNGN_ENTER_PANDEMONIUM)
        || grd[x][y] == DNGN_CLOSED_DOOR)
        return 1;
    return 0;
}

// return TRUE if a room was made successfully
static bool make_room(int sx,int sy,int ex,int ey,int max_doors, int doorlevel)
{
    int find_door = 0;
    int diag_door = 0;
    int rx,ry;

    // check top & bottom for possible doors
    for (rx = sx; rx <= ex; rx++)
    {
        find_door += good_door_spot(rx,sy);
        find_door += good_door_spot(rx,ey);
    }

    // check left and right for possible doors
    for (ry = sy+1; ry < ey; ry++)
    {
        find_door += good_door_spot(sx,ry);
        find_door += good_door_spot(ex,ey);
    }

    diag_door += good_door_spot(sx,sy);
    diag_door += good_door_spot(ex,sy);
    diag_door += good_door_spot(sx,ey);
    diag_door += good_door_spot(ex,ey);

    if ((diag_door + find_door) > 1 && max_doors == 1)
        return false;

    if (find_door == 0 || find_door > max_doors)
        return false;

    // look for 'special' rock walls - don't interrupt them
    if (find_in_area(sx,sy,ex,ey,DNGN_BUILDER_SPECIAL_WALL))
        return false;

    // convert the area to floor
    for (rx=sx; rx<=ex; rx++)
        for(ry=sy; ry<=ey; ry++)
            if (grd[rx][ry] <= DNGN_FLOOR)
                grd[rx][ry] = DNGN_FLOOR;

    // put some doors on the sides (but not in corners),
    // where it makes sense to do so.
    for(ry=sy+1; ry<ey; ry++)
    {
        // left side
        if (grd[sx-1][ry] == DNGN_FLOOR
            && grd[sx-1][ry-1] < DNGN_LAST_SOLID_TILE
            && grd[sx-1][ry+1] < DNGN_LAST_SOLID_TILE)
        {
            if (random2(10) < doorlevel)
                grd[sx-1][ry] = DNGN_CLOSED_DOOR;
        }

        // right side
        if (grd[ex+1][ry] == DNGN_FLOOR
            && grd[ex+1][ry-1] < DNGN_LAST_SOLID_TILE
            && grd[ex+1][ry+1] < DNGN_LAST_SOLID_TILE)
        {
            if (random2(10) < doorlevel)
                grd[ex+1][ry] = DNGN_CLOSED_DOOR;
        }
    }

    // put some doors on the top & bottom
    for(rx=sx+1; rx<ex; rx++)
    {
        // top
        if (grd[rx][sy-1] == DNGN_FLOOR
            && grd[rx-1][sy-1] < DNGN_LAST_SOLID_TILE
            && grd[rx+1][sy-1] < DNGN_LAST_SOLID_TILE)
        {
            if (random2(10) < doorlevel)
                grd[rx][sy-1] = DNGN_CLOSED_DOOR;
        }

        // bottom
        if (grd[rx][ey+1] == DNGN_FLOOR
            && grd[rx-1][ey+1] < DNGN_LAST_SOLID_TILE
            && grd[rx+1][ey+1] < DNGN_LAST_SOLID_TILE)
        {
            if (random2(10) < doorlevel)
                grd[rx][ey+1] = DNGN_CLOSED_DOOR;
        }
    }

    return true;
}                               //end make_room()

static void builder_monsters(int level_number, char level_type, int mon_wanted)
{
    int i = 0;
    int totalplaced = 0;
    int not_used=0;
    int x,y;
    int lava_spaces, water_spaces;
    int aq_creatures;
    int swimming_things[4];

    if (level_type == LEVEL_PANDEMONIUM)
        return;

    for (i = 0; i < mon_wanted; i++)
    {
        if (place_monster(not_used, RANDOM_MONSTER, level_number, BEH_SLEEP,
            MHITNOT, false, 1,1, true))
            totalplaced++;
    }

    // Unique beasties:
    int which_unique;

    if (level_number > 0
        && !(you.where_are_you == BRANCH_DIS
             || you.where_are_you == BRANCH_GEHENNA
             || you.where_are_you == BRANCH_COCYTUS
             || you.where_are_you == BRANCH_TARTARUS
             || you.where_are_you == BRANCH_ORCISH_MINES
             || you.where_are_you == BRANCH_HIVE
             || you.where_are_you == BRANCH_LAIR
             || you.where_are_you == BRANCH_ECUMENICAL_TEMPLE))
    {
        while(one_chance_in(3))
        {
            which_unique = -1;   //     30 in total

            while(which_unique < 0 || you.unique_creatures[which_unique])
            {
                // sometimes,  we just quit if a unique is already placed.
                if (which_unique >= 0 && !one_chance_in(3))
                {
                    which_unique = -1;
                    break;
                }

                which_unique = ((level_number > 19) ? 20 + random2(11) :
                    (level_number > 16) ? 13 + random2(10) :
                    (level_number > 13) ?  9 + random2( 9) :
                    (level_number >  9) ?  6 + random2( 5) :
                    (level_number >  7) ?  4 + random2( 4) :
                    (level_number >  3) ?  2 + random2( 4)
                                     : random2(4));
            }

            // usually, we'll have quit after a few tries. Make sure we don't
            // create unique[-1] by accident.
            if (which_unique < 0)
                break;

            // note: unique_creatures 40 + used by unique demons
            if (place_monster(not_used, 280 + which_unique, level_number, BEH_SLEEP,
                MHITNOT, false, 1,1, true))
                totalplaced++;
        }
    }

    // do aquatic and lava monsters:

    // count the number of lava and water tiles {dlb}:
    for (x = 0; x < GXM; x++)
    {
        for (y = 0; y < GYM; y++)
        {
            if (grd[x][y] == DNGN_LAVA)
                lava_spaces++;
            else if (grd[x][y] == DNGN_DEEP_WATER
                     || grd[x][y] == DNGN_SHALLOW_WATER)
                water_spaces++;
        }
    }

    if (lava_spaces > 49)
    {
        for (i = 0; i < 4; i++)
            swimming_things[i] = MONS_LAVA_WORM + random2(3);

        aq_creatures = random2avg(9, 2) + (random2(lava_spaces) / 10);

        if (aq_creatures > 25)
            aq_creatures = 25;

        for (i = 0; i < aq_creatures; i++)
        {
            if (place_monster(not_used, swimming_things[random2(4)],
                level_number, BEH_SLEEP, MHITNOT, false, 1,1, true))
                totalplaced++;

            if (totalplaced > 99)
                break;
        }
    }

    if (water_spaces > 49)
    {
        for (i = 0; i < 4; i++)
        {
            // mixing enums and math ticks me off !!! 15jan2000 {dlb}
            swimming_things[i] = MONS_BIG_FISH + random2(4);

            // swamp worms and h2o elementals generated below: {dlb}
            if (you.where_are_you == BRANCH_SWAMP && !one_chance_in(3))
                swimming_things[i] = MONS_SWAMP_WORM;
        }

        if (level_number >= 25 && one_chance_in(5))
            swimming_things[0] = MONS_WATER_ELEMENTAL;

        if (you.where_are_you == BRANCH_COCYTUS)
            swimming_things[3] = MONS_WATER_ELEMENTAL;

        aq_creatures = random2avg(9, 2) + (random2(water_spaces) / 7);

        if (aq_creatures > 25)
            aq_creatures = 25;

        for (i = 0; i < aq_creatures; i++)
        {
            if (place_monster(not_used, swimming_things[random2(4)],
                level_number, BEH_SLEEP, MHITNOT, false, 1,1, true))
                totalplaced++;

            if (totalplaced > 99)
                break;
        }
    }
}

static void builder_items(int level_number, char level_type, int items_wanted)
{
    int i = 0;
    unsigned char specif_type = OBJ_RANDOM;
    int items_levels = level_number;

    if (you.where_are_you == BRANCH_VAULTS)
    {
        items_levels *= 15;
        items_levels /= 10;
    }
    else if (you.where_are_you == BRANCH_ORCISH_MINES)
    {
        specif_type = OBJ_GOLD; /* lots of gold in the orcish mines */
    }

    if (you.where_are_you == BRANCH_DIS
        || you.where_are_you == BRANCH_GEHENNA
        || you.where_are_you == BRANCH_COCYTUS
        || you.where_are_you == BRANCH_TARTARUS
        || you.where_are_you == BRANCH_SLIME_PITS
        || you.where_are_you == BRANCH_HALL_OF_BLADES
        || you.where_are_you == BRANCH_ECUMENICAL_TEMPLE)
    {
        /* No items in hell, the slime pits, the Hall */
        return;
    }
    else
    {
        for (i = 0; i < items_wanted; i++)
            items(1, specif_type, OBJ_RANDOM, 0, items_levels, 250);

        // Make sure there's a very good chance of a knife being placed
        // in the first five levels, but not a guarantee of one. -- bwr
        if (you.where_are_you == BRANCH_MAIN_DUNGEON
            && level_type == LEVEL_DUNGEON
            && level_number < 5 && one_chance_in(4))
        {
            int item_no = items(0, OBJ_WEAPONS, WPN_KNIFE, 0, 0, 250);

            // Guarantee that the knife is uncursed and non-special
            if (item_no != 501)
            {
                mitm.pluses[item_no] = 50;
                mitm.pluses2[item_no] = 50;
                mitm.special[item_no] = 0;
            }
        }
    }
}

// the entire intent of this function is to find a
// hallway from a special room to a floor space somewhere,
// changing the special room wall (DNGN_BUILDER_SPECIAL_WALL)
// to a closed door,  and normal rock wall to pre-floor.
// Anything that might otherwise block the hallway is changed
// to pre-floor.
static void specr_2(spec_room &sr)
{
    int bkout = 0;
    int cx = 0, cy = 0;
    int sx = 0, sy = 0;
    int dx = 0, dy = 0;
    int i,j;

    // paranoia -- how did we get here if there's no actual special room??
    if (!sr.created)
        return;

  grolko:

    if (bkout > 100)
        return;

    switch (random2(4))
    {
    case 0:
        // go up from north edge
        cx = sr.x1 + (random2(sr.x2 - sr.x1));
        cy = sr.y1;
        dx = 0;
        dy = -1;
        break;
    case 1:
        // go down from south edge
        cx = sr.x1 + (random2(sr.x2 - sr.x1));
        cy = sr.y2;
        dx = 0;
        dy = 1;
        break;
    case 2:
        // go left from west edge
        cy = sr.y1 + (random2(sr.y2 - sr.y1));
        cx = sr.x1;
        dx = -1;
        dy = 0;
        break;
    case 3:
        // go right from east edge
        cy = sr.y1 + (random2(sr.y2 - sr.y1));
        cx = sr.x2;
        dx = 1;
        dy = 0;
        break;
    }

    sx = cx;
    sy = cy;

    for (i = 0; i < 100; i++)
    {
        sx += dx;
        sy += dy;

        // quit if we run off the map before finding floor
        if (sx < 6 || sx > (GXM - 7) || sy < 6 || sy > (GYM - 7))
        {
            bkout++;
            goto grolko;
        }

        // look around for floor
        if (i > 0)
        {
            if (grd[sx + 1][sy] == DNGN_FLOOR)
                break;
            if (grd[sx][sy + 1] == DNGN_FLOOR)
                break;
            if (grd[sx - 1][sy] == DNGN_FLOOR)
                break;
            if (grd[sx][sy - 1] == DNGN_FLOOR)
                break;
        }
    }

    sx = cx;
    sy = cy;

    for (j = 0; j < i + 2; j++)
    {
        if (grd[sx][sy] == DNGN_BUILDER_SPECIAL_WALL)
            grd[sx][sy] = DNGN_CLOSED_DOOR;

        if (j > 0 && grd[sx + dx][sy + dy] > DNGN_ROCK_WALL
            && grd[sx + dx][sy + dy] < DNGN_FLOOR)
            grd[sx][sy] = DNGN_BUILDER_SPECIAL_FLOOR;

        if (grd[sx][sy] == DNGN_ROCK_WALL)
            grd[sx][sy] = DNGN_BUILDER_SPECIAL_FLOOR;

        sx += dx;
        sy += dy;
    }

    sr.hooked_up = true;
}                               // end specr_2()

static void special_room(int level_number, spec_room &sr)
{
    char spec_room_type = SROOM_LAIR_KOBOLD;
    int lev_mons;
    int thing_created = 0;
    int x,y;

    unsigned char obj_type = OBJ_RANDOM;  // used in calling items() {dlb}
    unsigned char i;        // general purpose loop variable {dlb}
    int temp_rand = 0;          // probability determination {dlb}

    FixedVector < int, 10 > mons_alloc; // was [20] {dlb}

    char lordx = 0, lordy = 0;

    // overwrites anything;  this function better be called early on during
    // creation..
    int room_x1 = 8 + random2(55);
    int room_y1 = 8 + random2(45);
    int room_x2 = room_x1 + 6 + random2(4);
    int room_y2 = room_y1 + 6 + random2(4);

    // do special walls & floor
    make_box(room_x1, room_y1, room_x2, room_y2, DNGN_BUILDER_SPECIAL_FLOOR,
        DNGN_BUILDER_SPECIAL_WALL);

    // set up passed in spec_room structure
    sr.created = true;
    sr.hooked_up = false;
    sr.x1 = room_x1 + 1;
    sr.x2 = room_x2 - 1;
    sr.y1 = room_y1 + 1;
    sr.y2 = room_y2 - 1;

    if (level_number < 7)
    {
        spec_room_type = SROOM_LAIR_KOBOLD;
    }
    else
    {
        spec_room_type = random2(NUM_SPECIAL_ROOMS);

        if (level_number < 23 && one_chance_in(4))
            spec_room_type = SROOM_BEEHIVE;

        if (level_number < 17 && one_chance_in(4))
            spec_room_type = SROOM_LAIR_ORC;

        if (level_number > 17 && spec_room_type == SROOM_LAIR_KOBOLD)
            spec_room_type = SROOM_LAIR_ORC;
    }


    switch (spec_room_type)
    {
    case SROOM_LAIR_ORC:
        // determine which monster array to generate {dlb}:
        lev_mons = ((level_number > 24) ? 3 :
                    (level_number > 15) ? 2 :
                    (level_number >  9) ? 1
                                     : 0);

        // fill with baseline monster type {dlb}:
        for (i = 0; i < 10; i++)
        {
            mons_alloc[i] = MONS_ORC;
        }

        // fill in with special monster types {dlb}:
        switch (lev_mons)
        {
        case 0:
            mons_alloc[9] = MONS_ORC_WARRIOR;
            break;
        case 1:
            mons_alloc[8] = MONS_ORC_WARRIOR;
            mons_alloc[9] = MONS_ORC_WARRIOR;
            break;
        case 2:
            mons_alloc[6] = MONS_ORC_KNIGHT;
            mons_alloc[7] = MONS_ORC_WARRIOR;
            mons_alloc[8] = MONS_ORC_WARRIOR;
            mons_alloc[9] = MONS_OGRE;
            break;
        case 3:
            mons_alloc[2] = MONS_ORC_WARRIOR;
            mons_alloc[3] = MONS_ORC_WARRIOR;
            mons_alloc[4] = MONS_ORC_WARRIOR;
            mons_alloc[5] = MONS_ORC_KNIGHT;
            mons_alloc[6] = MONS_ORC_KNIGHT;
            mons_alloc[7] = MONS_OGRE;
            mons_alloc[8] = MONS_OGRE;
            mons_alloc[9] = MONS_TROLL;
            break;
        }

        // place monsters and give them items {dlb}:
        for (x = sr.x1; x <= sr.x2; x++)
        {
            for (y = sr.y1; y <= sr.y2; y++)
            {
                if (one_chance_in(4))
                    continue;

                mons_place(mons_alloc[random2(10)], BEH_SLEEP, MHITNOT,
                    true, x, y);
            }
        }
        break;

    case SROOM_LAIR_KOBOLD:
        lordx = sr.x1 + random2(sr.x2 - sr.x1);
        lordy = sr.y1 + random2(sr.y2 - sr.y1);

        // determine which monster array to generate {dlb}:
        lev_mons = ((level_number < 4) ? 0 :
                    (level_number < 6) ? 1 : (level_number < 9) ? 2 : 3);

        // fill with baseline monster type {dlb}:
        for (i = 0; i < 10; i++)
        {
            mons_alloc[i] = MONS_KOBOLD;
        }

        // fill in with special monster types {dlb}:
        // in this case, they are uniformly the same {dlb}:
        for (i = (7 - lev_mons); i < 10; i++)
        {
            mons_alloc[i] = MONS_BIG_KOBOLD;
        }

        // place monsters and give them items {dlb}:
        for (x = sr.x1; x <= sr.x2; x++)
        {
            for (y = sr.y1; y <= sr.y2; y++)
            {
                if (one_chance_in(4))
                    continue;

                // we'll put the boss down later.
                if (x == lordx && y == lordy)
                    continue;

                mons_place(mons_alloc[random2(10)], BEH_SLEEP, MHITNOT,
                    true, x, y);
            }
        }

        // put the boss monster down
        mons_place(MONS_BIG_KOBOLD, BEH_SLEEP, MHITNOT,
            true, x, y);

        break;

        // should only appear in deep levels, with a guardian
        // Maybe have several types of treasure room?
    case SROOM_TREASURY:
        // place treasure {dlb}:
        for (x = sr.x1; x <= sr.x2; x++)
        {
            for (y = sr.y1; y <= sr.y2; y++)
            {
                temp_rand = random2(11);

                obj_type = ((temp_rand > 8) ? OBJ_WEAPONS :       // 2 in 11
                          (temp_rand > 6) ? OBJ_ARMOUR :        // 2 in 11
                          (temp_rand > 5) ? OBJ_MISSILES :      // 1 in 11
                          (temp_rand > 4) ? OBJ_WANDS :         // 1 in 11
                          (temp_rand > 3) ? OBJ_SCROLLS :       // 1 in 11
                          (temp_rand > 2) ? OBJ_JEWELLERY :     // 1 in 11
                          (temp_rand > 1) ? OBJ_BOOKS :         // 1 in 11
                          (temp_rand > 0) ? OBJ_STAVES          // 1 in 11
                                          : OBJ_POTIONS);       // 1 in 11

                thing_created = items( 1, obj_type, OBJ_RANDOM, 1,
                                                    level_number * 3, 250 );

                mitm.x[thing_created] = x;
                mitm.y[thing_created] = y;
            }
        }

        // place guardian {dlb}:
        mons_place(MONS_GUARDIAN_NAGA, BEH_SLEEP, MHITNOT,
            true, sr.x1 + random2(sr.x2 - sr.x1),
            sr.y1 + random2(sr.y2 - sr.y1));

        break;

    case SROOM_BEEHIVE:
        beehive(sr);
        break;
    }
}                               // end special_room()

// fills a special room with bees
static void beehive(spec_room &sr)
{
    int temp_rand;          // probability determination {dlb}
    int i;
    int x,y;

    for (x = sr.x1; x <= sr.x2; x++)
    {
        for (y = sr.y1; y <= sr.y2; y++)
        {
            if (coinflip())
                continue;

            for (i = 0; i < MAX_ITEMS-50; i++)
            {
                if (mitm.base_type[i] == OBJ_UNASSIGNED)
                    break;
            }

            if (mitm.base_type[i] != OBJ_UNASSIGNED)
                goto finished_food;

            mitm.quantity[i] = 1;
            mitm.base_type[i] = OBJ_FOOD;
            mitm.sub_type[i] = (one_chance_in(25) ? FOOD_ROYAL_JELLY
                                                   : FOOD_HONEYCOMB);
            mitm.x[i] = x;
            mitm.y[i] = y;

            item_colour(i);
        }
    }


  finished_food:

    int queenx = sr.x1 + random2(sr.x2 - sr.x1);
    int queeny = sr.y1 + random2(sr.y2 - sr.y1);

    for (x = sr.x1; x <= sr.x2; x++)
    {
        for (y = sr.y1; y <= sr.y2; y++)
        {
            if (x == queenx && y == queeny)
                continue;

            // the hive is chock full of bees!

            mons_place(one_chance_in(7)?MONS_KILLER_BEE_LARVA:MONS_KILLER_BEE,
                BEH_SLEEP,  MHITNOT,  true, x, y);
        }
    }

    mons_place(MONS_QUEEN_BEE, BEH_SLEEP, MHITNOT, true, queenx, queeny);
}                               // end beehive()

static void build_minivaults(int level_number, int force_vault)
{
    // for some weird reason can't put a vault on level 1, because monster equip
    // isn't generated.
    int altar_count = 0;

    FixedVector < char, 7 > acq_item_class;
    // hack - passing chars through '...' promotes them to ints, which
    // barfs under gcc in fixvec.h.  So don't.
    acq_item_class[0] = OBJ_WEAPONS;
    acq_item_class[1] = OBJ_ARMOUR;
    acq_item_class[2] = OBJ_WEAPONS;
    acq_item_class[3] = OBJ_JEWELLERY;
    acq_item_class[4] = OBJ_BOOKS;
    acq_item_class[5] = OBJ_STAVES;
    acq_item_class[6] = OBJ_MISCELLANY;

    FixedVector < int, 7 > mons_array(RANDOM_MONSTER,
                                      RANDOM_MONSTER,
                                      RANDOM_MONSTER,
                                      RANDOM_MONSTER,
                                      RANDOM_MONSTER,
                                      RANDOM_MONSTER,
                                      RANDOM_MONSTER);

    char vgrid[81][81];

    if (force_vault == 200)
    {
#ifdef USE_NEW_MINIVAULTS
        force_vault = 200 + random2(37);
#else
        force_vault = 200 + random2(33);
#endif
    }

    vault_main(vgrid, mons_array, force_vault, level_number);

    int vx, vy;
    int v1x, v1y;

    /* find a target area which can be safely overwritten: */
    while(1)
    {
        //if ( one_chance_in(1000) ) return;
        v1x = 12 + random2(45);
        v1y = 12 + random2(35);

        for (vx = v1x; vx < v1x + 12; vx++)
        {
            for (vy = v1y; vy < v1y + 12; vy++)
            {
                if (one_chance_in(2000))
                    return;

                if ((grd[vx][vy] != DNGN_FLOOR
                        && grd[vx][vy] != DNGN_ROCK_WALL
                        && grd[vx][vy] != DNGN_CLOSED_DOOR
                        && grd[vx][vy] != DNGN_SECRET_DOOR)
                    || igrd[vx][vy] != NON_ITEM
                    || mgrd[vx][vy] != NON_MONSTER)
                {
                    goto out_of_check;
                }
            }
        }

        /* must not be completely isolated: */
        for (vx = v1x; vx < v1x + 12; vx++)
        {
            //  if (vx != v1x && vx != v1x + 12) continue;
            for (vy = v1y; vy < v1y + 12; vy++)
            {
                //   if (vy != v1y && vy != v1y + 12) continue;
                if (grd[vx][vy] == DNGN_FLOOR
                    || grd[vx][vy] == DNGN_CLOSED_DOOR
                    || grd[vx][vy] == DNGN_SECRET_DOOR)
                    goto break_out;
            }
        }

      out_of_check:
        continue;

      break_out:
        break;
    }

    for (vx = v1x; vx < v1x + 12; vx++)
    {
        for (vy = v1y; vy < v1y + 12; vy++)
        {
            grd[vx][vy] = vgrid[vx - v1x][vy - v1y];
        }
    }

    // these two are throwaways:
    int initial_x, initial_y;

    // paint the minivault onto the grid
    for (vx = v1x; vx < v1x + 12; vx++)
    {
        for (vy = v1y; vy < v1y + 12; vy++)
        {
            altar_count = vault_grid(level_number, vx, vy, altar_count, acq_item_class,
                mons_array, grd[vx][vy], initial_x, initial_y);
        }
    }
}                               // end build_minivaults()

static void build_vaults(int level_number, int force_vault)
{
    // for some weird reason can't put a vault on level 1, because monster equip
    // isn't generated.
    int i,j;                // general loop variables
    int altar_count = 0;
    FixedVector < char, 10 > stair_exist;
    char stx, sty;
    int initial_x=0, initial_y=0;

    FixedVector < char, 7 > acq_item_class;
    // hack - passing chars through '...' promotes them to ints, which
    // barfs under gcc in fixvec.h.  So don't. -- GDL
    acq_item_class[0] = OBJ_WEAPONS;
    acq_item_class[1] = OBJ_ARMOUR;
    acq_item_class[2] = OBJ_WEAPONS;
    acq_item_class[3] = OBJ_JEWELLERY;
    acq_item_class[4] = OBJ_BOOKS;
    acq_item_class[5] = OBJ_STAVES;
    acq_item_class[6] = OBJ_MISCELLANY;

    FixedVector < int, 7 > mons_array(RANDOM_MONSTER,
                                      RANDOM_MONSTER,
                                      RANDOM_MONSTER,
                                      RANDOM_MONSTER,
                                      RANDOM_MONSTER,
                                      RANDOM_MONSTER,
                                      RANDOM_MONSTER);

    char roomsss = 10 + random2(90);
    char which_room = 0;

    bool exclusive = (one_chance_in(10) ? false : true);

    //bool exclusive2 = coinflip();    // usage commented out below {dlb}

    char vgrid[81][81];

    char gluggy = vault_main(vgrid, mons_array, force_vault, level_number);

    int vx, vy;
    int v1x = 0, v1y = 0, v2x = 0, v2y = 0;

    //int item_made;

    char dig_dir_x = 0;
    char dig_dir_y = 0;
    char dig_place_x = 0;
    char dig_place_y = 0;

    // note: assumes *no* previous item (I think) or monster (definitely)
    // placement
    for (vx = 0; vx < GXM; vx++)
    {
        for (vy = 0; vy < GYM; vy++)
        {
            altar_count = vault_grid(level_number, vx, vy, altar_count, acq_item_class,
                mons_array, vgrid[vy][vx], initial_x, initial_y);
        }
    }

    switch (gluggy)
    {
    case MAP_NORTH:
        v1x = 1;
        v2x = GXM;
        v1y = 1;
        v2y = 35;
        initial_y++;
        dig_dir_x = 0;
        dig_dir_y = 1;
        break;

    case MAP_NORTHWEST:
        v1x = 1;
        v2x = 40;
        v1y = 1;
        v2y = 35;
        initial_y++;
        dig_dir_x = 1;
        dig_dir_y = 0;
        break;

    case MAP_NORTHEAST:
        v1x = 40;
        v2x = GXM;
        v1y = 1;
        v2y = 35;
        initial_y++;
        dig_dir_x = -1;
        dig_dir_y = 0;
        break;

    case MAP_SOUTHWEST:
        v1x = 1;
        v2x = 40;
        v1y = 35;
        v2y = GYM;
        initial_y--;
        dig_dir_x = 0;
        dig_dir_y = -1;
        break;

    case MAP_SOUTHEAST:
        v1x = 40;
        v2x = GXM;
        v1y = 35;
        v2y = GYM;
        initial_y--;
        dig_dir_x = 0;
        dig_dir_y = -1;
        break;

    case MAP_ENCOMPASS:
        return;

    case MAP_NORTH_DIS:
        v1x = 1;
        v2x = GXM;
        v1y = 1;
        v2y = 35;
        plan_4(1, 1, 80, 35, DNGN_METAL_WALL);
        goto vstair;
    }

    char cnx, cny;
    char romx1[30], romy1[30], romx2[30], romy2[30];

    for (i = 0; i < roomsss; i++)
    {
        do
        {
            romx1[which_room] = 10 + random2(50);
            romy1[which_room] = 10 + random2(40);
            romx2[which_room] = romx1[which_room] + 2 + random2(8);
            romy2[which_room] = romy1[which_room] + 2 + random2(8);
        }
        while ((romx1[which_room] >= v1x && romx1[which_room] <= v2x
                   && romy1[which_room] >= v1y && romy1[which_room] <= v2y)
               || (romx2[which_room] >= v1x && romx2[which_room] <= v2x
                   && romy2[which_room] >= v1y && romy2[which_room] <= v2y));

        if (i == 0)
        {
            join_the_dots(initial_x, initial_y, romx1[which_room], romy1[which_room],
                v1x, v1y, v2x, v2y);
        }
        else if (exclusive)
        {
            for (cnx = romx1[which_room] - 1; cnx < romx2[which_room] + 1;
                                                                        cnx++)
            {
                for (cny = romy1[which_room] - 1; cny < romy2[which_room] + 1;
                                                                        cny++)
                {
                    if (grd[cnx][cny] != DNGN_ROCK_WALL)
                        goto continuing;
                }
            }
        }

        replace_area(romx1[which_room], romy1[which_room], romx2[which_room],
                   romy2[which_room], DNGN_ROCK_WALL, DNGN_FLOOR);

        if (which_room > 0)     // && !exclusive2
        {
            const int rx1 = romx1[which_room];
            const int rx2 = romx2[which_room];
            const int prev_rx1 = romx1[which_room - 1];
            const int prev_rx2 = romx2[which_room - 1];

            const int ry1 = romy1[which_room];
            const int ry2 = romy2[which_room];
            const int prev_ry1 = romy1[which_room - 1];
            const int prev_ry2 = romy2[which_room - 1];

            join_the_dots( rx1 + random2( rx2 - rx1 ),
                           ry1 + random2( ry2 - ry1 ),
                           prev_rx1 + random2( prev_rx2 - prev_rx1 ),
                           prev_ry1 + random2( prev_ry2 - prev_ry1 ),
                           v1x, v1y, v2x, v2y);
        }

        which_room++;

        if (which_room >= 29)
            break;

      continuing:
        continue;               // next i loop

    }

  vstair:
    dig_place_x = initial_x;
    dig_place_y = initial_y;

    if (gluggy != MAP_NORTH_DIS)
    {
        for (i = 0; i < 40; i++)
        {
            dig_place_x += dig_dir_x;
            dig_place_y += dig_dir_y;

            if (dig_place_x < 10 || dig_place_x > (GXM - 10)
                || dig_place_y < 10 || dig_place_y > (GYM - 10))
            {
                break;
            }

            if (grd[dig_place_x][dig_place_y] == DNGN_ROCK_WALL)
                grd[dig_place_x][dig_place_y] = DNGN_FLOOR;
        }
    }

    unsigned char pos_x, pos_y;

    for (stx = 0; stx < 10; stx++)
        stair_exist[stx] = 0;

    for (stx = 0; stx < GXM; stx++)
    {
        for (sty = 0; sty < GYM; sty++)
        {
            if (grd[stx][sty] >= DNGN_STONE_STAIRS_DOWN_I
                    && grd[stx][sty] <= DNGN_ROCK_STAIRS_UP)
            {
                stair_exist[grd[stx][sty] - 82] = 1;
            }
        }
    }

    if (you.where_are_you == BRANCH_DIS)
    {
        for (sty = 0; sty < 5; sty++)
            stair_exist[sty] = 1;

        for (sty = 6; sty < 10; sty++)
            stair_exist[sty] = 0;
    }

    for (j = 0; j < (coinflip()? 4 : 3); j++)
    {
        for (i = 0; i < 2; i++)
        {

            if (stair_exist[(82 + j + (i * 4)) - 82] == 1)   // does this look funny to *you*? {dlb}
                continue;

            do
            {
                pos_x = 10 + random2(GXM - 20);
                pos_y = 10 + random2(GYM - 20);
            }
            while (grd[pos_x][pos_y] != DNGN_FLOOR
                   || (pos_x >= v1x && pos_x <= v2x && pos_y >= v1y
                       && pos_y <= v2y));

            grd[pos_x][pos_y] = j + ((i == 0) ? DNGN_STONE_STAIRS_DOWN_I
                                      : DNGN_STONE_STAIRS_UP_I);
        }
    }
}                               // end build_vaults()

// returns altar_count - seems rather odd to me to force such a return
// when I believe the value is only used in the case of the ecumenical
// temple - oh, well... {dlb}
static int vault_grid(int level_number, int vx, int vy, int altar_count,
    FixedVector < char, 7 > &acq_item_class, FixedVector < int, 7 > &mons_array,
    char vgrid, int &initial_x, int &initial_y)
{
    int not_used;

    // first, set base tile for grids {dlb}:
    grd[vx][vy] = ((vgrid == 'x') ? DNGN_ROCK_WALL :
                   (vgrid == 'c') ? DNGN_STONE_WALL :
                   (vgrid == 'v') ? DNGN_METAL_WALL :
                   (vgrid == 'b') ? DNGN_GREEN_CRYSTAL_WALL :
                   (vgrid == 'a') ? DNGN_WAX_WALL :
                   (vgrid == '+') ? DNGN_CLOSED_DOOR :
                   (vgrid == '=') ? DNGN_SECRET_DOOR :
                   (vgrid == 'w') ? DNGN_DEEP_WATER :
                   (vgrid == 'l') ? DNGN_LAVA :
                   (vgrid == '>') ? DNGN_ROCK_STAIRS_DOWN :
                   (vgrid == '<') ? DNGN_ROCK_STAIRS_UP :
                   (vgrid == '}') ? DNGN_STONE_STAIRS_DOWN_I :
                   (vgrid == '{') ? DNGN_STONE_STAIRS_UP_I :
                   (vgrid == ')') ? DNGN_STONE_STAIRS_DOWN_II :
                   (vgrid == '(') ? DNGN_STONE_STAIRS_UP_II :
                   (vgrid == ']') ? DNGN_STONE_STAIRS_DOWN_III :
                   (vgrid == '[') ? DNGN_STONE_STAIRS_UP_III :
                   (vgrid == 'A') ? DNGN_STONE_ARCH :
                   (vgrid == 'B') ? (DNGN_ALTAR_ZIN + altar_count) :// see below
                   (vgrid == 'C') ? pick_an_altar() :   // f(x) elsewhere {dlb}
                   (vgrid == 'I') ? DNGN_ORCISH_IDOL :
                   (vgrid == 'S') ? DNGN_SILVER_STATUE :
                   (vgrid == 'G') ? DNGN_GRANITE_STATUE :
                   (vgrid == 'H') ? DNGN_ORANGE_CRYSTAL_STATUE :
                   (vgrid == 'T') ? DNGN_BLUE_FOUNTAIN :
                   (vgrid == 'U') ? DNGN_SPARKLING_FOUNTAIN :
                   (vgrid == 'V') ? DNGN_PERMADRY_FOUNTAIN :
                   (vgrid == '\0')? DNGN_ROCK_WALL :
                                    DNGN_FLOOR); // includes everything else

    // then, handle oddball grids {dlb}:
    switch (vgrid)
    {
    case 'B':
        altar_count++;
        break;
    case '@':
        initial_x = vx;
        initial_y = vy;
        break;
    case '^':
        place_specific_trap(vx, vy, TRAP_RANDOM);
        break;
    }

    // then, handle grids that place "stuff" {dlb}:
    switch (vgrid)              // yes, I know this is a bit ugly ... {dlb}
    {
    case 'R':
    case '$':
    case '%':
    case '*':
    case '|':
    case 'P':                   // possible rune
    case 'O':                   // definite rune
    case 'Z':                   // definite orb
        {
            int item_made = NON_ITEM;
            unsigned char which_class = OBJ_RANDOM;
            unsigned char which_type = OBJ_RANDOM;
            int which_depth;
            bool possible_rune = one_chance_in(3);      // lame, I know {dlb}

            if (vgrid == 'R')
            {
                which_class = OBJ_FOOD;
                which_type = (one_chance_in(3) ? FOOD_ROYAL_JELLY
                                               : FOOD_HONEYCOMB);
            }
            else if (vgrid == '$')
            {
                which_class = OBJ_GOLD;
                which_type = OBJ_RANDOM;
            }
            else if (vgrid == '%' || vgrid == '*')
            {
                which_class = OBJ_RANDOM;
                which_type = OBJ_RANDOM;
            }
            else if (vgrid == 'Z')
            {
                which_class = OBJ_ORBS;
                which_type = ORB_ZOT;
            }
            else if (vgrid == '|' || (vgrid == 'P' && !possible_rune))
            {
                which_class = acq_item_class[random2(7)];
                which_type = OBJ_RANDOM;
            }
            else                // for 'P' (1 out of 3 times) {dlb}
            {
                which_class = OBJ_MISCELLANY;
                which_type = MISC_RUNE_OF_ZOT;
            }

            which_depth = ((vgrid == '|'
                                || vgrid == 'P'
                                || vgrid == 'O'
                                || vgrid == 'Z') ? 351 :
                           (vgrid == '*')        ? 5 + (level_number * 2)
                                                 : level_number);

            item_made = items(1, which_class, which_type, 1, which_depth, 250);

            mitm.x[item_made] = vx;
            mitm.y[item_made] = vy;
        }
        break;
    }

    // finally, handle grids that place monsters {dlb}:
    if (vgrid >= '0' && vgrid <= '9')
    {
        int monster_level;
        int monster_type_thing;

        monster_level = ((vgrid == '8') ? (4 + (level_number * 2)) :
                         (vgrid == '9') ? (5 + level_number) : level_number);

        if (monster_level > 30) // very high level monsters more common here
            monster_level = 30;

        monster_type_thing = ((vgrid == '8'
                               || vgrid == '9'
                               || vgrid == '0') ? RANDOM_MONSTER
                                                : mons_array[(vgrid - '1')]);

        place_monster(not_used, monster_type_thing, monster_level, BEH_SLEEP,
            MHITNOT, true, vx, vy, false);
    }

    // again, this seems odd, given that this is just one of many
    // vault types {dlb}
    return altar_count;
}                               // end vault_grid()

static void replace_area(int sx, int sy, int ex, int ey, unsigned char replace,
    unsigned char feature)
{
    int x,y;
    for(x=sx; x<=ex; x++)
        for(y=sy; y<=ey; y++)
            if (grd[x][y] == replace)
                grd[x][y] = feature;
}

static void join_the_dots(unsigned char dotx1, unsigned char doty1,
                          unsigned char dotx2, unsigned char doty2,
                          char forbid_x1, char forbid_y1, char forbid_x2,
                          char forbid_y2)
{
    if (dotx1 == dotx2 && doty1 == doty2)
        return;

    char atx = dotx1, aty = doty1;

    int join_count = 0;

    grd[atx][aty] = DNGN_FLOOR;

    do
    {
        join_count++;

        if (join_count > 10000) // just insurance
            return;

        if (atx < dotx2
            && (forbid_x1 == 0
                || (atx + 1 < forbid_x1 || atx + 1 > forbid_x2
                    || (aty > forbid_y2 || aty < forbid_y1))))
        {
            atx++;
            goto continuing;
        }

        if (atx > dotx2
            && (forbid_x2 == 0
                || (atx - 1 > forbid_x2 || atx - 1 < forbid_x1
                    || (aty > forbid_y2 || aty < forbid_y1))))
        {
            atx--;
            goto continuing;
        }

        if (aty > doty2
            && (forbid_y2 == 0
                || (aty - 1 > forbid_y2 || aty - 1 < forbid_y1
                    || (atx > forbid_x2 || atx < forbid_x1))))
        {
            aty--;
            goto continuing;
        }

        if (aty < doty2
            && (forbid_y1 == 0
                || (aty + 1 < forbid_y1 || aty + 1 > forbid_y2
                    || (atx > forbid_x2 || atx < forbid_x1))))
        {
            aty++;
            goto continuing;
        }

      continuing:
        grd[atx][aty] = DNGN_FLOOR;

    }
    while (atx != dotx2 || aty != doty2);
}                               // end join_the_dots()

static void place_pool(unsigned char pool_type, unsigned char pool_x1,
                       unsigned char pool_y1, unsigned char pool_x2,
                       unsigned char pool_y2)
{
    int i, j;
    unsigned char left_edge, right_edge;

    // don't place LAVA pools in crypt.. use shallow water instead.
    if ((you.where_are_you == BRANCH_CRYPT || you.where_are_you == BRANCH_TOMB)
        && pool_type == DNGN_LAVA)
        pool_type = DNGN_SHALLOW_WATER;

    if (pool_x1 >= pool_x2 - 4 || pool_y1 >= pool_y2 - 4)
        return;

    left_edge = pool_x1 + 2 + random2(pool_x2 - pool_x1);
    right_edge = pool_x2 - 2 - random2(pool_x2 - pool_x1);

    for (j = pool_y1 + 1; j < pool_y2 - 1; j++)
    {
        for (i = pool_x1 + 1; i < pool_x2 - 1; i++)
        {
            if (i >= left_edge && i <= right_edge && grd[i][j] == DNGN_FLOOR)
                grd[i][j] = pool_type;
        }

        if (j - pool_y1 < (pool_y2 - pool_y1) / 2 || one_chance_in(4))
        {
            if (left_edge > pool_x1 + 1)
                left_edge -= random2(3);

            if (right_edge < pool_x2 - 1)
                right_edge += random2(3);
        }

        if (left_edge < pool_x2 - 1
            && (j - pool_y1 >= (pool_y2 - pool_y1) / 2
                || left_edge <= pool_x1 + 2 || one_chance_in(4)))
        {
            left_edge += random2(3);
        }

        if (right_edge > pool_x1 + 1
            && (j - pool_y1 >= (pool_y2 - pool_y1) / 2
                || right_edge >= pool_x2 - 2 || one_chance_in(4)))
        {
            right_edge -= random2(3);
        }
    }
}                               // end place_pool()

static void many_pools(unsigned char pool_type)
{
    int pools = 0;
    int i = 0, j = 0, k = 0, l = 0;
    int m = 0, n = 0;
    int no_pools = 20 + random2avg(9, 2);
    int timeout = 0;

    do
    {
        timeout++;

        if (timeout >= 30000)
            break;

        i = 10 + random2(50);
        j = 10 + random2(40);
        k = i + 4 + random2(8) + random2(7);
        l = j + 4 + random2(8) + random2(7);

        for (m = i; m < k; m++)
        {
            for (n = j; n < l; n++)
            {
                if (grd[m][n] != DNGN_FLOOR)
                    goto continue_pools;
            }
        }

        place_pool(pool_type, i, j, k, l);
        pools++;

      continue_pools:
        continue;
    }
    while (pools < no_pools);
}                               // end many_pools()

static void item_colour(int p)
{
    int switchnum = 0;

    switch (mitm.base_type[p])
    {
    case OBJ_WEAPONS:
        if (mitm.special[p] % 30 == SPWPN_RANDART_I)
            break;              // unrandarts already coloured

        if (mitm.special[p] >= NWPN_SINGING_SWORD)
        {
            switch (mitm.special[p])   // was: - 180, but that is *wrong* {dlb}
            {
            case NWPN_SINGING_SWORD:
            case NWPN_SCEPTRE_OF_TORMENT:
                mitm.colour[p] = YELLOW;
                break;
            case NWPN_WRATH_OF_TROG:
            case NWPN_SWORD_OF_POWER:
                mitm.colour[p] = RED;
                break;
            case NWPN_SCYTHE_OF_CURSES:
                mitm.colour[p] = DARKGREY;
                break;
            case NWPN_MACE_OF_VARIABILITY:
                mitm.colour[p] = random_colour();
                break;
            case NWPN_GLAIVE_OF_PRUNE:
                mitm.colour[p] = MAGENTA;
                break;
            case NWPN_SWORD_OF_ZONGULDROK:
                mitm.colour[p] = LIGHTGREY;
                break;
            case NWPN_KNIFE_OF_ACCURACY:
                mitm.colour[p] = LIGHTCYAN;
                break;
            case NWPN_STAFF_OF_OLGREB:
                mitm.colour[p] = GREEN;
                break;
            case NWPN_VAMPIRES_TOOTH:
                mitm.colour[p] = WHITE;
                break;
            case NWPN_STAFF_OF_WUCAD_MU:
                mitm.colour[p] = BROWN;
                break;
            }
            break;
        }

        if (is_demonic(mitm.sub_type[p]))
            mitm.colour[p] = random_colour();
        else if (launches_things(mitm.sub_type[p]))
            mitm.colour[p] = BROWN;
        else
        {
            switch (mitm.sub_type[p])
            {
            case WPN_CLUB:
            case WPN_GIANT_CLUB:
            case WPN_GIANT_SPIKED_CLUB:
            case WPN_ANCUS:
            case WPN_WHIP:
            case WPN_QUARTERSTAFF:
                mitm.colour[p] = BROWN;
                break;
            case WPN_QUICK_BLADE:
                mitm.colour[p] = LIGHTBLUE;
                break;
            case WPN_EXECUTIONERS_AXE:
                mitm.colour[p] = RED;
                break;
            default:
                mitm.colour[p] = LIGHTCYAN;
                if (mitm.special[p] / 30 == DWPN_DWARVEN)
                    mitm.colour[p] = CYAN;
                break;
            }
        }

        // I don't think this is ever done -- see start of case {dlb}:
        if (mitm.special[p] % 30 >= SPWPN_RANDART_I && one_chance_in(5))
            mitm.colour[p] = random_colour();
        break;

    case OBJ_MISSILES:
        switch (mitm.sub_type[p])
        {
        case MI_STONE:
        case MI_LARGE_ROCK:
            mitm.colour[p] = BROWN;
            break;
        default:
            mitm.colour[p] = LIGHTCYAN;
            if (mitm.special[p] / 30 == DAMMO_DWARVEN)
                mitm.colour[p] = CYAN;
            break;
        }
        break;

    case OBJ_ARMOUR:
        if (mitm.special[p] % 30 == SPARM_RANDART_I)
            break;              /* unrandarts have already been coloured */

        switch (mitm.sub_type[p])
        {
        case ARM_CLOAK:
        case ARM_ROBE:
            mitm.colour[p] = random_colour();
            break;

        case ARM_HELMET:
            //caps and wizard's hats are random coloured
            if (mitm.pluses2[p] == 2 || mitm.pluses2[p] == 3)
                mitm.colour[p] = random_colour();
            else
                mitm.colour[p] = LIGHTCYAN;
            break;

        case ARM_BOOTS: // maybe more interesting boot colours?
        case ARM_GLOVES:
        case ARM_LEATHER_ARMOUR:
            mitm.colour[p] = BROWN;
            break;
        case ARM_DRAGON_HIDE:
        case ARM_DRAGON_ARMOUR:
            mitm.colour[p] = mons_colour(MONS_DRAGON);
            break;
        case ARM_TROLL_HIDE:
        case ARM_TROLL_LEATHER_ARMOUR:
            mitm.colour[p] = mons_colour(MONS_TROLL);
            break;
        case ARM_CRYSTAL_PLATE_MAIL:
            mitm.colour[p] = LIGHTGREY;
            break;
        case ARM_ICE_DRAGON_HIDE:
        case ARM_ICE_DRAGON_ARMOUR:
            mitm.colour[p] = mons_colour(MONS_ICE_DRAGON);
            break;
        case ARM_STEAM_DRAGON_HIDE:
        case ARM_STEAM_DRAGON_ARMOUR:
            mitm.colour[p] = mons_colour(MONS_STEAM_DRAGON);
            break;
        case ARM_MOTTLED_DRAGON_HIDE:
        case ARM_MOTTLED_DRAGON_ARMOUR:
            mitm.colour[p] = mons_colour(MONS_MOTTLED_DRAGON);
            break;
        case ARM_STORM_DRAGON_HIDE:
        case ARM_STORM_DRAGON_ARMOUR:
            mitm.colour[p] = mons_colour(MONS_STORM_DRAGON);
            break;
        case ARM_GOLD_DRAGON_HIDE:
        case ARM_GOLD_DRAGON_ARMOUR:
            mitm.colour[p] = mons_colour(MONS_GOLDEN_DRAGON);
            break;
        case ARM_ANIMAL_SKIN:
            mitm.colour[p] = BROWN;
            break;
        case ARM_SWAMP_DRAGON_HIDE:
        case ARM_SWAMP_DRAGON_ARMOUR:
            mitm.colour[p] = mons_colour(MONS_SWAMP_DRAGON);
            break;
        default:
            mitm.colour[p] = LIGHTCYAN;
            if (mitm.special[p] / 30 == DARM_DWARVEN)
                mitm.colour[p] = CYAN;
            break;
        }

        // I don't think this is ever done -- see start of case {dlb}:
        if (mitm.special[p] % 30 >= SPARM_RANDART_I && one_chance_in(5))
            mitm.colour[p] = random_colour();
        break;

    case OBJ_WANDS:
        mitm.special[p] = you.item_description[IDESC_WANDS][mitm.sub_type[p]];

        switch (mitm.special[p] % 12)
        {
        case 0:         //"iron wand"
            mitm.colour[p] = CYAN;
            break;
        case 1:         //"brass wand"
        case 5:         //"gold wand"
            mitm.colour[p] = YELLOW;
            break;
        case 2:         //"bone wand"
        case 8:         //"ivory wand"
        case 9:         //"glass wand"
        case 10:                //"lead wand"
            mitm.colour[p] = LIGHTGREY;
            break;
        case 3:         //"wooden wand"
        case 4:         //"copper wand"
        case 7:         //"bronze wand"
            mitm.colour[p] = BROWN;
            break;
        case 6:         //"silver wand"
            mitm.colour[p] = WHITE;
            break;
        case 11:                //"plastic wand"
            mitm.colour[p] = random_colour();
            break;
        }

        if (mitm.special[p] / 12 == 9)
            mitm.colour[p] = DARKGREY;

        // rare wands (eg disintegration - these will be very rare):
        // maybe only 1 thing, like: crystal, shining, etc.
        break;

    case OBJ_POTIONS:
        mitm.special[p] = you.item_description[IDESC_POTIONS][mitm.sub_type[p]];

        switch (mitm.special[p] % 14)
        {
        case 0:         //"clear potion"
            mitm.colour[p] = LIGHTGREY;
            break;
        case 1:         //"blue potion"
        case 7:         //"inky potion"
            mitm.colour[p] = BLUE;
            break;
        case 2:         //"black potion"
            mitm.colour[p] = DARKGREY;
            break;
        case 3:         //"silvery potion"
        case 13:        //"white potion"
            mitm.colour[p] = WHITE;
            break;
        case 4:         //"cyan potion"
            mitm.colour[p] = CYAN;
            break;
        case 5:         //"purple potion"
            mitm.colour[p] = MAGENTA;
            break;
        case 6:         //"orange potion"
            mitm.colour[p] = LIGHTRED;
            break;
        case 8:         //"red potion"
            mitm.colour[p] = RED;
            break;
        case 9:         //"yellow potion"
            mitm.colour[p] = YELLOW;
            break;
        case 10:        //"green potion"
            mitm.colour[p] = GREEN;
            break;
        case 11:        //"brown potion"
            mitm.colour[p] = BROWN;
            break;
        case 12:        //"pink potion"
            mitm.colour[p] = LIGHTMAGENTA;
            break;
        }
        break;

    case OBJ_FOOD:
        switch (mitm.sub_type[p])
        {
        case FOOD_BEEF_JERKY:
        case FOOD_BREAD_RATION:
        case FOOD_LYCHEE:
        case FOOD_MEAT_RATION:
        case FOOD_RAMBUTAN:
        case FOOD_SAUSAGE:
        case FOOD_SULTANA:
            mitm.colour[p] = BROWN;
            break;
        case FOOD_BANANA:
        case FOOD_CHEESE:
        case FOOD_HONEYCOMB:
        case FOOD_LEMON:
        case FOOD_PIZZA:
        case FOOD_ROYAL_JELLY:
            mitm.colour[p] = YELLOW;
            break;
        case FOOD_PEAR:
            mitm.colour[p] = LIGHTGREEN;
            break;
        case FOOD_CHOKO:
        case FOOD_SNOZZCUMBER:
            mitm.colour[p] = GREEN;
            break;
        case FOOD_APRICOT:
        case FOOD_ORANGE:
            mitm.colour[p] = LIGHTRED;
            break;
        case FOOD_STRAWBERRY:
            mitm.colour[p] = RED;
            break;
        case FOOD_APPLE:
            mitm.colour[p] = (coinflip() ? RED : GREEN);
            break;
        case FOOD_GRAPE:
            mitm.colour[p] = (coinflip() ? MAGENTA : GREEN);
            break;
        default:
            mitm.colour[p] = BROWN;
        }
        break;

    case OBJ_JEWELLERY:
        if (mitm.special[p] == 200)
        {
            mitm.colour[p] = random_colour();
            break;
        }

        if (mitm.special[p] == 201)
            break;              /* unrandarts have already been coloured */

        mitm.colour[p] = YELLOW;
        mitm.special[p] = you.item_description[IDESC_RINGS][mitm.sub_type[p]];

        switchnum = mitm.special[p] % 13;

        switch (switchnum)
        {
        case 0:
        case 5:
            mitm.colour[p] = BROWN;
            break;
        case 1:
        case 8:
        case 11:
            mitm.colour[p] = LIGHTGREY;
            break;
        case 2:
        case 6:
            mitm.colour[p] = YELLOW;
            break;
        case 3:
        case 4:
            mitm.colour[p] = CYAN;
            break;
        case 7:
            mitm.colour[p] = BROWN;
            break;
        case 9:
        case 10:
            mitm.colour[p] = WHITE;
            break;
        case 12:
            mitm.colour[p] = GREEN;
            break;
        case 13:
            mitm.colour[p] = LIGHTCYAN;
            break;
        }

        if (mitm.sub_type[p] >= AMU_RAGE)
        {
            switch (switchnum)
            {
            case 0:             //"zirconium amulet"
            case 9:             //"ivory amulet"
            case 11:            //"platinum amulet"
                mitm.colour[p] = WHITE;
                break;
            case 1:             //"sapphire amulet"
                mitm.colour[p] = LIGHTBLUE;
                break;
            case 2:             //"golden amulet"
            case 6:             //"brass amulet"
                mitm.colour[p] = YELLOW;
                break;
            case 3:             //"emerald amulet"
                mitm.colour[p] = GREEN;
                break;
            case 4:             //"garnet amulet"
            case 8:             //"ruby amulet"
                mitm.colour[p] = RED;
                break;
            case 5:             //"bronze amulet"
            case 7:             //"copper amulet"
                mitm.colour[p] = BROWN;
                break;
            case 10:            //"bone amulet"
                mitm.colour[p] = LIGHTGREY;
                break;
            case 12:            //"jade amulet"
                mitm.colour[p] = GREEN;
                break;
            case 13:            //"plastic amulet"
                mitm.colour[p] = random_colour();
            }
        }

        // blackened - same for both rings and amulets
        if (mitm.special[p] / 13 == 5)
            mitm.colour[p] = DARKGREY;
        break;

    case OBJ_SCROLLS:
        mitm.colour[p] = LIGHTGREY;
        mitm.special[p] = you.item_description[IDESC_SCROLLS][mitm.sub_type[p]];
        mitm.pluses[p] = you.item_description[IDESC_SCROLLS_II][mitm.sub_type[p]];
        break;

    case OBJ_BOOKS:
        switch (mitm.special[p] % 10)
        {
        case 0:
        case 1:
            mitm.colour[p] = random_colour();
            break;
        case 2:
            mitm.colour[p] = (one_chance_in(3) ? BROWN : DARKGREY);
            break;
        case 3:
            mitm.colour[p] = CYAN;
            break;
        case 4:
            mitm.colour[p] = LIGHTGREY;
            break;
        default:
            break;
        }
        break;

    case OBJ_STAVES:
        mitm.colour[p] = BROWN;
        break;

    case OBJ_ORBS:
        mitm.colour[p] = LIGHTMAGENTA;
        break;

    case OBJ_MISCELLANY:
        switch (mitm.sub_type[p])
        {
        case MISC_BOTTLED_EFREET:
        case MISC_STONE_OF_EARTH_ELEMENTALS:
            mitm.colour[p] = BROWN;
            break;

        case MISC_AIR_ELEMENTAL_FAN:
        case MISC_CRYSTAL_BALL_OF_ENERGY:
        case MISC_CRYSTAL_BALL_OF_FIXATION:
        case MISC_CRYSTAL_BALL_OF_SEEING:
        case MISC_DISC_OF_STORMS:
        case MISC_HORN_OF_GERYON:
        case MISC_LANTERN_OF_SHADOWS:
            mitm.colour[p] = LIGHTGREY;
            break;

        case MISC_LAMP_OF_FIRE:
            mitm.colour[p] = YELLOW;
            break;

        case MISC_BOX_OF_BEASTS:
        case MISC_RUNE_OF_ZOT:
            mitm.colour[p] = DARKGREY;
            break;

        case MISC_EMPTY_EBONY_CASKET:
            mitm.colour[p] = DARKGREY;
            break;

        case MISC_DECK_OF_SUMMONINGS:
        case MISC_DECK_OF_WONDERS:
        default:
            mitm.colour[p] = random_colour();
            break;
        }
        break;

    case OBJ_CORPSES:
        mitm.colour[p] = RED;
        break;
    }
}                               // end item_colour()

// Checks how rare a weapon is. Many of these have special routines for
// placement, especially those with a rarity of zero. Chance is out of 10.
static char rare_weapon(unsigned char w_type)
{

    // zero value weapons must be placed specially -- see items() {dlb}
    if (is_demonic(w_type))
        return 0;

    switch (w_type)
    {
    case WPN_CLUB:
    case WPN_DAGGER:
        return 10;
    case WPN_HAND_AXE:
    case WPN_MACE:
    case WPN_QUARTERSTAFF:
        return 9;
    case WPN_BOW:
    case WPN_FLAIL:
    case WPN_HAMMER:
    case WPN_LONG_SWORD:
    case WPN_SABRE:
    case WPN_SHORT_SWORD:
    case WPN_SLING:
    case WPN_SPEAR:
        return 8;
    case WPN_AXE:
    case WPN_MORNINGSTAR:
        return 7;
    case WPN_BATTLEAXE:
    case WPN_CROSSBOW:
    case WPN_GREAT_SWORD:
    case WPN_SCIMITAR:
    case WPN_TRIDENT:
        return 6;
    case WPN_GLAIVE:
    case WPN_HALBERD:
        return 5;
    case WPN_BROAD_AXE:
    case WPN_HAND_CROSSBOW:
    case WPN_SPIKED_FLAIL:
    case WPN_WHIP:
        return 4;
    case WPN_GREAT_MACE:
        return 3;
    case WPN_ANCUS:
    case WPN_GREAT_FLAIL:
    case WPN_SCYTHE:
        return 2;
    case WPN_GIANT_CLUB:
    case WPN_GIANT_SPIKED_CLUB:
        return 1;
        // zero value weapons must be placed specially -- see items() {dlb}
    case WPN_DOUBLE_SWORD:
    case WPN_EVENINGSTAR:
    case WPN_EXECUTIONERS_AXE:
    case WPN_KATANA:
    case WPN_KNIFE:
    case WPN_QUICK_BLADE:
    case WPN_TRIPLE_SWORD:
        return 0;

    default:
        // should not happen now that calls are bounded by NUM_WEAPONS {dlb}
        return -1;
    }
}                               // end rare_weapon()

//jmf: generate altar based on where you are, or possibly randomly
static int pick_an_altar(void)
{
    int altar_type = 0;
    int temp_rand;              // probability determination {dlb}

    if (!one_chance_in(5))
    {
        switch (you.where_are_you)
        {
        case BRANCH_CRYPT:
            altar_type = (coinflip() ? DNGN_ALTAR_KIKUBAAQUDGHA
                                     : DNGN_ALTAR_YREDELEMNUL);
            break;

        case BRANCH_ORCISH_MINES:       // violent gods
            temp_rand = random2(5);

            altar_type = ((temp_rand == 0) ? DNGN_ALTAR_VEHUMET :
                          (temp_rand == 1) ? DNGN_ALTAR_MAKHLEB :
                          (temp_rand == 2) ? DNGN_ALTAR_OKAWARU :
                          (temp_rand == 3) ? DNGN_ALTAR_TROG
                                           : DNGN_ALTAR_XOM);
            break;

        case BRANCH_SLIME_PITS:             //jmf: no altars in Slime Pits
        case BRANCH_ECUMENICAL_TEMPLE:      //or extra ones in the temple
            altar_type = DNGN_FLOOR;
            break;

        case BRANCH_VAULTS: // "lawful" gods
            temp_rand = random2(7);

            altar_type = ((temp_rand == 0) ? DNGN_ALTAR_ELYVILON :
                          (temp_rand == 1) ? DNGN_ALTAR_SIF_MUNA :
                          (temp_rand == 2) ? DNGN_ALTAR_SHINING_ONE :
                          (temp_rand == 3 || temp_rand == 4) ? DNGN_ALTAR_OKAWARU
                                           : DNGN_ALTAR_ZIN);
            break;

        case BRANCH_HALL_OF_BLADES:
            altar_type = DNGN_ALTAR_OKAWARU;
            break;

        case BRANCH_ELVEN_HALLS:    // "magic" gods
            temp_rand = random2(4);

            altar_type = ((temp_rand == 0) ? DNGN_ALTAR_VEHUMET :
                          (temp_rand == 1) ? DNGN_ALTAR_SIF_MUNA :
                          (temp_rand == 2) ? DNGN_ALTAR_XOM
                                           : DNGN_ALTAR_MAKHLEB);
            break;

        case BRANCH_TOMB:
            altar_type = DNGN_ALTAR_KIKUBAAQUDGHA;
            break;

        default:
            do
            {
                altar_type = DNGN_ALTAR_ZIN + random2(NUM_GODS - 1);
            }
            while (altar_type == DNGN_ALTAR_NEMELEX_XOBEH);
            break;
        }
    }
    else
    {
        temp_rand = random2(9);

        altar_type = ((temp_rand == 0) ? DNGN_ALTAR_ZIN :
                      (temp_rand == 1) ? DNGN_ALTAR_SHINING_ONE :
                      (temp_rand == 2) ? DNGN_ALTAR_KIKUBAAQUDGHA :
                      (temp_rand == 3) ? DNGN_ALTAR_XOM :
                      (temp_rand == 4) ? DNGN_ALTAR_OKAWARU :
                      (temp_rand == 5) ? DNGN_ALTAR_MAKHLEB :
                      (temp_rand == 6) ? DNGN_ALTAR_SIF_MUNA :
                      (temp_rand == 7) ? DNGN_ALTAR_TROG
                                       : DNGN_ALTAR_ELYVILON);
    }

    return altar_type;
}                               // end pick_an_altar()

static void place_altar(void)
{
    int px, py;
    int i, j;
    int k = 0, l = 0;
    int altar_type = pick_an_altar();

    while(true)
    {
      rand_px:

        px = 15 + random2(55);
        py = 15 + random2(45);
        k++;

        if (k == 5000)
            return;

        l = 0;

        for (i = px - 2; i < px + 3; i++)
        {
            for (j = py - 2; j < py + 3; j++)
            {
                if (grd[i][j] == DNGN_FLOOR)
                    l++;

                if ((grd[i][j] != DNGN_ROCK_WALL
                        && grd[i][j] != DNGN_CLOSED_DOOR
                        && grd[i][j] != DNGN_SECRET_DOOR
                        && grd[i][j] != DNGN_FLOOR)
                    || mgrd[i][j] != NON_MONSTER)
                {
                    goto rand_px;
                }
            }
        }

        if (l == 0)
            goto rand_px;

        for (i = px - 2; i < px + 3; i++)
        {
            for (j = py - 2; j < py + 3; j++)
            {
                grd[i][j] = DNGN_FLOOR;
            }
        }

        grd[px][py] = altar_type;

        return;
    }
}                               // end place_altar()

static void place_shops(int level_number)
{
    int temp_rand = 0;          // probability determination {dlb}
    int timeout = 0;

    unsigned char no_shops = 0;
    unsigned char shop_place_x = 0;
    unsigned char shop_place_y = 0;

    // maximum number of shops placed is four {dlb}:
    temp_rand = random2(125);

    no_shops = ((temp_rand > 28) ? 0 :           // 76.8% probability
                (temp_rand > 4)  ? 1             // 19.2% probability
                                 : 1 + random2(4));  //  4.0% probability

    if (no_shops == 0 || level_number < 3)
        return;

    for (int i = 0; i < no_shops; i++)
    {
        timeout = 0;

        do
        {
            shop_place_x = random2(GXM - 20) + 10;
            shop_place_y = random2(GYM - 20) + 10;

            timeout++;

            if (timeout > 20000)
                return;
        }
        while (grd[shop_place_x][shop_place_y] != DNGN_FLOOR);

        place_spec_shop(level_number, shop_place_x, shop_place_y, SHOP_RANDOM);
    }
}                               // end place_shops()

static void place_spec_shop(int level_number, unsigned char shop_x, unsigned char shop_y,
                            unsigned char force_s_type)
{
    int orb = 0;
    int i = 0;
    int j = 0;                  // loop variable
    int item_level;

    for (i = 0; i < 6; i++)
    {
        if (i == 5)
            return;
        if (env.shop_type[i] == SHOP_UNASSIGNED)
            break;
    }

    for (j = 0; j < 3; j++)
    {
        env.keeper_name[i][j] = 1 + random2(200);
    }

    env.shop_level[i] = level_number * 2;

    env.shop_type[i] =
        ((force_s_type != SHOP_RANDOM) ? force_s_type : random2(NUM_SHOPS));

    if (env.shop_type[i] != SHOP_WEAPON_ANTIQUE
        && env.shop_type[i] != SHOP_ARMOUR_ANTIQUE
        && env.shop_type[i] != SHOP_GENERAL_ANTIQUE)
    {
        env.shop_greed[i] = 10 + random2(5) + random2(level_number / 2);
    }
    else
        env.shop_greed[i] = 15 + random2avg(19, 2) + random2(level_number);

    int plojy = 5 + random2avg(12, 3);

    for (j = 0; j < plojy; j++)
    {
        if (env.shop_type[i] != SHOP_WEAPON_ANTIQUE
            && env.shop_type[i] != SHOP_ARMOUR_ANTIQUE
            && env.shop_type[i] != SHOP_GENERAL_ANTIQUE)
        {
            item_level = level_number + random2((level_number + 1) * 2);
        }
        else
            item_level = level_number + random2((level_number + 1) * 3);

        if (one_chance_in(4))
            item_level = 351;

        // don't generate gold in shops!  This used to be possible with
        // General Stores (see item_in_shop() below)   (GDL)
        while(true)
        {
            orb = items( 1, item_in_shop(env.shop_type[i]), OBJ_RANDOM, 0,
                item_level, 250 );
            if (mitm.base_type[orb] != OBJ_GOLD)
                break;
            // reset object and try again
            mitm.base_type[orb] = OBJ_UNASSIGNED;
        }

        // set object 'position' (gah!) & ID status
        mitm.x[orb] = 0;
        mitm.y[orb] = 5 + i;

        if (env.shop_type[i] != SHOP_WEAPON_ANTIQUE
            && env.shop_type[i] != SHOP_ARMOUR_ANTIQUE
            && env.shop_type[i] != SHOP_GENERAL_ANTIQUE)
        {
            mitm.id[orb] = 3;
        }
    }

    env.shop_x[i] = shop_x;
    env.shop_y[i] = shop_y;

    grd[shop_x][shop_y] = DNGN_ENTER_SHOP;
}                               // end place_spec_shop()

static unsigned char item_in_shop(unsigned char shop_type)
{
    switch (shop_type)
    {
    case SHOP_WEAPON:
        if (one_chance_in(3))
            return OBJ_MISSILES;
        // *** deliberate fall through here  {dlb} ***
    case SHOP_WEAPON_ANTIQUE:
        return OBJ_WEAPONS;

    case SHOP_ARMOUR:
    case SHOP_ARMOUR_ANTIQUE:
        return OBJ_ARMOUR;

    case SHOP_GENERAL:
    case SHOP_GENERAL_ANTIQUE:
        return OBJ_RANDOM;

    case SHOP_JEWELLERY:
        return OBJ_JEWELLERY;

    case SHOP_WAND:
        return OBJ_WANDS;

    case SHOP_BOOK:
        return OBJ_BOOKS;

    case SHOP_FOOD:
        return OBJ_FOOD;

    case SHOP_DISTILLERY:
        return OBJ_POTIONS;

    case SHOP_SCROLL:
        return OBJ_SCROLLS;
    }

    return OBJ_RANDOM;
}                               // end item_in_shop()

static void spotty_level(bool seeded, int iterations, bool boxy)
{
    // assumes starting with a level full of rock walls (1)
    int i, j, k, l;

    if (!seeded)
    {
        for (i = DNGN_STONE_STAIRS_DOWN_I; i < DNGN_ROCK_STAIRS_UP; i++)
        {
            if (i == DNGN_ROCK_STAIRS_DOWN || i == DNGN_STONE_STAIRS_UP_I)
                continue;

            do
            {
                j = 10 + random2(GXM - 20);
                k = 10 + random2(GYM - 20);
            }
            while (grd[j][k] != DNGN_ROCK_WALL
                            && grd[j + 1][k] != DNGN_ROCK_WALL);

            grd[j][k] = i;

            if (i == DNGN_STONE_STAIRS_DOWN_I)
                grd[j + 1][k] = DNGN_STONE_STAIRS_UP_I;
            if (grd[j][k - 1] == DNGN_ROCK_WALL)
                grd[j][k - 1] = DNGN_FLOOR;
            if (grd[j][k + 1] == DNGN_ROCK_WALL)
                grd[j][k + 1] = DNGN_FLOOR;
            if (grd[j - 1][k] == DNGN_ROCK_WALL)
                grd[j - 1][k] = DNGN_FLOOR;
            if (grd[j + 1][k] == DNGN_ROCK_WALL)
                grd[j + 1][k] = DNGN_FLOOR;
        }
    }                           // end if !seeded

    l = iterations;

    if (l == 0)
        l = 200 + random2(1500);

    for (i = 0; i < l; i++)
    {
        do
        {
            j = random2(GXM - 20) + 10;
            k = random2(GYM - 20) + 10;
        }
        while (grd[j][k] == DNGN_ROCK_WALL
               && grd[j - 1][k] == DNGN_ROCK_WALL
               && grd[j + 1][k] == DNGN_ROCK_WALL
               && grd[j][k - 1] == DNGN_ROCK_WALL
               && grd[j][k + 1] == DNGN_ROCK_WALL
               && grd[j - 2][k] == DNGN_ROCK_WALL
               && grd[j + 2][k] == DNGN_ROCK_WALL
               && grd[j][k - 2] == DNGN_ROCK_WALL
               && grd[j][k + 2] == DNGN_ROCK_WALL);

        if (grd[j][k] == DNGN_ROCK_WALL)
            grd[j][k] = DNGN_FLOOR;
        if (grd[j][k - 1] == DNGN_ROCK_WALL)
            grd[j][k - 1] = DNGN_FLOOR;
        if (grd[j][k + 1] == DNGN_ROCK_WALL)
            grd[j][k + 1] = DNGN_FLOOR;
        if (grd[j - 1][k] == DNGN_ROCK_WALL)
            grd[j - 1][k] = DNGN_FLOOR;
        if (grd[j + 1][k] == DNGN_ROCK_WALL)
            grd[j + 1][k] = DNGN_FLOOR;

        if (boxy)
        {
            if (grd[j - 1][k - 1] == DNGN_ROCK_WALL)
                grd[j][k - 1] = DNGN_FLOOR;
            if (grd[j + 1][k + 1] == DNGN_ROCK_WALL)
                grd[j][k + 1] = DNGN_FLOOR;
            if (grd[j - 1][k + 1] == DNGN_ROCK_WALL)
                grd[j - 1][k] = DNGN_FLOOR;
            if (grd[j + 1][k - 1] == DNGN_ROCK_WALL)
                grd[j + 1][k] = DNGN_FLOOR;
        }
    }
}                               // end spotty_level()

static void bigger_room(void)
{
    unsigned char i, j;
    unsigned char pos_x, pos_y;

    for (i = 10; i < (GXM - 10); i++)
    {
        for (j = 10; j < (GYM - 10); j++)
        {
            if (grd[i][j] == DNGN_ROCK_WALL)
                grd[i][j] = DNGN_FLOOR;
        }
    }

    many_pools(DNGN_DEEP_WATER);

    int pair_count = coinflip()?4:3;

    for (j = 0; j < pair_count; j++)
        for (i = 0; i < 2; i++)
            place_specific_stair(j + ((i==0)?DNGN_STONE_STAIRS_DOWN_I
                                            :DNGN_STONE_STAIRS_UP_I));
}                               // end bigger_room()

// various plan_xxx functions
static void plan_main(int level_number, char force_plan)
{
    // possible values for do_stairs:
    //  0 - stairs already done
    //  1 - stairs already done, do spotty
    //  2 - no stairs
    //  3 - no stairs, do spotty
    char do_stairs = 0;
    unsigned char pos_x;
    unsigned char pos_y;
    unsigned char special_grid = (one_chance_in(3) ? DNGN_METAL_WALL
                                                   : DNGN_STONE_WALL);
    int i,j;

    if (!force_plan)
        force_plan = 1 + random2(12);

    do_stairs = ((force_plan == 1) ? plan_1() :
                 (force_plan == 2) ? plan_2() :
                 (force_plan == 3) ? plan_3() :
                 (force_plan == 4) ? plan_4(0, 0, 0, 0, 99) :
                 (force_plan == 5) ? (one_chance_in(9) ? plan_5()
                                                       : plan_3()) :
                 (force_plan == 6) ? plan_6(level_number)
                                   : plan_3());

    if (do_stairs == 3 || do_stairs == 1)
        spotty_level(true, 0, coinflip());

    if (do_stairs == 2 || do_stairs == 3)
    {
        int pair_count = coinflip()?4:3;

        for (j = 0; j < pair_count; j++)
            for (i = 0; i < 2; i++)
                place_specific_stair(j + ((i==0)?DNGN_STONE_STAIRS_DOWN_I
                                                :DNGN_STONE_STAIRS_UP_I));
    }

    if (one_chance_in(20))
        replace_area(0,0,GXM-1,GYM-1,DNGN_ROCK_WALL,special_grid);
}                               // end plan_main()

static char plan_1(void)
{
    int temp_rand = 0;          // probability determination {dlb}

    unsigned char width = (10 - random2(7));    // value range of [4,10] {dlb}

    replace_area(10, 10, (GXM - 10), (10 + width), DNGN_ROCK_WALL, DNGN_FLOOR);
    replace_area(10, (60 - width), (GXM - 10), (GYM - 10),
        DNGN_ROCK_WALL, DNGN_FLOOR);
    replace_area(10, 10, (10 + width), (GYM - 10), DNGN_ROCK_WALL, DNGN_FLOOR);
    replace_area((60 - width), 10, (GXM - 10), (GYM - 10),
        DNGN_ROCK_WALL, DNGN_FLOOR);

    // possible early returns {dlb}:
    temp_rand = random2(4);

    if (temp_rand > 2)          // 25% chance {dlb}
        return 3;
    else if (temp_rand > 1)     // 25% chance {dlb}
        return 2;
    else                        // 50% chance {dlb}
    {
        unsigned char width2 = (coinflip()? (1 + random2(5)) : 5);

        replace_area(10, (35 - width2), (GXM - 10), (35 + width2),
                   DNGN_ROCK_WALL, DNGN_FLOOR);
        replace_area((40 - width2), 10, (40 + width2), (GYM - 10),
                   DNGN_ROCK_WALL, DNGN_FLOOR);
    }

    // possible early returns {dlb}:
    temp_rand = random2(4);

    if (temp_rand > 2)          // 25% chance {dlb}
        return 3;
    else if (temp_rand > 1)     // 25% chance {dlb}
        return 2;
    else                        // 50% chance {dlb}
    {
        temp_rand = random2(15);

        if (temp_rand > 7)      // 7 in 15 odds {dlb}
        {
            spec_room sr = { false, false, 0,0,0,0 };
            sr.x1 = 25;
            sr.y1 = 25;
            sr.x2 = (GXM - 25);
            sr.y2 = (GYM - 25);

            int oblique_max = 0;
            if (coinflip())
                oblique_max = 5 + random2(20);

            temp_rand = random2(7);

            unsigned char floor_type = ((temp_rand > 1) ? DNGN_FLOOR :   // 5/7
                                        (temp_rand > 0) ? DNGN_DEEP_WATER// 1/7
                                                        : DNGN_LAVA);    // 1/7
            octa_room(sr, oblique_max, floor_type);
        }
    }

    // final return {dlb}:
    return (one_chance_in(5) ? 3 : 2);
}                               // end plan_1()

// just a cross:
static char plan_2(void)
{
    char width2 = (5 - random2(5));     // value range of [1,5] {dlb}

    replace_area(10, (35 - width2), (GXM - 10), (35 + width2),
                                            DNGN_ROCK_WALL, DNGN_FLOOR);
    replace_area((40 - width2), 10, (40 + width2), (GYM - 10),
                                            DNGN_ROCK_WALL, DNGN_FLOOR);

    return (one_chance_in(4) ? 2 : 3);
}                               // end plan_2()

static char plan_3(void)
{

    /* Draws a room, then another and links them together, then another and etc
       Of course, this can easily end up looking just like a make_trail level.
     */
    int i;
    char cnx, cny;
    char roomsss = 30 + random2(90);

    bool exclusive = (one_chance_in(10) ? false : true);
    bool exclusive2 = coinflip();

    char romx1[30], romy1[30], romx2[30], romy2[30];

    char which_room = 0;

    for (i = 0; i < roomsss; i++)
    {
        romx1[which_room] = 10 + random2(50);
        romy1[which_room] = 10 + random2(40);
        romx2[which_room] = romx1[which_room] + 2 + random2(8);
        romy2[which_room] = romy1[which_room] + 2 + random2(8);

        if (exclusive)
        {
            for (cnx = romx1[which_room] - 1; cnx < romx2[which_room] + 1;
                                                                        cnx++)
            {
                for (cny = romy1[which_room] - 1; cny < romy2[which_room] + 1;
                                                                        cny++)
                {
                    if (grd[cnx][cny] != DNGN_ROCK_WALL)
                        goto continuing;
                }
            }
        }

        replace_area(romx1[which_room], romy1[which_room], romx2[which_room],
                   romy2[which_room], DNGN_ROCK_WALL, DNGN_FLOOR);

        if (which_room > 0 && !exclusive2)
        {
            const int rx1 = romx1[which_room];
            const int rx2 = romx2[which_room];
            const int prev_rx1 = romx1[which_room - 1];
            const int prev_rx2 = romx2[which_room - 1];

            const int ry1 = romy1[which_room];
            const int ry2 = romy2[which_room];
            const int prev_ry1 = romy1[which_room - 1];
            const int prev_ry2 = romy2[which_room - 1];

            join_the_dots( rx1 + random2( rx2 - rx1 ),
                           ry1 + random2( ry2 - ry1 ),
                           prev_rx1 + random2( prev_rx2 - prev_rx1 ),
                           prev_ry1 + random2( prev_ry2 - prev_ry1 ),
                           0, 0, 0, 0 );
        }

        which_room++;

        if (which_room >= 29)
            break;

      continuing:
        continue;
    }

    if (exclusive2)
    {
        for (i = 0; i < which_room; i++)
        {
            if (i > 0)
            {
                const int rx1 = romx1[i];
                const int rx2 = romx2[i];
                const int prev_rx1 = romx1[i - 1];
                const int prev_rx2 = romx2[i - 1];

                const int ry1 = romy1[i];
                const int ry2 = romy2[i];
                const int prev_ry1 = romy1[i - 1];
                const int prev_ry2 = romy2[i - 1];

                join_the_dots( rx1 + random2( rx2 - rx1 ),
                               ry1 + random2( ry2 - ry1 ),
                               prev_rx1 + random2( prev_rx2 - prev_rx1 ),
                               prev_ry1 + random2( prev_ry2 - prev_ry1 ),
                               0, 0, 0, 0 );
            }
        }
    }

    return 2;
}                               // end plan_3()

static char plan_4(char forbid_x1, char forbid_y1, char forbid_x2,
                            char forbid_y2, unsigned char force_wall)
{
    // a more chaotic version of city level
    int temp_rand;              // req'd for probability checking

    int number_boxes = 5000;
    unsigned char drawing = DNGN_ROCK_WALL;
    char b1x, b1y, b2x, b2y;
    char cnx, cny;
    int i;

    temp_rand = random2(81);

    number_boxes = ((temp_rand > 48) ? 4000 :   // odds: 32 in 81 {dlb}
                    (temp_rand > 24) ? 3000 :   // odds: 24 in 81 {dlb}
                    (temp_rand >  8) ? 5000 :   // odds: 16 in 81 {dlb}
                    (temp_rand >  0) ? 2000     // odds:  8 in 81 {dlb}
                                     : 1000);   // odds:  1 in 81 {dlb}

    if (force_wall != 99)
        drawing = force_wall;
    else
    {
        temp_rand = random2(18);

        drawing = ((temp_rand > 7) ? DNGN_ROCK_WALL :   // odds: 10 in 18 {dlb}
                   (temp_rand > 2) ? DNGN_STONE_WALL    // odds:  5 in 18 {dlb}
                                   : DNGN_METAL_WALL);  // odds:  3 in 18 {dlb}
    }

    replace_area(10, 10, (GXM - 10), (GYM - 10), DNGN_ROCK_WALL, DNGN_FLOOR);

    // replace_area can also be used to fill in:
    for (i = 0; i < number_boxes; i++)
    {

        b1x = 11 + random2(45);
        b1y = 11 + random2(35);

        b2x = b1x + 3 + random2(7) + random2(5);
        b2y = b1y + 3 + random2(7) + random2(5);

        if (forbid_x1 != 0 || forbid_x2 != 0)
        {
            if (b1x <= forbid_x2 && b1x >= forbid_x1
                    && b1y <= forbid_y2 && b1y >= forbid_y1)
            {
                goto continuing;
            }
            else if (b2x <= forbid_x2 && b2x >= forbid_x1
                    && b2y <= forbid_y2 && b2y >= forbid_y1)
            {
                goto continuing;
            }
        }

        for (cnx = b1x - 1; cnx < b2x + 1; cnx++)
        {
            for (cny = b1y - 1; cny < b2y + 1; cny++)
            {
                if (grd[cnx][cny] != DNGN_FLOOR)
                    goto continuing;
            }
        }

        if (force_wall == 99)
        {
            // NB: comparison reversal here - combined
            temp_rand = random2(1200);

            // probabilities *not meant* to sum to one! {dlb}
            if (temp_rand < 417)        // odds: 261 in 1200 {dlb}
                drawing = DNGN_ROCK_WALL;
            else if (temp_rand < 156)   // odds: 116 in 1200 {dlb}
                drawing = DNGN_STONE_WALL;
            else if (temp_rand < 40)    // odds:  40 in 1200 {dlb}
                drawing = DNGN_METAL_WALL;
        }

        temp_rand = random2(210);

        if (temp_rand > 71)     // odds: 138 in 210 {dlb}
            replace_area(b1x, b1y, b2x, b2y, DNGN_FLOOR, drawing);
        else                    // odds:  72 in 210 {dlb}
            box_room(b1x, b2x - 1, b1y, b2y - 1, drawing);

      continuing:
        continue;
    }

    if (forbid_x1 == 0 && one_chance_in(4))     // a market square
    {
        spec_room sr = { false, false, 0, 0, 0, 0 };
        sr.x1 = 25;
        sr.y1 = 25;
        sr.x2 = 55;
        sr.y2 = 45;

        int oblique_max = 0;
        if (!one_chance_in(4))
            oblique_max = 5 + random2(20);      // used elsewhere {dlb}

        unsigned char feature = DNGN_FLOOR;
        if (one_chance_in(10))
            feature = coinflip()? DNGN_DEEP_WATER : DNGN_LAVA;

        octa_room(sr, oblique_max, feature);
    }

    return 2;
}                               // end plan_4()

static char plan_5(void)
{
    unsigned char imax = 5 + random2(20);       // value range of [5,24] {dlb}

    for (unsigned char i = 0; i < imax; i++)
    {
        join_the_dots( random2(GXM - 20) + 10, random2(GYM - 20) + 10,
                       random2(GXM - 20) + 10, random2(GYM - 20) + 10,
                       0, 0, 0, 0 );
    }

    if (!one_chance_in(4))
        spotty_level(true, 100, coinflip());

    return 2;
}                               // end plan_5()

static char plan_6(int level_number)
{
    spec_room sr = { false, false, 0,0,0,0 };

    // circle of standing stones (well, kind of)
    sr.x1 = 10;
    sr.x2 = (GXM - 10);
    sr.y1 = 10;
    sr.y2 = (GYM - 10);

    octa_room(sr, 14, DNGN_FLOOR);

    replace_area(23, 23, 26, 26, DNGN_FLOOR, DNGN_STONE_WALL);
    replace_area(23, 47, 26, 50, DNGN_FLOOR, DNGN_STONE_WALL);
    replace_area(55, 23, 58, 26, DNGN_FLOOR, DNGN_STONE_WALL);
    replace_area(55, 47, 58, 50, DNGN_FLOOR, DNGN_STONE_WALL);
    replace_area(39, 20, 43, 23, DNGN_FLOOR, DNGN_STONE_WALL);
    replace_area(39, 50, 43, 53, DNGN_FLOOR, DNGN_STONE_WALL);
    replace_area(20, 30, 23, 33, DNGN_FLOOR, DNGN_STONE_WALL);
    replace_area(20, 40, 23, 43, DNGN_FLOOR, DNGN_STONE_WALL);
    replace_area(58, 30, 61, 33, DNGN_FLOOR, DNGN_STONE_WALL);
    replace_area(58, 40, 61, 43, DNGN_FLOOR, DNGN_STONE_WALL);

    grd[35][32] = DNGN_STONE_WALL;
    grd[46][32] = DNGN_STONE_WALL;
    grd[35][40] = DNGN_STONE_WALL;
    grd[46][40] = DNGN_STONE_WALL;

    grd[69][34] = DNGN_STONE_STAIRS_DOWN_I;
    grd[69][35] = DNGN_STONE_STAIRS_DOWN_II;
    grd[69][36] = DNGN_STONE_STAIRS_DOWN_III;

    grd[10][34] = DNGN_STONE_STAIRS_UP_I;
    grd[10][35] = DNGN_STONE_STAIRS_UP_II;
    grd[10][36] = DNGN_STONE_STAIRS_UP_III;

    if (level_number > 20 && coinflip())
    {
        grd[40][36] = DNGN_ENTER_ABYSS; // remember pandemonium
        grd[41][36] = DNGN_ENTER_ABYSS;
    }

    return 0;
}                               // end plan_6()

static bool octa_room(spec_room &sr, int oblique_max, unsigned char type_floor)
{
    int x,y;

    // hack - avoid lava in the crypt {gdl}
    if ((you.where_are_you == BRANCH_CRYPT || you.where_are_you == BRANCH_TOMB)
         && type_floor == DNGN_LAVA)
        type_floor = DNGN_SHALLOW_WATER;

    int oblique = oblique_max;

    // check octagonal room for special; avoid if exists
    for (x = sr.x1; x < sr.x2; x++)
    {
        for (y = sr.y1 + oblique; y < sr.y2 - oblique; y++)
        {
            if (grd[x][y] == DNGN_BUILDER_SPECIAL_WALL)
                return false;
        }

        if (oblique > 0)
            oblique--;

        if (x > sr.x2 - oblique_max)
            oblique += 2;
    }

    oblique = oblique_max;


    for (x = sr.x1; x < sr.x2; x++)
    {
        for (y = sr.y1 + oblique; y < sr.y2 - oblique; y++)
        {
            if (grd[x][y] == DNGN_ROCK_WALL)
                grd[x][y] = type_floor;

            if (grd[x][y] == DNGN_FLOOR && type_floor == DNGN_SHALLOW_WATER)
                grd[x][y] = DNGN_SHALLOW_WATER;

            if (type_floor >= DNGN_LAST_SOLID_TILE
                && grd[x][y] == DNGN_CLOSED_DOOR)
            {
                grd[x][y] = DNGN_FLOOR;       // ick
            }
        }

        if (oblique > 0)
            oblique--;

        if (x > sr.x2 - oblique_max)
            oblique += 2;
    }

    return true;
}                               // end octa_room()

static void labyrinth_level(int level_number)
{
    int temp_rand;              // probability determination {dlb}

    int keep_lx = 0, keep_ly = 0;
    int keep_lx2 = 0, keep_ly2 = 0;
    char start_point_x = 10;
    char start_point_y = 10;
    char going_x = 1;
    char going_y = (coinflip() ? 0 : 1);
    bool do_2 = false;
    int clear_space = 1;
    unsigned char traps_put2 = 0;

    if (coinflip())
    {
        start_point_x = (GXM - 10);
        going_x = -1;
    }

    if (coinflip())
    {
        start_point_y = (GYM - 10);

        if (going_y == 1)
            going_y = -1;
    }

    int lx = start_point_x;
    int ly = start_point_y;

    if (going_y)
        goto do_y;

  do_x:
    traps_put2 = 0;
    clear_space = 0;            // ( coinflip()? 3 : 2 );

    do
    {
        lx += going_x;

        if (grd[lx][ly] == DNGN_ROCK_WALL)
            grd[lx][ly] = DNGN_FLOOR;
    }
    while (lx < (GXM - 8) && lx > 8
           && grd[lx + going_x * (2 + clear_space)][ly] == DNGN_ROCK_WALL);

    going_x = 0;

    if (ly < 32)
        going_y = 1;
    else if (ly > 37)
        going_y = -1;
    else
        goto finishing;

  do_y:                 // if (going_y != 0)
    if (do_2)
    {
        lx = keep_lx2;
        ly = keep_ly2;
    }

    // do_2 = false is the problem
    if (coinflip())
    {
        clear_space = 0;
        do_2 = false;
    }
    else
    {
        clear_space = 2;
        do_2 = true;
    }

    do
    {
        ly += going_y;

        if (grd[lx][ly] == DNGN_ROCK_WALL)
            grd[lx][ly] = DNGN_FLOOR;
    }
    while (ly < (GYM - 8) && ly > 8
           && grd[lx][ly + going_y * (2 + clear_space)] == DNGN_ROCK_WALL);

    keep_lx = lx;
    keep_ly = ly;

    if (lx < 37)
        going_x = 1;
    else if (lx > 42)
        going_x = -1;

    if (ly < 33)
        ly += 2;
    else if (ly > 37)
        ly -= 2;

    clear_space = ((!do_2) ? 6 : 2);

    do
    {
        lx += going_x;

        if (grd[lx][ly] == DNGN_ROCK_WALL)
            grd[lx][ly] = DNGN_FLOOR;
    }
    while (lx < (GXM - 8) && lx > 8
           && grd[lx + going_x * (2 + clear_space)][ly] == DNGN_ROCK_WALL);

    if (do_2)
    {
        keep_lx2 = lx;
        keep_ly2 = ly;
    }

    lx = keep_lx;
    ly = keep_ly;

    going_y = 0;

    if (lx < 37)
        going_x = 1;
    else if (lx > 42)
        going_x = -1;
    else
        goto finishing;

    goto do_x;

  finishing:
    start_point_x = 10 + random2(GXM - 20);

    int treasure_item = 0;

    unsigned char glopop = OBJ_RANDOM;  // used in calling items() {dlb}

    int num_items = 8 + random2avg(9, 2);
    for (int i = 0; i < num_items; i++)
    {
        temp_rand = random2(11);

        glopop = ((temp_rand == 0 || temp_rand == 9)  ? OBJ_WEAPONS :
                  (temp_rand == 1 || temp_rand == 10) ? OBJ_ARMOUR :
                  (temp_rand == 2)                    ? OBJ_MISSILES :
                  (temp_rand == 3)                    ? OBJ_WANDS :
                  (temp_rand == 4)                    ? OBJ_MISCELLANY :
                  (temp_rand == 5)                    ? OBJ_SCROLLS :
                  (temp_rand == 6)                    ? OBJ_JEWELLERY :
                  (temp_rand == 7)                    ? OBJ_BOOKS
                  /* (temp_rand == 8) */              : OBJ_STAVES);

        treasure_item = items(1, glopop, OBJ_RANDOM, 1, level_number * 3, 250);
        mitm.x[treasure_item] = lx;
        mitm.y[treasure_item] = ly;
    }

    mons_place(MONS_MINOTAUR, BEH_SLEEP, MHITNOT, true, lx, ly);

    grd[lx][ly] = DNGN_ROCK_STAIRS_UP;

    link_items();

    // turn rock walls into undiggable stone or metal:
    temp_rand = random2(50);

    unsigned char wall_xform = ((temp_rand > 10) ? DNGN_STONE_WALL   // 78.0%
                                                 : DNGN_METAL_WALL); // 22.0%

    replace_area(0,0,GXM-1,GYM-1,DNGN_ROCK_WALL,wall_xform);

}                               // end labyrinth_level()

static bool is_wall(int x, int y)
{
    unsigned char feat = grd[x][y];

    switch (feat)
    {
        case DNGN_ROCK_WALL:
        case DNGN_STONE_WALL:
        case DNGN_METAL_WALL:
        case DNGN_GREEN_CRYSTAL_WALL:
        case DNGN_WAX_WALL:
            return true;
        default:
            return false;
    }
}


static int box_room_door_spot(int x, int y)
{
    // if there is a door near us embedded in rock, we have to be a door too.
    if ((grd[x-1][y] == DNGN_CLOSED_DOOR && is_wall(x-1,y-1) && is_wall(x-1,y+1))
     || (grd[x+1][y] == DNGN_CLOSED_DOOR && is_wall(x+1,y-1) && is_wall(x+1,y+1))
     || (grd[x][y-1] == DNGN_CLOSED_DOOR && is_wall(x-1,y-1) && is_wall(x+1,y-1))
     || (grd[x][y+1] == DNGN_CLOSED_DOOR && is_wall(x-1,y+1) && is_wall(x+1,y+1)))
    {
        grd[x][y] = DNGN_CLOSED_DOOR;
        return 2;
    }

    // to be a good spot for a door, we need non-wall on two sides and
    // wall on two sides.
    bool nor = is_wall(x, y-1);
    bool sou = is_wall(x, y+1);
    bool eas = is_wall(x-1, y);
    bool wes = is_wall(x+1, y);

    if (nor == sou && eas == wes && nor != eas)
        return 1;

    return 0;
}

static int box_room_doors( int bx1, int bx2, int by1, int by2, int new_doors)
{
    int good_doors[200];        // 1 == good spot,  2 == door placed!
    int spot;
    int i,j;
    int doors_placed = new_doors;

    // sanity
    if ( 2 * ( (bx2 - bx1) + (by2-by1) ) > 200)
        return 0;

    // go through, building list of good door spots,  and replacing wall
    // with door if we're about to block off another door.
    int spot_count = 0;

    // top & bottom
    for(i=bx1+1; i<bx2; i++)
    {
        good_doors[spot_count ++] = box_room_door_spot(i, by1);
        good_doors[spot_count ++] = box_room_door_spot(i, by2);
    }
    // left & right
    for(i=by1+1; i<by2; i++)
    {
        good_doors[spot_count ++] = box_room_door_spot(bx1, i);
        good_doors[spot_count ++] = box_room_door_spot(bx2, i);
    }

    if (new_doors == 0)
    {
        // count # of doors we HAD to place
        for(i=0; i<spot_count; i++)
            if (good_doors[i] == 2)
                doors_placed++;

        return doors_placed;
    }

    while(new_doors > 0 && spot_count > 0)
    {
        spot = random2(spot_count);
        if (good_doors[spot] != 1)
            continue;

        j = 0;
        for(i=bx1+1; i<bx2; i++)
        {
            if (spot == j++)
            {
                grd[i][by1] = DNGN_CLOSED_DOOR;
                break;
            }
            if (spot == j++)
            {
                grd[i][by2] = DNGN_CLOSED_DOOR;
                break;
            }
        }

        for(i=by1+1; i<by2; i++)
        {
            if (spot == j++)
            {
                grd[bx1][i] = DNGN_CLOSED_DOOR;
                break;
            }
            if (spot == j++)
            {
                grd[bx2][i] = DNGN_CLOSED_DOOR;
                break;
            }
        }

        // try not to put a door in the same place twice
        good_doors[spot] = 2;
        new_doors --;
    }

    return doors_placed;
}


static void box_room(int bx1, int bx2, int by1, int by2, int wall_type)
{
    // hack -- avoid lava in the crypt. {gdl}
    if ((you.where_are_you == BRANCH_CRYPT || you.where_are_you == BRANCH_TOMB)
         && wall_type == DNGN_LAVA)
        wall_type = DNGN_SHALLOW_WATER;

    int temp_rand, new_doors, doors_placed;

    // do top & bottom walls
    replace_area(bx1,by1,bx2,by1,DNGN_FLOOR,wall_type);
    replace_area(bx1,by2,bx2,by2,DNGN_FLOOR,wall_type);

    // do left & right walls
    replace_area(bx1,by1+1,bx1,by2-1,DNGN_FLOOR,wall_type);
    replace_area(bx2,by1+1,bx2,by2-1,DNGN_FLOOR,wall_type);

    // sometimes we have to place doors, or else we shut in other buildings' doors
    doors_placed = box_room_doors(bx1, bx2, by1, by2, 0);

    temp_rand = random2(100);
    new_doors = (temp_rand > 45) ? 2 :
                ((temp_rand > 22) ? 1 : 3);

    // small rooms don't have as many doors
    if ((bx2-bx1)*(by2-by1) < 36)
        new_doors--;

    new_doors -= doors_placed;
    if (new_doors > 0)
        box_room_doors(bx1, bx2, by1, by2, new_doors);
}

static void city_level(int level_number)
{
    int temp_rand;          // probability determination {dlb}
    int wall_type;          // remember, can have many wall types in one level
    int wall_type_room;     // simplifies logic of innermost loop {dlb}

    int xs = 0, ys = 0;
    int x1 = 0, x2 = 0;
    int y1 = 0, y2 = 0;
    unsigned char pos_x, pos_y;
    int i,j;

    temp_rand = random2(8);

    wall_type = ((temp_rand > 4) ? DNGN_ROCK_WALL :     // 37.5% {dlb}
                 (temp_rand > 1) ? DNGN_STONE_WALL      // 37.5% {dlb}
                                 : DNGN_METAL_WALL);    // 25.0% {dlb}

    make_box(7,7,GXM-7,GYM-7,DNGN_FLOOR);

    for (i = 0; i < 5; i++)
    {
        for (j = 0; j < 4; j++)
        {
            xs = 8 + (i * 13);
            ys = 8 + (j * 14);
            x1 = xs + random2avg(5, 2);
            y1 = ys + random2avg(5, 2);
            x2 = xs + 11 - random2avg(5, 2);
            y2 = ys + 11 - random2avg(5, 2);

            temp_rand = random2(280);

            if (temp_rand > 39) // 85.7% draw room(s) {dlb}
            {
                wall_type_room = ((temp_rand > 63) ? wall_type :       // 77.1%
                                  (temp_rand > 54) ? DNGN_STONE_WALL : //  3.2%
                                  (temp_rand > 45) ? DNGN_ROCK_WALL    //  3.2%
                                                   : DNGN_METAL_WALL); //  2.1%

                box_room(x1, x2, y1, y2, wall_type_room);

                // inner room - neat.
                if (x2 - x1 > 5 && y2 - y1 > 5 && one_chance_in(8))
                {
                    box_room(x1 + 2, x2 - 2, y1 + 2, y2 - 2, wall_type);

                    // treasure area.. neat.
                    if (one_chance_in(3))
                        treasure_area(level_number, x1 + 3, x2 - 3, y1 + 3, y2 - 3);
                }
            }
        }
    }

    int stair_count = coinflip()?2:1;

    for (j = 0; j < stair_count; j++)
        for (i = 0; i < 2; i++)
            place_specific_stair(j + ((i==0)?DNGN_STONE_STAIRS_DOWN_I
                                            :DNGN_STONE_STAIRS_UP_I));

}                               // end city_level()

static bool treasure_area(int level_number, unsigned char ta1_x,
                          unsigned char ta2_x, unsigned char ta1_y,
                          unsigned char ta2_y)
{
    int x_count = 0;
    int y_count = 0;
    int item_made = 0;

    ta2_x++;
    ta2_y++;

    if (ta2_x <= ta1_x || ta2_y <= ta1_y)
        return false;

    if ((ta2_x - ta1_x) * (ta2_y - ta1_y) >= 40)
        return false;

    for (x_count = ta1_x; x_count < ta2_x; x_count++)
    {
        for (y_count = ta1_y; y_count < ta2_y; y_count++)
        {
            if (grd[x_count][y_count] != DNGN_FLOOR || coinflip())
                continue;

            item_made = items( 1, OBJ_RANDOM, OBJ_RANDOM, 1,
                                            random2( level_number * 2 ), 250 );
            mitm.x[item_made] = x_count;
            mitm.y[item_made] = y_count;
        }
    }

    return true;
}                               // end treasure_area()

static void diamond_rooms(int level_number)
{
    char numb_diam = 1 + random2(10);
    char type_floor = DNGN_DEEP_WATER;
    int runthru = 0;
    int i, oblique_max;

    // I guess no diamond rooms in either of these places {dlb}:
    if (you.where_are_you == BRANCH_DIS
        || you.where_are_you == BRANCH_TARTARUS)
    {
        return;
    }

    if (level_number > 5 + random2(5) && coinflip())
        type_floor = DNGN_SHALLOW_WATER;

    if (level_number > 10 + random2(5) && coinflip())
        type_floor = DNGN_DEEP_WATER;

    if (level_number > 17 && coinflip())
        type_floor = DNGN_LAVA;

    if (level_number > 10 && one_chance_in(15))
        type_floor = (coinflip()? DNGN_STONE_WALL : DNGN_ROCK_WALL);

    if (level_number > 12 && one_chance_in(20))
        type_floor = DNGN_METAL_WALL;

    if (you.where_are_you == BRANCH_GEHENNA)
        type_floor = DNGN_LAVA;
    else if (you.where_are_you == BRANCH_COCYTUS)
        type_floor = DNGN_DEEP_WATER;

    for (i = 0; i < numb_diam; i++)
    {
        spec_room sr = { false, false, 0, 0, 0, 0 };

        sr.x1 = 8 + random2(43);
        sr.y1 = 8 + random2(35);
        sr.x2 = sr.x1 + 5 + random2(15);
        sr.y2 = sr.y1 + 5 + random2(10);

        oblique_max = (sr.x2 - sr.x1) / 2;      //random2(20) + 5;
        int oblique = oblique_max;

        if (!octa_room(sr, oblique_max, type_floor))
        {
            runthru++;
            if (runthru > 9)
            {
                runthru = 0;
            }
            else
            {
                i--;
                continue;
            }
        }
    }                           // end "for(bk...)"
}                               // end diamond_rooms()

static void big_room(int level_number)
{
    unsigned char type_floor = DNGN_FLOOR;
    unsigned char type_2 = DNGN_FLOOR;
    int i, j, k, l;

    spec_room sr = { false, false, 0, 0, 0, 0 };
    int oblique;

    if (one_chance_in(4))
    {
        oblique = 5 + random2(20);

        sr.x1 = 8 + random2(30);
        sr.y1 = 8 + random2(22);
        sr.x2 = sr.x1 + 20 + random2(10);
        sr.y2 = sr.y1 + 20 + random2(8);

        // usually floor, except at higher levels
        if (!one_chance_in(5) || level_number < 8 + random2(8))
        {
            octa_room(sr, oblique, DNGN_FLOOR);
            return;
        }

        // default is lava.
        type_floor = DNGN_LAVA;

        if (level_number > 7)
            type_floor = ((random2(level_number) < 14) ? DNGN_DEEP_WATER
                                                    : DNGN_LAVA);

        octa_room(sr, oblique, type_floor);
    }

    // what now?
    sr.x1 = 8 + random2(30);
    sr.y1 = 8 + random2(22);
    sr.x2 = sr.x1 + 20 + random2(10);
    sr.y2 = sr.y1 + 20 + random2(8);

    // check for previous special
    if (find_in_area(sr.x1, sr.y1, sr.x2, sr.y2, DNGN_BUILDER_SPECIAL_WALL))
        return;

    if (level_number > 7 && one_chance_in(4))
    {
        type_floor = ((random2(level_number) < 14) ? DNGN_DEEP_WATER
                                                : DNGN_LAVA);
    }

    // make the big room.
    replace_area(sr.x1, sr.y1, sr.x2, sr.y2, DNGN_ROCK_WALL, type_floor);
    replace_area(sr.x1, sr.y1, sr.x2, sr.y2, DNGN_CLOSED_DOOR, type_floor);

    if (type_floor == DNGN_FLOOR)
        type_2 = DNGN_ROCK_WALL + random2(4);

    // no lava in the Crypt or Tomb, thanks!
    if (you.where_are_you == BRANCH_CRYPT || you.where_are_you == BRANCH_TOMB)
    {
        if (type_floor == DNGN_LAVA)
            type_floor = DNGN_SHALLOW_WATER;
        if (type_2 == DNGN_LAVA)
            type_2 = DNGN_SHALLOW_WATER;
    }

    // sometimes make it a chequerboard
    if (one_chance_in(4))
    {
        chequerboard( sr, type_floor, type_floor, type_2 );
    }
    // sometimes make an inside room w/ stone wall.
    else if (one_chance_in(6))
    {
        i = sr.x1;
        j = sr.y1;
        k = sr.x2;
        l = sr.y2;

        do
        {
            i += 2 + random2(3);
            j += 2 + random2(3);
            k -= 2 + random2(3);
            l -= 2 + random2(3);
            // check for too small
            if (i >= k - 3)
                break;
            if (j >= l - 3)
                break;

            box_room(i, k, j, l, DNGN_STONE_WALL);

        }
        while (level_number < 1500);       // ie forever
    }
}                               // end big_room()

// helper function for chequerboard rooms
// note that box boundaries are INclusive
static void chequerboard(spec_room &sr, unsigned char
    target,  unsigned char floor1, unsigned char floor2)
{
    int i, j;

    if (sr.x2 >= sr.x1 || sr.y2 >= sr.y1)
        return;

    for (i = sr.x1; i <= sr.x2; i++)
        for (j = sr.y1; j <= sr.y2; j++)
            if (grd[i][j] == target)
                grd[i][j] = (((i + j) % 2) ? floor2 : floor1);
}                               // end chequerboard()

static void roguey_level(int level_number, spec_room &sr)
{
    int bcount_x, bcount_y;
    int cn = 0;
    int i;

    FixedVector < unsigned char, 30 > rox1;
    FixedVector < unsigned char, 30 > rox2;
    FixedVector < unsigned char, 30 > roy1;
    FixedVector < unsigned char, 30 > roy2;

    for (bcount_y = 0; bcount_y < 5; bcount_y++)
    {
        for (bcount_x = 0; bcount_x < 5; bcount_x++)
        {
            // rooms:
            rox1[cn] = bcount_x * 13 + 8 + random2(4);
            roy1[cn] = bcount_y * 11 + 8 + random2(4);

            rox2[cn] = rox1[cn] + 3 + random2(8);
            roy2[cn] = roy1[cn] + 3 + random2(6);

            // bounds
            if (rox2[cn] > GXM-8)
                rox2[cn] = GXM-8;

            cn++;
        }
    }

    cn = 0;

    for (i = 0; i < 25; i++)
    {
        replace_area(rox1[i], roy1[i], rox2[i], roy2[i], DNGN_ROCK_WALL,
            DNGN_FLOOR);

        // inner room?
        if (rox2[i] - rox1[i] > 5 && roy2[i] - roy1[i] > 5)
        {
            if (random2(100 - level_number) < 3)
            {
                if (!one_chance_in(4))
                {
                    box_room( rox1[i] + 2, rox2[i] - 2, roy1[i] + 2,
                                roy2[i] - 2, (coinflip() ? DNGN_STONE_WALL
                                                          : DNGN_ROCK_WALL) );
                }
                else
                {
                    box_room( rox1[i] + 2, rox2[i] - 2, roy1[i] + 2,
                                 roy2[i] - 2, DNGN_METAL_WALL );
                }

                if (coinflip())
                {
                    treasure_area( level_number, rox1[i] + 3, rox2[i] - 3,
                                      roy1[i] + 3, roy2[i] - 3 );
                }
            }
        }
    }                           // end "for i"

    // Now, join them together:
    unsigned char pos_x, pos_y;
    FixedVector < char, 2 > pos;
    FixedVector < char, 2 > jpos;

    char doing = 0;

    char last_room = 0;
    int bp;

    for (bp = 0; bp < 2; bp++)
    {
        for (i = 0; i < 25; i++)
        {
            if (bp == 0 && (!(i % 5) || i == 0))
                continue;

            if (bp == 1 && i < 5)
                continue;

            switch (bp)
            {
            case 0:
                last_room = i - 1;
                pos[0] = rox1[i];      // - 1;
                pos[1] = roy1[i] + random2(roy2[i] - roy1[i]);
                jpos[0] = rox2[last_room];      // + 1;
                jpos[1] = roy1[last_room]
                                + random2(roy2[last_room] - roy1[last_room]);
                break;

            case 1:
                last_room = i - 5;
                pos[1] = roy1[i];      // - 1;
                pos[0] = rox1[i] + random2(rox2[i] - rox1[i]);
                jpos[1] = roy2[last_room];      // + 1;
                jpos[0] = rox1[last_room]
                                + random2(rox2[last_room] - rox1[last_room]);
                break;
            }

            while (pos[0] != jpos[0] || pos[1] != jpos[1])
            {
                doing = (coinflip()? 1 : 0);

                if (pos[doing] < jpos[doing])
                    pos[doing]++;
                else if (pos[doing] > jpos[doing])
                    pos[doing]--;

                if (grd[pos[0]][pos[1]] == DNGN_ROCK_WALL)
                    grd[pos[0]][pos[1]] = DNGN_FLOOR;
            }

            if (grd[pos[0]][pos[1]] == DNGN_FLOOR)
            {
                if ((grd[pos[0] + 1][pos[1]] == DNGN_ROCK_WALL
                        && grd[pos[0] - 1][pos[1]] == DNGN_ROCK_WALL)
                    || (grd[pos[0]][pos[1] + 1] == DNGN_ROCK_WALL
                        && grd[pos[0]][pos[1] - 1] == DNGN_ROCK_WALL))
                {
                    grd[pos[0]][pos[1]] = 103;
                }
            }
        }                       // end "for bp, for i"
    }

    // is one of them a special room?
    if (level_number > 8 && one_chance_in(10))
    {
        int spec_room_done = random2(25);

        sr.created = true;
        sr.hooked_up = true;
        sr.x1 = rox1[spec_room_done];
        sr.x2 = rox2[spec_room_done];
        sr.y1 = roy1[spec_room_done];
        sr.y2 = roy2[spec_room_done];
        morgue(sr);

        // make the room 'special' so it doesn't get overwritten
        // by something else (or put monsters in walls, etc..).

        // top
        replace_area(sr.x1-1, sr.y1-1, sr.x2+1,sr.y1-1, DNGN_ROCK_WALL, DNGN_BUILDER_SPECIAL_WALL);
        replace_area(sr.x1-1, sr.y1-1, sr.x2+1,sr.y1-1, DNGN_FLOOR, DNGN_BUILDER_SPECIAL_FLOOR);
        replace_area(sr.x1-1, sr.y1-1, sr.x2+1,sr.y1-1, DNGN_CLOSED_DOOR, DNGN_BUILDER_SPECIAL_FLOOR);

        // bottom
        replace_area(sr.x1-1, sr.y2+1, sr.x2+1,sr.y2+1, DNGN_ROCK_WALL, DNGN_BUILDER_SPECIAL_WALL);
        replace_area(sr.x1-1, sr.y2+1, sr.x2+1,sr.y2+1, DNGN_FLOOR, DNGN_BUILDER_SPECIAL_FLOOR);
        replace_area(sr.x1-1, sr.y2+1, sr.x2+1,sr.y2+1, DNGN_CLOSED_DOOR, DNGN_BUILDER_SPECIAL_FLOOR);

        // left
        replace_area(sr.x1-1, sr.y1-1, sr.x1-1, sr.y2+1, DNGN_ROCK_WALL, DNGN_BUILDER_SPECIAL_WALL);
        replace_area(sr.x1-1, sr.y1-1, sr.x1-1, sr.y2+1, DNGN_FLOOR, DNGN_BUILDER_SPECIAL_FLOOR);
        replace_area(sr.x1-1, sr.y1-1, sr.x1-1, sr.y2+1, DNGN_CLOSED_DOOR, DNGN_BUILDER_SPECIAL_FLOOR);

        // right
        replace_area(sr.x2+1, sr.y1-1, sr.x2+1, sr.y2+1, DNGN_ROCK_WALL, DNGN_BUILDER_SPECIAL_WALL);
        replace_area(sr.x2+1, sr.y1-1, sr.x2+1, sr.y2+1, DNGN_FLOOR, DNGN_BUILDER_SPECIAL_FLOOR);
        replace_area(sr.x2+1, sr.y1-1, sr.x2+1, sr.y2+1, DNGN_CLOSED_DOOR, DNGN_BUILDER_SPECIAL_FLOOR);
    }

    int stair_count = coinflip()?2:1;

    for (int j = 0; j < stair_count; j++)
        for (i = 0; i < 2; i++)
            place_specific_stair(j + ((i==0)?DNGN_STONE_STAIRS_DOWN_I
                                            :DNGN_STONE_STAIRS_UP_I));
}                               // end roguey_level()

static void morgue(spec_room &sr)
{
    int temp_rand = 0;          // probability determination {dlb}
    int x,y;

    for (x = sr.x1; x < sr.x2; x++)
    {
        for (y = sr.y1; y < sr.y2; y++)
        {
            if (grd[x][y] == DNGN_FLOOR || grd[x][y] == DNGN_BUILDER_SPECIAL_FLOOR)
            {
                int mon_type;
                temp_rand = random2(24);

                mon_type =  ((temp_rand > 11) ? MONS_ZOMBIE_SMALL :  // 50.0%
                             (temp_rand >  7) ? MONS_WIGHT :         // 16.7%
                             (temp_rand >  3) ? MONS_NECROPHAGE :    // 16.7%
                             (temp_rand >  0) ? MONS_WRAITH          // 12.5%
                                              : MONS_VAMPIRE);       //  4.2%

                mons_place(mon_type, BEH_SLEEP, MHITNOT, true, x, y);
            }
        }
    }
}                               // end morgue()

static bool place_specific_trap(unsigned char spec_x, unsigned char spec_y,
                                unsigned char spec_type)
{
    if (spec_type == TRAP_RANDOM)
        spec_type = random2(NUM_TRAPS);

    for (int tcount = 0; tcount < MAX_TRAPS; tcount++)
    {
        if (env.trap_type[tcount] == TRAP_UNASSIGNED)
        {
            env.trap_type[tcount] = spec_type;
            env.trap_x[tcount] = spec_x;
            env.trap_y[tcount] = spec_y;
            grd[spec_x][spec_y] = DNGN_UNDISCOVERED_TRAP;
            return true;
        }

        if (tcount >= MAX_TRAPS - 1)
            return false;
    }

    return false;
}                               // end place_specific_trap()

void define_zombie(int mid, int ztype, int cs)
{
    int mons_sec2 = 0;
    int zombie_size;

    // set size based on zombie class (cs)
    switch(cs)
    {
        case MONS_ZOMBIE_SMALL:
        case MONS_SIMULACRUM_SMALL:
        case MONS_SKELETON_SMALL:
            zombie_size = 1;
            break;

        case MONS_ZOMBIE_LARGE:
        case MONS_SIMULACRUM_LARGE:
        case MONS_SKELETON_LARGE:
            zombie_size = 2;
            break;

        case MONS_SPECTRAL_THING:
            zombie_size = -1;
            break;

        default:
            // this should NEVER happen.
            perror("\ncreate_zombie() got passed incorrect zombie type!\n");
            end(0);
            break;
    }

    int test, cls;
    // that is, random creature from which to fashion undead
    if (ztype == 250)
    {
        while(true)
        {
            // this limit can be updated if mons->number goes >8 bits..
            test = random2(182);            // not guaranteed to be valid!
            cls = mons_charclass(test);
            if (cls == MONS_PROGRAM_BUG)
                continue;

            // monster class must be zombifiable and match class size
            if (!mons_zombie_size(cls))
                continue;

            if ((cs == MONS_SKELETON_SMALL || cs == MONS_SKELETON_LARGE)
                && !mons_skeleton(cls))
                continue;

            // size must match
            if (mons_zombie_size(cls) == zombie_size)
                break;

            // can make a spectral thing out of anything, I think.
            if (zombie_size < 0)
                break;
        }

        // set type and secondary appropriately
        menv[mid].number = cls;
        mons_sec2 = cls;
    }
    else
    {
        menv[mid].number = mons_charclass(ztype);
        mons_sec2 = menv[mid].number;
    }

    menv[mid].type = menv[mid].number;

    define_monster(mid);

    menv[mid].hit_points = hit_points(menv[mid].hit_dice, 6, 5);
    menv[mid].max_hit_points = menv[mid].hit_points;

    menv[mid].armor_class -= 2;

    if (menv[mid].armor_class < 0)
        menv[mid].armor_class = 0;

    menv[mid].evasion -= 5;

    if (menv[mid].evasion < 0)
        menv[mid].evasion = 0;

    menv[mid].speed -= 2;

    if (menv[mid].speed < 3)
        menv[mid].speed = 3;

    menv[mid].speed_increment = 70;
    menv[mid].number = mons_sec2;

    if (cs == MONS_ZOMBIE_SMALL || cs == MONS_ZOMBIE_LARGE)
    {
        menv[mid].type = ((mons_zombie_size(menv[mid].number) == 2)
                                    ? MONS_ZOMBIE_LARGE : MONS_ZOMBIE_SMALL);
    }
    else if (cs == MONS_SKELETON_SMALL || cs == MONS_SKELETON_LARGE)
    {
        menv[mid].hit_points = hit_points(menv[mid].hit_dice, 5, 4);
        menv[mid].max_hit_points = menv[mid].hit_points;

        menv[mid].armor_class -= 4;

        if (menv[mid].armor_class < 0)
            menv[mid].armor_class = 0;

        menv[mid].evasion -= 2;

        if (menv[mid].evasion < 0)
            menv[mid].evasion = 0;

        menv[mid].type = ((mons_zombie_size(menv[mid].number) == 2)
                            ? MONS_SKELETON_LARGE : MONS_SKELETON_SMALL);
    }
    else if (cs == MONS_SIMULACRUM_SMALL || cs == MONS_SIMULACRUM_LARGE)
    {
        menv[mid].hit_points = hit_points(menv[mid].hit_dice, 4, 4);
        menv[mid].max_hit_points = menv[mid].hit_points;
        menv[mid].type = ((mons_zombie_size(menv[mid].number) == 2)
                            ? MONS_SIMULACRUM_LARGE : MONS_SIMULACRUM_SMALL);
    }
    else if (cs == MONS_SPECTRAL_THING)
    {
        menv[mid].hit_points = hit_points(menv[mid].hit_dice, 4, 4);
        menv[mid].max_hit_points = menv[mid].hit_points;
        menv[mid].armor_class += 4;
        menv[mid].type = MONS_SPECTRAL_THING;
    }

    menv[mid].number = mons_sec2;
}                               // end define_zombie()

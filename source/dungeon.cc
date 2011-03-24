/*
 *  File:       dungeon.cc
 *  Summary:    Functions used when building new levels.
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 *
 *   <9>     07-Aug-2001 MV     clean up of give_item; distribution of
 *                              wands, potions and scrolls
 *                              underground rivers and lakes
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

#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

#include "AppHdr.h"
#include "abyss.h"
#include "defines.h"
#include "enum.h"
#include "globals.h"
#include "externs.h"
#include "dungeon.h"
#include "itemname.h"
#include "itemprop.h"
#include "items.h"
#include "maps.h"
#include "misc.h"
#include "mon-util.h"
#include "mon-pick.h"
#include "monplace.h"
#include "player.h"
#include "randart.h"
#include "spl-book.h"
#include "stuff.h"

#define MAX_PIT_MONSTERS   10

struct pit_mons_def {
    int type;
    int rare;
};

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

static int builder_by_type( int level_number, int level_type );
static int builder_by_branch( int level_number );
static int builder_normal( int level_number, int level_type, spec_room &s );
static int builder_basic( int level_number );
static void builder_extras(int level_number, int level_type);
static void builder_items(int level_number, int level_type, int items_wanted);
static void builder_monsters(int level_number, int level_type, int mon_wanted);

static void place_specific_stair( int stair );
static void place_branch_entrances(int dlevel, int level_type);

static bool place_specific_trap( int spec_x, int spec_y, int spec_type,
                                 bool force = false );

static void place_traps( int level_number );
static void prepare_swamp(void);
static void prepare_water( int level_number );
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

#ifdef USE_RIVERS
static void build_river(unsigned char river_type); //mv
static void build_lake(unsigned char lake_type); //mv
#endif // USE_RIVERS

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
static int  box_room_doors( int bx1, int bx2, int by1, int by2, int new_doors);
static void city_level(int level_number);
static void diamond_rooms(int level_number);

// ITEM & SHOP FUNCTIONS
static void place_shops(int level_number);
static unsigned char item_in_shop(unsigned char shop_type);

static bool treasure_area(int level_number, unsigned char ta1_x,
                          unsigned char ta2_x, unsigned char ta1_y,
                          unsigned char ta2_y);

static void set_weapon_special(int the_weapon, int spwpn);
static void big_room(int level_number);

static bool build_maze( spec_room &sr, int wall, int straightness, bool secret_doors );
static void chequerboard( spec_room &sr, unsigned char floor, unsigned char wall );

static void roguey_level(int level_number, spec_room &sr);

// SPECIAL ROOM BUILDERS
static void special_room(int level_number, spec_room &sr);
static void specr_2(spec_room &sr);

// VAULT FUNCTIONS
static void build_vaults( int level_number, int force_vault,
                          bool down_stairs = true );

static void build_minivaults( int level_number, int force_vault );

static void vault_grid( int vault, int level_number, int vx, int vy,
                        int &altar_count,
                        const FixedVector < char, 7 > &acq_item_class,
                        const FixedVector < int, 7 > &mons_array,
                        char vgrid, int &initial_x, int &initial_y );

static void paint_vault( char vgrid[81][81],
                         int sx, int sy, int ex, int ey,
                         int level_number, int force_vault,
                         const FixedVector< char, 7 > &acq_item_class,
                         const FixedVector< int, 7 > &mons_array,
                         int &initial_x, int &initial_y );

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

void builder( int level_number, int level_type )
{
    int i;          // generic loop variable
    int x,y;        // generic map loop variables

    // blank level with DNGN_ROCK_WALL
    make_box( 0, 0, GXM - 1, GYM - 1, DNGN_ROCK_WALL, DNGN_ROCK_WALL );

    // delete all traps
    for (i = 0; i < MAX_TRAPS; i++)
        env.trap[i].type = TRAP_UNASSIGNED;

    // initialize all items
    for (i = 0; i < MAX_ITEMS; i++)
        init_item( mitm[i] );

    // reset all monsters
    for (i = 0; i < MAX_MONSTERS; i++)
        menv[i].type = -1;

    // unlink all monsters and items from the grid
    for (x = 0; x < GXM; x++)
    {
        for(y = 0; y < GYM; y++)
        {
            mgrd[x][y] = NON_MONSTER;
            igrd[x][y] = NON_ITEM;
        }
    }

    // reset all shops
    for (int shcount = 0; shcount < MAX_SHOPS; shcount++)
        env.shop[shcount].type = SHOP_UNASSIGNED;

    int skip_build;

    skip_build = builder_by_type( level_number, level_type );
    if (skip_build < 0)
        return;

    if (skip_build == 0)
    {
        skip_build = builder_by_branch( level_number );

        if (skip_build < 0)
            return;
    }

    spec_room sr = { false, false, 0, 0, 0, 0 };

    if (skip_build == 0)
    {
        // do 'normal' building.  Well, except for the swamp.
        if (!player_in_branch( BRANCH_SWAMP ))
            skip_build = builder_normal(level_number, level_type, sr);

        if (skip_build == 0)
        {
            skip_build = builder_basic( level_number );

            if (skip_build == 0)
                builder_extras( level_number, level_type );
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
    if (player_in_branch( BRANCH_SWAMP ))
        prepare_swamp();

    // figure out how many 'normal' monsters we should place
    int mon_wanted = 0;

    if (level_type == LEVEL_ABYSS
        || player_in_branch( BRANCH_ECUMENICAL_TEMPLE ))
    {
        mon_wanted = 0;
    }
    else
    {
        mon_wanted = roll_dice( 3, 10 );

        if (player_in_hell())
            mon_wanted += roll_dice( 3, 8 );
        else if (player_in_branch( BRANCH_HALL_OF_BLADES ))
            mon_wanted += roll_dice( 6, 8 );

        // unlikely - now only possible in HoB {dlb} 10mar2000
        if (mon_wanted > 60)
            mon_wanted = 60;
    }

    place_branch_entrances( level_number, level_type );

    check_doors();
    hide_doors();

    int items_wanted = 3 + roll_dice(3, 9);

    if (level_number > 5 && one_chance_in(500 - 5 * level_number))
        items_wanted += roll_dice(3, 9);  // rich level!

    // change pre-rock (105) to rock,  and pre-floor (106) to floor
    replace_area( 0, 0, GXM-1, GYM-1, DNGN_BUILDER_SPECIAL_WALL, DNGN_ROCK_WALL );
    replace_area( 0, 0, GXM-1, GYM-1, DNGN_BUILDER_SPECIAL_FLOOR, DNGN_FLOOR );

    // place items
    builder_items( level_number, level_type, items_wanted );

    // place traps
    if (!player_in_branch( BRANCH_ECUMENICAL_TEMPLE ))
        place_traps( level_number );

    // place monsters
    builder_monsters( level_number, level_type, mon_wanted );

    // place shops,  if appropriate
    if (player_in_branch( BRANCH_MAIN_DUNGEON )
         || player_in_branch( BRANCH_ORCISH_MINES )
         || player_in_branch( BRANCH_ELVEN_HALLS )
         || player_in_branch( BRANCH_LAIR )
         || player_in_branch( BRANCH_VAULTS )
         || player_in_branch( BRANCH_SNAKE_PIT )
         || player_in_branch( BRANCH_SWAMP ))
    {
        place_shops(level_number);
    }

    // If level part of Dis -> all walls metal;
    // If part of vaults -> walls depend on level;
    // If part of crypt -> all walls stone:
    if (player_in_branch( BRANCH_DIS )
        || player_in_branch( BRANCH_VAULTS )
        || player_in_branch( BRANCH_CRYPT ))
    {
        // always the case with Dis {dlb}
        unsigned char vault_wall = DNGN_METAL_WALL;

        if (player_in_branch( BRANCH_VAULTS ))
        {
            vault_wall = DNGN_ROCK_WALL;

            if (level_number > you.branch_stairs[STAIRS_VAULTS] + 2)
                vault_wall = DNGN_STONE_WALL;

            if (level_number > you.branch_stairs[STAIRS_VAULTS] + 4)
                vault_wall = DNGN_METAL_WALL;

            if (level_number > you.branch_stairs[STAIRS_VAULTS] + 6
                && one_chance_in(10))
            {
                vault_wall = DNGN_GREEN_CRYSTAL_WALL;
            }
        }
        else if (player_in_branch( BRANCH_CRYPT ))
        {
            vault_wall = DNGN_STONE_WALL;
        }

        replace_area( 0, 0, GXM-1, GYM-1, DNGN_ROCK_WALL, vault_wall );
    }

    // Top level of branch levels - replaces up stairs
    // with stairs back to dungeon or wherever:
    for (i = 0; i < 30; i++)
    {
        if (you.branch_stairs[i] == 0)
            break;

        if (level_number == you.branch_stairs[i] + 1
            && level_type == LEVEL_DUNGEON
            && you.where_are_you == BRANCH_ORCISH_MINES + i)
        {
            for (x = X_BOUND_1 + 1; x < X_BOUND_2; x++)
            {
                for (y = Y_BOUND_1 + 1; y < Y_BOUND_2; y++)
                {
                    if (grd[x][y] >= DNGN_STONE_STAIRS_UP_I
                        && grd[x][y] <= DNGN_ROCK_STAIRS_UP)
                    {
                        grd[x][y] = DNGN_RETURN_FROM_ORCISH_MINES + i;
                    }
                }
            }
        }
    }

    // bottom level of branch - replaces down stairs with floor
    for (i = 0; i < MAX_BRANCHES; i++)
    {
        if (level_number == you.branch_stairs[i] + branch_depth(i)
            && level_type == LEVEL_DUNGEON
            && you.where_are_you == BRANCH_ORCISH_MINES + i)
        {
            for (x = X_BOUND_1 + 1; x < X_BOUND_2; x++)
            {
                for (y = Y_BOUND_1 + 1; y < Y_BOUND_2; y++)
                {
                    if (grd[x][y] >= DNGN_STONE_STAIRS_DOWN_I
                        && grd[x][y] <= DNGN_ROCK_STAIRS_DOWN)
                    {
                        grd[x][y] = DNGN_FLOOR;
                    }
                }
            }
        }
    }

    if (player_in_branch( BRANCH_CRYPT ))
    {
        if (one_chance_in(3))
            mons_place( MONS_CURSE_SKULL, BEH_SLEEP, MHITNOT, false, 0, 0 );

        if (one_chance_in(7))
            mons_place( MONS_CURSE_SKULL, BEH_SLEEP, MHITNOT, false, 0, 0 );
    }

    if (player_in_branch( BRANCH_ORCISH_MINES ) && one_chance_in(5))
        place_altar();

    // hall of blades (1 level deal) - no down staircases, thanks!
    if (player_in_branch( BRANCH_HALL_OF_BLADES ))
    {
        for (x = X_BOUND_1 + 1; x < X_BOUND_2; x++)
        {
            for (y = Y_BOUND_1 + 1; y < Y_BOUND_2; y++)
            {
                if (grd[x][y] >= DNGN_STONE_STAIRS_DOWN_I
                    && grd[x][y] <= DNGN_ROCK_STAIRS_UP)
                {
                    grd[x][y] = DNGN_FLOOR;
                }
            }
        }
    }

    link_items();

    if (you.level_type == LEVEL_DUNGEON
        && !player_in_branch(BRANCH_COCYTUS)
        && !player_in_branch(BRANCH_SWAMP))
    {
        prepare_water( level_number );
    }
}                               // end builder()

// Returns item slot or NON_ITEM if it fails
int make_item( int allow_uniques,       // not just true-false,
                                        //     because of BCR acquirement hack
               int force_class,         // desired OBJECTS class {dlb}
               int force_type,          // desired SUBTYPE - enum varies by OBJ
               bool dont_place,         // don't randomly place item on level
               int item_level,          // level of the item, can differ from global
               int item_race )          // weapon / armour racial categories
                                        // item_race also gives type of rune!
{
    int temp_rand = 0;             // probability determination {dlb}
    int temp_value = 0;            // temporary value storage {dlb}
    int loopy = 0;                 // just another loop variable {dlb}
    int count = 0;                 // just another loop variable {dlb}
    int pluses;                    // number of pluses on rings

    unsigned long race;
    int race_plus = 0;
    int race_plus2 = 0;
    int x_pos, y_pos;

    FixedVector < int, SPELLBOOK_SIZE > fpass;
    int icky = 0;
    int p = 0;

    // find an emtpy slot for the item (with culling if required)
    p = get_item_slot(10);
    if (p == NON_ITEM)
        return (NON_ITEM);

    // clear all properties except mitm.base_type <used in switch below> {dlb}:
    mitm[p].sub_type = 0;
    mitm[p].flags = 0;
    mitm[p].special = 0;
    mitm[p].plus = 0;
    mitm[p].plus2 = 0;
    mitm[p].x = 0;
    mitm[p].y = 0;
    mitm[p].link = NON_ITEM;
    mitm[p].slot = -1;

    // first store if we passed in a special request:
    const bool good_item  = (item_level == MAKE_GOOD_ITEM);
    const bool abyss_item = (you.level_type == LEVEL_ABYSS); // track artefacts

    // now cap the item (note: the above special requests get capped here too)
    if (item_level > MAX_ITEM_CREATE_LEVEL)
        item_level = MAX_ITEM_CREATE_LEVEL;

    // determine base_type for item generated {dlb}:
    if (force_class != OBJ_RANDOM)
        mitm[p].base_type = force_class;
    else
    {
        // nice and large for subtle differences {dlb}
        temp_rand = random2(1000);

        mitm[p].base_type =  ((temp_rand <   5) ? OBJ_STAVES :   //  0.50%
                              (temp_rand <  20) ? OBJ_BOOKS :    //  1.50%
                              (temp_rand <  45) ? OBJ_JEWELLERY ://  2.50%
                              (temp_rand <  80) ? OBJ_WANDS :    //  3.50%
                              (temp_rand < 150) ? OBJ_FOOD :     //  7.00%
                              (temp_rand < 250) ? OBJ_ARMOUR :   // 10.00%
                              (temp_rand < 350) ? OBJ_WEAPONS :  // 10.00%
                              (temp_rand < 450) ? OBJ_POTIONS :  // 10.00%
                              (temp_rand < 600) ? OBJ_MISSILES : // 15.00%
                              (temp_rand < 800) ? OBJ_SCROLLS    // 20.00%
                                                : OBJ_GOLD);     // 20.00%

        // misc items placement wholly dependent upon current depth {dlb}:
        if (item_level > 7 && (20 + item_level) >= random2(3500))
            mitm[p].base_type = OBJ_MISCELLANY;

        if (item_level < 7
            && (mitm[p].base_type == OBJ_BOOKS
                || mitm[p].base_type == OBJ_STAVES
                || mitm[p].base_type == OBJ_WANDS)
            && random2(7) >= item_level)
        {
            mitm[p].base_type = coinflip() ? OBJ_POTIONS : OBJ_SCROLLS;
        }
    }

    // Assume quantity of one, and adjust otherwise.
    mitm[p].quantity = 1;
    mitm[p].plus = 0;
    mitm[p].plus2 = 0;
    mitm[p].special = 0;

    // determine sub_type accordingly {dlb}:
    switch (mitm[p].base_type)
    {
    case OBJ_WEAPONS:
        // [0] initialize:
        mitm[p].special = SPWPN_NORMAL;

        // [1] establish the type of weapon:
        if (force_type != OBJ_RANDOM)
            mitm[p].sub_type = force_type;
        else
        {
            if (random2(20) < 20 - item_level - 5 * good_item)
            {
                // these are the common/low level weapon types
                temp_rand = random2(12);

                mitm[p].sub_type = ((temp_rand ==  0) ? WPN_KNIFE         :
                                    (temp_rand ==  1) ? WPN_STAFF         :
                                    (temp_rand ==  2) ? WPN_SLING         :
                                    (temp_rand ==  3) ? WPN_SPEAR         :
                                    (temp_rand ==  4) ? WPN_HAND_AXE      :
                                    (temp_rand ==  5) ? WPN_DAGGER        :
                                    (temp_rand ==  6) ? WPN_MACE          :
                                    (temp_rand ==  7) ? WPN_DAGGER        :
                                    (temp_rand ==  8) ? WPN_CLUB          :
                                    (temp_rand ==  9) ? WPN_HAMMER        :
                                    (temp_rand == 10) ? WPN_WHIP
                                                      : WPN_SABRE);
            }
            else if (item_level > 6
                    && random2(3000) < (10 + item_level) * (good_item + 1))
            {
                // place the weapon_rarity() == 0 weapons
                //
                // this replaced the infinite loop (wasteful) -- may need
                // to make into its own function to allow ease of tweaking
                // distribution {dlb}:
                temp_rand = random2(10);

                mitm[p].sub_type = ((temp_rand == 9) ? WPN_LAJATANG :
                                    (temp_rand == 8) ? WPN_DEMON_BLADE :
                                    (temp_rand == 7) ? WPN_DEMON_TRIDENT :
                                    (temp_rand == 6) ? WPN_DEMON_WHIP :
                                    (temp_rand == 5) ? WPN_DOUBLE_SWORD :
                                    (temp_rand == 4) ? WPN_EVENINGSTAR :
                                    (temp_rand == 3) ? WPN_EXECUTIONERS_AXE :
                                    (temp_rand == 2) ? WPN_KATANA :
                                    (temp_rand == 1) ? WPN_QUICK_BLADE
                                 /*(temp_rand == 0)*/: WPN_TRIPLE_SWORD);
            }
            else
            {
                // pick a weapon based on rarity
                for (;;)
                {
                    temp_value = random2( NUM_WEAPONS );

                    if (weapon_rarity(temp_value) > random2(10))
                    {
                        mitm[p].sub_type = temp_value;
                        break;
                    }
                }
            }
        }

        // [2] determine weapon race, and apply bonus
        switch (item_race)
        {
        case MAKE_ITEM_ELVEN:
            set_equip_race( mitm[p], ISFLAG_ELVEN );
            break;

        case MAKE_ITEM_DWARVEN:
            set_equip_race( mitm[p], ISFLAG_DWARVEN );
            break;

        case MAKE_ITEM_ORCISH:
            set_equip_race( mitm[p], ISFLAG_ORCISH );
            break;

        case MAKE_ITEM_RANDOM_RACE:
            switch (mitm[p].sub_type)
            {
            case WPN_CLUB:
                set_equip_race( mitm[p], one_chance_in(4) ? ISFLAG_ORCISH
                                                          : ISFLAG_NO_RACE );
                break;

            case WPN_MACE:
            case WPN_GREAT_MACE:
            case WPN_FLAIL:
            case WPN_SPIKED_FLAIL:
            case WPN_DIRE_FLAIL:
                set_equip_race( mitm[p], (temp_rand < 133) ? ISFLAG_ORCISH :
                                         (temp_rand < 267) ? ISFLAG_DWARVEN :
                                         (temp_rand < 333) ? ISFLAG_ELVEN
                                                           : ISFLAG_NO_RACE );
                break;

            case WPN_MORNINGSTAR:
            case WPN_HAMMER:
                temp_rand = random2(1000);

                set_equip_race( mitm[p], (temp_rand < 167) ? ISFLAG_DWARVEN :
                                         (temp_rand < 300) ? ISFLAG_ORCISH
                                                           : ISFLAG_NO_RACE );
                break;

            case WPN_DAGGER:
                temp_rand = random2(1000);

                set_equip_race( mitm[p], (temp_rand < 125) ? ISFLAG_ELVEN :
                                         (temp_rand < 225) ? ISFLAG_DWARVEN :
                                         (temp_rand < 350) ? ISFLAG_ORCISH
                                                           : ISFLAG_NO_RACE );
                break;

            case WPN_SHORT_SWORD:
                temp_rand = random2(1000);

                set_equip_race( mitm[p], (temp_rand < 167) ? ISFLAG_ELVEN :
                                         (temp_rand < 300) ? ISFLAG_DWARVEN :
                                         (temp_rand < 400) ? ISFLAG_ORCISH
                                                           : ISFLAG_NO_RACE );
                break;

            case WPN_FALCHION:
                temp_rand = random2(1000);

                set_equip_race( mitm[p], (temp_rand < 167) ? ISFLAG_ELVEN :
                                         (temp_rand < 300) ? ISFLAG_ORCISH :
                                         (temp_rand < 375) ? ISFLAG_DWARVEN
                                                           : ISFLAG_NO_RACE );
                break;

            case WPN_LONG_SWORD:
                temp_rand = random2(1000);

                set_equip_race( mitm[p], (temp_rand < 250) ? ISFLAG_ELVEN :
                                         (temp_rand < 344) ? ISFLAG_ORCISH
                                                           : ISFLAG_NO_RACE );
                break;

            case WPN_GREAT_SWORD:
                set_equip_race( mitm[p], one_chance_in(6) ? ISFLAG_ORCISH
                                                          : ISFLAG_NO_RACE );
                break;

            case WPN_SCIMITAR:
                set_equip_race( mitm[p], one_chance_in(4) ? ISFLAG_ORCISH
                                                          : ISFLAG_NO_RACE );
                break;

            case WPN_WAR_AXE:
            case WPN_HAND_AXE:
            case WPN_BROAD_AXE:
            case WPN_BATTLEAXE:
                temp_rand = random2(1000);

                set_equip_race( mitm[p], (temp_rand < 250) ? ISFLAG_DWARVEN :
                                         (temp_rand < 375) ? ISFLAG_ORCISH
                                                           : ISFLAG_NO_RACE );
                break;

            case WPN_SPEAR:
            case WPN_TRIDENT:
                temp_rand = random2(1000);

                set_equip_race( mitm[p], (temp_rand < 125) ? ISFLAG_ELVEN :
                                         (temp_rand < 235) ? ISFLAG_ORCISH
                                                           : ISFLAG_NO_RACE );
                break;

            case WPN_HALBERD:
            case WPN_GLAIVE:
            case WPN_EXECUTIONERS_AXE:
            case WPN_LOCHABER_AXE:
                set_equip_race( mitm[p], one_chance_in(10) ? ISFLAG_ORCISH
                                                           : ISFLAG_NO_RACE );
                break;

            case WPN_QUICK_BLADE:
                set_equip_race( mitm[p], one_chance_in(8) ? ISFLAG_ELVEN
                                                          : ISFLAG_NO_RACE );
                break;

            case WPN_BOW:
                temp_rand = random2(1000);

                set_equip_race( mitm[p], (temp_rand < 250) ? ISFLAG_ELVEN :
                                         (temp_rand < 313) ? ISFLAG_ORCISH
                                                           : ISFLAG_NO_RACE );
                break;

            case WPN_LONGBOW:
                set_equip_race( mitm[p], one_chance_in(3)  ? ISFLAG_ELVEN
                                                           : ISFLAG_NO_RACE );
                break;

            case WPN_CROSSBOW:
                temp_rand = random2(1000);

                set_equip_race( mitm[p], (temp_rand < 167) ? ISFLAG_DWARVEN :
                                         (temp_rand < 236) ? ISFLAG_ORCISH
                                                           : ISFLAG_NO_RACE );
                break;

            case WPN_HAND_CROSSBOW:
                set_equip_race( mitm[p], one_chance_in(6) ? ISFLAG_ELVEN
                                                          : ISFLAG_NO_RACE );
                break;

            case WPN_BLOWGUN:
                temp_rand = random2(1000);

                set_equip_race( mitm[p], (temp_rand < 125) ? ISFLAG_ORCISH :
                                         (temp_rand < 169) ? ISFLAG_ELVEN
                                                           : ISFLAG_NO_RACE );
                break;

            case WPN_KATANA:
            case WPN_LAJATANG:
            case WPN_KNIFE:
            case WPN_SLING:
            case WPN_BLESSED_BLADE:
            case WPN_DEMON_TRIDENT:
            case WPN_DEMON_WHIP:
            case WPN_DEMON_BLADE:
            default:
                // no particular race associated with these items
                break;
            }
            break;
        }

        switch (get_equip_race( mitm[p] ))
        {
        case ISFLAG_ORCISH:
            if (coinflip())
                race_plus--;
            if (coinflip())
                race_plus2++;
            break;

        case ISFLAG_ELVEN:
            race_plus += random2(3);
            break;

        case ISFLAG_DWARVEN:
            if (coinflip())
                race_plus++;
            if (coinflip())
                race_plus2++;
            break;
        }

        mitm[p].plus  = race_plus;
        mitm[p].plus2 = race_plus2;

        // [3] establish the pluses (and possible curse) on the weapon
        //
        // Knives and clubs are rarely enchanted.
        if ((mitm[p].sub_type != WPN_KNIFE
                && mitm[p].sub_type != WPN_CLUB
                && mitm[p].sub_type != WPN_GIANT_CLUB
                && mitm[p].sub_type != WPN_GIANT_SPIKED_CLUB)
            || (item_level > 5 && one_chance_in(20))) // well, sometimes
        {
            // pluses
            if (random2(200) <= 20 + item_level * 3
                                   + 20 * good_item
                                   + 20 * is_demonic( mitm[p] ))
            {
                const int chance = 20 + item_level * 2 + 20 * good_item;

                if (!one_chance_in(10))
                {
                    for (;;)
                    {
                        mitm[p].plus++;

                        if (random2(300) > chance
                            || (mitm[p].plus >= MAX_WPN_ENCHANT
                                && !one_chance_in(20)))
                        {
                            break;
                        }
                    }
                }

                if (mitm[p].plus == race_plus || !one_chance_in(9))
                {
                    for (;;)
                    {
                        mitm[p].plus2++;

                        if (random2(300) > chance
                            || (mitm[p].plus2 >= MAX_WPN_ENCHANT
                                && !one_chance_in(20)))
                        {
                            break;
                        }
                    }
                }
            }

            // curses
            if (!good_item && one_chance_in(10))
            {
                // set curse flag for use later (see ego)
                do_curse_item( mitm[p] );

                // This code negates at least one of the stats.
                // Distrobution: 90% both, 5% to-hit, 5% to-dam...
                // some acceptable deviation when + to-hit is 0.
                // First: 95% chance of to-hit.
                if (!one_chance_in(20))
                    mitm[p].plus  = race_plus - mitm[p].plus  - random2(3);

                // Second: 1/20 * 1 + 19/20 * 18/19 = 95% chance of to-dam.
                if (mitm[p].plus > race_plus || !one_chance_in(19))
                    mitm[p].plus2 = race_plus2 - mitm[p].plus2 - random2(3);
            }
        }

        // [4] make artefacts?  if we do, we must break!
        if (allow_uniques
            && item_level > 4
            && random2(10000) <= (100 + item_level * 3) * (good_item + 1))
        {
            // fixed artefacts: this will clobber early work (new type, pluses)
            if (force_class == OBJ_RANDOM
                && force_type == OBJ_RANDOM
                && one_chance_in(20))
            {
                if (make_item_fixed_artefact( mitm[p], abyss_item ))
                    break;
            }

            // unrandarts: this may clobber early work (new type if random)
            if (you.level_type != LEVEL_ABYSS
                && you.level_type != LEVEL_PANDEMONIUM
                && one_chance_in(30))
            {
                icky = find_okay_unrandart( OBJ_WEAPONS, force_type );

                if (icky != -1)
                {
                    make_item_unrandart( mitm[p], icky );
                    break;
                }
            }

            // randarts: this will use our early work to make an artefact
            //
            // Note that there is nothing to stop randarts being
            // reproduced, except vast improbability.
            //
            // Knives and clubs are rarely enchanted.
            if ((mitm[p].sub_type != WPN_KNIFE
                    && mitm[p].sub_type != WPN_CLUB
                    && mitm[p].sub_type != WPN_GIANT_CLUB
                    && mitm[p].sub_type != WPN_GIANT_SPIKED_CLUB)
                || (item_level > 4 && one_chance_in(20)))
            {
                // Sometimes we make pluses a bit more extreme on artefacts
                // Remember: higher pluses tend to result in weaker artefacts,
                // so we don't want to do this too much.
                if (one_chance_in(5))
                {
                    if (mitm[p].plus >= 0)
                        mitm[p].plus += (coinflip() ? 1 : 2);
                    else
                        mitm[p].plus -= (coinflip() ? 1 : 2);
                }

                if (one_chance_in(5))
                {
                    if (mitm[p].plus2 >= 0)
                        mitm[p].plus2 += (coinflip() ? 1 : 2);
                    else
                        mitm[p].plus2 -= (coinflip() ? 1 : 2);
                }

                // needs to come after pluses are decided:
                if (make_item_randart(mitm[p]) && mitm[p].ra_props[RAP_CURSED])
                    do_curse_item( mitm[p] );

                break;
            }
        }

        ASSERT( !is_fixed_artefact(mitm[p]) && !is_random_artefact(mitm[p]) );

        // [5] make ego if we didn't make an artefact
        //
        // Must meet random chance OR good OR demonic.
        // Then, if we cursed the item above (in pluses), skip ego most times.
        // Also, Orcish weapons aren't ego half the time (unless good).
        if ((random2(600) <= 30 + item_level * 3
                || (good_item && !one_chance_in(30))
                || is_demonic( mitm[p] ))
            && (!item_cursed( mitm[p] ) || one_chance_in(10))
            && (get_equip_race( mitm[p] ) != ISFLAG_ORCISH
                || good_item
                || coinflip()))
        {
            int tmp;

            // Note: This still doesn't guarantee special enchantment...
            // Some weapons don't get ego types, others might pass
            // on assigning one.  This is approximately what the
            // old clobber-through used to do, only the percentages
            // are more easily seen (and it guarantees setting
            // more absolutely... the old code tried 5 times with
            // about 80 +/- 7.5% chance each, depending on weapon type).
            switch (mitm[p].sub_type)
            {
            case WPN_MACE:
            case WPN_GREAT_MACE:
                if (random2(1000) < 70)
                {
                    set_weapon_special( p, SPWPN_DISRUPTION );
                    break;
                }
                // intentional fall-through
            case WPN_DIRE_FLAIL:
            case WPN_EVENINGSTAR:
            case WPN_FLAIL:
            case WPN_HAMMER:
            case WPN_MORNINGSTAR:
            case WPN_SPIKED_FLAIL:
                tmp = random2( (mitm[p].sub_type != WPN_EVENINGSTAR
                                && mitm[p].sub_type != WPN_SPIKED_FLAIL
                                && mitm[p].sub_type != WPN_DIRE_FLAIL)
                                                        ? 870 : 1000 );

                set_weapon_special( p, (tmp <  50) ? SPWPN_ELECTROCUTION :
                                       (tmp < 120) ? SPWPN_ORC_SLAYING :
                                       (tmp < 460) ? SPWPN_PROTECTION :
                                       (tmp < 630) ? SPWPN_HOLY_WRATH :
                                       (tmp < 800) ? SPWPN_VORPAL :
                                       (tmp < 830) ? SPWPN_FLAMING :
                                       (tmp < 860) ? SPWPN_FREEZING :
                                       (tmp < 870) ? SPWPN_DISTORTION :
                                       // flail, morningstar, hammer
                                       (tmp < 885) ? SPWPN_PAIN :
                                       (tmp < 900) ? SPWPN_VAMPIRICISM :
                                       (tmp < 950) ? SPWPN_VENOM
                                                   : SPWPN_DRAINING );
                break;


            case WPN_DAGGER:
            case WPN_SHORT_SWORD:
            case WPN_SABRE:
                tmp = random2( (mitm[p].sub_type != WPN_DAGGER) ? 850
                                                                : 1000 );

                set_weapon_special( p, (tmp < 200) ? SPWPN_VENOM :
                                       (tmp < 333) ? SPWPN_SPEED :
                                       (tmp < 444) ? SPWPN_DRAINING :
                                       (tmp < 500) ? SPWPN_HOLY_WRATH :
                                       (tmp < 550) ? SPWPN_FLAMING :
                                       (tmp < 600) ? SPWPN_FREEZING :
                                       (tmp < 660) ? SPWPN_ORC_SLAYING :
                                       (tmp < 740) ? SPWPN_PROTECTION :
                                       (tmp < 800) ? SPWPN_ELECTROCUTION :
                                       (tmp < 840) ? SPWPN_VAMPIRICISM :
                                       (tmp < 850) ? SPWPN_DISTORTION :
                                       // short sword, sabre
                                       (tmp < 975) ? SPWPN_VENOM
                                                   : SPWPN_PAIN );
                break;


            case WPN_FALCHION:
            case WPN_LONG_SWORD:
            case WPN_SCIMITAR:
            case WPN_GREAT_SWORD:
            case WPN_DOUBLE_SWORD:
            case WPN_TRIPLE_SWORD:
                tmp = random2( (mitm[p].sub_type == WPN_GREAT_SWORD
                                || mitm[p].sub_type == WPN_DOUBLE_SWORD
                                || mitm[p].sub_type == WPN_TRIPLE_SWORD)
                                                            ? 930 : 1000 );

                set_weapon_special( p, (tmp < 300) ? SPWPN_VORPAL :
                                       (tmp < 530) ? SPWPN_HOLY_WRATH :
                                       (tmp < 625) ? SPWPN_ELECTROCUTION :
                                       (tmp < 675) ? SPWPN_DRAINING :
                                       (tmp < 740) ? SPWPN_ORC_SLAYING :
                                       (tmp < 810) ? SPWPN_PROTECTION :
                                       (tmp < 850) ? SPWPN_FLAMING :
                                       (tmp < 890) ? SPWPN_FREEZING :
                                       (tmp < 900) ? SPWPN_DISTORTION :
                                       (tmp < 930) ? SPWPN_VAMPIRICISM :
                                       // great, double, triple
                                       (tmp < 970) ? SPWPN_SPEED :
                                       (tmp < 980) ? SPWPN_PAIN
                                                   : SPWPN_VENOM );
                break;


            case WPN_WAR_AXE:
            case WPN_BROAD_AXE:
            case WPN_BATTLEAXE:
            case WPN_EXECUTIONERS_AXE:
            case WPN_HAND_AXE:
                tmp = random2( (mitm[p].sub_type == WPN_HAND_AXE) ? 950
                                                                  : 1000 );

                set_weapon_special( p, (tmp < 111) ? SPWPN_VENOM :
                                       (tmp < 265) ? SPWPN_ELECTROCUTION :
                                       (tmp < 400) ? SPWPN_FLAMING :
                                       (tmp < 535) ? SPWPN_FREEZING :
                                       (tmp < 665) ? SPWPN_ORC_SLAYING :
                                       (tmp < 890) ? SPWPN_VORPAL :
                                       (tmp < 900) ? SPWPN_DISTORTION :
                                       (tmp < 925) ? SPWPN_VAMPIRICISM :
                                       (tmp < 950) ? SPWPN_PAIN :
                                       // hand axe
                                       (tmp < 975) ? SPWPN_DRAINING
                                                   : SPWPN_HOLY_WRATH );
                break;


            case WPN_HALBERD:
            case WPN_GLAIVE:
            case WPN_SCYTHE:
            case WPN_TRIDENT:
            case WPN_SPEAR:
            case WPN_LOCHABER_AXE:
                tmp = random2( (mitm[p].sub_type == WPN_SPEAR) ? 875 :
                               (mitm[p].sub_type == WPN_SCYTHE
                                || mitm[p].sub_type == WPN_TRIDENT) ? 935
                                                               : 1000 );

                set_weapon_special( p, (tmp < 390) ? SPWPN_REACHING :
                                       (tmp < 520) ? SPWPN_VENOM :
                                       (tmp < 575) ? SPWPN_FLAMING :
                                       (tmp < 630) ? SPWPN_FREEZING :
                                       (tmp < 720) ? SPWPN_ORC_SLAYING :
                                       (tmp < 820) ? SPWPN_VORPAL :
                                       (tmp < 830) ? SPWPN_DISTORTION :
                                       (tmp < 860) ? SPWPN_VAMPIRICISM :
                                       (tmp < 875) ? SPWPN_PAIN :
                                       // spear
                                       (tmp < 935) ? SPWPN_SPEED :
                                       // scythe, trident
                                       (tmp < 970) ? SPWPN_PROTECTION
                                                   : SPWPN_HOLY_WRATH );
                break;


            case WPN_WHIP:
                tmp = random2(1000);

                set_weapon_special( p, (tmp < 250) ? SPWPN_ELECTROCUTION :
                                       (tmp < 450) ? SPWPN_SPEED :
                                       (tmp < 860) ? SPWPN_REACHING :
                                       (tmp < 930) ? SPWPN_VENOM :
                                       (tmp < 960) ? SPWPN_FLAMING :
                                       (tmp < 990) ? SPWPN_FREEZING
                                                   : SPWPN_DISTORTION );
                break;


            case WPN_BLOWGUN:
                if (coinflip())
                    set_weapon_special( p, SPWPN_VENOM );
                break;

            case WPN_SLING:
            case WPN_HAND_CROSSBOW:
                if (coinflip())
                    break;
                // **** intentional fall through here ****
            case WPN_BOW:
            case WPN_LONGBOW:
            case WPN_CROSSBOW:
                tmp = random2(1000);

                set_weapon_special( p, (tmp < 375) ? SPWPN_FLAMING :
                                       (tmp < 750) ? SPWPN_FREEZING :
                                       (tmp < 920) ? SPWPN_PROTECTION :
                                       (tmp < 980) ? SPWPN_VORPAL
                                                   : SPWPN_SPEED );
                break;


            case WPN_STAFF:
            case WPN_QUARTERSTAFF:
                tmp = random2(1000);

                set_weapon_special( p, (tmp < 200) ? SPWPN_HOLY_WRATH :
                                       (tmp < 500) ? SPWPN_PROTECTION :
                                       (tmp < 650) ? SPWPN_VORPAL :
                                       (tmp < 700) ? SPWPN_ELECTROCUTION :
                                       (tmp < 750) ? SPWPN_SPEED :
                                       (tmp < 865) ? SPWPN_FLAMING :
                                       (tmp < 980) ? SPWPN_FREEZING :
                                       (tmp < 990) ? SPWPN_DISTORTION
                                                   : SPWPN_PAIN );
                break;

            case WPN_DEMON_TRIDENT:
            case WPN_DEMON_WHIP:
            case WPN_DEMON_BLADE:
                tmp = random2( (mitm[p].sub_type == WPN_DEMON_BLADE) ? 865
                                                                     : 1000 );

                set_weapon_special( p, (tmp < 250) ? SPWPN_VENOM :
                                       (tmp < 430) ? SPWPN_VAMPIRICISM :
                                       (tmp < 450) ? SPWPN_DISTORTION :
                                       (tmp < 610) ? SPWPN_ELECTROCUTION :
                                       (tmp < 675) ? SPWPN_FLAMING :
                                       (tmp < 740) ? SPWPN_FREEZING :
                                       (tmp < 840) ? SPWPN_DRAINING :
                                       (tmp < 865) ? SPWPN_PAIN
                                       // demon blade
                                                   : SPWPN_REACHING );
                break;

            case WPN_BLESSED_BLADE:     // special gift of TSO
                set_weapon_special( p, SPWPN_HOLY_WRATH );
                break;

            // These weapons don't get default ego types.
            case WPN_ANCUS:
            case WPN_CLUB:
            case WPN_GIANT_CLUB:
            case WPN_GIANT_SPIKED_CLUB:
            case WPN_KNIFE:
            case WPN_QUICK_BLADE:
            case WPN_KATANA:
            case WPN_LAJATANG:
            default:
                break;
            }

            // Remove illegal brands:
            // - no holy wrath or orc slaying orcish weapons
            // I'm clobbering them with venom, to match the orc love
            // of poisoned missile weapons (although this might make
            // the Mines into another poison resist required dungeon,
            // and we might not want that so we'll have to see)...
            // the old code removed the brand, but also had to go out
            // of the way to avoid creating orcish weapons when looking
            // for good_item because of that (and there was also a
            // 50% chance of stripping any brand, that's now handled
            // in the above create ego weapon check... it's also not
            // used with good_item now).
            if (get_equip_race( mitm[p] ) == ISFLAG_ORCISH)
            {
                const int brand = get_weapon_brand( mitm[p] );
                if (brand == SPWPN_HOLY_WRATH || brand == SPWPN_ORC_SLAYING)
                    set_weapon_special( p, SPWPN_VENOM );
            }
        }

        // [6] apply fancy description if appropriate
        //
        // If not already fancy (and not a club), and ego or has a
        // non-base plus (or occasionally, just 'cause) then make fancy.
        // Note: artefacts never use these flags... they make their own
        // descriptions.
        if (get_equip_desc( mitm[p] ) == ISFLAG_NO_DESC
            && mitm[p].sub_type != WPN_CLUB
            && mitm[p].sub_type != WPN_GIANT_CLUB
            && mitm[p].sub_type != WPN_GIANT_SPIKED_CLUB)
        {
            int ench = 10 * item_cursed( mitm[p] )
                        + 5 * abs( mitm[p].plus  - race_plus )
                        + 5 * abs( mitm[p].plus2 - race_plus2 );

            if (get_weapon_brand( mitm[p] ) != SPWPN_NORMAL)
                ench += 50;
            else
            {
                ench +=  5 + 20 * (mitm[p].plus != race_plus
                                   || mitm[p].plus2 != race_plus2);
            }

            if (random2(100) < MINIMUM( ench, 95 ))
            {
                set_equip_desc( mitm[p], (coinflip() ? ISFLAG_GLOWING
                                                     : ISFLAG_RUNED) );
            }
        }
        break;

    case OBJ_MISSILES:
        // [0] initalize
        mitm[p].special = SPMSL_NORMAL;

        // [1] choose base missile type
        if (force_type != OBJ_RANDOM)
            mitm[p].sub_type = force_type;
        else
        {
            temp_rand = random2(1000);
            mitm[p].sub_type = (temp_rand < 300) ? MI_STONE :         // 30 %
                               (temp_rand < 500) ? MI_DART  :         // 20 %
                               (temp_rand < 700) ? MI_ARROW :         // 20 %
                               (temp_rand < 900) ? MI_BOLT            // 20 %
                                                 : MI_NEEDLE;         // 10 %
        }

        // [2] choose quantity
        if (mitm[p].sub_type == MI_LARGE_ROCK)
            mitm[p].quantity = 1 + roll_zdice(2,3);
        else
            mitm[p].quantity = roll_dice(4,15);

        // [3] choose race
        race_plus = 0;
        if (mitm[p].sub_type != MI_STONE && mitm[p].sub_type != MI_LARGE_ROCK)
        {
            race = ISFLAG_NO_RACE;

            switch (item_race)
            {
            case MAKE_ITEM_ELVEN:
                race = ISFLAG_ELVEN;
                break;

            case MAKE_ITEM_DWARVEN:
                race = ISFLAG_DWARVEN;
                break;

            case MAKE_ITEM_ORCISH:
                race = ISFLAG_ORCISH;
                break;

            case MAKE_ITEM_RANDOM_RACE:
                switch (mitm[p].sub_type)
                {
                case MI_ARROW:
                    temp_rand = random2(1000);

                    race = (temp_rand < 250) ? ISFLAG_ELVEN :
                           (temp_rand < 375) ? ISFLAG_ORCISH
                                             : ISFLAG_NO_RACE;
                    break;

                case MI_BOLT:
                    temp_rand = random2(1000);

                    race = (temp_rand < 250) ? ISFLAG_DWARVEN :
                           (temp_rand < 375) ? ISFLAG_ORCISH
                                             : ISFLAG_NO_RACE;
                    break;

                case MI_NEEDLE:
                    temp_rand = random2(1000);

                    race = (temp_rand < 250) ? ISFLAG_ORCISH :
                           (temp_rand < 375) ? ISFLAG_ELVEN
                                             : ISFLAG_NO_RACE;
                    break;

                case MI_DART:
                    temp_rand = random2(1000);

                    race = (temp_rand < 150) ? ISFLAG_ELVEN :
                           (temp_rand < 300) ? ISFLAG_DWARVEN :
                           (temp_rand < 400) ? ISFLAG_ORCISH
                                             : ISFLAG_NO_RACE;
                    break;

                default: // stones and rocks don't have racial types
                    break;
                }
                break;
            }

            set_equip_race( mitm[p], race );

            // elven arrows/darts and dwarven bolts are quality items
            if (((race == ISFLAG_ELVEN
                    && (mitm[p].sub_type == MI_ARROW
                        || mitm[p].sub_type == MI_DART))
                || (race == ISFLAG_DWARVEN
                    && mitm[p].sub_type == MI_BOLT))
                && coinflip())
            {
                race_plus += (coinflip() ? 1 : 2);
            }

            mitm[p].plus += race_plus;
        }

        // [4] choose plus
        if (mitm[p].sub_type != MI_LARGE_ROCK)
        {
            if (random2(100) < 10 + 2 * item_level + 20 * good_item)
                mitm[p].plus += roll_dice( 1, MAX_WPN_ENCHANT );

            // "curse" item by negating plus (no real curse status on missiles)
            if (!good_item && one_chance_in(10))
                mitm[p].plus = race_plus - mitm[p].plus - random2(3);
        }

        // [5] choose brand
        // no ego types for stones/rocks, needles (no damage) only poisoned
        if (mitm[p].sub_type == MI_NEEDLE)
        {
            // XXX: Actually, it might be nice if there where
            // some paralysis or slowing poison needles, just
            // so that blowguns have some added utility over
            // the other launchers/throwing weapons. -- bwr
            if (good_item || !one_chance_in(10))
                set_item_ego_type( mitm[p], OBJ_MISSILES, SPMSL_POISONED );
        }
        else if (mitm[p].sub_type != MI_STONE
                && mitm[p].sub_type != MI_LARGE_ROCK)
        {
            race = get_equip_race( mitm[p] );

            // orcish ammo gets poisoned a lot more often -- in the original
            // code it was poisoned every time!?
            // Then it just clobbered all brands (including none) 1/3rd of
            // the time.  Now it just extends to poison range to 3x ice+fire.
            const int pois = ((race == ISFLAG_ORCISH) ? 480 : 150);

            int chance = 3000 - 50 * item_level;

            if (race == ISFLAG_ELVEN || race == ISFLAG_DWARVEN)
                chance -= 500;

            // decide specials:
            temp_rand = random2( (good_item) ? pois : chance );

            set_item_ego_type( mitm[p], OBJ_MISSILES,
                               (temp_rand <  60)  ? SPMSL_FLAME :
                               (temp_rand < 120)  ? SPMSL_ICE   :
                               (temp_rand < pois) ? SPMSL_POISONED
                                                  : SPMSL_NORMAL );
        }

        // Reduced quantity if ego, and good_items which somehow do not
        // have a brand get a few extra.
        if (get_ammo_brand( mitm[p] ) != SPMSL_NORMAL)
            mitm[p].quantity -= random2(10);
        else if (good_item)
        {
            if (mitm[p].sub_type == MI_LARGE_ROCK)
                mitm[p].quantity += (coinflip() ? 1 : 2);
            else
                mitm[p].quantity += 5 + random2(9);
        }

        // Make sure we're creating at least one.
        if (mitm[p].quantity < 1)
            mitm[p].quantity = 1;

        // [6] apply fancy description if appropriate
        if (get_equip_desc( mitm[p] ) == ISFLAG_NO_DESC
            && mitm[p].sub_type != MI_LARGE_ROCK)
        {
            int ench = 10 * abs( mitm[p].plus  - race_plus );

            const int brand = get_ammo_brand( mitm[p] );
            if (brand != SPMSL_NORMAL && brand != SPMSL_POISONED)
                ench += 50;
            else
                ench +=  5 + 20 * (mitm[p].plus != race_plus);

            if (random2(100) < MINIMUM( ench, 95 ))
            {
                set_equip_desc( mitm[p], (coinflip() ? ISFLAG_GLOWING
                                                     : ISFLAG_RUNED) );
            }
        }
        break;

    case OBJ_ARMOUR:
        // [0] initialize:
        mitm[p].special = SPARM_NORMAL;

        // [1] establish the type of armour:
        if (force_type != OBJ_RANDOM)
            mitm[p].sub_type = force_type;
        else
        {
            mitm[p].sub_type = random2(3);

            if (random2(35) <= item_level + 10)
            {
                mitm[p].sub_type = random2(5);
                if (one_chance_in(4))
                    mitm[p].sub_type = ARM_ANIMAL_SKIN;
            }

            if (random2(60) <= item_level + 10)
            {
                mitm[p].sub_type = random2(8);
                if (one_chance_in(10))
                    mitm[p].sub_type = ARM_STUDDED_LEATHER_ARMOUR;
            }

            // dragon, ice, troll, crystal plate
            if (random2(30000) <= (item_level + 10) * (good_item + 1))
                mitm[p].sub_type = ARM_DRAGON_HIDE + random2(7);
            else if (random2(45000) <= (item_level + 10) * (good_item + 1))
            {
                // steam, mottled, storm, gold, swamp, animal skins
                mitm[p].sub_type = ARM_STEAM_DRAGON_HIDE + random2(11);

                if (mitm[p].sub_type == ARM_ANIMAL_SKIN && one_chance_in(20))
                    mitm[p].sub_type = ARM_CRYSTAL_PLATE_MAIL;
            }

            // secondary armours (more common if good_item):
            if (one_chance_in( 5 - 2 * good_item ))
            {
                mitm[p].sub_type = ARM_SHIELD + random2(5);

                if (mitm[p].sub_type == ARM_SHIELD)                 // 33.3%
                {
                    if (coinflip())
                        mitm[p].sub_type = ARM_BUCKLER;             // 50.0%
                    else if (one_chance_in(3))
                        mitm[p].sub_type = ARM_LARGE_SHIELD;        // 16.7%
                }
                else if (mitm[p].sub_type == ARM_HELMET && one_chance_in(3))
                {
                    // more details later
                    mitm[p].sub_type = ARM_CAP;
                }
                else if (mitm[p].sub_type == ARM_BOOTS && one_chance_in(10))
                {
                    mitm[p].sub_type = (coinflip() ? ARM_NAGA_BARDING
                                                   : ARM_CENTAUR_BARDING);
                }
            }
        }

        // apply helmet features
        if (mitm[p].sub_type == ARM_HELMET || mitm[p].sub_type == ARM_CAP)
        {
            set_helmet_desc( mitm[p], THELM_DESC_PLAIN );

            // add fancy features (wings, horns, etc)
            if (one_chance_in(3))
                set_helmet_random_desc( mitm[p] );

            // set sub-sub-type
            if (mitm[p].sub_type == ARM_HELMET)
            {
                set_helmet_type( mitm[p], (coinflip() ? THELM_HELMET
                                                      : THELM_HELM) );
            }
            else
            {
                set_helmet_type( mitm[p], (one_chance_in(5) ? THELM_WIZARD_HAT
                                                            : THELM_CAP) );
            }
        }

        // If we have a hide, we'll enchant it to a full armour if we're
        // trying for a good item, else we'll break since hides are always
        // always plain.
        if (armour_is_hide( mitm[p] ))
        {
            if (!good_item)
                break;

            hide2armour( mitm[p] );
        }

        // [2] determine and set armour race, then apply bonus
        switch (item_race)
        {
        case MAKE_ITEM_NO_RACE:
        default:
            break;

        case MAKE_ITEM_ELVEN:
            set_equip_race( mitm[p], ISFLAG_ELVEN );
            break;

        case MAKE_ITEM_DWARVEN:
            set_equip_race( mitm[p], ISFLAG_DWARVEN );
            break;

        case MAKE_ITEM_ORCISH:
            set_equip_race( mitm[p], ISFLAG_ORCISH );
            break;

        case MAKE_ITEM_RANDOM_RACE:
            switch (mitm[p].sub_type)
            {
            case ARM_SHIELD:
            case ARM_BUCKLER:
            case ARM_LARGE_SHIELD:
                temp_rand = random2(1000);

                set_equip_race( mitm[p], (temp_rand < 167) ? ISFLAG_DWARVEN :
                                         (temp_rand < 271) ? ISFLAG_ELVEN :
                                         (temp_rand < 362) ? ISFLAG_ORCISH
                                                           : ISFLAG_NO_RACE );
                break;

            case ARM_CLOAK:
                temp_rand = random2(1000);

                set_equip_race( mitm[p], (temp_rand < 125) ? ISFLAG_ELVEN :
                                         (temp_rand < 234) ? ISFLAG_DWARVEN :
                                         (temp_rand < 330) ? ISFLAG_ORCISH
                                                           : ISFLAG_NO_RACE );
                break;

            case ARM_GLOVES:
                set_equip_race( mitm[p], one_chance_in(8) ? ISFLAG_ELVEN
                                                          : ISFLAG_NO_RACE );
                break;

            case ARM_BOOTS:
                temp_rand = random2(1000);

                set_equip_race( mitm[p], (temp_rand <  83) ? ISFLAG_DWARVEN :
                                         (temp_rand < 198) ? ISFLAG_ELVEN :
                                         (temp_rand < 298) ? ISFLAG_ORCISH
                                                           : ISFLAG_NO_RACE );
                break;

            case ARM_CAP:
                set_equip_race( mitm[p], one_chance_in(12) ? ISFLAG_ELVEN
                                                           : ISFLAG_NO_RACE );
                break;

            case ARM_HELMET:
                temp_rand = random2(1000);

                set_equip_race( mitm[p], (temp_rand <  83) ? ISFLAG_DWARVEN :
                                         (temp_rand < 141) ? ISFLAG_ORCISH
                                                           : ISFLAG_NO_RACE );
                break;

            case ARM_ROBE:
                set_equip_race( mitm[p], one_chance_in(8) ? ISFLAG_ELVEN
                                                          : ISFLAG_NO_RACE );
                break;

            case ARM_LEATHER_ARMOUR:
            case ARM_STUDDED_LEATHER_ARMOUR:
                temp_rand = random2(1000);

                set_equip_race( mitm[p], (temp_rand < 100) ? ISFLAG_ORCISH :
                                         (temp_rand < 175) ? ISFLAG_ELVEN
                                                           : ISFLAG_NO_RACE );
                break;

            case ARM_RING_MAIL:
            case ARM_SCALE_MAIL:
            case ARM_CHAIN_MAIL:
                temp_rand = random2(1000);

                set_equip_race( mitm[p], (temp_rand < 100) ? ISFLAG_ORCISH :
                                         (temp_rand < 190) ? ISFLAG_DWARVEN :
                                         (temp_rand < 258) ? ISFLAG_ELVEN
                                                           : ISFLAG_NO_RACE );
                break;

            case ARM_SPLINT_MAIL:
            case ARM_BANDED_MAIL:
            case ARM_PLATE_MAIL:
                temp_rand = random2(1000);

                set_equip_race( mitm[p], (temp_rand < 100) ? ISFLAG_ORCISH :
                                         (temp_rand < 190) ? ISFLAG_DWARVEN
                                                           : ISFLAG_NO_RACE );
                break;

            default:
                // skins, hides, crystal PM, barding never have racial type
                break;
            }
            break;
        }

        // plus from racial type:
        race_plus = 0;

        switch (get_equip_race( mitm[p] ))
        {
        case ISFLAG_ELVEN:
            if (coinflip())
                race_plus += (coinflip() ? 1 : 2);
            break;

        case ISFLAG_DWARVEN:
            if (coinflip())
                race_plus++;
            break;

        case ISFLAG_ORCISH:
            race_plus += (1 - random2(3));
            break;

        default:
            break;
        }

        mitm[p].plus = race_plus;

        // [3] establish the plus (and possible curse) on the armour
        if (random2(200) <= 20 + item_level * 3
                           + 20 * good_item
                           + 20 * is_helmet_type( mitm[p], THELM_WIZARD_HAT ))
        {
            const int max_plus = armour_max_enchant( mitm[p] );
            const int chance = 20 + item_level * 2 + 20 * good_item;

            for (;;)
            {
                mitm[p].plus++;

                if (random2(300) > chance
                    || (mitm[p].plus >= max_plus && !one_chance_in(20)))
                {
                    break;
                }
            }
        }

        // cursed
        if (!good_item && one_chance_in(10))
        {
            do_curse_item( mitm[p] );

            if (!one_chance_in(20))
                mitm[p].plus = race_plus - mitm[p].plus - random2(3);
        }

        // [4] make artefacts?  if we do, we must break!
        if (allow_uniques
            && item_level > 4
            && random2(10000) <= (100 + item_level * 3) * (good_item + 1))
        {
            // unrand artefacts: may clobber above work (new type, pluses)
            if (you.level_type != LEVEL_ABYSS
                && you.level_type != LEVEL_PANDEMONIUM
                && one_chance_in(30))
            {
                icky = find_okay_unrandart( OBJ_ARMOUR, force_type );
                if (icky != -1)
                {
                    make_item_unrandart( mitm[p], icky );
                    break;
                }
            }

            // Sometimes we make pluses a bit more extreme on artefacts
            // Remember: higher pluses tend to result in weaker artefacts,
            // so we don't want to do this too much.
            if (one_chance_in(5))
            {
                if (mitm[p].plus >= 0)
                    mitm[p].plus += (coinflip() ? 1 : 2);
                else
                    mitm[p].plus -= (coinflip() ? 1 : 2);
            }

            // needs to come after plus is decided
            if (make_item_randart( mitm[p] ) && mitm[p].ra_props[RAP_CURSED])
                do_curse_item( mitm[p] );

            break;
        }

        // [5] make ego if we didn't make an artefact
        if ((random2(600) <= 30 + item_level * 3
                || (good_item && !one_chance_in(20))
                || is_helmet_type( mitm[p], THELM_WIZARD_HAT ))
            && (!item_cursed( mitm[p] ) || one_chance_in(30))
            && (get_equip_race( mitm[p] ) != ISFLAG_ORCISH
                || good_item
                || coinflip()))
        {
            int tmp;

            switch (mitm[p].sub_type)
            {
            case ARM_BUCKLER:
            case ARM_SHIELD:
            case ARM_LARGE_SHIELD:
                tmp = random2(1000);

                set_item_ego_type( mitm[p], OBJ_ARMOUR,
                                   (tmp <  40) ? SPARM_RESISTANCE :
                                   (tmp < 160) ? SPARM_FIRE_RESISTANCE :
                                   (tmp < 280) ? SPARM_COLD_RESISTANCE :
                                   (tmp < 400) ? SPARM_POISON_RESISTANCE :
                                   (tmp < 520) ? SPARM_POSITIVE_ENERGY
                                               : SPARM_PROTECTION );
                break;

            case ARM_CLOAK:
                tmp = random2(1000);

                if (get_equip_race( mitm[p] ) == ISFLAG_DWARVEN)
                {
                    // Note that Protection is different than having
                    // a clock with base AC 4 (armour skill only applies
                    // on the base, not on the ego bonus).
                    set_item_ego_type( mitm[p], OBJ_ARMOUR,
                                       (tmp < 500) ? SPARM_PROTECTION
                                                   : SPARM_PRESERVATION );
                }
                else
                {
                    set_item_ego_type( mitm[p], OBJ_ARMOUR,
                                       (tmp < 250) ? SPARM_POISON_RESISTANCE :
                                       (tmp < 500) ? SPARM_DARKNESS :
                                       (tmp < 750) ? SPARM_MAGIC_RESISTANCE
                                                   : SPARM_PRESERVATION );
                }
                break;

            case ARM_HELMET:
            case ARM_CAP:
                tmp = (coinflip() + is_helmet_type(mitm[p], THELM_WIZARD_HAT));

                set_item_ego_type( mitm[p], OBJ_ARMOUR,
                                   (tmp == 0) ? SPARM_SEE_INVISIBLE :
                                   (tmp == 1) ? SPARM_INTELLIGENCE
                                              : SPARM_MAGIC_RESISTANCE );
                break;

            case ARM_GLOVES:
                set_item_ego_type( mitm[p], OBJ_ARMOUR,
                                   coinflip() ? SPARM_DEXTERITY
                                              : SPARM_STRENGTH );
                break;

            case ARM_NAGA_BARDING:
            case ARM_CENTAUR_BARDING:
            case ARM_BOOTS:
                tmp = random2(600) + 200 * (mitm[p].sub_type != ARM_BOOTS);

                set_item_ego_type( mitm[p], OBJ_ARMOUR,
                                   (tmp < 200) ? SPARM_RUNNING :
                                   (tmp < 400) ? SPARM_LEVITATION :
                                   (tmp < 600) ? SPARM_STEALTH :
                                   (tmp < 700) ? SPARM_COLD_RESISTANCE
                                               : SPARM_FIRE_RESISTANCE );
                break;

            case ARM_ROBE:
                tmp = random2(1000);

                set_item_ego_type( mitm[p], OBJ_ARMOUR,
                                   (tmp < 230) ? SPARM_MAGIC_RESISTANCE :
                                   (tmp < 460) ? SPARM_FIRE_RESISTANCE :
                                   (tmp < 690) ? SPARM_COLD_RESISTANCE :
                                   (tmp < 920) ? SPARM_POSITIVE_ENERGY :
                                   (tmp < 995) ? SPARM_RESISTANCE
                                               : SPARM_ARCHMAGI );
                break;

            default:    // other body armours:
                // Note: Dragon armours + Crystal PM don't get ego types.
                if (mitm[p].sub_type == ARM_PLATE_MAIL && one_chance_in(15))
                {
                    set_item_ego_type( mitm[p],
                                       OBJ_ARMOUR, SPARM_PONDEROUSNESS );
                    mitm[p].plus += 2 + random2(3);
                }
                else if (!armour_is_hide( mitm[p], true )
                        && mitm[p].sub_type != ARM_CRYSTAL_PLATE_MAIL)
                {
                    tmp = random2(1000);

                    set_item_ego_type( mitm[p], OBJ_ARMOUR,
                                       (tmp < 200) ? SPARM_POISON_RESISTANCE :
                                       (tmp < 360) ? SPARM_MAGIC_RESISTANCE :
                                       (tmp < 430) ? SPARM_POSITIVE_ENERGY :
                                       (tmp < 715) ? SPARM_COLD_RESISTANCE
                                                   : SPARM_FIRE_RESISTANCE );
                }
                break;
            }
        }

        // [6] apply fancy description if appropriate
        //
        // If not already fancy, and ego/non-base plus/lucky then make fancy
        // note: artefacts never use these flags... they make their own
        // descriptions.
        if (get_equip_desc( mitm[p] ) == ISFLAG_NO_DESC
            && ((get_armour_ego_type( mitm[p] ) != SPARM_NORMAL
                    && !one_chance_in(3))
                || (item_cursed( mitm[p] ) && one_chance_in(3))
                || random2(20) < 3 + 2 * abs( mitm[p].plus  - race_plus )
                                   + 2 * (mitm[p].plus != race_plus)))
        {
            int ench = 10 * item_cursed( mitm[p] )
                       + 10 * abs( mitm[p].plus  - race_plus );

            if (get_armour_ego_type( mitm[p] ) != SPARM_NORMAL)
                ench += 50;
            else
                ench +=  5 + 20 * (mitm[p].plus != race_plus);

            if (random2(100) < MINIMUM( ench, 95 ))
            {
                temp_rand = random2(3);
                set_equip_desc( mitm[p], (temp_rand == 0) ? ISFLAG_GLOWING :
                                         (temp_rand == 1) ? ISFLAG_RUNED
                                                          : ISFLAG_SHINY );
            }
        }
        break;

    case OBJ_WANDS:
        // determine sub_type:
        if (force_type != OBJ_RANDOM)
            mitm[p].sub_type = force_type;
        else
        {
            mitm[p].sub_type = random2( NUM_WANDS );

            // Adjusted distribution here -- bwr
            // Wands used to be uniform (5.26% each)
            //
            // Now:
            // invis, hasting, healing  (1.11% each)
            // fireball, teleportaion   (3.74% each)
            // others                   (6.37% each)
            if (!good_item
                && ((mitm[p].sub_type == WAND_INVISIBILITY
                        || mitm[p].sub_type == WAND_HASTING
                        || mitm[p].sub_type == WAND_HEALING)
                    || ((mitm[p].sub_type == WAND_FIREBALL
                            || mitm[p].sub_type == WAND_TELEPORTATION)
                        && coinflip())))
            {
                mitm[p].sub_type = random2( NUM_WANDS );
            }
        }

        // determine charges:
        temp_value = ((mitm[p].sub_type == WAND_HEALING
                          || mitm[p].sub_type == WAND_HASTING
                          || mitm[p].sub_type == WAND_INVISIBILITY)   ? 5 :
                         (mitm[p].sub_type == WAND_FLAME
                          || mitm[p].sub_type == WAND_FROST
                          || mitm[p].sub_type == WAND_MAGIC_DARTS
                          || mitm[p].sub_type == WAND_RANDOM_EFFECTS) ? 15
                                                                      : 9 );

        mitm[p].plus = roll_zdice( 2, temp_value );
        break;

    case OBJ_FOOD:              // this can be parsed out {dlb}
        // determine sub_type:
        if (force_type != OBJ_RANDOM)
            mitm[p].sub_type = force_type;
        else
        {
            temp_rand = random2(1000);

            mitm[p].sub_type =
                    ((temp_rand < 200) ? FOOD_MEAT_RATION : // 20.00% chance
                     (temp_rand < 400) ? FOOD_BREAD_RATION :// 20.00% chance
                     (temp_rand < 480) ? FOOD_PEAR :        //  8.00% chance
                     (temp_rand < 560) ? FOOD_APPLE :       //  8.00% chance
                     (temp_rand < 640) ? FOOD_CHOKO :       //  8.00% chance
                     (temp_rand < 690) ? FOOD_ORANGE :      //  5.00% chance
                     (temp_rand < 740) ? FOOD_BANANA :      //  5.00% chance
                     (temp_rand < 780) ? FOOD_APRICOT :     //  4.00% chance
                     (temp_rand < 820) ? FOOD_SAUSAGE :     //  4.00% chance
                     (temp_rand < 840) ? FOOD_STRAWBERRY :  //  2.00% chance
                     (temp_rand < 860) ? FOOD_RAMBUTAN :    //  2.00% chance
                     (temp_rand < 880) ? FOOD_LEMON :       //  2.00% chance
                     (temp_rand < 900) ? FOOD_GRAPE :       //  2.00% chance
                     (temp_rand < 920) ? FOOD_SULTANA :     //  2.00% chance
                     (temp_rand < 940) ? FOOD_LYCHEE :      //  2.00% chance
                     (temp_rand < 960) ? FOOD_BEEF_JERKY :  //  2.00% chance
                     (temp_rand < 970) ? FOOD_CHEESE :      //  1.00% chance
                     (temp_rand < 980) ? FOOD_PIZZA :       //  1.00% chance
                     (temp_rand < 990) ? FOOD_SNOZZCUMBER : //  1.00% chance
                     (temp_rand < 995) ? FOOD_HONEYCOMB     //  0.50% chance
                                       : FOOD_ROYAL_JELLY );//  0.50% chance
        }

        // Happens with ghoul food acquirement -- use place_chunks() outherwise
        if (mitm[p].sub_type == FOOD_CHUNK)
        {
            for (count = 0; count < 1000; count++)
            {
                temp_rand = random2( NUM_MONSTERS );     // random monster
                temp_rand = mons_species( temp_rand );   // corpse base type

                if (mons_weight( temp_rand ) > 0)        // drops a corpse
                    break;
            }

            // set chunk flavour (default to common dungeon rat steaks):
            mitm[p].plus = (count == 1000) ? MONS_RAT : temp_rand;

            // set duration
            mitm[p].special = (10 + random2(11)) * 10;
        }

        // determine quantity:
        if (allow_uniques > 1)
            mitm[p].quantity = allow_uniques;
        else
        {
            mitm[p].quantity = 1;

            if (mitm[p].sub_type != FOOD_MEAT_RATION
                && mitm[p].sub_type != FOOD_BREAD_RATION)
            {
                if (one_chance_in(80))
                    mitm[p].quantity += random2(3);

                if (mitm[p].sub_type == FOOD_STRAWBERRY
                    || mitm[p].sub_type == FOOD_GRAPE
                    || mitm[p].sub_type == FOOD_SULTANA)
                {
                    mitm[p].quantity += 1 + roll_dice(2,7);
                }
            }
        }
        break;

    case OBJ_POTIONS:
        temp_rand = random2(1000);

        mitm[p].quantity = (temp_rand < 905) ? 1 :
                           (temp_rand < 998) ? 2
                                             : 3;

        if (force_type != OBJ_RANDOM)
            mitm[p].sub_type = force_type;
        else
        {
            temp_rand = random2( 9 - 2 * good_item );

            switch (temp_rand)  // general types of potion;
            {
            case 0:
            case 1:
            case 2:
            case 8:
                // healing potions
                if (one_chance_in(3))
                    mitm[p].sub_type = POT_HEAL_WOUNDS;         // 14.074%
                else
                    mitm[p].sub_type = POT_HEALING;             // 28.148%

                if (one_chance_in(20))
                {
                    mitm[p].quantity = 1;
                    mitm[p].sub_type = POT_CURE_MUTATION;       //  2.222%
                }
                break;

            case 3:
            case 4:
                // enhancements
                if (coinflip())
                    mitm[p].sub_type = POT_SPEED;               //  6.444%
                else
                    mitm[p].sub_type = POT_MIGHT;               //  6.444%

                if (one_chance_in(10))
                    mitm[p].sub_type = POT_BERSERK_RAGE;        //  1.432%

                if (one_chance_in(5))
                    mitm[p].sub_type = POT_INVISIBILITY;        //  3.580%

                if (one_chance_in(6))
                    mitm[p].sub_type = POT_LEVITATION;          //  3.580%

                if (one_chance_in(30))
                    mitm[p].sub_type = POT_PORRIDGE;            //  0.741%
                break;

            case 5:
                // gain ability
                mitm[p].sub_type = POT_GAIN_STRENGTH + random2(3); // 1.125%
                                                            // or 0.375% each
                if (one_chance_in(10))
                    mitm[p].sub_type = POT_EXPERIENCE;          //  0.125%

                if (one_chance_in(10))
                    mitm[p].sub_type = POT_MAGIC;               //  0.139%

                if (!one_chance_in(8))
                    mitm[p].sub_type = POT_RESTORE_ABILITIES;   //  9.722%
                else
                    mitm[p].quantity = 1;
                break;

            case 6:
            case 7:
                // bad things
                switch (random2(6))
                {
                case 0:
                case 4:
                    // is this not always the case? - no, level one is 0 {dlb}
                    if (item_level > 0)
                    {
                        mitm[p].sub_type = POT_POISON;          //  6.475%

                        if (item_level > 10 && one_chance_in(4))
                            mitm[p].sub_type = POT_STRONG_POISON;

                        break;
                    }

                /* **** intentional fall through **** */     // ignored for %
                case 5:
                    if (item_level > 6)
                    {
                        mitm[p].sub_type = POT_MUTATION;        //  3.237%
                        break;
                    }

                /* **** intentional fall through **** */     // ignored for %
                case 1:
                    mitm[p].sub_type = POT_SLOWING;             //  3.237%
                    break;

                case 2:
                    mitm[p].sub_type = POT_PARALYSIS;           //  3.237%
                    break;

                case 3:
                    mitm[p].sub_type = POT_CONFUSION;           //  3.237%
                    break;

                }

                if (one_chance_in(8))
                    mitm[p].sub_type = POT_DEGENERATION;        //  2.775%

                if (one_chance_in(1000))                        //  0.022%
                    mitm[p].sub_type = POT_DECAY;

                break;
            }
        }
        break;

    case OBJ_SCROLLS:
        // determine sub_type:
        if (force_type != OBJ_RANDOM)
            mitm[p].sub_type = force_type;
        else
        {
            // used to upgrade some scrolls (+10 guarantees that happens)
            const int depth_mod = random2( 1 + item_level ) + 10 * good_item;

            temp_rand = random2(1000);

            mitm[p].sub_type =
                    ((temp_rand >= 840) ? SCR_IDENTIFY :          // 16%
                     (temp_rand >= 740) ? SCR_TELEPORTATION :     // 10%
                     (temp_rand >= 680) ? SCR_MAGIC_MAPPING :     // 6%
                     (temp_rand >= 620) ? SCR_RANDOM_USELESSNESS :// 6%
                     (temp_rand >= 560) ? SCR_REMOVE_CURSE :      // 6%
                     (temp_rand >= 510) ? SCR_FEAR :              // 5%
                     (temp_rand >= 460) ? SCR_NOISE :             // 5%
                     (temp_rand >= 410) ? SCR_BLINKING :          // 5%
                     (temp_rand >= 370) ? SCR_DETECT_CURSE :      // 4%
                     (temp_rand >= 340) ? SCR_PAPER :             // 3%
                     (temp_rand >= 310) ? SCR_FORGETFULNESS :     // 3%
                     (temp_rand >= 280) ? SCR_CURSE_WEAPON :      // 3%
                     (temp_rand >= 250) ? SCR_CURSE_ARMOUR :      // 3%
                     (temp_rand >= 220) ? SCR_RECHARGING :        // 3%
                     (temp_rand >= 190) ? SCR_ENCHANT_ARMOUR :    // 3%
                     (temp_rand >= 160) ? SCR_ENCHANT_WEAPON_I :  // 3%
                     (temp_rand >= 130) ? SCR_ENCHANT_WEAPON_II : // 3%

                 // Crawl is kind to newbie adventurers {dlb}:
                 // yes -- these five are messy {dlb}:
                 // yes they are a hellish mess of tri-ops and long lines,
                 // this formating is somewhat better -- bwr
                     (temp_rand >= 100) ?
                        ((item_level < 4) ? SCR_TELEPORTATION
                                          : SCR_IMMOLATION) :    // 3%
                     (temp_rand >=  80) ?
                         ((depth_mod < 4) ? SCR_TELEPORTATION
                                          : SCR_ACQUIREMENT) :   // 2%
                     (temp_rand >=  60) ?
                         ((depth_mod < 4) ? SCR_DETECT_CURSE
                                          : SCR_SUMMONING) :     // 2%
                     (temp_rand >=  40) ?
                         ((depth_mod < 4) ? SCR_TELEPORTATION    // 2%
                                          : SCR_ENCHANT_WEAPON_III) :
                     (temp_rand >=  20) ?
                         ((depth_mod < 7) ? SCR_DETECT_CURSE
                                          : SCR_TORMENT)         // 2%
                     // default:
                       : ((depth_mod < 7) ? SCR_TELEPORTATION    // 2%
                                          : SCR_VORPALISE_WEAPON));
        }

        // determine quantity:
        temp_rand = random2(1000);

        mitm[p].quantity = ((temp_rand < 970
                          || mitm[p].sub_type == SCR_VORPALISE_WEAPON
                          || mitm[p].sub_type == SCR_ENCHANT_WEAPON_III
                          || mitm[p].sub_type == SCR_RECHARGING
                          || mitm[p].sub_type == SCR_ACQUIREMENT
                          || mitm[p].sub_type == SCR_TORMENT)  ? 1 :    // 97%
                                            (temp_rand < 990)  ? 2      //  2%
                                                               : 3);    //  1%
        break;

    case OBJ_JEWELLERY:
        // [1] determine jewellery type
        if (force_type != OBJ_RANDOM)
            mitm[p].sub_type = force_type;
        else
        {
            mitm[p].sub_type = (!one_chance_in(4) ? random2(24)   // rings
                                                  : AMU_RAGE + random2(10));

            // Adjusted distribution here -- bwr
            if ((mitm[p].sub_type == RING_INVISIBILITY
                    || mitm[p].sub_type == RING_REGENERATION
                    || mitm[p].sub_type == RING_TELEPORT_CONTROL
                    || mitm[p].sub_type == RING_FIRE
                    || mitm[p].sub_type == RING_ICE
                    || mitm[p].sub_type == RING_SLAYING)
                && !one_chance_in(3))
            {
                mitm[p].sub_type = random2(24);
            }
        }

        // [2] determine plus/cursed status
        pluses = ring_has_pluses( mitm[p] );

        if (pluses)
        {
            int tmp;

            if (!one_chance_in(3))
                mitm[p].plus = 1 + random2(3);
            else
            {
                tmp = MAX_RING_PLUS / 2;

                mitm[p].plus = 1 + random2( tmp + 1 )
                                 + random2( MAX_RING_PLUS - tmp );
            }

            if (pluses == 2)
            {
                // sometimes transfer first plus to second
                if (mitm[p].plus > 1 && one_chance_in(6))
                {
                    mitm[p].plus  = 0;
                    mitm[p].plus2 = (coinflip() ? 1 : 2);
                }

                if (!one_chance_in(3))
                    mitm[p].plus2 += 1 + random2(3);
                else
                {
                    tmp = MAX_RING_PLUS / 2;

                    mitm[p].plus2 = 1 + random2( tmp + 1 )
                                      + random2( MAX_RING_PLUS - tmp );
                }

                // sometimes transfer second plus to first
                if (mitm[p].plus && mitm[p].plus2 > 1 && one_chance_in(6))
                {
                    mitm[p].plus  += (coinflip() ? 1 : 2);
                    mitm[p].plus2  = 0;
                }

                if (mitm[p].plus + mitm[p].plus2 > MAX_RING_PLUS + 2)
                {
                    if (mitm[p].plus
                        && (mitm[p].plus > MAX_RING_PLUS || coinflip()))
                    {
                        mitm[p].plus--;
                    }

                    if (mitm[p].plus2
                        && (mitm[p].plus2 > MAX_RING_PLUS || coinflip()))
                    {
                        mitm[p].plus2--;
                    }
                }

                if (mitm[p].plus2 > MAX_RING_PLUS)
                    mitm[p].plus2 = MAX_RING_PLUS;
            }

            if (mitm[p].plus > MAX_RING_PLUS)
                mitm[p].plus = MAX_RING_PLUS;
        }

        // curse items
        if (!good_item
            && (mitm[p].sub_type == RING_HUNGER
                || mitm[p].sub_type == RING_TELEPORTATION
                || one_chance_in(10)))
        {
            // Rings of slaying are the only current ring with "hidden"
            // pluses, so cursing would be giving away too much.
            if (mitm[p].sub_type != RING_SLAYING)
                do_curse_item( mitm[p] );

            if (pluses && !one_chance_in(20))
                mitm[p].plus = -mitm[p].plus - random2(3);

            if (pluses == 2 && (mitm[p].plus >= 0 || !one_chance_in(19)))
                mitm[p].plus2 = -mitm[p].plus2 - random2(3);
        }

        // [3] make into an artefact?
        if (allow_uniques
            && item_level > 4
            && random2(10000) <= (100 + item_level * 3) * (good_item + 1))
        {
            // unrand artefacts: will clobber above work (new type, pluses)
            if (you.level_type != LEVEL_ABYSS
                && you.level_type != LEVEL_PANDEMONIUM
                && one_chance_in(30))
            {
                icky = find_okay_unrandart( OBJ_JEWELLERY, force_type );

                if (icky != -1)
                {
                    make_item_unrandart( mitm[p], icky );
                    break;
                }
            }

            // Make a randart.
            // All jewellery base types should now work. -- bwr
            if (ring_has_pluses( mitm[p] ))
            {
                // make pluses more extreme
                if (one_chance_in(5))
                {
                    if (mitm[p].plus >= 0)
                        mitm[p].plus += (coinflip() ? 1 : 2);
                    else
                        mitm[p].plus -= (coinflip() ? 1 : 2);
                }

                if (mitm[p].sub_type == RING_SLAYING && one_chance_in(5))
                {
                    if (mitm[p].plus2 >= 0)
                        mitm[p].plus2 += (coinflip() ? 1 : 2);
                    else
                        mitm[p].plus2 -= (coinflip() ? 1 : 2);
                }
            }

            if (make_item_randart( mitm[p] ) && mitm[p].ra_props[RAP_CURSED])
                do_curse_item( mitm[p] );
        }
        break;

    case OBJ_BOOKS:
        if (force_type != OBJ_RANDOM)
            mitm[p].sub_type = force_type;
        else
        {
            if (random2(3000) <= 40 + 5 * item_level + 60 * good_item)
                mitm[p].sub_type = BOOK_MANUAL;
            else if (random2(7000) <= 20 + 3 * item_level + 70 * good_item)
                mitm[p].sub_type = BOOK_DESTRUCTION;
            else
            {
                // random spellbook
                for (loopy = 0; loopy < 100; loopy++)
                {
                    do
                    {
                        mitm[p].sub_type = random2(NUM_BOOKS);
                    }
                    while (mitm[p].sub_type == BOOK_DESTRUCTION
                           || mitm[p].sub_type == BOOK_MANUAL);

                    const int roll = 1 + random2( item_level + 1 )
                                       + 10 * good_item;

                    const int rare = book_rarity( mitm[p].sub_type );

                    if (roll >= rare)
                        break;
                }
            }
        }

        mitm[p].special = random2( NUM_BOOKS_ADJ * NUM_BOOKS_MAT );

        // give manuals their skill
        if (mitm[p].sub_type == BOOK_MANUAL)
        {
            if (one_chance_in(8))
            {
                mitm[p].plus = SK_SPELLCASTING
                                    + random2(NUM_SKILLS - SK_SPELLCASTING);
            }
            else
            {
                mitm[p].plus = random2(SK_UNARMED_COMBAT);

                if (mitm[p].plus == SK_UNUSED_1)
                    mitm[p].plus = SK_UNARMED_COMBAT;
            }
        }
        break;

    case OBJ_STAVES:
        if (force_type != OBJ_RANDOM)
            mitm[p].sub_type = force_type;
        else
        {
            mitm[p].sub_type = random2(13);

            // top three non-spell staves are in separate block -- bwr
            if (mitm[p].sub_type >= 10)
                mitm[p].sub_type = STAFF_AIR + mitm[p].sub_type - 10;

            // spell staves
            if (one_chance_in(20))
                mitm[p].sub_type = STAFF_SMITING + random2(9);

            if ((mitm[p].sub_type == STAFF_ENERGY
                || mitm[p].sub_type == STAFF_CHANNELING) && one_chance_in(4))
            {
                mitm[p].sub_type = (coinflip() ? STAFF_WIZARDRY : STAFF_POWER);
            }
        }

        mitm[p].special = random2( NUM_STAVE_ADJ );

        if (item_is_rod( mitm[p] ))
        {
            mitm[p].plus2 = (6 + random2( MAX_ROD_CHARGE - 5 )) * ROD_CHARGE_MULT;
            mitm[p].plus = random2( mitm[p].plus2 + 1 );
        }

        if (!good_item && one_chance_in(10))
            do_curse_item( mitm[p] );
        break;

    case OBJ_ORBS:              // always forced in current setup {dlb}
        if (force_type != OBJ_RANDOM)
            mitm[p].sub_type = force_type;

        // I think we only have one type of orb now, so ... {dlb}
        set_unique_item_status( OBJ_ORBS, mitm[p].sub_type, UNIQ_EXISTS );
        break;

    // I think these must always be forced, too ... {dlb}

    case OBJ_MISCELLANY: //mv: rewrote with use of NUM_MISCELLANY (9 Aug 01)
        if (force_type != OBJ_RANDOM)
            mitm[p].sub_type = force_type;
        else
        {
            do
                mitm[p].sub_type = random2( NUM_MISCELLANY );
            while //mv: never generated
               (mitm[p].sub_type == MISC_RUNE_OF_ZOT
                || mitm[p].sub_type == MISC_HORN_OF_GERYON
                || mitm[p].sub_type == MISC_PORTABLE_ALTAR_OF_NEMELEX

                || (mitm[p].sub_type == MISC_DECK_OF_POWER
                    && !one_chance_in(6))
                || (mitm[p].sub_type == MISC_DECK_OF_SUMMONINGS
                    && !one_chance_in(3))
                || (mitm[p].sub_type == MISC_DECK_OF_TRICKS
                    && !one_chance_in(3))
                || (mitm[p].sub_type == MISC_DECK_OF_WONDERS
                    && !one_chance_in(10)));
        }

        // other fields by type:
        switch (mitm[p].sub_type)
        {
        case MISC_EMPTY_EBONY_CASKET:
            // filling those silly empty boxes -- bwr
            if (!one_chance_in(20))
                mitm[p].sub_type = MISC_BOX_OF_BEASTS;
            break;

        case MISC_RUNE_OF_ZOT:
            mitm[p].plus = item_race;
            break;

        case MISC_DECK_OF_TRICKS:
            mitm[p].plus = 4 + roll_dice(2,7);
            break;

        case MISC_DECK_OF_WONDERS:
        case MISC_DECK_OF_SUMMONINGS:
        case MISC_DECK_OF_POWER:
            mitm[p].plus = 4 + random2(10);
            break;

        default:
            break;
        }
        break;

    // that is, everything turns to gold if not enumerated above, so ... {dlb}
    default:
        mitm[p].base_type = OBJ_GOLD;
        mitm[p].colour = YELLOW;

        // Note that acquirement level gold gives less than the
        // price of a scroll of acquirement (500 gold). -- bwr
        if (good_item)
            mitm[p].quantity = 100 + random2(200) + random2(200);
        else
            mitm[p].quantity = 1 + roll_zdice(2,10) + random2(item_level);
        break;
    }

    if (dont_place) // should really only be used for monster inventories.
    {
        mitm[p].x = 0;
        mitm[p].y = 0;
        mitm[p].link = NON_ITEM;
        mitm[p].slot = -1;
    }
    else
    {
        // place on a random clear spot:
        random_in_bounds( x_pos, y_pos );
        move_item_to_grid( &p, x_pos, y_pos );

        // put a trap under it?
        int trap = 5;

        appearance_type  fancy = item_appearance( mitm[p] );
        if (fancy == APP_FANCY)
            trap += 5;
        else if (fancy == APP_SPECIAL)
            trap += 10;

        if (random2(100) < trap)
            place_specific_trap( x_pos, y_pos, TRAP_NONTELEPORT );
    }

    item_colour( mitm[p] );

    // Okay, this check should be redundant since the purpose of
    // this function is to create valid items.  Still, we're adding
    // this safety for fear that a report of Trog giving a non-existant
    // item might symbolize something more serious. -- bwr
    return (is_valid_item( mitm[p] ) ? p : NON_ITEM);
}                               // end make_item()


void give_item( int mid, int level_number ) //mv: cleanup+minor changes
{
    int temp_rand = 0;          // probability determination {dlb}

    int bp = 0;
    int thing_created = 0;
    int hand_used = 0;          // for Ettins etc.
    int base = 0;
    int sub = 0;

    int iquan = 0;
    // forces colour and quantity, too for intial weapons {dlb}
    int force_item = 0;
    int armour_plus;
    int brand;

    int race_default = mons_genus( menv[mid].type );

    // convert genus into item race:
    if (race_default == MONS_ORC)
        race_default = MAKE_ITEM_ORCISH;
    else if (race_default == MONS_ELF)
        race_default = MAKE_ITEM_ELVEN;
    else
        race_default = MAKE_ITEM_RANDOM_RACE;

    int item_race = race_default;
    int give_level = level_number;

    //mv: THIS CODE DISTRIBUTES WANDS/SCROLLS/POTIONS

    const int mlvl = mons_power( menv[mid].type );
    const bool unique = mons_is_unique( menv[mid].type );
    const bool spellcaster = mons_class_flag( menv[mid].type, M_SPELLCASTER );
    const bool use = (mons_itemuse( menv[mid].type ) == MONUSE_MAGIC_ITEMS);

    //mv - give scroll
    if ((unique && (!spellcaster || one_chance_in(3)))
        || (use && mlvl >= 5 && one_chance_in(50)))
    {
        thing_created = make_item( 0, OBJ_SCROLLS, OBJ_RANDOM, true,
                                   give_level, 0 );

        if (thing_created == NON_ITEM)
            return;

        mitm[thing_created].flags = 0;
        menv[mid].inv[MSLOT_SCROLL] = thing_created;
    }

    //mv - give potion
    if ((unique && (!spellcaster || one_chance_in(3)))
        || (use && mlvl >= 5 && one_chance_in(50)))
    {
        thing_created = make_item( 0, OBJ_POTIONS, OBJ_RANDOM, true,
                                   give_level, 0 );

        if (thing_created == NON_ITEM)
            return;

        mitm[thing_created].flags = 0;
        menv[mid].inv[MSLOT_POTION] = thing_created;
    }

    //mv - give wand
    if ((unique && ((!spellcaster && mlvl >= 5) || one_chance_in(5)))
        || (use && mlvl >= 10 && one_chance_in(200)))
    {
        thing_created = make_item( 0, OBJ_WANDS, OBJ_RANDOM, true,
                                   give_level, 0 );

        if (thing_created == NON_ITEM)
            return;

        mitm[thing_created].flags = 0;
        menv[mid].inv[MSLOT_WAND] = thing_created;
    }

    //end of DISTRIBUTE WANDS/POTIONS/SCROLLS CODE

    bp = get_item_slot();
    if (bp == NON_ITEM)
        return;

    // this flags things to "goto give_armour" below ... {dlb}
    ASSERT( mitm[bp].base_type == OBJ_UNASSIGNED );

    if (menv[mid].type == MONS_DANCING_WEAPON
        && ((player_in_branch( BRANCH_HALL_OF_BLADES ) && coinflip())
            || coinflip()))
    {
        give_level = MAKE_GOOD_ITEM;
    }

    // moved setting of quantity here to keep it in mind {dlb}
    iquan = 1;
    // I wonder if this is even used, given calls to item() {dlb}


    switch (menv[mid].type)
    {
    case MONS_KOBOLD:
        // a few of the smarter kobolds have blowguns.
        if (one_chance_in(15) && level_number > 2)
        {
            mitm[bp].base_type = OBJ_WEAPONS;
            mitm[bp].sub_type = WPN_BLOWGUN;
            break;
        }
        // intentional fallthrough
    case MONS_BIG_KOBOLD:
        if (random2(5) < 3)     // give hand weapon
        {
            mitm[bp].base_type = OBJ_WEAPONS;

            temp_rand = random2(3) + (level_number > 0) + (level_number > 2);
            mitm[bp].sub_type = ((temp_rand > 2) ? WPN_SHORT_SWORD :
                                 (temp_rand > 1) ? WPN_CLUB
                                                 : WPN_DAGGER);
        }
        else if (random2(5) < 2 && level_number > 1)    // give darts
        {
            item_race = MAKE_ITEM_NO_RACE;
            mitm[bp].base_type = OBJ_MISSILES;
            mitm[bp].sub_type = MI_DART;
            iquan = 1 + random2(5);
        }
        else
            goto give_ammo;
        break;

    case MONS_HOBGOBLIN:
        if (one_chance_in(3))
            item_race = MAKE_ITEM_ORCISH;

        if (random2(5) < 3)     // give hand weapon
        {
            mitm[bp].base_type = OBJ_WEAPONS;
            mitm[bp].sub_type = WPN_CLUB;
        }
        else
            goto give_ammo;
        break;

    case MONS_GOBLIN:
        if (one_chance_in(12) && level_number > 2)
        {
            if (one_chance_in(3))
                item_race = MAKE_ITEM_ORCISH;

            mitm[bp].base_type = OBJ_WEAPONS;
            mitm[bp].sub_type = WPN_BLOWGUN;
            break;
        }
        // deliberate fall through {dlb}
    case MONS_IJYB:
        if (one_chance_in(3))
            item_race = MAKE_ITEM_ORCISH;
        // deliberate fall through {dlb}
    case MONS_JESSICA:
        if (random2(5) < 3)     // < 1 // give hand weapon
        {
            mitm[bp].base_type = OBJ_WEAPONS;
            mitm[bp].sub_type = (coinflip() ? WPN_DAGGER : WPN_CLUB);
        }
        else
            goto give_ammo;
        break;

    case MONS_WIGHT:
    case MONS_DUANE:
        mitm[bp].base_type = OBJ_WEAPONS;
        mitm[bp].sub_type = (one_chance_in(6) ? WPN_WAR_AXE + random2(4)
                                              : WPN_MACE + random2(12));

        if (coinflip())
        {
            force_item = 1;
            item_race = MAKE_ITEM_NO_RACE;
            mitm[bp].plus += 1 + random2(3);
            mitm[bp].plus2 += 1 + random2(3);

            if (one_chance_in(5))
                set_item_ego_type( mitm[bp], OBJ_WEAPONS, SPWPN_FREEZING );
        }

        if (one_chance_in(3))
            do_curse_item( mitm[bp] );
        break;

    case MONS_EDMUND:
    case MONS_DONALD:
        mitm[bp].base_type = OBJ_WEAPONS;
        mitm[bp].sub_type = WPN_MACE;
        // fall through
    case MONS_GNOLL:
    case MONS_OGRE_MAGE:
    case MONS_NAGA_WARRIOR:
    case MONS_GREATER_NAGA:
        item_race = MAKE_ITEM_NO_RACE;

        if (!one_chance_in(5))
        {
            mitm[bp].base_type = OBJ_WEAPONS;

            temp_rand = random2(5);
            mitm[bp].sub_type = ((temp_rand >  2) ? WPN_SPEAR : // 40%
                                 (temp_rand == 2) ? WPN_FLAIL : // 20%
                                 (temp_rand == 1) ? WPN_HALBERD // 20%
                                                  : WPN_CLUB);  // 20%
        }
        break;

    case MONS_ORC:
        if (one_chance_in(15) && level_number > 2)
        {
            mitm[bp].base_type = OBJ_WEAPONS;
            mitm[bp].sub_type = WPN_BLOWGUN;
            break;
        }
        // deliberate fall through {gdl}
    case MONS_ORC_PRIEST:
    case MONS_TERENCE:
    case MONS_DRACONIAN:
    case MONS_DRACONIAN_ZEALOT:
        if (!one_chance_in(5))
        {
            mitm[bp].base_type = OBJ_WEAPONS;

            temp_rand = random2(240);
            mitm[bp].sub_type = ((temp_rand > 209) ? WPN_DAGGER :      //12.50%
                                 (temp_rand > 179) ? WPN_CLUB :        //12.50%
                                 (temp_rand > 152) ? WPN_FLAIL :       //11.25%
                                 (temp_rand > 128) ? WPN_HAND_AXE :    //10.00%
                                 (temp_rand > 108) ? WPN_HAMMER :      // 8.33%
                                 (temp_rand >  88) ? WPN_HALBERD :     // 8.33%
                                 (temp_rand >  68) ? WPN_SHORT_SWORD : // 8.33%
                                 (temp_rand >  48) ? WPN_MACE :        // 8.33%
                                 (temp_rand >  38) ? WPN_WHIP :        // 4.17%
                                 (temp_rand >  28) ? WPN_TRIDENT :     // 4.17%
                                 (temp_rand >  18) ? WPN_FALCHION :    // 4.17%
                                 (temp_rand >   8) ? WPN_MORNINGSTAR : // 4.17%
                                 (temp_rand >   2) ? WPN_WAR_AXE           // 2.50%
                                                   : WPN_SPIKED_FLAIL);// 1.25%
        }
        else
            goto give_ammo;
        break;

    case MONS_DEEP_ELF_FIGHTER:
    case MONS_DEEP_ELF_HIGH_PRIEST:
    case MONS_DEEP_ELF_KNIGHT:
    case MONS_DEEP_ELF_PRIEST:
    case MONS_DEEP_ELF_SOLDIER:
        mitm[bp].base_type = OBJ_WEAPONS;

        temp_rand = random2(100);
        mitm[bp].sub_type = ((temp_rand > 80) ? WPN_LONG_SWORD :    // 20%
                             (temp_rand > 60) ? WPN_SHORT_SWORD :   // 20%
                             (temp_rand > 48) ? WPN_SCIMITAR :      // 12%
                             (temp_rand > 36) ? WPN_MACE :          // 12%
                             (temp_rand > 24) ? WPN_BOW :           // 12%
                             (temp_rand > 12) ? WPN_LONGBOW         // 12%
                                              : WPN_HAND_CROSSBOW); // 12%
        break;

    case MONS_DEEP_ELF_ANNIHILATOR:
    case MONS_DEEP_ELF_CONJURER:
    case MONS_DEEP_ELF_DEATH_MAGE:
    case MONS_DEEP_ELF_DEMONOLOGIST:
    case MONS_DEEP_ELF_MAGE:
    case MONS_DEEP_ELF_SORCERER:
    case MONS_DEEP_ELF_SUMMONER:
    case MONS_DRACONIAN_SHIFTER:
    case MONS_DRACONIAN_SCORCHER:
    case MONS_DRACONIAN_ANNIHILATOR:
    case MONS_DRACONIAN_CALLER:
        mitm[bp].base_type = OBJ_WEAPONS;

        temp_rand = random2(6);
        mitm[bp].sub_type = ((temp_rand > 3) ? WPN_LONG_SWORD : // 2 in 6
                             (temp_rand > 2) ? WPN_SHORT_SWORD :// 1 in 6
                             (temp_rand > 1) ? WPN_SABRE :      // 1 in 6
                             (temp_rand > 0) ? WPN_DAGGER       // 1 in 6
                                             : WPN_WHIP);       // 1 in 6
        break;

    case MONS_ORC_WARRIOR:
    case MONS_ORC_HIGH_PRIEST:
    case MONS_BLORK_THE_ORC:
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
    case MONS_PALE_DRACONIAN:
    case MONS_RED_DRACONIAN:
    case MONS_WHITE_DRACONIAN:
    case MONS_GREEN_DRACONIAN:
    case MONS_MOTTLED_DRACONIAN:
    case MONS_BLACK_DRACONIAN:
    case MONS_YELLOW_DRACONIAN:
    case MONS_PURPLE_DRACONIAN:
        mitm[bp].base_type = OBJ_WEAPONS;

        temp_rand = random2(120);
        mitm[bp].sub_type = ((temp_rand > 109) ? WPN_LONG_SWORD :   // 8.33%
                             (temp_rand >  99) ? WPN_SHORT_SWORD :  // 8.33%
                             (temp_rand >  89) ? WPN_SCIMITAR :     // 8.33%
                             (temp_rand >  79) ? WPN_BATTLEAXE :    // 8.33%
                             (temp_rand >  69) ? WPN_HAND_AXE :     // 8.33%
                             (temp_rand >  59) ? WPN_HALBERD :      // 8.33%
                             (temp_rand >  49) ? WPN_GLAIVE :       // 8.33%
                             (temp_rand >  39) ? WPN_MORNINGSTAR :  // 8.33%
                             (temp_rand >  29) ? WPN_GREAT_MACE :   // 8.33%
                             (temp_rand >  19) ? WPN_TRIDENT :      // 8.33%
                             (temp_rand >  10) ? WPN_WAR_AXE :      // 7.50%
                             (temp_rand >   1) ? WPN_FLAIL :        // 7.50%
                             (temp_rand >   0) ? WPN_BROAD_AXE      // 0.83%
                                               : WPN_SPIKED_FLAIL); // 0.83%
        break;

    case MONS_ORC_WARLORD:
        // being at the top has its priviledges
        if (one_chance_in(4))
            give_level = MAKE_GOOD_ITEM;
        // deliberate fall-through
    case MONS_ORC_KNIGHT:
    case MONS_URUG:
    case MONS_NORBERT:
    case MONS_JOZEF:
    case MONS_VAULT_GUARD:
    case MONS_VAMPIRE_KNIGHT:
    case MONS_DRACONIAN_KNIGHT:
        mitm[bp].base_type = OBJ_WEAPONS;

        temp_rand = random2(25);
        mitm[bp].sub_type = ((temp_rand > 20) ? WPN_GREAT_SWORD :   // 16%
                             (temp_rand > 16) ? WPN_LONG_SWORD :    // 16%
                             (temp_rand > 12) ? WPN_BATTLEAXE :     // 16%
                             (temp_rand >  8) ? WPN_WAR_AXE :       // 16%
                             (temp_rand >  5) ? WPN_SCIMITAR :      // 12%
                             (temp_rand >  3) ? WPN_GREAT_MACE :    //  8%
                             (temp_rand >  2) ? WPN_LOCHABER_AXE :  //  4%
                             (temp_rand >  1) ? WPN_GLAIVE :        //  4%
                             (temp_rand >  0) ? WPN_BROAD_AXE       //  4%
                                              : WPN_HALBERD);       //  4%

        if (one_chance_in(4))
            mitm[bp].plus += 1 + random2(3);
        break;

    case MONS_CYCLOPS:
    case MONS_STONE_GIANT:
        item_race = MAKE_ITEM_NO_RACE;
        mitm[bp].base_type = OBJ_MISSILES;
        mitm[bp].sub_type = MI_LARGE_ROCK;
        break;

    case MONS_TWO_HEADED_OGRE:
    case MONS_ETTIN:
        item_race = MAKE_ITEM_NO_RACE;
        hand_used = 0;

        if (menv[mid].inv[MSLOT_WEAPON] != NON_ITEM)
            hand_used = 1;

        mitm[bp].base_type = OBJ_WEAPONS;
        mitm[bp].sub_type = (one_chance_in(3) ? WPN_GIANT_SPIKED_CLUB
                                              : WPN_GIANT_CLUB);

        if (one_chance_in(10) || menv[mid].type == MONS_ETTIN)
            mitm[bp].sub_type = WPN_GREAT_MACE;
        break;

    case MONS_REAPER:
        give_level = MAKE_GOOD_ITEM;
        // intentional fall-through...
    case MONS_SIGMUND:
        item_race = MAKE_ITEM_NO_RACE;
        mitm[bp].base_type = OBJ_WEAPONS;
        mitm[bp].sub_type = WPN_SCYTHE;
        break;

    case MONS_BALRUG:
        item_race = MAKE_ITEM_NO_RACE;
        mitm[bp].base_type = OBJ_WEAPONS;
        mitm[bp].sub_type = WPN_DEMON_WHIP;
        break;

    case MONS_RED_DEVIL:
    case MONS_AGNES:
        if (!one_chance_in(3))
        {
            item_race = MAKE_ITEM_NO_RACE;
            mitm[bp].base_type = OBJ_WEAPONS;
            mitm[bp].sub_type = (one_chance_in(3) ? WPN_DEMON_TRIDENT
                                                  : WPN_TRIDENT);
        }
        break;

    case MONS_OGRE:
    case MONS_HILL_GIANT:
    case MONS_EROLCHA:
        item_race = MAKE_ITEM_NO_RACE;
        mitm[bp].base_type = OBJ_WEAPONS;

        mitm[bp].sub_type = (one_chance_in(3) ? WPN_GIANT_SPIKED_CLUB
                                              : WPN_GIANT_CLUB);

        if (one_chance_in(10))
            mitm[bp].sub_type = WPN_GREAT_MACE;
        break;

    case MONS_CENTAUR:
    case MONS_CENTAUR_WARRIOR:
        item_race = MAKE_ITEM_NO_RACE;
        mitm[bp].base_type = OBJ_WEAPONS;
        mitm[bp].sub_type = WPN_BOW;
        break;

    case MONS_YAKTAUR:
    case MONS_YAKTAUR_CAPTAIN:
        item_race = MAKE_ITEM_NO_RACE;
        mitm[bp].base_type = OBJ_WEAPONS;
        mitm[bp].sub_type = WPN_CROSSBOW;
        break;

    case MONS_EFREET:
    case MONS_ERICA:
        force_item = 1;
        item_race = MAKE_ITEM_NO_RACE;
        mitm[bp].base_type = OBJ_WEAPONS;
        mitm[bp].sub_type = WPN_SCIMITAR;
        mitm[bp].plus = random2(5);
        mitm[bp].plus2 = random2(5);
        mitm[bp].colour = RED;  // forced by force_item above {dlb}
        set_item_ego_type( mitm[bp], OBJ_WEAPONS, SPWPN_FLAMING );
        break;

    case MONS_ANGEL:
        force_item = 1;
        mitm[bp].base_type = OBJ_WEAPONS;

        set_equip_desc( mitm[bp], ISFLAG_GLOWING );
        mitm[bp].colour = WHITE;        // forced by force_item above {dlb}

        if (one_chance_in(3))
        {
            mitm[bp].sub_type = (one_chance_in(3) ? WPN_GREAT_MACE : WPN_MACE);
            set_item_ego_type( mitm[bp], OBJ_WEAPONS, SPWPN_DISRUPTION );
        }
        else
        {
            mitm[bp].sub_type = WPN_LONG_SWORD;
            set_item_ego_type( mitm[bp], OBJ_WEAPONS, SPWPN_HOLY_WRATH );
        }

        mitm[bp].plus = 1 + random2(3);
        mitm[bp].plus2 = 1 + random2(3);
        break;

    case MONS_DAEVA:
        force_item = 1;
        mitm[bp].base_type = OBJ_WEAPONS;

        mitm[bp].sub_type = (one_chance_in(4) ? WPN_BLESSED_BLADE
                                              : WPN_LONG_SWORD);

        set_equip_desc( mitm[bp], ISFLAG_GLOWING );
        mitm[bp].colour = YELLOW;        // forced by force_item above {dlb}

        set_item_ego_type( mitm[bp], OBJ_WEAPONS, SPWPN_HOLY_WRATH );
        mitm[bp].plus = 1 + random2(3);
        mitm[bp].plus2 = 1 + random2(3);
        break;

    case MONS_HELL_KNIGHT:
    case MONS_MAUD:
    case MONS_ADOLF:
    case MONS_MARGERY:
        force_item = 1;
        mitm[bp].base_type = OBJ_WEAPONS;
        mitm[bp].sub_type = WPN_LONG_SWORD + random2(3);

        if (one_chance_in(7))
            mitm[bp].sub_type = WPN_HALBERD;
        if (one_chance_in(7))
            mitm[bp].sub_type = WPN_GLAIVE;
        if (one_chance_in(7))
            mitm[bp].sub_type = WPN_GREAT_MACE;
        if (one_chance_in(7))
            mitm[bp].sub_type = WPN_BATTLEAXE;
        if (one_chance_in(7))
            mitm[bp].sub_type = WPN_WAR_AXE;
        if (one_chance_in(7))
            mitm[bp].sub_type = WPN_BROAD_AXE;
        if (one_chance_in(7))
            mitm[bp].sub_type = WPN_DEMON_TRIDENT;
        if (one_chance_in(7))
            mitm[bp].sub_type = WPN_DEMON_BLADE;
        if (one_chance_in(7))
            mitm[bp].sub_type = WPN_DEMON_WHIP;

        temp_rand = random2(3);
        set_equip_desc( mitm[bp], (temp_rand == 1) ? ISFLAG_GLOWING :
                                  (temp_rand == 2) ? ISFLAG_RUNED
                                                   : ISFLAG_NO_DESC );

        if (one_chance_in(3))
            set_item_ego_type( mitm[bp], OBJ_WEAPONS, SPWPN_FLAMING );
        else if (one_chance_in(3))
        {
            temp_rand = random2(5);

            set_item_ego_type( mitm[bp], OBJ_WEAPONS,
                                ((temp_rand == 0) ? SPWPN_DRAINING :
                                 (temp_rand == 1) ? SPWPN_VORPAL :
                                 (temp_rand == 2) ? SPWPN_PAIN :
                                 (temp_rand == 3) ? SPWPN_DISTORTION
                                                  : SPWPN_SPEED) );
        }

        mitm[bp].plus += random2(6);
        mitm[bp].plus2 += random2(6);

        mitm[bp].colour = RED;  // forced by force_item above {dlb}

        if (one_chance_in(3))
            mitm[bp].colour = DARKGREY;
        if (one_chance_in(5))
            mitm[bp].colour = CYAN;
        break;

    case MONS_FIRE_GIANT:
        force_item = 1;
        mitm[bp].base_type = OBJ_WEAPONS;
        mitm[bp].sub_type = WPN_GREAT_SWORD;
        mitm[bp].plus = 0;
        mitm[bp].plus2 = 0;
        set_item_ego_type( mitm[bp], OBJ_WEAPONS, SPWPN_FLAMING );

        mitm[bp].colour = RED;  // forced by force_item above {dlb}
        if (one_chance_in(3))
            mitm[bp].colour = DARKGREY;
        if (one_chance_in(5))
            mitm[bp].colour = CYAN;
        break;

    case MONS_FROST_GIANT:
        force_item = 1;
        mitm[bp].base_type = OBJ_WEAPONS;
        mitm[bp].sub_type = WPN_BATTLEAXE;
        mitm[bp].plus = 0;
        mitm[bp].plus2 = 0;
        set_item_ego_type( mitm[bp], OBJ_WEAPONS, SPWPN_FREEZING );

        // forced by force_item above {dlb}
        mitm[bp].colour = (one_chance_in(3) ? WHITE : CYAN);
        break;

    case MONS_ORC_WIZARD:
    case MONS_ORC_SORCERER:
    case MONS_KOBOLD_DEMONOLOGIST:
    case MONS_NECROMANCER:
    case MONS_WIZARD:
    case MONS_PSYCHE:
    case MONS_JOSEPHINE:
        mitm[bp].base_type = OBJ_WEAPONS;
        mitm[bp].sub_type = WPN_DAGGER;
        break;

    case MONS_BORIS:
        mitm[bp].base_type = OBJ_WEAPONS;
        mitm[bp].sub_type = WPN_DAGGER;
        give_level = MAKE_GOOD_ITEM;
        break;

    case MONS_CEREBOV:
        force_item = 1;
        make_item_fixed_artefact( mitm[bp], false, SPWPN_SWORD_OF_CEREBOV );
        break;

    case MONS_DISPATER:
        force_item = 1;
        make_item_fixed_artefact( mitm[bp], false, SPWPN_STAFF_OF_DISPATER );
        break;

    case MONS_ASMODEUS:
        force_item = 1;
        make_item_fixed_artefact( mitm[bp], false, SPWPN_SCEPTRE_OF_ASMODEUS );
        break;

    case MONS_GERYON:
        //mv: probably should be moved out of this switch,
        //but it's not worth of it, unless we have more
        //monsters with misc. items
        mitm[bp].base_type = OBJ_MISCELLANY;
        mitm[bp].sub_type = MISC_HORN_OF_GERYON;
        break;

    case MONS_SALAMANDER: //mv: new 8 Aug 2001
        // Yes, they've got really nice items, but it's almost impossible
        // to get them.
        force_item = 1;
        item_race = MAKE_ITEM_NO_RACE;
        mitm[bp].base_type = OBJ_WEAPONS;
        temp_rand = random2(6);

        mitm[bp].sub_type = ((temp_rand == 5) ? WPN_GREAT_SWORD :
                             (temp_rand == 4) ? WPN_TRIDENT :
                             (temp_rand == 3) ? WPN_SPEAR :
                             (temp_rand == 2) ? WPN_GLAIVE :
                             (temp_rand == 1) ? WPN_BOW
                                              : WPN_HALBERD);

        set_item_ego_type( mitm[bp], OBJ_WEAPONS, SPWPN_FLAMING );

        mitm[bp].plus = random2(5);
        mitm[bp].plus2 = random2(5);
        mitm[bp].colour = RED;  // forced by force_item above {dlb}
        break;
    }                           // end "switch(menv[mid].type)"

    // only happens if something in above switch doesn't set it {dlb}
    if (mitm[bp].base_type == OBJ_UNASSIGNED)
        goto give_ammo;

    mitm[bp].x = 0;
    mitm[bp].y = 0;
    mitm[bp].link = NON_ITEM;
    mitm[bp].slot = -1;

    if (force_item)
        mitm[bp].quantity = iquan;
    else if (mons_is_unique( menv[mid].type ) && give_level != MAKE_GOOD_ITEM)
    {
        if (random2(100) <= 9 + menv[mid].hit_dice + 25 * (!spellcaster))
            give_level = MAKE_GOOD_ITEM;
        else
            give_level = level_number + 5;
    }

    base = mitm[bp].base_type;
    sub = mitm[bp].sub_type;

    // Note this mess, all the work above doesn't mean much unless
    // force_item is set... otherwise we're just going to take the
    // base and subtypes and create a new item. -- bwr
    thing_created = ((force_item) ? bp : make_item( 0, base, sub, true,
                                                    give_level, item_race ) );

    if (thing_created == NON_ITEM)
        return;

    mitm[thing_created].x = 0;
    mitm[thing_created].y = 0;
    mitm[thing_created].link = NON_ITEM;
    mitm[thing_created].slot = -1;
    unset_ident_flags( mitm[thing_created], ISFLAG_IDENT_MASK );

    //mv: now every item gets in appropriate slot
    //no more miscellany in potion slot etc. (19 May 2001)
    // hand_used = 0 unless Ettin's 2nd hand etc.
    if (mitm[thing_created].base_type == OBJ_WEAPONS)
    {
        menv[mid].inv[hand_used] = thing_created;

        brand = get_weapon_brand( mitm[thing_created] );

        if (brand == SPWPN_PROTECTION)
            menv[mid].armour_class += 5;
        else if ((brand == SPWPN_DISRUPTION || brand == SPWPN_HOLY_WRATH)
                && mons_is_unholy( &menv[mid] ))
        {
            set_item_ego_type( mitm[thing_created], OBJ_WEAPONS,
                               (coinflip() ? SPWPN_DRAINING
                                           : SPWPN_VAMPIRICISM) );
        }
    }
    else if (mitm[thing_created].base_type == OBJ_MISSILES)
        menv[mid].inv[MSLOT_MISSILE] = thing_created;
    else if (mitm[thing_created].base_type == OBJ_SCROLLS)
        menv[mid].inv[MSLOT_SCROLL] = thing_created;
    else if (mitm[thing_created].base_type == OBJ_GOLD)
        menv[mid].inv[MSLOT_GOLD] = thing_created;
    else if (mitm[thing_created].base_type == OBJ_POTIONS)
        menv[mid].inv[MSLOT_POTION] = thing_created;
    else if (mitm[thing_created].base_type == OBJ_MISCELLANY)
        menv[mid].inv[MSLOT_MISCELLANY] = thing_created;

    if (!force_item || mitm[thing_created].colour == BLACK)
        item_colour( mitm[thing_created] );

  give_ammo:
    // mv: gives ammunition
    // note that item_race is not reset for this section
    if (menv[mid].inv[MSLOT_WEAPON] != NON_ITEM
        && is_range_weapon( mitm[menv[mid].inv[MSLOT_WEAPON]] ))
    {
        base = OBJ_MISSILES;
        sub = fires_ammo_type( mitm[menv[mid].inv[MSLOT_WEAPON]] );

        thing_created = make_item( 0, base, sub, true, give_level, item_race );
        if (thing_created == NON_ITEM)
            return;

        // monsters will always have poisoned needles -- otherwise
        // they are just going to behave badly --GDL
        if (sub == MI_NEEDLE)
            set_item_ego_type(mitm[thing_created], OBJ_MISSILES, SPMSL_POISONED);

        mitm[thing_created].x = 0;
        mitm[thing_created].y = 0;
        menv[mid].inv[MSLOT_MISSILE] = thing_created;

        item_colour( mitm[thing_created] );
    }                           // end if needs ammo

    bp = get_item_slot();
    if (bp == NON_ITEM)
        return;

    mitm[bp].x = 0;
    mitm[bp].y = 0;
    mitm[bp].link = NON_ITEM;
    mitm[bp].slot = -1;

    item_race = race_default;
    give_level = 1 + (level_number / 2);

    switch (menv[mid].type)
    {
    case MONS_BORIS:
    case MONS_ANGEL:
        if (coinflip())
            give_level = MAKE_GOOD_ITEM;
        // fall-through
    case MONS_OGRE_MAGE:
    case MONS_DRACONIAN:
    case MONS_RED_DRACONIAN:
    case MONS_WHITE_DRACONIAN:
    case MONS_GREEN_DRACONIAN:
    case MONS_PALE_DRACONIAN:
    case MONS_MOTTLED_DRACONIAN:
    case MONS_BLACK_DRACONIAN:
    case MONS_YELLOW_DRACONIAN:
    case MONS_PURPLE_DRACONIAN:
    case MONS_DRACONIAN_SHIFTER:
    case MONS_DRACONIAN_SCORCHER:
    case MONS_DRACONIAN_ANNIHILATOR:
    case MONS_DRACONIAN_CALLER:
    case MONS_DRACONIAN_MONK:
    case MONS_DRACONIAN_ZEALOT:
    case MONS_DRACONIAN_KNIGHT:
        if (item_race == MAKE_ITEM_RANDOM_RACE)
            item_race = MAKE_ITEM_NO_RACE;
        // deliberate fall through {dlb}
    case MONS_SIGMUND:
    case MONS_IJYB:
    case MONS_FRANCIS:
    case MONS_NECROMANCER:
    case MONS_VAMPIRE_MAGE:
    case MONS_WIGHT:
        mitm[bp].base_type = OBJ_ARMOUR;
        mitm[bp].sub_type = ARM_ROBE;
        break;

    case MONS_ORC_WIZARD:
    case MONS_ORC_SORCERER:
    case MONS_BLORK_THE_ORC:
    case MONS_PSYCHE:
    case MONS_JESSICA:
    case MONS_NORRIS:
    case MONS_KOBOLD_DEMONOLOGIST:
    case MONS_WIZARD:
    case MONS_NAGA_MAGE:
        mitm[bp].base_type = OBJ_ARMOUR;
        switch (random2(6))
        {
        case 0:
        case 3:
        case 5:
        default:
            mitm[bp].sub_type = ARM_ROBE;
            break;

        case 1:
        case 4:
            mitm[bp].sub_type = ARM_LEATHER_ARMOUR;
            break;

        case 2:
            mitm[bp].sub_type = ARM_STUDDED_LEATHER_ARMOUR;
            break;
        }
        break;

    case MONS_ORC:
    case MONS_ORC_PRIEST:
        if (random2(5) <= 2)
            goto give_shield;
        // deliberate fall through {dlb}
    case MONS_ORC_HIGH_PRIEST:
    case MONS_DEEP_ELF_ANNIHILATOR:
    case MONS_DEEP_ELF_CONJURER:
    case MONS_DEEP_ELF_DEATH_MAGE:
    case MONS_DEEP_ELF_DEMONOLOGIST:
    case MONS_DEEP_ELF_MAGE:
    case MONS_DEEP_ELF_SORCERER:
    case MONS_DEEP_ELF_SUMMONER:
    case MONS_NAGA:
    case MONS_ERICA:
    case MONS_HAROLD:
    case MONS_JOSEPH:
    case MONS_JOSEPHINE:
    case MONS_JOZEF:
    case MONS_NORBERT:
    case MONS_TERENCE:
        mitm[bp].base_type = OBJ_ARMOUR;

        temp_rand = random2(14) + (mons_is_unique( menv[mid].type ));

        switch (temp_rand)
        {
        case 0:
        case 1:
            mitm[bp].sub_type = ARM_ROBE;
            break;
        case 2:
        case 3:
        case 10:
            mitm[bp].sub_type = ARM_LEATHER_ARMOUR;
            break;
        case 4:
        case 5:
        case 11:
            mitm[bp].sub_type = ARM_STUDDED_LEATHER_ARMOUR;
            break;
        case 6:
        case 7:
        case 12:
            mitm[bp].sub_type = ARM_RING_MAIL;
            break;
        case 8:
        case 13:
            mitm[bp].sub_type = ARM_SCALE_MAIL;
            break;
        case 9:
        case 14:
            mitm[bp].sub_type = ARM_CHAIN_MAIL;
            break;
        }
        break;

    case MONS_NAGA_WARRIOR:
    case MONS_GREATER_NAGA:
    case MONS_DUANE:
    case MONS_RUPERT:
    case MONS_WAYNE:
    case MONS_MICHAEL:
        mitm[bp].base_type = OBJ_ARMOUR;
        mitm[bp].sub_type = ARM_LEATHER_ARMOUR + random2(4);
        if (mitm[bp].sub_type == ARM_LEATHER_ARMOUR)
            mitm[bp].sub_type = ARM_STUDDED_LEATHER_ARMOUR;
        break;

    case MONS_DEEP_ELF_KNIGHT:
    case MONS_DEEP_ELF_FIGHTER:
    case MONS_DEEP_ELF_SOLDIER:
    case MONS_DEEP_ELF_PRIEST:
    case MONS_DEEP_ELF_HIGH_PRIEST:
    case MONS_URUG:
    case MONS_DONALD:
    case MONS_EDMUND:
    case MONS_AGNES:
        mitm[bp].base_type = OBJ_ARMOUR;
        mitm[bp].sub_type = ARM_RING_MAIL + random2(3);
        break;

    case MONS_ORC_WARLORD:
        // being at the top has its priviledges
        if (one_chance_in(4))
            give_level = MAKE_GOOD_ITEM;
        // deliberate fall through
    case MONS_ORC_KNIGHT:
    case MONS_ORC_WARRIOR:
    case MONS_ADOLF:
    case MONS_HELL_KNIGHT:
    case MONS_LOUISE:
    case MONS_FRANCES:
    case MONS_MARGERY:
    case MONS_MAUD:
    case MONS_VAMPIRE_KNIGHT:
    case MONS_VAULT_GUARD:
        mitm[bp].base_type = OBJ_ARMOUR;
        mitm[bp].sub_type = ARM_CHAIN_MAIL + random2(4);
        break;

    default:
        goto give_shield;
    }                           // end of switch(menv [mid].type)

    iquan = 1; // because it may have been set earlier
               // by giving ammo or weapons {dlb}

    base = mitm[bp].base_type;
    sub = mitm[bp].sub_type;

    if (mons_is_unique( menv[mid].type ) && give_level != MAKE_GOOD_ITEM)
    {
        if (random2(100) <= 9 + menv[mid].hit_dice + 25 * (!spellcaster))
            give_level = MAKE_GOOD_ITEM;
        else
            give_level = level_number + 5;
    }

    thing_created = make_item( 0, base, sub, true, give_level, item_race );

    if (thing_created == NON_ITEM)
        return;

    mitm[thing_created].x = 0;
    mitm[thing_created].y = 0;
    mitm[thing_created].link = NON_ITEM;
    mitm[thing_created].slot = -1;
    menv[mid].inv[MSLOT_ARMOUR] = thing_created;

    // Override colour set in make_item() for robes.
    if (mitm[thing_created].base_type == OBJ_ARMOUR
        && mitm[thing_created].sub_type == ARM_ROBE)
    {
        mitm[thing_created].colour = mons_class_colour( menv[mid].type );
    }

    menv[mid].armour_class += property( mitm[thing_created], PARM_AC );

    armour_plus = mitm[thing_created].plus;

    ASSERT(abs(armour_plus) < 20);

    if (abs(armour_plus) < 20)
        menv[mid].armour_class += armour_plus;

    menv[mid].evasion += property( mitm[thing_created], PARM_EVASION ) / 2;

    if (menv[mid].evasion < 1)
        menv[mid].evasion = 1;   // This *shouldn't* happen.

    // Give out shields to melee fighter types:
give_shield:
    // don't over-ride launcher ammo with a shield
    if (menv[mid].inv[MSLOT_MISSILE] != NON_ITEM) return;

    const int wpn = menv[mid].inv[MSLOT_WEAPON];

    // Note that there are a pile of assumptions about the items being usable,
    // this covers very little in the way of checking that the equipment
    // we give monsters is legal.
    if (wpn != NON_ITEM
        && (hands_reqd( mitm[wpn], mons_size(menv[mid].type) ) == HANDS_TWO
            || is_double_ended( mitm[wpn] )))
    {
        return;
    }

    base = OBJ_ARMOUR;
    sub = ARM_NONE;
    item_race = race_default;
    give_level = level_number;

    switch (menv[mid].type)
    {
    case MONS_HAROLD:
    case MONS_JOSEPH:
    case MONS_RUPERT:
    case MONS_MAUD:
    case MONS_ADOLF:
    case MONS_DUANE:
    case MONS_LOUISE:
        sub = (coinflip() ? ARM_SHIELD : ARM_BUCKLER);
        break;

    case MONS_URUG:
    case MONS_ORC_KNIGHT:
    case MONS_NAGA_WARRIOR:
    case MONS_HELL_KNIGHT:
    case MONS_VAMPIRE_KNIGHT:
    case MONS_VAULT_GUARD:
    case MONS_NORBERT:
    case MONS_JOZEF:
    case MONS_FRANCES:
    case MONS_AGNES:
    case MONS_WAYNE:
    case MONS_DRACONIAN_ZEALOT:
    case MONS_DRACONIAN_KNIGHT:
        sub = (one_chance_in(3) ? ARM_LARGE_SHIELD : ARM_SHIELD);
        break;

    case MONS_ORC_WARLORD:
        // being at the top has its priviledges
        if (one_chance_in(4))
            give_level = MAKE_GOOD_ITEM;

        sub = ARM_LARGE_SHIELD;
        break;

    case MONS_DRACONIAN:
    case MONS_RED_DRACONIAN:
    case MONS_WHITE_DRACONIAN:
    case MONS_GREEN_DRACONIAN:
    case MONS_PALE_DRACONIAN:
    case MONS_MOTTLED_DRACONIAN:
    case MONS_BLACK_DRACONIAN:
    case MONS_YELLOW_DRACONIAN:
    case MONS_PURPLE_DRACONIAN:
        if (!one_chance_in(5))
            break;
        // fall through
    case MONS_DEEP_ELF_SOLDIER:
    case MONS_DEEP_ELF_PRIEST:
        sub = (one_chance_in(3) ? ARM_SHIELD : ARM_BUCKLER);
        break;

    case MONS_DEEP_ELF_FIGHTER:
    case MONS_DEEP_ELF_HIGH_PRIEST:
        sub = (coinflip() ? ARM_SHIELD : ARM_BUCKLER);
        break;

    case MONS_DEEP_ELF_KNIGHT:
        sub = ARM_SHIELD;
        break;

    case MONS_ORC_HIGH_PRIEST:
    case MONS_TERENCE:
    case MONS_EDMUND:
        if (!one_chance_in(5))
            sub = ARM_BUCKLER;
        break;

    case MONS_BLORK_THE_ORC:
    case MONS_ORC_WARRIOR:
    case MONS_MUMMY_PRIEST:
    case MONS_GREATER_MUMMY:
    case MONS_SKELETAL_WARRIOR:
    case MONS_GREATER_NAGA:
    case MONS_WIGHT:
    case MONS_DONALD:
        if (!one_chance_in(5))
            sub = ARM_SHIELD;
        break;

    case MONS_TITAN:
    case MONS_FIRE_GIANT:
    case MONS_FROST_GIANT:
    case MONS_HILL_GIANT:
        item_race = MAKE_ITEM_NO_RACE;
        sub = ARM_LARGE_SHIELD;
        break;

    case MONS_ANGEL:
        item_race = MAKE_ITEM_NO_RACE;
        sub = (coinflip() ? ARM_SHIELD : ARM_BUCKLER);
        give_level = MAKE_GOOD_ITEM;
        break;

    default:
        break;
    }

    if (sub != ARM_NONE)
    {
        if (mons_is_unique(menv[mid].type) && give_level != MAKE_GOOD_ITEM)
        {
            if (random2(100) <= 9 + menv[mid].hit_dice + 25 * (!spellcaster))
                give_level = MAKE_GOOD_ITEM;
            else
                give_level = level_number + 5;
        }

        thing_created = make_item( 0, base, sub, true, give_level, item_race );

        if (thing_created == NON_ITEM)
            return;

        mitm[thing_created].x = 0;
        mitm[thing_created].y = 0;
        mitm[thing_created].link = NON_ITEM;
        mitm[thing_created].slot = -1;
        menv[mid].inv[MSLOT_MISSILE] = thing_created;

        if (get_armour_ego_type( mitm[thing_created] ) == SPARM_PROTECTION)
            menv[mid].armour_class += 3;
    }
}                               // end give_item()

//---------------------------------------------------------------------------
//                           PRIVATE HELPER FUNCTIONS
//---------------------------------------------------------------------------

static void set_weapon_special( int the_weapon, int spwpn )
{
    set_item_ego_type( mitm[the_weapon], OBJ_WEAPONS, spwpn );
}                               // end set_weapon_special()

static void check_doors(void)
{
    unsigned char ig;
    unsigned char solid_count = 0;      // clarifies innermost loop {dlb}
    int x,y;

    for (x = X_BOUND_1 + 1; x < X_BOUND_2; x++)
    {
        for (y = Y_BOUND_1 + 1; y < Y_BOUND_2; y++)
        {
            ig = grd[x][y];

            if (ig != DNGN_CLOSED_DOOR)
                continue;

            solid_count = 0;

            // first half of each conditional represents bounds checking {dlb}:
            if (grid_is_solid( grd[x - 1][y] ))
                solid_count++;

            if (grid_is_solid( grd[x + 1][y] ))
                solid_count++;

            if (grid_is_solid( grd[x][y - 1] ))
                solid_count++;

            if (grid_is_solid( grd[x][y + 1] ))
                solid_count++;

            grd[x][y] = ((solid_count < 2) ? DNGN_FLOOR : DNGN_CLOSED_DOOR);
        }
    }
}                               // end check_doors()

static void hide_doors(void)
{
    unsigned char dx = 0, dy = 0;     // loop variables
    unsigned char wall_count = 0;     // clarifies inner loop {dlb}

    for (dx = X_BOUND_1 + 1; dx < X_BOUND_2; dx++)
    {
        for (dy = Y_BOUND_1 + 1; dy < Y_BOUND_2; dy++)
        {
            if (grd[dx][dy] == DNGN_CLOSED_DOOR)
            {
                wall_count = 0;

                if (grid_is_wall( grd[dx - 1][dy] ))
                    wall_count++;

                if (grid_is_wall( grd[dx + 1][dy] ))
                    wall_count++;

                if (grid_is_wall( grd[dx][dy - 1] ))
                    wall_count++;

                if (grid_is_wall( grd[dx][dy + 1] ))
                    wall_count++;

                // if door is attached to more than one wall, hide it {dlb}:
                if (wall_count > 1 && one_chance_in(4))
                    grd[dx][dy] = DNGN_SECRET_DOOR;
            }
        }
    }
}                               // end hide_doors()

static void prepare_swamp(void)
{
    int i, j;                   // loop variables
    int temp_rand;              // probability determination {dlb}

    for (i = X_BOUND_1 + 1; i < X_BOUND_2; i++)
    {
        for (j = Y_BOUND_1 + 1; j < Y_BOUND_2; j++)
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

// Gives iater which is next to ground/shallow water a chance of being
// shallow. Checks each water space.
static void prepare_water( int level_number )
{
    int i, j, k, l;             // loop variables {dlb}
    unsigned char which_grid;   // code compaction {dlb}

    for (i = X_BOUND_1 + 1; i < X_BOUND_2; i++)
    {
        for (j = Y_BOUND_1 + 1; j < Y_BOUND_2; j++)
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

                            // must come first {dlb}
                            if (which_grid == DNGN_SHALLOW_WATER
                                && one_chance_in( 8 + level_number ))
                            {
                                grd[i][j] = DNGN_SHALLOW_WATER;
                            }
                            else if (which_grid >= DNGN_FLOOR
                                     && random2(100) < 80 - level_number * 4)
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
    {
        for(x = sx; x <= ex; x++)
        {
            for(y = sy; y <= ey; y++)
            {
                if (grd[x][y] == feature)
                    return (true);
            }
        }
    }

    return (false);
}

// stamp a box.  can avoid a possible type,  and walls and floors can
// be different (or not stamped at all)
// Note that the box boundaries are INclusive.
static bool make_box( int room_x1, int room_y1, int room_x2, int room_y2,
                      unsigned char floor, unsigned char wall,
                      unsigned char avoid )
{
    int bx, by;

    // check for avoidance
    if (find_in_area( room_x1, room_y1, room_x2, room_y2, avoid ))
        return (false);

    // draw walls
    if (wall != 0)
    {
        for (bx = room_x1; bx <= room_x2; bx++)
        {
            grd[bx][room_y1] = wall;
            grd[bx][room_y2] = wall;
        }
        for (by = room_y1 + 1; by < room_y2; by++)
        {
            grd[room_x1][by] = wall;
            grd[room_x2][by] = wall;
        }
    }

    // draw floor
    if (floor != 0)
    {
        for (bx = room_x1 + 1; bx < room_x2; bx++)
        {
            for (by = room_y1 + 1; by < room_y2; by++)
            {
                grd[bx][by] = floor;
            }
        }
    }

    return (true);
}

// take care of labyrinth, abyss, pandemonium
// returns 1 if we should skip further generation,
// -1 if we should immediately quit,  and 0 otherwise.
static int builder_by_type( int level_number, int level_type )
{
    int which_demon;
    int ret = 0;

    switch (level_type)
    {
    case LEVEL_LABYRINTH:
        labyrinth_level( level_number );
        ret = -1;
        break;

    case LEVEL_ABYSS:
        init_abyss();
        ret = 1;
        break;

    case LEVEL_PANDEMONIUM:
        which_demon = -1;

        // Could do spotty_level, but that doesn't always put all paired
        // stairs reachable from each other which isn't a problem in normal
        // dungeon but could be in Pandemonium

        // first we look to see if we want to place a special demonlord
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

        if (which_demon >= 0)   // build the demonlord's vault
        {
            you.unique_creatures[40 + which_demon] = 1;
            build_vaults( PANDEMONIUM_DEPTH, VAULT_MNOLEG + which_demon );
        }
        else
        {
            // standard pandemonium level code:
            if (one_chance_in(3))
                big_room( PANDEMONIUM_DEPTH );

            plan_main( level_number, 0 );

            if (one_chance_in(30))
                prepare_swamp();
            else
                builder_extras( PANDEMONIUM_DEPTH, level_type );

            build_minivaults( PANDEMONIUM_DEPTH, VAULT_RAND_DEMON_1 );
        }
        ret = 1;
        break;

    default:
        // must be normal dungeon, return 0
        break;
    }

    return (ret);
}

static int branch_bottom( int stairs )
{
    return (you.branch_stairs[stairs] + branch_depth(stairs));
}

// returns 1 if we should skip further generation,
// -1 if we should immediately quit,  and 0 otherwise.
static int builder_by_branch(int level_number)
{
    switch (you.where_are_you)
    {
    case BRANCH_HIVE:
        if (level_number == branch_bottom( STAIRS_HIVE ))
            build_vaults( level_number, VAULT_BEEHIVE, false );
        else
            spotty_level( false, 100 + random2(500), false );
        return (1);

    case BRANCH_SLIME_PITS:
        if (level_number == branch_bottom( STAIRS_SLIME_PITS ))
            build_vaults( level_number, VAULT_SLIME_PIT, false );
        else
            spotty_level( false, 100 + random2(500), false );
        return (1);

    case BRANCH_VAULTS:
        if (level_number == branch_bottom( STAIRS_VAULTS ))
        {
            build_vaults( level_number, VAULT_BOTTOM_OF_VAULTS, false );
            return (1);
        }
        break;

    case BRANCH_HALL_OF_BLADES:
        if (level_number == branch_bottom( STAIRS_HALL_OF_BLADES ))
        {
            build_vaults( level_number, VAULT_HALL_OF_BLADES, false );
            return (1);
        }
        break;

    case BRANCH_HALL_OF_ZOT:
        if (level_number == branch_bottom( STAIRS_HALL_OF_ZOT ))
        {
            build_vaults( level_number, VAULT_HALL_OF_ZOT, false );
            return (1);
        }
        break;

    case BRANCH_ECUMENICAL_TEMPLE:
        if (level_number == branch_bottom( STAIRS_ECUMENICAL_TEMPLE ))
        {
            build_vaults( level_number, VAULT_TEMPLE, false );
            return (1);
        }
        break;

    case BRANCH_SNAKE_PIT:
        if (level_number == branch_bottom( STAIRS_SNAKE_PIT ))
        {
            build_vaults( level_number, VAULT_SNAKE_PIT, false );
            return (1);
        }
        break;

    case BRANCH_ELVEN_HALLS:
        if (level_number == branch_bottom( STAIRS_ELVEN_HALLS ))
        {
            build_vaults( level_number, VAULT_ELF_HALL, false );
            return (1);
        }
        break;

    case BRANCH_TOMB:
        if (level_number == you.branch_stairs[STAIRS_TOMB] + 1)
        {
            build_vaults( level_number, VAULT_TOMB_1 );
            return (1);
        }
        else if (level_number == you.branch_stairs[STAIRS_TOMB] + 2)
        {
            build_vaults( level_number, VAULT_TOMB_2 );
            return (1);
        }
        else if (level_number == you.branch_stairs[STAIRS_TOMB] + 3)
        {
            build_vaults( level_number, VAULT_TOMB_3, false );
            return (1);
        }
        break;

    case BRANCH_SWAMP:
        if (level_number == branch_bottom( STAIRS_SWAMP ))
        {
            build_vaults( level_number, VAULT_SWAMP, false );
            return (1);
        }
        break;

    case BRANCH_ORCISH_MINES:
        spotty_level( false, 100 + random2(500), false );
        return (1);

    case BRANCH_LAIR:
        if (!one_chance_in(3))
        {
            spotty_level( false, 100 + random2(500), false );
            return (1);
        }
        break;

    case BRANCH_VESTIBULE_OF_HELL:
        build_vaults( level_number, VAULT_VESTIBULE_MAP );
        link_items();
        return (-1);

    case BRANCH_DIS:
        if (level_number == HELL_DEPTH)
        {
            build_vaults( level_number, VAULT_CASTLE_DIS, false );
            return (1);
        }
        break;

    case BRANCH_GEHENNA:
        if (level_number == HELL_DEPTH)
        {
            build_vaults( level_number, VAULT_ASMODEUS, false );
            return (1);
        }
        break;

    case BRANCH_COCYTUS:
        if (level_number == HELL_DEPTH)
        {
            build_vaults( level_number, VAULT_ANTAEUS, false );
            return (1);
        }
        break;

    case BRANCH_TARTARUS:
        if (level_number == HELL_DEPTH)
        {
            build_vaults( level_number, VAULT_ERESHKIGAL, false );
            return (1);
        }
        break;

    default:
        break;
    }

    return (0);
}

// returns 1 if we should dispense with city building,
// 0 otherwise.  Also sets special_room if one is generated
// so that we can link it up later.
static int builder_normal( int level_number, int level_type, spec_room &sr )
{
    UNUSED( level_type );

    bool skipped = false;
    bool done_city = false;

    if (player_in_branch( BRANCH_DIS ))
    {
        city_level(level_number);
        return 1;
    }

    if (player_in_branch( BRANCH_MAIN_DUNGEON )
        && level_number > 10 && level_number < 23 && one_chance_in(9))
    {
        // Can't have vaults on you.where_are_you != BRANCH_MAIN_DUNGEON levels
        build_vaults( level_number, VAULT_RANDOM );
        return 1;
    }

    if (player_in_branch( BRANCH_VAULTS ))
    {
        if (one_chance_in(3))
            city_level(level_number);
        else
            plan_main(level_number, 4);
        return 1;
    }

    if (level_number > 7 && level_number < 23)
    {
        if (one_chance_in(16))
        {
            spotty_level(false, 0, coinflip());
            return 1;
        }

        if (one_chance_in(16))
        {
            bigger_room();
            return 1;
        }
    }

    if (level_number > 2 && level_number < 23 && one_chance_in(3))
    {
        plan_main(level_number, 0);

        if (one_chance_in(3) && level_number > 6)
            build_minivaults( level_number, VAULT_MINIVAULT_1 );

        return 1;
    }

    if (one_chance_in(3))
        skipped = true;

    //V was 3
    if (!skipped && one_chance_in(7))
    {
        // sometimes roguey_levels generate a special room
        roguey_level( level_number, sr );

        if (level_number > 6
            && player_in_branch( BRANCH_MAIN_DUNGEON )
            && one_chance_in(4))
        {
            build_minivaults( level_number, VAULT_MINIVAULT_1 );
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
        special_room( level_number, sr );

    return 0;
}

// returns 1 if we should skip extras(),  otherwise 0
static int builder_basic( int level_number )
{
    int temp_rand;
    int doorlevel = random2(11);
    int corrlength = 2 + random2(14);
    int roomsize = 4 + random2(5) + random2(6);
    int no_corr = (one_chance_in(100) ? 500 + random2(500) : 30 + random2(200));
    int intersect_chance = (one_chance_in(20) ? 400 : random2(20));

    make_trail( 35, 30, 35, 20, corrlength, intersect_chance, no_corr,
                DNGN_STONE_STAIRS_DOWN_I, DNGN_STONE_STAIRS_UP_I );

    make_trail( 10, 15, 10, 15, corrlength, intersect_chance, no_corr,
                DNGN_STONE_STAIRS_DOWN_II, DNGN_STONE_STAIRS_UP_II );

    make_trail( 50, 20, 10, 15, corrlength, intersect_chance, no_corr,
                DNGN_STONE_STAIRS_DOWN_III, DNGN_STONE_STAIRS_UP_III );

    if (one_chance_in(4))
    {
        make_trail( 10, 20, 40, 20, corrlength, intersect_chance, no_corr,
                    DNGN_ROCK_STAIRS_DOWN );
    }

    if (one_chance_in(4))
    {
        make_trail( 50, 20, 40, 20, corrlength, intersect_chance, no_corr,
                    DNGN_ROCK_STAIRS_UP );
    }


    if (level_number > 1 && one_chance_in(3))
        big_room( level_number );

    if (random2(level_number) > 6 && one_chance_in(3))
        diamond_rooms( level_number );

    // make some rooms:
    int i, no_rooms, max_doors;
    int sx,sy,ex,ey, time_run;

    temp_rand = random2(750);
    time_run = 0;

    no_rooms = ((temp_rand > 63) ? (3 + roll_dice(2,14)) :   // 91.47% {dlb}
                (temp_rand > 14) ? 100                       //  6.53% {dlb}
                                 : 1);                       //  2.00% {dlb}

    max_doors = 2 + random2(8);

    for (i = 0; i < no_rooms; i++)
    {
        sx = 8 + random2(50);
        sy = 8 + random2(40);
        ex = sx + 2 + random2(roomsize);
        ey = sy + 2 + random2(roomsize);

        if (!make_room( sx, sy, ex, ey, max_doors, doorlevel ))
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

        if (!make_room( sx, sy, ex, ey, max_doors, doorlevel ))
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

static void builder_extras( int level_number, int level_type )
{
    if (level_number >= 11 && level_number <= 23
        && level_type == LEVEL_DUNGEON
        && one_chance_in(15))
    {
        place_specific_stair( DNGN_ENTER_LABYRINTH );
    }

    if (level_number > 6
        && level_type == LEVEL_DUNGEON
        && player_in_branch( BRANCH_MAIN_DUNGEON )
        && one_chance_in(3))
    {
        build_minivaults( level_number, VAULT_MINIVAULT_1 );
        return;
    }

    if (level_number > 5 && one_chance_in(10))
    {
        many_pools( (coinflip() ? DNGN_DEEP_WATER : DNGN_LAVA) );
        return;
    }

#ifdef USE_RIVERS

    //mv: it's better to be here so other dungeon features
    // are not overriden by water
    int river_type = one_chance_in( 5 + level_number ) ? DNGN_SHALLOW_WATER
                                                       : DNGN_DEEP_WATER;

    if (level_number > 11
        && (one_chance_in(5) || (level_number > 15 && !one_chance_in(5))))
    {
        river_type = DNGN_LAVA;
    }


    if (player_in_branch( BRANCH_GEHENNA ))
    {
        river_type = DNGN_LAVA;

        if (coinflip())
            build_river( river_type );
        else
            build_lake( river_type );
    }
    else if (player_in_branch( BRANCH_COCYTUS ))
    {
        river_type = DNGN_DEEP_WATER;

        if (coinflip())
            build_river( river_type );
        else
            build_lake( river_type );
    }


    if (level_number > 8 && one_chance_in(16))
        build_river( river_type );
    else if (level_number > 8 && one_chance_in(12))
    {
        build_lake( (river_type != DNGN_SHALLOW_WATER) ? river_type
                                                       : DNGN_DEEP_WATER );
    }
#endif // USE_RIVERS
}

void arm_trap( int trap_id, int amount )
{
    ASSERT( env.trap[trap_id].type != TRAP_UNASSIGNED );

    if (amount != -1)
        env.trap[trap_id].ammo = amount;
    else
    {
        switch (env.trap[trap_id].type)
        {
        case TRAP_DART:
        case TRAP_ARROW:
        case TRAP_BOLT:
        case TRAP_NEEDLE:
            env.trap[trap_id].ammo = 35 + roll_dice(2, 20);
            break;

        case TRAP_SPEAR:
        case TRAP_AXE:
            env.trap[trap_id].ammo = roll_dice(3, 4);
            break;

        default:
            // magic and blade traps
            env.trap[trap_id].ammo = 0;
            break;
        }
    }
}

static void place_traps( int level_number )
{
    int i;

    int num_traps = roll_zdice(2,5);
    num_traps += random2( (level_number < 28) ? (level_number / 4) : 7 );

    if (num_traps > MAX_TRAPS)
        num_traps = MAX_TRAPS;

    for (i = 0; i < num_traps; i++)
    {
        // traps could already be placed in vaults
        if (env.trap[i].type != TRAP_UNASSIGNED)
            continue;

        random_in_bounds( env.trap[i].x, env.trap[i].y );

        env.trap[i].type = TRAP_DART;

        if ((random2(1 + level_number) > 1) && one_chance_in(4))
            env.trap[i].type = TRAP_NEEDLE;
        if (random2(1 + level_number) > 2)
            env.trap[i].type = TRAP_ARROW;
        if (random2(1 + level_number) > 3)
            env.trap[i].type = TRAP_SPEAR;
        if (random2(1 + level_number) > 5)
            env.trap[i].type = TRAP_AXE;
        if (random2(1 + level_number) > 7)
            env.trap[i].type = TRAP_BOLT;
        if (random2(1 + level_number) > 11)
            env.trap[i].type = TRAP_BLADE;

        if ((random2(1 + level_number) > 14 && one_chance_in(3))
            || (player_in_branch( BRANCH_HALL_OF_ZOT ) && coinflip()))
        {
            env.trap[i].type = TRAP_ZOT;
        }

        if (one_chance_in(20))
            env.trap[i].type = TRAP_TELEPORT;
        if (one_chance_in(40))
            env.trap[i].type = TRAP_AMNESIA;

        // After a specific point we divide up bolt and arrow traps fairly,
        // thus assuring that bows and xbows are equally viable.
        if (level_number > 13
            && (env.trap[i].type == TRAP_BOLT || env.trap[i].type == TRAP_ARROW))
        {
            env.trap[i].type = (coinflip()) ? TRAP_BOLT : TRAP_ARROW;
        }

        grd[env.trap[i].x][env.trap[i].y] = DNGN_UNDISCOVERED_TRAP;
        arm_trap(i);
    }                           // end "for i"
}                               // end place_traps()

static void place_specific_stair( int stair )
{
    int sx, sy;

    random_in_bounds( sx, sy );
    grd[sx][sy] = stair;
}


static void place_branch_entrances( int dlevel, int level_type )
{
    int sx, sy;

    if (!level_type == LEVEL_DUNGEON)
        return;

    if (player_in_branch( BRANCH_MAIN_DUNGEON ))
    {
        // stair to HELL
        if (dlevel >= 20 && dlevel <= 27)
            place_specific_stair( DNGN_ENTER_HELL );

        // stair to PANDEMONIUM
        if (dlevel >= 20 && dlevel <= 50 && (dlevel == 23 || one_chance_in(4)))
            place_specific_stair( DNGN_ENTER_PANDEMONIUM );

        // stairs to ABYSS
        if (dlevel >= 20 && dlevel <= 30 && (dlevel == 24 || one_chance_in(3)))
            place_specific_stair( DNGN_ENTER_ABYSS );

        // level 26: replaces all down stairs with staircases to Zot:
        if (dlevel == 26)
        {
            int cx = 0, cy = 0;
            int count = 0;

            for (sx = X_BOUND_1 + 1; sx < X_BOUND_2; sx++)
            {
                for (sy = Y_BOUND_1 + 1; sy < Y_BOUND_2; sy++)
                {
                    if (grd[sx][sy] >= DNGN_STONE_STAIRS_DOWN_I
                            && grd[sx][sy] <= DNGN_ROCK_STAIRS_DOWN)
                    {
                        grd[sx][sy] = DNGN_FLOOR;

                        count++;
                        if (one_chance_in(count))
                        {
                            cx = sx;
                            cy = sy;
                        }
                    }
                }
            }

            // As a backup, if we didn't find a stair, we make one.
            if (count)
                grd[cx][cy] = DNGN_ENTER_ZOT;
            else
                place_specific_stair( DNGN_ENTER_ZOT );
        }
    }

    // place actual branch entrances
    for (int branch = 0; branch < MAX_BRANCHES; branch++)
    {
        int entrance = 100;

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

        place_specific_stair( DNGN_ENTER_ORCISH_MINES + branch );
    }   // end loop - possible branch entrances
}

static void make_trail(int xs, int xr, int ys, int yr, int corrlength,
                       int intersect_chance, int no_corr, unsigned char begin,
                       unsigned char end)
{
    int x_start, y_start;                   // begin point
    int x_ps, y_ps;                         // end point
    int finish = 0;
    int length = 0;
    int temp_rand;

    // temp positions
    int dir_x = 0;
    int dir_y = 0;
    int dir_x2, dir_y2;

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
                dir_x = (coinflip() ? -1 : 1);

            dir_y = 0;
        }
        else
        {
            if (dir_y2 != 0 && temp_rand < 6)
                dir_y = dir_y2;

            if (dir_y2 == 0 || temp_rand >= 6)
                dir_y = (coinflip() ? -1 : 1);

            dir_x = 0;
        }

        if (dir_x == 0 && dir_y == 0)
            continue;

        if (x_ps < X_BOUND_1 + 3)
        {
            dir_x = 1;
            dir_y = 0;
        }

        if (y_ps < Y_BOUND_1 + 3)
        {
            dir_y = 1;
            dir_x = 0;
        }

        if (x_ps > X_BOUND_2 - 3)
        {
            dir_x = -1;
            dir_y = 0;
        }

        if (y_ps > Y_BOUND_2 - 3)
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
            if (x_ps < X_BOUND_1 + 4)
            {
                dir_y = 0;      //random2(3) - 1;
                dir_x = 1;
            }

            if (x_ps > X_BOUND_2 - 4)
            {
                dir_y = 0;      //random2(3) - 1;
                dir_x = -1;
            }

            if (y_ps < Y_BOUND_1 + 4)
            {
                dir_y = 1;
                dir_x = 0;      //random2(3) - 1;
            }

            if (y_ps > Y_BOUND_2 - 4)
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
    if ((!grid_is_solid( grd[x][y] ) && grd[x][y] < DNGN_ENTER_PANDEMONIUM)
        || grd[x][y] == DNGN_CLOSED_DOOR)
    {
        return 1;
    }

    return 0;
}

// return TRUE if a room was made successfully
static bool make_room( int sx, int sy, int ex,int ey,
                       int max_doors, int doorlevel )
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
    for (ry = sy + 1; ry < ey; ry++)
    {
        find_door += good_door_spot(sx,ry);
        find_door += good_door_spot(ex,ry);
    }

    diag_door += good_door_spot(sx,sy);
    diag_door += good_door_spot(ex,sy);
    diag_door += good_door_spot(sx,ey);
    diag_door += good_door_spot(ex,ey);

    if ((diag_door + find_door) > 1 && max_doors == 1)
        return (false);

    if (find_door == 0 || find_door > max_doors)
        return (false);

    // look for 'special' rock walls - don't interrupt them
    if (find_in_area( sx, sy, ex, ey, DNGN_BUILDER_SPECIAL_WALL ))
        return (false);

    // convert the area to floor
    for (rx = sx; rx <= ex; rx++)
    {
        for (ry = sy; ry <= ey; ry++)
        {
            if (grd[rx][ry] <= DNGN_FLOOR)
                grd[rx][ry] = DNGN_FLOOR;
        }
    }

    // put some doors on the sides (but not in corners),
    // where it makes sense to do so.
    for (ry = sy + 1; ry < ey; ry++)
    {
        // left side
        if (grd[sx-1][ry] == DNGN_FLOOR
            && grid_is_solid( grd[sx-1][ry-1] )
            && grid_is_solid( grd[sx-1][ry+1] ))
        {
            if (random2(10) < doorlevel)
                grd[sx-1][ry] = DNGN_CLOSED_DOOR;
        }

        // right side
        if (grd[ex+1][ry] == DNGN_FLOOR
            && grid_is_solid( grd[ex+1][ry-1] )
            && grid_is_solid( grd[ex+1][ry+1] ))
        {
            if (random2(10) < doorlevel)
                grd[ex+1][ry] = DNGN_CLOSED_DOOR;
        }
    }

    // put some doors on the top & bottom
    for (rx = sx + 1; rx < ex; rx++)
    {
        // top
        if (grd[rx][sy-1] == DNGN_FLOOR
            && grid_is_solid( grd[rx-1][sy-1] )
            && grid_is_solid( grd[rx+1][sy-1] ))
        {
            if (random2(10) < doorlevel)
                grd[rx][sy-1] = DNGN_CLOSED_DOOR;
        }

        // bottom
        if (grd[rx][ey+1] == DNGN_FLOOR
            && grid_is_solid( grd[rx-1][ey+1] )
            && grid_is_solid( grd[rx+1][ey+1] ))
        {
            if (random2(10) < doorlevel)
                grd[rx][ey+1] = DNGN_CLOSED_DOOR;
        }
    }

    return (true);
}                               //end make_room()

// function to handle putting down lava/water critters
static int builder_swimmers( int level, int spaces, int swimmers[4] )
{
    int total = 0;
    int not_used = 0;

    int num = random2( 1 + spaces / 10 ) + roll_zdice(2,5);
    if (num > 15)
        num = 15;

    for (int i = 0; i < num; i++)
    {
        if (place_monster( not_used, swimmers[ random2(4) ], level, BEH_SLEEP,
                           MHITNOT, false, 1, 1, true ))
        {
            total++;
        }
    }

#if DEBUG_DIAGNOSTICS
    mpr( MSGCH_DIAGNOSTICS, "num swimmers = %d", total );
#endif

    return (total);
}

static void builder_monsters( int level_number, int level_type, int mon_wanted )
{
    int i = 0;
    int totalplaced = 0;
    int not_used = 0;
    int x,y;
    int lava_spaces, water_spaces;
    int swimmers[4];

    if (level_type == LEVEL_PANDEMONIUM)
        return;

    for (i = 0; i < mon_wanted; i++)
    {
        if (place_monster( not_used, RANDOM_MONSTER, level_number, BEH_SLEEP,
                           MHITNOT, false, 1, 1, true, PROX_ANYWHERE ))
        {
            totalplaced++;
        }
    }

    totalplaced += place_unique_monsters( false, PROX_ANYWHERE );

    // do aquatic and lava monsters:

    // count the number of lava and water tiles {dlb}:
    lava_spaces = 0;
    water_spaces = 0;

    for (x = X_BOUND_1 + 1; x < X_BOUND_2; x++)
    {
        for (y = Y_BOUND_1 + 1; y < Y_BOUND_2; y++)
        {
            if (grd[x][y] == DNGN_LAVA)
            {
                lava_spaces++;
            }
            else if (grd[x][y] == DNGN_DEEP_WATER
                     || grd[x][y] == DNGN_SHALLOW_WATER)
            {
                water_spaces++;
            }
        }
    }

#if DEBUG_DIAGNOSTICS
    mpr( MSGCH_DIAGNOSTICS, "water = %d; lava = %d", water_spaces, lava_spaces );
#endif

    if (lava_spaces > 49)
    {
        for (i = 0; i < 4; i++)
        {
            swimmers[i] = MONS_LAVA_WORM + random2(3);

            //mv: this is really ugly, but easiest
            //IMO generation of water/lava beasts should be changed,
            //because we want data driven code and not things like it
            if (one_chance_in(30))
                swimmers[i] = MONS_SALAMANDER;
        }

        totalplaced += builder_swimmers( level_number, lava_spaces, swimmers );
    }

    if (water_spaces > 49)
    {
        for (i = 0; i < 4; i++)
        {
            // mixing enums and math ticks me off !!! 15jan2000 {dlb}
            swimmers[i] = MONS_BIG_FISH + random2(4);

            // swamp worms and h2o elementals generated below: {dlb}
            if (player_in_branch( BRANCH_SWAMP ) && !one_chance_in(3))
                swimmers[i] = MONS_SWAMP_WORM;
        }

        if (level_number >= 25 && one_chance_in(5))
            swimmers[0] = MONS_WATER_ELEMENTAL;

        if (player_in_branch( BRANCH_COCYTUS ))
            swimmers[3] = MONS_WATER_ELEMENTAL;

        totalplaced += builder_swimmers( level_number, water_spaces, swimmers );
    }
}

static void builder_items(int level_number, int level_type, int items_wanted)
{
    UNUSED( level_type );

    int i = 0;
    unsigned char specif_type = OBJ_RANDOM;
    int items_levels = level_number;
    int item_no;

    if (player_in_branch( BRANCH_VAULTS ))
        items_levels += 5;      // now only slightly better in vaults
    else if (player_in_branch( BRANCH_ORCISH_MINES ))
        specif_type = OBJ_GOLD; /* lots of gold in the orcish mines */

    if (player_in_branch( BRANCH_VESTIBULE_OF_HELL )
        || player_in_hell()
        || player_in_branch( BRANCH_SLIME_PITS )
        || player_in_branch( BRANCH_HALL_OF_BLADES )
        || player_in_branch( BRANCH_ECUMENICAL_TEMPLE ))
    {
        /* No items in hell, the slime pits, the Hall */
        return;
    }
    else
    {
        for (i = 0; i < items_wanted; i++)
            make_item( 1, specif_type, OBJ_RANDOM, false, items_levels, 250 );

        // Make sure there's a very good chance of a knife being placed
        // in the first five levels, but not a guarantee of one.  The
        // intent of this is to reduce the advantage that "cutting"
        // starting weapons have.  -- bwr
        if (player_in_branch( BRANCH_MAIN_DUNGEON )
            && level_number < 5 && coinflip())
        {
            item_no = make_item( 0, OBJ_WEAPONS, WPN_KNIFE, false, 0, 250 );

            // Guarantee that the knife is uncursed and non-special
            if (item_no != NON_ITEM)
            {
                mitm[item_no].plus = 0;
                mitm[item_no].plus2 = 0;
                mitm[item_no].flags = 0;   // no id, no race/desc, no curse
                mitm[item_no].special = 0; // no ego type
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
static void specr_2( spec_room &sr )
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
        if (!in_bounds( sx, sy ))
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
            grd[sx][sy] = DNGN_SECRET_DOOR;

        if (j > 0 && grd[sx + dx][sy + dy] > DNGN_ROCK_WALL
            && grd[sx + dx][sy + dy] < DNGN_FLOOR)
        {
            grd[sx][sy] = DNGN_BUILDER_SPECIAL_FLOOR;
        }

        if (grd[sx][sy] == DNGN_ROCK_WALL)
            grd[sx][sy] = DNGN_BUILDER_SPECIAL_FLOOR;

        sx += dx;
        sy += dy;
    }

    sr.hooked_up = true;
}                               // end specr_2()


// Fill special room sr with monsters from the pit_list at density%...
// then place a "lord of the pit" of lord_type at (lordx, lordy).
static void fill_monster_pit( spec_room &sr,
                          FixedVector<pit_mons_def, MAX_PIT_MONSTERS> &pit_list,
                          int density, int lord_type, int lordx, int lordy )
{
    int i, x, y;

    // make distribution cumulative
    for (i = 1; i < MAX_PIT_MONSTERS; i++)
    {
        // assuming that the first zero rarity is the end of the list:
        if (!pit_list[i].rare)
            break;

        pit_list[i].rare = pit_list[i].rare + pit_list[i - 1].rare;
    }

    const int num_types = i;
    const int rare_sum = pit_list[num_types - 1].rare;

    // calculate die_size, factoring in the density% of the pit
    const int die_size = (rare_sum * 100) / density;

#if DEBUG_DIAGNOSTICS
    for (i = 0; i < num_types; i++)
    {
        char buff[ ITEMNAME_SIZE ];

        const int delta = ((i > 0) ? pit_list[i].rare - pit_list[i - 1].rare
                                   : pit_list[i].rare);

        const float perc = (static_cast<float>( delta ) * 100.0)
                                / static_cast<float>( rare_sum );

        mpr( MSGCH_DIAGNOSTICS, "%6.2f%%: %s", perc,
              moname( pit_list[i].type, true, DESC_PLAIN, buff ) );
    }
#endif

    // put the boss monster down
    if (lord_type != MONS_PROGRAM_BUG)
        mons_place( lord_type, BEH_SLEEP, MHITNOT, true, lordx, lordy );

    // place monsters and give them items {dlb}:
    for (x = sr.x1; x <= sr.x2; x++)
    {
        for (y = sr.y1; y <= sr.y2; y++)
        {
            // avoid the boss (or anyone else we may have dropped already)
            if (mgrd[x][y] != NON_MONSTER)
                continue;

            const int roll = random2( die_size );

            // density skip (no need to iterate)
            if (roll >= rare_sum)
                continue;

            // run throught the cumulative chances and place a monster
            for (i = 0; i < num_types; i++)
            {
                if (roll < pit_list[i].rare)
                {
                    mons_place( pit_list[i].type, BEH_SLEEP, MHITNOT,
                                true, x, y );
                    break;
                }
            }
        }
    }
}

// overwrites anything;  this function better be called early on during
// creation..
static void special_room( int level_number, spec_room &sr )
{
    char room_type = SROOM_LAIR_KOBOLD;
    int thing_created = 0;
    int x, y;

    unsigned char obj_type = OBJ_RANDOM;  // used in calling make_item() {dlb}
    int i;                                // general purpose loop variable {dlb}
    int temp_rand = 0;                    // probability determination {dlb}

    FixedVector< pit_mons_def, MAX_PIT_MONSTERS >  pit_list;

    for (i = 0; i < MAX_PIT_MONSTERS; i++)
    {
        pit_list[i].type = MONS_PROGRAM_BUG;
        pit_list[i].rare = 0;
    }

    // pick a type of room
    if (level_number < 7)
        room_type = SROOM_LAIR_KOBOLD;
    else
    {
        room_type = random2(NUM_SPECIAL_ROOMS);

        if ((level_number > 13 && room_type == SROOM_LAIR_KOBOLD)
            || (level_number < 12 && room_type == SROOM_JELLY_PIT)
            || (level_number < 16
                && (room_type == SROOM_TREASURY || room_type == SROOM_MORGUE))
            || (level_number < 17 && one_chance_in(4)))
        {
            room_type = SROOM_LAIR_ORC;
        }

        if (level_number > 19 && coinflip())
            room_type = SROOM_MORGUE;
    }

    room_type = SROOM_JELLY_PIT;

    int room_x1 = 0;
    int room_y1 = 0;
    int room_x2, room_y2;

    // pick a well dimensioned rectangle of a good size:
    int short_side = 4 + random2(5);
    int room_size = 25;

    if (room_type != SROOM_TREASURY)
    {
        room_size += random2(11);

        if (random2(100) < level_number)
        {
            short_side += 1;
            room_size += random2(15);
        }
    }

    // flip dimensions (adding one for the extra wall):
    if (coinflip())
    {
        room_x2 = short_side + 1;
        room_y2 = room_size / short_side + 1;
    }
    else
    {
        room_x2 = room_size / short_side + 1;
        room_y2 = short_side + 1;
    }

    // find a home for the pit:
    random_place_rectangle( room_x1, room_y1, room_x2, room_y2 );

    // do special walls & floor:
    make_box( room_x1, room_y1, room_x2, room_y2,
              DNGN_BUILDER_SPECIAL_FLOOR, DNGN_BUILDER_SPECIAL_WALL );


    // set up passed in spec_room structure
    sr.created = true;
    sr.hooked_up = false;
    sr.x1 = room_x1 + 1;
    sr.x2 = room_x2 - 1;
    sr.y1 = room_y1 + 1;
    sr.y2 = room_y2 - 1;

    // find home for the lord (setting lord later)
    int lord = MONS_PROGRAM_BUG;
    const int lordx = sr.x1 + random2(sr.x2 - sr.x1);
    const int lordy = sr.y1 + random2(sr.y2 - sr.y1);

    switch (room_type)
    {
    case SROOM_LAIR_ORC:
#if DEBUG_DIAGNOSTICS
        mpr( MSGCH_DIAGNOSTICS, "Build: Orc lair" );
#endif
        lord = MONS_ORC_KNIGHT;

        // XXX: eventually it'd be nice to be able to make pit_lists
        // automatically using generation level/rarity, branch,
        // and species/genus infomation.
        pit_list[0].type = MONS_ORC;
        pit_list[0].rare = 50;

        pit_list[1].type = MONS_ORC_WARRIOR;
        pit_list[1].rare = 5 + level_number * 2;

        pit_list[2].type = MONS_WARG;
        pit_list[2].rare = 5 + level_number / 4;

        pit_list[3].type = MONS_ORC_PRIEST;
        pit_list[3].rare = 2 + level_number / 2;

        pit_list[4].type = MONS_ORC_WIZARD;
        pit_list[4].rare = 2 + level_number / 2;

        if (level_number > 8)
        {
            pit_list[5].type = MONS_ORC_KNIGHT;
            pit_list[5].rare = 5 + (level_number - 8) * 2;

            pit_list[6].type = MONS_OGRE;
            pit_list[6].rare = 2 + (level_number - 8) / 2;

            if (level_number > 15)
            {
                lord = MONS_ORC_WARLORD;

                pit_list[7].type = MONS_TROLL;
                pit_list[7].rare = 5 + (level_number - 15) / 2;
            }
        }

        fill_monster_pit( sr, pit_list, 75, lord, lordx, lordy );
        break;

    case SROOM_LAIR_KOBOLD:
#if DEBUG_DIAGNOSTICS
        mpr( MSGCH_DIAGNOSTICS, "Build: Kobold lair" );
#endif
        lord = MONS_BIG_KOBOLD;

        pit_list[0].type = MONS_KOBOLD;
        pit_list[0].rare = 50;

        pit_list[1].type = MONS_BIG_KOBOLD;
        pit_list[1].rare = 7 * ((level_number < 10) ? level_number : 10);

        if (level_number >= 10)
        {
            pit_list[2].type = MONS_KOBOLD_DEMONOLOGIST;
            pit_list[2].rare = 13;
        }

        fill_monster_pit( sr, pit_list, 75, lord, lordx, lordy );
        break;

    case SROOM_MORGUE:
#if DEBUG_DIAGNOSTICS
        mpr( MSGCH_DIAGNOSTICS, "Build: Morgue" );
#endif
        if (level_number < 20)
            lord = (coinflip() ? MONS_VAMPIRE_KNIGHT : MONS_GREATER_MUMMY);
        else
        {
            lord = (one_chance_in(3) ? MONS_LICH : MONS_VAMPIRE_MAGE);

            if (one_chance_in(20))
                lord = (coinflip() ? MONS_CURSE_SKULL : MONS_ANCIENT_LICH);
        }

        // based off the old percentages (divided a bit to expand num types)
        pit_list[0].type = MONS_ZOMBIE_SMALL;
        pit_list[0].rare = 35;

        pit_list[1].type = MONS_ZOMBIE_LARGE;
        pit_list[1].rare = 25;

        pit_list[2].type = MONS_NECROPHAGE;
        pit_list[2].rare = 15;

        pit_list[3].type = MONS_GHOUL;
        pit_list[3].rare = 5;

        pit_list[4].type = MONS_WIGHT;
        pit_list[4].rare = 15;

        pit_list[5].type = MONS_WRAITH;
        pit_list[5].rare = 10;

        pit_list[6].type = MONS_FREEZING_WRAITH;
        pit_list[6].rare = 5;

        pit_list[7].type = MONS_SHADOW_WRAITH;
        pit_list[7].rare = 5;

        pit_list[8].type = MONS_VAMPIRE;
        pit_list[8].rare = 5;

        if (level_number > 19)
        {
            pit_list[9].type = MONS_VAMPIRE_KNIGHT;
            pit_list[9].rare = 5;
        }

        fill_monster_pit( sr, pit_list, 90, lord, lordx, lordy );
        break;

    case SROOM_JELLY_PIT:
#if DEBUG_DIAGNOSTICS
        mpr( MSGCH_DIAGNOSTICS, "Build: Jelly Pit" );
#endif
        pit_list[0].type = MONS_OOZE;
        pit_list[0].rare = 27 - level_number / 5;

        pit_list[1].type = MONS_JELLY;
        pit_list[1].rare = 20;

        pit_list[2].type = MONS_BROWN_OOZE;
        pit_list[2].rare = 3 + level_number;

        pit_list[3].type = MONS_DEATH_OOZE;
        pit_list[3].rare = 2 + (2 * level_number) / 3;

        if (level_number >= 12)
        {
            pit_list[4].type = MONS_AZURE_JELLY;
            pit_list[4].rare = 1 + (level_number - 12) / 3;
        }

        if (level_number >= 15)
        {
            pit_list[5].type = MONS_ACID_BLOB;
            pit_list[5].rare = 1 + (level_number - 15) / 4;
        }

        fill_monster_pit( sr, pit_list, 90, lord, lordx, lordy );
        break;

    case SROOM_TREASURY:
#if DEBUG_DIAGNOSTICS
        mpr( MSGCH_DIAGNOSTICS, "Build: Treasury (Guardian Naga)" );
#endif
        // XXX: Actually, this (like beehive) might be better off
        // being a minivault... leaving this function to only do
        // monster pits (which might eventually be more automated
        // and generalized). -- bwr
        //
        // should only appear in deep levels, with a guardian
        // Maybe have several types of treasure room?
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

                thing_created = make_item( 1, obj_type, OBJ_RANDOM, true,
                                           level_number + 10, 250 );

                if (thing_created != NON_ITEM)
                    move_item_to_grid( &thing_created, x, y );
            }
        }

        // place guardian {dlb}:
        mons_place( MONS_GUARDIAN_NAGA, BEH_GUARD, MHITNOT, true, lordx, lordy );
        break;

    default:
        break;
    }
}                               // end special_room()

static void build_minivaults( int level_number, int force_vault )
{
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

    // randomizing vaults here:
    if (force_vault == VAULT_MINIVAULT_1)
    {
        const int num_mini = VAULT_MINIVAULT_35 - VAULT_MINIVAULT_1 + 1;
        force_vault = VAULT_MINIVAULT_1 + random2( num_mini );
    }
    else if (force_vault == VAULT_RAND_DEMON_1)
    {
        const int num_rand = VAULT_RAND_DEMON_9 - VAULT_RAND_DEMON_1 + 1;
        force_vault = VAULT_RAND_DEMON_1 + random2( num_rand );
    }

    vault_main( vgrid, mons_array, force_vault, level_number );

    int vx, vy;
    int v1x, v1y;

    /* find a target area which can be safely overwritten: */
    for (;;)
    {
        if (one_chance_in(1000))
            return;

        v1x = 12 + random2(45);
        v1y = 12 + random2(35);

        for (vx = v1x; vx < v1x + 12; vx++)
        {
            for (vy = v1y; vy < v1y + 12; vy++)
            {
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
        for (vx = v1x - 1; vx <= v1x + 12; vx++)
        {
            for (vy = v1y - 1; vy <= v1y + 12; vy++)
            {
                if ((vy == v1y - 1 || vy == v1y + 12
                        || vx == v1x - 1 || vx == v1x + 12)
                    && (grd[vx][vy] == DNGN_FLOOR
                        || grd[vx][vy] == DNGN_CLOSED_DOOR
                        || grd[vx][vy] == DNGN_SECRET_DOOR))
                {
                    goto break_out;
                }
            }
        }

      out_of_check:
        continue;

      break_out:
        break;
    }

    // 8 symetries: horizonal, vertical, diagonal (2 * 2 * 2)
    bool flip_x = coinflip();
    bool flip_y = coinflip();
    bool flip_xy = coinflip();

    char fgrid[81][81];

    for (int x = 0; x < 12; x++)
    {
        vx = ((flip_x) ? 11 - x : x);

        for (int y = 0; y < 12; y++)
        {
            vy = ((flip_y) ? 11 - y : y);

            // Note that we're copying into the flip grid at the location
            // the vault will be placed (old version just copied onto
            // grd, but that doesn't work with the new paint_vault()).
            // Note also that the array is (y,x)... this is because that's
            // how the large vaults are stored, so we need to do the same
            // here so paint_vault will work.
            fgrid[v1y + y][v1x + x] = ((flip_xy) ? vgrid[vx][vy] : vgrid[vy][vx]);
        }
    }

    // these two are throwaways:
    int initial_x, initial_y;

    paint_vault( fgrid, v1x, v1y, v1x + 12, v1y + 12,
                 level_number, force_vault,
                 acq_item_class, mons_array, initial_x, initial_y );
}                               // end build_minivaults()

static void build_vaults( int level_number, int force_vault, bool down_stairs )
{
    // for some weird reason can't put a vault on level 1, because monster equip
    // isn't generated.
    int i,j;                // general loop variables
    int stx, sty;
    int initial_x = 0, initial_y = 0;

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

    int roomsss = 10 + random2(90);

    bool exclusive = (one_chance_in(10) ? false : true);

    //bool exclusive2 = coinflip();    // usage commented out below {dlb}
    //int item_made;

    char  vgrid[81][81];

    char gluggy = vault_main( vgrid, mons_array, force_vault, level_number );

    int v1x = 0, v1y = 0, v2x = 0, v2y = 0;

    int dig_dir_x = 0;
    int dig_dir_y = 0;
    int dig_place_x = 0;
    int dig_place_y = 0;

    // note: assumes *no* previous item (I think) or monster (definitely)
    // placement
    paint_vault( vgrid, 0, 0, GXM, GYM, level_number, force_vault,
                 acq_item_class, mons_array, initial_x, initial_y );

    switch (gluggy)
    {
    case MAP_ENCOMPASS:
        // map is a full level map, no further work
        return;

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

    }

    // Dis uses the city plan for the rest of the map so we do it here.
    if (force_vault == VAULT_CASTLE_DIS)
        plan_4( v1x, v1y, v2x, v2y, DNGN_METAL_WALL );
    else
    {
        // lay down the rest of the dungeon plan
        int  which_room = 0;
        int  cnx, cny;
        int  romx1[30], romy1[30], romx2[30], romy2[30];

        for (i = 0; i < roomsss; i++)
        {
            do
            {
                romx1[which_room] = 0;
                romy1[which_room] = 0;
                romx2[which_room] = 2 + random2(8);
                romy2[which_room] = 2 + random2(8);

                // This is the area we're going to clear, so it's in_bounds().
                random_place_rectangle( romx1[which_room], romy1[which_room],
                                        romx2[which_room], romy2[which_room] );
            }
            while (rectangles_overlap( romx1[which_room], romy1[which_room],
                                       romx2[which_room], romy2[which_room],
                                       v1x, v1y, v2x, v2y )); // inclusively

            if (i == 0)
            {
                join_the_dots( initial_x, initial_y,
                               romx1[which_room], romy1[which_room],
                               v1x, v1y, v2x, v2y );
            }
            else if (exclusive)
            {
                // Since the random_place_rectangle is in_bounds() these
                // checks go no farther than the map_bounds()
                for (cnx = romx1[which_room] - 1; cnx <= romx2[which_room] + 1; cnx++)
                {
                    for (cny = romy1[which_room] - 1; cny <= romy2[which_room] + 1; cny++)
                    {
                        if (grd[cnx][cny] != DNGN_ROCK_WALL)
                            goto continuing;    // continue the outer room loop
                    }
                }
            }

            replace_area( romx1[which_room], romy1[which_room],
                          romx2[which_room], romy2[which_room],
                          DNGN_ROCK_WALL, DNGN_FLOOR );

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

        dig_place_x = initial_x;
        dig_place_y = initial_y;

        for (i = 0; i < 40; i++)
        {
            dig_place_x += dig_dir_x;
            dig_place_y += dig_dir_y;

            if (!in_bounds( dig_place_x, dig_place_y ))
                break;

            if (grd[dig_place_x][dig_place_y] == DNGN_ROCK_WALL)
                grd[dig_place_x][dig_place_y] = DNGN_FLOOR;
        }
    }

    // doing stairs:
    int pos_x, pos_y;
    FixedVector< bool, 8 > stair_exist;

    // first we need to check which stairs are inside the vault
    for (stx = 0; stx < 8; stx++)
        stair_exist[stx] = false;

    for (stx = X_BOUND_1 + 1; stx < X_BOUND_2; stx++)
    {
        for (sty = Y_BOUND_1 + 1; sty < Y_BOUND_2; sty++)
        {
            if (grd[stx][sty] >= DNGN_STONE_STAIRS_DOWN_I
                && grd[stx][sty] <= DNGN_ROCK_STAIRS_UP)
            {
                stair_exist[ grd[stx][sty] - DNGN_STONE_STAIRS_DOWN_I ] = true;
            }
        }
    }

    // add any remaining stairs
    const int num_stairs_pairs = (coinflip() ? 4 : 3);

    for (j = 0; j < num_stairs_pairs; j++)
    {
        // skip down_stairs unless they're allowed
        for (i = ((down_stairs) ? 0 : 1); i < 2; i++)
        {
            const int indx = (i * 4) + j;

            if (stair_exist[ indx ])
                continue;

            // get location that's not in the vault:
            do
            {
                random_in_bounds( pos_x, pos_y );
            }
            while (in_rectangle( pos_x, pos_y, v1x, v1y, v2x, v2y ));

            grd[pos_x][pos_y] = DNGN_STONE_STAIRS_DOWN_I + indx;
        }
    }
}                               // end build_vaults()

static int make_demonlord_rune( int demonlord )
{
    int ret = NON_ITEM;
    int rune_type = RUNE_NONE;

    switch (demonlord)
    {
    case MONS_PANDEMONIUM_DEMON:
        if (random2(20) < 3)            // this is approximately the old rate
            rune_type = RUNE_DEMONIC;
        break;

    case MONS_MNOLEG:
        rune_type = RUNE_MNOLEG;
        break;
    case MONS_LOM_LOBON:
        rune_type = RUNE_LOM_LOBON;
        break;
    case MONS_CEREBOV:
        rune_type = RUNE_CEREBOV;
        break;
    case MONS_GLOORX_VLOQ:
        rune_type = RUNE_GLOORX_VLOQ;
        break;

    case MONS_DISPATER:
        rune_type = RUNE_DIS;
        break;
    case MONS_ASMODEUS:
        rune_type = RUNE_GEHENNA;
        break;
    case MONS_ANTAEUS:
        rune_type = RUNE_COCYTUS;
        break;
    case MONS_ERESHKIGAL:
        rune_type = RUNE_TARTARUS;
        break;

    default:
        break;
    }

    if (rune_type != RUNE_NONE)
        ret = make_item( 1, OBJ_MISCELLANY, MISC_RUNE_OF_ZOT, true, 0, rune_type );

    return (ret);
}

// returns true if possible rune site.
static void vault_grid( int vault, int level_number, int vx, int vy,
                        int &altar_count,
                        const FixedVector < char, 7 > &acq_item_class,
                        const FixedVector < int, 7 > &mons_array,
                        char vgrid, int &initial_x, int &initial_y )
{
    // first, set base tile for grids {dlb}:
    grd[vx][vy] = ((vgrid == 'x') ? DNGN_ROCK_WALL :
                   (vgrid == 'X') ? DNGN_PERMAROCK_WALL :
                   (vgrid == 'c') ? DNGN_STONE_WALL :
                   (vgrid == 'v') ? DNGN_METAL_WALL :
                   (vgrid == 'b') ? DNGN_GREEN_CRYSTAL_WALL :
                   (vgrid == 'a') ? DNGN_WAX_WALL :
                   (vgrid == '+') ? DNGN_CLOSED_DOOR :
                   (vgrid == '=') ? DNGN_SECRET_DOOR :
                   (vgrid == 's') ? DNGN_SHALLOW_WATER :
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

                   (vgrid == 'F') ? (one_chance_in(100)
                                     ? (coinflip() ? DNGN_SILVER_STATUE
                                                   : DNGN_ORANGE_CRYSTAL_STATUE)
                                     : DNGN_GRANITE_STATUE) :

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
        place_specific_trap( vx, vy, TRAP_RANDOM, true );
        break;
    case '&':
        place_specific_trap( vx, vy, TRAP_NONTELEPORT, true );
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
    case 'O':                   // rune site
    case 'Z':                   // definite orb
        {
            int item_made = NON_ITEM;
            unsigned char which_class = OBJ_RANDOM;
            unsigned char which_type = OBJ_RANDOM;
            int which_depth;
            int spec = 250;

            if (vgrid == 'R')
            {
                which_class = OBJ_FOOD;
                which_type = (one_chance_in(10) ? FOOD_ROYAL_JELLY
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
            else if (vgrid == '|')
            {
                which_class = acq_item_class[random2(7)];
                which_type = OBJ_RANDOM;
            }
            else if (vgrid == 'O')
            {
                which_class = OBJ_MISCELLANY;
                which_type  = MISC_RUNE_OF_ZOT;

                // get type of rune:
                if (you.level_type == LEVEL_PANDEMONIUM)
                {
                    if (vault >= VAULT_MNOLEG && vault <= VAULT_GLOORX_VLOQ)
                        spec = vault;
                    else
                        spec = RUNE_DEMONIC;
                }
                else if (you.level_type == LEVEL_ABYSS)
                    spec = RUNE_ABYSSAL;
                else
                    spec = you.where_are_you;
            }

            which_depth = ((vgrid == '|'
                            || vgrid == 'O'
                            || vgrid == 'Z') ? MAKE_GOOD_ITEM :
                           (vgrid == '*')    ? 5 + (level_number * 2)
                                             : level_number);

            item_made = make_item( 1, which_class, which_type, true,
                                   which_depth, spec );

            if (item_made != NON_ITEM)
                move_item_to_grid( &item_made, vx, vy );

            int trap = 5;

            appearance_type  fancy = item_appearance( mitm[item_made] );
            if (fancy == APP_FANCY)
                trap += 5;
            else if (fancy == APP_SPECIAL)
                trap += 10;

            if (random2(100) < trap)
                place_specific_trap( vx, vy, TRAP_NONTELEPORT, true );
        }
        break;
    }

    // finally, handle grids that place monsters {dlb}:
    if (isdigit( vgrid ))
    {
        int mon_id;
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

        place_monster( mon_id, monster_type_thing, monster_level, BEH_GUARD,
                       MHITNOT, true, vx, vy, false );

        if (monster_type_thing != RANDOM_MONSTER && mon_id != -1)
        {
            int rune = make_demonlord_rune( monster_type_thing );

            if (rune != NON_ITEM)
                menv[mon_id].inv[MSLOT_MISCELLANY] = rune;
        }
    }
    return;
}                               // end vault_grid()

// Paints vault from vgrid onto the grd at (sx,sy)-(ex,ey), providing
// monsters and items as required.
static void paint_vault( char vgrid[81][81],
                         int sx, int sy, int ex, int ey,
                         int level_number, int force_vault,
                         const FixedVector< char, 7 > &acq_item_class,
                         const FixedVector< int, 7 > &mons_array,
                         int &initial_x, int &initial_y )
{
    int altar_count = 0;

    // paint the vault onto the grid
    for (int vx = sx; vx < ex; vx++)
    {
        for (int vy = sy; vy < ey; vy++)
        {
            vault_grid( force_vault, level_number, vx, vy, altar_count,
                        acq_item_class, mons_array, vgrid[vy][vx],
                        initial_x, initial_y );
        }
    }
}

static void replace_area( int sx, int sy, int ex, int ey,
                          unsigned char replace, unsigned char feature )
{
    for (int x = sx; x <= ex; x++)
    {
        for (int y = sy; y <= ey; y++)
        {
            if (grd[x][y] == replace)
                grd[x][y] = feature;
        }
    }
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
    if (pool_type == DNGN_LAVA
        && (player_in_branch(BRANCH_CRYPT) || player_in_branch(BRANCH_TOMB)))
    {
        pool_type = DNGN_SHALLOW_WATER;
    }

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

static void many_pools( unsigned char pool_type )
{
    int pools = 0;
    int i = 0, j = 0, k = 0, l = 0;
    int m = 0, n = 0;
    int no_pools = 20 + roll_zdice(2,5);
    int timeout = 0;

    if (player_in_branch( BRANCH_COCYTUS ))
        pool_type = DNGN_DEEP_WATER;
    else if (player_in_branch( BRANCH_GEHENNA ))
        pool_type = DNGN_LAVA;

#if DEBUG_DIAGNOSTICS
    mpr( MSGCH_DIAGNOSTICS, "Build: %spools ",
          (pool_type == DNGN_LAVA) ? "lava " : "water " );
#endif

    do
    {
        timeout++;

        if (timeout >= 10000)
            break;

        i = 0;
        j = 0;
        k = 2 + roll_dice(2,9);
        l = 2 + roll_dice(2,9);
        random_place_rectangle( i, j, k, l );

        for (m = i; m < k; m++)
        {
            for (n = j; n < l; n++)
            {
                if (grd[m][n] != DNGN_FLOOR)
                    goto continue_pools;
            }
        }

        place_pool( pool_type, i, j, k, l );
        pools++;

      continue_pools:
        continue;
    }
    while (pools < no_pools);
}                               // end many_pools()

static int brighten_colour( int colour )
{
    return ((colour < DARKGREY) ? (colour + 8) : colour);
}

void item_colour( item_def &item )
{
    int switchnum = 0;
    int temp_value;

    switch (item.base_type)
    {
    case OBJ_WEAPONS:
        if (is_unrandom_artefact( item ))
            return;

        if (is_fixed_artefact( item ))
        {
            switch (item.special)   // was: - 180, but that is *wrong* {dlb}
            {
            case SPWPN_SINGING_SWORD:
            case SPWPN_SCEPTRE_OF_TORMENT:
                item.colour = YELLOW;
                break;
            case SPWPN_WRATH_OF_TROG:
            case SPWPN_SWORD_OF_POWER:
                item.colour = RED;
                break;
            case SPWPN_SCYTHE_OF_CURSES:
                item.colour = DARKGREY;
                break;
            case SPWPN_MACE_OF_VARIABILITY:
                item.colour = element_colour( EC_RANDOM );
                break;
            case SPWPN_GLAIVE_OF_PRUNE:
                item.colour = MAGENTA;
                break;
            case SPWPN_SWORD_OF_ZONGULDROK:
                item.colour = LIGHTGREY;
                break;
            case SPWPN_KNIFE_OF_ACCURACY:
                item.colour = LIGHTCYAN;
                break;
            case SPWPN_STAFF_OF_OLGREB:
                item.colour = GREEN;
                break;
            case SPWPN_VAMPIRES_TOOTH:
                item.colour = WHITE;
                break;
            case SPWPN_STAFF_OF_WUCAD_MU:
                item.colour = BROWN;
                break;
            }
            return;
        }

        if (is_demonic( item ))
            item.colour = element_colour( EC_RANDOM );
        else if (is_range_weapon( item ))
            item.colour = BROWN;
        else
        {
            switch (item.sub_type)
            {
            case WPN_CLUB:
            case WPN_GIANT_CLUB:
            case WPN_GIANT_SPIKED_CLUB:
            case WPN_ANCUS:
            case WPN_WHIP:
            case WPN_QUARTERSTAFF:
            case WPN_STAFF:
                item.colour = BROWN;
                break;
            case WPN_QUICK_BLADE:
                item.colour = LIGHTBLUE;
                break;
            case WPN_EXECUTIONERS_AXE:
                item.colour = RED;
                break;
            default:
                item.colour = LIGHTCYAN;
                if (get_equip_race( item ) == ISFLAG_DWARVEN)
                    item.colour = CYAN;
                break;
            }
        }
        break;

    case OBJ_MISSILES:
        switch (item.sub_type)
        {
        case MI_STONE:
            item.colour = LIGHTGREY;
            break;
        case MI_LARGE_ROCK:
        case MI_ARROW:
            item.colour = BROWN;
            break;
        case MI_NEEDLE:
            item.colour = WHITE;
            break;
        default:
            item.colour = LIGHTCYAN;
            if (get_equip_race( item ) == ISFLAG_DWARVEN)
                item.colour = CYAN;
            break;
        }
        break;

    case OBJ_ARMOUR:
        if (is_unrandom_artefact( item ))
            return;              /* unrandarts have already been coloured */

        switch (item.sub_type)
        {
        case ARM_CLOAK:
        case ARM_ROBE:
        case ARM_NAGA_BARDING:
        case ARM_CENTAUR_BARDING:
        case ARM_CAP:
            item.colour = element_colour( race_colour(get_equip_race(item)) );
            break;

        case ARM_HELMET:
            item.colour = LIGHTCYAN;
            if (get_equip_race(item) == ISFLAG_DWARVEN)
                item.colour = CYAN;
            break;

        case ARM_BOOTS: // maybe more interesting boot colours?
        case ARM_GLOVES:
        case ARM_LEATHER_ARMOUR:
        case ARM_STUDDED_LEATHER_ARMOUR:
            item.colour = BROWN;
            break;

        case ARM_CRYSTAL_PLATE_MAIL:
            item.colour = element_colour( EC_CRYSTAL );
            break;

        case ARM_DRAGON_HIDE:
        case ARM_DRAGON_ARMOUR:
            item.colour = mons_class_colour( MONS_DRAGON );
            break;
        case ARM_TROLL_HIDE:
        case ARM_TROLL_LEATHER_ARMOUR:
            item.colour = mons_class_colour( MONS_TROLL );
            break;
        case ARM_ICE_DRAGON_HIDE:
        case ARM_ICE_DRAGON_ARMOUR:
            item.colour = mons_class_colour( MONS_ICE_DRAGON );
            break;
        case ARM_STEAM_DRAGON_HIDE:
        case ARM_STEAM_DRAGON_ARMOUR:
            item.colour = mons_class_colour( MONS_STEAM_DRAGON );
            break;
        case ARM_MOTTLED_DRAGON_HIDE:
        case ARM_MOTTLED_DRAGON_ARMOUR:
            item.colour = mons_class_colour( MONS_MOTTLED_DRAGON );
            break;
        case ARM_STORM_DRAGON_HIDE:
        case ARM_STORM_DRAGON_ARMOUR:
            item.colour = mons_class_colour( MONS_STORM_DRAGON );
            break;
        case ARM_GOLD_DRAGON_HIDE:
        case ARM_GOLD_DRAGON_ARMOUR:
            item.colour = mons_class_colour( MONS_GOLDEN_DRAGON );
            break;
        case ARM_ANIMAL_SKIN:
            item.colour = BROWN;
            break;
        case ARM_SWAMP_DRAGON_HIDE:
        case ARM_SWAMP_DRAGON_ARMOUR:
            item.colour = mons_class_colour( MONS_SWAMP_DRAGON );
            break;
        default:
            item.colour = LIGHTCYAN;
            if (get_equip_race(item) == ISFLAG_DWARVEN)
                item.colour = CYAN;
            break;
        }
        break;

    case OBJ_WANDS:
        item.special = you.item_description[IDESC_WANDS][item.sub_type];

        switch (item.special % NUM_WANDS_MAT)
        {
        case 0:         //"iron wand"
            item.colour = CYAN;
            break;
        case 1:         //"brass wand"
        case 5:         //"gold wand"
            item.colour = YELLOW;
            break;
        case 9:         //"glass wand"
            item.colour = LIGHTCYAN;
            break;
        case 6:         //"silver wand"
        default:
            item.colour = LIGHTGREY;
            break;
        case 10:        //"lead wand"
            item.colour = DARKGREY;
            break;
        case 3:         //"wooden wand"
        case 4:         //"copper wand"
        case 7:         //"bronze wand"
            item.colour = BROWN;
            break;
        case 2:         //"bone wand"
        case 8:         //"ivory wand"
            item.colour = WHITE;
            break;
        case 11:        //"plastic wand"
            item.colour = element_colour( EC_RANDOM );
            break;
        }

        switchnum = item.special / NUM_WANDS_MAT;

        if (switchnum == 9)                     // blackened
            item.colour = DARKGREY;
        else if (switchnum == 11)               // glowing
            item.colour = brighten_colour( item.colour );

        // rare wands (eg disintegration - these will be very rare):
        // maybe only 1 thing, like: crystal, shining, etc.
        break;

    case OBJ_POTIONS:
        item.special = you.item_description[IDESC_POTIONS][item.sub_type];

        switch (item.special % NUM_POTIONS_MAT)
        {
        case 0:         //"clear potion"
        case 3:         //"silvery potion"
        default:
            item.colour = LIGHTGREY;
            break;
        case 1:         //"blue potion"
        case 7:         //"inky potion"
            item.colour = BLUE;
            break;
        case 2:         //"black potion"
            item.colour = DARKGREY;
            break;
        case 13:        //"white potion"
            item.colour = WHITE;
            break;
        case 4:         //"cyan potion"
            item.colour = CYAN;
            break;
        case 5:         //"purple potion"
            item.colour = MAGENTA;
            break;
        case 6:         //"orange potion"
            item.colour = LIGHTRED;
            break;
        case 8:         //"red potion"
            item.colour = RED;
            break;
        case 9:         //"yellow potion"
            item.colour = YELLOW;
            break;
        case 10:        //"green potion"
            item.colour = GREEN;
            break;
        case 11:        //"brown potion"
            item.colour = BROWN;
            break;
        case 12:        //"pink potion"
            item.colour = LIGHTMAGENTA;
            break;
        }

        switchnum = item.special / NUM_POTIONS_MAT;

        if (switchnum == 6 || switchnum == 15)        // glowing/effulgent
            item.colour = brighten_colour( item.colour );
        break;

    case OBJ_FOOD:
        switch (item.sub_type)
        {
        case FOOD_BEEF_JERKY:
        case FOOD_BREAD_RATION:
        case FOOD_LYCHEE:
        case FOOD_MEAT_RATION:
        case FOOD_RAMBUTAN:
        case FOOD_SAUSAGE:
        case FOOD_SULTANA:
            item.colour = BROWN;
            break;
        case FOOD_BANANA:
        case FOOD_CHEESE:
        case FOOD_HONEYCOMB:
        case FOOD_LEMON:
        case FOOD_PIZZA:
        case FOOD_ROYAL_JELLY:
            item.colour = YELLOW;
            break;
        case FOOD_PEAR:
            item.colour = LIGHTGREEN;
            break;
        case FOOD_CHOKO:
        case FOOD_SNOZZCUMBER:
            item.colour = GREEN;
            break;
        case FOOD_APRICOT:
        case FOOD_ORANGE:
            item.colour = LIGHTRED;
            break;
        case FOOD_STRAWBERRY:
            item.colour = RED;
            break;
        case FOOD_APPLE:
            item.colour = (coinflip() ? RED : GREEN);
            break;
        case FOOD_GRAPE:
            item.colour = (coinflip() ? MAGENTA : GREEN);
            break;
        case FOOD_CHUNK:
            // set the appropriate colour of the meat:
            temp_value = mons_class_colour( item.plus );
            item.colour = (temp_value == BLACK) ? LIGHTRED : temp_value;
            break;
        default:
            item.colour = BROWN;
        }
        break;

    case OBJ_JEWELLERY:
        /* unrandarts have already been coloured */
        if (is_unrandom_artefact( item ))
            return;

        item.colour = YELLOW;

        if (is_random_artefact( item ))
            switchnum = randart_ring_material( item );
        else
        {
            item.special = you.item_description[IDESC_RINGS][item.sub_type];
            switchnum = item.special % NUM_RINGS_MAT;
        }

        if (jewellery_is_amulet( item ))
        {
            switch (switchnum)
            {
            case 0:             //"zirconium amulet"
            case 9:             //"ivory amulet"
            case 11:            //"platinum amulet"
                item.colour = WHITE;
                break;
            case 1:             //"sapphire amulet"
                item.colour = LIGHTBLUE;
                break;
            case 2:             //"gold amulet"
            case 6:             //"brass amulet"
            case 15:            // electrum
                item.colour = YELLOW;
                break;
            case 3:             //"emerald amulet"
                item.colour = GREEN;
                break;
            case 4:             //"garnet amulet"
            case 8:             //"ruby amulet"
                item.colour = RED;
                break;
            case 5:             //"bronze amulet"
            case 7:             //"copper amulet"
                item.colour = BROWN;
                break;
            case 10:            //"bone amulet"
                item.colour = LIGHTGREY;
                break;
            case 12:            //"jade amulet"
                item.colour = GREEN;
                break;
            case 13:            //"plastic amulet"
                item.colour = element_colour( EC_RANDOM );
                break;
            case 14:            // orichalcum
                item.colour = LIGHTRED;
                break;
            }
        }
        else // ring
        {
            switch (switchnum)
            {
            case 0:         // wooden
            case 5:         // bronze
            case 7:         // copper
                item.colour = BROWN;
                break;
            case 1:         // silver
            case 8:         // granite
            case 11:        // marble
                item.colour = LIGHTGREY;
                break;
            case 2:         // gold
            case 6:         // brass
                item.colour = YELLOW;
                break;
            case 3:         // iron
                item.colour = CYAN;
                break;
            case 9:         // ivory
            case 10:        // bone
                item.colour = WHITE;
                break;
            case 12:        // jade
                item.colour = GREEN;
                break;
            case 4:         // steel
            case 13:        // glass
                item.colour = LIGHTCYAN;
                break;
            case 14:        // lapis lazuli
                item.colour = BLUE;
                break;
            case 15:        // obsidian
                item.colour = DARKGREY;
                break;
            }
        }

        if (!is_random_artefact( item ))
        {
            switchnum = item.special / NUM_RINGS_MAT;

            // blackened - same for both rings and amulets
            if (switchnum == 5)
                item.colour = DARKGREY;

            // glowing
            if (jewellery_is_amulet( item ))
            {
                if (switchnum == 6)
                    item.colour = brighten_colour( item.colour );
            }
            else
            {
                if (switchnum == 2)
                    item.colour = brighten_colour( item.colour );
            }
        }
        break;

    case OBJ_SCROLLS:
        item.colour = WHITE;

        if (item.sub_type != SCR_RANDOM_USELESSNESS)
            item.special = you.item_description[IDESC_SCROLLS][item.sub_type];
        else
        {
            // Only a few different names to encourage some stacking.
            item.special = you.item_description[IDESC_SCROLLS][item.sub_type]
                            + roll_zdice( 2, 3 );
        }
        break;

    case OBJ_BOOKS:
        switch (item.special % NUM_BOOKS_MAT)
        {
        case 0:         // just "book"
        case 1:         // hardcover
        case 6:         // paperback
        default:
            item.colour = element_colour( EC_RANDOM );
            break;
        case 2:         // leatherbound
            item.colour = (!one_chance_in(3) ? BROWN : DARKGREY);
            break;
        case 3:         // metal-bound
            item.colour = CYAN;
            break;
        case 4:         // papyrus
            item.colour = LIGHTGREY;
            break;
        }

        if (item.special / NUM_BOOKS_MAT == 5)  // glowing
            item.colour = brighten_colour( item.colour );
        break;

    case OBJ_STAVES:
        item.colour = BROWN;
        break;

    case OBJ_ORBS:
        item.colour = LIGHTMAGENTA;
        break;

    case OBJ_MISCELLANY:
        switch (item.sub_type)
        {
        case MISC_ROD_OF_STRIKING:
        case MISC_BOTTLED_EFREET:
        case MISC_STONE_OF_EARTH_ELEMENTALS:
            item.colour = BROWN;
            break;

        case MISC_AIR_ELEMENTAL_FAN:
        case MISC_CRYSTAL_BALL_OF_ENERGY:
        case MISC_CRYSTAL_BALL_OF_FIXATION:
        case MISC_CRYSTAL_BALL_OF_SEEING:
        case MISC_DISC_OF_STORMS:
        case MISC_HORN_OF_GERYON:
        case MISC_LANTERN_OF_SHADOWS:
            item.colour = LIGHTGREY;
            break;

        case MISC_LAMP_OF_FIRE:
            item.colour = YELLOW;
            break;

        case MISC_BOX_OF_BEASTS:
            item.colour = DARKGREY;
            break;

        case MISC_RUNE_OF_ZOT:
            switch (item.plus)
            {
            case RUNE_DIS:                      // iron
                item.colour = CYAN;
                break;

            case RUNE_COCYTUS:                  // icy
                item.colour = element_colour( EC_ICE );
                break;

            case RUNE_TARTARUS:                 // bone
                item.colour = WHITE;
                break;

            case RUNE_SLIME_PITS:               // slimy
                item.colour = element_colour( EC_SLIME );
                break;

            case RUNE_ELVEN_HALLS:              // elven
                item.colour = element_colour( EC_ELVEN );
                break;

            case RUNE_SNAKE_PIT:                // serpentine
                item.colour = LIGHTGREEN;
                break;

            case RUNE_VAULTS:                   // silver
                item.colour = LIGHTGREY;
                break;

            case RUNE_TOMB:                     // golden
                item.colour = YELLOW;
                break;

            case RUNE_SWAMP:                    // decaying
                item.colour = BROWN;
                break;

            // These two are hardly unique, but since colour isn't used for
            // stacking, so we don't have to worry to much about this. -- bwr
            case RUNE_DEMONIC:             // random pandemonium demonlords
            case RUNE_ABYSSAL:             // random in abyss
            default:
                item.colour = element_colour( EC_RANDOM );
                break;

            case RUNE_MNOLEG:                   // glowing
                item.colour = element_colour( EC_MUTAGENIC );
                break;

            case RUNE_LOM_LOBON:                // magical
                item.colour = element_colour( EC_MAGIC );
                break;

            case RUNE_CEREBOV:                  // fiery
                item.colour = element_colour( EC_FIRE );
                break;

            case RUNE_GLOORX_VLOQ:              // dark
                item.colour = element_colour( EC_DARK );
                break;

            case RUNE_GEHENNA:                  // obsidian
                item.colour = DARKGREY;
                break;
            }
            break;

        case MISC_EMPTY_EBONY_CASKET:
            item.colour = DARKGREY;
            break;

        case MISC_DECK_OF_SUMMONINGS:
        case MISC_DECK_OF_WONDERS:
        case MISC_DECK_OF_TRICKS:
        case MISC_DECK_OF_POWER:
        default:
            item.colour = element_colour( EC_RANDOM );
            break;
        }
        break;

    case OBJ_CORPSES:
        // set the appropriate colour of the body:
        temp_value = mons_class_colour( item.plus );
        item.colour = (temp_value == BLACK) ? LIGHTRED : temp_value;
        break;

    case OBJ_GOLD:
        item.colour = YELLOW;
        break;
    }

    // Clobber special looking artefacts sometimes:
    if (is_random_artefact( item ) && item_appearance( item ) == APP_SPECIAL)
    {
        const int sugg = randart_suggested_colour( item );

        // Use suggested colours most of the time, else fall back
        // on the old chance of any random colour (artefact jewellery
        // we leave alone more often because the colour has already
        // been selected to match the material above).
        if (sugg != BLACK && !one_chance_in(10))
            item.colour = sugg;
        else if ((item.base_type != OBJ_JEWELLERY && one_chance_in(5))
                || one_chance_in(10))
        {
            item.colour = element_colour( EC_RANDOM );
        }
    }
}                               // end item_colour()

//jmf: generate altar based on where you are, or possibly randomly
static int pick_an_altar(void)
{
    int altar_type = 0;
    int temp_rand;              // probability determination {dlb}

    if (player_in_branch( BRANCH_SLIME_PITS )
        || player_in_branch( BRANCH_ECUMENICAL_TEMPLE )
        || you.level_type == LEVEL_LABYRINTH)
    {
        // no extra altars in temple, none at all in slime pits or labyrinth
        altar_type = DNGN_FLOOR;
    }
    else if (you.level_type == LEVEL_DUNGEON && !one_chance_in(5))
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
        // Note: this case includes the pandemonium or the abyss.
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

    return (altar_type);
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

static void place_shops( int level_number )
{
    const int tmp = random2(100);
    const int no_shops = (tmp >= 24) ? 0 :                       // 76%
                         (tmp >=  4) ? 1 :                       // 20%
                         (tmp >=  2) ? roll_dice(1,3)            //  2%
                                     : roll_dice(1,3) + random2(MAX_SHOPS - 2); //  2%

    if (no_shops == 0 || level_number < 3)
        return;

#if DEBUG_DIAGNOSTICS
    mpr( MSGCH_DIAGNOSTICS, "Build: %d shops", no_shops );
#endif

    for (int i = 0; i < no_shops; i++)
    {
        int shop_x = 0, shop_y = 0;

        random_in_bounds( shop_x, shop_y );
        place_spec_shop( level_number, shop_x, shop_y, SHOP_RANDOM );
    }
}                               // end place_shops()

void place_spec_shop( int level_number,
                      int shop_x, int shop_y,
                      shop_type force_type )
{
    int obj = 0;
    int i = 0;
    int j = 0;                  // loop variable
    int item_level;

    bool have_food = false;

    // NOTE: assuming shops are always filled from the start!
    for (i = 0; i < MAX_SHOPS; i++)
    {
        if (env.shop[i].type == SHOP_UNASSIGNED)
            break;
        else if (env.shop[i].type == SHOP_FOOD)
            have_food = true;
    }

    if (i == MAX_SHOPS)
        return;

    char buff[ ITEMNAME_SIZE ];
    make_name( random_int(), false, buff );

    strncpy( env.shop[i].keeper_name, buff, kNameLen );
    env.shop[i].keeper_name[ kNameLen - 1 ] = '\0';

    if (force_type != SHOP_RANDOM)
        env.shop[i].type = force_type;
    else if (level_number < 10 && !have_food && one_chance_in(3))
        env.shop[i].type = SHOP_FOOD;
    else
        env.shop[i].type = random2( NUM_SHOPS );

    if (env.shop[i].type == SHOP_WEAPON_ANTIQUE
            || env.shop[i].type == SHOP_ARMOUR_ANTIQUE
            || env.shop[i].type == SHOP_GENERAL_ANTIQUE)
    {
        // Base prices for antique shop are low because the items
        // are unidentified...  we compensate by being very greedy.
        env.shop[i].greed = 15 + roll_dice(2,10) + random2( level_number );
        env.shop[i].level = 10 + level_number + random2( level_number * 2 );
    }
    else
    {
        env.shop[i].greed = 10 + random2(5) + random2( level_number );
        env.shop[i].level = 10 + level_number + random2( level_number );
    }

    int num_items = 6 + roll_dice(2,5);

    if (num_items > MAX_SHOP_ITEMS)
        num_items = MAX_SHOP_ITEMS;

    for (j = 0; j < num_items; j++)
    {
        if (one_chance_in(3))
            item_level = MAKE_GOOD_ITEM;
        else
            item_level = env.shop[i].level + random2(11) - 5;

        // don't generate gold in shops!  This used to be possible with
        // General Stores (see item_in_shop() below)   (GDL)
        while (true)
        {
            obj = make_item( 1, item_in_shop( env.shop[i].type ),
                             OBJ_RANDOM, true, item_level, 250 );

            if (obj != NON_ITEM
                && mitm[obj].base_type != OBJ_GOLD
                && (env.shop[i].type != SHOP_GENERAL_ANTIQUE
                    || mitm[obj].base_type != OBJ_FOOD))
            {
                break;
            }

            // reset object and try again
            if (obj != NON_ITEM)
                invalidate_item( mitm[obj] );
        }

        // stop if we're having difficulty making new objects
        if (obj == NON_ITEM)
            break;

        // make sure there's only one (except missiles and food (ie grapes))
        if (mitm[obj].base_type != OBJ_MISSILES
            && mitm[obj].base_type != OBJ_FOOD)
        {
            mitm[obj].quantity = 1;
        }

        // set object 'position' (gah!) & ID status
        mitm[obj].x = 0;
        mitm[obj].y = 5 + i;

        if (env.shop[i].type != SHOP_WEAPON_ANTIQUE
            && env.shop[i].type != SHOP_ARMOUR_ANTIQUE
            && env.shop[i].type != SHOP_GENERAL_ANTIQUE)
        {
            set_ident_flags( mitm[obj], ISFLAG_IDENT_MASK );
        }
    }

    env.shop[i].x = static_cast<unsigned char>( shop_x );
    env.shop[i].y = static_cast<unsigned char>( shop_y );

    grd[shop_x][shop_y] = DNGN_ENTER_SHOP;
}                               // end place_spec_shop()

static unsigned char item_in_shop(unsigned char shop_type)
{
    switch (shop_type)
    {
    case SHOP_WEAPON:
    case SHOP_WEAPON_ANTIQUE:
        return (one_chance_in(5) ? OBJ_MISSILES : OBJ_WEAPONS);

    case SHOP_ARMOUR:
    case SHOP_ARMOUR_ANTIQUE:
        return (OBJ_ARMOUR);

    case SHOP_GENERAL:
    case SHOP_GENERAL_ANTIQUE:
        return (OBJ_RANDOM);

    case SHOP_JEWELLERY:
        return (OBJ_JEWELLERY);

    case SHOP_WAND:
        return (OBJ_WANDS);

    case SHOP_BOOK:
        return (OBJ_BOOKS);

    case SHOP_FOOD:
        return (OBJ_FOOD);

    case SHOP_DISTILLERY:
        return (OBJ_POTIONS);

    case SHOP_SCROLL:
        return (OBJ_SCROLLS);
    }

    return (OBJ_RANDOM);
}                               // end item_in_shop()

static void spotty_level(bool seeded, int iterations, bool boxy)
{
    // assumes starting with a level full of rock walls (1)
    int i, j, k, l;

    if (!seeded)
    {
        for (i = DNGN_STONE_STAIRS_DOWN_I; i < DNGN_ROCK_STAIRS_UP; i++)
        {
            if (i == DNGN_ROCK_STAIRS_DOWN
                || (i == DNGN_STONE_STAIRS_UP_I
                    && !player_in_branch( BRANCH_SLIME_PITS )))
            {
                continue;
            }

            do
            {
                // we wand this to be +1 away from the edge
                j = X_BOUND_1 + 2 + random2( X_WIDTH - 4 );
                k = Y_BOUND_1 + 2 + random2( Y_WIDTH - 4 );
            }
            while (grd[j][k] != DNGN_ROCK_WALL
                    && grd[j + 1][k] != DNGN_ROCK_WALL);

            grd[j][k] = i;

            // creating elevators
            if (i == DNGN_STONE_STAIRS_DOWN_I
                && !player_in_branch( BRANCH_SLIME_PITS ))
            {
                grd[j + 1][k] = DNGN_STONE_STAIRS_UP_I;
            }

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

    // boxy levels have more clearing, so they get fewer iterations:
    if (l == 0)
        l = 200 + random2( (boxy ? 750 : 1500) );

    for (i = 0; i < l; i++)
    {
        do
        {
            // we wand this to be +2 away from the edge
            j = X_BOUND_1 + 3 + random2( X_WIDTH - 6 );
            k = Y_BOUND_1 + 3 + random2( Y_WIDTH - 6 );
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
                grd[j - 1][k - 1] = DNGN_FLOOR;
            if (grd[j + 1][k + 1] == DNGN_ROCK_WALL)
                grd[j + 1][k + 1] = DNGN_FLOOR;
            if (grd[j - 1][k + 1] == DNGN_ROCK_WALL)
                grd[j - 1][k + 1] = DNGN_FLOOR;
            if (grd[j + 1][k - 1] == DNGN_ROCK_WALL)
                grd[j + 1][k - 1] = DNGN_FLOOR;
        }
    }
}                               // end spotty_level()

static void bigger_room(void)
{
    unsigned char i, j;

    for (i = X_BOUND_1 + 1; i < X_BOUND_2; i++)
    {
        for (j = Y_BOUND_1 + 1; j < Y_BOUND_2; j++)
        {
            if (grd[i][j] == DNGN_ROCK_WALL)
                grd[i][j] = DNGN_FLOOR;
        }
    }

    many_pools(DNGN_DEEP_WATER);

    if (one_chance_in(3))
    {
        if (coinflip())
            build_river( DNGN_DEEP_WATER );
        else
            build_lake( DNGN_DEEP_WATER );
    }

    int pair_count = coinflip() ? 4 : 3;

    for (j = 0; j < pair_count; j++)
    {
        for (i = 0; i < 2; i++)
        {
            place_specific_stair( j + ((i==0) ? DNGN_STONE_STAIRS_DOWN_I
                                              : DNGN_STONE_STAIRS_UP_I) );
        }
    }
}                               // end bigger_room()

// various plan_xxx functions
static void plan_main( int level_number, char force_plan )
{
    // possible values for do_stairs:
    //  0 - stairs already done
    //  1 - stairs already done, do spotty
    //  2 - no stairs
    //  3 - no stairs, do spotty
    char do_stairs = 0;
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
        spotty_level( true, 0, coinflip() );

    if (do_stairs == 2 || do_stairs == 3)
    {
        const int pair_count = coinflip() ? 4 : 3;

        for (j = 0; j < pair_count; j++)
        {
            for (i = 0; i < 2; i++)
            {
                place_specific_stair( j + ((i==0) ? DNGN_STONE_STAIRS_DOWN_I
                                                  : DNGN_STONE_STAIRS_UP_I) );
            }
        }
    }

    if (one_chance_in(20))
        replace_area( 0, 0, GXM-1, GYM-1, DNGN_ROCK_WALL, special_grid );
}                               // end plan_main()

static char plan_1(void)
{
#if DEBUG_DIAGNOSTICS
    mpr( MSGCH_DIAGNOSTICS, "Build: plan_1" );
#endif

    int temp_rand = 0;          // probability determination {dlb}

    unsigned char width = 4 + random2(7);    // value range of [4,10] {dlb}

    // building a thick rectangle
    // upper edge
    replace_area( X_BOUND_1 + 1, Y_BOUND_1 + 1,
                  X_BOUND_2 - 1, (Y_BOUND_1 + width + 1),
                  DNGN_ROCK_WALL, DNGN_FLOOR );

    // lower edge
    replace_area( X_BOUND_1 + 1, (Y_BOUND_2 - width - 1),
                  X_BOUND_2 - 1, Y_BOUND_2 - 1,
                  DNGN_ROCK_WALL, DNGN_FLOOR );

    // left edge
    replace_area( X_BOUND_1 + 1, Y_BOUND_1 + 1,
                  (X_BOUND_1 + width + 1), Y_BOUND_2 - 1,
                  DNGN_ROCK_WALL, DNGN_FLOOR );

    // right edge
    replace_area( (X_BOUND_2 - width - 1), Y_BOUND_1 + 1,
                  X_BOUND_2 - 1, Y_BOUND_2 - 1,
                  DNGN_ROCK_WALL, DNGN_FLOOR );

    // possible early returns {dlb}:
    temp_rand = random2(4);

    if (temp_rand > 2)          // 25% chance {dlb}
        return 3;
    else if (temp_rand > 1)     // 25% chance {dlb}
        return 2;
    else                        // 50% chance {dlb}
    {
        // make a cross
        unsigned char width2 = (coinflip() ? (1 + random2(5)) : 5);

        // horizontal
        replace_area( X_BOUND_1 + 1, (GYM / 2 - width2),
                      X_BOUND_2 - 1, (GYM / 2 + width2),
                      DNGN_ROCK_WALL, DNGN_FLOOR );

        // vertical
        replace_area( (GXM / 2 - width2), Y_BOUND_1 + 1,
                      (GXM / 2 + width2), Y_BOUND_2 - 1,
                      DNGN_ROCK_WALL, DNGN_FLOOR );
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
            sr.x1 = X_BOUND_1 + 20;
            sr.y1 = Y_BOUND_1 + 20;
            sr.x2 = X_BOUND_2 - 20;
            sr.y2 = Y_BOUND_2 - 20;

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
#if DEBUG_DIAGNOSTICS
    mpr( MSGCH_DIAGNOSTICS, "Build: plan_2" );
#endif

    char width = 2 + random2(5);     // value range of [2,6] {dlb}

    // build a cross:
    // horizontal:
    replace_area( X_BOUND_1 + 1, (GYM / 2 - width),
                  X_BOUND_2 - 1, (GYM / 2 + width),
                  DNGN_ROCK_WALL, DNGN_FLOOR );

    // vertical:
    replace_area( (GXM / 2 - width), Y_BOUND_1 + 1,
                  (GXM / 2 + width), Y_BOUND_2 - 1,
                  DNGN_ROCK_WALL, DNGN_FLOOR );

    return (one_chance_in(4) ? 2 : 3);
}                               // end plan_2()

static char plan_3(void)
{
#if DEBUG_DIAGNOSTICS
    mpr( MSGCH_DIAGNOSTICS, "Build: plan_3" );
#endif

    /* Draws a room, then another and links them together, then another and etc
       Of course, this can easily end up looking just like a make_trail level.
     */
    int  i;
    int  cnx, cny;
    int  roomsss = 30 + random2(90);

    bool exclusive = (one_chance_in(10) ? false : true);
    bool exclusive2 = coinflip();

    int romx1[30], romy1[30], romx2[30], romy2[30];

    int which = 0;

    for (i = 0; i < roomsss; i++)
    {
        romx1[which] = 0;
        romy1[which] = 0;
        romx2[which] = 2 + random2(8);
        romy2[which] = 2 + random2(8);

        random_place_rectangle( romx1[which], romy1[which],
                                romx1[which], romy1[which] );

        if (exclusive)
        {
            for (cnx = romx1[which] - 1; cnx < romx2[which] + 1; cnx++)
            {
                for (cny = romy1[which] - 1; cny < romy2[which] + 1; cny++)
                {
                    if (grd[cnx][cny] != DNGN_ROCK_WALL)
                        goto continuing;
                }
            }
        }

        replace_area( romx1[which], romy1[which], romx2[which], romy2[which],
                      DNGN_ROCK_WALL, DNGN_FLOOR );

        if (which > 0 && !exclusive2)
        {
            const int rx1 = romx1[which];
            const int rx2 = romx2[which];
            const int prev_rx1 = romx1[which - 1];
            const int prev_rx2 = romx2[which - 1];

            const int ry1 = romy1[which];
            const int ry2 = romy2[which];
            const int prev_ry1 = romy1[which - 1];
            const int prev_ry2 = romy2[which - 1];

            join_the_dots( rx1 + random2( rx2 - rx1 ),
                           ry1 + random2( ry2 - ry1 ),
                           prev_rx1 + random2( prev_rx2 - prev_rx1 ),
                           prev_ry1 + random2( prev_ry2 - prev_ry1 ),
                           0, 0, 0, 0 );
        }

        which++;

        if (which >= 29)
            break;

      continuing:
        continue;
    }

    if (exclusive2)
    {
        for (i = 0; i < which; i++)
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

static char plan_4( char forbid_x1, char forbid_y1,
                    char forbid_x2, char forbid_y2, unsigned char force_wall )
{
#if DEBUG_DIAGNOSTICS
    mpr( MSGCH_DIAGNOSTICS, "Build: plan_4" );
#endif

    // a more chaotic version of city level
    int temp_rand;              // req'd for probability checking

    int number_boxes = 5000;
    unsigned char drawing = DNGN_ROCK_WALL;
    int b1x, b1y, b2x, b2y;
    int cnx, cny;
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

    replace_area( X_BOUND_1 + 1, Y_BOUND_1 + 1, X_BOUND_2 - 1, Y_BOUND_2 - 1,
                  DNGN_ROCK_WALL, DNGN_FLOOR );

    // replace_area can also be used to fill in:
    for (i = 0; i < number_boxes; i++)
    {
        b1x = 0;
        b1y = 0;
        b2x = 1 + roll_dice(2,7);
        b2y = 1 + roll_dice(2,7);

        random_place_rectangle( b1x, b1y, b2x, b2y );

        if ((forbid_x1 != 0 || forbid_x2 != 0)
            && rectangles_overlap( b1x, b1y, b2x, b2y, forbid_x1, forbid_y1,
                                    forbid_x2, forbid_y2 ))
        {
            goto continuing;
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
            replace_area( b1x, b1y, b2x, b2y, DNGN_FLOOR, drawing );
        else                    // odds:  72 in 210 {dlb}
            box_room( b1x, b2x, b1y, b2y, drawing );

      continuing:
        continue;
    }

    if (forbid_x1 == 0 && one_chance_in(4))     // a market square
    {
        spec_room sr = { false, false, 0, 0, 0, 0 };
        sr.x1 = X_BOUND_1 + 20;
        sr.y1 = Y_BOUND_1 + 20;
        sr.x2 = X_BOUND_2 - 20;
        sr.y2 = Y_BOUND_2 - 20;

        int oblique_max = 0;
        if (!one_chance_in(4))
            oblique_max = 5 + random2(20);      // used elsewhere {dlb}

        unsigned char feature = DNGN_FLOOR;
        if (one_chance_in(10))
            feature = (coinflip()? DNGN_DEEP_WATER : DNGN_LAVA);

        octa_room( sr, oblique_max, feature );
    }

    return 2;
}                               // end plan_4()

static char plan_5(void)
{
#if DEBUG_DIAGNOSTICS
    mpr( MSGCH_DIAGNOSTICS, "Build: plan_5" );
#endif

    int imax = 5 + random2(20);       // value range of [5,24] {dlb}

    for (int i = 0; i < imax; i++)
    {
        int x1, y1, x2, y2;

        random_in_bounds( x1, y1, DNGN_RANDOM, false, false );
        random_in_bounds( x2, y2, DNGN_RANDOM, false, false );

        join_the_dots( x1, y1, x2, y2, 0, 0, 0, 0 );
    }

    if (!one_chance_in(4))
        spotty_level( true, 100, coinflip() );

    return 2;
}                               // end plan_5()

// circle of standing stones (well, kind of)
static char plan_6(int level_number)
{
#if DEBUG_DIAGNOSTICS
    mpr( MSGCH_DIAGNOSTICS, "Build: plan_6" );
#endif

    spec_room sr = { false, false, 0,0,0,0 };

    // not using GXM, or X_BOUNDS here because everything is very specified:
    sr.x1 = 10;
    sr.x2 = 70;
    sr.y1 = 10;
    sr.y2 = 60;

    octa_room( sr, 14, DNGN_FLOOR );

    replace_area( 23, 23, 26, 26, DNGN_FLOOR, DNGN_STONE_WALL );
    replace_area( 23, 47, 26, 50, DNGN_FLOOR, DNGN_STONE_WALL );

    replace_area( 55, 23, 58, 26, DNGN_FLOOR, DNGN_STONE_WALL );
    replace_area( 55, 47, 58, 50, DNGN_FLOOR, DNGN_STONE_WALL );

    replace_area( 39, 20, 43, 23, DNGN_FLOOR, DNGN_STONE_WALL );
    replace_area( 39, 50, 43, 53, DNGN_FLOOR, DNGN_STONE_WALL );

    replace_area( 20, 30, 23, 33, DNGN_FLOOR, DNGN_STONE_WALL );
    replace_area( 20, 40, 23, 43, DNGN_FLOOR, DNGN_STONE_WALL );

    replace_area( 58, 30, 61, 33, DNGN_FLOOR, DNGN_STONE_WALL );
    replace_area( 58, 40, 61, 43, DNGN_FLOOR, DNGN_STONE_WALL );

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

    // This "back door" is often one of the easier ways to get out of
    // pandemonium... the easiest is to use the banish spell.
    //
    // Note, that although "level_number > 20" will work for most
    // trips to pandemonium (through regular portals), it might not
    // work for demonspawn who gate themselves there. -- bwr
    if (((player_in_branch( BRANCH_MAIN_DUNGEON ) && level_number > 20)
            || you.level_type == LEVEL_PANDEMONIUM)
        && (coinflip() || you.mutation[ MUT_PANDEMONIUM ] > 1))
    {
        grd[40][36] = DNGN_ENTER_ABYSS;
        grd[41][36] = DNGN_ENTER_ABYSS;
    }

    return 0;
}                               // end plan_6()

static bool octa_room(spec_room &sr, int oblique_max, unsigned char type_floor)
{
    int x,y;

    // hack - avoid lava in the crypt {gdl}
    if ((player_in_branch( BRANCH_CRYPT ) || player_in_branch( BRANCH_TOMB ))
         && type_floor == DNGN_LAVA)
    {
        type_floor = DNGN_SHALLOW_WATER;
    }

    int oblique = oblique_max;

    // check octagonal room for special; avoid if exists
    for (x = sr.x1; x < sr.x2; x++)
    {
        for (y = sr.y1 + oblique; y < sr.y2 - oblique; y++)
        {
            if (grd[x][y] == DNGN_BUILDER_SPECIAL_WALL)
                return (false);
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

            if (grd[x][y] == DNGN_CLOSED_DOOR && type_floor >= MINMOVE)
                grd[x][y] = DNGN_FLOOR;       // ick
        }

        if (oblique > 0)
            oblique--;

        if (x > sr.x2 - oblique_max)
            oblique += 2;
    }

    return (true);
}                               // end octa_room()

#if USE_NEW_LABYRINTH

static void labyrinth_level( int level_number )
{
    // Going right up to the border like this makes the outer wall
    // guaranteed to be permanent.
    spec_room sr = { false, false, X_BOUND_1, Y_BOUND_1, X_BOUND_2, Y_BOUND_2 };

    int temp_rand = random2(50);

    int walls = ((temp_rand < 40) ? DNGN_STONE_WALL   // 80.0%
                                  : DNGN_METAL_WALL); // 20.0%

    // No secret doors and tending towards straightness in order to be nice.
    build_maze( sr, walls, 300 - random2( 4 * level_number ), false );

    // find start and end points:
    const int dx = (sr.x2 - sr.x1) / 2;
    const int dy = (sr.y2 - sr.y1) / 2;

    int sx, sy, lx, ly;

    for (;;)
    {
        // Doing it this way guarantees us that these spots are empty,
        // since we're only considering the "rooms" we randomly connected.
        sx = sr.x1 + 1 + 2 * random2( dx );
        sy = sr.y1 + 1 + 2 * random2( dy );

        lx = sr.x1 + 1 + 2 * random2( dx );
        ly = sr.y1 + 1 + 2 * random2( dy );

        if (grid_distance( sx, sy, lx, ly ) > 20)
            break;
    }

    grd[sx][sy] = DNGN_STONE_STAIRS_UP_I;  // marks the starting stairs
    grd[lx][ly] = DNGN_ROCK_STAIRS_UP;     // marks the ending stairs

    // place goods and minotaur
    int base_type = OBJ_RANDOM;
    int num_items = 8 + roll_zdice(2,5);

    for (int i = 0; i < num_items; i++)
    {
        temp_rand = random2(11);

        base_type = ((temp_rand == 0 || temp_rand == 9)  ? OBJ_WEAPONS :
                     (temp_rand == 1 || temp_rand == 10) ? OBJ_ARMOUR :
                     (temp_rand == 2)                    ? OBJ_MISSILES :
                     (temp_rand == 3)                    ? OBJ_WANDS :
                     (temp_rand == 4)                    ? OBJ_MISCELLANY :
                     (temp_rand == 5)                    ? OBJ_SCROLLS :
                     (temp_rand == 6)                    ? OBJ_JEWELLERY :
                     (temp_rand == 7)                    ? OBJ_BOOKS
                     /* (temp_rand == 8) */              : OBJ_STAVES);

        int item = make_item( 1, base_type, OBJ_RANDOM, true,
                              (!one_chance_in(4) ? MAKE_GOOD_ITEM
                                                 : level_number + 10), 250 );

        if (item != NON_ITEM)
            move_item_to_grid( &item, lx, ly );
    }

    mons_place( MONS_MINOTAUR, BEH_SLEEP, MHITNOT, true, lx, ly );
}

#else

// old laybrinth code
static void labyrinth_level( int level_number )
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
    start_point_x = 10 + random2( GXM - 20 );

    int treasure_item = 0;

    unsigned char glopop = OBJ_RANDOM;  // used in calling make_item() {dlb}

    int num_items = 8 + roll_zdice(2,5);
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

        treasure_item = make_item( 1, glopop, OBJ_RANDOM, true,
                                   level_number + 10, 250 );

        if (treasure_item != NON_ITEM)
            move_item_to_grid( &treasure_item, lx, ly );
    }

    mons_place( MONS_MINOTAUR, BEH_SLEEP, MHITNOT, true, lx, ly );

    grd[lx][ly] = DNGN_ROCK_STAIRS_UP;

    link_items();

    // turn rock walls into undiggable stone or metal:
    temp_rand = random2(50);

    unsigned char wall_xform = ((temp_rand < 40) ? DNGN_STONE_WALL   // 80.0%
                                                 : DNGN_METAL_WALL); // 20.0%

    replace_area( 0, 0, GXM - 1, GYM - 1, DNGN_ROCK_WALL, wall_xform );
}                               // end labyrinth_level()
#endif

static int box_room_door_spot(int x, int y)
{
    // if there is a door near us embedded in rock, we have to be a door too.
    if ((grd[x-1][y] == DNGN_CLOSED_DOOR
            && grid_is_wall( grd[x-1][y-1] ) && grid_is_wall( grd[x-1][y+1] ))
        || (grd[x+1][y] == DNGN_CLOSED_DOOR
            && grid_is_wall( grd[x+1][y-1] ) && grid_is_wall( grd[x+1][y+1] ))
        || (grd[x][y-1] == DNGN_CLOSED_DOOR
            && grid_is_wall( grd[x-1][y-1] ) && grid_is_wall( grd[x+1][y-1] ))
        || (grd[x][y+1] == DNGN_CLOSED_DOOR
            && grid_is_wall( grd[x-1][y+1] ) && grid_is_wall( grd[x+1][y+1] )))
    {
        grd[x][y] = DNGN_CLOSED_DOOR;
        return 2;
    }

    // to be a good spot for a door, we need non-wall on two sides and
    // wall on two sides.
    bool nor = grid_is_wall( grd[x][y-1] );
    bool sou = grid_is_wall( grd[x][y+1] );
    bool eas = grid_is_wall( grd[x-1][y] );
    bool wes = grid_is_wall( grd[x+1][y] );

    if (nor == sou && eas == wes && nor != eas)
        return 1;

    return 0;
}

static int box_room_doors( int bx1, int bx2, int by1, int by2, int new_doors)
{
    int good_doors[200];        // 1 == good spot,  2 == door placed!
    int spot;
    int i;
    int doors_placed = new_doors;

    // sanity
    if (2 * ((bx2 - bx1) + (by2 - by1)) > 200)
        return 0;

    // go through, building list of good door spots,  and replacing wall
    // with door if we're about to block off another door.
    int spot_count = 0;

    // top & bottom
    for (i = bx1 + 1; i < bx2; i++)
    {
        good_doors[spot_count++] = box_room_door_spot( i, by1 );
        good_doors[spot_count++] = box_room_door_spot( i, by2 );
    }
    // left & right
    for (i = by1 + 1; i < by2; i++)
    {
        good_doors[spot_count++] = box_room_door_spot( bx1, i );
        good_doors[spot_count++] = box_room_door_spot( bx2, i );
    }

    if (new_doors == 0)
    {
        // count # of doors we HAD to place
        for (i = 0; i < spot_count; i++)
        {
            if (good_doors[i] == 2)
                doors_placed++;
        }

        return (doors_placed);
    }

    for ( ; new_doors > 0; new_doors--)
    {
        int found = 0;
        spot = 0;

        // randomly pick a valid slot
        for (i = 0; i < spot_count; i++)
        {
            if (good_doors[i] == 1)
            {
                found++;
                if (one_chance_in( found ))
                    spot = i;
            }
        }

        // no slot found
        if (!found)
            break;

        // make a door
        if (spot < 2 * (bx2 - bx1 - 1))
        {
            if ((spot % 2) == 0)
                grd[bx1 + 1 + spot / 2][by1] = DNGN_CLOSED_DOOR;
            else
                grd[bx1 + 1 + spot / 2][by2] = DNGN_CLOSED_DOOR;
        }
        else
        {
            spot -= 2 * (bx2 - bx1 - 1);

            if ((spot % 2) == 0)
                grd[bx1][by1 + 1 + spot / 2] = DNGN_CLOSED_DOOR;
            else
                grd[bx2][by1 + 1 + spot / 2] = DNGN_CLOSED_DOOR;
        }
    }

    return (doors_placed);
}


static void box_room(int bx1, int bx2, int by1, int by2, int wall_type)
{
    // hack -- avoid lava in the crypt. {gdl}
    if ((player_in_branch( BRANCH_CRYPT ) || player_in_branch( BRANCH_TOMB ))
         && wall_type == DNGN_LAVA)
    {
        wall_type = DNGN_SHALLOW_WATER;
    }

    int temp_rand, new_doors, doors_placed;

    // do top & bottom walls
    replace_area( bx1, by1, bx2, by1, DNGN_FLOOR, wall_type );
    replace_area( bx1, by2, bx2, by2, DNGN_FLOOR, wall_type );

    // do left & right walls
    replace_area( bx1, by1 + 1, bx1, by2 - 1, DNGN_FLOOR, wall_type );
    replace_area( bx2, by1 + 1, bx2, by2 - 1, DNGN_FLOOR, wall_type );

    // sometimes we have to place doors, or else we shut in other buildings' doors
    doors_placed = box_room_doors( bx1, bx2, by1, by2, 0 );

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

static void city_level( int level_number )
{
    int temp_rand;          // probability determination {dlb}
    int wall_type;          // remember, can have many wall types in one level
    int wall_type_room;     // simplifies logic of innermost loop {dlb}

    int xs = 0, ys = 0;
    int x1 = 0, x2 = 0;
    int y1 = 0, y2 = 0;
    int i,j;

#if DEBUG_DIAGNOSTICS
    mpr( MSGCH_DIAGNOSTICS, "Build: city_level" );
#endif

    temp_rand = random2(8);

    wall_type = ((temp_rand > 4) ? DNGN_ROCK_WALL :     // 37.5% {dlb}
                 (temp_rand > 1) ? DNGN_STONE_WALL      // 37.5% {dlb}
                                 : DNGN_METAL_WALL);    // 25.0% {dlb}

    if (one_chance_in(100))
        wall_type = DNGN_GREEN_CRYSTAL_WALL;

    make_box( X_BOUND_1 + 1, Y_BOUND_1 + 1, X_BOUND_2 - 1, Y_BOUND_2 - 1,
              DNGN_FLOOR );

    for (i = 0; i < 5; i++)
    {
        for (j = 0; j < 4; j++)
        {
            xs = 8 + (i * 13);
            ys = 8 + (j * 14);
            x1 = xs + roll_zdice(2,3);
            y1 = ys + roll_zdice(2,3);
            x2 = xs + 11 - roll_zdice(2,3);
            y2 = ys + 11 - roll_zdice(2,3);

            temp_rand = random2(280);

            if (temp_rand > 39) // 85.7% draw room(s) {dlb}
            {
                wall_type_room = ((temp_rand > 63) ? wall_type :       // 77.1%
                                  (temp_rand > 54) ? DNGN_STONE_WALL : //  3.2%
                                  (temp_rand > 45) ? DNGN_ROCK_WALL    //  3.2%
                                                   : DNGN_METAL_WALL); //  2.1%

                if (one_chance_in(250))
                    wall_type_room = DNGN_GREEN_CRYSTAL_WALL;

                box_room( x1, x2, y1, y2, wall_type_room );

                // inner room - neat.
                if (x2 - x1 > 5 && y2 - y1 > 5 && one_chance_in(8))
                {
                    box_room( x1 + 2, x2 - 2, y1 + 2, y2 - 2, wall_type );

                    // treasure area.. neat.
                    if (one_chance_in(3))
                    {
                        treasure_area( level_number, x1 + 3, x2 - 3, y1 + 3, y2 - 3 );
                    }
                }
            }
        }
    }

    const int stair_count = coinflip() ? 2 : 1;

    for (j = 0; j < stair_count; j++)
    {
        for (i = 0; i < 2; i++)
        {
            place_specific_stair( j + ((i==0) ? DNGN_STONE_STAIRS_DOWN_I
                                              : DNGN_STONE_STAIRS_UP_I) );
        }
    }
}                               // end city_level()

static bool treasure_area( int level_number, unsigned char ta1_x,
                           unsigned char ta2_x, unsigned char ta1_y,
                           unsigned char ta2_y )
{
    int x_count = 0;
    int y_count = 0;
    int item_made = 0;

    ta2_x++;
    ta2_y++;

    if (ta2_x <= ta1_x || ta2_y <= ta1_y)
        return (false);

    if ((ta2_x - ta1_x) * (ta2_y - ta1_y) >= 40)
        return (false);

    for (x_count = ta1_x; x_count < ta2_x; x_count++)
    {
        for (y_count = ta1_y; y_count < ta2_y; y_count++)
        {
            if (grd[x_count][y_count] != DNGN_FLOOR || coinflip())
                continue;

            item_made = make_item( 1, OBJ_RANDOM, OBJ_RANDOM, true,
                                   level_number + 5, 250 );

            if (item_made != NON_ITEM)
                move_item_to_grid( &item_made, x_count, y_count );
        }
    }

    return (true);
}                               // end treasure_area()

static void diamond_rooms( int level_number )
{
    char numb_diam = 1 + random2(10);
    char type_floor = DNGN_DEEP_WATER;
    int runthru = 0;
    int i, oblique_max;

#if DEBUG_DIAGNOSTICS
    mpr( MSGCH_DIAGNOSTICS, "Build: diamond_rooms" );
#endif

    // I guess no diamond rooms in either of these places {dlb}:
    if (player_in_branch( BRANCH_DIS ) || player_in_branch( BRANCH_TARTARUS ))
        return;

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

    if (player_in_branch( BRANCH_GEHENNA ))
        type_floor = DNGN_LAVA;
    else if (player_in_branch( BRANCH_COCYTUS ))
        type_floor = DNGN_DEEP_WATER;

    for (i = 0; i < numb_diam; i++)
    {
        spec_room sr = { false, false, 0, 0, 0, 0 };

        sr.x1 = 8 + random2(43);
        sr.y1 = 8 + random2(35);
        sr.x2 = sr.x1 + 5 + random2(15);
        sr.y2 = sr.y1 + 5 + random2(10);

        oblique_max = (sr.x2 - sr.x1) / 2;      //random2(20) + 5;

        if (!octa_room( sr, oblique_max, type_floor ))
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

static void big_room( int level_number )
{
    int i, j, k, l;

    spec_room sr = { false, false, 0, 0, 0, 0 };

    sr.x1 = 8 + random2(30);
    sr.y1 = 8 + random2(22);
    sr.x2 = sr.x1 + 20 + random2(10);
    sr.y2 = sr.y1 + 20 + random2(8);

    // choose a floor type:
    int type_floor = DNGN_SHALLOW_WATER;

    // usually floor, except at higher levels
    if (!one_chance_in(5) || level_number < 8 + random2(8))
        type_floor = DNGN_FLOOR;
    else if (level_number > 7)
    {
        type_floor = ((random2(level_number) < 14) ? DNGN_DEEP_WATER
                                                   : DNGN_LAVA);
    }

    // no lava in the Crypt or Tomb, thanks!
    if (player_in_branch( BRANCH_CRYPT ) || player_in_branch( BRANCH_TOMB ))
    {
        if (type_floor == DNGN_LAVA)
            type_floor = DNGN_SHALLOW_WATER;
    }

    // octa_rooms have their own special handling, so we do them and exit here
    if (one_chance_in(4))
    {
        octa_room( sr, 5 + random2(20), type_floor );
        return;
    }

    // check for previous special in the rectangular big_room area
    if (find_in_area( sr.x1, sr.y1, sr.x2, sr.y2, DNGN_BUILDER_SPECIAL_WALL ))
        return;

    // choose a wall types:
    int tmp = random2(1000);
    int type_wall = ((tmp <  25) ? DNGN_GREEN_CRYSTAL_WALL :
                     (tmp < 100) ? DNGN_METAL_WALL
                                 : DNGN_STONE_WALL);

    if (level_number < 12 && coinflip())
        type_wall = DNGN_ROCK_WALL;

    // make the big room.
    replace_area( sr.x1, sr.y1, sr.x2, sr.y2, DNGN_ROCK_WALL, type_floor );
    replace_area( sr.x1, sr.y1, sr.x2, sr.y2, DNGN_CLOSED_DOOR, type_floor );

    // make a special rectangular big_room

    switch ((level_number == PANDEMONIUM_DEPTH) ? random2(5) : random2(3))
    {
    case 0:  // Mazes
    default:    // this results in mazes being more common in pandemonium
        {
            // shinking the vault to leave an easy through route around the maze
            sr.x1++;
            sr.x2--;
            sr.y1++;
            sr.y2--;

            int straight = 100;

            if (one_chance_in(4))
                straight += (coinflip() ? 1 : -1) * random2(2 * level_number);

            if (build_maze( sr, type_wall, straight, true ))
            {
#if DEBUG_DIAGNOSTICS
                mpr( MSGCH_DIAGNOSTICS, "Build: maze (%d,%d)-(%d,%d)",
                          sr.x1, sr.y1, sr.x2, sr.y2 );
#endif

                // first put down any doors we have to:
                box_room_doors( sr.x1, sr.x2, sr.y1, sr.y2, 0 );
                // now put down a few extra to help guarantee access:
                box_room_doors( sr.x1, sr.x2, sr.y1, sr.y2, roll_dice(2,2) );
            }
        }
        break;

    case 1:
#if DEBUG_DIAGNOSTICS
        mpr( MSGCH_DIAGNOSTICS, "Build: chequerboard (%d,%d)-(%d,%d)",
                  sr.x1, sr.y1, sr.x2, sr.y2 );
#endif
        chequerboard( sr, type_floor, type_wall );
        break;

    case 2:
        // sometimes make an inside room w/ stone wall.
#if DEBUG_DIAGNOSTICS
        mpr( MSGCH_DIAGNOSTICS, "Build: nested rooms (%d,%d)-(%d,%d)",
                  sr.x1, sr.y1, sr.x2, sr.y2 );
#endif

        i = sr.x1;
        j = sr.y1;
        k = sr.x2;
        l = sr.y2;

        if (type_wall == DNGN_ROCK_WALL)
            type_wall = DNGN_STONE_WALL;

        for (;;)
        {
            i += 2 + random2(3);
            j += 2 + random2(3);
            k -= 2 + random2(3);
            l -= 2 + random2(3);

            // check for too small
            if (k - i < 3 || l - j < 3)
                break;

            box_room( i, k, j, l, type_wall );

            // rotate wall types (used to be just stone
            if (coinflip() || type_wall == DNGN_GREEN_CRYSTAL_WALL)
            {
                tmp = random2(1000);
                type_wall = ((tmp <  25) ? DNGN_GREEN_CRYSTAL_WALL :
                             (tmp < 100) ? DNGN_METAL_WALL
                                         : DNGN_STONE_WALL);
            }
            else if (coinflip())
            {
                if (type_wall == DNGN_STONE_WALL)
                    type_wall = DNGN_METAL_WALL;
                else if (type_wall == DNGN_METAL_WALL)
                    type_wall = DNGN_GREEN_CRYSTAL_WALL;
            }
        }
        break;
    }

    // put a few monsters and items near the middle
    j = roll_dice( 1, 4 );
    for (i = 0; i < j; i++)
    {
        int mon = create_monster( RANDOM_MONSTER, BEH_SLEEP, 0,
                                  (sr.x1 + sr.x2) / 2, (sr.y1 + sr.y2) / 2,
                                  MHITYOU, 250, false, level_number + 5 );

        if (mon == -1)
            break;

        int it = make_item( 1, OBJ_RANDOM, OBJ_RANDOM, true,
                            level_number + 5, 250 );

        if (it != NON_ITEM)
            move_item_to_grid( &it, menv[mon].x, menv[mon].y );
    }
}                               // end big_room()

// helper function for chequerboard rooms (now generalized 4x4 repeat patterns)
// note that box boundaries are INclusive
static void chequerboard( spec_room &sr, unsigned char floor, unsigned char wall )
{
    int i, j;

    if (sr.x2 < sr.x1 || sr.y2 < sr.y1)
        return;

    // each nibble is a row of 4 columns
    const int cols = 4;
    const int rows = 4;

    const unsigned long patterns[] = { 0xA5A5,      // standard
                                       0x4141,      // loose chequer
                                       0x0505,      // loose pillar grid
                                       0x6969,      // two wide pillar chequer
                                       0xC030,      // loose two wide chequer
                                       0x6996,      // big pillar chequer
                                       0x0660,      // loose big pillar grid
                                       0x1428,      // disjoint loose chequer
                                       0x4E40,      // big pluses
                                       0x4A40,      // empty pluses
                                       0x1248,      // diagonal
                                     };

    const int num_patterns = sizeof( patterns ) / sizeof( unsigned long );

    const unsigned long mask = patterns[ random2(num_patterns) ];

    // flips and shifts to increase the possibilities (but some won't care)
    const bool flip_x = coinflip();
    const bool flip_y = coinflip();
    const int shift_x = random2( rows );
    const int shift_y = random2( cols );

    for (i = sr.x1; i <= sr.x2; i++)
    {
        for (j = sr.y1; j <= sr.y2; j++)
        {
            const int x = ((flip_x) ? 3 - ((i + shift_x) % rows)
                                    : ((i + shift_x) % rows));

            const int y = ((flip_y) ? 3 - ((j + shift_y) % cols)
                                    : ((j + shift_y) % cols));

            if (mask & (1 << (cols * x + y)))
                grd[i][j] = wall;
            else
                grd[i][j] = floor;
        }
    }
}                               // end chequerboard()

// walks in each random direction that can visit an unvisited (special) grid.
static void build_maze_visit( int x, int y, int facing,
                              int straightness, bool secret_doors )
{
    const int wstraight = straightness;
    const int wturn = (300 - straightness) / 2;

    grd[x][y] = DNGN_FLOOR;

    for (;;)
    {
        int nx = 0;
        int ny = 0;
        int nf = RDIR_REST;
        int count = 0;

        if (in_bounds(x - 2, y) && grd[x - 2][y] == DNGN_BUILDER_SPECIAL_FLOOR)
        {
            count += (facing == RDIR_LEFT) ? wstraight : wturn;

            nf = RDIR_LEFT;
            nx = x - 2;
            ny = y;
        }

        if (in_bounds(x + 2, y) && grd[x + 2][y] == DNGN_BUILDER_SPECIAL_FLOOR)
        {
            const int w = (facing == RDIR_RIGHT) ? wstraight : wturn;

            count += w;
            if (random2( count ) < w)
            {
                nf = RDIR_RIGHT;
                nx = x + 2;
                ny = y;
            }
        }

        if (in_bounds(x, y - 2) && grd[x][y - 2] == DNGN_BUILDER_SPECIAL_FLOOR)
        {
            const int w = (facing == RDIR_UP) ? wstraight : wturn;

            count += w;
            if (random2( count ) < w)
            {
                nf = RDIR_UP;
                nx = x;
                ny = y - 2;
            }
        }

        if (in_bounds(x, y + 2) && grd[x][y + 2] == DNGN_BUILDER_SPECIAL_FLOOR)
        {
            const int w = (facing == RDIR_DOWN) ? wstraight : wturn;

            count += w;
            if (random2( count ) < w)
            {
                nf = RDIR_DOWN;
                nx = x;
                ny = y + 2;
            }
        }

        if (!count)
            break;

        // calculate the wall spot between the current and new pos
        const int mx = (x + nx) / 2;
        const int my = (y + ny) / 2;

        if (secret_doors && one_chance_in(100))
            grd[mx][my] = DNGN_SECRET_DOOR;
        else
            grd[mx][my] = DNGN_FLOOR;

        build_maze_visit( nx, ny, nf, straightness, secret_doors );
    }
}

// Straightness weights the amount of twistiness in the maze.  At a
// weight of 100, straight ahead is equal to turning left or right.
// Higher (> 100) will get mazes with straighter and longer corridors,
// and lower (< 100) will get mazes that are even more twisty!
static bool build_maze( spec_room &sr, int wall,
                        int straightness, bool secret_doors )
{
    int x, y;

    if (straightness > 290)
        straightness = 290;
    else if (straightness < 10)
        straightness = 10;

    // need to have even parity, reduce an x side if needed
    if ((sr.x2 - sr.x1) % 2 == 1)
    {
        if (coinflip())
            sr.x1++;
        else
            sr.x2--;
    }

    // need to have even parity, reduce an x side if needed
    if ((sr.y2 - sr.y1) % 2 == 1)
    {
        if (coinflip())
            sr.y1++;
        else
            sr.y2--;
    }

    // safty check for size:
    if (sr.x2 - sr.x1 < 6 || sr.y2 - sr.y1 < 6)
        return (false);

    // init area
    for (x = sr.x1; x <= sr.x2; x++)
    {
        for (y = sr.y1; y <= sr.y2; y++)
        {
            grd[x][y] = DNGN_BUILDER_SPECIAL_FLOOR;
        }
    }

    // build the grid
    for (x = sr.x1; x <= sr.x2; x += 2)
        replace_area( x, sr.y1, x, sr.y2, DNGN_BUILDER_SPECIAL_FLOOR, wall );

    for (y = sr.y1; y <= sr.y2; y += 2)
        replace_area( sr.x1, y, sr.x2, y, DNGN_BUILDER_SPECIAL_FLOOR, wall );

    // recursively build a maze
    build_maze_visit( sr.x1 + 1, sr.y1 + 1, RDIR_DOWN, straightness, secret_doors );

    return (true);
}

static void roguey_level( int level_number, spec_room &sr )
{
    int bcount_x, bcount_y;
    int cn = 0;
    int i;

    FixedVector < unsigned char, 30 > rox1;
    FixedVector < unsigned char, 30 > rox2;
    FixedVector < unsigned char, 30 > roy1;
    FixedVector < unsigned char, 30 > roy2;

#if DEBUG_DIAGNOSTICS
    mpr( MSGCH_DIAGNOSTICS, "Build: roguey_level" );
#endif

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
            if (rox2[cn] > X_BOUND_2 - 1)
                rox2[cn] = X_BOUND_2 - 1;

            cn++;
        }
    }

    cn = 0;

    for (i = 0; i < 25; i++)
    {
        replace_area( rox1[i], roy1[i], rox2[i], roy2[i],
                      DNGN_ROCK_WALL, DNGN_FLOOR );

        // inner room?
        if (rox2[i] - rox1[i] > 5 && roy2[i] - roy1[i] > 5)
        {
            if (random2(100 - level_number) < 3)
            {
                if (!one_chance_in(4))
                {
                    box_room( rox1[i] + 2, rox2[i] - 2,
                              roy1[i] + 2, roy2[i] - 2,
                              (coinflip() ? DNGN_STONE_WALL : DNGN_ROCK_WALL) );
                }
                else
                {
                    box_room( rox1[i] + 2, rox2[i] - 2,
                              roy1[i] + 2, roy2[i] - 2,
                              DNGN_METAL_WALL );
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
                jpos[1] = roy1[last_room] + random2(roy2[last_room] - roy1[last_room]);
                break;

            case 1:
                last_room = i - 5;
                pos[1] = roy1[i];      // - 1;
                pos[0] = rox1[i] + random2(rox2[i] - rox1[i]);
                jpos[1] = roy2[last_room];      // + 1;
                jpos[0] = rox1[last_room] + random2(rox2[last_room] - rox1[last_room]);
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
        special_room( level_number, sr );

        // make the room 'special' so it doesn't get overwritten
        // by something else (or put monsters in walls, etc..).

        // top
        replace_area( sr.x1-1, sr.y1-1, sr.x2+1, sr.y1-1,
                      DNGN_ROCK_WALL, DNGN_BUILDER_SPECIAL_WALL );
        replace_area( sr.x1-1, sr.y1-1, sr.x2+1, sr.y1-1,
                      DNGN_FLOOR, DNGN_BUILDER_SPECIAL_FLOOR );
        replace_area( sr.x1-1, sr.y1-1, sr.x2+1, sr.y1-1,
                      DNGN_CLOSED_DOOR, DNGN_BUILDER_SPECIAL_FLOOR );

        // bottom
        replace_area( sr.x1-1, sr.y2+1, sr.x2+1, sr.y2+1,
                      DNGN_ROCK_WALL, DNGN_BUILDER_SPECIAL_WALL );
        replace_area( sr.x1-1, sr.y2+1, sr.x2+1, sr.y2+1,
                      DNGN_FLOOR, DNGN_BUILDER_SPECIAL_FLOOR );
        replace_area( sr.x1-1, sr.y2+1, sr.x2+1, sr.y2+1,
                      DNGN_CLOSED_DOOR, DNGN_BUILDER_SPECIAL_FLOOR );

        // left
        replace_area( sr.x1-1, sr.y1-1, sr.x1-1, sr.y2+1,
                      DNGN_ROCK_WALL, DNGN_BUILDER_SPECIAL_WALL );
        replace_area( sr.x1-1, sr.y1-1, sr.x1-1, sr.y2+1,
                      DNGN_FLOOR, DNGN_BUILDER_SPECIAL_FLOOR );
        replace_area( sr.x1-1, sr.y1-1, sr.x1-1, sr.y2+1,
                      DNGN_CLOSED_DOOR, DNGN_BUILDER_SPECIAL_FLOOR );

        // right
        replace_area( sr.x2+1, sr.y1-1, sr.x2+1, sr.y2+1,
                      DNGN_ROCK_WALL, DNGN_BUILDER_SPECIAL_WALL );
        replace_area( sr.x2+1, sr.y1-1, sr.x2+1, sr.y2+1,
                      DNGN_FLOOR, DNGN_BUILDER_SPECIAL_FLOOR );
        replace_area( sr.x2+1, sr.y1-1, sr.x2+1, sr.y2+1,
                      DNGN_CLOSED_DOOR, DNGN_BUILDER_SPECIAL_FLOOR );
    }

    const int stair_count = coinflip() ? 2 : 1;

    for (int j = 0; j < stair_count; j++)
    {
        for (i = 0; i < 2; i++)
        {
            place_specific_stair(j + ((i==0) ? DNGN_STONE_STAIRS_DOWN_I
                                             : DNGN_STONE_STAIRS_UP_I));
        }
    }
}                               // end roguey_level()

static bool place_specific_trap( int spec_x, int spec_y, int spec_type, bool force )
{
    if (spec_type == TRAP_RANDOM || spec_type == TRAP_NONTELEPORT)
    {
        // Now allowing items to be created on teleport traps because:
        // (a) you can throw an item on top of one already
        // (b) apportation can be used to pull it off
        do
        {
            spec_type = random2( NUM_TRAPS );
        }
        while (spec_type == TRAP_TELEPORT
                && (spec_type == TRAP_NONTELEPORT
                    || mgrd[spec_x][spec_y] != NON_MONSTER)
                && ((spec_type == TRAP_SPEAR
                        || spec_type == TRAP_AXE
                        || spec_type == TRAP_BOLT)
                    && you.depth < 5)
                && ((spec_type == TRAP_BLADE
                        || spec_type == TRAP_ZOT)
                    && you.depth < 12));
    }

    int tcount;
    for (tcount = 0; tcount < MAX_TRAPS; tcount++)
    {
        if (env.trap[tcount].type == TRAP_UNASSIGNED)
            break;
    }

    if (force && tcount == MAX_TRAPS)
    {
        tcount = random2( MAX_TRAPS );
        remove_trap( tcount );
    }

    if (tcount < MAX_TRAPS)
    {
        env.trap[tcount].type = spec_type;
        env.trap[tcount].x = spec_x;
        env.trap[tcount].y = spec_y;
        grd[spec_x][spec_y] = DNGN_UNDISCOVERED_TRAP;

        arm_trap(tcount);
        return (true);
    }

    return (false);
}                               // end place_specific_trap()

void define_zombie( int mid, int ztype, int cs, int power )
{
    int mons_sec2 = 0;
    int zombie_size = 0;
    bool ignore_rarity = false;
    int test, cls;

    if (power > 27)
        power = 27;

    // set size based on zombie class (cs)
    switch (cs)
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

    // that is, random creature from which to fashion undead
    if (ztype == 250)
    {
        // Okay, we're going dirt simple here because this function is
        // notorious for hanging games.  Basically, some areas (abyss)
        // and dungeons (temple, slime pits) don't have any species to
        // make zombies from... others (elven halls) don't have a lot
        // of variety either.  So basically, everything that's not the
        // main dungeon is probably prone to problems and we'll just
        // ignore rarity on all of them rather than try to make a list.
        if (you.level_type != LEVEL_DUNGEON
            || player_in_hell()
            || !player_in_branch( BRANCH_MAIN_DUNGEON ))
        {
            ignore_rarity = true;
        }

        // how OOD this zombie can be.
        int relax = 5;

        // pick an appropriate creature to make a zombie out of,
        // levelwise.  The old code was generating absolutely
        // incredible OOD zombies.
        while (true)
        {
            // Just a little more protection against a case where things
            // keep aborting before things get relaxed.
            if (one_chance_in(1000))
                ignore_rarity = true;

            test = random2(NUM_MONSTERS);
            cls = mons_species(test);
            if (cls == MONS_PROGRAM_BUG)
                continue;

            // don't make out-of-rarity zombies when we don't have to
            if (!ignore_rarity && mons_rarity(cls) == 0)
                continue;

            // monster class must be zombifiable
            if (!mons_zombie_size(cls))
                continue;

            // if skeleton, monster must have a skeleton
            if ((cs == MONS_SKELETON_SMALL || cs == MONS_SKELETON_LARGE)
                && !mons_skeleton(cls))
            {
                continue;
            }

            // size must match, but you can make a spectral thing out of anything.
            if (mons_zombie_size(cls) != zombie_size && zombie_size >= 0)
                continue;

            // hack -- non-dungeon zombies are always made out of nastier
            // monsters
            if (you.level_type != LEVEL_DUNGEON || player_in_hell())
            {
                if (mons_power(cls) > 8)  // currently num HD
                    continue;

                if (mons_class_speed(cls) < 7)
                    continue;
            }

            // check for rarity.. and OOD - identical to mons_place()
            int level, diff, chance;

            level  = (ignore_rarity) ? mons_class_hit_dice(cls) - 4
                                     : mons_level(cls) - 4;

            diff   = level - power;

            chance = (ignore_rarity) ? 100
                                     : mons_rarity(cls) - (diff * diff) / 2;

            if (power > level - relax
                && power < level + relax
                && roll_curved_percentile() < chance)
            {
                break;
            }

            // every so often,  we'll relax the OOD restrictions.  Avoids
            // infinite loops (if we don't do this,  things like creating
            // a large skeleton on level 1 may hang the game!
            if (one_chance_in(5))
            {
                relax++;

                // if we're already looking at this size, ignore rarity too
                if (relax + power > 25)
                    ignore_rarity = true;
            }
        }

        // set type and secondary appropriately
        menv[mid].number = cls;
        mons_sec2 = cls;
    }
    else
    {
        menv[mid].number = mons_species(ztype);
        mons_sec2 = menv[mid].number;
    }

    menv[mid].type = menv[mid].number;

    define_monster( mid );

    switch (cs)
    {
    case MONS_ZOMBIE_SMALL:
    case MONS_ZOMBIE_LARGE:
    default:
        menv[mid].type = ((mons_zombie_size(menv[mid].number) == 2)
                                ? MONS_ZOMBIE_LARGE : MONS_ZOMBIE_SMALL);

        menv[mid].hit_points = hit_points( menv[mid].hit_dice, 6, 4 );
        menv[mid].armour_class -= 2;
        menv[mid].evasion -= 4;
        menv[mid].speed = (menv[mid].speed * 8) / 10;
        break;

    case MONS_SKELETON_SMALL:
    case MONS_SKELETON_LARGE:
        menv[mid].type = ((mons_zombie_size( menv[mid].number ) == 2)
                            ? MONS_SKELETON_LARGE : MONS_SKELETON_SMALL);

        menv[mid].hit_points = hit_points( menv[mid].hit_dice, 4, 4 );
        menv[mid].armour_class -= 6;
        menv[mid].evasion -= 4;
        menv[mid].speed = (menv[mid].speed * 8) / 10;
        break;

    case MONS_SIMULACRUM_SMALL:
    case MONS_SIMULACRUM_LARGE:
        menv[mid].type = ((mons_zombie_size( menv[mid].number ) == 2)
                            ? MONS_SIMULACRUM_LARGE : MONS_SIMULACRUM_SMALL);

        menv[mid].hit_points = hit_points( menv[mid].hit_dice, 3, 4 );
        menv[mid].armour_class += 4;
        menv[mid].evasion -= 2;
        break;

    case MONS_SPECTRAL_THING:
        menv[mid].type = MONS_SPECTRAL_THING;

        menv[mid].hit_points = hit_points( menv[mid].hit_dice, 4, 4 );
        menv[mid].armour_class += 2;
        menv[mid].evasion += 2;
        break;
    }

    menv[mid].max_hit_points = menv[mid].hit_points;

    if (menv[mid].armour_class < 0)
        menv[mid].armour_class = 0;

    if (menv[mid].evasion < 0)
        menv[mid].evasion = 0;

    if (menv[mid].speed < 3)
        menv[mid].speed = 3;

    menv[mid].energy = 70 + random2(10);
    menv[mid].number = mons_sec2;
    menv[mid].colour = mons_class_colour( cs );
}                               // end define_zombie()

#ifdef USE_RIVERS

static void build_river( unsigned char river_type ) //mv
{
    int tmp;
    int i,j;
    int y, width;

    if (player_in_branch( BRANCH_CRYPT ) || player_in_branch( BRANCH_TOMB ))
        return;

#if DEBUG_DIAGNOSTICS
    mpr( MSGCH_DIAGNOSTICS, "Build: %sriver ",
              (river_type == DNGN_LAVA) ? "lava " : "water " );
#endif

    // Made rivers less wide... min width five rivers were too annoying. -- bwr
    width = 3 + random2(4);

    y = Y_BOUND_1 + 1 + roll_zdice( 3, Y_WIDTH / 3 + 1 ) - width;

    for (i = X_BOUND_1 + 1; i < X_BOUND_2; i++)
    {
        tmp = random2(3);

        if (tmp == 0)
            y++;
        else if (tmp == 1)
            y--;

        tmp = random2(4);

        if (tmp == 0)
            width = MINIMUM( width + 1, 6 );    // 25%
        else if (tmp == 1)
            width = MAXIMUM( width - 1, 2 );    // 25%

        for (j = y; j < y + width ; j++)
        {
            if (!in_bounds( i, j ))
                continue;

            // Note that vaults might have been created in this area!
            // So we'll avoid the silliness of orcs/royal jelly on
            // lava and deep water grids. -- bwr
            if (!one_chance_in(200)
                && grd[i][j] != DNGN_BUILDER_SPECIAL_FLOOR
                && mgrd[i][j] == NON_MONSTER
                && igrd[i][j] == NON_ITEM)
            {
                if (width == 2 && river_type == DNGN_DEEP_WATER && coinflip())
                    grd[i][j] = DNGN_SHALLOW_WATER;
                else
                    grd[i][j] = river_type;
            }
        }
    }
}                               // end build_river()

static void build_lake( unsigned char lake_type ) //mv
{
    int i, j;
    int x_min, x_max, x1, y1, x2, y2;

    if (player_in_branch( BRANCH_CRYPT ) || player_in_branch( BRANCH_TOMB ))
        return;

#if DEBUG_DIAGNOSTICS
    mpr( MSGCH_DIAGNOSTICS, "Build: %slake",
              (lake_type == DNGN_LAVA) ? "lava " : "water " );
#endif

    x_min = 0;
    y1    = 0;
    x_max = 16 + random2(15);
    y2    =  8 + random2(12);
    random_place_rectangle( x_min, y1, x_max, y2 );

    const int avg_x = (x_max - x_min) / 2;
    const int mid_x = x_min + avg_x;
    const int mid_y = y1 + (y2 - y1) / 2;

    // staring nice and centered
    int tmp = random2( avg_x - 1 ) + 1;
    x1 = mid_x - tmp;
    x2 = mid_x + tmp;

    for (j = y1; j < y2; j++)
    {
        if (coinflip())
            x1 += (coinflip() ? 1 : -1) * random2(3);

        if (coinflip())
            x2 += (coinflip() ? 1 : -1) * random2(3);

        if (j < mid_y)
        {
            x2 += random2(3);
            x1 -= random2(3);
        }
        else
        {
            x2 -= random2(3);
            x1 += random2(3);
        }

        if (x2 > x_max)
            x2 = x_max - random2(3);

        if (x1 < x_min)
            x1 = x_min + random2(3);

        if (x2 - x1 < 4 && j > mid_y)
        {
            x2 += random2(3);
            x1 -= random2(3);
        }

        for (i = x1; i < x2 ; i++)
        {
            if (!in_bounds( i, j ))
                continue;

            // Note that vaults might have been created in this area!
            // So we'll avoid the silliness of monsters and items
            // on lava and deep water grids. -- bwr
            // Making lava "crustier", to make it a little different.
            if (!one_chance_in( (lake_type == DNGN_LAVA) ? 10 : 200 )
                && grd[i][j] != DNGN_BUILDER_SPECIAL_FLOOR
                && mgrd[i][j] == NON_MONSTER
                && igrd[i][j] == NON_ITEM)
            {
                grd[i][j] = lake_type;
            }
        }

        if (x2 - x1 <= 2 && j > mid_y)
            break;
    }

    // occasionally make an island (or a peninsula or isthmus, we don't check)
    if (one_chance_in(3))
    {
        // doesn't need to be randomized, the shape will do that already
        x1 = mid_x;
        y1 = mid_y;

        // making a "spotty" island:
        int island_size = roll_dice(2,3);

        for (i = 0; i < island_size; i++)
        {
            if (grd[x1][y1] == lake_type)      grd[x1][y1]   = DNGN_FLOOR;
            if (grd[x1 - 1][y1] == lake_type)  grd[x1 - 1][y1] = DNGN_FLOOR;
            if (grd[x1 + 1][y1] == lake_type)  grd[x1 + 1][y1] = DNGN_FLOOR;
            if (grd[x1][y1 - 1] == lake_type)  grd[x1][y1 - 1] = DNGN_FLOOR;
            if (grd[x1][y1 + 1] == lake_type)  grd[x1][y1 + 1] = DNGN_FLOOR;

            // place a big monster or two
            if (mgrd[x1][y1] == NON_MONSTER && one_chance_in(4))
            {
                mons_place( RANDOM_MONSTER, BEH_SLEEP, MHITNOT, true, x1, y1,
                            you.level_type, PROX_ANYWHERE, you.depth + 10 );
            }

            // place some items (yeah, this can make items in hell)
            if (igrd[x1][y1] == NON_ITEM && coinflip())
            {
                int it = make_item( 1, OBJ_RANDOM, OBJ_RANDOM, true,
                                    you.depth + 5, 250 );

                if (it != NON_ITEM)
                    move_item_to_grid( &it, x1, y1 );
            }

            // sometimes use a fancier terrain
            if (mgrd[x1][y1] == NON_MONSTER && igrd[x1][y1] == NON_ITEM)
            {
                if (one_chance_in(3))
                    place_specific_trap( x1, y1, TRAP_RANDOM );
                else if (one_chance_in(4))
                {
                    tmp = random2(1000);
                    grd[x1][y1] = ((tmp < 500) ? DNGN_ROCK_WALL :
                                   (tmp < 700) ? DNGN_STONE_WALL :
                                   (tmp < 750) ? DNGN_METAL_WALL :
                                   (tmp < 850) ? DNGN_ORCISH_IDOL :
                                   (tmp < 950) ? DNGN_GRANITE_STATUE :
                                   (tmp < 990) ? DNGN_BLUE_FOUNTAIN :
                                   (tmp < 995) ? DNGN_GREEN_CRYSTAL_WALL :
                                   (tmp < 997) ? DNGN_SPARKLING_FOUNTAIN :
                                   (tmp < 998) ? DNGN_SILVER_STATUE
                                               : DNGN_ORANGE_CRYSTAL_STATUE);
                }
            }

            // shift x,y for the next iteration
            if (coinflip())
                x1 += (coinflip() ? 1 : -1);
            else
                y1 += (coinflip() ? 1 : -1);
        }
    }
}                               // end lake()

#endif // USE_RIVERS

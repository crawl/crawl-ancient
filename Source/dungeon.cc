/*
 *  File:       dungeon.cc
 *  Summary:    Functions used when building new levels.
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 *
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

#include "AppHdr.h"
#include "dungeon.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include <assert.h>

#ifdef DOS
#include <conio.h>
#include <io.h>
#include <SYS\STAT.H>
#endif

#include "externs.h"

#include "debug.h"
#include "itemname.h"
#include "maps.h"
#include "mon-pick.h"
#include "monstuff.h"
#include "mon-util.h"
#include "randart.h"
#include "stuff.h"
#include "spl-book.h"
#include "wpn-misc.h"


int bcount_x, bcount_y;
int bi, bj, bk;
int bp;      // *not* global (I think) in [at least] items(), give_item() {dlb}

// these are the variables for individual levels.
// used in builder() and make_trail() only, I think {dlb}
static int corrlength;

// used in builder() and make_trail() only, I think {dlb}
static int intersect_chance = 0;

// used in builder() and make_trail() only, I think {dlb}
static int no_corr;

// used in builder() and make_trail() only, I think {dlb}
static int x_ps, y_ps;

// used in builder() and roguey_level() only, I think {dlb}
static char is_a_specroom = 0;

static int time_run = 0;        // for repeating search for 1-door rooms
int many_many = 0;              // for multiple levels

// possibly needs to remain global for make_trail to work {dlb}
int dir_x, dir_y;

// oddball usage in builder() and make_trail() only, I think -- explore {dlb}
static int finish = 0;

// confusing usage in builder(), special_room(), beehive(), and morgue() {dlb}
static int bno_mons = 0;

int length;
int count_x;

int x1, x2, y1, y2;             // used all over the place ... d'oh!!! {dlb}

//int cx = 0, cy = 0;
//int sx = 0, sy = 0;
//int dx = 0, dy = 0;

static int room_x1, room_x2;
static int room_y1, room_y2;
static char oblique;
static char oblique_max;
static int vsx, vsy;            // used in vaults function

static bool is_weapon_special(int the_weapon);
static bool place_specific_trap(unsigned char spec_x, unsigned char spec_y,
                                unsigned char spec_type);

static bool treasure_area(int many_many, unsigned char ta1_x,
                          unsigned char ta2_x, unsigned char ta1_y,
                          unsigned char ta2_y);

static char plan_3(void);
static char plan_6(void);
static char rare_weapon(unsigned char w_type);
static int pick_an_altar(void);
static int vault_grid(int vx, int vy, int altar_count,
                        FixedVector < char, 7 > &acq_item_class,
                        FixedVector < int, 7 > &mons_array,
                        char vgrid);

static unsigned char item_in_shop(unsigned char shop_type);
static unsigned char plan_1(void);
static unsigned char plan_2(void);
static unsigned char plan_4(char forbid_x1, char forbid_y1, char forbid_x2,
                            char forbid_y2, unsigned char force_wall);

static unsigned char plan_5(void);
static void beehive(void);
static void big_room(void);
static void bigger_room(void);
static void box_room(int bx1, int bx2, int by1, int by2, int wall_type);
static void build_minivaults(int force_vault);
static void build_vaults(int force_vault);
static void check_doors(void);
static void chequerboard(unsigned char cheq1, unsigned char cheq2,
                         unsigned char deleted, unsigned char ch_x1,
                         unsigned char chy1, unsigned char ch_x2,
                         unsigned char chy2);

static void city_level(void);
static void clear_area(unsigned char x_min, unsigned char y_min,
                       unsigned char x_max, unsigned char y_max,
                       unsigned char deleting, unsigned char replacing);

static void define_zombie(char not_zombsize, int ztype, int cs);
static void diamond_rooms(void);
static void generate_abyss(void);
static void give_item(void);
static void hide_doors(void);
static void item_colour(int p);
static void join_the_dots(unsigned char dotx1, unsigned char doty1,
                          unsigned char dotx2, unsigned char doty2,
                          char forbid_x1, char forbid_y1, char forbid_x2,
                          char forbid_y2);

static void labyrinth_level(void);
static void link_items(void);
static void make_room(int max_doors, int doorlevel);
static void make_trail(void);
static void many_pools(unsigned char pool_type);
static void morgue(void);
static void octa_room(unsigned char type_floor);
static void place_altar(void);
static void place_curse_skull(void);
static void place_pool(unsigned char pool_type, unsigned char pool_x1,
                       unsigned char pool_y1, unsigned char pool_x2,
                       unsigned char pool_y2);

static void place_shops(void);
static void place_spec_shop(unsigned char shop_x, unsigned char shop_y,
                            unsigned char force_s_type);

static void place_traps(void);
static void plan_main(char force_plan);
static void prepare_swamp(void);
static void prepare_water(void);
static void roguey_level(void);
static void set_weapon_special(int the_weapon, int spwpn);
static void special_room(void);
static void specr_2(void);
static void spotty_level(bool seeded, int iterations, bool boxy);
static void place_branch_entrances(int dlevel, char level_type);

/*
 **************************************************
 *                                                *
 *             BEGIN PUBLIC FUNCTIONS             *
 *                                                *
 **************************************************
*/
int builder(unsigned int lev_numb, char level_type)
{

    int temp_rand = 0;   // probability determination {dlb}
    int loopy = 0;       // trying to clear out bcount_x from these files {dlb}

    int lava_spaces = 0;
    int water_spaces = 0;
    int aq_creatures = 0;
    int type_of_aq = 0;

    FixedVector < int, 4 > swimming_things;

    int max_doors;              // passed only to make_room() {dlb}
    int doorlevel;              // passed only to make_room() {dlb}
    int roomsize;               // internal to builder() {dlb}
    int no_rooms;               // internal to builder() {dlb}
    int x_start, y_start;       // internal to builder() {dlb}
    bool skipped = false;
    bool done_city = false;

    srandom(time(NULL));

    many_many = lev_numb;

    unsigned char border_type = (many_many != 60) ? DNGN_ROCK_WALL
                                                  : DNGN_METAL_WALL;

    for (bcount_x = 0; bcount_x < GXM; bcount_x++)
    {
        for (bcount_y = 0; bcount_y < GYM; bcount_y++)
        {
            // better than before - border not drawn on *every* iteration {dlb}
            // either fill with border or rock wall:
            if ((bcount_x == 6 || bcount_x == (GXM - 7))
                || (bcount_y == 6 || bcount_y == (GYM - 7)))
            {
                grd[bcount_x][bcount_y] = border_type;
            }
            else
                grd[bcount_x][bcount_y] = DNGN_ROCK_WALL;   // must be value 1

            igrd[bcount_x][bcount_y] = NON_ITEM;
            mgrd[bcount_x][bcount_y] = NON_MONSTER;
        }
    }

    for (loopy = 0; loopy < MAX_TRAPS; loopy++)
    {
        env.trap_type[loopy] = TRAP_UNASSIGNED;
        env.trap_x[loopy] = 1;
        env.trap_y[loopy] = 1;
    }


    for (loopy = 0; loopy < MAX_ITEMS; loopy++)
    {
        mitm.base_type[loopy] = OBJ_UNASSIGNED;
        mitm.sub_type[loopy] = 0;
        mitm.pluses[loopy] = 0;
        mitm.pluses2[loopy] = 0;
        mitm.special[loopy] = 0;
        bcount_y = 100000;      // what the h*** ??? {dlb}
        mitm.quantity[loopy] = 0;
        mitm.colour[loopy] = 0;
        mitm.x[loopy] = 0;
        mitm.y[loopy] = 0;
        mitm.id[loopy] = 0;
        mitm.link[loopy] = NON_ITEM;
    }

    for (loopy = 0; loopy < MAX_MONSTERS; loopy++)
    {
        menv[loopy].type = -1;
        menv[loopy].target_x = random2(GXM);
        menv[loopy].target_y = random2(GYM);
        menv[loopy].behavior = (one_chance_in(10) ? BEH_WANDER : BEH_SLEEP);
        menv[loopy].enchantment1 = 0;
        menv[loopy].enchantment[0] = ENCH_NONE;
        menv[loopy].enchantment[1] = ENCH_NONE;
        menv[loopy].enchantment[2] = ENCH_NONE;

        for (bj = 0; bj < NUM_MONSTER_SLOTS; bj++)
            menv[loopy].inv[bj] = NON_ITEM;

        menv[loopy].number = 250;
    }

    if (level_type == LEVEL_LABYRINTH)
    {
        labyrinth_level();
        return 0;
    }
    else if (level_type == LEVEL_ABYSS)
    {
        generate_abyss();
        goto skip_level;
    }
    else if (level_type == LEVEL_PANDEMONIUM)
    {
        // Could do spotty_level, but that doesn't always put all paired
        // stairs reachable from each other which isn't a problem in normal
        // dungeon but could be in Pandemonium
        if (one_chance_in(15))
        {
            char which_demon = 0;

            do
            {
                which_demon = random2(4);

                // makes these things less likely as you find more
                if (one_chance_in(4))
                    goto normal_pan;
            }
            while (you.unique_creatures[40 + which_demon] == 1);

            you.unique_creatures[40 + which_demon] = 1;
            build_vaults(which_demon + 60);

            //link_items(unique_items, igrd, menv, it);
            // must not call link_items unless returning immediately after
            goto skip_level;
        }

      normal_pan:
        plan_main(0);
        //if ( one_chance_in(3) )
        {
            build_minivaults(300 + random2(9));
            done_city = true;
            goto skip_level;
        }
        done_city = true;

        goto skip_level;
    }

    // here is where "normal" levels begin ... {dlb}
    is_a_specroom = 0;
    done_city = false;

    switch (you.where_are_you)
    {
    case BRANCH_HIVE:
        if (many_many == you.branch_stairs[STAIRS_HIVE]
                                            + branch_depth(STAIRS_HIVE))
        {
            build_vaults(80);
        }
        else
        {
            spotty_level(false, 100 + random2(500), false);
            done_city = true;
        }
        goto skip_level;
        break;

    case BRANCH_SLIME_PITS:
        if (many_many == you.branch_stairs[STAIRS_SLIME_PITS]
                                            + branch_depth(STAIRS_SLIME_PITS))
        {
            build_vaults(81);
        }
        else
        {
            spotty_level(false, 100 + random2(500), false);
            done_city = true;
        }
        goto skip_level;
        break;

    case BRANCH_VAULTS:
        if (many_many == you.branch_stairs[STAIRS_VAULTS]
                                            + branch_depth(STAIRS_VAULTS))
        {
            build_vaults(82);
            goto skip_level;
        }
        break;

    case BRANCH_HALL_OF_BLADES:
        if (many_many == you.branch_stairs[STAIRS_HALL_OF_BLADES]
                                        + branch_depth(STAIRS_HALL_OF_BLADES))
        {
            build_vaults(83);
            goto skip_level;
        }
        break;

    case BRANCH_HALL_OF_ZOT:
        if (many_many == you.branch_stairs[STAIRS_HALL_OF_ZOT]
                                        + branch_depth(STAIRS_HALL_OF_ZOT))
        {
            build_vaults(84);
            goto skip_level;
        }
        break;

    case BRANCH_ECUMENICAL_TEMPLE:
        if (many_many == you.branch_stairs[STAIRS_ECUMENICAL_TEMPLE]
                                    + branch_depth(STAIRS_ECUMENICAL_TEMPLE))
        {
            build_vaults(85);
            goto skip_level;
        }
        break;

    case BRANCH_SNAKE_PIT:
        if (many_many == you.branch_stairs[STAIRS_SNAKE_PIT]
                                    + branch_depth(STAIRS_SNAKE_PIT))
        {
            build_vaults(86);
            goto skip_level;
        }
        break;

    case BRANCH_ELVEN_HALLS:
        if (many_many == you.branch_stairs[STAIRS_ELVEN_HALLS]
                                    + branch_depth(STAIRS_ELVEN_HALLS))
        {
            build_vaults(87);
            goto skip_level;
        }
        break;

    case BRANCH_TOMB:
        if (many_many == you.branch_stairs[STAIRS_TOMB] + 1)
        {
            build_vaults(88);
            goto skip_level;
        }
        else if (many_many == you.branch_stairs[STAIRS_TOMB] + 2)
        {
            build_vaults(89);
            goto skip_level;
        }
        else if (many_many == you.branch_stairs[STAIRS_TOMB] + 3)
        {
            build_vaults(90);
            goto skip_level;
        }
        break;

    case BRANCH_SWAMP:
        if (many_many == you.branch_stairs[STAIRS_SWAMP]
                                            + branch_depth(STAIRS_SWAMP))
        {
            build_vaults(91);
            goto skip_level;
        }
        else
            goto basic_level;
        break;

    case BRANCH_ORCISH_MINES:
        spotty_level(false, 100 + random2(500), false);
        done_city = true;
        goto skip_level;
        break;

    case BRANCH_LAIR:
        if (!one_chance_in(3))
        {
            spotty_level(false, 100 + random2(500), false);
            done_city = true;
            goto skip_level;
        }
        break;

    case BRANCH_VESTIBULE_OF_HELL:
        build_vaults(50);
        link_items();
        return 0;
        break;

    case BRANCH_DIS:
        if (many_many == 33)    // ( == 84 || == 79 || == 89 || == 94 )
        {
            build_vaults(51);
            goto skip_level;
        }
        break;

    case BRANCH_GEHENNA:
        if (many_many == 33)    // ( == 84 || == 79 || == 89 || == 94 )
        {
            build_vaults(52);
            goto skip_level;
        }
        break;

    case BRANCH_COCYTUS:
        if (many_many == 33)    // ( == 84 || == 79 || == 89 || == 94 )
        {
            build_vaults(53);
            goto skip_level;
        }
        break;

    case BRANCH_TARTARUS:
        if (many_many == 33)    // ( == 84 || == 79 || == 89 || == 94 )
        {
            build_vaults(54);
            goto skip_level;
        }
        break;

    default:
        break;
    }

    //Vestibule of Hell 60
    //Dis               75 - 79
    //Gehenna           80 - 84
    //Cocytus           85 - 89
    //Tartarus          90 - 94

    // from here on, can only integrate when many_many has been figured
    // out {dlb}:
    if (you.where_are_you == BRANCH_MAIN_DUNGEON && level_type == LEVEL_DUNGEON
        && (many_many > 7 && many_many < 23) && one_chance_in(7))
    {
        // Can't have vaults on you.where_are_you != BRANCH_MAIN_DUNGEON levels
        build_vaults(100);
        done_city = true;
        goto skip_level;
    }

    if (many_many != 65 && one_chance_in(3))
        skipped = true;

    if (you.where_are_you == BRANCH_DIS || you.where_are_you == BRANCH_VAULTS)
        goto do_city;

    if (many_many > 7 && many_many < 23)
    {
        if (one_chance_in(18))
        {
            spotty_level(false, 0, coinflip());
            done_city = true;
            goto skip_level;
        }

        if (one_chance_in(18))
        {
            bigger_room();
            done_city = true;
            goto skip_level;
        }
    }

    if ((many_many > 2 && many_many < 23) && one_chance_in(3))
    {
        plan_main(0);
        if (one_chance_in(3))
            build_minivaults(200);
        done_city = true;
        goto skip_level;
    }

    //V was 3
    if (!skipped && one_chance_in(7))
    {
        // is_a_specroom can be changed to 2 in this function:
        // in which case it shouldn't be done again.
        roguey_level();

        if (many_many > 6
            && you.where_are_you == BRANCH_MAIN_DUNGEON
            && you.level_type == LEVEL_DUNGEON && one_chance_in(4))
        {
            build_minivaults(200);
            done_city = true;
            goto skip_level;
        }

        if (skipped)
            goto skip_level;
    }
    else
    {
        if (!skipped && many_many > 13
                    && !(you.where_are_you == BRANCH_DIS
                         || you.where_are_you == BRANCH_GEHENNA
                         || you.where_are_you == BRANCH_COCYTUS
                         || you.where_are_you == BRANCH_TARTARUS)
                    && one_chance_in(8))
        {
          do_city:
            if (many_many == 33 && you.where_are_you == BRANCH_DIS)
                build_vaults(51);
            else if (one_chance_in(3))
                city_level();
            else
                plan_main(4);

            done_city = true;
        }
    }

    // is_a_specroom: 0 = none, 2 = in roguey_level (no need for specr_2),
    // 1 = special_room (vault)
    if (is_a_specroom != 2 && many_many > 5 && !done_city
                    && !(you.where_are_you == BRANCH_DIS
                         || you.where_are_you == BRANCH_GEHENNA
                         || you.where_are_you == BRANCH_COCYTUS
                         || you.where_are_you == BRANCH_TARTARUS)
                    && one_chance_in(5))
    {
        is_a_specroom = 1;
        special_room();
    }

  basic_level:
    doorlevel = random2(11);
    corrlength = 2 + random2(14);
    roomsize = 4 + random2(5) + random2(6);
    no_corr = (one_chance_in(100) ? 500 + random2(500) : 30 + random2(200));

    intersect_chance = (one_chance_in(20) ? 400 : random2(20));

    if (!done_city)
    {
        do
        {
            x_start = 35 + random2(30);
            y_start = 35 + random2(20);
        }
        while (grd[x_start][y_start] != DNGN_ROCK_WALL
                    && grd[x_start][y_start] != DNGN_FLOOR);

        grd[x_start][y_start] = DNGN_STONE_STAIRS_DOWN_I;
        x_ps = x_start;
        y_ps = y_start;
        make_trail();
        grd[x_ps][y_ps] = DNGN_STONE_STAIRS_UP_I;

        do
        {
            x_start = 10 + random2(15);
            y_start = 10 + random2(15);
        }
        while (grd[x_start][y_start] != DNGN_ROCK_WALL
                   && grd[x_start][y_start] != DNGN_FLOOR);

        grd[x_start][y_start] = DNGN_STONE_STAIRS_DOWN_II;
        finish = 0;
        x_ps = x_start;
        y_ps = y_start;
        make_trail();
        grd[x_ps][y_ps] = DNGN_STONE_STAIRS_UP_II;

        do
        {
            x_start = 50 + random2(20);
            y_start = 10 + random2(15);
        }
        while (grd[x_start][y_start] != DNGN_ROCK_WALL
                   && grd[x_start][y_start] != DNGN_FLOOR);

        grd[x_start][y_start] = DNGN_STONE_STAIRS_DOWN_III;
        finish = 0;
        x_ps = x_start;
        y_ps = y_start;
        make_trail();
        grd[x_ps][y_ps] = DNGN_STONE_STAIRS_UP_III;

        if (one_chance_in(4))
        {
            do
            {
                x_start = 10 + random2(20);
                y_start = 40 + random2(20);
            }
            while (grd[x_start][y_start] != DNGN_ROCK_WALL
                       && grd[x_start][y_start] != DNGN_FLOOR);

            grd[x_start][y_start] = DNGN_ROCK_STAIRS_DOWN;
            finish = 0;
            x_ps = x_start;
            y_ps = y_start;
            make_trail();
        }

        if (one_chance_in(4))
        {
            do
            {
                x_start = 50 + random2(20);
                y_start = 40 + random2(20);
            }
            while (grd[x_start][y_start] != DNGN_ROCK_WALL
                       && grd[x_start][y_start] != DNGN_FLOOR);

            grd[x_start][y_start] = DNGN_STONE_STAIRS_UP_III;
            finish = 0;
            x_ps = x_start;
            y_ps = y_start;
            make_trail();
        }
    }                           // end of done_city

    if (!done_city)
    {
        if (many_many > 1 && one_chance_in(16))
            big_room();

        if ((you.where_are_you == BRANCH_DIS
                || you.where_are_you == BRANCH_GEHENNA
                || you.where_are_you == BRANCH_COCYTUS
                || you.where_are_you == BRANCH_TARTARUS)
            || (random2(many_many) > 6 && one_chance_in(3)))
        {
            diamond_rooms();
        }

        // make some rooms:
        temp_rand = random2(750);

        no_rooms = ((temp_rand > 63) ? (5 + random2avg(29, 2)) : // 91.47% {dlb}
                    (temp_rand > 14) ? 100                       //  6.53% {dlb}
                                     : 1);                       //  2.00% {dlb}

        max_doors = 2 + random2(8);

        for (bk = 0; bk < no_rooms; bk++)
        {
            room_x1 = 8 + random2(50);
            room_y1 = 8 + random2(40);
            room_x2 = room_x1 + 2 + random2(roomsize);
            room_y2 = room_y1 + 2 + random2(roomsize);

            make_room(max_doors, doorlevel);

            if (time_run > 30)
            {
                time_run = 0;
                bk++;
                continue;
            }
        }

        // make some more rooms:
        no_rooms = 1 + random2(3);
        max_doors = 1;

        for (bk = 0; bk < no_rooms; bk++)
        {
            room_x1 = 8 + random2(55);
            room_y1 = 8 + random2(45);
            room_x2 = room_x1 + 5 + random2(6);
            room_y2 = room_y1 + 5 + random2(6);

            make_room(max_doors, doorlevel);

            if (time_run > 30)
            {
                time_run = 0;
                bk++;
                continue;
            }
        }
    }                           // end  "if (!done_city)"

    if (many_many > 6 && you.where_are_you == BRANCH_MAIN_DUNGEON
                        && you.level_type == LEVEL_DUNGEON && one_chance_in(3))
    {
        build_minivaults(200);
        done_city = true;
        goto skip_level;
    }

    if (many_many > 5
        && !(you.where_are_you == BRANCH_DIS
             || you.where_are_you == BRANCH_GEHENNA
             || you.where_are_you == BRANCH_COCYTUS
             || you.where_are_you == BRANCH_TARTARUS)
        && one_chance_in(10))
    {
        many_pools((coinflip()? DNGN_DEEP_WATER : DNGN_LAVA));
    }

    if (many_many >= 11 && many_many <= 23
        && !(you.where_are_you == BRANCH_DIS
              || you.where_are_you == BRANCH_GEHENNA
              || you.where_are_you == BRANCH_COCYTUS
              || you.where_are_you == BRANCH_TARTARUS)
        && one_chance_in(15))
    {
        do
        {
            bi = random2(GXM);
            bj = random2(GYM);
            if (one_chance_in(500))
                goto out_of_lab;
        }
        while (grd[bi][bj] != DNGN_FLOOR);

        grd[bi][bj] = DNGN_ENTER_LABYRINTH;
    }

  out_of_lab:
    if (is_a_specroom == 1)
        specr_2();

  skip_level:
    /* should goto skip_level if everything is done except:
       items placed, monsters placed, gates etc placed.
       note that there can already be some monsters/items etc present
       Stairs must already be done */

    // time to make the swamp {dlb}:
    if (you.where_are_you == BRANCH_SWAMP)
        prepare_swamp();

    // determine how many monsters present {dlb}:
    if (level_type == LEVEL_ABYSS
        || you.where_are_you == BRANCH_ECUMENICAL_TEMPLE)
    {
        bno_mons = 0;
    }
    else
    {
        bno_mons = random2avg(58, 3);

        if (you.where_are_you == BRANCH_DIS
            || you.where_are_you == BRANCH_GEHENNA
            || you.where_are_you == BRANCH_COCYTUS
            || you.where_are_you == BRANCH_TARTARUS
            || you.where_are_you == BRANCH_HALL_OF_BLADES)
        {
            bno_mons += random2avg(48, 3);
        }

        if (you.where_are_you == BRANCH_HALL_OF_BLADES)
            bno_mons += random2avg(48, 3);

        // unlikely - now only possible in HoB {dlb} 10mar2000
        if (bno_mons > 120)
            bno_mons = 120;
    }

    bi = 0;
    bk = 0;
    int kloppo = 0;

    if (level_type == LEVEL_PANDEMONIUM)
        goto finished_monsters;

    for (kloppo = 0; kloppo < bno_mons; kloppo++)
    {
        FixedVector < int, 2 > passed;

        passed[1] = 1;
        kloppo += place_monster( 250, RANDOM_MONSTER, false, 1, 1, BEH_SLEEP,
                                          MHITNOT, true, many_many, passed );
    }
    // keep kloppo for later (aquatic monsters)

  do_uniques:

    // Unique beasties:
    if (many_many > 0
        && !(you.where_are_you == BRANCH_DIS
             || you.where_are_you == BRANCH_GEHENNA
             || you.where_are_you == BRANCH_COCYTUS
             || you.where_are_you == BRANCH_TARTARUS
             || you.where_are_you == BRANCH_ORCISH_MINES
             || you.where_are_you == BRANCH_HIVE
             || you.where_are_you == BRANCH_LAIR
             || you.where_are_you == BRANCH_ECUMENICAL_TEMPLE)
        && one_chance_in(3))
    {
        int which_unique = 0;   //     30 in total

      redo_uniques:
        which_unique = ((many_many > 19) ? 20 + random2(11) :
                        (many_many > 16) ? 13 + random2(10) :
                        (many_many > 13) ?  9 + random2( 9) :
                        (many_many >  9) ?  6 + random2( 5) :
                        (many_many >  7) ?  4 + random2( 4) :
                        (many_many >  3) ?  2 + random2( 4)
                                         : random2(4));

        if (you.unique_creatures[which_unique])
        {
            if (one_chance_in(3))
                goto redo_uniques;
            else
                goto finished_uniques;
        }
        else
        {
            // note: unique_creatures 40 + used by unique demons
            FixedVector < int, 2 > passed;

            passed[1] = 1;
            kloppo += place_monster( 250, 280 + which_unique, false, 1, 1,
                                BEH_SLEEP, MHITNOT, true, many_many, passed );

            if (one_chance_in(3))
                goto do_uniques;
        }
    }

    // do aquatic and lava monsters:
  finished_uniques:

    // count the number of lava and water tiles {dlb}:
    for (bcount_x = 0; bcount_x < GXM; bcount_x++)
    {
        for (bcount_y = 0; bcount_y < GYM; bcount_y++)
        {
            if (grd[bcount_x][bcount_y] == DNGN_LAVA)
                lava_spaces++;
            else if (grd[bcount_x][bcount_y] == DNGN_DEEP_WATER
                     || grd[bcount_x][bcount_y] == DNGN_SHALLOW_WATER)
            {
                water_spaces++;
            }
        }
    }

    if (lava_spaces > 49)
    {
        for (bcount_x = 0; bcount_x < 4; bcount_x++)
            swimming_things[bcount_x] = MONS_LAVA_WORM + random2(3);

        aq_creatures = random2avg(9, 2) + (random2(lava_spaces) / 10);

        if (aq_creatures > 25)
            aq_creatures = 25;

        for (bcount_x = 0; bcount_x < aq_creatures; bcount_x++)
        {
            type_of_aq = swimming_things[random2(4)];
            FixedVector < int, 2 > passed2;
            passed2[1] = 1;

            kloppo += place_monster( 250, type_of_aq, false, 1, 1, BEH_SLEEP,
                                          MHITNOT, true, many_many, passed2 );

            if (kloppo > 99)
                break;
        }
    }

    if (water_spaces > 49)
    {
        for (bcount_x = 0; bcount_x < 4; bcount_x++)
        {
            // mixing enums and math ticks me off !!! 15jan2000 {dlb}
            swimming_things[bcount_x] = MONS_BIG_FISH + random2(4);

            // swamp worms and h2o elementals generated below: {dlb}
            if (you.where_are_you == BRANCH_SWAMP && !one_chance_in(3))
                swimming_things[bcount_x] = MONS_SWAMP_WORM;
        }

        if (many_many >= 25 && one_chance_in(5))
            swimming_things[0] = MONS_WATER_ELEMENTAL;

        if (many_many == 89)    // Cocytus
            swimming_things[3] = MONS_WATER_ELEMENTAL;

        aq_creatures = random2avg(9, 2) + (random2(water_spaces) / 7);

        if (aq_creatures > 25)
            aq_creatures = 25;

        for (bcount_x = 0; bcount_x < aq_creatures; bcount_x++)
        {
            type_of_aq = swimming_things[random2(4)];

            FixedVector < int, 2 > passed2;
            passed2[1] = 1;

            kloppo += place_monster( 250, type_of_aq, false, 1, 1, BEH_SLEEP,
                                          MHITNOT, true, many_many, passed2 );

            if (kloppo > 99)
                break;
        }
    }

  finished_monsters:

    place_branch_entrances(many_many, level_type);

    check_doors();

    if (you.where_are_you != BRANCH_DIS && you.where_are_you != BRANCH_VAULTS)
        hide_doors();

    if (you.where_are_you != BRANCH_ECUMENICAL_TEMPLE)
        place_traps();

    int no_it = random2avg(32, 3);

    if (many_many > 5 && random2(500 - many_many) <= 3)
        no_it += random2(100);  // rich level!

    int plojy = 0;
    unsigned char specif_type = OBJ_RANDOM;
    int items_levels = many_many;

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
        no_it = 0;
    }
    else
    {
        for (plojy = 0; plojy < no_it; plojy++)
            items(1, specif_type, OBJ_RANDOM, 0, items_levels, 250);

        // Make sure there's a very good chance of a knife being placed
        // in the first five levels, but not a guarantee of one. -- bwr
        if (you.where_are_you == BRANCH_MAIN_DUNGEON
            && level_type == LEVEL_DUNGEON
            && many_many < 5 && one_chance_in(4))
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

    for (unsigned char shcount = 0; shcount < 5; shcount++)
        env.shop_type[shcount] = SHOP_UNASSIGNED;

    if (level_type == LEVEL_DUNGEON
        && !(you.where_are_you == BRANCH_DIS
             || you.where_are_you == BRANCH_GEHENNA
             || you.where_are_you == BRANCH_COCYTUS
             || you.where_are_you == BRANCH_TARTARUS
             || you.where_are_you == BRANCH_LAIR
             || you.where_are_you == BRANCH_ECUMENICAL_TEMPLE))
    {
        place_shops();
    }

    // not really certain what this does - needs to be rectified {dlb}:
    for (bcount_x = 0; bcount_x < GXM; bcount_x++)
    {
        for (bcount_y = 0; bcount_y < GYM; bcount_y++)
        {
            if (grd[bcount_x][bcount_y] == DNGN_STATUE_35
                    || grd[bcount_x][bcount_y] == 105)      // was 'i' {dlb}
            {
                grd[bcount_x][bcount_y] = DNGN_ROCK_WALL;
            }
            else if (grd[bcount_x][bcount_y] == 47
                     || grd[bcount_x][bcount_y] == 36
                     || grd[bcount_x][bcount_y] == DNGN_ENTER_ZOT
                     || grd[bcount_x][bcount_y] == 106) // was 'j' {dlb}
            {
                grd[bcount_x][bcount_y] = DNGN_FLOOR;
            }
        }
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

            if (many_many > you.branch_stairs[STAIRS_VAULTS] + 2)
                vault_wall = DNGN_STONE_WALL;
            if (many_many > you.branch_stairs[STAIRS_VAULTS] + 4)
                vault_wall = DNGN_METAL_WALL;
            if (many_many > you.branch_stairs[STAIRS_VAULTS] + 6
                && one_chance_in(10))
                vault_wall = DNGN_GREEN_CRYSTAL_WALL;
        }
        else if (you.where_are_you == BRANCH_CRYPT)
        {
            vault_wall = DNGN_STONE_WALL;
        }

        for (bi = 1; bi < GXM; bi++)
        {
            for (bj = 1; bj < GYM; bj++)
            {
                if (grd[bi][bj] == DNGN_ROCK_WALL)
                    grd[bi][bj] = vault_wall;
            }
        }
    }

    // Top level of branch levels - replaces up stairs
    // with stairs back to dungeon or wherever:
    for (count_x = 0; count_x < 30; count_x++)
    {
        if (you.branch_stairs[count_x] == 0)
            break;

        if (many_many == you.branch_stairs[count_x] + 1
            && level_type == LEVEL_DUNGEON
            && you.where_are_you == BRANCH_ORCISH_MINES + count_x)
        {
            for (bi = 1; bi < GXM; bi++)
            {
                for (bj = 1; bj < GYM; bj++)
                {
                    if (grd[bi][bj] >= DNGN_STONE_STAIRS_UP_I
                        && grd[bi][bj] <= DNGN_ROCK_STAIRS_UP)
                    {
                        grd[bi][bj] = DNGN_RETURN_DUNGEON_I + count_x;
                    }
                }
            }
        }
    }

    // bottom level of branch - replaces down stairs with up ladders:
    for (count_x = 0; count_x < 30; count_x++)
    {
        if (many_many == you.branch_stairs[count_x] + branch_depth(count_x)
            && level_type == LEVEL_DUNGEON
            && you.where_are_you == BRANCH_ORCISH_MINES + count_x)
        {
            for (bi = 1; bi < GXM; bi++)
            {
                for (bj = 1; bj < GYM; bj++)
                {
                    if (grd[bi][bj] >= DNGN_STONE_STAIRS_DOWN_I
                        && grd[bi][bj] <= DNGN_ROCK_STAIRS_DOWN)
                    {
                        grd[bi][bj] = DNGN_ROCK_STAIRS_UP;
                    }
                }
            }
        }
    }

    if (you.where_are_you == BRANCH_CRYPT)
    {
        if (one_chance_in(3))
            place_curse_skull();

        if (one_chance_in(7))
            place_curse_skull();
    }

    if (you.where_are_you >= BRANCH_ORCISH_MINES
        && you.level_type == LEVEL_DUNGEON && one_chance_in(5))
        place_altar();

    // hall of blades (1 level deal) - no down staircases, thanks!
    if (you.where_are_you == BRANCH_HALL_OF_BLADES)
    {
        for (bi = 1; bi < GXM; bi++)
        {
            for (bj = 1; bj < GYM; bj++)
            {
                if (grd[bi][bj] >= DNGN_STONE_STAIRS_DOWN_I
                    && grd[bi][bj] <= DNGN_ROCK_STAIRS_UP)
                {
                    grd[bi][bj] = DNGN_FLOOR;
                }
            }
        }
    }

    link_items();
    prepare_water();

    return 0;
}                               // end builder()

int place_monster(unsigned char plus_seventy, int typed, bool is_summoning,
                  int px, int py, char behaviour, int hitting,
                  bool allow_bands, int many_many,
                  FixedVector < int, 2 > &passed)
{
    int temp_rand = 0;          // probability determination {dlb}
    int band = BAND_NO_BAND;    // internal to place_monster() {dlb}
    int band_no = 0;            // internal to place_monster() {dlb}

    unsigned char grid_ok = DNGN_FLOOR;
    int worm = 0;
    int plussed = 0;
    int inv_delete = 0;
    int lev_mons;
    int passout;

    int k = 0;

  start_here:
    for (k = 0; k < MAX_MONSTERS; k++)
    {
        if (menv[k].type == -1)
            break;
        else if (k == MAX_MONSTERS - 1)
            return plussed;
    }

    if (is_summoning && mgrd[px][py] != NON_MONSTER)
        return 0;

    bk = k;

    // interesting -- only use for passed[0] I can find {dlb}
    passed[0] = k;

    // basically passes back the index of the monster that is placed {dlb}
    for (inv_delete = 0; inv_delete < NUM_MONSTER_SLOTS; inv_delete++)
        menv[bk].inv[inv_delete] = NON_ITEM;

    // hall of blades:
    if (typed == RANDOM_MONSTER
        && you.where_are_you == BRANCH_HALL_OF_BLADES
        && you.level_type == LEVEL_DUNGEON)
    {
        typed = MONS_DANCING_WEAPON;
    }

    lev_mons = many_many;

    if (typed == RANDOM_MONSTER)
    {
        if (you.where_are_you == BRANCH_MAIN_DUNGEON
            && lev_mons != 51 && one_chance_in(4))
        {
            lev_mons = random2(many_many);
        }
        else
            lev_mons = many_many;

        if (you.where_are_you == BRANCH_MAIN_DUNGEON && lev_mons < 28)
        {
            // potentially nasty surprise, but very rare
            if (one_chance_in(5000))
                lev_mons = random2(27);

            // slightly out of depth monsters are more common:
            if (one_chance_in(50))
                lev_mons += random2(6);

            if (lev_mons > 27)
                lev_mons = 27;
        }

        /* Abyss or Pandemonium. Almost never called from Pan;
           probably only if a rand demon gets summon anything spell */
        if (lev_mons == 51
            || you.level_type == LEVEL_PANDEMONIUM
            || you.level_type == LEVEL_ABYSS)
        {
            do
            {
                do
                {
                    // was: random2(400) {dlb}
                    menv[bk].type = random2(NUM_MONSTERS);
                }
                while (!mons_abyss(menv[bk].type));
            }
            while (random2avg(100, 2) > mons_rare_abyss(menv[bk].type));
        }
        else
        {
            int level, diff, chance;

            do
            {
                do
                {
                    // was: random2(400) {dlb}
                    menv[bk].type = random2(NUM_MONSTERS);
                }
                while (mons_rarity(menv[bk].type) == 0);

                // little touch of aliasing to reveal what the while
                // condition actually is, any good optimizing compiler
                // was probably doing it this way (instead of five
                // function calls) -- bwr
                level  = mons_level( menv[bk].type );
                diff   = level - lev_mons;
                chance = mons_rarity( menv[bk].type ) - (diff * diff);
            }
            while ( !(lev_mons > level - 5 && lev_mons < level + 5
                         && random2avg(100, 2) <= chance) );
        }
    }
    else
        menv[bk].type = typed;

    if (monster_habitat(menv[bk].type) != DNGN_FLOOR)
    {
        menv[bk].number = 0;
        grid_ok = monster_habitat(menv[bk].type);
    }

    if (band != BAND_NO_BAND && band_no > 0)
    {
        switch (band)
        {
        case BAND_KOBOLDS:
            menv[bk].type = MONS_KOBOLD;
            break;

        case BAND_ORC_KNIGHT:
        case BAND_ORC_HIGH_PRIEST:
            temp_rand = random2(32);
            menv[bk].type = ((temp_rand > 18) ? MONS_ORC :          // 13 in 32
                             (temp_rand >  8) ? MONS_ORC_WARRIOR :  // 10 in 32
                             (temp_rand >  6) ? MONS_OGRE :         //  2 in 32
                             (temp_rand >  4) ? MONS_TROLL :        //  2 in 32
                             (temp_rand >  2) ? MONS_ORC_WIZARD :   //  2 in 32
                             (temp_rand >  0) ? MONS_ORC_PRIEST     //  2 in 32
                                              : MONS_ORC_SORCEROR); //  1 in 32
            break;

        case BAND_KILLER_BEES:
            menv[bk].type = MONS_KILLER_BEE;
            break;
        case BAND_FLYING_SKULLS:
            menv[bk].type = MONS_FLYING_SKULL;
            break;
        case BAND_SLIME_CREATURES:
            menv[bk].type = MONS_SLIME_CREATURE;
            break;
        case BAND_YAKS:
            menv[bk].type = MONS_YAK;
            break;

        case BAND_UGLY_THINGS:
            menv[bk].type = ((many_many > 21 && one_chance_in(4)) ?
                                    MONS_VERY_UGLY_THING : MONS_UGLY_THING);
            break;

        case BAND_HELL_HOUNDS:
            menv[bk].type = MONS_HELL_HOUND;
            break;
        case BAND_JACKALS:
            menv[bk].type = MONS_JACKAL;
            break;
        case BAND_GNOLLS:
            menv[bk].type = MONS_GNOLL;
            break;
        case BAND_BUMBLEBEES:
            menv[bk].type = MONS_BUMBLEBEE;
            break;
        case BAND_CENTAURS:
            menv[bk].type = MONS_CENTAUR;
            break;
        case BAND_YAKTAURS:
            menv[bk].type = MONS_YAKTAUR;
            break;
        case BAND_INSUBSTANTIAL_WISPS:
            menv[bk].type = MONS_INSUBSTANTIAL_WISP;
            break;
        case BAND_DEATH_YAKS:
            menv[bk].type = MONS_DEATH_YAK;
            break;

        case BAND_NECROMANCER:                // necromancer
            temp_rand = random2(13);
            menv[bk].type = ((temp_rand > 9) ? MONS_ZOMBIE_SMALL :   // 3 in 13
                             (temp_rand > 6) ? MONS_ZOMBIE_LARGE :   // 3 in 13
                             (temp_rand > 3) ? MONS_SKELETON_SMALL : // 3 in 13
                             (temp_rand > 0) ? MONS_SKELETON_LARGE   // 3 in 13
                                             : MONS_NECROPHAGE);     // 1 in 13
            break;

        case BAND_BALRUG:
            menv[bk].type = (coinflip()? MONS_NEQOXEC : MONS_ORANGE_DEMON);
            break;
        case BAND_CACODEMON:
            menv[bk].type = MONS_LEMURE;
            break;

        case BAND_EXECUTIONER:
            menv[bk].type = (coinflip() ? MONS_ABOMINATION_SMALL
                                        : MONS_ABOMINATION_LARGE);
            break;

        case BAND_HELLWING:
            menv[bk].type = (coinflip() ? MONS_HELLWING
                                        : MONS_SMOKE_DEMON);
            break;

        case BAND_DEEP_ELF_FIGHTER:    // deep elf fighter
            temp_rand = random2(11);
            menv[bk].type =
                    ((temp_rand >  4) ? MONS_DEEP_ELF_SOLDIER : // 6 in 11
                     (temp_rand == 4) ? MONS_DEEP_ELF_FIGHTER : // 1 in 11
                     (temp_rand == 3) ? MONS_DEEP_ELF_KNIGHT :  // 1 in 11
                     (temp_rand == 2) ? MONS_DEEP_ELF_CONJURER :// 1 in 11
                     (temp_rand == 1) ? MONS_DEEP_ELF_MAGE      // 1 in 11
                                      : MONS_DEEP_ELF_PRIEST);  // 1 in 11
            break;
        }
    }                           // end "if ( band != 0 && band_no > 0 )"

    // this is in addition to the above {dlb}:
    switch (band)
    {
    case BAND_ORCS:
        if (band_no > 0)
            menv[bk].type = MONS_ORC;
        if (one_chance_in(5))
            menv[bk].type = MONS_ORC_WIZARD;
        if (one_chance_in(7))
            menv[bk].type = MONS_ORC_PRIEST;
        break;

    case BAND_HELL_KNIGHTS:
        if (band_no > 0)
            menv[bk].type = MONS_HELL_KNIGHT;
        if (one_chance_in(4))
            menv[bk].type = MONS_NECROMANCER;
        break;

    //case 12 is orc high priest

    case BAND_OGRE_MAGE:
        if (band_no > 0)
            menv[bk].type = MONS_OGRE;
        if (one_chance_in(3))
            menv[bk].type = MONS_TWO_HEADED_OGRE;
        break;                  // ogre mage

        // comment does not match value (30, TWO_HEADED_OGRE) prior to
        // enum replacement [!!!] 14jan2000 {dlb}

    case BAND_DEEP_ELF_KNIGHT:                    // deep elf knight
        temp_rand = random2(208);
        menv[bk].type =
                ((temp_rand > 159) ? MONS_DEEP_ELF_SOLDIER :    // 23.08%
                 (temp_rand > 111) ? MONS_DEEP_ELF_FIGHTER :    // 23.08%
                 (temp_rand >  79) ? MONS_DEEP_ELF_KNIGHT :     // 15.38%
                 (temp_rand >  51) ? MONS_DEEP_ELF_MAGE :       // 13.46%
                 (temp_rand >  35) ? MONS_DEEP_ELF_PRIEST :     //  7.69%
                 (temp_rand >  19) ? MONS_DEEP_ELF_CONJURER :   //  7.69%
                 (temp_rand >   3) ? MONS_DEEP_ELF_SUMMONER :    // 7.69%
                 (temp_rand ==  3) ? MONS_DEEP_ELF_DEMONOLOGIST :// 0.48%
                 (temp_rand ==  2) ? MONS_DEEP_ELF_ANNIHILATOR : // 0.48%
                 (temp_rand ==  1) ? MONS_DEEP_ELF_SORCEROR      // 0.48%
                                   : MONS_DEEP_ELF_DEATH_MAGE);  // 0.48%
        break;

    case BAND_DEEP_ELF_HIGH_PRIEST:                // deep elf high priest
        temp_rand = random2(16);
        menv[bk].type =
                ((temp_rand > 12) ? MONS_DEEP_ELF_SOLDIER :     // 3 in 16
                 (temp_rand >  9) ? MONS_DEEP_ELF_FIGHTER :     // 3 in 16
                 (temp_rand >  6) ? MONS_DEEP_ELF_PRIEST :      // 3 in 16
                 (temp_rand == 6) ? MONS_DEEP_ELF_MAGE :        // 1 in 16
                 (temp_rand == 5) ? MONS_DEEP_ELF_SUMMONER :    // 1 in 16
                 (temp_rand == 4) ? MONS_DEEP_ELF_CONJURER :    // 1 in 16
                 (temp_rand == 3) ? MONS_DEEP_ELF_DEMONOLOGIST :// 1 in 16
                 (temp_rand == 2) ? MONS_DEEP_ELF_ANNIHILATOR : // 1 in 16
                 (temp_rand == 1) ? MONS_DEEP_ELF_SORCEROR      // 1 in 16
                                  : MONS_DEEP_ELF_DEATH_MAGE);  // 1 in 16
        break;

    case BAND_KOBOLD_DEMONOLOGIST:
        temp_rand = random2(13);
        menv[bk].type = ((temp_rand > 4) ? MONS_KOBOLD :             // 8 in 13
                         (temp_rand > 0) ? MONS_BIG_KOBOLD           // 4 in 13
                                         : MONS_KOBOLD_DEMONOLOGIST);// 1 in 13
        break;

    case BAND_NAGAS:
        menv[bk].type = MONS_NAGA;
        break;
    case BAND_WAR_DOGS:
        menv[bk].type = MONS_WAR_DOG;
        break;
    case BAND_GREY_RATS:
        menv[bk].type = MONS_GREY_RAT;
        break;
    case BAND_GREEN_RATS:
        menv[bk].type = MONS_GREEN_RAT;
        break;
    case BAND_ORANGE_RATS:
        menv[bk].type = MONS_ORANGE_RAT;
        break;
    case BAND_SHEEP:
        menv[bk].type = MONS_SHEEP;
        break;
    case BAND_GHOULS:
        menv[bk].type = (coinflip() ? MONS_GHOUL : MONS_NECROPHAGE);
        break;
    case BAND_DEEP_TROLLS:
        menv[bk].type = MONS_DEEP_TROLL;
        break;
    case BAND_HOGS:
        menv[bk].type = MONS_HOG;
        break;
    case BAND_HELL_HOGS:
        menv[bk].type = MONS_HELL_HOG;
        break;
    case BAND_GIANT_MOSQUITOES:
        menv[bk].type = MONS_GIANT_MOSQUITO;
        break;
    case BAND_BOGGARTS:
        menv[bk].type = MONS_BOGGART;
        break;
    case BAND_BLINK_FROGS:
        menv[bk].type = MONS_BLINK_FROG;
        break;
    case BAND_SKELETAL_WARRIORS:
        menv[bk].type = MONS_SKELETAL_WARRIOR;
        break;
    }

    if (monster_habitat(menv[bk].type) == DNGN_FLOOR)
        menv[bk].number = 250;

    define_monster(bk, menv);

    if (menv[bk].type >= MONS_TERENCE && menv[bk].type <= MONS_BORIS)
        you.unique_creatures[menv[bk].type - 280] = 1;

    plussed++;

    if (plus_seventy != 250)
        menv[bk].number = plus_seventy;

    if (menv[bk].type == MONS_ZOMBIE_SMALL
        || menv[bk].type == MONS_ZOMBIE_LARGE
        || menv[bk].type == MONS_SIMULACRUM_SMALL
        || menv[bk].type == MONS_SIMULACRUM_LARGE
        || menv[bk].type == MONS_SKELETON_SMALL
        || menv[bk].type == MONS_SKELETON_LARGE
        || menv[bk].type == MONS_SPECTRAL_THING)
    {
        define_zombie( 3, plus_seventy,
                          ((plus_seventy != 250) ? menv[bk].type : 250) );
    }

    if (mons_flag(menv[bk].type, M_INVIS))
    {
        menv[bk].enchantment1 = 1;
        menv[bk].enchantment[2] = ENCH_INVIS;
    }

    if (menv[bk].type == MONS_SHAPESHIFTER)
    {
        menv[bk].enchantment1 = 1;
        menv[bk].enchantment[1] = ENCH_SHAPESHIFTER;
    }
    else if (menv[bk].type == MONS_GLOWING_SHAPESHIFTER)
    {
        menv[bk].enchantment1 = 1;
        menv[bk].enchantment[1] = ENCH_GLOWING_SHAPESHIFTER;
    }
    else if (menv[bk].type == MONS_BUTTERFLY
             || menv[bk].type == MONS_FIRE_VORTEX
             || menv[bk].type == MONS_SPATIAL_VORTEX
             || menv[bk].type == MONS_BALL_LIGHTNING
             || menv[bk].type == MONS_VAPOUR)
    {
        menv[bk].enchantment1 = 1;
        menv[bk].enchantment[0] = ENCH_CONFUSION;
    }

    switch (band)
    {
    case BAND_NO_BAND:                     // no band
      give_up_on_band:
        passout = 0;

        if (is_summoning)
        {
            menv[bk].x = px;
            menv[bk].y = py;
        }
        else if (grid_ok == DNGN_FLOOR)
        {
            do
            {
                menv[bk].x = 10 + random2(GXM - 10);
                menv[bk].y = 10 + random2(GYM - 10);
            }
            while ((grd[menv[bk].x][menv[bk].y] < DNGN_FLOOR
                        || mgrd[menv[bk].x][menv[bk].y] != NON_MONSTER)
                    || (passed[1] == 0
                        && menv[bk].x < you.x_pos + 7
                        && menv[bk].x > you.x_pos - 7
                        && menv[bk].y < you.y_pos + 7
                        && menv[bk].y > you.y_pos - 7
                        && !one_chance_in(100)));
        }
        else
        {
            do
            {
                menv[bk].x = 10 + random2(GXM - 10);
                menv[bk].y = 10 + random2(GYM - 10);
            }
            while ((grd[menv[bk].x][menv[bk].y] != grid_ok
                    && (grid_ok != DNGN_DEEP_WATER
                        || grd[menv[bk].x][menv[bk].y] != DNGN_SHALLOW_WATER))
                    || (menv[bk].x == you.x_pos && menv[bk].y == you.y_pos));
        }

        bj = 0;
        break;

    default:
        menv[bk].x = menv[bk - 1].x;
        menv[bk].y = menv[bk - 1].y;
        passout = 0;

        do
        {
            menv[bk].x += random2(3) - 1;
            menv[bk].y += random2(3) - 1;

            if (menv[bk].x > (GXM - 1)
                || menv[bk].x < 5 || menv[bk].y > (GYM - 1) || menv[bk].y < 5)
            {
                menv[bk].x = menv[bk - 1].x;
                menv[bk].y = menv[bk - 1].y;
            }

            passout++;

            if (passout > 10000)
            {
                band_no--;
                goto give_up_on_band;
            }
        }
        while (grd[menv[bk].x][menv[bk].y] < DNGN_FLOOR
               || mgrd[menv[bk].x][menv[bk].y] != NON_MONSTER
               || (menv[bk].x == you.x_pos && menv[bk].y == you.y_pos));

        band_no--;
        // cannot have bands of water creatures!
        break;
    }                           // end "switch (band)"

    if (allow_bands)
    {
        if (bk < MAX_MONSTERS - 50 && band == 0 && band_no == 0)
        {
            switch (menv[bk].type)
            {
            case MONS_ORC:
                if (coinflip())
                    break;
                // intentional fall-through {dlb}
            case MONS_ORC_WARRIOR:
                band = BAND_ORCS;       // orcs
                band_no = 2 + random2(3);
                break;

            case MONS_BIG_KOBOLD:
                if (many_many > 3)
                {
                    band = BAND_KOBOLDS;
                    band_no = 2 + random2(6);
                }
                break;

            case MONS_ORC_KNIGHT:
            case MONS_ORC_WARLORD:
                band = BAND_ORC_KNIGHT;       // orcs + knight
                band_no = 3 + random2(4);
                break;
            case MONS_KILLER_BEE:
                band = BAND_KILLER_BEES;       // killer bees
                band_no = 2 + random2(4);
                break;
            case MONS_FLYING_SKULL:
                band = BAND_FLYING_SKULLS;       // flying skulls
                band_no = 2 + random2(4);
                break;
            case MONS_SLIME_CREATURE:
                band = BAND_SLIME_CREATURES;       // slime creatures
                band_no = 2 + random2(4);
                break;
            case MONS_YAK:
                band = BAND_YAKS;       // yaks
                band_no = 2 + random2(4);
                break;
            case MONS_UGLY_THING:
                band = BAND_UGLY_THINGS;       // ugly things
                band_no = 2 + random2(4);
                break;
            case MONS_HELL_HOUND:
                band = BAND_HELL_HOUNDS;       // hell hound
                band_no = 2 + random2(3);
                break;
            case MONS_JACKAL:
                band = BAND_JACKALS;      // jackal
                band_no = 1 + random2(3);
                break;
            case MONS_HELL_KNIGHT:
            case MONS_MARGERY:
                band = BAND_HELL_KNIGHTS;      // hell knight
                band_no = 4 + random2(4);
                break;
            case MONS_JOSEPHINE:
            case MONS_NECROMANCER:
            case MONS_VAMPIRE_MAGE:
                band = BAND_NECROMANCER;      // necromancer
                band_no = 4 + random2(4);
                break;
            case MONS_ORC_HIGH_PRIEST:
                band = BAND_ORC_HIGH_PRIEST;      // orc high priest
                band_no = 4 + random2(4);
                break;
            case MONS_GNOLL:
                band = BAND_GNOLLS;      // gnoll
                band_no = ((coinflip())? 3 : 2);
                break;
            case MONS_BUMBLEBEE:
                band = BAND_BUMBLEBEES;      // bumble bees
                band_no = 2 + random2(4);
                break;
            case MONS_CENTAUR:
            case MONS_CENTAUR_WARRIOR:
                if (many_many > 9 && one_chance_in(3))
                {
                    band = BAND_CENTAURS;  // centaurs
                    band_no = 2 + random2(4);
                }
                break;

            case MONS_YAKTAUR:
            case MONS_YAKTAUR_CAPTAIN:
                if (coinflip())
                {
                    band = BAND_YAKTAURS;  // yaktaurs
                    band_no = 2 + random2(3);
                }
                break;

            case MONS_DEATH_YAK:
                band = BAND_DEATH_YAKS;      // death yaks
                band_no = 2 + random2(4);
                break;
            case MONS_INSUBSTANTIAL_WISP:
                band = BAND_INSUBSTANTIAL_WISPS;      // wisps
                band_no = 4 + random2(5);
                break;
            case MONS_OGRE_MAGE:
                band = BAND_OGRE_MAGE;      // ogre mage
                band_no = 4 + random2(4);
                break;
            case MONS_BALRUG:
                band = BAND_BALRUG;      // RED gr demon
                band_no = 2 + random2(3);
                break;
            case MONS_CACODEMON:
                band = BAND_CACODEMON;      // BROWN gr demon
                band_no = 1 + random2(3);
                break;

            case MONS_EXECUTIONER:
                if (coinflip())
                {
                    band = BAND_EXECUTIONER;  // DARKGREY gr demon
                    band_no = 1 + random2(3);
                }
                break;

            case MONS_HELLWING:
                if (coinflip())
                {
                    band = BAND_HELLWING;  // LIGHTGREY gr demon
                    band_no = 1 + random2(4);
                }
                break;

            case MONS_DEEP_ELF_FIGHTER:
                if (coinflip())
                {
                    band = BAND_DEEP_ELF_FIGHTER;  // deep elf warrior
                    band_no = 3 + random2(4);
                }
                break;

            case MONS_DEEP_ELF_KNIGHT:
                if (coinflip())
                {
                    band = BAND_DEEP_ELF_KNIGHT;  // deep elf knight
                    band_no = 3 + random2(4);
                }
                break;

            case MONS_DEEP_ELF_HIGH_PRIEST:
                if (coinflip())
                {
                    band = BAND_DEEP_ELF_HIGH_PRIEST;  // deep elf high priest
                    band_no = 3 + random2(4);
                }
                break;

            case MONS_KOBOLD_DEMONOLOGIST:
                if (coinflip())
                {
                    band = BAND_KOBOLD_DEMONOLOGIST;  // kobold demonologist
                    band_no = 3 + random2(6);
                }
                break;

            case MONS_NAGA_MAGE:
            case MONS_NAGA_WARRIOR:
                band = BAND_NAGAS;      // Nagas
                band_no = 3 + random2(4);
                break;
            case MONS_WAR_DOG:
                band = BAND_WAR_DOGS;      // war dogs
                band_no = 2 + random2(4);
                break;
            case MONS_GREY_RAT:
                band = BAND_GREY_RATS;      // grey rats
                band_no = 4 + random2(6);
                break;
            case MONS_GREEN_RAT:
                band = BAND_GREEN_RATS;      // green rats
                band_no = 4 + random2(6);
                break;
            case MONS_ORANGE_RAT:
                band = BAND_ORANGE_RATS;      // orange rats
                band_no = 3 + random2(4);
                break;
            case MONS_SHEEP:
                band = BAND_SHEEP;      // sheep
                band_no = 3 + random2(5);
                break;
            case MONS_GHOUL:
                band = BAND_GHOULS;      // ghoul
                band_no = 2 + random2(3);
                break;
            case MONS_HOG:
                band = BAND_HOGS;      // hog
                band_no = 1 + random2(3);
                break;
            case MONS_GIANT_MOSQUITO:
                band = BAND_GIANT_MOSQUITOES;      // mosquito
                band_no = 1 + random2(3);
                break;
            case MONS_DEEP_TROLL:
                band = BAND_DEEP_TROLLS;      // deep troll
                band_no = 3 + random2(3);
                break;
            case MONS_HELL_HOG:
                band = BAND_HELL_HOGS;      // hell-hog
                band_no = 1 + random2(3);
                break;
            case MONS_BOGGART:
                band = BAND_BOGGARTS;      // boggart
                band_no = 2 + random2(3);
                break;
            case MONS_BLINK_FROG:
                band = BAND_BLINK_FROGS;      // blink frog
                band_no = 2 + random2(3);
                break;
            case MONS_SKELETAL_WARRIOR:
                band = BAND_SKELETAL_WARRIORS;      // skeletal warrior
                band_no = 2 + random2(3);
                break;
            }                   // end "switch (menv[bk].type)"
        }     // end "if (bk < MAX_MONSTERS - 50 && band == 0 && band_no == 0)"
    }                           // end "if (allow_bands)"

    if (band > 0 && band_no == 0)
        band = 0;

    //if ( monster_habitat(menv[bk].type) == DNGN_FLOOR )
    //{
    mgrd[menv[bk].x][menv[bk].y] = bk;
    //}

    if (mons_itemuse(menv[bk].type) > 0
        || (menv[bk].type == MONS_DANCING_WEAPON && plus_seventy != 1))
        give_item();

    if (menv[bk].type == MONS_TWO_HEADED_OGRE
        || menv[bk].type == MONS_EROLCHA)
        give_item();

    // give manticores 8 to 16 spike volleys.
    // they're not spellcasters so this doesn't screw anything up.
    if (menv[bk].type == MONS_MANTICORE)
        menv[bk].number = 8 + random2(9);

    menv[bk].behavior = behaviour;
    menv[bk].monster_foe = hitting;

    if (menv[bk].type == MONS_SHAPESHIFTER)
    {
        menv[bk].enchantment1 = 1;
        menv[bk].enchantment[1] = ENCH_SHAPESHIFTER;
    }

    if (menv[bk].type == MONS_GLOWING_SHAPESHIFTER)
    {
        menv[bk].enchantment1 = 1;
        menv[bk].enchantment[1] = ENCH_GLOWING_SHAPESHIFTER;
    }

    if (mons_flag(menv[bk].type, M_INVIS))
    {
        menv[bk].enchantment1 = 1;
        menv[bk].enchantment[2] = ENCH_INVIS;
    }

    if (menv[bk].type == MONS_BUTTERFLY)
    {
        menv[bk].enchantment1 = 1;
        menv[bk].enchantment[0] = ENCH_CONFUSION;
    }

    if (menv[bk].type == MONS_DANCING_WEAPON)
        menv[bk].number = mitm.colour[menv[bk].inv[MSLOT_WEAPON]];

    if (band != 0 || worm != 0)
        goto start_here;

    return plussed;             // which is either 0 or 1, I think {dlb}
}                               // end place_monster()

// OBJ_RANDOM used for force_class *and* force_type calls -- not a typo {dlb}

int items(unsigned char allow_uniques,  // not just true-false,
                                        // because of BCR acquirement hack
          unsigned char force_class,    // desired OBJECTS class {dlb}
          unsigned char force_type,     // desired SUBTYPE - enum varies by OBJ
          int force_place,              // ???
          int many_many,        // level of the item, can differ from global
          int force_spec)       // weapon / armour racial categories
{

    int temp_rand = 0;          // probability determination {dlb}
    int range_charges = 0;      // for OBJ_WANDS charge count {dlb}
    unsigned char temp_value = 0;       // temporary value storage {dlb}
    int loopy = 0;              // just another loop variable {dlb}


    int quant = 0;
    int pot = 0;                // argh! just for potions!!! {dlb}
    FixedVector < int, SPELLBOOK_SIZE > fpass;

    // used in books:
    //char strungy[10];
    //int bkk = 0;
    int icky = 0;
    //int numbo = 0;
    //int multip = 0;
    //int xj = 0;
    int bp = 0;

    int no_unique = 0;

    // find an emtpy space for the item:
    // should this not go to 500 instead? {dlb}
    for (bp = 0; bp < 400; bp++)
    {
        if (mitm.base_type[bp] == OBJ_UNASSIGNED || mitm.quantity[bp] == 0)
            break;
        if (bp == 380)
            return NON_ITEM;
    }

    // clear all properties except mitm.base_type <used in switch below> {dlb}:
    mitm.sub_type[bp] = 0;
    mitm.id[bp] = 0;
    mitm.special[bp] = 0;
    mitm.pluses[bp] = 0;
    mitm.pluses2[bp] = 0;

    // cap many_many unless an acquirement-level item {dlb}:
    if (many_many > 50 && many_many != 351)
        many_many = 50;

    // determine base_type for item generated {dlb}:
    if (force_class != OBJ_RANDOM)
        mitm.base_type[bp] = force_class;
    else
    {
        // nice and large for subtle differences {dlb}
        temp_rand = random2(10000);

        mitm.base_type[bp] = ((temp_rand <  74)  ? OBJ_STAVES :   //  0.74%
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
        if (many_many > 7 && (20 + many_many) >= random2(3500))
            mitm.base_type[bp] = OBJ_MISCELLANY;
    }

    // determine sub_type accordingly {dlb}:
    switch (mitm.base_type[bp])
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
                mitm.sub_type[bp] = temp_value;
                break;
            }
        }

        // this all needs to be compacted (wasteful) {dlb}:
        if (random2(200) < 20 - many_many)
            mitm.sub_type[bp] = WPN_KNIFE;
        if (random2(200) < 20 - many_many)
            mitm.sub_type[bp] = WPN_QUARTERSTAFF;
        if (random2(200) < 20 - many_many)
            mitm.sub_type[bp] = WPN_SLING;
        if (random2(200) < 20 - many_many)
            mitm.sub_type[bp] = WPN_SPEAR;
        if (random2(200) < 20 - many_many)
            mitm.sub_type[bp] = WPN_HAND_AXE;
        if (random2(200) < 20 - many_many)
            mitm.sub_type[bp] = WPN_DAGGER;
        if (random2(200) < 20 - many_many)
            mitm.sub_type[bp] = WPN_MACE;
        if (random2(200) < 20 - many_many)
            mitm.sub_type[bp] = WPN_DAGGER;
        if (random2(200) < 20 - many_many)
            mitm.sub_type[bp] = WPN_CLUB;
        if (random2(200) < 20 - many_many)
            mitm.sub_type[bp] = WPN_HAMMER;
        if (random2(200) < 20 - many_many)
            mitm.sub_type[bp] = WPN_WHIP;
        if (random2(200) < 20 - many_many)
            mitm.sub_type[bp] = WPN_SABRE;

        // place ultra-rare "0" weapons:
        if (many_many > 6 && random2(100) < (10 + many_many)
                                                        && one_chance_in(30))
        {
            // this replaced the infinite loop (wasteful) -- may need
            // to make into its own function to allow ease of tweaking
            // distribution {dlb}:
            temp_rand = random2(9);

            mitm.sub_type[bp] = ((temp_rand == 8) ? WPN_DEMON_BLADE :
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
            if (mitm.sub_type[bp] != WPN_CLUB && many_many > 2
                && random2(2000) <= 100 + (many_many * 3) && coinflip())
            {
                if (you.level_type != LEVEL_ABYSS
                    && you.level_type != LEVEL_PANDEMONIUM
                    && one_chance_in(50))
                {
                    icky = find_okay_unrandart(OBJ_WEAPONS);

                    if (icky != -1)
                    {
                        quant = 1;
                        make_item_unrandart(icky, bp);
                        break;
                    }
                }

                mitm.special[bp] = 26 + random2(4);
                mitm.pluses[bp] = 50;
                mitm.pluses2[bp] = 50;
                mitm.pluses[bp] += random2(7);
                mitm.pluses2[bp] += random2(7);

                if (one_chance_in(3))
                    mitm.pluses[bp] += random2(7);

                if (one_chance_in(3))
                    mitm.pluses2[bp] += random2(7);

                if (one_chance_in(9))
                    mitm.pluses[bp] -= random2(7);

                if (one_chance_in(9))
                    mitm.pluses2[bp] -= random2(7);

                quant = 1;

                if (one_chance_in(4))
                {
                    mitm.pluses[bp] = 150 - random2(6);
                    mitm.pluses2[bp] = 50 - random2(6);
                }
                else if ((mitm.pluses[bp] < 50 || mitm.pluses2[bp] < 50)
                         && !one_chance_in(3))
                {
                    mitm.pluses[bp] += 100;     /* cursed! nasty */
                }
                break;
            }

            if (many_many > 6
                && random2(3000) <= 30 + (many_many * 3) && one_chance_in(20))
            {
                no_unique = random2(12);

                if (no_unique < 7)
                {
                    if (you.unique_items[no_unique] == 1
                        || (many_many == 51
                            && you.unique_items[no_unique] == 0)
                        || (you.unique_items[no_unique] == 2
                            && many_many != 51))
                    {
                        goto out_of_uniques;
                    }
                }
                else if (you.unique_items[24 + no_unique - 7] == 1
                         || (many_many == 51
                             && you.unique_items[24 + no_unique - 7] == 0)
                         || (you.unique_items[24 + no_unique - 7] == 2
                             && many_many != 51))
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
                mitm.pluses[bp] = 50;
                mitm.pluses2[bp] = 50;

                switch (no_unique)
                {
                case 0:
                    mitm.sub_type[bp] = WPN_LONG_SWORD;
                    mitm.special[bp] = NWPN_SINGING_SWORD;
                    mitm.pluses[bp] += 7;
                    mitm.pluses2[bp] += 6;
                    break;
                case 1:
                    mitm.sub_type[bp] = WPN_BATTLEAXE;
                    mitm.special[bp] = NWPN_WRATH_OF_TROG;
                    mitm.pluses[bp] += 3;
                    mitm.pluses2[bp] += 11;
                    break;
                case 2:
                    mitm.sub_type[bp] = WPN_SCYTHE;
                    mitm.special[bp] = NWPN_SCYTHE_OF_CURSES;
                    mitm.pluses[bp] += 11;
                    mitm.pluses2[bp] += 11;
                    break;
                case 3:
                    mitm.sub_type[bp] = WPN_MACE;
                    mitm.special[bp] = NWPN_MACE_OF_VARIABILITY;
                    mitm.pluses[bp] += random2(12) - 4;
                    mitm.pluses2[bp] += random2(12) - 4;
                    break;
                case 4:
                    mitm.sub_type[bp] = WPN_GLAIVE;
                    mitm.special[bp] = NWPN_GLAIVE_OF_PRUNE;
                    mitm.pluses[bp] += 0;
                    mitm.pluses2[bp] += 12;
                    break;
                case 5:
                    mitm.sub_type[bp] = WPN_MACE;
                    mitm.special[bp] = NWPN_SCEPTRE_OF_TORMENT;
                    mitm.pluses[bp] += 7;
                    mitm.pluses2[bp] += 6;
                    break;
                case 6:
                    mitm.sub_type[bp] = WPN_LONG_SWORD;
                    mitm.special[bp] = NWPN_SWORD_OF_ZONGULDROK;
                    mitm.pluses[bp] += 9;
                    mitm.pluses2[bp] += 9;
                    break;
                case 7:
                    if (coinflip())
                    {
                        mitm.sub_type[bp] = WPN_GREAT_SWORD;
                        mitm.special[bp] = NWPN_SWORD_OF_POWER;
                        mitm.pluses[bp] += 0;
                        mitm.pluses2[bp] += 0;
                    }
                    else
                    {
                        // must avoid setting flag if not created.
                        goto out_of_uniques;
                    }
                    break;
                case 8:
                    mitm.sub_type[bp] = WPN_DAGGER;
                    mitm.special[bp] = NWPN_KNIFE_OF_ACCURACY;
                    mitm.pluses[bp] += 27;
                    mitm.pluses2[bp] -= 1;
                    break;
                case 9:
                    mitm.sub_type[bp] = WPN_QUARTERSTAFF;
                    mitm.special[bp] = NWPN_STAFF_OF_OLGREB;
                    break;
                case 10:
                    mitm.sub_type[bp] = WPN_DAGGER;
                    mitm.special[bp] = NWPN_VAMPIRES_TOOTH;
                    mitm.pluses[bp] += 3;
                    mitm.pluses2[bp] += 4;
                    break;
                case 11:
                    mitm.sub_type[bp] = WPN_QUARTERSTAFF;
                    mitm.special[bp] = NWPN_STAFF_OF_WUCAD_MU;
                    mitm.pluses[bp] += 0;
                    mitm.pluses2[bp] += 0;
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
            mitm.sub_type[bp] = force_type;

        if (many_many == 351 && (mitm.sub_type[bp] == WPN_CLUB
                                        || mitm.sub_type[bp] == WPN_SLING))
        {
            mitm.sub_type[bp] = WPN_LONG_SWORD;
        }

        quant = 1;

        mitm.pluses[bp] = 50;
        mitm.pluses2[bp] = 50;
        mitm.special[bp] = 0;

        if (force_spec == 250 && coinflip())
        {
            // XXX: The "* 30"s here are because of an ugly hack that
            // should be removed -- bwr
            switch (mitm.sub_type[bp])
            {
            case WPN_CLUB:
                if (coinflip())
                    mitm.special[bp] = DWPN_ORCISH * 30;
                break;

            case WPN_MACE:
            case WPN_FLAIL:
            case WPN_SPIKED_FLAIL:
            case WPN_GREAT_MACE:
            case WPN_GREAT_FLAIL:
                if (one_chance_in(3))
                    mitm.special[bp] = DWPN_ORCISH * 30;
                if (one_chance_in(4))
                    mitm.special[bp] = DWPN_DWARVEN * 30;
                if (one_chance_in(6))
                    mitm.special[bp] = DWPN_ELVEN * 30;
                break;

            case WPN_MORNINGSTAR:
            case WPN_HAMMER:
                if (one_chance_in(3))
                    mitm.special[bp] = DWPN_ORCISH * 30;
                if (one_chance_in(3))
                    mitm.special[bp] = DWPN_DWARVEN * 30;
                break;

            case WPN_DAGGER:
                if (one_chance_in(3))
                    mitm.special[bp] = DWPN_ORCISH * 30;
                if (one_chance_in(4))
                    mitm.special[bp] = DWPN_DWARVEN * 30;
                if (one_chance_in(4))
                    mitm.special[bp] = DWPN_ELVEN * 30;
                break;

            case WPN_SHORT_SWORD:
                if (one_chance_in(3))
                    mitm.special[bp] = DWPN_ORCISH * 30;
                if (one_chance_in(3))
                    mitm.special[bp] = DWPN_DWARVEN * 30;
                if (one_chance_in(3))
                    mitm.special[bp] = DWPN_ELVEN * 30;
                break;

            case WPN_LONG_SWORD:
                if (one_chance_in(4))
                    mitm.special[bp] = DWPN_ORCISH * 30;
                if (coinflip())
                    mitm.special[bp] = DWPN_ELVEN * 30;
                break;

            case WPN_GREAT_SWORD:
                if (one_chance_in(3))
                    mitm.special[bp] = DWPN_ORCISH * 30;
                break;

            case WPN_SCIMITAR:
                if (coinflip())
                    mitm.special[bp] = DWPN_ORCISH * 30;
                break;

            case WPN_AXE:
            case WPN_HAND_AXE:
            case WPN_BROAD_AXE:
            case WPN_BATTLEAXE:
                if (one_chance_in(3))
                    mitm.special[bp] = DWPN_ORCISH * 30;
                if (coinflip())
                    mitm.special[bp] = DWPN_DWARVEN * 30;
                break;

            case WPN_SPEAR:
                if (one_chance_in(4))
                    mitm.special[bp] = DWPN_ORCISH * 30;
                if (one_chance_in(4))
                    mitm.special[bp] = DWPN_ELVEN * 30;
                break;

            case WPN_HALBERD:
            case WPN_GLAIVE:
            case WPN_TRIDENT:
            case WPN_EXECUTIONERS_AXE:
                if (one_chance_in(5))
                    mitm.special[bp] = DWPN_ORCISH * 30;
                break;

            case WPN_QUICK_BLADE:
                if (one_chance_in(4))
                    mitm.special[bp] = DWPN_ELVEN * 30;
                break;

            case WPN_KATANA:
            case WPN_KNIFE:
                mitm.special[bp] = 0;
                break;

                // 13 - sling

            case WPN_BOW:
                if (one_chance_in(6))
                    mitm.special[bp] = DWPN_ORCISH * 30;
                if (coinflip())
                    mitm.special[bp] = DWPN_ELVEN * 30;
                break;

            case WPN_CROSSBOW:
                if (one_chance_in(4))
                    mitm.special[bp] = DWPN_ORCISH * 30;
                if (one_chance_in(4))
                    mitm.special[bp] = DWPN_DWARVEN * 30;
                break;

            case WPN_HAND_CROSSBOW:
                if (one_chance_in(3))
                    mitm.special[bp] = DWPN_ELVEN * 30;
                break;
            }
        }

        // fine, but out-of-order relative to mitm.special[] ordering {dlb}
        switch (force_spec)
        {
        case 1:         // elf
            mitm.special[bp] = DWPN_ELVEN * 30;
            break;

        case 2:         // dwarf
            mitm.special[bp] = DWPN_DWARVEN * 30;
            break;

        case 3:         // orc
            mitm.special[bp] = DWPN_ORCISH * 30;
            break;
        }

        switch (mitm.special[bp])
        {
        case DWPN_ORCISH * 30:
            if (coinflip())
                mitm.pluses[bp]--;
            if (coinflip())
                mitm.pluses2[bp]++;
            break;

        case DWPN_ELVEN * 30:
            if (coinflip())
                mitm.pluses[bp]++;
            break;

        case DWPN_DWARVEN * 30:
            if (coinflip())
                mitm.pluses[bp]++;
            if (coinflip())
                mitm.pluses2[bp]++;
            break;
        }

        if ((random2(200) <= 50 + many_many
                || many_many == 351
                || is_demonic(mitm.sub_type[bp]))
            // nobody would bother enchanting a club
            && mitm.sub_type[bp] != WPN_CLUB
            && mitm.sub_type[bp] != WPN_GIANT_CLUB
            && mitm.sub_type[bp] != WPN_GIANT_SPIKED_CLUB)
        {
            // this is dangerous -- not change below {dlb}
            if (many_many == 351)
                many_many = 200;

            // odd-looking, but this is how the algorithm compacts {dlb}:
            for (loopy = 0; loopy < 4; loopy++)
            {
                mitm.pluses[bp] += random2(3);

                if (!(random2(350) <= 20 + many_many))
                    break;
            }

            // odd-looking, but this is how the algorithm compacts {dlb}:
            for (loopy = 0; loopy < 4; loopy++)
            {
                mitm.pluses2[bp] += random2(3);

                if (!(random2(500) <= 50 + many_many))
                    break;
            }

            if (random2(300) <= 100 + many_many
                || (many_many == 200 && coinflip()) // was "== 351", see above
                || is_demonic(mitm.sub_type[bp]))
            {
                // note: even this doesn't guarantee special enchantment
                switch (mitm.sub_type[bp])
                {
                case WPN_EVENINGSTAR:
                    if (coinflip())
                        set_weapon_special(bp, SPWPN_DRAINING);
                    // **** intentional fall through here ****
                case WPN_MORNINGSTAR:
                    if (one_chance_in(4))
                        set_weapon_special(bp, SPWPN_VENOM);

                    if (one_chance_in(4))
                    {
                        set_weapon_special(bp, (coinflip() ? SPWPN_FLAMING
                                                           : SPWPN_FREEZING));
                    }

                    if (one_chance_in(20))
                        set_weapon_special(bp, SPWPN_VAMPIRICISM);
                    // **** intentional fall through here ****
                case WPN_MACE:
                case WPN_GREAT_MACE:
                    if ((mitm.sub_type[bp] == WPN_MACE
                            || mitm.sub_type[bp] == WPN_GREAT_MACE)
                        && one_chance_in(4))
                    {
                        set_weapon_special(bp, SPWPN_DISRUPTION);
                    }
                    // **** intentional fall through here ****
                case WPN_FLAIL:
                case WPN_SPIKED_FLAIL:
                case WPN_GREAT_FLAIL:
                case WPN_HAMMER:
                    if (one_chance_in(15))
                        set_weapon_special(bp, SPWPN_DISTORTION);

                    if (one_chance_in(25))
                        set_weapon_special(bp, SPWPN_PAIN);

                    if (one_chance_in(3) &&
                        (!is_weapon_special(bp) || one_chance_in(5)))
                        set_weapon_special(bp, SPWPN_VORPAL);

                    if (one_chance_in(4))
                        set_weapon_special(bp, SPWPN_HOLY_WRATH);

                    if (one_chance_in(3))
                        set_weapon_special(bp, SPWPN_PROTECTION);

                    if (one_chance_in(10))
                        set_weapon_special(bp, SPWPN_DRAINING);
                    break;


                case WPN_DAGGER:
                    if (one_chance_in(3))
                        set_weapon_special(bp, SPWPN_VENOM);

                    if (one_chance_in(10))
                        set_weapon_special(bp, SPWPN_VAMPIRICISM);
                    // **** intentional fall through here ****
                case WPN_SHORT_SWORD:
                    if (one_chance_in(8))
                        set_weapon_special(bp, SPWPN_VENOM);
                    // **** intentional fall through here ****
                case WPN_SCIMITAR:
                    if (one_chance_in(7))
                        set_weapon_special(bp, SPWPN_SPEED);
                    // **** intentional fall through here ****
                case WPN_LONG_SWORD:
                    if (one_chance_in(12))
                        set_weapon_special(bp, SPWPN_VENOM);
                    // **** intentional fall through here ****
                case WPN_GREAT_SWORD:
                case WPN_DOUBLE_SWORD:
                case WPN_TRIPLE_SWORD:
                    if (one_chance_in(10))
                        set_weapon_special(bp, SPWPN_VAMPIRICISM);

                    if (one_chance_in(15))
                        set_weapon_special(bp, SPWPN_DISTORTION);

                    if (one_chance_in(25))
                        set_weapon_special(bp, SPWPN_PAIN);

                    if (one_chance_in(5))
                    {
                        set_weapon_special(bp, (coinflip() ? SPWPN_FLAMING
                                                           : SPWPN_FREEZING));
                    }

                    if (one_chance_in(7))
                        set_weapon_special(bp, SPWPN_ELECTROCUTION);

                    if (one_chance_in(7))
                        set_weapon_special(bp, SPWPN_PROTECTION);

                    if (one_chance_in(8))
                        set_weapon_special(bp, SPWPN_ORC_SLAYING);

                    if (one_chance_in(12))
                        set_weapon_special(bp, SPWPN_DRAINING);

                    if (one_chance_in(4))
                        set_weapon_special(bp, SPWPN_HOLY_WRATH);

                    if (one_chance_in(4)
                            && (!is_weapon_special(bp) || one_chance_in(3)))
                    {
                        set_weapon_special(bp, SPWPN_VORPAL);
                    }

                    break;


                case WPN_AXE:
                case WPN_BROAD_AXE:
                case WPN_BATTLEAXE:
                case WPN_EXECUTIONERS_AXE:
                    if (one_chance_in(25))
                        set_weapon_special(bp, SPWPN_HOLY_WRATH);

                    if (one_chance_in(14))
                        set_weapon_special(bp, SPWPN_DRAINING);
                    // **** intentional fall through here ****
                case WPN_HAND_AXE:
                    if (one_chance_in(10))
                        set_weapon_special(bp, SPWPN_VAMPIRICISM);

                    if (one_chance_in(15))
                        set_weapon_special(bp, SPWPN_DISTORTION);

                    if (one_chance_in(25))
                        set_weapon_special(bp, SPWPN_PAIN);

                    if (one_chance_in(3)
                            && (!is_weapon_special(bp) || one_chance_in(5)))
                    {
                        set_weapon_special(bp, SPWPN_VORPAL);
                    }

                    if (one_chance_in(6))
                        set_weapon_special(bp, SPWPN_ORC_SLAYING);

                    if (one_chance_in(4))
                    {
                        set_weapon_special(bp, (coinflip() ? SPWPN_FLAMING
                                                           : SPWPN_FREEZING));
                    }

                    if (one_chance_in(8))
                        set_weapon_special(bp, SPWPN_ELECTROCUTION);

                    if (one_chance_in(12))
                        set_weapon_special(bp, SPWPN_VENOM);

                    break;

                case WPN_WHIP:
                    if (one_chance_in(20))
                        set_weapon_special(bp, SPWPN_DISTORTION);

                    if (one_chance_in(6))
                    {
                        set_weapon_special(bp, (coinflip() ? SPWPN_FLAMING
                                                           : SPWPN_FREEZING));
                    }

                    if (one_chance_in(6))
                        set_weapon_special(bp, SPWPN_VENOM);

                    if (coinflip())
                        set_weapon_special(bp, SPWPN_REACHING);

                    if (one_chance_in(5))
                        set_weapon_special(bp, SPWPN_SPEED);

                    if (one_chance_in(5))
                        set_weapon_special(bp, SPWPN_ELECTROCUTION);
                    break;

                case WPN_HALBERD:
                case WPN_GLAIVE:
                    if (one_chance_in(30))
                        set_weapon_special(bp, SPWPN_HOLY_WRATH);

                    if (one_chance_in(4))
                        set_weapon_special(bp, SPWPN_PROTECTION);
                    // **** intentional fall through here ****
                case WPN_SCYTHE:
                case WPN_TRIDENT:
                    if (one_chance_in(5))
                        set_weapon_special(bp, SPWPN_SPEED);
                    // **** intentional fall through here ****
                case WPN_SPEAR:
                    if (one_chance_in(10))
                        set_weapon_special(bp, SPWPN_VAMPIRICISM);

                    if (one_chance_in(20))
                        set_weapon_special(bp, SPWPN_DISTORTION);

                    if (one_chance_in(25))
                        set_weapon_special(bp, SPWPN_PAIN);

                    if (one_chance_in(5) &&
                        (!is_weapon_special(bp) || one_chance_in(6)))
                        set_weapon_special(bp, SPWPN_VORPAL);

                    if (one_chance_in(6))
                        set_weapon_special(bp, SPWPN_ORC_SLAYING);

                    if (one_chance_in(6))
                    {
                        set_weapon_special(bp, (coinflip() ? SPWPN_FLAMING
                                                           : SPWPN_FREEZING));
                    }

                    if (one_chance_in(6))
                        set_weapon_special(bp, SPWPN_VENOM);

                    if (one_chance_in(3))
                        set_weapon_special(bp, SPWPN_REACHING);
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
                        set_weapon_special(bp, (coinflip() ? SPWPN_FLAME
                                                           : SPWPN_FROST));
                    }
                    break;

                // quarterstaff - not powerful, as this would make
                // the 'staves' skill just too good
                case WPN_QUARTERSTAFF:
                    if (one_chance_in(15))
                        set_weapon_special(bp, SPWPN_DISTORTION);

                    if (one_chance_in(25))
                        set_weapon_special(bp, SPWPN_PAIN);

                    if (one_chance_in(5))
                        set_weapon_special(bp, SPWPN_PROTECTION);

                    if (one_chance_in(5))
                        set_weapon_special(bp, SPWPN_SPEED);
                    break;


                case WPN_DEMON_TRIDENT:
                case WPN_DEMON_WHIP:
                case WPN_DEMON_BLADE:
                    if (mitm.special[bp] >= 90)
                        mitm.special[bp] = mitm.special[bp] % 30;

                    if (one_chance_in(3)
                        && (mitm.sub_type[bp] == WPN_DEMON_WHIP
                            || mitm.sub_type[bp] == WPN_DEMON_TRIDENT))
                    {
                        set_weapon_special(bp, SPWPN_REACHING);
                    }

                    if (one_chance_in(5))
                        set_weapon_special(bp, SPWPN_DRAINING);

                    if (one_chance_in(5))
                        set_weapon_special(bp, SPWPN_FLAMING);

                    if (one_chance_in(5))
                        set_weapon_special(bp, SPWPN_FREEZING);

                    if (one_chance_in(5))
                        set_weapon_special(bp, SPWPN_ELECTROCUTION);

                    if (one_chance_in(5))
                        set_weapon_special(bp, SPWPN_VAMPIRICISM);

                    if (one_chance_in(5))
                        set_weapon_special(bp, SPWPN_PAIN);

                    if (one_chance_in(5))
                        set_weapon_special(bp, SPWPN_VENOM);
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
                mitm.pluses[bp] = 150;
                mitm.pluses[bp] -= random2(4);
                mitm.pluses2[bp] = 50;
                mitm.pluses2[bp] -= random2(4);

                // clear specials {dlb}
                mitm.special[bp] = (mitm.special[bp] / 30) * 30;
            }
        }

        // value was "0" comment said "orc" so I went with comment {dlb}
        if (mitm.special[bp] / 30 == DWPN_ORCISH)
        {
            if (mitm.special[bp] % 30 != SPWPN_NORMAL)
            {
                if (mitm.special[bp] % 30 == SPWPN_HOLY_WRATH
                    || mitm.special[bp] % 30 == SPWPN_ORC_SLAYING
                    || coinflip())
                {
                    // this makes no sense {dlb}
                    if (mitm.special[bp] % 30 == SPWPN_ELECTROCUTION)
                        mitm.pluses[bp] = 50;

                    // no holy wrath or slay orc and 1/2 the time no-ego
                    // that is, set it to plain' ol orcishness ... {dlb}
                    mitm.special[bp] = 90;
                }
            }
        }


        if ((mitm.special[bp] != 0
                || (mitm.pluses[bp] != 50 && one_chance_in(3)))
            && mitm.sub_type[bp] != WPN_CLUB
            && mitm.special[bp] / 30 == DWPN_PLAIN)
        {
            // that is, runed or glowing {dlb}
            mitm.special[bp] += (coinflip() ? DWPN_GLOWING : DWPN_RUNED) * 30;
        }
        break;

    case OBJ_MISSILES:
        mitm.pluses[bp] = 0;
        mitm.sub_type[bp] = random2(4);

        if (force_type != OBJ_RANDOM)
            mitm.sub_type[bp] = force_type;

        // "* 30"s are important here too -- bwr
        if (force_spec != 250)
        {
            switch (force_spec)
            {
            case 3:             // orc
                mitm.special[bp] = DAMMO_ORCISH * 30;
                break;
            case 1:             // elf
                mitm.special[bp] = DAMMO_ELVEN * 30;
                break;
            case 2:             // dw
                mitm.special[bp] = DAMMO_DWARVEN * 30;
                break;
            }
        }
        else
        {
            if ((mitm.sub_type[bp] == MI_ARROW
                    || mitm.sub_type[bp] == MI_DART)
                && one_chance_in(4))
            {
                // elven - not for bolts, though
                mitm.special[bp] = DAMMO_ELVEN * 30;
            }

            if ((mitm.sub_type[bp] == MI_ARROW
                    || mitm.sub_type[bp] == MI_BOLT
                    || mitm.sub_type[bp] == MI_DART)
                && one_chance_in(4))
            {
                mitm.special[bp] = DAMMO_ORCISH * 30;
            }

            if ((mitm.sub_type[bp] == MI_DART
                    || mitm.sub_type[bp] == MI_BOLT)
                && one_chance_in(6))
            {
                mitm.special[bp] = DAMMO_DWARVEN * 30;
            }
        }

        if ((mitm.sub_type[bp] == MI_ARROW
                || mitm.sub_type[bp] == MI_BOLT
                || mitm.sub_type[bp] == MI_DART)
            && one_chance_in(7))
        {
            mitm.special[bp] = mitm.special[bp] % 30 + 1;
        }

        if ((mitm.sub_type[bp] == MI_ARROW
                || mitm.sub_type[bp] == MI_BOLT
                || mitm.sub_type[bp] == MI_DART)
            && one_chance_in(7))
        {
            mitm.special[bp] = mitm.special[bp] % 30 + 2;
        }

        if ((mitm.special[bp] == 90 || one_chance_in(5) && one_chance_in(3))
            && (mitm.sub_type[bp] == MI_ARROW
                || mitm.sub_type[bp] == MI_BOLT
                || mitm.sub_type[bp] == MI_DART))
        {
            mitm.special[bp] = mitm.special[bp] % 30 + 3;
        }

        quant = 1 + random2(9) + random2(12) + random2(15) + random2(12);

        if (mitm.sub_type[bp] == MI_LARGE_ROCK)
            quant = 1 + random2avg(5, 2);

        if (10 + many_many >= random2(100))
            mitm.pluses[bp] = random2(5);

        mitm.pluses[bp] += 50;
        break;

    case OBJ_ARMOUR:
        quant = 1;

        mitm.sub_type[bp] = random2(3);
        mitm.pluses[bp] = 0;
        mitm.pluses2[bp] = 0;

        if (random2(35) <= many_many + 10)
        {
            mitm.sub_type[bp] = random2(5);
            if (one_chance_in(4))
                mitm.sub_type[bp] = ARM_ANIMAL_SKIN;

        }

        if (random2(60) <= many_many + 10)
            mitm.sub_type[bp] = random2(8);

        if (10 + many_many >= random2(400) && one_chance_in(10))
            mitm.sub_type[bp] = ARM_DRAGON_HIDE + random2(7);

        if (10 + many_many >= random2(500) && one_chance_in(20))
        {
            mitm.sub_type[bp] = ARM_STEAM_DRAGON_HIDE + random2(11);

            if (mitm.sub_type[bp] == ARM_ANIMAL_SKIN)
                mitm.sub_type[bp] = ARM_CRYSTAL_PLATE_MAIL;
        }

        if (one_chance_in(4))
            mitm.sub_type[bp] = ARM_SHIELD + random2(7);

        if (force_type != OBJ_RANDOM)
            mitm.sub_type[bp] = force_type;

        if (allow_uniques == 1
            && many_many > 2
            && random2(2000) <= (100 + many_many * 3)
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
                    make_item_unrandart(icky, bp);
                    break;
                }
            }

            hide2armour(&(mitm.sub_type[bp]));

            mitm.special[bp] = 26 + random2(4);
            mitm.pluses[bp] = 50;
            mitm.pluses2[bp] = random2(150);

            if (mitm.sub_type[bp] == ARM_BOOTS)
            {
                mitm.pluses2[bp] = 0;
                if (one_chance_in(6))
                    mitm.pluses2[bp] += random2(3);
            }

            if (mitm.sub_type[bp] == ARM_HELMET)
            {
                mitm.pluses2[bp] = 0;
                if (one_chance_in(3))
                    mitm.pluses2[bp] += random2(4);
            }

            mitm.pluses[bp] += random2(4);

            if (one_chance_in(5))
                mitm.pluses[bp] += random2(4);

            if (one_chance_in(6))
                mitm.pluses[bp] -= random2(8);

            quant = 1;

            if (one_chance_in(5))
            {
                mitm.pluses[bp] = 150 - random2(6);
            }
            else if (mitm.pluses[bp] < 50 && !one_chance_in(3))
                mitm.pluses[bp] += 100; /* cursed! nasty */
            break;
        }

        mitm.pluses[bp] = 0;

        if (mitm.sub_type[bp] == ARM_HELMET && coinflip())
            mitm.pluses2[bp] = random2(4);

        // 180 - orc, 150 - dwar, 120 - elf
        // XXX: "* 30" important!  please fix -- bwr
        if (force_spec == 250 && coinflip())
        {
            switch (mitm.sub_type[bp])
            {
            case ARM_SHIELD:    // shield - must do special things for this!
            case ARM_BUCKLER:
                if (one_chance_in(4))
                    mitm.special[bp] = DARM_ELVEN * 30;
                // ****** deliberate fall-through here? ******
            case ARM_LARGE_SHIELD:
                if (one_chance_in(3))
                    mitm.special[bp] = DARM_DWARVEN * 30;
                break;

            case ARM_CLOAK:
                if (one_chance_in(4))
                    mitm.special[bp] = DARM_ORCISH * 30;
                if (one_chance_in(4))
                    mitm.special[bp] = DARM_DWARVEN * 30;
                if (one_chance_in(4))
                    mitm.special[bp] = DARM_ELVEN * 30;
                break;

            // no helmet, for various reasons
            case ARM_GLOVES:
                if (one_chance_in(4))
                    mitm.special[bp] = DARM_ELVEN * 30;
                break;

            case ARM_BOOTS:
                if (one_chance_in(4))
                {
                    mitm.pluses2[bp] = 1;
                    break;      /* naga barding */
                }
                if (one_chance_in(4))
                {
                    mitm.pluses2[bp] = 2;
                    break;      /* naga barding */
                }
                if (one_chance_in(4))
                    mitm.special[bp] = DARM_ORCISH * 30;
                if (one_chance_in(4))
                    mitm.special[bp] = DARM_ELVEN * 30;
                if (one_chance_in(6))
                    mitm.special[bp] = DARM_DWARVEN * 30;
                break;

            case ARM_ROBE:
                if (one_chance_in(4))
                    mitm.special[bp] = DARM_ELVEN * 30;
                break;

            default:            // other body armours:

                if (mitm.sub_type[bp] <= ARM_CHAIN_MAIL && one_chance_in(6))
                    mitm.special[bp] = DARM_ELVEN * 30;
                if (mitm.sub_type[bp] >= ARM_RING_MAIL && one_chance_in(5))
                    mitm.special[bp] = DARM_DWARVEN * 30;
                if (one_chance_in(5))
                    mitm.special[bp] = DARM_ORCISH * 30;
                break;
            }
        }

        switch (force_spec)
        {
        case 1:         // elf
            mitm.special[bp] = DARM_ELVEN * 30;
            break;

        case 2:         // dwarf
            mitm.special[bp] = DARM_DWARVEN * 30;
            if (coinflip())
                mitm.pluses[bp]++;
            break;

        case 3:         // orc
            mitm.special[bp] = DARM_ORCISH * 30;
            break;
        }


        if (50 + many_many >= random2(250)
            || (mitm.sub_type[bp] == ARM_HELMET && mitm.pluses2[bp] == 3))
        {
            mitm.pluses[bp] += random2(3);

            if (mitm.sub_type[bp] <= 7 && 20 + many_many >= random2(300))
                mitm.pluses[bp] += random2(3);

            if (30 + many_many >= random2(350)
                && (mitm.special[bp] != DARM_ORCISH * 30
                    || mitm.sub_type[bp] <= ARM_PLATE_MAIL))
            {
                switch (mitm.sub_type[bp])
                {
                case ARM_SHIELD:   // shield - must do special things for this!
                case ARM_LARGE_SHIELD:
                case ARM_BUCKLER:
                    mitm.special[bp] += SPARM_PROTECTION;
                    break;  // prot
                    //break;

                case ARM_CLOAK:
                    if (mitm.special[bp] == DARM_DWARVEN * 30)
                        break;

                    switch (random2(4))
                    {
                    case 0:
                        mitm.special[bp] += SPARM_POISON_RESISTANCE;
                        break;
                    case 1:
                        mitm.special[bp] += SPARM_DARKNESS;
                        break;
                    case 2:
                        mitm.special[bp] += SPARM_MAGIC_RESISTANCE;
                        break;
                    case 3:
                        mitm.special[bp] += SPARM_PRESERVATION;
                        break;
                    }
                    break;

                case ARM_HELMET:
                    if (mitm.pluses2[bp] == 2 && one_chance_in(4))
                        mitm.special[bp] += SPARM_DARKNESS;
                    else if (mitm.pluses2[bp] == 3 && one_chance_in(4))
                        mitm.special[bp] += SPARM_MAGIC_RESISTANCE;
                    else
                        switch (random2(3))
                        {
                        case 0:
                            mitm.special[bp] += SPARM_SEE_INVISIBLE;
                            break;
                        case 1:
                            mitm.special[bp] += SPARM_DARKNESS;
                            break;
                        case 2:
                            mitm.special[bp] += SPARM_INTELLIGENCE;
                            break;
                        }
                    break;

                case ARM_GLOVES:
                    mitm.special[bp] += (coinflip() ? SPARM_DEXTERITY
                                                    : SPARM_STRENGTH);
                    break;

                case ARM_BOOTS:
                    switch (random2(3))
                    {
                    case 0:
                        mitm.special[bp] += SPARM_RUNNING;
                        break;
                    case 1:
                        mitm.special[bp] += SPARM_LEVITATION;
                        break;
                    case 2:
                        mitm.special[bp] += SPARM_STEALTH;
                        break;
                    }
                    break;

                case ARM_ROBE:
                    switch (random2(4))
                    {
                    case 0:
                        mitm.special[bp] += (coinflip() ? SPARM_COLD_RESISTANCE
                                                        : SPARM_FIRE_RESISTANCE);
                        break;
                    case 1:
                        mitm.special[bp] += SPARM_MAGIC_RESISTANCE;
                        break;
                    case 2:
                        mitm.special[bp] += (coinflip() ? SPARM_POSITIVE_ENERGY
                                                        : SPARM_RESISTANCE);
                        break;
                    case 3:
                        if (force_type != OBJ_RANDOM
                            || mitm.special[bp] != 0
                            || random2(50) > 10 + many_many)
                        {
                            break;
                        }

                        mitm.special[bp] += SPARM_ARCHMAGI;
                        break;
                    }
                    break;

                default:    // other body armours:
                    mitm.special[bp] += (coinflip() ? 3 : 2);
                    if (one_chance_in(9))
                        mitm.special[bp] = SPARM_POSITIVE_ENERGY;
                    if (one_chance_in(5))
                        mitm.special[bp] = SPARM_MAGIC_RESISTANCE;
                    if (one_chance_in(5))
                        mitm.special[bp] = SPARM_POISON_RESISTANCE;

                    if (mitm.sub_type[bp] == ARM_PLATE_MAIL
                        && one_chance_in(15))
                    {
                        mitm.special[bp] += SPARM_PONDEROUSNESS;
                        mitm.pluses[bp] += 1 + random2(4);
                    }
                    break;
                }
            }
        }
        else if (one_chance_in(12))
        {
            mitm.pluses[bp] = (coinflip() ? 99 : 98);
            if (one_chance_in(5))
                mitm.pluses[bp] -= random2(3);
            mitm.special[bp] = 0;   // heh heh heh
        }

        if (mitm.special[bp] < 120
            && (mitm.special[bp] != 0
                || (mitm.pluses[bp] != 0 && one_chance_in(3))))
        {
            mitm.special[bp] += random2(3) * 30;
        }

        if (mitm.sub_type[bp] > ARM_LARGE_SHIELD)
            mitm.special[bp] = 0;       // sorry about that.

        mitm.pluses[bp] += 50;

        // make sure you don't get a hide from a scroll of acquirement
        if (many_many == 351)
            hide2armour( &(mitm.sub_type[bp]) ); // what of animal hides? {dlb}

        if (mitm.sub_type[bp] >= ARM_DRAGON_HIDE
            && mitm.sub_type[bp] <= ARM_ANIMAL_SKIN)
        {
            mitm.special[bp] = 0;
        }

        /* skin armours + Crystal PM don't get special enchantments
           or species, but can be randarts
         */
        break;

    case OBJ_WANDS:
        // determine sub_type:
        mitm.sub_type[bp] = ((force_type != OBJ_RANDOM) ? force_type
                                                        : random2(NUM_WANDS));

        // determine upper bound on charges:
        range_charges = ((mitm.sub_type[bp] == WAND_HEALING)           ? 10 :
                         (mitm.sub_type[bp] == WAND_FLAME
                          || mitm.sub_type[bp] == WAND_FROST
                          || mitm.sub_type[bp] == WAND_MAGIC_DARTS
                          || mitm.sub_type[bp] == WAND_RANDOM_EFFECTS) ? 28
                                                                       : 16);

        // generate charges randomly:
        mitm.pluses[bp] = random2avg(range_charges, 3);
        // set quantity to one:
        quant = 1;
        break;

    case OBJ_FOOD:              // this can be parsed out {dlb}
        // determine sub_type:
        if (force_type == OBJ_RANDOM)
        {
            temp_rand = random2(1344);

            mitm.sub_type[bp] =
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
            mitm.sub_type[bp] = force_type;

        // determine quantity:
        if (allow_uniques > 1)
            quant = allow_uniques;
        else
        {
            quant = 1;

            if (one_chance_in(80))
                quant += random2(3);

            if (mitm.sub_type[bp] == FOOD_STRAWBERRY
                || mitm.sub_type[bp] == FOOD_GRAPE
                || mitm.sub_type[bp] == FOOD_SULTANA)
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
                mitm.sub_type[bp] = POT_HEAL_WOUNDS;
            else
                mitm.sub_type[bp] = POT_HEALING;

            if (one_chance_in(20))
                mitm.sub_type[bp] = POT_CURE_MUTATION;
            break;

        case POT_MIGHT:
        case POT_GAIN_STRENGTH:
            // enhancements
            if (coinflip())
                mitm.sub_type[bp] = POT_SPEED;
            else
                mitm.sub_type[bp] = POT_MIGHT;

            if (one_chance_in(10))
                mitm.sub_type[bp] = POT_BERSERK_RAGE;

            if (one_chance_in(5))
                mitm.sub_type[bp] = POT_INVISIBILITY;

            if (one_chance_in(6))
                mitm.sub_type[bp] = POT_LEVITATION;

            if (one_chance_in(30))
                mitm.sub_type[bp] = POT_PORRIDGE;
            break;

        case POT_GAIN_DEXTERITY:
            // gain ability
            mitm.sub_type[bp] = POT_GAIN_STRENGTH + random2(3);
            if (one_chance_in(10))
                mitm.sub_type[bp] = POT_EXPERIENCE;
            if (one_chance_in(10))
                mitm.sub_type[bp] = POT_MAGIC;
            if (!one_chance_in(8))
                mitm.sub_type[bp] = POT_RESTORE_ABILITIES;
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
                if (many_many > 0)
                {
                    mitm.sub_type[bp] = POT_POISON;

                    if (many_many > 10 && one_chance_in(4))
                        mitm.sub_type[bp] = POT_STRONG_POISON;

                    break;
                }
            /* **** intentional fall through **** */
            case 1:
                mitm.sub_type[bp] = POT_SLOWING;
                break;
            case 2:
                mitm.sub_type[bp] = POT_PARALYSIS;
                break;
            case 3:
                mitm.sub_type[bp] = POT_CONFUSION;
                break;
            case 5:
                if (many_many > 6)
                {
                    mitm.sub_type[bp] = POT_MUTATION;
                    break;
                }
            }

            if (one_chance_in(8))
                mitm.sub_type[bp] = POT_DEGENERATION;

            if (one_chance_in(1000))
                mitm.sub_type[bp] = POT_DECAY;

            break;
        }

        if (force_type != OBJ_RANDOM)
            mitm.sub_type[bp] = force_type;

        mitm.pluses[bp] = 0;
        break;

    case OBJ_SCROLLS:
        // determine sub_type:
        if (force_type == OBJ_RANDOM)
        {
            // only used in certain cases {dlb}
            int depth_mod = random2(1 + many_many);

            temp_rand = random2(920);

            mitm.sub_type[bp] =
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
                         ((many_many < 4) ? SCR_TELEPORTATION
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
            mitm.sub_type[bp] = force_type;

        // determine quantity:
        temp_rand = random2(48);

        quant = ((temp_rand > 1
                  || mitm.sub_type[bp] == SCR_VORPALISE_WEAPON
                  || mitm.sub_type[bp] == SCR_ACQUIREMENT
                  || mitm.sub_type[bp] == SCR_TORMENT) ? 1 :    // 95.83%
                                       (temp_rand > 0) ? 2      //  2.08%
                                                       : 3);    //  2.08%
        mitm.pluses[bp] = 0;
        break;

    case OBJ_JEWELLERY:
        // determine whether an unrandart will be generated {dlb}:
        if (many_many > 2 && you.level_type != LEVEL_ABYSS
            && you.level_type != LEVEL_PANDEMONIUM
            && random2(2000) <= 100 + (many_many * 3) && one_chance_in(4))
        {
            icky = find_okay_unrandart(OBJ_JEWELLERY);

            if (icky != -1)
            {
                quant = 1;
                make_item_unrandart(icky, bp);
                break;
            }
        }

        // otherwise, determine jewellery type {dlb}:
        if (force_type == OBJ_RANDOM)
        {
            mitm.sub_type[bp] = (!one_chance_in(4) ? random2(24)   // rings
                                                   : AMU_RAGE + random2(10));
        }
        else
            mitm.sub_type[bp] = force_type;

        // quantity is always one {dlb}:
        quant = 1;

        // everything begins as uncursed, unenchanted jewellery {dlb}:
        mitm.pluses[bp] = 50;

        // set pluses for rings that require them {dlb}:
        switch (mitm.sub_type[bp])
        {
        case RING_PROTECTION:
        case RING_STRENGTH:
        case RING_SLAYING:
        case RING_EVASION:
        case RING_DEXTERITY:
        case RING_INTELLIGENCE:
            if (one_chance_in(5))       // 20% of such rings are cursed {dlb}
            {
                mitm.pluses[bp] = 150;
                mitm.pluses[bp] -= (coinflip() ? 2 : 1);

                if (one_chance_in(3))
                    mitm.pluses[bp] -= random2(4);
            }
            else
            {
                mitm.pluses[bp] += 1 + (one_chance_in(3) ? random2(3)
                                                         : random2avg(6, 2));
            }
            break;

        default:
            break;
        }

        // rings of slaying also require that pluses2 be set {dlb}:
        if (mitm.sub_type[bp] == RING_SLAYING)
        {
            mitm.pluses2[bp] = 50;
            mitm.pluses2[bp] += 1 + (one_chance_in(3) ? random2(3)
                                                      : random2avg(6, 2));

            // should this happen to cursed rings? {dlb}
            if (random2(25) < 9)        // 36% of such rings {dlb}
            {
                mitm.pluses[bp] = 50;
                mitm.pluses2[bp] += 2;
            }
        }

        // confusing generation of seeming randarts
        // -- how does this fit in? {dlb}
        if (allow_uniques == 1 && many_many > 2
            && random2(2000) <= 100 + (many_many * 3) && coinflip())
        {
            if (mitm.sub_type[bp] == RING_PROTECTION)
                mitm.sub_type[bp] = RING_PROTECTION_FROM_FIRE + random2(3);

            if (mitm.sub_type[bp] == RING_STRENGTH
                || mitm.sub_type[bp] == RING_SLAYING)
            {
                mitm.sub_type[bp] = RING_SEE_INVISIBLE + random2(4);
            }

            if (mitm.sub_type[bp] == RING_EVASION)
                mitm.sub_type[bp] = RING_SUSTAIN_ABILITIES;

            if (mitm.sub_type[bp] == RING_DEXTERITY
                || mitm.sub_type[bp] == RING_INTELLIGENCE)
            {
                mitm.sub_type[bp] = RING_WIZARDRY + random2(8);
            }

            // hmmm ... maybe this should work off of pluses? {dlb}
            if (mitm.sub_type[bp] == AMU_INACCURACY)
                mitm.sub_type[bp] = AMU_RAGE + random2(8);

            /* Can't allow base ring types with +s */
            mitm.special[bp] = 200;
            mitm.pluses[bp] = random2(70);
            mitm.pluses2[bp] = random2(150);
            break;
        }

        // rings of hunger and teleportation are always cursed {dlb}:
        if (mitm.sub_type[bp] == RING_HUNGER
            || mitm.sub_type[bp] == RING_TELEPORTATION)
        {
            mitm.pluses[bp] = 150;
        }
        break;

    case OBJ_BOOKS:
      create_book:
        do
        {
            mitm.sub_type[bp] = random2(NUM_BOOKS);

            if (book_rarity(mitm.sub_type[bp]) == 100)
                continue;

            if (mitm.sub_type[bp] != BOOK_DESTRUCTION
                && mitm.sub_type[bp] != BOOK_MANUAL)
            {
                if (one_chance_in(10))
                {
                    if (coinflip())
                        mitm.sub_type[bp] = BOOK_WIZARDRY;
                    else
                        mitm.sub_type[bp] = BOOK_POWER;
                }

                if (random2(many_many + 1) + 1 >= book_rarity(mitm.sub_type[bp])
                    || one_chance_in(100))
                {
                    break;
                }
                else
                {
                    mitm.sub_type[bp] = BOOK_DESTRUCTION;
                    continue;
                }
            }
        }
        while (mitm.sub_type[bp] == BOOK_DESTRUCTION
                   || mitm.sub_type[bp] == BOOK_MANUAL);

        if (book_rarity(mitm.sub_type[bp]) == 100)
            goto create_book;

// now why isn't the same sequence #ifdef'd out for staves??? {dlb}
#if 0
        mitm.pluses[bp] = 127;
        itoa(127, strungy, 2);

        if (force_type != OBJ_RANDOM)
            mitm.sub_type[bp] = force_type;

        // shouldn't let spell no 1 be erased

        spellbook_template(mitm.sub_type[bp], fpass);

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

        mitm.pluses[bp] = numbo + 64;
#endif

        mitm.special[bp] = random2(5);

        if (one_chance_in(10))
            mitm.special[bp] += random2(8) * 10;

        if (force_type != OBJ_RANDOM)           //jmf: ADD THESE
            mitm.sub_type[bp] = force_type;     //     TWO LINES!

        quant = 1;

        // tome of destruction : rare!
        if (force_type == BOOK_DESTRUCTION
            || (random2(7000) <= many_many + 20 && many_many > 10
                && force_type == OBJ_RANDOM))
        {
            mitm.sub_type[bp] = BOOK_DESTRUCTION;
        }

        // skill manuals - also rare
        // fixed to generate manuals for *all* extant skills - 14mar2000 {dlb}
        if (force_type == BOOK_MANUAL
            || (random2(4000) <= many_many + 20 && many_many > 6
                && force_type == OBJ_RANDOM))
        {
            mitm.sub_type[bp] = BOOK_MANUAL;
            mitm.pluses[bp] = (one_chance_in(4) ? random2(14) + 25
                                                : random2(19));

            if (mitm.pluses[bp] == SK_UNUSED_1)
                mitm.pluses[bp] = SK_UNARMED_COMBAT;
        }
        break;

    case OBJ_STAVES:            // this can be parsed, too {dlb}
        mitm.sub_type[bp] = random2(10);

        // spell staves {dlb}
        if (one_chance_in(5))
            mitm.sub_type[bp] = STAFF_SMITING + random2(9);
        // remaining non-spell staves {dlb}
        if (one_chance_in(15))
            mitm.sub_type[bp] = STAFF_AIR + random2(3);

        if (mitm.sub_type[bp] >= STAFF_DESTRUCTION_I
            && mitm.sub_type[bp] <= STAFF_DESTRUCTION_IV && one_chance_in(3))
        {
            mitm.sub_type[bp] = random2(6);
        }

        if (force_type != OBJ_RANDOM)
            mitm.sub_type[bp] = force_type;

        mitm.special[bp] = random2(9);

        // do we need all this special stuff for staves if it was removed
        // for spellbooks? {dlb}
        // Hopefully, not... that was an ugly hack I removed -- bwr
#if 0
        if (mitm.sub_type[bp] >= STAFF_SMITING   // that is, spell staves {dlb}
                && mitm.sub_type[bp] <= STAFF_DEMONOLOGY)
        {
            mitm.pluses[bp] = 127;

            if (force_type != OBJ_RANDOM)
                mitm.sub_type[bp] = force_type;

            itoa(127, strungy, 2);

            if (force_type != OBJ_RANDOM)
                mitm.sub_type[bp] = force_type;

            spellbook_template(mitm.sub_type[bp] + 40, fpass);

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

            mitm.pluses[bp] = numbo + 64;
        }
#endif
        // XXX: Is this the only magic we need from above? -- bwr
        if (force_type != OBJ_RANDOM)
            mitm.sub_type[bp] = force_type;

        quant = 1;
        break;

    case OBJ_ORBS:              // always forced in current setup {dlb}
        quant = 1;

        if (force_type != OBJ_RANDOM)
            mitm.sub_type[bp] = force_type;

        // I think we only have one type of orb now, so ... {dlb}
        if (mitm.sub_type[bp] >= 4 && mitm.sub_type[bp] <= 19)
            you.unique_items[mitm.sub_type[bp] + 3] = 1;
        break;

    // I think these must always be forced, too ... {dlb}
    case OBJ_MISCELLANY:
        if (force_type == OBJ_RANDOM)
        {
            mitm.sub_type[bp] = random2(6);

            if (one_chance_in(6))
                mitm.sub_type[bp] = MISC_BOX_OF_BEASTS + random2(10);

            if (mitm.sub_type[bp] == MISC_RUNE_OF_ZOT)
                mitm.sub_type[bp] = MISC_CRYSTAL_BALL_OF_FIXATION;

            if (mitm.sub_type[bp] == MISC_DECK_OF_POWER)
            {
                if (one_chance_in(4))
                    mitm.sub_type[bp] = MISC_DECK_OF_POWER;
                else
                    mitm.sub_type[bp] = MISC_DECK_OF_TRICKS;
            }

            if (one_chance_in(20))
                mitm.sub_type[bp] = MISC_DECK_OF_SUMMONINGS;
        }
        else
        {
            mitm.sub_type[bp] = force_type;
        }

        if (mitm.sub_type[bp] == MISC_DECK_OF_WONDERS
            || mitm.sub_type[bp] == MISC_DECK_OF_SUMMONINGS
            || mitm.sub_type[bp] == MISC_DECK_OF_POWER)
        {
            mitm.pluses[bp] = 4 + random2(10);
        }

        if (mitm.sub_type[bp] == MISC_DECK_OF_TRICKS)
            mitm.pluses[bp] = 6 + random2avg(15, 2);

        if (mitm.sub_type[bp] == MISC_RUNE_OF_ZOT)
        {
            mitm.pluses[bp] =
                   ((you.level_type == LEVEL_PANDEMONIUM) ? 50 :
                    (you.level_type == LEVEL_ABYSS)       ? 51
                                                          : you.where_are_you);
        }

        quant = 1;
        break;

    // that is, everything turns to gold if not enumerated above, so ... {dlb}
    default:
        mitm.base_type[bp] = OBJ_GOLD;
        quant = 1 + random2avg(19, 2) + random2(many_many);
        break;
    }

    mitm.quantity[bp] = quant;

    if (force_place == 1)
    {
        mitm.x[bp] = 1;
        mitm.y[bp] = 1;
    }
    else
    {
        do
        {
            mitm.x[bp] = random2(GXM);
            mitm.y[bp] = random2(GYM);
        }
        while (grd[mitm.x[bp]][mitm.y[bp]] != DNGN_FLOOR);
    }

    item_colour(bp);

    return bp;
}                               // end items()

/*
 **************************************************
 *                                                *
 *              END PUBLIC FUNCTIONS              *
 *                                                *
 **************************************************
*/

static void make_room(int max_doors, int doorlevel)
{
    int find_door = 0;
    int diag_door = 0;

    for (bcount_x = room_x1; bcount_x < room_x2; bcount_x++)
    {
        if ((grd[bcount_x][room_y1 - 1] > 16
                || grd[bcount_x][room_y1 - 1] == DNGN_CLOSED_DOOR)
            && grd[bcount_x][room_y1 - 1] < DNGN_EXIT_PANDEMONIUM)
            // value was "100" -- does this make any sense to anyone? {dlb}
        {
            find_door++;
        }

        if ((grd[bcount_x][room_y2] > 16
                || grd[bcount_x][room_y2] == DNGN_CLOSED_DOOR)
            && grd[bcount_x][room_y2] < DNGN_EXIT_PANDEMONIUM)
        {
            find_door++;
        }
    }

    for (bcount_y = room_y1; bcount_y < room_y2; bcount_y++)
    {
        if ((grd[room_x1 - 1][bcount_y] > 16
                || grd[room_x1 - 1][bcount_y] == DNGN_CLOSED_DOOR)
            && grd[room_x1 - 1][bcount_y] < DNGN_EXIT_PANDEMONIUM)
        {
            find_door++;
        }

        if ((grd[room_x2][bcount_y] > 16
                || grd[room_x2][bcount_y] == DNGN_CLOSED_DOOR)
            && grd[room_x2][bcount_y] < DNGN_EXIT_PANDEMONIUM)
        {
            find_door++;
        }
    }

    if (grd[room_x1 - 1][room_y1 - 1] > 16
        || grd[room_x1 - 1][room_y1 - 1] == DNGN_CLOSED_DOOR)
    {
        diag_door++;
    }

    if (grd[room_x1 - 1][room_y2] > 16
        || grd[room_x1 - 1][room_y2] == DNGN_CLOSED_DOOR)
    {
        diag_door++;
    }

    if (grd[room_x2][room_y1 - 1] > 16
        || grd[room_x2][room_y1 - 1] == DNGN_CLOSED_DOOR)
    {
        diag_door++;
    }

    if (grd[room_x2][room_y2] > 16
        || grd[room_x2][room_y2] == DNGN_CLOSED_DOOR)
    {
        diag_door++;
    }

    if ((diag_door + find_door) > 1 && max_doors == 1)
    {
        bk--;
        time_run++;
        return;
    }

    if (find_door == 0 || find_door > max_doors)
    {
        bk--;
        time_run++;
        return;
    }

    for (bcount_x = room_x1; bcount_x < room_x2; bcount_x++)
    {
        for (bcount_y = room_y1; bcount_y < room_y2; bcount_y++)
        {
            //grd[bcount_x][bcount_y] = DNGN_FLOOR;
            if (grd[bcount_x][bcount_y] == 105)
            {
                bk--;
                time_run++;
                return;
            }
        }
    }

    for (bcount_x = room_x1; bcount_x < room_x2; bcount_x++)
    {
        for (bcount_y = room_y1; bcount_y < room_y2; bcount_y++)
        {
            if (grd[bcount_x][bcount_y] <= DNGN_FLOOR)
                grd[bcount_x][bcount_y] = DNGN_FLOOR;
        }
    }

    for (bcount_y = room_y1; bcount_y < room_y2; bcount_y++)
    {
        if (grd[room_x1 - 1][bcount_y] == DNGN_FLOOR
            && grd[room_x1 - 1][bcount_y - 1] <= 16
            && grd[room_x1 - 1][bcount_y + 1] <= 16
            && bcount_y > room_y1 && bcount_y < room_y2)
        {
            if (random2(10) < doorlevel)
                grd[room_x1 - 1][bcount_y] = DNGN_CLOSED_DOOR;
        }

        if (grd[room_x2][bcount_y] == DNGN_FLOOR
            && grd[room_x2][bcount_y - 1] <= 16
            && grd[room_x2][bcount_y + 1] <= 16
            && bcount_y > room_y1 && bcount_y < room_y2)
        {
            if (random2(10) < doorlevel)
                grd[room_x2][bcount_y] = DNGN_CLOSED_DOOR;
        }
    }

    for (bcount_x = room_x1; bcount_x < room_x2; bcount_x++)
    {
        if (grd[bcount_x][room_y1 - 1] == DNGN_FLOOR
            && grd[bcount_x - 1][room_y1 - 1] <= 16
            && grd[bcount_x + 1][room_y1 - 1] <= 16
            && bcount_x > room_x1 && bcount_x < room_x2)
        {
            if (random2(10) < doorlevel)
                grd[bcount_x][room_y1 - 1] = DNGN_CLOSED_DOOR;
        }

        if (grd[bcount_x][room_y2] == DNGN_FLOOR
            && grd[bcount_x - 1][room_y2] <= 16
            && grd[bcount_x + 1][room_y2] <= 16
            && bcount_x > room_x1 && bcount_x < room_x2)
        {
            if (random2(10) < doorlevel)
                grd[bcount_x][room_y2] = DNGN_CLOSED_DOOR;
        }
    }

    time_run++;
}                               //end make_room()

static void big_room(void)
{
    unsigned char type_floor = DNGN_FLOOR;
    unsigned char type_2 = DNGN_FLOOR;
    int i, j, k, l;

    if (one_chance_in(4))
    {
        oblique = 5 + random2(20);
        oblique_max = oblique;

        room_x1 = 8 + random2(30);
        room_y1 = 8 + random2(22);
        room_x2 = room_x1 + 20 + random2(10);
        room_y2 = room_y1 + 20 + random2(8);

        if (!one_chance_in(5) || many_many < 8 + random2(8))
        {
            octa_room(DNGN_FLOOR);
            //chequerboard(grd, type_floor, DNGN_FLOOR, type_floor,
            // room_x1 + 1, room_y1 + 1, room_x2 - 1, room_y2 - 1);
            return;
        }

        type_floor = DNGN_LAVA;

        if (many_many > 7)      // stops water on level 1
        {
            type_floor = ((random2(many_many) < 14) ? DNGN_DEEP_WATER
                                                    : DNGN_LAVA);
        }

        if (many_many >= 75 && many_many < 80)  // Dis - Is this even possible?
            return;
        else if (many_many >= 80 && many_many < 85)     // Gehenna
            type_floor = DNGN_LAVA;
        else if (many_many >= 85 && many_many < 90)     // Cocytus
            type_floor = DNGN_DEEP_WATER;
        else if (many_many >= 90 && many_many < 95)     // Tartarus
            return;
        else
        {
            octa_room(type_floor);
            return;
        }
    }

    room_x1 = 8 + random2(30);
    room_y1 = 8 + random2(22);
    room_x2 = room_x1 + 20 + random2(10);
    room_y2 = room_y1 + 20 + random2(8);

    for (bi = room_x1; bi < room_x2; bi++)
    {
        for (bj = room_y1; bj < room_y2; bj++)
        {
            if (grd[bi][bj] == 105)
                return;
        }
    }

    if (many_many > 7 && one_chance_in(4))      // stops water on level 1
    {
        type_floor = ((random2(many_many) < 14) ? DNGN_DEEP_WATER
                                                : DNGN_LAVA);
    }

    // Is this even possible?
    if (many_many >= 75 && many_many < 80)
        return;

    if (many_many >= 80 && many_many < 85 && !one_chance_in(3)) // Gehenna
        type_floor = DNGN_LAVA;

    if (many_many >= 85 && many_many < 90 && !one_chance_in(3)) // Cocytus
        type_floor = DNGN_DEEP_WATER;

    //if ( many_many >= 90 && many_many < 95 )    // Tartarus
    //  return;

    for (bi = room_x1; bi < room_x2; bi++)
    {
        for (bj = room_y1; bj < room_y2; bj++)
        {
            if (grd[bi][bj] == DNGN_ROCK_WALL
                || grd[bi][bj] == DNGN_CLOSED_DOOR)
            {
                grd[bi][bj] = type_floor;
            }
        }
    }

    if (type_floor == DNGN_FLOOR)
        type_2 = DNGN_ROCK_WALL + random2(4);

    if (you.where_are_you == BRANCH_CRYPT || you.where_are_you == BRANCH_TOMB)
    {
        if (type_floor == DNGN_LAVA)
            type_floor = DNGN_SHALLOW_WATER;
        if (type_2 == DNGN_LAVA)
            type_2 == DNGN_SHALLOW_WATER;
    }

    if (one_chance_in(4))
    {
        chequerboard( type_floor, type_2, type_floor, room_x1 + 1, room_y1 + 1,
                      room_x2 - 1, room_y2 - 1 );
    }
    else if (one_chance_in(6))
    {
        i = room_x1;
        j = room_y1;
        k = room_x2;
        l = room_y2;

        do
        {
            i = i + 2 + random2(3);
            j = j + 2 + random2(3);
            k = k - 2 - random2(3);
            l = l - 2 - random2(3);
            if (i >= k - 3)
                break;
            if (j >= l - 3)
                break;

            box_room(i, k, j, l, DNGN_STONE_WALL);

        }
        while (many_many < 1500);       // ie forever
    }
}                               // end big_room()

static void diamond_rooms(void)
{
    char numb_diam = 1 + random2(10);
    char type_floor = DNGN_DEEP_WATER;
    int runthru = 0;

    // I guess no diamond rooms in either of these places {dlb}:
    if (you.where_are_you == BRANCH_DIS
        || you.where_are_you == BRANCH_TARTARUS)
    {
        return;
    }

    // there must be some way to make clearer sense of this morass !!! {dlb}
    if (many_many > 5 + random2(5) && coinflip())
        type_floor = DNGN_SHALLOW_WATER;

    if (many_many > 10 + random2(5) && coinflip())
        type_floor = DNGN_DEEP_WATER;

    if (many_many > 17 && coinflip())
        type_floor = DNGN_LAVA;

    if (many_many > 10 && one_chance_in(15))
        type_floor = (coinflip()? DNGN_STONE_WALL : DNGN_ROCK_WALL);

    if (many_many > 12 && one_chance_in(20))
        type_floor = DNGN_METAL_WALL;

    if (you.where_are_you == BRANCH_GEHENNA)
        type_floor = DNGN_LAVA;
    else if (you.where_are_you == BRANCH_COCYTUS)
        type_floor = DNGN_DEEP_WATER;

    //type_floor = DNGN_DEEP_WATER;
    for (bk = 0; bk < numb_diam; bk++)
    {
        room_x1 = 8 + random2(43);
        room_y1 = 8 + random2(35);
        room_x2 = room_x1 + 5 + random2(15);
        room_y2 = room_y1 + 5 + random2(10);

        oblique = (room_x2 - room_x1) / 2;      //random2(20) + 5;

        oblique_max = oblique;

        for (bi = room_x1; bi < room_x2; bi++)
        {
            for (bj = room_y1 + oblique; bj < room_y2 - oblique; bj++)
            {
                if (grd[bi][bj] == DNGN_FLOOR)
                    goto draw_it;
            }

            if (oblique > 0)
                oblique--;

            if (bi > room_x2 - oblique_max)
                oblique += 2;
        }

        bk--;
        runthru++;

        if (runthru > 9)
        {
            runthru = 0;
            bk++;
        }
        continue;

      draw_it:
        octa_room(type_floor);
    }                           // end "for(bk...)"
}                               // end diamond_rooms()

static void octa_room(unsigned char type_floor)
{
    // hack - avoid lava in the crypt {gdl}
    if ((you.where_are_you == BRANCH_CRYPT || you.where_are_you == BRANCH_TOMB)
         && type_floor == DNGN_LAVA)
        type_floor = DNGN_SHALLOW_WATER;

    for (bi = room_x1; bi < room_x2; bi++)
    {
        for (bj = room_y1 + oblique; bj < room_y2 - oblique; bj++)
        {
            if (grd[bi][bj] == 105)
                return;
        }

        if (oblique > 0)
            oblique--;

        if (bi > room_x2 - oblique_max)
            oblique += 2;
    }

    oblique = oblique_max;

    for (bi = room_x1; bi < room_x2; bi++)
    {
        for (bj = room_y1 + oblique; bj < room_y2 - oblique; bj++)
        {

            //if (bj - room_y1 <= oblique && i - room_x1 <= oblique) continue;
            //if (bj - room_y1 + i - room_x1 < oblique) continue;
            //if ((room_y2 - j) * -1 + room_x2 - 1 < oblique) continue;

            if (grd[bi][bj] == DNGN_ROCK_WALL)
                grd[bi][bj] = type_floor;

            if (grd[bi][bj] == DNGN_FLOOR && type_floor == DNGN_SHALLOW_WATER)
                grd[bi][bj] = DNGN_SHALLOW_WATER;

            if (type_floor >= DNGN_LAST_SOLID_TILE
                && grd[bi][bj] == DNGN_CLOSED_DOOR)
            {
                grd[bi][bj] = DNGN_FLOOR;       // ick
            }
        }

        if (oblique > 0)
            oblique--;

        if (bi > room_x2 - oblique_max)
            oblique += 2;
    }
}                               // end octa_room()

static void make_trail(void)
{
    int temp_rand = 0;          // probability determination {dlb}
    int dir_x2 = 0;
    int dir_y2 = 0;

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

        if (dir_x == 0)
            length = random2(corrlength) + 2;

        bi = 0;

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

            // This bit is a near copy of the above:
            if (grd[x_ps + dir_x][y_ps + dir_y] == 105)
                break;

            // Shouldn't this be:
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

    finish = 0;
}                               // end make_trail()

static bool is_weapon_special(int the_weapon)
{
    return ((mitm.special[the_weapon] % 30) != SPWPN_NORMAL);
}                               // end is_weapon_special()

static void set_weapon_special(int the_weapon, int spwpn)
{
    mitm.special[the_weapon] = (mitm.special[the_weapon] / 30) * 30 + spwpn;
}                               // end set_weapon_special()

static void special_room(void)
{
    char spec_room_type;
    int lev_mons;
    int thing_created = 0;

    //char depth_max = 3;   // not used anywhere in any function {dlb}
    unsigned char glopop = OBJ_RANDOM;  // used in calling items() {dlb}
    unsigned char loopy;        // general purpose loop variable {dlb}
    int temp_rand = 0;          // probability determination {dlb}

    FixedVector < int, 10 > mons_alloc; // was [20] {dlb}

    char lordx = 0, lordy = 0;

    room_x1 = 8 + random2(55);
    room_y1 = 8 + random2(45);
    room_x2 = room_x1 + 6 + random2(4);
    room_y2 = room_y1 + 6 + random2(4);

    // inner room? {dlb}
    for (bcount_x = room_x1; bcount_x < room_x2; bcount_x++)
    {
        for (bcount_y = room_y1; bcount_y < room_y2; bcount_y++)
        {
            grd[bcount_x][bcount_y] = 105;
        }
    }

    x1 = room_x1 + 1;
    x2 = room_x2 - 1;
    y1 = room_y1 + 1;
    y2 = room_y2 - 1;

    // outer room? {dlb}
    for (bcount_x = x1; bcount_x < x2; bcount_x++)
    {
        for (bcount_y = y1; bcount_y < y2; bcount_y++)
        {
            grd[bcount_x][bcount_y] = DNGN_ENTER_ZOT;   // value was 117 {dlb}
        }
    }

    // placing monsters in special room:
    bno_mons = 0;

    for (bk = 0; bk < MAX_MONSTERS; bk++)
    {
        if (menv[bk].type != -1)
            bno_mons++;
    }

    if (many_many < 7)
    {
        spec_room_type = SROOM_LAIR_KOBOLD;
    }
    else
    {
        spec_room_type = random2(NUM_SPECIAL_ROOMS);

        if (many_many < 23 && one_chance_in(4))
            spec_room_type = SROOM_BEEHIVE;

        if (many_many < 17 && one_chance_in(4))
            spec_room_type = SROOM_LAIR_ORC;

        if (many_many > 17 && spec_room_type == SROOM_LAIR_KOBOLD)
            spec_room_type = SROOM_LAIR_ORC;
    }


    switch (spec_room_type)
    {
    case SROOM_LAIR_ORC:
        // determine which monster array to generate {dlb}:
        lev_mons = ((many_many > 24) ? 3 :
                    (many_many > 15) ? 2 :
                    (many_many >  9) ? 1
                                     : 0);

        // fill with baseline monster type {dlb}:
        for (loopy = 0; loopy < 10; loopy++)
        {
            mons_alloc[loopy] = MONS_ORC;
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
        for (bcount_x = x1; bcount_x < x2; bcount_x++)
        {
            for (bcount_y = y1; bcount_y < y2; bcount_y++)
            {
                if (bno_mons > 80)
                    return;

                if (one_chance_in(4))
                    continue;

                for (bk = 0; bk < 85; bk++)
                {
                    if (menv[bk].type == -1)
                        break;
                }

                menv[bk].type = mons_alloc[random2(10)];
                define_monster(bk, menv);

                menv[bk].x = bcount_x;
                menv[bk].y = bcount_y;

                bno_mons++;

                give_item();
            }
        }
        break;

    case SROOM_LAIR_KOBOLD:
        lordx = x1 + random2(x2 - x1);
        lordy = y1 + random2(y2 - y1);

        // determine which monster array to generate {dlb}:
        lev_mons = ((many_many < 4) ? 0 :
                    (many_many < 6) ? 1 : (many_many < 9) ? 2 : 3);

        // fill with baseline monster type {dlb}:
        for (loopy = 0; loopy < 10; loopy++)
        {
            mons_alloc[loopy] = MONS_KOBOLD;
        }

        // fill in with special monster types {dlb}:
        // in this case, they are uniformly the same {dlb}:
        for (loopy = (7 - lev_mons); loopy < 10; loopy++)
        {
            mons_alloc[loopy] = MONS_BIG_KOBOLD;
        }

        // place monsters and give them items {dlb}:
        for (bcount_x = x1; bcount_x < x2; bcount_x++)
        {
            for (bcount_y = y1; bcount_y < y2; bcount_y++)
            {
                if (bno_mons > 80)
                    return;

                if (one_chance_in(4))
                    continue;

                for (bk = 0; bk < 85; bk++)
                {
                    if (menv[bk].type == -1)
                        break;
                }

                if (bcount_x == lordx && bcount_y == lordy)
                    menv[bk].type = MONS_BIG_KOBOLD;
                else
                    menv[bk].type = mons_alloc[random2(10)];

                define_monster(bk, menv);

                menv[bk].x = bcount_x;
                menv[bk].y = bcount_y;

                bno_mons++;

                give_item();
            }
        }
        break;

        // should only appear in deep levels, with a guardian
        // Maybe have several types of treasure room?
    case SROOM_TREASURY:
        // place treasure {dlb}:
        for (bcount_x = x1; bcount_x < x2; bcount_x++)
        {
            for (bcount_y = y1; bcount_y < y2; bcount_y++)
            {
                temp_rand = random2(11);

                glopop = ((temp_rand > 8) ? OBJ_WEAPONS :       // 2 in 11
                          (temp_rand > 6) ? OBJ_ARMOUR :        // 2 in 11
                          (temp_rand > 5) ? OBJ_MISSILES :      // 1 in 11
                          (temp_rand > 4) ? OBJ_WANDS :         // 1 in 11
                          (temp_rand > 3) ? OBJ_SCROLLS :       // 1 in 11
                          (temp_rand > 2) ? OBJ_JEWELLERY :     // 1 in 11
                          (temp_rand > 1) ? OBJ_BOOKS :         // 1 in 11
                          (temp_rand > 0) ? OBJ_STAVES          // 1 in 11
                                          : OBJ_POTIONS);       // 1 in 11

                thing_created = items( 1, glopop, OBJ_RANDOM, 1,
                                                    many_many * 3, 250 );

                mitm.x[thing_created] = bcount_x;
                mitm.y[thing_created] = bcount_y;
            }
        }

        // place guardian {dlb}:
        for (bk = 0; bk < MAX_MONSTERS; bk++)
        {
            if (menv[bk].type == -1)
                break;
            if (bk == MAX_MONSTERS - 3)
                return;         // surely this couldn't happen?!
        }

        menv[bk].type = MONS_GUARDIAN_NAGA;
        define_monster(bk, menv);

        menv[bk].x = x1;
        menv[bk].y = y1;

        bno_mons++;
        break;

    case SROOM_BEEHIVE:
        beehive();
        break;
    }
}                               // end special_room()

static void beehive(void)
{
    int temp_rand = 0;          // probability determination {dlb}
    long quant;                 // these odd things are to avoid warnings

    for (bcount_x = x1; bcount_x < x2; bcount_x++)
    {
        for (bcount_y = y1; bcount_y < y2; bcount_y++)
        {
            if (coinflip())
                continue;

            for (bp = 0; bp < 210; bp++)
            {
                if (bp == 200)
                    goto finished_it;

                if (mitm.base_type[bp] == OBJ_UNASSIGNED)
                    break;
            }

            mitm.base_type[bp] = OBJ_FOOD;
            mitm.sub_type[bp] = (one_chance_in(25) ? FOOD_ROYAL_JELLY
                                                   : FOOD_HONEYCOMB);

            quant = 1;
            mitm.quantity[bp] = quant;

            mitm.x[bp] = bcount_x;
            mitm.y[bp] = bcount_y;

            item_colour(bp);
        }
    }


  finished_it:

    bool queen = true;

    for (bcount_x = x1; bcount_x < x2; bcount_x++)
    {
        for (bcount_y = y1; bcount_y < y2; bcount_y++)
        {
            if (bno_mons > 100)
                return;

            if (grd[bcount_x][bcount_y] != DNGN_FLOOR
                && grd[bcount_x][bcount_y] != DNGN_ENTER_ZOT)
            {
                continue;       // shouldn't need this, but oh well.
            }

            // 117 for "special rooms"
            // ??? = DNGN_ENTER_ZOT 17jan2000 {dlb}
            for (bk = 0; bk < 85; bk++)
            {
                if (menv[bk].type == -1)
                    break;
            }

            temp_rand = random2(6);

            menv[bk].type = ((temp_rand > 1) ? MONS_KILLER_BEE :
                             (temp_rand > 0) ? ((queen) ? MONS_QUEEN_BEE
                                                        : MONS_KILLER_BEE)
                                             : MONS_KILLER_BEE_LARVA);

            if (temp_rand == 1)
                queen = false;
            else if (bcount_x == x2 - 1 && bcount_y == y2 - 1)
                menv[bk].type = MONS_QUEEN_BEE;

            define_monster(bk, menv);

            menv[bk].x = bcount_x;
            menv[bk].y = bcount_y;

            mgrd[menv[bk].x][menv[bk].y] = bk;
            bno_mons++;

            // who wrote this and why? {dlb}
            menv[bk].behavior = BEH_SLEEP; // should be sleeping
        }
    }
}                               // end beehive()

static void morgue(void)
{
    int temp_rand = 0;          // probability determination {dlb}

    for (bcount_x = x1; bcount_x < x2; bcount_x++)
    {
        for (bcount_y = y1; bcount_y < y2; bcount_y++)
        {
            if (bno_mons > 100)
                return;

            if (grd[bcount_x][bcount_y] != DNGN_FLOOR
                && grd[bcount_x][bcount_y] != DNGN_ENTER_ZOT)
            {
                continue;       // shouldn't need this, but oh well.
            }

            // 117 for "special rooms" -- if anyone can decipher this
            // and relate to DNGN_ENTER_ZOT (117), let me know {dlb}
            for (bk = 0; bk < 85; bk++)
            {
                if (menv[bk].type == -1)
                    break;
            }

            temp_rand = random2(24);

            menv[bk].type = ((temp_rand > 11) ? MONS_ZOMBIE_SMALL :  // 50.0%
                             (temp_rand >  7) ? MONS_WIGHT :         // 16.7%
                             (temp_rand >  3) ? MONS_NECROPHAGE :    // 16.7%
                             (temp_rand >  0) ? MONS_WRAITH          // 12.5%
                                              : MONS_VAMPIRE);       //  4.2%

            if (menv[bk].type == MONS_ZOMBIE_SMALL)
                define_zombie(3, 250, 250);
            else
                define_monster(bk, menv);

            menv[bk].x = bcount_x;
            menv[bk].y = bcount_y;

            mgrd[menv[bk].x][menv[bk].y] = bk;

            bno_mons++;

            // who wrote this and why? {dlb}
            menv[bk].behavior = BEH_CHASING_II; // should be sleeping
        }
    }
}                               // end morgue()

static void define_zombie(char not_zombsize,    /* 1=2, 2=1, 3=1 or 2 */
                          int ztype, int cs)
{
    int mons_sec2 = 0;

    // that is, random creature from which to fashion undead
    if (ztype == 250)
    {
        do
        {
            // seems to me like this value needs to be updated {dlb}
            menv[bk].number = random2(80);
        }
        while (!mons_zombie_size(menv[bk].number)
               || mons_zombie_size(menv[bk].number) == not_zombsize
               || (cs > MONS_GIANT_MITE && !mons_skeleton(menv[bk].number)));

        mons_sec2 = menv[bk].number;
    }
    else
    {
        mons_sec2 = ztype;
        menv[bk].number = ztype;
    }

    menv[bk].number = mons_charclass(menv[bk].number);
    menv[bk].type = menv[bk].number;

    define_monster(bk, menv);

    menv[bk].hit_points = hit_points(menv[bk].hit_dice, 6, 5);
    menv[bk].max_hit_points = menv[bk].hit_points;

    menv[bk].armor_class -= 2;

    if (menv[bk].armor_class < 0)
        menv[bk].armor_class = 0;

    menv[bk].evasion -= 5;

    if (menv[bk].evasion < 0)
        menv[bk].evasion = 0;

    menv[bk].speed -= 2;

    if (menv[bk].speed < 3)
        menv[bk].speed = 3;

    menv[bk].speed_increment = 70;
    menv[bk].number = mons_sec2;

    if (cs == MONS_ZOMBIE_SMALL || cs == MONS_ZOMBIE_LARGE)
    {
        menv[bk].type = ((mons_zombie_size(menv[bk].number) == 2)
                                    ? MONS_ZOMBIE_LARGE : MONS_ZOMBIE_SMALL);
    }
    else if (cs == MONS_SKELETON_SMALL || cs == MONS_SKELETON_LARGE)
    {
        menv[bk].hit_points = hit_points(menv[bk].hit_dice, 5, 4);
        menv[bk].max_hit_points = menv[bk].hit_points;

        menv[bk].armor_class -= 4;

        if (menv[bk].armor_class < 0)
            menv[bk].armor_class = 0;

        menv[bk].evasion -= 2;

        if (menv[bk].evasion < 0)
            menv[bk].evasion = 0;

        menv[bk].type = ((mons_zombie_size(menv[bk].number) == 2)
                            ? MONS_SKELETON_LARGE : MONS_SKELETON_SMALL);
    }
    else if (cs == MONS_SIMULACRUM_SMALL || cs == MONS_SIMULACRUM_LARGE)
    {
        menv[bk].hit_points = hit_points(menv[bk].hit_dice, 4, 4);
        menv[bk].max_hit_points = menv[bk].hit_points;
        menv[bk].type = ((mons_zombie_size(menv[bk].number) == 2)
                            ? MONS_SIMULACRUM_LARGE : MONS_SIMULACRUM_SMALL);
    }
    else if (cs == MONS_SPECTRAL_THING)
    {
        menv[bk].hit_points = hit_points(menv[bk].hit_dice, 4, 4);
        menv[bk].max_hit_points = menv[bk].hit_points;
        menv[bk].armor_class += 4;
        menv[bk].type = MONS_SPECTRAL_THING;
    }

    menv[bk].number = mons_sec2;
}                               // end define_zombie()

static void specr_2(void)
{
    int bkout = 0;
    int cx = 0, cy = 0;         // these were globals, once {dlb}
    int sx = 0, sy = 0;         // these were globals, once {dlb}
    int dx = 0, dy = 0;         // these were globals, once {dlb}

  grolko:

    if (bkout > 100)
        return;

    switch (random2(4))
    {
    case 0:
        cx = x1 + (random2(x2 - x1));
        cy = y1;
        dx = 0;
        dy = -1;
        break;
    case 1:
        cx = x1 + (random2(x2 - x1));
        cy = y2;
        dx = 0;
        dy = 1;
        break;
    case 2:
        cy = y1 + (random2(y2 - y1));
        cx = x1;
        dx = -1;
        dy = 0;
        break;
    case 3:
        cy = y1 + (random2(y2 - y1));
        cx = x2;
        dx = 1;
        dy = 0;
        break;
    }

    sx = cx;
    sy = cy;

    for (bcount_x = 0; bcount_x < 100; bcount_x++)
    {
        sx += dx;
        sy += dy;

        if (sx < 6 || sx > (GXM - 7) || sy < 6 || sy > (GYM - 7))
        {
            bkout++;
            goto grolko;
        }

        if (bcount_x > 0)
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

    for (bcount_y = 0; bcount_y < bcount_x + 2; bcount_y++)
    {
        if (grd[sx][sy] == 105)
            grd[sx][sy] = DNGN_CLOSED_DOOR;

        if (bcount_y > 0 && grd[sx + dx][sy + dy] > DNGN_ROCK_WALL
            && grd[sx + dx][sy + dy] < DNGN_FLOOR)
            grd[sx][sy] = DNGN_ROCK_STAIRS_UP;

        if (grd[sx][sy] == DNGN_ROCK_WALL)
            grd[sx][sy] = 106;

        sx += dx;
        sy += dy;
    }
}                               // end specr_2()

static void give_item(void)
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
    int give_level = many_many;

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

    mitm.x[bp] = 1;
    mitm.y[bp] = 1;

    if (menv[bk].type == MONS_DANCING_WEAPON
        && you.where_are_you == BRANCH_HALL_OF_BLADES && one_chance_in(3))
    {
        give_level = 351;
    }

    // moved setting of quantity here to keep it in mind {dlb}
    iquan = 1;
    // I wonder if this is even used, given calls to item() {dlb}

    switch (menv[bk].type)
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

        if (menv[bk].inv[MSLOT_WEAPON] != NON_ITEM)
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
    }                           // end "switch(menv[bk].type)"


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

    mitm.x[thing_created] = 1;
    mitm.y[thing_created] = 1;
    mitm.id[thing_created] = 0;

    if (mitm.base_type[thing_created] == OBJ_WEAPONS)
        // hand_used = 0 unless Ettin's 2nd hand etc.
        menv[bk].inv[hand_used] = thing_created;
    else if (mitm.base_type[thing_created] == OBJ_MISSILES)
        menv[bk].inv[MSLOT_MISSILE] = thing_created;
    else if (mitm.base_type[thing_created] == OBJ_SCROLLS)
        menv[bk].inv[MSLOT_SCROLL] = thing_created;
    // but not potions? huh? {dlb}
    // only Geryon gets something other than weapon explicitly {dlb}
    else if (mitm.base_type[thing_created] == OBJ_GOLD
             || mitm.base_type[thing_created] == OBJ_MISCELLANY)
    {
        menv[bk].inv[MSLOT_POTION] = thing_created;
    }

    // SPWPN_PROTECTION and NWPN_S_o_Z ??? {dlb}
    if (mitm.base_type[thing_created] == OBJ_WEAPONS
        && mitm.special[thing_created] % 30 == SPWPN_PROTECTION)
    {
        menv[bk].armor_class += 5;
    }

    if (!force_item)
        item_colour(thing_created);

  give_armour:

    // actually, should be give_ammo now

    // this bit gives ammunition to go with missile hand weapons.
    // only gives darts for hand xbows

    // note that force_spec is not reset for this section

    if (menv[bk].inv[MSLOT_WEAPON] != NON_ITEM
        && launches_things(mitm.sub_type[menv[bk].inv[MSLOT_WEAPON]]))
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
        mitm.sub_type[bp] = launched_by(mitm.sub_type[menv[bk].inv[MSLOT_WEAPON]]);
        iquan = 3 + random2avg(16, 2);

        // that is, lose SPWPN's, I think, but retain racial typing {dlb}
        mitm.special[bp] = (mitm.special[menv[bk].inv[MSLOT_WEAPON]] / 30) * 30;

        if (force_item)
            mitm.quantity[bp] = iquan;

        xitc = mitm.base_type[bp];
        xitt = mitm.sub_type[bp];

        thing_created = ((force_item) ? bp : items( 0, xitc, xitt, 1,
                                                    give_level, force_spec ));

        mitm.x[thing_created] = 1;
        mitm.y[thing_created] = 1;
        mitm.id[thing_created] = 0;
        menv[bk].inv[MSLOT_MISSILE] = thing_created;

        // again, SPWPN_PROTECTION + ???, I think {dlb}
        if (mitm.base_type[thing_created] == OBJ_WEAPONS
            && mitm.special[thing_created] % 30 == SPWPN_PROTECTION)
        {
            menv[bk].armor_class += 3;
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

    switch (menv[bk].type)
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
    }                           // end of switch(menv [bk].type)

    // because it may have been set earlier by giving ammo or weapons {dlb}
    iquan = 1;

    xitc = mitm.base_type[bp];
    xitt = mitm.sub_type[bp];

    thing_created = items(0, xitc, xitt, 1, 1 + (many_many / 2), force_spec);

    mitm.x[thing_created] = 1;
    mitm.y[thing_created] = 1;
    menv[bk].inv[MSLOT_ARMOUR] = thing_created;

    // Wights' robes are white:
    // this completely overrides colouring above -- d'oh! {dlb}
    if (menv[bk].type != MONS_WIGHT)
        item_colour(thing_created);
    else
        mitm.colour[thing_created] = WHITE;

    menv[bk].armor_class += property( mitm.base_type[thing_created],
                                        mitm.sub_type[thing_created], PARM_AC );

    int armour_plus = 0;

    armour_plus = mitm.pluses[thing_created]
                        - ((mitm.pluses[thing_created] > 130) ? 150 : 50);

    ASSERT(abs(armour_plus) < 20);

    if (abs(armour_plus) < 20)
        menv[bk].armor_class += armour_plus;

    menv[bk].evasion += property( mitm.base_type[thing_created],
                                    mitm.sub_type[thing_created],
                                    PARM_EVASION ) / 2;

    if (menv[bk].evasion < 1)
        menv[bk].evasion = 1;   // This *shouldn't* happen.
}                               // end give_item()

static void item_colour(int p)
{
    bp = p;                     // what to do about this? {dlb}
    int switchnum = 0;

    switch (mitm.base_type[bp])
    {
    case OBJ_WEAPONS:
        if (mitm.special[bp] % 30 == SPWPN_RANDART_I)
            break;              // unrandarts already coloured

        if (mitm.special[bp] >= NWPN_SINGING_SWORD)
        {
            switch (mitm.special[bp])   // was: - 180, but that is *wrong* {dlb}
            {
            case NWPN_SINGING_SWORD:
            case NWPN_SCEPTRE_OF_TORMENT:
                mitm.colour[bp] = YELLOW;
                break;
            case NWPN_WRATH_OF_TROG:
            case NWPN_SWORD_OF_POWER:
                mitm.colour[bp] = RED;
                break;
            case NWPN_SCYTHE_OF_CURSES:
                mitm.colour[bp] = DARKGREY;
                break;
            case NWPN_MACE_OF_VARIABILITY:
                mitm.colour[bp] = random_colour();
                break;
            case NWPN_GLAIVE_OF_PRUNE:
                mitm.colour[bp] = MAGENTA;
                break;
            case NWPN_SWORD_OF_ZONGULDROK:
                mitm.colour[bp] = LIGHTGREY;
                break;
            case NWPN_KNIFE_OF_ACCURACY:
                mitm.colour[bp] = LIGHTCYAN;
                break;
            case NWPN_STAFF_OF_OLGREB:
                mitm.colour[bp] = GREEN;
                break;
            case NWPN_VAMPIRES_TOOTH:
                mitm.colour[bp] = WHITE;
                break;
            case NWPN_STAFF_OF_WUCAD_MU:
                mitm.colour[bp] = BROWN;
                break;
            }
            break;
        }

        if (is_demonic(mitm.sub_type[bp]))
            mitm.colour[bp] = random_colour();
        else if (launches_things(mitm.sub_type[bp]))
            mitm.colour[bp] = BROWN;
        else
        {
            switch (mitm.sub_type[bp])
            {
            case WPN_CLUB:
            case WPN_GIANT_CLUB:
            case WPN_GIANT_SPIKED_CLUB:
            case WPN_ANCUS:
            case WPN_WHIP:
            case WPN_QUARTERSTAFF:
                mitm.colour[bp] = BROWN;
                break;
            case WPN_QUICK_BLADE:
                mitm.colour[bp] = LIGHTBLUE;
                break;
            case WPN_EXECUTIONERS_AXE:
                mitm.colour[bp] = RED;
                break;
            default:
                mitm.colour[bp] = LIGHTCYAN;
                if (mitm.special[bp] / 30 == DWPN_DWARVEN)
                    mitm.colour[bp] = CYAN;
                break;
            }
        }

        // I don't think this is ever done -- see start of case {dlb}:
        if (mitm.special[bp] % 30 >= SPWPN_RANDART_I && one_chance_in(5))
            mitm.colour[bp] = random_colour();
        break;

    case OBJ_MISSILES:
        switch (mitm.sub_type[bp])
        {
        case MI_STONE:
        case MI_LARGE_ROCK:
            mitm.colour[bp] = BROWN;
            break;
        default:
            mitm.colour[bp] = LIGHTCYAN;
            if (mitm.special[bp] / 30 == DAMMO_DWARVEN)
                mitm.colour[bp] = CYAN;
            break;
        }
        break;

    case OBJ_ARMOUR:
        if (mitm.special[bp] % 30 == SPARM_RANDART_I)
            break;              /* unrandarts have already been coloured */

        switch (mitm.sub_type[bp])
        {
        case ARM_CLOAK:
        case ARM_ROBE:
            mitm.colour[bp] = random_colour();
            break;

        case ARM_HELMET:
            //caps and wizard's hats are random coloured
            if (mitm.pluses2[bp] == 2 || mitm.pluses2[bp] == 3)
                mitm.colour[bp] = random_colour();
            else
                mitm.colour[bp] = LIGHTCYAN;
            break;

        case ARM_BOOTS: // maybe more interesting boot colours?
        case ARM_GLOVES:
        case ARM_LEATHER_ARMOUR:
            mitm.colour[bp] = BROWN;
            break;
        case ARM_DRAGON_HIDE:
        case ARM_DRAGON_ARMOUR:
            mitm.colour[bp] = mons_colour(MONS_DRAGON);
            break;
        case ARM_TROLL_HIDE:
        case ARM_TROLL_LEATHER_ARMOUR:
            mitm.colour[bp] = mons_colour(MONS_TROLL);
            break;
        case ARM_CRYSTAL_PLATE_MAIL:
            mitm.colour[bp] = LIGHTGREY;
            break;
        case ARM_ICE_DRAGON_HIDE:
        case ARM_ICE_DRAGON_ARMOUR:
            mitm.colour[bp] = mons_colour(MONS_ICE_DRAGON);
            break;
        case ARM_STEAM_DRAGON_HIDE:
        case ARM_STEAM_DRAGON_ARMOUR:
            mitm.colour[bp] = mons_colour(MONS_STEAM_DRAGON);
            break;
        case ARM_MOTTLED_DRAGON_HIDE:
        case ARM_MOTTLED_DRAGON_ARMOUR:
            mitm.colour[bp] = mons_colour(MONS_MOTTLED_DRAGON);
            break;
        case ARM_STORM_DRAGON_HIDE:
        case ARM_STORM_DRAGON_ARMOUR:
            mitm.colour[bp] = mons_colour(MONS_STORM_DRAGON);
            break;
        case ARM_GOLD_DRAGON_HIDE:
        case ARM_GOLD_DRAGON_ARMOUR:
            mitm.colour[bp] = mons_colour(MONS_GOLDEN_DRAGON);
            break;
        case ARM_ANIMAL_SKIN:
            mitm.colour[bp] = BROWN;
            break;
        case ARM_SWAMP_DRAGON_HIDE:
        case ARM_SWAMP_DRAGON_ARMOUR:
            mitm.colour[bp] = mons_colour(MONS_SWAMP_DRAGON);
            break;
        default:
            mitm.colour[bp] = LIGHTCYAN;
            if (mitm.special[bp] / 30 == DARM_DWARVEN)
                mitm.colour[bp] = CYAN;
            break;
        }

        // I don't think this is ever done -- see start of case {dlb}:
        if (mitm.special[bp] % 30 >= SPARM_RANDART_I && one_chance_in(5))
            mitm.colour[bp] = random_colour();
        break;

    case OBJ_WANDS:
        mitm.special[bp] = you.item_description[IDESC_WANDS][mitm.sub_type[bp]];

        switch (mitm.special[bp] % 12)
        {
        case 0:         //"iron wand"
            mitm.colour[bp] = CYAN;
            break;
        case 1:         //"brass wand"
        case 5:         //"gold wand"
            mitm.colour[bp] = YELLOW;
            break;
        case 2:         //"bone wand"
        case 8:         //"ivory wand"
        case 9:         //"glass wand"
        case 10:                //"lead wand"
            mitm.colour[bp] = LIGHTGREY;
            break;
        case 3:         //"wooden wand"
        case 4:         //"copper wand"
        case 7:         //"bronze wand"
            mitm.colour[bp] = BROWN;
            break;
        case 6:         //"silver wand"
            mitm.colour[bp] = WHITE;
            break;
        case 11:                //"plastic wand"
            mitm.colour[bp] = random_colour();
            break;
        }

        if (mitm.special[bp] / 12 == 9)
            mitm.colour[bp] = DARKGREY;

        // rare wands (eg disintegration - these will be very rare):
        // maybe only 1 thing, like: crystal, shining, etc.
        break;

    case OBJ_POTIONS:
        mitm.special[bp] = you.item_description[IDESC_POTIONS][mitm.sub_type[bp]];

        switch (mitm.special[bp] % 14)
        {
        case 0:         //"clear potion"
            mitm.colour[bp] = LIGHTGREY;
            break;
        case 1:         //"blue potion"
        case 7:         //"inky potion"
            mitm.colour[bp] = BLUE;
            break;
        case 2:         //"black potion"
            mitm.colour[bp] = DARKGREY;
            break;
        case 3:         //"silvery potion"
        case 13:        //"white potion"
            mitm.colour[bp] = WHITE;
            break;
        case 4:         //"cyan potion"
            mitm.colour[bp] = CYAN;
            break;
        case 5:         //"purple potion"
            mitm.colour[bp] = MAGENTA;
            break;
        case 6:         //"orange potion"
            mitm.colour[bp] = LIGHTRED;
            break;
        case 8:         //"red potion"
            mitm.colour[bp] = RED;
            break;
        case 9:         //"yellow potion"
            mitm.colour[bp] = YELLOW;
            break;
        case 10:        //"green potion"
            mitm.colour[bp] = GREEN;
            break;
        case 11:        //"brown potion"
            mitm.colour[bp] = BROWN;
            break;
        case 12:        //"pink potion"
            mitm.colour[bp] = LIGHTMAGENTA;
            break;
        }
        break;

    case OBJ_FOOD:
        switch (mitm.sub_type[bp])
        {
        case FOOD_BEEF_JERKY:
        case FOOD_BREAD_RATION:
        case FOOD_LYCHEE:
        case FOOD_MEAT_RATION:
        case FOOD_RAMBUTAN:
        case FOOD_SAUSAGE:
        case FOOD_SULTANA:
            mitm.colour[bp] = BROWN;
            break;
        case FOOD_BANANA:
        case FOOD_CHEESE:
        case FOOD_HONEYCOMB:
        case FOOD_LEMON:
        case FOOD_PIZZA:
        case FOOD_ROYAL_JELLY:
            mitm.colour[bp] = YELLOW;
            break;
        case FOOD_PEAR:
            mitm.colour[bp] = LIGHTGREEN;
            break;
        case FOOD_CHOKO:
        case FOOD_SNOZZCUMBER:
            mitm.colour[bp] = GREEN;
            break;
        case FOOD_APRICOT:
        case FOOD_ORANGE:
            mitm.colour[bp] = LIGHTRED;
            break;
        case FOOD_STRAWBERRY:
            mitm.colour[bp] = RED;
            break;
        case FOOD_APPLE:
            mitm.colour[bp] = (coinflip() ? RED : GREEN);
            break;
        case FOOD_GRAPE:
            mitm.colour[bp] = (coinflip() ? MAGENTA : GREEN);
            break;
        default:
            mitm.colour[bp] = BROWN;
        }
        break;

    case OBJ_JEWELLERY:
        if (mitm.special[bp] == 200)
        {
            mitm.colour[bp] = random_colour();
            break;
        }

        if (mitm.special[bp] == 201)
            break;              /* unrandarts have already been coloured */

        mitm.colour[bp] = YELLOW;
        mitm.special[bp] = you.item_description[IDESC_RINGS][mitm.sub_type[bp]];

        switchnum = mitm.special[bp] % 13;

        switch (switchnum)
        {
        case 0:
        case 5:
            mitm.colour[bp] = BROWN;
            break;
        case 1:
        case 8:
        case 11:
            mitm.colour[bp] = LIGHTGREY;
            break;
        case 2:
        case 6:
            mitm.colour[bp] = YELLOW;
            break;
        case 3:
        case 4:
            mitm.colour[bp] = CYAN;
            break;
        case 7:
            mitm.colour[bp] = BROWN;
            break;
        case 9:
        case 10:
            mitm.colour[bp] = WHITE;
            break;
        case 12:
            mitm.colour[bp] = GREEN;
            break;
        case 13:
            mitm.colour[bp] = LIGHTCYAN;
            break;
        }

        if (mitm.sub_type[bp] >= AMU_RAGE)
        {
            switch (switchnum)
            {
            case 0:             //"zirconium amulet"
            case 9:             //"ivory amulet"
            case 11:            //"platinum amulet"
                mitm.colour[bp] = WHITE;
                break;
            case 1:             //"sapphire amulet"
                mitm.colour[bp] = LIGHTBLUE;
                break;
            case 2:             //"golden amulet"
            case 6:             //"brass amulet"
                mitm.colour[bp] = YELLOW;
                break;
            case 3:             //"emerald amulet"
                mitm.colour[bp] = GREEN;
                break;
            case 4:             //"garnet amulet"
            case 8:             //"ruby amulet"
                mitm.colour[bp] = RED;
                break;
            case 5:             //"bronze amulet"
            case 7:             //"copper amulet"
                mitm.colour[bp] = BROWN;
                break;
            case 10:            //"bone amulet"
                mitm.colour[bp] = LIGHTGREY;
                break;
            case 12:            //"jade amulet"
                mitm.colour[bp] = GREEN;
                break;
            case 13:            //"plastic amulet"
                mitm.colour[bp] = random_colour();
            }
        }

        // blackened - same for both rings and amulets
        if (mitm.special[bp] / 13 == 5)
            mitm.colour[bp] = DARKGREY;
        break;

    case OBJ_SCROLLS:
        mitm.colour[bp] = LIGHTGREY;
        mitm.special[bp] = you.item_description[IDESC_SCROLLS][mitm.sub_type[bp]];
        mitm.pluses[bp] = you.item_description[IDESC_SCROLLS_II][mitm.sub_type[bp]];
        break;

    case OBJ_BOOKS:
        switch (mitm.special[bp] % 10)
        {
        case 0:
        case 1:
            mitm.colour[bp] = random_colour();
            break;
        case 2:
            mitm.colour[bp] = (one_chance_in(3) ? BROWN : DARKGREY);
            break;
        case 3:
            mitm.colour[bp] = CYAN;
            break;
        case 4:
            mitm.colour[bp] = LIGHTGREY;
            break;
        default:
            break;
        }
        break;

    case OBJ_STAVES:
        mitm.colour[bp] = BROWN;
        break;

    case OBJ_ORBS:
        mitm.colour[bp] = LIGHTMAGENTA;
        break;

    case OBJ_MISCELLANY:
        switch (mitm.sub_type[bp])
        {
        case MISC_BOTTLED_EFREET:
        case MISC_STONE_OF_EARTH_ELEMENTALS:
            mitm.colour[bp] = BROWN;
            break;

        case MISC_AIR_ELEMENTAL_FAN:
        case MISC_CRYSTAL_BALL_OF_ENERGY:
        case MISC_CRYSTAL_BALL_OF_FIXATION:
        case MISC_CRYSTAL_BALL_OF_SEEING:
        case MISC_DISC_OF_STORMS:
        case MISC_HORN_OF_GERYON:
        case MISC_LANTERN_OF_SHADOWS:
            mitm.colour[bp] = LIGHTGREY;
            break;

        case MISC_LAMP_OF_FIRE:
            mitm.colour[bp] = YELLOW;
            break;

        case MISC_BOX_OF_BEASTS:
        case MISC_RUNE_OF_ZOT:
            mitm.colour[bp] = DARKGREY;
            break;

        case MISC_EMPTY_EBONY_CASKET:
            mitm.colour[bp] = DARKGREY;
            break;

        case MISC_DECK_OF_SUMMONINGS:
        case MISC_DECK_OF_WONDERS:
        default:
            mitm.colour[bp] = random_colour();
            break;
        }
        break;

    case OBJ_CORPSES:
        mitm.colour[bp] = RED;
        break;
    }
}                               // end item_colour()

static void place_traps(void)
{
    int num_traps = random2avg(9, 2);

    for (bi = 0; bi < num_traps; bi++)
    {
        // traps can be placed in vaults
        if (env.trap_type[bi] != TRAP_UNASSIGNED)
            continue;

        do
        {
            env.trap_x[bi] = 10 + random2(GXM - 20);
            env.trap_y[bi] = 10 + random2(GYM - 20);
        }
        while (grd[env.trap_x[bi]][env.trap_y[bi]] != DNGN_FLOOR);

        env.trap_type[bi] = TRAP_DART;

        if (random2(1 + many_many) > 1)
            env.trap_type[bi] = TRAP_ARROW;
        if (random2(1 + many_many) > 3)
            env.trap_type[bi] = TRAP_SPEAR;
        if (random2(1 + many_many) > 5)
            env.trap_type[bi] = TRAP_AXE;
        if (random2(1 + many_many) > 7)
            env.trap_type[bi] = TRAP_BOLT;
        if (random2(1 + many_many) > 11)
            env.trap_type[bi] = TRAP_BLADE;

        if ((random2(1 + many_many) > 14 && one_chance_in(3))
            || (you.where_are_you == BRANCH_HALL_OF_ZOT && coinflip()))
        {
            env.trap_type[bi] = TRAP_ZOT;
        }

        if (one_chance_in(20))
            env.trap_type[bi] = TRAP_TELEPORT;
        if (one_chance_in(40))
            env.trap_type[bi] = TRAP_AMNESIA;

        grd[env.trap_x[bi]][env.trap_y[bi]] = DNGN_UNDISCOVERED_TRAP;
    }                           // end "for bi"
}                               // end place_traps()

static void link_items(void)
{
    int b = 0;
    int n = 0;
    int m = 0;

    for (bi = 0; bi < MAX_ITEMS; bi++)
    {
        if (mitm.base_type[bi] == OBJ_UNASSIGNED || mitm.quantity[bi] == 0
            || mitm.x[bi] == 1)
        {
            mitm.link[bi] = NON_ITEM;
            continue;
        }

        if (igrd[mitm.x[bi]][mitm.y[bi]] == NON_ITEM)
        {
            igrd[mitm.x[bi]][mitm.y[bi]] = bi;
            continue;
        }
        else
        {
            b = igrd[mitm.x[bi]][mitm.y[bi]];

            while (b != NON_ITEM)
            {
                m = b;
                n = mitm.link[b];
                b = n;
            }                   // end of while

            mitm.link[m] = bi;

        }
    }                           // end of for i

    for (bi = 0; bi < MAX_MONSTERS; bi++)
    {
        for (bj = MSLOT_GOLD; bj >= MSLOT_WEAPON; bj--)
        {
            if (menv[bi].inv[bj] == NON_ITEM)
                continue;

            const unsigned int item = menv[bi].inv[bj];
            if (igrd[ mitm.x[item] ][ mitm.y[item] ] == item)
                igrd[ mitm.x[item] ][ mitm.y[item] ] = mitm.link[ item ];
        }
    }
}                               // end link_items()

static void roguey_level(void)
{
    FixedVector < unsigned char, 30 > rox1;
    FixedVector < unsigned char, 30 > rox2;
    FixedVector < unsigned char, 30 > roy1;
    FixedVector < unsigned char, 30 > roy2;
    int cn = 0;

    for (bcount_y = 0; bcount_y < 5; bcount_y++)
    {
        for (bcount_x = 0; bcount_x < 5; bcount_x++)
        {
            // rooms:
            rox1[cn] = bcount_x * 13 + 8 + random2(4);
            roy1[cn] = bcount_y * 11 + 8 + random2(4);

            rox2[cn] = rox1[cn] + 3 + random2(8);
            roy2[cn] = roy1[cn] + 3 + random2(6);

            cn++;
        }
    }

    cn = 0;

    for (bi = 0; bi < 25; bi++)
    {
        for (bcount_x = rox1[bi]; bcount_x < rox2[bi]; bcount_x++)
        {
            for (bcount_y = roy1[bi]; bcount_y < roy2[bi]; bcount_y++)
            {
                if (grd[bcount_x][bcount_y] == DNGN_ROCK_WALL
                    && bcount_x < (GXM - 8))
                {
                    grd[bcount_x][bcount_y] = DNGN_FLOOR;
                }
            }
        }


        if (rox2[bi] - rox1[bi] > 5 && roy2[bi] - roy1[bi] > 5)
        {
            if (random2(100 - many_many) < 3)
            {
                if (!one_chance_in(4))
                {
                    box_room( rox1[bi] + 2, rox2[bi] - 2, roy1[bi] + 2,
                                roy2[bi] - 2, (coinflip() ? DNGN_STONE_WALL
                                                          : DNGN_ROCK_WALL) );
                }
                else
                {
                    box_room( rox1[bi] + 2, rox2[bi] - 2, roy1[bi] + 2,
                                 roy2[bi] - 2, DNGN_METAL_WALL );
                }

                if (coinflip())
                {
                    treasure_area( many_many, rox1[bi] + 3, rox2[bi] - 3,
                                      roy1[bi] + 3, roy2[bi] - 3 );
                }
            }
        }
    }                           // end "for bi"

    // Now, join them together:
    unsigned char pos_x, pos_y;
    FixedVector < char, 2 > pos;
    FixedVector < char, 2 > jpos;

    char doing = 0;

    char last_room = 0;


    for (bp = 0; bp < 2; bp++)
    {
        for (bi = 0; bi < 25; bi++)
        {
            if (bp == 0 && (!(bi % 5) || bi == 0))
                continue;

            if (bp == 1 && bi < 5)
                continue;

            switch (bp)
            {
            case 0:
                last_room = bi - 1;
                pos[0] = rox1[bi];      // - 1;
                pos[1] = roy1[bi] + random2(roy2[bi] - roy1[bi]);
                jpos[0] = rox2[last_room];      // + 1;
                jpos[1] = roy1[last_room]
                                + random2(roy2[last_room] - roy1[last_room]);
                break;

            case 1:
                last_room = bi - 5;
                pos[1] = roy1[bi];      // - 1;
                pos[0] = rox1[bi] + random2(rox2[bi] - rox1[bi]);
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
        }                       // end "for bp, for bi"
    }

    if (many_many > 8 && one_chance_in(10))
    {
        int spec_room_done = random2(25);

        x1 = rox1[spec_room_done];
        x2 = rox2[spec_room_done];
        y1 = roy1[spec_room_done];
        y2 = roy2[spec_room_done];
        morgue();

        is_a_specroom = 2;      // ????????????????????????????
        // should this be here?

        for (bcount_x = x1 - 1; bcount_x < x2; bcount_x++)
        {
            if (grd[bcount_x][y1 - 1] == DNGN_ROCK_WALL)
                grd[bcount_x][y1 - 1] = 105;

            if (grd[bcount_x][y1 - 1] == DNGN_FLOOR
                || grd[x1 - 1][bcount_y] == DNGN_CLOSED_DOOR)
            {
                grd[bcount_x][y1 - 1] = DNGN_ENTER_ZOT;
            }

            if (grd[bcount_x][y2] == DNGN_ROCK_WALL)
                grd[bcount_x][y2] = 105;

            if (grd[bcount_x][y2] == DNGN_FLOOR
                || grd[x1 - 1][bcount_y] == DNGN_CLOSED_DOOR)
            {
                grd[bcount_x][y2] = DNGN_ENTER_ZOT;
            }
        }

        for (bcount_y = y1 - 1; bcount_y < y2; bcount_y++)
        {
            if (grd[x1 - 1][bcount_y] == DNGN_ROCK_WALL)
                grd[x1 - 1][bcount_y] = 105;

            if (grd[x1 - 1][bcount_y] == DNGN_FLOOR
                || grd[x1 - 1][bcount_y] == DNGN_CLOSED_DOOR)
            {
                grd[x1 - 1][bcount_y] = DNGN_ENTER_ZOT;
            }

            if (grd[x2][bcount_y] == DNGN_ROCK_WALL)
                grd[x2][bcount_y] = 105;

            if (grd[x2][bcount_y] == DNGN_FLOOR
                || grd[x2][bcount_y] == DNGN_CLOSED_DOOR)
            {
                grd[x2][bcount_y] = DNGN_ENTER_ZOT;
            }
        }

        if (grd[x2][y2] == DNGN_ROCK_WALL)
            grd[x2][y2] = 105;
    }

    for (bj = 0; bj < (coinflip()? 4 : 3); bj++)
    {
        for (bi = 0; bi < 2; bi++)
        {
            do
            {
                pos_x = 10 + random2(GXM - 20);
                pos_y = 10 + random2(GYM - 20);
            }
            while (grd[pos_x][pos_y] != DNGN_FLOOR);

            grd[pos_x][pos_y] = bj + ((bi == 0) ? DNGN_STONE_STAIRS_DOWN_I
                                                : DNGN_STONE_STAIRS_UP_I);
        }
    }
}                               // end roguey_level()

static void city_level(void)
{
    int temp_rand;          // probability determination {dlb}
    int wall_type;          // remember, can have many wall types in one level
    int wall_type_room;     // simplifies logic of innermost loop {dlb}

    int xs = 0, ys = 0;
    int x1 = 0, x2 = 0;
    int y1 = 0, y2 = 0;
    unsigned char pos_x, pos_y;

    temp_rand = random2(8);

    wall_type = ((temp_rand > 4) ? DNGN_ROCK_WALL :     // 37.5% {dlb}
                 (temp_rand > 1) ? DNGN_STONE_WALL      // 37.5% {dlb}
                                 : DNGN_METAL_WALL);    // 25.0% {dlb}

    for (bcount_x = 7; bcount_x < (GXM - 7); bcount_x++)
    {
        for (bcount_y = 7; bcount_y < (GYM - 7); bcount_y++)
        {
            grd[bcount_x][bcount_y] = DNGN_FLOOR;
            igrd[bcount_x][bcount_y] = NON_ITEM;
        }
    }

    for (bi = 0; bi < 5; bi++)
    {
        for (bj = 0; bj < 4; bj++)
        {
            xs = 8 + (bi * 13);
            ys = 8 + (bj * 14);
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

                if (x2 - x1 > 2 && y2 - y1 > 2 && one_chance_in(8))
                    box_room(x1 + 2, x2 - 2, y1 + 2, y2 - 2, wall_type);

                if (many_many != 59 && one_chance_in(4))
                    treasure_area(many_many, x1 + 3, x2 - 3, y1 + 3, y2 - 3);
            }
        }
    }

    for (bj = 0; bj < (coinflip()? 2 : 1); bj++)
    {
        for (bi = 0; bi < 2; bi++)
        {
            do
            {
                pos_x = random2(GXM - 20) + 10;
                pos_y = random2(GYM - 20) + 10;
            }
            while (grd[pos_x][pos_y] != DNGN_FLOOR);

            grd[pos_x][pos_y] = bj + ((bi == 0) ? DNGN_STONE_STAIRS_DOWN_I
                                                : DNGN_STONE_STAIRS_UP_I);
        }
    }
}                               // end city_level()

static void box_room(int bx1, int bx2, int by1, int by2, int wall_type)
{
    // hack -- avoid lava in the crypt. {gdl}
    if ((you.where_are_you == BRANCH_CRYPT || you.where_are_you == BRANCH_TOMB)
         && wall_type == DNGN_LAVA)
        wall_type = DNGN_SHALLOW_WATER;

    int temp_rand;              // probability determination {dlb}

    for (bcount_x = bx1; bcount_x < bx2 + 1; bcount_x++)
    {
        if (grd[bcount_x][by1] == DNGN_FLOOR)
            grd[bcount_x][by1] = wall_type;
        if (grd[bcount_x][by2] == DNGN_FLOOR)
            grd[bcount_x][by2] = wall_type;
    }

    for (bcount_y = by1; bcount_y < by2; bcount_y++)
    {
        if (grd[bx1][bcount_y] == DNGN_FLOOR)
            grd[bx1][bcount_y] = wall_type;
        if (grd[bx2][bcount_y] == DNGN_FLOOR)
            grd[bx2][bcount_y] = wall_type;
    }

    switch (random2(4))         // guarantee one doorway into room {dlb}
    {
    case 0:
        grd[bx1][by1 + random2(by2 - by1 + 1)] = DNGN_CLOSED_DOOR;
        break;
    case 1:
        grd[bx2][by1 + random2(by2 - by1 + 1)] = DNGN_CLOSED_DOOR;
        break;
    case 2:
        grd[bx1 + random2(bx2 - bx1 + 1)][by1] = DNGN_CLOSED_DOOR;
        break;
    case 3:
        grd[bx1 + random2(bx2 - bx1 + 1)][by2] = DNGN_CLOSED_DOOR;
        break;
    }

    temp_rand = random2(36);

    if (temp_rand > 15)         // 55.6% probability one additional door {dlb}
    {
        if (temp_rand > 27)
            grd[bx1 + random2(by2 - by1 + 1)][by1] = DNGN_CLOSED_DOOR; // 22.2%
        else if (temp_rand > 19)
            grd[bx1 + random2(bx2 - bx1 + 1)][by2] = DNGN_CLOSED_DOOR; // 22.2%
        else if (temp_rand > 17)
            grd[bx1][by1 + random2(by2 - by1 + 1)] = DNGN_CLOSED_DOOR; //  5.6%
        else
            grd[bx2][by1 + random2(by2 - by1 + 1)] = DNGN_CLOSED_DOOR; //  5.6%
    }
    else if (temp_rand > 7)     // 22.2% probability two additional doors {dlb}
    {
        if (temp_rand > 13)
        {
            grd[bx1][by1 + random2(by2 - by1 + 1)] = DNGN_CLOSED_DOOR; //  5.6%
            grd[bx1 + random2(by2 - by1 + 1)][by1] = DNGN_CLOSED_DOOR;
        }
        else if (temp_rand > 11)
        {
            grd[bx1][by1 + random2(by2 - by1 + 1)] = DNGN_CLOSED_DOOR; //  5.6%
            grd[bx1 + random2(bx2 - bx1 + 1)][by2] = DNGN_CLOSED_DOOR;
        }
        else if (temp_rand > 9)
        {
            grd[bx2][by1 + random2(by2 - by1 + 1)] = DNGN_CLOSED_DOOR; //  5.6%
            grd[bx1 + random2(by2 - by1 + 1)][by1] = DNGN_CLOSED_DOOR;
        }
        else
        {
            grd[bx2][by1 + random2(by2 - by1 + 1)] = DNGN_CLOSED_DOOR; //  5.6%
            grd[bx1 + random2(bx2 - bx1 + 1)][by2] = DNGN_CLOSED_DOOR;
        }
    }
    // no "else" -> 22.2% probability NO additional doors {dlb}
}                               // end box_room()

static void check_doors(void)
{
    unsigned char ig;
    unsigned char solid_count = 0;      // clarifies innermost loop {dlb}

    for (bcount_x = 0; bcount_x < GXM; bcount_x++)
    {
        for (bcount_y = 0; bcount_y < GYM; bcount_y++)
        {
            ig = grd[bcount_x][bcount_y];

            if (ig != DNGN_CLOSED_DOOR && ig != 103 && ig != 33)
                continue;

            solid_count = 0;

            // first half of each conditional represents bounds checking {dlb}:
            if (bcount_x > 0
                        && grd[bcount_x - 1][bcount_y] < DNGN_LAST_SOLID_TILE)
            {
                solid_count++;
            }

            if (bcount_x < (GXM - 1)
                        && grd[bcount_x + 1][bcount_y] < DNGN_LAST_SOLID_TILE)
            {
                solid_count++;
            }

            if (bcount_y > 0
                        && grd[bcount_x][bcount_y - 1] < DNGN_LAST_SOLID_TILE)
            {
                solid_count++;
            }

            if (bcount_y < (GYM - 1)
                        && grd[bcount_x][bcount_y + 1] < DNGN_LAST_SOLID_TILE)
            {
                solid_count++;
            }

            grd[bcount_x][bcount_y] = ((solid_count < 2) ? DNGN_FLOOR
                                                         : DNGN_CLOSED_DOOR);
        }
    }
}                               // end check_doors()

static void labyrinth_level(void)
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
    for (bcount_x = 0; bcount_x < num_items; bcount_x++)
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

        treasure_item = items(1, glopop, OBJ_RANDOM, 1, many_many * 3, 250);
        mitm.x[treasure_item] = lx;
        mitm.y[treasure_item] = ly;
    }

    FixedVector < int, 2 > passed;

    passed[1] = 1;
    place_monster( 250, MONS_MINOTAUR, true, lx, ly, BEH_SLEEP, MHITNOT, false,
                      many_many, passed );

    grd[lx][ly] = DNGN_ROCK_STAIRS_UP;

    link_items();


    // turn rock walls into undiggable stone or metal:
    temp_rand = random2(50);

    unsigned char wall_xform = ((temp_rand > 10) ? DNGN_STONE_WALL   // 78.0%
                                                 : DNGN_METAL_WALL); // 22.0%

    for (bcount_x = 0; bcount_x < GXM; bcount_x++)
    {
        for (bcount_y = 0; bcount_y < GYM; bcount_y++)
        {
            if (grd[bcount_x][bcount_y] == DNGN_ROCK_WALL)
                grd[bcount_x][bcount_y] = wall_xform;
        }
    }
}                               // end labyrinth_level()

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

static void place_shops(void)
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
                                 : random2(5));  //  4.0% probability

    if (no_shops == 0 || many_many < 3)
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

        place_spec_shop(shop_place_x, shop_place_y, SHOP_RANDOM);
    }
}                               // end place_shops()

static void place_spec_shop(unsigned char shop_x, unsigned char shop_y,
                            unsigned char force_s_type)
{
    int orb = 0;
    int i = 0;
    int j = 0;                  // loop variable
    int item_level = many_many;

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

    env.shop_level[i] = many_many * 2;

    env.shop_type[i] =
        ((force_s_type != SHOP_RANDOM) ? force_s_type : random2(NUM_SHOPS));

    if (env.shop_type[i] != SHOP_WEAPON_ANTIQUE
        && env.shop_type[i] != SHOP_ARMOUR_ANTIQUE
        && env.shop_type[i] != SHOP_GENERAL_ANTIQUE)
    {
        env.shop_greed[i] = 10 + random2(5) + random2(many_many / 2);
    }
    else
        env.shop_greed[i] = 15 + random2avg(19, 2) + random2(many_many);

    int plojy = 5 + random2avg(12, 3);

    for (j = 0; j < plojy; j++)
    {
        if (env.shop_type[i] != SHOP_WEAPON_ANTIQUE
            && env.shop_type[i] != SHOP_ARMOUR_ANTIQUE
            && env.shop_type[i] != SHOP_GENERAL_ANTIQUE)
        {
            item_level = many_many + random2((many_many + 1) * 2);
        }
        else
            item_level = many_many + random2((many_many + 1) * 3);

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

static bool treasure_area(int many_many, unsigned char ta1_x,
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
                                            random2( many_many * 2 ), 250 );
            mitm.x[item_made] = x_count;
            mitm.y[item_made] = y_count;
        }
    }

    return true;
}                               // end treasure_area()

static void hide_doors(void)
{
    unsigned char dx = 0, dy = 0;     // loop variables
    unsigned char wall_count = 0;     // clarifies inner loop {dlb}

    for (dx = 0; dx < GXM; dx++)
    {
        for (dy = 0; dy < GYM; dy++)  // was "dy < 80" but that is wrong {dlb}
        {
            // only one out of three doors are candidates for hiding {dlb}:
            if (grd[dx][dy] == DNGN_CLOSED_DOOR && one_chance_in(3))
            {
                wall_count = 0;

                // first half of each conditional represents
                // bounds checking {dlb}:
                if (dx > 0 && grd[dx - 1][dy] == DNGN_ROCK_WALL)
                    wall_count++;

                if (dx < (GXM - 1) && grd[dx + 1][dy] == DNGN_ROCK_WALL)
                    wall_count++;

                if (dy > 0 && grd[dx][dy - 1] == DNGN_ROCK_WALL)
                    wall_count++;

                if (dy < (GYM - 1) && grd[dx][dy + 1] == DNGN_ROCK_WALL)
                    wall_count++;

                // if door is attached to more than one wall, hide it {dlb}:
                if (wall_count > 1)
                    grd[dx][dy] = DNGN_SECRET_DOOR;
            }
        }
    }
}                               // end hide_doors()

static void chequerboard(unsigned char cheq1, unsigned char cheq2,
                         unsigned char deleted, unsigned char ch_x1,
                         unsigned char chy1, unsigned char ch_x2,
                         unsigned char chy2)
{
    int i, j;

    if (ch_x1 >= ch_x2 || chy1 >= chy2)
        return;

    for (i = ch_x1; i < ch_x2; i++)
    {
        for (j = chy1; j < chy2; j++)
        {
            if (grd[i][j] == deleted)
                grd[i][j] = (((i + j) % 2) ? cheq2 : cheq1);
        }
    }
}                               // end chequerboard()

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

    for (bj = 0; bj < (coinflip()? 4 : 3); bj++)
    {
        for (bi = 0; bi < 2; bi++)
        {
            do
            {
                pos_x = 10 + random2(GXM - 20);
                pos_y = 10 + random2(GYM - 20);
            }
            while (grd[pos_x][pos_y] != DNGN_FLOOR);

            grd[pos_x][pos_y] = bj + ((bi == 0) ? DNGN_STONE_STAIRS_DOWN_I
                                                : DNGN_STONE_STAIRS_UP_I);
        }
    }
}                               // end bigger_room()

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

static void generate_abyss(void)
{
    int i, j;                   // loop variables
    int temp_rand;              // probability determination {dlb}

    for (i = 5; i < (GXM - 5); i++)
    {
        for (j = 5; j < (GYM - 5); j++)
        {
            temp_rand = random2(4000);

            grd[i][j] = ((temp_rand > 999) ? DNGN_FLOOR :       // 75.0%
                         (temp_rand > 400) ? DNGN_ROCK_WALL :   // 15.0%
                         (temp_rand > 100) ? DNGN_STONE_WALL :  //  7.5%
                         (temp_rand >   0) ? DNGN_METAL_WALL    //  2.5%
                                           : DNGN_CLOSED_DOOR); // 1 in 4000
        }
    }

    grd[45][35] = DNGN_FLOOR;
}                               // end generate_abyss()

static void clear_area(unsigned char x_min, unsigned char y_min,
                       unsigned char x_max, unsigned char y_max,
                       unsigned char deleting, unsigned char replacing)
{
    // first coordinates *must* be top left
    unsigned char loop_x = 0, loop_y = 0;

    for (loop_x = x_min; loop_x < x_max; loop_x++)
    {
        for (loop_y = y_min; loop_y < y_max; loop_y++)
        {
            if (grd[loop_x][loop_y] == deleting)
                grd[loop_x][loop_y] = replacing;
        }
    }
}                               // end clear_area()

static void plan_main(char force_plan)
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

    if (!force_plan)
        force_plan = 1 + random2(12);

    do_stairs = ((force_plan == 1) ? plan_1() :
                 (force_plan == 2) ? plan_2() :
                 (force_plan == 3) ? plan_3() :
                 (force_plan == 4) ? plan_4(0, 0, 0, 0, 99) :
                 (force_plan == 5) ? (one_chance_in(9) ? plan_5()
                                                       : plan_3()) :
                 (force_plan == 6) ? plan_6()
                                   : plan_3());

    if (do_stairs == 3 || do_stairs == 1)
        spotty_level(true, 0, coinflip());

    if (do_stairs == 2 || do_stairs == 3)
    {
        for (bj = 0; bj < (coinflip()? 4 : 3); bj++)
        {
            for (bi = 0; bi < 2; bi++)
            {
                do
                {
                    pos_x = 10 + random2(GXM - 20);
                    pos_y = 10 + random2(GYM - 20);
                }
                while (grd[pos_x][pos_y] != DNGN_FLOOR);
                // this could cause a hang ... should cap loop {dlb}

                grd[pos_x][pos_y] = bj + ((bi == 0) ? DNGN_STONE_STAIRS_DOWN_I
                                                    : DNGN_STONE_STAIRS_UP_I);
            }
        }
    }

    if (one_chance_in(20))
    {
        for (bi = 0; bi < GXM; bi++)
        {
            for (bj = 0; bj < GYM; bj++)
            {
                if (grd[bi][bj] == DNGN_ROCK_WALL)
                    grd[bi][bj] = special_grid;
            }
        }
    }
}                               // end plan_main()

static unsigned char plan_1(void)
{
    int temp_rand = 0;          // probability determination {dlb}

    unsigned char width = (10 - random2(7));    // value range of [4,10] {dlb}

    clear_area(10, 10, (GXM - 10), (10 + width), DNGN_ROCK_WALL, DNGN_FLOOR);

    clear_area(10, (60 - width), (GXM - 10), (GYM - 10),
                                                DNGN_ROCK_WALL, DNGN_FLOOR);

    clear_area(10, 10, (10 + width), (GYM - 10), DNGN_ROCK_WALL, DNGN_FLOOR);
    clear_area((60 - width), 10, (GXM - 10), (GYM - 10),
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

        clear_area(10, (35 - width2), (GXM - 10), (35 + width2),
                   DNGN_ROCK_WALL, DNGN_FLOOR);
        clear_area((40 - width2), 10, (40 + width2), (GYM - 10),
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
        room_x1 = 25;
        room_y1 = 25;
        room_x2 = (GXM - 25);
        room_y2 = (GYM - 25);

        if (coinflip())
            oblique_max = 5 + random2(20);

        temp_rand = random2(15);

        if (temp_rand > 7)      // 7 in 15 odds {dlb}
        {
            temp_rand = random2(7);

            unsigned char floor_type = ((temp_rand > 1) ? DNGN_FLOOR :   // 5/7
                                        (temp_rand > 0) ? DNGN_DEEP_WATER// 1/7
                                                        : DNGN_LAVA);    // 1/7
            octa_room(floor_type);
        }
    }

    // final return {dlb}:
    return (one_chance_in(5) ? 3 : 2);
}                               // end plan_1()

// just a cross:
static unsigned char plan_2(void)
{
    char width2 = (5 - random2(5));     // value range of [1,5] {dlb}

    clear_area(10, (35 - width2), (GXM - 10), (35 + width2),
                                            DNGN_ROCK_WALL, DNGN_FLOOR);
    clear_area((40 - width2), 10, (40 + width2), (GYM - 10),
                                            DNGN_ROCK_WALL, DNGN_FLOOR);

    return (one_chance_in(4) ? 2 : 3);
}                               // end plan_2()

static char plan_3(void)
{

    /* Draws a room, then another and links them together, then another and etc
       Of course, this can easily end up looking just like a make_trail level.
     */
    char cnx, cny;
    char roomsss = 30 + random2(90);

    bool exclusive = (one_chance_in(10) ? false : true);
    bool exclusive2 = coinflip();

    char romx1[30], romy1[30], romx2[30], romy2[30];

    char which_room = 0;

    for (bi = 0; bi < roomsss; bi++)
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

        clear_area(romx1[which_room], romy1[which_room], romx2[which_room],
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
        for (bi = 0; bi < which_room; bi++)
        {
            if (bi > 0)
            {
                const int rx1 = romx1[bi];
                const int rx2 = romx2[bi];
                const int prev_rx1 = romx1[bi - 1];
                const int prev_rx2 = romx2[bi - 1];

                const int ry1 = romy1[bi];
                const int ry2 = romy2[bi];
                const int prev_ry1 = romy1[bi - 1];
                const int prev_ry2 = romy2[bi - 1];

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

static unsigned char plan_4(char forbid_x1, char forbid_y1, char forbid_x2,
                            char forbid_y2, unsigned char force_wall)
{
    // a more chaotic version of city level
    int temp_rand;              // req'd for probability checking

    int number_boxes = 5000;
    unsigned char drawing = DNGN_ROCK_WALL;
    char b1x, b1y, b2x, b2y;
    char cnx, cny;

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

    clear_area(10, 10, (GXM - 10), (GYM - 10), DNGN_ROCK_WALL, DNGN_FLOOR);

    // clear_area can also be used to fill in:
    for (bi = 0; bi < number_boxes; bi++)
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
            clear_area(b1x, b1y, b2x, b2y, DNGN_FLOOR, drawing);
        else                    // odds:  72 in 210 {dlb}
            box_room(b1x, b2x - 1, b1y, b2y - 1, drawing);

      continuing:
        continue;
    }

    if (forbid_x1 == 0 && one_chance_in(4))     // a market square
    {
        room_x1 = 25;
        room_y1 = 25;
        room_x2 = 55;
        room_y2 = 45;

        if (!one_chance_in(4))
            oblique_max = 5 + random2(20);      // used elsewhere {dlb}

        if (!one_chance_in(10))
            octa_room(DNGN_FLOOR);
        else
            octa_room((coinflip()? DNGN_DEEP_WATER : DNGN_LAVA));
    }

    return 2;
}                               // end plan_4()

static unsigned char plan_5(void)
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

static char plan_6(void)
{
    // circle of standing stones (well, kind of)
    room_x1 = 10;
    room_x2 = (GXM - 10);
    room_y1 = 10;
    room_y2 = (GYM - 10);

    oblique_max = 14;           //5 + random2(10);

    octa_room(DNGN_FLOOR);

    clear_area(23, 23, 26, 26, DNGN_FLOOR, DNGN_STONE_WALL);
    clear_area(23, 47, 26, 50, DNGN_FLOOR, DNGN_STONE_WALL);
    clear_area(55, 23, 58, 26, DNGN_FLOOR, DNGN_STONE_WALL);
    clear_area(55, 47, 58, 50, DNGN_FLOOR, DNGN_STONE_WALL);
    clear_area(39, 20, 43, 23, DNGN_FLOOR, DNGN_STONE_WALL);
    clear_area(39, 50, 43, 53, DNGN_FLOOR, DNGN_STONE_WALL);
    clear_area(20, 30, 23, 33, DNGN_FLOOR, DNGN_STONE_WALL);
    clear_area(20, 40, 23, 43, DNGN_FLOOR, DNGN_STONE_WALL);
    clear_area(58, 30, 61, 33, DNGN_FLOOR, DNGN_STONE_WALL);
    clear_area(58, 40, 61, 43, DNGN_FLOOR, DNGN_STONE_WALL);

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

    if (many_many > 20 && coinflip())
    {
        grd[40][36] = DNGN_ENTER_ABYSS; // remember pandemonium
        grd[41][36] = DNGN_ENTER_ABYSS;
    }

    return 0;
}                               // end plan_6()

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

static void build_vaults(int force_vault)
{
    // for some weird reason can't put a vault on level 1, because monster equip
    // isn't generated.
    int altar_count = 0;
    FixedVector < char, 10 > stair_exist;
    char stx, sty;

    FixedVector < char, 7 > acq_item_class(OBJ_WEAPONS,
                                           OBJ_ARMOUR,
                                           OBJ_WEAPONS,
                                           OBJ_JEWELLERY,
                                           OBJ_BOOKS,
                                           OBJ_STAVES,
                                           OBJ_MISCELLANY);

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

    char gluggy = vault_main(vgrid, mons_array, force_vault, many_many);

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
            altar_count = vault_grid(vx, vy, altar_count, acq_item_class,
                                                    mons_array, vgrid[vy][vx]);
        }
    }

    switch (gluggy)
    {
    case MAP_NORTH:
        v1x = 1;
        v2x = GXM;
        v1y = 1;
        v2y = 35;
        vsy++;
        dig_dir_x = 0;
        dig_dir_y = 1;
        break;

    case MAP_NORTHWEST:
        v1x = 1;
        v2x = 40;
        v1y = 1;
        v2y = 35;
        vsy++;
        dig_dir_x = 1;
        dig_dir_y = 0;
        break;

    case MAP_NORTHEAST:
        v1x = 40;
        v2x = GXM;
        v1y = 1;
        v2y = 35;
        vsy++;
        dig_dir_x = -1;
        dig_dir_y = 0;
        break;

    case MAP_SOUTHWEST:
        v1x = 1;
        v2x = 40;
        v1y = 35;
        v2y = GYM;
        vsy--;
        dig_dir_x = 0;
        dig_dir_y = -1;
        break;

    case MAP_SOUTHEAST:
        v1x = 40;
        v2x = GXM;
        v1y = 35;
        v2y = GYM;
        vsy--;
        dig_dir_x = 0;
        dig_dir_y = -1;
        break;

    case MAP_ENCOMPASS:
        goto full_screen;

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

    for (bi = 0; bi < roomsss; bi++)
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

        if (bi == 0)
        {
            join_the_dots(vsx, vsy, romx1[which_room], romy1[which_room],
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

        clear_area(romx1[which_room], romy1[which_room], romx2[which_room],
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
        continue;               // next bi loop

    }

  vstair:
    dig_place_x = vsx;
    dig_place_y = vsy;

    if (gluggy != MAP_NORTH_DIS)
    {
        for (bi = 0; bi < 40; bi++)
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

    for (bj = 0; bj < (coinflip()? 4 : 3); bj++)
    {
        for (bi = 0; bi < 2; bi++)
        {

            if (stair_exist[(82 + bj + (bi * 4)) - 82] == 1)   // does this look funny to *you*? {dlb}
                continue;

            do
            {
                pos_x = 10 + random2(GXM - 20);
                pos_y = 10 + random2(GYM - 20);
            }
            while (grd[pos_x][pos_y] != DNGN_FLOOR
                   || (pos_x >= v1x && pos_x <= v2x && pos_y >= v1y
                       && pos_y <= v2y));

            grd[pos_x][pos_y] = bj + ((bi == 0) ? DNGN_STONE_STAIRS_DOWN_I
                                      : DNGN_STONE_STAIRS_UP_I);
        }
    }

  full_screen:
    return;
}                               // end build_vaults()

static void build_minivaults(int force_vault)
{
    // for some weird reason can't put a vault on level 1, because monster equip
    // isn't generated.
    int altar_count = 0;

    FixedVector < char, 7 > acq_item_class(OBJ_WEAPONS,
                                           OBJ_ARMOUR,
                                           OBJ_WEAPONS,
                                           OBJ_JEWELLERY,
                                           OBJ_BOOKS,
                                           OBJ_STAVES,
                                           OBJ_MISCELLANY);

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

    // was "+ random2(34)" but that would attempt to generate an
    // undefined vault {dlb}

    vault_main(vgrid, mons_array, force_vault, many_many);

    int vx, vy;
    int v1x, v1y;

    /* find a target area which can be safely overwritten: */
    do
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
    while (1);

    for (vx = v1x; vx < v1x + 12; vx++)
    {
        for (vy = v1y; vy < v1y + 12; vy++)
        {
            grd[vx][vy] = vgrid[vx - v1x][vy - v1y];
        }
    }

    /* note: assumes *no* previous item (I think) or monster
       (definitely) placement */
    for (vx = v1x; vx < v1x + 12; vx++)
    {
        for (vy = v1y; vy < v1y + 12; vy++)
        {
            altar_count = vault_grid(vx, vy, altar_count, acq_item_class,
                                                    mons_array, grd[vx][vy]);
        }
    }
}                               // end build_minivaults()

// returns altar_count - seems rather odd to me to force such a return
// when I believe the value is only used in the case of the ecumenical
// temple - oh, well... {dlb}
static int vault_grid(int vx, int vy, int altar_count, FixedVector < char,
                      7 > &acq_item_class, FixedVector < int, 7 > &mons_array,
                      char vgrid)
{
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
        vsx = vx;
        vsy = vy;
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
                           (vgrid == '*')        ? 5 + (many_many * 2)
                                                 : many_many);

            item_made = items(1, which_class, which_type, 1, which_depth, 250);

            mitm.x[item_made] = vx;
            mitm.y[item_made] = vy;
        }
        break;
    }

    // finally, handle grids that place monsters {dlb}:
    if (vgrid >= '0' && vgrid <= '9')
    {
        FixedVector < int, 2 > passed;
        int monster_level;
        int monster_type_thing;

        passed[1] = 1;

        monster_level = ((vgrid == '8') ? (4 + (many_many * 2)) :
                         (vgrid == '9') ? (5 + many_many) : many_many);

        if (monster_level > 30) // very high level monsters more common here
            monster_level = 30;

        monster_type_thing = ((vgrid == '8'
                               || vgrid == '9'
                               || vgrid == '0') ? RANDOM_MONSTER
                                                : mons_array[(vgrid - '1')]);

        place_monster(250, monster_type_thing, true, vx, vy, BEH_SLEEP,
                      MHITNOT, false, monster_level, passed);
    }

    // again, this seems odd, given that this is just one of many
    // vault types {dlb}
    return altar_count;
}                               // end vault_grid()

static void place_curse_skull(void)
{
    FixedVector < int, 2 > passed;
    int px, py;
    int i, j;
    int k = 0;
    int l = 0;

    do
    {
      rand_px:
        px = 15 + random2(55);
        py = 15 + random2(45);
        k++;

        if (k == 5000)
            return;

        l = 0;

        for (i = px - 4; i < px + 4; i++)
        {
            for (j = py - 4; j < py + 5; j++)
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

        for (i = px - 4; i < px + 4; i++)
        {
            for (j = py - 4; j < py + 5; j++)
            {
                grd[i][j] = DNGN_FLOOR;
            }
        }

        passed[1] = 1;
        place_monster(250, MONS_CURSE_SKULL, true, px, py, BEH_SLEEP, MHITNOT,
                      false, many_many, passed);

        return;
    }
    while (1);
}                               // end place_curse_skull()

static void place_altar(void)
{
    int px, py;
    int i, j;
    int k = 0, l = 0;
    int altar_type = pick_an_altar();

    do
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
    while (1);
}                               // end place_altar()

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

//jmf: generate altar based on where you are (or randomly, as old code)
static int pick_an_altar(void)
{

    int altar_type = 0;
    int temp_rand;              // probability determination {dlb}

#ifdef USE_NEW_ALTAR_CODE
    switch (you.where_are_you)
    {
    case BRANCH_CRYPT:
        altar_type = (coinflip() ? DNGN_ALTAR_KIKUBAAQUDGHA
                                 : DNGN_ALTAR_YREDELEMNUL);
        break;

    case BRANCH_ORCISH_MINES:
        temp_rand = random2(5);

        altar_type = ((temp_rand == 0) ? DNGN_ALTAR_VEHUMET :
                      (temp_rand == 1) ? DNGN_ALTAR_MAKHLEB :
                      (temp_rand == 2) ? DNGN_ALTAR_OKAWARU :
                      (temp_rand == 3) ? DNGN_ALTAR_TROG
                                       : DNGN_ALTAR_XOM);
        break;

    case BRANCH_SLIME_PITS:
        altar_type = DNGN_FLOOR;        //jmf: no altars in Slime Pits
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

#else
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
#endif

    return altar_type;
}                               // end pick_an_altar()

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
            for (sx = 1; bi < GXM; sx++)
                for (sy = 1; bj < GYM; sy++)
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



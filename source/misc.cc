/*
 *  File:       misc.cc
 *  Summary:    Misc functions.
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 *   <3>   11/14/99      cdl    evade with random40(ev) vice random2(ev)
 *   <2>    5/20/99      BWR    Multi-user support, new berserk code.
 *   <1>    -/--/--      LRH    Created
 */

#include "AppHdr.h"
#include "misc.h"

#include <string.h>
#if !(defined(__IBMCPP__) || defined(__BCPLUSPLUS__))
#include <unistd.h>
#endif
#include <stdlib.h>
#include <stdio.h>

#ifdef DOS
#include <conio.h>
#endif

#include "externs.h"

#include "fight.h"
#include "files.h"
#include "food.h"
#include "it_use2.h"
#include "lev-pand.h"
#include "monplace.h"
#include "mon-util.h"
#include "monstuff.h"
#include "ouch.h"
#include "player.h"
#include "shopping.h"
#include "skills.h"
#include "skills2.h"
#include "spells.h"
#include "spells3.h"
#include "stuff.h"
#include "transfor.h"
#include "view.h"

#ifdef MACROS
#include "macro.h"
#endif

bool scramble(void);
bool trap_item(char base_type, char sub_type, char beam_x, char beam_y);
static void dart_trap(bool trap_known, int trapped, struct bolt &pbolt);

void destroy_item(int dest)
{

    int c = 0;
    int cy = 0;

    if (dest == NON_ITEM)
        return;

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
                mitm.quantity[dest] = 0;
                mitm.link[dest] = NON_ITEM;

                if (monster->type == MONS_DANCING_WEAPON)
                    monster_die(monster, KILL_RESET, 0);

                return;
            }
        }
    }

    unsigned int udest = dest;

    for (c = 0; c < MAX_ITEMS; c++)
    {
        if (mitm.link[c] == udest)
        {
            mitm.link[c] = mitm.link[dest];
            mitm.quantity[dest] = 0;
            mitm.link[dest] = NON_ITEM;
            return;
        }
    }

    // Okay, so that didn't work. Now we go for the slower way:
    for (c = 2; c < (GXM - 1); c++)
    {
        for (cy = 2; cy < (GYM - 1); cy++)
        {
            if (igrd[c][cy] == dest)
            {
                igrd[c][cy] = mitm.link[dest];
                mitm.quantity[dest] = 0;
                mitm.link[dest] = NON_ITEM;
                return;
            }
        }
    }
}                               // end destroy_item()


void place_chunks(int mcls, unsigned char rot_status, unsigned char chx,
                  unsigned char chy, unsigned char ch_col)
{
    const int max_chunks = mons_weight( mcls ) / 150;

    int o;
    for (o = 0; o < MAX_ITEMS; o++)
    {
        if (o >= 200 + random2(150))
            return;

        if (mitm.quantity[o] == 0)
        {
            mitm.id[o] = 0;
            mitm.base_type[o] = OBJ_FOOD;
            mitm.pluses[o] = mcls;
            mitm.pluses2[o] = 0;
            mitm.sub_type[o] = 21;
            mitm.special[o] = rot_status;
            if (ch_col == BLACK)
                ch_col = LIGHTRED;
            mitm.colour[o] = ch_col;
            mitm.quantity[o] = 1 + random2( max_chunks );
            mitm.link[o] = NON_ITEM;
            break;
        }
    }                           // end of o loop

    if (igrd[chx][chy] == NON_ITEM)
    {
        igrd[chx][chy] = o;
    }
    else
    {
        int hug = igrd[chx][chy];

        igrd[chx][chy] = o;
        mitm.link[o] = hug;
    }

    // One day I'll add code for various monster organs.

    // seems to me that this should come about only
    // after the corpse has been butchered ... {dlb}
    if (monster_descriptor(mcls, MDSC_LEAVES_HIDE) && !one_chance_in(3))
    {
        for (o = 0; o < MAX_ITEMS; o++)
        {
            // should be a way to place this outside loop for efficiency
            // reasons {dlb}
            if (o >= 200 + random2(150))
                return;

            if (mitm.quantity[o] == 0)
            {
                mitm.quantity[o] = 1;
                mitm.link[o] = NON_ITEM;

                // these values are common to all: {dlb}
                mitm.id[o] = 0;
                mitm.base_type[o] = OBJ_ARMOUR;
                mitm.pluses[o] = 50;
                mitm.pluses2[o] = 0;
                mitm.special[o] = 0;
                mitm.colour[o] = mons_colour(mcls);

                // these values cannot be set by a reasonable formula: {dlb}
                switch (mcls)
                {
                case MONS_DRAGON:
                    mitm.sub_type[o] = ARM_DRAGON_HIDE;
                    break;
                case MONS_TROLL:
                    mitm.sub_type[o] = ARM_TROLL_HIDE;
                    break;
                case MONS_ICE_DRAGON:
                    mitm.sub_type[o] = ARM_ICE_DRAGON_HIDE;
                    break;
                case MONS_STEAM_DRAGON:
                    mitm.sub_type[o] = ARM_STEAM_DRAGON_HIDE;
                    break;
                case MONS_MOTTLED_DRAGON:
                    mitm.sub_type[o] = ARM_MOTTLED_DRAGON_HIDE;
                    break;
                case MONS_STORM_DRAGON:
                    mitm.sub_type[o] = ARM_STORM_DRAGON_HIDE;
                    break;
                case MONS_GOLDEN_DRAGON:
                    mitm.sub_type[o] = ARM_GOLD_DRAGON_HIDE;
                    break;
                case MONS_SWAMP_DRAGON:
                    mitm.sub_type[o] = ARM_SWAMP_DRAGON_HIDE;
                    break;
                default:
                    // future implementation {dlb}
                    mitm.sub_type[o] = ARM_ANIMAL_SKIN;
                    break;
                }
                break;          // out of the for-loop {dlb}
            }
        }

        if (igrd[chx][chy] == NON_ITEM)
        {
            igrd[chx][chy] = o;
        }
        else
        {
            int hug = igrd[chx][chy];

            igrd[chx][chy] = o;
            mitm.link[o] = hug;
        }
    }
}                               // end place_chunks()

char search_around(void)
{
    char srx = 0;
    char sry = 0;
    int i;

    for (srx = you.x_pos - 1; srx < you.x_pos + 2; srx++)
    {
        for (sry = you.y_pos - 1; sry < you.y_pos + 2; sry++)
        {
            // don't exclude own square; may be levitating
            if (grd[srx][sry] == DNGN_SECRET_DOOR
                && random2(17) <= 1 + you.skills[SK_TRAPS_DOORS])
            {
                grd[srx][sry] = DNGN_CLOSED_DOOR;
                mpr("You found a secret door!");
                exercise(SK_TRAPS_DOORS, ((coinflip())? 2 : 1));
            }

            if (grd[srx][sry] == DNGN_UNDISCOVERED_TRAP
                && random2(17) <= 1 + you.skills[SK_TRAPS_DOORS])
            {
                i = trap_at_xy(srx, sry);

                grd[srx][sry] = trap_category(env.trap_type[i]);

                mpr("You found a trap!");
            }
        }
    }

    you.turn_is_over = 1;
    return 1;
}                               // end search_around()

void in_a_cloud(void)
{
    int cl = env.cgrid[you.x_pos][you.y_pos];
    int hurted = 0;

    switch (env.cloud_type[cl])
    {
    case CLOUD_FIRE:
    case CLOUD_FIRE_MON:
        mpr("You are engulfed in roaring flames!");
        if (player_res_fire() <= 100)
        {
            hurted += ((random2avg(23, 3) + 10) * you.time_taken) / 10;
            if (player_res_fire() < 100)
                hurted += ((random2avg(14, 2) + 3) * you.time_taken) / 10;

            hurted -= random2(player_AC());
            if (hurted < 1)
                hurted = 0;
            else
                ouch(hurted, cl, KILLED_BY_CLOUD);
        }
        else
        {
            canned_msg(MSG_YOU_RESIST);
            hurted += ((random2avg(23, 3) + 10) * you.time_taken) / 10;
            hurted /= (1 + (player_res_fire() - 100) * (player_res_fire() - 100));
            ouch(hurted, cl, KILLED_BY_CLOUD);
        }
        scrolls_burn(7, OBJ_SCROLLS);
        break;

    case CLOUD_STINK:
    case CLOUD_STINK_MON:
        // If you don't have to breathe, unaffected
        mpr("You are engulfed in noxious fumes!");
        if (player_res_poison())
            break;

        hurted += (random2(3) * you.time_taken) / 10;
        if (hurted < 1)
            hurted = 0;
        else
            ouch((hurted * you.time_taken) / 10, cl, KILLED_BY_CLOUD);

        if (1 + random2(27) >= you.experience_level)
        {
            mpr("You choke on the stench!");
            you.conf += (coinflip()? 3 : 2);
        }
        break;

    case CLOUD_COLD:
    case CLOUD_COLD_MON:
        mpr("You are engulfed in freezing vapours!");
        if (player_res_cold() <= 100)
        {
            hurted += ((random2avg(23, 3) + 10) * you.time_taken) / 10;
            if (player_res_cold() < 100)
                hurted += ((random2avg(14, 2) + 3) * you.time_taken) / 10;

            hurted -= random2(player_AC());
            if (hurted < 0)
                hurted = 0;

            ouch((hurted * you.time_taken) / 10, cl, KILLED_BY_CLOUD);
        }
        else
        {
            canned_msg(MSG_YOU_RESIST);
            hurted += ((random2avg(23, 3) + 10) * you.time_taken) / 10;
            hurted /= (1 + (player_res_cold() - 100) * (player_res_cold() - 100));
            ouch(hurted, cl, KILLED_BY_CLOUD);
        }
        scrolls_burn(7, OBJ_POTIONS);
        break;

    case CLOUD_POISON:
    case CLOUD_POISON_MON:
        // If you don't have to breathe, unaffected
        mpr("You are engulfed in poison gas!");
        if (!player_res_poison())
        {
            ouch((random2(10) * you.time_taken) / 10, cl, KILLED_BY_CLOUD);
            you.poison++;
        }
        break;

    case CLOUD_GREY_SMOKE:
    case CLOUD_BLUE_SMOKE:
    case CLOUD_PURP_SMOKE:
    case CLOUD_BLACK_SMOKE:
    case CLOUD_GREY_SMOKE_MON:
    case CLOUD_BLUE_SMOKE_MON:
    case CLOUD_PURP_SMOKE_MON:
    case CLOUD_BLACK_SMOKE_MON:
        mpr("You are engulfed in a cloud of smoke!");
        break;

    case CLOUD_STEAM:
    case CLOUD_STEAM_MON:
        mpr("You are engulfed in a cloud of scalding steam!");
        if (you.species == SP_PALE_DRACONIAN && you.experience_level > 5)
        {
            mpr("It doesn't seem to affect you.");
            return;
        }

        if (you.equip[EQ_BODY_ARMOUR] != -1
            && you.inv_type[you.equip[EQ_BODY_ARMOUR]] ==
            ARM_STEAM_DRAGON_ARMOUR)
        {
            mpr("It doesn't seem to affect you.");
            return;
        }

        hurted += (random2(6) * you.time_taken) / 10;
        if (hurted < 0 || player_res_fire() > 100)
            hurted = 0;

        ouch((hurted * you.time_taken) / 10, cl, KILLED_BY_CLOUD);
        break;

    case CLOUD_MIASMA:
    case CLOUD_MIASMA_MON:
        mpr("You are engulfed in a dark miasma.");

        if (player_prot_life() > random2(3))
            return;

        //beam_colour = 4;

        if (!player_res_poison())
            you.poison++;

        hurted += (random2avg(12, 3) * you.time_taken) / 10;    // 3

        if (hurted < 0)
            hurted = 0;

        ouch(hurted, cl, KILLED_BY_CLOUD);
        potion_effect(POT_SLOWING, 5);

        if (you.hp_max > 4 && coinflip())
        {
            you.base_hp--;
            calc_hp();
        }
        break;
    }

    return;
}                               // end in_a_cloud()


void merfolk_start_swimming(void)
{
    FixedVector < char, 8 > removed;

    if (you.attribute[ATTR_TRANSFORMATION] != TRAN_NONE)
        untransform();

    for (int i = EQ_WEAPON; i < EQ_RIGHT_RING; i++)
    {
        removed[i] = 0;
    }

    if (you.equip[EQ_BOOTS] != -1)
        removed[EQ_BOOTS] = 1;

    // Perhaps a bit to easy for the player, but we allow merfolk
    // to slide out of heavy body armour freely when entering water,
    // rather than handling emcumbered swimming. -- bwr
    if (!player_light_armour())
    {
        // Can't slide out of just the body armour, cloak comes off -- bwr
        if (you.equip[EQ_CLOAK])
            removed[EQ_CLOAK] = 1;

        removed[EQ_BODY_ARMOUR] = 1;
    }

    remove_equipment(removed);
}

void up_stairs(void)
{
    unsigned char stair_find = grd[you.x_pos][you.y_pos];
    char old_level_where = you.where_are_you;
    bool was_a_labyrinth = false;

    if (stair_find == DNGN_ENTER_SHOP)
    {
        shop();
        return;
    }

    if ((stair_find < DNGN_STONE_STAIRS_UP_I
            || stair_find > DNGN_ROCK_STAIRS_UP)
        && (stair_find < DNGN_RETURN_DUNGEON_I || stair_find > 150))
    {
        mpr("You can't go up here.");
        return;
    }

    if (you.burden_state == BS_OVERLOADED)
    {
        mpr("You are carrying too much to climb upwards.");
        you.turn_is_over = 1;
        return;
    }

    if (you.your_level == 0
            && !yesno("Are you sure you want to leave the Dungeon?", false))
    {
        mpr("Alright, then stay!");
        return;
    }

    unsigned char old_level = you.your_level;

    // Make sure we return to our main dungeon level... labyrinth entrances
    // in the abyss or pandemonium a bit trouble (well the labyrinth does
    // provide a way out of those places, its really not that bad I suppose)
    if (you.level_type == LEVEL_LABYRINTH)
    {
        you.level_type = LEVEL_DUNGEON;
        was_a_labyrinth = true;
    }

    you.your_level--;

    int i = 0;

    if (you.your_level == -1)
    {
        mpr("You have escaped!");

        for (i = 0; i < ENDOFPACK; i++)
        {
            if (you.inv_quantity[i] > 0 && you.inv_class[i] == OBJ_ORBS)
                ouch(-9999, 0, KILLED_BY_WINNING);
        }

        ouch(-9999, 0, KILLED_BY_LEAVING);
    }

    mpr("Entering...");
    you.prev_targ = MHITNOT;
    you.pet_target = MHITNOT;

    if (you.where_are_you == BRANCH_VESTIBULE_OF_HELL)
    {
        mpr("Thank you for visiting Hell. Please come again soon.");
        you.where_are_you = BRANCH_MAIN_DUNGEON;
        stair_find = DNGN_STONE_STAIRS_UP_I;
    }

    if (you.where_are_you > BRANCH_MAIN_DUNGEON
        && you.where_are_you < BRANCH_ORCISH_MINES
        && you.where_are_you != BRANCH_VESTIBULE_OF_HELL)
    {
        you.where_are_you = BRANCH_VESTIBULE_OF_HELL;
        you.your_level = 27;
    }

    switch (stair_find)
    {
    case DNGN_RETURN_DUNGEON_I:
    case DNGN_RETURN_DUNGEON_II:
    case DNGN_RETURN_DUNGEON_III:
    case DNGN_RETURN_DUNGEON_IV:
    case DNGN_EXIT_ZOT:
    case DNGN_RETURN_DUNGEON_V:
        mpr("Welcome back to the Dungeon!");
        you.where_are_you = BRANCH_MAIN_DUNGEON;
        break;
    case DNGN_RETURN_LAIR_II:
    case DNGN_RETURN_LAIR_III:
    case DNGN_RETURN_LAIR_IV:
        mpr("Welcome back to the Lair of Beasts!");
        you.where_are_you = BRANCH_LAIR;
        break;
    case DNGN_RETURN_VAULTS_II:
    case DNGN_RETURN_VAULTS_III:
        mpr("Welcome back to the Vaults!");
        you.where_are_you = BRANCH_VAULTS;
        break;
    case DNGN_RETURN_CRYPT:
        mpr("Welcome back to the Crypt!");
        you.where_are_you = BRANCH_CRYPT;
        break;
    case DNGN_RETURN_MINES:
        mpr("Welcome back to the Orcish Mines!");
        you.where_are_you = BRANCH_ORCISH_MINES;
        break;
    }

    unsigned char stair_taken = stair_find;
    bool moving_level = true;
    bool want_followers = true;

    if (you.where_are_you == BRANCH_VESTIBULE_OF_HELL)
        you.your_level = you.hell_exit;

    load( stair_taken, moving_level, was_a_labyrinth, old_level,
        want_followers, false, old_level_where );

    moving_level = false;

    new_level();

    if (you.levitation)
    {
        if (you.duration[DUR_CONTROLLED_FLIGHT])
            mpr("You fly upwards.");
        else
            mpr("You float upwards... And bob straight up to the ceiling!");
    }
    else
        mpr("You climb upwards.");

    save_game(false);
}                               // end up_stairs()

void down_stairs(bool remove_stairs, int old_level)
{
    int i;
    char old_level_type = you.level_type;
    bool was_a_labyrinth = false;
    const unsigned char stair_find = grd[you.x_pos][you.y_pos];

    //int old_level = you.your_level;
    bool leave_abyss_pan = false;
    char old_where = you.where_are_you;

#ifdef SHUT_LABYRINTH
    if (stair_find == DNGN_ENTER_LABYRINTH)
    {
        mpr("Sorry, this section of the dungeon is closed for fumigation.");
        mpr("Try again next release.");
        return;
    }
#endif

    if ((stair_find < DNGN_ENTER_LABYRINTH
            || stair_find > DNGN_ROCK_STAIRS_DOWN)
        && stair_find != DNGN_ENTER_HELL
        && ((stair_find < DNGN_ENTER_DIS
                || stair_find > DNGN_TRANSIT_PANDEMONIUM)
            && stair_find != DNGN_STONE_ARCH)
        && !(stair_find >= DNGN_ENTER_ORCISH_MINES
            && stair_find < DNGN_RETURN_DUNGEON_I))
    {
        strcpy(info, "You can't go down here!");
        mpr(info);
        return;
    }

    if (stair_find >= DNGN_ENTER_LABYRINTH
        && stair_find <= DNGN_ROCK_STAIRS_DOWN
        && you.where_are_you == BRANCH_VESTIBULE_OF_HELL)
    {
        mpr("A mysterious force prevents you from descending the staircase.");
        return;
    }                           /* down stairs in vestibule are one-way */

    if (stair_find == DNGN_STONE_ARCH)
    {
        mpr("You can't go down here!");
        return;
    }

    if (you.levitation && !wearing_amulet(AMU_CONTROLLED_FLIGHT))
    {
        mpr("You're floating high up above the floor!");
        return;
    }

    if (stair_find == DNGN_ENTER_ZOT)
    {
        int num_runes = 0;

        for (i = 0; i < ENDOFPACK; i++)
        {
            if (you.inv_quantity[i] > 0
                && you.inv_class[i] == OBJ_MISCELLANY
                && you.inv_type[i] == MISC_RUNE_OF_ZOT)
            {
                num_runes += you.inv_quantity[i];
            }
        }

        if (num_runes < NUMBER_OF_RUNES_NEEDED)
        {
            switch (NUMBER_OF_RUNES_NEEDED)
            {
            case 1:
                mpr("You need a Rune to enter this place.");
                break;
            default:
                sprintf(info, "You need at least %d Runes to enter this place.",
                        NUMBER_OF_RUNES_NEEDED);
                mpr(info);
            }
            return;
        }
    }

    if (you.level_type == LEVEL_PANDEMONIUM
            && stair_find == DNGN_TRANSIT_PANDEMONIUM)
    {
        was_a_labyrinth = true;
    }
    else
    {
        if (you.level_type != LEVEL_DUNGEON)
            was_a_labyrinth = true;

        you.level_type = LEVEL_DUNGEON;
    }

    mpr("Entering...");
    you.prev_targ = MHITNOT;
    you.pet_target = MHITNOT;

    if (stair_find == DNGN_ENTER_HELL)
    {
        you.where_are_you = BRANCH_VESTIBULE_OF_HELL;
        you.hell_exit = you.your_level;

        mpr("Welcome to Hell!");
        mpr("Please enjoy your stay.");

        more();

        you.your_level = 26;    // = 59;
    }

    if ((stair_find >= DNGN_ENTER_DIS && stair_find <= DNGN_ENTER_TARTARUS)
        || (stair_find >= DNGN_ENTER_ORCISH_MINES && stair_find <= 150))
    {
        // no idea why such a huge switch and not 100-grd[][]
        // planning ahead for re-organizaing grd[][] values - 13jan2000 {dlb}
        strcpy( info, "Welcome to " );
        switch (stair_find)
        {
        case DNGN_ENTER_DIS:
            strcat(info, "the Iron City of Dis!");
            you.where_are_you = BRANCH_DIS;
            you.your_level = 26;
            break;
        case DNGN_ENTER_GEHENNA:
            strcat(info, "Gehenna!");
            you.where_are_you = BRANCH_GEHENNA;
            you.your_level = 26;
            break;
        case DNGN_ENTER_COCYTUS:
            strcat(info, "Cocytus!");
            you.where_are_you = BRANCH_COCYTUS;
            you.your_level = 26;
            break;
        case DNGN_ENTER_TARTARUS:
            strcat(info, "Tartarus!");
            you.where_are_you = BRANCH_TARTARUS;
            you.your_level = 26;
            break;
        case DNGN_ENTER_ORCISH_MINES:
            strcat(info, "the Orcish Mines!");
            you.where_are_you = BRANCH_ORCISH_MINES;
            break;
        case DNGN_ENTER_HIVE:
            strcpy(info, "You hear a buzzing sound coming from all directions.");
            you.where_are_you = BRANCH_HIVE;
            break;
        case DNGN_ENTER_LAIR:
            strcat(info, "the Lair of Beasts!");
            you.where_are_you = BRANCH_LAIR;
            break;
        case DNGN_ENTER_SLIME_PITS:
            strcat(info, "the Pits of Slime!");
            you.where_are_you = BRANCH_SLIME_PITS;
            break;
        case DNGN_ENTER_VAULTS:
            strcat(info, "the Vaults!");
            you.where_are_you = BRANCH_VAULTS;
            break;
        case DNGN_ENTER_CRYPT:
            strcat(info, "the Crypt!");
            you.where_are_you = BRANCH_CRYPT;
            break;
        case DNGN_ENTER_HALL_OF_BLADES:
            strcat(info, "the Hall of Blades!");
            you.where_are_you = BRANCH_HALL_OF_BLADES;
            break;
        case DNGN_ENTER_ZOT:
            strcat(info, "the Hall of Zot!");
            you.where_are_you = BRANCH_HALL_OF_ZOT;
            break;
        case DNGN_ENTER_TEMPLE:
            strcat(info, "the Ecumenical Temple!");
            you.where_are_you = BRANCH_ECUMENICAL_TEMPLE;
            break;
        case DNGN_ENTER_SNAKE_PIT:
            strcat(info, "the Snake Pit!");
            you.where_are_you = BRANCH_SNAKE_PIT;
            break;
        case DNGN_ENTER_ELVEN_HALLS:
            strcat(info, "the Elven Halls!");
            you.where_are_you = BRANCH_ELVEN_HALLS;
            break;
        case DNGN_ENTER_TOMB:
            strcat(info, "the Tomb!");
            you.where_are_you = BRANCH_TOMB;
            break;
        case DNGN_ENTER_SWAMP:
            strcat(info, "the Swamp!");
            you.where_are_you = BRANCH_SWAMP;
            break;
        }

        mpr(info);
    }

    if (stair_find == DNGN_ENTER_LABYRINTH)
    {
        you.level_type = LEVEL_LABYRINTH;
        grd[you.x_pos][you.y_pos] = DNGN_FLOOR;
    }

    if (stair_find == DNGN_ENTER_ABYSS)
    {
        you.level_type = LEVEL_ABYSS;
    }

    if (stair_find == DNGN_ENTER_PANDEMONIUM)
    {
        you.level_type = LEVEL_PANDEMONIUM;
    }

    if (you.level_type == LEVEL_LABYRINTH || you.level_type == LEVEL_ABYSS
        || you.level_type == LEVEL_PANDEMONIUM)
    {
        char glorpstr[kFileNameSize];
        char del_file[kFileNameSize];
        int sysg;

#ifdef SAVE_DIR_PATH
        sprintf(glorpstr, SAVE_DIR_PATH "%s%d", you.your_name, getuid());
#else
        strncpy(glorpstr, you.your_name, kFileNameLen);

        // glorpstr [strlen(glorpstr)] = 0;
        // This is broken. Length is not valid yet! We have to check if we got
        // a trailing NULL; if not, write one:
        /* is name 6 chars or more? */
        if (strlen(you.your_name) > kFileNameLen - 1)
            glorpstr[kFileNameLen] = '\0';
#endif

        strcpy(del_file, glorpstr);
        strcat(del_file, ".lab");

#ifdef DOS
        strupr(del_file);
#endif
        sysg = unlink(del_file);

#ifdef WIZARD
        strcpy(info, "Deleting: ");
        strcat(info, del_file);
        mpr(info);
        more();
#endif
    }

    if (stair_find == DNGN_EXIT_ABYSS || stair_find == DNGN_EXIT_PANDEMONIUM)
    {
        leave_abyss_pan = true;
        //you.your_level--;
        mpr("You pass through the gate, and find yourself at the top of a staircase.");
        more();
    }

    if (you.level_type == LEVEL_DUNGEON)
        you.your_level++;

    int stair_taken = stair_find;

    //unsigned char save_old = 1;
    bool moving_level = true;
    bool want_followers = true;

    if (you.level_type == LEVEL_LABYRINTH || you.level_type == LEVEL_ABYSS)
        stair_taken = DNGN_FLOOR;       //81;

    if (you.level_type == LEVEL_PANDEMONIUM)
        stair_taken = DNGN_TRANSIT_PANDEMONIUM;

    if (remove_stairs)
        grd[you.x_pos][you.y_pos] = DNGN_FLOOR;

    load( stair_taken, moving_level, was_a_labyrinth, old_level,
          want_followers, false, old_where );

    moving_level = false;

    unsigned char pc = 0;
    unsigned char pt = random2avg(28, 3);


    switch (you.level_type)
    {
    case LEVEL_LABYRINTH:
        mpr("You enter a dark and forbidding labyrinth.");
        you.your_level++;
        break;

    case LEVEL_ABYSS:
        mpr("You enter the Abyss!");
        mpr("To return, you must find a gate leading back.");

        grd[you.x_pos][you.y_pos] = DNGN_FLOOR;

        if (old_level_type != LEVEL_PANDEMONIUM)
            you.your_level--;   // Linley-suggested addition 17jan2000 {dlb}

        init_pandemonium();     /* colours only */

        if (you.where_are_you > BRANCH_MAIN_DUNGEON
            && you.where_are_you < BRANCH_ORCISH_MINES)
        {
            // ie if you're in Hell
            you.where_are_you = BRANCH_MAIN_DUNGEON;
            you.your_level = 25;
        }
        break;

    case LEVEL_PANDEMONIUM:
        if (old_level_type == LEVEL_PANDEMONIUM)
        {
            mpr("You pass into a different region of Pandemonium.");
            init_pandemonium();
            for (pc = 0; pc < pt; pc++)
                pandemonium_mons();
        }
        else
        {
            mpr("You enter the halls of Pandemonium!");
            if (old_level_type != LEVEL_ABYSS)
                you.your_level--;       // Linley-suggested addition 17jan2000 {dlb}

            mpr("To return, you must find a gate leading back.");

            init_pandemonium();

            for (pc = 0; pc < pt; pc++)
                pandemonium_mons();

            if (you.where_are_you > BRANCH_MAIN_DUNGEON
                && you.where_are_you < BRANCH_ORCISH_MINES)
            {
                // ie if you're in Hell
                you.where_are_you = BRANCH_MAIN_DUNGEON;
                you.your_level = 25;
            }
        }
        break;

    default:
        mpr("You climb downwards.");
        break;
    }

    new_level();

    you.turn_is_over = 1;

    save_game(false);
}                               // end down_stairs()

void new_level(void)
{
    char temp_quant[10];

    textcolor(LIGHTGREY);

    itoa(you.your_level + 1, temp_quant, 10);

    // maybe last part better expresssed as <= PIT {dlb}
    if (you.where_are_you >= BRANCH_DIS
        && you.where_are_you < BRANCH_ORCISH_MINES)
    {
        itoa(you.your_level - 26, temp_quant, 10);
    }

    /* Remember, must add this to the death_string in ouch */
    if (you.where_are_you >= BRANCH_ORCISH_MINES
        && you.where_are_you <= BRANCH_SWAMP)
    {
        itoa(you.your_level - you.branch_stairs[you.where_are_you - 10],
             temp_quant, 10);
    }

    gotoxy(46, 12);

    env.floor_colour = LIGHTGREY;
    env.rock_colour = BROWN;

    if (you.level_type == LEVEL_PANDEMONIUM)
    {
        cprintf("- Pandemonium            ");

        env.floor_colour = (mcolour[env.mons_alloc[9]] == BLACK)
                                    ? LIGHTGREY : mcolour[env.mons_alloc[9]];

        env.rock_colour = (mcolour[env.mons_alloc[8]] == BLACK)
                                    ? LIGHTGREY : mcolour[env.mons_alloc[8]];
    }
    else if (you.level_type == LEVEL_ABYSS)
    {
        cprintf("- The Abyss               ");

        env.floor_colour = (mcolour[env.mons_alloc[9]] == BLACK)
                                    ? LIGHTGREY : mcolour[env.mons_alloc[9]];

        env.rock_colour = (mcolour[env.mons_alloc[8]] == BLACK)
                                    ? LIGHTGREY : mcolour[env.mons_alloc[8]];
    }
    else if (you.level_type == LEVEL_LABYRINTH)
    {
        cprintf("- a Labyrinth           ");
    }
    else
    {
        if (you.where_are_you != BRANCH_VESTIBULE_OF_HELL)
            cprintf(temp_quant);

        switch (you.where_are_you)
        {
        case BRANCH_MAIN_DUNGEON:
            cprintf(" of the Dungeon           ");
            break;
        case BRANCH_DIS:
            env.floor_colour = CYAN;
            env.rock_colour = CYAN;
            cprintf(" of Dis                   ");
            break;
        case BRANCH_GEHENNA:
            env.floor_colour = DARKGREY;
            env.rock_colour = RED;
            cprintf(" of Gehenna               ");
            break;
        case BRANCH_VESTIBULE_OF_HELL:
            env.floor_colour = LIGHTGREY;
            env.rock_colour = LIGHTGREY;
            cprintf("- the Vestibule of Hell            ");
            break;
        case BRANCH_COCYTUS:
            env.floor_colour = LIGHTBLUE;
            env.rock_colour = LIGHTCYAN;
            cprintf(" of Cocytus                   ");
            break;
        case BRANCH_TARTARUS:
            env.floor_colour = DARKGREY;
            env.rock_colour = DARKGREY;
            cprintf(" of Tartarus                ");
            break;
        case BRANCH_INFERNO:
            env.floor_colour = LIGHTRED;
            env.rock_colour = RED;
            cprintf(" of the Inferno               ");
            break;
        case BRANCH_THE_PIT:
            env.floor_colour = RED;
            env.rock_colour = DARKGREY;
            cprintf(" of the Pit              ");
            break;
        case BRANCH_ORCISH_MINES:
            env.floor_colour = BROWN;
            env.rock_colour = BROWN;
            cprintf(" of the Orcish Mines          ");
            break;
        case BRANCH_HIVE:
            env.floor_colour = YELLOW;
            env.rock_colour = BROWN;
            cprintf(" of the Hive                  ");
            break;
        case BRANCH_LAIR:
            env.floor_colour = GREEN;
            env.rock_colour = BROWN;
            cprintf(" of the Lair                  ");
            break;
        case BRANCH_SLIME_PITS:
            env.floor_colour = GREEN;
            env.rock_colour = LIGHTGREEN;
            cprintf(" of the Slime Pits            ");
            break;
        case BRANCH_VAULTS:
            env.floor_colour = LIGHTGREY;
            env.rock_colour = BROWN;
            cprintf(" of the Vaults                ");
            break;
        case BRANCH_CRYPT:
            env.floor_colour = LIGHTGREY;
            env.rock_colour = LIGHTGREY;
            cprintf(" of the Crypt                 ");
            break;
        case BRANCH_HALL_OF_BLADES:
            env.floor_colour = LIGHTGREY;
            env.rock_colour = LIGHTGREY;
            cprintf(" of the Hall of Blades        ");
            break;

        case BRANCH_HALL_OF_ZOT:
            if (you.your_level - you.branch_stairs[7] <= 1)
            {
                env.floor_colour = LIGHTGREY;
                env.rock_colour = LIGHTGREY;
            }
            else
            {
                switch (you.your_level - you.branch_stairs[7])
                {
                case 2:
                    env.rock_colour = LIGHTGREY;
                    env.floor_colour = BLUE;
                    break;
                case 3:
                    env.rock_colour = BLUE;
                    env.floor_colour = LIGHTBLUE;
                    break;
                case 4:
                    env.rock_colour = LIGHTBLUE;
                    env.floor_colour = MAGENTA;
                    break;
                case 5:
                    env.rock_colour = MAGENTA;
                    env.floor_colour = LIGHTMAGENTA;
                    break;
                }
            }
            cprintf(" of the Realm of Zot          ");
            break;

        case BRANCH_ECUMENICAL_TEMPLE:
            env.floor_colour = LIGHTGREY;
            env.rock_colour = LIGHTGREY;
            cprintf(" of the Temple                ");
            break;
        case BRANCH_SNAKE_PIT:
            env.floor_colour = LIGHTGREEN;
            env.rock_colour = YELLOW;
            cprintf(" of the Snake Pit             ");
            break;
        case BRANCH_ELVEN_HALLS:
            env.floor_colour = DARKGREY;
            env.rock_colour = LIGHTGREY;
            cprintf(" of the Elven Halls           ");
            break;
        case BRANCH_TOMB:
            env.floor_colour = YELLOW;
            env.rock_colour = LIGHTGREY;
            cprintf(" of the Tomb                  ");
            break;
        case BRANCH_SWAMP:
            env.floor_colour = BROWN;
            env.rock_colour = BROWN;
            cprintf(" of the Swamp                 ");
            break;
        }
    }                           // end else
}                               // end new_level()

static void dart_trap(bool trap_known, int trapped, struct bolt &pbolt)
{
    int damage_taken = 0;
    int trap_hit, your_dodge;

    if (random2(10) < 2 || (trap_known && !one_chance_in(4)))
    {
        strcpy(info, "You avoid triggering a");
        strcat(info, pbolt.beam_name);
        strcat(info, " trap.");
        mpr(info);
        return;
    }

    if (you.equip[EQ_SHIELD] != -1)
        exercise(SK_SHIELDS, (random2(3)) / 2);

    strcpy(info, "A");
    strcat(info, pbolt.beam_name);
    strcat(info, " shoots out and ");

    if (random2(50 + 5 * you.shield_blocks) < player_shield_class())
    {
        you.shield_blocks++;
        strcat(info, "hits your shield.");
        mpr(info);
        goto out_of_trap;
    }

    // note that this uses full ( not random2limit(foo,40) ) player_evasion.
    // XXX: These functions see completely inappropriate... -- bwr
    trap_hit = (20 + (you.your_level * 2)) * random2(200) / 100;
    your_dodge = player_evasion() + random2(you.dex) / 3
                            - 2 + (you.duration[DUR_REPEL_MISSILES] * 10);
    if (trap_hit >= your_dodge && you.duration[DUR_DEFLECT_MISSILES] == 0)
    {
        strcat(info, "hits you!");
        mpr(info);

        if ((strstr(pbolt.beam_name, "needle") != NULL)
                && random2(100) < 50 - (3*player_AC()/2)
                && !player_res_poison())
        {
            mpr("You are poisoned.");
            you.poison += 1 + random2(3);
        }

        damage_taken = random2(pbolt.damage);
        damage_taken -= random2(player_AC() + 1);

        if (damage_taken > 0)
            ouch(damage_taken, 0, KILLED_BY_TRAP);
    }
    else
    {
        strcat(info, "misses you.");
        mpr(info);
    }

    if (player_light_armour() && coinflip())
        exercise(SK_DODGING, 1);

  out_of_trap:

    pbolt.target_x = you.x_pos;
    pbolt.target_y = you.y_pos;

    if (coinflip())
        itrap(pbolt, trapped);
}                               // end dart_trap()

//
// itrap takes location from target_x, target_y of bolt strcture.
//

void itrap(struct bolt &pbolt, int trapped)
{
    switch (env.trap_type[trapped])
    {
    case TRAP_DART:
        pbolt.colour = OBJ_MISSILES;
        pbolt.damage = MI_DART;
        break;
    case TRAP_ARROW:
        pbolt.colour = OBJ_MISSILES;
        pbolt.damage = MI_ARROW;
        break;
    case TRAP_BOLT:
        pbolt.colour = OBJ_MISSILES;
        pbolt.damage = MI_BOLT;
        break;
    case TRAP_SPEAR:
        pbolt.colour = OBJ_WEAPONS;
        pbolt.damage = WPN_SPEAR;
        break;
    case TRAP_AXE:
        pbolt.colour = OBJ_WEAPONS;
        pbolt.damage = WPN_HAND_AXE;
        break;
    case TRAP_NEEDLE:
        pbolt.colour = OBJ_MISSILES;
        pbolt.damage = MI_NEEDLE;
        break;
    default:
        return;
    }

    trap_item(pbolt.colour, pbolt.damage, pbolt.target_x, pbolt.target_y);

    return;
}                               // end itrap()

void handle_traps(char trt, int i, bool trap_known)
{
    struct bolt beam;

    switch (trt)
    {
    case TRAP_DART:
        strcpy(beam.beam_name, " dart");
        beam.damage = 4 + (you.your_level / 2);
        dart_trap(trap_known, i, beam);
        break;

    case TRAP_NEEDLE:
        strcpy(beam.beam_name, " needle");
        beam.damage = 0;
        dart_trap(trap_known, i, beam);
        break;

    case TRAP_ARROW:
        strcpy(beam.beam_name, "n arrow");
        beam.damage = 7 + you.your_level;
        dart_trap(trap_known, i, beam);
        break;

    case TRAP_BOLT:
        strcpy(beam.beam_name, " bolt");
        beam.damage = 13 + you.your_level;
        dart_trap(trap_known, i, beam);
        break;

    case TRAP_SPEAR:
        strcpy(beam.beam_name, " spear");
        beam.damage = 10 + you.your_level;
        dart_trap(trap_known, i, beam);
        break;

    case TRAP_AXE:
        strcpy(beam.beam_name, "n axe");
        beam.damage = 15 + you.your_level;
        dart_trap(trap_known, i, beam);
        break;

    case TRAP_TELEPORT:
        mpr("You enter a teleport trap!");

        if (scan_randarts(RAP_PREVENT_TELEPORTATION))
            mpr("You feel a weird sense of stasis.");
        else
            you_teleport2(true);
        break;

    case TRAP_AMNESIA:
        mpr("You feel momentarily disoriented.");
        if (!wearing_amulet(AMU_CLARITY))
            forget_map(random2avg(100, 2));
        break;

    case TRAP_BLADE:
        if (trap_known && one_chance_in(3))
            mpr("You avoid triggering a blade trap.");
        else if (random2limit(player_evasion(), 40)
                        + (random2(you.dex) / 3) + (trap_known ? 3 : 0) > 8)
        {
            mpr("A huge blade swings just past you!");
        }
        else
        {
            mpr("A huge blade swings out and slices into you!");
            ouch( (you.your_level * 2) + random2avg(29, 2)
                    - random2(1 + player_AC()), 0, KILLED_BY_TRAP );
        }
        break;

    case TRAP_ZOT:
    default:
        mpr((trap_known) ? "You enter the Zot trap."
                         : "Oh no! You have blundered into a Zot trap!");
        miscast_effect( SPTYP_RANDOM, random2(30) + you.your_level,
                        75 + random2(100), 3 );
        break;
    }
}                               // end handle_traps()

void disarm_trap( struct dist &disa )
{
    if (you.berserker)
    {
        canned_msg(MSG_TOO_BERSERK);
        return;
    }

    int i, j;

    for (i = 0; i < MAX_TRAPS; i++)
    {
        if (env.trap_x[i] == you.x_pos + disa.dx
            && env.trap_y[i] == you.y_pos + disa.dy)
        {
            break;
        }

        if (i == MAX_TRAPS - 1)
        {
            mpr("Error - couldn't find that trap.");
            return;
        }
    }

    if (trap_category(env.trap_type[i]) == DNGN_TRAP_MAGICAL)
    {
        mpr("You can't disarm that trap.");
        return;
    }

    if (random2(you.skills[SK_TRAPS_DOORS] + 2) <=
        random2(you.your_level + 5))
    {
        mpr("You failed to disarm the trap.");

        you.turn_is_over = 1;

        if (random2(you.dex) > 5 + random2(5 + you.your_level))
            exercise(SK_TRAPS_DOORS, 1 + random2(you.your_level / 5));
        else
        {
            handle_traps(env.trap_type[i], i, false);

            if (coinflip())
                exercise(SK_TRAPS_DOORS, 1);
        }

        return;
    }

    mpr("You have disarmed the trap.");

    struct bolt beam;

    beam.target_x = you.x_pos + disa.dx;
    beam.target_y = you.y_pos + disa.dy;

    if (env.trap_type[i] != TRAP_BLADE
        && trap_category(env.trap_type[i]) == DNGN_TRAP_MECHANICAL)
    {
        for (j = 0; j < 20; j++)
        {
            // places items (eg darts), which will automatically stack
            itrap(beam, i);

            if (j > 10 && one_chance_in(3))
                break;
        }
    }

    grd[you.x_pos + disa.dx][you.y_pos + disa.dy] = DNGN_FLOOR;
    env.trap_type[i] = TRAP_UNASSIGNED;
    you.turn_is_over = 1;

    // reduced from 5 + random2(5)
    exercise(SK_TRAPS_DOORS, 1 + random2(5) + (you.your_level / 5));
}                               // end disarm_trap()

void manage_clouds(void)
{
    // amount which cloud dissipates - must be unsigned! {dlb}
    unsigned int dissipate = 0;

    for (unsigned char cc = 0; cc < MAX_CLOUDS; cc++)
    {
        if (env.cloud_type[cc] == CLOUD_NONE)   // no cloud -> next iteration
            continue;

        dissipate = you.time_taken;

        // water -> flaming clouds:
        // lava -> freezing clouds:
        if ((env.cloud_type[cc] == CLOUD_FIRE
                || env.cloud_type[cc] == CLOUD_FIRE_MON)
            && grd[env.cloud_x[cc]][env.cloud_y[cc]] == DNGN_DEEP_WATER)
        {
            dissipate *= 4;
        }
        else if ((env.cloud_type[cc] == CLOUD_COLD
                    || env.cloud_type[cc] == CLOUD_COLD_MON)
                && grd[env.cloud_x[cc]][env.cloud_y[cc]] == DNGN_LAVA)
        {
            dissipate *= 4;
        }

        // double the amount when slowed - must be applied last(!):
        if (you.slow)
            dissipate *= 2;

        // apply calculated rate to the actual cloud:
        env.cloud_decay[cc] -= dissipate;

        // check for total dissipatation and handle accordingly:
        if (env.cloud_decay[cc] < 1)
        {
            env.cloud_type[cc] = CLOUD_NONE;
            env.cloud_decay[cc] = 0;
            env.cgrid[env.cloud_x[cc]][env.cloud_y[cc]] = EMPTY_CLOUD;
            env.cloud_no--;
        }
    }

    return;
}                               // end manage_clouds()

void weird_writing(char stringy[40])
{
    int temp_rand = 0;          // for probability determinations {dlb}

    temp_rand = random2(15);

    // you'll see why later on {dlb}
    strcpy(stringy, (temp_rand == 0) ? "writhing" :
                    (temp_rand == 1) ? "bold" :
                    (temp_rand == 2) ? "faint" :
                    (temp_rand == 3) ? "spidery" :
                    (temp_rand == 4) ? "blocky" :
                    (temp_rand == 5) ? "angular" :
                    (temp_rand == 6) ? "shimmering" :
                    (temp_rand == 7) ? "glowing" : "");

    if (temp_rand < 8)
        strcat(stringy, " ");   // see above for reasoning {dlb}

    temp_rand = random2(14);

    strcat(stringy, (temp_rand ==  0) ? "yellow" :
                    (temp_rand ==  1) ? "brown" :
                    (temp_rand ==  2) ? "black" :
                    (temp_rand ==  3) ? "purple" :
                    (temp_rand ==  4) ? "orange" :
                    (temp_rand ==  5) ? "lime-green" :
                    (temp_rand ==  6) ? "blue" :
                    (temp_rand ==  7) ? "grey" :
                    (temp_rand ==  8) ? "silver" :
                    (temp_rand ==  9) ? "gold" :
                    (temp_rand == 10) ? "umber" :
                    (temp_rand == 11) ? "charcoal" :
                    (temp_rand == 12) ? "pastel" :
                    (temp_rand == 13) ? "mauve"
                                      : "colourless");

    strcat(stringy, " ");

    temp_rand = random2(14);

    strcat(stringy, (temp_rand == 0) ? "writing" :
                    (temp_rand == 1) ? "scrawl" :
                    (temp_rand == 2) ? "sigils" :
                    (temp_rand == 3) ? "runes" :
                    (temp_rand == 4) ? "hieroglyphics" :
                    (temp_rand == 5) ? "scrawl" :
                    (temp_rand == 6) ? "print-out" :
                    (temp_rand == 7) ? "binary code" :
                    (temp_rand == 8) ? "glyphs" :
                    (temp_rand == 9) ? "symbols"
                                     : "text");

    return;
}                               // end weird_writing()

// must be a better name than 'place' for the first parameter {dlb}
void fall_into_a_pool(bool place, unsigned char grype)
{
    bool escape = false;
    FixedVector< char, 2 > empty;

    if (you.species == SP_MERFOLK && grype == DNGN_DEEP_WATER)
    {
        // These can happen when we enter deep water directly -- bwr
        merfolk_start_swimming();
        return;
    }

    strcpy(info, "You fall into the ");

    strcat(info, (grype == DNGN_LAVA)       ? "lava" :
                 (grype == DNGN_DEEP_WATER) ? "water"
                                            : "programming rift");

    strcat(info, "!");
    mpr(info);

    more();
    mesclr();

    if (grype == DNGN_LAVA)
    {
        strcpy(info, "The lava burns you");

        if (player_res_fire() <= 100)
        {
            strcat(info, " to a cinder!");
            mpr(info);
            ouch(-9999, 0, KILLED_BY_LAVA);
        }
        else
        {
            // should boost # of bangs per damage in the future {dlb}
            strcat(info, "!");
            mpr(info);
            ouch( (10 + random2avg(58, 3)) / (player_res_fire() - 100), 0,
                  KILLED_BY_LAVA );
        }
    }

    // a distinction between stepping and falling from you.levitation
    // prevents stepping into a thin stream of lava to get to the other side.
    if (scramble())
    {
        if (place)
        {
            if (empty_surrounds(you.x_pos, you.y_pos, DNGN_FLOOR, false, empty))
            {
                you.x_pos = empty[0];
                you.y_pos = empty[1];
                escape = true;
            }
            else
                escape = false;
        }
        else
            escape = true;
    }
    else
    {
        // that is, don't display following when fall from levitating
        if (!place)
            mpr("You try to escape, but your burden drags you down!");
    }

    if (escape)
    {
        mpr("You manage to scramble free!");

        if (grype == DNGN_LAVA)
            scrolls_burn(10, OBJ_SCROLLS);

        return;
    }

    mpr("You drown...");

    if (grype == DNGN_LAVA)
        ouch(-9999, 0, KILLED_BY_LAVA);
    else if (grype == DNGN_DEEP_WATER)
        ouch(-9999, 0, KILLED_BY_WATER);

    // Okay, so you don't trigger a trap when you scramble onto it.
    //I really can't be bothered right now.
}                               // end fall_into_a_pool()

bool scramble(void)
{
    int max_carry = carrying_capacity();

    if ((max_carry / 2) + random2(max_carry / 2) <= you.burden)
        return false;
    else
        return true;
}                               // end scramble()

void weird_colours(unsigned char coll, char wc[30])
{
    unsigned char coll_div16 = coll / 16; // conceivable max is then 16 {dlb}

    // Must start with a consonant!
    strcpy(wc, (coll_div16 == 0 || coll_div16 ==  7) ? "brilliant" :
               (coll_div16 == 1 || coll_div16 ==  8) ? "pale" :
               (coll_div16 == 2 || coll_div16 ==  9) ? "mottled" :
               (coll_div16 == 3 || coll_div16 == 10) ? "shimmering" :
               (coll_div16 == 4 || coll_div16 == 11) ? "bright" :
               (coll_div16 == 5 || coll_div16 == 12) ? "dark" :
               (coll_div16 == 6 || coll_div16 == 13) ? "shining"
                                                     : "faint");

    strcat(wc, " ");

    while (coll > 17)
        coll -= 10;

    strcat(wc, (coll ==  0) ? "red" :
               (coll ==  1) ? "purple" :
               (coll ==  2) ? "green" :
               (coll ==  3) ? "orange" :
               (coll ==  4) ? "magenta" :
               (coll ==  5) ? "black" :
               (coll ==  6) ? "grey" :
               (coll ==  7) ? "silver" :
               (coll ==  8) ? "gold" :
               (coll ==  9) ? "pink" :
               (coll == 10) ? "yellow" :
               (coll == 11) ? "white" :
               (coll == 12) ? "brown" :
               (coll == 13) ? "aubergine" :
               (coll == 14) ? "ochre" :
               (coll == 15) ? "leaf green" :
               (coll == 16) ? "mauve" :
               (coll == 17) ? "azure"
                            : "colourless");

    return;
}                               // end weird_colours()

bool go_berserk(bool intentional)
{
    if (you.berserker)
    {
        if (intentional)
            mpr("You're already berserk!");
        // or else you won't notice -- no message here.
        return false;
    }

    if (you.exhausted)
    {
        if (intentional)
            mpr("You're too exhausted to go berserk.");
        // or else they won't notice -- no message here
        return false;
    }

    if (you.is_undead == US_UNDEAD || you.species == SP_GHOUL)
    {
        if (intentional)
            mpr("You cannot raise a blood rage in your lifeless body.");
        // or else you won't notice -- no message here
        return false;
    }

    mpr("A red film seems to cover your vision as you go berserk!");
    mpr("You feel yourself moving faster!");
    mpr("You feel mighty!");

    you.berserker += 20 + random2avg(19, 2);

    calc_hp();
    you.hp *= 15;
    you.hp /= 10;

    deflate_hp(you.hp_max, false);

    if (!you.might)
        modify_stat(STAT_STRENGTH, 5, true);

    you.might += you.berserker;
    you.haste += you.berserker;

    if (you.berserk_penalty != NO_BERSERK_PENALTY)
        you.berserk_penalty = 0;

    return true;
}                               // end go_berserk()

bool trap_item(char base_type, char sub_type, char beam_x, char beam_y)
{
    if (igrd[beam_x][beam_y] != NON_ITEM)
    {
        // first || clause ==> that is, stackable items present {dlb}
        if ((base_type == OBJ_MISSILES
                || base_type == OBJ_FOOD
                || base_type == OBJ_SCROLLS
                || base_type == OBJ_POTIONS
                || base_type == OBJ_UNKNOWN_II)
            && base_type == mitm.base_type[igrd[beam_x][beam_y]]
            && sub_type == mitm.sub_type[igrd[beam_x][beam_y]]
            && mitm.pluses[igrd[beam_x][beam_y]] == 50
            && (mitm.special[igrd[beam_x][beam_y]] == 0
                || (mitm.special[igrd[beam_x][beam_y]] == SPMSL_POISONED
                    && base_type == OBJ_MISSILES && sub_type == MI_NEEDLE)))
        {
            mitm.quantity[igrd[beam_x][beam_y]]++;
            return false;
        }
    }                           // end of if igrd != NON_ITEM

    if (igrd[beam_x][beam_y] != NON_ITEM)
    {
        if (mitm.link[igrd[beam_x][beam_y]] != NON_ITEM)
        {
            if (mitm.link[mitm.link[igrd[beam_x][beam_y]]] != NON_ITEM)
                return false;
        }
    }

    // don't want to go overboard here. Will only generate up to three
    // separate trap items, or less if there are other items present.
    int o;

    for (o = 0; o < MAX_ITEMS; o++)
    {
        if (o == 1)
            continue;

        if (o >= 200)
            return false;

        if (mitm.quantity[o] == 0)
        {
            mitm.id[o] = 0;
            mitm.base_type[o] = base_type;
            mitm.sub_type[o] = sub_type;
            mitm.pluses[o] = 50;
            mitm.pluses2[o] = 50;
            mitm.special[o] = (base_type == OBJ_MISSILES && sub_type == MI_NEEDLE)
                ?SPMSL_POISONED:0;
            mitm.colour[o] = (base_type == OBJ_MISSILES && sub_type == MI_NEEDLE)
                ?WHITE:LIGHTCYAN;
            mitm.quantity[o] = 1;
            mitm.link[o] = NON_ITEM;
            break;
        }
    }                           // end of o loop

    if (igrd[beam_x][beam_y] == NON_ITEM)
        igrd[beam_x][beam_y] = o;
    else
    {
        int hug = igrd[beam_x][beam_y];

        igrd[beam_x][beam_y] = o;
        mitm.link[o] = hug;
    }

    return true;
}                               // end trap_item()

// returns appropriate trap symbol for a given trap type {dlb}
unsigned char trap_category(unsigned char trap_type)
{
    switch (trap_type)
    {
    case TRAP_TELEPORT:
    case TRAP_AMNESIA:
    case TRAP_ZOT:
        return DNGN_TRAP_MAGICAL;

    case TRAP_DART:
    case TRAP_ARROW:
    case TRAP_SPEAR:
    case TRAP_AXE:
    case TRAP_BLADE:
    case TRAP_BOLT:
    case TRAP_NEEDLE:
    default:                    // what *would* be the default? {dlb}
        return DNGN_TRAP_MECHANICAL;
    }
}                               // end trap_category()

// returns index of the trap for a given (x,y) coordinate pair {dlb}
int trap_at_xy(int which_x, int which_y)
{

    for (int which_trap = 0; which_trap < MAX_TRAPS; which_trap++)
    {
        if (env.trap_x[which_trap] == which_x
            && env.trap_y[which_trap] == which_y)
        {
            return which_trap;
        }
    }

    // no idea how well this will be handled elsewhere: {dlb}
    return -1;
}                               // end trap_at_xy()

/*
 *  File:       direct.cc
 *  Summary:    Functions used when picking squares.
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 * <4>  11/23/99       LRH   Looking at monsters now
 *                           displays more info
 * <3>  5/12/99        BWR   changes to allow for space selection of target.
 *                           CR, ESC, and 't' in targeting.
 * <2>  5/09/99        JDJ   look_around no longer prints a prompt.
 * <1>  -/--/--        LRH   Created
 */

#include "AppHdr.h"
#include "direct.h"

#include <stdlib.h>
#include <string.h>

#ifdef DOS
#include <conio.h>
#endif

#include "externs.h"

#include "debug.h"
#include "describe.h"
#include "itemname.h"
#include "monstuff.h"
#include "mon-util.h"
#include "player.h"
#include "stuff.h"
#include "view.h"

#ifdef MACROS
#include "macro.h"
#endif

int dir_cursor(char rng);
char mons_find(unsigned char xps, unsigned char yps, FixedVector < char,
               2 > &mfp, char direction);

//---------------------------------------------------------------
//
// direction
//
// Handles entering of directions. Passes to look_around for cursor
// aiming.
//
//---------------------------------------------------------------
void direction( char rnge, struct dist &moves )
{
    char ink = 0;
    char looked = 0;

    moves.nothing = dir_cursor(rnge);

    if (moves.nothing == -9999 || moves.nothing == -10000
        || moves.nothing == -10001)
    {
        mpr("Aim (move cursor or select with '-' or '+'/'=', then 'p', '.', or '>')");
        moves.prev_targ = 0;

        if (moves.nothing == -10000)
            moves.prev_targ = 1;

        if (moves.nothing == -10001)
            moves.prev_targ = 2;

        moves.nothing = look_around(moves);
        looked = 1;
    }

    if (moves.nothing > 50000)
        moves.nothing -= 50000;

    if (moves.nothing > 10000)
    {
        moves.nothing -= 10000;
        ink = 1;
    }

    if (moves.nothing == -1)
    {
        mpr("What an unusual direction.");
        return;
    }

    if (moves.nothing == 253)
    {
        if (you.prev_targ == MHITNOT || you.prev_targ == MHITYOU)
        {
            mpr("You haven't got a target.");
            moves.nothing = -1;
        }
        else
        {
            struct monsters *montarget = &menv[you.prev_targ];

            if (!mons_near(montarget)
                || (montarget->enchantment[2] == ENCH_INVIS
                    && !player_see_invis()))
            {
                mpr("You can't see that creature any more.");
                moves.nothing = -1;
            }
            else
            {
                moves.move_x = montarget->x - you.x_pos;
                moves.move_y = montarget->y - you.y_pos;
                moves.target_x = moves.move_x + you.x_pos;
                moves.target_y = moves.move_y + you.y_pos;
            }
        }
        return;
    }

    if (moves.nothing == 254)
    {
        moves.nothing = -1;
        return;
    }

    if (looked == 0)
    {
        moves.move_x = ((int) (moves.nothing / 100)) - 7;
        moves.move_y = moves.nothing - (((int) moves.nothing / 100) * 100) - 7;
    }

    /*target_x = move_x + you.x_pos;
       target_y = move_y + you.y_pos; */

    if (ink == 1)
    {
        moves.target_x = 1;
        moves.target_y = 1;
    }

    if (moves.move_x == 0)
    {
        if (moves.move_y > 0)
            moves.move_y = 1;
        if (moves.move_y < 0)
            moves.move_y = -1;
    }

    if (moves.move_y == 0)
    {
        if (moves.move_x > 0)
            moves.move_x = 1;
        if (moves.move_x < 0)
            moves.move_x = -1;
    }

    if (abs(moves.move_x) == abs(moves.move_y))
    {
        if (moves.move_y > 0)
            moves.move_y = 1;
        if (moves.move_y < 0)
            moves.move_y = -1;
        if (moves.move_x > 0)
            moves.move_x = 1;
        if (moves.move_x < 0)
            moves.move_x = -1;
    }

    if (mgrd[moves.target_x][moves.target_y] != NON_MONSTER
        && rnge != 100)
        you.prev_targ = mgrd[moves.target_x][moves.target_y];

}                               // end of direction()

//---------------------------------------------------------------
//
// dir_cursor
//
// Another cursor input thing.
//
//---------------------------------------------------------------
int dir_cursor(char rng)
{
    char mve_x = 0, mve_y = 0;
    char bk = 0;
    int keyy;

    if (rng == 100)
        return -9999;

  getkey:
    keyy = getch();

#ifdef LINUX
    keyy = translate_keypad(keyy);
    //keyy = key_to_command(keyy);     // maybe replace with this? {dlb}
#endif

    if (keyy != 0 && keyy != '*' && keyy != '.')
    {
        switch (keyy)
        {
        case 'b':
        case '1':
            mve_x = -1;
            mve_y = 1;
            break;

        case 'j':
        case '2':
            mve_y = 1;
            mve_x = 0;
            break;

        case 'u':
        case '9':
            mve_x = 1;
            mve_y = -1;
            break;

        case 'k':
        case '8':
            mve_y = -1;
            mve_x = 0;
            break;

        case 'y':
        case '7':
            mve_y = -1;
            mve_x = -1;
            break;

        case 'h':
        case '4':
            mve_x = -1;
            mve_y = 0;
            break;

        case 'n':
        case '3':
            mve_y = 1;
            mve_x = 1;
            break;

        case 'l':
        case '6':
            mve_x = 1;
            mve_y = 0;
            break;

        case 't':
        case 'p':
            return 253;

        case '=':
        case '+':
            return -10000;

        case '-':
            return -10001;

        case 27:
            return 254;

        default:
            goto getkey;
        }
        return mve_x * 100 + mve_y + 707 + 10000;
    }

    if (keyy != '*' && keyy != '.')
        keyy = getch();

    switch (keyy)
    {
    case 'O':
        mve_x = -1;
        mve_y = 1;
        bk = 1;
        break;
    case 'P':
        mve_y = 1;
        mve_x = 0;
        bk = 1;
        break;
    case 'I':
        mve_x = 1;
        mve_y = -1;
        bk = 1;
        break;
    case 'H':
        mve_y = -1;
        mve_x = 0;
        bk = 1;
        break;
    case 'G':
        mve_y = -1;
        mve_x = -1;
        bk = 1;
        break;
    case 'K':
        mve_x = -1;
        mve_y = 0;
        bk = 1;
        break;
    case 'Q':
        mve_y = 1;
        mve_x = 1;
        bk = 1;
        break;
    case 'M':
        mve_x = 1;
        mve_y = 0;
        bk = 1;
        break;
    case '.':
    case 'S':
        bk = 1;
        mve_x = 0;
        mve_y = 0;
        break;
    case '*':
        bk = 0;
        break;                  // was 'L'

    default:
        return -1;
    }

    if (bk == 1)
    {
        return mve_x * 100 + mve_y + 707 + 10000;
    }

    if (rng == 0)
        return 254;

    return -9999;
}

//---------------------------------------------------------------
//
// look_around
//
// Accessible by the x key and when using cursor aiming. Lets you
// find out what symbols mean, and is the way to access monster
// descriptions.
//
//---------------------------------------------------------------
int look_around(struct dist &moves)
{
    int xps = 17;
    int yps = 9;
    int gotch;
    char mve_x = 0;
    char mve_y = 0;
    int trf = 0;
    FixedVector < char, 2 > monsfind_pos;
    int p = 0;
    unsigned int targ_item;

    bool printed_already = true;

    monsfind_pos[0] = you.x_pos;
    monsfind_pos[1] = you.y_pos;

    message_current_target();

    gotoxy(xps, yps);
    gotoxy(xps + 1, yps);

    losight(env.show, grd, you.x_pos, you.y_pos);

    do
    {
        if (moves.prev_targ == 0)
        {
            gotch = getch();

#ifdef LINUX
            gotch = translate_keypad(gotch);

            // maybe replace with this? {dlb}
            //gotch = key_to_command(gotch);
#endif
        }
        else
        {
            if (moves.prev_targ == 1)
                gotch = '+';
            else
                gotch = '-';
            moves.prev_targ = 0;
            printed_already = false;
        }

        if (gotch != 0 && gotch != 13)
        {
            switch (gotch)
            {
            case '.':
            case ' ':
            case '\n':
            case '5':
                mve_x = 0;
                mve_y = 0;
                gotch = 'S';
                goto thingy;

            case 'b':
            case '1':
                mve_x = -1;
                mve_y = 1;
                break;

            case 'j':
            case '2':
                mve_y = 1;
                mve_x = 0;
                break;

            case 'u':
            case '9':
                mve_x = 1;
                mve_y = -1;
                break;

            case 'k':
            case '8':
                mve_y = -1;
                mve_x = 0;
                break;

            case 'y':
            case '7':
                mve_y = -1;
                mve_x = -1;
                break;

            case 'h':
            case '4':
                mve_x = -1;
                mve_y = 0;
                break;

            case 'n':
            case '3':
                mve_y = 1;
                mve_x = 1;
                break;

            case 'l':
            case '6':
                mve_x = 1;
                mve_y = 0;
                break;

            case '?':
                {
                    const int tx = you.x_pos + xps - 17;
                    const int ty = you.y_pos + yps - 9;
                    const unsigned char tmon = mgrd[ tx ][ ty ];

                    mve_x = 0;
                    mve_y = 0;
                    if (mgrd[ tx ][ ty ] == NON_MONSTER)
                        continue;

                    if (menv[ tmon ].enchantment[2] == ENCH_INVIS
                        && !player_see_invis())
                    {
                        continue;
                    }

                    if (monster_habitat( menv[ tmon ].type ) != DNGN_FLOOR
                        && menv[ tmon ].number == 1)
                    {
                        continue;
                    }

                    describe_monsters( menv[ tmon ].type, tmon );

    #ifdef PLAIN_TERM
                    redraw_screen();
    #endif
                }
                break;

            case 'p':
            case '>':
                goto finished_looking;

            case '-':
                mve_x = 0;
                mve_y = 0;
                if (mons_find(xps, yps, monsfind_pos, -1) == 1)
                {
                    xps = monsfind_pos[0];
                    yps = monsfind_pos[1];
                }
                break;

            case '+':
            case '=':
                mve_x = 0;
                mve_y = 0;
                if (mons_find(xps, yps, monsfind_pos, 1) == 1)
                {
                    xps = monsfind_pos[0];
                    yps = monsfind_pos[1];
                }
                break;

            default:
                return -1;
            }

            goto gotchy;
        }

        if (gotch != 13)
            gotch = getch();

        mve_x = 0;
        mve_y = 0;

      thingy:
        switch (gotch)
        {
        case 13:
            gotch = 'S';
            break;

        case 'O':
            mve_x = -1;
            mve_y = 1;
            break;

        case 'P':
            mve_y = 1;
            mve_x = 0;
            break;

        case 'I':
            mve_x = 1;
            mve_y = -1;
            break;

        case 'H':
            mve_y = -1;
            mve_x = 0;
            break;

        case 'G':
            mve_y = -1;
            mve_x = -1;
            break;

        case 'K':
            mve_x = -1;
            mve_y = 0;
            break;

        case 'Q':
            mve_y = 1;
            mve_x = 1;
            break;

        case 'M':
            mve_x = 1;
            mve_y = 0;
            break;

        case 'S':
            break;
            // need <, > etc

        default:
            return -1;
        }

      gotchy:
        gotoxy(xps, yps);

        if (xps + mve_x >= 9 && xps + mve_x < 26)
            xps += mve_x;
        if (yps + mve_y >= 1 && yps + mve_y < 18)
            yps += mve_y;

        if (printed_already)
            mesclr();

        printed_already = true;

        if (env.show[xps - 8][yps] == 0 && (xps != 17 || yps != 9))
        {
            mpr("You can't see that place.");
            goto glogokh;
        }

        if (mgrd[you.x_pos + xps - 17][you.y_pos + yps - 9] != NON_MONSTER)
        {
            int i = mgrd[you.x_pos + xps - 17][you.y_pos + yps - 9];

            if (grd[you.x_pos + xps - 17][you.y_pos + yps - 9]
                                                    == DNGN_SHALLOW_WATER)
            {
                if (menv[i].enchantment[2] == ENCH_INVIS
                    && mons_flies(menv[i].type) == 0 && !player_see_invis())
                {
                    mpr("There is a strange disturbance in the water here.");
                }
            }

            if (menv[i].enchantment[2] == ENCH_INVIS && !player_see_invis())
                goto look_clouds;

            // and how the hell does a variable named mmov_x communicate
            // this? :P {dlb}
            int mmov_x = menv[i].inv[MSLOT_WEAPON];

            if (menv[i].type == MONS_DANCING_WEAPON)
            {
                it_name(mmov_x, 2, str_pass);
                strcpy(info, str_pass);
                strcat(info, ".");
                mpr(info);
            }
            else
            {
                strcpy(info, ptr_monam( &(menv[i]), 2 ));
                strcat(info, ".");
                mpr(info);

                if (mmov_x != NON_ITEM)
                {
                    strcpy(info, "It is wielding ");
                    it_name(mmov_x, 3, str_pass);
                    strcat(info, str_pass);

                    // 2-headed ogres can wield 2 weapons
                    if (menv[i].type == MONS_TWO_HEADED_OGRE
                        && menv[i].inv[MSLOT_MISSILE] != NON_ITEM)
                    {
                        strcat(info, ",");
                        mpr(info);
                        strcpy(info, " and ");
                        it_name(menv[i].inv[MSLOT_MISSILE], 3, str_pass);
                        strcat(info, str_pass);
                    }
                    strcat(info, ".");
                    mpr(info);
                }
            }

            if (menv[i].type == MONS_HYDRA)
            {
                strcpy(info, "It has ");
                itoa(menv[i].number, st_prn, 10);
                strcat(info, st_prn);
                strcat(info, " heads.");
                mpr(info);
            }

            print_wounds(&menv[i]);

            if (menv[i].behavior == BEH_ENSLAVED)
                mpr("It is friendly.");

            if (menv[i].behavior == BEH_SLEEP)
                mpr("It doesn't appear to have noticed you.");

            if (menv[i].enchantment1)
            {
                for (p = 0; p < 3; p++)
                {
                    switch (menv[i].enchantment[p])
                    {
                    case ENCH_YOUR_ROT_I:
                    case ENCH_YOUR_ROT_II:
                    case ENCH_YOUR_ROT_III:
                    case ENCH_YOUR_ROT_IV:
                        mpr("It is rotting away."); //jmf: "covered in sores"?
                        break;
                    case ENCH_BACKLIGHT_I:
                    case ENCH_BACKLIGHT_II:
                    case ENCH_BACKLIGHT_III:
                    case ENCH_BACKLIGHT_IV:
                        mpr("It is softly glowing.");
                        break;
                    case ENCH_SLOW:
                        mpr("It is moving slowly.");
                        break;
                    case ENCH_HASTE:
                        mpr("It is moving very quickly.");
                        break;
                    case ENCH_CONFUSION:
                        mpr("It appears to be bewildered and confused.");
                        break;
                    case ENCH_INVIS:
                        mpr("It is slightly transparent.");
                        break;
                    case ENCH_CHARM:
                        mpr("It is in your thrall.");
                        break;
                    case ENCH_YOUR_STICKY_FLAME_I:
                    case ENCH_YOUR_STICKY_FLAME_II:
                    case ENCH_YOUR_STICKY_FLAME_III:
                    case ENCH_YOUR_STICKY_FLAME_IV:
                    case ENCH_STICKY_FLAME_I:
                    case ENCH_STICKY_FLAME_II:
                    case ENCH_STICKY_FLAME_III:
                    case ENCH_STICKY_FLAME_IV:
                        mpr("It is covered in liquid flames.");
                        break;
                    }
                }
            }
#ifdef WIZARD
            stethoscope(i);
#endif
        }

      look_clouds:
        if (env.cgrid[you.x_pos + xps - 17][you.y_pos + yps - 9]
                                                        != EMPTY_CLOUD)
        {
            const char cloud_inspected = env.cgrid[you.x_pos + xps - 17]
                                                  [you.y_pos + yps - 9];

            const char cloud_type = env.cloud_type[ cloud_inspected ];

            strcpy(info, "There is a cloud of ");
            strcat(info,
                (cloud_type == CLOUD_FIRE
                  || cloud_type == CLOUD_FIRE_MON) ? "flame" :
                (cloud_type == CLOUD_STINK
                  || cloud_type == CLOUD_STINK_MON) ? "noxious fumes" :
                (cloud_type == CLOUD_COLD
                  || cloud_type == CLOUD_COLD_MON) ? "freezing vapour" :
                (cloud_type == CLOUD_POISON
                  || cloud_type == CLOUD_POISON_MON) ? "poison gases" :
                (cloud_type == CLOUD_GREY_SMOKE
                  || cloud_type == CLOUD_GREY_SMOKE_MON) ? "grey smoke" :
                (cloud_type == CLOUD_BLUE_SMOKE
                  || cloud_type == CLOUD_BLUE_SMOKE_MON) ? "blue smoke" :
                (cloud_type == CLOUD_PURP_SMOKE
                  || cloud_type == CLOUD_PURP_SMOKE_MON) ? "purple smoke" :
                (cloud_type == CLOUD_STEAM
                  || cloud_type == CLOUD_STEAM_MON) ? "steam" :
                (cloud_type == CLOUD_MIASMA
                  || cloud_type == CLOUD_MIASMA_MON) ? "foul pestilence" :
                (cloud_type == CLOUD_BLACK_SMOKE
                  || cloud_type == CLOUD_BLACK_SMOKE_MON) ? "black smoke"
                                                          : "buggy goodness");
            strcat(info, " here.");
            mpr(info);
        }

        targ_item = igrd[ you.x_pos + xps - 17 ][ you.y_pos + yps - 9 ];

        if (targ_item != NON_ITEM)
        {
            if (mitm.base_type[ targ_item ] == OBJ_GOLD)
            {
                mpr("You see some money here.");
            }
            else
            {
                strcpy(info, "You see ");
                it_name( targ_item, 3, str_pass);
                strcat(info, str_pass);
                strcat(info, " here.");
                mpr(info);
            }

            if (mitm.link[ targ_item ] != NON_ITEM)
                mpr("There is something else lying underneath.");
        }

        switch (grd[you.x_pos + xps - 17][you.y_pos + yps - 9])
        {
        case DNGN_STONE_WALL:
            mpr("A stone wall.");
            break;
        case DNGN_ROCK_WALL:
        case DNGN_SECRET_DOOR:
            if (you.level_type == LEVEL_PANDEMONIUM)
                mpr("A wall of the weird stuff which makes up Pandemonium.");
            else
                mpr("A rock wall.");
            break;
        case DNGN_CLOSED_DOOR:
            mpr("A closed door.");
            break;
        case DNGN_METAL_WALL:
            mpr("A metal wall.");
            break;
        case DNGN_GREEN_CRYSTAL_WALL:
            mpr("A wall of green crystal.");
            break;
        case DNGN_ORCISH_IDOL:
            mpr("An orcish idol.");
            break;
        case DNGN_WAX_WALL:
            mpr("A wall of solid wax.");
            break;
        case DNGN_SILVER_STATUE:
            mpr("A silver statue.");
            break;
        case DNGN_GRANITE_STATUE:
            mpr("A granite statue.");
            break;
        case DNGN_ORANGE_CRYSTAL_STATUE:
            mpr("An orange crystal statue.");
            break;
        case DNGN_LAVA:
            mpr("Some lava.");
            break;
        case DNGN_DEEP_WATER:
            mpr("Some deep water.");
            break;
        case DNGN_SHALLOW_WATER:
            mpr("Some shallow water.");
            break;
        case DNGN_UNDISCOVERED_TRAP:
        case DNGN_FLOOR:
            mpr("Floor.");
            break;
        case DNGN_OPEN_DOOR:
            mpr("An open door.");
            break;
        case DNGN_ROCK_STAIRS_DOWN:
            mpr("A rock staircase leading down.");
            break;
        case DNGN_STONE_STAIRS_DOWN_I:
        case DNGN_STONE_STAIRS_DOWN_II:
        case DNGN_STONE_STAIRS_DOWN_III:
            mpr("A stone staircase leading down.");
            break;
        case DNGN_ROCK_STAIRS_UP:
            mpr("A rock staircase leading upwards.");
            break;
        case DNGN_STONE_STAIRS_UP_I:
        case DNGN_STONE_STAIRS_UP_II:
        case DNGN_STONE_STAIRS_UP_III:
            mpr("A stone staircase leading up.");
            break;
        case DNGN_ENTER_HELL:
            mpr("A gateway to hell.");
            break;
        case DNGN_BRANCH_STAIRS:
            mpr("A staircase to a branch level.");
            break;
        case DNGN_TRAP_MECHANICAL:
        case DNGN_TRAP_MAGICAL:
        case DNGN_TRAP_III:
            for (trf = 0; trf < MAX_TRAPS; trf++)
            {
                if (env.trap_x[trf] == you.x_pos + xps - 17
                    && env.trap_y[trf] == you.y_pos + yps - 9)
                {
                    break;
                }

                if (trf == MAX_TRAPS - 1)
                {
                    mpr("Error - couldn't find that trap.");
                    error_message_to_player();
                    break;
                }
            }

            switch (env.trap_type[trf])
            {
            case TRAP_DART:
                mpr("A dart trap.");
                break;
            case TRAP_ARROW:
                mpr("An arrow trap.");
                break;
            case TRAP_SPEAR:
                mpr("A spear trap.");
                break;
            case TRAP_AXE:
                mpr("An axe trap.");
                break;
            case TRAP_TELEPORT:
                mpr("A teleportation trap.");
                break;
            case TRAP_AMNESIA:
                mpr("An amnesia trap.");
                break;
            case TRAP_BLADE:
                mpr("A blade trap.");
                break;
            case TRAP_BOLT:
                mpr("A bolt trap.");
                break;
            case TRAP_ZOT:
                mpr("A Zot trap.");
                break;
            default:
                mpr("An undefined trap. Huh?");
                error_message_to_player();
                break;
            }
            break;
        case DNGN_ENTER_SHOP:
            mpr("A shop.");
            break;
        case DNGN_ENTER_LABYRINTH:
            mpr("A labyrinth entrance.");
            break;
        case DNGN_ENTER_DIS:
            mpr("A gateway to the Iron City of Dis.");
            break;
        case DNGN_ENTER_GEHENNA:
            mpr("A gateway to Gehenna.");
            break;
        case DNGN_ENTER_COCYTUS:
            mpr("A gateway to the freezing wastes of Cocytus.");
            break;
        case DNGN_ENTER_TARTARUS:
            mpr("A gateway to the decaying netherworld of Tartarus.");
            break;
        case DNGN_ENTER_ABYSS:
            mpr("A gateway to the infinite Abyss.");
            break;
        case DNGN_EXIT_ABYSS:
            mpr("A gateway leading out of the Abyss.");
            break;
        case DNGN_STONE_ARCH:
            mpr("An empty arch of ancient stone.");
            break;
        case DNGN_ENTER_PANDEMONIUM:
            mpr("A gate leading to the halls of Pandemonium.");
            break;
        case DNGN_EXIT_PANDEMONIUM:
            mpr("A gate leading out of Pandemonium.");
            break;
        case DNGN_TRANSIT_PANDEMONIUM:
            mpr("A gate leading to another region of Pandemonium.");
            break;
        case DNGN_ENTER_ORCISH_MINES:
            mpr("A staircase to the Orcish Mines.");
            break;
        case DNGN_ENTER_HIVE:
            mpr("A staircase to the Hive.");
            break;
        case DNGN_ENTER_LAIR_I:
            mpr("A staircase to the Lair.");
            break;
        case DNGN_ENTER_SLIME_PITS:
            mpr("A staircase to the Slime Pits.");
            break;
        case DNGN_ENTER_VAULTS:
            mpr("A staircase to the Vaults.");
            break;
        case DNGN_ENTER_CRYPT_I:
            mpr("A staircase to the Crypt.");
            break;
        case DNGN_ENTER_HALL_OF_BLADES:
            mpr("A staircase to the Hall of Blades.");
            break;
        case DNGN_ENTER_ZOT:
            mpr("A gate to the Realm of Zot.");
            break;
        case DNGN_ENTER_TEMPLE:
            mpr("A staircase to the Ecumenical Temple.");
            break;
        case DNGN_ENTER_SNAKE_PIT:
            mpr("A staircase to the Snake Pit.");
            break;
        case DNGN_ENTER_ELVEN_HALLS:
            mpr("A staircase to the Elven Halls.");
            break;
        case DNGN_ENTER_TOMB:
            mpr("A staircase to the Tomb.");
            break;
        case DNGN_ENTER_SWAMP:
            mpr("A staircase to the Swamp.");
            break;
        case DNGN_RETURN_DUNGEON_I:
        case DNGN_RETURN_DUNGEON_II:
        case DNGN_RETURN_DUNGEON_III:
        case DNGN_RETURN_DUNGEON_IV:
        case DNGN_RETURN_DUNGEON_V:
            mpr("A staircase back to the Dungeon.");
            break;
        case DNGN_RETURN_LAIR_II:
            mpr("A staircase back to the Lair.");
            break;
        case DNGN_RETURN_VAULTS:
            mpr("A staircase back to the Vaults.");
            break;
        case DNGN_RETURN_CRYPT_II:
            mpr("A staircase back to the Crpyt.");
            break;
        case DNGN_RETURN_LAIR_III:
            mpr("A staircase back to the Lair.");
            break;
        case DNGN_RETURN_MINES:
            mpr("A staircase back to the Mines.");
            break;
        case DNGN_RETURN_CRYPT_III:
            mpr("A staircase back to the Crypt.");
            break;
        case DNGN_EXIT_ZOT:
            mpr("A gate leading back out of this place.");
            break;
        case DNGN_RETURN_LAIR_IV:
            mpr("A staircase back to the Lair.");
            break;
        case DNGN_ALTAR_ZIN:
            mpr("A glowing white marble altar of Zin.");
            break;
        case DNGN_ALTAR_SHINING_ONE:
            mpr("A glowing golden altar of the Shining One.");
            break;
        case DNGN_ALTAR_KIKUBAAQUDGHA:
            mpr("An ancient bone altar of Kikubaaqudgha.");
            break;
        case DNGN_ALTAR_YREDELEMNUL:
            mpr("A basalt altar of Yredelemnul.");
            break;
        case DNGN_ALTAR_XOM:
            mpr("A shimmering altar of Xom.");
            break;
        case DNGN_ALTAR_VEHUMET:
            mpr("A shining altar of Vehumet.");
            break;
        case DNGN_ALTAR_OKAWARU:
            mpr("An iron altar of Okawaru.");
            break;
        case DNGN_ALTAR_MAKHLEB:
            mpr("A burning altar of Makhleb.");
            break;
        case DNGN_ALTAR_SIF_MUNA:
            mpr("A deep blue altar of Sif Muna.");
            break;
        case DNGN_ALTAR_TROG:
            mpr("A bloodstained altar of Trog.");
            break;
        case DNGN_ALTAR_NEMELEX_XOBEH:
            mpr("A sparkling altar of Nemelex Xobeh.");
            break;
        case DNGN_ALTAR_ELYVILON:
            mpr("A silver altar of Elyvilon.");
            break;
        case DNGN_BLUE_FOUNTAIN:
            mpr("A fountain of clear blue water.");
            break;
        case DNGN_SPARKLING_FOUNTAIN:
            mpr("A fountain of sparkling water.");
            break;
        case DNGN_DRY_FOUNTAIN_I:
        case DNGN_DRY_FOUNTAIN_II:
        case DNGN_DRY_FOUNTAIN_IV:
        case DNGN_DRY_FOUNTAIN_VI:
        case DNGN_DRY_FOUNTAIN_VIII:
        case DNGN_PERMADRY_FOUNTAIN:
            mpr("A dry fountain.");
            break;
        }

      glogokh:
        itoa((int) grd[you.x_pos + xps - 17][you.y_pos + yps - 9], st_prn, 10);
        strcpy(info, st_prn);
        gotoxy(xps + 1, yps);
    }
    while (gotch != 'S');

  finished_looking:
    moves.move_x = xps - 17;
    moves.move_y = yps - 9;
    moves.target_x = you.x_pos + xps - 17;
    moves.target_y = you.y_pos + yps - 9;

    if (gotch == 'p')
        return 253;
    if (gotch == '>')
        return 50001;

    return 0;                   //mve_x * 100 + mve_y + 707 + 10000;
}                               // end look_around()

//---------------------------------------------------------------
//
// mons_find
//
// Finds the next monster (moving in a spiral outwards from the
// player, so closer monsters are chosen first; starts to player's
// left) and puts its coordinates in mfp. Returns 1 if it found
// a monster, zero otherwise. If direction is -1, goes backwards.
//
//---------------------------------------------------------------
char mons_find(unsigned char xps, unsigned char yps,
                            FixedVector < char, 2 > &mfp, char direction)
{
    unsigned char temp_xps = xps;
    unsigned char temp_yps = yps;
    char x_change = 0;
    char y_change = 0;

    int i, j;

    if (direction == 1 && temp_xps == 9 && temp_yps == 17)
        return 0;               // end of spiral

    while (temp_xps >= 8 && temp_xps <= 25 && temp_yps <= 17) // yps always >= 0
    {
        if (direction == -1 && temp_xps == 17 && temp_yps == 9)
            return 0;           // can't go backwards from you

        if (direction == 1)
        {
            if (temp_xps == 8)
            {
                x_change = 0;
                y_change = -1;
            }
            else if (temp_xps - 17 == 0 && temp_yps - 9 == 0)
            {
                x_change = -1;
                y_change = 0;
            }
            else if (abs(temp_xps - 17) <= abs(temp_yps - 9))
            {
                if (temp_xps - 17 >= 0 && temp_yps - 9 <= 0)
                {
                    if (abs(temp_xps - 17) > abs(temp_yps - 9 + 1))
                    {
                        x_change = 0;
                        y_change = -1;
                        if (temp_xps - 17 > 0)
                            y_change = 1;
                        goto finished_spiralling;
                    }
                }
                x_change = -1;
                if (temp_yps - 9 < 0)
                    x_change = 1;
                y_change = 0;
            }
            else
            {
                x_change = 0;
                y_change = -1;
                if (temp_xps - 17 > 0)
                    y_change = 1;
            }
        }                       // end if (direction == 1)
        else
        {
            /*
               This part checks all eight surrounding squares to find the
               one that leads on to the present square.
             */
            for (i = -1; i < 2; i++)
            {
                for (j = -1; j < 2; j++)
                {
                    if (i == 0 && j == 0)
                        continue;

                    if (temp_xps + i == 8)
                    {
                        x_change = 0;
                        y_change = -1;
                    }
                    else if (temp_xps + i - 17 == 0 && temp_yps + j - 9 == 0)
                    {
                        x_change = -1;
                        y_change = 0;
                    }
                    else if (abs(temp_xps + i - 17) <= abs(temp_yps + j - 9))
                    {
                        const int xi = temp_xps + i - 17;
                        const int yj = temp_yps + j - 9;

                        if (xi >= 0 && yj <= 0)
                        {
                            if (abs(xi) > abs(yj + 1))
                            {
                                x_change = 0;
                                y_change = -1;
                                if (xi > 0)
                                    y_change = 1;
                                goto finished_spiralling;
                            }
                        }

                        x_change = -1;
                        if (yj < 0)
                            x_change = 1;
                        y_change = 0;
                    }
                    else
                    {
                        x_change = 0;
                        y_change = -1;
                        if (temp_xps + i - 17 > 0)
                            y_change = 1;
                    }

                    if (temp_xps + i + x_change == temp_xps
                        && temp_yps + j + y_change == temp_yps)
                    {
                        goto finished_spiralling;
                    }
                }
            }
        }                       // end else


      finished_spiralling:
        x_change *= direction;
        y_change *= direction;

        temp_xps += x_change;
        if (temp_yps + y_change <= 17)  // it can wrap, unfortunately
            temp_yps += y_change;

        const int targ_x = you.x_pos + temp_xps - 17;
        const int targ_y = you.y_pos + temp_yps - 9;
        const int targ_mon = mgrd[ targ_x ][ targ_y ];

        // We don't want to be looking outside the bounds of the arrays:
        if (temp_xps <= 25 && temp_xps >= 8 && temp_yps <= 17// && temp_yps >= 1
            && targ_x >= 0 && targ_x < GXM && targ_y >= 0 && targ_y < GYM)
        {
            if (targ_mon != NON_MONSTER && env.show[temp_xps - 8][temp_yps] != 0
                && (menv[ targ_mon ].enchantment[2] != ENCH_INVIS
                    || player_see_invis())
                && (monster_habitat(menv[ targ_mon ].type) == DNGN_FLOOR
                    || menv[ targ_mon ].number != 1))
            {
                //mpr("Found something!");
                //more();
                mfp[0] = temp_xps;
                mfp[1] = temp_yps;
                return 1;
            }
        }
    }

    return 0;
}

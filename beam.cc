/*
 *  File:       beam.cc
 *  Summary:    Functions related to ranged attacks.
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 *   <4>    11/14/99     cdl    evade beams with random40(ev) vice random2(ev)
 *                              all armour now protects against shrapnel
 *   <3>     6/ 2/99     DML    Added enums
 *   <2>     5/20/99     BWR    Added refreshs for curses
 *   <1>     -/--/--     LRH    Created
 */

#include "AppHdr.h"
#include "beam.h"

#ifdef DOS
#include <dos.h>
#include <conio.h>
#endif

#include <stdlib.h>
#include <string.h>
#include "externs.h"
#include "enum.h"

#include "bang.h"
#include "direct.h"
#include "effects.h"
#include "fight.h"
#include "it_use2.h"
#include "itemname.h"
#include "items.h"
#include "levels.h"
#include "message.h"
#include "misc.h"
#include "monstuff.h"
#include "mstruct.h"
#include "mstuff2.h"
#include "ouch.h"
#include "output.h"
#include "player.h"
#include "abyss.h"
#include "shopping.h"
#include "skills.h"
#include "spells1.h"
#include "spells2.h"
#include "spells3.h"
#include "stuff.h"
#include "view.h"

#ifdef USE_CURSES
#include <curses.h>
#endif

int check_mons_resists(struct bolt beam[1], int o, int hurted);
int check_mons_magres(int mn, int pow);
int mons_ench_f2(int o, char is_near, int func_pass[10], struct bolt beam[1]);


// Up to line 1111


/*
   Handles any projectiles which leave visible non-smoke trails and can affect
   multiple opponents. Eg firebolt, but not magic dart or thrown weapons.
   Should be merged with missile(), as there is a lot of redundant code here.
 */
void beam(struct bolt beam[1])
{
    int beam_sign_x = 0;
    int beam_sign_y = 0;
    float bmoy = 0;
    int bmax = 0;
    float bmay = 0;
    int n, o;

    char clouty = 0;

    char crumble = 0;
    char bounce = 0;

    int hurted = 0;

    beam[0].bx = beam[0].source_x;
    beam[0].by = beam[0].source_y;

    if (beam[0].move_x > 0)
        beam_sign_x = 1;
    if (beam[0].move_x < 0)
    {
        beam_sign_x = -1;
        beam[0].move_x *= -1;
    }

    if (beam[0].move_y > 0)
        beam_sign_y = 1;
    if (beam[0].move_y < 0)
    {
        beam_sign_y = -1;
        beam[0].move_y *= -1;
    }

    if (beam[0].move_x > 1 || beam[0].move_y > 1 || beam[0].move_x < -1 || beam[0].move_y < -1)
    {

        bmoy = beam[0].move_y;

        if (beam[0].move_x > beam[0].move_y)
        {
            if (beam_sign_x == 1)
                bmay = (float) (beam[0].move_y) / (beam[0].move_x + 1);
            if (beam_sign_x == -1)
                bmay = (float) (beam[0].move_y) / (beam[0].move_x + 1);
            bmax = 1;
        }
        else
        {
            if (beam_sign_y == 1)
                bmay = (float) (beam[0].move_x) / (beam[0].move_y + 1);
            if (beam_sign_y == -1)
                bmay = (float) (beam[0].move_x) / (beam[0].move_y + 1);
            bmax = 2;
        }
        bmoy = 1;
    }                           //else beam_inc = 0;

    for (n = 1; n < beam[0].range; n++)
    {
        if (beam[0].beam_name[0] == '0' && beam[0].flavour != BEAM_MAGIC)
            return;

        beam[0].move_x = beam_sign_x;
        beam[0].move_y = beam_sign_y;


        if (bmax == 1)
        {
            beam[0].move_x = beam_sign_x;
            bmoy += bmay;
            if (bmoy >= 1)
            {
                beam[0].move_y = beam_sign_y;
                bmoy -= 1;
            }
            else
                beam[0].move_y = 0;
        }



        if (bmax == 2)
        {
            beam[0].move_y = beam_sign_y;
            bmoy += bmay;
            if (bmoy >= 1)
            {
                beam[0].move_x = beam_sign_x;
                bmoy -= 1;
            }
            else
                beam[0].move_x = 0;
        }

        if (strcmp(beam[0].beam_name, "blast of poison") == 0)
        {
            if (beam[0].thing_thrown != KILL_MON)
                place_cloud(CLOUD_POISON, beam[0].bx, beam[0].by, random2(4) + 2);
            else
                place_cloud(CLOUD_POISON_MON, beam[0].bx, beam[0].by, random2(4) + 2);
        }

        if (strcmp(beam[0].beam_name, "blast of poison") == 0 && beam[0].bx == beam[0].target_x && beam[0].by == beam[0].target_y)
        {
            if (beam[0].thing_thrown != KILL_MON)
                big_cloud(4, beam[0].bx, beam[0].by, 0);
            else
                big_cloud(104, beam[0].bx, beam[0].by, 0);
            beam[0].aim_down = 0;
            return;
        }

        if (beam[0].colour == BROWN && beam[0].beam_name[0] == '0')
        {
            beam[0].bx += beam[0].move_x;
            beam[0].by += beam[0].move_y;

//#ifndef WIZARD
            if (grd[beam[0].bx][beam[0].by] == DNGN_STONE_WALL || grd[beam[0].bx][beam[0].by] == DNGN_METAL_WALL || beam[0].bx <= 5 || beam[0].by <= 5 || beam[0].bx >= GXM - 5 || beam[0].by >= GYM - 5)
            {
                beam[0].aim_down = 0;
                return;
            }
//#else
            if (beam[0].bx < 0 || beam[0].by < 0 || beam[0].bx > GXM - 1 || beam[0].by > GYM - 1)
            {
                beam[0].aim_down = 0;
                return;
            }
//#endif
            if (grd[beam[0].bx][beam[0].by] == DNGN_ROCK_WALL)
            {
                grd[beam[0].bx][beam[0].by] = DNGN_FLOOR;
                if (crumble == 0)       // && see_grid(beam[0].bx, beam[0].by) == 1)

                {
                    mpr("You hear a grinding noise.");
                    crumble = 1;
                    beam[0].wand_id = 1;
                }
            }
        }
        else if (grd[beam[0].bx + beam[0].move_x][beam[0].by + beam[0].move_y] >= MINMOVE)
        {
            beam[0].bx += beam[0].move_x;
            beam[0].by += beam[0].move_y;
        }
        else
        {

/*              if (beam[0].flavour == 10)
   {
   explosion1(beam);
   beam[0].aim_down = 0;
   return;
   } */
            if ((beam[0].colour == WHITE && beam[0].flavour != BEAM_COLD
                        && beam[0].hit >= 15) || beam[0].flavour == BEAM_NUKE)
                 /* disintegration (or powerful disruption), eye of devastation */
            {
                if ((grd[beam[0].bx + beam[0].move_x][beam[0].by + beam[0].move_y] == DNGN_ROCK_WALL || grd[beam[0].bx + beam[0].move_x][beam[0].by + beam[0].move_y] == DNGN_WAX_WALL) && !(beam[0].bx <= 6 || beam[0].by <= 6 || beam[0].bx >= GXM - 6 || beam[0].by >= GYM - 6))
                {
                    grd[beam[0].bx + beam[0].move_x][beam[0].by + beam[0].move_y] = DNGN_FLOOR;
                    mpr("You hear a grinding noise.");
                    beam[0].wand_id = 1;
                }
                if (grd[beam[0].bx + beam[0].move_x][beam[0].by + beam[0].move_y] == DNGN_ORCISH_IDOL || (grd[beam[0].bx + beam[0].move_x][beam[0].by + beam[0].move_y] >= DNGN_SILVER_STATUE && grd[beam[0].bx + beam[0].move_x][beam[0].by + beam[0].move_y] <= DNGN_STATUE_39))
                {
                    grd[beam[0].bx + beam[0].move_x][beam[0].by + beam[0].move_y] = DNGN_FLOOR;
                    if (see_grid(beam[0].bx + beam[0].move_x, beam[0].by + beam[0].move_y) == 0)
                        mpr("You hear a hideous screaming!");
                    else
                        mpr("The statue screams as its substance crumbles away!");
                    beam[0].wand_id = 1;
                }
                return;
            }

            if (beam[0].beam_name[0] == '0' || beam[0].flavour != BEAM_ELECTRICITY)
                // ie non-bouncing enchantments, non-electrical beams etc
            {
                beam[0].aim_down = 0;
                return;
            }

            n++;                // reduces beam's range by 1

            bounce += 2;
            if (random2(bounce) >= 1 || bounce > 3)
            {
                beam[0].aim_down = 0;
                return;
            }
            strcpy(info, "The ");
            strcat(info, beam[0].beam_name);
            strcat(info, " bounces.");
            mpr(info);
            if (beam[0].move_x == 0 ^ beam[0].move_y == 0)      //   ^ XOR

            {
                beam_sign_x *= -1;
                beam_sign_y *= -1;
            }
            if (beam[0].move_x != 0 && beam[0].move_y != 0)
            {
                if (grd[beam[0].bx][beam[0].by + beam[0].move_y] >= MINMOVE && grd[beam[0].bx + beam[0].move_x][beam[0].by] >= MINMOVE)
                {
                    beam_sign_x *= -1;
                    beam_sign_y *= -1;
                }
                if (grd[beam[0].bx + beam[0].move_x][beam[0].by] < MINMOVE && grd[beam[0].bx][beam[0].by + beam[0].move_y] >= MINMOVE)
                {
                    beam_sign_x *= -1;
                }
                if (grd[beam[0].bx][beam[0].by + beam[0].move_y] < MINMOVE && grd[beam[0].bx + beam[0].move_x][beam[0].by] >= MINMOVE)
                {
                    beam_sign_y *= -1;
                }
                if ((grd[beam[0].bx][beam[0].by + beam[0].move_y] < MINMOVE && grd[beam[0].bx + beam[0].move_x][beam[0].by] < MINMOVE))         // || (grd [beam[0].bx] [beam[0].by + beam[0].move_y] > 10 && grd [beam[0].bx + beam[0].move_x] [beam[0].by]))

                {
                    beam_sign_x *= -1;
                    beam_sign_y *= -1;
                }

            }
        }

/*      if (beam[0].bx > you.x_pos - 9 && beam[0].bx < you.x_pos + 9 && beam[0].by > you.y_pos - 9 && beam[0].by < you.y_pos + 9 && (show [beam[0].bx - you.x_pos + 9] [beam[0].by - you.y_pos + 9] != 0 || (beam[0].bx == you.x_pos && beam[0].by == you.y_pos)) && beam[0].type != 0) */
        if (((beam[0].bx > you.x_pos - 9 && beam[0].bx < you.x_pos + 9 && beam[0].by > you.y_pos - 9 && beam[0].by < you.y_pos + 9) || (beam[0].bx == you.x_pos && beam[0].by == you.y_pos)) && beam[0].type != 0)
        {
            if (beam[0].range != n)
            {
                textcolor(beam[0].colour);
                gotoxy(beam[0].bx - you.x_pos + 18, beam[0].by - you.y_pos + 9);
                putch(beam[0].type);
#ifdef USE_CURSES
                // If we don't refresh curses we won't
                // guarantee that the beam is visible
                refresh();
#endif
                delay(15);
            }
        }

        if (bounce > 0)
            bounce--;

        if (env.cgrid[beam[0].bx][beam[0].by] != CNG)   // hit a cloud

        {

            if (beam[0].beam_name[0] == '0' && beam[0].colour == DARKGREY)
            {
                env.cloud_type[env.cgrid[beam[0].bx][beam[0].by]] = random2(8) + 1;
            } /* polymorph randomly changes clouds in its path */

            if (beam[0].colour == 200 || beam[0].beam_name[0] == '0')
                goto out_of_cloud_bit;

            clouty = env.cgrid[beam[0].bx][beam[0].by];

/* 1 = fire, 3 = cold, 2 = stinking */

            if ((env.cloud_type[clouty] == CLOUD_COLD && beam[0].flavour == BEAM_FIRE) || (env.cloud_type[clouty] == CLOUD_FIRE && beam[0].flavour == BEAM_COLD))
            {
                if (see_grid(beam[0].bx, beam[0].by) == 1)
                {
                    mpr("You hear a sizzling sound!");
                }

                env.cloud_type[clouty] = CLOUD_NONE;
                env.cgrid[env.cloud_x[clouty]][env.cloud_y[clouty]] = CNG;
                env.cloud_decay[clouty] = 0;
                env.cloud_no--;

                n += 5;

            }

        }

out_of_cloud_bit:
        if (beam[0].bx == you.x_pos && beam[0].by == you.y_pos && beam[0].colour != BROWN)
        {                       // ^^^ digging
            // have to do something about enchantments here.
            if (beam[0].colour == 200)
            {
                beam[0].tracer = 1;
                goto out_of_hit_you;
            }

/*      if (beam[0].flavour == 10)
   {
   explosion1(beam);
   beam[0].aim_down = 0;
   return;
   }
 */

            if (beam[0].beam_name[0] != '0')     // ie enchantments always hit

            {
                if (player_light_armour() && random2(2) == 0
                                && beam[0].move_x != 0 && beam[0].move_y != 0)
                    exercise(SK_DODGING, 1);

                if (you.duration[DUR_REPEL_MISSILES] != 0
                                    || you.mutation[MUT_REPULSION_FIELD] == 3)
                    beam[0].hit -= random2(beam[0].hit / 2);

                if (you.duration[DUR_DEFLECT_MISSILES] != 0)
                    beam[0].hit = random2(beam[0].hit / 2);

                if (beam[0].hit < random40(player_evasion())
                                            + random2(you.dex) / 3 - 2
                            && (beam[0].move_x != 0 || beam[0].move_y != 0))
                {
                    strcpy(info, "The ");
                    strcat(info, beam[0].beam_name);
                    strcat(info, " misses you.");
                    mpr(info);
                    continue;
                }
            }




            if (beam[0].beam_name[0] == '0')
            {
                beam[0].aim_down = 0;
                if (beam[0].colour != BLUE && beam[0].colour != MAGENTA && beam[0].colour != GREEN && ((beam[0].colour != LIGHTGREY && beam[0].colour != LIGHTGREEN) || beam[0].move_x != 0 || beam[0].move_y != 0))
// if ((beam[0].thing_thrown == 1 || beam[0].thing_thrown == 3) && you_resist_magic(beam[0].ench_power) == 1)
                    if (you_resist_magic(beam[0].ench_power) == 1)
                    {
                        strcpy(info, "You resist.");
                        mpr(info);
                        return;
                    }
                switch (beam[0].colour)
                {
                case BLACK:
                    potion_effect(POT_SLOWING, beam[0].ench_power);
                    beam[0].wand_id = 1;
                    return;     // slow

                case BLUE:
                    potion_effect(POT_SPEED, beam[0].ench_power);
                    beam[0].wand_id = 1;
                    return;     // haste

                case GREEN:
                    potion_effect(POT_HEAL_WOUNDS, beam[0].ench_power);
                    beam[0].wand_id = 1;
                    return;     // heal (heal wounds potion eff)

                case CYAN:
                    potion_effect(POT_PARALYSIS, beam[0].ench_power);
                    beam[0].wand_id = 1;
                    return;     // paralysis

                case RED:
                    potion_effect(POT_CONFUSION, beam[0].ench_power);
                    beam[0].wand_id = 1;
                    return;     // confusion

                case MAGENTA:
                    potion_effect(POT_INVISIBILITY, beam[0].ench_power);
                    beam[0].wand_id = 1;
                    return;     // invisibility
                    // 6 is used by digging

                case LIGHTGREY:
                    you_teleport();
                    beam[0].wand_id = 1;
                    return;
                case DARKGREY:
                    strcpy(info, "This is polymorph other only!");
                    mpr(info);
                    beam[0].wand_id = 1;
                    return;
                case LIGHTBLUE:
                    potion_effect(POT_CONFUSION, beam[0].ench_power);
                    beam[0].wand_id = 1;
                    return;     // enslavement - confusion?

                case LIGHTGREEN:
                    if (you.level_type == 2)
                    {
                        mpr("You feel trapped.");
                        break;
                    }
                    mpr("You are cast into the Abyss!");
                    more();
                    banished(96);
                    beam[0].wand_id = 1;
                    return;     // banishment to the abyss

                case LIGHTMAGENTA:        // pain

                    if (you.is_undead != 0 || you.mutation[MUT_TORMENT_RESISTANCE] != 0)
                    {
                        strcpy(info, "You are unaffected.");
                        mpr(info);
                        return;
                    }
                    strcpy(info, "Pain shoots through your body!");
                    mpr(info);
                    strcpy(beam[0].beam_name, "spell");
                    if (YOU_KILL(beam[0].thing_thrown))
                        ouch(random2(beam[0].hit), 0, KILLED_BY_TARGETTING);
                    else
                        ouch(random2(beam[0].hit), beam[0].beam_source, KILLED_BY_BEAM);
                    beam[0].wand_id = 1;
                    return;
                case WHITE:
                    mpr("You are blasted!");
                    strcpy(beam[0].beam_name, "spell");
                    if (YOU_KILL(beam[0].thing_thrown))
                        ouch(random2(beam[0].hit), 0, KILLED_BY_TARGETTING);
                    else
                        ouch(random2(beam[0].hit), beam[0].beam_source, KILLED_BY_BEAM);
                    beam[0].wand_id = 1;
                    return;



                }               // end of switch (beam[0].colour)

            }                   // end of if beam[0].beam_name [0] == 48



            strcpy(info, "The ");
            strcat(info, beam[0].beam_name);
            strcat(info, " hits you");

            hurted = 0;

            if (beam[0].damage > 100)
            {
                hurted += random2(beam[0].damage - 100);
                hurted += random2(beam[0].damage - 100);
                hurted += random2(beam[0].damage - 100);
            }
            else
                hurted += random2(beam[0].damage);

            hurted -= random2(player_AC() + 1);
            // don't bother with + 1 here
            hurted -= random2(player_shield_class());
            you.shield_blocks++;

            // shrapnel
            if (beam[0].flavour == BEAM_FRAG )
            {
                hurted -= random2(player_AC() + 1);
                hurted -= random2(player_AC() + 1);
            }

            strcat(info, "!");
            mpr(info);

            if (you.equip[EQ_SHIELD] != -1)
                if (beam[0].move_x != 0 || beam[0].move_y != 0)
                    exercise(SK_SHIELDS, (random2(3)) / 2);

            if (you.equip[EQ_BODY_ARMOUR] != -1)
                if (random2(1000) <= mass(OBJ_ARMOUR, you.inv_type[you.equip[EQ_BODY_ARMOUR]]) && random2(4) == 0)
                    exercise(SK_ARMOUR, 1);

            if (hurted <= 0)
                hurted = 0;


            hurted = check_your_resists(hurted, beam[0].flavour);

/*      check_your_resists(); */
/*      strcat(info, "!");
   if (strcmp(info, "How terrible") != 0) mpr(info); */

            if (beam[0].flavour == BEAM_FIRE || stricmp(beam[0].beam_name, "hellfire") == 0)
                scrolls_burn(3, 6);     // also above

            if (beam[0].flavour == BEAM_COLD)
                scrolls_burn(3, 8);

            if (YOU_KILL(beam[0].thing_thrown))
                ouch(hurted, 0, KILLED_BY_TARGETTING);
            else
                ouch(hurted, beam[0].beam_source, KILLED_BY_BEAM);

            you.redraw_hit_points = 1;

            n++;                // reduces beam's range

            if (beam[0].flavour != BEAM_ELECTRICITY)
                n += random2(4) + 2;    /* If it isn't lightning, reduce range by a lot */

            if (beam[0].flavour == BEAM_EXPLOSION)
            {
                explosion1(beam);
                return;
            }

            if (beam[0].move_x == 0 && beam[0].move_y == 0)
                break;

            goto check_aimed;
        }

out_of_hit_you:
        if (beam[0].target_x == beam[0].bx && beam[0].target_y == beam[0].by && beam[0].aim_down == 1 && ((grd[beam[0].bx][beam[0].by] == DNGN_LAVA && beam[0].flavour == BEAM_COLD) || ((grd[beam[0].bx][beam[0].by] == DNGN_DEEP_WATER || grd[beam[0].bx][beam[0].by] == DNGN_SHALLOW_WATER) && beam[0].flavour == BEAM_FIRE)))
            place_cloud(CLOUD_STEAM, beam[0].bx, beam[0].by, 2 + random2(5));     // steam

        if (mgrd[beam[0].bx][beam[0].by] != MNG && (beam[0].colour != BROWN || beam[0].beam_name[0] != '0'))
        {
            o = mgrd[beam[0].bx][beam[0].by];

            if (beam[0].beam_name[0] == '0')
                goto enchanting;
            if (beam[0].beam_name[0] != '0')
            {
// tracer = 0 = run out of range
                // tracer = 1 = hits you in range
                // tracer_mons = 0 = hits no monsters
                //       "     = 1 = hits monster before you (possibly also after)
                //       "     = 2 = hits monster after but not before
                //       "     = 3 = hits tamed monster(s) but no others
                //       "     = 4 = hits tamed monster(s) + possibly other things

                if (menv[o].type >= MLAVA0 && menv[o].number == 1 && (beam[0].bx != beam[0].target_x || beam[0].by != beam[0].target_y || beam[0].aim_down != 1))
                    goto check_aimed;

                if (beam[0].beam_name[0] != '0')         // ie enchantments always hit

                {
                    if (beam[0].hit < random2(menv[o].evasion))
                    {
                        strcpy(info, "The ");
                        strcat(info, beam[0].beam_name);
                        strcat(info, " misses ");
                        strcat(info, monam(menv[o].number, menv[o].type, menv[o].enchantment[2], 1));   //gmon_name [menv_class [o]]);

                        strcat(info, ".");
                        mpr(info);
                        goto check_aimed;
                    }
                }



                strcpy(info, "The ");
                strcat(info, beam[0].beam_name);
                strcat(info, " hits ");
                strcat(info, monam(menv[o].number, menv[o].type, menv[o].enchantment[2], 1));   //gmon_name [mons_class [o]]);

                strcat(info, ".");
                mpr(info);

                hurted = 0;

                if (beam[0].damage > 100)
                {
                    hurted += random2(beam[0].damage - 100);
                    hurted += random2(beam[0].damage - 100);
                    hurted += random2(beam[0].damage - 100);
                }
                else
                    hurted += random2(beam[0].damage);

                hurted -= random2(menv[o].armor_class + 1);
                if (beam[0].flavour == BEAM_FRAG)      // shrapnel

                {
                    hurted -= random2(menv[o].armor_class + 1);
                    hurted -= random2(menv[o].armor_class + 1);
                }
                if (hurted <= 0)
                    hurted = 0;
                hurted = check_mons_resists(beam, o, hurted);

                menv[o].hit_points -= hurted;


//if ((beam[0].thing_thrown == 1 || beam[0].thing_thrown == 3) && mons_holiness(menv [o].type) < 0 && hurted > 0) naughty(15, 4);

                if (menv[o].behavior == 0)
                    menv[o].behavior = 1;
                if (YOU_KILL(beam[0].thing_thrown) && menv[o].behavior == BEH_ENSLAVED && hurted > 0)
                {
                    menv[o].behavior = BEH_CHASING_I;
//   naughty(8, 5);
                }
                if (menv[o].hit_points <= 0)
                {
                    switch(beam[0].thing_thrown)
                    {
                    case KILL_YOU:     /* your beam */
                    case KILL_YOU_MISSILE:
                        monster_die(o, KILL_YOU_MISSILE, 0);
                        break;  /*  "    " */
                    case KILL_MON:
                        monster_die(o, KILL_MON_MISSILE, beam[0].beam_source);
                        break;  /* dragon breath &c */
                    }
                }
                else
                {
                    if (YOU_KILL(beam[0].thing_thrown) && mons_near(o))
                        print_wounds(o);
                    if (menv[o].type >= 389 && menv[o].type <= 393)
                        mimic_alert(o);
                }

                if (beam[0].flavour == BEAM_EXPLOSION && strcmp(beam[0].beam_name, "hellfire") != 0)
                {
                    explosion1(beam);
                    beam[0].aim_down = 0;
                    return;
                }
                if (beam[0].flavour != BEAM_ELECTRICITY)
                    n += random2(5) + 3;        /* If it isn't lightning, reduce range by a lot */

            }



          enchanting:if (menv[o].x == beam[0].bx && menv[o].y == beam[0].by && menv[o].type != -1 && beam[0].beam_name[0] == '0')
            {

                if (beam[0].colour == 200)
                {
                    if (beam[0].trac_targ != MHITYOU)
                    {


                        if (menv[o].behavior == 7 && menv[beam[0].beam_source].behavior == 7)
                        {
                            beam[0].tracer_mons = 4;
                            return;     //goto check_aimed;

                        }

                        if (menv[o].behavior == 7 && beam[0].tracer_mons == 0)
                        {
                            beam[0].tracer_mons = 3;
                            beam[0].trac_hit_tamed = 1;
                            return;
                            //goto check_aimed;
                        }

                        if (o == menv[beam[0].beam_source].monster_foe)
                        {
                            beam[0].trac_hit_mons = 1;
                            return;
                            //goto check_aimed;
                        }

                        if (beam[0].tracer_mons == 0)
                        {
                            beam[0].tracer_mons = 1;    //3;

                            return;
                            //goto check_aimed;
                        }
                        if (beam[0].trac_hit_mons == 1 && beam[0].tracer_mons != 1)
                            beam[0].tracer_mons = 2;    // else

                        if (beam[0].trac_hit_mons == 0)
                            beam[0].tracer_mons = 1;
                    }
                    else        // Aimed at you:

                    {

                        if (beam[0].tracer == 1 && beam[0].tracer_mons != 1)
                            beam[0].tracer_mons = 2;    // else

                        if (beam[0].tracer == 0)
                            beam[0].tracer_mons = 1;
                    }
                    goto check_aimed;

                }


                if (you.invis != 0 && mons_see_invis(menv[0].type) != 1 && menv[o].behavior == 0)
                    menv[o].behavior = 1;

                if (beam[0].colour == LIGHTGREY)        //: // teleportation

                {
                    if (check_mons_magres(o, beam[0].ench_power) == 0 && (beam[0].move_x != 0 || beam[0].move_y))
                        goto it_resists;
                    if (mons_near(o) && menv[o].enchantment[2] != 6)
                    {
                        strcpy(info, monam(menv[o].number, menv[o].type, menv[o].enchantment[2], 0));
                        strcat(info, " looks slightly unstable.");
                        mpr(info);
                        beam[0].wand_id = 1;
                    }
                    monster_teleport(o, 0);
                    beam[0].aim_down = 0;
                    return;
                }

                if (beam[0].colour == DARKGREY)
                {
                    if (check_mons_magres(o, beam[0].ench_power) == 0)
                        goto it_resists;
                    monster_polymorph(o, 250, 100);
                    beam[0].aim_down = 0;
                    beam[0].wand_id = 1;
                    return;
                }

                if (beam[0].colour == LIGHTGREEN)
                {
                    if (check_mons_magres(o, beam[0].ench_power) == 0)
                        goto it_resists;
                    if (you.level_type == 2)
                    {
                        strcpy(info, monam(menv[o].number, menv[o].type, menv[o].enchantment[2], 0));
                        strcat(info, " wobbles for a moment.");
                        mpr(info);
                    }
                    else
                        monster_die(o, KILL_RESET, beam[0].beam_source);
                    beam[0].aim_down = 0;
                    beam[0].wand_id = 1;
                    return;
                }

                if (beam[0].colour == LIGHTCYAN)
                {
                    if (mons_holiness(menv[o].type) != 0 || menv[o].type == 131)
                        goto unaffected;
                    if (check_mons_magres(o, beam[0].ench_power) == 0)
                        goto it_resists;
                    monster_polymorph(o, 131, 100);
                    beam[0].aim_down = 0;
                    beam[0].wand_id = 1;
                    return;
                }

                if (beam[0].colour == YELLOW)
                {
                    if (mons_holiness(menv[o].type) != 1)
                    {
                      unaffected:strcpy(info, monam(menv[o].number, menv[o].type, menv[o].enchantment[2], 0));
                        strcat(info, " is unaffected.");
                        mpr(info);
                        beam[0].aim_down = 0;
                        return;
                    }
                    strcpy(info, monam(menv[o].number, menv[o].type, menv[o].enchantment[2], 0));
                    strcat(info, " convulses!");
                    mpr(info);
                    menv[o].hit_points -= random2(beam[0].hit);
                    menv[o].hit_points -= random2(beam[0].hit);
                    menv[o].hit_points -= random2(beam[0].hit);
                    strcpy(beam[0].beam_name, "spell");
                    char killer = 0;

                    switch (beam[0].thing_thrown)
                    {
                    case KILL_YOU:
                        killer = KILL_YOU_MISSILE;
                        break;  // your beam

                    case KILL_YOU_MISSILE:
                        killer = KILL_YOU_MISSILE;
                        break;  //  "    "

                    case KILL_MON:
                        killer = KILL_MON_MISSILE;
                        break;  // dragon breath &c

                    }
                    if (menv[o].hit_points <= 0)
                        monster_die(o, killer, beam[0].beam_source);
                    else
                    {
                        print_wounds(o);
                        if (menv[o].type >= 389 && menv[o].type <= 393)
                            mimic_alert(o);
                    }
                    beam[0].aim_down = 0;
                    return;
                }

                if (check_mons_magres(o, beam[0].ench_power) == 0 && beam[0].colour != BLUE && beam[0].colour != GREEN && beam[0].colour != MAGENTA)
                {
                  it_resists:strcpy(info, monam(menv[o].number, menv[o].type, menv[o].enchantment[2], 0));
                    strcat(info, " resists.");
                    mpr(info);
                    beam[0].aim_down = 0;
                    return;
                }


                if (beam[0].colour == LIGHTMAGENTA)       /* pain/agony */
                {
                    if (mons_holiness(menv[o].type) > 0)
                        goto it_resists;
                    strcpy(info, monam(menv[o].number, menv[o].type, menv[o].enchantment[2], 0));
                    strcat(info, " convulses in agony!");
                    mpr(info);
                    if (strstr(beam[0].beam_name, "agony") != NULL)
                    {
                        menv[o].hit_points = menv[o].hit_points / 2;
                        if (menv[o].hit_points <= 1)
                            menv[o].hit_points = 1;
                        strcpy(beam[0].beam_name, "agony");
                    }
                    else
                    {
                        menv[o].hit_points -= random2(beam[0].hit);
                        strcpy(beam[0].beam_name, "pain");
                    }
                    char killer = 0;

                    switch (beam[0].thing_thrown)
                    {
                    case KILL_YOU:
                        killer = KILL_YOU_MISSILE;
                        break;  // your beam

                    case KILL_YOU_MISSILE:
                        killer = KILL_YOU_MISSILE;
                        break;  //  "    "

                    case KILL_MON:
                        killer = KILL_MON_MISSILE;
                        break;  // dragon breath &c

                    }
                    if (menv[o].hit_points <= 0)
                        monster_die(o, killer, beam[0].beam_source);
                    else
                    {
                        print_wounds(o);
                        if (menv[o].type >= 389 && menv[o].type <= 393)
                            mimic_alert(o);
                    }
                    beam[0].aim_down = 0;
                    return;
                }

                if (beam[0].colour == WHITE)       /* disrupt/disintegrate */
                {
                    strcpy(info, monam(menv[o].number, menv[o].type, menv[o].enchantment[2], 0));
                    strcat(info, " is blasted.");
                    mpr(info);
                    menv[o].hit_points -= random2(beam[0].hit + 1);
                    strcpy(beam[0].beam_name, "spell");
                    char killer = 0;

                    switch (beam[0].thing_thrown)
                    {
                    case KILL_YOU:
                        killer = KILL_YOU_MISSILE;
                        break;  // your beam

                    case KILL_YOU_MISSILE:
                        killer = KILL_YOU_MISSILE;
                        break;  //  "    "

                    case KILL_MON:
                        killer = KILL_MON_MISSILE;
                        break;  // dragon breath &c

                    }
                    if (menv[o].hit_points <= 0)
                        monster_die(o, killer, beam[0].beam_source);
                    else
                    {
                        print_wounds(o);
                        if (menv[o].type >= 389 && menv[o].type <= 393)
                            mimic_alert(o);
                    }
                    beam[0].aim_down = 0;
                    beam[0].wand_id = 1;
                    return;
                }

                int func_pass[10];

                if ((beam[0].colour == LIGHTRED && mons_holiness(menv[o].type) == 1) || (beam[0].colour == 16 && mons_holiness(menv[o].type) == 2))
                {
                    strcpy(info, monam(menv[o].number, menv[o].type, menv[o].enchantment[2], 0));
                    strcat(info, " is enslaved.");
                    mpr(info);
                    menv[o].behavior = 7;
                    beam[0].wand_id = 1;
                    return;
                }
                if (beam[0].colour == LIGHTRED)
                {
                    goto unaffected;
                }

                mons_ench_f2(o, mons_near(o), func_pass, beam);

                beam[0].wand_id = func_pass[1];
                beam[0].aim_down = 0;
                return;

            }                   // end of if ...

        }

check_aimed:
        if (beam[0].bx == beam[0].target_x && beam[0].by == beam[0].target_y && beam[0].aim_down == 1)
        {
            goto landed;
        }

        if (beam[0].bx == beam[0].target_x && beam[0].by == beam[0].target_y && (beam[0].flavour == BEAM_EXPLOSION || beam[0].flavour == BEAM_HOLY))
        {
            explosion1(beam);
            beam[0].aim_down = 0;
            return;
        }



    }                           // end of for n

landed:
    // had if beam == tracer
    if (beam[0].bx == beam[0].target_x && beam[0].by == beam[0].target_y && beam[0].aim_down == 1)
    {
        beam[0].aim_down = 0;
        return;
    }

    if (beam[0].flavour == BEAM_EXPLOSION)
    {
        explosion1(beam);
        beam[0].aim_down = 0;
        return;
    }


    if (beam[0].beam_name[0] == '0' && beam[0].colour != BROWN && beam[0].colour != 200)
    {
        strcpy(info, "Nothing appears to happen.");
        mpr(info);
    }

    beam[0].aim_down = 0;
/* ^ must have! */

}                               // end of void beam(void);



/* tracer = 0 = run out of range or hits a wall
   tracer = 1 = hits you in range
   tracer = 2 = hits friendly monster
   tracer = 3 = hits hostile monster

   tracer_mons = 1 = hits monster specified in trac_targ

   tracer_hit_mons now holds value of mons_see_invis of attacking monster.
   If it's 0, won't register invis monsters or you
 */

/*
   Similar to beam(), but handles single-target missiles like sticky flame or
   orb of electrocution.

   throw_2 is the item, either in your invent or in env for a monster.
 */
void missile(struct bolt beam[1], int throw_2)
{
    int beam_sign_x = 0;
    int beam_sign_y = 0;
    float bmoy = 0;
    int bmax = 0;
    float bmay = 0;

    beam[0].bx = beam[0].source_x;
    beam[0].by = beam[0].source_y;
    int bmhit = beam[0].hit;

/*brek = 1;
   brek = 0; */

/*beam[0].tracer = 0; */

    int hurted = 0;

    if (beam[0].move_x > 1 || beam[0].move_y > 1 || beam[0].move_x < -1 || beam[0].move_y < -1)
    {

        if (beam[0].move_x > 0)
            beam_sign_x = 1;
        if (beam[0].move_x < 0)
        {
            beam_sign_x = -1;
            beam[0].move_x *= -1;
        }

        if (beam[0].move_y > 0)
            beam_sign_y = 1;
        if (beam[0].move_y < 0)
        {
            beam_sign_y = -1;
            beam[0].move_y *= -1;
        }

        if (beam[0].move_x > beam[0].move_y)
        {
            if (beam_sign_x == 1)
                bmay = (float) (beam[0].move_y) / (beam[0].move_x + 1);
            if (beam_sign_x == -1)
                bmay = (float) (beam[0].move_y) / (beam[0].move_x + 1);
            bmax = 1;
        }
        else
        {
            if (beam_sign_y == 1)
                bmay = (float) (beam[0].move_x) / (beam[0].move_y + 1);
            if (beam_sign_y == -1)
                bmay = (float) (beam[0].move_x) / (beam[0].move_y + 1);
            bmax = 2;
        }
        bmoy = 1;



    }

    int n;

    for (n = 1; n < beam[0].range; n++)
    {

        if (beam[0].beam_name[0] == '0' && beam[0].flavour != BEAM_MAGIC)
        {
            return;
        }


        beam[0].hit = bmhit;


        if (bmax == 1)
        {
            beam[0].move_x = beam_sign_x;
            bmoy += bmay;
            if (bmoy >= 1)
            {
                beam[0].move_y = beam_sign_y;
                bmoy -= 1;
            }
            else
                beam[0].move_y = 0;
        }



        if (bmax == 2)
        {
            beam[0].move_y = beam_sign_y;
            bmoy += bmay;
            if (bmoy >= 1)
            {
                beam[0].move_x = beam_sign_x;
                bmoy -= 1;
            }
            else
                beam[0].move_x = 0;
        }

        if (grd[beam[0].bx + beam[0].move_x][beam[0].by + beam[0].move_y] >= MINMOVE)
        {
            beam[0].bx += beam[0].move_x;
            beam[0].by += beam[0].move_y;

            if (beam[0].bx == beam[0].target_x && beam[0].by == beam[0].target_y)
            {
                if (beam[0].flavour == BEAM_EXPLOSION)
                {
                    if (beam[0].thing_thrown == KILL_YOU_MISSILE)
                        beam[0].thing_thrown = KILL_YOU;
                    if (beam[0].thing_thrown == KILL_MON_MISSILE)
                        beam[0].thing_thrown = KILL_MON;
                    explosion1(beam);
                    beam[0].aim_down = 0;
                    return;
                }

                if (strcmp(beam[0].beam_name, "orb of electricity") == 0 || strcmp(beam[0].beam_name, "metal orb") == 0 || strcmp(beam[0].beam_name, "great blast of cold") == 0)
                {
                    explosion1(beam);
                    beam[0].aim_down = 0;
                    return;
                }

                if (beam[0].flavour == BEAM_CLOUD)       // cloud

                {
                    beam[0].aim_down = 0;
                    return;
                }

            }

            if (strcmp(beam[0].beam_name, "orb of energy") == 0)
            {
                place_cloud(CLOUD_ENERGY, beam[0].bx, beam[0].by, random2(5) + 1);
            }

            if (strcmp(beam[0].beam_name, "great blast of cold") == 0)
            {
                place_cloud(CLOUD_COLD, beam[0].bx, beam[0].by, random2(5) + 3);
            }

            if (strcmp(beam[0].beam_name, "ball of steam") == 0)
            {
                place_cloud(CLOUD_STEAM, beam[0].bx, beam[0].by, random2(5) + 1);
            }

            if (strcmp(beam[0].beam_name, "sticky flame") == 0)
            {
                place_cloud(CLOUD_STICKY_FLAME, beam[0].bx, beam[0].by, random2(4) + 1);
            }

            if (strcmp(beam[0].beam_name, "poison gas") == 0)
            {
                if (beam[0].thing_thrown != KILL_MON)
                    place_cloud(CLOUD_POISON, beam[0].bx, beam[0].by, random2(4) + 2);
                else
                    place_cloud(CLOUD_POISON_MON, beam[0].bx, beam[0].by, random2(4) + 2);
            }

            if (strcmp(beam[0].beam_name, "foul vapour") == 0 && beam[0].bx == beam[0].target_x && beam[0].by == beam[0].target_y)
            {
                if (beam[0].thing_thrown != KILL_MON)
                    big_cloud(2, beam[0].bx, beam[0].by, 0);
                else
                    big_cloud(102, beam[0].bx, beam[0].by, 0);
                return;
            }


/*if (beam[0].bx > you.x_pos - 9 && beam[0].bx < you.x_pos + 9 && beam[0].by > you.y_pos - 9 && beam[0].by < you.y_pos + 9 && show [beam[0].bx - you.x_pos + 9] [beam[0].by - you.y_pos + 9] != 0) */
            if (beam[0].colour != 200)  /* tracer doesn't update the screen */
                if (beam[0].bx > you.x_pos - 9 && beam[0].bx < you.x_pos + 9 && beam[0].by > you.y_pos - 9 && beam[0].by < you.y_pos + 9)
                {
#ifdef DOS_TERM
                    viewwindow(1, false);
#endif
                    textcolor(beam[0].colour);
                    gotoxy(beam[0].bx - you.x_pos + 18, beam[0].by - you.y_pos + 9);
                    putch(beam[0].type);
#ifdef USE_CURSES
                    // If we don't refresh curses we won't
                    // guarantee that the beam is visible
                    refresh();
#endif
                    delay(15);
                }

            if (beam[0].bx == you.x_pos && beam[0].by == you.y_pos)
            {
                /* I find it more logical that there can be virtually unavoidable missiles than missiles that can almost always be avoided. */

                if (beam[0].colour == 200)      /* tracer */
                {
                    if (beam[0].trac_hit_mons == 1 || you.invis == 0)
                    {
                        beam[0].tracer = 1;
                        return;
                    }
                }
                else
                {
                    /* Start of : if it's not a tracer */
                    if (you.equip[EQ_SHIELD] != -1)
                        if (beam[0].move_x != 0 || beam[0].move_y != 0)
                            exercise(SK_SHIELDS, random2(2));

                    if (player_shield_class() > 0
                            && random2(beam[0].hit * 5 + 5 * you.shield_blocks)
                                        <= random2(player_shield_class()) + (random2(you.dex) / 5) - 1)
                    {
                        you.shield_blocks++;
                        strcpy(info, "You block the ");
                        strcat(info, beam[0].beam_name);
                        strcat(info, ".");
                        mpr(info);
                        if (beam[0].flavour == BEAM_EXPLOSION)
                        {
                            explosion1(beam);
                            beam[0].aim_down = 0;
                            return;
                        }
                        if (strcmp(beam[0].beam_name, "orb of electricity") == 0 || strcmp(beam[0].beam_name, "metal orb") == 0 || strcmp(beam[0].beam_name, "great blast of cold") == 0)
                        {
                            explosion1(beam);
                            beam[0].aim_down = 0;
                            return;
                        }
                        break;
                    }           // end of block

                    if (player_light_armour() && random2(2) == 0
                                && beam[0].move_x != 0 && beam[0].move_y != 0)
                        exercise(SK_DODGING, 1);

                    if (you.duration[DUR_REPEL_MISSILES] != 0 || you.mutation[MUT_REPULSION_FIELD] == 3)
                        beam[0].hit = random2(beam[0].hit);

                    if (beam[0].hit >= random40(player_evasion())
                                                + random2(you.dex) / 3 - 2
                                    && you.duration[DUR_DEFLECT_MISSILES] == 0)
                    {

                        strcpy(info, "The ");
                        strcat(info, beam[0].beam_name);
                        strcat(info, " hits you");
/*      if (beam[0].thing_thrown == 3 || beam[0].thing_thrown == 4)
   {
   if (beam[0].bx > you.x_pos - 7 && beam[0].bx < you.x_pos + 7 && beam[0].by > you.y_pos - 7 && beam[0].by < you.y_pos + 7)
   {
   viewwindow(1);
   textcolor(beam[0].colour);
   gotoxy(beam[0].bx - you.x_pos + 18, beam[0].by - you.y_pos + 9);
   putch(35);
   }
   } */

                        hurted = 0;
                        if (beam[0].damage > 100)
                        {
                            hurted += random2(beam[0].damage - 100);
                            hurted += random2(beam[0].damage - 100);
                            hurted += random2(beam[0].damage - 100);
                        }
                        else
                            hurted += random2(beam[0].damage);

                        strcat(info, "!");
                        mpr(info);

                        hurted = check_your_resists(hurted, beam[0].flavour);

                        if (strstr(beam[0].beam_name, "poison") != NULL && beam[0].flavour != BEAM_POISON && player_res_poison() == 0 && random2(hurted) - random2(player_AC()) > 0)
                        {
                            mpr("You are poisoned.");
                            you.poison += 1 + random2(3);
                        }

                        if (beam[0].flavour == BEAM_LAVA
                            || (beam[0].flavour == BEAM_FIRE
                            && strcmp(beam[0].beam_name, "ball of steam") != 0)
                            || stricmp(beam[0].beam_name, "hellfire") == 0)
                        {
                            scrolls_burn(2, 6);
                        }

                        if (beam[0].flavour == BEAM_COLD)
                            scrolls_burn(2, 8);

                        hurted -= random2(player_AC() + 1);
                        if (hurted <= 0)
                            hurted = 0;


                        if (you.equip[EQ_BODY_ARMOUR] != -1)
                            if (random2(1000) <= mass(OBJ_ARMOUR, you.inv_type[you.equip[EQ_BODY_ARMOUR]]) && random2(4) == 0)
                                exercise(SK_ARMOUR, 1);

                        if (YOU_KILL(beam[0].thing_thrown))
                            ouch(hurted, 0, KILLED_BY_TARGETTING);
                        else
                            ouch(hurted, beam[0].beam_source, KILLED_BY_BEAM);

                        you.redraw_hit_points = 1;

                        if (strcmp(beam[0].beam_name, "sticky flame") == 0 && (you.species != SP_MOTTLED_DRACONIAN || you.experience_level < 6))
                        {
                            if (you.equip[EQ_BODY_ARMOUR] == -1 || you.inv_type[you.equip[EQ_BODY_ARMOUR]] != 25)
                                you.duration[DUR_LIQUID_FLAMES] += 1 + random2(3) + random2(3) + random2(3);
                        }

                        break;
                    }
                    else
                    {
                        strcpy(info, "The ");
                        strcat(info, beam[0].beam_name);
                        strcat(info, " misses you!");
                        mpr(info);
                        if (beam[0].move_x == 0 && beam[0].move_y == 0)
                            break;
                    }


                }               /* end of if not tracer */



                if (beam[0].flavour == BEAM_EXPLOSION)
                {
                    explosion1(beam);
                    beam[0].aim_down = 0;
                    return;
                }
                if (strcmp(beam[0].beam_name, "orb of electricity") == 0 || strcmp(beam[0].beam_name, "metal orb") == 0 || strcmp(beam[0].beam_name, "great blast of cold") == 0)
                {
                    explosion1(beam);
                    beam[0].aim_down = 0;
                    return;
                }


            }

            char count_x = 0;

            if (mgrd[beam[0].bx][beam[0].by] != MNG)
            {
                count_x = 0;

                /* this won't display a "you miss it" if you throw an beam[0].aim_down thing at a lava worm, only if you hit. */
                int o = mgrd[beam[0].bx][beam[0].by];

                if (beam[0].colour == 200)      /* tracer */
                {
                    if (menv[o].type < MLAVA0 || menv[o].number == 0)
                    {
                        if (beam[0].trac_hit_mons == 1 || menv[o].enchantment[2] != 6)
                        {
                            if (o == beam[0].trac_targ)
                                beam[0].tracer_mons = 1;
                            if (menv[o].behavior == 7)
                                beam[0].tracer = 2;
                            else
                                beam[0].tracer = 3;
                            return;
                        }
                    }
                }
                else
                    /* Start of : It's not a tracer */
                {

                    if (beam[0].hit >= random2(menv[o].evasion) && ((menv[o].type < MLAVA0 || menv[o].number == 0) || (beam[0].bx == beam[0].target_x && beam[0].by == beam[0].target_y && beam[0].aim_down == 1)))
                    {
                        hurted = 0;

                        if (beam[0].damage > 100)
                        {
                            hurted += random2(beam[0].damage - 100);
                            hurted += random2(beam[0].damage - 100);
                            hurted += random2(beam[0].damage - 100);
                        }
                        else
                            hurted += random2(beam[0].damage);

                        if (mons_near(o))
                        {
                            strcpy(info, "The ");
                            strcat(info, beam[0].beam_name);
                            strcat(info, " hits ");
                            strcat(info, monam(menv[o].number, menv[o].type, menv[o].enchantment[2], 1));       //gmon_name [mons_class [o]]);

                            strcat(info, ".");
                            mpr(info);
                        }

                        hurted = check_mons_resists(beam, o, hurted);

                        menv[o].hit_points -= hurted;

                        if (YOU_KILL(beam[0].thing_thrown) && menv[o].behavior == BEH_ENSLAVED && hurted > 0)
                        {
//   naughty(8, 5);
                            menv[o].behavior = 1;
                        }
//  if ((beam[0].thing_thrown == 1 || beam[0].thing_thrown == 3) && mons_holiness(menv [o].type) < 0 && hurted > 0) naughty(15, 4);
                        if (YOU_KILL(beam[0].thing_thrown) && mons_near(o))
                            print_wounds(o);
                        if (menv[o].type >= 389 && menv[o].type <= 393)
                            mimic_alert(o);

                        if (menv[o].behavior == 0)
                            menv[o].behavior = 1;


/*              if (beam[0].thing_thrown == 3 || beam[0].thing_thrown == 4)
   {
   if (beam[0].bx > you.x_pos - 7 && beam[0].bx < you.x_pos + 7 && beam[0].by > you.y_pos - 7 && beam[0].by < you.y_pos + 7)
   {
   viewwindow(1);
   textcolor(beam[0].colour);
   gotoxy(beam[0].bx - you.x_pos + 18, beam[0].by - you.y_pos + 9);
   putch(35);
   }
   } */
                        if (menv[o].hit_points <= 0)
                        {
                            switch (beam[0].thing_thrown)
                            {
                            case KILL_YOU:     /* You threw */
                            case KILL_YOU_MISSILE:
                                monster_die(o, KILL_YOU_MISSILE, beam[0].beam_source);
                                break;  /* You cast a missile spell which doesn't leave an item. */
                            case KILL_MON:     /* mons threw */
                            case KILL_MON_MISSILE:
                                monster_die(o, KILL_MON_MISSILE, beam[0].beam_source);
                                break;  /* mons cast a missile spell which doesn't leave an item. */

                            }
                        }
                        else
                        {
                            if (strcmp(beam[0].beam_name, "sticky flame") == 0)
                                sticky_flame_monster(o, 0, hurted);

                            /* looks for missiles which aren't poison but are poison*ed* */
                            if (strstr(beam[0].beam_name, "poison") != NULL && beam[0].flavour != BEAM_POISON && random2(hurted) - random2(menv[o].armor_class) > 0)
                                 poison_monster(o,!YOU_KILL(beam[0].thing_thrown));
                        }
                        count_x = 1;

                        if (beam[0].flavour == BEAM_EXPLOSION)
                        {
                            explosion1(beam);
                            beam[0].aim_down = 0;
                            return;
                        }

                        if (strcmp(beam[0].beam_name, "orb of electricity") == 0 || strcmp(beam[0].beam_name, "metal orb") == 0 || strcmp(beam[0].beam_name, "great blast of cold") == 0)
                        {
                            explosion1(beam);
                            beam[0].aim_down = 0;
                            return;
                        }


                        break;
                    }
                    else if (YOU_KILL(beam[0].thing_thrown) && mons_near(o) && (menv[o].type < MLAVA0 || menv[o].number == 0))  // No message if monster missile misses

                    {
                        strcpy(info, "The ");
                        strcat(info, beam[0].beam_name);
                        strcat(info, " misses ");
                        strcat(info, monam(menv[o].number, menv[o].type, menv[o].enchantment[2], 1));
                        strcat(info, ".");
                        mpr(info);

                        if (you.invis != 0 && mons_see_invis(menv[o].type) != 0 && menv[o].behavior == 0)
                            menv[o].behavior = 1;

                    }

                }               /* end of if not a tracer */

            }                   // end of if (mgrid ... != MNG)


            if (count_x == 1)
            {
                count_x = 0;
                break;
            }

        }                       // end of if.

        if (beam[0].bx == beam[0].target_x && beam[0].by == beam[0].target_y && beam[0].aim_down == 1)
        {
            goto landed;
        }


    }                           // end of for n loop

landed:
    if (beam[0].flavour == BEAM_EXPLOSION)
    {
        explosion1(beam);
        beam[0].aim_down = 0;
        return;
    }

    if (beam[0].colour == 200)
        return;

    if (strcmp(beam[0].beam_name, "orb of electricity") == 0 || strcmp(beam[0].beam_name, "metal orb") == 0 || strcmp(beam[0].beam_name, "great blast of cold") == 0)
    {
        explosion1(beam);
        beam[0].aim_down = 0;
        return;
    }


    if (beam[0].thing_thrown == KILL_YOU)      // ie if you threw it.
    {
        if (grd[beam[0].bx][beam[0].by] != 61 && grd[beam[0].bx][beam[0].by] != DNGN_DEEP_WATER)
        {
            if (you.inv_class[throw_2] != 1
                || random2((you.inv_type[throw_2] == MI_STONE) ? 3 : 2) != 0)
            {
                item_place(throw_2, beam[0].bx, beam[0].by, 1);
            }
        }

        if (throw_2 == you.equip[EQ_WEAPON])
        {
            you.equip[EQ_WEAPON] = -1;
            strcpy(info, "You are empty handed.");
            mpr(info);
        }
    }


    if (beam[0].thing_thrown == KILL_MON && (grd[beam[0].bx][beam[0].by] != DNGN_LAVA && grd[beam[0].bx][beam[0].by] != DNGN_DEEP_WATER) && random2(2) != 0)       // monster threw it.

    {

        if (mitm.quantity[throw_2] == 0)        // 0 instead of 1 because the mitm.quantity is reduced by 1 when it is thrown.

        {
        }
        else
        {

            if (igrd[beam[0].bx][beam[0].by] != 501)
            {
                if ((mitm.base_type[throw_2] == 1 || mitm.base_type[throw_2] == 4 || mitm.base_type[throw_2] == 6 || mitm.base_type[throw_2] == 8 || mitm.base_type[throw_2] == 9) && mitm.base_type[throw_2] == mitm.base_type[igrd[beam[0].bx][beam[0].by]] && mitm.sub_type[throw_2] == mitm.sub_type[igrd[beam[0].bx][beam[0].by]] && mitm.pluses[throw_2] == mitm.pluses[igrd[beam[0].bx][beam[0].by]] && mitm.pluses2[throw_2] == mitm.pluses2[igrd[beam[0].bx][beam[0].by]] && mitm.special[throw_2] == mitm.special[igrd[beam[0].bx][beam[0].by]])
                {
                    mitm.quantity[igrd[beam[0].bx][beam[0].by]]++;
                    beam[0].aim_down = 0;
                    return;
                }
            }                   // end of if igrd != 501

            int o = 0;

            for (o = 0; o < ITEMS; o++)
            {
                if (mitm.quantity[o] == 0)
                {
                    mitm.id[o] = mitm.id[throw_2];
                    mitm.base_type[o] = mitm.base_type[throw_2];
                    mitm.sub_type[o] = mitm.sub_type[throw_2];
                    mitm.pluses[o] = mitm.pluses[throw_2];
                    mitm.pluses2[o] = mitm.pluses2[throw_2];
                    mitm.special[o] = mitm.special[throw_2];
                    mitm.colour[o] = mitm.colour[throw_2];
                    mitm.quantity[o] = 1;
                    mitm.link[o] = 501;
                    /*                              it_no ++; */
                    break;
                }
            }                   // end of o loop

            if (igrd[beam[0].bx][beam[0].by] == 501)
            {
                igrd[beam[0].bx][beam[0].by] = o;
            }
            else
            {
                int hug = igrd[beam[0].bx][beam[0].by];

                igrd[beam[0].bx][beam[0].by] = o;
                mitm.link[o] = hug;
            }

        }                       // end of else

    }                           // if (thing_throw == 2) ...


    beam[0].aim_down = 0;
/* ^ must have this before every return in this function! */

}                               // end of missile()


/*
   Returns the damage taken by a monster from a "flavoured" (fire, ice etc)
   attack. Damage from clouds and branded weapons is handled elsewhere.
 */
int check_mons_resists(struct bolt beam[1], int o, int hurted)
{
    switch (beam[0].flavour)
    {
    case BEAM_FIRE:                     /* fire */
        if (mons_res_fire(menv[o].type) > 0)
        {
            if (mons_near(o) && menv[o].enchantment[2] != 6)
            {
                strcpy(info, monam(menv[o].number, menv[o].type, menv[o].enchantment[2], 0));
                strcat(info, " appears unharmed.");
                mpr(info);
            }
            hurted = 0;
        }
        else if (menv[o].inv[2] != 501 && mitm.special[menv[o].inv[2]] % 30 == 2)
        {
            if (mons_near(o) && menv[o].enchantment[2] != 6)
            {
                strcpy(info, monam(menv[o].number, menv[o].type, menv[o].enchantment[2], 0));
                strcat(info, " resists.");
                mpr(info);
            }
            hurted /= 3;
        }

        if (mons_res_fire(menv[o].type) == -1 && (menv[o].inv[2] == 501 || mitm.special[menv[o].inv[2]] % 30 != 2))
        {
            strcpy(info, monam(menv[o].number, menv[o].type, menv[o].enchantment[2], 0));
            strcat(info, " is burned terribly!");
            mpr(info);
            hurted *= 15;
            hurted /= 10;
        }
        break;


    case BEAM_COLD:                     /* cold */
        if (mons_res_cold(menv[o].type) > 0)
        {
            if (mons_near(o) && menv[o].enchantment[2] != 6)
            {
                strcpy(info, monam(menv[o].number, menv[o].type, menv[o].enchantment[2], 0));
                strcat(info, " appears unharmed.");
                mpr(info);
            }
            hurted = 0;
        }
        else if (menv[o].inv[2] != 501 && mitm.special[menv[o].inv[2]] % 30 == 3)
        {
            if (mons_near(o) && menv[o].enchantment[2] != 6)
            {
                strcpy(info, monam(menv[o].number, menv[o].type, menv[o].enchantment[2], 0));
                strcat(info, " resists.");
                mpr(info);
            }
            hurted /= 3;
        }

        if (mons_res_cold(menv[o].type) == -1 && (menv[o].inv[2] == 501 || mitm.special[menv[o].inv[2]] % 30 != 3))
        {
            strcpy(info, monam(menv[o].number, menv[o].type, menv[o].enchantment[2], 0));
            strcat(info, " is frozen!");
            mpr(info);
            hurted *= 15;
            hurted /= 10;
        }
        break;

    case BEAM_ELECTRICITY:
        if (mons_res_elec(menv[o].type) > 0)
        {
            if (mons_near(o) && menv[o].enchantment[2] != 6)
            {
                strcpy(info, monam(menv[o].number, menv[o].type, menv[o].enchantment[2], 0));
                strcat(info, " appears unharmed.");
                mpr(info);
            }
            hurted = 0;
        }
        break;


    case BEAM_POISON:                     /* poison */
        if (random2(3) != 0)
        {
             poison_monster(o,!YOU_KILL(beam[0].thing_thrown));
        }

        if (mons_res_poison(menv[o].type) > 0)
        {
            if (mons_near(o) && menv[o].enchantment[2] != 6)
            {
                strcpy(info, monam(menv[o].number, menv[o].type, menv[o].enchantment[2], 0));
                strcat(info, " appears unharmed.");
                mpr(info);
            }
            hurted = 0;
        }
        break;

    case BEAM_NEG:                     /* negativity */
        if (mons_holiness(menv[o].type) > 0)
        {
            if (mons_near(o) && menv[o].enchantment[2] != 6)
            {
                strcpy(info, monam(menv[o].number, menv[o].type, menv[o].enchantment[2], 0));
                strcat(info, " appears unharmed.");
                mpr(info);
            }
            hurted = 0;
        }
        else
        {
            if (mons_near(o))
            {
                strcpy(info, monam(menv[o].number, menv[o].type, menv[o].enchantment[2], 0));
                strcat(info, " is drained.");
                mpr(info);
            }
            if (random2(5) == 0)
                menv[o].hit_dice--;
            menv[o].max_hit_points -= 2 + random2(3);
            menv[o].hit_points -= 2 + random2(3);
            if (menv[o].hit_points >= menv[o].max_hit_points)
                menv[o].hit_points = menv[o].max_hit_points;
            if (menv[o].hit_dice <= 0)
                menv[o].hit_points = 0;
        }                       /* end else */

        break;


    case BEAM_HOLY:                    /* flame of cleansing */
        if (mons_holiness(menv[o].type) <= 0)
        {
            if (mons_near(o) && menv[o].enchantment[2] != 6)
            {
                strcpy(info, monam(menv[o].number, menv[o].type, menv[o].enchantment[2], 0));
                strcat(info, " appears unharmed.");
                mpr(info);
            }
            hurted = 0;
        }
        break;

    case BEAM_ICE:                    /* ice - about 50% of damage is cold, other 50% is impact and can't be resisted (except by AC, of course) */
        if (mons_res_cold(menv[o].type) > 0)
        {
            if (mons_near(o) && menv[o].enchantment[2] != 6)
            {
                strcpy(info, monam(menv[o].number, menv[o].type, menv[o].enchantment[2], 0));
                strcat(info, " partially resists.");
                mpr(info);
            }
            hurted /= 2;
        }
        else if (menv[o].inv[2] != 501 && mitm.special[menv[o].inv[2]] % 30 == 3)
        {
            if (mons_near(o) && menv[o].enchantment[2] != 6)
            {
                strcpy(info, monam(menv[o].number, menv[o].type, menv[o].enchantment[2], 0));
                strcat(info, " partially resists.");
                mpr(info);
            }
            hurted /= 2;
        }

        if (mons_res_cold(menv[o].type) == -1 && (menv[o].inv[2] == 501 || mitm.special[menv[o].inv[2]] % 30 != 3))
        {
            strcpy(info, monam(menv[o].number, menv[o].type, menv[o].enchantment[2], 0));
            strcat(info, " is frozen!");
            mpr(info);
            hurted *= 13;
            hurted /= 10;
        }
        break;

    }                           /* end of switch */


    if (stricmp(beam[0].beam_name, "hellfire") == 0
                                            || beam[0].flavour == BEAM_LAVA)
    {
        if (mons_res_fire(menv[o].type) == 2)
        {
            if (mons_near(o) && menv[o].enchantment[2] != 6)
            {
                strcpy(info, monam(menv[o].number, menv[o].type, menv[o].enchantment[2], 0));
                strcat(info, " appears unharmed.");
                mpr(info);
            }
            hurted = 0;
        }
        if (mons_res_fire(menv[o].type) == 1 || (menv[o].inv[2] != 501 && mitm.special[menv[o].inv[2]] % 30 == 2))
        {
            if (mons_near(o) && menv[o].enchantment[2] != 6)
            {
                strcpy(info, monam(menv[o].number, menv[o].type, menv[o].enchantment[2], 0));
                strcat(info, " partially resists.");
                mpr(info);
            }
            hurted /= 2;
        }
        if (mons_res_fire(menv[o].type) == -1 && (menv[o].inv[2] == 501 || mitm.special[menv[o].inv[2]] % 30 != 2))
        {
            strcpy(info, monam(menv[o].number, menv[o].type, menv[o].enchantment[2], 0));
            strcat(info, " is burned terribly!");
            mpr(info);
            hurted *= 12;       /* hellfire */
            hurted /= 10;
        }
    }

    return hurted;

}



/*
   Returns 0 if the monster made its save against hostile enchantments/some
   other magics.
 */
int check_mons_magres(int mn, int pow)
{

    int mrs = mon_resist_mag(menv[mn].type, menv[mn].hit_dice);

    if (mrs == 5000)
        return 0;

//if (random2(pow) + random2(pow) + random2(pow) >= mrs) return 1;
    //if (random2(pow) + random2(pow) + random2(pow) + random2(pow) + random2(pow) >= mrs) return 1;
    //if (random2(pow) + random2(pow) + random2(pow

    if (pow > 40)
        pow = ((pow - 40) / 2) + 40;
    if (pow > 70)
        pow = ((pow - 70) / 2) + 70;
    if (pow > 90)
        pow = ((pow - 90) / 2) + 90;

    if (pow > 120)
        pow = 120;


    int mrchance = 100 + mrs;

    mrchance -= pow;
    int mrch2 = random2(100) + random2(101);

#ifdef WIZARD
    strcpy(info, "Pow:");
    itoa(pow, st_prn, 10);
    strcat(info, st_prn);
    strcat(info, ", mrs: ");
    itoa(mrs, st_prn, 10);
    strcat(info, st_prn);
    strcat(info, ", mrchance:");
    itoa(mrchance, st_prn, 10);
    strcat(info, st_prn);
    strcat(info, ", mrch2:");
    itoa(mrch2, st_prn, 10);
    strcat(info, st_prn);
    mpr(info);
#endif

    if (mrch2 < mrchance)
        return 0;               /* ie saved successfully */

    return 1;

}                               /* end of check_mons_magres */

/*
   Enchants all monsters in player's sight.
 */
void mass_enchantment(int wh_enchant, int pow)
{

    int i = 0;
    int p;
    char brek = 0;

    viewwindow(0, false);
    for (i = 0; i < MNST; i++)
    {
        if (menv[i].type == -1)
            continue;
        if (!mons_near(i))
            continue;

        if (wh_enchant == 30 && (mons_holiness(menv[i].type) != 1 || menv[i].behavior == 7))
            continue;
        /* assuming that the only mass charm is control undead */

        if (wh_enchant != 30 || mons_holiness(menv[i].type) != 1)
/*   if (check_mons_magres(i, pow) == 0 && beam[0].colour != 1 && beam[0].colour != 2 && beam[0].colour != 5) */
            if (check_mons_magres(i, pow) == 0)         /* && beam[0].colour != 1 && beam[0].colour != 2 && beam[0].colour != 5) */
            {
                strcpy(info, monam(menv[i].number, menv[i].type, menv[i].enchantment[2], 0));
                strcat(info, " resists.");
                mpr(info);
/*      beam[0].aim_down = 0; */
                continue;
            }

        if (menv[i].x > you.x_pos - 9 && menv[i].x < you.x_pos + 9 && menv[i].y > you.y_pos - 9 && menv[i].y < you.y_pos + 9 && see_grid(menv[i].x, menv[i].y) != 0)    /*show [menv [i].x - you.x_pos + 9] [menv [i].y - you.y_pos + 9] != 0) */
        {
            if (menv[i].enchantment1 == 1)
                for (p = 0; p < 3; p++)
                {
                    if (menv[i].enchantment[p] == wh_enchant)
                        brek = 1;
                }               // end of for p

            for (p = 0; p < 3; p++)
            {
                if (brek == 1)
                {
                    brek = 0;
                    break;
                }
                if (menv[i].enchantment[p] == 0)
                {
                    menv[i].enchantment[p] = wh_enchant;
                    menv[i].enchantment1 = 1;
                    if (menv[i].enchantment[2] != 6 || player_see_invis() != 0)
                    {
                        strcpy(info, monam(menv[i].number, menv[i].type, menv[i].enchantment[2], 0));
                        switch (wh_enchant)
                        {
                        case 4:
                            strcat(info, " looks frightened.");
                            break;
                        case 5:
                            strcat(info, " looks rather confused.");
                            break;
                        case 30:
                            strcat(info, " submits to your will.");
                            menv[i].behavior = 7;
                            break;
                        }
                        mpr(info);
                        break;
                    }
                    else
                        mpr("Nothing appears to happen.");
                }
            }                   // end of for p

        }                       // end of if visible

    }                           // end of for i

}                               // end of void cause_fear()


/*
   Monster has probably failed save, now it gets enchanted somehow.
 */
int mons_ench_f2(int o, char is_near, int func_pass[10], struct bolt beam[1])
{

    char brek = 0;
    char p;

    switch (beam[0].colour)     /* put in magic resistance */
    {
    case BLACK:                     /* slow monster */
        for (p = 0; p < 3; p++)
        {
            if (menv[o].enchantment[p] == 1)
            {
                if (is_near)
                    mpr("Nothing appears to happen.");
                return 1;
            }
            if (menv[o].enchantment[p] == 2)
            {
                menv[o].enchantment[p] = 0;
                if (menv[o].speed >= 100)
                {
                    menv[o].speed = ((menv[o].speed - 100) / 2) + 100;
                }
                else
                    menv[o].speed /= 2;
                if (menv[o].enchantment[0] == 0 && menv[o].enchantment[1] == 0 && menv[o].enchantment[2] == 0)
                    menv[o].enchantment1 = 0;
                brek = 1;
                break;
            }
        }                       /* end of for p */

        if (brek == 0)
            for (p = 0; p < 3; p++)
            {
                if (menv[o].enchantment[p] == 0)        /* || p == 2) replaces 3rd enchantment if all full. */
                {
                    menv[o].enchantment[p] = 1;
                    if (menv[o].speed >= 100)
                    {
                        menv[o].speed = ((menv[o].speed - 100) / 2) + 100;
                    }
                    else
                        menv[o].speed /= 2;
                    menv[o].enchantment1 = 1;
                    break;
                }
            }


        // put in an exception for fungi, plants and other things you won't
        // notice slow down.
        if (is_near)
        {
            strcpy(info, monam(menv[o].number, menv[o].type,
                                                menv[o].enchantment[2], 0));
            strcat(info, " seems to slow down.");
            mpr(info);
            func_pass[1] = 1;
        }
        return 1;

    case BLUE:                     // haste

        for (p = 0; p < 3; p++)
        {
            if (menv[o].enchantment[p] == 2)
                goto nothinghap;
            if (menv[o].enchantment[p] == 1)
            {
                menv[o].enchantment[p] = 0;
                if (menv[o].speed >= 100)
                {
                    menv[o].speed = ((menv[o].speed - 100) * 2) + 100;
                }
                else
                    menv[o].speed *= 2;
                if (menv[o].enchantment[0] == 0 && menv[o].enchantment[1] == 0 && menv[o].enchantment[2] == 0)
                    menv[o].enchantment1 = 0;
                brek = 1;
                break;
            }
        }                       /* end of for p */

        if (brek == 0)
            for (p = 0; p < 3; p++)
            {
                if (menv[o].enchantment[p] == 0)
                {
                    menv[o].enchantment[p] = 2;
                    if (menv[o].speed >= 100)
                    {
                        menv[o].speed = ((menv[o].speed - 100) * 2) + 100;
                    }
                    else
                        menv[o].speed *= 2;
                    menv[o].enchantment1 = 1;
                    break;
                }
            }
        if (p == 2 && menv[o].enchantment[p] != 0)
            goto nothinghap;

        // put in an exception for fungi, plants and other things you won't
        // notice speed up.
        if (is_near)
        {
            strcpy(info, monam(menv[o].number, menv[o].type,
                                            menv[o].enchantment[2], 0));
            strcat(info, " seems to speed up.");
            mpr(info);
            func_pass[1] = 1;
        }
        return 1;


    case GREEN:                     /* healing */
        if (menv[o].hit_points == menv[o].max_hit_points)
        {
            goto nothinghap;
        }
        menv[o].hit_points += random2(15) + 5;
        if (menv[o].hit_points >= menv[o].max_hit_points)
        {
            menv[o].hit_points = menv[o].max_hit_points;
            if (is_near)
            {
                strcpy(info, monam(menv[o].number, menv[o].type,
                                                menv[o].enchantment[2], 0));
                strcat(info, "'s wounds heal themselves!");
                mpr(info);
                func_pass[1] = 1;
            }
            return 1;
        }

        if (is_near)
        {
            strcpy(info, monam(menv[o].number, menv[o].type,
                                                    menv[o].enchantment[2], 0));
            strcat(info, " is healed somewhat.");
            mpr(info);
            func_pass[1] = 1;
        }
        return 1;

    case CYAN:                     /* paralysis */
        menv[o].speed_increment = 0;

        if (is_near == 1)
        {
            strcpy(info, monam(menv[o].number, menv[o].type,
                                                menv[o].enchantment[2], 0));
            strcat(info, " suddenly stops moving!");
            mpr(info);
            func_pass[1] = 1;
        }

        if (grd[menv[o].x][menv[o].y] == DNGN_LAVA_X
                                || grd[menv[o].x][menv[o].y] == DNGN_WATER_X)
        {
            if (mons_flies(menv[o].type) == 1)
            {
                // don't worry about invisibility - you should be able to
                // see if something has fallen into the lava
                if (is_near == 1)
                {
                    strcpy(info, monam(menv[o].number, menv[o].type,
                                                    menv[o].enchantment[2], 0));

                    if (grd[menv[o].x][menv[o].y] == DNGN_WATER_X)
                    {
                        strcat(info, " falls into the water!");
                    }
                    else
                        strcat(info, " falls into the lava!");
                    mpr(info);
                }

                switch (beam[0].thing_thrown)
                {
                case KILL_YOU:
                case KILL_YOU_MISSILE:
                    monster_die(o, KILL_YOU, beam[0].beam_source);
                    break;      /*  "    " */
                case KILL_MON:
                    monster_die(o, KILL_MON_MISSILE, beam[0].beam_source);
                    break;      /* dragon breath &c */
                }
            }
        }
        return 1;

    case RED:                     /* confusion */
        for (p = 0; p < 3; p++)
        {
            if (menv[o].enchantment[p] == 4)
                return 1;
        }                       /* end of for p */
        for (p = 0; p < 3; p++)
        {
            /* replaces 3rd enchantment if all full. */
            if (menv[o].enchantment[p] == 0 || p == 2)
            {
                menv[o].enchantment[p] = 5;
                menv[o].enchantment1 = 1;
                break;
            }
        }

        // put in an exception for fungi, plants and other things you won't
        // notice you.slow down.
        if (is_near == 1)
        {
            strcpy(info, monam(menv[o].number, menv[o].type,
                                                menv[o].enchantment[2], 0));

            strcat(info, " appears confused.");
            mpr(info);
            func_pass[1] = 1;
        }
        return 1;


    case MAGENTA:                     /* invisibility */
        if (menv[o].enchantment[2] == 6 || is_near != 1)
            goto nothinghap;

        if (menv[o].enchantment[2] != 0)
        {
            if (is_near)
            {
                strcpy(info, monam(menv[o].number, menv[o].type,
                                                menv[o].enchantment[2], 0));
                strcat(info, " flickers for a moment.");
                mpr(info);
                func_pass[1] = 1;
            }
            return 1;
        }

        if (is_near == 1)
        {
            strcpy(info, monam(menv[o].number, menv[o].type,
                                                menv[o].enchantment[2], 0));
            strcat(info, " flickers and vanishes!");
            mpr(info);
            func_pass[1] = 1;
        }
        menv[o].enchantment[2] = 6;
        menv[o].enchantment1 = 1;
        return 1;

        /* 6 is used by digging
           7            teleport
           8            polymorph */
    case LIGHTBLUE:                     /* charm */
        for (p = 0; p < 3; p++)
        {
            if (menv[o].enchantment[p] == 7)
                return 1;
        }                       /* end of for p */
        for (p = 0; p < 4; p++)
        {
            if (p == 3)
                return 1;
            if (menv[o].enchantment[p] == 0)    /* replaces 3rd enchantment if all full. */
            {
                menv[o].enchantment[p] = 30;
                menv[o].enchantment1 = 1;
                menv[o].behavior = 7;
                break;
            }
        }

        // put in an exception for fungi, plants and other things you won't
        // notice slow down.
        if (is_near == 1)
        {
            strcpy(info, monam(menv[o].number, menv[o].type,
                                                menv[o].enchantment[2], 0));
            strcat(info, " is charmed.");
            mpr(info);
            func_pass[1] = 1;
        }
        return 1;

    }                           /* end of switch (beam_colour) */
    goto somethinghap;

nothinghap:
    if (is_near)
    {
        strcpy(info, "Nothing appears to happen.");
        mpr(info);
    }

somethinghap:
    return 1;
}






/*
   Puts the poison value into a monster's enchantment variable.
 */
void poison_monster(int mn, char source)
{
    int p;
    int brek = 0;

    if (menv[mn].type == -1)
        return;

    if (mons_res_poison(menv[mn].type) > 0)
        return;
    if (menv[mn].inv[2] != 501 && mitm.special[menv[mn].inv[2]] % 30 == 4)
        return;

    for (p = 0; p < 3; p++)
    {
        if (menv[mn].enchantment[p] % 50 == 10)
        {
            return;
        }
        if (menv[mn].enchantment[p] % 50 >= 7 && menv[mn].enchantment[p] % 50 < 10)
        {
            menv[mn].enchantment[p]++;
            brek = 1;
            if (mons_near(mn) && (menv[mn].enchantment[2] != 6 || player_see_invis() != 0))
            {
                strcpy(info, monam(menv[mn].number, menv[mn].type, menv[mn].enchantment[2], 0));
                strcat(info, " looks rather sicker.");
                mpr(info);
            }
            break;
        }
    }                           /* end of for p */

    if (brek == 0)
        for (p = 0; p < 3; p++)
        {

            if (menv[mn].enchantment[p] == 0)
            {
                menv[mn].enchantment[p] = 7;
                menv[mn].enchantment1 = 1;
                if (mons_near(mn) && (menv[mn].enchantment[2] != 6 || player_see_invis() != 0))
                {
                    strcpy(info, monam(menv[mn].number, menv[mn].type, menv[mn].enchantment[2], 0));
                    strcat(info, " looks rather sick.");
                    mpr(info);
                }
                break;
            }
        }

    if (source == 1 && menv[mn].enchantment[p] <= 10 && menv[mn].enchantment[p] >= 7)
    {
        menv[mn].enchantment[p] += 50;
    }

}


/*
   Like poison_monster; makes the monster burn if hit by napalm.
 */
void sticky_flame_monster(int mn, char source, int power)       /* modelled on the poison_monster function */
{

    int long_last = 0;
    int brek = 0;
    int p;

    if (menv[mn].type == -1)
        return;

    if (mons_res_fire(menv[mn].type) > 0)
        return;
    if (menv[mn].inv[2] != 501 && mitm.special[menv[mn].inv[2]] % 30 == 2)
        return;

    long_last = random2(power) / 2 + 1;
    if (long_last >= 4)
        long_last = 4;

    for (p = 0; p < 3; p++)
    {
        if (menv[mn].enchantment[p] == 34 || menv[mn].enchantment[p] == 64)     // already covered in sticky flame - ouch!

        {
            return;
        }
        if ((menv[mn].enchantment[p] >= 31 && menv[mn].enchantment[p] < 34) || (menv[mn].enchantment[p] >= 61 && menv[mn].enchantment[p] < 64))
        {
            menv[mn].enchantment[p] += long_last;
            if (menv[mn].enchantment[p] > 34 && menv[mn].enchantment[p] < 60)
                menv[mn].enchantment[p] = 34;
            if (menv[mn].enchantment[p] > 64)
                menv[mn].enchantment[p] = 64;
            brek = 1;
            if (mons_near(mn) && (menv[mn].enchantment[2] != 6 || player_see_invis() != 0))
            {
                strcpy(info, monam(menv[mn].number, menv[mn].type, menv[mn].enchantment[2], 0));
                strcat(info, " is covered in liquid fire!");
                mpr(info);
            }
            break;
        }
    }                           /* end of for p */

    if (brek == 0)
        for (p = 0; p < 3; p++)
        {
            if (menv[mn].enchantment[p] == 0)
            {
                menv[mn].enchantment[p] = 30 + long_last;
                menv[mn].enchantment1 = 1;
                if (mons_near(mn) && (menv[mn].enchantment[2] != 6 || player_see_invis() != 0))
                {
                    strcpy(info, monam(menv[mn].number, menv[mn].type, menv[mn].enchantment[2], 0));
                    strcat(info, " is covered in liquid fire!");
                    mpr(info);
                }
                break;
            }
        }

    if (source == 1 && menv[mn].enchantment[p] <= 60 && menv[mn].enchantment[p] >= 31)
    {
        menv[mn].enchantment[p] += 30;
    }

}

/*
   Places a cloud with the given stats. May delete old clouds to make way if
   there are too many (30) on level. Will overwrite an old cloud under some
   circumstances.
 */
void place_cloud(unsigned char cl_type, unsigned char ctarget_x, unsigned char ctarget_y, unsigned char cl_range)       // if env.cloud_type > 100, it is a monster's cloud
 {
    unsigned char ci = 0;

    char c_deleted = 100;

    if (env.cloud_no >= 30)
        c_deleted = random2(30);

    if (env.cgrid[ctarget_x][ctarget_y] != CNG)
    {
     if ((env.cloud_type[env.cgrid[ctarget_x][ctarget_y]] >= 5 // smoke
         && env.cloud_type[env.cgrid[ctarget_x][ctarget_y]] <= 8)
         || env.cloud_type[env.cgrid[ctarget_x][ctarget_y]] == 2 // stink
         || env.cloud_type[env.cgrid[ctarget_x][ctarget_y]] == 10 // smoke
         || env.cloud_decay[env.cgrid[ctarget_x][ctarget_y]] <= 20)//soon gone
     {
            env.cloud_type[env.cgrid[ctarget_x][ctarget_y]] = CLOUD_NONE;
            env.cgrid[ctarget_x][ctarget_y] = CNG;
            env.cloud_decay[env.cgrid[ctarget_x][ctarget_y]] = 0;
            env.cloud_no--;
     } else return;
    }


    for (ci = 0; ci < CLOUDS; ci++)
    {
        if (ci == c_deleted)
        {
            env.cloud_type[ci] = CLOUD_NONE;
            env.cgrid[env.cloud_x[ci]][env.cloud_y[ci]] = CNG;
            env.cloud_decay[ci] = 0;
            env.cloud_no--;
        }

        if (env.cloud_type[ci] == 0)    // ie is empty

        {
            env.cloud_type[ci] = cl_type;
            env.cloud_x[ci] = ctarget_x;
            env.cloud_y[ci] = ctarget_y;
            env.cloud_decay[ci] = cl_range * 10;
            env.cloud_no++;
            env.cgrid[env.cloud_x[ci]][env.cloud_y[ci]] = ci;
            break;
        }

    }

//      move_x = 0; move_y = 0;

}                               // end of void place_cloud();



/*
   Used by monsters in "planning" which spell to cast. Fires off a "tracer"
   which tells the monster what it'll hit if it breathes/casts etc.

   tracer = 0 = run out of range or hits a wall
   tracer = 1 = hits you in range
   tracer = 2 = hits friendly monster
   tracer = 3 = hits hostile monster

   tracer_mons = 1 = hits monster specified in trac_targ

   tracer_hit_mons now holds value of mons_see_invis of attacking monster.
   If it's 0, won't register invis monsters or you
 */
void tracer_f(int i, struct bolt beem[1])
{
    strcpy(beem[0].beam_name, "0tracer");
    beem[0].trac_hit_mons = mons_see_invis(menv[i].type);
    beem[0].tracer = 0;
    beem[0].tracer_mons = 0;
    beem[0].trac_targ = menv[i].monster_foe;
    beem[0].colour = 200;
    beem[0].beam_name[0] = '0';
    beem[0].type = 0;
    beem[0].flavour = BEAM_MAGIC;
    beem[0].damage = 0;
    beem[0].range = 10;
    beem[0].move_x = beem[0].trac_targ_x - menv[i].x;
    beem[0].move_y = beem[0].trac_targ_y - menv[i].y;
    beem[0].source_x = menv[i].x;
    beem[0].source_y = menv[i].y;
    beem[0].aim_down = 0;
    beem[0].beam_source = i;
    missile(beem, 0);
}


/*
   When a mimic is hit by a ranged attack, it teleports away (the slow way)
   and changes its appearance - the appearance change is in monster_teleport
   in mstuff2.
 */
void mimic_alert(int mimic)
{
    int p = 0;

    for (p = 0; p < 3; p++)
    {
        if (menv[mimic].enchantment[p] >= 40 && menv[mimic].enchantment[p] <= 43)
            return;
    }

    monster_teleport(mimic, 0);
}

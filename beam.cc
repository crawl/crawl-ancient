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

#include <stdlib.h>
#include <string.h>

#ifdef DOS
#include <dos.h>
#include <conio.h>
#endif

#ifdef USE_CURSES
#include <curses.h>
#endif

#include "externs.h"

#include "bang.h"
#include "effects.h"
#include "fight.h"
#include "it_use2.h"
#include "itemname.h"
#include "items.h"
#include "monstuff.h"
#include "mon-util.h"
#include "mstuff2.h"
#include "ouch.h"
#include "player.h"
#include "religion.h"
#include "skills.h"
#include "spells1.h"
#include "spells3.h"
#include "spells4.h"
#include "stuff.h"
#include "view.h"


void sticky_flame_monster(int mn, bool source, int power);




/*
 *  Handles any projectiles which leave visible non-smoke trails and can affect
 *  multiple opponents. Eg firebolt, but not magic dart or thrown weapons.
 *  Should be merged with missile(), as there is a lot of redundant code here.
 */
void beam( struct bolt *pbolt )
{

    int beam_sign_x = 0;
    int beam_sign_y = 0;
    int bmax = 0;
    float bmoy = 0;
    float bmay = 0;
    int n, o;
    struct monsters *monster = 0;    // NULL {dlb}
    char clouty = 0;
    bool crumble = false;
    char bounce = 0;
    int hurted = 0;

    pbolt->bx = pbolt->source_x;
    pbolt->by = pbolt->source_y;

    if ( pbolt->move_x > 0 )
      beam_sign_x = 1;
    else if ( pbolt->move_x < 0 )
    {
        beam_sign_x = -1;
        pbolt->move_x *= -1;
    }

    if ( pbolt->move_y > 0 )
      beam_sign_y = 1;
    else if ( pbolt->move_y < 0 )
    {
        beam_sign_y = -1;
        pbolt->move_y *= -1;
    }

    if ( pbolt->move_x > 1 || pbolt->move_x < -1
          || pbolt->move_y > 1 || pbolt->move_y < -1 )
    {

        bmoy = pbolt->move_y;

        if ( pbolt->move_x > pbolt->move_y )
        {
            if ( beam_sign_x == 1 )
              bmay = (float) (pbolt->move_y) / (pbolt->move_x + 1);
            if ( beam_sign_x == -1 )
              bmay = (float) (pbolt->move_y) / (pbolt->move_x + 1);
            bmax = 1;
        }
        else
        {
            if ( beam_sign_y == 1 )
              bmay = (float) (pbolt->move_x) / (pbolt->move_y + 1);
            if ( beam_sign_y == -1 )
              bmay = (float) (pbolt->move_x) / (pbolt->move_y + 1);
            bmax = 2;
        }
        bmoy = 1;
    }                           //else beam_inc = 0;

    for (n = 1; n < pbolt->range; n++)
    {
        if ( pbolt->beam_name[0] == '0' && pbolt->flavour != BEAM_MAGIC )
          return;

        pbolt->move_x = beam_sign_x;
        pbolt->move_y = beam_sign_y;

        if ( bmax == 1 )
        {
            pbolt->move_x = beam_sign_x;
            bmoy += bmay;
            if ( bmoy >= 1 )
            {
                pbolt->move_y = beam_sign_y;
                bmoy--;
            }
            else
              pbolt->move_y = 0;
        }


        if ( bmax == 2 )
        {
            pbolt->move_y = beam_sign_y;
            bmoy += bmay;
            if ( bmoy >= 1 )
            {
                pbolt->move_x = beam_sign_x;
                bmoy--;
            }
            else
              pbolt->move_x = 0;
        }

        if ( strcmp(pbolt->beam_name, "blast of poison") == 0 )
        {
            if ( pbolt->thing_thrown != KILL_MON )
              place_cloud(CLOUD_POISON, pbolt->bx, pbolt->by, random2(4) + 2);
            else
              place_cloud(CLOUD_POISON_MON, pbolt->bx, pbolt->by, random2(4) + 2);

            if ( pbolt->bx == pbolt->target_x && pbolt->by == pbolt->target_y )
            {
                if (pbolt->thing_thrown != KILL_MON)
                    big_cloud(CLOUD_POISON, pbolt->bx, pbolt->by, 0, 9);
                else
                    big_cloud(CLOUD_POISON_MON, pbolt->bx, pbolt->by, 0, 9);

                pbolt->aim_down = 0;

                return;
            }
        }

        if (pbolt->colour == BROWN && pbolt->beam_name[0] == '0')
        {
            pbolt->bx += pbolt->move_x;
            pbolt->by += pbolt->move_y;

//#ifndef WIZARD
            if ( grd[pbolt->bx][pbolt->by] == DNGN_STONE_WALL
                  || grd[pbolt->bx][pbolt->by] == DNGN_METAL_WALL
                  || pbolt->bx <= 5 || pbolt->bx >= (GXM - 5)
                  || pbolt->by <= 5 || pbolt->by >= (GYM - 5) )
            {
                pbolt->aim_down = 0;
                return;
            }
//#else
            if ( pbolt->bx < 0  || pbolt->bx > (GXM - 1)
                  || pbolt->by < 0 || pbolt->by > (GYM - 1) )
            {
                pbolt->aim_down = 0;
                return;
            }
//#endif
            if ( grd[pbolt->bx][pbolt->by] == DNGN_ROCK_WALL )
            {
                grd[pbolt->bx][pbolt->by] = DNGN_FLOOR;

                if ( !crumble )       // && see_grid(pbolt->bx, pbolt->by) )
                {
                    mpr("You hear a grinding noise.");
                    crumble = true;
                    pbolt->wand_id = 1;
                }

            }
        }
        else if ( grd[pbolt->bx + pbolt->move_x][pbolt->by + pbolt->move_y] >= MINMOVE )
        {
            pbolt->bx += pbolt->move_x;
            pbolt->by += pbolt->move_y;
        }
        else
        {

/*              if (pbolt->flavour == BEAM_EXPLOSION)
   {
   explosion1(pbolt);
   pbolt->aim_down = 0;
   return;
   } */
            if ((pbolt->colour == WHITE && pbolt->flavour != BEAM_COLD
                 && pbolt->hit >= 15) || pbolt->flavour == BEAM_NUKE)
                /* disintegration (or powerful disruption), eye of devastation */
            {
                if ( ( grd[pbolt->bx + pbolt->move_x][pbolt->by + pbolt->move_y] == DNGN_ROCK_WALL || grd[pbolt->bx + pbolt->move_x][pbolt->by + pbolt->move_y] == DNGN_WAX_WALL )
                     && !( pbolt->bx <= 6 || pbolt->by <= 6 || pbolt->bx >= (GXM - 6) || pbolt->by >= (GYM - 6) ) )
                {
                    grd[pbolt->bx + pbolt->move_x][pbolt->by + pbolt->move_y] = DNGN_FLOOR;
                    mpr("You hear a grinding noise.");
                    pbolt->wand_id = 1;
                }
                if (grd[pbolt->bx + pbolt->move_x][pbolt->by + pbolt->move_y] == DNGN_ORCISH_IDOL || (grd[pbolt->bx + pbolt->move_x][pbolt->by + pbolt->move_y] >= DNGN_SILVER_STATUE && grd[pbolt->bx + pbolt->move_x][pbolt->by + pbolt->move_y] <= DNGN_STATUE_39))
                {
                    grd[pbolt->bx + pbolt->move_x][pbolt->by + pbolt->move_y] = DNGN_FLOOR;

                    if ( !see_grid(pbolt->bx + pbolt->move_x, pbolt->by + pbolt->move_y) )
                      mpr("You hear a hideous screaming!");
                    else
                      mpr("The statue screams as its substance crumbles away!");

                    pbolt->wand_id = 1;
                }
                return;
            }

            if (pbolt->beam_name[0] == '0' || pbolt->flavour != BEAM_ELECTRICITY)
                // ie non-bouncing enchantments, non-electrical beams etc
            {
                pbolt->aim_down = 0;
                return;
            }

            n++;                // reduces beam's range by 1

            bounce += 2;

            if ( bounce > 3 || !one_chance_in(bounce) )
            {
                pbolt->aim_down = 0;
                return;
            }

            strcpy(info, "The ");
            strcat(info, pbolt->beam_name);
            strcat(info, " bounces.");
            mpr(info);

            if ( pbolt->move_x == 0 ^ pbolt->move_y == 0 )      //   ^ XOR
            {
                beam_sign_x *= -1;
                beam_sign_y *= -1;
            }

            if ( pbolt->move_x != 0 && pbolt->move_y != 0 )
            {
                if (grd[pbolt->bx][pbolt->by + pbolt->move_y] >= MINMOVE && grd[pbolt->bx + pbolt->move_x][pbolt->by] >= MINMOVE)
                {
                    beam_sign_x *= -1;
                    beam_sign_y *= -1;
                }

                if (grd[pbolt->bx + pbolt->move_x][pbolt->by] < MINMOVE && grd[pbolt->bx][pbolt->by + pbolt->move_y] >= MINMOVE)
                {
                    beam_sign_x *= -1;
                }

                if (grd[pbolt->bx][pbolt->by + pbolt->move_y] < MINMOVE && grd[pbolt->bx + pbolt->move_x][pbolt->by] >= MINMOVE)
                {
                    beam_sign_y *= -1;
                }

                if ((grd[pbolt->bx][pbolt->by + pbolt->move_y] < MINMOVE && grd[pbolt->bx + pbolt->move_x][pbolt->by] < MINMOVE))         // || grd[pbolt->bx][pbolt->by + pbolt->move_y] > DNGN_LAST_SOLID_TILE )
                {
                    beam_sign_x *= -1;
                    beam_sign_y *= -1;
                }

            }
        }

/*      if (pbolt->bx > you.x_pos - 9 && pbolt->bx < you.x_pos + 9 && pbolt->by > you.y_pos - 9 && pbolt->by < you.y_pos + 9 && (show [pbolt->bx - you.x_pos + 9] [pbolt->by - you.y_pos + 9] != 0 || (pbolt->bx == you.x_pos && pbolt->by == you.y_pos)) && pbolt->type != 0) */
        if (((pbolt->bx > you.x_pos - 9 && pbolt->bx < you.x_pos + 9 && pbolt->by > you.y_pos - 9 && pbolt->by < you.y_pos + 9) || (pbolt->bx == you.x_pos && pbolt->by == you.y_pos)) && pbolt->type != 0)
        {
            if (pbolt->range != n)
            {
                textcolor(pbolt->colour);
                gotoxy(pbolt->bx - you.x_pos + 18, pbolt->by - you.y_pos + 9);
                putch(pbolt->type);
#ifdef USE_CURSES
                // If we don't refresh curses we won't
                // guarantee that the beam is visible
                refresh();
#endif
                delay(15);
            }
        }

        if ( bounce > 0 )
          bounce--;

        if ( env.cgrid[pbolt->bx][pbolt->by] != CNG )   // hit a cloud
        {
            if ( pbolt->beam_name[0] == '0' && pbolt->colour == DARKGREY )    /* polymorph randomly changes clouds in its path */
              env.cloud_type[env.cgrid[pbolt->bx][pbolt->by]] = 1 + random2(8);    // this needs to be fixed to take into account player/monster clouds {dlb}

            if ( pbolt->colour == 200 || pbolt->beam_name[0] == '0' )
              goto out_of_cloud_bit;

            clouty = env.cgrid[pbolt->bx][pbolt->by];

            if ( ( ( env.cloud_type[clouty] == CLOUD_COLD || env.cloud_type[clouty] == CLOUD_COLD_MON )
                   && (pbolt->flavour == BEAM_FIRE || pbolt->flavour == BEAM_LAVA) )
                || ( ( env.cloud_type[clouty] == CLOUD_FIRE || env.cloud_type[clouty] == CLOUD_FIRE_MON )
                     && pbolt->flavour == BEAM_COLD ) )
            {
                if ( !silenced(pbolt->bx, pbolt->by)
                     && !silenced(you.x_pos, you.y_pos) )
                  mpr("You hear a sizzling sound!");

                env.cloud_type[clouty] = CLOUD_NONE;
                env.cgrid[env.cloud_x[clouty]][env.cloud_y[clouty]] = CNG;
                env.cloud_decay[clouty] = 0;
                env.cloud_no--;

                n += 5;

            }

        }

out_of_cloud_bit:
        if (pbolt->bx == you.x_pos && pbolt->by == you.y_pos && pbolt->colour != BROWN)
        {                       // ^^^ digging
            // have to do something about enchantments here.

            if (pbolt->colour == 200)
            {
                pbolt->tracer = 1;
                goto out_of_hit_you;
            }

/* ******************************************************************

            if ( pbolt->flavour == BEAM_EXPLOSION )
            {
                explosion1(pbolt);
                pbolt->aim_down = 0;
                return;
            }

****************************************************************** */

            if (pbolt->beam_name[0] != '0')    // ie enchantments always hit
            {
                if ( player_light_armour()
                    && pbolt->move_x != 0
                    && pbolt->move_y != 0
                    && coinflip() )
                  exercise(SK_DODGING, 1);

                if ( you.duration[DUR_REPEL_MISSILES]
                      || you.mutation[MUT_REPULSION_FIELD] == 3 )
                  pbolt->hit -= random2(pbolt->hit / 2);

                if ( you.duration[DUR_DEFLECT_MISSILES] )
                  pbolt->hit = random2(pbolt->hit / 2);

                if ( pbolt->hit < random2limit(player_evasion(), 40) + random2(you.dex) / 3 - 2
                    && ( pbolt->move_x != 0 || pbolt->move_y != 0 ) )
                  {
                    strcpy(info, "The ");
                    strcat(info, pbolt->beam_name);
                    strcat(info, " misses you.");
                    mpr(info);
                    continue;
                  }
            }

            if ( pbolt->beam_name[0] == '0' )     // that is, no "named" beam has been passed - I smell a hack {dlb}
            {
                pbolt->aim_down = 0;
                if (pbolt->colour != BLUE && pbolt->colour != MAGENTA && pbolt->colour != GREEN && ((pbolt->colour != LIGHTGREY && pbolt->colour != LIGHTGREEN) || pbolt->move_x != 0 || pbolt->move_y != 0))
// if ( (pbolt->thing_thrown == KILL_YOU || pbolt->thing_thrown == KILL_YOU_MISSILE) && you_resist_magic(pbolt->ench_power) )
                    if ( you_resist_magic(pbolt->ench_power) )
                    {
                        canned_msg(MSG_YOU_RESIST);
                        return;
                    }

                switch ( pbolt->colour )         // these colors are misapplied - see mons_ench_f2() {dlb}
                {
                case BLACK:
                    potion_effect(POT_SLOWING, pbolt->ench_power);
                    pbolt->wand_id = 1;
                    return;     // slow

                case BLUE:
                    potion_effect(POT_SPEED, pbolt->ench_power);
                    pbolt->wand_id = 1;
                    return;     // haste

                case GREEN:
                    potion_effect(POT_HEAL_WOUNDS, pbolt->ench_power);
                    pbolt->wand_id = 1;
                    return;     // heal (heal wounds potion eff)

                case CYAN:
                    potion_effect(POT_PARALYSIS, pbolt->ench_power);
                    pbolt->wand_id = 1;
                    return;     // paralysis

                case RED:
                    potion_effect(POT_CONFUSION, pbolt->ench_power);
                    pbolt->wand_id = 1;
                    return;     // confusion

                case MAGENTA:
                    potion_effect(POT_INVISIBILITY, pbolt->ench_power);
                    pbolt->wand_id = 1;
                    return;     // invisibility

                // 6 is used by digging

                case LIGHTGREY:
                    you_teleport();
                    pbolt->wand_id = 1;
                    return;
                case DARKGREY:
                    mpr("This is polymorph other only!");
                    pbolt->wand_id = 1;
                    return;
                case LIGHTBLUE:
                    potion_effect(POT_CONFUSION, pbolt->ench_power);
                    pbolt->wand_id = 1;
                    return;     // enslavement - confusion?

                case LIGHTGREEN:
                    if (you.level_type == LEVEL_ABYSS)
                    {
                        mpr("You feel trapped.");
                        break;
                    }
                    mpr("You are cast into the Abyss!");
                    more();
                    banished(DNGN_ENTER_ABYSS);
                    pbolt->wand_id = 1;
                    return;     // banishment to the abyss

                case LIGHTMAGENTA:      // pain
                    if ( you.is_undead || you.mutation[MUT_TORMENT_RESISTANCE] )
                    {
                        mpr("You are unaffected.");
                        return;
                    }
                    mpr("Pain shoots through your body!");
                    strcpy(pbolt->beam_name, "spell");
                    if ( YOU_KILL(pbolt->thing_thrown) )
                      ouch(random2(pbolt->hit), 0, KILLED_BY_TARGETTING);
                    else
                      ouch(random2(pbolt->hit), pbolt->beam_source, KILLED_BY_BEAM);
                    pbolt->wand_id = 1;
                    return;
                case WHITE:
                    mpr("You are blasted!");
                    strcpy(pbolt->beam_name, "spell");
                    if ( YOU_KILL(pbolt->thing_thrown) )
                      ouch(random2(pbolt->hit), 0, KILLED_BY_TARGETTING);
                    else
                      ouch(random2(pbolt->hit), pbolt->beam_source, KILLED_BY_BEAM);
                    pbolt->wand_id = 1;
                    return;
                }               // end of switch (pbolt->colour)
            }                   // end of if pbolt->beam_name[0] == 48


            strcpy(info, "The ");
            strcat(info, pbolt->beam_name);
            strcat(info, " hits you");
            strcat(info, "!");
            mpr(info);

            hurted = 0;

            if (pbolt->damage > 100)
            {
                hurted += random2(pbolt->damage - 100);
                hurted += random2(pbolt->damage - 100);
                hurted += random2(pbolt->damage - 100);
            }
            else
              hurted += random2(pbolt->damage);

            hurted -= random2(1 + player_AC());
        // don't bother with + 1 here
            hurted -= random2(player_shield_class());
            you.shield_blocks++;

          // shrapnel
            if (pbolt->flavour == BEAM_FRAG)
            {
                hurted -= random2(1 + player_AC());
                hurted -= random2(1 + player_AC());
            }

            if (you.equip[EQ_SHIELD] != -1)
                if (pbolt->move_x != 0 || pbolt->move_y != 0)
                    exercise(SK_SHIELDS, (random2(3)) / 2);

            if ( you.equip[EQ_BODY_ARMOUR] != -1 )
              if ( !player_light_armour() && one_chance_in(4)
                  && random2(1000) <= mass(OBJ_ARMOUR, you.inv_type[you.equip[EQ_BODY_ARMOUR]]) )
                    exercise(SK_ARMOUR, 1);

            if (hurted < 0)
              hurted = 0;

            hurted = check_your_resists(hurted, pbolt->flavour);

            if ( pbolt->flavour == BEAM_FIRE
                  || stricmp(pbolt->beam_name, "hellfire") == 0 )
              scrolls_burn(3, OBJ_SCROLLS);   // also above
            else if ( pbolt->flavour == BEAM_COLD )
              scrolls_burn(3, OBJ_POTIONS);

            if ( YOU_KILL(pbolt->thing_thrown) )
              ouch(hurted, 0, KILLED_BY_TARGETTING);
            else
              ouch(hurted, pbolt->beam_source, KILLED_BY_BEAM);

            n++;                // reduces beam's range

            if ( pbolt->flavour == BEAM_LAVA )
              n += 1; // lava doesn't go far, but it goes through most stuff
            else if ( pbolt->flavour != BEAM_ELECTRICITY )
              n += random2(4) + 2; /* If it isn't lightning, reduce range by a lot */

            if ( pbolt->flavour == BEAM_EXPLOSION )
            {
                explosion1(pbolt);
                return;
            }

            if ( pbolt->move_x == 0 && pbolt->move_y == 0 )
              break;

            goto check_aimed;
        }

out_of_hit_you:
        if ( pbolt->target_x == pbolt->bx && pbolt->target_y == pbolt->by && pbolt->aim_down == 1 && ((grd[pbolt->bx][pbolt->by] == DNGN_LAVA && pbolt->flavour == BEAM_COLD) || ((grd[pbolt->bx][pbolt->by] == DNGN_DEEP_WATER || grd[pbolt->bx][pbolt->by] == DNGN_SHALLOW_WATER) && pbolt->flavour == BEAM_FIRE)) )
          place_cloud(CLOUD_STEAM, pbolt->bx, pbolt->by, 2 + random2(5));

        if ( mgrd[pbolt->bx][pbolt->by] != MNG && (pbolt->colour != BROWN || pbolt->beam_name[0] != '0') )
        {
            o = mgrd[pbolt->bx][pbolt->by];
            monster = &menv[o];

            if ( pbolt->beam_name[0] == '0' )
              goto enchanting;
            else
            {
            // tracer = 0 = run out of range
            // tracer = 1 = hits you in range
            // tracer_mons = 0 = hits no monsters
            //       "     = 1 = hits monster before you (possibly also after)
            //       "     = 2 = hits monster after but not before
            //       "     = 3 = hits tamed monster(s) but no others
            //       "     = 4 = hits tamed monster(s) + possibly other things

                if ( monster_habitat(monster->type) != DNGN_FLOOR && monster->number == 1 && (pbolt->bx != pbolt->target_x || pbolt->by != pbolt->target_y || pbolt->aim_down != 1) )
                  goto check_aimed;

                if ( pbolt->beam_name[0] != '0' )        // ie enchantments always hit    // isn't this already assumed above ??? {dlb}
                {
                    if (pbolt->hit < random2(monster->evasion))
                    {
                        strcpy(info, "The ");
                        strcat(info, pbolt->beam_name);
                        strcat(info, " misses ");
                        strcat(info, monam(monster->number, monster->type, monster->enchantment[2], 1));   //gmon_name [menv_class [o]]);
                        strcat(info, ".");
                        mpr(info);
                        goto check_aimed;
                    }
                }

                strcpy(info, "The ");
                strcat(info, pbolt->beam_name);
                strcat(info, " hits ");
                strcat(info, monam(monster->number, monster->type, monster->enchantment[2], 1));   //gmon_name [mons_class [o]]);
                strcat(info, ".");
                mpr(info);

                hurted = 0;

                if ( pbolt->damage > 100 )
                {
                    hurted += random2(pbolt->damage - 100);
                    hurted += random2(pbolt->damage - 100);
                    hurted += random2(pbolt->damage - 100);
                }
                else
                  hurted += random2(pbolt->damage);

                hurted -= random2(1 + monster->armor_class);

                if ( pbolt->flavour == BEAM_FRAG )
                {
                    hurted -= random2(1 + monster->armor_class);
                    hurted -= random2(1 + monster->armor_class);
                }

                if ( hurted < 1 )
                  hurted = 0;

                hurted = check_mons_resists(monster, pbolt, hurted);

                hurt_monster(monster, hurted);

                if ( monster->behavior == BEH_SLEEP )
                  monster->behavior = BEH_CHASING_I;

                if ( YOU_KILL(pbolt->thing_thrown) && monster->behavior == BEH_ENSLAVED && hurted > 0 )
                {
                    monster->behavior = BEH_CHASING_I;
                    //naughty(NAUGHTY_STABBING, 5);
                }

                if ( monster->hit_points < 1 )
                {
                    switch (pbolt->thing_thrown)
                    {
                      case KILL_YOU:      // your beam
                      case KILL_YOU_MISSILE:
                        monster_die(monster, KILL_YOU_MISSILE, 0);
                        break;
                      case KILL_MON:      // dragon breath &c
                        monster_die(monster, KILL_MON_MISSILE, pbolt->beam_source);
                        break;
                    }
                }
                else
                {
                    if ( YOU_KILL(pbolt->thing_thrown) && mons_near(monster) )
                      print_wounds(monster);

                    if ( mons_category(monster->type) == MC_MIMIC )
                      mimic_alert(monster);
                }

                if ( pbolt->flavour == BEAM_EXPLOSION && strcmp(pbolt->beam_name, "hellfire") != 0 )
                {
                    explosion1(pbolt);
                    pbolt->aim_down = 0;
                    return;
                }

                if ( pbolt->flavour != BEAM_ELECTRICITY )
                  n += random2(5) + 3;        /* If it isn't lightning, reduce range by a lot */
            }    // end "pbolt->beam_name[0] != '0'"


enchanting:
            if (monster->x == pbolt->bx && monster->y == pbolt->by && monster->type != -1 && pbolt->beam_name[0] == '0')
            {
                if ( pbolt->colour == 200 )
                {
                    if ( pbolt->trac_targ != MHITYOU )
                    {
                        if ( monster->behavior == BEH_ENSLAVED )
                        {
                            if ( menv[pbolt->beam_source].behavior == BEH_ENSLAVED )
                            {
                                pbolt->tracer_mons = 4;
                                return;    //goto check_aimed;
                            }

                            if ( pbolt->tracer_mons == 0 )
                            {
                                pbolt->tracer_mons = 3;
                                pbolt->trac_hit_tamed = 1;
                                return;    //goto check_aimed;
                            }
                        }

                        if ( o == menv[pbolt->beam_source].monster_foe )
                        {
                            pbolt->trac_hit_mons = 1;
                            return;    //goto check_aimed;
                        }

                        if ( pbolt->tracer_mons == 0 )
                        {
                            pbolt->tracer_mons = 1;    //3;
                            return;    //goto check_aimed;
                        }

                        if ( pbolt->trac_hit_mons == 1 && pbolt->tracer_mons != 1 )
                          pbolt->tracer_mons = 2;    // else

                        if ( pbolt->trac_hit_mons == 0 )
                          pbolt->tracer_mons = 1;
                    }
                    else        // Aimed at you:
                    {

                        if ( pbolt->tracer == 1 && pbolt->tracer_mons != 1 )
                          pbolt->tracer_mons = 2;    // else

                        if ( pbolt->tracer == 0 )
                          pbolt->tracer_mons = 1;
                    }

                    goto check_aimed;
                }    // end "if pbolt->colour == 200"

                if ( you.invis && !mons_see_invis(monster->type) && monster->behavior == BEH_SLEEP )
                  monster->behavior = BEH_CHASING_I;

                if ( pbolt->colour == LIGHTGREY )        // teleportation
                {
                    if ( !check_mons_magres(monster, pbolt->ench_power) && ( pbolt->move_x || pbolt->move_y ) )
                      goto it_resists;

                    if ( simple_monster_message(monster, " looks slightly unstable.") )
                      pbolt->wand_id = 1;

                    monster_teleport(monster, false);
                    pbolt->aim_down = 0;
                    return;
                }

                if ( pbolt->colour == DARKGREY )
                {
                    if ( !check_mons_magres(monster, pbolt->ench_power) )
                      goto it_resists;

                    if ( monster_polymorph(monster, RANDOM_MONSTER, 100) )
                      pbolt->wand_id = 1;

                    pbolt->aim_down = 0;
                    return;
                }

                if ( pbolt->colour == LIGHTGREEN )
                {
                    if ( !check_mons_magres(monster, pbolt->ench_power) )
                      goto it_resists;
                    if ( you.level_type == LEVEL_ABYSS )
                      simple_monster_message(monster, " wobbles for a moment.");
                    else
                      monster_die(monster, KILL_RESET, pbolt->beam_source);
                    pbolt->aim_down = 0;
                    pbolt->wand_id = 1;    // need to tie this into messaging outcome {dlb}
                    return;
                }

                if ( pbolt->colour == LIGHTCYAN )
                {
                    if ( mons_holiness(monster->type) != MH_NATURAL
                        || monster->type == MONS_PULSATING_LUMP )
                        goto unaffected;

                    if ( !check_mons_magres(monster, pbolt->ench_power) )
                        goto it_resists;

                    if ( monster_polymorph(monster, MONS_PULSATING_LUMP, 100) )
                      pbolt->wand_id = 1;

                    pbolt->aim_down = 0;
                    return;
                }

                if ( pbolt->colour == YELLOW )
                {
                    if ( mons_holiness(monster->type) != MH_UNDEAD )
                    {
unaffected:
                        simple_monster_message(monster, " is unaffected.");
                        pbolt->aim_down = 0;
                        return;
                    }

                    simple_monster_message(monster, " convulses!");

                    hurt_monster(monster, random2(pbolt->hit) + random2(pbolt->hit) + random2(pbolt->hit));

                    strcpy(pbolt->beam_name, "spell");

                    char killer = 0;

                    switch (pbolt->thing_thrown)
                    {
                      case KILL_YOU:
                      case KILL_YOU_MISSILE:
                        killer = KILL_YOU_MISSILE;
                        break;  // your beam

                      case KILL_MON:
                        killer = KILL_MON_MISSILE;
                        break;  // dragon breath &c
                    }

                    if ( monster->hit_points < 1 )
                      monster_die(monster, killer, pbolt->beam_source);
                    else
                    {
                        print_wounds(monster);

                        if ( mons_category(monster->type) == MC_MIMIC )
                          mimic_alert(monster);
                    }

                    pbolt->aim_down = 0;
                    return;
                }

                if ( !check_mons_magres(monster, pbolt->ench_power) && pbolt->colour != BLUE && pbolt->colour != GREEN && pbolt->colour != MAGENTA )
                {
it_resists:
                    simple_monster_message(monster, " resists.");

                    pbolt->aim_down = 0;
                    return;
                }

                if ( pbolt->colour == LIGHTMAGENTA )     /* pain/agony */
                {
                    if ( mons_holiness(monster->type) == MH_UNDEAD || mons_holiness(monster->type) == MH_DEMONIC )
                      goto it_resists;

                    simple_monster_message(monster, " convulses in agony!");

                    if (strstr(pbolt->beam_name, "agony") != NULL)
                    {
                        monster->hit_points = monster->hit_points / 2;

                        if ( monster->hit_points < 1 )
                          monster->hit_points = 1;

                        strcpy(pbolt->beam_name, "agony");
                    }
                    else
                    {
                        hurt_monster(monster, random2(pbolt->hit));
                        strcpy(pbolt->beam_name, "pain");
                    }

                    char killer = 0;

                    switch (pbolt->thing_thrown)
                    {
                      case KILL_YOU:
                      case KILL_YOU_MISSILE:
                        killer = KILL_YOU_MISSILE;
                        break;  // your beam

                      case KILL_MON:
                        killer = KILL_MON_MISSILE;
                        break;  // dragon breath &c
                    }

                    if ( monster->hit_points < 1 )
                      monster_die(monster, killer, pbolt->beam_source);
                    else
                      {
                        print_wounds(monster);

                        if ( mons_category(monster->type) == MC_MIMIC )
                          mimic_alert(monster);
                      }

                    pbolt->aim_down = 0;
                    return;
                }

                if ( pbolt->colour == WHITE )    /* disrupt/disintegrate */
                {

                    if ( simple_monster_message(monster, " is blasted.") )
                      pbolt->wand_id = 1;

                    hurt_monster(monster, random2(1 + pbolt->hit));

                    strcpy(pbolt->beam_name, "spell");

                    char killer = 0;

                    switch (pbolt->thing_thrown)
                    {
                      case KILL_YOU:
                      case KILL_YOU_MISSILE:
                        killer = KILL_YOU_MISSILE;
                        break;  // your beam

                      case KILL_MON:
                        killer = KILL_MON_MISSILE;
                        break;  // dragon breath &c
                    }

                    if ( monster->hit_points < 1 )
                      monster_die(monster, killer, pbolt->beam_source);
                    else
                    {
                        print_wounds(monster);

                        if ( mons_category(monster->type) == MC_MIMIC )
                          mimic_alert(monster);
                    }

                    pbolt->aim_down = 0;
                    return;
                }

                if ((pbolt->colour == LIGHTRED && mons_holiness(monster->type) == MH_UNDEAD) || (pbolt->colour == 16 && mons_holiness(monster->type) == MH_DEMONIC))
                {
                    if ( simple_monster_message(monster, " is enslaved.") )
                      pbolt->wand_id = 1;

                    monster->behavior = BEH_ENSLAVED;
                    return;
                }

                if ( pbolt->colour == LIGHTRED )
                  goto unaffected;

                //jmf: FIXME: ought to do a better "affected by sleep" check
                if ( pbolt->colour == BEAM_SLEEP    // I do not think this is right -- should that not be "flavour" and not "colour" {dlb}
                    && mons_holiness(monster->type) == MH_NATURAL )
                {
                    if ( simple_monster_message(monster, " looks drowsy...") )
                      pbolt->wand_id = 1;

                    monster->behavior = BEH_SLEEP;
                    return;
                }

                if ( pbolt->colour == BEAM_BACKLIGHT ) // I do not think this is right -- should that not be "flavour" and not "colour" {dlb}
                {
                    if ( backlight_monsters(monster->x, monster->y, pbolt->hit, 0) )
                      return;
                    else
                      goto unaffected;
                }

                mons_ench_f2(monster, pbolt);

                pbolt->aim_down = 0;
                return;

            }                   // end "if ..."

        }    // end "if ..." circa line 573

check_aimed:
        if ( pbolt->bx == pbolt->target_x && pbolt->by == pbolt->target_y
            && pbolt->aim_down == 1 )
          goto landed;

        if ( pbolt->bx == pbolt->target_x && pbolt->by == pbolt->target_y
            && ( pbolt->flavour == BEAM_EXPLOSION || pbolt->flavour == BEAM_HOLY ) )
        {
            explosion1(pbolt);
            pbolt->aim_down = 0;
            return;
        }

    }                           // end of for n

landed:
    // had if beam == tracer
    if ( pbolt->bx == pbolt->target_x && pbolt->by == pbolt->target_y
        && pbolt->aim_down == 1 )
    {
        pbolt->aim_down = 0;
        return;
    }

    if ( pbolt->flavour == BEAM_EXPLOSION )
    {
        explosion1(pbolt);
        pbolt->aim_down = 0;
        return;
    }


    if ( pbolt->beam_name[0] == '0' && pbolt->colour != BROWN && pbolt->colour != 200 )
      canned_msg(MSG_NOTHING_HAPPENS);

    pbolt->aim_down = 0;
/* ^ must have! */

}          // end beam();




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
void missile( struct bolt *pbolt, int throw_2 )
{

    int beam_sign_x = 0;
    int beam_sign_y = 0;
    float bmoy = 0;
    int bmax = 0;
    float bmay = 0;

    pbolt->bx = pbolt->source_x;
    pbolt->by = pbolt->source_y;

    int bmhit = pbolt->hit;

    //pbolt->tracer = 0;

    int hurted = 0;

    if (pbolt->move_x > 1 || pbolt->move_y > 1 || pbolt->move_x < -1 || pbolt->move_y < -1)
    {
        if (pbolt->move_x > 0)
          beam_sign_x = 1;

        if (pbolt->move_x < 0)
        {
            beam_sign_x = -1;
            pbolt->move_x *= -1;
        }

        if (pbolt->move_y > 0)
          beam_sign_y = 1;

        if (pbolt->move_y < 0)
        {
            beam_sign_y = -1;
            pbolt->move_y *= -1;
        }

        if (pbolt->move_x > pbolt->move_y)
        {
            if (beam_sign_x == 1)
              bmay = (float) (pbolt->move_y) / (pbolt->move_x + 1);

            if (beam_sign_x == -1)
              bmay = (float) (pbolt->move_y) / (pbolt->move_x + 1);

            bmax = 1;
        }
        else
        {
            if (beam_sign_y == 1)
              bmay = (float) (pbolt->move_x) / (pbolt->move_y + 1);

            if (beam_sign_y == -1)
              bmay = (float) (pbolt->move_x) / (pbolt->move_y + 1);

            bmax = 2;
        }

        bmoy = 1;
    }

    int n;

    for (n = 1; n < pbolt->range; n++)
    {
        if ( pbolt->beam_name[0] == '0' && pbolt->flavour != BEAM_MAGIC )
          return;

        pbolt->hit = bmhit;

        if ( bmax == 1 )
        {
            pbolt->move_x = beam_sign_x;
            bmoy += bmay;
            if ( bmoy >= 1 )
            {
                pbolt->move_y = beam_sign_y;
                bmoy--;
            }
            else
                pbolt->move_y = 0;
        }

        if (bmax == 2)
        {
            pbolt->move_y = beam_sign_y;
            bmoy += bmay;
            if (bmoy >= 1)
            {
                pbolt->move_x = beam_sign_x;
                bmoy--;
            }
            else
                pbolt->move_x = 0;
        }

        if (grd[pbolt->bx + pbolt->move_x][pbolt->by + pbolt->move_y] >= MINMOVE)
        {
            pbolt->bx += pbolt->move_x;
            pbolt->by += pbolt->move_y;

            if ( pbolt->bx == pbolt->target_x && pbolt->by == pbolt->target_y )
            {
                if ( pbolt->flavour == BEAM_EXPLOSION )
                {
                    if ( pbolt->thing_thrown == KILL_YOU_MISSILE )
                      pbolt->thing_thrown = KILL_YOU;
                    if ( pbolt->thing_thrown == KILL_MON_MISSILE )
                      pbolt->thing_thrown = KILL_MON;

                    explosion1(pbolt);
                    pbolt->aim_down = 0;
                    return;
                }

                if (strcmp(pbolt->beam_name, "orb of electricity") == 0 || strcmp(pbolt->beam_name, "metal orb") == 0 || strcmp(pbolt->beam_name, "great blast of cold") == 0)
                {
                    explosion1(pbolt);
                    pbolt->aim_down = 0;
                    return;
                }

                if ( pbolt->flavour == BEAM_ACID )
                {
                    pbolt->aim_down = 0;
                    return;
                }

            }

// hmmm ... I wonder if these need CLOUD_foo_MON checks, too: {dlb}
            if ( strcmp(pbolt->beam_name, "orb of energy") == 0 )
              place_cloud(CLOUD_PURP_SMOKE, pbolt->bx, pbolt->by, random2(5) + 1);

            if ( strcmp(pbolt->beam_name, "great blast of cold") == 0 )
              place_cloud(CLOUD_COLD, pbolt->bx, pbolt->by, random2(5) + 3);

            if ( strcmp(pbolt->beam_name, "ball of steam") == 0 )
              place_cloud(CLOUD_STEAM, pbolt->bx, pbolt->by, random2(5) + 1);

            if ( strcmp(pbolt->beam_name, "sticky flame") == 0 )
              place_cloud(CLOUD_BLACK_SMOKE, pbolt->bx, pbolt->by, random2(4) + 1);

            if ( strcmp(pbolt->beam_name, "poison gas") == 0 )
            {
                if ( pbolt->thing_thrown != KILL_MON )
                  place_cloud(CLOUD_POISON, pbolt->bx, pbolt->by, random2(4) + 2);
                else
                  place_cloud(CLOUD_POISON_MON, pbolt->bx, pbolt->by, random2(4) + 2);
            }

            if ( strcmp(pbolt->beam_name, "foul vapour") == 0 && pbolt->bx == pbolt->target_x && pbolt->by == pbolt->target_y )
            {
                big_cloud(( (pbolt->thing_thrown != KILL_MON) ? CLOUD_STINK : CLOUD_STINK_MON ), pbolt->bx, pbolt->by, 0, 9);
                return;
            }


            if (pbolt->colour != 200)  /* tracer doesn't update the screen */
                if (pbolt->bx > you.x_pos - 9 && pbolt->bx < you.x_pos + 9 && pbolt->by > you.y_pos - 9 && pbolt->by < you.y_pos + 9)
                {
#ifdef DOS_TERM
                    viewwindow(1, false);
#endif
                    textcolor(pbolt->colour);
                    gotoxy(pbolt->bx - you.x_pos + 18, pbolt->by - you.y_pos + 9);
                    putch(pbolt->type);
#ifdef USE_CURSES
                    // If we don't refresh curses we won't
                    // guarantee that the beam is visible
                    refresh();
#endif
                    delay(15);
                }

            if (pbolt->bx == you.x_pos && pbolt->by == you.y_pos)
            {
                /* I find it more logical that there can be virtually unavoidable missiles than missiles that can almost always be avoided. */

                if (pbolt->colour == 200)      /* tracer */
                {
                    if ( pbolt->trac_hit_mons == 1 || !you.invis )
                    {
                        pbolt->tracer = 1;
                        return;
                    }
                }
                else
                {
                    /* Start of : if it's not a tracer */
                    if (you.equip[EQ_SHIELD] != -1)
                      if (pbolt->move_x != 0 || pbolt->move_y != 0)
                        if ( coinflip() )
                          exercise(SK_SHIELDS, 1);

                    if (player_shield_class() > 0
                        && random2(pbolt->hit * 5 + 5 * you.shield_blocks)
                        <= random2(player_shield_class()) + (random2(you.dex) / 5) - 1)
                    {
                        you.shield_blocks++;
                        strcpy(info, "You block the ");
                        strcat(info, pbolt->beam_name);
                        strcat(info, ".");
                        mpr(info);

                        if (pbolt->flavour == BEAM_EXPLOSION)
                        {
                            explosion1(pbolt);
                            pbolt->aim_down = 0;
                            return;
                        }
                        if (strcmp(pbolt->beam_name, "orb of electricity") == 0 || strcmp(pbolt->beam_name, "metal orb") == 0 || strcmp(pbolt->beam_name, "great blast of cold") == 0)
                        {
                            explosion1(pbolt);
                            pbolt->aim_down = 0;
                            return;
                        }
                        break;
                    }           // end of block

                    if ( player_light_armour() && pbolt->move_x != 0
                        && pbolt->move_y != 0 && coinflip() )
                      {
                        exercise(SK_DODGING, 1);
                      }

                    if ( you.duration[DUR_REPEL_MISSILES] != 0
                          || you.mutation[MUT_REPULSION_FIELD] == 3 )
                      {
                        pbolt->hit = random2(pbolt->hit);
                      }

                    if ( pbolt->hit >= random2limit(player_evasion(), 40) + random2(you.dex) / 3 - 2
                        && you.duration[DUR_DEFLECT_MISSILES] == 0)
                      {
                        strcpy(info, "The ");
                        strcat(info, pbolt->beam_name);
                        strcat(info, " hits you");

/* ******************************************************************

                    if ( pbolt->thing_thrown == KILL_YOU_MISSILE || pbolt->thing_thrown == KILL_MON_MISSILE )
                    {
                        if (pbolt->bx > you.x_pos - 7 && pbolt->bx < you.x_pos + 7 && pbolt->by > you.y_pos - 7 && pbolt->by < you.y_pos + 7)
                        {
                            viewwindow(1);
                            textcolor(pbolt->colour);
                            gotoxy(pbolt->bx - you.x_pos + 18, pbolt->by - you.y_pos + 9);
                            putch(35);
                        }
                    }

****************************************************************** */

                        hurted = 0;
                        if ( pbolt->damage > 100 )
                        {
                            hurted += random2(pbolt->damage - 100);
                            hurted += random2(pbolt->damage - 100);
                            hurted += random2(pbolt->damage - 100);
                        }
                        else
                          hurted += random2(pbolt->damage);

                        strcat(info, "!");
                        mpr(info);

                        hurted = check_your_resists(hurted, pbolt->flavour);

                        if ( strstr(pbolt->beam_name, "poison") != NULL
                            && pbolt->flavour != BEAM_POISON
                            && !player_res_poison()
                            && random2(hurted) - random2(player_AC()) > 0 )
                        {
                            mpr("You are poisoned.");
                            you.poison += 1 + random2(3);
                        }

                        if ( pbolt->flavour == BEAM_LAVA
                              || ( pbolt->flavour == BEAM_FIRE && strcmp(pbolt->beam_name, "ball of steam") != 0 )
                              || stricmp(pbolt->beam_name, "hellfire") == 0 )
                        {
                            scrolls_burn(2, OBJ_SCROLLS);
                        }

                        if ( pbolt->flavour == BEAM_COLD )
                          scrolls_burn(2, OBJ_POTIONS);

                        hurted -= random2(1 + player_AC());

                        if ( hurted < 0 )
                          hurted = 0;

                        if ( you.equip[EQ_BODY_ARMOUR] != -1 )
                          if ( !player_light_armour() && one_chance_in(4)
                              && random2(1000) <= mass(OBJ_ARMOUR, you.inv_type[you.equip[EQ_BODY_ARMOUR]]) )
                            exercise(SK_ARMOUR, 1);

                        if ( YOU_KILL(pbolt->thing_thrown) )
                          ouch(hurted, 0, KILLED_BY_TARGETTING);
                        else
                          ouch(hurted, pbolt->beam_source, KILLED_BY_BEAM);

                        if (strcmp(pbolt->beam_name, "sticky flame") == 0 && (you.species != SP_MOTTLED_DRACONIAN || you.experience_level < 6))
                        {
                            if (you.equip[EQ_BODY_ARMOUR] == -1 || you.inv_type[you.equip[EQ_BODY_ARMOUR]] != 25)
                                you.duration[DUR_LIQUID_FLAMES] += random2avg(7,3) + 1;
                        }

                        break;
                    }
                    else
                    {
                        strcpy(info, "The ");
                        strcat(info, pbolt->beam_name);
                        strcat(info, " misses you!");
                        mpr(info);
                        if (pbolt->move_x == 0 && pbolt->move_y == 0)
                            break;
                    }


                }               /* end of if not tracer */



                if (pbolt->flavour == BEAM_EXPLOSION)
                {
                    explosion1(pbolt);
                    pbolt->aim_down = 0;
                    return;
                }
                if (strcmp(pbolt->beam_name, "orb of electricity") == 0 || strcmp(pbolt->beam_name, "metal orb") == 0 || strcmp(pbolt->beam_name, "great blast of cold") == 0)
                {
                    explosion1(pbolt);
                    pbolt->aim_down = 0;
                    return;
                }


            }

            char count_x = 0;
            struct monsters *monster = 0;    // not really the best place for the declaration, but "oh, well" {dlb}

            if ( mgrd[pbolt->bx][pbolt->by] != MNG )
            {
                count_x = 0;

                /* this won't display a "you miss it" if you throw an pbolt->aim_down thing at a lava worm, only if you hit. */
                int o = mgrd[pbolt->bx][pbolt->by];
                monster = &menv[o];

                if ( pbolt->colour == 200 )      /* tracer */
                {
                    if ( monster_habitat(monster->type) == DNGN_FLOOR || monster->number == 0 )
                    {
                        if ( pbolt->trac_hit_mons == 1 || monster->enchantment[2] != ENCH_INVIS )
                        {
                            if ( o == pbolt->trac_targ )
                              pbolt->tracer_mons = 1;
                            if ( monster->behavior == BEH_ENSLAVED )
                              pbolt->tracer = 2;
                            else
                              pbolt->tracer = 3;
                            return;
                        }
                    }
                }
                else
                    /* Start of : It's not a tracer */
                {

                    if ( pbolt->hit >= random2(monster->evasion) && (( monster_habitat(monster->type) == DNGN_FLOOR || monster->number == 0) || (pbolt->bx == pbolt->target_x && pbolt->by == pbolt->target_y && pbolt->aim_down == 1)))
                    {
                        hurted = 0;

                        if (pbolt->damage > 100)
                        {
                            hurted += random2(pbolt->damage - 100);
                            hurted += random2(pbolt->damage - 100);
                            hurted += random2(pbolt->damage - 100);
                        }
                        else
                            hurted += random2(pbolt->damage);

                        if ( mons_near(monster) && ( monster->enchantment[2] != ENCH_INVIS || player_see_invis() ) )
                        {
                            strcpy(info, "The ");
                            strcat(info, pbolt->beam_name);
                            strcat(info, " hits ");
                            strcat(info, monam(monster->number, monster->type, monster->enchantment[2], 1));
                            strcat(info, ".");
                            mpr(info);
                        }

                        hurted = check_mons_resists(monster, pbolt, hurted);

                        hurt_monster(monster, hurted);

                        if (YOU_KILL(pbolt->thing_thrown) && monster->behavior == BEH_ENSLAVED && hurted > 0)
                        {
                            //naughty(NAUGHTY_STABBING, 5);
                            monster->behavior = BEH_CHASING_I;
                        }

                        if ( YOU_KILL(pbolt->thing_thrown) && mons_near(monster) )
                          print_wounds(monster);
                        if ( mons_category(monster->type) == MC_MIMIC )
                          mimic_alert(monster);

                        if ( monster->behavior == BEH_SLEEP )
                          monster->behavior = BEH_CHASING_I;


/*              if (pbolt->thing_thrown == KILL_YOU_MISSILE || pbolt->thing_thrown == KILL_MON_MISSILE)
   {
   if (pbolt->bx > you.x_pos - 7 && pbolt->bx < you.x_pos + 7 && pbolt->by > you.y_pos - 7 && pbolt->by < you.y_pos + 7)
   {
   viewwindow(1);
   textcolor(pbolt->colour);
   gotoxy(pbolt->bx - you.x_pos + 18, pbolt->by - you.y_pos + 9);
   putch(35);
   }
   } */
                        if ( monster->hit_points < 1 )
                        {
                            switch (pbolt->thing_thrown)
                            {
                              case KILL_YOU:      /* You threw */
                              case KILL_YOU_MISSILE:
                                monster_die(monster, KILL_YOU_MISSILE, pbolt->beam_source);
                                break;  /* You cast a missile spell which doesn't leave an item. */
                              case KILL_MON:      /* mons threw */
                              case KILL_MON_MISSILE:
                                monster_die(monster, KILL_MON_MISSILE, pbolt->beam_source);
                                break;  /* mons cast a missile spell which doesn't leave an item. */
                            }
                        }
                        else
                        {
                            if ( strcmp(pbolt->beam_name, "sticky flame") == 0 )
                              sticky_flame_monster(o, false, hurted);

                            /* looks for missiles which aren't poison but are poison*ed* */
                            if ( strstr(pbolt->beam_name, "poison") != NULL
                                && pbolt->flavour != BEAM_POISON
                                && random2(hurted) - random2(monster->armor_class) > 0)
                              poison_monster(monster, YOU_KILL(pbolt->thing_thrown));
                        }
                        count_x = 1;

                        if (pbolt->flavour == BEAM_EXPLOSION)
                        {
                            explosion1(pbolt);
                            pbolt->aim_down = 0;
                            return;
                        }

                        if (strcmp(pbolt->beam_name, "orb of electricity") == 0 || strcmp(pbolt->beam_name, "metal orb") == 0 || strcmp(pbolt->beam_name, "great blast of cold") == 0)
                        {
                            explosion1(pbolt);
                            pbolt->aim_down = 0;
                            return;
                        }

                        break;
                    }
                    else if (YOU_KILL(pbolt->thing_thrown) && mons_near(monster) && ( monster_habitat(monster->type) == DNGN_FLOOR || monster->number == 0))  // No message if monster missile misses
                    {
                        strcpy(info, "The ");
                        strcat(info, pbolt->beam_name);
                        strcat(info, " misses ");
                        strcat(info, monam(monster->number, monster->type, monster->enchantment[2], 1));
                        strcat(info, ".");
                        mpr(info);

                        if ( ( !you.invis || mons_see_invis(monster->type) ) && monster->behavior == BEH_SLEEP )
                          monster->behavior = BEH_CHASING_I;

                    }

                }               /* end of if not a tracer */

            }                   // end of if (mgrid ... != MNG)


            if (count_x == 1)
            {
                count_x = 0;
                break;
            }

        }                       // end of if.

        if (pbolt->bx == pbolt->target_x && pbolt->by == pbolt->target_y && pbolt->aim_down == 1)
          goto landed;

    }                           // end of for n loop

landed:
    if (pbolt->flavour == BEAM_EXPLOSION)
    {
        explosion1(pbolt);
        pbolt->aim_down = 0;
        return;
    }

    if ( pbolt->colour == 200 )
      return;

    if (strcmp(pbolt->beam_name, "orb of electricity") == 0 || strcmp(pbolt->beam_name, "metal orb") == 0 || strcmp(pbolt->beam_name, "great blast of cold") == 0)
    {
        explosion1(pbolt);
        pbolt->aim_down = 0;
        return;
    }


    if (pbolt->thing_thrown == KILL_YOU)       // ie if you threw it.

    {
        if (grd[pbolt->bx][pbolt->by] != DNGN_LAVA && grd[pbolt->bx][pbolt->by] != DNGN_DEEP_WATER)
        {
            if ( you.inv_class[throw_2] != 1
                  || !one_chance_in( (you.inv_type[throw_2] == MI_STONE) ? 3 : 2) )
            {
                item_place(throw_2, pbolt->bx, pbolt->by, 1);
            }
        }

        if ( throw_2 == you.equip[EQ_WEAPON] )
        {
            you.equip[EQ_WEAPON] = -1;
            mpr("You are empty handed.");
        }
    }


    if (pbolt->thing_thrown == KILL_MON && (grd[pbolt->bx][pbolt->by] != DNGN_LAVA && grd[pbolt->bx][pbolt->by] != DNGN_DEEP_WATER) && coinflip())         // monster threw it.
    {
        if (mitm.quantity[throw_2] == 0)        // 0 instead of 1 because the mitm.quantity is reduced by 1 when it is thrown.
        {
        }
        else
        {
            if (igrd[pbolt->bx][pbolt->by] != ING)
            {
                if ((mitm.base_type[throw_2] == OBJ_MISSILES
                     || mitm.base_type[throw_2] == OBJ_FOOD
                     || mitm.base_type[throw_2] == OBJ_SCROLLS
                     || mitm.base_type[throw_2] == OBJ_POTIONS
                     || mitm.base_type[throw_2] == OBJ_UNKNOWN_II)
                    && mitm.base_type[throw_2] == mitm.base_type[igrd[pbolt->bx][pbolt->by]]
                    && mitm.sub_type[throw_2] == mitm.sub_type[igrd[pbolt->bx][pbolt->by]]
                    && mitm.pluses[throw_2] == mitm.pluses[igrd[pbolt->bx][pbolt->by]]
                    && mitm.pluses2[throw_2] == mitm.pluses2[igrd[pbolt->bx][pbolt->by]]
                    && mitm.special[throw_2] == mitm.special[igrd[pbolt->bx][pbolt->by]])
                {
                    mitm.quantity[igrd[pbolt->bx][pbolt->by]]++;
                    pbolt->aim_down = 0;
                    return;
                }
            }                   // end of if igrd != ING

            int o = 0;    // bad bad bad (!!!) using same variable name in the same function -- ick!!! {dlb}

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
                    mitm.link[o] = ING;
                    break;
                }
            }                   // end of o loop

            if (igrd[pbolt->bx][pbolt->by] == ING)
            {
                igrd[pbolt->bx][pbolt->by] = o;
            }
            else
            {
                int hug = igrd[pbolt->bx][pbolt->by];

                igrd[pbolt->bx][pbolt->by] = o;
                mitm.link[o] = hug;
            }

        }                       // end of else

    }                           // if (thing_throw == 2) ...

    pbolt->aim_down = 0;
/* ^ must have this before every return in this function! */

}          // end missile()




// returns damage taken by a monster from a "flavoured" (fire, ice, etc.)
// attack -- damage from clouds and branded weapons handled elsewhere.
int check_mons_resists( struct monsters *monster, struct bolt *pbolt, int hurted )
{

    switch ( pbolt->flavour )
    {
    case BEAM_FIRE:
        if (mons_res_fire(monster->type) > 0)
        {
            simple_monster_message(monster, " appears unharmed.");
            hurted = 0;
        }
        else if (monster->inv[MSLOT_ARMOUR] != ING && mitm.special[monster->inv[MSLOT_ARMOUR]] % 30 == SPARM_FIRE_RESISTANCE)
        {
            simple_monster_message(monster, " resists.");
            hurted /= 3;
        }

        if (mons_res_fire(monster->type) == -1 && (monster->inv[MSLOT_ARMOUR] == ING || mitm.special[monster->inv[MSLOT_ARMOUR]] % 30 != SPARM_FIRE_RESISTANCE))
        {
            simple_monster_message(monster, " is burned terribly!");
            hurted *= 15;
            hurted /= 10;
        }
        break;


    case BEAM_COLD:
        if (mons_res_cold(monster->type) > 0)
        {
            simple_monster_message(monster, " appears unharmed.");
            hurted = 0;
        }
        else if (monster->inv[MSLOT_ARMOUR] != ING && mitm.special[monster->inv[MSLOT_ARMOUR]] % 30 == SPARM_COLD_RESISTANCE)
        {
            simple_monster_message(monster, " resists.");
            hurted /= 3;
        }

        if (mons_res_cold(monster->type) == -1 && (monster->inv[MSLOT_ARMOUR] == ING || mitm.special[monster->inv[MSLOT_ARMOUR]] % 30 != SPARM_COLD_RESISTANCE))
        {
            simple_monster_message(monster, " is frozen!");
            hurted *= 15;
            hurted /= 10;
        }
        break;

    case BEAM_ELECTRICITY:
        if (mons_res_elec(monster->type) > 0)
        {
            simple_monster_message(monster, " appears unharmed.");
            hurted = 0;
        }
        break;


    case BEAM_POISON:
        if (!one_chance_in(3))
          poison_monster(monster, YOU_KILL(pbolt->thing_thrown));

        if ( mons_res_poison(monster->type) > 0 )
        {
            simple_monster_message(monster, " appears unharmed.");
            hurted = 0;
        }
        break;

    case BEAM_NEG:
        if ( mons_holiness(monster->type) == MH_UNDEAD || mons_holiness(monster->type) == MH_DEMONIC )
        {
            simple_monster_message(monster, " appears unharmed.");
            hurted = 0;
        }
        else
        {
            simple_monster_message(monster, " is drained.");

            if ( one_chance_in(5) )
              monster->hit_dice--;

            monster->max_hit_points -= 2 + random2(3);
            monster->hit_points -= 2 + random2(3);

            if ( monster->hit_points >= monster->max_hit_points )
              monster->hit_points = monster->max_hit_points;

            if ( monster->hit_dice < 1 )
              monster->hit_points = 0;
        }                       // end else

        break;


    case BEAM_HOLY:             // flame of cleansing
        if ( mons_holiness(monster->type) == MH_NATURAL || mons_holiness(monster->type) == MH_HOLY )
        {
            simple_monster_message(monster, " appears unharmed.");
            hurted = 0;
        }
        break;

    case BEAM_ICE:              /* ice - about 50% of damage is cold, other 50% is impact and can't be resisted (except by AC, of course) */
        if (mons_res_cold(monster->type) > 0)
        {
            simple_monster_message(monster, " partially resists.");
            hurted /= 2;
        }
        else if (monster->inv[MSLOT_ARMOUR] != ING && mitm.special[monster->inv[MSLOT_ARMOUR]] % 30 == SPARM_COLD_RESISTANCE)
        {
            simple_monster_message(monster, " partially resists.");
            hurted /= 2;
        }

        if (mons_res_cold(monster->type) == -1 && (monster->inv[MSLOT_ARMOUR] == ING || mitm.special[monster->inv[MSLOT_ARMOUR]] % 30 != SPARM_COLD_RESISTANCE))
        {
            simple_monster_message(monster, " is frozen!");
            hurted *= 13;
            hurted /= 10;
        }
        break;

    }                           /* end of switch */


    if ( pbolt->flavour == BEAM_LAVA ) //jmf: lava != hellfire
      {
        if (mons_res_fire(monster->type) > 0 || (monster->inv[MSLOT_ARMOUR] != ING && mitm.special[monster->inv[MSLOT_ARMOUR]] % 30 == SPARM_FIRE_RESISTANCE))
        {
            simple_monster_message(monster, " partially resists.");
            hurted /= 2;
        }
        if (mons_res_fire(monster->type) == -1 && (monster->inv[MSLOT_ARMOUR] == ING || mitm.special[monster->inv[MSLOT_ARMOUR]] % 30 != SPARM_FIRE_RESISTANCE))
        {
            simple_monster_message(monster, " is burned terribly!");
            hurted *= 12;
            hurted /= 10;
        }
      }
    else if ( stricmp(pbolt->beam_name, "hellfire") == 0 )
    {
        if (mons_res_fire(monster->type) == 2)
        {
            simple_monster_message(monster, " appears unharmed.");
            hurted = 0;
        }
        if (mons_res_fire(monster->type) == 1 || (monster->inv[MSLOT_ARMOUR] != ING && mitm.special[monster->inv[MSLOT_ARMOUR]] % 30 == SPARM_FIRE_RESISTANCE))
        {
            simple_monster_message(monster, " partially resists.");
            hurted /= 2;
        }
        if (mons_res_fire(monster->type) == -1 && (monster->inv[MSLOT_ARMOUR] == ING || mitm.special[monster->inv[MSLOT_ARMOUR]] % 30 != SPARM_FIRE_RESISTANCE))
        {
            simple_monster_message(monster, " is burned terribly!");
            hurted *= 12;       /* hellfire */
            hurted /= 10;
        }
    }

    return hurted;

}          // end check_mons_resists()




// these return values seem "backward" to me {dlb}:

// Returns 0 if the monster made its save against hostile
// enchantments/some other magics.
bool check_mons_magres( struct monsters *monster, int pow )
{

    int mrs = mon_resist_mag(monster->type, monster->hit_dice);

    if ( mrs == 5000 )
      return false;

    if ( pow > 40 )                     // nested if's rather than stacked 'em
    {                                   // uglier than before but slightly
        pow = ((pow - 40) / 2) + 40;    // more efficient 16jan2000 {dlb}

        if ( pow > 70 )
        {
            pow = ((pow - 70) / 2) + 70;

            if ( pow > 90 )
            {
                pow = ((pow - 90) / 2) + 90;

                if ( pow > 120 )
                  pow = 120;
            }
        }
    }

    int mrchance = (100 + mrs) - pow;

    int mrch2 = random2avg(200,2);

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

    return ( (mrch2 < mrchance) ? false : true );

}          // end check_mons_magres()




// Enchants all monsters in player's sight.
void mass_enchantment( int wh_enchant, int pow )
{

    int i = 0;           // loop variable {dlb}
    int p;               // loop variable {dlb}
    bool brek = false;
    struct monsters *monster = 0;    // NULL {dlb}

    viewwindow(0, false);

    for (i = 0; i < MNST; i++)
    {
        monster = &menv[i];

        if ( monster->type == -1 || !mons_near(monster) )
          continue;

// assuming that the only mass charm is control undead:
        if ( wh_enchant == ENCH_CHARM
            && ( mons_holiness(monster->type) != MH_UNDEAD || monster->behavior == BEH_ENSLAVED ) )
          continue;

        if ( wh_enchant != ENCH_CHARM || mons_holiness(monster->type) != MH_UNDEAD )
            if ( !check_mons_magres(monster, pow) )
            {
                simple_monster_message(monster, " resists.");
                continue;
            }

        if ( mons_near(monster) )
        {
            if ( monster->enchantment1 == 1 )
              for (p = 0; p < 3; p++)
              {
                  if ( monster->enchantment[p] == wh_enchant )
                    brek = true;
              }

            for (p = 0; p < 3; p++)
            {
                if ( brek )
                {
                    brek = false;
                    break;
                }

                if ( monster->enchantment[p] == ENCH_NONE )
                {
                    monster->enchantment[p] = wh_enchant;
                    monster->enchantment1 = 1;

                    if ( monster->enchantment[2] != ENCH_INVIS || player_see_invis() )
                    {
                        switch ( wh_enchant )
                        {
                          case ENCH_FEAR:
                            simple_monster_message(monster, " looks frightened.");
                            break;
                          case ENCH_CONFUSION:
                            simple_monster_message(monster, " looks rather confused.");
                            break;
                          case ENCH_CHARM:
                            simple_monster_message(monster, " submits to your will.");
                            monster->behavior = BEH_ENSLAVED;
                            break;
                        }

                        break;    // I'm totally confused on this one {dlb}
                    }
                    else
                      canned_msg(MSG_NOTHING_HAPPENS);    // I do not like this at all -- gives away # of invisble monsters in area of effect {dlb}
                }
            }

        }        // end "if mons_near(monster)"

    }        // end "for i"

}          // end mass_enchantmenet()




/*
   Monster has probably failed save, now it gets enchanted somehow.
 */
bool mons_ench_f2( struct monsters *monster, struct bolt *pbolt )
{

    bool is_near = mons_near(monster);    // single caluclation permissible {dlb}
    bool brek = false;
    char p;              // loop variable

    switch ( pbolt->colour )     /* put in magic resistance */
    {
      case BLACK:         /* 0 = slow monster */
        for (p = 0; p < 3; p++)
        {
            if ( monster->enchantment[p] == ENCH_SLOW )
              goto nothinghap;

            if (monster->enchantment[p] == ENCH_HASTE)
            {
                monster->enchantment[p] = ENCH_NONE;

                if ( monster->speed >= 100 )
                  monster->speed = 100 + ((monster->speed - 100) / 2);
                else
                  monster->speed /= 2;

                if ( monster->enchantment[0] == ENCH_NONE && monster->enchantment[1] == ENCH_NONE && monster->enchantment[2] == ENCH_NONE)
                  monster->enchantment1 = 0;

                brek = true;
                break;
            }
        }                       /* end of for p */

        if ( !brek )
          for (p = 0; p < 3; p++)
          {
              if (monster->enchantment[p] == ENCH_NONE)        /* || p == 2) replaces 3rd enchantment if all full. */
              {
                  monster->enchantment[p] = ENCH_SLOW;

                  if ( monster->speed >= 100 )
                    monster->speed = 100 + ((monster->speed - 100) / 2);
                  else
                    monster->speed /= 2;

                  monster->enchantment1 = 1;
                  break;
              }
          }


        // put in an exception for fungi, plants and other things you won't
        // notice slow down.
        if ( simple_monster_message(monster, " seems to slow down.") )
          pbolt->wand_id = 1;

        return true;

    case BLUE:                  // 1 = haste
        for (p = 0; p < 3; p++)
        {
            if ( monster->enchantment[p] == ENCH_HASTE )
              goto nothinghap;

            if (monster->enchantment[p] == ENCH_SLOW)
            {
                monster->enchantment[p] = ENCH_NONE;

                if (monster->speed >= 100)
                  monster->speed = 100 + ((monster->speed - 100) << 1);
                else
                  monster->speed <<= 1;

                if ( monster->enchantment[0] == ENCH_NONE
                    && monster->enchantment[1] == ENCH_NONE
                    && monster->enchantment[2] == ENCH_NONE )
                  monster->enchantment1 = 0;

                brek = true;
                break;
            }
        }                       /* end of for p */

        if ( !brek )
          for (p = 0; p < 3; p++)
          {
              if (monster->enchantment[p] == ENCH_NONE)
              {
                  monster->enchantment[p] = ENCH_HASTE;

                  if (monster->speed >= 100)
                    monster->speed = 100 + ((monster->speed - 100) << 1);
                  else
                    monster->speed <<= 1;

                  monster->enchantment1 = 1;
                  break;
              }
          }

        if ( p == 2 && monster->enchantment[p] != ENCH_NONE )
          goto nothinghap;

        // put in an exception for fungi, plants and other things you won't
        // notice speed up.
        if ( simple_monster_message(monster, " seems to speed up.") )
          pbolt->wand_id = 1;

        return true;


    case GREEN:         /* 2 = healing */
        if ( heal_monster(monster, 5 + random2(15), false) )
        {
            if ( monster->hit_points == monster->max_hit_points )
            {
                if ( simple_monster_message(monster, "'s wounds heal themselves!") )
                  pbolt->wand_id = 1;
            }
            else
            {
                if ( simple_monster_message(monster, " is healed somewhat.") )
                  pbolt->wand_id = 1;
            }
        }
        else
        {
            if ( !simple_monster_message(monster, " is unaffected.") )
              canned_msg(MSG_NOTHING_HAPPENS);
        }
        return true;

    case CYAN:                  /* 3 = paralysis */
        monster->speed_increment = 0;

        if ( simple_monster_message(monster, " suddenly stops moving!") )
          pbolt->wand_id = 1;

        if ( grd[monster->x][monster->y] == DNGN_LAVA_X
              || grd[monster->x][monster->y] == DNGN_WATER_X )
        {
            if ( mons_flies(monster->type) == 1 )
            {
                // don't worry about invisibility - you should be able to
                // see if something has fallen into the lava
                if ( is_near )
                {
                    strcpy(info, monam(monster->number, monster->type, monster->enchantment[2], 0));
                    strcat(info, " falls into the ");
                    strcat(info, (grd[monster->x][monster->y] == DNGN_WATER_X) ? "water" : "lava");
                    strcat(info, "!");
                    mpr(info);
                }

                switch ( pbolt->thing_thrown )
                {
                  case KILL_YOU:
                  case KILL_YOU_MISSILE:
                    monster_die(monster, KILL_YOU, pbolt->beam_source);
                    break;      /*  "    " */
                  case KILL_MON:
                    monster_die(monster, KILL_MON_MISSILE, pbolt->beam_source);
                    break;      /* dragon breath &c */
                }
            }
        }
        return true;

    case RED:                   /* 4 = confusion */
        for (p = 0; p < 3; p++)
          if ( monster->enchantment[p] == ENCH_CONFUSION )
            return true;

    // replaces 3rd enchantment if all full:
        for (p = 0; p < 3; p++)
          if ( monster->enchantment[p] == ENCH_NONE || p == 2 )
          {
              monster->enchantment[p] = ENCH_CONFUSION;
              monster->enchantment1 = 1;
              break;
          }

        // put in an exception for fungi, plants and other things you won't
        // notice becoming confused.
        if ( simple_monster_message(monster, " appears confused.") )
          pbolt->wand_id = 1;

        return true;

    case MAGENTA:               /* 5 = invisibility */
        if ( monster->enchantment[2] == ENCH_INVIS || !is_near )
          goto nothinghap;

        if ( monster->enchantment[2] != ENCH_NONE )
        {
            if ( is_near )
            {
                strcpy(info, monam(monster->number, monster->type, monster->enchantment[2], 0));
                strcat(info, " flickers for a moment.");
                mpr(info);
                pbolt->wand_id = 1;
            }
            return true;
        }

        if ( is_near )
        {
            strcpy(info, monam(monster->number, monster->type, monster->enchantment[2], 0));
            strcat(info, " flickers and vanishes!");
            mpr(info);
            pbolt->wand_id = 1;
        }

        monster->enchantment[2] = ENCH_INVIS;
        monster->enchantment1 = 1;
        return true;

        /* 6 is used by digging
           7            teleport
           8            polymorph */

    case LIGHTBLUE:             /* 9 = charm */
        for (p = 0; p < 3; p++)
          if ( monster->enchantment[p] == ENCH_CHARM )
            return true;

        for (p = 0; p < 4; p++)
        {
            if ( p == 3 )
              return true;

            if ( monster->enchantment[p] == ENCH_NONE )    /* replaces 3rd enchantment if all full. */
            {
                monster->enchantment[p] = ENCH_CHARM;
                monster->enchantment1 = 1;
                monster->behavior = BEH_ENSLAVED;
                break;
            }
        }

        // put in an exception for fungi, plants and other things you won't
        // notice becoming charmed.
        if ( simple_monster_message(monster, " is charmed.") )
          pbolt->wand_id = 1;

        return true;

    }                           /* end of switch (beam_colour) */

    goto somethinghap;

nothinghap:
    if ( is_near )
      canned_msg(MSG_NOTHING_HAPPENS);

somethinghap:
    return true;

}          // end mons_ench_f2()




// puts the poison value into a monster's enchantment variable.
void poison_monster( struct monsters *monster, bool fromPlayer )
{

    int p;
    bool brek = false;

    if ( monster->type == -1 )
      return;

    if ( mons_res_poison(monster->type) > 0 )
      return;

    if ( monster->inv[MSLOT_ARMOUR] != ING && mitm.special[monster->inv[MSLOT_ARMOUR]] % 30 == SPARM_POISON_RESISTANCE )
      return;

    for (p = 0; p < 3; p++)
    {
        if ( monster->enchantment[p] % 50 == 10 )
          return;

        if ( monster->enchantment[p] % 50 >= 7 && monster->enchantment[p] % 50 < 10 )
        {
            (monster->enchantment[p])++;
            brek = true;
            simple_monster_message(monster, " looks rather more sickly.");
            break;
        }
    }

    if ( !brek )
      for (p = 0; p < 3; p++)
        if ( monster->enchantment[p] == ENCH_NONE )
        {
            monster->enchantment[p] = ENCH_YOUR_POISON_I;
            monster->enchantment1 = 1;
            simple_monster_message(monster, " looks rather ill.");
            break;
        }

    if ( !fromPlayer
        && monster->enchantment[p] <= ENCH_YOUR_POISON_IV
        && monster->enchantment[p] >= ENCH_YOUR_POISON_I )
    {
        monster->enchantment[p] += 50;
    }
    else
    {
      naughty(NAUGHTY_POISON, 5 + random2(3));     //jmf: TSO now hates poison
      done_good(GOOD_POISON, 5);                   //jmf: had test god who liked poison
    }

}          // end poison_monster()




// similar to poison_monster() -- makes the monster burn if hit by napalm
void sticky_flame_monster( int mn, bool source, int power )
{

    int long_last = 0;
    bool brek = false;
    int p;

    struct monsters *monster = &menv[mn];

    if ( monster->type == -1 )
      return;

    if ( mons_res_fire(monster->type) > 0 )
      return;

    if ( monster->inv[MSLOT_ARMOUR] != ING && mitm.special[monster->inv[MSLOT_ARMOUR]] % 30 == SPARM_FIRE_RESISTANCE )
      return;

    long_last = 1 + (random2(power) / 2);

    if ( long_last > 4 )
      long_last = 4;

    for (p = 0; p < 3; p++)
    {
        if ( monster->enchantment[p] == ENCH_YOUR_STICKY_FLAME_IV || monster->enchantment[p] == ENCH_STICKY_FLAME_IV )
        {
            // already covered in sticky flame - ouch!
            return;
        }
        if ( ( monster->enchantment[p] >= ENCH_YOUR_STICKY_FLAME_I && monster->enchantment[p] < ENCH_YOUR_STICKY_FLAME_IV )
                || ( monster->enchantment[p] >= ENCH_STICKY_FLAME_I && monster->enchantment[p] < ENCH_STICKY_FLAME_IV ) )
        {
            monster->enchantment[p] += long_last;
            if (monster->enchantment[p] > ENCH_YOUR_STICKY_FLAME_IV && monster->enchantment[p] < ENCH_POISON_IV)
              monster->enchantment[p] = ENCH_YOUR_STICKY_FLAME_IV;

            if (monster->enchantment[p] > ENCH_STICKY_FLAME_IV)
              monster->enchantment[p] = ENCH_STICKY_FLAME_IV;
            brek = true;
            simple_monster_message(monster, " is covered in liquid fire!");
            break;
        }
    }                           /* end of for p */

    if ( !brek )
      for (p = 0; p < 3; p++)
        if ( monster->enchantment[p] == ENCH_NONE )
        {
            monster->enchantment[p] = ENCH_CHARM + long_last;    // should set long_last in a way that this referenced ENCH_YOUR_STICKY_FLAME_I and not ENCH_CHARM {dlb}
            monster->enchantment1 = 1;
            simple_monster_message(monster, " is covered in liquid fire!");
            break;
        }

    if ( source
        && monster->enchantment[p] <= ENCH_POISON_IV
        && monster->enchantment[p] >= ENCH_YOUR_STICKY_FLAME_I )
    {
        monster->enchantment[p] += 30;
    }

}          // end sticky_flame_monster()




//   Places a cloud with the given stats. May delete old clouds to make way
//   if there are too many (CLOUDS == 30) on level. Will overwrite an old
//   cloud under some circumstances.
void place_cloud( unsigned char cl_type, unsigned char ctarget_x,unsigned char ctarget_y, unsigned char cl_range )
{

    char c_deleted = (env.cloud_no >= CLOUDS) ? random2(CLOUDS) : 100;
    char target_cgrid = env.cgrid[ctarget_x][ctarget_y];    // more compact {dlb}

    if ( target_cgrid != CNG )    // that is, another cloud already there {dlb}
    {
      if ( ( env.cloud_type[target_cgrid] >= CLOUD_GREY_SMOKE
            && env.cloud_type[target_cgrid] <= CLOUD_STEAM )
           || env.cloud_type[target_cgrid] == CLOUD_STINK
           || env.cloud_type[target_cgrid] == CLOUD_BLACK_SMOKE
           || env.cloud_decay[target_cgrid] <= 20 )    //soon gone
        {
            env.cloud_type[env.cgrid[ctarget_x][ctarget_y]] = CLOUD_NONE;
            env.cgrid[ctarget_x][ctarget_y] = CNG;
            env.cloud_decay[env.cgrid[ctarget_x][ctarget_y]] = 0;
            env.cloud_no--;
        }
        else
          return;
    }

    for (unsigned char ci = 0; ci < CLOUDS; ci++)
    {
        if (ci == c_deleted)
        {
            env.cloud_type[ci] = CLOUD_NONE;
            env.cgrid[env.cloud_x[ci]][env.cloud_y[ci]] = CNG;
            env.cloud_decay[ci] = 0;
            env.cloud_no--;
        }

        if (env.cloud_type[ci] == CLOUD_NONE)   // ie is empty
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

}          // end place_cloud();




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
void tracer_f( struct monsters *monster, struct bolt *pbolt )
{

    strcpy(pbolt->beam_name, "0tracer");
    pbolt->trac_hit_mons = mons_see_invis(monster->type);
    pbolt->tracer = 0;
    pbolt->tracer_mons = 0;
    pbolt->trac_targ = monster->monster_foe;
    pbolt->colour = 200;
    pbolt->beam_name[0] = '0';
    pbolt->type = 0;
    pbolt->flavour = BEAM_MAGIC;
    pbolt->damage = 0;
    pbolt->range = 10;
    pbolt->move_x = pbolt->trac_targ_x - monster->x;
    pbolt->move_y = pbolt->trac_targ_y - monster->y;
    pbolt->source_x = monster->x;
    pbolt->source_y = monster->y;
    pbolt->aim_down = 0;
    pbolt->beam_source = monster_index(monster);
    missile(pbolt, 0);

}          // end tracer_f()




/*
   When a mimic is hit by a ranged attack, it teleports away (the slow way)
   and changes its appearance - the appearance change is in monster_teleport
   in mstuff2.
 */
void mimic_alert( struct monsters *mimic )
{

    for (unsigned char p = 0; p < 3; p++)
      if ( mimic->enchantment[p] >= ENCH_TP_I && mimic->enchantment[p] <= ENCH_TP_IV )
        return;

    monster_teleport(mimic, false);

}          // end mimic_alert()
